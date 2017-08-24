/*
 * Copyright (C) 2011 Marvell International Ltd. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 */

#ifndef __MV_U3D_H
#define __MV_U3D_H

#define MV_U3D_EP_CONTEXT_ALIGNMENT	32
#define MV_U3D_TRB_ALIGNMENT	16
#define MV_U3D_DMA_BOUNDARY	4096
#define MV_U3D_EP0_MAX_PKT_SIZE	512

/*                    */
#define MV_U3D_WAIT_FOR_SETUP		0
#define MV_U3D_DATA_STATE_XMIT		1
#define MV_U3D_DATA_STATE_NEED_ZLP	2
#define MV_U3D_WAIT_FOR_OUT_STATUS	3
#define MV_U3D_DATA_STATE_RECV		4
#define MV_U3D_STATUS_STAGE		5

#define MV_U3D_EP_MAX_LENGTH_TRANSFER	0x10000

/*                       */
#define MV_U3D_USBINT_SETUP		0x00000001
#define MV_U3D_USBINT_RX_COMPLETE	0x00000002
#define MV_U3D_USBINT_TX_COMPLETE	0x00000004
#define MV_U3D_USBINT_UNDER_RUN	0x00000008
#define MV_U3D_USBINT_RXDESC_ERR	0x00000010
#define MV_U3D_USBINT_TXDESC_ERR	0x00000020
#define MV_U3D_USBINT_RX_TRB_COMPLETE	0x00000040
#define MV_U3D_USBINT_TX_TRB_COMPLETE	0x00000080
#define MV_U3D_USBINT_VBUS_VALID	0x00010000
#define MV_U3D_USBINT_STORAGE_CMD_FULL	0x00020000
#define MV_U3D_USBINT_LINK_CHG		0x01000000

/*                       */
#define MV_U3D_INTR_ENABLE_SETUP		0x00000001
#define MV_U3D_INTR_ENABLE_RX_COMPLETE		0x00000002
#define MV_U3D_INTR_ENABLE_TX_COMPLETE		0x00000004
#define MV_U3D_INTR_ENABLE_UNDER_RUN		0x00000008
#define MV_U3D_INTR_ENABLE_RXDESC_ERR		0x00000010
#define MV_U3D_INTR_ENABLE_TXDESC_ERR		0x00000020
#define MV_U3D_INTR_ENABLE_RX_TRB_COMPLETE	0x00000040
#define MV_U3D_INTR_ENABLE_TX_TRB_COMPLETE	0x00000080
#define MV_U3D_INTR_ENABLE_RX_BUFFER_ERR	0x00000100
#define MV_U3D_INTR_ENABLE_VBUS_VALID		0x00010000
#define MV_U3D_INTR_ENABLE_STORAGE_CMD_FULL	0x00020000
#define MV_U3D_INTR_ENABLE_LINK_CHG		0x01000000
#define MV_U3D_INTR_ENABLE_PRIME_STATUS	0x02000000

/*                  */
#define MV_U3D_LINK_CHANGE_LINK_UP		0x00000001
#define MV_U3D_LINK_CHANGE_SUSPEND		0x00000002
#define MV_U3D_LINK_CHANGE_RESUME		0x00000004
#define MV_U3D_LINK_CHANGE_WRESET		0x00000008
#define MV_U3D_LINK_CHANGE_HRESET		0x00000010
#define MV_U3D_LINK_CHANGE_VBUS_INVALID	0x00000020
#define MV_U3D_LINK_CHANGE_INACT		0x00000040
#define MV_U3D_LINK_CHANGE_DISABLE_AFTER_U0	0x00000080
#define MV_U3D_LINK_CHANGE_U1			0x00000100
#define MV_U3D_LINK_CHANGE_U2			0x00000200
#define MV_U3D_LINK_CHANGE_U3			0x00000400

/*                */
#define MV_U3D_BRIDGE_SETTING_VBUS_VALID	(1 << 16)

/*                            */
#define MV_U3D_CMD_RUN_STOP		0x00000001
#define MV_U3D_CMD_CTRL_RESET		0x00000002

