/*
 * AM33XX Power Management register bits
 *
 * This file is automatically generated from the AM33XX hardware databases.
 * Vaibhav Hiremath <hvaibhav@ti.com>
 *
 * Copyright (C) 2011-2012 Texas Instruments Incorporated - http://www.ti.com/
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */


#ifndef __ARCH_ARM_MACH_OMAP2_CM_REGBITS_33XX_H
#define __ARCH_ARM_MACH_OMAP2_CM_REGBITS_33XX_H

/*
                                                                              
                                             
 */
#define AM33XX_AUTO_DPLL_MODE_SHIFT			0
#define AM33XX_AUTO_DPLL_MODE_WIDTH			3
#define AM33XX_AUTO_DPLL_MODE_MASK			(0x7 << 0)

/*                           */
#define AM33XX_CLKACTIVITY_ADC_FCLK_SHIFT		14
#define AM33XX_CLKACTIVITY_ADC_FCLK_WIDTH		1
#define AM33XX_CLKACTIVITY_ADC_FCLK_MASK		(1 << 16)

/*                               */
#define AM33XX_CLKACTIVITY_CAN_CLK_SHIFT		11
#define AM33XX_CLKACTIVITY_CAN_CLK_WIDTH		1
#define AM33XX_CLKACTIVITY_CAN_CLK_MASK			(1 << 11)

/*                                    */
#define AM33XX_CLKACTIVITY_CLK_24MHZ_GCLK_SHIFT		4
#define AM33XX_CLKACTIVITY_CLK_24MHZ_GCLK_WIDTH		1
#define AM33XX_CLKACTIVITY_CLK_24MHZ_GCLK_MASK		(1 << 4)

/*                               */
#define AM33XX_CLKACTIVITY_CPSW_125MHZ_GCLK_SHIFT	4
#define AM33XX_CLKACTIVITY_CPSW_125MHZ_GCLK_WIDTH	1
#define AM33XX_CLKACTIVITY_CPSW_125MHZ_GCLK_MASK	(1 << 4)

/*                               */
#define AM33XX_CLKACTIVITY_CPSW_250MHZ_GCLK_SHIFT	4
#define AM33XX_CLKACTIVITY_CPSW_250MHZ_GCLK_WIDTH	1
#define AM33XX_CLKACTIVITY_CPSW_250MHZ_GCLK_MASK	(1 << 4)

/*                               */
#define AM33XX_CLKACTIVITY_CPSW_50MHZ_GCLK_SHIFT	5
#define AM33XX_CLKACTIVITY_CPSW_50MHZ_GCLK_WIDTH	1
#define AM33XX_CLKACTIVITY_CPSW_50MHZ_GCLK_MASK		(1 << 5)

/*                               */
#define AM33XX_CLKACTIVITY_CPSW_5MHZ_GCLK_SHIFT		6
#define AM33XX_CLKACTIVITY_CPSW_5MHZ_GCLK_WIDTH		1
#define AM33XX_CLKACTIVITY_CPSW_5MHZ_GCLK_MASK		(1 << 6)

/*                             */
#define AM33XX_CLKACTIVITY_CPTS_RFT_GCLK_SHIFT		6
#define AM33XX_CLKACTIVITY_CPTS_RFT_GCLK_WIDTH		1
#define AM33XX_CLKACTIVITY_CPTS_RFT_GCLK_MASK		(1 << 6)

/*                             */
#define AM33XX_CLKACTIVITY_CUST_EFUSE_SYS_CLK_SHIFT	9
#define AM33XX_CLKACTIVITY_CUST_EFUSE_SYS_CLK_WIDTH	1
#define AM33XX_CLKACTIVITY_CUST_EFUSE_SYS_CLK_MASK	(1 << 9)

/*                             */
#define AM33XX_CLKACTIVITY_DBGSYSCLK_SHIFT		2
#define AM33XX_CLKACTIVITY_DBGSYSCLK_WIDTH		1
#define AM33XX_CLKACTIVITY_DBGSYSCLK_MASK		(1 << 2)

/*                             */
#define AM33XX_CLKACTIVITY_DEBUG_CLKA_SHIFT		4
#define AM33XX_CLKACTIVITY_DEBUG_CLKA_WIDTH		1
#define AM33XX_CLKACTIVITY_DEBUG_CLKA_MASK		(1 << 4)

/*                             */
#define AM33XX_CLKACTIVITY_EMIF_GCLK_SHIFT		2
#define AM33XX_CLKACTIVITY_EMIF_GCLK_WIDTH		1
#define AM33XX_CLKACTIVITY_EMIF_GCLK_MASK		(1 << 2)

/*                             */
#define AM33XX_CLKACTIVITY_GFX_FCLK_SHIFT		9
#define AM33XX_CLKACTIVITY_GFX_FCLK_WIDTH		1
#define AM33XX_CLKACTIVITY_GFX_FCLK_MASK		(1 << 9)

