/*
 *  i2c-algo-pca.c i2c driver algorithms for PCA9564 adapters
 *    Copyright (C) 2004 Arcom Control Systems
 *    Copyright (C) 2008 Pengutronix
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *  MA 02110-1301 USA.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/delay.h>
#include <linux/jiffies.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/i2c.h>
#include <linux/i2c-algo-pca.h>

#define DEB1(fmt, args...) do { if (i2c_debug >= 1)			\
				 printk(KERN_DEBUG fmt, ## args); } while (0)
#define DEB2(fmt, args...) do { if (i2c_debug >= 2)			\
				 printk(KERN_DEBUG fmt, ## args); } while (0)
#define DEB3(fmt, args...) do { if (i2c_debug >= 3)			\
				 printk(KERN_DEBUG fmt, ## args); } while (0)

static int i2c_debug;

#define pca_outw(adap, reg, val) adap->write_byte(adap->data, reg, val)
#define pca_inw(adap, reg) adap->read_byte(adap->data, reg)

#define pca_status(adap) pca_inw(adap, I2C_PCA_STA)
#define pca_clock(adap) adap->i2c_clock
#define pca_set_con(adap, val) pca_outw(adap, I2C_PCA_CON, val)
#define pca_get_con(adap) pca_inw(adap, I2C_PCA_CON)
#define pca_wait(adap) adap->wait_for_completion(adap->data)

static void pca_reset(struct i2c_algo_pca_data *adap)
{
	if (adap->chip == I2C_PCA_CHIP_9665) {
		/*                                           
                                       
   */
		pca_outw(adap, I2C_PCA_INDPTR, I2C_PCA_IPRESET);
		pca_outw(adap, I2C_PCA_IND, 0xA5);
		pca_outw(adap, I2C_PCA_IND, 0x5A);
	} else {
		adap->reset_chip(adap->data);
	}
}

/*
                                             
  
                                                 
 */
static int pca_start(struct i2c_algo_pca_data *adap)
{
	int sta = pca_get_con(adap);
	DEB2("=== START\n");
	sta |= I2C_PCA_CON_STA;
	sta &= ~(I2C_PCA_CON_STO|I2C_PCA_CON_SI);
	pca_set_con(adap, sta);
	return pca_wait(adap);
}

/*
                                                     
  
                                                         
 */
static int pca_repeated_start(struct i2c_algo_pca_data *adap)
{
	int sta = pca_get_con(adap);
	DEB2("=== REPEATED START\n");
	sta |= I2C_PCA_CON_STA;
	sta &= ~(I2C_PCA_CON_STO|I2C_PCA_CON_SI);
	pca_set_con(adap, sta);
	return pca_wait(adap);
}

/*
                                           
  
                                                      
  
                                                                   
                                                             
                 
 */
static void pca_stop(struct i2c_algo_pca_data *adap)
{
	int sta = pca_get_con(adap);
	DEB2("=== STOP\n");
	sta |= I2C_PCA_CON_STO;
	sta &= ~(I2C_PCA_CON_STA|I2C_PCA_CON_SI);
	pca_set_con(adap, sta);
}

/*
                                     
  
                                          
 */
static int pca_address(struct i2c_algo_pca_data *adap,
		       struct i2c_msg *msg)
{
	int sta = pca_get_con(adap);
	int addr;

	addr = ((0x7f & msg->addr) << 1);
	if (msg->flags & I2C_M_RD)
		addr |= 1;
	DEB2("=== SLAVE ADDRESS %#04x+%c=%#04x\n",
	     msg->addr, msg->flags & I2C_M_RD ? 'R' : 'W', addr);

	pca_outw(adap, I2C_PCA_DAT, addr);

	sta &= ~(I2C_PCA_CON_STO|I2C_PCA_CON_STA|I2C_PCA_CON_SI);
	pca_set_con(adap, sta);

	return pca_wait(adap);
}

/*
                   
  
                                              
 */
static int pca_tx_byte(struct i2c_algo_pca_data *adap,
		       __u8 b)
{
	int sta = pca_get_con(adap);
	DEB2("=== WRITE %#04x\n", b);
	pca_outw(adap, I2C_PCA_DAT, b);

	sta &= ~(I2C_PCA_CON_STO|I2C_PCA_CON_STA|I2C_PCA_CON_SI);
	pca_set_con(adap, sta);

	return pca_wait(adap);
}

