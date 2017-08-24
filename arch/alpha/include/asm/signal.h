#ifndef _ASMAXP_SIGNAL_H
#define _ASMAXP_SIGNAL_H

#include <uapi/asm/signal.h>

/*                                                                     
                                                                   */

#define _NSIG		64
#define _NSIG_BPW	64
#define _NSIG_WORDS	(_NSIG / _NSIG_BPW)

typedef unsigned long old_sigset_t;		/*                  */

typedef struct {
	unsigned long sig[_NSIG_WORDS];
} sigset_t;

struct osf_sigaction {
	__sighandler_t	sa_handler;
	old_sigset_t	sa_mask;
	int		sa_flags;
};

#define __ARCH_HAS_KA_RESTORER
#include <asm/sigcontext.h>
#endif