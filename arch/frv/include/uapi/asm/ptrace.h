/* ptrace.h: ptrace() relevant definitions
 *
 * Copyright (C) 2003 Red Hat, Inc. All Rights Reserved.
 * Written by David Howells (dhowells@redhat.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#ifndef _UAPI_ASM_PTRACE_H
#define _UAPI_ASM_PTRACE_H

#include <asm/registers.h>


#define PT_PSR		0
#define	PT_ISR		1
#define PT_CCR		2
#define PT_CCCR		3
#define PT_LR		4
#define PT_LCR		5
#define PT_PC		6

#define PT__STATUS	7	/*                  */
#define PT_SYSCALLNO	8	/*                      */
#define PT_ORIG_GR8	9	/*                               */
#define PT_GNER0	10
#define PT_GNER1	11
#define PT_IACC0H	12
#define PT_IACC0L	13

#define PT_GR(j)	( 14 + (j))	/*                  */
#define PT_FR(j)	( 78 + (j))	/*                  */
#define PT_FNER(j)	(142 + (j))	/*                   */
#define PT_MSR(j)	(144 + (j))	/*                  */
#define PT_ACC(j)	(146 + (j))	/*                  */
#define PT_ACCG(jklm)	(154 + (jklm))	/*                                                    */
#define PT_FSR(j)	(156 + (j))	/*                  */
#define PT__GPEND	78
#define PT__END		157

#define PT_TBR		PT_GR(0)
#define PT_SP		PT_GR(1)
#define PT_FP		PT_GR(2)
#define PT_PREV_FRAME	PT_GR(28)	/*                                                   */
#define PT_CURR_TASK	PT_GR(29)	/*              */


/*                                                                       */
#define PTRACE_GETREGS		12
#define PTRACE_SETREGS		13
#define PTRACE_GETFPREGS	14
#define PTRACE_SETFPREGS	15
#define PTRACE_GETFDPIC		31	/*                                   */

#define PTRACE_GETFDPIC_EXEC	0	/*                                       */
#define PTRACE_GETFDPIC_INTERP	1	/*                                        */

#endif /*                    */
