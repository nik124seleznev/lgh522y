#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/semaphore.h>
#include "disp_drv_log.h"
#include <mach/mt_boot.h>
#include "disp_drv_platform.h"
#include "lcd_drv.h"
#include "dsi_drv.h"
#include "dpi_drv.h"

#include "lcm_drv.h"
#include "disp_hal.h"
#include "debug.h"
#include "mach/mt_spm_idle.h"
#define ALIGN_TO(x, n)  \
	(((x) + ((n) - 1)) & ~((n) - 1))

//                           
//                                                                            
//                   
//                                                                            
static UINT32 dsiTmpBufBpp = 3;
extern LCM_DRIVER *lcm_drv;
extern LCM_PARAMS *lcm_params;
extern unsigned int is_video_mode_running;
extern BOOL DISP_IsDecoupleMode(void);
extern unsigned int FB_LAYER;

typedef struct
{
    UINT32 pa;
    UINT32 pitchInBytes;
} TempBuffer;

#ifndef MT65XX_NEW_DISP
static TempBuffer s_tmpBuffers[3];
#endif
static bool needStartDSI = false;

//                                                                            
//                   
//                                                                            

static void init_lcd_te_control(void)
{
    const LCM_DBI_PARAMS *dbi = &(lcm_params->dbi);

    LCD_CHECK_RET(LCD_TE_Enable(FALSE));
#ifdef BUILD_UBOOT
    {
        extern BOOTMODE g_boot_mode;
        printf("boot_mode = %d\n",g_boot_mode);
        if(g_boot_mode == META_BOOT)
            return;
    }
#endif
    if (LCM_DBI_TE_MODE_DISABLED == dbi->te_mode) {
        LCD_CHECK_RET(LCD_TE_Enable(FALSE));
        return;
    }

    if (LCM_DBI_TE_MODE_VSYNC_ONLY == dbi->te_mode) {
        LCD_CHECK_RET(LCD_TE_SetMode(LCD_TE_MODE_VSYNC_ONLY));
    } else if (LCM_DBI_TE_MODE_VSYNC_OR_HSYNC == dbi->te_mode) {
        LCD_CHECK_RET(LCD_TE_SetMode(LCD_TE_MODE_VSYNC_OR_HSYNC));
        LCD_CHECK_RET(LCD_TE_ConfigVHSyncMode(dbi->te_hs_delay_cnt,
                                              dbi->te_vs_width_cnt,
                     (LCD_TE_VS_WIDTH_CNT_DIV)dbi->te_vs_width_cnt_div));
    } else ASSERT(0);

    LCD_CHECK_RET(LCD_TE_SetEdgePolarity(dbi->te_edge_polarity));
    LCD_CHECK_RET(LCD_TE_Enable(TRUE));
}
static __inline LCD_IF_WIDTH to_lcd_if_width(LCM_DBI_DATA_WIDTH data_width)
{
    switch(data_width)
    {
    case LCM_DBI_DATA_WIDTH_8BITS  : return LCD_IF_WIDTH_8_BITS;
    case LCM_DBI_DATA_WIDTH_9BITS  : return LCD_IF_WIDTH_9_BITS;
    case LCM_DBI_DATA_WIDTH_16BITS : return LCD_IF_WIDTH_16_BITS;
    case LCM_DBI_DATA_WIDTH_18BITS : return LCD_IF_WIDTH_18_BITS;
    case LCM_DBI_DATA_WIDTH_24BITS : return LCD_IF_WIDTH_24_BITS;
    default : ASSERT(0);
    }
    return LCD_IF_WIDTH_18_BITS;
}

static BOOL disp_drv_dsi_init_context(void)
{
    if (lcm_drv != NULL && lcm_params != NULL){
		return TRUE;
	}

    if (NULL == lcm_drv) {
        return FALSE;
    }

    lcm_drv->get_params(lcm_params);


    return TRUE;
}

static void dsi_IsGlitchWorkaroundEnabled(void)
{
	//                                                   
	CHIP_SW_VER ver = mt_get_chip_sw_ver();
	//                                          
	if(ver >= CHIP_SW_VER_02 )
	{
		printk("this is 6582 E2 chip, disable glitch detect!\n");
		lcm_params->dsi.compatibility_for_nvk = 0;
	}
}

