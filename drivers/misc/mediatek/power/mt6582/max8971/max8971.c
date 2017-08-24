
/*
 * MAXIM MAX8971 Charger Driver
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 */


#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/i2c.h>
#include <linux/err.h>
#include <linux/interrupt.h>
#include <linux/device.h>
#include <linux/sysfs.h>
#include <linux/platform_device.h>
#include <linux/power_supply.h>
#include <linux/xlog.h>

#include <linux/delay.h>
/*                                                 */
#if defined(CONFIG_MUIC)
//                            
//                                   
#else
//                            
#endif
/*                                      */

//                                            
#include <mach/mt_typedefs.h>
#include <mach/mt_gpio.h>
#include "max8971.h"
#include <cust_gpio_usage.h>
#include <cust_gpio_boot.h>

#include <linux/delay.h>
#include <linux/hrtimer.h>
#include <linux/proc_fs.h>              //                                  

#define MAX8971_DEV_I2C_NAME        "charger_ic_i2c"

//                           

int MST_test_mode=0;
int charger_source=0;

kal_bool chargin_hw_init_done = KAL_FALSE; 

#define UNLIMITED_TEMP_VAL      0xA4

#define CHG_EN_SET_N_OMAP               83

#define CHR_IC_DEALY                            200     /*        */
#define CHR_IC_SET_DEALY                        1500    /*         */
#define CHR_TIMER_SECS                          3600 /*          */
#define TEMP_NO_BATTERY  1   //                                                       

//                                 

#ifdef MAX8971_TOPOFF_WORKAROUND
#define MAX8971_TOPOFF_DELAY    ((HZ) * 60 + 0 /*                */ )  //                           
#endif

struct max8971_chip *test_chip;
static DEFINE_MUTEX(charging_lock);
//                   
struct delayed_work     charger_timer_work;
struct timer_list charging_timer;
enum power_supply_type charging_ic_status;


struct max8971_chip {
        struct i2c_client *client;

#ifdef MAX8971_TOPOFF_WORKAROUND
	struct delayed_work topoff_work;
        u8 start_topoff_work;
	u8 prev_chg_dtl;
#endif
	struct power_supply charger;
	struct max8971_platform_data *pdata;
    int irq;
    int chg_online;
};

static struct max8971_platform_data max8971_data = {

//                                                                    
//                                                                    
//                                                                    
        .chgcc = 0x18,	                //                             

//                                                                
        .fchgtime = 0x00,               //                           

//                                                                              
        .chgrstrt = 0x01,               //                                      

//                                                                           
        .dcilmt = 0x3C,                 //                                     

//                                                                       
        .topofftime = 0x00,             //                              
//                                                                       

        .topofftshld = 0x03,            //                               
//                                                                       

        .chgcv = 0x02,                  //                                     
//                                                                           

        .ifst2p8= 0x00,

//                                                                                         
//                                                                      
        .thm_config = 0x1,              //                                                   

        .int_mask = 0xFF,               //                        
//                                                                   
//                                                               
};

static struct i2c_board_info __initdata max8971_i2c_data=
{
                I2C_BOARD_INFO(MAX8971_DEV_I2C_NAME, 0x35),
                .platform_data = &max8971_data,
};

static struct max8971_chip *max8971_chg;

static int max8971_write_reg(struct i2c_client *client, u8 reg, u8 value)
{
	int i=3;
	int ret;

	while (i--) {
        ret= i2c_smbus_write_byte_data(client, reg, value);

        if (ret < 0)
				printk("***** max8971_write_reg error *****\n");
		else
		break;
	}

        return ret;


}

enum power_supply_type get_charging_ic_status()
{
        return charging_ic_status;
}
EXPORT_SYMBOL(get_charging_ic_status);

void set_boot_charging_mode(int charging_mode)
{
        charging_ic_status = charging_mode;
       //                                                    
}
EXPORT_SYMBOL(set_boot_charging_mode);

static int max8971_read_reg(struct i2c_client *client, u8 reg)
{
	int i=3;
	int ret;
	
	while(i--){
		ret=i2c_smbus_read_byte_data(client, reg);

	 if (ret < 0)
				printk("***** max8971_read_reg error *****\n");
		else
		break;
	
	}
	return ret;
}

static int max8971_set_bits(struct i2c_client *client, u8 reg, u8 mask, u8 data)
{
	u8 value = 0;
	int ret;

	ret = max8971_read_reg(client, reg);
	if (ret < 0)
		goto out;
	value &= ~mask;
	value |= data;
	ret = max8971_write_reg(client, reg, value);
out:
	return ret;
}


