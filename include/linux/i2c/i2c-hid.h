/*
 * HID over I2C protocol implementation
 *
 * Copyright (c) 2012 Benjamin Tissoires <benjamin.tissoires@gmail.com>
 * Copyright (c) 2012 Ecole Nationale de l'Aviation Civile, France
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file COPYING in the main directory of this archive for
 * more details.
 */

#ifndef __LINUX_I2C_HID_H
#define __LINUX_I2C_HID_H

#include <linux/types.h>

/* 
                                                                     
                                                                            
  
                                                                             
                                                                             
                                                                          
  
                                      
                                
                                                                      
                                       
                                                 
 */
struct i2c_hid_platform_data {
	u16 hid_descriptor_address;
};

#endif /*                   */
