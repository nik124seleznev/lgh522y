/* elf-fdpic.h: FDPIC ELF load map
 *
 * Copyright (C) 2003 Red Hat, Inc. All Rights Reserved.
 * Written by David Howells (dhowells@redhat.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef _UAPI_LINUX_ELF_FDPIC_H
#define _UAPI_LINUX_ELF_FDPIC_H

#include <linux/elf.h>

#define PT_GNU_STACK    (PT_LOOS + 0x474e551)

/*                                                                   */
struct elf32_fdpic_loadseg {
	Elf32_Addr	addr;		/*                              */
	Elf32_Addr	p_vaddr;	/*                      */
	Elf32_Word	p_memsz;	/*                                  */
};

struct elf32_fdpic_loadmap {
	Elf32_Half	version;	/*                                              */
	Elf32_Half	nsegs;		/*                    */
	struct elf32_fdpic_loadseg segs[];
};

#define ELF32_FDPIC_LOADMAP_VERSION	0x0000

#endif /*                         */
