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
#include "config.h"
#include "gl_os.h"

extern WLAN_REQ_ENTRY arWlanOidReqTable[];

/*                                                                              
                                                
                                                                                
*/
#if CFG_TCP_IP_CHKSUM_OFFLOAD
/*  */
/*                                                                             */
/*                                                                             */
/*                                                                            */
/*                                                   */
/*  */
/*                                                          */
/*                                                         */
/*                                                                   */
/*                                                                    */
/*  */
NDIS_TASK_OFFLOAD arOffloadTasks[] = {
	{
	 NDIS_TASK_OFFLOAD_VERSION,
	 sizeof(NDIS_TASK_OFFLOAD),
	 TcpIpChecksumNdisTask,
	 0,
	 sizeof(NDIS_TASK_TCP_IP_CHECKSUM)
	 }
};


/*  */
/*                                                        */
/*  */
UINT_32 u4OffloadTasksCount = sizeof(arOffloadTasks) / sizeof(arOffloadTasks[0]);

/*       */
#define  OFFLOAD_V4_TX_IP_OPT_SUPPORT           1
#define  OFFLOAD_V4_TX_TCP_OPT_SUPPORT          1
#define  OFFLOAD_V4_TX_TCP_CHKSUM_SUPPORT       1
#define  OFFLOAD_V4_TX_UDP_CHKSUM_SUPPORT       1
#define  OFFLOAD_V4_TX_IP_CHKSUM_SUPPORT        1

/*       */
#define  OFFLOAD_V4_RX_IP_OPT_SUPPORT           1
#define  OFFLOAD_V4_RX_TCP_OPT_SUPPORT          1
#define  OFFLOAD_V4_RX_TCP_CHKSUM_SUPPORT       1
#define  OFFLOAD_V4_RX_UDP_CHKSUM_SUPPORT       1
#define  OFFLOAD_V4_RX_IP_CHKSUM_SUPPORT        1

/*       */
#define  OFFLOAD_V6_TX_IP_OPT_SUPPORT           0
#define  OFFLOAD_V6_TX_TCP_OPT_SUPPORT          1
#define  OFFLOAD_V6_TX_TCP_CHKSUM_SUPPORT       1
#define  OFFLOAD_V6_TX_UDP_CHKSUM_SUPPORT       1

/*       */
#define  OFFLOAD_V6_RX_IP_OPT_SUPPORT           0
#define  OFFLOAD_V6_RX_TCP_OPT_SUPPORT          1
#define  OFFLOAD_V6_RX_TCP_CHKSUM_SUPPORT       1
#define  OFFLOAD_V6_RX_UDP_CHKSUM_SUPPORT       1

/*  */
/*                                                                                */
/*                                                                               */
/*                                                */
/*  */
NDIS_TASK_TCP_IP_CHECKSUM rTcpIpChecksumTask = {
	/*             */
	{
	 OFFLOAD_V4_TX_IP_OPT_SUPPORT,
	 OFFLOAD_V4_TX_TCP_OPT_SUPPORT,
	 OFFLOAD_V4_TX_TCP_CHKSUM_SUPPORT,
	 OFFLOAD_V4_TX_UDP_CHKSUM_SUPPORT,
	 OFFLOAD_V4_TX_IP_CHKSUM_SUPPORT}
	,
	/*            */
	{
	 OFFLOAD_V4_RX_IP_OPT_SUPPORT,
	 OFFLOAD_V4_RX_TCP_OPT_SUPPORT,
	 OFFLOAD_V4_RX_TCP_CHKSUM_SUPPORT,
	 OFFLOAD_V4_RX_UDP_CHKSUM_SUPPORT,
	 OFFLOAD_V4_RX_IP_CHKSUM_SUPPORT}
	,
	/*             */
	{
	 OFFLOAD_V6_TX_IP_OPT_SUPPORT,
	 OFFLOAD_V6_TX_TCP_OPT_SUPPORT,
	 OFFLOAD_V6_TX_TCP_CHKSUM_SUPPORT,
	 OFFLOAD_V6_TX_UDP_CHKSUM_SUPPORT}
	,
	/*            */
	{
	 OFFLOAD_V6_RX_IP_OPT_SUPPORT,
	 OFFLOAD_V6_RX_TCP_OPT_SUPPORT,
	 OFFLOAD_V6_RX_TCP_CHKSUM_SUPPORT,
	 OFFLOAD_V6_RX_UDP_CHKSUM_SUPPORT}
	,
};
#endif				/*                           */


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
/* 
                                                                               
                           
 
                                                                 
                                                                                     
                                                           
                                                                            
                                                                            
                                                                             
                                                                 
 
                             
                                      
*/
/*                                                                            */
WLAN_STATUS
reqQueryVendorDriverVersion(IN P_GLUE_INFO_T prGlueInfo,
			    OUT PVOID pvQryBuf, IN UINT_32 u4QryBufLen, OUT PUINT_32 pu4QryInfoLen)
{
	DEBUGFUNC("reqQueryVendorDriverVersion");


	ASSERT(prGlueInfo);
	ASSERT(pu4QryInfoLen);

	*pu4QryInfoLen = sizeof(UINT_32);

	if (u4QryBufLen < sizeof(UINT_32)) {
		return WLAN_STATUS_BUFFER_TOO_SHORT;
	}

	ASSERT(pvQryBuf);


	*(PUINT_32) pvQryBuf = ((UINT_32) NIC_DRIVER_MAJOR_VERSION << 16) +
	    (UINT_32) NIC_DRIVER_MINOR_VERSION;

	DBGLOG(REQ, INFO, ("Vendor driver version: 0x%08x\n", *(PUINT_32) pvQryBuf));

	return WLAN_STATUS_SUCCESS;

}				/*                                      */


/*                                                                            */
/* 
                                                                        
 
                                                                 
                                                                                     
                                                           
                                                                            
                                                                            
                                                                             
                                                                 
 
                             
                                      
*/
/*                                                                            */
WLAN_STATUS
reqQueryHardwareStatus(IN P_GLUE_INFO_T prGlueInfo,
		       OUT PVOID pvQryBuf, IN UINT_32 u4QryBufLen, OUT PUINT_32 pu4QryInfoLen)
{
	DEBUGFUNC("reqQueryHardwareStatus");


	ASSERT(prGlueInfo);
	ASSERT(pu4QryInfoLen);

	*pu4QryInfoLen = sizeof(NDIS_HARDWARE_STATUS);

	if (u4QryBufLen < sizeof(NDIS_HARDWARE_STATUS)) {
		return WLAN_STATUS_BUFFER_TOO_SHORT;
	}

	ASSERT(pvQryBuf);

	*(PNDIS_HARDWARE_STATUS) pvQryBuf = NdisHardwareStatusReady;

	return WLAN_STATUS_SUCCESS;

}				/*                                 */


