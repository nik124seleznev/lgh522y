/*
 * include/linux/uio_driver.h
 *
 * Copyright(C) 2005, Benedikt Spranger <b.spranger@linutronix.de>
 * Copyright(C) 2005, Thomas Gleixner <tglx@linutronix.de>
 * Copyright(C) 2006, Hans J. Koch <hjk@hansjkoch.de>
 * Copyright(C) 2006, Greg Kroah-Hartman <greg@kroah.com>
 *
 * Userspace IO driver.
 *
 * Licensed under the GPLv2 only.
 */

#ifndef _EXM_DRIVER_H_
#define _EXM_DRIVER_H_

#include <linux/fs.h>
#include <linux/interrupt.h>

struct module;
struct exm_map;

/* 
                                                      
                                                       
                                                                  
                                                            
                                                       
                   
                     
                                           
                                                                       
                                       
 */
struct exm_mem {
	const char *name;
	phys_addr_t addr;
	unsigned long size;
	int memtype;
	void __iomem *internal_addr;
	struct exm_map *map;
};

#define MAX_EXM_MAPS	5

struct exm_portio;

/* 
                                                     
                                                     
                                
                              
                                                  
                                          
 */
struct exm_port {
	const char *name;
	unsigned long start;
	unsigned long size;
	int porttype;
	struct exm_portio *portio;
};

#define MAX_EXM_PORT_REGIONS	5

struct exm_device;

/* 
                                            
                                                 
                      
                                   
                                                                  
                                                        
                                            
                                       
                                
                                      
                                             
                                             
                                                   
                                                                     
 */
struct exm_info {
	struct exm_device *exm_dev;
	const char *name;
	const char *version;
	struct exm_mem mem[MAX_EXM_MAPS];
	struct exm_port port[MAX_EXM_PORT_REGIONS];
	long irq;
	unsigned long irq_flags;
	void *priv;
	 irqreturn_t(*handler) (int irq, struct exm_info *dev_info);
	int (*mmap) (struct exm_info *info, struct vm_area_struct *vma);
	int (*open) (struct exm_info *info, struct inode *inode);
	int (*release) (struct exm_info *info, struct inode *inode);
	int (*irqcontrol) (struct exm_info *info, s32 irq_on);
};

extern int __must_check
__exm_register_device(struct module *owner, struct device *parent, struct exm_info *info);

/*                                                           */
#define exm_register_device(parent, info) \
	__exm_register_device(THIS_MODULE, parent, info)

extern void exm_unregister_device(struct exm_info *info);
extern void exm_event_notify(struct exm_info *info);

/*                           */
#define EXM_IRQ_CUSTOM	-1
#define EXM_IRQ_NONE	0

/*                              */
#define EXM_MEM_NONE	0
#define EXM_MEM_PHYS	1
#define EXM_MEM_LOGICAL	2
#define EXM_MEM_VIRTUAL 3

/*                                */
#define EXM_PORT_NONE	0
#define EXM_PORT_X86	1
#define EXM_PORT_GPIO	2
#define EXM_PORT_OTHER	3

#endif				/*                      */
