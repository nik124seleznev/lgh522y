/*
                                                                                                     
*/

/*                 
                                                 
*/



/*
                
  
                      
                                                             
                                                  
                                                                      
  
                   
  
                                                                               
  
                         
                                                
                                   
                                                                           
                   
*/

#ifndef _HIF_H
#define _HIF_H

/*                                                                              
                                                      
                                                                                
*/
#if _PF_COLIBRI
#include "colibri.h"
#endif

#include "mt6620_reg.h"

/*                                                                              
                                            
                                                                                
*/

#define NIC_INTERRUPT_MODE      NdisInterruptLevelSensitive

/*                                                */
#define NIC_INTERFACE_TYPE      NdisInterfaceInternal
#define NIC_ATTRIBUTE           (NDIS_ATTRIBUTE_DESERIALIZE | \
				 NDIS_ATTRIBUTE_NO_HALT_ON_SUSPEND)
#define NIC_DMA_MAPPED          0
#define NIC_MINIPORT_INT_REG    1

/*                                                 
                                                                          
                                                                   */
#define NIC_RESOURCE_BUF_SZ     (sizeof(NDIS_RESOURCE_LIST) + \
				(10 * sizeof(CM_PARTIAL_RESOURCE_DESCRIPTOR)))

#define GPIO_PIN_RESERVED_BITS  0xfe000000u
#define GPIO_ALT_RESERVED_BITS  0xfffc0000u

/*                                            */

/*                                                          */
/*                            */

/*                  */
#define SPI_FUN_WR              0x80000000UL
#define SPI_FUN_RD              0x0
#define SPI_FUN_LEN_msk         0x00000FFFUL
#define SPI_FUN_LEN(x)          ((x & SPI_FUN_LEN_msk) << 16)

#define BUS_RETRY_COUNT         10000


/*                                                                      
                   
 */
#define WIFI_HIF_MAX_RX0_LEN_CNT    4
#define WIFI_HIF_MAX_RX1_LEN_CNT    4
#define WIFI_HIF_ENHANCED_MODE      0

#define WIFI_HIF_SPI_DATAOUT_MODE   1
#define WIFI_HIF_SPI_INTOUT_MODE    1
#define WIFI_HIF_SPI_ENDIAN         0
#if CONFIG_SPI_8_BIT_MODE
#define WIFI_HIF_SPI_MODE_SEL   0
#else
#define WIFI_HIF_SPI_MODE_SEL   2
#endif

/*                                                              
                                                                        
                               
 */
#define SPICSR_8BIT_MODE_DATA \
	( \
	    (WIFI_HIF_SPI_DATAOUT_MODE) | \
	    (WIFI_HIF_SPI_INTOUT_MODE << 1) | \
	    (WIFI_HIF_SPI_ENDIAN << 4) | \
	    (WIFI_HIF_SPI_MODE_SEL << 5) | \
	    (WIFI_HIF_MAX_RX0_LEN_CNT << 16) | \
	    (WIFI_HIF_MAX_RX1_LEN_CNT << 20) | \
	    (WIFI_HIF_ENHANCED_MODE << 24) \
	)

#define SPICSR_8BIT_MODE_ADDR \
	( \
	    (MCR_WCSR & 0xFFFF) | (4 << 16) | SPI_FUN_WR \
	)

/*                                                               
                                                                  
                               
 */
#define SPICSR_32BIT_MODE_DATA \
	SWAP32( \
	    (WIFI_HIF_SPI_DATAOUT_MODE) | \
	    (WIFI_HIF_SPI_INTOUT_MODE << 1) | \
	    (WIFI_HIF_SPI_ENDIAN << 4) | \
	    (WIFI_HIF_SPI_MODE_SEL << 5) | \
	    (WIFI_HIF_MAX_RX0_LEN_CNT << 16) | \
	    (WIFI_HIF_MAX_RX1_LEN_CNT << 20) | \
	    (WIFI_HIF_ENHANCED_MODE << 24) \
	)

#define SPICSR_32BIT_MODE_ADDR \
	SWAP32( \
	    (MCR_WCSR & 0xFFFF) | (4 << 16) | SPI_FUN_WR \
	)


/*                                                                              
                                             
                                                                                
*/


/*                                                      
                                  
 */
typedef struct _GL_HIF_INFO_T {

	NDIS_MINIPORT_INTERRUPT rInterrupt;	/*                               
                       */

	UINT_32 u4InterruptLevel;
	UINT_32 u4InterruptVector;

#if SC32442_SPI
	HANDLE gWaitEvent;
	UINT_32 u4sysIntr;
#endif

	UINT_32 u4ReqFlag;	/*              */

} GL_HIF_INFO_T, *P_GL_HIF_INFO_T;


/*                                                                              
                                                        
                                                                                
*/
#endif				/*        */
