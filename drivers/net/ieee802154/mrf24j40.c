/*
 * Driver for Microchip MRF24J40 802.15.4 Wireless-PAN Networking controller
 *
 * Copyright (C) 2012 Alan Ott <alan@signal11.us>
 *                    Signal 11 Software
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/spi/spi.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/pinctrl/consumer.h>
#include <net/wpan-phy.h>
#include <net/mac802154.h>
#include <net/ieee802154.h>

/*                                  */
#define REG_RXMCR    0x00  /*                     */
#define REG_PANIDL   0x01  /*              */
#define REG_PANIDH   0x02  /*               */
#define REG_SADRL    0x03  /*                     */
#define REG_SADRH    0x04  /*                      */
#define REG_EADR0    0x05  /*                                    */
#define REG_TXMCR    0x11  /*                      */
#define REG_PACON0   0x16  /*                         */
#define REG_PACON1   0x17  /*                         */
#define REG_PACON2   0x18  /*                         */
#define REG_TXNCON   0x1B  /*                              */
#define REG_TXSTAT   0x24  /*                        */
#define REG_SOFTRST  0x2A  /*            */
#define REG_TXSTBL   0x2E  /*                  */
#define REG_INTSTAT  0x31  /*                  */
#define REG_INTCON   0x32  /*                   */
#define REG_RFCTL    0x36  /*                          */
#define REG_BBREG1   0x39  /*                    */
#define REG_BBREG2   0x3A  /* */
#define REG_BBREG6   0x3E  /* */
#define REG_CCAEDTH  0x3F  /*                            */

/*                                 */
#define REG_RFCON0     0x200  /*                      */
#define REG_RFCON1     0x201
#define REG_RFCON2     0x202
#define REG_RFCON3     0x203
#define REG_RFCON5     0x205
#define REG_RFCON6     0x206
#define REG_RFCON7     0x207
#define REG_RFCON8     0x208
#define REG_RSSI       0x210
#define REG_SLPCON0    0x211  /*                               */
#define REG_SLPCON1    0x220
#define REG_WAKETIMEL  0x222  /*                              */
#define REG_WAKETIMEH  0x223  /*                               */
#define REG_RX_FIFO    0x300  /*              */

/*                                                                    */
#define MRF24J40_CHAN_MIN 11
#define MRF24J40_CHAN_MAX 26
#define CHANNEL_MASK (((u32)1 << (MRF24J40_CHAN_MAX + 1)) \
		      - ((u32)1 << MRF24J40_CHAN_MIN))

#define TX_FIFO_SIZE 128 /*                */
#define RX_FIFO_SIZE 144 /*                */
#define SET_CHANNEL_DELAY_US 192 /*                */

/*                     */
struct mrf24j40 {
	struct spi_device *spi;
	struct ieee802154_dev *dev;

	struct mutex buffer_mutex; /*                          */
	struct completion tx_complete;
	struct work_struct irqwork;
	u8 *buf; /*                                                  */
};

/*                                                               */
#define MRF24J40_READSHORT(reg) ((reg) << 1)
#define MRF24J40_WRITESHORT(reg) ((reg) << 1 | 1)
#define MRF24J40_READLONG(reg) (1 << 15 | (reg) << 5)
#define MRF24J40_WRITELONG(reg) (1 << 15 | (reg) << 5 | 1 << 4)

/*                                                                     */
#define MAX_SPI_SPEED_HZ 10000000

#define printdev(X) (&X->spi->dev)

static int write_short_reg(struct mrf24j40 *devrec, u8 reg, u8 value)
{
	int ret;
	struct spi_message msg;
	struct spi_transfer xfer = {
		.len = 2,
		.tx_buf = devrec->buf,
		.rx_buf = devrec->buf,
	};

	spi_message_init(&msg);
	spi_message_add_tail(&xfer, &msg);

	mutex_lock(&devrec->buffer_mutex);
	devrec->buf[0] = MRF24J40_WRITESHORT(reg);
	devrec->buf[1] = value;

	ret = spi_sync(devrec->spi, &msg);
	if (ret)
		dev_err(printdev(devrec),
			"SPI write Failed for short register 0x%hhx\n", reg);

	mutex_unlock(&devrec->buffer_mutex);
	return ret;
}

