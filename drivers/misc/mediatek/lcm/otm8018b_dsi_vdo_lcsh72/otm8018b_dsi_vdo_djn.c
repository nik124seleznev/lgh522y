#ifndef BUILD_LK
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

#define FRAME_WIDTH  										(480)
#define FRAME_HEIGHT 										(800)

#define REGFLAG_DELAY             							0XFD
#define REGFLAG_END_OF_TABLE      							0xFE   //                        

#define LCM_ID (0x018B)

#ifndef TRUE
    #define   TRUE     1
#endif

#ifndef FALSE
    #define   FALSE    0
#endif

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
#define read_reg(cmd)										lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)   			lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)       

static struct LCM_setting_table {
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[64];
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

    params->type = LCM_TYPE_DSI;

    params->width = FRAME_WIDTH;
    params->height = FRAME_HEIGHT;

    //                    
    params->dbi.te_mode = LCM_DBI_TE_MODE_VSYNC_ONLY;
    params->dbi.te_edge_polarity = LCM_POLARITY_RISING;

    params->dsi.mode = BURST_VDO_MODE;
	
    //    
    /*                      */
    params->dsi.LANE_NUM = LCM_TWO_LANE;
    //                                                                
    params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
    params->dsi.data_format.trans_seq = LCM_DSI_TRANS_SEQ_MSB_FIRST;
    params->dsi.data_format.padding = LCM_DSI_PADDING_ON_LSB;
    params->dsi.data_format.format = LCM_DSI_FORMAT_RGB888;

    //                                         
    //                      
    params->dsi.packet_size = 256;

    params->dsi.PS = LCM_PACKED_PS_24BIT_RGB888;
    params->dsi.word_count = 480 * 3;	

    params->dsi.vertical_sync_active = 84;//   
    params->dsi.vertical_backporch = 28;//   
    params->dsi.vertical_frontporch = 28;//   
    params->dsi.vertical_active_line = FRAME_HEIGHT; 

    //                                    
    params->dsi.horizontal_sync_active = 6;//    
    params->dsi.horizontal_backporch = 33;//    
    params->dsi.horizontal_frontporch = 33;//    
    params->dsi.horizontal_active_pixel = FRAME_WIDTH;
    params->dsi.horizontal_blanking_pixel = 60;

    params->dsi.PLL_CLOCK = 221; //                                                           
}

static unsigned int lcm_compare_id(void)
{
    unsigned char buffer[5];
    unsigned int data_array[16];

    SET_RESET_PIN(1);  //                             
    SET_RESET_PIN(0);
    MDELAY(10);
    SET_RESET_PIN(1);
    MDELAY(50);	

    data_array[0] = 0x00053700;//                                       
    dsi_set_cmdq(data_array, 1, 1);

    read_reg_v2(0xA1, buffer, 5);

    return (LCM_ID == ((buffer[0] << 8) | buffer[1]))? 1: 0;
}

