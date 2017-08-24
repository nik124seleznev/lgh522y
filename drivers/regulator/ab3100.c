/*
 * drivers/regulator/ab3100.c
 *
 * Copyright (C) 2008-2009 ST-Ericsson AB
 * License terms: GNU General Public License (GPL) version 2
 * Low-level control of the AB3100 IC Low Dropout (LDO)
 * regulators, external regulator and buck converter
 * Author: Mattias Wallin <mattias.wallin@stericsson.com>
 * Author: Linus Walleij <linus.walleij@stericsson.com>
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/regulator/driver.h>
#include <linux/mfd/ab3100.h>
#include <linux/mfd/abx500.h>
#include <linux/of.h>
#include <linux/regulator/of_regulator.h>

/*                                                             */
#define AB3100_LDO_A		0x40
#define AB3100_LDO_C		0x41
#define AB3100_LDO_D		0x42
#define AB3100_LDO_E		0x43
#define AB3100_LDO_E_SLEEP	0x44
#define AB3100_LDO_F		0x45
#define AB3100_LDO_G		0x46
#define AB3100_LDO_H		0x47
#define AB3100_LDO_H_SLEEP_MODE	0
#define AB3100_LDO_H_SLEEP_EN	2
#define AB3100_LDO_ON		4
#define AB3100_LDO_H_VSEL_AC	5
#define AB3100_LDO_K		0x48
#define AB3100_LDO_EXT		0x49
#define AB3100_BUCK		0x4A
#define AB3100_BUCK_SLEEP	0x4B
#define AB3100_REG_ON_MASK	0x10

/* 
                          
                                                            
                                                           
                             
                                                         
                                                   
 */
struct ab3100_regulator {
	struct regulator_dev *rdev;
	struct device *dev;
	struct ab3100_platform_data *plfdata;
	u8 regreg;
};

/*                                              */
static const u8 ab3100_reg_init_order[AB3100_NUM_REGULATORS+2] = {
	AB3100_LDO_A,
	AB3100_LDO_C,
	AB3100_LDO_E,
	AB3100_LDO_E_SLEEP,
	AB3100_LDO_F,
	AB3100_LDO_G,
	AB3100_LDO_H,
	AB3100_LDO_K,
	AB3100_LDO_EXT,
	AB3100_BUCK,
	AB3100_BUCK_SLEEP,
	AB3100_LDO_D,
};

/*                                                         */
#define LDO_A_VOLTAGE 2750000
#define LDO_C_VOLTAGE 2650000
#define LDO_D_VOLTAGE 2650000

static const unsigned int ldo_e_buck_typ_voltages[] = {
	1800000,
	1400000,
	1300000,
	1200000,
	1100000,
	1050000,
	900000,
};

static const unsigned int ldo_f_typ_voltages[] = {
	1800000,
	1400000,
	1300000,
	1200000,
	1100000,
	1050000,
	2500000,
	2650000,
};

static const unsigned int ldo_g_typ_voltages[] = {
	2850000,
	2750000,
	1800000,
	1500000,
};

static const unsigned int ldo_h_typ_voltages[] = {
	2750000,
	1800000,
	1500000,
	1200000,
};

static const unsigned int ldo_k_typ_voltages[] = {
	2750000,
	1800000,
};


/*                       */
static struct ab3100_regulator
ab3100_regulators[AB3100_NUM_REGULATORS] = {
	{
		.regreg = AB3100_LDO_A,
	},
	{
		.regreg = AB3100_LDO_C,
	},
	{
		.regreg = AB3100_LDO_D,
	},
	{
		.regreg = AB3100_LDO_E,
	},
	{
		.regreg = AB3100_LDO_F,
	},
	{
		.regreg = AB3100_LDO_G,
	},
	{
		.regreg = AB3100_LDO_H,
	},
	{
		.regreg = AB3100_LDO_K,
	},
	{
		.regreg = AB3100_LDO_EXT,
		/*                                        */
	},
	{
		.regreg = AB3100_BUCK,
	},
};

/*
                                                                
                                  
 */
