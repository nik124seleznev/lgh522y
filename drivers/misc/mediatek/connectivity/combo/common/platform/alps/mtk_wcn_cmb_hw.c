/*       
                                            

                                                                                           
*/




/*                                                                              
                                                     
                                                                                
*/

/*                                                                              
                                             
                                                                                
*/
#ifdef DFT_TAG
#undef DFT_TAG
#endif
#define DFT_TAG "[WMT-CMB-HW]"


/*                                                                              
                                                          
                                                                                
*/

#include "wmt_plat.h"
#include "wmt_lib.h"
#include "mtk_wcn_cmb_hw.h"
#include "osal_typedef.h"


/*                                                                              
                                                
                                                                                
*/
#define DFT_RTC_STABLE_TIME 100
#define DFT_LDO_STABLE_TIME 100
#define DFT_RST_STABLE_TIME 30
#define DFT_OFF_STABLE_TIME 10
#define DFT_ON_STABLE_TIME 30

#define MT6630_SW_STRAP_SUPPORT

/*                                                                              
                                                 
                                                                                
*/

/*                                                                              
                                                  
                                                                                
*/



/*                                                                              
                                                   
                                                                                
*/

PWR_SEQ_TIME gPwrSeqTime;




/*                                                                              
                                                            
                                                                                
*/



/*                                                                              
                                                
                                                                                
*/

INT32 mtk_wcn_cmb_hw_pwr_off(VOID)
{
	INT32 iRet = 0;
	WMT_INFO_FUNC("CMB-HW, hw_pwr_off start\n");

	/*                                                                 */
	/*                                      */

	/*                                                                 */
	iRet += wmt_plat_eirq_ctrl(PIN_BGF_EINT, PIN_STA_EINT_DIS);
	WMT_INFO_FUNC("CMB-HW, BGF_EINT IRQ unregistered and disabled\n");
	iRet += wmt_plat_gpio_ctrl(PIN_BGF_EINT, PIN_STA_DEINIT);
	/*                                                                     */
	iRet += wmt_plat_eirq_ctrl(PIN_ALL_EINT, PIN_STA_DEINIT);
	WMT_INFO_FUNC("CMB-HW, ALL_EINT IRQ unregistered and disabled\n");
	iRet += wmt_plat_gpio_ctrl(PIN_ALL_EINT, PIN_STA_DEINIT);
	/*                     */
	iRet += wmt_plat_gpio_ctrl(PIN_GPS_SYNC, PIN_STA_DEINIT);

	/*                                                              */
	iRet += wmt_plat_audio_ctrl(CMB_STUB_AIF_0, CMB_STUB_AIF_CTRL_DIS);

	/*                                                              */
	iRet += wmt_plat_gpio_ctrl(PIN_SDIO_GRP, PIN_STA_DEINIT);
	iRet += wmt_plat_gpio_ctrl(PIN_RST, PIN_STA_OUT_L);
	iRet += wmt_plat_gpio_ctrl(PIN_PMU, PIN_STA_OUT_L);

	/*                                                            */
	iRet += wmt_plat_gpio_ctrl(PIN_UART_GRP, PIN_STA_DEINIT);

	/*                         */
	iRet += wmt_plat_gpio_ctrl(PIN_LDO, PIN_STA_OUT_L);

	/*                  */
	iRet += wmt_plat_gpio_ctrl(PIN_GPS_LNA, PIN_STA_DEINIT);

	WMT_INFO_FUNC("CMB-HW, hw_pwr_off finish\n");
	return iRet;
}

