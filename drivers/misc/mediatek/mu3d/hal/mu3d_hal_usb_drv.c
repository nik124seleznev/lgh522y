#include <linux/mu3d/hal/mu3d_hal_osal.h>
#define _MTK_USB_DRV_EXT_
#include <linux/mu3d/hal/mu3d_hal_usb_drv.h>
#undef _MTK_USB_DRV_EXT_
#include <linux/mu3d/hal/mu3d_hal_hw.h>
#include <linux/mu3d/hal/mu3d_hal_qmu_drv.h>
#include <linux/mu3d/hal/mu3d_hal_comm.h>
#include <linux/mu3phy/mtk-phy.h>

struct USB_REQ *mu3d_hal_get_req( DEV_INT32 ep_num, USB_DIR dir)
{
    DEV_INT32 ep_index=0;

    if(dir == USB_TX) {
        ep_index = ep_num;
    } else if(dir == USB_RX) {
        ep_index = ep_num + MAX_EP_NUM;
    } else {
        BUG_ON(1);
    }

    return &g_u3d_req[ep_index];
}

/* 
                                                            
  
 */
void mu3d_hal_pdn_dis(void)
{
	os_clrmsk(U3D_SSUSB_IP_PW_CTRL2, SSUSB_IP_DEV_PDN);
	#ifdef SUPPORT_U3
	os_clrmsk(U3D_SSUSB_U3_CTRL_0P, (SSUSB_U3_PORT_DIS|SSUSB_U3_PORT_PDN|SSUSB_U3_PORT_U2_CG_EN));
	#endif
	os_clrmsk(U3D_SSUSB_U2_CTRL_0P, (SSUSB_U2_PORT_DIS|SSUSB_U2_PORT_PDN|SSUSB_U2_PORT_U2_CG_EN));
}

/* 
                                                                    
  
 */
void mu3d_hal_ssusb_en(void)
{
	os_printk(K_INFO, "%s\n", __func__);

	os_clrmsk(U3D_SSUSB_IP_PW_CTRL0, SSUSB_IP_SW_RST);
	os_clrmsk(U3D_SSUSB_IP_PW_CTRL2, SSUSB_IP_DEV_PDN);

	#ifdef SUPPORT_U3
	os_clrmsk(U3D_SSUSB_U3_CTRL_0P, (SSUSB_U3_PORT_DIS | SSUSB_U3_PORT_PDN | SSUSB_U3_PORT_HOST_SEL));
	#endif

	#ifdef SUPPORT_OTG
	os_setmsk(U3D_SSUSB_U2_CTRL_0P, (SSUSB_U2_PORT_OTG_MAC_AUTO_SEL | SSUSB_U2_PORT_OTG_SEL));
	os_clrmsk(U3D_SSUSB_U2_CTRL_0P, (SSUSB_U2_PORT_DIS | SSUSB_U2_PORT_PDN));
	#else
	os_clrmsk(U3D_SSUSB_U2_CTRL_0P, (SSUSB_U2_PORT_DIS | SSUSB_U2_PORT_PDN | SSUSB_U2_PORT_HOST_SEL));
	#endif

	os_setmsk(U3D_SSUSB_REF_CK_CTRL, (SSUSB_REF_MAC_CK_GATE_EN | SSUSB_REF_PHY_CK_GATE_EN | SSUSB_REF_CK_GATE_EN | SSUSB_REF_MAC3_CK_GATE_EN));

	/*                                              */
	mu3d_hal_check_clk_sts();
}

void _ex_mu3d_hal_ssusb_en(void)
{
	os_printk(K_INFO, "%s\n", __func__);

	os_clrmsk(U3D_SSUSB_IP_PW_CTRL0, SSUSB_IP_SW_RST);
	os_clrmsk(U3D_SSUSB_IP_PW_CTRL2, SSUSB_IP_DEV_PDN);
	#ifdef SUPPORT_U3
	os_clrmsk(U3D_SSUSB_U3_CTRL_0P, (SSUSB_U3_PORT_DIS | SSUSB_U3_PORT_PDN | SSUSB_U3_PORT_HOST_SEL));
	#endif
	os_clrmsk(U3D_SSUSB_U2_CTRL_0P, (SSUSB_U2_PORT_DIS | SSUSB_U2_PORT_PDN | SSUSB_U2_PORT_HOST_SEL));

	os_setmsk(U3D_SSUSB_REF_CK_CTRL, (SSUSB_REF_MAC_CK_GATE_EN | SSUSB_REF_PHY_CK_GATE_EN | SSUSB_REF_CK_GATE_EN | SSUSB_REF_MAC3_CK_GATE_EN));

	/*                                              */
	mu3d_hal_check_clk_sts();
}


/* 
                                                     
 */
void mu3d_hal_dft_reg(void)
{
	DEV_UINT32 tmp;

	/*                              */
	#ifdef NEVER
	//                               
	os_setmsk(U3D_SSUSB_SYS_CK_CTRL, SSUSB_SYS_CK_DIV2_EN);
	//                               
	os_writelmsk(U3D_USB20_TIMING_PARAMETER, 63, TIME_VALUE_1US);
	#ifdef SUPPORT_U3
	//                               
	os_writelmsk(U3D_TIMING_PULSE_CTRL, 63, CNT_1US_VALUE);
	#endif
	#endif

	//               
	//      
	os_writelmsk(U3D_USB20_TIMING_PARAMETER, U3D_MAC_SYS_CK, TIME_VALUE_1US);
	//      
 	os_writelmsk(U3D_SSUSB_U2_PHY_PLL, U3D_MAC_REF_CK, SSUSB_U2_PORT_1US_TIMER);
	//            
	tmp = div_and_rnd_up(600, (1000 / U3D_MAC_REF_CK)) + 1;
	os_writelmsk(U3D_SSUSB_IP_PW_CTRL0,
		(tmp<<SSUSB_IP_U2_ENTER_SLEEP_CNT_OFST), SSUSB_IP_U2_ENTER_SLEEP_CNT);

	//               
#ifdef SUPPORT_U3
	//      
	os_writelmsk(U3D_TIMING_PULSE_CTRL, U3D_MAC_SYS_CK, CNT_1US_VALUE);
	//      
 	os_writelmsk(U3D_REF_CK_PARAMETER, U3D_MAC_REF_CK, REF_1000NS);
	//             
	tmp = div_and_rnd_up(300, (1000 / U3D_MAC_REF_CK));
	os_writelmsk(U3D_UX_EXIT_LFPS_TIMING_PARAMETER,
		(tmp<<RX_UX_EXIT_LFPS_REF_OFST), RX_UX_EXIT_LFPS_REF);
#endif

#ifdef NEVER
	/*                              */
	//                               
 	os_writelmsk(U3D_SSUSB_U2_PHY_PLL, 10, SSUSB_U2_PORT_1US_TIMER);
	//      
	os_writelmsk(U3D_SSUSB_IP_PW_CTRL0,
		(7<<SSUSB_IP_U2_ENTER_SLEEP_CNT_OFST), SSUSB_IP_U2_ENTER_SLEEP_CNT);
	#ifdef SUPPORT_U3
	//                           
 	os_writelmsk(U3D_REF_CK_PARAMETER, 10, REF_1000NS);
	//       
	os_writelmsk(U3D_UX_EXIT_LFPS_TIMING_PARAMETER,
		(3<<RX_UX_EXIT_LFPS_REF_OFST), RX_UX_EXIT_LFPS_REF);
	#endif
#endif /*       */

	/*                                               */
	#ifndef CONFIG_MTK_FPGA
	//                  
	os_writel(U3D_SSUSB_PRB_CTRL0, 0xffff);
	#endif

	//               
	//                                                                   
	os_setmsk(U3D_POWER_MANAGEMENT, (LPM_BESL_STALL | LPM_BESLD_STALL));


	//               
	#ifdef SUPPORT_U3
	#ifdef U2_U3_SWITCH_AUTO
	os_setmsk(U3D_USB2_TEST_MODE, U2U3_AUTO_SWITCH);
	#endif

	//                                                   
	os_writel(U3D_LTSSM_CTRL, os_readl(U3D_LTSSM_CTRL) &~ SOFT_U3_EXIT_EN);

	#ifndef CONFIG_MTK_FPGA
	os_writel(U3D_PIPE_LATCH_SELECT, 0);
	#endif

	#endif

	#if ISO_UPDATE_TEST & ISO_UPDATE_MODE
	os_setmsk(U3D_POWER_MANAGEMENT, ISO_UPDATE);
	#else
	os_clrmsk(U3D_POWER_MANAGEMENT, ISO_UPDATE);
	#endif

	#ifdef DIS_ZLP_CHECK_CRC32
	//                                                   
	os_writel(U3D_LINK_CAPABILITY_CTRL, ZLP_CRC32_CHK_DIS);
	#endif
}

/* 
                                              
  
 */
