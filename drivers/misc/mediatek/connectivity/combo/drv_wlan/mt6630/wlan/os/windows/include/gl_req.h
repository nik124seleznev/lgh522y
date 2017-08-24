/*
                                                                                             
*/

/*                  
                                                                              
                                                                    
                 

                                                                             
                                                                          
*/



/*
                   
  
                      
                                                             
                                                  
                                                                      
  
                   
  
                                                                               
  
                         
                                                
                                   
  
                   
                                                          
                                                                          
                                          
                                                         
  
                   
                                                          
                                                                              
                                                                                        
                                                                                              
                                                            
                                                                                                       
                                                                                      
  
                   
                                                          
    
                                                                           
                                         
                                                                           
                                                           
                                                                           
                    
  
*/

#ifndef _GL_REQ_H
#define _GL_REQ_H

/*                                                                              
                                                     
                                                                                
*/

/*                                                                              
                                                          
                                                                                
*/
#include "gl_typedef.h"

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

/*                                                                              
                                                            
                                                                                
*/
/*                                                                            */
/*                                                                            */
/*                                                                            */
WLAN_STATUS
reqQueryVendorDriverVersion(IN P_GLUE_INFO_T prGlueInfo,
			    OUT PVOID pvQryBuf, IN UINT_32 u4QryBufLen, OUT PUINT_32 pu4QryInfoLen);

WLAN_STATUS
reqQueryHardwareStatus(IN P_GLUE_INFO_T prGlueInfo,
		       OUT PVOID pvQryBuf, IN UINT_32 u4QryBufLen, OUT PUINT_32 pu4QryInfoLen);

WLAN_STATUS
reqQueryMedia(IN P_GLUE_INFO_T prGlueInfo,
	      OUT PVOID pvQryBuf, IN UINT_32 u4QryBufLen, OUT PUINT_32 pu4QryInfoLen);

WLAN_STATUS
reqQueryVendorDescription(IN P_GLUE_INFO_T prGlueInfo,
			  OUT PVOID pvQryBuf, IN UINT_32 u4QryBufLen, OUT PUINT_32 pu4QryInfoLen);

WLAN_STATUS
reqQueryDriverVersion(IN P_GLUE_INFO_T prGlueInfo,
		      OUT PVOID pvQryBuf, IN UINT_32 u4QryBufLen, OUT PUINT_32 pu4QryInfoLen);

WLAN_STATUS
reqQueryPhysicalMedium(IN P_GLUE_INFO_T prGlueInfo,
		       OUT PVOID pvQryBuf, IN UINT_32 u4QryBufLen, OUT PUINT_32 pu4QryInfoLen);

WLAN_STATUS
reqQueryMacOptions(IN P_GLUE_INFO_T prGlueInfo,
		   OUT PVOID pvQryBuf, IN UINT_32 u4QryBufLen, OUT PUINT_32 pu4QryInfoLen);

WLAN_STATUS
reqQueryMediaConnectStatus(IN P_GLUE_INFO_T prGlueInfo,
			   OUT PVOID pvQryBuf, IN UINT_32 u4QryBufLen, OUT PUINT_32 pu4QryInfoLen);

WLAN_STATUS
reqQueryMaxFrameSize(IN P_GLUE_INFO_T prGlueInfo,
		     IN PVOID pvQueryBuf, IN UINT_32 u4QueryBufLen, OUT PUINT_32 pu4QueryInfoLen);

WLAN_STATUS
reqQueryTxBufferSpace(IN P_GLUE_INFO_T prGlueInfo,
		      OUT PVOID pvQueryBuf, IN UINT_32 u4QueryBufLen, OUT PUINT_32 pu4QueryInfoLen);

WLAN_STATUS
reqQueryRxBufferSpace(IN P_GLUE_INFO_T prGlueInfo,
		      OUT PVOID pvQueryBuf, IN UINT_32 u4QueryBufLen, OUT PUINT_32 pu4QueryInfoLen);

