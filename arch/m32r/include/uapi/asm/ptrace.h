/*
 * linux/include/asm-m32r/ptrace.h
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * M32R version:
 *   Copyright (C) 2001-2002, 2004  Hirokazu Takata <takata at linux-m32r.org>
 */
#ifndef _UAPI_ASM_M32R_PTRACE_H
#define _UAPI_ASM_M32R_PTRACE_H


/*                                                            */
#define PT_R4		0
#define PT_R5		1
#define PT_R6		2
#define PT_REGS 	3
#define PT_R0		4
#define PT_R1		5
#define PT_R2		6
#define PT_R3		7
#define PT_R7		8
#define PT_R8		9
#define PT_R9		10
#define PT_R10		11
#define PT_R11		12
#define PT_R12		13
#define PT_SYSCNR	14
#define PT_R13		PT_FP
#define PT_R14		PT_LR
#define PT_R15		PT_SP

/*                                                        */
#define PT_ACC0H	15
#define PT_ACC0L	16
#define PT_ACC1H	17	/*                     */
#define PT_ACC1L	18	/*                     */
#define PT_PSW		19
#define PT_BPC		20
#define PT_BBPSW	21
#define PT_BBPC		22
#define PT_SPU		23
#define PT_FP		24
#define PT_LR		25
#define PT_SPI		26
#define PT_ORIGR0	27

/*                              */
#define PT_PC		30
#define PT_CBR		31
#define PT_EVB		32


/*                     */
#define SPR_CR0 PT_PSW
#define SPR_CR1 PT_CBR		/*           */
#define SPR_CR2 PT_SPI
#define SPR_CR3 PT_SPU
#define SPR_CR4
#define SPR_CR5 PT_EVB		/*                                  */
#define SPR_CR6 PT_BPC
#define SPR_CR7
#define SPR_CR8 PT_BBPSW
#define SPR_CR9
#define SPR_CR10
#define SPR_CR11
#define SPR_CR12
#define SPR_CR13 PT_WR
#define SPR_CR14 PT_BBPC
#define SPR_CR15

/*                                                            
                               */
struct pt_regs {
	/*                                 */
	unsigned long r4;
	unsigned long r5;
	unsigned long r6;
	struct pt_regs *pt_regs;
	unsigned long r0;
	unsigned long r1;
	unsigned long r2;
	unsigned long r3;
	unsigned long r7;
	unsigned long r8;
	unsigned long r9;
	unsigned long r10;
	unsigned long r11;
	unsigned long r12;
	long syscall_nr;

	/*                                                                  */
	unsigned long acc0h;
	unsigned long acc0l;
	unsigned long acc1h;	/*                     */
	unsigned long acc1l;	/*                     */
	unsigned long psw;
	unsigned long bpc;		/*                            */
	unsigned long bbpsw;
	unsigned long bbpc;
	unsigned long spu;		/*                  */
	unsigned long fp;
	unsigned long lr;		/*                          */
	unsigned long spi;		/*                    */
	unsigned long orig_r0;
};

/*                                                                       */
#define PTRACE_GETREGS		12
#define PTRACE_SETREGS		13

#define PTRACE_OLDSETOPTIONS	21


#endif /*                         */