/*                             */
#define AM33XX_CLKACTIVITY_GFX_L3_GCLK_SHIFT		8
#define AM33XX_CLKACTIVITY_GFX_L3_GCLK_WIDTH		1
#define AM33XX_CLKACTIVITY_GFX_L3_GCLK_MASK		(1 << 8)

/*                           */
#define AM33XX_CLKACTIVITY_GPIO0_GDBCLK_SHIFT		8
#define AM33XX_CLKACTIVITY_GPIO0_GDBCLK_WIDTH		1
#define AM33XX_CLKACTIVITY_GPIO0_GDBCLK_MASK		(1 << 8)

/*                               */
#define AM33XX_CLKACTIVITY_GPIO_1_GDBCLK_SHIFT		19
#define AM33XX_CLKACTIVITY_GPIO_1_GDBCLK_WIDTH		1
#define AM33XX_CLKACTIVITY_GPIO_1_GDBCLK_MASK		(1 << 19)

/*                               */
#define AM33XX_CLKACTIVITY_GPIO_2_GDBCLK_SHIFT		20
#define AM33XX_CLKACTIVITY_GPIO_2_GDBCLK_WIDTH		1
#define AM33XX_CLKACTIVITY_GPIO_2_GDBCLK_MASK		(1 << 20)

/*                               */
#define AM33XX_CLKACTIVITY_GPIO_3_GDBCLK_SHIFT		21
#define AM33XX_CLKACTIVITY_GPIO_3_GDBCLK_WIDTH		1
#define AM33XX_CLKACTIVITY_GPIO_3_GDBCLK_MASK		(1 << 21)

/*                               */
#define AM33XX_CLKACTIVITY_GPIO_4_GDBCLK_SHIFT		22
#define AM33XX_CLKACTIVITY_GPIO_4_GDBCLK_WIDTH		1
#define AM33XX_CLKACTIVITY_GPIO_4_GDBCLK_MASK		(1 << 22)

/*                               */
#define AM33XX_CLKACTIVITY_GPIO_5_GDBCLK_SHIFT		26
#define AM33XX_CLKACTIVITY_GPIO_5_GDBCLK_WIDTH		1
#define AM33XX_CLKACTIVITY_GPIO_5_GDBCLK_MASK		(1 << 26)

/*                               */
#define AM33XX_CLKACTIVITY_GPIO_6_GDBCLK_SHIFT		18
#define AM33XX_CLKACTIVITY_GPIO_6_GDBCLK_WIDTH		1
#define AM33XX_CLKACTIVITY_GPIO_6_GDBCLK_MASK		(1 << 18)

/*                           */
#define AM33XX_CLKACTIVITY_I2C0_GFCLK_SHIFT		11
#define AM33XX_CLKACTIVITY_I2C0_GFCLK_WIDTH		1
#define AM33XX_CLKACTIVITY_I2C0_GFCLK_MASK		(1 << 11)

/*                               */
#define AM33XX_CLKACTIVITY_I2C_FCLK_SHIFT		24
#define AM33XX_CLKACTIVITY_I2C_FCLK_WIDTH		1
#define AM33XX_CLKACTIVITY_I2C_FCLK_MASK		(1 << 24)

/*                                */
#define AM33XX_CLKACTIVITY_PRUSS_IEP_GCLK_SHIFT		5
#define AM33XX_CLKACTIVITY_PRUSS_IEP_GCLK_WIDTH		1
#define AM33XX_CLKACTIVITY_PRUSS_IEP_GCLK_MASK		(1 << 5)

/*                                */
#define AM33XX_CLKACTIVITY_PRUSS_OCP_GCLK_SHIFT		4
#define AM33XX_CLKACTIVITY_PRUSS_OCP_GCLK_WIDTH		1
#define AM33XX_CLKACTIVITY_PRUSS_OCP_GCLK_MASK		(1 << 4)

/*                                */
#define AM33XX_CLKACTIVITY_PRUSS_UART_GCLK_SHIFT	6
#define AM33XX_CLKACTIVITY_PRUSS_UART_GCLK_WIDTH	1
#define AM33XX_CLKACTIVITY_PRUSS_UART_GCLK_MASK		(1 << 6)

/*                              */
#define AM33XX_CLKACTIVITY_L3S_GCLK_SHIFT		3
#define AM33XX_CLKACTIVITY_L3S_GCLK_WIDTH		1
#define AM33XX_CLKACTIVITY_L3S_GCLK_MASK		(1 << 3)

/*                             */
#define AM33XX_CLKACTIVITY_L3_AON_GCLK_SHIFT		3
#define AM33XX_CLKACTIVITY_L3_AON_GCLK_WIDTH		1
#define AM33XX_CLKACTIVITY_L3_AON_GCLK_MASK		(1 << 3)

/*                             */
#define AM33XX_CLKACTIVITY_L3_GCLK_SHIFT		4
#define AM33XX_CLKACTIVITY_L3_GCLK_WIDTH		1
#define AM33XX_CLKACTIVITY_L3_GCLK_MASK			(1 << 4)

