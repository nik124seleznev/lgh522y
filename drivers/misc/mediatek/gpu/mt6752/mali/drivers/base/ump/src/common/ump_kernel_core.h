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





#ifndef _UMP_KERNEL_CORE_H_
#define _UMP_KERNEL_CORE_H_


#include <linux/mutex.h>
#include <linux/rwsem.h>
#include <asm/atomic.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/cred.h>
#include <asm/mmu_context.h>

#ifdef CONFIG_KDS
#include <linux/kds.h>
#endif
#include <linux/ump-common.h>
#include <ump/src/common/ump_kernel_descriptor_mapping.h>

/*              */
struct umpp_session;

/* 
                       
                                                                  
 */
typedef enum
{
	UMP_MGMT_EXTERNAL = (1ul << 0) /*                                                                   */
	/*                            */
} umpp_management_flags;

/* 
                                                   
                                    
 */
typedef struct umpp_device
{
	struct mutex secure_id_map_lock; /*                                     */
	umpp_descriptor_mapping * secure_id_map; /*                                             */
} umpp_device;

/* 
                                                                 
                                                                 
                                            
 */
typedef struct umpp_cpu_mapping
{
	struct list_head        link; /*                                              */
	void                  *vaddr_start; /*                               */
	size_t                nr_pages; /*                                  */
	uint64_t              page_off; /*                                                                           */
	ump_dd_handle         handle; /*                                          */
	struct umpp_session * session; /*                                     */
} umpp_cpu_mapping;

/* 
                                     
                                             
                                                   
   */
typedef struct umpp_allocation
{
	ump_secure_id id; /*                               */
	atomic_t refcount; /*               */

	ump_alloc_flags flags; /*                                   */
	uint32_t management_flags; /*                            */

	pid_t owner; /*                                                    */

	ump_dd_security_filter filter_func; /*                                                              */
	ump_dd_final_release_callback final_release_func; /*                                                  */
	void* callback_data; /*                                         */

	uint64_t size; /*                                     */
	uint64_t blocksCount; /*                                                            */
	ump_dd_physical_block_64 * block_array; /*                                                                 */

	struct mutex     map_list_lock; /*                                */
	struct list_head map_list; /*                                                 */

#ifdef CONFIG_KDS
	struct kds_resource kds_res; /*                                                          */
#endif

	void * backendData; /*                                     */
} umpp_allocation;

/* 
                                                     
                                                                                                               
                                                                  
 */
typedef struct umpp_session_memory_usage
{
	ump_secure_id id; /*                                    */
	ump_dd_handle mem; /*                      */

	/* 
                                                                            
                                                                      
                                                                  
    */
	atomic_t process_usage_count;

	struct list_head link; /*                                              */
} umpp_session_memory_usage;

/* 
                                           
                                                        
 */
typedef struct umpp_session
{
	struct mutex session_lock; /*                                      */
	struct list_head memory_usage; /*                                                           */
	void*  import_handler_data[UMPP_EXTERNAL_MEM_COUNT]; /*                                           */
} umpp_session;

/* 
                  
                                                                            
                                                                           
 */
ump_result umpp_core_constructor(void);

/* 
                     
                                                                         
 */
void umpp_core_destructor(void);

/* 
                     
                                                                           
                                                                               
 */
umpp_session *umpp_core_session_start(void);

/* 
                   
                                                                                
                                                                            
 */
void umpp_core_session_end(umpp_session *session);

/* 
                                                      
                                                                                    
                                                               
                                                                       
                                                                   
                                    
                                                                                   
 */
umpp_cpu_mapping * umpp_dd_find_enclosing_mapping(umpp_allocation * alloc, void* uaddr, size_t size);

/* 
                                           
                                                                                               
                                                           
                                    
 */
void umpp_dd_add_cpu_mapping(umpp_allocation * alloc, umpp_cpu_mapping * map);

/* 
                                                     
                                                                     
                                             
 */
void umpp_dd_remove_cpu_mapping(umpp_allocation * alloc, umpp_cpu_mapping * target);

/* 
                                                                            
                                                          
                                                                             
                                                                     
                                                                                            
                                                                     
 */
int umpp_dd_find_start_block(const umpp_allocation * alloc, uint64_t offset, uint64_t * const block_index, uint64_t * const block_internal_offset);

/* 
                            
                                                              
                               
                                                   
                                                           
                                                                    
 */
void umpp_dd_cpu_msync_now(ump_dd_handle mem, ump_cpu_msync_op op, void * address, size_t size);

/* 
                                    
                                                       
                                                                               
   */
void umpp_import_handlers_init(umpp_session * session);

/* 
                                 
                                                                
                                                                               
 */
void umpp_import_handlers_term(umpp_session * session);

#endif /*                     */

