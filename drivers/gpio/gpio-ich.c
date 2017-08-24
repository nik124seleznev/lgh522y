/*
 * Intel ICH6-10, Series 5 and 6 GPIO driver
 *
 * Copyright (C) 2010 Extreme Engineering Solutions.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/pci.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/mfd/lpc_ich.h>

#define DRV_NAME "gpio_ich"

/*
                                           
                                                                           
                                                                          
                                                                           
                                                                          
                                                                   
                  
 */
enum GPIO_REG {
	GPIO_USE_SEL = 0,
	GPIO_IO_SEL,
	GPIO_LVL,
};

static const u8 ichx_regs[3][3] = {
	{0x00, 0x30, 0x40},	/*                      */
	{0x04, 0x34, 0x44},	/*                     */
	{0x0c, 0x38, 0x48},	/*                  */
};

static const u8 ichx_reglen[3] = {
	0x30, 0x10, 0x10,
};

#define ICHX_WRITE(val, reg, base_res)	outl(val, (reg) + (base_res)->start)
#define ICHX_READ(reg, base_res)	inl((reg) + (base_res)->start)

struct ichx_desc {
	/*                                    */
	uint ngpio;

	/*                                                              */
	bool uses_gpe0;

	/*                                            */
	u32 use_sel_ignore[3];

	/*                                                                */
	int (*request)(struct gpio_chip *chip, unsigned offset);
	int (*get)(struct gpio_chip *chip, unsigned offset);
};

static struct {
	spinlock_t lock;
	struct platform_device *dev;
	struct gpio_chip chip;
	struct resource *gpio_base;	/*              */
	struct resource *pm_base;	/*                          */
	struct ichx_desc *desc;	/*                                         */
	u32 orig_gpio_ctrl;	/*                                          */
	u8 use_gpio;		/*                              */
} ichx_priv;

static int modparam_gpiobase = -1;	/*         */
module_param_named(gpiobase, modparam_gpiobase, int, 0444);
MODULE_PARM_DESC(gpiobase, "The GPIO number base. -1 means dynamic, "
			   "which is the default.");

static int ichx_write_bit(int reg, unsigned nr, int val, int verify)
{
	unsigned long flags;
	u32 data, tmp;
	int reg_nr = nr / 32;
	int bit = nr & 0x1f;
	int ret = 0;

	spin_lock_irqsave(&ichx_priv.lock, flags);

	data = ICHX_READ(ichx_regs[reg][reg_nr], ichx_priv.gpio_base);
	if (val)
		data |= 1 << bit;
	else
		data &= ~(1 << bit);
	ICHX_WRITE(data, ichx_regs[reg][reg_nr], ichx_priv.gpio_base);
	tmp = ICHX_READ(ichx_regs[reg][reg_nr], ichx_priv.gpio_base);
	if (verify && data != tmp)
		ret = -EPERM;

	spin_unlock_irqrestore(&ichx_priv.lock, flags);

	return ret;
}

static int ichx_read_bit(int reg, unsigned nr)
{
	unsigned long flags;
	u32 data;
	int reg_nr = nr / 32;
	int bit = nr & 0x1f;

	spin_lock_irqsave(&ichx_priv.lock, flags);

	data = ICHX_READ(ichx_regs[reg][reg_nr], ichx_priv.gpio_base);

	spin_unlock_irqrestore(&ichx_priv.lock, flags);

	return data & (1 << bit) ? 1 : 0;
}

static bool ichx_gpio_check_available(struct gpio_chip *gpio, unsigned nr)
{
	return !!(ichx_priv.use_gpio & (1 << (nr / 32)));
}

static int ichx_gpio_direction_input(struct gpio_chip *gpio, unsigned nr)
{
	/*
                                                                    
                    
  */
	if (ichx_write_bit(GPIO_IO_SEL, nr, 1, 1))
		return -EINVAL;

	return 0;
}

static int ichx_gpio_direction_output(struct gpio_chip *gpio, unsigned nr,
					int val)
{
	/*                        */
	ichx_write_bit(GPIO_LVL, nr, val, 0);

	/*
                                                                     
                   
  */
	if (ichx_write_bit(GPIO_IO_SEL, nr, 0, 1))
		return -EINVAL;

	return 0;
}

