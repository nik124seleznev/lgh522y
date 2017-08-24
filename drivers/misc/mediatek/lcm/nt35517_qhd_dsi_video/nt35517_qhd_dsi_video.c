#ifndef BUILD_LK
#include <linux/string.h>
#endif
#include "lcm_drv.h"

#ifdef BUILD_LK
    #include <string.h>
    #include <platform/mt_pmic.h>
	#include <platform/mt_gpio.h>
    #include <platform/upmu_common.h>
#elif defined(BUILD_UBOOT)
	#include <asm/arch/mt_gpio.h>
    #include <platform/upmu_common.h>
#else
	#include <mach/mt_gpio.h>
	#include <mach/upmu_common.h>
    #include <mach/mt_pm_ldo.h>
#endif

#ifdef BUILD_LK
#define LCM_PRINT printf
#else
#if defined(BUILD_UBOOT)
#define LCM_PRINT printf
#else
#define LCM_PRINT printk
#endif
#endif

#define LCM_DBG(fmt, arg...) \
	LCM_PRINT ("[LCM-NT35517-QHD-DSI-VDO] %s (line:%d) :" fmt "\r\n", __func__, __LINE__, ## arg)

//                                                                            
//                 
//                                                                            

#define FRAME_WIDTH  										(540)
#define FRAME_HEIGHT 										(960)
#define  LCM_ID_NT35517 0x5517

#define GPIO_LCM_RST      GPIO83


#define REGFLAG_DELAY             							0XFE
#define REGFLAG_END_OF_TABLE      							0xFF   //                        

#define LCM_DSI_CMD_MODE									0

//                                                                   

//                                                                            
//                 
//                                                                            

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))


//                                                                            
//                 
//                                                                            

#define dsi_set_cmdq_V3(para_tbl,size,force_update)        lcm_util.dsi_set_cmdq_V3(para_tbl,size,force_update)
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	        lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)											lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)   				lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)

#ifndef BUILD_LK
static bool fgisFirst = TRUE;
#endif


