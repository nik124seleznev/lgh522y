#include <linux/string.h>

#include "lcm_drv.h"

//                                                                            
//                 
//                                                                            

#define FRAME_WIDTH  										(320)
#define FRAME_HEIGHT 										(480)

#define REGFLAG_DELAY             							0XFE
#define REGFLAG_END_OF_TABLE      							0xFF   //                        

//                                                                            
//                 
//                                                                            

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    								(lcm_util.set_reset_pin((v)))

#define UDELAY(n) 											(lcm_util.udelay(n))
#define MDELAY(n) 											(lcm_util.mdelay(n))


//                                                                            
//                 
//                                                                            

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg											lcm_util.dsi_read_reg()
       

struct LCM_setting_table {
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[64];
};


static struct LCM_setting_table lcm_initialization_setting[] = {
	
	/*
       

                                            
 
                                           
                                           
                                           

                   

                                                     
                                           

    

                                  
 
                                 
 */

	{0xB9,	3,	{0xFF, 0x83, 0x63}},
	{REGFLAG_DELAY, 10, {}},

	{0xB0,	2,	{0x01, 0x09}},
	{REGFLAG_DELAY, 10, {}},	
	
#if 0
	{0xB1,	19,	{0x01, 0x00, 0x44, 0x07,
				 0x01, 0x11, 0x11, 0x2F,
				 0x37, 0x27, 0x27, 0x40,
				 0x32, 0x00, 0xE6, 0xE6,
				 0xE6, 0xE6, 0xE6}},
#else//            
{0xB1,	19, {0x01, 0x00, 0x44, 0x07,
			 0x01, 0x0E, 0x0E, 0x21,
			 0x29, 0x3F, 0x3F, 0x40,
			 0x32, 0x00, 0xE6, 0xE6,
			 0xE6, 0xE6, 0xE6}},

#endif
	{REGFLAG_DELAY, 10, {}},

	{0xB2,	2,	{0x08, 0x00}},
	{REGFLAG_DELAY, 10, {}},
	
	{0xB4,	7,	{0x02, 0x19, 0x9C, 0x08,
				 0x19, 0x05, 0x73}},
	{REGFLAG_DELAY, 10, {}},

	{0xB6,	1,	{0x1C}},
	{REGFLAG_DELAY, 10, {}},

	{0xBF,	1,	{0x05, 0x60, 0x00, 0x10}},
	{REGFLAG_DELAY, 10, {}},

#if 0
	{0xE2,	1,	{0x01}},//                        
	{REGFLAG_DELAY, 10, {}},

	{0xE5,	1,	{0x15}},//                      
	{REGFLAG_DELAY, 10, {}},
#endif
		
	//             
	{0x44,	2,	{((FRAME_HEIGHT/2)>>8), ((FRAME_HEIGHT/2)&0xFF)}},
	{0x35,	1,	{0x00}},
	//          
	#if 0
	{0xE0,	34,	{0x00, 0x0A, 0x0F, 0x2F,
				 0x32, 0x3F, 0x22, 0x42,
				 0x85, 0x89, 0xCC, 0xD1,
				 0xD6, 0x15, 0x15, 0x12,
				 0x18, 0x00, 0x0A, 0x0F,
				 0x2F, 0x32, 0x3F, 0x22,
				 0x42, 0x85, 0x89, 0xCC,
				 0xD1, 0xD6, 0x15, 0x15,
				 0x12, 0x18}},
#else//            
{0xE0,	34, {0x00, 0x06, 0x0A, 0x12,
			 0x15, 0x3B, 0x1D, 0x34,
			 0x87, 0x8E, 0xCC, 0xCF,
			 0xCE, 0x0E, 0x12, 0x11,
			 0x18, 0x00, 0x06, 0x0A,
			 0x12, 0x15, 0x3B, 0x1D,
			 0x34, 0x87, 0x8E, 0xCC,
			 0xCF, 0xCE, 0x0E, 0x12,
			 0x11, 0x18}},

#endif
	{REGFLAG_DELAY, 10, {}},

	{0xBA,	14,	{0x11, 0x00, 0x56, 0xC6,
							 0x10, 0x89, 0xFF, 0x0F,
							 0x33, 0x6E, 0x04, 0x07,
							 0x9A, 0x14}},

	{0xC2,	1,	{0x04}},
	{REGFLAG_DELAY, 10, {}},

	{0x3A,	1,	{0x77}},
	{REGFLAG_DELAY, 10, {}},

	{0x36,	1,	{0x0A}},
	{REGFLAG_DELAY, 10, {}},

	{0x51,	1,	{0x00}},
	{REGFLAG_DELAY, 10, {}},

	{0x53,	1,	{0x24}},
	{REGFLAG_DELAY, 10, {}},

	{0x55,	1,	{0x01}},
	{REGFLAG_DELAY, 10, {}},

	{0x5E,	1,	{0x70}},
	{REGFLAG_DELAY, 10, {}},

	{0xC9,	2,	{0x00,0X03}},//         
	{REGFLAG_DELAY, 10, {}},

	//     
	//                                                                                                                                  


	//                                  
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

/*
                                                    
                                                               
                                                                 
                                 
  
*/

static struct LCM_setting_table lcm_sleep_out_setting[] = {
    //          
	{0x11, 1, {0x00}},
    {REGFLAG_DELAY, 120, {}},

