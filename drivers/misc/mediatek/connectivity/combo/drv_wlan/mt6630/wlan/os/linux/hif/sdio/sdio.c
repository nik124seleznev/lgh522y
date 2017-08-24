/******************************************************************************
*[File]             sdio.c
*[Version]          v1.0
*[Revision Date]    2010-03-01
*[Author]
*[Description]
*    The program provides SDIO HIF driver
*[Copyright]
*    Copyright (C) 2010 MediaTek Incorporation. All Rights Reserved.
******************************************************************************/



/*
** $Log: sdio.c $
**
** 07 05 2013 terry.wu
** [BORA00002207] [MT6630 Wi-Fi] TXM & MQM Implementation
** 1. Avoid large packet Tx issue
**
** 02 01 2013 cp.wu
** [BORA00002227] [MT6630 Wi-Fi][Driver] Update for Makefile and HIFSYS modifications
** 1. eliminate MT5931/MT6620/MT6628 logic
** 2. add firmware download control sequence
**
** 11 21 2012 terry.wu
** [BORA00002207] [MT6630 Wi-Fi] TXM & MQM Implementation
** [Driver] Fix linux drvier build error.
**
** 09 17 2012 cm.chang
** [BORA00002149] [MT6630 Wi-Fi] Initial software development
** Duplicate source from MT6620 v2.3 driver branch
** (Davinci label: MT6620_WIFI_Driver_V2_3_120913_1942_As_MT6630_Base)
**
** 08 24 2012 cp.wu
** [WCXRP00001269] [MT6620 Wi-Fi][Driver] cfg80211 porting merge back to DaVinci
** .
**
** 08 24 2012 cp.wu
** [WCXRP00001269] [MT6620 Wi-Fi][Driver] cfg80211 porting merge back to DaVinci
** cfg80211 support merge back from ALPS.JB to DaVinci - MT6620 Driver v2.3 branch.
 *
 * 04 12 2012 terry.wu
 * NULL
 * Add AEE message support
 * 1) Show AEE warning(red screen) if SDIO access error occurs

 *
 * 02 14 2012 cp.wu
 * [WCXRP00000851] [MT6628 Wi-Fi][Driver] Add HIFSYS related definition to driver source tree
 * include correct header file upon setting.
 *
 * 11 10 2011 cp.wu
 * [WCXRP00001098] [MT6620 Wi-Fi][Driver] Replace printk by DBG LOG macros in linux porting layer
 * 1. eliminaite direct calls to printk in porting layer.
 * 2. replaced by DBGLOG, which would be XLOG on ALPS platforms.
 *
 * 09 20 2011 cp.wu
 * [WCXRP00000994] [MT6620 Wi-Fi][Driver] dump message for bus error and reset bus error flag while re-initialized
 * 1. always show error message for SDIO bus errors.
 * 2. reset bus error flag when re-initialization
 *
 * 08 17 2011 cp.wu
 * [WCXRP00000851] [MT6628 Wi-Fi][Driver] Add HIFSYS related definition to driver source tree
 * add MT6628 related definitions for Linux/Android driver.
 *
 * 05 18 2011 cp.wu
 * [WCXRP00000702] [MT5931][Driver] Modify initialization sequence for E1 ASIC
 * add device ID for MT5931.
 *
 * 04 08 2011 pat.lu
 * [WCXRP00000623] [MT6620 Wi-Fi][Driver] use ARCH define to distinguish PC Linux driver
 * Use CONFIG_X86 instead of PC_LINUX_DRIVER_USE option to have proper compile settting for PC Linux driver
 *
 * 03 22 2011 pat.lu
 * [WCXRP00000592] [MT6620 Wi-Fi][Driver] Support PC Linux Environment Driver Build
 * Add a compiler option "PC_LINUX_DRIVER_USE" for building driver in PC Linux environment.
 *
 * 03 18 2011 cp.wu
 * [WCXRP00000559] [MT6620 Wi-Fi][Driver] Combine TX/RX DMA buffers into a single one to reduce physically continuous memory consumption
 * deprecate CFG_HANDLE_IST_IN_SDIO_CALLBACK.
 *
 * 03 15 2011 cp.wu
 * [WCXRP00000559] [MT6620 Wi-Fi][Driver] Combine TX/RX DMA buffers into a single one to reduce physically continuous memory consumption
 * 1. deprecate CFG_HANDLE_IST_IN_SDIO_CALLBACK
 * 2. Use common coalescing buffer for both TX/RX directions
 *
 *
 * 03 07 2011 terry.wu
 * [WCXRP00000521] [MT6620 Wi-Fi][Driver] Remove non-standard debug message
 * Toggle non-standard debug messages to comments.
 *
 * 11 15 2010 jeffrey.chang
 * [WCXRP00000181] [MT6620 Wi-Fi][Driver] fix the driver message "GLUE_FLAG_HALT skip INT" during unloading
 * Fix GLUE_FALG_HALT message which cause driver to hang
 *
 * 11 08 2010 cp.wu
 * [WCXRP00000166] [MT6620 Wi-Fi][Driver] use SDIO CMD52 for enabling/disabling interrupt to reduce transaction period
 * correct typo
 *
 * 11 08 2010 cp.wu
 * [WCXRP00000166] [MT6620 Wi-Fi][Driver] use SDIO CMD52 for enabling/disabling interrupt to reduce transaction period
 * change to use CMD52 for enabling/disabling interrupt to reduce SDIO transaction time
 *
 * 11 01 2010 yarco.yang
 * [WCXRP00000149] [MT6620 WI-Fi][Driver]Fine tune performance on MT6516 platform
 * Add code to run WlanIST in SDIO callback.
 *
 * 10 19 2010 cp.wu
 * [WCXRP00000122] [MT6620 Wi-Fi][Driver] Preparation for YuSu source tree integration
 * remove HIF_SDIO_ONE flags because the settings could be merged for runtime detection instead of compile-time.
 *
 * 10 19 2010 jeffrey.chang
 * [WCXRP00000120] [MT6620 Wi-Fi][Driver] Refine linux kernel module to the license of MTK propietary and enable MTK HIF by default
 * Refine linux kernel module to the license of MTK and enable MTK HIF
 *
 * 08 21 2010 jeffrey.chang
 * NULL
 * 1) add sdio two setting
 * 2) bug fix of sdio glue
 *
 * 08 18 2010 jeffrey.chang
 * NULL
 * support multi-function sdio
 *
 * 08 18 2010 cp.wu
 * NULL
 * #if defined(__X86__) is not working, change to use #ifdef CONFIG_X86.
 *
 * 08 17 2010 cp.wu
 * NULL
 * add ENE SDIO host workaround for x86 linux platform.
 *
 * 07 08 2010 cp.wu
 *
 * [WPD00003833] [MT6620 and MT5931] Driver migration - move to new repository.
 *
 * 06 06 2010 kevin.huang
 * [WPD00003832][MT6620 5931] Create driver base
 * [MT6620 5931] Create driver base
 *
 * 05 07 2010 jeffrey.chang
 * [WPD00003826]Initial import for Linux port
 * Fix hotplug bug
 *
 * 03 28 2010 jeffrey.chang
 * [WPD00003826]Initial import for Linux port
 * clear sdio interrupt
 *
 * 03 24 2010 jeffrey.chang
 * [WPD00003826]Initial import for Linux port
 * initial import for Linux port
**
*/

