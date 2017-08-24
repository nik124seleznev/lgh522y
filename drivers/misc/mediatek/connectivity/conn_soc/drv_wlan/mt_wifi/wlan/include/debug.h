/*
                                                                                 
*/

/*                 
                                             

                                                                                
                     
*/



/*
                  
  
                   
                                                                                          
                                                    
  
                   
                                                                                          
                                       
  
                   
                                                                                          
                                
  
                   
                                                                                          
                                            
  
                   
                                                                                          
                                   
  
                   
                                                                                          
                            
  
                      
                                                                                            
    
  
                          
                                                             
                  
                                                                        
                                                                   
                                                                             
                                                       
  
                   
                                                                                                                
    
  
                   
       
                                                              
  
                   
  
                        
  
                   
  
                                                                               
  
                        
                                        
                                 
  
                        
                                                    
                                     
  
                   
                                                    
                                             
  
                   
                                                    
             
  
                   
                                                    
                      
                                                                      
                                                           
                                         
                                          
  
                   
                                                    
                                          
  
                         
                                                
                                   
  
                   
                                                          
                                                         
  
                   
                                                          
                               
                                                                                
                                                                           
                    
                                                                           
                                                       
                                                                           
                    
  
*/

#ifndef _DEBUG_H
#define _DEBUG_H

/*                                                                              
                                                     
                                                                                
*/
#ifndef BUILD_QA_DBG
#define BUILD_QA_DBG 0
#endif

/*                                                                              
                                                          
                                                                                
*/
#include "gl_typedef.h"
#if CFG_SUPPORT_XLOG
#include "linux/xlog.h"
#endif

extern UINT_8   aucDebugModule[];
extern UINT_32  u4DebugModule;

/*                                                                              
                                                
                                                                                
*/
/*                               
                                                                              
 */
#define DBG_CLASS_ERROR         BIT(0)
#define DBG_CLASS_WARN          BIT(1)
#define DBG_CLASS_STATE         BIT(2)
#define DBG_CLASS_EVENT         BIT(3)
#define DBG_CLASS_TRACE         BIT(4)
#define DBG_CLASS_INFO          BIT(5)
#define DBG_CLASS_LOUD          BIT(6)
#define DBG_CLASS_TEMP          BIT(7)
#define DBG_CLASS_MASK          BITS(0,7)


#if defined(LINUX)
#define DBG_PRINTF_64BIT_DEC    "lld"

#else  //       
#define DBG_PRINTF_64BIT_DEC    "I64d"

#endif
/*                                                                              
                                                 
                                                                                
*/
/*                           */
typedef enum _ENUM_DBG_MODULE_T {
    DBG_INIT_IDX = 0,       /*                    */
    DBG_HAL_IDX,            /*                   */
    DBG_INTR_IDX,           /*               */
    DBG_REQ_IDX,
    DBG_TX_IDX,
    DBG_RX_IDX,
    DBG_RFTEST_IDX,         /*                 */
    DBG_EMU_IDX,            /*                    */

    DBG_SW1_IDX,            /*                    */
    DBG_SW2_IDX,            /*                    */
    DBG_SW3_IDX,            /*                    */
    DBG_SW4_IDX,            /*                    */

    DBG_HEM_IDX,            /*     */
    DBG_AIS_IDX,            /*     */
    DBG_RLM_IDX,            /*     */
    DBG_MEM_IDX,            /*     */
    DBG_CNM_IDX,            /*     */
    DBG_RSN_IDX,            /*     */
    DBG_BSS_IDX,            /*     */
    DBG_SCN_IDX,            /*     */
    DBG_SAA_IDX,            /*     */
    DBG_AAA_IDX,            /*     */
    DBG_P2P_IDX,            /*     */
    DBG_QM_IDX,             /*         */
    DBG_SEC_IDX,            /*     */
    DBG_BOW_IDX,            /*     */
    DBG_WAPI_IDX,           /*      */
    DBG_ROAMING_IDX,        /*         */
	DBG_TDLS_IDX,			/*      */ /*                  */

    DBG_MODULE_NUM          /*                       */
} ENUM_DBG_MODULE_T;

//    
//                                                      
//                                                
//                                                                            
//      

