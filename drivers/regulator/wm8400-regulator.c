/*
 * Regulator support for WM8400
 *
 * Copyright 2008 Wolfson Microelectronics PLC.
 *
 * Author: Mark Brown <broonie@opensource.wolfsonmicro.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 */

#include <linux/bug.h>
#include <linux/err.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/regulator/driver.h>
#include <linux/mfd/wm8400-private.h>

static int wm8400_ldo_list_voltage(struct regulator_dev *dev,
				   unsigned selector)
{
	if (selector > WM8400_LDO1_VSEL_MASK)
		return -EINVAL;

	if (selector < 15)
		return 900000 + (selector * 50000);
	else
		return 1700000 + ((selector - 15) * 100000);
}

static int wm8400_ldo_map_voltage(struct regulator_dev *dev,
				  int min_uV, int max_uV)
{
	u16 val;
	int volt;

	if (min_uV < 900000 || min_uV > 3300000)
		return -EINVAL;

	if (min_uV < 1700000) /*                            */
		val = DIV_ROUND_UP(min_uV - 900000, 50000);
	else /*                            */
		val = DIV_ROUND_UP(min_uV - 1700000, 100000) + 15;

	volt = wm8400_ldo_list_voltage(dev, val);
	if (volt < min_uV || volt > max_uV)
		return -EINVAL;

	return val;
}

static struct regulator_ops wm8400_ldo_ops = {
	.is_enabled = regulator_is_enabled_regmap,
	.enable = regulator_enable_regmap,
	.disable = regulator_disable_regmap,
	.list_voltage = wm8400_ldo_list_voltage,
	.get_voltage_sel = regulator_get_voltage_sel_regmap,
	.set_voltage_sel = regulator_set_voltage_sel_regmap,
	.map_voltage = wm8400_ldo_map_voltage,
};

static unsigned int wm8400_dcdc_get_mode(struct regulator_dev *dev)
{
	struct wm8400 *wm8400 = rdev_get_drvdata(dev);
	int offset = (rdev_get_id(dev) - WM8400_DCDC1) * 2;
	u16 data[2];
	int ret;

	ret = wm8400_block_read(wm8400, WM8400_DCDC1_CONTROL_1 + offset, 2,
				data);
	if (ret != 0)
		return 0;

	/*                      */
	if (data[0] & WM8400_DC1_SLEEP)
		return REGULATOR_MODE_STANDBY;

	/*                    */
	if (!(data[0] & WM8400_DC1_ACTIVE))
		return REGULATOR_MODE_IDLE;

	/*                                             */
	if (data[1] & WM8400_DC1_FRC_PWM)
		return REGULATOR_MODE_FAST;
	else
		return REGULATOR_MODE_NORMAL;
}

static int wm8400_dcdc_set_mode(struct regulator_dev *dev, unsigned int mode)
{
	struct wm8400 *wm8400 = rdev_get_drvdata(dev);
	int offset = (rdev_get_id(dev) - WM8400_DCDC1) * 2;
	int ret;

	switch (mode) {
	case REGULATOR_MODE_FAST:
		/*                                  */
		ret = wm8400_set_bits(wm8400, WM8400_DCDC1_CONTROL_2 + offset,
				      WM8400_DC1_FRC_PWM, WM8400_DC1_FRC_PWM);
		if (ret != 0)
			return ret;

		return wm8400_set_bits(wm8400, WM8400_DCDC1_CONTROL_1 + offset,
				       WM8400_DC1_ACTIVE | WM8400_DC1_SLEEP,
				       WM8400_DC1_ACTIVE);

	case REGULATOR_MODE_NORMAL:
		/*                   */
		ret = wm8400_set_bits(wm8400, WM8400_DCDC1_CONTROL_2 + offset,
				      WM8400_DC1_FRC_PWM, 0);
		if (ret != 0)
			return ret;

		return wm8400_set_bits(wm8400, WM8400_DCDC1_CONTROL_1 + offset,
				       WM8400_DC1_ACTIVE | WM8400_DC1_SLEEP,
				       WM8400_DC1_ACTIVE);

	case REGULATOR_MODE_IDLE:
		/*                    */
		return wm8400_set_bits(wm8400, WM8400_DCDC1_CONTROL_1 + offset,
				       WM8400_DC1_ACTIVE | WM8400_DC1_SLEEP, 0);
	default:
		return -EINVAL;
	}
}

static unsigned int wm8400_dcdc_get_optimum_mode(struct regulator_dev *dev,
						 int input_uV, int output_uV,
						 int load_uA)
{
	return REGULATOR_MODE_NORMAL;
}

static struct regulator_ops wm8400_dcdc_ops = {
	.is_enabled = regulator_is_enabled_regmap,
	.enable = regulator_enable_regmap,
	.disable = regulator_disable_regmap,
	.list_voltage = regulator_list_voltage_linear,
	.map_voltage = regulator_map_voltage_linear,
	.get_voltage_sel = regulator_get_voltage_sel_regmap,
	.set_voltage_sel = regulator_set_voltage_sel_regmap,
	.get_mode = wm8400_dcdc_get_mode,
	.set_mode = wm8400_dcdc_set_mode,
	.get_optimum_mode = wm8400_dcdc_get_optimum_mode,
};

