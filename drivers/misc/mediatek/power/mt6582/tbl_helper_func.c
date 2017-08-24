#include <linux/xlog.h>
#include <mach/mt_typedefs.h>

#ifdef CONFIG_MTK_BQ24196_SUPPORT
#include "bq24196.h"
#endif

#ifdef CONFIG_MTK_BQ24297_SUPPORT
#include "bq24297.h"
#endif

#ifdef MTK_BQ24296_SUPPORT
#include "bq24296.h"
#endif

#ifdef CONFIG_MTK_NCP1851_SUPPORT
#include "ncp1851.h"
#endif

#ifdef MTK_NCP1854_SUPPORT
#include "ncp1854.h"
#endif
/*                                       */
/*                                             */
/*                                                            */

#ifdef CONFIG_USB_MTK_HDRC_HCD
void tbl_charger_otg_vbus(int mode)
{
    xlog_printk(ANDROID_LOG_INFO, "Power/Battery", "[tbl_charger_otg_vbus] mode = %d\n", mode);

    if(mode&0xFF)
    {
#ifdef CONFIG_MTK_BQ24196_SUPPORT
        bq24196_set_chg_config(0x3); //   
        bq24196_set_boost_lim(0x1); //            
        bq24196_set_en_hiz(0x0);
#endif

#ifdef CONFIG_MTK_BQ24297_SUPPORT
        bq24297_set_otg_config(0x1); //   
        bq24297_set_boost_lim(0x1); //            
        bq24297_set_en_hiz(0x0);
#endif

#ifdef MTK_BQ24296_SUPPORT
        bq24296_set_chg_config(0x0); //              
        bq24296_set_otg_config(0x1); //   
        bq24296_set_boostv(0x7); //                    
        bq24296_set_boost_lim(0x1); //            
        bq24296_set_en_hiz(0x0);
#endif

#ifdef CONFIG_MTK_NCP1851_SUPPORT
        ncp1851_set_chg_en(0x0); //               
        ncp1851_set_otg_en(0x1); //          
#endif

#ifdef MTK_NCP1854_SUPPORT
        ncp1854_set_chg_en(0x0); //               
        ncp1854_set_otg_en(0x1); //          
#endif
    }
    else
    {
#ifdef CONFIG_MTK_BQ24196_SUPPORT
        bq24196_set_chg_config(0x0); //                     
#endif

#ifdef CONFIG_MTK_BQ24297_SUPPORT
        bq24297_set_otg_config(0x0); //                     
#endif

#ifdef MTK_BQ24296_SUPPORT
        bq24296_set_otg_config(0x0); //            
        bq24296_set_chg_config(0x0); //               
#endif

#ifdef CONFIG_MTK_NCP1851_SUPPORT
        ncp1851_set_otg_en(0x0);
#endif

#ifdef MTK_NCP1854_SUPPORT
        ncp1854_set_otg_en(0x0);
#endif
    }
};
#endif