static int __set_charger(struct max8971_chip *chip, int enable)
{
    u8  reg_val= 0;
	printk("[Max8971] set charger called\n");

//                         
    //                          
    reg_val = MAX8971_CHGPROT_UNLOCKED<<MAX8971_CHGPROT_SHIFT;
    max8971_write_reg(chip->client, MAX8971_REG_PROTCMD, reg_val);   
    MAX8971_DEBUG(DLEVEL_0, "[LGE_MAX8971] MAX8971_REG_PROTCMD[0x%x] = 0x%x\n", MAX8971_REG_PROTCMD, reg_val);
//                                        
	if (enable) {
		/*                */

        //                                  
        reg_val = ((chip->pdata->chgcc<<MAX8971_CHGCC_SHIFT) |
                   (chip->pdata->fchgtime<<MAX8971_FCHGTIME_SHIFT));
        max8971_write_reg(chip->client, MAX8971_REG_FCHGCRNT, reg_val);
        MAX8971_DEBUG(DLEVEL_0, "[LGE_MAX8971] MAX8971_REG_FCHGCRNT[0x%x] = 0x%x\n", MAX8971_REG_FCHGCRNT, reg_val);

       //                                                      
        reg_val = ((chip->pdata->chgrstrt<<MAX8971_CHGRSTRT_SHIFT) |
                   (chip->pdata->dcilmt<<MAX8971_DCILMT_SHIFT));
        max8971_write_reg(chip->client, MAX8971_REG_DCCRNT, reg_val);
        MAX8971_DEBUG(DLEVEL_0, "[LGE_MAX8971] MAX8971_REG_DCCRNT[0x%x] = 0x%x\n", MAX8971_REG_DCCRNT, reg_val);
        
	if( MST_test_mode == 1){
		printk("[Max8971] MST_test_mode set chgcv 4.1V");
        	//                     
		chip->pdata->chgcv=0x01;
        	reg_val = ((chip->pdata->topofftime<<MAX8971_TOPOFFTIME_SHIFT) |
                   	(chip->pdata->topofftshld<<MAX8971_TOPOFFTSHLD_SHIFT) |
                   	(chip->pdata->chgcv<<MAX8971_CHGCV_SHIFT));
       		 max8971_write_reg(chip->client, MAX8971_REG_TOPOFF, reg_val);
             MAX8971_DEBUG(DLEVEL_0,"[LGE_MAX8971] MAX8971_REG_TOPOFF[0x%x] = 0x%x\n", MAX8971_REG_TOPOFF, reg_val);
	}else{
        //                     
        reg_val = ((chip->pdata->topofftime<<MAX8971_TOPOFFTIME_SHIFT) |
                   (chip->pdata->topofftshld<<MAX8971_TOPOFFTSHLD_SHIFT) |
                   (chip->pdata->chgcv<<MAX8971_CHGCV_SHIFT));
        max8971_write_reg(chip->client, MAX8971_REG_TOPOFF, reg_val);
        MAX8971_DEBUG(DLEVEL_0, "[LGE_MAX8971] MAX8971_REG_TOPOFF[0x%x] = 0x%x\n", MAX8971_REG_TOPOFF, reg_val);
	}

        //                          
        reg_val = ((chip->pdata->regtemp<<MAX8971_REGTEMP_SHIFT) |
                   (chip->pdata->thm_config<<MAX8971_THM_CNFG_SHIFT) |
                   (chip->pdata->safetyreg<<MAX8971_SAFETYREG_SHIFT));
        max8971_write_reg(chip->client, MAX8971_REG_TEMPREG, reg_val);       
        MAX8971_DEBUG(DLEVEL_0, "[LGE_MAX8971] MAX8971_REG_TEMPREG[0x%x] = 0x%x\n", MAX8971_REG_TEMPREG, reg_val);

        //                                      
        //                                                               
//                                                                            
//                                                                         

	} else {
		/*                */
        max8971_set_bits(chip->client, MAX8971_REG_CHGCNTL1, MAX8971_USB_SUS_MASK, 1);
	}
	MAX8971_DEBUG(DLEVEL_1,"[max8971]!!!!!!!!!!!!!!!! max8971 reg value debug start!!!!!!!!!!!!!!!!!!!!!!!!\n");
	reg_val=0;
	reg_val = max8971_read_reg(chip->client, MAX8971_REG_CHG_STAT);
	MAX8971_DEBUG(DLEVEL_1,"[max8971] max8971_charger MAX8971_REG_CHG_STAT[0x%x] = 0x%x\n",MAX8971_REG_CHG_STAT, reg_val);
	reg_val = max8971_read_reg(chip->client, MAX8971_REG_DETAILS1);
	MAX8971_DEBUG(DLEVEL_1,"[max8971] max8971_charger MAX8971_REG_DETAILS1[0x%x] = 0x%x\n",MAX8971_REG_DETAILS1,reg_val);
	reg_val = max8971_read_reg(chip->client, MAX8971_REG_DETAILS2);
	MAX8971_DEBUG(DLEVEL_1,"[max8971] max8971_charger MAX8971_REG_DETAILS2[0x%x] = 0x%x\n",MAX8971_REG_DETAILS2, reg_val);
	reg_val = max8971_read_reg(chip->client, MAX8971_REG_CHGCNTL1);
	MAX8971_DEBUG(DLEVEL_0,"[max8971] max8971_charger MAX8971_REG_CHGCNTL1 = 0x%x\n",reg_val);
	reg_val = max8971_read_reg(chip->client, MAX8971_REG_FCHGCRNT);
	MAX8971_DEBUG(DLEVEL_0,"[max8971] max8971_charger MAX8971_REG_FCHGCRNT = 0x%x\n",reg_val);
	reg_val = max8971_read_reg(chip->client, MAX8971_REG_DCCRNT);
	MAX8971_DEBUG(DLEVEL_0,"[max8971] max8971_charger MAX8971_REG_DCCRNT = 0x%x\n",reg_val);
	reg_val = max8971_read_reg(chip->client, MAX8971_REG_TOPOFF);
	MAX8971_DEBUG(DLEVEL_0,"[max8971] max8971_charger MAX8971_REG_TOPOFF = 0x%x\n",reg_val);
	reg_val = max8971_read_reg(chip->client, MAX8971_REG_TEMPREG);
	MAX8971_DEBUG(DLEVEL_0,"[max8971] max8971_charger MAX8971_REG_TEMPREG = 0x%x\n",reg_val);
	reg_val = max8971_read_reg(chip->client, MAX8971_REG_PROTCMD);
	MAX8971_DEBUG(DLEVEL_0,"[max8971] max8971_charger MAX8971_REG_PROTCMD = 0x%x\n",reg_val);
	MAX8971_DEBUG(DLEVEL_1,"[max8971]!!!!!!!!!!!!!!!! max8971 reg value debug end!!!!!!!!!!!!!!!!!!!!!!!!\n");

//                                                                                       
	return 0;

}

