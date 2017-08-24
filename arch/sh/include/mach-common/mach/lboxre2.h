#ifndef __ASM_SH_LBOXRE2_H
#define __ASM_SH_LBOXRE2_H

/*
 * Copyright (C) 2007 Nobuhiro Iwamatsu
 *
 * NTT COMWARE L-BOX RE2 support
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 */
#include <linux/sh_intc.h>

#define IRQ_CF1		evt2irq(0x320)	/*     */
#define IRQ_CF0		evt2irq(0x340)	/*     */
#define IRQ_INTD	evt2irq(0x360)	/*      */
#define IRQ_ETH1	evt2irq(0x380)	/*        */
#define IRQ_ETH0	evt2irq(0x3a0)	/*        */
#define IRQ_INTA	evt2irq(0x3c0)	/*      */

void init_lboxre2_IRQ(void);

#define __IO_PREFIX	lboxre2
#include <asm/io_generic.h>

#endif  /*                    */
