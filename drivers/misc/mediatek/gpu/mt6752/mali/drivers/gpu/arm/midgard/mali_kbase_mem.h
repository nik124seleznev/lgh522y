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

#ifndef _KBASE_MEM_H_
#define _KBASE_MEM_H_

#ifndef _KBASE_H_
#error "Don't include this file directly, use mali_kbase.h instead"
#endif

#include <malisw/mali_malisw.h>
#include <linux/kref.h>

#ifdef CONFIG_UMP
#include <linux/ump.h>
#endif				/*            */
#include "mali_base_kernel.h"
#include <mali_kbase_hw.h>
#include "mali_kbase_pm.h"
#include "mali_kbase_defs.h"
#ifdef CONFIG_MALI_GATOR_SUPPORT
#include "mali_kbase_gator.h"
#endif  /*                         */

/*                                                                                                     */
#define KBASEP_TMEM_GROWABLE_BLOCKSIZE_PAGES_LOG2_HW_ISSUE_8316 (2)	/*                  */

/*                                                                                    
                                                                                                                            
                                                                                                                              
                                                                                                          */
#define KBASEP_TMEM_GROWABLE_BLOCKSIZE_PAGES_LOG2_HW_ISSUE_9630 (3)	/*                  */

#define KBASEP_TMEM_GROWABLE_BLOCKSIZE_PAGES_LOG2 (0)	/*                 */

/*                                  */
#define KBASEP_TMEM_GROWABLE_BLOCKSIZE_PAGES (1u << KBASEP_TMEM_GROWABLE_BLOCKSIZE_PAGES_LOG2)
#define KBASEP_TMEM_GROWABLE_BLOCKSIZE_PAGES_HW_ISSUE_8316 (1u << KBASEP_TMEM_GROWABLE_BLOCKSIZE_PAGES_LOG2_HW_ISSUE_8316)
#define KBASEP_TMEM_GROWABLE_BLOCKSIZE_PAGES_HW_ISSUE_9630 (1u << KBASEP_TMEM_GROWABLE_BLOCKSIZE_PAGES_LOG2_HW_ISSUE_9630)
/* 
                
 */
typedef struct kbase_cpu_mapping {
	struct   list_head mappings_list;
	struct   kbase_mem_phy_alloc *alloc;
	struct   kbase_context *kctx;
	struct   kbase_va_region *region;
	pgoff_t  page_off;
	int      count;
	unsigned long vm_start;
	unsigned long vm_end;
} kbase_cpu_mapping;

enum kbase_memory_type {
	KBASE_MEM_TYPE_NATIVE,
	KBASE_MEM_TYPE_IMPORTED_UMP,
	KBASE_MEM_TYPE_IMPORTED_UMM,
	KBASE_MEM_TYPE_ALIAS,
	KBASE_MEM_TYPE_TB,
	KBASE_MEM_TYPE_RAW
};

/*                                                      
                                               */
struct kbase_aliased {
	struct kbase_mem_phy_alloc *alloc; /*                                       */
	u64 offset; /*          */
	u64 length; /*          */
};

/* 
                                                   
  */
#define KBASE_MEM_PHY_ALLOC_ACCESSED_CACHED  (1ul << 0)
#define KBASE_MEM_PHY_ALLOC_LARGE            (1ul << 1)

/*                                
                           
                                                  
                                                                          
                                                                                   
                                                                                       
                                                                          
 */
struct kbase_mem_phy_alloc
{
	struct kref           kref; /*                               */
	atomic_t              gpu_mappings;
	size_t                nents; /*      */
	phys_addr_t *         pages; /*                                     */

	/*                    */
	struct list_head      mappings;

	/*                */
	enum kbase_memory_type type;

	unsigned long properties;

	/*                                        */
	union {
#ifdef CONFIG_UMP
		ump_dd_handle ump_handle;
#endif /*            */
#if defined(CONFIG_DMA_SHARED_BUFFER)
		struct {
			struct dma_buf *dma_buf;
			struct dma_buf_attachment *dma_attachment;
			unsigned int current_mapping_usage_count;
			struct sg_table *sgt;
		} umm;
#endif /*                                   */
		struct {
			mali_size64 stride;
			size_t nents;
			struct kbase_aliased *aliased;
		} alias;
		/*                                                           */
		struct kbase_context *kctx;
	} imported;
};

