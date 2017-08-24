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





#ifndef _KBASE_UKU_H_
#define _KBASE_UKU_H_

#include "mali_uk.h"
#include <malisw/mali_malisw.h>
#include "mali_base_kernel.h"

/*                                                                                                  */
#if defined(CONFIG_MALI_ERROR_INJECT)
#define SUPPORT_MALI_ERROR_INJECT
#elif defined(MALI_ERROR_INJECT)
#if MALI_ERROR_INJECT
#define SUPPORT_MALI_ERROR_INJECT
#endif
#endif
#if defined(CONFIG_MALI_NO_MALI)
#define SUPPORT_MALI_NO_MALI
#elif defined(MALI_NO_MALI)
#if MALI_NO_MALI
#define SUPPORT_MALI_NO_MALI
#endif
#endif

#if defined(SUPPORT_MALI_NO_MALI) || defined(SUPPORT_MALI_ERROR_INJECT)
#include "mali_kbase_model_dummy.h"
#endif

#include "mali_kbase_gpuprops_types.h"

#define BASE_UK_VERSION_MAJOR 8
#define BASE_UK_VERSION_MINOR 0

struct kbase_uk_mem_alloc {
	union uk_header header;
	/*    */
	u64 va_pages;
	u64 commit_pages;
	u64 extent;
	/*        */
	u64 flags;
	/*     */
	u64 gpu_va;
	u16 va_alignment;
	u8  padding[6];
};

struct kbase_uk_mem_free {
	union uk_header header;
	/*    */
	mali_addr64 gpu_addr;
	/*     */
};

struct kbase_uk_mem_alias {
	union uk_header header;
	/*        */
	u64 flags;
	/*    */
	u64 stride;
	u64 nents;
	union kbase_pointer ai;
	/*     */
	u64         gpu_va;
	u64         va_pages;
};

struct kbase_uk_mem_import {
	union uk_header header;
	/*    */
	union kbase_pointer phandle;
	u32 type;
	u32 padding;
	/*        */
	u64         flags;
	/*     */
	mali_addr64 gpu_va;
	u64         va_pages;
};

struct kbase_uk_mem_flags_change {
	union uk_header header;
	/*    */
	mali_addr64 gpu_va;
	u64 flags;
	u64 mask;
};

struct kbase_uk_job_submit {
	union uk_header header;
	/*    */
	union kbase_pointer addr;
	u32 nr_atoms;
	u32 stride;		/*                                                   */
	/*     */
};

struct kbase_uk_post_term {
	union uk_header header;
};

struct kbase_uk_sync_now {
	union uk_header header;

	/*    */
	struct base_syncset sset;

	/*     */
};

struct kbase_uk_hwcnt_setup {
	union uk_header header;

	/*    */
	mali_addr64 dump_buffer;
	u32 jm_bm;
	u32 shader_bm;
	u32 tiler_bm;
	u32 l3_cache_bm;
	u32 mmu_l2_bm;
	u32 padding;
	/*     */
};

struct kbase_uk_hwcnt_dump {
	union uk_header header;
};

struct kbase_uk_hwcnt_clear {
	union uk_header header;
};

struct kbase_uk_fence_validate {
	union uk_header header;
	/*    */
	s32 fd;
	u32 padding;
	/*     */
};

struct kbase_uk_stream_create {
	union uk_header header;
	/*    */
	char name[32];
	/*     */
	s32 fd;
	u32 padding;
};

/* 
                                                                 
                                                                        
                                                        
                                                                         
                                                                  
 */
#define BASE_CPU_PROPERTY_FLAG_LITTLE_ENDIAN F_BIT_0
struct base_cpu_id_props {
	/* 
          
  */
	u32 id;

	/* 
                   
  */
	u16 part;
	/* 
                                       
  */
	u8 implementer;

	/* 
               
  */
	u8 variant;
	/* 
                    
  */
	u8 arch;

	/* 
                
  */
	u8 rev;

	/* 
                                          
                                                  
  */
	u8 valid;

	u8 padding[1];
};

/* 
                                                                 
                                                                        
                                                        
                                                                         
                                                                  
 */
struct base_cpu_props {
	u32 nr_cores;        /*                       */

	/* 
                                                            
                                                    
  */
	u32 cpu_page_size_log2;

	/* 
                                                                          
                                                               
  */
	u32 cpu_l1_dcache_line_size_log2;

	/* 
                                                                     
                                       
   
                                                              
                                                                       
                       
  */
	u32 cpu_l1_dcache_size;

	/* 
                                  
   
                                                                          
                              
  */
	u32 cpu_flags;

	/* 
                               
                                                                          
                                                                          
  */
	u32 max_cpu_clock_speed_mhz;

	/* 
                                  
   
                                                                      
                                                                          
                                                                        
                                
   
                                                             
                                                                   
  */
	u64 available_memory_size;

	/* 
                        
  */
	struct base_cpu_id_props cpu_id;

	u32 padding;
};

/* 
                                                                 
                                                                        
                                                        
                                                                         
                                                                  
 */
struct kbase_uk_cpuprops {
	union uk_header header;

	/*    */
	struct base_cpu_props props;
	/*     */
};

struct kbase_uk_gpuprops {
	union uk_header header;

	/*    */
	struct mali_base_gpu_props props;
	/*     */
};

