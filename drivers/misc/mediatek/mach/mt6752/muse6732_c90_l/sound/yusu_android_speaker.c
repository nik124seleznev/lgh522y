/*                                                                            
                                                         
                                                                              
*/
#include <asm/uaccess.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include "yusu_android_speaker.h"
#include <mach/mt_gpio.h>

/*                                                                            
                                     
                                                                              
*/
#if 0
static bool eamp_log_on = false;

#define EAMP_PRINTK(fmt, arg...) \
	do { \
		if (eamp_log_on) xlog_printk(ANDROID_LOG_INFO,"EAMP", "[EAMP]: %s() "fmt"\n", __func__,##arg); \
	}while (0)
#endif

/*                                                                            
                       
                                                                              
*/

//               
#define EAMP_SLAVE_ADDR_WRITE	0xE0
#define EAMP_SLAVE_ADDR_READ	0xE1
#define EAMP_I2C_CHANNEL     	(1)        //             
#define EAMP_I2C_DEVNAME "TPA2058D3"
#define SOUND_I2C_CHANNEL 1
#define USE_ANALOG_SWITCH 1

//                
#define EAMP_REG_SUBSYSTEMCONTROL  			0x00
#define EAMP_REG_INPUTCONTROL				0x01
#define EAMP_REG_LIMITER_CONTROL			0x02
#define EAMP_REG_SPEAKER_OUTPUT_CONTROL		0x03
#define EAMP_REG_HEADPHONE_OUTPUT_CONTROL	0x04
#define EAMP_REG_SPEAKER_VOLUME				0x05
#define EAMP_REG_HEADPHONE_LEFT_VOLUME		0x06
#define EAMP_REG_HEADPHONE_RIGHT_VOLUME		0x07

//             
#define AUDIO_CONTROL_POINT_NUM (5);

//             
static struct i2c_client *new_client = NULL;

#if 0 //                        
static const struct i2c_device_id eamp_i2c_id[] = {{EAMP_I2C_DEVNAME,0},{}};
static unsigned short force[] = {EAMP_I2C_CHANNEL, EAMP_SLAVE_ADDR_WRITE, I2C_CLIENT_END, I2C_CLIENT_END};
static const unsigned short * const forces[] = { force, NULL };
static struct i2c_client_address_data addr_data = { .forces = forces,};
#endif

//                        
static const struct i2c_device_id eamp_i2c_id[] = {{EAMP_I2C_DEVNAME,0},{}};
static struct i2c_board_info __initdata  eamp_dev={I2C_BOARD_INFO(EAMP_I2C_DEVNAME,(EAMP_SLAVE_ADDR_WRITE>>1))};

//                   
//                                                                                             
static int eamp_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id);
static int eamp_i2c_remove(struct i2c_client *client);
//          
static struct i2c_driver eamp_i2c_driver = {
    .probe = eamp_i2c_probe,
    .remove = eamp_i2c_remove,
    //                          
    .driver = {
    	.name =EAMP_I2C_DEVNAME,
    	},
    .id_table = eamp_i2c_id,
    //                           
};

//                                              
static bool gsk_on = false;
static bool gep_on = false;
static bool ghp_on = false;
//               
static u8  gspvol = 0x1f;
static u8  ghplvol = 0x19;
static u8  ghprvol = 0x19;
static u8  gch1gain = 0x0;
static u8  gch2gain = 0x0;

//    
static u32 gMode	 = 0;
static u32 gPreMode  = 0;

//             
static int const speaker_response_time = 6; //  
static int const headphone_response_time = 12; //  

//                          
#define CONFIG_USING_EXTAMP_ALL_VOICE_BUFFER	0

//    
typedef enum
{
    GAIN_MASK_HP	  = 0x1,
    GAIN_MASK_SPEAKER = 0x2,
    GAIN_MASK_INPUT2  = 0x4,
    GAIN_MASK_INPUT1  = 0x8,
    GAIN_MASK_ALL     = (GAIN_MASK_HP     |
                         GAIN_MASK_SPEAKER |
                         GAIN_MASK_INPUT2  |
                         GAIN_MASK_INPUT1)
}gain_mask;

//                      
static bool gsk_forceon = false;
static bool gsk_preon   = false;
static bool gep_preon   = false;

//             
static  int gCtrPointNum = AUDIO_CONTROL_POINT_NUM;
static  s8  gCtrPoint[] = {2,2,5,5,5};  //                                       
static  s8  gCtrPoint_in1Gain[]= {0,6,12,20};
static  s8  gCtrPoint_in2Gain[]= {0,6,12,20};
static  s8  gCtrPoint_SpeakerVol[] = {
	-60 ,-50, -45, -42,
	-39 ,-36, -33, -30,
	-27 ,-24, -21, -20,
	-19 ,-18, -17, -16,
	-15 ,-14, -13, -12,
	-11 ,-10, -9,  -8 ,
	-7  ,-6 , -5 , -4 ,
	-3  ,-2 , -1,  0};

