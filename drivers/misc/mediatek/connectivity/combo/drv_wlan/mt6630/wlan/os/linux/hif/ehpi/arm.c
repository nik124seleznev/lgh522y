/*
                                                                                         
*/

/*                     
                              

                       
*/



/*
                    
  
                      
                                                             
                                                  
                                                                      
  
                   
                                                                                
                                                                                          
                                                   
  
                   
                                                                                
                                    
  
                   
                                                                                
                                                                                     
                                              
                                                     
                                               
  
                   
                                                                                
                                                                                               
                                                                                                       
                                                                               
  
                   
                                                                                
                                          
                                                        
  
                   
                                                                                
                                                    
  
                   
                                                                                
                           
  
                   
                                                                                
                              
*/

/*                                                                             
                                                     
                                                                               
*/
#if !defined(MCR_EHTCR)
#define MCR_EHTCR                           0x0054
#endif

/*                                                                              
                                                      
                                                                                
*/
#include "gl_os.h"
#include "colibri.h"
#include "wlan_lib.h"

/*                                                                              
                                             
                                                                                
*/

/*                                                                              
                                              
                                                                                
*/

/*                                                                              
                                               
                                                                                
*/
static void __iomem *mt5931_mcr_base;


/*                                                                              
                                         
                                                                                
*/
#if CFG_EHPI_FASTER_BUS_TIMING
#define EHPI_CONFIG     MSC_CS(4, MSC_RBUFF_SLOW | \
	    MSC_RRR(4) | \
	    MSC_RDN(8) | \
	    MSC_RDF(7) | \
	    MSC_RBW_16 | \
	    MSC_RT_VLIO)
#else
#define EHPI_CONFIG     MSC_CS(4, MSC_RBUFF_SLOW | \
	    MSC_RRR(7) | \
	    MSC_RDN(13) | \
	    MSC_RDF(12) | \
	    MSC_RBW_16 | \
	    MSC_RT_VLIO)
#endif				/*                            */

/*                                                                              
                                                        
                                                                                
*/
static VOID collibri_ehpi_reg_init(VOID);

static VOID collibri_ehpi_reg_uninit(VOID);

static VOID mt5931_ehpi_reg_init(VOID);

static VOID mt5931_ehpi_reg_uninit(VOID);

static void busSetIrq(void);

static void busFreeIrq(void);

static irqreturn_t glEhpiInterruptHandler(int irq, void *dev_id);

#if DBG
static void initTrig(void);
#endif

/*                                                                              
                                            
                                                                                
*/

/*                                                                            */
/* 
                                                       
 
                                                       
                                                       
 
                                            
*/
/*                                                                            */
WLAN_STATUS glRegisterBus(probe_card pfProbe, remove_card pfRemove)
{

	ASSERT(pfProbe);
	ASSERT(pfRemove);

	pr_info("mtk_sdio: MediaTek eHPI WLAN driver\n");
	pr_info("mtk_sdio: Copyright MediaTek Inc.\n");

	if (pfProbe(NULL) != WLAN_STATUS_SUCCESS) {
		pfRemove();
		return WLAN_STATUS_FAILURE;
	}

	return WLAN_STATUS_SUCCESS;
}				/*                        */


/*                                                                            */
/* 
                                                         
 
                                                       
 
                
*/
/*                                                                            */
VOID glUnregisterBus(remove_card pfRemove)
{
	ASSERT(pfRemove);
	pfRemove();

	/*                             */

	return;
}				/*                          */


/*                                                                            */
/* 
                                                                            
 
                                                      
                                                                            
 
                
*/
/*                                                                            */
VOID glSetHifInfo(P_GLUE_INFO_T prGlueInfo, ULONG ulCookie)
{
	P_GL_HIF_INFO_T prHif = NULL;
	ASSERT(prGlueInfo);

	prHif = &prGlueInfo->rHifInfo;

	/*                                           */
	prHif->mcr_addr_base = mt5931_mcr_base + EHPI_OFFSET_ADDR;
	prHif->mcr_data_base = mt5931_mcr_base + EHPI_OFFSET_DATA;

	return;
}				/*                       */