/*                                                                            */
/* 
                                                                             
 
                                                                 
                                                                                     
                                                           
                                                                            
                                                                            
                                                                             
                                                                 
 
                             
                                      
*/
/*                                                                            */
WLAN_STATUS
reqQueryMedia(IN P_GLUE_INFO_T prGlueInfo,
	      OUT PVOID pvQryBuf, IN UINT_32 u4QryBufLen, OUT PUINT_32 pu4QryInfoLen)
{
	DEBUGFUNC("reqQueryMedia");


	ASSERT(prGlueInfo);
	ASSERT(pu4QryInfoLen);

	*pu4QryInfoLen = sizeof(PNDIS_MEDIUM);

	if (u4QryBufLen < sizeof(PNDIS_MEDIUM)) {
		return WLAN_STATUS_BUFFER_TOO_SHORT;
	}

	ASSERT(pvQryBuf);

	*(PNDIS_MEDIUM) pvQryBuf = NdisMedium802_3;

	return WLAN_STATUS_SUCCESS;

}				/*                        */


/*                                                                            */
/* 
                                                                
 
                                                                 
                                                                                     
                                                           
                                                                            
                                                                            
                                                                             
                                                                 
 
                             
                                    
*/
/*                                                                            */
WLAN_STATUS
reqQueryVendorDescription(IN P_GLUE_INFO_T prGlueInfo,
			  OUT PVOID pvQryBuf, IN UINT_32 u4QryBufLen, OUT PUINT_32 pu4QryInfoLen)
{

	DEBUGFUNC("reqQueryVendorDescription");


	ASSERT(prGlueInfo);
	ASSERT(pu4QryInfoLen);

	*pu4QryInfoLen = (UINT_32) prGlueInfo->ucDriverDescLen;

	if (u4QryBufLen < (UINT_32) prGlueInfo->ucDriverDescLen) {
		/*                                            */
		return WLAN_STATUS_INVALID_LENGTH;
	}

	ASSERT(pvQryBuf);

	NdisMoveMemory(pvQryBuf,
		       (PVOID) prGlueInfo->aucDriverDesc, (UINT_32) prGlueInfo->ucDriverDescLen);

	return WLAN_STATUS_SUCCESS;

}				/*                                    */


/*                                                                            */
/* 
                                                                            
                
 
                                                                 
                                                                                     
                                                           
                                                                            
                                                                            
                                                                             
                                                                 
 
                             
                                      
*/
/*                                                                            */
WLAN_STATUS
reqQueryDriverVersion(IN P_GLUE_INFO_T prGlueInfo,
		      OUT PVOID pvQryBuf, IN UINT_32 u4QryBufLen, OUT PUINT_32 pu4QryInfoLen)
{
	DEBUGFUNC("reqQueryDriverVersion");


	ASSERT(prGlueInfo);
	ASSERT(pu4QryInfoLen);

	*pu4QryInfoLen = 2;

	if (u4QryBufLen < 2) {
		return WLAN_STATUS_BUFFER_TOO_SHORT;
	}

	ASSERT(pvQryBuf);

	*(PUINT_16) pvQryBuf = prGlueInfo->u2NdisVersion;

	return WLAN_STATUS_SUCCESS;

}				/*                                */


/*                                                                            */
/* 
                                                                            
                
 
                                                                 
                                                                                     
                                                           
                                                                            
                                                                            
                                                                             
                                                                 
 
                             
                                      
*/
/*                                                                            */
WLAN_STATUS
reqQueryPhysicalMedium(IN P_GLUE_INFO_T prGlueInfo,
		       OUT PVOID pvQryBuf, IN UINT_32 u4QryBufLen, OUT PUINT_32 pu4QryInfoLen)
{
	DEBUGFUNC("reqQueryPhysicalMedium");


	ASSERT(prGlueInfo);
	ASSERT(pu4QryInfoLen);

	*pu4QryInfoLen = sizeof(NDIS_PHYSICAL_MEDIUM);

	if (u4QryBufLen < sizeof(NDIS_PHYSICAL_MEDIUM)) {
		return WLAN_STATUS_BUFFER_TOO_SHORT;
	}

	ASSERT(pvQryBuf);

	*(PNDIS_PHYSICAL_MEDIUM) pvQryBuf = NdisPhysicalMediumWirelessLan;

	return WLAN_STATUS_SUCCESS;

}				/*                                 */


/*                                                                            */
/* 
                                                                
 
                                                                 
                                                                                     
                                                           
                                                                            
                                                                            
                                                                             
                                                                 
 
                             
                                      
*/
/*                                                                            */
WLAN_STATUS
reqQueryMacOptions(IN P_GLUE_INFO_T prGlueInfo,
		   OUT PVOID pvQryBuf, IN UINT_32 u4QryBufLen, OUT PUINT_32 pu4QryInfoLen)
{
	DEBUGFUNC("reqQueryMacOptions");


	ASSERT(prGlueInfo);
	ASSERT(pu4QryInfoLen);

	*pu4QryInfoLen = sizeof(UINT_32);

	if (u4QryBufLen < sizeof(UINT_32)) {
		return WLAN_STATUS_BUFFER_TOO_SHORT;
	}

	ASSERT(pvQryBuf);


	/*                                                              
                                                                    
                                                                 
                                                                   
                                                                 
                                                                    
                                                                  
                                                 */

	/*                                                             
                                              */

#if defined(WINDOWS_CE)		/*                                               */
	*(PUINT_32) pvQryBuf = (UINT_32) (NDIS_MAC_OPTION_TRANSFERS_NOT_PEND |
					  NDIS_MAC_OPTION_COPY_LOOKAHEAD_DATA |
					  NDIS_MAC_OPTION_NO_LOOPBACK |
					  NDIS_MAC_OPTION_8021P_PRIORITY);
#else
	*(PUINT_32) pvQryBuf = (UINT_32) (NDIS_MAC_OPTION_TRANSFERS_NOT_PEND |
					  NDIS_MAC_OPTION_COPY_LOOKAHEAD_DATA |
					  NDIS_MAC_OPTION_NO_LOOPBACK);
#endif

	return WLAN_STATUS_SUCCESS;

}				/*                             */


