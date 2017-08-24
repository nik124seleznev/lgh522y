#ifndef BUILD_LK
#include <linux/string.h>
#else
#include <string.h>
#endif
#include "lcm_drv.h"

#ifdef BUILD_LK
	#include <platform/mt_gpio.h>
	#include <platform/mt_pmic.h>
	#include <platform/mt_i2c.h>
#elif defined(BUILD_UBOOT)
#else
	#include <mach/mt_gpio.h>
	#include <mach/mt_pm_ldo.h>
#endif

//                                                                            
//                 
//                                                                            

#define FRAME_WIDTH  (1920) //     
#define FRAME_HEIGHT (1080) //      

#define LCM_ID_NT35590 (0x90)
//                                        

#define GPIO_LCD_RST_EN      GPIO_EDP_SYSRSTN_PIN	//       
#define GPIO_LCD_STB_EN      GPIO_EDP_STANDBY_PIN		//        
#define GPIO_LCD_LED_EN      GPIO_LCM_LED_EN 		//       

#define GPIO_LCD_LED_PWM_EN      (GPIO116 | 0x80000000) //       


//                                                                            
//                 
//                                                                            

//                                                                
static LCM_UTIL_FUNCS lcm_util = 
{
	.set_reset_pin = NULL,
	.udelay = NULL,
	.mdelay = NULL,
};


#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))


//                                                                            
//                 
//                                                                            

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	        lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)											lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)   				lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)   


#define   LCM_DSI_CMD_MODE							0

#if 0
static void init_lcm_registers(void)
{
	unsigned int data_array[16];
		
#if 1

	data_array[0] = 0x00011500;  //                    
	dsi_set_cmdq(data_array, 1, 1);
	
	MDELAY(20);
	
	data_array[0]=0x0bae1500;
	data_array[1]=0xeaee1500;
	data_array[2]=0x5fef1500;
	data_array[3]=0x68f21500;	
	data_array[4]=0x00ee1500;
	data_array[5]=0x00ef1500;
	dsi_set_cmdq(&data_array, 6, 1);

#if 0

	data_array[0] = 0x7DB21500;  					 
	dsi_set_cmdq(data_array, 1, 1);

	data_array[0] = 0x0BAE1500;  					 
	dsi_set_cmdq(data_array, 1, 1);
	
	data_array[0] = 0x18B61500;  					 
	dsi_set_cmdq(data_array, 1, 1);
	
	data_array[0] = 0xEAEE1500;  					 
	dsi_set_cmdq(data_array, 1, 1);
	
	data_array[0] = 0x5FEF1500;  					 
	dsi_set_cmdq(data_array, 1, 1);
	
	data_array[0] = 0x68F21500;  					 
	dsi_set_cmdq(data_array, 1, 1);
	
	data_array[0] = 0x00EE1500;  					 
	dsi_set_cmdq(data_array, 1, 1);

	data_array[0] = 0x00EF1500;  					 
	dsi_set_cmdq(data_array, 1, 1);
	
	data_array[0] = 0x64D21500;  					 
	dsi_set_cmdq(data_array, 1, 1);

	data_array[0] = 0x00101500;  //                                 
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(20);
#endif

	//                                                  
	//                               
	//          

	//                                                                 
	//                               
#endif

#if 0
	data_array[0] = 0x00010500;  //                    
	dsi_set_cmdq(data_array, 1, 1);
	
	MDELAY(20);

	data_array[0] = 0x00023902; 
	data_array[1] = 0x00000BAE; 					 
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902; 
	data_array[1] = 0x0000EAEE; 					 
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902; 
	data_array[1] = 0x00005FEF; 					 
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902; 
	data_array[1] = 0x000068F2; 					 
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902; 
	data_array[1] = 0x000000EE; 					 
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902; 
	data_array[1] = 0x000000EF; 					 
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00100500;  //                                 
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(20);

	data_array[0] = 0x00290500;  //                                  
	dsi_set_cmdq(data_array, 1, 1);
#endif
	//          
}
#endif

//                                                                            
//                            
//                                                                            

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}

#if 1
#ifdef BUILD_LK

#define IT6151_BUSNUM                I2C3
#define it6151_SLAVE_ADDR/*               */       0x58  //                       


U32 it6151_reg_i2c_write_byte (U8 dev_addr,U8  cmd, U8 data)
{
    U8 cmdBufferLen = 1;
    U8 dataBufferLen = 1;	
    U32 ret_code = I2C_OK;
    U8 write_data[I2C_FIFO_SIZE];
    int transfer_len = 0;
    int i=0, cmdIndex=0, dataIndex=0;
	
    dev_addr = dev_addr<<1;  //          
    
    transfer_len = cmdBufferLen + dataBufferLen;
    if(I2C_FIFO_SIZE < (cmdBufferLen + dataBufferLen))
    {
        dprintf(CRITICAL, "[it6151_i2c_write] exceed I2C FIFO length!! \n");
        return 0;
    }

    //                    
    //                          

    while(cmdIndex < cmdBufferLen)
    {
        write_data[i] = cmd;
        cmdIndex++;
        i++;
    }

    while(dataIndex < dataBufferLen)
    {
        write_data[i] = data;
        dataIndex++;
        i++;
    }


    /*                           */
    for( i=0 ; i < transfer_len ; i++ )
    {
        dprintf(CRITICAL, "[it6151_i2c_write] write_data[%d]=%x\n", i, write_data[i]);
    }

    ret_code = mt_i2c_write(IT6151_BUSNUM, dev_addr, write_data, transfer_len,0);


    return ret_code;
}


