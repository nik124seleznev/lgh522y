/*
 * linux/ipc/sem.c
 * Copyright (C) 1992 Krishna Balasubramanian
 * Copyright (C) 1995 Eric Schenk, Bruno Haible
 *
 * /proc/sysvipc/sem support (c) 1999 Dragos Acostachioaie <dragos@iname.com>
 *
 * SMP-threaded, sysctl's added
 * (c) 1999 Manfred Spraul <manfred@colorfullife.com>
 * Enforced range limit on SEM_UNDO
 * (c) 2001 Red Hat Inc
 * Lockless wakeup
 * (c) 2003 Manfred Spraul <manfred@colorfullife.com>
 * Further wakeup optimizations, documentation
 * (c) 2010 Manfred Spraul <manfred@colorfullife.com>
 *
 * support for audit of ipc object properties and permission changes
 * Dustin Kirkland <dustin.kirkland@us.ibm.com>
 *
 * namespaces support
 * OpenVZ, SWsoft Inc.
 * Pavel Emelianov <xemul@openvz.org>
 *
 * Implementation notes: (May 2010)
 * This file implements System V semaphores.
 *
 * User space visible behavior:
 * - FIFO ordering for semop() operations (just FIFO, not starvation
 *   protection)
 * - multiple semaphore operations that alter the same semaphore in
 *   one semop() are handled.
 * - sem_ctime (time of last semctl()) is updated in the IPC_SET, SETVAL and
 *   SETALL calls.
 * - two Linux specific semctl() commands: SEM_STAT, SEM_INFO.
 * - undo adjustments at process exit are limited to 0..SEMVMX.
 * - namespace are supported.
 * - SEMMSL, SEMMNS, SEMOPM and SEMMNI can be configured at runtine by writing
 *   to /proc/sys/kernel/sem.
 * - statistics about the usage are reported in /proc/sysvipc/sem.
 *
 * Internals:
 * - scalability:
 *   - all global variables are read-mostly.
 *   - semop() calls and semctl(RMID) are synchronized by RCU.
 *   - most operations do write operations (actually: spin_lock calls) to
 *     the per-semaphore array structure.
 *   Thus: Perfect SMP scaling between independent semaphore arrays.
 *         If multiple semaphores in one array are used, then cache line
 *         trashing on the semaphore array spinlock will limit the scaling.
 * - semncnt and semzcnt are calculated on demand in count_semncnt() and
 *   count_semzcnt()
 * - the task that performs a successful semop() scans the list of all
 *   sleeping tasks and completes any pending operations that can be fulfilled.
 *   Semaphores are actively given to waiting tasks (necessary for FIFO).
 *   (see update_queue())
 * - To improve the scalability, the actual wake-up calls are performed after
 *   dropping all locks. (see wake_up_sem_queue_prepare(),
 *   wake_up_sem_queue_do())
 * - All work is done by the waker, the woken up task does not have to do
 *   anything - not even acquiring a lock or dropping a refcount.
 * - A woken up task may not even touch the semaphore array anymore, it may
 *   have been destroyed already by a semctl(RMID).
 * - The synchronizations between wake-ups due to a timeout/signal and a
 *   wake-up due to a completed semaphore operation is achieved by using an
 *   intermediate state (IN_WAKEUP).
 * - UNDO values are stored in an array (one per process and per
 *   semaphore array, lazily allocated). For backwards compatibility, multiple
 *   modes for the UNDO variables are supported (per process, per thread)
 *   (see copy_semundo, CLONE_SYSVSEM)
 * - There are two lists of the pending operations: a per-array list
 *   and per-semaphore list (stored in the array). This allows to achieve FIFO
 *   ordering without always scanning all pending operations.
 *   The worst-case behavior is nevertheless O(N^2) for N wakeups.
 */

#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/time.h>
#include <linux/security.h>
#include <linux/syscalls.h>
#include <linux/audit.h>
#include <linux/capability.h>
#include <linux/seq_file.h>
#include <linux/rwsem.h>
#include <linux/nsproxy.h>
#include <linux/ipc_namespace.h>

#include <asm/uaccess.h>
#include "util.h"

/*                                                           */
struct sem {
	int	semval;		/*               */
	int	sempid;		/*                       */
	spinlock_t	lock;	/*                                      */
	struct list_head pending_alter; /*                               */
					/*                          */
	struct list_head pending_const; /*                               */
					/*                                */
	time_t	sem_otime;	/*                         */
} ____cacheline_aligned_in_smp;

/*                                                    */
struct sem_queue {
	struct list_head	list;	 /*                             */
	struct task_struct	*sleeper; /*              */
	struct sem_undo		*undo;	 /*                */
	int			pid;	 /*                                  */
	int			status;	 /*                                */
	struct sembuf		*sops;	 /*                             */
	int			nsops;	 /*                      */
	int			alter;	 /*                             */
};

/*                                                                       
                          
 */
struct sem_undo {
	struct list_head	list_proc;	/*                    
                                   
                       */
	struct rcu_head		rcu;		/*                         */
	struct sem_undo_list	*ulp;		/*                           */
	struct list_head	list_id;	/*                          
                                 */
	int			semid;		/*                          */
	short			*semadj;	/*                      */
						/*                   */
};

/*                                                                        
                                                           
 */
struct sem_undo_list {
	atomic_t		refcnt;
	spinlock_t		lock;
	struct list_head	list_proc;
};


#define sem_ids(ns)	((ns)->ids[IPC_SEM_IDS])

#define sem_checkid(sma, semid)	ipc_checkid(&sma->sem_perm, semid)

static int newary(struct ipc_namespace *, struct ipc_params *);
static void freeary(struct ipc_namespace *, struct kern_ipc_perm *);
#ifdef CONFIG_PROC_FS
static int sysvipc_sem_proc_show(struct seq_file *s, void *it);
#endif

#define SEMMSL_FAST	256 /*                    */
#define SEMOPM_FAST	64  /*                      */

/*
           
                    
                           
                                   
                                                       
                                                                          
   
                                               
                                                 
 */

#define sc_semmsl	sem_ctls[0]
#define sc_semmns	sem_ctls[1]
#define sc_semopm	sem_ctls[2]
#define sc_semmni	sem_ctls[3]

void sem_init_ns(struct ipc_namespace *ns)
{
	ns->sc_semmsl = SEMMSL;
	ns->sc_semmns = SEMMNS;
	ns->sc_semopm = SEMOPM;
	ns->sc_semmni = SEMMNI;
	ns->used_sems = 0;
	ipc_init_ids(&ns->ids[IPC_SEM_IDS]);
}

#ifdef CONFIG_IPC_NS
void sem_exit_ns(struct ipc_namespace *ns)
{
	free_ipcs(ns, &sem_ids(ns), freeary);
	idr_destroy(&ns->ids[IPC_SEM_IDS].ipcs_idr);
}
#endif

