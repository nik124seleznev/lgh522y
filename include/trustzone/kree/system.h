/*
                                         
 */

#ifndef __KREE_H__
#define __KREE_H__

#ifdef CONFIG_MTK_IN_HOUSE_TEE_SUPPORT

#include "trustzone/tz_cross/trustzone.h"

//                           
typedef void* KREE_SESSION_HANDLE;

typedef uint32_t KREE_SHAREDMEM_HANDLE;


/*                    */
/* 
                              
  
                                               
                                                                                      
                      
 */
TZ_RESULT KREE_CreateSession(const char *ta_uuid, KREE_SESSION_HANDLE *pHandle);

/* 
                     
  
                                             
                      
 */
TZ_RESULT KREE_CloseSession(KREE_SESSION_HANDLE handle);


/* 
                          
  
                                                     
                                          
                                                                                 
                                                                      
                                                    
 */
TZ_RESULT KREE_TeeServiceCall(KREE_SESSION_HANDLE handle, uint32_t command,
                              uint32_t paramTypes, MTEEC_PARAM param[4]);

#endif /*                                 */
#endif /*            */
