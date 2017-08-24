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

#include <mali_kbase.h>
#include <mali_kbase_jm.h>
#include <mali_kbase_js.h>
#include <mali_kbase_js_policy_cfs.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 9, 0)
#include <linux/sched/rt.h>
#endif

/* 
                                      
                                                                                                                                        
                                                                                      
 */
#define CINSTR_DUMPING_ENABLED (2 == MALI_INSTRUMENTATION_LEVEL)

/*                                                             */
#define WEIGHT_FIXEDPOINT_SHIFT 10
#define WEIGHT_TABLE_SIZE       40
#define WEIGHT_0_NICE           (WEIGHT_TABLE_SIZE/2)
#define WEIGHT_0_VAL            (1 << WEIGHT_FIXEDPOINT_SHIFT)

#define LOOKUP_VARIANT_MASK ((1u<<KBASEP_JS_MAX_NR_CORE_REQ_VARIANTS) - 1u)

#define PROCESS_PRIORITY_MIN (-20)
#define PROCESS_PRIORITY_MAX  (19)

/*                                                  */
#define JS_CORE_REQ_ALL_OTHERS \
	(BASE_JD_REQ_CF | BASE_JD_REQ_V | BASE_JD_REQ_PERMON | BASE_JD_REQ_EXTERNAL_RESOURCES | BASEP_JD_REQ_EVENT_NEVER)

/*                                                    */

/*                                                                                               
                                                                 */
#define JS_CTX_REQ_ALL_OTHERS_8987 \
	(KBASE_CTX_FLAG_PRIVILEGED)
#define JS_CORE_REQ_COMPUTE_SLOT_8987 \
	(BASE_JD_REQ_CS)
#define JS_CORE_REQ_ONLY_COMPUTE_SLOT_8987 \
	(BASE_JD_REQ_ONLY_COMPUTE)

/*                                                                    */
#define JS_CTX_REQ_ALL_OTHERS \
	(KBASE_CTX_FLAG_PRIVILEGED | KBASE_CTX_FLAG_HINT_ONLY_COMPUTE)
#define JS_CORE_REQ_COMPUTE_SLOT \
	(BASE_JD_REQ_CS | BASE_JD_REQ_ONLY_COMPUTE)

/*                                                                      
                                                                               
                                                                                     
                                                                       */
static const struct kbasep_atom_req core_req_variants[] = {
	{
	 /*                     */
	 (JS_CORE_REQ_ALL_OTHERS | BASE_JD_REQ_FS | BASE_JD_REQ_FS_AFBC |
						BASE_JD_REQ_COHERENT_GROUP),
	 (JS_CTX_REQ_ALL_OTHERS),
	 0},
	{
	 /*                                            */
	 (JS_CORE_REQ_ALL_OTHERS | JS_CORE_REQ_COMPUTE_SLOT),
	 (JS_CTX_REQ_ALL_OTHERS),
	 0},
	{
	 /*                                                   */
	 (JS_CORE_REQ_ALL_OTHERS | JS_CORE_REQ_COMPUTE_SLOT | BASE_JD_REQ_COHERENT_GROUP),
	 (JS_CTX_REQ_ALL_OTHERS),
	 0},
	{
	 /*                                                                            */
	 (JS_CORE_REQ_ALL_OTHERS | JS_CORE_REQ_COMPUTE_SLOT | BASE_JD_REQ_COHERENT_GROUP | BASE_JD_REQ_T),
	 (JS_CTX_REQ_ALL_OTHERS),
	 0},

	{
	 /*           */
	 0,
	 0,
	 0},

	{
	 /*                                                                       */
	 (JS_CORE_REQ_ALL_OTHERS | JS_CORE_REQ_COMPUTE_SLOT | BASE_JD_REQ_COHERENT_GROUP | BASE_JD_REQ_SPECIFIC_COHERENT_GROUP),
	 (JS_CTX_REQ_ALL_OTHERS),
	 0			/*           */
	 },
	{
	 /*                                                                       */
	 (JS_CORE_REQ_ALL_OTHERS | JS_CORE_REQ_COMPUTE_SLOT | BASE_JD_REQ_COHERENT_GROUP | BASE_JD_REQ_SPECIFIC_COHERENT_GROUP),
	 (JS_CTX_REQ_ALL_OTHERS),
	 1			/*           */
	 },

	/*                                                                 */
	{
	 /*   */
	 0,
	 0,
	 0},
};

static const struct kbasep_atom_req core_req_variants_8987[] = {
	{
	 /*                     */
	 (JS_CORE_REQ_ALL_OTHERS | BASE_JD_REQ_FS | BASE_JD_REQ_COHERENT_GROUP),
	 (JS_CTX_REQ_ALL_OTHERS_8987),
	 0},
	{
	 /*                                            */
	 (JS_CORE_REQ_ALL_OTHERS | JS_CORE_REQ_COMPUTE_SLOT_8987),
	 (JS_CTX_REQ_ALL_OTHERS_8987),
	 0},
	{
	 /*                                                   */
	 (JS_CORE_REQ_ALL_OTHERS | JS_CORE_REQ_COMPUTE_SLOT_8987 | BASE_JD_REQ_COHERENT_GROUP),
	 (JS_CTX_REQ_ALL_OTHERS_8987),
	 0},
	{
	 /*                                                                            */
	 (JS_CORE_REQ_ALL_OTHERS | JS_CORE_REQ_COMPUTE_SLOT_8987 | BASE_JD_REQ_COHERENT_GROUP | BASE_JD_REQ_T),
	 (JS_CTX_REQ_ALL_OTHERS_8987),
	 0},

	{
	 /*                                                        
    
                                                                       
                                                                  */
	 (JS_CORE_REQ_ALL_OTHERS | JS_CORE_REQ_ONLY_COMPUTE_SLOT_8987 | BASE_JD_REQ_COHERENT_GROUP),
	 (JS_CTX_REQ_ALL_OTHERS_8987 | KBASE_CTX_FLAG_HINT_ONLY_COMPUTE),
	 0},

	{
	 /*                                                                      
                                                                  */
	 (JS_CORE_REQ_ALL_OTHERS | JS_CORE_REQ_ONLY_COMPUTE_SLOT_8987 | BASE_JD_REQ_COHERENT_GROUP | BASE_JD_REQ_SPECIFIC_COHERENT_GROUP),
	 (JS_CTX_REQ_ALL_OTHERS_8987 | KBASE_CTX_FLAG_HINT_ONLY_COMPUTE),
	 0			/*           */
	 },
	{
	 /*                                                                      
                                                                  */
	 (JS_CORE_REQ_ALL_OTHERS | JS_CORE_REQ_ONLY_COMPUTE_SLOT_8987 | BASE_JD_REQ_COHERENT_GROUP | BASE_JD_REQ_SPECIFIC_COHERENT_GROUP),
	 (JS_CTX_REQ_ALL_OTHERS_8987 | KBASE_CTX_FLAG_HINT_ONLY_COMPUTE),
	 1			/*           */
	 },
	/*                                                                 */
	{
	 /*   */
	 0,
	 0,
	 0},
};

#define CORE_REQ_VARIANT_FRAGMENT                    0
#define CORE_REQ_VARIANT_COMPUTE_ALL_CORES           1
#define CORE_REQ_VARIANT_COMPUTE_ONLY_COHERENT_GROUP 2
#define CORE_REQ_VARIANT_COMPUTE_OR_TILING           3
#define CORE_REQ_VARIANT_COMPUTE_SPECIFIC_COHERENT_0 5
#define CORE_REQ_VARIANT_COMPUTE_SPECIFIC_COHERENT_1 6

#define CORE_REQ_VARIANT_ONLY_COMPUTE_8987                     4
#define CORE_REQ_VARIANT_ONLY_COMPUTE_8987_SPECIFIC_COHERENT_0 5
#define CORE_REQ_VARIANT_ONLY_COMPUTE_8987_SPECIFIC_COHERENT_1 6

