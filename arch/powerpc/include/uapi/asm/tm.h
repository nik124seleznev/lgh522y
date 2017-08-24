#ifndef _ASM_POWERPC_TM_H
#define _ASM_POWERPC_TM_H

/*                                                                  
                                                                       
                                                                       
 */
#define TM_CAUSE_PERSISTENT	0x01
#define TM_CAUSE_RESCHED	0xde
#define TM_CAUSE_TLBI		0xdc
#define TM_CAUSE_FAC_UNAV	0xda
#define TM_CAUSE_SYSCALL	0xd8  /*            */
#define TM_CAUSE_MISC		0xd6  /*            */
#define TM_CAUSE_SIGNAL		0xd4
#define TM_CAUSE_ALIGNMENT	0xd2
#define TM_CAUSE_EMULATE	0xd0

#endif