/*
                 
  
                       
 */
static void pca_rx_byte(struct i2c_algo_pca_data *adap,
			__u8 *b, int ack)
{
	*b = pca_inw(adap, I2C_PCA_DAT);
	DEB2("=== READ %#04x %s\n", *b, ack ? "ACK" : "NACK");
}

/*
                                                                      
  
                                       
 */
static int pca_rx_ack(struct i2c_algo_pca_data *adap,
		      int ack)
{
	int sta = pca_get_con(adap);

	sta &= ~(I2C_PCA_CON_STO|I2C_PCA_CON_STA|I2C_PCA_CON_SI|I2C_PCA_CON_AA);

	if (ack)
		sta |= I2C_PCA_CON_AA;

	pca_set_con(adap, sta);
	return pca_wait(adap);
}

static int pca_xfer(struct i2c_adapter *i2c_adap,
		    struct i2c_msg *msgs,
		    int num)
{
	struct i2c_algo_pca_data *adap = i2c_adap->algo_data;
	struct i2c_msg *msg = NULL;
	int curmsg;
	int numbytes = 0;
	int state;
	int ret;
	int completed = 1;
	unsigned long timeout = jiffies + i2c_adap->timeout;

	while ((state = pca_status(adap)) != 0xf8) {
		if (time_before(jiffies, timeout)) {
			msleep(10);
		} else {
			dev_dbg(&i2c_adap->dev, "bus is not idle. status is "
				"%#04x\n", state);
			return -EBUSY;
		}
	}

	DEB1("{{{ XFER %d messages\n", num);

	if (i2c_debug >= 2) {
		for (curmsg = 0; curmsg < num; curmsg++) {
			int addr, i;
			msg = &msgs[curmsg];

			addr = (0x7f & msg->addr) ;

			if (msg->flags & I2C_M_RD)
				printk(KERN_INFO "    [%02d] RD %d bytes from %#02x [%#02x, ...]\n",
				       curmsg, msg->len, addr, (addr << 1) | 1);
			else {
				printk(KERN_INFO "    [%02d] WR %d bytes to %#02x [%#02x%s",
				       curmsg, msg->len, addr, addr << 1,
				       msg->len == 0 ? "" : ", ");
				for (i = 0; i < msg->len; i++)
					printk("%#04x%s", msg->buf[i], i == msg->len - 1 ? "" : ", ");
				printk("]\n");
			}
		}
	}

	curmsg = 0;
	ret = -EIO;
	while (curmsg < num) {
		state = pca_status(adap);

		DEB3("STATE is 0x%02x\n", state);
		msg = &msgs[curmsg];

		switch (state) {
		case 0xf8: /*                                  */
			completed = pca_start(adap);
			break;

		case 0x08: /*                                        */
		case 0x10: /*                                                 */
			completed = pca_address(adap, msg);
			break;

		case 0x18: /*                                                   */
		case 0x28: /*                                                                 */
			if (numbytes < msg->len) {
				completed = pca_tx_byte(adap,
							msg->buf[numbytes]);
				numbytes++;
				break;
			}
			curmsg++; numbytes = 0;
			if (curmsg == num)
				pca_stop(adap);
			else
				completed = pca_repeated_start(adap);
			break;

		case 0x20: /*                                                       */
			DEB2("NOT ACK received after SLA+W\n");
			pca_stop(adap);
			ret = -ENXIO;
			goto out;

		case 0x40: /*                                                   */
			completed = pca_rx_ack(adap, msg->len > 1);
			break;

		case 0x50: /*                                                     */
			if (numbytes < msg->len) {
				pca_rx_byte(adap, &msg->buf[numbytes], 1);
				numbytes++;
				completed = pca_rx_ack(adap,
						       numbytes < msg->len - 1);
				break;
			}
			curmsg++; numbytes = 0;
			if (curmsg == num)
				pca_stop(adap);
			else
				completed = pca_repeated_start(adap);
			break;

		case 0x48: /*                                                       */
			DEB2("NOT ACK received after SLA+R\n");
			pca_stop(adap);
			ret = -ENXIO;
			goto out;

		case 0x30: /*                                                                     */
			DEB2("NOT ACK received after data byte\n");
			pca_stop(adap);
			goto out;

		case 0x38: /*                                                    */
			DEB2("Arbitration lost\n");
			/*
                                                 
                                                  
                                                  
                                      
     
                                              
              
    */
			pca_start(adap);
			goto out;

		case 0x58: /*                                                        */
			if (numbytes == msg->len - 1) {
				pca_rx_byte(adap, &msg->buf[numbytes], 0);
				curmsg++; numbytes = 0;
				if (curmsg == num)
					pca_stop(adap);
				else
					completed = pca_repeated_start(adap);
			} else {
				DEB2("NOT ACK sent after data byte received. "
				     "Not final byte. numbytes %d. len %d\n",
				     numbytes, msg->len);
				pca_stop(adap);
				goto out;
			}
			break;
		case 0x70: /*                           */
			DEB2("BUS ERROR - SDA Stuck low\n");
			pca_reset(adap);
			goto out;
		case 0x90: /*                           */
			DEB2("BUS ERROR - SCL Stuck low\n");
			pca_reset(adap);
			goto out;
		case 0x00: /*                                                                              */
			DEB2("BUS ERROR - Illegal START or STOP\n");
			pca_reset(adap);
			goto out;
		default:
			dev_err(&i2c_adap->dev, "unhandled SIO state 0x%02x\n", state);
			break;
		}

		if (!completed)
			goto out;
	}

	ret = curmsg;
 out:
	DEB1("}}} transferred %d/%d messages. "
	     "status is %#04x. control is %#04x\n",
	     curmsg, num, pca_status(adap),
	     pca_get_con(adap));
	return ret;
}

