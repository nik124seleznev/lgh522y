/*******************************************************************************
  STMMAC Common Header File

  Copyright (C) 2007-2009  STMicroelectronics Ltd

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

#ifndef __COMMON_H__
#define __COMMON_H__

#include <linux/etherdevice.h>
#include <linux/netdevice.h>
#include <linux/phy.h>
#include <linux/module.h>
#include <linux/init.h>
#if defined(CONFIG_VLAN_8021Q) || defined(CONFIG_VLAN_8021Q_MODULE)
#define STMMAC_VLAN_TAG_USED
#include <linux/if_vlan.h>
#endif

#include "descs.h"
#include "mmc.h"

#undef CHIP_DEBUG_PRINT
/*                                                              */
/*                          */

#ifdef CHIP_DEBUG_PRINT
#define CHIP_DBG(fmt, args...)  printk(fmt, ## args)
#else
#define CHIP_DBG(fmt, args...)  do { } while (0)
#endif

/*                        */
#define	DWMAC_CORE_3_40	0x34
#define	DWMAC_CORE_3_50	0x35

#undef FRAME_FILTER_DEBUG
/*                            */

struct stmmac_extra_stats {
	/*                 */
	unsigned long tx_underflow ____cacheline_aligned;
	unsigned long tx_carrier;
	unsigned long tx_losscarrier;
	unsigned long vlan_tag;
	unsigned long tx_deferred;
	unsigned long tx_vlan;
	unsigned long tx_jabber;
	unsigned long tx_frame_flushed;
	unsigned long tx_payload_error;
	unsigned long tx_ip_header_error;
	/*                */
	unsigned long rx_desc;
	unsigned long sa_filter_fail;
	unsigned long overflow_error;
	unsigned long ipc_csum_error;
	unsigned long rx_collision;
	unsigned long rx_crc;
	unsigned long dribbling_bit;
	unsigned long rx_length;
	unsigned long rx_mii;
	unsigned long rx_multicast;
	unsigned long rx_gmac_overflow;
	unsigned long rx_watchdog;
	unsigned long da_rx_filter_fail;
	unsigned long sa_rx_filter_fail;
	unsigned long rx_missed_cntr;
	unsigned long rx_overflow_cntr;
	unsigned long rx_vlan;
	/*                      */
	unsigned long tx_undeflow_irq;
	unsigned long tx_process_stopped_irq;
	unsigned long tx_jabber_irq;
	unsigned long rx_overflow_irq;
	unsigned long rx_buf_unav_irq;
	unsigned long rx_process_stopped_irq;
	unsigned long rx_watchdog_irq;
	unsigned long tx_early_irq;
	unsigned long fatal_bus_error_irq;
	/*                  */
	unsigned long rx_early_irq;
	unsigned long threshold;
	unsigned long tx_pkt_n;
	unsigned long rx_pkt_n;
	unsigned long normal_irq_n;
	unsigned long rx_normal_irq_n;
	unsigned long napi_poll;
	unsigned long tx_normal_irq_n;
	unsigned long tx_clean;
	unsigned long tx_reset_ic_bit;
	unsigned long irq_receive_pmt_irq_n;
	/*          */
	unsigned long mmc_tx_irq_n;
	unsigned long mmc_rx_irq_n;
	unsigned long mmc_rx_csum_offload_irq_n;
	/*     */
	unsigned long irq_tx_path_in_lpi_mode_n;
	unsigned long irq_tx_path_exit_lpi_mode_n;
	unsigned long irq_rx_path_in_lpi_mode_n;
	unsigned long irq_rx_path_exit_lpi_mode_n;
	unsigned long phy_eee_wakeup_error_n;
	/*                      */
	unsigned long ip_hdr_err;
	unsigned long ip_payload_err;
	unsigned long ip_csum_bypassed;
	unsigned long ipv4_pkt_rcvd;
	unsigned long ipv6_pkt_rcvd;
	unsigned long rx_msg_type_ext_no_ptp;
	unsigned long rx_msg_type_sync;
	unsigned long rx_msg_type_follow_up;
	unsigned long rx_msg_type_delay_req;
	unsigned long rx_msg_type_delay_resp;
	unsigned long rx_msg_type_pdelay_req;
	unsigned long rx_msg_type_pdelay_resp;
	unsigned long rx_msg_type_pdelay_follow_up;
	unsigned long ptp_frame_type;
	unsigned long ptp_ver;
	unsigned long timestamp_dropped;
	unsigned long av_pkt_rcvd;
	unsigned long av_tagged_pkt_rcvd;
	unsigned long vlan_tag_priority_val;
	unsigned long l3_filter_match;
	unsigned long l4_filter_match;
	unsigned long l3_l4_filter_no_match;
	/*     */
	unsigned long irq_pcs_ane_n;
	unsigned long irq_pcs_link_n;
	unsigned long irq_rgmii_n;
	unsigned long pcs_link;
	unsigned long pcs_duplex;
	unsigned long pcs_speed;
};

