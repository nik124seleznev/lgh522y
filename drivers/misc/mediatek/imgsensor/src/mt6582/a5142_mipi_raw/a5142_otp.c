/*                                                                                                
           
                                                         
                                          
          
                                                         
     
                                                                
                                                                     
                             
   
                                 
                                                     
                                                                                          
                                                                                                 */

#include <linux/videodev2.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <asm/atomic.h>
#include <linux/slab.h>


#include "kd_camera_hw.h"
#include "kd_imgsensor.h"
#include "kd_imgsensor_define.h"
#include "kd_imgsensor_errcode.h"

#include "a5142mipi_Sensor.h"
#include "a5142mipi_Camera_Sensor_para.h"
#include "a5142mipi_CameraCustomized.h"

#include "a5142_otp.h"

#define A5142MIPI_DEBUG
#ifdef A5142MIPI_DEBUG
#define SENSORDB printk
#else
#define SENSORDB(x,...)
#endif

#define TRULY_ID           0x0001
#define LSC_REGSIZE        106

#define GAIN_DEFAULT       0x112A
#define GAIN_GREEN1_ADDR   0x3056
#define GAIN_BLUE_ADDR     0x3058
#define GAIN_RED_ADDR      0x305A
#define GAIN_GREEN2_ADDR   0x305C

USHORT golden_r;
USHORT golden_gr;
USHORT golden_gb;
USHORT golden_b;

USHORT current_r;
USHORT current_gr;
USHORT current_gb;
USHORT current_b;

extern kal_uint16 A5142MIPI_write_cmos_sensor(kal_uint32 addr, kal_uint32 para);
extern kal_uint16 A5142MIPI_read_cmos_sensor(kal_uint32 addr);

USHORT otp_reglist[] = 
{
	0x381e, //         
	0x3820,
	0x3822,
	0x3824,
	0x3826,
	0x3828,
	0x382a,
	0x382c,
	0x382e,
	0x3830,      
	0x3832,
	0x3834,
	0x3836,
	0x3838,
	0x383a,
	0x383c,
	0x383e,
	0x3840,      
	0x3842,
	0x3844,
	0x3846,
	0x3848,
	0x384a,
	0x384c,
	0x384e,
	0x3850,      
	0x3852,
	0x3854,
	0x3856,
	0x3858,
	0x385a,
	0x385c,
	0x385e,
	0x3860,      
	0x3862,
	0x3864,
	0x3866,
	0x3868,
	0x386a,
	0x386c,
	0x386e,
	0x3870,      
	0x3872,
	0x3874,
	0x3876,
	0x3878,
	0x387a,
	0x387c,
	0x387e,
	0x3880,      
	0x3882,
	0x3884,
	0x3886,
	0x3888,
	0x388a,
	0x388c,
	0x388e,
	0x3890,      
	0x3892,
	0x3894,
	0x3896,
	0x3898,
	0x389a,
	0x389c,
	0x389e,
	0x38a0,      
	0x38a2,
	0x38a4,
	0x38a6,
	0x38a8,
	0x38aa,
	0x38ac,
	0x38ae,
	0x38b0,      
	0x38b2,
	0x38b4,
	0x38b6,
	0x38b8,
	0x38ba,
	0x38bc,
	0x38be,
	0x38c0,      
	0x38c2,
	0x38c4,
	0x38c6,
	0x38c8,
	0x38ca,
	0x38cc,
	0x38ce,
	0x38d0,      
	0x38d2,
	0x38d4,
	0x38d6,
	0x38d8,
	0x38da,
	0x38dc,
	0x38de,
	0x38e0,      
	0x38e2,
	0x38e4,
	0x38e6,
	0x38e8,
	0x38ea,
	0x38ec,
	0x38ee,
	0x38f0,
};

