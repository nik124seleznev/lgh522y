/*                                                                         */

/*
 *	m53xx.c -- platform support for ColdFire 53xx based boards
 *
 *	Copyright (C) 1999-2002, Greg Ungerer (gerg@snapgear.com)
 *	Copyright (C) 2000, Lineo (www.lineo.com)
 *	Yaroslav Vinogradov yaroslav.vinogradov@freescale.com
 *	Copyright Freescale Semiconductor, Inc 2006
 *	Copyright (c) 2006, emlix, Sebastian Hess <shess@hessware.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

/*                                                                         */

#include <linux/kernel.h>
#include <linux/param.h>
#include <linux/init.h>
#include <linux/io.h>
#include <asm/machdep.h>
#include <asm/coldfire.h>
#include <asm/mcfsim.h>
#include <asm/mcfuart.h>
#include <asm/mcfdma.h>
#include <asm/mcfwdebug.h>
#include <asm/mcfclk.h>

/*                                                                         */

DEFINE_CLK(0, "flexbus", 2, MCF_CLK);
DEFINE_CLK(0, "mcfcan.0", 8, MCF_CLK);
DEFINE_CLK(0, "fec.0", 12, MCF_CLK);
DEFINE_CLK(0, "edma", 17, MCF_CLK);
DEFINE_CLK(0, "intc.0", 18, MCF_CLK);
DEFINE_CLK(0, "intc.1", 19, MCF_CLK);
DEFINE_CLK(0, "iack.0", 21, MCF_CLK);
DEFINE_CLK(0, "mcfi2c.0", 22, MCF_CLK);
DEFINE_CLK(0, "mcfqspi.0", 23, MCF_CLK);
DEFINE_CLK(0, "mcfuart.0", 24, MCF_BUSCLK);
DEFINE_CLK(0, "mcfuart.1", 25, MCF_BUSCLK);
DEFINE_CLK(0, "mcfuart.2", 26, MCF_BUSCLK);
DEFINE_CLK(0, "mcftmr.0", 28, MCF_CLK);
DEFINE_CLK(0, "mcftmr.1", 29, MCF_CLK);
DEFINE_CLK(0, "mcftmr.2", 30, MCF_CLK);
DEFINE_CLK(0, "mcftmr.3", 31, MCF_CLK);

DEFINE_CLK(0, "mcfpit.0", 32, MCF_CLK);
DEFINE_CLK(0, "mcfpit.1", 33, MCF_CLK);
DEFINE_CLK(0, "mcfpit.2", 34, MCF_CLK);
DEFINE_CLK(0, "mcfpit.3", 35, MCF_CLK);
DEFINE_CLK(0, "mcfpwm.0", 36, MCF_CLK);
DEFINE_CLK(0, "mcfeport.0", 37, MCF_CLK);
DEFINE_CLK(0, "mcfwdt.0", 38, MCF_CLK);
DEFINE_CLK(0, "sys.0", 40, MCF_BUSCLK);
DEFINE_CLK(0, "gpio.0", 41, MCF_BUSCLK);
DEFINE_CLK(0, "mcfrtc.0", 42, MCF_CLK);
DEFINE_CLK(0, "mcflcd.0", 43, MCF_CLK);
DEFINE_CLK(0, "mcfusb-otg.0", 44, MCF_CLK);
DEFINE_CLK(0, "mcfusb-host.0", 45, MCF_CLK);
DEFINE_CLK(0, "sdram.0", 46, MCF_CLK);
DEFINE_CLK(0, "ssi.0", 47, MCF_CLK);
DEFINE_CLK(0, "pll.0", 48, MCF_CLK);

DEFINE_CLK(1, "mdha.0", 32, MCF_CLK);
DEFINE_CLK(1, "skha.0", 33, MCF_CLK);
DEFINE_CLK(1, "rng.0", 34, MCF_CLK);

struct clk *mcf_clks[] = {
	&__clk_0_2,	/*         */
	&__clk_0_8,	/*          */
	&__clk_0_12,	/*       */
	&__clk_0_17,	/*      */
	&__clk_0_18,	/*        */
	&__clk_0_19,	/*        */
	&__clk_0_21,	/*        */
	&__clk_0_22,	/*          */
	&__clk_0_23,	/*           */
	&__clk_0_24,	/*           */
	&__clk_0_25,	/*           */
	&__clk_0_26,	/*           */
	&__clk_0_28,	/*          */
	&__clk_0_29,	/*          */
	&__clk_0_30,	/*          */
	&__clk_0_31,	/*          */

