#ifndef _PARISC_SHMBUF_H
#define _PARISC_SHMBUF_H

/* 
                                                    
                                                                     
                                 
  
                         
                                         
                                  
 */

struct shmid64_ds {
	struct ipc64_perm	shm_perm;	/*                 */
#ifndef CONFIG_64BIT
	unsigned int		__pad1;
#endif
	__kernel_time_t		shm_atime;	/*                  */
#ifndef CONFIG_64BIT
	unsigned int		__pad2;
#endif
	__kernel_time_t		shm_dtime;	/*                  */
#ifndef CONFIG_64BIT
	unsigned int		__pad3;
#endif
	__kernel_time_t		shm_ctime;	/*                  */
#ifndef CONFIG_64BIT
	unsigned int		__pad4;
#endif
	size_t			shm_segsz;	/*                         */
	__kernel_pid_t		shm_cpid;	/*                */
	__kernel_pid_t		shm_lpid;	/*                      */
	unsigned int		shm_nattch;	/*                         */
	unsigned int		__unused1;
	unsigned int		__unused2;
};

struct shminfo64 {
	unsigned long	shmmax;
	unsigned long	shmmin;
	unsigned long	shmmni;
	unsigned long	shmseg;
	unsigned long	shmall;
	unsigned long	__unused1;
	unsigned long	__unused2;
	unsigned long	__unused3;
	unsigned long	__unused4;
};

#endif /* _PARISC_SHMBUF_H */
