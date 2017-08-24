#ifndef BUILD_UBOOT
#include <linux/kernel.h>
#endif
#include <linux/string.h>

#include "lcm_drv.h"

//                                                                            
//                 
//                                                                            

#define FRAME_WIDTH  (480)
#define FRAME_HEIGHT (800)

//                                                                            
//                 
//                                                                            

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))
#define REGFLAG_DELAY             							0XFE
#define REGFLAG_END_OF_TABLE      							0x00   //                        


//                                                                            
//                 
//                                                                            
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)									lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)				lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg											lcm_util.dsi_read_reg()
#define read_reg_v2(cmd, buffer, buffer_size)   				lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)      

#define LCM_DSI_CMD_MODE

struct LCM_setting_table {
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[64];
};


static struct LCM_setting_table lcm_initialization_setting[] = {
	
	/*
       

                                            
 
                                           
                                           
                                           

                   

                                                     
                                           

    

                                  
 
                                 
 */

	
	{0xB9,3,{0xff,0x83,0x69}},
	{0xB0,2,{0x01,0x0B}},
	{0xB1,19,{0x01,0x00,0x34,0x07,0x00,0x11,0x11,0x34,0x3A,0x3F,0x3F,0x07,0x23,0x01,0xE6,0xE6,0xE6,0xE6,0xE6}},
	{0xB2,15,{0x00,0x20,0x03,0x03,0x70,0x00,0xFF,0x00,0x00,0x00,0x00,0x03,0x03,0x00,0x01}},
	{0xB4,5,{0x00,0x0C,0xA0,0x0E,0x06}},
	{0xB6,2,{0x35,0x35}},
	
	/*            */
	{0x44,	2,	{((FRAME_HEIGHT/2)>>8), ((FRAME_HEIGHT/2)&0xFF)}},
	{0x35,	1,	{0x00}},
	
	/*           */
	{0x51,	1,	{0xFF}},
	{0x53,	1,	{0x24}},
	{0x55,	1,	{0x01}},
	