/*                                                                              
                                                     
                                                                                
*/

/*                                                                              
                                                          
                                                                                
*/

#include "gl_os.h"

#if MTK_WCN_HIF_SDIO
#include "hif_sdio.h"
#else
#include <linux/mmc/card.h>
#include <linux/mmc/host.h>
#include <linux/mmc/sdio.h>
#include <linux/mmc/sdio_func.h>	/*                   */
#include <linux/mmc/sdio_ids.h>
#endif

#include <linux/mm.h>
#ifndef CONFIG_X86
#include <asm/memory.h>
#endif

#if defined(MT6630)
#include "mt6630_reg.h"
#endif

#if CFG_DBG_GPIO_PINS		/*                                          */
#include <mach/mt6516_gpio.h>
#endif

/*                                                                              
                                                
                                                                                
*/

#define HIF_SDIO_ERR_TITLE_STR              "["CHIP_NAME"] SDIO Access Error!"
#define HIF_SDIO_ERR_DESC_STR               "**SDIO Access Error**\n"

#define HIF_SDIO_ACCESS_RETRY_LIMIT         3

#if MTK_WCN_HIF_SDIO


/*
                      
  
 */

static INT_32 mtk_sdio_probe(MTK_WCN_HIF_SDIO_CLTCTX, const MTK_WCN_HIF_SDIO_FUNCINFO *);

static INT_32 mtk_sdio_remove(MTK_WCN_HIF_SDIO_CLTCTX);
static INT_32 mtk_sdio_interrupt(MTK_WCN_HIF_SDIO_CLTCTX);

/*
                           
 */

static MTK_WCN_HIF_SDIO_FUNCINFO funcInfo[] = {
#if defined(MT6630)
	{MTK_WCN_HIF_SDIO_FUNC(0x037a, 0x6630, 0x1, 512)},
#endif
};


static MTK_WCN_HIF_SDIO_CLTINFO cltInfo = {
	.func_tbl = funcInfo,
	.func_tbl_size = sizeof(funcInfo) / sizeof(MTK_WCN_HIF_SDIO_FUNCINFO),
	.hif_clt_probe = mtk_sdio_probe,
	.hif_clt_remove = mtk_sdio_remove,
	.hif_clt_irq = mtk_sdio_interrupt,
};

#else

