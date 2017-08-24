/*
 * Helper module for board specific I2C bus registration
 *
 * Copyright (C) 2009 Nokia Corporation.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include "soc.h"
#include "omap_hwmod.h"
#include "omap_device.h"
#include "omap-pm.h"

#include "prm.h"
#include "common.h"
#include "mux.h"
#include "i2c.h"

/*                                                   */
#define I2C_EN					BIT(15)
#define OMAP2_I2C_CON_OFFSET			0x24
#define OMAP4_I2C_CON_OFFSET			0xA4

#define MAX_OMAP_I2C_HWMOD_NAME_LEN	16

static void __init omap2_i2c_mux_pins(int bus_id)
{
	char mux_name[sizeof("i2c2_scl.i2c2_scl")];

	/*                              */
	if (bus_id == 1)
		return;

	sprintf(mux_name, "i2c%i_scl.i2c%i_scl", bus_id, bus_id);
	omap_mux_init_signal(mux_name, OMAP_PIN_INPUT);
	sprintf(mux_name, "i2c%i_sda.i2c%i_sda", bus_id, bus_id);
	omap_mux_init_signal(mux_name, OMAP_PIN_INPUT);
}

/* 
                                              
                           
  
                                                                      
               
                     
                            
                    
                               
                                                                          
                  
 */
int omap_i2c_reset(struct omap_hwmod *oh)
{
	u32 v;
	u16 i2c_con;
	int c = 0;

	if (oh->class->rev == OMAP_I2C_IP_VERSION_2) {
		i2c_con = OMAP4_I2C_CON_OFFSET;
	} else if (oh->class->rev == OMAP_I2C_IP_VERSION_1) {
		i2c_con = OMAP2_I2C_CON_OFFSET;
	} else {
		WARN(1, "Cannot reset I2C block %s: unsupported revision\n",
		     oh->name);
		return -EINVAL;
	}

	/*             */
	v = omap_hwmod_read(oh, i2c_con);
	v &= ~I2C_EN;
	omap_hwmod_write(v, oh, i2c_con);

	/*                            */
	omap_hwmod_softreset(oh);

	/*            */
	v = omap_hwmod_read(oh, i2c_con);
	v |= I2C_EN;
	omap_hwmod_write(v, oh, i2c_con);

	/*                       */
	omap_test_timeout((omap_hwmod_read(oh,
				oh->class->sysc->syss_offs)
				& SYSS_RESETDONE_MASK),
				MAX_MODULE_SOFTRESET_WAIT, c);

	if (c == MAX_MODULE_SOFTRESET_WAIT)
		pr_warning("%s: %s: softreset failed (waited %d usec)\n",
			__func__, oh->name, MAX_MODULE_SOFTRESET_WAIT);
	else
		pr_debug("%s: %s: softreset in %d usec\n", __func__,
			oh->name, c);

	return 0;
}

static int __init omap_i2c_nr_ports(void)
{
	int ports = 0;

	if (cpu_is_omap24xx())
		ports = 2;
	else if (cpu_is_omap34xx())
		ports = 3;
	else if (cpu_is_omap44xx())
		ports = 4;
	return ports;
}

/*
                                                                
                                                       
                                                             
 */
static void omap_pm_set_max_mpu_wakeup_lat_compat(struct device *dev, long t)
{
	omap_pm_set_max_mpu_wakeup_lat(dev, t);
}

static const char name[] = "omap_i2c";

int __init omap_i2c_add_bus(struct omap_i2c_bus_platform_data *i2c_pdata,
				int bus_id)
{
	int l;
	struct omap_hwmod *oh;
	struct platform_device *pdev;
	char oh_name[MAX_OMAP_I2C_HWMOD_NAME_LEN];
	struct omap_i2c_bus_platform_data *pdata;
	struct omap_i2c_dev_attr *dev_attr;

	if (bus_id > omap_i2c_nr_ports())
		return -EINVAL;

	omap2_i2c_mux_pins(bus_id);

	l = snprintf(oh_name, MAX_OMAP_I2C_HWMOD_NAME_LEN, "i2c%d", bus_id);
	WARN(l >= MAX_OMAP_I2C_HWMOD_NAME_LEN,
		"String buffer overflow in I2C%d device setup\n", bus_id);
	oh = omap_hwmod_lookup(oh_name);
	if (!oh) {
			pr_err("Could not look up %s\n", oh_name);
			return -EEXIST;
	}

	pdata = i2c_pdata;
	/*
                                                                       
                                                                   
                            
  */
	pdata->rev = oh->class->rev;

	dev_attr = (struct omap_i2c_dev_attr *)oh->dev_attr;
	pdata->flags = dev_attr->flags;

	/*
                                                             
                                                            
                                                       
              
                                          
  */
	if (cpu_is_omap34xx())
		pdata->set_mpu_wkup_lat = omap_pm_set_max_mpu_wakeup_lat_compat;
	pdev = omap_device_build(name, bus_id, oh, pdata,
				 sizeof(struct omap_i2c_bus_platform_data));
	WARN(IS_ERR(pdev), "Could not build omap_device for %s\n", name);

	return PTR_RET(pdev);
}

static  int __init omap_i2c_cmdline(void)
{
	return omap_register_i2c_bus_cmdline();
}
omap_subsys_initcall(omap_i2c_cmdline);
