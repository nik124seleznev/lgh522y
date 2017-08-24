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
#include "precomp.h"

#if CFG_SUPPORT_ROAMING
/*                                                                              
                                                
                                                                                
*/

/*                                                                              
                                                 
                                                                                
*/

/*                                                                              
                                                  
                                                                                
*/

/*                                                                              
                                                   
                                                                                
*/

#if DBG
/*                              */
static PUINT_8 apucDebugRoamingState[ROAMING_STATE_NUM] = {
	(PUINT_8) DISP_STRING("ROAMING_STATE_IDLE"),
	(PUINT_8) DISP_STRING("ROAMING_STATE_DECISION"),
	(PUINT_8) DISP_STRING("ROAMING_STATE_DISCOVERY"),
	(PUINT_8) DISP_STRING("ROAMING_STATE_ROAM")
};

/*              */
#endif				/*     */

/*                                                                              
                                             
                                                                                
*/

#define ROAMING_ENABLE_CHECK(_roam) \
	do { \
		if (!(_roam->fgIsEnableRoaming)) \
			return; \
	} while (0)

/*                                                                              
                                                             
                                                                                
*/

/*                                                                              
                                                
                                                                                
*/

/*                                                                            */
/* 
                                                                             
 
                                   
 
                
*/
/*                                                                            */
VOID roamingFsmInit(IN P_ADAPTER_T prAdapter)
{
	P_ROAMING_INFO_T prRoamingFsmInfo;
	P_CONNECTION_SETTINGS_T prConnSettings;

	DBGLOG(ROAMING, LOUD, ("->roamingFsmInit(): Current Time = %ld\n", kalGetTimeTick()));

	prRoamingFsmInfo = (P_ROAMING_INFO_T) &(prAdapter->rWifiVar.rRoamingInfo);
	prConnSettings = &(prAdapter->rWifiVar.rConnSettings);

	/*                    */
	prRoamingFsmInfo->fgIsEnableRoaming = prConnSettings->fgIsEnableRoaming;
	prRoamingFsmInfo->eCurrentState = ROAMING_STATE_IDLE;
	prRoamingFsmInfo->rRoamingDiscoveryUpdateTime = 0;

	return;
}				/*                         */

/*                                                                            */
/* 
                                                                       
 
                                   
 
                
*/
/*                                                                            */
VOID roamingFsmUninit(IN P_ADAPTER_T prAdapter)
{
	P_ROAMING_INFO_T prRoamingFsmInfo;

	DBGLOG(ROAMING, LOUD, ("->roamingFsmUninit(): Current Time = %ld\n", kalGetTimeTick()));

	prRoamingFsmInfo = (P_ROAMING_INFO_T) &(prAdapter->rWifiVar.rRoamingInfo);

	prRoamingFsmInfo->eCurrentState = ROAMING_STATE_IDLE;

	return;
}				/*                           */

/*                                                                            */
/* 
                                  
 
                                         
                                       
 
                
*/
/*                                                                            */
VOID roamingFsmSendCmd(IN P_ADAPTER_T prAdapter, IN P_CMD_ROAMING_TRANSIT_T prTransit)
{
	P_ROAMING_INFO_T prRoamingFsmInfo;
	WLAN_STATUS rStatus;

	DBGLOG(ROAMING, LOUD, ("->roamingFsmSendCmd(): Current Time = %ld\n", kalGetTimeTick()));

	prRoamingFsmInfo = (P_ROAMING_INFO_T) &(prAdapter->rWifiVar.rRoamingInfo);

	rStatus = wlanSendSetQueryCmd(prAdapter,	/*           */
				      CMD_ID_ROAMING_TRANSIT,	/*       */
				      TRUE,	/*            */
				      FALSE,	/*            */
				      FALSE,	/*         */
				      NULL,	/*                  */
				      NULL,	/*                     */
				      sizeof(CMD_ROAMING_TRANSIT_T),	/*                   */
				      (PUINT_8) prTransit,	/*               */
				      NULL,	/*                  */
				      0	/*                     */
	    );

	/*                                         */

	return;
}				/*                            */

