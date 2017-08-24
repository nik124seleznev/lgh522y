
#ifndef __ALSPS_H__
#define __ALSPS_H__


#include <linux/wakelock.h>
#include <linux/interrupt.h>
#include <linux/miscdevice.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/workqueue.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/hwmsensor.h>
#include <linux/earlysuspend.h> 
#include <linux/hwmsen_dev.h>
#include <alsps_factory.h>

#define ALSPS_TAG					"<ALS/PS> "
#define ALSPS_FUN(f)			printk(ALSPS_TAG"%s\n", __func__)
#define ALSPS_ERR(fmt, args...)	printk(ALSPS_TAG"%s %d : "fmt, __func__, __LINE__, ##args)
#define ALSPS_LOG(fmt, args...)	printk(ALSPS_TAG fmt, ##args)
#define ALSPS_VER(fmt, args...)   printk(ALSPS_TAG"%s: "fmt, __func__, ##args) //         

#define   OP_ALSPS_DELAY	0X01
#define	OP_ALSPS_ENABLE	0X02
#define	OP_ALSPS_GET_DATA	0X04

#define ALSPS_INVALID_VALUE -1

#define EVENT_TYPE_ALS_VALUE         		ABS_X
#define EVENT_TYPE_PS_VALUE         		REL_Z
#define EVENT_TYPE_ALS_STATUS     		ABS_WHEEL
#define EVENT_TYPE_PS_STATUS        		REL_Y


#define ALSPS_VALUE_MAX (32767)
#define ALSPS_VALUE_MIN (-32768)
#define ALSPS_STATUS_MIN (0)
#define ALSPS_STATUS_MAX (64)
#define ALSPS_DIV_MAX (32767)
#define ALSPS_DIV_MIN (1)


#define MAX_CHOOSE_ALSPS_NUM 5

struct als_control_path
{
	int (*open_report_data)(int open);//                        
	int (*enable_nodata)(int en);//                                   
	int (*set_delay)(u64 delay);
	int (*access_data_fifo)(void);//                               
	bool is_report_input_direct;
	bool is_support_batch;//                                         
	bool is_polling_mode;
	bool is_use_common_factory;
};

struct ps_control_path
{
	int (*open_report_data)(int open);//                        
	int (*enable_nodata)(int en);//                                   
	int (*set_delay)(u64 delay);
	int (*access_data_fifo)(void);//                               
	int (*ps_calibration)(int type, int value);
	int (*ps_threshold_setting)(int type, int value[2]);
	bool is_report_input_direct;
	bool is_support_batch;//                                         
	bool is_polling_mode;
	bool is_use_common_factory;
};

struct als_data_path
{
	int (*get_data)(int *als_value, int *status);
	int (*als_get_raw_data)(int *als_value);
	int vender_div;
};

struct ps_data_path
{
	int (*get_data)(int *ps_value, int *status);
	int (*ps_get_raw_data)(int *ps_value);
	int vender_div;
};

struct alsps_init_info
{
    	char *name;
	int (*init)(void);
	int (*uninit)(void);
	struct platform_driver* platform_diver_addr;
};

struct alsps_data{
	hwm_sensor_data als_data ;
	hwm_sensor_data ps_data ;
	int data_updata;
};

struct alsps_drv_obj {
    void *self;
	int polling;
	int (*alsps_operate)(void* self, uint32_t command, void* buff_in, int size_in,
		void* buff_out, int size_out, int* actualout);
};

struct alsps_context {
	struct input_dev   		*idev;
	struct miscdevice   	mdev;
	struct work_struct  	report_ps;
	struct work_struct  	report_als;
	struct mutex 			alsps_op_mutex;
	struct timer_list   		timer_als;  /*                  */
	struct timer_list   		timer_ps;  /*                  */
	
	atomic_t            		trace;
	atomic_t            		delay_als; /*                                            */
	atomic_t				delay_ps;/*                                           */
	atomic_t            		wake;  /*                                             */

	struct early_suspend   early_drv;
	atomic_t                	early_suspend;

	struct alsps_data       	drv_data;
	struct als_control_path	als_ctl;
	struct als_data_path   	als_data;
	struct ps_control_path ps_ctl;
	struct ps_data_path   	ps_data;
	
	bool					is_als_active_nodata;//                                                                 
	bool					is_als_active_data;//                           
	bool					is_ps_active_nodata;//                                                                 
	bool					is_ps_active_data;//                           
	
	bool 				is_als_first_data_after_enable;
	bool 				is_ps_first_data_after_enable;
	bool 				is_als_polling_run;
	bool 				is_ps_polling_run;
	bool 				is_als_batch_enable;	//                                                                 
	bool 				is_ps_batch_enable;	//                                                                 
	bool 				is_get_valid_ps_data_after_enable;
};

//             
extern int alsps_aal_enable(int enable);
extern int alsps_aal_get_status(void);
extern int alsps_aal_get_data(void);

//               
extern int alsps_driver_add(struct alsps_init_info* obj) ;
extern int ps_report_interrupt_data(int value);
extern int als_data_report(struct input_dev *dev, int value,int status);
extern int als_register_control_path(struct als_control_path *ctl);
extern int als_register_data_path(struct als_data_path *data);
extern int ps_data_report(struct input_dev *dev, int value,int status);
extern int ps_register_control_path(struct ps_control_path *ctl);
extern int ps_register_data_path(struct ps_data_path *data);

#endif
