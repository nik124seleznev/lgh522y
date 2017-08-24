/*
 * lppaca.h
 * Copyright (C) 2001  Mike Corrigan IBM Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */
#ifndef _ASM_POWERPC_LPPACA_H
#define _ASM_POWERPC_LPPACA_H
#ifdef __KERNEL__

/*
                                                                     
                          
 */
#ifdef CONFIG_PPC_BOOK3S

/*
                                                                  
                         
 */
#include <linux/cache.h>
#include <linux/threads.h>
#include <asm/types.h>
#include <asm/mmu.h>

/*
                                                              
                                                  
 */
#define NR_LPPACAS	1

/*
                                                                    
                                          
 */
struct lppaca {
	/*                                     */

	u32	desc;			/*                        */
	u16	size;			/*                     */
	u16	reserved1;
	u16	reserved2:14;
	u8	shared_proc:1;		/*                            */
	u8	secondary_thread:1;	/*                            */
	u8	reserved3[14];
	volatile u32 dyn_hw_node_id;	/*                          */
	volatile u32 dyn_hw_proc_id;	/*                          */
	u8	reserved4[56];
	volatile u8 vphn_assoc_counts[8]; /*                             */
					  /*                               */
	u8	reserved5[32];

	/*                                            */

	u8	reserved6[48];
	u8	cede_latency_hint;
	u8	reserved7[7];
	u8	dtl_enable_mask;	/*                         */
	u8	donate_dedicated_cpu;	/*                             */
	u8	fpregs_in_use;
	u8	pmcregs_in_use;
	u8	reserved8[28];
	u64	wait_state_cycles;	/*                           */
	u8	reserved9[28];
	u16	slb_count;		/*                       */
	u8	idle;			/*                     */
	u8	vmxregs_in_use;

	/*                                             */

	/*
                                                                    
                                                                
                                                                     
                        
                                                                     
                                                                      
  */
	volatile u32 yield_count;
	volatile u32 dispersion_count;	/*                               */
	volatile u64 cmo_faults;	/*                      */
	volatile u64 cmo_fault_time;	/*                     */
	u8	reserved10[104];

	/*               */

	u32	page_ins;		/*                             */
	u8	reserved11[148];
	volatile u64 dtl_idx;		/*                               */
	u8	reserved12[96];
} __attribute__((__aligned__(0x400)));

extern struct lppaca lppaca[];

#define lppaca_of(cpu)	(*paca[cpu].lppaca_ptr)

/*
                                                                     
                                                                    
                                                     
 */
struct slb_shadow {
	u32	persistent;		/*                           */
	u32	buffer_length;		/*                            */
	u64	reserved;
	struct	{
		u64     esid;
		u64	vsid;
	} save_area[SLB_NUM_BOLTED];
} ____cacheline_aligned;

extern struct slb_shadow slb_shadow[];

/*
                                                                   
 */
struct dtl_entry {
	u8	dispatch_reason;
	u8	preempt_reason;
	u16	processor_id;
	u32	enqueue_to_dispatch_time;
	u32	ready_to_enqueue_time;
	u32	waiting_to_ready_time;
	u64	timebase;
	u64	fault_addr;
	u64	srr0;
	u64	srr1;
};

#define DISPATCH_LOG_BYTES	4096	/*               */
#define N_DISPATCH_LOG		(DISPATCH_LOG_BYTES / sizeof(struct dtl_entry))

extern struct kmem_cache *dtl_cache;

/*
                                                                               
                                                                       
                                                                   
                                                      
 */
extern void (*dtl_consumer)(struct dtl_entry *entry, u64 index);

#endif /*                   */
#endif /*            */
#endif /*                       */
