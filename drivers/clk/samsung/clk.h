/*
 * Copyright (c) 2013 Samsung Electronics Co., Ltd.
 * Copyright (c) 2013 Linaro Ltd.
 * Author: Thomas Abraham <thomas.ab@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Common Clock Framework support for all Samsung platforms
*/

#ifndef __SAMSUNG_CLK_H
#define __SAMSUNG_CLK_H

#include <linux/clk.h>
#include <linux/clkdev.h>
#include <linux/io.h>
#include <linux/clk-provider.h>
#include <linux/of.h>
#include <linux/of_address.h>

/* 
                                                          
                                          
                                                             
                                                                  
 */
struct samsung_clock_alias {
	unsigned int		id;
	const char		*dev_name;
	const char		*alias;
};

#define ALIAS(_id, dname, a)	\
	{							\
		.id		= _id,				\
		.dev_name	= dname,			\
		.alias		= a,				\
	}

/* 
                                                                      
                                          
                                        
                                            
                                           
                                               
 */
struct samsung_fixed_rate_clock {
	unsigned int		id;
	char			*name;
	const char		*parent_name;
	unsigned long		flags;
	unsigned long		fixed_rate;
};

#define FRATE(_id, cname, pname, f, frate)		\
	{						\
		.id		= _id,			\
		.name		= cname,		\
		.parent_name	= pname,		\
		.flags		= f,			\
		.fixed_rate	= frate,		\
	}

/*
                                                                          
                                          
                                          
                                   
                                      
                               
                                             
 */
struct samsung_fixed_factor_clock {
	unsigned int		id;
	char			*name;
	const char		*parent_name;
	unsigned long		mult;
	unsigned long		div;
	unsigned long		flags;
};

#define FFACTOR(_id, cname, pname, m, d, f)		\
	{						\
		.id		= _id,			\
		.name		= cname,		\
		.parent_name	= pname,		\
		.mult		= m,			\
		.div		= d,			\
		.flags		= f,			\
	}

/* 
                                                        
                                          
                                                             
                                 
                                                         
                                                           
                                          
                                                           
                                                                      
                                                      
                                        
                                                                  
 */
struct samsung_mux_clock {
	unsigned int		id;
	const char		*dev_name;
	const char		*name;
	const char		**parent_names;
	u8			num_parents;
	unsigned long		flags;
	unsigned long		offset;
	u8			shift;
	u8			width;
	u8			mux_flags;
	const char		*alias;
};

#define __MUX(_id, dname, cname, pnames, o, s, w, f, mf, a)	\
	{							\
		.id		= _id,				\
		.dev_name	= dname,			\
		.name		= cname,			\
		.parent_names	= pnames,			\
		.num_parents	= ARRAY_SIZE(pnames),		\
		.flags		= f,				\
		.offset		= o,				\
		.shift		= s,				\
		.width		= w,				\
		.mux_flags	= mf,				\
		.alias		= a,				\
	}

#define MUX(_id, cname, pnames, o, s, w)			\
	__MUX(_id, NULL, cname, pnames, o, s, w, 0, 0, NULL)

#define MUX_A(_id, cname, pnames, o, s, w, a)			\
	__MUX(_id, NULL, cname, pnames, o, s, w, 0, 0, a)

#define MUX_F(_id, cname, pnames, o, s, w, f, mf)		\
	__MUX(_id, NULL, cname, pnames, o, s, w, f, mf, NULL)

/* 
                                          
                                                        
                                                             
                                 
                                          
                                          
                                                           
                                                                      
                                        
                                                                  
 */
struct samsung_div_clock {
	unsigned int		id;
	const char		*dev_name;
	const char		*name;
	const char		*parent_name;
	unsigned long		flags;
	unsigned long		offset;
	u8			shift;
	u8			width;
	u8			div_flags;
	const char		*alias;
	struct clk_div_table	*table;
};

