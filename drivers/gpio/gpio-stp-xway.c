/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright (C) 2012 John Crispin <blogic@openwrt.org>
 *
 */

#include <linux/slab.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/of_platform.h>
#include <linux/mutex.h>
#include <linux/gpio.h>
#include <linux/io.h>
#include <linux/of_gpio.h>
#include <linux/clk.h>
#include <linux/err.h>

#include <lantiq_soc.h>

/*
                                                                           
                                                                                
                                                                                
                                                    
 */

/*                    */
#define XWAY_STP_CON0		0x00
/*                    */
#define XWAY_STP_CON1		0x04
/*                 */
#define XWAY_STP_CPU0		0x08
/*                 */
#define XWAY_STP_CPU1		0x0C
/*                 */
#define XWAY_STP_AR		0x10

/*                                        */
#define XWAY_STP_CON_SWU	BIT(31)

/*                        */
#define XWAY_STP_2HZ		0
#define XWAY_STP_4HZ		BIT(23)
#define XWAY_STP_8HZ		BIT(24)
#define XWAY_STP_10HZ		(BIT(24) | BIT(23))
#define XWAY_STP_SPEED_MASK	(0xf << 23)

/*                                   */
#define XWAY_STP_UPD_FPI	BIT(31)
#define XWAY_STP_UPD_MASK	(BIT(31) | BIT(30))

/*                                    */
#define XWAY_STP_ADSL_SHIFT	24
#define XWAY_STP_ADSL_MASK	0x3

/*                                              */
#define XWAY_STP_PHY_MASK	0x3
#define XWAY_STP_PHY1_SHIFT	27
#define XWAY_STP_PHY2_SHIFT	15

/*                            */
#define XWAY_STP_GROUP0		BIT(0)
#define XWAY_STP_GROUP1		BIT(1)
#define XWAY_STP_GROUP2		BIT(2)
#define XWAY_STP_GROUP_MASK	(0x7)

/*                         */
#define XWAY_STP_FALLING	BIT(26)
#define XWAY_STP_EDGE_MASK	BIT(26)

#define xway_stp_r32(m, reg)		__raw_readl(m + reg)
#define xway_stp_w32(m, val, reg)	__raw_writel(val, m + reg)
#define xway_stp_w32_mask(m, clear, set, reg) \
		ltq_w32((ltq_r32(m + reg) & ~(clear)) | (set), \
		m + reg)

struct xway_stp {
	struct gpio_chip gc;
	void __iomem *virt;
	u32 edge;	/*                                                 */
	u32 shadow;	/*                                  */
	u8 groups;	/*                                      */
	u8 dsl;		/*                                          */
	u8 phy1;	/*                              */
	u8 phy2;	/*                              */
	u8 reserved;	/*                                             */
};

/* 
                                               
                                                  
                               
                                                    
  
                                               
 */
static void xway_stp_set(struct gpio_chip *gc, unsigned gpio, int val)
{
	struct xway_stp *chip =
		container_of(gc, struct xway_stp, gc);

	if (val)
		chip->shadow |= BIT(gpio);
	else
		chip->shadow &= ~BIT(gpio);
	xway_stp_w32(chip->virt, chip->shadow, XWAY_STP_CPU0);
	xway_stp_w32_mask(chip->virt, 0, XWAY_STP_CON_SWU, XWAY_STP_CON0);
}

/* 
                                                                
                                                  
                               
                                                    
  
                                          
 */
static int xway_stp_dir_out(struct gpio_chip *gc, unsigned gpio, int val)
{
	xway_stp_set(gc, gpio, val);

	return 0;
}

/* 
                                          
                                                  
                               
  
                                 
 */
static int xway_stp_request(struct gpio_chip *gc, unsigned gpio)
{
	struct xway_stp *chip =
		container_of(gc, struct xway_stp, gc);

	if ((gpio < 8) && (chip->reserved & BIT(gpio))) {
		dev_err(gc->dev, "GPIO %d is driven by hardware\n", gpio);
		return -ENODEV;
	}

	return 0;
}

/* 
                                                                        
                                                
 */
