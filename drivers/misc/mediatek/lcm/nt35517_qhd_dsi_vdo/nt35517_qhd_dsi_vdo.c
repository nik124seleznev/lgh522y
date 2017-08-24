#ifdef BUILD_LK
#else
#include <linux/string.h>
#endif

#include "lcm_drv.h"
#ifdef BUILD_LK
	#include <platform/mt_gpio.h>
#elif defined(BUILD_UBOOT)
	#include <asm/arch/mt_gpio.h>
#else
	#include <mach/mt_gpio.h>
#endif

//                                                                            
//                 
//                                                                            
#define FRAME_WIDTH  										(540)
#define FRAME_HEIGHT 										(960)

#define REGFLAG_DELAY             							0XFE
#define REGFLAG_END_OF_TABLE      							0xFF   //                        

#define LCM_DSI_CMD_MODE									0
#define LCM_ID 												0x17

#define	GPIO_LCD_ID1_PIN	17
#define	GPIO_LCD_ID2_PIN	142
//                                                                            
//                 
//                                                                            
static LCM_UTIL_FUNCS lcm_util = {0};
#define SET_RESET_PIN(v)    								(lcm_util.set_reset_pin((v)))
#define UDELAY(n) 											(lcm_util.udelay(n))
#define MDELAY(n) 											(lcm_util.mdelay(n))

static unsigned int lcd_id_pin = 0;
static unsigned int first_inited = 0;

//                                                                            
//                 
//                                                                            
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg											lcm_util.dsi_read_reg()
#define read_reg_v2(cmd, buffer, buffer_size)   			lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)  


struct LCM_setting_table {
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[120];
};

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
	params->dsi.mode   = BURST_VDO_MODE; //                    
	//                                      
	
	//    
	/*                      */
	params->dsi.LANE_NUM				= LCM_TWO_LANE;
	//                                                                
	params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
	params->dsi.data_format.trans_seq	= LCM_DSI_TRANS_SEQ_MSB_FIRST;
	params->dsi.data_format.padding 	= LCM_DSI_PADDING_ON_LSB;
	params->dsi.data_format.format		= LCM_DSI_FORMAT_RGB888;
	
	//                                         
	//                      
	params->dsi.packet_size=256;
	
	//                     
	params->dsi.intermediat_buffer_num = 2;
	
	params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
	
	params->dsi.vertical_sync_active    = 10;// 
	params->dsi.vertical_backporch		= 40;//     
	params->dsi.vertical_frontporch 	= 40;//     
	params->dsi.vertical_active_line	= FRAME_HEIGHT; 
	
	params->dsi.horizontal_sync_active	= 10;// 
	params->dsi.horizontal_backporch	= 40;//  
	params->dsi.horizontal_frontporch	= 40;//  
	params->dsi.horizontal_active_pixel = FRAME_WIDTH;

	params->dsi.PLL_CLOCK = 260;//                                                                        
										//                                                                   
	params->dsi.pll_div1=0x1;		//                               
	params->dsi.pll_div2=0x1;		//                                
	params->dsi.fbk_div =0x10;	    //                                                                      
	params->dsi.fbk_sel=0x1;		//                                        
	params->dsi.rg_bir=0x5;
	params->dsi.rg_bic=0x2;
	params->dsi.rg_bp=0xC;
}

