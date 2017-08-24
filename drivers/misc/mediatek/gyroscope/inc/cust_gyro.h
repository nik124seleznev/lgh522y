#ifndef __CUST_GYRO_H__
#define __CUST_GYRO_H__

#include <linux/types.h>

#define GYRO_CUST_I2C_ADDR_NUM 2

struct gyro_hw {
    unsigned short addr;
    int i2c_num;    /*                                */
    int direction;  /*                             */
    int power_id;   /*                                                                         */
    int power_vol;  /*                                      */
    int firlen;     /*                                 */
    int (*power)(struct gyro_hw *hw, unsigned int on, char *devname);
    unsigned char	i2c_addr[GYRO_CUST_I2C_ADDR_NUM]; /*                                                                                     */
    int power_vio_id;   /*                                                                         */
    int power_vio_vol;  /*                                      */
    bool is_batch_supported;
};

extern struct gyro_hw* get_cust_gyro_hw(void);
#endif 
