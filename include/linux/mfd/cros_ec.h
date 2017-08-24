/*
 * ChromeOS EC multi-function device
 *
 * Copyright (C) 2012 Google, Inc
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __LINUX_MFD_CROS_EC_H
#define __LINUX_MFD_CROS_EC_H

#include <linux/mfd/cros_ec_commands.h>

/*
                                                                        
 */
enum {
	EC_MSG_TX_HEADER_BYTES	= 3,
	EC_MSG_TX_TRAILER_BYTES	= 1,
	EC_MSG_TX_PROTO_BYTES	= EC_MSG_TX_HEADER_BYTES +
					EC_MSG_TX_TRAILER_BYTES,
	EC_MSG_RX_PROTO_BYTES	= 3,

	/*                        */
	EC_MSG_BYTES		= EC_HOST_PARAM_SIZE + EC_MSG_TX_PROTO_BYTES,

};

/* 
                                                               
  
                                             
                                     
                                     
                           
                                      
                           
 */
struct cros_ec_msg {
	u8 version;
	u8 cmd;
	uint8_t *out_buf;
	int out_len;
	uint8_t *in_buf;
	int in_len;
};

/* 
                                                                 
  
                                   
                      
                         
                               
                               
        
                                                                    
                                                                     
                                                            
  
                                                                 
                                                                      
                  
                                
                                  
                                
                                   
                                                   
                                                          
                                                             
                       
                                              
                                                                    
                                                                        
                            
                                                                  
 */
struct cros_ec_device {
	const char *name;
	void *priv;
	int irq;
	uint8_t *din;
	uint8_t *dout;
	int din_size;
	int dout_size;
	int (*command_send)(struct cros_ec_device *ec,
			uint16_t cmd, void *out_buf, int out_len);
	int (*command_recv)(struct cros_ec_device *ec,
			uint16_t cmd, void *in_buf, int in_len);
	int (*command_sendrecv)(struct cros_ec_device *ec,
			uint16_t cmd, void *out_buf, int out_len,
			void *in_buf, int in_len);
	int (*command_xfer)(struct cros_ec_device *ec,
			struct cros_ec_msg *msg);

	const char *ec_name;
	const char *phys_name;
	struct device *parent;

	/*                                              */
	struct device *dev;
	struct mutex dev_lock;
	bool wake_enabled;
	bool was_wake_device;
	struct blocking_notifier_head event_notifier;
};

/* 
                                                                          
  
                                                           
  
                            
                                
 */
int cros_ec_suspend(struct cros_ec_device *ec_dev);

/* 
                                                                        
  
                                                          
  
                            
                                
 */
int cros_ec_resume(struct cros_ec_device *ec_dev);

/* 
                                                                        
  
                                                                         
                                                                           
                                                              
  
                              
                         
 */
int cros_ec_prepare_tx(struct cros_ec_device *ec_dev,
		       struct cros_ec_msg *msg);

/* 
                                        
  
                                                                    
                  
  
                              
                                
 */
int cros_ec_remove(struct cros_ec_device *ec_dev);

/* 
                                                                         
  
                                                                        
                                                  
  
                              
                                
 */
int cros_ec_register(struct cros_ec_device *ec_dev);

#endif /*                       */