static const struct sdio_device_id mtk_sdio_ids[] = {
#if defined(MT6630)
	{SDIO_DEVICE(0x037a, 0x6630)},	/*                                   */
#endif
	{ /*                 */ },
};

MODULE_DEVICE_TABLE(sdio, mtk_sdio_ids);

#endif

/*                                                                              
                                                 
                                                                                
*/


/*                                                                              
                                                  
                                                                                
*/

/*                                                                              
                                                   
                                                                                
*/
static probe_card pfWlanProbe;
static remove_card pfWlanRemove;


#if (MTK_WCN_HIF_SDIO == 0)
static struct sdio_driver mtk_sdio_driver = {
	.name = "wlan",		/*                        */
	.id_table = mtk_sdio_ids,
	.probe = NULL,
	.remove = NULL,
};
#endif


#if CFG_DBG_GPIO_PINS

/*            */
UINT_32 dbgPinSTP[] = {
	GPIO_PLATFORM(33)	/*                       */
	    , GPIO_PLATFORM(62)	/*                      */
	    , GPIO_PLATFORM(80)	/*                           */
	    , GPIO_PLATFORM(81)	/*                                 */
	    , GPIO_PLATFORM(17)	/*                            */
	    , GPIO_PLATFORM(18)	/*                            */
	    , GPIO_PLATFORM(19)	/*                             */
	    , GPIO_INVALID	/*                           */
	    , GPIO_INVALID	/*                          */
	    , GPIO_PLATFORM(135)	/*                          */
	    , GPIO_PLATFORM(136)	/*                       */
	    , GPIO_PLATFORM(30)	/*                                         */
	    , GPIO_PLATFORM(31)	/*                                         */
	    , GPIO_PLATFORM(124)	/*                               */
	    , GPIO_PLATFORM(125)	/*                                  */
	    , GPIO_PLATFORM(21)	/*                                 */
	    , GPIO_PLATFORM(29)	/*                                         */
	    , GPIO_PLATFORM(20)	/*                    */
	    /*                 */ /*                                        */
	    /*                  */ /*                                         */
	    /*                  */ /*                                     */
};
#endif
/*                                                                              
                                             
                                                                                
*/

/*                                                                              
                                                             
                                                                                
*/

/*                                                                              
                                                
                                                                                
*/
#if CFG_DBG_GPIO_PINS
void debug_gpio_init(void)
{
	int i;

	for (i = 0; i < sizeof(dbgPinSTP) / sizeof(dbgPinSTP[0]); ++i) {
		if (GPIO_INVALID == dbgPinSTP[i])
			continue;

		/*                                                             */
		mt_set_gpio_pull_enable(dbgPinSTP[i], 0);	/*              */
		mt_set_gpio_dir(dbgPinSTP[i], GPIO_DIR_OUT);	/*            */
		mt_set_gpio_mode(dbgPinSTP[i], GPIO_MODE_00);	/*               */

		/*                              */
		mt_set_gpio_out(dbgPinSTP[i], GPIO_OUT_ZERO);	/*         */
		mt_set_gpio_out(dbgPinSTP[i], GPIO_OUT_ONE);	/*          */
		mt_set_gpio_out(dbgPinSTP[i], GPIO_OUT_ZERO);	/*         */
		mt_set_gpio_out(dbgPinSTP[i], GPIO_OUT_ONE);	/*          */
	}
	/*                                                             */
}

void debug_gpio_deinit(void)
{
	int i;
	for (i = 0; i < sizeof(dbgPinSTP) / sizeof(dbgPinSTP[0]); ++i) {
		if (GPIO_INVALID == dbgPinSTP[i])
			continue;

		/*                                                             */
		mt_set_gpio_dir(dbgPinSTP[i], GPIO_DIR_IN);
	}

	/*                                             */
}

void mtk_wcn_stp_debug_gpio_assert(UINT_32 dwIndex, UINT_32 dwMethod)
{
	unsigned int i;

	if (dwIndex >= (sizeof(dbgPinSTP) / sizeof(dbgPinSTP[0])))
		/*                                                                         */
		return;

	if (dwIndex > IDX_STP_MAX)
		/*                                                                                                */

	if (GPIO_INVALID == dbgPinSTP[dwIndex])
		return;

	if (dwMethod & DBG_TIE_DIR) {
		if (dwMethod & DBG_HIGH)
			mt_set_gpio_out(dbgPinSTP[dwIndex], GPIO_OUT_ONE);
		else
			mt_set_gpio_out(dbgPinSTP[dwIndex], GPIO_OUT_ZERO);

		return;
	}

	if (dwMethod & DBG_TOGGLE(0)) {
		for (i = 0; i < DBG_TOGGLE_NUM(dwMethod); ++i) {
			mt_set_gpio_out(dbgPinSTP[dwIndex], GPIO_OUT_ZERO);
			mt_set_gpio_out(dbgPinSTP[dwIndex], GPIO_OUT_ONE);
		}
		return;
	}

	return;
}
#endif