/*                                                                            */
/* 
                                                                            
                     
 
                                                                 
                                                                                     
                                                           
                                                                            
                                                                            
                                                                             
                                                                 
 
                             
                                      
*/
/*                                                                            */
WLAN_STATUS
reqQueryMediaConnectStatus(IN P_GLUE_INFO_T prGlueInfo,
			   OUT PVOID pvQryBuf, IN UINT_32 u4QryBufLen, OUT PUINT_32 pu4QryInfoLen)
{
	DEBUGFUNC("wlanoidQueryMediaConnectStatus");


	ASSERT(prGlueInfo);
	ASSERT(pu4QryInfoLen);

	*pu4QryInfoLen = sizeof(ENUM_PARAM_MEDIA_STATE_T);

	if (u4QryBufLen < sizeof(ENUM_PARAM_MEDIA_STATE_T)) {
		return WLAN_STATUS_BUFFER_TOO_SHORT;
	}

	ASSERT(pvQryBuf);

	/*                                                                */
	*(P_ENUM_PARAM_MEDIA_STATE_T) pvQryBuf = prGlueInfo->eParamMediaStateIndicated;

	DBGLOG(REQ, INFO, ("Media State: %s\n",
			   ((prGlueInfo->eParamMediaStateIndicated == PARAM_MEDIA_STATE_CONNECTED) ?
			    "Connected" : "Disconnected")));

	return WLAN_STATUS_SUCCESS;

}				/*                                     */


/*                                                                            */
/* 
                                                                         
 
                                                                     
                                                                                         
                                                               
                                                                                
                                                                                
                                                                                 
                                                                     
 
                             
                                      
*/
/*                                                                            */
WLAN_STATUS
reqQueryMaxFrameSize(IN P_GLUE_INFO_T prGlueInfo,
		     OUT PVOID pvQueryBuf, IN UINT_32 u4QueryBufLen, OUT PUINT_32 pu4QueryInfoLen)
{
	DEBUGFUNC("reqQueryMaxFrameSize");


	ASSERT(prGlueInfo);
	ASSERT(pu4QueryInfoLen);

	*pu4QueryInfoLen = sizeof(UINT_32);

	if (u4QueryBufLen < sizeof(UINT_32)) {
		return WLAN_STATUS_BUFFER_TOO_SHORT;
	}

	ASSERT(pvQueryBuf);

	*(PUINT_32) pvQueryBuf = ETHERNET_MAX_PKT_SZ - ETHERNET_HEADER_SZ;

	return WLAN_STATUS_SUCCESS;

}				/*                               */


/*                                                                            */
/* 
                                                                        
                                            
 
                                                                     
                                                                                         
                                                               
                                                                                
                                                                                
                                                                                 
                                                                     
 
                             
                                    
*/
/*                                                                            */
WLAN_STATUS
reqQueryTxBufferSpace(IN P_GLUE_INFO_T prGlueInfo,
		      OUT PVOID pvQueryBuf, IN UINT_32 u4QueryBufLen, OUT PUINT_32 pu4QueryInfoLen)
{
	DEBUGFUNC("reqQueryTxBufferSpace");


	ASSERT(prGlueInfo);
	ASSERT(pu4QueryInfoLen);

	*pu4QueryInfoLen = sizeof(UINT_32);

	if (u4QueryBufLen < sizeof(UINT_32)) {
		return WLAN_STATUS_INVALID_LENGTH;
	}

	ASSERT(pvQueryBuf);

	*(PUINT_32) pvQueryBuf = (UINT_32) CFG_RX_MAX_PKT_NUM * ETHERNET_MAX_PKT_SZ;

	return WLAN_STATUS_SUCCESS;

}				/*                                */


/*                                                                            */
/* 
                                                                        
                                           
 
                                                                     
                                                                                         
                                                               
                                                                                
                                                                                
                                                                                 
                                                                     
 
                             
                                    
*/
/*                                                                            */
WLAN_STATUS
reqQueryRxBufferSpace(IN P_GLUE_INFO_T prGlueInfo,
		      OUT PVOID pvQueryBuf, IN UINT_32 u4QueryBufLen, OUT PUINT_32 pu4QueryInfoLen)
{
	DEBUGFUNC("reqQueryRxBufferSpace");


	ASSERT(prGlueInfo);
	ASSERT(pu4QueryInfoLen);

	*pu4QueryInfoLen = sizeof(UINT_32);

	if (u4QueryBufLen < sizeof(UINT_32)) {
		return WLAN_STATUS_INVALID_LENGTH;
	}

	ASSERT(pvQueryBuf);

	*(PUINT_32) pvQueryBuf = (UINT_32) CFG_RX_MAX_PKT_NUM * ETHERNET_MAX_PKT_SZ;

	return WLAN_STATUS_SUCCESS;

}				/*                                */


/*                                                                            */
/* 
                                                                        
                  
 
                                                                     
                                                                                         
                                                               
                                                                                
                                                                                
                                                                                 
                                                                     
 
                             
                                    
*/
/*                                                                            */
WLAN_STATUS
reqQueryMaxTotalSize(IN P_GLUE_INFO_T prGlueInfo,
		     OUT PVOID pvQueryBuf, IN UINT_32 u4QueryBufLen, OUT PUINT_32 pu4QueryInfoLen)
{
	DEBUGFUNC("reqQueryMaxTotalSize");


	ASSERT(prGlueInfo);
	ASSERT(pu4QueryInfoLen);

	*pu4QueryInfoLen = sizeof(UINT_32);

	if (u4QueryBufLen < sizeof(UINT_32)) {
		return WLAN_STATUS_INVALID_LENGTH;
	}

	ASSERT(pvQueryBuf);

	*(PUINT_32) pvQueryBuf = ETHERNET_MAX_PKT_SZ;

	return WLAN_STATUS_SUCCESS;

}				/*                               */


/*                                                                            */
/* 
                                                                           
                          
 
                                                                     
                                                                                         
                                                               
                                                                                
                                                                                
                                                                                 
                                                                     
 
                             
                                    
*/
/*                                                                            */
WLAN_STATUS
reqQueryRcvErrorAlignment(IN P_GLUE_INFO_T prGlueInfo,
			  OUT PVOID pvQueryBuf,
			  IN UINT_32 u4QueryBufLen, OUT PUINT_32 pu4QueryInfoLen)
{
	DEBUGFUNC("reqQueryRcvErrorAlignment");


	ASSERT(prGlueInfo);
	ASSERT(pu4QueryInfoLen);

	*pu4QueryInfoLen = sizeof(UINT_32);

	if (u4QueryBufLen < sizeof(UINT_32)) {
		return WLAN_STATUS_INVALID_LENGTH;
	}

	ASSERT(pvQueryBuf);

	*(PUINT_32) pvQueryBuf = 0;

	return WLAN_STATUS_SUCCESS;

}				/*                                    */


/*                                                                            */
/* 
                                                                                
                                                                               
                                                      
 
                                                                 
                                                                                  
                                                         
                                                                            
                                                                              
                                                                            
                                                         
 
                             
*/
/*                                                                            */
WLAN_STATUS
reqSetCurrentLookahead(IN P_GLUE_INFO_T prGlueInfo,
		       IN PVOID prSetBuffer, IN UINT_32 u4SetBufferLen, OUT PUINT_32 pu4SetInfoLen)
{
	DEBUGFUNC("reqSetCurrentLookahead");

	ASSERT(prGlueInfo);
	ASSERT(pu4SetInfoLen);

	*pu4SetInfoLen = sizeof(UINT_32);

	return WLAN_STATUS_SUCCESS;

}				/*                                 */


