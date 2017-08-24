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





#if defined(CONFIG_DMA_SHARED_BUFFER)
#include <linux/dma-buf.h>
#endif				/*                                   */
#ifdef CONFIG_COMPAT
#include <linux/compat.h>
#endif
#include <mali_kbase.h>
#include <mali_kbase_uku.h>
#include <mali_kbase_js_affinity.h>
#include <mali_kbase_10969_workaround.h>
#ifdef CONFIG_UMP
#include <linux/ump.h>
#endif				/*            */
#include <linux/random.h>

#define beenthere(kctx, f, a...)  dev_dbg(kctx->kbdev->dev, "%s:" f, __func__, ##a)

#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 8, 0)
/*                                            */
#define prandom_u32 random32
#endif

/*
                                                             
                                                                    
                                                                
                         
            
                                       
                                                                               
 */

static void __user *
get_compat_pointer(struct kbase_context *kctx, const union kbase_pointer *p)
{
#ifdef CONFIG_COMPAT
	if (kctx->is_compat)
		return compat_ptr(p->compat_value);
	else
#endif
		return p->value;
}

/*                                                                                                
  
                                             
  
                                                           
                                                                  
 */
static int jd_run_atom(struct kbase_jd_atom *katom)
{
	struct kbase_context *kctx = katom->kctx;

	KBASE_DEBUG_ASSERT(katom->status != KBASE_JD_ATOM_STATE_UNUSED);

	if ((katom->core_req & BASEP_JD_REQ_ATOM_TYPE) == BASE_JD_REQ_DEP) {
		/*                      */
		katom->status = KBASE_JD_ATOM_STATE_COMPLETED;
		return 0;
	} else if (katom->core_req & BASE_JD_REQ_SOFT_JOB) {
		/*          */
		if ((katom->core_req & BASEP_JD_REQ_ATOM_TYPE)
						  == BASE_JD_REQ_SOFT_REPLAY) {
			if (!kbase_replay_process(katom))
				katom->status = KBASE_JD_ATOM_STATE_COMPLETED;
		} else if (kbase_process_soft_job(katom) == 0) {
			kbase_finish_soft_job(katom);
			katom->status = KBASE_JD_ATOM_STATE_COMPLETED;
		} else {
			/*                           */
			list_add_tail(&katom->dep_item[0], &kctx->waiting_soft_jobs);
		}
		return 0;
	}

	katom->status = KBASE_JD_ATOM_STATE_IN_JS;
	/*                                                                  */
	return kbasep_js_add_job(kctx, katom);
}

#ifdef CONFIG_KDS

/*                                       
                                                                                        
                                                  */

static void kbase_jd_kds_waiters_add(struct kbase_jd_atom *katom)
{
	struct kbase_context *kctx;

	KBASE_DEBUG_ASSERT(katom);

	kctx = katom->kctx;

	list_add_tail(&katom->node, &kctx->waiting_kds_resource);
}

/*                                            
                                                                                              
                                                                                                     
                                                  */

static void kbase_jd_kds_waiters_remove(struct kbase_jd_atom *katom)
{
	KBASE_DEBUG_ASSERT(katom);
	list_del(&katom->node);
}

static void kds_dep_clear(void *callback_parameter, void *callback_extra_parameter)
{
	struct kbase_jd_atom *katom;
	struct kbase_jd_context *ctx;
	struct kbase_device *kbdev;

	katom = (struct kbase_jd_atom *)callback_parameter;
	KBASE_DEBUG_ASSERT(katom);
	ctx = &katom->kctx->jctx;
	kbdev = katom->kctx->kbdev;
	KBASE_DEBUG_ASSERT(kbdev);

	mutex_lock(&ctx->lock);

	/*                                                               */
	if (katom->kds_dep_satisfied)
		goto out;

	/*                                             */
	katom->kds_dep_satisfied = MALI_TRUE;

	/*                                                              */
	if (!kbase_jd_katom_dep_atom(&katom->dep[0]) &&
			!kbase_jd_katom_dep_atom(&katom->dep[1])) {
		/*                                       */
		mali_bool resched = MALI_FALSE;

		resched = jd_run_atom(katom);

		if (katom->status == KBASE_JD_ATOM_STATE_COMPLETED) {
			/*                               */
			resched |= jd_done_nolock(katom);
		}

		if (resched)
			kbasep_js_try_schedule_head_ctx(kbdev);
	}
 out:
	mutex_unlock(&ctx->lock);
}

static void kbase_cancel_kds_wait_job(struct kbase_jd_atom *katom)
{
	KBASE_DEBUG_ASSERT(katom);

	/*                                                                
                                                             */

	if (katom->status == KBASE_JD_ATOM_STATE_QUEUED) {
		/*                                   */
		katom->event_code = BASE_JD_EVENT_JOB_CANCELLED;
		if (jd_done_nolock(katom))
			kbasep_js_try_schedule_head_ctx(katom->kctx->kbdev);
	}
}
#endif				/*            */

#ifdef CONFIG_DMA_SHARED_BUFFER
static mali_error kbase_jd_umm_map(struct kbase_context *kctx, struct kbase_va_region *reg)
{
	struct sg_table *sgt;
	struct scatterlist *s;
	int i;
	phys_addr_t *pa;
	mali_error err;
	size_t count = 0;

	KBASE_DEBUG_ASSERT(reg->alloc->type == KBASE_MEM_TYPE_IMPORTED_UMM);
	KBASE_DEBUG_ASSERT(NULL == reg->alloc->imported.umm.sgt);
	sgt = dma_buf_map_attachment(reg->alloc->imported.umm.dma_attachment, DMA_BIDIRECTIONAL);

	if (IS_ERR_OR_NULL(sgt))
		return MALI_ERROR_FUNCTION_FAILED;

	/*                */
	reg->alloc->imported.umm.sgt = sgt;

	pa = kbase_get_phy_pages(reg);
	KBASE_DEBUG_ASSERT(pa);

	for_each_sg(sgt->sgl, s, sgt->nents, i) {
		int j;
		size_t pages = PFN_UP(sg_dma_len(s));

		WARN_ONCE(sg_dma_len(s) & (PAGE_SIZE-1),
		"sg_dma_len(s)=%u is not a multiple of PAGE_SIZE\n",
		sg_dma_len(s));

		WARN_ONCE(sg_dma_address(s) & (PAGE_SIZE-1),
		"sg_dma_address(s)=%llx is not aligned to PAGE_SIZE\n",
		(unsigned long long) sg_dma_address(s));

		for (j = 0; (j < pages) && (count < reg->nr_pages); j++, count++)
			*pa++ = sg_dma_address(s) + (j << PAGE_SHIFT);
		WARN_ONCE(j < pages,
		"sg list from dma_buf_map_attachment > dma_buf->size=%zu\n",
		reg->alloc->imported.umm.dma_buf->size);
	}

	if (WARN_ONCE(count < reg->nr_pages,
			"sg list from dma_buf_map_attachment < dma_buf->size=%zu\n",
			reg->alloc->imported.umm.dma_buf->size)) {
		err = MALI_ERROR_FUNCTION_FAILED;
		goto out;
	}

	/*                                          */
	reg->alloc->nents = count;

	err = kbase_mmu_insert_pages(kctx, reg->start_pfn, kbase_get_phy_pages(reg), kbase_reg_current_backed_size(reg), reg->flags | KBASE_REG_GPU_WR | KBASE_REG_GPU_RD);

out:
	if (MALI_ERROR_NONE != err) {
		dma_buf_unmap_attachment(reg->alloc->imported.umm.dma_attachment, reg->alloc->imported.umm.sgt, DMA_BIDIRECTIONAL);
		reg->alloc->imported.umm.sgt = NULL;
	}

	return err;
}