void init_dsi(BOOL isDsiPoweredOn)
{
	//                                           
	if (lcm_params->dsi.mode == CMD_MODE)
	{
    		DSI_PHY_clk_setting(lcm_params);
	}

    //                                                                              
    DSI_CHECK_RET(DSI_Init(isDsiPoweredOn));
	dsi_IsGlitchWorkaroundEnabled();

	if(0 < lcm_params->dsi.compatibility_for_nvk)
	{
    		DSI_CHECK_RET(DSI_TXRX_Control(TRUE,                    //       
                                   TRUE,                    //      
                                   lcm_params->dsi.LANE_NUM, //      
                                   0,                       //      
                                   lcm_params->dsi.null_packet_en,  //              
                                   FALSE,                   //                 
                                   FALSE,                   //           
								   FALSE,
                                   0));                     //               
//                               
//                                 
	}
	else
	{
		DSI_CHECK_RET(DSI_TXRX_Control(TRUE,                    //       
                                   TRUE,                    //      
                                   lcm_params->dsi.LANE_NUM, //      
                                   0,                       //      
                                   lcm_params->dsi.null_packet_en,  //              
                                   FALSE,                   //                 
                                   FALSE,                   //           
								   (BOOL)(1 - lcm_params->dsi.cont_clock),
                                   0));                     //               
	}

	//                  
#ifdef CONFIG_MIXMODE_FOR_INCELL
        if (lcm_params->dsi.mixmode_mipi_clock)
            lcm_params->dsi.PLL_CLOCK = lcm_params->dsi.mixmode_mipi_clock;
#endif    	
	DSI_PHY_clk_switch(TRUE);
	DSI_PHY_TIMCONFIG(lcm_params);

	DSI_CHECK_RET(DSI_PS_Control(lcm_params->dsi.PS, lcm_params->height, lcm_params->width * dsiTmpBufBpp));

	if(lcm_params->dsi.mode != CMD_MODE)
	{
		DSI_Config_VDO_Timing(lcm_params);
		DSI_Set_VM_CMD(lcm_params);
//                                               
//                             
    }

#ifdef CONFIG_MIXMODE_FOR_INCELL
    if (lcm_params->dsi.mixmode_enable)
    {
        DSI_MixMode_Start();
    }
#endif
    DSI_CHECK_RET(DSI_enable_MIPI_txio(TRUE));


}

//                                                                            
//                                     
//                                                                            
static DISP_STATUS dsi_config_ddp(UINT32 fbPA)
{
	struct disp_path_config_struct config = {0};
	if (DISP_IsDecoupleMode()) {
		config.srcModule = DISP_MODULE_RDMA;
	} else {
		config.srcModule = DISP_MODULE_OVL;
	}

	config.bgROI.x = 0;
	config.bgROI.y = 0;
	config.bgROI.width = lcm_params->width;
	config.bgROI.height = lcm_params->height;
	config.bgColor = 0x0;	//                 

	config.pitch = ALIGN_TO(lcm_params->width, MTK_FB_ALIGNMENT)*4;
	config.srcROI.x = 0;config.srcROI.y = 0;
	config.srcROI.height= lcm_params->height;
	config.srcROI.width= lcm_params->width;
	config.ovl_config.source = OVL_LAYER_SOURCE_MEM; 
	
	if(lcm_params->dsi.mode != CMD_MODE)
	{
		config.ovl_config.layer = DDP_OVL_LAYER_MUN-1;
		config.ovl_config.layer_en = 0;
		//                      
		disp_path_config_layer(&config.ovl_config);
		//                          
		//                            
	}
#if 1
		//                             
	if(lcm_params->dsi.mode != CMD_MODE)
	{
		config.ovl_config.layer = DDP_OVL_LAYER_MUN-1-1;
		config.ovl_config.layer_en = 0;
		//                      
		disp_path_config_layer(&config.ovl_config);
		//                          
		//                            
	}
#endif
		config.ovl_config.layer = DDP_OVL_LAYER_MUN-1;
		config.ovl_config.layer_en = 1; 
		config.ovl_config.fmt = eARGB8888;
		config.ovl_config.addr = fbPA;	
		config.ovl_config.source = OVL_LAYER_SOURCE_MEM; 
		config.ovl_config.src_x = 0;
		config.ovl_config.src_y = 0;
		config.ovl_config.dst_x = 0;	   //    
		config.ovl_config.dst_y = 0;
		config.ovl_config.dst_w = lcm_params->width;
		config.ovl_config.dst_h = lcm_params->height;
		config.ovl_config.src_pitch = ALIGN_TO(lcm_params->width, MTK_FB_ALIGNMENT)*4; //            
		config.ovl_config.keyEn = 0;
		config.ovl_config.key = 0xFF;	   //          
		config.ovl_config.aen = 0;			  //             
		config.ovl_config.alpha = 0;	

		/*                                               
                                                                   
                                                
                                                                             
                                                                                            
                                              */
		LCD_LayerSetAddress(FB_LAYER, fbPA);
		LCD_LayerSetFormat(FB_LAYER, eARGB8888);
		LCD_LayerSetOffset(FB_LAYER, 0, 0);
		LCD_LayerSetSize(FB_LAYER,lcm_params->width,lcm_params->height);
		LCD_LayerSetPitch(FB_LAYER, ALIGN_TO(lcm_params->width, MTK_FB_ALIGNMENT) * 4);
		LCD_LayerEnable(FB_LAYER, TRUE);

		if(lcm_params->dsi.mode == CMD_MODE)
			config.dstModule = DISP_MODULE_DSI_CMD;//                  
		else
			config.dstModule = DISP_MODULE_DSI_VDO;//                  
		config.outFormat = RDMA_OUTPUT_FORMAT_ARGB; 
		disp_path_config(&config);
		
	if(lcm_params->dsi.mode != CMD_MODE)
	{
		//                    
		disp_path_get_mutex();
	}

	//                                     
	{
		M4U_PORT_STRUCT portStruct;

		portStruct.ePortID = DISP_OVL_0;		   //                                             
		portStruct.Virtuality = 1;
		portStruct.Security = 0;
		portStruct.domain = 3;			  //                
		portStruct.Distance = 1;
		portStruct.Direction = 0;
		m4u_config_port(&portStruct);
	}

	if(lcm_params->dsi.mode != CMD_MODE)
	{
		disp_path_release_mutex();
		//                                    
		//              
		//            
	}
	printk("%s, config done\n", __func__);
	return DISP_STATUS_OK;
}