static int read_short_reg(struct mrf24j40 *devrec, u8 reg, u8 *val)
{
	int ret = -1;
	struct spi_message msg;
	struct spi_transfer xfer = {
		.len = 2,
		.tx_buf = devrec->buf,
		.rx_buf = devrec->buf,
	};

	spi_message_init(&msg);
	spi_message_add_tail(&xfer, &msg);

	mutex_lock(&devrec->buffer_mutex);
	devrec->buf[0] = MRF24J40_READSHORT(reg);
	devrec->buf[1] = 0;

	ret = spi_sync(devrec->spi, &msg);
	if (ret)
		dev_err(printdev(devrec),
			"SPI read Failed for short register 0x%hhx\n", reg);
	else
		*val = devrec->buf[1];

	mutex_unlock(&devrec->buffer_mutex);
	return ret;
}

static int read_long_reg(struct mrf24j40 *devrec, u16 reg, u8 *value)
{
	int ret;
	u16 cmd;
	struct spi_message msg;
	struct spi_transfer xfer = {
		.len = 3,
		.tx_buf = devrec->buf,
		.rx_buf = devrec->buf,
	};

	spi_message_init(&msg);
	spi_message_add_tail(&xfer, &msg);

	cmd = MRF24J40_READLONG(reg);
	mutex_lock(&devrec->buffer_mutex);
	devrec->buf[0] = cmd >> 8 & 0xff;
	devrec->buf[1] = cmd & 0xff;
	devrec->buf[2] = 0;

	ret = spi_sync(devrec->spi, &msg);
	if (ret)
		dev_err(printdev(devrec),
			"SPI read Failed for long register 0x%hx\n", reg);
	else
		*value = devrec->buf[2];

	mutex_unlock(&devrec->buffer_mutex);
	return ret;
}

static int write_long_reg(struct mrf24j40 *devrec, u16 reg, u8 val)
{
	int ret;
	u16 cmd;
	struct spi_message msg;
	struct spi_transfer xfer = {
		.len = 3,
		.tx_buf = devrec->buf,
		.rx_buf = devrec->buf,
	};

	spi_message_init(&msg);
	spi_message_add_tail(&xfer, &msg);

	cmd = MRF24J40_WRITELONG(reg);
	mutex_lock(&devrec->buffer_mutex);
	devrec->buf[0] = cmd >> 8 & 0xff;
	devrec->buf[1] = cmd & 0xff;
	devrec->buf[2] = val;

	ret = spi_sync(devrec->spi, &msg);
	if (ret)
		dev_err(printdev(devrec),
			"SPI write Failed for long register 0x%hx\n", reg);

	mutex_unlock(&devrec->buffer_mutex);
	return ret;
}

/*                                                                           
                                                                           
                                                                    */
static int write_tx_buf(struct mrf24j40 *devrec, u16 reg,
			const u8 *data, size_t length)
{
	int ret;
	u16 cmd;
	u8 lengths[2];
	struct spi_message msg;
	struct spi_transfer addr_xfer = {
		.len = 2,
		.tx_buf = devrec->buf,
	};
	struct spi_transfer lengths_xfer = {
		.len = 2,
		.tx_buf = &lengths, /*                                       */
	};
	struct spi_transfer data_xfer = {
		.len = length,
		.tx_buf = data,
	};

	/*                                                                */
	if (length > TX_FIFO_SIZE-2) {
		dev_err(printdev(devrec), "write_tx_buf() was passed too large a buffer. Performing short write.\n");
		length = TX_FIFO_SIZE-2;
	}

	spi_message_init(&msg);
	spi_message_add_tail(&addr_xfer, &msg);
	spi_message_add_tail(&lengths_xfer, &msg);
	spi_message_add_tail(&data_xfer, &msg);

	cmd = MRF24J40_WRITELONG(reg);
	mutex_lock(&devrec->buffer_mutex);
	devrec->buf[0] = cmd >> 8 & 0xff;
	devrec->buf[1] = cmd & 0xff;
	lengths[0] = 0x0; /*                                       */
	lengths[1] = length; /*              */

	ret = spi_sync(devrec->spi, &msg);
	if (ret)
		dev_err(printdev(devrec), "SPI write Failed for TX buf\n");

	mutex_unlock(&devrec->buffer_mutex);
	return ret;
}

