/*
 * OMAP5 thermal driver.
 *
 * Copyright (C) 2011-2012 Texas Instruments Inc.
 * Contact:
 *	Eduardo Valentin <eduardo.valentin@ti.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 */

#include "ti-thermal.h"
#include "ti-bandgap.h"
#include "omap5xxx-bandgap.h"

/*
                                                                      
                                                            
 */

/*
                                                             
 */
static struct temp_sensor_registers
omap5430_mpu_temp_sensor_registers = {
	.temp_sensor_ctrl = OMAP5430_TEMP_SENSOR_MPU_OFFSET,
	.bgap_tempsoff_mask = OMAP5430_BGAP_TEMPSOFF_MASK,
	.bgap_eocz_mask = OMAP5430_BGAP_TEMP_SENSOR_EOCZ_MASK,
	.bgap_dtemp_mask = OMAP5430_BGAP_TEMP_SENSOR_DTEMP_MASK,

	.bgap_mask_ctrl = OMAP5430_BGAP_CTRL_OFFSET,
	.mask_hot_mask = OMAP5430_MASK_HOT_MPU_MASK,
	.mask_cold_mask = OMAP5430_MASK_COLD_MPU_MASK,
	.mask_sidlemode_mask = OMAP5430_MASK_SIDLEMODE_MASK,
	.mask_counter_delay_mask = OMAP5430_MASK_COUNTER_DELAY_MASK,
	.mask_freeze_mask = OMAP5430_MASK_FREEZE_MPU_MASK,
	.mask_clear_mask = OMAP5430_MASK_CLEAR_MPU_MASK,
	.mask_clear_accum_mask = OMAP5430_MASK_CLEAR_ACCUM_MPU_MASK,


	.bgap_counter = OMAP5430_BGAP_CTRL_OFFSET,
	.counter_mask = OMAP5430_COUNTER_MASK,

	.bgap_threshold = OMAP5430_BGAP_THRESHOLD_MPU_OFFSET,
	.threshold_thot_mask = OMAP5430_T_HOT_MASK,
	.threshold_tcold_mask = OMAP5430_T_COLD_MASK,

	.tshut_threshold = OMAP5430_BGAP_TSHUT_MPU_OFFSET,
	.tshut_hot_mask = OMAP5430_TSHUT_HOT_MASK,
	.tshut_cold_mask = OMAP5430_TSHUT_COLD_MASK,

	.bgap_status = OMAP5430_BGAP_STATUS_OFFSET,
	.status_clean_stop_mask = 0x0,
	.status_bgap_alert_mask = OMAP5430_BGAP_ALERT_MASK,
	.status_hot_mask = OMAP5430_HOT_MPU_FLAG_MASK,
	.status_cold_mask = OMAP5430_COLD_MPU_FLAG_MASK,

	.bgap_cumul_dtemp = OMAP5430_BGAP_CUMUL_DTEMP_MPU_OFFSET,
	.ctrl_dtemp_0 = OMAP5430_BGAP_DTEMP_MPU_0_OFFSET,
	.ctrl_dtemp_1 = OMAP5430_BGAP_DTEMP_MPU_1_OFFSET,
	.ctrl_dtemp_2 = OMAP5430_BGAP_DTEMP_MPU_2_OFFSET,
	.ctrl_dtemp_3 = OMAP5430_BGAP_DTEMP_MPU_3_OFFSET,
	.ctrl_dtemp_4 = OMAP5430_BGAP_DTEMP_MPU_4_OFFSET,
	.bgap_efuse = OMAP5430_FUSE_OPP_BGAP_MPU,
};

/*
                                                             
 */
