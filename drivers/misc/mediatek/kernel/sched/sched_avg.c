/* Copyright (c) 2012, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/*
                                                    
 */
#include <linux/module.h>
#include <linux/percpu.h>
#include <linux/hrtimer.h>
#include <linux/sched.h>
#include <linux/math64.h>

static DEFINE_PER_CPU(u64, nr_prod_sum);
static DEFINE_PER_CPU(u64, last_time);
static DEFINE_PER_CPU(u64, nr);
static DEFINE_PER_CPU(unsigned long, iowait_prod_sum);
static DEFINE_PER_CPU(spinlock_t, nr_lock) = __SPIN_LOCK_UNLOCKED(nr_lock);
static u64 last_get_time;

/* 
                           
                                                                
                                                               
                   
  
                                                            
                                                           
 */
void sched_get_nr_running_avg(int *avg, int *iowait_avg)
{
	int cpu;
	u64 curr_time = sched_clock();
	s64 diff = (s64) (curr_time - last_get_time);
	u64 tmp_avg = 0, tmp_iowait = 0, old_lgt;
	bool clk_faulty = 0;
	u32 cpumask = 0;

	*avg = 0;
	*iowait_avg = 0;

	if (!diff)
		return;
    WARN(diff<0, "[sched_get_nr_running_avg] time last:%llu curr:%llu ",
         last_get_time, curr_time);

	old_lgt = last_get_time;
	last_get_time = curr_time;
	/*                                  */
	for_each_possible_cpu(cpu) {
		unsigned long flags;

		spin_lock_irqsave(&per_cpu(nr_lock, cpu), flags);
		/*                                                */
		if ((s64) (curr_time - per_cpu(last_time, cpu) < 0)) {
			clk_faulty = 1;
			cpumask |= 1 << cpu;
		}
		/*        */
		tmp_avg += per_cpu(nr_prod_sum, cpu);
		tmp_avg += per_cpu(nr, cpu) * (curr_time - per_cpu(last_time, cpu));
		tmp_iowait = per_cpu(iowait_prod_sum, cpu);
		tmp_iowait += nr_iowait_cpu(cpu) * (curr_time - per_cpu(last_time, cpu));
		per_cpu(last_time, cpu) = curr_time;
		per_cpu(nr_prod_sum, cpu) = 0;
		per_cpu(iowait_prod_sum, cpu) = 0;
		spin_unlock_irqrestore(&per_cpu(nr_lock, cpu), flags);
	}

	/*                                                */
	if (clk_faulty) {
		*avg = 0;
		*iowait_avg = 0;
		pr_warn("[%s] **** CPU (0x%08x)clock may unstable !!\n", __func__, cpumask);
		return;
	}
	/*       */

	*avg = (int)div64_u64(tmp_avg * 100, (u64) diff);
	*iowait_avg = (int)div64_u64(tmp_iowait * 100, (u64) diff);

    WARN(*avg<0, "[sched_get_nr_running_avg] avg:%d(%llu/%lld), time last:%llu curr:%llu ",
         *avg, tmp_avg, diff, old_lgt, curr_time);
	if (unlikely(*avg<0))
		*avg = 0;
    WARN(*iowait_avg<0, "[sched_get_nr_running_avg] iowait_avg:%d(%llu/%lld) time last:%llu curr:%llu ",
         *iowait_avg, tmp_iowait, diff, old_lgt, curr_time);
	if (unlikely(*iowait_avg<0))
		*iowait_avg = 0;
}
EXPORT_SYMBOL(sched_get_nr_running_avg);

/* 
                       
                                              
                                         
                                                          
               
  
                                                      
 */
void sched_update_nr_prod(int cpu, unsigned long nr_running, bool inc)
{
	s64 diff;
	u64 curr_time;
	unsigned long flags;

	spin_lock_irqsave(&per_cpu(nr_lock, cpu), flags);
	curr_time = sched_clock();
	diff = (s64) (curr_time - per_cpu(last_time, cpu));
	/*                                       */
	if (diff < 0) {
		spin_unlock_irqrestore(&per_cpu(nr_lock, cpu), flags);
		return;
	}
	/*                                        */

	per_cpu(last_time, cpu) = curr_time;
	per_cpu(nr, cpu) = nr_running + (inc ? 1 : -1);

	BUG_ON(per_cpu(nr, cpu) < 0);

	per_cpu(nr_prod_sum, cpu) += nr_running * diff;
	per_cpu(iowait_prod_sum, cpu) += nr_iowait_cpu(cpu) * diff;
	spin_unlock_irqrestore(&per_cpu(nr_lock, cpu), flags);
}
EXPORT_SYMBOL(sched_update_nr_prod);
