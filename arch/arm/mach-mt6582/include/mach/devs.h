#ifndef __DEVS_H__
#define __DEVS_H__

#include <board-custom.h>
#include <mach/board.h>

#define CFG_DEV_UART1
#define CFG_DEV_UART2
#ifdef CONFIG_MACH_LGE
/*                             */
#else
#define CFG_DEV_UART3
#define CFG_DEV_UART4
#endif
/*
                    
 */

#define MTK_UART_SIZE 0x100

/*
                             
 */

extern int mt_board_init(void);

//                                               
//                                       

#endif  /*                    */

