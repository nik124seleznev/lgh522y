/*
 * Copyright (C) ST-Ericsson SA 2010
 *
 * Author: Rabin Vincent <rabin.vincent@stericsson.com> for ST-Ericsson
 * License terms: GNU General Public License (GPL) version 2
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>

#include <asm/cputype.h>
#include <asm/tlbflush.h>
#include <asm/cacheflush.h>
#include <asm/mach/map.h>

#include "setup.h"

#include "db8500-regs.h"
#include "id.h"

struct dbx500_asic_id dbx500_id;

static unsigned int ux500_read_asicid(phys_addr_t addr)
{
	phys_addr_t base = addr & ~0xfff;
	struct map_desc desc = {
		.virtual	= UX500_VIRT_ROM,
		.pfn		= __phys_to_pfn(base),
		.length		= SZ_16K,
		.type		= MT_DEVICE,
	};

	iotable_init(&desc, 1);

	/*                         */
	local_flush_tlb_all();
	flush_cache_all();

	return readl(IOMEM(UX500_VIRT_ROM + (addr & 0xfff)));
}

static void ux500_print_soc_info(unsigned int asicid)
{
	unsigned int rev = dbx500_revision();

	pr_info("DB%4x ", dbx500_partnumber());

	if (rev == 0x01)
		pr_cont("Early Drop");
	else if (rev >= 0xA0)
		pr_cont("v%d.%d" , (rev >> 4) - 0xA + 1, rev & 0xf);
	else
		pr_cont("Unknown");

	pr_cont(" [%#010x]\n", asicid);
}

static unsigned int partnumber(unsigned int asicid)
{
	return (asicid >> 8) & 0xffff;
}

/*
                                          
                                             
                                             
                                               
                                             
                                               
                                             
                                           
 */

void __init ux500_map_io(void)
{
	unsigned int cpuid = read_cpuid_id();
	unsigned int asicid = 0;
	phys_addr_t addr = 0;

	switch (cpuid) {
	case 0x410fc090: /*          */
	case 0x411fc091: /*          */
		addr = 0x9001FFF4;
		break;

	case 0x412fc091: /*                              */
		asicid = ux500_read_asicid(0x9001DBF4);
		if (partnumber(asicid) == 0x8500 ||
		    partnumber(asicid) == 0x8520)
			/*          */
			break;

		/*          */
		addr = 0x9001FFF4;
		break;

	case 0x413fc090: /*        */
		addr = 0xFFFFDBF4;
		break;
	}

	if (addr)
		asicid = ux500_read_asicid(addr);

	if (!asicid) {
		pr_err("Unable to identify SoC\n");
		ux500_unknown_soc();
	}

	dbx500_id.process = asicid >> 24;
	dbx500_id.partnumber = partnumber(asicid);
	dbx500_id.revision = asicid & 0xff;

	ux500_print_soc_info(asicid);
}
