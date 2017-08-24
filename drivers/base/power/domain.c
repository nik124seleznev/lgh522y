/*
 * drivers/base/power/domain.c - Common code related to device power domains.
 *
 * Copyright (C) 2011 Rafael J. Wysocki <rjw@sisk.pl>, Renesas Electronics Corp.
 *
 * This file is released under the GPLv2.
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/io.h>
#include <linux/pm_runtime.h>
#include <linux/pm_domain.h>
#include <linux/pm_qos.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/sched.h>
#include <linux/suspend.h>
#include <linux/export.h>

#define GENPD_DEV_CALLBACK(genpd, type, callback, dev)		\
({								\
	type (*__routine)(struct device *__d); 			\
	type __ret = (type)0;					\
								\
	__routine = genpd->dev_ops.callback; 			\
	if (__routine) {					\
		__ret = __routine(dev); 			\
	} else {						\
		__routine = dev_gpd_data(dev)->ops.callback;	\
		if (__routine) 					\
			__ret = __routine(dev);			\
	}							\
	__ret;							\
})

#define GENPD_DEV_TIMED_CALLBACK(genpd, type, callback, dev, field, name)	\
({										\
	ktime_t __start = ktime_get();						\
	type __retval = GENPD_DEV_CALLBACK(genpd, type, callback, dev);		\
	s64 __elapsed = ktime_to_ns(ktime_sub(ktime_get(), __start));		\
	struct gpd_timing_data *__td = &dev_gpd_data(dev)->td;			\
	if (!__retval && __elapsed > __td->field) {				\
		__td->field = __elapsed;					\
		dev_warn(dev, name " latency exceeded, new value %lld ns\n",	\
			__elapsed);						\
		genpd->max_off_time_changed = true;				\
		__td->constraint_changed = true;				\
	}									\
	__retval;								\
})

static LIST_HEAD(gpd_list);
static DEFINE_MUTEX(gpd_list_lock);

static struct generic_pm_domain *pm_genpd_lookup_name(const char *domain_name)
{
	struct generic_pm_domain *genpd = NULL, *gpd;

	if (IS_ERR_OR_NULL(domain_name))
		return NULL;

	mutex_lock(&gpd_list_lock);
	list_for_each_entry(gpd, &gpd_list, gpd_list_node) {
		if (!strcmp(gpd->name, domain_name)) {
			genpd = gpd;
			break;
		}
	}
	mutex_unlock(&gpd_list_lock);
	return genpd;
}

#ifdef CONFIG_PM

struct generic_pm_domain *dev_to_genpd(struct device *dev)
{
	if (IS_ERR_OR_NULL(dev->pm_domain))
		return ERR_PTR(-EINVAL);

	return pd_to_genpd(dev->pm_domain);
}

static int genpd_stop_dev(struct generic_pm_domain *genpd, struct device *dev)
{
	return GENPD_DEV_TIMED_CALLBACK(genpd, int, stop, dev,
					stop_latency_ns, "stop");
}

static int genpd_start_dev(struct generic_pm_domain *genpd, struct device *dev)
{
	return GENPD_DEV_TIMED_CALLBACK(genpd, int, start, dev,
					start_latency_ns, "start");
}

static bool genpd_sd_counter_dec(struct generic_pm_domain *genpd)
{
	bool ret = false;

	if (!WARN_ON(atomic_read(&genpd->sd_count) == 0))
		ret = !!atomic_dec_and_test(&genpd->sd_count);

	return ret;
}

static void genpd_sd_counter_inc(struct generic_pm_domain *genpd)
{
	atomic_inc(&genpd->sd_count);
	smp_mb__after_atomic_inc();
}

static void genpd_acquire_lock(struct generic_pm_domain *genpd)
{
	DEFINE_WAIT(wait);

	mutex_lock(&genpd->lock);
	/*
                                                             
                           
  */
	for (;;) {
		prepare_to_wait(&genpd->status_wait_queue, &wait,
				TASK_UNINTERRUPTIBLE);
		if (genpd->status == GPD_STATE_ACTIVE
		    || genpd->status == GPD_STATE_POWER_OFF)
			break;
		mutex_unlock(&genpd->lock);

		schedule();

		mutex_lock(&genpd->lock);
	}
	finish_wait(&genpd->status_wait_queue, &wait);
}

static void genpd_release_lock(struct generic_pm_domain *genpd)
{
	mutex_unlock(&genpd->lock);
}

static void genpd_set_active(struct generic_pm_domain *genpd)
{
	if (genpd->resume_count == 0)
		genpd->status = GPD_STATE_ACTIVE;
}

static void genpd_recalc_cpu_exit_latency(struct generic_pm_domain *genpd)
{
	s64 usecs64;

	if (!genpd->cpu_data)
		return;

	usecs64 = genpd->power_on_latency_ns;
	do_div(usecs64, NSEC_PER_USEC);
	usecs64 += genpd->cpu_data->saved_exit_latency;
	genpd->cpu_data->idle_state->exit_latency = usecs64;
}

/* 
                                                                           
                                 
  
                                                                           
                                   
 */
static int __pm_genpd_poweron(struct generic_pm_domain *genpd)
	__releases(&genpd->lock) __acquires(&genpd->lock)
{
	struct gpd_link *link;
	DEFINE_WAIT(wait);
	int ret = 0;

	/*                                                                  */
	for (;;) {
		prepare_to_wait(&genpd->status_wait_queue, &wait,
				TASK_UNINTERRUPTIBLE);
		if (genpd->status != GPD_STATE_WAIT_MASTER)
			break;
		mutex_unlock(&genpd->lock);

		schedule();

		mutex_lock(&genpd->lock);
	}
	finish_wait(&genpd->status_wait_queue, &wait);

	if (genpd->status == GPD_STATE_ACTIVE
	    || (genpd->prepared_count > 0 && genpd->suspend_power_off))
		return 0;

	if (genpd->status != GPD_STATE_POWER_OFF) {
		genpd_set_active(genpd);
		return 0;
	}

	if (genpd->cpu_data) {
		cpuidle_pause_and_lock();
		genpd->cpu_data->idle_state->disabled = true;
		cpuidle_resume_and_unlock();
		goto out;
	}

	/*
                                                                      
                                                                      
            
  */
	list_for_each_entry(link, &genpd->slave_links, slave_node) {
		genpd_sd_counter_inc(link->master);
		genpd->status = GPD_STATE_WAIT_MASTER;

		mutex_unlock(&genpd->lock);

		ret = pm_genpd_poweron(link->master);

		mutex_lock(&genpd->lock);

		/*
                                                             
                                     
   */
		genpd->status = GPD_STATE_POWER_OFF;
		wake_up_all(&genpd->status_wait_queue);
		if (ret) {
			genpd_sd_counter_dec(link->master);
			goto err;
		}
	}

	if (genpd->power_on) {
		ktime_t time_start = ktime_get();
		s64 elapsed_ns;

		ret = genpd->power_on(genpd);
		if (ret)
			goto err;

		elapsed_ns = ktime_to_ns(ktime_sub(ktime_get(), time_start));
		if (elapsed_ns > genpd->power_on_latency_ns) {
			genpd->power_on_latency_ns = elapsed_ns;
			genpd->max_off_time_changed = true;
			genpd_recalc_cpu_exit_latency(genpd);
			if (genpd->name)
				pr_warning("%s: Power-on latency exceeded, "
					"new value %lld ns\n", genpd->name,
					elapsed_ns);
		}
	}

 out:
	genpd_set_active(genpd);

	return 0;

 err:
	list_for_each_entry_continue_reverse(link, &genpd->slave_links, slave_node)
		genpd_sd_counter_dec(link->master);

	return ret;
}

