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





#if !defined(_KBASE_TRACE_TIMELINE_H)
#define _KBASE_TRACE_TIMELINE_H

#ifdef CONFIG_MALI_TRACE_TIMELINE

typedef enum
{
	#define KBASE_TIMELINE_TRACE_CODE(enum_val, desc, format, format_desc) enum_val
	#include "mali_kbase_trace_timeline_defs.h"
	#undef KBASE_TIMELINE_TRACE_CODE
} kbase_trace_timeline_code;

/*                                      */
mali_error kbasep_trace_timeline_debugfs_init(struct kbase_device *kbdev);
/*                                     */
void kbasep_trace_timeline_debugfs_term(struct kbase_device *kbdev);

/*                                                                      
             
                                                                             
                                                                   */
#include "mali_timeline.h"

/*                                                                            
                                     */
#define KBASE_TIMELINE_ATOMS_IN_FLIGHT(kctx, count)                                 \
	do                                                                          \
	{                                                                           \
		struct timespec ts;                                                 \
		getnstimeofday(&ts);                                                \
		trace_mali_timeline_atoms_in_flight(ts.tv_sec, ts.tv_nsec,          \
		                                            (int)kctx->timeline.owner_tgid, \
		                                            count);                         \
	} while (0)

/*                                  */
#define KBASE_TIMELINE_ATOM_READY(kctx, atom_id)                   \
	do                                                             \
	{                                                              \
		struct timespec ts;                                        \
		getnstimeofday(&ts);                                       \
		trace_mali_timeline_atom(ts.tv_sec, ts.tv_nsec,            \
		                         CTX_FLOW_ATOM_READY,              \
		                         (int)kctx->timeline.owner_tgid,   \
		                         atom_id);                         \
	} while (0)

/*                                               
  
                                                                             
                                                                 
  
                                                                             
                                     */
#define KBASE_TIMELINE_ATOMS_SUBMITTED(kctx, js, count)                             \
	do                                                                          \
	{                                                                           \
		struct timespec ts;                                                 \
		getnstimeofday(&ts);                                                \
		trace_mali_timeline_gpu_slot_active(ts.tv_sec, ts.tv_nsec,          \
		                                    SW_SET_GPU_SLOT_ACTIVE,         \
		                                    (int)kctx->timeline.owner_tgid, \
		                                    js, count);                     \
	} while (0)


/*                                */
#define KBASE_TIMELINE_JOB_START_NEXT(kctx, js, count)                             \
	do                                                                          \
	{                                                                           \
		struct timespec ts;                                                 \
		getnstimeofday(&ts);                                                \
		trace_mali_timeline_gpu_slot_action(ts.tv_sec, ts.tv_nsec,          \
		                                    SW_SET_GPU_SLOT_NEXT, \
		                                    (int)kctx->timeline.owner_tgid, \
		                                    js, count);                     \
	} while (0)

/*                                */
#define KBASE_TIMELINE_JOB_START_HEAD(kctx, js, count)                             \
	do                                                                          \
	{                                                                           \
		struct timespec ts;                                                 \
		getnstimeofday(&ts);                                                \
		trace_mali_timeline_gpu_slot_action(ts.tv_sec, ts.tv_nsec,          \
		                                    SW_SET_GPU_SLOT_HEAD,           \
		                                    (int)kctx->timeline.owner_tgid, \
		                                    js, count);                     \
	} while (0)

/*                                                                     */
#define KBASE_TIMELINE_TRY_SOFT_STOP(kctx, js, count) \
	do                                                                          \
	{                                                                           \
		struct timespec ts;                                                 \
		getnstimeofday(&ts);                                                \
		trace_mali_timeline_gpu_slot_action(ts.tv_sec, ts.tv_nsec,          \
		                                    SW_SET_GPU_SLOT_STOPPING,       \
		                                    (kctx) ? (int)kctx->timeline.owner_tgid : 0, \
		                                    js, count);                     \
	} while (0)



/*                                  */
#define KBASE_TIMELINE_GPU_POWER(kbdev, active)                                    \
	do                                                                         \
	{                                                                          \
		struct timespec ts;                                                \
		getnstimeofday(&ts);                                               \
		trace_mali_timeline_gpu_power_active(ts.tv_sec, ts.tv_nsec,        \
		                                     SW_SET_GPU_POWER_ACTIVE, active); \
	} while (0)

/*                            */
#define KBASE_TIMELINE_POWER_TILER(kbdev, bitmap)                               \
	do                                                                      \
	{                                                                       \
		struct timespec ts;                                             \
		getnstimeofday(&ts);                                            \
		trace_mali_timeline_gpu_power_active(ts.tv_sec, ts.tv_nsec,     \
		                                     SW_SET_GPU_POWER_TILER_ACTIVE, \
		                                     hweight64(bitmap));        \
	} while (0)

