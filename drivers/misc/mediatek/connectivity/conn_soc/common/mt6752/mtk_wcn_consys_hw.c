/*       
                                            

                                                                                           
*/




/*                                                                              
                                                     
                                                                                
*/

/*                                                                              
                                             
                                                                                
*/

#ifdef DFT_TAG
#undef DFT_TAG
#endif
#define DFT_TAG "[WMT-CONSYS-HW]"


/*                                                                              
                                                          
                                                                                
*/
#include <mach/mt_clkmgr.h>
#include <linux/delay.h>
#include <mach/upmu_common.h>
#include <asm/memblock.h>
#include "osal_typedef.h"
#include "mtk_wcn_consys_hw.h"

#if CONSYS_EMI_MPU_SETTING
#include <mach/emi_mpu.h>
#endif
#if CONSYS_PMIC_CTRL_ENABLE 
#include <mach/mt_pm_ldo.h>
#endif
#ifdef CONFIG_MTK_HIBERNATION
#include <mach/mtk_hibernate_dpm.h>
#endif

#if	 CONFIG_MTK_WCN_ARM64
#include <linux/of_reserved_mem.h>
#endif

#define EMI_MPU_PROTECTION_IS_READY 1

/*                                                                              
                                                
                                                                                
*/


/*                                                                              
                                                 
                                                                                
*/

/*                                                                              
                                                  
                                                                                
*/
UINT8 __iomem *pEmibaseaddr = NULL;
#ifdef BT_WIFI_SHARE_V33
extern BT_WIFI_V33_STATUS gBtWifiV33;
#endif
phys_addr_t gConEmiPhyBase;
struct CONSYS_BASE_ADDRESS conn_reg;

/*                                                                              
                                                   
                                                                                
*/





/*                                                                              
                                                            
                                                                                
*/



/*                                                                              
                                                
                                                                                
*/
#if CONSYS_ENALBE_SET_JTAG
	UINT32 gJtagCtrl = 0;

#define JTAG_ADDR1_BASE 0x10002000

char *jtag_addr1 = (char *)JTAG_ADDR1_BASE;

#define JTAG1_REG_WRITE(addr, value)	\
writel(value, ((volatile PUINT32)(jtag_addr1+(addr-JTAG_ADDR1_BASE))))
#define JTAG1_REG_READ(addr)			\
readl(((volatile PUINT32)(jtag_addr1+(addr-JTAG_ADDR1_BASE))))

static INT32 mtk_wcn_consys_jtag_set_for_mcu(VOID)
{
#if 1
    int iRet = -1;

	WMT_PLAT_INFO_FUNC("WCN jtag_set_for_mcu start...\n");
    jtag_addr1 = ioremap(JTAG_ADDR1_BASE, 0x5000);
    if (jtag_addr1 == 0) {
        printk("remap jtag_addr1 fail!\n");
        return iRet;
    }
    printk("jtag_addr1 = 0x%p\n", jtag_addr1);

	JTAG1_REG_WRITE(0x100053c4, 0x11111100);
	JTAG1_REG_WRITE(0x100053d4, 0x00111111);

			/*                      */
	JTAG1_REG_WRITE(0x10002014, 0x00000003);
	JTAG1_REG_WRITE(0x10005334, 0x55000000);
	JTAG1_REG_WRITE(0x10005344, 0x00555555);
	JTAG1_REG_WRITE(0x10005008, 0xc0000000);
	JTAG1_REG_WRITE(0x10005018, 0x0000000d);
	JTAG1_REG_WRITE(0x10005014, 0x00000032);
	JTAG1_REG_WRITE(0x100020a4, 0x000000ff);
	JTAG1_REG_WRITE(0x100020d4, 0x000000b4);
	JTAG1_REG_WRITE(0x100020d8, 0x0000004b);


#else
	WMT_PLAT_INFO_FUNC("WCN jtag set for mcu start...\n");
		kal_int32 iRet = 0;
		kal_uint32 tmp = 0;
		kal_int32 addr = 0;
		kal_int32 remap_addr1 = 0;
		kal_int32 remap_addr2 = 0;
		
		remap_addr1 = ioremap(JTAG_ADDR1_BASE, 0x1000);
		if (remap_addr1 == 0) {
			printk("remap jtag_addr1 fail!\n");
			return -1;
		}
		
		remap_addr2 = ioremap(JTAG_ADDR2_BASE, 0x100);
		if (remap_addr2 == 0) {
			printk("remap jtag_addr2 fail!\n");
			return -1;
		}

	/*                             */
		addr = remap_addr1 + 0x03C0;
		tmp = DRV_Reg32(addr);
		tmp = tmp & 0xff;
		tmp = tmp | 0x11111100;
		DRV_WriteReg32(addr, tmp);
		WMT_PLAT_INFO_FUNC("(RegAddr, RegVal):(0x%08x, 0x%08x)", addr, DRV_Reg32(addr)); 

		addr = remap_addr1 + 0x03D0;
		tmp = DRV_Reg32(addr);
		tmp = tmp & 0xff000000;
		tmp = tmp | 0x00111111;
		DRV_WriteReg32(addr, tmp);
		WMT_PLAT_INFO_FUNC("(RegAddr, RegVal):(0x%08x, 0x%08x)", addr, DRV_Reg32(addr)); 

	/*                               */
		/*          */
		//                  
		addr = remap_addr2 + 0x0014;
		tmp = 0x00000003;
		DRV_WriteReg32(addr, tmp);
		WMT_PLAT_INFO_FUNC("(RegAddr, RegVal):(0x%08x, 0x%08x)", addr, DRV_Reg32(addr)); 
		/*                 */
		//                  
		addr = remap_addr1 + 0x0334;
		tmp = 0x55000000;
		DRV_WriteReg32(addr, tmp);
		WMT_PLAT_INFO_FUNC("(RegAddr, RegVal):(0x%08x, 0x%08x)", addr, DRV_Reg32(addr));
	
		//                  
		addr = remap_addr1 + 0x0344;
		tmp = 0x00555555;
		DRV_WriteReg32(addr, tmp);
		WMT_PLAT_INFO_FUNC("(RegAddr, RegVal):(0x%08x, 0x%08x)", addr, DRV_Reg32(addr));
		/*                      */
		//                  
		addr = remap_addr1 + 0x0008;
		tmp = 0xc0000000;
		DRV_WriteReg32(addr, tmp);
		WMT_PLAT_INFO_FUNC("(RegAddr, RegVal):(0x%08x, 0x%08x)", addr, DRV_Reg32(addr));
	
		//                  
		addr = remap_addr1 + 0x0018;
		tmp = 0x0000000d;
		DRV_WriteReg32(addr, tmp);
		WMT_PLAT_INFO_FUNC("(RegAddr, RegVal):(0x%08x, 0x%08x)", addr, DRV_Reg32(addr));
	
		//                  
		addr = remap_addr1 + 0x0014;
		tmp = 0x00000032;
		DRV_WriteReg32(addr, tmp);
		WMT_PLAT_INFO_FUNC("(RegAddr, RegVal):(0x%08x, 0x%08x)", addr, DRV_Reg32(addr));
	
		/*           */
		//                  
		addr = remap_addr2 + 0x00a4;
		tmp = 0x000000ff;
		DRV_WriteReg32(addr, tmp);
		WMT_PLAT_INFO_FUNC("(RegAddr, RegVal):(0x%08x, 0x%08x)", addr, DRV_Reg32(addr));
	
		/*                  */
		//                  
		addr = remap_addr2 + 0x00d4;
		tmp = 0x000000b4;
		DRV_WriteReg32(addr, tmp);
		WMT_PLAT_INFO_FUNC("(RegAddr, RegVal):(0x%08x, 0x%08x)", addr, DRV_Reg32(addr));
	
		//                  
		addr = remap_addr2 + 0x00d8;
		tmp = 0x0000004b;
		DRV_WriteReg32(addr, tmp);
		WMT_PLAT_INFO_FUNC("(RegAddr, RegVal):(0x%08x, 0x%08x)", addr, DRV_Reg32(addr));
#endif

	return 0;
}

