#ifndef __CCCI_CFG_H__
#define __CCCI_CFG_H__

#include <ccci_ch.h>


/*                                                                 */
/*                                                                                             */
/*                                                                 */
//                                                                                   
#define CCCI_VERSION        "v1.4 20120618"


/*                                                                 */
/*                                                                                                */
/*                                                                 */
#define CCCI_MAX_CHANNEL    (100)


#define CCCI_WAKEUP_LOCK_NAME_LEN    (16)

#define MDLOGGER_FILE_PATH "/data/mdl/mdl_config"

#define IMG_INF_LEN            (256)

#define EE_BUF_LEN            (256)

#define EE_TIMER_BASE        (HZ)

#define CCCI_NODE_TYPE_NUM    (10)
#define NET_PORT_NUM        (4)

//                         
#define STD_CHR_DEV_NUM        CCCI_MAX_CH_NUM //                                                                   
#define IPC_DEV_NUM            (20)
#define FS_DEV_NUM            (10)
#define VIR_CHR_DEV_NUM        (10)
#define TTY_DEV_NUM            (10)
#define RPC_DEV_NUM            (10)

#define CCCI_MAX_VCHR_NUM    (10)
#define CCCI_VIR_CHR_KFIFO_SIZE        (32)


/*                                                                 */
/*                                                                                              */
/*                                                                 */
//                        

#endif//              