#define NUM_CORE_REQ_VARIANTS NELEMS(core_req_variants)
#define NUM_CORE_REQ_VARIANTS_8987 NELEMS(core_req_variants_8987)

/*                                                                       */
static const u32 variants_supported_ss_state[] = {
	/*                          */
	(1u << CORE_REQ_VARIANT_FRAGMENT),

	/*                                                                         
                                 */
	(1u << CORE_REQ_VARIANT_COMPUTE_ALL_CORES)
	    | (1u << CORE_REQ_VARIANT_COMPUTE_ONLY_COHERENT_GROUP)
	    | (1u << CORE_REQ_VARIANT_COMPUTE_OR_TILING)
	    | (1u << CORE_REQ_VARIANT_COMPUTE_SPECIFIC_COHERENT_0),

	/*                                                                 */
	(1u << CORE_REQ_VARIANT_COMPUTE_ONLY_COHERENT_GROUP)
	    | (1u << CORE_REQ_VARIANT_COMPUTE_SPECIFIC_COHERENT_1)
};

/*                                                                            
                                                                                  
                                       */
static const u32 variants_supported_ss_allcore_state[] = {
	/*                          */
	(1u << CORE_REQ_VARIANT_FRAGMENT),

	/*                                                                         
                                                               */
	(1u << CORE_REQ_VARIANT_COMPUTE_ALL_CORES)
	    | (1u << CORE_REQ_VARIANT_COMPUTE_ONLY_COHERENT_GROUP)
	    | (1u << CORE_REQ_VARIANT_COMPUTE_OR_TILING)
	    | (1u << CORE_REQ_VARIANT_COMPUTE_SPECIFIC_COHERENT_0)
	    | (1u << CORE_REQ_VARIANT_COMPUTE_SPECIFIC_COHERENT_1),

	/*                */
	0
};

/*                                                                          
                     
  
                                                                                 
                                                                             
                                                                             
                        */
static const u32 variants_supported_ss_state_8987[] = {
	/*                          */
	(1u << CORE_REQ_VARIANT_FRAGMENT),

	/*                                                                         */
	(1u << CORE_REQ_VARIANT_COMPUTE_ALL_CORES)
	    | (1u << CORE_REQ_VARIANT_COMPUTE_ONLY_COHERENT_GROUP)
	    | (1u << CORE_REQ_VARIANT_COMPUTE_OR_TILING),

	/*                                                                 
                                                                         
                                                   
   
                                                                        
                                     */
	(1u << CORE_REQ_VARIANT_ONLY_COMPUTE_8987)
	    | (1u << CORE_REQ_VARIANT_ONLY_COMPUTE_8987_SPECIFIC_COHERENT_0)
	    | (1u << CORE_REQ_VARIANT_ONLY_COMPUTE_8987_SPECIFIC_COHERENT_1)
};

/*                                                                                      */
#define KBASEP_JS_CHECKFLAG_QUEUED       (1u << 0) /*                          */
#define KBASEP_JS_CHECKFLAG_SCHEDULED    (1u << 1) /*                             */
#define KBASEP_JS_CHECKFLAG_IS_QUEUED    (1u << 2) /*                                 */
#define KBASEP_JS_CHECKFLAG_IS_SCHEDULED (1u << 3) /*                                    */

enum {
	KBASEP_JS_CHECK_NOTQUEUED = KBASEP_JS_CHECKFLAG_QUEUED,
	KBASEP_JS_CHECK_NOTSCHEDULED = KBASEP_JS_CHECKFLAG_SCHEDULED,
	KBASEP_JS_CHECK_QUEUED = KBASEP_JS_CHECKFLAG_QUEUED | KBASEP_JS_CHECKFLAG_IS_QUEUED,
	KBASEP_JS_CHECK_SCHEDULED = KBASEP_JS_CHECKFLAG_SCHEDULED | KBASEP_JS_CHECKFLAG_IS_SCHEDULED
};

typedef u32 kbasep_js_check;

/*
                    
 */

/*                                                                                     */

/*               */
static const int weight_of_priority[] = {
	/*      */ 11, 14, 18, 23,
	/*      */ 29, 36, 45, 56,
	/*      */ 70, 88, 110, 137,
	/*      */ 171, 214, 268, 335,
	/*      */ 419, 524, 655, 819,
	/*      */ 1024, 1280, 1600, 2000,
	/*      */ 2500, 3125, 3906, 4883,
	/*      */ 6104, 7630, 9538, 11923,
	/*      */ 14904, 18630, 23288, 29110,
	/*      */ 36388, 45485, 56856, 71070
};

/* 
                                                                       
                                                                        
                                                                       
                                                                              
                  
 */
STATIC u64 priority_weight(struct kbasep_js_policy_cfs_ctx *ctx_info, u64 time_us)
{
	u64 time_delta_us;
	int priority;

	priority = ctx_info->process_priority + ctx_info->bag_priority;

	/*                                                     */
	if (priority != 0 && time_us != 0) {
		int clamped_priority;

		/*                                  */
		if (priority > PROCESS_PRIORITY_MAX)
			clamped_priority = PROCESS_PRIORITY_MAX;
		else if (priority < PROCESS_PRIORITY_MIN)
			clamped_priority = PROCESS_PRIORITY_MIN;
		else
			clamped_priority = priority;

		/*                            */
		time_delta_us = (time_us * weight_of_priority[WEIGHT_0_NICE + clamped_priority]);
		/*                 */
		time_delta_us = time_delta_us >> WEIGHT_FIXEDPOINT_SHIFT;
		/*                                     */
		if (0 == time_delta_us)
			time_delta_us++;
	} else {
		time_delta_us = time_us;
	}

	return time_delta_us;
}

#if KBASE_TRACE_ENABLE
STATIC int kbasep_js_policy_trace_get_refcnt_nolock(struct kbase_device *kbdev, struct kbase_context *kctx)
{
	struct kbasep_js_device_data *js_devdata;
	int as_nr;
	int refcnt = 0;

	js_devdata = &kbdev->js_data;

	as_nr = kctx->as_nr;
	if (as_nr != KBASEP_AS_NR_INVALID) {
		struct kbasep_js_per_as_data *js_per_as_data;
		js_per_as_data = &js_devdata->runpool_irq.per_as_data[as_nr];

		refcnt = js_per_as_data->as_busy_refcount;
	}

	return refcnt;
}

STATIC INLINE int kbasep_js_policy_trace_get_refcnt(struct kbase_device *kbdev, struct kbase_context *kctx)
{
	unsigned long flags;
	struct kbasep_js_device_data *js_devdata;
	int refcnt = 0;

	js_devdata = &kbdev->js_data;

	spin_lock_irqsave(&js_devdata->runpool_irq.lock, flags);
	refcnt = kbasep_js_policy_trace_get_refcnt_nolock(kbdev, kctx);
	spin_unlock_irqrestore(&js_devdata->runpool_irq.lock, flags);

	return refcnt;
}
#else				/*                     */
STATIC int kbasep_js_policy_trace_get_refcnt_nolock(struct kbase_device *kbdev, struct kbase_context *kctx)
{
	CSTD_UNUSED(kbdev);
	CSTD_UNUSED(kctx);
	return 0;
}

STATIC INLINE int kbasep_js_policy_trace_get_refcnt(struct kbase_device *kbdev, struct kbase_context *kctx)
{
	CSTD_UNUSED(kbdev);
	CSTD_UNUSED(kctx);
	return 0;
}
#endif				/*                     */