/* 
                                                                         
                                 
 */
int pm_genpd_poweron(struct generic_pm_domain *genpd)
{
	int ret;

	mutex_lock(&genpd->lock);
	ret = __pm_genpd_poweron(genpd);
	mutex_unlock(&genpd->lock);
	return ret;
}

/* 
                                                                              
                                                   
 */
int pm_genpd_name_poweron(const char *domain_name)
{
	struct generic_pm_domain *genpd;

	genpd = pm_genpd_lookup_name(domain_name);
	return genpd ? pm_genpd_poweron(genpd) : -EINVAL;
}

#endif /*           */

#ifdef CONFIG_PM_RUNTIME

static int genpd_start_dev_no_timing(struct generic_pm_domain *genpd,
				     struct device *dev)
{
	return GENPD_DEV_CALLBACK(genpd, int, start, dev);
}

static int genpd_save_dev(struct generic_pm_domain *genpd, struct device *dev)
{
	return GENPD_DEV_TIMED_CALLBACK(genpd, int, save_state, dev,
					save_state_latency_ns, "state save");
}

static int genpd_restore_dev(struct generic_pm_domain *genpd, struct device *dev)
{
	return GENPD_DEV_TIMED_CALLBACK(genpd, int, restore_state, dev,
					restore_state_latency_ns,
					"state restore");
}

static int genpd_dev_pm_qos_notifier(struct notifier_block *nb,
				     unsigned long val, void *ptr)
{
	struct generic_pm_domain_data *gpd_data;
	struct device *dev;

	gpd_data = container_of(nb, struct generic_pm_domain_data, nb);

	mutex_lock(&gpd_data->lock);
	dev = gpd_data->base.dev;
	if (!dev) {
		mutex_unlock(&gpd_data->lock);
		return NOTIFY_DONE;
	}
	mutex_unlock(&gpd_data->lock);

	for (;;) {
		struct generic_pm_domain *genpd;
		struct pm_domain_data *pdd;

		spin_lock_irq(&dev->power.lock);

		pdd = dev->power.subsys_data ?
				dev->power.subsys_data->domain_data : NULL;
		if (pdd && pdd->dev) {
			to_gpd_data(pdd)->td.constraint_changed = true;
			genpd = dev_to_genpd(dev);
		} else {
			genpd = ERR_PTR(-ENODATA);
		}

		spin_unlock_irq(&dev->power.lock);

		if (!IS_ERR(genpd)) {
			mutex_lock(&genpd->lock);
			genpd->max_off_time_changed = true;
			mutex_unlock(&genpd->lock);
		}

		dev = dev->parent;
		if (!dev || dev->power.ignore_children)
			break;
	}

	return NOTIFY_DONE;
}

/* 
                                                                   
                                                        
                                           
 */
static int __pm_genpd_save_device(struct pm_domain_data *pdd,
				  struct generic_pm_domain *genpd)
	__releases(&genpd->lock) __acquires(&genpd->lock)
{
	struct generic_pm_domain_data *gpd_data = to_gpd_data(pdd);
	struct device *dev = pdd->dev;
	int ret = 0;

	if (gpd_data->need_restore)
		return 0;

	mutex_unlock(&genpd->lock);

	genpd_start_dev(genpd, dev);
	ret = genpd_save_dev(genpd, dev);
	genpd_stop_dev(genpd, dev);

	mutex_lock(&genpd->lock);

	if (!ret)
		gpd_data->need_restore = true;

	return ret;
}

/* 
                                                                         
                                                           
                                           
 */
static void __pm_genpd_restore_device(struct pm_domain_data *pdd,
				      struct generic_pm_domain *genpd)
	__releases(&genpd->lock) __acquires(&genpd->lock)
{
	struct generic_pm_domain_data *gpd_data = to_gpd_data(pdd);
	struct device *dev = pdd->dev;
	bool need_restore = gpd_data->need_restore;

	gpd_data->need_restore = false;
	mutex_unlock(&genpd->lock);

	genpd_start_dev(genpd, dev);
	if (need_restore)
		genpd_restore_dev(genpd, dev);

	mutex_lock(&genpd->lock);
}

/* 
                                                                           
                              
  
                                                                         
                                                                            
                                                                             
                                                            
 */
static bool genpd_abort_poweroff(struct generic_pm_domain *genpd)
{
	return genpd->status == GPD_STATE_WAIT_MASTER
		|| genpd->status == GPD_STATE_ACTIVE || genpd->resume_count > 0;
}

/* 
                                                                              
                                  
  
                                                                              
          
 */
void genpd_queue_power_off_work(struct generic_pm_domain *genpd)
{
	queue_work(pm_wq, &genpd->power_off_work);
}

/* 
                                                           
                                   
  
                                                                               
                                                                               
                                                              
 */
static int pm_genpd_poweroff(struct generic_pm_domain *genpd)
	__releases(&genpd->lock) __acquires(&genpd->lock)
{
	struct pm_domain_data *pdd;
	struct gpd_link *link;
	unsigned int not_suspended;
	int ret = 0;

 start:
	/*
                                                                   
                                                       
                                                         
                                                               
                                      
  */
	if (genpd->status == GPD_STATE_POWER_OFF
	    || genpd->status == GPD_STATE_WAIT_MASTER
	    || genpd->resume_count > 0 || genpd->prepared_count > 0)
		return 0;

	if (atomic_read(&genpd->sd_count) > 0)
		return -EBUSY;

	not_suspended = 0;
	list_for_each_entry(pdd, &genpd->dev_list, list_node) {
		enum pm_qos_flags_status stat;

		stat = dev_pm_qos_flags(pdd->dev,
					PM_QOS_FLAG_NO_POWER_OFF
						| PM_QOS_FLAG_REMOTE_WAKEUP);
		if (stat > PM_QOS_FLAGS_NONE)
			return -EBUSY;

		if (pdd->dev->driver && (!pm_runtime_suspended(pdd->dev)
		    || pdd->dev->power.irq_safe))
			not_suspended++;
	}

	if (not_suspended > genpd->in_progress)
		return -EBUSY;

	if (genpd->poweroff_task) {
		/*
                                                         
                                                    
   */
		genpd->status = GPD_STATE_REPEAT;
		return 0;
	}

	if (genpd->gov && genpd->gov->power_down_ok) {
		if (!genpd->gov->power_down_ok(&genpd->domain))
			return -EAGAIN;
	}

	genpd->status = GPD_STATE_BUSY;
	genpd->poweroff_task = current;

	list_for_each_entry_reverse(pdd, &genpd->dev_list, list_node) {
		ret = atomic_read(&genpd->sd_count) == 0 ?
			__pm_genpd_save_device(pdd, genpd) : -EBUSY;

		if (genpd_abort_poweroff(genpd))
			goto out;

		if (ret) {
			genpd_set_active(genpd);
			goto out;
		}

		if (genpd->status == GPD_STATE_REPEAT) {
			genpd->poweroff_task = NULL;
			goto start;
		}
	}

	if (genpd->cpu_data) {
		/*
                                                                
                                                                
                                                                 
                                                               
                                    
   */
		genpd->status = GPD_STATE_POWER_OFF;
		cpuidle_pause_and_lock();
		genpd->cpu_data->idle_state->disabled = false;
		cpuidle_resume_and_unlock();
		goto out;
	}

	if (genpd->power_off) {
		ktime_t time_start;
		s64 elapsed_ns;

		if (atomic_read(&genpd->sd_count) > 0) {
			ret = -EBUSY;
			goto out;
		}

		time_start = ktime_get();

		/*
                                                                
                                                                
                                                                
                                                                 
                                                                
                        
   */
		ret = genpd->power_off(genpd);
		if (ret == -EBUSY) {
			genpd_set_active(genpd);
			goto out;
		}

		elapsed_ns = ktime_to_ns(ktime_sub(ktime_get(), time_start));
		if (elapsed_ns > genpd->power_off_latency_ns) {
			genpd->power_off_latency_ns = elapsed_ns;
			genpd->max_off_time_changed = true;
			if (genpd->name)
				pr_warning("%s: Power-off latency exceeded, "
					"new value %lld ns\n", genpd->name,
					elapsed_ns);
		}
	}

	genpd->status = GPD_STATE_POWER_OFF;

	list_for_each_entry(link, &genpd->slave_links, slave_node) {
		genpd_sd_counter_dec(link->master);
		genpd_queue_power_off_work(link->master);
	}

 out:
	genpd->poweroff_task = NULL;
	wake_up_all(&genpd->status_wait_queue);
	return ret;
}

