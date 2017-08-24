#include <linux/module.h>
#include <linux/spinlock.h>
#include <linux/sort.h>
#include <asm/uaccess.h>

static inline unsigned long
ex_insn_addr(const struct exception_table_entry *x)
{
	return (unsigned long)&x->insn + x->insn;
}
static inline unsigned long
ex_fixup_addr(const struct exception_table_entry *x)
{
	return (unsigned long)&x->fixup + x->fixup;
}

int fixup_exception(struct pt_regs *regs)
{
	const struct exception_table_entry *fixup;
	unsigned long new_ip;

#ifdef CONFIG_PNPBIOS
	if (unlikely(SEGMENT_IS_PNP_CODE(regs->cs))) {
		extern u32 pnp_bios_fault_eip, pnp_bios_fault_esp;
		extern u32 pnp_bios_is_utter_crap;
		pnp_bios_is_utter_crap = 1;
		printk(KERN_CRIT "PNPBIOS fault.. attempting recovery.\n");
		__asm__ volatile(
			"movl %0, %%esp\n\t"
			"jmp *%1\n\t"
			: : "g" (pnp_bios_fault_esp), "g" (pnp_bios_fault_eip));
		panic("do_trap: can't hit this");
	}
#endif

	fixup = search_exception_tables(regs->ip);
	if (fixup) {
		new_ip = ex_fixup_addr(fixup);

		if (fixup->fixup - fixup->insn >= 0x7ffffff0 - 4) {
			/*                              */
			current_thread_info()->uaccess_err = 1;
			new_ip -= 0x7ffffff0;
		}
		regs->ip = new_ip;
		return 1;
	}

	return 0;
}

/*                                                */
int __init early_fixup_exception(unsigned long *ip)
{
	const struct exception_table_entry *fixup;
	unsigned long new_ip;

	fixup = search_exception_tables(*ip);
	if (fixup) {
		new_ip = ex_fixup_addr(fixup);

		if (fixup->fixup - fixup->insn >= 0x7ffffff0 - 4) {
			/*                                                  */
			return 0;
		}

		*ip = new_ip;
		return 1;
	}

	return 0;
}

/*
                                                               
                                                                  
                            
                                                               
                  
 */
const struct exception_table_entry *
search_extable(const struct exception_table_entry *first,
	       const struct exception_table_entry *last,
	       unsigned long value)
{
	while (first <= last) {
		const struct exception_table_entry *mid;
		unsigned long addr;

		mid = ((last - first) >> 1) + first;
		addr = ex_insn_addr(mid);
		if (addr < value)
			first = mid + 1;
		else if (addr > value)
			last = mid - 1;
		else
			return mid;
        }
        return NULL;
}

/*
                                                            
                                                           
                                                           
                                                   
  
 */
static int cmp_ex(const void *a, const void *b)
{
	const struct exception_table_entry *x = a, *y = b;

	/*
                                                              
                                                            
                     
   
                                                   
  */
	return x->insn - y->insn;
}

void sort_extable(struct exception_table_entry *start,
		  struct exception_table_entry *finish)
{
	struct exception_table_entry *p;
	int i;

	/*                                                                   */
	i = 0;
	for (p = start; p < finish; p++) {
		p->insn += i;
		i += 4;
		p->fixup += i;
		i += 4;
	}

	sort(start, finish - start, sizeof(struct exception_table_entry),
	     cmp_ex, NULL);

	/*                         */
	i = 0;
	for (p = start; p < finish; p++) {
		p->insn -= i;
		i += 4;
		p->fixup -= i;
		i += 4;
	}
}

#ifdef CONFIG_MODULES
/*
                                                                     
                                       
 */
void trim_init_extable(struct module *m)
{
	/*                  */
	while (m->num_exentries &&
	       within_module_init(ex_insn_addr(&m->extable[0]), m)) {
		m->extable++;
		m->num_exentries--;
	}
	/*            */
	while (m->num_exentries &&
	       within_module_init(ex_insn_addr(&m->extable[m->num_exentries-1]), m))
		m->num_exentries--;
}
#endif /*                */
