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

#define FRAME_WIDTH  										(720)
#define FRAME_HEIGHT 										(1280)

#define REGFLAG_DELAY             							0XFEFF
#define REGFLAG_END_OF_TABLE      							0xFFFF   //                        

//                                  

#define LCM_ID_OTM1283 0x40

//                                                                            
//                 
//                                                                            

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    								(lcm_util.set_reset_pin((v)))
#define SET_GPIO_OUT(gpio_num,val)    						(lcm_util.set_gpio_out((gpio_num),(val)))


#define UDELAY(n) 											(lcm_util.udelay(n))
#define MDELAY(n) 											(lcm_util.mdelay(n))



//                   
//                    

#ifdef _LCM_DEBUG_
#define lcm_debug(fmt, args...) printk(fmt, ##args)
#else
#define lcm_debug(fmt, args...) do { } while (0)
#endif

#ifdef _LCM_INFO_
#define lcm_info(fmt, args...) printk(fmt, ##args)
#else
#define lcm_info(fmt, args...) do { } while (0)
#endif
#define lcm_err(fmt, args...) printk(fmt, ##args)

//                                                                            
//                 
//                                                                            

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg											lcm_util.dsi_read_reg()
#define read_reg_v2(cmd, buffer, buffer_size)   			lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)    
       

static struct LCM_setting_table {
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[64];
};

static struct LCM_setting_table lcm_initialization_setting[] = {
	
	/*
       

                                            
 
                                           
                                           
                                           

                   

                                                     
                                           

    

                                  
 
                                 
 */


	//                                                

	{0x00, 1, {0x00}},
	//                        
	{0xff, 3, {0x12,0x83,0x01}},
	//                        
	
	{0x00, 1, {0x80}},
	//                        
	{0xff, 2, {0x12,0x83}},
	//                        

//                                                         
	{0x00, 1, {0x80}},
	//                        
	{0xc0, 9, {0x00,0x64,0x00,0x10,0x10,0x00,0x64,0x10,0x10}},
	//                        

	{0x00, 1, {0x90}},
	//                        
	{0xc0, 6, {0x00,0x5c,0x00,0x01,0x00,0x04}},
	//                        
	
	{0x00, 1, {0xa4}},
	//                        
	{0xc0, 1, {0x22}},
	//                        
	
	{0x00, 1, {0xb3}},
	//                        
	{0xc0,2,{0x00,0x50}},
	//                         
	
	{0x00, 1, {0x81}},
	//                        
	{0xc1,1,{0x55}},
	//                        

	{0x00, 1, {0x90}},
	//                        
	{0xc4, 1, {0x49}},
	//                        
	
	{0x00, 1, {0xb9}},
	//                        
	{0xb0, 1, {0x51}},
	//                        
		
	{0x00,1,{0xa0}},             //            
	//                        
	{0xc4, 14, {0x05,0x10,0x06,0x02,0x05,0x15,0x10,0x05,0x10,0x07,0x02,0x05,0x15,0x10}},
	//                        

	{0x00, 1, {0xb0}},
	//                        
	{0xc4, 2, {0x00,0x00}},
	//                        

	{0x00,1,{0x91}},             //                                             
	//                        
	{0xc5,2,{0x46,0x40}},
	//                        

	{0x00,1,{0x00}},             //                        
	//                        
	{0xd8,2,{0xbc,0xbc}},
	//                        

	{0x00,1,{0x00}},             //           
	//                        
	{0xd9,1,{0x54}},
	//                        

	{0x00,1,{0x81}},             //                  
	{0xc4,1,{0x82}},
	//                        

	{0x00,1,{0xb0}},             //                                                           
	{0xc5,2,{0x04,0x38}},
	//                        

	{0x00,1,{0xbb}},             //                         
	{0xc5,1,{0x80}},
	//                        

	{0x00,1,{0x82}},		//                                    
	{0xC4,1,{0x02}}, 
	//                        

	{0x00,1,{0xc6}},		//         
	{0xB0,1,{0x03}}, 
	//                        

//                                                           
	{0x00,1,{0x00}},             //   
	{0xd0,1,{0x40}},
	//                        

