/*
 * General-Purpose Memory Controller for OMAP2
 *
 * Copyright (C) 2005-2006 Nokia Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __OMAP2_GPMC_H
#define __OMAP2_GPMC_H

#include <linux/platform_data/mtd-nand-omap2.h>

/*                                */
#define GPMC_CS_NUM		8

#define GPMC_CS_CONFIG1		0x00
#define GPMC_CS_CONFIG2		0x04
#define GPMC_CS_CONFIG3		0x08
#define GPMC_CS_CONFIG4		0x0c
#define GPMC_CS_CONFIG5		0x10
#define GPMC_CS_CONFIG6		0x14
#define GPMC_CS_CONFIG7		0x18
#define GPMC_CS_NAND_COMMAND	0x1c
#define GPMC_CS_NAND_ADDRESS	0x20
#define GPMC_CS_NAND_DATA	0x24

/*                  */
#define GPMC_CONFIG_RDY_BSY	0x00000001
#define GPMC_CONFIG_DEV_SIZE	0x00000002
#define GPMC_CONFIG_DEV_TYPE	0x00000003
#define GPMC_SET_IRQ_STATUS	0x00000004
#define GPMC_CONFIG_WP		0x00000005

#define GPMC_ENABLE_IRQ		0x0000000d

/*              */
#define GPMC_ECC_READ		0 /*                             */
#define GPMC_ECC_WRITE		1 /*                              */
#define GPMC_ECC_READSYN	2 /*                                   */

#define GPMC_CONFIG1_WRAPBURST_SUPP     (1 << 31)
#define GPMC_CONFIG1_READMULTIPLE_SUPP  (1 << 30)
#define GPMC_CONFIG1_READTYPE_ASYNC     (0 << 29)
#define GPMC_CONFIG1_READTYPE_SYNC      (1 << 29)
#define GPMC_CONFIG1_WRITEMULTIPLE_SUPP (1 << 28)
#define GPMC_CONFIG1_WRITETYPE_ASYNC    (0 << 27)
#define GPMC_CONFIG1_WRITETYPE_SYNC     (1 << 27)
#define GPMC_CONFIG1_CLKACTIVATIONTIME(val) ((val & 3) << 25)
#define GPMC_CONFIG1_PAGE_LEN(val)      ((val & 3) << 23)
#define GPMC_CONFIG1_WAIT_READ_MON      (1 << 22)
#define GPMC_CONFIG1_WAIT_WRITE_MON     (1 << 21)
#define GPMC_CONFIG1_WAIT_MON_IIME(val) ((val & 3) << 18)
#define GPMC_CONFIG1_WAIT_PIN_SEL(val)  ((val & 3) << 16)
#define GPMC_CONFIG1_DEVICESIZE(val)    ((val & 3) << 12)
#define GPMC_CONFIG1_DEVICESIZE_16      GPMC_CONFIG1_DEVICESIZE(1)
#define GPMC_CONFIG1_DEVICETYPE(val)    ((val & 3) << 10)
#define GPMC_CONFIG1_DEVICETYPE_NOR     GPMC_CONFIG1_DEVICETYPE(0)
#define GPMC_CONFIG1_MUXTYPE(val)       ((val & 3) << 8)
#define GPMC_CONFIG1_TIME_PARA_GRAN     (1 << 4)
#define GPMC_CONFIG1_FCLK_DIV(val)      (val & 3)
#define GPMC_CONFIG1_FCLK_DIV2          (GPMC_CONFIG1_FCLK_DIV(1))
#define GPMC_CONFIG1_FCLK_DIV3          (GPMC_CONFIG1_FCLK_DIV(2))
#define GPMC_CONFIG1_FCLK_DIV4          (GPMC_CONFIG1_FCLK_DIV(3))
#define GPMC_CONFIG7_CSVALID		(1 << 6)

#define GPMC_DEVICETYPE_NOR		0
#define GPMC_DEVICETYPE_NAND		2
#define GPMC_CONFIG_WRITEPROTECT	0x00000010
#define WR_RD_PIN_MONITORING		0x00600000
#define GPMC_IRQ_FIFOEVENTENABLE	0x01
#define GPMC_IRQ_COUNT_EVENT		0x02

#define GPMC_BURST_4			4	/*              */
#define GPMC_BURST_8			8	/*              */
#define GPMC_BURST_16			16	/*               */
#define GPMC_DEVWIDTH_8BIT		1	/*                    */
#define GPMC_DEVWIDTH_16BIT		2	/*                     */
#define GPMC_MUX_AAD			1	/*                          */
#define GPMC_MUX_AD			2	/*                     */

