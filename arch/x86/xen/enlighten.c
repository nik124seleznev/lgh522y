/*
                                           
  
                                                                    
                       
                            
                    
                       
                      
  
                                                                  
 */

#include <linux/cpu.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/smp.h>
#include <linux/preempt.h>
#include <linux/hardirq.h>
#include <linux/percpu.h>
#include <linux/delay.h>
#include <linux/start_kernel.h>
#include <linux/sched.h>
#include <linux/kprobes.h>
#include <linux/bootmem.h>
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/page-flags.h>
#include <linux/highmem.h>
#include <linux/console.h>
#include <linux/pci.h>
#include <linux/gfp.h>
#include <linux/memblock.h>
#include <linux/edd.h>

#include <xen/xen.h>
#include <xen/events.h>
#include <xen/interface/xen.h>
#include <xen/interface/version.h>
#include <xen/interface/physdev.h>
#include <xen/interface/vcpu.h>
#include <xen/interface/memory.h>
#include <xen/interface/xen-mca.h>
#include <xen/features.h>
#include <xen/page.h>
#include <xen/hvm.h>
#include <xen/hvc-console.h>
#include <xen/acpi.h>

#include <asm/paravirt.h>
#include <asm/apic.h>
#include <asm/page.h>
#include <asm/xen/pci.h>
#include <asm/xen/hypercall.h>
#include <asm/xen/hypervisor.h>
#include <asm/fixmap.h>
#include <asm/processor.h>
#include <asm/proto.h>
#include <asm/msr-index.h>
#include <asm/traps.h>
#include <asm/setup.h>
#include <asm/desc.h>
#include <asm/pgalloc.h>
#include <asm/pgtable.h>
#include <asm/tlbflush.h>
#include <asm/reboot.h>
#include <asm/stackprotector.h>
#include <asm/hypervisor.h>
#include <asm/mwait.h>
#include <asm/pci_x86.h>
#include <asm/pat.h>

#ifdef CONFIG_ACPI
#include <linux/acpi.h>
#include <asm/acpi.h>
#include <acpi/pdc_intel.h>
#include <acpi/processor.h>
#include <xen/interface/platform.h>
#endif

#include "xen-ops.h"
#include "mmu.h"
#include "smp.h"
#include "multicalls.h"

EXPORT_SYMBOL_GPL(hypercall_page);

/*
                                            
                                                                        
                                                                                
                                                                             
                                                                     
                              
                                                                           
                                                      
  
                                                                       
                                                                         
                                                                     
             
  
 */
DEFINE_PER_CPU(struct vcpu_info *, xen_vcpu);

/*
                                                                      
                                                                       
                                                      
 */
DEFINE_PER_CPU(struct vcpu_info, xen_vcpu_info);

enum xen_domain_type xen_domain_type = XEN_NATIVE;
EXPORT_SYMBOL_GPL(xen_domain_type);

unsigned long *machine_to_phys_mapping = (void *)MACH2PHYS_VIRT_START;
EXPORT_SYMBOL(machine_to_phys_mapping);
unsigned long  machine_to_phys_nr;
EXPORT_SYMBOL(machine_to_phys_nr);

struct start_info *xen_start_info;
EXPORT_SYMBOL_GPL(xen_start_info);

struct shared_info xen_dummy_shared_info;

void *xen_initial_gdt;

RESERVE_BRK(shared_info_page_brk, PAGE_SIZE);
__read_mostly int xen_have_vector_callback;
EXPORT_SYMBOL_GPL(xen_have_vector_callback);

/*
                                                                        
                                            
 */
struct shared_info *HYPERVISOR_shared_info = &xen_dummy_shared_info;

/*
                                                                    
                                                                    
                                                                   
                                                                     
                                                               
                                                                    
  
                                                                     
                                           
  
                                 
 */
static int have_vcpu_info_placement = 1;

struct tls_descs {
	struct desc_struct desc[3];
};

/*
                                                                    
                                                                   
                                                                
                                                                     
                   
 */
static DEFINE_PER_CPU(struct tls_descs, shadow_tls_desc);

static void clamp_max_cpus(void)
{
#ifdef CONFIG_SMP
	if (setup_max_cpus > MAX_VIRT_CPUS)
		setup_max_cpus = MAX_VIRT_CPUS;
#endif
}

static void xen_vcpu_setup(int cpu)
{
	struct vcpu_register_vcpu_info info;
	int err;
	struct vcpu_info *vcpup;

	BUG_ON(HYPERVISOR_shared_info == &xen_dummy_shared_info);

	/*
                                                                
                                                                 
                                             
                                                             
                              
   
                                                                    
                                                                   
                      
  */
	if (xen_hvm_domain()) {
		if (per_cpu(xen_vcpu, cpu) == &per_cpu(xen_vcpu_info, cpu))
			return;
	}
	if (cpu < MAX_VIRT_CPUS)
		per_cpu(xen_vcpu,cpu) = &HYPERVISOR_shared_info->vcpu_info[cpu];

	if (!have_vcpu_info_placement) {
		if (cpu >= MAX_VIRT_CPUS)
			clamp_max_cpus();
		return;
	}

	vcpup = &per_cpu(xen_vcpu_info, cpu);
	info.mfn = arbitrary_virt_to_mfn(vcpup);
	info.offset = offset_in_page(vcpup);

	/*                                                      
                                                              
                      
                                                                     
                                                                   
                                                                    
                                                 
  */
	err = HYPERVISOR_vcpu_op(VCPUOP_register_vcpu_info, cpu, &info);

	if (err) {
		printk(KERN_DEBUG "register_vcpu_info failed: err=%d\n", err);
		have_vcpu_info_placement = 0;
		clamp_max_cpus();
	} else {
		/*                                                    
                         */
		per_cpu(xen_vcpu, cpu) = vcpup;
	}
}

/*
                                               
                                                
                                     
 */