static u32 pca_func(struct i2c_adapter *adap)
{
	return I2C_FUNC_I2C | I2C_FUNC_SMBUS_EMUL;
}

static const struct i2c_algorithm pca_algo = {
	.master_xfer	= pca_xfer,
	.functionality	= pca_func,
};

static unsigned int pca_probe_chip(struct i2c_adapter *adap)
{
	struct i2c_algo_pca_data *pca_data = adap->algo_data;
	/*                                                            
                                                               
                                                                 
                           
  */
	pca_outw(pca_data, I2C_PCA_INDPTR, I2C_PCA_IADR);
	pca_outw(pca_data, I2C_PCA_IND, 0xAA);
	pca_outw(pca_data, I2C_PCA_INDPTR, I2C_PCA_ITO);
	pca_outw(pca_data, I2C_PCA_IND, 0x00);
	pca_outw(pca_data, I2C_PCA_INDPTR, I2C_PCA_IADR);
	if (pca_inw(pca_data, I2C_PCA_IND) == 0xAA) {
		printk(KERN_INFO "%s: PCA9665 detected.\n", adap->name);
		pca_data->chip = I2C_PCA_CHIP_9665;
	} else {
		printk(KERN_INFO "%s: PCA9564 detected.\n", adap->name);
		pca_data->chip = I2C_PCA_CHIP_9564;
	}
	return pca_data->chip;
}

