/*******************************************************************************

  Header file for stmmac platform data

  Copyright (C) 2009  STMicroelectronics Ltd

  This program is free software; you can redistribute it and/or modify it
  under the terms and conditions of the GNU General Public License,
  version 2, as published by the Free Software Foundation.

  This program is distributed in the hope it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

  The full GNU General Public License is included in this distribution in
  the file called "COPYING".

  Author: Giuseppe Cavallaro <peppe.cavallaro@st.com>
*******************************************************************************/

#ifndef __STMMAC_PLATFORM_DATA
#define __STMMAC_PLATFORM_DATA

#include <linux/platform_device.h>

#define STMMAC_RX_COE_NONE	0
#define STMMAC_RX_COE_TYPE1	1
#define STMMAC_RX_COE_TYPE2	2

/*                                                                 
                 
                                                                       */

/*                           */
#define	STMMAC_CSR_60_100M	0x0	/*                    */
#define	STMMAC_CSR_100_150M	0x1	/*                    */
#define	STMMAC_CSR_20_35M	0x2	/*                    */
#define	STMMAC_CSR_35_60M	0x3	/*                    */
#define	STMMAC_CSR_150_250M	0x4	/*                     */
#define	STMMAC_CSR_250_300M	0x5	/*                     */

/*                                                      
                                                                       
                                                                       
                                                                      
                                
                                                                       
                                                         
 */
#define STMMAC_CSR_I_4		0x8	/*             */
#define STMMAC_CSR_I_6		0x9	/*             */
#define STMMAC_CSR_I_8		0xA	/*             */
#define STMMAC_CSR_I_10		0xB	/*              */
#define STMMAC_CSR_I_12		0xC	/*              */
#define STMMAC_CSR_I_14		0xD	/*              */
#define STMMAC_CSR_I_16		0xE	/*              */
#define STMMAC_CSR_I_18		0xF	/*              */

/*                                */
#define DMA_AXI_BLEN_4		(1 << 1)
#define DMA_AXI_BLEN_8		(1 << 2)
#define DMA_AXI_BLEN_16		(1 << 3)
#define DMA_AXI_BLEN_32		(1 << 4)
#define DMA_AXI_BLEN_64		(1 << 5)
#define DMA_AXI_BLEN_128	(1 << 6)
#define DMA_AXI_BLEN_256	(1 << 7)
#define DMA_AXI_BLEN_ALL (DMA_AXI_BLEN_4 | DMA_AXI_BLEN_8 | DMA_AXI_BLEN_16 \
			| DMA_AXI_BLEN_32 | DMA_AXI_BLEN_64 \
			| DMA_AXI_BLEN_128 | DMA_AXI_BLEN_256)

/*                                                                   */

struct stmmac_mdio_bus_data {
	int (*phy_reset)(void *priv);
	unsigned int phy_mask;
	int *irqs;
	int probed_phy_irq;
};

struct stmmac_dma_cfg {
	int pbl;
	int fixed_burst;
	int mixed_burst;
	int burst_len;
};

struct plat_stmmacenet_data {
	char *phy_bus_name;
	int bus_id;
	int phy_addr;
	int interface;
	struct stmmac_mdio_bus_data *mdio_bus_data;
	struct stmmac_dma_cfg *dma_cfg;
	int clk_csr;
	int has_gmac;
	int enh_desc;
	int tx_coe;
	int rx_coe;
	int bugged_jumbo;
	int pmt;
	int force_sf_dma_mode;
	int riwt_off;
	void (*fix_mac_speed)(void *priv, unsigned int speed);
	void (*bus_setup)(void __iomem *ioaddr);
	int (*init)(struct platform_device *pdev);
	void (*exit)(struct platform_device *pdev);
	void *custom_cfg;
	void *custom_data;
	void *bsp_priv;
};
#endif
