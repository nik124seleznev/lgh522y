#ifndef __ASM_METAG_GLOBAL_LOCK_H
#define __ASM_METAG_GLOBAL_LOCK_H

#include <asm/metag_mem.h>

/* 
                                                            
                                        
  
                                                                               
                                                                              
                                                                       
  
                                                                              
                                                                              
                                                                             
             
 */
#define __global_lock1(flags) do {					\
	unsigned int __trval;						\
	asm volatile("MOV	%0,#0\n\t"				\
		     "SWAP	%0,TXMASKI\n\t"				\
		     "LOCK1"						\
		     : "=r" (__trval)					\
		     :							\
		     : "memory");					\
	(flags) = __trval;						\
} while (0)

/* 
                                                              
                                          
  
                                                                
                                                                            
                                                                              
              
  
                                                                              
                   
 */
#define __global_unlock1(flags) do {					\
	unsigned int __trval = (flags);					\
	asm volatile("LOCK0\n\t"					\
		     "MOV	TXMASKI,%0"				\
		     :							\
		     : "r" (__trval)					\
		     : "memory");					\
} while (0)

/* 
                                                            
                                        
  
                                                                             
                                                                                
                                                                               
                                                           
  
                                                                            
                                   
 */
#define __global_lock2(flags) do {					\
	unsigned int __trval;						\
	unsigned int __aloc_hi = LINSYSEVENT_WR_ATOMIC_LOCK & 0xFFFF0000; \
	asm volatile("MOV	%0,#0\n\t"				\
		     "SWAP	%0,TXMASKI\n\t"				\
		     "LOCK2\n\t"					\
		     "SETD	[%1+#0x40],D1RtP"			\
		     : "=r&" (__trval)					\
		     : "u" (__aloc_hi)					\
		     : "memory");					\
	(flags) = __trval;						\
} while (0)

/* 
                                                              
                                          
  
                                                                            
                                                                             
                                                                                
                                                                 
  
                                                                               
                            
 */
#define __global_unlock2(flags) do {					\
	unsigned int __trval = (flags);					\
	unsigned int __alock_hi = LINSYSEVENT_WR_ATOMIC_LOCK & 0xFFFF0000; \
	asm volatile("SETD	[%1+#0x00],D1RtP\n\t"			\
		     "LOCK0\n\t"					\
		     "MOV	TXMASKI,%0"				\
		     :							\
		     : "r" (__trval),					\
		       "u" (__alock_hi)					\
		     : "memory");					\
} while (0)

#endif /*                           */
