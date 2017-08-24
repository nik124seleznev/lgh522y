/*
                                                                                      
*/



/*
                   
  
                      
                                                         
                                                  
  
                      
                                                         
    
  
                      
                                                         
                                                        
  
                   
                                                                                     
                               
                                                      
  
                      
                                                             
                                                  
                                                                      
  
                   
                                                                                       
                                    
  
                   
  
                                                                               
  
                   
                                                    
                                                    
  
                   
                                                    
                                                                         
                                                       
  
                         
                                                
                                   
  
                   
                                                          
                                                                  
  
                   
                                                          
                                                                                                            
                                                                      
                                                                        
  
                         
                                                          
                                             
  
                   
                                                          
                                                                                          
                                                                            
              
                                                                            
                                          
                                                                            
                                                                                 
  
                                                                            
                                                                            
                                                   
                                                                            
                                               
                                                                            
                                                                           
                            
                                                                           
                               
                                                                           
                                  
                                                                           
                       
*/

#ifndef _HIF_TX_H
#define _HIF_TX_H

/*                                                                              
                                                     
                                                                                
*/

/*                                                                              
                                                          
                                                                                
*/

/*                                                                              
                                                
                                                                                
*/
/*                                                   */
#define HIF_TX_BUFF_MAX_SIZE                1552	/*                                 */

/*                                             */
#define HIF_TX_BUFF_COUNT_TC0               3
#define HIF_TX_BUFF_COUNT_TC1               3
#define HIF_TX_BUFF_COUNT_TC2               3
#define HIF_TX_BUFF_COUNT_TC3               3
#define HIF_TX_BUFF_COUNT_TC4               2

#define TX_HDR_SIZE                         sizeof(HIF_TX_HEADER_T)

#define CMD_HDR_SIZE                        sizeof(WIFI_CMD_T)

#define CMD_PKT_SIZE_FOR_IMAGE              2048	/*                                  */


/*                                                */
/*                */
#define HIF_TX_HDR_TX_BYTE_COUNT_MASK       BITS(0, 15)

/*              */
#define HIF_TX_HDR_ETHER_TYPE_OFFSET_MASK   BITS(0, 6)
#define HIF_TX_HDR_IP_CSUM                  BIT(7)

/*              */
#define HIF_TX_HDR_TCP_CSUM                 BIT(0)
#define HIF_TX_HDR_QUEUE_IDX_MASK           BITS(3, 6)
#define HIF_TX_HDR_QUEUE_IDX_OFFSET         3
#define HIF_TX_HDR_PORT_IDX_MASK            BIT(7)
#define HIF_TX_HDR_PORT_IDX_OFFSET          7


/*                                                                              
                                             
                                                                                
*/
typedef struct _HIF_HW_TX_HEADER_T {
	UINT_16 u2TxByteCount;
	UINT_8 ucEtherTypeOffset;
	UINT_8 ucCSflags;
	UINT_8 aucBuffer[0];
} HIF_HW_TX_HEADER_T, *P_HIF_HW_TX_HEADER_T;

typedef struct _HIF_TX_HEADER_T {
	UINT_16 u2TxByteCount_UserPriority;
	UINT_8 ucEtherTypeOffset;
	UINT_8 ucResource_PktType_CSflags;
	UINT_8 ucWlanHeaderLength;
	UINT_8 ucPktFormtId_Flags;
	UINT_16 u2LLH;		/*         */
	UINT_16 u2SeqNo;	/*         */
	UINT_8 ucStaRecIdx;
	UINT_8 ucForwardingType_SessionID_Reserved;
	UINT_8 ucPacketSeqNo;
	UINT_8 ucAck_BIP_BasicRate;
	UINT_8 aucReserved[2];
} HIF_TX_HEADER_T, *P_HIF_TX_HEADER_T;

typedef enum _ENUM_HIF_OOB_CTRL_PKT_TYPE_T {
	HIF_OOB_CTRL_PKT_TYPE_LOOPBACK = 1,
	HIF_OOB_CTRL_PKT_TYP_NUM
} ENUM_HIF_OOB_CTRL_PKT_TYPE_T, *P_ENUM_HIF_OOB_CTRL_PKT_TYPE_T;

/*                                                                              
                                                  
                                                                                
*/

/*                                                                              
                                                   
                                                                                
*/

/*                                                                              
                                             
                                                                                
*/
#define TFCB_FRAME_PAD_TO_DW(u2Length)      ALIGN_4(u2Length)

/*                                                                              
                                                            
                                                                                
*/
/*                                                                               
                                                     
 */
static __KAL_INLINE__ VOID hif_txDataTypeCheck(VOID);

static __KAL_INLINE__ VOID hif_txDataTypeCheck(VOID)
{
	DATA_STRUC_INSPECTING_ASSERT(sizeof(HIF_TX_HEADER_T) == 16);

	return;
}

#endif /*          */
