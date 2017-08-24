/*
 * Copyright (C) 2011-2013 ARM Limited. All rights reserved.
 * 
 * This program is free software and is provided to you under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation, and any use by you of this program is subject to the terms of such GNU licence.
 * 
 * A copy of the licence is included with the program, and can also be obtained from Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "mali_pp_job.h"
#include "mali_pp.h"
#include "mali_hw_core.h"
#include "mali_group.h"
#include "regs/mali_200_regs.h"
#include "mali_kernel_common.h"
#include "mali_kernel_core.h"
#include "mali_dma.h"
#if defined(CONFIG_MALI400_PROFILING)
#include "mali_osk_profiling.h"
#endif

/*                                       */
#define MALI_PP_MALI200_NUM_FRAME_REGISTERS ((0x04C/4)+1)
/*                                                 */
#define MALI_PP_MALI400_NUM_FRAME_REGISTERS ((0x058/4)+1)

static struct mali_pp_core* mali_global_pp_cores[MALI_MAX_NUMBER_OF_PP_CORES] = { NULL };
static u32 mali_global_num_pp_cores = 0;

/*                    */
static void mali_pp_irq_probe_trigger(void *data);
static _mali_osk_errcode_t mali_pp_irq_probe_ack(void *data);

struct mali_pp_core *mali_pp_create(const _mali_osk_resource_t *resource, struct mali_group *group, mali_bool is_virtual, u32 bcast_id)
{
	struct mali_pp_core* core = NULL;

	MALI_DEBUG_PRINT(2, ("Mali PP: Creating Mali PP core: %s\n", resource->description));
	MALI_DEBUG_PRINT(2, ("Mali PP: Base address of PP core: 0x%x\n", resource->base));

	if (mali_global_num_pp_cores >= MALI_MAX_NUMBER_OF_PP_CORES) {
		MALI_PRINT_ERROR(("Mali PP: Too many PP core objects created\n"));
		return NULL;
	}

	core = _mali_osk_malloc(sizeof(struct mali_pp_core));
	if (NULL != core) {
		core->core_id = mali_global_num_pp_cores;
		core->bcast_id = bcast_id;

		if (_MALI_OSK_ERR_OK == mali_hw_core_create(&core->hw_core, resource, MALI200_REG_SIZEOF_REGISTER_BANK)) {
			_mali_osk_errcode_t ret;

			if (!is_virtual) {
				ret = mali_pp_reset(core);
			} else {
				ret = _MALI_OSK_ERR_OK;
			}

			if (_MALI_OSK_ERR_OK == ret) {
				ret = mali_group_add_pp_core(group, core);
				if (_MALI_OSK_ERR_OK == ret) {
					/*                                                          */
					MALI_DEBUG_ASSERT(!is_virtual || -1 != resource->irq);

					core->irq = _mali_osk_irq_init(resource->irq,
					                               mali_group_upper_half_pp,
					                               group,
					                               mali_pp_irq_probe_trigger,
					                               mali_pp_irq_probe_ack,
					                               core,
					                               resource->description);
					if (NULL != core->irq) {
						mali_global_pp_cores[mali_global_num_pp_cores] = core;
						mali_global_num_pp_cores++;

						return core;
					} else {
						MALI_PRINT_ERROR(("Mali PP: Failed to setup interrupt handlers for PP core %s\n", core->hw_core.description));
					}
					mali_group_remove_pp_core(group);
				} else {
					MALI_PRINT_ERROR(("Mali PP: Failed to add core %s to group\n", core->hw_core.description));
				}
			}
			mali_hw_core_delete(&core->hw_core);
		}

		_mali_osk_free(core);
	} else {
		MALI_PRINT_ERROR(("Mali PP: Failed to allocate memory for PP core\n"));
	}

	return NULL;
}

