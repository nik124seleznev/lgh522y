/*       
                                            

                                                                                           
*/



#ifndef _WMT_FUNC_H_
#define _WMT_FUNC_H_

#include "wmt_core.h"
#include "wmt_plat.h"
#include "osal.h"
/*                                                                              
                                                     
                                                                                
*/

/*                                                                              
                                             
                                                                                
*/

#if 1				/*                                                                      */
#define CFG_FUNC_BT_SUPPORT 1
#else
#define CFG_FUNC_BT_SUPPORT 0
#endif


#if 1				/*                              */
#define CFG_FUNC_FM_SUPPORT 1
#else
#define CFG_FUNC_FM_SUPPORT 0
#endif

#if 1				/*                               */
#define CFG_FUNC_GPS_SUPPORT 1
#else
#define CFG_FUNC_GPS_SUPPORT 0
#endif

#if 1				/*                                */
#define CFG_FUNC_WIFI_SUPPORT 1
#else
#define CFG_FUNC_WIFI_SUPPORT 0
#endif

#if 1
#define CFG_FUNC_ANT_SUPPORT 1
#else
#define CFG_FUNC_ANT_SUPPORT 0
#endif


/*                                                                              
                                                          
                                                                                
*/


/*                                                                              
                                                
                                                                                
*/


/*                                                                              
                                                 
                                                                                
*/

typedef INT32(*SUBSYS_FUNC_ON) (P_WMT_IC_OPS pOps, P_WMT_GEN_CONF pConf);
typedef INT32(*SUBSYS_FUNC_OFF) (P_WMT_IC_OPS pOps, P_WMT_GEN_CONF pConf);

typedef struct _WMT_FUNC_OPS_ {
	SUBSYS_FUNC_ON func_on;
	SUBSYS_FUNC_OFF func_off;
} WMT_FUNC_OPS, *P_WMT_FUNC_OPS;

typedef struct _CMB_PIN_CTRL_REG_ {
	UINT32 regAddr;
	UINT32 regValue;
	UINT32 regMask;

} CMB_PIN_CTRL_REG, *P_CMB_PIN_CTRL_REG;

typedef struct _CMB_PIN_CTRL_ {
	UINT32 pinId;
	UINT32 regNum;
	P_CMB_PIN_CTRL_REG pFuncOnArray;
	P_CMB_PIN_CTRL_REG pFuncOffArray;

} CMB_PIN_CTRL, *P_CMB_PIN_CTRL;

typedef enum _ENUM_CMP_PIN_ID_ {
	CMB_PIN_EEDI_ID = 0,
	CMB_PIN_EEDO_ID = 1,
	CMB_PIN_GSYNC_ID = 2,
} ENUM_CMP_PIN_ID, *P_ENUM_CMP_PIN_ID;

/*                                                                              
                                                  
                                                                                
*/


/*                                                                              
                                                   
                                                                                
*/

/*                                                                              
                                                            
                                                                                
*/


/*                                                                              
                                                
                                                                                
*/







#endif				/*              */
