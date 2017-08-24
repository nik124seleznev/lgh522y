#ifndef _ASM_S390_MODULE_H
#define _ASM_S390_MODULE_H

#include <asm-generic/module.h>

/*
                                                                 
 */

struct mod_arch_syminfo
{
	unsigned long got_offset;
	unsigned long plt_offset;
	int got_initialized;
	int plt_initialized;
};

struct mod_arch_specific
{
	/*                                                   */
	unsigned long got_offset;
	/*                                                   */
	unsigned long plt_offset;
	/*                  */
	unsigned long got_size;
	/*                  */
	unsigned long plt_size;
	/*                               */
	int nsyms;
	/*                                                      */
	struct mod_arch_syminfo *syminfo;
};

#endif /*                    */
