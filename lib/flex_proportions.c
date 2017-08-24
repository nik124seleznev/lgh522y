/*
 *  Floating proportions with flexible aging period
 *
 *   Copyright (C) 2011, SUSE, Jan Kara <jack@suse.cz>
 *
 * The goal of this code is: Given different types of event, measure proportion
 * of each type of event over time. The proportions are measured with
 * exponentially decaying history to give smooth transitions. A formula
 * expressing proportion of event of type 'j' is:
 *
 *   p_{j} = (\Sum_{i>=0} x_{i,j}/2^{i+1})/(\Sum_{i>=0} x_i/2^{i+1})
 *
 * Where x_{i,j} is j's number of events in i-th last time period and x_i is
 * total number of events in i-th last time period.
 *
 * Note that p_{j}'s are normalised, i.e.
 *
 *   \Sum_{j} p_{j} = 1,
 *
 * This formula can be straightforwardly computed by maintaing denominator
 * (let's call it 'd') and for each event type its numerator (let's call it
 * 'n_j'). When an event of type 'j' happens, we simply need to do:
 *   n_j++; d++;
 *
 * When a new period is declared, we could do:
 *   d /= 2
 *   for each j
 *     n_j /= 2
 *
 * To avoid iteration over all event types, we instead shift numerator of event
 * j lazily when someone asks for a proportion of event j or when event j
 * occurs. This can bit trivially implemented by remembering last period in
 * which something happened with proportion of type j.
 */
#include <linux/flex_proportions.h>

int fprop_global_init(struct fprop_global *p)
{
	int err;

	p->period = 0;
	/*                                                      */
	err = percpu_counter_init(&p->events, 1);
	if (err)
		return err;
	seqcount_init(&p->sequence);
	return 0;
}

void fprop_global_destroy(struct fprop_global *p)
{
	percpu_counter_destroy(&p->events);
}

/*
                                                                          
                                         
  
                                                                           
                                                                               
                                  
 */
bool fprop_new_period(struct fprop_global *p, int periods)
{
	s64 events;
	unsigned long flags;

	local_irq_save(flags);
	events = percpu_counter_sum(&p->events);
	/*
                                             
  */
	if (events <= 1) {
		local_irq_restore(flags);
		return false;
	}
	write_seqcount_begin(&p->sequence);
	if (periods < 64)
		events -= events >> periods;
	/*                                                                   */
	percpu_counter_add(&p->events, -events);
	p->period += periods;
	write_seqcount_end(&p->sequence);
	local_irq_restore(flags);

	return true;
}

/*
                   
 */

int fprop_local_init_single(struct fprop_local_single *pl)
{
	pl->events = 0;
	pl->period = 0;
	raw_spin_lock_init(&pl->lock);
	return 0;
}

void fprop_local_destroy_single(struct fprop_local_single *pl)
{
}

static void fprop_reflect_period_single(struct fprop_global *p,
					struct fprop_local_single *pl)
{
	unsigned int period = p->period;
	unsigned long flags;

	/*                                  */
	if (pl->period == period)
		return;
	raw_spin_lock_irqsave(&pl->lock, flags);
	/*                                                    */
	if (pl->period >= period) {
		raw_spin_unlock_irqrestore(&pl->lock, flags);
		return;
	}
	/*                            */
	if (period - pl->period < BITS_PER_LONG)
		pl->events >>= period - pl->period;
	else
		pl->events = 0;
	pl->period = period;
	raw_spin_unlock_irqrestore(&pl->lock, flags);
}

/*                           */
void __fprop_inc_single(struct fprop_global *p, struct fprop_local_single *pl)
{
	fprop_reflect_period_single(p, pl);
	pl->events++;
	percpu_counter_add(&p->events, 1);
}

/*                                      */
void fprop_fraction_single(struct fprop_global *p,
			   struct fprop_local_single *pl,
			   unsigned long *numerator, unsigned long *denominator)
{
	unsigned int seq;
	s64 num, den;

	do {
		seq = read_seqcount_begin(&p->sequence);
		fprop_reflect_period_single(p, pl);
		num = pl->events;
		den = percpu_counter_read_positive(&p->events);
	} while (read_seqcount_retry(&p->sequence, seq));

	/*
                                                                     
                  
  */
	if (den <= num) {
		if (num)
			den = num;
		else
			den = 1;
	}
	*denominator = den;
	*numerator = num;
}

/*
                   
 */
#define PROP_BATCH (8*(1+ilog2(nr_cpu_ids)))

int fprop_local_init_percpu(struct fprop_local_percpu *pl)
{
	int err;

	err = percpu_counter_init(&pl->events, 0);
	if (err)
		return err;
	pl->period = 0;
	raw_spin_lock_init(&pl->lock);
	return 0;
}

void fprop_local_destroy_percpu(struct fprop_local_percpu *pl)
{
	percpu_counter_destroy(&pl->events);
}

static void fprop_reflect_period_percpu(struct fprop_global *p,
					struct fprop_local_percpu *pl)
{
	unsigned int period = p->period;
	unsigned long flags;

	/*                                  */
	if (pl->period == period)
		return;
	raw_spin_lock_irqsave(&pl->lock, flags);
	/*                                                    */
	if (pl->period >= period) {
		raw_spin_unlock_irqrestore(&pl->lock, flags);
		return;
	}
	/*                            */
	if (period - pl->period < BITS_PER_LONG) {
		s64 val = percpu_counter_read(&pl->events);

		if (val < (nr_cpu_ids * PROP_BATCH))
			val = percpu_counter_sum(&pl->events);

		__percpu_counter_add(&pl->events,
			-val + (val >> (period-pl->period)), PROP_BATCH);
	} else
		percpu_counter_set(&pl->events, 0);
	pl->period = period;
	raw_spin_unlock_irqrestore(&pl->lock, flags);
}

/*                           */
void __fprop_inc_percpu(struct fprop_global *p, struct fprop_local_percpu *pl)
{
	fprop_reflect_period_percpu(p, pl);
	__percpu_counter_add(&pl->events, 1, PROP_BATCH);
	percpu_counter_add(&p->events, 1);
}

void fprop_fraction_percpu(struct fprop_global *p,
			   struct fprop_local_percpu *pl,
			   unsigned long *numerator, unsigned long *denominator)
{
	unsigned int seq;
	s64 num, den;

	do {
		seq = read_seqcount_begin(&p->sequence);
		fprop_reflect_period_percpu(p, pl);
		num = percpu_counter_read_positive(&pl->events);
		den = percpu_counter_read_positive(&p->events);
	} while (read_seqcount_retry(&p->sequence, seq));

	/*
                                                                     
                  
  */
	if (den <= num) {
		if (num)
			den = num;
		else
			den = 1;
	}
	*denominator = den;
	*numerator = num;
}

/*
                                                                           
                                                           
 */
void __fprop_inc_percpu_max(struct fprop_global *p,
			    struct fprop_local_percpu *pl, int max_frac)
{
	if (unlikely(max_frac < FPROP_FRAC_BASE)) {
		unsigned long numerator, denominator;

		fprop_fraction_percpu(p, pl, &numerator, &denominator);
		if (numerator >
		    (((u64)denominator) * max_frac) >> FPROP_FRAC_SHIFT)
			return;
	} else
		fprop_reflect_period_percpu(p, pl);
	__percpu_counter_add(&pl->events, 1, PROP_BATCH);
	percpu_counter_add(&p->events, 1);
}
