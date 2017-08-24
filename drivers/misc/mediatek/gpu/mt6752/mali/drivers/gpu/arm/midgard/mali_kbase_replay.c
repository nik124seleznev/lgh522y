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

#include <linux/dma-mapping.h>
#include <mali_kbase_config.h>
#include <mali_kbase.h>
#include <mali_kbase_mem.h>

#define JOB_NOT_STARTED 0
#define JOB_TYPE_MASK      0xfe
#define JOB_TYPE_NULL      (1 << 1)
#define JOB_TYPE_VERTEX    (5 << 1)
#define JOB_TYPE_TILER     (7 << 1)
#define JOB_TYPE_FUSED     (8 << 1)
#define JOB_TYPE_FRAGMENT  (9 << 1)

#define JOB_FLAG_DESC_SIZE           (1 << 0)
#define JOB_FLAG_PERFORM_JOB_BARRIER (1 << 8)

#define JOB_HEADER_32_FBD_OFFSET (31*4)
#define JOB_HEADER_64_FBD_OFFSET (44*4)

#define FBD_POINTER_MASK (~0x3f)

#define SFBD_TILER_OFFSET (48*4)

#define MFBD_TILER_OFFSET       (14*4)

#define FBD_HIERARCHY_WEIGHTS 8
#define FBD_HIERARCHY_MASK_MASK 0x1fff

#define FBD_TYPE 1

#define HIERARCHY_WEIGHTS 13

#define JOB_HEADER_ID_MAX                 0xffff

#define JOB_SOURCE_ID(status)		(((status) >> 16) & 0xFFFF)
#define JOB_POLYGON_LIST		(0x03)

struct job_head {
	u32 status;
	u32 not_complete_index;
	u64 fault_addr;
	u16 flags;
	u16 index;
	u16 dependencies[2];
	union {
		u64 _64;
		u32 _32;
	} next;
	u32 x[2];
	union {
		u64 _64;
		u32 _32;
	} fragment_fbd;
};

static void dump_job_head(struct kbase_context *kctx, char *head_str,
		struct job_head *job)
{
#ifdef CONFIG_MALI_DEBUG
	dev_dbg(kctx->kbdev->dev, "%s\n", head_str);
	dev_dbg(kctx->kbdev->dev, "addr               = %p\n"
			"status             = %x\n"
			"not_complete_index = %x\n"
			"fault_addr         = %llx\n"
			"flags              = %x\n"
			"index              = %x\n"
			"dependencies       = %x,%x\n",
			job, job->status, job->not_complete_index,
			job->fault_addr, job->flags, job->index,
			job->dependencies[0],
			job->dependencies[1]);

	if (job->flags & JOB_FLAG_DESC_SIZE)
		dev_dbg(kctx->kbdev->dev, "next               = %llx\n",
				job->next._64);
	else
		dev_dbg(kctx->kbdev->dev, "next               = %x\n",
				job->next._32);
#endif
}

struct kbasep_map_struct {
	mali_addr64 gpu_addr;
	struct kbase_mem_phy_alloc *alloc;
	struct page **pages;
	void *addr;
	size_t size;
	mali_bool is_cached;
};

static void *kbasep_map(struct kbase_context *kctx, mali_addr64 gpu_addr,
		size_t size, struct kbasep_map_struct *map)
{
	struct kbase_va_region *region;
	unsigned long page_index;
	unsigned int offset = gpu_addr & ~PAGE_MASK;
	size_t page_count = PFN_UP(offset + size);
	phys_addr_t *page_array;
	struct page **pages;
	void *cpu_addr = NULL;
	pgprot_t prot;
	size_t i;

	if (!size || !map)
		return NULL;

	/*                                           */
	if (size > ((size_t)-1 / PAGE_SIZE))
		return NULL;

	region = kbase_region_tracker_find_region_enclosing_address(kctx, gpu_addr);
	if (!region || (region->flags & KBASE_REG_FREE))
		return NULL;

	page_index = (gpu_addr >> PAGE_SHIFT) - region->start_pfn;

	/*                                            */
	if (-1UL - page_count < page_index)
		return NULL;

	if (page_index + page_count > kbase_reg_current_backed_size(region))
		return NULL;

	page_array = kbase_get_phy_pages(region);
	if (!page_array)
		return NULL;

	pages = kmalloc_array(page_count, sizeof(struct page *), GFP_KERNEL);
	if (!pages)
		return NULL;

	for (i = 0; i < page_count; i++)
		pages[i] = pfn_to_page(PFN_DOWN(page_array[page_index + i]));

	prot = PAGE_KERNEL;
	if (!(region->flags & KBASE_REG_CPU_CACHED)) {
		/*              */
		prot = pgprot_writecombine(prot);
	}

	cpu_addr = vmap(pages, page_count, VM_MAP, prot);
	if (!cpu_addr)
		goto vmap_failed;

	map->gpu_addr = gpu_addr;
	map->alloc = kbase_mem_phy_alloc_get(region->alloc);
	map->pages = pages;
	map->addr = (void *)((uintptr_t)cpu_addr + offset);
	map->size = size;
	map->is_cached = (region->flags & KBASE_REG_CPU_CACHED) != 0;

	if (map->is_cached) {
		/*                 */
		size_t sz = MIN(((size_t) PAGE_SIZE - offset), size);
		phys_addr_t pa = page_to_phys(map->pages[0]) + offset;

		kbase_sync_single(kctx, pa, sz, dma_sync_single_for_cpu);

		/*                            */
		for (i = 1; page_count > 2 && i < page_count - 1; i++) {
			pa = page_to_phys(map->pages[i]);
			kbase_sync_single(kctx, pa, PAGE_SIZE,
					dma_sync_single_for_cpu);
		}

		/*                         */
		if (page_count > 1) {
			pa = page_to_phys(map->pages[page_count - 1]);
			sz = ((offset + size - 1) & ~PAGE_MASK) + 1;
			kbase_sync_single(kctx, pa, sz,
					dma_sync_single_for_cpu);
		}
	}

	return map->addr;

vmap_failed:
	kfree(pages);

	return NULL;
}

