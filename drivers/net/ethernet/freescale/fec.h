/*                                                                          */

/*
 *	fec.h  --  Fast Ethernet Controller for Motorola ColdFire SoC
 *		   processors.
 *
 *	(C) Copyright 2000-2005, Greg Ungerer (gerg@snapgear.com)
 *	(C) Copyright 2000-2001, Lineo (www.lineo.com)
 */

/*                                                                          */
#ifndef FEC_H
#define	FEC_H
/*                                                                          */

#include <linux/clocksource.h>
#include <linux/net_tstamp.h>
#include <linux/ptp_clock_kernel.h>

#if defined(CONFIG_M523x) || defined(CONFIG_M527x) || defined(CONFIG_M528x) || \
    defined(CONFIG_M520x) || defined(CONFIG_M532x) || \
    defined(CONFIG_ARCH_MXC) || defined(CONFIG_SOC_IMX28)
/*
                                                              
                                                              
                   
 */
#define FEC_IEVENT		0x004 /*                     */
#define FEC_IMASK		0x008 /*                    */
#define FEC_R_DES_ACTIVE	0x010 /*                        */
#define FEC_X_DES_ACTIVE	0x014 /*                         */
#define FEC_ECNTRL		0x024 /*                      */
#define FEC_MII_DATA		0x040 /*                      */
#define FEC_MII_SPEED		0x044 /*                       */
#define FEC_MIB_CTRLSTAT	0x064 /*                        */
#define FEC_R_CNTRL		0x084 /*                     */
#define FEC_X_CNTRL		0x0c4 /*                      */
#define FEC_ADDR_LOW		0x0e4 /*                        */
#define FEC_ADDR_HIGH		0x0e8 /*                         */
#define FEC_OPD			0x0ec /*                         */
#define FEC_HASH_TABLE_HIGH	0x118 /*                        */
#define FEC_HASH_TABLE_LOW	0x11c /*                       */
#define FEC_GRP_HASH_TABLE_HIGH	0x120 /*                        */
#define FEC_GRP_HASH_TABLE_LOW	0x124 /*                       */
#define FEC_X_WMRK		0x144 /*                          */
#define FEC_R_BOUND		0x14c /*                        */
#define FEC_R_FSTART		0x150 /*                        */
#define FEC_R_DES_START		0x180 /*                         */
#define FEC_X_DES_START		0x184 /*                          */
#define FEC_R_BUFF_SIZE		0x188 /*                           */
#define FEC_R_FIFO_RSFL		0x190 /*                                     */
#define FEC_R_FIFO_RSEM		0x194 /*                                      */
#define FEC_R_FIFO_RAEM		0x198 /*                                     */
#define FEC_R_FIFO_RAFL		0x19c /*                                    */
#define FEC_RACC		0x1C4 /*                              */
#define FEC_MIIGSK_CFGR		0x300 /*                          */
#define FEC_MIIGSK_ENR		0x308 /*                   */

#define BM_MIIGSK_CFGR_MII		0x00
#define BM_MIIGSK_CFGR_RMII		0x01
#define BM_MIIGSK_CFGR_FRCONT_10M	0x40

#else

#define FEC_ECNTRL		0x000 /*                      */
#define FEC_IEVENT		0x004 /*                    */
#define FEC_IMASK		0x008 /*                    */
#define FEC_IVEC		0x00c /*                          */
#define FEC_R_DES_ACTIVE	0x010 /*                        */
#define FEC_X_DES_ACTIVE	0x014 /*                         */
#define FEC_MII_DATA		0x040 /*                      */
#define FEC_MII_SPEED		0x044 /*                       */
#define FEC_R_BOUND		0x08c /*                        */
#define FEC_R_FSTART		0x090 /*                        */
#define FEC_X_WMRK		0x0a4 /*                          */
#define FEC_X_FSTART		0x0ac /*                         */
#define FEC_R_CNTRL		0x104 /*                     */
#define FEC_MAX_FRM_LEN		0x108 /*                          */
#define FEC_X_CNTRL		0x144 /*                      */
#define FEC_ADDR_LOW		0x3c0 /*                        */
#define FEC_ADDR_HIGH		0x3c4 /*                         */
#define FEC_GRP_HASH_TABLE_HIGH	0x3c8 /*                        */
#define FEC_GRP_HASH_TABLE_LOW	0x3cc /*                       */
#define FEC_R_DES_START		0x3d0 /*                         */
#define FEC_X_DES_START		0x3d4 /*                          */
#define FEC_R_BUFF_SIZE		0x3d8 /*                           */
#define FEC_FIFO_RAM		0x400 /*                 */

