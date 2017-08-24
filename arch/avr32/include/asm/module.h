#ifndef __ASM_AVR32_MODULE_H
#define __ASM_AVR32_MODULE_H

#include <asm-generic/module.h>

struct mod_arch_syminfo {
	unsigned long got_offset;
	int got_initialized;
};

struct mod_arch_specific {
	/*                                                   */
	unsigned long got_offset;
	/*                  */
	unsigned long got_size;
	/*                               */
	int nsyms;
	/*                                              */
	struct mod_arch_syminfo *syminfo;
};

#define MODULE_PROC_FAMILY "AVR32v1"

#define MODULE_ARCH_VERMAGIC MODULE_PROC_FAMILY

#endif /*                      */
