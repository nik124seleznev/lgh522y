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



#include <linux/ioport.h>
#include <mali_kbase.h>
#include <mali_kbase_defs.h>
#include <mali_kbase_config.h>

#include "mali_kbase_cpu_vexpress.h"

/*                                                                                 
                                                                     
                                               
 */
#define KBASE_VE_JS_SCHEDULING_TICK_NS_DEBUG    15000000u      /*                                                                                          */
#define KBASE_VE_JS_SOFT_STOP_TICKS_DEBUG       1	/*                                              */
#define KBASE_VE_JS_SOFT_STOP_TICKS_CL_DEBUG    1	/*                                                 */
#define KBASE_VE_JS_HARD_STOP_TICKS_SS_DEBUG    333	/*                     */
#define KBASE_VE_JS_HARD_STOP_TICKS_SS_8401_DEBUG 2000	/*                                                                                                                 */
#define KBASE_VE_JS_HARD_STOP_TICKS_CL_DEBUG    166	/*                       */
#define KBASE_VE_JS_HARD_STOP_TICKS_NSS_DEBUG   100000	/*                                     */
#define KBASE_VE_JS_RESET_TICKS_SS_DEBUG        500	/*                                                                                                    */
#define KBASE_VE_JS_RESET_TICKS_SS_8401_DEBUG   3000	/*                                            */
#define KBASE_VE_JS_RESET_TICKS_CL_DEBUG        500	/*                          */
#define KBASE_VE_JS_RESET_TICKS_NSS_DEBUG       100166	/*                            */

#define KBASE_VE_JS_SCHEDULING_TICK_NS          1250000000u	/*       */
#define KBASE_VE_JS_SOFT_STOP_TICKS             2	/*                             */
#define KBASE_VE_JS_SOFT_STOP_TICKS_CL          1	/*                                 */
#define KBASE_VE_JS_HARD_STOP_TICKS_SS          4	/*                     */
#define KBASE_VE_JS_HARD_STOP_TICKS_SS_8401     24	/*                                                                                                                 */
#define KBASE_VE_JS_HARD_STOP_TICKS_CL          2	/*                       */
#define KBASE_VE_JS_HARD_STOP_TICKS_NSS         1200	/*                            */
#define KBASE_VE_JS_RESET_TICKS_SS              6	/*                           */
#define KBASE_VE_JS_RESET_TICKS_SS_8401         36	/*                                                                                                                     */
#define KBASE_VE_JS_RESET_TICKS_CL              3	/*                            */
#define KBASE_VE_JS_RESET_TICKS_NSS             1201	/*                            */

#define KBASE_VE_JS_RESET_TIMEOUT_MS            3000	/*                                 */
#define KBASE_VE_JS_CTX_TIMESLICE_NS            1000000	/*                                                                                               */
#define KBASE_VE_POWER_MANAGEMENT_CALLBACKS     ((uintptr_t)&pm_callbacks)
#define KBASE_VE_CPU_SPEED_FUNC                 ((uintptr_t)&kbase_get_vexpress_cpu_clock_speed)

#define HARD_RESET_AT_POWER_OFF 0

#ifndef CONFIG_OF
static kbase_io_resources io_resources = {
	.job_irq_number = 68,
	.mmu_irq_number = 69,
	.gpu_irq_number = 70,
	.io_memory_region = {
	.start = 0xFC010000,
	.end = 0xFC010000 + (4096 * 4) - 1
	}

};
#endif /*           */

static int pm_callback_power_on(struct kbase_device *kbdev)
{
	/*                                                                                                             */
	return 1;
}

static void pm_callback_power_off(struct kbase_device *kbdev)
{
#if HARD_RESET_AT_POWER_OFF
	/*                                                                      
                                                         
                                                                                  
                                                                     
                                                                                
                    
  */
	KBASE_TRACE_ADD(kbdev, CORE_GPU_HARD_RESET, NULL, NULL, 0u, 0);
	kbase_os_reg_write(kbdev, GPU_CONTROL_REG(GPU_COMMAND), GPU_COMMAND_HARD_RESET);
#endif
}

