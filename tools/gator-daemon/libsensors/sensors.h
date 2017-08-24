/*
    sensors.h - Part of libsensors, a Linux library for reading sensor data.
    Copyright (c) 1998, 1999  Frodo Looijaard <frodol@dds.nl>
    Copyright (C) 2007, 2010  Jean Delvare <khali@linux-fr.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
    MA 02110-1301 USA.
*/

/*                                                                          */

#ifndef LIB_SENSORS_SENSORS_H
#define LIB_SENSORS_SENSORS_H

#include <stdio.h>
#include <limits.h>

/*                                       */

/*                                                                         
                                                                            
                                                                               
                                      */
#define SENSORS_API_VERSION		0x432

#define SENSORS_CHIP_NAME_PREFIX_ANY	NULL
#define SENSORS_CHIP_NAME_ADDR_ANY	(-1)

#define SENSORS_BUS_TYPE_ANY		(-1)
#define SENSORS_BUS_TYPE_I2C		0
#define SENSORS_BUS_TYPE_ISA		1
#define SENSORS_BUS_TYPE_PCI		2
#define SENSORS_BUS_TYPE_SPI		3
#define SENSORS_BUS_TYPE_VIRTUAL	4
#define SENSORS_BUS_TYPE_ACPI		5
#define SENSORS_BUS_TYPE_HID		6
#define SENSORS_BUS_NR_ANY		(-1)
#define SENSORS_BUS_NR_IGNORE		(-2)

