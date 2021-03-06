#include <linux/string.h>

#include "lcm_drv.h"


//                                                                            
//                 
//                                                                            

#define FRAME_WIDTH  (240)
#define FRAME_HEIGHT (400)
#define LCM_ID       (0x52)
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

static __inline unsigned int LOW_BYTE(unsigned int x)
{
    return (x & 0xFF);
}

static __inline unsigned int HIGH_BYTE(unsigned int x)
{
    return ((x >> 8) & 0xFF);
}

static __inline void send_ctrl_cmd(unsigned int cmd)
{
    lcm_util.send_cmd(cmd);
}

static __inline void send_data_cmd(unsigned int data)
{
    lcm_util.send_data(data);
}

static __inline unsigned int read_data_cmd(void)
{
    return lcm_util.read_data();
}

static __inline void set_lcm_register(unsigned int regIndex,
                                      unsigned int regData)
{
    send_ctrl_cmd(regIndex);
    send_data_cmd(regData);
}


static void init_lcm_registers(void)
{
    set_lcm_register(0x83, 0x02);    //         
    set_lcm_register(0x85, 0x03);    //            
    set_lcm_register(0x8c, 0x93);
    set_lcm_register(0x91, 0x01);    //         
    set_lcm_register(0x83, 0x00);
    
    //              
    set_lcm_register(0x3E, 0xB0);
    set_lcm_register(0x3F, 0x03);
    set_lcm_register(0x40, 0x10);
    set_lcm_register(0x41, 0x56);
    set_lcm_register(0x42, 0x13);
    set_lcm_register(0x43, 0x46);
    set_lcm_register(0x44, 0x23);
    set_lcm_register(0x45, 0x76);
    set_lcm_register(0x46, 0x00);
    set_lcm_register(0x47, 0x5e);
    set_lcm_register(0x48, 0x4f);
    set_lcm_register(0x49, 0x40);
    
    //                     
    set_lcm_register(0x17, 0x91);    //                                       
    set_lcm_register(0x23, 0x01);    //                 
    set_lcm_register(0x2B, 0xF9);    //              
    MDELAY(10);
    
    set_lcm_register(0x1B, 0x16);    //                    
    set_lcm_register(0x1A, 0x11);    //                     
    set_lcm_register(0x1C, 0x0D);    //           
    set_lcm_register(0x1F, 0x42);    //               
    MDELAY(20);
    
    set_lcm_register(0x19, 0x0A);    //                                           
    set_lcm_register(0x19, 0x1A);    //                                           
    MDELAY(40);
    set_lcm_register(0x19, 0x12);    //                                           
    MDELAY(40);
    set_lcm_register(0x1E, 0x2C);    //                   
    MDELAY(100);
    set_lcm_register(0x3C, 0x60);
    set_lcm_register(0x3D, 0x40);
    set_lcm_register(0x34, 0x38);
    set_lcm_register(0x35, 0x38); 
    set_lcm_register(0x24, 0x38);
    MDELAY(40);
    set_lcm_register(0x24, 0x3C);
    set_lcm_register(0x16, 0x08);    //       
    set_lcm_register(0x01, 0x02);
    set_lcm_register(0x55, 0x00);
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
    
    params->dbi.port                    = 0;
    params->dbi.clock_freq              = LCM_DBI_CLOCK_FREQ_52M;
    params->dbi.data_width              = LCM_DBI_DATA_WIDTH_18BITS;
    params->dbi.data_format.color_order = LCM_COLOR_ORDER_RGB;
    params->dbi.data_format.trans_seq   = LCM_DBI_TRANS_SEQ_LSB_FIRST;
    params->dbi.data_format.padding     = LCM_DBI_PADDING_ON_MSB;
    params->dbi.data_format.format      = LCM_DBI_FORMAT_RGB666;
    params->dbi.data_format.width       = LCM_DBI_DATA_WIDTH_18BITS;
    params->dbi.cpu_write_bits          = LCM_DBI_CPU_WRITE_32_BITS;
    params->dbi.io_driving_current      = 0;

    params->dbi.te_mode                 = LCM_DBI_TE_MODE_VSYNC_OR_HSYNC;
    params->dbi.te_edge_polarity        = LCM_POLARITY_RISING;
    params->dbi.te_hs_delay_cnt         = 50;
    params->dbi.te_vs_width_cnt         = 277;
    params->dbi.te_vs_width_cnt_div     = LCM_DBI_TE_VS_WIDTH_CNT_DIV_16;

    params->dbi.parallel.write_setup    = 2;
    params->dbi.parallel.write_hold     = 2;
    params->dbi.parallel.write_wait     = 4;
    params->dbi.parallel.read_setup     = 2;
    params->dbi.parallel.read_latency   = 9;
    params->dbi.parallel.wait_period    = 0;
}


