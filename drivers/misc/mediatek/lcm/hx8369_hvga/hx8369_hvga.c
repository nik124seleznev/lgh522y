#include <linux/string.h>

#include "lcm_drv.h"


//                                                                            
//                 
//                                                                            

#define FRAME_WIDTH  (320)
#define FRAME_HEIGHT (480)
#define LCM_ID       (0x69)

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

static __inline void send_ctrl_cmd(unsigned int cmd)
{
	lcm_util.send_cmd(cmd);
}

static __inline void send_data_cmd(unsigned int data)
{
	lcm_util.send_data(data&0xff);
}

static __inline unsigned int read_data_cmd(void)
{
    return 0xFF&lcm_util.read_data();
}

static __inline void set_lcm_register(unsigned int regIndex,
		unsigned int regData)
{
	send_ctrl_cmd(regIndex);
	send_data_cmd(regData);
}

static void sw_clear_panel(unsigned int color)
{
	unsigned short x0, y0, x1, y1, x, y;
	unsigned short h_X_start,l_X_start,h_X_end,l_X_end,h_Y_start,l_Y_start,h_Y_end,l_Y_end;

	x0 = (unsigned short)0;
	y0 = (unsigned short)0;
	x1 = (unsigned short)FRAME_WIDTH-1;
	y1 = (unsigned short)FRAME_HEIGHT-1;

	h_X_start=((x0&0xFF00)>>8);
	l_X_start=(x0&0x00FF);
	h_X_end=((x1&0xFF00)>>8);
	l_X_end=(x1&0x00FF);

	h_Y_start=((y0&0xFF00)>>8);
	l_Y_start=(y0&0x00FF);
	h_Y_end=((y1&0xFF00)>>8);
	l_Y_end=(y1&0x00FF);

	send_ctrl_cmd(0x2A);
	send_data_cmd(h_X_start); 
	send_data_cmd(l_X_start); 
	send_data_cmd(h_X_end); 
	send_data_cmd(l_X_end); 

	send_ctrl_cmd(0x2B);
	send_data_cmd(h_Y_start); 
	send_data_cmd(l_Y_start); 
	send_data_cmd(h_Y_end); 
	send_data_cmd(l_Y_end); 

	send_ctrl_cmd(0x29); 

	send_ctrl_cmd(0x2C);
    for (y = y0; y <= y1; ++ y) {
        for (x = x0; x <= x1; ++ x) {
            lcm_util.send_data(color);
        }
    }
}

