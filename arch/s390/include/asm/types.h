/*
                
  
                                           
 */
#ifndef _S390_TYPES_H
#define _S390_TYPES_H

#include <uapi/asm/types.h>

/*
                                                                       
 */

#ifndef __ASSEMBLY__

#ifndef CONFIG_64BIT
typedef union {
	unsigned long long pair;
	struct {
		unsigned long even;
		unsigned long odd;
	} subreg;
} register_pair;

#endif /*                  */
#endif /*               */
#endif /*               */
