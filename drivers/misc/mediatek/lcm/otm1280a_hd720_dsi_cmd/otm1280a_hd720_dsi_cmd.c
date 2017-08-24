#include <linux/string.h>

#include "lcm_drv.h"

//                                                                            
//                 
//                                                                            
#define LCM_DSI_CMD_MODE									1
#if (LCM_DSI_CMD_MODE)
#define FRAME_WIDTH  										(720)
#define FRAME_HEIGHT 										(1280)
#else
#define FRAME_WIDTH  										(720)
#define FRAME_HEIGHT 										(1023)
#endif

#define REGFLAG_DELAY             							0XFE
#define REGFLAG_END_OF_TABLE      							0xFF   //                        


#ifndef TRUE
    #define TRUE 1
#endif

#ifndef FALSE
    #define FALSE 0
#endif
#define NT35590    0
static unsigned int lcm_esd_test = FALSE;      //                  
//                                  

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

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	        lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)											lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)   				lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)    

struct LCM_setting_table {
    unsigned char cmd;
    unsigned char count;
    unsigned char para_list[64];
};


static struct LCM_setting_table lcm_initialization_setting[] = {
	
	/*
       

                                            
 
                                           
                                           
                                           

                   

                                                     
                                           

    

                                  
 
                                 
 */

/* 
                               
                         

                         
                         

                                    
                            
                            
                        
                         

                                    
            
                         

                                    
                            
                            
                            
                            
                            
                  
                         

                                    
                            
                            
                            
                        
                         


                   
                   

                                                            
                         

                
                                                             
                   

             
                                    
                            
                            
                            
                            
                            
                            
                            
                  
                          

                                    
                            
                            
            

                   
                         

                   
                         

                   
                         

                   
                         
 
        
                                                                                                                                     
*/
};


#if 0
static struct LCM_setting_table lcm_set_window[] = {
	{0x2A,	4,	{0x00, 0x00, (FRAME_WIDTH>>8), (FRAME_WIDTH&0xFF)}},
	{0x2B,	4,	{0x00, 0x00, (FRAME_HEIGHT>>8), (FRAME_HEIGHT&0xFF)}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};
#endif

static struct LCM_setting_table lcm_sleep_out_setting[] = {
    //          
	{0x11, 1, {0x00}},
    {REGFLAG_DELAY, 120, {}},