void mu3d_hal_rst_dev(void)
{
	DEV_INT32 ret;

	os_printk(K_ERR, "%s\n", __func__);

#if 0
	os_writel(U3D_SSUSB_DEV_RST_CTRL, SSUSB_DEV_SW_RST | SSUSB_DEV_SW_BMU_RST | SSUSB_DEV_SW_QMU_RST| SSUSB_DEV_SW_DRAM_RST);
	os_writel(U3D_SSUSB_DEV_RST_CTRL, 0);
#else
	os_writel(U3D_SSUSB_DEV_RST_CTRL, SSUSB_DEV_SW_RST);
	os_writel(U3D_SSUSB_DEV_RST_CTRL, 0);
#endif

	mu3d_hal_check_clk_sts();

	ret = wait_for_value(U3D_SSUSB_IP_PW_STS1, SSUSB_DEV_QMU_RST_B_STS, SSUSB_DEV_QMU_RST_B_STS, 1, 10);
	if (ret == RET_FAIL)
        os_printk(K_ERR, "SSUSB_DEV_QMU_RST_B_STS NG\n");

	ret = wait_for_value(U3D_SSUSB_IP_PW_STS1, SSUSB_DEV_BMU_RST_B_STS, SSUSB_DEV_BMU_RST_B_STS, 1, 10);
	if (ret == RET_FAIL)
        os_printk(K_ERR, "SSUSB_DEV_BMU_RST_B_STS NG\n");

	ret = wait_for_value(U3D_SSUSB_IP_PW_STS1, SSUSB_DEV_RST_B_STS, SSUSB_DEV_RST_B_STS, 1, 10);
	if (ret == RET_FAIL)
        os_printk(K_ERR, "SSUSB_DEV_RST_B_STS NG\n");
}

/* 
                                                                   
  
 */
DEV_INT32 mu3d_hal_check_clk_sts(void)
{
	DEV_INT32 ret;

	os_printk(K_ERR, "%s\n", __func__);

	ret = wait_for_value(U3D_SSUSB_IP_PW_STS1, SSUSB_SYS125_RST_B_STS, SSUSB_SYS125_RST_B_STS, 1, 10);
	if (ret == RET_FAIL)
        os_printk(K_ERR, "SSUSB_SYS125_RST_B_STS NG\n");

	//                                                                             
	if (!(os_readl(U3D_SSUSB_U2_CTRL_0P) & SSUSB_U2_PORT_PDN))
	{
		ret = wait_for_value(U3D_SSUSB_IP_PW_STS2, SSUSB_U2_MAC_SYS_RST_B_STS, SSUSB_U2_MAC_SYS_RST_B_STS, 1, 10);
		if (ret == RET_FAIL)
			os_printk(K_ERR, "SSUSB_U2_MAC_SYS_RST_B_STS NG\n");
	}

#ifdef SUPPORT_U3
	//                                                                             
	if (!(os_readl(U3D_SSUSB_U3_CTRL_0P) & SSUSB_U3_PORT_PDN))
	{
		ret = wait_for_value(U3D_SSUSB_IP_PW_STS1, SSUSB_U3_MAC_RST_B_STS, SSUSB_U3_MAC_RST_B_STS, 1, 100);
		if (ret == RET_FAIL)
	        os_printk(K_ERR, "SSUSB_U3_MAC_RST_B_STS NG, U3D_SSUSB_IP_PW_STS1 is 0x%x\n", os_readl(U3D_SSUSB_IP_PW_STS1));
	}
#endif

	os_printk(K_CRIT, "check clk pass!!\n");
	return RET_SUCCESS;
}

/* 
                                 
  
 */
DEV_INT32 mu3d_hal_link_up(DEV_INT32 latch_val)
{
	mu3d_hal_ssusb_en();
	mu3d_hal_rst_dev();
	os_ms_delay(50);
	os_writel(U3D_USB3_CONFIG, USB3_EN);
	os_writel(U3D_PIPE_LATCH_SELECT, latch_val);//                   

	return 0;
}

/* 
                                              
  
 */
void mu3d_hal_initr_dis(void)
{
	/*                           */
	os_writel(U3D_LV1IECR, 0xFFFFFFFF);

	/*                            */
	os_writel(U3D_EPIECR, 0xFFFFFFFF);

	/*                       */
	os_writel(U3D_DMAIECR, 0xFFFFFFFF);

#ifdef SUPPORT_OTG
	/*                                */
	os_writel(U3D_QIECR0, 0xFFFFFFFF);

	/*                                                           */
	os_writel(U3D_QIECR1, 0xFFFFFFFF);

	/*                                 */
	os_writel(U3D_QEMIECR, 0xFFFFFFFF);

	/*                                                               */
	os_writel(U3D_TQERRIECR0, 0xFFFFFFFF);

	/*                                                               */
	os_writel(U3D_RQERRIECR0, 0xFFFFFFFF);

	/*                                 */
	os_writel(U3D_RQERRIECR1, 0xFFFFFFFF);
#endif
}

void mu3d_hal_clear_intr(void)
{
	os_printk(K_ERR, "%s\n", __func__);

	/*                                          */
	os_writel(U3D_EPISR, 0xFFFFFFFF);

	/*                                              */
	os_writel(U3D_DMAISR, 0xFFFFFFFF);

#ifdef SUPPORT_OTG
	/*                                     */
	os_writel(U3D_QISAR0, 0xFFFFFFFF);

	/*                                                                */
	os_writel(U3D_QISAR1, 0xFFFFFFFF);

	/*                                */
	os_writel(U3D_QEMIR, 0xFFFFFFFF);

	/*                                                             */
	os_writel(U3D_TQERRIR0, 0xFFFFFFFF);

	/*                                                             */
	os_writel(U3D_RQERRIR0, 0xFFFFFFFF);

	/*                               */
	os_writel(U3D_RQERRIR1, 0xFFFFFFFF);
#endif

	/*                         */
	os_writel(U3D_DEV_LINK_INTR, 0xFFFFFFFF);

	/*                                     */
	os_writel(U3D_COMMON_USB_INTR, 0xFFFFFFFF);

	/*                                */
	os_writel(U3D_LTSSM_INTR, 0xFFFFFFFF);
}

/* 
                                                           
  
 */
void mu3d_hal_system_intr_en(void)
{
	DEV_UINT16 int_en;
	DEV_UINT32 ltssm_int_en;

	os_printk(K_ERR, "%s\n", __func__);

	/*                               */
	os_writel(U3D_EPIECR, os_readl(U3D_EPIER));

	/*                          */
	os_writel(U3D_DMAIECR, os_readl(U3D_DMAIER));

	/*                                 */
	os_writel(U3D_COMMON_USB_INTR_ENABLE, 0x00);

	/*                                      */
	os_writel(U3D_COMMON_USB_INTR, os_readl(U3D_COMMON_USB_INTR));

	/*                               */
	int_en = SUSPEND_INTR_EN|RESUME_INTR_EN|RESET_INTR_EN|CONN_INTR_EN|DISCONN_INTR_EN \
			|VBUSERR_INTR_EN|LPM_INTR_EN|LPM_RESUME_INTR_EN;
	os_writel(U3D_COMMON_USB_INTR_ENABLE, int_en);

	#ifdef SUPPORT_U3
	/*                            */
	os_writel(U3D_LTSSM_INTR_ENABLE, 0x00);
	os_printk(K_ERR, "U3D_LTSSM_INTR: %x\n", os_readl(U3D_LTSSM_INTR));

	/*                          */
	os_writel(U3D_LTSSM_INTR, os_readl(U3D_LTSSM_INTR));

	/*                           */
	ltssm_int_en = SS_INACTIVE_INTR_EN|SS_DISABLE_INTR_EN|COMPLIANCE_INTR_EN|LOOPBACK_INTR_EN \
		     |HOT_RST_INTR_EN|WARM_RST_INTR_EN|RECOVERY_INTR_EN|ENTER_U0_INTR_EN|ENTER_U1_INTR_EN \
		     |ENTER_U2_INTR_EN|ENTER_U3_INTR_EN|EXIT_U1_INTR_EN|EXIT_U2_INTR_EN|EXIT_U3_INTR_EN \
		     |RXDET_SUCCESS_INTR_EN|VBUS_RISE_INTR_EN|VBUS_FALL_INTR_EN|U3_LFPS_TMOUT_INTR_EN|U3_RESUME_INTR_EN;
	os_writel(U3D_LTSSM_INTR_ENABLE, ltssm_int_en);
	#endif

	#ifdef SUPPORT_OTG
	//                                     
	os_printk(K_ERR, "U3D_SSUSB_OTG_STS: %x\n", os_readl(U3D_SSUSB_OTG_STS));
	os_writel(U3D_SSUSB_OTG_STS_CLR, os_readl(U3D_SSUSB_OTG_STS));
	os_writel(U3D_SSUSB_OTG_INT_EN,
		os_readl(U3D_SSUSB_OTG_INT_EN) | SSUSB_VBUS_CHG_INT_B_EN | SSUSB_CHG_B_ROLE_B_INT_EN | \
		SSUSB_CHG_A_ROLE_B_INT_EN | SSUSB_ATTACH_B_ROLE_INT_EN);
	#endif

	#ifdef USE_SSUSB_QMU
	/*                       */
	os_writel(U3D_QIESR1, TXQ_EMPTY_IESR | TXQ_CSERR_IESR | TXQ_LENERR_IESR | \
				RXQ_EMPTY_IESR | RXQ_CSERR_IESR | RXQ_LENERR_IESR | \
				RXQ_ZLPERR_IESR);
	#endif

	/*                         */
	os_writel(U3D_DMAIESR, EP0DMAIESR);

	/*                              */
	os_writel(U3D_DEV_LINK_INTR_ENABLE, SSUSB_DEV_SPEED_CHG_INTR_EN);
}