static int ab3100_enable_regulator(struct regulator_dev *reg)
{
	struct ab3100_regulator *abreg = rdev_get_drvdata(reg);
	int err;
	u8 regval;

	err = abx500_get_register_interruptible(abreg->dev, 0, abreg->regreg,
						&regval);
	if (err) {
		dev_warn(&reg->dev, "failed to get regid %d value\n",
			 abreg->regreg);
		return err;
	}

	/*                                                      */
	if (regval & AB3100_REG_ON_MASK)
		return 0;

	regval |= AB3100_REG_ON_MASK;

	err = abx500_set_register_interruptible(abreg->dev, 0, abreg->regreg,
						regval);
	if (err) {
		dev_warn(&reg->dev, "failed to set regid %d value\n",
			 abreg->regreg);
		return err;
	}

	return 0;
}

static int ab3100_disable_regulator(struct regulator_dev *reg)
{
	struct ab3100_regulator *abreg = rdev_get_drvdata(reg);
	int err;
	u8 regval;

	/*
                                                                 
                                                      
  */
	pr_info("Called ab3100_disable_regulator\n");
	if (abreg->regreg == AB3100_LDO_D) {
		dev_info(&reg->dev, "disabling LDO D - shut down system\n");
		/*                                                 */
		return abx500_set_register_interruptible(abreg->dev, 0,
							 AB3100_LDO_D, 0x00U);
	}

	/*
                                         
  */
	err = abx500_get_register_interruptible(abreg->dev, 0, abreg->regreg,
						&regval);
	if (err) {
		dev_err(&reg->dev, "unable to get register 0x%x\n",
			abreg->regreg);
		return err;
	}
	regval &= ~AB3100_REG_ON_MASK;
	return abx500_set_register_interruptible(abreg->dev, 0, abreg->regreg,
						 regval);
}

static int ab3100_is_enabled_regulator(struct regulator_dev *reg)
{
	struct ab3100_regulator *abreg = rdev_get_drvdata(reg);
	u8 regval;
	int err;

	err = abx500_get_register_interruptible(abreg->dev, 0, abreg->regreg,
						&regval);
	if (err) {
		dev_err(&reg->dev, "unable to get register 0x%x\n",
			abreg->regreg);
		return err;
	}

	return regval & AB3100_REG_ON_MASK;
}

static int ab3100_get_voltage_regulator(struct regulator_dev *reg)
{
	struct ab3100_regulator *abreg = rdev_get_drvdata(reg);
	u8 regval;
	int err;

	/*
                                                       
                          
  */
	err = abx500_get_register_interruptible(abreg->dev, 0,
						abreg->regreg, &regval);
	if (err) {
		dev_warn(&reg->dev,
			 "failed to get regulator value in register %02x\n",
			 abreg->regreg);
		return err;
	}

	/*                                   */
	regval &= 0xE0;
	regval >>= 5;

	if (regval >= reg->desc->n_voltages) {
		dev_err(&reg->dev,
			"regulator register %02x contains an illegal voltage setting\n",
			abreg->regreg);
		return -EINVAL;
	}

	return reg->desc->volt_table[regval];
}

static int ab3100_set_voltage_regulator_sel(struct regulator_dev *reg,
					    unsigned selector)
{
	struct ab3100_regulator *abreg = rdev_get_drvdata(reg);
	u8 regval;
	int err;

	err = abx500_get_register_interruptible(abreg->dev, 0,
						abreg->regreg, &regval);
	if (err) {
		dev_warn(&reg->dev,
			 "failed to get regulator register %02x\n",
			 abreg->regreg);
		return err;
	}

	/*                                                        */
	regval &= ~0xE0;
	regval |= (selector << 5);

	err = abx500_set_register_interruptible(abreg->dev, 0,
						abreg->regreg, regval);
	if (err)
		dev_warn(&reg->dev, "failed to set regulator register %02x\n",
			abreg->regreg);

	return err;
}

static int ab3100_set_suspend_voltage_regulator(struct regulator_dev *reg,
						int uV)
{
	struct ab3100_regulator *abreg = rdev_get_drvdata(reg);
	u8 regval;
	int err;
	int bestindex;
	u8 targetreg;

	if (abreg->regreg == AB3100_LDO_E)
		targetreg = AB3100_LDO_E_SLEEP;
	else if (abreg->regreg == AB3100_BUCK)
		targetreg = AB3100_BUCK_SLEEP;
	else
		return -EINVAL;

	/*                                                          */
	bestindex = regulator_map_voltage_iterate(reg, uV, uV);

	err = abx500_get_register_interruptible(abreg->dev, 0,
						targetreg, &regval);
	if (err) {
		dev_warn(&reg->dev,
			 "failed to get regulator register %02x\n",
			 targetreg);
		return err;
	}

	/*                                                        */
	regval &= ~0xE0;
	regval |= (bestindex << 5);

	err = abx500_set_register_interruptible(abreg->dev, 0,
						targetreg, regval);
	if (err)
		dev_warn(&reg->dev, "failed to set regulator register %02x\n",
			abreg->regreg);

	return err;
}

