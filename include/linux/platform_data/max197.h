/*
 * Maxim MAX197 A/D Converter Driver
 *
 * Copyright (c) 2012 Savoir-faire Linux Inc.
 *          Vivien Didelot <vivien.didelot@savoirfairelinux.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * For further information, see the Documentation/hwmon/max197 file.
 */

/* 
                                                         
                                                                        
                                                          
 */
struct max197_platform_data {
	int (*convert)(u8 ctrl);
};
