/*
 * Copyright (C) 2009 Texas Instruments
 * Added OMAP4 support- Santosh Shilimkar <santosh.shilimkar@ti.com>
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#ifndef __ASM_ARCH_SERIAL_H
#define __ASM_ARCH_SERIAL_H

#include <linux/init.h>

/*
                                                                     
                                                         
  
                                                                       
                                
  
                                                                 
                                                           
                                                                      
                                                      
 */
#define OMAP_UART_INFO_OFS	0x3ffc

/*                    */
#define OMAP1_UART1_BASE	0xfffb0000
#define OMAP1_UART2_BASE	0xfffb0800
#define OMAP1_UART3_BASE	0xfffb9800

#define OMAP_PORT_SHIFT		2
#define OMAP7XX_PORT_SHIFT	0

#define OMAP1510_BASE_BAUD	(12000000/16)
#define OMAP16XX_BASE_BAUD	(48000000/16)

/*
                                                                      
                               
 */
#define OMAP1UART1		11
#define OMAP1UART2		12
#define OMAP1UART3		13

#ifndef __ASSEMBLER__
extern void omap_serial_init(void);
#endif

#endif
