/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2012 MIPS Technologies, Inc.
 */
#ifndef __ASM_FW_H_
#define __ASM_FW_H_

#include <asm/bootinfo.h>	/*                     */

enum fw_memtypes {
	fw_dontuse,
	fw_code,
	fw_free,
};

typedef struct {
	unsigned long base;	/*              */
	unsigned int size;	/*       */
	enum fw_memtypes type;	/*             */
} fw_memblock_t;

/*                                             */
#define FW_MAX_MEMBLOCKS	32

extern int fw_argc;
extern int *_fw_argv;
extern int *_fw_envp;

/*
                                                                      
                                                                   
 */
#define fw_argv(index)		((char *)(long)_fw_argv[(index)])
#define fw_envp(index)		((char *)(long)_fw_envp[(index)])

extern void fw_init_cmdline(void);
extern char *fw_getcmdline(void);
extern fw_memblock_t *fw_getmdesc(void);
extern void fw_meminit(void);
extern char *fw_getenv(char *name);
extern unsigned long fw_getenvl(char *name);
extern void fw_init_early_console(char port);

#endif /*             */