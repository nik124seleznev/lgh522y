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

#ifndef _KBASE_GPU_MEMORY_H
#define _KBASE_GPU_MEMORY_H

#include <mali_kbase.h>
#include <linux/debugfs.h>
#include <linux/seq_file.h>

/* 
                                             
 */
mali_error kbasep_gpu_memory_debugfs_init(struct kbase_device *kbdev);

/* 
                                            
 */
void kbasep_gpu_memory_debugfs_term(struct kbase_device *kbdev);

#endif  /*                   */