	{0xD5,26,{0x00,0x05,0x03,0x00,0x01,0x09,0x10,0x80,0x37,0x37,0x20,0x31,0x46,0x8A,0x57,0x9B,0x20,0x31,0x46,0x8A,0x57,0x9B,0x07,0x0F,0x02,0x00}},
	{0xE0,34,{0x00,0x0D,0x11,0x31,0x33,0x37,0x1E,0x3A,0x07,0x0F,0x0E,0x12,0x15,0x14,0x16,0x10,0x15,0x00,0x0D,0x11,0x31,0x33,0x37,0x1E,0x3A,0x07,0x0F,0x0E,0x12,0x15,0x14,0x16,0x10,0x15}},
    {REGFLAG_DELAY, 10, {}},
     /*
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
       */
    {REGFLAG_DELAY, 10, {}},
	{0xBA,13,{0x00,0xA0,0xC6,0x00,0x0A,0x00,0x10,0x30,0x6F,0x02,0x11,0x18,0x40}},
    {REGFLAG_DELAY, 10, {}},
	{0x3A,1,{0x77}},
	{0x2A,4,{0x00,0x00,0x01,0xDF}},
	{0x2B,4,{0x00,0x00,0x03,0x1F}},
	{0x11,0,{}},
	{REGFLAG_DELAY, 120, {}},
	{0x29,0,{}},
	{0x2C,0,{}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_set_window[] = {
	{0x2A,	4,	{0x00, 0x00, (FRAME_WIDTH>>8), (FRAME_WIDTH&0xFF)}},
	{0x2B,	4,	{0x00, 0x00, (FRAME_HEIGHT>>8), (FRAME_HEIGHT&0xFF)}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_sleep_out_setting[] = {
    //          
	{0x11, 1, {0x00}},
    {REGFLAG_DELAY, 100, {}},

    //           
	{0x29, 1, {0x00}},
	{REGFLAG_DELAY, 10, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_deep_sleep_mode_in_setting[] = {
	//                     
	{0x28, 1, {0x00}},
    {REGFLAG_DELAY, 10, {}},
    //              
	{0x10, 1, {0x00}},
    {REGFLAG_DELAY, 100, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_backlight_level_setting[] = {
	{0x51, 1, {0xFF}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_compare_id_setting[] = {
	//                     
	{0xB9,	3,	{0xFF, 0x83, 0x69}},
	{REGFLAG_DELAY, 10, {}},

        //              
        //                   

	{REGFLAG_END_OF_TABLE, 0x00, {}}
};



void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
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
				MDELAY(10);
       	}
    }
	
}


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

#if defined(LCM_DSI_CMD_MODE)
		params->dsi.mode   = CMD_MODE;
#else
		params->dsi.mode   = SYNC_PULSE_VDO_MODE;
#endif
	
		//    
		/*                      */
		params->dsi.LANE_NUM				= LCM_TWO_LANE;
		//                                                                
		params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
		params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
		params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
		params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

		//                                         
		//                      
		params->dsi.packet_size=256;

		//                     
		params->dsi.intermediat_buffer_num = 2;

		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;

		params->dsi.vertical_sync_active				= 3;
		params->dsi.vertical_backporch					= 12;
		params->dsi.vertical_frontporch					= 2;
		params->dsi.vertical_active_line				= FRAME_HEIGHT; 

		params->dsi.horizontal_sync_active				= 10;
		params->dsi.horizontal_backporch				= 50;
		params->dsi.horizontal_frontporch				= 50;
		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;

		//                     
		params->dsi.pll_div1=26;		//                                                             
		params->dsi.pll_div2=1; 		//                             

}


static void lcm_init(void)
{
    /*                    
                            
                           */
	
    SET_RESET_PIN(0);
    MDELAY(25);
    SET_RESET_PIN(1);
    MDELAY(100);
	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);

	/*                                                  
                     
               
                     
               
    
                                                                                                             
    
                                                                   
                              
                          
                                 
                                     
        
                      
                                                    
         
                                                      
           */
}


static void lcm_suspend(void)
{
	push_table(lcm_deep_sleep_mode_in_setting, sizeof(lcm_deep_sleep_mode_in_setting) / sizeof(struct LCM_setting_table), 1);
	//                 
	//          
	//                 
}


static void lcm_resume(void)
{
	//           
	
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

	dsi_set_cmdq(&data_array, 7, 0);

}

static void lcm_setbacklight(unsigned int level)
{
	//                                  
	lcm_backlight_level_setting[0].para_list[0] = level;

	push_table(lcm_backlight_level_setting, sizeof(lcm_backlight_level_setting) / sizeof(struct LCM_setting_table), 1);
}


static void lcm_setpwm(unsigned int divider)
{
	//    
}


static unsigned int lcm_getpwm(unsigned int divider)
{
	//                                                                
	//                                                                  
	unsigned int pwm_clk = 23706 / (1<<divider);	
	return pwm_clk;
}


//                                                                            
//                        
//                                                                            
static unsigned int lcm_compare_id()
    {
        unsigned int id = 0;
        unsigned char buffer[2];
        unsigned int array[16];

        SET_RESET_PIN(1);  //                             
        SET_RESET_PIN(0);
        MDELAY(10);
        SET_RESET_PIN(1);
        MDELAY(10);
    
        push_table(lcm_compare_id_setting, sizeof(lcm_compare_id_setting) / sizeof(struct LCM_setting_table), 1);
    
        array[0] = 0x00023700;//                                       
        dsi_set_cmdq(array, 1, 1);
        //                    
        read_reg_v2(0xF4, buffer, 2);
        id = buffer[0]; //               
        
        #ifdef BUILD_UBOOT
           printf("%s, id1 = 0x%08x\n", __func__, id); 
        #else
           printk("%s, id1 = 0x%08x\n", __func__, id);   
        #endif
		
        return (0x69 == id)?1:0;
    }


LCM_DRIVER hx8369b_wvga_dsi_vdo_drv = 
{
    .name			= "hx8369_dsi",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.set_backlight	= lcm_setbacklight,
	.set_pwm        = lcm_setpwm,
	.get_pwm        = lcm_getpwm,
	.compare_id     = lcm_compare_id,
#if defined(LCM_DSI_CMD_MODE)
    .update         = lcm_update,
#endif
    };