static U32 it6151_reg_i2c_read_byte (U8 dev_addr,U8  *cmdBuffer, U8 *dataBuffer)
{
    U32 ret_code = I2C_OK;
    dev_addr = (dev_addr<<1) | 1;  //         
    ret_code = mt_i2c_write(IT6151_BUSNUM, dev_addr, cmdBuffer, 1, 0);    //                     
    if (ret_code != I2C_OK)
        return ret_code;

    ret_code = mt_i2c_read(IT6151_BUSNUM, dev_addr, dataBuffer, 1, 0);

    if (ret_code != I2C_OK)
        return ret_code;	

    //                                       

    return ret_code;
}

 //   
 
 /*                                                                             
                                                      
                                                                              */
#else
extern UINT8 it6151_reg_i2c_read_byte(U8 dev_addr,U8  *cmdBuffer, U8 *dataBuffer);
extern void it6151_reg_i2c_write_byte(U8 dev_addr,U8  cmd, U8 data);
#endif
#endif

//                                                                   
//                                                                                          
//                                                                   


//                                       
//                                            
//                                  
//                                                               
//                                        
#define PANEL_RESOLUTION_1920x1080p60





//                                                             

//                           
//                             


//                                                             
#define MIPI_4_LANE 	(3)
#define MIPI_3_LANE 	(2)
#define MIPI_2_LANE 	(1)
#define MIPI_1_LANE		(0)

//                         
#define RGB_24b         (0x3E)
#define RGB_30b         (0x0D)
#define RGB_36b         (0x1D)
#define RGB_18b_P       (0x1E)
#define RGB_18b_L       (0x2E)
#define YCbCr_16b       (0x2C)
#define YCbCr_20b       (0x0C)
#define YCbCr_24b       (0x1C)


//                
#define B_DPTXIN_6Bpp   (0)
#define B_DPTXIN_8Bpp   (1)
#define B_DPTXIN_10Bpp  (2)
#define B_DPTXIN_12Bpp  (3)

#define B_LBR    		(1)
#define B_HBR    		(0)

#define B_4_LANE 		(3)
#define B_2_LANE 		(1)
#define B_1_LANE 		(0)

#define B_SSC_ENABLE   	(1)
#define B_SSC_DISABLE   (0)

//                                                                         
//         
//                                                                         
#define TRAINING_BITRATE	(B_HBR)
#define DPTX_SSC_SETTING	(B_SSC_DISABLE)
#define MP_MCLK_INV			(0)
#define MP_CONTINUOUS_CLK	(1)
#define MP_LANE_DESKEW		(1)
#define MP_PCLK_DIV			(2)
#define MP_LANE_SWAP		(0)
#define MP_PN_SWAP			(0)

#define DP_PN_SWAP			(0)
#define DP_AUX_PN_SWAP		(0)
#define DP_LANE_SWAP		(0)

#define LVDS_LANE_SWAP		(0)
#define LVDS_PN_SWAP		(0)
#define LVDS_DC_BALANCE		(0)

#define LVDS_6BIT			(0) //                             
#define VESA_MAP		    (1) //                                 

#define INT_MASK			(7)
//                                                                     
//               
//                                                                     
#ifdef PANEL_RESOLUTION_1280x800_NOUFO
#define PANEL_WIDTH 1280
#define VIC 0
#define MP_HPOL 0
#define MP_VPOL 1
#define DPTX_LANE_COUNT  B_2_LANE
#define MIPI_LANE_COUNT  MIPI_4_LANE
#define EN_UFO 0
#define MIPI_PACKED_FMT		RGB_24b
#define MP_H_RESYNC			1
#define MP_V_RESYNC			0
#endif

#ifdef PANEL_RESOLUTION_1920x1080p60
#define PANEL_WIDTH 1920
#define VIC 0x10
#define MP_HPOL 1
#define MP_VPOL 1
#define DPTX_LANE_COUNT  B_2_LANE
#define MIPI_LANE_COUNT  MIPI_4_LANE
#define EN_UFO 0
#define MIPI_PACKED_FMT		RGB_24b
#define MP_H_RESYNC			1
#define MP_V_RESYNC			0
#endif