/*                                                                            */
/* 
                                                            
 
                                         
 
              
*/
/*                                                                            */

#if MTK_WCN_HIF_SDIO

static INT_32 mtk_sdio_interrupt(MTK_WCN_HIF_SDIO_CLTCTX cltCtx)
{
	P_GLUE_INFO_T prGlueInfo = NULL;
	INT_32 ret = 0;

	prGlueInfo = mtk_wcn_hif_sdio_get_drvdata(cltCtx);

	/*                     */

	if (!prGlueInfo) {
		/*                                                                     */
		return -HIF_SDIO_ERR_FAIL;
	}

	if (prGlueInfo->ulFlag & GLUE_FLAG_HALT) {
		/*                                                        */
		ret = mtk_wcn_hif_sdio_writel(cltCtx, MCR_WHLPCR, WHLPCR_INT_EN_CLR);
		return ret;
	}

	ret = mtk_wcn_hif_sdio_writel(cltCtx, MCR_WHLPCR, WHLPCR_INT_EN_CLR);

	KAL_WAKE_LOCK(prGlueInfo->prAdapter, &prGlueInfo->rIntrWakeLock);

	set_bit(GLUE_FLAG_INT_BIT, &prGlueInfo->ulFlag);

	/*                                                             */
#if CFG_SUPPORT_MULTITHREAD
	wake_up_interruptible(&prGlueInfo->waitq_hif);
#else
	wake_up_interruptible(&prGlueInfo->waitq);
#endif

	return ret;
}

#else

static unsigned int in_interrupt;

static void mtk_sdio_interrupt(struct sdio_func *func)
{
	P_GLUE_INFO_T prGlueInfo = NULL;

	int ret = 0;

	prGlueInfo = sdio_get_drvdata(func);
	/*                     */

	if (!prGlueInfo) {
		/*                                                                     */
		return;
	}

	if (prGlueInfo->ulFlag & GLUE_FLAG_HALT) {
		sdio_writel(prGlueInfo->rHifInfo.func, WHLPCR_INT_EN_CLR, MCR_WHLPCR, &ret);
		/*                                                        */
		return;
	}

	sdio_writel(prGlueInfo->rHifInfo.func, WHLPCR_INT_EN_CLR, MCR_WHLPCR, &ret);

#if 0
	wlanISR(prGlueInfo->prAdapter, TRUE);

	if (prGlueInfo->ulFlag & GLUE_FLAG_HALT) {
		/*                                           */
		/*                                                         */
	} else {
		wlanIST(prGlueInfo->prAdapter);
	}
#endif

	set_bit(GLUE_FLAG_INT_BIT, &prGlueInfo->ulFlag);

	/*                                                             */
#if CFG_SUPPORT_MULTITHREAD
	wake_up_interruptible(&prGlueInfo->waitq_hif);
#else
	wake_up_interruptible(&prGlueInfo->waitq);
#endif
}
#endif

/*                                                                            */
/* 
                                               
 
                                          
                                                   
 
              
*/
/*                                                                            */

#if MTK_WCN_HIF_SDIO

/*                        */
static const MTK_WCN_HIF_SDIO_FUNCINFO *prFunc;


static INT_32
mtk_sdio_probe(MTK_WCN_HIF_SDIO_CLTCTX cltCtx, const MTK_WCN_HIF_SDIO_FUNCINFO *prFuncInfo)
{
	INT_32 ret = HIF_SDIO_ERR_SUCCESS;

	prFunc = prFuncInfo;

	if (pfWlanProbe((PVOID) & cltCtx) != WLAN_STATUS_SUCCESS) {
		/*                                                                        */
		pfWlanRemove();
		ret = -(HIF_SDIO_ERR_FAIL);
	} else {
		/*                                                                    */
	}
	return ret;
}
#else
static int mtk_sdio_probe(struct sdio_func *func, const struct sdio_device_id *id)
{
	int ret = 0;
	int i = 0;

	/*                                                  */

	ASSERT(func);
	ASSERT(id);

	/*                                                               */
	/*                                                                                   */
	/*                                                                                       */
	/*                                                                                       */
	/*                                                                                           */
	/*                                                                                       */
	/*                                                                                             */
	/*                                                                                             */
	/*                                                                                         */

	/*                                                              */
	/*                                                   */
	/*                                                                            */

	for (i = 0; i < func->card->num_info; i++) {
		/*                                                                      */
	}

	sdio_claim_host(func);
	ret = sdio_enable_func(func);
	sdio_release_host(func);

	if (ret) {
		/*                                                         */
		goto out;
	}
	/*                                                       */

	if (pfWlanProbe((PVOID) func) != WLAN_STATUS_SUCCESS) {
		/*                                                                        */
		pfWlanRemove();
		ret = -1;
	} else {
#if CFG_DBG_GPIO_PINS
		/*                                                                     */
		/*                           */
		debug_gpio_init();
#endif
	}

out:
	/*                                                               */
	return ret;
}
#endif


