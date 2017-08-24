#ifndef __ASM_SUSPEND_H
#define __ASM_SUSPEND_H

#define NR_CTX_REGS (11+2+2)

/*
                                                                          
                                                 
 */
struct cpu_suspend_ctx {
	/*
                                         
                                        
  */
	u64 ctx_regs[NR_CTX_REGS];
	u64 sp;
} __aligned(16);

struct sleep_save_sp {
	phys_addr_t *save_ptr_stash;
	phys_addr_t save_ptr_stash_phys;
};

extern void cpu_resume(void);
extern int cpu_suspend(unsigned long);

#endif