UINT32 mtk_wcn_consys_jtag_flag_ctrl(UINT32 en)
{
	WMT_PLAT_INFO_FUNC("%s jtag set for MCU\n",en ? "enable" : "disable");
	gJtagCtrl = en;

	return 0;
}

#endif


#if CONFIG_OF

#define PWR_ON_OFF_API_AVALIABLE 1

INT32 mtk_wcn_consys_hw_reg_ctrl(UINT32 on,UINT32 co_clock_en)
{
	
#if PWR_ON_OFF_API_AVALIABLE
	INT32 iRet = -1;
#endif

	UINT32 retry = 10;
	UINT32 consysHwChipId = 0;

	WMT_PLAT_INFO_FUNC("CONSYS-HW-REG-CTRL(0x%08x),start\n",on);
	WMT_PLAT_DBG_FUNC("CONSYS_EMI_MAPPING dump before power on/off(0x%08x)\n",CONSYS_REG_READ(CONSYS_EMI_MAPPING));

	if(on)
	{
#if CONSYS_PMIC_CTRL_ENABLE 
		/*                                          */
	    /*                                                                                                  */
		mt6325_upmu_set_rg_vcn18_on_ctrl(0);
		/*                                                                                            */
		hwPowerOn(MT6325_POWER_LDO_VCN18, VOL_1800, "wcn_drv"); 

		udelay(150);
		
		if(co_clock_en)
		{
			/*                 */
			/*                                                                                  */
			/*                                                 */
			mt6325_upmu_set_rg_vcn28_on_ctrl(0);
		}
		else
		{
			/*                */
			/*                                                                                       */
			mt6325_upmu_set_rg_vcn28_on_ctrl(1);
			/*                                                                           */
			hwPowerOn(MT6325_POWER_LDO_VCN28, VOL_2800, "wcn_drv"); 
		}
#endif

		/*                                                                          */
		CONSYS_REG_WRITE((conn_reg.ap_rgu_base + CONSYS_CPU_SW_RST_OFFSET), CONSYS_REG_READ(conn_reg.ap_rgu_base + CONSYS_CPU_SW_RST_OFFSET) | CONSYS_CPU_SW_RST_BIT | CONSYS_CPU_SW_RST_CTRL_KEY);
		WMT_PLAT_INFO_FUNC("reg dump:CONSYS_CPU_SW_RST_REG(0x%x)\n",CONSYS_REG_READ(conn_reg.ap_rgu_base + CONSYS_CPU_SW_RST_OFFSET));
		/*                                                                  */
		CONSYS_REG_WRITE((conn_reg.topckgen_base + CONSYS_TOP_CLKCG_CLR_OFFSET), CONSYS_REG_READ(conn_reg.topckgen_base + CONSYS_TOP_CLKCG_CLR_OFFSET) | CONSYS_TOP_CLKCG_BIT);
		/*                                                                       */
		CONSYS_REG_WRITE((conn_reg.spm_base + CONSYS_PWRON_CONFG_EN_OFFSET), CONSYS_PWRON_CONFG_EN_VALUE);		

#if PWR_ON_OFF_API_AVALIABLE
		iRet = conn_power_on(); //                      
		if(iRet)
		{
			WMT_PLAT_ERR_FUNC("conn_power_on fail(%d)\n",iRet);
		}else
		{
			WMT_PLAT_INFO_FUNC("conn_power_on ok\n");
		}
#else
		/*                                                                                                                */			
		CONSYS_REG_WRITE(conn_reg.spm_base + CONSYS_TOP1_PWR_CTRL_OFFSET, CONSYS_REG_READ(conn_reg.spm_base + CONSYS_TOP1_PWR_CTRL_OFFSET) | CONSYS_SPM_PWR_ON_BIT);
		/*                                                                                                             */		
		while (0 == (CONSYS_PWR_ON_ACK_BIT & CONSYS_REG_READ(conn_reg.spm_base + CONSYS_PWR_CONN_ACK_OFFSET)));
		/*                                                                                                             */		
		CONSYS_REG_WRITE(conn_reg.spm_base + CONSYS_TOP1_PWR_CTRL_OFFSET, CONSYS_REG_READ(conn_reg.spm_base + CONSYS_TOP1_PWR_CTRL_OFFSET) | CONSYS_SRAM_CONN_PD_BIT);
		/*                                                                                                                   */					
		CONSYS_REG_WRITE(conn_reg.spm_base + CONSYS_TOP1_PWR_CTRL_OFFSET, CONSYS_REG_READ(conn_reg.spm_base + CONSYS_TOP1_PWR_CTRL_OFFSET) | CONSYS_SPM_PWR_ON_S_BIT);
		/*                                                                                                             */		
		CONSYS_REG_WRITE(conn_reg.spm_base + CONSYS_TOP1_PWR_CTRL_OFFSET, CONSYS_REG_READ(conn_reg.spm_base + CONSYS_TOP1_PWR_CTRL_OFFSET) & ~CONSYS_CLK_CTRL_BIT);
		/*           */	
		udelay(1);
		/*                                                                                                               */						
		while (0 == (CONSYS_PWR_CONN_ACK_S_BIT & CONSYS_REG_READ(conn_reg.spm_base + CONSYS_PWR_CONN_ACK_S_OFFSET)));
		/*                                                                                                            */				
		CONSYS_REG_WRITE(conn_reg.spm_base + CONSYS_TOP1_PWR_CTRL_OFFSET, CONSYS_REG_READ(conn_reg.spm_base + CONSYS_TOP1_PWR_CTRL_OFFSET) & ~CONSYS_SPM_PWR_ISO_S_BIT);
		/*                                                                                                                     */					
		CONSYS_REG_WRITE(conn_reg.spm_base + CONSYS_TOP1_PWR_CTRL_OFFSET, CONSYS_REG_READ(conn_reg.spm_base + CONSYS_TOP1_PWR_CTRL_OFFSET) | CONSYS_SPM_PWR_RST_BIT);

#endif
		WMT_PLAT_INFO_FUNC("reg dump:CONSYS_PWR_CONN_ACK_REG(0x%x)\n",CONSYS_REG_READ(conn_reg.spm_base + CONSYS_PWR_CONN_ACK_OFFSET));
		WMT_PLAT_INFO_FUNC("reg dump:CONSYS_PWR_CONN_ACK_S_REG(0x%x)\n",CONSYS_REG_READ(conn_reg.spm_base + CONSYS_PWR_CONN_ACK_S_OFFSET));
		WMT_PLAT_INFO_FUNC("reg dump:CONSYS_TOP1_PWR_CTRL_REG(0x%x)\n",CONSYS_REG_READ(conn_reg.spm_base + CONSYS_TOP1_PWR_CTRL_OFFSET));
		/*                                                    */
		udelay(10);
		/*                                                              */
		
		/*                                                                    */
		while (retry-- > 0)
		{
			consysHwChipId = CONSYS_REG_READ(conn_reg.mcu_base + CONSYS_CHIP_ID_OFFSET);
			if((consysHwChipId == 0x6752) || (consysHwChipId == 0x6582) || (consysHwChipId == 0x6572) || (consysHwChipId == 0x6571)) {
				WMT_PLAT_INFO_FUNC("retry(%d)consys chipId(0x%08x)\n", retry,consysHwChipId);
				break;
			} else {
				WMT_PLAT_INFO_FUNC("Read CONSYS chipId(0x%08x)",consysHwChipId); 
			}
			msleep(20);
		}

		/*                                                                                                                    */
		
		/*                                                                                                   */
		/*                                                                              */
		/*                                                                                                            */
		CONSYS_REG_WRITE(conn_reg.mcu_base + CONSYS_MCU_CFG_ACR_OFFSET, CONSYS_REG_READ(conn_reg.mcu_base + CONSYS_MCU_CFG_ACR_OFFSET) | CONSYS_MCU_CFG_ACR_MBIST_BIT);

		/*                                                                                                           */

#if 0
		/*                                                                                                        
                                                   */
		CONSYS_REG_WRITE(CONSYS_AFE_REG_DIG_RCK_01,CONSYS_AFE_REG_DIG_RCK_01_VALUE);
		CONSYS_REG_WRITE(CONSYS_AFE_REG_WBG_PLL_02,CONSYS_AFE_REG_WBG_PLL_02_VALUE);
		CONSYS_REG_WRITE(CONSYS_AFE_REG_WBG_WB_TX_01,CONSYS_AFE_REG_WBG_WB_TX_01_VALUE);
#endif
		/*                                                                                                     */
		CONSYS_REG_WRITE(conn_reg.ap_rgu_base + CONSYS_CPU_SW_RST_OFFSET,
			(CONSYS_REG_READ(conn_reg.ap_rgu_base + CONSYS_CPU_SW_RST_OFFSET) & ~CONSYS_CPU_SW_RST_BIT) | CONSYS_CPU_SW_RST_CTRL_KEY);

		msleep(5);

	}else{

#if PWR_ON_OFF_API_AVALIABLE
		WMT_PLAT_INFO_FUNC("\n conn_power_off begin\n");
		/*                                                                              */
		iRet = conn_power_off();//                    
		WMT_PLAT_INFO_FUNC("\n conn_power_off end\n");
		if(iRet)
		{
			WMT_PLAT_ERR_FUNC("conn_power_off fail(%d)\n",iRet);
		}else
		{
			WMT_PLAT_INFO_FUNC("conn_power_off ok\n");
		}
#else
		/*                                                                                                */		
		CONSYS_REG_WRITE(conn_reg.spm_base + CONSYS_TOP1_PWR_CTRL_OFFSET, CONSYS_REG_READ(conn_reg.spm_base + CONSYS_TOP1_PWR_CTRL_OFFSET) & ~CONSYS_SPM_PWR_RST_BIT);
		/*                                                                                         */
		CONSYS_REG_WRITE(conn_reg.spm_base + CONSYS_TOP1_PWR_CTRL_OFFSET, CONSYS_REG_READ(conn_reg.spm_base + CONSYS_TOP1_PWR_CTRL_OFFSET) | CONSYS_SPM_PWR_ISO_S_BIT);
		/*                  */										
		/*         */
		udelay(1);
		/*                                                                                                  */	
		CONSYS_REG_WRITE(conn_reg.spm_base + CONSYS_TOP1_PWR_CTRL_OFFSET, CONSYS_REG_READ(conn_reg.spm_base + CONSYS_TOP1_PWR_CTRL_OFFSET) & ~CONSYS_SRAM_CONN_PD_BIT);
		/*                                                                                                  */	
		CONSYS_REG_WRITE(conn_reg.spm_base + CONSYS_TOP1_PWR_CTRL_OFFSET, CONSYS_REG_READ(conn_reg.spm_base + CONSYS_TOP1_PWR_CTRL_OFFSET) & ~(CONSYS_SPM_PWR_ON_BIT | CONSYS_SPM_PWR_ON_S_BIT));
		/*                                                                                                           */
		CONSYS_REG_WRITE(conn_reg.spm_base + CONSYS_TOP1_PWR_CTRL_OFFSET, CONSYS_REG_READ(conn_reg.spm_base + CONSYS_TOP1_PWR_CTRL_OFFSET) |CONSYS_CLK_CTRL_BIT);

#endif

#if CONSYS_PMIC_CTRL_ENABLE

		/*                                                                                     */
		mt6325_upmu_set_rg_vcn28_on_ctrl(0);
		/*                                                                           */
		hwPowerDown(MT6325_POWER_LDO_VCN28, "wcn_drv");

		/*                                                                                       */
		mt6325_upmu_set_rg_vcn18_mode_set(0);
		hwPowerDown(MT6325_POWER_LDO_VCN18, "wcn_drv"); 

#endif

	}
	WMT_PLAT_INFO_FUNC("CONSYS-HW-REG-CTRL(0x%08x),finish\n",on);
	return 0;
}

