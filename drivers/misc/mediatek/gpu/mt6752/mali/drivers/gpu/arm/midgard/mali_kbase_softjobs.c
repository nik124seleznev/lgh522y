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





#include <mali_kbase.h>

#include <linux/dma-mapping.h>
#ifdef CONFIG_SYNC
#include "sync.h"
#include <linux/syscalls.h>
#include "mali_kbase_sync.h"
#endif


/*                                                  */
#define KBASE_CACHE_ALIGNMENT_MASK		((1<<L1_CACHE_SHIFT)-1)

/* 
                              
  
                                                                    
                                                                       
 */

static int kbase_dump_cpu_gpu_time(struct kbase_jd_atom *katom)
{
	struct kbase_va_region *reg;
	phys_addr_t addr = 0;
	u64 pfn;
	u32 offset;
	char *page;
	struct timespec ts;
	struct base_dump_cpu_gpu_counters data;
	u64 system_time;
	u64 cycle_counter;
	mali_addr64 jc = katom->jc;
	struct kbase_context *kctx = katom->kctx;
	int pm_active_err;

	u32 hi1, hi2;

	memset(&data, 0, sizeof(data));

	/*                                                                       
                                                                         
                               */
	pm_active_err = kbase_pm_context_active_handle_suspend(kctx->kbdev, KBASE_PM_SUSPEND_HANDLER_DONT_REACTIVATE);
	if (pm_active_err) {
		struct kbasep_js_device_data *js_devdata = &kctx->kbdev->js_data;

		/*                                                           
                                                                            */
		mutex_lock(&js_devdata->runpool_mutex);
		list_add_tail(&katom->dep_item[1], &js_devdata->suspended_soft_jobs_list);
		mutex_unlock(&js_devdata->runpool_mutex);

		return pm_active_err;
	}

	kbase_pm_request_gpu_cycle_counter(kctx->kbdev);

	/*                                                                            */
	do {
		hi1 = kbase_reg_read(kctx->kbdev, GPU_CONTROL_REG(CYCLE_COUNT_HI), NULL);
		cycle_counter = kbase_reg_read(kctx->kbdev, GPU_CONTROL_REG(CYCLE_COUNT_LO), NULL);
		hi2 = kbase_reg_read(kctx->kbdev, GPU_CONTROL_REG(CYCLE_COUNT_HI), NULL);
		cycle_counter |= (((u64) hi1) << 32);
	} while (hi1 != hi2);

	/*                                                                            */
	do {
		hi1 = kbase_reg_read(kctx->kbdev, GPU_CONTROL_REG(TIMESTAMP_HI), NULL);
		system_time = kbase_reg_read(kctx->kbdev, GPU_CONTROL_REG(TIMESTAMP_LO), NULL);
		hi2 = kbase_reg_read(kctx->kbdev, GPU_CONTROL_REG(TIMESTAMP_HI), NULL);
		system_time |= (((u64) hi1) << 32);
	} while (hi1 != hi2);

	/*                                       */
	getrawmonotonic(&ts);

	kbase_pm_release_gpu_cycle_counter(kctx->kbdev);

	kbase_pm_context_idle(kctx->kbdev);

	data.sec = ts.tv_sec;
	data.usec = ts.tv_nsec / 1000;
	data.system_time = system_time;
	data.cycle_counter = cycle_counter;

	pfn = jc >> PAGE_SHIFT;
	offset = jc & ~PAGE_MASK;

	/*                                                            */
	katom->event_code = BASE_JD_EVENT_JOB_CANCELLED;
	if (offset > 0x1000 - sizeof(data)) {
		/*                          */
		return 0;
	}

	kbase_gpu_vm_lock(kctx);
	reg = kbase_region_tracker_find_region_enclosing_address(kctx, jc);
	if (reg &&
	    (reg->flags & KBASE_REG_GPU_WR) &&
	    reg->alloc && reg->alloc->pages)
		addr = reg->alloc->pages[pfn - reg->start_pfn];

	kbase_gpu_vm_unlock(kctx);
	if (!addr)
		return 0;

	page = kmap(pfn_to_page(PFN_DOWN(addr)));
	if (!page)
		return 0;

	dma_sync_single_for_cpu(katom->kctx->kbdev->dev,
			page_private(pfn_to_page(PFN_DOWN(addr))) +
			offset, sizeof(data),
			DMA_BIDIRECTIONAL);
	memcpy(page + offset, &data, sizeof(data));
	dma_sync_single_for_device(katom->kctx->kbdev->dev,
			page_private(pfn_to_page(PFN_DOWN(addr))) +
			offset, sizeof(data),
			DMA_BIDIRECTIONAL);
	kunmap(pfn_to_page(PFN_DOWN(addr)));

	/*                                 */
	katom->event_code = BASE_JD_EVENT_DONE;

	return 0;
}

#ifdef CONFIG_SYNC

/*                                                                                     
  
                                        
 */