/*                               */
#define AM33XX_CLKACTIVITY_L4FW_GCLK_SHIFT		8
#define AM33XX_CLKACTIVITY_L4FW_GCLK_WIDTH		1
#define AM33XX_CLKACTIVITY_L4FW_GCLK_MASK		(1 << 8)

/*                               */
#define AM33XX_CLKACTIVITY_L4HS_GCLK_SHIFT		3
#define AM33XX_CLKACTIVITY_L4HS_GCLK_WIDTH		1
#define AM33XX_CLKACTIVITY_L4HS_GCLK_MASK		(1 << 3)

/*                               */
#define AM33XX_CLKACTIVITY_L4LS_GCLK_SHIFT		8
#define AM33XX_CLKACTIVITY_L4LS_GCLK_WIDTH		1
#define AM33XX_CLKACTIVITY_L4LS_GCLK_MASK		(1 << 8)

/*                                      */
#define AM33XX_CLKACTIVITY_L4LS_GFX_GCLK_SHIFT		8
#define AM33XX_CLKACTIVITY_L4LS_GFX_GCLK_WIDTH		1
#define AM33XX_CLKACTIVITY_L4LS_GFX_GCLK_MASK		(1 << 8)

/*                             */
#define AM33XX_CLKACTIVITY_L4_CEFUSE_GICLK_SHIFT	8
#define AM33XX_CLKACTIVITY_L4_CEFUSE_GICLK_WIDTH	1
#define AM33XX_CLKACTIVITY_L4_CEFUSE_GICLK_MASK		(1 << 8)

/*                          */
#define AM33XX_CLKACTIVITY_L4_RTC_GCLK_SHIFT		8
#define AM33XX_CLKACTIVITY_L4_RTC_GCLK_WIDTH		1
#define AM33XX_CLKACTIVITY_L4_RTC_GCLK_MASK		(1 << 8)

/*                                  */
#define AM33XX_CLKACTIVITY_L4_WKUP_AON_GCLK_SHIFT	2
#define AM33XX_CLKACTIVITY_L4_WKUP_AON_GCLK_WIDTH	1
#define AM33XX_CLKACTIVITY_L4_WKUP_AON_GCLK_MASK	(1 << 2)

/*                           */
#define AM33XX_CLKACTIVITY_L4_WKUP_GCLK_SHIFT		2
#define AM33XX_CLKACTIVITY_L4_WKUP_GCLK_WIDTH		1
#define AM33XX_CLKACTIVITY_L4_WKUP_GCLK_MASK		(1 << 2)

/*                               */
#define AM33XX_CLKACTIVITY_LCDC_GCLK_SHIFT		17
#define AM33XX_CLKACTIVITY_LCDC_GCLK_WIDTH		1
#define AM33XX_CLKACTIVITY_LCDC_GCLK_MASK		(1 << 17)

/*                               */
#define AM33XX_CLKACTIVITY_LCDC_L3_OCP_GCLK_SHIFT	4
#define AM33XX_CLKACTIVITY_LCDC_L3_OCP_GCLK_WIDTH	1
#define AM33XX_CLKACTIVITY_LCDC_L3_OCP_GCLK_MASK	(1 << 4)

/*                               */
#define AM33XX_CLKACTIVITY_LCDC_L4_OCP_GCLK_SHIFT	5
#define AM33XX_CLKACTIVITY_LCDC_L4_OCP_GCLK_WIDTH	1
#define AM33XX_CLKACTIVITY_LCDC_L4_OCP_GCLK_MASK	(1 << 5)

/*                             */
#define AM33XX_CLKACTIVITY_MCASP_GCLK_SHIFT		7
#define AM33XX_CLKACTIVITY_MCASP_GCLK_WIDTH		1
#define AM33XX_CLKACTIVITY_MCASP_GCLK_MASK		(1 << 7)

/*                             */
#define AM33XX_CLKACTIVITY_MMC_FCLK_SHIFT		3
#define AM33XX_CLKACTIVITY_MMC_FCLK_WIDTH		1
#define AM33XX_CLKACTIVITY_MMC_FCLK_MASK		(1 << 3)

/*                          */
#define AM33XX_CLKACTIVITY_MPU_CLK_SHIFT		2
#define AM33XX_CLKACTIVITY_MPU_CLK_WIDTH		1
#define AM33XX_CLKACTIVITY_MPU_CLK_MASK			(1 << 2)

/*                                   */
#define AM33XX_CLKACTIVITY_OCPWP_L3_GCLK_SHIFT		4
#define AM33XX_CLKACTIVITY_OCPWP_L3_GCLK_WIDTH		1
#define AM33XX_CLKACTIVITY_OCPWP_L3_GCLK_MASK		(1 << 4)

/*                                   */
#define AM33XX_CLKACTIVITY_OCPWP_L4_GCLK_SHIFT		5
#define AM33XX_CLKACTIVITY_OCPWP_L4_GCLK_WIDTH		1
#define AM33XX_CLKACTIVITY_OCPWP_L4_GCLK_MASK		(1 << 5)

/*                          */
#define AM33XX_CLKACTIVITY_RTC_32KCLK_SHIFT		9
#define AM33XX_CLKACTIVITY_RTC_32KCLK_WIDTH		1
#define AM33XX_CLKACTIVITY_RTC_32KCLK_MASK		(1 << 9)

