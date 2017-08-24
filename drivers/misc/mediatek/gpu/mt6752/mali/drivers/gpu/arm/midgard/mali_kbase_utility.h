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





#ifndef _KBASE_UTILITY_H
#define _KBASE_UTILITY_H

#ifndef _KBASE_H_
#error "Don't include this file directly, use mali_kbase.h instead"
#endif

/*                                                                  
  
                                                     
                                               
  
                                                          
                                        
 */
mali_bool kbasep_list_member_of(const struct list_head *base, struct list_head *entry);

#endif				/*                  */