/*                             */
#define CSR_F_35M	35000000
#define CSR_F_60M	60000000
#define CSR_F_100M	100000000
#define CSR_F_150M	150000000
#define CSR_F_250M	250000000
#define CSR_F_300M	300000000

#define	MAC_CSR_H_FRQ_MASK	0x20

#define HASH_TABLE_SIZE 64
#define PAUSE_TIME 0x200

/*                      */
#define FLOW_OFF	0
#define FLOW_RX		1
#define FLOW_TX		2
#define FLOW_AUTO	(FLOW_TX | FLOW_RX)

/*             */
#define STMMAC_PCS_RGMII	(1 << 0)
#define STMMAC_PCS_SGMII	(1 << 1)
#define STMMAC_PCS_TBI		(1 << 2)
#define STMMAC_PCS_RTBI		(1 << 3)

#define SF_DMA_MODE 1		/*                                      */

/*                                */
#define DMA_HW_FEAT_MIISEL	0x00000001	/*                     */
#define DMA_HW_FEAT_GMIISEL	0x00000002	/*                   */
#define DMA_HW_FEAT_HDSEL	0x00000004	/*                     */
#define DMA_HW_FEAT_EXTHASHEN	0x00000008	/*                         */
#define DMA_HW_FEAT_HASHSEL	0x00000010	/*             */
#define DMA_HW_FEAT_ADDMAC	0x00000020	/*                       */
#define DMA_HW_FEAT_PCSSEL	0x00000040	/*               */
#define DMA_HW_FEAT_L3L4FLTREN	0x00000080	/*                           */
#define DMA_HW_FEAT_SMASEL	0x00000100	/*                     */
#define DMA_HW_FEAT_RWKSEL	0x00000200	/*                   */
#define DMA_HW_FEAT_MGKSEL	0x00000400	/*                  */
#define DMA_HW_FEAT_MMCSEL	0x00000800	/*             */
#define DMA_HW_FEAT_TSVER1SEL	0x00001000	/*                     */
#define DMA_HW_FEAT_TSVER2SEL	0x00002000	/*                      */
#define DMA_HW_FEAT_EEESEL	0x00004000	/*                           */
#define DMA_HW_FEAT_AVSEL	0x00008000	/*            */
#define DMA_HW_FEAT_TXCOESEL	0x00010000	/*                        */
#define DMA_HW_FEAT_RXTYP1COE	0x00020000	/*                       */
#define DMA_HW_FEAT_RXTYP2COE	0x00040000	/*                       */
#define DMA_HW_FEAT_RXFIFOSIZE	0x00080000	/*                      */
#define DMA_HW_FEAT_RXCHCNT	0x00300000	/*                            */
#define DMA_HW_FEAT_TXCHCNT	0x00c00000	/*                            */
#define DMA_HW_FEAT_ENHDESSEL	0x01000000	/*                      */
/*                                        */
#define DMA_HW_FEAT_INTTSEN	0x02000000
#define DMA_HW_FEAT_FLEXIPPSEN	0x04000000	/*                     */
#define DMA_HW_FEAT_SAVLANINS	0x08000000	/*                     */
#define DMA_HW_FEAT_ACTPHYIF	0x70000000	/*                           */
#define DEFAULT_DMA_PBL		8

/*                                       */
#define MAX_DMA_RIWT		0xff
#define MIN_DMA_RIWT		0x20
/*                        */
#define STMMAC_COAL_TX_TIMER	40000
#define STMMAC_MAX_COAL_TX_TICK	100000
#define STMMAC_TX_MAX_FRAMES	256
#define STMMAC_TX_FRAMES	64

/*               */
enum rx_frame_status {
	good_frame = 0,
	discard_frame = 1,
	csum_none = 2,
	llc_snap = 4,
};

enum dma_irq_status {
	tx_hard_error = 0x1,
	tx_hard_error_bump_tc = 0x2,
	handle_rx = 0x4,
	handle_tx = 0x8,
};

#define	CORE_IRQ_TX_PATH_IN_LPI_MODE	(1 << 1)
#define	CORE_IRQ_TX_PATH_EXIT_LPI_MODE	(1 << 2)
#define	CORE_IRQ_RX_PATH_IN_LPI_MODE	(1 << 3)
#define	CORE_IRQ_RX_PATH_EXIT_LPI_MODE	(1 << 4)

#define	CORE_PCS_ANE_COMPLETE		(1 << 5)
#define	CORE_PCS_LINK_STATUS		(1 << 6)
#define	CORE_RGMII_IRQ			(1 << 7)

