#ifndef _LINUX_SUSPEND_H
#define _LINUX_SUSPEND_H

#include <linux/swap.h>
#include <linux/notifier.h>
#include <linux/init.h>
#include <linux/pm.h>
#include <linux/mm.h>
#include <linux/freezer.h>
#include <asm/errno.h>

#ifdef CONFIG_VT
extern void pm_set_vt_switch(int);
#else
static inline void pm_set_vt_switch(int do_switch)
{
}
#endif

#ifdef CONFIG_VT_CONSOLE_SLEEP
extern int pm_prepare_console(void);
extern void pm_restore_console(void);
#else
static inline int pm_prepare_console(void)
{
	return 0;
}

static inline void pm_restore_console(void)
{
}
#endif

typedef int __bitwise suspend_state_t;

#define PM_SUSPEND_ON		((__force suspend_state_t) 0)
#define PM_SUSPEND_FREEZE	((__force suspend_state_t) 1)
#define PM_SUSPEND_STANDBY	((__force suspend_state_t) 2)
#define PM_SUSPEND_MEM		((__force suspend_state_t) 3)
#define PM_SUSPEND_MIN		PM_SUSPEND_FREEZE
#define PM_SUSPEND_MAX		((__force suspend_state_t) 4)

enum suspend_stat_step {
	SUSPEND_FREEZE = 1,
	SUSPEND_PREPARE,
	SUSPEND_SUSPEND,
	SUSPEND_SUSPEND_LATE,
	SUSPEND_SUSPEND_NOIRQ,
	SUSPEND_RESUME_NOIRQ,
	SUSPEND_RESUME_EARLY,
	SUSPEND_RESUME
};

struct suspend_stats {
	int	success;
	int	fail;
	int	failed_freeze;
	int	failed_prepare;
	int	failed_suspend;
	int	failed_suspend_late;
	int	failed_suspend_noirq;
	int	failed_resume;
	int	failed_resume_early;
	int	failed_resume_noirq;
#define	REC_FAILED_NUM	2
	int	last_failed_dev;
	char	failed_devs[REC_FAILED_NUM][40];
	int	last_failed_errno;
	int	errno[REC_FAILED_NUM];
	int	last_failed_step;
	enum suspend_stat_step	failed_steps[REC_FAILED_NUM];
};

extern struct suspend_stats suspend_stats;

static inline void dpm_save_failed_dev(const char *name)
{
	strlcpy(suspend_stats.failed_devs[suspend_stats.last_failed_dev],
		name,
		sizeof(suspend_stats.failed_devs[0]));
	suspend_stats.last_failed_dev++;
	suspend_stats.last_failed_dev %= REC_FAILED_NUM;
}

static inline void dpm_save_failed_errno(int err)
{
	suspend_stats.errno[suspend_stats.last_failed_errno] = err;
	suspend_stats.last_failed_errno++;
	suspend_stats.last_failed_errno %= REC_FAILED_NUM;
}

static inline void dpm_save_failed_step(enum suspend_stat_step step)
{
	suspend_stats.failed_steps[suspend_stats.last_failed_step] = step;
	suspend_stats.last_failed_step++;
	suspend_stats.last_failed_step %= REC_FAILED_NUM;
}

/* 
                                                                          
                       
  
                                                                            
                
                                                                         
                                                                           
                           
                                                                      
                                                            
  
                                                               
                                                                           
                                                                           
                                                                        
                                                                        
                                                                          
                                                         
  
                                                                               
               
                                                                        
                                                                       
                                                                          
                                                                     
                                                                          
                                                
  
                                                                             
                               
                                                                  
                                                                         
                                                                     
                                                                    
             
  
                                                                               
                                               
                                                                     
                                                                          
               
  
                                                                         
                                                                      
                                 
                                                                        
                                                                       
                                                               
  
                                           
                                                                           
             
                                                                        
                                                                        
                                                                         
                      
  
                                                                            
                                                                     
                                                                        
                                                                       
                                                                         
                                                            
  
                                                                           
                                                                    
                                                      
                                                                        
                                                                         
                                                                          
            
  
                                                         
                                                            
                                                                        
                                                            
 */
struct platform_suspend_ops {
	int (*valid)(suspend_state_t state);
	int (*begin)(suspend_state_t state);
	int (*prepare)(void);
	int (*prepare_late)(void);
	int (*enter)(suspend_state_t state);
	void (*wake)(void);
	void (*finish)(void);
	bool (*suspend_again)(void);
	void (*end)(void);
	void (*recover)(void);
};

#ifdef CONFIG_SUSPEND
/* 
                                                              
                                           
 */