/*                                                                            */
/* 
                                               
 
                                                      
 
                
*/
/*                                                                            */
VOID glClearHifInfo(P_GLUE_INFO_T prGlueInfo)
{
	P_GL_HIF_INFO_T prHif = NULL;
	ASSERT(prGlueInfo);

	prHif = &prGlueInfo->rHifInfo;

	/*              */
	prHif->mcr_addr_base = 0;
	prHif->mcr_data_base = 0;

	return;
}				/*                         */


/*                                                                            */
/* 
                                                                                 
 
                                                                   
                                                                                  
                                            
 
                
*/
/*                                                                            */
BOOL glBusInit(PVOID pvData)
{
#if DBG
	initTrig();
#endif

	/*                                      */
	collibri_ehpi_reg_init();

	/*                                */
	mt5931_ehpi_reg_init();

	return TRUE;
};


/*                                                                            */
/* 
                                                  
 
                                                   
 
                
*/
/*                                                                            */
VOID glBusRelease(PVOID pvData)
{
	/*                                */
	mt5931_ehpi_reg_uninit();

	/*                                        */
	collibri_ehpi_reg_uninit();

	return;
}				/*                       */


/*                                                                            */
/* 
                                                                    
 
                                                       
                                                                
                                                         
 
                                          
                                  
*/
/*                                                                            */
INT_32 glBusSetIrq(PVOID pvData, PVOID pfnIsr, PVOID pvCookie)
{
	struct net_device *pDev = (struct net_device *)pvData;
	int i4Status = 0;

	/*                           */
	busSetIrq();

	/*                                       */
	pDev->irq = WLAN_STA_IRQ;

	/*                          */
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 12)
	i4Status = request_irq(pDev->irq, glEhpiInterruptHandler, SA_SHIRQ, pDev->name, pvCookie);
#else
	i4Status = request_irq(pDev->irq,
			       glEhpiInterruptHandler,
			       IRQF_DISABLED | IRQF_SHARED | IRQF_TRIGGER_FALLING,
			       pDev->name, pvCookie);
#endif

	if (i4Status < 0) {
		pr_debug("request_irq(%d) failed\n", pDev->irq);
	} else {
		pr_info("request_irq(%d) success with dev_id(%x)\n", pDev->irq,
		       (unsigned int)pvCookie);
	}

	return i4Status;
}


/*                                                                            */
/* 
                                                                            
 
                                                       
                                                         
 
                
*/
/*                                                                            */
VOID glBusFreeIrq(PVOID pvData, PVOID pvCookie)
{
	struct net_device *prDev = (struct net_device *)pvData;

	if (!prDev) {
		pr_info("Invalid net_device context.\n");
		return;
	}

	if (prDev->irq) {
		disable_irq(prDev->irq);
		free_irq(prDev->irq, pvCookie);
		prDev->irq = 0;
	}

	busFreeIrq();

	return;
}


/*                                                                            */
/* 
                        
 
                                                    
                                              
 
                
*/
/*                                                                            */
VOID glSetPowerState(IN P_GLUE_INFO_T prGlueInfo, IN UINT_32 ePowerMode)
{
	return;
}


#if DBG
/*                                                                            */
/* 
                            
 
             
*/
/*                                                                            */
void setTrig(void)
{
	GPSR1 = (0x1UL << 8);
}				/*                  */


/*                                                                            */
/* 
                            
 
             
*/
/*                                                                            */
void clearTrig(void)
{
	GPCR1 = (0x1UL << 8);
}				/*                    */


/*                                                                            */
/* 
                                            
 
             
*/
/*                                                                            */
static void initTrig(void)
{
	set_GPIO_mode(GPIO40_FFDTR | GPIO_OUT);
	clearTrig();
}				/*                   */
#endif