static void kbase_jd_umm_unmap(struct kbase_context *kctx, struct kbase_mem_phy_alloc *alloc)
{
	KBASE_DEBUG_ASSERT(kctx);
	KBASE_DEBUG_ASSERT(alloc);
	KBASE_DEBUG_ASSERT(alloc->imported.umm.dma_attachment);
	KBASE_DEBUG_ASSERT(alloc->imported.umm.sgt);
	dma_buf_unmap_attachment(alloc->imported.umm.dma_attachment,
	    alloc->imported.umm.sgt, DMA_BIDIRECTIONAL);
	alloc->imported.umm.sgt = NULL;
	alloc->nents = 0;
}
#endif				/*                          */

void kbase_jd_free_external_resources(struct kbase_jd_atom *katom)
{
#ifdef CONFIG_KDS
	if (katom->kds_rset) {
		struct kbase_jd_context *jctx = &katom->kctx->jctx;

		/*
                                                     
                      
   */

		mutex_lock(&jctx->lock);
		kbase_jd_kds_waiters_remove(katom);
		mutex_unlock(&jctx->lock);

		/*                                               */
		kds_resource_set_release_sync(&katom->kds_rset);
	}
#endif				/*            */
}

static void kbase_jd_post_external_resources(struct kbase_jd_atom *katom)
{
	KBASE_DEBUG_ASSERT(katom);
	KBASE_DEBUG_ASSERT(katom->core_req & BASE_JD_REQ_EXTERNAL_RESOURCES);

#ifdef CONFIG_KDS
	/*                                                                      */
	if (katom->kds_rset)
		katom->kds_dep_satisfied = MALI_TRUE;
#endif				/*            */

	kbase_gpu_vm_lock(katom->kctx);
	/*                                      */
	if (katom->extres) {
		u32 res_no;

		res_no = katom->nr_extres;
		while (res_no-- > 0) {
			struct kbase_mem_phy_alloc *alloc = katom->extres[res_no].alloc;
#ifdef CONFIG_DMA_SHARED_BUFFER
			if (alloc->type == KBASE_MEM_TYPE_IMPORTED_UMM) {
				alloc->imported.umm.current_mapping_usage_count--;

				if (0 == alloc->imported.umm.current_mapping_usage_count) {
					struct kbase_va_region *reg;

					reg = kbase_region_tracker_find_region_base_address(
							katom->kctx,
							katom->extres[res_no].gpu_address);

					if (reg && reg->alloc == alloc)
						kbase_mmu_teardown_pages(
								katom->kctx,
								reg->start_pfn,
								kbase_reg_current_backed_size(reg));

					kbase_jd_umm_unmap(katom->kctx, alloc);
				}
			}
#endif	/*                          */
			kbase_mem_phy_alloc_put(katom->extres[res_no].alloc);
		}
		kfree(katom->extres);
		katom->extres = NULL;
	}
	kbase_gpu_vm_unlock(katom->kctx);
}

#if (defined(CONFIG_KDS) && defined(CONFIG_UMP)) || defined(CONFIG_DMA_SHARED_BUFFER_USES_KDS)
static void add_kds_resource(struct kds_resource *kds_res, struct kds_resource **kds_resources, u32 *kds_res_count, unsigned long *kds_access_bitmap, mali_bool exclusive)
{
	u32 i;

	for (i = 0; i < *kds_res_count; i++) {
		/*                            */
		if (kds_resources[i] == kds_res)
			return;
	}

	kds_resources[*kds_res_count] = kds_res;
	if (exclusive)
		set_bit(*kds_res_count, kds_access_bitmap);
	(*kds_res_count)++;
}
#endif

/*
                                                
  
                                              
 */

static mali_error kbase_jd_pre_external_resources(struct kbase_jd_atom *katom, const struct base_jd_atom_v2 *user_atom)
{
	mali_error err_ret_val = MALI_ERROR_FUNCTION_FAILED;
	u32 res_no;
#ifdef CONFIG_KDS
	u32 kds_res_count = 0;
	struct kds_resource **kds_resources = NULL;
	unsigned long *kds_access_bitmap = NULL;
#endif				/*            */
	struct base_external_resource *input_extres;

	KBASE_DEBUG_ASSERT(katom);
	KBASE_DEBUG_ASSERT(katom->core_req & BASE_JD_REQ_EXTERNAL_RESOURCES);

	/*                                 */
	if (!katom->nr_extres)
		return MALI_ERROR_FUNCTION_FAILED;

	katom->extres = kmalloc_array(katom->nr_extres, sizeof(*katom->extres), GFP_KERNEL);
	if (NULL == katom->extres) {
		err_ret_val = MALI_ERROR_OUT_OF_MEMORY;
		goto early_err_out;
	}

	/*                                                
                                                       
                     */
	BUILD_BUG_ON(sizeof(*input_extres) > sizeof(*katom->extres));
	input_extres = (struct base_external_resource *)
			(((unsigned char *)katom->extres) +
			(sizeof(*katom->extres) - sizeof(*input_extres)) *
			katom->nr_extres);

	if (copy_from_user(input_extres,
			get_compat_pointer(katom->kctx, &user_atom->extres_list),
			sizeof(*input_extres) * katom->nr_extres) != 0) {
		err_ret_val = MALI_ERROR_FUNCTION_FAILED;
		goto early_err_out;
	}
#ifdef CONFIG_KDS
	/*                                */
	KBASE_DEBUG_ASSERT(0 != katom->nr_extres);
	kds_resources = kmalloc_array(katom->nr_extres, sizeof(struct kds_resource *), GFP_KERNEL);

	if (NULL == kds_resources) {
		err_ret_val = MALI_ERROR_OUT_OF_MEMORY;
		goto early_err_out;
	}

	KBASE_DEBUG_ASSERT(0 != katom->nr_extres);
	kds_access_bitmap = kzalloc(sizeof(unsigned long) * ((katom->nr_extres + BITS_PER_LONG - 1) / BITS_PER_LONG), GFP_KERNEL);

	if (NULL == kds_access_bitmap) {
		err_ret_val = MALI_ERROR_OUT_OF_MEMORY;
		goto early_err_out;
	}
#endif				/*            */

	/*                                                            */
	kbase_gpu_vm_lock(katom->kctx);
	for (res_no = 0; res_no < katom->nr_extres; res_no++) {
		struct base_external_resource *res;
		struct kbase_va_region *reg;

		res = &input_extres[res_no];
		reg = kbase_region_tracker_find_region_enclosing_address(katom->kctx,
				res->ext_resource & ~BASE_EXT_RES_ACCESS_EXCLUSIVE);
		/*                                       */
		if (NULL == reg || (reg->flags & KBASE_REG_FREE)) {
			/*           */
			goto failed_loop;
		}

		/*                                               */
		switch (reg->alloc->type) {
		case BASE_TMEM_IMPORT_TYPE_UMP:
			{
#if defined(CONFIG_KDS) && defined(CONFIG_UMP)
				struct kds_resource *kds_res;

				kds_res = ump_dd_kds_resource_get(reg->alloc->imported.ump_handle);
				if (kds_res)
					add_kds_resource(kds_res, kds_resources, &kds_res_count,
							kds_access_bitmap,
							res->ext_resource & BASE_EXT_RES_ACCESS_EXCLUSIVE);
#endif				/*                                           */
				break;
			}
#ifdef CONFIG_DMA_SHARED_BUFFER
		case BASE_TMEM_IMPORT_TYPE_UMM:
			{
#ifdef CONFIG_DMA_SHARED_BUFFER_USES_KDS
				struct kds_resource *kds_res;

				kds_res = get_dma_buf_kds_resource(reg->alloc->imported.umm.dma_buf);
				if (kds_res)
					add_kds_resource(kds_res, kds_resources, &kds_res_count, kds_access_bitmap, res->ext_resource & BASE_EXT_RES_ACCESS_EXCLUSIVE);
#endif
				reg->alloc->imported.umm.current_mapping_usage_count++;
				if (1 == reg->alloc->imported.umm.current_mapping_usage_count) {
					/*                                                
                                                                 
                                                        */
					mali_error tmp;
					tmp = kbase_jd_umm_map(katom->kctx, reg);
					if (MALI_ERROR_NONE != tmp) {
						/*                                      */
						err_ret_val = tmp;
						reg->alloc->imported.umm.current_mapping_usage_count--;
						goto failed_loop;
					}
				}
				break;
			}
#endif
		default:
			goto failed_loop;
		}

		/*                                                       */
		/*                                                            
                                                                  
                                                              
                                        
     */
		katom->extres[res_no].gpu_address = reg->start_pfn << PAGE_SHIFT; /*                                                                      */
		katom->extres[res_no].alloc = kbase_mem_phy_alloc_get(reg->alloc);
	}
	/*                                      */
	/*                                          */
	kbase_gpu_vm_unlock(katom->kctx);

#ifdef CONFIG_KDS
	if (kds_res_count) {
		int wait_failed;

		/*                                        */
		katom->kds_dep_satisfied = MALI_FALSE;

		wait_failed = kds_async_waitall(&katom->kds_rset,
				&katom->kctx->jctx.kds_cb, katom, NULL,
				kds_res_count, kds_access_bitmap,
				kds_resources);

		if (wait_failed)
			goto failed_kds_setup;
		else
			kbase_jd_kds_waiters_add(katom);
	} else {
		/*                                     */
		katom->kds_dep_satisfied = MALI_TRUE;
	}
	kfree(kds_resources);
	kfree(kds_access_bitmap);
#endif				/*            */

	/*             */
	return MALI_ERROR_NONE;

/*                        */

#ifdef CONFIG_KDS
 failed_kds_setup:

	/*                      */
	kbase_gpu_vm_lock(katom->kctx);
#endif				/*            */

 failed_loop:
	/*                    */
	while (res_no-- > 0) {
		struct kbase_mem_phy_alloc *alloc = katom->extres[res_no].alloc;
#ifdef CONFIG_DMA_SHARED_BUFFER
		if (alloc->type == KBASE_MEM_TYPE_IMPORTED_UMM) {
			alloc->imported.umm.current_mapping_usage_count--;

			if (0 == alloc->imported.umm.current_mapping_usage_count) {
				struct kbase_va_region *reg;

				reg = kbase_region_tracker_find_region_base_address(
						katom->kctx,
						katom->extres[res_no].gpu_address);

				if (reg && reg->alloc == alloc) 
					kbase_mmu_teardown_pages(katom->kctx,
							reg->start_pfn,
					    		kbase_reg_current_backed_size(reg));

				kbase_jd_umm_unmap(katom->kctx, alloc);
			}
		}
#endif				/*                          */
		kbase_mem_phy_alloc_put(alloc);
	}
	kbase_gpu_vm_unlock(katom->kctx);

 early_err_out:
	kfree(katom->extres);
	katom->extres = NULL;
#ifdef CONFIG_KDS
	kfree(kds_resources);
	kfree(kds_access_bitmap);
#endif				/*            */
	return err_ret_val;
}

