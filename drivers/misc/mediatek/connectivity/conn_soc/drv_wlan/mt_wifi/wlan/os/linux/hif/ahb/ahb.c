/******************************************************************************
*[File]             ahb.c
*[Version]          v1.0
*[Revision Date]    2013-01-16
*[Author]
*[Description]
*    The program provides AHB HIF driver
*[Copyright]
*    Copyright (C) 2013 MediaTek Incorporation. All Rights Reserved.
******************************************************************************/



/*
** $Log: ahb.c $
 *
 * 01 16 2013 vend_samp.lin
 * Port sdio.c to ahb.c on MT6572/MT6582
 * 1) Initial version
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
#include <linux/interrupt.h>
//                         
#include <linux/device.h>
//                        
#include <linux/platform_device.h>
//                     
//                       
//                       

#include <linux/mm.h>
#ifndef CONFIG_X86
#include <asm/memory.h>
#endif

#ifdef CONFIG_OF
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include <linux/of.h>
#else

#endif

#include <mach/mt_pm_ldo.h>
#include <mach/mt_gpt.h>

#include "gl_os.h"

#if defined(MT6620)
    #include "mt6620_reg.h"
#elif defined(MT5931)
    #include "mt5931_reg.h"
#elif defined(MT6628)
    #include "mtreg.h"
#endif

//                                                          
//                     
//                        

#ifdef HIF_DEBUG_SUP
#define HIF_DBG(msg)   printk msg
#else
#define HIF_DBG(msg)
#endif /*               */

#ifdef HIF_DEBUG_SUP_TX
#define HIF_DBG_TX(msg)   printk msg
#else
#define HIF_DBG_TX(msg)
#endif /*               */


/*                                                                              
                                                
                                                                                
*/


/*                                                                              
                                                 
                                                                                
*/


/*                                                                              
                                             
                                                                                
*/


/*                                                                              
                                                             
                                                                                
*/

static void
HifAhbDmaEnhanceModeConf(
    IN GLUE_INFO_T                  *GlueInfo,
    IN UINT_32                       BurstLen,
    IN UINT_32                       PortId,
    IN UINT_32                       TransByte
    );

static irqreturn_t
HifAhbISR(
    IN int                          Irq,
    IN void                         *Arg
    );

static int
HifAhbProbe (
    VOID
    );

static int
HifAhbRemove (
    VOID
    );

#if (MTK_WCN_SINGLE_MODULE == 0)
static int
HifAhbBusCntGet(
    VOID
    );

static int
HifAhbBusCntClr(
    VOID
    );

static int HifTxCnt = 0;
#endif /*                       */

#if (CONF_HIF_DEV_MISC == 1)
static ssize_t 
HifAhbMiscRead(
    IN struct file                  *Filp,
    OUT char __user                 *DstBuf,
    IN size_t                       Size,
    IN loff_t                       *Ppos
    );

static ssize_t 
HifAhbMiscWrite(
    IN struct file                  *Filp,
    IN const char __user            *SrcBuf,
    IN size_t                       Size,
    IN loff_t                       *Ppos
    );

static int
HifAhbMiscIoctl(
    IN struct file                  *Filp,
    IN unsigned int                 Cmd,
    IN unsigned long                arg
    );

static int
HifAhbMiscOpen(
    IN struct inode                 *Inodep,
    IN struct file                  *Filp
    );

static int
HifAhbMiscClose(
    IN struct inode                 *Inodep,
    IN struct file                  *Filp
    );
#else

static int
HifAhbPltmProbe(
    IN struct platform_device       *PDev
    );

static int __exit
HifAhbPltmRemove(
    IN struct platform_device       *PDev
    );

#ifdef CONFIG_PM
static int
HifAhbPltmSuspend(
    IN struct platform_device       *PDev,
    pm_message_t                    Message
    );

static int
HifAhbPltmResume(
    IN struct platform_device       *PDev
    );
#endif /*           */

#endif /*                   */

#if (CONF_HIF_LOOPBACK_AUTO == 1) /*                           */
static VOID
HifAhbLoopbkAuto(
    IN unsigned long            arg
    );
#endif /*                        */

#if (CONF_HIF_DMA_INT == 1)
static irqreturn_t
HifDmaISR(
    IN int                          Irq,
    IN void                         *Arg
    );
#endif /*                  */



/*                                                                              
                                                  
                                                                                
*/
extern BOOLEAN fgIsResetting;


/*                                                                              
                                                   
                                                                                
*/

/*                                          */
static probe_card pfWlanProbe = NULL;

/*                                    */
static remove_card pfWlanRemove = NULL;

/*              */
static GL_HIF_DMA_OPS_T *pfWlanDmaOps = NULL;

static BOOLEAN WlanDmaFatalErr = 0;

#if (CONF_HIF_DEV_MISC == 1)
static const struct file_operations MtkAhbOps = {
	.owner				= THIS_MODULE,
	.read				= HifAhbMiscRead,
	.write				= HifAhbMiscWrite,
	.unlocked_ioctl		= HifAhbMiscIoctl,
	.compat_ioctl		= HifAhbMiscIoctl,
	.open				= HifAhbMiscOpen,
	.release			= HifAhbMiscClose,
};

static struct miscdevice MtkAhbDriver = {
	.minor = MISC_DYNAMIC_MINOR, /*                  */
	.name  = HIF_MOD_NAME,
	.fops  = &MtkAhbOps,
};
#else

#ifdef CONFIG_OF
static const struct of_device_id apwifi_of_ids[] = {
	{ .compatible = "mediatek,WIFI", },
	{}
};
#endif

struct platform_driver MtkPltmAhbDriver = {
	.driver = {
		.name = "mt-wifi",
		.owner = THIS_MODULE,
#ifdef CONFIG_OF
        .of_match_table = apwifi_of_ids,
#endif
	},
	.probe = HifAhbPltmProbe,
#ifdef CONFIG_PM
	.suspend = HifAhbPltmSuspend,
	.resume = HifAhbPltmResume,
#else
    .suspend = NULL,
    .resume = NULL,
#endif /*           */
	.remove = __exit_p ( HifAhbPltmRemove ),
};

static struct platform_device *HifAhbPDev;

#endif /*                   */




/*                                                                              
                                                       
                                                                                
*/

#if 0
/*                     */
#define JTAG_ADDR1_BASE 0x10208000
#define JTAG_ADDR2_BASE 0x10005300
char *jtag_addr1 = (char *)JTAG_ADDR1_BASE;
char *jtag_addr2 = (char *)JTAG_ADDR2_BASE;

#define JTAG1_REG_WRITE(addr, value)    \
    writel(value, ((volatile UINT_32 *)(jtag_addr1+(addr-JTAG_ADDR1_BASE))))
#define JTAG1_REG_READ(addr)            \
    readl(((volatile UINT_32 *)(jtag_addr1+(addr-JTAG_ADDR1_BASE))))
#define JTAG2_REG_WRITE(addr, value)    \
    writel(value, ((volatile UINT_32 *)(jtag_addr2+(addr-JTAG_ADDR2_BASE))))
#define JTAG2_REG_READ(addr)            \
    readl(((volatile UINT_32 *)(jtag_addr2+(addr-JTAG_ADDR2_BASE))))

