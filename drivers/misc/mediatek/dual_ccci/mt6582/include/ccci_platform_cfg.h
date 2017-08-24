#ifndef __CCCI_PLATFORM_CFG_H__
#define __CCCI_PLATFORM_CFG_H__
#include <linux/version.h>
#include <mach/irqs.h>
#include <mach/mt_irq.h>
#include <mach/mt_reg_base.h>
#include <mach/mt_typedefs.h>
#include <mach/mt_boot.h>
#include <mach/sync_write.h>
//                                                            
#define MD1_DEV_MAJOR        (184)
#define MD2_DEV_MAJOR        (169)

//                                          
//                                          
#define CCCI_PLATFORM             "MT6582E1"
#define CCCI1_DRIVER_VER         0x20121001
#define CCCI2_DRIVER_VER         0x20121001

#define CURR_SEC_CCCI_SYNC_VER            (1)    //                                                                    

#define CCMNI_V1            (1)
#define CCMNI_V2            (2)

#define MD_HEADER_VER_NO    (2)
#define GFH_HEADER_VER_NO   (1)


//                                                        
//                    
#define MD_EX_LOG_SIZE                    (2*1024)
#define CCCI_MISC_INFO_SMEM_SIZE        (1*1024)
#define CCCI_SHARED_MEM_SIZE             UL(0x200000) //                          

#define MD_IMG_DUMP_SIZE                (1<<8)
#define DSP_IMG_DUMP_SIZE                (1<<9)

#define CCMNI_V1_PORT_NUM               (3)          //            
#define CCMNI_V2_PORT_NUM               (3)          //             


//                      
#define CCCI1_PCM_SMEM_SIZE                (16 * 2 * 1024)                //    
#define CCCI1_MD_LOG_SIZE                (137*1024*4+64*1024+112*1024)    //       

#define RPC1_MAX_BUF_SIZE                2048 //        
#define RPC1_REQ_BUF_NUM                2              //                                  

#define CCCI1_TTY_BUF_SIZE                (16 * 1024)
#define CCCI1_CCMNI_BUF_SIZE            (16*1024)
#define CCCI1_TTY_PORT_NUM                (3)
//                                                                       


//                      
#define CCCI2_PCM_SMEM_SIZE                (16 * 2 * 1024)                    //     
#define CCCI2_MD_LOG_SIZE                (137*1024*4+64*1024+112*1024)    //       

#define RPC2_MAX_BUF_SIZE                2048 //        
#define RPC2_REQ_BUF_NUM                2              //                                  

#define CCCI2_TTY_BUF_SIZE                (16 * 1024)
#define CCCI2_CCMNI_BUF_SIZE            (16*1024)
#define CCCI2_TTY_PORT_NUM              (3)
//                                                                       




//                                                               
/*                                    */
//                                                                                                           
//                                                                                                                              

/*                                */
#define CCCI_STATIC_SHARED_MEM           //                                                              
//                                                                                                                            

/*                                   */
//                              
//                           
#define  ENABLE_MEM_SIZE_CHECK


/*                                   */
#define  ENABLE_EMI_PROTECTION              //                               

/*                                   */
#define  ENABLE_MEM_REMAP_HW

/*                               */
//                                                                                        

//                                     
//                                   
#define ENABLE_32K_CLK_LESS                    //                    
#define  ENABLE_MD_WDT_PROCESS                //                                                      
//                                                                                               
#define ENABLE_AEE_MD_EE                        //                    
#define  ENABLE_DRAM_API                        //                          
#define ENABLE_SW_MEM_REMAP

#define ENABLE_DUMP_MD_REGISTER
/*                                                              */
#define CCIF_BASE                (AP_CCIF_BASE)
#define CCIF_CON(addr)            ((addr) + 0x0000)
#define CCIF_BUSY(addr)            ((addr) + 0x0004)
#define CCIF_START(addr)        ((addr) + 0x0008)
#define CCIF_TCHNUM(addr)        ((addr) + 0x000C)
#define CCIF_RCHNUM(addr)        ((addr) + 0x0010)
#define CCIF_ACK(addr)            ((addr) + 0x0014)

/*                                                                                      */
#define CCIF_TXCHDATA(addr)     ((addr) + 0x0100)
#define CCIF_RXCHDATA(addr)     ((addr) + 0x0100 + 128)

/*            */
#define MD_CCIF_CON(base)        ((base) + 0x0000)
#define MD_CCIF_BUSY(base)        ((base) + 0x0004)
#define MD_CCIF_START(base)        ((base) + 0x0008)
#define MD_CCIF_TCHNUM(base)    ((base) + 0x000C)
#define MD_CCIF_RCHNUM(base)    ((base) + 0x0010)
#define MD_CCIF_ACK(base)        ((base) + 0x0014)

/*                 */
#define CCIF_CON_SEQ 0x00 /*            */
#define CCIF_CON_ARB 0x01 /*             */
//                                       

//                                  
#define CCIF_STD_V1_MAX_CH_NUM                (8)
#define CCIF_STD_V1_RUN_TIME_DATA_OFFSET    (0x140)        //            
#define CCIF_STD_V1_RUM_TIME_MEM_MAX_LEN    (256-64)    //            


/*                                                            */
//                            
#define MD_DEBUG_MODE            (DEBUGTOP_BASE+0x1A010)
#define MD_DBG_JTAG_BIT            1<<0


/*                                                           */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36))
#define CCIF_MASK(irq) \
        do {    \
            mt65xx_irq_mask(irq);  \
        } while (0)
#define CCIF_UNMASK(irq) \
        do {    \
            mt65xx_irq_unmask(irq);  \
        } while (0)

#else
#define CCIF_MASK(irq) \
        do {    \
            disable_irq(irq); \
        } while (0)
#define CCIF_UNMASK(irq) \
        do {    \
            enable_irq(irq); \
        } while (0)
#endif

#define CCIF_CLEAR_PHY(pc)   do {    \
            *CCIF_ACK(pc) = 0xFFFFFFFF; \
        } while (0)


#define ccci_write32(a, v)            mt65xx_reg_sync_writel(v, a)
#define ccci_write16(a, v)            mt65xx_reg_sync_writew(v, a)
#define ccci_write8(a, v)            mt65xx_reg_sync_writeb(v, a)


#define ccci_read32(a)                (*((volatile unsigned int*)a))
#define ccci_read16(a)                (*((volatile unsigned short*)a))
#define ccci_read8(a)                (*((volatile unsigned char*)a))

#endif //                        

