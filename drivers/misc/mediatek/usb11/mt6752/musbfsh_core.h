/*
 * MUSB OTG driver defines
 *
 * Copyright 2005 Mentor Graphics Corporation
 * Copyright (C) 2005-2006 by Texas Instruments
 * Copyright (C) 2006-2007 Nokia Corporation
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
 * NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef __MUSBFSH_CORE_H__
#define __MUSBFSH_CORE_H__


#include <linux/slab.h>
#include <linux/list.h>
#include <linux/interrupt.h>
#include <linux/errno.h>
#include <linux/timer.h>
#include <linux/clk.h>
#include <linux/device.h>
#include <linux/usb.h>
#include <linux/usb/hcd.h>
#include <linux/usb/otg.h>

struct musbfsh;
struct musbfsh_hw_ep;
struct musbfsh_ep;

/*                                           */
#define MUSBFSH_HWVERS_MAJOR(x)	((x >> 10) & 0x1f)
#define MUSBFSH_HWVERS_MINOR(x)	(x & 0x3ff)
#define MUSBFSH_HWVERS_RC		0x8000
#define MUSBFSH_HWVERS_1300	0x52C
#define MUSBFSH_HWVERS_1400	0x590
#define MUSBFSH_HWVERS_1800	0x720
#define MUSBFSH_HWVERS_1900	0x784
#define MUSBFSH_HWVERS_2000	0x800

#include <linux/musbfsh.h>
#include "musbfsh_io.h"
#include "musbfsh_regs.h"
#include "musbfsh_debug.h"

/*                                                                          */

#define	is_host_capable()	(1)

extern irqreturn_t musbfsh_h_ep0_irq(struct musbfsh *);
extern void musbfsh_host_tx(struct musbfsh *, u8);
extern void musbfsh_host_rx(struct musbfsh *, u8);
//                                                                            


/*                                                                       */

#ifndef MUSBFSH_C_NUM_EPS
#define MUSBFSH_C_NUM_EPS ((u8)16)
#endif

#ifndef MUSBFSH_MAX_END0_PACKET
#define MUSBFSH_MAX_END0_PACKET ((u16)MUSBFSH_EP0_FIFOSIZE)
#endif

/*                      */
enum musbfsh_h_ep0_state {
	MUSBFSH_EP0_IDLE,
	MUSBFSH_EP0_START,			/*                     */
	MUSBFSH_EP0_IN,			/*                */
	MUSBFSH_EP0_OUT,			/*                        */
	MUSBFSH_EP0_STATUS,		/*                      */
} __attribute__ ((packed));

/*                                                                          */

/*                                                                 */
#define musbfsh_ep_select(_mbase, _epnum) \
	musbfsh_writeb((_mbase), MUSBFSH_INDEX, (_epnum))
#define	MUSBFSH_EP_OFFSET			MUSBFSH_INDEXED_OFFSET

/*                                                                       */

#define test_devctl_hst_mode(_x) \
	(musbfsh_readb((_x)->mregs, MUSBFSH_DEVCTL)&MUSBFSH_DEVCTL_HM)

/*                                                                          */

/*
                                                           
  
                                                  
 */
struct musbfsh_hw_ep {
	struct musbfsh		*musbfsh;
    void __iomem		*fifo;
	void __iomem		*regs;
	/*                                */
	u8			epnum;
	/*                                          */
	bool			is_shared_fifo;
	bool			tx_double_buffered;
	bool			rx_double_buffered;
	u16			max_packet_sz_tx;
	u16			max_packet_sz_rx;
	struct dma_channel	*tx_channel;
	struct dma_channel	*rx_channel;
    void __iomem		*target_regs;
	/*                                         */
	struct musbfsh_qh		*in_qh;
	struct musbfsh_qh		*out_qh;
	u8			rx_reinit;
	u8			tx_reinit;
};


struct musbfsh_csr_regs {
	/*                */
	u16 txmaxp, txcsr, rxmaxp, rxcsr;
	u16 rxfifoadd, txfifoadd;
	u8 txtype, txinterval, rxtype, rxinterval;
	u8 rxfifosz, txfifosz;
	u8 txfunaddr, txhubaddr, txhubport;
	u8 rxfunaddr, rxhubaddr, rxhubport;
};

struct musbfsh_context_registers {

	u8 power;
	u16 intrtxe, intrrxe;
	u8 intrusbe;
	u16 frame;
	u8 index, testmode;

	u8 devctl, busctl, misc;
	u32 otg_interfsel;
	u32 l1_int;