	&__clk_0_32,	/*          */
	&__clk_0_33,	/*          */
	&__clk_0_34,	/*          */
	&__clk_0_35,	/*          */
	&__clk_0_36,	/*          */
	&__clk_0_37,	/*            */
	&__clk_0_38,	/*          */
	&__clk_0_40,	/*       */
	&__clk_0_41,	/*        */
	&__clk_0_42,	/*          */
	&__clk_0_43,	/*          */
	&__clk_0_44,	/*              */
	&__clk_0_45,	/*               */
	&__clk_0_46,	/*         */
	&__clk_0_47,	/*       */
	&__clk_0_48,	/*       */

	&__clk_1_32,	/*        */
	&__clk_1_33,	/*        */
	&__clk_1_34,	/*       */
	NULL,
};

static struct clk * const enable_clks[] __initconst = {
	&__clk_0_2,	/*         */
	&__clk_0_18,	/*        */
	&__clk_0_19,	/*        */
	&__clk_0_21,	/*        */
	&__clk_0_24,	/*           */
	&__clk_0_25,	/*           */
	&__clk_0_26,	/*           */
	&__clk_0_28,	/*          */
	&__clk_0_29,	/*          */
	&__clk_0_32,	/*          */
	&__clk_0_33,	/*          */
	&__clk_0_37,	/*            */
	&__clk_0_40,	/*       */
	&__clk_0_41,	/*        */
	&__clk_0_46,	/*         */
	&__clk_0_48,	/*       */
};

static struct clk * const disable_clks[] __initconst = {
	&__clk_0_8,	/*          */
	&__clk_0_12,	/*       */
	&__clk_0_17,	/*      */
	&__clk_0_22,	/*          */
	&__clk_0_23,	/*           */
	&__clk_0_30,	/*          */
	&__clk_0_31,	/*          */
	&__clk_0_34,	/*          */
	&__clk_0_35,	/*          */
	&__clk_0_36,	/*          */
	&__clk_0_38,	/*          */
	&__clk_0_42,	/*          */
	&__clk_0_43,	/*          */
	&__clk_0_44,	/*              */
	&__clk_0_45,	/*               */
	&__clk_0_47,	/*       */
	&__clk_1_32,	/*        */
	&__clk_1_33,	/*        */
	&__clk_1_34,	/*       */
};


static void __init m53xx_clk_init(void)
{
	unsigned i;

	/*                                    */
	for (i = 0; i < ARRAY_SIZE(enable_clks); ++i)
		__clk_init_enabled(enable_clks[i]);
	/*                                     */
	for (i = 0; i < ARRAY_SIZE(disable_clks); ++i)
		__clk_init_disabled(disable_clks[i]);
}

/*                                                                         */

#if IS_ENABLED(CONFIG_SPI_COLDFIRE_QSPI)

static void __init m53xx_qspi_init(void)
{
	/*                                               */
	writew(0x01f0, MCFGPIO_PAR_QSPI);
}

#endif /*                                      */

/*                                                                         */

static void __init m53xx_uarts_init(void)
{
	/*                          */
	writew(readw(MCFGPIO_PAR_UART) | 0x0FFF, MCFGPIO_PAR_UART);
}

/*                                                                         */

static void __init m53xx_fec_init(void)
{
	u8 v;

	/*                                                   */
	v = readb(MCFGPIO_PAR_FECI2C);
	v |= MCF_GPIO_PAR_FECI2C_PAR_MDC_EMDC |
		MCF_GPIO_PAR_FECI2C_PAR_MDIO_EMDIO;
	writeb(v, MCFGPIO_PAR_FECI2C);

	v = readb(MCFGPIO_PAR_FEC);
	v = MCF_GPIO_PAR_FEC_PAR_FEC_7W_FEC | MCF_GPIO_PAR_FEC_PAR_FEC_MII_FEC;
	writeb(v, MCFGPIO_PAR_FEC);
}

/*                                                                         */

