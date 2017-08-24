/*
 *  External connector (extcon) class driver
 *
 * Copyright (C) 2012 Samsung Electronics
 * Author: Donggeun Kim <dg77.kim@samsung.com>
 * Author: MyungJoo Ham <myungjoo.ham@samsung.com>
 *
 * based on switch class driver
 * Copyright (C) 2008 Google, Inc.
 * Author: Mike Lockwood <lockwood@android.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
*/

#ifndef __LINUX_EXTCON_H__
#define __LINUX_EXTCON_H__

#include <linux/device.h>
#include <linux/notifier.h>
#include <linux/sysfs.h>

#define SUPPORTED_CABLE_MAX	32
#define CABLE_NAME_MAX		30

/*
                                                              
                                                         
                                                                  
                                      
                                                             
                                     
  
                                                               
  
                                                                   
                                                                    
                                                    
                                                                   
                                                                     
                                                                  
                                                
 */
enum extcon_cable_name {
	EXTCON_USB = 0,
	EXTCON_USB_HOST,
	EXTCON_TA, /*                */
	EXTCON_FAST_CHARGER,
	EXTCON_SLOW_CHARGER,
	EXTCON_CHARGE_DOWNSTREAM, /*                             */
	EXTCON_HDMI,
	EXTCON_MHL,
	EXTCON_DVI,
	EXTCON_VGA,
	EXTCON_DOCK,
	EXTCON_LINE_IN,
	EXTCON_LINE_OUT,
	EXTCON_MIC_IN,
	EXTCON_HEADPHONE_OUT,
	EXTCON_SPDIF_IN,
	EXTCON_SPDIF_OUT,
	EXTCON_VIDEO_IN,
	EXTCON_VIDEO_OUT,
	EXTCON_MECHANICAL,
};
extern const char extcon_cable_name[][CABLE_NAME_MAX + 1];

struct extcon_cable;

/* 
                                                                          
                                                                    
            
                                                                     
                                                        
                  
                                                                             
                                                    
                                                       
                                                          
                                                        
                                                           
                                                        
                                        
                                                                        
                              
                                                                         
                                
                                                                
                                                                
                 
                                                             
                                                                     
                                                
         
                                                                
                                                                   
                                       
                                                            
  
                                                                        
                                                                     
                                                                           
                                        
 */
struct extcon_dev {
	/*                                         */
	const char	*name;
	const char **supported_cable;
	const u32	*mutually_exclusive;

	/*                                                        */
	ssize_t	(*print_name)(struct extcon_dev *edev, char *buf);
	ssize_t	(*print_state)(struct extcon_dev *edev, char *buf);

	/*                                           */
	struct device	*dev;
	u32		state;
	struct raw_notifier_head nh;
	struct list_head entry;
	spinlock_t lock; /*                                */
	int max_supported;

	/*                                   */
	struct device_type extcon_dev_type;
	struct extcon_cable *cables;
	/*                                              */
	struct attribute_group attr_g_muex;
	struct attribute **attrs_muex;
	struct device_attribute *d_attrs_muex;
};

/* 
                                                                          
                           
                                                
                                         
                                 
                                   
                                                                
 */
struct extcon_cable {
	struct extcon_dev *edev;
	int cable_index;

	struct attribute_group attr_g;
	struct device_attribute attr_name;
	struct device_attribute attr_state;

	struct attribute *attrs[3]; /*                           */
};

/* 
                                                         
                                 
                                                                              
                                                                           
                                  
                                   
                                                   
 */
struct extcon_specific_cable_nb {
	struct notifier_block internal_nb;
	struct notifier_block *user_nb;
	int cable_index;
	struct extcon_dev *edev;
	unsigned long previous_value;
};

#if IS_ENABLED(CONFIG_EXTCON)

/*
                                                      
                                                          
 */
extern int extcon_dev_register(struct extcon_dev *edev, struct device *dev);
extern void extcon_dev_unregister(struct extcon_dev *edev);
extern struct extcon_dev *extcon_get_extcon_dev(const char *extcon_name);

/*
                                                                            
                                                                            
                                                                           
                            
 */
static inline u32 extcon_get_state(struct extcon_dev *edev)
{
	return edev->state;
}

extern int extcon_set_state(struct extcon_dev *edev, u32 state);
extern int extcon_update_state(struct extcon_dev *edev, u32 mask, u32 state);

/*
                                                                      
                                                                           
                                                                
 */
extern int extcon_find_cable_index(struct extcon_dev *sdev,
				   const char *cable_name);
extern int extcon_get_cable_state_(struct extcon_dev *edev, int cable_index);
extern int extcon_set_cable_state_(struct extcon_dev *edev, int cable_index,
				   bool cable_state);

extern int extcon_get_cable_state(struct extcon_dev *edev,
				  const char *cable_name);
extern int extcon_set_cable_state(struct extcon_dev *edev,
				  const char *cable_name, bool cable_state);

/*
                                                                   
                                                                         
                                                                        
                                                   
 */
extern int extcon_register_interest(struct extcon_specific_cable_nb *obj,
				    const char *extcon_name,
				    const char *cable_name,
				    struct notifier_block *nb);
extern int extcon_unregister_interest(struct extcon_specific_cable_nb *nb);

/*
                                                            
                                                                          
                                                                       
                                                                           
                                                                   
 */
extern int extcon_register_notifier(struct extcon_dev *edev,
				    struct notifier_block *nb);
extern int extcon_unregister_notifier(struct extcon_dev *edev,
				      struct notifier_block *nb);
#else /*               */
static inline int extcon_dev_register(struct extcon_dev *edev,
				      struct device *dev)
{
	return 0;
}

static inline void extcon_dev_unregister(struct extcon_dev *edev) { }

static inline u32 extcon_get_state(struct extcon_dev *edev)
{
	return 0;
}

static inline int extcon_set_state(struct extcon_dev *edev, u32 state)
{
	return 0;
}

static inline int extcon_update_state(struct extcon_dev *edev, u32 mask,
				       u32 state)
{
	return 0;
}

static inline int extcon_find_cable_index(struct extcon_dev *edev,
					  const char *cable_name)
{
	return 0;
}

static inline int extcon_get_cable_state_(struct extcon_dev *edev,
					  int cable_index)
{
	return 0;
}

static inline int extcon_set_cable_state_(struct extcon_dev *edev,
					  int cable_index, bool cable_state)
{
	return 0;
}

static inline int extcon_get_cable_state(struct extcon_dev *edev,
			const char *cable_name)
{
	return 0;
}

static inline int extcon_set_cable_state(struct extcon_dev *edev,
			const char *cable_name, int state)
{
	return 0;
}

static inline struct extcon_dev *extcon_get_extcon_dev(const char *extcon_name)
{
	return NULL;
}

static inline int extcon_register_notifier(struct extcon_dev *edev,
					   struct notifier_block *nb)
{
	return 0;
}

static inline int extcon_unregister_notifier(struct extcon_dev *edev,
					     struct notifier_block *nb)
{
	return 0;
}

static inline int extcon_register_interest(struct extcon_specific_cable_nb *obj,
					   const char *extcon_name,
					   const char *cable_name,
					   struct notifier_block *nb)
{
	return 0;
}

static inline int extcon_unregister_interest(struct extcon_specific_cable_nb
						    *obj)
{
	return 0;
}
#endif /*               */
#endif /*                    */