static struct temp_sensor_registers
omap5430_gpu_temp_sensor_registers = {
	.temp_sensor_ctrl = OMAP5430_TEMP_SENSOR_GPU_OFFSET,
	.bgap_tempsoff_mask = OMAP5430_BGAP_TEMPSOFF_MASK,
	.bgap_eocz_mask = OMAP5430_BGAP_TEMP_SENSOR_EOCZ_MASK,
	.bgap_dtemp_mask = OMAP5430_BGAP_TEMP_SENSOR_DTEMP_MASK,

	.bgap_mask_ctrl = OMAP5430_BGAP_CTRL_OFFSET,
	.mask_hot_mask = OMAP5430_MASK_HOT_GPU_MASK,
	.mask_cold_mask = OMAP5430_MASK_COLD_GPU_MASK,
	.mask_sidlemode_mask = OMAP5430_MASK_SIDLEMODE_MASK,
	.mask_counter_delay_mask = OMAP5430_MASK_COUNTER_DELAY_MASK,
	.mask_freeze_mask = OMAP5430_MASK_FREEZE_GPU_MASK,
	.mask_clear_mask = OMAP5430_MASK_CLEAR_GPU_MASK,
	.mask_clear_accum_mask = OMAP5430_MASK_CLEAR_ACCUM_GPU_MASK,

	.bgap_counter = OMAP5430_BGAP_CTRL_OFFSET,
	.counter_mask = OMAP5430_COUNTER_MASK,

	.bgap_threshold = OMAP5430_BGAP_THRESHOLD_GPU_OFFSET,
	.threshold_thot_mask = OMAP5430_T_HOT_MASK,
	.threshold_tcold_mask = OMAP5430_T_COLD_MASK,

	.tshut_threshold = OMAP5430_BGAP_TSHUT_GPU_OFFSET,
	.tshut_hot_mask = OMAP5430_TSHUT_HOT_MASK,
	.tshut_cold_mask = OMAP5430_TSHUT_COLD_MASK,

	.bgap_status = OMAP5430_BGAP_STATUS_OFFSET,
	.status_clean_stop_mask = 0x0,
	.status_bgap_alert_mask = OMAP5430_BGAP_ALERT_MASK,
	.status_hot_mask = OMAP5430_HOT_GPU_FLAG_MASK,
	.status_cold_mask = OMAP5430_COLD_GPU_FLAG_MASK,

	.bgap_cumul_dtemp = OMAP5430_BGAP_CUMUL_DTEMP_GPU_OFFSET,
	.ctrl_dtemp_0 = OMAP5430_BGAP_DTEMP_GPU_0_OFFSET,
	.ctrl_dtemp_1 = OMAP5430_BGAP_DTEMP_GPU_1_OFFSET,
	.ctrl_dtemp_2 = OMAP5430_BGAP_DTEMP_GPU_2_OFFSET,
	.ctrl_dtemp_3 = OMAP5430_BGAP_DTEMP_GPU_3_OFFSET,
	.ctrl_dtemp_4 = OMAP5430_BGAP_DTEMP_GPU_4_OFFSET,

	.bgap_efuse = OMAP5430_FUSE_OPP_BGAP_GPU,
};

/*
                                                              
 */
