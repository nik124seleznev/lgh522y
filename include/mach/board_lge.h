#ifndef __ASM_ARCH_MTK_BOARD_LGE_H
#define __ASM_ARCH_MTK_BOARD_LGE_H


typedef enum {
    HW_REV_EVB1 = 0,
    HW_REV_EVB2,
    HW_REV_A,
    HW_REV_B,
    HW_REV_C,
    HW_REV_D,
    HW_REV_E,
    HW_REV_F,
    HW_REV_G,
    HW_REV_H,
    HW_REV_I,
    HW_REV_1_0,
    HW_REV_1_1,
    HW_REV_1_2,
    HW_REV_1_3,
    HW_REV_MAX
} hw_rev_type;

typedef enum {
    LT_CABLE_56K = 6,
    LT_CABLE_130K,
    USB_CABLE_400MA,
    USB_CABLE_DTC_500MA,
    ABNORMAL_USB_CABLE_400MA,
    LT_CABLE_910K,
    NO_INIT_CABLE,
} usb_cable_type;

hw_rev_type lge_get_board_revno(void);
usb_cable_type lge_get_board_cable(void);
/*                                                                                     */
#if defined(CONFIG_PRE_SELF_DIAGNOSIS)
int lge_pre_self_diagnosis(char *drv_bus_code, int func_code, char *dev_code, char *drv_code, int errno);
#endif
#define AUXADC_PCB_REV_CHANNEL	1
extern int get_pcb_revision_voltage(void);
/*              */

#if defined(CONFIG_PRE_SELF_DIAGNOSIS)
int lge_pre_self_diagnosis(char *drv_bus_code, int func_code, char *dev_code, char *drv_code, int errno);

struct pre_selfd_platform_data {
    int (*set_values) (int r, int g, int b);
    int (*get_values) (int *r, int *g, int *b);
};
#endif

enum lge_laf_mode_type {
    LGE_LAF_MODE_NORMAL = 0,
    LGE_LAF_MODE_LAF,
};

enum lge_laf_mode_type lge_get_laf_mode(void);

/*                                                                         */
#ifdef CONFIG_LGE_CABLE_ID_DETECT
typedef struct {
    int adc_min;
    int adc_max;
    int cable_type;
} usb_cable_adc_type;

typedef enum {
    _56K    = 0,
    _130K   = 1,
    _910K   = 2,
    _USER   = 3,
    CABLE_MAX,
} usb_Cable_enum_type;

extern int IMM_auxadc_GetOneChannelValue(int dwChannel, int data[4], int* rawdata);
int get_usb_type(void);

#if defined(CONFIG_MACH_MT6752)
#define AUXADC_USB_ID_CHANNEL 12
#elif defined(CONFIG_MACH_MT6582)
#define AUXADC_USB_ID_CHANNEL 0
#else
#error USB_ID ADC port is not defined!
#endif

/*                                                                                   */
extern int g_AtCmdChargeMode;//             
extern int g_AtCmdUsbId;     //              
extern int g_AtCmdUsbAdc;    //              
/*              */

#endif
/*              */
#if defined(CONFIG_LGE_HANDLE_PANIC) || defined(CONFIG_LGE_HIDDEN_RESET)
//                                   
//                                   
//                                   
//                                   
//                                   
//                                   
#if defined(CONFIG_MACH_MT6752)
#define LGE_BSP_MEM_BASE_ADDR           0x40000000U
#else
#define LGE_BSP_MEM_BASE_ADDR			0x80000000
#endif

#if defined (TARGET_MT6582_Y50) /*                */
extern unsigned long LGE_BSP_MEM_MAX_PHY_ADDR;
#else
#define LGE_BSP_MEM_MAX_PHY_ADDR        (LGE_BSP_MEM_BASE_ADDR + (1024 * SZ_1M)) //            
#endif

#define LGE_BSP_PRELOADER_MEM_SIZE      (1 * SZ_1M) //                                         
#define LGE_BSP_RESERVED_MEM_SIZE       (1 * SZ_1M) //                                         
#define LGE_BSP_LK_MEM_SIZE             (1 * SZ_1M) //                                         

#define LGE_BSP_RESERVED_MEM_PHY_ADDR   (LGE_BSP_MEM_MAX_PHY_ADDR - LGE_BSP_LK_MEM_SIZE - LGE_BSP_RESERVED_MEM_SIZE) //            
#define LGE_BSP_RAM_CONSOLE_PHY_ADDR    (LGE_BSP_RESERVED_MEM_PHY_ADDR) //            
#define LGE_BSP_RAM_CONSOLE_SIZE        (124 * 2 * SZ_1K) //           

#define LGE_BSP_CRASH_LOG_PHY_ADDR      (LGE_BSP_RAM_CONSOLE_PHY_ADDR + LGE_BSP_RAM_CONSOLE_SIZE) //            
#define LGE_BSP_CRASH_LOG_SIZE          (4 * SZ_1K) //            

