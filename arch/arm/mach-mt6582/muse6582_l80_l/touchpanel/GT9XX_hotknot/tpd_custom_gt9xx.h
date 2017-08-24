#ifndef TPD_CUSTOM_GT9XX_H__
#define TPD_CUSTOM_GT9XX_H__

#include <linux/hrtimer.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
//                     

#include <linux/init.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/bitops.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/byteorder/generic.h>
#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif
#include <linux/interrupt.h>
#include <linux/time.h>
#include <linux/rtpm_prio.h>

#include <linux/proc_fs.h>
#include <asm/uaccess.h>

#include <mach/mt_pm_ldo.h>
#include <mach/mt_typedefs.h>
#include <mach/mt_boot.h>
#include <mach/mt_gpio.h>

#include <cust_eint.h>
#include <linux/jiffies.h>

/*                        */

#define TPD_KEY_COUNT   4
#define key_1           60,850              //             
#define key_2           180,850
#define key_3           300,850
#define key_4           420,850

#define TPD_KEYS        {KEY_BACK, KEY_HOME, KEY_MENU, KEY_SEARCH}
#define TPD_KEYS_DIM    {{key_1,50,30},{key_2,50,30},{key_3,50,30},{key_4,50,30}}

struct goodix_ts_data
{
    spinlock_t irq_lock;
    struct i2c_client *client;
    struct input_dev  *input_dev;
    struct hrtimer timer;
    struct work_struct  work;
    struct early_suspend early_suspend;
    s32 irq_is_disable;
    s32 use_irq;
    u16 abs_x_max;
    u16 abs_y_max;
    u8  max_touch_num;
    u8  int_trigger_type;
    u8  green_wake_mode;
    u8  chip_type;
    u8  enter_update;
    u8  gtp_is_suspend;
    u8  gtp_rawdiff_mode;
};

extern u16 show_len;
extern u16 total_len;
extern u8 gtp_rawdiff_mode;

extern int tpd_halt;
extern s32 gtp_send_cfg(struct i2c_client *client);
extern void gtp_reset_guitar(struct i2c_client *client, s32 ms);
extern void gtp_int_sync(s32 ms);
extern u8 gup_init_update_proc(struct i2c_client *client);
extern u8 gup_init_fw_proc(struct i2c_client *client);
extern void mt_eint_unmask(unsigned int line);
extern void mt_eint_mask(unsigned int line);
extern s32 gtp_i2c_read(struct i2c_client *client, u8 *buf, s32 len);
extern s32 gtp_i2c_write(struct i2c_client *client, u8 *buf, s32 len);
extern int i2c_write_bytes(struct i2c_client *client, u16 addr, u8 *txbuf, int len);
extern int i2c_read_bytes(struct i2c_client *client, u16 addr, u8 *rxbuf, int len);
extern s32 gtp_i2c_read_dbl_check(struct i2c_client *client, u16 addr, u8 *rxbuf, int len);
extern u8 wait_hotknot_state;
extern u8 got_hotknot_state;
extern u8 got_hotknot_extra_state;
extern u8 hotknot_paired_flag;
extern wait_queue_head_t bp_waiter;
extern s32 gup_load_hotknot_system(void);

extern unsigned char gtp_default_FW_fl[];
//                                                                             
#define GTP_CUSTOM_CFG        0
#define GTP_DRIVER_SEND_CFG   1       //                                                                          
#define GTP_HAVE_TOUCH_KEY    0
#define GTP_POWER_CTRL_SLEEP  0       //                         
#define GTP_AUTO_UPDATE       0       //                     
#define GTP_CHANGE_X2Y        0				//           
#define GTP_HEADER_FW_UPDATE  0       //                                                                                              
#define GTP_AUTO_UPDATE_CFG   0       //                                                                        

#define GTP_SUPPORT_I2C_DMA   1       //                                                           
#define GTP_COMPATIBLE_MODE   1       //                       

#define GTP_CREATE_WR_NODE    1
#define GTP_ESD_PROTECT       1       //                                         
#define GUP_USE_HEADER_FILE   0
#define GTP_FW_DOWNLOAD       0       //                    
//                          