	{0x00,1,{0x00}},             //        
	{0xd1,2,{0x00,0x00}},
	//                        

//                                                                      
	{0x00,1,{0x80}},             //                          
	//                        
	{0xcb,11,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
	//                        

	{0x00,1,{0x90}},             //                          
	//                        
	{0xcb,15,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
	//                        

	{0x00,1,{0xa0}},             //                          
	//                        
	{0xcb,15,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
	//                        

	{0x00,1,{0xb0}},             //                          
	//                         
	{0xcb,15,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
	//                        

	{0x00,1,{0xc0}},             //                          
	//                        
	{0xcb,15,{0x05,0x05,0x05,0x05,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
	//                        

	{0x00,1,{0xd0}},             //                          
	//                        
	{0xcb,15,{0xff,0xff,0xff,0x00,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x00,0x00,0x00}},
	//                        

	{0x00,1,{0xe0}},             //                          
	//                        
	{0xcb,14,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0x00,0x05,0x05,0x05}},
	//                        

	{0x00,1,{0xf0}},             //                          
	//                        
	{0xcb,11,{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}},
	//                        

//                                                                     
	{0x00,1,{0x80}},             //                         
	//                        
	{0xcc,15,{0x02,0x0a,0x0c,0x0e,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
	//                        

	{0x00,1,{0x90}},             //                         
	//                        
	{0xcc,15,{0x00,0x00,0x00,0x00,0x2e,0x2d,0x06,0x01,0x09,0x0b,0x0d,0x0f,0x00,0x00,0x00}},
	//                        

	{0x00,1,{0xa0}},             //                         
	//                        
	{0xcc,14,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2e,0x2d,0x05}},
	//                        

	{0x00,1,{0xb0}},             //                         
	//                        
	{0xcc,15,{0x05,0x0f,0x0d,0x0b,0x09,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
	//                        

	{0x00,1,{0xc0}},             //                         
	//                        
	{0xcc,15,{0x00,0x00,0x00,0x00,0x2d,0x2e,0x01,0x06,0x10,0x0e,0x0c,0x0a,0x00,0x00,0x00}},
	//                        

	{0x00,1,{0xd0}},             //                         
	//                        
	{0xcc,14,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2d,0x2e,0x02}},
	//                        

//                                                                
	{0x00,1,{0x80}},             //                 
	//                        
	{0xce,12,{0x87,0x03,0x10,0x86,0x03,0x10,0x00,0x00,0x00,0x00,0x00,0x00}},
	//                        

	{0x00,1,{0x90}},             //                  
	//                        
	{0xce,14,{0x35,0x01,0x10,0x35,0x02,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
	//                        

	{0x00,1,{0xa0}},             //                     
	//                        
	{0xce,14,{0x38,0x03,0x04,0xf8,0x00,0x10,0x00,0x38,0x02,0x04,0xf9,0x00,0x10,0x00}},
	//                        

	{0x00,1,{0xb0}},             //                     
	//                        
	{0xce,14,{0x38,0x01,0x04,0xfa,0x00,0x10,0x00,0x38,0x00,0x04,0xfb,0x00,0x10,0x00}},
	//                        

	{0x00,1,{0xc0}},             //                     
	//                        
	{0xce,14,{0x30,0x00,0x04,0xfc,0x00,0x10,0x00,0x30,0x01,0x04,0xfd,0x00,0x10,0x00}},
	//                        

	{0x00,1,{0xd0}},             //                     
	//                        
	{0xce,14,{0x30,0x02,0x04,0xfe,0x00,0x10,0x00,0x30,0x03,0x04,0xff,0x00,0x10,0x00}},
	//                        

	{0x00,1,{0x80}},             //                     
	//                        
	{0xcf,14,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
	//                        

	{0x00,1,{0x90}},             //                     
	//                        
	{0xcf,14,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
	//                        

	{0x00,1,{0xa0}},             //                     
	//                        
	{0xcf,14,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
	//                        

	{0x00,1,{0xb0}},             //                     
	//                        
	{0xcf,14,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
	//                        

	{0x00,1,{0xc0}},             //                  
	//                        
	{0xcf,11,{0x01,0x01,0x20,0x20,0x00,0x00,0x01,0x81,0x00,0x03,0x08}}, //              
	//                        

	{0x00,1,{0xb5}},             //                    
	//                        
	{0xc5,6,{0x3f,0xff,0xff,0x3f,0xff,0xff}},
	//                        
//                                                         
	{0x00,1,{0x90}},             //      
	//                        
	{0xf5,4,{0x02,0x11,0x02,0x11}},
	//                         
	
	{0x00, 1, {0x90}},
	//                        
	{0xc5, 1, {0x50}},
	//                          

	{0x00, 1, {0x94}},
	//                        
	{0xc5, 1, {0x66}},
	//                         

//                                                  
	{0x00, 1, {0xb2}},
	//                        
	{0xf5,2,{0x00,0x00}},
	//                        

	{0x00,1,{0xb4}},             //       
	//                        
	{0xf5,2,{0x00,0x00}},
	//                        

	{0x00,1,{0xb6}},             //     
	//                        
	{0xf5,2,{0x00,0x00}},
	//                        

	{0x00,1,{0xb8}},             //       
	//                         
	{0xf5,2,{0x00,0x00}},
	//                        

	{0x00,1,{0x94}},  		//         
	//                        
	{0xF5,1,{0x02}},
	//                        

	{0x00,1,{0xBA}},  		//           
	//                        
	{0xF5,1,{0x03}},
	//                        

	{0x00,1,{0xb4}},             //                         
	//                        
	{0xc5,1,{0xc0}},		//                                                 
	//                        

	{0x00,1,{0xb2}},             //                   
	//                        
	{0xc5,1,{0x40}},
	//                        


        {0x00,1,{0xA0}}, 
        //                        
        {0xC1,1,{0x02}},  //                                                                                             
       //                         
        
//                                                 
	{0x00,1,{0x00}},
	//                        
	{0xE1,16,{0x00,0x15,0x1B,0x0D,0x06,0x0E,0x09,0x09,0x04,0x07,0x0C,0x08,0x0F,0x0F,0x09,0x00}},
	//                        
							
	{0x00,1,{0x00}},
	//                        
	{0xE2,16,{0x00,0x15,0x1B,0x0D,0x06,0x0E,0x09,0x09,0x04,0x07,0x0C,0x08,0x0F,0x0F,0x09,0x00}},
	//                        


	{0x00,1,{0x00}},             //                  
	//                        
	{0xff,3,{0xff,0xff,0xff}},
	//                        
};

static struct LCM_setting_table lcm_set_window[] = {
	{0x2A,	4,	{0x00, 0x00, (FRAME_WIDTH>>8), (FRAME_WIDTH&0xFF)}},
	{0x2B,	4,	{0x00, 0x00, (FRAME_HEIGHT>>8), (FRAME_HEIGHT&0xFF)}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_sleep_out_setting[] = {
    //          
	{0x11, 0, {0x00}},
    {REGFLAG_DELAY, 120, {}},

    //           
	{0x29, 0, {0x00}},
//                         
	
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_sleep_in_setting[] = {
	//                     
	{0x28, 0, {0x00}},

    //              
	{0x10, 0, {0x00}},

	{REGFLAG_END_OF_TABLE, 0x00, {}}
};
static void lcm_init_registers()
{
	unsigned int data_array[16];
	
	lcm_debug("liyibo : %s %d\n", __func__,__LINE__);
	
	data_array[0] = 0x00042902;
	data_array[1] = 0x018312FF;
	dsi_set_cmdq(&data_array, 2, 1);//        
	
	data_array[0] = 0x80002300;
	dsi_set_cmdq(&data_array, 1, 1);	//                 
	
	data_array[0] = 0x00032902;
	data_array[1] = 0x008312FF;
	dsi_set_cmdq(&data_array, 2, 1);
	
	/*                                                    */
	data_array[0] = 0x80002300;
	dsi_set_cmdq(&data_array, 1, 1);	//            
	
	data_array[0] = 0x000A2902;
	data_array[1] = 0x006400C0;
	data_array[2] = 0x6400110F;
	data_array[3] = 0x0000110F;
	dsi_set_cmdq(&data_array, 4, 1);
	
	data_array[0] = 0x90002300;
	dsi_set_cmdq(&data_array, 1, 1);	//                    
	
	data_array[0] = 0x00072902;
	data_array[1] = 0x005C00C0;
	data_array[2] = 0x00040001;
	dsi_set_cmdq(&data_array, 3, 1);
	
	data_array[0] = 0xA4002300;
	dsi_set_cmdq(&data_array, 1, 1);	//           
	
	data_array[0] = 0x1CC02300;
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0] = 0xB3002300;
	dsi_set_cmdq(&data_array, 1, 1);	//                                              
	
	data_array[0] = 0x00032902;
	data_array[1] = 0x005000C0;
	dsi_set_cmdq(&data_array, 2, 1);
	
	data_array[0] = 0x81002300;
	dsi_set_cmdq(&data_array, 1, 1);	//                
	
	data_array[0] = 0x55C12300;
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0] = 0x81002300;				//                  
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0] = 0x82C42300;
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0] = 0x90002300;				//                          
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0] = 0x49C42300;
	dsi_set_cmdq(&data_array, 1, 1);
	
	/*                                            */
	data_array[0] = 0xA0002300;
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0] = 0x000F2902;	//            
	data_array[1] = 0x061005C4;
	data_array[2] = 0x10150502;
	data_array[3] = 0x02071005;
	data_array[4] = 0x00101505;
	dsi_set_cmdq(&data_array, 5, 1);
	
	data_array[0] = 0xB0002300;
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0] = 0x00032902;	//                     
	data_array[1] = 0x000000C4;
	dsi_set_cmdq(&data_array, 2, 1);
	
	data_array[0] = 0x91002300;
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0] = 0x00032902;	//                                              
	data_array[1] = 0x005019c5;
	dsi_set_cmdq(&data_array, 2, 1);
	
	data_array[0] = 0x00002300;
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0] = 0x00032902;	//                          
	data_array[1] = 0x00bcbcd8;
	dsi_set_cmdq(&data_array, 2, 1);
	
	data_array[0] = 0x00002300;
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0] = 0xd9652300;		//           
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0] = 0xB0002300;
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0] = 0x00032902;	//                           
	data_array[1] = 0x00b804c5;
	dsi_set_cmdq(&data_array, 2, 1);
	
	data_array[0] = 0xBB002300;
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0] = 0x80c52300;	//                         
	dsi_set_cmdq(&data_array, 1, 1);
	
	/*                                                              */
	data_array[0] = 0x00002300;
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0] = 0x40d02300;		//   
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0] = 0x00002300;
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0] = 0x00032902;	//        
	data_array[1] = 0x000000d1;
	dsi_set_cmdq(&data_array, 2, 1);
	