/*                                          */
#define DBG_YARCO_IDX           DBG_SW1_IDX
#define DBG_KEVIN_IDX           DBG_SW2_IDX
#define DBG_CMC_IDX             DBG_SW3_IDX
#define DBG_GEORGE_IDX          DBG_SW4_IDX

/*                                                                              
                                                  
                                                                                
*/

/*                                                                              
                                                   
                                                                                
*/

/*                                                                              
                                             
                                                                                
*/
/*                                                  */
#define OS_SYSTIME_DBG_FORMAT               "0x%08x"

/*                                             */
#define OS_SYSTIME_DBG_ARGUMENT(systime)    (systime)

/*                                               */
#define MACSTR          "%02x:%02x:%02x:%02x:%02x:%02x"

/*                                          */
#define MAC2STR(a)      ((PUINT_8)a)[0], ((PUINT_8)a)[1], ((PUINT_8)a)[2], \
                        ((PUINT_8)a)[3], ((PUINT_8)a)[4], ((PUINT_8)a)[5]

/*                                                                             */
#define DUMPVAR(variable, format)           (#variable " = " format "\n", variable)

/*                                                                        */
#define DUMPMACADDR(addr)                   (#addr " = " MACSTR "\n", MAC2STR(addr))


/*                                                                    
                                                                          
                                                   
  
                                                           
 */
#define LOG_FUNC_TIME           kalPrint
#define LOG_FUNC                kalPrint