static inline void kbase_mem_phy_alloc_gpu_mapped(struct kbase_mem_phy_alloc *alloc)
{
	KBASE_DEBUG_ASSERT(alloc);
	/*                                          */
	if (alloc->type == KBASE_MEM_TYPE_NATIVE)
		atomic_inc(&alloc->gpu_mappings);
}

static inline void kbase_mem_phy_alloc_gpu_unmapped(struct kbase_mem_phy_alloc *alloc)
{
	KBASE_DEBUG_ASSERT(alloc);
	/*                                          */
	if (alloc->type == KBASE_MEM_TYPE_NATIVE)
		if (0 > atomic_dec_return(&alloc->gpu_mappings)) {
			pr_err("Mismatched %s:\n", __func__);
			dump_stack();
		}
}

void kbase_mem_kref_free(struct kref * kref);

mali_error kbase_mem_init(struct kbase_device * kbdev);
void kbase_mem_halt(struct kbase_device * kbdev);
void kbase_mem_term(struct kbase_device * kbdev);

static inline struct kbase_mem_phy_alloc * kbase_mem_phy_alloc_get(struct kbase_mem_phy_alloc * alloc)
{
	kref_get(&alloc->kref);
	return alloc;
}

static inline struct kbase_mem_phy_alloc * kbase_mem_phy_alloc_put(struct kbase_mem_phy_alloc * alloc)
{
	kref_put(&alloc->kref, kbase_mem_kref_free);
	return NULL;
}

/* 
                                                        
 */
typedef struct kbase_va_region {
	struct rb_node rblink;
	struct list_head link;

	struct kbase_context *kctx;	/*                          */

	u64 start_pfn;		/*                      */
	size_t nr_pages;

/*             */
#define KBASE_REG_FREE              (1ul << 0)
/*                  */
#define KBASE_REG_CPU_WR            (1ul << 1)
/*                  */
#define KBASE_REG_GPU_WR            (1ul << 2)
/*                 */
#define KBASE_REG_GPU_NX            (1ul << 3)
/*                */
#define KBASE_REG_CPU_CACHED        (1ul << 4)
/*                */
#define KBASE_REG_GPU_CACHED        (1ul << 5)

#define KBASE_REG_GROWABLE          (1ul << 6)
/*                 */
#define KBASE_REG_PF_GROW           (1ul << 7)

/*                  */
#define KBASE_REG_CUSTOM_VA         (1ul << 8)

/*                           */
#define KBASE_REG_SHARE_IN          (1ul << 9)
/*                                   */
#define KBASE_REG_SHARE_BOTH        (1ul << 10)

/*                             */
#define KBASE_REG_ZONE_MASK         (3ul << 11)
#define KBASE_REG_ZONE(x)           (((x) & 3) << 11)

/*                 */
#define KBASE_REG_GPU_RD            (1ul<<13)
/*                 */
#define KBASE_REG_CPU_RD            (1ul<<14)

/*                               */
#define KBASE_REG_ALIGNED           (1ul<<15)

/*                                                */
#define KBASE_REG_MEMATTR_MASK      (7ul << 16)
#define KBASE_REG_MEMATTR_INDEX(x)  (((x) & 7) << 16)
#define KBASE_REG_MEMATTR_VALUE(x)  (((x) & KBASE_REG_MEMATTR_MASK) >> 16)

#define KBASE_REG_ZONE_SAME_VA      KBASE_REG_ZONE(0)

/*                               */
/*
                                                                            
                                                                         
                                                                           
                                                                          
                                                          
 */
#define KBASE_REG_ZONE_EXEC         KBASE_REG_ZONE(1)	/*                                       */
#define KBASE_REG_ZONE_EXEC_BASE    ((1ULL << 32) >> PAGE_SHIFT)
#define KBASE_REG_ZONE_EXEC_SIZE    ((16ULL * 1024 * 1024) >> PAGE_SHIFT)

#define KBASE_REG_ZONE_CUSTOM_VA         KBASE_REG_ZONE(2)
#define KBASE_REG_ZONE_CUSTOM_VA_BASE    (KBASE_REG_ZONE_EXEC_BASE + KBASE_REG_ZONE_EXEC_SIZE) /*                                    */
#define KBASE_REG_ZONE_CUSTOM_VA_SIZE    (((1ULL << 44) >> PAGE_SHIFT) - KBASE_REG_ZONE_CUSTOM_VA_BASE)
/*                         */

	unsigned long flags;

	size_t extent; /*                           */

	struct kbase_mem_phy_alloc * alloc; /*                                                                          */

	/*                                                  */
	struct kds_resource *kds_res;

} kbase_va_region;

