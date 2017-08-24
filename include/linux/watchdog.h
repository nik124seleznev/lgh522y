/*
                                           
  
                                
                                      
  
 */
#ifndef _LINUX_WATCHDOG_H
#define _LINUX_WATCHDOG_H


#include <linux/bitops.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <uapi/linux/watchdog.h>

struct watchdog_ops;
struct watchdog_device;

/*                                                       
  
                            
                                                        
                                                       
                                                                         
                                                                     
                                                                           
                                                                            
                                                                     
                                                                         
                                                       
  
                                                                     
                                                                        
                                                                         
                         
 */
struct watchdog_ops {
	struct module *owner;
	/*                      */
	int (*start)(struct watchdog_device *);
	int (*stop)(struct watchdog_device *);
	/*                     */
	int (*ping)(struct watchdog_device *);
	unsigned int (*status)(struct watchdog_device *);
	int (*set_timeout)(struct watchdog_device *, unsigned int);
	unsigned int (*get_timeleft)(struct watchdog_device *);
	void (*ref)(struct watchdog_device *);
	void (*unref)(struct watchdog_device *);
	long (*ioctl)(struct watchdog_device *, unsigned int, unsigned long);
};

/*                                                                       
  
                                                                   
                                          
                                    
                                 
                                               
                                                    
                                                      
                                                
                                                           
                                                           
                                                    
                                                   
                                                                 
  
                                                                 
                         
  
                                                                          
                                                                 
  
                                                                          
           
 */
struct watchdog_device {
	int id;
	struct cdev cdev;
	struct device *dev;
	struct device *parent;
	const struct watchdog_info *info;
	const struct watchdog_ops *ops;
	unsigned int bootstatus;
	unsigned int timeout;
	unsigned int min_timeout;
	unsigned int max_timeout;
	void *driver_data;
	struct mutex lock;
	unsigned long status;
/*                              */
#define WDOG_ACTIVE		0	/*                                */
#define WDOG_DEV_OPEN		1	/*                            */
#define WDOG_ALLOW_RELEASE	2	/*                                 */
#define WDOG_NO_WAY_OUT		3	/*                             */
#define WDOG_UNREGISTERED	4	/*                                  */
};

#ifdef CONFIG_WATCHDOG_NOWAYOUT
#define WATCHDOG_NOWAYOUT		1
#define WATCHDOG_NOWAYOUT_INIT_STATUS	(1 << WDOG_NO_WAY_OUT)
#else
#define WATCHDOG_NOWAYOUT		0
#define WATCHDOG_NOWAYOUT_INIT_STATUS	0
#endif

/*                                                                           */
static inline bool watchdog_active(struct watchdog_device *wdd)
{
	return test_bit(WDOG_ACTIVE, &wdd->status);
}

/*                                                        */
static inline void watchdog_set_nowayout(struct watchdog_device *wdd, bool nowayout)
{
	if (nowayout)
		set_bit(WDOG_NO_WAY_OUT, &wdd->status);
}

/*                                                                   */
static inline bool watchdog_timeout_invalid(struct watchdog_device *wdd, unsigned int t)
{
	return ((wdd->max_timeout != 0) &&
		(t < wdd->min_timeout || t > wdd->max_timeout));
}

/*                                                                         */
static inline void watchdog_set_drvdata(struct watchdog_device *wdd, void *data)
{
	wdd->driver_data = data;
}

static inline void *watchdog_get_drvdata(struct watchdog_device *wdd)
{
	return wdd->driver_data;
}

/*                                  */
extern int watchdog_init_timeout(struct watchdog_device *wdd,
				  unsigned int timeout_parm, struct device *dev);
extern int watchdog_register_device(struct watchdog_device *);
extern void watchdog_unregister_device(struct watchdog_device *);

#endif  /*                          */
