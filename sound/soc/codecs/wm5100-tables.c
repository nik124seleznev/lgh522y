/*
 * wm5100-tables.c  --  WM5100 ALSA SoC Audio driver data
 *
 * Copyright 2011-2 Wolfson Microelectronics plc
 *
 * Author: Mark Brown <broonie@opensource.wolfsonmicro.com>
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "wm5100.h"

bool wm5100_volatile_register(struct device *dev, unsigned int reg)
{
	switch (reg) {
	case WM5100_SOFTWARE_RESET:
	case WM5100_DEVICE_REVISION:
	case WM5100_FX_CTRL:
	case WM5100_INTERRUPT_STATUS_1:
	case WM5100_INTERRUPT_STATUS_2:
	case WM5100_INTERRUPT_STATUS_3:
	case WM5100_INTERRUPT_STATUS_4:
	case WM5100_INTERRUPT_RAW_STATUS_2:
	case WM5100_INTERRUPT_RAW_STATUS_3:
	case WM5100_INTERRUPT_RAW_STATUS_4:
	case WM5100_OUTPUT_STATUS_1:
	case WM5100_OUTPUT_STATUS_2:
	case WM5100_INPUT_ENABLES_STATUS:
	case WM5100_MIC_DETECT_3:
		return 1;
	default:
		if ((reg >= WM5100_DSP1_PM_0 && reg <= WM5100_DSP1_PM_1535) ||
		    (reg >= WM5100_DSP1_ZM_0 && reg <= WM5100_DSP1_ZM_2047) ||
		    (reg >= WM5100_DSP1_DM_0 && reg <= WM5100_DSP1_DM_511) ||
		    (reg >= WM5100_DSP2_PM_0 && reg <= WM5100_DSP2_PM_1535) ||
		    (reg >= WM5100_DSP2_ZM_0 && reg <= WM5100_DSP2_ZM_2047) ||
		    (reg >= WM5100_DSP2_DM_0 && reg <= WM5100_DSP2_DM_511) ||
		    (reg >= WM5100_DSP3_PM_0 && reg <= WM5100_DSP3_PM_1535) ||
		    (reg >= WM5100_DSP3_ZM_0 && reg <= WM5100_DSP3_ZM_2047) ||
		    (reg >= WM5100_DSP3_DM_0 && reg <= WM5100_DSP3_DM_511))
			return 1;
		else
			return 0;
	}
}

bool wm5100_readable_register(struct device *dev, unsigned int reg)
{
	switch (reg) {
	case WM5100_SOFTWARE_RESET:
	case WM5100_DEVICE_REVISION:
	case WM5100_CTRL_IF_1:
	case WM5100_TONE_GENERATOR_1:
	case WM5100_PWM_DRIVE_1:
	case WM5100_PWM_DRIVE_2:
	case WM5100_PWM_DRIVE_3:
	case WM5100_CLOCKING_1:
	case WM5100_CLOCKING_3:
	case WM5100_CLOCKING_4:
	case WM5100_CLOCKING_5:
	case WM5100_CLOCKING_6:
	case WM5100_CLOCKING_7:
	case WM5100_CLOCKING_8:
	case WM5100_ASRC_ENABLE:
	case WM5100_ASRC_STATUS:
	case WM5100_ASRC_RATE1:
	case WM5100_ISRC_1_CTRL_1:
	case WM5100_ISRC_1_CTRL_2:
	case WM5100_ISRC_2_CTRL1:
	case WM5100_ISRC_2_CTRL_2:
	case WM5100_FLL1_CONTROL_1:
	case WM5100_FLL1_CONTROL_2:
	case WM5100_FLL1_CONTROL_3:
	case WM5100_FLL1_CONTROL_5:
	case WM5100_FLL1_CONTROL_6:
	case WM5100_FLL1_EFS_1:
	case WM5100_FLL2_CONTROL_1:
	case WM5100_FLL2_CONTROL_2:
	case WM5100_FLL2_CONTROL_3:
	case WM5100_FLL2_CONTROL_5:
	case WM5100_FLL2_CONTROL_6:
	case WM5100_FLL2_EFS_1:
	case WM5100_MIC_CHARGE_PUMP_1:
	case WM5100_MIC_CHARGE_PUMP_2:
	case WM5100_HP_CHARGE_PUMP_1:
	case WM5100_LDO1_CONTROL:
	case WM5100_MIC_BIAS_CTRL_1:
	case WM5100_MIC_BIAS_CTRL_2:
	case WM5100_MIC_BIAS_CTRL_3:
	case WM5100_ACCESSORY_DETECT_MODE_1:
	case WM5100_HEADPHONE_DETECT_1:
	case WM5100_HEADPHONE_DETECT_2:
	case WM5100_MIC_DETECT_1:
	case WM5100_MIC_DETECT_2:
	case WM5100_MIC_DETECT_3:
	case WM5100_MISC_CONTROL:
	case WM5100_INPUT_ENABLES:
	case WM5100_INPUT_ENABLES_STATUS:
	case WM5100_IN1L_CONTROL:
	case WM5100_IN1R_CONTROL:
	case WM5100_IN2L_CONTROL:
	case WM5100_IN2R_CONTROL:
	case WM5100_IN3L_CONTROL:
	case WM5100_IN3R_CONTROL:
	case WM5100_IN4L_CONTROL:
	case WM5100_IN4R_CONTROL:
	case WM5100_RXANC_SRC:
	case WM5100_INPUT_VOLUME_RAMP:
	case WM5100_ADC_DIGITAL_VOLUME_1L:
	case WM5100_ADC_DIGITAL_VOLUME_1R:
	case WM5100_ADC_DIGITAL_VOLUME_2L:
	case WM5100_ADC_DIGITAL_VOLUME_2R:
	case WM5100_ADC_DIGITAL_VOLUME_3L:
	case WM5100_ADC_DIGITAL_VOLUME_3R:
	case WM5100_ADC_DIGITAL_VOLUME_4L:
	case WM5100_ADC_DIGITAL_VOLUME_4R:
	case WM5100_OUTPUT_ENABLES_2:
	case WM5100_OUTPUT_STATUS_1:
	case WM5100_OUTPUT_STATUS_2:
	case WM5100_CHANNEL_ENABLES_1:
	case WM5100_OUT_VOLUME_1L:
	case WM5100_OUT_VOLUME_1R:
	case WM5100_DAC_VOLUME_LIMIT_1L:
	case WM5100_DAC_VOLUME_LIMIT_1R:
	case WM5100_OUT_VOLUME_2L:
	case WM5100_OUT_VOLUME_2R:
	case WM5100_DAC_VOLUME_LIMIT_2L:
	case WM5100_DAC_VOLUME_LIMIT_2R:
	case WM5100_OUT_VOLUME_3L:
	case WM5100_OUT_VOLUME_3R:
	case WM5100_DAC_VOLUME_LIMIT_3L:
	case WM5100_DAC_VOLUME_LIMIT_3R:
	case WM5100_OUT_VOLUME_4L:
	case WM5100_OUT_VOLUME_4R:
	case WM5100_DAC_VOLUME_LIMIT_5L:
	case WM5100_DAC_VOLUME_LIMIT_5R:
	case WM5100_DAC_VOLUME_LIMIT_6L:
	case WM5100_DAC_VOLUME_LIMIT_6R:
	case WM5100_DAC_AEC_CONTROL_1:
	case WM5100_OUTPUT_VOLUME_RAMP:
	case WM5100_DAC_DIGITAL_VOLUME_1L:
	case WM5100_DAC_DIGITAL_VOLUME_1R:
	case WM5100_DAC_DIGITAL_VOLUME_2L:
	case WM5100_DAC_DIGITAL_VOLUME_2R:
	case WM5100_DAC_DIGITAL_VOLUME_3L:
	case WM5100_DAC_DIGITAL_VOLUME_3R:
	case WM5100_DAC_DIGITAL_VOLUME_4L:
	case WM5100_DAC_DIGITAL_VOLUME_4R:
	case WM5100_DAC_DIGITAL_VOLUME_5L:
	case WM5100_DAC_DIGITAL_VOLUME_5R:
	case WM5100_DAC_DIGITAL_VOLUME_6L:
	case WM5100_DAC_DIGITAL_VOLUME_6R:
	case WM5100_PDM_SPK1_CTRL_1:
	case WM5100_PDM_SPK1_CTRL_2:
	case WM5100_PDM_SPK2_CTRL_1:
	case WM5100_PDM_SPK2_CTRL_2:
	case WM5100_AUDIO_IF_1_1:
	case WM5100_AUDIO_IF_1_2:
	case WM5100_AUDIO_IF_1_3:
	case WM5100_AUDIO_IF_1_4:
	case WM5100_AUDIO_IF_1_5:
	case WM5100_AUDIO_IF_1_6:
	case WM5100_AUDIO_IF_1_7:
	case WM5100_AUDIO_IF_1_8:
	case WM5100_AUDIO_IF_1_9:
	case WM5100_AUDIO_IF_1_10:
	case WM5100_AUDIO_IF_1_11:
	case WM5100_AUDIO_IF_1_12:
	case WM5100_AUDIO_IF_1_13:
	case WM5100_AUDIO_IF_1_14:
	case WM5100_AUDIO_IF_1_15:
	case WM5100_AUDIO_IF_1_16:
	case WM5100_AUDIO_IF_1_17:
	case WM5100_AUDIO_IF_1_18:
	case WM5100_AUDIO_IF_1_19:
	case WM5100_AUDIO_IF_1_20:
	case WM5100_AUDIO_IF_1_21:
	case WM5100_AUDIO_IF_1_22:
	case WM5100_AUDIO_IF_1_23:
	case WM5100_AUDIO_IF_1_24:
	case WM5100_AUDIO_IF_1_25:
	case WM5100_AUDIO_IF_1_26:
	case WM5100_AUDIO_IF_1_27:
	case WM5100_AUDIO_IF_2_1:
	case WM5100_AUDIO_IF_2_2:
	case WM5100_AUDIO_IF_2_3:
	case WM5100_AUDIO_IF_2_4:
	case WM5100_AUDIO_IF_2_5:
	case WM5100_AUDIO_IF_2_6:
	case WM5100_AUDIO_IF_2_7:
	case WM5100_AUDIO_IF_2_8:
	case WM5100_AUDIO_IF_2_9:
	case WM5100_AUDIO_IF_2_10:
	case WM5100_AUDIO_IF_2_11:
	case WM5100_AUDIO_IF_2_18:
	case WM5100_AUDIO_IF_2_19:
	case WM5100_AUDIO_IF_2_26:
	case WM5100_AUDIO_IF_2_27:
	case WM5100_AUDIO_IF_3_1:
	case WM5100_AUDIO_IF_3_2:
	case WM5100_AUDIO_IF_3_3:
	case WM5100_AUDIO_IF_3_4:
	case WM5100_AUDIO_IF_3_5:
	case WM5100_AUDIO_IF_3_6:
	case WM5100_AUDIO_IF_3_7:
	case WM5100_AUDIO_IF_3_8:
	case WM5100_AUDIO_IF_3_9:
	case WM5100_AUDIO_IF_3_10:
	case WM5100_AUDIO_IF_3_11:
	case WM5100_AUDIO_IF_3_18:
	case WM5100_AUDIO_IF_3_19:
	case WM5100_AUDIO_IF_3_26:
	case WM5100_AUDIO_IF_3_27:
	case WM5100_PWM1MIX_INPUT_1_SOURCE:
	case WM5100_PWM1MIX_INPUT_1_VOLUME:
	case WM5100_PWM1MIX_INPUT_2_SOURCE:
	case WM5100_PWM1MIX_INPUT_2_VOLUME:
	case WM5100_PWM1MIX_INPUT_3_SOURCE:
	case WM5100_PWM1MIX_INPUT_3_VOLUME:
	case WM5100_PWM1MIX_INPUT_4_SOURCE:
	case WM5100_PWM1MIX_INPUT_4_VOLUME:
	case WM5100_PWM2MIX_INPUT_1_SOURCE:
	case WM5100_PWM2MIX_INPUT_1_VOLUME:
	case WM5100_PWM2MIX_INPUT_2_SOURCE:
	case WM5100_PWM2MIX_INPUT_2_VOLUME:
	case WM5100_PWM2MIX_INPUT_3_SOURCE:
	case WM5100_PWM2MIX_INPUT_3_VOLUME:
	case WM5100_PWM2MIX_INPUT_4_SOURCE:
	case WM5100_PWM2MIX_INPUT_4_VOLUME:
	case WM5100_OUT1LMIX_INPUT_1_SOURCE:
	case WM5100_OUT1LMIX_INPUT_1_VOLUME:
	case WM5100_OUT1LMIX_INPUT_2_SOURCE:
	case WM5100_OUT1LMIX_INPUT_2_VOLUME:
	case WM5100_OUT1LMIX_INPUT_3_SOURCE:
	case WM5100_OUT1LMIX_INPUT_3_VOLUME:
	case WM5100_OUT1LMIX_INPUT_4_SOURCE:
	case WM5100_OUT1LMIX_INPUT_4_VOLUME:
	case WM5100_OUT1RMIX_INPUT_1_SOURCE:
	case WM5100_OUT1RMIX_INPUT_1_VOLUME:
	case WM5100_OUT1RMIX_INPUT_2_SOURCE:
	case WM5100_OUT1RMIX_INPUT_2_VOLUME:
	case WM5100_OUT1RMIX_INPUT_3_SOURCE:
	case WM5100_OUT1RMIX_INPUT_3_VOLUME:
	case WM5100_OUT1RMIX_INPUT_4_SOURCE:
	case WM5100_OUT1RMIX_INPUT_4_VOLUME:
	case WM5100_OUT2LMIX_INPUT_1_SOURCE:
	case WM5100_OUT2LMIX_INPUT_1_VOLUME:
	case WM5100_OUT2LMIX_INPUT_2_SOURCE:
	case WM5100_OUT2LMIX_INPUT_2_VOLUME:
	case WM5100_OUT2LMIX_INPUT_3_SOURCE:
	case WM5100_OUT2LMIX_INPUT_3_VOLUME:
	case WM5100_OUT2LMIX_INPUT_4_SOURCE:
	case WM5100_OUT2LMIX_INPUT_4_VOLUME:
	case WM5100_OUT2RMIX_INPUT_1_SOURCE:
	case WM5100_OUT2RMIX_INPUT_1_VOLUME:
	case WM5100_OUT2RMIX_INPUT_2_SOURCE:
	case WM5100_OUT2RMIX_INPUT_2_VOLUME:
	case WM5100_OUT2RMIX_INPUT_3_SOURCE:
	case WM5100_OUT2RMIX_INPUT_3_VOLUME:
	case WM5100_OUT2RMIX_INPUT_4_SOURCE:
	case WM5100_OUT2RMIX_INPUT_4_VOLUME:
	case WM5100_OUT3LMIX_INPUT_1_SOURCE:
	case WM5100_OUT3LMIX_INPUT_1_VOLUME:
	case WM5100_OUT3LMIX_INPUT_2_SOURCE:
	case WM5100_OUT3LMIX_INPUT_2_VOLUME:
	case WM5100_OUT3LMIX_INPUT_3_SOURCE:
	case WM5100_OUT3LMIX_INPUT_3_VOLUME:
	case WM5100_OUT3LMIX_INPUT_4_SOURCE:
	case WM5100_OUT3LMIX_INPUT_4_VOLUME:
	case WM5100_OUT3RMIX_INPUT_1_SOURCE:
	case WM5100_OUT3RMIX_INPUT_1_VOLUME:
	case WM5100_OUT3RMIX_INPUT_2_SOURCE:
	case WM5100_OUT3RMIX_INPUT_2_VOLUME:
	case WM5100_OUT3RMIX_INPUT_3_SOURCE:
	case WM5100_OUT3RMIX_INPUT_3_VOLUME:
	case WM5100_OUT3RMIX_INPUT_4_SOURCE:
	case WM5100_OUT3RMIX_INPUT_4_VOLUME:
	case WM5100_OUT4LMIX_INPUT_1_SOURCE:
	case WM5100_OUT4LMIX_INPUT_1_VOLUME:
	case WM5100_OUT4LMIX_INPUT_2_SOURCE:
	case WM5100_OUT4LMIX_INPUT_2_VOLUME:
	case WM5100_OUT4LMIX_INPUT_3_SOURCE:
	case WM5100_OUT4LMIX_INPUT_3_VOLUME:
	case WM5100_OUT4LMIX_INPUT_4_SOURCE:
	case WM5100_OUT4LMIX_INPUT_4_VOLUME:
	case WM5100_OUT4RMIX_INPUT_1_SOURCE:
	case WM5100_OUT4RMIX_INPUT_1_VOLUME:
	case WM5100_OUT4RMIX_INPUT_2_SOURCE:
	case WM5100_OUT4RMIX_INPUT_2_VOLUME:
	case WM5100_OUT4RMIX_INPUT_3_SOURCE:
	case WM5100_OUT4RMIX_INPUT_3_VOLUME:
	case WM5100_OUT4RMIX_INPUT_4_SOURCE:
	case WM5100_OUT4RMIX_INPUT_4_VOLUME:
	case WM5100_OUT5LMIX_INPUT_1_SOURCE:
	case WM5100_OUT5LMIX_INPUT_1_VOLUME:
	case WM5100_OUT5LMIX_INPUT_2_SOURCE:
	case WM5100_OUT5LMIX_INPUT_2_VOLUME:
	case WM5100_OUT5LMIX_INPUT_3_SOURCE:
	case WM5100_OUT5LMIX_INPUT_3_VOLUME:
	case WM5100_OUT5LMIX_INPUT_4_SOURCE:
	case WM5100_OUT5LMIX_INPUT_4_VOLUME:
	case WM5100_OUT5RMIX_INPUT_1_SOURCE:
	case WM5100_OUT5RMIX_INPUT_1_VOLUME:
	case WM5100_OUT5RMIX_INPUT_2_SOURCE:
	case WM5100_OUT5RMIX_INPUT_2_VOLUME:
	case WM5100_OUT5RMIX_INPUT_3_SOURCE:
	case WM5100_OUT5RMIX_INPUT_3_VOLUME:
	case WM5100_OUT5RMIX_INPUT_4_SOURCE:
	case WM5100_OUT5RMIX_INPUT_4_VOLUME:
	case WM5100_OUT6LMIX_INPUT_1_SOURCE:
	case WM5100_OUT6LMIX_INPUT_1_VOLUME:
	case WM5100_OUT6LMIX_INPUT_2_SOURCE:
	case WM5100_OUT6LMIX_INPUT_2_VOLUME:
	case WM5100_OUT6LMIX_INPUT_3_SOURCE:
	case WM5100_OUT6LMIX_INPUT_3_VOLUME:
	case WM5100_OUT6LMIX_INPUT_4_SOURCE:
	case WM5100_OUT6LMIX_INPUT_4_VOLUME:
	case WM5100_OUT6RMIX_INPUT_1_SOURCE:
	case WM5100_OUT6RMIX_INPUT_1_VOLUME:
	case WM5100_OUT6RMIX_INPUT_2_SOURCE:
	case WM5100_OUT6RMIX_INPUT_2_VOLUME:
	case WM5100_OUT6RMIX_INPUT_3_SOURCE:
	case WM5100_OUT6RMIX_INPUT_3_VOLUME:
	case WM5100_OUT6RMIX_INPUT_4_SOURCE:
	case WM5100_OUT6RMIX_INPUT_4_VOLUME:
	case WM5100_AIF1TX1MIX_INPUT_1_SOURCE:
	case WM5100_AIF1TX1MIX_INPUT_1_VOLUME:
	case WM5100_AIF1TX1MIX_INPUT_2_SOURCE:
	case WM5100_AIF1TX1MIX_INPUT_2_VOLUME:
	case WM5100_AIF1TX1MIX_INPUT_3_SOURCE:
	case WM5100_AIF1TX1MIX_INPUT_3_VOLUME:
	case WM5100_AIF1TX1MIX_INPUT_4_SOURCE:
	case WM5100_AIF1TX1MIX_INPUT_4_VOLUME:
	case WM5100_AIF1TX2MIX_INPUT_1_SOURCE:
	case WM5100_AIF1TX2MIX_INPUT_1_VOLUME:
	case WM5100_AIF1TX2MIX_INPUT_2_SOURCE:
	case WM5100_AIF1TX2MIX_INPUT_2_VOLUME:
	case WM5100_AIF1TX2MIX_INPUT_3_SOURCE:
	case WM5100_AIF1TX2MIX_INPUT_3_VOLUME:
	case WM5100_AIF1TX2MIX_INPUT_4_SOURCE:
	case WM5100_AIF1TX2MIX_INPUT_4_VOLUME:
	case WM5100_AIF1TX3MIX_INPUT_1_SOURCE:
	case WM5100_AIF1TX3MIX_INPUT_1_VOLUME:
	case WM5100_AIF1TX3MIX_INPUT_2_SOURCE:
	case WM5100_AIF1TX3MIX_INPUT_2_VOLUME:
	case WM5100_AIF1TX3MIX_INPUT_3_SOURCE:
	case WM5100_AIF1TX3MIX_INPUT_3_VOLUME:
	case WM5100_AIF1TX3MIX_INPUT_4_SOURCE:
	case WM5100_AIF1TX3MIX_INPUT_4_VOLUME:
	case WM5100_AIF1TX4MIX_INPUT_1_SOURCE:
	case WM5100_AIF1TX4MIX_INPUT_1_VOLUME:
	case WM5100_AIF1TX4MIX_INPUT_2_SOURCE:
	case WM5100_AIF1TX4MIX_INPUT_2_VOLUME:
	case WM5100_AIF1TX4MIX_INPUT_3_SOURCE:
	case WM5100_AIF1TX4MIX_INPUT_3_VOLUME:
	case WM5100_AIF1TX4MIX_INPUT_4_SOURCE:
	case WM5100_AIF1TX4MIX_INPUT_4_VOLUME:
	case WM5100_AIF1TX5MIX_INPUT_1_SOURCE:
	case WM5100_AIF1TX5MIX_INPUT_1_VOLUME:
	case WM5100_AIF1TX5MIX_INPUT_2_SOURCE:
	case WM5100_AIF1TX5MIX_INPUT_2_VOLUME:
	case WM5100_AIF1TX5MIX_INPUT_3_SOURCE:
	case WM5100_AIF1TX5MIX_INPUT_3_VOLUME:
	case WM5100_AIF1TX5MIX_INPUT_4_SOURCE:
	case WM5100_AIF1TX5MIX_INPUT_4_VOLUME:
	case WM5100_AIF1TX6MIX_INPUT_1_SOURCE:
	case WM5100_AIF1TX6MIX_INPUT_1_VOLUME:
	case WM5100_AIF1TX6MIX_INPUT_2_SOURCE:
	case WM5100_AIF1TX6MIX_INPUT_2_VOLUME:
	case WM5100_AIF1TX6MIX_INPUT_3_SOURCE:
	case WM5100_AIF1TX6MIX_INPUT_3_VOLUME:
	case WM5100_AIF1TX6MIX_INPUT_4_SOURCE:
	case WM5100_AIF1TX6MIX_INPUT_4_VOLUME:
	case WM5100_AIF1TX7MIX_INPUT_1_SOURCE:
	case WM5100_AIF1TX7MIX_INPUT_1_VOLUME:
	case WM5100_AIF1TX7MIX_INPUT_2_SOURCE:
	case WM5100_AIF1TX7MIX_INPUT_2_VOLUME:
	case WM5100_AIF1TX7MIX_INPUT_3_SOURCE:
	case WM5100_AIF1TX7MIX_INPUT_3_VOLUME:
	case WM5100_AIF1TX7MIX_INPUT_4_SOURCE:
	case WM5100_AIF1TX7MIX_INPUT_4_VOLUME:
	case WM5100_AIF1TX8MIX_INPUT_1_SOURCE:
	case WM5100_AIF1TX8MIX_INPUT_1_VOLUME:
	case WM5100_AIF1TX8MIX_INPUT_2_SOURCE:
	case WM5100_AIF1TX8MIX_INPUT_2_VOLUME:
	case WM5100_AIF1TX8MIX_INPUT_3_SOURCE:
	case WM5100_AIF1TX8MIX_INPUT_3_VOLUME:
	case WM5100_AIF1TX8MIX_INPUT_4_SOURCE:
	case WM5100_AIF1TX8MIX_INPUT_4_VOLUME:
	case WM5100_AIF2TX1MIX_INPUT_1_SOURCE:
	case WM5100_AIF2TX1MIX_INPUT_1_VOLUME:
	case WM5100_AIF2TX1MIX_INPUT_2_SOURCE:
	case WM5100_AIF2TX1MIX_INPUT_2_VOLUME:
	case WM5100_AIF2TX1MIX_INPUT_3_SOURCE:
	case WM5100_AIF2TX1MIX_INPUT_3_VOLUME:
	case WM5100_AIF2TX1MIX_INPUT_4_SOURCE:
	case WM5100_AIF2TX1MIX_INPUT_4_VOLUME:
	case WM5100_AIF2TX2MIX_INPUT_1_SOURCE:
	case WM5100_AIF2TX2MIX_INPUT_1_VOLUME:
	case WM5100_AIF2TX2MIX_INPUT_2_SOURCE:
	case WM5100_AIF2TX2MIX_INPUT_2_VOLUME:
	case WM5100_AIF2TX2MIX_INPUT_3_SOURCE:
	case WM5100_AIF2TX2MIX_INPUT_3_VOLUME:
	case WM5100_AIF2TX2MIX_INPUT_4_SOURCE:
	case WM5100_AIF2TX2MIX_INPUT_4_VOLUME:
	case WM5100_AIF3TX1MIX_INPUT_1_SOURCE:
	case WM5100_AIF3TX1MIX_INPUT_1_VOLUME:
	case WM5100_AIF3TX1MIX_INPUT_2_SOURCE:
	case WM5100_AIF3TX1MIX_INPUT_2_VOLUME:
	case WM5100_AIF3TX1MIX_INPUT_3_SOURCE:
	case WM5100_AIF3TX1MIX_INPUT_3_VOLUME:
	case WM5100_AIF3TX1MIX_INPUT_4_SOURCE:
	case WM5100_AIF3TX1MIX_INPUT_4_VOLUME:
	case WM5100_AIF3TX2MIX_INPUT_1_SOURCE:
	case WM5100_AIF3TX2MIX_INPUT_1_VOLUME:
	case WM5100_AIF3TX2MIX_INPUT_2_SOURCE:
	case WM5100_AIF3TX2MIX_INPUT_2_VOLUME:
	case WM5100_AIF3TX2MIX_INPUT_3_SOURCE:
	case WM5100_AIF3TX2MIX_INPUT_3_VOLUME:
	case WM5100_AIF3TX2MIX_INPUT_4_SOURCE:
	case WM5100_AIF3TX2MIX_INPUT_4_VOLUME:
	case WM5100_EQ1MIX_INPUT_1_SOURCE:
	case WM5100_EQ1MIX_INPUT_1_VOLUME:
	case WM5100_EQ1MIX_INPUT_2_SOURCE:
	case WM5100_EQ1MIX_INPUT_2_VOLUME:
	case WM5100_EQ1MIX_INPUT_3_SOURCE:
	case WM5100_EQ1MIX_INPUT_3_VOLUME:
	case WM5100_EQ1MIX_INPUT_4_SOURCE:
	case WM5100_EQ1MIX_INPUT_4_VOLUME:
	case WM5100_EQ2MIX_INPUT_1_SOURCE:
	case WM5100_EQ2MIX_INPUT_1_VOLUME:
	case WM5100_EQ2MIX_INPUT_2_SOURCE:
	case WM5100_EQ2MIX_INPUT_2_VOLUME:
	case WM5100_EQ2MIX_INPUT_3_SOURCE:
	case WM5100_EQ2MIX_INPUT_3_VOLUME:
	case WM5100_EQ2MIX_INPUT_4_SOURCE:
	case WM5100_EQ2MIX_INPUT_4_VOLUME:
	case WM5100_EQ3MIX_INPUT_1_SOURCE:
	case WM5100_EQ3MIX_INPUT_1_VOLUME:
	case WM5100_EQ3MIX_INPUT_2_SOURCE:
	case WM5100_EQ3MIX_INPUT_2_VOLUME:
	case WM5100_EQ3MIX_INPUT_3_SOURCE:
	case WM5100_EQ3MIX_INPUT_3_VOLUME:
	case WM5100_EQ3MIX_INPUT_4_SOURCE:
	case WM5100_EQ3MIX_INPUT_4_VOLUME:
	case WM5100_EQ4MIX_INPUT_1_SOURCE:
	case WM5100_EQ4MIX_INPUT_1_VOLUME:
	case WM5100_EQ4MIX_INPUT_2_SOURCE:
	case WM5100_EQ4MIX_INPUT_2_VOLUME:
	case WM5100_EQ4MIX_INPUT_3_SOURCE:
	case WM5100_EQ4MIX_INPUT_3_VOLUME:
	case WM5100_EQ4MIX_INPUT_4_SOURCE:
	case WM5100_EQ4MIX_INPUT_4_VOLUME:
	case WM5100_DRC1LMIX_INPUT_1_SOURCE:
	case WM5100_DRC1LMIX_INPUT_1_VOLUME:
	case WM5100_DRC1LMIX_INPUT_2_SOURCE:
	case WM5100_DRC1LMIX_INPUT_2_VOLUME:
	case WM5100_DRC1LMIX_INPUT_3_SOURCE:
	case WM5100_DRC1LMIX_INPUT_3_VOLUME:
	case WM5100_DRC1LMIX_INPUT_4_SOURCE:
	case WM5100_DRC1LMIX_INPUT_4_VOLUME:
	case WM5100_DRC1RMIX_INPUT_1_SOURCE:
	case WM5100_DRC1RMIX_INPUT_1_VOLUME:
	case WM5100_DRC1RMIX_INPUT_2_SOURCE:
	case WM5100_DRC1RMIX_INPUT_2_VOLUME:
	case WM5100_DRC1RMIX_INPUT_3_SOURCE:
	case WM5100_DRC1RMIX_INPUT_3_VOLUME:
	case WM5100_DRC1RMIX_INPUT_4_SOURCE:
	case WM5100_DRC1RMIX_INPUT_4_VOLUME:
	case WM5100_HPLP1MIX_INPUT_1_SOURCE:
	case WM5100_HPLP1MIX_INPUT_1_VOLUME:
	case WM5100_HPLP1MIX_INPUT_2_SOURCE:
	case WM5100_HPLP1MIX_INPUT_2_VOLUME:
	case WM5100_HPLP1MIX_INPUT_3_SOURCE:
	case WM5100_HPLP1MIX_INPUT_3_VOLUME:
	case WM5100_HPLP1MIX_INPUT_4_SOURCE:
	case WM5100_HPLP1MIX_INPUT_4_VOLUME:
	case WM5100_HPLP2MIX_INPUT_1_SOURCE:
	case WM5100_HPLP2MIX_INPUT_1_VOLUME:
	case WM5100_HPLP2MIX_INPUT_2_SOURCE:
	case WM5100_HPLP2MIX_INPUT_2_VOLUME:
	case WM5100_HPLP2MIX_INPUT_3_SOURCE:
	case WM5100_HPLP2MIX_INPUT_3_VOLUME:
	case WM5100_HPLP2MIX_INPUT_4_SOURCE:
	case WM5100_HPLP2MIX_INPUT_4_VOLUME:
	case WM5100_HPLP3MIX_INPUT_1_SOURCE:
	case WM5100_HPLP3MIX_INPUT_1_VOLUME:
	case WM5100_HPLP3MIX_INPUT_2_SOURCE:
	case WM5100_HPLP3MIX_INPUT_2_VOLUME:
	case WM5100_HPLP3MIX_INPUT_3_SOURCE:
	case WM5100_HPLP3MIX_INPUT_3_VOLUME:
	case WM5100_HPLP3MIX_INPUT_4_SOURCE:
	case WM5100_HPLP3MIX_INPUT_4_VOLUME:
	case WM5100_HPLP4MIX_INPUT_1_SOURCE:
	case WM5100_HPLP4MIX_INPUT_1_VOLUME:
	case WM5100_HPLP4MIX_INPUT_2_SOURCE:
	case WM5100_HPLP4MIX_INPUT_2_VOLUME:
	case WM5100_HPLP4MIX_INPUT_3_SOURCE:
	case WM5100_HPLP4MIX_INPUT_3_VOLUME:
	case WM5100_HPLP4MIX_INPUT_4_SOURCE:
	case WM5100_HPLP4MIX_INPUT_4_VOLUME:
	case WM5100_DSP1LMIX_INPUT_1_SOURCE:
	case WM5100_DSP1LMIX_INPUT_1_VOLUME:
	case WM5100_DSP1LMIX_INPUT_2_SOURCE:
	case WM5100_DSP1LMIX_INPUT_2_VOLUME:
	case WM5100_DSP1LMIX_INPUT_3_SOURCE:
	case WM5100_DSP1LMIX_INPUT_3_VOLUME:
	case WM5100_DSP1LMIX_INPUT_4_SOURCE:
	case WM5100_DSP1LMIX_INPUT_4_VOLUME:
	case WM5100_DSP1RMIX_INPUT_1_SOURCE:
	case WM5100_DSP1RMIX_INPUT_1_VOLUME:
	case WM5100_DSP1RMIX_INPUT_2_SOURCE:
	case WM5100_DSP1RMIX_INPUT_2_VOLUME:
	case WM5100_DSP1RMIX_INPUT_3_SOURCE:
	case WM5100_DSP1RMIX_INPUT_3_VOLUME:
	case WM5100_DSP1RMIX_INPUT_4_SOURCE:
	case WM5100_DSP1RMIX_INPUT_4_VOLUME:
	case WM5100_DSP1AUX1MIX_INPUT_1_SOURCE:
	case WM5100_DSP1AUX2MIX_INPUT_1_SOURCE:
	case WM5100_DSP1AUX3MIX_INPUT_1_SOURCE:
	case WM5100_DSP1AUX4MIX_INPUT_1_SOURCE:
	case WM5100_DSP1AUX5MIX_INPUT_1_SOURCE:
	case WM5100_DSP1AUX6MIX_INPUT_1_SOURCE:
	case WM5100_DSP2LMIX_INPUT_1_SOURCE:
	case WM5100_DSP2LMIX_INPUT_1_VOLUME:
	case WM5100_DSP2LMIX_INPUT_2_SOURCE:
	case WM5100_DSP2LMIX_INPUT_2_VOLUME:
	case WM5100_DSP2LMIX_INPUT_3_SOURCE:
	case WM5100_DSP2LMIX_INPUT_3_VOLUME:
	case WM5100_DSP2LMIX_INPUT_4_SOURCE:
	case WM5100_DSP2LMIX_INPUT_4_VOLUME:
	case WM5100_DSP2RMIX_INPUT_1_SOURCE:
	case WM5100_DSP2RMIX_INPUT_1_VOLUME:
	case WM5100_DSP2RMIX_INPUT_2_SOURCE:
	case WM5100_DSP2RMIX_INPUT_2_VOLUME:
	case WM5100_DSP2RMIX_INPUT_3_SOURCE:
	case WM5100_DSP2RMIX_INPUT_3_VOLUME:
	case WM5100_DSP2RMIX_INPUT_4_SOURCE:
	case WM5100_DSP2RMIX_INPUT_4_VOLUME:
	case WM5100_DSP2AUX1MIX_INPUT_1_SOURCE:
	case WM5100_DSP2AUX2MIX_INPUT_1_SOURCE:
	case WM5100_DSP2AUX3MIX_INPUT_1_SOURCE:
	case WM5100_DSP2AUX4MIX_INPUT_1_SOURCE:
	case WM5100_DSP2AUX5MIX_INPUT_1_SOURCE:
	case WM5100_DSP2AUX6MIX_INPUT_1_SOURCE:
	case WM5100_DSP3LMIX_INPUT_1_SOURCE:
	case WM5100_DSP3LMIX_INPUT_1_VOLUME:
	case WM5100_DSP3LMIX_INPUT_2_SOURCE:
	case WM5100_DSP3LMIX_INPUT_2_VOLUME:
	case WM5100_DSP3LMIX_INPUT_3_SOURCE:
	case WM5100_DSP3LMIX_INPUT_3_VOLUME:
	case WM5100_DSP3LMIX_INPUT_4_SOURCE:
	case WM5100_DSP3LMIX_INPUT_4_VOLUME:
	case WM5100_DSP3RMIX_INPUT_1_SOURCE:
	case WM5100_DSP3RMIX_INPUT_1_VOLUME:
	case WM5100_DSP3RMIX_INPUT_2_SOURCE:
	case WM5100_DSP3RMIX_INPUT_2_VOLUME:
	case WM5100_DSP3RMIX_INPUT_3_SOURCE:
	case WM5100_DSP3RMIX_INPUT_3_VOLUME:
	case WM5100_DSP3RMIX_INPUT_4_SOURCE:
	case WM5100_DSP3RMIX_INPUT_4_VOLUME:
	case WM5100_DSP3AUX1MIX_INPUT_1_SOURCE:
	case WM5100_DSP3AUX2MIX_INPUT_1_SOURCE:
	case WM5100_DSP3AUX3MIX_INPUT_1_SOURCE:
	case WM5100_DSP3AUX4MIX_INPUT_1_SOURCE:
	case WM5100_DSP3AUX5MIX_INPUT_1_SOURCE:
	case WM5100_DSP3AUX6MIX_INPUT_1_SOURCE:
	case WM5100_ASRC1LMIX_INPUT_1_SOURCE:
	case WM5100_ASRC1RMIX_INPUT_1_SOURCE:
	case WM5100_ASRC2LMIX_INPUT_1_SOURCE:
	case WM5100_ASRC2RMIX_INPUT_1_SOURCE:
	case WM5100_ISRC1DEC1MIX_INPUT_1_SOURCE:
	case WM5100_ISRC1DEC2MIX_INPUT_1_SOURCE:
	case WM5100_ISRC1DEC3MIX_INPUT_1_SOURCE:
	case WM5100_ISRC1DEC4MIX_INPUT_1_SOURCE:
	case WM5100_ISRC1INT1MIX_INPUT_1_SOURCE:
	case WM5100_ISRC1INT2MIX_INPUT_1_SOURCE:
	case WM5100_ISRC1INT3MIX_INPUT_1_SOURCE:
	case WM5100_ISRC1INT4MIX_INPUT_1_SOURCE:
	case WM5100_ISRC2DEC1MIX_INPUT_1_SOURCE:
	case WM5100_ISRC2DEC2MIX_INPUT_1_SOURCE:
	case WM5100_ISRC2DEC3MIX_INPUT_1_SOURCE:
	case WM5100_ISRC2DEC4MIX_INPUT_1_SOURCE:
	case WM5100_ISRC2INT1MIX_INPUT_1_SOURCE:
	case WM5100_ISRC2INT2MIX_INPUT_1_SOURCE:
	case WM5100_ISRC2INT3MIX_INPUT_1_SOURCE:
	case WM5100_ISRC2INT4MIX_INPUT_1_SOURCE:
	case WM5100_GPIO_CTRL_1:
	case WM5100_GPIO_CTRL_2:
	case WM5100_GPIO_CTRL_3:
	case WM5100_GPIO_CTRL_4:
	case WM5100_GPIO_CTRL_5:
	case WM5100_GPIO_CTRL_6:
	case WM5100_MISC_PAD_CTRL_1:
	case WM5100_MISC_PAD_CTRL_2:
	case WM5100_MISC_PAD_CTRL_3:
	case WM5100_MISC_PAD_CTRL_4:
	case WM5100_MISC_PAD_CTRL_5:
	case WM5100_MISC_GPIO_1:
	case WM5100_INTERRUPT_STATUS_1:
	case WM5100_INTERRUPT_STATUS_2:
	case WM5100_INTERRUPT_STATUS_3:
	case WM5100_INTERRUPT_STATUS_4:
	case WM5100_INTERRUPT_RAW_STATUS_2:
	case WM5100_INTERRUPT_RAW_STATUS_3:
	case WM5100_INTERRUPT_RAW_STATUS_4:
	case WM5100_INTERRUPT_STATUS_1_MASK:
	case WM5100_INTERRUPT_STATUS_2_MASK:
	case WM5100_INTERRUPT_STATUS_3_MASK:
	case WM5100_INTERRUPT_STATUS_4_MASK:
	case WM5100_INTERRUPT_CONTROL:
	case WM5100_IRQ_DEBOUNCE_1:
	case WM5100_IRQ_DEBOUNCE_2:
	case WM5100_FX_CTRL:
	case WM5100_EQ1_1:
	case WM5100_EQ1_2:
	case WM5100_EQ1_3:
	case WM5100_EQ1_4:
	case WM5100_EQ1_5:
	case WM5100_EQ1_6:
	case WM5100_EQ1_7:
	case WM5100_EQ1_8:
	case WM5100_EQ1_9:
	case WM5100_EQ1_10:
	case WM5100_EQ1_11:
	case WM5100_EQ1_12:
	case WM5100_EQ1_13:
	case WM5100_EQ1_14:
	case WM5100_EQ1_15:
	case WM5100_EQ1_16:
	case WM5100_EQ1_17:
	case WM5100_EQ1_18:
	case WM5100_EQ1_19:
	case WM5100_EQ1_20:
	case WM5100_EQ2_1:
	case WM5100_EQ2_2:
	case WM5100_EQ2_3:
	case WM5100_EQ2_4:
	case WM5100_EQ2_5:
	case WM5100_EQ2_6:
	case WM5100_EQ2_7:
	case WM5100_EQ2_8:
	case WM5100_EQ2_9:
	case WM5100_EQ2_10:
	case WM5100_EQ2_11:
	case WM5100_EQ2_12:
	case WM5100_EQ2_13:
	case WM5100_EQ2_14:
	case WM5100_EQ2_15:
	case WM5100_EQ2_16:
	case WM5100_EQ2_17:
	case WM5100_EQ2_18:
	case WM5100_EQ2_19:
	case WM5100_EQ2_20:
	case WM5100_EQ3_1:
	case WM5100_EQ3_2:
	case WM5100_EQ3_3:
	case WM5100_EQ3_4:
	case WM5100_EQ3_5:
	case WM5100_EQ3_6:
	case WM5100_EQ3_7:
	case WM5100_EQ3_8:
	case WM5100_EQ3_9:
	case WM5100_EQ3_10:
	case WM5100_EQ3_11:
	case WM5100_EQ3_12:
	case WM5100_EQ3_13:
	case WM5100_EQ3_14:
	case WM5100_EQ3_15:
	case WM5100_EQ3_16:
	case WM5100_EQ3_17:
	case WM5100_EQ3_18:
	case WM5100_EQ3_19:
	case WM5100_EQ3_20:
	case WM5100_EQ4_1:
	case WM5100_EQ4_2:
	case WM5100_EQ4_3:
	case WM5100_EQ4_4:
	case WM5100_EQ4_5:
	case WM5100_EQ4_6:
	case WM5100_EQ4_7:
	case WM5100_EQ4_8:
	case WM5100_EQ4_9:
	case WM5100_EQ4_10:
	case WM5100_EQ4_11:
	case WM5100_EQ4_12:
	case WM5100_EQ4_13:
	case WM5100_EQ4_14:
	case WM5100_EQ4_15:
	case WM5100_EQ4_16:
	case WM5100_EQ4_17:
	case WM5100_EQ4_18:
	case WM5100_EQ4_19:
	case WM5100_EQ4_20:
	case WM5100_DRC1_CTRL1:
	case WM5100_DRC1_CTRL2:
	case WM5100_DRC1_CTRL3:
	case WM5100_DRC1_CTRL4:
	case WM5100_DRC1_CTRL5:
	case WM5100_HPLPF1_1:
	case WM5100_HPLPF1_2:
	case WM5100_HPLPF2_1:
	case WM5100_HPLPF2_2:
	case WM5100_HPLPF3_1:
	case WM5100_HPLPF3_2:
	case WM5100_HPLPF4_1:
	case WM5100_HPLPF4_2:
	case WM5100_DSP1_CONTROL_1:
	case WM5100_DSP1_CONTROL_2:
	case WM5100_DSP1_CONTROL_3:
	case WM5100_DSP1_CONTROL_4:
	case WM5100_DSP1_CONTROL_5:
	case WM5100_DSP1_CONTROL_6:
	case WM5100_DSP1_CONTROL_7:
	case WM5100_DSP1_CONTROL_8:
	case WM5100_DSP1_CONTROL_9:
	case WM5100_DSP1_CONTROL_10:
	case WM5100_DSP1_CONTROL_11:
	case WM5100_DSP1_CONTROL_12:
	case WM5100_DSP1_CONTROL_13:
	case WM5100_DSP1_CONTROL_14:
	case WM5100_DSP1_CONTROL_15:
	case WM5100_DSP1_CONTROL_16:
	case WM5100_DSP1_CONTROL_17:
	case WM5100_DSP1_CONTROL_18:
	case WM5100_DSP1_CONTROL_19:
	case WM5100_DSP1_CONTROL_20:
	case WM5100_DSP1_CONTROL_21:
	case WM5100_DSP1_CONTROL_22:
	case WM5100_DSP1_CONTROL_23:
	case WM5100_DSP1_CONTROL_24:
	case WM5100_DSP1_CONTROL_25:
	case WM5100_DSP1_CONTROL_26:
	case WM5100_DSP1_CONTROL_27:
	case WM5100_DSP1_CONTROL_28:
	case WM5100_DSP1_CONTROL_29:
	case WM5100_DSP1_CONTROL_30:
	case WM5100_DSP2_CONTROL_1:
	case WM5100_DSP2_CONTROL_2:
	case WM5100_DSP2_CONTROL_3:
	case WM5100_DSP2_CONTROL_4:
	case WM5100_DSP2_CONTROL_5:
	case WM5100_DSP2_CONTROL_6:
	case WM5100_DSP2_CONTROL_7:
	case WM5100_DSP2_CONTROL_8:
	case WM5100_DSP2_CONTROL_9:
	case WM5100_DSP2_CONTROL_10:
	case WM5100_DSP2_CONTROL_11:
	case WM5100_DSP2_CONTROL_12:
	case WM5100_DSP2_CONTROL_13:
	case WM5100_DSP2_CONTROL_14:
	case WM5100_DSP2_CONTROL_15:
	case WM5100_DSP2_CONTROL_16:
	case WM5100_DSP2_CONTROL_17:
	case WM5100_DSP2_CONTROL_18:
	case WM5100_DSP2_CONTROL_19:
	case WM5100_DSP2_CONTROL_20:
	case WM5100_DSP2_CONTROL_21:
	case WM5100_DSP2_CONTROL_22:
	case WM5100_DSP2_CONTROL_23:
	case WM5100_DSP2_CONTROL_24:
	case WM5100_DSP2_CONTROL_25:
	case WM5100_DSP2_CONTROL_26:
	case WM5100_DSP2_CONTROL_27:
	case WM5100_DSP2_CONTROL_28:
	case WM5100_DSP2_CONTROL_29:
	case WM5100_DSP2_CONTROL_30:
	case WM5100_DSP3_CONTROL_1:
	case WM5100_DSP3_CONTROL_2:
	case WM5100_DSP3_CONTROL_3:
	case WM5100_DSP3_CONTROL_4:
	case WM5100_DSP3_CONTROL_5:
	case WM5100_DSP3_CONTROL_6:
	case WM5100_DSP3_CONTROL_7:
	case WM5100_DSP3_CONTROL_8:
	case WM5100_DSP3_CONTROL_9:
	case WM5100_DSP3_CONTROL_10:
	case WM5100_DSP3_CONTROL_11:
	case WM5100_DSP3_CONTROL_12:
	case WM5100_DSP3_CONTROL_13:
	case WM5100_DSP3_CONTROL_14:
	case WM5100_DSP3_CONTROL_15:
	case WM5100_DSP3_CONTROL_16:
	case WM5100_DSP3_CONTROL_17:
	case WM5100_DSP3_CONTROL_18:
	case WM5100_DSP3_CONTROL_19:
	case WM5100_DSP3_CONTROL_20:
	case WM5100_DSP3_CONTROL_21:
	case WM5100_DSP3_CONTROL_22:
	case WM5100_DSP3_CONTROL_23:
	case WM5100_DSP3_CONTROL_24:
	case WM5100_DSP3_CONTROL_25:
	case WM5100_DSP3_CONTROL_26:
	case WM5100_DSP3_CONTROL_27:
	case WM5100_DSP3_CONTROL_28:
	case WM5100_DSP3_CONTROL_29:
	case WM5100_DSP3_CONTROL_30:
		return 1;
	default:
		if ((reg >= WM5100_DSP1_PM_0 && reg <= WM5100_DSP1_PM_1535) ||
		    (reg >= WM5100_DSP1_ZM_0 && reg <= WM5100_DSP1_ZM_2047) ||
		    (reg >= WM5100_DSP1_DM_0 && reg <= WM5100_DSP1_DM_511) ||
		    (reg >= WM5100_DSP2_PM_0 && reg <= WM5100_DSP2_PM_1535) ||
		    (reg >= WM5100_DSP2_ZM_0 && reg <= WM5100_DSP2_ZM_2047) ||
		    (reg >= WM5100_DSP2_DM_0 && reg <= WM5100_DSP2_DM_511) ||
		    (reg >= WM5100_DSP3_PM_0 && reg <= WM5100_DSP3_PM_1535) ||
		    (reg >= WM5100_DSP3_ZM_0 && reg <= WM5100_DSP3_ZM_2047) ||
		    (reg >= WM5100_DSP3_DM_0 && reg <= WM5100_DSP3_DM_511))
			return 1;
		else
			return 0;
	}
}

struct reg_default wm5100_reg_defaults[WM5100_REGISTER_COUNT] = {
	{ 0x0000, 0x0000 },  /*                         */
	{ 0x0001, 0x0000 },  /*                          */
	{ 0x0010, 0x0801 },  /*                    */
	{ 0x0020, 0x0000 },  /*                           */
	{ 0x0030, 0x0000 },  /*                      */
	{ 0x0031, 0x0100 },  /*                      */
	{ 0x0032, 0x0100 },  /*                      */
	{ 0x0100, 0x0002 },  /*                     */
	{ 0x0101, 0x0000 },  /*                     */
	{ 0x0102, 0x0011 },  /*                     */
	{ 0x0103, 0x0011 },  /*                     */
	{ 0x0104, 0x0011 },  /*                     */
	{ 0x0107, 0x0000 },  /*                     */
	{ 0x0108, 0x0000 },  /*                     */
	{ 0x0120, 0x0000 },  /*                      */
	{ 0x0121, 0x0000 },  /*                      */
	{ 0x0122, 0x0000 },  /*                     */
	{ 0x0141, 0x8000 },  /*                        */
	{ 0x0142, 0x0000 },  /*                        */
	{ 0x0143, 0x8000 },  /*                       */
	{ 0x0144, 0x0000 },  /*                        */
	{ 0x0182, 0x0000 },  /*                         */
	{ 0x0183, 0x0000 },  /*                         */
	{ 0x0184, 0x0000 },  /*                         */
	{ 0x0186, 0x0177 },  /*                         */
	{ 0x0187, 0x0001 },  /*                         */
	{ 0x0188, 0x0000 },  /*                     */
	{ 0x01A2, 0x0000 },  /*                         */
	{ 0x01A3, 0x0000 },  /*                         */
	{ 0x01A4, 0x0000 },  /*                         */
	{ 0x01A6, 0x0177 },  /*                         */
	{ 0x01A7, 0x0001 },  /*                         */
	{ 0x01A8, 0x0000 },  /*                     */
	{ 0x0200, 0x0020 },  /*                            */
	{ 0x0201, 0xB084 },  /*                            */
	{ 0x0202, 0xBBDE },  /*                           */
	{ 0x0211, 0x20D4 },  /*                       */
	{ 0x0215, 0x0062 },  /*                          */
	{ 0x0216, 0x0062 },  /*                          */
	{ 0x0217, 0x0062 },  /*                          */
	{ 0x0280, 0x0004 },  /*                                  */
	{ 0x0288, 0x0020 },  /*                             */
	{ 0x0289, 0x0000 },  /*                             */
	{ 0x0290, 0x1100 },  /*                       */
	{ 0x0291, 0x009F },  /*                       */
	{ 0x0292, 0x0000 },  /*                       */
	{ 0x0301, 0x0000 },  /*                        */
	{ 0x0302, 0x0000 },  /*                               */
	{ 0x0310, 0x2280 },  /*                 */
	{ 0x0311, 0x0080 },  /*                       */
	{ 0x0312, 0x2280 },  /*                       */
	{ 0x0313, 0x0080 },  /*                       */
	{ 0x0314, 0x2280 },  /*                       */
	{ 0x0315, 0x0080 },  /*                       */
	{ 0x0316, 0x2280 },  /*                       */
	{ 0x0317, 0x0080 },  /*                       */
	{ 0x0318, 0x0000 },  /*                    */
	{ 0x0319, 0x0022 },  /*                            */
	{ 0x0320, 0x0180 },  /*                                */
	{ 0x0321, 0x0180 },  /*                                */
	{ 0x0322, 0x0180 },  /*                                */
	{ 0x0323, 0x0180 },  /*                                */
	{ 0x0324, 0x0180 },  /*                                */
	{ 0x0325, 0x0180 },  /*                                */
	{ 0x0326, 0x0180 },  /*                                */
	{ 0x0327, 0x0180 },  /*                                */
	{ 0x0401, 0x0000 },  /*                           */
	{ 0x0402, 0x0000 },  /*                          */
	{ 0x0403, 0x0000 },  /*                          */
	{ 0x0408, 0x0000 },  /*                            */
	{ 0x0410, 0x0080 },  /*                        */
	{ 0x0411, 0x0080 },  /*                        */
	{ 0x0412, 0x0080 },  /*                              */
	{ 0x0413, 0x0080 },  /*                              */
	{ 0x0414, 0x0080 },  /*                        */
	{ 0x0415, 0x0080 },  /*                        */
	{ 0x0416, 0x0080 },  /*                              */
	{ 0x0417, 0x0080 },  /*                              */
	{ 0x0418, 0x0080 },  /*                        */
	{ 0x0419, 0x0080 },  /*                        */
	{ 0x041A, 0x0080 },  /*                              */
	{ 0x041B, 0x0080 },  /*                              */
	{ 0x041C, 0x0080 },  /*                        */
	{ 0x041D, 0x0080 },  /*                        */
	{ 0x041E, 0x0080 },  /*                              */
	{ 0x041F, 0x0080 },  /*                              */
	{ 0x0420, 0x0080 },  /*                              */
	{ 0x0421, 0x0080 },  /*                              */
	{ 0x0440, 0x0000 },  /*                            */
	{ 0x0441, 0x0022 },  /*                             */
	{ 0x0480, 0x0180 },  /*                                */
	{ 0x0481, 0x0180 },  /*                                */
	{ 0x0482, 0x0180 },  /*                                */
	{ 0x0483, 0x0180 },  /*                                */
	{ 0x0484, 0x0180 },  /*                                */
	{ 0x0485, 0x0180 },  /*                                */
	{ 0x0486, 0x0180 },  /*                                */
	{ 0x0487, 0x0180 },  /*                                */
	{ 0x0488, 0x0180 },  /*                                */
	{ 0x0489, 0x0180 },  /*                                */
	{ 0x048A, 0x0180 },  /*                                */
	{ 0x048B, 0x0180 },  /*                                */
	{ 0x04C0, 0x0069 },  /*                          */
	{ 0x04C1, 0x0000 },  /*                          */
	{ 0x04C2, 0x0069 },  /*                          */
	{ 0x04C3, 0x0000 },  /*                          */
	{ 0x0500, 0x000C },  /*                       */
	{ 0x0501, 0x0008 },  /*                       */
	{ 0x0502, 0x0000 },  /*                       */
	{ 0x0503, 0x0000 },  /*                       */
	{ 0x0504, 0x0000 },  /*                       */
	{ 0x0505, 0x0300 },  /*                       */
	{ 0x0506, 0x0300 },  /*                       */
	{ 0x0507, 0x1820 },  /*                       */
	{ 0x0508, 0x1820 },  /*                       */
	{ 0x0509, 0x0000 },  /*                        */
	{ 0x050A, 0x0001 },  /*                        */
	{ 0x050B, 0x0002 },  /*                        */
	{ 0x050C, 0x0003 },  /*                        */
	{ 0x050D, 0x0004 },  /*                        */
	{ 0x050E, 0x0005 },  /*                        */
	{ 0x050F, 0x0006 },  /*                        */
	{ 0x0510, 0x0007 },  /*                        */
	{ 0x0511, 0x0000 },  /*                        */
	{ 0x0512, 0x0001 },  /*                        */
	{ 0x0513, 0x0002 },  /*                        */
	{ 0x0514, 0x0003 },  /*                        */
	{ 0x0515, 0x0004 },  /*                        */
	{ 0x0516, 0x0005 },  /*                        */
	{ 0x0517, 0x0006 },  /*                        */
	{ 0x0518, 0x0007 },  /*                        */
	{ 0x0519, 0x0000 },  /*                        */
	{ 0x051A, 0x0000 },  /*                        */
	{ 0x0540, 0x000C },  /*                       */
	{ 0x0541, 0x0008 },  /*                       */
	{ 0x0542, 0x0000 },  /*                       */
	{ 0x0543, 0x0000 },  /*                       */
	{ 0x0544, 0x0000 },  /*                       */
	{ 0x0545, 0x0300 },  /*                       */
	{ 0x0546, 0x0300 },  /*                       */
	{ 0x0547, 0x1820 },  /*                       */
	{ 0x0548, 0x1820 },  /*                       */
	{ 0x0549, 0x0000 },  /*                        */
	{ 0x054A, 0x0001 },  /*                        */
	{ 0x0551, 0x0000 },  /*                        */
	{ 0x0552, 0x0001 },  /*                        */
	{ 0x0559, 0x0000 },  /*                        */
	{ 0x055A, 0x0000 },  /*                        */
	{ 0x0580, 0x000C },  /*                       */
	{ 0x0581, 0x0008 },  /*                       */
	{ 0x0582, 0x0000 },  /*                       */
	{ 0x0583, 0x0000 },  /*                       */
	{ 0x0584, 0x0000 },  /*                       */
	{ 0x0585, 0x0300 },  /*                       */
	{ 0x0586, 0x0300 },  /*                       */
	{ 0x0587, 0x1820 },  /*                       */
	{ 0x0588, 0x1820 },  /*                       */
	{ 0x0589, 0x0000 },  /*                        */
	{ 0x058A, 0x0001 },  /*                        */
	{ 0x0591, 0x0000 },  /*                        */
	{ 0x0592, 0x0001 },  /*                        */
	{ 0x0599, 0x0000 },  /*                        */
	{ 0x059A, 0x0000 },  /*                        */
	{ 0x0640, 0x0000 },  /*                                 */
	{ 0x0641, 0x0080 },  /*                                 */
	{ 0x0642, 0x0000 },  /*                                 */
	{ 0x0643, 0x0080 },  /*                                 */
	{ 0x0644, 0x0000 },  /*                                 */
	{ 0x0645, 0x0080 },  /*                                 */
	{ 0x0646, 0x0000 },  /*                                 */
	{ 0x0647, 0x0080 },  /*                                 */
	{ 0x0648, 0x0000 },  /*                                 */
	{ 0x0649, 0x0080 },  /*                                 */
	{ 0x064A, 0x0000 },  /*                                 */
	{ 0x064B, 0x0080 },  /*                                 */
	{ 0x064C, 0x0000 },  /*                                 */
	{ 0x064D, 0x0080 },  /*                                 */
	{ 0x064E, 0x0000 },  /*                                 */
	{ 0x064F, 0x0080 },  /*                                 */
	{ 0x0680, 0x0000 },  /*                                  */
	{ 0x0681, 0x0080 },  /*                                  */
	{ 0x0682, 0x0000 },  /*                                  */
	{ 0x0683, 0x0080 },  /*                                  */
	{ 0x0684, 0x0000 },  /*                                  */
	{ 0x0685, 0x0080 },  /*                                  */
	{ 0x0686, 0x0000 },  /*                                  */
	{ 0x0687, 0x0080 },  /*                                  */
	{ 0x0688, 0x0000 },  /*                                  */
	{ 0x0689, 0x0080 },  /*                                  */
	{ 0x068A, 0x0000 },  /*                                  */
	{ 0x068B, 0x0080 },  /*                                  */
	{ 0x068C, 0x0000 },  /*                                  */
	{ 0x068D, 0x0080 },  /*                                  */
	{ 0x068E, 0x0000 },  /*                                  */
	{ 0x068F, 0x0080 },  /*                                  */
	{ 0x0690, 0x0000 },  /*                                  */
	{ 0x0691, 0x0080 },  /*                                  */
	{ 0x0692, 0x0000 },  /*                                  */
	{ 0x0693, 0x0080 },  /*                                  */
	{ 0x0694, 0x0000 },  /*                                  */
	{ 0x0695, 0x0080 },  /*                                  */
	{ 0x0696, 0x0000 },  /*                                  */
	{ 0x0697, 0x0080 },  /*                                  */
	{ 0x0698, 0x0000 },  /*                                  */
	{ 0x0699, 0x0080 },  /*                                  */
	{ 0x069A, 0x0000 },  /*                                  */
	{ 0x069B, 0x0080 },  /*                                  */
	{ 0x069C, 0x0000 },  /*                                  */
	{ 0x069D, 0x0080 },  /*                                  */
	{ 0x069E, 0x0000 },  /*                                  */
	{ 0x069F, 0x0080 },  /*                                  */
	{ 0x06A0, 0x0000 },  /*                                  */
	{ 0x06A1, 0x0080 },  /*                                  */
	{ 0x06A2, 0x0000 },  /*                                  */
	{ 0x06A3, 0x0080 },  /*                                  */
	{ 0x06A4, 0x0000 },  /*                                  */
	{ 0x06A5, 0x0080 },  /*                                  */
	{ 0x06A6, 0x0000 },  /*                                  */
	{ 0x06A7, 0x0080 },  /*                                  */
	{ 0x06A8, 0x0000 },  /*                                  */
	{ 0x06A9, 0x0080 },  /*                                  */
	{ 0x06AA, 0x0000 },  /*                                  */
	{ 0x06AB, 0x0080 },  /*                                  */
	{ 0x06AC, 0x0000 },  /*                                  */
	{ 0x06AD, 0x0080 },  /*                                  */
	{ 0x06AE, 0x0000 },  /*                                  */
	{ 0x06AF, 0x0080 },  /*                                  */
	{ 0x06B0, 0x0000 },  /*                                  */
	{ 0x06B1, 0x0080 },  /*                                  */
	{ 0x06B2, 0x0000 },  /*                                  */
	{ 0x06B3, 0x0080 },  /*                                  */
	{ 0x06B4, 0x0000 },  /*                                  */
	{ 0x06B5, 0x0080 },  /*                                  */
	{ 0x06B6, 0x0000 },  /*                                  */
	{ 0x06B7, 0x0080 },  /*                                  */
	{ 0x06B8, 0x0000 },  /*                                  */
	{ 0x06B9, 0x0080 },  /*                                  */
	{ 0x06BA, 0x0000 },  /*                                  */
	{ 0x06BB, 0x0080 },  /*                                  */
	{ 0x06BC, 0x0000 },  /*                                  */
	{ 0x06BD, 0x0080 },  /*                                  */
	{ 0x06BE, 0x0000 },  /*                                  */
	{ 0x06BF, 0x0080 },  /*                                  */
	{ 0x06C0, 0x0000 },  /*                                  */
	{ 0x06C1, 0x0080 },  /*                                  */
	{ 0x06C2, 0x0000 },  /*                                  */
	{ 0x06C3, 0x0080 },  /*                                  */
	{ 0x06C4, 0x0000 },  /*                                  */
	{ 0x06C5, 0x0080 },  /*                                  */
	{ 0x06C6, 0x0000 },  /*                                  */
	{ 0x06C7, 0x0080 },  /*                                  */
	{ 0x06C8, 0x0000 },  /*                                  */
	{ 0x06C9, 0x0080 },  /*                                  */
	{ 0x06CA, 0x0000 },  /*                                  */
	{ 0x06CB, 0x0080 },  /*                                  */
	{ 0x06CC, 0x0000 },  /*                                  */
	{ 0x06CD, 0x0080 },  /*                                  */
	{ 0x06CE, 0x0000 },  /*                                  */
	{ 0x06CF, 0x0080 },  /*                                  */
	{ 0x06D0, 0x0000 },  /*                                  */
	{ 0x06D1, 0x0080 },  /*                                  */
	{ 0x06D2, 0x0000 },  /*                                  */
	{ 0x06D3, 0x0080 },  /*                                  */
	{ 0x06D4, 0x0000 },  /*                                  */
	{ 0x06D5, 0x0080 },  /*                                  */
	{ 0x06D6, 0x0000 },  /*                                  */
	{ 0x06D7, 0x0080 },  /*                                  */
	{ 0x06D8, 0x0000 },  /*                                  */
	{ 0x06D9, 0x0080 },  /*                                  */
	{ 0x06DA, 0x0000 },  /*                                  */
	{ 0x06DB, 0x0080 },  /*                                  */
	{ 0x06DC, 0x0000 },  /*                                  */
	{ 0x06DD, 0x0080 },  /*                                  */
	{ 0x06DE, 0x0000 },  /*                                  */
	{ 0x06DF, 0x0080 },  /*                                  */
	{ 0x0700, 0x0000 },  /*                                    */
	{ 0x0701, 0x0080 },  /*                                    */
	{ 0x0702, 0x0000 },  /*                                    */
	{ 0x0703, 0x0080 },  /*                                    */
	{ 0x0704, 0x0000 },  /*                                    */
	{ 0x0705, 0x0080 },  /*                                    */
	{ 0x0706, 0x0000 },  /*                                    */
	{ 0x0707, 0x0080 },  /*                                    */
	{ 0x0708, 0x0000 },  /*                                    */
	{ 0x0709, 0x0080 },  /*                                    */
	{ 0x070A, 0x0000 },  /*                                    */
	{ 0x070B, 0x0080 },  /*                                    */
	{ 0x070C, 0x0000 },  /*                                    */
	{ 0x070D, 0x0080 },  /*                                    */
	{ 0x070E, 0x0000 },  /*                                    */
	{ 0x070F, 0x0080 },  /*                                    */
	{ 0x0710, 0x0000 },  /*                                    */
	{ 0x0711, 0x0080 },  /*                                    */
	{ 0x0712, 0x0000 },  /*                                    */
	{ 0x0713, 0x0080 },  /*                                    */
	{ 0x0714, 0x0000 },  /*                                    */
	{ 0x0715, 0x0080 },  /*                                    */
	{ 0x0716, 0x0000 },  /*                                    */
	{ 0x0717, 0x0080 },  /*                                    */
	{ 0x0718, 0x0000 },  /*                                    */
	{ 0x0719, 0x0080 },  /*                                    */
	{ 0x071A, 0x0000 },  /*                                    */
	{ 0x071B, 0x0080 },  /*                                    */
	{ 0x071C, 0x0000 },  /*                                    */
	{ 0x071D, 0x0080 },  /*                                    */
	{ 0x071E, 0x0000 },  /*                                    */
	{ 0x071F, 0x0080 },  /*                                    */
	{ 0x0720, 0x0000 },  /*                                    */
	{ 0x0721, 0x0080 },  /*                                    */
	{ 0x0722, 0x0000 },  /*                                    */
	{ 0x0723, 0x0080 },  /*                                    */
	{ 0x0724, 0x0000 },  /*                                    */
	{ 0x0725, 0x0080 },  /*                                    */
	{ 0x0726, 0x0000 },  /*                                    */
	{ 0x0727, 0x0080 },  /*                                    */
	{ 0x0728, 0x0000 },  /*                                    */
	{ 0x0729, 0x0080 },  /*                                    */
	{ 0x072A, 0x0000 },  /*                                    */
	{ 0x072B, 0x0080 },  /*                                    */
	{ 0x072C, 0x0000 },  /*                                    */
	{ 0x072D, 0x0080 },  /*                                    */
	{ 0x072E, 0x0000 },  /*                                    */
	{ 0x072F, 0x0080 },  /*                                    */
	{ 0x0730, 0x0000 },  /*                                    */
	{ 0x0731, 0x0080 },  /*                                    */
	{ 0x0732, 0x0000 },  /*                                    */
	{ 0x0733, 0x0080 },  /*                                    */
	{ 0x0734, 0x0000 },  /*                                    */
	{ 0x0735, 0x0080 },  /*                                    */
	{ 0x0736, 0x0000 },  /*                                    */
	{ 0x0737, 0x0080 },  /*                                    */
	{ 0x0738, 0x0000 },  /*                                    */
	{ 0x0739, 0x0080 },  /*                                    */
	{ 0x073A, 0x0000 },  /*                                    */
	{ 0x073B, 0x0080 },  /*                                    */
	{ 0x073C, 0x0000 },  /*                                    */
	{ 0x073D, 0x0080 },  /*                                    */
	{ 0x073E, 0x0000 },  /*                                    */
	{ 0x073F, 0x0080 },  /*                                    */
	{ 0x0740, 0x0000 },  /*                                    */
	{ 0x0741, 0x0080 },  /*                                    */
	{ 0x0742, 0x0000 },  /*                                    */
	{ 0x0743, 0x0080 },  /*                                    */
	{ 0x0744, 0x0000 },  /*                                    */
	{ 0x0745, 0x0080 },  /*                                    */
	{ 0x0746, 0x0000 },  /*                                    */
	{ 0x0747, 0x0080 },  /*                                    */
	{ 0x0748, 0x0000 },  /*                                    */
	{ 0x0749, 0x0080 },  /*                                    */
	{ 0x074A, 0x0000 },  /*                                    */
	{ 0x074B, 0x0080 },  /*                                    */
	{ 0x074C, 0x0000 },  /*                                    */
	{ 0x074D, 0x0080 },  /*                                    */
	{ 0x074E, 0x0000 },  /*                                    */
	{ 0x074F, 0x0080 },  /*                                    */
	{ 0x0780, 0x0000 },  /*                                    */
	{ 0x0781, 0x0080 },  /*                                    */
	{ 0x0782, 0x0000 },  /*                                    */
	{ 0x0783, 0x0080 },  /*                                    */
	{ 0x0784, 0x0000 },  /*                                    */
	{ 0x0785, 0x0080 },  /*                                    */
	{ 0x0786, 0x0000 },  /*                                    */
	{ 0x0787, 0x0080 },  /*                                    */
	{ 0x0788, 0x0000 },  /*                                    */
	{ 0x0789, 0x0080 },  /*                                    */
	{ 0x078A, 0x0000 },  /*                                    */
	{ 0x078B, 0x0080 },  /*                                    */
	{ 0x078C, 0x0000 },  /*                                    */
	{ 0x078D, 0x0080 },  /*                                    */
	{ 0x078E, 0x0000 },  /*                                    */
	{ 0x078F, 0x0080 },  /*                                    */
	{ 0x0880, 0x0000 },  /*                                */
	{ 0x0881, 0x0080 },  /*                                */
	{ 0x0882, 0x0000 },  /*                                */
	{ 0x0883, 0x0080 },  /*                                */
	{ 0x0884, 0x0000 },  /*                                */
	{ 0x0885, 0x0080 },  /*                                */
	{ 0x0886, 0x0000 },  /*                                */
	{ 0x0887, 0x0080 },  /*                                */
	{ 0x0888, 0x0000 },  /*                                */
	{ 0x0889, 0x0080 },  /*                                */
	{ 0x088A, 0x0000 },  /*                                */
	{ 0x088B, 0x0080 },  /*                                */
	{ 0x088C, 0x0000 },  /*                                */
	{ 0x088D, 0x0080 },  /*                                */
	{ 0x088E, 0x0000 },  /*                                */
	{ 0x088F, 0x0080 },  /*                                */
	{ 0x0890, 0x0000 },  /*                                */
	{ 0x0891, 0x0080 },  /*                                */
	{ 0x0892, 0x0000 },  /*                                */
	{ 0x0893, 0x0080 },  /*                                */
	{ 0x0894, 0x0000 },  /*                                */
	{ 0x0895, 0x0080 },  /*                                */
	{ 0x0896, 0x0000 },  /*                                */
	{ 0x0897, 0x0080 },  /*                                */
	{ 0x0898, 0x0000 },  /*                                */
	{ 0x0899, 0x0080 },  /*                                */
	{ 0x089A, 0x0000 },  /*                                */
	{ 0x089B, 0x0080 },  /*                                */
	{ 0x089C, 0x0000 },  /*                                */
	{ 0x089D, 0x0080 },  /*                                */
	{ 0x089E, 0x0000 },  /*                                */
	{ 0x089F, 0x0080 },  /*                                */
	{ 0x08C0, 0x0000 },  /*                                  */
	{ 0x08C1, 0x0080 },  /*                                  */
	{ 0x08C2, 0x0000 },  /*                                  */
	{ 0x08C3, 0x0080 },  /*                                  */
	{ 0x08C4, 0x0000 },  /*                                  */
	{ 0x08C5, 0x0080 },  /*                                  */
	{ 0x08C6, 0x0000 },  /*                                  */
	{ 0x08C7, 0x0080 },  /*                                  */
	{ 0x08C8, 0x0000 },  /*                                  */
	{ 0x08C9, 0x0080 },  /*                                  */
	{ 0x08CA, 0x0000 },  /*                                  */
	{ 0x08CB, 0x0080 },  /*                                  */
	{ 0x08CC, 0x0000 },  /*                                  */
	{ 0x08CD, 0x0080 },  /*                                  */
	{ 0x08CE, 0x0000 },  /*                                  */
	{ 0x08CF, 0x0080 },  /*                                  */
	{ 0x0900, 0x0000 },  /*                                  */
	{ 0x0901, 0x0080 },  /*                                  */
	{ 0x0902, 0x0000 },  /*                                  */
	{ 0x0903, 0x0080 },  /*                                  */
	{ 0x0904, 0x0000 },  /*                                  */
	{ 0x0905, 0x0080 },  /*                                  */
	{ 0x0906, 0x0000 },  /*                                  */
	{ 0x0907, 0x0080 },  /*                                  */
	{ 0x0908, 0x0000 },  /*                                  */
	{ 0x0909, 0x0080 },  /*                                  */
	{ 0x090A, 0x0000 },  /*                                  */
	{ 0x090B, 0x0080 },  /*                                  */
	{ 0x090C, 0x0000 },  /*                                  */
	{ 0x090D, 0x0080 },  /*                                  */
	{ 0x090E, 0x0000 },  /*                                  */
	{ 0x090F, 0x0080 },  /*                                  */
	{ 0x0910, 0x0000 },  /*                                  */
	{ 0x0911, 0x0080 },  /*                                  */
	{ 0x0912, 0x0000 },  /*                                  */
	{ 0x0913, 0x0080 },  /*                                  */
	{ 0x0914, 0x0000 },  /*                                  */
	{ 0x0915, 0x0080 },  /*                                  */
	{ 0x0916, 0x0000 },  /*                                  */
	{ 0x0917, 0x0080 },  /*                                  */
	{ 0x0918, 0x0000 },  /*                                  */
	{ 0x0919, 0x0080 },  /*                                  */
	{ 0x091A, 0x0000 },  /*                                  */
	{ 0x091B, 0x0080 },  /*                                  */
	{ 0x091C, 0x0000 },  /*                                  */
	{ 0x091D, 0x0080 },  /*                                  */
	{ 0x091E, 0x0000 },  /*                                  */
	{ 0x091F, 0x0080 },  /*                                  */
	{ 0x0940, 0x0000 },  /*                                  */
	{ 0x0941, 0x0080 },  /*                                  */
	{ 0x0942, 0x0000 },  /*                                  */
	{ 0x0943, 0x0080 },  /*                                  */
	{ 0x0944, 0x0000 },  /*                                  */
	{ 0x0945, 0x0080 },  /*                                  */
	{ 0x0946, 0x0000 },  /*                                  */
	{ 0x0947, 0x0080 },  /*                                  */
	{ 0x0948, 0x0000 },  /*                                  */
	{ 0x0949, 0x0080 },  /*                                  */
	{ 0x094A, 0x0000 },  /*                                  */
	{ 0x094B, 0x0080 },  /*                                  */
	{ 0x094C, 0x0000 },  /*                                  */
	{ 0x094D, 0x0080 },  /*                                  */
	{ 0x094E, 0x0000 },  /*                                  */
	{ 0x094F, 0x0080 },  /*                                  */
	{ 0x0950, 0x0000 },  /*                                     */
	{ 0x0958, 0x0000 },  /*                                     */
	{ 0x0960, 0x0000 },  /*                                     */
	{ 0x0968, 0x0000 },  /*                                     */
	{ 0x0970, 0x0000 },  /*                                     */
	{ 0x0978, 0x0000 },  /*                                     */
	{ 0x0980, 0x0000 },  /*                                  */
	{ 0x0981, 0x0080 },  /*                                  */
	{ 0x0982, 0x0000 },  /*                                  */
	{ 0x0983, 0x0080 },  /*                                  */
	{ 0x0984, 0x0000 },  /*                                  */
	{ 0x0985, 0x0080 },  /*                                  */
	{ 0x0986, 0x0000 },  /*                                  */
	{ 0x0987, 0x0080 },  /*                                  */
	{ 0x0988, 0x0000 },  /*                                  */
	{ 0x0989, 0x0080 },  /*                                  */
	{ 0x098A, 0x0000 },  /*                                  */
	{ 0x098B, 0x0080 },  /*                                  */
	{ 0x098C, 0x0000 },  /*                                  */
	{ 0x098D, 0x0080 },  /*                                  */
	{ 0x098E, 0x0000 },  /*                                  */
	{ 0x098F, 0x0080 },  /*                                  */
	{ 0x0990, 0x0000 },  /*                                     */
	{ 0x0998, 0x0000 },  /*                                     */
	{ 0x09A0, 0x0000 },  /*                                     */
	{ 0x09A8, 0x0000 },  /*                                     */
	{ 0x09B0, 0x0000 },  /*                                     */
	{ 0x09B8, 0x0000 },  /*                                     */
	{ 0x09C0, 0x0000 },  /*                                  */
	{ 0x09C1, 0x0080 },  /*                                  */
	{ 0x09C2, 0x0000 },  /*                                  */
	{ 0x09C3, 0x0080 },  /*                                  */
	{ 0x09C4, 0x0000 },  /*                                  */
	{ 0x09C5, 0x0080 },  /*                                  */
	{ 0x09C6, 0x0000 },  /*                                  */
	{ 0x09C7, 0x0080 },  /*                                  */
	{ 0x09C8, 0x0000 },  /*                                  */
	{ 0x09C9, 0x0080 },  /*                                  */
	{ 0x09CA, 0x0000 },  /*                                  */
	{ 0x09CB, 0x0080 },  /*                                  */
	{ 0x09CC, 0x0000 },  /*                                  */
	{ 0x09CD, 0x0080 },  /*                                  */
	{ 0x09CE, 0x0000 },  /*                                  */
	{ 0x09CF, 0x0080 },  /*                                  */
	{ 0x09D0, 0x0000 },  /*                                     */
	{ 0x09D8, 0x0000 },  /*                                     */
	{ 0x09E0, 0x0000 },  /*                                     */
	{ 0x09E8, 0x0000 },  /*                                     */
	{ 0x09F0, 0x0000 },  /*                                     */
	{ 0x09F8, 0x0000 },  /*                                     */
	{ 0x0A80, 0x0000 },  /*                                   */
	{ 0x0A88, 0x0000 },  /*                                   */
	{ 0x0A90, 0x0000 },  /*                                   */
	{ 0x0A98, 0x0000 },  /*                                   */
	{ 0x0B00, 0x0000 },  /*                                      */
	{ 0x0B08, 0x0000 },  /*                                      */
	{ 0x0B10, 0x0000 },  /*                                      */
	{ 0x0B18, 0x0000 },  /*                                      */
	{ 0x0B20, 0x0000 },  /*                                      */
	{ 0x0B28, 0x0000 },  /*                                      */
	{ 0x0B30, 0x0000 },  /*                                      */
	{ 0x0B38, 0x0000 },  /*                                      */
	{ 0x0B40, 0x0000 },  /*                                      */
	{ 0x0B48, 0x0000 },  /*                                      */
	{ 0x0B50, 0x0000 },  /*                                      */
	{ 0x0B58, 0x0000 },  /*                                      */
	{ 0x0B60, 0x0000 },  /*                                      */
	{ 0x0B68, 0x0000 },  /*                                      */
	{ 0x0B70, 0x0000 },  /*                                      */
	{ 0x0B78, 0x0000 },  /*                                      */
	{ 0x0C00, 0xA001 },  /*                      */
	{ 0x0C01, 0xA001 },  /*                      */
	{ 0x0C02, 0xA001 },  /*                      */
	{ 0x0C03, 0xA001 },  /*                      */
	{ 0x0C04, 0xA001 },  /*                      */
	{ 0x0C05, 0xA001 },  /*                      */
	{ 0x0C23, 0x4003 },  /*                          */
	{ 0x0C24, 0x0000 },  /*                          */
	{ 0x0C25, 0x0000 },  /*                          */
	{ 0x0C26, 0x0000 },  /*                          */
	{ 0x0C27, 0x0000 },  /*                          */
	{ 0x0C28, 0x0000 },  /*                      */
	{ 0x0D00, 0x0000 },  /*                             */
	{ 0x0D01, 0x0000 },  /*                             */
	{ 0x0D02, 0x0000 },  /*                             */
	{ 0x0D03, 0x0000 },  /*                             */
	{ 0x0D04, 0x0000 },  /*                                 */
	{ 0x0D05, 0x0000 },  /*                                 */
	{ 0x0D06, 0x0000 },  /*                                 */
	{ 0x0D07, 0xFFFF },  /*                                  */
	{ 0x0D08, 0xFFFF },  /*                                  */
	{ 0x0D09, 0xFFFF },  /*                                  */
	{ 0x0D0A, 0xFFFF },  /*                                  */
	{ 0x0D1F, 0x0000 },  /*                            */
	{ 0x0D20, 0xFFFF },  /*                         */
	{ 0x0D21, 0xFFFF },  /*                         */
	{ 0x0E00, 0x0000 },  /*                  */
	{ 0x0E10, 0x6318 },  /*                */
	{ 0x0E11, 0x6300 },  /*                */
	{ 0x0E12, 0x0FC8 },  /*                */
	{ 0x0E13, 0x03FE },  /*                */
	{ 0x0E14, 0x00E0 },  /*                */
	{ 0x0E15, 0x1EC4 },  /*                */
	{ 0x0E16, 0xF136 },  /*                */
	{ 0x0E17, 0x0409 },  /*                */
	{ 0x0E18, 0x04CC },  /*                */
	{ 0x0E19, 0x1C9B },  /*                 */
	{ 0x0E1A, 0xF337 },  /*                 */
	{ 0x0E1B, 0x040B },  /*                 */
	{ 0x0E1C, 0x0CBB },  /*                 */
	{ 0x0E1D, 0x16F8 },  /*                 */
	{ 0x0E1E, 0xF7D9 },  /*                 */
	{ 0x0E1F, 0x040A },  /*                 */
	{ 0x0E20, 0x1F14 },  /*                 */
	{ 0x0E21, 0x058C },  /*                 */
	{ 0x0E22, 0x0563 },  /*                 */
	{ 0x0E23, 0x4000 },  /*                 */
	{ 0x0E26, 0x6318 },  /*                */
	{ 0x0E27, 0x6300 },  /*                */
	{ 0x0E28, 0x0FC8 },  /*                */
	{ 0x0E29, 0x03FE },  /*                */
	{ 0x0E2A, 0x00E0 },  /*                */
	{ 0x0E2B, 0x1EC4 },  /*                */
	{ 0x0E2C, 0xF136 },  /*                */
	{ 0x0E2D, 0x0409 },  /*                */
	{ 0x0E2E, 0x04CC },  /*                */
	{ 0x0E2F, 0x1C9B },  /*                 */
	{ 0x0E30, 0xF337 },  /*                 */
	{ 0x0E31, 0x040B },  /*                 */
	{ 0x0E32, 0x0CBB },  /*                 */
	{ 0x0E33, 0x16F8 },  /*                 */
	{ 0x0E34, 0xF7D9 },  /*                 */
	{ 0x0E35, 0x040A },  /*                 */
	{ 0x0E36, 0x1F14 },  /*                 */
	{ 0x0E37, 0x058C },  /*                 */
	{ 0x0E38, 0x0563 },  /*                 */
	{ 0x0E39, 0x4000 },  /*                 */
	{ 0x0E3C, 0x6318 },  /*                */
	{ 0x0E3D, 0x6300 },  /*                */
	{ 0x0E3E, 0x0FC8 },  /*                */
	{ 0x0E3F, 0x03FE },  /*                */
	{ 0x0E40, 0x00E0 },  /*                */
	{ 0x0E41, 0x1EC4 },  /*                */
	{ 0x0E42, 0xF136 },  /*                */
	{ 0x0E43, 0x0409 },  /*                */
	{ 0x0E44, 0x04CC },  /*                */
	{ 0x0E45, 0x1C9B },  /*                 */
	{ 0x0E46, 0xF337 },  /*                 */
	{ 0x0E47, 0x040B },  /*                 */
	{ 0x0E48, 0x0CBB },  /*                 */
	{ 0x0E49, 0x16F8 },  /*                 */
	{ 0x0E4A, 0xF7D9 },  /*                 */
	{ 0x0E4B, 0x040A },  /*                 */
	{ 0x0E4C, 0x1F14 },  /*                 */
	{ 0x0E4D, 0x058C },  /*                 */
	{ 0x0E4E, 0x0563 },  /*                 */
	{ 0x0E4F, 0x4000 },  /*                 */
	{ 0x0E52, 0x6318 },  /*                */
	{ 0x0E53, 0x6300 },  /*                */
	{ 0x0E54, 0x0FC8 },  /*                */
	{ 0x0E55, 0x03FE },  /*                */
	{ 0x0E56, 0x00E0 },  /*                */
	{ 0x0E57, 0x1EC4 },  /*                */
	{ 0x0E58, 0xF136 },  /*                */
	{ 0x0E59, 0x0409 },  /*                */
	{ 0x0E5A, 0x04CC },  /*                */
	{ 0x0E5B, 0x1C9B },  /*                 */
	{ 0x0E5C, 0xF337 },  /*                 */
	{ 0x0E5D, 0x040B },  /*                 */
	{ 0x0E5E, 0x0CBB },  /*                 */
	{ 0x0E5F, 0x16F8 },  /*                 */
	{ 0x0E60, 0xF7D9 },  /*                 */
	{ 0x0E61, 0x040A },  /*                 */
	{ 0x0E62, 0x1F14 },  /*                 */
	{ 0x0E63, 0x058C },  /*                 */
	{ 0x0E64, 0x0563 },  /*                 */
	{ 0x0E65, 0x4000 },  /*                 */
	{ 0x0E80, 0x0018 },  /*                     */
	{ 0x0E81, 0x0933 },  /*                     */
	{ 0x0E82, 0x0018 },  /*                     */
	{ 0x0E83, 0x0000 },  /*                     */
	{ 0x0E84, 0x0000 },  /*                     */
	{ 0x0EC0, 0x0000 },  /*                   */
	{ 0x0EC1, 0x0000 },  /*                   */
	{ 0x0EC4, 0x0000 },  /*                   */
	{ 0x0EC5, 0x0000 },  /*                   */
	{ 0x0EC8, 0x0000 },  /*                   */
	{ 0x0EC9, 0x0000 },  /*                   */
	{ 0x0ECC, 0x0000 },  /*                   */
	{ 0x0ECD, 0x0000 },  /*                   */
	{ 0x0F02, 0x0000 },  /*                         */
	{ 0x0F03, 0x0000 },  /*                         */
	{ 0x0F04, 0x0000 },  /*                         */
	{ 0x1002, 0x0000 },  /*                         */
	{ 0x1003, 0x0000 },  /*                         */
	{ 0x1004, 0x0000 },  /*                         */
	{ 0x1102, 0x0000 },  /*                         */
	{ 0x1103, 0x0000 },  /*                         */
	{ 0x1104, 0x0000 },  /*                         */
};