/*                                                                            */
/* 
                                                                           
                                                                            
 
                                                                     
                                                                                         
                                                               
                                                                                
                                                                                
                                                                                 
                                                                     
 
                             
                                    
*/
/*                                                                            */
WLAN_STATUS
reqQueryMaxSendPackets(IN P_GLUE_INFO_T prGlueInfo,
		       OUT PVOID pvQueryBuf, IN UINT_32 u4QueryBufLen, OUT PUINT_32 pu4QueryInfoLen)
{
	DEBUGFUNC("reqQueryMaxSendPackets");


	ASSERT(prGlueInfo);
	ASSERT(pu4QueryInfoLen);

	*pu4QueryInfoLen = sizeof(UINT_32);

	if (u4QueryBufLen < sizeof(UINT_32)) {
		return WLAN_STATUS_INVALID_LENGTH;
	}

	ASSERT(pvQueryBuf);

	*(PUINT_32) pvQueryBuf = MAX_ARRAY_SEND_PACKETS;

	return WLAN_STATUS_SUCCESS;

}				/*                                 */


/*                                                                            */
/* 
                                                                                  
                                   
 
                                                                 
                                                                                     
                                                           
                                                                            
                                                                            
                                                                             
                                                                 
 
                             
                                    
*/
/*                                                                            */
WLAN_STATUS
reqQueryMaxListSize(IN P_GLUE_INFO_T prGlueInfo,
		    OUT PVOID pvQryBuf, IN UINT_32 u4QryBufLen, OUT PUINT_32 pu4QryInfoLen)
{
	DEBUGFUNC("reqQueryMaxListSize");


	ASSERT(prGlueInfo);
	ASSERT(pu4QryInfoLen);

	*pu4QryInfoLen = sizeof(UINT_32);

	if (u4QryBufLen < sizeof(UINT_32)) {
		return WLAN_STATUS_INVALID_LENGTH;
	}

	ASSERT(pvQryBuf);

	*(PUINT_32) pvQryBuf = MAX_NUM_GROUP_ADDR;

	return WLAN_STATUS_SUCCESS;

}				/*                              */


/*                                                                            */
/* 
                                                                       
 
                                                                     
                                                                                         
                                                               
                                                                                
                                                                                
                                                                                 
                                                                     
 
                             
                                      
*/
/*                                                                            */
WLAN_STATUS
reqQueryPnPCapabilities(IN P_GLUE_INFO_T prGlueInfo,
			IN PVOID pvQueryBuf, IN UINT_32 u4QueryBufLen, OUT PUINT_32 pu4QueryInfoLen)
{
	PNDIS_PNP_CAPABILITIES prPwrMgtCap = (PNDIS_PNP_CAPABILITIES) pvQueryBuf;

	DEBUGFUNC("pwrmgtQueryPnPCapabilities");


	ASSERT(prGlueInfo);
	ASSERT(pu4QueryInfoLen);

	*pu4QueryInfoLen = sizeof(NDIS_PNP_CAPABILITIES);

	if (u4QueryBufLen < sizeof(NDIS_PNP_CAPABILITIES)) {
		return WLAN_STATUS_BUFFER_TOO_SHORT;
	}

	ASSERT(pvQueryBuf);

#if CFG_ENABLE_WAKEUP_ON_LAN
	prPwrMgtCap->Flags = PARAM_DEVICE_WAKE_UP_ENABLE;

	prPwrMgtCap->WakeUpCapabilities.MinMagicPacketWakeUp = NdisDeviceStateD2;

	prPwrMgtCap->WakeUpCapabilities.MinPatternWakeUp = NdisDeviceStateD2;
#else
	prPwrMgtCap->Flags = 0;

	prPwrMgtCap->WakeUpCapabilities.MinMagicPacketWakeUp = ParamDeviceStateUnspecified;

	prPwrMgtCap->WakeUpCapabilities.MinPatternWakeUp = ParamDeviceStateUnspecified;
#endif

	prPwrMgtCap->WakeUpCapabilities.MinLinkChangeWakeUp = ParamDeviceStateUnspecified;

	return WLAN_STATUS_SUCCESS;

}				/*                                  */


/*                                                                            */
/* 
                                                                 
                                                                 
                                                                           
 
                                                                     
                                                                                         
                                                               
                                                                                
                                                                                
                                                                                 
                                                                     
 
                             
                                    
*/
/*                                                                            */
WLAN_STATUS
reqQueryAssocInfo(IN P_GLUE_INFO_T prGlueInfo,
		  OUT PVOID pvQueryBuffer,
		  IN UINT_32 u4QueryBufferLen, OUT PUINT_32 pu4QueryInfoLen)
{

#if DBG
	PNDIS_802_11_ASSOCIATION_INFORMATION prAssocInfo =
	    (PNDIS_802_11_ASSOCIATION_INFORMATION) pvQueryBuffer;
	PUINT_8 cp;
#endif

	DEBUGFUNC("wlanoidQueryAssocInfo");


	ASSERT(prGlueInfo);
	ASSERT(pu4QueryInfoLen);

	*pu4QueryInfoLen = sizeof(NDIS_802_11_ASSOCIATION_INFORMATION) +
	    prGlueInfo->rNdisAssocInfo.RequestIELength +
	    prGlueInfo->rNdisAssocInfo.ResponseIELength;

	if (u4QueryBufferLen < *pu4QueryInfoLen) {
		return WLAN_STATUS_INVALID_LENGTH;
	}

	ASSERT(pvQueryBuffer);

	kalMemCopy(pvQueryBuffer, (PVOID) & prGlueInfo->rNdisAssocInfo, *pu4QueryInfoLen);

#if DBG
	/*                                                 */
	DBGLOG(REQ, INFO, ("QUERY: Assoc Info - Length: %d\n", prAssocInfo->Length));

	DBGLOG(REQ, INFO, ("AvailableRequestFixedIEs: 0x%04x\n",
			   prAssocInfo->AvailableRequestFixedIEs));
	DBGLOG(REQ, INFO, ("Request Capabilities: 0x%04x\n",
			   prAssocInfo->RequestFixedIEs.Capabilities));
	DBGLOG(REQ, INFO, ("Request Listen Interval: 0x%04x\n",
			   prAssocInfo->RequestFixedIEs.ListenInterval));
	cp = (PUINT_8) &prAssocInfo->RequestFixedIEs.CurrentAPAddress;
	DBGLOG(REQ, INFO, ("CurrentAPAddress: %02x-%02x-%02x-%02x-%02x-%02x\n",
			   cp[0], cp[1], cp[2], cp[3], cp[4], cp[5]));
	DBGLOG(REQ, INFO, ("Request IEs: length=%d, offset=%d\n",
			   prAssocInfo->RequestIELength, prAssocInfo->OffsetRequestIEs));

	cp = (PUINT_8) pvQueryBuffer + sizeof(NDIS_802_11_ASSOCIATION_INFORMATION);
	DBGLOG_MEM8(REQ, INFO, cp, prAssocInfo->RequestIELength);
	cp += prAssocInfo->RequestIELength;

	DBGLOG(REQ, INFO, ("AvailableResponseFixedIEs: 0x%04x\n",
			   prAssocInfo->AvailableResponseFixedIEs));
	DBGLOG(REQ, INFO, ("Response Capabilities: 0x%04x\n",
			   prAssocInfo->ResponseFixedIEs.Capabilities));
	DBGLOG(REQ, INFO, ("StatusCode: 0x%04x\n", prAssocInfo->ResponseFixedIEs.StatusCode));
	DBGLOG(REQ, INFO, ("AssociationId: 0x%04x\n", prAssocInfo->ResponseFixedIEs.AssociationId));
	DBGLOG(REQ, INFO, ("Response IEs: length=%d, offset=%d\n",
			   prAssocInfo->ResponseIELength, prAssocInfo->OffsetResponseIEs));
	DBGLOG_MEM8(REQ, INFO, cp, prAssocInfo->ResponseIELength);
#endif

	return WLAN_STATUS_SUCCESS;

}				/*                            */