#ifdef CONFIG_MALI_DEBUG
STATIC void kbasep_js_debug_check(struct kbasep_js_policy_cfs *policy_info, struct kbase_context *kctx, kbasep_js_check check_flag)
{
	/*                                                                      
                                                           */

	if (check_flag & KBASEP_JS_CHECKFLAG_QUEUED) {
		mali_bool is_queued;
		mali_bool expect_queued;

		is_queued = (kbasep_list_member_of(
				&policy_info->ctx_queue_head,
				&kctx->jctx.sched_info.runpool.policy_ctx.cfs.list)) ?
				MALI_TRUE : MALI_FALSE;

		if (!is_queued)
			is_queued = (kbasep_list_member_of(&policy_info->ctx_rt_queue_head, &kctx->jctx.sched_info.runpool.policy_ctx.cfs.list)) ? MALI_TRUE : MALI_FALSE;

		expect_queued = (check_flag & KBASEP_JS_CHECKFLAG_IS_QUEUED) ? MALI_TRUE : MALI_FALSE;

		KBASE_DEBUG_ASSERT_MSG(expect_queued == is_queued, "Expected context %p to be %s but it was %s\n", kctx, (expect_queued) ? "queued" : "not queued", (is_queued) ? "queued" : "not queued");

	}

	if (check_flag & KBASEP_JS_CHECKFLAG_SCHEDULED) {
		mali_bool is_scheduled;
		mali_bool expect_scheduled;

		is_scheduled = (kbasep_list_member_of(
			&policy_info->scheduled_ctxs_head,
			&kctx->jctx.sched_info.runpool.policy_ctx.cfs.list)) ?
			MALI_TRUE : MALI_FALSE;

		expect_scheduled = (check_flag & KBASEP_JS_CHECKFLAG_IS_SCHEDULED) ? MALI_TRUE : MALI_FALSE;
		KBASE_DEBUG_ASSERT_MSG(expect_scheduled == is_scheduled, "Expected context %p to be %s but it was %s\n", kctx, (expect_scheduled) ? "scheduled" : "not scheduled", (is_scheduled) ? "scheduled" : "not scheduled");
	}

}
#else				/*                   */
STATIC void kbasep_js_debug_check(struct kbasep_js_policy_cfs *policy_info, struct kbase_context *kctx, kbasep_js_check check_flag)
{
	CSTD_UNUSED(policy_info);
	CSTD_UNUSED(kctx);
	CSTD_UNUSED(check_flag);
}
#endif				/*                   */

STATIC INLINE void set_slot_to_variant_lookup(u32 *bit_array, u32 slot_idx, u32 variants_supported)
{
	u32 overall_bit_idx = slot_idx * KBASEP_JS_MAX_NR_CORE_REQ_VARIANTS;
	u32 word_idx = overall_bit_idx / 32;
	u32 bit_idx = overall_bit_idx % 32;

	KBASE_DEBUG_ASSERT(slot_idx < BASE_JM_MAX_NR_SLOTS);
	KBASE_DEBUG_ASSERT((variants_supported & ~LOOKUP_VARIANT_MASK) == 0);

	bit_array[word_idx] |= variants_supported << bit_idx;
}

STATIC INLINE u32 get_slot_to_variant_lookup(u32 *bit_array, u32 slot_idx)
{
	u32 overall_bit_idx = slot_idx * KBASEP_JS_MAX_NR_CORE_REQ_VARIANTS;
	u32 word_idx = overall_bit_idx / 32;
	u32 bit_idx = overall_bit_idx % 32;

	u32 res;

	KBASE_DEBUG_ASSERT(slot_idx < BASE_JM_MAX_NR_SLOTS);

	res = bit_array[word_idx] >> bit_idx;
	res &= LOOKUP_VARIANT_MASK;

	return res;
}

/*                                                                           
                                                                               
                                                                
  
                                                                           
                                                                           
                                                      */
#ifdef CONFIG_MALI_DEBUG
STATIC void debug_check_core_req_variants(struct kbase_device *kbdev, struct kbasep_js_policy_cfs *policy_info)
{
	struct kbasep_js_device_data *js_devdata;
	u32 i;
	int j;

	js_devdata = &kbdev->js_data;

	for (j = 0; j < kbdev->gpu_props.num_job_slots; ++j) {
		base_jd_core_req job_core_req;
		mali_bool found = MALI_FALSE;

		job_core_req = js_devdata->js_reqs[j];
		for (i = 0; i < policy_info->num_core_req_variants; ++i) {
			base_jd_core_req var_core_req;

			var_core_req = policy_info->core_req_variants[i].core_req;

			if ((var_core_req & job_core_req) == job_core_req) {
				found = MALI_TRUE;
				break;
			}
		}

		/*                          */
		KBASE_DEBUG_ASSERT_MSG(found != MALI_FALSE, "Job slot %d features 0x%x not matched by core_req_variants. " "Rework core_req_variants and vairants_supported_<...>_state[] to match\n", j, job_core_req);
	}
}
#endif

STATIC void build_core_req_variants(struct kbase_device *kbdev, struct kbasep_js_policy_cfs *policy_info)
{
	KBASE_DEBUG_ASSERT(kbdev != NULL);
	KBASE_DEBUG_ASSERT(policy_info != NULL);
	CSTD_UNUSED(kbdev);

	if (kbase_hw_has_issue(kbdev, BASE_HW_ISSUE_8987)) {
		KBASE_DEBUG_ASSERT(NUM_CORE_REQ_VARIANTS_8987 <= KBASEP_JS_MAX_NR_CORE_REQ_VARIANTS);

		/*                                 */
		memcpy(policy_info->core_req_variants, core_req_variants_8987, sizeof(core_req_variants_8987));

		policy_info->num_core_req_variants = NUM_CORE_REQ_VARIANTS_8987;
	} else {
		KBASE_DEBUG_ASSERT(NUM_CORE_REQ_VARIANTS <= KBASEP_JS_MAX_NR_CORE_REQ_VARIANTS);

		/*                                 */
		memcpy(policy_info->core_req_variants, core_req_variants, sizeof(core_req_variants));

		policy_info->num_core_req_variants = NUM_CORE_REQ_VARIANTS;
	}

	KBASE_DEBUG_CODE(debug_check_core_req_variants(kbdev, policy_info));
}

STATIC void build_slot_lookups(struct kbase_device *kbdev, struct kbasep_js_policy_cfs *policy_info)
{
	u8 i;
	const u32 *variants_supported_ss_for_this_hw = variants_supported_ss_state;

	KBASE_DEBUG_ASSERT(kbdev != NULL);
	KBASE_DEBUG_ASSERT(policy_info != NULL);

	KBASE_DEBUG_ASSERT(kbdev->gpu_props.num_job_slots <= NELEMS(variants_supported_ss_state));
	KBASE_DEBUG_ASSERT(kbdev->gpu_props.num_job_slots <= NELEMS(variants_supported_ss_allcore_state));
	KBASE_DEBUG_ASSERT(kbdev->gpu_props.num_job_slots <= NELEMS(variants_supported_ss_state_8987));

	if (kbase_hw_has_issue(kbdev, BASE_HW_ISSUE_8987))
		variants_supported_ss_for_this_hw = variants_supported_ss_state_8987;

	/*                                                                   */
	for (i = 0; i < kbdev->gpu_props.num_job_slots; ++i) {
		set_slot_to_variant_lookup(policy_info->slot_to_variant_lookup_ss_state, i, variants_supported_ss_for_this_hw[i]);

		set_slot_to_variant_lookup(policy_info->slot_to_variant_lookup_ss_allcore_state, i, variants_supported_ss_allcore_state[i]);
	}

}

