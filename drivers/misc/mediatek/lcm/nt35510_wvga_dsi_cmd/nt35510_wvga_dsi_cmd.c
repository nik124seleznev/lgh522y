#include <linux/string.h>

#include "lcm_drv.h"


//                                                                            
//                 
//                                                                            

#define FRAME_WIDTH  (480)
#define FRAME_HEIGHT (800)

#define LCM_ID_NT35510 (5510)

//                                                                            
//                 
//                                                                            

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))

/*           */
#define LSDA_GPIO_PIN (GPIO_DISP_LSDA_PIN)

#define SET_GPIO_OUT(n, v)  (lcm_util.set_gpio_out((n), (v)))

#define SET_LSDA_LOW   SET_GPIO_OUT(LSDA_GPIO_PIN, 0)
#define SET_LSDA_HIGH  SET_GPIO_OUT(LSDA_GPIO_PIN, 1)

/*             */


#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))


//                                                                            
//                 
//                                                                            

#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)									lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)				lcm_util.dsi_write_regs(addr, pdata, byte_nums)
//                                                  
#define read_reg(cmd)											lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)   				lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)    


#define LCM_DSI_CMD_MODE

static void init_lcm_registers(void)
{
	unsigned int data_array[16];
	
		data_array[0]=0x00023902;
		data_array[1]=0x00000035;//                     
		dsi_set_cmdq(&data_array, 2, 1);
		MDELAY(10);
	
		data_array[0]=0x00063902;
		data_array[1]=0x52aa55f0;//                                         
		data_array[2]=0x00000108;
		dsi_set_cmdq(&data_array, 3, 1);
		MDELAY(10);

		data_array[0]=0x00353902;
		data_array[1]=0x000000d1;
		data_array[2]=0x00420016;
		data_array[3]=0x00740061;
		data_array[4]=0x00ad0097;
		data_array[5]=0x010001de;
		data_array[6]=0x01500126;
		data_array[7]=0x01b30187;
		data_array[8]=0x02d901b9;
		data_array[9]=0x02210205;
		data_array[10]=0x02640246;
		data_array[11]=0x03c20296;		
		data_array[12]=0x032e0304;
		data_array[13]=0x03eb0374;
		data_array[14]=0x000000ff;
		dsi_set_cmdq(&data_array,15, 1);
		MDELAY(20);
		
		data_array[0]=0x00353902;
		data_array[1]=0x000000d2;
		data_array[2]=0x00420016;
		data_array[3]=0x00740061;
		data_array[4]=0x00ad0097;
		data_array[5]=0x010001de;
		data_array[6]=0x01500126;
		data_array[7]=0x01b30187;
		data_array[8]=0x02d901b6;
		data_array[9]=0x02210205;
		data_array[10]=0x02640246;
		data_array[11]=0x03c20296;		
		data_array[12]=0x032e0304;
		data_array[13]=0x03eb0374;
		data_array[14]=0x000000ff;
		dsi_set_cmdq(&data_array,15, 1);
		MDELAY(20);

		data_array[0]=0x00353902;
		data_array[1]=0x000000d3;
		data_array[2]=0x00420016;
		data_array[3]=0x00740061;
		data_array[4]=0x00ad0097;
		data_array[5]=0x010001de;
		data_array[6]=0x01500126;
		data_array[7]=0x01b30187;
		data_array[8]=0x02d901b6;
		data_array[9]=0x02210205;
		data_array[10]=0x02640246;
		data_array[11]=0x03c20296;		
		data_array[12]=0x032e0304;
		data_array[13]=0x03eb0374;
		data_array[14]=0x000000ff;
		dsi_set_cmdq(&data_array,15, 1);
		MDELAY(20);

		data_array[0]=0x00353902;
		data_array[1]=0x000000d4;
		data_array[2]=0x00420016;
		data_array[3]=0x00740061;
		data_array[4]=0x00ad0097;
		data_array[5]=0x010001de;
		data_array[6]=0x01500126;
		data_array[7]=0x01b30187;
		data_array[8]=0x02d901b6;
		data_array[9]=0x02210205;
		data_array[10]=0x02640246;
		data_array[11]=0x03c20296;		
		data_array[12]=0x032e0304;
		data_array[13]=0x03eb0374;
		data_array[14]=0x000000ff;
		dsi_set_cmdq(&data_array,15, 1);
		MDELAY(20);

		data_array[0]=0x00353902;
		data_array[1]=0x000000d5;
		data_array[2]=0x00420016;
		data_array[3]=0x00740061;
		data_array[4]=0x00ad0097;
		data_array[5]=0x010001de;
		data_array[6]=0x01500126;
		data_array[7]=0x01b30187;
		data_array[8]=0x02d901b6;
		data_array[9]=0x02210205;
		data_array[10]=0x02640246;
		data_array[11]=0x03c20296;		
		data_array[12]=0x032e0304;
		data_array[13]=0x03eb0374;
		data_array[14]=0x000000ff;
		dsi_set_cmdq(&data_array,15, 1);
		MDELAY(20);


		data_array[0]=0x00353902;
		data_array[1]=0x000000d6;
		data_array[2]=0x00420016;
		data_array[3]=0x00740061;
		data_array[4]=0x00ad0097;
		data_array[5]=0x010001de;
		data_array[6]=0x01500126;
		data_array[7]=0x01b30187;
		data_array[8]=0x02d901b6;
		data_array[9]=0x02210205;
		data_array[10]=0x02640246;
		data_array[11]=0x03c20296;		
		data_array[12]=0x032e0304;
		data_array[13]=0x03eb0374;
		data_array[14]=0x000000ff;
		dsi_set_cmdq(&data_array,15, 1);
		MDELAY(20);
	
		data_array[0]=0x00043902;
		data_array[1]=0x242424b9;//                    
		dsi_set_cmdq(&data_array, 2, 1);
		MDELAY(10);
	
		data_array[0]=0x00043902;
		data_array[1]=0x242424ba;//                    
		dsi_set_cmdq(&data_array, 2, 1);
		MDELAY(10);
	
		data_array[0]=0x00043902;
		data_array[1]=0x018800bc;//                                     
		dsi_set_cmdq(&data_array, 2, 1);
		MDELAY(10);
	
		data_array[0]=0x00043902;
		data_array[1]=0x018800bd;//                                     
		dsi_set_cmdq(&data_array, 2, 1);
		MDELAY(10);
	
		data_array[0]=0x00033902;
		data_array[1]=0x007800be;//                                 
		dsi_set_cmdq(&data_array, 2, 1);
		MDELAY(10);
	
		data_array[0]=0x00063902;
		data_array[1]=0x52aa55f0;//                                         
		data_array[2]=0x00000008;
		dsi_set_cmdq(&data_array, 3, 1);
		MDELAY(10);
	
		data_array[0]=0x00033902;
		data_array[1]=0x0000efb1;//                                                          
		dsi_set_cmdq(&data_array,2, 1);
		MDELAY(10);
		
		data_array[0]=0x00043902;
		data_array[1]=0x050505bc;//                         
		dsi_set_cmdq(&data_array,2, 1);
		MDELAY(10);
	
		//                         
		//                               
		//            
	
		data_array[0]=0x00053902;
		data_array[1]=0x0100002a;
		data_array[2]=0x000000df;//                
		dsi_set_cmdq(&data_array,3, 1);
		MDELAY(200);
	
		data_array[0]=0x00053902;
		data_array[1]=0x0300002B;
		data_array[2]=0x00000055;//             
		dsi_set_cmdq(&data_array,3, 1);
		MDELAY(200);

	    //          
		data_array[0]=0x00023902;
		data_array[1]=0x0000ff51;
		dsi_set_cmdq(&data_array,2, 1);
		MDELAY(10);

		data_array[0]=0x00023902;
		data_array[1]=0x00002c53;
		dsi_set_cmdq(&data_array,2, 1);
		MDELAY(10);
		
		data_array[0]=0x00023902;
		data_array[1]=0x00000255;
		dsi_set_cmdq(&data_array,2, 1);
		MDELAY(10);
        //        

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
		params->dbi.te_mode				= LCM_DBI_TE_MODE_VSYNC_ONLY;
		//                                                   
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
		
		params->dsi.data_format.trans_seq	= LCM_DSI_TRANS_SEQ_MSB_FIRST;
		
		params->dsi.data_format.padding 	= LCM_DSI_PADDING_ON_LSB;
		
		params->dsi.data_format.format	  = LCM_DSI_FORMAT_RGB888;
		
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
		params->dsi.pll_div1=0x1a;		
		params->dsi.pll_div2=1; 	

		
}

