/*
 * linux/ipc/util.c
 * Copyright (C) 1992 Krishna Balasubramanian
 *
 * Sep 1997 - Call suser() last after "normal" permission checks so we
 *            get BSD style process accounting right.
 *            Occurs in several places in the IPC code.
 *            Chris Evans, <chris@ferret.lmh.ox.ac.uk>
 * Nov 1999 - ipc helper functions, unified SMP locking
 *	      Manfred Spraul <manfred@colorfullife.com>
 * Oct 2002 - One lock per IPC id. RCU ipc_free for lock-free grow_ary().
 *            Mingming Cao <cmm@us.ibm.com>
 * Mar 2006 - support for audit of ipc object properties
 *            Dustin Kirkland <dustin.kirkland@us.ibm.com>
 * Jun 2006 - namespaces ssupport
 *            OpenVZ, SWsoft Inc.
 *            Pavel Emelianov <xemul@openvz.org>
 *
 * General sysv ipc locking scheme:
 *	rcu_read_lock()
 *          obtain the ipc object (kern_ipc_perm) by looking up the id in an idr
 *	    tree.
 *	    - perform initial checks (capabilities, auditing and permission,
 *	      etc).
 *	    - perform read-only operations, such as STAT, INFO commands.
 *	      acquire the ipc lock (kern_ipc_perm.lock) through
 *	      ipc_lock_object()
 *		- perform data updates, such as SET, RMID commands and
 *		  mechanism-specific operations (semop/semtimedop,
 *		  msgsnd/msgrcv, shmat/shmdt).
 *	    drop the ipc lock, through ipc_unlock_object().
 *	rcu_read_unlock()
 *
 *  The ids->rwsem must be taken when:
 *	- creating, removing and iterating the existing entries in ipc
 *	  identifier sets.
 *	- iterating through files under /proc/sysvipc/
 *
 *  Note that sems have a special fast path that avoids kern_ipc_perm.lock -
 *  see sem_lock().
 */

#include <linux/mm.h>
#include <linux/shm.h>
#include <linux/init.h>
#include <linux/msg.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>
#include <linux/notifier.h>
#include <linux/capability.h>
#include <linux/highuid.h>
#include <linux/security.h>
#include <linux/rcupdate.h>
#include <linux/workqueue.h>
#include <linux/seq_file.h>
#include <linux/proc_fs.h>
#include <linux/audit.h>
#include <linux/nsproxy.h>
#include <linux/rwsem.h>
#include <linux/memory.h>
#include <linux/ipc_namespace.h>

#include <asm/unistd.h>

#include "util.h"

struct ipc_proc_iface {
	const char *path;
	const char *header;
	int ids;
	int (*show)(struct seq_file *, void *);
};

static void ipc_memory_notifier(struct work_struct *work)
{
	ipcns_notify(IPCNS_MEMCHANGED);
}

static int ipc_memory_callback(struct notifier_block *self,
				unsigned long action, void *arg)
{
	static DECLARE_WORK(ipc_memory_wq, ipc_memory_notifier);

	switch (action) {
	case MEM_ONLINE:    /*                                    */
	case MEM_OFFLINE:   /*                                           */
		/*
                                                               
                          
                                                              
                                                              
                                           
                                                         
   */
		if (!work_pending(&ipc_memory_wq))
			schedule_work(&ipc_memory_wq);
		break;
	case MEM_GOING_ONLINE:
	case MEM_GOING_OFFLINE:
	case MEM_CANCEL_ONLINE:
	case MEM_CANCEL_OFFLINE:
	default:
		break;
	}

	return NOTIFY_OK;
}

static struct notifier_block ipc_memory_nb = {
	.notifier_call = ipc_memory_callback,
	.priority = IPC_CALLBACK_PRI,
};

/* 
                                      
  
                                                                     
                          
                                                                    
                                                                     
                                                                  
 */
 
static int __init ipc_init(void)
{
	sem_init();
	msg_init();
	shm_init();
	register_hotmemory_notifier(&ipc_memory_nb);
	register_ipcns_notifier(&init_ipc_ns);
	return 0;
}
__initcall(ipc_init);

/* 
                                             
                       
  
                                                                         
                                             
 */
 
void ipc_init_ids(struct ipc_ids *ids)
{
	init_rwsem(&ids->rwsem);

	ids->in_use = 0;
	ids->seq = 0;
	ids->next_id = -1;
	{
		int seq_limit = INT_MAX/SEQ_MULTIPLIER;
		if (seq_limit > USHRT_MAX)
			ids->seq_max = USHRT_MAX;
		 else
		 	ids->seq_max = seq_limit;
	}

	idr_init(&ids->ipcs_idr);
}