static void lcm_init(void)
{
	unsigned int data_array[16];    
	SET_RESET_PIN(1);
	MDELAY(20);
	SET_RESET_PIN(0);
	MDELAY(20);
	SET_RESET_PIN(1);
	MDELAY(50);

	data_array[0]=0x00053902;
	data_array[1]=0x2555aaff;
	data_array[2]=0x00000001;
	dsi_set_cmdq(&data_array,3,1);

	data_array[0]=0x00363902;
	data_array[1]=0x4A0000F2;
	data_array[2]=0x0000A80A;
	data_array[3]=0x00000000;
	data_array[4]=0x00000000;
	data_array[5]=0x000B0000;
	data_array[6]=0x00000000;
	data_array[7]=0x00000000;
	data_array[8]=0x51014000;
	data_array[9]=0x01000100;
	dsi_set_cmdq(&data_array,10,1);

	data_array[0]=0x00083902;
	data_array[1]=0x070302F3;
	data_array[2]=0x0DD18845;
	dsi_set_cmdq(&data_array,3,1);
	//      

	data_array[0]=0x00063902;
	data_array[1]=0x52AA55F0;
	data_array[2]=0x00000008;
	dsi_set_cmdq(&data_array,3,1);

	data_array[0]=0x00043902;
	data_array[1]=0x0000CCB1;	
	dsi_set_cmdq(&data_array,2,1);
/*
                          
                               

                          
                          
                               
*/
	data_array[0]=0x00053902;
	data_array[1]=0x020201B8;
	data_array[2]=0x00000002;
	dsi_set_cmdq(&data_array,3,1);
/*
                          
                               

                          
                          
                               

                          
                          
                          
                            
                            
                               
*/
	data_array[0]=0x00073902;
	data_array[1]=0x0D0653C9;
	data_array[2]=0x0000171a;
	dsi_set_cmdq(&data_array,3,1);

//                 


	
  data_array[0]=0x00063902;
	data_array[1]=0x52AA55F0;        //      
	data_array[2]=0x00000108;
	dsi_set_cmdq(&data_array,3,1);

	data_array[0]=0x00043902;
	data_array[1]=0x050505B0;		//          
	dsi_set_cmdq(&data_array,2,1);

	data_array[0]=0x00043902;
	data_array[1]=0x050505B1;		//            
	dsi_set_cmdq(&data_array,2,1);

	data_array[0]=0x00043902;
	data_array[1]=0x010101B2;		//           
	dsi_set_cmdq(&data_array,2,1);

	data_array[0]=0x00043902;
	data_array[1]=0x0d0d0dB3;		//         
	dsi_set_cmdq(&data_array,2,1);

	data_array[0]=0x00043902;
	data_array[1]=0x090909B4;		//             
	dsi_set_cmdq(&data_array,2,1);

	data_array[0]=0x00043902;
	data_array[1]=0x444444B6;		//           
	dsi_set_cmdq(&data_array,2,1);

	data_array[0]=0x00043902;
	data_array[1]=0x444444B7;		//            
	dsi_set_cmdq(&data_array,2,1);

	data_array[0]=0x00043902;
	data_array[1]=0x131313B8;		//           
	dsi_set_cmdq(&data_array,2,1);

	data_array[0]=0x00043902;		//                       
	data_array[1]=0x242424B9;
	dsi_set_cmdq(&data_array,2,1);

	data_array[0]=0x00043902;
	data_array[1]=0x242424BA;		//                         
	dsi_set_cmdq(&data_array,2,1);

	data_array[0]=0x00043902;
	data_array[1]=0x008000BC;	//                              
	dsi_set_cmdq(&data_array,2,1);

	data_array[0]=0x00043902;
	data_array[1]=0x008000BD;	//                         
	dsi_set_cmdq(&data_array,2,1);

	//                          
	//                              

	data_array[0]=0x43Be1500;	
	dsi_set_cmdq(&data_array,1,1);

	data_array[0]=0x00033902;
	data_array[1]=0x000004C0;	
	dsi_set_cmdq(&data_array,2,1);

	data_array[0]=0x00ca1500;	
	dsi_set_cmdq(&data_array,1,1);
		data_array[0]=0x04cf1500;	
	dsi_set_cmdq(&data_array,1,1);


	
	data_array[0]=0x00C21500;	
	dsi_set_cmdq(&data_array,1,1);
	/*              */
  
        data_array[0]=0x00053902;
        data_array[1]=0x0d1006D0;
        data_array[2]=0x0000000f;
        dsi_set_cmdq(&data_array,3,1);

        data_array[0]=0x00113902;
        data_array[1]=0x001600D1;
        data_array[2]=0x003e0023;
        data_array[3]=0x00720058;
        data_array[4]=0x00a90095;
        data_array[5]=0x000000e0;
        dsi_set_cmdq(&data_array,6,1);

        data_array[0]=0x00113902;
        data_array[1]=0x001600D5;
        data_array[2]=0x003e0023;
        data_array[3]=0x00720058;
        data_array[4]=0x00a90095;
        data_array[5]=0x000000e0;
        dsi_set_cmdq(&data_array,6,1);

                data_array[0]=0x00113902;
        data_array[1]=0x001600D9;
        data_array[2]=0x003e0023;
        data_array[3]=0x00720058;
        data_array[4]=0x00a90095;
        data_array[5]=0x000000e0;
        dsi_set_cmdq(&data_array,6,1);

        data_array[0]=0x00113902;
        data_array[1]=0x001600e0;
        data_array[2]=0x003e0023;
        data_array[3]=0x00720058;
        data_array[4]=0x00a90095;
        data_array[5]=0x000000e0;
        dsi_set_cmdq(&data_array,6,1);

        data_array[0]=0x00113902;
        data_array[1]=0x001600e4;
        data_array[2]=0x003e0023;
        data_array[3]=0x00720058;
        data_array[4]=0x00a90095;
        data_array[5]=0x000000e0;
        dsi_set_cmdq(&data_array,6,1);

        data_array[0]=0x00113902;
        data_array[1]=0x001600e8;
        data_array[2]=0x003e0023;
        data_array[3]=0x00720058;
        data_array[4]=0x00a90095;
        data_array[5]=0x000000e0;
        dsi_set_cmdq(&data_array,6,1);

        data_array[0]=0x00113902;
        data_array[1]=0x010601D2;
        data_array[2]=0x01710141;
        data_array[3]=0x01FA01bb;
        data_array[4]=0x023902fc;
        data_array[5]=0x0000007e;
        dsi_set_cmdq(&data_array,6,1);

        data_array[0]=0x00113902;
        data_array[1]=0x010601D6;
        data_array[2]=0x01710141;
        data_array[3]=0x01FA01bb;
        data_array[4]=0x023902fc;
        data_array[5]=0x0000007e;
        dsi_set_cmdq(&data_array,6,1);
        
        data_array[0]=0x00113902;
        data_array[1]=0x010601Dd;
        data_array[2]=0x01710141;
        data_array[3]=0x01FA01bb;
        data_array[4]=0x023902fc;
        data_array[5]=0x0000007e;
        dsi_set_cmdq(&data_array,6,1);

        data_array[0]=0x00113902;
        data_array[1]=0x010601e1;
        data_array[2]=0x01710141;
        data_array[3]=0x01FA01bb;
        data_array[4]=0x023902fc;
        data_array[5]=0x0000007e;
        dsi_set_cmdq(&data_array,6,1);

        data_array[0]=0x00113902;
        data_array[1]=0x010601e5;
        data_array[2]=0x01710141;
        data_array[3]=0x01FA01bb;
        data_array[4]=0x023902fc;
        data_array[5]=0x0000007e;
        dsi_set_cmdq(&data_array,6,1);

        data_array[0]=0x00113902;
        data_array[1]=0x010601e9;
        data_array[2]=0x01710141;
        data_array[3]=0x01FA01bb;
        data_array[4]=0x023902fc;
        data_array[5]=0x0000007e;
        dsi_set_cmdq(&data_array,6,1);

        
        data_array[0]=0x00113902;
        data_array[1]=0x02a402D3;
        data_array[2]=0x030403e0;
        data_array[3]=0x0363033d;
        data_array[4]=0x03ad038e;
        data_array[5]=0x000000c8;
        dsi_set_cmdq(&data_array,6,1);

        data_array[0]=0x00113902;
        data_array[1]=0x02a402D7;
        data_array[2]=0x030403e0;
        data_array[3]=0x0363033d;
        data_array[4]=0x03ad038e;
        data_array[5]=0x000000c8;
        dsi_set_cmdq(&data_array,6,1);

        data_array[0]=0x00113902;
        data_array[1]=0x02a402De;
        data_array[2]=0x030403e0;
        data_array[3]=0x0363033d;
        data_array[4]=0x03ad038e;
        data_array[5]=0x000000c8;
        dsi_set_cmdq(&data_array,6,1);

        data_array[0]=0x00113902;
        data_array[1]=0x02a402e2;
        data_array[2]=0x030403e0;
        data_array[3]=0x0363033d;
        data_array[4]=0x03ad038e;
        data_array[5]=0x000000c8;
        dsi_set_cmdq(&data_array,6,1);

        data_array[0]=0x00113902;
        data_array[1]=0x02a402e6;
        data_array[2]=0x030403e0;
        data_array[3]=0x0363033d;
        data_array[4]=0x03ad038e;
        data_array[5]=0x000000c8;
        dsi_set_cmdq(&data_array,6,1);

        data_array[0]=0x00113902;
        data_array[1]=0x02a402ea;
        data_array[2]=0x030403e0;
        data_array[3]=0x0363033d;
        data_array[4]=0x03ad038e;
        data_array[5]=0x000000c8;
        dsi_set_cmdq(&data_array,6,1);

        data_array[0]=0x00053902;
        data_array[1]=0x03f103D4;
        data_array[2]=0x000000ff;
        dsi_set_cmdq(&data_array,3,1);

        data_array[0]=0x00053902;
        data_array[1]=0x03f103D8;
        data_array[2]=0x000000ff;
        dsi_set_cmdq(&data_array,3,1);

        data_array[0]=0x00053902;
        data_array[1]=0x03f103df;
        data_array[2]=0x000000ff;
        dsi_set_cmdq(&data_array,3,1);

        data_array[0]=0x00053902;
        data_array[1]=0x03f103e3;
        data_array[2]=0x000000ff;
        dsi_set_cmdq(&data_array,3,1);

        data_array[0]=0x00053902;
        data_array[1]=0x03f103e7;
        data_array[2]=0x000000ff;
        dsi_set_cmdq(&data_array,3,1);

        data_array[0]=0x00053902;
        data_array[1]=0x03f103eb;
        data_array[2]=0x000000ff;
        dsi_set_cmdq(&data_array,3,1);

        
	/*                  */
	


	data_array[0]=0x773A1500;
	dsi_set_cmdq(&data_array,1,1);

    data_array[0]=0x00361500;
	dsi_set_cmdq(&data_array,1,1);

	data_array[0]=0x00351500;
	dsi_set_cmdq(&data_array,1,1);

	data_array[0]=0x00110500;
	dsi_set_cmdq(&data_array,1,1);
	MDELAY(20);

	data_array[0]=0x00291500;
	dsi_set_cmdq(&data_array,1,1);
	
	data_array[0]=0x00320500;
	dsi_set_cmdq(&data_array,1,1);		
	MDELAY(150);
}

