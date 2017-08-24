/* bmm050.c - bmm050 compass driver
 *
 *
 * This software program is licensed subject to the GNU General Public License
 * (GPL).Version 2,June 1991, available at http://www.fsf.org/copyleft/gpl.html

 * (C) Copyright 2011 Bosch Sensortec GmbH
 * All Rights Reserved
 */

/*
                                               
 */
#ifndef __BMM050_H__
#define __BMM050_H__

#include <linux/ioctl.h>

#define CALIBRATION_DATA_SIZE	12

/*            
                                                      
                                                        
*/
//                             
#define BMM050_I2C_ADDR		0x13

//                                                     
//                                        
//                        
//                        
//                  
//                        


#define CONVERT_M			1
#define CONVERT_M_DIV		4
#define CONVERT_O			1
#define CONVERT_O_DIV		71		//                                          
#define CONVERT_G			1
#define CONVERT_G_DIV		938
#define CONVERT_VRV			1
#define CONVERT_VRV_DIV	16384
#define CONVERT_FLIP		1

#endif /*              */

