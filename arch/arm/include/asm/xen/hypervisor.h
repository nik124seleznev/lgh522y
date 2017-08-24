#ifndef _ASM_ARM_XEN_HYPERVISOR_H
#define _ASM_ARM_XEN_HYPERVISOR_H

extern struct shared_info *HYPERVISOR_shared_info;
extern struct start_info *xen_start_info;

/*                                                 */
enum paravirt_lazy_mode {
	PARAVIRT_LAZY_NONE,
	PARAVIRT_LAZY_MMU,
	PARAVIRT_LAZY_CPU,
};

static inline enum paravirt_lazy_mode paravirt_get_lazy_mode(void)
{
	return PARAVIRT_LAZY_NONE;
}

#endif /*                           */
