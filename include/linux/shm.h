#ifndef _LINUX_SHM_H_
#define _LINUX_SHM_H_

#include <asm/page.h>
#include <uapi/linux/shm.h>

#define SHMALL (SHMMAX/PAGE_SIZE*(SHMMNI/16)) /*                             */
#include <asm/shmparam.h>
struct shmid_kernel /*                       */
{	
	struct kern_ipc_perm	shm_perm;
	struct file *		shm_file;
	unsigned long		shm_nattch;
	unsigned long		shm_segsz;
	time_t			shm_atim;
	time_t			shm_dtim;
	time_t			shm_ctim;
	pid_t			shm_cprid;
	pid_t			shm_lprid;
	struct user_struct	*mlock_user;

	/*                                                             */
	struct task_struct	*shm_creator;
};

/*                           */
#define	SHM_DEST	01000	/*                                          */
#define SHM_LOCKED      02000   /*                             */
#define SHM_HUGETLB     04000   /*                                 */
#define SHM_NORESERVE   010000  /*                              */

/*                           */

/*
                                                                              
                                                                              
          
  
                                      
                                    
 */
#define SHM_HUGE_SHIFT  26
#define SHM_HUGE_MASK   0x3f
#define SHM_HUGE_2MB    (21 << SHM_HUGE_SHIFT)
#define SHM_HUGE_1GB    (30 << SHM_HUGE_SHIFT)

#ifdef CONFIG_SYSVIPC
long do_shmat(int shmid, char __user *shmaddr, int shmflg, unsigned long *addr,
	      unsigned long shmlba);
extern int is_file_shm_hugepages(struct file *file);
extern void exit_shm(struct task_struct *task);
#else
static inline long do_shmat(int shmid, char __user *shmaddr,
			    int shmflg, unsigned long *addr,
			    unsigned long shmlba)
{
	return -ENOSYS;
}
static inline int is_file_shm_hugepages(struct file *file)
{
	return 0;
}
static inline void exit_shm(struct task_struct *task)
{
}
#endif

#endif /*               */
