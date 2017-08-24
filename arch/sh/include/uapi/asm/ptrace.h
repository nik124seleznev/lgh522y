/*
 * Copyright (C) 1999, 2000  Niibe Yutaka
 */
#ifndef _UAPI__ASM_SH_PTRACE_H
#define _UAPI__ASM_SH_PTRACE_H


#define PTRACE_GETREGS		12	/*                   */
#define PTRACE_SETREGS		13

#define PTRACE_GETFPREGS	14	/*               */
#define PTRACE_SETFPREGS	15

#define PTRACE_GETFDPIC		31	/*                                   */

#define PTRACE_GETFDPIC_EXEC	0	/*                                       */
#define PTRACE_GETFDPIC_INTERP	1	/*                                        */

#define	PTRACE_GETDSPREGS	55	/*               */
#define	PTRACE_SETDSPREGS	56

#define PT_TEXT_END_ADDR	240
#define PT_TEXT_ADDR		244	/*                            */
#define PT_DATA_ADDR		248	/*                            */
#define PT_TEXT_LEN		252

#if defined(__SH5__) || defined(CONFIG_CPU_SH5)
#include <asm/ptrace_64.h>
#else
#include <asm/ptrace_32.h>
#endif


#endif /*                        */
