#ifndef __DDP_DEBUG_H__
#define __DDP_DEBUG_H__

#include <linux/kernel.h>
#include <linux/mmprofile.h>
#include <linux/aee.h>
#include "ddp_drv.h"

extern struct DDP_MMP_Events_t
{
    MMP_Event DDP;
    MMP_Event MutexParent;
    MMP_Event Mutex[6];
    MMP_Event BackupReg;
    MMP_Event DDP_IRQ;
    MMP_Event SCL_IRQ;
    MMP_Event ROT_IRQ;
    MMP_Event OVL_IRQ;
    MMP_Event WDMA0_IRQ;
    MMP_Event WDMA1_IRQ;
    MMP_Event RDMA0_IRQ;
    MMP_Event RDMA1_IRQ;
    MMP_Event COLOR_IRQ;
    MMP_Event BLS_IRQ;
    MMP_Event TDSHP_IRQ;
    MMP_Event CMDQ_IRQ;
    MMP_Event Mutex_IRQ;
    MMP_Event WAIT_INTR;
    MMP_Event Debug;
} DDP_MMP_Events;
//                                                                                
//                                                                                
//                                                                                

extern unsigned int dbg_log;
extern unsigned int irq_log;
extern unsigned int irq_err_log;

#define DISP_IRQ(string, args...) if(irq_log) printk("[DDP]"string,##args)  //            
#define DISP_DBG(string, args...) if(dbg_log) printk("[DDP]"string,##args)  //                                                                                  
#if 1  //                                                                                       
#define DISP_MSG(string, args...) printk("[DDP]"string,##args)  //                                   
#else
#define DISP_MSG(string, args...)
#endif
#define DISP_ERR(string, args...) printk("[DDP]error:"string,##args)  //                   
#define DDP_IRQ_ERR(string) if(irq_err_log) aee_kernel_warning_api(__FILE__, __LINE__, DB_OPT_DEFAULT, "DDP, "string, string)  //            

void ddp_debug_init(void);
void ddp_debug_exit(void);
int ddp_mem_test(void);
int ddp_mem_test2(void);
void ddp_enable_bls(int BLS_switch);
int ddp_dump_info(DISP_MODULE_ENUM module);

#endif /*                 */
