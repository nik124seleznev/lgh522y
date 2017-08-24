#ifndef __ALPHA_COMPILER_H
#define __ALPHA_COMPILER_H

#include <uapi/asm/compiler.h>

/*                                                                   
                                                                       
                               */

#include <linux/compiler.h>
#undef inline
#undef __inline__
#undef __inline
#undef __always_inline
#define __always_inline		inline __attribute__((always_inline))

#endif /*                    */
