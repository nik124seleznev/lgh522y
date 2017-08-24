#ifndef _XTENSA_UNISTD_H
#define _XTENSA_UNISTD_H

#define __ARCH_WANT_SYS_CLONE
#include <uapi/asm/unistd.h>

#define __ARCH_WANT_STAT64
#define __ARCH_WANT_SYS_UTIME
#define __ARCH_WANT_SYS_LLSEEK
#define __ARCH_WANT_SYS_GETPGRP

/* 
                                                            
 */

#define __IGNORE_fork				/*           */
#define __IGNORE_time
#define __IGNORE_alarm				/*               */
#define __IGNORE_pause
#define __IGNORE_mmap				/*           */
#define __IGNORE_vfork				/*           */
#define __IGNORE_fadvise64			/*                  */

#endif /*                  */