static int ichx_gpio_get(struct gpio_chip *chip, unsigned nr)
{
	return ichx_read_bit(GPIO_LVL, nr);
}

static int ich6_gpio_get(struct gpio_chip *chip, unsigned nr)
{
	unsigned long flags;
	u32 data;

	/*
                                                                     
                       
  */
	if (nr < 16) {
		if (!ichx_priv.pm_base)
			return -ENXIO;

		spin_lock_irqsave(&ichx_priv.lock, flags);

		/*                                         */
		ICHX_WRITE(1 << (16 + nr), 0, ichx_priv.pm_base);
		data = ICHX_READ(0, ichx_priv.pm_base);

		spin_unlock_irqrestore(&ichx_priv.lock, flags);

		return (data >> 16) & (1 << nr) ? 1 : 0;
	} else {
		return ichx_gpio_get(chip, nr);
	}
}

static int ichx_gpio_request(struct gpio_chip *chip, unsigned nr)
{
	if (!ichx_gpio_check_available(chip, nr))
		return -ENXIO;

	/*
                                                                    
                                                                       
                                                                 
                                                                      
  */
	if (ichx_priv.desc->use_sel_ignore[nr / 32] & (1 << (nr & 0x1f)))
		return 0;

	return ichx_read_bit(GPIO_USE_SEL, nr) ? 0 : -ENODEV;
}

static int ich6_gpio_request(struct gpio_chip *chip, unsigned nr)
{
	/*
                                                                  
                                                                    
                                                                 
                    
  */
	if (nr == 16 || nr == 17)
		nr -= 16;

	return ichx_gpio_request(chip, nr);
}

static void ichx_gpio_set(struct gpio_chip *chip, unsigned nr, int val)
{
	ichx_write_bit(GPIO_LVL, nr, val, 0);
}

static void ichx_gpiolib_setup(struct gpio_chip *chip)
{
	chip->owner = THIS_MODULE;
	chip->label = DRV_NAME;
	chip->dev = &ichx_priv.dev->dev;

	/*                                                  */
	chip->request = ichx_priv.desc->request ?
		ichx_priv.desc->request : ichx_gpio_request;
	chip->get = ichx_priv.desc->get ?
		ichx_priv.desc->get : ichx_gpio_get;

	chip->set = ichx_gpio_set;
	chip->direction_input = ichx_gpio_direction_input;
	chip->direction_output = ichx_gpio_direction_output;
	chip->base = modparam_gpiobase;
	chip->ngpio = ichx_priv.desc->ngpio;
	chip->can_sleep = 0;
	chip->dbg_show = NULL;
}

/*                           */
static struct ichx_desc ich6_desc = {
	/*                                                               */
	.request = ich6_gpio_request,
	.get = ich6_gpio_get,

	/*                                                */
	.uses_gpe0 = true,

	.ngpio = 50,
};

/*            */
static struct ichx_desc i3100_desc = {
	/*
                                                                     
                                                                
                            
  */
	.use_sel_ignore = {0x00130000, 0x00010000, 0x0},

	/*                                       */
	.request = ich6_gpio_request,
	.get = ich6_gpio_get,

	/*                                                */
	.uses_gpe0 = true,

	.ngpio = 50,
};

/*                     */
static struct ichx_desc ich7_desc = {
	.ngpio = 50,
};

/*            */
static struct ichx_desc ich9_desc = {
	.ngpio = 61,
};

/*                                                                         */
static struct ichx_desc ich10_cons_desc = {
	.ngpio = 61,
};
static struct ichx_desc ich10_corp_desc = {
	.ngpio = 72,
};

/*                                                        */
static struct ichx_desc intel5_desc = {
	.ngpio = 76,
};

static int ichx_gpio_request_regions(struct resource *res_base,
						const char *name, u8 use_gpio)
{
	int i;

	if (!res_base || !res_base->start || !res_base->end)
		return -ENODEV;

	for (i = 0; i < ARRAY_SIZE(ichx_regs[0]); i++) {
		if (!(use_gpio & (1 << i)))
			continue;
		if (!request_region(res_base->start + ichx_regs[0][i],
				    ichx_reglen[i], name))
			goto request_err;
	}
	return 0;

request_err:
	/*                                                     */
	for (i--; i >= 0; i--) {
		if (!(use_gpio & (1 << i)))
			continue;
		release_region(res_base->start + ichx_regs[0][i],
			       ichx_reglen[i]);
	}
	return -EBUSY;
}

