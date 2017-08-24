/* asmmacro.h: Assembler macros.
 *
 * Copyright (C) 1996 David S. Miller (davem@caipfs.rutgers.edu)
 */

#ifndef _SPARC_ASMMACRO_H
#define _SPARC_ASMMACRO_H

/*                                                               
                                                              
                        
 */
#define SAVE_ALL_HEAD \
	sethi	%hi(trap_setup), %l4; \
	jmpl	%l4 + %lo(trap_setup), %l6;
#define SAVE_ALL \
	SAVE_ALL_HEAD \
	 nop;

/*                                                         */
#define RESTORE_ALL b ret_trap_entry; clr %l6;

/*                                                      
                                                        
                            
  
          
                                          
                                       
                          
  
                                          
                                                 
                          
 */
#define LEON_PI(...)				\
662:	__VA_ARGS__

#define SUN_PI_(...)				\
	.section .leon_1insn_patch, "ax";	\
	.word 662b;				\
	__VA_ARGS__;				\
	.previous

#endif /*                      */