void mali_pp_delete(struct mali_pp_core *core)
{
	u32 i;

	MALI_DEBUG_ASSERT_POINTER(core);

	_mali_osk_irq_term(core->irq);
	mali_hw_core_delete(&core->hw_core);

	/*                              */
	for (i = 0; i < mali_global_num_pp_cores; i++) {
		if (mali_global_pp_cores[i] == core) {
			mali_global_pp_cores[i] = NULL;
			mali_global_num_pp_cores--;

			if (i != mali_global_num_pp_cores) {
				/*                                                                   
                                                        */
				mali_global_pp_cores[i] = mali_global_pp_cores[mali_global_num_pp_cores];
				mali_global_pp_cores[mali_global_num_pp_cores] = NULL;
			}

			break;
		}
	}

	_mali_osk_free(core);
}

void mali_pp_stop_bus(struct mali_pp_core *core)
{
	MALI_DEBUG_ASSERT_POINTER(core);
	/*                                                                      */
	mali_hw_core_register_write(&core->hw_core, MALI200_REG_ADDR_MGMT_CTRL_MGMT, MALI200_REG_VAL_CTRL_MGMT_STOP_BUS);
}

_mali_osk_errcode_t mali_pp_stop_bus_wait(struct mali_pp_core *core)
{
	int i;

	MALI_DEBUG_ASSERT_POINTER(core);

	/*                            */
	mali_pp_stop_bus(core);

	/*                            */
	for (i = 0; i < MALI_REG_POLL_COUNT_FAST; i++) {
		if (mali_hw_core_register_read(&core->hw_core, MALI200_REG_ADDR_MGMT_STATUS) & MALI200_REG_VAL_STATUS_BUS_STOPPED)
			break;
	}

	if (MALI_REG_POLL_COUNT_FAST == i) {
		MALI_PRINT_ERROR(("Mali PP: Failed to stop bus on %s. Status: 0x%08x\n", core->hw_core.description, mali_hw_core_register_read(&core->hw_core, MALI200_REG_ADDR_MGMT_STATUS)));
		return _MALI_OSK_ERR_FAULT;
	}
	return _MALI_OSK_ERR_OK;
}

/*                             
                                                                             */
static const u32 mali_frame_registers_reset_values[_MALI_PP_MAX_FRAME_REGISTERS] = {
	0x0, /*                                */
	0x0, /*                                           */
	0x0, /*                               */
	0x2, /*                         */
	0x0, /*                        */
	0x0, /*                              */
	0x0, /*                             */
	0x0, /*                             */
	0x0, /*                             */
	0x0, /*                             */
	0x0, /*                                  */
	0x0, /*                              */
	0x0, /*                           */
	0x0, /*                                          */
	0x0, /*          */
	0x0, /*          */
	0x0, /*                          */
	0x0, /*                          */
	0x75, /*                             */
	0x0, /*                        */
	0x0, /*                              */
	0x0, /*                  */
	0x0 /*                                   */
};

/*                           */
static const u32 mali_wb_registers_reset_values[_MALI_PP_MAX_WB_REGISTERS] = {
	0x0, /*                            */
	0x0, /*                             */
	0x0, /*                                  */
	0x0, /*                               */
	0x0, /*                   */
	0x0, /*                            */
	0x0, /*                           */
	0x0, /*                         */
	0x0, /*                         */
	0x0, /*                                 */
	0x0, /*                                          */
	0x0  /*                                           */
};

/*                                                   */
static const u32 mali_perf_cnt_enable_reset_value = 0;

