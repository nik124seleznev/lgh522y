/*                     */
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/mutex.h>
#include <linux/uaccess.h>
#include <linux/syscalls.h>
#include <linux/mtd/nand.h>

#include <asm/irq.h>
#include <asm/io.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/irq.h>
#include <asm/mach/map.h>
#include <asm/mach/time.h>
#include <asm/setup.h>

#include <mach/system.h>
#include <mach/board.h>
#include <mach/hardware.h>
#include <mach/mt_gpio.h>
#include <mach/mt_bt.h>
#include <mach/eint.h>
#include <mach/mtk_rtc.h>
#include <mach/mt_typedefs.h>
//                                 
#include <cust_gpio_usage.h>
#include <cust_eint.h>
#include "board-custom.h"
#if defined(CONFIG_MTK_COMBO) || defined(CONFIG_MTK_COMBO_MODULE)
#include <mach/mtk_wcn_cmb_stub.h>

#endif

#if defined(CONFIG_MTK_WCN_CMB_SDIO_SLOT)

static sdio_irq_handler_t mtk_wcn_cmb_sdio_eirq_handler = NULL;
int mtk_wcn_sdio_irq_flag_set (int falg);
static atomic_t sdio_irq_enable_flag;
static pm_callback_t mtk_wcn_cmb_sdio_pm_cb = NULL;
static void *mtk_wcn_cmb_sdio_pm_data = NULL;
static void *mtk_wcn_cmb_sdio_eirq_data = NULL;
//                                                                       
//                                  

const static u32 mtk_wcn_cmb_sdio_eint_pin = GPIO_WIFI_EINT_PIN;
const static u32 mtk_wcn_cmb_sdio_eint_num = CUST_EINT_WIFI_NUM;
const static u32 mtk_wcn_cmb_sdio_eint_m_eint = GPIO_WIFI_EINT_PIN_M_EINT;
const static u32 mtk_wcn_cmb_sdio_eint_m_gpio = GPIO_WIFI_EINT_PIN_M_GPIO;


    #if (CONFIG_MTK_WCN_CMB_SDIO_SLOT == 0)
		static unsigned char combo_port_pwr_map[4] = {0x0, 0xFF, 0xFF, 0xFF};
		/*
                                                               
                                                          
  */
    #elif (CONFIG_MTK_WCN_CMB_SDIO_SLOT == 1)
		static unsigned char combo_port_pwr_map[4] = {0xFF, 0x0, 0xFF, 0xFF};
		/*
                                                               
                                                          
  */
    #elif (CONFIG_MTK_WCN_CMB_SDIO_SLOT == 2)
		static unsigned char combo_port_pwr_map[4] = {0xFF, 0xFF, 0x0, 0xFF};
		/*
                                                               
                                                          
  */

    #elif (CONFIG_MTK_WCN_CMB_SDIO_SLOT == 3)
		static unsigned char combo_port_pwr_map[4] = {0xFF, 0xFF, 0xFF, 0x0};
		/*
                                                               
                                                          
  */

    #else
        #error "unsupported CONFIG_MTK_WCN_CMB_SDIO_SLOT" CONFIG_MTK_WCN_CMB_SDIO_SLOT
    #endif

#else
#if 0
static sdio_irq_handler_t mt_wifi_irq_handler = NULL;
static pm_message_t mt_wifi_pm_state = { .event = PM_EVENT_HIBERNATE };
static pm_callback_t mt_wifi_pm_cb = NULL;
static void *mt_wifi_pm_data = NULL;
static void *mt_wifi_irq_data = NULL;
static int mt_wifi_pm_late_cb = 0;
#endif
/*                                      */

#ifndef CUST_EINT_WIFI_NUM
#define CUST_EINT_WIFI_NUM          0
#endif

#ifndef CUST_EINT_WIFI_TYPE
#define CUST_EINT_WIFI_TYPE 				0
#endif

#endif
/*                                                                       */
/*                                                                       */
/*                                                                       */
extern kal_bool pmic_chrdet_status(void);

