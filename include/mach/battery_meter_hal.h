#ifndef _BATTERY_METER_HAL_H
#define _BATTERY_METER_HAL_H

#include <mach/mt_typedefs.h>

/*                                                              */
/*        */
/*                                                              */
#define BM_LOG_CRTI 1
#define BM_LOG_FULL 2

#define CUST_DISABLE_CAPACITY_OCV2CV_TRANSFORM

#define bm_print(num, fmt, args...)   \
do {									\
	if (Enable_FGADC_LOG >= (int)num) {				\
		pr_notice(fmt, ##args); \
	}								   \
} while (0)


/*                                                              */
/*      */
/*                                                              */
typedef enum {
	BATTERY_METER_CMD_HW_FG_INIT,

	BATTERY_METER_CMD_GET_HW_FG_CURRENT,	/*                     */
	BATTERY_METER_CMD_GET_HW_FG_CURRENT_SIGN,	/*  */
	BATTERY_METER_CMD_GET_HW_FG_CAR,	/*                    */

	BATTERY_METER_CMD_HW_RESET,	/*                          */

	BATTERY_METER_CMD_GET_ADC_V_BAT_SENSE,
	BATTERY_METER_CMD_GET_ADC_V_I_SENSE,
	BATTERY_METER_CMD_GET_ADC_V_BAT_TEMP,
	BATTERY_METER_CMD_GET_ADC_V_CHARGER,

	BATTERY_METER_CMD_GET_HW_OCV,

	BATTERY_METER_CMD_DUMP_REGISTER,

	BATTERY_METER_CMD_NUMBER
} BATTERY_METER_CTRL_CMD;

/*                                                              */
/*           */
/*                                                              */

/*                                                              */
/*         */
/*                                                              */
typedef kal_int32(*BATTERY_METER_CONTROL) (BATTERY_METER_CTRL_CMD cmd, void *data);

/*                                                              */
/*                    */
/*                                                              */
extern int Enable_FGADC_LOG;

/*                                                              */
/*                   */
/*                                                              */
extern kal_int32 bm_ctrl_cmd(BATTERY_METER_CTRL_CMD cmd, void *data);


#endif				/*                              */
