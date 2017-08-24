/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright (C) 2012 John Crispin <blogic@openwrt.org>
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/platform_device.h>
#include <linux/mutex.h>
#include <linux/gpio.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/io.h>
#include <linux/slab.h>

#include <lantiq_soc.h>

/*
                                                                           
                                                                          
                                            
 */

#define LTQ_EBU_BUSCON	0x1e7ff		/*                               */
#define LTQ_EBU_WP	0x80000000	/*                   */

struct ltq_mm {
	struct of_mm_gpio_chip mmchip;
	u16 shadow;	/*                          */
};

/* 
                                                              
                                                    
  
                                                                         
                                                        
 */
static void ltq_mm_apply(struct ltq_mm *chip)
{
	unsigned long flags;

	spin_lock_irqsave(&ebu_lock, flags);
	ltq_ebu_w32(LTQ_EBU_BUSCON, LTQ_EBU_BUSCON1);
	__raw_writew(chip->shadow, chip->mmchip.regs);
	ltq_ebu_w32(LTQ_EBU_BUSCON | LTQ_EBU_WP, LTQ_EBU_BUSCON1);
	spin_unlock_irqrestore(&ebu_lock, flags);
}

/* 
                                             
                                                  
                               
                                                    
  
                                              
 */
static void ltq_mm_set(struct gpio_chip *gc, unsigned offset, int value)
{
	struct of_mm_gpio_chip *mm_gc = to_of_mm_gpio_chip(gc);
	struct ltq_mm *chip =
		container_of(mm_gc, struct ltq_mm, mmchip);

	if (value)
		chip->shadow |= (1 << offset);
	else
		chip->shadow &= ~(1 << offset);
	ltq_mm_apply(chip);
}

/* 
                                                              
                                                  
                               
                                                    
  
                                        
 */
static int ltq_mm_dir_out(struct gpio_chip *gc, unsigned offset, int value)
{
	ltq_mm_set(gc, offset, value);

	return 0;
}

/* 
                                                       
                                                       
 */
static void ltq_mm_save_regs(struct of_mm_gpio_chip *mm_gc)
{
	struct ltq_mm *chip =
		container_of(mm_gc, struct ltq_mm, mmchip);

	/*                                                               */
	ltq_ebu_w32(CPHYSADDR(chip->mmchip.regs) | 0x1, LTQ_EBU_ADDRSEL1);

	ltq_mm_apply(chip);
}

static int ltq_mm_probe(struct platform_device *pdev)
{
	struct resource *res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	struct ltq_mm *chip;
	const __be32 *shadow;
	int ret = 0;

	if (!res) {
		dev_err(&pdev->dev, "failed to get memory resource\n");
		return -ENOENT;
	}

	chip = kzalloc(sizeof(*chip), GFP_KERNEL);
	if (!chip)
		return -ENOMEM;

	chip->mmchip.gc.ngpio = 16;
	chip->mmchip.gc.label = "gpio-mm-ltq";
	chip->mmchip.gc.direction_output = ltq_mm_dir_out;
	chip->mmchip.gc.set = ltq_mm_set;
	chip->mmchip.save_regs = ltq_mm_save_regs;

	/*                                                            */
	shadow = of_get_property(pdev->dev.of_node, "lantiq,shadow", NULL);
	if (shadow)
		chip->shadow = be32_to_cpu(*shadow);

	ret = of_mm_gpiochip_add(pdev->dev.of_node, &chip->mmchip);
	if (ret)
		kfree(chip);
	return ret;
}

static const struct of_device_id ltq_mm_match[] = {
	{ .compatible = "lantiq,gpio-mm" },
	{},
};
MODULE_DEVICE_TABLE(of, ltq_mm_match);

static struct platform_driver ltq_mm_driver = {
	.probe = ltq_mm_probe,
	.driver = {
		.name = "gpio-mm-ltq",
		.owner = THIS_MODULE,
		.of_match_table = ltq_mm_match,
	},
};

static int __init ltq_mm_init(void)
{
	return platform_driver_register(&ltq_mm_driver);
}

subsys_initcall(ltq_mm_init);