void mt_power_off(void)
{
	printk("mt_power_off\n");

	/*                */
	rtc_bbpu_power_down();

	while (1) {
#if defined(CONFIG_POWER_EXT)
		//   
		printk("EVB without charger\n");
#else	
		//      
		printk("Phone with charger\n");
		if (pmic_chrdet_status() == KAL_TRUE)
			arch_reset(0, "power_off_with_charger");
#endif
    }
}

/*                                                                       */
/*                                                                       */
/*                                                                       */
/*          */
/*                              */
struct mt3326_gps_hardware mt3326_gps_hw = {
    .ext_power_on =  NULL,
    .ext_power_off = NULL,
};

/*                                                                       */
/*                                                                       */
/*                                                                       */
#ifdef CONFIG_MTK_BT_SUPPORT
void mt_bt_power_on(void)
{
    printk(KERN_INFO "+mt_bt_power_on\n");

#if defined(CONFIG_MTK_COMBO) || defined(CONFIG_MTK_COMBO_MODULE)
    /*                    */
    /*
                                                                                     
     */
#else 
    /*                    */
#endif

    printk(KERN_INFO "-mt_bt_power_on\n");
}
EXPORT_SYMBOL(mt_bt_power_on);

void mt_bt_power_off(void)
{
    printk(KERN_INFO "+mt_bt_power_off\n");

#if defined(CONFIG_MTK_COMBO) || defined(CONFIG_MTK_COMBO_MODULE)
    /*                    */
    /*
                                                                                     
     */
#else
    /*                    */
#endif

    printk(KERN_INFO "-mt_bt_power_off\n");
}
EXPORT_SYMBOL(mt_bt_power_off);

int mt_bt_suspend(pm_message_t state)
{
    printk(KERN_INFO "+mt_bt_suspend\n");
    printk(KERN_INFO "-mt_bt_suspend\n");
    return MT_BT_OK;
}

int mt_bt_resume(pm_message_t state)
{
    printk(KERN_INFO "+mt_bt_resume\n");
    printk(KERN_INFO "-mt_bt_resume\n");
    return MT_BT_OK;
}
#endif



#if defined(CONFIG_MTK_WCN_CMB_SDIO_SLOT)
static void mtk_wcn_cmb_sdio_enable_eirq(void)
	{
    mt_eint_unmask(mtk_wcn_cmb_sdio_eint_num);/*                    */
	}
	
static void mtk_wcn_cmb_sdio_disable_eirq(void)
	{
    mt_eint_mask(mtk_wcn_cmb_sdio_eint_num); /*                    */
	}
	
static void mtk_wcn_cmb_sdio_eirq_handler_stub(void)
	{
    if ((NULL != mtk_wcn_cmb_sdio_eirq_handler) && (0 != atomic_read(&sdio_irq_enable_flag))) {
        mtk_wcn_cmb_sdio_eirq_handler(mtk_wcn_cmb_sdio_eirq_data);
	    }
	}
	
static void mtk_wcn_cmb_sdio_request_eirq(sdio_irq_handler_t irq_handler, void *data)
	{
    mtk_wcn_sdio_irq_flag_set (0);
    mtk_wcn_cmb_sdio_eirq_data    = data;
    mtk_wcn_cmb_sdio_eirq_handler = irq_handler;
    #if CUST_EINT_WIFI_DEBOUNCE_EN
    mt_eint_set_hw_debounce(mtk_wcn_cmb_sdio_eint_num, CUST_EINT_WIFI_DEBOUNCE_CN); /*                   */
    #endif
    mt_set_gpio_pull_enable(mtk_wcn_cmb_sdio_eint_pin, GPIO_PULL_ENABLE);
    mt_set_gpio_pull_select(mtk_wcn_cmb_sdio_eint_pin, GPIO_PULL_UP);
    mt_eint_registration(mtk_wcn_cmb_sdio_eint_num/*                   */,
				CUST_EINT_WIFI_TYPE,
        mtk_wcn_cmb_sdio_eirq_handler_stub,
        0);
    mt_eint_mask(mtk_wcn_cmb_sdio_eint_num);/*                   */

	}
	