#ifdef CONFIG_PROC_FS
static const struct file_operations sysvipc_proc_fops;
/* 
                                                                                                  
                        
                                                              
                                 
                       
 */
void __init ipc_init_proc_interface(const char *path, const char *header,
		int ids, int (*show)(struct seq_file *, void *))
{
	struct proc_dir_entry *pde;
	struct ipc_proc_iface *iface;

	iface = kmalloc(sizeof(*iface), GFP_KERNEL);
	if (!iface)
		return;
	iface->path	= path;
	iface->header	= header;
	iface->ids	= ids;
	iface->show	= show;

	pde = proc_create_data(path,
			       S_IRUGO,        /*                */
			       NULL,           /*            */
			       &sysvipc_proc_fops,
			       iface);
	if (!pde) {
		kfree(iface);
	}
}
#endif

/* 
                                                     
                       
                        
   
                                 
                                                                   
          
                                                         
 */
 
static struct kern_ipc_perm *ipc_findkey(struct ipc_ids *ids, key_t key)
{
	struct kern_ipc_perm *ipc;
	int next_id;
	int total;

	for (total = 0, next_id = 0; total < ids->in_use; next_id++) {
		ipc = idr_find(&ids->ipcs_idr, next_id);

		if (ipc == NULL)
			continue;

		if (ipc->key != key) {
			total++;
			continue;
		}

		rcu_read_lock();
		ipc_lock_object(ipc);
		return ipc;
	}

	return NULL;
}

/* 
                                            
                           
  
                                  
 */

int ipc_get_maxid(struct ipc_ids *ids)
{
	struct kern_ipc_perm *ipc;
	int max_id = -1;
	int total, id;

	if (ids->in_use == 0)
		return -1;

	if (ids->in_use == IPCMNI)
		return IPCMNI - 1;

	/*                               */
	total = 0;
	for (id = 0; id < IPCMNI && total < ids->in_use; id++) {
		ipc = idr_find(&ids->ipcs_idr, id);
		if (ipc != NULL) {
			max_id = id;
			total++;
		}
	}
	return max_id;
}

/* 
                                     
                           
                               
                                          
  
                                                                   
                                                                     
                                                                         
                                                                          
  
                                         
 */
int ipc_addid(struct ipc_ids* ids, struct kern_ipc_perm* new, int size)
{
	kuid_t euid;
	kgid_t egid;
	int id;
	int next_id = ids->next_id;

	if (size > IPCMNI)
		size = IPCMNI;

	if (ids->in_use >= size)
		return -ENOSPC;

	idr_preload(GFP_KERNEL);

	spin_lock_init(&new->lock);
	new->deleted = 0;
	rcu_read_lock();
	spin_lock(&new->lock);

	id = idr_alloc(&ids->ipcs_idr, new,
		       (next_id < 0) ? 0 : ipcid_to_idx(next_id), 0,
		       GFP_NOWAIT);
	idr_preload_end();
	if (id < 0) {
		spin_unlock(&new->lock);
		rcu_read_unlock();
		return id;
	}

	ids->in_use++;

	current_euid_egid(&euid, &egid);
	new->cuid = new->uid = euid;
	new->gid = new->cgid = egid;

	if (next_id < 0) {
		new->seq = ids->seq++;
		if (ids->seq > ids->seq_max)
			ids->seq = 0;
	} else {
		new->seq = ipcid_to_seqx(next_id);
		ids->next_id = -1;
	}

	new->id = ipc_buildid(id, new->seq);
	return id;
}

/* 
                                       
                 
                          
                                            
                          
  
                                                                      
                               
 */
static int ipcget_new(struct ipc_namespace *ns, struct ipc_ids *ids,
		struct ipc_ops *ops, struct ipc_params *params)
{
	int err;

	down_write(&ids->rwsem);
	err = ops->getnew(ns, params);
	up_write(&ids->rwsem);
	return err;
}

/* 
                                                              
                     
                            
                                            
                          
  
                                                                        
                                                                          
                
  
                                      
  
                                                       
 */
static int ipc_check_perms(struct ipc_namespace *ns,
			   struct kern_ipc_perm *ipcp,
			   struct ipc_ops *ops,
			   struct ipc_params *params)
{
	int err;

	if (ipcperms(ns, ipcp, params->flg))
		err = -EACCES;
	else {
		err = ops->associate(ipcp, params->flg);
		if (!err)
			err = ipcp->id;
	}

