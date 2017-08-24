/*
 * Copyright (c) 2010 Broadcom Corporation
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _BRCMF_BUS_H_
#define _BRCMF_BUS_H_

/*                                                */
enum brcmf_bus_state {
	BRCMF_BUS_DOWN,		/*                               */
	BRCMF_BUS_LOAD,		/*                                  */
	BRCMF_BUS_DATA		/*                           */
};

struct brcmf_bus_dcmd {
	char *name;
	char *param;
	int param_len;
	struct list_head list;
};

/* 
                                                  
  
                                                
                                                  
                                                                  
                                                        
                                                          
                                                                
  
                                                            
                                                             
                                                           
                                                     
 */
struct brcmf_bus_ops {
	int (*init)(struct device *dev);
	void (*stop)(struct device *dev);
	int (*txdata)(struct device *dev, struct sk_buff *skb);
	int (*txctl)(struct device *dev, unsigned char *msg, uint len);
	int (*rxctl)(struct device *dev, unsigned char *msg, uint len);
	struct pktq * (*gettxq)(struct device *dev);
};

/* 
                                                                      
  
                                            
                                      
                                    
                                                  
                                                   
                                                            
                                          
                                             
                                                                  
                                               
                                         
 */
struct brcmf_bus {
	union {
		struct brcmf_sdio_dev *sdio;
		struct brcmf_usbdev *usb;
	} bus_priv;
	struct device *dev;
	struct brcmf_pub *drvr;
	enum brcmf_bus_state state;
	uint maxctl;
	unsigned long tx_realloc;
	u8 align;
	u32 chip;
	u32 chiprev;
	struct list_head dcmd_list;

	struct brcmf_bus_ops *ops;
};

/*
                    
 */
static inline int brcmf_bus_init(struct brcmf_bus *bus)
{
	return bus->ops->init(bus->dev);
}

static inline void brcmf_bus_stop(struct brcmf_bus *bus)
{
	bus->ops->stop(bus->dev);
}

static inline int brcmf_bus_txdata(struct brcmf_bus *bus, struct sk_buff *skb)
{
	return bus->ops->txdata(bus->dev, skb);
}

static inline
int brcmf_bus_txctl(struct brcmf_bus *bus, unsigned char *msg, uint len)
{
	return bus->ops->txctl(bus->dev, msg, len);
}

static inline
int brcmf_bus_rxctl(struct brcmf_bus *bus, unsigned char *msg, uint len)
{
	return bus->ops->rxctl(bus->dev, msg, len);
}

static inline
struct pktq *brcmf_bus_gettxq(struct brcmf_bus *bus)
{
	if (!bus->ops->gettxq)
		return ERR_PTR(-ENOENT);

	return bus->ops->gettxq(bus->dev);
}
/*
                                        
 */

extern bool brcmf_c_prec_enq(struct device *dev, struct pktq *q,
			 struct sk_buff *pkt, int prec);

/*                                                            */
extern void brcmf_rx_frames(struct device *dev, struct sk_buff_head *rxlist);

/*                                                                    */
extern int brcmf_attach(uint bus_hdrlen, struct device *dev);
/*                                                                */
extern void brcmf_detach(struct device *dev);
/*                                                        */
extern void brcmf_dev_reset(struct device *dev);
/*                                                         */
extern void brcmf_txflowblock(struct device *dev, bool state);

/*                                                          */
extern void brcmf_txcomplete(struct device *dev, struct sk_buff *txp,
			     bool success);

extern int brcmf_bus_start(struct device *dev);

#ifdef CONFIG_BRCMFMAC_SDIO
extern void brcmf_sdio_exit(void);
extern void brcmf_sdio_init(void);
extern void brcmf_sdio_register(void);
#endif
#ifdef CONFIG_BRCMFMAC_USB
extern void brcmf_usb_exit(void);
extern void brcmf_usb_register(void);
#endif

#endif				/*               */