static void init_lcm_registers(void)
{
	send_ctrl_cmd(0xB9);  //             
	send_data_cmd(0xFF);  
	send_data_cmd(0x83);  
	send_data_cmd(0x69);

	send_ctrl_cmd(0xB1);  //         
	send_data_cmd(0x85);
	send_data_cmd(0x00);
	send_data_cmd(0x34);
	send_data_cmd(0x0A);
	send_data_cmd(0x00);
	send_data_cmd(0x0F);
	send_data_cmd(0x0F);
	send_data_cmd(0x2A);
	send_data_cmd(0x32);
	send_data_cmd(0x3F);
	send_data_cmd(0x3F);
	send_data_cmd(0x01);
	send_data_cmd(0x23);
	send_data_cmd(0x01);
	send_data_cmd(0xE6);
	send_data_cmd(0xE6);
	send_data_cmd(0xE6);
	send_data_cmd(0xE6);
	send_data_cmd(0xE6);

	send_ctrl_cmd(0xB2);  //                     
	send_data_cmd(0x00);  
	send_data_cmd(0x20);  
	send_data_cmd(0x03);  
	send_data_cmd(0x03);  
	send_data_cmd(0x70);  
	send_data_cmd(0x00);  
	send_data_cmd(0xFF);  
	send_data_cmd(0x06);  
	send_data_cmd(0x00);  
	send_data_cmd(0x00);  
	send_data_cmd(0x00);  
	send_data_cmd(0x03);  
	send_data_cmd(0x03);  
	send_data_cmd(0x00);  
	send_data_cmd(0x01);  

	send_ctrl_cmd(0xB4);  //                              
	send_data_cmd(0x00);  
	send_data_cmd(0x18);  
	send_data_cmd(0x80);  
	send_data_cmd(0x06);  
	send_data_cmd(0x02);  

	send_ctrl_cmd(0xB6);  //         
	send_data_cmd(0x3A);  
	send_data_cmd(0x3A);  

	send_ctrl_cmd(0xD5);
	send_data_cmd(0x00);
	send_data_cmd(0x02);
	send_data_cmd(0x03);
	send_data_cmd(0x00);
	send_data_cmd(0x01);
	send_data_cmd(0x03);
	send_data_cmd(0x28);
	send_data_cmd(0x70);
	send_data_cmd(0x01);
	send_data_cmd(0x03);
	send_data_cmd(0x00);
	send_data_cmd(0x00);
	send_data_cmd(0x40);
	send_data_cmd(0x06);
	send_data_cmd(0x51);
	send_data_cmd(0x07);
	send_data_cmd(0x00);
	send_data_cmd(0x00);
	send_data_cmd(0x41);
	send_data_cmd(0x06);
	send_data_cmd(0x50);
	send_data_cmd(0x07);
	send_data_cmd(0x07);
	send_data_cmd(0x0F);
	send_data_cmd(0x04);
	send_data_cmd(0x00);

	send_ctrl_cmd(0xE0); //          
	send_data_cmd(0x00);  
	send_data_cmd(0x13);  
	send_data_cmd(0x19);  
	send_data_cmd(0x38);  
	send_data_cmd(0x3D);  
	send_data_cmd(0x3F);  
	send_data_cmd(0x28);  
	send_data_cmd(0x46);  
	send_data_cmd(0x07);  
	send_data_cmd(0x0D);  
	send_data_cmd(0x0E);  
	send_data_cmd(0x12);  
	send_data_cmd(0x15);  
	send_data_cmd(0x12);  
	send_data_cmd(0x14);  
	send_data_cmd(0x0F);  
	send_data_cmd(0x17);  
	send_data_cmd(0x00);  
	send_data_cmd(0x13);  
	send_data_cmd(0x19);  
	send_data_cmd(0x38);  
	send_data_cmd(0x3D);  
	send_data_cmd(0x3F);  
	send_data_cmd(0x28);  
	send_data_cmd(0x46);  
	send_data_cmd(0x07);  
	send_data_cmd(0x0D);  
	send_data_cmd(0x0E);  
	send_data_cmd(0x12);  
	send_data_cmd(0x15);  
	send_data_cmd(0x12);  
	send_data_cmd(0x14);  
	send_data_cmd(0x0F);  
	send_data_cmd(0x17);  

	send_ctrl_cmd(0xC1); //        
	send_data_cmd(0x01);  
	send_data_cmd(0x04);  
	send_data_cmd(0x13);  
	send_data_cmd(0x1A);  
	send_data_cmd(0x20);  
	send_data_cmd(0x27);  
	send_data_cmd(0x2C);  
	send_data_cmd(0x32);  
	send_data_cmd(0x36);  
	send_data_cmd(0x3F);  
	send_data_cmd(0x47);  
	send_data_cmd(0x50);  
	send_data_cmd(0x59);  
	send_data_cmd(0x60);  
	send_data_cmd(0x68);  
	send_data_cmd(0x71);  
	send_data_cmd(0x7B);  
	send_data_cmd(0x82);  
	send_data_cmd(0x89);  
	send_data_cmd(0x91);  
	send_data_cmd(0x98);  
	send_data_cmd(0xA0);  
	send_data_cmd(0xA8);  
	send_data_cmd(0xB0);  
	send_data_cmd(0xB8);  
	send_data_cmd(0xC1);  
	send_data_cmd(0xC9);  
	send_data_cmd(0xD0);  
	send_data_cmd(0xD7);  
	send_data_cmd(0xE0);  
	send_data_cmd(0xE7);  
	send_data_cmd(0xEF);  
	send_data_cmd(0xF7);  
	send_data_cmd(0xFE);  
	send_data_cmd(0xCF);  
	send_data_cmd(0x52);  
	send_data_cmd(0x34);  
	send_data_cmd(0xF8);  
	send_data_cmd(0x51);  
	send_data_cmd(0xF5);  
	send_data_cmd(0x9D);  
	send_data_cmd(0x75);  
	send_data_cmd(0x00);  
	send_data_cmd(0x04);  
	send_data_cmd(0x13);  
	send_data_cmd(0x1A);  
	send_data_cmd(0x20);  
	send_data_cmd(0x27);  
	send_data_cmd(0x2C);  
	send_data_cmd(0x32);  
	send_data_cmd(0x36);  
	send_data_cmd(0x3F);  
	send_data_cmd(0x47);  
	send_data_cmd(0x50);  
	send_data_cmd(0x59);  
	send_data_cmd(0x60);  
	send_data_cmd(0x68);  
	send_data_cmd(0x71);  
	send_data_cmd(0x7B);  
	send_data_cmd(0x82);  
	send_data_cmd(0x89);  
	send_data_cmd(0x91);  
	send_data_cmd(0x98);  
	send_data_cmd(0xA0);  
	send_data_cmd(0xA8);  
	send_data_cmd(0xB0);  
	send_data_cmd(0xB8);  
	send_data_cmd(0xC1); 
	send_data_cmd(0xC9);  
	send_data_cmd(0xD0);  
	send_data_cmd(0xD7);  
	send_data_cmd(0xE0);  
	send_data_cmd(0xE7);  
	send_data_cmd(0xEF);  
	send_data_cmd(0xF7);  
	send_data_cmd(0xFE);  
	send_data_cmd(0xCF);  
	send_data_cmd(0x52);  
	send_data_cmd(0x34);  
	send_data_cmd(0xF8);  
	send_data_cmd(0x51);  
	send_data_cmd(0xF5);  
	send_data_cmd(0x9D);  
	send_data_cmd(0x75);  
	send_data_cmd(0x00);  
	send_data_cmd(0x04);  
	send_data_cmd(0x13);  
	send_data_cmd(0x1A);  
	send_data_cmd(0x20);  
	send_data_cmd(0x27);  
	send_data_cmd(0x2C);  
	send_data_cmd(0x32);  
	send_data_cmd(0x36);  
	send_data_cmd(0x3F);  
	send_data_cmd(0x47);  
	send_data_cmd(0x50);  
	send_data_cmd(0x59);  
	send_data_cmd(0x60);  
	send_data_cmd(0x68);  
	send_data_cmd(0x71);  
	send_data_cmd(0x7B);  
	send_data_cmd(0x82); 
	send_data_cmd(0x89);  
	send_data_cmd(0x91);  
	send_data_cmd(0x98);  
	send_data_cmd(0xA0);  
	send_data_cmd(0xA8);  
	send_data_cmd(0xB0);  
	send_data_cmd(0xB8);  
	send_data_cmd(0xC1);  
	send_data_cmd(0xC9);  
	send_data_cmd(0xD0);  
	send_data_cmd(0xD7);  
	send_data_cmd(0xE0);  
	send_data_cmd(0xE7);  
	send_data_cmd(0xEF);  
	send_data_cmd(0xF7);  
	send_data_cmd(0xFE);  
	send_data_cmd(0xCF);  
	send_data_cmd(0x52);  
	send_data_cmd(0x34);  
	send_data_cmd(0xF8);  
	send_data_cmd(0x51);  
	send_data_cmd(0xF5);  
	send_data_cmd(0x9D);  
	send_data_cmd(0x75);  
	send_data_cmd(0x00);

	send_ctrl_cmd(0x3A);  //                           
	send_data_cmd(0x07);   //                                                        

	send_ctrl_cmd(0x51);//                        
	send_data_cmd(0x00);//                        
	MDELAY(50);

	send_ctrl_cmd(0x53);//                  
	send_data_cmd(0x24);
	MDELAY(50);

	send_ctrl_cmd(0x55);
	send_data_cmd(0x01);//             
	MDELAY(50);

	send_ctrl_cmd(0x5e);//                             
	send_data_cmd(0x70);//
	MDELAY(50);

	send_ctrl_cmd(0xCA);  //            
    send_data_cmd(0x24); 
    send_data_cmd(0x24); 
    send_data_cmd(0x24); 
    send_data_cmd(0x23); 
    send_data_cmd(0x23); 
    send_data_cmd(0x23); 
    send_data_cmd(0x22); 
    send_data_cmd(0x22); 
    send_data_cmd(0x22); 
    send_data_cmd(0x22); 

	send_ctrl_cmd(0x11); 	
	MDELAY(120);
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

	params->type   = LCM_TYPE_DBI;
	params->ctrl   = LCM_CTRL_PARALLEL_DBI;
	params->width  = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;
	params->io_select_mode = 3;

	params->dbi.port                    = 0;
	params->dbi.clock_freq              = LCM_DBI_CLOCK_FREQ_104M;
	params->dbi.data_width              = LCM_DBI_DATA_WIDTH_24BITS;
	params->dbi.data_format.color_order = LCM_COLOR_ORDER_RGB;
	params->dbi.data_format.trans_seq   = LCM_DBI_TRANS_SEQ_MSB_FIRST;
	params->dbi.data_format.padding     = LCM_DBI_PADDING_ON_MSB;
	params->dbi.data_format.format      = LCM_DBI_FORMAT_RGB888;
	params->dbi.data_format.width       = LCM_DBI_DATA_WIDTH_24BITS;
	params->dbi.cpu_write_bits          = LCM_DBI_CPU_WRITE_32_BITS;
	params->dbi.io_driving_current      = LCM_DRIVING_CURRENT_8MA;

	params->dbi.parallel.write_setup    = 1;
	params->dbi.parallel.write_hold     = 1;
	params->dbi.parallel.write_wait     = 3;
	params->dbi.parallel.read_setup     = 3;
	params->dbi.parallel.read_latency   = 20;
	params->dbi.parallel.wait_period    = 0;
	
/*                       
                                                                     
                                                               */
}

