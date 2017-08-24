/*
 * MAX8971 Charger Driver
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __LINUX_MAX8971_CHARGER_H
#define __LINUX_MAX8971_CHARGER_H

#define MAX8971_DEV_NAME        "charger_ic"   //        
#define MAX8971_DEV_ADDR        (u8)0x6A

//                                                                      

#define OMAP_SEND                               122  //                           

#define D(fmt, args...) //                                               

#define DLEVEL_0 0
#define DLEVEL_1 0
#define DLEVEL_2 0

#define MAX8971_DEBUG(level, format, args...) do{ \
    if(level) \
	{\
		printk(format,##args);\
	}\
}while(0)

/* 
               
 */

#define MAX8971_REG_CHGINT      0x0F

#define MAX8971_REG_CHGINT_MASK 0x01

#define MAX8971_REG_CHG_STAT    0x02

#define MAX8971_DCV_MASK        0x80
#define MAX8971_DCV_SHIFT       7
#define MAX8971_TOPOFF_MASK     0x40
#define MAX8971_TOPOFF_SHIFT    6
#define MAX8971_DCI_OK          0x40    //                    
#define MAX8971_DCI_OK_SHIFT    6
#define MAX8971_DCOVP_MASK      0x20
#define MAX8971_DCOVP_SHIFT     5
#define MAX8971_DCUVP_MASK      0x10
#define MAX8971_DCUVP_SHIFT     4
#define MAX8971_CHG_MASK        0x08
#define MAX8971_CHG_SHIFT       3
#define MAX8971_BAT_MASK        0x04
#define MAX8971_BAT_SHIFT       2
#define MAX8971_THM_MASK        0x02
#define MAX8971_THM_SHIFT       1
#define MAX8971_PWRUP_OK_MASK   0x01
#define MAX8971_PWRUP_OK_SHIFT  0
#define MAX8971_I2CIN_MASK      0x01
#define MAX8971_I2CIN_SHIFT     0

#define MAX8971_REG_DETAILS1    0x03
#define MAX8971_DC_V_MASK       0x80
#define MAX8971_DC_V_SHIFT      7
#define MAX8971_DC_I_MASK       0x40
#define MAX8971_DC_I_SHIFT      6
#define MAX8971_DC_OVP_MASK     0x20
#define MAX8971_DC_OVP_SHIFT    5
#define MAX8971_DC_UVP_MASK     0x10
#define MAX8971_DC_UVP_SHIFT    4
#define MAX8971_THM_DTLS_MASK   0x07
#define MAX8971_THM_DTLS_SHIFT  0

#define MAX8971_THM_DTLS_COLD   1       //                                   
#define MAX8971_THM_DTLS_COOL   2       //                    
#define MAX8971_THM_DTLS_NORMAL 3       //                    
#define MAX8971_THM_DTLS_WARM   4       //                    
#define MAX8971_THM_DTLS_HOT    5       //                                   

#define MAX8971_REG_DETAILS2    0x04
#define MAX8971_BAT_DTLS_MASK   0x30
#define MAX8971_BAT_DTLS_SHIFT  4
#define MAX8971_CHG_DTLS_MASK   0x0F
#define MAX8971_CHG_DTLS_SHIFT  0

#define MAX8971_BAT_DTLS_BATDEAD        0   //          
#define MAX8971_BAT_DTLS_TIMER_FAULT    1   //                                                     
#define MAX8971_BAT_DTLS_BATOK          2   //              
#define MAX8971_BAT_DTLS_GTBATOV        3   //             

#define MAX8971_CHG_DTLS_DEAD_BAT           0   //                           
#define MAX8971_CHG_DTLS_PREQUAL            1   //                           
#define MAX8971_CHG_DTLS_FAST_CHARGE_CC     2   //                           
#define MAX8971_CHG_DTLS_FAST_CHARGE_CV     3   //                              
#define MAX8971_CHG_DTLS_TOP_OFF            4   //                               
#define MAX8971_CHG_DTLS_DONE               5   //                                             
#define MAX8971_CHG_DTLS_TIMER_FAULT        6   //                       
#define MAX8971_CHG_DTLS_TEMP_SUSPEND       7   //                   
#define MAX8971_CHG_DTLS_USB_SUSPEND        8   //                                                                  
#define MAX8971_CHG_DTLS_THERMAL_LOOP_ACTIVE    9   //             
#define MAX8971_CHG_DTLS_CHG_OFF            10  //                             

