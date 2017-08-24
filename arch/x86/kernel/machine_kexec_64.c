/*
 * handle transition of Linux booting another kernel
 * Copyright (C) 2002-2005 Eric Biederman  <ebiederm@xmission.com>
 *
 * This source code is licensed under the GNU General Public License,
 * Version 2.  See the file COPYING for more details.
 */

#include <linux/mm.h>
#include <linux/kexec.h>
#include <linux/string.h>
#include <linux/gfp.h>
#include <linux/reboot.h>
#include <linux/numa.h>
#include <linux/ftrace.h>
#include <linux/io.h>
#include <linux/suspend.h>

#include <asm/init.h>
#include <asm/pgtable.h>
#include <asm/tlbflush.h>
#include <asm/mmu_context.h>
#include <asm/debugreg.h>

static void free_transition_pgtable(struct kimage *image)
{
	free_page((unsigned long)image->arch.pud);
	free_page((unsigned long)image->arch.pmd);
	free_page((unsigned long)image->arch.pte);
}

static int init_transition_pgtable(struct kimage *image, pgd_t *pgd)
{
	pud_t *pud;
	pmd_t *pmd;
	pte_t *pte;
	unsigned long vaddr, paddr;
	int result = -ENOMEM;

	vaddr = (unsigned long)relocate_kernel;
	paddr = __pa(page_address(image->control_code_page)+PAGE_SIZE);
	pgd += pgd_index(vaddr);
	if (!pgd_present(*pgd)) {
		pud = (pud_t *)get_zeroed_page(GFP_KERNEL);
		if (!pud)
			goto err;
		image->arch.pud = pud;
		set_pgd(pgd, __pgd(__pa(pud) | _KERNPG_TABLE));
	}
	pud = pud_offset(pgd, vaddr);
	if (!pud_present(*pud)) {
		pmd = (pmd_t *)get_zeroed_page(GFP_KERNEL);
		if (!pmd)
			goto err;
		image->arch.pmd = pmd;
		set_pud(pud, __pud(__pa(pmd) | _KERNPG_TABLE));
	}
	pmd = pmd_offset(pud, vaddr);
	if (!pmd_present(*pmd)) {
		pte = (pte_t *)get_zeroed_page(GFP_KERNEL);
		if (!pte)
			goto err;
		image->arch.pte = pte;
		set_pmd(pmd, __pmd(__pa(pte) | _KERNPG_TABLE));
	}
	pte = pte_offset_kernel(pmd, vaddr);
	set_pte(pte, pfn_pte(paddr >> PAGE_SHIFT, PAGE_KERNEL_EXEC));
	return 0;
err:
	free_transition_pgtable(image);
	return result;
}

static void *alloc_pgt_page(void *data)
{
	struct kimage *image = (struct kimage *)data;
	struct page *page;
	void *p = NULL;

	page = kimage_alloc_control_pages(image, 0);
	if (page) {
		p = page_address(page);
		clear_page(p);
	}

	return p;
}

static int init_pgtable(struct kimage *image, unsigned long start_pgtable)
{
	struct x86_mapping_info info = {
		.alloc_pgt_page	= alloc_pgt_page,
		.context	= image,
		.pmd_flag	= __PAGE_KERNEL_LARGE_EXEC,
	};
	unsigned long mstart, mend;
	pgd_t *level4p;
	int result;
	int i;

	level4p = (pgd_t *)__va(start_pgtable);
	clear_page(level4p);
	for (i = 0; i < nr_pfn_mapped; i++) {
		mstart = pfn_mapped[i].start << PAGE_SHIFT;
		mend   = pfn_mapped[i].end << PAGE_SHIFT;

		result = kernel_ident_mapping_init(&info,
						 level4p, mstart, mend);
		if (result)
			return result;
	}

	/*
                                                       
                                                                      
                                                                  
                                      
  */
	for (i = 0; i < image->nr_segments; i++) {
		mstart = image->segment[i].mem;
		mend   = mstart + image->segment[i].memsz;

		result = kernel_ident_mapping_init(&info,
						 level4p, mstart, mend);

		if (result)
			return result;
	}

	return init_transition_pgtable(image, level4p);
}