#define LGE_CRASH_CTX_BUF_PHY_ADDR      (LGE_BSP_CRASH_LOG_PHY_ADDR + LGE_BSP_CRASH_LOG_SIZE) //           
#define LGE_CRASH_CTX_BUF_SIZE          (1 * SZ_1K) //           

#define LGE_BSP_HIDDEN_RESET_PHY_ADDR   (LGE_CRASH_CTX_BUF_PHY_ADDR + LGE_CRASH_CTX_BUF_SIZE) //            
#define LGE_BSP_HIDDEN_RESET_SIZE       (LGE_BSP_RESERVED_MEM_SIZE - LGE_BSP_RAM_CONSOLE_SIZE - LGE_BSP_CRASH_LOG_SIZE - LGE_CRASH_CTX_BUF_SIZE)

//                                                                                     
//                                        
#define LGE_BOOT_REASON_MAGIC_CODE          0x1234ABCD
//                  
//                                              
#define LGE_BOOT_KERNEL_CRASH               0xFF0001FF
#define LGE_BOOT_HIDDEN_RESET_REBOOT        0xFF0002FF
#define LGE_BOOT_BNR_RECOVERY_MODE_REBOOT   0x77665555
/*                                                                                         */
#define LGE_BOOT_NORMAL_POWER_OFF           0xFF0003FF
/*                                                                                         */
/*                                                                                        */
#define LGE_BOOT_DLOAD_REBOOT               0x6C616664
#define LGE_BOOT_LAF_RESTART_REBOOT         0x6F656D52
/*                                                                                        */
#define LGE_BOOT_INIT_BOOT_REASON           0x00000000
//                                          
#define LGE_BOOT_FOTA_REBOOT                0x77665566
//                                          

int lge_save_boot_reason(unsigned long reason, unsigned long extra1, unsigned long extra2);
unsigned long lge_get_boot_reason(unsigned long *pExtra1, unsigned long * pExtra2);
#endif

//                                              
#define FACTORY_PID 0x6000
#define LGE_FACTORY_CABLE_TYPE 1
#define MAX_IMEI_LEN 19
#define LGE_PIF_CABLE 2
#define LGE_130K_CABLE 3

#if 1 //                        
typedef enum LGBmCableIdTag
{
	USB_CABLE_ID_NONE  = 0,
	USB_CABLE_ID_OPEN,
	USB_CABLE_ID_56K,
	USB_CABLE_ID_130K,
	USB_CABLE_ID_180K,
	USB_CABLE_ID_910K,
	USB_CABLE_ID_UNKNOWN,

	USB_CABLE_ID_MAX
}
LGBmCableId;
#else
typedef enum {
	DEVICE_NONE,   //  
	DEVICE_OPEN_CABLE,   //  
	DEVICE_FACTORY_UART_CABLE,   //     
	DEVICE_FACTORY_USB_CABLE,  //    
	DEVICE_FACTORY_DOWNLOAD_CABLE,  //     
	DEVIDE_MAX
} USB_ID_TYPE;
extern USB_ID_TYPE readUSB_ID_Value();
#endif

#ifdef CONFIG_USB_G_LGE_MULTIPLE_CONFIGURATION
#define MAC_OS_TYPE	0x02
#define WIN_LINUX_TYPE	0xFF
#endif

//                                                             
//                                                                                     
//                                              

#ifdef CONFIG_USB_G_LGE_MULTIPLE_CONFIGURATION
void lgeusb_set_host_os(u16);
bool lgeusb_get_host_os(void);
#endif

#ifdef CONFIG_LGE_PM_BATTERY_ID
typedef enum {
	BATT_ID_UNKNOWN = 0,
	BATT_ID_DS2704_N,
	BATT_ID_DS2704_L,
	BATT_ID_DS2704_C,
	BATT_ID_ISL6296_N,
	BATT_ID_ISL6296_L,
	BATT_ID_ISL6296_C,
	BATT_ID_RA4301_VC0,
	BATT_ID_RA4301_VC1,
	BATT_ID_RA4301_VC2,
	BATT_ID_SW3800_VC0,
	BATT_ID_SW3800_VC1,
	BATT_ID_SW3800_VC2,
	BATT_ID_MAX,
} BATT_ID;

BATT_ID lge_get_battery_id(void);
char* lge_get_battery_id_str(void);
#endif

enum lge_boot_mode_type {
	LGE_BOOT_MODE_NORMAL = 0,
	LGE_BOOT_MODE_CHARGER,
	LGE_BOOT_MODE_CHARGERLOGO,
	LGE_BOOT_MODE_QEM_56K,
	LGE_BOOT_MODE_QEM_130K,
	LGE_BOOT_MODE_QEM_910K,
	LGE_BOOT_MODE_PIF_56K,
	LGE_BOOT_MODE_PIF_130K,
	LGE_BOOT_MODE_PIF_910K,
	LGE_BOOT_MODE_MINIOS    /*                          */
};
#if defined(CONFIG_LGE_KSWITCH)
int lge_get_kswitch_status(void);
#endif

enum lge_boot_mode_type lge_get_boot_mode(void);
#endif /*                            */
