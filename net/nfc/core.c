/*
 * Copyright (C) 2011 Instituto Nokia de Tecnologia
 *
 * Authors:
 *    Lauro Ramos Venancio <lauro.venancio@openbossa.org>
 *    Aloisio Almeida Jr <aloisio.almeida@openbossa.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the
 * Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": %s: " fmt, __func__

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/rfkill.h>
#include <linux/nfc.h>

#include <net/genetlink.h>

#include "nfc.h"

#define VERSION "0.1"

#define NFC_CHECK_PRES_FREQ_MS	2000

int nfc_devlist_generation;
DEFINE_MUTEX(nfc_devlist_mutex);

/*                      */
static DEFINE_IDA(nfc_index_ida);

/* 
                                      
  
                                       
  
                                                                   
 */
int nfc_dev_up(struct nfc_dev *dev)
{
	int rc = 0;

	pr_debug("dev_name=%s\n", dev_name(&dev->dev));

	device_lock(&dev->dev);

	if (dev->rfkill && rfkill_blocked(dev->rfkill)) {
		rc = -ERFKILL;
		goto error;
	}

	if (!device_is_registered(&dev->dev)) {
		rc = -ENODEV;
		goto error;
	}

	if (dev->dev_up) {
		rc = -EALREADY;
		goto error;
	}

	if (dev->ops->dev_up)
		rc = dev->ops->dev_up(dev);

	if (!rc)
		dev->dev_up = true;

error:
	device_unlock(&dev->dev);
	return rc;
}

/* 
                                         
  
                                        
 */
int nfc_dev_down(struct nfc_dev *dev)
{
	int rc = 0;

	pr_debug("dev_name=%s\n", dev_name(&dev->dev));

	device_lock(&dev->dev);

	if (!device_is_registered(&dev->dev)) {
		rc = -ENODEV;
		goto error;
	}

	if (!dev->dev_up) {
		rc = -EALREADY;
		goto error;
	}

	if (dev->polling || dev->active_target) {
		rc = -EBUSY;
		goto error;
	}

	if (dev->ops->dev_down)
		dev->ops->dev_down(dev);

	dev->dev_up = false;

error:
	device_unlock(&dev->dev);
	return rc;
}

static int nfc_rfkill_set_block(void *data, bool blocked)
{
	struct nfc_dev *dev = data;

	pr_debug("%s blocked %d", dev_name(&dev->dev), blocked);

	if (!blocked)
		return 0;

	nfc_dev_down(dev);

	return 0;
}

static const struct rfkill_ops nfc_rfkill_ops = {
	.set_block = nfc_rfkill_set_block,
};

/* 
                                                 
  
                                               
                                                                    
  
                                                                    
                                        
 */
int nfc_start_poll(struct nfc_dev *dev, u32 im_protocols, u32 tm_protocols)
{
	int rc;

	pr_debug("dev_name %s initiator protocols 0x%x target protocols 0x%x\n",
		 dev_name(&dev->dev), im_protocols, tm_protocols);

	if (!im_protocols && !tm_protocols)
		return -EINVAL;

	device_lock(&dev->dev);

	if (!device_is_registered(&dev->dev)) {
		rc = -ENODEV;
		goto error;
	}

	if (!dev->dev_up) {
		rc = -ENODEV;
		goto error;
	}

	if (dev->polling) {
		rc = -EBUSY;
		goto error;
	}

	rc = dev->ops->start_poll(dev, im_protocols, tm_protocols);
	if (!rc) {
		dev->polling = true;
		dev->rf_mode = NFC_RF_NONE;
	}

error:
	device_unlock(&dev->dev);
	return rc;
}

/* 
                                               
  
                                              
 */
int nfc_stop_poll(struct nfc_dev *dev)
{
	int rc = 0;

	pr_debug("dev_name=%s\n", dev_name(&dev->dev));

	device_lock(&dev->dev);

	if (!device_is_registered(&dev->dev)) {
		rc = -ENODEV;
		goto error;
	}

	if (!dev->polling) {
		rc = -EINVAL;
		goto error;
	}

	dev->ops->stop_poll(dev);
	dev->polling = false;
	dev->rf_mode = NFC_RF_NONE;

error:
	device_unlock(&dev->dev);
	return rc;
}

