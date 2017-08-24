#ifndef _SBCHK_BASE_H
#define _SBCHK_BASE_H

#ifndef FALSE
  #define FALSE (0)
#endif

#ifndef TRUE
  #define TRUE  (1)
#endif

/*                                                                         
                      
                                                                          */
#define HASH_OUTPUT_LEN                     (20)

/*                                                                         
              
                                                                          */
#define SBCHK_ENGINE_PATH                   "/system/bin/sbchk"
#define SBCHK_MODULE_PATH                   "/system/lib/modules/sec.ko"
#define MODEM_CORE_MODULE_PATH              "/system/lib/modules/ccci.ko"
#define MODEM_PLAT_MODULE_PATH              "/system/lib/modules/ccci_plat.ko"
#define INIT_RC_PATH                        "init.rc"

/*                                                                         
               
                                                                          */
#define SBCHK_BASE_HASH_CHECK               FALSE

/*                                                                         
                    
                                                                          */
#if SBCHK_BASE_HASH_CHECK

    #error MUST fill the hash value of '/system/bin/sbchk', 
    'init.rc', '/system/lib/modules/sec.ko', '/system/lib/modules/ccci.ko' and 
    '/system/lib/modules/ccci_plat.ko'

    /* 
                                                   
                                                      
                                                   
                                                       
       
                                                
                                                          
                                                                         
                                                                            
                                                                             
                                                                     
                                                      
                                                                                     
                                                                                      
                                                                     
                                                   
                                                                                      
                                                                                       
                                                                     
                                                                   
                                                                                            
                                                                                            
                                                                     
                                                                               
                                                                   
                                                                   
                                                                     
                                                 

                              
                                                                                                                     
                                                                                                    
    */

    //                                                                    
    //                                                                    
    #define SBCHK_ENGINE_HASH ?????????????????????????????????????????
    #define SBCHK_MODULE_HASH ?????????????????????????????????????????
    #define MODEM_PLAT_MODULE_HASH ?????????????????????????????????????????    
    #define MODEM_CORE_MODULE_HASH ?????????????????????????????????????????    
    #define INIT_RC_HASH ?????????????????????????????????????????        

#else

    /*       */
    //                                                                    
    #define SBCHK_ENGINE_HASH "0000000000000000000000000000000000000000"
    #define SBCHK_MODULE_HASH "0000000000000000000000000000000000000000"
    #define MODEM_PLAT_MODULE_HASH "0000000000000000000000000000000000000000"    
    #define MODEM_CORE_MODULE_HASH "0000000000000000000000000000000000000000"    
    #define INIT_RC_HASH "0000000000000000000000000000000000000000"        

#endif  

/*                                                                         
              
                                                                          */
#define SEC_OK                              (0x0000)
#define SBCHK_BASE_OPEN_FAIL                (0x1000)
#define SBCHK_BASE_READ_FAIL                (0x1001)
#define SBCHK_BASE_HASH_INIT_FAIL           (0x1002)
#define SBCHK_BASE_HASH_DATA_FAIL           (0x1003)
#define SBCHK_BASE_HASH_CHECK_FAIL          (0x1004)
#define SBCHK_BASE_INDEX_OUT_OF_RANGE       (0xFFFFFFFF)

/*                                                                         
                     
                                                                          */
extern void sbchk_base(void);


/*                                                                             
              
                                                                              */


#endif   /*           */
