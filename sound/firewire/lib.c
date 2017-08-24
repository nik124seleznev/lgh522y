/*
 * miscellaneous helper functions
 *
 * Copyright (c) Clemens Ladisch <clemens@ladisch.de>
 * Licensed under the terms of the GNU General Public License, version 2.
 */

#include <linux/delay.h>
#include <linux/device.h>
#include <linux/firewire.h>
#include <linux/module.h>
#include "lib.h"

#define ERROR_RETRY_DELAY_MS	5

/* 
                                                                  
                                                
                               
                                                     
                             
                             
  
                                                                          
                                                                            
                                                                      
                                                     
 */
int snd_fw_transaction(struct fw_unit *unit, int tcode,
		       u64 offset, void *buffer, size_t length)
{
	struct fw_device *device = fw_parent_device(unit);
	int generation, rcode, tries = 0;

	for (;;) {
		generation = device->generation;
		smp_rmb(); /*                        */
		rcode = fw_run_transaction(device->card, tcode,
					   device->node_id, generation,
					   device->max_speed, offset,
					   buffer, length);

		if (rcode == RCODE_COMPLETE)
			return 0;

		if (rcode_is_permanent_error(rcode) || ++tries >= 3) {
			dev_err(&unit->device, "transaction failed: %s\n",
				fw_rcode_string(rcode));
			return -EIO;
		}

		msleep(ERROR_RETRY_DELAY_MS);
	}
}
EXPORT_SYMBOL(snd_fw_transaction);

MODULE_DESCRIPTION("FireWire audio helper functions");
MODULE_AUTHOR("Clemens Ladisch <clemens@ladisch.de>");
MODULE_LICENSE("GPL v2");