#else
INT32 mtk_wcn_consys_hw_reg_ctrl(UINT32 on,UINT32 co_clock_en)
{

#if PWR_ON_OFF_API_AVALIABLE
	INT32 iRet = -1;
#endif

	UINT32 retry = 10;
	UINT32 consysHwChipId = 0;
	
	WMT_PLAT_INFO_FUNC("CONSYS-HW-REG-CTRL(0x%08x),start\n",on);
	WMT_PLAT_DBG_FUNC("CONSYS_EMI_MAPPING dump before power on/off(0x%08x)\n",CONSYS_REG_READ(CONSYS_EMI_MAPPING));

	if(on)
	{
#if CONSYS_PMIC_CTRL_ENABLE 
		/*                                          */
	    /*                                                                                                  */
		mt6325_upmu_set_rg_vcn18_on_ctrl(0);
		/*                                                                                            */
		hwPowerOn(MT6325_POWER_LDO_VCN18, VOL_1800, "wcn_drv"); 

		udelay(150);
		
		if(co_clock_en)
		{
			/*                 */
			/*                                                                                  */
			/*                                                 */
			mt6325_upmu_set_rg_vcn28_on_ctrl(0);
		}
		else
		{
			/*                */
			/*                                                                                       */
			mt6325_upmu_set_rg_vcn28_on_ctrl(1);
			/*                                                                           */
			hwPowerOn(MT6325_POWER_LDO_VCN28, VOL_2800, "wcn_drv"); 
		}
#endif

		/*                                                                          */
		CONSYS_REG_WRITE(CONSYS_CPU_SW_RST_REG, (CONSYS_REG_READ(CONSYS_CPU_SW_RST_REG) | CONSYS_CPU_SW_RST_BIT | CONSYS_CPU_SW_RST_CTRL_KEY));
		WMT_PLAT_INFO_FUNC("reg dump:CONSYS_CPU_SW_RST_REG(0x%x)\n",CONSYS_REG_READ(CONSYS_CPU_SW_RST_REG));
		/*                                                                  */
		CONSYS_REG_WRITE(CONSYS_TOP_CLKCG_CLR_REG,CONSYS_REG_READ(CONSYS_TOP_CLKCG_CLR_REG) | CONSYS_TOP_CLKCG_BIT);
		/*                                                                       */
		CONSYS_REG_WRITE(CONSYS_PWRON_CONFG_EN_REG, CONSYS_PWRON_CONFG_EN_VALUE);		

#if PWR_ON_OFF_API_AVALIABLE
		iRet = conn_power_on(); //                    
		if(iRet)
		{
			WMT_PLAT_ERR_FUNC("conn_power_on fail(%d)\n",iRet);
		}else
		{
			WMT_PLAT_INFO_FUNC("conn_power_on ok\n");
		}
#else
		/*                                                                                                                */			
		CONSYS_REG_WRITE(CONSYS_TOP1_PWR_CTRL_REG, CONSYS_REG_READ(CONSYS_TOP1_PWR_CTRL_REG) | CONSYS_SPM_PWR_ON_BIT);
		/*                                                                                                             */		
		while (0 == (CONSYS_PWR_ON_ACK_BIT & CONSYS_REG_READ(CONSYS_PWR_CONN_ACK_REG)));
		/*                                                                                                             */		
		CONSYS_REG_WRITE(CONSYS_TOP1_PWR_CTRL_REG, CONSYS_REG_READ(CONSYS_TOP1_PWR_CTRL_REG) | CONSYS_SRAM_CONN_PD_BIT);
		/*                                                                                                                   */					
		CONSYS_REG_WRITE(CONSYS_TOP1_PWR_CTRL_REG, CONSYS_REG_READ(CONSYS_TOP1_PWR_CTRL_REG) | CONSYS_SPM_PWR_ON_S_BIT);
		/*                                                                                                             */		
		CONSYS_REG_WRITE(CONSYS_TOP1_PWR_CTRL_REG, CONSYS_REG_READ(CONSYS_TOP1_PWR_CTRL_REG) & ~CONSYS_CLK_CTRL_BIT);
		/*           */	
		udelay(1);
		/*                                                                                                               */						
		while (0 == (CONSYS_PWR_CONN_ACK_S_BIT & CONSYS_REG_READ(CONSYS_PWR_CONN_ACK_S_REG)));
		/*                                                                                                            */				
		CONSYS_REG_WRITE(CONSYS_TOP1_PWR_CTRL_REG, CONSYS_REG_READ(CONSYS_TOP1_PWR_CTRL_REG) & ~CONSYS_SPM_PWR_ISO_S_BIT);
		/*                                                                                                                     */					
		CONSYS_REG_WRITE(CONSYS_TOP1_PWR_CTRL_REG, CONSYS_REG_READ(CONSYS_TOP1_PWR_CTRL_REG) | CONSYS_SPM_PWR_RST_BIT);
#endif

		WMT_PLAT_INFO_FUNC("reg dump:CONSYS_PWR_CONN_ACK_REG(0x%x)\n",CONSYS_REG_READ(CONSYS_PWR_CONN_ACK_REG));
		WMT_PLAT_INFO_FUNC("reg dump:CONSYS_PWR_CONN_ACK_S_REG(0x%x)\n",CONSYS_REG_READ(CONSYS_PWR_CONN_ACK_S_REG));
		WMT_PLAT_INFO_FUNC("reg dump:CONSYS_TOP1_PWR_CTRL_REG(0x%x)\n",CONSYS_REG_READ(CONSYS_TOP1_PWR_CTRL_REG));
		/*                                                    */
		udelay(10);
		/*                                                              */
		
		/*                                                                    */
		while (retry-- > 0)
		{
			WMT_PLAT_DBG_FUNC("CONSYS_CHIP_ID_REG(0x%08x)",CONSYS_REG_READ(CONSYS_CHIP_ID_REG));
			consysHwChipId = CONSYS_REG_READ(CONSYS_CHIP_ID_REG);
			if((consysHwChipId == 0x6752) || (consysHwChipId == 0x6582) || (consysHwChipId == 0x6572) || (consysHwChipId == 0x6571))
			{
				WMT_PLAT_INFO_FUNC("retry(%d)consys chipId(0x%08x)\n", retry,consysHwChipId);
				break;
			}
			msleep(20);
		}

		/*                                                                                                                    */
		
		/*                                                                                                   */
		/*                                                                              */
		/*                                                                                                            */
		CONSYS_REG_WRITE(CONSYS_MCU_CFG_ACR_REG, CONSYS_REG_READ(CONSYS_MCU_CFG_ACR_REG) | CONSYS_MCU_CFG_ACR_MBIST_BIT);

		/*                                                                                                           */

#if 0
		/*                                                                                                        
                                                   */
		CONSYS_REG_WRITE(CONSYS_AFE_REG_DIG_RCK_01,CONSYS_AFE_REG_DIG_RCK_01_VALUE);
		CONSYS_REG_WRITE(CONSYS_AFE_REG_WBG_PLL_02,CONSYS_AFE_REG_WBG_PLL_02_VALUE);
		CONSYS_REG_WRITE(CONSYS_AFE_REG_WBG_WB_TX_01,CONSYS_AFE_REG_WBG_WB_TX_01_VALUE);
#endif
		/*                                                                                                     */
		CONSYS_REG_WRITE(CONSYS_CPU_SW_RST_REG,
			(CONSYS_REG_READ(CONSYS_CPU_SW_RST_REG) & ~CONSYS_CPU_SW_RST_BIT) | CONSYS_CPU_SW_RST_CTRL_KEY);

		msleep(5);

	}else{

#if PWR_ON_OFF_API_AVALIABLE

		WMT_PLAT_INFO_FUNC("\n conn_power_off begin\n");
		/*                                                                              */
		iRet = conn_power_off();//                     
		WMT_PLAT_INFO_FUNC("\n conn_power_off end\n");
		if(iRet)
		{
			WMT_PLAT_ERR_FUNC("conn_power_off fail(%d)\n",iRet);
		}else
		{
			WMT_PLAT_INFO_FUNC("conn_power_off ok\n");
		}
#else
		/*                                                                                                */		
		CONSYS_REG_WRITE(CONSYS_TOP1_PWR_CTRL_REG, CONSYS_REG_READ(CONSYS_TOP1_PWR_CTRL_REG) & ~CONSYS_SPM_PWR_RST_BIT);
		/*                                                                                        */
		CONSYS_REG_WRITE(CONSYS_TOP1_PWR_CTRL_REG, CONSYS_REG_READ(CONSYS_TOP1_PWR_CTRL_REG) | CONSYS_SPM_PWR_ISO_S_BIT);
		/*                  */										
		/*         */
		udelay(1);
		/*                                                                                                  */	
		CONSYS_REG_WRITE(CONSYS_TOP1_PWR_CTRL_REG, CONSYS_REG_READ(CONSYS_TOP1_PWR_CTRL_REG) & ~CONSYS_SRAM_CONN_PD_BIT);
		/*                                                                                                  */	
		CONSYS_REG_WRITE(CONSYS_TOP1_PWR_CTRL_REG, CONSYS_REG_READ(CONSYS_TOP1_PWR_CTRL_REG) & ~(CONSYS_SPM_PWR_ON_BIT | CONSYS_SPM_PWR_ON_S_BIT));
		/*                                                                                                          */
		CONSYS_REG_WRITE(CONSYS_TOP1_PWR_CTRL_REG, CONSYS_REG_READ(CONSYS_TOP1_PWR_CTRL_REG) |CONSYS_CLK_CTRL_BIT);
#endif

#if CONSYS_PMIC_CTRL_ENABLE

		/*                                                                                     */
		mt6325_upmu_set_rg_vcn28_on_ctrl(0);
		/*                                                                           */
		hwPowerDown(MT6325_POWER_LDO_VCN28, "wcn_drv");

		/*                                                                                       */
		mt6325_upmu_set_rg_vcn18_mode_set(0);
		hwPowerDown(MT6325_POWER_LDO_VCN18, "wcn_drv"); 

#endif

	}
	WMT_PLAT_INFO_FUNC("CONSYS-HW-REG-CTRL(0x%08x),finish\n",on);
	return 0;
}
#endif