void __init config_BSP(char *commandp, int size)
{
#if !defined(CONFIG_BOOTPARAM)
	/*                                                 */
	memcpy(commandp, (char *) 0x4000, 4);
	if(strncmp(commandp, "kcl ", 4) == 0){
		memcpy(commandp, (char *) 0x4004, size);
		commandp[size-1] = 0;
	} else {
		memset(commandp, 0, size);
	}
#endif
	mach_sched_init = hw_timer_init;
	m53xx_clk_init();
	m53xx_uarts_init();
	m53xx_fec_init();
#if IS_ENABLED(CONFIG_SPI_COLDFIRE_QSPI)
	m53xx_qspi_init();
#endif

#ifdef CONFIG_BDM_DISABLE
	/*
                                                                      
                                                                     
                                                   
  */
	wdebug(MCFDEBUG_CSR, MCFDEBUG_CSR_PSTCLK);
#endif
}

/*                                                                         */
/*                      */
/*                                                                         */
/* 
                             
 */
#define MAX_FVCO	500000	/*     */
#define MAX_FSYS	80000 	/*     */
#define MIN_FSYS	58333 	/*     */
#define FREF		16000   /*     */


#define MAX_MFD		135     /*            */
#define MIN_MFD		88      /*            */
#define BUSDIV		6       /*         */

/*
                                   
 */
#define MIN_LPD		(1 << 0)    /*                       */
#define MAX_LPD		(1 << 15)   /*                       */
#define DEFAULT_LPD	(1 << 1)	/*                       */

#define SYS_CLK_KHZ	80000
#define SYSTEM_PERIOD	12.5
/*
                           
 */  
#define SDRAM_BL	8	/*                       */
#define SDRAM_TWR	2	/*           */
#define SDRAM_CASL	2.5	/*                */
#define SDRAM_TRCD	2	/*           */
#define SDRAM_TRP	2	/*           */
#define SDRAM_TRFC	7	/*           */
#define SDRAM_TREFI	7800	/*       */

#define EXT_SRAM_ADDRESS	(0xC0000000)
#define FLASH_ADDRESS		(0x00000000)
#define SDRAM_ADDRESS		(0x40000000)

#define NAND_FLASH_ADDRESS	(0xD0000000)

int sys_clk_khz = 0;
int sys_clk_mhz = 0;

void wtm_init(void);
void scm_init(void);
void gpio_init(void);
void fbcs_init(void);
void sdramc_init(void);
int  clock_pll (int fsys, int flags);
int  clock_limp (int);
int  clock_exit_limp (void);
int  get_sys_clock (void);

asmlinkage void __init sysinit(void)
{
	sys_clk_khz = clock_pll(0, 0);
	sys_clk_mhz = sys_clk_khz/1000;
	
	wtm_init();
	scm_init();
	gpio_init();
	fbcs_init();
	sdramc_init();
}

void wtm_init(void)
{
	/*                        */
	writew(0, MCF_WTM_WCR);
}

#define MCF_SCM_BCR_GBW		(0x00000100)
#define MCF_SCM_BCR_GBR		(0x00000200)

void scm_init(void)
{
	/*                         */
	writel(0x77777777, MCF_SCM_MPR);
    
	/*                                                         
                         */
	writel(0, MCF_SCM_PACRA);
	writel(0, MCF_SCM_PACRB);
	writel(0, MCF_SCM_PACRC);
	writel(0, MCF_SCM_PACRD);
	writel(0, MCF_SCM_PACRE);
	writel(0, MCF_SCM_PACRF);

	/*               */
	writel(MCF_SCM_BCR_GBR | MCF_SCM_BCR_GBW, MCF_SCM_BCR);
}


void fbcs_init(void)
{
	writeb(0x3E, MCFGPIO_PAR_CS);

	/*                   */
	writel(0x10080000, MCF_FBCS1_CSAR);

	writel(0x002A3780, MCF_FBCS1_CSCR);
	writel(MCF_FBCS_CSMR_BAM_2M | MCF_FBCS_CSMR_V, MCF_FBCS1_CSMR);

	/*                                                      */
	writew(0xffff, 0x10080000);

	/*               */
	writel(EXT_SRAM_ADDRESS, MCF_FBCS1_CSAR);
	writel(MCF_FBCS_CSCR_PS_16 |
		MCF_FBCS_CSCR_AA |
		MCF_FBCS_CSCR_SBM |
		MCF_FBCS_CSCR_WS(1),
		MCF_FBCS1_CSCR);
	writel(MCF_FBCS_CSMR_BAM_512K | MCF_FBCS_CSMR_V, MCF_FBCS1_CSMR);

	/*                               */
	writel(FLASH_ADDRESS, MCF_FBCS0_CSAR);
	writel(MCF_FBCS_CSCR_PS_16 |
		MCF_FBCS_CSCR_BEM |
		MCF_FBCS_CSCR_AA |
		MCF_FBCS_CSCR_SBM |
		MCF_FBCS_CSCR_WS(7),
		MCF_FBCS0_CSCR);
	writel(MCF_FBCS_CSMR_BAM_32M | MCF_FBCS_CSMR_V, MCF_FBCS0_CSMR);
}