static void lcm_init(void)
{
    SET_RESET_PIN(0);
    MDELAY(40);
    SET_RESET_PIN(1);
    MDELAY(250);

    init_lcm_registers();

    //                              
    //
    set_lcm_register(0x23, 0x03);   //                  
}


static void lcm_suspend(void)
{
    set_lcm_register(0x24, 0x38); //                         
    MDELAY(40);
    set_lcm_register(0x24, 0x28); //                         
    MDELAY(40);
    set_lcm_register(0x24, 0x20); //                         
    set_lcm_register(0x3C, 0x00); //                
    set_lcm_register(0x1B, 0x10); //                
    set_lcm_register(0x19, 0x0A); //            
    set_lcm_register(0x1E, 0x00); //         
    set_lcm_register(0x19, 0x01); //      
    set_lcm_register(0x17, 0x00); //         
}


static void lcm_resume(void)
{
    set_lcm_register(0x17, 0x91); //                                   
    set_lcm_register(0x2B, 0xF9); //             
    MDELAY(10);
    set_lcm_register(0x1B, 0x14); //                
    set_lcm_register(0x1A, 0x11); //                 
    set_lcm_register(0x1C, 0x0D); //         
    set_lcm_register(0x1F, 0x42); //             
    MDELAY(20);
    set_lcm_register(0x19, 0x0A); //                                               
    set_lcm_register(0x19, 0x1A); //                                               
    MDELAY(40);
    set_lcm_register(0x19, 0x12); //                                               
    MDELAY(40);
    set_lcm_register(0x1E, 0x2C); //                    
    MDELAY(100);

    //                   
    set_lcm_register(0x3C, 0x60); //                
    set_lcm_register(0x3D, 0x40); //                 
    set_lcm_register(0x34, 0x38); //              
    set_lcm_register(0x35, 0x38); //              
    set_lcm_register(0x24, 0x38); //                         
    MDELAY(40);
    set_lcm_register(0x24, 0x3C); //                         
    set_lcm_register(0x16, 0x08); //                                             
    set_lcm_register(0x01, 0x02); //                                
    set_lcm_register(0x55, 0x00); //                                                         

    //                              
    //
    set_lcm_register(0x23, 0x03);   //                  
}


static void lcm_update(unsigned int x, unsigned int y,
                       unsigned int width, unsigned int height)
{
    unsigned int x0 = x;
    unsigned int y0 = y;
    unsigned int x1 = x0 + width - 1;
    unsigned int y1 = y0 + height - 1;

	set_lcm_register(0x02, HIGH_BYTE(x0));
	set_lcm_register(0x03, LOW_BYTE(x0));
	set_lcm_register(0x04, HIGH_BYTE(x1));
	set_lcm_register(0x05, LOW_BYTE(x1));
	set_lcm_register(0x06, HIGH_BYTE(y0));
	set_lcm_register(0x07, LOW_BYTE(y0));
	set_lcm_register(0x08, HIGH_BYTE(y1));
	set_lcm_register(0x09, LOW_BYTE(y1));

    send_ctrl_cmd(0x22);
}

static unsigned int lcm_compare_id(void)
{
	send_ctrl_cmd(0x00);
    return (LCM_ID == read_data_cmd())?1:0;
}


//                                                                            
//                      
//                                                                            

LCM_DRIVER bm8578_lcm_drv = 
{
    .name			= "bm8578",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.update         = lcm_update,
	.compare_id     = lcm_compare_id
};