/*
                                                          
 */
static int ab3100_get_voltage_regulator_external(struct regulator_dev *reg)
{
	struct ab3100_regulator *abreg = rdev_get_drvdata(reg);

	if (abreg->plfdata)
		return abreg->plfdata->external_voltage;
	else
		/*                                                */
		return 0;
}

static struct regulator_ops regulator_ops_fixed = {
	.list_voltage = regulator_list_voltage_linear,
	.enable      = ab3100_enable_regulator,
	.disable     = ab3100_disable_regulator,
	.is_enabled  = ab3100_is_enabled_regulator,
};

static struct regulator_ops regulator_ops_variable = {
	.enable      = ab3100_enable_regulator,
	.disable     = ab3100_disable_regulator,
	.is_enabled  = ab3100_is_enabled_regulator,
	.get_voltage = ab3100_get_voltage_regulator,
	.set_voltage_sel = ab3100_set_voltage_regulator_sel,
	.list_voltage = regulator_list_voltage_table,
};

static struct regulator_ops regulator_ops_variable_sleepable = {
	.enable      = ab3100_enable_regulator,
	.disable     = ab3100_disable_regulator,
	.is_enabled  = ab3100_is_enabled_regulator,
	.get_voltage = ab3100_get_voltage_regulator,
	.set_voltage_sel = ab3100_set_voltage_regulator_sel,
	.set_suspend_voltage = ab3100_set_suspend_voltage_regulator,
	.list_voltage = regulator_list_voltage_table,
};

/*
                                                   
                                                       
                                                    
                                                 
 */
static struct regulator_ops regulator_ops_external = {
	.enable      = ab3100_enable_regulator,
	.disable     = ab3100_disable_regulator,
	.is_enabled  = ab3100_is_enabled_regulator,
	.get_voltage = ab3100_get_voltage_regulator_external,
};

static struct regulator_desc
ab3100_regulator_desc[AB3100_NUM_REGULATORS] = {
	{
		.name = "LDO_A",
		.id   = AB3100_LDO_A,
		.ops  = &regulator_ops_fixed,
		.n_voltages = 1,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
		.min_uV = LDO_A_VOLTAGE,
		.enable_time = 200,
	},
	{
		.name = "LDO_C",
		.id   = AB3100_LDO_C,
		.ops  = &regulator_ops_fixed,
		.n_voltages = 1,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
		.min_uV = LDO_C_VOLTAGE,
		.enable_time = 200,
	},
	{
		.name = "LDO_D",
		.id   = AB3100_LDO_D,
		.ops  = &regulator_ops_fixed,
		.n_voltages = 1,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
		.min_uV = LDO_D_VOLTAGE,
		.enable_time = 200,
	},
	{
		.name = "LDO_E",
		.id   = AB3100_LDO_E,
		.ops  = &regulator_ops_variable_sleepable,
		.n_voltages = ARRAY_SIZE(ldo_e_buck_typ_voltages),
		.volt_table = ldo_e_buck_typ_voltages,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
		.enable_time = 200,
	},
	{
		.name = "LDO_F",
		.id   = AB3100_LDO_F,
		.ops  = &regulator_ops_variable,
		.n_voltages = ARRAY_SIZE(ldo_f_typ_voltages),
		.volt_table = ldo_f_typ_voltages,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
		.enable_time = 600,
	},
	{
		.name = "LDO_G",
		.id   = AB3100_LDO_G,
		.ops  = &regulator_ops_variable,
		.n_voltages = ARRAY_SIZE(ldo_g_typ_voltages),
		.volt_table = ldo_g_typ_voltages,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
		.enable_time = 400,
	},
	{
		.name = "LDO_H",
		.id   = AB3100_LDO_H,
		.ops  = &regulator_ops_variable,
		.n_voltages = ARRAY_SIZE(ldo_h_typ_voltages),
		.volt_table = ldo_h_typ_voltages,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
		.enable_time = 200,
	},
	{
		.name = "LDO_K",
		.id   = AB3100_LDO_K,
		.ops  = &regulator_ops_variable,
		.n_voltages = ARRAY_SIZE(ldo_k_typ_voltages),
		.volt_table = ldo_k_typ_voltages,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
		.enable_time = 200,
	},
	{
		.name = "LDO_EXT",
		.id   = AB3100_LDO_EXT,
		.ops  = &regulator_ops_external,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
	},
	{
		.name = "BUCK",
		.id   = AB3100_BUCK,
		.ops  = &regulator_ops_variable_sleepable,
		.n_voltages = ARRAY_SIZE(ldo_e_buck_typ_voltages),
		.volt_table = ldo_e_buck_typ_voltages,
		.type = REGULATOR_VOLTAGE,
		.owner = THIS_MODULE,
		.enable_time = 1000,
	},
};

