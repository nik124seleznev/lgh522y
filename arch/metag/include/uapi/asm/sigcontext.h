#ifndef _ASM_METAG_SIGCONTEXT_H
#define _ASM_METAG_SIGCONTEXT_H

#include <asm/ptrace.h>

/*
                                                                     
                                                                       
                                                                  
                                 
 */
struct sigcontext {
	struct user_gp_regs regs;	/*                   */

	/*
                                              
                                                                     
                                                              
                                                                      
  */
	struct user_cb_regs cb;

	/*
                                                             
  */
	struct user_rp_state rp;

	unsigned long oldmask;
};

#endif
