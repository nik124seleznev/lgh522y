/* include/linux/ist8301.h - IST8303 compass driver
 *
 * Copyright (C) 2009 Technology Inc.
 * Author: Minghung.Chou
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
#ifndef IST8303_H
#define IST8303_H

#include <linux/ioctl.h>
//                                   

//                                                                              

/*                                                                             */
#define IST8303_REG_WIA				0x00  //          
#define IST8303_REG_MINFO			0x01  //           
#define IST8303_REG_STAT1			0x02  //                   
#define IST8303_REG_DATAX_L			0x03  //                         
#define IST8303_REG_DATAX_H		        0x04  //                          
#define IST8303_REG_DATAY_L			0x05  //                         
#define IST8303_REG_DATAY_H		        0x06  //                          
#define IST8303_REG_DATAZ_L			0x07  //                         
#define IST8303_REG_DATAZ_H		        0x08  //                          
#define IST8303_REG_STAT2			0x09  //                   
#define IST8303_REG_CNTRL1			0x0A  //                            
#define IST8303_REG_CNTRL2			0x0B  //                            
#define IST8303_REG_STCR			0x0C  //                   
#define IST8303_REG_CNTRL3			0x0D  //                            
#define IST8303_REG_TEMP_L			0x1C  //                              
#define IST8303_REG_TEMP_H		        0x1D  //                               
#define IST8303_REG_SSR                         0x42  //                           
#define IST8303_REG_CTR                         0x40  //                    
#define IST8303_REG_BTR                         0x57  //                        

#if 0
/*                                                               */
#define IST8303_CTRL1_PC1			0x80
#define IST8303_CTRL1_FS1_NORMAL		0x00 //      
#define IST8303_CTRL1_FS1_FORCE			0x02 //     
#define IST8303_CTRL1_ODR1			0x10 //           
#define IST8303_CTRL2_DREN			0x08
#define IST8303_CTRL2_DRP			0x04
#define IST8303_CTRL3_NOFORCE_BIT		0x00
#define IST8303_CTRL3_FORCE_BIT			0x40
#define IST8303_CTRL3_B0_LO_CLR			0x00
#define IST8303_CTRL3_RESET_BIT			0x80

#define IST8303_INS_DRDY_BIT            0x40
#endif


#define IST8303_BUFSIZE				256
#define IST8303_NORMAL_MODE			0
#define IST8303_FORCE_MODE			1
#define IST8303_IRQ				IRQ_EINT9

//                                        
#define CONVERT_M                       33
#define ORIENTATION_ACCURACY_RATE       1


#endif