static  s8 gCtrPoint_HeadPhoneLVol[]= {
	-60 ,-50, -45, -42,
	-39 ,-36, -33, -30,
	-27 ,-24, -21, -20,
	-19 ,-18, -17, -16,
	-15 ,-14, -13, -12,
	-11 ,-10, -9,  -8 ,
	-7  ,-6 , -5 , -4 ,
	-3  ,-2 , -1,  0};

static  s8 gCtrPoint_HeadPhoneRVol[]= {
	-60 ,-50, -45, -42,
	-39 ,-36, -33, -30,
	-27 ,-24, -21, -20,
	-19 ,-18, -17, -16,
	-15 ,-14, -13, -12,
	-11 ,-10, -9,  -8 ,
	-7  ,-6 , -5 , -4 ,
	-3  ,-2 , -1,  0};

static  s8 *gCtrPoint_table[5]={
	gCtrPoint_in1Gain,
	gCtrPoint_in2Gain,
	gCtrPoint_SpeakerVol,
	gCtrPoint_HeadPhoneLVol,
	gCtrPoint_HeadPhoneRVol};

//                        

//                 
ssize_t static eamp_read_byte(u8 addr, u8 *returnData)
{
	char cmd_buf[1]={0x00};
	char readData = 0;
	int ret=0;

	if(!new_client)
	{
		printk("I2C client not initialized!!\n");
		return -1;
	}

	cmd_buf[0] = addr;

	ret = i2c_master_send(new_client, &cmd_buf[0], 1);
	if (ret < 0) {
		printk("read sends command error!!\n");
		return -1;
	}
	ret = i2c_master_recv(new_client, &readData, 1);
	if (ret < 0) {
		printk("reads recv data error!!\n");
		return -1;
	}
	*returnData = readData;
	printk("addr 0x%x data 0x%x\n",addr, readData);
	return 0;
}

//                 
static u8 I2CRead(u8 addr)
{
	u8 regvalue;
	eamp_read_byte(addr,&regvalue);
	return regvalue;
}

//               
static ssize_t I2CWrite(u8 addr, u8 writeData)
{
	char	write_data[2] = {0};
	int    ret=0;

	if(!new_client)
	{
		printk("I2C client not initialized!!\n");
		return -1;
	}

	write_data[0] = addr;		  //         
	write_data[1] = writeData;

	ret = i2c_master_send(new_client, write_data, 2);
	if (ret < 0) {
		printk("write sends command error!!\n");
		return -1;
	}
	printk("addr 0x%x data 0x%x\n",addr,writeData);
	return 0;
}

//              
static ssize_t eamp_write_byte(u8 addr, u8 writeData)
{
	char	write_data[2] = {0};
	int    ret=0;

	if(!new_client)
	{
		printk("I2C client not initialized!!\n");
		return -1;
	}

	write_data[0] = addr;		  //         
	write_data[1] = writeData;
	ret = i2c_master_send(new_client, write_data, 2);
	if (ret < 0) {
		printk("write sends command error!!\n");
		return -1;
	}
	printk("addr 0x%x data 0x%x\n",addr,writeData);
	return 0;
}

//                                                                                                           
#if 0
//                   
static ssize_t eamp_set_bypass_mode(bool enable)
{
	printk("%s: enable=%d\n", __func__,enable);
	u8 temp_control_reg = 0;
	eamp_read_byte(EAMP_REG_SUBSYSTEMCONTROL,&temp_control_reg);

	if (enable == true)
	{
		//             
		eamp_write_byte(EAMP_REG_SUBSYSTEMCONTROL, temp_control_reg & ~0x10);
	}
	else
	{
		//               
		eamp_write_byte(EAMP_REG_SUBSYSTEMCONTROL, temp_control_reg | 0x10);
	}
	return 0;
}

//                      
static ssize_t eamp_set_sws_mode(bool deactivate)
{
	printk("%s: deactivate=%d\n", __func__,deactivate);
	u8 temp_control_reg = 0;
	eamp_read_byte(EAMP_REG_SUBSYSTEMCONTROL,&temp_control_reg);
	if (deactivate == true)
	{
		//             
		eamp_write_byte(EAMP_REG_SUBSYSTEMCONTROL, temp_control_reg | 0x08);
	}
	else
	{
		//               
		eamp_write_byte(EAMP_REG_SUBSYSTEMCONTROL, temp_control_reg & ~0x08);
	}
	return 0;
}

