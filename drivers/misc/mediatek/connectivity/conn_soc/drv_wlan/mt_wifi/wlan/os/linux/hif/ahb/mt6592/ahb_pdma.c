/******************************************************************************
*[File]             ahb_pdma.c
*[Version]          v1.0
*[Revision Date]    2013-03-13
*[Author]
*[Description]
*    The program provides AHB PDMA driver
*[Copyright]
*    Copyright (C) 2013 MediaTek Incorporation. All Rights Reserved.
******************************************************************************/



/*
                     
  
                           
                       
                     
  
*/

/*                                                                              
                                                     
                                                                                
*/

/*                                                                              
                                                          
                                                                                
*/

#define MODULE_AHB_DMA

#include <linux/version.h>      /*                            */

#include <linux/kernel.h>       /*          */

#include <linux/timer.h>        /*                   */
#include <linux/jiffies.h>      /*         */
#include <linux/delay.h>        /*                         */

#if CONFIG_ANDROID
#include <linux/wakelock.h>
#endif

#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 12)
#include <linux/irq.h>          /*              */
#endif

#include <linux/netdevice.h>    /*                                            */
#include <linux/etherdevice.h>  /*                               */
#include <linux/wireless.h>     /*                      */
#include <linux/if_arp.h>
#include <linux/inetdevice.h>   /*                  */

#include <linux/ip.h>           /*              */

#include <linux/string.h>       /*                                */
#include <linux/stddef.h>       /*                      */

#include <linux/proc_fs.h>      /*                                          */

#include <linux/rtnetlink.h>    /*                                   */
#include <linux/kthread.h>      /*                                      */
#include <asm/uaccess.h>        /*                      */
#include <linux/fs.h>           /*                       */
#include <linux/vmalloc.h>

#include <linux/kfifo.h>        /*                     */
#include <linux/cdev.h>         /*                    */

#include <linux/firmware.h>     /*                       */

#include <linux/random.h>


#include <asm/io.h>             /*                  */

#include <linux/module.h>

#include <mach/mt_clkmgr.h>

#include "hif.h"
#include "hif_pdma.h"

#include <mach/emi_mpu.h>

//                           

//                      

#ifdef PDMA_DEBUG_SUP
#define PDMA_DBG(msg)   printk msg
#else
#define PDMA_DBG(msg)
#endif /*                */

static UINT_32 gDmaReg[AP_DMA_HIF_0_LENGTH/4+1];


/*                                                                              
                                                
                                                                                
*/


/*                                                                              
                                                 
                                                                                
*/


/*                                                                              
                                             
                                                                                
*/


/*                                                                              
                                                             
                                                                                
*/
static VOID
HifPdmaConfig (
    IN void                     *HifInfoSrc,
    IN void                     *Conf
    );

static VOID
HifPdmaStart(
    IN void                     *HifInfoSrc
    );

static VOID
HifPdmaStop(
    IN void                     *HifInfoSrc
    );

static MTK_WCN_BOOL
HifPdmaPollStart(
    IN void                     *HifInfoSrc
    );

static MTK_WCN_BOOL
HifPdmaPollIntr(
    IN void                     *HifInfoSrc
    );

static VOID
HifPdmaAckIntr(
    IN void                     *HifInfoSrc
    );


static VOID
HifPdmaClockCtrl(
    IN UINT_32                   FlgIsEnabled
    );

static VOID
HifPdmaRegDump(
    IN void                     *HifInfoSrc
    );

static VOID
HifPdmaReset(
    IN void                     *HifInfoSrc
    );


/*                                                                              
                                                  
                                                                                
*/


/*                                                                              
                                                   
                                                                                
*/
GL_HIF_DMA_OPS_T HifPdmaOps = {
    .DmaConfig = HifPdmaConfig,
    .DmaStart = HifPdmaStart,
    .DmaStop = HifPdmaStop,
    .DmaPollStart = HifPdmaPollStart,
    .DmaPollIntr = HifPdmaPollIntr,
    .DmaAckIntr = HifPdmaAckIntr,
    .DmaClockCtrl = HifPdmaClockCtrl,
    .DmaRegDump = HifPdmaRegDump,
    .DmaReset = HifPdmaReset
};