static int wmt_set_jtag_for_mcu(void)
{
    int iRet = 0;
	unsigned int tmp = 0;

    jtag_addr1 = ioremap(JTAG_ADDR1_BASE, 0x5000);
    if (jtag_addr1 == 0) {
        printk("remap jtag_addr1 fail!\n");
        return -1;
    }
    printk("jtag_addr1 = 0x%p\n", jtag_addr1);
    jtag_addr2 = ioremap(JTAG_ADDR2_BASE, 0x1000);
    if (jtag_addr2 == 0) {
        printk("remap jtag_addr2 fail!\n");
        return -1;
    }
    printk("jtag_addr2 = 0x%p\n", jtag_addr2);

	/*                      */
    JTAG1_REG_WRITE(0x10208004, 0xffffffff);
	JTAG1_REG_WRITE(0x10208014, 0xffffffff);
	JTAG1_REG_WRITE(0x10209004, 0xffffffff);
	JTAG1_REG_WRITE(0x1020A004, 0xffffffff);
	JTAG1_REG_WRITE(0x1020B004, 0xffffffff);

/*                  */
	/*                  */
	/*                    */
	tmp = JTAG1_REG_READ(0x1020A040);
	tmp &= 0xffffff80;
	tmp |= 0x7f;
	JTAG1_REG_WRITE(0x1020A040, tmp);

#if 0
	/*                    */
	tmp = JTAG1_REG_READ(0x1020A060);
	tmp &= 0xffffff80;
	tmp |= 0x31;
    JTAG1_REG_WRITE(0x1020A060, tmp);
	/*                                          */
	//                                                             
	JTAG2_REG_WRITE(0x10005300, 0x00000000);
#endif

#if 1 //                   
    /*                    */
    tmp = JTAG1_REG_READ(0x1020A060);
    tmp &= 0xffffff80;
    tmp |= 0x31;
    JTAG1_REG_WRITE(0x1020A060, tmp);
    /*                                          */
    //                                                             
    JTAG2_REG_WRITE(0x1000530C, 0x08888888);//                   
#endif


/*                  */
    
 	/*                  */
	/*                    */
	tmp = JTAG1_REG_READ(0x1020A050);
	tmp &= 0xffffff80;
	tmp |= 0x7f;
	JTAG1_REG_WRITE(0x1020A050, tmp);
	/*                    */
	tmp = JTAG1_REG_READ(0x1020A070);
	tmp &= 0xffffff80;
	tmp |= 0x31;
	JTAG1_REG_WRITE(0x1020A070, tmp);
	/*                    
                      
                      */
	JTAG2_REG_WRITE(0x1000531C, 0xff111111);
	JTAG2_REG_WRITE(0x1000532C, 0x000fffff);

#if 0
    JTAG2_REG_WRITE(0x10005410, 0x77700000); /*                   */
    JTAG2_REG_WRITE(0x10005420, 0x00000077); /*                   */
    JTAG2_REG_WRITE(0x10005370, 0x70000000); /*        */
    JTAG2_REG_WRITE(0x10005380, 0x00000777); /*                 */
    JTAG2_REG_WRITE(0x100053a0, 0x70000000); /*        */
    JTAG2_REG_WRITE(0x100053b0, 0x00000777); /*                 */
    JTAG2_REG_WRITE(0x100053d0, 0x00777000); /*                   */
#endif

#if 0
	/*                          */
	JTAG2_REG_WRITE(0x1000540C, 0xffffffff);
	JTAG2_REG_WRITE(0x1000541C, 0xffffffff);
	JTAG2_REG_WRITE(0x1000538C, 0x11111fff);
	JTAG2_REG_WRITE(0x100053AC, 0xf1111111);
    JTAG2_REG_WRITE(0x100053BC, 0x00000fff);
    JTAG2_REG_WRITE(0x100053DC, 0xfffff001);
    JTAG2_REG_WRITE(0x100053EC, 0x1111100f);
	JTAG2_REG_WRITE(0x1000533C, 0x00ffffff);
	JTAG2_REG_WRITE(0x1000532C, 0xff000000);
#endif
	
	return iRet;
}


static int wmt_set_jtag_for_gps(void)
{
    int iRet = 0;
	unsigned int tmp = 0;
    /*                  */
	/*                  */
	/*                                         */
	tmp = JTAG1_REG_READ(0x1020B040);
	tmp &= 0xffC8ffff;
	tmp |= (0x37 << 16);
	JTAG1_REG_WRITE(0x1020B040, tmp);
	
	/*                                         */
	tmp = JTAG1_REG_READ(0x1020B050);
	tmp &= 0xffC8ffff;
	tmp |= (0x20 << 16);
	JTAG1_REG_WRITE(0x1020B050, tmp);

	/*                                          */
	/*                                          */
	JTAG2_REG_WRITE(0x100053BC, 0xf0fff000);
	JTAG2_REG_WRITE(0x100053CC, 0x0000000F);

    /*                                                               */
	
    return iRet;
}
#endif /*   */


/*                                                                            */
/* 
                                                       
 
                                                       
                                                       
 
                                                                        
*/
/*                                                                            */
WLAN_STATUS
glRegisterBus (
    probe_card pfProbe,
    remove_card pfRemove
    )
{
    WLAN_STATUS Ret;


    ASSERT(pfProbe);
    ASSERT(pfRemove);

    printk("glRegisterBus\n");

//                                                              
//    printk(KERN_INFO "mtk_sdio: Copyright MediaTek Inc.\n");

    pfWlanProbe = pfProbe; /*                                                         */
    pfWlanRemove = pfRemove;

#if (CONF_HIF_DEV_MISC == 1)
    if ((Ret = misc_register(&MtkAhbDriver)) != 0) {
//                                                       
		return Ret;
	}

    HifAhbProbe();
#else

    Ret = platform_driver_register(&MtkPltmAhbDriver);

#endif /*                   */

    return Ret;

} /*                        */


/*                                                                            */
/* 
                                                         
 
                                                       
 
                
*/
/*                                                                            */
VOID
glUnregisterBus(
    remove_card pfRemove
    )
{
    ASSERT(pfRemove);

    pfRemove();

#if (CONF_HIF_DEV_MISC == 1)
    HifAhbRemove();

	if ((misc_deregister(&MtkAhbDriver)) != 0) {
		//                                                             
	}
#else

    platform_driver_unregister(&MtkPltmAhbDriver);
#endif /*                   */

    return;

} /*                          */


/*                                                                            */
/* 
                                                                   
 
                                                      
 
                
*/
/*                                                                            */
VOID
glResetHif (
    GLUE_INFO_T *GlueInfo
    )
{
    GL_HIF_INFO_T *HifInfo;


    ASSERT(GlueInfo);
    HifInfo = &GlueInfo->rHifInfo;
    if (HifInfo->DmaOps)
        HifInfo->DmaOps->DmaReset(HifInfo);
}


