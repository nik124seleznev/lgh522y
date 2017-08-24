/*
 * net/tipc/core.h: Include file for TIPC global declarations
 *
 * Copyright (c) 2005-2006, Ericsson AB
 * Copyright (c) 2005-2007, 2010-2011, Wind River Systems
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the names of the copyright holders nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * Alternatively, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") version 2 as published by the Free
 * Software Foundation.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _TIPC_CORE_H
#define _TIPC_CORE_H

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/tipc.h>
#include <linux/tipc_config.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/timer.h>
#include <linux/string.h>
#include <asm/uaccess.h>
#include <linux/interrupt.h>
#include <linux/atomic.h>
#include <asm/hardirq.h>
#include <linux/netdevice.h>
#include <linux/in.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>


#define TIPC_MOD_VER "2.0.0"

#define ULTRA_STRING_MAX_LEN	32768
#define TIPC_MAX_SUBSCRIPTIONS	65535
#define TIPC_MAX_PUBLICATIONS	65535

struct tipc_msg;	/*       */

int tipc_snprintf(char *buf, int len, const char *fmt, ...);

/*
                            
 */
#define ELINKCONG EAGAIN	/*                                          */

/*
                                 
 */
extern u32 tipc_own_addr __read_mostly;
extern int tipc_max_ports __read_mostly;
extern int tipc_net_id __read_mostly;
extern int tipc_remote_management __read_mostly;

/*
                         
 */
extern int tipc_random __read_mostly;

/*
                                              
 */
extern int tipc_core_start_net(unsigned long);
extern int  tipc_handler_start(void);
extern void tipc_handler_stop(void);
extern int  tipc_netlink_start(void);
extern void tipc_netlink_stop(void);
extern int  tipc_socket_init(void);
extern void tipc_socket_stop(void);

/*
                             
 */
typedef void (*Handler) (unsigned long);

u32 tipc_k_signal(Handler routine, unsigned long argument);

/* 
                                    
                                     
                                                            
                                                         
  
                                                                               
 */
static inline void k_init_timer(struct timer_list *timer, Handler routine,
				unsigned long argument)
{
	setup_timer(timer, routine, argument);
}

/* 
                                
                                     
                               
  
                                                                
                                                                               
  
                                                                         
                                                                   
                                                                 
                                                               
 */
static inline void k_start_timer(struct timer_list *timer, unsigned long msec)
{
	mod_timer(timer, jiffies + msecs_to_jiffies(msec) + 1);
}

/* 
                                  
                                     
  
                                          
                                                              
  
                                                                         
                                                                         
 */
static inline void k_cancel_timer(struct timer_list *timer)
{
	del_timer_sync(timer);
}

/* 
                                   
                                     
  
                                                          
  
                                                             
  
                                                                          
                                                                           
 */
static inline void k_term_timer(struct timer_list *timer)
{
}

/*
                           
  
                                                                 
                                                                   
  
                                                                            
                                            
 */
#define BUF_HEADROOM LL_MAX_HEADER

struct tipc_skb_cb {
	void *handle;
};

#define TIPC_SKB_CB(__skb) ((struct tipc_skb_cb *)&((__skb)->cb[0]))

static inline struct tipc_msg *buf_msg(struct sk_buff *skb)
{
	return (struct tipc_msg *)skb->data;
}

extern struct sk_buff *tipc_buf_acquire(u32 size);

#endif