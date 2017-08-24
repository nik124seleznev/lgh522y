/*
 * linux/sound/cs42l52.h -- Platform data for CS42L52
 *
 * Copyright (c) 2012 Cirrus Logic Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __CS42L52_H
#define __CS42L52_H

struct cs42l52_platform_data {

	/*                                     */
	unsigned int micbias_lvl;

	/*                                             */
	unsigned int mica_cfg;

	/*                                             */
	unsigned int micb_cfg;

	/*                             */
	unsigned int mica_sel;

	/*                             */
	unsigned int micb_sel;

	/*                                        */
	unsigned int chgfreq;

};

#endif /*             */