struct rgmii_adv {
	unsigned int pause;
	unsigned int duplex;
	unsigned int lp_pause;
	unsigned int lp_duplex;
};

#define STMMAC_PCS_PAUSE	1
#define STMMAC_PCS_ASYM_PAUSE	2

/*                     */
struct dma_features {
	unsigned int mbps_10_100;
	unsigned int mbps_1000;
	unsigned int half_duplex;
	unsigned int hash_filter;
	unsigned int multi_addr;
	unsigned int pcs;
	unsigned int sma_mdio;
	unsigned int pmt_remote_wake_up;
	unsigned int pmt_magic_frame;
	unsigned int rmon;
	/*                */
	unsigned int time_stamp;
	/*                */
	unsigned int atime_stamp;
	/*                                           */
	unsigned int eee;
	unsigned int av;
	/*                */
	unsigned int tx_coe;
	unsigned int rx_coe_type1;
	unsigned int rx_coe_type2;
	unsigned int rxfifo_over_2048;
	/*                              */
	unsigned int number_rx_channel;
	unsigned int number_tx_channel;
	/*                                */
	unsigned int enh_desc;
};

/*                                    */
#define BUF_SIZE_16KiB 16384
#define BUF_SIZE_8KiB 8192
#define BUF_SIZE_4KiB 4096
#define BUF_SIZE_2KiB 2048

/*                    */
#define PMT_NOT_SUPPORTED 0
#define PMT_SUPPORTED 1

/*                    */
#define MAC_CTRL_REG		0x00000000	/*             */
#define MAC_ENABLE_TX		0x00000008	/*                    */
#define MAC_RNABLE_RX		0x00000004	/*                 */

/*                    */
#define STMMAC_DEFAULT_LIT_LS	0x3E8
#define STMMAC_DEFAULT_TWT_LS	0x0

#define STMMAC_CHAIN_MODE	0x1
#define STMMAC_RING_MODE	0x2

struct stmmac_desc_ops {
	/*                                       */
	void (*init_rx_desc) (struct dma_desc *p, int disable_rx_ic, int mode,
			      int end);
	/*                                       */
	void (*init_tx_desc) (struct dma_desc *p, int mode, int end);

	/*                                                           */
	void (*prepare_tx_desc) (struct dma_desc *p, int is_fs, int len,
				 int csum_flag, int mode);
	/*                                     */
	void (*set_tx_owner) (struct dma_desc *p);
	int (*get_tx_owner) (struct dma_desc *p);
	/*                                                         */
	void (*close_tx_desc) (struct dma_desc *p);
	/*                                                           */
	void (*release_tx_desc) (struct dma_desc *p, int mode);
	/*                                                         
                                                                 */
	void (*clear_tx_ic) (struct dma_desc *p);
	/*                                             */
	int (*get_tx_ls) (struct dma_desc *p);
	/*                                                 */
	int (*tx_status) (void *data, struct stmmac_extra_stats *x,
			  struct dma_desc *p, void __iomem *ioaddr);
	/*                                         */
	int (*get_tx_len) (struct dma_desc *p);
	/*                                                         */
	int (*get_rx_owner) (struct dma_desc *p);
	void (*set_rx_owner) (struct dma_desc *p);
	/*                            */
	int (*get_rx_frame_len) (struct dma_desc *p, int rx_coe_type);
	/*                                                  */
	int (*rx_status) (void *data, struct stmmac_extra_stats *x,
			  struct dma_desc *p);
	void (*rx_extended_status) (void *data, struct stmmac_extra_stats *x,
				    struct dma_extended_desc *p);
	/*                             */
	void (*enable_tx_timestamp) (struct dma_desc *p);
	/*                         */
	int (*get_tx_timestamp_status) (struct dma_desc *p);
	/*                     */
	 u64(*get_timestamp) (void *desc, u32 ats);
	/*                         */
	int (*get_rx_timestamp_status) (void *desc, u32 ats);
};

struct stmmac_dma_ops {
	/*                         */
	int (*init) (void __iomem *ioaddr, int pbl, int fb, int mb,
		     int burst_len, u32 dma_tx, u32 dma_rx, int atds);
	/*                    */
	void (*dump_regs) (void __iomem *ioaddr);
	/*                                         
                                                        */
	void (*dma_mode) (void __iomem *ioaddr, int txmode, int rxmode);
	/*                                         */
	void (*dma_diagnostic_fr) (void *data, struct stmmac_extra_stats *x,
				   void __iomem *ioaddr);
	void (*enable_dma_transmission) (void __iomem *ioaddr);
	void (*enable_dma_irq) (void __iomem *ioaddr);
	void (*disable_dma_irq) (void __iomem *ioaddr);
	void (*start_tx) (void __iomem *ioaddr);
	void (*stop_tx) (void __iomem *ioaddr);
	void (*start_rx) (void __iomem *ioaddr);
	void (*stop_rx) (void __iomem *ioaddr);
	int (*dma_interrupt) (void __iomem *ioaddr,
			      struct stmmac_extra_stats *x);
	/*                                                  */
	unsigned int (*get_hw_feature) (void __iomem *ioaddr);
	/*                            */
	void (*rx_watchdog) (void __iomem *ioaddr, u32 riwt);
};

