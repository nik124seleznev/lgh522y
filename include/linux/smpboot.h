#ifndef _LINUX_SMPBOOT_H
#define _LINUX_SMPBOOT_H

#include <linux/types.h>

struct task_struct;
/*                               */
struct smpboot_thread_data;

/* 
                                                                    
                                                            
                                        
                                                                              
                         
                                              
                                                                 
                                                 
                                                                
                               
                                                               
                              
                                                            
                         
                                                                
                          
                                                                      
                                                        
                                                     
                                                           
                                            
 */
struct smp_hotplug_thread {
	struct task_struct __percpu	**store;
	struct list_head		list;
	int				(*thread_should_run)(unsigned int cpu);
	void				(*thread_fn)(unsigned int cpu);
	void				(*create)(unsigned int cpu);
	void				(*setup)(unsigned int cpu);
	void				(*cleanup)(unsigned int cpu, bool online);
	void				(*park)(unsigned int cpu);
	void				(*unpark)(unsigned int cpu);
	void				(*pre_unpark)(unsigned int cpu);
	bool				selfparking;
	const char			*thread_comm;
};

int smpboot_register_percpu_thread(struct smp_hotplug_thread *plug_thread);
void smpboot_unregister_percpu_thread(struct smp_hotplug_thread *plug_thread);
int smpboot_thread_schedule(void);

#endif
