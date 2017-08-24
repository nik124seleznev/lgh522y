/***************************************************************************
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 *    File  	: lgtp_common.h
 *    Author(s)   : Branden You < branden.you@lge.com >
 *    Description :
 *
 ***************************************************************************/

#if !defined ( _LGTP_COMMON_H_ )
#define _LGTP_COMMON_H_

/*                                                                           
                      
                                                                           */
#include <linux/types.h>
#include <linux/err.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/time.h>
#include <linux/delay.h>
#include <linux/hrtimer.h>
#include <linux/jiffies.h>
#include <linux/version.h>

#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/input/mt.h>
#include <linux/platform_device.h>

#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>

#include <linux/mutex.h>
#include <linux/async.h>
#include <linux/atomic.h>
#include <linux/workqueue.h>
#include <linux/wakelock.h>

#include <linux/firmware.h>

#if defined ( CONFIG_MTK_TOUCHPANEL )
#include <mach/wd_api.h>
#include <mach/eint.h>
#include <mach/mt_wdt.h>
#include <mach/mt_gpt.h>
#include <mach/mt_reg_base.h>
#include <mach/mt_pm_ldo.h>
#include <mach/mt_typedefs.h>
#include <mach/mt_boot.h>
#include <cust_eint.h>
#include "tpd.h"
#else
#include <mach/board.h>
#endif


#if defined ( CONFIG_HAS_EARLYSUSPEND )
#include <linux/earlysuspend.h>
#elif defined (CONFIG_FB )
#include <linux/notifier.h>
#include <linux/fb.h>
#endif


/*                                                                           
                              
                                                                           */
#define LPWG_ENABLE_ON_SUSPEND /*                 */
#define KNOCK_ON_SUPPORT /*                                              */
#define KNOCK_CODE_OVERTAP_FW

#if (LINUX_VERSION_CODE > KERNEL_VERSION(3, 4, 67))
#define KERNEL_ABOVE_3_4_67
#endif

/*                                                                           
                  
                                                                           */
#define LGTC_TRUE 	1
#define LGTC_FALSE 	0

#define LGTC_SUCCESS 	0
#define LGTC_FAIL 		-1

#define LGTC_MAX_TAP_COUNT 16

#define LGTC_LCD_ON 	1
#define LGTC_LCD_OFF 	0

#define LGTC_PROXIMITY_FAR 		1
#define LGTC_PROXIMITY_NEAR 	0

#define LGTC_COVER_CLOSE 		1
#define LGTC_COVER_OPEN 		0

#define LGTC_MAX_RETRY_COUNT 	2

#define FW_VER_INFO_SIZE		32

#define MAX_FINGER		10
#define MAX_BUTTON		4
#define MAX_KNOCK		16

#define MAX_POINT_SIZE_FOR_LPWG	256


enum {
	MT_PROTOCOL_A = 0,
	MT_PROTOCOL_B,
};


enum {
	UEVENT_IDLE = 0,
	UEVENT_BUSY,
};



enum {
	FINGER = 0,
	PALM,
	PEN,
	GLOVE,
	HOVER,
};

enum {
	KEY_NONE = 0,
	TOUCH_HARD_KEY,
	TOUCH_SOFT_KEY,
	VIRTUAL_KEY,
};

enum {
	BUTTON_RELEASED	= 0,
	BUTTON_PRESSED	= 1,
	BUTTON_CANCLED	= 0xff,
};

enum {
	IC_CTRL_READ = 1,
	IC_CTRL_WRITE,
};

enum {
	LPWG_NONE = 0,
	LPWG_DOUBLE_TAP,
	LPWG_PASSWORD,
	LPWG_SIGNATURE,
};


enum {
	TEST_VERSION = 0,
	OFFICIAL_VERSION,
};

typedef enum {
	ERROR = -1,
	NO_ERROR = 0,
	IGNORE_EVENT,
	IGNORE_EVENT_BUT_SAVE_IT,
} error_type;

typedef enum
{
	T_REPORT_NORMAL = 0,
	T_REPORT_OFF = 1,
	T_REPORT_KNOCK_ON_ONLY = 2,
	T_REPORT_KNOCK_CODE_ONLY = 3,
	T_REPORT_KNOCK_ON_CODE = 4,
	T_REPORT_GESTURE = 5,

	NUM_OF_T_REPORT_MODE
} E_TouchReportMode;

