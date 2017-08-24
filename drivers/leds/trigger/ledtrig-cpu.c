/*
 * ledtrig-cpu.c - LED trigger based on CPU activity
 *
 * This LED trigger will be registered for each possible CPU and named as
 * cpu0, cpu1, cpu2, cpu3, etc.
 *
 * It can be bound to any LED just like other triggers using either a
 * board file or via sysfs interface.
 *
 * An API named ledtrig_cpu is exported for any user, who want to add CPU
 * activity indication in their code
 *
 * Copyright 2011 Linus Walleij <linus.walleij@linaro.org>
 * Copyright 2011 - 2012 Bryan Wu <bryan.wu@canonical.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/percpu.h>
#include <linux/syscore_ops.h>
#include <linux/rwsem.h>
#include "../leds.h"

#define MAX_NAME_LEN	8

struct led_trigger_cpu {
	char name[MAX_NAME_LEN];
	struct led_trigger *_trig;
};

static DEFINE_PER_CPU(struct led_trigger_cpu, cpu_trig);

/* 
                                              
                                
  
                                                       
                                     
 */
void ledtrig_cpu(enum cpu_led_event ledevt)
{
	struct led_trigger_cpu *trig = &__get_cpu_var(cpu_trig);

	/*                            */
	switch (ledevt) {
	case CPU_LED_IDLE_END:
	case CPU_LED_START:
		/*                                      */
		led_trigger_event(trig->_trig, LED_FULL);
		break;

	case CPU_LED_IDLE_START:
	case CPU_LED_STOP:
	case CPU_LED_HALTED:
		/*                       */
		led_trigger_event(trig->_trig, LED_OFF);
		break;

	default:
		/*                             */
		break;
	}
}
EXPORT_SYMBOL(ledtrig_cpu);

static int ledtrig_cpu_syscore_suspend(void)
{
	ledtrig_cpu(CPU_LED_STOP);
	return 0;
}

static void ledtrig_cpu_syscore_resume(void)
{
	ledtrig_cpu(CPU_LED_START);
}

static void ledtrig_cpu_syscore_shutdown(void)
{
	ledtrig_cpu(CPU_LED_HALTED);
}

static struct syscore_ops ledtrig_cpu_syscore_ops = {
	.shutdown	= ledtrig_cpu_syscore_shutdown,
	.suspend	= ledtrig_cpu_syscore_suspend,
	.resume		= ledtrig_cpu_syscore_resume,
};

static int __init ledtrig_cpu_init(void)
{
	int cpu;

	/*                               */
	BUILD_BUG_ON(CONFIG_NR_CPUS > 9999);

	/*
                                                      
                                                         
                           
  */
	for_each_possible_cpu(cpu) {
		struct led_trigger_cpu *trig = &per_cpu(cpu_trig, cpu);

		snprintf(trig->name, MAX_NAME_LEN, "cpu%d", cpu);

		led_trigger_register_simple(trig->name, &trig->_trig);
	}

	register_syscore_ops(&ledtrig_cpu_syscore_ops);

	pr_info("ledtrig-cpu: registered to indicate activity on CPUs\n");

	return 0;
}
module_init(ledtrig_cpu_init);

static void __exit ledtrig_cpu_exit(void)
{
	int cpu;

	for_each_possible_cpu(cpu) {
		struct led_trigger_cpu *trig = &per_cpu(cpu_trig, cpu);

		led_trigger_unregister_simple(trig->_trig);
		trig->_trig = NULL;
		memset(trig->name, 0, MAX_NAME_LEN);
	}

	unregister_syscore_ops(&ledtrig_cpu_syscore_ops);
}
module_exit(ledtrig_cpu_exit);

MODULE_AUTHOR("Linus Walleij <linus.walleij@linaro.org>");
MODULE_AUTHOR("Bryan Wu <bryan.wu@canonical.com>");
MODULE_DESCRIPTION("CPU LED trigger");
MODULE_LICENSE("GPL");