STATIC mali_error cached_variant_idx_init(const struct kbasep_js_policy_cfs *policy_info, const struct kbase_context *kctx, struct kbase_jd_atom *atom)
{
	struct kbasep_js_policy_cfs_job *job_info;
	u32 i;
	base_jd_core_req job_core_req;
	u32 job_device_nr;
	kbase_context_flags ctx_flags;
	const struct kbasep_js_kctx_info *js_kctx_info;
	const struct kbase_device *kbdev;

	KBASE_DEBUG_ASSERT(policy_info != NULL);
	KBASE_DEBUG_ASSERT(kctx != NULL);
	KBASE_DEBUG_ASSERT(atom != NULL);

	kbdev = container_of(policy_info, const struct kbase_device, js_data.policy.cfs);
	job_info = &atom->sched_info.cfs;
	job_core_req = atom->core_req;
	job_device_nr = atom->device_nr;
	js_kctx_info = &kctx->jctx.sched_info;
	ctx_flags = js_kctx_info->ctx.flags;

	/*                                                         */
	if ((job_core_req & BASE_JD_REQ_SPECIFIC_COHERENT_GROUP) != MALI_FALSE && job_device_nr >= kbdev->gpu_props.num_core_groups) {
		/*                                                            
                                              */
		return MALI_ERROR_FUNCTION_FAILED;
	}

	/*                                                                        
                                                              */
	for (i = 0; i < policy_info->num_core_req_variants; ++i) {
		base_jd_core_req var_core_req;
		kbase_context_flags var_ctx_req;
		u32 var_device_nr;
		var_core_req = policy_info->core_req_variants[i].core_req;
		var_ctx_req = policy_info->core_req_variants[i].ctx_req;
		var_device_nr = policy_info->core_req_variants[i].device_nr;

		if ((var_core_req & job_core_req) == job_core_req && (var_ctx_req & ctx_flags) == ctx_flags && ((var_core_req & BASE_JD_REQ_SPECIFIC_COHERENT_GROUP) == MALI_FALSE || var_device_nr == job_device_nr)) {
			job_info->cached_variant_idx = i;
			return MALI_ERROR_NONE;
		}
	}

	/*                                                                        
                                  */
	return MALI_ERROR_FUNCTION_FAILED;
}

STATIC mali_bool dequeue_job(struct kbase_device *kbdev,
			     struct kbase_context *kctx,
			     u32 variants_supported,
			     struct kbase_jd_atom ** const katom_ptr,
			     int job_slot_idx)
{
	struct kbasep_js_device_data *js_devdata;
	struct kbasep_js_policy_cfs *policy_info;
	struct kbasep_js_policy_cfs_ctx *ctx_info;

	KBASE_DEBUG_ASSERT(kbdev != NULL);
	KBASE_DEBUG_ASSERT(katom_ptr != NULL);
	KBASE_DEBUG_ASSERT(kctx != NULL);

	js_devdata = &kbdev->js_data;
	policy_info = &js_devdata->policy.cfs;
	ctx_info = &kctx->jctx.sched_info.runpool.policy_ctx.cfs;

	/*                                                 */
	if (kbasep_js_is_submit_allowed(js_devdata, kctx) != MALI_FALSE) {
		/*                            */
		while (variants_supported != 0) {
			long variant_idx;
			struct list_head *job_list;

			variant_idx = ffs(variants_supported) - 1;
			job_list = &ctx_info->job_list_head[variant_idx];

			if (!list_empty(job_list)) {
				/*                                     */
				{
					struct kbase_jd_atom *front_atom = 
							list_entry(job_list->next, struct kbase_jd_atom, sched_info.cfs.list);

					KBASE_TRACE_ADD_SLOT(kbdev, JS_POLICY_DEQUEUE_JOB, front_atom->kctx, front_atom, front_atom->jc, job_slot_idx);
				}
				*katom_ptr = list_entry(job_list->next, struct kbase_jd_atom, sched_info.cfs.list);
				list_del(job_list->next);

				(*katom_ptr)->sched_info.cfs.ticks = 0;

				/*                                              */
				list_del(&kctx->jctx.sched_info.runpool.policy_ctx.cfs.list);
				list_add_tail(&kctx->jctx.sched_info.runpool.policy_ctx.cfs.list, &policy_info->scheduled_ctxs_head);

				return MALI_TRUE;
			}

			variants_supported &= ~(1u << variant_idx);
		}
		/*                              */
	}

	/*                                           */

	return MALI_FALSE;
}

/* 
                                         
 */
STATIC INLINE mali_bool timer_callback_should_run(struct kbase_device *kbdev)
{
	struct kbasep_js_device_data *js_devdata;
	s8 nr_running_ctxs;

	KBASE_DEBUG_ASSERT(kbdev != NULL);
	js_devdata = &kbdev->js_data;

	/*                                                                         
                                                                             
                           */
	nr_running_ctxs = js_devdata->nr_user_contexts_running;

#ifdef CONFIG_MALI_DEBUG
	if (js_devdata->softstop_always && nr_running_ctxs > 0) {
		/*                                                          */
		return MALI_TRUE;
	}
#endif				/*                   */

	if (kbase_hw_has_issue(kbdev, BASE_HW_ISSUE_9435)) {
		/*                                                                        
                                                                             
                            
                              
    
                                                                             
                                                                
                                     
   */
		{
			s8 nr_compute_ctxs = kbasep_js_ctx_attr_count_on_runpool(kbdev, KBASEP_JS_CTX_ATTR_COMPUTE);
			s8 nr_noncompute_ctxs = nr_running_ctxs - nr_compute_ctxs;

			return (mali_bool) (nr_compute_ctxs >= 2 || nr_noncompute_ctxs > 0);
		}
	} else {
		/*                                                             */
		return (mali_bool) (nr_running_ctxs > 0);
	}
}

static enum hrtimer_restart timer_callback(struct hrtimer *timer)
{
	unsigned long flags;
	struct kbase_device *kbdev;
	struct kbasep_js_device_data *js_devdata;
	struct kbasep_js_policy_cfs *policy_info;
	int s;
	mali_bool reset_needed = MALI_FALSE;

	KBASE_DEBUG_ASSERT(timer != NULL);

	policy_info = container_of(timer, struct kbasep_js_policy_cfs, scheduling_timer);
	kbdev = container_of(policy_info, struct kbase_device, js_data.policy.cfs);
	js_devdata = &kbdev->js_data;