	return err;
}

/* 
                                                        
                 
                          
                                            
                          
  
                                                                      
                                   
                                                                       
                                                 
  
                                      
 */
static int ipcget_public(struct ipc_namespace *ns, struct ipc_ids *ids,
		struct ipc_ops *ops, struct ipc_params *params)
{
	struct kern_ipc_perm *ipcp;
	int flg = params->flg;
	int err;

	/*
                                                                   
                                                 
  */
	down_write(&ids->rwsem);
	ipcp = ipc_findkey(ids, params->key);
	if (ipcp == NULL) {
		/*              */
		if (!(flg & IPC_CREAT))
			err = -ENOENT;
		else
			err = ops->getnew(ns, params);
	} else {
		/*                                             */

		if (flg & IPC_CREAT && flg & IPC_EXCL)
			err = -EEXIST;
		else {
			err = 0;
			if (ops->more_checks)
				err = ops->more_checks(ipcp, params);
			if (!err)
				/*
                                            
              
     */
				err = ipc_check_perms(ns, ipcp, ops, params);
		}
		ipc_unlock(ipcp);
	}
	up_write(&ids->rwsem);

	return err;
}


/* 
                                      
                           
                                                                
  
                                                                    
                                                                 
 */
 
void ipc_rmid(struct ipc_ids *ids, struct kern_ipc_perm *ipcp)
{
	int lid = ipcid_to_idx(ipcp->id);

	idr_remove(&ids->ipcs_idr, lid);

	ids->in_use--;

	ipcp->deleted = 1;

	return;
}

/* 
                                 
                      
  
                                                                         
                                           
 */
 
void *ipc_alloc(int size)
{
	void *out;
	if(size > PAGE_SIZE)
		out = vmalloc(size);
	else
		out = kmalloc(size, GFP_KERNEL);
	return out;
}

/* 
                                         
                                      
                       
  
                                                                       
                               
 */

void ipc_free(void* ptr, int size)
{
	if(size > PAGE_SIZE)
		vfree(ptr);
	else
		kfree(ptr);
}

/* 
                                              
                      
  
                                                              
                                                          
 */
void *ipc_rcu_alloc(int size)
{
	/*
                                                 
  */
	struct ipc_rcu *out = ipc_alloc(sizeof(struct ipc_rcu) + size);
	if (unlikely(!out))
		return NULL;
	atomic_set(&out->refcount, 1);
	return out + 1;
}

int ipc_rcu_getref(void *ptr)
{
	struct ipc_rcu *p = ((struct ipc_rcu *)ptr) - 1;

	return atomic_inc_not_zero(&p->refcount);
}

void ipc_rcu_putref(void *ptr, void (*func)(struct rcu_head *head))
{
	struct ipc_rcu *p = ((struct ipc_rcu *)ptr) - 1;

	if (!atomic_dec_and_test(&p->refcount))
		return;

	call_rcu(&p->rcu, func);
}

void ipc_rcu_free(struct rcu_head *head)
{
	struct ipc_rcu *p = container_of(head, struct ipc_rcu, rcu);

	if (is_vmalloc_addr(p))
		vfree(p);
	else
		kfree(p);
}

/* 
                                   
                     
                            
                                 
  
                                                  
                                        
  
                                                                 
 */
 
int ipcperms(struct ipc_namespace *ns, struct kern_ipc_perm *ipcp, short flag)
{
	kuid_t euid = current_euid();
	int requested_mode, granted_mode;

	audit_ipc_obj(ipcp);
	requested_mode = (flag >> 6) | (flag >> 3) | flag;
	granted_mode = ipcp->mode;
	if (uid_eq(euid, ipcp->cuid) ||
	    uid_eq(euid, ipcp->uid))
		granted_mode >>= 6;
	else if (in_group_p(ipcp->cgid) || in_group_p(ipcp->gid))
		granted_mode >>= 3;
	/*                                                                  */
	if ((requested_mode & ~granted_mode & 0007) && 
	    !ns_capable(ns->user_ns, CAP_IPC_OWNER))
		return -1;

	return security_ipc_permission(ipcp, flag);
}

/*
                                                                   
                              
 */

/* 
                                                                
                          
                                  
  
                                                                    
                                     
 */
 

