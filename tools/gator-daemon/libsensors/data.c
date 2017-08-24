/*
    data.c - Part of libsensors, a Linux library for reading sensor data.
    Copyright (c) 1998, 1999  Frodo Looijaard <frodol@dds.nl>
    Copyright (C) 2007, 2009  Jean Delvare <khali@linux-fr.org>

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

/*                                                                                           */

/*                                  */
#define _GNU_SOURCE

#include <stdlib.h>
#include <string.h>

#include "access.h"
#include "error.h"
#include "data.h"
#include "sensors.h"
#include "version.h"

const char *libsensors_version = LM_VERSION;

char **sensors_config_files = NULL;
int sensors_config_files_count = 0;
int sensors_config_files_max = 0;

sensors_chip *sensors_config_chips = NULL;
int sensors_config_chips_count = 0;
int sensors_config_chips_subst = 0;
int sensors_config_chips_max = 0;

sensors_bus *sensors_config_busses = NULL;
int sensors_config_busses_count = 0;
int sensors_config_busses_max = 0;

sensors_chip_features *sensors_proc_chips = NULL;
int sensors_proc_chips_count = 0;
int sensors_proc_chips_max = 0;

sensors_bus *sensors_proc_bus = NULL;
int sensors_proc_bus_count = 0;
int sensors_proc_bus_max = 0;

void sensors_free_chip_name(sensors_chip_name *chip)
{
	free(chip->prefix);
}

/*
                                                                           

                                
                              
                              
                            
                              
                         
                 

                                                     
                                                                        
                                                                        
                                                   

                                                                         
                                                                          
                                                                       
*/

int sensors_parse_chip_name(const char *name, sensors_chip_name *res)
{
	char *dash;

	/*                                                         */
	if (!strncmp(name, "*-", 2)) {
		res->prefix = SENSORS_CHIP_NAME_PREFIX_ANY;
		name += 2;
	} else {
		if (!(dash = strchr(name, '-')))
			return -SENSORS_ERR_CHIP_NAME;
		res->prefix = strndup(name, dash - name);
		if (!res->prefix)
			sensors_fatal_error(__func__,
					    "Allocating name prefix");
		name = dash + 1;
	}

	/*                                                                   
                         */
	if (!strcmp(name, "*")) {
		res->bus.type = SENSORS_BUS_TYPE_ANY;
		res->bus.nr = SENSORS_BUS_NR_ANY;
		res->addr = SENSORS_CHIP_NAME_ADDR_ANY;
		return 0;
	}

	if (!(dash = strchr(name, '-')))
		goto ERROR;
	if (!strncmp(name, "i2c", dash - name))
		res->bus.type = SENSORS_BUS_TYPE_I2C;
	else if (!strncmp(name, "isa", dash - name))
		res->bus.type = SENSORS_BUS_TYPE_ISA;
	else if (!strncmp(name, "pci", dash - name))
		res->bus.type = SENSORS_BUS_TYPE_PCI;
	else if (!strncmp(name, "spi", dash - name))
		res->bus.type = SENSORS_BUS_TYPE_SPI;
	else if (!strncmp(name, "virtual", dash - name))
		res->bus.type = SENSORS_BUS_TYPE_VIRTUAL;
	else if (!strncmp(name, "acpi", dash - name))
		res->bus.type = SENSORS_BUS_TYPE_ACPI;
	else if (!strncmp(name, "hid", dash - name))
		res->bus.type = SENSORS_BUS_TYPE_HID;
	else
		goto ERROR;
	name = dash + 1;

	/*                                                         
                                                                   
                         */
	switch (res->bus.type) {
	case SENSORS_BUS_TYPE_I2C:
	case SENSORS_BUS_TYPE_SPI:
	case SENSORS_BUS_TYPE_HID:
		if (!strncmp(name, "*-", 2)) {
			res->bus.nr = SENSORS_BUS_NR_ANY;
			name += 2;
			break;
		}

		res->bus.nr = strtoul(name, &dash, 10);
		if (*name == '\0' || *dash != '-' || res->bus.nr < 0)
			goto ERROR;
		name = dash + 1;
		break;
	default:
		res->bus.nr = SENSORS_BUS_NR_ANY;
	}

	/*                                                        */
	if (!strcmp(name, "*")) {
		res->addr = SENSORS_CHIP_NAME_ADDR_ANY;
	} else {
		res->addr = strtoul(name, &dash, 16);
		if (*name == '\0' || *dash != '\0' || res->addr < 0)
			goto ERROR;
	}

	return 0;

ERROR:
	free(res->prefix);
	return -SENSORS_ERR_CHIP_NAME;
}

