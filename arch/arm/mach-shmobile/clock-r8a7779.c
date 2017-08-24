/*
 * r8a7779 clock framework support
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
#include <linux/bitops.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/io.h>
#include <linux/sh_clk.h>
#include <linux/clkdev.h>
#include <mach/clock.h>
#include <mach/common.h>

/*
                     
                                
                 
                                                                       
                                  
                                  
                                  
                                  
                                    
                                  
                                    
                                    
                                    
               
                                        
                                        
*/

#define MD(nr)	BIT(nr)

#define FRQMR		IOMEM(0xffc80014)
#define MSTPCR0		IOMEM(0xffc80030)
#define MSTPCR1		IOMEM(0xffc80034)
#define MSTPCR3		IOMEM(0xffc8003c)
#define MSTPSR1		IOMEM(0xffc80044)
#define MSTPSR4		IOMEM(0xffc80048)
#define MSTPSR6		IOMEM(0xffc8004c)
#define MSTPCR4		IOMEM(0xffc80050)
#define MSTPCR5		IOMEM(0xffc80054)
#define MSTPCR6		IOMEM(0xffc80058)
#define MSTPCR7		IOMEM(0xffc80040)

#define MODEMR		0xffcc0020


/*                                                   
                                                        
 */

static struct clk_mapping cpg_mapping = {
	.phys	= 0xffc80000,
	.len	= 0x80,
};

/*
                                                                        
                          
 */
static struct clk plla_clk = {
	/*                                               */
	.mapping	= &cpg_mapping,
};

/*
                                             
                          
 */
SH_FIXED_RATIO_CLK_SET(clkz_clk,	plla_clk, 1, 1);
SH_FIXED_RATIO_CLK_SET(clkzs_clk,	plla_clk, 1, 1);
SH_FIXED_RATIO_CLK_SET(clki_clk,	plla_clk, 1, 1);
SH_FIXED_RATIO_CLK_SET(clks_clk,	plla_clk, 1, 1);
SH_FIXED_RATIO_CLK_SET(clks1_clk,	plla_clk, 1, 1);
SH_FIXED_RATIO_CLK_SET(clks3_clk,	plla_clk, 1, 1);
SH_FIXED_RATIO_CLK_SET(clks4_clk,	plla_clk, 1, 1);
SH_FIXED_RATIO_CLK_SET(clkb_clk,	plla_clk, 1, 1);
SH_FIXED_RATIO_CLK_SET(clkout_clk,	plla_clk, 1, 1);
SH_FIXED_RATIO_CLK_SET(clkp_clk,	plla_clk, 1, 1);
SH_FIXED_RATIO_CLK_SET(clkg_clk,	plla_clk, 1, 1);

static struct clk *main_clks[] = {
	&plla_clk,
	&clkz_clk,
	&clkzs_clk,
	&clki_clk,
	&clks_clk,
	&clks1_clk,
	&clks3_clk,
	&clks4_clk,
	&clkb_clk,
	&clkout_clk,
	&clkp_clk,
	&clkg_clk,
};

enum { MSTP323, MSTP322, MSTP321, MSTP320,
	MSTP115, MSTP114,
	MSTP103, MSTP101, MSTP100,
	MSTP030,
	MSTP029, MSTP028, MSTP027, MSTP026, MSTP025, MSTP024, MSTP023, MSTP022, MSTP021,
	MSTP016, MSTP015, MSTP014,
	MSTP007,
	MSTP_NR };

