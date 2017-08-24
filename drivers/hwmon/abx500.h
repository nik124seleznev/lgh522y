/*
 * Copyright (C) ST-Ericsson 2010 - 2013
 * License terms: GNU General Public License v2
 * Author: Martin Persson <martin.persson@stericsson.com>
 *         Hongbo Zhang <hongbo.zhang@linaro.com>
 */

#ifndef _ABX500_H
#define _ABX500_H

#define NUM_SENSORS 5

struct abx500_temp;

/*
                                                    
                                   
                            
                                
                                     
                                    
 */
struct abx500_temp_ops {
	int (*read_sensor)(struct abx500_temp *, u8, int *);
	int (*irq_handler)(int, struct abx500_temp *);
	ssize_t (*show_name)(struct device *,
			struct device_attribute *, char *);
	ssize_t (*show_label) (struct device *,
			struct device_attribute *, char *);
	int (*is_visible)(struct attribute *, int);
};

/*
                                                         
                         
                           
                                 
                                     
                                     
                                     
                                                               
                                           
                                           
                                                                    
                                       
               
                                                  
                                                                      
 */
struct abx500_temp {
	struct platform_device *pdev;
	struct device *hwmon_dev;
	struct abx500_temp_ops ops;
	u8 gpadc_addr[NUM_SENSORS];
	unsigned long min[NUM_SENSORS];
	unsigned long max[NUM_SENSORS];
	unsigned long max_hyst[NUM_SENSORS];
	bool min_alarm[NUM_SENSORS];
	bool max_alarm[NUM_SENSORS];
	struct delayed_work work;
	bool work_active;
	struct mutex lock;
	int monitored_sensors;
	void *plat_data;
};

int abx500_hwmon_init(struct abx500_temp *data);

#endif /*           */