static void kbasep_unmap(struct kbase_context *kctx,
		struct kbasep_map_struct *map)
{
	void *addr = (void *)((uintptr_t)map->addr & PAGE_MASK);

	vunmap(addr);

	if (map->is_cached) {
		off_t offset = (uintptr_t)map->addr & ~PAGE_MASK;
		size_t size = map->size;
		size_t page_count = PFN_UP(offset + size);
		size_t i;

		/*                 */
		size_t sz = MIN(((size_t) PAGE_SIZE - offset), size);
		phys_addr_t pa = page_to_phys(map->pages[0]) + offset;

		kbase_sync_single(kctx, pa, sz, dma_sync_single_for_device);

		/*                            */
		for (i = 1; page_count > 2 && i < page_count - 1; i++) {
			pa = page_to_phys(map->pages[i]);
			kbase_sync_single(kctx, pa, PAGE_SIZE,
					dma_sync_single_for_device);
		}

		/*                         */
		if (page_count > 1) {
			pa = page_to_phys(map->pages[page_count - 1]);
			sz = ((offset + size - 1) & ~PAGE_MASK) + 1;
			kbase_sync_single(kctx, pa, sz,
					dma_sync_single_for_device);
		}
	}

	kfree(map->pages);

	map->gpu_addr = 0;
	map->alloc = kbase_mem_phy_alloc_put(map->alloc);
	map->pages = NULL;
	map->addr = NULL;
	map->size = 0;
	map->is_cached = MALI_FALSE;
}

static mali_error kbasep_replay_reset_sfbd(struct kbase_context *kctx,
		mali_addr64 fbd_address, mali_addr64 tiler_heap_free,
		u16 hierarchy_mask, u32 default_weight)
{
	struct {
		u32 padding_1[1];
		u32 flags;
		u64 padding_2[2];
		u64 heap_free_address;
		u32 padding[8];
		u32 weights[FBD_HIERARCHY_WEIGHTS];
	} *fbd_tiler;
	struct kbasep_map_struct map;

	dev_dbg(kctx->kbdev->dev, "fbd_address: %llx\n", fbd_address);

	fbd_tiler = kbasep_map(kctx, fbd_address + SFBD_TILER_OFFSET,
			sizeof(*fbd_tiler), &map);
	if (!fbd_tiler) {
		dev_err(kctx->kbdev->dev, "kbasep_replay_reset_fbd: failed to map fbd\n");
		return MALI_ERROR_FUNCTION_FAILED;
	}

#ifdef CONFIG_MALI_DEBUG
	dev_dbg(kctx->kbdev->dev,
		"FBD tiler:\n"
		"flags = %x\n"
		"heap_free_address = %llx\n",
		fbd_tiler->flags, fbd_tiler->heap_free_address);
#endif
	if (hierarchy_mask) {
		u32 weights[HIERARCHY_WEIGHTS];
		u16 old_hierarchy_mask = fbd_tiler->flags &
						       FBD_HIERARCHY_MASK_MASK;
		int i, j = 0;

		for (i = 0; i < HIERARCHY_WEIGHTS; i++) {
			if (old_hierarchy_mask & (1 << i)) {
				KBASE_DEBUG_ASSERT(j < FBD_HIERARCHY_WEIGHTS);
				weights[i] = fbd_tiler->weights[j++];
			} else {
				weights[i] = default_weight;
			}
		}


		dev_dbg(kctx->kbdev->dev, "Old hierarchy mask=%x  New hierarchy mask=%x\n",
				old_hierarchy_mask, hierarchy_mask);

		for (i = 0; i < HIERARCHY_WEIGHTS; i++)
			dev_dbg(kctx->kbdev->dev, " Hierarchy weight %02d: %08x\n",
					i, weights[i]);

		j = 0;

		for (i = 0; i < HIERARCHY_WEIGHTS; i++) {
			if (hierarchy_mask & (1 << i)) {
				KBASE_DEBUG_ASSERT(j < FBD_HIERARCHY_WEIGHTS);

				dev_dbg(kctx->kbdev->dev, " Writing hierarchy level %02d (%08x) to %d\n",
						i, weights[i], j);

				fbd_tiler->weights[j++] = weights[i];
			}
		}

		for (; j < FBD_HIERARCHY_WEIGHTS; j++)
			fbd_tiler->weights[j] = 0;

		fbd_tiler->flags = hierarchy_mask | (1 << 16);
	}

	fbd_tiler->heap_free_address = tiler_heap_free;

	dev_dbg(kctx->kbdev->dev, "heap_free_address=%llx flags=%x\n",
			fbd_tiler->heap_free_address, fbd_tiler->flags);

	kbasep_unmap(kctx, &map);

	return MALI_ERROR_NONE;
}

