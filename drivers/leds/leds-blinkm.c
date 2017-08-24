/*
 *  leds-blinkm.c
 *  (c) Jan-Simon Möller (dl9pf@gmx.de)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/jiffies.h>
#include <linux/i2c.h>
#include <linux/err.h>
#include <linux/mutex.h>
#include <linux/sysfs.h>
#include <linux/printk.h>
#include <linux/pm_runtime.h>
#include <linux/leds.h>
#include <linux/delay.h>

/*                                                 */
static const unsigned short normal_i2c[] = { 0x09, I2C_CLIENT_END };

static int blinkm_transfer_hw(struct i2c_client *client, int cmd);
static int blinkm_test_run(struct i2c_client *client);

struct blinkm_led {
	struct i2c_client *i2c_client;
	struct led_classdev led_cdev;
	int id;
	atomic_t active;
};

struct blinkm_work {
	struct blinkm_led *blinkm_led;
	struct work_struct work;
};

#define cdev_to_blmled(c)          container_of(c, struct blinkm_led, led_cdev)
#define work_to_blmwork(c)         container_of(c, struct blinkm_work, work)

struct blinkm_data {
	struct i2c_client *i2c_client;
	struct mutex update_lock;
	/*                              */
	struct blinkm_led blinkm_leds[3];
	/*                                   */
	u8 red;			/*           */
	u8 green;		/*             */
	u8 blue;		/*            */
	/*                                 */
	u8 next_red;			/*           */
	u8 next_green;		/*             */
	u8 next_blue;		/*            */
	/*              */
	u8 args[7];		/*                              */
	u8 i2c_addr;		/*          */
	u8 fw_ver;		/*                  */
	/*                              */
	u8 hue;			/*          */
	u8 saturation;		/*                 */
	u8 brightness;		/*                 */
	u8 next_hue;			/*          */
	u8 next_saturation;		/*                 */
	u8 next_brightness;		/*                 */
	/*                         */
	u8 fade_speed;		/*                        */
	s8 time_adjust;		/*                        */
	u8 fade:1;		/*                      */
	u8 rand:1;		/*                       */
	u8 script_id;		/*           */
	u8 script_repeats;	/*                   */
	u8 script_startline;	/*               */
};

/*        */
#define RED   0
#define GREEN 1
#define BLUE  2

/*                                                    */
#define BLM_GO_RGB            0
#define BLM_FADE_RGB          1
#define BLM_FADE_HSB          2
#define BLM_FADE_RAND_RGB     3
#define BLM_FADE_RAND_HSB     4
#define BLM_PLAY_SCRIPT       5
#define BLM_STOP_SCRIPT       6
#define BLM_SET_FADE_SPEED    7
#define BLM_SET_TIME_ADJ      8
#define BLM_GET_CUR_RGB       9
#define BLM_WRITE_SCRIPT_LINE 10
#define BLM_READ_SCRIPT_LINE  11
#define BLM_SET_SCRIPT_LR     12	/*                  */
#define BLM_SET_ADDR          13
#define BLM_GET_ADDR          14
#define BLM_GET_FW_VER        15
#define BLM_SET_STARTUP_PARAM 16

/*                
                                      
  
                             
                            
                                           
                                               
                                                   
  
 */
static const struct {
	char cmdchar;
	u8 cmdbyte;
	u8 nr_args;
	u8 nr_ret;
	u8 dir:2;
} blinkm_cmds[17] = {
  /*                                         */
	{ 'n', 0x6e, 3, 0, 1},
	{ 'c', 0x63, 3, 0, 1},
	{ 'h', 0x68, 3, 0, 1},
	{ 'C', 0x43, 3, 0, 1},
	{ 'H', 0x48, 3, 0, 1},
	{ 'p', 0x70, 3, 0, 1},
	{ 'o', 0x6f, 0, 0, 1},
	{ 'f', 0x66, 1, 0, 1},
	{ 't', 0x74, 1, 0, 1},
	{ 'g', 0x67, 0, 3, 0},
	{ 'W', 0x57, 7, 0, 1},
	{ 'R', 0x52, 2, 5, 2},
	{ 'L', 0x4c, 3, 0, 1},
	{ 'A', 0x41, 4, 0, 1},
	{ 'a', 0x61, 0, 1, 0},
	{ 'Z', 0x5a, 0, 1, 0},
	{ 'B', 0x42, 5, 0, 1},
};

