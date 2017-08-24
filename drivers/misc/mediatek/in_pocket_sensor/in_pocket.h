#ifndef __INPK_H__
#define __INPK_H__


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


#define INPK_TAG		"<IN_POCKET> "
#define INPK_FUN(f)		printk(INPK_TAG"%s\n", __func__)
#define INPK_ERR(fmt, args...)	printk(INPK_TAG"%s %d : "fmt, __func__, __LINE__, ##args)
#define INPK_LOG(fmt, args...)	printk(INPK_TAG fmt, ##args)
#define INPK_VER(fmt, args...)  printk(INPK_TAG"%s: "fmt, __func__, ##args) //         

//                           
#define	OP_INPK_ENABLE		0X02
//                             

#define INPK_INVALID_VALUE -1

#define EVENT_TYPE_INPK_VALUE		REL_X

#define INPK_VALUE_MAX (32767)
#define INPK_VALUE_MIN (-32768)
#define INPK_STATUS_MIN (0)
#define INPK_STATUS_MAX (64)
#define INPK_DIV_MAX (32767)
#define INPK_DIV_MIN (1)

typedef enum {
	INPK_DEACTIVATE,
	INPK_ACTIVATE,
	INPK_SUSPEND,
	INPK_RESUME
} inpk_state_e;

struct inpk_control_path
{
//                                                                   
	int (*open_report_data)(int open);//                        
//                       
	//                                                                 
};

struct inpk_data_path
{
	int (*get_data)(u16 *value, int *status);
};

struct inpk_init_info
{
    	char *name;
	int (*init)(void);
	int (*uninit)(void);
	struct platform_driver* platform_diver_addr;
};

struct inpk_data{
	hwm_sensor_data inpk_data ;
	int data_updata;
	//                  
};

struct inpk_drv_obj {
    void *self;
	int polling;
	int (*inpk_operate)(void* self, uint32_t command, void* buff_in, int size_in,
		void* buff_out, int size_out, int* actualout);
};

struct inpk_context {
	struct input_dev   *idev;
	struct miscdevice   mdev;
	struct work_struct  report;
	struct mutex inpk_op_mutex;
	atomic_t            wake;  /*                                             */
	atomic_t            trace;

	struct early_suspend    early_drv;
	atomic_t                early_suspend;
	atomic_t                suspend;

	struct inpk_data       drv_data;
	struct inpk_control_path   inpk_ctl;
	struct inpk_data_path   inpk_data;
	bool			is_active_nodata;		//                                                                 
	bool			is_active_data;		//                           
	bool 		is_batch_enable;	//                                                                 
};

extern int inpk_notify(void);
extern int inpk_driver_add(struct inpk_init_info* obj) ;
extern int inpk_register_control_path(struct inpk_control_path *ctl);
extern int inpk_register_data_path(struct inpk_data_path *data);

#endif
