/*
                    
  
  
 */

#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include "kd_camera_hw.h"
#include "eeprom.h"
#include "eeprom_define.h"

#include "dummy_eeprom.h"
#include <asm/system.h>		/*         */

/*                                   */
/*                             */
#ifdef EEPROM_DEBUG
#define EEPROMDB(fmt, args...)	pr_debug(fmt, ##args)
#else
#define EEPROMDB(x, ...)
#endif


static DEFINE_SPINLOCK(g_EEPROMLock);	/*         */

#define EEPROM_I2C_BUSNUM 1
static struct i2c_board_info kd_eeprom_dev __initdata =
    { I2C_BOARD_INFO("dummy_eeprom", 0xA0 >> 1) };

/*                                                                              
 
                                                                               */
#define EEPROM_ICS_REVISION 1	/*               */
/*                                                                              
 
                                                                               */
#define EEPROM_DRVNAME "dummy_eeprom"
#define EEPROM_I2C_GROUP_ID 0
/*                                                                              
 
                                                                               */
/*                
                                                                                
                                                  
                                                              
                                  
                         
                         
   */
static struct i2c_client *g_pstI2Cclient;

/*                           */
static dev_t g_EEPROMdevno = MKDEV(EEPROM_DEV_MAJOR_NUMBER, 0);
static struct cdev *g_pEEPROM_CharDrv;
/*                                 */
static struct class *EEPROM_class;
static atomic_t g_EEPROMatomic;
/*                                                                              
 
                                                                               */
/*                                                                                    */
int iWriteEEPROM(u16 a_u2Addr, u32 a_u4Bytes, u8 *puDataInBytes)
{
	int i4RetValue = 0;
	u32 u4Index = 0;
	char puSendCmd[8] = { (char)(a_u2Addr >> 8), (char)(a_u2Addr & 0xFF),
		0, 0, 0, 0, 0, 0
	};
	if (a_u4Bytes + 2 > 8) {
		EEPROMDB
		    ("[EEPROM] exceed I2c-mt65xx.c 8 bytes limitation (include address 2 Byte)\n");
		return -1;
	}

	for (u4Index = 0; u4Index < a_u4Bytes; u4Index += 1) {
		puSendCmd[(u4Index + 2)] = puDataInBytes[u4Index];
	}

	i4RetValue = i2c_master_send(g_pstI2Cclient, puSendCmd, (a_u4Bytes + 2));
	if (i4RetValue != (a_u4Bytes + 2)) {
		EEPROMDB("[EEPROM] I2C write  failed!!\n");
		return -1;
	}
	mdelay(10);		/*                                                       */

	/*                                             */
	return 0;
}


/*                                                                                    */
int iReadEEPROM(u16 a_u2Addr, u32 ui4_length, u8 *a_puBuff)
{
	int i4RetValue = 0;
	char puReadCmd[2] = { (char)(a_u2Addr >> 8), (char)(a_u2Addr & 0xFF) };

	/*                                       */

	if (ui4_length > 8) {
		EEPROMDB("[EEPROM] exceed I2c-mt65xx.c 8 bytes limitation\n");
		return -1;
	}
	spin_lock(&g_EEPROMLock);	/*         */
	g_pstI2Cclient->addr = g_pstI2Cclient->addr & (I2C_MASK_FLAG | I2C_WR_FLAG);
	spin_unlock(&g_EEPROMLock);	/*         */

	/*                                         */
	i4RetValue = i2c_master_send(g_pstI2Cclient, puReadCmd, 2);
	if (i4RetValue != 2) {
		EEPROMDB("[EEPROM] I2C send read address failed!!\n");
		return -1;
	}
	/*                                         */
	i4RetValue = i2c_master_recv(g_pstI2Cclient, (char *)a_puBuff, ui4_length);
	if (i4RetValue != ui4_length) {
		EEPROMDB("[EEPROM] I2C read data failed!!\n");
		return -1;
	}
	spin_lock(&g_EEPROMLock);	/*         */
	g_pstI2Cclient->addr = g_pstI2Cclient->addr & I2C_MASK_FLAG;
	spin_unlock(&g_EEPROMLock);	/*         */

	/*                                            */
	return 0;
}


