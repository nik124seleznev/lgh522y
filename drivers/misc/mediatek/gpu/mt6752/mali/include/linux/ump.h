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

#ifndef _UMP_KERNEL_INTERFACE_H_
#define _UMP_KERNEL_INTERFACE_H_

/* 
                      
     
 */

/*                                                     
      */

/* 
                                                           
 */
typedef void * ump_dd_handle;

#ifdef CONFIG_KDS
#include <linux/kds.h>
#endif

#include <linux/ump-common.h>

#define UMP_KERNEL_API_EXPORT

#if defined(__KERNEL__)
#include <linux/ump-import.h>
#else
#include <ump/src/library/common/ump_user.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/* 
                                                  
 */
#define UMP_DD_INVALID_MEMORY_HANDLE ((ump_dd_handle)0)

/* 
                                                              
 */
typedef struct ump_dd_physical_block_64
{
	uint64_t addr; /*                                     */
	uint64_t size; /*                                                             */
} ump_dd_physical_block_64;

/* 
                        
  
                                                               
                    
                    
                                 
                                                                                                                
  
                                                                                           
                                                        
                                                                                               
  
                                          
                    
                               
                                                      
  
                
                               
                              
 */
typedef bool (*ump_dd_security_filter)(ump_secure_id, ump_dd_handle, void *);

/* 
                             
  
                                                                                                                      
                                                                                                                                
                                                 
  
                                          
                             
                                                    
 */
typedef void (*ump_dd_final_release_callback)(const ump_dd_handle, void *);

/* 
                     
                                                                     
                                                          
                                                               
                                                                                 
                                                                                             
                                                                        
                                                                                                          
                                                                    
                                            
                                                                                                               
                                                                                 
                                                                                                                
                                                                                                  
 */
UMP_KERNEL_API_EXPORT ump_dd_handle ump_dd_allocate_64(uint64_t size, ump_alloc_flags flags, ump_dd_security_filter filter_func, ump_dd_final_release_callback final_release_func, void* callback_data);


/* 
                          
                                                                           
                                                                                                      
                                              
                                                                 
 */
UMP_KERNEL_API_EXPORT ump_alloc_flags ump_dd_allocation_flags_get(const ump_dd_handle mem);


/* 
                                                        
  
                                                                              
                                                                                     
                                
                          
                                                                                                         
                                                                                                                                
  
                                                                                   
                                    
  
                                                                                
  
                             
                          
                         
  
                                   
  
                                                              
 */
UMP_KERNEL_API_EXPORT ump_secure_id ump_dd_secure_id_get(const ump_dd_handle mem);

#ifdef CONFIG_KDS
/* 
                                                          
  
                                                                               
                                      
  
                                                
                                                                            
 */
UMP_KERNEL_API_EXPORT struct kds_resource * ump_dd_kds_resource_get(const ump_dd_handle mem);
#endif

/* 
                                              
  
                                                                                     
                                             
                                                      
                                
  
                                                                                                                          
  
                                                                                     
  
                                                                                 
  
                      
                          
  
                                                                                                                             
  
                                                                                                   
 */
UMP_KERNEL_API_EXPORT ump_dd_handle ump_dd_from_secure_id(ump_secure_id secure_id);


/* 
                                                                            
  
                                                                                   
                                                                                                                                              
  
                                                                               
                                    
  
                                   
                                                                                         
                                                                                      
 */
UMP_KERNEL_API_EXPORT void ump_dd_phys_blocks_get_64(const ump_dd_handle mem, uint64_t * const pCount, const ump_dd_physical_block_64 ** const pArray);

/* 
                                                         
  
                                                                
  
                                                                               
                                    
  
                                                                           
  
                    
  
                                   
  
                                                                            
 */
UMP_KERNEL_API_EXPORT uint64_t ump_dd_size_get_64(const ump_dd_handle mem);


/* 
                                                                  
  
                                                     
                                                 
  
                                                                               
                                    
  
                                                        
                                
                              
                                                                        
  
                                                                         
  
                  
  
                                   
                                                                  
 */