#ifdef PANEL_RESOLUTION_1920x1200p60RB
#define PANEL_WIDTH 1920
#define VIC 0 //                                                              
#define MP_HPOL 1
#define MP_VPOL 0
#define DPTX_LANE_COUNT  B_2_LANE
#define MIPI_LANE_COUNT  MIPI_4_LANE
#define EN_UFO 0
#define MIPI_PACKED_FMT		RGB_24b
#define MP_H_RESYNC			1
#define MP_V_RESYNC			0
#endif

#ifdef PANEL_RESOLUTION_2048x1536
#define PANEL_WIDTH 2048
#define VIC 0 //                                                              
#define MP_HPOL 0
#define MP_VPOL 1
#define MIPI_LANE_COUNT  MIPI_4_LANE
#define DPTX_LANE_COUNT  B_4_LANE
#define EN_UFO 1
#define MIPI_PACKED_FMT		RGB_24b
#define MP_H_RESYNC			0
#define MP_V_RESYNC			0
#endif

#ifdef PANEL_RESOLUTION_2048x1536_NOUFO
#define PANEL_WIDTH 2048
#define VIC 0 //                                                              
#define MP_HPOL 0
#define MP_VPOL 1
#define MIPI_LANE_COUNT  MIPI_4_LANE
#define DPTX_LANE_COUNT  B_4_LANE
#define EN_UFO 0
#define MIPI_PACKED_FMT		RGB_24b
#define MP_H_RESYNC			1
#define MP_V_RESYNC			0
#endif

#ifdef PANEL_RESOLUTION_2048x1536_NOUFO_18B
#define PANEL_WIDTH 2048
#define VIC 0 //                                                              
#define MP_HPOL 0
#define MP_VPOL 1
#define MIPI_LANE_COUNT  MIPI_4_LANE
#define DPTX_LANE_COUNT  B_4_LANE
#define EN_UFO 0
#define MIPI_PACKED_FMT		RGB_18b_P
#define MP_H_RESYNC			1
#define MP_V_RESYNC			0
#endif
//                                                                         
//               
//                                                                         

#ifndef MIPI_LANE_COUNT
    #define MIPI_LANE_COUNT  MIPI_4_LANE
#endif
#ifndef PN_SWAP
    #define PN_SWAP 0
#endif

//                                                                         

#ifndef DPTX_LANE_COUNT
    //                      
    #define DPTX_LANE_COUNT  B_4_LANE
    //                                  
    //                                  
#endif


//                                                                         

#define DP_I2C_ADDR 0x5C
#define MIPI_I2C_ADDR 0x6C