static LCM_setting_table_V3 lcm_initialization_setting[] = {
	
	/*
       

                                            
 
                                           
                                           
                                           

                   

                                                     
                                           

    

                                  
 
                                 
 */
/*
     
                                          
 
                                                                                                                                                                                                                                      
 
                                                             
 
                                           
          
                                                 
 
                         
 
                               
 
                                           
 
                         
 
                         
 
                                                 
 
                                                       

                         
 
                         
 
                         
 
                                      
                           
                                           
         
                                                 
 
            
                                   
 
            
                                   
 
             
                                   
 
             
                                   
 
            
                                   
 
            
                                   
 
          
                                     
 
                         
                                   
 
              
                                   
 
                      
                                   
 
                             
                                   
 
                         
                                   
 
                         
 
                         
 
                                         
 
                 
     
                                                                                                     
                                                                                                     
                                                                                                     
                                         
     
                                                                                                     
                                                                                                     
                                                                                                     
                                         
     
                                                                                                     
                                                                                                     
                                                                                                     
                                         
      
                                                                                                     
                                                                                                     
                                                                                                     
                                         
     
                                                                                                     
                                                                                                     
                                                                                                     
                                         
     
                                                                                                     
                                                                                                     
                                                                                                     
                                         
 
                                                
                            
                                      
                                           
                                       
                                                     
                            
            
                            
                            
 
                    
                                                  
                  
                                                 
     
                                               
 
       
                                     
 
         
                                     
 
        
                                     
 
       
                                     
 
       
                                     
 
                                               
                                     
                                     
                                     
                                     
                                     
 
                                
 
                                     
 
                                
                                     
 
                              
                         
 
                                      
                               
                         
                                        
 
                 
     
                                                                                                     
                                                                                                     
                                                                                                     
                                         
     
                                                                                                     
                                                                                                     
                                                                                                     
                                         
     
                                                                                                     
                                                                                                     
                                                                                                     
                                         
      
                                                                                                     
                                                                                                     
                                                                                                     
                                         
     
                                                                                                     
                                                                                                     
                                                                                                     
                                         
     
                                                                                                     
                                                                                                     
                                                                                                     
                                         
 
                                              
                          
                                    
                                         
                                     
                                                   
                          
            
                          
                          
 
                    
                                                  
                  
                                                 
      

*/

       {0x39, 0xF0, 5, {0x55, 0xAA, 0x52,0x08,0x00}},

	{0x15, 0xB3, 1, {0x80}},

	{0x39, 0xB1, 3, {0xFC, 0x00, 0x00}},

	{0x15, 0xB6, 1, {0x05}},

	{0x39, 0xB7, 2, {0x74, 0x74}},

	{0x39, 0xB8, 4, {0x01, 0x06, 0x06,0x06}},

	{0x15, 0xBA, 1, {0x06}},

	{0x15, 0xBB, 1, {0x33}},

	{0x15, 0xBC, 1, {0x00}},
	
	{0x39, 0xC9, 6, {0x61,0x06,0x0D,0x17,0x17,0x00}},

	{0x39, 0xF0, 5, {0x55,0xAA,0x52,0x08,0x01}},

	{0x15, 0xB0, 1, {0x0A}},

	{0x15, 0xB6, 1, {0x34}},

	{0x15, 0xB1, 1, {0x0A}},

	{0x15, 0xB7, 1, {0x24}},
	
	{0x15, 0xB2, 1, {0x02}},

	{0x15, 0xB8, 1, {0x36}},

	{0x15, 0xB3, 1, {0x10}},

	{0x15, 0xB9, 1, {0x34}},

	{0x15, 0xB4, 1, {0x06}},

	{0x15, 0xBA, 1, {0x24}},

	{0x39, 0xBC, 3, {0x00,0x60,0x00}},

	{0x39, 0xBD, 3, {0x00,0x60,0x00}},

	//                        

	{0x15, 0xBE, 1, {0x2D}},

	//                        

	{0x39, 0xD1, 16, {0x00,0x00,0x00,0x01,0x00,0x03,0x00,0x09,0x00,0x12,0x00,0x2F,0x00,0x59,0x00,0x9F}},

	{0x39, 0xD2, 16, {0x00,0xD6,0x01,0x25,0x01,0x5D,0x01,0xAB,0x01,0xE1,0x01,0xE2,0x02,0x15,0x02,0x46}},

	{0x39, 0xD3, 16, {0x02,0x61,0x02,0x7D,0x02,0x92,0x02,0xAB,0x02,0xB9,0x02,0xD3,0x02,0xE4,0x02,0xFF}},

	{0x39, 0xD4, 4, {0x03,0x44,0x03,0xFF}},

	{0x39, 0xD5, 16, {0x00,0x00,0x00,0x01,0x00,0x03,0x00,0x09,0x00,0x12,0x00,0x2F,0x00,0x59,0x00,0x9F}},

	{0x39, 0xD6, 16, {0x00,0xD6,0x01,0x25,0x01,0x5D,0x01,0xAB,0x01,0xE1,0x01,0xE2,0x02,0x15,0x02,0x46}},

	{0x39, 0xD7, 16, {0x02,0x61,0x02,0x7D,0x02,0x92,0x02,0xAB,0x02,0xB9,0x02,0xD3,0x02,0xE4,0x02,0xFF}},

	{0x39, 0xD8, 4, {0x03,0x44,0x03,0xFF}},

	{0x39, 0xD9, 16, {0x00,0x00,0x00,0x01,0x00,0x03,0x00,0x09,0x00,0x12,0x00,0x2F,0x00,0x59,0x00,0x9F}},

	{0x39, 0xDD, 16, {0x00,0xD6,0x01,0x25,0x01,0x5D,0x01,0xAB,0x01,0xE1,0x01,0xE2,0x02,0x15,0x02,0x46}},

	{0x39, 0xDE, 16, {0x02,0x61,0x02,0x7D,0x02,0x92,0x02,0xAB,0x02,0xB9,0x02,0xD3,0x02,0xE4,0x02,0xFF}},

	{0x39, 0xDF, 4, {0x03,0x44,0x03,0xFF}},	

	{0x39, 0xE0, 16, {0x00,0x00,0x00,0x01,0x00,0x03,0x00,0x09,0x00,0x12,0x00,0x2F,0x00,0x59,0x00,0x9F}},

	{0x39, 0xE1, 16, {0x00,0xD6,0x01,0x25,0x01,0x5D,0x01,0xAB,0x01,0xE1,0x01,0xE2,0x02,0x15,0x02,0x46}},

	{0x39, 0xE2, 16, {0x02,0x61,0x02,0x7D,0x02,0x92,0x02,0xAB,0x02,0xB9,0x02,0xD3,0x02,0xE4,0x02,0xFF}},

	{0x39, 0xE3, 4, {0x03,0x44,0x03,0xFF}},

	{0x39, 0xE4, 16, {0x00,0x00,0x00,0x01,0x00,0x03,0x00,0x09,0x00,0x12,0x00,0x2F,0x00,0x59,0x00,0x9F}},

	{0x39, 0xE5, 16, {0x00,0xD6,0x01,0x25,0x01,0x5D,0x01,0xAB,0x01,0xE1,0x01,0xE2,0x02,0x15,0x02,0x46}},

	{0x39, 0xE6, 16, {0x02,0x61,0x02,0x7D,0x02,0x92,0x02,0xAB,0x02,0xB9,0x02,0xD3,0x02,0xE4,0x02,0xFF}},

	{0x39, 0xE7, 4, {0x03,0x44,0x03,0xFF}},

	{0x39, 0xE8, 16, {0x00,0x00,0x00,0x01,0x00,0x03,0x00,0x09,0x00,0x12,0x00,0x2F,0x00,0x59,0x00,0x9F}},

	{0x39, 0xE9, 16, {0x00,0xD6,0x01,0x25,0x01,0x5D,0x01,0xAB,0x01,0xE1,0x01,0xE2,0x02,0x15,0x02,0x46}},

	{0x39, 0xEA, 16, {0x02,0x61,0x02,0x7D,0x02,0x92,0x02,0xAB,0x02,0xB9,0x02,0xD3,0x02,0xE4,0x02,0xFF}},

	{0x39, 0xEB, 4, {0x03,0x44,0x03,0xFF}},

	{0x15, 0xC0, 1, {0xC0}},
	
	{0x15, 0xC2, 1, {0x20}},

	{0x15, 0x35, 1, {0x00}},

	{0x39, 0xFF, 4, {0xAA,0x55,0x25,0x01}},

	{0x15, 0x6F, 1, {0x0B}},

	{0x39, 0xF4, 4, {0x12,0x12,0x56,0x13}},
	
	{0x05,0x11,0,{}},		
	{REGFLAG_ESCAPE_ID,REGFLAG_DELAY_MS_V3, 120, {}},
	{0x05,0x29,0,{}},
	{REGFLAG_ESCAPE_ID,REGFLAG_DELAY_MS_V3, 10, {}},

       
};