//                                      
static ssize_t eamp_clear_input_gain(void)
{
	printk("%s:\n", __func__);
	u8 temp_input_reg = 0;

	//                                                      
	//                                         
	eamp_write_byte(EAMP_REG_INPUTCONTROL, temp_input_reg);

	return 0;
}

//                                  
static ssize_t eamp_set_input_gain(u8 inGain)
{
	printk("%s: nGain(0x%x)\n", __func__,inGain);
	u8 temp_input_reg = 0;

	eamp_read_byte(EAMP_REG_INPUTCONTROL,&temp_input_reg);
	temp_input_reg = (temp_input_reg >>4)<<4;
	eamp_write_byte(EAMP_REG_INPUTCONTROL, temp_input_reg | (inGain & 0xf));

	return 0;
}
#endif

//                            
static ssize_t eamp_set_input1_gain(u8 inGain)
{
	u8 temp_input_reg = 0;

	printk("%s: inGain1(0x%x)\n", __func__,inGain);

	eamp_read_byte(EAMP_REG_INPUTCONTROL,&temp_input_reg);
	temp_input_reg = (temp_input_reg & 0xf3) | (inGain & 0x3)<<2 ;
	eamp_write_byte(EAMP_REG_INPUTCONTROL, temp_input_reg);

	return 0;
}

//                            
static ssize_t eamp_set_input2_gain(u8 inGain)
{
	u8 temp_input_reg = 0;

	printk("%s: inGain2(0x%x)\n", __func__, inGain);
	eamp_read_byte(EAMP_REG_INPUTCONTROL,&temp_input_reg);
	temp_input_reg = ((temp_input_reg >>2)<<2) | (inGain & 0x3) ;
	eamp_write_byte(EAMP_REG_INPUTCONTROL, temp_input_reg);

	return 0;
}

#if 0
//                                                                                         
static ssize_t eamp_set_input_mode(bool in1se, bool in2se)
{
	u8 temp_input_reg = 0;
	eamp_read_byte(EAMP_REG_INPUTCONTROL,&temp_input_reg);
	if(in1se)
	{

		temp_input_reg = temp_input_reg & ~0x20;
	}
	else
	{
		temp_input_reg =temp_input_reg | 0x20;
	}

	if(in2se)
	{

		temp_input_reg =temp_input_reg & ~0x10;
	}
	else
	{
		temp_input_reg =temp_input_reg | 0x10;
	}
	return eamp_write_byte(EAMP_REG_INPUTCONTROL, temp_input_reg);
}

//                       
static ssize_t eamp_Release_attackTime_speed(u8 ATK_time, u8 REL_time)
{
	u8	write_data = (ATK_time & 0x07) | (REL_time & 0x0f)<<3 ;
	return eamp_write_byte(EAMP_REG_LIMITER_CONTROL,write_data);
}

//                       

//                 
static ssize_t eamp_set_speakerLimiter_level(u8 limitlev)
{
	u8 temp_speak_reg = 0;
	eamp_read_byte(EAMP_REG_SPEAKER_OUTPUT_CONTROL,&temp_speak_reg);

	temp_speak_reg = (temp_speak_reg >>4) <<4;
	temp_speak_reg  = temp_speak_reg | (limitlev & 0x0f);
	return eamp_write_byte(EAMP_REG_SPEAKER_OUTPUT_CONTROL,temp_speak_reg);
}

//                                             
static ssize_t eamp_speakerLimiter_enable(bool enable)
{
	u8 temp_speak_reg = 0;
	eamp_read_byte(EAMP_REG_SPEAKER_OUTPUT_CONTROL,&temp_speak_reg);

	if (enable == true)
	{
		//      
		eamp_write_byte(EAMP_REG_SPEAKER_OUTPUT_CONTROL, temp_speak_reg | 0x10);
	}
	else
	{
		//       
		eamp_write_byte(EAMP_REG_SPEAKER_OUTPUT_CONTROL, temp_speak_reg & ~0x10);
	}
	return 0;
}

//                             
static ssize_t eamp_set_speakerOut(u8 speakerout)
{
	u8 temp_speak_reg = 0;
	eamp_read_byte(EAMP_REG_SPEAKER_OUTPUT_CONTROL,&temp_speak_reg);

	temp_speak_reg = (temp_speak_reg & 0x9f ) | ((speakerout & 0x03) << 5);
	eamp_write_byte(EAMP_REG_SPEAKER_OUTPUT_CONTROL, temp_speak_reg);
	return 0;
}