    //           
	{0x29, 1, {0x00}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_deep_sleep_mode_in_setting[] = {
	//                     
	{0x28, 1, {0x00}},

    //              
	{0x10, 1, {0x00}},

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

static struct LCM_setting_table lcm_backlight_level_setting[] = {
	{0x51, 1, {0xFF}},
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
		unsigned int div2_real=0;
		unsigned int cycle_time = 0;
		unsigned int ui = 0;
		unsigned int hs_trail_m, hs_trail_n;
		#define NS_TO_CYCLE(n, c)	((n) / c + (( (n) % c) ? 1 : 0))

		memset(params, 0, sizeof(LCM_PARAMS));
	
		params->type   = LCM_TYPE_DSI;

		params->width  = FRAME_WIDTH;
		params->height = FRAME_HEIGHT;


#if (LCM_DSI_CMD_MODE)
		params->dbi.te_mode 				= LCM_DBI_TE_MODE_VSYNC_ONLY;
		params->dbi.te_edge_polarity		= LCM_POLARITY_FALLING;
#endif

#if (LCM_DSI_CMD_MODE)
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

		params->dsi.vertical_sync_active				= 2;
		params->dsi.vertical_backporch					= 8;
		params->dsi.vertical_frontporch					= 10;
		params->dsi.vertical_active_line				= FRAME_HEIGHT; 

		params->dsi.horizontal_sync_active				= 10;
		params->dsi.horizontal_backporch				= 20;
		params->dsi.horizontal_frontporch				= 40;
		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;

		//                     
#if (LCM_DSI_CMD_MODE)
		params->dsi.pll_div1=63;		//                                                             
		params->dsi.pll_div2=1;			//                             
#else
		params->dsi.pll_div1=63;		//                                                             
		params->dsi.pll_div2=1;			//                              
#endif
/*                  
             
*/

#if (NT35590)
		div2_real=params->dsi.pll_div2 ? params->dsi.pll_div2*0x02 : 0x1;
		cycle_time = (8 * 1000 * div2_real)/ (26 * (params->dsi.pll_div1+0x01));
		ui = (1000 * div2_real)/ (26 * (params->dsi.pll_div1+0x01)) + 1;
		
		hs_trail_m=params->dsi.LANE_NUM;
		hs_trail_n=NS_TO_CYCLE(((params->dsi.LANE_NUM * 4 * ui) + 60), cycle_time);

//                                                                                                                
		params->dsi.HS_TRAIL	= 20;
		params->dsi.HS_ZERO 	= NS_TO_CYCLE((115 + 6 * ui), cycle_time);//              
		params->dsi.HS_PRPR 	= NS_TO_CYCLE((50 + 4 * ui), cycle_time);//                           
		//                    
		if (params->dsi.HS_PRPR < 2)
			params->dsi.HS_PRPR = 2;

		params->dsi.LPX 		= NS_TO_CYCLE(200, cycle_time);//        
		
		params->dsi.TA_SACK 	= 1;
		params->dsi.TA_GET		= 5 * params->dsi.LPX;//     
		params->dsi.TA_SURE 	= 3 * params->dsi.LPX / 2;//                   
		params->dsi.TA_GO		= 4 * params->dsi.LPX;//     
	
		params->dsi.CLK_TRAIL	= NS_TO_CYCLE(70, cycle_time);//        
		//                      
		if (params->dsi.CLK_TRAIL < 2)
			params->dsi.CLK_TRAIL = 2;
		params->dsi.CLK_ZERO	= NS_TO_CYCLE((300), cycle_time);//              
		params->dsi.LPX_WAIT	= 1;
		params->dsi.CONT_DET	= 0;
		
		params->dsi.CLK_HS_PRPR = NS_TO_CYCLE((38 + 95) / 2, cycle_time);//                  

#endif
}



static void lcm_init_register(void)
{
	unsigned int data_array[16];
#if (NT35590) /*                  */
    	data_array[0] = 0x00023902;//                               
    	data_array[1] = 0x000000FF;                 
    	dsi_set_cmdq(&data_array, 2, 1);     
    	
    	data_array[0] = 0x00023902;//                             
    	data_array[1] = 0x000001BA;                 
    	dsi_set_cmdq(&data_array, 2, 1);    
    	
      data_array[0] = 0x00023902;//                   
      data_array[1] = 0x000008C2;                 
    	dsi_set_cmdq(&data_array, 2, 1);   
    	
      data_array[0] = 0x00023902;//            
      data_array[1] = 0x000001FF;                 
    	dsi_set_cmdq(&data_array, 2, 1);   
    	
      data_array[0] = 0x00023902; 
      data_array[1] = 0x00003A00;                 
    	dsi_set_cmdq(&data_array, 2, 1);  
    	
    	data_array[0] = 0x00023902;
      data_array[1] = 0x00003301;                 
    	dsi_set_cmdq(&data_array, 2, 1);  
    	
    	data_array[0] = 0x00023902;
      data_array[1] = 0x00005302;                 
    	dsi_set_cmdq(&data_array, 2, 1);  
    	
    	data_array[0] = 0x00023902;//        
      data_array[1] = 0x00008509;                 
    	dsi_set_cmdq(&data_array, 2, 1);  
    	
    	data_array[0] = 0x00023902;//          
      data_array[1] = 0x0000250E;                 
    	dsi_set_cmdq(&data_array, 2, 1);  
    	
    	data_array[0] = 0x00023902;//                          
      data_array[1] = 0x00000A0F;                 
    	dsi_set_cmdq(&data_array, 2, 1);  
    	
    	data_array[0] = 0x00023902;//             
      data_array[1] = 0x0000970B;                 
    	dsi_set_cmdq(&data_array, 2, 1);  
    	
    	data_array[0] = 0x00023902;//
      data_array[1] = 0x0000970C;                 
    	dsi_set_cmdq(&data_array, 2, 1);  

    	data_array[0] = 0x00023902;//   
      data_array[1] = 0x00008C11;                 
    	dsi_set_cmdq(&data_array, 2, 1);  
    	
    	data_array[0] = 0x00023902;//   
      data_array[1] = 0x00007B36;                 
    	dsi_set_cmdq(&data_array, 2, 1);  
    	
    	data_array[0] = 0x00023902;//             
      data_array[1] = 0x00002C71;                 
    	dsi_set_cmdq(&data_array, 2, 1);  

    	data_array[0] = 0x00023902;
      data_array[1] = 0x000005FF;         
    	dsi_set_cmdq(&data_array, 2, 1);    
    	
    	data_array[0] = 0x00023902; //                
      data_array[1] = 0x00000001;                   
    	dsi_set_cmdq(&data_array, 2, 1);              
    	data_array[0] = 0x00023902;                   
      data_array[1] = 0x00008D02;                   
    	dsi_set_cmdq(&data_array, 2, 1);              
    	data_array[0] = 0x00023902;                   
      data_array[1] = 0x00008D03;                   
    	dsi_set_cmdq(&data_array, 2, 1);              
    	data_array[0] = 0x00023902;                   
      data_array[1] = 0x00008D04;     
    	dsi_set_cmdq(&data_array, 2, 1);
    	data_array[0] = 0x00023902;     
      data_array[1] = 0x00003005;     
    	dsi_set_cmdq(&data_array, 2, 1);
    	data_array[0] = 0x00023902;//           
      data_array[1] = 0x00003306;             
    	dsi_set_cmdq(&data_array, 2, 1);        
      data_array[0] = 0x00023902;             
      data_array[1] = 0x00007707;             
    	dsi_set_cmdq(&data_array, 2, 1);        
    	data_array[0] = 0x00023902;             
      data_array[1] = 0x00000008;        
    	dsi_set_cmdq(&data_array, 2, 1);   
    	data_array[0] = 0x00023902;        
      data_array[1] = 0x00000009;        
    	dsi_set_cmdq(&data_array, 2, 1);   
    	data_array[0] = 0x00023902;        
      data_array[1] = 0x0000000a;        
    	dsi_set_cmdq(&data_array, 2, 1);   
    	data_array[0] = 0x00023902;        
      data_array[1] = 0x0000800b;     
    	dsi_set_cmdq(&data_array, 2, 1);
    	data_array[0] = 0x00023902;//   
      data_array[1] = 0x0000C80c;     
      dsi_set_cmdq(&data_array, 2, 1);
      data_array[0] = 0x00023902; //  
      data_array[1] = 0x0000000D;     
    	dsi_set_cmdq(&data_array, 2, 1);
    	data_array[0] = 0x00023902;     
      data_array[1] = 0x00001b0E;     
    	dsi_set_cmdq(&data_array, 2, 1);
    	data_array[0] = 0x00023902;     
      data_array[1] = 0x0000070F;     
    	dsi_set_cmdq(&data_array, 2, 1);
    	data_array[0] = 0x00023902;     
      data_array[1] = 0x00005710;     
    	dsi_set_cmdq(&data_array, 2, 1);
    	data_array[0] = 0x00023902;     
      data_array[1] = 0x00000011;     
    	dsi_set_cmdq(&data_array, 2, 1);
    	data_array[0] = 0x00023902;//   
      data_array[1] = 0x00000012;     
    	dsi_set_cmdq(&data_array, 2, 1);
      data_array[0] = 0x00023902;            
      data_array[1] = 0x00001e13;            
    	dsi_set_cmdq(&data_array, 2, 1);       
    	data_array[0] = 0x00023902;            
      data_array[1] = 0x00000014;            
    	dsi_set_cmdq(&data_array, 2, 1);       
    	data_array[0] = 0x00023902;            
      data_array[1] = 0x00001A15;            
    	dsi_set_cmdq(&data_array, 2, 1);       
    	data_array[0] = 0x00023902;            
      data_array[1] = 0x00000516;            
    	dsi_set_cmdq(&data_array, 2, 1);       
    	data_array[0] = 0x00023902;            
      data_array[1] = 0x00000017;             
    	dsi_set_cmdq(&data_array, 2, 1);     
    	
    	data_array[0] = 0x00023902;//   
      data_array[1] = 0x00001E18;     
    	dsi_set_cmdq(&data_array, 2, 1);
      data_array[0] = 0x00023902;            
      data_array[1] = 0x0000FF19;            
    	dsi_set_cmdq(&data_array, 2, 1);       
    	data_array[0] = 0x00023902;            
      data_array[1] = 0x0000001A;            
    	dsi_set_cmdq(&data_array, 2, 1);       
    	data_array[0] = 0x00023902;            
      data_array[1] = 0x0000FC1B;            
    	dsi_set_cmdq(&data_array, 2, 1);       
    	data_array[0] = 0x00023902;            
      data_array[1] = 0x0000801C;            
    	dsi_set_cmdq(&data_array, 2, 1);       
    	data_array[0] = 0x00023902;            
      data_array[1] = 0x0000001D;             
    	dsi_set_cmdq(&data_array, 2, 1);  
    	   
    	data_array[0] = 0x00023902;//           
      data_array[1] = 0x0000001E;             
			dsi_set_cmdq(&data_array, 2, 1);     
			                                     
			data_array[0] = 0x00023902;          
      data_array[1] = 0x0000771F;          
    	dsi_set_cmdq(&data_array, 2, 1);     
    	data_array[0] = 0x00023902;                                   
      data_array[1] = 0x00000020;          
    	dsi_set_cmdq(&data_array, 2, 1);     
    	data_array[0] = 0x00023902;          
      data_array[1] = 0x00000021;         
    	dsi_set_cmdq(&data_array, 2, 1);     
    	data_array[0] = 0x00023902;          
      data_array[1] = 0x00005522;           
    	dsi_set_cmdq(&data_array, 2, 1);      
    	data_array[0] = 0x00023902;            
      data_array[1] = 0x00000D23;            
    	dsi_set_cmdq(&data_array, 2, 1);        
    	data_array[0] = 0x00023902;//   
      data_array[1] = 0x0000A031;     
    	dsi_set_cmdq(&data_array, 2, 1);
      data_array[0] = 0x00023902;     
      data_array[1] = 0x00000032;     
    	dsi_set_cmdq(&data_array, 2, 1);
    	data_array[0] = 0x00023902;     
      data_array[1] = 0x0000B833;         
    	dsi_set_cmdq(&data_array, 2, 1);    
    	data_array[0] = 0x00023902;            
      data_array[1] = 0x0000BB34;            
    	dsi_set_cmdq(&data_array, 2, 1);        
    	data_array[0] = 0x00023902;             
      data_array[1] = 0x00001135;             
    	dsi_set_cmdq(&data_array, 2, 1);        
    	data_array[0] = 0x00023902;             
      data_array[1] = 0x00000136;             
    	dsi_set_cmdq(&data_array, 2, 1);        
    	data_array[0] = 0x00023902;//           
      data_array[1] = 0x00000B37;             
      dsi_set_cmdq(&data_array, 2, 1);        
      data_array[0] = 0x00023902; //          
      data_array[1] = 0x00000138;             
    	dsi_set_cmdq(&data_array, 2, 1);        
    	data_array[0] = 0x00023902;             
      data_array[1] = 0x00000B39;             
    	dsi_set_cmdq(&data_array, 2, 1);        
    	data_array[0] = 0x00023902;             
      data_array[1] = 0x00000844;             
    	dsi_set_cmdq(&data_array, 2, 1);        
    	data_array[0] = 0x00023902;             
      data_array[1] = 0x00008045;             
    	dsi_set_cmdq(&data_array, 2, 1);        
    	data_array[0] = 0x00023902;                
      data_array[1] = 0x0000CC46;                
    	dsi_set_cmdq(&data_array, 2, 1);           
    	data_array[0] = 0x00023902;//              
      data_array[1] = 0x00000447;                
    	dsi_set_cmdq(&data_array, 2, 1);           
      data_array[0] = 0x00023902;                          
      data_array[1] = 0x00000048;                          
    	dsi_set_cmdq(&data_array, 2, 1);                     
    	data_array[0] = 0x00023902;                          
      data_array[1] = 0x00000049;                                 
    	dsi_set_cmdq(&data_array, 2, 1);                            
    	data_array[0] = 0x00023902;                                 
      data_array[1] = 0x0000014a;                                 
    	dsi_set_cmdq(&data_array, 2, 1);                            
    	data_array[0] = 0x00023902;                                 
      data_array[1] = 0x0000036c;                                 
    	dsi_set_cmdq(&data_array, 2, 1);                            
    	data_array[0] = 0x00023902;                                 
      data_array[1] = 0x0000036D;                                 
    	dsi_set_cmdq(&data_array, 2, 1);                            
    	data_array[0] = 0x00023902;//                               
      data_array[1] = 0x00002F6E;                                 
			dsi_set_cmdq(&data_array, 2, 1); 		
			
    	data_array[0] = 0x00023902; //  
      data_array[1] = 0x00000043;     
    	dsi_set_cmdq(&data_array, 2, 1);
    	data_array[0] = 0x00023902;     
      data_array[1] = 0x0000234b;     
    	dsi_set_cmdq(&data_array, 2, 1);
    	data_array[0] = 0x00023902;     
      data_array[1] = 0x0000014c;     
    	dsi_set_cmdq(&data_array, 2, 1);
    	data_array[0] = 0x00023902;      
      data_array[1] = 0x00002350;      
    	dsi_set_cmdq(&data_array, 2, 1); 
    	data_array[0] = 0x00023902;      
      data_array[1] = 0x00000151;      
    	dsi_set_cmdq(&data_array, 2, 1); 
    	data_array[0] = 0x00023902;//    
      data_array[1] = 0x00002358;      
    	dsi_set_cmdq(&data_array, 2, 1); 
      data_array[0] = 0x00023902;      
      data_array[1] = 0x00000159;     
    	dsi_set_cmdq(&data_array, 2, 1);
    	data_array[0] = 0x00023902;     
      data_array[1] = 0x0000235D;     
    	dsi_set_cmdq(&data_array, 2, 1);
    	data_array[0] = 0x00023902;     
      data_array[1] = 0x0000015E;     
    	dsi_set_cmdq(&data_array, 2, 1);
    	data_array[0] = 0x00023902;     
      data_array[1] = 0x00002362;     
    	dsi_set_cmdq(&data_array, 2, 1);
    	data_array[0] = 0x00023902;     
      data_array[1] = 0x00000163;     
    	dsi_set_cmdq(&data_array, 2, 1);
    	data_array[0] = 0x00023902;//   
      data_array[1] = 0x00002367;       
      dsi_set_cmdq(&data_array, 2, 1); 
      data_array[0] = 0x00023902; //  
      data_array[1] = 0x00000168;     
    	dsi_set_cmdq(&data_array, 2, 1);
    	data_array[0] = 0x00023902;     
      data_array[1] = 0x00000089;     
    	dsi_set_cmdq(&data_array, 2, 1);
    	data_array[0] = 0x00023902;     
      data_array[1] = 0x0000018D;     
    	dsi_set_cmdq(&data_array, 2, 1);
    	data_array[0] = 0x00023902;     
      data_array[1] = 0x0000648E;     
    	dsi_set_cmdq(&data_array, 2, 1);
    	data_array[0] = 0x00023902;                       
      data_array[1] = 0x0000208F;                       
    	dsi_set_cmdq(&data_array, 2, 1);                  
    	data_array[0] = 0x00023902;//                     
      data_array[1] = 0x00008E97;                       
    	dsi_set_cmdq(&data_array, 2, 1);                  
      data_array[0] = 0x00023902;                                 
      data_array[1] = 0x00008C82;                                 
    	dsi_set_cmdq(&data_array, 2, 1);                            
    	data_array[0] = 0x00023902;                                 
      data_array[1] = 0x00000283;                                 
    	dsi_set_cmdq(&data_array, 2, 1);                            
    	data_array[0] = 0x00023902;                                 
      data_array[1] = 0x00000ABB;                                 
    	dsi_set_cmdq(&data_array, 2, 1);                            
    	data_array[0] = 0x00023902;                                 
      data_array[1] = 0x00000ABc;                                 
    	dsi_set_cmdq(&data_array, 2, 1);                            
    	data_array[0] = 0x00023902;                                 
      data_array[1] = 0x00002524;                                 
    	dsi_set_cmdq(&data_array, 2, 1);                            
    	data_array[0] = 0x00023902;//                               
      data_array[1] = 0x00005525;                                 
			dsi_set_cmdq(&data_array, 2, 1); 	
			
			data_array[0] = 0x00023902;      
      data_array[1] = 0x00000526;      
    	dsi_set_cmdq(&data_array, 2, 1); 
    	data_array[0] = 0x00023902;      
      data_array[1] = 0x00002327;      
    	dsi_set_cmdq(&data_array, 2, 1); 
    	data_array[0] = 0x00023902;      
      data_array[1] = 0x00000128;      
    	dsi_set_cmdq(&data_array, 2, 1); 
    	data_array[0] = 0x00023902;      
      data_array[1] = 0x00003129;      
    	dsi_set_cmdq(&data_array, 2, 1); 
    	data_array[0] = 0x00023902;      
      data_array[1] = 0x00005D2A;      
    	dsi_set_cmdq(&data_array, 2, 1); 
    	data_array[0] = 0x00023902;//   
      data_array[1] = 0x0000012B;     
    	dsi_set_cmdq(&data_array, 2, 1);
      data_array[0] = 0x00023902;     
      data_array[1] = 0x0000002F;     
    	dsi_set_cmdq(&data_array, 2, 1);
    	data_array[0] = 0x00023902;     
      data_array[1] = 0x00001030;          
    	dsi_set_cmdq(&data_array, 2, 1);     
    	data_array[0] = 0x00023902;             
      data_array[1] = 0x000012A7;             
    	dsi_set_cmdq(&data_array, 2, 1);        
    	data_array[0] = 0x00023902;             
      data_array[1] = 0x0000032D;             
    	dsi_set_cmdq(&data_array, 2, 1);

    	data_array[0] = 0x00023902;//       
      data_array[1] = 0x000000FF;                 
    	dsi_set_cmdq(&data_array, 2, 1);    
    	
    	data_array[0] = 0x00023902;
      data_array[1] = 0x000001FB;                 
    	dsi_set_cmdq(&data_array, 2, 1);    
    	
    	data_array[0] = 0x00023902;//           
      data_array[1] = 0x000001FF;                 
    	dsi_set_cmdq(&data_array, 2, 1);       
    	
    	data_array[0] = 0x00023902;
      data_array[1] = 0x000001FB;                 
    	dsi_set_cmdq(&data_array, 2, 1);   
    	
    	data_array[0] = 0x00023902;//           
      data_array[1] = 0x000002FF;                 
    	dsi_set_cmdq(&data_array, 2, 1);       
    	
    	data_array[0] = 0x00023902;
      data_array[1] = 0x000001FB;                 
    	dsi_set_cmdq(&data_array, 2, 1);       
    	
    	data_array[0] = 0x00023902;//           
      data_array[1] = 0x000003FF;                 
    	dsi_set_cmdq(&data_array, 2, 1);       
    	
    	data_array[0] = 0x00023902;
      data_array[1] = 0x000001FB;                 
    	dsi_set_cmdq(&data_array, 2, 1);     

    	data_array[0] = 0x00023902;//          
      data_array[1] = 0x000004FF;         
    	dsi_set_cmdq(&data_array, 2, 1);    
    	                                    
    	data_array[0] = 0x00023902;         
      data_array[1] = 0x000001FB;         
    	dsi_set_cmdq(&data_array, 2, 1);    
    	
    	data_array[0] = 0x00023902;//          
      data_array[1] = 0x000005FF;         
    	dsi_set_cmdq(&data_array, 2, 1);    
    	                                    
    	data_array[0] = 0x00023902;         
      data_array[1] = 0x000001FB;         
    	dsi_set_cmdq(&data_array, 2, 1); 
    	
    	data_array[0] = 0x00023902;     //           
      data_array[1] = 0x000000FF;         
    	dsi_set_cmdq(&data_array, 2, 1);  
    	
     	data_array[0] = 0x00110500;                
    	dsi_set_cmdq(&data_array, 1, 1); 
    	MDELAY(120); 
    	
    	data_array[0] = 0x00023902;//                 
      data_array[1] = 0x0000FF51;         
    	dsi_set_cmdq(&data_array, 2, 1);    
    	                                    
    	data_array[0] = 0x00023902;         
      data_array[1] = 0x00002C53;         
    	dsi_set_cmdq(&data_array, 2, 1); 
    	
    	data_array[0] = 0x00023902;     
      data_array[1] = 0x00000055;         
    	dsi_set_cmdq(&data_array, 2, 1);  
    	
    	data_array[0] = 0x00290500;                
    	dsi_set_cmdq(&data_array, 1, 1); 
    	
    	data_array[0] = 0x00023902;         
      data_array[1] = 0x000000FF;         
    	dsi_set_cmdq(&data_array, 2, 1); 
    	
    	data_array[0] = 0x00023902;     
        data_array[1] = 0x00000035;         
    	dsi_set_cmdq(&data_array, 2, 1);  
    	         
#else  //                   
    	data_array[0] = 0x00042902;                                    
    	data_array[1] = 0x018012FF;                                            
    	dsi_set_cmdq(&data_array, 2, 1);                                                        
                                                                               
     	data_array[0] = 0x80001500;                      
     	dsi_set_cmdq(&data_array, 1, 1);                           
     	MDELAY(50);                                     
                                                      
    	data_array[0] = 0x00032902;                          
    	data_array[1] = 0x008012FF;                                
    	dsi_set_cmdq(&data_array, 2, 1);                         
                                                              
     	data_array[0] = 0x84001500;                           
     	dsi_set_cmdq(&data_array, 1, 1);                 
     	MDELAY(50);                                    
     	                                               
    	data_array[0] = 0x00032902;                      
    	data_array[1] = 0x000210FF;                              
    	dsi_set_cmdq(&data_array, 2, 1);                 
                                                       
 		data_array[0] = 0x00351500; //                     
     	dsi_set_cmdq(&data_array, 1, 1);                 
     	MDELAY(50);	                                                  
     																		                
#if 1                                           
#if 0                                           
    	data_array[0] = 0x00042902;               
    	data_array[1] = 0x018012FF;               
    	dsi_set_cmdq(&data_array, 2, 1);  
                                        
     	data_array[0] = 0x80001500;       
     	dsi_set_cmdq(&data_array, 1, 1);  
     	MDELAY(50);	                      
                                        
    	data_array[0] = 0x00032902;       
    	data_array[1] = 0x008012FF;       
    	dsi_set_cmdq(&data_array, 2, 1);  
 #endif                                 
 		data_array[0] = 0x82001500;         
     	dsi_set_cmdq(&data_array, 1, 1);  
     	MDELAY(50);                       
                                        
    	data_array[0] = 0x00022902;       
    	data_array[1] = 0x00000DC1;       
    	dsi_set_cmdq(&data_array, 2, 1);  
/*                                      
                                       
                                       
                                       
*/                                      
#endif                                  
#endif                                  
}                                       

static void lcm_init(void)
{
    SET_RESET_PIN(1);
    SET_RESET_PIN(0);
    MDELAY(10);
    SET_RESET_PIN(1);
    MDELAY(10);
    lcm_init_register();
//                                                                                                                  
}


static void lcm_suspend(void)
{
	push_table(lcm_deep_sleep_mode_in_setting, sizeof(lcm_deep_sleep_mode_in_setting) / sizeof(struct LCM_setting_table), 1);
}


static void lcm_resume(void)
{
	lcm_init();
	
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
	unsigned int default_level = 145;
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

static unsigned int lcm_esd_check(void)
{
#ifndef BUILD_UBOOT
        if(lcm_esd_test)
        {
            lcm_esd_test = FALSE;
            return TRUE;
        }

        //                                                
        //                                                                      
        //                                                                   
        //                                                    
        /*
                                            
                                                 
                
                                                                        
                
                                                
        */

        if(read_reg(0xB6) == 0x42)
        {
            return FALSE;
        }
        else
        {            
            return TRUE;
        }
#endif
}

static unsigned int lcm_esd_recover(void)
{
    unsigned char para = 0;

    SET_RESET_PIN(1);
    SET_RESET_PIN(0);
    MDELAY(1);
    SET_RESET_PIN(1);
    MDELAY(120);
	  push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
    MDELAY(10);
	  push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1);
    MDELAY(10);
    dsi_set_cmdq_V2(0x35, 1, &para, 1);     //          
    MDELAY(10);

    return TRUE;
}

//                                                                            
//                      
//                                                                            
LCM_DRIVER otm1280a_hd720_dsi_cmd_drv = 
{
    .name			= "hd720",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
#if (LCM_DSI_CMD_MODE)
	.update         = lcm_update,
	.set_backlight	= lcm_setbacklight,
//                              
//                              
//                              
//                                       
//                                 
#endif
};