static mali_error kbasep_replay_reset_mfbd(struct kbase_context *kctx,
		mali_addr64 fbd_address, mali_addr64 tiler_heap_free,
		u16 hierarchy_mask, u32 default_weight)
{
	struct kbasep_map_struct map;
	struct {
		u32 padding_0;
		u32 flags;
		u64 padding_1[2];
		u64 heap_free_address;
		u64 padding_2;
		u32 weights[FBD_HIERARCHY_WEIGHTS];
	} *fbd_tiler;

	dev_dbg(kctx->kbdev->dev, "fbd_address: %llx\n", fbd_address);

	fbd_tiler = kbasep_map(kctx, fbd_address + MFBD_TILER_OFFSET,
			sizeof(*fbd_tiler), &map);
	if (!fbd_tiler) {
		dev_err(kctx->kbdev->dev,
			       "kbasep_replay_reset_fbd: failed to map fbd\n");
		return MALI_ERROR_FUNCTION_FAILED;
	}

#ifdef CONFIG_MALI_DEBUG
	dev_dbg(kctx->kbdev->dev, "FBD tiler:\n"
			"flags = %x\n"
			"heap_free_address = %llx\n",
			fbd_tiler->flags,
			fbd_tiler->heap_free_address);
#endif
	if (hierarchy_mask) {
		u32 weights[HIERARCHY_WEIGHTS];
		u16 old_hierarchy_mask = (fbd_tiler->flags) &
						       FBD_HIERARCHY_MASK_MASK;
		int i, j = 0;

		for (i = 0; i < HIERARCHY_WEIGHTS; i++) {
			if (old_hierarchy_mask & (1 << i)) {
				KBASE_DEBUG_ASSERT(j < FBD_HIERARCHY_WEIGHTS);
				weights[i] = fbd_tiler->weights[j++];
			} else {
				weights[i] = default_weight;
			}
		}


		dev_dbg(kctx->kbdev->dev, "Old hierarchy mask=%x  New hierarchy mask=%x\n",
				old_hierarchy_mask, hierarchy_mask);

		for (i = 0; i < HIERARCHY_WEIGHTS; i++)
			dev_dbg(kctx->kbdev->dev, " Hierarchy weight %02d: %08x\n",
					i, weights[i]);

		j = 0;

		for (i = 0; i < HIERARCHY_WEIGHTS; i++) {
			if (hierarchy_mask & (1 << i)) {
				KBASE_DEBUG_ASSERT(j < FBD_HIERARCHY_WEIGHTS);

				dev_dbg(kctx->kbdev->dev,
				" Writing hierarchy level %02d (%08x) to %d\n",
							     i, weights[i], j);

				fbd_tiler->weights[j++] = weights[i];
			}
		}

		for (; j < FBD_HIERARCHY_WEIGHTS; j++)
			fbd_tiler->weights[j] = 0;

		fbd_tiler->flags = hierarchy_mask | (1 << 16);
	}

	fbd_tiler->heap_free_address = tiler_heap_free;

	kbasep_unmap(kctx, &map);

	return MALI_ERROR_NONE;
}

/* 
                                                              
  
                                
                           
                                      
  
                                               
                                                               
                                                                     
                                                         
                                                                               
                                                          
                                                                     
                                           
  
                                                            
 */
static mali_error kbasep_replay_reset_tiler_job(struct kbase_context *kctx,
		mali_addr64 job_header,	mali_addr64 tiler_heap_free,
		u16 hierarchy_mask, u32 default_weight,	mali_bool job_64)
{
	struct kbasep_map_struct map;
	mali_addr64 fbd_address;

	if (job_64) {
		u64 *job_ext;

		job_ext = kbasep_map(kctx,
				job_header + JOB_HEADER_64_FBD_OFFSET,
				sizeof(*job_ext), &map);

		if (!job_ext) {
			dev_err(kctx->kbdev->dev, "kbasep_replay_reset_tiler_job: failed to map jc\n");
			return MALI_ERROR_FUNCTION_FAILED;
		}

		fbd_address = *job_ext;

		kbasep_unmap(kctx, &map);
	} else {
		u32 *job_ext;

		job_ext = kbasep_map(kctx,
				job_header + JOB_HEADER_32_FBD_OFFSET,
				sizeof(*job_ext), &map);

		if (!job_ext) {
			dev_err(kctx->kbdev->dev, "kbasep_replay_reset_tiler_job: failed to map jc\n");
			return MALI_ERROR_FUNCTION_FAILED;
		}

		fbd_address = *job_ext;

		kbasep_unmap(kctx, &map);
	}

	if (fbd_address & FBD_TYPE) {
		return kbasep_replay_reset_mfbd(kctx,
						fbd_address & FBD_POINTER_MASK,
						tiler_heap_free,
						hierarchy_mask,
						default_weight);
	} else {
		return kbasep_replay_reset_sfbd(kctx,
						fbd_address & FBD_POINTER_MASK,
						tiler_heap_free,
						hierarchy_mask,
						default_weight);
	}
}

