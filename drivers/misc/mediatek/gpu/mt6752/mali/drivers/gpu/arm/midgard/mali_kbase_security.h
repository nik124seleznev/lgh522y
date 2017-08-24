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

#ifndef _KBASE_SECURITY_H_
#define _KBASE_SECURITY_H_

/*                */
#define KBASE_SEC_FLAG_NOAUDIT (0u << 0)	/*                                   */
#define KBASE_SEC_FLAG_AUDIT   (1u << 0)	/*                                          */
#define KBASE_SEC_FLAG_MASK    (KBASE_SEC_FLAG_AUDIT)	/*                             */

/*                                                                  */
enum kbase_security_capability {
	/*                                           */
	KBASE_SEC_INSTR_HW_COUNTERS_COLLECT = 1,
	KBASE_SEC_MODIFY_PRIORITY
	    /*                                       */
};

/* 
                                                                                                 
                                         
                                                    
                                                            
                                                                         
                                                                              
 */

mali_bool kbase_security_has_capability(struct kbase_context *kctx, enum kbase_security_capability cap, u32 flags);

#endif				/*                    */
