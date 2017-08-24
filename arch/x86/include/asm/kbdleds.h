#ifndef _ASM_X86_KBDLEDS_H
#define _ASM_X86_KBDLEDS_H

/*
                                                                            
                                                                           
                                      
 */

#include <asm/setup.h>

static inline int kbd_defleds(void)
{
	return boot_params.kbd_status & 0x20 ? (1 << VC_NUMLOCK) : 0;
}

#endif /*                    */
