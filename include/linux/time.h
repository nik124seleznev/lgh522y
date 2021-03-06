#ifndef _LINUX_TIME_H
#define _LINUX_TIME_H

# include <linux/cache.h>
# include <linux/seqlock.h>
# include <linux/math64.h>
#include <uapi/linux/time.h>

extern struct timezone sys_tz;

/*                                                 */
#define MSEC_PER_SEC	1000L
#define USEC_PER_MSEC	1000L
#define NSEC_PER_USEC	1000L
#define NSEC_PER_MSEC	1000000L
#define USEC_PER_SEC	1000000L
#define NSEC_PER_SEC	1000000000L
#define FSEC_PER_SEC	1000000000000000LL

#define TIME_T_MAX	(time_t)((1UL << ((sizeof(time_t) << 3) - 1)) - 1)

static inline int timespec_equal(const struct timespec *a,
                                 const struct timespec *b)
{
	return (a->tv_sec == b->tv_sec) && (a->tv_nsec == b->tv_nsec);
}

/*
                        
                       
                        
 */
static inline int timespec_compare(const struct timespec *lhs, const struct timespec *rhs)
{
	if (lhs->tv_sec < rhs->tv_sec)
		return -1;
	if (lhs->tv_sec > rhs->tv_sec)
		return 1;
	return lhs->tv_nsec - rhs->tv_nsec;
}

static inline int timeval_compare(const struct timeval *lhs, const struct timeval *rhs)
{
	if (lhs->tv_sec < rhs->tv_sec)
		return -1;
	if (lhs->tv_sec > rhs->tv_sec)
		return 1;
	return lhs->tv_usec - rhs->tv_usec;
}

extern unsigned long mktime(const unsigned int year, const unsigned int mon,
			    const unsigned int day, const unsigned int hour,
			    const unsigned int min, const unsigned int sec);

extern void set_normalized_timespec(struct timespec *ts, time_t sec, s64 nsec);

/*
                                                                
                                                                 
                                        
 */
extern struct timespec timespec_add_safe(const struct timespec lhs,
					 const struct timespec rhs);


static inline struct timespec timespec_add(struct timespec lhs,
						struct timespec rhs)
{
	struct timespec ts_delta;
	set_normalized_timespec(&ts_delta, lhs.tv_sec + rhs.tv_sec,
				lhs.tv_nsec + rhs.tv_nsec);
	return ts_delta;
}

/*
                                      
 */
static inline struct timespec timespec_sub(struct timespec lhs,
						struct timespec rhs)
{
	struct timespec ts_delta;
	set_normalized_timespec(&ts_delta, lhs.tv_sec - rhs.tv_sec,
				lhs.tv_nsec - rhs.tv_nsec);
	return ts_delta;
}

#define KTIME_MAX			((s64)~((u64)1 << 63))
#if (BITS_PER_LONG == 64)
# define KTIME_SEC_MAX			(KTIME_MAX / NSEC_PER_SEC)
#else
# define KTIME_SEC_MAX			LONG_MAX
#endif

/*
                                                         
 */
static inline bool timespec_valid(const struct timespec *ts)
{
	/*                             */
	if (ts->tv_sec < 0)
		return false;
	/*                                           */
	if ((unsigned long)ts->tv_nsec >= NSEC_PER_SEC)
		return false;
	return true;
}

static inline bool timespec_valid_strict(const struct timespec *ts)
{
	if (!timespec_valid(ts))
		return false;
	/*                                             */
	if ((unsigned long long)ts->tv_sec >= KTIME_SEC_MAX)
		return false;
	return true;
}

extern bool persistent_clock_exist;

static inline bool has_persistent_clock(void)
{
	return persistent_clock_exist;
}

extern void read_persistent_clock(struct timespec *ts);
extern void read_boot_clock(struct timespec *ts);
extern int persistent_clock_is_local;
extern int update_persistent_clock(struct timespec now);
void timekeeping_init(void);
extern int timekeeping_suspended;

