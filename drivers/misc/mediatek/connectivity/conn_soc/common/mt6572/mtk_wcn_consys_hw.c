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
#include <mach/upmu_common.h>
#include "mtk_wcn_consys_hw.h"
#include <mach/mt_clkmgr.h>
#if CONSYS_PMIC_CTRL_ENABLE 
#include <mach/mt_pm_ldo.h>
#endif



/*                                                                              
                                                
                                                                                
*/


/*                                                                              
                                                 
                                                                                
*/

/*                                                                              
                                                  
                                                                                
*/
UINT8 __iomem *pEmibaseaddr = NULL;
/*                                                                              
                                                   
                                                                                
*/





/*                                                                              
                                                            
                                                                                
*/



/*                                                                              
                                                
                                                                                
*/
#if CONSYS_ENALBE_SET_JTAG
	UINT32 gJtagCtrl = 0;
	#define JTAG_ADDR1_BASE 0x10208000
	#define JTAG_ADDR2_BASE 0x10005300
	char *jtag_addr1 = (char *)JTAG_ADDR1_BASE;
	char *jtag_addr2 = (char *)JTAG_ADDR2_BASE;

	#define JTAG1_REG_WRITE(addr, value)    \
    writel(value, ((volatile UINT32 *)(jtag_addr1+(addr-JTAG_ADDR1_BASE))))
	#define JTAG1_REG_READ(addr)            \
    readl(((volatile UINT32 *)(jtag_addr1+(addr-JTAG_ADDR1_BASE))))
	#define JTAG2_REG_WRITE(addr, value)    \
    writel(value, ((volatile UINT32 *)(jtag_addr2+(addr-JTAG_ADDR2_BASE))))
	#define JTAG2_REG_READ(addr)            \
    readl(((volatile UINT32 *)(jtag_addr2+(addr-JTAG_ADDR2_BASE))))

static INT32 mtk_wcn_consys_jtag_set_for_mcu(VOID)
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

                /*                          */
    JTAG2_REG_WRITE(0x10005410, 0x77700000); /*                   */
    JTAG2_REG_WRITE(0x10005420, 0x00000077); /*                   */
    JTAG2_REG_WRITE(0x10005370, 0x70000000); /*        */
    JTAG2_REG_WRITE(0x10005380, 0x00000777); /*                 */
    JTAG2_REG_WRITE(0x100053a0, 0x70000000); /*        */
    JTAG2_REG_WRITE(0x100053b0, 0x00000777); /*                 */
    JTAG2_REG_WRITE(0x100053d0, 0x00777000); /*                   */

    return iRet;
}

UINT32 mtk_wcn_consys_jtag_flag_ctrl(UINT32 en)
{
	WMT_PLAT_INFO_FUNC("%s jtag set for MCU\n",en ? "enable" : "disable");
	gJtagCtrl = en;

	return 0;
}

#endif


INT32 mtk_wcn_consys_hw_reg_ctrl(UINT32 on,UINT32 co_clock_en)
{
	INT32 iRet = -1;
	UINT32 retry = 10;
	UINT32 consysHwChipId = 0;
	
	WMT_PLAT_INFO_FUNC("CONSYS-HW-REG-CTRL(0x%08x),start\n",on);
	if(on)
	{
		
#if CONSYS_PMIC_CTRL_ENABLE 
		/*                                                          */
	    /*                                                                                     */
		upmu_set_vcn_1v8_lp_mode_set(0);
		//                          
		/*                                           */
		hwPowerOn(MT6323_POWER_LDO_VCN_1V8,VOL_DEFAULT,"MOD_WMT");
		
		udelay(150);
		if(co_clock_en)
		{
			/*                                                                        */
			upmu_set_vcn28_on_ctrl(0);
		}
		else
		{
			/*                                                                            */
			upmu_set_vcn28_on_ctrl(1);
			/*                                                           */
			//                        
			/*                                           */
			hwPowerOn(MT6323_POWER_LDO_VCN28,VOL_DEFAULT,"MOD_WMT");
		}
#endif

		/*                                                                  */
#if 0		
		/*                                                                                                                                            */
		CONSYS_SET_BIT(CONSYS_CPU_SW_RST_REG, CONSYS_CPU_SW_RST_BIT | CONSYS_CPU_SW_RST_CTRL_KEY);
#endif

		/*                               */
		CONSYS_REG_WRITE(CONSYS_TOP_CLKCG_CLR_REG,CONSYS_REG_READ(CONSYS_TOP_CLKCG_CLR_REG) | CONSYS_TOP_CLKCG_BIT);
		/*                               */
		CONSYS_REG_WRITE(CONSYS_PWRON_CONFG_EN_REG, CONSYS_PWRON_CONFG_EN_VALUE);		

		conn_power_on(SYS_CON);
		WMT_PLAT_INFO_FUNC("reg dump:CONSYS_PWR_CONN_ACK_REG(0x%x)\n",CONSYS_REG_READ(CONSYS_PWR_CONN_ACK_REG));
		WMT_PLAT_INFO_FUNC("reg dump:CONSYS_PWR_CONN_ACK_S_REG(0x%x)\n",CONSYS_REG_READ(CONSYS_PWR_CONN_ACK_S_REG));
		WMT_PLAT_INFO_FUNC("reg dump:CONSYS_TOP1_PWR_CTRL_REG(0x%x)\n",CONSYS_REG_READ(CONSYS_TOP1_PWR_CTRL_REG));
		/*                           */
		udelay(10);
		
		/*                                                                                             */
		/*                                                      */
		while (retry-- > 0)
		{
			WMT_PLAT_DBG_FUNC("CONSYS_CHIP_ID_REG(0x%08x)",CONSYS_REG_READ(CONSYS_CHIP_ID_REG));
			consysHwChipId = CONSYS_REG_READ(CONSYS_CHIP_ID_REG);
			if((consysHwChipId == 0x6582) || (consysHwChipId == 0x6572))
			{
				WMT_PLAT_INFO_FUNC("retry(%d)consys chipId(0x%08x)\n", retry,consysHwChipId);
				break;
			}
			msleep(20);
		}
		/*                                                                  */
#if 0
		/*                                                                                         */
		
		/*                                                                                                                                              */
		/*                                                                              */
		/*                                                                                                            */
		CONSYS_SET_BIT(CONSYS_MCU_CFG_ACR_REG, CONSYS_MCU_CFG_ACR_MBIST_BIT);
		/*                                                                                                        
                                                   */
		CONSYS_REG_WRITE(CONSYS_AFE_REG_DIG_RCK_01,CONSYS_AFE_REG_DIG_RCK_01_VALUE);
		CONSYS_REG_WRITE(CONSYS_AFE_REG_WBG_PLL_02,CONSYS_AFE_REG_WBG_PLL_02_VALUE);
		CONSYS_REG_WRITE(CONSYS_AFE_REG_WBG_WB_TX_01,CONSYS_AFE_REG_WBG_WB_TX_01_VALUE);
		/*                                                                                         */
		CONSYS_CLR_BIT_WITH_KEY(CONSYS_CPU_SW_RST_REG, CONSYS_CPU_SW_RST_BIT , CONSYS_CPU_SW_RST_CTRL_KEY);
#endif
		msleep(5);
		iRet = 0;
	}else{

		conn_power_off(SYS_CON,1000);

#if CONSYS_PMIC_CTRL_ENABLE
		/*                             */
//                                       
		//                          
		/*                              */
//                                                       

		if(0 == co_clock_en)
		{
			/*                              */
			upmu_set_vcn28_on_ctrl(0);
			/*                  */
			//                        
			/*                              */
			hwPowerDown(MT6323_POWER_LDO_VCN28,"MOD_WMT");
		}
		/*                             */
		upmu_set_vcn_1v8_lp_mode_set(0);
		//                          
		/*                              */
		hwPowerDown(MT6323_POWER_LDO_VCN_1V8,"MOD_WMT");

#endif
		
		iRet = 0;

	}
	WMT_PLAT_INFO_FUNC("CONSYS-HW-REG-CTRL(0x%08x),finish\n",on);
	return iRet;
}

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