static struct kbase_pm_callback_conf pm_callbacks = {
	.power_on_callback = pm_callback_power_on,
	.power_off_callback = pm_callback_power_off,
	.power_suspend_callback  = NULL,
	.power_resume_callback = NULL
};

/*                                                                                  */
static struct kbase_attribute config_attributes[] = {
#ifdef CONFIG_MALI_DEBUG
/*                                                                              */
	{
	 KBASE_CONFIG_ATTR_JS_SCHEDULING_TICK_NS,
	 KBASE_VE_JS_SCHEDULING_TICK_NS_DEBUG},

	{
	 KBASE_CONFIG_ATTR_JS_SOFT_STOP_TICKS,
	 KBASE_VE_JS_SOFT_STOP_TICKS_DEBUG},

	{
	 KBASE_CONFIG_ATTR_JS_SOFT_STOP_TICKS_CL,
	 KBASE_VE_JS_SOFT_STOP_TICKS_CL_DEBUG},

	{
	 KBASE_CONFIG_ATTR_JS_HARD_STOP_TICKS_SS,
	 KBASE_VE_JS_HARD_STOP_TICKS_SS_DEBUG},

	{
	 KBASE_CONFIG_ATTR_JS_HARD_STOP_TICKS_CL,
	 KBASE_VE_JS_HARD_STOP_TICKS_CL_DEBUG},

	{
	 KBASE_CONFIG_ATTR_JS_HARD_STOP_TICKS_NSS,
	 KBASE_VE_JS_HARD_STOP_TICKS_NSS_DEBUG},

	{
	 KBASE_CONFIG_ATTR_JS_RESET_TICKS_SS,
	 KBASE_VE_JS_RESET_TICKS_SS_DEBUG},

	{
	 KBASE_CONFIG_ATTR_JS_RESET_TICKS_CL,
	 KBASE_VE_JS_RESET_TICKS_CL_DEBUG},

	{
	 KBASE_CONFIG_ATTR_JS_RESET_TICKS_NSS,
	 KBASE_VE_JS_RESET_TICKS_NSS_DEBUG},
#else				/*                   */
/*                                                                 */
	{
	 KBASE_CONFIG_ATTR_JS_SCHEDULING_TICK_NS,
	 KBASE_VE_JS_SCHEDULING_TICK_NS},

	{
	 KBASE_CONFIG_ATTR_JS_SOFT_STOP_TICKS,
	 KBASE_VE_JS_SOFT_STOP_TICKS},

	{
	 KBASE_CONFIG_ATTR_JS_SOFT_STOP_TICKS_CL,
	 KBASE_VE_JS_SOFT_STOP_TICKS_CL},

	{
	 KBASE_CONFIG_ATTR_JS_HARD_STOP_TICKS_SS,
	 KBASE_VE_JS_HARD_STOP_TICKS_SS},

	{
	 KBASE_CONFIG_ATTR_JS_HARD_STOP_TICKS_CL,
	 KBASE_VE_JS_HARD_STOP_TICKS_CL},

	{
	 KBASE_CONFIG_ATTR_JS_HARD_STOP_TICKS_NSS,
	 KBASE_VE_JS_HARD_STOP_TICKS_NSS},

	{
	 KBASE_CONFIG_ATTR_JS_RESET_TICKS_SS,
	 KBASE_VE_JS_RESET_TICKS_SS},

	{
	 KBASE_CONFIG_ATTR_JS_RESET_TICKS_CL,
	 KBASE_VE_JS_RESET_TICKS_CL},

	{
	 KBASE_CONFIG_ATTR_JS_RESET_TICKS_NSS,
	 KBASE_VE_JS_RESET_TICKS_NSS},
#endif				/*                   */
	{
	 KBASE_CONFIG_ATTR_JS_RESET_TIMEOUT_MS,
	 KBASE_VE_JS_RESET_TIMEOUT_MS},

	{
	 KBASE_CONFIG_ATTR_JS_CTX_TIMESLICE_NS,
	 KBASE_VE_JS_CTX_TIMESLICE_NS},

