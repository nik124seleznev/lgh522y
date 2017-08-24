/*
 * Copyright 2010 Tilera Corporation. All Rights Reserved.
 *
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public License
 *   as published by the Free Software Foundation, version 2.
 *
 *   This program is distributed in the hope that it will be useful, but
 *   WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, GOOD TITLE or
 *   NON INFRINGEMENT.  See the GNU General Public License for
 *   more details.
 */

#ifndef _UAPI_ASM_TILE_PTRACE_H
#define _UAPI_ASM_TILE_PTRACE_H

#include <arch/chip.h>
#include <arch/abi.h>

/*                                         */
#if CHIP_WORD_SIZE() == 32
#define PTREGS_OFFSET_REG(n)    ((n)*4)
#else
#define PTREGS_OFFSET_REG(n)    ((n)*8)
#endif
#define PTREGS_OFFSET_BASE      0
#define PTREGS_OFFSET_TP        PTREGS_OFFSET_REG(53)
#define PTREGS_OFFSET_SP        PTREGS_OFFSET_REG(54)
#define PTREGS_OFFSET_LR        PTREGS_OFFSET_REG(55)
#define PTREGS_NR_GPRS          56
#define PTREGS_OFFSET_PC        PTREGS_OFFSET_REG(56)
#define PTREGS_OFFSET_EX1       PTREGS_OFFSET_REG(57)
#define PTREGS_OFFSET_FAULTNUM  PTREGS_OFFSET_REG(58)
#define PTREGS_OFFSET_ORIG_R0   PTREGS_OFFSET_REG(59)
#define PTREGS_OFFSET_FLAGS     PTREGS_OFFSET_REG(60)
#if CHIP_HAS_CMPEXCH()
#define PTREGS_OFFSET_CMPEXCH   PTREGS_OFFSET_REG(61)
#endif
#define PTREGS_SIZE             PTREGS_OFFSET_REG(64)


#ifndef __ASSEMBLY__

#ifndef __KERNEL__
/*                                                                       */
typedef uint_reg_t pt_reg_t;
#endif

/*
                                                                             
                                                                     
 */
struct pt_regs {
	/*                                                     */
	/*                                                             */
	pt_reg_t regs[53];
	pt_reg_t tp;		/*                       */
	pt_reg_t sp;		/*                       */
	pt_reg_t lr;		/*                       */

	/*                          */
	pt_reg_t pc;		/*                          */
	pt_reg_t ex1;		/*                                           */
	pt_reg_t faultnum;	/*                                        */
	pt_reg_t orig_r0;	/*                                */
	pt_reg_t flags;		/*                   */
#if !CHIP_HAS_CMPEXCH()
	pt_reg_t pad[3];
#else
	pt_reg_t cmpexch;	/*                                         */
	pt_reg_t pad[2];
#endif
};

#endif /*              */

#define PTRACE_GETREGS		12
#define PTRACE_SETREGS		13
#define PTRACE_GETFPREGS	14
#define PTRACE_SETFPREGS	15

/*                                                                   */
#define PTRACE_EVENT_MIGRATE	16
#define PTRACE_O_TRACEMIGRATE	(1 << PTRACE_EVENT_MIGRATE)

/*
                                                              
                                                               
                                                           
 */
#define PT_FLAGS_COMPAT		0x10000  /*                                   */

#endif /*                         */
