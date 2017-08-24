/*
                                                                                                 
*/

/*                      
                                                  

                                          
*/



/*
                     
  
                           
                       
                     
  
*/

#ifndef _HIF_PDMA_H
#define _HIF_PDMA_H

#include "mtk_porting.h"


/*                                                                              
                                                     
                                                                                
*/


/*                                                                              
                                                          
                                                                                
*/


/*                                                                              
                                                
                                                                                
*/


/*                                                                              
                                                 
                                                                                
*/

typedef enum _MTK_WCN_HIF_PDMA_BURST_LEN {
	HIF_PDMA_BURST_1_4 = 0,
	HIF_PDMA_BURST_2_4,
	HIF_PDMA_BURST_3_4,
	HIF_PDMA_BURST_4_4
} MTK_WCN_HIF_PDMA_BURST_LEN;


/*                                       */
#ifdef CONFIG_OF
/*          */
#define AP_DMA_HIF_BASE             0x11000080
#else
/*                */
#define AP_DMA_HIF_BASE             0x11000180
#endif

#define AP_DMA_HIF_0_INT_FLAG       (0x0000)
#define AP_DMA_HIF_0_INT_EN         (0x0004)
#define AP_DMA_HIF_0_EN             (0x0008)
#define AP_DMA_HIF_0_RST            (0x000C)
#define AP_DMA_HIF_0_STOP           (0x0010)
#define AP_DMA_HIF_0_FLUSH          (0x0014)
#define AP_DMA_HIF_0_CON            (0x0018)
#define AP_DMA_HIF_0_SRC_ADDR       (0x001C)
#define AP_DMA_HIF_0_DST_ADDR       (0x0020)
#define AP_DMA_HIF_0_LEN            (0x0024)
#define AP_DMA_HIF_0_INT_BUF_SIZE   (0x0038)
#define AP_DMA_HIF_0_DEBUG_STATUS   (0x0050)

#define AP_DMA_HIF_0_LENGTH         0x0054

/*                       */
#define ADH_CR_FLAG_0               BIT(0)

/*                     */
#define ADH_CR_INTEN_FLAG_0         BIT(0)

/*                 */
#define ADH_CR_EN                   BIT(0)

/*                  */
#define ADH_CR_HARD_RST             BIT(1)
#define ADH_CR_WARM_RST             BIT(0)

/*                   */
#define ADH_CR_PAUSE                BIT(1)
#define ADH_CR_STOP                 BIT(0)

/*                    */
#define ADH_CR_FLUSH                BIT(0)

/*                  */
#define ADH_CR_BURST_LEN            BITS(16,17)
#define ADH_CR_BURST_LEN_OFFSET     16
#define ADH_CR_SLOW_CNT             BITS(5,14)
#define ADH_CR_SLOW_EN              BIT(2)
#define ADH_CR_FIX_EN               BIT(1)
#define ADH_CR_FIX_EN_OFFSET        1
#define ADH_CR_DIR                  BIT(0)

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