static void lcm_suspend(void)
{
	unsigned int data_array[16];
	
	data_array[0]=0x00280500;
	dsi_set_cmdq(&data_array,1,1);
	MDELAY(20);
	data_array[0]=0x00100500;
	dsi_set_cmdq(&data_array,1,1);
	data_array[0]=0x00220500;
	dsi_set_cmdq(&data_array,1,1);	
	MDELAY(150);
}

static void lcm_resume(void)
{
	lcm_init();	
	
	/*
                             
                          
                               
             
                          
                               
            
 */
}

static unsigned int lcm_compare_id(void)
{
	#if 1
	unsigned int  data_array[16];
	unsigned char buffer_c5[3];
	unsigned char buffer_04[3];
	
	SET_RESET_PIN(1);
	MDELAY(20);
	SET_RESET_PIN(0);
	MDELAY(20);
	SET_RESET_PIN(1);
	MDELAY(50);

	data_array[0] = 0x00033700;
	dsi_set_cmdq(data_array, 1, 1);
	read_reg_v2(0x04, buffer_04, 3);

	data_array[0] = 0x00033700;
	dsi_set_cmdq(data_array, 1, 1);
	read_reg_v2(0x04, buffer_04, 3);
	
	data_array[0]=0x00063902;
	data_array[1]=0x52AA55F0;
	data_array[2]=0x00000108;
	dsi_set_cmdq(&data_array,3,1);
	
	data_array[0] = 0x00033700;
	dsi_set_cmdq(data_array, 1, 1);
	read_reg_v2(0xC5, buffer_c5, 3);

	if ((buffer_c5[0]==0x55)&&(buffer_c5[1]==0x17)){
		return 1;
	}else{
		return 1;
	}
#endif
#if 0
	unsigned int lcd_id1_pin = 0;
	unsigned int lcd_id2_pin = 0;
	if (!first_inited)
	{
		SET_RESET_PIN(1);
		SET_RESET_PIN(0);
		MDELAY(50);//           
		SET_RESET_PIN(1);
		MDELAY(300);//            

		mt_set_gpio_mode(GPIO_LCD_ID1_PIN, GPIO_MODE_GPIO);
		mt_set_gpio_dir(GPIO_LCD_ID1_PIN, GPIO_DIR_IN);
		mt_set_gpio_pull_enable(GPIO_LCD_ID1_PIN, GPIO_PULL_ENABLE);
		mt_set_gpio_pull_select(GPIO_LCD_ID1_PIN, GPIO_PULL_DOWN);
		mt_set_gpio_mode(GPIO_LCD_ID2_PIN, GPIO_MODE_GPIO);
		mt_set_gpio_dir(GPIO_LCD_ID2_PIN, GPIO_DIR_IN);
		mt_set_gpio_pull_enable(GPIO_LCD_ID2_PIN, GPIO_PULL_ENABLE);
		mt_set_gpio_pull_select(GPIO_LCD_ID2_PIN, GPIO_PULL_DOWN);
		MDELAY(50);
		
		lcd_id1_pin = mt_get_gpio_in(GPIO_LCD_ID1_PIN);
		lcd_id2_pin = mt_get_gpio_in(GPIO_LCD_ID2_PIN);

		lcd_id_pin = (lcd_id2_pin << 1) | lcd_id1_pin;
		first_inited = 1;
	}
	#ifndef BUILD_LK
	printk("<0>nt35517: lcd_id2_pin=%d,lcd_id1_pin=%d\n",lcd_id2_pin,lcd_id1_pin);
	printk("<0> 2222222222222222222222222222222222\n");
	#endif
	  if (lcd_id2_pin == 1) 
	    return 1;
    else 
	    return 1;
	    #endif
}

LCM_DRIVER nt35517_dsi_vdo_lcm_drv = 
{
	.name			= "nt35517_dsi_vedio",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id     = lcm_compare_id,	
};