int max8971_check_eoc_status(void)
{
    u8 reg_val = 0;
    u8 eoc_status = 0;
    //      
    reg_val = max8971_read_reg(max8971_chg->client, MAX8971_REG_DETAILS2);
    eoc_status = (reg_val & MAX8971_CHG_DTLS_MASK) >> MAX8971_CHG_DTLS_SHIFT;

    printk("[LGE_MAX8971] eoc state!!! MAX8971_REG_DETAILS2[0x%x] = 0x%x \n", MAX8971_REG_DETAILS2, eoc_status);
    
    if( eoc_status == MAX8971_CHG_DTLS_DONE )
    {
        printk("[LGE_MAX8971] max8971_check_eoc_status = %d \n", eoc_status);
        return 1;
    }

    return 0;
}
EXPORT_SYMBOL(max8971_check_eoc_status);

int max8971_start_charging(unsigned mA)
{
    u8 reg_val=0;
    int i;

    printk("[LGE_MAX8971] max8971_start_charging (%d)mA \n", mA);
//                                                         
//                                                                       
//                                                                          
    reg_val = ((max8971_chg->pdata->chgrstrt<<MAX8971_CHGRSTRT_SHIFT) |
               (0x09<<MAX8971_DCILMT_SHIFT));				//      
    max8971_write_reg(max8971_chg->client, MAX8971_REG_DCCRNT, reg_val);
    MAX8971_DEBUG(DLEVEL_0, "[LGE_MAX8971] MAX8971_REG_DCCRNT[0x%x] = 0x%x\n", MAX8971_REG_DCCRNT, reg_val);
    
    if(mA==900)
    {
	charging_ic_status = POWER_SUPPLY_TYPE_MAINS;
	reg_val = (0<<MAX8971_DCMON_DIS_SHIFT) | (0<<MAX8971_USB_SUS_SHIFT);
        max8971_write_reg(max8971_chg->client, MAX8971_REG_CHGCNTL1, reg_val);
        dev_info(&max8971_chg->client->dev, "[LGE_MAX8971] MAX8971_REG_CHGCNTL1[0x%x] = 0x%x\n", MAX8971_REG_CHGCNTL1, reg_val);
    }
    else if(mA==850) 
    {
	charging_ic_status = POWER_SUPPLY_TYPE_MAINS;
        reg_val = (1<<MAX8971_DCMON_DIS_SHIFT) | (0<<MAX8971_USB_SUS_SHIFT);

        max8971_write_reg(max8971_chg->client, MAX8971_REG_CHGCNTL1, reg_val);
        dev_info(&max8971_chg->client->dev, "[LGE_MAX8971] MAX8971_REG_CHGCNTL1[0x%x] = 0x%x\n", MAX8971_REG_CHGCNTL1, reg_val);
    }
	else if(mA==1200) 
    {
	charging_ic_status = POWER_SUPPLY_TYPE_MAINS;
        reg_val = (1<<MAX8971_DCMON_DIS_SHIFT) | (0<<MAX8971_USB_SUS_SHIFT);

        max8971_write_reg(max8971_chg->client, MAX8971_REG_CHGCNTL1, reg_val);
        dev_info(&max8971_chg->client->dev, "[LGE_MAX8971] MAX8971_REG_CHGCNTL1[0x%x] = 0x%x\n", MAX8971_REG_CHGCNTL1, reg_val);
    }
    else if(mA==800) 
    {
        reg_val = (1<<MAX8971_DCMON_DIS_SHIFT) | (0<<MAX8971_USB_SUS_SHIFT);

        max8971_write_reg(max8971_chg->client, MAX8971_REG_CHGCNTL1, reg_val);
	charging_ic_status = POWER_SUPPLY_TYPE_MAINS;
        dev_info(&max8971_chg->client->dev, "[LGE_MAX8971] MAX8971_REG_CHGCNTL1[0x%x] = 0x%x\n", MAX8971_REG_CHGCNTL1,reg_val);
    }
    else if(mA==600) 
    {
        printk( "[LGE_MAX8971]  600mA ");
        reg_val = (0<<MAX8971_DCMON_DIS_SHIFT) | (0<<MAX8971_USB_SUS_SHIFT);
        max8971_write_reg(max8971_chg->client, MAX8971_REG_CHGCNTL1, reg_val);
        dev_info(&max8971_chg->client->dev, "[LGE_MAX8971] MAX8971_REG_CHGCNTL1[0x%x] = 0x%x\n", MAX8971_REG_CHGCNTL1,reg_val);
    }
    else if(mA==500) 
    {
	//                                            
	if(charger_source==2)
       	    reg_val = (1<<MAX8971_DCMON_DIS_SHIFT) | (0<<MAX8971_USB_SUS_SHIFT);
	else
            reg_val = (0<<MAX8971_DCMON_DIS_SHIFT) | (0<<MAX8971_USB_SUS_SHIFT);

        max8971_write_reg(max8971_chg->client, MAX8971_REG_CHGCNTL1, reg_val);
        dev_info(&max8971_chg->client->dev, "[LGE_MAX8971] MAX8971_REG_CHGCNTL1[0x%x] = 0x%x\n", MAX8971_REG_CHGCNTL1,reg_val);
    }
    else if(mA==450) 
    {
	charging_ic_status = POWER_SUPPLY_TYPE_USB;
	if(MST_test_mode == 1 ){
        	reg_val = (1<<MAX8971_DCMON_DIS_SHIFT) | (0<<MAX8971_USB_SUS_SHIFT);
	}
	else
        	reg_val = (0<<MAX8971_DCMON_DIS_SHIFT) | (0<<MAX8971_USB_SUS_SHIFT);
		
        max8971_write_reg(max8971_chg->client, MAX8971_REG_CHGCNTL1, reg_val);
        dev_info(&max8971_chg->client->dev, "[LGE_MAX8971] MAX8971_REG_CHGCNTL1[0x%x] = 0x%x\n", MAX8971_REG_CHGCNTL1,reg_val);
    }

    else if(mA==1550)
    {
#ifdef _TEST_
	charging_ic_status = POWER_SUPPLY_TYPE_FACTORY ;
#endif /*        */
	reg_val = (1<<MAX8971_DCMON_DIS_SHIFT) | (0<<MAX8971_USB_SUS_SHIFT);
        max8971_write_reg(max8971_chg->client, MAX8971_REG_CHGCNTL1, reg_val);
        dev_info(&max8971_chg->client->dev, "[LGE_MAX8971] MAX8971_REG_CHGCNTL1[0x%x] = 0x%x\n", MAX8971_REG_CHGCNTL1,reg_val);
    }
    else if(mA==400)  
    {
	if(charger_source==2)
	    reg_val = (1<<MAX8971_DCMON_DIS_SHIFT) | (0<<MAX8971_USB_SUS_SHIFT);
	else
            reg_val = (0<<MAX8971_DCMON_DIS_SHIFT) | (0<<MAX8971_USB_SUS_SHIFT);
	max8971_write_reg(max8971_chg->client, MAX8971_REG_CHGCNTL1, reg_val);
        dev_info(&max8971_chg->client->dev, "[LGE_MAX8971] MAX8971_REG_CHGCNTL1[0x%x] = 0x%x\n", MAX8971_REG_CHGCNTL1,reg_val);
    }
    else if(mA==350)  
    {
//                                             
	reg_val = (1<<MAX8971_DCMON_DIS_SHIFT) | (0<<MAX8971_USB_SUS_SHIFT);
	max8971_write_reg(max8971_chg->client, MAX8971_REG_CHGCNTL1, reg_val);
        dev_info(&max8971_chg->client->dev, "[LGE_MAX8971] MAX8971_REG_CHGCNTL1[0x%x] = 0x%x", MAX8971_REG_CHGCNTL1,reg_val);
    }
    else if(mA==300)  
    {
//                                             
	reg_val = (1<<MAX8971_DCMON_DIS_SHIFT) | (0<<MAX8971_USB_SUS_SHIFT);
	max8971_write_reg(max8971_chg->client, MAX8971_REG_CHGCNTL1, reg_val);
        dev_info(&max8971_chg->client->dev, "[LGE_MAX8971] MAX8971_REG_CHGCNTL1[0x%x] = 0x%x\n", MAX8971_REG_CHGCNTL1, reg_val);
    }
    else if(mA==250)  
    {
//                                             
	reg_val = (0<<MAX8971_DCMON_DIS_SHIFT) | (0<<MAX8971_USB_SUS_SHIFT);
	max8971_write_reg(max8971_chg->client, MAX8971_REG_CHGCNTL1, reg_val);
        dev_info(&max8971_chg->client->dev, "[LGE_MAX8971] MAX8971_REG_CHGCNTL1[0x%x] = 0x%x\n", MAX8971_REG_CHGCNTL1, reg_val);
    }

    else
	printk("[max8971_start_chargin] can not find current\n");


   //                                              
    reg_val = max8971_chg->pdata->int_mask=0xE3 ;
    max8971_write_reg(max8971_chg->client,MAX8971_REG_CHGINT_MASK , reg_val);
    //                 
    max8971_chg->chg_online = 1;
    max8971_chg->pdata->chgcc = FCHG_CURRENT(mA);
    printk("[max8971] max8971_chg->pdata->chgcc: 0x%x \n", max8971_chg->pdata->chgcc);

    reg_val = ((FCHG_CURRENT(mA)<<MAX8971_CHGCC_SHIFT) |
               (max8971_chg->pdata->fchgtime<<MAX8971_FCHGTIME_SHIFT));
    max8971_write_reg(max8971_chg->client, MAX8971_REG_FCHGCRNT, reg_val);
    MAX8971_DEBUG(DLEVEL_0, "[LGE_MAX8971] MAX8971_REG_FCHGCRNT[0x%x] = 0x%x\n", MAX8971_REG_FCHGCRNT, reg_val);
    
    for (i=0x09; i<=0x1F; i++)
    {
        reg_val = ((max8971_chg->pdata->chgrstrt<<MAX8971_CHGRSTRT_SHIFT) |
                   (i<<MAX8971_DCILMT_SHIFT));
        max8971_write_reg(max8971_chg->client, MAX8971_REG_DCCRNT, reg_val);
 //                                                          
      //                                                       
//                                                                           
  //                                                                            
//                                                             
        MAX8971_DEBUG(DLEVEL_2,  "[LGE_MAX8971] MAX8971_REG_CHGCNTL1[0x%x] = 0x%x\n", MAX8971_REG_DCCRNT, reg_val);
        udelay(200);
    }
    reg_val = ((max8971_chg->pdata->chgrstrt<<MAX8971_CHGRSTRT_SHIFT) |
               (max8971_chg->pdata->dcilmt<<MAX8971_DCILMT_SHIFT));
    max8971_write_reg(max8971_chg->client, MAX8971_REG_DCCRNT, reg_val);
    MAX8971_DEBUG(DLEVEL_0, "[LGE_MAX8971] MAX8971_REG_CHGCNTL1[0x%x] = 0x%x\n", MAX8971_REG_DCCRNT, reg_val);
    
    __set_charger(max8971_chg, 1);



    return 0;
}
EXPORT_SYMBOL(max8971_start_charging);