STATIC INLINE void jd_resolve_dep(struct list_head *out_list, struct kbase_jd_atom *katom, u8 d)
{
	u8 other_d = !d;

	while (!list_empty(&katom->dep_head[d])) {
		struct kbase_jd_atom *dep_atom;

		dep_atom = list_entry(katom->dep_head[d].next, 
				struct kbase_jd_atom, dep_item[d]);

		list_del(katom->dep_head[d].next);

		kbase_jd_katom_dep_clear(&dep_atom->dep[d]);

		if (katom->event_code != BASE_JD_EVENT_DONE) {
			/*                                                                             */
			if (kbase_jd_katom_dep_atom(&dep_atom->dep[other_d])) {
				list_del(&dep_atom->dep_item[other_d]);
				kbase_jd_katom_dep_clear(&dep_atom->dep[other_d]);
			}
#ifdef CONFIG_KDS
			if (!dep_atom->kds_dep_satisfied) {
				/*                                                                                                  
                                                                                                     
     */
				dep_atom->kds_dep_satisfied = MALI_TRUE;
			}
#endif

			/*                                                                 */
			if (!(kbase_jd_katom_dep_type(&dep_atom->dep[d]) == BASE_JD_DEP_TYPE_ORDER &&
					katom->event_code > BASE_JD_EVENT_ACTIVE)) {
				dep_atom->event_code = katom->event_code;
				KBASE_DEBUG_ASSERT(dep_atom->status != KBASE_JD_ATOM_STATE_UNUSED);
				dep_atom->status = KBASE_JD_ATOM_STATE_COMPLETED;
			}

			list_add_tail(&dep_atom->dep_item[0], out_list);
		} else if (!kbase_jd_katom_dep_atom(&dep_atom->dep[other_d])) {
#ifdef CONFIG_KDS
			if (dep_atom->kds_dep_satisfied)
#endif
				list_add_tail(&dep_atom->dep_item[0], out_list);
		}
	}
}

KBASE_EXPORT_TEST_API(jd_resolve_dep)

#if MALI_CUSTOMER_RELEASE == 0
static void jd_force_failure(struct kbase_device *kbdev, struct kbase_jd_atom *katom)
{
	kbdev->force_replay_count++;

	if (kbdev->force_replay_count >= kbdev->force_replay_limit) {
		kbdev->force_replay_count = 0;
		katom->event_code = BASE_JD_EVENT_FORCE_REPLAY;

		if (kbdev->force_replay_random)
			kbdev->force_replay_limit =
			   (prandom_u32() % KBASEP_FORCE_REPLAY_RANDOM_LIMIT) + 1;

		dev_info(kbdev->dev, "force_replay : promoting to error\n");
	}
}

/*                                               
  
                                                                             
                                                  */
static void jd_check_force_failure(struct kbase_jd_atom *katom)
{
	struct kbase_context *kctx = katom->kctx;
	struct kbase_device *kbdev = kctx->kbdev;
	int i;

	if ((kbdev->force_replay_limit == KBASEP_FORCE_REPLAY_DISABLED) ||
	    (katom->core_req & BASEP_JD_REQ_EVENT_NEVER))
		return;

	for (i = 1; i < BASE_JD_ATOM_COUNT; i++) {
		if (kbase_jd_katom_dep_atom(&kctx->jctx.atoms[i].dep[0]) == katom ||
		    kbase_jd_katom_dep_atom(&kctx->jctx.atoms[i].dep[1]) == katom) {
			struct kbase_jd_atom *dep_atom = &kctx->jctx.atoms[i];

			if ((dep_atom->core_req & BASEP_JD_REQ_ATOM_TYPE) ==
						     BASE_JD_REQ_SOFT_REPLAY &&
			    (dep_atom->core_req & kbdev->force_replay_core_req)
					     == kbdev->force_replay_core_req) {
				jd_force_failure(kbdev, katom);
				return;
			}
		}
	}
}
#endif

/*
                                                                      
              
  
                                                                                                   
                                                                                   
  
                                                  
 */
