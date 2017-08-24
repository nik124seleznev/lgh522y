/*
                                                                                                 
*/

/*                      
                                                  

                                          
*/



/*
                     
  
                           
                       
                     
  
*/

#ifndef _HIF_GDMA_H
#define _HIF_GDMA_H

#include "mtk_porting.h"


/*                                                                              
                                                     
                                                                                
*/


/*                                                                              
                                                          
                                                                                
*/


/*                                                                              
                                                
                                                                                
*/


/*                                                                              
                                                 
                                                                                
*/

typedef enum _MTK_WCN_HIF_GDMA_BURST_LEN {
	HIF_GDMA_BURST_1_8 = 0,
	HIF_GDMA_BURST_2_8,
	HIF_GDMA_BURST_3_8,
	HIF_GDMA_BURST_4_8,
	HIF_GDMA_BURST_5_8,
	HIF_GDMA_BURST_6_8,
	HIF_GDMA_BURST_7_8,
	HIF_GDMA_BURST_8_8 /*                            */
} MTK_WCN_HIF_GDMA_BURST_LEN;

typedef enum _MTK_WCN_HIF_GDMA_WRITE_LEN {
	HIF_GDMA_WRITE_0 = 0, /*                            */
	HIF_GDMA_WRITE_1, /*                            */
	HIF_GDMA_WRITE_2, /*                            */
	HIF_GDMA_WRITE_3 /*                            */
} MTK_WCN_HIF_GDMA_WRITE_LEN;

typedef enum _MTK_WCN_HIF_GDMA_RATIO {
	HIF_GDMA_RATIO_0 = 0, /*     */
	HIF_GDMA_RATIO_1 /*     */
} MTK_WCN_HIF_GDMA_RATIO;

typedef enum _MTK_WCN_HIF_GDMA_CONNECT {
	HIF_GDMA_CONNECT_NO = 0, /*            */
	HIF_GDMA_CONNECT_SET1, /*                        */
    HIF_GDMA_CONNECT_SET2, /*                        */
    HIF_GDMA_CONNECT_SET3 /*                        */
} MTK_WCN_HIF_GDMA_CONNECT;


/*                                       */
#define AP_DMA_HIF_BASE             0x11000100

#define AP_P_DMA_G_DMA_2_INT_FLAG   (0x0000)
#define AP_P_DMA_G_DMA_2_CON        (0x0018)
#define AP_P_DMA_G_DMA_2_CONNECT    (0x0034)
#define AP_P_DMA_G_DMA_2_LEN1       (0x0024)
#define AP_P_DMA_G_DMA_2_SRC_ADDR   (0x001C)
#define AP_P_DMA_G_DMA_2_DST_ADDR   (0x0020)
#define AP_P_DMA_G_DMA_2_INT_EN     (0x0004)
#define AP_P_DMA_G_DMA_2_EN         (0x0008)
#define AP_P_DMA_G_DMA_2_RST        (0x000C)
#define AP_P_DMA_G_DMA_2_STOP       (0x0010)

#define AP_DMA_HIF_0_LENGTH         0x0038

/*                       */
#define ADH_CR_FLAG_0               BIT(0)

/*                     */
#define ADH_CR_INTEN_FLAG_0         BIT(0)

/*                 */
#define ADH_CR_EN                   BIT(0)
#define ADH_CR_CONN_BUR_EN          BIT(1)

/*                   */
#define ADH_CR_PAUSE                BIT(1)
#define ADH_CR_STOP                 BIT(0)

/*                      */
#define ADH_CR_FLAG_FINISH          BIT(30)
#define ADH_CR_RSIZE                BITS(28,29)
#define ADH_CR_RSIZE_OFFSET         28
#define ADH_CR_WSIZE                BITS(24,25)
#define ADH_CR_WSIZE_OFFSET         24
#define ADH_CR_BURST_LEN            BITS(16,18)
#define ADH_CR_BURST_LEN_OFFSET     16
#define ADH_CR_WADDR_FIX_EN         BIT(3)
#define ADH_CR_WADDR_FIX_EN_OFFSET  3
#define ADH_CR_RADDR_FIX_EN         BIT(4)
#define ADH_CR_RADDR_FIX_EN_OFFSET  4

/*                          */
#define ADH_CR_RATIO                BIT(3)
#define ADH_CR_RATIO_OFFSET         3
#define ADH_CR_DIR                  BIT(2)
#define ADH_CR_DIR_OFFSET           2
#define ADH_CR_CONNECT              BITS(0,1)

/*                  */
#define ADH_CR_LEN                  BITS(0,19)




/*                                                                              
                                                  
                                                                                
*/

/*                                                                              
                                                   
                                                                                
*/

/*                                                                              
                                             
                                                                                
*/

/*                                                                              
                                                             
                                                                                
*/

/*                                                                              
                                                
                                                                                
*/
#endif /*             */

/*                   */
