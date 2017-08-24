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

/*                                                                              
                                                
                                                                                
*/

/*                                                                            */
/* 
                                                                         
 
                                                          
 
                       
 
*/
/*                                                                            */
static BOOLEAN cnmTimerSetTimer(IN P_ADAPTER_T prAdapter, IN OS_SYSTIME rTimeout)
{
	P_ROOT_TIMER prRootTimer;
	BOOLEAN fgNeedWakeLock;

	ASSERT(prAdapter);

	prRootTimer = &prAdapter->rRootTimer;

	kalSetTimer(prAdapter->prGlueInfo, rTimeout);

	if (rTimeout <= SEC_TO_SYSTIME(WAKE_LOCK_MAX_TIME)) {
		fgNeedWakeLock = TRUE;

		if (!prRootTimer->fgWakeLocked) {
			KAL_WAKE_LOCK(prAdapter, &prRootTimer->rWakeLock);
			prRootTimer->fgWakeLocked = TRUE;
		}
	} else {
		fgNeedWakeLock = FALSE;
	}

	return fgNeedWakeLock;
}

/*                                                                            */
/* 
                                                            
 
                      
 
                
*/
/*                                                                            */
VOID cnmTimerInitialize(IN P_ADAPTER_T prAdapter)
{
	P_ROOT_TIMER prRootTimer;
	KAL_SPIN_LOCK_DECLARATION();

	ASSERT(prAdapter);

	prRootTimer = &prAdapter->rRootTimer;

	/*                                        */

	KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_TIMER);
	LINK_INITIALIZE(&prRootTimer->rLinkHead);
	KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_TIMER);

	KAL_WAKE_LOCK_INIT(prAdapter, &prRootTimer->rWakeLock, "WLAN Timer");
	prRootTimer->fgWakeLocked = FALSE;
	return;
}

/*                                                                            */
/* 
                                                         
                                                                      
 
            
 
                
*/
/*                                                                            */
VOID cnmTimerDestroy(IN P_ADAPTER_T prAdapter)
{
	P_ROOT_TIMER prRootTimer;
	KAL_SPIN_LOCK_DECLARATION();

	ASSERT(prAdapter);

	prRootTimer = &prAdapter->rRootTimer;

	if (prRootTimer->fgWakeLocked) {
		KAL_WAKE_UNLOCK(prAdapter, &prRootTimer->rWakeLock);
		prRootTimer->fgWakeLocked = FALSE;
	}
	KAL_WAKE_LOCK_DESTROY(prAdapter, &prRootTimer->rWakeLock);

	KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_TIMER);
	LINK_INITIALIZE(&prRootTimer->rLinkHead);
	KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_TIMER);

	/*                                                            */

	return;
}

/*                                                                            */
/* 
                                                       
 
                                                  
                                                       
                                                     
 
                
*/
/*                                                                            */
VOID
cnmTimerInitTimer(IN P_ADAPTER_T prAdapter,
		  IN P_TIMER_T prTimer, IN PFN_MGMT_TIMEOUT_FUNC pfFunc, IN UINT_32 u4Data)
{
	ASSERT(prAdapter);

	ASSERT(prTimer);

#if DBG
	/*                                                        */
	if (pfFunc == NULL) {
		DBGLOG(CNM, WARN, ("Init timer with NULL callback function!\n"));
	}
#endif

#if DBG
	ASSERT(prAdapter->rRootTimer.rLinkHead.prNext);
	{
		P_LINK_T prTimerList;
		P_LINK_ENTRY_T prLinkEntry;
		P_TIMER_T prPendingTimer;

		prTimerList = &(prAdapter->rRootTimer.rLinkHead);

		LINK_FOR_EACH(prLinkEntry, prTimerList) {
			prPendingTimer = LINK_ENTRY(prLinkEntry, TIMER_T, rLinkEntry);
			ASSERT(prPendingTimer);
			ASSERT(prPendingTimer != prTimer);
		}
	}
#endif

	LINK_ENTRY_INITIALIZE(&prTimer->rLinkEntry);

	prTimer->pfMgmtTimeOutFunc = pfFunc;
	prTimer->u4Data = u4Data;

	return;
}


