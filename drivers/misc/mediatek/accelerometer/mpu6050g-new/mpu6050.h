#ifndef MPU6050_H
#define MPU6050_H

#include <linux/ioctl.h>

#define MPU6050_I2C_SLAVE_ADDR		0xD0


/*                                                                */
#define MPU6050_REG_DEVID			0x75
#define	MPU6050_REG_BW_RATE			0x1A
#define MPU6050_REG_POWER_CTL                   0x6B 
#define MPU6050_REG_POWER_CTL2                  0x6C
#define MPU6050_REG_INT_ENABLE		0x38 
#define MPU6050_REG_DATA_FORMAT		0x1C 
#define MPU6050_REG_DATAX0			0x3B 
#define MPU6050_REG_DATAY0			0x3D
#define MPU6050_REG_DATAZ0			0x3F
#define MPU6050_REG_RESET               0x68

/*                */ 
#define MPU6050_FIXED_DEVID         0x68 //        
													
                                           //           
#define MPU6050_BW_260HZ            0x00   //      
#define MPU6050_BW_184HZ            0x01   //        
#define MPU6050_BW_94HZ             0x02   //      
#define MPU6050_BW_44HZ             0x03   //      
#define MPU6050_BW_21HZ             0x04   //      
#define MPU6050_BW_10HZ             0x05   //      
#define MPU6050_BW_5HZ              0x06   //      

#define MPU6050_DEV_RESET           0x80

//                               
#define MPU6050_RANGE_2G			(0x00 << 3)
#define MPU6050_RANGE_4G			(0x01 << 3)
#define MPU6050_RANGE_8G			(0x02 << 3)
#define MPU6050_RANGE_16G			(0x03 << 3)
//                                      


#define MPU6050_SLEEP				0x40	//                           



//                      
#define MPU6050_SUCCESS                     0
#define MPU6050_ERR_I2C                     -1
#define MPU6050_ERR_STATUS                  -3
#define MPU6050_ERR_SETUP_FAILURE           -4
#define MPU6050_ERR_GETGSENSORDATA          -5
#define MPU6050_ERR_IDENTIFICATION          -6



#define MPU6050_BUFSIZE				256  

#endif