void __init sem_init (void)
{
	sem_init_ns(&init_ipc_ns);
	ipc_init_proc_interface("sysvipc/sem",
				"       key      semid perms      nsems   uid   gid  cuid  cgid      otime      ctime\n",
				IPC_SEM_IDS, sysvipc_sem_proc_show);
}

/* 
                                                
                        
  
                                                               
                                                                       
 */
static void unmerge_queues(struct sem_array *sma)
{
	struct sem_queue *q, *tq;

	/*                                  */
	if (sma->complex_count)
		return;
	/*
                                       
                                                          
           
  */
	list_for_each_entry_safe(q, tq, &sma->pending_alter, list) {
		struct sem *curr;
		curr = &sma->sem_base[q->sops[0].sem_num];

		list_add_tail(&q->list, &curr->pending_alter);
	}
	INIT_LIST_HEAD(&sma->pending_alter);
}

/* 
                                                             
                        
  
                                                                       
                                                                      
                                                      
                                                                          
 */
static void merge_queues(struct sem_array *sma)
{
	int i;
	for (i = 0; i < sma->sem_nsems; i++) {
		struct sem *sem = sma->sem_base + i;

		list_splice_init(&sem->pending_alter, &sma->pending_alter);
	}
}

static void sem_rcu_free(struct rcu_head *head)
{
	struct ipc_rcu *p = container_of(head, struct ipc_rcu, rcu);
	struct sem_array *sma = ipc_rcu_to_struct(p);

	security_sem_free(sma);
	ipc_rcu_free(head);
}

/*
                                                              
                                 
                                                              
                              
                                                           
 */
static void sem_wait_array(struct sem_array *sma)
{
	int i;
	struct sem *sem;

	if (sma->complex_count)  {
		/*                                                       
                                                           
   */
		return;
	}

	for (i = 0; i < sma->sem_nsems; i++) {
		sem = sma->sem_base + i;
		spin_unlock_wait(&sem->lock);
	}
}

/*
                                                                      
                                                                     
                                                                   
                                                               
                                                    
 */
static inline int sem_lock(struct sem_array *sma, struct sembuf *sops,
			      int nsops)
{
	struct sem *sem;

	if (nsops != 1) {
		/*                                         */
		ipc_lock_object(&sma->sem_perm);

		/*                                                 
                                        
   */
		sem_wait_array(sma);
		return -1;
	}

	/*
                                                          
                  
                                                           
                                                
                                                  
                                
                                                             
                                                             
                                                                
                 
                                                               
                                                       
                                              
  */
	sem = sma->sem_base + sops->sem_num;

	if (sma->complex_count == 0) {
		/*
                                                    
                                    
   */
		spin_lock(&sem->lock);

		/*                                         */
		if (!spin_is_locked(&sma->sem_perm.lock)) {
			/*                                          */
			smp_mb();

			/*                                      
                                                      
                                             
    */
			if (sma->complex_count == 0) {
				/*                       */
				return sops->sem_num;
			}
		}
		spin_unlock(&sem->lock);
	}

	/*                                  */
	ipc_lock_object(&sma->sem_perm);

	if (sma->complex_count == 0) {
		/*             
                                                      
                           
   */
		spin_lock(&sem->lock);
		ipc_unlock_object(&sma->sem_perm);
		return sops->sem_num;
	} else {
		/*                                                    
               
   */
		sem_wait_array(sma);
		return -1;
	}
}

static inline void sem_unlock(struct sem_array *sma, int locknum)
{
	if (locknum == -1) {
		unmerge_queues(sma);
		ipc_unlock_object(&sma->sem_perm);
	} else {
		struct sem *sem = sma->sem_base + locknum;
		spin_unlock(&sem->lock);
	}
}

/*
                                                                     
               
  
                                      
 */
static inline struct sem_array *sem_obtain_lock(struct ipc_namespace *ns,
			int id, struct sembuf *sops, int nsops, int *locknum)
{
	struct kern_ipc_perm *ipcp;
	struct sem_array *sma;

	ipcp = ipc_obtain_object(&sem_ids(ns), id);
	if (IS_ERR(ipcp))
		return ERR_CAST(ipcp);

	sma = container_of(ipcp, struct sem_array, sem_perm);
	*locknum = sem_lock(sma, sops, nsops);

	/*                                                        
                                                          
  */
	if (!ipcp->deleted)
		return container_of(ipcp, struct sem_array, sem_perm);

	sem_unlock(sma, *locknum);
	return ERR_PTR(-EINVAL);
}

static inline struct sem_array *sem_obtain_object(struct ipc_namespace *ns, int id)
{
	struct kern_ipc_perm *ipcp = ipc_obtain_object(&sem_ids(ns), id);

	if (IS_ERR(ipcp))
		return ERR_CAST(ipcp);

	return container_of(ipcp, struct sem_array, sem_perm);
}

static inline struct sem_array *sem_obtain_object_check(struct ipc_namespace *ns,
							int id)
{
	struct kern_ipc_perm *ipcp = ipc_obtain_object_check(&sem_ids(ns), id);

	if (IS_ERR(ipcp))
		return ERR_CAST(ipcp);

	return container_of(ipcp, struct sem_array, sem_perm);
}

static inline void sem_lock_and_putref(struct sem_array *sma)
{
	sem_lock(sma, NULL, -1);
	ipc_rcu_putref(sma, ipc_rcu_free);
}

static inline void sem_rmid(struct ipc_namespace *ns, struct sem_array *s)
{
	ipc_rmid(&sem_ids(ns), &s->sem_perm);
}

/*
                             
                                                                   
                                                           
                           
                                                    
                                      
                                                           
                              
                         
                                         
                                                       
                                                                    
                                                               
                                                                
                                                  
                                                                   
  
                                                                        
         
                                                                         
                                                                      
                                                         
                                                                 
                                                            
                                                            
                                                            
                                                                    
                                                              
                                                
  
 */
#define IN_WAKEUP	1

/* 
                                      
                 
                                                                    
  
                                               
 */

static int newary(struct ipc_namespace *ns, struct ipc_params *params)
{
	int id;
	int retval;
	struct sem_array *sma;
	int size;
	key_t key = params->key;
	int nsems = params->u.nsems;
	int semflg = params->flg;
	int i;

	if (!nsems)
		return -EINVAL;
	if (ns->used_sems + nsems > ns->sc_semmns)
		return -ENOSPC;

	size = sizeof (*sma) + nsems * sizeof (struct sem);
	sma = ipc_rcu_alloc(size);
	if (!sma) {
		return -ENOMEM;
	}
	memset (sma, 0, size);

	sma->sem_perm.mode = (semflg & S_IRWXUGO);
	sma->sem_perm.key = key;

	sma->sem_perm.security = NULL;
	retval = security_sem_alloc(sma);
	if (retval) {
		ipc_rcu_putref(sma, ipc_rcu_free);
		return retval;
	}

	id = ipc_addid(&sem_ids(ns), &sma->sem_perm, ns->sc_semmni);
	if (id < 0) {
		ipc_rcu_putref(sma, sem_rcu_free);
		return id;
	}
	ns->used_sems += nsems;

	sma->sem_base = (struct sem *) &sma[1];

	for (i = 0; i < nsems; i++) {
		INIT_LIST_HEAD(&sma->sem_base[i].pending_alter);
		INIT_LIST_HEAD(&sma->sem_base[i].pending_const);
		spin_lock_init(&sma->sem_base[i].lock);
	}

	sma->complex_count = 0;
	INIT_LIST_HEAD(&sma->pending_alter);
	INIT_LIST_HEAD(&sma->pending_const);
	INIT_LIST_HEAD(&sma->list_id);
	sma->sem_nsems = nsems;
	sma->sem_ctime = get_seconds();
	sem_unlock(sma, -1);
	rcu_read_unlock();

	return sma->sem_perm.id;
}