static int iWriteData(unsigned int ui4_offset, unsigned int ui4_length, unsigned char *pinputdata)
{
	int i4RetValue = 0;
	int i4ResidueDataLength;
	u32 u4IncOffset = 0;
	u32 u4CurrentOffset;
	u8 *pBuff;

	EEPROMDB("[EEPROM] iWriteData\n");


	if (ui4_offset + ui4_length >= 0x2000) {
		EEPROMDB("[EEPROM] Write Error!! S-24CS64A not supprt address >= 0x2000!!\n");
		return -1;
	}

	i4ResidueDataLength = (int)ui4_length;
	u4CurrentOffset = ui4_offset;
	pBuff = pinputdata;
	EEPROMDB("[EEPROM] iWriteData u4CurrentOffset is %d\n", u4CurrentOffset);
	do {
		if (i4ResidueDataLength >= 6) {
			i4RetValue = iWriteEEPROM((u16) u4CurrentOffset, 6, pBuff);
			if (i4RetValue != 0) {
				EEPROMDB("[EEPROM] I2C iWriteData failed!!\n");
				return -1;
			}
			u4IncOffset += 6;
			i4ResidueDataLength -= 6;
			u4CurrentOffset = ui4_offset + u4IncOffset;
			pBuff = pinputdata + u4IncOffset;
		} else {
			i4RetValue =
			    iWriteEEPROM((u16) u4CurrentOffset, i4ResidueDataLength, pBuff);
			if (i4RetValue != 0) {
				EEPROMDB("[EEPROM] I2C iWriteData failed!!\n");
				return -1;
			}
			u4IncOffset += 6;
			i4ResidueDataLength -= 6;
			u4CurrentOffset = ui4_offset + u4IncOffset;
			pBuff = pinputdata + u4IncOffset;
			/*        */
		}
	} while (i4ResidueDataLength > 0);
	EEPROMDB("[EEPROM] iWriteData done\n");

	return 0;
}

/*                                                        */
static int iReadData(unsigned int ui4_offset, unsigned int ui4_length, unsigned char *pinputdata)
{
	int i4RetValue = 0;
	int i4ResidueDataLength;
	u32 u4IncOffset = 0;
	u32 u4CurrentOffset;
	u8 *pBuff;
	EEPROMDB("[EEPROM] iReadData\n");

	if (ui4_offset + ui4_length >= 0x2000) {
		EEPROMDB("[EEPROM] Read Error!! S-24CS64A not supprt address >= 0x2000!!\n");
		return -1;
	}

	i4ResidueDataLength = (int)ui4_length;
	u4CurrentOffset = ui4_offset;
	pBuff = pinputdata;
	do {
		if (i4ResidueDataLength >= 8) {
			i4RetValue = iReadEEPROM((u16) u4CurrentOffset, 8, pBuff);
			if (i4RetValue != 0) {
				EEPROMDB("[EEPROM] I2C iReadData failed!!\n");
				return -1;
			}
			u4IncOffset += 8;
			i4ResidueDataLength -= 8;
			u4CurrentOffset = ui4_offset + u4IncOffset;
			pBuff = pinputdata + u4IncOffset;
		} else {
			i4RetValue = iReadEEPROM((u16) u4CurrentOffset, i4ResidueDataLength, pBuff);
			if (i4RetValue != 0) {
				EEPROMDB("[EEPROM] I2C iReadData failed!!\n");
				return -1;
			}
			u4IncOffset += 8;
			i4ResidueDataLength -= 8;
			u4CurrentOffset = ui4_offset + u4IncOffset;
			pBuff = pinputdata + u4IncOffset;
			/*        */
		}
	} while (i4ResidueDataLength > 0);
/*                                                                                                                                                          */
	EEPROMDB("[EEPROM] iReadData done\n");
	return 0;
}


/*                                                                              
 
                                                                               */
#define NEW_UNLOCK_IOCTL
#ifndef NEW_UNLOCK_IOCTL
static int EEPROM_Ioctl(struct inode *a_pstInode,
			struct file *a_pstFile, unsigned int a_u4Command, unsigned long a_u4Param)