#endif /*              */


/*
                                          
 */
#if defined(CONFIG_ARCH_MXC) || defined(CONFIG_SOC_IMX28)
struct bufdesc {
	unsigned short cbd_datlen;	/*             */
	unsigned short cbd_sc;	/*                         */
	unsigned long cbd_bufaddr;	/*                */
};
#else
struct bufdesc {
	unsigned short	cbd_sc;			/*                         */
	unsigned short	cbd_datlen;		/*             */
	unsigned long	cbd_bufaddr;		/*                */
};
#endif

struct bufdesc_ex {
	struct bufdesc desc;
	unsigned long cbd_esc;
	unsigned long cbd_prot;
	unsigned long cbd_bdu;
	unsigned long ts;
	unsigned short res0[4];
};

/*
 *	The following definitions courtesy of commproc.h, which where
 *	Copyright (c) 1997 Dan Malek (dmalek@jlc.net).
 */
#define BD_SC_EMPTY     ((ushort)0x8000)        /*                  */
#define BD_SC_READY     ((ushort)0x8000)        /*                   */
#define BD_SC_WRAP      ((ushort)0x2000)        /*                        */
#define BD_SC_INTRPT    ((ushort)0x1000)        /*                     */
#define BD_SC_CM        ((ushort)0x0200)        /*                 */
#define BD_SC_ID        ((ushort)0x0100)        /*                      */
#define BD_SC_P         ((ushort)0x0100)        /*              */
#define BD_SC_BR        ((ushort)0x0020)        /*                */
#define BD_SC_FR        ((ushort)0x0010)        /*               */
#define BD_SC_PR        ((ushort)0x0008)        /*              */
#define BD_SC_OV        ((ushort)0x0002)        /*         */
#define BD_SC_CD        ((ushort)0x0001)        /*    */

/*                                                           
*/
#define BD_ENET_RX_EMPTY        ((ushort)0x8000)
#define BD_ENET_RX_WRAP         ((ushort)0x2000)
#define BD_ENET_RX_INTR         ((ushort)0x1000)
#define BD_ENET_RX_LAST         ((ushort)0x0800)
#define BD_ENET_RX_FIRST        ((ushort)0x0400)
#define BD_ENET_RX_MISS         ((ushort)0x0100)
#define BD_ENET_RX_LG           ((ushort)0x0020)
#define BD_ENET_RX_NO           ((ushort)0x0010)
#define BD_ENET_RX_SH           ((ushort)0x0008)
#define BD_ENET_RX_CR           ((ushort)0x0004)
#define BD_ENET_RX_OV           ((ushort)0x0002)
#define BD_ENET_RX_CL           ((ushort)0x0001)
#define BD_ENET_RX_STATS        ((ushort)0x013f)        /*                 */

/*                                                            
*/
#define BD_ENET_TX_READY        ((ushort)0x8000)
#define BD_ENET_TX_PAD          ((ushort)0x4000)
#define BD_ENET_TX_WRAP         ((ushort)0x2000)
#define BD_ENET_TX_INTR         ((ushort)0x1000)
#define BD_ENET_TX_LAST         ((ushort)0x0800)
#define BD_ENET_TX_TC           ((ushort)0x0400)
#define BD_ENET_TX_DEF          ((ushort)0x0200)
#define BD_ENET_TX_HB           ((ushort)0x0100)
#define BD_ENET_TX_LC           ((ushort)0x0080)
#define BD_ENET_TX_RL           ((ushort)0x0040)
#define BD_ENET_TX_RCMASK       ((ushort)0x003c)
#define BD_ENET_TX_UN           ((ushort)0x0002)
#define BD_ENET_TX_CSL          ((ushort)0x0001)
#define BD_ENET_TX_STATS        ((ushort)0x03ff)        /*                 */