mali_bool jd_done_nolock(struct kbase_jd_atom *katom)
{
	struct kbase_context *kctx = katom->kctx;
	struct kbase_device *kbdev = kctx->kbdev;
	struct list_head completed_jobs;
	struct list_head runnable_jobs;
	mali_bool need_to_try_schedule_context = MALI_FALSE;
	int i;

	INIT_LIST_HEAD(&completed_jobs);
	INIT_LIST_HEAD(&runnable_jobs);

	KBASE_DEBUG_ASSERT(katom->status != KBASE_JD_ATOM_STATE_UNUSED);

#if MALI_CUSTOMER_RELEASE == 0
	jd_check_force_failure(katom);
#endif


	/*                                                                    
                                                                         */
	for (i = 0; i < 2; i++) {
		if (kbase_jd_katom_dep_atom(&katom->dep[i])) {
			list_del(&katom->dep_item[i]);
			kbase_jd_katom_dep_clear(&katom->dep[i]);
		}
	}

	/*                                                                                                 
                                   
   
                                                                                                              
                                    
  */

	if ((kbase_hw_has_issue(kbdev, BASE_HW_ISSUE_10817) || kbase_hw_has_issue(kbdev, BASE_HW_ISSUE_10959)) &&
		  katom->event_code == BASE_JD_EVENT_TILE_RANGE_FAULT) {
		if ((katom->core_req & BASE_JD_REQ_FS) && (katom->atom_flags & KBASE_KATOM_FLAG_BEEN_SOFT_STOPPPED)) {
			/*                                 */
			katom->event_code = BASE_JD_EVENT_DONE;
			katom->atom_flags = katom->atom_flags & (~KBASE_KATOM_FLAG_BEEN_SOFT_STOPPPED);
		}
	}

	katom->status = KBASE_JD_ATOM_STATE_COMPLETED;
	list_add_tail(&katom->dep_item[0], &completed_jobs);

	while (!list_empty(&completed_jobs)) {
		katom = list_entry(completed_jobs.prev, struct kbase_jd_atom, dep_item[0]);
		list_del(completed_jobs.prev);

		KBASE_DEBUG_ASSERT(katom->status == KBASE_JD_ATOM_STATE_COMPLETED);

		for (i = 0; i < 2; i++)
			jd_resolve_dep(&runnable_jobs, katom, i);

		if (katom->core_req & BASE_JD_REQ_EXTERNAL_RESOURCES)
			kbase_jd_post_external_resources(katom);

		while (!list_empty(&runnable_jobs)) {
			struct kbase_jd_atom *node;

			node = list_entry(runnable_jobs.prev, struct kbase_jd_atom, dep_item[0]);
			
			list_del(runnable_jobs.prev);

			KBASE_DEBUG_ASSERT(node->status != KBASE_JD_ATOM_STATE_UNUSED);

			if (katom->event_code == BASE_JD_EVENT_DONE) {
				need_to_try_schedule_context |= jd_run_atom(node);
			} else {
				node->event_code = katom->event_code;

				if ((node->core_req & BASEP_JD_REQ_ATOM_TYPE)
						  == BASE_JD_REQ_SOFT_REPLAY) {
					if (kbase_replay_process(node))
						/*                          */
						continue;
				} else if (node->core_req &
							BASE_JD_REQ_SOFT_JOB) {
					/*                                                                       */
					if (BASE_JD_REQ_SOFT_FENCE_WAIT == node->core_req)
						list_del(&node->dep_item[0]);

					kbase_finish_soft_job(node);
				}
				node->status = KBASE_JD_ATOM_STATE_COMPLETED;
			}

			if (node->status == KBASE_JD_ATOM_STATE_COMPLETED)
				list_add_tail(&node->dep_item[0], &completed_jobs);
		}

		/*                                                          
                                                                
   */
		kbase_disjoint_event_potential(kctx->kbdev);
		kbase_event_post(kctx, katom);

		/*                                                            
                                                               
                             */
		if (--kctx->jctx.job_nr == 0)
			wake_up(&kctx->jctx.zero_jobs_wait);	/*                                                               
                                                                        */
	}

	return need_to_try_schedule_context;
}

KBASE_EXPORT_TEST_API(jd_done_nolock)

#ifdef CONFIG_GPU_TRACEPOINTS
enum {
	CORE_REQ_DEP_ONLY,
	CORE_REQ_SOFT,
	CORE_REQ_COMPUTE,
	CORE_REQ_FRAGMENT,
	CORE_REQ_VERTEX,
	CORE_REQ_TILER,
	CORE_REQ_FRAGMENT_VERTEX,
	CORE_REQ_FRAGMENT_VERTEX_TILER,
	CORE_REQ_FRAGMENT_TILER,
	CORE_REQ_VERTEX_TILER,
	CORE_REQ_UNKNOWN
};
static const char * const core_req_strings[] = {
	"Dependency Only Job",
	"Soft Job",
	"Compute Shader Job",
	"Fragment Shader Job",
	"Vertex/Geometry Shader Job",
	"Tiler Job",
	"Fragment Shader + Vertex/Geometry Shader Job",
	"Fragment Shader + Vertex/Geometry Shader Job + Tiler Job",
	"Fragment Shader + Tiler Job",
	"Vertex/Geometry Shader Job + Tiler Job",
	"Unknown Job"
};
static const char *kbasep_map_core_reqs_to_string(base_jd_core_req core_req)
{
	if (core_req & BASE_JD_REQ_SOFT_JOB)
		return core_req_strings[CORE_REQ_SOFT];
	if (core_req & BASE_JD_REQ_ONLY_COMPUTE)
		return core_req_strings[CORE_REQ_COMPUTE];
	switch (core_req & (BASE_JD_REQ_FS | BASE_JD_REQ_CS | BASE_JD_REQ_T)) {
	case BASE_JD_REQ_DEP:
		return core_req_strings[CORE_REQ_DEP_ONLY];
	case BASE_JD_REQ_FS:
		return core_req_strings[CORE_REQ_FRAGMENT];
	case BASE_JD_REQ_CS:
		return core_req_strings[CORE_REQ_VERTEX];
	case BASE_JD_REQ_T:
		return core_req_strings[CORE_REQ_TILER];
	case (BASE_JD_REQ_FS | BASE_JD_REQ_CS):
		return core_req_strings[CORE_REQ_FRAGMENT_VERTEX];
	case (BASE_JD_REQ_FS | BASE_JD_REQ_T):
		return core_req_strings[CORE_REQ_FRAGMENT_TILER];
	case (BASE_JD_REQ_CS | BASE_JD_REQ_T):
		return core_req_strings[CORE_REQ_VERTEX_TILER];
	case (BASE_JD_REQ_FS | BASE_JD_REQ_CS | BASE_JD_REQ_T):
		return core_req_strings[CORE_REQ_FRAGMENT_VERTEX_TILER];
	}
	return core_req_strings[CORE_REQ_UNKNOWN];
}
#endif

mali_bool jd_submit_atom(struct kbase_context *kctx,
			 const struct base_jd_atom_v2 *user_atom,
			 struct kbase_jd_atom *katom)
{
	struct kbase_jd_context *jctx = &kctx->jctx;
	base_jd_core_req core_req;
	int queued = 0;
	int i;
	mali_bool ret;

	/*                                                                    
                                                                  */
	jctx->job_nr++;

	core_req = user_atom->core_req;

	katom->start_timestamp.tv64 = 0;
	katom->time_spent_us = 0;
	katom->udata = user_atom->udata;
	katom->kctx = kctx;
	katom->nr_extres = user_atom->nr_extres;
	katom->extres = NULL;
	katom->device_nr = user_atom->device_nr;
	katom->affinity = 0;
	katom->jc = user_atom->jc;
	katom->coreref_state = KBASE_ATOM_COREREF_STATE_NO_CORES_REQUESTED;
	katom->core_req = core_req;
	katom->nice_prio = user_atom->prio;
	katom->atom_flags = 0;
	katom->retry_count = 0;

#ifdef CONFIG_KDS
	/*                                                           
                                                                                */
	katom->kds_dep_satisfied = MALI_TRUE;
	katom->kds_rset = NULL;
#endif				/*            */