	/*                                                                    */
	data_array[0] = 0x80002300;
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0] = 0x000C2902;	//                          
	data_array[1] = 0x000000cb;
	data_array[2] = 0x00000000;
	data_array[3] = 0x00000000;
	dsi_set_cmdq(&data_array, 4, 1);
	
	data_array[0] = 0x90002300;
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0] = 0x00102902;	//                          
	data_array[1] = 0x000000cb;
	data_array[2] = 0x00000000;
	data_array[3] = 0x00000000;
	data_array[4] = 0x00000000;
	dsi_set_cmdq(&data_array, 5, 1);
	
	data_array[0] = 0xa0002300;
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0] = 0x00102902;	//                          
	data_array[1] = 0x000000cb;
	data_array[2] = 0x00000000;
	data_array[3] = 0x00000000;
	data_array[4] = 0x00000000;
	dsi_set_cmdq(&data_array, 5, 1);
	
	data_array[0] = 0xb0002300;
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0] = 0x00102902;	//                          
	data_array[1] = 0x000000cb;
	data_array[2] = 0x00000000;
	data_array[3] = 0x00000000;
	data_array[4] = 0x00000000;
	dsi_set_cmdq(&data_array, 5, 1);
	
	data_array[0] = 0xc0002300;
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0] = 0x00102902;	//                          
	data_array[1] = 0x050505cb;
	data_array[2] = 0x00050505;
	data_array[3] = 0x00000000;
	data_array[4] = 0x00000000;
	dsi_set_cmdq(&data_array, 5, 1);
	
	data_array[0] = 0xd0002300;
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0] = 0x00102902;	//                          
	data_array[1] = 0x000000cb;
	data_array[2] = 0x05050000;
	data_array[3] = 0x05050505;
	data_array[4] = 0x00000505;
	dsi_set_cmdq(&data_array, 5, 1);
	
	data_array[0] = 0xe0002300;
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0] = 0x000F2902;	//                          
	data_array[1] = 0x000000cb;
	data_array[2] = 0x00000000;
	data_array[3] = 0x00000000;
	data_array[4] = 0x00050500;
	dsi_set_cmdq(&data_array, 5, 1);
	
	data_array[0] = 0xf0002300;
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0] = 0x000c2902;	//                          
	data_array[1] = 0xffffffcb;
	data_array[2] = 0xffffffff;
	data_array[3] = 0xffffffff;
	dsi_set_cmdq(&data_array, 4, 1);
	
	/*                                                       */
	data_array[0] = 0x80002300;
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0] = 0x00102902;	//                         
	data_array[1] = 0x0e0c0acc;
	data_array[2] = 0x00040210;
	data_array[3] = 0x00000000;
	data_array[4] = 0x00000000;
	dsi_set_cmdq(&data_array, 5, 1);
	
	data_array[0] = 0x90002300;
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0] = 0x00102902;	//                         
	data_array[1] = 0x000000cc;
	data_array[2] = 0x2d2e0000;
	data_array[3] = 0x0f0d0b09;
	data_array[4] = 0x00000301;
	dsi_set_cmdq(&data_array, 5, 1);
	
	data_array[0] = 0xa0002300;
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0] = 0x000f2902;	//                         
	data_array[1] = 0x000000cc;
	data_array[2] = 0x00040210;
	data_array[3] = 0x00000000;
	data_array[4] = 0x002d2e00;
	dsi_set_cmdq(&data_array, 5, 1);
	
	data_array[0] = 0xb0002300;
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0] = 0x00102902;	//                         
	data_array[1] = 0x0b0d0fcc;
	data_array[2] = 0x00010309;
	data_array[3] = 0x00000000;
	data_array[4] = 0x00000000;
	dsi_set_cmdq(&data_array, 5, 1);
	
	data_array[0] = 0xc0002300;
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0] = 0x00102902;	//                         
	data_array[1] = 0x000000cc;
	data_array[2] = 0x2e2d0000;
	data_array[3] = 0x0a0c0e10;
	data_array[4] = 0x00000204;
	dsi_set_cmdq(&data_array, 5, 1);
	
	data_array[0] = 0xd0002300;
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0] = 0x000f2902;	//                         
	data_array[1] = 0x000000cc;
	data_array[2] = 0x00000000;
	data_array[3] = 0x00000000;
	data_array[4] = 0x002e2d00;
	dsi_set_cmdq(&data_array, 5, 1);
	
	/*                                                       */
	data_array[0] = 0x80002300;
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0] = 0x000d2902;	//                 
	data_array[1] = 0x18038fce;
	data_array[2] = 0x8d18038e;
	data_array[3] = 0x038c1803;
	data_array[4] = 0x00000018;
	dsi_set_cmdq(&data_array, 5, 1);
	
	data_array[0] = 0x90002300;
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0] = 0x000f2902;	//                  
	data_array[1] = 0x000000ce;
	data_array[2] = 0x00000000;
	data_array[3] = 0x00000000;
	data_array[4] = 0x00000000;
	dsi_set_cmdq(&data_array, 5, 1);
	
	data_array[0] = 0xa0002300;
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0] = 0x000f2902;	//                     
	data_array[1] = 0x050b38ce;
	data_array[2] = 0x00000000;
	data_array[3] = 0x01050a38;
	data_array[4] = 0x00000000;
	dsi_set_cmdq(&data_array, 5, 1);
	
	data_array[0] = 0xb0002300;
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0] = 0x000f2902;	//                     
	data_array[1] = 0x050938ce;
	data_array[2] = 0x00000002;
	data_array[3] = 0x03050838;
	data_array[4] = 0x00000000;
	dsi_set_cmdq(&data_array, 5, 1);
	
	data_array[0] = 0xc0002300;
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0] = 0x000f2902;	//                     
	data_array[1] = 0x050738ce;
	data_array[2] = 0x00000004;
	data_array[3] = 0x05050638;
	data_array[4] = 0x00000000;
	dsi_set_cmdq(&data_array, 5, 1);
	
	data_array[0] = 0xd0002300;
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0] = 0x000f2902;	//                     
	data_array[1] = 0x050538ce;
	data_array[2] = 0x00000006;
	data_array[3] = 0x07050438;
	data_array[4] = 0x00000000;
	dsi_set_cmdq(&data_array, 5, 1);
	
	data_array[0] = 0x80002300;
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0] = 0x000f2902;	//                     
	data_array[1] = 0x000000cf;
	data_array[2] = 0x00000000;
	data_array[3] = 0x00000000;
	data_array[4] = 0x00000000;
	dsi_set_cmdq(&data_array, 5, 1);
	
	data_array[0] = 0x90002300;
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0] = 0x000f2902;	//                     
	data_array[1] = 0x000000cf;
	data_array[2] = 0x00000000;
	data_array[3] = 0x00000000;
	data_array[4] = 0x00000000;
	dsi_set_cmdq(&data_array, 5, 1);
	
	data_array[0] = 0xa0002300;
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0] = 0x000f2902;	//                     
	data_array[1] = 0x000000cf;
	data_array[2] = 0x00000000;
	data_array[3] = 0x00000000;
	data_array[4] = 0x00000000;
	dsi_set_cmdq(&data_array, 5, 1);
	
	data_array[0] = 0xb0002300;
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0] = 0x000f2902;	//                     
	data_array[1] = 0x000000cf;
	data_array[2] = 0x00000000;
	data_array[3] = 0x00000000;
	data_array[4] = 0x00000000;
	dsi_set_cmdq(&data_array, 5, 1);
	
	data_array[0] = 0xc0002300;
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0] = 0x000c2902;	//             
	data_array[1] = 0x200101cf;
	data_array[2] = 0x01000020;
	data_array[3] = 0x08030081;
	dsi_set_cmdq(&data_array, 4, 1);
	
	data_array[0] = 0xb5002300;
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0] = 0x00072902;	//                          
	data_array[1] = 0xfff133c5;
	data_array[2] = 0x00fff133;
	dsi_set_cmdq(&data_array, 3, 1);
	
	data_array[0] = 0x90002300;
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0] = 0x00052902;	//      
	data_array[1] = 0x021102f5;
	data_array[2] = 0x00000011;
	dsi_set_cmdq(&data_array, 3, 1);
	
	data_array[0] = 0x90002300;
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0] = 0x50c52300;	//                             
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0] = 0x94002300;
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0] = 0x66c52300;	//         
	dsi_set_cmdq(&data_array, 1, 1);
	
	/*                                               */
	data_array[0] = 0xb2002300;
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0] = 0x00032902;	//     
	data_array[1] = 0x000000f5;
	dsi_set_cmdq(&data_array, 2, 1);
	
	data_array[0] = 0xb4002300;
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0] = 0x00032902;	//       
	data_array[1] = 0x000000f5;
	dsi_set_cmdq(&data_array, 2, 1);
	
	data_array[0] = 0xb6002300;
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0] = 0x00032902;	//     
	data_array[1] = 0x000000f5;
	dsi_set_cmdq(&data_array, 2, 1);
	
	data_array[0] = 0xb8002300;
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0] = 0x00032902;	//       
	data_array[1] = 0x000000f5;
	dsi_set_cmdq(&data_array, 2, 1);
	
	data_array[0] = 0xb4002300;
	dsi_set_cmdq(&data_array, 1, 1);
	
	data_array[0] = 0xC0C52300;
	dsi_set_cmdq(&data_array, 1, 1);
	
	/*                                            */
	data_array[0] = 0x00002300;
	dsi_set_cmdq(&data_array, 1, 1);

	data_array[0] = 0x00112902;
	data_array[1] = 0x231d0ae1;
	data_array[2] = 0x0a0f040d;
	data_array[3] = 0x0a060309;
	data_array[4] = 0x0d110e05;
	data_array[5] = 0x00000001;
	dsi_set_cmdq(&data_array, 6 ,1);
	
	data_array[0] = 0x00002300;
	dsi_set_cmdq(&data_array, 1, 1);

	data_array[0] = 0x00112902;
	data_array[1] = 0x221d0ae2;
	data_array[2] = 0x0b0e040e;
	data_array[3] = 0x0906020a;
	data_array[4] = 0x0d110e05;
	data_array[5] = 0x00000001;
	dsi_set_cmdq(&data_array, 6 ,1);
	
	data_array[0] = 0x00002300;
	dsi_set_cmdq(&data_array, 1, 1);

	data_array[0] = 0x00042902;
	data_array[1] = 0x000000ff;
	dsi_set_cmdq(&data_array, 2 ,1);
}
static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
	unsigned int i;
	lcm_debug("liyibo : %s %d\n", __func__,__LINE__);
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
		lcm_debug("liyibo : %s %d\n", __func__,__LINE__);
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}