_mali_osk_errcode_t mali_pp_hard_reset(struct mali_pp_core *core)
{
	/*                                                  */
	const u32 reset_invalid_value = 0xC0FFE000;
	const u32 reset_check_value = 0xC01A0000;
	int i;

	MALI_DEBUG_ASSERT_POINTER(core);
	MALI_DEBUG_PRINT(2, ("Mali PP: Hard reset of core %s\n", core->hw_core.description));

	/*                                                                                           */
	mali_hw_core_register_write_relaxed(&core->hw_core, MALI200_REG_ADDR_MGMT_WRITE_BOUNDARY_LOW, reset_invalid_value);
	mali_hw_core_register_write_relaxed(&core->hw_core, MALI200_REG_ADDR_MGMT_INT_MASK, MALI200_REG_VAL_IRQ_MASK_NONE);

	/*                     */
	mali_hw_core_register_write(&core->hw_core, MALI200_REG_ADDR_MGMT_CTRL_MGMT, MALI200_REG_VAL_CTRL_MGMT_FORCE_RESET);

	/*                               */
	for (i = 0; i < MALI_REG_POLL_COUNT_FAST; i++) {
		mali_hw_core_register_write(&core->hw_core, MALI200_REG_ADDR_MGMT_WRITE_BOUNDARY_LOW, reset_check_value);
		if (reset_check_value == mali_hw_core_register_read(&core->hw_core, MALI200_REG_ADDR_MGMT_WRITE_BOUNDARY_LOW)) {
			break;
		}
	}

	if (MALI_REG_POLL_COUNT_FAST == i) {
		MALI_PRINT_ERROR(("Mali PP: The hard reset loop didn't work, unable to recover\n"));
	}

	mali_hw_core_register_write(&core->hw_core, MALI200_REG_ADDR_MGMT_WRITE_BOUNDARY_LOW, 0x00000000); /*                            */
	/*                      */
	mali_hw_core_register_write(&core->hw_core, MALI200_REG_ADDR_MGMT_INT_CLEAR, MALI200_REG_VAL_IRQ_MASK_ALL);
	mali_hw_core_register_write(&core->hw_core, MALI200_REG_ADDR_MGMT_INT_MASK, MALI200_REG_VAL_IRQ_MASK_USED);

	return _MALI_OSK_ERR_OK;
}

void mali_pp_reset_async(struct mali_pp_core *core)
{
	MALI_DEBUG_ASSERT_POINTER(core);

	MALI_DEBUG_PRINT(4, ("Mali PP: Reset of core %s\n", core->hw_core.description));

	mali_hw_core_register_write(&core->hw_core, MALI200_REG_ADDR_MGMT_INT_MASK, 0); /*                  */
	mali_hw_core_register_write(&core->hw_core, MALI200_REG_ADDR_MGMT_INT_RAWSTAT, MALI200_REG_VAL_IRQ_MASK_ALL);
	mali_hw_core_register_write(&core->hw_core, MALI200_REG_ADDR_MGMT_CTRL_MGMT, MALI400PP_REG_VAL_CTRL_MGMT_SOFT_RESET);
}

_mali_osk_errcode_t mali_pp_reset_wait(struct mali_pp_core *core)
{
	int i;
	u32 rawstat = 0;

	for (i = 0; i < MALI_REG_POLL_COUNT_FAST; i++) {
		if (!(mali_pp_read_status(core) & MALI200_REG_VAL_STATUS_RENDERING_ACTIVE)) {
			rawstat = mali_hw_core_register_read(&core->hw_core, MALI200_REG_ADDR_MGMT_INT_RAWSTAT);
			if (rawstat == MALI400PP_REG_VAL_IRQ_RESET_COMPLETED) {
				break;
			}
		}
	}

	if (i == MALI_REG_POLL_COUNT_FAST) {
		MALI_PRINT_ERROR(("Mali PP: Failed to reset core %s, rawstat: 0x%08x\n",
		                  core->hw_core.description, rawstat));
		return _MALI_OSK_ERR_FAULT;
	}

	/*                      */
	mali_hw_core_register_write(&core->hw_core, MALI200_REG_ADDR_MGMT_INT_CLEAR, MALI200_REG_VAL_IRQ_MASK_ALL);
	mali_hw_core_register_write(&core->hw_core, MALI200_REG_ADDR_MGMT_INT_MASK, MALI200_REG_VAL_IRQ_MASK_USED);

	return _MALI_OSK_ERR_OK;
}

