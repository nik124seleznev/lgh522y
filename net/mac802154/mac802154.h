/*
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
 *
 * Written by:
 * Pavel Smolenskiy <pavel.smolenskiy@gmail.com>
 * Maxim Gorbachyov <maxim.gorbachev@siemens.com>
 * Dmitry Eremin-Solenikov <dbaryshkov@gmail.com>
 * Alexander Smirnov <alex.bluesman.smirnov@gmail.com>
 */
#ifndef MAC802154_H
#define MAC802154_H

/*                               */
struct mac802154_priv {
	struct ieee802154_dev hw;
	struct ieee802154_ops *ops;

	/*                */
	struct wpan_phy *phy;

	int open_count;

	/*                                        
                     
                               
                       
   
                                                             
  */
	struct list_head	slaves;
	struct mutex		slaves_mtx;

	/*                                                                  
                       
  */
	struct workqueue_struct	*dev_workqueue;

	/*                                                                     
                                                                      
                                              
  */
	bool running;
};

#define	MAC802154_DEVICE_STOPPED	0x00
#define MAC802154_DEVICE_RUN		0x01

/*                            
  
                                                                        
                                                                      
                                                           
 */
struct mac802154_sub_if_data {
	struct list_head list; /*                                  */

	struct mac802154_priv *hw;
	struct net_device *dev;

	int type;

	spinlock_t mib_lock;

	__le16 pan_id;
	__le16 short_addr;

	u8 chan;
	u8 page;

	/*               */
	u8 bsn;
	/*               */
	u8 dsn;
};

#define mac802154_to_priv(_hw)	container_of(_hw, struct mac802154_priv, hw)

#define MAC802154_CHAN_NONE		0xff /*                        */

extern struct ieee802154_reduced_mlme_ops mac802154_mlme_reduced;
extern struct ieee802154_mlme_ops mac802154_mlme_wpan;

int mac802154_slave_open(struct net_device *dev);
int mac802154_slave_close(struct net_device *dev);

void mac802154_monitors_rx(struct mac802154_priv *priv, struct sk_buff *skb);
void mac802154_monitor_setup(struct net_device *dev);

void mac802154_wpans_rx(struct mac802154_priv *priv, struct sk_buff *skb);
void mac802154_wpan_setup(struct net_device *dev);

netdev_tx_t mac802154_tx(struct mac802154_priv *priv, struct sk_buff *skb,
			 u8 page, u8 chan);

/*               */
void mac802154_dev_set_short_addr(struct net_device *dev, u16 val);
u16 mac802154_dev_get_short_addr(const struct net_device *dev);
void mac802154_dev_set_ieee_addr(struct net_device *dev);
u16 mac802154_dev_get_pan_id(const struct net_device *dev);
void mac802154_dev_set_pan_id(struct net_device *dev, u16 val);
void mac802154_dev_set_page_channel(struct net_device *dev, u8 page, u8 chan);
u8 mac802154_dev_get_dsn(const struct net_device *dev);

#endif /*             */