//                         
//                            
static ssize_t eamp_set_headPhoneLimiter_level(u8 limitlev)
{
	u8 temp_headphone_reg = 0;
	eamp_read_byte(EAMP_REG_HEADPHONE_OUTPUT_CONTROL,&temp_headphone_reg);

	temp_headphone_reg = (temp_headphone_reg >>3) <<3;
	temp_headphone_reg	= temp_headphone_reg | (limitlev & 0x07);
	return eamp_write_byte(EAMP_REG_HEADPHONE_OUTPUT_CONTROL,temp_headphone_reg);
}

//                                  
static ssize_t eamp_headPhoneLimiter_enable(bool enable)
{
	u8 temp_headphone_reg = 0;
	eamp_read_byte(EAMP_REG_HEADPHONE_OUTPUT_CONTROL,&temp_headphone_reg);

	if (enable == true)
	{
		//      
		eamp_write_byte(EAMP_REG_HEADPHONE_OUTPUT_CONTROL, temp_headphone_reg | 0x10);
	}
	else
	{
		//       
		eamp_write_byte(EAMP_REG_HEADPHONE_OUTPUT_CONTROL, temp_headphone_reg & ~0x10);
	}
	return 0;
}

//                              
static ssize_t eamp_set_headPhoneOut(u8 headphoneout)
{
	u8 temp_headphone_reg = 0;
	eamp_read_byte(EAMP_REG_HEADPHONE_OUTPUT_CONTROL,&temp_headphone_reg);

	temp_headphone_reg = (temp_headphone_reg & 0x9f ) | ((headphoneout & 0x03) << 5);
	return eamp_write_byte(EAMP_REG_HEADPHONE_OUTPUT_CONTROL, temp_headphone_reg);
}

//              

//            
static ssize_t eamp_set_speaker_Open(bool enable)
{
	printk("%s: enable=%d\n",__func__,enable);
	u8 temp_spvol_reg = 0;
	eamp_read_byte(EAMP_REG_SPEAKER_VOLUME,&temp_spvol_reg);

	if (enable == true)
	{
		//      
		eamp_write_byte(EAMP_REG_SPEAKER_VOLUME, temp_spvol_reg | 0x20);
	}
	else
	{
		//       
		eamp_write_byte(EAMP_REG_SPEAKER_VOLUME, temp_spvol_reg & ~0x20);
	}
	return 0;
}

//                             
//                              
static ssize_t eamp_set_headPhoneL_open(bool enable)
{
	printk("%s: enable=%d\n",__func__,enable);
	u8 temp_hpvol_reg = 0;
	eamp_read_byte(EAMP_REG_HEADPHONE_LEFT_VOLUME,&temp_hpvol_reg);

	if (enable == true)
	{
		//      
		eamp_write_byte(EAMP_REG_HEADPHONE_LEFT_VOLUME, temp_hpvol_reg | 0x20);
	}
	else
	{
		//       
		eamp_write_byte(EAMP_REG_HEADPHONE_LEFT_VOLUME, temp_hpvol_reg & ~0x20);
	}
	return 0;
}

//                           
static ssize_t eamp_set_headPhone_lvol(u8 HPL_Vol)
{
	printk("%s: vol=0x%x\n",__func__,HPL_Vol);

	u8 temp_hpvol_reg = 0;
	eamp_read_byte(EAMP_REG_HEADPHONE_LEFT_VOLUME,&temp_hpvol_reg);
	temp_hpvol_reg = (temp_hpvol_reg>>5)<<5;
	temp_hpvol_reg = temp_hpvol_reg & 0xbf;
	temp_hpvol_reg = temp_hpvol_reg | (HPL_Vol & 0x1f);

	return eamp_write_byte(EAMP_REG_HEADPHONE_LEFT_VOLUME,temp_hpvol_reg);
}

//                                        

//                               
static ssize_t eamp_set_headPhoneR_open(bool enable)
{
	printk("%s:enable=%d\n",__func__,enable);
	u8 temp_hpvol_reg = 0;
	eamp_read_byte(EAMP_REG_HEADPHONE_RIGHT_VOLUME,&temp_hpvol_reg);

	if (enable == true)
	{
		//      
		eamp_write_byte(EAMP_REG_HEADPHONE_RIGHT_VOLUME, temp_hpvol_reg | 0x20);
	}
	else
	{
		//       
		eamp_write_byte(EAMP_REG_HEADPHONE_RIGHT_VOLUME, temp_hpvol_reg & ~0x20);
	}
	return 0;
}

//                           
static ssize_t eamp_set_headPhone_rvol(u8 HPR_Vol)
{
	printk("%s:vol=0x%x\n",__func__,HPR_Vol);

	u8 temp_hpvol_reg = 0;
	eamp_read_byte(EAMP_REG_HEADPHONE_RIGHT_VOLUME,&temp_hpvol_reg);
	temp_hpvol_reg = (temp_hpvol_reg>>5)<<5;
	temp_hpvol_reg = temp_hpvol_reg | (HPR_Vol & 0x1f);

	return eamp_write_byte(EAMP_REG_HEADPHONE_RIGHT_VOLUME,temp_hpvol_reg);
}
#endif