static void lcm_init(void)
{

	SET_RESET_PIN(1);
	SET_RESET_PIN(0);
	MDELAY(25);
	
	SET_RESET_PIN(1);
	MDELAY(50);      

	init_lcm_registers();

}



static void lcm_suspend(void)
{
	unsigned int data_array[16];

	data_array[0]=0x00280500; //            
	dsi_set_cmdq(&data_array, 1, 1);
	MDELAY(20); 
	data_array[0] = 0x00100500; //         
	dsi_set_cmdq(&data_array, 1, 1);
	MDELAY(150);
}


static void lcm_resume(void)
{
	unsigned int data_array[16];

	data_array[0] = 0x00110500; //          
	dsi_set_cmdq(&data_array, 1, 1);
	MDELAY(150);
	data_array[0] = 0x00290500; //           
	dsi_set_cmdq(&data_array, 1, 1);
	MDELAY(20);
	      
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
	//                          

	dsi_set_cmdq(&data_array, 7, 0);

}


static unsigned int lcm_compare_id(void)
{
	unsigned int id=0;
	unsigned char buffer[2];
	unsigned int array[16];  

	//             
	SET_RESET_PIN(1);
	SET_RESET_PIN(0);
	MDELAY(25);
	
	SET_RESET_PIN(1);
	MDELAY(50);      

	array[0]=0x00063902;
	array[1]=0x52aa55f0;
	array[2]=0x00000108;
	dsi_set_cmdq(array, 3, 1);
	MDELAY(10);

	array[0] = 0x00083700;
	dsi_set_cmdq(array, 1, 1);

	//                                                                                
	read_reg_v2(0xc5, buffer,2);

	id = buffer[0]<<8 |buffer[1]; 
	      
#ifdef BUILD_UBOOT
	printf("%s, id = 0x%x \n", __func__, id);
#endif

    if(id == LCM_ID_NT35510)
    	return 1;
    else
    	return 0;

}


LCM_DRIVER nt35510_wvga_dsi_cmd_drv = 
{
    .name			= "nt35510_dsi",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id    = lcm_compare_id,
#if defined(LCM_DSI_CMD_MODE)
    .update         = lcm_update,
#endif
    };
