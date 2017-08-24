#ifndef _ASM_X86_VM86_H
#define _ASM_X86_VM86_H


#include <asm/ptrace.h>
#include <uapi/asm/vm86.h>

/*
                                                                             
                                                                    
                                                                   
                                                                     
                                                                    
                                                             
 */

struct kernel_vm86_regs {
/*
                                                                  
 */
	struct pt_regs pt;
/*
                                  
 */
	unsigned short es, __esh;
	unsigned short ds, __dsh;
	unsigned short fs, __fsh;
	unsigned short gs, __gsh;
};

struct kernel_vm86_struct {
	struct kernel_vm86_regs regs;
/*
                                                                        
                                                                           
                                                                           
                                                         
                                                                       
                                                                   
 */
#define VM86_TSS_ESP0 flags
	unsigned long flags;
	unsigned long screen_bitmap;
	unsigned long cpu_type;
	struct revectored_struct int_revectored;
	struct revectored_struct int21_revectored;
	struct vm86plus_info_struct vm86plus;
	struct pt_regs *regs32;   /*                                          */
/*
                                                                         
                                                                    
                                                                          
                     
                                                                       

                                            
                                                                  
 */
};

#ifdef CONFIG_VM86

void handle_vm86_fault(struct kernel_vm86_regs *, long);
int handle_vm86_trap(struct kernel_vm86_regs *, long, int);
struct pt_regs *save_v86_state(struct kernel_vm86_regs *);

struct task_struct;
void release_vm86_irqs(struct task_struct *);

#else

#define handle_vm86_fault(a, b)
#define release_vm86_irqs(a)

static inline int handle_vm86_trap(struct kernel_vm86_regs *a, long b, int c)
{
	return 0;
}

#endif /*             */

#endif /*                 */
