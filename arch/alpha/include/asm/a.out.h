#ifndef __ALPHA_A_OUT_H__
#define __ALPHA_A_OUT_H__

#include <uapi/asm/a.out.h>


/*                                                                   
                                                                     
                                                                    
                                                               */
#define SET_AOUT_PERSONALITY(BFPM, EX) \
	set_personality (((BFPM->taso || EX.ah.entry < 0x100000000L \
			   ? ADDR_LIMIT_32BIT : 0) | PER_OSF4))

#endif /*                 */