void xen_vcpu_restore(void)
{
	int cpu;

	for_each_possible_cpu(cpu) {
		bool other_cpu = (cpu != smp_processor_id());
		bool is_up = HYPERVISOR_vcpu_op(VCPUOP_is_up, cpu, NULL);

		if (other_cpu && is_up &&
		    HYPERVISOR_vcpu_op(VCPUOP_down, cpu, NULL))
			BUG();

		xen_setup_runstate_info(cpu);

		if (have_vcpu_info_placement)
			xen_vcpu_setup(cpu);

		if (other_cpu && is_up &&
		    HYPERVISOR_vcpu_op(VCPUOP_up, cpu, NULL))
			BUG();
	}
}

static void __init xen_banner(void)
{
	unsigned version = HYPERVISOR_xen_version(XENVER_version, NULL);
	struct xen_extraversion extra;
	HYPERVISOR_xen_version(XENVER_extraversion, &extra);

	printk(KERN_INFO "Booting paravirtualized kernel on %s\n",
	       pv_info.name);
	printk(KERN_INFO "Xen version: %d.%d%s%s\n",
	       version >> 16, version & 0xffff, extra.extraversion,
	       xen_feature(XENFEAT_mmu_pt_update_preserve_ad) ? " (preserve-AD)" : "");
}
/*                                                         */
bool
xen_running_on_version_or_later(unsigned int major, unsigned int minor)
{
	unsigned int version;

	if (!xen_domain())
		return false;

	version = HYPERVISOR_xen_version(XENVER_version, NULL);
	if ((((version >> 16) == major) && ((version & 0xffff) >= minor)) ||
		((version >> 16) > major))
		return true;
	return false;
}

#define CPUID_THERM_POWER_LEAF 6
#define APERFMPERF_PRESENT 0

static __read_mostly unsigned int cpuid_leaf1_edx_mask = ~0;
static __read_mostly unsigned int cpuid_leaf1_ecx_mask = ~0;

static __read_mostly unsigned int cpuid_leaf1_ecx_set_mask;
static __read_mostly unsigned int cpuid_leaf5_ecx_val;
static __read_mostly unsigned int cpuid_leaf5_edx_val;

static void xen_cpuid(unsigned int *ax, unsigned int *bx,
		      unsigned int *cx, unsigned int *dx)
{
	unsigned maskebx = ~0;
	unsigned maskecx = ~0;
	unsigned maskedx = ~0;
	unsigned setecx = 0;
	/*
                                                              
                                              
  */
	switch (*ax) {
	case 1:
		maskecx = cpuid_leaf1_ecx_mask;
		setecx = cpuid_leaf1_ecx_set_mask;
		maskedx = cpuid_leaf1_edx_mask;
		break;

	case CPUID_MWAIT_LEAF:
		/*                         */
		*ax = 0;
		*bx = 0;
		*cx = cpuid_leaf5_ecx_val;
		*dx = cpuid_leaf5_edx_val;
		return;

	case CPUID_THERM_POWER_LEAF:
		/*                                       */
		maskecx = ~(1 << APERFMPERF_PRESENT);
		break;

	case 0xb:
		/*                                  */
		maskebx = 0;
		break;
	}

	asm(XEN_EMULATE_PREFIX "cpuid"
		: "=a" (*ax),
		  "=b" (*bx),
		  "=c" (*cx),
		  "=d" (*dx)
		: "0" (*ax), "2" (*cx));

	*bx &= maskebx;
	*cx &= maskecx;
	*cx |= setecx;
	*dx &= maskedx;

}

static bool __init xen_check_mwait(void)
{
#ifdef CONFIG_ACPI
	struct xen_platform_op op = {
		.cmd			= XENPF_set_processor_pminfo,
		.u.set_pminfo.id	= -1,
		.u.set_pminfo.type	= XEN_PM_PDC,
	};
	uint32_t buf[3];
	unsigned int ax, bx, cx, dx;
	unsigned int mwait_mask;

	/*                                                          
                                                                       
                                                                       
                                                                 
                                                                       
                                                                        
                                    
  */
	if (!xen_initial_domain())
		return false;

	/*
                                                                  
                                                              
  */
	if (!xen_running_on_version_or_later(4, 2))
		return false;

	ax = 1;
	cx = 0;

	native_cpuid(&ax, &bx, &cx, &dx);

	mwait_mask = (1 << (X86_FEATURE_EST % 32)) |
		     (1 << (X86_FEATURE_MWAIT % 32));

	if ((cx & mwait_mask) != mwait_mask)
		return false;

	/*                                                            
                                                                 
  */

	ax = CPUID_MWAIT_LEAF;
	bx = 0;
	cx = 0;
	dx = 0;

	native_cpuid(&ax, &bx, &cx, &dx);

	/*                                                                
                                                                       
  */
	buf[0] = ACPI_PDC_REVISION_ID;
	buf[1] = 1;
	buf[2] = (ACPI_PDC_C_CAPABILITY_SMP | ACPI_PDC_EST_CAPABILITY_SWSMP);

	set_xen_guest_handle(op.u.set_pminfo.pdc, buf);

	if ((HYPERVISOR_dom0_op(&op) == 0) &&
	    (buf[2] & (ACPI_PDC_C_C1_FFH | ACPI_PDC_C_C2C3_FFH))) {
		cpuid_leaf5_ecx_val = cx;
		cpuid_leaf5_edx_val = dx;
	}
	return true;
#else
	return false;
#endif
}
static void __init xen_init_cpuid_mask(void)
{
	unsigned int ax, bx, cx, dx;
	unsigned int xsave_mask;

	cpuid_leaf1_edx_mask =
		~((1 << X86_FEATURE_MTRR) |  /*              */
		  (1 << X86_FEATURE_ACC));   /*                    */

	if (!xen_initial_domain())
		cpuid_leaf1_edx_mask &=
			~((1 << X86_FEATURE_APIC) |  /*                    */
			  (1 << X86_FEATURE_ACPI));  /*              */

	cpuid_leaf1_ecx_mask &= ~(1 << (X86_FEATURE_X2APIC % 32));

	ax = 1;
	cx = 0;
	xen_cpuid(&ax, &bx, &cx, &dx);

	xsave_mask =
		(1 << (X86_FEATURE_XSAVE % 32)) |
		(1 << (X86_FEATURE_OSXSAVE % 32));

	/*                                                                 */
	if ((cx & xsave_mask) != xsave_mask)
		cpuid_leaf1_ecx_mask &= ~xsave_mask; /*                         */
	if (xen_check_mwait())
		cpuid_leaf1_ecx_set_mask = (1 << (X86_FEATURE_MWAIT % 32));
}