_mali_osk_errcode_t mali_pp_reset(struct mali_pp_core *core)
{
	mali_pp_reset_async(core);
	return mali_pp_reset_wait(core);
}

void mali_pp_job_dma_cmd_prepare(struct mali_pp_core *core, struct mali_pp_job *job, u32 sub_job,
                                 mali_bool restart_virtual, mali_dma_cmd_buf *buf)
{
	u32 relative_address;
	u32 start_index;
	u32 nr_of_regs;
	u32 *frame_registers = mali_pp_job_get_frame_registers(job);
	u32 *wb0_registers = mali_pp_job_get_wb0_registers(job);
	u32 *wb1_registers = mali_pp_job_get_wb1_registers(job);
	u32 *wb2_registers = mali_pp_job_get_wb2_registers(job);
	u32 counter_src0 = mali_pp_job_get_perf_counter_src0(job, sub_job);
	u32 counter_src1 = mali_pp_job_get_perf_counter_src1(job, sub_job);

	MALI_DEBUG_ASSERT_POINTER(core);

	/*                       */

	/*
                                                                       
                                                                  
                                                             
  */
	mali_dma_write_conditional(buf, &core->hw_core, MALI200_REG_ADDR_FRAME, mali_pp_job_get_addr_frame(job, sub_job), mali_frame_registers_reset_values[MALI200_REG_ADDR_FRAME / sizeof(u32)]);

	/*                                                                                     */
	if (!mali_pp_job_is_virtual(job) || restart_virtual) {
		mali_dma_write_conditional(buf, &core->hw_core, MALI200_REG_ADDR_STACK, mali_pp_job_get_addr_stack(job, sub_job), mali_frame_registers_reset_values[MALI200_REG_ADDR_STACK / sizeof(u32)]);
	}

	/*                                                                           */
	relative_address = MALI200_REG_ADDR_RSW;
	start_index = MALI200_REG_ADDR_RSW / sizeof(u32);
	nr_of_regs = (MALI200_REG_ADDR_STACK - MALI200_REG_ADDR_RSW) / sizeof(u32);

	mali_dma_write_array_conditional(buf, &core->hw_core,
	                                 relative_address, &frame_registers[start_index],
	                                 nr_of_regs, &mali_frame_registers_reset_values[start_index]);

	/*                             */
	relative_address = MALI200_REG_ADDR_STACK_SIZE;
	start_index = MALI200_REG_ADDR_STACK_SIZE / sizeof(u32);

	mali_dma_write_conditional(buf, &core->hw_core,
	                           relative_address, frame_registers[start_index],
	                           mali_frame_registers_reset_values[start_index]);

	/*                           */

	/*                           */
	relative_address = MALI200_REG_ADDR_ORIGIN_OFFSET_X;
	start_index = MALI200_REG_ADDR_ORIGIN_OFFSET_X / sizeof(u32);
	nr_of_regs = MALI_PP_MALI400_NUM_FRAME_REGISTERS - MALI200_REG_ADDR_ORIGIN_OFFSET_X / sizeof(u32);

	mali_dma_write_array_conditional(buf, &core->hw_core,
	                                 relative_address, &frame_registers[start_index],
	                                 nr_of_regs, &mali_frame_registers_reset_values[start_index]);

	/*                     */
	if (wb0_registers[0]) { /*                                     */
		mali_dma_write_array_conditional(buf, &core->hw_core, MALI200_REG_ADDR_WB0, wb0_registers, _MALI_PP_MAX_WB_REGISTERS, mali_wb_registers_reset_values);
	}

	if (wb1_registers[0]) { /*                                     */
		mali_dma_write_array_conditional(buf, &core->hw_core, MALI200_REG_ADDR_WB1, wb1_registers, _MALI_PP_MAX_WB_REGISTERS, mali_wb_registers_reset_values);
	}

	if (wb2_registers[0]) { /*                                     */
		mali_dma_write_array_conditional(buf, &core->hw_core, MALI200_REG_ADDR_WB2, wb2_registers, _MALI_PP_MAX_WB_REGISTERS, mali_wb_registers_reset_values);
	}

	if (MALI_HW_CORE_NO_COUNTER != counter_src0) {
		mali_dma_write(buf, &core->hw_core, MALI200_REG_ADDR_MGMT_PERF_CNT_0_SRC, counter_src0);
		mali_dma_write_conditional(buf, &core->hw_core, MALI200_REG_ADDR_MGMT_PERF_CNT_0_ENABLE, MALI200_REG_VAL_PERF_CNT_ENABLE, mali_perf_cnt_enable_reset_value);
	}
	if (MALI_HW_CORE_NO_COUNTER != counter_src1) {
		mali_dma_write(buf, &core->hw_core, MALI200_REG_ADDR_MGMT_PERF_CNT_1_SRC, counter_src1);
		mali_dma_write_conditional(buf, &core->hw_core, MALI200_REG_ADDR_MGMT_PERF_CNT_1_ENABLE, MALI200_REG_VAL_PERF_CNT_ENABLE, mali_perf_cnt_enable_reset_value);
	}

	/*                                           */
	mali_dma_write(buf, &core->hw_core, MALI200_REG_ADDR_MGMT_CTRL_MGMT, MALI200_REG_VAL_CTRL_MGMT_START_RENDERING);
}

