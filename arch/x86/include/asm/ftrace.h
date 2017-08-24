#ifndef _ASM_X86_FTRACE_H
#define _ASM_X86_FTRACE_H

#ifdef __ASSEMBLY__

	/*                                                         */
	.macro MCOUNT_SAVE_FRAME skip=0
	 /*
                                          
   */
	subq $(SS+8-\skip), %rsp
	movq %rax, RAX(%rsp)
	movq %rcx, RCX(%rsp)
	movq %rdx, RDX(%rsp)
	movq %rsi, RSI(%rsp)
	movq %rdi, RDI(%rsp)
	movq %r8, R8(%rsp)
	movq %r9, R9(%rsp)
	 /*                                 */
	movq SS+8(%rsp), %rdx
	movq %rdx, RIP(%rsp)
	.endm

	.macro MCOUNT_RESTORE_FRAME skip=0
	movq R9(%rsp), %r9
	movq R8(%rsp), %r8
	movq RDI(%rsp), %rdi
	movq RSI(%rsp), %rsi
	movq RDX(%rsp), %rdx
	movq RCX(%rsp), %rcx
	movq RAX(%rsp), %rax
	addq $(SS+8-\skip), %rsp
	.endm

#endif

#ifdef CONFIG_FUNCTION_TRACER
#ifdef CC_USING_FENTRY
# define MCOUNT_ADDR		((long)(__fentry__))
#else
# define MCOUNT_ADDR		((long)(mcount))
#endif
#define MCOUNT_INSN_SIZE	5 /*                    */

#ifdef CONFIG_DYNAMIC_FTRACE
#define ARCH_SUPPORTS_FTRACE_OPS 1
#endif

#ifndef __ASSEMBLY__
extern void mcount(void);
extern atomic_t modifying_ftrace_code;
extern void __fentry__(void);

static inline unsigned long ftrace_call_adjust(unsigned long addr)
{
	/*
                                                       
                                                       
  */
	return addr;
}

#ifdef CONFIG_DYNAMIC_FTRACE

struct dyn_arch_ftrace {
	/*                              */
};

int ftrace_int3_handler(struct pt_regs *regs);

#endif /*                        */
#endif /*              */
#endif /*                        */


#if !defined(__ASSEMBLY__) && !defined(COMPILE_OFFSETS)

#if defined(CONFIG_FTRACE_SYSCALLS) && defined(CONFIG_IA32_EMULATION)
#include <asm/compat.h>

/*
                                                             
                                                            
                                                               
  
                                                          
                                          
 */
#define ARCH_TRACE_IGNORE_COMPAT_SYSCALLS 1
static inline bool arch_trace_is_compat_syscall(struct pt_regs *regs)
{
	if (is_compat_task())
		return true;
	return false;
}
#endif /*                                                 */
#endif /*                                    */

#endif /*                   */
