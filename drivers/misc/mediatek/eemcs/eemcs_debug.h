/*                                                                              
                        
                                                                               */

#ifndef __EEMCS_DEBUG_H__
#define __EEMCS_DEBUG_H__

#include "eemcs_kal.h"

#define EEMCS_DEBUG
#define LOG_FUNC(a...) printk(KERN_ERR a)

/*                                                                              
                                                 
  
                                  
                                                                                         
  
                                                                               */
#define DBG_LEVEL_ERR          BIT(0)
#define DBG_LEVEL_WAR          BIT(1)
#define DBG_LEVEL_STA          BIT(2)
#define DBG_LEVEL_TRA          BIT(3)
#define DBG_LEVEL_INF          BIT(4)
#define DBG_LEVEL_DEF          BIT(5)
#define DBG_LEVEL_REV          BIT(6)
#define DBG_LEVEL_DBG          BIT(7)
#define DBG_LEVEL_MASK         BITS(0,7)

/*                                                                              
                                                
 
                              
 
                                                                               */

typedef enum _ENUM_DBG_MODULE_T {
    DBG_INIT_IDX = 0,         /*                          */
    DBG_MSDC_IDX,             /*                         */
    DBG_SDIO_IDX,             /*                    */
    DBG_CCCI_IDX,             /*                            */
    DBG_NETD_IDX,             /*                */
    DBG_CHAR_IDX,             /*                      */
    DBG_FUNC_IDX,             /*                                  */
    DBG_IPCD_IDX,             /*                          */
    DBG_BOOT_IDX,             /*           */
    DBG_FSUT_IDX,             /*          */
    DBG_RPCD_IDX,             /*                */
    DBG_EXPT_IDX,             /*                       */
    DBG_SMSG_IDX,             /*                           */
    DBG_PORE_IDX,             /*                */
    DBG_SYSF_IDX,             /*             */
    DBG_MODULE_NUM
} ENUM_DBG_MODULE_T;

extern KAL_UINT64 g_eemcs_dbg_m[DBG_MODULE_NUM];

#define FUNC_NAME __FUNCTION__


#if defined(EEMCS_DEBUG)   
#define DBGLOG(_mod, _level, _fmt, a...) \
    do{ \
        if (g_eemcs_dbg_m[DBG_##_mod##_IDX] & DBG_LEVEL_##_level) { \
            if (DBG_LEVEL_##_level == DBG_LEVEL_ERR) { \
                LOG_FUNC("[EEMCS/"#_mod"] [Error:%d]%s:"_fmt"\n",\
                         __LINE__,__FUNCTION__,##a); \
            } \
            else if (DBG_LEVEL_##_level == DBG_LEVEL_WAR) { \
                LOG_FUNC("[EEMCS/"#_mod"] [Warn:%d]%s:"_fmt"\n",\
                         __LINE__,__FUNCTION__,##a); \
            } \
            else { \
                LOG_FUNC("[EEMCS/"#_mod"] "_fmt"\n",##a); \
            } \
        } \
    }while(0)

#define CDEV_LOG(_port_id, _mod, _level, _fmt, a...) \
    do{ \
        if (g_eemcs_dbg_m[DBG_PORE_IDX] & (1 << _port_id)) { \
                LOG_FUNC("[EEMCS/"#_mod"] "_fmt"\n",##a); \
        } \
    }while(0)
#else
#define DBGLOG(_mod, _level, _fmt, a...)
#define CDEV_LOG(_port_id, _mod, _level, _fmt, a...)   
#endif

#define DEBUG_LOG_FUNCTION_ENTRY \
do{   \
    DBGLOG(FUNC,DBG,"[EEMCS]==> %s",__FUNCTION__);  \
}while(0)

#define DEBUG_LOG_FUNCTION_LEAVE \
do{ \
    DBGLOG(FUNC,DBG,"[EEMCS]<== %s",__FUNCTION__); \
}while(0)

/*                                                                              
                                    
                                                                               */

KAL_INT32 eemcs_debug_mod_init(void);
void eemcs_debug_deinit(void);

#endif //                 
