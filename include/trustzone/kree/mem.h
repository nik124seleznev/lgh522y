/*
                                                  
 */

#ifndef __KREE_MEM_H__
#define __KREE_MEM_H__

#ifdef CONFIG_MTK_IN_HOUSE_TEE_SUPPORT

#include "trustzone/tz_cross/trustzone.h"


//                           
typedef void* KREE_SESSION_HANDLE;

#define KREE_SESSION_HANDLE_NULL    ((KREE_SESSION_HANDLE)0)
#define KREE_SESSION_HANDLE_FAIL    ((KREE_SESSION_HANDLE)-1)

/* 
                       
  
                                                   
                                                                            
                                                               
  
                                                  
                                                                             
                             
 */

//                            
typedef uint32_t KREE_SHAREDMEM_HANDLE;

//                            
typedef uint32_t KREE_SECUREMEM_HANDLE;

//                                  
typedef uint32_t KREE_SECURECM_HANDLE;

//                                          
typedef uint32_t KREE_RELEASECM_HANDLE;

/* 
                          
  
                                          
  
                                                     
                                             
 */
typedef struct { 
    void* buffer; 
    uint32_t size; 
} KREE_SHAREDMEM_PARAM;

//                               
TZ_RESULT kree_register_sharedmem (KREE_SESSION_HANDLE session, KREE_SHAREDMEM_HANDLE *mem_handle, 
    uint32_t start, uint32_t size, uint32_t map_p);

TZ_RESULT kree_unregister_sharedmem (KREE_SESSION_HANDLE session, KREE_SHAREDMEM_HANDLE mem_handle);    

/* 
                
  
                                                                                        
                                                              
                                                                                     
  
                              
                                                    
                                   
                                                                  
                                                      
                      
  
                                                                                 
                                                                                         
  
                                                                           
                                                                  
  
                                                    
 */
 
/* 
                         
  
                                         
                                                                
                                                         
                          
 */
TZ_RESULT KREE_RegisterSharedmem (    KREE_SESSION_HANDLE session, 
    KREE_SHAREDMEM_HANDLE *shm_handle, KREE_SHAREDMEM_PARAM *param);

    
/* 
                           
  
                                          
                                                 
                          
 */    
TZ_RESULT KREE_UnregisterSharedmem (KREE_SESSION_HANDLE session, KREE_SHAREDMEM_HANDLE shm_handle);

/* 
                
  
                                                    
                                                                                        
                                                                                                              
                                                                                    
                                  
  
                       
                                      
                                          
                 
 */
 
/* 
                           
  
                       
                                                                  
   
                   
                                          
                                                                  
                                             
                                                  
  
                
                                                       
                                             
  
                                          
                                                                  
                                                   
                                                                  
                          
 */ 
TZ_RESULT KREE_AllocSecuremem (KREE_SESSION_HANDLE session, 
    KREE_SECUREMEM_HANDLE *mem_handle, uint32_t alignment, uint32_t size);

/* 
                          
  
                    
                                                        
   
                      
                                                                                        
                                       
  
                                        
                                                 
                                
 */ 
TZ_RESULT KREE_ReferenceSecuremem (KREE_SESSION_HANDLE session, KREE_SECUREMEM_HANDLE mem_handle);

/* 
                            
  
                      
                                                            
                                                      
  
                                        
                                                 
                                
 */ 
TZ_RESULT KREE_UnreferenceSecuremem (KREE_SESSION_HANDLE session, KREE_SECUREMEM_HANDLE mem_handle);

/* 
                      
  
                                                          
                                                                    
                                                          
                                                                                          
  
                             
                                      
                                          
                 
                                                     
 */
 
/* 
                                 
  
                       
                                                                  
   
                   
                                          
                                                                  
                                             
                                                  
  
                
                                                       
                                             
  
                                           
                                                                      
                                                   
                                                                  

                          
 */ 
TZ_RESULT KREE_AllocSecurechunkmem (KREE_SESSION_HANDLE session, 
    KREE_SECURECM_HANDLE *cm_handle, uint32_t alignment, uint32_t size);

/* 
                                
  
                    
                                                        
   
                      
                                                                                        
                                       
  
                                        
                                                      
                                
 */ 
TZ_RESULT KREE_ReferenceSecurechunkmem (KREE_SESSION_HANDLE session, KREE_SECURECM_HANDLE cm_handle);

/* 
                                  
  
                      
                                                            
                                                      
  
                                        
                                                      
                                
 */ 
TZ_RESULT KREE_UnreferenceSecurechunkmem (KREE_SESSION_HANDLE session, KREE_SECURECM_HANDLE cm_handle);

/* 
                                    
  
                                                           
  
                                        
                                    
                                
                                               
                                
 */
TZ_RESULT KREE_ReadSecurechunkmem (KREE_SESSION_HANDLE session, uint32_t offset, uint32_t size, void *buffer);

/* 
                                     
  
                                                            
  
                                        
                                    
                                
                                                
                                
 */
TZ_RESULT KREE_WriteSecurechunkmem (KREE_SESSION_HANDLE session, uint32_t offset, uint32_t size, void *buffer);    

/* 
                                        
  
                                                                
  
                                        
                                                     
                                
 */
TZ_RESULT KREE_GetSecurechunkReleaseSize (KREE_SESSION_HANDLE session, uint32_t *size);    


/* 
                      
  
                                                             
  
                                        
                                                     
                                
 */
TZ_RESULT KREE_GetTEETotalSize (KREE_SESSION_HANDLE session, uint32_t *size);    

#endif /*                                 */
#endif /*                */
