#include <linux/types.h>
#include <cust_baro.h>
#include <mach/mt_pm_ldo.h>


/*                                                                           */
static struct baro_hw cust_baro_hw = {
    .i2c_num = 1,
    .direction = 0,
    .power_id = MT65XX_POWER_NONE,  /*                   */
    .power_vol= VOL_DEFAULT,        /*                   */
    .firlen = 32, //                                                                 
    .is_batch_supported = false,
};
/*                                                                           */
struct baro_hw* get_cust_baro_hw(void) 
{
    return &cust_baro_hw;
}
