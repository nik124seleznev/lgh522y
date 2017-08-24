/* mpu3000.h
 *
 * (C) Copyright 2008 
 * MediaTek <www.mediatek.com>
 *
 * mpu300 head file for MT65xx
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef MPU3000_H
#define MPU3000_H
	 
#include <linux/ioctl.h>
	 
#define MPU3000_I2C_SLAVE_ADDR		0xD0
#define MPU3000_FIXED_DEVID			0xD0


/*                                                                */
#define MPU3000_REG_DEVID			         0x00
#define MPU3000_REG_OFS_XH			0x0C
#define MPU3000_REG_OFS_XL		         0x0D
#define MPU3000_REG_OFS_YH			0x0E
#define MPU3000_REG_OFS_YL			0x0F
#define MPU3000_REG_OFS_ZH			0x10
#define MPU3000_REG_OFS_ZL			0x11
#define MPU3000_REG_FIFO_EN			0x12
#define MPU3000_REG_AUX_VDD			0x13
#define MPU3000_REG_AUX_ID		         0x14
#define MPU3000_REG_SAMRT_DIV		0x15
#define MPU3000_REG_DATA_FMT			0x16   //                                                                              
#define MPU3000_REG_INT_EN			0x17
#define MPU3000_REG_AUX_ADDR   		0x18
#define MPU3000_REG_INT_STA			0x1A

#define MPU3000_REG_TEMP_OUTH		0x1B
#define MPU3000_REG_TEMP_OUTL	         0x1C
#define MPU3000_REG_GYRO_XH			0x1D
#define MPU3000_REG_GYRO_XL			0x1E
#define MPU3000_REG_GYRO_YH			0x1F
#define MPU3000_REG_GYRO_YL			0x20
#define MPU3000_REG_GYRO_ZH			0x21
#define MPU3000_REG_GYRO_ZL			0x22
#define MPU3000_REG_AUX_XH			0x23
#define MPU3000_REG_AUX_XL			0x24
#define MPU3000_REG_AUX_YH			0x25
#define MPU3000_REG_AUX_YL			0x26
#define MPU3000_REG_AUX_ZH			0x27
#define MPU3000_REG_AUX_ZL			0x28
#define MPU3000_REG_FIFO_CNTH		0x3A
#define MPU3000_REG_FIFO_CNTL		0x3B
#define MPU3000_REG_FIFO_DATA		0x3C
#define MPU3000_REG_FIFO_CTL			0x3D
#define MPU3000_REG_PWR_CTL			0x3E

	 

/*                                */ 
#define MPU3000_I2C_DIS					0x80	//                       
#define MPU3000_DEVID_MASK			0x7E	//                              

#define MPU3000_FIFO_TEMP_EN			0x80	//                                      
#define MPU3000_FIFO_GYROX_EN		0x40	//                                
#define MPU3000_FIFO_GYROY_EN		0x20	//                                
#define MPU3000_FIFO_GYROZ_EN		0x10	//                                
#define MPU3000_FIFO_AUXX_EN			0x08	//                                         
#define MPU3000_FIFO_AUXY_EN			0x04	//                                         
#define MPU3000_FIFO_AUXZ_EN			0x02	//                                         
#define MPU3000_FIFO_FOOTER_EN		0x01	//                              

#define MPU3000_AUX_VDDIO_EN			0x04	//                                                               
#define MPU3000_AUX_VDDIO_DIS			0x00	//                                                                
#define MPU3000_AUX_ID_MASK			0x7F	//           
#define MPU3000_AUX_CLKOUT_EN		0x80	//                                           

#define MPU3000_EXT_SYNC				0x05	//                                                                                 
#define MPU3000_SYNC_NONE 			0x00
#define MPU3000_SYNC_TEMP			0x01
#define MPU3000_SYNC_GYROX			0x02
#define MPU3000_SYNC_GYROY			0x03
#define MPU3000_SYNC_GYROZ			0x04
#define MPU3000_SYNC_AUXX			0x05
#define MPU3000_SYNC_AUXY			0x06
#define MPU3000_SYNC_AUXZ			0x07