/* 
                                                                            
                                                                            
 */
static void genpd_power_off_work_fn(struct work_struct *work)
{
	struct generic_pm_domain *genpd;

	genpd = container_of(work, struct generic_pm_domain, power_off_work);

	genpd_acquire_lock(genpd);
	pm_genpd_poweroff(genpd);
	genpd_release_lock(genpd);
}

/* 
                                                                          
                           
  
                                                                        
                                                                   
                                                                               
 */
static int pm_genpd_runtime_suspend(struct device *dev)
{
	struct generic_pm_domain *genpd;
	bool (*stop_ok)(struct device *__dev);
	int ret;

	dev_dbg(dev, "%s()\n", __func__);

	genpd = dev_to_genpd(dev);
	if (IS_ERR(genpd))
		return -EINVAL;

	might_sleep_if(!genpd->dev_irq_safe);

	stop_ok = genpd->gov ? genpd->gov->stop_ok : NULL;
	if (stop_ok && !stop_ok(dev))
		return -EBUSY;

	ret = genpd_stop_dev(genpd, dev);
	if (ret)
		return ret;

	/*
                                                                      
                                 
  */
	if (dev->power.irq_safe)
		return 0;

	mutex_lock(&genpd->lock);
	genpd->in_progress++;
	pm_genpd_poweroff(genpd);
	genpd->in_progress--;
	mutex_unlock(&genpd->lock);

	return 0;
}

/* 
                                                                        
                          
  
                                                                       
                                                                   
                                                                               
 */
static int pm_genpd_runtime_resume(struct device *dev)
{
	struct generic_pm_domain *genpd;
	DEFINE_WAIT(wait);
	int ret;

	dev_dbg(dev, "%s()\n", __func__);

	genpd = dev_to_genpd(dev);
	if (IS_ERR(genpd))
		return -EINVAL;

	might_sleep_if(!genpd->dev_irq_safe);

	/*                                                        */
	if (dev->power.irq_safe)
		return genpd_start_dev_no_timing(genpd, dev);

	mutex_lock(&genpd->lock);
	ret = __pm_genpd_poweron(genpd);
	if (ret) {
		mutex_unlock(&genpd->lock);
		return ret;
	}
	genpd->status = GPD_STATE_BUSY;
	genpd->resume_count++;
	for (;;) {
		prepare_to_wait(&genpd->status_wait_queue, &wait,
				TASK_UNINTERRUPTIBLE);
		/*
                                                             
                                                               
              
   */
		if (!genpd->poweroff_task || genpd->poweroff_task == current)
			break;
		mutex_unlock(&genpd->lock);

		schedule();

		mutex_lock(&genpd->lock);
	}
	finish_wait(&genpd->status_wait_queue, &wait);
	__pm_genpd_restore_device(dev->power.subsys_data->domain_data, genpd);
	genpd->resume_count--;
	genpd_set_active(genpd);
	wake_up_all(&genpd->status_wait_queue);
	mutex_unlock(&genpd->lock);

	return 0;
}

/* 
                                                                              
 */
void pm_genpd_poweroff_unused(void)
{
	struct generic_pm_domain *genpd;

	mutex_lock(&gpd_list_lock);

	list_for_each_entry(genpd, &gpd_list, gpd_list_node)
		genpd_queue_power_off_work(genpd);

	mutex_unlock(&gpd_list_lock);
}

#else

static inline int genpd_dev_pm_qos_notifier(struct notifier_block *nb,
					    unsigned long val, void *ptr)
{
	return NOTIFY_DONE;
}

static inline void genpd_power_off_work_fn(struct work_struct *work) {}

#define pm_genpd_runtime_suspend	NULL
#define pm_genpd_runtime_resume		NULL

#endif /*                   */

#ifdef CONFIG_PM_SLEEP

/* 
                                                                        
                              
 */
static bool pm_genpd_present(struct generic_pm_domain *genpd)
{
	struct generic_pm_domain *gpd;

	if (IS_ERR_OR_NULL(genpd))
		return false;

	list_for_each_entry(gpd, &gpd_list, gpd_list_node)
		if (gpd == genpd)
			return true;

	return false;
}

static bool genpd_dev_active_wakeup(struct generic_pm_domain *genpd,
				    struct device *dev)
{
	return GENPD_DEV_CALLBACK(genpd, bool, active_wakeup, dev);
}

static int genpd_suspend_dev(struct generic_pm_domain *genpd, struct device *dev)
{
	return GENPD_DEV_CALLBACK(genpd, int, suspend, dev);
}

static int genpd_suspend_late(struct generic_pm_domain *genpd, struct device *dev)
{
	return GENPD_DEV_CALLBACK(genpd, int, suspend_late, dev);
}

static int genpd_resume_early(struct generic_pm_domain *genpd, struct device *dev)
{
	return GENPD_DEV_CALLBACK(genpd, int, resume_early, dev);
}

static int genpd_resume_dev(struct generic_pm_domain *genpd, struct device *dev)
{
	return GENPD_DEV_CALLBACK(genpd, int, resume, dev);
}

static int genpd_freeze_dev(struct generic_pm_domain *genpd, struct device *dev)
{
	return GENPD_DEV_CALLBACK(genpd, int, freeze, dev);
}

static int genpd_freeze_late(struct generic_pm_domain *genpd, struct device *dev)
{
	return GENPD_DEV_CALLBACK(genpd, int, freeze_late, dev);
}

static int genpd_thaw_early(struct generic_pm_domain *genpd, struct device *dev)
{
	return GENPD_DEV_CALLBACK(genpd, int, thaw_early, dev);
}