/*
                                             
 */
static inline int sem_security(struct kern_ipc_perm *ipcp, int semflg)
{
	struct sem_array *sma;

	sma = container_of(ipcp, struct sem_array, sem_perm);
	return security_sem_associate(sma, semflg);
}

/*
                                             
 */
static inline int sem_more_checks(struct kern_ipc_perm *ipcp,
				struct ipc_params *params)
{
	struct sem_array *sma;

	sma = container_of(ipcp, struct sem_array, sem_perm);
	if (params->u.nsems > sma->sem_nsems)
		return -EINVAL;

	return 0;
}

SYSCALL_DEFINE3(semget, key_t, key, int, nsems, int, semflg)
{
	struct ipc_namespace *ns;
	struct ipc_ops sem_ops;
	struct ipc_params sem_params;

	ns = current->nsproxy->ipc_ns;

	if (nsems < 0 || nsems > ns->sc_semmsl)
		return -EINVAL;

	sem_ops.getnew = newary;
	sem_ops.associate = sem_security;
	sem_ops.more_checks = sem_more_checks;

	sem_params.key = key;
	sem_params.flg = semflg;
	sem_params.u.nsems = nsems;

	return ipcget(ns, &sem_ids(ns), &sem_ops, &sem_params);
}

/*                                                                    
                        
                                                      
                         
                  
                                
  
                                           
                                                                   
                                   
 */

static int perform_atomic_semop(struct sem_array *sma, struct sembuf *sops,
			     int nsops, struct sem_undo *un, int pid)
{
	int result, sem_op;
	struct sembuf *sop;
	struct sem * curr;

	for (sop = sops; sop < sops + nsops; sop++) {
		curr = sma->sem_base + sop->sem_num;
		sem_op = sop->sem_op;
		result = curr->semval;
  
		if (!sem_op && result)
			goto would_block;

		result += sem_op;
		if (result < 0)
			goto would_block;
		if (result > SEMVMX)
			goto out_of_range;
		if (sop->sem_flg & SEM_UNDO) {
			int undo = un->semadj[sop->sem_num] - sem_op;
			/*
                                            
    */
			if (undo < (-SEMAEM - 1) || undo > SEMAEM)
				goto out_of_range;
		}
		curr->semval = result;
	}

	sop--;
	while (sop >= sops) {
		sma->sem_base[sop->sem_num].sempid = pid;
		if (sop->sem_flg & SEM_UNDO)
			un->semadj[sop->sem_num] -= sop->sem_op;
		sop--;
	}
	
	return 0;

out_of_range:
	result = -ERANGE;
	goto undo;

would_block:
	if (sop->sem_flg & IPC_NOWAIT)
		result = -EAGAIN;
	else
		result = 1;

undo:
	sop--;
	while (sop >= sops) {
		sma->sem_base[sop->sem_num].semval -= sop->sem_op;
		sop--;
	}

	return result;
}

/*                                                      
                                        
                                     
  
                                            
 */
static void wake_up_sem_queue_prepare(struct list_head *pt,
				struct sem_queue *q, int error)
{
	if (list_empty(pt)) {
		/*
                                                                 
                                                   
   */
		preempt_disable();
	}
	q->status = IN_WAKEUP;
	q->pid = error;

	list_add_tail(&q->list, pt);
}

/* 
                                                   
                                    
  
                         
                                                                          
                                                                        
                                           
 */
static void wake_up_sem_queue_do(struct list_head *pt)
{
	struct sem_queue *q, *t;
	int did_something;

	did_something = !list_empty(pt);
	list_for_each_entry_safe(q, t, pt, list) {
		wake_up_process(q->sleeper);
		/*                                                      */
		smp_wmb();
		q->status = q->pid;
	}
	if (did_something)
		preempt_enable();
}

static void unlink_queue(struct sem_array *sma, struct sem_queue *q)
{
	list_del(&q->list);
	if (q->nsops > 1)
		sma->complex_count--;
}

/*                       
                        
                                        
  
                                                                      
                                                                       
                                                                      
                      
                                                                  
 */
static int check_restart(struct sem_array *sma, struct sem_queue *q)
{
	/*                                                               */
	if (!list_empty(&sma->pending_alter))
		return 1;

	/*                                                     */
	if (q->nsops > 1)
		return 1;

	/*                                                       
                                        
                                           
                                                      
                                                        
                                    
                                                     
                                                        
                                                      
                                                            
  */
	return 0;
}

/* 
                                                            
                         
                                        
                                                      
  
                                                                       
                                                                             
                                                                          
             
                                                                    
                       
                                                                               
 */
static int wake_const_ops(struct sem_array *sma, int semnum,
				struct list_head *pt)
{
	struct sem_queue *q;
	struct list_head *walk;
	struct list_head *pending_list;
	int semop_completed = 0;

	if (semnum == -1)
		pending_list = &sma->pending_const;
	else
		pending_list = &sma->sem_base[semnum].pending_const;

	walk = pending_list->next;
	while (walk != pending_list) {
		int error;

		q = container_of(walk, struct sem_queue, list);
		walk = walk->next;

		error = perform_atomic_semop(sma, q->sops, q->nsops,
						 q->undo, q->pid);

		if (error <= 0) {
			/*                                                 */

			unlink_queue(sma, q);

			wake_up_sem_queue_prepare(pt, q, error);
			if (error == 0)
				semop_completed = 1;
		}
	}
	return semop_completed;
}

/* 
                                                                              
                        
                                        
                               
                                                     
  
                                                                     
                                                                     
                   
                                                                               
 */
static int do_smart_wakeup_zero(struct sem_array *sma, struct sembuf *sops,
					int nsops, struct list_head *pt)
{
	int i;
	int semop_completed = 0;
	int got_zero = 0;

	/*                                           */
	if (sops) {
		for (i = 0; i < nsops; i++) {
			int num = sops[i].sem_num;

			if (sma->sem_base[num].semval == 0) {
				got_zero = 1;
				semop_completed |= wake_const_ops(sma, num, pt);
			}
		}
	} else {
		/*
                                                 
                             
   */
		for (i = 0; i < sma->sem_nsems; i++) {
			if (sma->sem_base[i].semval == 0) {
				got_zero = 1;
				semop_completed |= wake_const_ops(sma, i, pt);
			}
		}
	}
	/*
                                            
                                     
  */
	if (got_zero)
		semop_completed |= wake_const_ops(sma, -1, pt);

	return semop_completed;
}


