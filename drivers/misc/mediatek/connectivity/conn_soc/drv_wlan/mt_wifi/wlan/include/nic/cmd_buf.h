/*
       
*/

/*                     
                                                                    

                                                                              
                        
*/



/*
                    
  
                   
                                                                                                                                 
                                         
  
                         
       
                                                                     
  
                   
   
                                                                                                 
                                                                                                   
                                                                                                                         
  
                   
   
                                                                               
  
                      
                                                             
                    
*/

#ifndef _CMD_BUF_H
#define _CMD_BUF_H

/*                                                                              
                                                     
                                                                                
*/

/*                                                                              
                                                          
                                                                                
*/

/*                                                                              
                                                
                                                                                
*/

/*                                                                              
                                                 
                                                                                
*/

typedef enum _COMMAND_TYPE {
    COMMAND_TYPE_GENERAL_IOCTL,
    COMMAND_TYPE_NETWORK_IOCTL,
    COMMAND_TYPE_SECURITY_FRAME,
    COMMAND_TYPE_MANAGEMENT_FRAME,
    COMMAND_TYPE_NUM
} COMMAND_TYPE, *P_COMMAND_TYPE;

typedef VOID (*PFN_CMD_DONE_HANDLER) (
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
    );

typedef VOID (*PFN_CMD_TIMEOUT_HANDLER) (
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo
    );

struct _CMD_INFO_T {
    QUE_ENTRY_T rQueEntry;

    COMMAND_TYPE    eCmdType;

    UINT_16     u2InfoBufLen;   /*                                  */
    PUINT_8     pucInfoBuffer;  /*                                       */
    P_NATIVE_PACKET prPacket;   /*                                       */

    ENUM_NETWORK_TYPE_INDEX_T eNetworkType;
    UINT_8      ucStaRecIndex;  /*                                       */

    PFN_CMD_DONE_HANDLER        pfCmdDoneHandler;
    PFN_CMD_TIMEOUT_HANDLER     pfCmdTimeoutHandler;

    BOOLEAN     fgIsOid; /*                                   */

    UINT_8      ucCID;
    BOOLEAN     fgSetQuery;
    BOOLEAN     fgNeedResp;
    BOOLEAN     fgDriverDomainMCR; /*                                                      */
    UINT_8      ucCmdSeqNum;
    UINT_32     u4SetInfoLen; /*                                            */

    /*                                     */
    PVOID       pvInformationBuffer;
    UINT_32     u4InformationBufferLength;

    /*              */
    UINT_32     u4PrivateData;
};


/*                                                                              
                                                  
                                                                                
*/

/*                                                                              
                                                   
                                                                                
*/

/*                                                                              
                                             
                                                                                
*/

/*                                                                              
                                                             
                                                                                
*/
VOID
cmdBufInitialize (
    IN P_ADAPTER_T prAdapter
    );

P_CMD_INFO_T
cmdBufAllocateCmdInfo (
    IN P_ADAPTER_T prAdapter,
    IN UINT_32 u4Length
    );

VOID
cmdBufFreeCmdInfo (
    IN P_ADAPTER_T prAdapter,
    IN P_CMD_INFO_T prCmdInfo
    );

/*                                                                            */
/*                                                                            */
/*                                                                            */
WLAN_STATUS
wlanSendSetQueryCmd (
    IN P_ADAPTER_T  prAdapter,
    UINT_8          ucCID,
    BOOLEAN         fgSetQuery,
    BOOLEAN         fgNeedResp,
    BOOLEAN         fgIsOid,
    PFN_CMD_DONE_HANDLER pfCmdDoneHandler,
    PFN_CMD_TIMEOUT_HANDLER pfCmdTimeoutHandler,
    UINT_32         u4SetQueryInfoLen,
    PUINT_8         pucInfoBuffer,
    OUT PVOID       pvSetQueryBuffer,
    IN UINT_32      u4SetQueryBufferLen
    );


/*                                                                              
                                                
                                                                                
*/
#endif /*            */