static int genpd_thaw_dev(struct generic_pm_domain *genpd, struct device *dev)
{
	return GENPD_DEV_CALLBACK(genpd, int, thaw, dev);
}

/* 
                                                                                
                                               
  
                                                                            
                                                                                
  
                                                                               
                                                                              
                                                                             
             
 */
static void pm_genpd_sync_poweroff(struct generic_pm_domain *genpd)
{
	struct gpd_link *link;

	if (genpd->status == GPD_STATE_POWER_OFF)
		return;

	if (genpd->suspended_count != genpd->device_count
	    || atomic_read(&genpd->sd_count) > 0)
		return;

	if (genpd->power_off)
		genpd->power_off(genpd);

	genpd->status = GPD_STATE_POWER_OFF;

	list_for_each_entry(link, &genpd->slave_links, slave_node) {
		genpd_sd_counter_dec(link->master);
		pm_genpd_sync_poweroff(link->master);
	}
}

/* 
                                                                              
                                 
  
                                                                               
                                                                              
                                                                             
             
 */
static void pm_genpd_sync_poweron(struct generic_pm_domain *genpd)
{
	struct gpd_link *link;

	if (genpd->status != GPD_STATE_POWER_OFF)
		return;

	list_for_each_entry(link, &genpd->slave_links, slave_node) {
		pm_genpd_sync_poweron(link->master);
		genpd_sd_counter_inc(link->master);
	}

	if (genpd->power_on)
		genpd->power_on(genpd);

	genpd->status = GPD_STATE_ACTIVE;
}

/* 
                                                                          
                         
                                           
  
                                                                               
                                                                               
                                                                               
                                                                               
                                                                                
                                                                             
                                                                               
                                                                            
                                                                                
                                              
 */
static bool resume_needed(struct device *dev, struct generic_pm_domain *genpd)
{
	bool active_wakeup;

	if (!device_can_wakeup(dev))
		return false;

	active_wakeup = genpd_dev_active_wakeup(genpd, dev);
	return device_may_wakeup(dev) ? active_wakeup : !active_wakeup;
}

/* 
                                                                        
                                           
  
                                                                               
                                                                               
                                                                      
                             
 */
static int pm_genpd_prepare(struct device *dev)
{
	struct generic_pm_domain *genpd;
	int ret;

	dev_dbg(dev, "%s()\n", __func__);

	genpd = dev_to_genpd(dev);
	if (IS_ERR(genpd))
		return -EINVAL;

	/*
                                                                        
                                                                      
                                                   
  */
	pm_runtime_get_noresume(dev);
	if (pm_runtime_barrier(dev) && device_may_wakeup(dev))
		pm_wakeup_event(dev, 0);

	if (pm_wakeup_pending()) {
		pm_runtime_put(dev);
		return -EBUSY;
	}

	if (resume_needed(dev, genpd))
		pm_runtime_resume(dev);

	genpd_acquire_lock(genpd);

	if (genpd->prepared_count++ == 0) {
		genpd->suspended_count = 0;
		genpd->suspend_power_off = genpd->status == GPD_STATE_POWER_OFF;
	}

	genpd_release_lock(genpd);

	if (genpd->suspend_power_off) {
		pm_runtime_put_noidle(dev);
		return 0;
	}

	/*
                                                                      
                                                                    
                                                                      
                           
  */
	pm_runtime_resume(dev);
	__pm_runtime_disable(dev, false);

	ret = pm_generic_prepare(dev);
	if (ret) {
		mutex_lock(&genpd->lock);

		if (--genpd->prepared_count == 0)
			genpd->suspend_power_off = false;

		mutex_unlock(&genpd->lock);
		pm_runtime_enable(dev);
	}

	pm_runtime_put(dev);
	return ret;
}

/* 
                                                                     
                           
  
                                                                               
                                                                           
                                         
 */
static int pm_genpd_suspend(struct device *dev)
{
	struct generic_pm_domain *genpd;

	dev_dbg(dev, "%s()\n", __func__);

	genpd = dev_to_genpd(dev);
	if (IS_ERR(genpd))
		return -EINVAL;

	return genpd->suspend_power_off ? 0 : genpd_suspend_dev(genpd, dev);
}

/* 
                                                                          
                           
  
                                                                     
                                                                   
                                                                               
 */
static int pm_genpd_suspend_late(struct device *dev)
{
	struct generic_pm_domain *genpd;

	dev_dbg(dev, "%s()\n", __func__);

	genpd = dev_to_genpd(dev);
	if (IS_ERR(genpd))
		return -EINVAL;

	return genpd->suspend_power_off ? 0 : genpd_suspend_late(genpd, dev);
}

/* 
                                                                                
                           
  
                                                                             
                
 */
static int pm_genpd_suspend_noirq(struct device *dev)
{
	struct generic_pm_domain *genpd;

	dev_dbg(dev, "%s()\n", __func__);

	genpd = dev_to_genpd(dev);
	if (IS_ERR(genpd))
		return -EINVAL;

	if (genpd->suspend_power_off
	    || (dev->power.wakeup_path && genpd_dev_active_wakeup(genpd, dev)))
		return 0;

	genpd_stop_dev(genpd, dev);

	/*
                                                                       
                                                                      
                                                                   
  */
	genpd->suspended_count++;
	pm_genpd_sync_poweroff(genpd);

	return 0;
}

/* 
                                                                         
                          
  
                                                                               
 */
static int pm_genpd_resume_noirq(struct device *dev)
{
	struct generic_pm_domain *genpd;

	dev_dbg(dev, "%s()\n", __func__);

	genpd = dev_to_genpd(dev);
	if (IS_ERR(genpd))
		return -EINVAL;

	if (genpd->suspend_power_off
	    || (dev->power.wakeup_path && genpd_dev_active_wakeup(genpd, dev)))
		return 0;

	/*
                                                                       
                                                                      
                                                                   
  */
	pm_genpd_sync_poweron(genpd);
	genpd->suspended_count--;

	return genpd_start_dev(genpd, dev);
}

/* 
                                                                        
                          
  
                                                                      
                                                                   
                                                                         
           
 */
static int pm_genpd_resume_early(struct device *dev)
{
	struct generic_pm_domain *genpd;

	dev_dbg(dev, "%s()\n", __func__);

	genpd = dev_to_genpd(dev);
	if (IS_ERR(genpd))
		return -EINVAL;

	return genpd->suspend_power_off ? 0 : genpd_resume_early(genpd, dev);
}

/* 
                                                          
                          
  
                                                                              
                                                                           
                                            
 */
static int pm_genpd_resume(struct device *dev)
{
	struct generic_pm_domain *genpd;

	dev_dbg(dev, "%s()\n", __func__);

	genpd = dev_to_genpd(dev);
	if (IS_ERR(genpd))
		return -EINVAL;

	return genpd->suspend_power_off ? 0 : genpd_resume_dev(genpd, dev);
}

/* 
                                                           
                          
  
                                                                              
                                                                           
                                            
 */
static int pm_genpd_freeze(struct device *dev)
{
	struct generic_pm_domain *genpd;

	dev_dbg(dev, "%s()\n", __func__);

	genpd = dev_to_genpd(dev);
	if (IS_ERR(genpd))
		return -EINVAL;

	return genpd->suspend_power_off ? 0 : genpd_freeze_dev(genpd, dev);
}