//                  
static ssize_t eamp_set_speaker_vol(u8	vol)
{
	u8 temp_spvol_reg = 0;

	printk("%s: vol=0x%x\n",__func__,vol);

	eamp_read_byte(EAMP_REG_SPEAKER_VOLUME,&temp_spvol_reg);
	temp_spvol_reg = (temp_spvol_reg >> 5) << 5;
	temp_spvol_reg = temp_spvol_reg | (vol & 0x1f);
	eamp_write_byte(EAMP_REG_SPEAKER_VOLUME, temp_spvol_reg);
	return 0;
}

//                      
static ssize_t eamp_set_headPhone_vol(u8 HP_vol)
{
	u8 temp_hpvol_reg = 0;

	printk("%s: vol=0x%x\n",__func__,HP_vol);

	eamp_read_byte(EAMP_REG_HEADPHONE_LEFT_VOLUME,&temp_hpvol_reg);
	temp_hpvol_reg = (temp_hpvol_reg>>5)<<5;
	temp_hpvol_reg = temp_hpvol_reg | 0x40;
	temp_hpvol_reg = temp_hpvol_reg | (HP_vol & 0x1f);
	return eamp_write_byte(EAMP_REG_HEADPHONE_LEFT_VOLUME,temp_hpvol_reg);
}

//                                                                                                 

//                               
static ssize_t eamp_resetRegister(void)
{
	printk("%s:\n", __func__);
	I2CWrite(0,0x18);
	I2CWrite(1,0x00);
	I2CWrite(2,0x2A);
	I2CWrite(3,0x0F);
	I2CWrite(4,0x00);
	I2CWrite(5,0x15);
	I2CWrite(6,0x5F);
	I2CWrite(7,0x1F);
	return 0;
}

static ssize_t eamp_openEarpiece(void)
{
	printk("%s:\n", __func__);
#if USE_ANALOG_SWITCH
        mt_set_gpio_out(GPIO_AUDIO_SEL, GPIO_OUT_ONE);
#endif
	I2CWrite(0,0x10); //                                                   
	gep_on=true;
	return 0;
}

static ssize_t eamp_closeEarpiece(void)
{
	printk("%s:\n", __func__);
#if USE_ANALOG_SWITCH
	mt_set_gpio_out(GPIO_AUDIO_SEL, GPIO_OUT_ZERO);
#endif
	I2CWrite(0,0x00);//                                      
	gep_on=false;
	return 0;
}

static ssize_t eamp_openheadPhone(void)
{
	printk("%s:\n", __func__);
#if 0
    if(!new_client)
    {
        printk("I2C client not initialized!!\n");
        eamp_register();
        msleep(1);
    }
#endif

	if(gep_on) //                                     
	{
		I2CWrite(0,0x10);//                           
	}
	else
	{
		I2CWrite(0,0x00);//         
	}
#if CONFIG_USING_EXTAMP_ALL_VOICE_BUFFER
	if(gMode==2) //            
	{
		I2CWrite(1,0x30 | gch1gain<<2 | gch2gain);//                                                   
	}
	else
#endif
	{
		//                                                                                       
		I2CWrite(1,0x10 | gch1gain<<2 | gch2gain);//                                                                         
	}
	//                                       
	//                                         

	I2CWrite(6,0x7F);//                    
	I2CWrite(7,0x7F);//                      
#if CONFIG_USING_EXTAMP_ALL_VOICE_BUFFER
	if(gMode==2)
	{
		I2CWrite(4,0x40);//                     
	}
	else
#endif
	{
		I2CWrite(4,0x20);//                      
	}
	if(ghplvol == ghprvol)
	{
		//                                                      
	}
	else
	{
		//                           
		//                           
	}
	ghp_on = true;
	msleep(headphone_response_time);
	return 0;
}

static ssize_t eamp_closeheadPhone(void)
{
	printk("%s:\n", __func__);
	I2CWrite(6,0x20);  //                                          
	I2CWrite(4,0x00);	//       
	I2CWrite(6,0x00);	//           
	I2CWrite(7,0x00);	//           
	//                 
	//                 
	if(!gsk_on)
	{
		I2CWrite(0,0x08); //         
	}
	ghp_on = false;
	return 0;
}

