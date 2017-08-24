/*
 * Copyright (C) 2004, 2007-2010, 2011-2012 Synopsys, Inc. (www.synopsys.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __ASM_ARC_ELF_H
#define __ASM_ARC_ELF_H

#include <linux/types.h>
#include <uapi/asm/elf.h>

/*                                                                      */
#define EM_ARCOMPACT		93

/*                                       */
#define  R_ARC_32		0x4
#define  R_ARC_32_ME		0x1B
#define  R_ARC_S25H_PCREL	0x10
#define  R_ARC_S25W_PCREL	0x11

/*                                    */
#define ELF_ARCH		EM_ARCOMPACT
#define ELF_CLASS		ELFCLASS32

#ifdef CONFIG_CPU_BIG_ENDIAN
#define ELF_DATA		ELFDATA2MSB
#else
#define ELF_DATA		ELFDATA2LSB
#endif

/*
                 
                                                        
                                                   
 */
struct elf32_hdr;
extern int elf_check_arch(const struct elf32_hdr *);
#define elf_check_arch	elf_check_arch

#define CORE_DUMP_USE_REGSET

#define ELF_EXEC_PAGESIZE	PAGE_SIZE

/*
                                                                             
                                                                           
                                                                             
                                                                      
 */
#define ELF_ET_DYN_BASE		(2 * TASK_SIZE / 3)

/*
                                                                     
                                                                      
                        
 */
#define ELF_PLAT_INIT(_r, load_addr)	((_r)->r0 = 0)

/*
                                                                   
                                     
 */
#define ELF_HWCAP	(0)

/*
                                                                  
                                                                 
                                                
 */
#define ELF_PLATFORM	(NULL)

#endif