//                                                                   
//                                                                                  
//                                                                   
//                                                       
//         
//                                                       
//                                                                           
void IT6151_DPTX_init(void)
{	
#ifndef BUILD_LK
		printk("\IT6151_DPTX_init !!!\n");
#else
		dprintf(INFO, "[LK/LCM] IT6151_DPTX_init\n");
#endif	
	it6151_reg_i2c_write_byte(DP_I2C_ADDR,0x05,0x29);
	it6151_reg_i2c_write_byte(DP_I2C_ADDR,0x05,0x00);
	
	it6151_reg_i2c_write_byte(DP_I2C_ADDR,0x09,INT_MASK);//                                 
	it6151_reg_i2c_write_byte(DP_I2C_ADDR,0x0A,0x00);
	it6151_reg_i2c_write_byte(DP_I2C_ADDR,0x0B,0x00);
	it6151_reg_i2c_write_byte(DP_I2C_ADDR,0xC5,0xC1);
	it6151_reg_i2c_write_byte(DP_I2C_ADDR,0xB5,0x00);
	it6151_reg_i2c_write_byte(DP_I2C_ADDR,0xB7,0x80);
	it6151_reg_i2c_write_byte(DP_I2C_ADDR,0xC4,0xF0);
	it6151_reg_i2c_write_byte(DP_I2C_ADDR,0x06,0xFF);//                    
	it6151_reg_i2c_write_byte(DP_I2C_ADDR,0x07,0xFF);//                    
	it6151_reg_i2c_write_byte(DP_I2C_ADDR,0x08,0xFF);//                    
	
	it6151_reg_i2c_write_byte(DP_I2C_ADDR,0x05,0x00);
	it6151_reg_i2c_write_byte(DP_I2C_ADDR,0x0c,0x08);
	it6151_reg_i2c_write_byte(DP_I2C_ADDR,0x21,0x05);
	it6151_reg_i2c_write_byte(DP_I2C_ADDR,0x3a,0x04);
	it6151_reg_i2c_write_byte(DP_I2C_ADDR,0x5f,0x06);
	//                                 
	it6151_reg_i2c_write_byte(DP_I2C_ADDR,0xc9,0xf5);
	it6151_reg_i2c_write_byte(DP_I2C_ADDR,0xca,0x4c);
	it6151_reg_i2c_write_byte(DP_I2C_ADDR,0xcb,0x37);
	it6151_reg_i2c_write_byte(DP_I2C_ADDR,0xd3,0x03);
	it6151_reg_i2c_write_byte(DP_I2C_ADDR,0xd4,0x60);
	it6151_reg_i2c_write_byte(DP_I2C_ADDR,0xe8,0x11);
	it6151_reg_i2c_write_byte(DP_I2C_ADDR,0xec,VIC);

	MDELAY(5);			

	it6151_reg_i2c_write_byte(DP_I2C_ADDR,0x23,0x42);
	it6151_reg_i2c_write_byte(DP_I2C_ADDR,0x24,0x07);
	it6151_reg_i2c_write_byte(DP_I2C_ADDR,0x25,0x01);
	it6151_reg_i2c_write_byte(DP_I2C_ADDR,0x26,0x00);
	it6151_reg_i2c_write_byte(DP_I2C_ADDR,0x27,0x10);
	it6151_reg_i2c_write_byte(DP_I2C_ADDR,0x2B,0x05);
	it6151_reg_i2c_write_byte(DP_I2C_ADDR,0x23,0x40);
	it6151_reg_i2c_write_byte(DP_I2C_ADDR,0x22,(DP_AUX_PN_SWAP<<3)|(DP_PN_SWAP<<2)|0x03);
	it6151_reg_i2c_write_byte(DP_I2C_ADDR,0x16,(DPTX_SSC_SETTING<<4)|(DP_LANE_SWAP<<3)|(DPTX_LANE_COUNT<<1)|TRAINING_BITRATE);
	it6151_reg_i2c_write_byte(DP_I2C_ADDR,0x0f,0x01);
	it6151_reg_i2c_write_byte(DP_I2C_ADDR,0x76,0xa7);
	it6151_reg_i2c_write_byte(DP_I2C_ADDR,0x77,0xaf);
	it6151_reg_i2c_write_byte(DP_I2C_ADDR,0x7e,0x8f);
	it6151_reg_i2c_write_byte(DP_I2C_ADDR,0x7f,0x07);
	it6151_reg_i2c_write_byte(DP_I2C_ADDR,0x80,0xef);
	it6151_reg_i2c_write_byte(DP_I2C_ADDR,0x81,0x5f);
	it6151_reg_i2c_write_byte(DP_I2C_ADDR,0x82,0xef);
	it6151_reg_i2c_write_byte(DP_I2C_ADDR,0x83,0x07);
	it6151_reg_i2c_write_byte(DP_I2C_ADDR,0x88,0x38);
	it6151_reg_i2c_write_byte(DP_I2C_ADDR,0x89,0x1f);
	it6151_reg_i2c_write_byte(DP_I2C_ADDR,0x8a,0x48);
	it6151_reg_i2c_write_byte(DP_I2C_ADDR,0x0f,0x00);
	it6151_reg_i2c_write_byte(DP_I2C_ADDR,0x5c,0xf3);
	it6151_reg_i2c_write_byte(DP_I2C_ADDR,0x17,0x04);
	it6151_reg_i2c_write_byte(DP_I2C_ADDR,0x17,0x01);
	MDELAY(5);	
}

