/*
 * Copyright (C) 2005-2012 Imagination Technologies Ltd.
 *
 * This file contains the architecture-dependant parts of system setup.
 *
 */

#include <linux/export.h>
#include <linux/bootmem.h>
#include <linux/console.h>
#include <linux/cpu.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/genhd.h>
#include <linux/init.h>
#include <linux/initrd.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/memblock.h>
#include <linux/mm.h>
#include <linux/of_fdt.h>
#include <linux/pfn.h>
#include <linux/root_dev.h>
#include <linux/sched.h>
#include <linux/seq_file.h>
#include <linux/start_kernel.h>
#include <linux/string.h>

#include <asm/cachepart.h>
#include <asm/clock.h>
#include <asm/core_reg.h>
#include <asm/cpu.h>
#include <asm/da.h>
#include <asm/highmem.h>
#include <asm/hwthread.h>
#include <asm/l2cache.h>
#include <asm/mach/arch.h>
#include <asm/metag_mem.h>
#include <asm/metag_regs.h>
#include <asm/mmu.h>
#include <asm/mmzone.h>
#include <asm/processor.h>
#include <asm/prom.h>
#include <asm/sections.h>
#include <asm/setup.h>
#include <asm/traps.h>

/*                                             */
#define DEFAULT_PRIV	(TXPRIVEXT_COPRO_BITS		| \
			 TXPRIVEXT_TXTRIGGER_BIT	| \
			 TXPRIVEXT_TXGBLCREG_BIT	| \
			 TXPRIVEXT_ILOCK_BIT		| \
			 TXPRIVEXT_TXITACCYC_BIT	| \
			 TXPRIVEXT_TXDIVTIME_BIT	| \
			 TXPRIVEXT_TXAMAREGX_BIT	| \
			 TXPRIVEXT_TXTIMERI_BIT		| \
			 TXPRIVEXT_TXSTATUS_BIT		| \
			 TXPRIVEXT_TXDISABLE_BIT)

/*                      */
#ifdef CONFIG_METAG_META12
#define META2_PRIV	0
#else
#define META2_PRIV	(TXPRIVEXT_TXTIMER_BIT		| \
			 TXPRIVEXT_TRACE_BIT)
#endif

/*                                 */
#ifdef CONFIG_METAG_UNALIGNED
#define UNALIGNED_PRIV	TXPRIVEXT_ALIGNREW_BIT
#else
#define UNALIGNED_PRIV	0
#endif

#define PRIV_BITS 	(DEFAULT_PRIV			| \
			 META2_PRIV			| \
			 UNALIGNED_PRIV)

/*
                     
                                             
                                                 
                                           
                                                         
  
                   
                                                        
 */
#ifdef CONFIG_METAG_META12
#define PRIVSYSR_BITS	TXPRIVSYSR_ALL_BITS
#else
#define PRIVSYSR_BITS	(TXPRIVSYSR_ALL_BITS & ~TXPRIVSYSR_CORECODE_BIT)
#endif

/*                                        */
#define PIOREG_BITS	0xffffffff

/*
                                         
                                                                       
 */
#define PSYREG_BITS	0xfffffffb


extern char _heap_start[];

#ifdef CONFIG_METAG_BUILTIN_DTB
extern u32 __dtb_start[];
#endif

#ifdef CONFIG_DA_CONSOLE
/*                                        */
extern struct console dash_console;
#endif

struct machine_desc *machine_desc __initdata;

/*
                                                 
                                                                            
                                                     
 */
u8 cpu_2_hwthread_id[NR_CPUS] __read_mostly = {
	[0 ... NR_CPUS-1] = BAD_HWTHREAD_ID
};
EXPORT_SYMBOL_GPL(cpu_2_hwthread_id);

/*
                                                 
                                                                           
                                                                        
 */
u8 hwthread_id_2_cpu[4] __read_mostly = {
	[0 ... 3] = BAD_CPU_ID
};

/*                                                                       
                                                                     
                                                                          
             
                                                                    
                                  
 */
unsigned int meta_memoffset;
EXPORT_SYMBOL(meta_memoffset);

static char __initdata *original_cmd_line;

DEFINE_PER_CPU(PTBI, pTBI);

/*
                                                      
  
                                 
  
                           
                              
                 
                 
                 
                 
 */
