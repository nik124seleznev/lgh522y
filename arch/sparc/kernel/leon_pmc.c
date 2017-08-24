/* leon_pmc.c: LEON Power-down cpu_idle() handler
 *
 * Copyright (C) 2011 Daniel Hellstrom (daniel@gaisler.com) Aeroflex Gaisler AB
 */

#include <linux/init.h>
#include <linux/pm.h>

#include <asm/leon_amba.h>
#include <asm/cpu_type.h>
#include <asm/leon.h>
#include <asm/processor.h>

/*                                                                            */
unsigned int pmc_leon_fixup_ids[] = {
	AEROFLEX_UT699,
	GAISLER_GR712RC,
	LEON4_NEXTREME1,
	0
};

int pmc_leon_need_fixup(void)
{
	unsigned int systemid = amba_system_id >> 16;
	unsigned int *id;

	id = &pmc_leon_fixup_ids[0];
	while (*id != 0) {
		if (*id == systemid)
			return 1;
		id++;
	}

	return 0;
}

/*
                                                                       
                                      
 */
void pmc_leon_idle_fixup(void)
{
	/*                                                           
                                                              
                                                                
                                                                 
                                                                 
  */
	register unsigned int address = (unsigned int)leon3_irqctrl_regs;

	/*                                                   */
	local_irq_enable();

	__asm__ __volatile__ (
		"wr	%%g0, %%asr19\n"
		"lda	[%0] %1, %%g0\n"
		:
		: "r"(address), "i"(ASI_LEON_BYPASS));
}

/*
                             
                                      
 */
void pmc_leon_idle(void)
{
	/*                                                   */
	local_irq_enable();

	/*                                                    */
	__asm__ __volatile__ ("wr	%g0, %asr19\n\t");
}

/*                                  */
static int __init leon_pmc_install(void)
{
	if (sparc_cpu_model == sparc_leon) {
		/*                                      */
		if (pmc_leon_need_fixup())
			sparc_idle = pmc_leon_idle_fixup;
		else
			sparc_idle = pmc_leon_idle;

		printk(KERN_INFO "leon: power management initialized\n");
	}

	return 0;
}

/*                                                            
                       
 */
late_initcall(leon_pmc_install);