/*                                                                            */
/* 
                                                                            
 
                                                    
                                                                            
 
                
*/
/*                                                                            */
VOID
glSetHifInfo (
    GLUE_INFO_T *GlueInfo,
    ULONG ulCookie
    )
{
    GL_HIF_INFO_T *HifInfo;


    /*          */
    ASSERT(GlueInfo);
    HifInfo = &GlueInfo->rHifInfo;
#if (CONF_HIF_DEV_MISC == 1)
    HifInfo->Dev = MtkAhbDriver.this_device;
#else
    HifInfo->Dev = &HifAhbPDev->dev;
#endif /*                   */
    SET_NETDEV_DEV(GlueInfo->prDevHandler, HifInfo->Dev);

    HifInfo->HifRegBaseAddr = ioremap(HIF_DRV_BASE, HIF_DRV_LENGTH);
    HifInfo->McuRegBaseAddr = ioremap(CONN_MCU_DRV_BASE, CONN_MCU_REG_LENGTH);
    printk("HifInfo->HifRegBaseAddr=0x%p, HifInfo->McuRegBaseAddr=0x%p \n",
        HifInfo->HifRegBaseAddr, HifInfo->McuRegBaseAddr);

    /*                     */
    HifInfo->fgDmaEnable = FALSE;
    HifInfo->DmaRegBaseAddr = 0;
    HifInfo->DmaOps = NULL;

    /*              */
    HifInfo->ChipID = HIF_REG_READL(HifInfo, MCR_WCIR) & 0xFFFF;
    printk("[WiFi/HIF] ChipID = 0x%x\n", HifInfo->ChipID);


    /*          */
	WlanDmaFatalErr = 0; /*                  */

#if (CONF_MTK_AHB_DMA == 1)
    spin_lock_init(&HifInfo->DdmaLock);

//                                            
//                             
//                                                 
        HifPdmaInit(HifInfo);

    pfWlanDmaOps = HifInfo->DmaOps;
#endif /*                  */

    /*                                      */
#if (CONF_HIF_LOOPBACK_AUTO == 1) /*                           */
{
    extern int
        kalDevLoopbkThread(
            IN void *data
            );

    init_timer(&(HifInfo->HifTmrLoopbkFn));
    HifInfo->HifTmrLoopbkFn.function = HifAhbLoopbkAuto;
    HifInfo->HifTmrLoopbkFn.data = (unsigned long) GlueInfo;

    init_waitqueue_head(&HifInfo->HifWaitq);
    HifInfo->HifTaskLoopbkFn = kthread_run(\
                    kalDevLoopbkThread, GlueInfo->prDevHandler, "LoopbkThread");
    HifInfo->HifLoopbkFlg = 0;

    /*                                                              */
    HifInfo->HifTmrLoopbkFn.expires = jiffies + MSEC_TO_SYSTIME(30000);
    add_timer(&(HifInfo->HifTmrLoopbkFn));

    HIF_DBG(("[WiFi/HIF] Start loopback test after 10 seconds (jiffies = %u)...\n",
            jiffies));
}
#endif /*                        */

#if (CONF_HIF_DMA_INT == 1)
    init_waitqueue_head(&HifInfo->HifDmaWaitq);
    HifInfo->HifDmaWaitFlg = 0;
#endif /*                  */

} /*                       */


/*                                                                            */
/* 
                                               
 
                                                    
 
                
*/
/*                                                                            */
VOID
glClearHifInfo (
    GLUE_INFO_T *GlueInfo
    )
{
	iounmap(GlueInfo->rHifInfo.HifRegBaseAddr);
	iounmap(GlueInfo->rHifInfo.DmaRegBaseAddr);
	iounmap(GlueInfo->rHifInfo.McuRegBaseAddr);
    return;

} /*                         */


/*                                                                            */
/* 
                                               
 
                                                    
 
                
*/
/*                                                                            */
VOID
glGetChipInfo (
    GLUE_INFO_T *GlueInfo,
	UINT_8 *pucChipBuf
    )
{
	GL_HIF_INFO_T *HifInfo;


	HifInfo = &GlueInfo->rHifInfo;
	printk("glGetChipInfo ChipID = 0x%x\n", HifInfo->ChipID);
	if (HifInfo->ChipID == MTK_CHIP_ID_6571)
		kalMemCopy(pucChipBuf, "6571", strlen("6571"));
	else if (HifInfo->ChipID == MTK_CHIP_ID_8127)
		kalMemCopy(pucChipBuf, "8127", strlen("8127"));
    else if (HifInfo->ChipID == MTK_CHIP_ID_6752)
		kalMemCopy(pucChipBuf, "6752", strlen("6752"));
	else
		kalMemCopy(pucChipBuf, "SOC", strlen("SOC"));
} /*                        */


#if CFG_SPM_WORKAROUND_FOR_HOTSPOT
/*                                                                            */
/* 
                                                                       
 
                                                    
 
                
*/
/*                                                                            */
BOOLEAN
glIsChipNeedWakelock (
    GLUE_INFO_T *GlueInfo
    )
{
	GL_HIF_INFO_T *HifInfo;

	HifInfo = &GlueInfo->rHifInfo;
	if (HifInfo->ChipID == MTK_CHIP_ID_6572 || HifInfo->ChipID == MTK_CHIP_ID_6582)
		return TRUE;
	else
        return FALSE;
} /*                               */
#endif /*                                */


/*                                                                            */
/* 
                                                                                 
 
                                                                   
                                                                                  
                                            
 
                
*/
/*                                                                            */
BOOLEAN
glBusInit (
    PVOID pvData
    )
{
    return TRUE;

} /*                    */


/*                                                                            */
/* 
                                                  
 
                                                   
 
                
*/
/*                                                                            */
VOID
glBusRelease (
    PVOID pvData
    )
{
    return;

} /*                       */


/*                                                                            */
/* 
                                                                    
 
                                                       
                                                                
                                                         
 
                                          
                                  
*/
/*                                                                            */
//                                
//                        
//                              
//      
//      
#ifdef CONFIG_OF
INT_32
glBusSetIrq (
    PVOID pvData,
    PVOID pfnIsr,
    PVOID pvCookie
    )
{
    struct device_node *node = NULL;
    unsigned int irq_info[3] = {0, 0, 0};
    //                      
    unsigned int irq_id = 0;
    unsigned int irq_flags = 0;

    struct net_device *prNetDevice;

    ASSERT(pvData);
    if (!pvData) {
        return -1;
    }
    prNetDevice = (struct net_device *) pvData;

    node = of_find_compatible_node(NULL, NULL, "mediatek,WIFI");
    if(node){
        irq_id = irq_of_parse_and_map(node,0);
        /*                                */
        //                                                 
        //                                                         
        //                                                  
        printk("WIFI-OF: get wifi irq(%d)\n", irq_id);
    }else{
        printk("WIFI-OF: get wifi device node fail\n");
    }

    /*                                  */
    if (of_property_read_u32_array(node, "interrupts",
                    irq_info, ARRAY_SIZE(irq_info))){
        printk("WIFI-OF: get interrupt flag from DTS fail\n");
    }else{
        irq_flags = irq_info[2];
        printk("WIFI-OF: get interrupt flag(0x%x)\n", irq_flags);
    }

    /*                  */
    if (request_irq(irq_id,
                    HifAhbISR,
                    irq_flags, HIF_MOD_NAME, prNetDevice)) {
        printk("WIFI-OF: request irq %d fail!\n", irq_id);
        return -1;
    }

    return 0;
}

VOID
glBusFreeIrq (
    PVOID pvData,
    PVOID pvCookie
    )
{
    struct device_node *node = NULL;
    unsigned int irq_info[3] = {0, 0, 0};
    //                      
    unsigned int irq_id = 0;
    unsigned int irq_flags = 0;

    struct net_device *prNetDevice;

    /*      */
    ASSERT(pvData);
    if (!pvData) {
        //                                                           
        return;
    }
    prNetDevice = (struct net_device *) pvData;

    node = of_find_compatible_node(NULL, NULL, "mediatek,WIFI");
    if(node){
        irq_id = irq_of_parse_and_map(node,0);
        /*                                */
        //                                                 
        //                                                         
        //                                                  
        printk("WIFI-OF: get wifi irq(%d)\n", irq_id);
    }else{
        printk("WIFI-OF: get wifi device node fail\n");
    }

    /*                                  */
    if (of_property_read_u32_array(node, "interrupts",
                    irq_info, ARRAY_SIZE(irq_info))){
        printk("WIFI-OF: get interrupt flag from DTS fail\n");
    }else{
        irq_flags = irq_info[2];
        printk("WIFI-OF: get interrupt flag(0x%x)\n", irq_flags);
    }

    /*              */
    free_irq(irq_id, prNetDevice);
    return;

}

#else