static struct nfc_target *nfc_find_target(struct nfc_dev *dev, u32 target_idx)
{
	int i;

	if (dev->n_targets == 0)
		return NULL;

	for (i = 0; i < dev->n_targets; i++) {
		if (dev->targets[i].idx == target_idx)
			return &dev->targets[i];
	}

	return NULL;
}

int nfc_dep_link_up(struct nfc_dev *dev, int target_index, u8 comm_mode)
{
	int rc = 0;
	u8 *gb;
	size_t gb_len;
	struct nfc_target *target;

	pr_debug("dev_name=%s comm %d\n", dev_name(&dev->dev), comm_mode);

	if (!dev->ops->dep_link_up)
		return -EOPNOTSUPP;

	device_lock(&dev->dev);

	if (!device_is_registered(&dev->dev)) {
		rc = -ENODEV;
		goto error;
	}

	if (dev->dep_link_up == true) {
		rc = -EALREADY;
		goto error;
	}

	gb = nfc_llcp_general_bytes(dev, &gb_len);
	if (gb_len > NFC_MAX_GT_LEN) {
		rc = -EINVAL;
		goto error;
	}

	target = nfc_find_target(dev, target_index);
	if (target == NULL) {
		rc = -ENOTCONN;
		goto error;
	}

	rc = dev->ops->dep_link_up(dev, target, comm_mode, gb, gb_len);
	if (!rc) {
		dev->active_target = target;
		dev->rf_mode = NFC_RF_INITIATOR;
	}

error:
	device_unlock(&dev->dev);
	return rc;
}

int nfc_dep_link_down(struct nfc_dev *dev)
{
	int rc = 0;

	pr_debug("dev_name=%s\n", dev_name(&dev->dev));

	if (!dev->ops->dep_link_down)
		return -EOPNOTSUPP;

	device_lock(&dev->dev);

	if (!device_is_registered(&dev->dev)) {
		rc = -ENODEV;
		goto error;
	}

	if (dev->dep_link_up == false) {
		rc = -EALREADY;
		goto error;
	}

	rc = dev->ops->dep_link_down(dev);
	if (!rc) {
		dev->dep_link_up = false;
		dev->active_target = NULL;
		dev->rf_mode = NFC_RF_NONE;
		nfc_llcp_mac_is_down(dev);
		nfc_genl_dep_link_down_event(dev);
	}

error:
	device_unlock(&dev->dev);

	return rc;
}

int nfc_dep_link_is_up(struct nfc_dev *dev, u32 target_idx,
		       u8 comm_mode, u8 rf_mode)
{
	dev->dep_link_up = true;

	nfc_llcp_mac_is_up(dev, target_idx, comm_mode, rf_mode);

	return nfc_genl_dep_link_up_event(dev, target_idx, comm_mode, rf_mode);
}
EXPORT_SYMBOL(nfc_dep_link_is_up);

/* 
                                                             
  
                                             
                                                          
                                                              
 */
int nfc_activate_target(struct nfc_dev *dev, u32 target_idx, u32 protocol)
{
	int rc;
	struct nfc_target *target;

	pr_debug("dev_name=%s target_idx=%u protocol=%u\n",
		 dev_name(&dev->dev), target_idx, protocol);

	device_lock(&dev->dev);

	if (!device_is_registered(&dev->dev)) {
		rc = -ENODEV;
		goto error;
	}

	if (dev->active_target) {
		rc = -EBUSY;
		goto error;
	}

	target = nfc_find_target(dev, target_idx);
	if (target == NULL) {
		rc = -ENOTCONN;
		goto error;
	}

	rc = dev->ops->activate_target(dev, target, protocol);
	if (!rc) {
		dev->active_target = target;
		dev->rf_mode = NFC_RF_INITIATOR;

		if (dev->ops->check_presence && !dev->shutting_down)
			mod_timer(&dev->check_pres_timer, jiffies +
				  msecs_to_jiffies(NFC_CHECK_PRES_FREQ_MS));
	}

error:
	device_unlock(&dev->dev);
	return rc;
}

