/*
 * Copyright (C) 2012 ARM Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _UAPI__ASM_SIGCONTEXT_H
#define _UAPI__ASM_SIGCONTEXT_H

#include <linux/types.h>

/*
                                                                    
                                         
 */
struct sigcontext {
	__u64 fault_address;
	/*                   */
	__u64 regs[31];
	__u64 sp;
	__u64 pc;
	__u64 pstate;
	/*                                                    */
	__u8 __reserved[4096] __attribute__((__aligned__(16)));
};

/*
                                                                      
                                                                             
                                                                          
                           
 */
struct _aarch64_ctx {
	__u32 magic;
	__u32 size;
};

#define FPSIMD_MAGIC	0x46508001

struct fpsimd_context {
	struct _aarch64_ctx head;
	__u32 fpsr;
	__u32 fpcr;
	__uint128_t vregs[32];
};


#endif /*                         */
