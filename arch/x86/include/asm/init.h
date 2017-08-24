#ifndef _ASM_X86_INIT_H
#define _ASM_X86_INIT_H

struct x86_mapping_info {
	void *(*alloc_pgt_page)(void *); /*                             */
	void *context;			 /*                            */
	unsigned long pmd_flag;		 /*                         */
	bool kernel_mapping;		 /*                                 */
};

int kernel_ident_mapping_init(struct x86_mapping_info *info, pgd_t *pgd_page,
				unsigned long addr, unsigned long end);

#endif /*                 */