INT32 mtk_wcn_consys_hw_bt_paldo_ctrl(UINT32 enable)
{
	if(enable){
		/*                                      */
		/*                                                              */
#if CONSYS_PMIC_CTRL_ENABLE 
		hwPowerOn(MT6323_POWER_LDO_VCN33_BT,VOL_3300,"MOD_WMT");
		upmu_set_vcn33_on_ctrl_bt(1);
		
#endif
		WMT_PLAT_INFO_FUNC("WMT do BT PMIC on\n");
	}else{
		/*              */
		/*                                                              */
#if CONSYS_PMIC_CTRL_ENABLE 
		upmu_set_vcn33_on_ctrl_bt(0);
		hwPowerDown(MT6323_POWER_LDO_VCN33_BT,"MOD_WMT");
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
		hwPowerOn(MT6323_POWER_LDO_VCN33_WIFI,VOL_3300,"MOD_WMT");
		upmu_set_vcn33_on_ctrl_wifi(1);
#endif
		WMT_PLAT_INFO_FUNC("WMT do WIFI PMIC on\n");
	}else{
		/*                */
		/*                                                                 */
#if CONSYS_PMIC_CTRL_ENABLE 
		upmu_set_vcn33_on_ctrl_wifi(0);
		hwPowerDown(MT6323_POWER_LDO_VCN33_WIFI,"MOD_WMT");
#endif
		WMT_PLAT_INFO_FUNC("WMT do WIFI PMIC off\n");
	}
	return 0;
}

INT32 mtk_wcn_consys_hw_vcn28_ctrl(UINT32 enable)
{
	if(enable){
		/*                                                 */
#if CONSYS_PMIC_CTRL_ENABLE 
		hwPowerOn(MT6323_POWER_LDO_VCN28,VOL_DEFAULT,"MOD_WMT");
#endif
		WMT_PLAT_INFO_FUNC("turn on vcn28 for fm/gps usage in co-clock mode\n");
	}else{
		/*                                                   */
#if CONSYS_PMIC_CTRL_ENABLE 
		hwPowerDown(MT6323_POWER_LDO_VCN28,"MOD_WMT");
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

INT32 mtk_wcn_consys_hw_init()
{
	INT32 iRet = -1;

	pEmibaseaddr = ioremap_nocache(CONSYS_EMI_AP_PHY_BASE,CONSYS_EMI_MEM_SIZE);
	//                                                     
	if(pEmibaseaddr)
	{
		WMT_PLAT_INFO_FUNC("EMI mapping OK(0x%p)\n",pEmibaseaddr);
		memset(pEmibaseaddr,0,CONSYS_EMI_MEM_SIZE);
		iRet = 0;
	}else{
		WMT_PLAT_ERR_FUNC("EMI mapping fail\n");
	}

	return iRet;
}

INT32 mtk_wcn_consys_hw_deinit()
{
	if(pEmibaseaddr)
	{
		iounmap(pEmibaseaddr);
		pEmibaseaddr = NULL;
	}
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

