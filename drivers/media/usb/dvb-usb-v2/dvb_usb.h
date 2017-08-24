/*
 * DVB USB framework
 *
 * Copyright (C) 2004-6 Patrick Boettcher <patrick.boettcher@desy.de>
 * Copyright (C) 2012 Antti Palosaari <crope@iki.fi>
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License along
 *    with this program; if not, write to the Free Software Foundation, Inc.,
 *    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef DVB_USB_H
#define DVB_USB_H

#include <linux/usb/input.h>
#include <linux/firmware.h>
#include <media/rc-core.h>

#include "dvb_frontend.h"
#include "dvb_demux.h"
#include "dvb_net.h"
#include "dmxdev.h"
#include "dvb-usb-ids.h"

/*
                                                   
  
             
                   
                        
                        
                        
                   
                        
                        
                        
  
  
                                
                                                  
                                                       
                                                   
  
                                                              
 */

/*                                            */
#define adap_to_d(adap) (container_of(adap, struct dvb_usb_device, \
		adapter[adap->id]))
#define adap_to_priv(adap) (adap_to_d(adap)->priv)
#define fe_to_adap(fe) ((struct dvb_usb_adapter *) ((fe)->dvb->priv))
#define fe_to_d(fe) (adap_to_d(fe_to_adap(fe)))
#define fe_to_priv(fe) (fe_to_d(fe)->priv)
#define d_to_priv(d) (d->priv)

#define dvb_usb_dbg_usb_control_msg(udev, r, t, v, i, b, l) { \
	char *direction; \
	if (t == (USB_TYPE_VENDOR | USB_DIR_OUT)) \
		direction = ">>>"; \
	else \
		direction = "<<<"; \
	dev_dbg(&udev->dev, "%s: %02x %02x %02x %02x %02x %02x %02x %02x " \
			"%s %*ph\n",  __func__, t, r, v & 0xff, v >> 8, \
			i & 0xff, i >> 8, l & 0xff, l >> 8, direction, l, b); \
}

#define DVB_USB_STREAM_BULK(endpoint_, count_, size_) { \
	.type = USB_BULK, \
	.count = count_, \
	.endpoint = endpoint_, \
	.u = { \
		.bulk = { \
			.buffersize = size_, \
		} \
	} \
}

#define DVB_USB_STREAM_ISOC(endpoint_, count_, frames_, size_, interval_) { \
	.type = USB_ISOC, \
	.count = count_, \
	.endpoint = endpoint_, \
	.u = { \
		.isoc = { \
			.framesperurb = frames_, \
			.framesize = size_,\
			.interval = interval_, \
		} \
	} \
}

#define DVB_USB_DEVICE(vend, prod, props_, name_, rc) \
	.match_flags = USB_DEVICE_ID_MATCH_DEVICE, \
	.idVendor = (vend), \
	.idProduct = (prod), \
	.driver_info = (kernel_ulong_t) &((const struct dvb_usb_driver_info) { \
		.props = (props_), \
		.name = (name_), \
		.rc_map = (rc), \
	})

struct dvb_usb_device;
struct dvb_usb_adapter;

/* 
                                                                               
                   
                     
                                  
                                                     
 */
struct dvb_usb_driver_info {
	const char *name;
	const char *rc_map;
	const struct dvb_usb_device_properties *props;
};

/* 
                                                
                                    
                                                       
                                                
                                                   
                                            
                                                            
                                                                      
 */
struct dvb_usb_rc {
	const char *map_name;
	u64 allowed_protos;
	int (*change_protocol)(struct rc_dev *dev, u64 *rc_type);
	int (*query) (struct dvb_usb_device *d);
	unsigned int interval;
	const enum rc_driver_type driver_type;
	bool bulk_mode;
};

/* 
                                         
                  
                             
                                        
 */
struct usb_data_stream_properties {
#define USB_BULK  1
#define USB_ISOC  2
	u8 type;
	u8 count;
	u8 endpoint;

	union {
		struct {
			unsigned int buffersize; /*         */
		} bulk;
		struct {
			int framesperurb;
			int framesize;
			int interval;
		} isoc;
	} u;
};

/* 
                                       
                              
                                                     
                                                        
                                                     
                                            
 */
#define MAX_NO_OF_FE_PER_ADAP 3
struct dvb_usb_adapter_properties {
#define DVB_USB_ADAP_HAS_PID_FILTER               0x01
#define DVB_USB_ADAP_PID_FILTER_CAN_BE_TURNED_OFF 0x02
#define DVB_USB_ADAP_NEED_PID_FILTERING           0x04
	u8 caps;

	u8 pid_filter_count;
	int (*pid_filter_ctrl) (struct dvb_usb_adapter *, int);
	int (*pid_filter) (struct dvb_usb_adapter *, int, u16, int);

	struct usb_data_stream_properties stream;
};

/* 
                                                                    
                                                 
                                   
                                                                     
                                                       
                                                             
                                                                     
                                                                         
           
                                                                                
                                                                             
                                                   
                                                    
                                                      
                                                         
                                              
                                                      
                                                      
                                                            
                                                       
                                                           
                                                            
                                                      
                                                         
                                                                     
                                                                       
                 
                                        
                                                                         
                                                                              
                                                                              
                                                                        
 */
#define MAX_NO_OF_ADAPTER_PER_DEVICE 2
struct dvb_usb_device_properties {
	const char *driver_name;
	struct module *owner;
	short *adapter_nr;