/*                  */
static INLINE phys_addr_t *kbase_get_phy_pages(struct kbase_va_region *reg)
{
	KBASE_DEBUG_ASSERT(reg);
	KBASE_DEBUG_ASSERT(reg->alloc);

	return reg->alloc->pages;
}

static INLINE size_t kbase_reg_current_backed_size(struct kbase_va_region * reg)
{
	KBASE_DEBUG_ASSERT(reg);
	/*                                                   */
	if (reg->alloc)
		return reg->alloc->nents;
	else
		return 0;
}

#define KBASE_MEM_PHY_ALLOC_LARGE_THRESHOLD ((size_t)(4*1024)) /*                                               */

static INLINE struct kbase_mem_phy_alloc * kbase_alloc_create(size_t nr_pages, enum kbase_memory_type type)
{
	struct kbase_mem_phy_alloc *alloc;
	const size_t alloc_size =
			sizeof(*alloc) + sizeof(*alloc->pages) * nr_pages;

	/*                                                                */
	if (nr_pages > ((((size_t) -1) - sizeof(*alloc))
			/ sizeof(*alloc->pages)))
		return ERR_PTR(-ENOMEM);

	/*                                                                     */
	if (alloc_size > KBASE_MEM_PHY_ALLOC_LARGE_THRESHOLD)
		alloc = vzalloc(alloc_size);
	else
		alloc = kzalloc(alloc_size, GFP_KERNEL);

	if (!alloc)
		return ERR_PTR(-ENOMEM);

	/*                         */
	if (alloc_size > KBASE_MEM_PHY_ALLOC_LARGE_THRESHOLD)
		alloc->properties |= KBASE_MEM_PHY_ALLOC_LARGE;

	kref_init(&alloc->kref);
	atomic_set(&alloc->gpu_mappings, 0);
	alloc->nents = 0;
	alloc->pages = (void*)(alloc + 1);
	INIT_LIST_HEAD(&alloc->mappings);
	alloc->type = type;

	return alloc;
}

static INLINE int kbase_reg_prepare_native(struct kbase_va_region * reg, struct kbase_context * kctx)
{
	KBASE_DEBUG_ASSERT(reg);
	KBASE_DEBUG_ASSERT(!reg->alloc);
	KBASE_DEBUG_ASSERT(reg->flags & KBASE_REG_FREE);

	reg->alloc = kbase_alloc_create(reg->nr_pages, KBASE_MEM_TYPE_NATIVE);
	if (IS_ERR(reg->alloc))
		return PTR_ERR(reg->alloc);
	else if (!reg->alloc)
		return -ENOMEM;
	reg->alloc->imported.kctx = kctx;
	reg->flags &= ~KBASE_REG_FREE;
	return 0;
}

static inline int kbase_atomic_add_pages(int num_pages, atomic_t *used_pages)
{
	int new_val = atomic_add_return(num_pages, used_pages);
#ifdef CONFIG_MALI_GATOR_SUPPORT
	kbase_trace_mali_total_alloc_pages_change((long long int)new_val);
#endif
	return new_val;
}

static inline int kbase_atomic_sub_pages(int num_pages, atomic_t *used_pages)
{
	int new_val = atomic_sub_return(num_pages, used_pages);
#ifdef CONFIG_MALI_GATOR_SUPPORT
	kbase_trace_mali_total_alloc_pages_change((long long int)new_val);
#endif
	return new_val;
}

/* 
                                                               
  
                                                                             
              
  
                                                            
                                                    
 */
int kbase_mem_lowlevel_init(struct kbase_device *kbdev);


