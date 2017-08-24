/*
                                                                  
                                                                  
  
                                                                        
                                                                          
                                                
 */

#ifndef __LINUX_USB_MUSBFSH_H
#define __LINUX_USB_MUSBFSH_H

/*                                                                       
                                                                     
 */
enum musbfsh_mode {
	MUSBFSH_UNDEFINED = 0,
	MUSBFSH_HOST,		/*                       */
	MUSBFSH_PERIPHERAL,	/*                       */
	MUSBFSH_OTG		/*                   */
};

struct clk;
enum musbfsh_fifo_style {
	FIFO_RXTX,
	FIFO_TX,
	FIFO_RX
} __attribute__ ((packed));

enum musbfsh_buf_mode {
	BUF_SINGLE,
	BUF_DOUBLE
} __attribute__ ((packed));

struct musbfsh_fifo_cfg {
	u8 hw_ep_num;
	enum musbfsh_fifo_style style;
	enum musbfsh_buf_mode mode;
	u16 maxpacket;
};

#define MUSBFSH_EP_FIFO(ep, st, m, pkt)		\
{						\
	.hw_ep_num	= ep,			\
	.style		= st,			\
	.mode		= m,			\
	.maxpacket	= pkt,			\
}

#define MUSBFSH_EP_FIFO_SINGLE(ep, st, pkt)	\
	MUSBFSH_EP_FIFO(ep, st, BUF_SINGLE, pkt)

#define MUSBFSH_EP_FIFO_DOUBLE(ep, st, pkt)	\
	MUSBFSH_EP_FIFO(ep, st, BUF_DOUBLE, pkt)

struct musbfsh_hdrc_eps_bits {
	const char name[16];
	u8 bits;
};

struct musbfsh_hdrc_config {
	struct musbfsh_fifo_cfg *fifo_cfg;	/*                          */
	unsigned fifo_cfg_size;	/*                                */

	/*                                     */
	unsigned multipoint:1;	/*                   */
	unsigned dyn_fifo:1 __deprecated;	/*                              */
	unsigned soft_con:1 __deprecated;	/*                       */
	unsigned utm_16:1 __deprecated;	/*                           */
	unsigned big_endian:1;	/*                             */
	unsigned mult_bulk_tx:1;	/*                                  */
	unsigned mult_bulk_rx:1;	/*                                  */
	unsigned high_iso_tx:1;	/*                           */
	unsigned high_iso_rx:1;	/*                           */
	unsigned dma:1 __deprecated;	/*              */
	unsigned vendor_req:1 __deprecated;	/*                           */

	u8 num_eps;		/*                                */
	u8 dma_channels __deprecated;	/*                        */
	u8 dyn_fifo_size;	/*                       */
	u8 vendor_ctrl __deprecated;	/*                          */
	u8 vendor_stat __deprecated;	/*                         */
	u8 dma_req_chan __deprecated;	/*                                   */
	u8 ram_bits;		/*                  */

	struct musbfsh_hdrc_eps_bits *eps_bits __deprecated;

};

struct musbfsh_hdrc_platform_data {
	/*                                                  */
	u8 mode;

	const char *clock;
	/*                                  */
	int (*set_vbus) (struct device *dev, int is_on);

	/*                                                      */
	u8 power;

	u8 min_power;
	u8 potpgt;
	/*                                             */
	unsigned extvbus:1;

	/*                            */
	int (*set_power) (int state);

	/*                                     */
	struct musbfsh_hdrc_config *config;

	void *board_data;
	const void *platform_ops;
};

#endif				/*                       */