static void xen_set_debugreg(int reg, unsigned long val)
{
	HYPERVISOR_set_debugreg(reg, val);
}

static unsigned long xen_get_debugreg(int reg)
{
	return HYPERVISOR_get_debugreg(reg);
}

static void xen_end_context_switch(struct task_struct *next)
{
	xen_mc_flush();
	paravirt_end_context_switch(next);
}

static unsigned long xen_store_tr(void)
{
	return 0;
}

/*
                                                                     
                                                                   
                                                                      
         
 */
static void set_aliased_prot(void *v, pgprot_t prot)
{
	int level;
	pte_t *ptep;
	pte_t pte;
	unsigned long pfn;
	struct page *page;

	ptep = lookup_address((unsigned long)v, &level);
	BUG_ON(ptep == NULL);

	pfn = pte_pfn(*ptep);
	page = pfn_to_page(pfn);

	pte = pfn_pte(pfn, prot);

	if (HYPERVISOR_update_va_mapping((unsigned long)v, pte, 0))
		BUG();

	if (!PageHighMem(page)) {
		void *av = __va(PFN_PHYS(pfn));

		if (av != v)
			if (HYPERVISOR_update_va_mapping((unsigned long)av, pte, 0))
				BUG();
	} else
		kmap_flush_unused();
}

static void xen_alloc_ldt(struct desc_struct *ldt, unsigned entries)
{
	const unsigned entries_per_page = PAGE_SIZE / LDT_ENTRY_SIZE;
	int i;

	for(i = 0; i < entries; i += entries_per_page)
		set_aliased_prot(ldt + i, PAGE_KERNEL_RO);
}

static void xen_free_ldt(struct desc_struct *ldt, unsigned entries)
{
	const unsigned entries_per_page = PAGE_SIZE / LDT_ENTRY_SIZE;
	int i;

	for(i = 0; i < entries; i += entries_per_page)
		set_aliased_prot(ldt + i, PAGE_KERNEL);
}

static void xen_set_ldt(const void *addr, unsigned entries)
{
	struct mmuext_op *op;
	struct multicall_space mcs = xen_mc_entry(sizeof(*op));

	trace_xen_cpu_set_ldt(addr, entries);

	op = mcs.args;
	op->cmd = MMUEXT_SET_LDT;
	op->arg1.linear_addr = (unsigned long)addr;
	op->arg2.nr_ents = entries;

	MULTI_mmuext_op(mcs.mc, op, 1, NULL, DOMID_SELF);

	xen_mc_issue(PARAVIRT_LAZY_CPU);
}

static void xen_load_gdt(const struct desc_ptr *dtr)
{
	unsigned long va = dtr->address;
	unsigned int size = dtr->size + 1;
	unsigned pages = (size + PAGE_SIZE - 1) / PAGE_SIZE;
	unsigned long frames[pages];
	int f;

	/*
                                                             
                                    
  */

	BUG_ON(size > 65536);
	BUG_ON(va & ~PAGE_MASK);

	for (f = 0; va < dtr->address + size; va += PAGE_SIZE, f++) {
		int level;
		pte_t *ptep;
		unsigned long pfn, mfn;
		void *virt;

		/*
                                                       
                                                     
                                                
                                                     
                                                     
   */
		ptep = lookup_address(va, &level);
		BUG_ON(ptep == NULL);

		pfn = pte_pfn(*ptep);
		mfn = pfn_to_mfn(pfn);
		virt = __va(PFN_PHYS(pfn));

		frames[f] = mfn;

		make_lowmem_page_readonly((void *)va);
		make_lowmem_page_readonly(virt);
	}

	if (HYPERVISOR_set_gdt(frames, size / sizeof(struct desc_struct)))
		BUG();
}

/*
                                                            
 */
static void __init xen_load_gdt_boot(const struct desc_ptr *dtr)
{
	unsigned long va = dtr->address;
	unsigned int size = dtr->size + 1;
	unsigned pages = (size + PAGE_SIZE - 1) / PAGE_SIZE;
	unsigned long frames[pages];
	int f;

	/*
                                                             
                                    
  */

	BUG_ON(size > 65536);
	BUG_ON(va & ~PAGE_MASK);

	for (f = 0; va < dtr->address + size; va += PAGE_SIZE, f++) {
		pte_t pte;
		unsigned long pfn, mfn;

		pfn = virt_to_pfn(va);
		mfn = pfn_to_mfn(pfn);

		pte = pfn_pte(pfn, PAGE_KERNEL_RO);

		if (HYPERVISOR_update_va_mapping((unsigned long)va, pte, 0))
			BUG();

		frames[f] = mfn;
	}

	if (HYPERVISOR_set_gdt(frames, size / sizeof(struct desc_struct)))
		BUG();
}

static inline bool desc_equal(const struct desc_struct *d1,
			      const struct desc_struct *d2)
{
	return d1->a == d2->a && d1->b == d2->b;
}

static void load_TLS_descriptor(struct thread_struct *t,
				unsigned int cpu, unsigned int i)
{
	struct desc_struct *shadow = &per_cpu(shadow_tls_desc, cpu).desc[i];
	struct desc_struct *gdt;
	xmaddr_t maddr;
	struct multicall_space mc;

	if (desc_equal(shadow, &t->tls_array[i]))
		return;

	*shadow = t->tls_array[i];

	gdt = get_cpu_gdt_table(cpu);
	maddr = arbitrary_virt_to_machine(&gdt[GDT_ENTRY_TLS_MIN+i]);
	mc = __xen_mc_entry(0);

	MULTI_update_descriptor(mc.mc, maddr.maddr, t->tls_array[i]);
}