/* 
                                                                   
                         
                                        
                                                      
  
                                                                     
                                                                        
                                                                          
             
                                                                    
                       
                                                                      
  
                                                                          
 */
static int update_queue(struct sem_array *sma, int semnum, struct list_head *pt)
{
	struct sem_queue *q;
	struct list_head *walk;
	struct list_head *pending_list;
	int semop_completed = 0;

	if (semnum == -1)
		pending_list = &sma->pending_alter;
	else
		pending_list = &sma->sem_base[semnum].pending_alter;

again:
	walk = pending_list->next;
	while (walk != pending_list) {
		int error, restart;

		q = container_of(walk, struct sem_queue, list);
		walk = walk->next;

		/*                                                         
                                                          
                                                 
                                                              
                                                           
                                                    
   */
		if (semnum != -1 && sma->sem_base[semnum].semval == 0)
			break;

		error = perform_atomic_semop(sma, q->sops, q->nsops,
					 q->undo, q->pid);

		/*                                      */
		if (error > 0)
			continue;

		unlink_queue(sma, q);

		if (error) {
			restart = 0;
		} else {
			semop_completed = 1;
			do_smart_wakeup_zero(sma, q->sops, q->nsops, pt);
			restart = check_restart(sma, q);
		}

		wake_up_sem_queue_prepare(pt, q, error);
		if (restart)
			goto again;
	}
	return semop_completed;
}

/* 
                                          
                        
                                                         
  
                                                        
                                                       
 */
static void set_semotime(struct sem_array *sma, struct sembuf *sops)
{
	if (sops == NULL) {
		sma->sem_base[0].sem_otime = get_seconds();
	} else {
		sma->sem_base[sops[0].sem_num].sem_otime =
							get_seconds();
	}
}

/* 
                                                                        
                        
                                        
                               
                              
                                                     
  
                                                                             
                                                                          
                                                                       
                                                     
                                                            
 */
static void do_smart_update(struct sem_array *sma, struct sembuf *sops, int nsops,
			int otime, struct list_head *pt)
{
	int i;

	otime |= do_smart_wakeup_zero(sma, sops, nsops, pt);

	if (!list_empty(&sma->pending_alter)) {
		/*                                                          */
		otime |= update_queue(sma, -1, pt);
	} else {
		if (!sops) {
			/*
                                                   
                       
    */
			for (i = 0; i < sma->sem_nsems; i++)
				otime |= update_queue(sma, i, pt);
		} else {
			/*
                                               
                                                 
                 
                                                    
                                                 
                                                  
                                     
    */
			for (i = 0; i < nsops; i++) {
				if (sops[i].sem_op > 0) {
					otime |= update_queue(sma,
							sops[i].sem_num, pt);
				}
			}
		}
	}
	if (otime)
		set_semotime(sma, sops);
}

/*                                                       
                                                                   
                                                                
                                                                 
                                                                            
                                                         
                                                                 
                                                                      
 */
static int count_semncnt (struct sem_array * sma, ushort semnum)
{
	int semncnt;
	struct sem_queue * q;

	semncnt = 0;
	list_for_each_entry(q, &sma->sem_base[semnum].pending_alter, list) {
		struct sembuf * sops = q->sops;
		BUG_ON(sops->sem_num != semnum);
		if ((sops->sem_op < 0) && !(sops->sem_flg & IPC_NOWAIT))
			semncnt++;
	}

	list_for_each_entry(q, &sma->pending_alter, list) {
		struct sembuf * sops = q->sops;
		int nsops = q->nsops;
		int i;
		for (i = 0; i < nsops; i++)
			if (sops[i].sem_num == semnum
			    && (sops[i].sem_op < 0)
			    && !(sops[i].sem_flg & IPC_NOWAIT))
				semncnt++;
	}
	return semncnt;
}

static int count_semzcnt (struct sem_array * sma, ushort semnum)
{
	int semzcnt;
	struct sem_queue * q;

	semzcnt = 0;
	list_for_each_entry(q, &sma->sem_base[semnum].pending_const, list) {
		struct sembuf * sops = q->sops;
		BUG_ON(sops->sem_num != semnum);
		if ((sops->sem_op == 0) && !(sops->sem_flg & IPC_NOWAIT))
			semzcnt++;
	}

	list_for_each_entry(q, &sma->pending_const, list) {
		struct sembuf * sops = q->sops;
		int nsops = q->nsops;
		int i;
		for (i = 0; i < nsops; i++)
			if (sops[i].sem_num == semnum
			    && (sops[i].sem_op == 0)
			    && !(sops[i].sem_flg & IPC_NOWAIT))
				semzcnt++;
	}
	return semzcnt;
}

/*                                                                    
                                                                          
                          
 */
static void freeary(struct ipc_namespace *ns, struct kern_ipc_perm *ipcp)
{
	struct sem_undo *un, *tu;
	struct sem_queue *q, *tq;
	struct sem_array *sma = container_of(ipcp, struct sem_array, sem_perm);
	struct list_head tasks;
	int i;

	/*                                                            */
	ipc_assert_locked_object(&sma->sem_perm);
	list_for_each_entry_safe(un, tu, &sma->list_id, list_id) {
		list_del(&un->list_id);
		spin_lock(&un->ulp->lock);
		un->semid = -1;
		list_del_rcu(&un->list_proc);
		spin_unlock(&un->ulp->lock);
		kfree_rcu(un, rcu);
	}

	/*                                                             */
	INIT_LIST_HEAD(&tasks);
	list_for_each_entry_safe(q, tq, &sma->pending_const, list) {
		unlink_queue(sma, q);
		wake_up_sem_queue_prepare(&tasks, q, -EIDRM);
	}

	list_for_each_entry_safe(q, tq, &sma->pending_alter, list) {
		unlink_queue(sma, q);
		wake_up_sem_queue_prepare(&tasks, q, -EIDRM);
	}
	for (i = 0; i < sma->sem_nsems; i++) {
		struct sem *sem = sma->sem_base + i;
		list_for_each_entry_safe(q, tq, &sem->pending_const, list) {
			unlink_queue(sma, q);
			wake_up_sem_queue_prepare(&tasks, q, -EIDRM);
		}
		list_for_each_entry_safe(q, tq, &sem->pending_alter, list) {
			unlink_queue(sma, q);
			wake_up_sem_queue_prepare(&tasks, q, -EIDRM);
		}
	}

	/*                                       */
	sem_rmid(ns, sma);
	sem_unlock(sma, -1);
	rcu_read_unlock();

	wake_up_sem_queue_do(&tasks);
	ns->used_sems -= sma->sem_nsems;
	ipc_rcu_putref(sma, sem_rcu_free);
}