#if MTK_WCN_HIF_SDIO
static INT_32 mtk_sdio_remove(MTK_WCN_HIF_SDIO_CLTCTX cltCtx)
{
	INT_32 ret = HIF_SDIO_ERR_SUCCESS;
	/*                                                  */
	pfWlanRemove();

	return ret;
}
#else
static void mtk_sdio_remove(struct sdio_func *func)
{
	/*                                                  */

#if CFG_DBG_GPIO_PINS
	/*                                                             */
	debug_gpio_deinit();
#endif

	ASSERT(func);
	/*                                                  */
	pfWlanRemove();

	sdio_claim_host(func);
	sdio_disable_func(func);
	/*                                                         */
	sdio_release_host(func);

	/*                                                       */
}
#endif

#if (MTK_WCN_HIF_SDIO == 0)
static int mtk_sdio_suspend(struct device *pDev, pm_message_t state)
{
	/*                                                                   */
	/*                                                            */

	return 0;
}

int mtk_sdio_resume(struct device *pDev)
{
	/*                                                                  */

	return 0;
}
#endif


/*                                                                            */
/* 
                                                       
 
                                                       
                                                       
 
                                            
*/
/*                                                                            */
WLAN_STATUS glRegisterBus(probe_card pfProbe, remove_card pfRemove)
{
	int ret = 0;

	ASSERT(pfProbe);
	ASSERT(pfRemove);

	/*                                                            */
	/* printk(KERN_INFO "mtk_sdio: Copyright MediaTek Inc.\n"); */

	pfWlanProbe = pfProbe;
	pfWlanRemove = pfRemove;

#if MTK_WCN_HIF_SDIO
	/*                          */
	ret =
	    ((mtk_wcn_hif_sdio_client_reg(&cltInfo) ==
	      HIF_SDIO_ERR_SUCCESS) ? WLAN_STATUS_SUCCESS : WLAN_STATUS_FAILURE);
#else
	mtk_sdio_driver.probe = mtk_sdio_probe;
	mtk_sdio_driver.remove = mtk_sdio_remove;

	mtk_sdio_driver.drv.suspend = mtk_sdio_suspend;
	mtk_sdio_driver.drv.resume = mtk_sdio_resume;

	ret =
	    (sdio_register_driver(&mtk_sdio_driver) ==
	     0) ? WLAN_STATUS_SUCCESS : WLAN_STATUS_FAILURE;
#endif

	return ret;
}				/*                        */


/*                                                                            */
/* 
                                                         
 
                                                       
 
                
*/
/*                                                                            */
VOID glUnregisterBus(remove_card pfRemove)
{
	ASSERT(pfRemove);
	pfRemove();

#if MTK_WCN_HIF_SDIO
	/*                            */
	mtk_wcn_hif_sdio_client_unreg(&cltInfo);
#else
	sdio_unregister_driver(&mtk_sdio_driver);
#endif

	return;
}				/*                          */


/*                                                                            */
/* 
                                                                            
 
                                                      
                                                                            
 
                
*/
/*                                                                            */
VOID glSetHifInfo(P_GLUE_INFO_T prGlueInfo, ULONG ulCookie)
{
	P_GL_HIF_INFO_T prHif = NULL;

	prHif = &prGlueInfo->rHifInfo;

#if MTK_WCN_HIF_SDIO
	/*                                                               */
	prHif->prFuncInfo = prFunc;
	prHif->cltCtx = *((MTK_WCN_HIF_SDIO_CLTCTX *) ulCookie);
	mtk_wcn_hif_sdio_set_drvdata(prHif->cltCtx, prGlueInfo);

#else
	prHif->func = (struct sdio_func *)ulCookie;

	/*                                                                           */
	/*                                                                                  */
	/*                                                                                  */
	/*                                                                             */

	sdio_set_drvdata(prHif->func, prGlueInfo);

	SET_NETDEV_DEV(prGlueInfo->prDevHandler, &prHif->func->dev);
#endif

	return;
}				/*                       */

/*                                                                            */
/* 
                                               
 
                                                      
 
                
*/
/*                                                                            */
VOID glClearHifInfo(P_GLUE_INFO_T prGlueInfo)
{
	/*                               */
	/*                     */
	/*                                */

	return;
}				/*                         */


/*                                                                            */
/* 
                                                                                 
 
                                                                   
                                                                                  
                                            
 
                
*/
/*                                                                            */
BOOL glBusInit(PVOID pvData)
{
#if (MTK_WCN_HIF_SDIO == 0)
	int ret = 0;
	struct sdio_func *func = NULL;

	ASSERT(pvData);

	func = (struct sdio_func *)pvData;

	sdio_claim_host(func);
	ret = sdio_set_block_size(func, 512);
	sdio_release_host(func);

	if (ret) {
		/*                                                                */
	} else {
		/*                                                              */
	}

	/*                                                                                */
	/*                                                                                */
	/*                                                                                                          */
	/*                                                                                                            */
#endif
	return TRUE;
}				/*                    */