void _ex_mu3d_hal_system_intr_en(void)
{
	DEV_UINT16 int_en;
	DEV_UINT32 ltssm_int_en;

	os_printk(K_ERR, "%s\n", __func__);

	/*                               */
	os_writel(U3D_EPIECR, os_readl(U3D_EPIER));

	/*                          */
	os_writel(U3D_DMAIECR, os_readl(U3D_DMAIER));

	/*                                 */
	os_writel(U3D_COMMON_USB_INTR_ENABLE, 0x00);

	/*                                      */
	os_writel(U3D_COMMON_USB_INTR, os_readl(U3D_COMMON_USB_INTR));

	/*                               */
	int_en = SUSPEND_INTR_EN|RESUME_INTR_EN|RESET_INTR_EN|CONN_INTR_EN|DISCONN_INTR_EN \
			|VBUSERR_INTR_EN|LPM_INTR_EN|LPM_RESUME_INTR_EN;
	os_writel(U3D_COMMON_USB_INTR_ENABLE, int_en);

	#ifdef SUPPORT_U3
	/*                            */
	os_writel(U3D_LTSSM_INTR_ENABLE, 0x00);
	os_printk(K_ERR, "U3D_LTSSM_INTR: %x\n", os_readl(U3D_LTSSM_INTR));

	/*                          */
	os_writel(U3D_LTSSM_INTR, os_readl(U3D_LTSSM_INTR));

	/*                           */
	ltssm_int_en = SS_INACTIVE_INTR_EN|SS_DISABLE_INTR_EN|COMPLIANCE_INTR_EN|LOOPBACK_INTR_EN \
		     |HOT_RST_INTR_EN|WARM_RST_INTR_EN|RECOVERY_INTR_EN|ENTER_U0_INTR_EN|ENTER_U1_INTR_EN \
		     |ENTER_U2_INTR_EN|ENTER_U3_INTR_EN|EXIT_U1_INTR_EN|EXIT_U2_INTR_EN|EXIT_U3_INTR_EN \
		     |RXDET_SUCCESS_INTR_EN|VBUS_RISE_INTR_EN|VBUS_FALL_INTR_EN|U3_LFPS_TMOUT_INTR_EN|U3_RESUME_INTR_EN;
	os_writel(U3D_LTSSM_INTR_ENABLE, ltssm_int_en);
	#endif

	#ifdef SUPPORT_OTG
	//                                     
	os_printk(K_ERR, "U3D_SSUSB_OTG_STS: %x\n", os_readl(U3D_SSUSB_OTG_STS));
	os_writel(U3D_SSUSB_OTG_STS_CLR, os_readl(U3D_SSUSB_OTG_STS));
	os_writel(U3D_SSUSB_OTG_INT_EN,
		os_readl(U3D_SSUSB_OTG_INT_EN) | SSUSB_VBUS_CHG_INT_B_EN | SSUSB_CHG_B_ROLE_B_INT_EN | \
		SSUSB_CHG_A_ROLE_B_INT_EN | SSUSB_ATTACH_B_ROLE_INT_EN);
	#endif

	#ifdef USE_SSUSB_QMU
	/*                       */
	os_writel(U3D_QIESR1, TXQ_EMPTY_IESR | TXQ_CSERR_IESR | TXQ_LENERR_IESR | \
				RXQ_EMPTY_IESR | RXQ_CSERR_IESR | RXQ_LENERR_IESR | \
				RXQ_ZLPERR_IESR);
	#endif

	/*                         */
	os_writel(U3D_DMAIESR, EP0DMAIESR);

	/*                              */
	os_writel(U3D_DEV_LINK_INTR_ENABLE, SSUSB_DEV_SPEED_CHG_INTR_EN);
}

/* 
                                      
  
 */
void mu3d_hal_resume(void)
{
	#ifdef SUPPORT_U3
	if(os_readl(U3D_DEVICE_CONF) & HW_USB2_3_SEL){ //  
		mu3d_hal_pdn_ip_port(1, 0, 1, 0);

		os_setmsk(U3D_LINK_POWER_CONTROL, UX_EXIT);
	}
	else
	#endif
	{ //     
		mu3d_hal_pdn_ip_port(1, 0, 0, 1);

		os_setmsk(U3D_POWER_MANAGEMENT, RESUME);
    	os_ms_delay(10);
    	os_clrmsk(U3D_POWER_MANAGEMENT, RESUME);
	}
}

/* 
                                                 
  
 */
void mu3d_hal_u2dev_connect()
{
	os_writel(U3D_POWER_MANAGEMENT, os_readl(U3D_POWER_MANAGEMENT) | SOFT_CONN);
	os_writel(U3D_POWER_MANAGEMENT, os_readl(U3D_POWER_MANAGEMENT) | SUSPENDM_ENABLE);
	os_printk(K_INFO, "SOFTCONN = 1\n");
}

void mu3d_hal_u2dev_disconn()
{
	os_writel(U3D_POWER_MANAGEMENT, os_readl(U3D_POWER_MANAGEMENT) & ~SOFT_CONN);
	os_writel(U3D_POWER_MANAGEMENT, os_readl(U3D_POWER_MANAGEMENT) & ~SUSPENDM_ENABLE);
	os_printk(K_INFO, "SOFTCONN = 0\n");
}

/* 
                                        
  
 */
