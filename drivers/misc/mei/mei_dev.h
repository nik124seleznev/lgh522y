/*
 *
 * Intel Management Engine Interface (Intel MEI) Linux driver
 * Copyright (c) 2003-2012, Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 */

#ifndef _MEI_DEV_H_
#define _MEI_DEV_H_

#include <linux/types.h>
#include <linux/watchdog.h>
#include <linux/poll.h>
#include <linux/mei.h>
#include <linux/mei_cl_bus.h>

#include "hw.h"
#include "hw-me-regs.h"
#include "hbm.h"

/*
                       
 */
#define MEI_WD_HDR_SIZE       4
#define MEI_WD_STOP_MSG_SIZE  MEI_WD_HDR_SIZE
#define MEI_WD_START_MSG_SIZE (MEI_WD_HDR_SIZE + 16)

#define MEI_WD_DEFAULT_TIMEOUT   120  /*         */
#define MEI_WD_MIN_TIMEOUT       120  /*         */
#define MEI_WD_MAX_TIMEOUT     65535  /*         */

#define MEI_WD_STOP_TIMEOUT      10 /*       */

#define MEI_WD_STATE_INDEPENDENCE_MSG_SENT       (1 << 0)

#define MEI_RD_MSG_BUF_SIZE           (128 * sizeof(u32))


/*
                    
 */
extern const uuid_le mei_amthif_guid;

/*
                       
 */
extern const uuid_le mei_wd_guid;

/*
                                      
 */
extern const u8 mei_wd_state_independence_msg[3][4];

/*
                                
 */
#define MEI_CLIENTS_MAX 256

/*
                                     
                                    
  
                                  
                                            
 */
#define  MEI_MAX_OPEN_HANDLE_COUNT (MEI_CLIENTS_MAX - 1)

/*
                          
 */
#define MEI_HOST_CLIENT_ID_ANY        (-1)
#define MEI_HBM_HOST_CLIENT_ID         0 /*                                  */
#define MEI_WD_HOST_CLIENT_ID          1
#define MEI_IAMTHIF_HOST_CLIENT_ID     2


/*            */
enum file_state {
	MEI_FILE_INITIALIZING = 0,
	MEI_FILE_CONNECTING,
	MEI_FILE_CONNECTED,
	MEI_FILE_DISCONNECTING,
	MEI_FILE_DISCONNECTED
};

/*                   */
enum mei_dev_state {
	MEI_DEV_INITIALIZING = 0,
	MEI_DEV_INIT_CLIENTS,
	MEI_DEV_ENABLED,
	MEI_DEV_RESETTING,
	MEI_DEV_DISABLED,
	MEI_DEV_POWER_DOWN,
	MEI_DEV_POWER_UP
};

const char *mei_dev_state_str(int state);

enum iamthif_states {
	MEI_IAMTHIF_IDLE,
	MEI_IAMTHIF_WRITING,
	MEI_IAMTHIF_FLOW_CONTROL,
	MEI_IAMTHIF_READING,
	MEI_IAMTHIF_READ_COMPLETE
};

enum mei_file_transaction_states {
	MEI_IDLE,
	MEI_WRITING,
	MEI_WRITE_COMPLETE,
	MEI_FLOW_CONTROL,
	MEI_READING,
	MEI_READ_COMPLETE
};

enum mei_wd_states {
	MEI_WD_IDLE,
	MEI_WD_RUNNING,
	MEI_WD_STOPPING,
};

/* 
                                                                      
                         
                          
                          
                         
                          
 */
enum mei_cb_file_ops {
	MEI_FOP_READ = 0,
	MEI_FOP_WRITE,
	MEI_FOP_IOCTL,
	MEI_FOP_OPEN,
	MEI_FOP_CLOSE
};

/*
                                
 */
struct mei_msg_data {
	u32 size;
	unsigned char *data;
};

/* 
                                                          
  
                              
                            
                                              
 */
struct mei_me_client {
	struct mei_client_properties props;
	u8 client_id;
	u8 mei_flow_ctrl_creds;
};


struct mei_cl;

/* 
                                                       
  
                                                  
                                  
 */
struct mei_cl_cb {
	struct list_head list;
	struct mei_cl *cl;
	enum mei_cb_file_ops fop_type;
	struct mei_msg_data request_buffer;
	struct mei_msg_data response_buffer;
	unsigned long buf_idx;
	unsigned long read_time;
	struct file *file_object;
};