static struct clk mstp_clks[MSTP_NR] = {
	[MSTP323] = SH_CLK_MSTP32(&clkp_clk, MSTPCR3, 23, 0), /*       */
	[MSTP322] = SH_CLK_MSTP32(&clkp_clk, MSTPCR3, 22, 0), /*       */
	[MSTP321] = SH_CLK_MSTP32(&clkp_clk, MSTPCR3, 21, 0), /*       */
	[MSTP320] = SH_CLK_MSTP32(&clkp_clk, MSTPCR3, 20, 0), /*       */
	[MSTP115] = SH_CLK_MSTP32(&clkp_clk, MSTPCR1, 15, 0), /*      */
	[MSTP114] = SH_CLK_MSTP32(&clkp_clk, MSTPCR1, 14, 0), /*       */
	[MSTP103] = SH_CLK_MSTP32(&clks_clk, MSTPCR1,  3, 0), /*    */
	[MSTP101] = SH_CLK_MSTP32(&clkp_clk, MSTPCR1,  1, 0), /*      */
	[MSTP100] = SH_CLK_MSTP32(&clkp_clk, MSTPCR1,  0, 0), /*        */
	[MSTP030] = SH_CLK_MSTP32(&clkp_clk, MSTPCR0, 30, 0), /*      */
	[MSTP029] = SH_CLK_MSTP32(&clkp_clk, MSTPCR0, 29, 0), /*      */
	[MSTP028] = SH_CLK_MSTP32(&clkp_clk, MSTPCR0, 28, 0), /*      */
	[MSTP027] = SH_CLK_MSTP32(&clkp_clk, MSTPCR0, 27, 0), /*      */
	[MSTP026] = SH_CLK_MSTP32(&clkp_clk, MSTPCR0, 26, 0), /*       */
	[MSTP025] = SH_CLK_MSTP32(&clkp_clk, MSTPCR0, 25, 0), /*       */
	[MSTP024] = SH_CLK_MSTP32(&clkp_clk, MSTPCR0, 24, 0), /*       */
	[MSTP023] = SH_CLK_MSTP32(&clkp_clk, MSTPCR0, 23, 0), /*       */
	[MSTP022] = SH_CLK_MSTP32(&clkp_clk, MSTPCR0, 22, 0), /*       */
	[MSTP021] = SH_CLK_MSTP32(&clkp_clk, MSTPCR0, 21, 0), /*       */
	[MSTP016] = SH_CLK_MSTP32(&clkp_clk, MSTPCR0, 16, 0), /*      */
	[MSTP015] = SH_CLK_MSTP32(&clkp_clk, MSTPCR0, 15, 0), /*      */
	[MSTP014] = SH_CLK_MSTP32(&clkp_clk, MSTPCR0, 14, 0), /*      */
	[MSTP007] = SH_CLK_MSTP32(&clks_clk, MSTPCR0,  7, 0), /*      */
};

static struct clk_lookup lookups[] = {
	/*             */
	CLKDEV_CON_ID("plla_clk", &plla_clk),
	CLKDEV_CON_ID("clkz_clk", &clkz_clk),
	CLKDEV_CON_ID("clkzs_clk", &clkzs_clk),

	/*             */
	CLKDEV_CON_ID("shyway_clk",	&clks_clk),
	CLKDEV_CON_ID("bus_clk",	&clkout_clk),
	CLKDEV_CON_ID("shyway4_clk",	&clks4_clk),
	CLKDEV_CON_ID("shyway3_clk",	&clks3_clk),
	CLKDEV_CON_ID("shyway1_clk",	&clks1_clk),
	CLKDEV_CON_ID("peripheral_clk",	&clkp_clk),

	/*               */
	CLKDEV_DEV_ID("sata_rcar", &mstp_clks[MSTP115]), /*      */
	CLKDEV_DEV_ID("fc600000.sata", &mstp_clks[MSTP115]), /*           */
	CLKDEV_DEV_ID("sh-eth", &mstp_clks[MSTP114]), /*       */
	CLKDEV_DEV_ID("ehci-platform.1", &mstp_clks[MSTP101]), /*                */
	CLKDEV_DEV_ID("ohci-platform.1", &mstp_clks[MSTP101]), /*                */
	CLKDEV_DEV_ID("ehci-platform.0", &mstp_clks[MSTP100]), /*                  */
	CLKDEV_DEV_ID("ohci-platform.0", &mstp_clks[MSTP100]), /*                  */
	CLKDEV_DEV_ID("sh_tmu.0", &mstp_clks[MSTP016]), /*       */
	CLKDEV_DEV_ID("sh_tmu.1", &mstp_clks[MSTP016]), /*       */
	CLKDEV_DEV_ID("sh_tmu.2", &mstp_clks[MSTP016]), /*       */
	CLKDEV_DEV_ID("i2c-rcar.0", &mstp_clks[MSTP030]), /*      */
	CLKDEV_DEV_ID("i2c-rcar.1", &mstp_clks[MSTP029]), /*      */
	CLKDEV_DEV_ID("i2c-rcar.2", &mstp_clks[MSTP028]), /*      */
	CLKDEV_DEV_ID("i2c-rcar.3", &mstp_clks[MSTP027]), /*      */
	CLKDEV_DEV_ID("sh-sci.0", &mstp_clks[MSTP026]), /*       */
	CLKDEV_DEV_ID("sh-sci.1", &mstp_clks[MSTP025]), /*       */
	CLKDEV_DEV_ID("sh-sci.2", &mstp_clks[MSTP024]), /*       */
	CLKDEV_DEV_ID("sh-sci.3", &mstp_clks[MSTP023]), /*       */
	CLKDEV_DEV_ID("sh-sci.4", &mstp_clks[MSTP022]), /*       */
	CLKDEV_DEV_ID("sh-sci.5", &mstp_clks[MSTP021]), /*       */
	CLKDEV_DEV_ID("sh-hspi.0", &mstp_clks[MSTP007]), /*       */
	CLKDEV_DEV_ID("sh-hspi.1", &mstp_clks[MSTP007]), /*       */
	CLKDEV_DEV_ID("sh-hspi.2", &mstp_clks[MSTP007]), /*       */
	CLKDEV_DEV_ID("sh_mobile_sdhi.0", &mstp_clks[MSTP323]), /*       */
	CLKDEV_DEV_ID("sh_mobile_sdhi.1", &mstp_clks[MSTP322]), /*       */
	CLKDEV_DEV_ID("sh_mobile_sdhi.2", &mstp_clks[MSTP321]), /*       */
	CLKDEV_DEV_ID("sh_mobile_sdhi.3", &mstp_clks[MSTP320]), /*       */
	CLKDEV_DEV_ID("rcar-du.0", &mstp_clks[MSTP103]), /*    */
};

