/*       
                                            

                                                                                           
*/



#ifndef _WMT_CTRL_H_
#define _WMT_CTRL_H_

#include "osal.h"

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
	UINT32 ctrlId;
	size_t au4CtrlData[DWCNT_CTRL_DATA];
} WMT_CTRL_DATA, *P_WMT_CTRL_DATA;

typedef enum _ENUM_WMT_CTRL_T {
	WMT_CTRL_HW_PWR_OFF = 0,	/*                      */
	WMT_CTRL_HW_PWR_ON = 1,	/*                     */
	WMT_CTRL_HW_RST = 2,	/*                     */
	WMT_CTRL_STP_CLOSE = 3,
	WMT_CTRL_STP_OPEN = 4,
	WMT_CTRL_STP_CONF = 5,
	WMT_CTRL_FREE_PATCH = 6,
	WMT_CTRL_GET_PATCH = 7,
	WMT_CTRL_GET_PATCH_NAME = 8,
	WMT_CTRL_HOST_BAUDRATE_SET = 9,
	WMT_CTRL_SDIO_HW = 10,	/*                                      */
	WMT_CTRL_SDIO_FUNC = 11,	/*                                                        */
	WMT_CTRL_HWIDVER_SET = 12,	/*                                                                           */
	WMT_CTRL_HWVER_GET = 13,	/*                                                  */
	WMT_CTRL_STP_RST = 14,
	WMT_CTRL_GET_WMT_CONF = 15,
	WMT_CTRL_TX = 16,	/*                                                             */
	WMT_CTRL_RX = 17,	/*                                                             */
	WMT_CTRL_RX_FLUSH = 18,	/*                                                            */
	WMT_CTRL_GPS_SYNC_SET = 19,
	WMT_CTRL_GPS_LNA_SET = 20,
	WMT_CTRL_PATCH_SEARCH = 21,
	WMT_CTRL_CRYSTAL_TRIMING_GET = 22,
	WMT_CTRL_CRYSTAL_TRIMING_PUT = 23,
	WMT_CTRL_HW_STATE_DUMP = 24,
	WMT_CTRL_GET_PATCH_NUM = 25,
	WMT_CTRL_GET_PATCH_INFO = 26,
	WMT_CTRL_SOC_PALDO_CTRL = 27,
	WMT_CTRL_SOC_WAKEUP_CONSYS = 28,
	WMT_CTRL_SET_STP_DBG_INFO = 29,
	WMT_CTRL_BGW_DESENSE_CTRL = 30,
	WMT_CTRL_EVT_ERR_TRG_ASSERT = 31,
	WMT_CTRL_MAX
} ENUM_WMT_CTRL_T, *P_ENUM_WMT_CTRL_T;

typedef INT32(*WMT_CTRL_FUNC) (P_WMT_CTRL_DATA);

/*                                                                              
                                                  
                                                                                
*/

/*                                                                              
                                                   
                                                                                
*/





/*                                                                              
                                                            
                                                                                
*/

extern INT32 wmt_ctrl(P_WMT_CTRL_DATA pWmtCtrlData);

extern INT32
wmt_ctrl_tx_ex(const PUINT8 pData,
	       const UINT32 size, PUINT32 writtenSize, const MTK_WCN_BOOL bRawFlag);


/*                                                                              
                                                
                                                                                
*/



#endif				/*              */
