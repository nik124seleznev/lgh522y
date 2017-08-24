/* 
        
              
  
                
               
  
                    
               
  
               
                           
  
                       
                  
              
  
 */

#ifndef _VAL_LOG_H_
#define _VAL_LOG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <linux/kernel.h>
//                       
#include <linux/printk.h>


#define MFV_LOG_ERROR   //     
#ifdef MFV_LOG_ERROR
#define MFV_LOGE(...) pr_err(__VA_ARGS__);
#else
#define MFV_LOGE(...)
#endif

#define MFV_LOG_WARNING //       
#ifdef MFV_LOG_WARNING
#define MFV_LOGW(...) pr_warning(__VA_ARGS__);
#else
#define MFV_LOGW(...)
#endif


#define MFV_LOG_DEBUG   //                 
#ifdef MFV_LOG_DEBUG
#define MFV_LOGD(...) pr_debug(__VA_ARGS__);
#else
#define MFV_LOGD(...)
#endif

#ifdef __cplusplus
}
#endif

#endif //                    