#define MPU3000_FS_RANGE			0x03	//                                            
#define MPU3000_FS_250 				0x00
#define MPU3000_FS_500				0x01
#define MPU3000_FS_1000				0x02
#define MPU3000_FS_2000				0x03
#define MPU3000_FS_MAX				0x03

#define MPU3000_FS_250_LSB			131		//          
#define MPU3000_FS_500_LSB			66
#define MPU3000_FS_1000_LSB			33
#define MPU3000_FS_2000_LSB			16
#define MPU3000_FS_MAX_LSB			131


#define MPU3000_SAM_RATE_MASK		0x07	//                                                 
#define MPU3000_RATE_8K_LPFB_256HZ 	0x00
#define MPU3000_RATE_1K_LPFB_188HZ	0x01
#define MPU3000_RATE_1K_LPFB_98HZ 	0x02
#define MPU3000_RATE_1K_LPFB_42HZ 	0x03
#define MPU3000_RATE_1K_LPFB_20HZ 	0x04
#define MPU3000_RATE_1K_LPFB_10HZ 	0x05
#define MPU3000_RATE_1K_LPFB_5HZ 	0x06


#define MPU3000_INT_ACTL				0x80	//                                      
#define MPU3000_INT_OPEN				0x40	//                                       
#define MPU3000_INT_LATCH_EN			0x20	//                                 
#define MPU3000_INT_CLR_BYRD			0x10	//                                 
#define MPU3000_MPU_RDY_EN			0x04	//                               
#define MPU3000_DMP_DONE_EN			0x02	//                           
#define MPU3000_RAW_RDY_EN			0x01	//                                 

#define MPU3000_INTS_MPU_RDY			0x04	//            
#define MPU3000_INTS_DMP_DONE		0x02	//                                     
#define MPU3000_INTS_RAW_RDY			0x01	//                              

#define MPU3000_FIFO_CNT_H			0x03	//                              
#define MPU3000_FIFO_CNT_L				0xFF	//                            
#define MPU3000_FIFOSIZE				512

#define MPU3000_FIFO_EN				0x40	//                                     
#define MPU3000_AUX_IF_EN				0x20	//                            
#define MPU3000_AUX_IF_RST			0x08	//                            
#define MPU3000_FIFO_RST				0x02	//                   
#define MPU3000_GYRO_RST				0x01	//                                       

#define MPU3000_H_SET					0x80	//                                                       
#define MPU3000_SLEEP					0x40	//                           
#define MPU3000_STBY_XG				0x20	//                          
#define MPU3000_STBY_YG				0x10	//                          
#define MPU3000_STBY_ZG				0x08	//                          

#define MPU3000_CLK_SEL_MASK			0x07	//                          
#define MPU3000_CLK_INTER_OSC		0x00
#define MPU3000_CLK_PLL_GYROX		0x01
#define MPU3000_CLK_PLL_GYROY		0x02
#define MPU3000_CLK_PLL_GYROZ		0x03
#define MPU3000_CLK_PLL_32K			0x04
#define MPU3000_CLK_PLL_19M			0x05
#define MPU3000_CLK_PLL_RESV			0x06
#define MPU3000_CLK_STOP_MASK		0x07

	 
#define MPU3000_SUCCESS				    0
#define MPU3000_ERR_I2C				  -1
#define MPU3000_ERR_STATUS			  -3
#define MPU3000_ERR_SETUP_FAILURE	  -4
#define MPU3000_ERR_GETGSENSORDATA  -5
#define MPU3000_ERR_IDENTIFICATION	  -6
	 

/*
               
                            
                      
                       
                       
                       
                      
                      
                      
                       

               
                         
                   
                    
                    
                         
                 

               
                                     
                               
                              
                              
                              
                              
                             
                      
*/

#define MPU3000_BUFSIZE 60

//                                      
//                  
#define DEGREE_TO_RAD	7506
	 
#endif //         

