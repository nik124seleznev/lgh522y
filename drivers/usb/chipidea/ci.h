/*
 * ci.h - common structures, functions, and macros of the ChipIdea driver
 *
 * Copyright (C) 2008 Chipidea - MIPS Technologies, Inc. All rights reserved.
 *
 * Author: David Lopo
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __DRIVERS_USB_CHIPIDEA_CI_H
#define __DRIVERS_USB_CHIPIDEA_CI_H

#include <linux/list.h>
#include <linux/irqreturn.h>
#include <linux/usb.h>
#include <linux/usb/gadget.h>

/*                                                                             
         
                                                                             */
#define TD_PAGE_COUNT      5
#define CI13XXX_PAGE_SIZE  4096ul /*                    */
#define ENDPT_MAX          32

/*                                                                             
             
                                                                             */
/* 
                                              
                                             
                                   
                        
                       
                                            
                                    
                                 
                                 
                                          
                                            
 */
struct ci13xxx_ep {
	struct usb_ep				ep;
	u8					dir;
	u8					num;
	u8					type;
	char					name[16];
	struct {
		struct list_head	queue;
		struct ci13xxx_qh	*ptr;
		dma_addr_t		dma;
	}					qh;
	int					wedge;

	/*                  */
	struct ci13xxx				*ci;
	spinlock_t				*lock;
	struct dma_pool				*td_pool;
};

enum ci_role {
	CI_ROLE_HOST = 0,
	CI_ROLE_GADGET,
	CI_ROLE_END,
};

/* 
                                                  
                         
                       
                                 
                                       
 */
struct ci_role_driver {
	int		(*start)(struct ci13xxx *);
	void		(*stop)(struct ci13xxx *);
	irqreturn_t	(*irq)(struct ci13xxx *);
	const char	*name;
};

/* 
                                                            
                                         
                                                        
                                                             
                             
                             
                                     
                                 
 */
struct hw_bank {
	unsigned	lpm;
	resource_size_t	phys;
	void __iomem	*abs;
	void __iomem	*cap;
	void __iomem	*op;
	size_t		size;
	void __iomem	**regmap;
};

/* 
                                                  
                                 
                                
                                      
                   
                                                       
                      
                                        
                                
                        
                                            
                                                     
                                                                
                         
                                                              
                                  
                          
                                       
                                     
                              
                                                              
                                               
                                             
                                
                                     
                                                                     
                               
                                           
                                                
                                                       
 */
struct ci13xxx {
	struct device			*dev;
	spinlock_t			lock;
	struct hw_bank			hw_bank;
	int				irq;
	struct ci_role_driver		*roles[CI_ROLE_END];
	enum ci_role			role;
	bool				is_otg;
	struct work_struct		work;
	struct workqueue_struct		*wq;

	struct dma_pool			*qh_pool;
	struct dma_pool			*td_pool;

	struct usb_gadget		gadget;
	struct usb_gadget_driver	*driver;
	unsigned			hw_ep_max;
	struct ci13xxx_ep		ci13xxx_ep[ENDPT_MAX];
	u32				ep0_dir;
	struct ci13xxx_ep		*ep0out, *ep0in;

	struct usb_request		*status;
	bool				setaddr;
	u8				address;
	u8				remote_wakeup;
	u8				suspended;
	u8				test_mode;

	struct ci13xxx_platform_data	*platdata;
	int				vbus_active;
	/*                                           */
	bool				global_phy;
	struct usb_phy			*transceiver;
	struct usb_hcd			*hcd;
	struct dentry			*debugfs;
};

static inline struct ci_role_driver *ci_role(struct ci13xxx *ci)
{
	BUG_ON(ci->role >= CI_ROLE_END || !ci->roles[ci->role]);
	return ci->roles[ci->role];
}

static inline int ci_role_start(struct ci13xxx *ci, enum ci_role role)
{
	int ret;

	if (role >= CI_ROLE_END)
		return -EINVAL;

	if (!ci->roles[role])
		return -ENXIO;

	ret = ci->roles[role]->start(ci);
	if (!ret)
		ci->role = role;
	return ret;
}

static inline void ci_role_stop(struct ci13xxx *ci)
{
	enum ci_role role = ci->role;

	if (role == CI_ROLE_END)
		return;

	ci->role = CI_ROLE_END;

	ci->roles[role]->stop(ci);
}

/*                                                                             
            
                                                                             */
/*               */
#define REG_BITS   (32)

/*                  */
enum ci13xxx_regs {
	CAP_CAPLENGTH,
	CAP_HCCPARAMS,
	CAP_DCCPARAMS,
	CAP_TESTMODE,
	CAP_LAST = CAP_TESTMODE,
	OP_USBCMD,
	OP_USBSTS,
	OP_USBINTR,
	OP_DEVICEADDR,
	OP_ENDPTLISTADDR,
	OP_PORTSC,
	OP_DEVLC,
	OP_OTGSC,
	OP_USBMODE,
	OP_ENDPTSETUPSTAT,
	OP_ENDPTPRIME,
	OP_ENDPTFLUSH,
	OP_ENDPTSTAT,
	OP_ENDPTCOMPLETE,
	OP_ENDPTCTRL,
	/*                       */
	OP_LAST = OP_ENDPTCTRL + ENDPT_MAX / 2,
};

/* 
                                    
                        
                       
  
                                          
 */
static inline u32 hw_read(struct ci13xxx *ci, enum ci13xxx_regs reg, u32 mask)
{
	return ioread32(ci->hw_bank.regmap[reg]) & mask;
}

/* 
                                    
                        
                       
                   
 */
static inline void hw_write(struct ci13xxx *ci, enum ci13xxx_regs reg,
			    u32 mask, u32 data)
{
	if (~mask)
		data = (ioread32(ci->hw_bank.regmap[reg]) & ~mask)
			| (data & mask);

	iowrite32(data, ci->hw_bank.regmap[reg]);
}

/* 
                                                  
                        
                       
  
                                          
 */
static inline u32 hw_test_and_clear(struct ci13xxx *ci, enum ci13xxx_regs reg,
				    u32 mask)
{
	u32 val = ioread32(ci->hw_bank.regmap[reg]) & mask;

	iowrite32(val, ci->hw_bank.regmap[reg]);
	return val;
}

/* 
                                                  
                        
                       
                   
  
                                          
 */
static inline u32 hw_test_and_write(struct ci13xxx *ci, enum ci13xxx_regs reg,
				    u32 mask, u32 data)
{
	u32 val = hw_read(ci, reg, ~0);

	hw_write(ci, reg, mask, data);
	return (val & mask) >> __ffs(mask);
}

int hw_device_reset(struct ci13xxx *ci, u32 mode);

int hw_port_test_set(struct ci13xxx *ci, u8 mode);

u8 hw_port_test_get(struct ci13xxx *ci);

#endif	/*                             */
