/*
                
  
                                            
 */
#ifndef _ASMS390_SIGNAL_H
#define _ASMS390_SIGNAL_H

#include <uapi/asm/signal.h>

/*                                                                     
                                 */
#include <asm/sigcontext.h>
#define _NSIG           _SIGCONTEXT_NSIG
#define _NSIG_BPW       _SIGCONTEXT_NSIG_BPW
#define _NSIG_WORDS     _SIGCONTEXT_NSIG_WORDS

typedef unsigned long old_sigset_t;             /*                  */

typedef struct {
        unsigned long sig[_NSIG_WORDS];
} sigset_t;

#define __ARCH_HAS_SA_RESTORER
#endif
