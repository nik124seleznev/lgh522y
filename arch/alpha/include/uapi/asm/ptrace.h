#ifndef _UAPI_ASMAXP_PTRACE_H
#define _UAPI_ASMAXP_PTRACE_H


/*
                                                              
                                                          
  
                                                                 
                                                                 
  
                                                          
                                    
  
                                                                  
                                   
 */

struct pt_regs {
	unsigned long r0;
	unsigned long r1;
	unsigned long r2;
	unsigned long r3;
	unsigned long r4;
	unsigned long r5;
	unsigned long r6;
	unsigned long r7;
	unsigned long r8;
	unsigned long r19;
	unsigned long r20;
	unsigned long r21;
	unsigned long r22;
	unsigned long r23;
	unsigned long r24;
	unsigned long r25;
	unsigned long r26;
	unsigned long r27;
	unsigned long r28;
	unsigned long hae;
/*                                                         */
	unsigned long trap_a0;
	unsigned long trap_a1;
	unsigned long trap_a2;
/*                              */
	unsigned long ps;
	unsigned long pc;
	unsigned long gp;
	unsigned long r16;
	unsigned long r17;
	unsigned long r18;
};

/*
                                                                     
                                                           
 */
struct switch_stack {
	unsigned long r9;
	unsigned long r10;
	unsigned long r11;
	unsigned long r12;
	unsigned long r13;
	unsigned long r14;
	unsigned long r15;
	unsigned long r26;
	unsigned long fp[32];	/*                */
};


#endif /*                       */
