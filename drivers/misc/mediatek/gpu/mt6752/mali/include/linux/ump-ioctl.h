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





#ifndef _UMP_IOCTL_H_
#define _UMP_IOCTL_H

#include <linux/ump-common.h>

/*
                                                                                                                           
                                                                                                                                          
 */

/*                                              */
typedef union ump_pointer
{
	void * value; /*                                           */
	uint32_t compat_value; /*                                                                       */
	uint64_t sizer; /*                                                   */
} ump_pointer;

/* 
                          
                                    
 */
typedef struct ump_k_allocate
{
	uint64_t size; /*                                  */
	ump_secure_id secure_id; /*                                            */
	ump_alloc_flags alloc_flags; /*                                     */
} ump_k_allocate;

/* 
                          
                                    
 */
typedef struct ump_k_sizequery
{
	uint64_t size; /*                            */
	ump_secure_id secure_id; /*                                              */
	uint32_t padding; /*              */
} ump_k_sizequery;

/* 
                                     
                                         
 */
typedef struct ump_k_msync
{
	ump_pointer mapped_ptr; /*                                             */
	ump_secure_id secure_id; /*                                                       */
	ump_cpu_msync_op cache_operation; /*                                   */
	uint64_t size; /*                                                  */
} ump_k_msync;

/* 
                             
                                  
 */
typedef struct ump_k_retain
{
	ump_secure_id secure_id; /*                                                  */
	uint32_t padding; /*              */
} ump_k_retain;

/* 
                              
                                   
 */
typedef struct ump_k_release
{
	ump_secure_id secure_id; /*                                                   */
	uint32_t padding; /*              */
} ump_k_release;

typedef struct ump_k_import
{
	ump_pointer phandle;                /*                                     */
	uint32_t type;                           /*                                  */
	ump_alloc_flags alloc_flags;        /*                                                */
	ump_secure_id secure_id;            /*                                                 */
	uint32_t padding;                        /*              */
} ump_k_import;

/* 
                                
                                                 
  
                      
 */
typedef struct ump_k_allocation_flags
{
	ump_secure_id secure_id; /*                                           */
	ump_alloc_flags alloc_flags; /*                                      */
} ump_k_allocation_flags;

#define UMP_CALL_MAX_SIZE 512
/*
                    
 */

/*                         */

#define UMP_IOC_MAGIC  '~'

#define UMP_FUNC_ALLOCATE _IOWR(UMP_IOC_MAGIC,  1, ump_k_allocate)
#define UMP_FUNC_SIZEQUERY _IOWR(UMP_IOC_MAGIC,  2, ump_k_sizequery)
#define UMP_FUNC_MSYNC _IOWR(UMP_IOC_MAGIC,  3, ump_k_msync)
#define UMP_FUNC_RETAIN _IOW(UMP_IOC_MAGIC,  4, ump_k_retain)
#define UMP_FUNC_RELEASE _IOW(UMP_IOC_MAGIC,  5, ump_k_release)
#define UMP_FUNC_ALLOCATION_FLAGS_GET _IOWR(UMP_IOC_MAGIC,  6, ump_k_allocation_flags)
#define UMP_FUNC_IMPORT _IOWR(UMP_IOC_MAGIC, 7, ump_k_import)

/*                           */
#define UMP_IOC_MAXNR 7

/*                                             */
#define UMP_LINUX_ID_BITS 15
#define UMP_LINUX_ID_MASK ((1ULL << UMP_LINUX_ID_BITS) - 1ULL)

/*                                                                              */
#define UMP_LINUX_OFFSET_BITS_64 37
#define UMP_LINUX_OFFSET_MASK_64 ((1ULL << UMP_LINUX_OFFSET_BITS_64)-1)
/*                                                             */
#define UMP_LINUX_OFFSET_BITS_32 17
#define UMP_LINUX_OFFSET_MASK_32 ((1ULL << UMP_LINUX_OFFSET_BITS_32)-1)

#if __SIZEOF_LONG__ == 8
#define UMP_LINUX_OFFSET_BITS UMP_LINUX_OFFSET_BITS_64
#define UMP_LINUX_OFFSET_MASK UMP_LINUX_OFFSET_MASK_64
#else
#define UMP_LINUX_OFFSET_BITS UMP_LINUX_OFFSET_BITS_32
#define UMP_LINUX_OFFSET_MASK UMP_LINUX_OFFSET_MASK_32
#endif

#endif /*               */
