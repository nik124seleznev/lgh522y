/*
 * Copyright (C) 2004-2006 Atmel Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef __ASM_AVR32_POSIX_TYPES_H
#define __ASM_AVR32_POSIX_TYPES_H

/*
                                                                     
                                                                      
                            
 */

typedef unsigned short  __kernel_mode_t;
#define __kernel_mode_t __kernel_mode_t

typedef unsigned short  __kernel_ipc_pid_t;
#define __kernel_ipc_pid_t __kernel_ipc_pid_t

typedef unsigned long	__kernel_size_t;
typedef long		__kernel_ssize_t;
typedef int             __kernel_ptrdiff_t;
#define __kernel_size_t __kernel_size_t

typedef unsigned short  __kernel_old_uid_t;
typedef unsigned short  __kernel_old_gid_t;
#define __kernel_old_uid_t __kernel_old_uid_t

typedef unsigned short  __kernel_old_dev_t;
#define __kernel_old_dev_t __kernel_old_dev_t

#include <asm-generic/posix_types.h>

#endif /*                           */