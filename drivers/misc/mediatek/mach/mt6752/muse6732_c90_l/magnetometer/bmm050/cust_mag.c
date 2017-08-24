#include <linux/types.h>
#include <mach/mt_pm_ldo.h>
#include <cust_mag.h>


static struct mag_hw cust_mag_hw = {
    .i2c_num = 1,
    .direction = 6,
    .power_id = MT65XX_POWER_NONE,  /*                   */
    .power_vol= VOL_DEFAULT,        /*                   */
};
struct mag_hw* get_cust_mag_hw(void)
{
    return &cust_mag_hw;
}
