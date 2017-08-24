#include <linux/types.h>
#include <mach/mt_pm_ldo.h>
#include <cust_gyro.h>

/*                                                                           */
/*
                                                                       
 
                                          
                 
           
                                                               
        
                                                   
 
*/
/*                                                                           */
static struct gyro_hw cust_gyro_hw = {
    .i2c_num = 2,
    .direction = 3,
    .power_id = MT65XX_POWER_NONE,  /*                   */
    .power_vol= VOL_DEFAULT,        /*                   */
    .firlen = 16,                   /*                                 */
   //                          
};
/*                                                                           */
struct gyro_hw* get_cust_gyro_hw(void) 
{
    return &cust_gyro_hw;
}
