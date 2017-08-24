/*
 * OMAP4xxx bandgap registers, bitfields and temperature definitions
 *
 * Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
 * Contact:
 *   Eduardo Valentin <eduardo.valentin@ti.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */
#ifndef __OMAP4XXX_BANDGAP_H
#define __OMAP4XXX_BANDGAP_H

/* 
                   
  
                                                    
                                                              
 */

/* 
                                
  
                                                    
                                     
 */

/*                        */
#define OMAP4430_FUSE_OPP_BGAP				0x0

/*                       */
#define OMAP4430_TEMP_SENSOR_CTRL_OFFSET		0xCC

/* 
                                            
  
                                                     
                                                       
                       
 */

/*                           */
#define OMAP4430_BGAP_TEMPSOFF_MASK			BIT(12)
#define OMAP4430_BGAP_TSHUT_MASK			BIT(11)
#define OMAP4430_SINGLE_MODE_MASK			BIT(10)
#define OMAP4430_BGAP_TEMP_SENSOR_SOC_MASK		BIT(9)
#define OMAP4430_BGAP_TEMP_SENSOR_EOCZ_MASK		BIT(8)
#define OMAP4430_BGAP_TEMP_SENSOR_DTEMP_MASK		(0xff << 0)

/* 
                                                 
  
                                                         
                                                           
                               
 */

/*                             */
#define OMAP4430_ADC_START_VALUE			0
#define OMAP4430_ADC_END_VALUE				127
/*                                           */
#define OMAP4430_MAX_FREQ				32768
#define OMAP4430_MIN_FREQ				32768
/*               */
#define OMAP4430_MIN_TEMP				-40000
#define OMAP4430_MAX_TEMP				125000
#define OMAP4430_HYST_VAL				5000

/* 
                                           
  
                                                    
                                                              
 */

/* 
                                
  
                                                    
                                     
 */

/*                        */
#define OMAP4460_FUSE_OPP_BGAP				0x0

/*                      */
#define OMAP4460_TEMP_SENSOR_CTRL_OFFSET		0xCC

/*                       */
#define OMAP4460_BGAP_CTRL_OFFSET			0x118

/*                          */
#define OMAP4460_BGAP_COUNTER_OFFSET			0x11C

/*                            */
#define OMAP4460_BGAP_THRESHOLD_OFFSET			0x120

/*                          */
#define OMAP4460_BGAP_TSHUT_OFFSET			0x124

/*                         */
#define OMAP4460_BGAP_STATUS_OFFSET			0x128

/* 
                                  
  
                                                     
                                                       
                       
 */
/*                           */
#define OMAP4460_BGAP_TEMPSOFF_MASK			BIT(13)
#define OMAP4460_BGAP_TEMP_SENSOR_SOC_MASK		BIT(11)
#define OMAP4460_BGAP_TEMP_SENSOR_EOCZ_MASK		BIT(10)
#define OMAP4460_BGAP_TEMP_SENSOR_DTEMP_MASK		(0x3ff << 0)

/*                            */
#define OMAP4460_SINGLE_MODE_MASK			BIT(31)
#define OMAP4460_MASK_HOT_MASK				BIT(1)
#define OMAP4460_MASK_COLD_MASK				BIT(0)

/*                               */
#define OMAP4460_COUNTER_MASK				(0xffffff << 0)

/*                                 */
#define OMAP4460_T_HOT_MASK				(0x3ff << 16)
#define OMAP4460_T_COLD_MASK				(0x3ff << 0)

/*                               */
#define OMAP4460_TSHUT_HOT_MASK				(0x3ff << 16)
#define OMAP4460_TSHUT_COLD_MASK			(0x3ff << 0)

/*                              */
#define OMAP4460_CLEAN_STOP_MASK			BIT(3)
#define OMAP4460_BGAP_ALERT_MASK			BIT(2)
#define OMAP4460_HOT_FLAG_MASK				BIT(1)
#define OMAP4460_COLD_FLAG_MASK				BIT(0)

/* 
                                                 
  
                                                         
                                                           
                               
 */

/*                             */
#define OMAP4460_ADC_START_VALUE			530
#define OMAP4460_ADC_END_VALUE				932
/*                      */
#define OMAP4460_MAX_FREQ				1500000
#define OMAP4460_MIN_FREQ				1000000
/*               */
#define OMAP4460_MIN_TEMP				-40000
#define OMAP4460_MAX_TEMP				123000
#define OMAP4460_HYST_VAL				5000
/*                       */
#define OMAP4460_TSHUT_HOT				900	/*           */
#define OMAP4460_TSHUT_COLD				895	/*           */
#define OMAP4460_T_HOT					800	/*          */
#define OMAP4460_T_COLD					795	/*          */

#endif /*                      */