#define GTP_CONFIG_MIN_LENGTH       186
#define GTP_CONFIG_MAX_LENGTH       240
#define GTP_CHARGER_SWITCH    0       //                                
#define GTP_WITH_PEN          0       
#define GTP_SLIDE_WAKEUP      0 
#define GTP_DBL_CLK_WAKEUP    0       //                                                            
#define HOTKNOT_BLOCK_RW      0

//                     
//                                                                                   
//                  
//                  

#define GTP_DEBUG_ON          0
#define GTP_DEBUG_ARRAY_ON    0
#define GTP_DEBUG_FUNC_ON     0

#define CFG_GROUP_LEN(p_cfg_grp)  (sizeof(p_cfg_grp) / sizeof(p_cfg_grp[0]))
#define FLASHLESS_FLASH_WORKROUND  1

#if GTP_COMPATIBLE_MODE
extern void force_reset_guitar(void);
#endif
//                                                                              
//                                     
/*                                                                              
                                                                            */
#define CTP_CFG_GROUP1 {\
0x4C,0xD0,0x02,0x00,0x05,0x05,0x34,0x10,\
0x01,0x0A,0x19,0x0F,0x50,0x3C,0x03,0x05,\
0x01,0x01,0x00,0x00,0x10,0x11,0x02,0x18,\
0x1A,0x1C,0x14,0x94,0x00,0x0C,0x26,0x00,\
0xFA,0x0E,0x00,0x00,0x00,0xBB,0x02,0x2D,\
0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,\
0x00,0x00,0x00,0x1A,0x5B,0x94,0xC5,0x02,\
0x08,0x00,0x00,0x04,0xA0,0x1D,0x00,0x80,\
0x26,0x00,0x69,0x31,0x00,0x5B,0x3E,0x00,\
0x50,0x50,0x00,0x50,0x00,0x00,0x00,0x00,\
0x57,0x50,0x30,0xFF,0xFF,0x17,0x00,0x00,\
0x00,0x00,0x00,0x01,0x0F,0x19,0x0F,0x1E,\
0x0A,0x00,0x02,0x00,0x00,0x00,0x00,0x00,\
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
0x16,0x18,0x12,0x14,0x0E,0x10,0x0A,0x0C,\
0x06,0x08,0x02,0x04,0xFF,0xFF,0xFF,0xFF,\
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,\
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x28,0x25,\
0x22,0x1F,0x1D,0x0E,0x1E,0x20,0x24,0x26,\
0x0A,0x08,0x06,0x02,0x00,0x01,0x04,0x07,\
0x09,0x0C,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,\
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,\
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,\
0xED,0x01,\
}

//                      

#define CTP_CFG_GROUP1_CHARGER {\
}

//                                               
#define CTP_CFG_GROUP2 {\
    }

//                                               
#define CTP_CFG_GROUP2_CHARGER {\
    }

//                                               
#define CTP_CFG_GROUP3 {\
    }

//                                               
#define CTP_CFG_GROUP3_CHARGER {\
    }

//                                                            
#define CTP_CFG_GROUP4 {\
    }

//                                                            
#define CTP_CFG_GROUP5 {\
    }

//                                                            
#define CTP_CFG_GROUP6 {\
    }


//                                                        
#define GTP_RST_PORT    GPIO_CTP_RST_PIN
#define GTP_INT_PORT    GPIO_CTP_EINT_PIN

#define GTP_GPIO_AS_INPUT(pin)          do{\
                                            if(pin == GPIO_CTP_EINT_PIN)\
                                                mt_set_gpio_mode(pin, GPIO_CTP_EINT_PIN_M_GPIO);\
                                            else\
                                                mt_set_gpio_mode(pin, GPIO_CTP_RST_PIN_M_GPIO);\
                                            mt_set_gpio_dir(pin, GPIO_DIR_IN);\
                                            mt_set_gpio_pull_enable(pin, GPIO_PULL_DISABLE);\
                                        }while(0)
#define GTP_GPIO_AS_INT(pin)            do{\
                                            mt_set_gpio_mode(pin, GPIO_CTP_EINT_PIN_M_EINT);\
                                            mt_set_gpio_dir(pin, GPIO_DIR_IN);\
                                            mt_set_gpio_pull_enable(pin, GPIO_PULL_DISABLE);\
                                        }while(0)