#define __DIV(_id, dname, cname, pname, o, s, w, f, df, a, t)	\
	{							\
		.id		= _id,				\
		.dev_name	= dname,			\
		.name		= cname,			\
		.parent_name	= pname,			\
		.flags		= f,				\
		.offset		= o,				\
		.shift		= s,				\
		.width		= w,				\
		.div_flags	= df,				\
		.alias		= a,				\
		.table		= t,				\
	}

#define DIV(_id, cname, pname, o, s, w)				\
	__DIV(_id, NULL, cname, pname, o, s, w, 0, 0, NULL, NULL)

#define DIV_A(_id, cname, pname, o, s, w, a)			\
	__DIV(_id, NULL, cname, pname, o, s, w, 0, 0, a, NULL)

#define DIV_F(_id, cname, pname, o, s, w, f, df)		\
	__DIV(_id, NULL, cname, pname, o, s, w, f, df, NULL, NULL)

#define DIV_T(_id, cname, pname, o, s, w, t)			\
	__DIV(_id, NULL, cname, pname, o, s, w, 0, 0, NULL, t)

/* 
                                                          
                                          
                                                             
                                  
                                          
                                          
                                                            
                                                             
                                          
                                                                  
 */
struct samsung_gate_clock {
	unsigned int		id;
	const char		*dev_name;
	const char		*name;
	const char		*parent_name;
	unsigned long		flags;
	unsigned long		offset;
	u8			bit_idx;
	u8			gate_flags;
	const char		*alias;
};

#define __GATE(_id, dname, cname, pname, o, b, f, gf, a)	\
	{							\
		.id		= _id,				\
		.dev_name	= dname,			\
		.name		= cname,			\
		.parent_name	= pname,			\
		.flags		= f,				\
		.offset		= o,				\
		.bit_idx	= b,				\
		.gate_flags	= gf,				\
		.alias		= a,				\
	}

#define GATE(_id, cname, pname, o, b, f, gf)			\
	__GATE(_id, NULL, cname, pname, o, b, f, gf, NULL)

#define GATE_A(_id, cname, pname, o, b, f, gf, a)		\
	__GATE(_id, NULL, cname, pname, o, b, f, gf, a)

#define GATE_D(_id, dname, cname, pname, o, b, f, gf)		\
	__GATE(_id, dname, cname, pname, o, b, f, gf, NULL)

#define GATE_DA(_id, dname, cname, pname, o, b, f, gf, a)	\
	__GATE(_id, dname, cname, pname, o, b, f, gf, a)

#define PNAME(x) static const char *x[] __initdata

/* 
                                                                            
                                                                   
                                              
 */
struct samsung_clk_reg_dump {
	u32	offset;
	u32	value;
};

extern void __init samsung_clk_init(struct device_node *np, void __iomem *base,
		unsigned long nr_clks, unsigned long *rdump,
		unsigned long nr_rdump, unsigned long *soc_rdump,
		unsigned long nr_soc_rdump);
extern void __init samsung_clk_of_register_fixed_ext(
		struct samsung_fixed_rate_clock *fixed_rate_clk,
		unsigned int nr_fixed_rate_clk,
		struct of_device_id *clk_matches);

extern void samsung_clk_add_lookup(struct clk *clk, unsigned int id);

extern void samsung_clk_register_alias(struct samsung_clock_alias *list,
		unsigned int nr_clk);
extern void __init samsung_clk_register_fixed_rate(
		struct samsung_fixed_rate_clock *clk_list, unsigned int nr_clk);
extern void __init samsung_clk_register_fixed_factor(
		struct samsung_fixed_factor_clock *list, unsigned int nr_clk);
extern void __init samsung_clk_register_mux(struct samsung_mux_clock *clk_list,
		unsigned int nr_clk);
extern void __init samsung_clk_register_div(struct samsung_div_clock *clk_list,
		unsigned int nr_clk);
extern void __init samsung_clk_register_gate(
		struct samsung_gate_clock *clk_list, unsigned int nr_clk);

extern unsigned long _get_rate(const char *clk_name);

#endif /*                 */