static struct temp_sensor_registers
omap5430_core_temp_sensor_registers = {
	.temp_sensor_ctrl = OMAP5430_TEMP_SENSOR_CORE_OFFSET,
	.bgap_tempsoff_mask = OMAP5430_BGAP_TEMPSOFF_MASK,
	.bgap_eocz_mask = OMAP5430_BGAP_TEMP_SENSOR_EOCZ_MASK,
	.bgap_dtemp_mask = OMAP5430_BGAP_TEMP_SENSOR_DTEMP_MASK,

	.bgap_mask_ctrl = OMAP5430_BGAP_CTRL_OFFSET,
	.mask_hot_mask = OMAP5430_MASK_HOT_CORE_MASK,
	.mask_cold_mask = OMAP5430_MASK_COLD_CORE_MASK,
	.mask_sidlemode_mask = OMAP5430_MASK_SIDLEMODE_MASK,
	.mask_counter_delay_mask = OMAP5430_MASK_COUNTER_DELAY_MASK,
	.mask_freeze_mask = OMAP5430_MASK_FREEZE_CORE_MASK,
	.mask_clear_mask = OMAP5430_MASK_CLEAR_CORE_MASK,
	.mask_clear_accum_mask = OMAP5430_MASK_CLEAR_ACCUM_CORE_MASK,

	.bgap_counter = OMAP5430_BGAP_CTRL_OFFSET,
	.counter_mask = OMAP5430_COUNTER_MASK,

	.bgap_threshold = OMAP5430_BGAP_THRESHOLD_CORE_OFFSET,
	.threshold_thot_mask = OMAP5430_T_HOT_MASK,
	.threshold_tcold_mask = OMAP5430_T_COLD_MASK,

	.tshut_threshold = OMAP5430_BGAP_TSHUT_CORE_OFFSET,
	.tshut_hot_mask = OMAP5430_TSHUT_HOT_MASK,
	.tshut_cold_mask = OMAP5430_TSHUT_COLD_MASK,

	.bgap_status = OMAP5430_BGAP_STATUS_OFFSET,
	.status_clean_stop_mask = 0x0,
	.status_bgap_alert_mask = OMAP5430_BGAP_ALERT_MASK,
	.status_hot_mask = OMAP5430_HOT_CORE_FLAG_MASK,
	.status_cold_mask = OMAP5430_COLD_CORE_FLAG_MASK,

	.bgap_cumul_dtemp = OMAP5430_BGAP_CUMUL_DTEMP_CORE_OFFSET,
	.ctrl_dtemp_0 = OMAP5430_BGAP_DTEMP_CORE_0_OFFSET,
	.ctrl_dtemp_1 = OMAP5430_BGAP_DTEMP_CORE_1_OFFSET,
	.ctrl_dtemp_2 = OMAP5430_BGAP_DTEMP_CORE_2_OFFSET,
	.ctrl_dtemp_3 = OMAP5430_BGAP_DTEMP_CORE_3_OFFSET,
	.ctrl_dtemp_4 = OMAP5430_BGAP_DTEMP_CORE_4_OFFSET,

	.bgap_efuse = OMAP5430_FUSE_OPP_BGAP_CORE,
};

/*                                                           */
static struct temp_sensor_data omap5430_mpu_temp_sensor_data = {
	.tshut_hot = OMAP5430_MPU_TSHUT_HOT,
	.tshut_cold = OMAP5430_MPU_TSHUT_COLD,
	.t_hot = OMAP5430_MPU_T_HOT,
	.t_cold = OMAP5430_MPU_T_COLD,
	.min_freq = OMAP5430_MPU_MIN_FREQ,
	.max_freq = OMAP5430_MPU_MAX_FREQ,
	.max_temp = OMAP5430_MPU_MAX_TEMP,
	.min_temp = OMAP5430_MPU_MIN_TEMP,
	.hyst_val = OMAP5430_MPU_HYST_VAL,
	.update_int1 = 1000,
	.update_int2 = 2000,
};

/*                                                           */
static struct temp_sensor_data omap5430_gpu_temp_sensor_data = {
	.tshut_hot = OMAP5430_GPU_TSHUT_HOT,
	.tshut_cold = OMAP5430_GPU_TSHUT_COLD,
	.t_hot = OMAP5430_GPU_T_HOT,
	.t_cold = OMAP5430_GPU_T_COLD,
	.min_freq = OMAP5430_GPU_MIN_FREQ,
	.max_freq = OMAP5430_GPU_MAX_FREQ,
	.max_temp = OMAP5430_GPU_MAX_TEMP,
	.min_temp = OMAP5430_GPU_MIN_TEMP,
	.hyst_val = OMAP5430_GPU_HYST_VAL,
	.update_int1 = 1000,
	.update_int2 = 2000,
};