INT32
mtk_wcn_consys_hw_gpio_ctrl (UINT32 on)
{
    INT32 iRet = 0;
	WMT_PLAT_INFO_FUNC("CONSYS-HW-GPIO-CTRL(0x%08x), start\n",on);

	if(on)
	{

		/*                                                       */
	    iRet += wmt_plat_gpio_ctrl(PIN_GPS_SYNC, PIN_STA_INIT);
	    iRet += wmt_plat_gpio_ctrl(PIN_GPS_LNA, PIN_STA_INIT);

	    iRet += wmt_plat_gpio_ctrl(PIN_I2S_GRP,PIN_STA_INIT);

	    /*                                                                                                                       */
	    //                                                                   
	    //                                                      
	    iRet += wmt_plat_eirq_ctrl(PIN_BGF_EINT, PIN_STA_INIT);
		iRet += wmt_plat_eirq_ctrl(PIN_BGF_EINT, PIN_STA_EINT_DIS);
	    WMT_PLAT_INFO_FUNC("CONSYS-HW, BGF IRQ registered and disabled \n");

	}else{

	    /*                                                              */
	    iRet += wmt_plat_eirq_ctrl(PIN_BGF_EINT, PIN_STA_EINT_DIS);
		iRet += wmt_plat_eirq_ctrl(PIN_BGF_EINT, PIN_STA_DEINIT);
	    WMT_PLAT_INFO_FUNC("CONSYS-HW, BGF IRQ unregistered and disabled\n");
	    //                                                         

		/*                                                       */
	    iRet += wmt_plat_gpio_ctrl(PIN_GPS_SYNC, PIN_STA_DEINIT);
	   	iRet += wmt_plat_gpio_ctrl(PIN_I2S_GRP,PIN_STA_DEINIT);
	    /*               */
	    iRet += wmt_plat_gpio_ctrl(PIN_GPS_LNA, PIN_STA_DEINIT);
	    

	}
	WMT_PLAT_INFO_FUNC("CONSYS-HW-GPIO-CTRL(0x%08x), finish\n",on);
    return iRet;

}