static void ichx_gpio_release_regions(struct resource *res_base, u8 use_gpio)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(ichx_regs[0]); i++) {
		if (!(use_gpio & (1 << i)))
			continue;
		release_region(res_base->start + ichx_regs[0][i],
			       ichx_reglen[i]);
	}
}

static int ichx_gpio_probe(struct platform_device *pdev)
{
	struct resource *res_base, *res_pm;
	int err;
	struct lpc_ich_info *ich_info = pdev->dev.platform_data;

	if (!ich_info)
		return -ENODEV;

	ichx_priv.dev = pdev;

	switch (ich_info->gpio_version) {
	case ICH_I3100_GPIO:
		ichx_priv.desc = &i3100_desc;
		break;
	case ICH_V5_GPIO:
		ichx_priv.desc = &intel5_desc;
		break;
	case ICH_V6_GPIO:
		ichx_priv.desc = &ich6_desc;
		break;
	case ICH_V7_GPIO:
		ichx_priv.desc = &ich7_desc;
		break;
	case ICH_V9_GPIO:
		ichx_priv.desc = &ich9_desc;
		break;
	case ICH_V10CORP_GPIO:
		ichx_priv.desc = &ich10_corp_desc;
		break;
	case ICH_V10CONS_GPIO:
		ichx_priv.desc = &ich10_cons_desc;
		break;
	default:
		return -ENODEV;
	}

	spin_lock_init(&ichx_priv.lock);
	res_base = platform_get_resource(pdev, IORESOURCE_IO, ICH_RES_GPIO);
	ichx_priv.use_gpio = ich_info->use_gpio;
	err = ichx_gpio_request_regions(res_base, pdev->name,
					ichx_priv.use_gpio);
	if (err)
		return err;

	ichx_priv.gpio_base = res_base;

	/*
                                                                    
                                                                         
                            
  */
	if (!ichx_priv.desc->uses_gpe0)
		goto init;

	res_pm = platform_get_resource(pdev, IORESOURCE_IO, ICH_RES_GPE0);
	if (!res_pm) {
		pr_warn("ACPI BAR is unavailable, GPI 0 - 15 unavailable\n");
		goto init;
	}

	if (!request_region(res_pm->start, resource_size(res_pm),
			pdev->name)) {
		pr_warn("ACPI BAR is busy, GPI 0 - 15 unavailable\n");
		goto init;
	}

	ichx_priv.pm_base = res_pm;

init:
	ichx_gpiolib_setup(&ichx_priv.chip);
	err = gpiochip_add(&ichx_priv.chip);
	if (err) {
		pr_err("Failed to register GPIOs\n");
		goto add_err;
	}

	pr_info("GPIO from %d to %d on %s\n", ichx_priv.chip.base,
	       ichx_priv.chip.base + ichx_priv.chip.ngpio - 1, DRV_NAME);

	return 0;

add_err:
	ichx_gpio_release_regions(ichx_priv.gpio_base, ichx_priv.use_gpio);
	if (ichx_priv.pm_base)
		release_region(ichx_priv.pm_base->start,
				resource_size(ichx_priv.pm_base));
	return err;
}

static int ichx_gpio_remove(struct platform_device *pdev)
{
	int err;

	err = gpiochip_remove(&ichx_priv.chip);
	if (err) {
		dev_err(&pdev->dev, "%s failed, %d\n",
				"gpiochip_remove()", err);
		return err;
	}

	ichx_gpio_release_regions(ichx_priv.gpio_base, ichx_priv.use_gpio);
	if (ichx_priv.pm_base)
		release_region(ichx_priv.pm_base->start,
				resource_size(ichx_priv.pm_base));

	return 0;
}

static struct platform_driver ichx_gpio_driver = {
	.driver		= {
		.owner	= THIS_MODULE,
		.name	= DRV_NAME,
	},
	.probe		= ichx_gpio_probe,
	.remove		= ichx_gpio_remove,
};

module_platform_driver(ichx_gpio_driver);

MODULE_AUTHOR("Peter Tyser <ptyser@xes-inc.com>");
MODULE_DESCRIPTION("GPIO interface for Intel ICH series");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:"DRV_NAME);
