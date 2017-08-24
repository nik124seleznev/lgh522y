/*
 * Digital I/O driver for Technologic Systems TS-5500
 *
 * Copyright (c) 2012 Savoir-faire Linux Inc.
 *	Vivien Didelot <vivien.didelot@savoirfairelinux.com>
 *
 * Technologic Systems platforms have pin blocks, exposing several Digital
 * Input/Output lines (DIO). This driver aims to support single pin blocks.
 * In that sense, the support is not limited to the TS-5500 blocks.
 * Actually, the following platforms have DIO support:
 *
 * TS-5500:
 *   Documentation: http://wiki.embeddedarm.com/wiki/TS-5500
 *   Blocks: DIO1, DIO2 and LCD port.
 *
 * TS-5600:
 *   Documentation: http://wiki.embeddedarm.com/wiki/TS-5600
 *   Blocks: LCD port (identical to TS-5500 LCD).
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/bitops.h>
#include <linux/gpio.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/platform_data/gpio-ts5500.h>
#include <linux/platform_device.h>
#include <linux/slab.h>

/*                                                            */
enum ts5500_blocks { TS5500_DIO1, TS5500_DIO2, TS5500_LCD, TS5600_LCD };

struct ts5500_priv {
	const struct ts5500_dio *pinout;
	struct gpio_chip gpio_chip;
	spinlock_t lock;
	bool strap;
	u8 hwirq;
};

/*
                                                                     
                                                                       
 */
static bool hex7d_reserved;

/*
                                                                
                                                                 
 */
struct ts5500_dio {
	const u8 value_addr;
	const u8 value_mask;
	const u8 control_addr;
	const u8 control_mask;
	const bool no_input;
	const bool no_output;
	const u8 irq;
};

#define TS5500_DIO_IN_OUT(vaddr, vbit, caddr, cbit)	\
	{						\
		.value_addr = vaddr,			\
		.value_mask = BIT(vbit),		\
		.control_addr = caddr,			\
		.control_mask = BIT(cbit),		\
	}

#define TS5500_DIO_IN(addr, bit)		\
	{					\
		.value_addr = addr,		\
		.value_mask = BIT(bit),		\
		.no_output = true,		\
	}

#define TS5500_DIO_IN_IRQ(addr, bit, _irq)	\
	{					\
		.value_addr = addr,		\
		.value_mask = BIT(bit),		\
		.no_output = true,		\
		.irq = _irq,			\
	}

#define TS5500_DIO_OUT(addr, bit)		\
	{					\
		.value_addr = addr,		\
		.value_mask = BIT(bit),		\
		.no_input = true,		\
	}

/*
                                                                         
                                                                              
                                                              
 */
#define TS5500_DIO_GROUP(vaddr, vbitfrom, caddr, cbit)		\
	TS5500_DIO_IN_OUT(vaddr, vbitfrom + 0, caddr, cbit),	\
	TS5500_DIO_IN_OUT(vaddr, vbitfrom + 1, caddr, cbit),	\
	TS5500_DIO_IN_OUT(vaddr, vbitfrom + 2, caddr, cbit),	\
	TS5500_DIO_IN_OUT(vaddr, vbitfrom + 3, caddr, cbit)

/*
                     
  
                               
                                                    
  
                                               
                                               
                                               
                                               
                                               
                                               
                                               
                                               
                                               
                                               
                                                
                                                
                                                
                                                
 */
static const struct ts5500_dio ts5500_dio1[] = {
	TS5500_DIO_GROUP(0x7b, 0, 0x7a, 0),
	TS5500_DIO_GROUP(0x7b, 4, 0x7a, 1),
	TS5500_DIO_GROUP(0x7c, 0, 0x7a, 5),
	TS5500_DIO_IN(0x7c, 4),
	TS5500_DIO_IN_IRQ(0x7c, 5, 7),
};

/*
                     
  
                               
                                                    
  
                                               
                                               
                                               
                                               
                                               
                                               
                                               
                                               
                                               
                                               
                                                
                                                
                                                
 */
static const struct ts5500_dio ts5500_dio2[] = {
	TS5500_DIO_GROUP(0x7e, 0, 0x7d, 0),
	TS5500_DIO_GROUP(0x7e, 4, 0x7d, 1),
	TS5500_DIO_GROUP(0x7f, 0, 0x7d, 5),
	TS5500_DIO_IN_IRQ(0x7f, 4, 6),
};

/*
                                     
                                
  
                               
                                                   
  
                                              
                                              
                                              
                                              
                                              
                                              
                                              
                                              
                                              
                                              
                                               
 */
