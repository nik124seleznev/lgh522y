#ifndef _ASM_X86_SIGCONTEXT_H
#define _ASM_X86_SIGCONTEXT_H

#include <uapi/asm/sigcontext.h>

#ifdef __i386__
struct sigcontext {
	unsigned short gs, __gsh;
	unsigned short fs, __fsh;
	unsigned short es, __esh;
	unsigned short ds, __dsh;
	unsigned long di;
	unsigned long si;
	unsigned long bp;
	unsigned long sp;
	unsigned long bx;
	unsigned long dx;
	unsigned long cx;
	unsigned long ax;
	unsigned long trapno;
	unsigned long err;
	unsigned long ip;
	unsigned short cs, __csh;
	unsigned long flags;
	unsigned long sp_at_signal;
	unsigned short ss, __ssh;

	/*
                                                               
                                                                
                                                                      
                                                                
                          
  */
	void __user *fpstate;		/*                                   */
	unsigned long oldmask;
	unsigned long cr2;
};
#else /*          */
struct sigcontext {
	unsigned long r8;
	unsigned long r9;
	unsigned long r10;
	unsigned long r11;
	unsigned long r12;
	unsigned long r13;
	unsigned long r14;
	unsigned long r15;
	unsigned long di;
	unsigned long si;
	unsigned long bp;
	unsigned long bx;
	unsigned long dx;
	unsigned long ax;
	unsigned long cx;
	unsigned long sp;
	unsigned long ip;
	unsigned long flags;
	unsigned short cs;
	unsigned short gs;
	unsigned short fs;
	unsigned short __pad0;
	unsigned long err;
	unsigned long trapno;
	unsigned long oldmask;
	unsigned long cr2;

	/*
                                                               
                                                                
                                                                      
                                                                
                          
  */
	void __user *fpstate;		/*                                   */
	unsigned long reserved1[8];
};
#endif /*           */
#endif /*                       */