/*                                                                            */
/* 
                                                  
 
                                                   
 
                
*/
/*                                                                            */
VOID glBusRelease(PVOID pvData)
{

	return;
}				/*                       */


/*                                                                            */
/* 
                                                                    
 
                                                       
                                                                
                                                         
 
                                          
                                  
*/
/*                                                                            */
INT_32 glBusSetIrq(PVOID pvData, PVOID pfnIsr, PVOID pvCookie)
{
	int ret = 0;

	struct net_device *prNetDevice = NULL;
	P_GLUE_INFO_T prGlueInfo = NULL;
	P_GL_HIF_INFO_T prHifInfo = NULL;

	ASSERT(pvData);
	if (!pvData)
		return -1;

	prNetDevice = (struct net_device *)pvData;
	prGlueInfo = (P_GLUE_INFO_T) pvCookie;
	ASSERT(prGlueInfo);
	if (!prGlueInfo)
		return -1;

	prHifInfo = &prGlueInfo->rHifInfo;

#if (MTK_WCN_HIF_SDIO == 0)
	sdio_claim_host(prHifInfo->func);
	ret = sdio_claim_irq(prHifInfo->func, mtk_sdio_interrupt);
	sdio_release_host(prHifInfo->func);
#else
	mtk_wcn_hif_sdio_enable_irq(prHifInfo->cltCtx, TRUE);
#endif

	return ret;
}				/*                      */


/*                                                                            */
/* 
                                                                            
 
                                                       
                                                         
 
                
*/
/*                                                                            */
VOID glBusFreeIrq(PVOID pvData, PVOID pvCookie)
{
	struct net_device *prNetDevice = NULL;
	P_GLUE_INFO_T prGlueInfo = NULL;
	P_GL_HIF_INFO_T prHifInfo = NULL;

	ASSERT(pvData);
	if (!pvData) {
		/*                                                             */
		return;
	}
	prNetDevice = (struct net_device *)pvData;
	prGlueInfo = (P_GLUE_INFO_T) pvCookie;
	ASSERT(prGlueInfo);
	if (!prGlueInfo) {
		/*                                                              */
		return;
	}

	prHifInfo = &prGlueInfo->rHifInfo;
#if (MTK_WCN_HIF_SDIO == 0)
	sdio_claim_host(prHifInfo->func);
	sdio_release_irq(prHifInfo->func);
	sdio_release_host(prHifInfo->func);
#else
	mtk_wcn_hif_sdio_enable_irq(prHifInfo->cltCtx, FALSE);
#endif

	return;
}				/*                      */

BOOLEAN glIsReadClearReg(UINT_32 u4Address)
{
	switch (u4Address) {
	case MCR_WHISR:
	case MCR_WASR:
	case MCR_D2HRM0R:
	case MCR_D2HRM1R:
	case MCR_WTQCR0:
	case MCR_WTQCR1:
	case MCR_WTQCR2:
	case MCR_WTQCR3:
	case MCR_WTQCR4:
	case MCR_WTQCR5:
	case MCR_WTQCR6:
	case MCR_WTQCR7:
		return TRUE;

	default:
		return FALSE;
	}
}

/*                                                                            */
/* 
                                      
 
                                                             
                                       
                                                                    
 
                                         
                                      
*/
/*                                                                            */
BOOL kalDevRegRead(IN P_GLUE_INFO_T prGlueInfo, IN UINT_32 u4Register, OUT PUINT_32 pu4Value)
{
	int ret = 0;
	UINT_8 ucRetryCount = 0;

	ASSERT(prGlueInfo);
	ASSERT(pu4Value);

	do {
#if MTK_WCN_HIF_SDIO
		ret =
		    mtk_wcn_hif_sdio_readl(prGlueInfo->rHifInfo.cltCtx, u4Register,
					   (PUINT_32) pu4Value);
#else
		if (!in_interrupt)
			sdio_claim_host(prGlueInfo->rHifInfo.func);

		*pu4Value = sdio_readl(prGlueInfo->rHifInfo.func, u4Register, &ret);

		if (!in_interrupt)
			sdio_release_host(prGlueInfo->rHifInfo.func);
#endif

		if (ret || ucRetryCount) {
			/*                   
                                                                            
                                                                            */

			if (glIsReadClearReg(u4Register) && (ucRetryCount == 0)) {
				/*                          */
				u4Register = MCR_WSR;
			}
		}

		ucRetryCount++;
		if (ucRetryCount > HIF_SDIO_ACCESS_RETRY_LIMIT)
			break;
	} while (ret);

	if (ret) {
		kalSendAeeWarning(HIF_SDIO_ERR_TITLE_STR,
				  HIF_SDIO_ERR_DESC_STR "sdio_readl() reports error: %x retry: %u",
				  ret, ucRetryCount);
		DBGLOG(HAL, ERROR,
		       ("sdio_readl() reports error: %x retry: %u\n", ret, ucRetryCount));
	}

	return (ret) ? FALSE : TRUE;
}				/*                        */


