/*
 * Meta performance counter support.
 *  Copyright (C) 2012 Imagination Technologies Ltd
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 */

#ifndef METAG_PERF_EVENT_H_
#define METAG_PERF_EVENT_H_

#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/perf_event.h>

/*                                     */
#include <asm/metag_mem.h>

/*
                                                                            
                                                                         
  
                                                                             
                                                                           
                                                                          
                                                                           
                                                      
  
                                                                           
                                                    
  
                                                                        
                                                 
 */

#define MAX_HWEVENTS		3
#define MAX_PERIOD		((1UL << 24) - 1)
#define METAG_INST_COUNTER	(MAX_HWEVENTS - 1)

/* 
                                                               
                                                             
                                           
                                 
  
                                                                  
                               
 */
struct cpu_hw_events {
	struct perf_event	*events[MAX_HWEVENTS];
	unsigned long		used_mask[BITS_TO_LONGS(MAX_HWEVENTS)];
	raw_spinlock_t		pmu_lock;
};

/* 
                                            
                            
                   
                          
                                           
                             
                               
                                      
                                      
                                                    
                                                                
                                                             
                                                                     
                                            
                                            
  
                                                                         
              
 */
struct metag_pmu {
	struct pmu	pmu;
	const char	*name;
	u32		version;
	irqreturn_t	(*handle_irq)(int irq_num, void *dev);
	void		(*enable)(struct hw_perf_event *evt, int idx);
	void		(*disable)(struct hw_perf_event *evt, int idx);
	u64		(*read)(int idx);
	void		(*write)(int idx, u32 val);
	int		(*event_map)(int idx);
	const int	(*cache_events)[PERF_COUNT_HW_CACHE_MAX]
				[PERF_COUNT_HW_CACHE_OP_MAX]
				[PERF_COUNT_HW_CACHE_RESULT_MAX];
	u32		max_period;
	int		max_events;
	atomic_t	active_events;
	struct mutex	reserve_mutex;
};

/*                                                    */
/*                                   */
#define PERF_COUNT(x)	(PERF_COUNT0 + (sizeof(u64) * (x)))
#define PERF_ICORE(x)	(PERF_ICORE0 + (sizeof(u64) * (x)))
#define PERF_CHAN(x)	(PERF_CHAN0 + (sizeof(u64) * (x)))

/*                    */
#define C(x) PERF_COUNT_HW_CACHE_##x
#define CACHE_OP_UNSUPPORTED	0xfffe
#define CACHE_OP_NONSENSE	0xffff

#endif
