/* 
        
                         
  
                
          
  
                    
                                           
  
               
                           
  
                       
                  
              
  
 */

#ifndef _VAL_VCODEC_UTILITY_H_
#define _VAL_VCODEC_UTILITY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "val_types_public.h"
#include "hal_api.h"

//                       

/* 
                
          
                   
                                
         
                                       
         
                                            
         
                                        
               
                                                                                    
 */
VDDRV_MRESULT_T BPDec(VAL_HANDLE_T hHandle, VAL_HANDLE_T *hBitHandle, WMV_BP_TYPE bpType);


/* 
                
                
                   
                                               
         
                                       
         
                                            
         
                                 
               
                                           
 */
VAL_UINT32_T GetReadBSPt(VAL_HANDLE_T hHandle, VAL_HANDLE_T hBitsHandle, VAL_UINT32_T *pBits);


/* 
                
                 
                   
                                         
         
                                       
               
                                     
 */
VAL_UINT32_T GetBPDecBits(VAL_HANDLE_T hHandle);


/* 
                
                 
                   
                                    
         
                                       
         
                                            
               
                                                                                    
 */
VDDRV_MRESULT_T WMVDecode_HW(VAL_HANDLE_T hHandle, VAL_HANDLE_T *hBitHandle);
//                       

#ifdef __cplusplus
}
#endif

#endif //                               
