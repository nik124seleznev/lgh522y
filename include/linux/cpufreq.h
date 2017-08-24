/*
 *  linux/include/linux/cpufreq.h
 *
 *  Copyright (C) 2001 Russell King
 *            (C) 2002 - 2003 Dominik Brodowski <linux@brodo.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef _LINUX_CPUFREQ_H
#define _LINUX_CPUFREQ_H

#include <asm/cputime.h>
#include <linux/mutex.h>
#include <linux/notifier.h>
#include <linux/threads.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/completion.h>
#include <linux/workqueue.h>
#include <linux/cpumask.h>
#include <asm/div64.h>

#define CPUFREQ_NAME_LEN 16
/*                                                                       */
#define CPUFREQ_NAME_PLEN (CPUFREQ_NAME_LEN + 1)


/*                                                                    
                                                                      
                                                                     */

#define CPUFREQ_TRANSITION_NOTIFIER	(0)
#define CPUFREQ_POLICY_NOTIFIER		(1)

#ifdef CONFIG_CPU_FREQ
int cpufreq_register_notifier(struct notifier_block *nb, unsigned int list);
int cpufreq_unregister_notifier(struct notifier_block *nb, unsigned int list);
extern void disable_cpufreq(void);
#else		/*                 */
static inline int cpufreq_register_notifier(struct notifier_block *nb,
						unsigned int list)
{
	return 0;
}
static inline int cpufreq_unregister_notifier(struct notifier_block *nb,
						unsigned int list)
{
	return 0;
}
static inline void disable_cpufreq(void) { }
#endif		/*                 */

/*                                                                          
                                                                          
                                      
 */

#define CPUFREQ_POLICY_POWERSAVE	(1)
#define CPUFREQ_POLICY_PERFORMANCE	(2)

/*                                                                     
                                                                      
                          
                                                                  
                                 
 */

struct cpufreq_governor;

/*                                                                   */
extern struct kobject *cpufreq_global_kobject;

#define CPUFREQ_ETERNAL			(-1)
struct cpufreq_cpuinfo {
	unsigned int		max_freq;
	unsigned int		min_freq;

	/*                            */
	unsigned int		transition_latency;
};

struct cpufreq_real_policy {
	unsigned int		min;    /*        */
	unsigned int		max;    /*        */
	unsigned int		policy; /*           */
	struct cpufreq_governor	*governor; /*           */
};

struct cpufreq_policy {
	/*                                             */
	cpumask_var_t		cpus;	/*                  */
	cpumask_var_t		related_cpus; /*                       */

	unsigned int		shared_type; /*                               
                         */
	unsigned int		cpu;    /*                                    */
	unsigned int		last_cpu; /*                                    
                      */
	struct cpufreq_cpuinfo	cpuinfo;/*           */

	unsigned int		min;    /*        */
	unsigned int		max;    /*        */
	unsigned int		cur;    /*                               
                           */
	unsigned int		policy; /*           */
	struct cpufreq_governor	*governor; /*           */
	void			*governor_data;
	bool			governor_enabled; /*                          */

	struct work_struct	update; /*                               
                                          */

	struct cpufreq_real_policy	user_policy;

	struct kobject		kobj;
	struct completion	kobj_unregister;
};

#define CPUFREQ_ADJUST			(0)
#define CPUFREQ_INCOMPATIBLE		(1)
#define CPUFREQ_NOTIFY			(2)
#define CPUFREQ_START			(3)
#define CPUFREQ_UPDATE_POLICY_CPU	(4)

/*               */
#define CPUFREQ_SHARED_TYPE_NONE (0) /*      */
#define CPUFREQ_SHARED_TYPE_HW	 (1) /*                             */
#define CPUFREQ_SHARED_TYPE_ALL	 (2) /*                                    */
#define CPUFREQ_SHARED_TYPE_ANY	 (3) /*                                       */

static inline bool policy_is_shared(struct cpufreq_policy *policy)
{
	return cpumask_weight(policy->cpus) > 1;
}

/*                                                                   */

#define CPUFREQ_PRECHANGE	(0)
#define CPUFREQ_POSTCHANGE	(1)
#define CPUFREQ_RESUMECHANGE	(8)
#define CPUFREQ_SUSPENDCHANGE	(9)

struct cpufreq_freqs {
	unsigned int cpu;	/*        */
	unsigned int old;
	unsigned int new;
	u8 flags;		/*                                     */
};


/* 
                                                                                     
                    
                  
                     
  
  
                            
 */
static inline unsigned long cpufreq_scale(unsigned long old, u_int div, u_int mult)
{
#if BITS_PER_LONG == 32

	u64 result = ((u64) old) * ((u64) mult);
	do_div(result, div);
	return (unsigned long) result;

#elif BITS_PER_LONG == 64

	unsigned long result = old * ((u64) mult);
	result /= div;
	return result;

#endif
};

/*                                                                    
                                                                      
                                                                     */