/* 
                                   
  
                                          
  
                                                           
                                                           
                                                                             
                                                                           
                         
                                                             
                                                                           
                                            
  
                                               
                                                                              
                                                           
                                                                             
                                                      
                                                     
                                                                     
                                                         
                                                                               
                                                          
                                                                               
                                                                              
  
                                                            
 */
static mali_error kbasep_replay_reset_job(struct kbase_context *kctx,
		mali_addr64 *job_header, mali_addr64 prev_jc,
		mali_addr64 tiler_heap_free, u16 hierarchy_mask,
		u32 default_weight, u16 hw_job_id_offset,
		mali_bool first_in_chain, mali_bool fragment_chain)
{
	struct job_head *job;
	mali_addr64 new_job_header;
	struct kbasep_map_struct map;

	job = kbasep_map(kctx, *job_header, sizeof(*job), &map);
	if (!job) {
		dev_err(kctx->kbdev->dev,
				 "kbasep_replay_parse_jc: failed to map jc\n");
		return MALI_ERROR_FUNCTION_FAILED;
	}

	dump_job_head(kctx, "Job header:", job);

	if (job->status == JOB_NOT_STARTED && !fragment_chain) {
		dev_err(kctx->kbdev->dev, "Job already not started\n");
		goto out_unmap;
	}
	job->status = JOB_NOT_STARTED;

	if ((job->flags & JOB_TYPE_MASK) == JOB_TYPE_VERTEX)
		job->flags = (job->flags & ~JOB_TYPE_MASK) | JOB_TYPE_NULL;

	if ((job->flags & JOB_TYPE_MASK) == JOB_TYPE_FUSED) {
		dev_err(kctx->kbdev->dev, "Fused jobs can not be replayed\n");
		goto out_unmap;
	}

	if (first_in_chain)
		job->flags |= JOB_FLAG_PERFORM_JOB_BARRIER;

	if ((job->dependencies[0] + hw_job_id_offset) > JOB_HEADER_ID_MAX ||
	    (job->dependencies[1] + hw_job_id_offset) > JOB_HEADER_ID_MAX ||
	    (job->index + hw_job_id_offset) > JOB_HEADER_ID_MAX) {
		dev_err(kctx->kbdev->dev,
			     "Job indicies/dependencies out of valid range\n");
		goto out_unmap;
	}

	if (job->dependencies[0])
		job->dependencies[0] += hw_job_id_offset;
	if (job->dependencies[1])
		job->dependencies[1] += hw_job_id_offset;

	job->index += hw_job_id_offset;

	if (job->flags & JOB_FLAG_DESC_SIZE) {
		new_job_header = job->next._64;
		if (!job->next._64)
			job->next._64 = prev_jc;
	} else {
		new_job_header = job->next._32;
		if (!job->next._32)
			job->next._32 = prev_jc;
	}
	dump_job_head(kctx, "Updated to:", job);

	if ((job->flags & JOB_TYPE_MASK) == JOB_TYPE_TILER) {
		mali_bool job_64 = (job->flags & JOB_FLAG_DESC_SIZE) != 0;

		if (kbasep_replay_reset_tiler_job(kctx, *job_header,
				tiler_heap_free, hierarchy_mask,
				default_weight, job_64) != MALI_ERROR_NONE)
			goto out_unmap;

	} else if ((job->flags & JOB_TYPE_MASK) == JOB_TYPE_FRAGMENT) {
		u64 fbd_address;

		if (job->flags & JOB_FLAG_DESC_SIZE)
			fbd_address = job->fragment_fbd._64;
		else
			fbd_address = (u64)job->fragment_fbd._32;

		if (fbd_address & FBD_TYPE) {
			if (kbasep_replay_reset_mfbd(kctx,
					fbd_address & FBD_POINTER_MASK,
					tiler_heap_free,
					hierarchy_mask,
					default_weight) != MALI_ERROR_NONE)
				goto out_unmap;
		} else {
			if (kbasep_replay_reset_sfbd(kctx,
					fbd_address & FBD_POINTER_MASK,
					tiler_heap_free,
					hierarchy_mask,
					default_weight) != MALI_ERROR_NONE)
				goto out_unmap;
		}
	}

	kbasep_unmap(kctx, &map);

	*job_header = new_job_header;

	return MALI_ERROR_NONE;

out_unmap:
	kbasep_unmap(kctx, &map);
	return MALI_ERROR_FUNCTION_FAILED;
}

/* 
                                                
  
                                         
                                                 
                                                 
  
                                                            
 */