static void mtk_wcn_cmb_sdio_register_pm(pm_callback_t pm_cb, void *data)
	{
    printk( KERN_INFO "mtk_wcn_cmb_sdio_register_pm (0x%p, 0x%p)\n", pm_cb, data);
    /*                             */
    mtk_wcn_cmb_sdio_pm_cb = pm_cb;
    mtk_wcn_cmb_sdio_pm_data = data;
	}
	
static void mtk_wcn_cmb_sdio_on (int sdio_port_num) {
    pm_message_t state = { .event = PM_EVENT_USER_RESUME };

    printk(KERN_INFO "mtk_wcn_cmb_sdio_on (%d) \n", sdio_port_num);

    /*                      */
    mtk_wcn_cmb_sdio_disable_eirq();
    mt_set_gpio_pull_enable(mtk_wcn_cmb_sdio_eint_pin, GPIO_PULL_DISABLE); /*                    */
    mt_set_gpio_mode(mtk_wcn_cmb_sdio_eint_pin, mtk_wcn_cmb_sdio_eint_m_eint); /*           */

    /*                     */
    if (mtk_wcn_cmb_sdio_pm_cb) {
        //                                                                                                                                  
        mtk_wcn_cmb_sdio_pm_cb(state, mtk_wcn_cmb_sdio_pm_data);
    }
    else {
        printk(KERN_WARNING "mtk_wcn_cmb_sdio_on no sd callback!!\n");
    }
}
	
static void mtk_wcn_cmb_sdio_off (int sdio_port_num) {
    pm_message_t state = { .event = PM_EVENT_USER_SUSPEND };

    printk(KERN_INFO "mtk_wcn_cmb_sdio_off (%d) \n", sdio_port_num);

    /*                     */
    if (mtk_wcn_cmb_sdio_pm_cb) {
        //                                                                                                                                 
        mtk_wcn_cmb_sdio_pm_cb(state, mtk_wcn_cmb_sdio_pm_data);
    }
    else {
        printk(KERN_WARNING "mtk_wcn_cmb_sdio_off no sd callback!!\n");
    }

    /*                      */
    mtk_wcn_cmb_sdio_disable_eirq();
    /*                                                             */
    mt_set_gpio_mode(mtk_wcn_cmb_sdio_eint_pin, mtk_wcn_cmb_sdio_eint_m_gpio); /*           */
    mt_set_gpio_dir(mtk_wcn_cmb_sdio_eint_pin, GPIO_DIR_IN);
    mt_set_gpio_pull_select(mtk_wcn_cmb_sdio_eint_pin, GPIO_PULL_UP);
    mt_set_gpio_pull_enable(mtk_wcn_cmb_sdio_eint_pin, GPIO_PULL_ENABLE);
}
	
int board_sdio_ctrl (unsigned int sdio_port_num, unsigned int on) {
#if defined(CONFIG_MTK_WCN_CMB_SDIO_SLOT)
			sdio_port_num = CONFIG_MTK_WCN_CMB_SDIO_SLOT;
			printk(KERN_WARNING "mt_combo_sdio_ctrl: force set sdio port to (%d)\n", sdio_port_num);
#endif
    if ((sdio_port_num >= 4) || (combo_port_pwr_map[sdio_port_num] == 0xFF) ) {
        /*                                          */
        printk(KERN_WARNING "mt_mtk_wcn_cmb_sdio_ctrl invalid port(%d, %d)\n", sdio_port_num, combo_port_pwr_map[sdio_port_num]);
        return -1;
    }
    /*                                                                           */

    if (!combo_port_pwr_map[sdio_port_num] && on) {
    	  printk(KERN_INFO  "board_sdio_ctrl force off before on\n");
        mtk_wcn_cmb_sdio_off(sdio_port_num);
        combo_port_pwr_map[sdio_port_num] = 0;
        /*           */
        mtk_wcn_cmb_sdio_on(sdio_port_num);
        combo_port_pwr_map[sdio_port_num] = 1;
    }
    else if (combo_port_pwr_map[sdio_port_num] && !on) {
        /*           */
        mtk_wcn_cmb_sdio_off(sdio_port_num);
        combo_port_pwr_map[sdio_port_num] = 0;
    }
    else {
        return -2;
    }
    return 0;
}
EXPORT_SYMBOL(board_sdio_ctrl);
int mtk_wcn_sdio_irq_flag_set (int flag)
{

    if (0 != flag)
    {
        atomic_set(&sdio_irq_enable_flag, 1);
    }
    else
    {
        atomic_set(&sdio_irq_enable_flag, 0);
    }
    printk(KERN_INFO  "sdio_irq_enable_flag:%d\n", atomic_read(&sdio_irq_enable_flag));

    return atomic_read(&sdio_irq_enable_flag);
}
EXPORT_SYMBOL(mtk_wcn_sdio_irq_flag_set);

