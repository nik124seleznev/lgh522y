/*
 * include/linux/extcon/extcon-adc-jack.h
 *
 * Analog Jack extcon driver with ADC-based detection capability.
 *
 * Copyright (C) 2012 Samsung Electronics
 * MyungJoo Ham <myungjoo.ham@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#ifndef _EXTCON_ADC_JACK_H_
#define _EXTCON_ADC_JACK_H_ __FILE__

#include <linux/module.h>
#include <linux/extcon.h>

/* 
                                                          
                                                                          
                                                              
                                                 
                                                 
  
                                                                            
                                                                             
                                      
  
                                                                         
                                                                           
 */
struct adc_jack_cond {
	u32 state; /*                                  */
	u32 min_adc;
	u32 max_adc;
};

/* 
                                                             
                                                                        
                                                                          
                                                                            
                                                         
                                                               
                                                                        
                                                                          
                                                    
                                            
                                                                        
                                                                        
                                                                          
                                                 
 */
struct adc_jack_pdata {
	const char *name;
	const char *consumer_channel;
	/*
                                 
  */
	const char **cable_names;
	/*                                    */
	struct adc_jack_cond *adc_conditions;

	unsigned long irq_flags;
	unsigned long handling_delay_ms; /*       */
};

#endif /*                    */