	/*                                                           
                                                                                      
                                                                                      
                                           
  */
	for (i = 0; i < 2; i++) {
		int dep_atom_number = user_atom->pre_dep[i].atom_id;
		base_jd_dep_type dep_atom_type = user_atom->pre_dep[i].dependency_type;

		if (dep_atom_number) {
			if ( dep_atom_type != BASE_JD_DEP_TYPE_ORDER &&
					dep_atom_type != BASE_JD_DEP_TYPE_DATA ) {
				katom->event_code = BASE_JD_EVENT_JOB_CONFIG_FAULT;
				katom->status = KBASE_JD_ATOM_STATE_COMPLETED;
				ret = jd_done_nolock(katom);
				goto out;
			}
		}
	}

	/*                  */
	for (i = 0; i < 2; i++) {
		int dep_atom_number = user_atom->pre_dep[i].atom_id;
		base_jd_dep_type dep_atom_type = user_atom->pre_dep[i].dependency_type;

		kbase_jd_katom_dep_clear(&katom->dep[i]);

		if (dep_atom_number) {
			struct kbase_jd_atom *dep_atom = &jctx->atoms[dep_atom_number];

			if (dep_atom->status == KBASE_JD_ATOM_STATE_UNUSED || dep_atom->status == KBASE_JD_ATOM_STATE_COMPLETED) {
				if (dep_atom->event_code != BASE_JD_EVENT_DONE) {
					/*                                                                           
                                                */
					if ( dep_atom_type == BASE_JD_DEP_TYPE_ORDER &&
							dep_atom->event_code > BASE_JD_EVENT_ACTIVE) {
						continue;
					}

					if (i == 1 && kbase_jd_katom_dep_atom(&katom->dep[0])) {
						/*                                */
						list_del(&katom->dep_item[0]);
						kbase_jd_katom_dep_clear(&katom->dep[0]);
					}

					/*                                                     */
					katom->event_code = dep_atom->event_code;
					katom->status = KBASE_JD_ATOM_STATE_QUEUED;
					if ((katom->core_req &
							BASEP_JD_REQ_ATOM_TYPE)
						  == BASE_JD_REQ_SOFT_REPLAY) {
						if (kbase_replay_process(katom)) {
							ret = MALI_FALSE;
							goto out;
						}
					}
					ret = jd_done_nolock(katom);

					goto out;
				}
			} else {
				/*                                                */
				list_add_tail(&katom->dep_item[i], &dep_atom->dep_head[i]);
				kbase_jd_katom_dep_set(&katom->dep[i], dep_atom, dep_atom_type);
				queued = 1;
			}
		}
	}

	/*                                                                  
                                                                        */
	katom->event_code = BASE_JD_EVENT_DONE;
	katom->status = KBASE_JD_ATOM_STATE_QUEUED;

	/*                                                                          */
	if (0 == katom->jc && (katom->core_req & BASEP_JD_REQ_ATOM_TYPE) != BASE_JD_REQ_DEP) {
		dev_warn(kctx->kbdev->dev, "Rejecting atom with jc = NULL");
		katom->event_code = BASE_JD_EVENT_JOB_INVALID;
		ret = jd_done_nolock(katom);
		goto out;
	}

	/*                                        */
	if ((katom->core_req & BASE_JD_REQ_SPECIFIC_COHERENT_GROUP) &&
	    (katom->device_nr >= kctx->kbdev->gpu_props.num_core_groups)) {
		dev_warn(kctx->kbdev->dev, "Rejecting atom with invalid device_nr %d", katom->device_nr);
		katom->event_code = BASE_JD_EVENT_JOB_INVALID;
		ret = jd_done_nolock(katom);
		goto out;
	}

	/*
                                                                                             
                                                                                             
                      
  */
	if (0 > katom->nice_prio) {
		mali_bool access_allowed;

		access_allowed = kbase_security_has_capability(kctx,
				KBASE_SEC_MODIFY_PRIORITY, KBASE_SEC_FLAG_NOAUDIT);

		if (!access_allowed) {
			/*                                                                         */
			katom->nice_prio = 0;
		}
	}

	/*                                        */
	if (katom->nice_prio) {
		/*                             */
		int nice_priority = katom->nice_prio + 128;

		/*                                                                             */
		katom->nice_prio = (((20 << 16) / 128) * nice_priority) >> 16;
	}

	if (katom->core_req & BASE_JD_REQ_EXTERNAL_RESOURCES) {
		/*                                                            */
		if (MALI_ERROR_NONE != kbase_jd_pre_external_resources(katom, user_atom)) {
			/*                                                                      */
			katom->event_code = BASE_JD_EVENT_JOB_INVALID;
			ret = jd_done_nolock(katom);
			goto out;
		}
	}

	/*                                                                
                                          
   
                                                                                 
   
                                                                       
  */
	if ((katom->core_req & BASE_JD_REQ_SOFT_JOB) == 0) {
		union kbasep_js_policy *js_policy = &(kctx->kbdev->js_data.policy);

		if (MALI_ERROR_NONE != kbasep_js_policy_init_job(js_policy, kctx, katom)) {
			katom->event_code = BASE_JD_EVENT_JOB_INVALID;
			ret = jd_done_nolock(katom);
			goto out;
		}
	} else {
		/*          */
		if (MALI_ERROR_NONE != kbase_prepare_soft_job(katom)) {
			katom->event_code = BASE_JD_EVENT_JOB_INVALID;
			ret = jd_done_nolock(katom);
			goto out;
		}
	}

#ifdef CONFIG_GPU_TRACEPOINTS
	katom->work_id = atomic_inc_return(&jctx->work_id);
	trace_gpu_job_enqueue((u32)kctx, katom->work_id, kbasep_map_core_reqs_to_string(katom->core_req));
#endif

	if (queued) {
		ret = MALI_FALSE;
		goto out;
	}
#ifdef CONFIG_KDS
	if (!katom->kds_dep_satisfied) {
		/*                                  */
		ret = MALI_FALSE;
		goto out;
	}
#endif				/*            */

	if ((katom->core_req & BASEP_JD_REQ_ATOM_TYPE)
						  == BASE_JD_REQ_SOFT_REPLAY) {
		if (kbase_replay_process(katom))
			ret = MALI_FALSE;
		else
			ret = jd_done_nolock(katom);

		goto out;
	} else if (katom->core_req & BASE_JD_REQ_SOFT_JOB) {
		if (kbase_process_soft_job(katom) == 0) {
			kbase_finish_soft_job(katom);
			ret = jd_done_nolock(katom);
			goto out;
		}
		/*                               */
		list_add_tail(&katom->dep_item[0], &kctx->waiting_soft_jobs);
		ret = MALI_FALSE;
	} else if ((katom->core_req & BASEP_JD_REQ_ATOM_TYPE) != BASE_JD_REQ_DEP) {
		katom->status = KBASE_JD_ATOM_STATE_IN_JS;
		ret = kbasep_js_add_job(kctx, katom);
	} else {
		/*                                                   */
		ret = jd_done_nolock(katom);
	}

 out:
	return ret;
}

#ifdef BASE_LEGACY_UK6_SUPPORT
mali_error kbase_jd_submit(struct kbase_context *kctx,
		const struct kbase_uk_job_submit *submit_data,
		int uk6_atom)
#else
mali_error kbase_jd_submit(struct kbase_context *kctx,
		const struct kbase_uk_job_submit *submit_data)
