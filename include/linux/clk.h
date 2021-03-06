/*
 *  linux/include/linux/clk.h
 *
 *  Copyright (C) 2004 ARM Limited.
 *  Written by Deep Blue Solutions Limited.
 *  Copyright (C) 2011-2012 Linaro Ltd <mturquette@linaro.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef __LINUX_CLK_H
#define __LINUX_CLK_H

#include <linux/err.h>
#include <linux/kernel.h>
#include <linux/notifier.h>

struct device;

struct clk;

#ifdef CONFIG_COMMON_CLK

/* 
                                   
  
                                                                       
                                                                   
                                                                        
                                                                        
                                 
  
                                                                      
                                                                        
                                                                    
                                              
  
                                                                       
                                                                         
  
 */
#define PRE_RATE_CHANGE			BIT(0)
#define POST_RATE_CHANGE		BIT(1)
#define ABORT_RATE_CHANGE		BIT(2)

/* 
                                                        
                                                    
                                                        
                              
  
                                                                    
                                                                      
                                                                   
                  
 */
struct clk_notifier {
	struct clk			*clk;
	struct srcu_notifier_head	notifier_head;
	struct list_head		node;
};

/* 
                                                                        
                                   
                                       
                                  
  
                                                                  
                                                                      
                                                                 
                                                               
 */
struct clk_notifier_data {
	struct clk		*clk;
	unsigned long		old_rate;
	unsigned long		new_rate;
};

int clk_notifier_register(struct clk *clk, struct notifier_block *nb);

int clk_notifier_unregister(struct clk *clk, struct notifier_block *nb);

#endif

/* 
                                       
                     
  
                                          
  
                                                 
 */
#ifdef CONFIG_HAVE_CLK_PREPARE
int clk_prepare(struct clk *clk);
#else
static inline int clk_prepare(struct clk *clk)
{
	might_sleep();
	return 0;
}
#endif

/* 
                                                     
                     
  
                                                                    
                                             
  
                                                 
 */
#ifdef CONFIG_HAVE_CLK_PREPARE
void clk_unprepare(struct clk *clk);
#else
static inline void clk_unprepare(struct clk *clk)
{
	might_sleep();
}
#endif

#ifdef CONFIG_HAVE_CLK
/* 
                                                               
                                    
                         
  
                                                               
                                                                 
                                                                 
                                                               
                                                                   
  
                                                            
  
                                                              
 */
struct clk *clk_get(struct device *dev, const char *id);

/* 
                                                                            
                                    
                         
  
                                                               
                                                                 
                                                                 
                                                               
                                                                   
  
                                                            
  
                                                                   
  
                                                                   
                
 */
struct clk *devm_clk_get(struct device *dev, const char *id);

/* 
                                                                          
                     
  
                                                                        
  
                                      
  
                                         
 */
int clk_enable(struct clk *clk);

/* 
                                                                               
                     
  
                                                                 
                                 
  
                                      
  
                                                               
                                                               
                                                                
            
 */
void clk_disable(struct clk *clk);

/* 
                                                                           
                                                                
                     
 */
unsigned long clk_get_rate(struct clk *clk);

/* 
                                    
                     
  
                                                                   
                                                                  
                 
  
                                                              
 */
void clk_put(struct clk *clk);

/* 
                                               
                                         
                                                  
  
                                                                   
                                                                  
                 
  
                                                              
 */
void devm_clk_put(struct device *dev, struct clk *clk);

/*
                                                             
 */


/* 
                                                                       
                     
                                  
  
                                                       
 */
long clk_round_rate(struct clk *clk, unsigned long rate);

/* 
                                                       
                     
                                  
  
                                         
 */
int clk_set_rate(struct clk *clk, unsigned long rate);

/* 
                                                              
                     
                               
  
                                         
 */
int clk_set_parent(struct clk *clk, struct clk *parent);

/* 
                                                              
                     
  
                                                              
                                             
 */
struct clk *clk_get_parent(struct clk *clk);

/* 
                                                       
                       
                         
  
                                                               
                                                                 
                                                                
                                                                     
                                                                         
  
                                                            
  
                                                                  
 */
struct clk *clk_get_sys(const char *dev_id, const char *con_id);

#else /*                  */

static inline struct clk *clk_get(struct device *dev, const char *id)
{
	return NULL;
}

static inline struct clk *devm_clk_get(struct device *dev, const char *id)
{
	return NULL;
}

static inline void clk_put(struct clk *clk) {}

static inline void devm_clk_put(struct device *dev, struct clk *clk) {}

static inline int clk_enable(struct clk *clk)
{
	return 0;
}

static inline void clk_disable(struct clk *clk) {}

static inline unsigned long clk_get_rate(struct clk *clk)
{
	return 0;
}

static inline int clk_set_rate(struct clk *clk, unsigned long rate)
{
	return 0;
}

static inline long clk_round_rate(struct clk *clk, unsigned long rate)
{
	return 0;
}

static inline int clk_set_parent(struct clk *clk, struct clk *parent)
{
	return 0;
}

static inline struct clk *clk_get_parent(struct clk *clk)
{
	return NULL;
}

#endif

/*                                                                        */
static inline int clk_prepare_enable(struct clk *clk)
{
	int ret;

	ret = clk_prepare(clk);
	if (ret)
		return ret;
	ret = clk_enable(clk);
	if (ret)
		clk_unprepare(clk);

	return ret;
}

/*                                                                            */
static inline void clk_disable_unprepare(struct clk *clk)
{
	clk_disable(clk);
	clk_unprepare(clk);
}

/* 
                                        
                               
                               
                                    
               
  
                                                                      
                                              
 */
int clk_add_alias(const char *alias, const char *alias_dev_name, char *id,
			struct device *dev);

struct device_node;
struct of_phandle_args;

#if defined(CONFIG_OF) && defined(CONFIG_COMMON_CLK)
struct clk *of_clk_get(struct device_node *np, int index);
struct clk *of_clk_get_by_name(struct device_node *np, const char *name);
struct clk *of_clk_get_from_provider(struct of_phandle_args *clkspec);
#else
static inline struct clk *of_clk_get(struct device_node *np, int index)
{
	return ERR_PTR(-ENOENT);
}
static inline struct clk *of_clk_get_by_name(struct device_node *np,
					     const char *name)
{
	return ERR_PTR(-ENOENT);
}
#endif

#endif
