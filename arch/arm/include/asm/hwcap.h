#ifndef __ASMARM_HWCAP_H
#define __ASMARM_HWCAP_H

#include <uapi/asm/hwcap.h>

#if !defined(__ASSEMBLY__)
/*
                                                                   
                                     
 */
#define ELF_HWCAP	(elf_hwcap)
extern unsigned int elf_hwcap;
#endif
#endif
