/*
 * Copyright (C) 2012 Texas Instruments Inc
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 * Contributors:
 *      Manjunath Hadli <manjunath.hadli@ti.com>
 *      Prabhakar Lad <prabhakar.lad@ti.com>
 */

#ifndef _VPFE_H
#define _VPFE_H

#ifdef __KERNEL__
#include <linux/v4l2-subdev.h>
#include <linux/clk.h>
#include <linux/i2c.h>

#include <media/davinci/vpfe_types.h>

#define CAPTURE_DRV_NAME	"vpfe-capture"

struct vpfe_route {
	__u32 input;
	__u32 output;
};

enum vpfe_subdev_id {
	VPFE_SUBDEV_TVP5146 = 1,
	VPFE_SUBDEV_MT9T031 = 2,
	VPFE_SUBDEV_TVP7002 = 3,
	VPFE_SUBDEV_MT9P031 = 4,
};

struct vpfe_ext_subdev_info {
	/*             */
	struct v4l2_subdev *subdev;
	/*                        */
	char module_name[32];
	/*                     */
	int grp_id;
	/*                            */
	int num_inputs;
	/*                                    */
	struct v4l2_input *inputs;
	/*                                            */
	struct vpfe_route *routes;
	/*                                  */
	struct vpfe_hw_if_param ccdc_if_params;
	/*                          */
	struct i2c_board_info board_info;
	/*                               */
	unsigned is_camera:1;
	/*                                   */
	unsigned can_route:1;
	/*              */
	unsigned registered:1;
};

struct vpfe_config {
	/*                                         */
	int num_subdevs;
	/*                               */
	struct vpfe_ext_subdev_info *sub_devs;
	/*               */
	char *card_name;
	/*                                   */
	int (*setup_input)(enum vpfe_subdev_id id);
	/*                  */
	int num_clocks;
	/*                              */
	char *clocks[];
};
#endif
#endif