	/*                        */
	spin_lock_irqsave(&js_devdata->runpool_irq.lock, flags);
	for (s = 0; s < kbdev->gpu_props.num_job_slots; s++) {
		struct kbase_jm_slot *slot = &kbdev->jm_slots[s];
		struct kbase_jd_atom *atom = NULL;

		if (kbasep_jm_nr_jobs_submitted(slot) > 0) {
			atom = kbasep_jm_peek_idx_submit_slot(slot, 0);
			KBASE_DEBUG_ASSERT(atom != NULL);

			if (kbasep_jm_is_dummy_workaround_job(kbdev, atom) != MALI_FALSE) {
				/*                                                                                                 */
				atom = NULL;
			}
		}

		if (atom != NULL) {
			/*                                                            */
			if (!kbase_hw_has_issue(kbdev, BASE_HW_ISSUE_5736)) {
				u32 ticks = atom->sched_info.cfs.ticks++;

#if !CINSTR_DUMPING_ENABLED
				u32 soft_stop_ticks, hard_stop_ticks, gpu_reset_ticks;
				if (atom->core_req & BASE_JD_REQ_ONLY_COMPUTE) {
					soft_stop_ticks = js_devdata->soft_stop_ticks_cl;
					hard_stop_ticks = js_devdata->hard_stop_ticks_cl;
					gpu_reset_ticks = js_devdata->gpu_reset_ticks_cl;
				} else {
					soft_stop_ticks = js_devdata->soft_stop_ticks;
					hard_stop_ticks = js_devdata->hard_stop_ticks_ss;
					gpu_reset_ticks = js_devdata->gpu_reset_ticks_ss;
				}

				/*                       */
				if (ticks == soft_stop_ticks) {
					int disjoint_threshold =
							KBASE_DISJOINT_STATE_INTERLEAVED_CONTEXT_COUNT_THRESHOLD;
					u32 softstop_flags = 0u;
					/*                                                                       
                                                    
      */
					dev_dbg(kbdev->dev, "Soft-stop");

#if !KBASE_DISABLE_SCHEDULING_SOFT_STOPS
					/*                                                            
                                    
       
                                                                   
                                                                   
                                           
       
                                                                   
                                                                 
                                                               
                                        */
					if (js_devdata->nr_user_contexts_running >= disjoint_threshold)
						softstop_flags |= JS_COMMAND_SW_CAUSES_DISJOINT;
					kbase_job_slot_softstop_swflags(kbdev,
							s, atom, softstop_flags);
#endif
				} else if (ticks == hard_stop_ticks) {
					/*                                                                          
                                                                    
      */
#if !KBASE_DISABLE_SCHEDULING_HARD_STOPS
					dev_warn(kbdev->dev, "JS: Job Hard-Stopped (took more than %lu ticks at %lu ms/tick)", (unsigned long)ticks, (unsigned long)(js_devdata->scheduling_tick_ns / 1000000u));
					kbase_job_slot_hardstop(atom->kctx, s, atom);
#endif
				} else if (ticks == gpu_reset_ticks) {
					/*                                                                          
                                                                                  
      */
					reset_needed = MALI_TRUE;
				}
#else				/*                         */
				/*                                                                          
                                                                                 
                                  */
				if (ticks == js_devdata->soft_stop_ticks) {
					/*                                                                 
                                                                   
      */
					dev_dbg(kbdev->dev, "Soft-stop");
				} else if (ticks == js_devdata->hard_stop_ticks_nss) {
					/*                                                                           
                           
      */
#if !KBASE_DISABLE_SCHEDULING_HARD_STOPS
					dev_warn(kbdev->dev, "JS: Job Hard-Stopped (took more than %lu ticks at %lu ms/tick)", (unsigned long)ticks, (unsigned long)(js_devdata->scheduling_tick_ns / 1000000u));
					kbase_job_slot_hardstop(atom->kctx, s, atom);
#endif
				} else if (ticks == js_devdata->gpu_reset_ticks_nss) {
					/*                                                                           
                                                                                  
      */
					reset_needed = MALI_TRUE;
				}
#endif				/*                         */
			}
		}
	}
#if KBASE_GPU_RESET_EN
	if (reset_needed) {
		dev_err(kbdev->dev, "JS: Job has been on the GPU for too long (KBASE_CONFIG_ATTR_JS_RESET_TICKS_SS/NSS timeout hit). Issueing GPU soft-reset to resolve.");

		if (kbase_prepare_to_reset_gpu_locked(kbdev))
			kbase_reset_gpu_locked(kbdev);
	}
#endif /*                    */
	/*                                                             */

	if (timer_callback_should_run(kbdev) != MALI_FALSE) {
		hrtimer_start(&policy_info->scheduling_timer, HR_TIMER_DELAY_NSEC(js_devdata->scheduling_tick_ns), HRTIMER_MODE_REL);
	} else {
		KBASE_TRACE_ADD(kbdev, JS_POLICY_TIMER_END, NULL, NULL, 0u, 0u);
		/*                                                                          */
	}

	spin_unlock_irqrestore(&js_devdata->runpool_irq.lock, flags);

	return HRTIMER_NORESTART;
}

/*
                        
 */