static unsigned long copy_semid_to_user(void __user *buf, struct semid64_ds *in, int version)
{
	switch(version) {
	case IPC_64:
		return copy_to_user(buf, in, sizeof(*in));
	case IPC_OLD:
	    {
		struct semid_ds out;

		memset(&out, 0, sizeof(out));

		ipc64_perm_to_ipc_perm(&in->sem_perm, &out.sem_perm);

		out.sem_otime	= in->sem_otime;
		out.sem_ctime	= in->sem_ctime;
		out.sem_nsems	= in->sem_nsems;

		return copy_to_user(buf, &out, sizeof(out));
	    }
	default:
		return -EINVAL;
	}
}

static time_t get_semotime(struct sem_array *sma)
{
	int i;
	time_t res;

	res = sma->sem_base[0].sem_otime;
	for (i = 1; i < sma->sem_nsems; i++) {
		time_t to = sma->sem_base[i].sem_otime;

		if (to > res)
			res = to;
	}
	return res;
}

static int semctl_nolock(struct ipc_namespace *ns, int semid,
			 int cmd, int version, void __user *p)
{
	int err;
	struct sem_array *sma;

	switch(cmd) {
	case IPC_INFO:
	case SEM_INFO:
	{
		struct seminfo seminfo;
		int max_id;

		err = security_sem_semctl(NULL, cmd);
		if (err)
			return err;
		
		memset(&seminfo,0,sizeof(seminfo));
		seminfo.semmni = ns->sc_semmni;
		seminfo.semmns = ns->sc_semmns;
		seminfo.semmsl = ns->sc_semmsl;
		seminfo.semopm = ns->sc_semopm;
		seminfo.semvmx = SEMVMX;
		seminfo.semmnu = SEMMNU;
		seminfo.semmap = SEMMAP;
		seminfo.semume = SEMUME;
		down_read(&sem_ids(ns).rwsem);
		if (cmd == SEM_INFO) {
			seminfo.semusz = sem_ids(ns).in_use;
			seminfo.semaem = ns->used_sems;
		} else {
			seminfo.semusz = SEMUSZ;
			seminfo.semaem = SEMAEM;
		}
		max_id = ipc_get_maxid(&sem_ids(ns));
		up_read(&sem_ids(ns).rwsem);
		if (copy_to_user(p, &seminfo, sizeof(struct seminfo))) 
			return -EFAULT;
		return (max_id < 0) ? 0: max_id;
	}
	case IPC_STAT:
	case SEM_STAT:
	{
		struct semid64_ds tbuf;
		int id = 0;

		memset(&tbuf, 0, sizeof(tbuf));

		rcu_read_lock();
		if (cmd == SEM_STAT) {
			sma = sem_obtain_object(ns, semid);
			if (IS_ERR(sma)) {
				err = PTR_ERR(sma);
				goto out_unlock;
			}
			id = sma->sem_perm.id;
		} else {
			sma = sem_obtain_object_check(ns, semid);
			if (IS_ERR(sma)) {
				err = PTR_ERR(sma);
				goto out_unlock;
			}
		}

		err = -EACCES;
		if (ipcperms(ns, &sma->sem_perm, S_IRUGO))
			goto out_unlock;

		err = security_sem_semctl(sma, cmd);
		if (err)
			goto out_unlock;

		kernel_to_ipc64_perm(&sma->sem_perm, &tbuf.sem_perm);
		tbuf.sem_otime = get_semotime(sma);
		tbuf.sem_ctime = sma->sem_ctime;
		tbuf.sem_nsems = sma->sem_nsems;
		rcu_read_unlock();
		if (copy_semid_to_user(p, &tbuf, version))
			return -EFAULT;
		return id;
	}
	default:
		return -EINVAL;
	}
out_unlock:
	rcu_read_unlock();
	return err;
}

static int semctl_setval(struct ipc_namespace *ns, int semid, int semnum,
		unsigned long arg)
{
	struct sem_undo *un;
	struct sem_array *sma;
	struct sem* curr;
	int err;
	struct list_head tasks;
	int val;
#if defined(CONFIG_64BIT) && defined(__BIG_ENDIAN)
	/*                  */
	val = arg >> 32;
#else
	/*                              */
	val = arg;
#endif

	if (val > SEMVMX || val < 0)
		return -ERANGE;

	INIT_LIST_HEAD(&tasks);

	rcu_read_lock();
	sma = sem_obtain_object_check(ns, semid);
	if (IS_ERR(sma)) {
		rcu_read_unlock();
		return PTR_ERR(sma);
	}

	if (semnum < 0 || semnum >= sma->sem_nsems) {
		rcu_read_unlock();
		return -EINVAL;
	}


	if (ipcperms(ns, &sma->sem_perm, S_IWUGO)) {
		rcu_read_unlock();
		return -EACCES;
	}

	err = security_sem_semctl(sma, SETVAL);
	if (err) {
		rcu_read_unlock();
		return -EACCES;
	}

	sem_lock(sma, NULL, -1);

	if (sma->sem_perm.deleted) {
		sem_unlock(sma, -1);
		rcu_read_unlock();
		return -EIDRM;
	}

	curr = &sma->sem_base[semnum];

	ipc_assert_locked_object(&sma->sem_perm);
	list_for_each_entry(un, &sma->list_id, list_id)
		un->semadj[semnum] = 0;

	curr->semval = val;
	curr->sempid = task_tgid_vnr(current);
	sma->sem_ctime = get_seconds();
	/*                                                      */
	do_smart_update(sma, NULL, 0, 0, &tasks);
	sem_unlock(sma, -1);
	rcu_read_unlock();
	wake_up_sem_queue_do(&tasks);
	return 0;
}

