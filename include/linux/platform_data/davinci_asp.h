/*
 * TI DaVinci Audio Serial Port support
 *
 * Copyright (C) 2012 Texas Instruments Incorporated - http://www.ti.com/
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#ifndef __DAVINCI_ASP_H
#define __DAVINCI_ASP_H

#include <linux/genalloc.h>

struct snd_platform_data {
	u32 tx_dma_offset;
	u32 rx_dma_offset;
	int asp_chan_q;	/*                                    */
	int ram_chan_q;	/*                                    */
	/*
                                                                       
                                                                  
                                       
  */
	unsigned enable_channel_combine:1;
	unsigned sram_size_playback;
	unsigned sram_size_capture;
	struct gen_pool *sram_pool;

	/*
                                                            
                                                            
                   
                                                              
                                                        
           
  */
	int clk_input_pin;

	/*
                                                                  
                                                                
                       
                                      
                                                                 
                      
   
                           
                                       
                          
   
                         
                                           
                                                     
                                                 
                                                   
                                                 
                                                   
                                                  
   
                            
                                  
                                                     
                                                     
                                                      
                                                    
                                                     
                                   
   
  */
	bool i2s_accurate_sck;

	/*                       */
	int tdm_slots;
	u8 op_mode;
	u8 num_serializer;
	u8 *serial_dir;
	u8 version;
	u8 txnumevt;
	u8 rxnumevt;
};

enum {
	MCASP_VERSION_1 = 0,	/*        */
	MCASP_VERSION_2,	/*               */
	MCASP_VERSION_3,        /*               */
};

enum mcbsp_clk_input_pin {
	MCBSP_CLKR = 0,		/*             */
	MCBSP_CLKS,
};

#define INACTIVE_MODE	0
#define TX_MODE		1
#define RX_MODE		2

#define DAVINCI_MCASP_IIS_MODE	0
#define DAVINCI_MCASP_DIT_MODE	1

#endif