INT32 mtk_wcn_consys_hw_pwr_on(UINT32 co_clock_en)
{
	INT32 iRet = 0;

	WMT_PLAT_INFO_FUNC("CONSYS-HW-PWR-ON, start\n");

	iRet += mtk_wcn_consys_hw_reg_ctrl(1,co_clock_en);
	iRet += mtk_wcn_consys_hw_gpio_ctrl(1);
#if CONSYS_ENALBE_SET_JTAG
	if(gJtagCtrl)
	{
		mtk_wcn_consys_jtag_set_for_mcu();
	}
#endif
	WMT_PLAT_INFO_FUNC("CONSYS-HW-PWR-ON, finish(%d)\n",iRet);
	return iRet;
}

INT32 mtk_wcn_consys_hw_pwr_off (VOID)
{
	INT32 iRet = 0;

	WMT_PLAT_INFO_FUNC("CONSYS-HW-PWR-OFF, start\n");

	iRet += mtk_wcn_consys_hw_reg_ctrl(0,0);
	iRet += mtk_wcn_consys_hw_gpio_ctrl(0);

	WMT_PLAT_INFO_FUNC("CONSYS-HW-PWR-OFF, finish(%d)\n",iRet);
	return iRet;
}


INT32
mtk_wcn_consys_hw_rst (UINT32 co_clock_en)
{
    INT32 iRet = 0;
    WMT_PLAT_INFO_FUNC("CONSYS-HW, hw_rst start, eirq should be disabled before this step\n");

    /*                             */
    iRet += mtk_wcn_consys_hw_reg_ctrl(0,co_clock_en);

    /*                            */
    iRet += mtk_wcn_consys_hw_reg_ctrl(1,co_clock_en);

    WMT_PLAT_INFO_FUNC("CONSYS-HW, hw_rst finish, eirq should be enabled after this step\n");
    return iRet;
}
#ifdef BT_WIFI_SHARE_V33
INT32 mtk_wcn_consys_hw_bt_paldo_ctrl(UINT32 enable)
{
	//                                                     
	if(enable){
		if(1 == gBtWifiV33.counter) {
			gBtWifiV33.counter++;
			WMT_PLAT_DBG_FUNC("V33 has been enabled,counter(%d)\n",gBtWifiV33.counter);
		} else if (2 == gBtWifiV33.counter) {
			WMT_PLAT_DBG_FUNC("V33 has been enabled,counter(%d)\n",gBtWifiV33.counter);
		} else {
#if CONSYS_PMIC_CTRL_ENABLE 
			/*                                      */
			/*                                                              */
			/*                                                    */
			hwPowerOn(MT6325_POWER_LDO_VCN33, VOL_3300, "wcn_drv"); 
			mt6325_upmu_set_rg_vcn33_on_ctrl(1); 
#endif
			WMT_PLAT_INFO_FUNC("WMT do BT/WIFI v3.3 on\n");
			gBtWifiV33.counter++;
		}

	}else{
		if(1 == gBtWifiV33.counter) {
			/*              */
			/*                                                              */
#if CONSYS_PMIC_CTRL_ENABLE 
			mt6325_upmu_set_rg_vcn33_on_ctrl(0);
			hwPowerDown(MT6325_POWER_LDO_VCN33,"wcn_drv");
#endif
			WMT_PLAT_INFO_FUNC("WMT do BT/WIFI v3.3 off\n");
			gBtWifiV33.counter--;
		} else if(2 == gBtWifiV33.counter){
			gBtWifiV33.counter--;
			WMT_PLAT_DBG_FUNC("V33 no need disabled,counter(%d)\n",gBtWifiV33.counter);
		} else {
			WMT_PLAT_DBG_FUNC("V33 has been disabled,counter(%d)\n",gBtWifiV33.counter);			
		}

	}
	//                                                          
	return 0;
}

