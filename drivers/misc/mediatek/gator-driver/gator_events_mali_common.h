/**
 * Copyright (C) ARM Limited 2012-2014. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#if !defined(GATOR_EVENTS_MALI_COMMON_H)
#define GATOR_EVENTS_MALI_COMMON_H

#include "gator.h"

#include <linux/module.h>
#include <linux/time.h>
#include <linux/math64.h>
#include <linux/slab.h>
#include <linux/io.h>

/*                                                         */
#ifndef MALI_SUPPORT
#error MALI_SUPPORT not defined!
#endif

/*                                               */
#define ACTIVITY_START  (1)
#define ACTIVITY_STOP   (2)

/*
                                           
 */
struct mali_counter {
	/*                                                            */
	unsigned long key;
	/*                      */
	unsigned long enabled;
	/*                                                          */
	unsigned long cores;
};

/*
           
 */
typedef int mali_profiling_set_event_type(unsigned int, int);
typedef void mali_profiling_control_type(unsigned int, unsigned int);

/*
                                                              
 */
extern int _mali_profiling_set_event(unsigned int, int);
extern void _mali_profiling_control(unsigned int, unsigned int);
extern void _mali_profiling_get_counters(unsigned int *, unsigned int *, unsigned int *, unsigned int *);

/* 
                                                                                                
                                                         
  
                                           
                              
                                                               
                                                                                    
                                                                                                 
  
                                                         
 */
extern int gator_mali_create_file_system(const char *mali_name, const char *event_name, struct super_block *sb, struct dentry *root, struct mali_counter *counter, unsigned long *event);

/* 
                                 
  
                                    
                                                                 
 */
extern void gator_mali_initialise_counters(struct mali_counter counters[], unsigned int n_counters);

#endif /*                             */