#endif
#if defined(CONFIG_WLAN)
#if !defined(CONFIG_MTK_WCN_CMB_SDIO_SLOT)
#if 0
static void mt_wifi_enable_irq(void)
{
    mt_eint_unmask(CUST_EINT_WIFI_NUM);
}

static void mt_wifi_disable_irq(void)
{
    mt_eint_mask(CUST_EINT_WIFI_NUM);
}

static void mt_wifi_eirq_handler(void)
{
    if (mt_wifi_irq_handler) {
        mt_wifi_irq_handler(mt_wifi_irq_data);
    }
}

static void mt_wifi_request_irq(sdio_irq_handler_t irq_handler, void *data)
{
		#if CUST_EINT_WIFI_SENSITIVE
    mt_eint_set_sens(CUST_EINT_WIFI_NUM, CUST_EINT_WIFI_SENSITIVE);
    #endif
    
    #if CUST_EINT_WIFI_DEBOUNCE_EN
    mt_eint_set_hw_debounce(CUST_EINT_WIFI_NUM, CUST_EINT_WIFI_DEBOUNCE_CN);
    #endif
    mt_eint_registration(CUST_EINT_WIFI_NUM,
				CUST_EINT_WIFI_TYPE,
        mt_wifi_eirq_handler,
        0);
    mt_eint_mask(CUST_EINT_WIFI_NUM);

    mt_wifi_irq_handler = irq_handler;
    mt_wifi_irq_data    = data;
}

static void mt_wifi_register_pm(pm_callback_t pm_cb, void *data)
{
    /*                             */
    mt_wifi_pm_cb = pm_cb;
    mt_wifi_pm_data = data;
}
#endif
#endif /*                                               */ 

int mt_wifi_resume(pm_message_t state)
{
    int evt = state.event;

    if (evt != PM_EVENT_USER_RESUME && evt != PM_EVENT_RESUME) {
        return -1;
    }

    /*                                                                            */

#if defined(CONFIG_MTK_COMBO) || defined(CONFIG_MTK_COMBO_MODULE)
    /*                                                          */


#endif

    return 0;
}

int mt_wifi_suspend(pm_message_t state)
{
    int evt = state.event;
#if defined(CONFIG_MTK_WCN_CMB_SDIO_SLOT)
    static int is_1st_suspend_from_boot = 1;
#endif

    if (evt != PM_EVENT_USER_SUSPEND && evt != PM_EVENT_SUSPEND) {
        return -1;
    }

#if defined(CONFIG_MTK_COMBO) || defined(CONFIG_MTK_COMBO_MODULE)
    #if defined(CONFIG_MTK_WCN_CMB_SDIO_SLOT)
    /*                                                          */
    if (is_1st_suspend_from_boot) {
        pm_message_t state = { .event = PM_EVENT_USER_SUSPEND };

        if (mtk_wcn_cmb_sdio_pm_cb) {
            is_1st_suspend_from_boot = 0;
            /*                                         
                                                                 
                                                                                   
                                        

                                                                         
                                                                                
                                                                               
                                                                      

                                                                       
                                                                      
            */
            printk(KERN_INFO "1st mt_wifi_suspend (PM_EVENT_USER_SUSPEND) \n");
            (*mtk_wcn_cmb_sdio_pm_cb)(state, mtk_wcn_cmb_sdio_pm_data);
        }
        else {
            printk(KERN_WARNING "1st mt_wifi_suspend but no sd callback!!\n");
        }
    }
    else {
        /*                                         */
    }
    #endif

#else
#endif
    return 0;
}

void mt_wifi_power_on(void)
{
    pm_message_t state = { .event = PM_EVENT_USER_RESUME };

    (void)mt_wifi_resume(state);
}
EXPORT_SYMBOL(mt_wifi_power_on);

