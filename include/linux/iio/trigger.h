/* The industrial I/O core, trigger handling functions
 *
 * Copyright (c) 2008 Jonathan Cameron
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 */
#include <linux/irq.h>
#include <linux/module.h>

#ifndef _IIO_TRIGGER_H_
#define _IIO_TRIGGER_H_

#ifdef CONFIG_IIO_TRIGGER
struct iio_subirq {
	bool enabled;
};

/* 
                                                                    
                                                       
                                                          
                                                           
                                    
                                                             
                                  
  
                                                               
                               
  */
struct iio_trigger_ops {
	struct module *owner;
	int (*set_trigger_state)(struct iio_trigger *trig, bool state);
	int (*try_reenable)(struct iio_trigger *trig);
	int (*validate_device)(struct iio_trigger *trig,
			       struct iio_dev *indio_dev);
};


/* 
                                                     
                                       
                                   
                               
                                                  
                                                              
                                                               
                                         
                                                       
                                                                   
                                                          
                                                    
                                                    
  */
struct iio_trigger {
	const struct iio_trigger_ops	*ops;
	int				id;
	const char			*name;
	struct device			dev;

	struct list_head		list;
	struct list_head		alloc_list;
	int use_count;

	struct irq_chip			subirq_chip;
	int				subirq_base;

	struct iio_subirq subirqs[CONFIG_IIO_CONSUMERS_PER_TRIGGER];
	unsigned long pool[BITS_TO_LONGS(CONFIG_IIO_CONSUMERS_PER_TRIGGER)];
	struct mutex			pool_lock;
};


static inline struct iio_trigger *to_iio_trigger(struct device *d)
{
	return container_of(d, struct iio_trigger, dev);
}

static inline void iio_trigger_put(struct iio_trigger *trig)
{
	module_put(trig->ops->owner);
	put_device(&trig->dev);
}

static inline struct iio_trigger *iio_trigger_get(struct iio_trigger *trig)
{
	get_device(&trig->dev);
	__module_get(trig->ops->owner);

	return trig;
}

/* 
                                                     
                               
                              
  
                                                                              
                                          
 */
static inline void iio_trigger_set_drvdata(struct iio_trigger *trig, void *data)
{
	dev_set_drvdata(&trig->dev, data);
}

/* 
                                                      
                               
  
                                                                 
 */
static inline void *iio_trigger_get_drvdata(struct iio_trigger *trig)
{
	return dev_get_drvdata(&trig->dev);
}

/* 
                                                                
                                       
  */
int iio_trigger_register(struct iio_trigger *trig_info);

/* 
                                                                
                                         
  */
void iio_trigger_unregister(struct iio_trigger *trig_info);

/* 
                                                     
                                
                                         
  
                                                           
  */
void iio_trigger_poll(struct iio_trigger *trig, s64 time);
void iio_trigger_poll_chained(struct iio_trigger *trig, s64 time);

irqreturn_t iio_trigger_generic_data_rdy_poll(int irq, void *private);

__printf(1, 2) struct iio_trigger *iio_trigger_alloc(const char *fmt, ...);
void iio_trigger_free(struct iio_trigger *trig);

#else
struct iio_trigger;
struct iio_trigger_ops;
#endif
#endif /*                 */
