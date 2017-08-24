#ifndef __CUST_ALSPS_H__
#define __CUST_ALSPS_H__

#include <linux/types.h>

#define C_CUST_ALS_LEVEL    16
#define C_CUST_I2C_ADDR_NUM 4

#define MAX_THRESHOLD_HIGH 0xffff
#define MIN_THRESHOLD_LOW 0x0

struct alsps_hw {
    int i2c_num;                                    /*                              */
    int power_id;                                   /*                                    */
    int power_vol;                                  /*                                         */
	int polling_mode;                               /*                                     */
	int polling_mode_ps;                               /*                                     */
	int polling_mode_als;                               /*                                     */
    unsigned char   i2c_addr[C_CUST_I2C_ADDR_NUM];  /*                                                          */
    unsigned int    als_level[C_CUST_ALS_LEVEL-1];  /*                                                                             */
    unsigned int    als_value[C_CUST_ALS_LEVEL];    /*                                    */
    unsigned int    ps_threshold;                   /*                                     */
	unsigned int    als_window_loss;                /*                    */
	unsigned int    ps_threshold_high;
	unsigned int    ps_threshold_low;
	unsigned int    als_threshold_high;
	unsigned int    als_threshold_low;
    int als_power_vio_id;                                   /*                                    */
    int als_power_vio_vol;                                  /*                                         */
    int ps_power_vdd_id;                                   /*                                   */
    int ps_power_vdd_vol;                                  /*                                        */
    int ps_power_vio_id;                                   /*                                   */
    int ps_power_vio_vol;                                  /*                                        */
    int power_lp_mode_ctrl;                                 /*                                                                     */
    bool is_batch_supported_ps;
    bool is_batch_supported_als;
};

extern struct alsps_hw* get_cust_alsps_hw(void);
#if defined(CONFIG_ROHM_RPR0521)
extern struct alsps_hw* rohm_get_cust_alsps_hw(void);
#endif

__weak int pmic_ldo_suspend_enable(int enable);
#endif