INT32 mtk_wcn_cmb_hw_pwr_on(VOID)
{
	static UINT32 _pwr_first_time = 1;
	INT32 iRet = 0;

	WMT_INFO_FUNC("CMB-HW, hw_pwr_on start\n");

	/*                           */
	iRet += wmt_plat_eirq_ctrl(PIN_BGF_EINT, PIN_STA_EINT_DIS);
	iRet += wmt_plat_eirq_ctrl(PIN_ALL_EINT, PIN_STA_EINT_DIS);

	/*                                                                   */
	iRet += wmt_plat_gpio_ctrl(PIN_LDO, PIN_STA_INIT);
	iRet += wmt_plat_gpio_ctrl(PIN_PMU, PIN_STA_INIT);
	iRet += wmt_plat_gpio_ctrl(PIN_RST, PIN_STA_INIT);
	iRet += wmt_plat_gpio_ctrl(PIN_SDIO_GRP, PIN_STA_INIT);
	iRet += wmt_plat_gpio_ctrl(PIN_BGF_EINT, PIN_STA_INIT);
	iRet += wmt_plat_gpio_ctrl(PIN_ALL_EINT, PIN_STA_INIT);
	iRet += wmt_plat_gpio_ctrl(PIN_GPS_SYNC, PIN_STA_INIT);
	iRet += wmt_plat_gpio_ctrl(PIN_GPS_LNA, PIN_STA_INIT);
	/*                                                  */ /*                                             */
	/*                                                        */

	/*                                         */
	iRet += wmt_plat_gpio_ctrl(PIN_LDO, PIN_STA_OUT_H);
	osal_sleep_ms(gPwrSeqTime.ldoStableTime);

	/*                             */
	if (_pwr_first_time) {
		/*                                                                            */
		iRet += wmt_plat_gpio_ctrl(PIN_RTC, PIN_STA_INIT);
		osal_sleep_ms(gPwrSeqTime.rtcStableTime);
		WMT_INFO_FUNC("CMB-HW, rtc clock exported\n");
	}

	/*                               */
	iRet += wmt_plat_gpio_ctrl(PIN_UART_GRP, PIN_STA_INIT);

#ifdef MT6630_SW_STRAP_SUPPORT
	switch (wmt_plat_get_comm_if_type()) {
	case STP_UART_IF_TX:
		iRet += wmt_plat_gpio_ctrl(PIN_UART_RX, PIN_STA_OUT_H);
		break;
	case STP_SDIO_IF_TX:
		iRet += wmt_plat_gpio_ctrl(PIN_UART_RX, PIN_STA_IN_NP);
		break;
	default:
		WMT_ERR_FUNC("not supported common interface\n");
		break;
	}
#endif
	/*                                                           */
	iRet += wmt_plat_gpio_ctrl(PIN_PMU, PIN_STA_OUT_L);
	iRet += wmt_plat_gpio_ctrl(PIN_RST, PIN_STA_OUT_L);
	osal_sleep_ms(gPwrSeqTime.offStableTime);

	/*                                           */
	iRet += wmt_plat_gpio_ctrl(PIN_PMU, PIN_STA_OUT_H);
	osal_sleep_ms(gPwrSeqTime.rstStableTime);

	/*                                          */
	iRet += wmt_plat_gpio_ctrl(PIN_RST, PIN_STA_OUT_H);
	osal_sleep_ms(gPwrSeqTime.onStableTime);

#ifdef MT6630_SW_STRAP_SUPPORT
	/*                            */
	iRet += wmt_plat_gpio_ctrl(PIN_UART_RX, PIN_STA_MUX);
#endif


	/*                                                             */
	/*                                                        */
	iRet += wmt_plat_audio_ctrl(CMB_STUB_AIF_1, CMB_STUB_AIF_CTRL_DIS);

	/*                                                                                                                           */
	/*                                                                    */
	iRet += wmt_plat_gpio_ctrl(PIN_BGF_EINT, PIN_STA_MUX);
	iRet += wmt_plat_eirq_ctrl(PIN_BGF_EINT, PIN_STA_INIT);
	WMT_INFO_FUNC("CMB-HW, BGF_EINT IRQ registered and disabled\n");

	/*                                                                     */
	iRet += wmt_plat_gpio_ctrl(PIN_ALL_EINT, PIN_STA_MUX);
	iRet += wmt_plat_eirq_ctrl(PIN_ALL_EINT, PIN_STA_INIT);
	WMT_INFO_FUNC("CMB-HW, hw_pwr_on finish (%d)\n", iRet);

	_pwr_first_time = 0;
	return iRet;

}

