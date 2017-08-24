/*
 * Copyright 2004-2007 Freescale Semiconductor, Inc. All Rights Reserved.
 * Copyright 2008 Juergen Beisert, kernel@pengutronix.de
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA  02110-1301, USA.
 */

#ifndef __ASM_ARCH_MXC_HARDWARE_H__
#define __ASM_ARCH_MXC_HARDWARE_H__

#include <asm/sizes.h>

#define addr_in_module(addr, mod) \
	((unsigned long)(addr) - mod ## _BASE_ADDR < mod ## _SIZE)

#define IMX_IO_P2V_MODULE(addr, module)					\
	(((addr) - module ## _BASE_ADDR) < module ## _SIZE ?		\
	 (addr) - (module ## _BASE_ADDR) + (module ## _BASE_ADDR_VIRT) : 0)

/*
                                                                              
                                                                                
                              
  
                                       
                              
                                                       
  
                                                                       
  
                                                                             
                                                                               
                                                                    
  
                                                            
  
       
                                                
        
                                                  
                                                   
                                                    
        
                                                   
                                                   
                                                  
        
                                                  
                                                   
                                                    
        
                                                   
                                                   
                                                  
                                                    
                                                   
        
                                                   
                                                   
                                                  
                                                    
                                                   
        
                                                  
                                                  
                                                   
                                                   
                                                   
                                                   
        
                                                  
                                                   
                                                   
                                                   
                                                   
        
                                                 
                                                 
                                                    
                                                   
 */
#define IMX_IO_P2V(x)	(						\
			(((x) & 0x80000000) >> 7) |			\
			(0xf4000000 +					\
			(((x) & 0x50000000) >> 6) +			\
			(((x) & 0x0b000000) >> 4) +			\
			(((x) & 0x000fffff))))

#define IMX_IO_ADDRESS(x)	IOMEM(IMX_IO_P2V(x))

#include "mxc.h"

#include "mx51.h"
#include "mx53.h"
#include "mx3x.h"
#include "mx31.h"
#include "mx35.h"
#include "mx2x.h"
#include "mx21.h"
#include "mx27.h"
#include "mx1.h"
#include "mx25.h"

#define imx_map_entry(soc, name, _type)	{				\
	.virtual = soc ## _IO_P2V(soc ## _ ## name ## _BASE_ADDR),	\
	.pfn = __phys_to_pfn(soc ## _ ## name ## _BASE_ADDR),		\
	.length = soc ## _ ## name ## _SIZE,				\
	.type = _type,							\
}

/*                                                                    */
/*                                           */
#define IMX_GPIO_NR(bank, nr)		(((bank) - 1) * 32 + (nr))

#endif /*                             */
