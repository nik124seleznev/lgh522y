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
#undef OPTIMIZER_HIDE_VAR

#define barrier() __memory_barrier()

#define RELOC_HIDE(ptr, off)					\
  ({ unsigned long __ptr;					\
     __ptr = (unsigned long) (ptr);				\
    (typeof(ptr)) (__ptr + (off)); })

/* This should act as an optimization barrier on var.
 * Given that this compiler does not have inline assembly, a compiler barrier
 * is the best we can do.
 */
#define OPTIMIZER_HIDE_VAR(var) barrier()

/* Intel ECC compiler doesn't support __builtin_types_compatible_p() */
#define __must_be_array(a) 0

#endif

#ifndef __HAVE_BUILTIN_BSWAP16__
/*                                        */
#define __HAVE_BUILTIN_BSWAP16__
#define __builtin_bswap16 _bswap16
#endif

