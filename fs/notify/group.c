/*
 *  Copyright (C) 2008 Red Hat, Inc., Eric Paris <eparis@redhat.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/srcu.h>
#include <linux/rculist.h>
#include <linux/wait.h>

#include <linux/fsnotify_backend.h>
#include "fsnotify.h"

#include <linux/atomic.h>

/*
                           
 */
void fsnotify_final_destroy_group(struct fsnotify_group *group)
{
	if (group->ops->free_group_priv)
		group->ops->free_group_priv(group);

	kfree(group);
}

/*
                                                                               
                       
                                                                             
                           
 */
void fsnotify_destroy_group(struct fsnotify_group *group)
{
	/*                                      */
	fsnotify_clear_marks_by_group(group);

	synchronize_srcu(&fsnotify_mark_srcu);

	/*                                            */
	fsnotify_flush_notify(group);

	fsnotify_put_group(group);
}

/*
                            
 */
void fsnotify_get_group(struct fsnotify_group *group)
{
	atomic_inc(&group->refcnt);
}

/*
                                                         
 */
void fsnotify_put_group(struct fsnotify_group *group)
{
	if (atomic_dec_and_test(&group->refcnt))
		fsnotify_final_destroy_group(group);
}

/*
                                                                           
 */
struct fsnotify_group *fsnotify_alloc_group(const struct fsnotify_ops *ops)
{
	struct fsnotify_group *group;

	group = kzalloc(sizeof(struct fsnotify_group), GFP_KERNEL);
	if (!group)
		return ERR_PTR(-ENOMEM);

	/*                                                            */
	atomic_set(&group->refcnt, 1);
	atomic_set(&group->num_marks, 0);

	mutex_init(&group->notification_mutex);
	INIT_LIST_HEAD(&group->notification_list);
	init_waitqueue_head(&group->notification_waitq);
	group->max_events = UINT_MAX;

	mutex_init(&group->mark_mutex);
	INIT_LIST_HEAD(&group->marks_list);

	group->ops = ops;

	return group;
}

int fsnotify_fasync(int fd, struct file *file, int on)
{
	struct fsnotify_group *group = file->private_data;

	return fasync_helper(fd, file, on, &group->fsn_fa) >= 0 ? 0 : -EIO;
}
