#ifndef _ASM_UNISTD_H_
#define _ASM_UNISTD_H_

#include <uapi/asm/unistd.h>


#define NR_syscalls 338

/*                                 */
#define __ARCH_WANT_OLD_STAT
#define __ARCH_WANT_STAT64
#define __ARCH_WANT_SYS_ALARM
/*                                     */
#define __ARCH_WANT_SYS_IPC
#define __ARCH_WANT_SYS_PAUSE
/*                                  */
/*                                */
#define __ARCH_WANT_SYS_TIME
#define __ARCH_WANT_SYS_UTIME
#define __ARCH_WANT_SYS_WAITPID
#define __ARCH_WANT_SYS_SOCKETCALL
#define __ARCH_WANT_SYS_FADVISE64
#define __ARCH_WANT_SYS_GETPGRP
#define __ARCH_WANT_SYS_LLSEEK
#define __ARCH_WANT_SYS_NICE
/*                                       */
#define __ARCH_WANT_SYS_OLDUMOUNT
/*                                    */
#define __ARCH_WANT_SYS_SIGPROCMASK
#define __ARCH_WANT_SYS_FORK
#define __ARCH_WANT_SYS_VFORK
#define __ARCH_WANT_SYS_CLONE

#endif /*                */