/* 
                                                  
  
                                             
                                                            
 */
int nfc_deactivate_target(struct nfc_dev *dev, u32 target_idx)
{
	int rc = 0;

	pr_debug("dev_name=%s target_idx=%u\n",
		 dev_name(&dev->dev), target_idx);

	device_lock(&dev->dev);

	if (!device_is_registered(&dev->dev)) {
		rc = -ENODEV;
		goto error;
	}

	if (dev->active_target == NULL) {
		rc = -ENOTCONN;
		goto error;
	}

	if (dev->active_target->idx != target_idx) {
		rc = -ENOTCONN;
		goto error;
	}

	if (dev->ops->check_presence)
		del_timer_sync(&dev->check_pres_timer);

	dev->ops->deactivate_target(dev, dev->active_target);
	dev->active_target = NULL;

error:
	device_unlock(&dev->dev);
	return rc;
}

/* 
                                      
  
                                             
                                   
                        
                                                     
                                                   
  
                                                                          
 */
int nfc_data_exchange(struct nfc_dev *dev, u32 target_idx, struct sk_buff *skb,
		      data_exchange_cb_t cb, void *cb_context)
{
	int rc;

	pr_debug("dev_name=%s target_idx=%u skb->len=%u\n",
		 dev_name(&dev->dev), target_idx, skb->len);

	device_lock(&dev->dev);

	if (!device_is_registered(&dev->dev)) {
		rc = -ENODEV;
		kfree_skb(skb);
		goto error;
	}

	if (dev->rf_mode == NFC_RF_INITIATOR && dev->active_target != NULL) {
		if (dev->active_target->idx != target_idx) {
			rc = -EADDRNOTAVAIL;
			kfree_skb(skb);
			goto error;
		}

		if (dev->ops->check_presence)
			del_timer_sync(&dev->check_pres_timer);

		rc = dev->ops->im_transceive(dev, dev->active_target, skb, cb,
					     cb_context);

		if (!rc && dev->ops->check_presence && !dev->shutting_down)
			mod_timer(&dev->check_pres_timer, jiffies +
				  msecs_to_jiffies(NFC_CHECK_PRES_FREQ_MS));
	} else if (dev->rf_mode == NFC_RF_TARGET && dev->ops->tm_send != NULL) {
		rc = dev->ops->tm_send(dev, skb);
	} else {
		rc = -ENOTCONN;
		kfree_skb(skb);
		goto error;
	}


error:
	device_unlock(&dev->dev);
	return rc;
}

int nfc_set_remote_general_bytes(struct nfc_dev *dev, u8 *gb, u8 gb_len)
{
	pr_debug("dev_name=%s gb_len=%d\n", dev_name(&dev->dev), gb_len);

	if (gb_len > NFC_MAX_GT_LEN)
		return -EINVAL;

	return nfc_llcp_set_remote_gb(dev, gb, gb_len);
}
EXPORT_SYMBOL(nfc_set_remote_general_bytes);

u8 *nfc_get_local_general_bytes(struct nfc_dev *dev, size_t *gb_len)
{
	pr_debug("dev_name=%s\n", dev_name(&dev->dev));

	return nfc_llcp_general_bytes(dev, gb_len);
}
EXPORT_SYMBOL(nfc_get_local_general_bytes);

int nfc_tm_data_received(struct nfc_dev *dev, struct sk_buff *skb)
{
	/*                               */
	if (dev->dep_link_up == false) {
		kfree_skb(skb);
		return -ENOLINK;
	}

	return nfc_llcp_data_received(dev, skb);
}
EXPORT_SYMBOL(nfc_tm_data_received);