static int __init parse_hwthread_map(char *p)
{
	int cpu;

	while (*p) {
		cpu = (*p++) - '0';
		if (cpu < 0 || cpu > 9)
			goto err_cpu;

		p++;		/*                 */
		cpu_2_hwthread_id[cpu] = (*p++) - '0';
		if (cpu_2_hwthread_id[cpu] >= 4)
			goto err_thread;
		hwthread_id_2_cpu[cpu_2_hwthread_id[cpu]] = cpu;

		if (*p == ',')
			p++;		/*            */
	}

	return 0;
err_cpu:
	pr_err("%s: hwthread_map cpu argument out of range\n", __func__);
	return -EINVAL;
err_thread:
	pr_err("%s: hwthread_map thread argument out of range\n", __func__);
	return -EINVAL;
}
early_param("hwthread_map", parse_hwthread_map);

void __init dump_machine_table(void)
{
	struct machine_desc *p;
	const char **compat;

	pr_info("Available machine support:\n\tNAME\t\tCOMPATIBLE LIST\n");
	for_each_machine_desc(p) {
		pr_info("\t%s\t[", p->name);
		for (compat = p->dt_compat; compat && *compat; ++compat)
			printk(" '%s'", *compat);
		printk(" ]\n");
	}

	pr_info("\nPlease check your kernel config and/or bootloader.\n");

	hard_processor_halt(HALT_PANIC);
}

#ifdef CONFIG_METAG_HALT_ON_PANIC
static int metag_panic_event(struct notifier_block *this, unsigned long event,
			     void *ptr)
{
	hard_processor_halt(HALT_PANIC);
	return NOTIFY_DONE;
}

static struct notifier_block metag_panic_block = {
	metag_panic_event,
	NULL,
	0
};
#endif

void __init setup_arch(char **cmdline_p)
{
	unsigned long start_pfn;
	unsigned long text_start = (unsigned long)(&_stext);
	unsigned long cpu = smp_processor_id();
	unsigned long heap_start, heap_end;
	unsigned long start_pte;
	PTBI _pTBI;
	PTBISEG p_heap;
	int heap_id, i;

	metag_cache_probe();

	metag_da_probe();
#ifdef CONFIG_DA_CONSOLE
	if (metag_da_enabled()) {
		/*                                       */
		register_console(&dash_console);
		add_preferred_console("ttyDA", 1, NULL);
	}
#endif

	/*                                                */
	machine_desc = setup_machine_fdt(original_cmd_line);
	/*                                                                 */
	if (!machine_desc) {
#ifdef CONFIG_METAG_BUILTIN_DTB
		/*                              */
		machine_desc = setup_machine_fdt(__dtb_start);
		if (!machine_desc)
			panic("Invalid embedded device tree.");
#else
		/*                                     */
		machine_desc = default_machine_desc();
#endif
#ifndef CONFIG_CMDLINE_FORCE
		/*                                                          */
		if (boot_command_line[0] && original_cmd_line[0])
			strlcat(boot_command_line, " ", COMMAND_LINE_SIZE);
		strlcat(boot_command_line, original_cmd_line,
			COMMAND_LINE_SIZE);
#endif
	}
	setup_meta_clocks(machine_desc->clocks);

	*cmdline_p = boot_command_line;
	parse_early_param();

	/*
                                                
  */
	check_for_cache_aliasing(cpu);


#ifdef CONFIG_METAG_HALT_ON_PANIC
	atomic_notifier_chain_register(&panic_notifier_list,
				       &metag_panic_block);
#endif

#ifdef CONFIG_DUMMY_CONSOLE
	conswitchp = &dummy_con;
#endif

	if (!(__core_reg_get(TXSTATUS) & TXSTATUS_PSTAT_BIT))
		panic("Privilege must be enabled for this thread.");

	_pTBI = __TBI(TBID_ISTAT_BIT);

	per_cpu(pTBI, cpu) = _pTBI;

	if (!per_cpu(pTBI, cpu))
		panic("No TBI found!");

	/*
                                                                  
                                                                
                                                             
                             
   
                                                                  
                                     
  */
	for (i = 0; i <= TBID_SIGNUM_MAX; i++)
		if (i != TBID_SIGNUM_LWK)
			_pTBI->fnSigs[i] = __TBIUnExpXXX;

	/*                                                            
                       
  */
	if (PAGE_OFFSET != text_start)
		panic("Kernel not loaded at PAGE_OFFSET (%#x) but at %#lx.",
		      PAGE_OFFSET, text_start);

	start_pte = mmu_read_second_level_page(text_start);

	/*
                                                                
  */
	if (!(start_pte & _PAGE_KERNEL))
		panic("kernel pte does not have PRIV set");

	/*
                                            
                                                              
                             
  */
	meta_memoffset = text_start - (start_pte & PAGE_MASK);

	/*                                 */
	heap_id = (__TBIThreadId() & TBID_THREAD_BITS)
		+ TBID_SEG(0, TBID_SEGSCOPE_LOCAL, TBID_SEGTYPE_HEAP);

	p_heap = __TBIFindSeg(NULL, heap_id);

	if (!p_heap)
		panic("Could not find heap from TBI!");

	/*                                                               */
	heap_start = (unsigned long) &_heap_start;

	/*                                                            
         
  */
	if (is_global_space(text_start)) {
		pr_debug("WARNING: running in global space!\n");
		heap_end = (unsigned long)p_heap->pGAddr + p_heap->Bytes;
	} else {
		heap_end = (unsigned long)p_heap->pLAddr + p_heap->Bytes;
	}

	ROOT_DEV = Root_RAM0;

	/*                                                              
                                                      
   
                                                   
  */
	init_mm.start_code = (unsigned long)(&_stext);
	init_mm.end_code = (unsigned long)(&_etext);
	init_mm.end_data = (unsigned long)(&_edata);
	init_mm.brk = (unsigned long)heap_start;

	min_low_pfn = PFN_UP(__pa(text_start));
	max_low_pfn = PFN_DOWN(__pa(heap_end));

	pfn_base = min_low_pfn;

	/*                                                            
                                                                       
  */
	max_pfn = max_low_pfn + ((1 << MAX_ORDER) - 1);
	max_pfn &= ~((1 << MAX_ORDER) - 1);

	start_pfn = PFN_UP(__pa(heap_start));

	if (min_low_pfn & ((1 << MAX_ORDER) - 1)) {
		/*                                                  
                                                     
                                                     
                                                     
                                                
                                                       
   */

		panic("Kernel must be %d byte aligned, currently at %#lx.",
		      1 << (MAX_ORDER + PAGE_SHIFT),
		      min_low_pfn << PAGE_SHIFT);
	}

#ifdef CONFIG_HIGHMEM
	highstart_pfn = highend_pfn = max_pfn;
	high_memory = (void *) __va(PFN_PHYS(highstart_pfn));
#else
	high_memory = (void *)__va(PFN_PHYS(max_pfn));
#endif

	paging_init(heap_end);

	setup_priv();

	/*                                                             */
	cpu_2_hwthread_id[smp_processor_id()] = hard_processor_id();
	hwthread_id_2_cpu[hard_processor_id()] = smp_processor_id();

	/*                                                                */
	copy_fdt();
	unflatten_device_tree();

#ifdef CONFIG_SMP
	smp_init_cpus();
#endif

	if (machine_desc->init_early)
		machine_desc->init_early();
}

