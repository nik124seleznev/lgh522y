/*
 *  Copied from arch/arm/include/asm/opcodes.h
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __ASM_ARM_OPCODES_H
#define __ASM_ARM_OPCODES_H

#ifndef __ASSEMBLY__
#include <linux/linkage.h>
extern asmlinkage unsigned int arm_check_condition(u32 opcode, u64 psr);
#endif

#define ARM_OPCODE_CONDTEST_FAIL   0
#define ARM_OPCODE_CONDTEST_PASS   1
#define ARM_OPCODE_CONDTEST_UNCOND 2


/*
                                     
                                                                           
                                                 
 */
#define ___asm_opcode_swab32(x) (	\
	  (((x) << 24) & 0xFF000000)	\
	| (((x) <<  8) & 0x00FF0000)	\
	| (((x) >>  8) & 0x0000FF00)	\
	| (((x) >> 24) & 0x000000FF)	\
)
#define ___asm_opcode_swab16(x) (	\
	  (((x) << 8) & 0xFF00)		\
	| (((x) >> 8) & 0x00FF)		\
)
#define ___asm_opcode_swahb32(x) (	\
	  (((x) << 8) & 0xFF00FF00)	\
	| (((x) >> 8) & 0x00FF00FF)	\
)
#define ___asm_opcode_swahw32(x) (	\
	  (((x) << 16) & 0xFFFF0000)	\
	| (((x) >> 16) & 0x0000FFFF)	\
)
#define ___asm_opcode_identity32(x) ((x) & 0xFFFFFFFF)
#define ___asm_opcode_identity16(x) ((x) & 0xFFFF)


/*
                          
  
                                                                             
                                                                         
  
                                                                               
                                                                               
  
  
                                        
  
                   
                                            
                                             
  
                                                                                
                                                                              
                                                                          
  
                                                                            
                                                            
                                                                      
  
                                                                              
                                                                            
                             
 */
#ifdef __ASSEMBLY__

#define ___opcode_swab32(x) ___asm_opcode_swab32(x)
#define ___opcode_swab16(x) ___asm_opcode_swab16(x)
#define ___opcode_swahb32(x) ___asm_opcode_swahb32(x)
#define ___opcode_swahw32(x) ___asm_opcode_swahw32(x)
#define ___opcode_identity32(x) ___asm_opcode_identity32(x)
#define ___opcode_identity16(x) ___asm_opcode_identity16(x)

#else /*                */

#include <linux/types.h>
#include <linux/swab.h>

#define ___opcode_swab32(x) swab32(x)
#define ___opcode_swab16(x) swab16(x)
#define ___opcode_swahb32(x) swahb32(x)
#define ___opcode_swahw32(x) swahw32(x)
#define ___opcode_identity32(x) ((u32)(x))
#define ___opcode_identity16(x) ((u16)(x))

#endif /*                */


#ifdef CONFIG_CPU_ENDIAN_BE8

#define __opcode_to_mem_arm(x) ___opcode_swab32(x)
#define __opcode_to_mem_thumb16(x) ___opcode_swab16(x)
#define __opcode_to_mem_thumb32(x) ___opcode_swahb32(x)
#define ___asm_opcode_to_mem_arm(x) ___asm_opcode_swab32(x)
#define ___asm_opcode_to_mem_thumb16(x) ___asm_opcode_swab16(x)
#define ___asm_opcode_to_mem_thumb32(x) ___asm_opcode_swahb32(x)

#else /*                         */

#define __opcode_to_mem_arm(x) ___opcode_identity32(x)
#define __opcode_to_mem_thumb16(x) ___opcode_identity16(x)
#define ___asm_opcode_to_mem_arm(x) ___asm_opcode_identity32(x)
#define ___asm_opcode_to_mem_thumb16(x) ___asm_opcode_identity16(x)
#ifndef CONFIG_CPU_ENDIAN_BE32
/*
                                                                              
                                                                       
                                    
 */
