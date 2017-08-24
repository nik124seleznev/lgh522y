/*
 * linux/arch/arm/mach-mmp/include/mach/regs-apbc.h
 *
 *   Application Peripheral Bus Clock Unit
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __ASM_MACH_REGS_APBC_H
#define __ASM_MACH_REGS_APBC_H

#include <mach/addr-map.h>

/*                                           */
#define APBC_APBCLK	(1 << 0)  /*                      */
#define APBC_FNCLK	(1 << 1)  /*                         */
#define APBC_RST	(1 << 2)  /*                  */

/*                                 */
#define APBC_FNCLKSEL(x)	(((x) & 0xf) << 4)

#endif /*                        */