static int semctl_main(struct ipc_namespace *ns, int semid, int semnum,
		int cmd, void __user *p)
{
	struct sem_array *sma;
	struct sem* curr;
	int err, nsems;
	ushort fast_sem_io[SEMMSL_FAST];
	ushort* sem_io = fast_sem_io;
	struct list_head tasks;

	INIT_LIST_HEAD(&tasks);

	rcu_read_lock();
	sma = sem_obtain_object_check(ns, semid);
	if (IS_ERR(sma)) {
		rcu_read_unlock();
		return PTR_ERR(sma);
	}

	nsems = sma->sem_nsems;

	err = -EACCES;
	if (ipcperms(ns, &sma->sem_perm, cmd == SETALL ? S_IWUGO : S_IRUGO))
		goto out_rcu_wakeup;

	err = security_sem_semctl(sma, cmd);
	if (err)
		goto out_rcu_wakeup;

	err = -EACCES;
	switch (cmd) {
	case GETALL:
	{
		ushort __user *array = p;
		int i;

		sem_lock(sma, NULL, -1);
		if (sma->sem_perm.deleted) {
			err = -EIDRM;
			goto out_unlock;
		}
		if(nsems > SEMMSL_FAST) {
			if (!ipc_rcu_getref(sma)) {
				err = -EIDRM;
				goto out_unlock;
			}
			sem_unlock(sma, -1);
			rcu_read_unlock();
			sem_io = ipc_alloc(sizeof(ushort)*nsems);
			if(sem_io == NULL) {
				ipc_rcu_putref(sma, ipc_rcu_free);
				return -ENOMEM;
			}

			rcu_read_lock();
			sem_lock_and_putref(sma);
			if (sma->sem_perm.deleted) {
				err = -EIDRM;
				goto out_unlock;
			}
		}
		for (i = 0; i < sma->sem_nsems; i++)
			sem_io[i] = sma->sem_base[i].semval;
		sem_unlock(sma, -1);
		rcu_read_unlock();
		err = 0;
		if(copy_to_user(array, sem_io, nsems*sizeof(ushort)))
			err = -EFAULT;
		goto out_free;
	}
	case SETALL:
	{
		int i;
		struct sem_undo *un;

		if (!ipc_rcu_getref(sma)) {
			err = -EIDRM;
			goto out_rcu_wakeup;
		}
		rcu_read_unlock();

		if(nsems > SEMMSL_FAST) {
			sem_io = ipc_alloc(sizeof(ushort)*nsems);
			if(sem_io == NULL) {
				ipc_rcu_putref(sma, ipc_rcu_free);
				return -ENOMEM;
			}
		}

		if (copy_from_user (sem_io, p, nsems*sizeof(ushort))) {
			ipc_rcu_putref(sma, ipc_rcu_free);
			err = -EFAULT;
			goto out_free;
		}

		for (i = 0; i < nsems; i++) {
			if (sem_io[i] > SEMVMX) {
				ipc_rcu_putref(sma, ipc_rcu_free);
				err = -ERANGE;
				goto out_free;
			}
		}
		rcu_read_lock();
		sem_lock_and_putref(sma);
		if (sma->sem_perm.deleted) {
			err = -EIDRM;
			goto out_unlock;
		}

		for (i = 0; i < nsems; i++)
			sma->sem_base[i].semval = sem_io[i];

		ipc_assert_locked_object(&sma->sem_perm);
		list_for_each_entry(un, &sma->list_id, list_id) {
			for (i = 0; i < nsems; i++)
				un->semadj[i] = 0;
		}
		sma->sem_ctime = get_seconds();
		/*                                                      */
		do_smart_update(sma, NULL, 0, 0, &tasks);
		err = 0;
		goto out_unlock;
	}
	/*                                                */
	}
	err = -EINVAL;
	if (semnum < 0 || semnum >= nsems)
		goto out_rcu_wakeup;

	sem_lock(sma, NULL, -1);
	if (sma->sem_perm.deleted) {
		err = -EIDRM;
		goto out_unlock;
	}
	curr = &sma->sem_base[semnum];

	switch (cmd) {
	case GETVAL:
		err = curr->semval;
		goto out_unlock;
	case GETPID:
		err = curr->sempid;
		goto out_unlock;
	case GETNCNT:
		err = count_semncnt(sma,semnum);
		goto out_unlock;
	case GETZCNT:
		err = count_semzcnt(sma,semnum);
		goto out_unlock;
	}

out_unlock:
	sem_unlock(sma, -1);
out_rcu_wakeup:
	rcu_read_unlock();
	wake_up_sem_queue_do(&tasks);
out_free:
	if(sem_io != fast_sem_io)
		ipc_free(sem_io, sizeof(ushort)*nsems);
	return err;
}

static inline unsigned long
copy_semid_from_user(struct semid64_ds *out, void __user *buf, int version)
{
	switch(version) {
	case IPC_64:
		if (copy_from_user(out, buf, sizeof(*out)))
			return -EFAULT;
		return 0;
	case IPC_OLD:
	    {
		struct semid_ds tbuf_old;

		if(copy_from_user(&tbuf_old, buf, sizeof(tbuf_old)))
			return -EFAULT;

		out->sem_perm.uid	= tbuf_old.sem_perm.uid;
		out->sem_perm.gid	= tbuf_old.sem_perm.gid;
		out->sem_perm.mode	= tbuf_old.sem_perm.mode;

		return 0;
	    }
	default:
		return -EINVAL;
	}
}

/*
                                                                     
                            
                                                                        
 */
static int semctl_down(struct ipc_namespace *ns, int semid,
		       int cmd, int version, void __user *p)
{
	struct sem_array *sma;
	int err;
	struct semid64_ds semid64;
	struct kern_ipc_perm *ipcp;

	if(cmd == IPC_SET) {
		if (copy_semid_from_user(&semid64, p, version))
			return -EFAULT;
	}

	down_write(&sem_ids(ns).rwsem);
	rcu_read_lock();

	ipcp = ipcctl_pre_down_nolock(ns, &sem_ids(ns), semid, cmd,
				      &semid64.sem_perm, 0);
	if (IS_ERR(ipcp)) {
		err = PTR_ERR(ipcp);
		goto out_unlock1;
	}

	sma = container_of(ipcp, struct sem_array, sem_perm);

	err = security_sem_semctl(sma, cmd);
	if (err)
		goto out_unlock1;

	switch (cmd) {
	case IPC_RMID:
		sem_lock(sma, NULL, -1);
		/*                                        */
		freeary(ns, ipcp);
		goto out_up;
	case IPC_SET:
		sem_lock(sma, NULL, -1);
		err = ipc_update_perm(&semid64.sem_perm, ipcp);
		if (err)
			goto out_unlock0;
		sma->sem_ctime = get_seconds();
		break;
	default:
		err = -EINVAL;
		goto out_unlock1;
	}

out_unlock0:
	sem_unlock(sma, -1);
out_unlock1:
	rcu_read_unlock();
out_up:
	up_write(&sem_ids(ns).rwsem);
	return err;
}

SYSCALL_DEFINE4(semctl, int, semid, int, semnum, int, cmd, unsigned long, arg)
{
	int version;
	struct ipc_namespace *ns;
	void __user *p = (void __user *)arg;

	if (semid < 0)
		return -EINVAL;

	version = ipc_parse_version(&cmd);
	ns = current->nsproxy->ipc_ns;

	switch(cmd) {
	case IPC_INFO:
	case SEM_INFO:
	case IPC_STAT:
	case SEM_STAT:
		return semctl_nolock(ns, semid, cmd, version, p);
	case GETALL:
	case GETVAL:
	case GETPID:
	case GETNCNT:
	case GETZCNT:
	case SETALL:
		return semctl_main(ns, semid, semnum, cmd, p);
	case SETVAL:
		return semctl_setval(ns, semid, semnum, arg);
	case IPC_RMID:
	case IPC_SET:
		return semctl_down(ns, semid, cmd, version, p);
	default:
		return -EINVAL;
	}
}

/*                                                                
                                                                     
                         
  
                                                        
                                                                          
                                                                      
                
  
                                                 
 */