int sensors_snprintf_chip_name(char *str, size_t size,
			       const sensors_chip_name *chip)
{
	if (sensors_chip_name_has_wildcards(chip))
		return -SENSORS_ERR_WILDCARDS;

	switch (chip->bus.type) {
	case SENSORS_BUS_TYPE_ISA:
		return snprintf(str, size, "%s-isa-%04x", chip->prefix,
				chip->addr);
	case SENSORS_BUS_TYPE_PCI:
		return snprintf(str, size, "%s-pci-%04x", chip->prefix,
				chip->addr);
	case SENSORS_BUS_TYPE_I2C:
		return snprintf(str, size, "%s-i2c-%hd-%02x", chip->prefix,
				chip->bus.nr, chip->addr);
	case SENSORS_BUS_TYPE_SPI:
		return snprintf(str, size, "%s-spi-%hd-%x", chip->prefix,
				chip->bus.nr, chip->addr);
	case SENSORS_BUS_TYPE_VIRTUAL:
		return snprintf(str, size, "%s-virtual-%x", chip->prefix,
				chip->addr);
	case SENSORS_BUS_TYPE_ACPI:
		return snprintf(str, size, "%s-acpi-%x", chip->prefix,
				chip->addr);
	case SENSORS_BUS_TYPE_HID:
		return snprintf(str, size, "%s-hid-%hd-%x", chip->prefix,
				chip->bus.nr, chip->addr);
	}

	return -SENSORS_ERR_CHIP_NAME;
}

int sensors_parse_bus_id(const char *name, sensors_bus_id *bus)
{
	char *endptr;

	if (strncmp(name, "i2c-", 4)) {
		return -SENSORS_ERR_BUS_NAME;
	}
	name += 4;
	bus->type = SENSORS_BUS_TYPE_I2C;
	bus->nr = strtoul(name, &endptr, 10);
	if (*name == '\0' || *endptr != '\0' || bus->nr < 0)
		return -SENSORS_ERR_BUS_NAME;
	return 0;
}

static int sensors_substitute_chip(sensors_chip_name *name,
				   const char *filename, int lineno)
{
	int i, j;
	for (i = 0; i < sensors_config_busses_count; i++)
		if (sensors_config_busses[i].bus.type == name->bus.type &&
		    sensors_config_busses[i].bus.nr == name->bus.nr)
			break;

	if (i == sensors_config_busses_count) {
		sensors_parse_error_wfn("Undeclared bus id referenced",
					filename, lineno);
		name->bus.nr = SENSORS_BUS_NR_IGNORE;
		return -SENSORS_ERR_BUS_NAME;
	}

	/*                           */
	for (j = 0; j < sensors_proc_bus_count; j++) {
		if (!strcmp(sensors_config_busses[i].adapter,
			    sensors_proc_bus[j].adapter)) {
			name->bus.nr = sensors_proc_bus[j].bus.nr;
			return 0;
		}
	}

	/*                                                             
                  */
	name->bus.nr = SENSORS_BUS_NR_IGNORE;
	return 0;
}

/*                                                                  
                                                                         
                        */
int sensors_substitute_busses(void)
{
	int err, i, j, lineno;
	sensors_chip_name_list *chips;
	const char *filename;
	int res = 0;

	for (i = sensors_config_chips_subst;
	     i < sensors_config_chips_count; i++) {
		filename = sensors_config_chips[i].line.filename;
		lineno = sensors_config_chips[i].line.lineno;
		chips = &sensors_config_chips[i].chips;
		for (j = 0; j < chips->fits_count; j++) {
			/*                                                
                */
			if (chips->fits[j].bus.nr == SENSORS_BUS_NR_ANY)
				continue;

			err = sensors_substitute_chip(&chips->fits[j],
						      filename, lineno);
			if (err)
				res = err;
		}
	}
	sensors_config_chips_subst = sensors_config_chips_count;
	return res;
}