/*                                                                            */
/* 
                                                                         
 
             
*/
/*                                                                            */
void busSetIrq(void)
{
#if defined(WLAN_STA_IRQ_GPIO)
	pxa_gpio_mode(WLAN_STA_IRQ_GPIO | GPIO_IN);
	set_irq_type(WLAN_STA_IRQ, IRQT_FALLING);
#endif
}


/*                                                                            */
/* 
                                                                      
 
             
*/
/*                                                                            */
void busFreeIrq(void)
{
#if defined(WLAN_STA_IRQ_GPIO)
	pxa_gpio_mode(WLAN_STA_IRQ_GPIO | GPIO_OUT);
#endif
}


/*                                                                            */
/* 
                                                                     
 
             
*/
/*                                                                            */
static VOID collibri_ehpi_reg_init(VOID)
{
	UINT_32 u4RegValue;

	/*                                    */
	pxa_gpio_mode(GPIO80_nCS_4_MD);

	/*                         */
	u4RegValue = MSC2;
	u4RegValue &= ~MSC_CS(4, 0xFFFF);
	u4RegValue |= EHPI_CONFIG;
	MSC2 = u4RegValue;

	pr_info("EHPI new MSC2:0x%08x\n", MSC2);

	return;
}


/*                                                                            */
/* 
                                                                   
 
             
*/
/*                                                                            */
static VOID collibri_ehpi_reg_uninit(VOID)
{
	UINT_32 u4RegValue;

	/*                                 */
	u4RegValue = MSC2;
	u4RegValue &= ~MSC_CS(4, 0xFFFF);
	MSC2 = u4RegValue;

	return;
}


/*                                                                            */
/* 
                                                                    
 
             
*/
/*                                                                            */
static VOID mt5931_ehpi_reg_init(VOID)
{
	struct resource *reso = NULL;

	/*                           */
	reso =
	    request_mem_region((unsigned long)MEM_MAPPED_ADDR, (unsigned long)MEM_MAPPED_LEN,
			       (char *)MODULE_PREFIX);
	if (!reso) {
		pr_err("request_mem_region(0x%08X) failed.\n", MEM_MAPPED_ADDR);
		return;
	}

	/*                             */
	mt5931_mcr_base = ioremap_nocache(MEM_MAPPED_ADDR, MEM_MAPPED_LEN);
	if (!(mt5931_mcr_base)) {
		release_mem_region(MEM_MAPPED_ADDR, MEM_MAPPED_LEN);
		pr_err("ioremap_nocache(0x%08X) failed.\n", MEM_MAPPED_ADDR);
		return;
	}

	return;
}


/*                                                                            */
/* 
                                                                  
 
             
*/
/*                                                                            */
static VOID mt5931_ehpi_reg_uninit(VOID)
{
	iounmap(mt5931_mcr_base);
	mt5931_mcr_base = NULL;

	release_mem_region(MEM_MAPPED_ADDR, MEM_MAPPED_LEN);

	return;
}


/*                                                                            */
/* 
                                                  
 
             
*/
/*                                                                            */
static irqreturn_t glEhpiInterruptHandler(int irq, void *dev_id)
{
	P_GLUE_INFO_T prGlueInfo = (P_GLUE_INFO_T) dev_id;

	ASSERT(prGlueInfo);

	if (!prGlueInfo)
		return IRQ_HANDLED;

	/*                    */
	wlanISR(prGlueInfo->prAdapter, TRUE);

	/*                        */
	if (prGlueInfo->ulFlag & GLUE_FLAG_HALT)
		return IRQ_HANDLED;

	/*                               */
	set_bit(GLUE_FLAG_INT_BIT, &prGlueInfo->ulFlag);

	/*                              */
	wake_up_interruptible(&prGlueInfo->waitq);

	return IRQ_HANDLED;
}