void mu3d_hal_dump_register(void)
{
	void __iomem *i;
	printk("\n\n[mu3d_hal_dump_register]\n");
	printk("SSUSB_DEV_BASE ==================> \n");

	for (i = SSUSB_DEV_BASE ; i<= SSUSB_DEV_BASE+0xC; i += 0x4)
		printk("[0x%lx] 0x%x \n" , (unsigned long)i, os_readl(i));

	for (i = SSUSB_DEV_BASE+0x20 ; i<= SSUSB_DEV_BASE+0x24; i += 0x4)
		printk("[0x%lx] 0x%x \n" , (unsigned long)i, os_readl(i));

	for (i = SSUSB_DEV_BASE+0x30 ; i<= SSUSB_DEV_BASE+0x34; i += 0x4)
		printk("[0x%lx] 0x%x \n" , (unsigned long)i, os_readl(i));

	for (i = SSUSB_DEV_BASE+0x40 ; i<= SSUSB_DEV_BASE+0x44; i += 0x4)
		printk("[0x%lx] 0x%x \n" , (unsigned long)i, os_readl(i));

	for (i = SSUSB_DEV_BASE+0x50 ; i<= SSUSB_DEV_BASE+0x50; i += 0x4)
		printk("[0x%lx] 0x%x \n" , (unsigned long)i, os_readl(i));

	for (i = SSUSB_DEV_BASE+0x70 ; i<= SSUSB_DEV_BASE+0x74; i += 0x4)
		printk( "[0x%lx] 0x%x \n" , (unsigned long)i, os_readl(i));

	for (i = SSUSB_DEV_BASE+0x80 ; i<= SSUSB_DEV_BASE+0x9C; i += 0x4)
		printk( "[0x%lx] 0x%x \n" , (unsigned long)i, os_readl(i));

	for (i = SSUSB_DEV_BASE+0xC0 ; i<= SSUSB_DEV_BASE+0xEC; i += 0x4)
		printk( "[0x%lx] 0x%x \n" , (unsigned long)i, os_readl(i));

	for (i = SSUSB_DEV_BASE+0x100 ; i<= SSUSB_DEV_BASE+0x10C; i += 0x4)
		printk( "[0x%lx] 0x%x \n" , (unsigned long)i, os_readl(i));

	for (i = SSUSB_DEV_BASE+0x400 ; i<= SSUSB_DEV_BASE+0x410; i += 0x4)
		printk( "[0x%lx] 0x%x \n" , (unsigned long)i, os_readl(i));

	for (i = SSUSB_DEV_BASE+0x700 ; i<= SSUSB_DEV_BASE+0x71C; i += 0x4)
		printk( "[0x%lx] 0x%x \n" , (unsigned long)i, os_readl(i));

	for (i = SSUSB_DEV_BASE+0xC04 ; i<= SSUSB_DEV_BASE+0xC10; i += 0x4)
		printk( "[0x%lx] 0x%x \n" , (unsigned long)i, os_readl(i));

	for (i = SSUSB_DEV_BASE+0xC20 ; i<= SSUSB_DEV_BASE+0xC3C; i += 0x4)
		printk( "[0x%lx] 0x%x \n" , (unsigned long)i, os_readl(i));

	for (i = SSUSB_DEV_BASE+0xC84 ; i<= SSUSB_DEV_BASE+0xC84; i += 0x4)
		printk( "[0x%lx] 0x%x \n" , (unsigned long)i, os_readl(i));

	printk( "SSUSB_DEV_BASE <================== \n");


	printk( "SSUSB_USB2_CSR ==================> \n");
	for (i = SSUSB_USB2_CSR_BASE+0x0000 ; i<= SSUSB_USB2_CSR_BASE+0x0060; i += 0x4)
		printk( "[0x%lx] 0x%x \n" , (unsigned long)i, os_readl(i));

	printk( "SSUSB_USB2_CSR <================== \n");


	printk( "SSUSB_SIFSLV_IPPC_BASE ==================> \n");
	for (i = SSUSB_SIFSLV_IPPC_BASE+0x0000 ; i<= SSUSB_SIFSLV_IPPC_BASE+0x002C; i += 0x4)
		printk( "[0x%lx] 0x%x \n" , (unsigned long)i, os_readl(i));

	for (i = SSUSB_SIFSLV_IPPC_BASE+0x0030 ; i<= SSUSB_SIFSLV_IPPC_BASE+0x0038; i += 0x4)
		printk( "[0x%lx] 0x%x \n" , (unsigned long)i, os_readl(i));

	for (i = SSUSB_SIFSLV_IPPC_BASE+0x0050 ; i<= SSUSB_SIFSLV_IPPC_BASE+0x0050; i += 0x4)
		printk( "[0x%lx] 0x%x \n" , (unsigned long)i, os_readl(i));

	for (i = SSUSB_SIFSLV_IPPC_BASE+0x007C ; i<= SSUSB_SIFSLV_IPPC_BASE+0x00A4; i += 0x4)
		printk( "[0x%lx] 0x%x \n" , (unsigned long)i, os_readl(i));

	printk( "SSUSB_SIFSLV_IPPC_BASE <================== \n");

	printk( "SSUSB_EPCTL_CSR_BASE ==================> \n");
	for (i = SSUSB_EPCTL_CSR_BASE+0x0000 ; i<= SSUSB_EPCTL_CSR_BASE+0x0028; i += 0x4)
		printk( "[0x%lx] 0x%x \n" , (unsigned long)i, os_readl(i));

	for (i = SSUSB_EPCTL_CSR_BASE+0x0030 ; i<= SSUSB_EPCTL_CSR_BASE+0x0030; i += 0x4)
		printk( "[0x%lx] 0x%x \n" , (unsigned long)i, os_readl(i));

	for (i = SSUSB_EPCTL_CSR_BASE+0x0040 ; i<= SSUSB_EPCTL_CSR_BASE+0x0048; i += 0x4)
		printk( "[0x%lx] 0x%x \n" , (unsigned long)i, os_readl(i));

	for (i = SSUSB_EPCTL_CSR_BASE+0x0050 ; i<= SSUSB_EPCTL_CSR_BASE+0x0054; i += 0x4)
		printk( "[0x%lx] 0x%x \n" , (unsigned long)i, os_readl(i));

	for (i = SSUSB_EPCTL_CSR_BASE+0x0060 ; i<= SSUSB_EPCTL_CSR_BASE+0x0064; i += 0x4)
		printk( "[0x%lx] 0x%x \n" , (unsigned long)i, os_readl(i));

	printk( "SSUSB_EPCTL_CSR_BASE <================== \n");

	printk( "SSUSB_USB3_SYS_CSR_BASE ==================> \n");
	for (i = SSUSB_USB3_SYS_CSR_BASE+0x0200 ; i<= SSUSB_USB3_SYS_CSR_BASE+0x022C; i += 0x4)
		printk( "[0x%lx] 0x%x \n" , (unsigned long)i, os_readl(i));

	for (i = SSUSB_USB3_SYS_CSR_BASE+0x0240 ; i<= SSUSB_USB3_SYS_CSR_BASE+0x0244; i += 0x4)
		printk( "[0x%lx] 0x%x \n" , (unsigned long)i, os_readl(i));

	for (i = SSUSB_USB3_SYS_CSR_BASE+0x0290 ; i<= SSUSB_USB3_SYS_CSR_BASE+0x029C; i += 0x4)
		printk( "[0x%lx] 0x%x \n" , (unsigned long)i, os_readl(i));

	for (i = SSUSB_USB3_SYS_CSR_BASE+0x02A0 ; i<= SSUSB_USB3_SYS_CSR_BASE+0x02AC; i += 0x4)
		printk( "[0x%lx] 0x%x \n" , (unsigned long)i, os_readl(i));

	for (i = SSUSB_USB3_SYS_CSR_BASE+0x02B0 ; i<= SSUSB_USB3_SYS_CSR_BASE+0x02B8; i += 0x4)
		printk( "[0x%lx] 0x%x \n" , (unsigned long)i, os_readl(i));

	for (i = SSUSB_USB3_SYS_CSR_BASE+0x02C0 ; i<= SSUSB_USB3_SYS_CSR_BASE+0x02DC; i += 0x4)
		printk( "[0x%lx] 0x%x \n" , (unsigned long)i, os_readl(i));

	printk( "SSUSB_USB3_SYS_CSR_BASE <================== \n");

	printk( "SSUSB_USB3_MAC_CSR ==================> \n");
	for (i = SSUSB_USB3_MAC_CSR_BASE+0x0000 ; i<= SSUSB_USB3_MAC_CSR_BASE+0x001C; i += 0x4)
		printk( "[0x%lx] 0x%x \n" , (unsigned long)i, os_readl(i));

	for (i = SSUSB_USB3_MAC_CSR_BASE+0x0050 ; i<= SSUSB_USB3_MAC_CSR_BASE+0x0054; i += 0x4)
		printk( "[0x%lx] 0x%x \n" , (unsigned long)i, os_readl(i));

	for (i = SSUSB_USB3_MAC_CSR_BASE+0x007C ; i<= SSUSB_USB3_MAC_CSR_BASE+0x00B0; i += 0x4)
		printk( "[0x%lx] 0x%x \n" , (unsigned long)i, os_readl(i));

	for (i = SSUSB_USB3_MAC_CSR_BASE+0x0010C ; i<= SSUSB_USB3_MAC_CSR_BASE+0x0148; i += 0x4)
		printk( "[0x%lx] 0x%x \n" , (unsigned long)i, os_readl(i));

	printk( "SSUSB_USB3_MAC_CSR <================== \n");

	printk("\n[mu3d_hal_dump_register]\n\n\n");
}

void mu3d_hal_u3dev_en(void)
{
	/*
                                
  */
	os_writel(U3D_USB3_CONFIG, USB3_EN);
	os_printk(K_INFO, "USB3_EN = 1\n");
}

/* 
                                          
  
 */
void mu3d_hal_u3dev_dis(void)
{
	/*
                                                       
  */
	os_writel(U3D_USB3_CONFIG, 0);
	os_printk(K_INFO, "USB3_EN = 0\n");
}

/* 
                                                     
                     
 */
void mu3d_hal_set_speed( USB_SPEED speed)
{
 	#ifndef EXT_VBUS_DET
 	os_writel(U3D_MISC_CTRL, 0);
	#else
 	os_writel(U3D_MISC_CTRL, 0x3);
 	#endif

	printk("mu3d_hal_set_speed speed=%d\n", speed);

	/*                  */
    if(speed == SSUSB_SPEED_FULL){
        os_writel(U3D_POWER_MANAGEMENT, os_readl(U3D_POWER_MANAGEMENT) & ~HS_ENABLE);
		mu3d_hal_u2dev_connect();
        g_u3d_setting.speed = SSUSB_SPEED_FULL;
    }
    else if(speed == SSUSB_SPEED_HIGH){
        os_writel(U3D_POWER_MANAGEMENT, os_readl(U3D_POWER_MANAGEMENT) | HS_ENABLE);
		mu3d_hal_u2dev_connect();
        g_u3d_setting.speed = SSUSB_SPEED_HIGH;
    }
	#ifdef SUPPORT_U3
	else if(speed == SSUSB_SPEED_SUPER){
        g_u3d_setting.speed = SSUSB_SPEED_SUPER;
		mu3d_hal_u2dev_disconn();
		mu3d_hal_u3dev_en();
    }
	#endif
    else{
        os_printk(K_ALET,"Unsupported speed!!\n");
        BUG_ON(1);
    }
}

/* 
                                             
         
 */
void mu3d_hal_pdn_cg_en(void)
{
#ifdef POWER_SAVING_MODE
	DEV_UINT8 speed = (os_readl(U3D_DEVICE_CONF) & SSUSB_DEV_SPEED);
	os_printk(K_INFO, "%s\n", __func__);

	switch (speed)
	{
		case SSUSB_SPEED_SUPER:
			os_printk(K_NOTICE, "Device: SUPER SPEED LOW POWER\n");
 			os_setmsk(U3D_SSUSB_U2_CTRL_0P, SSUSB_U2_PORT_PDN);
			break;
		case SSUSB_SPEED_HIGH:
			os_printk(K_NOTICE, "Device: HIGH SPEED LOW POWER\n");
			#ifdef SUPPORT_U3
 			os_setmsk(U3D_SSUSB_U3_CTRL_0P, SSUSB_U3_PORT_PDN);
			#endif
			break;
		case SSUSB_SPEED_FULL:
			os_printk(K_NOTICE, "Device: FULL SPEED LOW POWER\n");
			#ifdef SUPPORT_U3
			os_setmsk(U3D_SSUSB_U3_CTRL_0P, SSUSB_U3_PORT_PDN);
			#endif
			break;
		default:
			os_printk(K_NOTICE, "[ERROR] Are you kidding!?!?\n");
			break;
	}
#endif
}

void mu3d_hal_pdn_ip_port( DEV_UINT8 on, DEV_UINT8 touch_dis, DEV_UINT8 u3, DEV_UINT8 u2)
{
#ifdef POWER_SAVING_MODE
	os_printk(K_INFO, "%s on=%d, touch_dis=%d, u3=%d, u2=%d\n", __func__, on, touch_dis, u3, u2);
	if (on)
	{
		os_clrmsk(U3D_SSUSB_IP_PW_CTRL2, SSUSB_IP_DEV_PDN);

		#ifdef SUPPORT_U3
		if (u3)
			os_clrmsk(U3D_SSUSB_U3_CTRL_0P, SSUSB_U3_PORT_PDN
				| ((touch_dis) ? SSUSB_U3_PORT_DIS : 0));
		#endif
		if (u2)
			os_clrmsk(U3D_SSUSB_U2_CTRL_0P, SSUSB_U2_PORT_PDN
				| ((touch_dis) ? SSUSB_U2_PORT_DIS : 0));

		while(!(os_readl(U3D_SSUSB_IP_PW_STS1) & SSUSB_SYSPLL_STABLE));
	}
	else
	{
		#ifdef SUPPORT_U3
		if (u3)
			os_setmsk(U3D_SSUSB_U3_CTRL_0P, SSUSB_U3_PORT_PDN
				| ((touch_dis) ? SSUSB_U3_PORT_DIS : 0));
		#endif
		if (u2)
			os_setmsk(U3D_SSUSB_U2_CTRL_0P, SSUSB_U2_PORT_PDN
				| ((touch_dis) ? SSUSB_U2_PORT_DIS : 0));

		os_setmsk(U3D_SSUSB_IP_PW_CTRL2, SSUSB_IP_DEV_PDN);
	}
#else
	os_printk(K_INFO, "%s Does NOT support IP power down\n", __func__);
#endif
}

