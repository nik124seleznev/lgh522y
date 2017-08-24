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
#include "hif_gdma.h"

#if (CONF_MTK_AHB_DMA == 1)

//                      

#ifdef GDMA_DEBUG_SUP
#define GDMA_DBG(msg)   printk msg
#else
#define GDMA_DBG(msg)
#endif /*                */


/*                                                                              
                                                
                                                                                
*/


/*                                                                              
                                                 
                                                                                
*/


/*                                                                              
                                             
                                                                                
*/


/*                                                                              
                                                             
                                                                                
*/
static VOID
HifGdmaConfig (
    IN void                     *HifInfoSrc,
    IN void                     *Conf
    );

static VOID
HifGdmaStart(
    IN void                     *HifInfoSrc
    );

static VOID
HifGdmaStop(
    IN void                     *HifInfoSrc
    );

static MTK_WCN_BOOL
HifGdmaPollStart(
    IN void                     *HifInfoSrc
    );

static MTK_WCN_BOOL
HifGdmaPollIntr(
    IN void                     *HifInfoSrc
    );

static VOID
HifGdmaAckIntr(
    IN void                     *HifInfoSrc
    );

static VOID
HifGdmaClockCtrl(
    IN UINT_32                   FlgIsEnabled
    );

static VOID
HifGdmaRegDump(
    IN void                     *HifInfoSrc
    );

static VOID
HifGdmaReset(
    IN void                     *HifInfoSrc
    );


/*                                                                              
                                                  
                                                                                
*/


/*                                                                              
                                                   
                                                                                
*/
GL_HIF_DMA_OPS_T HifGdmaOps = {
    .DmaConfig = HifGdmaConfig,
    .DmaStart = HifGdmaStart,
    .DmaStop = HifGdmaStop,
    .DmaPollStart = HifGdmaPollStart,
    .DmaPollIntr = HifGdmaPollIntr,
    .DmaAckIntr = HifGdmaAckIntr,
    .DmaClockCtrl = HifGdmaClockCtrl,
    .DmaRegDump = HifGdmaRegDump,
    .DmaReset = HifGdmaReset
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
    /*                               */
    HifInfo->DmaRegBaseAddr = ioremap(AP_DMA_HIF_BASE, AP_DMA_HIF_0_LENGTH);

    /*                       */
    HifInfo->DmaOps = &HifGdmaOps;

    /*                  */
    HifInfo->fgDmaEnable = TRUE;

    GDMA_DBG(("GDMA> HifGdmaInit ok!\n"));
}


/*                                                                              
                                                         
                                                                                
*/