void mt_wifi_power_off(void)
{
    pm_message_t state = { .event = PM_EVENT_USER_SUSPEND };

    (void)mt_wifi_suspend(state);
}
EXPORT_SYMBOL(mt_wifi_power_off);

#endif /*                             */

/*                                                                       */
/*                                                                       */

/*                                                                       */
/*                                                                       */
/*                                                                       */

/*                                                                       */
/*                                                                       */
/*                                                                       */
#define MSDC_SDCARD_FLAG  (MSDC_SYS_SUSPEND | MSDC_WP_PIN_EN | MSDC_CD_PIN_EN | MSDC_REMOVABLE | MSDC_HIGHSPEED)
#define MSDC_SDIO_FLAG    (MSDC_EXT_SDIO_IRQ | MSDC_HIGHSPEED)

#if defined(CFG_DEV_MSDC0)
#if defined(CONFIG_MTK_WCN_CMB_SDIO_SLOT) && (CONFIG_MTK_WCN_CMB_SDIO_SLOT == 0)
    struct msdc_hw msdc0_hw = {	    
		.clk_src        = MSDC_CLKSRC_200MHZ,
	    .cmd_edge       = MSDC_SMPL_FALLING,
	    .rdata_edge     = MSDC_SMPL_FALLING,
	    .wdata_edge     = MSDC_SMPL_FALLING,
	    .clk_drv        = 0,
	    .cmd_drv        = 0,
	    .dat_drv        = 0,
	    .data_pins      = 4,
	    .data_offset    = 0,
	    //                                                                                                                     
	    .flags          = MSDC_SDIO_FLAG,//                                                                                              
	    .dat0rddly		= 0,
			.dat1rddly		= 0,
			.dat2rddly		= 0,
			.dat3rddly		= 0,
			.dat4rddly		= 0,
			.dat5rddly		= 0,
			.dat6rddly		= 0,
			.dat7rddly		= 0,
			.datwrddly		= 0,
			.cmdrrddly		= 0,
			.cmdrddly		= 0,
	    .host_function	= MSDC_SDIO,
	    .boot			= 0,
	    .request_sdio_eirq = mtk_wcn_cmb_sdio_request_eirq,
	    .enable_sdio_eirq  = mtk_wcn_cmb_sdio_enable_eirq,
	    .disable_sdio_eirq = mtk_wcn_cmb_sdio_disable_eirq,
	    .register_pm       = mtk_wcn_cmb_sdio_register_pm,
	};
	#else
	struct msdc_hw msdc0_hw = {
	    .clk_src        = MSDC_CLKSRC_200MHZ,
	    .cmd_edge       = MSDC_SMPL_FALLING,
		.rdata_edge 	= MSDC_SMPL_FALLING,
		.wdata_edge 	= MSDC_SMPL_FALLING,
	    .clk_drv        = 4,
	    .cmd_drv        = 2,
	    .dat_drv        = 2,
	    .data_pins      = 8,
	    .data_offset    = 0,
	    .flags          = MSDC_SYS_SUSPEND | MSDC_HIGHSPEED | MSDC_UHS1 | MSDC_DDR,
	    .dat0rddly		= 0xa,
			.dat1rddly		= 0,
			.dat2rddly		= 0,
			.dat3rddly		= 0,
			.dat4rddly		= 0,
			.dat5rddly		= 0,
			.dat6rddly		= 0,
			.dat7rddly		= 0,
			.datwrddly		= 0,
			.cmdrrddly		= 0,
			.cmdrddly		= 0,
	    .host_function	= MSDC_EMMC,
	    .boot			= MSDC_BOOT_EN,
	};
	#endif
