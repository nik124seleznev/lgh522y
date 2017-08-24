/*
 * OpenRISC Linux
 *
 * Linux architectural port borrowing liberally from similar works of
 * others.  All original copyrights apply as per the original source
 * declaration.
 *
 * OpenRISC implementation:
 * Copyright (C) 2003 Matjaz Breskvar <phoenix@bsemi.com>
 * Copyright (C) 2010-2011 Jonas Bonn <jonas@southpole.se>
 * et al.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#ifndef __ASM_OPENRISC_ELF_H
#define __ASM_OPENRISC_ELF_H


#include <linux/types.h>
#include <uapi/asm/elf.h>

/*
                                                                             
 */

#define elf_check_arch(x) \
	(((x)->e_machine == EM_OR32) || ((x)->e_machine == EM_OPENRISC))

/*                                                                           
                                                                           
                                                                             
                                                                        */

#define ELF_ET_DYN_BASE         (0x08000000)

/*
                            
                                           
 */
#define CORE_DUMP_USE_REGSET

#define ELF_EXEC_PAGESIZE	8192

extern void dump_elf_thread(elf_greg_t *dest, struct pt_regs *pt);
#define ELF_CORE_COPY_REGS(dest, regs) dump_elf_thread(dest, regs);

/*                                                                 
                                                                       
                                                       */

#define ELF_HWCAP	(0)

/*                                                                
                                                                 
                                                

                                                                        
                            */

#define ELF_PLATFORM	(NULL)

#endif