static mali_error kbasep_replay_find_hw_job_id(struct kbase_context *kctx,
		mali_addr64 jc,	u16 *hw_job_id)
{
	while (jc) {
		struct job_head *job;
		struct kbasep_map_struct map;

		dev_dbg(kctx->kbdev->dev,
			"kbasep_replay_find_hw_job_id: parsing jc=%llx\n", jc);

		job = kbasep_map(kctx, jc, sizeof(*job), &map);
		if (!job) {
			dev_err(kctx->kbdev->dev, "failed to map jc\n");

			return MALI_ERROR_FUNCTION_FAILED;
		}

		if (job->index > *hw_job_id)
			*hw_job_id = job->index;

		if (job->flags & JOB_FLAG_DESC_SIZE)
			jc = job->next._64;
		else
			jc = job->next._32;

		kbasep_unmap(kctx, &map);
	}

	return MALI_ERROR_NONE;
}

/* 
                                              
  
                                                        
                                                                           
                               
  
                                                                          
               
  
                                               
                                                         
                                                                     
                                                                     
                                                         
                                                                               
                                                          
                                                     
                                                                             
  
                                                           
 */
static mali_error kbasep_replay_parse_jc(struct kbase_context *kctx,
		mali_addr64 jc,	mali_addr64 prev_jc,
		mali_addr64 tiler_heap_free, u16 hierarchy_mask,
		u32 default_weight, u16 hw_job_id_offset,
		mali_bool fragment_chain)
{
	mali_bool first_in_chain = MALI_TRUE;
	int nr_jobs = 0;

	dev_dbg(kctx->kbdev->dev, "kbasep_replay_parse_jc: jc=%llx hw_job_id=%x\n",
			jc, hw_job_id_offset);

	while (jc) {
		dev_dbg(kctx->kbdev->dev, "kbasep_replay_parse_jc: parsing jc=%llx\n", jc);

		if (kbasep_replay_reset_job(kctx, &jc, prev_jc,
				tiler_heap_free, hierarchy_mask,
				default_weight, hw_job_id_offset,
				first_in_chain, fragment_chain) != MALI_ERROR_NONE)
			return MALI_ERROR_FUNCTION_FAILED;

		first_in_chain = MALI_FALSE;

		nr_jobs++;
		if (fragment_chain &&
		    nr_jobs >= BASE_JD_REPLAY_F_CHAIN_JOB_LIMIT) {
			dev_err(kctx->kbdev->dev,
				"Exceeded maximum number of jobs in fragment chain\n");
			return MALI_ERROR_FUNCTION_FAILED;
		}
	}

	return MALI_ERROR_NONE;
}

/* 
                                                                   
  
                                                                           
                                       
  
                                            
                                                                 
 */
static void kbasep_replay_reset_softjob(struct kbase_jd_atom *katom,
		struct kbase_jd_atom *dep_atom)
{
	katom->status = KBASE_JD_ATOM_STATE_QUEUED;
	kbase_jd_katom_dep_set(&katom->dep[0], dep_atom, BASE_JD_DEP_TYPE_DATA);
	list_add_tail(&katom->dep_item[0], &dep_atom->dep_head[0]);
}

/* 
                                  
  
                                                                              
                                 
  
                                                         
  
                                       
                                       
 */
static int kbasep_allocate_katom(struct kbase_context *kctx)
{
	struct kbase_jd_context *jctx = &kctx->jctx;
	int i;

	for (i = BASE_JD_ATOM_COUNT-1; i > 0; i--) {
		if (jctx->atoms[i].status == KBASE_JD_ATOM_STATE_UNUSED) {
			jctx->atoms[i].status = KBASE_JD_ATOM_STATE_QUEUED;
			dev_dbg(kctx->kbdev->dev,
				  "kbasep_allocate_katom: Allocated atom %d\n",
									    i);
			return i;
		}
	}

	return -1;
}

/* 
                         
  
                                                                              
  
                         
  
                                       
                                             
 */
static void kbasep_release_katom(struct kbase_context *kctx, int atom_id)
{
	struct kbase_jd_context *jctx = &kctx->jctx;

	dev_dbg(kctx->kbdev->dev, "kbasep_release_katom: Released atom %d\n",
			atom_id);

	while (!list_empty(&jctx->atoms[atom_id].dep_head[0]))
		list_del(jctx->atoms[atom_id].dep_head[0].next);

	while (!list_empty(&jctx->atoms[atom_id].dep_head[1]))
		list_del(jctx->atoms[atom_id].dep_head[1].next);

	jctx->atoms[atom_id].status = KBASE_JD_ATOM_STATE_UNUSED;
}

static void kbasep_replay_create_atom(struct kbase_context *kctx,
				      struct base_jd_atom_v2 *atom,
				      int atom_nr,
				      int prio)
{
	atom->nr_extres = 0;
	atom->extres_list.value = NULL;
	atom->device_nr = 0;
	/*                                       */
	atom->prio = ((prio << 16) / ((20 << 16) / 128)) - 128;
	atom->atom_number = atom_nr;

	base_jd_atom_dep_set(&atom->pre_dep[0], 0 , BASE_JD_DEP_TYPE_INVALID);
	base_jd_atom_dep_set(&atom->pre_dep[1], 0 , BASE_JD_DEP_TYPE_INVALID);

	atom->udata.blob[0] = 0;
	atom->udata.blob[1] = 0;
}

/* 
                                                            
  
                                                                         
                                                                             
                          
  
                                                                               
                 
                                                   
                                                           
                                               
  
                                                     
                                                        
                                                                           
                               
                                                            
 */