static int pca_init(struct i2c_adapter *adap)
{
	struct i2c_algo_pca_data *pca_data = adap->algo_data;

	adap->algo = &pca_algo;

	if (pca_probe_chip(adap) == I2C_PCA_CHIP_9564) {
		static int freqs[] = {330, 288, 217, 146, 88, 59, 44, 36};
		int clock;

		if (pca_data->i2c_clock > 7) {
			switch (pca_data->i2c_clock) {
			case 330000:
				pca_data->i2c_clock = I2C_PCA_CON_330kHz;
				break;
			case 288000:
				pca_data->i2c_clock = I2C_PCA_CON_288kHz;
				break;
			case 217000:
				pca_data->i2c_clock = I2C_PCA_CON_217kHz;
				break;
			case 146000:
				pca_data->i2c_clock = I2C_PCA_CON_146kHz;
				break;
			case 88000:
				pca_data->i2c_clock = I2C_PCA_CON_88kHz;
				break;
			case 59000:
				pca_data->i2c_clock = I2C_PCA_CON_59kHz;
				break;
			case 44000:
				pca_data->i2c_clock = I2C_PCA_CON_44kHz;
				break;
			case 36000:
				pca_data->i2c_clock = I2C_PCA_CON_36kHz;
				break;
			default:
				printk(KERN_WARNING
					"%s: Invalid I2C clock speed selected."
					" Using default 59kHz.\n", adap->name);
			pca_data->i2c_clock = I2C_PCA_CON_59kHz;
			}
		} else {
			printk(KERN_WARNING "%s: "
				"Choosing the clock frequency based on "
				"index is deprecated."
				" Use the nominal frequency.\n", adap->name);
		}

		pca_reset(pca_data);

		clock = pca_clock(pca_data);
		printk(KERN_INFO "%s: Clock frequency is %dkHz\n",
		     adap->name, freqs[clock]);

		pca_set_con(pca_data, I2C_PCA_CON_ENSIO | clock);
	} else {
		int clock;
		int mode;
		int tlow, thi;
		/*                                                          */
		int min_tlow, min_thi;
		/*                                                           
                                                        
                                                                
                                                             
                                                             
                                     
   */
		int raise_fall_time;

		if (pca_data->i2c_clock > 1265800) {
			printk(KERN_WARNING "%s: I2C clock speed too high."
				" Using 1265.8kHz.\n", adap->name);
			pca_data->i2c_clock = 1265800;
		}

		if (pca_data->i2c_clock < 60300) {
			printk(KERN_WARNING "%s: I2C clock speed too low."
				" Using 60.3kHz.\n", adap->name);
			pca_data->i2c_clock = 60300;
		}

		/*                                                           */
		clock = pca_clock(pca_data) / 100;

		if (pca_data->i2c_clock > 1000000) {
			mode = I2C_PCA_MODE_TURBO;
			min_tlow = 14;
			min_thi  = 5;
			raise_fall_time = 22; /*                           */
		} else if (pca_data->i2c_clock > 400000) {
			mode = I2C_PCA_MODE_FASTP;
			min_tlow = 17;
			min_thi  = 9;
			raise_fall_time = 22; /*                           */
		} else if (pca_data->i2c_clock > 100000) {
			mode = I2C_PCA_MODE_FAST;
			min_tlow = 44;
			min_thi  = 20;
			raise_fall_time = 58; /*                           */
		} else {
			mode = I2C_PCA_MODE_STD;
			min_tlow = 157;
			min_thi  = 134;
			raise_fall_time = 127; /*                           */
		}

		/*                                                          
                                                             
                              
   */
		if (clock < 648) {
			tlow = 255;
			thi = 1000000 - clock * raise_fall_time;
			thi /= (I2C_PCA_OSC_PER * clock) - tlow;
		} else {
			tlow = (1000000 - clock * raise_fall_time) * min_tlow;
			tlow /= I2C_PCA_OSC_PER * clock * (min_thi + min_tlow);
			thi = tlow * min_thi / min_tlow;
		}

		pca_reset(pca_data);

		printk(KERN_INFO
		     "%s: Clock frequency is %dHz\n", adap->name, clock * 100);

		pca_outw(pca_data, I2C_PCA_INDPTR, I2C_PCA_IMODE);
		pca_outw(pca_data, I2C_PCA_IND, mode);
		pca_outw(pca_data, I2C_PCA_INDPTR, I2C_PCA_ISCLL);
		pca_outw(pca_data, I2C_PCA_IND, tlow);
		pca_outw(pca_data, I2C_PCA_INDPTR, I2C_PCA_ISCLH);
		pca_outw(pca_data, I2C_PCA_IND, thi);

		pca_set_con(pca_data, I2C_PCA_CON_ENSIO);
	}
	udelay(500); /*                                   */

	return 0;
}

/*
                                                      
 */
int i2c_pca_add_bus(struct i2c_adapter *adap)
{
	int rval;

	rval = pca_init(adap);
	if (rval)
		return rval;

	return i2c_add_adapter(adap);
}
EXPORT_SYMBOL(i2c_pca_add_bus);

int i2c_pca_add_numbered_bus(struct i2c_adapter *adap)
{
	int rval;

	rval = pca_init(adap);
	if (rval)
		return rval;

	return i2c_add_numbered_adapter(adap);
}
EXPORT_SYMBOL(i2c_pca_add_numbered_bus);

MODULE_AUTHOR("Ian Campbell <icampbell@arcom.com>, "
	"Wolfram Sang <w.sang@pengutronix.de>");
MODULE_DESCRIPTION("I2C-Bus PCA9564/PCA9665 algorithm");
MODULE_LICENSE("GPL");

module_param(i2c_debug, int, 0);