static ssize_t show_color_common(struct device *dev, char *buf, int color)
{
	struct i2c_client *client;
	struct blinkm_data *data;
	int ret;

	client = to_i2c_client(dev);
	data = i2c_get_clientdata(client);

	ret = blinkm_transfer_hw(client, BLM_GET_CUR_RGB);
	if (ret < 0)
		return ret;
	switch (color) {
	case RED:
		return scnprintf(buf, PAGE_SIZE, "%02X\n", data->red);
		break;
	case GREEN:
		return scnprintf(buf, PAGE_SIZE, "%02X\n", data->green);
		break;
	case BLUE:
		return scnprintf(buf, PAGE_SIZE, "%02X\n", data->blue);
		break;
	default:
		return -EINVAL;
	}
	return -EINVAL;
}

static int store_color_common(struct device *dev, const char *buf, int color)
{
	struct i2c_client *client;
	struct blinkm_data *data;
	int ret;
	u8 value;

	client = to_i2c_client(dev);
	data = i2c_get_clientdata(client);

	ret = kstrtou8(buf, 10, &value);
	if (ret < 0) {
		dev_err(dev, "BlinkM: value too large!\n");
		return ret;
	}

	switch (color) {
	case RED:
		data->next_red = value;
		break;
	case GREEN:
		data->next_green = value;
		break;
	case BLUE:
		data->next_blue = value;
		break;
	default:
		return -EINVAL;
	}

	dev_dbg(dev, "next_red = %d, next_green = %d, next_blue = %d\n",
			data->next_red, data->next_green, data->next_blue);

	/*             */
	ret = blinkm_transfer_hw(client, BLM_GO_RGB);
	if (ret < 0) {
		dev_err(dev, "BlinkM: can't set RGB\n");
		return ret;
	}
	return 0;
}

static ssize_t show_red(struct device *dev, struct device_attribute *attr,
			char *buf)
{
	return show_color_common(dev, buf, RED);
}

static ssize_t store_red(struct device *dev, struct device_attribute *attr,
			 const char *buf, size_t count)
{
	int ret;

	ret = store_color_common(dev, buf, RED);
	if (ret < 0)
		return ret;
	return count;
}

static DEVICE_ATTR(red, S_IRUGO | S_IWUSR, show_red, store_red);

static ssize_t show_green(struct device *dev, struct device_attribute *attr,
			  char *buf)
{
	return show_color_common(dev, buf, GREEN);
}

static ssize_t store_green(struct device *dev, struct device_attribute *attr,
			   const char *buf, size_t count)
{

	int ret;

	ret = store_color_common(dev, buf, GREEN);
	if (ret < 0)
		return ret;
	return count;
}

static DEVICE_ATTR(green, S_IRUGO | S_IWUSR, show_green, store_green);

static ssize_t show_blue(struct device *dev, struct device_attribute *attr,
			 char *buf)
{
	return show_color_common(dev, buf, BLUE);
}

static ssize_t store_blue(struct device *dev, struct device_attribute *attr,
			  const char *buf, size_t count)
{
	int ret;

	ret = store_color_common(dev, buf, BLUE);
	if (ret < 0)
		return ret;
	return count;
}

static DEVICE_ATTR(blue, S_IRUGO | S_IWUSR, show_blue, store_blue);

static ssize_t show_test(struct device *dev, struct device_attribute *attr,
			 char *buf)
{
	return scnprintf(buf, PAGE_SIZE,
			 "#Write into test to start test sequence!#\n");
}