static ssize_t eamp_openspeaker(void)
{
	printk("%s:\n", __func__);
#if 0
if(!new_client)
{
    printk("I2C client not initialized!!\n");
    eamp_register();
    msleep(1);
}
#endif

#if 0
	if(gMode == 2) //            
	{
		I2CWrite(0,0x00);//                 
		I2CWrite(1,0x30 | gch1gain<<2 | gch2gain);//                                            
		I2CWrite(5,0x35);//                       
		//                                                  
		I2CWrite(3,0x40);//                              
		I2CWrite(5,0x20 | gspvol);//                               
	}
	else
	{
		I2CWrite(0,0x00);//                 
		I2CWrite(1,0x00 | gch1gain<<2 | gch2gain);//                                         
		I2CWrite(5,0x35);//                       
		I2CWrite(2,0x00);//                               
		I2CWrite(3,0x3D);//                             
		I2CWrite(5,0x20 | gspvol);//                               
	}
#else
    I2CWrite(0,0x00);//                 
    //                                                                                        
    I2CWrite(1,0x10 | gch1gain<<2 | gch2gain);//                                                                          
    //                                          
    I2CWrite(5,0x3F);//                       
    //                                                  
    I2CWrite(3,0x40);//                              
    //                                                           
#endif

	gsk_on = true;
	msleep(speaker_response_time);
	return 0;
}

static ssize_t eamp_closespeaker(void)
{
	printk("%s:\n", __func__);

	I2CWrite(5,0x20);//                      
	I2CWrite(3,0x00);//        
	I2CWrite(5,0x00);//           
	if(!ghp_on)
	{
		I2CWrite(0,0x08); //         
	}
	gsk_on = false;
	return 0;
}

//                                                         
//                        
//                                                               
//                               
//                                         
//                

static ssize_t eamp_changeGainVolume(unsigned long int param)
{
	u8 mask = param & 0xF;
	u32 gainvol = param & 0xFFFFFF;

	printk("%s: param(0x%x)\n",__func__,(unsigned int)param);

	if(mask & GAIN_MASK_INPUT1)
	{
		u8 ch1 = (gainvol>>22) & 0x3;
		if(gch1gain != ch1)
		{
			gch1gain = ch1;
			eamp_set_input1_gain(gch1gain);
		}
	}
	if(mask & GAIN_MASK_INPUT2)
	{
		u8 ch2 = (gainvol>>20) & 0x3;
	    if(gch2gain != ch2)
		{
			gch2gain = ch2;
			eamp_set_input2_gain(gch2gain);
		}
	}
	if(mask & GAIN_MASK_SPEAKER)
	{
	    u8 spk  = (gainvol>>15) & 0x1f;
		if(gspvol != spk)
		{
			gspvol	= spk;
			eamp_set_speaker_vol(gspvol);

		}
	}
	if(mask & GAIN_MASK_HP)
	{
		u8 hpl = (gainvol>>10) & 0x1f;
		if(ghplvol != hpl)
	    {
	    	ghplvol = hpl;
	        ghprvol = ghplvol; //              
			eamp_set_headPhone_vol(ghplvol);
		}
	}
	return 0;
}

static ssize_t eamp_getGainVolume(void)
{
	u8 gain     = I2CRead(1) & 0xF ;
	u8 speakvol = I2CRead(5) & 0x1F;
	u8 hplreg	= I2CRead(6);
	u8 hplvol	= hplreg & 0x1F;
	u8 hprvol	= I2CRead(7) & 0x1F;

	printk("%s:\n", __func__);

	if(  hplreg & 0x20 )
		return ( gain << 20 | speakvol << 15 |  \
		hplvol << 10 | hplvol << 5 );

	return ( gain << 20 | speakvol << 15 |	\
		hplvol << 10 | hprvol << 5 );
}

static ssize_t eamp_suspend(void)
{
	printk("%s:\n", __func__);
	eamp_resetRegister();
	return 0;
}

static ssize_t eamp_resume(void)
{
	printk("%s:\n", __func__);
	if(gsk_on)
	{
		eamp_openspeaker();
	}
	if(ghp_on)
	{
		eamp_openheadPhone();
	}
	if(gep_on)
	{
		eamp_openEarpiece();
	}
	return 0;
}

static ssize_t eamp_getRegister(unsigned int regName)
{
	printk("%s: Regname=%u\n",__func__,regName);

	if(regName >7)
		return -1;
	return I2CRead(regName);
}

static ssize_t eamp_setRegister(unsigned long int param)
{
	AMP_Control * p = (AMP_Control*)param;

	printk("%s:\n", __func__);

	if(p->param1 >7)
		return -1;

	return I2CWrite(p->param1,p->param2);
}

static ssize_t eamp_setMode(unsigned long int param)
{
	printk("%s: mode(%u)\n",__func__,(unsigned int)param);
	gMode = param;
	return 0;
}