static void lcd_reset(unsigned char enabled)
{    
	#ifdef BUILD_LK
		printf("%s,  %d LK \n", __func__, enabled);
	#else
		printk("%s,  %d kernel \n", __func__, enabled);
	#endif
    if (enabled)
    {
        mt_set_gpio_mode(GPIO_LCM_RST, GPIO_MODE_00);
	    mt_set_gpio_dir(GPIO_LCM_RST, GPIO_DIR_OUT);
        mt_set_gpio_out(GPIO_LCM_RST, GPIO_OUT_ONE);
    }
    else
    {	
        mt_set_gpio_mode(GPIO_LCM_RST, GPIO_MODE_00);
	    mt_set_gpio_dir(GPIO_LCM_RST, GPIO_DIR_OUT);
        mt_set_gpio_out(GPIO_LCM_RST, GPIO_OUT_ZERO);    	
    }
}

 
static void lcd_power_en(unsigned char enabled)
{
    #ifdef BUILD_LK
		printf("%s,  %d LK \n", __func__, enabled);
	#else
		printk("%s, %d kernel", __func__, enabled);
	#endif

   //              
    
    if (enabled)
    {      

        //                                                  

    }
    else
    {      

        //                                          
        //                                             
    }
}

//                                                                            
//                            
//                                                                            

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
	memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}

static void lcm_get_params(LCM_PARAMS *params)
{
		memset(params, 0, sizeof(LCM_PARAMS));
	
		params->type   = LCM_TYPE_DSI;

		params->width  = FRAME_WIDTH;
		params->height = FRAME_HEIGHT;
		
	    //                    
	    //                                                     
	    //                                                    
	
        #if (LCM_DSI_CMD_MODE)
		params->dsi.mode   = CMD_MODE;
        #else
		params->dsi.mode   = SYNC_EVENT_VDO_MODE; //                                      
        #endif
	
		//    
		/*                      */
		//                         
		params->dsi.LANE_NUM				= LCM_TWO_LANE;
		//                                                                
		params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

		//                     
		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
		
		params->dsi.vertical_sync_active				= 0x05;//       
		params->dsi.vertical_backporch					= 14;//       
		params->dsi.vertical_frontporch					= 12; //      
		params->dsi.vertical_active_line				= FRAME_HEIGHT; 

		params->dsi.horizontal_sync_active				= 0x16;//      
		params->dsi.horizontal_backporch				= 80;//         
		params->dsi.horizontal_frontporch				= 50;
		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;

	    //                   

		//                     
		//                  
		//                         
		//                                                    
		//                                
		params->dsi.PLL_CLOCK=240;
		//                               
	 	//                                                                                    
	 //                                                            
//                      
	 //                        
//     
	//                                                                                          
//      

}

