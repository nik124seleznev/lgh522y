#ifndef _CUST_BAT_H_
#define _CUST_BAT_H_

/*                                     */
//                                
#define TALKING_RECHARGE_VOLTAGE 3800
#define TALKING_SYNC_TIME		   60

/*                                */
#define MTK_TEMPERATURE_RECHARGE_SUPPORT
#define MAX_CHARGE_TEMPERATURE  50
#define MAX_CHARGE_TEMPERATURE_MINUS_X_DEGREE	47
#define MIN_CHARGE_TEMPERATURE  0
#define MIN_CHARGE_TEMPERATURE_PLUS_X_DEGREE	6
#define ERR_CHARGE_TEMPERATURE  0xFF

/*                           */
#define V_PRE2CC_THRES	 		3400	//  
#define V_CC2TOPOFF_THRES		4050
#define RECHARGING_VOLTAGE      4250
#define CHARGING_FULL_CURRENT    150	//  

/*                          */
//                               
#define USB_CHARGER_CURRENT_SUSPEND			0		//                  
#define USB_CHARGER_CURRENT_UNCONFIGURED	CHARGE_CURRENT_70_00_MA	//     
#define USB_CHARGER_CURRENT_CONFIGURED		CHARGE_CURRENT_500_00_MA	//      

#define USB_CHARGER_CURRENT                 CHARGE_CURRENT_500_00_MA
#define AC_CHARGER_CURRENT                  CHARGE_CURRENT_700_00_MA
#define NON_STD_AC_CHARGER_CURRENT          CHARGE_CURRENT_700_00_MA
#define CHARGING_HOST_CHARGER_CURRENT       CHARGE_CURRENT_100_00_MA

#define APPLE_0_5A_CHARGER_CURRENT          CHARGE_CURRENT_100_00_MA
#define APPLE_1_0A_CHARGER_CURRENT          CHARGE_CURRENT_100_00_MA
#define APPLE_2_1A_CHARGER_CURRENT          CHARGE_CURRENT_100_00_MA

/*                     */
#define ADB_CURRENT_CMD_ENABLE

/*                 */
#define BATTERY_AVERAGE_DATA_NUMBER	3	
#define BATTERY_AVERAGE_SIZE 	30

/*                     */
//                                                                                             
#define V_CHARGER_ENABLE 0				//              
#define V_CHARGER_MAX 6500				//      
#define V_CHARGER_MIN 4400				//      

/*               */
#define ONEHUNDRED_PERCENT_TRACKING_TIME	10	//          
#define NPERCENT_TRACKING_TIME	   			20	//          
#define SYNC_TO_REAL_TRACKING_TIME  		60	//          
#define V_0PERCENT_TRACKING							3450 //      

/*                */
//                                         
//                                         
//                                          
//                                     
//                                                   

/*                 */
//                                  
#define CUST_SOC_JEITA_SYNC_TIME 30
#define JEITA_RECHARGE_VOLTAGE  4110	//                    
#define JEITA_TEMP_ABOVE_POS_60_CV_VOLTAGE		BATTERY_VOLT_04_100000_V
#define JEITA_TEMP_POS_45_TO_POS_60_CV_VOLTAGE	BATTERY_VOLT_04_100000_V
#define JEITA_TEMP_POS_10_TO_POS_45_CV_VOLTAGE	BATTERY_VOLT_04_200000_V
#define JEITA_TEMP_POS_0_TO_POS_10_CV_VOLTAGE	BATTERY_VOLT_04_100000_V
#define JEITA_TEMP_NEG_10_TO_POS_0_CV_VOLTAGE	BATTERY_VOLT_03_900000_V
#define JEITA_TEMP_BELOW_NEG_10_CV_VOLTAGE		BATTERY_VOLT_03_900000_V

/*                                */
#define JEITA_NEG_10_TO_POS_0_FULL_CURRENT  120	//   
#define JEITA_TEMP_POS_45_TO_POS_60_RECHARGE_VOLTAGE  4000
#define JEITA_TEMP_POS_10_TO_POS_45_RECHARGE_VOLTAGE  4100
#define JEITA_TEMP_POS_0_TO_POS_10_RECHARGE_VOLTAGE   4000
#define JEITA_TEMP_NEG_10_TO_POS_0_RECHARGE_VOLTAGE   3800
#define JEITA_TEMP_POS_45_TO_POS_60_CC2TOPOFF_THRESHOLD	4050
#define JEITA_TEMP_POS_10_TO_POS_45_CC2TOPOFF_THRESHOLD	4050
#define JEITA_TEMP_POS_0_TO_POS_10_CC2TOPOFF_THRESHOLD	4050
#define JEITA_TEMP_NEG_10_TO_POS_0_CC2TOPOFF_THRESHOLD	3850


/*                      */
#define HIGH_BATTERY_VOLTAGE_SUPPORT

/*                               */
#ifdef CONFIG_MTK_DISABLE_POWER_ON_OFF_VOLTAGE_LIMITATION
#define CONFIG_DIS_CHECK_BATTERY
#endif

#ifdef CONFIG_MTK_FAN5405_SUPPORT
#define FAN5405_BUSNUM 1
#endif

#ifdef CONFIG_MTK_BQ24158_SUPPORT
#define BQ24158_BUSNUM 1
#endif

//                                      

/*                                      */
#ifdef CONFIG_MTK_PUMP_EXPRESS_SUPPORT
#define TA_START_VCHR_TUNUNG_VOLTAGE	3700	//                      
#define TA_CHARGING_CURRENT			    CHARGE_CURRENT_1500_00_MA
#undef V_CHARGER_MAX
#ifdef TA_9V_SUPPORT
#define V_CHARGER_MAX 9500				//      
#else
#define V_CHARGER_MAX 6000				//      
#endif
#endif

#ifdef CONFIG_MTK_EXTERNAL_CHARGER_SUPPORT
#define CHARGING_FULL_VOLTAGE		4350
#define CHARGER_CV_VOLTAGE		BATTERY_VOLT_04_400000_V
#endif

#endif /*              */ 