#if DBG

    #define TMP_BUF_LEN   256
    #define TMP_WBUF_LEN  (TMP_BUF_LEN * 2)

    extern PINT_16 g_wbuf_p;
    extern PINT_8 g_buf_p;

    /*                                                                 */
    #if defined(__FUNCTION__)
        #define DEBUGFUNC(_Func)
    #else
        #define DEBUGFUNC(_Func) static const char __FUNCTION__[] = _Func;
    #endif

    /*                                                         */
    /*                                                                              */
    #define INITLOG(_Fmt) \
    { \
        if (aucDebugModule[DBG_INIT_IDX] & DBG_CLASS_TRACE) { \
            LOG_FUNC("%s: ", __FUNCTION__); \
            LOG_FUNC _Fmt; \
        } \
    }

    #define ERRORLOG(_Fmt) \
    { \
        if (aucDebugModule[DBG_INIT_IDX] & DBG_CLASS_ERROR) { \
            LOG_FUNC("**Error[%s:%d]-", __FILE__, __LINE__); \
            LOG_FUNC _Fmt; \
        } \
    }

    #define WARNLOG(_Fmt) \
    { \
        if (aucDebugModule[DBG_INIT_IDX] & DBG_CLASS_WARN) { \
            LOG_FUNC("**Warning[%s:%d]-", __FILE__, __LINE__); \
            LOG_FUNC _Fmt; \
        } \
    }

    /*                                        */
    #define DBGLOG(_Module, _Class, _Fmt) \
        { \
            if (aucDebugModule[DBG_##_Module##_IDX] & DBG_CLASS_##_Class) { \
                if (DBG_CLASS_##_Class == DBG_CLASS_ERROR) { \
                    LOG_FUNC_TIME("[wlan] **Error[%s:%d]-", __FILE__, __LINE__); \
                    LOG_FUNC("[wlan] %s: (" #_Module " " #_Class ") ", __FUNCTION__); \
                } \
                else if (DBG_CLASS_##_Class == DBG_CLASS_WARN) { \
                    LOG_FUNC_TIME("[wlan] **Warning[%s:%d]-", __FILE__, __LINE__); \
                    LOG_FUNC("[wlan] %s: (" #_Module " " #_Class ") ", __FUNCTION__); \
                } \
                else { \
                    LOG_FUNC("[wlan] %s: (" #_Module " " #_Class ") ", __FUNCTION__); \
                } \
                LOG_FUNC _Fmt; \
            } \
        }

    #define DBGLOG_MEM8(_Module, _Class, _StartAddr, _Length) \
        { \
            if (aucDebugModule[DBG_##_Module##_IDX] & DBG_CLASS_##_Class) { \
                LOG_FUNC("%s: (" #_Module " " #_Class ")\n", __FUNCTION__); \
                dumpMemory8((PUINT_8) (_StartAddr), (UINT_32) (_Length)); \
            } \
        }

    #define DBGLOG_MEM32(_Module, _Class, _StartAddr, _Length) \
        { \
            if (aucDebugModule[DBG_##_Module##_IDX] & DBG_CLASS_##_Class) { \
                LOG_FUNC("%s: (" #_Module " " #_Class ")\n", __FUNCTION__); \
                dumpMemory32((PUINT_32) (_StartAddr), (UINT_32) (_Length)); \
            } \
        }
    /*              */

    /*                                                */
    #undef ASSERT
    /*              */

    #ifdef _lint
    #define ASSERT(_exp) \
        { \
            if (!(_exp)) {do {} while (1);} \
        }
    #else
    #define ASSERT(_exp) \
        { \
            if (!(_exp) && !fgIsBusAccessFailed) { \
                LOG_FUNC("Assertion failed: %s:%d %s\n", __FILE__, __LINE__, #_exp); \
                kalBreakPoint(); \
            } \
        }
    #endif /*       */

    #define ASSERT_REPORT(_exp, _fmt) \
        { \
            if (!(_exp) && !fgIsBusAccessFailed) { \
                LOG_FUNC("Assertion failed: %s:%d %s\n", __FILE__, __LINE__, #_exp); \
                LOG_FUNC _fmt; \
                kalBreakPoint(); \
            } \
        }

    #define DISP_STRING(_str)       _str

#else /*      */

    #define DEBUGFUNC(_Func)
    #define INITLOG(_Fmt)
    #define ERRORLOG(_Fmt)
    #define WARNLOG(_Fmt)

#if defined(LINUX)
    #define DBGLOG(_Module, _Class, _Fmt) \
    { \
        if (aucDebugModule[DBG_##_Module##_IDX] & DBG_CLASS_##_Class) { \
            if (DBG_CLASS_##_Class == DBG_CLASS_ERROR) { \
                LOG_FUNC_TIME("[wlan] **Error[%s:%d]-", __FILE__, __LINE__); \
                LOG_FUNC("[wlan] %s: (" #_Module " " #_Class ") ", __FUNCTION__); \
            } \
            else if (DBG_CLASS_##_Class == DBG_CLASS_WARN) { \
                LOG_FUNC_TIME("[wlan] **Warning[%s:%d]-", __FILE__, __LINE__); \
                LOG_FUNC("[wlan] %s: (" #_Module " " #_Class ") ", __FUNCTION__); \
            } \
            else { \
                LOG_FUNC("[wlan] %s: (" #_Module " " #_Class ") ", __FUNCTION__); \
            } \
            LOG_FUNC _Fmt; \
        } \
    }
#else
    #define DBGLOG(_Module, _Class, _Fmt)
#endif

#if CFG_SUPPORT_XLOG
    #define DBGLOG_MEM8(_Module, _Class, _StartAddr, _Length) \
    { \
        _Module##_##_Class##_LOGFUNC (__FUNCTION__);\
        _Module##_##_Class##_LOGDUMP8(_StartAddr, _Length); \
    }
#else
    #define DBGLOG_MEM8(_Module, _Class, _StartAddr, _Length)
#endif
    #define DBGLOG_MEM32(_Module, _Class, _StartAddr, _Length)

    #undef ASSERT

#if BUILD_QA_DBG
    #if defined(LINUX) /*                                */
        #define ASSERT(_exp) \
            { \
                if (!(_exp) && !fgIsBusAccessFailed) { \
                    LOG_FUNC("Assertion failed: %s:%d (%s)\n", __FILE__, __LINE__, #_exp); \
                    kalBreakPoint(); \
                } \
            }

        #define ASSERT_REPORT(_exp, _fmt) \
            { \
                if (!(_exp) && !fgIsBusAccessFailed) { \
                    LOG_FUNC("Assertion failed: %s:%d (%s)\n", __FILE__, __LINE__, #_exp); \
                    LOG_FUNC _fmt; \
                    kalBreakPoint(); \
                } \
            }
    #else
        #ifdef WINDOWS_CE
            #define UNICODE_TEXT(_msg)  TEXT(_msg)
            #define ASSERT(_exp) \
                { \
                    if (!(_exp) && !fgIsBusAccessFailed) { \
                        TCHAR rUbuf[256]; \
                        kalBreakPoint(); \
                        _stprintf(rUbuf, TEXT("Assertion failed: %s:%d %s\n"), \
                            UNICODE_TEXT(__FILE__), \
                            __LINE__, \
                            UNICODE_TEXT(#_exp)); \
                        MessageBox(NULL, rUbuf, TEXT("ASSERT!"), MB_OK); \
                    } \
                }

            #define ASSERT_REPORT(_exp, _fmt) \
                { \
                    if (!(_exp) && !fgIsBusAccessFailed) { \
                        TCHAR rUbuf[256]; \
                        kalBreakPoint(); \
                        _stprintf(rUbuf, TEXT("Assertion failed: %s:%d %s\n"), \
                            UNICODE_TEXT(__FILE__), \
                            __LINE__, \
                            UNICODE_TEXT(#_exp)); \
                        MessageBox(NULL, rUbuf, TEXT("ASSERT!"), MB_OK); \
                    } \
                }
        #else
            #define ASSERT(_exp) \
                { \
                    if (!(_exp) && !fgIsBusAccessFailed) { \
                        kalBreakPoint(); \
                    } \
                }

            #define ASSERT_REPORT(_exp, _fmt) \
                { \
                    if (!(_exp) && !fgIsBusAccessFailed) { \
                        kalBreakPoint(); \
                    } \
                }
        #endif /*            */
    #endif /*       */
#elif CFG_SUPPORT_XLOG
    #define ASSERT(_exp) \
        { \
            if (!(_exp) && !fgIsBusAccessFailed) { \
                XLOG_FUNC(ANDROID_LOG_DEBUG, "Warning at %s:%d (%s)\n", __FUNCTION__, __LINE__, #_exp); \
            } \
        }

    #define ASSERT_REPORT(_exp, _fmt) \
        { \
            if (!(_exp) && !fgIsBusAccessFailed) { \
                XLOG_FUNC(ANDROID_LOG_DEBUG, "Warning at %s:%d (%s)\n", __FUNCTION__, __LINE__, #_exp); \
                XLOG_FUNC(ANDROID_LOG_DEBUG, _fmt); \
            } \
        }
#else
    #define ASSERT(_exp) \
        { \
            if (!(_exp) && !fgIsBusAccessFailed) { \
                LOG_FUNC("Warning at %s:%d (%s)\n", __FUNCTION__, __LINE__, #_exp); \
            } \
        }

    #define ASSERT_REPORT(_exp, _fmt) \
        { \
            if (!(_exp) && !fgIsBusAccessFailed) { \
                LOG_FUNC("Warning at %s:%d (%s)\n", __FUNCTION__, __LINE__, #_exp); \
                LOG_FUNC _fmt; \
            } \
        }
#endif /*              */

    #define DISP_STRING(_str)       ""

#endif /*     */

#if CFG_STARTUP_DEBUG
    #if defined(LINUX)
#define DBGPRINTF kalPrint
    #else
#define DBGPRINTF DbgPrint
    #endif
#else
#define DBGPRINTF(...)
#endif


/*                                                              */
#ifndef DATA_STRUC_INSPECTING_ASSERT
#define DATA_STRUC_INSPECTING_ASSERT(expr) \
        switch (0) {case 0: case (expr): default:;}
#endif

/*                                                                              
                                                            
                                                                                
*/
#if DBG
VOID
dumpMemory8 (
    IN PUINT_8 pucStartAddr,
    IN UINT_32 u4Length
    );

VOID
dumpMemory32 (
    IN PUINT_32 pu4StartAddr,
    IN UINT_32  u4Length
    );
#elif CFG_SUPPORT_XLOG
VOID
dumpMemory8 (
    IN UINT_32  log_level,
    IN PUINT_8 pucStartAddr,
    IN UINT_32 u4Length
    );
#endif /*     */

/*                                                                              
                                                
                                                                                
*/
#endif /*          */

