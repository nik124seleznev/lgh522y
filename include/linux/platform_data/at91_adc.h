/*
 * Copyright (C) 2011 Free Electrons
 *
 * Licensed under the GPLv2 or later.
 */

#ifndef _AT91_ADC_H_
#define _AT91_ADC_H_

/* 
                                                                        
                                                            
                                                                
                                   
                                                            
                                                          
 */
struct at91_adc_reg_desc {
	u8	channel_base;
	u32	drdy_mask;
	u8	status_register;
	u8	trigger_register;
};

/* 
                                                    
                                                     
                                                            
                        
                                                          
 */
struct at91_adc_trigger {
	const char	*name;
	u8		value;
	bool		is_external;
};

/* 
                                                      
                                                             
                                                                   
                                                  
                                                          
                                                
                                                            
                                                                          
                                                       
 */
struct at91_adc_data {
	unsigned long			channels_used;
	u8				num_channels;
	struct at91_adc_reg_desc	*registers;
	u8				startup_time;
	struct at91_adc_trigger		*trigger_list;
	u8				trigger_number;
	bool				use_external_triggers;
	u16				vref;
};

extern void __init at91_add_device_adc(struct at91_adc_data *data);
#endif
