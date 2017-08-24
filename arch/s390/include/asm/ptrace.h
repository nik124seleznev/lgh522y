/*
 *  S390 version
 *    Copyright IBM Corp. 1999, 2000
 *    Author(s): Denis Joseph Barrow (djbarrow@de.ibm.com,barrow_dj@yahoo.com)
 */
#ifndef _S390_PTRACE_H
#define _S390_PTRACE_H

#include <uapi/asm/ptrace.h>

#ifndef __ASSEMBLY__

extern long psw_kernel_bits;
extern long psw_user_bits;

/*
                                                                 
                                  
 */
struct pt_regs 
{
	unsigned long args[1];
	psw_t psw;
	unsigned long gprs[NUM_GPRS];
	unsigned long orig_gpr2;
	unsigned int int_code;
	unsigned long int_parm_long;
};

/*
                                              
 */
struct per_regs {
	unsigned long control;		/*                  */
	unsigned long start;		/*                      */
	unsigned long end;		/*                    */
};

/*
                                                                            
 */
struct per_event {
	unsigned short cause;		/*                        */
	unsigned long address;		/*             */
	unsigned char paid;		/*                           */
};

/*
                                                                          
 */
struct per_struct_kernel {
	unsigned long cr9;		/*                  */
	unsigned long cr10;		/*                      */
	unsigned long cr11;		/*                    */
	unsigned long bits;		/*                        */
	unsigned long starting_addr;	/*                              */
	unsigned long ending_addr;	/*                            */
	unsigned short perc_atmid;	/*                */
	unsigned long address;		/*                              */
	unsigned char access_id;	/*                                */
};

#define PER_EVENT_MASK			0xEB000000UL

#define PER_EVENT_BRANCH		0x80000000UL
#define PER_EVENT_IFETCH		0x40000000UL
#define PER_EVENT_STORE			0x20000000UL
#define PER_EVENT_STORE_REAL		0x08000000UL
#define PER_EVENT_TRANSACTION_END	0x02000000UL
#define PER_EVENT_NULLIFICATION		0x01000000UL

#define PER_CONTROL_MASK		0x00e00000UL

#define PER_CONTROL_BRANCH_ADDRESS	0x00800000UL
#define PER_CONTROL_SUSPENSION		0x00400000UL
#define PER_CONTROL_ALTERATION		0x00200000UL

/*
                                                      
 */
#define arch_has_single_step()	(1)

#define user_mode(regs) (((regs)->psw.mask & PSW_MASK_PSTATE) != 0)
#define instruction_pointer(regs) ((regs)->psw.addr & PSW_ADDR_INSN)
#define user_stack_pointer(regs)((regs)->gprs[15])
#define profile_pc(regs) instruction_pointer(regs)

static inline long regs_return_value(struct pt_regs *regs)
{
	return regs->gprs[2];
}

int regs_query_register_offset(const char *name);
const char *regs_query_register_name(unsigned int offset);
unsigned long regs_get_register(struct pt_regs *regs, unsigned int offset);
unsigned long regs_get_kernel_stack_nth(struct pt_regs *regs, unsigned int n);

static inline unsigned long kernel_stack_pointer(struct pt_regs *regs)
{
	return regs->gprs[15] & PSW_ADDR_INSN;
}

#endif /*              */
#endif /*                */