static void xen_load_tls(struct thread_struct *t, unsigned int cpu)
{
	/*
                                                             
                                                        
                                                               
                                                      
                                                              
                                                               
                                                             
                                           
   
                                                               
                                                             
                               
   
                                                            
                                                             
                                                   
  */
	if (paravirt_get_lazy_mode() == PARAVIRT_LAZY_CPU) {
#ifdef CONFIG_X86_32
		lazy_load_gs(0);
#else
		loadsegment(fs, 0);
#endif
	}

	xen_mc_batch();

	load_TLS_descriptor(t, cpu, 0);
	load_TLS_descriptor(t, cpu, 1);
	load_TLS_descriptor(t, cpu, 2);

	xen_mc_issue(PARAVIRT_LAZY_CPU);
}

#ifdef CONFIG_X86_64
static void xen_load_gs_index(unsigned int idx)
{
	if (HYPERVISOR_set_segment_base(SEGBASE_GS_USER_SEL, idx))
		BUG();
}
#endif

static void xen_write_ldt_entry(struct desc_struct *dt, int entrynum,
				const void *ptr)
{
	xmaddr_t mach_lp = arbitrary_virt_to_machine(&dt[entrynum]);
	u64 entry = *(u64 *)ptr;

	trace_xen_cpu_write_ldt_entry(dt, entrynum, entry);

	preempt_disable();

	xen_mc_flush();
	if (HYPERVISOR_update_descriptor(mach_lp.maddr, entry))
		BUG();

	preempt_enable();
}

static int cvt_gate_to_trap(int vector, const gate_desc *val,
			    struct trap_info *info)
{
	unsigned long addr;

	if (val->type != GATE_TRAP && val->type != GATE_INTERRUPT)
		return 0;

	info->vector = vector;

	addr = gate_offset(*val);
#ifdef CONFIG_X86_64
	/*
                                                       
                                                               
                                                     
                                         
                                                  
  */
	if (addr == (unsigned long)debug)
		addr = (unsigned long)xen_debug;
	else if (addr == (unsigned long)int3)
		addr = (unsigned long)xen_int3;
	else if (addr == (unsigned long)stack_segment)
		addr = (unsigned long)xen_stack_segment;
	else if (addr == (unsigned long)double_fault ||
		 addr == (unsigned long)nmi) {
		/*                            */
		return 0;
#ifdef CONFIG_X86_MCE
	} else if (addr == (unsigned long)machine_check) {
		/*
                                              
                                        
   */
		;
#endif
	} else {
		/*                            */
		if (WARN_ON(val->ist != 0))
			return 0;
	}
#endif	/*               */
	info->address = addr;

	info->cs = gate_segment(*val);
	info->flags = val->dpl;
	/*                          */
	if (val->type == GATE_INTERRUPT)
		info->flags |= 1 << 2;

	return 1;
}

/*                             */
static DEFINE_PER_CPU(struct desc_ptr, idt_desc);

/*                                                                 
                    */
static void xen_write_idt_entry(gate_desc *dt, int entrynum, const gate_desc *g)
{
	unsigned long p = (unsigned long)&dt[entrynum];
	unsigned long start, end;

	trace_xen_cpu_write_idt_entry(dt, entrynum, g);

	preempt_disable();

	start = __this_cpu_read(idt_desc.address);
	end = start + __this_cpu_read(idt_desc.size) + 1;

	xen_mc_flush();

	native_write_idt_entry(dt, entrynum, g);

	if (p >= start && (p + 8) <= end) {
		struct trap_info info[2];

		info[1].address = 0;

		if (cvt_gate_to_trap(entrynum, g, &info[0]))
			if (HYPERVISOR_set_trap_table(info))
				BUG();
	}

	preempt_enable();
}

static void xen_convert_trap_info(const struct desc_ptr *desc,
				  struct trap_info *traps)
{
	unsigned in, out, count;

	count = (desc->size+1) / sizeof(gate_desc);
	BUG_ON(count > 256);

	for (in = out = 0; in < count; in++) {
		gate_desc *entry = (gate_desc*)(desc->address) + in;

		if (cvt_gate_to_trap(in, entry, &traps[out]))
			out++;
	}
	traps[out].address = 0;
}

void xen_copy_trap_info(struct trap_info *traps)
{
	const struct desc_ptr *desc = &__get_cpu_var(idt_desc);

	xen_convert_trap_info(desc, traps);
}

/*                                                                  
                                                                      
                                                 */
static void xen_load_idt(const struct desc_ptr *desc)
{
	static DEFINE_SPINLOCK(lock);
	static struct trap_info traps[257];

	trace_xen_cpu_load_idt(desc);

	spin_lock(&lock);

	__get_cpu_var(idt_desc) = *desc;

	xen_convert_trap_info(desc, traps);

	xen_mc_flush();
	if (HYPERVISOR_set_trap_table(traps))
		BUG();

	spin_unlock(&lock);
}

/*                                                             
                                */
static void xen_write_gdt_entry(struct desc_struct *dt, int entry,
				const void *desc, int type)
{
	trace_xen_cpu_write_gdt_entry(dt, entry, desc, type);

	preempt_disable();

	switch (type) {
	case DESC_LDT:
	case DESC_TSS:
		/*        */
		break;

	default: {
		xmaddr_t maddr = arbitrary_virt_to_machine(&dt[entry]);

		xen_mc_flush();
		if (HYPERVISOR_update_descriptor(maddr.maddr, *(u64 *)desc))
			BUG();
	}

	}

	preempt_enable();
}

/*
                                                                  
                                         
 */
static void __init xen_write_gdt_entry_boot(struct desc_struct *dt, int entry,
					    const void *desc, int type)
{
	trace_xen_cpu_write_gdt_entry(dt, entry, desc, type);

	switch (type) {
	case DESC_LDT:
	case DESC_TSS:
		/*        */
		break;

	default: {
		xmaddr_t maddr = virt_to_machine(&dt[entry]);

		if (HYPERVISOR_update_descriptor(maddr.maddr, *(u64 *)desc))
			dt[entry] = *(struct desc_struct *)desc;
	}

	}
}

static void xen_load_sp0(struct tss_struct *tss,
			 struct thread_struct *thread)
{
	struct multicall_space mcs;

	mcs = xen_mc_entry(0);
	MULTI_stack_switch(mcs.mc, __KERNEL_DS, thread->sp0);
	xen_mc_issue(PARAVIRT_LAZY_CPU);
}