UMP_KERNEL_API_EXPORT int ump_dd_retain(ump_dd_handle mem);


/* 
                                                      
  
                                                                                  
                                                                                     
                    
  
                                                                                                                          
                                                                               
  
                                                                          
  
                   
  
                                   
 */
UMP_KERNEL_API_EXPORT void ump_dd_release(ump_dd_handle mem);

/* 
                                                                      
                                                               
                                                                                                  
                                                                                                                                                                                  
                                                                                            
                                                           
                                                                            
                                                        
                                                                                                                                                                            
                                            
                                                                                                                                                                                              
                                                                                                                
                                                                                                                              
 */
UMP_KERNEL_API_EXPORT ump_dd_handle ump_dd_create_from_phys_blocks_64(const ump_dd_physical_block_64 * blocks, uint64_t num_blocks, ump_alloc_flags flags, ump_dd_security_filter filter_func, ump_dd_final_release_callback final_release_func, void* callback_data);


/*                  
                                                              
  
    
 */

/* 
                                                  
 */
#define UMP_DD_HANDLE_INVALID UMP_DD_INVALID_MEMORY_HANDLE

/* 
                                    
 */
typedef enum
{
	UMP_DD_SUCCESS, /*                     */
	UMP_DD_INVALID /*                     */
} ump_dd_status_code;


/* 
                                                              
 */
typedef struct ump_dd_physical_block
{
	unsigned long addr; /*                                     */
	unsigned long size; /*                                                   */
} ump_dd_physical_block;


/* 
                                              
  
                                                                                     
                                             
                                                                                           
                                
  
                                                                                                                                  
  
                                
                                        
  
                                                                                                                                                  
  
                                                                                             
 */
UMP_KERNEL_API_EXPORT ump_dd_handle ump_dd_handle_create_from_secure_id(ump_secure_id secure_id);



/* 
                                                                      
                                                               
  
                                                        
                                                                            
  
                                                                                                                              
 */
UMP_KERNEL_API_EXPORT ump_dd_handle ump_dd_handle_create_from_phys_blocks(ump_dd_physical_block * blocks, unsigned long num_blocks);


/* 
                                                                            
  
                                                                                                          
                                                                                                      
                                                                         
                                                      
  
                              
                             
  
                                   
  
                                                                                                                                   
 */
UMP_KERNEL_API_EXPORT unsigned long ump_dd_phys_block_count_get(ump_dd_handle mem);


/* 
                                                                            
  
                                                                                   
  
                                                                                            
  
                             
  
                                   
                                                                                                                                   
                                                                                         
                                                                                                                                     
  
                                                                              
 */
UMP_KERNEL_API_EXPORT ump_dd_status_code ump_dd_phys_blocks_get(ump_dd_handle mem, ump_dd_physical_block * const blocks, unsigned long num_blocks);


/* 
                                                                                                    
  
                                                                                   
  
                                                                                         
  
                              
  
                                   
                                                      
                                                                                                                                    
  
                                                                              
 */
UMP_KERNEL_API_EXPORT ump_dd_status_code ump_dd_phys_block_get(ump_dd_handle mem, unsigned long index, ump_dd_physical_block * const block);


/* 
                                                         
  
                                                                
  
                                                                                          
  
                    
  
                                   
  
                                                                            
 */
UMP_KERNEL_API_EXPORT unsigned long ump_dd_size_get(ump_dd_handle mem);


/* 
                                                       
  
                                                                                          
                                                                                                   
                                                                                                    
                                                 
  
                                                                                      
                                
                                                                                  
                                                                        
  
                                                                                                    
  
                         
  
                                   
 */
UMP_KERNEL_API_EXPORT void ump_dd_reference_add(ump_dd_handle mem);


/* 
                                                      
  
                                                                                    
                                                                                     
                    
  
                                                                                                            
  
                             
  
                                   
 */
UMP_KERNEL_API_EXPORT void ump_dd_reference_release(ump_dd_handle mem);

/*    */

#ifdef __cplusplus
}
#endif


/*     */ /*                                */

/*     */ /*                   */

#endif /*                          */
