#ifndef __NET_SCHED_CODEL_H
#define __NET_SCHED_CODEL_H

/*
 * Codel - The Controlled-Delay Active Queue Management algorithm
 *
 *  Copyright (C) 2011-2012 Kathleen Nichols <nichols@pollere.com>
 *  Copyright (C) 2011-2012 Van Jacobson <van@pollere.net>
 *  Copyright (C) 2012 Michael D. Taht <dave.taht@bufferbloat.net>
 *  Copyright (C) 2012 Eric Dumazet <edumazet@google.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions, and the following disclaimer,
 *    without modification.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The names of the authors may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * Alternatively, provided that this notice is retained in full, this
 * software may be distributed under the terms of the GNU General
 * Public License ("GPL") version 2, in which case the provisions of the
 * GPL apply INSTEAD OF those given above.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 *
 */

#include <linux/types.h>
#include <linux/ktime.h>
#include <linux/skbuff.h>
#include <net/pkt_sched.h>
#include <net/inet_ecn.h>
#include <linux/reciprocal_div.h>

/*                                          
                                            
                                             
                                             
  
                                                     
 */


/*                                             
                                                                 
 */
typedef u32 codel_time_t;
typedef s32 codel_tdiff_t;
#define CODEL_SHIFT 10
#define MS2TIME(a) ((a * NSEC_PER_MSEC) >> CODEL_SHIFT)

static inline codel_time_t codel_get_time(void)
{
	u64 ns = ktime_to_ns(ktime_get());

	return ns >> CODEL_SHIFT;
}

#define codel_time_after(a, b)		((s32)(a) - (s32)(b) > 0)
#define codel_time_after_eq(a, b)	((s32)(a) - (s32)(b) >= 0)
#define codel_time_before(a, b)		((s32)(a) - (s32)(b) < 0)
#define codel_time_before_eq(a, b)	((s32)(a) - (s32)(b) <= 0)

/*                                                                   */
struct codel_skb_cb {
	codel_time_t enqueue_time;
};

static struct codel_skb_cb *get_codel_cb(const struct sk_buff *skb)
{
	qdisc_cb_private_validate(skb, sizeof(struct codel_skb_cb));
	return (struct codel_skb_cb *)qdisc_skb_cb(skb)->data;
}

static codel_time_t codel_get_enqueue_time(const struct sk_buff *skb)
{
	return get_codel_cb(skb)->enqueue_time;
}

static void codel_set_enqueue_time(struct sk_buff *skb)
{
	get_codel_cb(skb)->enqueue_time = codel_get_time();
}

static inline u32 codel_time_to_us(codel_time_t val)
{
	u64 valns = ((u64)val << CODEL_SHIFT);

	do_div(valns, NSEC_PER_USEC);
	return (u32)valns;
}

/* 
                                                  
                                             
                                         
                                                    
 */
struct codel_params {
	codel_time_t	target;
	codel_time_t	interval;
	bool		ecn;
};

/* 
                                               
                                                            
                           
                                                
                                               
                                                      
                                                                         
                 
                                                                 
                                                 
 */
struct codel_vars {
	u32		count;
	u32		lastcount;
	bool		dropping;
	u16		rec_inv_sqrt;
	codel_time_t	first_above_time;
	codel_time_t	drop_next;
	codel_time_t	ldelay;
};

#define REC_INV_SQRT_BITS (8 * sizeof(u16)) /*                                 */
/*                                                      */
#define REC_INV_SQRT_SHIFT (32 - REC_INV_SQRT_BITS)

/* 
                                                                 
                                               
                                                          
                                                                
 */
struct codel_stats {
	u32		maxpacket;
	u32		drop_count;
	u32		ecn_mark;
};

static void codel_params_init(struct codel_params *params)
{
	params->interval = MS2TIME(100);
	params->target = MS2TIME(5);
	params->ecn = false;
}

static void codel_vars_init(struct codel_vars *vars)
{
	memset(vars, 0, sizeof(*vars));
}

static void codel_stats_init(struct codel_stats *stats)
{
	stats->maxpacket = 256;
}

/*
                                                                                                               
                                                        
  
                                                                           
 */
static void codel_Newton_step(struct codel_vars *vars)
{
	u32 invsqrt = ((u32)vars->rec_inv_sqrt) << REC_INV_SQRT_SHIFT;
	u32 invsqrt2 = ((u64)invsqrt * invsqrt) >> 32;
	u64 val = (3LL << 32) - ((u64)vars->count * invsqrt2);

	val >>= 2; /*                                      */
	val = (val * invsqrt) >> (32 - 2 + 1);

	vars->rec_inv_sqrt = val >> REC_INV_SQRT_SHIFT;
}

