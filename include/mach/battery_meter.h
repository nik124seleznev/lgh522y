#ifndef _BATTERY_METER_H
#define _BATTERY_METER_H

#include <mach/mt_typedefs.h>
#include "cust_battery_meter.h"
/*                                                              */
/*        */
/*                                                              */
#define FG_CURRENT_AVERAGE_SIZE 30

#ifdef CONFIG_LGE_PM_SOC_SCALE
#define FG_DOD_MIN	(SOC_RESCALING_FACTOR - 100)
#define FG_DOD_MAX	(100)
#define FG_SOC_MIN	(100 - FG_DOD_MAX)
#define FG_SOC_MAX	(100 - FG_DOD_MIN)
#endif

/*                                                              */
/*      */
/*                                                              */

/*                                                              */
/*           */
/*                                                              */

/*                                                              */
/*         */
/*                                                              */
typedef struct {
	INT32 BatteryTemp;
	INT32 TemperatureR;
} BATT_TEMPERATURE;

/*                                                              */
/*                    */
/*                                                              */

/*                                                              */
/*                   */
/*                                                              */
extern kal_int32 battery_meter_get_battery_voltage(kal_bool update);
extern kal_int32 battery_meter_get_charging_current_imm(void);
extern kal_int32 battery_meter_get_charging_current(void);
extern kal_int32 battery_meter_get_battery_current(void);
extern kal_bool battery_meter_get_battery_current_sign(void);
extern kal_int32 battery_meter_get_car(void);
extern kal_int32 battery_meter_get_battery_temperature(void);
extern kal_int32 battery_meter_get_charger_voltage(void);
extern kal_int32 battery_meter_get_battery_percentage(void);
extern kal_int32 battery_meter_initial(void);
extern kal_int32 battery_meter_reset(void);
extern kal_int32 battery_meter_sync(kal_int32 bat_i_sense_offset);

extern kal_int32 battery_meter_get_battery_zcv(void);
extern kal_int32 battery_meter_get_battery_nPercent_zcv(void);	/*                                 */
extern kal_int32 battery_meter_get_battery_nPercent_UI_SOC(void);	/*                */

extern kal_int32 battery_meter_get_tempR(kal_int32 dwVolt);
extern kal_int32 battery_meter_get_tempV(void);
extern kal_int32 battery_meter_get_VSense(void);	/*                */


#endif				/*                          */