/*                                                  */
struct mei_cl {
	struct list_head link;
	struct mei_device *dev;
	enum file_state state;
	wait_queue_head_t tx_wait;
	wait_queue_head_t rx_wait;
	wait_queue_head_t wait;
	int status;
	/*                        */
	u8 host_client_id;
	u8 me_client_id;
	u8 mei_flow_ctrl_creds;
	u8 timer_count;
	enum mei_file_transaction_states reading_state;
	enum mei_file_transaction_states writing_state;
	int sm_state;
	struct mei_cl_cb *read_cb;

	/*                 */
	struct mei_cl_device *device;
	struct list_head device_link;
	uuid_le device_uuid;
};

/*                   
  
                                               

                                           
                               
                                           
                                   

                                               
                                        
                                         

                                                         
                                                     
                                                     

                                            

                                                      

                                                 
                                                
 */
struct mei_hw_ops {

	bool (*host_is_ready) (struct mei_device *dev);

	bool (*hw_is_ready) (struct mei_device *dev);
	void (*hw_reset) (struct mei_device *dev, bool enable);
	int  (*hw_start) (struct mei_device *dev);
	void (*hw_config) (struct mei_device *dev);

	void (*intr_clear) (struct mei_device *dev);
	void (*intr_enable) (struct mei_device *dev);
	void (*intr_disable) (struct mei_device *dev);

	int (*hbuf_free_slots) (struct mei_device *dev);
	bool (*hbuf_is_ready) (struct mei_device *dev);
	size_t (*hbuf_max_len) (const struct mei_device *dev);

	int (*write)(struct mei_device *dev,
		     struct mei_msg_hdr *hdr,
		     unsigned char *buf);

	int (*rdbuf_full_slots)(struct mei_device *dev);

	u32 (*read_hdr)(const struct mei_device *dev);
	int (*read) (struct mei_device *dev,
		     unsigned char *buf, unsigned long len);
};

/*            */

/* 
                                        
                                                                
                       
  
                                                                  
                                          
                                      
                                                                     
                                                       
                        
                                                                         
                                                          
 */
struct mei_cl_ops {
	int (*enable)(struct mei_cl_device *device);
	int (*disable)(struct mei_cl_device *device);
	int (*send)(struct mei_cl_device *device, u8 *buf, size_t length);
	int (*recv)(struct mei_cl_device *device, u8 *buf, size_t length);
};

struct mei_cl_device *mei_cl_add_device(struct mei_device *dev,
					uuid_le uuid, char *name,
					struct mei_cl_ops *ops);
void mei_cl_remove_device(struct mei_cl_device *device);

int __mei_cl_async_send(struct mei_cl *cl, u8 *buf, size_t length);
int __mei_cl_send(struct mei_cl *cl, u8 *buf, size_t length);
int __mei_cl_recv(struct mei_cl *cl, u8 *buf, size_t length);
void mei_cl_bus_rx_event(struct mei_cl *cl);
int mei_cl_bus_init(void);
void mei_cl_bus_exit(void);


/* 
                                           
                                                             
                                                                    
                                                            
                                                           
  
                                          
                        
                  
                         
                                                                   
                                                 
                                              
                                  
 */
struct mei_cl_device {
	struct device dev;

	struct mei_cl *cl;

	const struct mei_cl_ops *ops;

	struct work_struct event_work;
	mei_cl_event_cb_t event_cb;
	void *event_context;
	unsigned long events;

	void *priv_data;
};

/* 
                                                 

                                                  
                                               

                                                             
                                                                
                                                
                                                                         
 */
struct mei_device {
	struct pci_dev *pdev;	/*                              */
	/*
                   
  */
	/*                                */
	struct mei_cl_cb read_list;		/*                   */
	struct mei_cl_cb write_list;		/*                    */
	struct mei_cl_cb write_waiting_list;	/*                     */
	struct mei_cl_cb ctrl_wr_list;		/*                          */
	struct mei_cl_cb ctrl_rd_list;		/*                         */

	/*
                 
  */
	struct list_head file_list;
	long open_handle_count;

	/*
                       
  */
	struct mutex device_lock; /*             */
	struct delayed_work timer_work;	/*                                   */

