/*
 * IEEE802.15.4-2003 specification
 *
 * Copyright (C) 2007-2012 Siemens AG
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#ifndef NET_MAC802154_H
#define NET_MAC802154_H

#include <net/af_ieee802154.h>

/*                          
                          
                            
                              
                                      
 */
#define MAC802154_FRAME_HARD_HEADER_LEN		(2 + 1 + 20 + 14)

/*                                                                       
                             
 */

/*                                          */
#define IEEE802515_AFILT_SADDR_CHANGED		0x00000001
/*                                         */
#define IEEE802515_AFILT_IEEEADDR_CHANGED	0x00000002
/*                                   */
#define IEEE802515_AFILT_PANID_CHANGED		0x00000004
/*                                               */
#define	IEEE802515_AFILT_PANC_CHANGED		0x00000008

struct ieee802154_hw_addr_filt {
	__le16	pan_id;		/*                                      
                                                   
                                                   
                                                  
                                           
     */
	__le16	short_addr;
	u8	ieee_addr[IEEE802154_ADDR_LEN];
	u8	pan_coord;
};

struct ieee802154_dev {
	/*                      */
	int	extra_tx_headroom;
	u32	flags;
	struct	device *parent;

	/*                          */
	struct	ieee802154_hw_addr_filt hw_filt;
	void	*priv;
	struct	wpan_phy *phy;
};

/*                                                     
  
                                                                      
                                                             
                                                                
                                                                
                                                               
 */

/*                                                                         */
#define	IEEE802154_HW_OMIT_CKSUM	0x00000001
/*                                                           */
#define	IEEE802154_HW_AACK		0x00000002

/*                                                               
  
                                                                
                                                                 
           
  
                                                                       
                                                                    
  
                                                                
                                                                 
  
                                                                        
                                                                   
                                                                  
                   
                                                                    
                   
  
                                                                  
                                                             
                                                               
                                                              
  
               
                                                  
                                                       
                                                                       
  
                    
                                                 
                                                       
                                            
 */
struct ieee802154_ops {
	struct module	*owner;
	int		(*start)(struct ieee802154_dev *dev);
	void		(*stop)(struct ieee802154_dev *dev);
	int		(*xmit)(struct ieee802154_dev *dev,
				struct sk_buff *skb);
	int		(*ed)(struct ieee802154_dev *dev, u8 *level);
	int		(*set_channel)(struct ieee802154_dev *dev,
				       int page,
				       int channel);
	int		(*set_hw_addr_filt)(struct ieee802154_dev *dev,
					  struct ieee802154_hw_addr_filt *filt,
					    unsigned long changed);
	int		(*ieee_addr)(struct ieee802154_dev *dev,
				     u8 addr[IEEE802154_ADDR_LEN]);
};

/*                                               */
struct ieee802154_dev *
ieee802154_alloc_device(size_t priv_data_lex, struct ieee802154_ops *ops);
void ieee802154_free_device(struct ieee802154_dev *dev);
int ieee802154_register_device(struct ieee802154_dev *dev);
void ieee802154_unregister_device(struct ieee802154_dev *dev);

void ieee802154_rx_irqsafe(struct ieee802154_dev *dev, struct sk_buff *skb,
			   u8 lqi);

#endif /*                 */
