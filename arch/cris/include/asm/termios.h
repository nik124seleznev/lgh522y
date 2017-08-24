#ifndef _CRIS_TERMIOS_H
#define _CRIS_TERMIOS_H

#include <uapi/asm/termios.h>


/*                                    
                                  
                                  
                                         
        
*/
#define INIT_C_CC "\003\034\177\025\004\0\1\0\021\023\032\0\022\017\027\026\0"

/*
                                                        
 */
#define SET_LOW_TERMIOS_BITS(termios, termio, x) { \
	unsigned short __tmp; \
	get_user(__tmp,&(termio)->x); \
	*(unsigned short *) &(termios)->x = __tmp; \
}

#define user_termio_to_kernel_termios(termios, termio) \
({ \
	SET_LOW_TERMIOS_BITS(termios, termio, c_iflag); \
	SET_LOW_TERMIOS_BITS(termios, termio, c_oflag); \
	SET_LOW_TERMIOS_BITS(termios, termio, c_cflag); \
	SET_LOW_TERMIOS_BITS(termios, termio, c_lflag); \
	copy_from_user((termios)->c_cc, (termio)->c_cc, NCC); \
})

/*
                                                        
 */
#define kernel_termios_to_user_termio(termio, termios) \
({ \
	put_user((termios)->c_iflag, &(termio)->c_iflag); \
	put_user((termios)->c_oflag, &(termio)->c_oflag); \
	put_user((termios)->c_cflag, &(termio)->c_cflag); \
	put_user((termios)->c_lflag, &(termio)->c_lflag); \
	put_user((termios)->c_line,  &(termio)->c_line); \
	copy_to_user((termio)->c_cc, (termios)->c_cc, NCC); \
})

#define user_termios_to_kernel_termios(k, u) copy_from_user(k, u, sizeof(struct termios2))
#define kernel_termios_to_user_termios(u, k) copy_to_user(u, k, sizeof(struct termios2))
#define user_termios_to_kernel_termios_1(k, u) copy_from_user(k, u, sizeof(struct termios))
#define kernel_termios_to_user_termios_1(u, k) copy_to_user(u, k, sizeof(struct termios))

#endif	/*                 */