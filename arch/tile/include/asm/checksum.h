/*
 * Copyright 2010 Tilera Corporation. All Rights Reserved.
 *
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public License
 *   as published by the Free Software Foundation, version 2.
 *
 *   This program is distributed in the hope that it will be useful, but
 *   WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, GOOD TITLE or
 *   NON INFRINGEMENT.  See the GNU General Public License for
 *   more details.
 */

#ifndef _ASM_TILE_CHECKSUM_H
#define _ASM_TILE_CHECKSUM_H

#include <asm-generic/checksum.h>

/*                                                 */
__wsum do_csum(const unsigned char *buff, int len);
#define do_csum do_csum

/*
                                                       
                                                                   
                                                                   
 */
static inline unsigned int csum_long(unsigned long x)
{
	unsigned long ret;
#ifdef __tilegx__
	ret = __insn_v2sadu(x, 0);
	ret = __insn_v2sadu(ret, 0);
#else
	ret = __insn_sadh_u(x, 0);
	ret = __insn_sadh_u(ret, 0);
#endif
	return ret;
}

#endif /*                      */