/*                               */
#define AM33XX_CLKACTIVITY_SPI_GCLK_SHIFT		25
#define AM33XX_CLKACTIVITY_SPI_GCLK_WIDTH		1
#define AM33XX_CLKACTIVITY_SPI_GCLK_MASK		(1 << 25)

/*                           */
#define AM33XX_CLKACTIVITY_SR_SYSCLK_SHIFT		3
#define AM33XX_CLKACTIVITY_SR_SYSCLK_WIDTH		1
#define AM33XX_CLKACTIVITY_SR_SYSCLK_MASK		(1 << 3)

/*                           */
#define AM33XX_CLKACTIVITY_TIMER0_GCLK_SHIFT		10
#define AM33XX_CLKACTIVITY_TIMER0_GCLK_WIDTH		1
#define AM33XX_CLKACTIVITY_TIMER0_GCLK_MASK		(1 << 10)

/*                           */
#define AM33XX_CLKACTIVITY_TIMER1_GCLK_SHIFT		13
#define AM33XX_CLKACTIVITY_TIMER1_GCLK_WIDTH		1
#define AM33XX_CLKACTIVITY_TIMER1_GCLK_MASK		(1 << 13)

/*                               */
#define AM33XX_CLKACTIVITY_TIMER2_GCLK_SHIFT		14
#define AM33XX_CLKACTIVITY_TIMER2_GCLK_WIDTH		1
#define AM33XX_CLKACTIVITY_TIMER2_GCLK_MASK		(1 << 14)

/*                               */
#define AM33XX_CLKACTIVITY_TIMER3_GCLK_SHIFT		15
#define AM33XX_CLKACTIVITY_TIMER3_GCLK_WIDTH		1
#define AM33XX_CLKACTIVITY_TIMER3_GCLK_MASK		(1 << 15)

/*                               */
#define AM33XX_CLKACTIVITY_TIMER4_GCLK_SHIFT		16
#define AM33XX_CLKACTIVITY_TIMER4_GCLK_WIDTH		1
#define AM33XX_CLKACTIVITY_TIMER4_GCLK_MASK		(1 << 16)

/*                               */
#define AM33XX_CLKACTIVITY_TIMER5_GCLK_SHIFT		27
#define AM33XX_CLKACTIVITY_TIMER5_GCLK_WIDTH		1
#define AM33XX_CLKACTIVITY_TIMER5_GCLK_MASK		(1 << 27)

/*                               */
#define AM33XX_CLKACTIVITY_TIMER6_GCLK_SHIFT		28
#define AM33XX_CLKACTIVITY_TIMER6_GCLK_WIDTH		1
#define AM33XX_CLKACTIVITY_TIMER6_GCLK_MASK		(1 << 28)

/*                               */
#define AM33XX_CLKACTIVITY_TIMER7_GCLK_SHIFT		13
#define AM33XX_CLKACTIVITY_TIMER7_GCLK_WIDTH		1
#define AM33XX_CLKACTIVITY_TIMER7_GCLK_MASK		(1 << 13)

/*                           */
#define AM33XX_CLKACTIVITY_UART0_GFCLK_SHIFT		12
#define AM33XX_CLKACTIVITY_UART0_GFCLK_WIDTH		1
#define AM33XX_CLKACTIVITY_UART0_GFCLK_MASK		(1 << 12)

/*                               */
#define AM33XX_CLKACTIVITY_UART_GFCLK_SHIFT		10
#define AM33XX_CLKACTIVITY_UART_GFCLK_WIDTH		1
#define AM33XX_CLKACTIVITY_UART_GFCLK_MASK		(1 << 10)

/*                           */
#define AM33XX_CLKACTIVITY_WDT0_GCLK_SHIFT		9
#define AM33XX_CLKACTIVITY_WDT0_GCLK_WIDTH		1
#define AM33XX_CLKACTIVITY_WDT0_GCLK_MASK		(1 << 9)

/*                           */
#define AM33XX_CLKACTIVITY_WDT1_GCLK_SHIFT		4
#define AM33XX_CLKACTIVITY_WDT1_GCLK_WIDTH		1
#define AM33XX_CLKACTIVITY_WDT1_GCLK_MASK		(1 << 4)

/*                         */
#define AM33XX_CLKDIV_SEL_GFX_FCLK_SHIFT		0
#define AM33XX_CLKDIV_SEL_GFX_FCLK_WIDTH		1
#define AM33XX_CLKDIV_SEL_GFX_FCLK_MASK			(1 << 0)

/*                        */
#define AM33XX_CLKOUT2DIV_SHIFT				3
#define AM33XX_CLKOUT2DIV_WIDTH				3
#define AM33XX_CLKOUT2DIV_MASK				(0x7 << 3)

/*                        */
#define AM33XX_CLKOUT2EN_SHIFT				7
#define AM33XX_CLKOUT2EN_WIDTH				1
#define AM33XX_CLKOUT2EN_MASK				(1 << 7)