	u8 bInterfaceNumber;
	unsigned int size_of_priv;
	u8 generic_bulk_ctrl_endpoint;
	u8 generic_bulk_ctrl_endpoint_response;
	unsigned int generic_bulk_ctrl_delay;

#define WARM                  0
#define COLD                  1
	int (*identify_state) (struct dvb_usb_device *, const char **);
	const char *firmware;
#define RECONNECTS_USB        1
	int (*download_firmware) (struct dvb_usb_device *,
			const struct firmware *);

	struct i2c_algorithm *i2c_algo;

	unsigned int num_adapters;
	int (*get_adapter_count) (struct dvb_usb_device *);
	struct dvb_usb_adapter_properties adapter[MAX_NO_OF_ADAPTER_PER_DEVICE];
	int (*power_ctrl) (struct dvb_usb_device *, int);
	int (*read_config) (struct dvb_usb_device *d);
	int (*read_mac_address) (struct dvb_usb_adapter *, u8 []);
	int (*frontend_attach) (struct dvb_usb_adapter *);
	int (*tuner_attach) (struct dvb_usb_adapter *);
	int (*frontend_ctrl) (struct dvb_frontend *, int);
	int (*streaming_ctrl) (struct dvb_frontend *, int);
	int (*init) (struct dvb_usb_device *);
	void (*exit) (struct dvb_usb_device *);
	int (*get_rc_config) (struct dvb_usb_device *, struct dvb_usb_rc *);
#define DVB_USB_FE_TS_TYPE_188        0
#define DVB_USB_FE_TS_TYPE_204        1
#define DVB_USB_FE_TS_TYPE_RAW        2
	int (*get_stream_config) (struct dvb_frontend *,  u8 *,
			struct usb_data_stream_properties *);
};

/* 
                                  
                                       
                                             
                                                                 
                                                            
  
                                                
                                            
 */
#define MAX_NO_URBS_FOR_DATA_STREAM 10
struct usb_data_stream {
	struct usb_device *udev;
	struct usb_data_stream_properties props;

#define USB_STATE_INIT    0x00
#define USB_STATE_URB_BUF 0x01
	u8 state;

	void (*complete) (struct usb_data_stream *, u8 *, size_t);

	struct urb    *urb_list[MAX_NO_URBS_FOR_DATA_STREAM];
	int            buf_num;
	unsigned long  buf_size;
	u8            *buf_list[MAX_NO_URBS_FOR_DATA_STREAM];
	dma_addr_t     dma_addr[MAX_NO_URBS_FOR_DATA_STREAM];

	int urbs_initialized;
	int urbs_submitted;

	void *user_priv;
};

/* 
                                       
                                        
                                       
                                               
                                                 
                                                                     
                                                        
                                  
                                                       
                                 
                          
                                   
                                       
                                                                       
                         
                                            
                                              
 */
struct dvb_usb_adapter {
	const struct dvb_usb_adapter_properties *props;
	struct usb_data_stream stream;
	u8 id;
	u8 ts_type;
	bool suspend_resume_active;
	bool pid_filtering;
	u8 feed_count;
	u8 max_feed_count;
	s8 active_fe;
#define ADAP_INIT                0
#define ADAP_SLEEP               1
#define ADAP_STREAMING           2
	unsigned long state_bits;

	/*     */
	struct dvb_adapter   dvb_adap;
	struct dmxdev        dmxdev;
	struct dvb_demux     demux;
	struct dvb_net       dvb_net;

	struct dvb_frontend *fe[MAX_NO_OF_FE_PER_ADAP];
	int (*fe_init[MAX_NO_OF_FE_PER_ADAP]) (struct dvb_frontend *);
	int (*fe_sleep[MAX_NO_OF_FE_PER_ADAP]) (struct dvb_frontend *);
};

/* 
                        
                            
                     
                                  
                                                    
                                                   
                                               
                                       
                                      
                                                         
                                             
                                      
                                  
                                            
                                          
                                                                              
                                           
 */
struct dvb_usb_device {
	const struct dvb_usb_device_properties *props;
	const char *name;
	const char *rc_map;
	bool rc_polling_active;
	struct usb_device *udev;
	struct usb_interface *intf;
	struct dvb_usb_rc rc;
	struct work_struct probe_work;
	pid_t work_pid;
	int powered;

	/*         */
	struct mutex usb_mutex;

	/*     */
	struct mutex i2c_mutex;
	struct i2c_adapter i2c_adap;

	struct dvb_usb_adapter adapter[MAX_NO_OF_ADAPTER_PER_DEVICE];

	/*                */
	struct rc_dev *rc_dev;
	char rc_phys[64];
	struct delayed_work rc_query_work;

	void *priv;
};

extern int dvb_usbv2_probe(struct usb_interface *,
		const struct usb_device_id *);
extern void dvb_usbv2_disconnect(struct usb_interface *);
extern int dvb_usbv2_suspend(struct usb_interface *, pm_message_t);
extern int dvb_usbv2_resume(struct usb_interface *);
extern int dvb_usbv2_reset_resume(struct usb_interface *);

/*                                                  */
extern int dvb_usbv2_generic_rw(struct dvb_usb_device *, u8 *, u16, u8 *, u16);
extern int dvb_usbv2_generic_write(struct dvb_usb_device *, u8 *, u16);
/*                                                       */
extern int dvb_usbv2_generic_rw_locked(struct dvb_usb_device *,
		u8 *, u16, u8 *, u16);
extern int dvb_usbv2_generic_write_locked(struct dvb_usb_device *, u8 *, u16);

#endif