#define __opcode_to_mem_thumb32(x) ___opcode_swahw32(x)
#define ___asm_opcode_to_mem_thumb32(x) ___asm_opcode_swahw32(x)
#endif

#endif /*                         */

#define __mem_to_opcode_arm(x) __opcode_to_mem_arm(x)
#define __mem_to_opcode_thumb16(x) __opcode_to_mem_thumb16(x)
#ifndef CONFIG_CPU_ENDIAN_BE32
#define __mem_to_opcode_thumb32(x) __opcode_to_mem_thumb32(x)
#endif

/*                                      */

/*                          */
#define __opcode_is_thumb32(x) (		\
	   ((x) & 0xF8000000) == 0xE8000000	\
	|| ((x) & 0xF0000000) == 0xF0000000	\
)
#define __opcode_is_thumb16(x) (					\
	   ((x) & 0xFFFF0000) == 0					\
	&& !(((x) & 0xF800) == 0xE800 || ((x) & 0xF000) == 0xF000)	\
)

/*                                                             */
#define __opcode_thumb32_first(x) (___opcode_identity16((x) >> 16))
#define __opcode_thumb32_second(x) (___opcode_identity16(x))
#define __opcode_thumb32_compose(first, second) (			\
	  (___opcode_identity32(___opcode_identity16(first)) << 16)	\
	| ___opcode_identity32(___opcode_identity16(second))		\
)
#define ___asm_opcode_thumb32_first(x) (___asm_opcode_identity16((x) >> 16))
#define ___asm_opcode_thumb32_second(x) (___asm_opcode_identity16(x))
#define ___asm_opcode_thumb32_compose(first, second) (			    \
	  (___asm_opcode_identity32(___asm_opcode_identity16(first)) << 16) \
	| ___asm_opcode_identity32(___asm_opcode_identity16(second))	    \
)

/*
                           
  
                                                                
                                                                  
                                                                      
        
  
                                                                       
                                                                        
                                                                   
                             
  
                                                                       
               
  
  
                                               
                                                            
                                                            
  
                                                                   
                                                              
                        
  
                                                                   
                                                              
                        
  
  
                                                                        
                                                                        
                                                                         
                                                                   
                                                                         
                                                                        
                             
  
                                                                    
 */
#include <linux/stringify.h>

#define __inst_arm(x) ___inst_arm(___asm_opcode_to_mem_arm(x))
#define __inst_thumb32(x) ___inst_thumb32(				\
	___asm_opcode_to_mem_thumb16(___asm_opcode_thumb32_first(x)),	\
	___asm_opcode_to_mem_thumb16(___asm_opcode_thumb32_second(x))	\
)
#define __inst_thumb16(x) ___inst_thumb16(___asm_opcode_to_mem_thumb16(x))

#ifdef CONFIG_THUMB2_KERNEL
#define __inst_arm_thumb16(arm_opcode, thumb_opcode) \
	__inst_thumb16(thumb_opcode)
#define __inst_arm_thumb32(arm_opcode, thumb_opcode) \
	__inst_thumb32(thumb_opcode)
#else
#define __inst_arm_thumb16(arm_opcode, thumb_opcode) __inst_arm(arm_opcode)
#define __inst_arm_thumb32(arm_opcode, thumb_opcode) __inst_arm(arm_opcode)
#endif

/*                                                     */
#ifdef __ASSEMBLY__
#define ___inst_arm(x) .long x
#define ___inst_thumb16(x) .short x
#define ___inst_thumb32(first, second) .short first, second
#else
#define ___inst_arm(x) ".long " __stringify(x) "\n\t"
#define ___inst_thumb16(x) ".short " __stringify(x) "\n\t"
#define ___inst_thumb32(first, second) \
	".short " __stringify(first) ", " __stringify(second) "\n\t"
#endif

#endif /*                     */