/* 
                                                                      
                          
  
                                                                    
                                                                   
                                                                         
           
 */
static int pm_genpd_freeze_late(struct device *dev)
{
	struct generic_pm_domain *genpd;

	dev_dbg(dev, "%s()\n", __func__);

	genpd = dev_to_genpd(dev);
	if (IS_ERR(genpd))
		return -EINVAL;

	return genpd->suspend_power_off ? 0 : genpd_freeze_late(genpd, dev);
}

/* 
                                                                               
                          
  
                                                                    
                                                                   
                                                                         
           
 */
static int pm_genpd_freeze_noirq(struct device *dev)
{
	struct generic_pm_domain *genpd;

	dev_dbg(dev, "%s()\n", __func__);

	genpd = dev_to_genpd(dev);
	if (IS_ERR(genpd))
		return -EINVAL;

	return genpd->suspend_power_off ? 0 : genpd_stop_dev(genpd, dev);
}

/* 
                                                                  
                        
  
                                                                          
                                
 */
static int pm_genpd_thaw_noirq(struct device *dev)
{
	struct generic_pm_domain *genpd;

	dev_dbg(dev, "%s()\n", __func__);

	genpd = dev_to_genpd(dev);
	if (IS_ERR(genpd))
		return -EINVAL;

	return genpd->suspend_power_off ? 0 : genpd_start_dev(genpd, dev);
}

/* 
                                                                  
                        
  
                                                                    
                                                                   
                                                                         
           
 */
static int pm_genpd_thaw_early(struct device *dev)
{
	struct generic_pm_domain *genpd;

	dev_dbg(dev, "%s()\n", __func__);

	genpd = dev_to_genpd(dev);
	if (IS_ERR(genpd))
		return -EINVAL;

	return genpd->suspend_power_off ? 0 : genpd_thaw_early(genpd, dev);
}

/* 
                                                                  
                        
  
                                                                            
                                                                           
                                            
 */
static int pm_genpd_thaw(struct device *dev)
{
	struct generic_pm_domain *genpd;

	dev_dbg(dev, "%s()\n", __func__);

	genpd = dev_to_genpd(dev);
	if (IS_ERR(genpd))
		return -EINVAL;

	return genpd->suspend_power_off ? 0 : genpd_thaw_dev(genpd, dev);
}

/* 
                                                                           
                          
  
                                                                     
                                                                             
 */
static int pm_genpd_restore_noirq(struct device *dev)
{
	struct generic_pm_domain *genpd;

	dev_dbg(dev, "%s()\n", __func__);

	genpd = dev_to_genpd(dev);
	if (IS_ERR(genpd))
		return -EINVAL;

	/*
                                                                       
                                                                      
                                                                   
   
                                                                     
                                                         
  */
	if (genpd->suspended_count++ == 0) {
		/*
                                                               
                                                                 
                                                               
   */
		genpd->status = GPD_STATE_POWER_OFF;
		if (genpd->suspend_power_off) {
			/*
                                                        
                                        
    */
			if (genpd->power_off)
				genpd->power_off(genpd);

			return 0;
		}
	}

	if (genpd->suspend_power_off)
		return 0;

	pm_genpd_sync_poweron(genpd);

	return genpd_start_dev(genpd, dev);
}

/* 
                                                                               
                                              
  
                                                                      
                                                                          
                                                                           
                                            
 */
static void pm_genpd_complete(struct device *dev)
{
	struct generic_pm_domain *genpd;
	bool run_complete;

	dev_dbg(dev, "%s()\n", __func__);

	genpd = dev_to_genpd(dev);
	if (IS_ERR(genpd))
		return;

	mutex_lock(&genpd->lock);

	run_complete = !genpd->suspend_power_off;
	if (--genpd->prepared_count == 0)
		genpd->suspend_power_off = false;

	mutex_unlock(&genpd->lock);

	if (run_complete) {
		pm_generic_complete(dev);
		pm_runtime_set_active(dev);
		pm_runtime_enable(dev);
		pm_request_idle(dev);
	}
}

/* 
                                                                               
                                                                           
  
                                                                              
                                                            
 */
void pm_genpd_syscore_switch(struct device *dev, bool suspend)
{
	struct generic_pm_domain *genpd;

	genpd = dev_to_genpd(dev);
	if (!pm_genpd_present(genpd))
		return;

	if (suspend) {
		genpd->suspended_count++;
		pm_genpd_sync_poweroff(genpd);
	} else {
		pm_genpd_sync_poweron(genpd);
		genpd->suspended_count--;
	}
}
EXPORT_SYMBOL_GPL(pm_genpd_syscore_switch);

#else

#define pm_genpd_prepare		NULL
#define pm_genpd_suspend		NULL
#define pm_genpd_suspend_late		NULL
#define pm_genpd_suspend_noirq		NULL
#define pm_genpd_resume_early		NULL
#define pm_genpd_resume_noirq		NULL
#define pm_genpd_resume			NULL
#define pm_genpd_freeze			NULL
#define pm_genpd_freeze_late		NULL
#define pm_genpd_freeze_noirq		NULL
#define pm_genpd_thaw_early		NULL
#define pm_genpd_thaw_noirq		NULL
#define pm_genpd_thaw			NULL
#define pm_genpd_restore_noirq		NULL
#define pm_genpd_complete		NULL

#endif /*                 */

static struct generic_pm_domain_data *__pm_genpd_alloc_dev_data(struct device *dev)
{
	struct generic_pm_domain_data *gpd_data;

	gpd_data = kzalloc(sizeof(*gpd_data), GFP_KERNEL);
	if (!gpd_data)
		return NULL;

	mutex_init(&gpd_data->lock);
	gpd_data->nb.notifier_call = genpd_dev_pm_qos_notifier;
	dev_pm_qos_add_notifier(dev, &gpd_data->nb);
	return gpd_data;
}

static void __pm_genpd_free_dev_data(struct device *dev,
				     struct generic_pm_domain_data *gpd_data)
{
	dev_pm_qos_remove_notifier(dev, &gpd_data->nb);
	kfree(gpd_data);
}

/* 
                                                            
                                          
                            
                                                                
 */
int __pm_genpd_add_device(struct generic_pm_domain *genpd, struct device *dev,
			  struct gpd_timing_data *td)
{
	struct generic_pm_domain_data *gpd_data_new, *gpd_data = NULL;
	struct pm_domain_data *pdd;
	int ret = 0;

	dev_dbg(dev, "%s()\n", __func__);

	if (IS_ERR_OR_NULL(genpd) || IS_ERR_OR_NULL(dev))
		return -EINVAL;

	gpd_data_new = __pm_genpd_alloc_dev_data(dev);
	if (!gpd_data_new)
		return -ENOMEM;

	genpd_acquire_lock(genpd);

	if (genpd->prepared_count > 0) {
		ret = -EAGAIN;
		goto out;
	}

	list_for_each_entry(pdd, &genpd->dev_list, list_node)
		if (pdd->dev == dev) {
			ret = -EINVAL;
			goto out;
		}

	ret = dev_pm_get_subsys_data(dev);
	if (ret)
		goto out;