/*                                                                            */
/* 
                                                      
 
                                   
 
              
*/
/*                                                                            */
VOID roamingFsmScanResultsUpdate(IN P_ADAPTER_T prAdapter)
{
	P_ROAMING_INFO_T prRoamingFsmInfo;

	prRoamingFsmInfo = (P_ROAMING_INFO_T) &(prAdapter->rWifiVar.rRoamingInfo);

	/*                          */
	ROAMING_ENABLE_CHECK(prRoamingFsmInfo);

	DBGLOG(ROAMING, LOUD,
	       ("->roamingFsmScanResultsUpdate(): Current Time = %ld\n", kalGetTimeTick()));

	GET_CURRENT_SYSTIME(&prRoamingFsmInfo->rRoamingDiscoveryUpdateTime);

	return;
}				/*                                      */

/*                                                                            */
/* 
                                                      
 
                                            
                                                                          
 
                
*/
/*                                                                            */
VOID roamingFsmSteps(IN P_ADAPTER_T prAdapter, IN ENUM_ROAMING_STATE_T eNextState)
{
	P_ROAMING_INFO_T prRoamingFsmInfo;
	ENUM_ROAMING_STATE_T ePreviousState;
	BOOLEAN fgIsTransition = (BOOLEAN) FALSE;

	prRoamingFsmInfo = (P_ROAMING_INFO_T) &(prAdapter->rWifiVar.rRoamingInfo);

	do {

		/*                        */
#if DBG
		DBGLOG(ROAMING, STATE, ("TRANSITION: [%s] -> [%s]\n",
					apucDebugRoamingState[prRoamingFsmInfo->eCurrentState],
					apucDebugRoamingState[eNextState]));
#else
		DBGLOG(ROAMING, STATE, ("[%d] TRANSITION: [%d] -> [%d]\n",
					DBG_ROAMING_IDX,
					prRoamingFsmInfo->eCurrentState, eNextState));
#endif
		/*                                                                                 */
		ePreviousState = prRoamingFsmInfo->eCurrentState;
		prRoamingFsmInfo->eCurrentState = eNextState;

		fgIsTransition = (BOOLEAN) FALSE;

		/*                                            */
		switch (prRoamingFsmInfo->eCurrentState) {
			/*                                                                  
                                                                             
                                                   
    */
		case ROAMING_STATE_IDLE:
		case ROAMING_STATE_DECISION:
			break;

		case ROAMING_STATE_DISCOVERY:
			{
				OS_SYSTIME rCurrentTime;

				GET_CURRENT_SYSTIME(&rCurrentTime);
				if (CHECK_FOR_TIMEOUT
				    (rCurrentTime, prRoamingFsmInfo->rRoamingDiscoveryUpdateTime,
				     SEC_TO_SYSTIME(ROAMING_DISCOVERY_TIMEOUT_SEC))) {
					DBGLOG(ROAMING, LOUD,
					       ("roamingFsmSteps: DiscoveryUpdateTime Timeout"));
					aisFsmRunEventRoamingDiscovery(prAdapter, TRUE);
				} else {
					DBGLOG(ROAMING, LOUD,
					       ("roamingFsmSteps: DiscoveryUpdateTime Updated"));
					aisFsmRunEventRoamingDiscovery(prAdapter, FALSE);
				}
			}
			break;

		case ROAMING_STATE_ROAM:
			break;

		default:
			ASSERT(0);	/*                                     */
		}
	}	while (fgIsTransition);

	return;

}				/*                          */