mali_error kbasep_js_policy_init(struct kbase_device *kbdev)
{
	struct kbasep_js_device_data *js_devdata;
	struct kbasep_js_policy_cfs *policy_info;

	KBASE_DEBUG_ASSERT(kbdev != NULL);
	js_devdata = &kbdev->js_data;
	policy_info = &js_devdata->policy.cfs;

	INIT_LIST_HEAD(&policy_info->ctx_queue_head);
	INIT_LIST_HEAD(&policy_info->scheduled_ctxs_head);
	INIT_LIST_HEAD(&policy_info->ctx_rt_queue_head);

	atomic64_set(&policy_info->least_runtime_us, KBASEP_JS_RUNTIME_EMPTY);
	atomic64_set(&policy_info->rt_least_runtime_us, KBASEP_JS_RUNTIME_EMPTY);

	hrtimer_init(&policy_info->scheduling_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	policy_info->scheduling_timer.function = timer_callback;

	policy_info->timer_running = MALI_FALSE;
	policy_info->head_runtime_us = 0;

	/*                                */
	build_core_req_variants(kbdev, policy_info);
	/*                                   */
	build_slot_lookups(kbdev, policy_info);

	return MALI_ERROR_NONE;
}

void kbasep_js_policy_term(union kbasep_js_policy *js_policy)
{
	struct kbasep_js_policy_cfs *policy_info;

	KBASE_DEBUG_ASSERT(js_policy != NULL);
	policy_info = &js_policy->cfs;

	/*                                          */
	KBASE_DEBUG_ASSERT(list_empty(&policy_info->ctx_queue_head));
	KBASE_DEBUG_ASSERT(KBASEP_JS_RUNTIME_EMPTY == atomic64_read(&policy_info->least_runtime_us));

	/*                                             */
	KBASE_DEBUG_ASSERT(list_empty(&policy_info->scheduled_ctxs_head));

	/*                                          */
	KBASE_DEBUG_ASSERT(list_empty(&policy_info->ctx_rt_queue_head));
	KBASE_DEBUG_ASSERT(KBASEP_JS_RUNTIME_EMPTY == atomic64_read(&policy_info->rt_least_runtime_us));

	hrtimer_cancel(&policy_info->scheduling_timer);
}

mali_error kbasep_js_policy_init_ctx(struct kbase_device *kbdev, struct kbase_context *kctx)
{
	struct kbasep_js_device_data *js_devdata;
	struct kbasep_js_policy_cfs_ctx *ctx_info;
	struct kbasep_js_policy_cfs *policy_info;
	u32 i;
	int policy;

	KBASE_DEBUG_ASSERT(kbdev != NULL);
	KBASE_DEBUG_ASSERT(kctx != NULL);

	js_devdata = &kbdev->js_data;
	policy_info = &kbdev->js_data.policy.cfs;
	ctx_info = &kctx->jctx.sched_info.runpool.policy_ctx.cfs;

	KBASE_TRACE_ADD_REFCOUNT(kbdev, JS_POLICY_INIT_CTX, kctx, NULL, 0u, kbasep_js_policy_trace_get_refcnt(kbdev, kctx));

	for (i = 0; i < policy_info->num_core_req_variants; ++i)
		INIT_LIST_HEAD(&ctx_info->job_list_head[i]);

	policy = current->policy;
	if (policy == SCHED_FIFO || policy == SCHED_RR) {
		ctx_info->process_rt_policy = MALI_TRUE;
		ctx_info->process_priority = (((MAX_RT_PRIO - 1) - current->rt_priority) / 5) - 20;
	} else {
		ctx_info->process_rt_policy = MALI_FALSE;
		ctx_info->process_priority = (current->static_prio - MAX_RT_PRIO) - 20;
	}

	ctx_info->bag_total_priority = 0;
	ctx_info->bag_total_nr_atoms = 0;

	/*                                                        
   
                                                                             
                                                                        
                                                                     */
	mutex_lock(&js_devdata->queue_mutex);

	/*                                                                          
                                                                     
                                                                       */
	ctx_info->runtime_us = policy_info->head_runtime_us + priority_weight(ctx_info, (u64) js_devdata->cfs_ctx_runtime_init_slices * (u64) (js_devdata->ctx_timeslice_ns / 1000u));

	mutex_unlock(&js_devdata->queue_mutex);

	return MALI_ERROR_NONE;
}

void kbasep_js_policy_term_ctx(union kbasep_js_policy *js_policy, struct kbase_context *kctx)
{
	struct kbasep_js_policy_cfs_ctx *ctx_info;
	struct kbasep_js_policy_cfs *policy_info;
	u32 i;

	KBASE_DEBUG_ASSERT(js_policy != NULL);
	KBASE_DEBUG_ASSERT(kctx != NULL);

	policy_info = &js_policy->cfs;
	ctx_info = &kctx->jctx.sched_info.runpool.policy_ctx.cfs;

	{
		struct kbase_device *kbdev = container_of(js_policy, struct kbase_device, js_data.policy);
		KBASE_TRACE_ADD_REFCOUNT(kbdev, JS_POLICY_TERM_CTX, kctx, NULL, 0u, kbasep_js_policy_trace_get_refcnt(kbdev, kctx));
	}

	/*                                 */
	for (i = 0; i < policy_info->num_core_req_variants; ++i)
		KBASE_DEBUG_ASSERT(list_empty(&ctx_info->job_list_head[i]));

	/*               */
}

/*
                     
 */

void kbasep_js_policy_enqueue_ctx(union kbasep_js_policy *js_policy, struct kbase_context *kctx)
{
	struct kbasep_js_policy_cfs *policy_info;
	struct kbasep_js_policy_cfs_ctx *ctx_info;
	struct kbase_context *head_ctx;
	struct kbase_context *list_kctx = NULL;
	struct kbasep_js_device_data *js_devdata;
	struct list_head *queue_head;
	struct list_head *pos;
	struct kbase_device *kbdev;
	atomic64_t *least_runtime_us;
	u64 head_runtime;

	KBASE_DEBUG_ASSERT(js_policy != NULL);
	KBASE_DEBUG_ASSERT(kctx != NULL);

	policy_info = &js_policy->cfs;
	ctx_info = &kctx->jctx.sched_info.runpool.policy_ctx.cfs;
	kbdev = container_of(js_policy, struct kbase_device, js_data.policy);
	js_devdata = &kbdev->js_data;

	KBASE_TRACE_ADD_REFCOUNT(kbdev, JS_POLICY_ENQUEUE_CTX, kctx, NULL, 0u, kbasep_js_policy_trace_get_refcnt(kbdev, kctx));

	/*                                         */
	kbasep_js_debug_check(policy_info, kctx, KBASEP_JS_CHECK_NOTQUEUED);

	/*                                                                      */
	if (policy_info->head_runtime_us > ctx_info->runtime_us + (u64) js_devdata->cfs_ctx_runtime_min_slices * (u64) (js_devdata->ctx_timeslice_ns / 1000u)) {
		/*                                                                         
                                                                                   
                                                                                 
              */
		ctx_info->runtime_us = policy_info->head_runtime_us - (u64) js_devdata->cfs_ctx_runtime_min_slices * (u64) (js_devdata->ctx_timeslice_ns / 1000u);
	}

	/*                                                        */
	if (ctx_info->process_rt_policy) {
		queue_head = &policy_info->ctx_rt_queue_head;
		least_runtime_us = &policy_info->rt_least_runtime_us;
	} else {
		queue_head = &policy_info->ctx_queue_head;
		least_runtime_us = &policy_info->least_runtime_us;
	}

	if (list_empty(queue_head)) {
		list_add_tail(&kctx->jctx.sched_info.runpool.policy_ctx.cfs.list, queue_head);
	} else {
		list_for_each(pos, queue_head) {
			struct kbasep_js_policy_cfs_ctx *list_ctx_info;

			list_kctx = list_entry(pos, struct kbase_context, jctx.sched_info.runpool.policy_ctx.cfs.list);
			list_ctx_info = &list_kctx->jctx.sched_info.runpool.policy_ctx.cfs;

			if ((kctx->jctx.sched_info.ctx.flags & KBASE_CTX_FLAG_PRIVILEGED) != 0)
				break;

			if ((list_ctx_info->runtime_us > ctx_info->runtime_us) && ((list_kctx->jctx.sched_info.ctx.flags & KBASE_CTX_FLAG_PRIVILEGED) == 0))
				break;
		}

		/*                              */
		list_add_tail(&kctx->jctx.sched_info.runpool.policy_ctx.cfs.list, &list_kctx->jctx.sched_info.runpool.policy_ctx.cfs.list);
	}

	/*                                      */
	head_ctx = list_entry(queue_head->next, struct kbase_context, jctx.sched_info.runpool.policy_ctx.cfs.list);
	head_runtime = head_ctx->jctx.sched_info.runpool.policy_ctx.cfs.runtime_us;
	atomic64_set(least_runtime_us, head_runtime);
}

mali_bool kbasep_js_policy_dequeue_head_ctx(union kbasep_js_policy *js_policy, struct kbase_context ** const kctx_ptr)
{
	struct kbasep_js_policy_cfs *policy_info;
	struct kbase_context *head_ctx;
	struct list_head *queue_head;
	atomic64_t *least_runtime_us;
	struct kbase_device *kbdev;

	KBASE_DEBUG_ASSERT(js_policy != NULL);
	KBASE_DEBUG_ASSERT(kctx_ptr != NULL);

	policy_info = &js_policy->cfs;
	kbdev = container_of(js_policy, struct kbase_device, js_data.policy);

	/*                                                     */
	if (list_empty(&policy_info->ctx_rt_queue_head)) {
		if (list_empty(&policy_info->ctx_queue_head)) {
			/*                    */
			return MALI_FALSE;
		} else {
			queue_head = &policy_info->ctx_queue_head;
			least_runtime_us = &policy_info->least_runtime_us;
		}
	} else {
		queue_head = &policy_info->ctx_rt_queue_head;
		least_runtime_us = &policy_info->rt_least_runtime_us;
	}

	/*                                                   */
	*kctx_ptr = list_entry(queue_head->next, struct kbase_context, jctx.sched_info.runpool.policy_ctx.cfs.list);
	/*                                                                               */
	if (queue_head->next->next == queue_head)
		atomic64_set(least_runtime_us, KBASEP_JS_RUNTIME_EMPTY);
	list_del(queue_head->next);

	KBASE_TRACE_ADD_REFCOUNT(kbdev, JS_POLICY_DEQUEUE_HEAD_CTX, *kctx_ptr, NULL, 0u, kbasep_js_policy_trace_get_refcnt(kbdev, *kctx_ptr));

	/*                         */
	if (!list_empty(queue_head)) {
		u64 head_runtime;

		head_ctx = list_entry(queue_head->next, struct kbase_context, jctx.sched_info.runpool.policy_ctx.cfs.list);

		/*                                                                
                                                                   
                                                                           */
		head_runtime = head_ctx->jctx.sched_info.runpool.policy_ctx.cfs.runtime_us;

		if (head_runtime > policy_info->head_runtime_us)
			policy_info->head_runtime_us = head_runtime;

		atomic64_set(least_runtime_us, head_runtime);
	}

	return MALI_TRUE;
}

mali_bool kbasep_js_policy_try_evict_ctx(union kbasep_js_policy *js_policy, struct kbase_context *kctx)
{
	struct kbasep_js_policy_cfs_ctx *ctx_info;
	struct kbasep_js_policy_cfs *policy_info;
	mali_bool is_present;
	struct list_head *queue_head;
	atomic64_t *least_runtime_us;
	struct list_head *qhead;
	struct kbase_device *kbdev;

	KBASE_DEBUG_ASSERT(js_policy != NULL);
	KBASE_DEBUG_ASSERT(kctx != NULL);

	policy_info = &js_policy->cfs;
	ctx_info = &kctx->jctx.sched_info.runpool.policy_ctx.cfs;
	kbdev = container_of(js_policy, struct kbase_device, js_data.policy);

	if (ctx_info->process_rt_policy) {
		queue_head = &policy_info->ctx_rt_queue_head;
		least_runtime_us = &policy_info->rt_least_runtime_us;
	} else {
		queue_head = &policy_info->ctx_queue_head;
		least_runtime_us = &policy_info->least_runtime_us;
	}

	qhead = queue_head;

	is_present = kbasep_list_member_of(qhead, &kctx->jctx.sched_info.runpool.policy_ctx.cfs.list);

	KBASE_TRACE_ADD_REFCOUNT_INFO(kbdev, JS_POLICY_TRY_EVICT_CTX, kctx, NULL, 0u, kbasep_js_policy_trace_get_refcnt(kbdev, kctx), is_present);

	if (is_present != MALI_FALSE) {
		struct kbase_context *head_ctx;

		qhead = queue_head;

		/*                                                                               */
		if (queue_head->next->next == queue_head)
			atomic64_set(least_runtime_us, KBASEP_JS_RUNTIME_EMPTY);

		/*                    */
		list_del(&kctx->jctx.sched_info.runpool.policy_ctx.cfs.list);

		qhead = queue_head;
		/*                         */
		if (!list_empty(qhead)) {
			u64 head_runtime;

			head_ctx = list_entry(qhead->next, struct kbase_context, jctx.sched_info.runpool.policy_ctx.cfs.list);

			/*                                                                
                                                                    
                                                                            */
			head_runtime = head_ctx->jctx.sched_info.runpool.policy_ctx.cfs.runtime_us;

			if (head_runtime > policy_info->head_runtime_us)
				policy_info->head_runtime_us = head_runtime;

			atomic64_set(least_runtime_us, head_runtime);
		}
	}

	return is_present;
}

void kbasep_js_policy_foreach_ctx_job(union kbasep_js_policy *js_policy, struct kbase_context *kctx,
	kbasep_js_policy_ctx_job_cb callback, mali_bool detach_jobs)
{
	struct kbasep_js_policy_cfs *policy_info;
	struct kbasep_js_policy_cfs_ctx *ctx_info;
	struct kbase_device *kbdev;
	u32 i;

	KBASE_DEBUG_ASSERT(js_policy != NULL);
	KBASE_DEBUG_ASSERT(kctx != NULL);

	kbdev = container_of(js_policy, struct kbase_device, js_data.policy);
	policy_info = &js_policy->cfs;
	ctx_info = &kctx->jctx.sched_info.runpool.policy_ctx.cfs;

	KBASE_TRACE_ADD_REFCOUNT(kbdev, JS_POLICY_FOREACH_CTX_JOBS, kctx, NULL, 0u, kbasep_js_policy_trace_get_refcnt(kbdev, kctx));

	/*                                                 */
	for (i = 0; i < policy_info->num_core_req_variants; ++i) {
		struct list_head *job_list;
		struct kbase_jd_atom *atom;
		struct kbase_jd_atom *tmp_iter;
		job_list = &ctx_info->job_list_head[i];
		/*                                                               
                                 */
		list_for_each_entry_safe(atom, tmp_iter, job_list, sched_info.cfs.list) {
			if (detach_jobs)
				list_del(&atom->sched_info.cfs.list);
			callback(kbdev, atom);
		}
	}

}

void kbasep_js_policy_runpool_add_ctx(union kbasep_js_policy *js_policy, struct kbase_context *kctx)
{
	struct kbasep_js_policy_cfs *policy_info;
	struct kbasep_js_device_data *js_devdata;
	struct kbase_device *kbdev;

	KBASE_DEBUG_ASSERT(js_policy != NULL);
	KBASE_DEBUG_ASSERT(kctx != NULL);

	policy_info = &js_policy->cfs;
	js_devdata = container_of(js_policy, struct kbasep_js_device_data, policy);

	kbdev = kctx->kbdev;

	{
		KBASE_TRACE_ADD_REFCOUNT(kbdev, JS_POLICY_RUNPOOL_ADD_CTX, kctx, NULL, 0u, kbasep_js_policy_trace_get_refcnt_nolock(kbdev, kctx));
	}

	/*                                         */
	kbasep_js_debug_check(policy_info, kctx, KBASEP_JS_CHECK_NOTSCHEDULED);

	/*                                                     */
	list_add_tail(&kctx->jctx.sched_info.runpool.policy_ctx.cfs.list, &policy_info->scheduled_ctxs_head);

	if (timer_callback_should_run(kbdev) != MALI_FALSE && policy_info->timer_running == MALI_FALSE) {
		hrtimer_start(&policy_info->scheduling_timer, HR_TIMER_DELAY_NSEC(js_devdata->scheduling_tick_ns), HRTIMER_MODE_REL);

		KBASE_TRACE_ADD(kbdev, JS_POLICY_TIMER_START, NULL, NULL, 0u, 0u);
		policy_info->timer_running = MALI_TRUE;
	}
}

void kbasep_js_policy_runpool_remove_ctx(union kbasep_js_policy *js_policy, struct kbase_context *kctx)
{
	struct kbasep_js_policy_cfs *policy_info;

	KBASE_DEBUG_ASSERT(js_policy != NULL);
	KBASE_DEBUG_ASSERT(kctx != NULL);

	policy_info = &js_policy->cfs;

	{
		struct kbase_device *kbdev = container_of(js_policy, struct kbase_device, js_data.policy);

		KBASE_TRACE_ADD_REFCOUNT(kbdev, JS_POLICY_RUNPOOL_REMOVE_CTX, kctx, NULL, 0u, kbasep_js_policy_trace_get_refcnt_nolock(kbdev, kctx));
	}

	/*                                         */
	kbasep_js_debug_check(policy_info, kctx, KBASEP_JS_CHECK_SCHEDULED);

	/*                                                                              */
	list_del(&kctx->jctx.sched_info.runpool.policy_ctx.cfs.list);
}

mali_bool kbasep_js_policy_should_remove_ctx(union kbasep_js_policy *js_policy, struct kbase_context *kctx)
{
	struct kbasep_js_policy_cfs_ctx *ctx_info;
	struct kbasep_js_policy_cfs *policy_info;
	struct kbasep_js_device_data *js_devdata;
	u64 least_runtime_us;

	KBASE_DEBUG_ASSERT(js_policy != NULL);
	KBASE_DEBUG_ASSERT(kctx != NULL);

	policy_info = &js_policy->cfs;
	ctx_info = &kctx->jctx.sched_info.runpool.policy_ctx.cfs;
	js_devdata = container_of(js_policy, struct kbasep_js_device_data, policy);

	if (ctx_info->process_rt_policy)
		least_runtime_us = atomic64_read(&policy_info->rt_least_runtime_us);
	else
		least_runtime_us = atomic64_read(&policy_info->least_runtime_us);

	if (KBASEP_JS_RUNTIME_EMPTY == least_runtime_us) {
		/*                */
		return MALI_FALSE;
	}

	if ((least_runtime_us + priority_weight(ctx_info, (u64) (js_devdata->ctx_timeslice_ns / 1000u)))
	    < ctx_info->runtime_us) {
		/*                                                                        
                                                                                 
                                                                                  
   */
		return MALI_TRUE;
	}

	return MALI_FALSE;
}

void kbasep_js_policy_runpool_timers_sync(union kbasep_js_policy *js_policy)
{
	struct kbasep_js_policy_cfs *policy_info;
	struct kbase_device *kbdev;
	struct kbasep_js_device_data *js_devdata;

	KBASE_DEBUG_ASSERT(js_policy != NULL);

	policy_info = &js_policy->cfs;
	kbdev = container_of(js_policy, struct kbase_device, js_data.policy);
	js_devdata = &kbdev->js_data;

	if (!timer_callback_should_run(kbdev)) {
		unsigned long flags;

		/*                                                    
                                                                          
                                      */
		spin_lock_irqsave(&js_devdata->runpool_irq.lock, flags);
		spin_unlock_irqrestore(&js_devdata->runpool_irq.lock, flags);

		/*                                                                   
                                                                   
                                                             
                                                                 */
		hrtimer_cancel(&policy_info->scheduling_timer);

		policy_info->timer_running = MALI_FALSE;
	}
}

/*
                       
 */

mali_error kbasep_js_policy_init_job(const union kbasep_js_policy *js_policy, const struct kbase_context *kctx, struct kbase_jd_atom *katom)
{
	const struct kbasep_js_policy_cfs *policy_info;

	KBASE_DEBUG_ASSERT(js_policy != NULL);
	KBASE_DEBUG_ASSERT(katom != NULL);
	KBASE_DEBUG_ASSERT(kctx != NULL);

	policy_info = &js_policy->cfs;

	/*                                                                           
                                          */
	return cached_variant_idx_init(policy_info, kctx, katom);
}

void kbasep_js_policy_register_job(union kbasep_js_policy *js_policy, struct kbase_context *kctx, struct kbase_jd_atom *katom)
{
	struct kbasep_js_policy_cfs_ctx *ctx_info;

	KBASE_DEBUG_ASSERT(js_policy != NULL);
	KBASE_DEBUG_ASSERT(katom != NULL);
	KBASE_DEBUG_ASSERT(kctx != NULL);

	ctx_info = &kctx->jctx.sched_info.runpool.policy_ctx.cfs;

	/*                                                */
	ctx_info->bag_total_nr_atoms++;
	ctx_info->bag_total_priority += katom->nice_prio;

	/*                                                        */
	if (ctx_info->bag_total_nr_atoms)
		ctx_info->bag_priority = (ctx_info->bag_total_priority / ctx_info->bag_total_nr_atoms) - 20;
}

void kbasep_js_policy_deregister_job(union kbasep_js_policy *js_policy, struct kbase_context *kctx, struct kbase_jd_atom *katom)
{
	struct kbasep_js_policy_cfs_ctx *ctx_info;

	KBASE_DEBUG_ASSERT(js_policy != NULL);
	CSTD_UNUSED(js_policy);
	KBASE_DEBUG_ASSERT(katom != NULL);
	KBASE_DEBUG_ASSERT(kctx != NULL);

	ctx_info = &kctx->jctx.sched_info.runpool.policy_ctx.cfs;

	/*                                                          */
	KBASE_DEBUG_ASSERT(ctx_info->bag_total_nr_atoms > 0);
	ctx_info->bag_total_nr_atoms--;
	ctx_info->bag_total_priority -= katom->nice_prio;
	KBASE_DEBUG_ASSERT(ctx_info->bag_total_priority >= 0);

	/*                                                        */
	if (ctx_info->bag_total_nr_atoms)
		ctx_info->bag_priority = (ctx_info->bag_total_priority / ctx_info->bag_total_nr_atoms) - 20;
}
KBASE_EXPORT_TEST_API(kbasep_js_policy_deregister_job)

mali_bool kbasep_js_policy_dequeue_job(struct kbase_device *kbdev,
				       int job_slot_idx,
				       struct kbase_jd_atom ** const katom_ptr)
{
	struct kbasep_js_device_data *js_devdata;
	struct kbasep_js_policy_cfs *policy_info;
	struct kbase_context *kctx;
	u32 variants_supported;
	struct list_head *pos;

	KBASE_DEBUG_ASSERT(kbdev != NULL);
	KBASE_DEBUG_ASSERT(katom_ptr != NULL);
	KBASE_DEBUG_ASSERT(job_slot_idx < BASE_JM_MAX_NR_SLOTS);

	js_devdata = &kbdev->js_data;
	policy_info = &js_devdata->policy.cfs;

	/*                                */
	if (kbdev->gpu_props.num_core_groups > 1 && kbasep_js_ctx_attr_is_attr_on_runpool(kbdev, KBASEP_JS_CTX_ATTR_COMPUTE_ALL_CORES) != MALI_FALSE) {
		/*                                                       */
		variants_supported = get_slot_to_variant_lookup(policy_info->slot_to_variant_lookup_ss_allcore_state, job_slot_idx);
	} else {
		/*          */
		variants_supported = get_slot_to_variant_lookup(policy_info->slot_to_variant_lookup_ss_state, job_slot_idx);
	}

	/*                                                                       */
	list_for_each(pos, &policy_info->scheduled_ctxs_head) {
		kctx = list_entry(pos, struct kbase_context, jctx.sched_info.runpool.policy_ctx.cfs.list);
		if (kctx->jctx.sched_info.runpool.policy_ctx.cfs.process_rt_policy) {
			if (dequeue_job(kbdev, kctx, variants_supported, katom_ptr, job_slot_idx)) {
				/*                             */
				return MALI_TRUE;
			}
		}
	}

	/*                                                                            */
	list_for_each(pos, &policy_info->scheduled_ctxs_head) {
		kctx = list_entry(pos, struct kbase_context, jctx.sched_info.runpool.policy_ctx.cfs.list);
		if (kctx->jctx.sched_info.runpool.policy_ctx.cfs.process_rt_policy == MALI_FALSE) {
			if (dequeue_job(kbdev, kctx, variants_supported, katom_ptr, job_slot_idx)) {
				/*                                 */
				return MALI_TRUE;
			}
		}
	}

	/*                                               */
	return MALI_FALSE;
}

void kbasep_js_policy_enqueue_job(union kbasep_js_policy *js_policy, struct kbase_jd_atom *katom)
{
	struct kbasep_js_policy_cfs_job *job_info;
	struct kbasep_js_policy_cfs_ctx *ctx_info;
	struct kbase_context *parent_ctx;

	KBASE_DEBUG_ASSERT(js_policy != NULL);
	KBASE_DEBUG_ASSERT(katom != NULL);
	parent_ctx = katom->kctx;
	KBASE_DEBUG_ASSERT(parent_ctx != NULL);

	job_info = &katom->sched_info.cfs;
	ctx_info = &parent_ctx->jctx.sched_info.runpool.policy_ctx.cfs;

	{
		struct kbase_device *kbdev = container_of(js_policy, struct kbase_device, js_data.policy);
		KBASE_TRACE_ADD(kbdev, JS_POLICY_ENQUEUE_JOB, katom->kctx, katom, katom->jc, 0);
	}
	list_add_tail(&katom->sched_info.cfs.list, &ctx_info->job_list_head[job_info->cached_variant_idx]);
}

void kbasep_js_policy_log_job_result(union kbasep_js_policy *js_policy, struct kbase_jd_atom *katom, u64 time_spent_us)
{
	struct kbasep_js_policy_cfs_ctx *ctx_info;
	struct kbase_context *parent_ctx;
	KBASE_DEBUG_ASSERT(js_policy != NULL);
	KBASE_DEBUG_ASSERT(katom != NULL);
	CSTD_UNUSED(js_policy);

	parent_ctx = katom->kctx;
	KBASE_DEBUG_ASSERT(parent_ctx != NULL);

	ctx_info = &parent_ctx->jctx.sched_info.runpool.policy_ctx.cfs;

	ctx_info->runtime_us += priority_weight(ctx_info, time_spent_us);

	katom->time_spent_us += time_spent_us;
}

mali_bool kbasep_js_policy_ctx_has_priority(union kbasep_js_policy *js_policy, struct kbase_context *current_ctx, struct kbase_context *new_ctx)
{
	struct kbasep_js_policy_cfs_ctx *current_ctx_info;
	struct kbasep_js_policy_cfs_ctx *new_ctx_info;

	KBASE_DEBUG_ASSERT(current_ctx != NULL);
	KBASE_DEBUG_ASSERT(new_ctx != NULL);
	CSTD_UNUSED(js_policy);

	current_ctx_info = &current_ctx->jctx.sched_info.runpool.policy_ctx.cfs;
	new_ctx_info = &new_ctx->jctx.sched_info.runpool.policy_ctx.cfs;

	if ((current_ctx_info->process_rt_policy == MALI_FALSE) && (new_ctx_info->process_rt_policy == MALI_TRUE))
		return MALI_TRUE;

	if ((current_ctx_info->process_rt_policy == new_ctx_info->process_rt_policy) && (current_ctx_info->bag_priority > new_ctx_info->bag_priority))
		return MALI_TRUE;

	return MALI_FALSE;
}
