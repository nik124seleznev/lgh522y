/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 1995, 96, 97, 98, 99, 2003 by Ralf Baechle
 * Copyright (C) 1999 Silicon Graphics, Inc.
 */
#ifndef _UAPI_ASM_SIGNAL_H
#define _UAPI_ASM_SIGNAL_H

#include <linux/types.h>

#define _NSIG		128
#define _NSIG_BPW	(sizeof(unsigned long) * 8)
#define _NSIG_WORDS	(_NSIG / _NSIG_BPW)

typedef struct {
	unsigned long sig[_NSIG_WORDS];
} sigset_t;

typedef unsigned long old_sigset_t;		/*                  */

#define SIGHUP		 1	/*                  */
#define SIGINT		 2	/*                    */
#define SIGQUIT		 3	/*                */
#define SIGILL		 4	/*                             */
#define SIGTRAP		 5	/*                     */
#define SIGIOT		 6	/* IOT trap (4.2 BSD).	*/
#define SIGABRT		 SIGIOT /*                */
#define SIGEMT		 7
#define SIGFPE		 8	/*                                   */
#define SIGKILL		 9	/*                             */
#define SIGBUS		10	/* BUS error (4.2 BSD).	 */
#define SIGSEGV		11	/*                                 */
#define SIGSYS		12
#define SIGPIPE		13	/*                       */
#define SIGALRM		14	/*                       */
#define SIGTERM		15	/*                     */
#define SIGUSR1		16	/*                                 */
#define SIGUSR2		17	/*                                 */
#define SIGCHLD		18	/*                                    */
#define SIGCLD		SIGCHLD /*                             */
#define SIGPWR		19	/*                                    */
#define SIGWINCH	20	/* Window size change (4.3 BSD, Sun).  */
#define SIGURG		21	/* Urgent condition on socket (4.2 BSD).  */
#define SIGIO		22	/* I/O now possible (4.2 BSD).	*/
#define SIGPOLL		SIGIO	/*                                     */
#define SIGSTOP		23	/*                             */
#define SIGTSTP		24	/*                         */
#define SIGCONT		25	/*                    */
#define SIGTTIN		26	/*                                    */
#define SIGTTOU		27	/*                                   */
#define SIGVTALRM	28	/* Virtual alarm clock (4.2 BSD).  */
#define SIGPROF		29	/* Profiling alarm clock (4.2 BSD).  */
#define SIGXCPU		30	/* CPU limit exceeded (4.2 BSD).  */
#define SIGXFSZ		31	/* File size limit exceeded (4.2 BSD).	*/

/*                                                          */
#define SIGRTMIN	32
#define SIGRTMAX	_NSIG

/*
                   
  
                                                               
                                                                              
                                                            
                                                                
                                                   
                                                                           
  
                                                                         
                                                 
  
                                                                         
                                                                        
                                                                       
                                                                   
                                    
 */
#define SA_ONSTACK	0x08000000
#define SA_RESETHAND	0x80000000
#define SA_RESTART	0x10000000
#define SA_SIGINFO	0x00000008
#define SA_NODEFER	0x40000000
#define SA_NOCLDWAIT	0x00010000
#define SA_NOCLDSTOP	0x00000001

#define SA_NOMASK	SA_NODEFER
#define SA_ONESHOT	SA_RESETHAND

#define MINSIGSTKSZ    2048
#define SIGSTKSZ       8192


#define SIG_BLOCK	1	/*                      */
#define SIG_UNBLOCK	2	/*                        */
#define SIG_SETMASK	3	/*                             */

#include <asm-generic/signal-defs.h>

#ifndef __KERNEL__
struct sigaction {
	unsigned int	sa_flags;
	__sighandler_t	sa_handler;
	sigset_t	sa_mask;
};
#endif

/*                          */
typedef struct sigaltstack {
	void __user *ss_sp;
	size_t ss_size;
	int ss_flags;
} stack_t;


#endif /*                    */