INT32 mtk_wcn_consys_hw_wifi_paldo_ctrl(UINT32 enable)
{
	mtk_wcn_consys_hw_bt_paldo_ctrl(enable);
	return 0;
}

#else
INT32 mtk_wcn_consys_hw_bt_paldo_ctrl(UINT32 enable)
{
	if(enable){
		/*                                      */
		/*                                                              */
#if CONSYS_PMIC_CTRL_ENABLE 
		/*                                                    */
		hwPowerOn(MT6325_POWER_LDO_VCN33, VOL_3300, "wcn_drv"); 
		mt6325_upmu_set_rg_vcn33_on_ctrl(1); 
		
#endif
		WMT_PLAT_INFO_FUNC("WMT do BT PMIC on\n");
	}else{
		/*              */
		/*                                                              */
#if CONSYS_PMIC_CTRL_ENABLE 
		mt6325_upmu_set_rg_vcn33_on_ctrl(0);
		hwPowerDown(MT6325_POWER_LDO_VCN33,"wcn_drv");
#endif
		WMT_PLAT_INFO_FUNC("WMT do BT PMIC off\n");
	}
	return 0;
}

INT32 mtk_wcn_consys_hw_wifi_paldo_ctrl(UINT32 enable)
{
	if(enable){
		/*                                        */
		/*                                                                 */
#if CONSYS_PMIC_CTRL_ENABLE 
		hwPowerOn(MT6325_POWER_LDO_VCN33, VOL_3300, "wcn_drv"); 
		mt6325_upmu_set_rg_vcn33_on_ctrl(1);
#endif
		WMT_PLAT_INFO_FUNC("WMT do WIFI PMIC on\n");
	}else{
		/*                */
		/*                                                                 */
#if CONSYS_PMIC_CTRL_ENABLE 
		mt6325_upmu_set_rg_vcn33_on_ctrl(0);
		hwPowerDown(MT6325_POWER_LDO_VCN33,"wcn_drv");
#endif
		WMT_PLAT_INFO_FUNC("WMT do WIFI PMIC off\n");
	}
	return 0;
}