/*                                                                            */
/* 
                           
 
                                                                       
                                                        
 
              
*/
/*                                                                            */
static VOID
HifGdmaConfig (
    IN void                     *HifInfoSrc,
    IN void                     *Param
    )
{
    GL_HIF_INFO_T *HifInfo = (GL_HIF_INFO_T *)HifInfoSrc;
    MTK_WCN_HIF_DMA_CONF *Conf = (MTK_WCN_HIF_DMA_CONF *)Param;
    UINT_32 RegVal;
    

    /*                    */
    Conf->Ratio = HIF_GDMA_RATIO_1;
    Conf->Connect = HIF_GDMA_CONNECT_SET1;
    Conf->Wsize = HIF_GDMA_WRITE_2;
    Conf->Burst = HIF_GDMA_BURST_4_8;
    Conf->Fix_en = TRUE;


    /*                      */
    GDMA_DBG(("GDMA> Conf->Dir = %d\n", Conf->Dir));

    RegVal = HIF_DMAR_READL(HifInfo, AP_P_DMA_G_DMA_2_CON);
    RegVal &= ~(ADH_CR_FLAG_FINISH | ADH_CR_RSIZE | ADH_CR_WSIZE | \
            ADH_CR_BURST_LEN | ADH_CR_WADDR_FIX_EN | ADH_CR_RADDR_FIX_EN);
    if (Conf->Dir == HIF_DMA_DIR_TX)
    {
        RegVal |= (((Conf->Wsize<<ADH_CR_WSIZE_OFFSET)&ADH_CR_WSIZE) | \
                ((Conf->Burst<<ADH_CR_BURST_LEN_OFFSET)&ADH_CR_BURST_LEN) | \
                ((Conf->Fix_en<<ADH_CR_WADDR_FIX_EN_OFFSET)&ADH_CR_WADDR_FIX_EN));
    }
    else
    {
        RegVal |= (((Conf->Wsize<<ADH_CR_RSIZE_OFFSET)&ADH_CR_RSIZE) | \
                ((Conf->Burst<<ADH_CR_BURST_LEN_OFFSET)&ADH_CR_BURST_LEN) | \
                ((Conf->Fix_en<<ADH_CR_RADDR_FIX_EN_OFFSET)&ADH_CR_RADDR_FIX_EN));
    }
    HIF_DMAR_WRITEL(HifInfo, AP_P_DMA_G_DMA_2_CON, RegVal);
    GDMA_DBG(("GDMA> AP_P_DMA_G_DMA_2_CON = 0x%08x\n", RegVal));

    /*                          */
    RegVal = HIF_DMAR_READL(HifInfo, AP_P_DMA_G_DMA_2_CONNECT);
    RegVal &= ~(ADH_CR_RATIO | ADH_CR_DIR | ADH_CR_CONNECT);
    RegVal |= (((Conf->Ratio<<ADH_CR_RATIO_OFFSET)&ADH_CR_RATIO) | \
            ((Conf->Dir<<ADH_CR_DIR_OFFSET)&ADH_CR_DIR) | \
            (Conf->Connect&ADH_CR_CONNECT));
    HIF_DMAR_WRITEL(HifInfo, AP_P_DMA_G_DMA_2_CONNECT, RegVal);
    GDMA_DBG(("GDMA> AP_P_DMA_G_DMA_2_CONNECT = 0x%08x\n", RegVal));

    /*                       */
    HIF_DMAR_WRITEL(HifInfo, AP_P_DMA_G_DMA_2_SRC_ADDR, Conf->Src);
    GDMA_DBG(("GDMA> AP_P_DMA_G_DMA_2_SRC_ADDR = 0x%08x\n",  Conf->Src));

    /*                       */
    HIF_DMAR_WRITEL(HifInfo, AP_P_DMA_G_DMA_2_DST_ADDR, Conf->Dst);
    GDMA_DBG(("GDMA> AP_P_DMA_G_DMA_2_DST_ADDR = 0x%08x\n",  Conf->Dst));

    /*                       */
    HIF_DMAR_WRITEL(HifInfo, AP_P_DMA_G_DMA_2_LEN1, (Conf->Count & ADH_CR_LEN));
    GDMA_DBG(("GDMA> AP_P_DMA_G_DMA_2_LEN1 = %u\n",  (Conf->Count & ADH_CR_LEN)));

}/*                      */


/*                                                                            */
/* 
                          
 
                                                                       
 
              
*/
/*                                                                            */
static VOID
HifGdmaStart(
    IN void                     *HifInfoSrc
    )
{
    GL_HIF_INFO_T *HifInfo = (GL_HIF_INFO_T *)HifInfoSrc;
    UINT_32 RegVal;


    /*                  */
    RegVal = HIF_DMAR_READL(HifInfo, AP_P_DMA_G_DMA_2_INT_EN);
    HIF_DMAR_WRITEL(HifInfo, AP_P_DMA_G_DMA_2_INT_EN, (RegVal | ADH_CR_INTEN_FLAG_0));


    /*           */
    RegVal = HIF_DMAR_READL(HifInfo, AP_P_DMA_G_DMA_2_EN);
    HIF_DMAR_WRITEL(HifInfo, AP_P_DMA_G_DMA_2_EN, (RegVal | ADH_CR_EN | ADH_CR_CONN_BUR_EN));

    GDMA_DBG(("GDMA> HifGdmaStart...\n"));

} /*                     */


