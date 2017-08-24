/*
 * Copyright (C) 2010-2011 Canonical Ltd <jeremy.kerr@canonical.com>
 * Copyright (C) 2011-2012 Mike Turquette, Linaro Ltd <mturquette@linaro.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Fixed rate clock implementation
 */

#include <linux/clk-provider.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/err.h>
#include <linux/of.h>

/*
                                               
  
                        
                                                              
                                                       
                                                                
                                                    
 */

#define to_clk_fixed_rate(_hw) container_of(_hw, struct clk_fixed_rate, hw)

static unsigned long clk_fixed_rate_recalc_rate(struct clk_hw *hw,
		unsigned long parent_rate)
{
	return to_clk_fixed_rate(hw)->fixed_rate;
}

const struct clk_ops clk_fixed_rate_ops = {
	.recalc_rate = clk_fixed_rate_recalc_rate,
};
EXPORT_SYMBOL_GPL(clk_fixed_rate_ops);

/* 
                                                                               
                                              
                            
                                       
                                   
                                         
 */
struct clk *clk_register_fixed_rate(struct device *dev, const char *name,
		const char *parent_name, unsigned long flags,
		unsigned long fixed_rate)
{
	struct clk_fixed_rate *fixed;
	struct clk *clk;
	struct clk_init_data init;

	/*                           */
	fixed = kzalloc(sizeof(struct clk_fixed_rate), GFP_KERNEL);
	if (!fixed) {
		pr_err("%s: could not allocate fixed clk\n", __func__);
		return ERR_PTR(-ENOMEM);
	}

	init.name = name;
	init.ops = &clk_fixed_rate_ops;
	init.flags = flags | CLK_IS_BASIC;
	init.parent_names = (parent_name ? &parent_name: NULL);
	init.num_parents = (parent_name ? 1 : 0);

	/*                                   */
	fixed->fixed_rate = fixed_rate;
	fixed->hw.init = &init;

	/*                    */
	clk = clk_register(dev, &fixed->hw);

	if (IS_ERR(clk))
		kfree(fixed);

	return clk;
}

#ifdef CONFIG_OF
/* 
                                                                    
 */
void of_fixed_clk_setup(struct device_node *node)
{
	struct clk *clk;
	const char *clk_name = node->name;
	u32 rate;

	if (of_property_read_u32(node, "clock-frequency", &rate))
		return;

	of_property_read_string(node, "clock-output-names", &clk_name);

	clk = clk_register_fixed_rate(NULL, clk_name, NULL, CLK_IS_ROOT, rate);
	if (!IS_ERR(clk))
		of_clk_add_provider(node, of_clk_src_simple_get, clk);
}
EXPORT_SYMBOL_GPL(of_fixed_clk_setup);
CLK_OF_DECLARE(fixed_clk, "fixed-clock", of_fixed_clk_setup);
#endif
