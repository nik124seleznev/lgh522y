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

#ifndef _KBASE_JD_DEBUGFS_H
#define _KBASE_JD_DEBUGFS_H

#include <linux/debugfs.h>

#include <mali_kbase.h>

/* 
                                       
  
                                                                          
                              
  
                                           
 */
int kbasep_jd_debugfs_init(struct kbase_device *kbdev);

/* 
                                                          
  
                                                                           
                             
  
                                           
 */
void kbasep_jd_debugfs_term(struct kbase_device *kbdev);

/* 
                                     
  
                                           
  
                                          
 */
int kbasep_jd_debugfs_ctx_add(struct kbase_context *kctx);

/* 
                                      
  
                                          
 */
void kbasep_jd_debugfs_ctx_remove(struct kbase_context *kctx);

#endif  /*                   */