/*                                                                            */
/* 
                                                                             
                               
 
                                                                     
                                                                                         
                                                               
                                                                                
                                                                                
                                                                                 
                                                                     
 
                             
                                    
*/
/*                                                                            */
WLAN_STATUS
reqExtQueryConfiguration(IN P_GLUE_INFO_T prGlueInfo,
			 OUT PVOID pvQueryBuffer,
			 IN UINT_32 u4QueryBufferLen, OUT PUINT_32 pu4QueryInfoLen)
{
	P_PARAM_802_11_CONFIG_T prQueryConfig = (P_PARAM_802_11_CONFIG_T) pvQueryBuffer;
	WLAN_STATUS rStatus = WLAN_STATUS_SUCCESS;
	UINT_32 u4QueryInfoLen = 0;

	DEBUGFUNC("wlanoidQueryConfiguration");


	ASSERT(prGlueInfo);
	ASSERT(pu4QueryInfoLen);

	*pu4QueryInfoLen = sizeof(PARAM_802_11_CONFIG_T);
	if (u4QueryBufferLen < sizeof(PARAM_802_11_CONFIG_T)) {
		return WLAN_STATUS_INVALID_LENGTH;
	}

	ASSERT(pvQueryBuffer);

	kalMemZero(prQueryConfig, sizeof(PARAM_802_11_CONFIG_T));

	/*                                         */
	prQueryConfig->u4Length = sizeof(PARAM_802_11_CONFIG_T);

	/*                 */
	rStatus = wlanoidQueryBeaconInterval(prGlueInfo->prAdapter,
					     &prQueryConfig->u4BeaconPeriod,
					     sizeof(UINT_32), &u4QueryInfoLen);

	if (rStatus != WLAN_STATUS_SUCCESS)
		return rStatus;

	/*             */
	rStatus = wlanoidQueryAtimWindow(prGlueInfo->prAdapter,
					 &prQueryConfig->u4ATIMWindow,
					 sizeof(UINT_32), &u4QueryInfoLen);

	if (rStatus != WLAN_STATUS_SUCCESS)
		return rStatus;

	/*                   */
	rStatus = wlanoidQueryFrequency(prGlueInfo->prAdapter,
					&prQueryConfig->u4DSConfig,
					sizeof(UINT_32), &u4QueryInfoLen);

	prQueryConfig->rFHConfig.u4Length = sizeof(PARAM_802_11_CONFIG_FH_T);

	return rStatus;

}				/*                                   */


/*                                                                            */
/* 
                                                                           
              
 
                                                                 
                                                                                  
                                                         
                                                                            
                                                                              
                                                                            
                                                         
 
                             
                                    
                                  
*/
/*                                                                            */
WLAN_STATUS
reqExtSetConfiguration(IN P_GLUE_INFO_T prGlueInfo,
		       IN PVOID pvSetBuffer, IN UINT_32 u4SetBufferLen, OUT PUINT_32 pu4SetInfoLen)
{
	WLAN_STATUS rStatus = WLAN_STATUS_SUCCESS;
	P_PARAM_802_11_CONFIG_T prNewConfig = (P_PARAM_802_11_CONFIG_T) pvSetBuffer;
	UINT_32 u4SetInfoLen = 0;

	DEBUGFUNC("wlanoidSetConfiguration");

	ASSERT(prGlueInfo);
	ASSERT(pu4SetInfoLen);

	*pu4SetInfoLen = sizeof(PARAM_802_11_CONFIG_T);

	if (u4SetBufferLen < *pu4SetInfoLen) {
		return WLAN_STATUS_INVALID_LENGTH;
	}

	/*                                                                          */
	if (wlanQueryTestMode(prGlueInfo->prAdapter) == FALSE &&
	    prGlueInfo->eParamMediaStateIndicated == PARAM_MEDIA_STATE_CONNECTED) {
		return WLAN_STATUS_NOT_ACCEPTED;
	}

	ASSERT(pvSetBuffer);

	/*                 */
	rStatus = wlanoidSetBeaconInterval(prGlueInfo->prAdapter,
					   &prNewConfig->u4BeaconPeriod,
					   sizeof(UINT_32), pu4SetInfoLen);

	if (wlanQueryTestMode(prGlueInfo->prAdapter) == FALSE && rStatus != WLAN_STATUS_SUCCESS) {
		return rStatus;
	}
	/*             */
	rStatus = wlanoidSetAtimWindow(prGlueInfo->prAdapter,
				       &prNewConfig->u4ATIMWindow, sizeof(UINT_32), pu4SetInfoLen);

	if (wlanQueryTestMode(prGlueInfo->prAdapter) == FALSE && rStatus != WLAN_STATUS_SUCCESS) {
		return rStatus;
	}
	/*                   */
	rStatus = wlanoidSetFrequency(prGlueInfo->prAdapter,
				      &prNewConfig->u4DSConfig, sizeof(UINT_32), pu4SetInfoLen);

	return rStatus;

}				/*                                 */