void mali_pp_job_start(struct mali_pp_core *core, struct mali_pp_job *job, u32 sub_job, mali_bool restart_virtual)
{
	u32 relative_address;
	u32 start_index;
	u32 nr_of_regs;
	u32 *frame_registers = mali_pp_job_get_frame_registers(job);
	u32 *wb0_registers = mali_pp_job_get_wb0_registers(job);
	u32 *wb1_registers = mali_pp_job_get_wb1_registers(job);
	u32 *wb2_registers = mali_pp_job_get_wb2_registers(job);
	u32 counter_src0 = mali_pp_job_get_perf_counter_src0(job, sub_job);
	u32 counter_src1 = mali_pp_job_get_perf_counter_src1(job, sub_job);

	MALI_DEBUG_ASSERT_POINTER(core);

	/*                       */

	/*
                                                                       
                                                                  
                                                             
  */
	mali_hw_core_register_write_relaxed_conditional(&core->hw_core, MALI200_REG_ADDR_FRAME, mali_pp_job_get_addr_frame(job, sub_job), mali_frame_registers_reset_values[MALI200_REG_ADDR_FRAME / sizeof(u32)]);

	/*                                                                                     */
	if (!mali_pp_job_is_virtual(job) || restart_virtual) {
		mali_hw_core_register_write_relaxed_conditional(&core->hw_core, MALI200_REG_ADDR_STACK, mali_pp_job_get_addr_stack(job, sub_job), mali_frame_registers_reset_values[MALI200_REG_ADDR_STACK / sizeof(u32)]);
	}

	/*                                                                           */
	relative_address = MALI200_REG_ADDR_RSW;
	start_index = MALI200_REG_ADDR_RSW / sizeof(u32);
	nr_of_regs = (MALI200_REG_ADDR_STACK - MALI200_REG_ADDR_RSW) / sizeof(u32);

	mali_hw_core_register_write_array_relaxed_conditional(&core->hw_core,
	        relative_address, &frame_registers[start_index],
	        nr_of_regs, &mali_frame_registers_reset_values[start_index]);

	/*                             */
	relative_address = MALI200_REG_ADDR_STACK_SIZE;
	start_index = MALI200_REG_ADDR_STACK_SIZE / sizeof(u32);

	mali_hw_core_register_write_relaxed_conditional(&core->hw_core,
	        relative_address, frame_registers[start_index],
	        mali_frame_registers_reset_values[start_index]);

	/*                           */

	/*                           */
	relative_address = MALI200_REG_ADDR_ORIGIN_OFFSET_X;
	start_index = MALI200_REG_ADDR_ORIGIN_OFFSET_X / sizeof(u32);
	nr_of_regs = MALI_PP_MALI400_NUM_FRAME_REGISTERS - MALI200_REG_ADDR_ORIGIN_OFFSET_X / sizeof(u32);

	mali_hw_core_register_write_array_relaxed_conditional(&core->hw_core,
	        relative_address, &frame_registers[start_index],
	        nr_of_regs, &mali_frame_registers_reset_values[start_index]);

	/*                     */
	if (wb0_registers[0]) { /*                                     */
		mali_hw_core_register_write_array_relaxed_conditional(&core->hw_core, MALI200_REG_ADDR_WB0, wb0_registers, _MALI_PP_MAX_WB_REGISTERS, mali_wb_registers_reset_values);
	}

	if (wb1_registers[0]) { /*                                     */
		mali_hw_core_register_write_array_relaxed_conditional(&core->hw_core, MALI200_REG_ADDR_WB1, wb1_registers, _MALI_PP_MAX_WB_REGISTERS, mali_wb_registers_reset_values);
	}

	if (wb2_registers[0]) { /*                                     */
		mali_hw_core_register_write_array_relaxed_conditional(&core->hw_core, MALI200_REG_ADDR_WB2, wb2_registers, _MALI_PP_MAX_WB_REGISTERS, mali_wb_registers_reset_values);
	}

	if (MALI_HW_CORE_NO_COUNTER != counter_src0) {
		mali_hw_core_register_write_relaxed(&core->hw_core, MALI200_REG_ADDR_MGMT_PERF_CNT_0_SRC, counter_src0);
		mali_hw_core_register_write_relaxed_conditional(&core->hw_core, MALI200_REG_ADDR_MGMT_PERF_CNT_0_ENABLE, MALI200_REG_VAL_PERF_CNT_ENABLE, mali_perf_cnt_enable_reset_value);
	}
	if (MALI_HW_CORE_NO_COUNTER != counter_src1) {
		mali_hw_core_register_write_relaxed(&core->hw_core, MALI200_REG_ADDR_MGMT_PERF_CNT_1_SRC, counter_src1);
		mali_hw_core_register_write_relaxed_conditional(&core->hw_core, MALI200_REG_ADDR_MGMT_PERF_CNT_1_ENABLE, MALI200_REG_VAL_PERF_CNT_ENABLE, mali_perf_cnt_enable_reset_value);
	}

#ifdef CONFIG_MALI400_HEATMAPS_ENABLED
	if(job->uargs.perf_counter_flag & _MALI_PERFORMANCE_COUNTER_FLAG_HEATMAP_ENABLE) {
		mali_hw_core_register_write_relaxed(&core->hw_core, MALI200_REG_ADDR_MGMT_PERFMON_CONTR, ((job->uargs.tilesx & 0x3FF) << 16) | 1);
		mali_hw_core_register_write_relaxed(&core->hw_core,  MALI200_REG_ADDR_MGMT_PERFMON_BASE, job->uargs.heatmap_mem & 0xFFFFFFF8);
	}
#endif /*                                 */

	MALI_DEBUG_PRINT(3, ("Mali PP: Starting job 0x%08X part %u/%u on PP core %s\n", job, sub_job + 1, mali_pp_job_get_sub_job_count(job), core->hw_core.description));

	/*                                                            */
	_mali_osk_write_mem_barrier();

	/*                                           */
	mali_hw_core_register_write_relaxed(&core->hw_core, MALI200_REG_ADDR_MGMT_CTRL_MGMT, MALI200_REG_VAL_CTRL_MGMT_START_RENDERING);

	/*                                                                */
	_mali_osk_write_mem_barrier();
}