static int mrf24j40_read_rx_buf(struct mrf24j40 *devrec,
				u8 *data, u8 *len, u8 *lqi)
{
	u8 rx_len;
	u8 addr[2];
	u8 lqi_rssi[2];
	u16 cmd;
	int ret;
	struct spi_message msg;
	struct spi_transfer addr_xfer = {
		.len = 2,
		.tx_buf = &addr,
	};
	struct spi_transfer data_xfer = {
		.len = 0x0, /*           */
		.rx_buf = data,
	};
	struct spi_transfer status_xfer = {
		.len = 2,
		.rx_buf = &lqi_rssi,
	};

	/*                                                              
                                                               */
	ret = read_long_reg(devrec, REG_RX_FIFO, &rx_len);
	if (ret)
		goto out;

	/*                                                            
                                  */
	if (rx_len > RX_FIFO_SIZE-1) {
		dev_err(printdev(devrec), "Invalid length read from device. Performing short read.\n");
		rx_len = RX_FIFO_SIZE-1;
	}

	if (rx_len > *len) {
		/*                                                          */
		dev_err(printdev(devrec), "Buffer not big enough. Performing short read\n");
		rx_len = *len;
	}

	/*                                       */
	cmd = MRF24J40_READLONG(REG_RX_FIFO+1);
	addr[0] = cmd >> 8 & 0xff;
	addr[1] = cmd & 0xff;
	data_xfer.len = rx_len;

	spi_message_init(&msg);
	spi_message_add_tail(&addr_xfer, &msg);
	spi_message_add_tail(&data_xfer, &msg);
	spi_message_add_tail(&status_xfer, &msg);

	ret = spi_sync(devrec->spi, &msg);
	if (ret) {
		dev_err(printdev(devrec), "SPI RX Buffer Read Failed.\n");
		goto out;
	}

	*lqi = lqi_rssi[0];
	*len = rx_len;

#ifdef DEBUG
	print_hex_dump(KERN_DEBUG, "mrf24j40 rx: ",
		DUMP_PREFIX_OFFSET, 16, 1, data, *len, 0);
	printk(KERN_DEBUG "mrf24j40 rx: lqi: %02hhx rssi: %02hhx\n",
		lqi_rssi[0], lqi_rssi[1]);
#endif

out:
	return ret;
}

static int mrf24j40_tx(struct ieee802154_dev *dev, struct sk_buff *skb)
{
	struct mrf24j40 *devrec = dev->priv;
	u8 val;
	int ret = 0;

	dev_dbg(printdev(devrec), "tx packet of %d bytes\n", skb->len);

	ret = write_tx_buf(devrec, 0x000, skb->data, skb->len);
	if (ret)
		goto err;

	/*                                          */
	ret = read_short_reg(devrec, REG_TXNCON, &val);
	if (ret)
		goto err;
	val |= 0x1;
	/*                                                    */
	if (skb->data[0] & IEEE802154_FC_ACK_REQ)
		val |= 0x4;
	write_short_reg(devrec, REG_TXNCON, val);

	INIT_COMPLETION(devrec->tx_complete);

	/*                                                        */
	ret = wait_for_completion_interruptible_timeout(
						&devrec->tx_complete,
						5 * HZ);
	if (ret == -ERESTARTSYS)
		goto err;
	if (ret == 0) {
		dev_warn(printdev(devrec), "Timeout waiting for TX interrupt\n");
		ret = -ETIMEDOUT;
		goto err;
	}

	/*                                       */
	ret = read_short_reg(devrec, REG_TXSTAT, &val);
	if (ret)
		goto err;
	if (val & 0x1) {
		dev_dbg(printdev(devrec), "Error Sending. Retry count exceeded\n");
		ret = -ECOMM; /*                           */
	} else
		dev_dbg(printdev(devrec), "Packet Sent\n");

err:

	return ret;
}