#if CFG_TCP_IP_CHKSUM_OFFLOAD
/*                                                                            */
/* 
                                                                    
 
                                                                     
                                                                                         
                                                               
                                                                                
                                                                                
                                                                                 
                                                                     
 
                             
                                      
                                   
*/
/*                                                                            */
NDIS_STATUS
reqQueryTaskOffload(IN P_GLUE_INFO_T prGlueInfo,
		    OUT PVOID pvQueryBuffer,
		    IN UINT_32 u4QueryBufferLen, OUT PUINT_32 pu4QueryInfoLen)
{
	PNDIS_TASK_OFFLOAD_HEADER prNdisTaskOffloadHdr;
	PNDIS_TASK_OFFLOAD prTaskOffload;
	PNDIS_TASK_TCP_IP_CHECKSUM prTcpIpChecksumTask;
	UINT_32 u4InfoLen;
	UINT_32 i;

	DEBUGFUNC("reqQueryTaskOffload");


	ASSERT(prGlueInfo);
	ASSERT(pu4QueryInfoLen);

	/*  */
	/*                                                                      */
	/*              */
	/*  */
	u4InfoLen = sizeof(NDIS_TASK_OFFLOAD_HEADER) +
	    FIELD_OFFSET(NDIS_TASK_OFFLOAD, TaskBuffer) + sizeof(NDIS_TASK_TCP_IP_CHECKSUM);

	if (u4InfoLen > u4QueryBufferLen) {
		*pu4QueryInfoLen = u4InfoLen;
		DBGLOG(REQ, TRACE, ("ulInfoLen(%d) > queryBufferLen(%d)\n",
				    u4InfoLen, u4QueryBufferLen));
		return NDIS_STATUS_BUFFER_TOO_SHORT;
	}

	ASSERT(pvQueryBuffer);

	/*  */
	/*                                      */
	/*  */
	prNdisTaskOffloadHdr = (PNDIS_TASK_OFFLOAD_HEADER) pvQueryBuffer;

	/*  */
	/*                                                                 */
	/*  */
	if (prNdisTaskOffloadHdr->EncapsulationFormat.Encapsulation != IEEE_802_3_Encapsulation) {
		DBGLOG(REQ, TRACE, ("Encapsulation  type is not supported.\n"));

		prNdisTaskOffloadHdr->OffsetFirstTask = 0;
		return NDIS_STATUS_NOT_SUPPORTED;
	}
	/*  */
	/*                                                                             */
	/*  */
	if ((prNdisTaskOffloadHdr->Size != sizeof(NDIS_TASK_OFFLOAD_HEADER)) ||
	    (prNdisTaskOffloadHdr->Version != NDIS_TASK_OFFLOAD_VERSION)) {
		DBGLOG(REQ, TRACE, ("Size or Version is not correct.\n"));

		prNdisTaskOffloadHdr->OffsetFirstTask = 0;
		return NDIS_STATUS_NOT_SUPPORTED;
	}
	/*  */
	/*                                                                  */
	/*                                                                 */
	/*                                                                  */
	/*  */
	prNdisTaskOffloadHdr->OffsetFirstTask = prNdisTaskOffloadHdr->Size;

	/*  */
	/*                                                                 */
	/*  */
	prTaskOffload = (PNDIS_TASK_OFFLOAD) ((PUCHAR) (pvQueryBuffer) +
					      prNdisTaskOffloadHdr->Size);
	/*  */
	/*                                                          */
	/*  */
	for (i = 0; i < u4OffloadTasksCount; i++) {
		prTaskOffload->Size = arOffloadTasks[i].Size;
		prTaskOffload->Version = arOffloadTasks[i].Version;
		prTaskOffload->Task = arOffloadTasks[i].Task;
		prTaskOffload->TaskBufferLength = arOffloadTasks[i].TaskBufferLength;

		/*  */
		/*                   */
		/*  */
		if (i != u4OffloadTasksCount - 1) {
			prTaskOffload->OffsetNextTask =
			    FIELD_OFFSET(NDIS_TASK_OFFLOAD,
					 TaskBuffer) + prTaskOffload->TaskBufferLength;
		} else {
			prTaskOffload->OffsetNextTask = 0;
		}

		switch (arOffloadTasks[i].Task) {
			/*  */
			/*                              */
			/*  */
		case TcpIpChecksumNdisTask:
			prTcpIpChecksumTask =
			    (PNDIS_TASK_TCP_IP_CHECKSUM) prTaskOffload->TaskBuffer;

			NdisMoveMemory(prTcpIpChecksumTask,
				       &rTcpIpChecksumTask, sizeof(rTcpIpChecksumTask));
			break;

		default:
			break;
		}

		/*  */
		/*                                 */
		/*  */
		if (i != u4OffloadTasksCount) {
			prTaskOffload = (PNDIS_TASK_OFFLOAD)
			    ((PUCHAR) prTaskOffload + prTaskOffload->OffsetNextTask);
		}
	}

	/*  */
	/*                                                      */
	/*  */
	*pu4QueryInfoLen = u4InfoLen;

	DBGLOG(REQ, TRACE, ("Offloading is set.\n"));
	/*                                                         */

	return NDIS_STATUS_SUCCESS;

}				/*                              */


