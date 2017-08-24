/*
 * Copyright (C) ST-Ericsson AB 2012
 * Author: Sjur Brendeland / sjur.brandeland@stericsson.com
 *
 * License terms: GNU General Public License (GPL) version 2
 */

#ifndef __INC_MODEM_DEV_H
#define __INC_MODEM_DEV_H
#include <linux/types.h>
#include <linux/platform_device.h>

struct ste_modem_device;

/* 
                                                                  
                                               
  
                                                                        
 */
struct ste_modem_dev_cb {
	void (*kick)(struct ste_modem_device *mdev, int notify_id);
};

/* 
                                                                             
  
                                                                        
                         
                                                               
                                                      
  
                                                                      
                       
 */
struct ste_modem_dev_ops {
	int (*power)(struct ste_modem_device *mdev, bool on);
	int (*kick)(struct ste_modem_device *mdev, int notify_id);
	int (*kick_subscribe)(struct ste_modem_device *mdev, int notify_id);
	int (*setup)(struct ste_modem_device *mdev,
		     struct ste_modem_dev_cb *cfg);
};

/* 
                                                           
                                      
                                             
                                  
 */
struct ste_modem_device {
	struct platform_device pdev;
	struct ste_modem_dev_ops ops;
	void *drv_data;
};

#endif /*               */