/*                        */
#define AM33XX_CLKOUT2SOURCE_SHIFT			0
#define AM33XX_CLKOUT2SOURCE_WIDTH			3
#define AM33XX_CLKOUT2SOURCE_MASK			(0x7 << 0)

/*
                                                                        
                                                                              
                    
 */
#define AM33XX_CLKSEL_SHIFT				0
#define AM33XX_CLKSEL_WIDTH				1
#define AM33XX_CLKSEL_MASK				(0x01 << 0)

/*
                                                                     
                     
 */
#define AM33XX_CLKSEL_0_0_SHIFT				0
#define AM33XX_CLKSEL_0_0_WIDTH				1
#define AM33XX_CLKSEL_0_0_MASK				(1 << 0)

#define AM33XX_CLKSEL_0_1_SHIFT				0
#define AM33XX_CLKSEL_0_1_WIDTH				2
#define AM33XX_CLKSEL_0_1_MASK				(3 << 0)

/*                                                 */
#define AM33XX_CLKSEL_0_2_SHIFT				0
#define AM33XX_CLKSEL_0_2_WIDTH				3
#define AM33XX_CLKSEL_0_2_MASK				(7 << 0)

/*                         */
#define AM33XX_CLKSEL_GFX_FCLK_SHIFT			1
#define AM33XX_CLKSEL_GFX_FCLK_WIDTH			1
#define AM33XX_CLKSEL_GFX_FCLK_MASK			(1 << 1)

/*
                                                                          
                                                                       
                                                                     
                                                                           
                                                                    
                                                                         
 */
#define AM33XX_CLKTRCTRL_SHIFT				0
#define AM33XX_CLKTRCTRL_WIDTH				2
#define AM33XX_CLKTRCTRL_MASK				(0x3 << 0)

/*
                                                                   
                                                           
                             
 */
#define AM33XX_DELTAMSTEP_SHIFT				0
#define AM33XX_DELTAMSTEP_WIDTH				20
#define AM33XX_DELTAMSTEP_MASK				(0xfffff << 0)

/*                                                                     */
#define AM33XX_DPLL_BYP_CLKSEL_SHIFT			23
#define AM33XX_DPLL_BYP_CLKSEL_WIDTH			1
#define AM33XX_DPLL_BYP_CLKSEL_MASK			(1 << 23)

/*                               */
#define AM33XX_DPLL_CLKDCOLDO_GATE_CTRL_SHIFT		8
#define AM33XX_DPLL_CLKDCOLDO_GATE_CTRL_WIDTH		1
#define AM33XX_DPLL_CLKDCOLDO_GATE_CTRL_MASK		(1 << 8)

/*                               */
#define AM33XX_DPLL_CLKDCOLDO_PWDN_SHIFT		12
#define AM33XX_DPLL_CLKDCOLDO_PWDN_WIDTH		1
#define AM33XX_DPLL_CLKDCOLDO_PWDN_MASK			(1 << 12)

/*                                                                     */
#define AM33XX_DPLL_CLKOUT_DIV_SHIFT			0
#define AM33XX_DPLL_CLKOUT_DIV_WIDTH			5
#define AM33XX_DPLL_CLKOUT_DIV_MASK			(0x1f << 0)

/*                                                         */
#define AM33XX_DPLL_CLKOUT_DIV_0_6_SHIFT		0
#define AM33XX_DPLL_CLKOUT_DIV_0_6_WIDTH		7
#define AM33XX_DPLL_CLKOUT_DIV_0_6_MASK			(0x7f << 0)

/*                                                                     */
#define AM33XX_DPLL_CLKOUT_DIVCHACK_SHIFT		5
#define AM33XX_DPLL_CLKOUT_DIVCHACK_WIDTH		1
#define AM33XX_DPLL_CLKOUT_DIVCHACK_MASK		(1 << 5)

/*                                                              */
#define AM33XX_DPLL_CLKOUT_DIVCHACK_M2_PER_SHIFT	7
#define AM33XX_DPLL_CLKOUT_DIVCHACK_M2_PER_WIDTH	1
#define AM33XX_DPLL_CLKOUT_DIVCHACK_M2_PER_MASK		(1 << 7)

/*
                                                                       
                     
 */
#define AM33XX_DPLL_CLKOUT_GATE_CTRL_SHIFT		8
#define AM33XX_DPLL_CLKOUT_GATE_CTRL_WIDTH		1
#define AM33XX_DPLL_CLKOUT_GATE_CTRL_MASK		(1 << 8)

/*
                                                                        
                     
 */
#define AM33XX_DPLL_DIV_SHIFT				0
#define AM33XX_DPLL_DIV_WIDTH				7
#define AM33XX_DPLL_DIV_MASK				(0x7f << 0)

#define AM33XX_DPLL_PER_DIV_MASK			(0xff << 0)

/*                                                     */
#define AM33XX_DPLL_DIV_0_7_SHIFT			0
#define AM33XX_DPLL_DIV_0_7_WIDTH			8
#define AM33XX_DPLL_DIV_0_7_MASK			(0xff << 0)