struct kbase_uk_mem_query {
	union uk_header header;
	/*    */
	mali_addr64 gpu_addr;
#define KBASE_MEM_QUERY_COMMIT_SIZE  1
#define KBASE_MEM_QUERY_VA_SIZE      2
#define KBASE_MEM_QUERY_FLAGS        3
	u64         query;
	/*     */
	u64         value;
};
	
struct kbase_uk_mem_commit {
	union uk_header header;
	/*    */
	mali_addr64 gpu_addr;
	u64         pages;
	/*     */
	u32 result_subcode;
	u32 padding;
};

struct kbase_uk_find_cpu_offset {
	union uk_header header;
	/*    */
	mali_addr64 gpu_addr;
	u64 cpu_addr;
	u64 size;
	/*     */
	mali_size64 offset;
};

#define KBASE_GET_VERSION_BUFFER_SIZE 64
struct kbase_uk_get_ddk_version {
	union uk_header header;
	/*     */
	char version_buffer[KBASE_GET_VERSION_BUFFER_SIZE];
	u32 version_string_size;
	u32 padding;
};

struct kbase_uk_disjoint_query {
	union uk_header header;
	/*     */
	u32 counter;
	u32 padding;
};

struct kbase_uk_set_flags {
	union uk_header header;
	/*    */
	u32 create_flags;
	u32 padding;
};

#if MALI_UNIT_TEST
#define TEST_ADDR_COUNT 4
#define KBASE_TEST_BUFFER_SIZE 128
typedef struct kbase_exported_test_data {
	mali_addr64 test_addr[TEST_ADDR_COUNT];		/*                  */
	u32 test_addr_pages[TEST_ADDR_COUNT];		/*                         */
	union kbase_pointer kctx;				/*                                    */
	union kbase_pointer mm;				/*                                    */
	u8 buffer1[KBASE_TEST_BUFFER_SIZE];   /*                                */
	u8 buffer2[KBASE_TEST_BUFFER_SIZE];   /*                                */
} kbase_exported_test_data;

struct kbase_uk_set_test_data {
	union uk_header header;
	/*    */
	struct kbase_exported_test_data test_data;
};

#endif				/*                */

#ifdef SUPPORT_MALI_ERROR_INJECT
struct kbase_uk_error_params {
	union uk_header header;
	/*    */
	struct kbase_error_params params;
};
#endif				/*                           */

#ifdef SUPPORT_MALI_NO_MALI
struct kbase_uk_model_control_params {
	union uk_header header;
	/*    */
	struct kbase_model_control_params params;
};
#endif				/*                      */

#define KBASE_MAXIMUM_EXT_RESOURCES       255

struct kbase_uk_ext_buff_kds_data {
	union uk_header header;
	union kbase_pointer external_resource;
	union kbase_pointer file_descriptor;
	u32 num_res;		/*                                        */
	u32 padding;
};

struct kbase_uk_keep_gpu_powered {
	union uk_header header;
	u32       enabled;
	u32       padding;
};

struct kbase_uk_profiling_controls {
	union uk_header header;
	u32 profiling_controls[FBDUMP_CONTROL_MAX];
};

struct kbase_uk_debugfs_mem_profile_add {
	union uk_header header;
	u32 len;
	union kbase_pointer buf;
};

enum kbase_uk_function_id {
	KBASE_FUNC_MEM_ALLOC = (UK_FUNC_ID + 0),
	KBASE_FUNC_MEM_IMPORT,
	KBASE_FUNC_MEM_COMMIT,
	KBASE_FUNC_MEM_QUERY,
	KBASE_FUNC_MEM_FREE,
	KBASE_FUNC_MEM_FLAGS_CHANGE,
	KBASE_FUNC_MEM_ALIAS,

#ifdef BASE_LEGACY_UK6_SUPPORT
	KBASE_FUNC_JOB_SUBMIT_UK6 = (UK_FUNC_ID + 7),
#endif /*                         */

	KBASE_FUNC_SYNC  = (UK_FUNC_ID + 8),

	KBASE_FUNC_POST_TERM,

	KBASE_FUNC_HWCNT_SETUP,
	KBASE_FUNC_HWCNT_DUMP,
	KBASE_FUNC_HWCNT_CLEAR,

	KBASE_FUNC_CPU_PROPS_REG_DUMP_OBSOLETE,
	KBASE_FUNC_GPU_PROPS_REG_DUMP,

	KBASE_FUNC_FIND_CPU_OFFSET,

	KBASE_FUNC_GET_VERSION,
	KBASE_FUNC_EXT_BUFFER_LOCK,
	KBASE_FUNC_SET_FLAGS,

	KBASE_FUNC_SET_TEST_DATA,
	KBASE_FUNC_INJECT_ERROR,
	KBASE_FUNC_MODEL_CONTROL,

	KBASE_FUNC_KEEP_GPU_POWERED,

	KBASE_FUNC_FENCE_VALIDATE,
	KBASE_FUNC_STREAM_CREATE,
	KBASE_FUNC_GET_PROFILING_CONTROLS,
	KBASE_FUNC_SET_PROFILING_CONTROLS, /*                            
                                             
                                     */

	KBASE_FUNC_DEBUGFS_MEM_PROFILE_ADD,
	KBASE_FUNC_JOB_SUBMIT = (UK_FUNC_ID + 28),
	KBASE_FUNC_DISJOINT_QUERY

};

#endif				/*               */