	struct musbfsh_csr_regs index_regs[MUSBFSH_C_NUM_EPS];
};

/*
                                      
 */
struct musbfsh {
	/*             */
	spinlock_t		lock;
	struct musbfsh_context_registers context;
	irqreturn_t		(*isr)(int, void *);

/*                                                                    */
#define MUSBFSH_PORT_STAT_RESUME	(1 << 31)

	u32			port1_status;

	unsigned long		rh_timer;
	enum musbfsh_h_ep0_state	ep0_stage;

	/*                                                            
                                                      
                                                                  
                                                                   
             
  */
	struct musbfsh_hw_ep	*bulk_ep;
	struct list_head	control;	/*               */
	struct list_head	in_bulk;	/*               */
	struct list_head	out_bulk;	/*               */

	/*                                                                 
                                            
  */
	void			(*board_set_vbus)(struct musbfsh *, int is_on);

	struct dma_controller	*dma_controller;
	struct musbfsh_dma_controller *musbfsh_dma_controller;

	struct device		*controller;
	void __iomem		*ctrl_base;
	void __iomem		*phy_base;
	void __iomem		*mregs;

	/*                                                   */
	u8			int_usb;
	u16			int_rx;
	u16			int_tx;
	u8			int_dma;

	int nIrq;
	unsigned		irq_wake:1;

	struct musbfsh_hw_ep	 endpoints[MUSBFSH_C_NUM_EPS];
#define control_ep		endpoints

#define VBUSERR_RETRY_COUNT	3
	u16			vbuserr_retry;
	u16 epmask;
	u8 nr_endpoints;

	u8 board_mode;		/*                   */
	int			(*board_set_power)(int state);
	bool			is_host;
	unsigned		is_multipoint:1; 

	unsigned long		idle_timeout;	/*                         */

	/*                                          */
	unsigned		is_active:1;
	unsigned ignore_disconnect:1;	/*                   */
	unsigned		dyn_fifo:1;	/*                         */

	struct musbfsh_hdrc_config	*config;
};

static inline void musbfsh_set_vbus(struct musbfsh *musbfsh, int is_on)
{
    INFO("musbfsh_set_vbus++, is_on=%d\r\n",is_on);
	if(musbfsh->board_set_vbus)
		musbfsh->board_set_vbus(musbfsh, is_on);
}

static inline void musbfsh_set_power(struct musbfsh *musbfsh, int is_on)
{
	INFO("musbfsh_set_power++, is_on=%d\r\n",is_on);
	if(musbfsh->board_set_power)
		musbfsh->board_set_power(is_on);
}

static inline void musbfsh_configure_ep0(struct musbfsh *musbfsh)
{
	musbfsh->endpoints[0].max_packet_sz_tx = MUSBFSH_EP0_FIFOSIZE;
	musbfsh->endpoints[0].max_packet_sz_rx = MUSBFSH_EP0_FIFOSIZE;
	musbfsh->endpoints[0].is_shared_fifo = true;
}



/*                                                                          */

extern const char musbfsh_driver_name[];

extern void musbfsh_start(struct musbfsh *musbfsh);
extern void musbfsh_stop(struct musbfsh *musbfsh);

extern void musbfsh_write_fifo(struct musbfsh_hw_ep *ep, u16 len, const u8 *src);
extern void musbfsh_read_fifo(struct musbfsh_hw_ep *ep, u16 len, u8 *dst);

extern void musbfsh_load_testpacket(struct musbfsh *);

extern irqreturn_t musbfsh_interrupt(struct musbfsh *);

extern void musbfsh_platform_enable(struct musbfsh *musbfsh);
extern void musbfsh_platform_disable(struct musbfsh *musbfsh);

#define musbfsh_platform_try_idle(x, y)		do {} while (0)

#define musbfsh_platform_get_vbus_status(x)	0

extern int __init musbfsh_platform_init(struct musbfsh *musbfsh);
extern int musbfsh_platform_exit(struct musbfsh *musbfsh);

#define CONFIG_MUSBFSH_DEBUG_FS
#ifdef CONFIG_MUSBFSH_DEBUG_FS
int musbfsh_init_debugfs(struct musbfsh *musb);
void musbfsh_exit_debugfs(struct musbfsh *musb);
void create_musbfsh_cmd_proc_entry(void);
#else
static inline int musbfsh_init_debugfs(struct musbfsh *musbfsh)
{
	return 0;
}
static inline void musbfsh_exit_debugfs(struct musbfsh *musbfsh)
{
}
static inline void create_musbfsh_cmd_proc_entry(void)
{
}
#endif

#endif	/*                    */
