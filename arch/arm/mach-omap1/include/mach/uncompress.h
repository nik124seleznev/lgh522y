/*
 * arch/arm/plat-omap/include/mach/uncompress.h
 *
 * Serial port stubs for kernel decompress status messages
 *
 * Initially based on:
 * linux-2.4.15-rmk1-dsplinux1.6/arch/arm/plat-omap/include/mach1510/uncompress.h
 * Copyright (C) 2000 RidgeRun, Inc.
 * Author: Greg Lonnon <glonnon@ridgerun.com>
 *
 * Rewritten by:
 * Author: <source@mvista.com>
 * 2004 (c) MontaVista Software, Inc.
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2. This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#include <linux/types.h>
#include <linux/serial_reg.h>

#include <asm/memory.h>
#include <asm/mach-types.h>

#include "serial.h"

#define MDR1_MODE_MASK			0x07

volatile u8 *uart_base;
int uart_shift;

/*
                                              
                                                         
 */
static void set_omap_uart_info(unsigned char port)
{
	/*
                                                      
                              
  */
	u32 ram_start = (u32)&uart_shift & 0xf8000000;
	u32 *uart_info = (u32 *)(ram_start + OMAP_UART_INFO_OFS);
	*uart_info = port;
}

static void putc(int c)
{
	if (!uart_base)
		return;

	/*                         */
	if ((uart_base[UART_OMAP_MDR1 << uart_shift] & MDR1_MODE_MASK) != 0)
		return;

	while (!(uart_base[UART_LSR << uart_shift] & UART_LSR_THRE))
		barrier();
	uart_base[UART_TX << uart_shift] = c;
}

static inline void flush(void)
{
}

/*
                                           
 */
#define _DEBUG_LL_ENTRY(mach, dbg_uart, dbg_shft, dbg_id)		\
	if (machine_is_##mach()) {					\
		uart_base = (volatile u8 *)(dbg_uart);			\
		uart_shift = (dbg_shft);				\
		port = (dbg_id);					\
		set_omap_uart_info(port);				\
		break;							\
	}

#define DEBUG_LL_OMAP7XX(p, mach)					\
	_DEBUG_LL_ENTRY(mach, OMAP1_UART##p##_BASE, OMAP7XX_PORT_SHIFT,	\
		OMAP1UART##p)

#define DEBUG_LL_OMAP1(p, mach)						\
	_DEBUG_LL_ENTRY(mach, OMAP1_UART##p##_BASE, OMAP_PORT_SHIFT,	\
		OMAP1UART##p)

static inline void arch_decomp_setup(void)
{
	int port = 0;

	/*
                                                                    
                                                                 
                                                                 
                     
  */
	do {
		/*                                                   */
		DEBUG_LL_OMAP7XX(1, herald);
		DEBUG_LL_OMAP7XX(1, omap_perseus2);

		/*                                        */
		DEBUG_LL_OMAP1(1, ams_delta);
		DEBUG_LL_OMAP1(1, nokia770);
		DEBUG_LL_OMAP1(1, omap_h2);
		DEBUG_LL_OMAP1(1, omap_h3);
		DEBUG_LL_OMAP1(1, omap_innovator);
		DEBUG_LL_OMAP1(1, omap_osk);
		DEBUG_LL_OMAP1(1, omap_palmte);
		DEBUG_LL_OMAP1(1, omap_palmz71);

		/*                                        */
		DEBUG_LL_OMAP1(2, omap_palmtt);

		/*                                        */
		DEBUG_LL_OMAP1(3, sx1);
	} while (0);
}
