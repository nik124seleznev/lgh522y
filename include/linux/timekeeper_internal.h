/*
                                                          
                                              
 */

#ifndef _LINUX_TIMEKEEPER_INTERNAL_H
#define _LINUX_TIMEKEEPER_INTERNAL_H

#include <linux/clocksource.h>
#include <linux/jiffies.h>
#include <linux/time.h>

/*                                                */
struct timekeeper {
	/*                                           */
	struct clocksource	*clock;
	/*                               */
	u32			mult;
	/*                                             */
	u32			shift;
	/*                                             */
	cycle_t			cycle_interval;
	/*                                                     */
	cycle_t			cycle_last;
	/*                                                           */
	u64			xtime_interval;
	/*                                                             */
	s64			xtime_remainder;
	/*                                                */
	u32			raw_interval;

	/*                                        */
	u64			xtime_sec;
	/*                            */
	u64			xtime_nsec;

	/*                                                        
                          */
	s64			ntp_error;
	/*                                                        
                              */
	u32			ntp_error_shift;

	/*
                                                                         
                                                                        
                                                                       
                                                                        
                            
   
                                                                
                                                                  
                                                             
   
                                                                       
                 
  */
	struct timespec		wall_to_monotonic;
	/*                                          */
	ktime_t			offs_real;
	/*                       */
	struct timespec		total_sleep_time;
	/*                                          */
	ktime_t			offs_boot;
	/*                                                                 */
	struct timespec		raw_time;
	/*                                          */
	s32			tai_offset;
	/*                                     */
	ktime_t			offs_tai;

};

static inline struct timespec tk_xtime(struct timekeeper *tk)
{
	struct timespec ts;

	ts.tv_sec = tk->xtime_sec;
	ts.tv_nsec = (long)(tk->xtime_nsec >> tk->shift);
	return ts;
}


#ifdef CONFIG_GENERIC_TIME_VSYSCALL

extern void update_vsyscall(struct timekeeper *tk);
extern void update_vsyscall_tz(void);

#elif defined(CONFIG_GENERIC_TIME_VSYSCALL_OLD)

extern void update_vsyscall_old(struct timespec *ts, struct timespec *wtm,
				struct clocksource *c, u32 mult);
extern void update_vsyscall_tz(void);

static inline void update_vsyscall(struct timekeeper *tk)
{
	struct timespec xt;

	xt = tk_xtime(tk);
	update_vsyscall_old(&xt, &tk->wall_to_monotonic, tk->clock, tk->mult);
}

#else

static inline void update_vsyscall(struct timekeeper *tk)
{
}
static inline void update_vsyscall_tz(void)
{
}
#endif

#endif /*                              */
