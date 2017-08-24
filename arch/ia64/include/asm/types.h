/*
                                                                        
                                                                       
                                                                       
                                                                       
                        
  
                                
  
                           
                                                               
 */
#ifndef _ASM_IA64_TYPES_H
#define _ASM_IA64_TYPES_H

#include <asm-generic/int-ll64.h>
#include <uapi/asm/types.h>

#ifdef __ASSEMBLY__
#else
/*
                                                                       
 */

struct fnptr {
	unsigned long ip;
	unsigned long gp;
};

#endif /*               */
#endif /*                   */