#define GTP_GPIO_GET_VALUE(pin)         mt_get_gpio_in(pin)
#define GTP_GPIO_OUTPUT(pin,level)      do{\
                                            if(pin == GPIO_CTP_EINT_PIN)\
                                                mt_set_gpio_mode(pin, GPIO_CTP_EINT_PIN_M_GPIO);\
                                            else\
                                                mt_set_gpio_mode(pin, GPIO_CTP_RST_PIN_M_GPIO);\
                                            mt_set_gpio_dir(pin, GPIO_DIR_OUT);\
                                            mt_set_gpio_out(pin, level);\
                                        }while(0)
#define GTP_GPIO_REQUEST(pin, label)    gpio_request(pin, label)
#define GTP_GPIO_FREE(pin)              gpio_free(pin)
#define GTP_IRQ_TAB                     {IRQ_TYPE_EDGE_RISING, IRQ_TYPE_EDGE_FALLING, IRQ_TYPE_LEVEL_LOW, IRQ_TYPE_LEVEL_HIGH}

//                                                            
#if GTP_CUSTOM_CFG
#define GTP_MAX_HEIGHT   800
#define GTP_MAX_WIDTH    480
#define GTP_INT_TRIGGER  0    //                  
#else
#define GTP_MAX_HEIGHT   1280
#define GTP_MAX_WIDTH    720
#define GTP_INT_TRIGGER  1
#endif
#define GTP_MAX_TOUCH      5

#define TPD_POWER_SOURCE_CUSTOM	MT6323_POWER_LDO_VGP1
#define VELOCITY_CUSTOM
#define TPD_VELOCITY_CUSTOM_X 15
#define TPD_VELOCITY_CUSTOM_Y 15

//                                                                     
#if GTP_HAVE_TOUCH_KEY
#define GTP_KEY_TAB	 {KEY_MENU, KEY_HOME, KEY_BACK, KEY_SEND}
#endif

//                                                                              
#define GTP_DRIVER_VERSION          "V2.1<2013/12/05>"
#define GTP_I2C_NAME                "Goodix-TS"
#define GT91XX_CONFIG_PROC_FILE     "gt9xx_config"
#define GTP_POLL_TIME               10
#define GTP_ADDR_LENGTH             2
#define GTP_CONFIG_MIN_LENGTH       186
#define GTP_CONFIG_MAX_LENGTH       240
#define FAIL                        0
#define SUCCESS                     1
#define SWITCH_OFF                  0
#define SWITCH_ON                   1

//                                                              
#if GTP_COMPATIBLE_MODE
typedef enum
{
    CHIP_TYPE_GT9  = 0,
    CHIP_TYPE_GT9F = 1,
} CHIP_TYPE_T;
#endif

#define GTP_REG_MATRIX_DRVNUM           0x8069
#define GTP_REG_MATRIX_SENNUM           0x806A
#define GTP_REG_RQST                    0x8043
#define GTP_REG_BAK_REF                 0x99D0
#define GTP_REG_MAIN_CLK                0x8020
#define GTP_REG_CHIP_TYPE               0x8000
#define GTP_REG_HAVE_KEY                0x804E
#define GTP_REG_HN_STATE                0xAB10

#define GTP_FL_FW_BURN                  0x00
#define GTP_FL_ESD_RECOVERY             0x01
#define GTP_FL_READ_REPAIR              0x02

#define GTP_BAK_REF_SEND                0
#define GTP_BAK_REF_STORE               1
#define CFG_LOC_DRVA_NUM                29
#define CFG_LOC_DRVB_NUM                30
#define CFG_LOC_SENS_NUM                31

#define GTP_CHK_FW_MAX                  1000
#define GTP_CHK_FS_MNT_MAX              300
#define GTP_BAK_REF_PATH                "/data/gtp_ref.bin"
#define GTP_MAIN_CLK_PATH               "/data/gtp_clk.bin"
#define GTP_RQST_CONFIG                 0x01
#define GTP_RQST_BAK_REF                0x02
#define GTP_RQST_RESET                  0x03
#define GTP_RQST_MAIN_CLOCK             0x04
#define GTP_RQST_HOTKNOT_CODE           0x20
#define GTP_RQST_RESPONDED              0x00
#define GTP_RQST_IDLE                   0xFF