void sdramc_init(void)
{
	/*
                                                          
                         
  */
	if (!(readl(MCF_SDRAMC_SDCR) & MCF_SDRAMC_SDCR_REF)) {
		/*                                  */
		
		/*                              */
		writel(MCF_SDRAMC_SDCS_BA(SDRAM_ADDRESS) |
			MCF_SDRAMC_SDCS_CSSZ(MCF_SDRAMC_SDCS_CSSZ_32MBYTE),
			MCF_SDRAMC_SDCS0);

	/*
                                          
  */
	writel(MCF_SDRAMC_SDCFG1_SRD2RW((int)((SDRAM_CASL + 2) + 0.5)) |
		MCF_SDRAMC_SDCFG1_SWT2RD(SDRAM_TWR + 1) |
		MCF_SDRAMC_SDCFG1_RDLAT((int)((SDRAM_CASL * 2) + 2)) |
		MCF_SDRAMC_SDCFG1_ACT2RW((int)(SDRAM_TRCD + 0.5)) |
		MCF_SDRAMC_SDCFG1_PRE2ACT((int)(SDRAM_TRP + 0.5)) |
		MCF_SDRAMC_SDCFG1_REF2ACT((int)(SDRAM_TRFC + 0.5)) |
		MCF_SDRAMC_SDCFG1_WTLAT(3),
		MCF_SDRAMC_SDCFG1);
	writel(MCF_SDRAMC_SDCFG2_BRD2PRE(SDRAM_BL / 2 + 1) |
		MCF_SDRAMC_SDCFG2_BWT2RW(SDRAM_BL / 2 + SDRAM_TWR) |
		MCF_SDRAMC_SDCFG2_BRD2WT((int)((SDRAM_CASL + SDRAM_BL / 2 - 1.0) + 0.5)) |
		MCF_SDRAMC_SDCFG2_BL(SDRAM_BL - 1),
		MCF_SDRAMC_SDCFG2);

            
	/*
                                      
  */
	writel(MCF_SDRAMC_SDCR_MODE_EN |
		MCF_SDRAMC_SDCR_CKE |
		MCF_SDRAMC_SDCR_DDR |
		MCF_SDRAMC_SDCR_MUX(1) |
		MCF_SDRAMC_SDCR_RCNT((int)(((SDRAM_TREFI / (SYSTEM_PERIOD * 64)) - 1) + 0.5)) |
		MCF_SDRAMC_SDCR_PS_16 |
		MCF_SDRAMC_SDCR_IPALL,
		MCF_SDRAMC_SDCR);

	/*
                                
  */
	writel(MCF_SDRAMC_SDMR_BNKAD_LEMR |
		MCF_SDRAMC_SDMR_AD(0x0) |
		MCF_SDRAMC_SDMR_CMD,
		MCF_SDRAMC_SDMR);

	/*
                                     
  */
	writel(MCF_SDRAMC_SDMR_BNKAD_LMR |
		MCF_SDRAMC_SDMR_AD(0x163) |
		MCF_SDRAMC_SDMR_CMD,
		MCF_SDRAMC_SDMR);

	/*
                          
  */
	writel(readl(MCF_SDRAMC_SDCR) | MCF_SDRAMC_SDCR_IPALL, MCF_SDRAMC_SDCR);

	/*
                          
  */
	writel(readl(MCF_SDRAMC_SDCR) | MCF_SDRAMC_SDCR_IREF, MCF_SDRAMC_SDCR);
	writel(readl(MCF_SDRAMC_SDCR) | MCF_SDRAMC_SDCR_IREF, MCF_SDRAMC_SDCR);

	/*
                                           
  */
	writel(MCF_SDRAMC_SDMR_BNKAD_LMR |
		MCF_SDRAMC_SDMR_AD(0x063) |
		MCF_SDRAMC_SDMR_CMD,
		MCF_SDRAMC_SDMR);
				
	/*
                                     
  */
	writel(readl(MCF_SDRAMC_SDCR) & ~MCF_SDRAMC_SDCR_MODE_EN,
		MCF_SDRAMC_SDCR);
	writel(MCF_SDRAMC_SDCR_REF | MCF_SDRAMC_SDCR_DQS_OE(0xC),
		MCF_SDRAMC_SDCR);
	}
}