#endif
#if defined(CFG_DEV_MSDC1)
#if defined(CONFIG_MTK_WCN_CMB_SDIO_SLOT) && (CONFIG_MTK_WCN_CMB_SDIO_SLOT == 1)
struct msdc_hw msdc1_hw = {
    .clk_src        = MSDC_CLKSRC_200MHZ,
    .cmd_edge       = MSDC_SMPL_FALLING,
    .rdata_edge     = MSDC_SMPL_FALLING,
    .wdata_edge     = MSDC_SMPL_FALLING,
    .clk_drv        = 4,
    .cmd_drv        = 3,
    .dat_drv        = 3,
    .clk_drv_sd_18	= 4,               /*             */
    .cmd_drv_sd_18	= 4,
    .dat_drv_sd_18	= 4,
    .clk_drv_sd_18_sdr50	= 4,       /*            */
    .cmd_drv_sd_18_sdr50	= 4,
    .dat_drv_sd_18_sdr50	= 4,
    .clk_drv_sd_18_ddr50	= 4,       /*            */
    .cmd_drv_sd_18_ddr50	= 4,
    .dat_drv_sd_18_ddr50	= 4,
    .data_pins      = 4,
    .data_offset    = 0,
    .flags          = MSDC_SDIO_FLAG,
    .dat0rddly		= 0,
    .dat1rddly		= 0,
    .dat2rddly		= 0,
    .dat3rddly		= 0,
    .dat4rddly		= 0,
    .dat5rddly		= 0,
    .dat6rddly		= 0,
    .dat7rddly		= 0,
    .datwrddly		= 0,
    .cmdrrddly		= 0,
    .cmdrddly       = 0,
    .host_function	= MSDC_SDIO,
    .boot           = 0,
    .cd_level       = MSDC_CD_LOW,
};


#else

struct msdc_hw msdc1_hw = {
    .clk_src        = MSDC_CLKSRC_200MHZ,
    .cmd_edge       = MSDC_SMPL_FALLING,
    .rdata_edge     = MSDC_SMPL_FALLING,
    .wdata_edge     = MSDC_SMPL_FALLING,
    .clk_drv        = 4,
    .cmd_drv        = 3,
    .dat_drv        = 3,
    .clk_drv_sd_18	= 4,               /*             */
    .cmd_drv_sd_18	= 4,
    .dat_drv_sd_18	= 4,
    .clk_drv_sd_18_sdr50	= 4,       /*            */
    .cmd_drv_sd_18_sdr50	= 4,
    .dat_drv_sd_18_sdr50	= 4,
    .clk_drv_sd_18_ddr50	= 4,       /*            */
    .cmd_drv_sd_18_ddr50	= 4,
    .dat_drv_sd_18_ddr50	= 4,
    .data_pins      = 4,
    .data_offset    = 0,
#ifdef CUST_EINT_MSDC1_INS_NUM
    .flags          = MSDC_SYS_SUSPEND | MSDC_WP_PIN_EN | MSDC_CD_PIN_EN | MSDC_REMOVABLE | MSDC_HIGHSPEED | MSDC_UHS1 |MSDC_DDR,
    
#else
    .flags          = MSDC_SYS_SUSPEND | MSDC_WP_PIN_EN | MSDC_HIGHSPEED | MSDC_UHS1 |MSDC_DDR,   
#endif
  	  .dat0rddly		= 0,
			.dat1rddly		= 0,
			.dat2rddly		= 0,
			.dat3rddly		= 0,
			.dat4rddly		= 0,
			.dat5rddly		= 0,
			.dat6rddly		= 0,
			.dat7rddly		= 0,
			.datwrddly		= 0,
			.cmdrrddly		= 0,
			.cmdrddly		= 0,
    .host_function	= MSDC_SD,
    .boot			= 0,
    .cd_level		= MSDC_CD_HIGH,
};
#endif
#endif
#if defined(CFG_DEV_MSDC2)
    #if defined(CONFIG_MTK_WCN_CMB_SDIO_SLOT) && (CONFIG_MTK_WCN_CMB_SDIO_SLOT == 2)
    /*                                                   */
	struct msdc_hw msdc2_hw = {	    
		.clk_src        = MSDC_CLKSRC_200MHZ,
	    .cmd_edge       = MSDC_SMPL_FALLING,
	    .rdata_edge     = MSDC_SMPL_FALLING,
    	.wdata_edge     = MSDC_SMPL_FALLING,
	    .clk_drv        = 0,
    	.cmd_drv        = 0,
    	.dat_drv        = 0,
	    .data_pins      = 4,
	    .data_offset    = 0,
	    //                                                                                                                     
	    .flags          = MSDC_SDIO_FLAG,//                                                                    
	    .dat0rddly		= 0,
			.dat1rddly		= 0,
			.dat2rddly		= 0,
			.dat3rddly		= 0,
			.dat4rddly		= 0,
			.dat5rddly		= 0,
			.dat6rddly		= 0,
			.dat7rddly		= 0,
			.datwrddly		= 0,
			.cmdrrddly		= 0,
			.cmdrddly		= 0,
	    .host_function	= MSDC_SDIO,
	    .boot			= 0,
	    .request_sdio_eirq = mtk_wcn_cmb_sdio_request_eirq,
	    .enable_sdio_eirq  = mtk_wcn_cmb_sdio_enable_eirq,
	    .disable_sdio_eirq = mtk_wcn_cmb_sdio_disable_eirq,
	    .register_pm       = mtk_wcn_cmb_sdio_register_pm,
	};
    #else

