/*
 * Copyright (C) 2013 ARM Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef __ASM_CPU_OPS_H
#define __ASM_CPU_OPS_H

#include <linux/init.h>
#include <linux/threads.h>

struct device_node;

/* 
                                                                    
  
                                                                    
                           
                                                                            
                                                             
                                                                         
                                                                
                  
                                          
                                                                       
                                                      
                                                                            
                                                                  
                               
                                                                         
                     
                                                                          
                                                                              
                                                                         
                                                                        
 */
struct cpu_operations {
	const char	*name;
	int		(*cpu_init)(struct device_node *, unsigned int);
	int		(*cpu_prepare)(unsigned int);
	int		(*cpu_boot)(unsigned int);
	void		(*cpu_postboot)(void);
#ifdef CONFIG_HOTPLUG_CPU
	int		(*cpu_disable)(unsigned int cpu);
	void		(*cpu_die)(unsigned int cpu);
	int		(*cpu_kill)(unsigned int cpu);
#endif
#ifdef CONFIG_ARM64_CPU_SUSPEND
	int		(*cpu_suspend)(unsigned long);
#endif
};

extern const struct cpu_operations *cpu_ops[NR_CPUS];
extern int __init cpu_read_ops(struct device_node *dn, int cpu);
extern void __init cpu_read_bootcpu_ops(void);

#endif /*                        */