static int mrf24j40_ed(struct ieee802154_dev *dev, u8 *level)
{
	/*       */
	printk(KERN_WARNING "mrf24j40: ed not implemented\n");
	*level = 0;
	return 0;
}

static int mrf24j40_start(struct ieee802154_dev *dev)
{
	struct mrf24j40 *devrec = dev->priv;
	u8 val;
	int ret;

	dev_dbg(printdev(devrec), "start\n");

	ret = read_short_reg(devrec, REG_INTCON, &val);
	if (ret)
		return ret;
	val &= ~(0x1|0x8); /*                                         */
	write_short_reg(devrec, REG_INTCON, val);

	return 0;
}

static void mrf24j40_stop(struct ieee802154_dev *dev)
{
	struct mrf24j40 *devrec = dev->priv;
	u8 val;
	int ret;
	dev_dbg(printdev(devrec), "stop\n");

	ret = read_short_reg(devrec, REG_INTCON, &val);
	if (ret)
		return;
	val |= 0x1|0x8; /*                                        */
	write_short_reg(devrec, REG_INTCON, val);

	return;
}

static int mrf24j40_set_channel(struct ieee802154_dev *dev,
				int page, int channel)
{
	struct mrf24j40 *devrec = dev->priv;
	u8 val;
	int ret;

	dev_dbg(printdev(devrec), "Set Channel %d\n", channel);

	WARN_ON(page != 0);
	WARN_ON(channel < MRF24J40_CHAN_MIN);
	WARN_ON(channel > MRF24J40_CHAN_MAX);

	/*                  */
	val = (channel-11) << 4 | 0x03;
	write_long_reg(devrec, REG_RFCON0, val);

	/*          */
	ret = read_short_reg(devrec, REG_RFCTL, &val);
	if (ret)
		return ret;
	val |= 0x04;
	write_short_reg(devrec, REG_RFCTL, val);
	val &= ~0x04;
	write_short_reg(devrec, REG_RFCTL, val);

	udelay(SET_CHANNEL_DELAY_US); /*               */

	return 0;
}

static int mrf24j40_filter(struct ieee802154_dev *dev,
			   struct ieee802154_hw_addr_filt *filt,
			   unsigned long changed)
{
	struct mrf24j40 *devrec = dev->priv;

	dev_dbg(printdev(devrec), "filter\n");

	if (changed & IEEE802515_AFILT_SADDR_CHANGED) {
		/*            */
		u8 addrh, addrl;
		addrh = filt->short_addr >> 8 & 0xff;
		addrl = filt->short_addr & 0xff;

		write_short_reg(devrec, REG_SADRH, addrh);
		write_short_reg(devrec, REG_SADRL, addrl);
		dev_dbg(printdev(devrec),
			"Set short addr to %04hx\n", filt->short_addr);
	}

	if (changed & IEEE802515_AFILT_IEEEADDR_CHANGED) {
		/*                */
		int i;
		for (i = 0; i < 8; i++)
			write_short_reg(devrec, REG_EADR0+i,
					filt->ieee_addr[7-i]);

#ifdef DEBUG
		printk(KERN_DEBUG "Set long addr to: ");
		for (i = 0; i < 8; i++)
			printk("%02hhx ", filt->ieee_addr[i]);
		printk(KERN_DEBUG "\n");
#endif
	}

	if (changed & IEEE802515_AFILT_PANID_CHANGED) {
		/*        */
		u8 panidl, panidh;
		panidh = filt->pan_id >> 8 & 0xff;
		panidl = filt->pan_id & 0xff;
		write_short_reg(devrec, REG_PANIDH, panidh);
		write_short_reg(devrec, REG_PANIDL, panidl);

		dev_dbg(printdev(devrec), "Set PANID to %04hx\n", filt->pan_id);
	}

	if (changed & IEEE802515_AFILT_PANC_CHANGED) {
		/*                 */
		u8 val;
		int ret;

		ret = read_short_reg(devrec, REG_RXMCR, &val);
		if (ret)
			return ret;
		if (filt->pan_coord)
			val |= 0x8;
		else
			val &= ~0x8;
		write_short_reg(devrec, REG_RXMCR, val);

		/*                                                          
                                                               
   */

		dev_dbg(printdev(devrec), "Set Pan Coord to %s\n",
					filt->pan_coord ? "on" : "off");
	}

	return 0;
}