/*
                                                
                                                                           
                                    
 */
static codel_time_t codel_control_law(codel_time_t t,
				      codel_time_t interval,
				      u32 rec_inv_sqrt)
{
	return t + reciprocal_divide(interval, rec_inv_sqrt << REC_INV_SQRT_SHIFT);
}


static bool codel_should_drop(const struct sk_buff *skb,
			      struct Qdisc *sch,
			      struct codel_vars *vars,
			      struct codel_params *params,
			      struct codel_stats *stats,
			      codel_time_t now)
{
	bool ok_to_drop;

	if (!skb) {
		vars->first_above_time = 0;
		return false;
	}

	vars->ldelay = now - codel_get_enqueue_time(skb);
	sch->qstats.backlog -= qdisc_pkt_len(skb);

	if (unlikely(qdisc_pkt_len(skb) > stats->maxpacket))
		stats->maxpacket = qdisc_pkt_len(skb);

	if (codel_time_before(vars->ldelay, params->target) ||
	    sch->qstats.backlog <= stats->maxpacket) {
		/*                                               */
		vars->first_above_time = 0;
		return false;
	}
	ok_to_drop = false;
	if (vars->first_above_time == 0) {
		/*                                             
                                                    
   */
		vars->first_above_time = now + params->interval;
	} else if (codel_time_after(now, vars->first_above_time)) {
		ok_to_drop = true;
	}
	return ok_to_drop;
}

typedef struct sk_buff * (*codel_skb_dequeue_t)(struct codel_vars *vars,
						struct Qdisc *sch);

static struct sk_buff *codel_dequeue(struct Qdisc *sch,
				     struct codel_params *params,
				     struct codel_vars *vars,
				     struct codel_stats *stats,
				     codel_skb_dequeue_t dequeue_func)
{
	struct sk_buff *skb = dequeue_func(vars, sch);
	codel_time_t now;
	bool drop;

	if (!skb) {
		vars->dropping = false;
		return skb;
	}
	now = codel_get_time();
	drop = codel_should_drop(skb, sch, vars, params, stats, now);
	if (vars->dropping) {
		if (!drop) {
			/*                                                  */
			vars->dropping = false;
		} else if (codel_time_after_eq(now, vars->drop_next)) {
			/*                                              
                                                    
                                    
                                     
                                                        
                                           
                           
    */
			while (vars->dropping &&
			       codel_time_after_eq(now, vars->drop_next)) {
				vars->count++; /*                           
                                     
      */
				codel_Newton_step(vars);
				if (params->ecn && INET_ECN_set_ce(skb)) {
					stats->ecn_mark++;
					vars->drop_next =
						codel_control_law(vars->drop_next,
								  params->interval,
								  vars->rec_inv_sqrt);
					goto end;
				}
				qdisc_drop(skb, sch);
				stats->drop_count++;
				skb = dequeue_func(vars, sch);
				if (!codel_should_drop(skb, sch,
						       vars, params, stats, now)) {
					/*                      */
					vars->dropping = false;
				} else {
					/*                            */
					vars->drop_next =
						codel_control_law(vars->drop_next,
								  params->interval,
								  vars->rec_inv_sqrt);
				}
			}
		}
	} else if (drop) {
		u32 delta;

		if (params->ecn && INET_ECN_set_ce(skb)) {
			stats->ecn_mark++;
		} else {
			qdisc_drop(skb, sch);
			stats->drop_count++;

			skb = dequeue_func(vars, sch);
			drop = codel_should_drop(skb, sch, vars, params,
						 stats, now);
		}
		vars->dropping = true;
		/*                                                             
                                                               
                                                           
   */
		delta = vars->count - vars->lastcount;
		if (delta > 1 &&
		    codel_time_before(now - vars->drop_next,
				      16 * params->interval)) {
			vars->count = delta;
			/*                                           
                           
                                                      
    */
			codel_Newton_step(vars);
		} else {
			vars->count = 1;
			vars->rec_inv_sqrt = ~0U >> REC_INV_SQRT_SHIFT;
		}
		vars->lastcount = vars->count;
		vars->drop_next = codel_control_law(now, params->interval,
						    vars->rec_inv_sqrt);
	}
end:
	return skb;
}
#endif
