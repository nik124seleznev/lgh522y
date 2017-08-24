/*
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 *
 * Copyright (C) 2012 Thomas Langer <thomas.langer@lantiq.com>
 * Copyright (C) 2012 John Crispin <blogic@openwrt.org>
 */

#include <linux/init.h>
#include <linux/io.h>
#include <linux/pm.h>
#include <asm/reboot.h>
#include <linux/export.h>

#include <lantiq_soc.h>

/*                            */
#define SYS1_CPU0RS		0x0040
/*                  */
#define CPU0RS_MASK		0x0003
/*                         */
#define SYS1_BM			0x00a0
/*                */
#define BM_MASK			0x0005

/*                                                           */
unsigned char ltq_boot_select(void)
{
	return ltq_sys1_r32(SYS1_BM) & BM_MASK;
}

/*                                                                */
int ltq_reset_cause(void)
{
	return ltq_sys1_r32(SYS1_CPU0RS) & CPU0RS_MASK;
}
EXPORT_SYMBOL_GPL(ltq_reset_cause);

#define BOOT_REG_BASE	(KSEG1 | 0x1F200000)
#define BOOT_PW1_REG	(BOOT_REG_BASE | 0x20)
#define BOOT_PW2_REG	(BOOT_REG_BASE | 0x24)
#define BOOT_PW1	0x4C545100
#define BOOT_PW2	0x0051544C

#define WDT_REG_BASE	(KSEG1 | 0x1F8803F0)
#define WDT_PW1		0x00BE0000
#define WDT_PW2		0x00DC0000

static void machine_restart(char *command)
{
	local_irq_disable();

	/*              */
	ltq_w32(BOOT_PW1, (void *)BOOT_PW1_REG); /*         */
	ltq_w32(BOOT_PW2, (void *)BOOT_PW2_REG); /*         */
	ltq_w32(0, (void *)BOOT_REG_BASE); /*                    */

	/*                */
	ltq_w32(WDT_PW1, (void *)WDT_REG_BASE);
	ltq_w32(WDT_PW2 |
		(0x3 << 26) | /*     */
		(0x2 << 24) | /*        */
		(0x1 << 31) | /*        */
		(1), /*        */
		(void *)WDT_REG_BASE);
	unreachable();
}

static void machine_halt(void)
{
	local_irq_disable();
	unreachable();
}

static void machine_power_off(void)
{
	local_irq_disable();
	unreachable();
}

static int __init mips_reboot_setup(void)
{
	_machine_restart = machine_restart;
	_machine_halt = machine_halt;
	pm_power_off = machine_power_off;
	return 0;
}

arch_initcall(mips_reboot_setup);