static int mrf24j40_handle_rx(struct mrf24j40 *devrec)
{
	u8 len = RX_FIFO_SIZE;
	u8 lqi = 0;
	u8 val;
	int ret = 0;
	struct sk_buff *skb;

	/*                                                             
                                                               */
	ret = read_short_reg(devrec, REG_BBREG1, &val);
	if (ret)
		goto out;
	val |= 4; /*              */
	write_short_reg(devrec, REG_BBREG1, val);

	skb = alloc_skb(len, GFP_KERNEL);
	if (!skb) {
		ret = -ENOMEM;
		goto out;
	}

	ret = mrf24j40_read_rx_buf(devrec, skb_put(skb, len), &len, &lqi);
	if (ret < 0) {
		dev_err(printdev(devrec), "Failure reading RX FIFO\n");
		kfree_skb(skb);
		ret = -EINVAL;
		goto out;
	}

	/*                      */
	skb_trim(skb, len-2);

	/*                                                                  
                                                                   
                         */
	ieee802154_rx_irqsafe(devrec->dev, skb, lqi);

	dev_dbg(printdev(devrec), "RX Handled\n");

out:
	/*                                                */
	ret = read_short_reg(devrec, REG_BBREG1, &val);
	if (ret)
		return ret;
	val &= ~0x4; /*                */
	write_short_reg(devrec, REG_BBREG1, val);

	return ret;
}

static struct ieee802154_ops mrf24j40_ops = {
	.owner = THIS_MODULE,
	.xmit = mrf24j40_tx,
	.ed = mrf24j40_ed,
	.start = mrf24j40_start,
	.stop = mrf24j40_stop,
	.set_channel = mrf24j40_set_channel,
	.set_hw_addr_filt = mrf24j40_filter,
};

static irqreturn_t mrf24j40_isr(int irq, void *data)
{
	struct mrf24j40 *devrec = data;

	disable_irq_nosync(irq);

	schedule_work(&devrec->irqwork);

	return IRQ_HANDLED;
}

static void mrf24j40_isrwork(struct work_struct *work)
{
	struct mrf24j40 *devrec = container_of(work, struct mrf24j40, irqwork);
	u8 intstat;
	int ret;

	/*                           */
	ret = read_short_reg(devrec, REG_INTSTAT, &intstat);
	if (ret)
		goto out;

	/*                       */
	if (intstat & 0x1)
		complete(&devrec->tx_complete);

	/*              */
	if (intstat & 0x8)
		mrf24j40_handle_rx(devrec);

out:
	enable_irq(devrec->spi->irq);
}