int nfc_tm_activated(struct nfc_dev *dev, u32 protocol, u8 comm_mode,
		     u8 *gb, size_t gb_len)
{
	int rc;

	device_lock(&dev->dev);

	dev->polling = false;

	if (gb != NULL) {
		rc = nfc_set_remote_general_bytes(dev, gb, gb_len);
		if (rc < 0)
			goto out;
	}

	dev->rf_mode = NFC_RF_TARGET;

	if (protocol == NFC_PROTO_NFC_DEP_MASK)
		nfc_dep_link_is_up(dev, 0, comm_mode, NFC_RF_TARGET);

	rc = nfc_genl_tm_activated(dev, protocol);

out:
	device_unlock(&dev->dev);

	return rc;
}
EXPORT_SYMBOL(nfc_tm_activated);

int nfc_tm_deactivated(struct nfc_dev *dev)
{
	dev->dep_link_up = false;
	dev->rf_mode = NFC_RF_NONE;

	return nfc_genl_tm_deactivated(dev);
}
EXPORT_SYMBOL(nfc_tm_deactivated);

/* 
                                                                  
  
                          
                  
 */
struct sk_buff *nfc_alloc_send_skb(struct nfc_dev *dev, struct sock *sk,
				   unsigned int flags, unsigned int size,
				   unsigned int *err)
{
	struct sk_buff *skb;
	unsigned int total_size;

	total_size = size +
		dev->tx_headroom + dev->tx_tailroom + NFC_HEADER_SIZE;

	skb = sock_alloc_send_skb(sk, total_size, flags & MSG_DONTWAIT, err);
	if (skb)
		skb_reserve(skb, dev->tx_headroom + NFC_HEADER_SIZE);

	return skb;
}

/* 
                                                                  
  
                          
                  
 */
struct sk_buff *nfc_alloc_recv_skb(unsigned int size, gfp_t gfp)
{
	struct sk_buff *skb;
	unsigned int total_size;

	total_size = size + 1;
	skb = alloc_skb(total_size, gfp);

	if (skb)
		skb_reserve(skb, 1);

	return skb;
}
EXPORT_SYMBOL(nfc_alloc_recv_skb);

/* 
                                                     
  
                                              
                                       
                                
  
                                                                         
                                                                      
                       
                                                                         
                                                                             
                                                                      
                                                                            
                                                               
 */
int nfc_targets_found(struct nfc_dev *dev,
		      struct nfc_target *targets, int n_targets)
{
	int i;

	pr_debug("dev_name=%s n_targets=%d\n", dev_name(&dev->dev), n_targets);

	for (i = 0; i < n_targets; i++)
		targets[i].idx = dev->target_next_idx++;

	device_lock(&dev->dev);

	if (dev->polling == false) {
		device_unlock(&dev->dev);
		return 0;
	}

	dev->polling = false;

	dev->targets_generation++;

	kfree(dev->targets);
	dev->targets = NULL;

	if (targets) {
		dev->targets = kmemdup(targets,
				       n_targets * sizeof(struct nfc_target),
				       GFP_ATOMIC);

		if (!dev->targets) {
			dev->n_targets = 0;
			device_unlock(&dev->dev);
			return -ENOMEM;
		}
	}

	dev->n_targets = n_targets;
	device_unlock(&dev->dev);

	nfc_genl_targets_found(dev);

	return 0;
}
EXPORT_SYMBOL(nfc_targets_found);

/* 
                                                                      
  
                                                              
                                           
  
                                                                      
                         
                                                                      
                                                                            
                                                               
 */
int nfc_target_lost(struct nfc_dev *dev, u32 target_idx)
{
	struct nfc_target *tg;
	int i;

	pr_debug("dev_name %s n_target %d\n", dev_name(&dev->dev), target_idx);

	device_lock(&dev->dev);

	for (i = 0; i < dev->n_targets; i++) {
		tg = &dev->targets[i];
		if (tg->idx == target_idx)
			break;
	}

	if (i == dev->n_targets) {
		device_unlock(&dev->dev);
		return -EINVAL;
	}

	dev->targets_generation++;
	dev->n_targets--;
	dev->active_target = NULL;

	if (dev->n_targets) {
		memcpy(&dev->targets[i], &dev->targets[i + 1],
		       (dev->n_targets - i) * sizeof(struct nfc_target));
	} else {
		kfree(dev->targets);
		dev->targets = NULL;
	}

	device_unlock(&dev->dev);

	nfc_genl_target_lost(dev, target_idx);

	return 0;
}
EXPORT_SYMBOL(nfc_target_lost);