#define MAX8971_REG_CHGCNTL1    0x05
#define MAX8971_DCMON_DIS_MASK  0x02
#define MAX8971_DCMON_DIS_SHIFT 1
#define MAX8971_USB_SUS_MASK    0x01
#define MAX8971_USB_SUS_SHIFT   0

#define MAX8971_REG_FCHGCRNT    0x06
#define MAX8971_CHGCC_MASK      0x1F
#define MAX8971_CHGCC_SHIFT     0
#define MAX8971_FCHGTIME_MASK   0xE0
#define MAX8971_FCHGTIME_SHIFT  5


#define MAX8971_REG_DCCRNT      0x07
#define MAX8971_CHGRSTRT_MASK   0x40
#define MAX8971_CHGRSTRT_SHIFT  6
#define MAX8971_DCILMT_MASK     0x3F
#define MAX8971_DCILMT_SHIFT    0

#define MAX8971_REG_TOPOFF          0x08
#define MAX8971_TOPOFFTIME_MASK     0xE0
#define MAX8971_TOPOFFTIME_SHIFT    5
#define MAX8971_IFST2P8_MASK        0x10
#define MAX8971_IFST2P8_SHIFT       4
#define MAX8971_TOPOFFTSHLD_MASK    0x0C
#define MAX8971_TOPOFFTSHLD_SHIFT   2
#define MAX8971_CHGCV_MASK          0x03
#define MAX8971_CHGCV_SHIFT         0

#define MAX8971_REG_TEMPREG     0x09
#define MAX8971_REGTEMP_MASK    0xC0
#define MAX8971_REGTEMP_SHIFT   6
#define MAX8971_THM_CNFG_MASK   0x08
#define MAX8971_THM_CNFG_SHIFT  3
#define MAX8971_SAFETYREG_MASK  0x01
#define MAX8971_SAFETYREG_SHIFT 0

#define MAX8971_REG_PROTCMD     0x0A
#define MAX8971_CHGPROT_MASK    0x0C
#define MAX8971_CHGPROT_SHIFT   2 

/* 
         
*/

#define TEMP_LOW_NO_BAT                 -300
#define TEMP_LOW_DISCHARGING    -100
#define TEMP_HIGH_DISCHARGING   550

#define TEMP_LOW_RECHARGING             -50
#define TEMP_HIGH_RECHARGING            500

#define TEMP_CHANGE_CHARGING_MODE      450    /*                                                                                                      */

#define RECHARGING_BAT_SOC_CON  97

/*                    */
enum power_supply_type get_charging_ic_status(void);

void charging_ic_active_default(void);
void charging_ic_deactive(void);

typedef enum {
        CHARG_FSM_CAUSE_ANY = 0,
        CHARG_FSM_CAUSE_CHARGING_TIMER_EXPIRED,
}charger_fsm_cause;
void charger_fsm(charger_fsm_cause reason);


//                                               
int twl6030battery_temperature(void);
int get_bat_soc(void);
struct delayed_work* get_charger_work(void);


typedef enum {
        FACTORY_CHARGER_ENABLE,
        FACTORY_CHARGER_DISABLE,
}charge_factory_cmd;

typedef enum {
        CHARGER_DISABLE,
        BATTERY_NO_CHARGER,
        CHARGER_NO_BATTERY,
        CHARGER_AND_BATTERY,
}charge_enable_state_t ;

typedef enum {
        RECHARGING_WAIT_UNSET,
        RECHARGING_WAIT_SET,
}recharging_state_t;


typedef enum {
        CHARGER_LOGO_STATUS_UNKNOWN,
        CHARGER_LOGO_STATUS_STARTED,
        CHARGER_LOGO_STATUS_END,
}charger_logo_state_t;