u32 mali_pp_core_get_version(struct mali_pp_core *core)
{
	MALI_DEBUG_ASSERT_POINTER(core);
	return mali_hw_core_register_read(&core->hw_core, MALI200_REG_ADDR_MGMT_VERSION);
}

struct mali_pp_core* mali_pp_get_global_pp_core(u32 index)
{
	if (mali_global_num_pp_cores > index) {
		return mali_global_pp_cores[index];
	}

	return NULL;
}

u32 mali_pp_get_glob_num_pp_cores(void)
{
	return mali_global_num_pp_cores;
}

/*                                                           */
static void mali_pp_irq_probe_trigger(void *data)
{
	struct mali_pp_core *core = (struct mali_pp_core *)data;
	mali_hw_core_register_write(&core->hw_core, MALI200_REG_ADDR_MGMT_INT_MASK, MALI200_REG_VAL_IRQ_MASK_USED);
	mali_hw_core_register_write(&core->hw_core, MALI200_REG_ADDR_MGMT_INT_RAWSTAT, MALI200_REG_VAL_IRQ_FORCE_HANG);
	_mali_osk_mem_barrier();
}

static _mali_osk_errcode_t mali_pp_irq_probe_ack(void *data)
{
	struct mali_pp_core *core = (struct mali_pp_core *)data;
	u32 irq_readout;

	irq_readout = mali_hw_core_register_read(&core->hw_core, MALI200_REG_ADDR_MGMT_INT_STATUS);
	if (MALI200_REG_VAL_IRQ_FORCE_HANG & irq_readout) {
		mali_hw_core_register_write(&core->hw_core, MALI200_REG_ADDR_MGMT_INT_CLEAR, MALI200_REG_VAL_IRQ_FORCE_HANG);
		_mali_osk_mem_barrier();
		return _MALI_OSK_ERR_OK;
	}

	return _MALI_OSK_ERR_FAULT;
}