static void xen_set_iopl_mask(unsigned mask)
{
	struct physdev_set_iopl set_iopl;

	/*                             */
	set_iopl.iopl = (mask == 0) ? 1 : (mask >> 12) & 3;
	HYPERVISOR_physdev_op(PHYSDEVOP_set_iopl, &set_iopl);
}

static void xen_io_delay(void)
{
}

#ifdef CONFIG_X86_LOCAL_APIC
static unsigned long xen_set_apic_id(unsigned int x)
{
	WARN_ON(1);
	return x;
}
static unsigned int xen_get_apic_id(unsigned long x)
{
	return ((x)>>24) & 0xFFu;
}
static u32 xen_apic_read(u32 reg)
{
	struct xen_platform_op op = {
		.cmd = XENPF_get_cpuinfo,
		.interface_version = XENPF_INTERFACE_VERSION,
		.u.pcpu_info.xen_cpuid = 0,
	};
	int ret = 0;

	/*                                                              
                                                          */
	if (!xen_initial_domain() || smp_processor_id())
		return 0;

	if (reg == APIC_LVR)
		return 0x10;

	if (reg != APIC_ID)
		return 0;

	ret = HYPERVISOR_dom0_op(&op);
	if (ret)
		return 0;

	return op.u.pcpu_info.apic_id << 24;
}

static void xen_apic_write(u32 reg, u32 val)
{
	/*                                             */
	WARN_ON(1);
}

static u64 xen_apic_icr_read(void)
{
	return 0;
}

static void xen_apic_icr_write(u32 low, u32 id)
{
	/*                                             */
	WARN_ON(1);
}

static void xen_apic_wait_icr_idle(void)
{
        return;
}

static u32 xen_safe_apic_wait_icr_idle(void)
{
        return 0;
}

static void set_xen_basic_apic_ops(void)
{
	apic->read = xen_apic_read;
	apic->write = xen_apic_write;
	apic->icr_read = xen_apic_icr_read;
	apic->icr_write = xen_apic_icr_write;
	apic->wait_icr_idle = xen_apic_wait_icr_idle;
	apic->safe_wait_icr_idle = xen_safe_apic_wait_icr_idle;
	apic->set_apic_id = xen_set_apic_id;
	apic->get_apic_id = xen_get_apic_id;

#ifdef CONFIG_SMP
	apic->send_IPI_allbutself = xen_send_IPI_allbutself;
	apic->send_IPI_mask_allbutself = xen_send_IPI_mask_allbutself;
	apic->send_IPI_mask = xen_send_IPI_mask;
	apic->send_IPI_all = xen_send_IPI_all;
	apic->send_IPI_self = xen_send_IPI_self;
#endif
}

#endif

static void xen_clts(void)
{
	struct multicall_space mcs;

	mcs = xen_mc_entry(0);

	MULTI_fpu_taskswitch(mcs.mc, 0);

	xen_mc_issue(PARAVIRT_LAZY_CPU);
}

static DEFINE_PER_CPU(unsigned long, xen_cr0_value);

static unsigned long xen_read_cr0(void)
{
	unsigned long cr0 = this_cpu_read(xen_cr0_value);

	if (unlikely(cr0 == 0)) {
		cr0 = native_read_cr0();
		this_cpu_write(xen_cr0_value, cr0);
	}

	return cr0;
}

static void xen_write_cr0(unsigned long cr0)
{
	struct multicall_space mcs;

	this_cpu_write(xen_cr0_value, cr0);

	/*                                                 
             */
	mcs = xen_mc_entry(0);

	MULTI_fpu_taskswitch(mcs.mc, (cr0 & X86_CR0_TS) != 0);

	xen_mc_issue(PARAVIRT_LAZY_CPU);
}

static void xen_write_cr4(unsigned long cr4)
{
	cr4 &= ~X86_CR4_PGE;
	cr4 &= ~X86_CR4_PSE;

	native_write_cr4(cr4);
}
#ifdef CONFIG_X86_64
static inline unsigned long xen_read_cr8(void)
{
	return 0;
}
static inline void xen_write_cr8(unsigned long val)
{
	BUG_ON(val);
}
#endif
static int xen_write_msr_safe(unsigned int msr, unsigned low, unsigned high)
{
	int ret;

	ret = 0;

	switch (msr) {
#ifdef CONFIG_X86_64
		unsigned which;
		u64 base;

	case MSR_FS_BASE:		which = SEGBASE_FS; goto set;
	case MSR_KERNEL_GS_BASE:	which = SEGBASE_GS_USER; goto set;
	case MSR_GS_BASE:		which = SEGBASE_GS_KERNEL; goto set;

	set:
		base = ((u64)high << 32) | low;
		if (HYPERVISOR_set_segment_base(which, base) != 0)
			ret = -EIO;
		break;
#endif

	case MSR_STAR:
	case MSR_CSTAR:
	case MSR_LSTAR:
	case MSR_SYSCALL_MASK:
	case MSR_IA32_SYSENTER_CS:
	case MSR_IA32_SYSENTER_ESP:
	case MSR_IA32_SYSENTER_EIP:
		/*                                                 
                                                       
                        */
		break;

	case MSR_IA32_CR_PAT:
		if (smp_processor_id() == 0)
			xen_set_pat(((u64)high << 32) | low);
		break;

	default:
		ret = native_write_msr_safe(msr, low, high);
	}

	return ret;
}

void xen_setup_shared_info(void)
{
	if (!xen_feature(XENFEAT_auto_translated_physmap)) {
		set_fixmap(FIX_PARAVIRT_BOOTMAP,
			   xen_start_info->shared_info);

		HYPERVISOR_shared_info =
			(struct shared_info *)fix_to_virt(FIX_PARAVIRT_BOOTMAP);
	} else
		HYPERVISOR_shared_info =
			(struct shared_info *)__va(xen_start_info->shared_info);

#ifndef CONFIG_SMP
	/*                                                            */
	xen_setup_vcpu_info_placement();
#endif

	xen_setup_mfn_list_list();
}