inline void nfc_driver_failure(struct nfc_dev *dev, int err)
{
	nfc_targets_found(dev, NULL, 0);
}
EXPORT_SYMBOL(nfc_driver_failure);

static void nfc_release(struct device *d)
{
	struct nfc_dev *dev = to_nfc_dev(d);

	pr_debug("dev_name=%s\n", dev_name(&dev->dev));

	nfc_genl_data_exit(&dev->genl_data);
	kfree(dev->targets);
	kfree(dev);
}

static void nfc_check_pres_work(struct work_struct *work)
{
	struct nfc_dev *dev = container_of(work, struct nfc_dev,
					   check_pres_work);
	int rc;

	device_lock(&dev->dev);

	if (dev->active_target && timer_pending(&dev->check_pres_timer) == 0) {
		rc = dev->ops->check_presence(dev, dev->active_target);
		if (rc == -EOPNOTSUPP)
			goto exit;
		if (rc) {
			u32 active_target_idx = dev->active_target->idx;
			device_unlock(&dev->dev);
			nfc_target_lost(dev, active_target_idx);
			return;
		}

		if (!dev->shutting_down)
			mod_timer(&dev->check_pres_timer, jiffies +
				  msecs_to_jiffies(NFC_CHECK_PRES_FREQ_MS));
	}

exit:
	device_unlock(&dev->dev);
}

static void nfc_check_pres_timeout(unsigned long data)
{
	struct nfc_dev *dev = (struct nfc_dev *)data;

	schedule_work(&dev->check_pres_work);
}

struct class nfc_class = {
	.name = "nfc",
	.dev_release = nfc_release,
};
EXPORT_SYMBOL(nfc_class);

static int match_idx(struct device *d, const void *data)
{
	struct nfc_dev *dev = to_nfc_dev(d);
	const unsigned int *idx = data;

	return dev->idx == *idx;
}

struct nfc_dev *nfc_get_device(unsigned int idx)
{
	struct device *d;

	d = class_find_device(&nfc_class, NULL, &idx, match_idx);
	if (!d)
		return NULL;

	return to_nfc_dev(d);
}

/* 
                                                  
  
                          
                                                              
 */
struct nfc_dev *nfc_allocate_device(struct nfc_ops *ops,
				    u32 supported_protocols,
				    u32 supported_se,
				    int tx_headroom, int tx_tailroom)
{
	struct nfc_dev *dev;

	if (!ops->start_poll || !ops->stop_poll || !ops->activate_target ||
	    !ops->deactivate_target || !ops->im_transceive)
		return NULL;

	if (!supported_protocols)
		return NULL;

	dev = kzalloc(sizeof(struct nfc_dev), GFP_KERNEL);
	if (!dev)
		return NULL;

	dev->ops = ops;
	dev->supported_protocols = supported_protocols;
	dev->supported_se = supported_se;
	dev->active_se = NFC_SE_NONE;
	dev->tx_headroom = tx_headroom;
	dev->tx_tailroom = tx_tailroom;

	nfc_genl_data_init(&dev->genl_data);

	dev->rf_mode = NFC_RF_NONE;

	/*                                */
	dev->targets_generation = 1;

	if (ops->check_presence) {
		init_timer(&dev->check_pres_timer);
		dev->check_pres_timer.data = (unsigned long)dev;
		dev->check_pres_timer.function = nfc_check_pres_timeout;

		INIT_WORK(&dev->check_pres_work, nfc_check_pres_work);
	}

	return dev;
}
EXPORT_SYMBOL(nfc_allocate_device);

/* 
                                                                   
  
                                   
 */