#if 0 //         
static int max8971_charger_detail_irq(int irq, void *data, u8 *val)
{
	struct max8971_chip *chip = (struct max8971_chip *)data;
	printk("[max8971] max8971_charger_detail_irq called \n");
    switch (irq) 
    {
    case MAX8971_IRQ_PWRUP_OK:
 //                                                               
        if ((val[0] & MAX8971_DCUVP_MASK) == 0) {
            //                               
            //                 
            //                           
            max8971_write_reg(chip->client, MAX8971_REG_CHGINT_MASK, chip->pdata->int_mask);            
            //                              
            chip->chg_online = 1;
        //                          
        }
        break;

    case MAX8971_IRQ_THM:
  //                                                                                                             
        break;

    case MAX8971_IRQ_BAT:
        dev_info(&chip->client->dev, "Battery Interrupt: details-0x%x\n", (val[2] & MAX8971_BAT_DTLS_MASK));
        switch ((val[2] & MAX8971_BAT_MASK)>>MAX8971_BAT_SHIFT) 
        {
        case MAX8971_BAT_DTLS_BATDEAD:
            break;
        case MAX8971_BAT_DTLS_TIMER_FAULT:
            break;
        case MAX8971_BAT_DTLS_BATOK:
            break;
        case MAX8971_BAT_DTLS_GTBATOV:
            break;
        default:
            break;
        }
        break;

    case MAX8971_IRQ_CHG:
  //                                                                                                              
        switch (val[2] & MAX8971_CHG_DTLS_MASK) 
        {
        case MAX8971_CHG_DTLS_DEAD_BAT:
            //                                                             
       		dev_info(&chip->client->dev, "MAX8971_CHG_DTLS_DEAD_BAT\n");
            break;
        case MAX8971_CHG_DTLS_PREQUAL:
            //                                                   
            break;
        case MAX8971_CHG_DTLS_FAST_CHARGE_CC:
            dev_info(&chip->client->dev, "MAX8971_CHG_DTLS_FAST_CHARGE_CC\n");
            //                                                   
            break;
        case MAX8971_CHG_DTLS_FAST_CHARGE_CV:
	    dev_info(&chip->client->dev, "MAX8971_CHG_DTLS_FAST_CHARGE_CV\n");
            //                                                   
            break;
        case MAX8971_CHG_DTLS_TOP_OFF:
	    dev_info(&chip->client->dev, "MAX8971_CHG_DTLS_TOP_OFF\n");
            //                                                   
            break;
        case MAX8971_CHG_DTLS_DONE:
            //                                                   
	    dev_info(&chip->client->dev, "MAX8971_CHG_DTLS_DONE\n");
//                            
            //                                           
            break;
        case MAX8971_CHG_DTLS_TIMER_FAULT:
	    dev_info(&chip->client->dev, "MAX8971_CHG_DTLS_TIMER_FAULT\n");
            //                                                   
            break;
        case MAX8971_CHG_DTLS_TEMP_SUSPEND:
	    dev_info(&chip->client->dev, "MAX8971_CHG_DTLS_TEMP_SUSPEND\n");
            //                                                   
            break;
        case MAX8971_CHG_DTLS_USB_SUSPEND:
	    dev_info(&chip->client->dev, "MAX8971_CHG_DTLS_USB_SUSPEND\n");
            //                                                   
            break;
        case MAX8971_CHG_DTLS_THERMAL_LOOP_ACTIVE:
	    dev_info(&chip->client->dev, "MAX8971_CHG_DTLS_THERMAL_LOOP_ACTIVE \n");
            //                                                   
            break;
        case MAX8971_CHG_DTLS_CHG_OFF:
	    dev_info(&chip->client->dev, "MAX8971_CHG_DTLS_CHG_OFF \n");
            //                                                   
            break;
        default:
            break;
        }
        break;

    case MAX8971_IRQ_DCUVP:
        if ((val[1] & MAX8971_DC_UVP_MASK) == 0) {
            //                                
            //                       
        }
        dev_info(&chip->client->dev, "DC Under voltage Interrupt: details-0x%x\n", (val[1] & MAX8971_DC_UVP_MASK));
/*                                               
  
                                                  
  */
        break;

    case MAX8971_IRQ_DCOVP:
        if (val[1] & MAX8971_DC_OVP_MASK) {
            //                                
//                                   
        }
        dev_info(&chip->client->dev, "DC Over voltage Interrupt: details-0x%x\n", (val[1] & MAX8971_DC_OVP_MASK));
        break;

//                         
//                                                                                                                       
//              
    case  MAX8971_IRQ_TOPOFF:
        dev_info(&chip->client->dev, "DC MAX8971_IRQ_TOPOFF Interrupt: details-0x%x\n", (val[1] & MAX8971_TOPOFF_MASK));
        break;


    case MAX8971_IRQ_DCV:
        dev_info(&chip->client->dev, "DC Input Voltage Limit Interrupt: details-0x%x\n", (val[1] & MAX8971_DC_V_MASK));
        break;

    }
    return 0;
}