/*                                           */
#define KBASE_TIMELINE_POWER_SHADER(kbdev, bitmap)                               \
	do                                                                       \
	{                                                                        \
		struct timespec ts;                                              \
		getnstimeofday(&ts);                                             \
		trace_mali_timeline_gpu_power_active(ts.tv_sec, ts.tv_nsec,      \
		                                     SW_SET_GPU_POWER_SHADER_ACTIVE, \
		                                     hweight64(bitmap));         \
	} while (0)

/*                         */
#define KBASE_TIMELINE_POWER_L2(kbdev, bitmap)                              \
	do                                                                      \
	{                                                                       \
		struct timespec ts;                                                 \
		getnstimeofday(&ts);                                                \
		trace_mali_timeline_gpu_power_active(ts.tv_sec, ts.tv_nsec,         \
				                             SW_SET_GPU_POWER_L2_ACTIVE,    \
				                             hweight64(bitmap));            \
	} while (0)

/*                        */
#define KBASE_TIMELINE_POWERING_L2(kbdev)                                   \
	do                                                                      \
	{                                                                       \
		struct timespec ts;                                                 \
		getnstimeofday(&ts);                                                \
		trace_mali_timeline_l2_power_active(ts.tv_sec, ts.tv_nsec,	        \
		                                    SW_FLOW_GPU_POWER_L2_POWERING,  \
		                                    1);                             \
	} while (0)

#define KBASE_TIMELINE_POWERED_L2(kbdev)                                    \
	do                                                                      \
	{                                                                       \
		struct timespec ts;                                                 \
		getnstimeofday(&ts);                                                \
		trace_mali_timeline_l2_power_active(ts.tv_sec, ts.tv_nsec,          \
		                                    SW_FLOW_GPU_POWER_L2_ACTIVE,    \
		                                     1);                            \
	} while (0)

/*                                        */
#define KBASE_TIMELINE_PM_SEND_EVENT(kbdev, event_type, pm_event_id) \
	do                                                               \
	{                                                                \
		struct timespec ts;                                          \
		getnstimeofday(&ts);                                         \
		trace_mali_timeline_pm_event(ts.tv_sec, ts.tv_nsec,          \
		                             SW_FLOW_PM_SEND_EVENT,          \
		                             event_type, pm_event_id);       \
	} while (0)

/*                                       */
#define KBASE_TIMELINE_PM_HANDLE_EVENT(kbdev, event_type, pm_event_id) \
	do                                                                 \
	{                                                                  \
		struct timespec ts;                                            \
		getnstimeofday(&ts);                                           \
		trace_mali_timeline_pm_event(ts.tv_sec, ts.tv_nsec,            \
		                             SW_FLOW_PM_HANDLE_EVENT,          \
		                             event_type, pm_event_id);        \
	} while (0)


/*                                   */
#define KBASE_TIMELINE_JOB_START(kctx, js, _consumerof_atom_number)     \
	do                                                                  \
	{                                                                   \
		struct timespec ts;                                             \
		getnstimeofday(&ts);                                            \
		trace_mali_timeline_slot_atom(ts.tv_sec, ts.tv_nsec,            \
		                              HW_START_GPU_JOB_CHAIN_SW_APPROX, \
		                              (int)kctx->timeline.owner_tgid,   \
		                              js, _consumerof_atom_number);     \
	} while (0)

/*                                   */
#define KBASE_TIMELINE_JOB_STOP(kctx, js, _producerof_atom_number_completed) \
	do                                                                  \
	{                                                                   \
		struct timespec ts;                                             \
		getnstimeofday(&ts);                                            \
		trace_mali_timeline_slot_atom(ts.tv_sec, ts.tv_nsec,            \
		                              HW_STOP_GPU_JOB_CHAIN_SW_APPROX,  \
		                              (int)kctx->timeline.owner_tgid,   \
		                              js, _producerof_atom_number_completed);     \
	} while (0)

/*                                                                           
                 */
#define KBASE_TIMELINE_PM_CHECKTRANS(kbdev, trace_code) \
	do                                                                  \
	{                                                                   \
		struct timespec ts;                                             \
		getnstimeofday(&ts);                                            \
		trace_mali_timeline_pm_checktrans(ts.tv_sec, ts.tv_nsec,        \
		                                  trace_code, \
		                                  1);     \
	} while (0)

/*                                 */
#define KBASE_TIMELINE_CONTEXT_ACTIVE(kbdev, count)                           \
	do                                                                    \
	{                                                                     \
		struct timespec ts;                                           \
		getnstimeofday(&ts);                                          \
		trace_mali_timeline_context_active(ts.tv_sec, ts.tv_nsec,     \
		                                   count);                    \
	} while (0)


