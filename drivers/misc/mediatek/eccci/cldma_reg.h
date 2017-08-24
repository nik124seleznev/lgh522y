#ifndef __CLDMA_REG_H__
#define __CLDMA_REG_H__

#include <mach/sync_write.h>

/*             */
#define CLDMA_AP_UL_SBDMA_CODA_VERSION (0x0000) //                                  
#define CLDMA_AP_UL_START_ADDR_0       (0x0100) //                                
#define CLDMA_AP_UL_START_ADDR_1       (0x0104)
#define CLDMA_AP_UL_START_ADDR_2       (0x0108)
#define CLDMA_AP_UL_START_ADDR_3       (0x010C)
#define CLDMA_AP_UL_START_ADDR_4       (0x0110)
#define CLDMA_AP_UL_START_ADDR_5       (0x0114)
#define CLDMA_AP_UL_START_ADDR_6       (0x0118)
#define CLDMA_AP_UL_START_ADDR_7       (0x011C)
#define CLDMA_AP_UL_START_ADDR_8       (0x0120)
#define CLDMA_AP_UL_START_ADDR_9       (0x0124)
#define CLDMA_AP_UL_CURRENT_ADDR_0     (0x0128) //                                       
#define CLDMA_AP_UL_CURRENT_ADDR_1     (0x012C)
#define CLDMA_AP_UL_CURRENT_ADDR_2     (0x0130)
#define CLDMA_AP_UL_CURRENT_ADDR_3     (0x0134)
#define CLDMA_AP_UL_CURRENT_ADDR_4     (0x0138)
#define CLDMA_AP_UL_CURRENT_ADDR_5     (0x013C)
#define CLDMA_AP_UL_CURRENT_ADDR_6     (0x0140)
#define CLDMA_AP_UL_CURRENT_ADDR_7     (0x0144)
#define CLDMA_AP_UL_CURRENT_ADDR_8     (0x0148)
#define CLDMA_AP_UL_CURRENT_ADDR_9     (0x014C)
#define CLDMA_AP_UL_STATUS     (0x0150) //                          
#define CLDMA_AP_UL_START_CMD  (0x0154) //                       
#define CLDMA_AP_UL_RESUME_CMD (0x0158) //                        
#define CLDMA_AP_UL_STOP_CMD   (0x015C) //                      
#define CLDMA_AP_UL_ERROR      (0x0160) //      
#define CLDMA_AP_UL_CFG        (0x0174) //                        
#define CLDMA_AP_HPQTCR        (0x0210) //                                          
#define CLDMA_AP_LPQTCR        (0x0214) //                                         
#define CLDMA_AP_HPQR  		(0x0218) //                             
#define CLDMA_AP_TCR0  		(0x021C) //                                   
#define CLDMA_AP_TCR1  		(0x0220)
#define CLDMA_AP_TCR2  		(0x0224)
#define CLDMA_AP_TCR3  		(0x0228)
#define CLDMA_AP_TCR4  		(0x022C)
#define CLDMA_AP_TCR5  		(0x0230)
#define CLDMA_AP_TCR6  		(0x0234)
#define CLDMA_AP_TCR7  		(0x0238)
#define CLDMA_AP_TCR_CMD    (0x023C) //                                 
#define CLDMA_AP_UL_CHECKSUM_CHANNEL_ENABLE    (0x0240) //                                              

/*              */
#define CLDMA_AP_SO_OUTDMA_CODA_VERSION (0x1000) //                                        
#define CLDMA_AP_SO_ERROR      			(0x1100) //      
#define CLDMA_AP_SO_CFG        			(0x1104) //                        
#define CLDMA_AP_SO_CHECKSUM_CHANNEL_ENABLE 	(0x1174) //                                              
#define CLDMA_AP_SO_START_ADDR_0       (0x1178) //                                
#define CLDMA_AP_SO_START_ADDR_1       (0x117C)
#define CLDMA_AP_SO_START_ADDR_2       (0x1180)
#define CLDMA_AP_SO_START_ADDR_3       (0x1184)
#define CLDMA_AP_SO_START_ADDR_4       (0x1188)
#define CLDMA_AP_SO_START_ADDR_5       (0x118C)
#define CLDMA_AP_SO_START_ADDR_6       (0x1190)
#define CLDMA_AP_SO_START_ADDR_7       (0x1194)
#define CLDMA_AP_SO_CURRENT_ADDR_0     (0x1198) //                                       
#define CLDMA_AP_SO_CURRENT_ADDR_1     (0x119C)
#define CLDMA_AP_SO_CURRENT_ADDR_2     (0x11A0)
#define CLDMA_AP_SO_CURRENT_ADDR_3     (0x11A4)
#define CLDMA_AP_SO_CURRENT_ADDR_4     (0x11A8)
#define CLDMA_AP_SO_CURRENT_ADDR_5     (0x11AC)
#define CLDMA_AP_SO_CURRENT_ADDR_6     (0x11B0)
#define CLDMA_AP_SO_CURRENT_ADDR_7     (0x11B4)
#define CLDMA_AP_SO_STATUS				(0x11B8) //                               
#define CLDMA_AP_SO_START_CMD			(0x11BC) //                            
#define CLDMA_AP_SO_RESUME_CMD			(0x11C0) //                             
#define CLDMA_AP_SO_STOP_CMD  			(0x11C4) //                           
#define CLDMA_AP_DEBUG_ID_EN  			(0x11C8) //                

