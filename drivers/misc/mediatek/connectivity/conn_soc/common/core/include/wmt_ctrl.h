/*
* Copyright (C) 2011-2014 MediaTek Inc.
* 
* This program is free software: you can redistribute it and/or modify it under the terms of the 
* GNU General Public License version 2 as published by the Free Software Foundation.
* 
* This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
* without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
* See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with this program.
* If not, see <http://www.gnu.org/licenses/>.
*/

/*       
                                            

                                                                                           
*/



#ifndef _WMT_CTRL_H_
#define _WMT_CTRL_H_

#include "osal.h"
#include "wmt_stp_exp.h"
/*                                                                              
                                                     
                                                                                
*/

/*                                                                              
                                             
                                                                                
*/

#define DWCNT_CTRL_DATA  (16)


/*                                                                              
                                                          
                                                                                
*/






/*                                                                              
                                                
                                                                                
*/



/*                                                                              
                                                 
                                                                                
*/

typedef struct _WMT_CTRL_DATA_ {
    SIZE_T ctrlId;
    SIZE_T au4CtrlData[DWCNT_CTRL_DATA];
} WMT_CTRL_DATA, *P_WMT_CTRL_DATA;

typedef enum _ENUM_WMT_CTRL_T {
    WMT_CTRL_HW_PWR_OFF   = 0, /*                      */
    WMT_CTRL_HW_PWR_ON = 1, /*                     */
    WMT_CTRL_HW_RST = 2,/*               */
    WMT_CTRL_STP_CLOSE = 3,
    WMT_CTRL_STP_OPEN = 4,
    WMT_CTRL_STP_CONF = 5,
    WMT_CTRL_FREE_PATCH = 6,
    WMT_CTRL_GET_PATCH = 7,
    WMT_CTRL_GET_PATCH_NAME = 8,
    WMT_CTRL_HWIDVER_SET = 9, //                                                                          
    WMT_CTRL_STP_RST = 10,
    WMT_CTRL_GET_WMT_CONF = 11,
    WMT_CTRL_TX = 12, /*                                                             */
    WMT_CTRL_RX = 13, /*                                                             */
    WMT_CTRL_RX_FLUSH = 14, /*                                                            */
    WMT_CTRL_GPS_SYNC_SET = 15,
    WMT_CTRL_GPS_LNA_SET = 16,
    WMT_CTRL_PATCH_SEARCH = 17,
    WMT_CTRL_CRYSTAL_TRIMING_GET = 18,
    WMT_CTRL_CRYSTAL_TRIMING_PUT = 19,
    WMT_CTRL_HW_STATE_DUMP = 20,
    WMT_CTRL_GET_PATCH_NUM = 21,
    WMT_CTRL_GET_PATCH_INFO = 22,
    WMT_CTRL_SOC_PALDO_CTRL = 23,
    WMT_CTRL_SOC_WAKEUP_CONSYS = 24,
    WMT_CTRL_SET_STP_DBG_INFO = 25,
    WMT_CTRL_BGW_DESENSE_CTRL = 26,
    WMT_CTRL_EVT_ERR_TRG_ASSERT = 27,
#if CFG_WMT_LTE_COEX_HANDLING
    WMT_CTRL_GET_TDM_REQ_ANTSEL = 28,
#endif
    WMT_CTRL_MAX
} ENUM_WMT_CTRL_T, *P_ENUM_WMT_CTRL_T;

typedef INT32 (*WMT_CTRL_FUNC)(P_WMT_CTRL_DATA);

/*                                                                              
                                                  
                                                                                
*/

/*                                                                              
                                                   
                                                                                
*/





/*                                                                              
                                                            
                                                                                
*/

extern INT32
wmt_ctrl (
    P_WMT_CTRL_DATA pWmtCtrlData
    );

extern INT32
wmt_ctrl_tx_ex (
    const PUINT8 pData,
    const UINT32 size,
    PUINT32 writtenSize,
    const MTK_WCN_BOOL bRawFlag
    );


/*                                                                              
                                                
                                                                                
*/



#endif /*              */