/* 
                                                              
  
                                                
                                     
  
                                            
 */
void kbase_mem_lowlevel_term(struct kbase_device *kbdev);

/* 
                                                  
  
                           
                                                     
                                                                       
                               
                                                                        
                                                                      
                                                                            
  
                                                  
                                                                   
                                                            
                                                                              
         
 */
mali_error kbase_mem_allocator_init(struct kbase_mem_allocator *allocator,
				    unsigned int max_size,
				    struct kbase_device *kbdev);

/* 
                                                           
  
                                                           
                                              
                                                                                                         
                                                                                                     
 */
mali_error kbase_mem_allocator_alloc(struct kbase_mem_allocator * allocator, size_t nr_pages, phys_addr_t *pages);

/* 
                                                                
  
                                                            
                                          
                                                                                           
                                                                       
 */
void kbase_mem_allocator_free(struct kbase_mem_allocator * allocator, size_t nr_pages, phys_addr_t *pages, mali_bool sync_back);

/* 
                                                 
  
                                                            
                                                                       
                           
  
                                              
 */
void kbase_mem_allocator_term(struct kbase_mem_allocator * allocator);



mali_error kbase_region_tracker_init(struct kbase_context *kctx);
void kbase_region_tracker_term(struct kbase_context *kctx);

struct kbase_va_region *kbase_region_tracker_find_region_enclosing_address(struct kbase_context *kctx, mali_addr64 gpu_addr);

/* 
                                                          
  
                                         
 */
struct kbase_va_region *kbase_region_tracker_find_region_base_address(struct kbase_context *kctx, mali_addr64 gpu_addr);

struct kbase_va_region *kbase_alloc_free_region(struct kbase_context *kctx, u64 start_pfn, size_t nr_pages, int zone);
void kbase_free_alloced_region(struct kbase_va_region *reg);
mali_error kbase_add_va_region(struct kbase_context *kctx, struct kbase_va_region *reg, mali_addr64 addr, size_t nr_pages, size_t align);

mali_error kbase_gpu_mmap(struct kbase_context *kctx, struct kbase_va_region *reg, mali_addr64 addr, size_t nr_pages, size_t align);
mali_bool kbase_check_alloc_flags(unsigned long flags);
void kbase_update_region_flags(struct kbase_va_region *reg, unsigned long flags);

void kbase_gpu_vm_lock(struct kbase_context *kctx);
void kbase_gpu_vm_unlock(struct kbase_context *kctx);

int kbase_alloc_phy_pages(struct kbase_va_region *reg, size_t vsize, size_t size);

mali_error kbase_mmu_init(struct kbase_context *kctx);
void kbase_mmu_term(struct kbase_context *kctx);

phys_addr_t kbase_mmu_alloc_pgd(struct kbase_context *kctx);
void kbase_mmu_free_pgd(struct kbase_context *kctx);
mali_error kbase_mmu_insert_pages(struct kbase_context *kctx, u64 vpfn,
				  phys_addr_t *phys, size_t nr,
				  unsigned long flags);
mali_error kbase_mmu_insert_single_page(struct kbase_context *kctx, u64 vpfn,
					phys_addr_t phys, size_t nr,
					unsigned long flags);

mali_error kbase_mmu_teardown_pages(struct kbase_context *kctx, u64 vpfn, size_t nr);
mali_error kbase_mmu_update_pages(struct kbase_context *kctx, u64 vpfn, phys_addr_t* phys, size_t nr, unsigned long flags);

/* 
                                                
  
                                                            
 */
mali_error kbase_gpu_mmap(struct kbase_context *kctx, struct kbase_va_region *reg, mali_addr64 addr, size_t nr_pages, size_t align);

/* 
                                                           
  
                                         
 */
mali_error kbase_gpu_munmap(struct kbase_context *kctx, struct kbase_va_region *reg);

/* 
                                                   
                                                                     
                                                              
 */
void kbase_mmu_update(struct kbase_context *kctx);

/* 
                                                   
                                                                     
                                                              
 */
void kbase_mmu_disable(struct kbase_context *kctx);

void kbase_mmu_interrupt(struct kbase_device *kbdev, u32 irq_stat);