#else
static long EEPROM_Ioctl(struct file *file, unsigned int a_u4Command, unsigned long a_u4Param)
#endif
{
	int i4RetValue = 0;
	u8 *pBuff = NULL;
	u8 *pWorkingBuff = NULL;
	stEEPROM_INFO_STRUCT *ptempbuf;

#ifdef EEPROMGETDLT_DEBUG
	struct timeval ktv1, ktv2;
	unsigned long TimeIntervalUS;
#endif

	if (_IOC_NONE == _IOC_DIR(a_u4Command)) {
	} else {
		pBuff = (u8 *) kmalloc(sizeof(stEEPROM_INFO_STRUCT), GFP_KERNEL);

		if (NULL == pBuff) {
			EEPROMDB("[EEPROM] ioctl allocate mem failed\n");
			return -ENOMEM;
		}

		if (_IOC_WRITE & _IOC_DIR(a_u4Command)) {
			if (copy_from_user((u8 *) pBuff, (u8 *) a_u4Param, sizeof(stEEPROM_INFO_STRUCT))) {	/*                             */
				kfree(pBuff);
				EEPROMDB("[EEPROM] ioctl copy from user failed\n");
				return -EFAULT;
			}
		}
	}

	ptempbuf = (stEEPROM_INFO_STRUCT *) pBuff;
	pWorkingBuff = (u8 *) kmalloc(ptempbuf->u4Length, GFP_KERNEL);
	if (NULL == pWorkingBuff) {
		kfree(pBuff);
		EEPROMDB("[EEPROM] ioctl allocate mem failed\n");
		return -ENOMEM;
	}
/*                                                                                                                            */


	if (copy_from_user((u8 *) pWorkingBuff, (u8 *) ptempbuf->pu1Params, ptempbuf->u4Length)) {
		kfree(pBuff);
		kfree(pWorkingBuff);
		EEPROMDB("[EEPROM] ioctl copy from user failed\n");
		return -EFAULT;
	}

	switch (a_u4Command) {
	case EEPROMIOC_S_WRITE:
		EEPROMDB("[EEPROM] Write CMD\n");
#ifdef EEPROMGETDLT_DEBUG
		do_gettimeofday(&ktv1);
#endif
		i4RetValue = iWriteData((u16) ptempbuf->u4Offset, ptempbuf->u4Length, pWorkingBuff);
#ifdef EEPROMGETDLT_DEBUG
		do_gettimeofday(&ktv2);
		if (ktv2.tv_sec > ktv1.tv_sec) {
			TimeIntervalUS = ktv1.tv_usec + 1000000 - ktv2.tv_usec;
		} else {
			TimeIntervalUS = ktv2.tv_usec - ktv1.tv_usec;
		}
		EEPROMDB("Write data %d bytes take %lu us\n", ptempbuf->u4Length, TimeIntervalUS);
#endif
		break;
	case EEPROMIOC_G_READ:
		EEPROMDB("[EEPROM] Read CMD\n");
#ifdef EEPROMGETDLT_DEBUG
		do_gettimeofday(&ktv1);
#endif
		EEPROMDB("[EEPROM] offset %d\n", ptempbuf->u4Offset);
		EEPROMDB("[EEPROM] length %d\n", ptempbuf->u4Length);
/*                                                                                                          */

		i4RetValue = iReadData((u16) ptempbuf->u4Offset, ptempbuf->u4Length, pWorkingBuff);
/*                                                                                                         */


#ifdef EEPROMGETDLT_DEBUG
		do_gettimeofday(&ktv2);
		if (ktv2.tv_sec > ktv1.tv_sec) {
			TimeIntervalUS = ktv1.tv_usec + 1000000 - ktv2.tv_usec;
		} else {
			TimeIntervalUS = ktv2.tv_usec - ktv1.tv_usec;
		}
		EEPROMDB("Read data %d bytes take %lu us\n", ptempbuf->u4Length, TimeIntervalUS);
#endif

		break;
	default:
		EEPROMDB("[EEPROM] No CMD\n");
		i4RetValue = -EPERM;
		break;
	}

	if (_IOC_READ & _IOC_DIR(a_u4Command)) {
		/*                                                                      */
		EEPROMDB("[EEPROM] to user length %d\n", ptempbuf->u4Length);
/*                                                                                                   */
		if (copy_to_user
		    ((u8 __user *) ptempbuf->pu1Params, (u8 *) pWorkingBuff, ptempbuf->u4Length)) {
			kfree(pBuff);
			kfree(pWorkingBuff);
			EEPROMDB("[EEPROM] ioctl copy to user failed\n");
			return -EFAULT;
		}
	}

	kfree(pBuff);
	kfree(pWorkingBuff);
	return i4RetValue;
}