static ssize_t store_test(struct device *dev, struct device_attribute *attr,
			  const char *buf, size_t count)
{

	struct i2c_client *client;
	int ret;
	client = to_i2c_client(dev);

	/*     */
	ret = blinkm_test_run(client);
	if (ret < 0)
		return ret;

	return count;
}

static DEVICE_ATTR(test, S_IRUGO | S_IWUSR, show_test, store_test);

/*                                      */

static struct attribute *blinkm_attrs[] = {
	&dev_attr_red.attr,
	&dev_attr_green.attr,
	&dev_attr_blue.attr,
	&dev_attr_test.attr,
	NULL,
};

static struct attribute_group blinkm_group = {
	.name = "blinkm",
	.attrs = blinkm_attrs,
};

static int blinkm_write(struct i2c_client *client, int cmd, u8 *arg)
{
	int result;
	int i;
	int arglen = blinkm_cmds[cmd].nr_args;
	/*                                                 */
	result = i2c_smbus_write_byte(client, blinkm_cmds[cmd].cmdbyte);
	if (result < 0)
		return result;
	/*                      */
	if (arglen == 0)
		return 0;

	for (i = 0; i < arglen; i++) {
		/*                   */
		result = i2c_smbus_write_byte(client, arg[i]);
		if (result < 0)
			return result;
	}
	return 0;
}

static int blinkm_read(struct i2c_client *client, int cmd, u8 *arg)
{
	int result;
	int i;
	int retlen = blinkm_cmds[cmd].nr_ret;
	for (i = 0; i < retlen; i++) {
		/*                   */
		result = i2c_smbus_read_byte(client);
		if (result < 0)
			return result;
		arg[i] = result;
	}

	return 0;
}

static int blinkm_transfer_hw(struct i2c_client *client, int cmd)
{
	/*                                         
                                                   
                                                     
                                              
                                                           
   
                                                               
                                                                 
                         
  */

	/*                                          */
	struct blinkm_data *data = i2c_get_clientdata(client);

	/*                                                
                                                  */
	if (mutex_lock_interruptible(&data->update_lock) < 0)
		return -EAGAIN;

	/*                                         */
	switch (cmd) {
	case BLM_FADE_RAND_RGB:
	case BLM_GO_RGB:
	case BLM_FADE_RGB:
		data->args[0] = data->next_red;
		data->args[1] = data->next_green;
		data->args[2] = data->next_blue;
		blinkm_write(client, cmd, data->args);
		data->red = data->args[0];
		data->green = data->args[1];
		data->blue = data->args[2];
		break;
	case BLM_FADE_HSB:
	case BLM_FADE_RAND_HSB:
		data->args[0] = data->next_hue;
		data->args[1] = data->next_saturation;
		data->args[2] = data->next_brightness;
		blinkm_write(client, cmd, data->args);
		data->hue = data->next_hue;
		data->saturation = data->next_saturation;
		data->brightness = data->next_brightness;
		break;
	case BLM_PLAY_SCRIPT:
		data->args[0] = data->script_id;
		data->args[1] = data->script_repeats;
		data->args[2] = data->script_startline;
		blinkm_write(client, cmd, data->args);
		break;
	case BLM_STOP_SCRIPT:
		blinkm_write(client, cmd, NULL);
		break;
	case BLM_GET_CUR_RGB:
		data->args[0] = data->red;
		data->args[1] = data->green;
		data->args[2] = data->blue;
		blinkm_write(client, cmd, NULL);
		blinkm_read(client, cmd, data->args);
		data->red = data->args[0];
		data->green = data->args[1];
		data->blue = data->args[2];
		break;
	case BLM_GET_ADDR:
		data->args[0] = data->i2c_addr;
		blinkm_write(client, cmd, NULL);
		blinkm_read(client, cmd, data->args);
		data->i2c_addr = data->args[0];
		break;
	case BLM_SET_TIME_ADJ:
	case BLM_SET_FADE_SPEED:
	case BLM_READ_SCRIPT_LINE:
	case BLM_WRITE_SCRIPT_LINE:
	case BLM_SET_SCRIPT_LR:
	case BLM_SET_ADDR:
	case BLM_GET_FW_VER:
	case BLM_SET_STARTUP_PARAM:
		dev_err(&client->dev,
				"BlinkM: cmd %d not implemented yet.\n", cmd);
		break;
	default:
		dev_err(&client->dev, "BlinkM: unknown command %d\n", cmd);
		mutex_unlock(&data->update_lock);
		return -EINVAL;
	}			/*                 */

	/*                        */
	mutex_unlock(&data->update_lock);
	return 0;
}

