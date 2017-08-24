/*
 * Copyright (C) 2012 Synopsys, Inc. (www.synopsys.com)
 *
 * Based on reduced version of METAG
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */


#include <linux/init.h>
#include <linux/reboot.h>
#include <linux/memblock.h>
#include <linux/of.h>
#include <linux/of_fdt.h>
#include <asm/prom.h>
#include <asm/clk.h>
#include <asm/mach_desc.h>

/* 
                                                                         
                                           
  
                                                                       
                                        
 */
struct machine_desc * __init setup_machine_fdt(void *dt)
{
	struct boot_param_header *devtree = dt;
	struct machine_desc *mdesc = NULL, *mdesc_best = NULL;
	unsigned int score, mdesc_score = ~1;
	unsigned long dt_root;
	const char *model, *compat;
	void *clk;
	char manufacturer[16];
	unsigned long len;

	/*                            */
	if (be32_to_cpu(devtree->magic) != OF_DT_HEADER)
		return NULL;

	initial_boot_params = devtree;
	dt_root = of_get_flat_dt_root();

	/*
                                                                    
                                                                
                              
  */
	for_each_machine_desc(mdesc) {
		score = of_flat_dt_match(dt_root, mdesc->dt_compat);
		if (score > 0 && score < mdesc_score) {
			mdesc_best = mdesc;
			mdesc_score = score;
		}
	}
	if (!mdesc_best) {
		const char *prop;
		long size;

		pr_err("\n unrecognized device tree list:\n[ ");

		prop = of_get_flat_dt_prop(dt_root, "compatible", &size);
		if (prop) {
			while (size > 0) {
				printk("'%s' ", prop);
				size -= strlen(prop) + 1;
				prop += strlen(prop) + 1;
			}
		}
		printk("]\n\n");

		machine_halt();
	}

	/*                                   */
	compat =  mdesc_best->dt_compat[0];

	model = strchr(compat, ',');
	if (model)
		model++;

	strlcpy(manufacturer, compat, model ? model - compat : strlen(compat));

	pr_info("Board \"%s\" from %s (Manufacturer)\n", model, manufacturer);

	/*                                                    */
	of_scan_flat_dt(early_init_dt_scan_chosen, boot_command_line);

	/*                                      */
	of_scan_flat_dt(early_init_dt_scan_root, NULL);

	/*                                                     */
	of_scan_flat_dt(early_init_dt_scan_memory, NULL);

	clk = of_get_flat_dt_prop(dt_root, "clock-frequency", &len);
	if (clk)
		arc_set_core_freq(of_read_ulong(clk, len/4));

	return mdesc_best;
}

/*
                                                                             
                                                     
 */
void __init copy_devtree(void)
{
	void *alloc = early_init_dt_alloc_memory_arch(
			be32_to_cpu(initial_boot_params->totalsize), 64);
	if (alloc) {
		memcpy(alloc, initial_boot_params,
				be32_to_cpu(initial_boot_params->totalsize));
		initial_boot_params = alloc;
	}
}
