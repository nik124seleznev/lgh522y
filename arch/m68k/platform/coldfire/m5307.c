/*                                                                         */

/*
 *	linux/arch/m68knommu/platform/5307/config.c
 *
 *	Copyright (C) 1999-2002, Greg Ungerer (gerg@snapgear.com)
 *	Copyright (C) 2000, Lineo (www.lineo.com)
 */

/*                                                                         */

#include <linux/kernel.h>
#include <linux/param.h>
#include <linux/init.h>
#include <linux/io.h>
#include <asm/machdep.h>
#include <asm/coldfire.h>
#include <asm/mcfsim.h>
#include <asm/mcfwdebug.h>
#include <asm/mcfclk.h>

/*                                                                         */

/*
                                                                    
 */
unsigned short ppdata;
unsigned char ledbank = 0xff;

/*                                                                         */

DEFINE_CLK(pll, "pll.0", MCF_CLK);
DEFINE_CLK(sys, "sys.0", MCF_BUSCLK);
DEFINE_CLK(mcftmr0, "mcftmr.0", MCF_BUSCLK);
DEFINE_CLK(mcftmr1, "mcftmr.1", MCF_BUSCLK);
DEFINE_CLK(mcfuart0, "mcfuart.0", MCF_BUSCLK);
DEFINE_CLK(mcfuart1, "mcfuart.1", MCF_BUSCLK);

struct clk *mcf_clks[] = {
	&clk_pll,
	&clk_sys,
	&clk_mcftmr0,
	&clk_mcftmr1,
	&clk_mcfuart0,
	&clk_mcfuart1,
	NULL
};

/*                                                                         */

void __init config_BSP(char *commandp, int size)
{
#if defined(CONFIG_NETtel) || \
    defined(CONFIG_SECUREEDGEMP3) || defined(CONFIG_CLEOPATRA)
	/*                                                 */
	memcpy(commandp, (char *) 0xf0004000, size);
	commandp[size-1] = 0;
#endif

	mach_sched_init = hw_timer_init;

	/*                                                             */
	mcf_mapirq2imr(25, MCFINTC_EINT1);
	mcf_mapirq2imr(27, MCFINTC_EINT3);
	mcf_mapirq2imr(29, MCFINTC_EINT5);
	mcf_mapirq2imr(31, MCFINTC_EINT7);

#ifdef CONFIG_BDM_DISABLE
	/*
                                                                      
                                                                     
                                                   
  */
	wdebug(MCFDEBUG_CSR, MCFDEBUG_CSR_PSTCLK);
#endif
}

/*                                                                         */
