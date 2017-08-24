/*
                                             
 */

#ifndef __REE_TRUSTZONE_H__
#define __REE_TRUSTZONE_H__

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* 
                                        
                                                                     
                    
  
                                            
                                       
 */
typedef struct {
    void *buffer;
    uint32_t size;
} MTEEC_MEM;

/* 
                                         
  
                                  
                                                                         
                                   
                                        
 */
typedef struct {
    uint32_t handle;
    uint32_t offset;
    uint32_t size;
} MTEEC_MEMREF;

/* 
                                         
  
                                            
                                            
 */
typedef struct {
    uint32_t a;
    uint32_t b;
} MTEEC_VALUE;

/* 
                  
  
                                                                                       
                                                                                     
                                       
                                                                           
 */
typedef union {
    MTEEC_MEM mem;
    MTEEC_MEMREF memref;
    MTEEC_VALUE value;
} MTEEC_PARAM;

/* 
                        
  
                      
 */
typedef enum 
{
    TZPT_NONE                      = 0,
    TZPT_VALUE_INPUT               = 1,
    TZPT_VALUE_OUTPUT              = 2,
    TZPT_VALUE_INOUT               = 3,
    TZPT_MEM_INPUT                 = 4,
    TZPT_MEM_OUTPUT                = 5,
    TZPT_MEM_INOUT                 = 6,
    TZPT_MEMREF_INPUT              = 7,
    TZPT_MEMREF_OUTPUT             = 8,
    TZPT_MEMREF_INOUT              = 9,
} TZ_PARAM_TYPES;


//                                                         
//                    
#define TZ_ParamTypes1(t1)          TZ_ParamTypes(t1, TZPT_NONE, TZPT_NONE, TZPT_NONE)
#define TZ_ParamTypes2(t1,t2)       TZ_ParamTypes(t1, t2, TZPT_NONE, TZPT_NONE)
#define TZ_ParamTypes3(t1,t2,t3)    TZ_ParamTypes(t1, t2, t3, TZPT_NONE)
#define TZ_ParamTypes4(t1,t2,t3,t4)    TZ_ParamTypes(t1, t2, t3, t4)

/* 
                                                          
  
                           
                           
                               
                               
                               
                               
                                
 */
static inline uint32_t TZ_ParamTypes(TZ_PARAM_TYPES t1, TZ_PARAM_TYPES t2,
                              TZ_PARAM_TYPES t3, TZ_PARAM_TYPES t4)
{
    return (TZ_PARAM_TYPES)(t1 | (t2 << 8) | (t3 << 16) | (t4 << 24));
}

/* 
                                      
  
                                                        
                                           
 */
static inline TZ_PARAM_TYPES TZ_GetParamTypes(uint32_t paramTypes, int num)
{
    return (TZ_PARAM_TYPES)((paramTypes >> (8*num)) & 0xff);
}

/* 
              
  
                                                        
                                                 
                                                  
  
                 
 */
#define TZ_RESULT_SUCCESS 0x00000000 //                              
#define TZ_RESULT_ERROR_GENERIC 0xFFFF0000 //                    
#define TZ_RESULT_ERROR_ACCESS_DENIED 0xFFFF0001 //                                      
#define TZ_RESULT_ERROR_CANCEL 0xFFFF0002 //                             
#define TZ_RESULT_ERROR_ACCESS_CONFLICT 0xFFFF0003 //                                     
#define TZ_RESULT_ERROR_EXCESS_DATA 0xFFFF0004 //                                                      
#define TZ_RESULT_ERROR_BAD_FORMAT 0xFFFF0005 //                                  
#define TZ_RESULT_ERROR_BAD_PARAMETERS 0xFFFF0006 //                               
#define TZ_RESULT_ERROR_BAD_STATE 0xFFFF0007 //                                             
#define TZ_RESULT_ERROR_ITEM_NOT_FOUND 0xFFFF0008 //                                      
#define TZ_RESULT_ERROR_NOT_IMPLEMENTED 0xFFFF0009 //                                                                 
#define TZ_RESULT_ERROR_NOT_SUPPORTED 0xFFFF000A //                                                                              
#define TZ_RESULT_ERROR_NO_DATA 0xFFFF000B //                           
#define TZ_RESULT_ERROR_OUT_OF_MEMORY 0xFFFF000C //                             
#define TZ_RESULT_ERROR_BUSY 0xFFFF000D //                                              
#define TZ_RESULT_ERROR_COMMUNICATION 0xFFFF000E //                                          
#define TZ_RESULT_ERROR_SECURITY 0xFFFF000F //                               
#define TZ_RESULT_ERROR_SHORT_BUFFER 0xFFFF0010 //                                                           
#define TZ_RESULT_ERROR_INVALID_HANDLE 0xFFFF0011 //                       

typedef int TZ_RESULT;

/* 
                                        
 */
const char *TZ_GetErrorString(TZ_RESULT res);

#ifdef __cplusplus
}
#endif

#endif /*                     */