#endif
INT32 mtk_wcn_consys_hw_vcn28_ctrl(UINT32 enable)
{
	if(enable){
		/*                                                 */
#if CONSYS_PMIC_CTRL_ENABLE 
		hwPowerOn(MT6325_POWER_LDO_VCN28, VOL_2800, "wcn_drv");
#endif
		WMT_PLAT_INFO_FUNC("turn on vcn28 for fm/gps usage in co-clock mode\n");
	}else{
		/*                                                   */
#if CONSYS_PMIC_CTRL_ENABLE 
		hwPowerDown(MT6325_POWER_LDO_VCN28, "wcn_drv");
#endif
		WMT_PLAT_INFO_FUNC("turn off vcn28 for fm/gps usage in co-clock mode\n");
	}
	return 0;
}

INT32 mtk_wcn_consys_hw_state_show(VOID)
{
	return 0;
}

#if CONSYS_WMT_REG_SUSPEND_CB_ENABLE
UINT32 mtk_wcn_consys_hw_osc_en_ctrl(UINT32 en)
{
	if(en)
	{
		WMT_PLAT_INFO_FUNC("enable consys sleep mode(turn off 26M)\n");
		CONSYS_REG_WRITE(CONSYS_AP2CONN_OSC_EN_REG, CONSYS_REG_READ(CONSYS_AP2CONN_OSC_EN_REG) & ~CONSYS_AP2CONN_OSC_EN_BIT);
	}else
	{
		WMT_PLAT_INFO_FUNC("disable consys sleep mode\n");
		CONSYS_REG_WRITE(CONSYS_AP2CONN_OSC_EN_REG, CONSYS_REG_READ(CONSYS_AP2CONN_OSC_EN_REG) | CONSYS_AP2CONN_OSC_EN_BIT);	
	}

	WMT_PLAT_INFO_FUNC("dump CONSYS_AP2CONN_OSC_EN_REG(0x%x)\n",CONSYS_REG_READ(CONSYS_AP2CONN_OSC_EN_REG));

	return 0;
}
#endif

INT32 mtk_wcn_consys_hw_restore(struct device *device)
{
	UINT32 addrPhy = 0;
	
	if(gConEmiPhyBase)
	{
	
#if CONSYS_EMI_MPU_SETTING
		/*                            */
		WMT_PLAT_INFO_FUNC("setting MPU for EMI share memory\n");
#if EMI_MPU_PROTECTION_IS_READY
		emi_mpu_set_region_protection(gConEmiPhyBase + SZ_1M/2,
			gConEmiPhyBase + SZ_1M - 1,
			13,
			SET_ACCESS_PERMISSON(FORBIDDEN,FORBIDDEN,FORBIDDEN,FORBIDDEN,FORBIDDEN,NO_PROTECTION,FORBIDDEN,NO_PROTECTION));
#endif

#endif
		/*                                                                   */
		addrPhy = (gConEmiPhyBase & 0xFFF00000) >> 20;

		/*                                       */
		addrPhy -= 0x400;/*        */
		addrPhy = addrPhy | 0x1000;

		CONSYS_REG_WRITE(conn_reg.topckgen_base + CONSYS_EMI_MAPPING_OFFSET, \
							CONSYS_REG_READ(conn_reg.topckgen_base + CONSYS_EMI_MAPPING_OFFSET) | addrPhy);

		WMT_PLAT_INFO_FUNC("CONSYS_EMI_MAPPING dump in restore cb(0x%08x)\n",CONSYS_REG_READ(conn_reg.topckgen_base + CONSYS_EMI_MAPPING_OFFSET));

#if 1
		pEmibaseaddr = ioremap_nocache(gConEmiPhyBase + SZ_1M/2, CONSYS_EMI_MEM_SIZE);
#else
		pEmibaseaddr = ioremap_nocache(CONSYS_EMI_AP_PHY_BASE,CONSYS_EMI_MEM_SIZE);
#endif
		if(pEmibaseaddr)
		{
			WMT_PLAT_INFO_FUNC("EMI mapping OK(0x%p)\n",pEmibaseaddr);
			memset(pEmibaseaddr,0,CONSYS_EMI_MEM_SIZE);
		}else{
			WMT_PLAT_ERR_FUNC("EMI mapping fail\n");
		}
	}
	else
	{
		WMT_PLAT_ERR_FUNC("consys emi memory address gConEmiPhyBase invalid\n");
	}

	return 0;
}