extern void suspend_set_ops(const struct platform_suspend_ops *ops);
extern int suspend_valid_only_mem(suspend_state_t state);
extern void freeze_wake(void);

/* 
                                                       
  
                                                                           
                                                                        
                                        
 */
extern void arch_suspend_disable_irqs(void);

/* 
                                                       
  
                                                                          
                                                                        
                                        
 */
extern void arch_suspend_enable_irqs(void);

extern int pm_suspend(suspend_state_t state);
#else /*                 */
#define suspend_valid_only_mem	NULL

static inline void suspend_set_ops(const struct platform_suspend_ops *ops) {}
static inline int pm_suspend(suspend_state_t state) { return -ENOSYS; }
static inline void freeze_wake(void) {}
#endif /*                 */

/*                                                                       
                                                                            
                                          
 */
struct pbe {
	void *address;		/*                     */
	void *orig_address;	/*                            */
	struct pbe *next;
};

/*                 */
extern void mark_free_pages(struct zone *zone);

/* 
                                                                 
  
                                                                      
                                                             
  
                                                                     
  
                                                                    
                                                                   
  
                                                                           
                                                                  
  
                                                                          
                                                                     
                                         
  
                                                                            
                                                                         
                                                                          
                                                                   
                                       
  
                                                                   
                                                                     
                                                         
  
                                                                              
                          
                                                                      
                                                          
  
                                                                              
                                            
                                                                         
                                                                           
                                                   
  
                                                                             
                                                                     
                                         
  
                                                                
                                                                   
                                       
  
                                                                    
                                                                        
                                                                      
                                                                      
 */
struct platform_hibernation_ops {
	int (*begin)(void);
	void (*end)(void);
	int (*pre_snapshot)(void);
	void (*finish)(void);
	int (*prepare)(void);
	int (*enter)(void);
	void (*leave)(void);
	int (*pre_restore)(void);
	void (*restore_cleanup)(void);
	void (*recover)(void);
};

#ifdef CONFIG_HIBERNATION
/*                         */
extern void __register_nosave_region(unsigned long b, unsigned long e, int km);
static inline void __init register_nosave_region(unsigned long b, unsigned long e)
{
	__register_nosave_region(b, e, 0);
}
static inline void __init register_nosave_region_late(unsigned long b, unsigned long e)
{
	__register_nosave_region(b, e, 1);
}
extern int swsusp_page_is_forbidden(struct page *);
extern void swsusp_set_page_free(struct page *);
extern void swsusp_unset_page_free(struct page *);
extern unsigned long get_safe_page(gfp_t gfp_mask);

extern void hibernation_set_ops(const struct platform_hibernation_ops *ops);
extern int hibernate(void);
#ifdef CONFIG_MTK_HIBERNATION
extern int pre_hibernate(void);
extern int mtk_hibernate(void);
extern int mtk_hibernate_abort(void);
#endif
extern bool system_entering_hibernation(void);
#else /*                    */
static inline void register_nosave_region(unsigned long b, unsigned long e) {}
static inline void register_nosave_region_late(unsigned long b, unsigned long e) {}
static inline int swsusp_page_is_forbidden(struct page *p) { return 0; }
static inline void swsusp_set_page_free(struct page *p) {}
static inline void swsusp_unset_page_free(struct page *p) {}

static inline void hibernation_set_ops(const struct platform_hibernation_ops *ops) {}
static inline int hibernate(void) { return -ENOSYS; }
static inline bool system_entering_hibernation(void) { return false; }
#endif /*                    */

/*                                */
#define PM_HIBERNATION_PREPARE	0x0001 /*                    */
#define PM_POST_HIBERNATION	0x0002 /*                      */
#define PM_SUSPEND_PREPARE	0x0003 /*                             */
#define PM_POST_SUSPEND		0x0004 /*                  */
#define PM_RESTORE_PREPARE	0x0005 /*                                */
#define PM_POST_RESTORE		0x0006 /*                */

extern struct mutex pm_mutex;

#ifdef CONFIG_PM_SLEEP
void save_processor_state(void);
void restore_processor_state(void);

/*                     */
extern int register_pm_notifier(struct notifier_block *nb);
extern int unregister_pm_notifier(struct notifier_block *nb);

#define pm_notifier(fn, pri) {				\
	static struct notifier_block fn##_nb =			\
		{ .notifier_call = fn, .priority = pri };	\
	register_pm_notifier(&fn##_nb);			\
}

/*                             */
extern bool events_check_enabled;

extern bool pm_wakeup_pending(void);
extern bool pm_get_wakeup_count(unsigned int *count, bool block);
extern bool pm_save_wakeup_count(unsigned int count);
extern void pm_wakep_autosleep_enabled(bool set);
extern void pm_print_active_wakeup_sources(void);