static irqreturn_t max8971_charger_handler(int irq, void *data)
{	
    struct max8971_chip *chip = (struct max8971_chip *)data;
    int irq_val, irq_mask, irq_name;
    u8 val[3];
//                                     
	printk("[max8971] max8971_charger_handler called ");

    irq_val = max8971_read_reg(chip->client, MAX8971_REG_CHGINT);
    if(irq_val < 0)
	return 0;   
    irq_mask = max8971_read_reg(chip->client, MAX8971_REG_CHGINT_MASK);
    if(irq_mask < 0)
	return 0;   

    val[0] = max8971_read_reg(chip->client, MAX8971_REG_CHG_STAT);
    if( val[0] < 0)
	return 0;   
    val[1] = max8971_read_reg(chip->client, MAX8971_REG_DETAILS1);
    if( val[1] < 0)
	return 0;   
    val[2] = max8971_read_reg(chip->client, MAX8971_REG_DETAILS2);
    if( val[2] < 0)
	return 0;   

        dev_info(&chip->client->dev, "MAX8971_REG_CHG_STAT- 0x%x\n", val[0]);
        dev_info(&chip->client->dev, "MAX8971_REG_DETAILS1- 0x%x\n", val[1]);
        dev_info(&chip->client->dev, "MAX8971_REG_DETAILS2- 0x%x\n", val[2]);

    for (irq_name = MAX8971_IRQ_PWRUP_OK; irq_name<MAX8971_NR_IRQS; irq_name++) {
        if ((irq_val & (0x01<<irq_name)) && !(irq_mask & (0x01<<irq_name))) {
            max8971_charger_detail_irq(irq_name, data, val);
        }
    }
	return IRQ_HANDLED;
}
#endif /*   */