struct stmmac_ops {
	/*                         */
	void (*core_init) (void __iomem *ioaddr);
	/*                                                    */
	int (*rx_ipc) (void __iomem *ioaddr);
	/*                    */
	void (*dump_regs) (void __iomem *ioaddr);
	/*                                                         */
	int (*host_irq_status) (void __iomem *ioaddr,
				struct stmmac_extra_stats *x);
	/*                          */
	void (*set_filter) (struct net_device *dev, int id);
	/*                      */
	void (*flow_ctrl) (void __iomem *ioaddr, unsigned int duplex,
			   unsigned int fc, unsigned int pause_time);
	/*                                              */
	void (*pmt) (void __iomem *ioaddr, unsigned long mode);
	/*                               */
	void (*set_umac_addr) (void __iomem *ioaddr, unsigned char *addr,
			       unsigned int reg_n);
	void (*get_umac_addr) (void __iomem *ioaddr, unsigned char *addr,
			       unsigned int reg_n);
	void (*set_eee_mode) (void __iomem *ioaddr);
	void (*reset_eee_mode) (void __iomem *ioaddr);
	void (*set_eee_timer) (void __iomem *ioaddr, int ls, int tw);
	void (*set_eee_pls) (void __iomem *ioaddr, int link);
	void (*ctrl_ane) (void __iomem *ioaddr, bool restart);
	void (*get_adv) (void __iomem *ioaddr, struct rgmii_adv *adv);
};

struct stmmac_hwtimestamp {
	void (*config_hw_tstamping) (void __iomem *ioaddr, u32 data);
	void (*config_sub_second_increment) (void __iomem *ioaddr);
	int (*init_systime) (void __iomem *ioaddr, u32 sec, u32 nsec);
	int (*config_addend) (void __iomem *ioaddr, u32 addend);
	int (*adjust_systime) (void __iomem *ioaddr, u32 sec, u32 nsec,
			       int add_sub);
	 u64(*get_systime) (void __iomem *ioaddr);
};

struct mac_link {
	int port;
	int duplex;
	int speed;
};

struct mii_regs {
	unsigned int addr;	/*             */
	unsigned int data;	/*          */
};

struct stmmac_ring_mode_ops {
	unsigned int (*is_jumbo_frm) (int len, int ehn_desc);
	unsigned int (*jumbo_frm) (void *priv, struct sk_buff *skb, int csum);
	void (*refill_desc3) (void *priv, struct dma_desc *p);
	void (*init_desc3) (struct dma_desc *p);
	void (*clean_desc3) (void *priv, struct dma_desc *p);
	int (*set_16kib_bfsize) (int mtu);
};

struct stmmac_chain_mode_ops {
	void (*init) (void *des, dma_addr_t phy_addr, unsigned int size,
		      unsigned int extend_desc);
	unsigned int (*is_jumbo_frm) (int len, int ehn_desc);
	unsigned int (*jumbo_frm) (void *priv, struct sk_buff *skb, int csum);
	void (*refill_desc3) (void *priv, struct dma_desc *p);
	void (*clean_desc3) (void *priv, struct dma_desc *p);
};

struct mac_device_info {
	const struct stmmac_ops *mac;
	const struct stmmac_desc_ops *desc;
	const struct stmmac_dma_ops *dma;
	const struct stmmac_ring_mode_ops *ring;
	const struct stmmac_chain_mode_ops *chain;
	const struct stmmac_hwtimestamp *ptp;
	struct mii_regs mii;	/*                        */
	struct mac_link link;
	unsigned int synopsys_uid;
};

struct mac_device_info *dwmac1000_setup(void __iomem *ioaddr);
struct mac_device_info *dwmac100_setup(void __iomem *ioaddr);

extern void stmmac_set_mac_addr(void __iomem *ioaddr, u8 addr[6],
				unsigned int high, unsigned int low);
extern void stmmac_get_mac_addr(void __iomem *ioaddr, unsigned char *addr,
				unsigned int high, unsigned int low);

extern void stmmac_set_mac(void __iomem *ioaddr, bool enable);

extern void dwmac_dma_flush_tx_fifo(void __iomem *ioaddr);
extern const struct stmmac_ring_mode_ops ring_mode_ops;
extern const struct stmmac_chain_mode_ops chain_mode_ops;

#endif /*              */
