#include <linux/string.h>

#include "lcm_drv.h"


//                                                                            
//                 
//                                                                            

#define FRAME_WIDTH  (320)
#define FRAME_HEIGHT (480)
#define LCM_ID       (0x7601)
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

static __inline unsigned int to_18_bit_inst_format(unsigned int val)
{
    return ((val & 0xFF00) << 2) |
           ((val & 0x00FF) << 1);
}

static __inline unsigned int to_16_bit_inst_format(unsigned int val)
{
    return (((val >> 2) & 0xFF00) |
           ((val >> 1) & 0x00FF));
}

static __inline void send_ctrl_cmd(unsigned int cmd)
{
    lcm_util.send_cmd(to_18_bit_inst_format(cmd));
}

static __inline void send_data_cmd(unsigned int data)
{
    lcm_util.send_data(to_18_bit_inst_format(data));
}

static __inline unsigned int read_data_cmd(void)
{
    unsigned int data;
    data = to_16_bit_inst_format(lcm_util.read_data());
	return data;
}

static __inline void set_lcm_register(unsigned int regIndex,
                                      unsigned int regData)
{
    send_ctrl_cmd(regIndex);
    send_data_cmd(regData);
}


static void init_lcm_registers(void)
{
    //               
    //                     
    set_lcm_register(0x01, 0x003C);
    set_lcm_register(0x02, 0x0100);
    set_lcm_register(0x03, 0x1020); //     
 
    //                  
    set_lcm_register(0x67, 0x0200);
    set_lcm_register(0x04, 0x0000);   //                                   
    //                                                    
    set_lcm_register(0x05, 0x0002);   //                            
    set_lcm_register(0x48, 0x4b90);
    set_lcm_register(0x49, 0x95a0);
    set_lcm_register(0x4a, 0xa0ac);
    set_lcm_register(0x4b, 0xb5ce);
    //                  
     
    set_lcm_register(0x08, 0x0808);
    set_lcm_register(0x0A, 0x0700);   //           
    set_lcm_register(0x0B, 0x0000);
    set_lcm_register(0x0C, 0x0770);
    set_lcm_register(0x0D, 0x0000);
    //                                             
    set_lcm_register(0x0E, 0x003F);   //                  
 
    set_lcm_register(0x11, 0x0406);
    set_lcm_register(0x12, 0x000E);
    MDELAY(20);
 
    set_lcm_register(0x13, 0x0222);
    set_lcm_register(0x14, 0x0015);
    set_lcm_register(0x15, 0x4277);
    set_lcm_register(0x16, 0x0000);
     
    //      
    set_lcm_register(0x30, 0x5a50);   //          
    set_lcm_register(0x31, 0x00c8);
    set_lcm_register(0x32, 0xc7be);
    set_lcm_register(0x33, 0x0003);
    set_lcm_register(0x36, 0x3443);
    set_lcm_register(0x3B, 0x0000);
    set_lcm_register(0x3C, 0x0000);
 
    set_lcm_register(0x2C, 0x5a50);   //            
    set_lcm_register(0x2D, 0x00c8);
    set_lcm_register(0x2E, 0xc7be);
    set_lcm_register(0x2F, 0x0003);
    set_lcm_register(0x35, 0x3443);
    set_lcm_register(0x39, 0x0000);
    set_lcm_register(0x3A, 0x0000);
 
    set_lcm_register(0x28, 0x5a50);   //           
    set_lcm_register(0x29, 0x00c8);
    set_lcm_register(0x2A, 0xc7be);
    set_lcm_register(0x2B, 0x0003);
    set_lcm_register(0x34, 0x3443);
    set_lcm_register(0x37, 0x0000);
    set_lcm_register(0x38, 0x0000);
 
    set_lcm_register(0x12, 0x200E);
    MDELAY(20);
    set_lcm_register(0x12, 0x2003);
    MDELAY(20);
 
    set_lcm_register(0x44, 0x013f);
    set_lcm_register(0x45, 0x0000); 
    set_lcm_register(0x46, 0x01df);
    set_lcm_register(0x47, 0x0000); 
    set_lcm_register(0x20, 0x0000);
    set_lcm_register(0x21, 0x013f);   //     
    set_lcm_register(0x07, 0x0012);
    MDELAY(40);
    set_lcm_register(0x07, 0x0017);
    set_lcm_register(0x22, 0xFFFF);
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
    params->dbi.data_format.trans_seq   = LCM_DBI_TRANS_SEQ_MSB_FIRST;
    params->dbi.data_format.padding     = LCM_DBI_PADDING_ON_LSB;
    params->dbi.data_format.format      = LCM_DBI_FORMAT_RGB666;
    params->dbi.data_format.width       = LCM_DBI_DATA_WIDTH_18BITS;
    params->dbi.cpu_write_bits          = LCM_DBI_CPU_WRITE_32_BITS;
    params->dbi.io_driving_current      = 0;

    params->dbi.te_mode                 = LCM_DBI_TE_MODE_VSYNC_ONLY;
    params->dbi.te_edge_polarity        = LCM_POLARITY_RISING;

    params->dbi.parallel.write_setup    = 0;
    params->dbi.parallel.write_hold     = 2;
    params->dbi.parallel.write_wait     = 4;
    params->dbi.parallel.read_setup     = 0;
    params->dbi.parallel.read_latency   = 11;
    params->dbi.parallel.wait_period    = 0;
}