static u32 g_u4Opened;
/*         */
/*            */
/*                                                        */
/*                                                             */
static int EEPROM_Open(struct inode *a_pstInode, struct file *a_pstFile)
{
	EEPROMDB("[S24EEPROM] EEPROM_Open\n");
	spin_lock(&g_EEPROMLock);
	if (g_u4Opened) {
		spin_unlock(&g_EEPROMLock);
		return -EBUSY;
	} else {
		g_u4Opened = 1;
		atomic_set(&g_EEPROMatomic, 0);
	}
	spin_unlock(&g_EEPROMLock);

	/*                                                                     */
	/*   */
	/*                                                    */
	/*              */
	/*   */

	return 0;
}

/*            */
/*                                                              */
/*                                       */
/*                                  */
/*                                  */
static int EEPROM_Release(struct inode *a_pstInode, struct file *a_pstFile)
{
	spin_lock(&g_EEPROMLock);

	g_u4Opened = 0;

	atomic_set(&g_EEPROMatomic, 0);

	spin_unlock(&g_EEPROMLock);

	return 0;
}

static const struct file_operations g_stEEPROM_fops = {
	.owner = THIS_MODULE,
	.open = EEPROM_Open,
	.release = EEPROM_Release,
	/*                       */
	.unlocked_ioctl = EEPROM_Ioctl
};

#define EEPROM_DYNAMIC_ALLOCATE_DEVNO 1
inline static int RegisterEEPROMCharDrv(void)
{
	struct device *EEPROM_device = NULL;

#if EEPROM_DYNAMIC_ALLOCATE_DEVNO
	if (alloc_chrdev_region(&g_EEPROMdevno, 0, 1, EEPROM_DRVNAME)) {
		EEPROMDB("[EEPROM] Allocate device no failed\n");

		return -EAGAIN;
	}
#else
	if (register_chrdev_region(g_EEPROMdevno, 1, EEPROM_DRVNAME)) {
		EEPROMDB("[EEPROM] Register device no failed\n");

		return -EAGAIN;
	}
#endif

	/*                 */
	g_pEEPROM_CharDrv = cdev_alloc();

	if (NULL == g_pEEPROM_CharDrv) {
		unregister_chrdev_region(g_EEPROMdevno, 1);

		EEPROMDB("[EEPROM] Allocate mem for kobject failed\n");

		return -ENOMEM;
	}
	/*                         */
	cdev_init(g_pEEPROM_CharDrv, &g_stEEPROM_fops);

	g_pEEPROM_CharDrv->owner = THIS_MODULE;

	/*               */
	if (cdev_add(g_pEEPROM_CharDrv, g_EEPROMdevno, 1)) {
		EEPROMDB("[EEPROM] Attatch file operation failed\n");

		unregister_chrdev_region(g_EEPROMdevno, 1);

		return -EAGAIN;
	}

	EEPROM_class = class_create(THIS_MODULE, "EEPROMdrv");
	if (IS_ERR(EEPROM_class)) {
		int ret = PTR_ERR(EEPROM_class);
		EEPROMDB("Unable to create class, err = %d\n", ret);
		return ret;
	}
	EEPROM_device = device_create(EEPROM_class, NULL, g_EEPROMdevno, NULL, EEPROM_DRVNAME);

	return 0;
}