/*                                                                            */
/* 
                                                                  
 
                                                                 
                                                                                  
                                                         
                                                                            
                                                                              
                                                                            
                                                         
 
                             
                                  
                                    
                             
                                   
*/
/*                                                                            */
NDIS_STATUS
reqExtSetTaskOffload(IN P_GLUE_INFO_T prGlueInfo,
		     IN PVOID prSetBuffer, IN UINT_32 u4SetBufferLen, OUT PUINT_32 pu4SetInfoLen)
{
	PNDIS_TASK_OFFLOAD_HEADER pNdisTaskOffloadHdr;
	PNDIS_TASK_OFFLOAD TaskOffload;
	PNDIS_TASK_OFFLOAD TmpOffload;
	PNDIS_TASK_TCP_IP_CHECKSUM pTcpIpChecksumTask;
	UINT i;
	UINT_32 u4BytesRead = 0;
	UINT_32 u4FlagTcpIpChksum = 0;
	UINT_32 u4ExtBytesRead = 0;
	NDIS_STATUS rStatus = NDIS_STATUS_SUCCESS;

	DEBUGFUNC("reqExtSetTaskOffload");


	ASSERT(prGlueInfo);
	ASSERT(pu4SetInfoLen);

	DBGLOG_MEM8(REQ, TRACE, prSetBuffer, u4SetBufferLen);

	u4BytesRead = sizeof(NDIS_TASK_OFFLOAD_HEADER);

	/*  */
	/*                                                        */
	/*                                                                           */
	/*  */
	pNdisTaskOffloadHdr = (PNDIS_TASK_OFFLOAD_HEADER) prSetBuffer;
	if (pNdisTaskOffloadHdr->EncapsulationFormat.Encapsulation != IEEE_802_3_Encapsulation) {
		DBGLOG(REQ, TRACE,
		       ("pNdisTaskOffloadHdr->EncapsulationFormat.Encapsulation != IEEE_802_3_Encapsulation\n"));
		pNdisTaskOffloadHdr->OffsetFirstTask = 0;
		return NDIS_STATUS_INVALID_DATA;
	}
	/*  */
	/*                           */
	/*  */
	if (pNdisTaskOffloadHdr->OffsetFirstTask == 0) {
		*pu4SetInfoLen = u4SetBufferLen;
		DBGLOG(REQ, TRACE, ("No offload task is set!!\n"));

		/*                                                 */
		u4FlagTcpIpChksum = 0;
		wlanSetInformation(prGlueInfo->prAdapter,
				   wlanoidSetCSUMOffload,
				   &u4FlagTcpIpChksum, sizeof(u4FlagTcpIpChksum), &u4ExtBytesRead);

		return NDIS_STATUS_PENDING;
	}
	/*  */
	/*                              */
	/*  */
	if (pNdisTaskOffloadHdr->OffsetFirstTask < pNdisTaskOffloadHdr->Size) {
		DBGLOG(REQ, TRACE,
		       ("pNdisTaskOffloadHdr->OffsetFirstTask (%d) < pNdisTaskOffloadHdr->Size (%d)\n",
			pNdisTaskOffloadHdr->OffsetFirstTask, pNdisTaskOffloadHdr->Size));
		pNdisTaskOffloadHdr->OffsetFirstTask = 0;
		return NDIS_STATUS_FAILURE;
	}
	/*  */
	/*                                */
	/*  */
	if (u4SetBufferLen < (pNdisTaskOffloadHdr->OffsetFirstTask + sizeof(NDIS_TASK_OFFLOAD))) {
		*pu4SetInfoLen = pNdisTaskOffloadHdr->OffsetFirstTask + sizeof(NDIS_TASK_OFFLOAD);

		DBGLOG(REQ, TRACE,
		       ("response of task offload does not have sufficient space even for 1 offload task!!\n"));
		return NDIS_STATUS_INVALID_LENGTH;
	}
	/*  */
	/*                                                          */
	/*  */
	TaskOffload = (NDIS_TASK_OFFLOAD *)
	    ((PUCHAR) pNdisTaskOffloadHdr + pNdisTaskOffloadHdr->OffsetFirstTask);

	TmpOffload = TaskOffload;

	/*  */
	/*                                                                  */
	/*  */
	while (TmpOffload) {

		u4BytesRead += FIELD_OFFSET(NDIS_TASK_OFFLOAD, TaskBuffer);

		switch (TmpOffload->Task) {

		case TcpIpChecksumNdisTask:
			/*  */
			/*                                   */
			/*  */
			if (u4SetBufferLen < u4BytesRead + sizeof(NDIS_TASK_TCP_IP_CHECKSUM)) {
				*pu4SetInfoLen = u4BytesRead + sizeof(NDIS_TASK_TCP_IP_CHECKSUM);
				return NDIS_STATUS_INVALID_LENGTH;
			}
			/*  */
			/*               */
			/*  */
			for (i = 0; i < u4OffloadTasksCount; i++) {
				if ((arOffloadTasks[i].Task == TmpOffload->Task) &&
				    (arOffloadTasks[i].Version == TmpOffload->Version)) {
					break;
				}
			}
			/*  */
			/*                       */
			/*  */
			if (i == u4OffloadTasksCount) {
				return NDIS_STATUS_NOT_SUPPORTED;
			}
			/*  */
			/*                                                                     */
			/*                                      */
			/*                                                                   */
			/*  */

			pTcpIpChecksumTask = (PNDIS_TASK_TCP_IP_CHECKSUM) TmpOffload->TaskBuffer;

			if (pTcpIpChecksumTask->V4Transmit.IpChecksum) {
				/*  */
				/*                                                                      */
				/*                   */
				/*  */
				if (rTcpIpChecksumTask.V4Transmit.IpChecksum == 0) {
					return NDIS_STATUS_NOT_SUPPORTED;
				}

				DBGLOG(REQ, TRACE, ("Set Sending IP offloading.\n"));
				/*  */
				/*                            */
				/*  */
				u4FlagTcpIpChksum |= CSUM_OFFLOAD_EN_TX_IP;
			}

			if (pTcpIpChecksumTask->V4Transmit.TcpChecksum) {
				/*  */
				/*                                                                   */
				/*                 */
				/*  */
				if (rTcpIpChecksumTask.V4Transmit.TcpChecksum == 0) {
					return NDIS_STATUS_NOT_SUPPORTED;
				}

				DBGLOG(REQ, TRACE, ("Set Sending TCP offloading.\n"));
				/*  */
				/*                             */
				/*  */
				u4FlagTcpIpChksum |= CSUM_OFFLOAD_EN_TX_TCP;
			}

			if (pTcpIpChecksumTask->V4Transmit.UdpChecksum) {
				/*  */
				/*                                                                */
				/*                        */
				/*  */
				if (rTcpIpChecksumTask.V4Transmit.UdpChecksum == 0) {
					return NDIS_STATUS_NOT_SUPPORTED;
				}

				DBGLOG(REQ, TRACE, ("Set Transmit UDP offloading.\n"));
				/*  */
				/*                             */
				/*  */
				u4FlagTcpIpChksum |= CSUM_OFFLOAD_EN_TX_UDP;
			}

			if (pTcpIpChecksumTask->V6Transmit.TcpChecksum) {
				/*  */
				/*                                                                  */
				/*                        */
				/*  */
				if (rTcpIpChecksumTask.V6Transmit.TcpChecksum == 0) {
					return NDIS_STATUS_NOT_SUPPORTED;
				}
				DBGLOG(REQ, TRACE, ("Set IPv6 Transmit TCP offloading.\n"));
				/*  */
				/*                               */
				/*  */
				u4FlagTcpIpChksum |= CSUM_OFFLOAD_EN_TX_TCP;
			}

			if (pTcpIpChecksumTask->V6Transmit.UdpChecksum) {
				/*  */
				/*                                                                  */
				/*                        */
				/*  */
				if (rTcpIpChecksumTask.V6Transmit.UdpChecksum == 0) {
					return NDIS_STATUS_NOT_SUPPORTED;
				}
				DBGLOG(REQ, TRACE, ("Set IPv6 Transmit UDP offloading.\n"));
				/*  */
				/*                               */
				/*  */
				u4FlagTcpIpChksum |= CSUM_OFFLOAD_EN_TX_UDP;
			}
			/*  */
			/*                                                        */
			/*  */
			if (pTcpIpChecksumTask->V4Receive.IpChecksum) {
				/*  */
				/*                                                                 */
				/*                        */
				/*  */
				if (rTcpIpChecksumTask.V4Receive.IpChecksum == 0) {
					return NDIS_STATUS_NOT_SUPPORTED;
				}
				DBGLOG(REQ, TRACE, ("Set Recieve IP offloading.\n"));
				/*  */
				/*                              */
				/*  */
				u4FlagTcpIpChksum |= CSUM_OFFLOAD_EN_RX_IPv4;
			}

			if (pTcpIpChecksumTask->V4Receive.TcpChecksum) {
				/*  */
				/*                                                                  */
				/*                        */
				/*  */
				if (rTcpIpChecksumTask.V4Receive.TcpChecksum == 0) {
					return NDIS_STATUS_NOT_SUPPORTED;
				}
				DBGLOG(REQ, TRACE, ("Set Recieve TCP offloading.\n"));
				/*  */
				/*                               */
				/*  */
				u4FlagTcpIpChksum |= CSUM_OFFLOAD_EN_RX_TCP;
			}

			if (pTcpIpChecksumTask->V4Receive.UdpChecksum) {
				/*  */
				/*                                                                  */
				/*                        */
				/*  */
				if (rTcpIpChecksumTask.V4Receive.UdpChecksum == 0) {
					return NDIS_STATUS_NOT_SUPPORTED;
				}
				DBGLOG(REQ, TRACE, ("Set Recieve UDP offloading.\n"));
				/*  */
				/*                               */
				/*  */
				u4FlagTcpIpChksum |= CSUM_OFFLOAD_EN_RX_UDP;
			}

			if (pTcpIpChecksumTask->V6Receive.TcpChecksum) {
				/*  */
				/*                                                                  */
				/*                        */
				/*  */
				if (rTcpIpChecksumTask.V6Receive.TcpChecksum == 0) {
					return NDIS_STATUS_NOT_SUPPORTED;
				}
				DBGLOG(REQ, TRACE, ("Set IPv6 Recieve TCP offloading.\n"));
				/*  */
				/*                               */
				/*  */
				u4FlagTcpIpChksum |=
				    (CSUM_OFFLOAD_EN_RX_IPv6 | CSUM_OFFLOAD_EN_RX_TCP);
			}

			if (pTcpIpChecksumTask->V6Receive.UdpChecksum) {
				/*  */
				/*                                                                  */
				/*                        */
				/*  */
				if (rTcpIpChecksumTask.V6Receive.UdpChecksum == 0) {
					return NDIS_STATUS_NOT_SUPPORTED;
				}
				DBGLOG(REQ, TRACE, ("Set IPv6 Recieve UDP offloading.\n"));
				/*  */
				/*                               */
				/*  */
				u4FlagTcpIpChksum |=
				    (CSUM_OFFLOAD_EN_RX_IPv6 | CSUM_OFFLOAD_EN_RX_UDP);
			}
			/*                                                                          */
			/*                                                                      */
			/*                                                 */
			/*  */
			if (pTcpIpChecksumTask->V6Transmit.IpOptionsSupported ||
			    pTcpIpChecksumTask->V6Receive.IpOptionsSupported) {
				return NDIS_STATUS_NOT_SUPPORTED;
			}

			/*                                               */
			wlanSetInformation(prGlueInfo->prAdapter,
					   wlanoidSetCSUMOffload,
					   &u4FlagTcpIpChksum,
					   sizeof(u4FlagTcpIpChksum), &u4ExtBytesRead);

			rStatus = NDIS_STATUS_PENDING;

			u4BytesRead += sizeof(NDIS_TASK_TCP_IP_CHECKSUM);
			break;

/*          */
			/*  */
			/*                                                                      */
			/*                                                                        */
			/*  */
/*                                   */
		}

		/*  */
		/*                                     */
		/*  */
		if (TmpOffload->OffsetNextTask) {
			TmpOffload = (PNDIS_TASK_OFFLOAD)
			    ((PUCHAR) TmpOffload + TmpOffload->OffsetNextTask);
		} else {
			TmpOffload = NULL;
		}

	}			/*       */

	*pu4SetInfoLen = u4BytesRead;

	return rStatus;

}				/*                               */
#endif				/*                           */