static inline int get_undo_list(struct sem_undo_list **undo_listp)
{
	struct sem_undo_list *undo_list;

	undo_list = current->sysvsem.undo_list;
	if (!undo_list) {
		undo_list = kzalloc(sizeof(*undo_list), GFP_KERNEL);
		if (undo_list == NULL)
			return -ENOMEM;
		spin_lock_init(&undo_list->lock);
		atomic_set(&undo_list->refcnt, 1);
		INIT_LIST_HEAD(&undo_list->list_proc);

		current->sysvsem.undo_list = undo_list;
	}
	*undo_listp = undo_list;
	return 0;
}

static struct sem_undo *__lookup_undo(struct sem_undo_list *ulp, int semid)
{
	struct sem_undo *un;

	list_for_each_entry_rcu(un, &ulp->list_proc, list_proc) {
		if (un->semid == semid)
			return un;
	}
	return NULL;
}

static struct sem_undo *lookup_undo(struct sem_undo_list *ulp, int semid)
{
	struct sem_undo *un;

  	assert_spin_locked(&ulp->lock);

	un = __lookup_undo(ulp, semid);
	if (un) {
		list_del_rcu(&un->list_proc);
		list_add_rcu(&un->list_proc, &ulp->list_proc);
	}
	return un;
}

/* 
                                                                  
                 
                             
  
                                                                         
                                                                      
                                                          
                                                                      
                              
 */
static struct sem_undo *find_alloc_undo(struct ipc_namespace *ns, int semid)
{
	struct sem_array *sma;
	struct sem_undo_list *ulp;
	struct sem_undo *un, *new;
	int nsems, error;

	error = get_undo_list(&ulp);
	if (error)
		return ERR_PTR(error);

	rcu_read_lock();
	spin_lock(&ulp->lock);
	un = lookup_undo(ulp, semid);
	spin_unlock(&ulp->lock);
	if (likely(un!=NULL))
		goto out;

	/*                                          */
	/*                                                    */
	sma = sem_obtain_object_check(ns, semid);
	if (IS_ERR(sma)) {
		rcu_read_unlock();
		return ERR_CAST(sma);
	}

	nsems = sma->sem_nsems;
	if (!ipc_rcu_getref(sma)) {
		rcu_read_unlock();
		un = ERR_PTR(-EIDRM);
		goto out;
	}
	rcu_read_unlock();

	/*                                     */
	new = kzalloc(sizeof(struct sem_undo) + sizeof(short)*nsems, GFP_KERNEL);
	if (!new) {
		ipc_rcu_putref(sma, ipc_rcu_free);
		return ERR_PTR(-ENOMEM);
	}

	/*                                             */
	rcu_read_lock();
	sem_lock_and_putref(sma);
	if (sma->sem_perm.deleted) {
		sem_unlock(sma, -1);
		rcu_read_unlock();
		kfree(new);
		un = ERR_PTR(-EIDRM);
		goto out;
	}
	spin_lock(&ulp->lock);

	/*
                                                                       
  */
	un = lookup_undo(ulp, semid);
	if (un) {
		kfree(new);
		goto success;
	}
	/*                                              */
	new->semadj = (short *) &new[1];
	new->ulp = ulp;
	new->semid = semid;
	assert_spin_locked(&ulp->lock);
	list_add_rcu(&new->list_proc, &ulp->list_proc);
	ipc_assert_locked_object(&sma->sem_perm);
	list_add(&new->list_id, &sma->list_id);
	un = new;

success:
	spin_unlock(&ulp->lock);
	sem_unlock(sma, -1);
out:
	return un;
}


/* 
                                                             
                                 
  
                                                                            
                                                                          
                                                                           
                                                                              
                            
  
                                                                             
 */
static int get_queue_result(struct sem_queue *q)
{
	int error;

	error = q->status;
	while (unlikely(error == IN_WAKEUP)) {
		cpu_relax();
		error = q->status;
	}

	return error;
}

SYSCALL_DEFINE4(semtimedop, int, semid, struct sembuf __user *, tsops,
		unsigned, nsops, const struct timespec __user *, timeout)
{
	int error = -EINVAL;
	struct sem_array *sma;
	struct sembuf fast_sops[SEMOPM_FAST];
	struct sembuf* sops = fast_sops, *sop;
	struct sem_undo *un;
	int undos = 0, alter = 0, max, locknum;
	struct sem_queue queue;
	unsigned long jiffies_left = 0;
	struct ipc_namespace *ns;
	struct list_head tasks;

	ns = current->nsproxy->ipc_ns;

	if (nsops < 1 || semid < 0)
		return -EINVAL;
	if (nsops > ns->sc_semopm)
		return -E2BIG;
	if(nsops > SEMOPM_FAST) {
		sops = kmalloc(sizeof(*sops)*nsops,GFP_KERNEL);
		if(sops==NULL)
			return -ENOMEM;
	}
	if (copy_from_user (sops, tsops, nsops * sizeof(*tsops))) {
		error=-EFAULT;
		goto out_free;
	}
	if (timeout) {
		struct timespec _timeout;
		if (copy_from_user(&_timeout, timeout, sizeof(*timeout))) {
			error = -EFAULT;
			goto out_free;
		}
		if (_timeout.tv_sec < 0 || _timeout.tv_nsec < 0 ||
			_timeout.tv_nsec >= 1000000000L) {
			error = -EINVAL;
			goto out_free;
		}
		jiffies_left = timespec_to_jiffies(&_timeout);
	}
	max = 0;
	for (sop = sops; sop < sops + nsops; sop++) {
		if (sop->sem_num >= max)
			max = sop->sem_num;
		if (sop->sem_flg & SEM_UNDO)
			undos = 1;
		if (sop->sem_op != 0)
			alter = 1;
	}

	INIT_LIST_HEAD(&tasks);

	if (undos) {
		/*                                                     */
		un = find_alloc_undo(ns, semid);
		if (IS_ERR(un)) {
			error = PTR_ERR(un);
			goto out_free;
		}
	} else {
		un = NULL;
		rcu_read_lock();
	}

	sma = sem_obtain_object_check(ns, semid);
	if (IS_ERR(sma)) {
		rcu_read_unlock();
		error = PTR_ERR(sma);
		goto out_free;
	}

	error = -EFBIG;
	if (max >= sma->sem_nsems)
		goto out_rcu_wakeup;

	error = -EACCES;
	if (ipcperms(ns, &sma->sem_perm, alter ? S_IWUGO : S_IRUGO))
		goto out_rcu_wakeup;

	error = security_sem_semop(sma, sops, nsops, alter);
	if (error)
		goto out_rcu_wakeup;

	error = -EIDRM;
	locknum = sem_lock(sma, sops, nsops);
	if (sma->sem_perm.deleted)
		goto out_unlock_free;
	/*
                                                               
                                                              
                                                                  
                                                                  
                                     
  */
	if (un && un->semid == -1)
		goto out_unlock_free;

	error = perform_atomic_semop(sma, sops, nsops, un,
					task_tgid_vnr(current));
	if (error == 0) {
		/*                                         
                          
   */
		if (alter)
			do_smart_update(sma, sops, nsops, 1, &tasks);
		else
			set_semotime(sma, sops);
	}
	if (error <= 0)
		goto out_unlock_free;

	/*                                                          
                                                
  */
		
	queue.sops = sops;
	queue.nsops = nsops;
	queue.undo = un;
	queue.pid = task_tgid_vnr(current);
	queue.alter = alter;

	if (nsops == 1) {
		struct sem *curr;
		curr = &sma->sem_base[sops->sem_num];

		if (alter) {
			if (sma->complex_count) {
				list_add_tail(&queue.list,
						&sma->pending_alter);
			} else {

				list_add_tail(&queue.list,
						&curr->pending_alter);
			}
		} else {
			list_add_tail(&queue.list, &curr->pending_const);
		}
	} else {
		if (!sma->complex_count)
			merge_queues(sma);

		if (alter)
			list_add_tail(&queue.list, &sma->pending_alter);
		else
			list_add_tail(&queue.list, &sma->pending_const);

		sma->complex_count++;
	}

	queue.status = -EINTR;
	queue.sleeper = current;

sleep_again:
	current->state = TASK_INTERRUPTIBLE;
	sem_unlock(sma, locknum);
	rcu_read_unlock();

	if (timeout)
		jiffies_left = schedule_timeout(jiffies_left);
	else
		schedule();

	error = get_queue_result(&queue);

	if (error != -EINTR) {
		/*                                                       
               
                                                             
                                                         
                                                         
                                                        
   */
		smp_mb();

		goto out_free;
	}

	rcu_read_lock();
	sma = sem_obtain_lock(ns, semid, sops, nsops, &locknum);

	/*
                                                                        
  */
	error = get_queue_result(&queue);

	/*
                                                      
  */
	if (IS_ERR(sma)) {
		rcu_read_unlock();
		goto out_free;
	}


	/*
                                                                 
                                                        
  */

	if (error != -EINTR) {
		goto out_unlock_free;
	}

	/*
                                                          
  */
	if (timeout && jiffies_left == 0)
		error = -EAGAIN;

	/*
                                          
  */
	if (error == -EINTR && !signal_pending(current))
		goto sleep_again;

	unlink_queue(sma, &queue);

out_unlock_free:
	sem_unlock(sma, locknum);
out_rcu_wakeup:
	rcu_read_unlock();
	wake_up_sem_queue_do(&tasks);
out_free:
	if(sops != fast_sops)
		kfree(sops);
	return error;
}