static void complete_soft_job(struct kbase_jd_atom *katom)
{
	struct kbase_context *kctx = katom->kctx;

	mutex_lock(&kctx->jctx.lock);
	list_del(&katom->dep_item[0]);
	kbase_finish_soft_job(katom);
	if (jd_done_nolock(katom))
		kbasep_js_try_schedule_head_ctx(kctx->kbdev);
	mutex_unlock(&kctx->jctx.lock);
}

static enum base_jd_event_code kbase_fence_trigger(struct kbase_jd_atom *katom, int result)
{
	struct sync_pt *pt;
	struct sync_timeline *timeline;

	if (!list_is_singular(&katom->fence->pt_list_head)) {
		/*                                                                         */
		return BASE_JD_EVENT_JOB_CANCELLED;
	}

	pt = list_first_entry(&katom->fence->pt_list_head, struct sync_pt, pt_list);
	timeline = pt->parent;

	if (!kbase_sync_timeline_is_ours(timeline)) {
		/*                                       */
		return BASE_JD_EVENT_JOB_CANCELLED;
	}

	kbase_sync_signal_pt(pt, result);

	sync_timeline_signal(timeline);

	return (result < 0) ? BASE_JD_EVENT_JOB_CANCELLED : BASE_JD_EVENT_DONE;
}

static void kbase_fence_wait_worker(struct work_struct *data)
{
	struct kbase_jd_atom *katom;
	struct kbase_context *kctx;

	katom = container_of(data, struct kbase_jd_atom, work);
	kctx = katom->kctx;

	complete_soft_job(katom);
}

static void kbase_fence_wait_callback(struct sync_fence *fence, struct sync_fence_waiter *waiter)
{
	struct kbase_jd_atom *katom = container_of(waiter, struct kbase_jd_atom, sync_waiter);
	struct kbase_context *kctx;

	KBASE_DEBUG_ASSERT(NULL != katom);

	kctx = katom->kctx;

	KBASE_DEBUG_ASSERT(NULL != kctx);

	/*                                        
                                                           
  */
	if (fence->status < 0)
		katom->event_code = BASE_JD_EVENT_JOB_CANCELLED;

	/*                                                                                    
   
                                                                                  
                                                                                               
  */

	KBASE_DEBUG_ASSERT(0 == object_is_on_stack(&katom->work));
	INIT_WORK(&katom->work, kbase_fence_wait_worker);
	queue_work(kctx->jctx.job_done_wq, &katom->work);
}

static int kbase_fence_wait(struct kbase_jd_atom *katom)
{
	int ret;

	KBASE_DEBUG_ASSERT(NULL != katom);
	KBASE_DEBUG_ASSERT(NULL != katom->kctx);

	sync_fence_waiter_init(&katom->sync_waiter, kbase_fence_wait_callback);

	ret = sync_fence_wait_async(katom->fence, &katom->sync_waiter);

	if (ret == 1) {
		/*                   */
		return 0;
	} else if (ret < 0) {
		goto cancel_atom;
	}
	return 1;

 cancel_atom:
	katom->event_code = BASE_JD_EVENT_JOB_CANCELLED;
	/*                                                            
                                                               */
	KBASE_DEBUG_ASSERT(0 == object_is_on_stack(&katom->work));
	INIT_WORK(&katom->work, kbase_fence_wait_worker);
	queue_work(katom->kctx->jctx.job_done_wq, &katom->work);
	return 1;
}

static void kbase_fence_cancel_wait(struct kbase_jd_atom *katom)
{
	if (sync_fence_cancel_async(katom->fence, &katom->sync_waiter) != 0) {
		/*                                                                             */
		return;
	}

	/*                                    */
	katom->event_code = BASE_JD_EVENT_JOB_CANCELLED;

	kbase_finish_soft_job(katom);

	if (jd_done_nolock(katom))
		kbasep_js_try_schedule_head_ctx(katom->kctx->kbdev);
}
#endif /*             */

int kbase_process_soft_job(struct kbase_jd_atom *katom)
{
	switch (katom->core_req & BASEP_JD_REQ_ATOM_TYPE) {
	case BASE_JD_REQ_SOFT_DUMP_CPU_GPU_TIME:
		return kbase_dump_cpu_gpu_time(katom);
#ifdef CONFIG_SYNC
	case BASE_JD_REQ_SOFT_FENCE_TRIGGER:
		KBASE_DEBUG_ASSERT(katom->fence != NULL);
		katom->event_code = kbase_fence_trigger(katom, katom->event_code == BASE_JD_EVENT_DONE ? 0 : -EFAULT);
		/*                                                    */
		sync_fence_put(katom->fence);
		katom->fence = NULL;
		break;
	case BASE_JD_REQ_SOFT_FENCE_WAIT:
		return kbase_fence_wait(katom);
#endif				/*             */
	case BASE_JD_REQ_SOFT_REPLAY:
		return kbase_replay_process(katom);
	}

	/*                  */
	return 0;
}

