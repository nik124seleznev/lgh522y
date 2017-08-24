/*
 * pin-controller/pin-mux/pin-config/gpio-driver for Samsung's SoC's.
 *
 * Copyright (c) 2012 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 * Copyright (c) 2012 Linaro Ltd
 *		http://www.linaro.org
 *
 * Author: Thomas Abraham <thomas.ab@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef __PINCTRL_SAMSUNG_H
#define __PINCTRL_SAMSUNG_H

#include <linux/pinctrl/pinctrl.h>
#include <linux/pinctrl/pinmux.h>
#include <linux/pinctrl/pinconf.h>
#include <linux/pinctrl/consumer.h>
#include <linux/pinctrl/machine.h>

#include <linux/gpio.h>

/*                                                    */
#define FUNC_OUTPUT	0x1

/* 
                                                                 
                                             
                                             
                                                
                                                  
                                                         
                                                                       
 */
enum pincfg_type {
	PINCFG_TYPE_FUNC,
	PINCFG_TYPE_DAT,
	PINCFG_TYPE_PUD,
	PINCFG_TYPE_DRV,
	PINCFG_TYPE_CON_PDN,
	PINCFG_TYPE_PUD_PDN,

	PINCFG_TYPE_NUM
};

/*
                                                                             
                                                                               
                                                                      
 */
#define PINCFG_TYPE_MASK		0xFF
#define PINCFG_VALUE_SHIFT		8
#define PINCFG_VALUE_MASK		(0xFF << PINCFG_VALUE_SHIFT)
#define PINCFG_PACK(type, value)	(((value) << PINCFG_VALUE_SHIFT) | type)
#define PINCFG_UNPACK_TYPE(cfg)		((cfg) & PINCFG_TYPE_MASK)
#define PINCFG_UNPACK_VALUE(cfg)	(((cfg) & PINCFG_VALUE_MASK) >> \
						PINCFG_VALUE_SHIFT)
/* 
                                                      
                                                             
                                                           
                                                             
                                                                            
  
                                                                             
                                                                        
                                                                            
                                                                          
                                                                       
                   
 */
enum eint_type {
	EINT_TYPE_NONE,
	EINT_TYPE_GPIO,
	EINT_TYPE_WKUP,
	EINT_TYPE_WKUP_MUX,
};

/*                                                               */
#define PIN_NAME_LENGTH	10

#define PIN_GROUP(n, p, f)				\
	{						\
		.name		= n,			\
		.pins		= p,			\
		.num_pins	= ARRAY_SIZE(p),	\
		.func		= f			\
	}

#define PMX_FUNC(n, g)					\
	{						\
		.name		= n,			\
		.groups		= g,			\
		.num_groups	= ARRAY_SIZE(g),	\
	}

struct samsung_pinctrl_drv_data;

/* 
                                                          
                                                                   
                                                                             
 */
struct samsung_pin_bank_type {
	u8 fld_width[PINCFG_TYPE_NUM];
	u8 reg_offset[PINCFG_TYPE_NUM];
};

/* 
                                                            
                                                                 
                                                           
                                              
                                                  
                                                                        
                                                                    
                                                            
                                                            
                                 
                                           
                                       
                                     
                                                        
                                             
                                                 
 */
struct samsung_pin_bank {
	struct samsung_pin_bank_type *type;
	u32		pctl_offset;
	u32		pin_base;
	u8		nr_pins;
	u8		eint_func;
	enum eint_type	eint_type;
	u32		eint_mask;
	u32		eint_offset;
	char		*name;
	void		*soc_priv;
	struct device_node *of_node;
	struct samsung_pinctrl_drv_data *drvdata;
	struct irq_domain *irq_domain;
	struct gpio_chip gpio_chip;
	struct pinctrl_gpio_range grange;
	spinlock_t slock;

	u32 pm_save[PINCFG_TYPE_NUM + 1]; /*                                  */
};

/* 
                                                       
                                                             
                                  
                                          
                                                        
                                                           
                                                                
                                                                   
                                                             
                                                                  
                                                                     
                                                  
                                                                         
                                 
                                                                           
                                 
                                 
 */
struct samsung_pin_ctrl {
	struct samsung_pin_bank	*pin_banks;
	u32		nr_banks;

	u32		base;
	u32		nr_pins;

	u32		geint_con;
	u32		geint_mask;
	u32		geint_pend;

	u32		weint_con;
	u32		weint_mask;
	u32		weint_pend;

	u32		svc;

	int		(*eint_gpio_init)(struct samsung_pinctrl_drv_data *);
	int		(*eint_wkup_init)(struct samsung_pinctrl_drv_data *);
	void		(*suspend)(struct samsung_pinctrl_drv_data *);
	void		(*resume)(struct samsung_pinctrl_drv_data *);

	char		*label;
};

/* 
                                                                             
                          
                                                       
                                                     
                                                                          
                                                        
                                                                          
                                                          
                                                           
                                         
                                                                 
                                              
 */
struct samsung_pinctrl_drv_data {
	struct list_head		node;
	void __iomem			*virt_base;
	struct device			*dev;
	int				irq;

	struct samsung_pin_ctrl		*ctrl;
	struct pinctrl_desc		pctl;
	struct pinctrl_dev		*pctl_dev;

	const struct samsung_pin_group	*pin_groups;
	unsigned int			nr_groups;
	const struct samsung_pmx_func	*pmx_functions;
	unsigned int			nr_functions;
};

/* 
                                                                          
                                                          
                                          
                                                    
                                                             
 */
struct samsung_pin_group {
	const char		*name;
	const unsigned int	*pins;
	u8			num_pins;
	u8			func;
};

/* 
                                                     
                                                                
                                                                       
                                                     
 */
struct samsung_pmx_func {
	const char		*name;
	const char		**groups;
	u8			num_groups;
};

/*                                        */
extern struct samsung_pin_ctrl exynos4210_pin_ctrl[];
extern struct samsung_pin_ctrl exynos4x12_pin_ctrl[];
extern struct samsung_pin_ctrl exynos5250_pin_ctrl[];
extern struct samsung_pin_ctrl s3c64xx_pin_ctrl[];

#endif /*                     */
