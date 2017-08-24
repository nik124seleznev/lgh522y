/*
 * Copyright (C) 2012 Imagination Technologies Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef _ASM_METAG_SWITCH_H
#define _ASM_METAG_SWITCH_H

/*                    */
#define __METAG_SW_PERM_BREAK	0x400002	/*                        */
#define __METAG_SW_SYS_LEGACY	0x440000	/*                     */
#define __METAG_SW_SYS		0x440001	/*              */

/*                                   */
#define __METAG_SW_ENCODING(TYPE)	(0xaf000000 | (__METAG_SW_##TYPE))

#endif /*                     */