USHORT lsc_reglist[] = 
{
   //           
	0x3600, 
	0x3602, 
	0x3604, 
	0x3606, 
	0x3608, 
	0x360A, 
	0x360C, 
	0x360E, 
	0x3610, 
	0x3612, 
	0x3614, 
	0x3616, 
	0x3618, 
	0x361A, 
	0x361C, 
	0x361E, 
	0x3620, 
	0x3622, 
	0x3624, 
	0x3626, 

	0x3640, 
	0x3642, 
	0x3644, 
	0x3646, 
	0x3648, 
	0x364A, 
	0x364C, 
	0x364E, 
	0x3650, 
	0x3652, 
	0x3654, 
	0x3656, 
	0x3658, 
	0x365A, 
	0x365C, 
	0x365E, 
	0x3660, 
	0x3662, 
	0x3664, 
	0x3666, 

	0x3680, 
	0x3682, 
	0x3684, 
	0x3686, 
	0x3688, 
	0x368A, 
	0x368C, 
	0x368E, 
	0x3690, 
	0x3692, 
	0x3694, 
	0x3696, 
	0x3698, 
	0x369A, 
	0x369C, 
	0x369E, 
	0x36A0, 
	0x36A2, 
	0x36A4, 
	0x36A6, 

	0x36C0, 
	0x36C2, 
	0x36C4, 
	0x36C6, 
	0x36C8, 
	0x36CA, 
	0x36CC, 
	0x36CE, 
	0x36D0, 
	0x36D2, 
	0x36D4, 
	0x36D6, 
	0x36D8, 
	0x36DA, 
	0x36DC, 
	0x36DE, 
	0x36E0, 
	0x36E2, 
	0x36E4, 
	0x36E6, 

	0x3700, 
	0x3702, 
	0x3704, 
	0x3706, 
	0x3708, 
	0x370A, 
	0x370C, 
	0x370E, 
	0x3710, 
	0x3712, 
	0x3714, 
	0x3716, 
	0x3718, 
	0x371A, 
	0x371C, 
	0x371E, 
	0x3720, 
	0x3722, 
	0x3724, 
	0x3726,
	
	0x3782, 
	0x3784,
	
	0x37C0, 
	0x37C2, 
	0x37C4, 
	0x37C6, 
};


/*                                                                                                
                               
                                                                  
                                                                  
                                             
                                             
                                                                                                 */
bool start_read_otp(USHORT zone)
{
	USHORT record_type;
	
	USHORT temp;
	USHORT cnt;
	cnt = 0;

	if(zone > 0x0f)
		return 0;

	record_type = 0x3000 | (zone * 256) ;

//                                                  

//                                                                         
	A5142MIPI_write_cmos_sensor(0x301A, 0x0610); //                  
	
	A5142MIPI_write_cmos_sensor(0x3134, 0xCD95); //                                 

	A5142MIPI_write_cmos_sensor(0x304C, record_type); //                        
//                                                                        

	A5142MIPI_write_cmos_sensor(0x304A, 0x0210); //                        

	while (cnt < 50)
	{
		temp = A5142MIPI_read_cmos_sensor(0x304A);
		temp = temp & 0x0060;
		if (temp == 0x0060)
		{
			break;
		}
		cnt++;
		mDELAY(10); 		
	}

	if (cnt == 50)
	{
	//                                         

	//                                                

		return 0;
	}
	return 1;
}


/*                                                                                                
                             
                                    
                                                                
                                              
                                                                                     
                                                                                                 */
/*
                                
 
              
                   
                          
  
                                
            
  

                                           
                                           

                 
                       
                            

                                                  
                                                     

             
 
*/


/*                                                                                                
                             
                                       
                                                                  
                                                                                                
                                                                                                 */
USHORT get_otp_flag(USHORT zone)
{
	USHORT flag;
	if(!start_read_otp(zone))
	{
	//                               
		return 0;
	}

	flag = A5142MIPI_read_cmos_sensor(0x3826);

//                            

	return flag;
}


/*                                                                                                
                                  
                                   
                                                                  
                                            
                                                                              
                                                                                                 */
USHORT get_otp_module_id(USHORT zone)
{

	USHORT module_id;
	if(!start_read_otp(zone))
	{
		SENSORDB("DBG_DH:Otp read start Fail!\n");
		return 0;
	}

	module_id = A5142MIPI_read_cmos_sensor(0x3806);

	SENSORDB("DBG_DH:Module ID: 0x%04x\n", module_id);

	return module_id;
}


/*                                                                                                
                                
                                       
                                                                  
                                            
                                                        
                                                                                                 */
USHORT get_otp_lens_id(USHORT zone)
{
	USHORT lens_id;
	if(!start_read_otp(zone))
	{
//                                
		return 0;
	}

	lens_id = A5142MIPI_read_cmos_sensor(0x3808);

//                                   

	return lens_id;
}


/*                                                                                                
                               
                                      
                                                                  
                                            
                                                       
                                                                                                 */
