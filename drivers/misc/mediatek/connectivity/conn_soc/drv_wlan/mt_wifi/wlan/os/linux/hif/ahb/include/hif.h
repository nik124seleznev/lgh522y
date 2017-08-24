/*
                                                                                                 
*/

/*                 
                                                                      

                                                              
*/



/*
** $Log: hif.h $
 *
 * 11 01 2010 yarco.yang
 * [WCXRP00000149] [MT6620 WI-Fi][Driver]Fine tune performance on MT6516 platform
 * Add GPIO debug function
 *
 * 10 19 2010 jeffrey.chang
 * [WCXRP00000120] [MT6620 Wi-Fi][Driver] Refine linux kernel module to the license of MTK propietary and enable MTK HIF by default
 * Refine linux kernel module to the license of MTK and enable MTK HIF
 *
 * 08 18 2010 jeffrey.chang
 * NULL
 * support multi-function sdio
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
 * 03 24 2010 jeffrey.chang
 * [WPD00003826]Initial import for Linux port 
 * initial import for Linux port
**  \main\maintrunk.MT5921\4 2009-10-20 17:38:28 GMT mtk01090
**  Refine driver unloading and clean up procedure. Block requests, stop main thread and clean up queued requests, and then stop hw.
**  \main\maintrunk.MT5921\3 2009-09-28 20:19:20 GMT mtk01090
**  Add private ioctl to carry OID structures. Restructure public/private ioctl interfaces to Linux kernel.
**  \main\maintrunk.MT5921\2 2009-08-18 22:57:05 GMT mtk01090
**  Add Linux SDIO (with mmc core) support.
**  Add Linux 2.6.21, 2.6.25, 2.6.26.
**  Fix compile warning in Linux.
**  \main\maintrunk.MT5921\2 2008-09-22 23:18:17 GMT mtk01461
**  Update driver for code review
** Revision 1.1  2007/07/05 07:25:33  MTK01461
** Add Linux initial code, modify doc, add 11BB, RF init code
**
** Revision 1.3  2007/06/27 02:18:51  MTK01461
** Update SCAN_FSM, Initial(Can Load Module), Proc(Can do Reg R/W), TX API
**
*/

#ifndef _HIF_H
#define _HIF_H

#include "gl_typedef.h"
#include "mtk_porting.h"


/*                                                                              
                                                     
                                                                                
*/
#define CONF_MTK_AHB_DMA         1  /*                                             */

#define CONF_HIF_DEV_MISC        0  /*                         */
#define CONF_HIF_LOOPBACK_AUTO   0  /*                                       */
                                    /*                           */

#define CONF_HIF_PMIC_TEST       0  /*                                */
#define CONF_HIF_CONNSYS_DBG     1  /*                                                   */

#define CONF_HIF_DMA_DBG         0
#define CONF_HIF_DMA_INT         0  /*                    */


/*                                                                              
                                                          
                                                                                
*/


/*                                                                              
                                                
                                                                                
*/
#ifndef CONN_MCU_CONFIG_BASE
#define CONN_MCU_CONFIG_BASE         0xF8070000 //      
#endif /*                      */

#define CONSYS_CPUPCR_REG		    (CONN_MCU_CONFIG_BASE + 0x00000160)
#define CONSYS_REG_READ(addr)       (*((volatile UINT_32*)(addr)))

#define CONN_MCU_DRV_BASE                0x18070000
#define CONN_MCU_REG_LENGTH              0x0200
#define CONN_MCU_CPUPCR                  0x0160


/*                                                                              
                                                 
                                                                                
*/

/*                                                                      
                        
 */
typedef struct _GL_HIF_DMA_OPS_T { /*               */
    VOID (*DmaConfig)(
        IN VOID                     *HifInfo,
        IN VOID                     *Conf
        );

    VOID (*DmaStart)(
        IN VOID                     *HifInfo
        );

    VOID (*DmaStop)(
        IN VOID                     *HifInfo
        );

    MTK_WCN_BOOL (*DmaPollStart)(
        IN VOID                     *HifInfo
        );

    MTK_WCN_BOOL (*DmaPollIntr)(
        IN VOID                     *HifInfo
        );

    VOID (*DmaAckIntr)(
        IN VOID                     *HifInfo
        );

    VOID (*DmaClockCtrl)(
        IN UINT_32                   FlgIsEnabled
        );

    VOID (*DmaRegDump)(
        IN VOID                     *HifInfo
        );

    VOID (*DmaReset)(
        IN VOID                     *HifInfo
        );

} GL_HIF_DMA_OPS_T;

typedef struct _GL_HIF_INFO_T {

    /*         */
    VOID                    *Dev; /*               */

#define MTK_CHIP_ID_6571	0x6571
#define MTK_CHIP_ID_6572    0x6572
#define MTK_CHIP_ID_6582    0X6582
#define MTK_CHIP_ID_8127    0X8127
#define MTK_CHIP_ID_6752    0X6752
    UINT_32                  ChipID;

    /*              */
    BOOLEAN                 fgIntReadClear;
    BOOLEAN                 fgMbxReadClear;
    BOOLEAN                 fgDmaEnable; /*                                  */

    /*             */
    UINT_8                   *HifRegBaseAddr; /*                   */
    UINT_8                   *McuRegBaseAddr; /*                        */

#if (CONF_HIF_LOOPBACK_AUTO == 1)
    struct timer_list       HifTmrLoopbkFn; /*                                 */
    wait_queue_head_t       HifWaitq;
    UINT_32                  HifLoopbkFlg;
    struct task_struct      *HifTaskLoopbkFn; /*                        */
#endif /*                        */

#if (CONF_HIF_DMA_INT == 1)
    wait_queue_head_t       HifDmaWaitq;
    UINT_32                  HifDmaWaitFlg;
#endif /*                  */

    /*             */
#define AP_DMA_HIF_LOCK(_lock)      //                                
#define AP_DMA_HIF_UNLOCK(_lock)    //                                  
    spinlock_t              DdmaLock; /*                    */

    UINT_8                   *DmaRegBaseAddr; /*                   */
    GL_HIF_DMA_OPS_T        *DmaOps; /*               */

} GL_HIF_INFO_T, *P_GL_HIF_INFO_T;