SYSCALL_DEFINE3(semop, int, semid, struct sembuf __user *, tsops,
		unsigned, nsops)
{
	return sys_semtimedop(semid, tsops, nsops, NULL);
}

/*                                                                     
                          
 */

int copy_semundo(unsigned long clone_flags, struct task_struct *tsk)
{
	struct sem_undo_list *undo_list;
	int error;

	if (clone_flags & CLONE_SYSVSEM) {
		error = get_undo_list(&undo_list);
		if (error)
			return error;
		atomic_inc(&undo_list->refcnt);
		tsk->sysvsem.undo_list = undo_list;
	} else 
		tsk->sysvsem.undo_list = NULL;

	return 0;
}

/*
                                                         
                                                                    
                                      
                                                                
                                                                       
                                                                       
                                                          
                                                                     
                                                                
                                                                       
 */
void exit_sem(struct task_struct *tsk)
{
	struct sem_undo_list *ulp;

	ulp = tsk->sysvsem.undo_list;
	if (!ulp)
		return;
	tsk->sysvsem.undo_list = NULL;

	if (!atomic_dec_and_test(&ulp->refcnt))
		return;

	for (;;) {
		struct sem_array *sma;
		struct sem_undo *un;
		struct list_head tasks;
		int semid, i;

		rcu_read_lock();
		un = list_entry_rcu(ulp->list_proc.next,
				    struct sem_undo, list_proc);
		if (&un->list_proc == &ulp->list_proc)
			semid = -1;
		 else
			semid = un->semid;

		if (semid == -1) {
			rcu_read_unlock();
			break;
		}

		sma = sem_obtain_object_check(tsk->nsproxy->ipc_ns, un->semid);
		/*                                             */
		if (IS_ERR(sma)) {
			rcu_read_unlock();
			continue;
		}

		sem_lock(sma, NULL, -1);
		/*                                             */
		if (sma->sem_perm.deleted) {
			sem_unlock(sma, -1);
			rcu_read_unlock();
			continue;
		}
		un = __lookup_undo(ulp, semid);
		if (un == NULL) {
			/*                                                   
                                            
    */
			sem_unlock(sma, -1);
			rcu_read_unlock();
			continue;
		}

		/*                                 */
		ipc_assert_locked_object(&sma->sem_perm);
		list_del(&un->list_id);

		spin_lock(&ulp->lock);
		list_del_rcu(&un->list_proc);
		spin_unlock(&ulp->lock);

		/*                                      */
		for (i = 0; i < sma->sem_nsems; i++) {
			struct sem * semaphore = &sma->sem_base[i];
			if (un->semadj[i]) {
				semaphore->semval += un->semadj[i];
				/*
				 * Range checks of the new semaphore value,
				 * not defined by sus:
				 * - Some unices ignore the undo entirely
				 *   (e.g. HP UX 11i 11.22, Tru64 V5.1)
				 * - some cap the value (e.g. FreeBSD caps
				 *   at 0, but doesn't enforce SEMVMX)
				 *
				 * Linux caps the semaphore value, both at 0
				 * and at SEMVMX.
				 *
				 * 	Manfred <manfred@colorfullife.com>
				 */
				if (semaphore->semval < 0)
					semaphore->semval = 0;
				if (semaphore->semval > SEMVMX)
					semaphore->semval = SEMVMX;
				semaphore->sempid = task_tgid_vnr(current);
			}
		}
		/*                                                      */
		INIT_LIST_HEAD(&tasks);
		do_smart_update(sma, NULL, 0, 1, &tasks);
		sem_unlock(sma, -1);
		rcu_read_unlock();
		wake_up_sem_queue_do(&tasks);

		kfree_rcu(un, rcu);
	}
	kfree(ulp);
}

#ifdef CONFIG_PROC_FS
static int sysvipc_sem_proc_show(struct seq_file *s, void *it)
{
	struct user_namespace *user_ns = seq_user_ns(s);
	struct sem_array *sma = it;
	time_t sem_otime;

	/*
                                                          
                                                     
                                                                   
                                                              
  */
	sem_wait_array(sma);

	sem_otime = get_semotime(sma);

	return seq_printf(s,
			  "%10d %10d  %4o %10u %5u %5u %5u %5u %10lu %10lu\n",
			  sma->sem_perm.key,
			  sma->sem_perm.id,
			  sma->sem_perm.mode,
			  sma->sem_nsems,
			  from_kuid_munged(user_ns, sma->sem_perm.uid),
			  from_kgid_munged(user_ns, sma->sem_perm.gid),
			  from_kuid_munged(user_ns, sma->sem_perm.cuid),
			  from_kgid_munged(user_ns, sma->sem_perm.cgid),
			  sem_otime,
			  sma->sem_ctime);
}
#endif