static void lcm_get_params(LCM_PARAMS *params)
{
	  	lcm_debug("liyibo : %s %d\n", __func__,__LINE__);
		memset(params, 0, sizeof(LCM_PARAMS));
	
		params->type   = LCM_TYPE_DSI;

		params->width  = FRAME_WIDTH;
		params->height = FRAME_HEIGHT;

		//                    
		params->dbi.te_mode 				= LCM_DBI_TE_MODE_VSYNC_ONLY;
		params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;

#if (LCM_DSI_CMD_MODE)
		params->dsi.mode   = CMD_MODE;
#else
		params->dsi.mode   = BURST_VDO_MODE;
#endif
	
		//    
		/*                      */
		params->dsi.LANE_NUM				= LCM_FOUR_LANE;
		//                                                                
		params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
		params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
		params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
		params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

		//                                         
		//                      
		params->dsi.packet_size=256;

		//                     
		params->dsi.intermediat_buffer_num = 0;

		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;

		params->dsi.vertical_sync_active				= 4;//  
		params->dsi.vertical_backporch					= 16;//   
		params->dsi.vertical_frontporch					= 16;
		params->dsi.vertical_active_line				= FRAME_HEIGHT; 

		params->dsi.horizontal_sync_active				= 6;//  
		params->dsi.horizontal_backporch				= 44;//        
		params->dsi.horizontal_frontporch				= 44;
		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;

    params->dsi.PLL_CLOCK = 215;//    
		//                     
		params->dsi.pll_div1=0;		//                                                             
		params->dsi.pll_div2=1; 		//                             
		params->dsi.fbk_div =11;    //                                                         

}