USHORT get_otp_vcm_id(USHORT zone)
{
	USHORT vcm_id;
	if(!start_read_otp(zone))
	{
	//                               
		return 0;
	}

	vcm_id = A5142MIPI_read_cmos_sensor(0x380A);

//                                 

	return vcm_id;	
}


/*                                                                                                
                                  
                                         
                                                                  
                                            
                                                          
                                                                                                 */
USHORT get_otp_driver_id(USHORT zone)
{
	USHORT driver_id;
	if(!start_read_otp(zone))
	{
	//                                
		return 0;
	}

	driver_id = A5142MIPI_read_cmos_sensor(0x380C);

//                                       

	return driver_id;
}

/*                                                                                                
                             
                                                                                
                                               
                                              
                                                                  
                                                                                                 */
/*
                                 
 
              
                 
                          
  
                                
            
  

                                               

                                     

                        
                           
                             
                            
                             
                          
             
 
*/


/*                                                                                                
                                
                                        
                                                                  
                                          
                                                               
                                                                                                 */
bool otp_lenc_update(USHORT zone)
{
	USHORT IsShading;
	int i;

//                 
	USHORT *otp_srt;
	USHORT *otp_dst;
	USHORT otp_lsc[LSC_REGSIZE] = {0};
	otp_srt = otp_reglist;
	otp_dst = lsc_reglist;

	if(!start_read_otp(zone))
	{
		SENSORDB("DBG_DH:LSC update err\n");
		return 0;
	}

	//                                  
	if(otp_lsc == NULL)
	{
	//                                   
		return 0;
	}

	for(i=0;i<LSC_REGSIZE;i++)
	{
		*(otp_lsc + i) = A5142MIPI_read_cmos_sensor(*(otp_srt + i));

  //                                                                     
	}

	for(i=0;i<LSC_REGSIZE;i++)
	{
		A5142MIPI_write_cmos_sensor(*(otp_dst + i), *(otp_lsc + i));
	}

	//               

	IsShading = A5142MIPI_read_cmos_sensor(0x3780);
	IsShading = IsShading | 0x8000;

	A5142MIPI_write_cmos_sensor(0x3780, IsShading); //          

	SENSORDB("DBG_DH:LSC update from OTP Okay!\n");

	return 1;
}


/*                                                                                                
                             
                                                                                              
                                               
                                            
                                                      
                                                                                                 */
int wb_multi_cal(USHORT value)
{
	USHORT temp;

	int multiple;

	int Digital_Gain;
	int Column_Gain;
	int Asc1_Gain;
	int Initial_Gain;

	Digital_Gain = (value >> 12) & 0x000f;
	if(Digital_Gain < 1 || Digital_Gain > 7)
	{
//                            
		return 0;
	}

	temp = (value >> 10) & 0x0003;
	if(temp == 0)
    	Column_Gain  = 1;
	else if(temp == 1)
    	Column_Gain  = 3;
	else if(temp == 2)
    	Column_Gain  = 2;
	else if(temp == 3)
    	Column_Gain  = 4;

	temp = ( value >> 8) & 0x0003;
	if(temp == 3)
		Asc1_Gain = 40; //              
	else if(temp == 2)
		Asc1_Gain = 20; //              
	else if(temp == 1)
		Asc1_Gain = 13; //              
	else 
		Asc1_Gain = 10; //              

	Initial_Gain = value & 0x007f;

	multiple = (10 * Digital_Gain * Column_Gain * Asc1_Gain * Initial_Gain) >> 5 ;
	
	return multiple;
}


/*                                                                                                
                            
                                                        
                                                 
                                                             
                                                                                                 */
USHORT wb_gain_cal(int multiple)
{
	USHORT value;

	if( multiple <= 130)     //             
	{
		value = (USHORT)(32 * multiple / 100);
		value = value | 0x1000;
	}	    
	else if(multiple <= 200) //             
	{
		value = (USHORT)(32 * multiple / 130);
		value = 0x1100 | value;
	}
	else if(multiple <= 400) //            
	{
		value = (USHORT)(32 * multiple / 200);
		value = 0x1200 | value;
	}
	else if(multiple <= 800) //            
	{
		value = (USHORT)(32 * multiple / 400);
		value = 0x1300 | value;
	}
	else if(multiple <= 1200) //             
	{
		value = (USHORT)(32 * multiple / 800);
		value = 0x1B00 | value;
	}
	else if(multiple <= 1600) //             
	{
		value = (USHORT)(32 * multiple / 1200);
		value = 0x1700 | value;
	}
	else if(multiple <= 6400)
	{
		value = (USHORT)(32 * multiple / 1600);
		value = 0x1F00 | value;
	}
	else
	{
		return 0;
	}
	
	return value;
}