/* 
                                           
                     
 */
void mu3d_hal_det_speed( USB_SPEED speed, DEV_UINT8 det_speed)
{
	DEV_UINT8 temp;
	DEV_UINT16 cnt_down = 10000;

	printk("===Start polling===\n");

	//                   
	if (!det_speed)
	{
		while (!(os_readl(U3D_DEV_LINK_INTR) & SSUSB_DEV_SPEED_CHG_INTR))
		{
			os_ms_delay(1);
			cnt_down--;

			if (cnt_down == 0) {
				printk("WTF!!!!!!!!!!!!!!!!!!!");
				return;
			}
		}
	}
	else
	{
		while(!(os_readl(U3D_DEV_LINK_INTR) & SSUSB_DEV_SPEED_CHG_INTR));
	}
	//     
	//                                                                 
	//      

	printk("===Polling End===\n");

	os_writel(U3D_DEV_LINK_INTR, SSUSB_DEV_SPEED_CHG_INTR);

	temp = (os_readl(U3D_DEVICE_CONF) & SSUSB_DEV_SPEED);
	switch (temp)
	{
		case SSUSB_SPEED_SUPER:
			os_printk(K_EMERG, "Device: SUPER SPEED\n");
			break;
		case SSUSB_SPEED_HIGH:
			os_printk(K_EMERG, "Device: HIGH SPEED\n");
			break;
		case SSUSB_SPEED_FULL:
			os_printk(K_EMERG, "Device: FULL SPEED\n");
			break;
		case SSUSB_SPEED_INACTIVE:
			os_printk(K_EMERG, "Device: INACTIVE\n");
			break;
	}

	if (temp != speed)
	{
		os_printk(K_EMERG, "desired speed: %d, detected speed: %d\n", speed, temp);
		os_ms_delay(5000);
		//         
	}
}

/* 
                                             
                                                                                       
 */
DEV_INT32 mu3d_hal_write_fifo( DEV_INT32 ep_num, DEV_INT32 length, DEV_UINT8 *buf, DEV_INT32 maxp)
{
	DEV_UINT32 residue;
	DEV_UINT32 count;
	DEV_UINT32 temp;

	os_printk(K_DEBUG, "%s epnum=%d, len=%d, buf=%p, maxp=%d\n", __func__, ep_num, length, buf, maxp);

	count = residue = length;

	while(residue > 0) {

		if(residue==1) {
			temp=((*buf)&0xFF);
			os_writeb(USB_FIFO(ep_num), temp);
			buf += 1;
			residue -= 1;
		} else if(residue==2) {
			temp=((*buf)&0xFF)+(((*(buf+1))<<8)&0xFF00);
			os_writew(USB_FIFO(ep_num), temp);
			buf += 2;
			residue -= 2;
		} else if(residue==3) {
			temp=((*buf)&0xFF)+(((*(buf+1))<<8)&0xFF00);
			os_writew(USB_FIFO(ep_num), temp);
			buf += 2;

			temp=((*buf)&0xFF);
			os_writeb(USB_FIFO(ep_num), temp);
			buf += 1;
			residue -= 3;
		} else {
			temp=((*buf)&0xFF)+(((*(buf+1))<<8)&0xFF00)+(((*(buf+2))<<16)&0xFF0000)+(((*(buf+3))<<24)&0xFF000000);
			os_writel(USB_FIFO(ep_num), temp);
			buf += 4;
			residue -= 4;
		}
	}

	if (ep_num == 0) {
		if (count == 0) {
			os_printk(K_DEBUG,"USB_EP0_DATAEND %8X+\n", os_readl(U3D_EP0CSR));
			os_writel(U3D_EP0CSR, os_readl(U3D_EP0CSR) | EP0_DATAEND | EP0_TXPKTRDY);
			os_printk(K_DEBUG,"USB_EP0_DATAEND %8X-\n", os_readl(U3D_EP0CSR));
		} else {
#ifdef AUTOSET
			if (count < maxp) {
				os_writel(U3D_EP0CSR, os_readl(U3D_EP0CSR) | EP0_TXPKTRDY);
				os_printk(K_DEBUG,"EP0_TXPKTRDY\n");
			}
#else
			os_writel(U3D_EP0CSR, os_readl(U3D_EP0CSR)|EP0_TXPKTRDY);
#endif
		}
	} else {
		if (count == 0) {
			USB_WriteCsr32(U3D_TX1CSR0, ep_num, USB_ReadCsr32(U3D_TX1CSR0, ep_num) | TX_TXPKTRDY);
		} else {
#ifdef AUTOSET
			if(count < maxp) {
				USB_WriteCsr32(U3D_TX1CSR0, ep_num, USB_ReadCsr32(U3D_TX1CSR0, ep_num) | TX_TXPKTRDY);
				os_printk(K_DEBUG,"short packet\n");
			}
#else
			USB_WriteCsr32(U3D_TX1CSR0, ep_num, USB_ReadCsr32(U3D_TX1CSR0, ep_num) | TX_TXPKTRDY);
#endif
		}
	}
	return count;
}

/* 
                                           
                                             
 */
DEV_INT32 mu3d_hal_read_fifo( DEV_INT32 ep_num, DEV_UINT8 *buf)
{
    DEV_UINT16 count, residue;
	DEV_UINT32 temp;
 	DEV_UINT8 *bp = buf ;

	if (ep_num==0) {
		residue = count = os_readl(U3D_RXCOUNT0);
	} else {
		residue = count = (USB_ReadCsr32(U3D_RX1CSR3, ep_num)>>EP_RX_COUNT_OFST);
	}

	os_printk(K_DEBUG, "%s, req->buf=%p, cnt=%d\n", __func__, buf, count);

    while(residue > 0) {

	temp= os_readl(USB_FIFO(ep_num));

	/*                    */
	*bp = temp&0xFF;

	/*                           */
        if(residue>1)
		*(bp+1) = (temp>>8)&0xFF;

	/*                           */
        if(residue>2)
		*(bp+2) = (temp>>16)&0xFF;

	/*                           */
        if(residue>3)
		*(bp+3) = (temp>>24)&0xFF;

        if(residue>4) {
		bp = bp + 4;
        	residue = residue - 4;
       	} else {
		residue = 0;
	}
    }

#ifdef AUTOCLEAR
	if (ep_num==0) {
		if (!count) {
			os_writel(U3D_EP0CSR, os_readl(U3D_EP0CSR)|EP0_RXPKTRDY);
			os_printk(K_DEBUG, "EP0_RXPKTRDY\n");
		}
	} else {
		if(!count){
			USB_WriteCsr32(U3D_RX1CSR0, ep_num, USB_ReadCsr32(U3D_RX1CSR0, ep_num) | RX_RXPKTRDY);
			os_printk(K_ALET,"ZLP\n");
		}
	}
#else
	if (ep_num==0) {
		os_writel(U3D_EP0CSR, os_readl(U3D_EP0CSR)|EP0_RXPKTRDY);
		os_printk(K_DEBUG, "EP0_RXPKTRDY\n");
	} else {
		USB_WriteCsr32(U3D_RX1CSR0, ep_num, USB_ReadCsr32(U3D_RX1CSR0, ep_num) | RX_RXPKTRDY);
		os_printk(K_DEBUG, "RX_RXPKTRDY\n");
	}
#endif
    return count;
}

/* 
                                                                                      
                                        
 */