/*                         */
struct gpmc_bool_timings {
	bool cycle2cyclediffcsen;
	bool cycle2cyclesamecsen;
	bool we_extra_delay;
	bool oe_extra_delay;
	bool adv_extra_delay;
	bool cs_extra_delay;
	bool time_para_granularity;
};

/*
                                                                         
                                                                               
 */
struct gpmc_timings {
	/*                                                            */
	u32 sync_clk;

	/*                                                             */
	u32 cs_on;		/*                */
	u32 cs_rd_off;		/*                       */
	u32 cs_wr_off;		/*                        */

	/*                                                  */
	u32 adv_on;		/*                */
	u32 adv_rd_off;		/*                       */
	u32 adv_wr_off;		/*                        */

	/*                                                  */
	u32 we_on;		/*                   */
	u32 we_off;		/*                     */

	/*                                                  */
	u32 oe_on;		/*                   */
	u32 oe_off;		/*                     */

	/*                                                                  */
	u32 page_burst_access;	/*                            */
	u32 access;		/*                                       */
	u32 rd_cycle;		/*                       */
	u32 wr_cycle;		/*                        */

	u32 bus_turnaround;
	u32 cycle2cycle_delay;

	u32 wait_monitoring;
	u32 clk_activation;

	/*                                    */
	u32 wr_access;		/*              */
	u32 wr_data_mux_bus;	/*                  */

	struct gpmc_bool_timings bool_timings;
};

/*                               */
struct gpmc_device_timings {
	u32 t_ceasu;	/*                           */
	u32 t_avdasu;	/*                            */
	/*                                                  
                                                    
                                                         
                                                    
                                                      
                      
  */
	u32 t_avdp_r;	/*                                   */
	u32 t_avdp_w;
	u32 t_aavdh;	/*                   */
	u32 t_oeasu;	/*                           */
	u32 t_aa;	/*                                */
	u32 t_iaa;	/*                     */
	u32 t_oe;	/*                               */
	u32 t_ce;	/*                              */
	u32 t_rd_cycle;	/*                 */
	u32 t_cez_r;	/*                               */
	u32 t_cez_w;	/*                                */
	u32 t_oez;	/*                          */
	u32 t_weasu;	/*                           */
	u32 t_wpl;	/*                      */
	u32 t_wph;	/*                        */
	u32 t_wr_cycle;	/*                  */

	u32 clk;
	u32 t_bacc;	/*                                          */
	u32 t_ces;	/*                      */
	u32 t_avds;	/*                       */
	u32 t_avdh;	/*                        */
	u32 t_ach;	/*                            */
	u32 t_rdyo;	/*                    */

	u32 t_ce_rdyz;	/*                                          */
	u32 t_ce_avd;	/*                       */

	/*                                        
                               
  */
	u8 cyc_aavdh_oe;/*                                  */
	u8 cyc_aavdh_we;/*                                   */
	u8 cyc_oe;	/*                                         */
	u8 cyc_wpl;	/*                                  */
	u32 cyc_iaa;	/*                               */

	/*              */
	bool ce_xdelay;
	bool avd_xdelay;
	bool oe_xdelay;
	bool we_xdelay;
};

struct gpmc_settings {
	bool burst_wrap;	/*                       */
	bool burst_read;	/*                              */
	bool burst_write;	/*                               */
	bool device_nand;	/*                */
	bool sync_read;		/*                           */
	bool sync_write;	/*                            */
	bool wait_on_read;	/*                       */
	bool wait_on_write;	/*                        */
	u32 burst_len;		/*                   */
	u32 device_width;	/*                                */
	u32 mux_add_data;	/*                          */
	u32 wait_pin;		/*                     */
};

extern int gpmc_calc_timings(struct gpmc_timings *gpmc_t,
			     struct gpmc_settings *gpmc_s,
			     struct gpmc_device_timings *dev_t);

extern void gpmc_update_nand_reg(struct gpmc_nand_regs *reg, int cs);
extern int gpmc_get_client_irq(unsigned irq_config);

extern unsigned int gpmc_ticks_to_ns(unsigned int ticks);

extern void gpmc_cs_write_reg(int cs, int idx, u32 val);
extern int gpmc_calc_divider(unsigned int sync_clk);
extern int gpmc_cs_set_timings(int cs, const struct gpmc_timings *t);
extern int gpmc_cs_program_settings(int cs, struct gpmc_settings *p);
extern int gpmc_cs_request(int cs, unsigned long size, unsigned long *base);
extern void gpmc_cs_free(int cs);
extern void omap3_gpmc_save_context(void);
extern void omap3_gpmc_restore_context(void);
extern int gpmc_configure(int cmd, int wval);
extern void gpmc_read_settings_dt(struct device_node *np,
				  struct gpmc_settings *p);

#endif