void __init r8a7779_clock_init(void)
{
	void __iomem *modemr = ioremap_nocache(MODEMR, PAGE_SIZE);
	u32 mode;
	int k, ret = 0;

	BUG_ON(!modemr);
	mode = ioread32(modemr);
	iounmap(modemr);

	if (mode & MD(1)) {
		plla_clk.rate = 1500000000;

		SH_CLK_SET_RATIO(&clkz_clk_ratio,	2, 3);
		SH_CLK_SET_RATIO(&clkzs_clk_ratio,	1, 6);
		SH_CLK_SET_RATIO(&clki_clk_ratio,	1, 2);
		SH_CLK_SET_RATIO(&clks_clk_ratio,	1, 6);
		SH_CLK_SET_RATIO(&clks1_clk_ratio,	1, 12);
		SH_CLK_SET_RATIO(&clks3_clk_ratio,	1, 8);
		SH_CLK_SET_RATIO(&clks4_clk_ratio,	1, 16);
		SH_CLK_SET_RATIO(&clkp_clk_ratio,	1, 24);
		SH_CLK_SET_RATIO(&clkg_clk_ratio,	1, 24);
		if (mode & MD(2)) {
			SH_CLK_SET_RATIO(&clkb_clk_ratio,	1, 36);
			SH_CLK_SET_RATIO(&clkout_clk_ratio,	1, 36);
		} else {
			SH_CLK_SET_RATIO(&clkb_clk_ratio,	1, 24);
			SH_CLK_SET_RATIO(&clkout_clk_ratio,	1, 24);
		}
	} else {
		plla_clk.rate = 1600000000;

		SH_CLK_SET_RATIO(&clkz_clk_ratio,	1, 2);
		SH_CLK_SET_RATIO(&clkzs_clk_ratio,	1, 8);
		SH_CLK_SET_RATIO(&clki_clk_ratio,	1, 2);
		SH_CLK_SET_RATIO(&clks_clk_ratio,	1, 8);
		SH_CLK_SET_RATIO(&clks1_clk_ratio,	1, 16);
		SH_CLK_SET_RATIO(&clks3_clk_ratio,	1, 8);
		SH_CLK_SET_RATIO(&clks4_clk_ratio,	1, 16);
		SH_CLK_SET_RATIO(&clkp_clk_ratio,	1, 32);
		SH_CLK_SET_RATIO(&clkg_clk_ratio,	1, 24);
		if (mode & MD(2)) {
			SH_CLK_SET_RATIO(&clkb_clk_ratio,	1, 32);
			SH_CLK_SET_RATIO(&clkout_clk_ratio,	1, 32);
		} else {
			SH_CLK_SET_RATIO(&clkb_clk_ratio,	1, 24);
			SH_CLK_SET_RATIO(&clkout_clk_ratio,	1, 24);
		}
	}

	for (k = 0; !ret && (k < ARRAY_SIZE(main_clks)); k++)
		ret = clk_register(main_clks[k]);

	if (!ret)
		ret = sh_clk_mstp_register(mstp_clks, MSTP_NR);

	clkdev_add_table(lookups, ARRAY_SIZE(lookups));

	if (!ret)
		shmobile_clk_init();
	else
		panic("failed to setup r8a7779 clocks\n");
}