/*                                    */
#ifndef MT_WF_HIF_IRQ_ID /*                  */
#define MT_WF_HIF_IRQ_ID   WF_HIF_IRQ_ID
#endif /*                  */

INT_32
glBusSetIrq (
    PVOID pvData,
    PVOID pfnIsr,
    PVOID pvCookie
    )
{
    int ret = 0;
    struct net_device *prNetDevice;
    GLUE_INFO_T *GlueInfo;
    GL_HIF_INFO_T *HifInfo;


    /*      */
    ASSERT(pvData);
    if (!pvData) {
        return -1;
    }

    prNetDevice = (struct net_device *) pvData;
    GlueInfo = (GLUE_INFO_T *) pvCookie;
    ASSERT(GlueInfo);
    if (!GlueInfo) {
        printk("GlueInfo == NULL!\n");
        return -1;
    }

    HifInfo = &GlueInfo->rHifInfo;


    /*                  */
    if (request_irq(MT_WF_HIF_IRQ_ID,
                    HifAhbISR,
                    IRQF_TRIGGER_LOW, HIF_MOD_NAME, prNetDevice)) {
        printk("request irq %d fail!\n", MT_WF_HIF_IRQ_ID);
        return -1;
    }

#if (CONF_HIF_DMA_INT == 1)
    if (request_irq(MT_GDMA2_IRQ_ID,
                    HifDmaISR,
                    IRQF_TRIGGER_LOW, "AHB_DMA", prNetDevice)) {
        printk("request irq %d fail!\n", MT_GDMA2_IRQ_ID);
        free_irq(MT_WF_HIF_IRQ_ID, prNetDevice);
        return -1;
    }
#endif /*                  */

    return ret;

} /*                      */


/*                                                                            */
/* 
                                                                            
 
                                                       
                                                         
 
                
*/
/*                                                                            */
VOID
glBusFreeIrq (
    PVOID pvData,
    PVOID pvCookie
    )
{
    struct net_device *prNetDevice;
    GLUE_INFO_T *GlueInfo;
    GL_HIF_INFO_T *HifInfo;


    /*      */
    ASSERT(pvData);
    if (!pvData) {
        //                                                           
        return;
    }

    prNetDevice = (struct net_device *) pvData;
    GlueInfo = (GLUE_INFO_T *) pvCookie;
    ASSERT(GlueInfo);
    if (!GlueInfo) {
        //                                                            
        return;
    }

    HifInfo = &GlueInfo->rHifInfo;


    /*              */
    free_irq(MT_WF_HIF_IRQ_ID, prNetDevice);
    return;

} /*                      */
#endif


/*                                                                            */
/* 
                                      
 
                                                           
                                      
                                                                    
 
                                         
                                      
*/
/*                                                                            */
BOOLEAN
kalDevRegRead (
    IN  GLUE_INFO_T     *GlueInfo,
    IN  UINT_32         RegOffset,
    OUT UINT_32         *pu4Value
    )
{
    GL_HIF_INFO_T *HifInfo;


    /*                       */
    ASSERT(GlueInfo);
    ASSERT(pu4Value);
    HifInfo = &GlueInfo->rHifInfo;

    /*                               */
    *pu4Value = HIF_REG_READL(HifInfo, RegOffset);

    if ((RegOffset == MCR_WRDR0) || (RegOffset == MCR_WRDR1))
    {
        HIF_DBG(("[WiFi/HIF] kalDevRegRead from Data Port 0 or 1\n"));
    }

    return TRUE;

} /*                        */


/*                                                                            */
/* 
                                       
 
                                                             
                                       
                                              
 
                                         
                                      
*/
/*                                                                            */
BOOLEAN
kalDevRegWrite (
    IN GLUE_INFO_T      *GlueInfo,
    IN UINT_32          RegOffset,
    IN UINT_32          RegValue
    )
{
    GL_HIF_INFO_T *HifInfo;


    /*                       */
    ASSERT(GlueInfo);
    HifInfo = &GlueInfo->rHifInfo;

    /*                                */
    HIF_REG_WRITEL(HifInfo, RegOffset, RegValue);

    if ((RegOffset == MCR_WTDR0) || (RegOffset == MCR_WTDR1))
    {
        HIF_DBG(("[WiFi/HIF] kalDevRegWrite to Data Port 0 or 1\n"));
    }

    return TRUE;

} /*                         */