static mali_error kbasep_replay_create_atoms(struct kbase_context *kctx,
		struct base_jd_atom_v2 *t_atom,
		struct base_jd_atom_v2 *f_atom, int prio)
{
	int t_atom_nr, f_atom_nr;

	t_atom_nr = kbasep_allocate_katom(kctx);
	if (t_atom_nr < 0) {
		dev_err(kctx->kbdev->dev, "Failed to allocate katom\n");
		return MALI_ERROR_FUNCTION_FAILED;
	}

	f_atom_nr = kbasep_allocate_katom(kctx);
	if (f_atom_nr < 0) {
		dev_err(kctx->kbdev->dev, "Failed to allocate katom\n");
		kbasep_release_katom(kctx, t_atom_nr);
		return MALI_ERROR_FUNCTION_FAILED;
	}

	kbasep_replay_create_atom(kctx, t_atom, t_atom_nr, prio);
	kbasep_replay_create_atom(kctx, f_atom, f_atom_nr, prio);

	base_jd_atom_dep_set(&f_atom->pre_dep[0], t_atom_nr , BASE_JD_DEP_TYPE_DATA);

	return MALI_ERROR_NONE;
}

#ifdef CONFIG_MALI_DEBUG
static void payload_dump(struct kbase_context *kctx, base_jd_replay_payload *payload)
{
	mali_addr64 next;

	dev_dbg(kctx->kbdev->dev, "Tiler jc list :\n");
	next = payload->tiler_jc_list;

	while (next) {
		struct kbasep_map_struct map;
		base_jd_replay_jc *jc_struct;

		jc_struct = kbasep_map(kctx, next, sizeof(*jc_struct), &map);

		if (!jc_struct)
			return;

		dev_dbg(kctx->kbdev->dev, "* jc_struct=%p jc=%llx next=%llx\n",
				jc_struct, jc_struct->jc, jc_struct->next);

		next = jc_struct->next;

		kbasep_unmap(kctx, &map);
	}
}
#endif

/* 
                                                              
  
                                                                       
  
                                          
                                               
                                                     
                                                        
                                                             
 */
static mali_error kbasep_replay_parse_payload(struct kbase_context *kctx,
					      struct kbase_jd_atom *replay_atom,
					      struct base_jd_atom_v2 *t_atom,
					      struct base_jd_atom_v2 *f_atom)
{
	base_jd_replay_payload *payload;
	mali_addr64 next;
	mali_addr64 prev_jc = 0;
	u16 hw_job_id_offset = 0;
	mali_error ret = MALI_ERROR_FUNCTION_FAILED;
	struct kbasep_map_struct map;

	dev_dbg(kctx->kbdev->dev, "kbasep_replay_parse_payload: replay_atom->jc = %llx sizeof(payload) = %zu\n",
			replay_atom->jc, sizeof(payload));

	kbase_gpu_vm_lock(kctx);

	payload = kbasep_map(kctx, replay_atom->jc, sizeof(*payload), &map);

	if (!payload) {
		kbase_gpu_vm_unlock(kctx);
		dev_err(kctx->kbdev->dev, "kbasep_replay_parse_payload: failed to map payload into kernel space\n");
		return MALI_ERROR_FUNCTION_FAILED;
	}

#ifdef CONFIG_MALI_DEBUG
	dev_dbg(kctx->kbdev->dev, "kbasep_replay_parse_payload: payload=%p\n", payload);
	dev_dbg(kctx->kbdev->dev, "Payload structure:\n"
				  "tiler_jc_list            = %llx\n"
				  "fragment_jc              = %llx\n"
				  "tiler_heap_free          = %llx\n"
				  "fragment_hierarchy_mask  = %x\n"
				  "tiler_hierarchy_mask     = %x\n"
				  "hierarchy_default_weight = %x\n"
				  "tiler_core_req           = %x\n"
				  "fragment_core_req        = %x\n",
							payload->tiler_jc_list,
							  payload->fragment_jc,
						      payload->tiler_heap_free,
					      payload->fragment_hierarchy_mask,
						 payload->tiler_hierarchy_mask,
					     payload->hierarchy_default_weight,
						       payload->tiler_core_req,
						   payload->fragment_core_req);
	payload_dump(kctx, payload);
#endif

	t_atom->core_req = payload->tiler_core_req | BASEP_JD_REQ_EVENT_NEVER;
	f_atom->core_req = payload->fragment_core_req | BASEP_JD_REQ_EVENT_NEVER;

	/*                               */
	if ((t_atom->core_req & BASEP_JD_REQ_ATOM_TYPE &
			       ~BASE_JD_REQ_COHERENT_GROUP) != BASE_JD_REQ_T ||
	    (f_atom->core_req & BASEP_JD_REQ_ATOM_TYPE &
			      ~BASE_JD_REQ_COHERENT_GROUP) != BASE_JD_REQ_FS ||
	     t_atom->core_req & BASE_JD_REQ_EXTERNAL_RESOURCES ||
	     f_atom->core_req & BASE_JD_REQ_EXTERNAL_RESOURCES) {
		dev_err(kctx->kbdev->dev, "Invalid core requirements\n");
		goto out;
	}

