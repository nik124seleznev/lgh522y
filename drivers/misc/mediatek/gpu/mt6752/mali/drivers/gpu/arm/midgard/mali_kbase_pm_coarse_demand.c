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
#include <mali_kbase_pm.h>

#if KBASE_PM_EN
static u64 coarse_demand_get_core_mask(struct kbase_device *kbdev)
{
	if (kbdev->pm.active_count == 0)
		return 0;

	return kbdev->shader_present_bitmap;
}

static mali_bool coarse_demand_get_core_active(struct kbase_device *kbdev)
{
	if (kbdev->pm.active_count == 0)
		return MALI_FALSE;

	return MALI_TRUE;
}

static void coarse_demand_init(struct kbase_device *kbdev)
{
	CSTD_UNUSED(kbdev);
}

static void coarse_demand_term(struct kbase_device *kbdev)
{
	CSTD_UNUSED(kbdev);
}

/*                                                                        
  
                                                                                         
 */
const struct kbase_pm_policy kbase_pm_coarse_demand_policy_ops = {
	"coarse_demand",			/*      */
	coarse_demand_init,			/*      */
	coarse_demand_term,			/*      */
	coarse_demand_get_core_mask,		/*               */
	coarse_demand_get_core_active,		/*                 */
	0u,					/*       */
	KBASE_PM_POLICY_ID_COARSE_DEMAND,	/*    */
};

KBASE_EXPORT_TEST_API(kbase_pm_coarse_demand_policy_ops)
#endif  /*             */