/*                                                                            */
/* 
                             
 
                                                             
                                       
                                         
                                              
                                                                 
 
                                         
                                      
*/
/*                                                                            */
BOOLEAN
kalDevPortRead (
    IN  GLUE_INFO_T     *GlueInfo,
    IN  UINT_16         Port,
    IN  UINT_16         Size,
    OUT PUINT_8         Buf,
    IN  UINT_16         MaxBufSize
    )
{
    GL_HIF_INFO_T *HifInfo;
#if (CONF_MTK_AHB_DMA == 1)
    MTK_WCN_HIF_DMA_CONF DmaConf;
    UINT_32 LoopCnt;
#endif /*                  */
    UINT_32 IdLoop, MaxLoop;
    UINT_32 *LoopBuf;
    unsigned long PollTimeout;


#if DBG
    //                                                                                                 
#endif

    /*              */
    if ((WlanDmaFatalErr == 1) || (fgIsResetting == TRUE) ||
		(HifIsFwOwn(GlueInfo->prAdapter) == TRUE))
    {
//                           
#if 0
		/*                         */
        if (WlanDmaFatalErr == 1)
        {
            /*                                 */
            static int testrx = 0;
            HifInfo = &GlueInfo->rHifInfo;
            if (testrx ++ >= 20)
            {
                testrx = 0;
                goto LabelErr;
            }
        }
#endif /*                      */

        return TRUE;
    }

    /*      */
    ASSERT(GlueInfo);
    HifInfo = &GlueInfo->rHifInfo;

    ASSERT(Buf);
    ASSERT(Size <= MaxBufSize);

    /*                                                           */
    if (Port == MCR_WRDR0)
        HifAhbDmaEnhanceModeConf(GlueInfo, HIF_BURST_4DW, HIF_TARGET_RXD0, Size);
    else if (Port == MCR_WRDR1)
        HifAhbDmaEnhanceModeConf(GlueInfo, HIF_BURST_4DW, HIF_TARGET_RXD1, Size);
    else if (Port == MCR_WHISR)
        HifAhbDmaEnhanceModeConf(GlueInfo, HIF_BURST_4DW, HIF_TARGET_WHISR, Size);
    /*                          */


    /*      */
#if (CONF_MTK_AHB_DMA == 1)
    if ((HifInfo->fgDmaEnable == TRUE) &&
        (HifInfo->DmaOps != NULL) &&
        ((Port == MCR_WRDR0) || (Port == MCR_WRDR1)))
    {
        /*                    */
#ifdef MTK_DMA_BUF_MEMCPY_SUP
        VOID *DmaVBuf = NULL, *DmaPBuf = NULL;
#endif /*                        */

        /*                                           */
        DmaConf.Count = Size;
        DmaConf.Dir = HIF_DMA_DIR_RX;
        DmaConf.Src = HIF_DRV_BASE + Port; /*                       */

        /*                                                                  
                                            */

#ifdef MTK_DMA_BUF_MEMCPY_SUP
        DmaConf.Dst = kalIOPhyAddrGet(Buf); /*                       */

        /*                          */
        if (DmaConf.Dst == NULL)
        {
            HIF_DBG(("[WiFi/HIF] Use Dma Buffer to RX packet (%d %d)...\n",
                    Size, CFG_RX_MAX_PKT_SIZE));
            ASSERT(Size <= CFG_RX_MAX_PKT_SIZE);

            kalDmaBufGet(&DmaVBuf, &DmaPBuf);
            DmaConf.Dst = (ULONG)DmaPBuf;
        }
#else
        /*
                                                           
                                                                                                  

                                                                                           
                                                                                         
                                                                                             

                                                                                                 
                                                                                                 
                                                                                               
                                                
        */
        /*                                                           */
        /*                                             */
        DmaConf.Dst = dma_map_single(HifInfo->Dev, Buf, Size, DMA_FROM_DEVICE);
#endif /*                        */

        /*                    */
        AP_DMA_HIF_LOCK(HifInfo); /*                                       */

        if (pfWlanDmaOps != NULL)
            pfWlanDmaOps->DmaClockCtrl(TRUE);

        HifInfo->DmaOps->DmaConfig(HifInfo, &DmaConf);
        HifInfo->DmaOps->DmaStart(HifInfo);

        /*                          */
        /*                                */
        /*                     */
#if (CONF_HIF_DMA_INT == 1)
        RtnVal = wait_event_interruptible_timeout(
                    HifInfo->HifDmaWaitq, (HifInfo->HifDmaWaitFlg != 0), 1000);
        if (RtnVal <= 0)
        {
            while(1)
            printk("fatal error1! reset DMA!\n");
        }
        HifInfo->HifDmaWaitFlg = 0;
#else

        LoopCnt = 0;
        PollTimeout = jiffies + HZ*5;

        do {
#if 0
            if (LoopCnt++ > 100000) {
#else
            if (time_before(jiffies, PollTimeout))
                ; /*                               */
            else
            {
#endif

#if (CONF_HIF_CONNSYS_DBG == 0)
                /*                             */
                printk("fatal error1! reset DMA!\n");
                break;
#else
                /*
                                                                       
                
                                                                                               
                                                                                                 
                                                                                                       
                    */
				printk("DMA LoopCnt > 100000... (%lu %lu)\n", jiffies, PollTimeout);
//         
                HifRegDump(GlueInfo->prAdapter);
                if (HifInfo->DmaOps->DmaRegDump != NULL)
                    HifInfo->DmaOps->DmaRegDump(HifInfo);

                LoopCnt = 0;
                WlanDmaFatalErr = 1;

#if (CONF_HIF_DMA_DBG == 0)
{
//                                     
                UINT_32 RegValChip, RegValLP, FwCnt;
                extern BOOLEAN mtk_wcn_wmt_assert(void);
                printk("CONNSYS FW CPUINFO:\n");
                for(FwCnt=0; FwCnt<512; FwCnt++)
                    printk("0x%08x ", MCU_REG_READL(HifInfo, CONN_MCU_CPUPCR)); //                                   
                printk("\n\n");
                kalDevRegRead(GlueInfo, 0x00, &RegValChip);
                kalDevRegRead(GlueInfo, 0x04, &RegValLP);
                if ((RegValChip != 0) && (RegValLP == 0))
                {
                    /*                                                 */
                    /*                                                  */
                    printk("<WLANRXERR><vend_samp.lin> 0x%x 0x%x 0x%x\n",
                        *(volatile unsigned int *)0xF0000024,
                        (UINT_32)RegValChip, (UINT_32)RegValLP);
                    return TRUE;
                }
                printk("<WLANRXERR><vend_samp.lin> 0x%x 0x%x 0x%x\n",
                    *(volatile unsigned int *)0xF0000024,
                    (UINT_32)RegValChip, (UINT_32)RegValLP);

#if 0
                mtk_wcn_wmt_assert();
                sprintf(AeeBuffer, "<WLANRXERR><vend_samp.lin> 0x%x 0x%x 0x%x",
                        *(volatile unsigned int *)0xF0000024,
                        (UINT_32)RegValChip, (UINT_32)RegValLP);
                kalSendAeeWarning(AeeBuffer, "", "");

				msleep(5000);
                /*                             */
                MaxLoop = Size >> 2;
                if (Size & 0x3)
                    MaxLoop ++;
                LoopBuf = (UINT_32 *)Buf;

                printk("PIO content =\n");
                for(IdLoop=0; IdLoop<MaxLoop; IdLoop++) {

                    *LoopBuf = HIF_REG_READL(HifInfo, Port);
                    printk("0x%0x ", *LoopBuf);
                    LoopBuf ++;
                }
                printk("\n\n");

                /*                       */
                HifRegDump(GlueInfo->prAdapter);
#endif
                return TRUE;
}
#endif /*                  */
#endif /*                      */
            }
        } while(!HifInfo->DmaOps->DmaPollIntr(HifInfo));
#endif /*                  */

        HifInfo->DmaOps->DmaAckIntr(HifInfo);
        HifInfo->DmaOps->DmaStop(HifInfo);

        LoopCnt = 0;
        do {
            if (LoopCnt++ > 100000) {
                /*                             */
                printk("fatal error2! reset DMA!\n");
                break;
            }
        } while(HifInfo->DmaOps->DmaPollStart(HifInfo) != 0);

        if (pfWlanDmaOps != NULL)
            pfWlanDmaOps->DmaClockCtrl(FALSE);

        AP_DMA_HIF_UNLOCK(HifInfo);

#ifdef MTK_DMA_BUF_MEMCPY_SUP
        if (DmaVBuf != NULL)
            kalMemCopy(Buf, DmaVBuf, Size);
#else
        dma_unmap_single(HifInfo->Dev, DmaConf.Dst, Size, DMA_FROM_DEVICE);
#endif /*                        */

        HIF_DBG(("[WiFi/HIF] DMA RX OK!\n"));
    }
    else
#endif /*                  */
    {

        /*                  */
        MaxLoop = Size >> 2;
        if (Size & 0x3)
            MaxLoop ++;
        LoopBuf = (UINT_32 *)Buf;

        for(IdLoop=0; IdLoop<MaxLoop; IdLoop++) {

            *LoopBuf = HIF_REG_READL(HifInfo, Port);
            LoopBuf ++;
        }
    }

    return TRUE;

} /*                         */


/*                                                                            */
/* 
                              
 
                                                             
                                       
                                          
                                               
                                                                 
 
                                         
                                      
*/
/*                                                                            */
BOOLEAN
kalDevPortWrite (
    IN GLUE_INFO_T      *GlueInfo,
    IN UINT_16          Port,
    IN UINT_16          Size,
    IN PUINT_8          Buf,
    IN UINT_16          MaxBufSize
    )
{
    GL_HIF_INFO_T *HifInfo;
#if (CONF_MTK_AHB_DMA == 1)
    MTK_WCN_HIF_DMA_CONF DmaConf;
    UINT_32 LoopCnt;
#endif /*                  */
    UINT_32 IdLoop, MaxLoop;
    UINT_32 *LoopBuf;
//                            
//                    
    unsigned long PollTimeout;


#if DBG
    //                                                                                                  
#endif
//                                                                                           

    /*              */
    if ((WlanDmaFatalErr == 1) || (fgIsResetting == TRUE) ||
		(HifIsFwOwn(GlueInfo->prAdapter) == TRUE))
    {
//                               
#if 0
		/*                         */
        if (WlanDmaFatalErr == 1)
        {
            /*                                 */
            static int testtx = 0;
            HifInfo = &GlueInfo->rHifInfo;
            if (testtx ++ >= 20)
            {
                testtx = 0;
                goto LabelErr;
            }
        }
#endif /*                      */

        return TRUE;
    }

    /*      */
#ifdef HIF_DEBUG_SUP_TX
//                           
#endif /*                  */

    ASSERT(GlueInfo);
    HifInfo = &GlueInfo->rHifInfo;

    ASSERT(Buf);
    ASSERT(Size <= MaxBufSize);

    HifTxCnt ++;

    /*                                                           */
    if (Port == MCR_WTDR0)
        HifAhbDmaEnhanceModeConf(GlueInfo, HIF_BURST_4DW, HIF_TARGET_TXD0, Size);
    else if (Port == MCR_WTDR1)
        HifAhbDmaEnhanceModeConf(GlueInfo, HIF_BURST_4DW, HIF_TARGET_TXD1, Size);
    /*                          */


    /*       */
#if (CONF_MTK_AHB_DMA == 1)
    if ((HifInfo->fgDmaEnable == TRUE) &&
        (HifInfo->DmaOps != NULL) &&
        ((Port == MCR_WTDR0) || (Port == MCR_WTDR1)))
    {
        /*                    */
#ifdef MTK_DMA_BUF_MEMCPY_SUP
        VOID *DmaVBuf = NULL, *DmaPBuf = NULL;
#endif /*                        */

        /*             */
        HIF_DBG_TX(("[WiFi/HIF/DMA] Prepare to send data...\n"));
        DmaConf.Count = Size;
        DmaConf.Dir = HIF_DMA_DIR_TX;
        DmaConf.Dst = HIF_DRV_BASE + Port; /*                       */

#ifdef MTK_DMA_BUF_MEMCPY_SUP
        DmaConf.Src = kalIOPhyAddrGet(Buf); /*                       */

        /*                          */
        if (DmaConf.Src == NULL)
        {
            HIF_DBG_TX(("[WiFi/HIF] Use Dma Buffer to TX packet (%d %d)...\n",
                    Size, CFG_RX_MAX_PKT_SIZE));
            ASSERT(Size <= CFG_RX_MAX_PKT_SIZE);

            kalDmaBufGet(&DmaVBuf, &DmaPBuf);
            DmaConf.Src = (ULONG)DmaPBuf;

            kalMemCopy(DmaVBuf, Buf, Size);
        }
#else

        /*
                                                           
                                                                                                  

                                                                                           
                                                                                         
                                                                                             

                                                                                                 
                                                                                                 
                                                                                               
                                                
        */
        /*                                   */
        DmaConf.Src = dma_map_single(HifInfo->Dev, Buf, Size, DMA_TO_DEVICE);
#endif /*                        */

        /*                */
        AP_DMA_HIF_LOCK(HifInfo);

        if (pfWlanDmaOps != NULL)
            pfWlanDmaOps->DmaClockCtrl(TRUE);

#if 0 //                       
{
extern int testgdmaclock;
if (testgdmaclock == 1)
{
    *(volatile unsigned int *)0xF0000084 |= 4; /*        */
    *(volatile unsigned int *)0xF0003010 |= 0x800; /*        */
    printk("enable GDMA clock...\n");
    testgdmaclock = 0;
}
}
#endif

        HifInfo->DmaOps->DmaConfig(HifInfo, &DmaConf);
        HifInfo->DmaOps->DmaStart(HifInfo);

        /*                          */
        /*                                */
        /*                     */
#if (CONF_HIF_DMA_INT == 1)
        RtnVal = wait_event_interruptible_timeout(
                    HifInfo->HifDmaWaitq, (HifInfo->HifDmaWaitFlg != 0), 1000);
        if (RtnVal <= 0)
        {
//                                            
//                                                    
//                                                     
//                              
            while(1)
            printk("fatal error1! reset DMA!\n");
        }
        HifInfo->HifDmaWaitFlg = 0;
#else

        LoopCnt = 0;
        PollTimeout = jiffies + HZ*5;

        do {
#if 0
            if (LoopCnt++ > 10000) {
#else
            if (time_before(jiffies, PollTimeout))
                ; /*                               */
            else
            {
#endif

#if (CONF_HIF_CONNSYS_DBG == 0)
                /*                             */
                printk("fatal error1! reset DMA!\n");
                break;
#else

				printk("DMA LoopCnt > 100000... (%lu %lu)\n", jiffies, PollTimeout);
//         
                HifRegDump(GlueInfo->prAdapter);
                if (HifInfo->DmaOps->DmaRegDump != NULL)
                    HifInfo->DmaOps->DmaRegDump(HifInfo);
                LoopCnt = 0;

                WlanDmaFatalErr = 1;

#if (CONF_HIF_DMA_DBG == 0)
{
//                                     
                UINT_32 RegValChip, RegValLP, FwCnt;
                extern BOOLEAN mtk_wcn_wmt_assert(void);
                printk("CONNSYS FW CPUINFO:\n");
                for(FwCnt=0; FwCnt<512; FwCnt++)
                    printk("0x%08x ", MCU_REG_READL(HifInfo, CONN_MCU_CPUPCR));
                printk("\n\n");
                kalDevRegRead(GlueInfo, 0x00, &RegValChip);
                kalDevRegRead(GlueInfo, 0x04, &RegValLP);
                if ((RegValChip != 0) && (RegValLP == 0))
                {
                    /*                                                 */
                    /*                                                  */
                    printk("<WLANTXERR><vend_samp.lin> 0x%x 0x%x 0x%x\n",
                        *(volatile unsigned int *)0xF0000024,
                        (UINT_32)RegValChip, (UINT_32)RegValLP);
                    return TRUE;
                }
                printk("<WLANTXERR><vend_samp.lin> 0x%x 0x%x 0x%x\n",
                    *(volatile unsigned int *)0xF0000024,
                    (UINT_32)RegValChip, (UINT_32)RegValLP);
#if 0
                mtk_wcn_wmt_assert();
                sprintf(AeeBuffer, "<WLANTXERR><vend_samp.lin> 0x%x 0x%x 0x%x",
                        *(volatile unsigned int *)0xF0000024,
                        (UINT_32)RegValChip, (UINT_32)RegValLP);
                kalSendAeeWarning(AeeBuffer, "", "");
#endif
                return TRUE;
}
#endif /*                  */
#endif /*                      */
            }
        } while(!HifInfo->DmaOps->DmaPollIntr(HifInfo));
#endif /*                  */

        HifInfo->DmaOps->DmaAckIntr(HifInfo);
        HifInfo->DmaOps->DmaStop(HifInfo);

        LoopCnt = 0;
        do {
            if (LoopCnt++ > 100000) {
                /*                             */
                printk("fatal error2! reset DMA!\n");
                break;
            }
        } while(HifInfo->DmaOps->DmaPollStart(HifInfo) != 0);

        if (pfWlanDmaOps != NULL)
            pfWlanDmaOps->DmaClockCtrl(FALSE);

        AP_DMA_HIF_UNLOCK(HifInfo);

#ifndef MTK_DMA_BUF_MEMCPY_SUP
        dma_unmap_single(HifInfo->Dev, DmaConf.Src, Size, DMA_TO_DEVICE);
#endif /*                        */

        HIF_DBG_TX(("[WiFi/HIF] DMA TX OK!\n"));
    }
    else
#endif /*                  */
    {
        /*          */
        MaxLoop = Size >> 2;
        LoopBuf = (UINT_32 *)Buf;

        HIF_DBG_TX(("[WiFi/HIF/PIO] Prepare to send data (%d 0x%p-0x%p)...\n",
                Size, LoopBuf, (((UINT8 *)LoopBuf) + (Size & (~0x03)))));

        if (Size & 0x3)
            MaxLoop ++;

        for(IdLoop=0; IdLoop<MaxLoop; IdLoop++) {

//                                              

            HIF_REG_WRITEL(HifInfo, Port, *LoopBuf);
            LoopBuf ++;
        }

        /*                                            */
        /*                                     */
#if 0
        if (Size & 0x3) {
            /*                                        */
            *((UINT_32 *)Last4B) = 0;
            BufLast = Last4B;
            BufSrc = ((UINT8 *)LoopBuf);

            for(IdLoop=0; IdLoop<(Size & 0x3); IdLoop++) {
                *BufLast = *BufSrc;
                BufLast ++;
                BufSrc ++;
            }

            HIF_DBG_TX(("0x%08x ", *((UINT_32 *)Last4B)));
            HIF_REG_WRITEL(HifInfo, Port, *((UINT_32 *)Last4B));
        }
#endif

        HIF_DBG_TX(("\n\n"));
    }

    return TRUE;

} /*                          */


#if 0 /*                   */
/*                                                                            */
/* 
                                                 
 
                                                                   
                                                  
                                                                  
 
                                         
                                      
*/
/*                                                                            */
BOOLEAN
kalDevWriteWithSdioCmd52 (
    IN GLUE_INFO_T      *GlueInfo,
    IN UINT_32          RegOffset,
    IN UINT_8           RegData
    )
{
    GL_HIF_INFO_T *HifInfo;


    printk("kalDevWriteWithSdioCmd52\n");

    ASSERT(GlueInfo);
    HifInfo = &GlueInfo->rHifInfo;

    /*                         */
    HIF_REG_WRITEB(HifInfo, RegOffset, RegData);

    return TRUE;

} /*                                   */
#endif


/*                                                                              
                                                         
                                                                                
*/

/*                                                                            */
/* 
                                                            
 
                                         
 
              
*/
/*                                                                            */
extern UINT_32 IsrCnt, IsrPassCnt;
static irqreturn_t
HifAhbISR(
    IN int                          Irq,
    IN void                         *Arg
    )
{
    struct net_device *prNetDevice = (struct net_device *)Arg;
    GLUE_INFO_T *GlueInfo;
    GL_HIF_INFO_T *HifInfo;


    /*      */
	IsrCnt ++;
    ASSERT(prNetDevice);
    GlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prNetDevice));
    ASSERT(GlueInfo);

    if (!GlueInfo) {
        //                                                          
        return IRQ_HANDLED;
    }
    HifInfo = &GlueInfo->rHifInfo;

    if (GlueInfo->ulFlag & GLUE_FLAG_HALT) {
        HIF_REG_WRITEL(HifInfo, MCR_WHLPCR, WHLPCR_INT_EN_CLR);
        //                                                      
        return IRQ_HANDLED;
    }

    HIF_REG_WRITEL(HifInfo, MCR_WHLPCR, WHLPCR_INT_EN_CLR);

#if 0
    wlanISR(GlueInfo->prAdapter, TRUE);

    if (GlueInfo->u4Flag & GLUE_FLAG_HALT) {
        /*                                           */
        //                                                       
    }
    else {
        wlanIST(GlueInfo->prAdapter);
    }
#endif

    /*                             */
    kalHifAhbKalWakeLockTimeout(GlueInfo);

    /*                     */
    set_bit(GLUE_FLAG_INT_BIT, &GlueInfo->ulFlag);

    /*                                                             */
    wake_up_interruptible(&GlueInfo->waitq);

	IsrPassCnt ++;
    return IRQ_HANDLED;

}


