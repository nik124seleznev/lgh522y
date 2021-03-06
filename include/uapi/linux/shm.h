#ifndef _UAPI_LINUX_SHM_H_
#define _UAPI_LINUX_SHM_H_

#include <linux/ipc.h>
#include <linux/errno.h>
#ifndef __KERNEL__
#include <unistd.h>
#endif

/*
                                                                    
                         
 */

#define SHMMAX 0x2000000		 /*                             */
#define SHMMIN 1			 /*                             */
#define SHMMNI 4096			 /*                             */
#ifndef __KERNEL__
#define SHMALL (SHMMAX/getpagesize()*(SHMMNI/16))
#endif
#define SHMSEG SHMMNI			 /*                             */


/*                                                                    */
struct shmid_ds {
	struct ipc_perm		shm_perm;	/*                 */
	int			shm_segsz;	/*                         */
	__kernel_time_t		shm_atime;	/*                  */
	__kernel_time_t		shm_dtime;	/*                  */
	__kernel_time_t		shm_ctime;	/*                  */
	__kernel_ipc_pid_t	shm_cpid;	/*                */
	__kernel_ipc_pid_t	shm_lpid;	/*                      */
	unsigned short		shm_nattch;	/*                         */
	unsigned short 		shm_unused;	/*               */
	void 			*shm_unused2;	/*                      */
	void			*shm_unused3;	/*        */
};

/*                                                    */
#include <asm/shmbuf.h>

/*                            */
#define SHM_R		0400	/*                                */
#define SHM_W		0200	/*                                */

/*                 */
#define	SHM_RDONLY	010000	/*                  */
#define	SHM_RND		020000	/*                                         */
#define	SHM_REMAP	040000	/*                            */
#define	SHM_EXEC	0100000	/*                  */

/*                            */
#define SHM_LOCK 	11
#define SHM_UNLOCK 	12

/*                   */
#define SHM_STAT 	13
#define SHM_INFO 	14

/*                                                 */
struct	shminfo {
	int shmmax;
	int shmmin;
	int shmmni;
	int shmseg;
	int shmall;
};

struct shm_info {
	int used_ids;
	unsigned long shm_tot;	/*                     */
	unsigned long shm_rss;	/*                    */
	unsigned long shm_swp;	/*                   */
	unsigned long swap_attempts;
	unsigned long swap_successes;
};


#endif /*                    */