#if 0
static void mali_pp_print_registers(struct mali_pp_core *core)
{
	MALI_DEBUG_PRINT(2, ("Mali PP: Register MALI200_REG_ADDR_MGMT_VERSION = 0x%08X\n", mali_hw_core_register_read(&core->hw_core, MALI200_REG_ADDR_MGMT_VERSION)));
	MALI_DEBUG_PRINT(2, ("Mali PP: Register MALI200_REG_ADDR_MGMT_CURRENT_REND_LIST_ADDR = 0x%08X\n", mali_hw_core_register_read(&core->hw_core, MALI200_REG_ADDR_MGMT_CURRENT_REND_LIST_ADDR)));
	MALI_DEBUG_PRINT(2, ("Mali PP: Register MALI200_REG_ADDR_MGMT_STATUS = 0x%08X\n", mali_hw_core_register_read(&core->hw_core, MALI200_REG_ADDR_MGMT_STATUS)));
	MALI_DEBUG_PRINT(2, ("Mali PP: Register MALI200_REG_ADDR_MGMT_INT_RAWSTAT = 0x%08X\n", mali_hw_core_register_read(&core->hw_core, MALI200_REG_ADDR_MGMT_INT_RAWSTAT)));
	MALI_DEBUG_PRINT(2, ("Mali PP: Register MALI200_REG_ADDR_MGMT_INT_MASK = 0x%08X\n", mali_hw_core_register_read(&core->hw_core, MALI200_REG_ADDR_MGMT_INT_MASK)));
	MALI_DEBUG_PRINT(2, ("Mali PP: Register MALI200_REG_ADDR_MGMT_INT_STATUS = 0x%08X\n", mali_hw_core_register_read(&core->hw_core, MALI200_REG_ADDR_MGMT_INT_STATUS)));
	MALI_DEBUG_PRINT(2, ("Mali PP: Register MALI200_REG_ADDR_MGMT_BUS_ERROR_STATUS = 0x%08X\n", mali_hw_core_register_read(&core->hw_core, MALI200_REG_ADDR_MGMT_BUS_ERROR_STATUS)));
	MALI_DEBUG_PRINT(2, ("Mali PP: Register MALI200_REG_ADDR_MGMT_PERF_CNT_0_ENABLE = 0x%08X\n", mali_hw_core_register_read(&core->hw_core, MALI200_REG_ADDR_MGMT_PERF_CNT_0_ENABLE)));
	MALI_DEBUG_PRINT(2, ("Mali PP: Register MALI200_REG_ADDR_MGMT_PERF_CNT_0_SRC = 0x%08X\n", mali_hw_core_register_read(&core->hw_core, MALI200_REG_ADDR_MGMT_PERF_CNT_0_SRC)));
	MALI_DEBUG_PRINT(2, ("Mali PP: Register MALI200_REG_ADDR_MGMT_PERF_CNT_0_VALUE = 0x%08X\n", mali_hw_core_register_read(&core->hw_core, MALI200_REG_ADDR_MGMT_PERF_CNT_0_VALUE)));
	MALI_DEBUG_PRINT(2, ("Mali PP: Register MALI200_REG_ADDR_MGMT_PERF_CNT_1_ENABLE = 0x%08X\n", mali_hw_core_register_read(&core->hw_core, MALI200_REG_ADDR_MGMT_PERF_CNT_1_ENABLE)));
	MALI_DEBUG_PRINT(2, ("Mali PP: Register MALI200_REG_ADDR_MGMT_PERF_CNT_1_SRC = 0x%08X\n", mali_hw_core_register_read(&core->hw_core, MALI200_REG_ADDR_MGMT_PERF_CNT_1_SRC)));
	MALI_DEBUG_PRINT(2, ("Mali PP: Register MALI200_REG_ADDR_MGMT_PERF_CNT_1_VALUE = 0x%08X\n", mali_hw_core_register_read(&core->hw_core, MALI200_REG_ADDR_MGMT_PERF_CNT_1_VALUE)));
}
#endif