int	vit6151_init(void)
{

	unsigned char VenID[2], DevID[2], RevID;
  	unsigned char cmdBuffer;

#ifdef BUILD_LK	
	dprintf(INFO, "[LK/LCM] vit6151_init\n");
#else
	printk("vit6151_init\n");
#endif

//       
//                                                    
#ifdef BUILD_LK
//       


#ifdef BUILD_LK
	  cmdBuffer = 0x00;
	  it6151_reg_i2c_read_byte(DP_I2C_ADDR, &cmdBuffer, &VenID[0]);
	  cmdBuffer = 0x01;
	  it6151_reg_i2c_read_byte(DP_I2C_ADDR, &cmdBuffer, &VenID[1]);
	  cmdBuffer = 0x02;
	  it6151_reg_i2c_read_byte(DP_I2C_ADDR, &cmdBuffer, &DevID[0]);
	  cmdBuffer = 0x03;
	  it6151_reg_i2c_read_byte(DP_I2C_ADDR, &cmdBuffer, &DevID[1]);

 	  cmdBuffer = 0x04;
 	  it6151_reg_i2c_read_byte(DP_I2C_ADDR, &cmdBuffer, &RevID);	  
#else
	  cmdBuffer = 0x00;
	  VenID[0] = it6151_reg_i2c_read_byte(DP_I2C_ADDR, &cmdBuffer, &VenID[0]);
	  cmdBuffer = 0x01;
	  VenID[1] = it6151_reg_i2c_read_byte(DP_I2C_ADDR, &cmdBuffer, &VenID[1]);
	  cmdBuffer = 0x02;
	  DevID[0] = it6151_reg_i2c_read_byte(DP_I2C_ADDR, &cmdBuffer, &DevID[0]);
	  cmdBuffer = 0x03;
	  DevID[1] = it6151_reg_i2c_read_byte(DP_I2C_ADDR, &cmdBuffer, &DevID[1]);
 	  cmdBuffer = 0x04;
 	  RevID = it6151_reg_i2c_read_byte(DP_I2C_ADDR, &cmdBuffer, &RevID);		  	  
#endif

#ifndef BUILD_LK	
	printk("Current DPDevID=%02X%02X\n", DevID[1], DevID[0]);
	printk("Current DPVenID=%02X%02X\n", VenID[1], VenID[0]);
	printk("Current DPRevID=%02X\n\n", RevID);	
#endif
 		
	if( VenID[0]==0x54 && VenID[1]==0x49 && DevID[0]==0x51 && DevID[1]==0x61 ){

#ifndef BUILD_LK	
	printk(" Test 1 DP_I2C_ADDR=0x%x, MIPI_I2C_ADDR=0x%x\n", DP_I2C_ADDR, MIPI_I2C_ADDR);
#else
	dprintf(INFO, "[LK/LCM] Test 1 DP_I2C_ADDR=0x%x, MIPI_I2C_ADDR=0x%x\n", DP_I2C_ADDR, MIPI_I2C_ADDR);
#endif
		it6151_reg_i2c_write_byte(DP_I2C_ADDR,0x05,0x04);//            
		it6151_reg_i2c_write_byte(DP_I2C_ADDR,0xfd,(MIPI_I2C_ADDR<<1)|1);
		it6151_reg_i2c_write_byte(MIPI_I2C_ADDR,0x05,0x00);
		it6151_reg_i2c_write_byte(MIPI_I2C_ADDR,0x0c,(MP_LANE_SWAP<<7)|(MP_PN_SWAP<<6)|(MIPI_LANE_COUNT<<4)|EN_UFO);
		it6151_reg_i2c_write_byte(MIPI_I2C_ADDR,0x11,MP_MCLK_INV);
		//                               
        if(RevID == 0xA1){			
		    it6151_reg_i2c_write_byte(MIPI_I2C_ADDR,0x19, MP_LANE_DESKEW); 
		}else{
		    it6151_reg_i2c_write_byte(MIPI_I2C_ADDR,0x19,(MP_CONTINUOUS_CLK<<1) | MP_LANE_DESKEW); 
		}
		
		//                                                   
		
		it6151_reg_i2c_write_byte(MIPI_I2C_ADDR,0x27, MIPI_PACKED_FMT);
		it6151_reg_i2c_write_byte(MIPI_I2C_ADDR,0x28,((PANEL_WIDTH/4-1)>>2)&0xC0);
		it6151_reg_i2c_write_byte(MIPI_I2C_ADDR,0x29,(PANEL_WIDTH/4-1)&0xFF);
		
		it6151_reg_i2c_write_byte(MIPI_I2C_ADDR,0x2e,0x34);
		it6151_reg_i2c_write_byte(MIPI_I2C_ADDR,0x2f,0x01);
		
		
		it6151_reg_i2c_write_byte(MIPI_I2C_ADDR,0x4e,(MP_V_RESYNC<<3)|(MP_H_RESYNC<<2)|(MP_VPOL<<1)|(MP_HPOL));
		it6151_reg_i2c_write_byte(MIPI_I2C_ADDR,0x80,(EN_UFO<<5)|MP_PCLK_DIV);
		it6151_reg_i2c_write_byte(MIPI_I2C_ADDR,0x84,0x8f);

		IT6151_DPTX_init();

		return 0;
	}

#ifndef BUILD_LK	
	printk(" Test 2 DP_I2C_ADDR=0x%x, MIPI_I2C_ADDR=0x%x\n", DP_I2C_ADDR, MIPI_I2C_ADDR);
#endif

#ifdef BUILD_LK
	  cmdBuffer = 0x00;
	  it6151_reg_i2c_read_byte(MIPI_I2C_ADDR, &cmdBuffer, &VenID[0]);
	  cmdBuffer = 0x01;
	  it6151_reg_i2c_read_byte(MIPI_I2C_ADDR, &cmdBuffer, &VenID[1]);
	  cmdBuffer = 0x02;
	  it6151_reg_i2c_read_byte(MIPI_I2C_ADDR, &cmdBuffer, &DevID[0]);
	  cmdBuffer = 0x03;
	  it6151_reg_i2c_read_byte(MIPI_I2C_ADDR, &cmdBuffer, &DevID[1]);      		

	  cmdBuffer = 0x04;
	  it6151_reg_i2c_read_byte(MIPI_I2C_ADDR, &cmdBuffer, &RevID);
#else
	  cmdBuffer = 0x00;
	  VenID[0] = it6151_reg_i2c_read_byte(MIPI_I2C_ADDR, &cmdBuffer, &VenID[0]);
	  cmdBuffer = 0x01;
	  VenID[1] = it6151_reg_i2c_read_byte(MIPI_I2C_ADDR, &cmdBuffer, &VenID[1]);
	  cmdBuffer = 0x02;
	  DevID[0] = it6151_reg_i2c_read_byte(MIPI_I2C_ADDR, &cmdBuffer, &DevID[0]);
	  cmdBuffer = 0x03;
	  DevID[1] = it6151_reg_i2c_read_byte(MIPI_I2C_ADDR, &cmdBuffer, &DevID[1]);      		

	  cmdBuffer = 0x04;
	  RevID = it6151_reg_i2c_read_byte(MIPI_I2C_ADDR, &cmdBuffer, &RevID);

#endif

#ifndef BUILD_LK
	printk("Current MPDevID=%02X%02X\n", DevID[1], DevID[0]);
	printk("Current MPVenID=%02X%02X\n", VenID[1], VenID[0]);
	printk("Current MPRevID=%02X\n\n", RevID);
#endif

	if(VenID[0]==0x54 && VenID[1]==0x49 && DevID[0]==0x21 && DevID[1]==0x61 ){
		    it6151_reg_i2c_write_byte(MIPI_I2C_ADDR,0x05,0x33);
		    it6151_reg_i2c_write_byte(MIPI_I2C_ADDR,0x05,0x40);
		    it6151_reg_i2c_write_byte(MIPI_I2C_ADDR,0x05,0x00);
		    it6151_reg_i2c_write_byte(MIPI_I2C_ADDR,0x0c,(MP_LANE_SWAP<<7)|(MP_PN_SWAP<<6)|(MIPI_LANE_COUNT<<4));
		    it6151_reg_i2c_write_byte(MIPI_I2C_ADDR,0x11, MP_MCLK_INV); 
		    it6151_reg_i2c_write_byte(MIPI_I2C_ADDR,0x19,(MP_CONTINUOUS_CLK<<1) | MP_LANE_DESKEW);  
		    it6151_reg_i2c_write_byte(MIPI_I2C_ADDR,0x4E,(MP_V_RESYNC<<3)|(MP_H_RESYNC<<2)|(MP_VPOL<<1)|(MP_HPOL));      
		    it6151_reg_i2c_write_byte(MIPI_I2C_ADDR,0x72,0x01); 
		    it6151_reg_i2c_write_byte(MIPI_I2C_ADDR,0x73,0x03); 
		    it6151_reg_i2c_write_byte(MIPI_I2C_ADDR,0x80,MP_PCLK_DIV); 
		    it6151_reg_i2c_write_byte(MIPI_I2C_ADDR,0xC0,0x13);   
		    it6151_reg_i2c_write_byte(MIPI_I2C_ADDR,0xC1,0x01);  
		    it6151_reg_i2c_write_byte(MIPI_I2C_ADDR,0xC2,0x47);  
		    it6151_reg_i2c_write_byte(MIPI_I2C_ADDR,0xC3,0x67);  
		    it6151_reg_i2c_write_byte(MIPI_I2C_ADDR,0xC4,0x04);  
		    it6151_reg_i2c_write_byte(MIPI_I2C_ADDR,0xCB,(LVDS_PN_SWAP<<5)|(LVDS_LANE_SWAP<<4)|(LVDS_6BIT<<2)|(LVDS_DC_BALANCE<<1)| VESA_MAP);  
			return 1;
	}	
	return -1;
//       
#endif //
//        
}