static ssize_t eamp_getCtrlPointNum(void)
{
	printk("%s:\n", __func__);
	return gCtrPointNum;
}

static ssize_t eamp_getCtrPointBits(unsigned long int param)
{
	printk("%s: CtrPointBits(%u)\n",__func__,(unsigned int)param);
	return gCtrPoint[param];
}

static ssize_t eamp_getCtrlPointTable(unsigned long int param)
{
	AMP_Control *ampCtl = (AMP_Control*)param;

	printk("%s: CtrlPointTable(0x%x)\n",__func__,(unsigned int)param);

	if(copy_to_user((void __user *)ampCtl->param2,(void *)gCtrPoint_table[ampCtl->param1], 1<<gCtrPoint[ampCtl->param1])){
		return -1;
	}
	return 0;
}

static int eamp_command(unsigned int type, unsigned long args, unsigned int count)
{
	printk("%s: type(%u)\n",__func__,type);
	switch(type)
	{
		case EAMP_SPEAKER_CLOSE:
		{
			eamp_closespeaker();
			break;
		}
		case EAMP_SPEAKER_OPEN:
		{
			eamp_openspeaker();
			break;
		}
		case EAMP_HEADPHONE_CLOSE:
		{
			eamp_closeheadPhone();
			break;
		}
		case EAMP_HEADPHONE_OPEN:
		{
			eamp_openheadPhone();
			break;
		}
		case EAMP_EARPIECE_OPEN:
		{
			eamp_openEarpiece();
			break;
		}
		case EAMP_EARPIECE_CLOSE:
		{
			eamp_closeEarpiece();
			break;
		}
		case EAMP_GETREGISTER_VALUE:
		{
			return eamp_getRegister(args);
			break;
		}
		case EAMP_GETAMP_GAIN:
		{
			return eamp_getGainVolume();
			break;
		}
		case EAMP_SETAMP_GAIN:
		{
			eamp_changeGainVolume(args);
			break;
		}
		case EAMP_SETREGISTER_VALUE:
		{
			eamp_setRegister(args);
			break;
		}
		case EAMP_GET_CTRP_NUM:
		{
			return eamp_getCtrlPointNum();
			break;
		}
		case EAMP_GET_CTRP_BITS:
		{
			return eamp_getCtrPointBits(args);
			break;
		}
		case EAMP_GET_CTRP_TABLE:
		{
			eamp_getCtrlPointTable(args);
			break;
		}
		case EAMP_SETMODE:
		{
			eamp_setMode(args);
		}
		default:
		return 0;
	}
	return 0;
}

int Audio_eamp_command(unsigned int type, unsigned long args, unsigned int count)
{
	return eamp_command(type,args,count);
}

#if 0
static int eamp_i2c_detect(struct i2c_client *client, int kind, struct i2c_board_info *info)
{
    strcpy(info->type, EAMP_I2C_DEVNAME);
	return 0;
}
#endif

static int eamp_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
       printk("%s:\n", __func__);
	new_client = client;
	Speaker_Init();
       mdelay(10);
	eamp_resetRegister();
	//                                      
	return 0;
}

static int eamp_i2c_remove(struct i2c_client *client)
{
	printk("%s:\n", __func__);
	new_client = NULL;
      i2c_unregister_device(client);
      i2c_del_driver(&eamp_i2c_driver);
	return 0;
}

static void eamp_poweron(void)
{
       printk("%s:\n", __func__);
	mt_set_gpio_mode(GPIO_EXT_SPKAMP_EN_PIN, GPIO_MODE_00);
	mt_set_gpio_pull_enable(GPIO_EXT_SPKAMP_EN_PIN, GPIO_PULL_ENABLE);
	mt_set_gpio_dir(GPIO_EXT_SPKAMP_EN_PIN, GPIO_DIR_OUT); //       
	mt_set_gpio_out(GPIO_EXT_SPKAMP_EN_PIN, GPIO_OUT_ZERO); //            
	udelay(1000);
	mt_set_gpio_dir(GPIO_EXT_SPKAMP_EN_PIN, GPIO_DIR_OUT); //       
	mt_set_gpio_out(GPIO_EXT_SPKAMP_EN_PIN, GPIO_OUT_ONE); //            

	return;
}

static void eamp_powerdown(void)
{
	printk("%s:\n", __func__);
	eamp_closeheadPhone();
	eamp_closespeaker();

        mt_set_gpio_dir(GPIO_EXT_SPKAMP_EN_PIN, GPIO_DIR_OUT); //       
        mt_set_gpio_out(GPIO_EXT_SPKAMP_EN_PIN, GPIO_OUT_ZERO); //            

	return;
}