WLAN_STATUS
reqQueryMaxTotalSize(IN P_GLUE_INFO_T prGlueInfo,
		     OUT PVOID pvQueryBuf, IN UINT_32 u4QueryBufLen, OUT PUINT_32 pu4QueryInfoLen);

WLAN_STATUS
reqQueryRcvErrorAlignment(IN P_GLUE_INFO_T prGlueInfo,
			  IN PVOID pvQueryBuf,
			  IN UINT_32 u4QueryBufLen, OUT PUINT_32 pu4QueryInfoLen);

WLAN_STATUS
reqSetCurrentLookahead(IN P_GLUE_INFO_T prGlueInfo,
		       IN PVOID prSetBuffer, IN UINT_32 u4SetBufferLen, OUT PUINT_32 pu4SetInfoLen);

WLAN_STATUS
reqQueryMaxSendPackets(IN P_GLUE_INFO_T prGlueInfo,
		       OUT PVOID pvQueryBuf,
		       IN UINT_32 u4QueryBufLen, OUT PUINT_32 pu4QueryInfoLen);

WLAN_STATUS
reqQueryMaxListSize(IN P_GLUE_INFO_T prGlueInfo,
		    OUT PVOID pvQryBuf, IN UINT_32 u4QryBufLen, OUT PUINT_32 pu4QryInfoLen);


WLAN_STATUS
reqQueryPnPCapabilities(IN P_GLUE_INFO_T prGlueInfo,
			IN PVOID pvQueryBuf,
			IN UINT_32 u4QueryBufLen, OUT PUINT_32 pu4QueryInfoLen);

WLAN_STATUS
reqQueryAssocInfo(IN P_GLUE_INFO_T prGlueInfo,
		  OUT PVOID pvQueryBuffer,
		  IN UINT_32 u4QueryBufferLen, OUT PUINT_32 pu4QueryInfoLen);

WLAN_STATUS
reqExtQueryConfiguration(IN P_GLUE_INFO_T prGlueInfo,
			 OUT PVOID pvQueryBuffer,
			 IN UINT_32 u4QueryBufferLen, OUT PUINT_32 pu4QueryInfoLen);

WLAN_STATUS
reqExtSetConfiguration(IN P_GLUE_INFO_T prGlueInfo,
		       IN PVOID pvSetBuffer, IN UINT_32 u4SetBufferLen, OUT PUINT_32 pu4SetInfoLen);

#if CFG_TCP_IP_CHKSUM_OFFLOAD
NDIS_STATUS
reqQueryTaskOffload(IN P_GLUE_INFO_T prGlueInfo,
		    OUT PVOID pvQueryBuffer,
		    IN UINT_32 u4QueryBufferLen, OUT PUINT_32 pu4QueryInfoLen);

NDIS_STATUS
reqExtSetTaskOffload(IN P_GLUE_INFO_T prGlueInfo,
		     IN PVOID prSetBuffer, IN UINT_32 u4SetBufferLen, OUT PUINT_32 pu4SetInfoLen);
#endif				/*                           */

/*                                                                            */
/*                                                                            */
/*                                                                            */
WLAN_STATUS
reqQuerySupportedList(IN P_GLUE_INFO_T prGlueInfo,
		      OUT PVOID pvQryBuf, IN UINT_32 u4QryBufLen, OUT PUINT_32 pu4QryInfoLen);

WLAN_STATUS
reqExtSetAcpiDevicePowerState(IN P_GLUE_INFO_T prGlueInfo,
			      IN PVOID pvSetBuffer,
			      IN UINT_32 u4SetBufferLen, OUT PUINT_32 pu4SetInfoLen);

LINT_EXT_HEADER_BEGIN
    BOOLEAN
imageFileMapping(IN NDIS_STRING rFileName,
		 OUT NDIS_HANDLE * pFileHandle,
		 OUT PVOID * ppvMapFileBuf, OUT PUINT_32 pu4FileLength);

BOOLEAN imageFileUnMapping(IN NDIS_HANDLE rFileHandle, OUT PVOID pvMapFileBuf);
LINT_EXT_HEADER_END
/*                                                                              
                                                
                                                                                
*/
#endif				/*           */
