/*
                                                                                           
*/

/*                  
                                                                  
                                             
*/




#ifndef _GL_RST_H
#define _GL_RST_H

/*                                                                              
                                                     
                                                                                
*/

/*                                                                              
                                                          
                                                                                
*/
#include "gl_typedef.h"

/*                                                                              
                                                
                                                                                
*/

/*                                                                              
                                                 
                                                                                
*/
#if 1
typedef INT_32(*wmt_wlan_probe_cb)(VOID);
typedef INT_32(*wmt_wlan_remove_cb)(VOID);
typedef INT_32(*wmt_wlan_bus_cnt_get_cb)(VOID);
typedef INT_32(*wmt_wlan_bus_cnt_clr_cb)(VOID);

typedef struct _MTK_WCN_WMT_WLAN_CB_INFO{
	wmt_wlan_probe_cb wlan_probe_cb;
	wmt_wlan_remove_cb wlan_remove_cb;
	wmt_wlan_bus_cnt_get_cb wlan_bus_cnt_get_cb;
	wmt_wlan_bus_cnt_clr_cb wlan_bus_cnt_clr_cb;
}MTK_WCN_WMT_WLAN_CB_INFO,*P_MTK_WCN_WMT_WLAN_CB_INFO;

extern INT_32 mtk_wcn_wmt_wlan_reg(P_MTK_WCN_WMT_WLAN_CB_INFO pWmtWlanCbInfo);
extern INT_32 mtk_wcn_wmt_wlan_unreg(VOID);
#endif

typedef enum _ENUM_RESET_STATUS_T {
    RESET_FAIL,
    RESET_SUCCESS
} ENUM_RESET_STATUS_T;

typedef struct _RESET_STRUCT_T {
    ENUM_RESET_STATUS_T rst_data;
    struct work_struct rst_work;
} RESET_STRUCT_T;

/*                                                       */
typedef enum _ENUM_WMTDRV_TYPE_T {
    WMTDRV_TYPE_BT = 0,
    WMTDRV_TYPE_FM = 1,
    WMTDRV_TYPE_GPS = 2,
    WMTDRV_TYPE_WIFI = 3,
    WMTDRV_TYPE_WMT = 4,
    WMTDRV_TYPE_STP = 5,
    WMTDRV_TYPE_SDIO1 = 6,
    WMTDRV_TYPE_SDIO2 = 7,
    WMTDRV_TYPE_LPBK = 8,
    WMTDRV_TYPE_MAX
} ENUM_WMTDRV_TYPE_T, *P_ENUM_WMTDRV_TYPE_T;

typedef enum _ENUM_WMTMSG_TYPE_T {
    WMTMSG_TYPE_POWER_ON = 0,
    WMTMSG_TYPE_POWER_OFF = 1,
    WMTMSG_TYPE_RESET = 2,
    WMTMSG_TYPE_STP_RDY= 3,
    WMTMSG_TYPE_HW_FUNC_ON= 4,
    WMTMSG_TYPE_MAX
} ENUM_WMTMSG_TYPE_T, *P_ENUM_WMTMSG_TYPE_T;

typedef enum _ENUM_WMTRSTMSG_TYPE_T{
    WMTRSTMSG_RESET_START = 0x0,
    WMTRSTMSG_RESET_END = 0x1,
    WMTRSTMSG_RESET_END_FAIL = 0x2,
    WMTRSTMSG_RESET_MAX,
    WMTRSTMSG_RESET_INVALID = 0xff
} ENUM_WMTRSTMSG_TYPE_T, *P_ENUM_WMTRSTMSG_TYPE_T;

typedef void (*PF_WMT_CB)(
    ENUM_WMTDRV_TYPE_T, /*                    */
    ENUM_WMTDRV_TYPE_T, /*                         */
    ENUM_WMTMSG_TYPE_T, /*              */
    void *, /*                                                                   
                                                                   */
    unsigned int /*                             */
    );


/*                                                                              
                                                        
                                                                                
*/

#if CFG_CHIP_RESET_SUPPORT
extern int mtk_wcn_wmt_msgcb_reg(ENUM_WMTDRV_TYPE_T eType, PF_WMT_CB pCb);
extern int mtk_wcn_wmt_msgcb_unreg(ENUM_WMTDRV_TYPE_T eType);
extern int wifi_reset_start(void);
extern int wifi_reset_end(ENUM_RESET_STATUS_T);
#endif

/*                                                                              
                                                  
                                                                                
*/

/*                                                                              
                                                   
                                                                                
*/

/*                                                                              
                                             
                                                                                
*/

/*                                                                              
                                                            
                                                                                
*/

/*                                                                              
                                                
                                                                                
*/

VOID
glResetInit(
    VOID
    );

VOID
glResetUninit(
    VOID
    );

VOID
glSendResetRequest(
    VOID
    );

BOOLEAN
kalIsResetting(
    VOID
    );


#endif /*           */