#endif /*                         */
{
	struct kbase_jd_context *jctx = &kctx->jctx;
	mali_error err = MALI_ERROR_NONE;
	int i;
	mali_bool need_to_try_schedule_context = MALI_FALSE;
	struct kbase_device *kbdev;
	void __user *user_addr;

	/*
                                                                          
                                                                   
  */
	kbdev = kctx->kbdev;

	beenthere(kctx, "%s", "Enter");

	if ((kctx->jctx.sched_info.ctx.flags & KBASE_CTX_FLAG_SUBMIT_DISABLED) != 0) {
		dev_err(kbdev->dev, "Attempt to submit to a context that has SUBMIT_DISABLED set on it");
		return MALI_ERROR_FUNCTION_FAILED;
	}

#ifdef BASE_LEGACY_UK6_SUPPORT
	if ((uk6_atom && submit_data->stride !=
			sizeof(struct base_jd_atom_v2_uk6)) ||
			submit_data->stride != sizeof(base_jd_atom_v2)) {
#else
	if (submit_data->stride != sizeof(base_jd_atom_v2)) {
#endif /*                         */
		dev_err(kbdev->dev, "Stride passed to job_submit doesn't match kernel");
		return MALI_ERROR_FUNCTION_FAILED;
	}

	user_addr = get_compat_pointer(kctx, &submit_data->addr);

	KBASE_TIMELINE_ATOMS_IN_FLIGHT(kctx, atomic_add_return(submit_data->nr_atoms, &kctx->timeline.jd_atoms_in_flight));

	for (i = 0; i < submit_data->nr_atoms; i++) {
		struct base_jd_atom_v2 user_atom;
		struct kbase_jd_atom *katom;

#ifdef BASE_LEGACY_UK6_SUPPORT
		if (uk6_atom) {
			struct base_jd_atom_v2_uk6 user_atom_v6;
			base_jd_dep_type dep_types[2] = {BASE_JD_DEP_TYPE_DATA, BASE_JD_DEP_TYPE_DATA};

			if (copy_from_user(&user_atom_v6, user_addr,
					sizeof(user_atom_v6))) {
				err = MALI_ERROR_FUNCTION_FAILED;
				KBASE_TIMELINE_ATOMS_IN_FLIGHT(kctx,
					atomic_sub_return(
					submit_data->nr_atoms - i,
					&kctx->timeline.jd_atoms_in_flight));
				break;
			}
			/*                                            */
			user_atom.jc = user_atom_v6.jc;
			user_atom.udata = user_atom_v6.udata;
			user_atom.extres_list = user_atom_v6.extres_list;
			user_atom.nr_extres = user_atom_v6.nr_extres;
			user_atom.core_req = user_atom_v6.core_req;

			/*                                               */
			if (!user_atom_v6.pre_dep[0])
				dep_types[0] = BASE_JD_DEP_TYPE_INVALID;

			base_jd_atom_dep_set(&user_atom.pre_dep[0],
					user_atom_v6.pre_dep[0],
					dep_types[0]);

			/*                                               */
			if (!user_atom_v6.pre_dep[1])
				dep_types[1] = BASE_JD_DEP_TYPE_INVALID;

			base_jd_atom_dep_set(&user_atom.pre_dep[1],
					user_atom_v6.pre_dep[1],
					dep_types[1]);

			user_atom.atom_number = user_atom_v6.atom_number;
			user_atom.prio = user_atom_v6.prio;
			user_atom.device_nr = user_atom_v6.device_nr;
		} else {
#endif /*                         */
		if (copy_from_user(&user_atom, user_addr, sizeof(user_atom)) != 0) {
			err = MALI_ERROR_FUNCTION_FAILED;
			KBASE_TIMELINE_ATOMS_IN_FLIGHT(kctx, atomic_sub_return(submit_data->nr_atoms - i, &kctx->timeline.jd_atoms_in_flight));
			break;
		}
#ifdef BASE_LEGACY_UK6_SUPPORT
		}
#endif /*                         */

		user_addr = (void __user *)((uintptr_t) user_addr + submit_data->stride);

		mutex_lock(&jctx->lock);
		katom = &jctx->atoms[user_atom.atom_number];

		while (katom->status != KBASE_JD_ATOM_STATE_UNUSED) {
			/*                                                    
              
    */
			mutex_unlock(&jctx->lock);
		
			/*                                                    
                                                         
                                                      
                                                         
                                                          
            
    */
			kbasep_js_try_schedule_head_ctx(kctx->kbdev);
			if (wait_event_killable(katom->completed,
				katom->status == KBASE_JD_ATOM_STATE_UNUSED)) {
				/*                                      
                            
     */
				return MALI_ERROR_NONE;
			}
			mutex_lock(&jctx->lock);
		}

		need_to_try_schedule_context |=
				       jd_submit_atom(kctx, &user_atom, katom);

		/*                                                                                 
                                         
   */
		kbase_disjoint_event_potential(kbdev);

		mutex_unlock(&jctx->lock);
	}

	if (need_to_try_schedule_context)
		kbasep_js_try_schedule_head_ctx(kbdev);

	return err;
}

KBASE_EXPORT_TEST_API(kbase_jd_submit)

static void kbasep_jd_cacheclean(struct kbase_device *kbdev)
{
	/*                                                                      */
	u32 max_loops = KBASE_CLEAN_CACHE_MAX_LOOPS;

	mutex_lock(&kbdev->cacheclean_lock);

	/*                                     */
	/*                               */
	KBASE_TRACE_ADD(kbdev, CORE_GPU_CLEAN_INV_CACHES, NULL, NULL, 0u, 0);
	kbase_reg_write(kbdev, GPU_CONTROL_REG(GPU_COMMAND), GPU_COMMAND_CLEAN_INV_CACHES, NULL);

	/*                                                    */
	while (--max_loops && (kbase_reg_read(kbdev, GPU_CONTROL_REG(GPU_IRQ_RAWSTAT), NULL) & CLEAN_CACHES_COMPLETED) == 0)
		;

	/*                                      */
	KBASE_TRACE_ADD(kbdev, CORE_GPU_IRQ_CLEAR, NULL, NULL, 0u, CLEAN_CACHES_COMPLETED);
	kbase_reg_write(kbdev, GPU_CONTROL_REG(GPU_IRQ_CLEAR), CLEAN_CACHES_COMPLETED, NULL);
	KBASE_DEBUG_ASSERT_MSG(kbdev->hwcnt.state != KBASE_INSTR_STATE_CLEANING,
	    "Instrumentation code was cleaning caches, but Job Management code cleared their IRQ - Instrumentation code will now hang.");

	mutex_unlock(&kbdev->cacheclean_lock);
}

/* 
                 
                                                                                           
                                                                    
                                                                                                                                                      
                                                                          
                                                                                             
 */
static void jd_done_worker(struct work_struct *data)
{
	struct kbase_jd_atom *katom = container_of(data, struct kbase_jd_atom, work);
	struct kbase_jd_context *jctx;
	struct kbase_context *kctx;
	struct kbasep_js_kctx_info *js_kctx_info;
	union kbasep_js_policy *js_policy;
	struct kbase_device *kbdev;
	struct kbasep_js_device_data *js_devdata;
	u64 cache_jc = katom->jc;
	struct kbasep_js_atom_retained_state katom_retained_state;

	/*                                            */
	KBASE_DEBUG_ASSERT((katom->core_req & BASE_JD_REQ_SOFT_JOB) == 0);

	kctx = katom->kctx;
	jctx = &kctx->jctx;
	kbdev = kctx->kbdev;
	js_kctx_info = &kctx->jctx.sched_info;

	js_devdata = &kbdev->js_data;
	js_policy = &kbdev->js_data.policy;

	KBASE_TRACE_ADD(kbdev, JD_DONE_WORKER, kctx, katom, katom->jc, 0);
	/*
                                                  
  */
	mutex_lock(&jctx->lock);
	mutex_lock(&js_kctx_info->ctx.jsctx_mutex);

	/*                                                                          
                                                                     
            
  */
	KBASE_DEBUG_ASSERT(js_kctx_info->ctx.is_scheduled != MALI_FALSE);

	if (kbase_hw_has_issue(kbdev, BASE_HW_ISSUE_6787) && katom->event_code != BASE_JD_EVENT_DONE && !(katom->event_code & BASE_JD_SW_EVENT))
		kbasep_jd_cacheclean(kbdev);  /*                                                    */
	else if (kbase_hw_has_issue(kbdev, BASE_HW_ISSUE_10676)) {
		if (kbdev->gpu_props.num_core_groups > 1 &&
		    !(katom->affinity & kbdev->gpu_props.props.coherency_info.group[0].core_mask) &&
		    (katom->affinity & kbdev->gpu_props.props.coherency_info.group[1].core_mask)) {
			dev_dbg(kbdev->dev, "JD: Flushing cache due to PRLAM-10676\n");
			kbasep_jd_cacheclean(kbdev);
		}
	}

	if (kbase_hw_has_issue(kbdev, BASE_HW_ISSUE_10969)            &&
	    (katom->core_req & BASE_JD_REQ_FS)                        &&
	    katom->event_code == BASE_JD_EVENT_TILE_RANGE_FAULT       &&
	    (katom->atom_flags & KBASE_KATOM_FLAG_BEEN_SOFT_STOPPPED) &&
	    !(katom->atom_flags & KBASE_KATOM_FLAGS_RERUN)) {
		dev_dbg(kbdev->dev, "Soft-stopped fragment shader job got a TILE_RANGE_FAULT. Possible HW issue, trying SW workaround\n");
		if (kbasep_10969_workaround_clamp_coordinates(katom)) {
			/*                                                       
                                                  
            
    */
			dev_dbg(kbdev->dev, "Clamping has been executed, try to rerun the job\n");
			katom->event_code = BASE_JD_EVENT_STOPPED;
			katom->atom_flags |= KBASE_KATOM_FLAGS_RERUN;

			/*                                                             
                                                                   
                                                           */
			kbasep_js_set_job_retry_submit_slot(katom, 0);
		}
	}

	/*                                                              
                                                                      
            */
	if (katom->event_code == BASE_JD_EVENT_PM_EVENT && !(katom->core_req & BASE_JD_REQ_SPECIFIC_COHERENT_GROUP)) {
		katom->event_code = BASE_JD_EVENT_STOPPED;
		/*                                                               
                                                                        
                                                               */
		kbasep_js_set_job_retry_submit_slot(katom, 1);
	}

	if (kbase_hw_has_issue(kbdev, BASE_HW_ISSUE_8316))
		kbase_as_poking_timer_release_atom(kbdev, kctx, katom);

	/*                                                                          
                                                                           
                           */
	kbasep_js_job_check_deref_cores(kbdev, katom);

	/*                                          */
	kbasep_js_atom_retained_state_copy(&katom_retained_state, katom);

	if (!kbasep_js_has_atom_finished(&katom_retained_state)) {
		unsigned long flags;

		/*                                                             */
		dev_dbg(kbdev->dev, "JS: Soft Stopped/Removed from next on Ctx %p; Requeuing", kctx);

		mutex_lock(&js_devdata->runpool_mutex);
		kbasep_js_clear_job_retry_submit(katom);
		/*                                                         
                                                                 
                                                            */
		katom->atom_flags &= ~(KBASE_KATOM_FLAG_BEEN_HARD_STOPPED);

		KBASE_TIMELINE_ATOM_READY(kctx, kbase_jd_atom_id(kctx, katom));
		spin_lock_irqsave(&js_devdata->runpool_irq.lock, flags);
		kbasep_js_policy_enqueue_job(js_policy, katom);
		spin_unlock_irqrestore(&js_devdata->runpool_irq.lock, flags);

		/*                                                                   
                                                                        
                                                                      
                                                                       
                                                                 
                                                                   */
		{
			int tmp;
			KBASE_DEBUG_ASSERT(kbasep_js_get_atom_retry_submit_slot(&katom_retained_state, &tmp) != MALI_FALSE);
			CSTD_UNUSED(tmp);
		}

		mutex_unlock(&js_devdata->runpool_mutex);
		mutex_unlock(&js_kctx_info->ctx.jsctx_mutex);
	} else {
		/*                                                      */
		mali_bool need_to_try_schedule_context;

		kbasep_js_remove_job(kbdev, kctx, katom);
		mutex_unlock(&js_kctx_info->ctx.jsctx_mutex);
		/*                                                              */

		need_to_try_schedule_context = jd_done_nolock(katom);

		/*                                                                    */
		KBASE_DEBUG_ASSERT(need_to_try_schedule_context == MALI_FALSE);
	}
	/*                                            */

	/*
                        
  */
	mutex_unlock(&jctx->lock);

	/*                                                                    
                                                                                         */
	kbasep_js_runpool_release_ctx_and_katom_retained_state(kbdev, kctx, &katom_retained_state);

	KBASE_TRACE_ADD(kbdev, JD_DONE_WORKER_END, kctx, NULL, cache_jc, 0);
}

/* 
                                 
                                                                           
                                                                 
  
                                                             
  
                                                                           
                                                                          
                                                                  
                                                                    
                                                                               
                                                         
 */
static void jd_cancel_worker(struct work_struct *data)
{
	struct kbase_jd_atom *katom = container_of(data, struct kbase_jd_atom, work);
	struct kbase_jd_context *jctx;
	struct kbase_context *kctx;
	struct kbasep_js_kctx_info *js_kctx_info;
	mali_bool need_to_try_schedule_context;
	struct kbase_device *kbdev;

	/*                                            */
	KBASE_DEBUG_ASSERT((katom->core_req & BASE_JD_REQ_SOFT_JOB) == 0);

	kctx = katom->kctx;
	kbdev = kctx->kbdev;
	jctx = &kctx->jctx;
	js_kctx_info = &kctx->jctx.sched_info;

	KBASE_TRACE_ADD(kbdev, JD_CANCEL_WORKER, kctx, katom, katom->jc, 0);

	/*                                                                         
                                                                      
                                                                   
                   
  */
	KBASE_DEBUG_ASSERT(js_kctx_info->ctx.is_scheduled == MALI_FALSE);

	/*                                                                       */
	kbasep_js_job_check_deref_cores(kctx->kbdev, katom);

	/*                                           */
	mutex_lock(&js_kctx_info->ctx.jsctx_mutex);
	kbasep_js_remove_cancelled_job(kbdev, kctx, katom);
	mutex_unlock(&js_kctx_info->ctx.jsctx_mutex);

	mutex_lock(&jctx->lock);

	need_to_try_schedule_context = jd_done_nolock(katom);
	/*                                                                                 
                                                                                     
                     */
	KBASE_DEBUG_ASSERT(need_to_try_schedule_context == MALI_FALSE);

	/*                                            */
	mutex_unlock(&jctx->lock);
}

/* 
                                                                
  
                                                                             
                                                                             
                                                                                 
  
                                                                                  
  
                                                 
  
                                                                
  
 */
void kbase_jd_done(struct kbase_jd_atom *katom, int slot_nr, ktime_t *end_timestamp,
		kbasep_js_atom_done_code done_code)
{
	struct kbase_context *kctx;
	struct kbase_device *kbdev;

	KBASE_DEBUG_ASSERT(katom);
	kctx = katom->kctx;
	KBASE_DEBUG_ASSERT(kctx);
	kbdev = kctx->kbdev;
	KBASE_DEBUG_ASSERT(kbdev);

	if (done_code & KBASE_JS_ATOM_DONE_EVICTED_FROM_NEXT)
		katom->event_code = BASE_JD_EVENT_REMOVED_FROM_NEXT;

	kbase_timeline_job_slot_done(kbdev, kctx, katom, slot_nr, done_code);

	KBASE_TRACE_ADD(kbdev, JD_DONE, kctx, katom, katom->jc, 0);

	kbase_job_check_leave_disjoint(kbdev, katom);
	kbasep_js_job_done_slot_irq(katom, slot_nr, end_timestamp, done_code);

	katom->slot_nr = slot_nr;

	KBASE_DEBUG_ASSERT(0 == object_is_on_stack(&katom->work));
	INIT_WORK(&katom->work, jd_done_worker);
	queue_work(kctx->jctx.job_done_wq, &katom->work);
}

KBASE_EXPORT_TEST_API(kbase_jd_done)

void kbase_jd_cancel(struct kbase_device *kbdev, struct kbase_jd_atom *katom)
{
	struct kbase_context *kctx;
	struct kbasep_js_kctx_info *js_kctx_info;

	KBASE_DEBUG_ASSERT(NULL != kbdev);
	KBASE_DEBUG_ASSERT(NULL != katom);
	kctx = katom->kctx;
	KBASE_DEBUG_ASSERT(NULL != kctx);

	js_kctx_info = &kctx->jctx.sched_info;

	KBASE_TRACE_ADD(kbdev, JD_CANCEL, kctx, katom, katom->jc, 0);

	/*                                                               */
	KBASE_DEBUG_ASSERT(js_kctx_info->ctx.is_scheduled == MALI_FALSE);

	katom->event_code = BASE_JD_EVENT_JOB_CANCELLED;

	KBASE_DEBUG_ASSERT(0 == object_is_on_stack(&katom->work));
	INIT_WORK(&katom->work, jd_cancel_worker);
	queue_work(kctx->jctx.job_done_wq, &katom->work);
}

struct zap_reset_data {
	/*                
                                      
                                                                                     
                                                                  
   
                                       
  */
	int stage;
	struct kbase_device *kbdev;
	struct hrtimer timer;
	spinlock_t lock; /*                                  */
};

static enum hrtimer_restart zap_timeout_callback(struct hrtimer *timer)
{
	struct zap_reset_data *reset_data = container_of(timer, struct zap_reset_data, timer);
	struct kbase_device *kbdev = reset_data->kbdev;
	unsigned long flags;

	spin_lock_irqsave(&reset_data->lock, flags);

	if (reset_data->stage == -1)
		goto out;

#if KBASE_GPU_RESET_EN
	if (kbase_prepare_to_reset_gpu(kbdev)) {
		dev_err(kbdev->dev, "Issueing GPU soft-reset because jobs failed to be killed (within %d ms) as part of context termination (e.g. process exit)\n", ZAP_TIMEOUT);
		kbase_reset_gpu(kbdev);
	}
#endif /*                    */
	reset_data->stage = 2;

 out:
	spin_unlock_irqrestore(&reset_data->lock, flags);

	return HRTIMER_NORESTART;
}

void kbase_jd_zap_context(struct kbase_context *kctx)
{
	struct kbase_jd_atom *katom;
	struct list_head *entry, *tmp;
	struct kbase_device *kbdev;
	struct zap_reset_data reset_data;
	unsigned long flags;

	KBASE_DEBUG_ASSERT(kctx);

	kbdev = kctx->kbdev;

	KBASE_TRACE_ADD(kbdev, JD_ZAP_CONTEXT, kctx, NULL, 0u, 0u);
	kbase_job_zap_context(kctx);

	mutex_lock(&kctx->jctx.lock);

	/*
                                                                                                 
                                     
  */

	list_for_each_safe(entry, tmp, &kctx->waiting_soft_jobs) {
		katom = list_entry(entry, struct kbase_jd_atom, dep_item[0]);
		kbase_cancel_soft_job(katom);
	}


#ifdef CONFIG_KDS

	/*                                                                             
                                                                                
                                                                                    
              
   
                                                                                              
                                                                                                
  */

	list_for_each(entry, &kctx->waiting_kds_resource) {
		katom = list_entry(entry, struct kbase_jd_atom, node);

		kbase_cancel_kds_wait_job(katom);
	}
#endif

	mutex_unlock(&kctx->jctx.lock);

	hrtimer_init_on_stack(&reset_data.timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	reset_data.timer.function = zap_timeout_callback;

	spin_lock_init(&reset_data.lock);

	reset_data.kbdev = kbdev;
	reset_data.stage = 1;

	hrtimer_start(&reset_data.timer, HR_TIMER_DELAY_MSEC(ZAP_TIMEOUT), HRTIMER_MODE_REL);

	/*                                                                     
                                                                         
                        */
	wait_event(kctx->jctx.zero_jobs_wait, kctx->jctx.job_nr == 0);
	wait_event(kctx->jctx.sched_info.ctx.is_scheduled_wait, kctx->jctx.sched_info.ctx.is_scheduled == MALI_FALSE);

	spin_lock_irqsave(&reset_data.lock, flags);
	if (reset_data.stage == 1) {
		/*                                         */
		reset_data.stage = -1;
	}
	spin_unlock_irqrestore(&reset_data.lock, flags);

	hrtimer_cancel(&reset_data.timer);

	if (reset_data.stage == 2) {
		/*                               
                                   
   */
		wait_event(kbdev->reset_wait, atomic_read(&kbdev->reset_gpu) == KBASE_RESET_GPU_NOT_PENDING);
	}
	destroy_hrtimer_on_stack(&reset_data.timer);

	dev_dbg(kbdev->dev, "Zap: Finished Context %p", kctx);

	/*                                                        */
	mutex_lock(&kctx->jctx.lock);
	mutex_lock(&kctx->jctx.sched_info.ctx.jsctx_mutex);
	mutex_unlock(&kctx->jctx.sched_info.ctx.jsctx_mutex);
	mutex_unlock(&kctx->jctx.lock);
}

KBASE_EXPORT_TEST_API(kbase_jd_zap_context)

mali_error kbase_jd_init(struct kbase_context *kctx)
{
	int i;
	mali_error mali_err = MALI_ERROR_NONE;
#ifdef CONFIG_KDS
	int err;
#endif				/*            */

	KBASE_DEBUG_ASSERT(kctx);

	kctx->jctx.job_done_wq = alloc_workqueue("mali_jd", WQ_HIGHPRI | WQ_UNBOUND, 1);
	if (NULL == kctx->jctx.job_done_wq) {
		mali_err = MALI_ERROR_OUT_OF_MEMORY;
		goto out1;
	}

	for (i = 0; i < BASE_JD_ATOM_COUNT; i++) {
		init_waitqueue_head(&kctx->jctx.atoms[i].completed);

		INIT_LIST_HEAD(&kctx->jctx.atoms[i].dep_head[0]);
		INIT_LIST_HEAD(&kctx->jctx.atoms[i].dep_head[1]);

		/*                                                                                   */
		kctx->jctx.atoms[i].event_code = BASE_JD_EVENT_JOB_INVALID;
		kctx->jctx.atoms[i].status = KBASE_JD_ATOM_STATE_UNUSED;
	}

	mutex_init(&kctx->jctx.lock);

	init_waitqueue_head(&kctx->jctx.zero_jobs_wait);

	spin_lock_init(&kctx->jctx.tb_lock);

#ifdef CONFIG_KDS
	err = kds_callback_init(&kctx->jctx.kds_cb, 0, kds_dep_clear);
	if (0 != err) {
		mali_err = MALI_ERROR_FUNCTION_FAILED;
		goto out2;
	}
#endif				/*            */

	kctx->jctx.job_nr = 0;

	return MALI_ERROR_NONE;

#ifdef CONFIG_KDS
 out2:
	destroy_workqueue(kctx->jctx.job_done_wq);
#endif				/*            */
 out1:
	return mali_err;
}

KBASE_EXPORT_TEST_API(kbase_jd_init)

void kbase_jd_exit(struct kbase_context *kctx)
{
	KBASE_DEBUG_ASSERT(kctx);

#ifdef CONFIG_KDS
	kds_callback_term(&kctx->jctx.kds_cb);
#endif				/*            */
	/*                               */
	destroy_workqueue(kctx->jctx.job_done_wq);
}

KBASE_EXPORT_TEST_API(kbase_jd_exit)
