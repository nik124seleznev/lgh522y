/*
 * arch/arm/plat-omap/include/mach/nand.h
 *
 * Copyright (C) 2006 Micron Technology Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef	_MTD_NAND_OMAP2_H
#define	_MTD_NAND_OMAP2_H

#include <linux/mtd/partitions.h>

#define	GPMC_BCH_NUM_REMAINDER	8

enum nand_io {
	NAND_OMAP_PREFETCH_POLLED = 0,	/*                               */
	NAND_OMAP_POLLED,		/*                               */
	NAND_OMAP_PREFETCH_DMA,		/*                            */
	NAND_OMAP_PREFETCH_IRQ		/*                           */
};

enum omap_ecc {
		/*                                        */
	OMAP_ECC_HAMMING_CODE_DEFAULT = 0, /*                     */
	OMAP_ECC_HAMMING_CODE_HW, /*                          */
		/*                                                         */
	OMAP_ECC_HAMMING_CODE_HW_ROMCODE, /*                              */
	OMAP_ECC_BCH4_CODE_HW, /*                    */
	OMAP_ECC_BCH8_CODE_HW, /*                    */
};

struct gpmc_nand_regs {
	void __iomem	*gpmc_status;
	void __iomem	*gpmc_nand_command;
	void __iomem	*gpmc_nand_address;
	void __iomem	*gpmc_nand_data;
	void __iomem	*gpmc_prefetch_config1;
	void __iomem	*gpmc_prefetch_config2;
	void __iomem	*gpmc_prefetch_control;
	void __iomem	*gpmc_prefetch_status;
	void __iomem	*gpmc_ecc_config;
	void __iomem	*gpmc_ecc_control;
	void __iomem	*gpmc_ecc_size_config;
	void __iomem	*gpmc_ecc1_result;
	void __iomem	*gpmc_bch_result0[GPMC_BCH_NUM_REMAINDER];
	void __iomem	*gpmc_bch_result1[GPMC_BCH_NUM_REMAINDER];
	void __iomem	*gpmc_bch_result2[GPMC_BCH_NUM_REMAINDER];
	void __iomem	*gpmc_bch_result3[GPMC_BCH_NUM_REMAINDER];
};

struct omap_nand_platform_data {
	int			cs;
	struct mtd_partition	*parts;
	int			nr_parts;
	bool			dev_ready;
	enum nand_io		xfer_type;
	int			devsize;
	enum omap_ecc           ecc_opt;
	struct gpmc_nand_regs	reg;

	/*                            */
	struct device_node	*of_node;
};
#endif
