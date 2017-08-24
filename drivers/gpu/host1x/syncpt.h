/*
 * Tegra host1x Syncpoints
 *
 * Copyright (c) 2010-2013, NVIDIA Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __HOST1X_SYNCPT_H
#define __HOST1X_SYNCPT_H

#include <linux/atomic.h>
#include <linux/kernel.h>
#include <linux/sched.h>

#include "intr.h"

struct host1x;

/*                                                   */
#define HOST1X_SYNCPT_RESERVED			0

struct host1x_syncpt {
	int id;
	atomic_t min_val;
	atomic_t max_val;
	u32 base_val;
	const char *name;
	int client_managed;
	struct host1x *host;
	struct device *dev;

	/*                */
	struct host1x_syncpt_intr intr;
};

/*                              */
int host1x_syncpt_init(struct host1x *host);

/*                        */
void host1x_syncpt_deinit(struct host1x *host);

/*
                                                                           
                                   
   */
static inline u32 host1x_syncpt_read_max(struct host1x_syncpt *sp)
{
	smp_rmb();
	return (u32)atomic_read(&sp->max_val);
}

/*
                                                                           
 */
static inline u32 host1x_syncpt_read_min(struct host1x_syncpt *sp)
{
	smp_rmb();
	return (u32)atomic_read(&sp->min_val);
}

/*                                        */
int host1x_syncpt_nb_pts(struct host1x *host);

/*                                        */
int host1x_syncpt_nb_bases(struct host1x *host);

/*                                    */
int host1x_syncpt_nb_mlocks(struct host1x *host);

/*
                                                                          
                         
  
                                             
   */
static inline bool host1x_syncpt_check_max(struct host1x_syncpt *sp, u32 real)
{
	u32 max;
	if (sp->client_managed)
		return true;
	max = host1x_syncpt_read_max(sp);
	return (s32)(max - real) >= 0;
}

/*                                              */
static inline int host1x_syncpt_client_managed(struct host1x_syncpt *sp)
{
	return sp->client_managed;
}

/*
                                                                      
                          
 */
static inline bool host1x_syncpt_idle(struct host1x_syncpt *sp)
{
	int min, max;
	smp_rmb();
	min = atomic_read(&sp->min_val);
	max = atomic_read(&sp->max_val);
	return (min == max);
}

/*                                                  */
struct host1x_syncpt *host1x_syncpt_get(struct host1x *host, u32 id);

/*                                    */
void host1x_syncpt_cpu_incr(struct host1x_syncpt *sp);

/*                                                          */
u32 host1x_syncpt_load(struct host1x_syncpt *sp);

/*                                                       */
bool host1x_syncpt_is_expired(struct host1x_syncpt *sp, u32 thresh);

/*                                                     */
void host1x_syncpt_save(struct host1x *host);

/*                                                      */
void host1x_syncpt_restore(struct host1x *host);

/*                                                                  */
u32 host1x_syncpt_load_wait_base(struct host1x_syncpt *sp);

/*                                   */
void host1x_syncpt_incr(struct host1x_syncpt *sp);

/*                                                                */
u32 host1x_syncpt_incr_max(struct host1x_syncpt *sp, u32 incrs);

/*                                                                */
int host1x_syncpt_wait(struct host1x_syncpt *sp, u32 thresh,
			long timeout, u32 *value);

/*                                  */
static inline int host1x_syncpt_is_valid(struct host1x_syncpt *sp)
{
	return sp->id < host1x_syncpt_nb_pts(sp->host);
}

/*                                                               */
int host1x_syncpt_patch_wait(struct host1x_syncpt *sp, void *patch_addr);

/*                             */
u32 host1x_syncpt_id(struct host1x_syncpt *sp);

/*                                     */
struct host1x_syncpt *host1x_syncpt_request(struct device *dev,
		int client_managed);

/*                    */
void host1x_syncpt_free(struct host1x_syncpt *sp);

#endif
