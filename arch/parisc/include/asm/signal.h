#ifndef _ASM_PARISC_SIGNAL_H
#define _ASM_PARISC_SIGNAL_H

#include <uapi/asm/signal.h>

#define _NSIG		64
/*                                                             */
#define _NSIG_BPW	BITS_PER_LONG
#define _NSIG_WORDS	(_NSIG / _NSIG_BPW)

# ifndef __ASSEMBLY__

/*                                                                     
                                 */

typedef unsigned long old_sigset_t;		/*                  */

typedef struct {
	/*                                                  */
	unsigned long sig[_NSIG_WORDS];
} sigset_t;

#ifndef __KERNEL__
struct sigaction {
	__sighandler_t sa_handler;
	unsigned long sa_flags;
	sigset_t sa_mask;		/*                             */
};
#endif

#include <asm/sigcontext.h>

#endif /*             */
#endif /*                      */
