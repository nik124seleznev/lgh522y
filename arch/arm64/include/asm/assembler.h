/*
 * Based on arch/arm/include/asm/assembler.h
 *
 * Copyright (C) 1996-2000 Russell King
 * Copyright (C) 2012 ARM Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef __ASSEMBLY__
#error "Only include this from assembly code"
#endif

#include <asm/ptrace.h>

/*
                                                                             
                                
 */
	.macro	push, xreg1, xreg2
	stp	\xreg1, \xreg2, [sp, #-16]!
	.endm

	.macro	pop, xreg1, xreg2
	ldp	\xreg1, \xreg2, [sp], #16
	.endm

/*
                                 
 */
	.macro	disable_irq
	msr	daifset, #2
	.endm

	.macro	enable_irq
	msr	daifclr, #2
	.endm

/*
                                       
 */
	.macro	save_and_disable_irqs, olddaif
	mrs	\olddaif, daif
	disable_irq
	.endm

	.macro	restore_irqs, olddaif
	msr	daif, \olddaif
	.endm

/*
                                       
 */
	.macro	disable_dbg
	msr	daifset, #8
	.endm

	.macro	enable_dbg
	msr	daifclr, #8
	.endm

	.macro	disable_step, tmp
	mrs	\tmp, mdscr_el1
	bic	\tmp, \tmp, #1
	msr	mdscr_el1, \tmp
	.endm

	.macro	enable_step, tmp
	mrs	\tmp, mdscr_el1
	orr	\tmp, \tmp, #1
	msr	mdscr_el1, \tmp
	.endm

	.macro	enable_dbg_if_not_stepping, tmp
	mrs	\tmp, mdscr_el1
	tbnz	\tmp, #0, 9990f
	enable_dbg
9990:
	.endm

/*
                          
 */
	.macro	smp_dmb, opt
#ifdef CONFIG_SMP
	dmb	\opt
#endif
	.endm

#define USER(l, x...)				\
9999:	x;					\
	.section __ex_table,"a";		\
	.align	3;				\
	.quad	9999b,l;			\
	.previous

/*
                    
 */
lr	.req	x30		//              

/*
               
 */
	 .macro	ventry	label
	.align	7
	b	\label
	.endm

/*
                                      
 */
#ifdef CONFIG_CPU_BIG_ENDIAN
#define CPU_BE(code...) code
#else
#define CPU_BE(code...)
#endif

/*
                                      
 */
#ifdef CONFIG_CPU_BIG_ENDIAN
#define CPU_LE(code...)
#else
#define CPU_LE(code...) code
#endif

/*
                                                                     
                                                                     
                        
 */
#ifndef CONFIG_CPU_BIG_ENDIAN
	.macro	regs_to_64, rd, lbits, hbits
#else
	.macro	regs_to_64, rd, hbits, lbits
#endif
	orr	\rd, \lbits, \hbits, lsl #32
	.endm