#define HN_DEVICE_PAIRED                0x80
#define HN_MASTER_DEPARTED              0x40
#define HN_SLAVE_DEPARTED               0x20
#define HN_MASTER_SEND                  0x10
#define HN_SLAVE_RECEIVED               0x08


//                                                            

//               
#define GTP_READ_COOR_ADDR          0x814E
#define GTP_REG_SLEEP               0x8040
#define GTP_REG_SENSOR_ID           0x814A
#define GTP_REG_CONFIG_DATA         0x8047
#define GTP_REG_VERSION             0x8140
#define GTP_REG_HW_INFO             0x4220
#define GTP_REG_REFRESH_RATE		0x8056

#define RESOLUTION_LOC              3
#define TRIGGER_LOC                 8


#define GTP_DMA_MAX_TRANSACTION_LENGTH  255   //             
#define GTP_DMA_MAX_I2C_TRANSFER_SIZE   (GTP_DMA_MAX_TRANSACTION_LENGTH - GTP_ADDR_LENGTH)
#define MAX_TRANSACTION_LENGTH        8
#define TPD_I2C_NUMBER				0
#define I2C_MASTER_CLOCK              300
#define MAX_I2C_TRANSFER_SIZE         (MAX_TRANSACTION_LENGTH - GTP_ADDR_LENGTH)
#define TPD_MAX_RESET_COUNT           3
#define TPD_CALIBRATION_MATRIX        {962,0,0,0,1600,0,0,0};


#define TPD_RESET_ISSUE_WORKAROUND
#define TPD_HAVE_CALIBRATION
#define TPD_NO_GPIO
#define TPD_RESET_ISSUE_WORKAROUND

#ifdef TPD_WARP_X
#undef TPD_WARP_X
#define TPD_WARP_X(x_max, x) ( x_max - 1 - x )
#else
#define TPD_WARP_X(x_max, x) x
#endif

#ifdef TPD_WARP_Y
#undef TPD_WARP_Y
#define TPD_WARP_Y(y_max, y) ( y_max - 1 - y )
#else
#define TPD_WARP_Y(y_max, y) y
#endif

//          
#define GTP_INFO(fmt,arg...)           printk("<<-GTP-INFO->> "fmt"\n",##arg)
#define GTP_ERROR(fmt,arg...)          printk("<<-GTP-ERROR->> "fmt"\n",##arg)
#define GTP_DEBUG(fmt,arg...)          do{\
                                         if(GTP_DEBUG_ON)\
                                         printk("<<-GTP-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                                       }while(0)
#define GTP_DEBUG_ARRAY(array, num)    do{\
                                         s32 i;\
                                         u8* a = array;\
                                         if(GTP_DEBUG_ARRAY_ON)\
                                         {\
                                            printk("<<-GTP-DEBUG-ARRAY->>\n");\
                                            for (i = 0; i < (num); i++)\
                                            {\
                                                printk("%02x   ", (a)[i]);\
                                                if ((i + 1 ) %10 == 0)\
                                                {\
                                                    printk("\n");\
                                                }\
                                            }\
                                            printk("\n");\
                                        }\
                                       }while(0)
#define GTP_DEBUG_FUNC()               do{\
                                         if(GTP_DEBUG_FUNC_ON)\
                                         printk("<<-GTP-FUNC->> Func:%s@Line:%d\n",__func__,__LINE__);\
                                       }while(0)
#define GTP_SWAP(x, y)                 do{\
                                         typeof(x) z = x;\
                                         x = y;\
                                         y = z;\
                                       }while (0)

//                                                                              
//        
#define ERROR_NO_FILE           2   //      
#define ERROR_FILE_READ         23  //      
#define ERROR_FILE_TYPE         21  //      
#define ERROR_GPIO_REQUEST      4   //     
#define ERROR_I2C_TRANSFER      5   //   
#define ERROR_NO_RESPONSE       16  //     
#define ERROR_TIMEOUT           110 //         

//                                                                            

#endif /*                      */