static void lcm_init(void)
{
    SET_RESET_PIN(0);
    MDELAY(100);
    SET_RESET_PIN(1);
    MDELAY(500);

    init_lcm_registers();
}


static void lcm_suspend(void)
{
    set_lcm_register(0x15, 0x0000);
    set_lcm_register(0x07, 0x0112);
    MDELAY(15);
    set_lcm_register(0x07, 0x0110);
    MDELAY(15);
    set_lcm_register(0x10, 0x0701);
}


static void lcm_resume(void)
{
#if 0
    set_lcm_register(0x10, 0x0700);
    MDELAY(15);
    set_lcm_register(0x11, 0x0010);
    set_lcm_register(0x14, 0x1f56);
    set_lcm_register(0x10, 0x0700);
    MDELAY(1);
    set_lcm_register(0x11, 0x0112);
    MDELAY(1);
    set_lcm_register(0x11, 0x0312);
    MDELAY(1);
    set_lcm_register(0x11, 0x0712);
    MDELAY(1);
    set_lcm_register(0x11, 0x0F1B);
    MDELAY(1);
    set_lcm_register(0x11, 0x0F3B);
    MDELAY(3);
    set_lcm_register(0x15, 0x0031);
    set_lcm_register(0x07, 0x1116);
    MDELAY(15);
    set_lcm_register(0x07, 0x1117);
#elif 1
    /*                                           
                                      
    */
    SET_RESET_PIN(0);
    MDELAY(100);
    SET_RESET_PIN(1);
    MDELAY(500);
    init_lcm_registers();
#endif
}


static void lcm_update(unsigned int x, unsigned int y,
                       unsigned int width, unsigned int height)
{
    unsigned int x0 = x;
    unsigned int y0 = y;
    unsigned int x1 = x0 + width - 1;
    unsigned int y1 = y0 + height - 1;

	set_lcm_register(0x44, x1);   //      
	set_lcm_register(0x45, x0);   //        
	set_lcm_register(0x46, y1);   //      
	set_lcm_register(0x47, y0);   //        

	set_lcm_register(0x20, y0);   //        
	set_lcm_register(0x21, x1);   //      

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

LCM_DRIVER ta7601_lcm_drv = 
{
	.name			= "ta7601",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.update         = lcm_update,
	.compare_id     = lcm_compare_id,
};