DEV_INT32 mu3d_hal_write_fifo_burst( DEV_INT32 ep_num, DEV_INT32 length, DEV_UINT8 *buf, DEV_INT32 maxp)
{
    DEV_UINT32 residue, count, actual;
	DEV_UINT32 temp;
    DEV_UINT8 *bp ;

	os_printk(K_DEBUG,"%s ep_num=%d, lenght=%d, buf=%p, maxp=%d\n",__func__, ep_num,length,buf,maxp);

	#if (BUS_MODE==PIO_MODE)
	/*                                                                       
                                     
  */
	printk("write_fifo_burst=ep_num=%d, lenght=%d, buf=%p, maxp=%d\n",ep_num,length,buf,maxp);
	#endif

	actual = 0;

#ifdef AUTOSET
 	while(!(USB_ReadCsr32(U3D_TX1CSR0, ep_num) & TX_FIFOFULL)) {
#endif

		if(length - actual > maxp) {
			count = residue = maxp;
		} else {
			count = residue = length - actual;
		}

		bp = buf + actual;

		while(residue > 0) {

			if(residue==1) {
				temp=((*buf)&0xFF);
				os_writeb(USB_FIFO(ep_num), temp);
				buf += 1;
				residue -= 1;
			} else if(residue==2) {
				temp=((*buf)&0xFF)+(((*(buf+1))<<8)&0xFF00);
				os_writew(USB_FIFO(ep_num), temp);
				buf += 2;
				residue -= 2;
			} else if(residue==3) {
				temp=((*buf)&0xFF)+(((*(buf+1))<<8)&0xFF00);
				os_writew(USB_FIFO(ep_num), temp);
				buf += 2;

				temp=((*buf)&0xFF);
				os_writeb(USB_FIFO(ep_num), temp);
				buf += 1;
				residue -= 3;
			} else {
				temp=((*buf)&0xFF)+(((*(buf+1))<<8)&0xFF00)+(((*(buf+2))<<16)&0xFF0000)+(((*(buf+3))<<24)&0xFF000000);
				os_writel(USB_FIFO(ep_num), temp);
				buf += 4;
				residue -= 4;
			}
		}
#ifdef NEVER
    	while(residue > 0){

			if(residue==1){
				temp=((*bp)&0xFF);
				os_writel(U3D_RISC_SIZE, RISC_SIZE_1B);
				unit=1;
			}
			else if(residue==2){
				temp=((*bp)&0xFF)+(((*(bp+1))<<8)&0xFF00);
				os_writel(U3D_RISC_SIZE, RISC_SIZE_2B);
				unit=2;
			}
			else if(residue==3){
				temp=((*bp)&0xFF)+(((*(bp+1))<<8)&0xFF00);
				os_writel(U3D_RISC_SIZE, RISC_SIZE_2B);
				unit=2;
			}
			else{
				temp=((*bp)&0xFF)+(((*(bp+1))<<8)&0xFF00)+(((*(bp+2))<<16)&0xFF0000)+(((*(bp+3))<<24)&0xFF000000);
				unit=4;
			}
			os_writel(USB_FIFO(ep_num), temp);
        	bp=bp+unit;
			residue-=unit;
    	}
		if(os_readl(U3D_RISC_SIZE)!=RISC_SIZE_4B){
			os_writel(U3D_RISC_SIZE, RISC_SIZE_4B);
		}
#endif /*       */
    	actual += count;

		if(length == 0) {
			USB_WriteCsr32(U3D_TX1CSR0, ep_num, USB_ReadCsr32(U3D_TX1CSR0, ep_num) | TX_TXPKTRDY);
			return actual;
		} else {
#ifdef AUTOSET
			if((count < maxp) && (count > 0)) {
				USB_WriteCsr32(U3D_TX1CSR0, ep_num, USB_ReadCsr32(U3D_TX1CSR0, ep_num) | TX_TXPKTRDY);
				os_printk(K_DEBUG,"short packet\n");
				return actual;
			}
			if(count == 0) {
				return actual;
			}
#else
			USB_WriteCsr32(U3D_TX1CSR0, ep_num, USB_ReadCsr32(U3D_TX1CSR0, ep_num) | TX_TXPKTRDY);
#endif
		}
#ifdef AUTOSET
	}
#endif
	return actual;
}

/* 
                                                                                     
                                            
 */
DEV_INT32 mu3d_hal_read_fifo_burst( DEV_INT32 ep_num, DEV_UINT8 *buf)
{
    DEV_UINT16 count, residue;
	DEV_UINT32 temp, actual;
 	DEV_UINT8 *bp;

	os_printk(K_INFO,"mu3d_hal_read_fifo_burst\n");
 	os_printk(K_ALET,"req->buf=%p\n", buf);
	actual = 0;
#ifdef AUTOCLEAR
	while(!(USB_ReadCsr32(U3D_RX1CSR0, ep_num) & RX_FIFOEMPTY)){
#endif
		residue = count = (USB_ReadCsr32(U3D_RX1CSR3, ep_num)>>EP_RX_COUNT_OFST);
 		os_printk(K_INFO,"count :%d ; req->actual :%d \n",count,actual);
		bp = buf + actual;

    	while(residue > 0){
			temp= os_readl(USB_FIFO(ep_num));
       		*bp = temp&0xFF;
			*(bp+1) = (temp>>8)&0xFF;
			*(bp+2) = (temp>>16)&0xFF;
			*(bp+3) = (temp>>24)&0xFF;
        	bp=bp+4;
        	if(residue>4){
        		residue=residue-4;
       		}
			else{
				residue=0;
			}
		}
    	actual += count;

#ifdef AUTOCLEAR
		if(!count){
			USB_WriteCsr32(U3D_RX1CSR0, ep_num, USB_ReadCsr32(U3D_RX1CSR0, ep_num) | RX_RXPKTRDY);
			os_printk(K_ALET,"zlp\n");
			os_printk(K_ALET,"actual :%d\n",actual);
			return actual;
		}
#else
		if(ep_num==0){
			os_writel(U3D_EP0CSR, os_readl(U3D_EP0CSR)|EP0_RXPKTRDY);
		}
		else{
			USB_WriteCsr32(U3D_RX1CSR0, ep_num, USB_ReadCsr32(U3D_RX1CSR0, ep_num) | RX_RXPKTRDY);
		}
#endif
#ifdef AUTOCLEAR
	}
#endif

	return actual;
}


/* 
                         
         
 */
void mu3d_hal_unfigured_ep(void)
{
    DEV_UINT32 i, tx_ep_num, rx_ep_num;
	struct USB_EP_SETTING *ep_setting;

	os_printk(K_INFO, "%s\n", __func__);

	g_TxFIFOadd = USB_TX_FIFO_START_ADDRESS;
	g_RxFIFOadd = USB_RX_FIFO_START_ADDRESS;

#ifdef HARDCODE_EP
	tx_ep_num = MAX_QMU_EP;//                                         
	rx_ep_num = MAX_QMU_EP;//                                                
#else
	tx_ep_num = os_readl(U3D_CAP_EPINFO) & CAP_TX_EP_NUM;
	rx_ep_num = (os_readl(U3D_CAP_EPINFO) & CAP_RX_EP_NUM) >> 8;
#endif

	for(i=1; i<=tx_ep_num; i++){
		USB_WriteCsr32(U3D_TX1CSR0, i, USB_ReadCsr32(U3D_TX1CSR0, i) & (~0x7FF));
		USB_WriteCsr32(U3D_TX1CSR1, i, 0);
  		USB_WriteCsr32(U3D_TX1CSR2, i, 0);
		ep_setting = &g_u3d_setting.ep_setting[i];
		ep_setting->fifoaddr = 0;
		ep_setting->enabled = 0;
	}

	for(i=1; i<=rx_ep_num; i++){
		USB_WriteCsr32(U3D_RX1CSR0, i, USB_ReadCsr32(U3D_RX1CSR0, i) & (~0x7FF));
		USB_WriteCsr32(U3D_RX1CSR1, i, 0);
		USB_WriteCsr32(U3D_RX1CSR2, i, 0);
		ep_setting = &g_u3d_setting.ep_setting[i+MAX_EP_NUM];
		ep_setting->fifoaddr = 0;
		ep_setting->enabled = 0;
	}
}

/* 
                             
         
 */
void mu3d_hal_unfigured_ep_num( DEV_UINT8 ep_num, USB_DIR dir)
{
    struct USB_EP_SETTING *ep_setting;

	os_printk(K_INFO, "%s %d\n", __func__, ep_num);

	if (dir == USB_TX) {
		USB_WriteCsr32(U3D_TX1CSR0, ep_num, USB_ReadCsr32(U3D_TX1CSR0, ep_num) & (~0x7FF));
		USB_WriteCsr32(U3D_TX1CSR1, ep_num, 0);
  		USB_WriteCsr32(U3D_TX1CSR2, ep_num, 0);
		ep_setting = &g_u3d_setting.ep_setting[ep_num];
		ep_setting->enabled = 0;
	}else{
		USB_WriteCsr32(U3D_RX1CSR0, ep_num, USB_ReadCsr32(U3D_RX1CSR0, ep_num) & (~0x7FF));
		USB_WriteCsr32(U3D_RX1CSR1, ep_num, 0);
		USB_WriteCsr32(U3D_RX1CSR2, ep_num, 0);
		ep_setting = &g_u3d_setting.ep_setting[ep_num+MAX_EP_NUM];
		ep_setting->enabled = 0;
	}
}