static unsigned int lcm_compare_id(void)
{
		unsigned int id = 0;
		unsigned char buffer[2];
		unsigned int array[16];
			lcm_debug("liyibo : %s %d\n", __func__,__LINE__);
			SET_RESET_PIN(1);  //                             
			SET_RESET_PIN(0);
			MDELAY(1);
			SET_RESET_PIN(1);
			MDELAY(150);
	
	//                                                                                                          
	
		array[0] = 0x00023700;//                                       
		dsi_set_cmdq(array, 1, 1);
		read_reg_v2(0xDA, buffer, 1);
	
		id = buffer[0]; //               
      #ifdef BUILD_UBOOT
		printf("%s,  id otm1283A= 0x%08x\n", __func__, id);
	  #endif
		return (LCM_ID_OTM1283 == id)?1:0;


}


static void lcm_init(void)
{
	unsigned int data_array[16];
		lcm_debug("liyibo : %s %d\n", __func__,__LINE__);
	SET_RESET_PIN(1);
    SET_RESET_PIN(0);
	MDELAY(1);//  
    SET_RESET_PIN(1);
	MDELAY(10);//   

	//                     
	//                           
	//                                
	push_table(lcm_initialization_setting,sizeof(lcm_initialization_setting)/sizeof(lcm_initialization_setting[0]),1);
	push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1);
}