/*                                                   */
void xen_setup_vcpu_info_placement(void)
{
	int cpu;

	for_each_possible_cpu(cpu)
		xen_vcpu_setup(cpu);

	/*                                                         
                                                */
	if (have_vcpu_info_placement) {
		pv_irq_ops.save_fl = __PV_IS_CALLEE_SAVE(xen_save_fl_direct);
		pv_irq_ops.restore_fl = __PV_IS_CALLEE_SAVE(xen_restore_fl_direct);
		pv_irq_ops.irq_disable = __PV_IS_CALLEE_SAVE(xen_irq_disable_direct);
		pv_irq_ops.irq_enable = __PV_IS_CALLEE_SAVE(xen_irq_enable_direct);
		pv_mmu_ops.read_cr2 = xen_read_cr2_direct;
	}
}

static unsigned xen_patch(u8 type, u16 clobbers, void *insnbuf,
			  unsigned long addr, unsigned len)
{
	char *start, *end, *reloc;
	unsigned ret;

	start = end = reloc = NULL;

#define SITE(op, x)							\
	case PARAVIRT_PATCH(op.x):					\
	if (have_vcpu_info_placement) {					\
		start = (char *)xen_##x##_direct;			\
		end = xen_##x##_direct_end;				\
		reloc = xen_##x##_direct_reloc;				\
	}								\
	goto patch_site

	switch (type) {
		SITE(pv_irq_ops, irq_enable);
		SITE(pv_irq_ops, irq_disable);
		SITE(pv_irq_ops, save_fl);
		SITE(pv_irq_ops, restore_fl);
#undef SITE

	patch_site:
		if (start == NULL || (end-start) > len)
			goto default_patch;

		ret = paravirt_patch_insns(insnbuf, len, start, end);

		/*                                                    
                                                       
                                                     
          */
		if (reloc > start && reloc < end) {
			int reloc_off = reloc - start;
			long *relocp = (long *)(insnbuf + reloc_off);
			long delta = start - (char *)addr;

			*relocp += delta;
		}
		break;

	default_patch:
	default:
		ret = paravirt_patch_default(type, clobbers, insnbuf,
					     addr, len);
		break;
	}

	return ret;
}

static const struct pv_info xen_info __initconst = {
	.paravirt_enabled = 1,
	.shared_kernel_pmd = 0,

#ifdef CONFIG_X86_64
	.extra_user_64bit_cs = FLAT_USER_CS64,
#endif

	.name = "Xen",
};

static const struct pv_init_ops xen_init_ops __initconst = {
	.patch = xen_patch,
};

static const struct pv_cpu_ops xen_cpu_ops __initconst = {
	.cpuid = xen_cpuid,

	.set_debugreg = xen_set_debugreg,
	.get_debugreg = xen_get_debugreg,

	.clts = xen_clts,

	.read_cr0 = xen_read_cr0,
	.write_cr0 = xen_write_cr0,

	.read_cr4 = native_read_cr4,
	.read_cr4_safe = native_read_cr4_safe,
	.write_cr4 = xen_write_cr4,

#ifdef CONFIG_X86_64
	.read_cr8 = xen_read_cr8,
	.write_cr8 = xen_write_cr8,
#endif

	.wbinvd = native_wbinvd,

	.read_msr = native_read_msr_safe,
	.write_msr = xen_write_msr_safe,

	.read_tsc = native_read_tsc,
	.read_pmc = native_read_pmc,

	.read_tscp = native_read_tscp,

	.iret = xen_iret,
	.irq_enable_sysexit = xen_sysexit,
#ifdef CONFIG_X86_64
	.usergs_sysret32 = xen_sysret32,
	.usergs_sysret64 = xen_sysret64,
#endif

	.load_tr_desc = paravirt_nop,
	.set_ldt = xen_set_ldt,
	.load_gdt = xen_load_gdt,
	.load_idt = xen_load_idt,
	.load_tls = xen_load_tls,
#ifdef CONFIG_X86_64
	.load_gs_index = xen_load_gs_index,
#endif

	.alloc_ldt = xen_alloc_ldt,
	.free_ldt = xen_free_ldt,

	.store_idt = native_store_idt,
	.store_tr = xen_store_tr,

	.write_ldt_entry = xen_write_ldt_entry,
	.write_gdt_entry = xen_write_gdt_entry,
	.write_idt_entry = xen_write_idt_entry,
	.load_sp0 = xen_load_sp0,

	.set_iopl_mask = xen_set_iopl_mask,
	.io_delay = xen_io_delay,

	/*                                                         */
	.swapgs = paravirt_nop,

	.start_context_switch = paravirt_start_context_switch,
	.end_context_switch = xen_end_context_switch,
};

static const struct pv_apic_ops xen_apic_ops __initconst = {
#ifdef CONFIG_X86_LOCAL_APIC
	.startup_ipi_hook = paravirt_nop,
#endif
};

static void xen_reboot(int reason)
{
	struct sched_shutdown r = { .reason = reason };

	if (HYPERVISOR_sched_op(SCHEDOP_shutdown, &r))
		BUG();
}

static void xen_restart(char *msg)
{
	xen_reboot(SHUTDOWN_reboot);
}

static void xen_emergency_restart(void)
{
	xen_reboot(SHUTDOWN_reboot);
}

static void xen_machine_halt(void)
{
	xen_reboot(SHUTDOWN_poweroff);
}

static void xen_machine_power_off(void)
{
	if (pm_power_off)
		pm_power_off();
	xen_reboot(SHUTDOWN_poweroff);
}

static void xen_crash_shutdown(struct pt_regs *regs)
{
	xen_reboot(SHUTDOWN_crash);
}

static int
xen_panic_event(struct notifier_block *this, unsigned long event, void *ptr)
{
	xen_reboot(SHUTDOWN_crash);
	return NOTIFY_DONE;
}

static struct notifier_block xen_panic_block = {
	.notifier_call= xen_panic_event,
};

int xen_panic_handler_init(void)
{
	atomic_notifier_chain_register(&panic_notifier_list, &xen_panic_block);
	return 0;
}