/*                                                                   */
#define BD_ENET_TX_INT          0x40000000
#define BD_ENET_TX_TS           0x20000000
#define BD_ENET_TX_PINS         0x10000000
#define BD_ENET_TX_IINS         0x08000000


/*                                                    */
#define FEC_IRQ_NUM		3

/*                                                                    
                                                                   
                          
                                                                    
                         
 */

#define FEC_ENET_RX_PAGES	8
#define FEC_ENET_RX_FRSIZE	2048
#define FEC_ENET_RX_FRPPG	(PAGE_SIZE / FEC_ENET_RX_FRSIZE)
#define RX_RING_SIZE		(FEC_ENET_RX_FRPPG * FEC_ENET_RX_PAGES)
#define FEC_ENET_TX_FRSIZE	2048
#define FEC_ENET_TX_FRPPG	(PAGE_SIZE / FEC_ENET_TX_FRSIZE)
#define TX_RING_SIZE		16	/*                      */
#define TX_RING_MOD_MASK	15	/*                    */

#define BD_ENET_RX_INT          0x00800000
#define BD_ENET_RX_PTP          ((ushort)0x0400)
#define BD_ENET_RX_ICE		0x00000020
#define BD_ENET_RX_PCR		0x00000010
#define FLAG_RX_CSUM_ENABLED	(BD_ENET_RX_ICE | BD_ENET_RX_PCR)
#define FLAG_RX_CSUM_ERROR	(BD_ENET_RX_ICE | BD_ENET_RX_PCR)

struct fec_enet_delayed_work {
	struct delayed_work delay_work;
	bool timeout;
};

/*                                                                       
                                                                      
                                                             
                                                                   
                                                                       
                                                                      
                                                         
 */
struct fec_enet_private {
	/*                                      */
	void __iomem *hwp;

	struct net_device *netdev;

	struct clk *clk_ipg;
	struct clk *clk_ahb;
	struct clk *clk_enet_out;
	struct clk *clk_ptp;

	/*                                                                   */
	unsigned char *tx_bounce[TX_RING_SIZE];
	struct	sk_buff *tx_skbuff[TX_RING_SIZE];
	struct	sk_buff *rx_skbuff[RX_RING_SIZE];

	/*                                      */
	dma_addr_t	bd_dma;
	/*                              */
	struct bufdesc	*rx_bd_base;
	struct bufdesc	*tx_bd_base;
	/*                          */
	struct bufdesc	*cur_rx, *cur_tx;
	/*                                 */
	struct bufdesc	*dirty_tx;

	struct	platform_device *pdev;

	int	opened;
	int	dev_id;

	/*                           */
	struct	mii_bus *mii_bus;
	struct	phy_device *phy_dev;
	int	mii_timeout;
	uint	phy_speed;
	phy_interface_t	phy_interface;
	int	link;
	int	full_duplex;
	int	speed;
	struct	completion mdio_done;
	int	irq[FEC_IRQ_NUM];
	int	bufdesc_ex;
	int	pause_flag;

	struct	napi_struct napi;
	int	csum_flags;

	struct ptp_clock *ptp_clock;
	struct ptp_clock_info ptp_caps;
	unsigned long last_overflow_check;
	spinlock_t tmreg_lock;
	struct cyclecounter cc;
	struct timecounter tc;
	int rx_hwtstamp_filter;
	u32 base_incval;
	u32 cycle_speed;
	int hwts_rx_en;
	int hwts_tx_en;
	struct timer_list time_keep;
	struct fec_enet_delayed_work delay_work;
};

void fec_ptp_init(struct net_device *ndev, struct platform_device *pdev);
void fec_ptp_start_cyclecounter(struct net_device *ndev);
int fec_ptp_ioctl(struct net_device *ndev, struct ifreq *ifr, int cmd);

/*                                                                          */
#endif /*       */
