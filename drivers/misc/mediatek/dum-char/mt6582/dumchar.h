#ifndef __DUMCHAR_H__
#define __DUMCHAR_H__


#include <linux/ioctl.h>
#include <linux/cdev.h>
#include <linux/mtd/mtd.h>
#include <linux/semaphore.h>
#include <linux/mmc/sd_misc.h>

/*
                           
 */
#define DUMCHAR_DEBUG
//                                                             

#ifdef DUMCHAR_DEBUG
#define DDEBUG(fmt, args...) printk( KERN_DEBUG "dumchar_debug: " fmt, ## args)
#else
#define DDEBUG(fmt, args...) 
#endif


#define DUMCHAR_MAJOR        0   /*                          */
#define MAX_SD_BUFFER		(512)
#define ALIE_LEN		512


//                   

struct dumchar_dev {
	char *dumname;	//                      
	char actname[64];	//                                            
	struct semaphore sem;     /*                  */
	dev_type type; //                           
	unsigned long long size; //              
	struct cdev cdev;	
	Region region; //        
	unsigned long long start_address; //        
	unsigned int mtd_index;	 //        
};

struct Region_Info {
	Region region;
	unsigned long long	size_Byte;
};

struct file_obj {
	struct file *act_filp; 
	int index; //                         
};

#define REGION_NUM 						8
#define EXT_CSD_BOOT_SIZE_MULT          226 /*   */
#define EXT_CSD_RPMB_SIZE_MULT          168 /*   */

#define	MSDC_RAW_DEVICE					"/dev/misc-sd"

#define mtd_for_each_device(mtd)			\
	for ((mtd) = __mtd_next_device(0);		\
	     (mtd) != NULL;				\
	     (mtd) = __mtd_next_device(mtd->index + 1))

#ifndef CONFIG_MTK_NEW_COMBO_EMMC_SUPPORT
#define EMMC_PART_BOOT1		(BOOT_1)
#define EMMC_PART_BOOT2		(BOOT_2)
#define EMMC_PART_RPMB		(RPMB)
#define EMMC_PART_GP1 		(GP_1)
#define EMMC_PART_GP2 		(GP_2)
#define EMMC_PART_GP3 		(GP_3)
#define EMMC_PART_GP4 		(GP_4)
#define EMMC_PART_USER 		(USER)
#define NAND_PART_USER 		(USER)

#endif

#endif /*              */