static const struct ts5500_dio ts5500_lcd[] = {
	TS5500_DIO_GROUP(0x72, 0, 0x7d, 2),
	TS5500_DIO_GROUP(0x72, 4, 0x7d, 3),
	TS5500_DIO_OUT(0x73, 0),
	TS5500_DIO_IN(0x73, 6),
	TS5500_DIO_IN_IRQ(0x73, 7, 1),
};

static inline struct ts5500_priv *ts5500_gc_to_priv(struct gpio_chip *chip)
{
	return container_of(chip, struct ts5500_priv, gpio_chip);
}

static inline void ts5500_set_mask(u8 mask, u8 addr)
{
	u8 val = inb(addr);
	val |= mask;
	outb(val, addr);
}

static inline void ts5500_clear_mask(u8 mask, u8 addr)
{
	u8 val = inb(addr);
	val &= ~mask;
	outb(val, addr);
}

static int ts5500_gpio_input(struct gpio_chip *chip, unsigned offset)
{
	struct ts5500_priv *priv = ts5500_gc_to_priv(chip);
	const struct ts5500_dio line = priv->pinout[offset];
	unsigned long flags;

	if (line.no_input)
		return -ENXIO;

	if (line.no_output)
		return 0;

	spin_lock_irqsave(&priv->lock, flags);
	ts5500_clear_mask(line.control_mask, line.control_addr);
	spin_unlock_irqrestore(&priv->lock, flags);

	return 0;
}

static int ts5500_gpio_get(struct gpio_chip *chip, unsigned offset)
{
	struct ts5500_priv *priv = ts5500_gc_to_priv(chip);
	const struct ts5500_dio line = priv->pinout[offset];

	return !!(inb(line.value_addr) & line.value_mask);
}

static int ts5500_gpio_output(struct gpio_chip *chip, unsigned offset, int val)
{
	struct ts5500_priv *priv = ts5500_gc_to_priv(chip);
	const struct ts5500_dio line = priv->pinout[offset];
	unsigned long flags;

	if (line.no_output)
		return -ENXIO;

	spin_lock_irqsave(&priv->lock, flags);
	if (!line.no_input)
		ts5500_set_mask(line.control_mask, line.control_addr);

	if (val)
		ts5500_set_mask(line.value_mask, line.value_addr);
	else
		ts5500_clear_mask(line.value_mask, line.value_addr);
	spin_unlock_irqrestore(&priv->lock, flags);

	return 0;
}

static void ts5500_gpio_set(struct gpio_chip *chip, unsigned offset, int val)
{
	struct ts5500_priv *priv = ts5500_gc_to_priv(chip);
	const struct ts5500_dio line = priv->pinout[offset];
	unsigned long flags;

	spin_lock_irqsave(&priv->lock, flags);
	if (val)
		ts5500_set_mask(line.value_mask, line.value_addr);
	else
		ts5500_clear_mask(line.value_mask, line.value_addr);
	spin_unlock_irqrestore(&priv->lock, flags);
}

static int ts5500_gpio_to_irq(struct gpio_chip *chip, unsigned offset)
{
	struct ts5500_priv *priv = ts5500_gc_to_priv(chip);
	const struct ts5500_dio *block = priv->pinout;
	const struct ts5500_dio line = block[offset];

	/*                                           */
	if (line.irq)
		return line.irq;

	/*                                                                  */
	if (priv->strap)
		return priv->hwirq;

	return -ENXIO;
}

static int ts5500_enable_irq(struct ts5500_priv *priv)
{
	int ret = 0;
	unsigned long flags;

	spin_lock_irqsave(&priv->lock, flags);
	if (priv->hwirq == 7)
		ts5500_set_mask(BIT(7), 0x7a); /*                 */
	else if (priv->hwirq == 6)
		ts5500_set_mask(BIT(7), 0x7d); /*                 */
	else if (priv->hwirq == 1)
		ts5500_set_mask(BIT(6), 0x7d); /*                */
	else
		ret = -EINVAL;
	spin_unlock_irqrestore(&priv->lock, flags);

	return ret;
}

static void ts5500_disable_irq(struct ts5500_priv *priv)
{
	unsigned long flags;

	spin_lock_irqsave(&priv->lock, flags);
	if (priv->hwirq == 7)
		ts5500_clear_mask(BIT(7), 0x7a); /*                 */
	else if (priv->hwirq == 6)
		ts5500_clear_mask(BIT(7), 0x7d); /*                 */
	else if (priv->hwirq == 1)
		ts5500_clear_mask(BIT(6), 0x7d); /*                */
	else
		dev_err(priv->gpio_chip.dev, "invalid hwirq %d\n", priv->hwirq);
	spin_unlock_irqrestore(&priv->lock, flags);
}