static void led_work(struct work_struct *work)
{
	int ret;
	struct blinkm_led *led;
	struct blinkm_data *data ;
	struct blinkm_work *blm_work = work_to_blmwork(work);

	led = blm_work->blinkm_led;
	data = i2c_get_clientdata(led->i2c_client);
	ret = blinkm_transfer_hw(led->i2c_client, BLM_GO_RGB);
	atomic_dec(&led->active);
	dev_dbg(&led->i2c_client->dev,
			"# DONE # next_red = %d, next_green = %d,"
			" next_blue = %d, active = %d\n",
			data->next_red, data->next_green,
			data->next_blue, atomic_read(&led->active));
	kfree(blm_work);
}

static int blinkm_led_common_set(struct led_classdev *led_cdev,
				 enum led_brightness value, int color)
{
	/*                                                             */
	struct blinkm_led *led = cdev_to_blmled(led_cdev);
	struct blinkm_data *data = i2c_get_clientdata(led->i2c_client);
	struct blinkm_work *bl_work;

	switch (color) {
	case RED:
		/*                               */
		if (data->next_red == (u8) value)
			return 0;
		/*                                                      
                                                                 
                                                                 
                                
                                        */
		if (atomic_read(&led->active) > 1)
			return 0;
		data->next_red = (u8) value;
		break;
	case GREEN:
		/*                               */
		if (data->next_green == (u8) value)
			return 0;
		/*                                                      
                                        */
		if (atomic_read(&led->active) > 1)
			return 0;
		data->next_green = (u8) value;
		break;
	case BLUE:
		/*                               */
		if (data->next_blue == (u8) value)
			return 0;
		/*                                                      
                                        */
		if (atomic_read(&led->active) > 1)
			return 0;
		data->next_blue = (u8) value;
		break;

	default:
		dev_err(&led->i2c_client->dev, "BlinkM: unknown color.\n");
		return -EINVAL;
	}

	bl_work = kzalloc(sizeof(*bl_work), GFP_ATOMIC);
	if (!bl_work)
		return -ENOMEM;

	atomic_inc(&led->active);
	dev_dbg(&led->i2c_client->dev,
			"#TO_SCHED# next_red = %d, next_green = %d,"
			" next_blue = %d, active = %d\n",
			data->next_red, data->next_green,
			data->next_blue, atomic_read(&led->active));

	/*                                     */
	bl_work->blinkm_led = led;
	INIT_WORK(&bl_work->work, led_work);
	/*                                              */
	schedule_work(&bl_work->work);

	return 0;
}

static void blinkm_led_red_set(struct led_classdev *led_cdev,
			       enum led_brightness value)
{
	blinkm_led_common_set(led_cdev, value, RED);
}

static void blinkm_led_green_set(struct led_classdev *led_cdev,
				 enum led_brightness value)
{
	blinkm_led_common_set(led_cdev, value, GREEN);
}

static void blinkm_led_blue_set(struct led_classdev *led_cdev,
				enum led_brightness value)
{
	blinkm_led_common_set(led_cdev, value, BLUE);
}

static void blinkm_init_hw(struct i2c_client *client)
{
	int ret;
	ret = blinkm_transfer_hw(client, BLM_STOP_SCRIPT);
	ret = blinkm_transfer_hw(client, BLM_GO_RGB);
}