bool DDMS_capturing=0;

static DISP_STATUS dsi_init(UINT32 fbVA, UINT32 fbPA, BOOL isLcmInited)
{
	if (!disp_drv_dsi_init_context())
		return DISP_STATUS_NOT_IMPLEMENTED;

	if(lcm_params->dsi.mode == CMD_MODE)
	{
		init_dsi(isLcmInited);
		MASKREG32(DSI_BASE + 0x10, 0x2, 0x2);
		if (NULL != lcm_drv->init && !isLcmInited)
		{
			lcm_drv->init();
			DSI_LP_Reset();
		}

		DSI_clk_HS_mode(1);

		DSI_SetMode(lcm_params->dsi.mode);
	}
	else {
        if (!isLcmInited)
        {
	        	//               
	       	 	//            
	        	//           
        }
        else
        {
            is_video_mode_running = true;
        }
		
	init_dsi(isLcmInited);

	MASKREG32(DSI_BASE + 0x10, 0x2, 0x2);
	if (NULL != lcm_drv->init && !isLcmInited)
	{
		lcm_drv->init();
		DSI_LP_Reset();
	}

	DSI_SetMode(lcm_params->dsi.mode);
	}

	RDMASetTargetLine(0, lcm_params->height*4/5);

    dsi_config_ddp(fbPA);
	DPI_PowerOn();
	DPI_PowerOff();
#ifdef SPM_SODI_ENABLED
    if(lcm_params->dsi.mode == CMD_MODE)
    {
        spm_sodi_lcm_video_mode(FALSE);
    }
    else
    {
        spm_sodi_lcm_video_mode(TRUE);
    }
#endif
	return DISP_STATUS_OK;
}


