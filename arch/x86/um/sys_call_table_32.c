/*
                                                                          
                             
 */

#include <linux/linkage.h>
#include <linux/sys.h>
#include <linux/cache.h>
#include <generated/user_constants.h>

#define __NO_STUBS

/*
                                                                               
                             
 */

/*                                                                */
#define sys_iopl sys_ni_syscall
#define sys_ioperm sys_ni_syscall

#define sys_vm86old sys_ni_syscall
#define sys_vm86 sys_ni_syscall

#define old_mmap sys_old_mmap

#define __SYSCALL_I386(nr, sym, compat) extern asmlinkage void sym(void) ;
#include <asm/syscalls_32.h>

#undef __SYSCALL_I386
#define __SYSCALL_I386(nr, sym, compat) [ nr ] = sym,

typedef asmlinkage void (*sys_call_ptr_t)(void);

extern asmlinkage void sys_ni_syscall(void);

const sys_call_ptr_t sys_call_table[] __cacheline_aligned = {
	/*
                                                 
                                
  */
	[0 ... __NR_syscall_max] = &sys_ni_syscall,
#include <asm/syscalls_32.h>
};

int syscall_table_size = sizeof(sys_call_table);
