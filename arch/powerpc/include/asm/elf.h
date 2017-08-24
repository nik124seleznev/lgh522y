/*
 * ELF register definitions..
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#ifndef _ASM_POWERPC_ELF_H
#define _ASM_POWERPC_ELF_H

#include <linux/sched.h>	/*                 */
#include <asm/page.h>
#include <asm/string.h>
#include <uapi/asm/elf.h>

/*
                                                                             
 */
#define elf_check_arch(x) ((x)->e_machine == ELF_ARCH)
#define compat_elf_check_arch(x)	((x)->e_machine == EM_PPC)

#define CORE_DUMP_USE_REGSET
#define ELF_EXEC_PAGESIZE	PAGE_SIZE

/*                                                                           
                                                                           
                                                                             
                                                                        */

extern unsigned long randomize_et_dyn(unsigned long base);
#define ELF_ET_DYN_BASE		(randomize_et_dyn(0x20000000))

/*
                                                                  
                                                          
  
                                                                         
                                                                    
                                                                 
                                                
 */
#define PPC_ELF_CORE_COPY_REGS(elf_regs, regs) \
	int i, nregs = min(sizeof(*regs) / sizeof(unsigned long), \
			   (size_t)ELF_NGREG);			  \
	for (i = 0; i < nregs; i++) \
		elf_regs[i] = ((unsigned long *) regs)[i]; \
	memset(&elf_regs[i], 0, (ELF_NGREG - i) * sizeof(elf_regs[0]))

/*                                                            */
static inline void ppc_elf_core_copy_regs(elf_gregset_t elf_regs,
					  struct pt_regs *regs)
{
	PPC_ELF_CORE_COPY_REGS(elf_regs, regs);
}
#define ELF_CORE_COPY_REGS(gregs, regs) ppc_elf_core_copy_regs(gregs, regs);

typedef elf_vrregset_t elf_fpxregset_t;

/*                                                                      
                                                                       
                                                       */
# define ELF_HWCAP	(cur_cpu_spec->cpu_user_features)
# define ELF_HWCAP2	(cur_cpu_spec->cpu_user_features2)

/*                                                                
                                                                 
                                                  */

#define ELF_PLATFORM	(cur_cpu_spec->platform)

/*                                                                   
                                                                     
                                                                
                                                                     
                                        
 */
#define ELF_BASE_PLATFORM (powerpc_base_platform)

#ifdef __powerpc64__
# define ELF_PLAT_INIT(_r, load_addr)	do {	\
	_r->gpr[2] = load_addr; 		\
} while (0)
#endif /*               */

#ifdef __powerpc64__
# define SET_PERSONALITY(ex)					\
do {								\
	if ((ex).e_ident[EI_CLASS] == ELFCLASS32)		\
		set_thread_flag(TIF_32BIT);			\
	else							\
		clear_thread_flag(TIF_32BIT);			\
	if (personality(current->personality) != PER_LINUX32)	\
		set_personality(PER_LINUX |			\
			(current->personality & (~PER_MASK)));	\
} while (0)
/*
                                                                    
                                                                      
                                                                      
                                                                      
                                       
 */
# define elf_read_implies_exec(ex, exec_stk) (is_32bit_task() ? \
		(exec_stk == EXSTACK_DEFAULT) : 0)
#else 
# define elf_read_implies_exec(ex, exec_stk) (exec_stk == EXSTACK_DEFAULT)
#endif /*               */

extern int dcache_bsize;
extern int icache_bsize;
extern int ucache_bsize;

/*                                      */
#define ARCH_HAS_SETUP_ADDITIONAL_PAGES
struct linux_binprm;
extern int arch_setup_additional_pages(struct linux_binprm *bprm,
				       int uses_interp);
#define VDSO_AUX_ENT(a,b) NEW_AUX_ENT(a,b)

/*                              */
#define STACK_RND_MASK (is_32bit_task() ? \
	(0x7ff >> (PAGE_SHIFT - 12)) : \
	(0x3ffff >> (PAGE_SHIFT - 12)))

extern unsigned long arch_randomize_brk(struct mm_struct *mm);
#define arch_randomize_brk arch_randomize_brk


#ifdef CONFIG_SPU_BASE
/*                                                            */
#define NT_SPU		1

#define ARCH_HAVE_EXTRA_ELF_NOTES

#endif /*                 */

#endif /*                    */
