/*
                                 
 */
#ifndef _ASM_SH_TITAN_H
#define _ASM_SH_TITAN_H

#include <linux/sh_intc.h>

#define __IO_PREFIX titan
#include <asm/io_generic.h>

/*                 */
#define TITAN_IRQ_WAN		evt2irq(0x240)	/*            */
#define TITAN_IRQ_LAN		evt2irq(0x2a0)	/*            */
#define TITAN_IRQ_MPCIA		evt2irq(0x300)	/*        */
#define TITAN_IRQ_MPCIB		evt2irq(0x360)	/*        */
#define TITAN_IRQ_USB		evt2irq(0x360)	/*     */

#endif /*                  */
