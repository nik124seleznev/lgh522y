/*
 *
 * (C) COPYRIGHT ARM Limited. All rights reserved.
 *
 * This program is free software and is provided to you under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation, and any use by you of this program is subject to the terms
 * of such GNU licence.
 *
 * A copy of the licence is included with the program, and can also be obtained
 * from Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 */



/* 
        
                                                          
 */

/* 
                                                      
  
                                                    
                                                                               
                                                                              
                                                                          
               
 */

#ifndef _MALI_KBASE_MMU_HW_H_
#define _MALI_KBASE_MMU_HW_H_

/*                      */
struct kbase_device;
struct kbase_as;
struct kbase_context;

/* 
                             
     
 */

/* 
                                                 
     
 */

/*                                   
 */
enum kbase_mmu_fault_type {
	KBASE_MMU_FAULT_TYPE_UNKNOWN = 0,
	KBASE_MMU_FAULT_TYPE_PAGE,
	KBASE_MMU_FAULT_TYPE_BUS
};

/*                                            
  
                                                                 
                                
  
                                                        
                                                         
                                                         
 */
void kbase_mmu_hw_configure(struct kbase_device *kbdev,
		struct kbase_as *as, struct kbase_context *kctx);

/*                                       
  
                                                                                
                                                                              
  
                                                                        
                                                                         
                                                                         
                                                                       
                                          
                                                        
                                                                     
                                                                               
                                             
  
                                                                    
 */
int kbase_mmu_hw_do_operation(struct kbase_device *kbdev, struct kbase_as *as,
		struct kbase_context *kctx, u64 vpfn, u32 nr, u32 type,
		unsigned int handling_irq);

/*                                                                    
  
                                                                     
  
                                                                   
                                                                    
                                                                   
                                                                        
 */
void kbase_mmu_hw_clear_fault(struct kbase_device *kbdev, struct kbase_as *as,
		struct kbase_context *kctx, enum kbase_mmu_fault_type type);

/*     *//*                             */
/*     *//*                          */

#endif	/*                       */