static void lcm_suspend(void)
{
		lcm_debug("liyibo : %s %d\n", __func__,__LINE__);
	SET_RESET_PIN(0);	
	MDELAY(1);	
	SET_RESET_PIN(1);

	push_table(lcm_sleep_in_setting, sizeof(lcm_sleep_in_setting) / sizeof(struct LCM_setting_table), 1);
    
	SET_GPIO_OUT(GPIO_LCM_PWR_EN,0);//                 
}



static void lcm_resume(void)
{
	lcm_debug("liyibo : %s %d\n", __func__,__LINE__);
	SET_GPIO_OUT(GPIO_LCM_PWR_EN,1);  //                
	lcm_init();
//                                                                                                        
}


static void lcm_update(unsigned int x, unsigned int y,
                       unsigned int width, unsigned int height)
{
		lcm_debug("liyibo : %s %d\n", __func__,__LINE__);
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
	data_array[3]= 0x00000000;
	data_array[4]= 0x00053902;
	data_array[5]= (y1_MSB<<24)|(y0_LSB<<16)|(y0_MSB<<8)|0x2b;
	data_array[6]= (y1_LSB);
	data_array[7]= 0x00000000;
	data_array[8]= 0x002c3909;

	dsi_set_cmdq(&data_array, 9, 0);

}







LCM_DRIVER otm1283a_6589_hd_dsi = 
{
    .name			= "otm1283a_6589_dsi",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id     = lcm_compare_id,
#if (LCM_DSI_CMD_MODE)
	.set_backlight	= lcm_setbacklight,
    .update         = lcm_update,
#endif
};