    //           
	{0x29, 1, {0x00}},
	{REGFLAG_DELAY, 20, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_deep_sleep_mode_in_setting[] = {
	//                     
	{0x28, 1, {0x00}},

    //              
	{0x10, 1, {0x00}},
    {REGFLAG_DELAY, 120, {}},

	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_backlight_level_setting[] = {
	{0x51, 1, {0xFF}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static struct LCM_setting_table lcm_backlight_mode_setting[] = {
	{0x55, 1, {0x1}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
	unsigned int i;

    for(i = 0; i < count; i++) {
		
        unsigned cmd;
        cmd = table[i].cmd;
		
        switch (cmd) {
			
            case REGFLAG_DELAY :
                MDELAY(table[i].count);
                break;
				
            case REGFLAG_END_OF_TABLE :
                break;
				
            default:
				dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
       	}
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
		params->dbi.te_mode 				= LCM_DBI_TE_MODE_VSYNC_ONLY;
		params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;

		params->dsi.mode   = CMD_MODE;

		//    
		/*                      */
		params->dsi.LANE_NUM				= LCM_TWO_LANE;
		//                                                                
		params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
		params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
		params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
		params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

		//                                         
		params->dsi.packet_size=256;

		//                     
		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;

		params->dsi.word_count=480*3;	
		params->dsi.vertical_sync_active=2;
		params->dsi.vertical_backporch=2;
		params->dsi.vertical_frontporch=2;
		params->dsi.vertical_active_line=800;
	
		params->dsi.line_byte=2180;		//             
		params->dsi.horizontal_sync_active_byte=26;
		params->dsi.horizontal_backporch_byte=206;
		params->dsi.horizontal_frontporch_byte=206;	
		params->dsi.rgb_byte=(480*3+6);	
	
		params->dsi.horizontal_sync_active_word_count=20;	
		params->dsi.horizontal_backporch_word_count=200;
		params->dsi.horizontal_frontporch_word_count=200;

		//                     
		params->dsi.pll_div1=38;		//                                                             
		params->dsi.pll_div2=1;			//                             

}


static void lcm_init(void)
{
    SET_RESET_PIN(1);
    SET_RESET_PIN(0);
    MDELAY(1);
    SET_RESET_PIN(1);
    MDELAY(10);

	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
}


static void lcm_suspend(void)
{
	push_table(lcm_deep_sleep_mode_in_setting, sizeof(lcm_deep_sleep_mode_in_setting) / sizeof(struct LCM_setting_table), 1);
}


static void lcm_resume(void)
{
	push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1);
}


static void lcm_update(unsigned int x, unsigned int y,
                       unsigned int width, unsigned int height)
{
	unsigned int x0 = x;
	unsigned int y0 = y;
	unsigned int x1 = x0 + width - 1;
	unsigned int y1 = y0 + height - 1;

	unsigned char x0_MSB = ((x0>>8)&0xFF);
	unsigned char x0_LSB = (x0&0xFF);
	unsigned char x1_MSB = ((x1>>8)&0xFF);
	unsigned char x1_LSB = (x1&0xFF);
	unsigned char y0_MSB = ((y0>>8)&0xFF);
	unsigned char y0_LSB = (y0&0xFF);
	unsigned char y1_MSB = ((y1>>8)&0xFF);
	unsigned char y1_LSB = (y1&0xFF);

	unsigned int data_array[16];

	data_array[0]= 0x00053902;
	data_array[1]= (x1_MSB<<24)|(x0_LSB<<16)|(x0_MSB<<8)|0x2a;
	data_array[2]= (x1_LSB);
	data_array[3]= 0x00053902;
	data_array[4]= (y1_MSB<<24)|(y0_LSB<<16)|(y0_MSB<<8)|0x2b;
	data_array[5]= (y1_LSB);
	data_array[6]= 0x002c3909;

	dsi_set_cmdq(data_array, 7, 0);

}


static void lcm_setbacklight(unsigned int level)
{
	unsigned int default_level = 0;
	unsigned int mapped_level = 0;

	//                                  
	if(level > 255) 
			level = 255;

	if(level >0) 
			mapped_level = default_level+(level)*(255-default_level)/(255);
	else
			mapped_level=0;

	//                                  
	lcm_backlight_level_setting[0].para_list[0] = mapped_level;

	push_table(lcm_backlight_level_setting, sizeof(lcm_backlight_level_setting) / sizeof(struct LCM_setting_table), 1);
}

static void lcm_setbacklight_mode(unsigned int mode)
{
	lcm_backlight_mode_setting[0].para_list[0] = mode;
	push_table(lcm_backlight_mode_setting, sizeof(lcm_backlight_mode_setting) / sizeof(struct LCM_setting_table), 1);
}

//                                            
// 
	//    
// 


//                                                    
// 
	//                                                                
	//                                                                  
//                                              
//                
// 


//                                                                            
//                      
//                                                                            
LCM_DRIVER hx8363_6575_dsi_hvga_lcm_drv = 
{
    .name			= "hx8363_6575_dsi_hvga",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.update         = lcm_update,
	.set_backlight	= lcm_setbacklight,
	.set_backlight_mode = lcm_setbacklight_mode,
	//                             
	//                              
};