static unsigned int lcm_check_status(void)
{
	send_ctrl_cmd(0xB9);
	if(read_data_cmd() != 0x00)	return 0;
	if(read_data_cmd() != 0xff)	return 0;
	if(read_data_cmd() != 0x83)	return 0;
	if(read_data_cmd() != 0x69)	return 0;

	return 1;
}


static void lcm_init(void)
{
	SET_RESET_PIN(0);
	MDELAY(25);
	SET_RESET_PIN(1);
	MDELAY(50);

	init_lcm_registers();

    //               
	send_ctrl_cmd(0x35);
	send_data_cmd(0x00);

    send_ctrl_cmd(0X0044);  //                             
    send_data_cmd(0X0000);  //                     
    send_data_cmd(0X0000);
	//                  
}


static void lcm_suspend(void)
{
	send_ctrl_cmd(0x10);
	MDELAY(120);
}


static void lcm_resume(void)
{
	send_ctrl_cmd(0x11);
	MDELAY(120);
}

static void lcm_update(unsigned int x, unsigned int y,
		unsigned int width, unsigned int height)
{
	unsigned short x0, y0, x1, y1;
	unsigned short h_X_start,l_X_start,h_X_end,l_X_end,h_Y_start,l_Y_start,h_Y_end,l_Y_end;

	x0 = (unsigned short)x;
	y0 = (unsigned short)y;
	x1 = (unsigned short)x+width-1;
	y1 = (unsigned short)y+height-1;

	h_X_start=((x0&0xFF00)>>8);
	l_X_start=(x0&0x00FF);
	h_X_end=((x1&0xFF00)>>8);
	l_X_end=(x1&0x00FF);

	h_Y_start=((y0&0xFF00)>>8);
	l_Y_start=(y0&0x00FF);
	h_Y_end=((y1&0xFF00)>>8);
	l_Y_end=(y1&0x00FF);

	send_ctrl_cmd(0x2A);
	send_data_cmd(h_X_start); 
	send_data_cmd(l_X_start); 
	send_data_cmd(h_X_end); 
	send_data_cmd(l_X_end); 

	send_ctrl_cmd(0x2B);
	send_data_cmd(h_Y_start); 
	send_data_cmd(l_Y_start); 
	send_data_cmd(h_Y_end); 
	send_data_cmd(l_Y_end); 

	send_ctrl_cmd(0x29); 

	send_ctrl_cmd(0x2C);
}