#if 0
struct msdc_hw msdc2_hw = {
    .clk_src        = MSDC_CLKSRC_200MHZ,
    .cmd_edge       = MSDC_SMPL_FALLING,
    .rdata_edge     = MSDC_SMPL_FALLING,
    .wdata_edge     = MSDC_SMPL_FALLING,
    .clk_drv        = 3,
    .cmd_drv        = 2,
    .dat_drv        = 2,
    .clk_drv_sd_18	= 4,
    .cmd_drv_sd_18	= 3,
    .dat_drv_sd_18	= 3,
    .data_pins      = 4,
    .data_offset    = 0,
    .flags          = MSDC_SYS_SUSPEND | MSDC_WP_PIN_EN | MSDC_CD_PIN_EN | MSDC_REMOVABLE | MSDC_HIGHSPEED,
  	  .dat0rddly		= 0,
			.dat1rddly		= 0,
			.dat2rddly		= 0,
			.dat3rddly		= 0,
			.dat4rddly		= 0,
			.dat5rddly		= 0,
			.dat6rddly		= 0,
			.dat7rddly		= 0,
			.datwrddly		= 0,
			.cmdrrddly		= 0,
			.cmdrddly		= 0,
    .host_function	= MSDC_SD,
    .boot			= 0,
    .cd_level		= MSDC_CD_LOW,
};
#endif
    #endif
#endif
#if defined(CFG_DEV_MSDC3)
	#if defined(CONFIG_MTK_WCN_CMB_SDIO_SLOT) && (CONFIG_MTK_WCN_CMB_SDIO_SLOT == 3)
    /*                                                   */
    struct msdc_hw msdc3_hw = {
	    .clk_src        = MSDC_CLKSRC_200MHZ,
	    .cmd_edge       = MSDC_SMPL_FALLING,
	    .rdata_edge     = MSDC_SMPL_FALLING,
    	.wdata_edge     = MSDC_SMPL_FALLING,
	    .clk_drv        = 0,
	    .cmd_drv        = 0,
	    .dat_drv        = 0,
	    .data_pins      = 4,
	    .data_offset    = 0,
	    //                                                                                                                     
	    .flags          = MSDC_SDIO_FLAG,//                                                                    
	    .dat0rddly		= 0,
			.dat1rddly		= 0,
			.dat2rddly		= 0,
			.dat3rddly		= 0,
			.dat4rddly		= 0,
			.dat5rddly		= 0,
			.dat6rddly		= 0,
			.dat7rddly		= 0,
			.datwrddly		= 0,
			.cmdrrddly		= 0,
			.cmdrddly		= 0,
	    .host_function	= MSDC_SDIO,
	    .boot			= 0,
	    .request_sdio_eirq = mtk_wcn_cmb_sdio_request_eirq,
	    .enable_sdio_eirq  = mtk_wcn_cmb_sdio_enable_eirq,
	    .disable_sdio_eirq = mtk_wcn_cmb_sdio_disable_eirq,
	    .register_pm       = mtk_wcn_cmb_sdio_register_pm,
	};
	#if 0
    
	struct msdc_hw msdc3_hw = {
	    .clk_src        = 1,
	    .cmd_edge       = MSDC_SMPL_FALLING,
	    .data_edge      = MSDC_SMPL_FALLING,
	    .clk_drv        = 4,
	    .cmd_drv        = 4,
	    .dat_drv        = 4,
	    .data_pins      = 4,
	    .data_offset    = 0,
	    .flags          = MSDC_SDCARD_FLAG,
	    .dat0rddly		= 0,
			.dat1rddly		= 0,
			.dat2rddly		= 0,
			.dat3rddly		= 0,
			.dat4rddly		= 0,
			.dat5rddly		= 0,
			.dat6rddly		= 0,
			.dat7rddly		= 0,
			.datwrddly		= 0,
			.cmdrrddly		= 0,
			.cmdrddly		= 0,
	};
	#endif
	#endif