/*                                                                            */
/* 
                         
 
                                                                       
 
              
*/
/*                                                                            */
static VOID
HifGdmaStop(
    IN void                     *HifInfoSrc
    )
{
    GL_HIF_INFO_T *HifInfo = (GL_HIF_INFO_T *)HifInfoSrc;
    UINT_32 RegVal;
//                   


    /*                   */
    RegVal = HIF_DMAR_READL(HifInfo, AP_P_DMA_G_DMA_2_INT_EN);
    HIF_DMAR_WRITEL(HifInfo, AP_P_DMA_G_DMA_2_INT_EN, (RegVal & ~(ADH_CR_INTEN_FLAG_0)));


#if 0 /*                                */
    /*          */
    RegVal = HIF_DMAR_READL(HifInfo, AP_P_DMA_G_DMA_2_STOP);
    HIF_DMAR_WRITEL(HifInfo, AP_P_DMA_G_DMA_2_STOP, (RegVal | ADH_CR_STOP));


    /*                             */
    pollcnt = 0;
    do {
        RegVal = HIF_DMAR_READL(HifInfo, AP_P_DMA_G_DMA_2_EN);
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
HifGdmaPollStart(
    IN void                     *HifInfoSrc
    )
{
    GL_HIF_INFO_T *HifInfo = (GL_HIF_INFO_T *)HifInfoSrc;
	UINT_32 RegVal;


    RegVal = HIF_DMAR_READL(HifInfo, AP_P_DMA_G_DMA_2_EN);
	return (((RegVal & ADH_CR_EN) != 0) ? TRUE : FALSE);

} /*                         */


/*                                                                            */
/* 
                              
 
                                                                       
 
              
*/
/*                                                                            */
static MTK_WCN_BOOL
HifGdmaPollIntr(
    IN void                     *HifInfoSrc
    )
{
    GL_HIF_INFO_T *HifInfo = (GL_HIF_INFO_T *)HifInfoSrc;
	UINT_32 RegVal;


	RegVal = HIF_DMAR_READL(HifInfo, AP_P_DMA_G_DMA_2_INT_FLAG);
	return (((RegVal & ADH_CR_FLAG_0) != 0) ? TRUE : FALSE);

} /*                        */


/*                                                                            */
/* 
                                     
 
                                                                       
 
              
*/
/*                                                                            */
static VOID
HifGdmaAckIntr(
    IN void                     *HifInfoSrc
    )
{
    GL_HIF_INFO_T *HifInfo = (GL_HIF_INFO_T *)HifInfoSrc;
	UINT_32 RegVal;


	/*                            */
	RegVal = HIF_DMAR_READL(HifInfo, AP_P_DMA_G_DMA_2_INT_FLAG);
	HIF_DMAR_WRITEL(HifInfo, AP_P_DMA_G_DMA_2_INT_FLAG, (RegVal & ~ADH_CR_FLAG_0));

} /*                       */


/*                                                                            */
/* 
                                     
 
                                                            
 
              
*/
/*                                                                            */
static VOID
HifGdmaClockCtrl(
    IN UINT_32                   FlgIsEnabled
    )
{
    if (FlgIsEnabled == TRUE)
        enable_clock(MT_CG_APDMA_SW_CG, "WLAN");
    else
        disable_clock(MT_CG_APDMA_SW_CG, "WLAN");
}


/*                                                                            */
/* 
                                     
 
                                                                       
 
              
*/
/*                                                                            */
static VOID
HifGdmaRegDump(
    IN void                     *HifInfoSrc
    )
{
    GL_HIF_INFO_T *HifInfo = (GL_HIF_INFO_T *)HifInfoSrc;
    UINT_32 RegId, RegVal;
    UINT_32 RegNum = 0;


    printk("PDMA> Register content 0x%x=\n\t", AP_DMA_HIF_BASE);
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

    printk("\nGDMA> clock status = 0x%x\n\n", *(volatile unsigned int *)0xF0000024);
}


/*                                                                            */
/* 
                   
 
                                                                       
 
              
*/
/*                                                                            */
static VOID
HifGdmaReset(
    IN void                     *HifInfoSrc
    )
{
    GL_HIF_INFO_T *HifInfo = (GL_HIF_INFO_T *)HifInfoSrc;
    UINT_32 LoopCnt;


    /*                                                            */
    printk("\nDMA> do warm reset...\n");

    /*                                                                      */

    HIF_DMAR_WRITEL(HifInfo, AP_P_DMA_G_DMA_2_RST, 0x01);

    for(LoopCnt=0; LoopCnt<10000; LoopCnt++)
    {
        if (!HifGdmaPollStart(HifInfo))
            break; /*          */
    }

    if (HifGdmaPollStart(HifInfo))
    {
        /*                                        */
        printk("\nDMA> do hard reset...\n");
        HIF_DMAR_WRITEL(HifInfo, AP_P_DMA_G_DMA_2_RST, 0x02);
        msleep(1);
        HIF_DMAR_WRITEL(HifInfo, AP_P_DMA_G_DMA_2_RST, 0x00);
    }
}

#endif /*                  */

/*                   */