/*                                                                            */
/* 
                                                 
 
                                                  
 
                
*/
/*                                                                            */
static VOID
cnmTimerStopTimer_impl(IN P_ADAPTER_T prAdapter, IN P_TIMER_T prTimer, IN BOOLEAN fgAcquireSpinlock)
{
	P_ROOT_TIMER prRootTimer;
	KAL_SPIN_LOCK_DECLARATION();

	ASSERT(prAdapter);
	ASSERT(prTimer);

	prRootTimer = &prAdapter->rRootTimer;

	if (fgAcquireSpinlock) {
		KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_TIMER);
	}

	if (timerPendingTimer(prTimer)) {
		LINK_REMOVE_KNOWN_ENTRY(&prRootTimer->rLinkHead, &prTimer->rLinkEntry);

		/*                                                                
                                                                     
                                                        
   */
		if (LINK_IS_EMPTY(&prRootTimer->rLinkHead)) {
			kalCancelTimer(prAdapter->prGlueInfo);

			if (fgAcquireSpinlock && prRootTimer->fgWakeLocked) {
				KAL_WAKE_UNLOCK(prAdapter, &prRootTimer->rWakeLock);
				prRootTimer->fgWakeLocked = FALSE;
			}
		}
	}

	if (fgAcquireSpinlock) {
		KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_TIMER);
	}
}

/*                                                                            */
/* 
                                                 
 
                                                  
 
                
*/
/*                                                                            */
VOID cnmTimerStopTimer(IN P_ADAPTER_T prAdapter, IN P_TIMER_T prTimer)
{
	ASSERT(prAdapter);
	ASSERT(prTimer);

	cnmTimerStopTimer_impl(prAdapter, prTimer, TRUE);
}

/*                                                                            */
/* 
                                                                 
 
                                                  
                                                                          
                                    
 
                
*/
/*                                                                            */
VOID cnmTimerStartTimer(IN P_ADAPTER_T prAdapter, IN P_TIMER_T prTimer, IN UINT_32 u4TimeoutMs)
{
	P_ROOT_TIMER prRootTimer;
	P_LINK_T prTimerList;
	OS_SYSTIME rExpiredSysTime, rTimeoutSystime;
	KAL_SPIN_LOCK_DECLARATION();

	ASSERT(prAdapter);
	ASSERT(prTimer);

	KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_TIMER);

	prRootTimer = &prAdapter->rRootTimer;
	prTimerList = &prRootTimer->rLinkHead;

	/*                                                                  
                                                 
  */
	if (u4TimeoutMs > MSEC_PER_MIN) {
		ASSERT(u4TimeoutMs <= ((UINT_32) 0xFFFF * MSEC_PER_MIN));

		prTimer->u2Minutes = (UINT_16) (u4TimeoutMs / MSEC_PER_MIN);
		u4TimeoutMs -= (prTimer->u2Minutes * MSEC_PER_MIN);
		if (u4TimeoutMs == 0) {
			u4TimeoutMs = MSEC_PER_MIN;
			prTimer->u2Minutes--;
		}
	} else {
		prTimer->u2Minutes = 0;
	}

	/*                                                           */
	ASSERT(u4TimeoutMs < (((UINT_32) 0x80000000 - MSEC_PER_SEC) / KAL_HZ));
	rTimeoutSystime = MSEC_TO_SYSTIME(u4TimeoutMs);
	rExpiredSysTime = kalGetTimeTick() + rTimeoutSystime;

	/*                                                        */
	if (LINK_IS_EMPTY(prTimerList) ||
	    TIME_BEFORE(rExpiredSysTime, prRootTimer->rNextExpiredSysTime)) {

		prRootTimer->rNextExpiredSysTime = rExpiredSysTime;
		cnmTimerSetTimer(prAdapter, rTimeoutSystime);
	}

	/*                                 */
	prTimer->rExpiredSysTime = rExpiredSysTime;

	if (!timerPendingTimer(prTimer)) {
		LINK_INSERT_TAIL(prTimerList, &prTimer->rLinkEntry);
	}

	KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_TIMER);

	return;
}