/*                     */
#define MV_U3D_EPXCR_EP_TYPE_CONTROL		0
#define MV_U3D_EPXCR_EP_TYPE_ISOC		1
#define MV_U3D_EPXCR_EP_TYPE_BULK		2
#define MV_U3D_EPXCR_EP_TYPE_INT		3
#define MV_U3D_EPXCR_EP_ENABLE_SHIFT		4
#define MV_U3D_EPXCR_MAX_BURST_SIZE_SHIFT	12
#define MV_U3D_EPXCR_MAX_PACKET_SIZE_SHIFT	16
#define MV_U3D_USB_BULK_BURST_OUT		6
#define MV_U3D_USB_BULK_BURST_IN		14

#define MV_U3D_EPXCR_EP_FLUSH		(1 << 7)
#define MV_U3D_EPXCR_EP_HALT		(1 << 1)
#define MV_U3D_EPXCR_EP_INIT		(1)

/*                       */
#define MV_U3D_XFERSTATUS_COMPLETE_SHIFT	24
#define MV_U3D_COMPLETE_INVALID	0
#define MV_U3D_COMPLETE_SUCCESS	1
#define MV_U3D_COMPLETE_BUFF_ERR	2
#define MV_U3D_COMPLETE_SHORT_PACKET	3
#define MV_U3D_COMPLETE_TRB_ERR	5
#define MV_U3D_XFERSTATUS_TRB_LENGTH_MASK	(0xFFFFFF)

#define MV_U3D_USB_LINK_BYPASS_VBUS	0x8

#define MV_U3D_LTSSM_PHY_INIT_DONE		0x80000000
#define MV_U3D_LTSSM_NEVER_GO_COMPLIANCE	0x40000000

#define MV_U3D_USB3_OP_REGS_OFFSET	0x100
#define MV_U3D_USB3_PHY_OFFSET		0xB800

#define DCS_ENABLE	0x1

/*         */
#define MV_U3D_RESET_TIMEOUT		10000
#define MV_U3D_FLUSH_TIMEOUT		100000
#define MV_U3D_OWN_TIMEOUT		10000
#define LOOPS_USEC_SHIFT	4
#define LOOPS_USEC		(1 << LOOPS_USEC_SHIFT)
#define LOOPS(timeout)		((timeout) >> LOOPS_USEC_SHIFT)

/*              */
#define MV_U3D_EP_DIR_IN		1
#define MV_U3D_EP_DIR_OUT		0
#define mv_u3d_ep_dir(ep)	(((ep)->ep_num == 0) ? \
				((ep)->u3d->ep0_dir) : ((ep)->direction))

/*                          */
struct mv_u3d_cap_regs {
	u32	rsvd[5];
	u32	dboff;	/*                          */
	u32	rtsoff;	/*                         */
	u32	vuoff;	/*                               */
};

/*                     */
struct mv_u3d_op_regs {
	u32	usbcmd;		/*                  */
	u32	rsvd1[11];
	u32	dcbaapl;	/*                                          */
	u32	dcbaaph;	/*                                           */
	u32	rsvd2[243];
	u32	portsc;		/*                                 */
	u32	portlinkinfo;	/*                        */
	u32	rsvd3[9917];
	u32	doorbell;	/*                   */
};

/*                                  */
struct epxcr {
	u32	epxoutcr0;	/*                           */
	u32	epxoutcr1;	/*                           */
	u32	epxincr0;	/*                          */
	u32	epxincr1;	/*                          */
};

/*                           */
struct xferstatus {
	u32	curdeqlo;	/*                         */
	u32	curdeqhi;	/*                          */
	u32	statuslo;	/*                     */
	u32	statushi;	/*                      */
};

/*                                 */
struct mv_u3d_vuc_regs {
	u32	ctrlepenable;	/*                                  */
	u32	setuplock;	/*                     */
	u32	endcomplete;	/*                                     */
	u32	intrcause;	/*                          */
	u32	intrenable;	/*                           */
	u32	trbcomplete;	/*                       */
	u32	linkchange;	/*                      */
	u32	rsvd1[5];
	u32	trbunderrun;	/*                       */
	u32	rsvd2[43];
	u32	bridgesetting;	/*                         */
	u32	rsvd3[7];
	struct xferstatus	txst[16];	/*                    */
	struct xferstatus	rxst[16];	/*                    */
	u32	ltssm;		/*                        */
	u32	pipe;		/*                       */
	u32	linkcr0;	/*                         */
	u32	linkcr1;	/*                         */
	u32	rsvd6[60];
	u32	mib0;		/*                       */
	u32	usblink;	/*                           */
	u32	ltssmstate;	/*                      */
	u32	linkerrorcause;	/*                           */
	u32	rsvd7[60];
	u32	devaddrtiebrkr;	/*                                */
	u32	itpinfo0;	/*                     */
	u32	itpinfo1;	/*                     */
	u32	rsvd8[61];
	struct epxcr	epcr[16];	/*                     */
	u32	rsvd9[64];
	u32	phyaddr;	/*                      */
	u32	phydata;	/*                   */
};

