/*
 * OF helpers for the GPIO API
 *
 * Copyright (c) 2007-2008  MontaVista Software, Inc.
 *
 * Author: Anton Vorontsov <avorontsov@ru.mvista.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef __LINUX_OF_GPIO_H
#define __LINUX_OF_GPIO_H

#include <linux/compiler.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/gpio.h>
#include <linux/of.h>

struct device_node;

/*
                                                                           
                                                                       
                                                                               
 */
enum of_gpio_flags {
	OF_GPIO_ACTIVE_LOW = 0x1,
};

#ifdef CONFIG_OF_GPIO

/*
                                       
 */
struct of_mm_gpio_chip {
	struct gpio_chip gc;
	void (*save_regs)(struct of_mm_gpio_chip *mm_gc);
	void __iomem *regs;
};

static inline struct of_mm_gpio_chip *to_of_mm_gpio_chip(struct gpio_chip *gc)
{
	return container_of(gc, struct of_mm_gpio_chip, gc);
}

extern int of_get_named_gpio_flags(struct device_node *np,
		const char *list_name, int index, enum of_gpio_flags *flags);

extern int of_mm_gpiochip_add(struct device_node *np,
			      struct of_mm_gpio_chip *mm_gc);

extern void of_gpiochip_add(struct gpio_chip *gc);
extern void of_gpiochip_remove(struct gpio_chip *gc);
extern int of_gpio_simple_xlate(struct gpio_chip *gc,
				const struct of_phandle_args *gpiospec,
				u32 *flags);

#else /*                */

/*                                                                        */
static inline int of_get_named_gpio_flags(struct device_node *np,
		const char *list_name, int index, enum of_gpio_flags *flags)
{
	return -ENOSYS;
}

static inline int of_gpio_simple_xlate(struct gpio_chip *gc,
				       const struct of_phandle_args *gpiospec,
				       u32 *flags)
{
	return -ENOSYS;
}

static inline void of_gpiochip_add(struct gpio_chip *gc) { }
static inline void of_gpiochip_remove(struct gpio_chip *gc) { }

#endif /*                */

/* 
                                                   
                                       
                                                        
  
                                                                
                                                                
                                         
                                                         
                                         
  
           
             
                      
             
                        
  
                                                                        
                                
 */
static inline int of_gpio_named_count(struct device_node *np, const char* propname)
{
	return of_count_phandle_with_args(np, propname, "#gpio-cells");
}

/* 
                                             
                                       
  
                                                                          
 */
static inline int of_gpio_count(struct device_node *np)
{
	return of_gpio_named_count(np, "gpios");
}

/* 
                                                                         
                                     
                            
                                     
  
                                                                              
                                                                              
                         
 */
static inline int of_get_gpio_flags(struct device_node *np, int index,
		      enum of_gpio_flags *flags)
{
	return of_get_named_gpio_flags(np, "gpios", index, flags);
}

/* 
                                                               
                                     
                                                           
                            
  
                                                                              
                                
 */
static inline int of_get_named_gpio(struct device_node *np,
                                   const char *propname, int index)
{
	return of_get_named_gpio_flags(np, propname, index, NULL);
}

/* 
                                                         
                                     
                            
  
                                                                              
                                
 */
static inline int of_get_gpio(struct device_node *np, int index)
{
	return of_get_gpio_flags(np, index, NULL);
}

#endif /*                   */