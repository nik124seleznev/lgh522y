/* 
        
                       
  
                
          
  
                    
                                                
  
               
                           
  
                       
                  
              
  
 */

#ifndef _HAL_TYPES_PUBLIC_H_
#define _HAL_TYPES_PUBLIC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "val_types_public.h"

/* 
                 
               
                   
                                                  
 */
typedef struct _HAL_POWER_T
{
    VAL_VOID_T          *pvHandle;              //                                         
    VAL_UINT32_T        u4HandleSize;           //                                                 
    VAL_DRIVER_TYPE_T   eDriverType;            //                           
    VAL_BOOL_T          fgEnable;               //                         
    VAL_VOID_T          *pvReserved;            //                                  
    VAL_UINT32_T        u4ReservedSize;         //                                                    
} HAL_POWER_T;

/* 
                 
             
                   
                                                
 */
typedef struct _HAL_ISR_T
{
    VAL_VOID_T *pvHandle;                //                                         
    VAL_UINT32_T u4HandleSize;           //                                                 
    VAL_DRIVER_TYPE_T eDriverType;       //                           
    VAL_BOOL_T  fgRegister;              //                                   
    VAL_VOID_T *pvReserved;              //                                  
    VAL_UINT32_T u4ReservedSize;         //                                                    
} HAL_ISR_T;

#ifdef __cplusplus
}
#endif

#endif //                             