	genpd->device_count++;
	genpd->max_off_time_changed = true;

	spin_lock_irq(&dev->power.lock);

	dev->pm_domain = &genpd->domain;
	if (dev->power.subsys_data->domain_data) {
		gpd_data = to_gpd_data(dev->power.subsys_data->domain_data);
	} else {
		gpd_data = gpd_data_new;
		dev->power.subsys_data->domain_data = &gpd_data->base;
	}
	gpd_data->refcount++;
	if (td)
		gpd_data->td = *td;

	spin_unlock_irq(&dev->power.lock);

	mutex_lock(&gpd_data->lock);
	gpd_data->base.dev = dev;
	list_add_tail(&gpd_data->base.list_node, &genpd->dev_list);
	gpd_data->need_restore = genpd->status == GPD_STATE_POWER_OFF;
	gpd_data->td.constraint_changed = true;
	gpd_data->td.effective_constraint_ns = -1;
	mutex_unlock(&gpd_data->lock);

 out:
	genpd_release_lock(genpd);

	if (gpd_data != gpd_data_new)
		__pm_genpd_free_dev_data(dev, gpd_data_new);

	return ret;
}

/* 
                                                               
                                                                              
                            
                            
                                                                
 */
int __pm_genpd_of_add_device(struct device_node *genpd_node, struct device *dev,
			     struct gpd_timing_data *td)
{
	struct generic_pm_domain *genpd = NULL, *gpd;

	dev_dbg(dev, "%s()\n", __func__);

	if (IS_ERR_OR_NULL(genpd_node) || IS_ERR_OR_NULL(dev))
		return -EINVAL;

	mutex_lock(&gpd_list_lock);
	list_for_each_entry(gpd, &gpd_list, gpd_list_node) {
		if (gpd->of_node == genpd_node) {
			genpd = gpd;
			break;
		}
	}
	mutex_unlock(&gpd_list_lock);

	if (!genpd)
		return -EINVAL;

	return __pm_genpd_add_device(genpd, dev, td);
}


/* 
                                                                          
                                                            
                            
                                                                
 */
int __pm_genpd_name_add_device(const char *domain_name, struct device *dev,
			       struct gpd_timing_data *td)
{
	return __pm_genpd_add_device(pm_genpd_lookup_name(domain_name), dev, td);
}

/* 
                                                                  
                                               
                              
 */
int pm_genpd_remove_device(struct generic_pm_domain *genpd,
			   struct device *dev)
{
	struct generic_pm_domain_data *gpd_data;
	struct pm_domain_data *pdd;
	bool remove = false;
	int ret = 0;

	dev_dbg(dev, "%s()\n", __func__);

	if (IS_ERR_OR_NULL(genpd) || IS_ERR_OR_NULL(dev)
	    ||  IS_ERR_OR_NULL(dev->pm_domain)
	    ||  pd_to_genpd(dev->pm_domain) != genpd)
		return -EINVAL;

	genpd_acquire_lock(genpd);

	if (genpd->prepared_count > 0) {
		ret = -EAGAIN;
		goto out;
	}

	genpd->device_count--;
	genpd->max_off_time_changed = true;

	spin_lock_irq(&dev->power.lock);

	dev->pm_domain = NULL;
	pdd = dev->power.subsys_data->domain_data;
	list_del_init(&pdd->list_node);
	gpd_data = to_gpd_data(pdd);
	if (--gpd_data->refcount == 0) {
		dev->power.subsys_data->domain_data = NULL;
		remove = true;
	}

	spin_unlock_irq(&dev->power.lock);

	mutex_lock(&gpd_data->lock);
	pdd->dev = NULL;
	mutex_unlock(&gpd_data->lock);

	genpd_release_lock(genpd);

	dev_pm_put_subsys_data(dev);
	if (remove)
		__pm_genpd_free_dev_data(dev, gpd_data);

	return 0;

 out:
	genpd_release_lock(genpd);

	return ret;
}

/* 
                                                                          
                                          
                                                           
 */
void pm_genpd_dev_need_restore(struct device *dev, bool val)
{
	struct pm_subsys_data *psd;
	unsigned long flags;

	spin_lock_irqsave(&dev->power.lock, flags);

	psd = dev_to_psd(dev);
	if (psd && psd->domain_data)
		to_gpd_data(psd->domain_data)->need_restore = val;

	spin_unlock_irqrestore(&dev->power.lock, flags);
}
EXPORT_SYMBOL_GPL(pm_genpd_dev_need_restore);

/* 
                                                                
                                                    
                                     
 */
int pm_genpd_add_subdomain(struct generic_pm_domain *genpd,
			   struct generic_pm_domain *subdomain)
{
	struct gpd_link *link;
	int ret = 0;

	if (IS_ERR_OR_NULL(genpd) || IS_ERR_OR_NULL(subdomain)
	    || genpd == subdomain)
		return -EINVAL;

 start:
	genpd_acquire_lock(genpd);
	mutex_lock_nested(&subdomain->lock, SINGLE_DEPTH_NESTING);

	if (subdomain->status != GPD_STATE_POWER_OFF
	    && subdomain->status != GPD_STATE_ACTIVE) {
		mutex_unlock(&subdomain->lock);
		genpd_release_lock(genpd);
		goto start;
	}

	if (genpd->status == GPD_STATE_POWER_OFF
	    &&  subdomain->status != GPD_STATE_POWER_OFF) {
		ret = -EINVAL;
		goto out;
	}

	list_for_each_entry(link, &genpd->master_links, master_node) {
		if (link->slave == subdomain && link->master == genpd) {
			ret = -EINVAL;
			goto out;
		}
	}

	link = kzalloc(sizeof(*link), GFP_KERNEL);
	if (!link) {
		ret = -ENOMEM;
		goto out;
	}
	link->master = genpd;
	list_add_tail(&link->master_node, &genpd->master_links);
	link->slave = subdomain;
	list_add_tail(&link->slave_node, &subdomain->slave_links);
	if (subdomain->status != GPD_STATE_POWER_OFF)
		genpd_sd_counter_inc(genpd);

 out:
	mutex_unlock(&subdomain->lock);
	genpd_release_lock(genpd);

	return ret;
}

/* 
                                                                      
                                                                      
                                                      
 */
int pm_genpd_add_subdomain_names(const char *master_name,
				 const char *subdomain_name)
{
	struct generic_pm_domain *master = NULL, *subdomain = NULL, *gpd;

	if (IS_ERR_OR_NULL(master_name) || IS_ERR_OR_NULL(subdomain_name))
		return -EINVAL;

	mutex_lock(&gpd_list_lock);
	list_for_each_entry(gpd, &gpd_list, gpd_list_node) {
		if (!master && !strcmp(gpd->name, master_name))
			master = gpd;

		if (!subdomain && !strcmp(gpd->name, subdomain_name))
			subdomain = gpd;

		if (master && subdomain)
			break;
	}
	mutex_unlock(&gpd_list_lock);

	return pm_genpd_add_subdomain(master, subdomain);
}

/* 
                                                                        
                                                         
                                       
 */
int pm_genpd_remove_subdomain(struct generic_pm_domain *genpd,
			      struct generic_pm_domain *subdomain)
{
	struct gpd_link *link;
	int ret = -EINVAL;

