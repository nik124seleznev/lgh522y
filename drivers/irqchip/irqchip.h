/*
 * Copyright (C) 2012 Thomas Petazzoni
 *
 * Thomas Petazzoni <thomas.petazzoni@free-electrons.com>
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2.  This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#ifndef _IRQCHIP_H
#define _IRQCHIP_H

/*
                                                                      
                                                               
                           
  
                                                                     
             
                                                    
                               
 */
#define IRQCHIP_DECLARE(name,compstr,fn)				\
	static const struct of_device_id irqchip_of_match_##name	\
	__used __section(__irqchip_of_table)				\
	= { .compatible = compstr, .data = fn }

#endif