int nfc_register_device(struct nfc_dev *dev)
{
	int rc;

	pr_debug("dev_name=%s\n", dev_name(&dev->dev));

	dev->idx = ida_simple_get(&nfc_index_ida, 0, 0, GFP_KERNEL);
	if (dev->idx < 0)
		return dev->idx;

	dev->dev.class = &nfc_class;
	dev_set_name(&dev->dev, "nfc%d", dev->idx);
	device_initialize(&dev->dev);

	mutex_lock(&nfc_devlist_mutex);
	nfc_devlist_generation++;
	rc = device_add(&dev->dev);
	mutex_unlock(&nfc_devlist_mutex);

	if (rc < 0)
		return rc;

	rc = nfc_llcp_register_device(dev);
	if (rc)
		pr_err("Could not register llcp device\n");

	rc = nfc_genl_device_added(dev);
	if (rc)
		pr_debug("The userspace won't be notified that the device %s was added\n",
			 dev_name(&dev->dev));

	dev->rfkill = rfkill_alloc(dev_name(&dev->dev), &dev->dev,
				   RFKILL_TYPE_NFC, &nfc_rfkill_ops, dev);
	if (dev->rfkill) {
		if (rfkill_register(dev->rfkill) < 0) {
			rfkill_destroy(dev->rfkill);
			dev->rfkill = NULL;
		}
	}

	return 0;
}
EXPORT_SYMBOL(nfc_register_device);

/* 
                                                                       
  
                                     
 */
void nfc_unregister_device(struct nfc_dev *dev)
{
	int rc, id;

	pr_debug("dev_name=%s\n", dev_name(&dev->dev));

	id = dev->idx;

	if (dev->rfkill) {
		rfkill_unregister(dev->rfkill);
		rfkill_destroy(dev->rfkill);
	}

	if (dev->ops->check_presence) {
		device_lock(&dev->dev);
		dev->shutting_down = true;
		device_unlock(&dev->dev);
		del_timer_sync(&dev->check_pres_timer);
		cancel_work_sync(&dev->check_pres_work);
	}

	rc = nfc_genl_device_removed(dev);
	if (rc)
		pr_debug("The userspace won't be notified that the device %s "
			 "was removed\n", dev_name(&dev->dev));

	nfc_llcp_unregister_device(dev);

	mutex_lock(&nfc_devlist_mutex);
	nfc_devlist_generation++;
	device_del(&dev->dev);
	mutex_unlock(&nfc_devlist_mutex);

	ida_simple_remove(&nfc_index_ida, id);
}
EXPORT_SYMBOL(nfc_unregister_device);

static int __init nfc_init(void)
{
	int rc;

	pr_info("NFC Core ver %s\n", VERSION);

	rc = class_register(&nfc_class);
	if (rc)
		return rc;

	rc = nfc_genl_init();
	if (rc)
		goto err_genl;

	/*                                    */
	nfc_devlist_generation = 1;

	rc = rawsock_init();
	if (rc)
		goto err_rawsock;

	rc = nfc_llcp_init();
	if (rc)
		goto err_llcp_sock;

	rc = af_nfc_init();
	if (rc)
		goto err_af_nfc;

	return 0;

err_af_nfc:
	nfc_llcp_exit();
err_llcp_sock:
	rawsock_exit();
err_rawsock:
	nfc_genl_exit();
err_genl:
	class_unregister(&nfc_class);
	return rc;
}

static void __exit nfc_exit(void)
{
	af_nfc_exit();
	nfc_llcp_exit();
	rawsock_exit();
	nfc_genl_exit();
	class_unregister(&nfc_class);
}

subsys_initcall(nfc_init);
module_exit(nfc_exit);

MODULE_AUTHOR("Lauro Ramos Venancio <lauro.venancio@openbossa.org>");
MODULE_DESCRIPTION("NFC Core ver " VERSION);
MODULE_VERSION(VERSION);
MODULE_LICENSE("GPL");
MODULE_ALIAS_NETPROTO(PF_NFC);
MODULE_ALIAS_GENL_FAMILY(NFC_GENL_NAME);