	bool recvd_hw_ready;
	/*
                                             
  */
	wait_queue_head_t wait_hw_ready;
	wait_queue_head_t wait_recvd_msg;
	wait_queue_head_t wait_stop_wd;

	/*
                      
  */
	enum mei_dev_state dev_state;
	enum mei_hbm_state hbm_state;
	u16 init_clients_timer;

	unsigned char rd_msg_buf[MEI_RD_MSG_BUF_SIZE];	/*                  */
	u32 rd_msg_hdr;

	/*              */
	u8 hbuf_depth;
	bool hbuf_is_ready;

	/*                           */
	struct {
		struct mei_msg_hdr hdr;
		unsigned char data[128];
	} wr_msg;

	struct {
		struct mei_msg_hdr hdr;
		unsigned char data[4];	/*                              */
	} wr_ext_msg;		/*                       */

	struct hbm_version version;

	struct mei_me_client *me_clients; /*                                  */
	DECLARE_BITMAP(me_clients_map, MEI_CLIENTS_MAX);
	DECLARE_BITMAP(host_clients_map, MEI_CLIENTS_MAX);
	unsigned long me_clients_num;
	unsigned long me_client_presentation_num;
	unsigned long me_client_index;

	struct mei_cl wd_cl;
	enum mei_wd_states wd_state;
	bool wd_pending;
	u16 wd_timeout;
	unsigned char wd_data[MEI_WD_START_MSG_SIZE];


	/*                             */
	struct mei_cl_cb amthif_cmd_list;
	/*                                                                  */
	struct mei_cl_cb amthif_rd_complete_list;
	struct file *iamthif_file_object;
	struct mei_cl iamthif_cl;
	struct mei_cl_cb *iamthif_current_cb;
	int iamthif_mtu;
	unsigned long iamthif_timer;
	u32 iamthif_stall_timer;
	unsigned char *iamthif_msg_buf; /*                                  */
	u32 iamthif_msg_buf_size;
	u32 iamthif_msg_buf_index;
	enum iamthif_states iamthif_state;
	bool iamthif_flow_control_pending;
	bool iamthif_ioctl;
	bool iamthif_canceled;

	struct work_struct init_work;

	/*                     */
	struct list_head device_list;

#if IS_ENABLED(CONFIG_DEBUG_FS)
	struct dentry *dbgfs_dir;
#endif /*                 */


	const struct mei_hw_ops *ops;
	char hw[0] __aligned(sizeof(void *));
};

static inline unsigned long mei_secs_to_jiffies(unsigned long sec)
{
	return msecs_to_jiffies(sec * MSEC_PER_SEC);
}

/* 
                                                                        
                           
                                          
                             
 */
static inline u32 mei_data2slots(size_t length)
{
	return DIV_ROUND_UP(sizeof(struct mei_msg_hdr) + length, 4);
}

/*
                               
 */
void mei_device_init(struct mei_device *dev);
void mei_reset(struct mei_device *dev, int interrupts);
int mei_start(struct mei_device *dev);
void mei_stop(struct mei_device *dev);

/*
                                     
 */

void mei_timer(struct work_struct *work);
int mei_irq_read_handler(struct mei_device *dev,
		struct mei_cl_cb *cmpl_list, s32 *slots);

int mei_irq_write_handler(struct mei_device *dev, struct mei_cl_cb *cmpl_list);
void mei_irq_compl_handler(struct mei_device *dev, struct mei_cl_cb *cmpl_list);

/*
                                        
 */
void mei_amthif_reset_params(struct mei_device *dev);

int mei_amthif_host_init(struct mei_device *dev);

int mei_amthif_write(struct mei_device *dev, struct mei_cl_cb *priv_cb);

int mei_amthif_read(struct mei_device *dev, struct file *file,
		char __user *ubuf, size_t length, loff_t *offset);

unsigned int mei_amthif_poll(struct mei_device *dev,
		struct file *file, poll_table *wait);

int mei_amthif_release(struct mei_device *dev, struct file *file);

struct mei_cl_cb *mei_amthif_find_read_list_entry(struct mei_device *dev,
						struct file *file);

void mei_amthif_run_next_cmd(struct mei_device *dev);