/*                                                                            */
/* 
                                       
 
                                                             
                                       
                                           
 
                                         
                                      
*/
/*                                                                            */
BOOL kalDevRegWrite(IN P_GLUE_INFO_T prGlueInfo, IN UINT_32 u4Register, IN UINT_32 u4Value)
{
	int ret = 0;
	UINT_8 ucRetryCount = 0;

	ASSERT(prGlueInfo);

	do {
#if MTK_WCN_HIF_SDIO
		ret = mtk_wcn_hif_sdio_writel(prGlueInfo->rHifInfo.cltCtx, u4Register, u4Value);
#else
		if (!in_interrupt)
			sdio_claim_host(prGlueInfo->rHifInfo.func);

		sdio_writel(prGlueInfo->rHifInfo.func, u4Value, u4Register, &ret);

		if (!in_interrupt)
			sdio_release_host(prGlueInfo->rHifInfo.func);
#endif

		if (ret || ucRetryCount) {
			/*                   
                                                                         
                         */
		}

		ucRetryCount++;
		if (ucRetryCount > HIF_SDIO_ACCESS_RETRY_LIMIT)
			break;

	} while (ret);

	if (ret) {
		kalSendAeeWarning(HIF_SDIO_ERR_TITLE_STR,
				  HIF_SDIO_ERR_DESC_STR "sdio_writel() reports error: %x retry: %u",
				  ret, ucRetryCount);
		DBGLOG(HAL, ERROR,
		       ("sdio_writel() reports error: %x retry: %u\n", ret, ucRetryCount));
	}

	return (ret) ? FALSE : TRUE;
}				/*                         */


/*                                                                            */
/* 
                             
 
                                                                     
                                               
                                                 
                                                      
                                                                         
 
                                         
                                      
*/
/*                                                                            */
BOOL
kalDevPortRead(IN P_GLUE_INFO_T prGlueInfo,
	       IN UINT_16 u2Port,
	       IN UINT_32 u4Len, OUT PUINT_8 pucBuf, IN UINT_32 u4ValidOutBufSize)
{
	P_GL_HIF_INFO_T prHifInfo = NULL;
	PUINT_8 pucDst = NULL;
	int count = u4Len;
	int ret = 0;
	int bNum = 0;

#if (MTK_WCN_HIF_SDIO == 0)
	struct sdio_func *prSdioFunc = NULL;
#endif

#if DBG
	/*                                                                                                         */
#endif

	ASSERT(prGlueInfo);
	prHifInfo = &prGlueInfo->rHifInfo;

	ASSERT(pucBuf);
	pucDst = pucBuf;

	ASSERT(u4Len <= u4ValidOutBufSize);

#if (MTK_WCN_HIF_SDIO == 0)
	prSdioFunc = prHifInfo->func;

	ASSERT(prSdioFunc->cur_blksize > 0);

	if (!in_interrupt)
		sdio_claim_host(prSdioFunc);

	/*                                                              */
	while (count >= prSdioFunc->cur_blksize) {
		count -= prSdioFunc->cur_blksize;
		bNum++;
	}
	if (count > 0 && bNum > 0)
		bNum++;

	if (bNum > 0) {
		ret = sdio_readsb(prSdioFunc, pucDst, u2Port, prSdioFunc->cur_blksize * bNum);

#ifdef CONFIG_X86
		/*                */
		{
			int tmp;
			sdio_writel(prSdioFunc, 0x0, SDIO_X86_WORKAROUND_WRITE_MCR, &tmp);
		}
#endif

	} else {
		ret = sdio_readsb(prSdioFunc, pucDst, u2Port, count);
	}

	if (!in_interrupt)
		sdio_release_host(prSdioFunc);
#else

	/*                                                              */
	while (count >= (prGlueInfo->rHifInfo).prFuncInfo->blk_sz) {
		count -= ((prGlueInfo->rHifInfo).prFuncInfo->blk_sz);
		bNum++;
	}
	if (count > 0 && bNum > 0)
		bNum++;

	if (bNum > 0) {
		ret =
		    mtk_wcn_hif_sdio_read_buf(prGlueInfo->rHifInfo.cltCtx, u2Port, (PUINT_32) pucDst,
					      ((prGlueInfo->rHifInfo).prFuncInfo->blk_sz) * bNum);
	} else {
		ret =
		    mtk_wcn_hif_sdio_read_buf(prGlueInfo->rHifInfo.cltCtx, u2Port, (PUINT_32) pucDst,
					      count);
	}
#endif

	if (ret) {
		kalSendAeeWarning(HIF_SDIO_ERR_TITLE_STR,
				  HIF_SDIO_ERR_DESC_STR "sdio_readsb() reports error: %x", ret);
		DBGLOG(HAL, ERROR, ("sdio_readsb() reports error: %x\n", ret));
	}

	return (ret) ? FALSE : TRUE;
}				/*                         */


