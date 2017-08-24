/*                                                                           */
/*              */
/*                                                      */
/*              */
/*                                                                           */
/*   Copyright (C) 1995-2000 Simon G. Vogl

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
    MA 02110-1301 USA.							     */
/*                                                                           */

/*                                                               
                                   */
#ifndef _LINUX_I2C_H
#define _LINUX_I2C_H

#include <linux/module.h>
#include <linux/mod_devicetable.h>
#include <linux/device.h>	/*                   */
#include <linux/sched.h>	/*                */
#include <linux/mutex.h>
#include <linux/of.h>		/*                        */
#include <linux/swab.h>		/*            */
#include <uapi/linux/i2c.h>

extern struct bus_type i2c_bus_type;
extern struct device_type i2c_adapter_type;
#define USE_I2C_MTK_EXT
#ifdef USE_I2C_MTK_EXT
#define I2C_A_FILTER_MSG	0x8000	/*                          */
#define I2C_A_CHANGE_TIMING	0x4000	/*                         */
#define I2C_MASK_FLAG	(0x00ff)
#define I2C_DMA_FLAG	(0xdead2000)
#define I2C_WR_FLAG		(0x1000)
#define I2C_RS_FLAG		(0x0800)
#define I2C_HS_FLAG   (0x0400)
#define I2C_ENEXT_FLAG (0x0200)
#define I2C_DISEXT_FLAG (0x0000)
#define I2C_POLL_FLAG (0x4000)
#define I2C_CH2_FLAG	(0x8000)

#define I2C_POLLING_FLAG (0x00000001)
#define I2C_PUSHPULL_FLAG (0x00000002)
#define I2C_3DCAMERA_FLAG (0x00000004)
#define I2C_DIRECTION_FLAG (0x00000008)
#endif
/*                                                                      */

struct i2c_msg;
struct i2c_algorithm;
struct i2c_adapter;
struct i2c_client;
struct i2c_driver;
union i2c_smbus_data;
struct i2c_board_info;

struct module;

#if defined(CONFIG_I2C) || defined(CONFIG_I2C_MODULE)
/*
                                                                             
                                                                           
                                                                        
                                                                 
                                                        
 */
extern int i2c_master_send(const struct i2c_client *client, const char *buf,
			   int count);
extern int i2c_master_recv(const struct i2c_client *client, char *buf,
			   int count);

extern int mt_i2c_master_send(const struct i2c_client *client, const char *buf,
			   int count, u32 ext_flag);

extern int mt_i2c_master_recv(const struct i2c_client *client, char *buf,
			   int count, u32 ext_flag);
/*                       
 */
extern int i2c_transfer(struct i2c_adapter *adap, struct i2c_msg *msgs,
			int num);
/*                 */
extern int __i2c_transfer(struct i2c_adapter *adap, struct i2c_msg *msgs,
			  int num);

/*                                                                       
                                                                           
                             
                                                                        
                                        */
extern s32 i2c_smbus_xfer(struct i2c_adapter *adapter, u16 addr,
			  unsigned short flags, char read_write, u8 command,
			  int size, union i2c_smbus_data *data);

/*                                                                       
                                  */

extern s32 i2c_smbus_read_byte(const struct i2c_client *client);
extern s32 i2c_smbus_write_byte(const struct i2c_client *client, u8 value);
extern s32 i2c_smbus_read_byte_data(const struct i2c_client *client,
				    u8 command);
extern s32 i2c_smbus_write_byte_data(const struct i2c_client *client,
				     u8 command, u8 value);
extern s32 i2c_smbus_read_word_data(const struct i2c_client *client,
				    u8 command);
extern s32 i2c_smbus_write_word_data(const struct i2c_client *client,
				     u8 command, u16 value);

static inline s32
i2c_smbus_read_word_swapped(const struct i2c_client *client, u8 command)
{
	s32 value = i2c_smbus_read_word_data(client, command);

	return (value < 0) ? value : swab16(value);
}

static inline s32
i2c_smbus_write_word_swapped(const struct i2c_client *client,
			     u8 command, u16 value)
{
	return i2c_smbus_write_word_data(client, command, swab16(value));
}

/*                                  */
extern s32 i2c_smbus_read_block_data(const struct i2c_client *client,
				     u8 command, u8 *values);
extern s32 i2c_smbus_write_block_data(const struct i2c_client *client,
				      u8 command, u8 length, const u8 *values);
/*                                  */
extern s32 i2c_smbus_read_i2c_block_data(const struct i2c_client *client,
					 u8 command, u8 length, u8 *values);
extern s32 i2c_smbus_write_i2c_block_data(const struct i2c_client *client,
					  u8 command, u8 length,
					  const u8 *values);
#endif /*     */

/* 
                                                     
                                                              
                                                          
                                      
                                         
                                          
                                        
                                      
                                                                   
                                                       
                                      
                                                          
                                         
                                                         
                                                                        
  
                                                                           
                                                                  
  
                                                                      
                                                                       
                                                                   
                                                                     
                                                                       
  
                                                                        
                                                                        
                                                                       
                                                       
  
                                                                      
                                                                       
                                                                     
                                                                    
               
 */
