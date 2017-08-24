/*
 * Tegra host1x Command DMA
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

#ifndef __HOST1X_CDMA_H
#define __HOST1X_CDMA_H

#include <linux/sched.h>
#include <linux/semaphore.h>
#include <linux/list.h>

struct host1x_syncpt;
struct host1x_userctx_timeout;
struct host1x_job;

/*
       
  
                                                   
                                                                     
                                                                 
            
        
                                      
                                                             
            
                                                                   
 */

struct push_buffer {
	u32 *mapped;			/*                          */
	dma_addr_t phys;		/*                                */
	u32 fence;			/*                     */
	u32 pos;			/*                   */
	u32 size_bytes;
};

struct buffer_timeout {
	struct delayed_work wq;		/*            */
	bool initialized;		/*                           */
	struct host1x_syncpt *syncpt;	/*                          */
	u32 syncpt_val;			/*                             */
	ktime_t start_ktime;		/*               */
	/*                             */
	int client;
};

enum cdma_event {
	CDMA_EVENT_NONE,		/*                           */
	CDMA_EVENT_SYNC_QUEUE_EMPTY,	/*                           */
	CDMA_EVENT_PUSH_BUFFER_SPACE	/*                               */
};

struct host1x_cdma {
	struct mutex lock;		/*                                 */
	struct semaphore sem;		/*                             */
	enum cdma_event event;		/*                               */
	unsigned int slots_used;	/*                                 */
	unsigned int slots_free;	/*                                 */
	unsigned int first_get;		/*                                   */
	unsigned int last_pos;		/*                              */
	struct push_buffer push_buffer;	/*                       */
	struct list_head sync_queue;	/*           */
	struct buffer_timeout timeout;	/*                            */
	bool running;
	bool torndown;
};

#define cdma_to_channel(cdma) container_of(cdma, struct host1x_channel, cdma)
#define cdma_to_host1x(cdma) dev_get_drvdata(cdma_to_channel(cdma)->dev->parent)
#define pb_to_cdma(pb) container_of(pb, struct host1x_cdma, push_buffer)

int host1x_cdma_init(struct host1x_cdma *cdma);
int host1x_cdma_deinit(struct host1x_cdma *cdma);
void host1x_cdma_stop(struct host1x_cdma *cdma);
int host1x_cdma_begin(struct host1x_cdma *cdma, struct host1x_job *job);
void host1x_cdma_push(struct host1x_cdma *cdma, u32 op1, u32 op2);
void host1x_cdma_end(struct host1x_cdma *cdma, struct host1x_job *job);
void host1x_cdma_update(struct host1x_cdma *cdma);
void host1x_cdma_peek(struct host1x_cdma *cdma, u32 dmaget, int slot,
		      u32 *out);
unsigned int host1x_cdma_wait_locked(struct host1x_cdma *cdma,
				     enum cdma_event event);
void host1x_cdma_update_sync_queue(struct host1x_cdma *cdma,
				   struct device *dev);
#endif
