/*
                                                               
                                                             
                        
 */
SCHED_FEAT(GENTLE_FAIR_SLEEPERS, true)

/*
                                                                   
        
 */
SCHED_FEAT(START_DEBIT, true)

/*
                                                               
                                                                
                                     
 */
SCHED_FEAT(NEXT_BUDDY, false)

/*
                                                         
                                                                   
                  
 */
SCHED_FEAT(LAST_BUDDY, true)

/*
                                                                  
                                                             
 */
SCHED_FEAT(CACHE_HOT_BUDDY, true)

/*
                                                    
 */
SCHED_FEAT(WAKEUP_PREEMPTION, true)

/*
                                         
 */
SCHED_FEAT(ARCH_POWER, true)

SCHED_FEAT(HRTICK, false)
SCHED_FEAT(DOUBLE_TICK, false)
SCHED_FEAT(LB_BIAS, true)

/*
                                                            
 */
SCHED_FEAT(NONTASK_POWER, true)

/*
                                                          
                                                                
 */
SCHED_FEAT(TTWU_QUEUE, true)

SCHED_FEAT(FORCE_SD_OVERLAP, false)
SCHED_FEAT(RT_RUNTIME_SHARE, true)
SCHED_FEAT(LB_MIN, false)

/*
                                                                    
                                                                 
                                                                   
                                    
 */
#ifdef CONFIG_NUMA_BALANCING
SCHED_FEAT(NUMA,	false)
SCHED_FEAT(NUMA_FORCE,	false)
#endif
