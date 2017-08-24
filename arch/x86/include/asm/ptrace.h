#ifndef _ASM_X86_PTRACE_H
#define _ASM_X86_PTRACE_H

#include <asm/segment.h>
#include <asm/page_types.h>
#include <uapi/asm/ptrace.h>

#ifndef __ASSEMBLY__
#ifdef __i386__

struct pt_regs {
	unsigned long bx;
	unsigned long cx;
	unsigned long dx;
	unsigned long si;
	unsigned long di;
	unsigned long bp;
	unsigned long ax;
	unsigned long ds;
	unsigned long es;
	unsigned long fs;
	unsigned long gs;
	unsigned long orig_ax;
	unsigned long ip;
	unsigned long cs;
	unsigned long flags;
	unsigned long sp;
	unsigned long ss;
};

#else /*          */

struct pt_regs {
	unsigned long r15;
	unsigned long r14;
	unsigned long r13;
	unsigned long r12;
	unsigned long bp;
	unsigned long bx;
/*                                                                    */
	unsigned long r11;
	unsigned long r10;
	unsigned long r9;
	unsigned long r8;
	unsigned long ax;
	unsigned long cx;
	unsigned long dx;
	unsigned long si;
	unsigned long di;
	unsigned long orig_ax;
/*                  */
/*                                  */
	unsigned long ip;
	unsigned long cs;
	unsigned long flags;
	unsigned long sp;
	unsigned long ss;
/*                   */
};

#endif /*           */

#include <linux/init.h>
#ifdef CONFIG_PARAVIRT
#include <asm/paravirt_types.h>
#endif

struct cpuinfo_x86;
struct task_struct;

extern unsigned long profile_pc(struct pt_regs *regs);
#define profile_pc profile_pc

extern unsigned long
convert_ip_to_linear(struct task_struct *child, struct pt_regs *regs);
extern void send_sigtrap(struct task_struct *tsk, struct pt_regs *regs,
			 int error_code, int si_code);

extern long syscall_trace_enter(struct pt_regs *);
extern void syscall_trace_leave(struct pt_regs *);

static inline unsigned long regs_return_value(struct pt_regs *regs)
{
	return regs->ax;
}

/*
                                                                            
                                                                         
                                                                         
                                                                        
                                                                             
 */
static inline int user_mode(struct pt_regs *regs)
{
#ifdef CONFIG_X86_32
	return (regs->cs & SEGMENT_RPL_MASK) == USER_RPL;
#else
	return !!(regs->cs & 3);
#endif
}

static inline int user_mode_vm(struct pt_regs *regs)
{
#ifdef CONFIG_X86_32
	return ((regs->cs & SEGMENT_RPL_MASK) | (regs->flags & X86_VM_MASK)) >=
		USER_RPL;
#else
	return user_mode(regs);
#endif
}

static inline int v8086_mode(struct pt_regs *regs)
{
#ifdef CONFIG_X86_32
	return (regs->flags & X86_VM_MASK);
#else
	return 0;	/*                                  */
#endif
}

#ifdef CONFIG_X86_64
static inline bool user_64bit_mode(struct pt_regs *regs)
{
#ifndef CONFIG_PARAVIRT
	/*
                                                             
                                                              
  */
	return regs->cs == __USER_CS;
#else
	/*                                                       */
	return regs->cs == __USER_CS || regs->cs == pv_info.extra_user_64bit_cs;
#endif
}

#define current_user_stack_pointer()	this_cpu_read(old_rsp)
/*                         */
#define compat_user_stack_pointer()	\
	(test_thread_flag(TIF_IA32) 	\
	 ? current_pt_regs()->sp 	\
	 : this_cpu_read(old_rsp))
#endif

#ifdef CONFIG_X86_32
extern unsigned long kernel_stack_pointer(struct pt_regs *regs);
#else
static inline unsigned long kernel_stack_pointer(struct pt_regs *regs)
{
	return regs->sp;
}
#endif

#define GET_IP(regs) ((regs)->ip)
#define GET_FP(regs) ((regs)->bp)
#define GET_USP(regs) ((regs)->sp)

#include <asm-generic/ptrace.h>

/*                                                    */
extern int regs_query_register_offset(const char *name);
extern const char *regs_query_register_name(unsigned int offset);
#define MAX_REG_OFFSET (offsetof(struct pt_regs, ss))

/* 
                                                           
                                                      
                                          
  
                                                                        
                                                                             
                                                            
 */
static inline unsigned long regs_get_register(struct pt_regs *regs,
					      unsigned int offset)
{
	if (unlikely(offset > MAX_REG_OFFSET))
		return 0;
#ifdef CONFIG_X86_32
	/*
                                                
                                           
  */
	if (offset == offsetof(struct pt_regs, sp) &&
	    regs->cs == __KERNEL_CS)
		return kernel_stack_pointer(regs);
#endif
	return *(unsigned long *)((unsigned long)regs + offset);
}

/* 
                                                              
                                                      
                                   
  
                                                                              
                                                                               
 */
static inline int regs_within_kernel_stack(struct pt_regs *regs,
					   unsigned long addr)
{
	return ((addr & ~(THREAD_SIZE - 1))  ==
		(kernel_stack_pointer(regs) & ~(THREAD_SIZE - 1)));
}

/* 
                                                           
                                                      
                           
  
                                                                            
                                                                        
                  
 */
static inline unsigned long regs_get_kernel_stack_nth(struct pt_regs *regs,
						      unsigned int n)
{
	unsigned long *addr = (unsigned long *)kernel_stack_pointer(regs);
	addr += n;
	if (regs_within_kernel_stack(regs, (unsigned long)addr))
		return *addr;
	else
		return 0;
}

#define arch_has_single_step()	(1)
#ifdef CONFIG_X86_DEBUGCTLMSR
#define arch_has_block_step()	(1)
#else
#define arch_has_block_step()	(boot_cpu_data.x86 >= 6)
#endif

#define ARCH_HAS_USER_SINGLE_STEP_INFO

/*
                                                                    
                                                                     
                                                                      
                                                                        
                                                                        
                                           
  
                                          
 */
#define arch_ptrace_stop_needed(code, info)				\
({									\
	set_thread_flag(TIF_NOTIFY_RESUME);				\
	false;								\
})

struct user_desc;
extern int do_get_thread_area(struct task_struct *p, int idx,
			      struct user_desc __user *info);
extern int do_set_thread_area(struct task_struct *p, int idx,
			      struct user_desc __user *info, int can_allocate);

#endif /*               */
#endif /*                   */