void gpio_init(void)
{
	/*                   */
	writew(MCF_GPIO_PAR_UART_PAR_URXD0 | MCF_GPIO_PAR_UART_PAR_UTXD0,
		MCFGPIO_PAR_UART);

	/*
                                                        
                      
  */
	writeb(0x00, MCFGPIO_PAR_TIMER);
	writeb(0x08, MCFGPIO_PDDR_TIMER);
	writeb(0x00, MCFGPIO_PCLRR_TIMER);
}

int clock_pll(int fsys, int flags)
{
	int fref, temp, fout, mfd;
	u32 i;

	fref = FREF;
        
	if (fsys == 0) {
		/*                           */
		mfd = readb(MCF_PLL_PFDR);

		return (fref * mfd / (BUSDIV * 4));
	}

	/*                                        */
	if (fsys > MAX_FSYS)
		fsys = MAX_FSYS;
	if (fsys < MIN_FSYS)
		fsys = MIN_FSYS;

	/*                                                    
                                                        
                                                        
                     */
	temp = 100 * fsys / fref;
	mfd = 4 * BUSDIV * temp / 100;
    	    	    	
	/*                                                    */
	fout = (fref * mfd / (BUSDIV * 4));

	/*
                                                           
                                                              
                                      
  */
	if (readl(MCF_SDRAMC_SDCR) & MCF_SDRAMC_SDCR_REF)
		/*                                  */
		writel(readl(MCF_SDRAMC_SDCR) & ~MCF_SDRAMC_SDCR_CKE,
			MCF_SDRAMC_SDCR);

	/*
                                                                  
                                                               
  */

	/*                 */
	clock_limp(DEFAULT_LPD);
     					
	/*                                */
	writeb(MCF_PLL_PODR_CPUDIV(BUSDIV/3) | MCF_PLL_PODR_BUSDIV(BUSDIV),
		MCF_PLL_PODR);
						
	writeb(mfd, MCF_PLL_PFDR);
		
	/*                */
	clock_exit_limp();
	
	/*
                                                         
  */
	if (readl(MCF_SDRAMC_SDCR) & MCF_SDRAMC_SDCR_REF)
		/*                        */
		writel(readl(MCF_SDRAMC_SDCR) | MCF_SDRAMC_SDCR_CKE,
			MCF_SDRAMC_SDCR);

	/*                                                                 */
	writel(MCF_SDRAMC_REFRESH, MCF_SDRAMC_LIMP_FIX);

	/*                              */
	for (i = 0; i < 0x200; i++)
		;

	return fout;
}

int clock_limp(int div)
{
	u32 temp;

	/*                         */
	if (div < MIN_LPD)
		div = MIN_LPD;
	if (div > MAX_LPD)
		div = MAX_LPD;
    
	/*                                                    
                       */
	temp = readw(MCF_CCM_CDR) & MCF_CCM_CDR_SSIDIV(0xF);
      
	/*                                       */
	writew(MCF_CCM_CDR_LPDIV(div) | MCF_CCM_CDR_SSIDIV(temp), MCF_CCM_CDR);
    
	writew(readw(MCF_CCM_MISCCR) | MCF_CCM_MISCCR_LIMP, MCF_CCM_MISCCR);
    
	return (FREF/(3*(1 << div)));
}

int clock_exit_limp(void)
{
	int fout;
	
	/*                */
	writew(readw(MCF_CCM_MISCCR) & ~MCF_CCM_MISCCR_LIMP, MCF_CCM_MISCCR);

	/*                      */
	while (!(readw(MCF_CCM_MISCCR) & MCF_CCM_MISCCR_PLL_LOCK))
		;
	
	fout = get_sys_clock();

	return fout;
}

int get_sys_clock(void)
{
	int divider;
	
	/*                                       */
	if (readw(MCF_CCM_MISCCR) & MCF_CCM_MISCCR_LIMP) {
		divider = readw(MCF_CCM_CDR) & MCF_CCM_CDR_LPDIV(0xF);
		return (FREF/(2 << divider));
	}
	else
		return (FREF * readb(MCF_PLL_PFDR)) / (BUSDIV * 4);
}