int max8971_stop_charging(void)
{
    u8 reg_val=0;
//                                                 
//                                                                             
    //                
    if(max8971_chg != NULL )
    {
	printk(" max8971_chg pointer is not NULL\n");
     	max8971_chg->chg_online = 0;
    	__set_charger(max8971_chg, 0);
    }
    else
	printk(" max8971_chg pointer is NULL\n");

        charging_ic_status = POWER_SUPPLY_TYPE_BATTERY;
    //                      
  //                                                                                                            

    return reg_val;
}
EXPORT_SYMBOL(max8971_stop_charging);

int max8971_stop_factory_charging(void)
{
    u8 reg_val=0;
    //                
    if(max8971_chg != NULL)
    {
	printk(" max8971_chg pointer is not NULL\n");
    	max8971_chg->chg_online = 0;
    	__set_charger(max8971_chg, 0);

    //                      
    max8971_set_bits(max8971_chg->client, MAX8971_REG_TOPOFF, MAX8971_IFST2P8_MASK, 0<<MAX8971_IFST2P8_SHIFT);
    }
    else
        printk(" max8971_chg pointer is NULL\n");
    
    return reg_val;
}
EXPORT_SYMBOL(max8971_stop_factory_charging);

/*
                                       
 
                 
          

                                                         
                                                                       
                                                                          

               
     
                                              
                                                                     
                                                                              
                                                                                                  
     
                     
     
                                              
                                                                            

                                                                              
                                                                                                  
     
                     
     
                                                                            

                                                                              
                                              
                                                                                                  
     
                     
     
                                               
                                                                            
                                                                              
                                                                                                  
     
                     
     
                                               
                                                                            
                                                                              
                                                                                                  
     
                     
     
                                            
                                                                            
                                                                              
                                                                                                  
     

                     
     
                                                 
                                                                     
                                                                              
                                                                                                  
     
                      
     
                                             
                                                                     
                                                                       
                                                                                                  
     
                      
     
                                               
                                                                     
                                                                       
                                                                                                  
     
                      
     
                                               
                                                                     
                                                                       
                                                                                                  
     
                      
     
                                               
                                                                     
                                                                       
                                                                                                  
     

        
                                                          


                                                   
                                                 
                                                                             
                       
                                
                                                 
                                                                                      

                              
     
                                                             
                                                             
                                                                           
                                                                              
                                                            
                    
     
    
                                  



             
 
                                      

*/

