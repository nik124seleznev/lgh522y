/*
 *  fair_share.c - A simple weight based Thermal governor
 *
 *  Copyright (C) 2012 Intel Corp
 *  Copyright (C) 2012 Durgadoss R <durgadoss.r@intel.com>
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

#include <linux/thermal.h>

#include "thermal_core.h"

/* 
                                                              
                            
 */
static int get_trip_level(struct thermal_zone_device *tz)
{
	int count = 0;
	unsigned long trip_temp;

	if (tz->trips == 0 || !tz->ops->get_trip_temp)
		return 0;

	for (count = 0; count < tz->trips; count++) {
		tz->ops->get_trip_temp(tz, count, &trip_temp);
		if (tz->temperature < trip_temp)
			break;
	}
	return count;
}

static long get_target_state(struct thermal_zone_device *tz,
		struct thermal_cooling_device *cdev, int weight, int level)
{
	unsigned long max_state;

	cdev->ops->get_max_state(cdev, &max_state);

	return (long)(weight * level * max_state) / (100 * tz->trips);
}

/* 
                                                                         
                            
  
                                                                    
                                                                        
  
                                  
                                                                       
                     
                                                                  
                                      
                                                                      
                                                                        
                                                                      
                                                           
                                             
 */
static int fair_share_throttle(struct thermal_zone_device *tz, int trip)
{
	const struct thermal_zone_params *tzp;
	struct thermal_cooling_device *cdev;
	struct thermal_instance *instance;
	int i;
	int cur_trip_level = get_trip_level(tz);

	if (!tz->tzp || !tz->tzp->tbp)
		return -EINVAL;

	tzp = tz->tzp;

	for (i = 0; i < tzp->num_tbps; i++) {
		if (!tzp->tbp[i].cdev)
			continue;

		cdev = tzp->tbp[i].cdev;
		instance = get_thermal_instance(tz, cdev, trip);
		if (!instance)
			continue;

		instance->target = get_target_state(tz, cdev,
					tzp->tbp[i].weight, cur_trip_level);

		instance->cdev->updated = false;
		thermal_cdev_update(cdev);
	}
	return 0;
}

static struct thermal_governor thermal_gov_fair_share = {
	.name		= "fair_share",
	.throttle	= fair_share_throttle,
};

int thermal_gov_fair_share_register(void)
{
	return thermal_register_governor(&thermal_gov_fair_share);
}

void thermal_gov_fair_share_unregister(void)
{
	thermal_unregister_governor(&thermal_gov_fair_share);
}

