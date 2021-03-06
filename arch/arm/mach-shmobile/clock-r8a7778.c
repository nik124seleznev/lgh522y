/*
 * r8a7778 clock framework support
 *
 * Copyright (C) 2013  Renesas Solutions Corp.
 * Copyright (C) 2013  Kuninori Morimoto <kuninori.morimoto.gx@renesas.com>
 *
 * based on r8a7779
 *
 * Copyright (C) 2011  Renesas Solutions Corp.
 * Copyright (C) 2011  Magnus Damm
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <linux/io.h>
#include <linux/sh_clk.h>
#include <linux/clkdev.h>
#include <mach/common.h>

#define MSTPCR0		IOMEM(0xffc80030)
#define MSTPCR1		IOMEM(0xffc80034)
#define MSTPCR3		IOMEM(0xffc8003c)
#define MSTPSR1		IOMEM(0xffc80044)
#define MSTPSR4		IOMEM(0xffc80048)
#define MSTPSR6		IOMEM(0xffc8004c)
#define MSTPCR4		IOMEM(0xffc80050)
#define MSTPCR5		IOMEM(0xffc80054)
#define MSTPCR6		IOMEM(0xffc80058)

/*                                                   
                                                        
 */

static struct clk_mapping cpg_mapping = {
	.phys	= 0xffc80000,
	.len	= 0x80,
};

static struct clk clkp = {
	.rate   = 62500000, /*                 */
	.flags  = CLK_ENABLE_ON_INIT,
	.mapping = &cpg_mapping,
};

static struct clk *main_clks[] = {
	&clkp,
};

enum {
	MSTP114,
	MSTP026, MSTP025, MSTP024, MSTP023, MSTP022, MSTP021,
	MSTP016, MSTP015,
	MSTP_NR };

static struct clk mstp_clks[MSTP_NR] = {
	[MSTP114] = SH_CLK_MSTP32(&clkp, MSTPCR1, 14, 0), /*       */
	[MSTP026] = SH_CLK_MSTP32(&clkp, MSTPCR0, 26, 0), /*       */
	[MSTP025] = SH_CLK_MSTP32(&clkp, MSTPCR0, 25, 0), /*       */
	[MSTP024] = SH_CLK_MSTP32(&clkp, MSTPCR0, 24, 0), /*       */
	[MSTP023] = SH_CLK_MSTP32(&clkp, MSTPCR0, 23, 0), /*       */
	[MSTP022] = SH_CLK_MSTP32(&clkp, MSTPCR0, 22, 0), /*       */
	[MSTP021] = SH_CLK_MSTP32(&clkp, MSTPCR0, 21, 0), /*       */
	[MSTP016] = SH_CLK_MSTP32(&clkp, MSTPCR0, 16, 0), /*      */
	[MSTP015] = SH_CLK_MSTP32(&clkp, MSTPCR0, 15, 0), /*      */
};

static struct clk_lookup lookups[] = {
	/*               */
	CLKDEV_DEV_ID("sh-eth",	&mstp_clks[MSTP114]), /*       */
	CLKDEV_DEV_ID("sh-sci.0", &mstp_clks[MSTP026]), /*       */
	CLKDEV_DEV_ID("sh-sci.1", &mstp_clks[MSTP025]), /*       */
	CLKDEV_DEV_ID("sh-sci.2", &mstp_clks[MSTP024]), /*       */
	CLKDEV_DEV_ID("sh-sci.3", &mstp_clks[MSTP023]), /*       */
	CLKDEV_DEV_ID("sh-sci.4", &mstp_clks[MSTP022]), /*       */
	CLKDEV_DEV_ID("sh-sci.5", &mstp_clks[MSTP021]), /*       */
	CLKDEV_DEV_ID("sh_tmu.0", &mstp_clks[MSTP016]), /*       */
	CLKDEV_DEV_ID("sh_tmu.1", &mstp_clks[MSTP015]), /*       */
};

void __init r8a7778_clock_init(void)
{
	int k, ret = 0;

	for (k = 0; !ret && (k < ARRAY_SIZE(main_clks)); k++)
		ret = clk_register(main_clks[k]);

	if (!ret)
		ret = sh_clk_mstp_register(mstp_clks, MSTP_NR);

	clkdev_add_table(lookups, ARRAY_SIZE(lookups));

	if (!ret)
		shmobile_clk_init();
	else
		panic("failed to setup r8a7778 clocks\n");
}