int max8971_start_Factory_charging(void)
{
    u8 reg_val=0;

    printk("[max8971] max8971_start_Factory_charging\n");    
#ifdef _TEST_
    charging_ic_status = POWER_SUPPLY_TYPE_FACTORY;
#endif /*        */

    reg_val = MAX8971_CHGPROT_UNLOCKED<<MAX8971_CHGPROT_SHIFT;
    max8971_write_reg(max8971_chg->client, MAX8971_REG_PROTCMD, reg_val);   
    reg_val = max8971_chg->pdata->int_mask=0xF3 ;
    max8971_write_reg(max8971_chg->client,MAX8971_REG_CHGINT_MASK , reg_val);
    max8971_chg->chg_online = 1;
    reg_val = (1<<MAX8971_DCMON_DIS_SHIFT) | (0<<MAX8971_USB_SUS_SHIFT);
    max8971_write_reg(max8971_chg->client, MAX8971_REG_CHGCNTL1, reg_val);
    max8971_set_bits(max8971_chg->client, MAX8971_REG_TOPOFF, MAX8971_IFST2P8_MASK, 1<<MAX8971_IFST2P8_SHIFT);
    return reg_val;
}
EXPORT_SYMBOL(max8971_start_Factory_charging);

static int max8971_charger_get_property(struct power_supply *psy,
                                        enum power_supply_property psp,
                                        union power_supply_propval *val)
{
	struct max8971_chip *chip = container_of(psy, struct max8971_chip, charger);
	int ret = 0;
    	int chg_dtls_val;
	printk(" max8971_charger_get_property: psp= %d\n",psp);

	switch (psp) {
	case POWER_SUPPLY_PROP_ONLINE:
		val->intval = chip->chg_online;
		break;
    case POWER_SUPPLY_PROP_STATUS:
		ret = max8971_read_reg(chip->client, MAX8971_REG_DETAILS2);
        chg_dtls_val = (ret & MAX8971_CHG_DTLS_MASK);
        if (chip->chg_online) {
            if (chg_dtls_val == MAX8971_CHG_DTLS_DONE) {
                val->intval = POWER_SUPPLY_STATUS_DISCHARGING;
            }
            else if ((chg_dtls_val == MAX8971_CHG_DTLS_TIMER_FAULT) ||
                     (chg_dtls_val == MAX8971_CHG_DTLS_TEMP_SUSPEND) ||
                     (chg_dtls_val == MAX8971_CHG_DTLS_USB_SUSPEND) ||
                     (chg_dtls_val == MAX8971_CHG_DTLS_CHG_OFF)) {
                val->intval = POWER_SUPPLY_STATUS_NOT_CHARGING;
            }
            else {
                val->intval = POWER_SUPPLY_STATUS_CHARGING;
            }
        }
        else {
            val->intval = POWER_SUPPLY_STATUS_DISCHARGING;
        }
        ret = 0;
		break;	
    default:
		ret = -ENODEV;
		break;
	}
	return ret;
}

static enum power_supply_property max8971_charger_props[] = {
	POWER_SUPPLY_PROP_ONLINE,
    POWER_SUPPLY_PROP_STATUS,
};

static struct power_supply max8971_charger_ps = {
   .name = "max8971",
  //                                 
   .type = POWER_SUPPLY_TYPE_UNKNOWN ,
   .properties = max8971_charger_props,
   .num_properties = ARRAY_SIZE(max8971_charger_props),
   .get_property = max8971_charger_get_property,
};
/*
                              
                                                           
 
                           
 
                                                                    

                                        
                  
                                          
                                                                     
                                                                           

                                                                      
                                                                      
                                                                      

                                                                                 
                                                                             
                                                            
         
     
                                                              
 
      
*/

/*                                                                */
/*
                                                                                             
 
                                                                
                                                    
                                         
     
                                         
 
                                                    
                                   
                     
                  
 
                    
                           
                          
              
                           
                                   
                               
  
              
 
                                                                                     
*/
/*                                                                */

//                                                 
#if defined(CONFIG_MUIC)
static int muic_control_max8971_charger(struct muic_client_device *mcdev)
{
//                                            
	TYPE_MUIC_MODE muic_mode = muic_detect_cable();

	switch (muic_mode)
	{
		case MUIC_NA_TA :
		case MUIC_LG_TA :
		case MUIC_TA_1A :
			pr_info("%s: TA.\n", __func__);
#if defined(CONFIG_MACH_LGE_U2)
			charger_source=2;	
			max8971_start_charging(TA_CHARING_CURRENT);
#else
			max8971_start_charging(800);
#endif
			break;

		case MUIC_AP_USB :
			if(get_unlimited_temp() ==  UNLIMITED_TEMP_VAL){
				MST_test_mode = 1;
				pr_info("%s: AP_USB.\n", __func__);
				max8971_start_charging(USB_CHARING_CURRENT_MST);
			}
			else{
				MST_test_mode = 0;
				pr_info("%s: AP_USB.\n", __func__);
				max8971_start_charging(USB_CHARING_CURRENT);
			}
			break;

		case MUIC_CP_USB :
			pr_info("%s: CP_USB.\n", __func__);
			max8971_start_Factory_charging();
			break;

		case MUIC_CP_UART :
			pr_info("%s: CP_UART.\n", __func__);
			max8971_start_Factory_charging();
			break;

		case MUIC_MHL :
			pr_info("%s: MHL.\n", __func__);
			max8971_start_charging(400);
			break;

		case MUIC_NONE :
			MST_test_mode=0;
			charger_source=0;
			pr_info("%s: NONE.\n", __func__);
			max8971_stop_charging();
			break;

		default :
			pr_info("%s: No charging.\n", __func__);
			break;
	}

	return 0;
}