	if (IS_ERR_OR_NULL(genpd) || IS_ERR_OR_NULL(subdomain))
		return -EINVAL;

 start:
	genpd_acquire_lock(genpd);

	list_for_each_entry(link, &genpd->master_links, master_node) {
		if (link->slave != subdomain)
			continue;

		mutex_lock_nested(&subdomain->lock, SINGLE_DEPTH_NESTING);

		if (subdomain->status != GPD_STATE_POWER_OFF
		    && subdomain->status != GPD_STATE_ACTIVE) {
			mutex_unlock(&subdomain->lock);
			genpd_release_lock(genpd);
			goto start;
		}

		list_del(&link->master_node);
		list_del(&link->slave_node);
		kfree(link);
		if (subdomain->status != GPD_STATE_POWER_OFF)
			genpd_sd_counter_dec(genpd);

		mutex_unlock(&subdomain->lock);

		ret = 0;
		break;
	}

	genpd_release_lock(genpd);

	return ret;
}

/* 
                                                                      
                                        
                                 
                                                                             
  
                                                               
                                                             
 */
int pm_genpd_add_callbacks(struct device *dev, struct gpd_dev_ops *ops,
			   struct gpd_timing_data *td)
{
	struct generic_pm_domain_data *gpd_data_new, *gpd_data = NULL;
	int ret = 0;

	if (!(dev && ops))
		return -EINVAL;

	gpd_data_new = __pm_genpd_alloc_dev_data(dev);
	if (!gpd_data_new)
		return -ENOMEM;

	pm_runtime_disable(dev);
	device_pm_lock();

	ret = dev_pm_get_subsys_data(dev);
	if (ret)
		goto out;

	spin_lock_irq(&dev->power.lock);

	if (dev->power.subsys_data->domain_data) {
		gpd_data = to_gpd_data(dev->power.subsys_data->domain_data);
	} else {
		gpd_data = gpd_data_new;
		dev->power.subsys_data->domain_data = &gpd_data->base;
	}
	gpd_data->refcount++;
	gpd_data->ops = *ops;
	if (td)
		gpd_data->td = *td;

	spin_unlock_irq(&dev->power.lock);

 out:
	device_pm_unlock();
	pm_runtime_enable(dev);

	if (gpd_data != gpd_data_new)
		__pm_genpd_free_dev_data(dev, gpd_data_new);

	return ret;
}
EXPORT_SYMBOL_GPL(pm_genpd_add_callbacks);

/* 
                                                                                
                                             
                                                         
  
                                                                  
 */
int __pm_genpd_remove_callbacks(struct device *dev, bool clear_td)
{
	struct generic_pm_domain_data *gpd_data = NULL;
	bool remove = false;
	int ret = 0;

	if (!(dev && dev->power.subsys_data))
		return -EINVAL;

	pm_runtime_disable(dev);
	device_pm_lock();

	spin_lock_irq(&dev->power.lock);

	if (dev->power.subsys_data->domain_data) {
		gpd_data = to_gpd_data(dev->power.subsys_data->domain_data);
		gpd_data->ops = (struct gpd_dev_ops){ NULL };
		if (clear_td)
			gpd_data->td = (struct gpd_timing_data){ 0 };

		if (--gpd_data->refcount == 0) {
			dev->power.subsys_data->domain_data = NULL;
			remove = true;
		}
	} else {
		ret = -EINVAL;
	}

	spin_unlock_irq(&dev->power.lock);

	device_pm_unlock();
	pm_runtime_enable(dev);

	if (ret)
		return ret;

	dev_pm_put_subsys_data(dev);
	if (remove)
		__pm_genpd_free_dev_data(dev, gpd_data);

	return 0;
}
EXPORT_SYMBOL_GPL(__pm_genpd_remove_callbacks);

/* 
                                                                      
                                                  
                                                        
  
                                                                              
                                                                              
                                                                            
 */
int pm_genpd_attach_cpuidle(struct generic_pm_domain *genpd, int state)
{
	struct cpuidle_driver *cpuidle_drv;
	struct gpd_cpu_data *cpu_data;
	struct cpuidle_state *idle_state;
	int ret = 0;

	if (IS_ERR_OR_NULL(genpd) || state < 0)
		return -EINVAL;

	genpd_acquire_lock(genpd);

	if (genpd->cpu_data) {
		ret = -EEXIST;
		goto out;
	}
	cpu_data = kzalloc(sizeof(*cpu_data), GFP_KERNEL);
	if (!cpu_data) {
		ret = -ENOMEM;
		goto out;
	}
	cpuidle_drv = cpuidle_driver_ref();
	if (!cpuidle_drv) {
		ret = -ENODEV;
		goto err_drv;
	}
	if (cpuidle_drv->state_count <= state) {
		ret = -EINVAL;
		goto err;
	}
	idle_state = &cpuidle_drv->states[state];
	if (!idle_state->disabled) {
		ret = -EAGAIN;
		goto err;
	}
	cpu_data->idle_state = idle_state;
	cpu_data->saved_exit_latency = idle_state->exit_latency;
	genpd->cpu_data = cpu_data;
	genpd_recalc_cpu_exit_latency(genpd);

 out:
	genpd_release_lock(genpd);
	return ret;

 err:
	cpuidle_driver_unref();

 err_drv:
	kfree(cpu_data);
	goto out;
}

/* 
                                                                           
                                                   
                                                    
 */
int pm_genpd_name_attach_cpuidle(const char *name, int state)
{
	return pm_genpd_attach_cpuidle(pm_genpd_lookup_name(name), state);
}

/* 
                                                                            
                                                           
  
                                                                             
                   
 */
int pm_genpd_detach_cpuidle(struct generic_pm_domain *genpd)
{
	struct gpd_cpu_data *cpu_data;
	struct cpuidle_state *idle_state;
	int ret = 0;

	if (IS_ERR_OR_NULL(genpd))
		return -EINVAL;

	genpd_acquire_lock(genpd);

	cpu_data = genpd->cpu_data;
	if (!cpu_data) {
		ret = -ENODEV;
		goto out;
	}
	idle_state = cpu_data->idle_state;
	if (!idle_state->disabled) {
		ret = -EAGAIN;
		goto out;
	}
	idle_state->exit_latency = cpu_data->saved_exit_latency;
	cpuidle_driver_unref();
	genpd->cpu_data = NULL;
	kfree(cpu_data);

 out:
	genpd_release_lock(genpd);
	return ret;
}

/* 
                                                                                
                                                        
 */
int pm_genpd_name_detach_cpuidle(const char *name)
{
	return pm_genpd_detach_cpuidle(pm_genpd_lookup_name(name));
}

/*                                                  */

/* 
                                                                            
                          
 */
static int pm_genpd_default_save_state(struct device *dev)
{
	int (*cb)(struct device *__dev);

	cb = dev_gpd_data(dev)->ops.save_state;
	if (cb)
		return cb(dev);

	if (dev->type && dev->type->pm)
		cb = dev->type->pm->runtime_suspend;
	else if (dev->class && dev->class->pm)
		cb = dev->class->pm->runtime_suspend;
	else if (dev->bus && dev->bus->pm)
		cb = dev->bus->pm->runtime_suspend;
	else
		cb = NULL;

	if (!cb && dev->driver && dev->driver->pm)
		cb = dev->driver->pm->runtime_suspend;

	return cb ? cb(dev) : 0;
}

