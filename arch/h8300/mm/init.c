/*
 *  linux/arch/h8300/mm/init.c
 *
 *  Copyright (C) 1998  D. Jeff Dionne <jeff@lineo.ca>,
 *                      Kenneth Albanowski <kjahds@kjahds.com>,
 *  Copyright (C) 2000  Lineo, Inc.  (www.lineo.com) 
 *
 *  Based on:
 *
 *  linux/arch/m68knommu/mm/init.c
 *  linux/arch/m68k/mm/init.c
 *
 *  Copyright (C) 1995  Hamish Macdonald
 *
 *  JAN/1999 -- hacked to support ColdFire (gerg@snapgear.com)
 *  DEC/2000 -- linux 2.4 support <davidm@snapgear.com>
 */

#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/types.h>
#include <linux/ptrace.h>
#include <linux/mman.h>
#include <linux/mm.h>
#include <linux/swap.h>
#include <linux/init.h>
#include <linux/highmem.h>
#include <linux/pagemap.h>
#include <linux/bootmem.h>
#include <linux/gfp.h>

#include <asm/setup.h>
#include <asm/segment.h>
#include <asm/page.h>
#include <asm/pgtable.h>
#include <asm/sections.h>

#undef DEBUG

/*
                                                               
                                                       
                                                             
                                                               
               
  
                                                                  
                                
  
                                                                
                
 */
static unsigned long empty_bad_page_table;

static unsigned long empty_bad_page;

unsigned long empty_zero_page;

extern unsigned long rom_length;

extern unsigned long memory_start;
extern unsigned long memory_end;

/*
                                                                     
                                        
                                                                        
                                                
 */
void __init paging_init(void)
{
	/*
                                                               
                                                
  */
#ifdef DEBUG
	unsigned long start_mem = PAGE_ALIGN(memory_start);
#endif
	unsigned long end_mem   = memory_end & PAGE_MASK;

#ifdef DEBUG
	printk ("start_mem is %#lx\nvirtual_end is %#lx\n",
		start_mem, end_mem);
#endif

	/*
                                                       
                                   
  */
	empty_bad_page_table = (unsigned long)alloc_bootmem_pages(PAGE_SIZE);
	empty_bad_page = (unsigned long)alloc_bootmem_pages(PAGE_SIZE);
	empty_zero_page = (unsigned long)alloc_bootmem_pages(PAGE_SIZE);
	memset((void *)empty_zero_page, 0, PAGE_SIZE);

	/*
                                               
  */
	set_fs (USER_DS);

#ifdef DEBUG
	printk ("before free_area_init\n");

	printk ("free_area_init -> start_mem is %#lx\nvirtual_end is %#lx\n",
		start_mem, end_mem);
#endif

	{
		unsigned long zones_size[MAX_NR_ZONES] = {0, };

		zones_size[ZONE_DMA]     = 0 >> PAGE_SHIFT;
		zones_size[ZONE_NORMAL]  = (end_mem - PAGE_OFFSET) >> PAGE_SHIFT;
#ifdef CONFIG_HIGHMEM
		zones_size[ZONE_HIGHMEM] = 0;
#endif
		free_area_init(zones_size);
	}
}

void __init mem_init(void)
{
	int codek = 0, datak = 0, initk = 0;
	/*                                                                */
	unsigned long tmp;
	extern unsigned long  _ramend, _ramstart;
	unsigned long len = &_ramend - &_ramstart;
	unsigned long start_mem = memory_start; /*                                            */
	unsigned long end_mem   = memory_end; /*                                                    */

#ifdef DEBUG
	printk(KERN_DEBUG "Mem_init: start=%lx, end=%lx\n", start_mem, end_mem);
#endif

	end_mem &= PAGE_MASK;
	high_memory = (void *) end_mem;

	start_mem = PAGE_ALIGN(start_mem);
	max_mapnr = num_physpages = MAP_NR(high_memory);

	/*                                                 */
	totalram_pages = free_all_bootmem();

	codek = (_etext - _stext) >> 10;
	datak = (__bss_stop - _sdata) >> 10;
	initk = (__init_begin - __init_end) >> 10;

	tmp = nr_free_pages() << PAGE_SHIFT;
	printk(KERN_INFO "Memory available: %luk/%luk RAM, %luk/%luk ROM (%dk kernel code, %dk data)\n",
	       tmp >> 10,
	       len >> 10,
	       (rom_length > 0) ? ((rom_length >> 10) - codek) : 0,
	       rom_length >> 10,
	       codek,
	       datak
	       );
}


#ifdef CONFIG_BLK_DEV_INITRD
void free_initrd_mem(unsigned long start, unsigned long end)
{
	free_reserved_area(start, end, 0, "initrd");
}
#endif

void
free_initmem(void)
{
#ifdef CONFIG_RAMKERNEL
	free_initmem_default(0);
#endif
}