static struct muic_client_ops max8971_opt = {
	.notifier_priority = MUIC_CLIENT_NOTI_POWER_MHL,
	.on_none = muic_control_max8971_charger,
	.on_na_ta = muic_control_max8971_charger,
	.on_lg_ta = muic_control_max8971_charger,
	.on_1a_ta = muic_control_max8971_charger,
	.on_cp_uart = muic_control_max8971_charger,
	.on_ap_usb = muic_control_max8971_charger,
	.on_cp_usb = muic_control_max8971_charger,
	.on_mhl = muic_control_max8971_charger,
};
#endif //           
//                                                 

static int max8971_probe(struct i2c_client *client,
			const struct i2c_device_id *id)
{

	struct i2c_adapter *adapter = to_i2c_adapter(client->dev.parent);
	struct max8971_chip *chip;
    int ret;

	printk(" max8971_probe called \n");

	if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_BYTE))
		return -EIO;

	chip = kzalloc(sizeof(*chip), GFP_KERNEL);
	if (!chip)
		return -ENOMEM;
	//                 
	//                                                                   
	//                                                                   

	mt_set_gpio_mode(GPIO_I2C0_SCA_PIN, GPIO_I2C0_SCA_PIN_M_SCL);
	mt_set_gpio_mode(GPIO_I2C0_SDA_PIN, GPIO_I2C0_SDA_PIN_M_SDA);

	//                        
	mt_set_gpio_mode(GPIO_EINT_CHG_STAT_PIN, GPIO_EINT_CHG_STAT_PIN_M_EINT);
	
	printk("GPIO_SCL[88] Mode:%d\nGPIO_SDA[89] Mode:%d\n"
				,mt_get_gpio_mode(GPIO_I2C0_SCA_PIN),mt_get_gpio_mode(GPIO_I2C0_SDA_PIN));
    
	chip->client = client;
	chip->pdata = client->dev.platform_data;

	max8971_chg = chip;
//                     

	i2c_set_clientdata(client, chip);

//                                                              
       //          
	chip->charger = max8971_charger_ps;
	//          

	//          
	//                                                               
	ret = power_supply_register(&client->dev, &chip->charger);
	//          

	if (ret) {
		dev_err(&client->dev, "failed: power supply register\n");
		i2c_set_clientdata(client, NULL);
		goto out;
	}
//                                             
/*
                                                                          
                                                                 
                          
     
                                                             
                 
     
*/
        /*                                                                */
        //                         
       
	//                                                                
        
/*               
                                                                              
         
*/
    /*                                                                */


        ret=max8971_read_reg(chip->client, MAX8971_REG_CHGINT);
        printk(" MAX8971_REG_CHGINT : %d\n", ret);
	chip->chg_online = 0;
	ret = max8971_read_reg(chip->client, MAX8971_REG_CHG_STAT);

	if (ret >= 0) 
    	{
		chip->chg_online = (ret & MAX8971_DCUVP_MASK) ? 0 : 1;
        	if (chip->chg_online) 
        	{
        	    //                      
            	max8971_write_reg(chip->client, MAX8971_REG_CHGINT_MASK, chip->pdata->int_mask);
	          //                         
        	}
	}

//                                                 
#if defined(CONFIG_MUIC)
	ret = muic_client_dev_register(client->name, (void*)chip, &max8971_opt);
	if (ret < 0) {
		dev_err(&client->dev, "Unable to register max8971_opt\n");
		goto out;
	}
#endif
//                                                 

    chargin_hw_init_done = KAL_TRUE;

	return 0;
out:
	kfree(chip);
	return ret;
}

static int max8971_remove(struct i2c_client *client)
{
    struct max8971_chip *chip = i2c_get_clientdata(client);

	free_irq(client->irq, chip);
	power_supply_unregister(&max8971_charger_ps);
	kfree(chip);

	return 0;
}

static const struct i2c_device_id max8971_i2c_id[] = {
	{ MAX8971_DEV_I2C_NAME, 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, max8971_id);

static struct i2c_driver max8971_i2c_driver = {
	.driver = {
		.name = MAX8971_DEV_I2C_NAME,
	},
	.probe		= max8971_probe,
	.remove		= (max8971_remove),
	.id_table	= max8971_i2c_id,
};

static int __init max8971_init(void)
{
	int ret;
	printk("max8971_init called \n");
	ret = i2c_register_board_info(2, &max8971_i2c_data, 1);
	if(ret)
    {
        printk("failed to i2c register driver. (%d) \n",ret);
        return ret;
    } else {
        printk("success to i2c register driver. (%d) \n",ret);
    }
	return i2c_add_driver(&max8971_i2c_driver);
}
module_init(max8971_init);

static void __exit max8971_exit(void)
{
	i2c_del_driver(&max8971_i2c_driver);
}
module_exit(max8971_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Clark Kim <clark.kim@maxim-ic.com>");
MODULE_DESCRIPTION("Power supply driver for MAX8971");
MODULE_VERSION("3.3");
MODULE_ALIAS("platform:max8971-charger");