static inline void lock_system_sleep(void)
{
	current->flags |= PF_FREEZER_SKIP;
	mutex_lock(&pm_mutex);
}

static inline void unlock_system_sleep(void)
{
	/*
                                                               
                         
   
           
                                                                    
                                                                
                                                      
   
                                                 
                                                            
                                                                    
                                                                   
                                                               
  */
	current->flags &= ~PF_FREEZER_SKIP;
	mutex_unlock(&pm_mutex);
}

#else /*                  */

static inline int register_pm_notifier(struct notifier_block *nb)
{
	return 0;
}

static inline int unregister_pm_notifier(struct notifier_block *nb)
{
	return 0;
}

#define pm_notifier(fn, pri)	do { (void)(fn); } while (0)

static inline bool pm_wakeup_pending(void) { return false; }

static inline void lock_system_sleep(void) {}
static inline void unlock_system_sleep(void) {}

#endif /*                  */

#ifdef CONFIG_PM_SLEEP_DEBUG
extern bool pm_print_times_enabled;
#else
#define pm_print_times_enabled	(false)
#endif

enum {
	TOI_CAN_HIBERNATE,
	TOI_CAN_RESUME,
	TOI_RESUME_DEVICE_OK,
	TOI_NORESUME_SPECIFIED,
	TOI_SANITY_CHECK_PROMPT,
	TOI_CONTINUE_REQ,
	TOI_RESUMED_BEFORE,
	TOI_BOOT_TIME,
	TOI_NOW_RESUMING,
	TOI_IGNORE_LOGLEVEL,
	TOI_TRYING_TO_RESUME,
	TOI_LOADING_ALT_IMAGE,
	TOI_STOP_RESUME,
	TOI_IO_STOPPED,
	TOI_NOTIFIERS_PREPARE,
	TOI_CLUSTER_MODE,
	TOI_BOOT_KERNEL,
};

#ifdef CONFIG_TOI

/*                        */
extern unsigned long toi_state;
#define set_toi_state(bit) (set_bit(bit, &toi_state))
#define clear_toi_state(bit) (clear_bit(bit, &toi_state))
#define test_toi_state(bit) (test_bit(bit, &toi_state))
extern int toi_running;

#define test_action_state(bit) (test_bit(bit, &toi_bkd.toi_action))
extern int try_tuxonice_hibernate(void);
#ifdef CONFIG_TOI_ENHANCE
extern int toi_abort_hibernate(void);
extern int toi_hibernate_fatalerror(void);
#endif

#else /*             */

#define toi_state		(0)
#define set_toi_state(bit) do { } while (0)
#define clear_toi_state(bit) do { } while (0)
#define test_toi_state(bit) (0)
#define toi_running (0)

static inline int try_tuxonice_hibernate(void) { return 0; }
#define test_action_state(bit) (0)
#ifdef CONFIG_TOI_ENHANCE
static inline int toi_abort_hibernate(void) { return 0; }
static inline int toi_hibernate_fatalerror(void) { return 0; }
#endif

#endif /*            */

#ifdef CONFIG_HIBERNATION
#ifdef CONFIG_TOI
extern void try_tuxonice_resume(void);
#else
#define try_tuxonice_resume() do { } while (0)
#endif

extern int resume_attempted;
extern int software_resume(void);

static inline void check_resume_attempted(void)
{
	if (resume_attempted)
		return;

	software_resume();
}
#else
#define check_resume_attempted() do { } while (0)
#define resume_attempted (0)
#endif

#ifdef CONFIG_PM_AUTOSLEEP

/*                          */
void queue_up_suspend_work(void);

#else /*                      */

static inline void queue_up_suspend_work(void) {}

#endif /*                      */

#ifdef CONFIG_ARCH_SAVE_PAGE_KEYS
/*
                                                                   
                                                                   
                                                                   
                                                                  
                            
 */
unsigned long page_key_additional_pages(unsigned long pages);
int page_key_alloc(unsigned long pages);
void page_key_free(void);
void page_key_read(unsigned long *pfn);
void page_key_memorize(unsigned long *pfn);
void page_key_write(void *address);

#else /*                             */

static inline unsigned long page_key_additional_pages(unsigned long pages)
{
	return 0;
}

static inline int  page_key_alloc(unsigned long pages)
{
	return 0;
}

static inline void page_key_free(void) {}
static inline void page_key_read(unsigned long *pfn) {}
static inline void page_key_memorize(unsigned long *pfn) {}
static inline void page_key_write(void *address) {}

#endif /*                             */

#endif /*                  */