static int ab3100_regulator_register(struct platform_device *pdev,
				     struct ab3100_platform_data *plfdata,
				     struct regulator_init_data *init_data,
				     struct device_node *np,
				     int id)
{
	struct regulator_desc *desc;
	struct ab3100_regulator *reg;
	struct regulator_dev *rdev;
	struct regulator_config config = { };
	int err, i;

	for (i = 0; i < AB3100_NUM_REGULATORS; i++) {
		desc = &ab3100_regulator_desc[i];
		if (desc->id == id)
			break;
	}
	if (desc->id != id)
		return -ENODEV;

	/*                                */
	reg = &ab3100_regulators[i];

	/*
                                    
                                                   
                                                      
                                                    
                               
  */
	reg->dev = &pdev->dev;
	if (plfdata) {
		reg->plfdata = plfdata;
		config.init_data = &plfdata->reg_constraints[i];
	} else if (np) {
		config.of_node = np;
		config.init_data = init_data;
	}
	config.dev = &pdev->dev;
	config.driver_data = reg;

	rdev = regulator_register(desc, &config);
	if (IS_ERR(rdev)) {
		err = PTR_ERR(rdev);
		dev_err(&pdev->dev,
			"%s: failed to register regulator %s err %d\n",
			__func__, desc->name,
			err);
		return err;
	}

	/*                                                     */
	reg->rdev = rdev;
	return 0;
}

static struct of_regulator_match ab3100_regulator_matches[] = {
	{ .name = "ab3100_ldo_a", .driver_data = (void *) AB3100_LDO_A, },
	{ .name = "ab3100_ldo_c", .driver_data = (void *) AB3100_LDO_C, },
	{ .name = "ab3100_ldo_d", .driver_data = (void *) AB3100_LDO_D, },
	{ .name = "ab3100_ldo_e", .driver_data = (void *) AB3100_LDO_E, },
	{ .name = "ab3100_ldo_f", .driver_data = (void *) AB3100_LDO_F },
	{ .name = "ab3100_ldo_g", .driver_data = (void *) AB3100_LDO_G },
	{ .name = "ab3100_ldo_h", .driver_data = (void *) AB3100_LDO_H },
	{ .name = "ab3100_ldo_k", .driver_data = (void *) AB3100_LDO_K },
	{ .name = "ab3100_ext", .driver_data = (void *) AB3100_LDO_EXT },
	{ .name = "ab3100_buck", .driver_data = (void *) AB3100_BUCK },
};

/*
                                        
                                                   
                                                                   
                                                                 
 */
/*                                               */
#define LDO_A_SETTING		0x16
/*                                                       */
#define LDO_C_SETTING		0x10
/*                                            */
#define LDO_D_SETTING		0x10
/*                                                      */
#define LDO_E_SETTING		0x10
/*                                                          */
#define LDO_E_SLEEP_SETTING	0x00
/*                                                      */
#define LDO_F_SETTING		0xD0
/*                                                        */
#define LDO_G_SETTING		0x00
/*                                                  */
#define LDO_H_SETTING		0x18
/*                                                        */
#define LDO_K_SETTING		0x00
/*                                                        */
#define LDO_EXT_SETTING		0x00
/*                                                     */
#define BUCK_SETTING	0x7D
/*                                                           */
#define BUCK_SLEEP_SETTING	0xAC

