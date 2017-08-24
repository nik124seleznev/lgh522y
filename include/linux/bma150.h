/*
 * Copyright (c) 2011 Bosch Sensortec GmbH
 * Copyright (c) 2011 Unixphere
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

#ifndef _BMA150_H_
#define _BMA150_H_

#define BMA150_DRIVER		"bma150"

#define BMA150_RANGE_2G		0
#define BMA150_RANGE_4G		1
#define BMA150_RANGE_8G		2

#define BMA150_BW_25HZ		0
#define BMA150_BW_50HZ		1
#define BMA150_BW_100HZ		2
#define BMA150_BW_190HZ		3
#define BMA150_BW_375HZ		4
#define BMA150_BW_750HZ		5
#define BMA150_BW_1500HZ	6

struct bma150_cfg {
	bool any_motion_int;		/*                                    */
	bool hg_int;			/*                                */
	bool lg_int;			/*                               */
	unsigned char any_motion_dur;	/*                     */
	unsigned char any_motion_thres;	/*                      */
	unsigned char hg_hyst;		/*                   */
	unsigned char hg_dur;		/*                 */
	unsigned char hg_thres;		/*                  */
	unsigned char lg_hyst;		/*                  */
	unsigned char lg_dur;		/*                */
	unsigned char lg_thres;		/*                 */
	unsigned char range;		/*                          */
	unsigned char bandwidth;	/*                       */
};

struct bma150_platform_data {
	struct bma150_cfg cfg;
	int (*irq_gpio_cfg)(void);
};

#endif /*            */