/*                                                                              
                                                        
                                                                                
*/

/*                                                                            */
/* 
                           
 
                                                                     
                                                        
 
              
*/
/*                                                                            */
VOID
HifPdmaInit (
    GL_HIF_INFO_T               *HifInfo
    )
{
    extern phys_addr_t gConEmiPhyBase;

    /*                               */
    HifInfo->DmaRegBaseAddr = ioremap(AP_DMA_HIF_BASE, AP_DMA_HIF_0_LENGTH);

    /*                       */
    HifInfo->DmaOps = &HifPdmaOps;

    /*                  */
    HifInfo->fgDmaEnable = TRUE;

    /*                         */
#ifdef MTK_TEE_CCCI_SECURE_SHARE_MEM_SUPPORT
    PDMA_DBG(("WIFI set EMI MPU for TEE project\n"));
    emi_mpu_set_region_protection(gConEmiPhyBase,
        gConEmiPhyBase + SZ_1M/2,
        5,
        SET_ACCESS_PERMISSON(FORBIDDEN,NO_PROTECTION,FORBIDDEN,FORBIDDEN));
#else
    printk("[wlan] WIFI set EMI MPU for non-TEE project\n");
    emi_mpu_set_region_protection(gConEmiPhyBase,
        gConEmiPhyBase + SZ_1M/2,
        4,
        SET_ACCESS_PERMISSON(FORBIDDEN,NO_PROTECTION,FORBIDDEN,FORBIDDEN));
#endif

    PDMA_DBG(("PDMA> HifPdmaInit ok!\n"));
}


/*                                                                              
                                                         
                                                                                
*/

/*                                                                            */
/* 
                           
 
                                                                       
                                                        
 
              
*/
/*                                                                            */
static VOID
HifPdmaConfig (
    IN void                     *HifInfoSrc,
    IN void                     *Param
    )
{
    GL_HIF_INFO_T *HifInfo = (GL_HIF_INFO_T *)HifInfoSrc;
    MTK_WCN_HIF_DMA_CONF *Conf = (MTK_WCN_HIF_DMA_CONF *)Param;
    UINT_32 RegVal;
    

    /*                    */
    Conf->Burst = HIF_PDMA_BURST_4_4; /*                   */
    Conf->Fix_en = FALSE;

    /*                      */
    PDMA_DBG(("PDMA> Conf->Dir = %d\n", Conf->Dir));

    /*                  */
    RegVal = HIF_DMAR_READL(HifInfo, AP_DMA_HIF_0_CON);
    RegVal &= ~(ADH_CR_BURST_LEN | ADH_CR_FIX_EN | ADH_CR_DIR);
    RegVal |= (((Conf->Burst<<ADH_CR_BURST_LEN_OFFSET)&ADH_CR_BURST_LEN) | \
            (Conf->Fix_en<<ADH_CR_FIX_EN_OFFSET) | \
            (Conf->Dir));
    HIF_DMAR_WRITEL(HifInfo, AP_DMA_HIF_0_CON, RegVal);
    PDMA_DBG(("PDMA> AP_DMA_HIF_0_CON = 0x%08x\n", RegVal));

    /*                       */
    HIF_DMAR_WRITEL(HifInfo, AP_DMA_HIF_0_SRC_ADDR, Conf->Src);
    PDMA_DBG(("PDMA> AP_DMA_HIF_0_SRC_ADDR = 0x%08x\n",  Conf->Src));

    /*                       */
    HIF_DMAR_WRITEL(HifInfo, AP_DMA_HIF_0_DST_ADDR, Conf->Dst);
    PDMA_DBG(("PDMA> AP_DMA_HIF_0_DST_ADDR = 0x%08x\n",  Conf->Dst));

    /*                  */
    HIF_DMAR_WRITEL(HifInfo, AP_DMA_HIF_0_LEN, (Conf->Count & ADH_CR_LEN));
    PDMA_DBG(("PDMA> AP_DMA_HIF_0_LEN = %u\n",  (Conf->Count & ADH_CR_LEN)));

}/*                      */