/* 
                                   
                                                                                                                                    
*/
void _ex_mu3d_hal_ep_enable(DEV_UINT8 ep_num, USB_DIR dir, TRANSFER_TYPE type, DEV_INT32 maxp,
                  DEV_INT8 interval, DEV_INT8 slot, DEV_INT8 burst, DEV_INT8 mult)
{
	DEV_INT32 ep_index=0;
	DEV_INT32 used_before;
	DEV_UINT8 fifosz=0, max_pkt, binterval;
	DEV_INT32 csr0, csr1, csr2;
	struct USB_EP_SETTING *ep_setting;
	DEV_UINT8 update_FIFOadd = 0;

	os_printk(K_INFO, "%s\n", __func__);

	/*                       */
	/*                         */
	//                                                                                                         

	/*
                                        
                       
                      
                                
                                
                        
  */

	if (slot > MAX_SLOT) {
		os_printk(K_ALET, "[ERROR]Configure wrong slot number(MAX=%d, Not=%d)\n", MAX_SLOT, slot);
		slot = MAX_SLOT;
	}

	if (type == USB_CTRL) {
		ep_setting = &g_u3d_setting.ep_setting[0];
		ep_setting->fifosz = maxp;
		ep_setting->maxp = maxp;
		csr0 = os_readl(U3D_EP0CSR) & EP0_W1C_BITS;
		csr0 |= maxp;
		os_writel(U3D_EP0CSR, csr0);

		os_setmsk(U3D_USB2_RX_EP_DATAERR_INTR, BIT16); //                        
		return;
	}

	if (dir == USB_TX) {
		ep_index = ep_num;
	} else if (dir == USB_RX) {
		ep_index = ep_num + MAX_EP_NUM;
	} else {
		BUG_ON(1);
	}

	ep_setting = &g_u3d_setting.ep_setting[ep_index];
	used_before = ep_setting->fifoaddr;

	if (ep_setting->enabled)
		return;

	binterval = interval;
	if (dir == USB_TX) {
		if((g_TxFIFOadd + maxp * (slot + 1) > os_readl(U3D_CAP_EPNTXFFSZ)) && (!used_before)) {
			os_printk(K_ALET,"mu3d_hal_ep_enable, FAILED: sram exhausted\n");
			os_printk(K_ALET,"g_FIFOadd :%x\n",g_TxFIFOadd );
			os_printk(K_ALET,"maxp :%d\n",maxp );
			os_printk(K_ALET,"mult :%d\n",slot );
			WARN_ON(1);
		}
	} else {
		if((g_RxFIFOadd + maxp * (slot + 1) > os_readl(U3D_CAP_EPNRXFFSZ)) && (!used_before)) {
			os_printk(K_ALET,"mu3d_hal_ep_enable, FAILED: sram exhausted\n");
			os_printk(K_ALET,"g_FIFOadd :%x\n",g_RxFIFOadd );
			os_printk(K_ALET,"maxp :%d\n",maxp );
			os_printk(K_ALET,"mult :%d\n",slot );
			WARN_ON(1);
		}
	}

	ep_setting->transfer_type = type;

	if (dir == USB_TX) {
		if(!ep_setting->fifoaddr) {
			ep_setting->fifoaddr = g_TxFIFOadd;
			update_FIFOadd = 1;
		}
	} else {
		if(!ep_setting->fifoaddr) {
			ep_setting->fifoaddr = g_RxFIFOadd;
			update_FIFOadd = 1;
		}
	}

	ep_setting->fifosz = maxp;
	ep_setting->maxp = maxp;
	ep_setting->dir = dir;
	ep_setting->enabled = 1;

	/*
                                                                                      
                                                                                                                
                                                                                                           
                                                                                                                     
  */
	if (maxp <=16)
		fifosz = USB_FIFOSZ_SIZE_16;
	else if (maxp <=32)
		fifosz = USB_FIFOSZ_SIZE_32;
	else if (maxp <=64)
		fifosz = USB_FIFOSZ_SIZE_64;
	else if (maxp <=128)
		fifosz = USB_FIFOSZ_SIZE_128;
	else if (maxp <=256)
		fifosz = USB_FIFOSZ_SIZE_256;
	else if (maxp <=512)
		fifosz = USB_FIFOSZ_SIZE_512;
	else if (maxp <=32768)
		fifosz = USB_FIFOSZ_SIZE_1024;
	else {
		os_printk(K_ERR, "%s Wrong MAXP\n", __func__);
		fifosz = USB_FIFOSZ_SIZE_1024;
	}

	if (dir == USB_TX) {
		//    
		csr0 = USB_ReadCsr32(U3D_TX1CSR0, ep_num) &~ TX_TXMAXPKTSZ;
		csr0 |= (maxp & TX_TXMAXPKTSZ);

#ifdef USE_SSUSB_QMU
		/*                                                         
                         
                       
                                   
                                          
                                     
                                          
                        
                         
   */
		csr0 &= ~TX_AUTOSET;
		csr0 |= TX_DMAREQEN;
#else
#ifdef AUTOSET
		csr0 |= TX_AUTOSET;
#endif
		/*                         */
		csr0 &= ~TX_DMAREQEN;
#endif

		//    
		max_pkt = (burst+1)*(mult+1)-1;
		csr1 = (burst & SS_TX_BURST);
		csr1 |= (slot<<TX_SLOT_OFST) & TX_SLOT;
		csr1 |= (max_pkt<<TX_MAX_PKT_OFST) & TX_MAX_PKT;
		csr1 |= (mult<<TX_MULT_OFST) & TX_MULT;

		//    
		csr2 = (ep_setting->fifoaddr>>4) & TXFIFOADDR;
		csr2 |= (fifosz<<TXFIFOSEGSIZE_OFST) & TXFIFOSEGSIZE;

		if(type == USB_BULK) {
			csr1 |= TYPE_BULK;
		} else if(type == USB_INTR) {
			csr1 |= TYPE_INT;
			csr2 |= (binterval<<TXBINTERVAL_OFST)&TXBINTERVAL;
		} else if(type == USB_ISO) {
			csr1 |= TYPE_ISO;
			csr2 |= (binterval<<TXBINTERVAL_OFST)&TXBINTERVAL;
		}

#ifdef USE_SSUSB_QMU
		/*                          */
		os_setmsk(U3D_EPIECR, (BIT0<<ep_num));
		/*            */
		os_setmsk(U3D_QGCSR, QMU_TX_EN(ep_num));
		/*                           */
		os_setmsk(U3D_QIESR0, QMU_TX_EN(ep_num));
#endif
		USB_WriteCsr32(U3D_TX1CSR0, ep_num, csr0);
		USB_WriteCsr32(U3D_TX1CSR1, ep_num, csr1);
		USB_WriteCsr32(U3D_TX1CSR2, ep_num, csr2);

		os_printk(K_INFO,"[CSR]U3D_TX%dCSR0 :%x\n", ep_num, USB_ReadCsr32(U3D_TX1CSR0, ep_num));
		os_printk(K_INFO,"[CSR]U3D_TX%dCSR1 :%x\n", ep_num, USB_ReadCsr32(U3D_TX1CSR1, ep_num));
		os_printk(K_INFO,"[CSR]U3D_TX%dCSR2 :%x\n", ep_num, USB_ReadCsr32(U3D_TX1CSR2, ep_num));

	}else if(dir == USB_RX) {
		//    
		csr0 = USB_ReadCsr32(U3D_RX1CSR0, ep_num) &~ RX_RXMAXPKTSZ;
		csr0 |= (maxp & RX_RXMAXPKTSZ);

#ifdef USE_SSUSB_QMU
		/*                                                         
                         
                       
                                   
                                          
                                     
                                          
                          
                         
   */
		csr0 &= ~RX_AUTOCLEAR;
		csr0 |= RX_DMAREQEN;
#else
#ifdef AUTOCLEAR
		csr0 |= RX_AUTOCLEAR;
#endif
		/*                         */
		csr0 &= ~RX_DMAREQEN;
#endif

		//    
		max_pkt = (burst+1)*(mult+1)-1;
		csr1 = (burst & SS_RX_BURST);
		csr1 |= (slot<<RX_SLOT_OFST) & RX_SLOT;
		csr1 |= (max_pkt<<RX_MAX_PKT_OFST) & RX_MAX_PKT;
		csr1 |= (mult<<RX_MULT_OFST) & RX_MULT;

		//    
		csr2 = (ep_setting->fifoaddr>>4) & RXFIFOADDR;
		csr2 |= (fifosz<<RXFIFOSEGSIZE_OFST) & RXFIFOSEGSIZE;

		if(type==USB_BULK) {
			csr1 |= TYPE_BULK;
		} else if(type==USB_INTR) {
			csr1 |= TYPE_INT;
			csr2 |= (binterval<<RXBINTERVAL_OFST)&RXBINTERVAL;
		} else if(type==USB_ISO) {
			csr1 |= TYPE_ISO;
			csr2 |= (binterval<<RXBINTERVAL_OFST)&RXBINTERVAL;
		}

#ifdef USE_SSUSB_QMU
		/*                          */
		os_setmsk(U3D_EPIECR, (BIT16<<ep_num));
		/*            */
		os_setmsk(U3D_QGCSR, QMU_TX_EN(ep_num));
		/*                         */
		os_setmsk(U3D_QIESR0, QMU_RX_EN(ep_num));
#endif
		USB_WriteCsr32(U3D_RX1CSR0, ep_num, csr0);
		USB_WriteCsr32(U3D_RX1CSR1, ep_num, csr1);
		USB_WriteCsr32(U3D_RX1CSR2, ep_num, csr2);

		os_printk(K_INFO,"[CSR]U3D_RX%dCSR0 :%x\n", ep_num, USB_ReadCsr32(U3D_RX1CSR0, ep_num));
		os_printk(K_INFO,"[CSR]U3D_RX%dCSR1 :%x\n", ep_num, USB_ReadCsr32(U3D_RX1CSR1, ep_num));
		os_printk(K_INFO,"[CSR]U3D_RX%dCSR2 :%x\n", ep_num, USB_ReadCsr32(U3D_RX1CSR2, ep_num));

		os_setmsk(U3D_USB2_RX_EP_DATAERR_INTR, BIT16<<ep_num); //                        
	} else {
		os_printk(K_ERR, "WHAT THE DIRECTION IS?!?!\n");
		BUG_ON(1);
	}

	if(update_FIFOadd == 1)
	{
		if(dir == USB_TX){
			/*                                                
                                                                  */
			int fifo_offset = 0;

			if ((maxp & 0xF))
				fifo_offset = ((maxp+16) >> 4) << 4;
			else
				fifo_offset = maxp;

			g_TxFIFOadd += (fifo_offset * (slot + 1));
		} else {
			int fifo_offset = 0;

			if ((maxp & 0xF))
				fifo_offset = ((maxp+16) >> 4) << 4;
			else
				fifo_offset = maxp;

			g_RxFIFOadd += (fifo_offset * (slot + 1));
		}
	}
}

