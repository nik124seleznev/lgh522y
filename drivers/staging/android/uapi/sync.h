/*
 * Copyright (C) 2012 Google, Inc.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef _UAPI_LINUX_SYNC_H
#define _UAPI_LINUX_SYNC_H

#include <linux/ioctl.h>
#include <linux/types.h>

/* 
                                                      
                                        
                           
                                                       
 */
struct sync_merge_data {
	__s32	fd2; /*                    */
	char	name[32]; /*                   */
	__s32	fence; /*                           */
};

/* 
                                                     
                                                          
                                           
                                                      
                                                               
                                                           
                                          
 */
struct sync_pt_info {
	__u32	len;
	char	obj_name[32];
	char	driver_name[32];
	__s32	status;
	__u64	timestamp_ns;

	__u8	driver_data[0];
};

/* 
                                                                    
                                                                   
                                                                 
                      
                       
                                                          
                                                                 
 */
struct sync_fence_info_data {
	__u32	len;
	char	name[32];
	__s32	status;

	__u8	pt_info[0];
};

#define SYNC_IOC_MAGIC		'>'

/* 
                                                  
  
                                                                 
 */
#define SYNC_IOC_WAIT		_IOW(SYNC_IOC_MAGIC, 0, __s32)

/* 
                                         
  
                                                                            
                                                                            
                                          
 */
#define SYNC_IOC_MERGE		_IOWR(SYNC_IOC_MAGIC, 1, struct sync_merge_data)

/* 
                                                                 
  
                                                                              
                                                                          
                                                                          
           
  
                                                                               
                                                                      
 */
#define SYNC_IOC_FENCE_INFO	_IOWR(SYNC_IOC_MAGIC, 2,\
	struct sync_fence_info_data)

#endif /*                    */
