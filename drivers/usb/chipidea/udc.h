/*
 * udc.h - ChipIdea UDC structures
 *
 * Copyright (C) 2008 Chipidea - MIPS Technologies, Inc. All rights reserved.
 *
 * Author: David Lopo
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __DRIVERS_USB_CHIPIDEA_UDC_H
#define __DRIVERS_USB_CHIPIDEA_UDC_H

#include <linux/list.h>

#define CTRL_PAYLOAD_MAX   64
#define RX        0  /*                                                    */
#define TX        1  /*                                                    */

/*                                    */
struct ci13xxx_td {
	/*   */
	u32 next;
#define TD_TERMINATE          BIT(0)
#define TD_ADDR_MASK          (0xFFFFFFEUL << 5)
	/*   */
	u32 token;
#define TD_STATUS             (0x00FFUL <<  0)
#define TD_STATUS_TR_ERR      BIT(3)
#define TD_STATUS_DT_ERR      BIT(5)
#define TD_STATUS_HALTED      BIT(6)
#define TD_STATUS_ACTIVE      BIT(7)
#define TD_MULTO              (0x0003UL << 10)
#define TD_IOC                BIT(15)
#define TD_TOTAL_BYTES        (0x7FFFUL << 16)
	/*   */
	u32 page[5];
#define TD_CURR_OFFSET        (0x0FFFUL <<  0)
#define TD_FRAME_NUM          (0x07FFUL <<  0)
#define TD_RESERVED_MASK      (0x0FFFUL <<  0)
} __attribute__ ((packed, aligned(4)));

/*                           */
struct ci13xxx_qh {
	/*   */
	u32 cap;
#define QH_IOS                BIT(15)
#define QH_MAX_PKT            (0x07FFUL << 16)
#define QH_ZLT                BIT(29)
#define QH_MULT               (0x0003UL << 30)
	/*   */
	u32 curr;
	/*       */
	struct ci13xxx_td        td;
	/*   */
	u32 RESERVED;
	struct usb_ctrlrequest   setup;
} __attribute__ ((packed, aligned(4)));

/* 
                                                  
                                             
                          
                                             
                                                
                                                 
                                                              
 */
struct ci13xxx_req {
	struct usb_request	req;
	struct list_head	queue;
	struct ci13xxx_td	*ptr;
	dma_addr_t		dma;
	struct ci13xxx_td	*zptr;
	dma_addr_t		zdma;
};

#ifdef CONFIG_USB_CHIPIDEA_UDC

int ci_hdrc_gadget_init(struct ci13xxx *ci);

#else

static inline int ci_hdrc_gadget_init(struct ci13xxx *ci)
{
	return -ENXIO;
}

#endif

#endif /*                              */