void mu3d_hal_ep_enable(DEV_UINT8 ep_num, USB_DIR dir, TRANSFER_TYPE type, DEV_INT32 maxp,
                  DEV_INT8 interval, DEV_INT8 slot,DEV_INT8 burst, DEV_INT8 mult)
{
	DEV_INT32 ep_index=0;
	DEV_INT32 used_before;
	DEV_UINT8 fifosz=0, max_pkt, binterval;
	DEV_INT32 csr0, csr1, csr2;
	struct USB_EP_SETTING *ep_setting;
	DEV_UINT8 update_FIFOadd = 0;

	/*                         */
	os_writel( U3D_USB3_EPCTRL_CAP, os_readl(U3D_USB3_EPCTRL_CAP) | TX_NUMP_0_EN | SET_EOB_EN | TX_BURST_EN);

	if(slot > MAX_SLOT) {
		os_printk(K_ALET, "!!!!!!!!!!!!!!Configure wrong slot number!!!!!!!!!(MAX=%d, Not=%d)\n", MAX_SLOT, slot);
		slot = MAX_SLOT;
	}

	if(type == USB_CTRL) {

		ep_setting = &g_u3d_setting.ep_setting[0];
		ep_setting->fifosz = maxp;
		ep_setting->maxp = maxp;
		csr0 = os_readl(U3D_EP0CSR) & EP0_W1C_BITS;
		csr0 |= maxp;
		os_writel(U3D_EP0CSR, csr0);

		os_setmsk(U3D_USB2_RX_EP_DATAERR_INTR, BIT16); //                        
		return;
	}

	if(dir == USB_TX) {
		ep_index = ep_num;
	} else if(dir == USB_RX) {
		ep_index = ep_num + MAX_EP_NUM;
	} else {
		BUG_ON(1);
	}

	ep_setting = &g_u3d_setting.ep_setting[ep_index];
	used_before = ep_setting->fifoaddr;

	if(ep_setting->enabled)
		return;

	binterval = interval;
	if(dir == USB_TX) {
		if((g_TxFIFOadd + maxp * (slot + 1) > os_readl(U3D_CAP_EPNTXFFSZ)) && (!used_before)) {
			os_printk(K_ALET,"mu3d_hal_ep_enable, FAILED: sram exhausted\n");
			os_printk(K_ALET,"g_FIFOadd :%x\n",g_TxFIFOadd );
			os_printk(K_ALET,"maxp :%d\n",maxp );
			os_printk(K_ALET,"mult :%d\n",slot );
			WARN_ON(1);
		}
	} else {
		if((g_RxFIFOadd + maxp * (slot + 1) > os_readl(U3D_CAP_EPNRXFFSZ)) && (!used_before)) {
			os_printk(K_ALET,"mu3d_hal_ep_enable, FAILED: sram exhausted\n");
			os_printk(K_ALET,"g_FIFOadd :%x\n",g_RxFIFOadd );
			os_printk(K_ALET,"maxp :%d\n",maxp );
			os_printk(K_ALET,"mult :%d\n",slot );
			WARN_ON(1);
		}
	}

	ep_setting->transfer_type = type;

	if(dir == USB_TX) {
		if(!ep_setting->fifoaddr) {
			ep_setting->fifoaddr = g_TxFIFOadd;
			update_FIFOadd = 1;
		}
	} else {
		if(!ep_setting->fifoaddr) {
			ep_setting->fifoaddr = g_RxFIFOadd;
			update_FIFOadd = 1;
		}
	}

	ep_setting->fifosz = maxp;
	ep_setting->maxp = maxp;
	ep_setting->dir = dir;
	ep_setting->enabled = 1;

	if (maxp <=16)
			fifosz = USB_FIFOSZ_SIZE_16;
	else if (maxp <=32)
			fifosz = USB_FIFOSZ_SIZE_32;
	else if (maxp <=64)
			fifosz = USB_FIFOSZ_SIZE_64;
	else if (maxp <=128)
			fifosz = USB_FIFOSZ_SIZE_128;
	else if (maxp <=256)
			fifosz = USB_FIFOSZ_SIZE_256;
	else if (maxp <=512)
			fifosz = USB_FIFOSZ_SIZE_512;
	else if (maxp <=32768)
			fifosz = USB_FIFOSZ_SIZE_1024;
	else {
		os_printk(K_ERR, "%s Wrong MAXP\n", __func__);
			fifosz = USB_FIFOSZ_SIZE_1024;
		}

	if(dir == USB_TX) {
		//    
		csr0 = USB_ReadCsr32(U3D_TX1CSR0, ep_num) &~ TX_TXMAXPKTSZ;
		csr0 |= (maxp & TX_TXMAXPKTSZ);
#if (BUS_MODE==PIO_MODE)
#ifdef AUTOSET
		csr0 |= TX_AUTOSET;
#endif
		csr0 &= ~TX_DMAREQEN;
#endif

		//    
		max_pkt = (burst+1)*(mult+1)-1;
		csr1 = (burst & SS_TX_BURST);
		csr1 |= (slot<<TX_SLOT_OFST) & TX_SLOT;
		csr1 |= (max_pkt<<TX_MAX_PKT_OFST) & TX_MAX_PKT;
		csr1 |= (mult<<TX_MULT_OFST) & TX_MULT;

		//    
		csr2 = (ep_setting->fifoaddr>>4) & TXFIFOADDR;
		csr2 |= (fifosz<<TXFIFOSEGSIZE_OFST) & TXFIFOSEGSIZE;

		if(type == USB_BULK) {
			csr1 |= TYPE_BULK;
		} else if(type == USB_INTR) {
			csr1 |= TYPE_INT;
			csr2 |= (binterval<<TXBINTERVAL_OFST)&TXBINTERVAL;
		} else if(type == USB_ISO) {
			csr1 |= TYPE_ISO;
			csr2 |= (binterval<<TXBINTERVAL_OFST)&TXBINTERVAL;
		}

#ifdef USE_SSUSB_QMU
		os_writel(U3D_EPIECR, os_readl(U3D_EPIECR)|(BIT0<<ep_num));
#endif
		USB_WriteCsr32(U3D_TX1CSR0, ep_num, csr0);
		USB_WriteCsr32(U3D_TX1CSR1, ep_num, csr1);
		USB_WriteCsr32(U3D_TX1CSR2, ep_num, csr2);

		os_printk(K_INFO,"[CSR]U3D_TX1CSR0 :%x\n",USB_ReadCsr32(U3D_TX1CSR0, ep_num));
		os_printk(K_INFO,"[CSR]U3D_TX1CSR1 :%x\n",USB_ReadCsr32(U3D_TX1CSR1, ep_num));
		os_printk(K_INFO,"[CSR]U3D_TX1CSR2 :%x\n",USB_ReadCsr32(U3D_TX1CSR2, ep_num));

	}else if(dir == USB_RX) {
		//    
		csr0 = USB_ReadCsr32(U3D_RX1CSR0, ep_num) &~ RX_RXMAXPKTSZ;
		csr0 |= (maxp & RX_RXMAXPKTSZ);
#if (BUS_MODE==PIO_MODE)
#ifdef AUTOCLEAR
		csr0 |= RX_AUTOCLEAR;
#endif
		csr0 &= ~RX_DMAREQEN;
#endif

		//    
		max_pkt = (burst+1)*(mult+1)-1;
		csr1 = (burst & SS_RX_BURST);
		csr1 |= (slot<<RX_SLOT_OFST) & RX_SLOT;
		csr1 |= (max_pkt<<RX_MAX_PKT_OFST) & RX_MAX_PKT;
		csr1 |= (mult<<RX_MULT_OFST) & RX_MULT;

		//    
		csr2 = (ep_setting->fifoaddr>>4) & RXFIFOADDR;
		csr2 |= (fifosz<<RXFIFOSEGSIZE_OFST) & RXFIFOSEGSIZE;

		if(type==USB_BULK) {
			csr1 |= TYPE_BULK;
		} else if(type==USB_INTR) {
			csr1 |= TYPE_INT;
			csr2 |= (binterval<<RXBINTERVAL_OFST)&RXBINTERVAL;
		} else if(type==USB_ISO) {
			csr1 |= TYPE_ISO;
			csr2 |= (binterval<<RXBINTERVAL_OFST)&RXBINTERVAL;
		}

#ifdef USE_SSUSB_QMU
		os_writel(U3D_EPIECR, os_readl(U3D_EPIECR)|(BIT16<<ep_num));
#endif
		USB_WriteCsr32(U3D_RX1CSR0, ep_num, csr0);
		USB_WriteCsr32(U3D_RX1CSR1, ep_num, csr1);
		USB_WriteCsr32(U3D_RX1CSR2, ep_num, csr2);

		os_printk(K_INFO,"[CSR]U3D_RX1CSR0 :%x\n",USB_ReadCsr32(U3D_RX1CSR0, ep_num));
		os_printk(K_INFO,"[CSR]U3D_RX1CSR1 :%x\n",USB_ReadCsr32(U3D_RX1CSR1, ep_num));
		os_printk(K_INFO,"[CSR]U3D_RX1CSR2 :%x\n",USB_ReadCsr32(U3D_RX1CSR2, ep_num));

		os_setmsk(U3D_USB2_RX_EP_DATAERR_INTR, BIT16<<ep_num); //                        
	} else {
		os_printk(K_ERR, "WHAT THE DIRECTION IS?!?!\n");
		BUG_ON(1);
	}

	if(update_FIFOadd == 1)
	{
		if(dir == USB_TX){
			if(maxp == 1023){
				g_TxFIFOadd += (1024 * (slot + 1));
			} else {
				g_TxFIFOadd += (maxp * (slot + 1));
			}
		} else {
			if(maxp == 1023) {
				g_RxFIFOadd += (1024 * (slot + 1));
			} else {
				g_RxFIFOadd += (maxp * (slot + 1));
			}
		}
	}
}