	{
	 KBASE_CONFIG_ATTR_POWER_MANAGEMENT_CALLBACKS,
	 KBASE_VE_POWER_MANAGEMENT_CALLBACKS},

	{
	 KBASE_CONFIG_ATTR_CPU_SPEED_FUNC,
	 KBASE_VE_CPU_SPEED_FUNC},

	{
	 KBASE_CONFIG_ATTR_END,
	 0}
};

/*                                                                    
                                                
                                     
 */
struct kbase_attribute config_attributes_hw_issue_8408[] = {
#ifdef CONFIG_MALI_DEBUG
/*                                                                              */
	{
	 KBASE_CONFIG_ATTR_JS_SCHEDULING_TICK_NS,
	 KBASE_VE_JS_SCHEDULING_TICK_NS_DEBUG},

	{
	 KBASE_CONFIG_ATTR_JS_SOFT_STOP_TICKS,
	 KBASE_VE_JS_SOFT_STOP_TICKS_DEBUG},

	{
	 KBASE_CONFIG_ATTR_JS_HARD_STOP_TICKS_SS,
	 KBASE_VE_JS_HARD_STOP_TICKS_SS_8401_DEBUG},

	{
	 KBASE_CONFIG_ATTR_JS_HARD_STOP_TICKS_NSS,
	 KBASE_VE_JS_HARD_STOP_TICKS_NSS_DEBUG},

	{
	 KBASE_CONFIG_ATTR_JS_RESET_TICKS_SS,
	 KBASE_VE_JS_RESET_TICKS_SS_8401_DEBUG},

	{
	 KBASE_CONFIG_ATTR_JS_RESET_TICKS_NSS,
	 KBASE_VE_JS_RESET_TICKS_NSS_DEBUG},
#else				/*                   */
/*                                                                 */
	{
	 KBASE_CONFIG_ATTR_JS_SCHEDULING_TICK_NS,
	 KBASE_VE_JS_SCHEDULING_TICK_NS},

	{
	 KBASE_CONFIG_ATTR_JS_SOFT_STOP_TICKS,
	 KBASE_VE_JS_SOFT_STOP_TICKS},

	{
	 KBASE_CONFIG_ATTR_JS_HARD_STOP_TICKS_SS,
	 KBASE_VE_JS_HARD_STOP_TICKS_SS_8401},

	{
	 KBASE_CONFIG_ATTR_JS_HARD_STOP_TICKS_NSS,
	 KBASE_VE_JS_HARD_STOP_TICKS_NSS},

	{
	 KBASE_CONFIG_ATTR_JS_RESET_TICKS_SS,
	 KBASE_VE_JS_RESET_TICKS_SS_8401},

	{
	 KBASE_CONFIG_ATTR_JS_RESET_TICKS_NSS,
	 KBASE_VE_JS_RESET_TICKS_NSS},
#endif				/*                   */
	{
	 KBASE_CONFIG_ATTR_JS_RESET_TIMEOUT_MS,
	 KBASE_VE_JS_RESET_TIMEOUT_MS},

	{
	 KBASE_CONFIG_ATTR_JS_CTX_TIMESLICE_NS,
	 KBASE_VE_JS_CTX_TIMESLICE_NS},

	{
	 KBASE_CONFIG_ATTR_POWER_MANAGEMENT_CALLBACKS,
	 KBASE_VE_POWER_MANAGEMENT_CALLBACKS},

	{
	 KBASE_CONFIG_ATTR_CPU_SPEED_FUNC,
	 KBASE_VE_CPU_SPEED_FUNC},

	{
	 KBASE_CONFIG_ATTR_END,
	 0}
};

static struct kbase_platform_config virtex7_platform_config = {
	.attributes = config_attributes,
#ifndef CONFIG_OF
	.io_resources = &io_resources
#endif
};

struct kbase_platform_config *kbase_get_platform_config(void)
{
	return &virtex7_platform_config;
}

int kbase_platform_early_init(void)
{
	/*                              */
	return 0;
}