INT32 mtk_wcn_cmb_hw_rst(VOID)
{
	INT32 iRet = 0;
	WMT_INFO_FUNC("CMB-HW, hw_rst start, eirq should be disabled before this step\n");
#ifdef MT6630_SW_STRAP_SUPPORT
	switch (wmt_plat_get_comm_if_type()) {
	case STP_UART_IF_TX:
		iRet += wmt_plat_gpio_ctrl(PIN_UART_RX, PIN_STA_OUT_H);
		break;
	case STP_SDIO_IF_TX:
		iRet += wmt_plat_gpio_ctrl(PIN_UART_RX, PIN_STA_IN_NP);
		break;
	default:
		WMT_ERR_FUNC("not supported common interface\n");
		break;
	}
#endif

	/*                                                           */
	iRet += wmt_plat_gpio_ctrl(PIN_PMU, PIN_STA_OUT_L);
	iRet += wmt_plat_gpio_ctrl(PIN_RST, PIN_STA_OUT_L);
	osal_sleep_ms(gPwrSeqTime.offStableTime);

	/*                                           */
	iRet += wmt_plat_gpio_ctrl(PIN_PMU, PIN_STA_OUT_H);
	osal_sleep_ms(gPwrSeqTime.rstStableTime);

	/*                                          */
	iRet += wmt_plat_gpio_ctrl(PIN_RST, PIN_STA_OUT_H);
	osal_sleep_ms(gPwrSeqTime.onStableTime);

#ifdef MT6630_SW_STRAP_SUPPORT
	/*                            */
	iRet += wmt_plat_gpio_ctrl(PIN_UART_RX, PIN_STA_MUX);
#endif

	WMT_INFO_FUNC("CMB-HW, hw_rst finish, eirq should be enabled after this step\n");
	return 0;
}

static VOID mtk_wcn_cmb_hw_dmp_seq(VOID)
{
	PUINT32 pTimeSlot = (PUINT32) &gPwrSeqTime;
	WMT_INFO_FUNC("combo chip power on sequence time, RTC (%d), LDO (%d), RST(%d), OFF(%d), ON(%d)\n", pTimeSlot[0],
		      /*             */
		      pTimeSlot[1], pTimeSlot[2], pTimeSlot[3], pTimeSlot[4]
	    );
	return;
}

INT32 mtk_wcn_cmb_hw_state_show(VOID)
{
	wmt_plat_gpio_ctrl(PIN_PMU, PIN_STA_SHOW);
	wmt_plat_gpio_ctrl(PIN_RST, PIN_STA_SHOW);
	wmt_plat_gpio_ctrl(PIN_RTC, PIN_STA_SHOW);
	return 0;
}



INT32 mtk_wcn_cmb_hw_init(P_PWR_SEQ_TIME pPwrSeqTime)
{
	if (NULL != pPwrSeqTime &&
	    pPwrSeqTime->ldoStableTime > 0 &&
	    pPwrSeqTime->rtcStableTime > 0 &&
	    pPwrSeqTime->offStableTime > DFT_OFF_STABLE_TIME &&
	    pPwrSeqTime->onStableTime > DFT_ON_STABLE_TIME &&
	    pPwrSeqTime->rstStableTime > DFT_RST_STABLE_TIME) {
		/*                               */
		WMT_DBG_FUNC("setting hw init sequence parameters\n");
		osal_memcpy(&gPwrSeqTime, pPwrSeqTime, osal_sizeof(gPwrSeqTime));
	} else {
		WMT_WARN_FUNC
		    ("invalid pPwrSeqTime parameter, use default hw init sequence parameters\n");
		gPwrSeqTime.ldoStableTime = DFT_LDO_STABLE_TIME;
		gPwrSeqTime.offStableTime = DFT_OFF_STABLE_TIME;
		gPwrSeqTime.onStableTime = DFT_ON_STABLE_TIME;
		gPwrSeqTime.rstStableTime = DFT_RST_STABLE_TIME;
		gPwrSeqTime.rtcStableTime = DFT_RTC_STABLE_TIME;
	}
	mtk_wcn_cmb_hw_dmp_seq();
	return 0;
}

INT32 mtk_wcn_cmb_hw_deinit(VOID)
{

	WMT_WARN_FUNC("mtk_wcn_cmb_hw_deinit start, set to default hw init sequence parameters\n");
	gPwrSeqTime.ldoStableTime = DFT_LDO_STABLE_TIME;
	gPwrSeqTime.offStableTime = DFT_OFF_STABLE_TIME;
	gPwrSeqTime.onStableTime = DFT_ON_STABLE_TIME;
	gPwrSeqTime.rstStableTime = DFT_RST_STABLE_TIME;
	gPwrSeqTime.rtcStableTime = DFT_RTC_STABLE_TIME;
	WMT_WARN_FUNC("mtk_wcn_cmb_hw_deinit finish\n");
	return 0;
}