static int __init customize_machine(void)
{
	/*                                               */
	if (machine_desc->init_machine)
		machine_desc->init_machine();
	return 0;
}
arch_initcall(customize_machine);

static int __init init_machine_late(void)
{
	if (machine_desc->init_late)
		machine_desc->init_late();
	return 0;
}
late_initcall(init_machine_late);

#ifdef CONFIG_PROC_FS
/*
                                             
 */
static const char *get_cpu_capabilities(unsigned int txenable)
{
#ifdef CONFIG_METAG_META21
	/*                                                                */
	int coreid = metag_in32(METAC_CORE_ID);
	unsigned int dsp_type = (coreid >> 3) & 7;
	unsigned int fpu_type = (coreid >> 7) & 3;

	switch (dsp_type | fpu_type << 3) {
	case (0x00): return "EDSP";
	case (0x01): return "DSP";
	case (0x08): return "EDSP+LFPU";
	case (0x09): return "DSP+LFPU";
	case (0x10): return "EDSP+FPU";
	case (0x11): return "DSP+FPU";
	}
	return "UNKNOWN";

#else
	if (!(txenable & TXENABLE_CLASS_BITS))
		return "DSP";
	else
		return "";
#endif
}

static int show_cpuinfo(struct seq_file *m, void *v)
{
	const char *cpu;
	unsigned int txenable, thread_id, major, minor;
	unsigned long clockfreq = get_coreclock();
#ifdef CONFIG_SMP
	int i;
	unsigned long lpj;
#endif

	cpu = "META";

	txenable = __core_reg_get(TXENABLE);
	major = (txenable & TXENABLE_MAJOR_REV_BITS) >> TXENABLE_MAJOR_REV_S;
	minor = (txenable & TXENABLE_MINOR_REV_BITS) >> TXENABLE_MINOR_REV_S;
	thread_id = (txenable >> 8) & 0x3;

#ifdef CONFIG_SMP
	for_each_online_cpu(i) {
		lpj = per_cpu(cpu_data, i).loops_per_jiffy;
		txenable = core_reg_read(TXUCT_ID, TXENABLE_REGNUM,
							cpu_2_hwthread_id[i]);

		seq_printf(m, "CPU:\t\t%s %d.%d (thread %d)\n"
			      "Clocking:\t%lu.%1luMHz\n"
			      "BogoMips:\t%lu.%02lu\n"
			      "Calibration:\t%lu loops\n"
			      "Capabilities:\t%s\n\n",
			      cpu, major, minor, i,
			      clockfreq / 1000000, (clockfreq / 100000) % 10,
			      lpj / (500000 / HZ), (lpj / (5000 / HZ)) % 100,
			      lpj,
			      get_cpu_capabilities(txenable));
	}
#else
	seq_printf(m, "CPU:\t\t%s %d.%d (thread %d)\n"
		   "Clocking:\t%lu.%1luMHz\n"
		   "BogoMips:\t%lu.%02lu\n"
		   "Calibration:\t%lu loops\n"
		   "Capabilities:\t%s\n",
		   cpu, major, minor, thread_id,
		   clockfreq / 1000000, (clockfreq / 100000) % 10,
		   loops_per_jiffy / (500000 / HZ),
		   (loops_per_jiffy / (5000 / HZ)) % 100,
		   loops_per_jiffy,
		   get_cpu_capabilities(txenable));
#endif /*            */

#ifdef CONFIG_METAG_L2C
	if (meta_l2c_is_present()) {
		seq_printf(m, "L2 cache:\t%s\n"
			      "L2 cache size:\t%d KB\n",
			      meta_l2c_is_enabled() ? "enabled" : "disabled",
			      meta_l2c_size() >> 10);
	}
#endif
	return 0;
}

