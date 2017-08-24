#ifndef _MT_ISP_H
#define _MT_ISP_H

#include <linux/ioctl.h>

/*                                                                              
 
                                                                               */
#define ISP_DEV_MAJOR_NUMBER    251
#define ISP_MAGIC               'k'
/*                                                                              
 
                                                                               */
#define ISP_INT_EXP_DONE        ((u32)0x1)
#define ISP_INT_IDLE            ((u32)0x1 << 3)
#define ISP_INT_ISP_DONE        ((u32)0x1 << 4)
#define ISP_INT_VSYNC           ((u32)0x1 << 10)
#define ISP_INT_STNR            ((u32)0x1 << 29)
#define ISP_INT_CLEAR_ALL       ((u32)0x1 << 30)
#define ISP_INT_CLEAR_WAIT      ((u32)0x1 << 31)

/*                                                                              
 
                                                                               */
typedef struct mt_isp_reg_s {
    unsigned long addr;   //                
    unsigned long val;    //                 
} mt_isp_reg_t;

typedef struct mt_isp_reg_io_s {
    unsigned long data;   //                        
    unsigned long count;  //      
} mt_isp_reg_io_t;

typedef struct mt_isp_wait_irq_s {
    unsigned long mode;     //                  
    unsigned long timeout;  //                              
} mt_isp_wait_irq_t;
/*                                                                              
 
                                                                               */
//                                  
//                           
//                             
//                      
//                               
//                                   
//                                   
//                
//      
#define MT_ISP_IOC_T_RESET      _IO  (ISP_MAGIC, 1)
//                          
#define MT_ISP_IOC_G_READ_REG   _IOWR(ISP_MAGIC, 2, mt_isp_reg_io_t)
//                         
#define MT_ISP_IOC_S_WRITE_REG  _IOWR(ISP_MAGIC, 3, mt_isp_reg_io_t)
//                             
#define MT_ISP_IOC_T_HOLD_REG   _IOW (ISP_MAGIC, 4, u32)
//                                        
#define MT_ISP_IOC_T_RUN        _IOW (ISP_MAGIC, 5, u32)
//         
#define MT_ISP_IOC_T_WAIT_IRQ   _IOW (ISP_MAGIC, 6, u32) //                                                     
//                                     
#define MT_ISP_IOC_T_DUMP_REG   _IO  (ISP_MAGIC, 7)
//                   
#define MT_ISP_IOC_T_DBG_FLAG   _IOW (ISP_MAGIC, 8, u32)
//                
#define MT_ISP_IOC_T_RESET_BUF  _IO  (ISP_MAGIC, 9)
//                      
#define MT_ISP_IOC_T_ENABLE_CAM_CLOCK  _IO  (ISP_MAGIC, 10)
/*                                                                              
 
                                                                               */
void mt_isp_mclk_ctrl(int en);
//
#endif