/*
                                                                           
                      
 */
#define AM33XX_DPLL_DRIFTGUARD_EN_SHIFT			8
#define AM33XX_DPLL_DRIFTGUARD_EN_WIDTH			1
#define AM33XX_DPLL_DRIFTGUARD_EN_MASK			(1 << 8)

/*
                                                                           
                                           
 */
#define AM33XX_DPLL_EN_SHIFT				0
#define AM33XX_DPLL_EN_WIDTH				3
#define AM33XX_DPLL_EN_MASK				(0x7 << 0)

/*
                                                                           
                      
 */
#define AM33XX_DPLL_LPMODE_EN_SHIFT			10
#define AM33XX_DPLL_LPMODE_EN_WIDTH			1
#define AM33XX_DPLL_LPMODE_EN_MASK			(1 << 10)

/*
                                                                        
                     
 */
#define AM33XX_DPLL_MULT_SHIFT				8
#define AM33XX_DPLL_MULT_WIDTH				11
#define AM33XX_DPLL_MULT_MASK				(0x7ff << 8)

/*                                                      */
#define AM33XX_DPLL_MULT_PERIPH_SHIFT			8
#define AM33XX_DPLL_MULT_PERIPH_WIDTH			12
#define AM33XX_DPLL_MULT_PERIPH_MASK			(0xfff << 8)

/*
                                                                           
                      
 */
#define AM33XX_DPLL_REGM4XEN_SHIFT			11
#define AM33XX_DPLL_REGM4XEN_WIDTH			1
#define AM33XX_DPLL_REGM4XEN_MASK			(1 << 11)

/*                               */
#define AM33XX_DPLL_SD_DIV_SHIFT			24
#define AM33XX_DPLL_SD_DIV_WIDTH			8
#define AM33XX_DPLL_SD_DIV_MASK				(0xff << 24)

/*
                                                                           
                                           
 */
#define AM33XX_DPLL_SSC_ACK_SHIFT			13
#define AM33XX_DPLL_SSC_ACK_WIDTH			1
#define AM33XX_DPLL_SSC_ACK_MASK			(1 << 13)

/*
                                                                           
                                           
 */
#define AM33XX_DPLL_SSC_DOWNSPREAD_SHIFT		14
#define AM33XX_DPLL_SSC_DOWNSPREAD_WIDTH		1
#define AM33XX_DPLL_SSC_DOWNSPREAD_MASK			(1 << 14)

/*
                                                                           
                                           
 */
#define AM33XX_DPLL_SSC_EN_SHIFT			12
#define AM33XX_DPLL_SSC_EN_WIDTH			1
#define AM33XX_DPLL_SSC_EN_MASK				(1 << 12)

/*                             */
#define AM33XX_HSDIVIDER_CLKOUT1_DIV_SHIFT		0
#define AM33XX_HSDIVIDER_CLKOUT1_DIV_WIDTH		5
#define AM33XX_HSDIVIDER_CLKOUT1_DIV_MASK		(0x1f << 0)

/*                             */
#define AM33XX_HSDIVIDER_CLKOUT1_DIVCHACK_SHIFT		5
#define AM33XX_HSDIVIDER_CLKOUT1_DIVCHACK_WIDTH		1
#define AM33XX_HSDIVIDER_CLKOUT1_DIVCHACK_MASK		(1 << 5)

/*                             */
#define AM33XX_HSDIVIDER_CLKOUT1_GATE_CTRL_SHIFT	8
#define AM33XX_HSDIVIDER_CLKOUT1_GATE_CTRL_WIDTH	1
#define AM33XX_HSDIVIDER_CLKOUT1_GATE_CTRL_MASK		(1 << 8)

/*                             */
#define AM33XX_HSDIVIDER_CLKOUT1_PWDN_SHIFT		12
#define AM33XX_HSDIVIDER_CLKOUT1_PWDN_WIDTH		1
#define AM33XX_HSDIVIDER_CLKOUT1_PWDN_MASK		(1 << 12)

/*                             */
#define AM33XX_HSDIVIDER_CLKOUT2_DIV_SHIFT		0
#define AM33XX_HSDIVIDER_CLKOUT2_DIV_WIDTH		5
#define AM33XX_HSDIVIDER_CLKOUT2_DIV_MASK		(0x1f << 0)

/*                             */
#define AM33XX_HSDIVIDER_CLKOUT2_DIVCHACK_SHIFT		5
#define AM33XX_HSDIVIDER_CLKOUT2_DIVCHACK_WIDTH		1
#define AM33XX_HSDIVIDER_CLKOUT2_DIVCHACK_MASK		(1 << 5)

/*                             */
#define AM33XX_HSDIVIDER_CLKOUT2_GATE_CTRL_SHIFT	8
#define AM33XX_HSDIVIDER_CLKOUT2_GATE_CTRL_WIDTH	1
#define AM33XX_HSDIVIDER_CLKOUT2_GATE_CTRL_MASK		(1 << 8)

