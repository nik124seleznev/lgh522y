/*
 * Copyright (C) 2012 Synopsys, Inc. (www.synopsys.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef _ASM_ARC_SERIAL_H
#define _ASM_ARC_SERIAL_H

/*
                                                                        
                             
                                                         
                                                                        
                                     
                                                                 
 */

#include <asm/clk.h>

#define BASE_BAUD	(arc_get_core_freq() / 16)

/*
                                                                          
                                                                          
                     
 */
#ifdef CONFIG_ARC_PLAT_TB10X
#undef BASE_BAUD
#define BASE_BAUD	(arc_get_core_freq() / 16 / 3)
#endif

#endif /*                   */