void kernel_to_ipc64_perm (struct kern_ipc_perm *in, struct ipc64_perm *out)
{
	out->key	= in->key;
	out->uid	= from_kuid_munged(current_user_ns(), in->uid);
	out->gid	= from_kgid_munged(current_user_ns(), in->gid);
	out->cuid	= from_kuid_munged(current_user_ns(), in->cuid);
	out->cgid	= from_kgid_munged(current_user_ns(), in->cgid);
	out->mode	= in->mode;
	out->seq	= in->seq;
}

/* 
                                                              
                                 
                                  
  
                                                                 
                                             
 */
 
void ipc64_perm_to_ipc_perm (struct ipc64_perm *in, struct ipc_perm *out)
{
	out->key	= in->key;
	SET_UID(out->uid, in->uid);
	SET_GID(out->gid, in->gid);
	SET_UID(out->cuid, in->cuid);
	SET_GID(out->cgid, in->cgid);
	out->mode	= in->mode;
	out->seq	= in->seq;
}

/* 
                    
                           
                          
  
                                                                      
  
                                        
                                          
 */
struct kern_ipc_perm *ipc_obtain_object(struct ipc_ids *ids, int id)
{
	struct kern_ipc_perm *out;
	int lid = ipcid_to_idx(id);

	out = idr_find(&ids->ipcs_idr, lid);
	if (!out)
		return ERR_PTR(-EINVAL);

	return out;
}

/* 
                                                      
                           
                          
  
                                                                        
  
                                               
 */
struct kern_ipc_perm *ipc_lock(struct ipc_ids *ids, int id)
{
	struct kern_ipc_perm *out;

	rcu_read_lock();
	out = ipc_obtain_object(ids, id);
	if (IS_ERR(out))
		goto err1;

	spin_lock(&out->lock);

	/*                                                        
                                                               
  */
	if (!out->deleted)
		return out;

	spin_unlock(&out->lock);
	out = ERR_PTR(-EINVAL);
err1:
	rcu_read_unlock();
	return out;
}

/* 
                          
                           
                          
  
                                                 
                                    
  
                                        
                                          
 */
struct kern_ipc_perm *ipc_obtain_object_check(struct ipc_ids *ids, int id)
{
	struct kern_ipc_perm *out = ipc_obtain_object(ids, id);

	if (IS_ERR(out))
		goto out;

	if (ipc_checkid(out, id))
		return ERR_PTR(-EIDRM);
out:
	return out;
}

/* 
                                  
                  
                            
                                                                           
                            
                                                              
  
                                                                        
 */
int ipcget(struct ipc_namespace *ns, struct ipc_ids *ids,
			struct ipc_ops *ops, struct ipc_params *params)
{
	if (params->key == IPC_PRIVATE)
		return ipcget_new(ns, ids, ops, params);
	else
		return ipcget_public(ns, ids, ops, params);
}

/* 
                                                      
                                       
                                          
 */
int ipc_update_perm(struct ipc64_perm *in, struct kern_ipc_perm *out)
{
	kuid_t uid = make_kuid(current_user_ns(), in->uid);
	kgid_t gid = make_kgid(current_user_ns(), in->gid);
	if (!uid_valid(uid) || !gid_valid(gid))
		return -EINVAL;

	out->uid = uid;
	out->gid = gid;
	out->mode = (out->mode & ~S_IRWXUGO)
		| (in->mode & S_IRWXUGO);

	return 0;
}

/* 
                                                                                      
                          
                                                    
                                       
                          
                               
                                                          
  
                                                                              
                                                                   
                                              
                                                             
                                                                          
                                                                                
  
                                                         
 */
struct kern_ipc_perm *ipcctl_pre_down_nolock(struct ipc_namespace *ns,
					struct ipc_ids *ids, int id, int cmd,
					struct ipc64_perm *perm, int extra_perm)
{
	kuid_t euid;
	int err = -EPERM;
	struct kern_ipc_perm *ipcp;

	ipcp = ipc_obtain_object_check(ids, id);
	if (IS_ERR(ipcp)) {
		err = PTR_ERR(ipcp);
		goto err;
	}

	audit_ipc_obj(ipcp);
	if (cmd == IPC_SET)
		audit_ipc_set_perm(extra_perm, perm->uid,
				   perm->gid, perm->mode);

	euid = current_euid();
	if (uid_eq(euid, ipcp->cuid) || uid_eq(euid, ipcp->uid)  ||
	    ns_capable(ns->user_ns, CAP_SYS_ADMIN))
		return ipcp; /*                   */
err:
	return ERR_PTR(err);
}

#ifdef CONFIG_ARCH_WANT_IPC_PARSE_VERSION


/* 
                                       
                           
  
                                                                  
                                                                     
                         
 */
 
