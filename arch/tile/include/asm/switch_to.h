/*
 * Copyright 2010 Tilera Corporation. All Rights Reserved.
 *
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public License
 *   as published by the Free Software Foundation, version 2.
 *
 *   This program is distributed in the hope that it will be useful, but
 *   WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, GOOD TITLE or
 *   NON INFRINGEMENT.  See the GNU General Public License for
 *   more details.
 */

#ifndef _ASM_TILE_SWITCH_TO_H
#define _ASM_TILE_SWITCH_TO_H

#include <arch/sim_def.h>

/*
                                                       
                                                                         
                                                                 
                                                                              
 */
#define CALLEE_SAVED_FIRST_REG 30
#define CALLEE_SAVED_REGS_COUNT 24   /*                                    */

#ifndef __ASSEMBLY__

struct task_struct;

/*
                                                                 
 */
#define prepare_arch_switch(next) _prepare_arch_switch(next)
void _prepare_arch_switch(struct task_struct *next);

struct task_struct;
#define switch_to(prev, next, last) ((last) = _switch_to((prev), (next)))
extern struct task_struct *_switch_to(struct task_struct *prev,
				      struct task_struct *next);

/*                                   */
extern struct task_struct *__switch_to(struct task_struct *prev,
				       struct task_struct *next,
				       unsigned long new_system_save_k_0);

/*                                               */
extern unsigned long get_switch_to_pc(void);

/*
                                                                
                                                             
                                                            
 */
#define finish_arch_switch(prev) do {                                     \
	if (unlikely((prev)->state == TASK_DEAD))                         \
		__insn_mtspr(SPR_SIM_CONTROL, SIM_CONTROL_OS_EXIT |       \
			((prev)->pid << _SIM_CONTROL_OPERATOR_BITS));     \
	__insn_mtspr(SPR_SIM_CONTROL, SIM_CONTROL_OS_SWITCH |             \
		(current->pid << _SIM_CONTROL_OPERATOR_BITS));            \
	if (current->mm == NULL && !kstack_hash &&                        \
	    current_thread_info()->homecache_cpu != smp_processor_id())   \
		homecache_migrate_kthread();                              \
} while (0)

/*                                          */
void ret_from_fork(void);

/*                                                   */
void ret_from_kernel_thread(void *fn, void *arg);

/*                                                          */
struct task_struct *sim_notify_fork(struct task_struct *prev);

#endif /*               */

#endif /*                       */