#define HIF_MOD_NAME                "AHB_SLAVE_HIF"

#define HIF_DRV_BASE                0x180F0000
#define HIF_DRV_LENGTH				0x005c

typedef enum _MTK_WCN_HIF_BURST_LEN {
	HIF_BURST_1DW = 0,
	HIF_BURST_4DW,
	HIF_BURST_8DW
} MTK_WCN_HIF_BURST_LEN;

typedef enum _MTK_WCN_HIF_TXRX_TARGET {
	HIF_TARGET_TXD0 = 0,
	HIF_TARGET_TXD1,
	HIF_TARGET_RXD0,
	HIF_TARGET_RXD1,
	HIF_TARGET_WHISR
} MTK_WCN_HIF_TXRX_TARGET;

typedef enum _MTK_WCN_HIF_DMA_DIR {
	HIF_DMA_DIR_TX = 0,
	HIF_DMA_DIR_RX
} MTK_WCN_HIF_DMA_DIR;

typedef struct _MTK_WCN_HIF_DMA_CONF {
    UINT_32                  Count;
    MTK_WCN_HIF_DMA_DIR     Dir;
	UINT_32                  Burst;
    UINT_32                  Wsize;
    UINT_32                  Ratio;
    UINT_32                  Connect;
	UINT_32                  Fix_en;
	ULONG                  	Src;
	ULONG					Dst;
} MTK_WCN_HIF_DMA_CONF;


/*                                                                              
                                                  
                                                                                
*/


/*                                                                              
                                                   
                                                                                
*/


/*                                                                              
                                             
                                                                                
*/
#define MCU_REG_READL(_hif, _addr)          \
            readl((volatile UINT_32 *)((_hif)->McuRegBaseAddr + _addr))

/*                                  */
#define HIF_REG_READL(_hif, _addr)          \
            readl((volatile UINT_32 *)((_hif)->HifRegBaseAddr + _addr))

#define HIF_REG_WRITEL(_hif, _addr, _val)   \
            writel(_val, ((volatile UINT_32 *)((_hif)->HifRegBaseAddr + _addr)))

#define HIF_REG_WRITEB(_hif, _addr, _val)   \
            writeb(_val, ((volatile UINT_32 *)((_hif)->HifRegBaseAddr + _addr)))

/*                                  */
#define HIF_DMAR_READL(_hif, _addr)          \
            readl((volatile UINT_32 *)((_hif)->DmaRegBaseAddr + _addr))

#define HIF_DMAR_WRITEL(_hif, _addr, _val)   \
            writel(_val, ((volatile UINT_32 *)((_hif)->DmaRegBaseAddr + _addr)))


/*                                                                              
                                                             
                                                                                
*/

#ifndef MODULE_AHB_DMA

VOID
HifRegDump(
    P_ADAPTER_T prAdapter
    );

BOOLEAN
HifIsFwOwn(
    P_ADAPTER_T prAdapter
    );

WLAN_STATUS
glRegisterBus(
    probe_card pfProbe,
    remove_card pfRemove
    );

VOID
glUnregisterBus(
    remove_card pfRemove
    );

VOID
glResetHif (
    GLUE_INFO_T *GlueInfo
    );

VOID
glSetHifInfo (
    P_GLUE_INFO_T prGlueInfo,
    ULONG ulCookie
    );

VOID
glClearHifInfo (
    P_GLUE_INFO_T prGlueInfo
    );

VOID
glGetChipInfo (
    GLUE_INFO_T *GlueInfo,
	UINT_8 *pucChipBuf
    );

#if CFG_SPM_WORKAROUND_FOR_HOTSPOT
BOOLEAN
glIsChipNeedWakelock (
    GLUE_INFO_T *GlueInfo
    );
#endif

BOOLEAN
glBusInit (
    PVOID pvData
    );

VOID
glBusRelease (
    PVOID pData
    );

INT_32
glBusSetIrq (
    PVOID pvData,
    PVOID pfnIsr,
    PVOID pvCookie
    );

VOID
glBusFreeIrq (
    PVOID pvData,
    PVOID pvCookie
    );

VOID
glSetPowerState (
    IN P_GLUE_INFO_T  prGlueInfo,
    IN UINT_32 ePowerMode
    );

VOID glDumpConnSysCpuInfo(P_GLUE_INFO_T prGlueInfo);

#endif /*                */

/*                                                                            */
/* 
                           
 
                                                                
                                                            
 
              
*/
/*                                                                            */
VOID
HifGdmaInit (
    GL_HIF_INFO_T               *HifInfo
    );

/*                                                                            */
/* 
                           
 
                                                                
                                                            
 
              
*/
/*                                                                            */
VOID
HifPdmaInit (
    GL_HIF_INFO_T               *HifInfo
    );

/*                                                                              
                                                
                                                                                
*/
#endif /*        */