static int mrf24j40_probe(struct spi_device *spi)
{
	int ret = -ENOMEM;
	u8 val;
	struct mrf24j40 *devrec;
	struct pinctrl *pinctrl;

	printk(KERN_INFO "mrf24j40: probe(). IRQ: %d\n", spi->irq);

	devrec = kzalloc(sizeof(struct mrf24j40), GFP_KERNEL);
	if (!devrec)
		goto err_devrec;
	devrec->buf = kzalloc(3, GFP_KERNEL);
	if (!devrec->buf)
		goto err_buf;

	pinctrl = devm_pinctrl_get_select_default(&spi->dev);
	if (IS_ERR(pinctrl))
		dev_warn(&spi->dev,
			"pinctrl pins are not configured from the driver");

	spi->mode = SPI_MODE_0; /*                                           */
	if (spi->max_speed_hz > MAX_SPI_SPEED_HZ)
		spi->max_speed_hz = MAX_SPI_SPEED_HZ;

	mutex_init(&devrec->buffer_mutex);
	init_completion(&devrec->tx_complete);
	INIT_WORK(&devrec->irqwork, mrf24j40_isrwork);
	devrec->spi = spi;
	spi_set_drvdata(spi, devrec);

	/*                                    */

	devrec->dev = ieee802154_alloc_device(0, &mrf24j40_ops);
	if (!devrec->dev)
		goto err_alloc_dev;

	devrec->dev->priv = devrec;
	devrec->dev->parent = &devrec->spi->dev;
	devrec->dev->phy->channels_supported[0] = CHANNEL_MASK;
	devrec->dev->flags = IEEE802154_HW_OMIT_CKSUM|IEEE802154_HW_AACK;

	dev_dbg(printdev(devrec), "registered mrf24j40\n");
	ret = ieee802154_register_device(devrec->dev);
	if (ret)
		goto err_register_device;

	/*                       
                                              */
	write_short_reg(devrec, REG_SOFTRST, 0x07);
	write_short_reg(devrec, REG_PACON2, 0x98);
	write_short_reg(devrec, REG_TXSTBL, 0x95);
	write_long_reg(devrec, REG_RFCON0, 0x03);
	write_long_reg(devrec, REG_RFCON1, 0x01);
	write_long_reg(devrec, REG_RFCON2, 0x80);
	write_long_reg(devrec, REG_RFCON6, 0x90);
	write_long_reg(devrec, REG_RFCON7, 0x80);
	write_long_reg(devrec, REG_RFCON8, 0x10);
	write_long_reg(devrec, REG_SLPCON1, 0x21);
	write_short_reg(devrec, REG_BBREG2, 0x80);
	write_short_reg(devrec, REG_CCAEDTH, 0x60);
	write_short_reg(devrec, REG_BBREG6, 0x40);
	write_short_reg(devrec, REG_RFCTL, 0x04);
	write_short_reg(devrec, REG_RFCTL, 0x0);
	udelay(192);

	/*                                                             */
	ret = read_short_reg(devrec, REG_RXMCR, &val);
	if (ret)
		goto err_read_reg;
	val &= ~0x3; /*                        */
	write_short_reg(devrec, REG_RXMCR, val);

	ret = request_irq(spi->irq,
			  mrf24j40_isr,
			  IRQF_TRIGGER_FALLING,
			  dev_name(&spi->dev),
			  devrec);

	if (ret) {
		dev_err(printdev(devrec), "Unable to get IRQ");
		goto err_irq;
	}

	return 0;

err_irq:
err_read_reg:
	ieee802154_unregister_device(devrec->dev);
err_register_device:
	ieee802154_free_device(devrec->dev);
err_alloc_dev:
	kfree(devrec->buf);
err_buf:
	kfree(devrec);
err_devrec:
	return ret;
}

static int mrf24j40_remove(struct spi_device *spi)
{
	struct mrf24j40 *devrec = spi_get_drvdata(spi);

	dev_dbg(printdev(devrec), "remove\n");

	free_irq(spi->irq, devrec);
	flush_work(&devrec->irqwork); /*                               */
	ieee802154_unregister_device(devrec->dev);
	ieee802154_free_device(devrec->dev);
	/*                                                           
              */

	/*                         */
	spi_set_drvdata(spi, NULL);
	kfree(devrec->buf);
	kfree(devrec);
	return 0;
}

static const struct spi_device_id mrf24j40_ids[] = {
	{ "mrf24j40", 0 },
	{ "mrf24j40ma", 0 },
	{ },
};
MODULE_DEVICE_TABLE(spi, mrf24j40_ids);

static struct spi_driver mrf24j40_driver = {
	.driver = {
		.name = "mrf24j40",
		.bus = &spi_bus_type,
		.owner = THIS_MODULE,
	},
	.id_table = mrf24j40_ids,
	.probe = mrf24j40_probe,
	.remove = mrf24j40_remove,
};

module_spi_driver(mrf24j40_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alan Ott");
MODULE_DESCRIPTION("MRF24J40 SPI 802.15.4 Controller Driver");