int mei_amthif_irq_write_complete(struct mei_device *dev, s32 *slots,
			struct mei_cl_cb *cb, struct mei_cl_cb *cmpl_list);

void mei_amthif_complete(struct mei_device *dev, struct mei_cl_cb *cb);
int mei_amthif_irq_read_msg(struct mei_device *dev,
			    struct mei_msg_hdr *mei_hdr,
			    struct mei_cl_cb *complete_list);
int mei_amthif_irq_read(struct mei_device *dev, s32 *slots);

/*
                
 */
int mei_nfc_host_init(struct mei_device *dev);
void mei_nfc_host_exit(void);

/*
                  
 */
extern const uuid_le mei_nfc_guid;

int mei_amthif_irq_write_complete(struct mei_device *dev, s32 *slots,
			struct mei_cl_cb *cb, struct mei_cl_cb *cmpl_list);

void mei_amthif_complete(struct mei_device *dev, struct mei_cl_cb *cb);
int mei_amthif_irq_read_message(struct mei_cl_cb *complete_list,
		struct mei_device *dev, struct mei_msg_hdr *mei_hdr);
int mei_amthif_irq_read(struct mei_device *dev, s32 *slots);


int mei_wd_send(struct mei_device *dev);
int mei_wd_stop(struct mei_device *dev);
int mei_wd_host_init(struct mei_device *dev);
/*
                                                          
                                            
                    
 */
void mei_watchdog_register(struct mei_device *dev);
/*
                                                              
                    
 */
void mei_watchdog_unregister(struct mei_device *dev);

/*
                           
 */

static inline void mei_hw_config(struct mei_device *dev)
{
	dev->ops->hw_config(dev);
}
static inline void mei_hw_reset(struct mei_device *dev, bool enable)
{
	dev->ops->hw_reset(dev, enable);
}

static inline void mei_hw_start(struct mei_device *dev)
{
	dev->ops->hw_start(dev);
}

static inline void mei_clear_interrupts(struct mei_device *dev)
{
	dev->ops->intr_clear(dev);
}

static inline void mei_enable_interrupts(struct mei_device *dev)
{
	dev->ops->intr_enable(dev);
}

static inline void mei_disable_interrupts(struct mei_device *dev)
{
	dev->ops->intr_disable(dev);
}

static inline bool mei_host_is_ready(struct mei_device *dev)
{
	return dev->ops->host_is_ready(dev);
}
static inline bool mei_hw_is_ready(struct mei_device *dev)
{
	return dev->ops->hw_is_ready(dev);
}

static inline bool mei_hbuf_is_ready(struct mei_device *dev)
{
	return dev->ops->hbuf_is_ready(dev);
}

static inline int mei_hbuf_empty_slots(struct mei_device *dev)
{
	return dev->ops->hbuf_free_slots(dev);
}

static inline size_t mei_hbuf_max_len(const struct mei_device *dev)
{
	return dev->ops->hbuf_max_len(dev);
}

static inline int mei_write_message(struct mei_device *dev,
			struct mei_msg_hdr *hdr,
			unsigned char *buf)
{
	return dev->ops->write(dev, hdr, buf);
}

static inline u32 mei_read_hdr(const struct mei_device *dev)
{
	return dev->ops->read_hdr(dev);
}

static inline void mei_read_slots(struct mei_device *dev,
		     unsigned char *buf, unsigned long len)
{
	dev->ops->read(dev, buf, len);
}

static inline int mei_count_full_read_slots(struct mei_device *dev)
{
	return dev->ops->rdbuf_full_slots(dev);
}

#if IS_ENABLED(CONFIG_DEBUG_FS)
int mei_dbgfs_register(struct mei_device *dev, const char *name);
void mei_dbgfs_deregister(struct mei_device *dev);
#else
static inline int mei_dbgfs_register(struct mei_device *dev, const char *name)
{
	return 0;
}
static inline void mei_dbgfs_deregister(struct mei_device *dev) {}
#endif /*                 */

int mei_register(struct mei_device *dev);
void mei_deregister(struct mei_device *dev);

#define MEI_HDR_FMT "hdr:host=%02d me=%02d len=%d comp=%1d"
#define MEI_HDR_PRM(hdr)                  \
	(hdr)->host_addr, (hdr)->me_addr, \
	(hdr)->length, (hdr)->msg_complete

#endif
