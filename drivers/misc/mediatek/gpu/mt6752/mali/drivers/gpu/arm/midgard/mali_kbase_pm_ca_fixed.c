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
static void fixed_init(struct kbase_device *kbdev)
{
	kbdev->pm.ca_in_transition = MALI_FALSE;
}

static void fixed_term(struct kbase_device *kbdev)
{
	CSTD_UNUSED(kbdev);
}

static u64 fixed_get_core_mask(struct kbase_device *kbdev)
{
	return kbdev->shader_present_bitmap;
}

static void fixed_update_core_status(struct kbase_device *kbdev, u64 cores_ready, u64 cores_transitioning)
{
	CSTD_UNUSED(kbdev);
	CSTD_UNUSED(cores_ready);
	CSTD_UNUSED(cores_transitioning);
}

/*                                                                       
  
                                                                                        
 */
const struct kbase_pm_ca_policy kbase_pm_ca_fixed_policy_ops = {
	"fixed",			/*      */
	fixed_init,			/*      */
	fixed_term,			/*      */
	fixed_get_core_mask,		/*               */
	fixed_update_core_status,	/*                    */
	0u,				/*       */
	KBASE_PM_CA_POLICY_ID_FIXED,	/*    */
};

KBASE_EXPORT_TEST_API(kbase_pm_ca_fixed_policy_ops)
#endif  /*             */