#if CONFIG_MTK_WCN_ARM64
/*                               */
reservedmem_of_init_fn reserve_memory_test_fn(struct reserved_mem *rmem, unsigned long node, const char *uname)
{
        pr_alert("%s, name: %s, uname: %s, base: 0x%llx, size: 0x%llx\n", __func__,  rmem->name, uname,
                        (unsigned long long)rmem->base,
                        (unsigned long long)rmem->size);
		gConEmiPhyBase = rmem->base;
        return 0;
}
RESERVEDMEM_OF_DECLARE(reserve_memory_test, "consys-reserve-memory", reserve_memory_test_fn);
#endif

VOID __init mtk_wcn_consys_memory_reserve(VOID)
{
#if CONFIG_MTK_WCN_ARM64
		WMT_PLAT_INFO_FUNC("Reserved memory by device tree!\n");
//                                                     
#else
	gConEmiPhyBase = arm_memblock_steal(SZ_1M,SZ_2M);

	if(gConEmiPhyBase)
	{
		WMT_PLAT_INFO_FUNC("memblock done: 0x%zx\n",(SIZE_T)gConEmiPhyBase);
	}else
	{
		WMT_PLAT_ERR_FUNC("memblock fail\n");
	}
#endif

}

INT32 mtk_wcn_consys_hw_init()
{

	INT32 iRet = -1;
	UINT32 addrPhy = 0;
	struct device_node *node = NULL;

 	node = of_find_compatible_node(NULL, NULL, "mediatek,CONSYS");
	if(node){
		/*                       */
		conn_reg.mcu_base = (SIZE_T)of_iomap(node, 0);
		WMT_PLAT_INFO_FUNC("Get mcu register base(0x%zx)\n",conn_reg.mcu_base);
		conn_reg.ap_rgu_base = (SIZE_T)of_iomap(node, 1);
		WMT_PLAT_INFO_FUNC("Get ap_rgu register base(0x%zx)\n",conn_reg.ap_rgu_base);
		conn_reg.topckgen_base = (SIZE_T)of_iomap(node, 2);
		WMT_PLAT_INFO_FUNC("Get topckgen register base(0x%zx)\n",conn_reg.topckgen_base);
		conn_reg.spm_base = (SIZE_T)of_iomap(node, 3);
		WMT_PLAT_INFO_FUNC("Get spm register base(0x%zx)\n",conn_reg.spm_base);
	} else {
		WMT_PLAT_ERR_FUNC("[%s] can't find CONSYS compatible node\n",__func__);
		return iRet;
	}
	
	if(gConEmiPhyBase)
	{
#if CONSYS_EMI_MPU_SETTING
		/*                            */
		WMT_PLAT_INFO_FUNC("setting MPU for EMI share memory\n");
#if EMI_MPU_PROTECTION_IS_READY
		emi_mpu_set_region_protection(gConEmiPhyBase + SZ_1M/2,
			gConEmiPhyBase + SZ_1M - 1,
			13,
			SET_ACCESS_PERMISSON(FORBIDDEN,FORBIDDEN,FORBIDDEN,FORBIDDEN,FORBIDDEN,NO_PROTECTION,FORBIDDEN,NO_PROTECTION));
#endif

#endif
		WMT_PLAT_INFO_FUNC("get consys start phy address(0x%zx)\n",(SIZE_T)gConEmiPhyBase);

		/*                                                                   */
		addrPhy = (gConEmiPhyBase & 0xFFF00000) >> 20;

		/*                                       */
		addrPhy -= 0x400;/*        */
		addrPhy = addrPhy | 0x1000;

		CONSYS_REG_WRITE(conn_reg.topckgen_base + CONSYS_EMI_MAPPING_OFFSET, \
							CONSYS_REG_READ(conn_reg.topckgen_base + CONSYS_EMI_MAPPING_OFFSET) | addrPhy);

		WMT_PLAT_INFO_FUNC("CONSYS_EMI_MAPPING dump(0x%08x)\n",CONSYS_REG_READ(conn_reg.topckgen_base + CONSYS_EMI_MAPPING_OFFSET));


#if 1
		pEmibaseaddr = ioremap_nocache(gConEmiPhyBase + SZ_1M/2, CONSYS_EMI_MEM_SIZE);
#else
		pEmibaseaddr = ioremap_nocache(CONSYS_EMI_AP_PHY_BASE,CONSYS_EMI_MEM_SIZE);
#endif
		//                                                     
		if(pEmibaseaddr)
		{
			WMT_PLAT_INFO_FUNC("EMI mapping OK(0x%p)\n",pEmibaseaddr);
			memset(pEmibaseaddr,0,CONSYS_EMI_MEM_SIZE);
			iRet = 0;
		}else{
			WMT_PLAT_ERR_FUNC("EMI mapping fail\n");
		}
	}
	else
	{
		WMT_PLAT_ERR_FUNC("consys emi memory address gConEmiPhyBase invalid\n");
	}
#ifdef CONFIG_MTK_HIBERNATION
	WMT_PLAT_INFO_FUNC("register connsys restore cb for complying with IPOH function\n");
	register_swsusp_restore_noirq_func(ID_M_CONNSYS,mtk_wcn_consys_hw_restore,NULL);
#endif
	return iRet;

}

INT32 mtk_wcn_consys_hw_deinit()
{
	if(pEmibaseaddr)
	{
		iounmap(pEmibaseaddr);
		pEmibaseaddr = NULL;
	}
#ifdef CONFIG_MTK_HIBERNATION
	unregister_swsusp_restore_noirq_func(ID_M_CONNSYS);
#endif

	return 0;
}


UINT8  *mtk_wcn_consys_emi_virt_addr_get(UINT32 ctrl_state_offset)
{
	UINT8 *p_virtual_addr = NULL;

	if(!pEmibaseaddr)
	{
		WMT_PLAT_ERR_FUNC("EMI base address is NULL\n");
		return NULL;
	}
	WMT_PLAT_DBG_FUNC("ctrl_state_offset(%08x)\n",ctrl_state_offset);
	p_virtual_addr = pEmibaseaddr + ctrl_state_offset;

	return p_virtual_addr;
}

UINT32 mtk_wcn_consys_soc_chipid()
{
	return PLATFORM_SOC_CHIP;
}

