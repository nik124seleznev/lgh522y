/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#ifndef _UAPI_ASM_POWERPC_MMAN_H
#define _UAPI_ASM_POWERPC_MMAN_H

#include <asm-generic/mman-common.h>


#define PROT_SAO	0x10		/*                        */

#define MAP_RENAME      MAP_ANONYMOUS   /*                      */
#define MAP_NORESERVE   0x40            /*                          */
#define MAP_LOCKED	0x80

#define MAP_GROWSDOWN	0x0100		/*                    */
#define MAP_DENYWRITE	0x0800		/*         */
#define MAP_EXECUTABLE	0x1000		/*                          */

#define MCL_CURRENT     0x2000          /*                                 */
#define MCL_FUTURE      0x4000          /*                                     */

#define MAP_POPULATE	0x8000		/*                                */
#define MAP_NONBLOCK	0x10000		/*                    */
#define MAP_STACK	0x20000		/*                                                                   */
#define MAP_HUGETLB	0x40000		/*                            */

#endif /*                          */