/*                                                                            */
/* 
                                                        
 
                                   
 
              
*/
/*                                                                            */
VOID roamingFsmRunEventStart(IN P_ADAPTER_T prAdapter)
{
	P_ROAMING_INFO_T prRoamingFsmInfo;
	ENUM_ROAMING_STATE_T eNextState;
	P_BSS_INFO_T prAisBssInfo;
	CMD_ROAMING_TRANSIT_T rTransit;

	prRoamingFsmInfo = (P_ROAMING_INFO_T) &(prAdapter->rWifiVar.rRoamingInfo);

	/*                          */
	ROAMING_ENABLE_CHECK(prRoamingFsmInfo);

	prAisBssInfo = prAdapter->prAisBssInfo;
	if (prAisBssInfo->eCurrentOPMode != OP_MODE_INFRASTRUCTURE) {
		return;
	}

	DBGLOG(ROAMING, EVENT, ("EVENT-ROAMING START: Current Time = %ld\n", kalGetTimeTick()));

	/*                        */
	/*                                           */
	if (!(prRoamingFsmInfo->eCurrentState == ROAMING_STATE_IDLE
	      || prRoamingFsmInfo->eCurrentState == ROAMING_STATE_ROAM)) {
		return;
	}

	eNextState = ROAMING_STATE_DECISION;
	if (eNextState != prRoamingFsmInfo->eCurrentState) {
		rTransit.u2Event = ROAMING_EVENT_START;
		rTransit.u2Data = prAisBssInfo->ucBssIndex;
		roamingFsmSendCmd(prAdapter, (P_CMD_ROAMING_TRANSIT_T) & rTransit);

		/*                    */
		roamingFsmSteps(prAdapter, eNextState);
	}

	return;
}				/*                                  */

/*                                                                            */
/* 
                                                                     
 
                                   
 
              
*/
/*                                                                            */
VOID roamingFsmRunEventDiscovery(IN P_ADAPTER_T prAdapter, IN UINT_32 u4Param)
{
	P_ROAMING_INFO_T prRoamingFsmInfo;
	ENUM_ROAMING_STATE_T eNextState;

	prRoamingFsmInfo = (P_ROAMING_INFO_T) &(prAdapter->rWifiVar.rRoamingInfo);

	/*                          */
	ROAMING_ENABLE_CHECK(prRoamingFsmInfo);

	DBGLOG(ROAMING, EVENT, ("EVENT-ROAMING DISCOVERY: Current Time = %ld\n", kalGetTimeTick()));

	/*                       */
	/*                                              */
	if (prRoamingFsmInfo->eCurrentState != ROAMING_STATE_DECISION) {
		return;
	}

	eNextState = ROAMING_STATE_DISCOVERY;
	/*                       */
	if (eNextState != prRoamingFsmInfo->eCurrentState) {
		P_BSS_INFO_T prAisBssInfo;
		P_BSS_DESC_T prBssDesc;

		/*                          */
		prAisBssInfo = prAdapter->prAisBssInfo;
		prBssDesc = scanSearchBssDescByBssid(prAdapter, prAisBssInfo->aucBSSID);
		if (prBssDesc) {
			prBssDesc->ucRCPI = (UINT_8) (u4Param & 0xff);
		}

		roamingFsmSteps(prAdapter, eNextState);
	}

	return;
}				/*                                      */

/*                                                                            */
/* 
                                              
 
                                   
 
              
*/
/*                                                                            */
VOID roamingFsmRunEventRoam(IN P_ADAPTER_T prAdapter)
{
	P_ROAMING_INFO_T prRoamingFsmInfo;
	ENUM_ROAMING_STATE_T eNextState;
	CMD_ROAMING_TRANSIT_T rTransit;

	prRoamingFsmInfo = (P_ROAMING_INFO_T) &(prAdapter->rWifiVar.rRoamingInfo);

	/*                          */
	ROAMING_ENABLE_CHECK(prRoamingFsmInfo);

	DBGLOG(ROAMING, EVENT, ("EVENT-ROAMING ROAM: Current Time = %ld\n", kalGetTimeTick()));

	/*                        */
	/*                                        */
	if (prRoamingFsmInfo->eCurrentState != ROAMING_STATE_DISCOVERY) {
		return;
	}

	eNextState = ROAMING_STATE_ROAM;
	/*                   */
	if (eNextState != prRoamingFsmInfo->eCurrentState) {
		rTransit.u2Event = ROAMING_EVENT_ROAM;
		roamingFsmSendCmd(prAdapter, (P_CMD_ROAMING_TRANSIT_T) & rTransit);

		/*                    */
		roamingFsmSteps(prAdapter, eNextState);
	}

	return;
}				/*                                 */