void kbase_cancel_soft_job(struct kbase_jd_atom *katom)
{
	switch (katom->core_req & BASEP_JD_REQ_ATOM_TYPE) {
#ifdef CONFIG_SYNC
	case BASE_JD_REQ_SOFT_FENCE_WAIT:
		kbase_fence_cancel_wait(katom);
		break;
#endif
	default:
		/*                                             */
		KBASE_DEBUG_ASSERT(0);
	}
}

mali_error kbase_prepare_soft_job(struct kbase_jd_atom *katom)
{
	switch (katom->core_req & BASEP_JD_REQ_ATOM_TYPE) {
	case BASE_JD_REQ_SOFT_DUMP_CPU_GPU_TIME:
		{
			if (0 != (katom->jc & KBASE_CACHE_ALIGNMENT_MASK))
				return MALI_ERROR_FUNCTION_FAILED;
		}
		break;
#ifdef CONFIG_SYNC
	case BASE_JD_REQ_SOFT_FENCE_TRIGGER:
		{
			struct base_fence fence;
			int fd;

			if (0 != copy_from_user(&fence, (__user void *)(uintptr_t) katom->jc, sizeof(fence)))
				return MALI_ERROR_FUNCTION_FAILED;

			fd = kbase_stream_create_fence(fence.basep.stream_fd);
			if (fd < 0)
				return MALI_ERROR_FUNCTION_FAILED;

			katom->fence = sync_fence_fdget(fd);

			if (katom->fence == NULL) {
				/*                                                                     
                                       */
				return MALI_ERROR_FUNCTION_FAILED;
			}
			fence.basep.fd = fd;
			if (0 != copy_to_user((__user void *)(uintptr_t) katom->jc, &fence, sizeof(fence))) {
				katom->fence = NULL;
				sys_close(fd);
				return MALI_ERROR_FUNCTION_FAILED;
			}
		}
		break;
	case BASE_JD_REQ_SOFT_FENCE_WAIT:
		{
			struct base_fence fence;

			if (0 != copy_from_user(&fence, (__user void *)(uintptr_t) katom->jc, sizeof(fence)))
				return MALI_ERROR_FUNCTION_FAILED;

			/*                                     */
			katom->fence = sync_fence_fdget(fence.basep.fd);
			if (katom->fence == NULL)
				return MALI_ERROR_FUNCTION_FAILED;
		}
		break;
#endif				/*             */
	case BASE_JD_REQ_SOFT_REPLAY:
		break;
	default:
		/*                      */
		return MALI_ERROR_FUNCTION_FAILED;
	}
	return MALI_ERROR_NONE;
}

void kbase_finish_soft_job(struct kbase_jd_atom *katom)
{
	switch (katom->core_req & BASEP_JD_REQ_ATOM_TYPE) {
	case BASE_JD_REQ_SOFT_DUMP_CPU_GPU_TIME:
		/*               */
		break;
#ifdef CONFIG_SYNC
	case BASE_JD_REQ_SOFT_FENCE_TRIGGER:
		if (katom->fence) {
			/*                                                       */
			kbase_fence_trigger(katom, katom->event_code == BASE_JD_EVENT_DONE ? 0 : -EFAULT);
			sync_fence_put(katom->fence);
			katom->fence = NULL;
		}
		break;
	case BASE_JD_REQ_SOFT_FENCE_WAIT:
		/*                                           */
		sync_fence_put(katom->fence);
		katom->fence = NULL;
		break;
#endif				/*             */
	}
}

void kbase_resume_suspended_soft_jobs(struct kbase_device *kbdev)
{
	LIST_HEAD(local_suspended_soft_jobs);
	struct kbase_jd_atom *tmp_iter;
	struct kbase_jd_atom *katom_iter;
	struct kbasep_js_device_data *js_devdata;
	mali_bool resched = MALI_FALSE;

	KBASE_DEBUG_ASSERT(kbdev);

	js_devdata = &kbdev->js_data;

	/*                          */
	mutex_lock(&js_devdata->runpool_mutex);
	list_splice_init(&js_devdata->suspended_soft_jobs_list, &local_suspended_soft_jobs);
	mutex_unlock(&js_devdata->runpool_mutex);

	/*                                                                       
                                                      */
	list_for_each_entry_safe(katom_iter, tmp_iter, &local_suspended_soft_jobs, dep_item[1])
	{
		struct kbase_context *kctx = katom_iter->kctx;

		mutex_lock(&kctx->jctx.lock);

		/*                             */
		list_del(&katom_iter->dep_item[1]);
		/*                                                     */
		list_del(&katom_iter->dep_item[0]);

		if (kbase_process_soft_job(katom_iter) == 0) {
			kbase_finish_soft_job(katom_iter);
			resched |= jd_done_nolock(katom_iter);
		} else {
			/*                           */
			KBASE_DEBUG_ASSERT((katom_iter->core_req & BASEP_JD_REQ_ATOM_TYPE)
					!= BASE_JD_REQ_SOFT_REPLAY);
			list_add_tail(&katom_iter->dep_item[0], &kctx->waiting_soft_jobs);
		}

		mutex_unlock(&kctx->jctx.lock);
	}

	if (resched)
		kbasep_js_try_schedule_head_ctx(kbdev);
}