typedef enum
{
	T_LPWG_CMD_MODE = 1,
	T_LPWG_CMD_LCD_PIXEL_SIZE = 2,
	T_LPWG_CMD_ACTIVE_TOUCH_AREA = 3,
	T_LPWG_CMD_TAP_COUNT = 4,
	T_LPWG_CMD_TAP_DISTANCE = 5,
	T_LPWG_CMD_LCD_STATUS = 6,
	T_LPWG_CMD_PROXIMITY_STATUS = 7,
	T_LPWG_CMD_FIRST_TWO_TAP = 8,
	T_LPWG_CMD_UPDATE_ALL = 9,

	NUM_OF_T_LPWG_CMD_TYPE
} E_TouchLpwgCmdType;

struct touch_fw_info {
	u8		fw_ic_product_id[11];
	u8		fw_bin_product_id[11];
	u8		fw_ic_version[FW_VER_INFO_SIZE];
	u8		fw_bin_version[FW_VER_INFO_SIZE];
	u8		fw_type;
	u8		fw_force_upgrade;
	u8		fw_path[256];
};

struct point {
	int x;
	int y;
};

typedef struct LGTcLpwgSettingTag
{
	unsigned int mode; 		/*                                                                                              */
	unsigned int lcdPixelSizeX;
	unsigned int lcdPixelSizeY;
	unsigned int activeTouchAreaX1;
	unsigned int activeTouchAreaX2;
	unsigned int activeTouchAreaY1;
	unsigned int activeTouchAreaY2;
	unsigned int tapCount; 	/*                   */
	unsigned int isFirstTwoTapSame; 	/*                                          */
	unsigned int lcdState; 		/*                     */
	unsigned int proximityState; 	/*                             */
	unsigned int coverState; 	/*                          */
} LGTcLpwgSetting;


typedef struct TouchModelConfigTag
{
	u32	int_pin;
	u32	reset_pin;
	unsigned long irqflags;
	u32	button_support;
	u32	number_of_button;
	u32	button_name[MAX_BUTTON];
	u32	protocol_type;
	u32	max_x;
	u32	max_y;
	u32	max_pressure;
	u32	max_width;
	u32	max_orientation;
	u32	max_id;
	char fw_image[256];
} TouchModelConfig;


typedef struct TouchPointTag
{
	int x;
	int y;
} TouchPoint;

typedef enum TouchDataTypeTag
{
	T_DATA_FINGER = 0,
	T_DATA_KEY,
	T_DATA_KNOCK_ON,
	T_DATA_KNOCK_CODE,

	NUM_OF_DATA_TYPE
} TouchDataType;

typedef struct TouchFingerDataTag
{
	u16	id; /*           */
	u16	x;
	u16	y;
	u16	width_major;
	u16	width_minor;
	u16	orientation;
	u16	pressure; /*                                     */
	u16	type; /*                                 */
} TouchFingerData;

typedef struct TouchKeyDataTag
{
	u16	id; 			/*        */
	u16	pressed; 	/*                                   */
} TouchKeyData;

typedef struct TouchReadDataTag
{
	TouchDataType type;
	u16 count;
	TouchFingerData fingerData[MAX_FINGER+1];
	TouchKeyData keyData[MAX_BUTTON+1];
	TouchPoint knockData[MAX_KNOCK+1];
} TouchReadData;

typedef struct TouchReportDataTage
{
	u32 finger; 	/*                                                               */
	u32 key; 	/*                                                            */
	u32 knockOn; 	/*                                                    */
	u32 knockCode; 	/*                                                    */
} TouchReportData;

struct lge_touch_data {
	void					*h_touch;

	struct input_dev		*input_dev;
	struct i2c_client		*client;
	struct kobject 			lge_touch_kobj;

#if defined ( CONFIG_HAS_EARLYSUSPEND )
	struct early_suspend	early_suspend;
#elif defined ( CONFIG_FB )
	struct notifier_block	fb_notif;
#endif

#if defined(CONFIG_ARCH_MSM8916)
	struct pinctrl			*ts_pinctrl;
	struct pinctrl_state	*ts_pinset_state_active;
	struct pinctrl_state	*ts_pinset_state_suspend;
#endif

	struct delayed_work		work_upgrade;
	struct delayed_work 	work_irq;
	struct delayed_work 	work_init;
	struct mutex			thread_lock;
	struct wake_lock		lpwg_wake_lock;

	struct touch_fw_info	fw_info;

	int isSuspend;

	TouchModelConfig mConfig;

	LGTcLpwgSetting lpwgSetting;
	E_TouchReportMode reportMode;