/*                                                                            */
/* 
                                                                            
 
                                   
 
              
*/
/*                                                                            */
VOID roamingFsmRunEventFail(IN P_ADAPTER_T prAdapter, IN UINT_32 u4Param)
{
	P_ROAMING_INFO_T prRoamingFsmInfo;
	ENUM_ROAMING_STATE_T eNextState;
	CMD_ROAMING_TRANSIT_T rTransit;

	prRoamingFsmInfo = (P_ROAMING_INFO_T) &(prAdapter->rWifiVar.rRoamingInfo);

	/*                          */
	ROAMING_ENABLE_CHECK(prRoamingFsmInfo);

	DBGLOG(ROAMING, EVENT,
	       ("EVENT-ROAMING FAIL: reason %x Current Time = %ld\n", u4Param, kalGetTimeTick()));

	/*                        */
	/*                                                 */
	if (prRoamingFsmInfo->eCurrentState != ROAMING_STATE_ROAM) {
		return;
	}

	eNextState = ROAMING_STATE_DECISION;
	/*                  */
	if (eNextState != prRoamingFsmInfo->eCurrentState) {
		rTransit.u2Event = ROAMING_EVENT_FAIL;
		rTransit.u2Data = (UINT_16) (u4Param & 0xffff);
		roamingFsmSendCmd(prAdapter, (P_CMD_ROAMING_TRANSIT_T) & rTransit);

		/*                    */
		roamingFsmSteps(prAdapter, eNextState);
	}

	return;
}				/*                                 */

/*                                                                            */
/* 
                                                                      
 
                                   
 
              
*/
/*                                                                            */
VOID roamingFsmRunEventAbort(IN P_ADAPTER_T prAdapter)
{
	P_ROAMING_INFO_T prRoamingFsmInfo;
	ENUM_ROAMING_STATE_T eNextState;
	CMD_ROAMING_TRANSIT_T rTransit;

	prRoamingFsmInfo = (P_ROAMING_INFO_T) &(prAdapter->rWifiVar.rRoamingInfo);

	ROAMING_ENABLE_CHECK(prRoamingFsmInfo);

	DBGLOG(ROAMING, EVENT, ("EVENT-ROAMING ABORT: Current Time = %ld\n", kalGetTimeTick()));

	eNextState = ROAMING_STATE_IDLE;
	/*                                         */
	if (eNextState != prRoamingFsmInfo->eCurrentState) {
		rTransit.u2Event = ROAMING_EVENT_ABORT;
		roamingFsmSendCmd(prAdapter, (P_CMD_ROAMING_TRANSIT_T) & rTransit);

		/*                    */
		roamingFsmSteps(prAdapter, eNextState);
	}

	return;
}				/*                                  */

/*                                                                            */
/* 
                                     
 
                                         
                                       
 
              
*/
/*                                                                            */
WLAN_STATUS roamingFsmProcessEvent(IN P_ADAPTER_T prAdapter, IN P_CMD_ROAMING_TRANSIT_T prTransit)
{
	DBGLOG(ROAMING, LOUD, ("ROAMING Process Events: Current Time = %ld\n", kalGetTimeTick()));

	if (ROAMING_EVENT_DISCOVERY == prTransit->u2Event) {
		roamingFsmRunEventDiscovery(prAdapter, prTransit->u2Data);
	}

	return WLAN_STATUS_SUCCESS;
}

#endif