/*                                                                            */
/* 
                                                                                     
                                                                                     
 
                                                       
                                                                              
                                                        
                                                                            
                                                                               
                                                         
 
                             
                                                                 
                                    
 
*/
/*                                                                            */
WLAN_STATUS
reqExtSetAcpiDevicePowerState(IN P_GLUE_INFO_T prGlueInfo,
			      IN PVOID pvSetBuffer,
			      IN UINT_32 u4SetBufferLen, OUT PUINT_32 pu4SetInfoLen)
{
	ASSERT(prGlueInfo);
	ASSERT(pvSetBuffer);
	ASSERT(pu4SetInfoLen);

	return wlanSetInformation(prGlueInfo->prAdapter,
				  wlanoidSetAcpiDevicePowerState,
				  pvSetBuffer, u4SetBufferLen, pu4SetInfoLen);
}

/*                                                                            */
/* 
                                                                  
 
                                                         
                                                                    
                                                                      
                                                                         
 
                               
                            
*/
/*                                                                            */
BOOLEAN
imageFileMapping(IN NDIS_STRING rFileName,
		 OUT NDIS_HANDLE * pFileHandle,
		 OUT PVOID * ppvMapFileBuf, OUT PUINT_32 pu4FileLength)
{
	NDIS_STATUS rStatus = NDIS_STATUS_FAILURE;
	NDIS_PHYSICAL_ADDRESS high = NDIS_PHYSICAL_ADDRESS_CONST(-1, -1);
	UINT FileLength;

	NdisOpenFile(&rStatus, pFileHandle, &FileLength, &rFileName, high);

	if (rStatus != NDIS_STATUS_SUCCESS) {
		DBGLOG(REQ, WARN, ("Use builded image\n"));
		return FALSE;
	} else {
		rStatus = NDIS_STATUS_FAILURE;
		NdisMapFile(&rStatus, ppvMapFileBuf, *pFileHandle);
		if (rStatus != NDIS_STATUS_SUCCESS) {
			NdisCloseFile(*pFileHandle);
			DBGLOG(REQ, INFO, ("map file fail!!\n"));
			return FALSE;
		}
	}

	*pu4FileLength = FileLength;

	return TRUE;
}

/*                                                                            */
/* 
                                                                      
 
                                                         
                                                                      
                                                                         
 
                               
                            
*/
/*                                                                            */
BOOLEAN imageFileUnMapping(IN NDIS_HANDLE rFileHandle, OUT PVOID pvMapFileBuf)
{
	if (rFileHandle) {
		if (pvMapFileBuf) {
			NdisUnmapFile(rFileHandle);
		}
		DBGLOG(REQ, INFO, ("closed open file\n"));
		NdisCloseFile(rFileHandle);
	}
	return TRUE;
}