/*          */
#define CLDMA_AP_CLDMA_CODA_VERSION (0x2000) //                               
#define CLDMA_AP_L2TISAR0   (0x2010) //                                                         
#define CLDMA_AP_L2TISAR1   (0x2014)
#define CLDMA_AP_L2TIMR0    (0x2018) //                                          
#define CLDMA_AP_L2TIMR1    (0x201C)
#define CLDMA_AP_L2TIMCR0   (0x2020) //                                                
#define CLDMA_AP_L2TIMCR1   (0x2024)
#define CLDMA_AP_L2TIMSR0   (0x2028) //                                              
#define CLDMA_AP_L2TIMSR1   (0x202C)
#define CLDMA_AP_L3TISAR0   (0x2030) //                                                         
#define CLDMA_AP_L3TISAR1   (0x2034)
#define CLDMA_AP_L3TIMR0    (0x2038) //                                          
#define CLDMA_AP_L3TIMR1    (0x203C)
#define CLDMA_AP_L3TIMCR0   (0x2040) //                                                
#define CLDMA_AP_L3TIMCR1   (0x2044)
#define CLDMA_AP_L3TIMSR0   (0x2048) //                                              
#define CLDMA_AP_L3TIMSR1   (0x204C)
#define CLDMA_AP_L2RISAR0   (0x2050) //                                                         
#define CLDMA_AP_L2RISAR1   (0x2054)
#define CLDMA_AP_L2RIMR0    (0x2058) //                                          
#define CLDMA_AP_L2RIMR1    (0x205C)
#define CLDMA_AP_L2RIMCR0   (0x2060) //                                                
#define CLDMA_AP_L2RIMCR1   (0x2064)
#define CLDMA_AP_L2RIMSR0   (0x2068) //                                              
#define CLDMA_AP_L2RIMSR1   (0x206C)
#define CLDMA_AP_L3RISAR0   (0x2070) //                                                         
#define CLDMA_AP_L3RISAR1   (0x2074)
#define CLDMA_AP_L3RIMR0    (0x2078) //                                          
#define CLDMA_AP_L3RIMR1    (0x207C)
#define CLDMA_AP_L3RIMCR0   (0x2080) //                                                
#define CLDMA_AP_L3RIMCR1   (0x2084)
#define CLDMA_AP_L3RIMSR0   (0x2088) //                                              
#define CLDMA_AP_L3RIMSR1   (0x208C)
#define CLDMA_AP_BUS_CFG    (0x2090) //                                     
#define CLDMA_AP_CHNL_DISABLE		(0x2094) //                             
#define CLDMA_AP_HIGH_PRIORITY		(0x2098) //                           
#define CLDMA_AP_BUS_STA    (0x209C) //                              
#define CLDMA_AP_CHNL_IDLE  (0x20B0) //                 

/*                */
#define CLDMA_AP_TQSAR(i)  (CLDMA_AP_UL_START_ADDR_0   + (4 *(i)))
#define CLDMA_AP_TQCPR(i)  (CLDMA_AP_UL_CURRENT_ADDR_0 + (4 *(i)))
#define CLDMA_AP_RQSAR(i)  (CLDMA_AP_SO_START_ADDR_0   + (4 *(i)))
#define CLDMA_AP_RQCPR(i)  (CLDMA_AP_SO_CURRENT_ADDR_0 + (4 *(i)))
#define CLDMA_AP_TQTCR(i)  (CLDMA_AP_TCR0 + (4 *(i)))

#define cldma_write32(b, a, v)			mt_reg_sync_writel(v, (b)+(a))
#define cldma_write16(b, a, v)			mt_reg_sync_writew(v, (b)+(a))
#define cldma_write8(b, a, v)			mt_reg_sync_writeb(v, (b)+(a))

#define cldma_read32(b, a)				ioread32((void __iomem *)((b)+(a)))
#define cldma_read16(b, a)				ioread16((void __iomem *)((b)+(a)))
#define cldma_read8(b, a)				ioread8((void __iomem *)((b)+(a)))

/*      */
#define CLDMA_BM_INT_ALL			0xFFFFFFFF
//             
#define CLDMA_BM_INT_ACTIVE_START	0xFF000000 //                                          
#define CLDMA_BM_INT_ERROR			0x00FF0000 //                                                                             
#define CLDMA_BM_INT_QUEUE_EMPTY	0x0000FF00 //                                                              
#define CLDMA_BM_INT_DONE			0x000000FF //                                                                
#define CLDMA_BM_INT_ACTIVE_LD_TC	0x000000FF //                                                 
#define CLDMA_BM_INT_INACTIVE_ERR	0x000000FF //                                               
//             
#define CLDMA_BM_INT_BD_LEN_ERR		0xFF000000 //                                                             
#define CLDMA_BM_INT_GPD_LEN_ERR	0x00FF0000 //                                                              
#define CLDMA_BM_INT_BD_CSERR		0x0000FF00 //                                           
#define CLDMA_BM_INT_GPD_CSERR		0x000000FF //                                            
#define CLDMA_BM_INT_DATA_LEN_MIS	0x00FF0000 //                                       
#define CLDMA_BM_INT_BD_64KERR		0x0000FF00 //                                              
#define CLDMA_BM_INT_GPD_64KERR		0x000000FF //                                               
#define CLDMA_BM_INT_RBIDX_ERR		0x80000000 //                            
#define CLDMA_BM_INT_FIFO_LEN_MIS	0x0000FF00 //                            
#define CLDMA_BM_INT_ALLEN			0x000000FF //                                                                  

#define CLDMA_BM_ALL_QUEUE 0xFF //             

#endif //               