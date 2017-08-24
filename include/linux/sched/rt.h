#ifndef _SCHED_RT_H
#define _SCHED_RT_H

/*
                                                          
                                                             
                                                           
                                                                  
  
                                                       
                                                        
                                                       
                                                       
                                                         
 */

#define MAX_USER_RT_PRIO	100
#define MAX_RT_PRIO		MAX_USER_RT_PRIO

#define MAX_PRIO		(MAX_RT_PRIO + 40)
#define DEFAULT_PRIO		(MAX_RT_PRIO + 20)

static inline int rt_prio(int prio)
{
	if (unlikely(prio < MAX_RT_PRIO))
		return 1;
	return 0;
}

static inline int rt_task(struct task_struct *p)
{
	return rt_prio(p->prio);
}

#ifdef CONFIG_RT_MUTEXES
extern int rt_mutex_getprio(struct task_struct *p);
extern void rt_mutex_setprio(struct task_struct *p, int prio);
extern void rt_mutex_adjust_pi(struct task_struct *p);
static inline bool tsk_is_pi_blocked(struct task_struct *tsk)
{
	return tsk->pi_blocked_on != NULL;
}
#else
static inline int rt_mutex_getprio(struct task_struct *p)
{
	return p->normal_prio;
}
# define rt_mutex_adjust_pi(p)		do { } while (0)
static inline bool tsk_is_pi_blocked(struct task_struct *tsk)
{
	return false;
}
#endif

extern void normalize_rt_tasks(void);


/*
                                                                 
                                             
 */
#define RR_TIMESLICE		(100 * HZ / 1000)

#endif /*             */
