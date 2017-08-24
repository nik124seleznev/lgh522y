#ifndef MPU6050_H
#define MPU6050_H

#include <linux/ioctl.h>

#define MPU6050_ACCESS_BY_GSE_I2C

#ifdef MPU6050_ACCESS_BY_GSE_I2C
    #define MPU6050_I2C_SLAVE_ADDR		(0xD2)   /*                                             */
#else
    #define MPU6050_I2C_SLAVE_ADDR		0xD0
#endif


/*                                                                */

#define MPU6050_REG_DEVID           0x75
#define MPU6050_REG_FIFO_EN         0x23    
#define MPU6050_REG_AUX_VDD         0x01    

#define MPU6050_REG_SAMRT_DIV       0x19    
#define MPU6050_REG_CFG             0x1A	  //                                                                              
#define MPU6050_REG_GYRO_CFG        0x1B	  //                                   


#define MPU6050_REG_GYRO_XH         0x43    

#define MPU6050_REG_TEMPH           0x41    


#define MPU6050_REG_FIFO_CNTH       0x72    
#define MPU6050_REG_FIFO_CNTL       0x73    
#define MPU6050_REG_FIFO_DATA       0x74    
#define MPU6050_REG_FIFO_CTL        0x6A    
#define MPU6050_REG_PWR_CTL	        0x6B    
#define MPU6050_REG_PWR_CTL2        0x6C


/*                                */ 

#define MPU6050_FIFO_GYROX_EN       0x40	//                                
#define MPU6050_FIFO_GYROY_EN       0x20	//                                
#define MPU6050_FIFO_GYROZ_EN       0x10	//                                

#define MPU6050_AUX_VDDIO_DIS       0x00	//                                                                

//                    
#define MPU6050_EXT_SYNC			0x03	//                                                                                        
#define MPU6050_SYNC_GYROX			0x02

//                         
#define MPU6050_FS_RANGE			0x03	//                                            
#define MPU6050_FS_1000				0x02


#define MPU6050_FS_1000_LSB			33
#define MPU6050_FS_MAX_LSB			131

#define MPU6050_RATE_1K_LPFB_188HZ	0x01
#define MPU6050_RATE_1K_LPFB_256HZ	0x00

#define MPU6050_FIFO_EN				0x40	//                                     

#define MPU6050_FIFO_RST			0x40    //                   

#define MPU6050_SLEEP               0x40	//                           


#define MPU6050_SUCCESS             0
#define MPU6050_ERR_I2C             -1
#define MPU6050_ERR_STATUS          -3
#define MPU6050_ERR_SETUP_FAILURE   -4
#define MPU6050_ERR_GETGSENSORDATA  -5
#define MPU6050_ERR_IDENTIFICATION  -6


#define MPU6050_BUFSIZE 60

//                                      
//                  
#define DEGREE_TO_RAD	7506

#endif //         

