/*
 * TI ADS7828 A/D Converter platform data definition
 *
 * Copyright (c) 2012 Savoir-faire Linux Inc.
 *          Vivien Didelot <vivien.didelot@savoirfairelinux.com>
 *
 * For further information, see the Documentation/hwmon/ads7828 file.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef _PDATA_ADS7828_H
#define _PDATA_ADS7828_H

/* 
                                                                    
                                         
                                                 
                                                   
 */
struct ads7828_platform_data {
	bool diff_input;
	bool ext_vref;
	unsigned int vref_mv;
};

#endif /*                  */