unsigned long get_seconds(void);
struct timespec current_kernel_time(void);
struct timespec __current_kernel_time(void); /*                          */
struct timespec get_monotonic_coarse(void);
void get_xtime_and_monotonic_and_sleep_offset(struct timespec *xtim,
				struct timespec *wtom, struct timespec *sleep);
void timekeeping_inject_sleeptime(struct timespec *delta);

#define CURRENT_TIME		(current_kernel_time())
#define CURRENT_TIME_SEC	((struct timespec) { get_seconds(), 0 })

/*                                                        
                                                          
                                                            
                                                                
                                                                    
                                                               
                                                                
                                 
 */
#ifdef CONFIG_ARCH_USES_GETTIMEOFFSET
extern u32 (*arch_gettimeoffset)(void);
#endif

extern void do_gettimeofday(struct timeval *tv);
extern int do_settimeofday(const struct timespec *tv);
extern int do_sys_settimeofday(const struct timespec *tv,
			       const struct timezone *tz);
#define do_posix_clock_monotonic_gettime(ts) ktime_get_ts(ts)
extern long do_utimes(int dfd, const char __user *filename, struct timespec *times, int flags);
struct itimerval;
extern int do_setitimer(int which, struct itimerval *value,
			struct itimerval *ovalue);
extern unsigned int alarm_setitimer(unsigned int seconds);
extern int do_getitimer(int which, struct itimerval *value);
extern int __getnstimeofday(struct timespec *tv);
extern void getnstimeofday(struct timespec *tv);
extern void getrawmonotonic(struct timespec *ts);
extern void getnstime_raw_and_real(struct timespec *ts_raw,
		struct timespec *ts_real);
extern void getboottime(struct timespec *ts);
extern void monotonic_to_bootbased(struct timespec *ts);
extern void get_monotonic_boottime(struct timespec *ts);

static inline bool timeval_valid(const struct timeval *tv)
{
	/* Dates before 1970 are bogus */
	if (tv->tv_sec < 0)
		return false;

	/* Can't have more microseconds then a second */
	if (tv->tv_usec < 0 || tv->tv_usec >= USEC_PER_SEC)
		return false;

	return true;
}

extern struct timespec timespec_trunc(struct timespec t, unsigned gran);
extern int timekeeping_valid_for_hres(void);
extern u64 timekeeping_max_deferment(void);
extern int timekeeping_inject_offset(struct timespec *ts);
extern s32 timekeeping_get_tai_offset(void);
extern void timekeeping_set_tai_offset(s32 tai_offset);
extern void timekeeping_clocktai(struct timespec *ts);

struct tms;
extern void do_sys_times(struct tms *);

/*
                                                                             
                                            
 */
struct tm {
	/*
                                                                 
                                                          
  */
	int tm_sec;
	/*                                                           */
	int tm_min;
	/*                                                         */
	int tm_hour;
	/*                                            */
	int tm_mday;
	/*                                                          */
	int tm_mon;
	/*                                */
	long tm_year;
	/*                                                      */
	int tm_wday;
	/*                                                           */
	int tm_yday;
};

void time_to_tm(time_t totalsecs, int offset, struct tm *result);

/* 
                                                   
                                                         
  
                                                               
             
 */
static inline s64 timespec_to_ns(const struct timespec *ts)
{
	return ((s64) ts->tv_sec * NSEC_PER_SEC) + ts->tv_nsec;
}

/* 
                                                 
                                                        
  
                                                              
             
 */
static inline s64 timeval_to_ns(const struct timeval *tv)
{
	return ((s64) tv->tv_sec * NSEC_PER_SEC) +
		tv->tv_usec * NSEC_PER_USEC;
}

/* 
                                                   
                                               
  
                                                             
 */
extern struct timespec ns_to_timespec(const s64 nsec);

/* 
                                                 
                                               
  
                                                            
 */
extern struct timeval ns_to_timeval(const s64 nsec);

/* 
                                                   
                                             
                                               
  
                                                                     
                                            
 */
static __always_inline void timespec_add_ns(struct timespec *a, u64 ns)
{
	a->tv_sec += __iter_div_u64_rem(a->tv_nsec + ns, NSEC_PER_SEC, &ns);
	a->tv_nsec = ns;
}

#endif
