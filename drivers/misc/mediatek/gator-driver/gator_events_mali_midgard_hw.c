/**
 * Copyright (C) ARM Limited 2012-2014. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include "gator.h"

#include <linux/module.h>
#include <linux/time.h>
#include <linux/math64.h>
#include <linux/slab.h>
#include <linux/io.h>

/*                           */
#if defined(MALI_SIMPLE_API)
/*                                                                 */
#include "mali/mali_kbase_gator_api.h"
#elif defined(MALI_DIR_MIDGARD)
/*                                                                 */
#include "mali_linux_trace.h"
#include "mali_kbase.h"
#include "mali_kbase_mem_linux.h"
#else
/*                                                              */
#include "linux/mali_linux_trace.h"
#include "kbase/src/common/mali_kbase.h"
#include "kbase/src/linux/mali_kbase_mem_linux.h"
#endif

/*                                                            */
#ifndef MALI_DDK_GATOR_API_VERSION
#define MALI_DDK_GATOR_API_VERSION 1
#endif

#if (MALI_DDK_GATOR_API_VERSION != 1) && (MALI_DDK_GATOR_API_VERSION != 2) && (MALI_DDK_GATOR_API_VERSION != 3)
#error MALI_DDK_GATOR_API_VERSION is invalid (must be 1 for r1/r2 DDK, or 2 for r3/r4 DDK, or 3 for r5 and later DDK).
#endif

#include "gator_events_mali_common.h"

/*
               
 */
#if MALI_DDK_GATOR_API_VERSION == 3
static uint32_t (*kbase_gator_instr_hwcnt_dump_irq_symbol)(struct kbase_gator_hwcnt_handles *);
static uint32_t (*kbase_gator_instr_hwcnt_dump_complete_symbol)(struct kbase_gator_hwcnt_handles *, uint32_t *const);
static struct kbase_gator_hwcnt_handles *(*kbase_gator_hwcnt_init_symbol)(struct kbase_gator_hwcnt_info *);
static void (*kbase_gator_hwcnt_term_symbol)(struct kbase_gator_hwcnt_info *, struct kbase_gator_hwcnt_handles *);

#else
static struct kbase_device *(*kbase_find_device_symbol)(int);
static struct kbase_context *(*kbase_create_context_symbol)(struct kbase_device *);
static void (*kbase_destroy_context_symbol)(struct kbase_context *);

#if MALI_DDK_GATOR_API_VERSION == 1
static void *(*kbase_va_alloc_symbol)(struct kbase_context *, u32);
static void (*kbase_va_free_symbol)(struct kbase_context *, void *);
#elif MALI_DDK_GATOR_API_VERSION == 2
static void *(*kbase_va_alloc_symbol)(struct kbase_context *, u32, struct kbase_hwc_dma_mapping *);
static void (*kbase_va_free_symbol)(struct kbase_context *, struct kbase_hwc_dma_mapping *);
#endif

static mali_error (*kbase_instr_hwcnt_enable_symbol)(struct kbase_context *, struct kbase_uk_hwcnt_setup *);
static mali_error (*kbase_instr_hwcnt_disable_symbol)(struct kbase_context *);
static mali_error (*kbase_instr_hwcnt_clear_symbol)(struct kbase_context *);
static mali_error (*kbase_instr_hwcnt_dump_irq_symbol)(struct kbase_context *);
static mali_bool (*kbase_instr_hwcnt_dump_complete_symbol)(struct kbase_context *, mali_bool *);

static long shader_present_low;
#endif

/*                                    
  
                                                                
                                                                    
                                                                   
                                                                   
                                                             
                                                                  
                                                                      
                                                                 
                                                                  
                                                                     
                                                              
 */
static const int READ_INTERVAL_NSEC = 950000;

#if GATOR_TEST
#include "gator_events_mali_midgard_hw_test.c"
#endif

#if MALI_DDK_GATOR_API_VERSION != 3
/*                        */
enum {
	JM_BLOCK = 0,
	TILER_BLOCK,
	SHADER_BLOCK,
	MMU_BLOCK
};
#endif