static void set_idt(void *newidt, u16 limit)
{
	struct desc_ptr curidt;

	/*                                         */
	curidt.size    = limit;
	curidt.address = (unsigned long)newidt;

	__asm__ __volatile__ (
		"lidtq %0\n"
		: : "m" (curidt)
		);
};


static void set_gdt(void *newgdt, u16 limit)
{
	struct desc_ptr curgdt;

	/*                                          */
	curgdt.size    = limit;
	curgdt.address = (unsigned long)newgdt;

	__asm__ __volatile__ (
		"lgdtq %0\n"
		: : "m" (curgdt)
		);
};

static void load_segments(void)
{
	__asm__ __volatile__ (
		"\tmovl %0,%%ds\n"
		"\tmovl %0,%%es\n"
		"\tmovl %0,%%ss\n"
		"\tmovl %0,%%fs\n"
		"\tmovl %0,%%gs\n"
		: : "a" (__KERNEL_DS) : "memory"
		);
}

int machine_kexec_prepare(struct kimage *image)
{
	unsigned long start_pgtable;
	int result;

	/*                       */
	start_pgtable = page_to_pfn(image->control_code_page) << PAGE_SHIFT;

	/*                                            */
	result = init_pgtable(image, start_pgtable);
	if (result)
		return result;

	return 0;
}

void machine_kexec_cleanup(struct kimage *image)
{
	free_transition_pgtable(image);
}

/*
                                                                  
                                                                  
 */
void machine_kexec(struct kimage *image)
{
	unsigned long page_list[PAGES_NR];
	void *control_page;
	int save_ftrace_enabled;

#ifdef CONFIG_KEXEC_JUMP
	if (image->preserve_context)
		save_processor_state();
#endif

	save_ftrace_enabled = __ftrace_enabled_save();

	/*                                              */
	local_irq_disable();
	hw_breakpoint_disable();

	if (image->preserve_context) {
#ifdef CONFIG_X86_IO_APIC
		/*
                                                       
                                                       
                                                     
                                                 
         
   */
		disable_IO_APIC();
#endif
	}

	control_page = page_address(image->control_code_page) + PAGE_SIZE;
	memcpy(control_page, relocate_kernel, KEXEC_CONTROL_CODE_MAX_SIZE);

	page_list[PA_CONTROL_PAGE] = virt_to_phys(control_page);
	page_list[VA_CONTROL_PAGE] = (unsigned long)control_page;
	page_list[PA_TABLE_PAGE] =
	  (unsigned long)__pa(page_address(image->control_code_page));

	if (image->type == KEXEC_TYPE_DEFAULT)
		page_list[PA_SWAP_PAGE] = (page_to_pfn(image->swap_page)
						<< PAGE_SHIFT);

	/*
                                                            
                                                                
                                                            
                                                         
                                        
   
                                                      
                                                         
  */
	load_segments();
	/*
                                  
                                                                
  */
	set_gdt(phys_to_virt(0), 0);
	set_idt(phys_to_virt(0), 0);

	/*             */
	image->start = relocate_kernel((unsigned long)image->head,
				       (unsigned long)page_list,
				       image->start,
				       image->preserve_context);

#ifdef CONFIG_KEXEC_JUMP
	if (image->preserve_context)
		restore_processor_state();
#endif

	__ftrace_enabled_restore(save_ftrace_enabled);
}

void arch_crash_save_vmcoreinfo(void)
{
	VMCOREINFO_SYMBOL(phys_base);
	VMCOREINFO_SYMBOL(init_level4_pgt);

#ifdef CONFIG_NUMA
	VMCOREINFO_SYMBOL(node_data);
	VMCOREINFO_LENGTH(node_data, MAX_NUMNODES);
#endif
}

