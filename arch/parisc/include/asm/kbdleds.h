#ifndef _ASM_PARISC_KBDLEDS_H
#define _ASM_PARISC_KBDLEDS_H

/*
                                                                  
                                                                 
                                         
 */

static inline int kbd_defleds(void)
{
#if defined(CONFIG_KEYBOARD_HIL) || defined(CONFIG_KEYBOARD_HIL_OLD)
	return 1 << VC_NUMLOCK;
#else
	return 0;
#endif
}

#endif /*                       */