//                                                                           

#define FCHG_CURRENT(x) ((x-250)/50+5)  //                                          

enum {
    MAX8971_FCHGTIME_DISABLE,
    MAX8974_FCHGTIME_4HRS,
    MAX8974_FCHGTIME_5HRS,  
    MAX8974_FCHGTIME_6HRS,  
    MAX8974_FCHGTIME_7HRS,  
    MAX8974_FCHGTIME_8HRS,  
    MAX8974_FCHGTIME_9HRS,  
    MAX8974_FCHGTIME_10HRS,  
};

enum {
    MAX8971_TOPOFFTIME_0MIN,
    MAX8971_TOPOFFTIME_10MIN,
    MAX8971_TOPOFFTIME_20MIN,
    MAX8971_TOPOFFTIME_30MIN,
    MAX8971_TOPOFFTIME_40MIN,
    MAX8971_TOPOFFTIME_50MIN,
    MAX8971_TOPOFFTIME_60MIN,
    MAX8971_TOPOFFTIME_70MIN,
};

enum {
    MAX8971_TOPOFFTSHLD_50mA,
    MAX8971_TOPOFFTSHLD_100mA,
    MAX8971_TOPOFFTSHLD_150mA,
    MAX8971_TOPOFFTSHLD_200mA,
};

enum {
    MAX8971_CHGCV_4P20V,
    MAX8971_CHGCV_4P10V,
    MAX8971_CHGCV_4P35V,
};

enum {
    MAX8971_CHGRSTRT_150mV,
    MAX8971_CHGRSTRT_100mV,
};

#define DCI_LIMIT(x) ((x<=100) ? 0 :           \
					  (x>=250 && x<=1550) ? ((x-250)/25+10) : -EINVAL)

enum {
    MAX8971_REGTEMP_105degree,
    MAX8971_REGTEMP_90degree,
    MAX8971_REGTEMP_120degree,
    MAX8971_REGTEMP_DISABLE,
};

enum {
    MAX8971_THM_CNFG_CONTINUOUS,
    MAX8971_THM_CNFG_NOT_MONITOR,
};

enum {
    MAX8971_SAFETYREG_REGION1,
    MAX8971_SAFETYREG_REGION2,
};

#define MAX8971_CHGPROT_LOCKED      0x00
#define MAX8971_CHGPROT_UNLOCKED    0x03

/*                 */
enum {
	MAX8971_IRQ_PWRUP_OK =0,
	MAX8971_IRQ_THM,
	MAX8971_IRQ_BAT,
	MAX8971_IRQ_CHG,
	MAX8971_IRQ_DCUVP,
	MAX8971_IRQ_DCOVP,
	//                
	MAX8971_IRQ_TOPOFF,
	MAX8971_IRQ_DCV,      
	MAX8971_NR_IRQS,
};

struct max8971_platform_data {
    unsigned char      chgcc;              //                    
    unsigned char      fchgtime;           //                 

    unsigned char      chgrstrt;           //                              
    unsigned char      dcilmt;             //                              

    unsigned char      topofftime;         //                       
    unsigned char      topofftshld;        //                       
    unsigned char      chgcv;              //                            
    unsigned char      ifst2p8;            //                  

    unsigned char      regtemp;            //                                                 
    unsigned char      safetyreg;          //                              
    unsigned char      thm_config;         //                              

    unsigned char      int_mask;           //            
};


#define GPIO_CHG_STATUS_INT     51

int max8971_stop_charging(void);
int max8971_stop_factory_charging(void);
int max8971_start_charging(unsigned mA);
void charger_fsm_max8971(charger_fsm_cause fsm_cause);
//                       
void set_boot_charging_mode(int charging_mode);
int max8971_start_Factory_charging(void);
int max8971_check_eoc_status(void);
    
extern int charging_done_flag ;

int get_bat_volt(void);
int get_unlimited_temp(void);
int device_power_control(char *reg_id, int on);
#endif