struct i2c_driver {
	unsigned int class;

	/*                                                                  
                                                    
  */
	int (*attach_adapter)(struct i2c_adapter *) __deprecated;

	/*                                  */
	int (*probe)(struct i2c_client *, const struct i2c_device_id *);
	int (*remove)(struct i2c_client *);

	/*                                                           */
	void (*shutdown)(struct i2c_client *);
	int (*suspend)(struct i2c_client *, pm_message_t mesg);
	int (*resume)(struct i2c_client *);

	/*                                                          
                                                                     
                                                                      
                                                   
  */
	void (*alert)(struct i2c_client *, unsigned int data);

	/*                                                                    
                    
  */
	int (*command)(struct i2c_client *client, unsigned int cmd, void *arg);

	struct device_driver driver;
	const struct i2c_device_id *id_table;

	/*                                                         */
	int (*detect)(struct i2c_client *, struct i2c_board_info *);
	const unsigned short *address_list;
	struct list_head clients;
};
#define to_i2c_driver(d) container_of(d, struct i2c_driver, driver)

/* 
                                                    
                                                                           
                                                               
                                                                      
                                                                      
                                                                   
                                                            
                                                             
                                                
                                                            
                                                                
                         
  
                                                                       
                                                                   
                       
 */
struct i2c_client {
	unsigned short flags;		/*                  */
	unsigned short addr;		/*                           */
					/*                             */
					/*                 */
	char name[I2C_NAME_SIZE];
	struct i2c_adapter *adapter;	/*                       */
	struct i2c_driver *driver;	/*                         */
	struct device dev;		/*                       */
	int irq;			/*                       */
	struct list_head detected;
	#ifdef USE_I2C_MTK_EXT
	__u32 timing;			/*                       */
	__u32 ext_flag;
	#endif
};
#define to_i2c_client(d) container_of(d, struct i2c_client, dev)

extern struct i2c_client *i2c_verify_client(struct device *dev);
extern struct i2c_adapter *i2c_verify_adapter(struct device *dev);

static inline struct i2c_client *kobj_to_i2c_client(struct kobject *kobj)
{
	struct device * const dev = container_of(kobj, struct device, kobj);
	return to_i2c_client(dev);
}

static inline void *i2c_get_clientdata(const struct i2c_client *dev)
{
	return dev_get_drvdata(&dev->dev);
}

static inline void i2c_set_clientdata(struct i2c_client *dev, void *data)
{
	dev_set_drvdata(&dev->dev, data);
}

/* 
                                                       
                                                  
                                         
                                   
                                                         
                                                 
                                                
                               
                                 
  
                                                                          
                                                                            
                                                                            
                                                                     
  
                                                                            
                                                                             
                                                                          
                                                                               
                                                                         
 */
struct i2c_board_info {
	char		type[I2C_NAME_SIZE];
	unsigned short	flags;
	unsigned short	addr;
	void		*platform_data;
	struct dev_archdata	*archdata;
	struct device_node *of_node;
	struct acpi_dev_node acpi_node;
	int		irq;
};

/* 
                                                                    
                                        
                                              
  
                                                                      
                                                                    
                                                                    
                                          
 */
#define I2C_BOARD_INFO(dev_type, dev_addr) \
	.type = dev_type, .addr = (dev_addr)


#if defined(CONFIG_I2C) || defined(CONFIG_I2C_MODULE)
/*                                                                     
                                                                    
                                                 
 */
extern struct i2c_client *
i2c_new_device(struct i2c_adapter *adap, struct i2c_board_info const *info);

/*                                                                       
                                                                     
                                                                           
                                                                            
                                    
 */
extern struct i2c_client *
i2c_new_probed_device(struct i2c_adapter *adap,
		      struct i2c_board_info *info,
		      unsigned short const *addr_list,
		      int (*probe)(struct i2c_adapter *, unsigned short addr));

/*                               */
extern int i2c_probe_func_quick_read(struct i2c_adapter *, unsigned short addr);

/*                                                                    
                                          
 */
extern struct i2c_client *
i2c_new_dummy(struct i2c_adapter *adap, u16 address);

extern void i2c_unregister_device(struct i2c_client *);
#endif /*     */

/*                                                                    
                                                                         
                                                  
 */
#ifdef CONFIG_I2C_BOARDINFO
extern int
i2c_register_board_info(int busnum, struct i2c_board_info const *info,
			unsigned n);
#else
static inline int
i2c_register_board_info(int busnum, struct i2c_board_info const *info,
			unsigned n)
{
	return 0;
}
#endif /*               */

/*
                                                                             
                                                                            
                                                                               
                                  
 */
struct i2c_algorithm {
	/*                                                                   
                                                             
                                                               
                              */
	/*                                                              
                                            */
	int (*master_xfer)(struct i2c_adapter *adap, struct i2c_msg *msgs,
			   int num);
	int (*smbus_xfer) (struct i2c_adapter *adap, u16 addr,
			   unsigned short flags, char read_write,
			   u8 command, int size, union i2c_smbus_data *data);

	/*                                        */
	u32 (*functionality) (struct i2c_adapter *);
};