inline static void UnregisterEEPROMCharDrv(void)
{
	/*                     */
	cdev_del(g_pEEPROM_CharDrv);

	unregister_chrdev_region(g_EEPROMdevno, 1);

	device_destroy(EEPROM_class, g_EEPROMdevno);
	class_destroy(EEPROM_class);
}


/*                                                                      */
#ifndef EEPROM_ICS_REVISION
static int EEPROM_i2c_detect(struct i2c_client *client, int kind, struct i2c_board_info *info);
#elif 0
static int EEPROM_i2c_detect(struct i2c_client *client, struct i2c_board_info *info);
#else
#endif
static int EEPROM_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id);
static int EEPROM_i2c_remove(struct i2c_client *);

static const struct i2c_device_id EEPROM_i2c_id[] = { {EEPROM_DRVNAME, 0}, {} };

#if 0				/*                                                       */
static unsigned short force[] =
    { EEPROM_I2C_GROUP_ID, S24CS64A_DEVICE_ID, I2C_CLIENT_END, I2C_CLIENT_END };
#else
/*                                                                                                            */
#endif
/*                                                                 */
/*                                                                         */


static struct i2c_driver EEPROM_i2c_driver = {
	.probe = EEPROM_i2c_probe,
	.remove = EEPROM_i2c_remove,
/*                              */
	.driver.name = EEPROM_DRVNAME,
	.id_table = EEPROM_i2c_id,
};

#ifndef EEPROM_ICS_REVISION
static int EEPROM_i2c_detect(struct i2c_client *client, int kind, struct i2c_board_info *info)
{
	strcpy(info->type, EEPROM_DRVNAME);
	return 0;
}
#endif
static int EEPROM_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int i4RetValue = 0;
	EEPROMDB("[EEPROM] Attach I2C\n");
/*                                */

	/*                       */
	spin_lock(&g_EEPROMLock);	/*         */
	g_pstI2Cclient = client;
	g_pstI2Cclient->addr = S24CS64A_DEVICE_ID >> 1;
	spin_unlock(&g_EEPROMLock);	/*         */

	EEPROMDB("[EEPROM] g_pstI2Cclient->addr = 0x%8x\n", g_pstI2Cclient->addr);
	/*                      */
	i4RetValue = RegisterEEPROMCharDrv();

	if (i4RetValue) {
		EEPROMDB("[EEPROM] register char device failed!\n");
		return i4RetValue;
	}


	EEPROMDB("[EEPROM] Attached!!\n");
	return 0;
}

static int EEPROM_i2c_remove(struct i2c_client *client)
{
	return 0;
}

static int EEPROM_probe(struct platform_device *pdev)
{
	return i2c_add_driver(&EEPROM_i2c_driver);
}

static int EEPROM_remove(struct platform_device *pdev)
{
	i2c_del_driver(&EEPROM_i2c_driver);
	return 0;
}

/*                    */
static struct platform_driver g_stEEPROM_Driver = {
	.probe = EEPROM_probe,
	.remove = EEPROM_remove,
	.driver = {
		   .name = EEPROM_DRVNAME,
		   .owner = THIS_MODULE,
		   }
};


static struct platform_device g_stEEPROM_Device = {
	.name = EEPROM_DRVNAME,
	.id = 0,
	.dev = {
		}
};

static int __init EEPROM_i2C_init(void)
{
	i2c_register_board_info(EEPROM_I2C_BUSNUM, &kd_eeprom_dev, 1);
	if (platform_driver_register(&g_stEEPROM_Driver)) {
		EEPROMDB("failed to register EEPROM driver\n");
		return -ENODEV;
	}

	if (platform_device_register(&g_stEEPROM_Device)) {
		EEPROMDB("failed to register EEPROM driver\n");
		return -ENODEV;
	}

	return 0;
}

static void __exit EEPROM_i2C_exit(void)
{
	platform_driver_unregister(&g_stEEPROM_Driver);
}
module_init(EEPROM_i2C_init);
module_exit(EEPROM_i2C_exit);

MODULE_DESCRIPTION("EEPROM driver");
MODULE_AUTHOR("Sean Lin <Sean.Lin@Mediatek.com>");
MODULE_LICENSE("GPL");