/*                                                                            */
/* 
                              
 
                                                                     
                                               
                                                  
                                                       
                                                                         
 
                                         
                                      
*/
/*                                                                            */
BOOL
kalDevPortWrite(IN P_GLUE_INFO_T prGlueInfo,
		IN UINT_16 u2Port, IN UINT_32 u4Len, IN PUINT_8 pucBuf, IN UINT_32 u4ValidInBufSize)
{
	P_GL_HIF_INFO_T prHifInfo = NULL;
	PUINT_8 pucSrc = NULL;
	int count = u4Len;
	int ret = 0;
	int bNum = 0;

#if (MTK_WCN_HIF_SDIO == 0)
	struct sdio_func *prSdioFunc = NULL;
#endif

#if DBG
	/*                                                                                                          */
#endif

	ASSERT(prGlueInfo);
	prHifInfo = &prGlueInfo->rHifInfo;

	ASSERT(pucBuf);
	pucSrc = pucBuf;

	ASSERT(u4Len <= u4ValidInBufSize);

#if (MTK_WCN_HIF_SDIO == 0)
	prSdioFunc = prHifInfo->func;
	ASSERT(prSdioFunc->cur_blksize > 0);

	if (!in_interrupt)
		sdio_claim_host(prSdioFunc);

	/*                                                              */
	while (count >= prSdioFunc->cur_blksize) {
		count -= prSdioFunc->cur_blksize;
		bNum++;
	}
	if (count > 0 && bNum > 0)
		bNum++;

	if (bNum > 0) {		/*            */
		ret = sdio_writesb(prSdioFunc, u2Port, pucSrc, prSdioFunc->cur_blksize * bNum);

#ifdef CONFIG_X86
		/*                */
		{
			int tmp;
			sdio_writel(prSdioFunc, 0x0, SDIO_X86_WORKAROUND_WRITE_MCR, &tmp);
		}
#endif

	} else {		/*           */

		ret = sdio_writesb(prSdioFunc, u2Port, pucSrc, count);
	}

	if (!in_interrupt)
		sdio_release_host(prSdioFunc);
#else
	/*                                                              */
	while (count >= ((prGlueInfo->rHifInfo).prFuncInfo->blk_sz)) {
		count -= ((prGlueInfo->rHifInfo).prFuncInfo->blk_sz);
		bNum++;
	}
	if (count > 0 && bNum > 0)
		bNum++;

	if (bNum > 0) {		/*            */
		ret =
		    mtk_wcn_hif_sdio_write_buf(prGlueInfo->rHifInfo.cltCtx, u2Port,
					       (PUINT_32) pucSrc,
					       ((prGlueInfo->rHifInfo).prFuncInfo->blk_sz) * bNum);
	} else {		/*           */
		ret =
		    mtk_wcn_hif_sdio_write_buf(prGlueInfo->rHifInfo.cltCtx, u2Port,
					       (PUINT_32) pucSrc, count);
	}
#endif

	if (ret) {
		kalSendAeeWarning(HIF_SDIO_ERR_TITLE_STR,
				  HIF_SDIO_ERR_DESC_STR "sdio_writesb() reports error: %x", ret);
		DBGLOG(HAL, ERROR, ("sdio_writesb() reports error: %x\n", ret));
	}

	return (ret) ? FALSE : TRUE;
}				/*                          */


/*                                                                            */
/* 
                                                 
 
                                                                     
                                               
                                                                 
 
                                         
                                      
*/
/*                                                                            */
BOOL kalDevWriteWithSdioCmd52(IN P_GLUE_INFO_T prGlueInfo, IN UINT_32 u4Addr, IN UINT_8 ucData)
{
	int ret = 0;

#if (MTK_WCN_HIF_SDIO == 0)
	if (!in_interrupt)
		sdio_claim_host(prGlueInfo->rHifInfo.func);

	sdio_writeb(prGlueInfo->rHifInfo.func, ucData, u4Addr, &ret);

	if (!in_interrupt)
		sdio_release_host(prGlueInfo->rHifInfo.func);
#else
	ret = mtk_wcn_hif_sdio_writeb(prGlueInfo->rHifInfo.cltCtx, u4Addr, ucData);
#endif

	if (ret) {
		kalSendAeeWarning(HIF_SDIO_ERR_TITLE_STR,
				  HIF_SDIO_ERR_DESC_STR "sdio_writeb() reports error: %x", ret);
		DBGLOG(HAL, ERROR, ("sdio_writeb() reports error: %x\n", ret));
	}

	return (ret) ? FALSE : TRUE;

}				/*                                   */


VOID glSetPowerState(IN P_GLUE_INFO_T prGlueInfo, IN UINT_32 ePowerMode)
{
	return;
}