/*                                                                   */

/* 
                                               
  
                                                                      
 */
void kbase_timeline_job_slot_submit(struct kbase_device *kbdev, struct kbase_context *kctx,
                                    struct kbase_jd_atom *katom, int js);

/* 
                                            
  
                                                 
                                 
                                    
  
                                                                     
  
                                                                             
               
                                    
                                         
  
                                                                      
 */
void kbase_timeline_job_slot_done(struct kbase_device *kbdev, struct kbase_context *kctx,
                                  struct kbase_jd_atom *katom, int js,
                                  kbasep_js_atom_done_code done_code);


/*                            */
void kbase_timeline_pm_send_event(struct kbase_device *kbdev,
                                  enum kbase_timeline_pm_event event_sent);

/*                             */
void kbase_timeline_pm_check_handle_event(struct kbase_device *kbdev, enum kbase_timeline_pm_event event);

/*                                                                     */
void kbase_timeline_pm_handle_event(struct kbase_device *kbdev, enum kbase_timeline_pm_event event);

/*                          */
void kbase_timeline_pm_l2_transition_start(struct kbase_device *kbdev);

/*                         */
void kbase_timeline_pm_l2_transition_done(struct kbase_device *kbdev);

#else

#define KBASE_TIMELINE_ATOMS_IN_FLIGHT(kctx, count) CSTD_NOP()

#define KBASE_TIMELINE_ATOM_READY(kctx, atom_id) CSTD_NOP()

#define KBASE_TIMELINE_ATOMS_SUBMITTED(kctx, js, count) CSTD_NOP()

#define KBASE_TIMELINE_JOB_START_NEXT(kctx, js, count) CSTD_NOP()

#define KBASE_TIMELINE_JOB_START_HEAD(kctx, js, count) CSTD_NOP()

#define KBASE_TIMELINE_TRY_SOFT_STOP(kctx, js, count) CSTD_NOP()

#define KBASE_TIMELINE_GPU_POWER(kbdev, active) CSTD_NOP()

#define KBASE_TIMELINE_POWER_TILER(kbdev, bitmap) CSTD_NOP()

#define KBASE_TIMELINE_POWER_SHADER(kbdev, bitmap) CSTD_NOP()

#define KBASE_TIMELINE_POWER_L2(kbdev, active) CSTD_NOP()

#define KBASE_TIMELINE_POWERING_L2(kbdev) CSTD_NOP()

#define KBASE_TIMELINE_POWERED_L2(kbdev)  CSTD_NOP()

#define KBASE_TIMELINE_PM_SEND_EVENT(kbdev, event_type, pm_event_id) CSTD_NOP()

#define KBASE_TIMELINE_PM_HANDLE_EVENT(kbdev, event_type, pm_event_id) CSTD_NOP()

#define KBASE_TIMELINE_JOB_START(kctx, js, _consumerof_atom_number) CSTD_NOP()

#define KBASE_TIMELINE_JOB_STOP(kctx, js, _producerof_atom_number_completed) CSTD_NOP()

#define KBASE_TIMELINE_PM_CHECKTRANS(kbdev, trace_code) CSTD_NOP()

#define KBASE_TIMELINE_CONTEXT_ACTIVE(kbdev, count) CSTD_NOP()

static INLINE void kbase_timeline_job_slot_submit(struct kbase_device *kbdev, struct kbase_context *kctx,
                                    struct kbase_jd_atom *katom, int js)
{
	lockdep_assert_held(&kbdev->js_data.runpool_irq.lock);
}

static INLINE void kbase_timeline_job_slot_done(struct kbase_device *kbdev, struct kbase_context *kctx,
                                    struct kbase_jd_atom *katom, int js,
                                    kbasep_js_atom_done_code done_code)
{
	lockdep_assert_held(&kbdev->js_data.runpool_irq.lock);
}

static INLINE void kbase_timeline_pm_send_event(struct kbase_device *kbdev, enum kbase_timeline_pm_event event_sent)
{
}

static INLINE void kbase_timeline_pm_check_handle_event(struct kbase_device *kbdev, enum kbase_timeline_pm_event event)
{
}

static INLINE void kbase_timeline_pm_handle_event(struct kbase_device *kbdev, enum kbase_timeline_pm_event event)
{
}

static INLINE void kbase_timeline_pm_l2_transition_start(struct kbase_device *kbdev)
{

}

static INLINE void kbase_timeline_pm_l2_transition_done(struct kbase_device *kbdev)
{

}
#endif				/*                            */

#endif				/*                         */