/*                             */
#define AM33XX_HSDIVIDER_CLKOUT2_PWDN_SHIFT		12
#define AM33XX_HSDIVIDER_CLKOUT2_PWDN_WIDTH		1
#define AM33XX_HSDIVIDER_CLKOUT2_PWDN_MASK		(1 << 12)

/*                             */
#define AM33XX_HSDIVIDER_CLKOUT3_DIV_SHIFT		0
#define AM33XX_HSDIVIDER_CLKOUT3_DIV_WIDTH		5
#define AM33XX_HSDIVIDER_CLKOUT3_DIV_MASK		(0x1f << 0)

/*                             */
#define AM33XX_HSDIVIDER_CLKOUT3_DIVCHACK_SHIFT		5
#define AM33XX_HSDIVIDER_CLKOUT3_DIVCHACK_WIDTH		1
#define AM33XX_HSDIVIDER_CLKOUT3_DIVCHACK_MASK		(1 << 5)

/*                             */
#define AM33XX_HSDIVIDER_CLKOUT3_GATE_CTRL_SHIFT	8
#define AM33XX_HSDIVIDER_CLKOUT3_GATE_CTRL_WIDTH	1
#define AM33XX_HSDIVIDER_CLKOUT3_GATE_CTRL_MASK		(1 << 8)

/*                             */
#define AM33XX_HSDIVIDER_CLKOUT3_PWDN_SHIFT		12
#define AM33XX_HSDIVIDER_CLKOUT3_PWDN_WIDTH		1
#define AM33XX_HSDIVIDER_CLKOUT3_PWDN_MASK		(1 << 12)

/*
                                                                       
                                                                         
                                                                  
                                                                   
                                                                          
                                                                    
                                                                    
                                                                 
                                                                    
                                                                     
                                                                     
                                                                         
                                                                  
                                                                      
                                                                     
                                                               
                                                                         
                                                                  
                                                                     
                                                                       
                                                                       
                                                                   
                                                                    
                                                                    
                                                                         
                                                                        
                                                        
                                                        
                                                                       
                                                                   
                                                                          
 */
#define AM33XX_IDLEST_SHIFT				16
#define AM33XX_IDLEST_WIDTH				2
#define AM33XX_IDLEST_MASK				(0x3 << 16)

/*                       */
#define AM33XX_MII_CLK_SEL_SHIFT			2
#define AM33XX_MII_CLK_SEL_WIDTH			1
#define AM33XX_MII_CLK_SEL_MASK				(1 << 2)

/*
                                                                   
                                                           
                             
 */
#define AM33XX_MODFREQDIV_EXPONENT_SHIFT		8
#define AM33XX_MODFREQDIV_EXPONENT_WIDTH		3
#define AM33XX_MODFREQDIV_EXPONENT_MASK			(0x7 << 8)

/*
                                                                   
                                                           
                             
 */
#define AM33XX_MODFREQDIV_MANTISSA_SHIFT		0
#define AM33XX_MODFREQDIV_MANTISSA_WIDTH		7
#define AM33XX_MODFREQDIV_MANTISSA_MASK			(0x7f << 0)

/*
                                                                       
                                                                         
                                                                  
                                                                   
                                                                          
                                                                    
                                                                    
                                                                 
                                                                    
                                                                     
                                                                     
                                                                         
                                                                  
                                                                      
                                                                     
                                                               
                                                                         
                                                                  
                                                                     
                                                                       
                                                                       
                                                                   
                                                                    
                                                                    
                                                                         
                                                                        
                                                        
                                                        
                                                                       
                                                                        
                                                                     
                           
 */
#define AM33XX_MODULEMODE_SHIFT				0
#define AM33XX_MODULEMODE_WIDTH				2
#define AM33XX_MODULEMODE_MASK				(0x3 << 0)

/*                                 */
#define AM33XX_OPTCLK_DEBUG_CLKA_SHIFT			30
#define AM33XX_OPTCLK_DEBUG_CLKA_WIDTH			1
#define AM33XX_OPTCLK_DEBUG_CLKA_MASK			(1 << 30)

/*                                 */
#define AM33XX_OPTFCLKEN_DBGSYSCLK_SHIFT		19
#define AM33XX_OPTFCLKEN_DBGSYSCLK_WIDTH		1
#define AM33XX_OPTFCLKEN_DBGSYSCLK_MASK			(1 << 19)

/*                               */
#define AM33XX_OPTFCLKEN_GPIO0_GDBCLK_SHIFT		18
#define AM33XX_OPTFCLKEN_GPIO0_GDBCLK_WIDTH		1
#define AM33XX_OPTFCLKEN_GPIO0_GDBCLK_MASK		(1 << 18)

/*                              */
#define AM33XX_OPTFCLKEN_GPIO_1_GDBCLK_SHIFT		18
#define AM33XX_OPTFCLKEN_GPIO_1_GDBCLK_WIDTH		1
#define AM33XX_OPTFCLKEN_GPIO_1_GDBCLK_MASK		(1 << 18)

