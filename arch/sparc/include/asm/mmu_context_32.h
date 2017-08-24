#ifndef __SPARC_MMU_CONTEXT_H
#define __SPARC_MMU_CONTEXT_H

#ifndef __ASSEMBLY__

#include <asm-generic/mm_hooks.h>

static inline void enter_lazy_tlb(struct mm_struct *mm, struct task_struct *tsk)
{
}

/*                                                          
                                                             
 */
int init_new_context(struct task_struct *tsk, struct mm_struct *mm);

/*                                                          
                                                            
                                                                
                                          
 */
void destroy_context(struct mm_struct *mm);

/*                                */
void switch_mm(struct mm_struct *old_mm, struct mm_struct *mm,
	       struct task_struct *tsk);

#define deactivate_mm(tsk,mm)	do { } while (0)

/*                                                  */
#define activate_mm(active_mm, mm) switch_mm((active_mm), (mm), NULL)

#endif /*                 */

#endif /*                          */
