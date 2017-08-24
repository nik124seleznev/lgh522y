/*
 *
 * Copyright 1999 Digi International (www.digi.com)
 *     James Puzzo <jamesp at digi dot com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY, EXPRESS OR IMPLIED; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 */

#ifndef __DGRP_COMMON_H
#define __DGRP_COMMON_H

#define DIGI_VERSION "1.9-29"

#include <linux/fs.h>
#include <linux/timer.h>
#include "drp.h"

#define DGRP_TTIME 100
#define DGRP_RTIME 100

/*                                                                       
                                 
                                                                        */

extern int dgrp_register_cudevices; /*                          */
extern int dgrp_register_prdevices; /*                                  */
extern int dgrp_poll_tick;          /*                       */

extern struct list_head nd_struct_list;

struct dgrp_poll_data {
	spinlock_t poll_lock;
	struct timer_list timer;
	int poll_tick;
	ulong poll_round;	/*                      */
	long node_active_count;
};

extern struct dgrp_poll_data dgrp_poll_data;
extern void dgrp_poll_handler(unsigned long arg);

/*                     */
extern const struct file_operations dgrp_mon_ops;

/*                 */
extern int dgrp_tty_init(struct nd_struct *nd);
extern void dgrp_tty_uninit(struct nd_struct *nd);

/*                       */
extern const struct file_operations dgrp_ports_ops;

/*                     */
extern const struct file_operations dgrp_net_ops;

/*                     */
extern const struct file_operations dgrp_dpa_ops;
extern void dgrp_dpa_data(struct nd_struct *, int, u8 *, int);

/*                   */
extern int dgrp_create_class_sysfs_files(void);
extern void dgrp_remove_class_sysfs_files(void);

extern void dgrp_create_node_class_sysfs_files(struct nd_struct *nd);
extern void dgrp_remove_node_class_sysfs_files(struct nd_struct *nd);

extern void dgrp_create_tty_sysfs(struct un_struct *un, struct device *c);
extern void dgrp_remove_tty_sysfs(struct device *c);

/*                      */
extern void dgrp_unregister_proc(void);
extern void dgrp_register_proc(void);

/*                                                                        
  
                                       
  
                                                          
                                                   
  
                                                                         */

void dgrp_carrier(struct ch_struct *ch);


/*
                                                              
                                                                        
 */

static inline void ID_TO_CHAR(long i, char *s)
{
	s[0] = ((i & 0xff00)>>8);
	s[1] = (i & 0xff);
	s[2] = 0;
}

static inline long CHAR_TO_ID(char *s)
{
	return ((s[0] & 0xff) << 8) | (s[1] & 0xff);
}

static inline struct nd_struct *nd_struct_get(long major)
{
	struct nd_struct *nd;

	list_for_each_entry(nd, &nd_struct_list, list) {
		if (major == nd->nd_major)
			return nd;
	}

	return NULL;
}

static inline int nd_struct_add(struct nd_struct *entry)
{
	struct nd_struct *ptr;

	ptr = nd_struct_get(entry->nd_major);

	if (ptr)
		return -EBUSY;

	list_add_tail(&entry->list, &nd_struct_list);

	return 0;
}

static inline int nd_struct_del(struct nd_struct *entry)
{
	struct nd_struct *nd;

	nd = nd_struct_get(entry->nd_major);

	if (!nd)
		return -ENODEV;

	list_del(&nd->list);
	return 0;
}

#endif /*                 */