static void init_lcm_registers(void) //       
{
    unsigned int data_arry[16];

    //                      
    data_arry[0] = 0x00001500;
    dsi_set_cmdq(&data_arry,1,1);

    //                                
    data_arry[0] = 0x00043902;
    data_arry[1] = 0x010980ff;
    dsi_set_cmdq(&data_arry,2,1);

    //                      
    data_arry[0] = 0x80001500;
    dsi_set_cmdq(&data_arry,1,1);

    //                           
    data_arry[0] = 0x00033902;
    data_arry[1] = 0x000980ff;
    dsi_set_cmdq(&data_arry,2,1);

    //                      
    //                          
    //                             

    //                      
    //                          
    //                             

    //                      
    data_arry[0] = 0x80001500;
    dsi_set_cmdq(&data_arry,1,1);

    //                                
    //                    
    //                    
    //      
    data_arry[0] = 0x000D3902;
    data_arry[1] = 0x021801F5;
    data_arry[2] = 0x02181018;
    data_arry[3] = 0x0F180E18;
    data_arry[4] = 0x00000020;
    dsi_set_cmdq(&data_arry,5,1);

    //                      
    data_arry[0] = 0x90001500;
    dsi_set_cmdq(&data_arry,1,1);

    //                                
    //                    
    //                
    data_arry[0] = 0x000B3902;
    data_arry[1] = 0x081802F5;
    data_arry[2] = 0x0D180618;
    data_arry[3] = 0x00180B18;
    dsi_set_cmdq(&data_arry,4,1);

    //                      
    data_arry[0] = 0xA0001500;
    dsi_set_cmdq(&data_arry,1,1);

    //                               
    //                    
    //      
    data_arry[0] = 0x00093902;
    data_arry[1] = 0x011810F5;
    data_arry[2] = 0x14181418;
    data_arry[3] = 0x00000018;
    dsi_set_cmdq(&data_arry,4,1);

    //                      
    data_arry[0] = 0xB0001500;
    dsi_set_cmdq(&data_arry,1,1);

    //                                
    //                    
    //                    
    //      
    data_arry[0] = 0x000D3902;
    data_arry[1] = 0x121814F5;
    data_arry[2] = 0x11181318;
    data_arry[3] = 0x00181318;
    data_arry[4] = 0x00000000;
    dsi_set_cmdq(&data_arry,5,1);

    //                      
    data_arry[0] = 0x8B001500;
    dsi_set_cmdq(&data_arry,1,1);

    //                      
    data_arry[0] = 0x40B01500;    //           
    dsi_set_cmdq(&data_arry,1,1);

    //                      
    data_arry[0] = 0xC0001500;
    dsi_set_cmdq(&data_arry,1,1);
    //                      
    data_arry[0] = 0x00C51500;
    dsi_set_cmdq(&data_arry,1,1);

    //                      
    data_arry[0] = 0x00001500;
    dsi_set_cmdq(&data_arry,1,1);

    //                           
    data_arry[0] = 0x00033902;
    data_arry[1] = 0x004343D8;
    dsi_set_cmdq(&data_arry,2,1);

    //                      
    data_arry[0] = 0xB1001500;
    dsi_set_cmdq(&data_arry,1,1);
    //                      
    data_arry[0] = 0xA9C51500;
    dsi_set_cmdq(&data_arry,1,1);

    //                      
    data_arry[0] = 0x90001500;
    dsi_set_cmdq(&data_arry,1,1);

    //                                
    data_arry[0] = 0x00043902;
    data_arry[1] = 0x01A796C5;
    dsi_set_cmdq(&data_arry,2,1);

    //                      
    data_arry[0] = 0x82001500;
    dsi_set_cmdq(&data_arry,1,1);
    //                      
    data_arry[0] = 0xA3C51500;
    dsi_set_cmdq(&data_arry,1,1);

    //                      
    data_arry[0] = 0x81001500;
    dsi_set_cmdq(&data_arry,1,1);
    //                      
    data_arry[0] = 0x66C11500;
    dsi_set_cmdq(&data_arry,1,1);

    //                      
    data_arry[0] = 0xA0001500;
    dsi_set_cmdq(&data_arry,1,1);
    //                      
    data_arry[0] = 0xEAC11500;
    dsi_set_cmdq(&data_arry,1,1);

    //                      
    data_arry[0] = 0xA1001500;
    dsi_set_cmdq(&data_arry,1,1);
    //                      
    data_arry[0] = 0x08C11500;     //
    dsi_set_cmdq(&data_arry,1,1);

    //                      
    data_arry[0] = 0xA2001500;
    dsi_set_cmdq(&data_arry,1,1);


    //                           
    data_arry[0] = 0x00033902;
    data_arry[1] = 0x001B02C0;
    dsi_set_cmdq(&data_arry,2,1);

    //                      
    data_arry[0] = 0x80001500;
    dsi_set_cmdq(&data_arry,1,1);
    //                      
    data_arry[0] = 0x30C41500;
    dsi_set_cmdq(&data_arry,1,1);

    //                      
    data_arry[0] = 0x81001500;
    dsi_set_cmdq(&data_arry,1,1);
    //                      
    data_arry[0] = 0x83C41500;
    dsi_set_cmdq(&data_arry,1,1);

    //                      
    data_arry[0] = 0x88001500;
    dsi_set_cmdq(&data_arry,1,1);
    //                      
    data_arry[0] = 0x80C41500;
    dsi_set_cmdq(&data_arry,1,1);

    //                      
    data_arry[0] = 0xA1001500;
    dsi_set_cmdq(&data_arry,1,1);
    //                      
    data_arry[0] = 0x10B31500;
    dsi_set_cmdq(&data_arry,1,1);

    //                      
    data_arry[0] = 0xB4001500;
    dsi_set_cmdq(&data_arry,1,1);
    //                      
    data_arry[0] = 0x50C01500;
    dsi_set_cmdq(&data_arry,1,1);

    //                      
    data_arry[0] = 0x00001500;
    dsi_set_cmdq(&data_arry,1,1);
    //                      
    data_arry[0] = 0x00361500;
    dsi_set_cmdq(&data_arry,1,1);

    //                      
    data_arry[0] = 0x90001500;
    dsi_set_cmdq(&data_arry,1,1);
    //                              
    //                 
    data_arry[0] = 0x00073902;
    data_arry[1] = 0x004400C0;
    data_arry[2] = 0x00030000;
    dsi_set_cmdq(&data_arry,3,1);

    //                      
    data_arry[0] = 0xA6001500;
    dsi_set_cmdq(&data_arry,1,1);

    //                                
    data_arry[0] = 0x00043902;
    data_arry[1] = 0x000001C1;
    dsi_set_cmdq(&data_arry,2,1);

    //                      
    data_arry[0] = 0x80001500;
    dsi_set_cmdq(&data_arry,1,1);

    //                               
    //                
    data_arry[0] = 0x00073902;
    data_arry[1] = 0x140387CE;
    data_arry[2] = 0x00140386;
    dsi_set_cmdq(&data_arry,3,1);

    //                      
    data_arry[0] = 0x90001500;
    dsi_set_cmdq(&data_arry,1,1);

    //                               
    //                
    data_arry[0] = 0x00073902;
    data_arry[1] = 0x141E33CE;
    data_arry[2] = 0x00141F33;
    dsi_set_cmdq(&data_arry,3,1);

    //                      
    data_arry[0] = 0xA0001500;
    dsi_set_cmdq(&data_arry,1,1);

    //                                
    //                    
    //                    
    //                
    data_arry[0] = 0x000F3902;
    data_arry[1] = 0x030338CE;
    data_arry[2] = 0x0014001C;
    data_arry[3] = 0x1D030238;
    data_arry[4] = 0x00001400;
    dsi_set_cmdq(&data_arry,5,1);

    //                      
    data_arry[0] = 0xB0001500;
    dsi_set_cmdq(&data_arry,1,1);

    //                                
    //                    
    //                    
    //                
    data_arry[0] = 0x000F3902;
    data_arry[1] = 0x030138CE;
    data_arry[2] = 0x0014001E;
    data_arry[3] = 0x1F030038;
    data_arry[4] = 0x00001400;
    dsi_set_cmdq(&data_arry,5,1);

    //                      
    data_arry[0] = 0xC0001500;

    dsi_set_cmdq(&data_arry,1,1);
    //                                
    //                    
    //                    
    //                
    data_arry[0] = 0x000F3902;
    data_arry[1] = 0x030030CE;
    data_arry[2] = 0x00140020;
    data_arry[3] = 0x21030130;
    data_arry[4] = 0x00001400;
    dsi_set_cmdq(&data_arry,5,1);

    //                      
    data_arry[0] = 0xD0001500;
    dsi_set_cmdq(&data_arry,1,1);

    //                                
    //                    
    //                    
    //                
    data_arry[0] = 0x000F3902;
    data_arry[1] = 0x030230CE;
    data_arry[2] = 0x00140022;
    data_arry[3] = 0x23030330;
    data_arry[4] = 0x00001400;
    dsi_set_cmdq(&data_arry,5,1);  //

    //                      
    data_arry[0] = 0xC6001500;
    dsi_set_cmdq(&data_arry,1,1);

    //                           
    data_arry[0] = 0x00033902;
    data_arry[1] = 0x008001CF;
    dsi_set_cmdq(&data_arry,2,1);

    //                      
    data_arry[0] = 0xC9001500;
    dsi_set_cmdq(&data_arry,1,1);
    //                      
    data_arry[0] = 0x10CF1500;
    dsi_set_cmdq(&data_arry,1,1);

    //                      
    data_arry[0] = 0xC0001500;
    dsi_set_cmdq(&data_arry,1,1);

    //                                
    //                    
    //                    
    //                     
    data_arry[0] = 0x00103902;
    data_arry[1] = 0x545400CB;
    data_arry[2] = 0x00005454;
    data_arry[3] = 0x54545454;
    data_arry[4] = 0x00000000;
    dsi_set_cmdq(&data_arry,5,1);

    //                      
    data_arry[0] = 0xD0001500;
    dsi_set_cmdq(&data_arry,1,1);

    //                                
    //                    
    //                    
    //                     
    data_arry[0] = 0x00103902;
    data_arry[1] = 0x000000CB;
    data_arry[2] = 0x54000000;
    data_arry[3] = 0x00545454;
    data_arry[4] = 0x54545400;
    dsi_set_cmdq(&data_arry,5,1);

    //                      
    data_arry[0] = 0xE0001500;
    dsi_set_cmdq(&data_arry,1,1);

    //                                
    //                    
    //                
    data_arry[0] = 0x000B3902;
    data_arry[1] = 0x000054CB;
    data_arry[2] = 0x00000000;
    data_arry[3] = 0x00000000;
    dsi_set_cmdq(&data_arry,4,1);

    //                      
    data_arry[0] = 0x80001500;
    dsi_set_cmdq(&data_arry,1,1);

    //                                
    //                    
    //                
    data_arry[0] = 0x000B3902;
    data_arry[1] = 0x252600CC;
    data_arry[2] = 0x00000602;
    data_arry[3] = 0x000C0E0A;
    dsi_set_cmdq(&data_arry,4,1);

    //                      
    data_arry[0] = 0x90001500;
    dsi_set_cmdq(&data_arry,1,1);
    //                                
    //                    
    //                    
    //                     
    data_arry[0] = 0x00103902;
    data_arry[1] = 0x000010CC;
    data_arry[2] = 0x00000000;
    data_arry[3] = 0x00000000;
    data_arry[4] = 0x05012526;
    dsi_set_cmdq(&data_arry,5,1);

    //                      
    data_arry[0] = 0xA0001500;
    dsi_set_cmdq(&data_arry,1,1);
    //                                
    //                    
    //                    
    //                     
    data_arry[0] = 0x00103902;//     
    data_arry[1] = 0x090000CC;
    data_arry[2] = 0x000F0B0D;
    data_arry[3] = 0x00000000;
    data_arry[4] = 0x00000000;
    dsi_set_cmdq(&data_arry,5,1);

    //                      
    data_arry[0] = 0xB0001500;
    dsi_set_cmdq(&data_arry,1,1);
    //                                
    //                    
    //                
    data_arry[0] = 0x000B3902;
    data_arry[1] = 0x262500CC;
    data_arry[2] = 0x00000105;
    data_arry[3] = 0x000D0B0F;
    dsi_set_cmdq(&data_arry,4,1);

    //                      
    data_arry[0] = 0xC0001500;
    dsi_set_cmdq(&data_arry,1,1);
    //                                
    //                    
    //                    
    //                     
    data_arry[0] = 0x00103902;
    data_arry[1] = 0x000009CC;
    data_arry[2] = 0x00000000;
    data_arry[3] = 0x00000000;
    data_arry[4] = 0x02062625;
    dsi_set_cmdq(&data_arry,5,1);

    //                      
    data_arry[0] = 0xD0001500;
    dsi_set_cmdq(&data_arry,1,1);
    //                                
    //                    
    //                    
    //                     
    data_arry[0] = 0x00103902;
    data_arry[1] = 0x100000CC;
    data_arry[2] = 0x000A0E0C;
    data_arry[3] = 0x00000000;
    data_arry[4] = 0x00000000;
    dsi_set_cmdq(&data_arry,5,1);

    //                      
    data_arry[0] = 0x00001500;
    dsi_set_cmdq(&data_arry,1,1);
    //                      
    data_arry[0] = 0x31D91500;
    dsi_set_cmdq(&data_arry,1,1);

    //                      
    data_arry[0] = 0x00001500;
    dsi_set_cmdq(&data_arry,1,1);
    //                      
    data_arry[0] = 0x00001500;
    dsi_set_cmdq(&data_arry,1,1);

    //                      
    data_arry[0] = 0x00001500;
    dsi_set_cmdq(&data_arry,1,1);
    //                                
    //                    
    //                    
    //                    
    //      
    data_arry[0] = 0x00113902;
    data_arry[1] = 0x0E0706E1;
    data_arry[2] = 0x0C16070D;
    data_arry[3] = 0x0506020C;
    data_arry[4] = 0x272B0F07;
    data_arry[5] = 0x0000000D;
    dsi_set_cmdq(&data_arry,6,1);

    //                      
    data_arry[0] = 0x00001500;
    dsi_set_cmdq(&data_arry,1,1);
    //                                
    //                    
    //                    
    //                    
    //      
    data_arry[0] = 0x00113902;
    data_arry[1] = 0x0E0706E2;
    data_arry[2] = 0x0C16070D;
    data_arry[3] = 0x0506020C;
    data_arry[4] = 0x272B0F07;
    data_arry[5] = 0x0000000D;
    dsi_set_cmdq(&data_arry,6,1);

    data_arry[0] = 0x00001500;
    dsi_set_cmdq(&data_arry,1,1);

    //                                
    data_arry[0] = 0x00043902;
    data_arry[1] = 0xffffffff;
    dsi_set_cmdq(&data_arry,2,1);

    data_arry[0] = 0x00001500;
    dsi_set_cmdq(&data_arry,1,1);
    //                       
    data_arry[0] = 0x773A1500;
    dsi_set_cmdq(&data_arry,1,1);

    //                      
    data_arry[0] = 0x00110500;
    dsi_set_cmdq(&data_arry,1,1);
    MDELAY(150);
    //                      
    data_arry[0] = 0x00290500;
    dsi_set_cmdq(&data_arry,1,1);
    MDELAY(50);
}