static int xway_stp_hw_init(struct xway_stp *chip)
{
	/*               */
	xway_stp_w32(chip->virt, 0, XWAY_STP_AR);
	xway_stp_w32(chip->virt, 0, XWAY_STP_CPU0);
	xway_stp_w32(chip->virt, 0, XWAY_STP_CPU1);
	xway_stp_w32(chip->virt, XWAY_STP_CON_SWU, XWAY_STP_CON0);
	xway_stp_w32(chip->virt, 0, XWAY_STP_CON1);

	/*                                                    */
	xway_stp_w32_mask(chip->virt, XWAY_STP_EDGE_MASK,
				chip->edge, XWAY_STP_CON0);

	/*                          */
	xway_stp_w32_mask(chip->virt, XWAY_STP_GROUP_MASK,
				chip->groups, XWAY_STP_CON1);

	/*                                                              */
	xway_stp_w32_mask(chip->virt,
			XWAY_STP_ADSL_MASK << XWAY_STP_ADSL_SHIFT,
			chip->dsl << XWAY_STP_ADSL_SHIFT,
			XWAY_STP_CON0);

	/*                                                         */
	xway_stp_w32_mask(chip->virt,
			XWAY_STP_PHY_MASK << XWAY_STP_PHY1_SHIFT,
			chip->phy1 << XWAY_STP_PHY1_SHIFT,
			XWAY_STP_CON0);
	xway_stp_w32_mask(chip->virt,
			XWAY_STP_PHY_MASK << XWAY_STP_PHY2_SHIFT,
			chip->phy2 << XWAY_STP_PHY2_SHIFT,
			XWAY_STP_CON1);

	/*                                             */
	chip->reserved = (chip->phy2 << 5) | (chip->phy1 << 2) | chip->dsl;

	/*
                                                                       
                            
  */
	if (chip->reserved)
		xway_stp_w32_mask(chip->virt, XWAY_STP_UPD_MASK,
			XWAY_STP_UPD_FPI, XWAY_STP_CON1);

	return 0;
}

static int xway_stp_probe(struct platform_device *pdev)
{
	struct resource *res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	const __be32 *shadow, *groups, *dsl, *phy;
	struct xway_stp *chip;
	struct clk *clk;
	int ret = 0;

	if (!res) {
		dev_err(&pdev->dev, "failed to request STP resource\n");
		return -ENOENT;
	}

	chip = devm_kzalloc(&pdev->dev, sizeof(*chip), GFP_KERNEL);
	if (!chip)
		return -ENOMEM;

	chip->virt = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(chip->virt))
		return PTR_ERR(chip->virt);

	chip->gc.dev = &pdev->dev;
	chip->gc.label = "stp-xway";
	chip->gc.direction_output = xway_stp_dir_out;
	chip->gc.set = xway_stp_set;
	chip->gc.request = xway_stp_request;
	chip->gc.base = -1;
	chip->gc.owner = THIS_MODULE;

	/*                                                            */
	shadow = of_get_property(pdev->dev.of_node, "lantiq,shadow", NULL);
	if (shadow)
		chip->shadow = be32_to_cpu(*shadow);

	/*                                              */
	groups = of_get_property(pdev->dev.of_node, "lantiq,groups", NULL);
	if (groups)
		chip->groups = be32_to_cpu(*groups) & XWAY_STP_GROUP_MASK;
	else
		chip->groups = XWAY_STP_GROUP0;
	chip->gc.ngpio = fls(chip->groups) * 8;

	/*                                                     */
	dsl = of_get_property(pdev->dev.of_node, "lantiq,dsl", NULL);
	if (dsl)
		chip->dsl = be32_to_cpu(*dsl) & XWAY_STP_ADSL_MASK;

	/*                                                 */
	if (of_machine_is_compatible("lantiq,ar9") ||
			of_machine_is_compatible("lantiq,gr9") ||
			of_machine_is_compatible("lantiq,vr9")) {
		phy = of_get_property(pdev->dev.of_node, "lantiq,phy1", NULL);
		if (phy)
			chip->phy1 = be32_to_cpu(*phy) & XWAY_STP_PHY_MASK;
		phy = of_get_property(pdev->dev.of_node, "lantiq,phy2", NULL);
		if (phy)
			chip->phy2 = be32_to_cpu(*phy) & XWAY_STP_PHY_MASK;
	}

	/*                                                                   */
	if (!of_find_property(pdev->dev.of_node, "lantiq,rising", NULL))
		chip->edge = XWAY_STP_FALLING;

	clk = clk_get(&pdev->dev, NULL);
	if (IS_ERR(clk)) {
		dev_err(&pdev->dev, "Failed to get clock\n");
		return PTR_ERR(clk);
	}
	clk_enable(clk);

	ret = xway_stp_hw_init(chip);
	if (!ret)
		ret = gpiochip_add(&chip->gc);

	if (!ret)
		dev_info(&pdev->dev, "Init done\n");

	return ret;
}

static const struct of_device_id xway_stp_match[] = {
	{ .compatible = "lantiq,gpio-stp-xway" },
	{},
};
MODULE_DEVICE_TABLE(of, xway_stp_match);

static struct platform_driver xway_stp_driver = {
	.probe = xway_stp_probe,
	.driver = {
		.name = "gpio-stp-xway",
		.owner = THIS_MODULE,
		.of_match_table = xway_stp_match,
	},
};

int __init xway_stp_init(void)
{
	return platform_driver_register(&xway_stp_driver);
}

subsys_initcall(xway_stp_init);