//                                                  
static DISP_STATUS dsi_enable_power(BOOL enable)
{
	disp_drv_dsi_init_context();

	if(lcm_params->dsi.mode == CMD_MODE)
	{
		if (enable)
		{
			//                
			DSI_CHECK_RET(DSI_PowerOn());

			//                         
			DSI_PHY_clk_setting(lcm_params);

			//                     
			DSI_CHECK_RET(DSI_RestoreRegisters());

			//                      
			DSI_CHECK_RET(DSI_SleepOut());

			//              
			DSI_PHY_clk_switch(1);

			//             
			DSI_CHECK_RET(DSI_Wakeup());

			//             
			DSI_CHECK_RET(DSI_EnableClk());

			DSI_CHECK_RET(DSI_enable_MIPI_txio(TRUE));
			DSI_Reset();
		}
		else
		{
			//                    
			DSI_CHECK_RET(DSI_WaitForNotBusy());
			DSI_CHECK_RET(DSI_BackupRegisters());

			//                
			DSI_clk_ULP_mode(1);
			DSI_lane0_ULP_mode(1);
			DSI_clk_HS_mode(0);
			//              
			DSI_CHECK_RET(DSI_DisableClk());
			DSI_CHECK_RET(DSI_PowerOff());

			//                 
			DSI_PHY_clk_switch(0);

			//                                                                      
			DSI_CHECK_RET(DSI_enable_MIPI_txio(FALSE));
		}
	}
	else
	{
		if (enable)
		{
			//                
			DSI_CHECK_RET(DSI_PowerOn());

			//                         
			DSI_PHY_clk_setting(lcm_params);

			//                     
			DSI_CHECK_RET(DSI_RestoreRegisters());

			//                      
			DSI_CHECK_RET(DSI_SleepOut());

			//              
			DSI_PHY_clk_switch(1);

			//             
			DSI_CHECK_RET(DSI_Wakeup());
			DSI_clk_HS_mode(0);
			//             
			DSI_CHECK_RET(DSI_EnableClk());

			DSI_CHECK_RET(DSI_enable_MIPI_txio(TRUE));
			DSI_Reset();
			needStartDSI = true;
		}
		else
		{
			is_video_mode_running = false;

			//                    
			DSI_CHECK_RET(DSI_WaitForNotBusy());
			DSI_CHECK_RET(DSI_BackupRegisters());

			//                
			DSI_clk_ULP_mode(1);
			DSI_lane0_ULP_mode(1);

			//              
			DSI_CHECK_RET(DSI_DisableClk());
			DSI_CHECK_RET(DSI_PowerOff());

			//                 
			DSI_PHY_clk_switch(0);

			//                                                                      
			DSI_CHECK_RET(DSI_enable_MIPI_txio(FALSE));
	    	}
	}

	return DISP_STATUS_OK;
}


//                                                                                    
static DISP_STATUS dsi_update_screen(BOOL isMuextLocked)
{
	disp_drv_dsi_init_context();

    DSI_CHECK_RET(DSI_enable_MIPI_txio(TRUE));

	//                               

	DSI_SetMode(lcm_params->dsi.mode);
#ifndef MT65XX_NEW_DISP
	LCD_CHECK_RET(LCD_StartTransfer(FALSE));
#endif
	if (lcm_params->type==LCM_TYPE_DSI && lcm_params->dsi.mode == CMD_MODE && !DDMS_capturing) {
//                     
#ifdef MT65XX_NEW_DISP
        DSI_CHECK_RET(DSI_StartTransfer(isMuextLocked));
#else
				DSI_CHECK_RET(DSI_Start());
#endif
	}
	else if (lcm_params->type==LCM_TYPE_DSI && lcm_params->dsi.mode != CMD_MODE && !DDMS_capturing)
	{
		DSI_clk_HS_mode(1);
        DSI_CHECK_RET(DSI_StartTransfer(isMuextLocked));
#ifndef BUILD_UBOOT
		is_video_mode_running = true;
#ifndef MT65XX_NEW_DISP
		if(lcm_params->dsi.noncont_clock)
			DSI_set_noncont_clk(true, lcm_params->dsi.noncont_clock_period);

		if(lcm_params->dsi.lcm_int_te_monitor)
			DSI_set_int_TE(true, lcm_params->dsi.lcm_int_te_period);
#endif
#endif
	}

	if (DDMS_capturing)
		DISP_LOG_PRINT(ANDROID_LOG_INFO, "DSI", "[DISP] kernel - dsi_update_screen. DDMS is capturing. Skip one frame. \n");

	return DISP_STATUS_OK;
}


static UINT32 dsi_get_working_buffer_size(void)
{
    disp_drv_dsi_init_context();

	if(lcm_params->dsi.mode != CMD_MODE) {

            return
            lcm_params->width *
            lcm_params->height *
            dsiTmpBufBpp *
            lcm_params->dsi.intermediat_buffer_num;
	}

    return 0;
}

static UINT32 dsi_get_working_buffer_bpp(void)
{
    disp_drv_dsi_init_context();

	if(lcm_params->dsi.mode != CMD_MODE)
	{
            return dsiTmpBufBpp;
	}

    return 0;
}