	/*                          */
	next = payload->tiler_jc_list;
	if (!next) {
		dev_err(kctx->kbdev->dev, "Invalid tiler JC list\n");
		goto out;
	}

	while (next) {
		base_jd_replay_jc *jc_struct;
		struct kbasep_map_struct jc_map;
		mali_addr64 jc;

		jc_struct = kbasep_map(kctx, next, sizeof(*jc_struct), &jc_map);

		if (!jc_struct) {
			dev_err(kctx->kbdev->dev, "Failed to map jc struct\n");
			goto out;
		}

		jc = jc_struct->jc;
		next = jc_struct->next;
		if (next)
			jc_struct->jc = 0;

		kbasep_unmap(kctx, &jc_map);

		if (jc) {
			u16 max_hw_job_id = 0;

			if (kbasep_replay_find_hw_job_id(kctx, jc,
					&max_hw_job_id) != MALI_ERROR_NONE)
				goto out;

			if (kbasep_replay_parse_jc(kctx, jc, prev_jc,
					payload->tiler_heap_free,
					payload->tiler_hierarchy_mask,
					payload->hierarchy_default_weight,
					hw_job_id_offset, MALI_FALSE) !=
					MALI_ERROR_NONE) {
				goto out;
			}

			hw_job_id_offset += max_hw_job_id;

			prev_jc = jc;
		}
	}
	t_atom->jc = prev_jc;

	/*                            */
	f_atom->jc = payload->fragment_jc;
	if (kbasep_replay_parse_jc(kctx, payload->fragment_jc, 0,
			payload->tiler_heap_free,
			payload->fragment_hierarchy_mask,
			payload->hierarchy_default_weight, 0,
			MALI_TRUE) != MALI_ERROR_NONE) {
		goto out;
	}

	if (!t_atom->jc || !f_atom->jc) {
		dev_err(kctx->kbdev->dev, "Invalid payload\n");
		goto out;
	}

	dev_dbg(kctx->kbdev->dev, "t_atom->jc=%llx f_atom->jc=%llx\n",
			t_atom->jc, f_atom->jc);
	ret = MALI_ERROR_NONE;

out:
	kbasep_unmap(kctx, &map);

	kbase_gpu_vm_unlock(kctx);

	return ret;
}

static void kbase_replay_process_worker(struct work_struct *data)
{
	struct kbase_jd_atom *katom;
	struct kbase_context *kctx;
	struct kbase_jd_context *jctx;
	bool need_to_try_schedule_context = false;

	struct base_jd_atom_v2 t_atom, f_atom;
	struct kbase_jd_atom *t_katom, *f_katom;

	katom = container_of(data, struct kbase_jd_atom, work);
	kctx = katom->kctx;
	jctx = &kctx->jctx;

	mutex_lock(&jctx->lock);

	if (kbasep_replay_create_atoms(kctx, &t_atom, &f_atom,
				       katom->nice_prio) != MALI_ERROR_NONE) {
		katom->event_code = BASE_JD_EVENT_JOB_CANCELLED;
		goto out;
	}

	t_katom = &jctx->atoms[t_atom.atom_number];
	f_katom = &jctx->atoms[f_atom.atom_number];

	if (kbasep_replay_parse_payload(kctx, katom, &t_atom, &f_atom) !=
							     MALI_ERROR_NONE) {
		kbasep_release_katom(kctx, t_atom.atom_number);
		kbasep_release_katom(kctx, f_atom.atom_number);
		katom->event_code = BASE_JD_EVENT_JOB_CANCELLED;
		goto out;
	}

	kbasep_replay_reset_softjob(katom, f_katom);

	need_to_try_schedule_context |= jd_submit_atom(kctx, &t_atom, t_katom);
	if (t_katom->event_code == BASE_JD_EVENT_JOB_INVALID) {
		dev_err(kctx->kbdev->dev, "Replay failed to submit atom\n");
		kbasep_release_katom(kctx, f_atom.atom_number);
		katom->event_code = BASE_JD_EVENT_JOB_CANCELLED;
		goto out;
	}
	need_to_try_schedule_context |= jd_submit_atom(kctx, &f_atom, f_katom);
	if (f_katom->event_code == BASE_JD_EVENT_JOB_INVALID) {
		dev_err(kctx->kbdev->dev, "Replay failed to submit atom\n");
		katom->event_code = BASE_JD_EVENT_JOB_CANCELLED;
		goto out;
	}

	katom->event_code = BASE_JD_EVENT_DONE;

out:
	if (katom->event_code != BASE_JD_EVENT_DONE)
		need_to_try_schedule_context |= jd_done_nolock(katom);

	if (need_to_try_schedule_context)
		kbasep_js_try_schedule_head_ctx(kctx->kbdev);
	mutex_unlock(&jctx->lock);
}

/* 
                                
  
                                                                             
                              
  
                                                  
                                                                  
 */