static void *c_start(struct seq_file *m, loff_t *pos)
{
	return (void *)(*pos == 0);
}
static void *c_next(struct seq_file *m, void *v, loff_t *pos)
{
	return NULL;
}
static void c_stop(struct seq_file *m, void *v)
{
}
const struct seq_operations cpuinfo_op = {
	.start = c_start,
	.next  = c_next,
	.stop  = c_stop,
	.show  = show_cpuinfo,
};
#endif /*                */

void __init metag_start_kernel(char *args)
{
	/*                                                            
                                     
  */
	__core_reg_set(TXTIMER, 0);

	/*                */
	memset(__bss_start, 0,
	       (unsigned long)__bss_stop - (unsigned long)__bss_start);

	/*                                                */
	original_cmd_line = args;

	current_thread_info()->cpu = hard_processor_id();

	start_kernel();
}

/* 
                                                        
  
                                                                              
                                                                   
 */
void setup_priv(void)
{
	unsigned int offset = hard_processor_id() << TXPRIVREG_STRIDE_S;

	__core_reg_set(TXPRIVEXT, PRIV_BITS);

	metag_out32(PRIVSYSR_BITS, T0PRIVSYSR + offset);
	metag_out32(PIOREG_BITS,   T0PIOREG   + offset);
	metag_out32(PSYREG_BITS,   T0PSYREG   + offset);
}

PTBI pTBI_get(unsigned int cpu)
{
	return per_cpu(pTBI, cpu);
}
EXPORT_SYMBOL(pTBI_get);

#if defined(CONFIG_METAG_DSP) && defined(CONFIG_METAG_FPU)
char capabilites[] = "dsp fpu";
#elif defined(CONFIG_METAG_DSP)
char capabilites[] = "dsp";
#elif defined(CONFIG_METAG_FPU)
char capabilites[] = "fpu";
#else
char capabilites[] = "";
#endif

static struct ctl_table caps_kern_table[] = {
	{
		.procname	= "capabilities",
		.data		= capabilites,
		.maxlen		= sizeof(capabilites),
		.mode		= 0444,
		.proc_handler	= proc_dostring,
	},
	{}
};

static struct ctl_table caps_root_table[] = {
	{
		.procname	= "kernel",
		.mode		= 0555,
		.child		= caps_kern_table,
	},
	{}
};

static int __init capabilities_register_sysctl(void)
{
	struct ctl_table_header *caps_table_header;

	caps_table_header = register_sysctl_table(caps_root_table);
	if (!caps_table_header) {
		pr_err("Unable to register CAPABILITIES sysctl\n");
		return -ENOMEM;
	}

	return 0;
}

core_initcall(capabilities_register_sysctl);