static void lcm_init(void)
{
    SET_RESET_PIN(1);
    SET_RESET_PIN(0);
    MDELAY(10);
    SET_RESET_PIN(1);
    MDELAY(50);

    init_lcm_registers();
}

static void lcm_suspend(void)
{
    unsigned int data_arry[16];

    //                       
    data_arry[0] = 0x00280500;
    dsi_set_cmdq(&data_arry,1,1);
    MDELAY(50);
    //                       
    data_arry[0] = 0x00100500;
    dsi_set_cmdq(&data_arry,1,1);
    MDELAY(150);
}


static void lcm_resume(void)
{
    unsigned int data_arry[16];

    //                       
    data_arry[0] = 0xFF511500;
    dsi_set_cmdq(&data_arry,1,1);
    MDELAY(50);
    //                       
    data_arry[0] = 0x00290500;
    dsi_set_cmdq(&data_arry,1,1);
    MDELAY(50);
    //                       
    data_arry[0] = 0x00110500;
    dsi_set_cmdq(&data_arry,1,1);
    MDELAY(150);
}


#ifndef BUILD_LK
//                 
#endif

static unsigned int lcm_esd_check(void)
{
    static int count = 0;
    static int err_count = 0;
    static int uncount = 0;
    int i;
    unsigned char fResult;
    unsigned char buffer[12];
    unsigned int array[16];

#ifdef ESD_DEBUG
    printk("lcm_esd_check <<<\n");
#endif
    for (i = 0; i < 12; i++)
        buffer[i] = 0x00;

    //                                 
    //                        
    //                                 
    array[0] = 0x00013700;
    dsi_set_cmdq(array, 1, 1);

    //                                 
    //                                             
    //                                 
    read_reg_v2(0x0A, buffer, 7);

#ifdef ESD_DEBUG
    printk("lcm_esd_check : read(0x0A)\n");
#endif

#ifdef ESD_DEBUG
    for (i = 0; i < 7; i++)
        printk("buffer[%d] : 0x%x \n", i, buffer[i]);
#endif

    //                                 
    //                             
    //                                 
    if (buffer[3] == 0x02) //                                      
    {
        if (buffer[4] & 0x02) //                     
            err_count ++;
        else
            err_count = 0;
    }
    else
    {
        err_count = 0;
    }

#ifdef ESD_DEBUG
    printk("lcm_esd_check err_count = %d\n", err_count);
#endif
    if ((buffer[0] != 0x9C) || (err_count >= 2))
    {
        err_count = 0;
        uncount++;

#ifndef BUILD_LK
        printk("lcm_esd_check failed, err_count = %d\n", err_count);
        for (i = 0; i < 7; i++)
	        printk("buffer[%d] : 0x%x \n", i, buffer[i]);
#endif

#ifdef ESD_DEBUG
        printk("lcm_esd_check unnormal uncount = %d\n", uncount);
        printk("lcm_esd_check >>>\n");
#endif
        fResult = 1;
    }
    else
    {
        count ++;
#ifdef ESD_DEBUG
        printk("lcm_esd_check normal count = %d\n", count);
        printk("lcm_esd_check >>>\n");
#endif
        fResult = 0;
    }

    //                                 
    //                        
    //                                 
    array[0] = 0x00033700;
    dsi_set_cmdq(array, 1, 1);

    //                                 
    //                   
    //                                                                  
    //                                 
    read_reg_v2(0xBC, buffer, 12);

#ifdef ESD_DEBUG
    printk("lcm_esd_check : read(0xBC)\n");
#endif

#ifdef ESD_DEBUG
    for (i = 0; i < 12; i++)                 		
        printk("buffer[%d] : 0x%x \n", i, buffer[i]); 
#endif

    if (fResult)
        return TRUE;
    else
        return FALSE;
} 

static unsigned int lcm_esd_recover(void)
{
    static int recount = 0;

#ifdef ESD_DEBUG
    printk("lcm_esd_recover\n");
#endif

    lcm_init();
    recount ++;

#ifndef BUILD_LK
    printk("lcm_esd_recover recover recount = %d\n", recount);
#endif
    return TRUE;
}

LCM_DRIVER otm8018b_dsi_vdo_lcsh72_lcm_drv = 
{
    .name           = "otm8018b_dsi_vdo_lcm_drv_dnj",
    .set_util_funcs = lcm_set_util_funcs,
    .compare_id     = lcm_compare_id,
    .get_params     = lcm_get_params,
    .init           = lcm_init,
    .suspend        = lcm_suspend,
    .resume         = lcm_resume,
    .esd_check      = lcm_esd_check,      //                                           
    .esd_recover    = lcm_esd_recover,    //                                           
};