/*                              */
#define AM33XX_OPTFCLKEN_GPIO_2_GDBCLK_SHIFT		18
#define AM33XX_OPTFCLKEN_GPIO_2_GDBCLK_WIDTH		1
#define AM33XX_OPTFCLKEN_GPIO_2_GDBCLK_MASK		(1 << 18)

/*                              */
#define AM33XX_OPTFCLKEN_GPIO_3_GDBCLK_SHIFT		18
#define AM33XX_OPTFCLKEN_GPIO_3_GDBCLK_WIDTH		1
#define AM33XX_OPTFCLKEN_GPIO_3_GDBCLK_MASK		(1 << 18)

/*                              */
#define AM33XX_OPTFCLKEN_GPIO_4_GDBCLK_SHIFT		18
#define AM33XX_OPTFCLKEN_GPIO_4_GDBCLK_WIDTH		1
#define AM33XX_OPTFCLKEN_GPIO_4_GDBCLK_MASK		(1 << 18)

/*                              */
#define AM33XX_OPTFCLKEN_GPIO_5_GDBCLK_SHIFT		18
#define AM33XX_OPTFCLKEN_GPIO_5_GDBCLK_WIDTH		1
#define AM33XX_OPTFCLKEN_GPIO_5_GDBCLK_MASK		(1 << 18)

/*                              */
#define AM33XX_OPTFCLKEN_GPIO_6_GDBCLK_SHIFT		18
#define AM33XX_OPTFCLKEN_GPIO_6_GDBCLK_WIDTH		1
#define AM33XX_OPTFCLKEN_GPIO_6_GDBCLK_MASK		(1 << 18)

/*
                                                                            
                                                                    
                                                                       
                                                                    
                                                                      
                                                                     
 */
#define AM33XX_STBYST_SHIFT				18
#define AM33XX_STBYST_WIDTH				1
#define AM33XX_STBYST_MASK				(1 << 18)

/*                                 */
#define AM33XX_STM_PMD_CLKDIVSEL_SHIFT			27
#define AM33XX_STM_PMD_CLKDIVSEL_WIDTH			3
#define AM33XX_STM_PMD_CLKDIVSEL_MASK			(0x7 << 27)

/*                                 */
#define AM33XX_STM_PMD_CLKSEL_SHIFT			22
#define AM33XX_STM_PMD_CLKSEL_WIDTH			2
#define AM33XX_STM_PMD_CLKSEL_MASK			(0x3 << 22)

/*
                                                                        
                                         
 */
#define AM33XX_ST_DPLL_CLK_SHIFT			0
#define AM33XX_ST_DPLL_CLK_WIDTH			1
#define AM33XX_ST_DPLL_CLK_MASK				(1 << 0)

/*                               */
#define AM33XX_ST_DPLL_CLKDCOLDO_SHIFT			8
#define AM33XX_ST_DPLL_CLKDCOLDO_WIDTH			1
#define AM33XX_ST_DPLL_CLKDCOLDO_MASK			(1 << 8)

/*
                                                                       
                     
 */
#define AM33XX_ST_DPLL_CLKOUT_SHIFT			9
#define AM33XX_ST_DPLL_CLKOUT_WIDTH			1
#define AM33XX_ST_DPLL_CLKOUT_MASK			(1 << 9)

/*                             */
#define AM33XX_ST_HSDIVIDER_CLKOUT1_SHIFT		9
#define AM33XX_ST_HSDIVIDER_CLKOUT1_WIDTH		1
#define AM33XX_ST_HSDIVIDER_CLKOUT1_MASK		(1 << 9)

/*                             */
#define AM33XX_ST_HSDIVIDER_CLKOUT2_SHIFT		9
#define AM33XX_ST_HSDIVIDER_CLKOUT2_WIDTH		1
#define AM33XX_ST_HSDIVIDER_CLKOUT2_MASK		(1 << 9)

/*                             */
#define AM33XX_ST_HSDIVIDER_CLKOUT3_SHIFT		9
#define AM33XX_ST_HSDIVIDER_CLKOUT3_WIDTH		1
#define AM33XX_ST_HSDIVIDER_CLKOUT3_MASK		(1 << 9)

/*
                                                                        
                                         
 */
#define AM33XX_ST_MN_BYPASS_SHIFT			8
#define AM33XX_ST_MN_BYPASS_WIDTH			1
#define AM33XX_ST_MN_BYPASS_MASK			(1 << 8)

/*                                 */
#define AM33XX_TRC_PMD_CLKDIVSEL_SHIFT			24
#define AM33XX_TRC_PMD_CLKDIVSEL_WIDTH			3
#define AM33XX_TRC_PMD_CLKDIVSEL_MASK			(0x7 << 24)

/*                                 */
#define AM33XX_TRC_PMD_CLKSEL_SHIFT			20
#define AM33XX_TRC_PMD_CLKSEL_WIDTH			2
#define AM33XX_TRC_PMD_CLKSEL_MASK			(0x3 << 20)

/*                              */
#define AM33XX_TIMER0_CLKSEL_WIDTH			2
#define AM33XX_TIMER0_CLKSEL_MASK			(0x3 << 4)
#endif