static int eamp_init(void)
{
       printk("%s:\n", __func__);
	eamp_poweron();

	return 0;
}

static int eamp_deinit(void)
{
	printk("%s:\n", __func__);
	eamp_powerdown();
	return 0;
}

static int eamp_register(void)
{
	printk("%s:\n", __func__);
	i2c_register_board_info(SOUND_I2C_CHANNEL,&eamp_dev,1);
       if (i2c_add_driver(&eamp_i2c_driver)){
		printk("fail to add device into i2c");
		return -1;
	}
	return 0;
}

/*                                                                            
                                                             
                                                                              
*/
extern void Yusu_Sound_AMP_Switch(BOOL enable);

bool Speaker_Init(void)
{
	printk("%s:\n", __func__);
	eamp_init();
	return true;
}

bool Speaker_Register(void)
{
	printk("%s:\n", __func__);
	eamp_register();
	return true;
}

int ExternalAmp(void)
{
	return 1;
}

void Sound_SpeakerL_SetVolLevel(int level)
{
	printk("%s: level=%d\n",__func__,level);
}

void Sound_SpeakerR_SetVolLevel(int level)
{
	printk("%s: level=%d\n",__func__,level);
}

void Sound_Speaker_Turnon(int channel)
{
       printk("%s:\n", __func__);
	eamp_command(EAMP_SPEAKER_OPEN,channel,1);
}

void Sound_Speaker_Turnoff(int channel)
{
       printk("%s:\n", __func__);
	eamp_command(EAMP_SPEAKER_CLOSE,channel,1);
}

void Sound_Speaker_SetVolLevel(int level)
{

}

void Sound_Headset_Turnon(void)
{
	printk("%s:\n", __func__);
	eamp_command(EAMP_HEADPHONE_OPEN,0,1);
}

void Sound_Headset_Turnoff(void)
{
	printk("%s:\n", __func__);
	eamp_command(EAMP_HEADPHONE_CLOSE,0,1);
}

void Sound_Earpiece_Turnon(void)
{
      printk("%s:\n", __func__);
      eamp_openEarpiece();
}

void Sound_Earpiece_Turnoff(void)
{
      printk("%s:\n", __func__);
      eamp_closeEarpiece();
}

//          
void AudioAMPDevice_Suspend(void)
{
	printk("%s:\n", __func__);
	eamp_suspend();
}

void AudioAMPDevice_Resume(void)
{
	printk("%s:\n", __func__);
	eamp_resume();
}

//                   
void AudioAMPDevice_SpeakerLouderOpen(void)
{
	printk("%s:\n", __func__);
	if(gsk_on && gMode != 2) //                              
		return;
	gsk_forceon = true;
	gPreMode = gMode;
	gsk_preon = gsk_on;
	gep_preon = gep_on;
	if(gsk_on)
	{
		eamp_closespeaker();
	}
	gMode = 0;
	eamp_openspeaker();
	return ;
}

//                   
void AudioAMPDevice_SpeakerLouderClose(void)
{
	printk("%s:\n", __func__);
	if(gsk_forceon)
	{
		eamp_closespeaker();
		gMode = gPreMode;
		if(gep_preon)
		{
			eamp_openEarpiece();
		}
		else if(gsk_preon)
		{
			eamp_openspeaker();
		}
	}
	gsk_forceon = false;
}

//                                 
void AudioAMPDevice_mute(void)
{
	if(ghp_on)
		eamp_closeheadPhone();
	if(gsk_on)
		eamp_closespeaker();
	//                          
}

bool Speaker_DeInit(void)
{
	printk("%s:\n", __func__);
	eamp_deinit();
	return true;
}

static char *ExtFunArray[] =
{
	"InfoMATVAudioStart",
	"InfoMATVAudioStop",
	"End",
};

kal_int32 Sound_ExtFunction(const char* name, void* param, int param_size)
{
	int i = 0;
	int funNum = -1;

	//                                                    
	while(strcmp("End",ExtFunArray[i]) != 0 ) {		//                                 

		if (strcmp(name,ExtFunArray[i]) == 0 ) {		//                                        
			funNum = i;
			break;
		}
		i++;
	}

	switch (funNum) {
	case 0:			//                  
		printk("InfoMATVAudioStart");
		break;

	case 1:			//                 
		printk("InfoMATVAudioStop");
		break;

	default:
		break;
	}
	return 1;
}

static int __init tpa_amp_init(void)
{
    printk("%s:\n", __func__);
    eamp_register();
}

static void __exit tpa_amp_exit(void)
{
    printk("%s:\n", __func__);
    eamp_deinit();
}


module_init(tpa_amp_init);
module_exit(tpa_amp_exit);