#endif
#if defined(CFG_DEV_MSDC4)
    #if defined(CONFIG_MTK_WCN_CMB_SDIO_SLOT) && (CONFIG_MTK_WCN_CMB_SDIO_SLOT == 4)
    struct msdc_hw msdc4_hw = {	    
		.clk_src        = MSDC_CLKSRC_200MHZ,
	    .cmd_edge       = MSDC_SMPL_FALLING,
	    .rdata_edge     = MSDC_SMPL_FALLING,
    	.wdata_edge     = MSDC_SMPL_FALLING,
	    .clk_drv        = 0,
	    .cmd_drv        = 0,
	    .dat_drv        = 0,
	    .data_pins      = 4,
	    .data_offset    = 0,
	    //                                                                                                                     
	    .flags          = MSDC_SDIO_FLAG,//                                                                                              
	    .dat0rddly		= 0,
			.dat1rddly		= 0,
			.dat2rddly		= 0,
			.dat3rddly		= 0,
			.dat4rddly		= 0,
			.dat5rddly		= 0,
			.dat6rddly		= 0,
			.dat7rddly		= 0,
			.datwrddly		= 0,
			.cmdrrddly		= 0,
			.cmdrddly		= 0,
	    .host_function	= MSDC_SDIO,
	    .boot			= 0,
	    .request_sdio_eirq = mtk_wcn_cmb_sdio_request_eirq,
	    .enable_sdio_eirq  = mtk_wcn_cmb_sdio_enable_eirq,
	    .disable_sdio_eirq = mtk_wcn_cmb_sdio_disable_eirq,
	    .register_pm       = mtk_wcn_cmb_sdio_register_pm,
	};
	#else
	struct msdc_hw msdc4_hw = {
	    .clk_src        = MSDC_CLKSRC_200MHZ,
	    .cmd_edge       = MSDC_SMPL_FALLING,
	    .rdata_edge     = MSDC_SMPL_FALLING,
    	.wdata_edge     = MSDC_SMPL_FALLING,
	    .clk_drv        = 1,
	    .cmd_drv        = 0,
	    .dat_drv        = 0,
	    .data_pins      = 8,
	    .data_offset    = 0,
	    .flags          = MSDC_SYS_SUSPEND | MSDC_HIGHSPEED | MSDC_UHS1 |MSDC_DDR,
	    .dat0rddly		= 0,
			.dat1rddly		= 0,
			.dat2rddly		= 0,
			.dat3rddly		= 0,
			.dat4rddly		= 0,
			.dat5rddly		= 0,
			.dat6rddly		= 0,
			.dat7rddly		= 0,
			.datwrddly		= 0,
			.cmdrrddly		= 0,
			.cmdrddly		= 0,
	    .host_function	= MSDC_EMMC,
	    .boot			= MSDC_BOOT_EN,
	};
	#endif
#endif

/*                    */
#if defined(CONFIG_MTK_MTD_NAND)
struct mt6575_nand_host_hw mt6575_nand_hw = {
    .nfi_bus_width          = 8,
	.nfi_access_timing		= NFI_DEFAULT_ACCESS_TIMING,
	.nfi_cs_num				= NFI_CS_NUM,
	.nand_sec_size			= 512,
	.nand_sec_shift			= 9,
	.nand_ecc_size			= 2048,
	.nand_ecc_bytes			= 32,
	.nand_ecc_mode			= NAND_ECC_HW,
};
#endif