#if (CONF_HIF_DMA_INT == 1)
/*                                                                            */
/* 
                                                            
 
                                         
 
              
*/
/*                                                                            */

static irqreturn_t
HifDmaISR(
    IN int                          Irq,
    IN void                         *Arg
    )
{
    struct net_device *prNetDevice = (struct net_device *)Arg;
    GLUE_INFO_T *GlueInfo;
    GL_HIF_INFO_T *HifInfo;


    /*      */
    ASSERT(prNetDevice);
    GlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prNetDevice));
    ASSERT(GlueInfo);

    if (!GlueInfo) {
        //                                                          
        return IRQ_HANDLED;
    }
    HifInfo = &GlueInfo->rHifInfo;

    /*                   */
    HifInfo->DmaOps->DmaAckIntr(HifInfo);

    /*                     */
    set_bit(1, &HifInfo->HifDmaWaitFlg);

    /*                                                             */
    wake_up_interruptible(&HifInfo->HifDmaWaitq);

    return IRQ_HANDLED;

}
#endif /*                  */


/*                                                                            */
/* 
                                               
 
                                          
                                                   
 
              
*/
/*                                                                            */
#include <mach/mt_gpio.h>

static int
HifAhbProbe (
    VOID
    )
{
    int Ret = 0;


    printk(KERN_INFO DRV_NAME "HifAhbProbe()\n");

//                

    /*                                             */
{
#ifdef CONFIG_MTK_PMIC_MT6397
#ifdef MTK_EXTERNAL_LDO
	/*                 */
	mt_set_gpio_mode(GPIO51, GPIO_MODE_04); 
	mt_set_gpio_dir(GPIO51, GPIO_DIR_OUT); 
	mt_set_gpio_pull_enable(GPIO51, GPIO_PULL_ENABLE);
	mt_set_gpio_pull_select(GPIO51, GPIO_PULL_UP);
#elif defined(MTK_ALPS_BOX_SUPPORT)
	/*              */
	mt_set_gpio_mode(GPIO89, GPIO_MODE_04); 
	mt_set_gpio_dir(GPIO89, GPIO_DIR_OUT); 
	mt_set_gpio_pull_enable(GPIO89, GPIO_PULL_ENABLE);
	mt_set_gpio_pull_select(GPIO89, GPIO_PULL_UP);
#else
	hwPowerOn(MT65XX_POWER_LDO_VGP4,VOL_3300,"WLAN");
#endif
#else
#ifdef CONFIG_OF  /*          */
    extern INT_32 mtk_wcn_consys_hw_wifi_paldo_ctrl(UINT_32 enable);
    //                                                    
    mtk_wcn_consys_hw_wifi_paldo_ctrl(1); /*                   */
#else  /*                */
    extern void upmu_set_vcn33_on_ctrl_wifi(UINT_32 val);
    hwPowerOn(MT6323_POWER_LDO_VCN33_WIFI, VOL_3300, "WLAN");
    upmu_set_vcn33_on_ctrl_wifi(1); /*                   */
#endif
#endif

//                                            
//                             
//                                         
}

#if (CONF_HIF_DEV_MISC == 1)
    if (pfWlanProbe((PVOID)&MtkAhbDriver.this_device) != WLAN_STATUS_SUCCESS) {
#else
    if (pfWlanProbe((PVOID)&HifAhbPDev->dev) != WLAN_STATUS_SUCCESS) {
#endif /*                   */

        //                                                                      

        pfWlanRemove();
        Ret = -1;
    }

    return Ret;

}


/*                                                                            */
/* 
                                             
 
                 
 
                                                        
*/
/*                                                                            */
static int
HifAhbRemove (
    VOID
    )
{
    printk(KERN_INFO DRV_NAME"HifAhbRemove()\n");

//                
    pfWlanRemove();

{
#ifdef CONFIG_MTK_PMIC_MT6397
#ifdef MTK_EXTERNAL_LDO
	/*                 */
	mt_set_gpio_mode(GPIO51, GPIO_MODE_04); 
	mt_set_gpio_dir(GPIO51, GPIO_DIR_OUT); 
	mt_set_gpio_pull_enable(GPIO51, GPIO_PULL_ENABLE);
	mt_set_gpio_pull_select(GPIO51, GPIO_PULL_DOWN);
#elif defined(MTK_ALPS_BOX_SUPPORT)
	/*              */
	mt_set_gpio_mode(GPIO89, GPIO_MODE_04); 
	mt_set_gpio_dir(GPIO89, GPIO_DIR_OUT); 
	mt_set_gpio_pull_enable(GPIO89, GPIO_PULL_ENABLE);
	mt_set_gpio_pull_select(GPIO89, GPIO_PULL_DOWN);
#else
	hwPowerDown(MT65XX_POWER_LDO_VGP4,"WLAN");
#endif
#else
#ifdef CONFIG_OF  /*          */
    extern INT_32 mtk_wcn_consys_hw_wifi_paldo_ctrl(UINT_32 enable);
    mtk_wcn_consys_hw_wifi_paldo_ctrl(0); /*                   */
    //                                            
#else  /*                */
    extern void upmu_set_vcn33_on_ctrl_wifi(UINT_32 val);
    upmu_set_vcn33_on_ctrl_wifi(0); /*                   */
    hwPowerDown(MT6323_POWER_LDO_VCN33_WIFI, "WLAN");
#endif
#endif

//                                             
//                             
//                                          
}

//                                                      
    return 0;

}


#if (MTK_WCN_SINGLE_MODULE == 0)
/*                                                                            */
/* 
                                                                  
 
                 
 
                  
*/
/*                                                                            */
static int
HifAhbBusCntGet(
    VOID
    )
{
    return HifTxCnt;
}


/*                                                                            */
/* 
                                                                    
 
                 
 
           
*/
/*                                                                            */
static int
HifAhbBusCntClr(
    VOID
    )
{
    HifTxCnt = 0;
    return 0;
}
#endif /*                       */


/*                                                                            */
/* 
                                                                            
 
                                                                 
                                                                    
                                                                                
                                                   
 
              
*/
/*                                                                            */
static void
HifAhbDmaEnhanceModeConf(
    IN GLUE_INFO_T      *GlueInfo,
    UINT_32              BurstLen,
    UINT_32              PortId,
    UINT_32              TransByte
    )
{
    GL_HIF_INFO_T *HifInfo;
	UINT_32 RegHSTCR;


    ASSERT(GlueInfo);
    HifInfo = &GlueInfo->rHifInfo;

    /*
                
                            
                                                                  
                                                                     
                               
                 
                                          
    */
	RegHSTCR = HIF_REG_READL(HifInfo, MCR_WHIER);

	RegHSTCR = HIF_REG_READL(HifInfo, MCR_HSTCR);
	RegHSTCR = \
		((BurstLen << HSTCR_AFF_BURST_LEN_OFFSET ) & HSTCR_AFF_BURST_LEN) | \
		((PortId << HSTCR_TRANS_TARGET_OFFSET ) & HSTCR_TRANS_TARGET) | \
		(((TransByte & 0x3) == 0)?(TransByte & HSTCR_HSIF_TRANS_CNT): \
                                    ((TransByte + 4) & HSTCR_HSIF_TRANS_CNT));
	HIF_REG_WRITEL(HifInfo, MCR_HSTCR, RegHSTCR);
	
//                                              
//                                                   

}


VOID
glSetPowerState (
    IN GLUE_INFO_T  *GlueInfo,
    IN UINT_32      ePowerMode
    )
{
    return;
}


#if (CONF_HIF_DEV_MISC == 1)
/*        */
static ssize_t 
HifAhbMiscRead(
    IN struct file              *Filp,
    OUT char __user             *DstBuf,
    IN size_t                   Size,
    IN loff_t                   *Ppos
    )
{
    return 0;
}


static ssize_t 
HifAhbMiscWrite(
    IN struct file              *Filp,
    IN const char __user        *SrcBuf,
    IN size_t                   Size,
    IN loff_t                   *Ppos
    )
{
    return 0;
}


static int
HifAhbMiscIoctl(
    IN struct file              *Filp,
    IN unsigned int             Cmd,
    IN unsigned long            arg
    )
{
    return 0;
}


static int
HifAhbMiscOpen(
    IN struct inode             *Inodep,
    IN struct file              *Filp
    )
{
    return 0;
}


static int
HifAhbMiscClose(
    IN struct inode             *Inodep,
    IN struct file              *Filp
    )
{
    return 0;
}
#else

/*                                                                            */
/* 
                                                              
 
                                                                     
 
           
*/
/*                                                                            */
static int
HifAhbPltmProbe(
    IN struct platform_device       *PDev
    )
{
    /*
                             
                               

                                   
                             

                                                         
                                                                    
                              
                                                           
                        
          
         
                                                                 
                   
                                                          
                        
          

                                        
                       
                                                                          
                     
          
    */
    HifAhbPDev = PDev;

    printk("HifAhbPltmProbe\n");

#if (CONF_HIF_PMIC_TEST == 1)
    wmt_set_jtag_for_mcu();
    wmt_set_jtag_for_gps();

{
//                                                                           
//                                     
}
#endif /*                    */

#if (MTK_WCN_SINGLE_MODULE == 1)
    HifAhbProbe(); /*                                          */

#else

    /*                               */
    printk("mtk_wcn_wmt_wlan_reg\n");
{
    MTK_WCN_WMT_WLAN_CB_INFO WmtCb;
    WmtCb.wlan_probe_cb = HifAhbProbe;
    WmtCb.wlan_remove_cb = HifAhbRemove;
    WmtCb.wlan_bus_cnt_get_cb = HifAhbBusCntGet;
    WmtCb.wlan_bus_cnt_clr_cb = HifAhbBusCntClr;
    mtk_wcn_wmt_wlan_reg(&WmtCb);
}
#endif /*                       */
    return 0;
}


/*                                                                            */
/* 
                                                              
 
                                                                     
 
           
*/
/*                                                                            */
static int __exit
HifAhbPltmRemove(
    IN struct platform_device       *PDev
    )
{
#if (MTK_WCN_SINGLE_MODULE == 0)
    mtk_wcn_wmt_wlan_unreg();
#endif /*                       */

#if (CONF_HIF_PMIC_TEST == 1)
{
//                                                                           
//                                     
}
#endif /*                    */

    return 0;
}


#ifdef CONFIG_PM
/*                                                                            */
/* 
                                                              
 
                                                                     
                            
 
           
*/
/*                                                                            */
static int
HifAhbPltmSuspend(
    IN struct platform_device       *PDev,
    pm_message_t                    Message
    )
{
    return 0;
}


/*                                                                            */
/* 
                                                              
 
                                                                     
 
           
*/
/*                                                                            */
static int
HifAhbPltmResume(
    IN struct platform_device       *PDev
    )
{
    return 0;
}
#endif /*           */

#endif /*                   */



#if (CONF_HIF_LOOPBACK_AUTO == 1)
/*                                                                            */
/* 
                                         
 
                                                        
 
              
*/
/*                                                                            */
static VOID
HifAhbLoopbkAuto(
    IN unsigned long            arg
    )
{

    P_GLUE_INFO_T GlueInfo = (P_GLUE_INFO_T) arg;
    GL_HIF_INFO_T *HifInfo = &GlueInfo->rHifInfo;


    ASSERT(GlueInfo);

    HIF_DBG(("[WiFi/HIF] Trigger to do loopback test...\n"));

    /*                                     */
//                                                              
//                                            

    set_bit(GLUE_FLAG_HIF_LOOPBK_AUTO_BIT, &HifInfo->HifLoopbkFlg);
    wake_up_interruptible(&HifInfo->HifWaitq);

    return;
}


#endif /*                        */

VOID glDumpConnSysCpuInfo(P_GLUE_INFO_T prGlueInfo) {
	GL_HIF_INFO_T *prHifInfo = &prGlueInfo->rHifInfo;
	unsigned short j = 0;
	for(; j<512; j++)
       printk("0x%08x \n", MCU_REG_READL(prHifInfo, CONN_MCU_CPUPCR)); //                                   
}

/*              */