static const struct machine_ops xen_machine_ops __initconst = {
	.restart = xen_restart,
	.halt = xen_machine_halt,
	.power_off = xen_machine_power_off,
	.shutdown = xen_machine_halt,
	.crash_shutdown = xen_crash_shutdown,
	.emergency_restart = xen_emergency_restart,
};

static void __init xen_boot_params_init_edd(void)
{
#if IS_ENABLED(CONFIG_EDD)
	struct xen_platform_op op;
	struct edd_info *edd_info;
	u32 *mbr_signature;
	unsigned nr;
	int ret;

	edd_info = boot_params.eddbuf;
	mbr_signature = boot_params.edd_mbr_sig_buffer;

	op.cmd = XENPF_firmware_info;

	op.u.firmware_info.type = XEN_FW_DISK_INFO;
	for (nr = 0; nr < EDDMAXNR; nr++) {
		struct edd_info *info = edd_info + nr;

		op.u.firmware_info.index = nr;
		info->params.length = sizeof(info->params);
		set_xen_guest_handle(op.u.firmware_info.u.disk_info.edd_params,
				     &info->params);
		ret = HYPERVISOR_dom0_op(&op);
		if (ret)
			break;

#define C(x) info->x = op.u.firmware_info.u.disk_info.x
		C(device);
		C(version);
		C(interface_support);
		C(legacy_max_cylinder);
		C(legacy_max_head);
		C(legacy_sectors_per_track);
#undef C
	}
	boot_params.eddbuf_entries = nr;

	op.u.firmware_info.type = XEN_FW_DISK_MBR_SIGNATURE;
	for (nr = 0; nr < EDD_MBR_SIG_MAX; nr++) {
		op.u.firmware_info.index = nr;
		ret = HYPERVISOR_dom0_op(&op);
		if (ret)
			break;
		mbr_signature[nr] = op.u.firmware_info.u.disk_mbr_signature.mbr_signature;
	}
	boot_params.edd_mbr_sig_buf_entries = nr;
#endif
}

/*
                                                                     
                                                                    
                                         
 */
static void __init xen_setup_stackprotector(void)
{
	pv_cpu_ops.write_gdt_entry = xen_write_gdt_entry_boot;
	pv_cpu_ops.load_gdt = xen_load_gdt_boot;

	setup_stack_canary_segment(0);
	switch_to_new_gdt(0);

	pv_cpu_ops.write_gdt_entry = xen_write_gdt_entry;
	pv_cpu_ops.load_gdt = xen_load_gdt;
}

/*                                           */
asmlinkage void __init xen_start_kernel(void)
{
	struct physdev_set_iopl set_iopl;
	int rc;

	if (!xen_start_info)
		return;

	xen_domain_type = XEN_PV_DOMAIN;

	xen_setup_machphys_mapping();

	/*                          */
	pv_info = xen_info;
	pv_init_ops = xen_init_ops;
	pv_cpu_ops = xen_cpu_ops;
	pv_apic_ops = xen_apic_ops;

	x86_init.resources.memory_setup = xen_memory_setup;
	x86_init.oem.arch_setup = xen_arch_setup;
	x86_init.oem.banner = xen_banner;

	xen_init_time_ops();

	/*
                                                                
  */

	xen_init_mmu_ops();

	/*                                               */
	__supported_pte_mask &= ~_PAGE_GLOBAL;
#if 0
	if (!xen_initial_domain())
#endif
		__supported_pte_mask &= ~(_PAGE_PWT | _PAGE_PCD);

	__supported_pte_mask |= _PAGE_IOMAP;

	/*
                                                             
                                 
  */
	__userpte_alloc_gfp &= ~__GFP_HIGHMEM;

	/*                           */
	x86_configure_nx();

	xen_setup_features();

	/*              */
	if (!xen_feature(XENFEAT_auto_translated_physmap))
		xen_build_dynamic_phys_to_machine();

	/*
                                                           
                                           
  */
	xen_setup_stackprotector();

	xen_init_irq_ops();
	xen_init_cpuid_mask();

#ifdef CONFIG_X86_LOCAL_APIC
	/*
                              
  */
	set_xen_basic_apic_ops();
#endif

	if (xen_feature(XENFEAT_mmu_pt_update_preserve_ad)) {
		pv_mmu_ops.ptep_modify_prot_start = xen_ptep_modify_prot_start;
		pv_mmu_ops.ptep_modify_prot_commit = xen_ptep_modify_prot_commit;
	}

	machine_ops = xen_machine_ops;

	/*
                                                              
                                                            
                                                            
  */
	xen_initial_gdt = &per_cpu(gdt_page, 0);

	xen_smp_init();

#ifdef CONFIG_ACPI_NUMA
	/*
                                                              
                                                               
                                            
  */
	acpi_numa = -1;
#endif
#ifdef CONFIG_X86_PAT
	/*
                                                             
                                                       
                                                       
  */
	pat_enabled = 0;
#endif
	/*                                                            
                                              */
	per_cpu(xen_vcpu, 0) = &HYPERVISOR_shared_info->vcpu_info[0];

	local_irq_disable();
	early_boot_irqs_disabled = true;

	xen_raw_console_write("mapping kernel into physical memory\n");
	xen_setup_kernel_pagetable((pgd_t *)xen_start_info->pt_base, xen_start_info->nr_pages);

	/*                                                                  */
	xen_build_mfn_list_list();

	/*                                                         */

#ifdef CONFIG_X86_32
	pv_info.kernel_rpl = 1;
	if (xen_feature(XENFEAT_supervisor_mode_kernel))
		pv_info.kernel_rpl = 0;
#else
	pv_info.kernel_rpl = 0;
#endif
	/*                                    */
	xen_reserve_top();

	/*                                                                  
                                                                        
                           
  */
	set_iopl.iopl = 1;
	rc = HYPERVISOR_physdev_op(PHYSDEVOP_set_iopl, &set_iopl);
	if (rc != 0)
		xen_raw_printk("physdev_op failed %d\n", rc);

#ifdef CONFIG_X86_32
	/*                          */
	cpu_detect(&new_cpu_data);
	new_cpu_data.hard_math = 1;
	new_cpu_data.wp_works_ok = 1;
	new_cpu_data.x86_capability[0] = cpuid_edx(1);
#endif

	/*                                             */
	boot_params.hdr.type_of_loader = (9 << 4) | 0;
	boot_params.hdr.ramdisk_image = xen_start_info->mod_start
		? __pa(xen_start_info->mod_start) : 0;
	boot_params.hdr.ramdisk_size = xen_start_info->mod_len;
	boot_params.hdr.cmd_line_ptr = __pa(xen_start_info->cmd_line);

	if (!xen_initial_domain()) {
		add_preferred_console("xenboot", 0, NULL);
		add_preferred_console("tty", 0, NULL);
		add_preferred_console("hvc", 0, NULL);
		if (pci_xen)
			x86_init.pci.arch_init = pci_xen_init;
	} else {
		const struct dom0_vga_console_info *info =
			(void *)((char *)xen_start_info +
				 xen_start_info->console.dom0.info_off);
		struct xen_platform_op op = {
			.cmd = XENPF_firmware_info,
			.interface_version = XENPF_INTERFACE_VERSION,
			.u.firmware_info.type = XEN_FW_KBD_SHIFT_FLAGS,
		};

		xen_init_vga(info, xen_start_info->console.dom0.info_size);
		xen_start_info->console.domU.mfn = 0;
		xen_start_info->console.domU.evtchn = 0;

		if (HYPERVISOR_dom0_op(&op) == 0)
			boot_params.kbd_status = op.u.firmware_info.u.kbd_shift_flags;

		xen_init_apic();

		/*                               */
		pci_request_acs();

		xen_acpi_sleep_register();

		/*                                    */
		x86_init.mpparse.find_smp_config = x86_init_noop;
		x86_init.mpparse.get_smp_config = x86_init_uint_noop;

		xen_boot_params_init_edd();
	}
#ifdef CONFIG_PCI
	/*                                              */
	pci_probe &= ~PCI_PROBE_BIOS;
#endif
	xen_raw_console_write("about to get started...\n");

	xen_setup_runstate_info(0);

	/*                 */
#ifdef CONFIG_X86_32
	i386_start_kernel();
#else
	x86_64_start_reservations((char *)__pa_symbol(&boot_params));
#endif
}