/*                                 
  
                                                                                                                
                                                          
  
                                                           
  
                                                                                     
  
                                                     
                                                                           
  
                                                                                                                  
         
 */
void *kbase_mmu_dump(struct kbase_context *kctx, int nr_pages);

mali_error kbase_sync_now(struct kbase_context *kctx, struct base_syncset *syncset);
void kbase_sync_single(struct kbase_context *kctx, phys_addr_t pa,
		size_t size, kbase_sync_kmem_fn sync_fn);
void kbase_pre_job_sync(struct kbase_context *kctx, struct base_syncset *syncsets, size_t nr);
void kbase_post_job_sync(struct kbase_context *kctx, struct base_syncset *syncsets, size_t nr);

/* 
                                          
  
                                                                          
                              
  
                                                                     
                                                               
                                                                   
  
                                                                          
 */
mali_error kbase_tmem_set_attributes(struct kbase_context *kctx, mali_addr64 gpu_addr, u32  attributes);

/* 
                                         
  
                                                                     
  
                                                                     
                                                               
                                                                
  
                                                                          
 */
mali_error kbase_tmem_get_attributes(struct kbase_context *kctx, mali_addr64 gpu_addr, u32 * const attributes);

/*                       */
mali_error kbase_mem_free(struct kbase_context *kctx, mali_addr64 gpu_addr);
mali_error kbase_mem_free_region(struct kbase_context *kctx, struct kbase_va_region *reg);
void kbase_os_mem_map_lock(struct kbase_context *kctx);
void kbase_os_mem_map_unlock(struct kbase_context *kctx);

/* 
                                                                       
  
                                                                                                  
                      
  
                                      
                                                                            
 */

void kbasep_os_process_page_usage_update(struct kbase_context *kctx, int pages);

/* 
                                                                       
  
                                                                                               
                       
  
                                                                    
                                                                            
 */

static INLINE void kbase_process_page_usage_inc(struct kbase_context *kctx, int pages)
{
	kbasep_os_process_page_usage_update(kctx, pages);
}

/* 
                                                                              
  
                                                                                                      
                       
  
                                                                    
                                                                            
 */

static INLINE void kbase_process_page_usage_dec(struct kbase_context *kctx, int pages)
{
	kbasep_os_process_page_usage_update(kctx, 0 - pages);
}

/* 
                                                                              
                               
  
                                                                               
                                                                              
                                                                               
                                                                            
                    
  
                                                                            
                                                                            
                                                   
                                                                   
                                                                             
                                                                              
                                                          
  
                                                                          
                     
 */
mali_error kbasep_find_enclosing_cpu_mapping_offset(struct kbase_context *kctx,
							mali_addr64 gpu_addr,
							unsigned long uaddr,
							size_t size,
							mali_size64 *offset);

enum hrtimer_restart kbasep_as_poke_timer_callback(struct hrtimer *timer);
void kbase_as_poking_timer_retain_atom(struct kbase_device *kbdev, struct kbase_context *kctx, struct kbase_jd_atom *katom);
void kbase_as_poking_timer_release_atom(struct kbase_device *kbdev, struct kbase_context *kctx, struct kbase_jd_atom *katom);

/* 
                                  
 
                                                               
 
                                                    
                                                                    
 
                                                                               
*/
int kbase_alloc_phy_pages_helper(struct kbase_mem_phy_alloc * alloc, size_t nr_pages_requested);

/* 
                             
 
                                                 
 
                                                       
                                                              
*/
int kbase_free_phy_pages_helper(struct kbase_mem_phy_alloc * alloc, size_t nr_pages_to_free);

#ifdef CONFIG_MALI_NO_MALI
static inline void kbase_wait_write_flush(struct kbase_context *kctx)
{
}
#else
void kbase_wait_write_flush(struct kbase_context *kctx);
#endif

/* 
                                     
 
                                                                
 
                                                                
                                                                             
                                   
                                                         
*/
void kbase_mmu_interrupt_process(struct kbase_device *kbdev,
		struct kbase_context *kctx, struct kbase_as *as);

int kbase_report_gpu_memory_usage();

int kbase_report_gpu_memory_peak();

#endif				/*               */
