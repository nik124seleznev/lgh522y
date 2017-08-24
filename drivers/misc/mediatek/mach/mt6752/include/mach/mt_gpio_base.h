#ifndef _MT_GPIO_BASE_H_
#define _MT_GPIO_BASE_H_

#include <mach/sync_write.h>
#include <mach/gpio_const.h>

#define GPIO_WR32(addr, data)   mt_reg_sync_writel(data, addr)
#define GPIO_RD32(addr)         __raw_readl(addr)
//                                                                                           
//                                                                                               
#define GPIO_SW_SET_BITS(BIT,REG)   GPIO_WR32(REG,GPIO_RD32(REG) | ((unsigned long)(BIT)))
#define GPIO_SET_BITS(BIT,REG)   GPIO_WR32(REG, (unsigned long)(BIT))
#define GPIO_CLR_BITS(BIT,REG)   GPIO_WR32(REG,GPIO_RD32(REG) & ~((unsigned long)(BIT)))

/*                                                                            */
typedef struct {         /*                      */   
    unsigned int val;        
    unsigned int set;
    unsigned int rst;
    unsigned int _align1;
} VAL_REGS;
/*                                                                            */
typedef struct {
    VAL_REGS    dir[6];             /*                          */
    u8          rsv00[160];         /*                          */    
    VAL_REGS    dout[6];            /*                          */
    u8          rsv01[160];         /*                          */
    VAL_REGS    din[6];             /*                          */
    u8          rsv02[160];         /*                          */
    VAL_REGS    mode[24];           /*                          */  
} GPIO_REGS;

#endif //                