static unsigned int IT6151_ESD_Check(void)
{
#ifndef BUILD_LK
	static 	unsigned char ucIsIT6151=0xFF;
	unsigned char ucReg, ucStat;
  	unsigned char cmdBuffer;

	if(ucIsIT6151==0xFF){
		unsigned char VenID[2], DevID[2];

#ifndef BUILD_LK
		printk("\nIT6151 1st IRQ !!!\n");
#endif		
    	cmdBuffer = 0x00;
    	VenID[0] = it6151_reg_i2c_read_byte(DP_I2C_ADDR, &cmdBuffer, &VenID[0]);    
    	cmdBuffer = 0x01;
    	VenID[1] = it6151_reg_i2c_read_byte(DP_I2C_ADDR, &cmdBuffer, &VenID[1]); 
    	cmdBuffer = 0x02;
   	DevID[0] = it6151_reg_i2c_read_byte(DP_I2C_ADDR, &cmdBuffer, &DevID[0]); 
    	cmdBuffer = 0x03;
    	DevID[1] = it6151_reg_i2c_read_byte(DP_I2C_ADDR, &cmdBuffer, &DevID[1]);             
		//                                                        
		//                                                        
		//                                                        
		//                                                        
	
		//                                                                   
		//                                                                   
#ifndef BUILD_LK
		printk("Current DevID=%02X%02X\n", DevID[1], DevID[0]);
		printk("Current VenID=%02X%02X\n", VenID[1], VenID[0]);
#endif
					
		if( VenID[0]==0x54 && VenID[1]==0x49 && DevID[0]==0x51 && DevID[1]==0x61){
				ucIsIT6151 = 1;
		}else{
				ucIsIT6151 = 0;
		}	
	}
	if(ucIsIT6151==1){
		cmdBuffer = 0x21;
   		ucReg = it6151_reg_i2c_read_byte(DP_I2C_ADDR, &cmdBuffer, &ucReg);  
		//                                                    
		if(ucReg & 0x02){
			//                                                         
#ifndef BUILD_LK			
			printk("\nIT6151 Reg0x21=0x%x !!!\n", ucReg);
#endif			
			it6151_reg_i2c_write_byte(DP_I2C_ADDR, 0x21, ucReg);
		}

		cmdBuffer = 0x06;
    		ucReg = it6151_reg_i2c_read_byte(DP_I2C_ADDR, &cmdBuffer, &ucReg); 
		//                                                    
		if(ucReg){
			//                                                         
#ifndef BUILD_LK			
			printk("\nIT6151 Reg0x06=0x%x !!!\n", ucReg);
#endif			
			it6151_reg_i2c_write_byte(DP_I2C_ADDR, 0x06, ucReg);

		  	cmdBuffer = 0x0D;
      			ucStat = it6151_reg_i2c_read_byte(DP_I2C_ADDR, &cmdBuffer, &ucStat); 
			//                                                     
			//                                                          
#ifndef BUILD_LK			
			printk("\nIT6151 Reg0x0D=0x%x !!!\n", ucStat);			
#endif			
			if(ucReg & 0x03){
				if(ucStat & 0x02){
//                        
					return TRUE;
				}
			}else if(ucReg & 0x04){
				if(ucStat & 0x04){
//                        
					return TRUE;
				}
			}
		}
	}
	return FALSE;
#endif	
}