static int blinkm_test_run(struct i2c_client *client)
{
	int ret;
	struct blinkm_data *data = i2c_get_clientdata(client);

	data->next_red = 0x01;
	data->next_green = 0x05;
	data->next_blue = 0x10;
	ret = blinkm_transfer_hw(client, BLM_GO_RGB);
	if (ret < 0)
		return ret;
	msleep(2000);

	data->next_red = 0x25;
	data->next_green = 0x10;
	data->next_blue = 0x31;
	ret = blinkm_transfer_hw(client, BLM_FADE_RGB);
	if (ret < 0)
		return ret;
	msleep(2000);

	data->next_hue = 0x50;
	data->next_saturation = 0x10;
	data->next_brightness = 0x20;
	ret = blinkm_transfer_hw(client, BLM_FADE_HSB);
	if (ret < 0)
		return ret;
	msleep(2000);

	return 0;
}

/*                                                        */
static int blinkm_detect(struct i2c_client *client, struct i2c_board_info *info)
{
	struct i2c_adapter *adapter = client->adapter;
	int ret;
	int count = 99;
	u8 tmpargs[7];

	if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_BYTE_DATA
				     | I2C_FUNC_SMBUS_WORD_DATA
				     | I2C_FUNC_SMBUS_WRITE_BYTE))
		return -ENODEV;

	/*                                                     */
	/*                                                       */

	/*                                                */
	while (count > 0) {
		ret = blinkm_write(client, BLM_GET_ADDR, NULL);
		usleep_range(5000, 10000);
		ret = blinkm_read(client, BLM_GET_ADDR, tmpargs);
		usleep_range(5000, 10000);
		if (tmpargs[0] == 0x09)
			count = 0;
		count--;
	}

	/*                                                   */
	ret = blinkm_write(client, BLM_GET_ADDR, NULL);
	if (ret < 0)
		return ret;
	usleep_range(20000, 30000);	/*                     */
	ret = blinkm_read(client, BLM_GET_ADDR, tmpargs);
	if (ret < 0)
		return ret;

	if (tmpargs[0] != 0x09) {
		dev_err(&client->dev, "enodev DEV ADDR = 0x%02X\n", tmpargs[0]);
		return -ENODEV;
	}

	strlcpy(info->type, "blinkm", I2C_NAME_SIZE);
	return 0;
}

static int blinkm_probe(struct i2c_client *client,
			const struct i2c_device_id *id)
{
	struct blinkm_data *data;
	struct blinkm_led *led[3];
	int err, i;
	char blinkm_led_name[28];

	data = devm_kzalloc(&client->dev,
			sizeof(struct blinkm_data), GFP_KERNEL);
	if (!data) {
		err = -ENOMEM;
		goto exit;
	}

	data->i2c_addr = 0x09;
	data->i2c_addr = 0x08;
	/*                                                            */
	data->fw_ver = 0xfe;
	/*                                                     
                                          */
	data->script_id = 0x01;
	data->i2c_client = client;

	i2c_set_clientdata(client, data);
	mutex_init(&data->update_lock);

	/*                      */
	err = sysfs_create_group(&client->dev.kobj, &blinkm_group);
	if (err < 0) {
		dev_err(&client->dev, "couldn't register sysfs group\n");
		goto exit;
	}

	for (i = 0; i < 3; i++) {
		/*                              */
		led[i] = &data->blinkm_leds[i];
		led[i]->i2c_client = client;
		led[i]->id = i;
		led[i]->led_cdev.max_brightness = 255;
		led[i]->led_cdev.flags = LED_CORE_SUSPENDRESUME;
		atomic_set(&led[i]->active, 0);
		switch (i) {
		case RED:
			snprintf(blinkm_led_name, sizeof(blinkm_led_name),
					 "blinkm-%d-%d-red",
					 client->adapter->nr,
					 client->addr);
			led[i]->led_cdev.name = blinkm_led_name;
			led[i]->led_cdev.brightness_set = blinkm_led_red_set;
			err = led_classdev_register(&client->dev,
						    &led[i]->led_cdev);
			if (err < 0) {
				dev_err(&client->dev,
					"couldn't register LED %s\n",
					led[i]->led_cdev.name);
				goto failred;
			}
			break;
		case GREEN:
			snprintf(blinkm_led_name, sizeof(blinkm_led_name),
					 "blinkm-%d-%d-green",
					 client->adapter->nr,
					 client->addr);
			led[i]->led_cdev.name = blinkm_led_name;
			led[i]->led_cdev.brightness_set = blinkm_led_green_set;
			err = led_classdev_register(&client->dev,
						    &led[i]->led_cdev);
			if (err < 0) {
				dev_err(&client->dev,
					"couldn't register LED %s\n",
					led[i]->led_cdev.name);
				goto failgreen;
			}
			break;
		case BLUE:
			snprintf(blinkm_led_name, sizeof(blinkm_led_name),
					 "blinkm-%d-%d-blue",
					 client->adapter->nr,
					 client->addr);
			led[i]->led_cdev.name = blinkm_led_name;
			led[i]->led_cdev.brightness_set = blinkm_led_blue_set;
			err = led_classdev_register(&client->dev,
						    &led[i]->led_cdev);
			if (err < 0) {
				dev_err(&client->dev,
					"couldn't register LED %s\n",
					led[i]->led_cdev.name);
				goto failblue;
			}
			break;
		}		/*            */
	}			/*         */

	/*                       */
	blinkm_init_hw(client);

	return 0;

failblue:
	led_classdev_unregister(&led[GREEN]->led_cdev);

failgreen:
	led_classdev_unregister(&led[RED]->led_cdev);

failred:
	sysfs_remove_group(&client->dev.kobj, &blinkm_group);
exit:
	return err;
}