/*                                                                            */
/* 
                          
 
                                                                       
 
              
*/
/*                                                                            */
static VOID
HifPdmaStart(
    IN void                     *HifInfoSrc
    )
{
    GL_HIF_INFO_T *HifInfo = (GL_HIF_INFO_T *)HifInfoSrc;
    UINT_32 RegVal, RegId, RegIdx;


    /*                  */
    RegVal = HIF_DMAR_READL(HifInfo, AP_DMA_HIF_0_INT_EN);
    HIF_DMAR_WRITEL(HifInfo, AP_DMA_HIF_0_INT_EN, (RegVal | ADH_CR_INTEN_FLAG_0));

	/*                            */
	RegIdx = 0;
    for(RegId=0; RegId<AP_DMA_HIF_0_LENGTH; RegId+=4)
    {
        gDmaReg[RegIdx++] = HIF_DMAR_READL(HifInfo, RegId);
	}

    /*           */
    RegVal = HIF_DMAR_READL(HifInfo, AP_DMA_HIF_0_EN);
    HIF_DMAR_WRITEL(HifInfo, AP_DMA_HIF_0_EN, (RegVal | ADH_CR_EN));

    PDMA_DBG(("PDMA> HifPdmaStart...\n"));

} /*                     */


/*                                                                            */
/* 
                         
 
                                                                       
 
              
*/
/*                                                                            */
static VOID
HifPdmaStop(
    IN void                     *HifInfoSrc
    )
{
    GL_HIF_INFO_T *HifInfo = (GL_HIF_INFO_T *)HifInfoSrc;
    UINT_32 RegVal;
//                   


    /*                   */
    RegVal = HIF_DMAR_READL(HifInfo, AP_DMA_HIF_0_INT_EN);
    HIF_DMAR_WRITEL(HifInfo, AP_DMA_HIF_0_INT_EN, (RegVal & ~(ADH_CR_INTEN_FLAG_0)));


#if 0 /*                                */
    /*          */
    RegVal = HIF_DMAR_READL(HifInfo, AP_DMA_HIF_0_STOP);
    HIF_DMAR_WRITEL(HifInfo, AP_DMA_HIF_0_STOP, (RegVal | ADH_CR_STOP));


    /*                             */
    pollcnt = 0;
    do {
        RegVal = HIF_DMAR_READL(HifInfo, AP_DMA_HIF_0_EN);
        if (pollcnt++ > 100000) {
            /*                       */
        }
    } while(RegVal&ADH_CR_EN);
#endif

} /*                    */


/*                                                                            */
/* 
                           
 
                                                                       
 
              
*/
/*                                                                            */
static MTK_WCN_BOOL
HifPdmaPollStart(
    IN void                     *HifInfoSrc
    )
{
    GL_HIF_INFO_T *HifInfo = (GL_HIF_INFO_T *)HifInfoSrc;
	UINT_32 RegVal;


    RegVal = HIF_DMAR_READL(HifInfo, AP_DMA_HIF_0_EN);
	return (((RegVal & ADH_CR_EN) != 0) ? TRUE : FALSE);

} /*                         */


/*                                                                            */
/* 
                              
 
                                                                       
 
              
*/
/*                                                                            */
static MTK_WCN_BOOL
HifPdmaPollIntr(
    IN void                     *HifInfoSrc
    )
{
    GL_HIF_INFO_T *HifInfo = (GL_HIF_INFO_T *)HifInfoSrc;
	UINT_32 RegVal;


	RegVal = HIF_DMAR_READL(HifInfo, AP_DMA_HIF_0_INT_FLAG);
	return (((RegVal & ADH_CR_FLAG_0) != 0) ? TRUE : FALSE);

} /*                        */