#define CPUFREQ_GOV_START	1
#define CPUFREQ_GOV_STOP	2
#define CPUFREQ_GOV_LIMITS	3
#define CPUFREQ_GOV_POLICY_INIT	4
#define CPUFREQ_GOV_POLICY_EXIT	5

struct cpufreq_governor {
	char	name[CPUFREQ_NAME_LEN];
	int	initialized;
	int	(*governor)	(struct cpufreq_policy *policy,
				 unsigned int event);
	ssize_t	(*show_setspeed)	(struct cpufreq_policy *policy,
					 char *buf);
	int	(*store_setspeed)	(struct cpufreq_policy *policy,
					 unsigned int freq);
	unsigned int max_transition_latency; /*                             
                                                      
                                         */
	struct list_head	governor_list;
	struct module		*owner;
};

/*
                                       
 */
extern int cpufreq_driver_target(struct cpufreq_policy *policy,
				 unsigned int target_freq,
				 unsigned int relation);
extern int __cpufreq_driver_target(struct cpufreq_policy *policy,
				   unsigned int target_freq,
				   unsigned int relation);


extern int __cpufreq_driver_getavg(struct cpufreq_policy *policy,
				   unsigned int cpu);

int cpufreq_register_governor(struct cpufreq_governor *governor);
void cpufreq_unregister_governor(struct cpufreq_governor *governor);


/*                                                                    
                                                                      
                                                                     */

#define CPUFREQ_RELATION_L 0  /*                                     */
#define CPUFREQ_RELATION_H 1  /*                                      */

struct freq_attr;

struct cpufreq_driver {
	struct module           *owner;
	char			name[CPUFREQ_NAME_LEN];
	u8			flags;
	/*
                                                                       
                                                                         
                                                                       
                                                                         
  */
	bool			have_governor_per_policy;

	/*                       */
	int	(*init)		(struct cpufreq_policy *policy);
	int	(*verify)	(struct cpufreq_policy *policy);

	/*                       */
	int	(*setpolicy)	(struct cpufreq_policy *policy);
	int	(*target)	(struct cpufreq_policy *policy,
				 unsigned int target_freq,
				 unsigned int relation);

	/*                                */
	unsigned int	(*get)	(unsigned int cpu);

	/*          */
	unsigned int (*getavg)	(struct cpufreq_policy *policy,
				 unsigned int cpu);
	int	(*bios_limit)	(int cpu, unsigned int *limit);

	int	(*exit)		(struct cpufreq_policy *policy);
	int	(*suspend)	(struct cpufreq_policy *policy);
	int	(*resume)	(struct cpufreq_policy *policy);
	struct freq_attr	**attr;
};

/*       */

#define CPUFREQ_STICKY		0x01	/*                                 
                                  */
#define CPUFREQ_CONST_LOOPS	0x02	/*                                
                                      
                              */
#define CPUFREQ_PM_NO_WARN	0x04	/*                                   
                   */

int cpufreq_register_driver(struct cpufreq_driver *driver_data);
int cpufreq_unregister_driver(struct cpufreq_driver *driver_data);


void cpufreq_notify_transition(struct cpufreq_policy *policy,
		struct cpufreq_freqs *freqs, unsigned int state);

static inline void cpufreq_verify_within_limits(struct cpufreq_policy *policy, unsigned int min, unsigned int max)
{
	if (policy->min < min)
		policy->min = min;
	if (policy->max < min)
		policy->max = min;
	if (policy->min > max)
		policy->min = max;
	if (policy->max > max)
		policy->max = max;
	if (policy->min > policy->max)
		policy->min = policy->max;
	return;
}

struct freq_attr {
	struct attribute attr;
	ssize_t (*show)(struct cpufreq_policy *, char *);
	ssize_t (*store)(struct cpufreq_policy *, const char *, size_t count);
};