/*                                                            */
static struct temp_sensor_data omap5430_core_temp_sensor_data = {
	.tshut_hot = OMAP5430_CORE_TSHUT_HOT,
	.tshut_cold = OMAP5430_CORE_TSHUT_COLD,
	.t_hot = OMAP5430_CORE_T_HOT,
	.t_cold = OMAP5430_CORE_T_COLD,
	.min_freq = OMAP5430_CORE_MIN_FREQ,
	.max_freq = OMAP5430_CORE_MAX_FREQ,
	.max_temp = OMAP5430_CORE_MAX_TEMP,
	.min_temp = OMAP5430_CORE_MIN_TEMP,
	.hyst_val = OMAP5430_CORE_HYST_VAL,
	.update_int1 = 1000,
	.update_int2 = 2000,
};

/*
                                                              
                                  
 */
static int
omap5430_adc_to_temp[
	OMAP5430_ADC_END_VALUE - OMAP5430_ADC_START_VALUE + 1] = {
	/*                 */
	-40000, -40000, -40000, -40000, -39800, -39400, -39000, -38600, -38200,
	-37800,
	/*                 */
	-37400, -37000, -36600, -36200, -35800, -35300, -34700, -34200, -33800,
	-33400,
	/*                 */
	-33000, -32600, -32200, -31800, -31400, -31000, -30600, -30200, -29800,
	-29400,
	/*                 */
	-29000, -28600, -28200, -27700, -27100, -26600, -26200, -25800, -25400,
	-25000,
	/*                 */
	-24600, -24200, -23800, -23400, -23000, -22600, -22200, -21600, -21400,
	-21000,
	/*                 */
	-20500, -19900, -19400, -19000, -18600, -18200, -17800, -17400, -17000,
	-16600,
	/*                 */
	-16200, -15800, -15400, -15000, -14600, -14200, -13800,	-13400, -13000,
	-12500,
	/*                 */
	-11900, -11400, -11000, -10600, -10200, -9800, -9400, -9000, -8600,
	-8200,
	/*                 */
	-7800, -7400, -7000, -6600, -6200, -5800, -5400, -5000, -4500, -3900,
	/*                 */
	-3400, -3000, -2600, -2200, -1800, -1400, -1000, -600, -200, 200,
	/*                 */
	600, 1000, 1400, 1800, 2200, 2600, 3000, 3400, 3900, 4500,
	/*                 */
	5000, 5400, 5800, 6200, 6600, 7000, 7400, 7800, 8200, 8600,
	/*                 */
	9000, 9400, 9800, 10200, 10600, 11000, 11400, 11800, 12200, 12700,
	/*                 */
	13300, 13800, 14200, 14600, 15000, 15400, 15800, 16200, 16600, 17000,
	/*                 */
	17400, 17800, 18200, 18600, 19000, 19400, 19800, 20200, 20600, 21100,
	/*                 */
	21400, 21900, 22500, 23000, 23400, 23800, 24200, 24600, 25000, 25400,
	/*                 */
	25800, 26200, 26600, 27000, 27400, 27800, 28200, 28600, 29000, 29400,
	/*                 */
	29800, 30200, 30600, 31000, 31400, 31900, 32500, 33000, 33400, 33800,
	/*                 */
	34200, 34600, 35000, 35400, 35800, 36200, 36600, 37000, 37400, 37800,
	/*                 */
	38200, 38600, 39000, 39400, 39800, 40200, 40600, 41000, 41400, 41800,
	/*                 */
	42200, 42600, 43100, 43700, 44200, 44600, 45000, 45400, 45800, 46200,
	/*                 */
	46600, 47000, 47400, 47800, 48200, 48600, 49000, 49400, 49800, 50200,
	/*                 */
	50600, 51000, 51400, 51800, 52200, 52600, 53000, 53400, 53800, 54200,
	/*                 */
	54600, 55000, 55400, 55900, 56500, 57000, 57400, 57800, 58200, 58600,
	/*                 */
	59000, 59400, 59800, 60200, 60600, 61000, 61400, 61800, 62200, 62600,
	/*                 */
	63000, 63400, 63800, 64200, 64600, 65000, 65400, 65800, 66200, 66600,
	/*                 */
	67000, 67400, 67800, 68200, 68600, 69000, 69400, 69800, 70200, 70600,
	/*                 */
	71000, 71500, 72100, 72600, 73000, 73400, 73800, 74200, 74600, 75000,
	/*                 */
	75400, 75800, 76200, 76600, 77000, 77400, 77800, 78200, 78600, 79000,
	/*                 */
	79400, 79800, 80200, 80600, 81000, 81400, 81800, 82200, 82600, 83000,
	/*                 */
	83400, 83800, 84200, 84600, 85000, 85400, 85800, 86200, 86600, 87000,
	/*                 */
	87400, 87800, 88200, 88600, 89000, 89400, 89800, 90200, 90600, 91000,
	/*                 */
	91400, 91800, 92200, 92600, 93000, 93400, 93800, 94200, 94600, 95000,
	/*                 */
	95400, 95800, 96200, 96600, 97000, 97500, 98100, 98600, 99000, 99400,
	/*                 */
	99800, 100200, 100600, 101000, 101400, 101800, 102200, 102600, 103000,
	103400,
	/*                 */
	103800, 104200, 104600, 105000, 105400, 105800, 106200, 106600, 107000,
	107400,
	/*                 */
	107800, 108200, 108600, 109000, 109400, 109800, 110200, 110600, 111000,
	111400,
	/*                 */
	111800, 112200, 112600, 113000, 113400, 113800, 114200, 114600, 115000,
	115400,
	/*                 */
	115800, 116200, 116600, 117000, 117400, 117800, 118200, 118600, 119000,
	119400,
	/*                 */
	119800, 120200, 120600, 121000, 121400, 121800, 122400, 122600, 123000,
	123400,
	/*                 */
	123800, 1242000, 124600, 124900, 125000, 125000,
};

