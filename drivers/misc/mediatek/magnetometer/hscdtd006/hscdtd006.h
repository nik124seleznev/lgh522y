/* include/linux/ami304.h - HSCDTD006 compass driver
 *
 * Copyright (C) 2009 AMIT Technology Inc.
 * Author: Kyle Chen <sw-support@amit-inc.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

/*
                                       
 */
#ifndef HSCDTD006_H
#define HSCDTD006_H

#include <linux/ioctl.h>
//                                   

#define HSCDTD006_I2C_ADDRESS 			0x0C  

/*                                                                                 */

#define HSCDTD006_REG_STBA        0x0B
#define HSCDTD006_REG_STBB        0x0C
#define HSCDTD006_REG_XOUT        0x10
#define HSCDTD006_REG_YOUT        0x12
#define HSCDTD006_REG_ZOUT        0x14
#define HSCDTD006_REG_DATAXH	  0x11
#define HSCDTD006_REG_DATAXL	  0x10
#define HSCDTD006_REG_DATAYH	  0x13
#define HSCDTD006_REG_DATAYL	  0x12
#define HSCDTD006_REG_DATAZH	  0x15
#define HSCDTD006_REG_DATAZL	  0x14

#define HSCDTD006_REG_STATUS      0x18

#define HSCDTD006_REG_CTRL1		  0x1B
#define HSCDTD006_REG_CTRL2		  0x1C
#define HSCDTD006_REG_CTRL3		  0x1D


/*                                                                   */
#define HSCDTD006_CTRL1_PC1			0x80   //                         
#define HSCDTD006_CTRL1_FS1_NORMAL			0x00 //      
#define HSCDTD006_CTRL1_FS1_FORCE			0x02 //     
//                                                                           
#define HSCDTD006_CTRL1_RSX_ODR1			0x0C //                                  
#define HSCDTD006_CTRL2_DREN			0x08
#define HSCDTD006_CTRL2_DRP			0x04
#define HSCDTD006_CTRL3_NOFORCE_BIT		0x00
#define HSCDTD006_CTRL3_FORCE_BIT			0x40
#define HSCDTD006_CTRL3_B0_LO_CLR			0x00
#define HSCDTD006_CTRL3_SRST			0x80  //        

#define HSCDTD006_BUFSIZE				256
#define HSCDTD006_NORMAL_MODE			0
#define HSCDTD006_FORCE_MODE			1
#define HSCDTD006_IRQ				IRQ_EINT9

//                                        
#define CONVERT_M                       1//  
#define ORIENTATION_ACCURACY_RATE                   1


enum {
    ACTIVE_SS_NUL = 0x00 ,
    ACTIVE_SS_ACC = 0x01 ,
    ACTIVE_SS_MAG = 0x02 ,
    ACTIVE_SS_ORI = 0x04 ,
};

struct TAIFD_HW_DATA {
    int activate;
    int delay;
    int acc[4];
    int mag[4];
};

struct TAIFD_SW_DATA {
    int acc[5];
    int mag[5];
    int ori[5];
};



#endif
