/*
 * MIPS idle loop and WAIT instruction support.
 *
 * Copyright (C) xxxx  the Anonymous
 * Copyright (C) 1994 - 2006 Ralf Baechle
 * Copyright (C) 2003, 2004  Maciej W. Rozycki
 * Copyright (C) 2001, 2004, 2011, 2012	 MIPS Technologies, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#include <linux/export.h>
#include <linux/init.h>
#include <linux/irqflags.h>
#include <linux/printk.h>
#include <linux/sched.h>
#include <asm/cpu.h>
#include <asm/cpu-info.h>
#include <asm/idle.h>
#include <asm/mipsregs.h>

/*
                                                                            
                                                                              
                                                            
                                                                               
                     
 */
void (*cpu_wait)(void);
EXPORT_SYMBOL(cpu_wait);

static void r3081_wait(void)
{
	unsigned long cfg = read_c0_conf();
	write_c0_conf(cfg | R30XX_CONF_HALT);
	local_irq_enable();
}

static void r39xx_wait(void)
{
	if (!need_resched())
		write_c0_conf(read_c0_conf() | TX39_CONF_HALT);
	local_irq_enable();
}

void r4k_wait(void)
{
	local_irq_enable();
	__r4k_wait();
}

/*
                                                                            
                                                                       
                                                                            
                                                                              
                               
 */
void r4k_wait_irqoff(void)
{
	if (!need_resched())
		__asm__(
		"	.set	push		\n"
		"	.set	mips3		\n"
		"	wait			\n"
		"	.set	pop		\n");
	local_irq_enable();
	__asm__(
	"	.globl __pastwait	\n"
	"__pastwait:			\n");
}

/*
                                                                         
                                                                 
 */
static void rm7k_wait_irqoff(void)
{
	if (!need_resched())
		__asm__(
		"	.set	push					\n"
		"	.set	mips3					\n"
		"	.set	noat					\n"
		"	mfc0	$1, $12					\n"
		"	sync						\n"
		"	mtc0	$1, $12		# stalls until W stage	\n"
		"	wait						\n"
		"	mtc0	$1, $12		# stalls until W stage	\n"
		"	.set	pop					\n");
	local_irq_enable();
}

/*
                                                                     
                                                                         
                                                                             
 */
static void au1k_wait(void)
{
	unsigned long c0status = read_c0_status() | 1;	/*         */

	__asm__(
	"	.set	mips3			\n"
	"	cache	0x14, 0(%0)		\n"
	"	cache	0x14, 32(%0)		\n"
	"	sync				\n"
	"	mtc0	%1, $12			\n" /*             */
	"	wait				\n"
	"	nop				\n"
	"	nop				\n"
	"	nop				\n"
	"	nop				\n"
	"	.set	mips0			\n"
	: : "r" (au1k_wait), "r" (c0status));
}

static int __initdata nowait;

static int __init wait_disable(char *s)
{
	nowait = 1;

	return 1;
}

__setup("nowait", wait_disable);

void __init check_wait(void)
{
	struct cpuinfo_mips *c = &current_cpu_data;

	if (nowait) {
		printk("Wait instruction disabled.\n");
		return;
	}

	switch (c->cputype) {
	case CPU_R3081:
	case CPU_R3081E:
		cpu_wait = r3081_wait;
		break;
	case CPU_TX3927:
		cpu_wait = r39xx_wait;
		break;
	case CPU_R4200:
/*                 */
	case CPU_R4600:
	case CPU_R4640:
	case CPU_R4650:
	case CPU_R4700:
	case CPU_R5000:
	case CPU_R5500:
	case CPU_NEVADA:
	case CPU_4KC:
	case CPU_4KEC:
	case CPU_4KSC:
	case CPU_5KC:
	case CPU_25KF:
	case CPU_PR4450:
	case CPU_BMIPS3300:
	case CPU_BMIPS4350:
	case CPU_BMIPS4380:
	case CPU_BMIPS5000:
	case CPU_CAVIUM_OCTEON:
	case CPU_CAVIUM_OCTEON_PLUS:
	case CPU_CAVIUM_OCTEON2:
	case CPU_JZRISC:
	case CPU_LOONGSON1:
	case CPU_XLR:
	case CPU_XLP:
		cpu_wait = r4k_wait;
		break;

	case CPU_RM7000:
		cpu_wait = rm7k_wait_irqoff;
		break;

	case CPU_M14KC:
	case CPU_M14KEC:
	case CPU_24K:
	case CPU_34K:
	case CPU_1004K:
		cpu_wait = r4k_wait;
		if (read_c0_config7() & MIPS_CONF7_WII)
			cpu_wait = r4k_wait_irqoff;
		break;

	case CPU_74K:
		cpu_wait = r4k_wait;
		if ((c->processor_id & 0xff) >= PRID_REV_ENCODE_332(2, 1, 0))
			cpu_wait = r4k_wait_irqoff;
		break;

	case CPU_TX49XX:
		cpu_wait = r4k_wait_irqoff;
		break;
	case CPU_ALCHEMY:
		cpu_wait = au1k_wait;
		break;
	case CPU_20KC:
		/*
                                           
                                       
                                   
   */
		if ((c->processor_id & 0xff) <= 0x64)
			break;

		/*
                                                           
                                                              
                                                           
                                                         
                                               
                         
   */
		break;
	case CPU_RM9000:
		if ((c->processor_id & 0x00ff) >= 0x40)
			cpu_wait = r4k_wait;
		break;
	default:
		break;
	}
}

static void smtc_idle_hook(void)
{
#ifdef CONFIG_MIPS_MT_SMTC
	void smtc_idle_loop_hook(void);

	smtc_idle_loop_hook();
#endif
}

void arch_cpu_idle(void)
{
	smtc_idle_hook();
	if (cpu_wait)
		cpu_wait();
	else
		local_irq_enable();
}
