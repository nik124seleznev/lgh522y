#ifndef __MT_PTP2_H__
#define __MT_PTP2_H__



#include <linux/kernel.h>
#include <mach/sync_write.h>
#include "mach/mt_reg_base.h"
#include "mach/mt_typedefs.h"



#ifdef __MT_PTP2_C__
	#define PTP2_EXTERN
#else
	#define PTP2_EXTERN extern
#endif



extern void __iomem *ptp2_base;  //          

//                                             
#define PTP2_BASEADDR            (ptp2_base + 0x600)
#define PTP2_CTRL_REG_BASEADDR   (PTP2_BASEADDR + 0x70)
#define PTP2_CTRL_REG_0          (PTP2_CTRL_REG_BASEADDR + 0x8)     //     
#define PTP2_CTRL_REG_1          (PTP2_CTRL_REG_BASEADDR + 0xc)     //     
#define PTP2_CTRL_REG_2          (PTP2_CTRL_REG_BASEADDR + 0x10)    //     
#define PTP2_CTRL_REG_3          (PTP2_CTRL_REG_BASEADDR + 0x14)    //     
#define PTP2_CTRL_REG_4          (PTP2_CTRL_REG_BASEADDR + 0x18)    //     

#define PTP2_REG_NUM             2



/* 
                         
*/
//          
#define PTP2_DET_SWRST                     31:31
#define PTP2_DET_RAMPSTART                 13:12
#define PTP2_DET_RAMPSTEP                  11:8
#define PTP2_DET_DELAY                      7:4
#define PTP2_DET_AUTO_STOP_BYPASS_ENABLE    3:3
#define PTP2_DET_TRIGGER_PUL_DELAY          2:2
#define PTP2_CTRL_ENABLE                    1:1
#define PTP2_DET_ENABLE                     0:0



/* 
                         
*/
//          
#define PTP2_MP0_nCORERESET        31:28
#define PTP2_MP0_STANDBYWFE        27:24
#define PTP2_MP0_STANDBYWFI        23:20
#define PTP2_MP0_STANDBYWFIL2      19:19
#define PTP2_MP1_nCORERESET        18:15
#define PTP2_MP1_STANDBYWFE        14:11
#define PTP2_MP1_STANDBYWFI        10:7
#define PTP2_MP1_STANDBYWFIL2       6:6

/*
                
 */
#undef  BIT
#define BIT(_bit_)                    (unsigned)(1 << (_bit_))
#define BITS(_bits_, _val_)           ((((unsigned) -1 >> (31 - ((1) ? _bits_))) & ~((1U << ((0) ? _bits_)) - 1)) & ((_val_)<<((0) ? _bits_)))
#define BITMASK(_bits_)               (((unsigned) -1 >> (31 - ((1) ? _bits_))) & ~((1U << ((0) ? _bits_)) - 1))
#define GET_BITS_VAL(_bits_, _val_)   (((_val_) & (_BITMASK_(_bits_))) >> ((0) ? _bits_))



//                                                                   
//                                                                                         
/* 
                                    
                                       
                                                          
                                                    
 */
#define ptp2_read(addr)	DRV_Reg32(addr)
#define ptp2_write(addr, val)    mt_reg_sync_writel(val, addr)
#define ptp2_write_field(addr, range, val)    ptp2_write(addr, (ptp2_read(addr) & ~(BITMASK(range))) | BITS(range, val))



/*
                      
 */
enum {
    PTP2_RAMPSTART_0 = 0b00,
    PTP2_RAMPSTART_1 = 0b01,
    PTP2_RAMPSTART_2 = 0b10,
    PTP2_RAMPSTART_3 = 0b11
};



/* 
                  
 */
enum {
    PTP2_CORE_RESET  = 0,
    PTP2_DEBUG_RESET = 1,
    PTP2_STANDBYWFI  = 2,
    PTP2_STANDBYWFE  = 3,
    PTP2_STANDBYWFI2 = 4,
    PTP2_TRIG_NUM    = 5
};



/* 
                       
*/
struct PTP2_data {
    unsigned int SWRST; //     
    unsigned int RAMPSTART; //     
    unsigned int RAMPSTEP; //    
    unsigned int DELAY; //   
    unsigned int AUTO_STOP_BYPASS_ENABLE; //   
    unsigned int TRIGGER_PUL_DELAY; //   
    unsigned int CTRL_ENABLE; //   
    unsigned int DET_ENABLE; //   
};

struct PTP2_trig {
    unsigned int mp0_nCORE_RESET; //     
    unsigned int mp0_STANDBY_WFE; //     
    unsigned int mp0_STANDBY_WFI; //     
    unsigned int mp0_STANDBY_WFIL2; //     
    unsigned int mp1_nCORE_RESET; //     
    unsigned int mp1_STANDBY_WFE; //     
    unsigned int mp1_STANDBY_WFI; //    
    unsigned int mp1_STANDBY_WFIL2; //   
};



PTP2_EXTERN void turn_on_LO(void);
PTP2_EXTERN void turn_off_LO(void);



#undef PTP2_EXTERN
#endif  //              