static struct regulator_desc regulators[] = {
	{
		.name = "LDO1",
		.id = WM8400_LDO1,
		.ops = &wm8400_ldo_ops,
		.enable_reg = WM8400_LDO1_CONTROL,
		.enable_mask = WM8400_LDO1_ENA,
		.n_voltages = WM8400_LDO1_VSEL_MASK + 1,
		.vsel_reg = WM8400_LDO1_CONTROL,
		.vsel_mask = WM8400_LDO1_VSEL_MASK,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	{
		.name = "LDO2",
		.id = WM8400_LDO2,
		.ops = &wm8400_ldo_ops,
		.enable_reg = WM8400_LDO2_CONTROL,
		.enable_mask = WM8400_LDO2_ENA,
		.n_voltages = WM8400_LDO2_VSEL_MASK + 1,
		.type = REGULATOR_VOLTAGE,
		.vsel_reg = WM8400_LDO2_CONTROL,
		.vsel_mask = WM8400_LDO2_VSEL_MASK,
		.owner = THIS_MODULE,
	},
	{
		.name = "LDO3",
		.id = WM8400_LDO3,
		.ops = &wm8400_ldo_ops,
		.enable_reg = WM8400_LDO3_CONTROL,
		.enable_mask = WM8400_LDO3_ENA,
		.n_voltages = WM8400_LDO3_VSEL_MASK + 1,
		.vsel_reg = WM8400_LDO3_CONTROL,
		.vsel_mask = WM8400_LDO3_VSEL_MASK,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	{
		.name = "LDO4",
		.id = WM8400_LDO4,
		.ops = &wm8400_ldo_ops,
		.enable_reg = WM8400_LDO4_CONTROL,
		.enable_mask = WM8400_LDO4_ENA,
		.n_voltages = WM8400_LDO4_VSEL_MASK + 1,
		.vsel_reg = WM8400_LDO4_CONTROL,
		.vsel_mask = WM8400_LDO4_VSEL_MASK,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	{
		.name = "DCDC1",
		.id = WM8400_DCDC1,
		.ops = &wm8400_dcdc_ops,
		.enable_reg = WM8400_DCDC1_CONTROL_1,
		.enable_mask = WM8400_DC1_ENA_MASK,
		.n_voltages = WM8400_DC1_VSEL_MASK + 1,
		.vsel_reg = WM8400_DCDC1_CONTROL_1,
		.vsel_mask = WM8400_DC1_VSEL_MASK,
		.min_uV = 850000,
		.uV_step = 25000,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	{
		.name = "DCDC2",
		.id = WM8400_DCDC2,
		.ops = &wm8400_dcdc_ops,
		.enable_reg = WM8400_DCDC2_CONTROL_1,
		.enable_mask = WM8400_DC1_ENA_MASK,
		.n_voltages = WM8400_DC2_VSEL_MASK + 1,
		.vsel_reg = WM8400_DCDC2_CONTROL_1,
		.vsel_mask = WM8400_DC2_VSEL_MASK,
		.min_uV = 850000,
		.uV_step = 25000,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
};

static int wm8400_regulator_probe(struct platform_device *pdev)
{
	struct wm8400 *wm8400 = container_of(pdev, struct wm8400, regulators[pdev->id]);
	struct regulator_config config = { };
	struct regulator_dev *rdev;

	config.dev = &pdev->dev;
	config.init_data = pdev->dev.platform_data;
	config.driver_data = wm8400;
	config.regmap = wm8400->regmap;

	rdev = regulator_register(&regulators[pdev->id], &config);
	if (IS_ERR(rdev))
		return PTR_ERR(rdev);

	platform_set_drvdata(pdev, rdev);

	return 0;
}

static int wm8400_regulator_remove(struct platform_device *pdev)
{
	struct regulator_dev *rdev = platform_get_drvdata(pdev);

	platform_set_drvdata(pdev, NULL);
	regulator_unregister(rdev);

	return 0;
}

static struct platform_driver wm8400_regulator_driver = {
	.driver = {
		.name = "wm8400-regulator",
	},
	.probe = wm8400_regulator_probe,
	.remove = wm8400_regulator_remove,
};

/* 
                                                                            
  
                                                                   
                                                           
                                                     
  
                                                   
                                            
                                                        
 */
int wm8400_register_regulator(struct device *dev, int reg,
			      struct regulator_init_data *initdata)
{
	struct wm8400 *wm8400 = dev_get_drvdata(dev);

	if (wm8400->regulators[reg].name)
		return -EBUSY;

	initdata->driver_data = wm8400;

	wm8400->regulators[reg].name = "wm8400-regulator";
	wm8400->regulators[reg].id = reg;
	wm8400->regulators[reg].dev.parent = dev;
	wm8400->regulators[reg].dev.platform_data = initdata;

	return platform_device_register(&wm8400->regulators[reg]);
}
EXPORT_SYMBOL_GPL(wm8400_register_regulator);

static int __init wm8400_regulator_init(void)
{
	return platform_driver_register(&wm8400_regulator_driver);
}
subsys_initcall(wm8400_regulator_init);

static void __exit wm8400_regulator_exit(void)
{
	platform_driver_unregister(&wm8400_regulator_driver);
}
module_exit(wm8400_regulator_exit);

MODULE_AUTHOR("Mark Brown <broonie@opensource.wolfsonmicro.com>");
MODULE_DESCRIPTION("WM8400 regulator driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:wm8400-regulator");