static void IT6151_ESD_Recover(void)
{
	unsigned char ucReg, ucStat;
  	unsigned char cmdBuffer;

#ifndef BUILD_LK		
	printk("\nIT6151_ESD_Recover\n");
#endif

	cmdBuffer = 0x06;
    	ucStat = it6151_reg_i2c_read_byte(DP_I2C_ADDR, &cmdBuffer, &ucStat); 	

	IT6151_DPTX_init();
}



static void lcm_get_params(LCM_PARAMS *params)
{

		memset(params, 0, sizeof(LCM_PARAMS));
	
		params->type   = LCM_TYPE_DSI;

		params->width  = FRAME_WIDTH;
		params->height = FRAME_HEIGHT;

#if 0
		//                    
		params->dbi.te_mode 				= LCM_DBI_TE_MODE_VSYNC_ONLY;
		params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;
#endif

        #if (LCM_DSI_CMD_MODE)
		params->dsi.mode   = CMD_MODE;
        #else
		params->dsi.mode   = SYNC_PULSE_VDO_MODE; //                                          
        #endif
	
		//    
		/*                      */
		//                         
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
		params->dsi.intermediat_buffer_num = 0;//                                                                                                          

		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
		params->dsi.word_count=1920*3; //       

		params->dsi.ufoe_enable = 0;
			
		params->dsi.vertical_sync_active				= 5;//                             
		params->dsi.vertical_backporch					= 4;//                            
		params->dsi.vertical_frontporch					= 36;//                         
		params->dsi.vertical_active_line				= FRAME_HEIGHT; 

		params->dsi.horizontal_sync_active				= 44;//                                  
		params->dsi.horizontal_backporch				= 88;//                               
		params->dsi.horizontal_frontporch				= 148;//                                
		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;

		//                     
		//                  
		//                                                                                         
		//                                                           
		//                                                                                              
		//                       
		//                        
              params->dsi.PLL_CLOCK = LCM_DSI_6589_PLL_CLOCK_442;//                                                            
    
		//                                 
		//                                

		params->dsi.cont_clock = 1;
		//                                  
		//                                                      

	       params->dsi.HS_TRAIL = 0x8; //   
	       params->dsi.HS_ZERO = 0xA; //   
	       params->dsi.HS_PRPR = 0x6; //   
	       params->dsi.LPX = 0x5; //   

	       params->dsi.DA_HS_EXIT = 0x6; //   
		
}
//                                                 
static void lcm_init(void)
{
	//                            
#ifdef BUILD_LK
		dprintf(INFO, "[LK/LCM] lcm_init() enter\n");

    	dprintf(INFO, "[LK/LCM] enable VGP3 1.2V\n");
	//         
	pmic_config_interface(0x41E, 0x1, 0x1, 15); 
	pmic_config_interface(0x43a, 0x0, 0x7, 5);
	
	//                                                
	//                                               
	//                                               
	#if 0
	mt_set_gpio_mode(GPIO_LCD_LED_PWM_EN, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO_LCD_LED_PWM_EN, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_LCD_LED_PWM_EN, GPIO_OUT_ONE);
	#endif
	MDELAY(20);
	
	//                                                
	//                                               
	//                                               
	mt_set_gpio_mode(GPIO_LCD_LED_EN, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO_LCD_LED_EN, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_LCD_LED_EN, GPIO_OUT_ONE);
	MDELAY(20);
#endif

	MDELAY(50); 
	
//  
#if 1
	SET_RESET_PIN(1);
	SET_RESET_PIN(0);
	MDELAY(1);
	SET_RESET_PIN(1);
#else

	#ifdef BUILD_LK
		dprintf(INFO, "[LK/LCM] eDP RST pin...\n");
	#else
		printk("[IT6151] eDP RST pin...\n");
	#endif
	mt_set_gpio_mode(GPIO_LCD_RST_EN, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO_LCD_RST_EN, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_LCD_RST_EN, GPIO_OUT_ONE);

	mt_set_gpio_out(GPIO_LCD_RST_EN, GPIO_OUT_ZERO);
	MDELAY(1);
	mt_set_gpio_out(GPIO_LCD_RST_EN, GPIO_OUT_ONE);	
	
#endif
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

#if 1//             
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
//                        
//                        
//                        
//                           


//                                 
//                                                                  


//                                                                  
#endif

/*
                     
                       
                       
                       
                       
                     
                     
                     
                     
                                            

                      
                     
                     
                     
                       
                       
                        

                              
                            
                      

                  
                             

                     
                            
                                     

*/

vit6151_init();


}



