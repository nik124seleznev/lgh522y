#include <linux/module.h>
#include <linux/sort.h>
#include <asm/uaccess.h>

/*
                                                               
                                                                  
                            
                                                               
                  
 */
const struct exception_table_entry *
search_extable(const struct exception_table_entry *first,
	       const struct exception_table_entry *last,
	       unsigned long value)
{
	const struct exception_table_entry *mid;
	unsigned long addr;

	while (first <= last) {
		mid = ((last - first) >> 1) + first;
		addr = extable_insn(mid);
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

	/*                                                 */
	return x->insn - y->insn;
}

void sort_extable(struct exception_table_entry *start,
		  struct exception_table_entry *finish)
{
	struct exception_table_entry *p;
	int i;

	/*                                                                 */
	for (p = start, i = 0; p < finish; p++, i += 8)
		p->insn += i;
	sort(start, finish - start, sizeof(*start), cmp_ex, NULL);
	/*                         */
	for (p = start, i = 0; p < finish; p++, i += 8)
		p->insn -= i;
}

#ifdef CONFIG_MODULES
/*
                                                                     
                                       
 */
void trim_init_extable(struct module *m)
{
	/*                    */
	while (m->num_exentries &&
	       within_module_init(extable_insn(&m->extable[0]), m)) {
		m->extable++;
		m->num_exentries--;
	}
	/*              */
	while (m->num_exentries &&
	       within_module_init(extable_insn(&m->extable[m->num_exentries-1]), m))
		m->num_exentries--;
}
#endif /*                */