static int blinkm_remove(struct i2c_client *client)
{
	struct blinkm_data *data = i2c_get_clientdata(client);
	int ret = 0;
	int i;

	/*                                            */
	for (i = 0; i < 3; i++) {
		flush_scheduled_work();
		led_classdev_unregister(&data->blinkm_leds[i].led_cdev);
	}

	/*           */
	data->next_red = 0x00;
	data->next_green = 0x00;
	data->next_blue = 0x00;
	ret = blinkm_transfer_hw(client, BLM_FADE_RGB);
	if (ret < 0)
		dev_err(&client->dev, "Failure in blinkm_remove ignored. Continuing.\n");

	/*           */
	data->next_hue = 0x00;
	data->next_saturation = 0x00;
	data->next_brightness = 0x00;
	ret = blinkm_transfer_hw(client, BLM_FADE_HSB);
	if (ret < 0)
		dev_err(&client->dev, "Failure in blinkm_remove ignored. Continuing.\n");

	/*                 */
	data->next_red = 0xff;
	ret = blinkm_transfer_hw(client, BLM_GO_RGB);
	if (ret < 0)
		dev_err(&client->dev, "Failure in blinkm_remove ignored. Continuing.\n");

	/*     */
	data->next_red = 0x00;
	ret = blinkm_transfer_hw(client, BLM_FADE_RGB);
	if (ret < 0)
		dev_err(&client->dev, "Failure in blinkm_remove ignored. Continuing.\n");

	sysfs_remove_group(&client->dev.kobj, &blinkm_group);
	return 0;
}

static const struct i2c_device_id blinkm_id[] = {
	{"blinkm", 0},
	{}
};

MODULE_DEVICE_TABLE(i2c, blinkm_id);

  /*                                          */
static struct i2c_driver blinkm_driver = {
	.class = I2C_CLASS_HWMON,
	.driver = {
		   .name = "blinkm",
		   },
	.probe = blinkm_probe,
	.remove = blinkm_remove,
	.id_table = blinkm_id,
	.detect = blinkm_detect,
	.address_list = normal_i2c,
};

module_i2c_driver(blinkm_driver);

MODULE_AUTHOR("Jan-Simon Moeller <dl9pf@gmx.de>");
MODULE_DESCRIPTION("BlinkM RGB LED driver");
MODULE_LICENSE("GPL");

