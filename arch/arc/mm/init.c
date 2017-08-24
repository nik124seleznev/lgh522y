/*
 * Copyright (C) 2004, 2007-2010, 2011-2012 Synopsys, Inc. (www.synopsys.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/bootmem.h>
#include <linux/memblock.h>
#include <linux/swap.h>
#include <linux/module.h>
#include <asm/page.h>
#include <asm/pgalloc.h>
#include <asm/sections.h>
#include <asm/arcregs.h>

pgd_t swapper_pg_dir[PTRS_PER_PGD] __aligned(PAGE_SIZE);
char empty_zero_page[PAGE_SIZE] __aligned(PAGE_SIZE);
EXPORT_SYMBOL(empty_zero_page);

/*                              */
static unsigned long arc_mem_sz = 0x20000000;  /*              */

/*                                                          */
static int __init setup_mem_sz(char *str)
{
	arc_mem_sz = memparse(str, NULL) & PAGE_MASK;

	/*                                                              */
	pr_info("\"mem=%s\": mem sz set to %ldM\n", str, TO_MB(arc_mem_sz));

	return 0;
}
early_param("mem", setup_mem_sz);

void __init early_init_dt_add_memory_arch(u64 base, u64 size)
{
	arc_mem_sz = size & PAGE_MASK;
	pr_info("Memory size set via devicetree %ldM\n", TO_MB(arc_mem_sz));
}

/*
                                                      
                                 
                                                         
                
 */
void __init setup_arch_memory(void)
{
	unsigned long zones_size[MAX_NR_ZONES] = { 0, 0 };
	unsigned long end_mem = CONFIG_LINUX_LINK_BASE + arc_mem_sz;

	init_mm.start_code = (unsigned long)_text;
	init_mm.end_code = (unsigned long)_etext;
	init_mm.end_data = (unsigned long)_edata;
	init_mm.brk = (unsigned long)_end;

	/*
                                                           
                                                      
                                                            
                                      
  */
	memblock_add(CONFIG_LINUX_LINK_BASE, arc_mem_sz);

	/*                                                           */

	/*                                                   */
	min_low_pfn = PFN_DOWN(CONFIG_LINUX_LINK_BASE);

	/*                                                           */
	max_low_pfn = max_pfn = PFN_DOWN(end_mem);

	max_mapnr = num_physpages = max_low_pfn - min_low_pfn;

	/*                                                          */
	memblock_reserve(CONFIG_LINUX_LINK_BASE,
			 __pa(_end) - CONFIG_LINUX_LINK_BASE);

	memblock_dump_all();

	/*                                                          */
	memset(zones_size, 0, sizeof(zones_size));
	zones_size[ZONE_NORMAL] = num_physpages;

	/*
                                                                   
                                                                
                                                      
                                         
  */
	free_area_init_node(0,			/*         */
			    zones_size,		/*                    */
			    min_low_pfn,	/*                   */
			    NULL);		/*          */
}

/*
                                
  
                   
                                                
 */
void __init mem_init(void)
{
	int codesize, datasize, initsize, reserved_pages, free_pages;
	int tmp;

	high_memory = (void *)(CONFIG_LINUX_LINK_BASE + arc_mem_sz);

	totalram_pages = free_all_bootmem();

	/*                                                       */
	reserved_pages = 0;
	for (tmp = 0; tmp < max_mapnr; tmp++)
		if (PageReserved(mem_map + tmp))
			reserved_pages++;

	/*                                    */
	free_pages = max_mapnr - reserved_pages;

	/*
                                                             
                                                 
                                                  
  */
	reserved_pages -= (((unsigned int)_end - CONFIG_LINUX_LINK_BASE) >>
								PAGE_SHIFT);

	codesize = _etext - _text;
	datasize = _end - _etext;
	initsize = __init_end - __init_begin;

	pr_info("Memory Available: %dM / %ldM (%dK code, %dK data, %dK init, %dK reserv)\n",
		PAGES_TO_MB(free_pages),
		TO_MB(arc_mem_sz),
		TO_KB(codesize), TO_KB(datasize), TO_KB(initsize),
		PAGES_TO_KB(reserved_pages));
}

/*
                                            
 */
void __init_refok free_initmem(void)
{
	free_initmem_default(0);
}

#ifdef CONFIG_BLK_DEV_INITRD
void __init free_initrd_mem(unsigned long start, unsigned long end)
{
	free_reserved_area(start, end, 0, "initrd");
}
#endif

#ifdef CONFIG_OF_FLATTREE
void __init early_init_dt_setup_initrd_arch(unsigned long start,
					    unsigned long end)
{
	pr_err("%s(%lx, %lx)\n", __func__, start, end);
}
#endif /*                    */