//                                          
//                                        
//                                                                 
static void lcm_setbacklight(unsigned int level)
{
	unsigned int default_level = 52;
	unsigned int mapped_level = 0;
	if(level > 255) 
		mapped_level = 255;

	if(level >= 102) 
		mapped_level = default_level+(level-102)*(255-default_level)/(255-102);
	else
		mapped_level = default_level-(102-level)*default_level/102;

	send_ctrl_cmd(0x51);
	send_data_cmd(mapped_level);	
}

static void lcm_setpwm(unsigned int divider)
{
    send_ctrl_cmd(0xC9);

    send_data_cmd(0x3E);
	send_data_cmd(0x00);
    send_data_cmd(0x00);
	send_data_cmd(0x01);
	send_data_cmd(0x0F | (divider<<4));   
//                      
	send_data_cmd(0x02);
	send_data_cmd(0x1E);   
	send_data_cmd(0x1E);
	send_data_cmd(0x00);
}

static unsigned int lcm_getpwm(unsigned int divider)
{
//                                                                
//                                                                   
	unsigned int pwm_clk = 23706 / (1<<divider);	
	return pwm_clk;
}

static unsigned int lcm_compare_id(void)
{	
    send_ctrl_cmd(0xB9);  //             
	send_data_cmd(0xFF);  
	send_data_cmd(0x83);  
	send_data_cmd(0x69);
    send_ctrl_cmd(0xC3);
	send_data_cmd(0xFF);

	send_ctrl_cmd(0xF4);
	read_data_cmd();
    return (LCM_ID == read_data_cmd())?1:0;
}

LCM_DRIVER hx8369_hvga_lcm_drv = 
{
    .name			= "hx8369_hvga",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.update         = lcm_update,
	.set_backlight	= lcm_setbacklight,
	.set_pwm        = lcm_setpwm,
	.get_pwm        = lcm_getpwm
	.compare_id     = lcm_compare_id,
	.check_status	= lcm_check_status
};