/* 
                                                                              
                          
 */
static int pm_genpd_default_restore_state(struct device *dev)
{
	int (*cb)(struct device *__dev);

	cb = dev_gpd_data(dev)->ops.restore_state;
	if (cb)
		return cb(dev);

	if (dev->type && dev->type->pm)
		cb = dev->type->pm->runtime_resume;
	else if (dev->class && dev->class->pm)
		cb = dev->class->pm->runtime_resume;
	else if (dev->bus && dev->bus->pm)
		cb = dev->bus->pm->runtime_resume;
	else
		cb = NULL;

	if (!cb && dev->driver && dev->driver->pm)
		cb = dev->driver->pm->runtime_resume;

	return cb ? cb(dev) : 0;
}

#ifdef CONFIG_PM_SLEEP

/* 
                                                                      
                          
 */
static int pm_genpd_default_suspend(struct device *dev)
{
	int (*cb)(struct device *__dev) = dev_gpd_data(dev)->ops.suspend;

	return cb ? cb(dev) : pm_generic_suspend(dev);
}

/* 
                                                                                
                          
 */
static int pm_genpd_default_suspend_late(struct device *dev)
{
	int (*cb)(struct device *__dev) = dev_gpd_data(dev)->ops.suspend_late;

	return cb ? cb(dev) : pm_generic_suspend_late(dev);
}

/* 
                                                                                
                          
 */
static int pm_genpd_default_resume_early(struct device *dev)
{
	int (*cb)(struct device *__dev) = dev_gpd_data(dev)->ops.resume_early;

	return cb ? cb(dev) : pm_generic_resume_early(dev);
}

/* 
                                                                    
                          
 */
static int pm_genpd_default_resume(struct device *dev)
{
	int (*cb)(struct device *__dev) = dev_gpd_data(dev)->ops.resume;

	return cb ? cb(dev) : pm_generic_resume(dev);
}

/* 
                                                                    
                          
 */
static int pm_genpd_default_freeze(struct device *dev)
{
	int (*cb)(struct device *__dev) = dev_gpd_data(dev)->ops.freeze;

	return cb ? cb(dev) : pm_generic_freeze(dev);
}

/* 
                                                                              
                          
 */
static int pm_genpd_default_freeze_late(struct device *dev)
{
	int (*cb)(struct device *__dev) = dev_gpd_data(dev)->ops.freeze_late;

	return cb ? cb(dev) : pm_generic_freeze_late(dev);
}

/* 
                                                                            
                          
 */
static int pm_genpd_default_thaw_early(struct device *dev)
{
	int (*cb)(struct device *__dev) = dev_gpd_data(dev)->ops.thaw_early;

	return cb ? cb(dev) : pm_generic_thaw_early(dev);
}

/* 
                                                                
                          
 */
static int pm_genpd_default_thaw(struct device *dev)
{
	int (*cb)(struct device *__dev) = dev_gpd_data(dev)->ops.thaw;

	return cb ? cb(dev) : pm_generic_thaw(dev);
}

#else /*                  */

#define pm_genpd_default_suspend	NULL
#define pm_genpd_default_suspend_late	NULL
#define pm_genpd_default_resume_early	NULL
#define pm_genpd_default_resume		NULL
#define pm_genpd_default_freeze		NULL
#define pm_genpd_default_freeze_late	NULL
#define pm_genpd_default_thaw_early	NULL
#define pm_genpd_default_thaw		NULL

#endif /*                  */

/* 
                                                             
                                          
                                                                       
                                                             
 */
void pm_genpd_init(struct generic_pm_domain *genpd,
		   struct dev_power_governor *gov, bool is_off)
{
	if (IS_ERR_OR_NULL(genpd))
		return;

	INIT_LIST_HEAD(&genpd->master_links);
	INIT_LIST_HEAD(&genpd->slave_links);
	INIT_LIST_HEAD(&genpd->dev_list);
	mutex_init(&genpd->lock);
	genpd->gov = gov;
	INIT_WORK(&genpd->power_off_work, genpd_power_off_work_fn);
	genpd->in_progress = 0;
	atomic_set(&genpd->sd_count, 0);
	genpd->status = is_off ? GPD_STATE_POWER_OFF : GPD_STATE_ACTIVE;
	init_waitqueue_head(&genpd->status_wait_queue);
	genpd->poweroff_task = NULL;
	genpd->resume_count = 0;
	genpd->device_count = 0;
	genpd->max_off_time_ns = -1;
	genpd->max_off_time_changed = true;
	genpd->domain.ops.runtime_suspend = pm_genpd_runtime_suspend;
	genpd->domain.ops.runtime_resume = pm_genpd_runtime_resume;
	genpd->domain.ops.runtime_idle = pm_generic_runtime_idle;
	genpd->domain.ops.prepare = pm_genpd_prepare;
	genpd->domain.ops.suspend = pm_genpd_suspend;
	genpd->domain.ops.suspend_late = pm_genpd_suspend_late;
	genpd->domain.ops.suspend_noirq = pm_genpd_suspend_noirq;
	genpd->domain.ops.resume_noirq = pm_genpd_resume_noirq;
	genpd->domain.ops.resume_early = pm_genpd_resume_early;
	genpd->domain.ops.resume = pm_genpd_resume;
	genpd->domain.ops.freeze = pm_genpd_freeze;
	genpd->domain.ops.freeze_late = pm_genpd_freeze_late;
	genpd->domain.ops.freeze_noirq = pm_genpd_freeze_noirq;
	genpd->domain.ops.thaw_noirq = pm_genpd_thaw_noirq;
	genpd->domain.ops.thaw_early = pm_genpd_thaw_early;
	genpd->domain.ops.thaw = pm_genpd_thaw;
	genpd->domain.ops.poweroff = pm_genpd_suspend;
	genpd->domain.ops.poweroff_late = pm_genpd_suspend_late;
	genpd->domain.ops.poweroff_noirq = pm_genpd_suspend_noirq;
	genpd->domain.ops.restore_noirq = pm_genpd_restore_noirq;
	genpd->domain.ops.restore_early = pm_genpd_resume_early;
	genpd->domain.ops.restore = pm_genpd_resume;
	genpd->domain.ops.complete = pm_genpd_complete;
	genpd->dev_ops.save_state = pm_genpd_default_save_state;
	genpd->dev_ops.restore_state = pm_genpd_default_restore_state;
	genpd->dev_ops.suspend = pm_genpd_default_suspend;
	genpd->dev_ops.suspend_late = pm_genpd_default_suspend_late;
	genpd->dev_ops.resume_early = pm_genpd_default_resume_early;
	genpd->dev_ops.resume = pm_genpd_default_resume;
	genpd->dev_ops.freeze = pm_genpd_default_freeze;
	genpd->dev_ops.freeze_late = pm_genpd_default_freeze_late;
	genpd->dev_ops.thaw_early = pm_genpd_default_thaw_early;
	genpd->dev_ops.thaw = pm_genpd_default_thaw;
	mutex_lock(&gpd_list_lock);
	list_add(&genpd->gpd_list_node, &gpd_list);
	mutex_unlock(&gpd_list_lock);
}