static bool kbase_replay_fault_check(struct kbase_jd_atom *katom)
{
	struct kbase_context *kctx = katom->kctx;
	struct device *dev = kctx->kbdev->dev;
	base_jd_replay_payload *payload;
	mali_addr64 job_header;
	mali_addr64 job_loop_detect;
	struct job_head *job;
	struct kbasep_map_struct job_map;
	struct kbasep_map_struct map;
	bool err = false;

	/*                                                                
                               
  */
	if (BASE_JD_EVENT_TERMINATED == katom->event_code) {
		return false;
	} else if (BASE_JD_EVENT_JOB_WRITE_FAULT == katom->event_code) {
		return true;
	} else if (BASE_JD_EVENT_FORCE_REPLAY == katom->event_code) {
		katom->event_code = BASE_JD_EVENT_DATA_INVALID_FAULT;
		return true;
	} else if (BASE_JD_EVENT_DATA_INVALID_FAULT != katom->event_code) {
		/*                                        
                                      
   */
		return false;
	}

	/*                                                                    
                                                                       
                                                       
  */
	kbase_gpu_vm_lock(kctx);

	payload = kbasep_map(kctx, katom->jc, sizeof(*payload), &map);
	if (!payload) {
		kbase_gpu_vm_unlock(kctx);
		dev_err(dev, "kbase_replay_fault_check: failed to map payload.\n");
		return false;
	}

#ifdef CONFIG_MALI_DEBUG
	dev_dbg(dev, "kbase_replay_fault_check: payload=%p\n", payload);
	dev_dbg(dev, "\nPayload structure:\n"
		     "fragment_jc              = 0x%llx\n"
		     "fragment_hierarchy_mask  = 0x%x\n"
		     "fragment_core_req        = 0x%x\n",
		     payload->fragment_jc,
		     payload->fragment_hierarchy_mask,
		     payload->fragment_core_req);
#endif
	/*                            */
	job_header      = (mali_addr64) payload->fragment_jc;
	job_loop_detect = job_header;
	while (job_header) {
		job = kbasep_map(kctx, job_header, sizeof(*job), &job_map);
		if (!job) {
			dev_err(dev, "failed to map jc\n");
			/*              */
			kbasep_unmap(kctx, &map);
			kbase_gpu_vm_unlock(kctx);
			return false;
		}


#ifdef CONFIG_MALI_DEBUG
		dev_dbg(dev, "\njob_head structure:\n"
			     "Source ID:0x%x Access:0x%x Exception:0x%x\n"
			     "at job addr               = %p\n"
			     "not_complete_index        = 0x%x\n"
			     "fault_addr                = 0x%llx\n"
			     "flags                     = 0x%x\n"
			     "index                     = 0x%x\n"
			     "dependencies              = 0x%x,0x%x\n",
			     JOB_SOURCE_ID(job->status),
			     ((job->status >> 8) & 0x3),
			     (job->status  & 0xFF),
			     job,
			     job->not_complete_index,
			     job->fault_addr,
			     job->flags,
			     job->index,
			     job->dependencies[0],
			     job->dependencies[1]);
#endif

		/*                                                    
                        */
		if ((BASE_JD_EVENT_DATA_INVALID_FAULT == katom->event_code) &&
		    (JOB_POLYGON_LIST == JOB_SOURCE_ID(job->status))) {
			err = true;
			kbasep_unmap(kctx, &job_map);
			break;
		}

		/*                                          */
		if (job->flags & JOB_FLAG_DESC_SIZE)
			job_header = job->next._64;
		else
			job_header = job->next._32;

		kbasep_unmap(kctx, &job_map);

		/*                         */
		if (job_header == job_loop_detect)
			break;
	}

	/*              */
	kbasep_unmap(kctx, &map);
	kbase_gpu_vm_unlock(kctx);

	return err;
}


/* 
                              
  
                                      
  
                                                                               
                                                                            
                                                      
  
                                             
                                                    
                                                       
 */
bool kbase_replay_process(struct kbase_jd_atom *katom)
{
	struct kbase_context *kctx = katom->kctx;
	struct kbase_jd_context *jctx = &kctx->jctx;

	if (katom->event_code == BASE_JD_EVENT_DONE) {
		dev_dbg(kctx->kbdev->dev, "Previous job succeeded - not replaying\n");

		if (katom->retry_count)
			kbase_disjoint_state_down(kctx->kbdev);

		return false;
	}

	if (jctx->sched_info.ctx.is_dying) {
		dev_dbg(kctx->kbdev->dev, "Not replaying; context is dying\n");

		if (katom->retry_count)
			kbase_disjoint_state_down(kctx->kbdev);

		return false;
	}

	/*                                                       */
	if (false == kbase_replay_fault_check(katom)) {
		dev_dbg(kctx->kbdev->dev,
			"Replay cancelled on event %x\n", katom->event_code);
		/*                                                            
                  
   */
		return false;
	}

	dev_warn(kctx->kbdev->dev, "Replaying jobs retry=%d\n",
			katom->retry_count);

	katom->retry_count++;

	if (katom->retry_count > BASEP_JD_REPLAY_LIMIT) {
		dev_err(kctx->kbdev->dev, "Replay exceeded limit - failing jobs\n");

		kbase_disjoint_state_down(kctx->kbdev);

		/*                                                            
                  */
		return false;
	}

	/*                                                                                   */
	if (katom->retry_count == 1)
		kbase_disjoint_state_up(kctx->kbdev);

	INIT_WORK(&katom->work, kbase_replay_process_worker);
	queue_work(kctx->event_workq, &katom->work);

	return true;
}