static PANEL_COLOR_FORMAT dsi_get_panel_color_format(void)
{
    disp_drv_dsi_init_context();

	{

	    switch(lcm_params->dsi.data_format.format)
	    {
		    case LCM_DSI_FORMAT_RGB565 : return PANEL_COLOR_FORMAT_RGB565;
		    case LCM_DSI_FORMAT_RGB666 : return PANEL_COLOR_FORMAT_RGB666;
		    case LCM_DSI_FORMAT_RGB888 : return PANEL_COLOR_FORMAT_RGB888;
		    default : ASSERT(0);
	    }

	}
}

static UINT32 dsi_get_dithering_bpp(void)
{
	return PANEL_COLOR_FORMAT_TO_BPP(dsi_get_panel_color_format());
}


//                               
DISP_STATUS dsi_capture_framebuffer(UINT32 pvbuf, UINT32 bpp)
{
	DSI_CHECK_RET(DSI_WaitForNotBusy());

	DDMS_capturing=1;

	if(lcm_params->dsi.mode == CMD_MODE)
	{
        LCD_CHECK_RET(LCD_EnableDCtoDsi(FALSE));
#ifndef MT65XX_NEW_DISP
	    LCD_CHECK_RET(LCD_Capture_Framebuffer(pvbuf, bpp));
#else
		DSI_CHECK_RET(DSI_Capture_Framebuffer(pvbuf, bpp, true));//        
#endif
	}
	else
	{
		DSI_CHECK_RET(DSI_Capture_Framebuffer(pvbuf, bpp, false));//          
	}


	if(lcm_params->dsi.mode == CMD_MODE)
	{
        LCD_CHECK_RET(LCD_EnableDCtoDsi(TRUE));
	}

	DDMS_capturing=0;

	return DISP_STATUS_OK;
}


//                                 
//                                                  
BOOL dsi_esd_check(void)
{
	BOOL result = false;

	if(lcm_params->dsi.mode == CMD_MODE){
		result = lcm_drv->esd_check();
		return result;
	}
	else
	{
#ifndef BUILD_UBOOT
#ifndef MT65XX_NEW_DISP
		if(lcm_params->dsi.lcm_int_te_monitor)
			result = DSI_esd_check();

		if(result)
			return true;

		if(lcm_params->dsi.lcm_ext_te_monitor)
			result = LCD_esd_check();
#else
		result = DSI_esd_check();
		DSI_LP_Reset();
		needStartDSI = true;
		if(!result)
			dsi_update_screen(true);
#endif
		return result;
#endif
	}

}
void disp_dsi_late_prepare(void)
{
fbconfig_set_cmd_mode();//                                  

}


void disp_dsi_post(void)
{

fbconfig_set_vdo_mode();
needStartDSI =  true;
DSI_StartTransfer(true);
//                        
}

int fbconfig_mipi_clk_set(unsigned int clk)
{
DSI_STATUS ret = DSI_STATUS_ERROR;

ret = fbconfig_DSI_set_CLK(clk);
return ret ;
}

void fbconfig_mipi_lane_set(unsigned int lane_num)
{
	fbconfig_DSI_set_lane_num(lane_num);

}

//                                 
//                                                  
void dsi_esd_reset(void)
{
     	//                                
    	//                                            
   	if(lcm_params->dsi.mode == CMD_MODE)
	{
        	DSI_clk_HS_mode(0);
        	DSI_clk_ULP_mode(0);
        	DSI_lane0_ULP_mode(0);
    	}
	else
	{
		DSI_SetMode(CMD_MODE);
        	DSI_clk_HS_mode(0);
		//                            
		DSI_Reset();
	}

}

const DISP_IF_DRIVER *DISP_GetDriverDSI(void)
{
    static const DISP_IF_DRIVER DSI_DISP_DRV =
    {
        .init                   = dsi_init,
        .enable_power           = dsi_enable_power,
        .update_screen          = dsi_update_screen,
        .get_working_buffer_size = dsi_get_working_buffer_size,

        .get_panel_color_format = dsi_get_panel_color_format,
        .get_working_buffer_bpp = dsi_get_working_buffer_bpp,
        .init_te_control        = init_lcd_te_control,
        .get_dithering_bpp	= dsi_get_dithering_bpp,
        .capture_framebuffer	= dsi_capture_framebuffer,
        .esd_reset              = dsi_esd_reset,
        .esd_check				= dsi_esd_check,
    };

    return &DSI_DISP_DRV;
}

