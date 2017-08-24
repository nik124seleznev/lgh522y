#ifndef _UAPI_ALPHA_TERMIOS_H
#define _UAPI_ALPHA_TERMIOS_H

#include <asm/ioctls.h>
#include <asm/termbits.h>

struct sgttyb {
	char	sg_ispeed;
	char	sg_ospeed;
	char	sg_erase;
	char	sg_kill;
	short	sg_flags;
};

struct tchars {
	char	t_intrc;
	char	t_quitc;
	char	t_startc;
	char	t_stopc;
	char	t_eofc;
	char	t_brkc;
};

struct ltchars {
	char	t_suspc;
	char	t_dsuspc;
	char	t_rprntc;
	char	t_flushc;
	char	t_werasc;
	char	t_lnextc;
};

struct winsize {
	unsigned short ws_row;
	unsigned short ws_col;
	unsigned short ws_xpixel;
	unsigned short ws_ypixel;
};

#define NCC 8
struct termio {
	unsigned short c_iflag;		/*                  */
	unsigned short c_oflag;		/*                   */
	unsigned short c_cflag;		/*                    */
	unsigned short c_lflag;		/*                  */
	unsigned char c_line;		/*                 */
	unsigned char c_cc[NCC];	/*                    */
};

/*
                                                                 
                                                            
                                                                
                                                                     
                                                                    
                              
 */
#define _VINTR	0
#define _VQUIT	1
#define _VERASE	2
#define _VKILL	3
#define _VEOF	4
#define _VMIN	4
#define _VEOL	5
#define _VTIME	5
#define _VEOL2	6
#define _VSWTC	7


#endif /*                       */
