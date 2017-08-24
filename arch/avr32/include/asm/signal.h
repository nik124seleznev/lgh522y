/*
 * Copyright (C) 2004-2006 Atmel Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef __ASM_AVR32_SIGNAL_H
#define __ASM_AVR32_SIGNAL_H

#include <uapi/asm/signal.h>

/*                                                                     
                                 */

#define _NSIG		64
#define _NSIG_BPW	32
#define _NSIG_WORDS	(_NSIG / _NSIG_BPW)

typedef unsigned long old_sigset_t;		/*                  */

typedef struct {
	unsigned long sig[_NSIG_WORDS];
} sigset_t;

#define __ARCH_HAS_SA_RESTORER

#include <asm/sigcontext.h>
#undef __HAVE_ARCH_SIG_BITOPS

#endif