/*                                                                            */
/* 
                                                         
 
            
 
                
*/
/*                                                                            */
VOID cnmTimerDoTimeOutCheck(IN P_ADAPTER_T prAdapter)
{
	P_ROOT_TIMER prRootTimer;
	P_LINK_T prTimerList;
	P_LINK_ENTRY_T prLinkEntry;
	P_TIMER_T prTimer;
	OS_SYSTIME rCurSysTime;
	PFN_MGMT_TIMEOUT_FUNC pfMgmtTimeOutFunc;
	UINT_32 u4TimeoutData;
	BOOLEAN fgNeedWakeLock;
	KAL_SPIN_LOCK_DECLARATION();

	ASSERT(prAdapter);

	/*                   */
	KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_TIMER);

	prRootTimer = &prAdapter->rRootTimer;
	prTimerList = &prRootTimer->rLinkHead;

	rCurSysTime = kalGetTimeTick();

	/*                                                 */
	prRootTimer->rNextExpiredSysTime = rCurSysTime + MGMT_MAX_TIMEOUT_INTERVAL;

	LINK_FOR_EACH(prLinkEntry, prTimerList) {
		prTimer = LINK_ENTRY(prLinkEntry, TIMER_T, rLinkEntry);
		ASSERT(prTimer);

		/*                                 */
		if (!TIME_BEFORE(rCurSysTime, prTimer->rExpiredSysTime)) {
			cnmTimerStopTimer_impl(prAdapter, prTimer, FALSE);

			pfMgmtTimeOutFunc = prTimer->pfMgmtTimeOutFunc;
			u4TimeoutData = prTimer->u4Data;

			if (prTimer->u2Minutes > 0) {
				prTimer->u2Minutes--;
				prTimer->rExpiredSysTime =
				    rCurSysTime + MSEC_TO_SYSTIME(MSEC_PER_MIN);
				LINK_INSERT_TAIL(prTimerList, &prTimer->rLinkEntry);
			} else if (pfMgmtTimeOutFunc) {
				KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_TIMER);
				(pfMgmtTimeOutFunc) (prAdapter, u4TimeoutData);
				KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_TIMER);
			}

			/*                                                            
                                                            
    */
			prLinkEntry = (P_LINK_ENTRY_T) prTimerList;

			prRootTimer->rNextExpiredSysTime = rCurSysTime + MGMT_MAX_TIMEOUT_INTERVAL;
		} else if (TIME_BEFORE(prTimer->rExpiredSysTime, prRootTimer->rNextExpiredSysTime)) {
			prRootTimer->rNextExpiredSysTime = prTimer->rExpiredSysTime;
		}
	}			/*                 */

	/*                                                                     
                                               
  */
	fgNeedWakeLock = FALSE;
	if (!LINK_IS_EMPTY(prTimerList)) {
		ASSERT(TIME_AFTER(prRootTimer->rNextExpiredSysTime, rCurSysTime));

		fgNeedWakeLock = cnmTimerSetTimer(prAdapter, (OS_SYSTIME)
						  ((INT_32) prRootTimer->rNextExpiredSysTime -
						   (INT_32) rCurSysTime));
	}

	if (prRootTimer->fgWakeLocked && !fgNeedWakeLock) {
		KAL_WAKE_UNLOCK(prAdapter, &prRootTimer->rWakeLock);
		prRootTimer->fgWakeLocked = FALSE;
	}

	/*                   */
	KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_TIMER);
}
