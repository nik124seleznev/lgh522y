#ifndef __CUST_ACC_H__
#define __CUST_ACC_H__

#include <linux/types.h>
#define G_CUST_I2C_ADDR_NUM 2

struct acc_hw {
	int i2c_num;		/*                                */
	int direction;		/*                             */
	int power_id;		/*                                                                              */
	int power_vol;		/*                                          */
	int firlen;		/*                                 */
	int (*power) (struct acc_hw *hw, unsigned int on, char *devname);
	unsigned char i2c_addr[G_CUST_I2C_ADDR_NUM];	/*                                                                                     */
	int power_vio_id;	/*                                                                              */
	int power_vio_vol;	/*                                          */
    bool is_batch_supported;
};

extern struct acc_hw *get_cust_acc_hw(void);
#endif
