#ifndef __LINUX_COMPILER_H
#error "Please don't include <linux/compiler-intel.h> directly, include <linux/compiler.h> instead."
#endif

#ifdef __ECC

/*                                                        
                         
 */

#include <asm/intrinsics.h>

/*                                                           
                                                  
 */
#undef barrier
#undef RELOC_HIDE

#define barrier() __memory_barrier()

#define RELOC_HIDE(ptr, off)					\
  ({ unsigned long __ptr;					\
     __ptr = (unsigned long) (ptr);				\
    (typeof(ptr)) (__ptr + (off)); })

/*                                                                   */
#define __must_be_array(a) 0

#endif

#ifndef __HAVE_BUILTIN_BSWAP16__
/*                                        */
#define __HAVE_BUILTIN_BSWAP16__
#define __builtin_bswap16 _bswap16
#endif

