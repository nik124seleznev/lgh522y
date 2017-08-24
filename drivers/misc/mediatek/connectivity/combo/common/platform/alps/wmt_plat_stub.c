/*       
                                            

                                                                                           
*/




/*                                                                              
                                                     
                                                                                
*/

/*                                                                              
                                             
                                                                                
*/


#if CONFIG_HAS_WAKELOCK
#include <linux/wakelock.h>
#define CFG_WMT_WAKELOCK_SUPPORT 1
#endif

#ifdef DFT_TAG
#undef DFT_TAG
#endif
#define DFT_TAG         "[WMT-PLAT]"


/*                                                                              
                                                          
                                                                                
*/

/*                             */
#include <mach/mtk_wcn_cmb_stub.h>
/*                            */
#include "wmt_plat.h"
#include "wmt_plat_stub.h"
#include "wmt_exp.h"
#include <mach/mtk_wcn_cmb_stub.h>
#include "wmt_lib.h"
#include "osal_typedef.h"

/*                                                                              
                                                
                                                                                
*/




/*                                                                              
                                                 
                                                                                
*/

/*                                                                              
                                                            
                                                                                
*/

static VOID wmt_plat_func_ctrl(UINT32 type, UINT32 on);



/*                                                                              
                                                  
                                                                                
*/


/*                                                                              
                                                   
                                                                                
*/


/*                                                                              
                                                
                                                                                
*/

static void wmt_plat_func_ctrl(unsigned int type, unsigned int on)
{
	if (on) {
		mtk_wcn_wmt_func_on((ENUM_WMTDRV_TYPE_T) type);
	} else {
		mtk_wcn_wmt_func_off((ENUM_WMTDRV_TYPE_T) type);
	}
	return;
}

static signed long wmt_plat_thremal_query(void)
{
	return wmt_lib_tm_temp_query();
}

#if MTK_WCN_CMB_FOR_SDIO_1V_AUTOK
static unsigned int wmt_plat_get_drv_status(unsigned int type)
{
    return wmt_lib_get_drv_status(type);
}
#endif

static INT32 wmt_plat_do_reset (UINT32 type)
{
    return wmt_lib_trigger_reset();
}

INT32 wmt_plat_stub_init(VOID)
{
	INT32 iRet = -1;
	CMB_STUB_CB stub_cb;
	stub_cb.aif_ctrl_cb = wmt_plat_audio_ctrl;
	stub_cb.func_ctrl_cb = wmt_plat_func_ctrl;
	stub_cb.thermal_query_cb = wmt_plat_thremal_query;
	stub_cb.wmt_do_reset_cb = wmt_plat_do_reset;
#if MTK_WCN_CMB_FOR_SDIO_1V_AUTOK
		stub_cb.get_drv_status_cb = wmt_plat_get_drv_status;
#endif
	stub_cb.size = sizeof(stub_cb);

	/*                      */
	iRet = mtk_wcn_cmb_stub_reg(&stub_cb);
	return iRet;
}
