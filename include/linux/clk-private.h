/*
 *  linux/include/linux/clk-private.h
 *
 *  Copyright (c) 2010-2011 Jeremy Kerr <jeremy.kerr@canonical.com>
 *  Copyright (C) 2011-2012 Linaro Ltd <mturquette@linaro.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef __LINUX_CLK_PRIVATE_H
#define __LINUX_CLK_PRIVATE_H

#include <linux/clk-provider.h>
#include <linux/list.h>

/*
                                                                             
                                              
  
                                                                               
                                                                           
                                                      
 */

#ifdef CONFIG_COMMON_CLK

struct clk {
	const char		*name;
	const struct clk_ops	*ops;
	struct clk_hw		*hw;
	struct clk		*parent;
	const char		**parent_names;
	struct clk		**parents;
	u8			num_parents;
	unsigned long		rate;
	unsigned long		new_rate;
	unsigned long		flags;
	unsigned int		enable_count;
	unsigned int		prepare_count;
	struct hlist_head	children;
	struct hlist_node	child_node;
	unsigned int		notifier_count;
#ifdef CONFIG_COMMON_CLK_DEBUG
	struct dentry		*dentry;
#endif
};

/*
                                                            
  
                                                                            
                                                                               
                                                                      
                                              
 */

#define DEFINE_CLK(_name, _ops, _flags, _parent_names,		\
		_parents)					\
	static struct clk _name = {				\
		.name = #_name,					\
		.ops = &_ops,					\
		.hw = &_name##_hw.hw,				\
		.parent_names = _parent_names,			\
		.num_parents = ARRAY_SIZE(_parent_names),	\
		.parents = _parents,				\
		.flags = _flags | CLK_IS_BASIC,			\
	}

#define DEFINE_CLK_FIXED_RATE(_name, _flags, _rate,		\
				_fixed_rate_flags)		\
	static struct clk _name;				\
	static const char *_name##_parent_names[] = {};		\
	static struct clk_fixed_rate _name##_hw = {		\
		.hw = {						\
			.clk = &_name,				\
		},						\
		.fixed_rate = _rate,				\
		.flags = _fixed_rate_flags,			\
	};							\
	DEFINE_CLK(_name, clk_fixed_rate_ops, _flags,		\
			_name##_parent_names, NULL);

#define DEFINE_CLK_GATE(_name, _parent_name, _parent_ptr,	\
				_flags, _reg, _bit_idx,		\
				_gate_flags, _lock)		\
	static struct clk _name;				\
	static const char *_name##_parent_names[] = {		\
		_parent_name,					\
	};							\
	static struct clk *_name##_parents[] = {		\
		_parent_ptr,					\
	};							\
	static struct clk_gate _name##_hw = {			\
		.hw = {						\
			.clk = &_name,				\
		},						\
		.reg = _reg,					\
		.bit_idx = _bit_idx,				\
		.flags = _gate_flags,				\
		.lock = _lock,					\
	};							\
	DEFINE_CLK(_name, clk_gate_ops, _flags,			\
			_name##_parent_names, _name##_parents);

#define _DEFINE_CLK_DIVIDER(_name, _parent_name, _parent_ptr,	\
				_flags, _reg, _shift, _width,	\
				_divider_flags, _table, _lock)	\
	static struct clk _name;				\
	static const char *_name##_parent_names[] = {		\
		_parent_name,					\
	};							\
	static struct clk *_name##_parents[] = {		\
		_parent_ptr,					\
	};							\
	static struct clk_divider _name##_hw = {		\
		.hw = {						\
			.clk = &_name,				\
		},						\
		.reg = _reg,					\
		.shift = _shift,				\
		.width = _width,				\
		.flags = _divider_flags,			\
		.table = _table,				\
		.lock = _lock,					\
	};							\
	DEFINE_CLK(_name, clk_divider_ops, _flags,		\
			_name##_parent_names, _name##_parents);

#define DEFINE_CLK_DIVIDER(_name, _parent_name, _parent_ptr,	\
				_flags, _reg, _shift, _width,	\
				_divider_flags, _lock)		\
	_DEFINE_CLK_DIVIDER(_name, _parent_name, _parent_ptr,	\
				_flags, _reg, _shift, _width,	\
				_divider_flags, NULL, _lock)

#define DEFINE_CLK_DIVIDER_TABLE(_name, _parent_name,		\
				_parent_ptr, _flags, _reg,	\
				_shift, _width, _divider_flags,	\
				_table, _lock)			\
	_DEFINE_CLK_DIVIDER(_name, _parent_name, _parent_ptr,	\
				_flags, _reg, _shift, _width,	\
				_divider_flags, _table, _lock)	\

#define DEFINE_CLK_MUX(_name, _parent_names, _parents, _flags,	\
				_reg, _shift, _width,		\
				_mux_flags, _lock)		\
	static struct clk _name;				\
	static struct clk_mux _name##_hw = {			\
		.hw = {						\
			.clk = &_name,				\
		},						\
		.reg = _reg,					\
		.shift = _shift,				\
		.mask = BIT(_width) - 1,			\
		.flags = _mux_flags,				\
		.lock = _lock,					\
	};							\
	DEFINE_CLK(_name, clk_mux_ops, _flags, _parent_names,	\
			_parents);

#define DEFINE_CLK_FIXED_FACTOR(_name, _parent_name,		\
				_parent_ptr, _flags,		\
				_mult, _div)			\
	static struct clk _name;				\
	static const char *_name##_parent_names[] = {		\
		_parent_name,					\
	};							\
	static struct clk *_name##_parents[] = {		\
		_parent_ptr,					\
	};							\
	static struct clk_fixed_factor _name##_hw = {		\
		.hw = {						\
			.clk = &_name,				\
		},						\
		.mult = _mult,					\
		.div = _div,					\
	};							\
	DEFINE_CLK(_name, clk_fixed_factor_ops, _flags,		\
			_name##_parent_names, _name##_parents);

/* 
                                                              
                                                          
                              
  
                                                                    
                                      
  
                                                                        
             
         
        
       
                 
                
          
  
                                                                               
                                     
  
                                                 
 */
int __clk_init(struct device *dev, struct clk *clk);

struct clk *__clk_register(struct device *dev, struct clk_hw *hw);

#endif /*                   */
#endif /*               */