/*                                                                                                
                            
                                                            
                                                                           
                                                                           
                                        
                                                      
                                                                                                 */
bool wb_gain_set(int r_ratio, int b_ratio)
{
	int gain_multiple;

	int R_gain_multiple;
	int B_gain_multiple;

	USHORT R_GAIN;
	USHORT B_GAIN;

	if(!r_ratio || !b_ratio)
	{
		SENSORDB("DBG_DH:OTP WB ratio Data Err\n");
		return 0;
	}

	A5142MIPI_write_cmos_sensor(GAIN_GREEN1_ADDR, GAIN_DEFAULT); //                         
	A5142MIPI_write_cmos_sensor(GAIN_GREEN2_ADDR, GAIN_DEFAULT); //                         

	gain_multiple = wb_multi_cal(GAIN_DEFAULT);

	if(r_ratio == 100)
		A5142MIPI_write_cmos_sensor(GAIN_RED_ADDR, GAIN_DEFAULT); //                     
	else
	{
		R_gain_multiple = gain_multiple * r_ratio / 100;
		R_GAIN = wb_gain_cal(R_gain_multiple);
		A5142MIPI_write_cmos_sensor(GAIN_RED_ADDR, R_GAIN);
	}

	if(b_ratio == 100)
		A5142MIPI_write_cmos_sensor(GAIN_BLUE_ADDR, GAIN_DEFAULT); //                      
	else
	{
		B_gain_multiple = gain_multiple * b_ratio / 100;
		B_GAIN = wb_gain_cal(B_gain_multiple);
		A5142MIPI_write_cmos_sensor(GAIN_BLUE_ADDR, B_GAIN);
	}
	return 1;
}


/*                                                                                                
                              
                                      
                                                                  
                                          
                                                             
                                                                                                 */
bool otp_wb_update(USHORT zone)
{
/*
                  
                 
                  
                  
                  
                  
                   
                   
*/
	USHORT golden_g, current_g;

	int r_ratio;
	int b_ratio;

	if(!start_read_otp(zone))
	{
//                                
		return 0;
	}

	golden_r  = A5142MIPI_read_cmos_sensor(0x3816);     //           
	golden_b  = A5142MIPI_read_cmos_sensor(0x3818);     //           
	golden_gr = A5142MIPI_read_cmos_sensor(0x381A);     //           
	golden_gb = A5142MIPI_read_cmos_sensor(0x381C);     //           

	current_r  = A5142MIPI_read_cmos_sensor(0x381E); //           
	current_b  = A5142MIPI_read_cmos_sensor(0x3820); //           
	current_gr = A5142MIPI_read_cmos_sensor(0x3822); //            
	current_gb = A5142MIPI_read_cmos_sensor(0x3824); //           

	golden_g = (golden_gr + golden_gb) / 2;
	current_g = (current_gr + current_gb) / 2;

	if(!golden_g || !current_g || !golden_r || !golden_b || !current_r || !current_b)
	{
		SENSORDB("DBG_DH:wb update err\n");
		return 0;
	}

	r_ratio = 100 * golden_r * current_g /( golden_g * current_r );
	b_ratio = 100 * golden_b * current_g /( golden_g * current_b );

	wb_gain_set(r_ratio, b_ratio);

	SENSORDB("DBG_DH:wb update Okay\n");	

	return 1;
}


/*                                                                                                
                           
                              
                                                                  
                                          
                                                             
                                                                                                 */
bool get_otp_wb(USHORT zone)
{
	if(!start_read_otp(zone))
	{
//                                
		return 0;
	}

	golden_r  = A5142MIPI_read_cmos_sensor(0x3816);     //           
	golden_b  = A5142MIPI_read_cmos_sensor(0x3818);     //           
	golden_gr = A5142MIPI_read_cmos_sensor(0x381A);     //           
	golden_gb = A5142MIPI_read_cmos_sensor(0x381C);     //           

	current_r  = A5142MIPI_read_cmos_sensor(0x381E); //           
	current_b  = A5142MIPI_read_cmos_sensor(0x3820); //           
	current_gr = A5142MIPI_read_cmos_sensor(0x3822); //            
	current_gb = A5142MIPI_read_cmos_sensor(0x3824); //           

//                         

	return 1;
}