	TouchReadData readData;

	TouchReportData reportData;
};


/*                    
  
                
                                                                   
                      
  
                           
                                                       
                                          
                                                           
                              
  
 */


struct touch_device_driver {
	error_type (*probe) (struct i2c_client *client, struct touch_fw_info* fw_info, struct attribute ***attribute_list);
	error_type (*remove) (struct i2c_client *client);
	error_type (*suspend) (struct i2c_client *client);
	error_type (*resume) (struct i2c_client *client);
	error_type (*init) (struct i2c_client *client);
	error_type (*isr) (struct i2c_client *client, TouchReadData *pData);
	error_type (*ic_ctrl) (struct i2c_client *client, u8 code, u32 value, u32 *ret);
	error_type (*fw_upgrade) (struct i2c_client *client);
	error_type (*lpwg) (struct i2c_client *client, E_TouchReportMode reportMode, LGTcLpwgSetting *pLpwgSetting);
	error_type (*sd) (struct i2c_client *client, char* buf, int* raw_status, int* ch_status);
};


/*      
  
 */
struct lge_touch_attribute {
	struct attribute	attr;
	ssize_t (*show)(struct i2c_client *client, char *buf);
	ssize_t (*store)(struct i2c_client *client,
		const char *buf, size_t count);
};

#define LGE_TOUCH_ATTR(_name, _mode, _show, _store)	\
struct lge_touch_attribute lge_touch_attr_##_name 	\
	= __ATTR(_name, _mode, _show, _store)



/*                                                                           
                    
                                                                           */


/*                                                                           
        
                                                                           */
#define LGTC_DEBUG 1

#define LGTC_TAG "[LGTC][L] "
#define LGTC_TAG_INFO "[LGTC][I] "

#if defined ( LGTC_DEBUG )
#define LGTC_FUN(f)  	   			printk(KERN_ERR LGTC_TAG"[FUNC] %s()\n", __FUNCTION__)
#define LGTC_WARN(fmt, args...)		printk(KERN_ERR LGTC_TAG"[WARN] %s() line=%d : "fmt, __FUNCTION__, __LINE__, ##args)
#define LGTC_ERR(fmt, args...)		printk(KERN_ERR LGTC_TAG"[ERROR] %s() line=%d : "fmt, __FUNCTION__, __LINE__, ##args)

/*                                                                                                                 */
#define LGTC_LOG(fmt, args...)		printk(KERN_ERR LGTC_TAG fmt, ##args)
#define LGTC_DBG(fmt, args...)		printk(KERN_ERR LGTC_TAG_INFO fmt, ##args)
#else
#define LGTC_FUN(f)  	   			printk(KERN_DEBUG LGTC_TAG"[FUNC] %s()\n", __FUNCTION__)
#define LGTC_WARN(fmt, args...)		printk(KERN_WARNING LGTC_TAG"[WARN] %s() line=%d : "fmt, __FUNCTION__, __LINE__, ##args)
#define LGTC_ERR(fmt, args...)		printk(KERN_ERR LGTC_TAG"[ERROR] %s() line=%d : "fmt, __FUNCTION__, __LINE__, ##args)

#define LGTC_LOG(fmt, args...)		printk(KERN_INFO LGTC_TAG fmt, ##args)
#define LGTC_DBG(fmt, args...)		printk(KERN_INFO LGTC_TAG fmt, ##args)
#endif


/*                   
   
                              
                                                                       
   
                                
                                                                       
   
                                
                                                                        
   
                                               
                                         
  */

#define DO_IF(do_work, goto_error)				\
do {								\
	if (do_work) { 						\
		printk(KERN_ERR "[Touch E] Action Failed [%s %d] \n",	\
			__func__, __LINE__); \
		goto goto_error; 				\
	}							\
} while (0)

#define DO_SAFE(do_work, goto_error) 				\
	DO_IF(unlikely((do_work) < 0), goto_error)

#define ASSIGN(do_assign, goto_error) 				\
do {								\
	if ((do_assign) == NULL) { 				\
		printk(KERN_ERR "[Touch E] Assign Failed [%s %d] \n",	\
			__func__, __LINE__); 		\
		goto goto_error; 				\
	}							\
} while (0)

#define ERROR_IF(cond, string, goto_error)	\
do {						\
	if (cond) {				\
		LGTC_ERR(string);		\
		goto goto_error;		\
	}					\
} while (0)




/*                                                                           
                            
                                                                           */



#endif /*                 */


/*             */