int ipc_parse_version (int *cmd)
{
	if (*cmd & IPC_64) {
		*cmd ^= IPC_64;
		return IPC_64;
	} else {
		return IPC_OLD;
	}
}

#endif /*                                    */

#ifdef CONFIG_PROC_FS
struct ipc_proc_iter {
	struct ipc_namespace *ns;
	struct ipc_proc_iface *iface;
};

/*
                                                                       
 */
static struct kern_ipc_perm *sysvipc_find_ipc(struct ipc_ids *ids, loff_t pos,
					      loff_t *new_pos)
{
	struct kern_ipc_perm *ipc;
	int total, id;

	total = 0;
	for (id = 0; id < pos && total < ids->in_use; id++) {
		ipc = idr_find(&ids->ipcs_idr, id);
		if (ipc != NULL)
			total++;
	}

	if (total >= ids->in_use)
		return NULL;

	for ( ; pos < IPCMNI; pos++) {
		ipc = idr_find(&ids->ipcs_idr, pos);
		if (ipc != NULL) {
			*new_pos = pos + 1;
			rcu_read_lock();
			ipc_lock_object(ipc);
			return ipc;
		}
	}

	/*                                                   */
	return NULL;
}

static void *sysvipc_proc_next(struct seq_file *s, void *it, loff_t *pos)
{
	struct ipc_proc_iter *iter = s->private;
	struct ipc_proc_iface *iface = iter->iface;
	struct kern_ipc_perm *ipc = it;

	/*                                              */
	if (ipc && ipc != SEQ_START_TOKEN)
		ipc_unlock(ipc);

	return sysvipc_find_ipc(&iter->ns->ids[iface->ids], *pos, pos);
}

/*
                                                            
                                                                          
 */
static void *sysvipc_proc_start(struct seq_file *s, loff_t *pos)
{
	struct ipc_proc_iter *iter = s->private;
	struct ipc_proc_iface *iface = iter->iface;
	struct ipc_ids *ids;

	ids = &iter->ns->ids[iface->ids];

	/*
                                                              
                   
  */
	down_read(&ids->rwsem);

	/*                    */
	if (*pos < 0)
		return NULL;

	/*                       */
	if (*pos == 0)
		return SEQ_START_TOKEN;

	/*                        */
	return sysvipc_find_ipc(ids, *pos - 1, pos);
}

static void sysvipc_proc_stop(struct seq_file *s, void *it)
{
	struct kern_ipc_perm *ipc = it;
	struct ipc_proc_iter *iter = s->private;
	struct ipc_proc_iface *iface = iter->iface;
	struct ipc_ids *ids;

	/*                                          */
	if (ipc && ipc != SEQ_START_TOKEN)
		ipc_unlock(ipc);

	ids = &iter->ns->ids[iface->ids];
	/*                                     */
	up_read(&ids->rwsem);
}

static int sysvipc_proc_show(struct seq_file *s, void *it)
{
	struct ipc_proc_iter *iter = s->private;
	struct ipc_proc_iface *iface = iter->iface;

	if (it == SEQ_START_TOKEN)
		return seq_puts(s, iface->header);

	return iface->show(s, it);
}

static const struct seq_operations sysvipc_proc_seqops = {
	.start = sysvipc_proc_start,
	.stop  = sysvipc_proc_stop,
	.next  = sysvipc_proc_next,
	.show  = sysvipc_proc_show,
};

static int sysvipc_proc_open(struct inode *inode, struct file *file)
{
	int ret;
	struct seq_file *seq;
	struct ipc_proc_iter *iter;

	ret = -ENOMEM;
	iter = kmalloc(sizeof(*iter), GFP_KERNEL);
	if (!iter)
		goto out;

	ret = seq_open(file, &sysvipc_proc_seqops);
	if (ret)
		goto out_kfree;

	seq = file->private_data;
	seq->private = iter;

	iter->iface = PDE_DATA(inode);
	iter->ns    = get_ipc_ns(current->nsproxy->ipc_ns);
out:
	return ret;
out_kfree:
	kfree(iter);
	goto out;
}

static int sysvipc_proc_release(struct inode *inode, struct file *file)
{
	struct seq_file *seq = file->private_data;
	struct ipc_proc_iter *iter = seq->private;
	put_ipc_ns(iter->ns);
	return seq_release_private(inode, file);
}

static const struct file_operations sysvipc_proc_fops = {
	.open    = sysvipc_proc_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = sysvipc_proc_release,
};
#endif /*                */
