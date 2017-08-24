/*
 * Copyright (C) 2010, 2012-2013 ARM Limited. All rights reserved.
 * 
 * This program is free software and is provided to you under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation, and any use by you of this program is subject to the terms of such GNU licence.
 * 
 * A copy of the licence is included with the program, and can also be obtained from Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

/* 
                       
                                                                    
 */

#ifndef __UMP_UK_TYPES_H__
#define __UMP_UK_TYPES_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*                                  */
#define MAKE_VERSION_ID(x) (((x) << 16UL) | (x))
#define IS_VERSION_ID(x) (((x) & 0xFFFF) == (((x) >> 16UL) & 0xFFFF))
#define GET_VERSION(x) (((x) >> 16UL) & 0xFFFF)
#define IS_API_MATCH(x, y) (IS_VERSION_ID((x)) && IS_VERSION_ID((y)) && (GET_VERSION((x)) == GET_VERSION((y))))

/* 
                      
                                          
                                                                 
                                                       
                                                 
 */
#define UMP_IOCTL_API_VERSION MAKE_VERSION_ID(2)

typedef enum
{
	_UMP_IOC_QUERY_API_VERSION = 1,
	_UMP_IOC_ALLOCATE,
	_UMP_IOC_RELEASE,
	_UMP_IOC_SIZE_GET,
	_UMP_IOC_MAP_MEM,    /*                   */
	_UMP_IOC_UNMAP_MEM,  /*                   */
	_UMP_IOC_MSYNC,
	_UMP_IOC_CACHE_OPERATIONS_CONTROL,
	_UMP_IOC_SWITCH_HW_USAGE,
	_UMP_IOC_LOCK,
	_UMP_IOC_UNLOCK,
}_ump_uk_functions;

typedef enum
{
	UMP_REF_DRV_UK_CONSTRAINT_NONE = 0,
	UMP_REF_DRV_UK_CONSTRAINT_PHYSICALLY_LINEAR = 1,
	UMP_REF_DRV_UK_CONSTRAINT_USE_CACHE = 4,
} ump_uk_alloc_constraints;

typedef enum
{
	_UMP_UK_MSYNC_CLEAN = 0,
	_UMP_UK_MSYNC_CLEAN_AND_INVALIDATE = 1,
	_UMP_UK_MSYNC_INVALIDATE = 2,
	_UMP_UK_MSYNC_FLUSH_L1   = 3,
	_UMP_UK_MSYNC_READOUT_CACHE_ENABLED = 128,
} ump_uk_msync_op;

typedef enum
{
	_UMP_UK_CACHE_OP_START = 0,
	_UMP_UK_CACHE_OP_FINISH  = 1,
} ump_uk_cache_op_control;

typedef enum
{
	_UMP_UK_READ = 1,
	_UMP_UK_READ_WRITE = 3,
} ump_uk_lock_usage;

typedef enum
{
	_UMP_UK_USED_BY_CPU = 0,
	_UMP_UK_USED_BY_MALI = 1,
	_UMP_UK_USED_BY_UNKNOWN_DEVICE= 100,
} ump_uk_user;

/* 
                                                                   
 */
typedef struct _ump_uk_api_version_s
{
	void *ctx;      /*                                                    */
	u32 version;    /*                                                                                    */
	u32 compatible; /*                                                        */
} _ump_uk_api_version_s;

/* 
                                                                      
 */
typedef struct _ump_uk_allocate_s
{
	void *ctx;                              /*                                                    */
	u32 secure_id;                          /*                                      */
	u32 size;                               /*                                                                  */
	ump_uk_alloc_constraints constraints;   /*                              */
} _ump_uk_allocate_s;

/* 
                                            
 */
typedef struct _ump_uk_size_get_s
{
	void *ctx;                              /*                                                    */
	u32 secure_id;                          /*               */
	u32 size;                               /*                         */
} _ump_uk_size_get_s;

/* 
                               
 */
typedef struct _ump_uk_release_s
{
	void *ctx;                              /*                                                    */
	u32 secure_id;                          /*               */
} _ump_uk_release_s;

typedef struct _ump_uk_map_mem_s
{
	void *ctx;                      /*                                                    */
	void *mapping;                  /*                                                            */
	void *phys_addr;                /*                         */
	unsigned long size;             /*             */
	u32 secure_id;                  /*                                       */
	void * _ukk_private;            /*                                                                                    */
	u32 cookie;
	u32 is_cached;            /*                                    */
} _ump_uk_map_mem_s;

typedef struct _ump_uk_unmap_mem_s
{
	void *ctx;            /*                                                    */
	void *mapping;
	u32 size;
	void * _ukk_private;
	u32 cookie;
} _ump_uk_unmap_mem_s;

typedef struct _ump_uk_msync_s
{
	void *ctx;            /*                                                    */
	void *mapping;        /*                     */
	void *address;        /*                         */
	u32 size;             /*                      */
	ump_uk_msync_op op;   /*                        */
	u32 cookie;           /*                                                                     */
	u32 secure_id;        /*                                                 */
	u32 is_cached;        /*                                 */
} _ump_uk_msync_s;

typedef struct _ump_uk_cache_operations_control_s
{
	void *ctx;                   /*                                                    */
	ump_uk_cache_op_control op;  /*                                    */
} _ump_uk_cache_operations_control_s;


typedef struct _ump_uk_switch_hw_usage_s
{
	void *ctx;            /*                                                    */
	u32 secure_id;        /*                                                 */
	ump_uk_user new_user;         /*                                                                     */

} _ump_uk_switch_hw_usage_s;

typedef struct _ump_uk_lock_s
{
	void *ctx;            /*                                                    */
	u32 secure_id;        /*                                                 */
	ump_uk_lock_usage lock_usage;
} _ump_uk_lock_s;

typedef struct _ump_uk_unlock_s
{
	void *ctx;            /*                                                    */
	u32 secure_id;        /*                                                 */
} _ump_uk_unlock_s;

#ifdef __cplusplus
}
#endif

#endif /*                    */