/*                            */
struct mv_u3d_ep_context {
	u32	rsvd0;
	u32	rsvd1;
	u32	trb_addr_lo;		/*                        */
	u32	trb_addr_hi;		/*                         */
	u32	rsvd2;
	u32	rsvd3;
	struct usb_ctrlrequest setup_buffer;	/*                   */
};

/*                            */
struct mv_u3d_trb_ctrl {
	u32	own:1;		/*              */
	u32	rsvd1:3;
	u32	chain:1;	/*                            
                         */
	u32	ioc:1;		/*                       */
	u32	rsvd2:4;
	u32	type:6;		/*          */
#define TYPE_NORMAL	1
#define TYPE_DATA	3
#define TYPE_LINK	6
	u32	dir:1;		/*                                          
                                     */
	u32	rsvd3:15;
};

/*                   
                                                                         
 */
struct mv_u3d_trb_hw {
	u32	buf_addr_lo;	/*                                */
	u32	buf_addr_hi;	/*                                 */
	u32	trb_len;	/*                 */
	struct mv_u3d_trb_ctrl	ctrl;	/*                  */
};

/*               */
struct mv_u3d_trb {
	struct mv_u3d_trb_hw *trb_hw;	/*                               */
	dma_addr_t trb_dma;		/*                             */
	struct list_head trb_list;	/*          */
};

/*                       */
struct mv_u3d {
	struct usb_gadget		gadget;
	struct usb_gadget_driver	*driver;
	spinlock_t			lock;	/*             */
	struct completion		*done;
	struct device			*dev;
	int				irq;

	/*                          */
	struct mv_u3d_cap_regs __iomem	*cap_regs;
	struct mv_u3d_op_regs __iomem	*op_regs;
	struct mv_u3d_vuc_regs __iomem	*vuc_regs;
	void __iomem			*phy_regs;

	unsigned int			max_eps;
	struct mv_u3d_ep_context	*ep_context;
	size_t				ep_context_size;
	dma_addr_t			ep_context_dma;

	struct dma_pool			*trb_pool; /*                        */
	struct mv_u3d_ep		*eps;

	struct mv_u3d_req		*status_req; /*                    */
	struct usb_ctrlrequest		local_setup_buff; /*                 */

	unsigned int		resume_state;	/*                     */
	unsigned int		usb_state;	/*                   */
	unsigned int		ep0_state;	/*                     */
	unsigned int		ep0_dir;

	unsigned int		dev_addr;	/*                */

	unsigned int		errors;

	unsigned		softconnect:1;
	unsigned		vbus_active:1;	/*                       */
	unsigned		remote_wakeup:1; /*                       */
	unsigned		clock_gating:1;	/*                     */
	unsigned		active:1;	/*                      */
	unsigned		vbus_valid_detect:1; /*                    */

	struct mv_usb_addon_irq *vbus;
	unsigned int		power;

	struct clk		*clk;
};

/*                         */
struct mv_u3d_ep {
	struct usb_ep		ep;
	struct mv_u3d		*u3d;
	struct list_head	queue;	/*                            */
	struct list_head	req_list; /*                    */
	struct mv_u3d_ep_context	*ep_context; /*            */
	u32			direction;
	char			name[14];
	u32			processing; /*                    
                          */
	spinlock_t		req_lock; /*         */
	unsigned		wedge:1;
	unsigned		enabled:1;
	unsigned		ep_type:2;
	unsigned		ep_num:8;
};

/*                        */
struct mv_u3d_req {
	struct usb_request	req;
	struct mv_u3d_ep	*ep;
	struct list_head	queue;	/*                              */
	struct list_head	list;	/*                 */
	struct list_head	trb_list; /*                       */

	struct mv_u3d_trb	*trb_head; /*                                 */
	unsigned		trb_count; /*                         */
	unsigned		chain;	   /*                  */
};

#endif