/*                     */
const struct ti_bandgap_data omap5430_data = {
	.features = TI_BANDGAP_FEATURE_TSHUT_CONFIG |
			TI_BANDGAP_FEATURE_FREEZE_BIT |
			TI_BANDGAP_FEATURE_TALERT |
			TI_BANDGAP_FEATURE_COUNTER_DELAY |
			TI_BANDGAP_FEATURE_HISTORY_BUFFER,
	.fclock_name = "l3instr_ts_gclk_div",
	.div_ck_name = "l3instr_ts_gclk_div",
	.conv_table = omap5430_adc_to_temp,
	.adc_start_val = OMAP5430_ADC_START_VALUE,
	.adc_end_val = OMAP5430_ADC_END_VALUE,
	.expose_sensor = ti_thermal_expose_sensor,
	.remove_sensor = ti_thermal_remove_sensor,
	.report_temperature = ti_thermal_report_sensor_temperature,
	.sensors = {
		{
		.registers = &omap5430_mpu_temp_sensor_registers,
		.ts_data = &omap5430_mpu_temp_sensor_data,
		.domain = "cpu",
		.register_cooling = ti_thermal_register_cpu_cooling,
		.unregister_cooling = ti_thermal_unregister_cpu_cooling,
		.slope = OMAP_GRADIENT_SLOPE_5430_CPU,
		.constant = OMAP_GRADIENT_CONST_5430_CPU,
		.slope_pcb = OMAP_GRADIENT_SLOPE_W_PCB_5430_CPU,
		.constant_pcb = OMAP_GRADIENT_CONST_W_PCB_5430_CPU,
		},
		{
		.registers = &omap5430_gpu_temp_sensor_registers,
		.ts_data = &omap5430_gpu_temp_sensor_data,
		.domain = "gpu",
		.slope = OMAP_GRADIENT_SLOPE_5430_GPU,
		.constant = OMAP_GRADIENT_CONST_5430_GPU,
		.slope_pcb = OMAP_GRADIENT_SLOPE_W_PCB_5430_GPU,
		.constant_pcb = OMAP_GRADIENT_CONST_W_PCB_5430_GPU,
		},
		{
		.registers = &omap5430_core_temp_sensor_registers,
		.ts_data = &omap5430_core_temp_sensor_data,
		.domain = "core",
		},
	},
	.sensor_count = 3,
};