void __ref xen_hvm_init_shared_info(void)
{
	int cpu;
	struct xen_add_to_physmap xatp;
	static struct shared_info *shared_info_page = 0;

	if (!shared_info_page)
		shared_info_page = (struct shared_info *)
			extend_brk(PAGE_SIZE, PAGE_SIZE);
	xatp.domid = DOMID_SELF;
	xatp.idx = 0;
	xatp.space = XENMAPSPACE_shared_info;
	xatp.gpfn = __pa(shared_info_page) >> PAGE_SHIFT;
	if (HYPERVISOR_memory_op(XENMEM_add_to_physmap, &xatp))
		BUG();

	HYPERVISOR_shared_info = (struct shared_info *)shared_info_page;

	/*                                                                 
                                                                   
                                                            
                                                                 
        
                                                                    
                                                                     
                                                 */
	for_each_online_cpu(cpu) {
		/*                      */
		if (cpu >= MAX_VIRT_CPUS)
			continue;
		per_cpu(xen_vcpu, cpu) = &HYPERVISOR_shared_info->vcpu_info[cpu];
	}
}

#ifdef CONFIG_XEN_PVHVM
static void __init init_hvm_pv_info(void)
{
	int major, minor;
	uint32_t eax, ebx, ecx, edx, pages, msr, base;
	u64 pfn;

	base = xen_cpuid_base();
	cpuid(base + 1, &eax, &ebx, &ecx, &edx);

	major = eax >> 16;
	minor = eax & 0xffff;
	printk(KERN_INFO "Xen version %d.%d.\n", major, minor);

	cpuid(base + 2, &pages, &msr, &ecx, &edx);

	pfn = __pa(hypercall_page);
	wrmsr_safe(msr, (u32)pfn, (u32)(pfn >> 32));

	xen_setup_features();

	pv_info.name = "Xen HVM";

	xen_domain_type = XEN_HVM_DOMAIN;
}

static int __cpuinit xen_hvm_cpu_notify(struct notifier_block *self,
				    unsigned long action, void *hcpu)
{
	int cpu = (long)hcpu;
	switch (action) {
	case CPU_UP_PREPARE:
		xen_vcpu_setup(cpu);
		if (xen_have_vector_callback) {
			xen_init_lock_cpu(cpu);
			if (xen_feature(XENFEAT_hvm_safe_pvclock))
				xen_setup_timer(cpu);
		}
		break;
	default:
		break;
	}
	return NOTIFY_OK;
}

static struct notifier_block xen_hvm_cpu_notifier __cpuinitdata = {
	.notifier_call	= xen_hvm_cpu_notify,
};

static void __init xen_hvm_guest_init(void)
{
	init_hvm_pv_info();

	xen_hvm_init_shared_info();

	if (xen_feature(XENFEAT_hvm_callback_vector))
		xen_have_vector_callback = 1;
	xen_hvm_smp_init();
	register_cpu_notifier(&xen_hvm_cpu_notifier);
	xen_unplug_emulated_devices();
	x86_init.irqs.intr_init = xen_init_IRQ;
	xen_hvm_init_time_ops();
	xen_hvm_init_mmu_ops();
}

static bool __init xen_hvm_platform(void)
{
	if (xen_pv_domain())
		return false;

	if (!xen_cpuid_base())
		return false;

	return true;
}

bool xen_hvm_need_lapic(void)
{
	if (xen_pv_domain())
		return false;
	if (!xen_hvm_domain())
		return false;
	if (xen_feature(XENFEAT_hvm_pirqs) && xen_have_vector_callback)
		return false;
	return true;
}
EXPORT_SYMBOL_GPL(xen_hvm_need_lapic);

const struct hypervisor_x86 x86_hyper_xen_hvm __refconst = {
	.name			= "Xen HVM",
	.detect			= xen_hvm_platform,
	.init_platform		= xen_hvm_guest_init,
	.x2apic_available	= xen_x2apic_para_available,
};
EXPORT_SYMBOL(x86_hyper_xen_hvm);
#endif