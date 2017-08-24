/*
 * Device tree helpers for DMA request / controller
 *
 * Based on of_gpio.c
 *
 * Copyright (C) 2012 Texas Instruments Incorporated - http://www.ti.com/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/device.h>
#include <linux/err.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/of_dma.h>

static LIST_HEAD(of_dma_list);
static DEFINE_MUTEX(of_dma_lock);

/* 
                                                                       
                                                                  
  
                                                                            
                                                                               
                                                                               
         
 */
static struct of_dma *of_dma_find_controller(struct of_phandle_args *dma_spec)
{
	struct of_dma *ofdma;

	list_for_each_entry(ofdma, &of_dma_list, of_dma_controllers)
		if ((ofdma->of_node == dma_spec->np) &&
		    (ofdma->of_dma_nbcells == dma_spec->args_count))
			return ofdma;

	pr_debug("%s: can't find DMA controller %s\n", __func__,
		 dma_spec->np->full_name);

	return NULL;
}

/* 
                                                                           
                                       
                                                               
                                             
                                                           
                         
  
                                                            
  
                                                                             
        
 */
int of_dma_controller_register(struct device_node *np,
				struct dma_chan *(*of_dma_xlate)
				(struct of_phandle_args *, struct of_dma *),
				void *data)
{
	struct of_dma	*ofdma;
	int		nbcells;
	const __be32	*prop;

	if (!np || !of_dma_xlate) {
		pr_err("%s: not enough information provided\n", __func__);
		return -EINVAL;
	}

	ofdma = kzalloc(sizeof(*ofdma), GFP_KERNEL);
	if (!ofdma)
		return -ENOMEM;

	prop = of_get_property(np, "#dma-cells", NULL);
	if (prop)
		nbcells = be32_to_cpup(prop);

	if (!prop || !nbcells) {
		pr_err("%s: #dma-cells property is missing or invalid\n",
		       __func__);
		kfree(ofdma);
		return -EINVAL;
	}

	ofdma->of_node = np;
	ofdma->of_dma_nbcells = nbcells;
	ofdma->of_dma_xlate = of_dma_xlate;
	ofdma->of_dma_data = data;

	/*                                               */
	mutex_lock(&of_dma_lock);
	list_add_tail(&ofdma->of_dma_controllers, &of_dma_list);
	mutex_unlock(&of_dma_lock);

	return 0;
}
EXPORT_SYMBOL_GPL(of_dma_controller_register);

/* 
                                                                            
                                      
  
                                                                  
 */
void of_dma_controller_free(struct device_node *np)
{
	struct of_dma *ofdma;

	mutex_lock(&of_dma_lock);

	list_for_each_entry(ofdma, &of_dma_list, of_dma_controllers)
		if (ofdma->of_node == np) {
			list_del(&ofdma->of_dma_controllers);
			kfree(ofdma);
			break;
		}

	mutex_unlock(&of_dma_lock);
}
EXPORT_SYMBOL_GPL(of_dma_controller_free);

/* 
                                                               
                                             
                                    
                                                           
                                                                  
  
                                                                      
                                                                           
                                                                            
 */
static int of_dma_match_channel(struct device_node *np, const char *name,
				int index, struct of_phandle_args *dma_spec)
{
	const char *s;

	if (of_property_read_string_index(np, "dma-names", index, &s))
		return -ENODEV;

	if (strcmp(name, s))
		return -ENODEV;

	if (of_parse_phandle_with_args(np, "dmas", "#dma-cells", index,
				       dma_spec))
		return -ENODEV;

	return 0;
}

/* 
                                                           
                                            
                                 
  
                                                                          
 */
struct dma_chan *of_dma_request_slave_channel(struct device_node *np,
					      const char *name)
{
	struct of_phandle_args	dma_spec;
	struct of_dma		*ofdma;
	struct dma_chan		*chan;
	int			count, i;

	if (!np || !name) {
		pr_err("%s: not enough information provided\n", __func__);
		return NULL;
	}

	count = of_property_count_strings(np, "dma-names");
	if (count < 0) {
		pr_err("%s: dma-names property missing or empty\n", __func__);
		return NULL;
	}

	for (i = 0; i < count; i++) {
		if (of_dma_match_channel(np, name, i, &dma_spec))
			continue;

		mutex_lock(&of_dma_lock);
		ofdma = of_dma_find_controller(&dma_spec);

		if (ofdma)
			chan = ofdma->of_dma_xlate(&dma_spec, ofdma);
		else
			chan = NULL;

		mutex_unlock(&of_dma_lock);

		of_node_put(dma_spec.np);

		if (chan)
			return chan;
	}

	return NULL;
}

/* 
                                                               
                                                                  
                                          
  
                                                                            
                                                                           
                                                                             
                                                                            
                                                                  
 */
struct dma_chan *of_dma_simple_xlate(struct of_phandle_args *dma_spec,
						struct of_dma *ofdma)
{
	int count = dma_spec->args_count;
	struct of_dma_filter_info *info = ofdma->of_dma_data;

	if (!info || !info->filter_fn)
		return NULL;

	if (count != 1)
		return NULL;

	return dma_request_channel(info->dma_cap, info->filter_fn,
			&dma_spec->args[0]);
}
EXPORT_SYMBOL_GPL(of_dma_simple_xlate);
