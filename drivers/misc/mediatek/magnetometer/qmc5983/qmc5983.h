/*
                                                
 */
	 
#ifndef __QMC5983_H__
#define __QMC5983_H__
	 
#include <linux/ioctl.h>  /*                  */
	 
#define QMC5983_IOCTL_BASE 'm'
	 /*                                                                    */
#define QMC5983_SET_RANGE		_IOW(QMC5983_IOCTL_BASE, 1, int)
#define QMC5983_SET_MODE		_IOW(QMC5983_IOCTL_BASE, 2, int)
#define QMC5983_SET_BANDWIDTH	_IOW(QMC5983_IOCTL_BASE, 3, int)
#define QMC5983_READ_MAGN_XYZ	_IOR(QMC5983_IOCTL_BASE, 4, int)	
#define QMC5983_SET_REGISTER_A	_IOW(QMC5983_IOCTL_BASE, 5, char *)
#define QMC5983_SELF_TEST	   _IOWR(QMC5983_IOCTL_BASE, 6, char *)

/*                                                                   */
	 /*                                */
#define CRA_REG_M	0x00  /*                          */
#define CRB_REG_M	0x01  /*                          */
#define MR_REG_M	0x02  /*               */
	 
	 /*                              */
#define OUT_X_M		0x03
#define OUT_X_L		0x04
#define OUT_Z_M		0x05
#define OUT_Z_L		0x06
#define OUT_Y_M		0x07
#define OUT_Y_L		0x08





	 
	 /*                                               */
#define IRA_REG_M	0x0A
#define IRB_REG_M   0x0B
#define IRC_REG_M   0x0C
	 
	 /*                               */
#define GAIN_0	1370	/*                          */
#define GAIN_1	1090	/*                         */
#define GAIN_2	820		/*                         */
#define GAIN_3	660		/*                         */
#define GAIN_4	440		/*                         */
#define GAIN_5	390		/*                         */
#define GAIN_6	330		/*                         */
#define GAIN_7	230		/*                         */
	 
	 /*                 */
#define SR_REG_M    0x09
	 
	 /*                       */
#define TO_MSB_T 0x31
#define TO_LSB_T 0x32
	 
	 /*                                                    */
#define AE_REG_0 0x10 /*                        */
#define AE_REG_1 0x30 /*                        */
#define AE_REG_2 0x50 /*                        */
#define AE_REG_3 0x70 /*                        */
	 
	 /*                                           */
#define RATE_REG_0 0x60  /*                               */
#define RATE_REG_1 0x64  /*                              */
#define RATE_REG_2 0x68  /*                            */
#define RATE_REG_3 0x6c  /*                              */
#define RATE_REG_4 0x70  /*                             */
#define RATE_REG_5 0x74  /*                             */
#define RATE_REG_6 0x78  /*                             */
#define RATE_REG_7 0x7c  /*                              */


	 
	 /*                                              */
	 /*                                 */
	 /*                                              */
	 
	 /*                                */
#define QMC5983_0_88G		0x00
#define QMC5983_1_3G		0x20
#define QMC5983_1_9G		0x40
#define QMC5983_2_5G		0x60
#define QMC5983_4_0G		0x80
#define QMC5983_4_7G		0xA0
#define QMC5983_5_6G		0xC0
#define QMC5983_8_1G		0xE0
	 
	 /*                                */
#define QMC5983_NORMAL_MODE	0x00
#define QMC5983_POS_BIAS	0x01
#define QMC5983_NEG_BIAS	0x02
#define QMC5983_CC_MODE		0x00
#define QMC5983_SC_MODE		0x01
#define QMC5983_IDLE_MODE	0x02
#define QMC5983_SLEEP_MODE	0x03
	 
	 /*                                */
#define QMC5983_ODR_75		0x00	/*                         */
#define QMC5983_ODR1_5		0x04	/*                        */
#define QMC5983_ODR3_0		0x08	/*                      */
#define QMC5983_ODR7_5		0x0C	/*                        */
#define QMC5983_ODR15		0x10	/*                       */
#define QMC5983_ODR30		0x14	/*                       */
#define QMC5983_ODR75		0x18	/*                       */
#define QMC5983_ODR220		0x1C	/*                        */
	 
  #define SAMPLE_AVERAGE_8		(0x3 << 5)
  #define OUTPUT_RATE_75		(0x6 << 2)
  #define MEASURE_NORMAL		0
  #define MEASURE_SELFTEST		0x1
#define GAIN_DEFAULT		  (3 << 5)


//                                                     
//                                        
//                        
//                        
//                  
//                        
#define CONVERT_M			6
#define CONVERT_M_DIV		100			//                  
#define CONVERT_O			1
#define CONVERT_O_DIV		1			//                 

  
#ifdef __KERNEL__
	 
#if 0 /*                             */
	 struct QMC5983_platform_data {
	 
		 u8 h_range;
	 
		 u8 axis_map_x;
		 u8 axis_map_y;
		 u8 axis_map_z;
	 
		 u8 negate_x;
		 u8 negate_y;
		 u8 negate_z;
	 
		 int (*init)(void);
		 void (*exit)(void);
		 int (*power_on)(void);
		 int (*power_off)(void);
	 
	 };
#endif

#endif /*            */
	 
	 
#endif  /*               */