/* 
                                                              
                                                                                
                                                             
                                                                           
                                                            
                                                                                
                                             
                                                                          
                                                                                
                 
                                                                                
                                                                      
                                                                               
                                                                          
                                                                           
                                                                           
 */
struct i2c_bus_recovery_info {
	int (*recover_bus)(struct i2c_adapter *);

	int (*get_scl)(struct i2c_adapter *);
	void (*set_scl)(struct i2c_adapter *, int val);
	int (*get_sda)(struct i2c_adapter *);

	void (*prepare_recovery)(struct i2c_bus_recovery_info *bri);
	void (*unprepare_recovery)(struct i2c_bus_recovery_info *bri);

	/*               */
	int scl_gpio;
	int sda_gpio;
};

int i2c_recover_bus(struct i2c_adapter *adap);

/*                           */
int i2c_generic_gpio_recovery(struct i2c_adapter *adap);
int i2c_generic_scl_recovery(struct i2c_adapter *adap);

/*
                                                                         
                                                     
 */
struct i2c_adapter {
	struct module *owner;
	unsigned int class;		  /*                              */
	const struct i2c_algorithm *algo; /*                                 */
	void *algo_data;

	/*                                            */
	struct rt_mutex bus_lock;

	int timeout;			/*            */
	int retries;
	struct device dev;		/*                    */

	int nr;
	char name[48];
	struct completion dev_released;

	struct mutex userspace_clients_lock;
	struct list_head userspace_clients;

	struct i2c_bus_recovery_info *bus_recovery_info;
};
#define to_i2c_adapter(d) container_of(d, struct i2c_adapter, dev)

static inline void *i2c_get_adapdata(const struct i2c_adapter *dev)
{
	return dev_get_drvdata(&dev->dev);
}

static inline void i2c_set_adapdata(struct i2c_adapter *dev, void *data)
{
	dev_set_drvdata(&dev->dev, data);
}

static inline struct i2c_adapter *
i2c_parent_is_i2c_adapter(const struct i2c_adapter *adapter)
{
	struct device *parent = adapter->dev.parent;

	if (parent != NULL && parent->type == &i2c_adapter_type)
		return to_i2c_adapter(parent);
	else
		return NULL;
}

int i2c_for_each_dev(void *data, int (*fn)(struct device *, void *));

/*                                                          */
void i2c_lock_adapter(struct i2c_adapter *);
void i2c_unlock_adapter(struct i2c_adapter *);

/*                             */
#define I2C_CLIENT_PEC	0x04		/*                           */
#define I2C_CLIENT_TEN	0x10		/*                                */
					/*                            */
#define I2C_CLIENT_WAKE	0x80		/*                                   */
#define I2C_CLIENT_SCCB	0x9000		/*                              */
					/*                                  */

/*                               */
#define I2C_CLASS_HWMON		(1<<0)	/*                 */
#define I2C_CLASS_DDC		(1<<3)	/*                              */
#define I2C_CLASS_SPD		(1<<7)	/*                */

/*                                     */
#define I2C_CLIENT_END		0xfffeU

/*                                                               */
#define I2C_ADDRS(addr, addrs...) \
	((const unsigned short []){ addr, ## addrs, I2C_CLIENT_END })


/*                                   */

/*                  
 */
#if defined(CONFIG_I2C) || defined(CONFIG_I2C_MODULE)
extern int i2c_add_adapter(struct i2c_adapter *);
extern void i2c_del_adapter(struct i2c_adapter *);
extern int i2c_add_numbered_adapter(struct i2c_adapter *);

extern int i2c_register_driver(struct module *, struct i2c_driver *);
extern void i2c_del_driver(struct i2c_driver *);

/*                                                           */
#define i2c_add_driver(driver) \
	i2c_register_driver(THIS_MODULE, driver)

extern struct i2c_client *i2c_use_client(struct i2c_client *client);
extern void i2c_release_client(struct i2c_client *client);

/*                                                            
                       */
extern void i2c_clients_command(struct i2c_adapter *adap,
				unsigned int cmd, void *arg);

extern struct i2c_adapter *i2c_get_adapter(int nr);
extern void i2c_put_adapter(struct i2c_adapter *adap);


/*                               */
static inline u32 i2c_get_functionality(struct i2c_adapter *adap)
{
	return adap->algo->functionality(adap);
}

/*                                                            */
static inline int i2c_check_functionality(struct i2c_adapter *adap, u32 func)
{
	return (func & i2c_get_functionality(adap)) == func;
}

/*                                                  */
static inline int i2c_adapter_id(struct i2c_adapter *adap)
{
	return adap->nr;
}

/* 
                                                                  
                                   
  
                                                                          
                                                                        
                                                                               
 */
#define module_i2c_driver(__i2c_driver) \
	module_driver(__i2c_driver, i2c_add_driver, \
			i2c_del_driver)

#endif /*     */

#if IS_ENABLED(CONFIG_ACPI_I2C)
extern void acpi_i2c_register_devices(struct i2c_adapter *adap);
#else
static inline void acpi_i2c_register_devices(struct i2c_adapter *adap) {}
#endif

#endif /*              */
