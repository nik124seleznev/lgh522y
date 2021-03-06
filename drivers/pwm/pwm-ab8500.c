/*
 * Copyright (C) ST-Ericsson SA 2010
 *
 * Author: Arun R Murthy <arun.murthy@stericsson.com>
 * License terms: GNU General Public License (GPL) version 2
 */
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/pwm.h>
#include <linux/mfd/abx500.h>
#include <linux/mfd/abx500/ab8500.h>
#include <linux/module.h>

/*
                     
             
 */
#define AB8500_PWM_OUT_CTRL1_REG	0x60
#define AB8500_PWM_OUT_CTRL2_REG	0x61
#define AB8500_PWM_OUT_CTRL7_REG	0x66

/*                            */
#define ENABLE_PWM			1
#define DISABLE_PWM			0

struct ab8500_pwm_chip {
	struct pwm_chip chip;
};

static int ab8500_pwm_config(struct pwm_chip *chip, struct pwm_device *pwm,
			     int duty_ns, int period_ns)
{
	int ret = 0;
	unsigned int higher_val, lower_val;
	u8 reg;

	/*
                                               
                                 
  */
	lower_val = duty_ns & 0x00FF;
	/*
                                             
                                 
  */
	higher_val = ((duty_ns & 0x0300) >> 8);

	reg = AB8500_PWM_OUT_CTRL1_REG + ((chip->base - 1) * 2);

	ret = abx500_set_register_interruptible(chip->dev, AB8500_MISC,
			reg, (u8)lower_val);
	if (ret < 0)
		return ret;
	ret = abx500_set_register_interruptible(chip->dev, AB8500_MISC,
			(reg + 1), (u8)higher_val);

	return ret;
}

static int ab8500_pwm_enable(struct pwm_chip *chip, struct pwm_device *pwm)
{
	int ret;

	ret = abx500_mask_and_set_register_interruptible(chip->dev,
				AB8500_MISC, AB8500_PWM_OUT_CTRL7_REG,
				1 << (chip->base - 1), ENABLE_PWM);
	if (ret < 0)
		dev_err(chip->dev, "%s: Failed to enable PWM, Error %d\n",
							pwm->label, ret);
	return ret;
}

static void ab8500_pwm_disable(struct pwm_chip *chip, struct pwm_device *pwm)
{
	int ret;

	ret = abx500_mask_and_set_register_interruptible(chip->dev,
				AB8500_MISC, AB8500_PWM_OUT_CTRL7_REG,
				1 << (chip->base - 1), DISABLE_PWM);
	if (ret < 0)
		dev_err(chip->dev, "%s: Failed to disable PWM, Error %d\n",
							pwm->label, ret);
	return;
}

static const struct pwm_ops ab8500_pwm_ops = {
	.config = ab8500_pwm_config,
	.enable = ab8500_pwm_enable,
	.disable = ab8500_pwm_disable,
	.owner = THIS_MODULE,
};

static int ab8500_pwm_probe(struct platform_device *pdev)
{
	struct ab8500_pwm_chip *ab8500;
	int err;

	/*
                                                            
                                                      
  */
	ab8500 = devm_kzalloc(&pdev->dev, sizeof(*ab8500), GFP_KERNEL);
	if (ab8500 == NULL) {
		dev_err(&pdev->dev, "failed to allocate memory\n");
		return -ENOMEM;
	}

	ab8500->chip.dev = &pdev->dev;
	ab8500->chip.ops = &ab8500_pwm_ops;
	ab8500->chip.base = pdev->id;
	ab8500->chip.npwm = 1;

	err = pwmchip_add(&ab8500->chip);
	if (err < 0)
		return err;

	dev_dbg(&pdev->dev, "pwm probe successful\n");
	platform_set_drvdata(pdev, ab8500);

	return 0;
}

static int ab8500_pwm_remove(struct platform_device *pdev)
{
	struct ab8500_pwm_chip *ab8500 = platform_get_drvdata(pdev);
	int err;

	err = pwmchip_remove(&ab8500->chip);
	if (err < 0)
		return err;

	dev_dbg(&pdev->dev, "pwm driver removed\n");

	return 0;
}

static struct platform_driver ab8500_pwm_driver = {
	.driver = {
		.name = "ab8500-pwm",
		.owner = THIS_MODULE,
	},
	.probe = ab8500_pwm_probe,
	.remove = ab8500_pwm_remove,
};
module_platform_driver(ab8500_pwm_driver);

MODULE_AUTHOR("Arun MURTHY <arun.murthy@stericsson.com>");
MODULE_DESCRIPTION("AB8500 Pulse Width Modulation Driver");
MODULE_ALIAS("platform:ab8500-pwm");
MODULE_LICENSE("GPL v2");
