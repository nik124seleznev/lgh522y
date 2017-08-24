#ifndef _CUST_BATTERY_METER_H
#define _CUST_BATTERY_METER_H

#include <mach/mt_typedefs.h>

//                                                             
//       
//                                                             
//                     
//                    
#define SOC_BY_SW_FG

//                                
//                     

/*                    */
#define CUST_TABT_NUMBER 17
#define VBAT_CHANNEL_NUMBER      7
#define ISENSE_CHANNEL_NUMBER	 6
#define VCHARGER_CHANNEL_NUMBER  4
#define VBATTEMP_CHANNEL_NUMBER  5

/*               */
#define R_BAT_SENSE 4					
#define R_I_SENSE 4						
#define R_CHARGER_1 330
#define R_CHARGER_2 39

#define TEMPERATURE_T0             110
#define TEMPERATURE_T1             0
#define TEMPERATURE_T2             25
#define TEMPERATURE_T3             50
#define TEMPERATURE_T              255 //                                             

#define FG_METER_RESISTANCE 	0

/*                   */
#ifdef CONFIG_LGE_PM_BATTERY_PROFILE
/*                                                           */
#else
//            
#define Q_MAX_POS_50	1973
#define Q_MAX_POS_25	1956
#define Q_MAX_POS_0		1747
#define Q_MAX_NEG_10	1549

#define Q_MAX_POS_50_H_CURRENT	1901
#define Q_MAX_POS_25_H_CURRENT	1854
#define Q_MAX_POS_0_H_CURRENT	1465
#define Q_MAX_NEG_10_H_CURRENT	673
#endif

/*                      */
#define OAM_D5		 1		//                 


/*                         */
#define CUST_TRACKING_POINT  14
//                               
#define CUST_HW_CC 		    0
#define AGING_TUNING_VALUE   103
#define CUST_R_FG_OFFSET    0
#ifdef CONFIG_MTK_FAN5405_SUPPORT
#define CUST_R_SENSE         68
#elif defined(CONFIG_MTK_BQ24158_SUPPORT)
#define CUST_R_SENSE         68
#else
#define CUST_R_SENSE         200
#endif

#define OCV_BOARD_COMPESATE	0 //   
#define R_FG_BOARD_BASE		1000
#define R_FG_BOARD_SLOPE	1000 //     
#define CAR_TUNE_VALUE		94 //    

#ifdef CONFIG_LGE_PM_SOC_SCALE
#define SOC_RESCALING_FACTOR 98
#endif

/*                */
#define CURRENT_DETECT_R_FG	10  //   
#define MinErrorOffset       1000
#define FG_VBAT_AVERAGE_SIZE 18
#define R_FG_VALUE 			0 //                 

#ifdef CONFIG_LGE_PM_BATTERY_PROFILE
#define CUST_POWERON_DELTA_CAPACITY_TOLRANCE		20
#define CUST_POWERON_LOW_CAPACITY_TOLRANCE		5
#define CUST_POWERON_MAX_VBAT_TOLRANCE			90
#define CUST_POWERON_DELTA_VBAT_TOLRANCE		30
#else /*              */
#define CUST_POWERON_DELTA_CAPACITY_TOLRANCE	40
#define CUST_POWERON_LOW_CAPACITY_TOLRANCE		5
#define CUST_POWERON_MAX_VBAT_TOLRANCE			90
#define CUST_POWERON_DELTA_VBAT_TOLRANCE		30
#endif

/*                               */
#ifdef CONFIG_MTK_DISABLE_POWER_ON_OFF_VOLTAGE_LIMITATION
#define FIXED_TBAT_25
#endif

/*                                                             */
#define VBAT_NORMAL_WAKEUP		3600		//    
#define VBAT_LOW_POWER_WAKEUP		3500		//    
#define NORMAL_WAKEUP_PERIOD		5400 		//                
#define LOW_POWER_WAKEUP_PERIOD		300		//              
#define CLOSE_POWEROFF_WAKEUP_PERIOD	30	//    

#endif	//                             