static const char *mali_name;

/*                           
  
                                                                        
                                                                                       
                                                                 
                                                                                    
                                                                                                              
 */

/*                   */
#if MALI_DDK_GATOR_API_VERSION == 3

static const char *const *hardware_counter_names;
static int number_of_hardware_counters;

#else

static const char *const hardware_counter_names[] = {
	/*             */
	"",
	"",
	"",
	"",
	"MESSAGES_SENT",
	"MESSAGES_RECEIVED",
	"GPU_ACTIVE",		/*   */
	"IRQ_ACTIVE",
	"JS0_JOBS",
	"JS0_TASKS",
	"JS0_ACTIVE",
	"",
	"JS0_WAIT_READ",
	"JS0_WAIT_ISSUE",
	"JS0_WAIT_DEPEND",
	"JS0_WAIT_FINISH",
	"JS1_JOBS",
	"JS1_TASKS",
	"JS1_ACTIVE",
	"",
	"JS1_WAIT_READ",
	"JS1_WAIT_ISSUE",
	"JS1_WAIT_DEPEND",
	"JS1_WAIT_FINISH",
	"JS2_JOBS",
	"JS2_TASKS",
	"JS2_ACTIVE",
	"",
	"JS2_WAIT_READ",
	"JS2_WAIT_ISSUE",
	"JS2_WAIT_DEPEND",
	"JS2_WAIT_FINISH",
	"JS3_JOBS",
	"JS3_TASKS",
	"JS3_ACTIVE",
	"",
	"JS3_WAIT_READ",
	"JS3_WAIT_ISSUE",
	"JS3_WAIT_DEPEND",
	"JS3_WAIT_FINISH",
	"JS4_JOBS",
	"JS4_TASKS",
	"JS4_ACTIVE",
	"",
	"JS4_WAIT_READ",
	"JS4_WAIT_ISSUE",
	"JS4_WAIT_DEPEND",
	"JS4_WAIT_FINISH",
	"JS5_JOBS",
	"JS5_TASKS",
	"JS5_ACTIVE",
	"",
	"JS5_WAIT_READ",
	"JS5_WAIT_ISSUE",
	"JS5_WAIT_DEPEND",
	"JS5_WAIT_FINISH",
	"JS6_JOBS",
	"JS6_TASKS",
	"JS6_ACTIVE",
	"",
	"JS6_WAIT_READ",
	"JS6_WAIT_ISSUE",
	"JS6_WAIT_DEPEND",
	"JS6_WAIT_FINISH",

	/*      */
	"",
	"",
	"",
	"JOBS_PROCESSED",
	"TRIANGLES",
	"QUADS",
	"POLYGONS",
	"POINTS",
	"LINES",
	"VCACHE_HIT",
	"VCACHE_MISS",
	"FRONT_FACING",
	"BACK_FACING",
	"PRIM_VISIBLE",
	"PRIM_CULLED",
	"PRIM_CLIPPED",
	"LEVEL0",
	"LEVEL1",
	"LEVEL2",
	"LEVEL3",
	"LEVEL4",
	"LEVEL5",
	"LEVEL6",
	"LEVEL7",
	"COMMAND_1",
	"COMMAND_2",
	"COMMAND_3",
	"COMMAND_4",
	"COMMAND_4_7",
	"COMMAND_8_15",
	"COMMAND_16_63",
	"COMMAND_64",
	"COMPRESS_IN",
	"COMPRESS_OUT",
	"COMPRESS_FLUSH",
	"TIMESTAMPS",
	"PCACHE_HIT",
	"PCACHE_MISS",
	"PCACHE_LINE",
	"PCACHE_STALL",
	"WRBUF_HIT",
	"WRBUF_MISS",
	"WRBUF_LINE",
	"WRBUF_PARTIAL",
	"WRBUF_STALL",
	"ACTIVE",
	"LOADING_DESC",
	"INDEX_WAIT",
	"INDEX_RANGE_WAIT",
	"VERTEX_WAIT",
	"PCACHE_WAIT",
	"WRBUF_WAIT",
	"BUS_READ",
	"BUS_WRITE",
	"",
	"",
	"",
	"",
	"",
	"UTLB_STALL",
	"UTLB_REPLAY_MISS",
	"UTLB_REPLAY_FULL",
	"UTLB_NEW_MISS",
	"UTLB_HIT",

	/*             */
	"",
	"",
	"",
	"SHADER_CORE_ACTIVE",
	"FRAG_ACTIVE",
	"FRAG_PRIMATIVES",
	"FRAG_PRIMATIVES_DROPPED",
	"FRAG_CYCLE_DESC",
	"FRAG_CYCLES_PLR",
	"FRAG_CYCLES_VERT",
	"FRAG_CYCLES_TRISETUP",
	"FRAG_CYCLES_RAST",
	"FRAG_THREADS",
	"FRAG_DUMMY_THREADS",
	"FRAG_QUADS_RAST",
	"FRAG_QUADS_EZS_TEST",
	"FRAG_QUADS_EZS_KILLED",
	"FRAG_QUADS_LZS_TEST",
	"FRAG_QUADS_LZS_KILLED",
	"FRAG_CYCLE_NO_TILE",
	"FRAG_NUM_TILES",
	"FRAG_TRANS_ELIM",
	"COMPUTE_ACTIVE",
	"COMPUTE_TASKS",
	"COMPUTE_THREADS",
	"COMPUTE_CYCLES_DESC",
	"TRIPIPE_ACTIVE",
	"ARITH_WORDS",
	"ARITH_CYCLES_REG",
	"ARITH_CYCLES_L0",
	"ARITH_FRAG_DEPEND",
	"LS_WORDS",
	"LS_ISSUES",
	"LS_RESTARTS",
	"LS_REISSUES_MISS",
	"LS_REISSUES_VD",
	"LS_REISSUE_ATTRIB_MISS",
	"LS_NO_WB",
	"TEX_WORDS",
	"TEX_BUBBLES",
	"TEX_WORDS_L0",
	"TEX_WORDS_DESC",
	"TEX_THREADS",
	"TEX_RECIRC_FMISS",
	"TEX_RECIRC_DESC",
	"TEX_RECIRC_MULTI",
	"TEX_RECIRC_PMISS",
	"TEX_RECIRC_CONF",
	"LSC_READ_HITS",
	"LSC_READ_MISSES",
	"LSC_WRITE_HITS",
	"LSC_WRITE_MISSES",
	"LSC_ATOMIC_HITS",
	"LSC_ATOMIC_MISSES",
	"LSC_LINE_FETCHES",
	"LSC_DIRTY_LINE",
	"LSC_SNOOPS",
	"AXI_TLB_STALL",
	"AXI_TLB_MIESS",
	"AXI_TLB_TRANSACTION",
	"LS_TLB_MISS",
	"LS_TLB_HIT",
	"AXI_BEATS_READ",
	"AXI_BEATS_WRITTEN",

	/*           */
	"",
	"",
	"",
	"",
	"MMU_HIT",
	"MMU_NEW_MISS",
	"MMU_REPLAY_FULL",
	"MMU_REPLAY_MISS",
	"MMU_TABLE_WALK",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"UTLB_HIT",
	"UTLB_NEW_MISS",
	"UTLB_REPLAY_FULL",
	"UTLB_REPLAY_MISS",
	"UTLB_STALL",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"L2_WRITE_BEATS",
	"L2_READ_BEATS",
	"L2_ANY_LOOKUP",
	"L2_READ_LOOKUP",
	"L2_SREAD_LOOKUP",
	"L2_READ_REPLAY",
	"L2_READ_SNOOP",
	"L2_READ_HIT",
	"L2_CLEAN_MISS",
	"L2_WRITE_LOOKUP",
	"L2_SWRITE_LOOKUP",
	"L2_WRITE_REPLAY",
	"L2_WRITE_SNOOP",
	"L2_WRITE_HIT",
	"L2_EXT_READ_FULL",
	"L2_EXT_READ_HALF",
	"L2_EXT_WRITE_FULL",
	"L2_EXT_WRITE_HALF",
	"L2_EXT_READ",
	"L2_EXT_READ_LINE",
	"L2_EXT_WRITE",
	"L2_EXT_WRITE_LINE",
	"L2_EXT_WRITE_SMALL",
	"L2_EXT_BARRIER",
	"L2_EXT_AR_STALL",
	"L2_EXT_R_BUF_FULL",
	"L2_EXT_RD_BUF_FULL",
	"L2_EXT_R_RAW",
	"L2_EXT_W_STALL",
	"L2_EXT_W_BUF_FULL",
	"L2_EXT_R_W_HAZARD",
	"L2_TAG_HAZARD",
	"L2_SNOOP_FULL",
	"L2_REPLAY_FULL"
};