#define cpufreq_freq_attr_ro(_name)		\
static struct freq_attr _name =			\
__ATTR(_name, 0444, show_##_name, NULL)

#define cpufreq_freq_attr_ro_perm(_name, _perm)	\
static struct freq_attr _name =			\
__ATTR(_name, _perm, show_##_name, NULL)

#define cpufreq_freq_attr_rw(_name)		\
static struct freq_attr _name =			\
__ATTR(_name, 0644, show_##_name, store_##_name)

struct global_attr {
	struct attribute attr;
	ssize_t (*show)(struct kobject *kobj,
			struct attribute *attr, char *buf);
	ssize_t (*store)(struct kobject *a, struct attribute *b,
			 const char *c, size_t count);
};

#define define_one_global_ro(_name)		\
static struct global_attr _name =		\
__ATTR(_name, 0444, show_##_name, NULL)

#define define_one_global_rw(_name)		\
static struct global_attr _name =		\
__ATTR(_name, 0644, show_##_name, store_##_name)

struct cpufreq_policy *cpufreq_cpu_get(unsigned int cpu);
void cpufreq_cpu_put(struct cpufreq_policy *data);
const char *cpufreq_get_current_driver(void);

/*                                                                    
                                                                      
                                                                     */
u64 get_cpu_idle_time(unsigned int cpu, u64 *wall, int io_busy);
int cpufreq_get_policy(struct cpufreq_policy *policy, unsigned int cpu);
int cpufreq_update_policy(unsigned int cpu);
bool have_governor_per_policy(void);
struct kobject *get_governor_parent_kobj(struct cpufreq_policy *policy);

#ifdef CONFIG_CPU_FREQ
/*                                                                               */
unsigned int cpufreq_get(unsigned int cpu);
#else
static inline unsigned int cpufreq_get(unsigned int cpu)
{
	return 0;
}
#endif

/*                                                                             */
#ifdef CONFIG_CPU_FREQ
unsigned int cpufreq_quick_get(unsigned int cpu);
unsigned int cpufreq_quick_get_max(unsigned int cpu);
#else
static inline unsigned int cpufreq_quick_get(unsigned int cpu)
{
	return 0;
}
static inline unsigned int cpufreq_quick_get_max(unsigned int cpu)
{
	return 0;
}
#endif


/*                                                                    
                                                                      
                                                                     */


/*
                                                                      
                                    
*/
#ifdef CONFIG_CPU_FREQ_GOV_PERFORMANCE
extern struct cpufreq_governor cpufreq_gov_performance;
#endif
#ifdef CONFIG_CPU_FREQ_DEFAULT_GOV_PERFORMANCE
#define CPUFREQ_DEFAULT_GOVERNOR	(&cpufreq_gov_performance)
#elif defined(CONFIG_CPU_FREQ_DEFAULT_GOV_POWERSAVE)
extern struct cpufreq_governor cpufreq_gov_powersave;
#define CPUFREQ_DEFAULT_GOVERNOR	(&cpufreq_gov_powersave)
#elif defined(CONFIG_CPU_FREQ_DEFAULT_GOV_USERSPACE)
extern struct cpufreq_governor cpufreq_gov_userspace;
#define CPUFREQ_DEFAULT_GOVERNOR	(&cpufreq_gov_userspace)
#elif defined(CONFIG_CPU_FREQ_DEFAULT_GOV_ONDEMAND)
extern struct cpufreq_governor cpufreq_gov_ondemand;
#define CPUFREQ_DEFAULT_GOVERNOR	(&cpufreq_gov_ondemand)
#elif defined(CONFIG_CPU_FREQ_DEFAULT_GOV_CONSERVATIVE)
extern struct cpufreq_governor cpufreq_gov_conservative;
#define CPUFREQ_DEFAULT_GOVERNOR	(&cpufreq_gov_conservative)
#elif defined(CONFIG_CPU_FREQ_DEFAULT_GOV_HOTPLUG)
extern struct cpufreq_governor cpufreq_gov_hotplug;
#define CPUFREQ_DEFAULT_GOVERNOR	(&cpufreq_gov_hotplug)
#elif defined(CONFIG_CPU_FREQ_DEFAULT_GOV_BALANCE)
extern struct cpufreq_governor cpufreq_gov_balance;
#define CPUFREQ_DEFAULT_GOVERNOR	(&cpufreq_gov_balance)
#elif defined(CONFIG_CPU_FREQ_DEFAULT_GOV_INTERACTIVE)
extern struct cpufreq_governor cpufreq_gov_interactive;
#define CPUFREQ_DEFAULT_GOVERNOR	(&cpufreq_gov_interactive)
#elif defined(CONFIG_CPU_FREQ_DEFAULT_GOV_INTERACTIVEPLUS)
extern struct cpufreq_governor cpufreq_gov_interactiveplus;
#define CPUFREQ_DEFAULT_GOVERNOR	(&cpufreq_gov_interactiveplus)
#endif


/*                                                                    
                                                                      
                                                                     */

#define CPUFREQ_ENTRY_INVALID ~0
#define CPUFREQ_TABLE_END     ~1

struct cpufreq_frequency_table {
	unsigned int	index;     /*     */
	unsigned int	frequency; /*                                      
                */
};

int cpufreq_frequency_table_cpuinfo(struct cpufreq_policy *policy,
				    struct cpufreq_frequency_table *table);

int cpufreq_frequency_table_verify(struct cpufreq_policy *policy,
				   struct cpufreq_frequency_table *table);

int cpufreq_frequency_table_target(struct cpufreq_policy *policy,
				   struct cpufreq_frequency_table *table,
				   unsigned int target_freq,
				   unsigned int relation,
				   unsigned int *index);

/*                                                        */
struct cpufreq_frequency_table *cpufreq_frequency_get_table(unsigned int cpu);

/*                                          */
extern struct freq_attr cpufreq_freq_attr_scaling_available_freqs;

void cpufreq_frequency_table_get_attr(struct cpufreq_frequency_table *table,
				      unsigned int cpu);
void cpufreq_frequency_table_update_policy_cpu(struct cpufreq_policy *policy);

void cpufreq_frequency_table_put_attr(unsigned int cpu);
#endif /*                  */
