/*
 * HID Sensors Driver
 * Copyright (c) 2012, Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */
#ifndef _HID_SENSORS_HUB_H
#define _HID_SENSORS_HUB_H

#include <linux/hid.h>
#include <linux/hid-sensor-ids.h>

/* 
                                                        
                                                    
                                                
                                                            
                                      
                                               
                                          
                                       
 */
struct hid_sensor_hub_attribute_info {
	u32 usage_id;
	u32 attrib_id;
	s32 report_id;
	s32 index;
	s32 units;
	s32 unit_expo;
	s32 size;
};

/* 
                                                              
                                   
                                        
                                          
 */
struct hid_sensor_hub_device {
	struct hid_device *hdev;
	u32 vendor_id;
	u32 product_id;
};

/* 
                                                              
                                                         
                               
                             
                                             
                                                              
                                     
 */
struct hid_sensor_hub_callbacks {
	struct platform_device *pdev;
	int (*suspend)(struct hid_sensor_hub_device *hsdev, void *priv);
	int (*resume)(struct hid_sensor_hub_device *hsdev, void *priv);
	int (*capture_sample)(struct hid_sensor_hub_device *hsdev,
			u32 usage_id, size_t raw_len, char *raw_data,
			void *priv);
	int (*send_event)(struct hid_sensor_hub_device *hsdev, u32 usage_id,
			 void *priv);
};

/*                        */

/* 
                                                            
                              
                                                             
                                            
 
                                                                 
                                                                 
                                    
*/
int sensor_hub_register_callback(struct hid_sensor_hub_device *hsdev,
			u32 usage_id,
			struct hid_sensor_hub_callbacks *usage_callback);

/* 
                                                        
                              
                                                             
 
                                                              
                                                               
*/
int sensor_hub_remove_callback(struct hid_sensor_hub_device *hsdev,
			u32 usage_id);


/*                                */

/* 
                                                                      
                              
                                                     
                                                                     
                                                
                                                                
 
                                                                        
                                     
*/
int sensor_hub_input_get_attribute_info(struct hid_sensor_hub_device *hsdev,
			u8 type,
			u32 usage_id, u32 attr_usage_id,
			struct hid_sensor_hub_attribute_info *info);

/* 
                                                                  
                                                                     
                                                
                                   
 
                                                                   
                                                                     
                                                           
*/

int sensor_hub_input_attr_get_raw_value(struct hid_sensor_hub_device *hsdev,
			u32 usage_id,
			u32 attr_usage_id, u32 report_id);
/* 
                                                
                                   
                                           
                      
 
                                                                         
                                                   
*/
int sensor_hub_set_feature(struct hid_sensor_hub_device *hsdev, u32 report_id,
			u32 field_index, s32 value);

/* 
                                                
                                   
                                           
                                       
 
                                                                         
                                                   
*/
int sensor_hub_get_feature(struct hid_sensor_hub_device *hsdev, u32 report_id,
			u32 field_index, s32 *value);

/*                       */

/*                                 */
struct hid_sensor_common {
	struct hid_sensor_hub_device *hsdev;
	struct platform_device *pdev;
	unsigned usage_id;
	bool data_ready;
	struct hid_sensor_hub_attribute_info poll;
	struct hid_sensor_hub_attribute_info report_state;
	struct hid_sensor_hub_attribute_info power_state;
	struct hid_sensor_hub_attribute_info sensitivity;
};

/*                                              */
static inline int hid_sensor_convert_exponent(int unit_expo)
{
	if (unit_expo < 0x08)
		return unit_expo;
	else if (unit_expo <= 0x0f)
		return -(0x0f-unit_expo+1);
	else
		return 0;
}

int hid_sensor_parse_common_attributes(struct hid_sensor_hub_device *hsdev,
					u32 usage_id,
					struct hid_sensor_common *st);
int hid_sensor_write_raw_hyst_value(struct hid_sensor_common *st,
					int val1, int val2);
int hid_sensor_read_raw_hyst_value(struct hid_sensor_common *st,
					int *val1, int *val2);
int hid_sensor_write_samp_freq_value(struct hid_sensor_common *st,
					int val1, int val2);
int hid_sensor_read_samp_freq_value(struct hid_sensor_common *st,
					int *val1, int *val2);

#endif