static const u8 ab3100_reg_initvals[] = {
	LDO_A_SETTING,
	LDO_C_SETTING,
	LDO_E_SETTING,
	LDO_E_SLEEP_SETTING,
	LDO_F_SETTING,
	LDO_G_SETTING,
	LDO_H_SETTING,
	LDO_K_SETTING,
	LDO_EXT_SETTING,
	BUCK_SETTING,
	BUCK_SLEEP_SETTING,
	LDO_D_SETTING,
};

static int ab3100_regulators_remove(struct platform_device *pdev)
{
	int i;

	for (i = 0; i < AB3100_NUM_REGULATORS; i++) {
		struct ab3100_regulator *reg = &ab3100_regulators[i];

		regulator_unregister(reg->rdev);
		reg->rdev = NULL;
	}
	return 0;
}

static int
ab3100_regulator_of_probe(struct platform_device *pdev, struct device_node *np)
{
	int err, i;

	/*
                                                               
                  
  */
	/*                   */
	for (i = 0; i < ARRAY_SIZE(ab3100_reg_init_order); i++) {
		err = abx500_set_register_interruptible(&pdev->dev, 0,
					ab3100_reg_init_order[i],
					ab3100_reg_initvals[i]);
		if (err) {
			dev_err(&pdev->dev, "regulator initialization failed with error %d\n",
				err);
			return err;
		}
	}

	for (i = 0; i < ARRAY_SIZE(ab3100_regulator_matches); i++) {
		err = ab3100_regulator_register(
			pdev, NULL, ab3100_regulator_matches[i].init_data,
			ab3100_regulator_matches[i].of_node,
			(int) ab3100_regulator_matches[i].driver_data);
		if (err) {
			ab3100_regulators_remove(pdev);
			return err;
		}
	}

	return 0;
}


static int ab3100_regulators_probe(struct platform_device *pdev)
{
	struct ab3100_platform_data *plfdata = pdev->dev.platform_data;
	struct device_node *np = pdev->dev.of_node;
	int err = 0;
	u8 data;
	int i;

	/*                  */
	err = abx500_get_register_interruptible(&pdev->dev, 0,
						AB3100_LDO_D, &data);
	if (err) {
		dev_err(&pdev->dev, "could not read initial status of LDO_D\n");
		return err;
	}
	if (data & 0x10)
		dev_notice(&pdev->dev,
			   "chip is already in active mode (Warm start)\n");
	else
		dev_notice(&pdev->dev,
			   "chip is in inactive mode (Cold start)\n");

	if (np) {
		err = of_regulator_match(&pdev->dev, np,
					 ab3100_regulator_matches,
					 ARRAY_SIZE(ab3100_regulator_matches));
		if (err < 0) {
			dev_err(&pdev->dev,
				"Error parsing regulator init data: %d\n", err);
			return err;
		}
		return ab3100_regulator_of_probe(pdev, np);
	}

	/*                   */
	for (i = 0; i < ARRAY_SIZE(ab3100_reg_init_order); i++) {
		err = abx500_set_register_interruptible(&pdev->dev, 0,
					ab3100_reg_init_order[i],
					plfdata->reg_initvals[i]);
		if (err) {
			dev_err(&pdev->dev, "regulator initialization failed with error %d\n",
				err);
			return err;
		}
	}

	/*                         */
	for (i = 0; i < AB3100_NUM_REGULATORS; i++) {
		struct regulator_desc *desc = &ab3100_regulator_desc[i];

		err = ab3100_regulator_register(pdev, plfdata, NULL, NULL,
						desc->id);
		if (err) {
			ab3100_regulators_remove(pdev);
			return err;
		}
	}

	return 0;
}

static struct platform_driver ab3100_regulators_driver = {
	.driver = {
		.name  = "ab3100-regulators",
		.owner = THIS_MODULE,
	},
	.probe = ab3100_regulators_probe,
	.remove = ab3100_regulators_remove,
};

static __init int ab3100_regulators_init(void)
{
	return platform_driver_register(&ab3100_regulators_driver);
}

static __exit void ab3100_regulators_exit(void)
{
	platform_driver_unregister(&ab3100_regulators_driver);
}

subsys_initcall(ab3100_regulators_init);
module_exit(ab3100_regulators_exit);

MODULE_AUTHOR("Mattias Wallin <mattias.wallin@stericsson.com>");
MODULE_DESCRIPTION("AB3100 Regulator driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:ab3100-regulators");
