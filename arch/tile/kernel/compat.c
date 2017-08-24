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

/*                                                          */
#define __SYSCALL_COMPAT

#include <linux/compat.h>
#include <linux/syscalls.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/fcntl.h>
#include <linux/uaccess.h>
#include <linux/signal.h>
#include <asm/syscalls.h>

/*
                                                                      
                                                        
                                                                     
                                                                 
                              
 */

COMPAT_SYSCALL_DEFINE4(truncate64, char __user *, filename, u32, dummy,
                       u32, low, u32, high)
{
	return sys_truncate(filename, ((loff_t)high << 32) | low);
}

COMPAT_SYSCALL_DEFINE4(ftruncate64, unsigned int, fd, u32, dummy,
                       u32, low, u32, high)
{
	return sys_ftruncate(fd, ((loff_t)high << 32) | low);
}

COMPAT_SYSCALL_DEFINE6(pread64, unsigned int, fd, char __user *, ubuf,
                       size_t, count, u32, dummy, u32, low, u32, high)
{
	return sys_pread64(fd, ubuf, count, ((loff_t)high << 32) | low);
}

COMPAT_SYSCALL_DEFINE6(pwrite64, unsigned int, fd, char __user *, ubuf,
                       size_t, count, u32, dummy, u32, low, u32, high)
{
	return sys_pwrite64(fd, ubuf, count, ((loff_t)high << 32) | low);
}

COMPAT_SYSCALL_DEFINE6(sync_file_range2, int, fd, unsigned int, flags,
                       u32, offset_lo, u32, offset_hi,
                       u32, nbytes_lo, u32, nbytes_hi)
{
	return sys_sync_file_range(fd, ((loff_t)offset_hi << 32) | offset_lo,
				   ((loff_t)nbytes_hi << 32) | nbytes_lo,
				   flags);
}

COMPAT_SYSCALL_DEFINE6(fallocate, int, fd, int, mode,
                       u32, offset_lo, u32, offset_hi,
                       u32, len_lo, u32, len_hi)
{
	return sys_fallocate(fd, mode, ((loff_t)offset_hi << 32) | offset_lo,
			     ((loff_t)len_hi << 32) | len_lo);
}

/*
                                                                   
                                                                 
                                              
 */
COMPAT_SYSCALL_DEFINE5(llseek, unsigned int, fd, unsigned int, offset_high,
		       unsigned int, offset_low, loff_t __user *, result,
		       unsigned int, origin)
{
	return sys_llseek(fd, offset_high, offset_low, result, origin);
}
 
/*                                                    */
#undef __SYSCALL
#define __SYSCALL(nr, call) [nr] = (call),

/*                       */
#define compat_sys_fadvise64_64 sys32_fadvise64_64
#define compat_sys_readahead sys32_readahead
#define sys_llseek compat_sys_llseek

/*                                                */
#define compat_sys_rt_sigreturn _compat_sys_rt_sigreturn
#define sys_clone _sys_clone

/*
                                                                    
                                                                     
 */
void *compat_sys_call_table[__NR_syscalls] = {
	[0 ... __NR_syscalls-1] = sys_ni_syscall,
#include <asm/unistd.h>
};