static void lcm_suspend(void)
{
	//                            

#ifdef BUILD_LK
	dprintf(INFO, "[LK/LCM] lcm_suspend() enter\n");
#else
	printk( "lcm_suspend() enter\n");
#endif
	//                                        
	//                               
	#if 0
	mt_set_gpio_mode(GPIO_LCD_LED_PWM_EN, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO_LCD_LED_PWM_EN, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_LCD_LED_PWM_EN, GPIO_OUT_ZERO);
	#endif
	MDELAY(20);
	//
	mt_set_gpio_mode(GPIO_LCD_LED_EN, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO_LCD_LED_EN, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_LCD_LED_EN, GPIO_OUT_ZERO);
	MDELAY(20);
	
	//                                       
	//                               

//             

//            
	//                   
//              
	//                 
}


static void lcm_resume(void)
{
#ifdef BUILD_LK
	dprintf(INFO, "[LK/LCM] lcm_resume() enter\n");
	//         
	//         
	pmic_config_interface(0x41E, 0x1, 0x1, 15); 
	pmic_config_interface(0x43a, 0x0, 0x7, 5);	
		
	//                                                
	//                                               
	//                                               
	#if 0
	mt_set_gpio_mode(GPIO_LCD_LED_PWM_EN, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO_LCD_LED_PWM_EN, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_LCD_LED_PWM_EN, GPIO_OUT_ONE);
	#endif
	MDELAY(20);
	MDELAY(20);
		
	//                                                
	//                                               
	//                                               
	mt_set_gpio_mode(GPIO_LCD_LED_EN, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO_LCD_LED_EN, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_LCD_LED_EN, GPIO_OUT_ONE);
	MDELAY(20);
#else
	printk( "lcm_resume() enter\n");
#endif

	//                            
	lcm_init();

	//                                        
	//                               

	//                                         
	//                               
	//            

}
         
#if (LCM_DSI_CMD_MODE)
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
	dsi_set_cmdq(data_array, 3, 1);
	
	data_array[0]= 0x00053902;
	data_array[1]= (y1_MSB<<24)|(y0_LSB<<16)|(y0_MSB<<8)|0x2b;
	data_array[2]= (y1_LSB);
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0]= 0x00290508; //                                  
	dsi_set_cmdq(data_array, 1, 1);
	
	data_array[0]= 0x002c3909;
	dsi_set_cmdq(data_array, 1, 0);

}
#endif
#if 0
static unsigned int lcm_compare_id(void)
{
	unsigned int id=0;
	unsigned char buffer[2];
	unsigned int array[16];  

	SET_RESET_PIN(1);
	SET_RESET_PIN(0);
	MDELAY(1);
	
	SET_RESET_PIN(1);
	MDELAY(20); 

	array[0] = 0x00023700;//                                       
	dsi_set_cmdq(array, 1, 1);
	
	read_reg_v2(0xF4, buffer, 2);
	id = buffer[0]; //               
    #ifdef BUILD_LK
		dprintf(INFO, "%s, LK nt35590 debug: nt35590 id = 0x%08x\n", __func__, id);
    #else
		printk("%s, kernel nt35590 horse debug: nt35590 id = 0x%08x\n", __func__, id);
    #endif

    if(id == LCM_ID_NT35590)
    	return 1;
    else
        return 0;


}
#endif
static unsigned int	lcm_esd_check(void)
{

#ifndef BUILD_LK
	printk("lcm_esd_check\n\n");
#endif
	return IT6151_ESD_Check();

}

static unsigned int	lcm_esd_recover(void)
{
#ifndef BUILD_LK
		printk("lcm_esd_recover\n\n");
#endif
	IT6151_ESD_Recover();
	return 0;
}

LCM_DRIVER it6151_fhd_edp_dsi_video_auo_lcm_drv = 
{
    .name			= "it6151_fhd_edp_dsi_video_auo",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	//                                 
#if (LCM_DSI_CMD_MODE)
    .update         = lcm_update,
#endif
//                               
//                                  
    };