#if 0
void mali_pp_print_state(struct mali_pp_core *core)
{
	MALI_DEBUG_PRINT(2, ("Mali PP: State: 0x%08x\n", mali_hw_core_register_read(&core->hw_core, MALI200_REG_ADDR_MGMT_STATUS) ));
}
#endif

void mali_pp_update_performance_counters(struct mali_pp_core *parent, struct mali_pp_core *child, struct mali_pp_job *job, u32 subjob)
{
	u32 val0 = 0;
	u32 val1 = 0;
	u32 counter_src0 = mali_pp_job_get_perf_counter_src0(job, subjob);
	u32 counter_src1 = mali_pp_job_get_perf_counter_src1(job, subjob);
#if defined(CONFIG_MALI400_PROFILING)
	int counter_index = COUNTER_FP_0_C0 + (2 * child->core_id);
#endif

	if (MALI_HW_CORE_NO_COUNTER != counter_src0) {
		val0 = mali_hw_core_register_read(&child->hw_core, MALI200_REG_ADDR_MGMT_PERF_CNT_0_VALUE);
		mali_pp_job_set_perf_counter_value0(job, subjob, val0);

#if defined(CONFIG_MALI400_PROFILING)
		_mali_osk_profiling_report_hw_counter(counter_index, val0);
#endif
	}

	if (MALI_HW_CORE_NO_COUNTER != counter_src1) {
		val1 = mali_hw_core_register_read(&child->hw_core, MALI200_REG_ADDR_MGMT_PERF_CNT_1_VALUE);
		mali_pp_job_set_perf_counter_value1(job, subjob, val1);

#if defined(CONFIG_MALI400_PROFILING)
		_mali_osk_profiling_report_hw_counter(counter_index + 1, val1);
#endif
	}
}

#if MALI_STATE_TRACKING
u32 mali_pp_dump_state(struct mali_pp_core *core, char *buf, u32 size)
{
	int n = 0;

	n += _mali_osk_snprintf(buf + n, size - n, "\tPP #%d: %s\n", core->core_id, core->hw_core.description);

	return n;
}
#endif