static const int number_of_hardware_counters = ARRAY_SIZE(hardware_counter_names);

#endif

#define GET_HW_BLOCK(c) (((c) >> 6) & 0x3)
#define GET_COUNTER_OFFSET(c) ((c) & 0x3f)

#if MALI_DDK_GATOR_API_VERSION == 3
/*                                                            */
static struct kbase_gator_hwcnt_handles *handles;

/*                                     */
static struct kbase_gator_hwcnt_info *in_out_info;

#else
/*                                       */
static void *kernel_dump_buffer;

#if MALI_DDK_GATOR_API_VERSION == 2
/*                                                 */
struct kbase_hwc_dma_mapping kernel_dump_buffer_handle;
#endif

/*                          */
static struct kbase_context *kbcontext;
static struct kbase_device *kbdevice;

/*
                                                                
                                                                  
 */
struct kbase_device *kbase_find_device(int minor);
#endif

static volatile bool kbase_device_busy;
static unsigned int num_hardware_counters_enabled;

/*                                            */
static struct mali_counter *counters;

/*                                                                 */
static int *counter_dump;

extern struct mali_counter mali_activity[3];

static const char *const mali_activity_names[] = {
	"fragment",
	"vertex",
	"opencl",
};

#define SYMBOL_GET(FUNCTION, ERROR_COUNT) \
	do { \
		if (FUNCTION ## _symbol) { \
			pr_err("gator: mali " #FUNCTION " symbol was already registered\n"); \
			(ERROR_COUNT)++; \
		} else { \
			FUNCTION ## _symbol = symbol_get(FUNCTION); \
			if (!FUNCTION ## _symbol) { \
				pr_err("gator: mali online " #FUNCTION " symbol not found\n"); \
				(ERROR_COUNT)++; \
			} \
		} \
	} while (0)

#define SYMBOL_CLEANUP(FUNCTION) \
	do { \
		if (FUNCTION ## _symbol) { \
			symbol_put(FUNCTION); \
			FUNCTION ## _symbol = NULL; \
		} \
	} while (0)

/* 
                                                                     
                                            
 */
static int init_symbols(void)
{
	int error_count = 0;
#if MALI_DDK_GATOR_API_VERSION == 3
	SYMBOL_GET(kbase_gator_instr_hwcnt_dump_irq, error_count);
	SYMBOL_GET(kbase_gator_instr_hwcnt_dump_complete, error_count);
	SYMBOL_GET(kbase_gator_hwcnt_init, error_count);
	SYMBOL_GET(kbase_gator_hwcnt_term, error_count);
#else
	SYMBOL_GET(kbase_find_device, error_count);
	SYMBOL_GET(kbase_create_context, error_count);
	SYMBOL_GET(kbase_va_alloc, error_count);
	SYMBOL_GET(kbase_instr_hwcnt_enable, error_count);
	SYMBOL_GET(kbase_instr_hwcnt_clear, error_count);
	SYMBOL_GET(kbase_instr_hwcnt_dump_irq, error_count);
	SYMBOL_GET(kbase_instr_hwcnt_dump_complete, error_count);
	SYMBOL_GET(kbase_instr_hwcnt_disable, error_count);
	SYMBOL_GET(kbase_va_free, error_count);
	SYMBOL_GET(kbase_destroy_context, error_count);
#endif

	return error_count;
}

/* 
                                                          
 */
static void clean_symbols(void)
{
#if MALI_DDK_GATOR_API_VERSION == 3
	SYMBOL_CLEANUP(kbase_gator_instr_hwcnt_dump_irq);
	SYMBOL_CLEANUP(kbase_gator_instr_hwcnt_dump_complete);
	SYMBOL_CLEANUP(kbase_gator_hwcnt_init);
	SYMBOL_CLEANUP(kbase_gator_hwcnt_term);
#else
	SYMBOL_CLEANUP(kbase_find_device);
	SYMBOL_CLEANUP(kbase_create_context);
	SYMBOL_CLEANUP(kbase_va_alloc);
	SYMBOL_CLEANUP(kbase_instr_hwcnt_enable);
	SYMBOL_CLEANUP(kbase_instr_hwcnt_clear);
	SYMBOL_CLEANUP(kbase_instr_hwcnt_dump_irq);
	SYMBOL_CLEANUP(kbase_instr_hwcnt_dump_complete);
	SYMBOL_CLEANUP(kbase_instr_hwcnt_disable);
	SYMBOL_CLEANUP(kbase_va_free);
	SYMBOL_CLEANUP(kbase_destroy_context);
#endif
}

/* 
                                              
                                                                       
                                                                         
                                                                                  
  
                                                                                
 */
static int is_read_scheduled(const struct timespec *current_time, u32 *prev_time_s, s32 *next_read_time_ns)
{
	/*                                                                           */
	if (current_time->tv_sec != *prev_time_s)
		*next_read_time_ns = *next_read_time_ns - NSEC_PER_SEC;

	/*                                                       */
	if (current_time->tv_nsec < *next_read_time_ns)
		return 0;

	/*                                                                                  */
	*next_read_time_ns = current_time->tv_nsec + READ_INTERVAL_NSEC;

	*prev_time_s = current_time->tv_sec;
	return 1;
}

static int start(void)
{
#if MALI_DDK_GATOR_API_VERSION != 3
	struct kbase_uk_hwcnt_setup setup;
	unsigned long long shadersPresent = 0;
	u16 bitmask[] = { 0, 0, 0, 0 };
	mali_error err;
#endif
	int cnt;

#if MALI_DDK_GATOR_API_VERSION == 3
	/*                   */
	num_hardware_counters_enabled = 0;

	/*                                                         */
	in_out_info = kmalloc(sizeof(*in_out_info), GFP_KERNEL);
	for (cnt = 0; cnt < ARRAY_SIZE(in_out_info->bitmask); cnt++)
		in_out_info->bitmask[cnt] = 0;

	/*                                                           */
	for (cnt = 0; cnt < number_of_hardware_counters; cnt++) {
		if (counters[cnt].enabled) {
			int block = GET_HW_BLOCK(cnt);
			int enable_bit = GET_COUNTER_OFFSET(cnt) / 4;

			in_out_info->bitmask[block] |= (1 << enable_bit);
			pr_debug("gator: Mali-Midgard: hardware counter %s selected [%d]\n", hardware_counter_names[cnt], cnt);
			num_hardware_counters_enabled++;
		}
	}

	/*                                        */
	if (num_hardware_counters_enabled > 0) {
		if (init_symbols() > 0) {
			clean_symbols();
			/*                                                      */
			return 0;
		}

		handles = kbase_gator_hwcnt_init_symbol(in_out_info);

		if (handles == NULL)
			goto out;

		kbase_device_busy = false;
	}

	return 0;
#else
	/*                   */
	num_hardware_counters_enabled = 0;

	/*                                                           */
	for (cnt = 0; cnt < number_of_hardware_counters; cnt++) {
		const struct mali_counter *counter = &counters[cnt];

		if (counter->enabled) {
			int block = GET_HW_BLOCK(cnt);
			int enable_bit = GET_COUNTER_OFFSET(cnt) / 4;

			bitmask[block] |= (1 << enable_bit);
			pr_debug("gator: Mali-Midgard: hardware counter %s selected [%d]\n", hardware_counter_names[cnt], cnt);
			num_hardware_counters_enabled++;
		}
	}

	/*                                        */
	if (num_hardware_counters_enabled > 0) {
		if (init_symbols() > 0) {
			clean_symbols();
			/*                                                      */
			return 0;
		}

		kbdevice = kbase_find_device_symbol(-1);

		/*                                   */
		if (kbcontext) {
			pr_debug("gator: Mali-Midgard: error context already present\n");
			goto out;
		}

		/*                                                                                 */
		kbcontext = kbase_create_context_symbol(kbdevice);
		if (!kbcontext) {
			pr_debug("gator: Mali-Midgard: error creating kbase context\n");
			goto out;
		}

		/*                                              */
		shadersPresent = kbdevice->shader_present_bitmap;
		shader_present_low = (unsigned long)shadersPresent;

		/*
                                                          
                                      
                                                          
                                                                       
                                                                    
                                                                        
                                                                        
   */
#if MALI_DDK_GATOR_API_VERSION == 1
		kernel_dump_buffer = kbase_va_alloc_symbol(kbcontext, 4096);
#elif MALI_DDK_GATOR_API_VERSION == 2
		kernel_dump_buffer = kbase_va_alloc_symbol(kbcontext, 4096, &kernel_dump_buffer_handle);
#endif
		if (!kernel_dump_buffer) {
			pr_debug("gator: Mali-Midgard: error trying to allocate va\n");
			goto destroy_context;
		}

		setup.dump_buffer = (uintptr_t)kernel_dump_buffer;
		setup.jm_bm = bitmask[JM_BLOCK];
		setup.tiler_bm = bitmask[TILER_BLOCK];
		setup.shader_bm = bitmask[SHADER_BLOCK];
		setup.mmu_l2_bm = bitmask[MMU_BLOCK];
		/*                                          */
		setup.l3_cache_bm = 0;

		/*                                                                   */
		err = kbase_instr_hwcnt_enable_symbol(kbcontext, &setup);
		if (err != MALI_ERROR_NONE) {
			pr_debug("gator: Mali-Midgard: can't setup hardware counters\n");
			goto free_buffer;
		}
		pr_debug("gator: Mali-Midgard: hardware counters enabled\n");
		kbase_instr_hwcnt_clear_symbol(kbcontext);
		pr_debug("gator: Mali-Midgard: hardware counters cleared\n");

		kbase_device_busy = false;
	}

	return 0;

free_buffer:
#if MALI_DDK_GATOR_API_VERSION == 1
	kbase_va_free_symbol(kbcontext, kernel_dump_buffer);
#elif MALI_DDK_GATOR_API_VERSION == 2
	kbase_va_free_symbol(kbcontext, &kernel_dump_buffer_handle);
#endif

destroy_context:
	kbase_destroy_context_symbol(kbcontext);
#endif

out:
	clean_symbols();
	return -1;
}

static void stop(void)
{
	unsigned int cnt;
#if MALI_DDK_GATOR_API_VERSION == 3
	struct kbase_gator_hwcnt_handles *temp_hand;
#else
	struct kbase_context *temp_kbcontext;
#endif

	pr_debug("gator: Mali-Midgard: stop\n");

	/*                              */
	for (cnt = 0; cnt < number_of_hardware_counters; cnt++)
		counters[cnt].enabled = 0;

	/*                                     */
#if MALI_DDK_GATOR_API_VERSION == 3
	if (num_hardware_counters_enabled > 0 && handles != NULL) {
		/*
                                                                  
                                                    
   */
		temp_hand = handles;
		handles = NULL;

		kbase_gator_hwcnt_term_symbol(in_out_info, temp_hand);

		kfree(in_out_info);

#else
	if (num_hardware_counters_enabled > 0 && kbcontext != NULL) {
		/*
                                                                  
                                                    
   */
		temp_kbcontext = kbcontext;
		kbcontext = NULL;

		kbase_instr_hwcnt_disable_symbol(temp_kbcontext);

#if MALI_DDK_GATOR_API_VERSION == 1
		kbase_va_free_symbol(temp_kbcontext, kernel_dump_buffer);
#elif MALI_DDK_GATOR_API_VERSION == 2
		kbase_va_free_symbol(temp_kbcontext, &kernel_dump_buffer_handle);
#endif

		kbase_destroy_context_symbol(temp_kbcontext);
#endif

		pr_debug("gator: Mali-Midgard: hardware counters stopped\n");

		clean_symbols();
	}
}

static int read_counter(const int cnt, const int len, const struct mali_counter *counter)
{
	const int block = GET_HW_BLOCK(cnt);
	const int counter_offset = GET_COUNTER_OFFSET(cnt);

#if MALI_DDK_GATOR_API_VERSION == 3
	const char *block_base_address = (char *)in_out_info->kernel_dump_buffer;
	int i;
	int shader_core_count = 0;
	u32 value = 0;

	for (i = 0; i < in_out_info->nr_hwc_blocks; i++) {
		if (block == in_out_info->hwc_layout[i]) {
			value += *((u32 *)(block_base_address + (0x100 * i)) + counter_offset);
			if (block == SHADER_BLOCK)
				++shader_core_count;
		}
	}

	if (shader_core_count > 1)
		value /= shader_core_count;
#else
	const char *block_base_address = (char *)kernel_dump_buffer + vithar_blocks[block];

	/*                                                                        */
	if (block == SHADER_BLOCK) {
		int i = 0;
		int shader_core_count = 0;

		value = 0;

		for (i = 0; i < 4; i++) {
			if ((shader_present_low >> i) & 1) {
				value += *((u32 *)(block_base_address + (0x100 * i)) + counter_offset);
				shader_core_count++;
			}
		}

		for (i = 0; i < 4; i++) {
			if ((shader_present_low >> (i+4)) & 1) {
				value += *((u32 *)(block_base_address + (0x100 * i) + 0x800) + counter_offset);
				shader_core_count++;
			}
		}

		/*                                                        */
		if (shader_core_count != 0)
			value /= shader_core_count;
	} else {
		value = *((u32 *)block_base_address + counter_offset);
	}
#endif

	counter_dump[len + 0] = counter->key;
	counter_dump[len + 1] = value;

	return 2;
}

static int read(int **buffer, bool sched_switch)
{
	int cnt;
	int len = 0;
	uint32_t success;

	struct timespec current_time;
	static u32 prev_time_s;
	static s32 next_read_time_ns;

	if (!on_primary_core() || sched_switch)
		return 0;

	getnstimeofday(&current_time);

	/*
                                                            
                                                               
                           
  */
	if (!is_read_scheduled(&current_time, &prev_time_s, &next_read_time_ns))
		return 0;

	/*
                          
                                                                                       
  */
	if (num_hardware_counters_enabled > 0) {
#if MALI_DDK_GATOR_API_VERSION != 3
		const unsigned int vithar_blocks[] = {
			0x700,	/*                                 */
			0x400,	/*                                 */
			0x000,	/*                                 */
			0x500	/*                                 */
		};
#endif

#if MALI_DDK_GATOR_API_VERSION == 3
		if (!handles)
			return -1;

		/*                                                              */
		if (kbase_gator_instr_hwcnt_dump_complete_symbol(handles, &success) == MALI_TRUE) {
#else
		if (!kbcontext)
			return -1;

		/*                                                              */
		if (kbase_instr_hwcnt_dump_complete_symbol(kbcontext, &success) == MALI_TRUE) {
#endif
			kbase_device_busy = false;

			if (success == MALI_TRUE) {
				/*                                                       */
				for (cnt = 0; cnt < number_of_hardware_counters; cnt++) {
					const struct mali_counter *counter = &counters[cnt];

					if (counter->enabled)
						len += read_counter(cnt, len, counter);
				}
			}
		}

		if (!kbase_device_busy) {
			kbase_device_busy = true;
#if MALI_DDK_GATOR_API_VERSION == 3
			kbase_gator_instr_hwcnt_dump_irq_symbol(handles);
#else
			kbase_instr_hwcnt_dump_irq_symbol(kbcontext);
#endif
		}
	}

	/*                   */
	if (buffer)
		*buffer = counter_dump;

	return len;
}

static int create_files(struct super_block *sb, struct dentry *root)
{
	unsigned int event;
	/*
                                        
  */
	for (event = 0; event < ARRAY_SIZE(mali_activity); event++) {
		if (gator_mali_create_file_system("Midgard", mali_activity_names[event], sb, root, &mali_activity[event], NULL) != 0)
			return -1;
	}

	for (event = 0; event < number_of_hardware_counters; event++) {
		if (gator_mali_create_file_system(mali_name, hardware_counter_names[event], sb, root, &counters[event], NULL) != 0)
			return -1;
	}

	return 0;
}

static void shutdown(void)
{
#if MALI_DDK_GATOR_API_VERSION == 3
	void (*kbase_gator_hwcnt_term_names_symbol)(void) = NULL;
	int error_count = 0;
#endif

	kfree(counters);
	kfree(counter_dump);

#if MALI_DDK_GATOR_API_VERSION == 3
	SYMBOL_GET(kbase_gator_hwcnt_term_names, error_count);

	number_of_hardware_counters = -1;
	hardware_counter_names = NULL;
	if (kbase_gator_hwcnt_term_names_symbol != NULL) {
		kbase_gator_hwcnt_term_names_symbol();
		pr_err("Released symbols\n");
	}

	SYMBOL_CLEANUP(kbase_gator_hwcnt_term_names);
#endif
}

static struct gator_interface gator_events_mali_midgard_interface = {
	.shutdown = shutdown,
	.create_files = create_files,
	.start = start,
	.stop = stop,
	.read = read
};

int gator_events_mali_midgard_hw_init(void)
{
#if MALI_DDK_GATOR_API_VERSION == 3
	const char *const *(*kbase_gator_hwcnt_init_names_symbol)(uint32_t *) = NULL;
	int error_count = 0;
#endif

	pr_debug("gator: Mali-Midgard: sw_counters init\n");

#if GATOR_TEST
	test_all_is_read_scheduled();
#endif

#if MALI_DDK_GATOR_API_VERSION == 3
	SYMBOL_GET(kbase_gator_hwcnt_init_names, error_count);
	if (error_count > 0) {
		SYMBOL_CLEANUP(kbase_gator_hwcnt_init_names);
		return 1;
	}

	number_of_hardware_counters = -1;
	hardware_counter_names = kbase_gator_hwcnt_init_names_symbol(&number_of_hardware_counters);

	SYMBOL_CLEANUP(kbase_gator_hwcnt_init_names);

	if ((hardware_counter_names == NULL) || (number_of_hardware_counters <= 0)) {
		pr_err("gator: Error reading hardware counters names: got %d names\n", number_of_hardware_counters);
		return -1;
	}
#else
	mali_name = "Midgard";
#endif

	counters = kmalloc(sizeof(*counters)*number_of_hardware_counters, GFP_KERNEL);
	counter_dump = kmalloc(sizeof(*counter_dump)*number_of_hardware_counters*2, GFP_KERNEL);

	gator_mali_initialise_counters(mali_activity, ARRAY_SIZE(mali_activity));
	gator_mali_initialise_counters(counters, number_of_hardware_counters);

	return gator_events_install(&gator_events_mali_midgard_interface);
}