/*                                        
 

                   
                         
                          

                
                  
                  
            
 
                  
              
     
           
 
                     
                     
                     
                           
            

                       
                           

                                                                                   
                             

                              
      
                             

                            
              
        
              
 */

static void lcm_init(void)
{

		LCM_DBG();
		#ifdef BUILD_LK
		printf("%s, LK \n", __func__);
	#else
		printk("%s, kernel", __func__);
	#endif
		//                 
		//                 

	#ifdef BUILD_LK
             upmu_set_rg_vgp6_vosel(0x7);
             upmu_set_rg_vgp6_sw_en(0x1);
	#else
		hwPowerOn(MT65XX_POWER_LDO_VGP6, VOL_3300, "LCM");  //                
	#endif
		MDELAY(80);
		lcd_reset(1);
		MDELAY(5);
		lcd_reset(0);
		MDELAY(5); 
		lcd_reset(1);
		MDELAY(50); 
		dsi_set_cmdq_V3(lcm_initialization_setting,sizeof(lcm_initialization_setting)/sizeof(lcm_initialization_setting[0]),1);		
}

static void lcm_suspend(void)
{
	unsigned int data_array[16];

	//                                        
	//                               
	#ifdef BUILD_LK
		printf("%s, LK \n", __func__);
	#else
		printk("%s, kernel", __func__);
	#endif
	data_array[0] = 0x00110500; //         
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(100); 

#ifdef BUILD_LK

	lcd_reset(0);
	MDELAY(10); //    
	//
	//                 

	//             
	MDELAY(120); 

	
	MDELAY(50);

#else
	//                  
    //
	//                 
	//             
	lcd_reset(0);
	MDELAY(10); //    
	//
	//                 

	//             
	MDELAY(120); 

if(fgisFirst == TRUE)
{
     fgisFirst = FALSE;
        hwPowerOn(MT65XX_POWER_LDO_VGP6, VOL_3300, "LCM");
}
	
        hwPowerDown(MT65XX_POWER_LDO_VGP6, "LCM");
	MDELAY(50);
#endif
	
}

static void lcm_resume(void)
{
	LCM_DBG();
	#ifdef BUILD_LK
		printf("%s, LK \n", __func__);
	#else
		printk("%s, kernel", __func__);
	#endif
#ifdef BUILD_LK
    		upmu_set_rg_vgp6_vosel(0x7);
   		upmu_set_rg_vgp6_sw_en(0x1);
		MDELAY(80);
		lcd_reset(1);
		MDELAY(5);
		lcd_reset(0);
		MDELAY(5); 
		lcd_reset(1);
		MDELAY(50); 
		dsi_set_cmdq_V3(lcm_initialization_setting,sizeof(lcm_initialization_setting)/sizeof(lcm_initialization_setting[0]),1);		
    MDELAY(50);


#else
		hwPowerOn(MT65XX_POWER_LDO_VGP6, VOL_3300, "LCM");
		MDELAY(80);
		lcd_reset(1);
		MDELAY(5);
		lcd_reset(0);
		MDELAY(5); 
		lcd_reset(1);
		MDELAY(50); 
		dsi_set_cmdq_V3(lcm_initialization_setting,sizeof(lcm_initialization_setting)/sizeof(lcm_initialization_setting[0]),1);		
    MDELAY(50);
#endif
}

/*                                       
 
                 

                 
                
 
                       
                           
 
                             

                    
  
               
  
     
   
              
  
      
 
              

       
 */

/*                                         
 
            
             
 */


LCM_DRIVER nt35517_qhd_dsi_vdo_lcm_drv = 
{
	.name		= "nt35517_qhd_dsi_vdo",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	//                                
	//                           
	//                               
#if (LCM_DSI_CMD_MODE)	
	.update         = lcm_update,
#endif
};