#ifdef __cplusplus
extern "C" {
#endif /*             */

extern const char *libsensors_version;

extern int sensors_sysfs_no_scaling;

typedef struct sensors_bus_id {
	short type;
	short nr;
} sensors_bus_id;

/*                                          */
typedef struct sensors_chip_name {
	char *prefix;
	sensors_bus_id bus;
	int addr;
	char *path;
} sensors_chip_name;

/*                                                                 
                                                                   
                                                               
                                                                     
                                 */
int sensors_init(FILE *input);

/*                                                   
                                             */
void sensors_cleanup(void);

/*                                                                          
             */
int sensors_parse_chip_name(const char *orig_name, sensors_chip_name *res);

/*                                                                       */
void sensors_free_chip_name(sensors_chip_name *chip);

/*                                                                          
                                                                          
                                            */
int sensors_snprintf_chip_name(char *str, size_t size,
			       const sensors_chip_name *chip);

/*                                                 
                                                                            
                   */
const char *sensors_get_adapter_name(const sensors_bus_id *bus);

typedef struct sensors_feature sensors_feature;

/*                                                                 
                                                                           
                                           
                                                                     */
char *sensors_get_label(const sensors_chip_name *name,
			const sensors_feature *feature);

/*                                                                            
                                                                          
                */
int sensors_get_value(const sensors_chip_name *name, int subfeat_nr,
		      double *value);

/*                                                                           
                                                                          
               */
int sensors_set_value(const sensors_chip_name *name, int subfeat_nr,
		      double value);

/*                                                                          
                                                                          */
int sensors_do_chip_sets(const sensors_chip_name *name);

/*                                                                       
                                                                            
                                                                           
                                                                           
                                      */
const sensors_chip_name *sensors_get_detected_chips(const sensors_chip_name
						    *match, int *nr);

/*                                                                 */
#define SENSORS_MODE_R			1
#define SENSORS_MODE_W			2
#define SENSORS_COMPUTE_MAPPING		4

typedef enum sensors_feature_type {
	SENSORS_FEATURE_IN		= 0x00,
	SENSORS_FEATURE_FAN		= 0x01,
	SENSORS_FEATURE_TEMP		= 0x02,
	SENSORS_FEATURE_POWER		= 0x03,
	SENSORS_FEATURE_ENERGY		= 0x04,
	SENSORS_FEATURE_CURR		= 0x05,
	SENSORS_FEATURE_HUMIDITY	= 0x06,
	SENSORS_FEATURE_MAX_MAIN,
	SENSORS_FEATURE_VID		= 0x10,
	SENSORS_FEATURE_INTRUSION	= 0x11,
	SENSORS_FEATURE_MAX_OTHER,
	SENSORS_FEATURE_BEEP_ENABLE	= 0x18,
	SENSORS_FEATURE_UNKNOWN		= INT_MAX,
} sensors_feature_type;

/*                                                                         
                                                                        */
typedef enum sensors_subfeature_type {
	SENSORS_SUBFEATURE_IN_INPUT = SENSORS_FEATURE_IN << 8,
	SENSORS_SUBFEATURE_IN_MIN,
	SENSORS_SUBFEATURE_IN_MAX,
	SENSORS_SUBFEATURE_IN_LCRIT,
	SENSORS_SUBFEATURE_IN_CRIT,
	SENSORS_SUBFEATURE_IN_AVERAGE,
	SENSORS_SUBFEATURE_IN_LOWEST,
	SENSORS_SUBFEATURE_IN_HIGHEST,
	SENSORS_SUBFEATURE_IN_ALARM = (SENSORS_FEATURE_IN << 8) | 0x80,
	SENSORS_SUBFEATURE_IN_MIN_ALARM,
	SENSORS_SUBFEATURE_IN_MAX_ALARM,
	SENSORS_SUBFEATURE_IN_BEEP,
	SENSORS_SUBFEATURE_IN_LCRIT_ALARM,
	SENSORS_SUBFEATURE_IN_CRIT_ALARM,

	SENSORS_SUBFEATURE_FAN_INPUT = SENSORS_FEATURE_FAN << 8,
	SENSORS_SUBFEATURE_FAN_MIN,
	SENSORS_SUBFEATURE_FAN_MAX,
	SENSORS_SUBFEATURE_FAN_ALARM = (SENSORS_FEATURE_FAN << 8) | 0x80,
	SENSORS_SUBFEATURE_FAN_FAULT,
	SENSORS_SUBFEATURE_FAN_DIV,
	SENSORS_SUBFEATURE_FAN_BEEP,
	SENSORS_SUBFEATURE_FAN_PULSES,
	SENSORS_SUBFEATURE_FAN_MIN_ALARM,
	SENSORS_SUBFEATURE_FAN_MAX_ALARM,

	SENSORS_SUBFEATURE_TEMP_INPUT = SENSORS_FEATURE_TEMP << 8,
	SENSORS_SUBFEATURE_TEMP_MAX,
	SENSORS_SUBFEATURE_TEMP_MAX_HYST,
	SENSORS_SUBFEATURE_TEMP_MIN,
	SENSORS_SUBFEATURE_TEMP_CRIT,
	SENSORS_SUBFEATURE_TEMP_CRIT_HYST,
	SENSORS_SUBFEATURE_TEMP_LCRIT,
	SENSORS_SUBFEATURE_TEMP_EMERGENCY,
	SENSORS_SUBFEATURE_TEMP_EMERGENCY_HYST,
	SENSORS_SUBFEATURE_TEMP_LOWEST,
	SENSORS_SUBFEATURE_TEMP_HIGHEST,
	SENSORS_SUBFEATURE_TEMP_ALARM = (SENSORS_FEATURE_TEMP << 8) | 0x80,
	SENSORS_SUBFEATURE_TEMP_MAX_ALARM,
	SENSORS_SUBFEATURE_TEMP_MIN_ALARM,
	SENSORS_SUBFEATURE_TEMP_CRIT_ALARM,
	SENSORS_SUBFEATURE_TEMP_FAULT,
	SENSORS_SUBFEATURE_TEMP_TYPE,
	SENSORS_SUBFEATURE_TEMP_OFFSET,
	SENSORS_SUBFEATURE_TEMP_BEEP,
	SENSORS_SUBFEATURE_TEMP_EMERGENCY_ALARM,
	SENSORS_SUBFEATURE_TEMP_LCRIT_ALARM,

	SENSORS_SUBFEATURE_POWER_AVERAGE = SENSORS_FEATURE_POWER << 8,
	SENSORS_SUBFEATURE_POWER_AVERAGE_HIGHEST,
	SENSORS_SUBFEATURE_POWER_AVERAGE_LOWEST,
	SENSORS_SUBFEATURE_POWER_INPUT,
	SENSORS_SUBFEATURE_POWER_INPUT_HIGHEST,
	SENSORS_SUBFEATURE_POWER_INPUT_LOWEST,
	SENSORS_SUBFEATURE_POWER_CAP,
	SENSORS_SUBFEATURE_POWER_CAP_HYST,
	SENSORS_SUBFEATURE_POWER_MAX,
	SENSORS_SUBFEATURE_POWER_CRIT,
	SENSORS_SUBFEATURE_POWER_AVERAGE_INTERVAL = (SENSORS_FEATURE_POWER << 8) | 0x80,
	SENSORS_SUBFEATURE_POWER_ALARM,
	SENSORS_SUBFEATURE_POWER_CAP_ALARM,
	SENSORS_SUBFEATURE_POWER_MAX_ALARM,
	SENSORS_SUBFEATURE_POWER_CRIT_ALARM,

	SENSORS_SUBFEATURE_ENERGY_INPUT = SENSORS_FEATURE_ENERGY << 8,

	SENSORS_SUBFEATURE_CURR_INPUT = SENSORS_FEATURE_CURR << 8,
	SENSORS_SUBFEATURE_CURR_MIN,
	SENSORS_SUBFEATURE_CURR_MAX,
	SENSORS_SUBFEATURE_CURR_LCRIT,
	SENSORS_SUBFEATURE_CURR_CRIT,
	SENSORS_SUBFEATURE_CURR_AVERAGE,
	SENSORS_SUBFEATURE_CURR_LOWEST,
	SENSORS_SUBFEATURE_CURR_HIGHEST,
	SENSORS_SUBFEATURE_CURR_ALARM = (SENSORS_FEATURE_CURR << 8) | 0x80,
	SENSORS_SUBFEATURE_CURR_MIN_ALARM,
	SENSORS_SUBFEATURE_CURR_MAX_ALARM,
	SENSORS_SUBFEATURE_CURR_BEEP,
	SENSORS_SUBFEATURE_CURR_LCRIT_ALARM,
	SENSORS_SUBFEATURE_CURR_CRIT_ALARM,

	SENSORS_SUBFEATURE_HUMIDITY_INPUT = SENSORS_FEATURE_HUMIDITY << 8,

	SENSORS_SUBFEATURE_VID = SENSORS_FEATURE_VID << 8,

	SENSORS_SUBFEATURE_INTRUSION_ALARM = SENSORS_FEATURE_INTRUSION << 8,
	SENSORS_SUBFEATURE_INTRUSION_BEEP,

	SENSORS_SUBFEATURE_BEEP_ENABLE = SENSORS_FEATURE_BEEP_ENABLE << 8,

	SENSORS_SUBFEATURE_UNKNOWN = INT_MAX,
} sensors_subfeature_type;

/*                                                       */
struct sensors_feature {
	char *name;
	int number;
	sensors_feature_type type;
	/*                                                    */
	int first_subfeature;
	int padding1;
};

/*                                     
                                                                             
                                                                            
                       
                              
                                                                     
                                                                           
                                      
                                                                                
                                                                           
                                            */
typedef struct sensors_subfeature {
	char *name;
	int number;
	sensors_subfeature_type type;
	int mapping;
	unsigned int flags;
} sensors_subfeature;

/*                                                                       
                                                                         
                                            
                                                                         
                    */
const sensors_feature *
sensors_get_features(const sensors_chip_name *name, int *nr);

/*                                                                          
                                                                         
                                            
                                                                         
                    */
const sensors_subfeature *
sensors_get_all_subfeatures(const sensors_chip_name *name,
			    const sensors_feature *feature, int *nr);

/*                                                                        
                                
                                                                         
                    */
const sensors_subfeature *
sensors_get_subfeature(const sensors_chip_name *name,
		       const sensors_feature *feature,
		       sensors_subfeature_type type);

#ifdef __cplusplus
}
#endif /*             */

#endif /*                         */
