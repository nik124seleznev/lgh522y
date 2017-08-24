/*
 * Copyright (C) 2004, 2007-2010, 2011-2012 Synopsys, Inc. (www.synopsys.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * vineetg: May 2011
 *  -Refactored get_new_mmu_context( ) to only handle live-mm.
 *   retiring-mm handled in other hooks
 *
 * Vineetg: March 25th, 2008: Bug #92690
 *  -Major rewrite of Core ASID allocation routine get_new_mmu_context
 *
 * Amit Bhor, Sameer Dhavale: Codito Technologies 2004
 */

#ifndef _ASM_ARC_MMU_CONTEXT_H
#define _ASM_ARC_MMU_CONTEXT_H

#include <asm/arcregs.h>
#include <asm/tlb.h>

#include <asm-generic/mm_hooks.h>

/*                        
  
                                                                            
                                                                  
                                                         
  
                                                                         
                                                          
                                                                          
                                                 
  
                                                            
                                                                              
                             
                                                                            
                                                    
  
                                                                 
                                                                            
                                                                            
                                                                           
                                                                          
                                                                              
                                                                           
                                                                           
                                                         
                                                                     
                                                 
                                                                           
                                                                             
                                                                       
                                                              
                                                         
 */

#define FIRST_ASID  0
#define MAX_ASID    255			/*                                */
#define NO_ASID     (MAX_ASID + 1)	/*                            */
#define NUM_ASID    ((MAX_ASID - FIRST_ASID) + 1)

/*                           */
extern struct mm_struct *asid_mm_map[NUM_ASID + 1];

extern int asid_cache;

/*
                                                                    
                
 */
static inline void get_new_mmu_context(struct mm_struct *mm)
{
	struct mm_struct *prev_owner;
	unsigned long flags;

	local_irq_save(flags);

	/*
                                                 
                                                                  
                                         
  */
	asid_mm_map[mm->context.asid] = (struct mm_struct *)NULL;

	/*                  */
	if (++asid_cache > MAX_ASID) {	/*                */
		asid_cache = FIRST_ASID;
		flush_tlb_all();
	}

	/*
                                                                     
                                                                   
                                                                      
                                                                      
   
                                                                      
                                                                        
                                                                       
                                                                        
                                                                      
                                                                     
                
  */
	prev_owner = asid_mm_map[asid_cache];
	if (prev_owner)
		prev_owner->context.asid = NO_ASID;

	/*                        */
	asid_mm_map[asid_cache] = mm;
	mm->context.asid = asid_cache;

#ifdef CONFIG_ARC_TLB_DBG
	pr_info("ARC_TLB_DBG: NewMM=0x%x OldMM=0x%x task_struct=0x%x Task: %s,"
	       " pid:%u, assigned asid:%lu\n",
	       (unsigned int)mm, (unsigned int)prev_owner,
	       (unsigned int)(mm->context.tsk), (mm->context.tsk)->comm,
	       (mm->context.tsk)->pid, mm->context.asid);
#endif

	write_aux_reg(ARC_REG_PID, asid_cache | MMU_ENABLE);

	local_irq_restore(flags);
}

/*
                                                          
            
 */
static inline int
init_new_context(struct task_struct *tsk, struct mm_struct *mm)
{
	mm->context.asid = NO_ASID;
#ifdef CONFIG_ARC_TLB_DBG
	mm->context.tsk = tsk;
#endif
	return 0;
}

/*                                                            
                                                                        
*/
static inline void switch_mm(struct mm_struct *prev, struct mm_struct *next,
			     struct task_struct *tsk)
{
#ifndef CONFIG_SMP
	/*                                                             */
	write_aux_reg(ARC_REG_SCRATCH_DATA0, next->pgd);
#endif

	/*
                                                                  
                                               
                                                    
                                                                       
                                                                     
                                                              
   
                                                                   
                                                             
                                                          
  */
	if (next->context.asid > asid_cache) {
		get_new_mmu_context(next);
	} else {
		/*
                                                     
                                           
   */
		write_aux_reg(ARC_REG_PID, next->context.asid | MMU_ENABLE);
	}

}

static inline void destroy_context(struct mm_struct *mm)
{
	unsigned long flags;

	local_irq_save(flags);

	asid_mm_map[mm->context.asid] = NULL;
	mm->context.asid = NO_ASID;

	local_irq_restore(flags);
}

/*                                                                         
                                                                             
                                              
                                                   
                                                                             
                                          
 */
#define deactivate_mm(tsk, mm)   do { } while (0)

static inline void activate_mm(struct mm_struct *prev, struct mm_struct *next)
{
#ifndef CONFIG_SMP
	write_aux_reg(ARC_REG_SCRATCH_DATA0, next->pgd);
#endif

	/*                                */
	get_new_mmu_context(next);

}

#define enter_lazy_tlb(mm, tsk)

#endif /*                         */