static int ts5500_dio_probe(struct platform_device *pdev)
{
	enum ts5500_blocks block = platform_get_device_id(pdev)->driver_data;
	struct ts5500_dio_platform_data *pdata = pdev->dev.platform_data;
	struct device *dev = &pdev->dev;
	const char *name = dev_name(dev);
	struct ts5500_priv *priv;
	struct resource *res;
	unsigned long flags;
	int ret;

	res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (!res) {
		dev_err(dev, "missing IRQ resource\n");
		return -EINVAL;
	}

	priv = devm_kzalloc(dev, sizeof(struct ts5500_priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	platform_set_drvdata(pdev, priv);
	priv->hwirq = res->start;
	spin_lock_init(&priv->lock);

	priv->gpio_chip.owner = THIS_MODULE;
	priv->gpio_chip.label = name;
	priv->gpio_chip.dev = dev;
	priv->gpio_chip.direction_input = ts5500_gpio_input;
	priv->gpio_chip.direction_output = ts5500_gpio_output;
	priv->gpio_chip.get = ts5500_gpio_get;
	priv->gpio_chip.set = ts5500_gpio_set;
	priv->gpio_chip.to_irq = ts5500_gpio_to_irq;
	priv->gpio_chip.base = -1;
	if (pdata) {
		priv->gpio_chip.base = pdata->base;
		priv->strap = pdata->strap;
	}

	switch (block) {
	case TS5500_DIO1:
		priv->pinout = ts5500_dio1;
		priv->gpio_chip.ngpio = ARRAY_SIZE(ts5500_dio1);

		if (!devm_request_region(dev, 0x7a, 3, name)) {
			dev_err(dev, "failed to request %s ports\n", name);
			return -EBUSY;
		}
		break;
	case TS5500_DIO2:
		priv->pinout = ts5500_dio2;
		priv->gpio_chip.ngpio = ARRAY_SIZE(ts5500_dio2);

		if (!devm_request_region(dev, 0x7e, 2, name)) {
			dev_err(dev, "failed to request %s ports\n", name);
			return -EBUSY;
		}

		if (hex7d_reserved)
			break;

		if (!devm_request_region(dev, 0x7d, 1, name)) {
			dev_err(dev, "failed to request %s 7D\n", name);
			return -EBUSY;
		}

		hex7d_reserved = true;
		break;
	case TS5500_LCD:
	case TS5600_LCD:
		priv->pinout = ts5500_lcd;
		priv->gpio_chip.ngpio = ARRAY_SIZE(ts5500_lcd);

		if (!devm_request_region(dev, 0x72, 2, name)) {
			dev_err(dev, "failed to request %s ports\n", name);
			return -EBUSY;
		}

		if (!hex7d_reserved) {
			if (!devm_request_region(dev, 0x7d, 1, name)) {
				dev_err(dev, "failed to request %s 7D\n", name);
				return -EBUSY;
			}

			hex7d_reserved = true;
		}

		/*                                 */
		spin_lock_irqsave(&priv->lock, flags);
		ts5500_clear_mask(BIT(4), 0x7d);
		spin_unlock_irqrestore(&priv->lock, flags);
		break;
	}

	ret = gpiochip_add(&priv->gpio_chip);
	if (ret) {
		dev_err(dev, "failed to register the gpio chip\n");
		return ret;
	}

	ret = ts5500_enable_irq(priv);
	if (ret) {
		dev_err(dev, "invalid interrupt %d\n", priv->hwirq);
		goto cleanup;
	}

	return 0;
cleanup:
	if (gpiochip_remove(&priv->gpio_chip))
		dev_err(dev, "failed to remove gpio chip\n");
	return ret;
}

static int ts5500_dio_remove(struct platform_device *pdev)
{
	struct ts5500_priv *priv = platform_get_drvdata(pdev);

	ts5500_disable_irq(priv);
	return gpiochip_remove(&priv->gpio_chip);
}

static struct platform_device_id ts5500_dio_ids[] = {
	{ "ts5500-dio1", TS5500_DIO1 },
	{ "ts5500-dio2", TS5500_DIO2 },
	{ "ts5500-dio-lcd", TS5500_LCD },
	{ "ts5600-dio-lcd", TS5600_LCD },
	{ }
};
MODULE_DEVICE_TABLE(platform, ts5500_dio_ids);

static struct platform_driver ts5500_dio_driver = {
	.driver = {
		.name = "ts5500-dio",
		.owner = THIS_MODULE,
	},
	.probe = ts5500_dio_probe,
	.remove = ts5500_dio_remove,
	.id_table = ts5500_dio_ids,
};

module_platform_driver(ts5500_dio_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Savoir-faire Linux Inc. <kernel@savoirfairelinux.com>");
MODULE_DESCRIPTION("Technologic Systems TS-5500 Digital I/O driver");