/*                                                                            */
/* 
                                     
 
                                                                       
 
              
*/
/*                                                                            */
static VOID
HifPdmaAckIntr(
    IN void                     *HifInfoSrc
    )
{
    GL_HIF_INFO_T *HifInfo = (GL_HIF_INFO_T *)HifInfoSrc;
	UINT_32 RegVal;


	/*                            */
	RegVal = HIF_DMAR_READL(HifInfo, AP_DMA_HIF_0_INT_FLAG);
	HIF_DMAR_WRITEL(HifInfo, AP_DMA_HIF_0_INT_FLAG, (RegVal & ~ADH_CR_FLAG_0));

} /*                       */


/*                                                                            */
/* 
                                     
 
                                                            
 
              
*/
/*                                                                            */
static VOID
HifPdmaClockCtrl(
    IN UINT_32                   FlgIsEnabled
    )
{
    if (FlgIsEnabled == TRUE)
        enable_clock(MT_CG_PERI_AP_DMA, "WLAN");
    else
        disable_clock(MT_CG_PERI_AP_DMA, "WLAN");
}


/*                                                                            */
/* 
                                     
 
                                                                       
 
              
*/
/*                                                                            */
static VOID
HifPdmaRegDump(
    IN void                     *HifInfoSrc
    )
{
    GL_HIF_INFO_T *HifInfo = (GL_HIF_INFO_T *)HifInfoSrc;
    UINT_32 RegId, RegVal, RegIdx;
    UINT_32 RegNum = 0;


    printk("PDMA> Register content before start 0x%x=\n\t", AP_DMA_HIF_BASE);
    for(RegId=0, RegIdx=0; RegId<AP_DMA_HIF_0_LENGTH; RegId+=4)
    {
        RegVal = gDmaReg[RegIdx++];
        printk("0x%08x ", RegVal);

        if (RegNum++ >= 3)
        {
            printk("\n");
            printk("PDMA> Register content 0x%x=\n\t", AP_DMA_HIF_BASE+RegId+4);
            RegNum = 0;
        }
    }

    printk("\nPDMA> Register content after start 0x%x=\n\t", AP_DMA_HIF_BASE);
    for(RegId=0; RegId<AP_DMA_HIF_0_LENGTH; RegId+=4)
    {
        RegVal = HIF_DMAR_READL(HifInfo, RegId);
        printk("0x%08x ", RegVal);

        if (RegNum++ >= 3)
        {
            printk("\n");
            printk("PDMA> Register content 0x%x=\n\t", AP_DMA_HIF_BASE+RegId+4);
            RegNum = 0;
        }
    }

    printk("\nPDMA> clock status = 0x%x\n\n", *(volatile unsigned int *)0xF0003018);
}


/*                                                                            */
/* 
                   
 
                                                                       
 
              
*/
/*                                                                            */
static VOID
HifPdmaReset(
    IN void                     *HifInfoSrc
    )
{
    GL_HIF_INFO_T *HifInfo = (GL_HIF_INFO_T *)HifInfoSrc;
    UINT_32 LoopCnt;


    /*                                                            */
    printk("\nDMA> do warm reset...\n");

    /*                                                                      */

    HIF_DMAR_WRITEL(HifInfo, AP_DMA_HIF_0_RST, 0x01);

    for(LoopCnt=0; LoopCnt<10000; LoopCnt++)
    {
        if (!HifPdmaPollStart(HifInfo))
            break; /*          */
    }

    if (HifPdmaPollStart(HifInfo))
    {
        /*                                        */
        printk("\nDMA> do hard reset...\n");
        HIF_DMAR_WRITEL(HifInfo, AP_DMA_HIF_0_RST, 0x02);
        msleep(1);
        HIF_DMAR_WRITEL(HifInfo, AP_DMA_HIF_0_RST, 0x00);
    }
}

//                             

/*                   */
