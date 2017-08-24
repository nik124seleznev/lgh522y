#ifndef __CUST_BARO_H__
#define __CUST_BARO_H__

#include <linux/types.h>

#define B_CUST_I2C_ADDR_NUM 2

struct baro_hw {
    int i2c_num;    /*                                */
    int direction;  /*                             */
    int power_id;   /*                                                                         */
    int power_vol;  /*                                      */
    int firlen;     /*                                 */
    int (*power)(struct baro_hw *hw, unsigned int on, char *devname);
    unsigned char	i2c_addr[B_CUST_I2C_ADDR_NUM]; /*                                                                                     */
    bool is_batch_supported;
};

extern struct baro_hw* get_cust_baro_hw(void);
#endif 
