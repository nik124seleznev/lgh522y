/*
   +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   Copyright (c) 2011 Synaptics, Inc.
   
   Permission is hereby granted, free of charge, to any person obtaining a copy of 
   this software and associated documentation files (the "Software"), to deal in 
   the Software without restriction, including without limitation the rights to use, 
   copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the 
   Software, and to permit persons to whom the Software is furnished to do so, 
   subject to the following conditions:
   
   The above copyright notice and this permission notice shall be included in all 
   copies or substantial portions of the Software.
   
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
   SOFTWARE.
   +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

#include <linux/module.h>
#include <linux/delay.h>
#include <linux/hrtimer.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/slab.h>
#include <linux/jiffies.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>

#include <mach/mt_gpio.h>

#include "s2200_driver.h"


/*                                 */
unsigned short SynaF34DataBase;
unsigned short SynaF34QueryBase;
unsigned short SynaF01DataBase;
unsigned short SynaF01CommandBase;
unsigned short SynaF01ControlBase;
unsigned short SynaF01QueryBase;

unsigned short SynaF34Reflash_BlockNum;
unsigned short SynaF34Reflash_BlockData;
unsigned short SynaF34ReflashQuery_BootID;
unsigned short SynaF34ReflashQuery_FlashPropertyQuery;
unsigned short SynaF34ReflashQuery_FirmwareBlockSize;
unsigned short SynaF34ReflashQuery_FirmwareBlockCount;
unsigned short SynaF34ReflashQuery_ConfigBlockSize;
unsigned short SynaF34ReflashQuery_ConfigBlockCount;

unsigned short SynaFirmwareBlockSize;
unsigned short SynaFirmwareBlockCount;
unsigned long SynaImageSize;

unsigned short SynaConfigBlockSize;
unsigned short SynaConfigBlockCount;
unsigned long SynaConfigImageSize;

unsigned short SynaBootloadID;

unsigned short SynaF34_FlashControl;

unsigned char *SynafirmwareImgData;
unsigned char *SynaconfigImgData;
unsigned char *SynalockImgData;
unsigned int SynafirmwareImgVersion;

unsigned char *my_image_bin;
unsigned long my_image_size;


int CompleteReflash(struct i2c_client *client);
int ConfigBlockReflash(struct i2c_client *client);
int CompleteReflash_Lockdown(struct i2c_client *client);
void SynaInitialize(struct i2c_client *client);
void SynaReadConfigInfo(struct i2c_client *client);
void SynaReadFirmwareInfo(struct i2c_client *client);
int SynaEnableFlashing(struct i2c_client *client);
int SynaProgramFirmware(struct i2c_client *client);
int SynaProgramConfiguration(struct i2c_client *client);
int SynaFinalizeReflash(struct i2c_client *client);
int SynaWaitForATTN(int time);

extern int synaptics_ts_write(struct i2c_client *client, u8 reg, u8 * buf, int len);
extern int synaptics_ts_read(struct i2c_client *client, u8 reg, int num, u8 *buf);


int FirmwareUpgrade(struct i2c_client *client, const char* fw_path, unsigned long fw_size, unsigned char* fw_start)
{
	int ret = 0;
	int fd = -1;
	mm_segment_t old_fs = 0;
	struct stat fw_bin_stat;
	unsigned long read_bytes;
	
	if(unlikely(fw_path[0] != 0)) {
		old_fs = get_fs();
		set_fs(get_ds());

		if ((fd = sys_open((const char __user *) fw_path, O_RDONLY, 0)) < 0) {
			TPD_ERR("Can not read FW binary from %s\n", fw_path);
			ret = -EEXIST;
			goto read_fail;
		}

		if ((ret = sys_newstat((char __user *) fw_path, (struct stat *)&fw_bin_stat)) < 0) {
			TPD_ERR("Can not read FW binary stat from %s\n", fw_path);
			goto fw_mem_alloc_fail;
		}

		my_image_size = fw_bin_stat.st_size;
		my_image_bin = kzalloc(sizeof(char) * (my_image_size+1), GFP_KERNEL);
		if (my_image_bin == NULL) {
			TPD_ERR("Can not allocate  memory\n");
			ret = -ENOMEM;
			goto fw_mem_alloc_fail;
		}

		read_bytes = sys_read(fd, (char __user *)my_image_bin, my_image_size);

		/*              */
		*(my_image_bin+my_image_size) = 0xFF;

		TPD_LOG("Touch FW image read %ld bytes from %s\n", read_bytes, fw_path);
		
	} else {
		my_image_size = fw_size-1;
		my_image_bin = (unsigned char *)(&fw_start[0]);
	}

	ret = CompleteReflash_Lockdown(client);
	if(ret < 0) {
		TPD_ERR("CompleteReflash_Lockdown fail\n");
	}

	if(unlikely(fw_path[0] != 0))
		kfree(my_image_bin);

fw_mem_alloc_fail:
	sys_close(fd);
read_fail:
	set_fs(old_fs);

	return ret;
}

static int writeRMI(struct i2c_client *client, u8 uRmiAddress, u8 *data, unsigned int length)
{
	return synaptics_ts_write(client, uRmiAddress, data, length);
}

static int readRMI(struct i2c_client *client, u8 uRmiAddress, u8 *data, unsigned int length)
{
	return synaptics_ts_read(client, uRmiAddress, length, data);
}

/*                                                                                     
                                                                                          
                                                           
 */
void SynaSetup(struct i2c_client *client)
{
    unsigned char address;
    unsigned char buffer[6];
	TPD_LOG("%s\n", __FUNCTION__);

	for (address = 0xe9; address > 0xd0; address = address - 6)
	{
    	readRMI(client, address, &buffer[0], 6);

		if(!buffer[5]) break;

		switch (buffer[5])
		{
			case 0x34:
				SynaF34DataBase = buffer[3];
				SynaF34QueryBase = buffer[0];
				break;
			case 0x01:
				SynaF01DataBase = buffer[3];
				SynaF01CommandBase = buffer[1];
				SynaF01ControlBase = buffer[2];
				SynaF01QueryBase = buffer[0];
				break;
		}
	}

	SynaF34Reflash_BlockNum = SynaF34DataBase;
	SynaF34Reflash_BlockData = SynaF34DataBase + 2;
	SynaF34ReflashQuery_BootID = SynaF34QueryBase;
	SynaF34ReflashQuery_FlashPropertyQuery = SynaF34QueryBase + 2;
	SynaF34ReflashQuery_FirmwareBlockSize = SynaF34QueryBase + 3;
	SynaF34ReflashQuery_FirmwareBlockCount = SynaF34QueryBase +5;
	SynaF34ReflashQuery_ConfigBlockSize = SynaF34QueryBase + 3;
	SynaF34ReflashQuery_ConfigBlockCount = SynaF34QueryBase + 7;

	SynafirmwareImgData = (unsigned char *)((&my_image_bin[0])+0x100);
	SynaconfigImgData   = (unsigned char *)(SynafirmwareImgData+SynaImageSize);
	SynafirmwareImgVersion = (unsigned int)(my_image_bin[7]);

	switch(SynafirmwareImgVersion)
	{
		case 2: 
			SynalockImgData = (unsigned char *)((&my_image_bin[0]) + 0xD0);
			break;
		case 3:
		case 4:
			SynalockImgData = (unsigned char *)((&my_image_bin[0]) + 0xC0);
			break;
		case 5:
			SynalockImgData = (unsigned char *)((&my_image_bin[0]) + 0xB0);
			break;
		default: break;
	}
}

/*                                           
 */
void SynaInitialize(struct i2c_client *client)
{	
	unsigned char uData[2];
	
	TPD_LOG("%s\n", __FUNCTION__);

	TPD_LOG("Initializing Reflash Process...\n");
	uData[0] = 0x00;
	writeRMI(client, 0xff, &uData[0], 1);

	SynaSetup(client);

	//                        
	uData[0] = 0x0f;
	writeRMI(client, SynaF01ControlBase+1, &uData[0], 1);

	readRMI(client, SynaF34ReflashQuery_FirmwareBlockSize, &uData[0], 2);

	SynaFirmwareBlockSize = uData[0] | (uData[1] << 8);
}

/*                                                                                          
                                                        
 */
void SynaReadFirmwareInfo(struct i2c_client *client)
{
	unsigned char uData[2];
	uData[0] = 0;
	uData[1] = 0;

	TPD_LOG("%s\n", __FUNCTION__);

	TPD_LOG("Read Firmware Info\n");

	readRMI(client, SynaF34ReflashQuery_FirmwareBlockSize, &uData[0], 2);
	SynaFirmwareBlockSize = uData[0] | (uData[1] << 8);

	readRMI(client, SynaF34ReflashQuery_FirmwareBlockCount, &uData[0], 2);
	SynaFirmwareBlockCount = uData[0] | (uData[1] << 8);
	SynaImageSize = SynaFirmwareBlockCount * SynaFirmwareBlockSize;
}

/*                                                                                        
                                                             
 */
void SynaReadConfigInfo(struct i2c_client *client)
{
	unsigned char uData[2];
	TPD_LOG("%s\n", __FUNCTION__);

	TPD_LOG("Read Config Info\n");

	readRMI(client, SynaF34ReflashQuery_ConfigBlockSize, &uData[0], 2);
	SynaConfigBlockSize = uData[0] | (uData[1] << 8);

	readRMI(client, SynaF34ReflashQuery_ConfigBlockCount, &uData[0], 2);
	SynaConfigBlockCount = uData[0] | (uData[1] << 8);
	SynaConfigImageSize = SynaConfigBlockCount * SynaConfigBlockSize;
}

/*                                                                                                 
 */
void SynaReadBootloadID(struct i2c_client *client)
{
	unsigned char uData[2];
	TPD_LOG("%s\n", __FUNCTION__);

	readRMI(client, SynaF34ReflashQuery_BootID, &uData[0], 2);
	SynaBootloadID = uData[0] + uData[1] * 0x100;
}

/*                                                                                                    
 */
void SynaWriteBootloadID(struct i2c_client *client)
{
	unsigned char uData[2];
	TPD_LOG("%s\n", __FUNCTION__);

	uData[0] = SynaBootloadID % 0x100;
	uData[1] = SynaBootloadID / 0x100;

	writeRMI(client, SynaF34Reflash_BlockData, &uData[0], 2);
}

/*                                                                               
 */
int SynaWaitForATTN(int time)
{
	int trial_us=0;

	while((mt_get_gpio_in(GPIO_CTP_EINT_PIN) != 0) && (trial_us < (time * 1000))) {
		udelay(1);
		trial_us++;
	}

	if (mt_get_gpio_in(GPIO_CTP_EINT_PIN) != 0)
		return -EBUSY;
	else
		return 0;
}

/*                                                                            
                                                                                         
 */
int SynaWaitATTN(struct i2c_client *client)
{
	int ret;
	unsigned char uData;
	unsigned char uStatus;

	ret = SynaWaitForATTN(1000);
	if (ret < 0) {
		TPD_ERR("SynaWaitForATTN 1000ms timeout error\n");
		return ret;
	}

	do {
 		readRMI(client, SynaF34_FlashControl, &uData, 1);
		readRMI(client, (SynaF01DataBase + 1), &uStatus, 1);
	} while (uData!= 0x80);

	return 0;
}

/*                                                 
 */
int SynaEnableFlashing(struct i2c_client *client)
{
	int ret;
	unsigned char uData;
	unsigned char uStatus;
	TPD_LOG("%s\n", __FUNCTION__);

	TPD_LOG("Enable Reflash...\n");
    
	//                                                                                          
	SynaReadBootloadID(client);
	SynaWriteBootloadID(client);

	//                                         
	do {
		readRMI(client, SynaF34_FlashControl, &uData, 1);
	} while (((uData & 0x0f) != 0x00));

	//           
	readRMI (client, SynaF01DataBase, &uStatus, 1);

	if ((uStatus &0x40) == 0)
	{
		//                                                                    
		//                                       
		uData = 0x0f;
		writeRMI(client, SynaF34_FlashControl, &uData, 1);
		ret = SynaWaitForATTN(100);
		if (ret < 0) {
			TPD_ERR("SynaWaitForATTN 100ms timeout error\n");
			return ret;
		}
		readRMI(client, (SynaF01DataBase + 1), &uStatus, 1);
		//                                                              
		SynaSetup(client);
		//                                                                                     
		//            
		do{
			readRMI(client, SynaF34_FlashControl, &uData, 1);
			//                                                                               
			//                                                 
		}while (uData != 0x80);
	}

	return 0;
}

/*                                                                                      
 */
int SynaProgramConfiguration(struct i2c_client *client)
{
	unsigned char uData[2];
	unsigned char *puData = (unsigned char *)&my_image_bin[0xb100];
	
	unsigned short blockNum;
	TPD_LOG("%s\n", __FUNCTION__);

	TPD_LOG("Program Configuration Section...\n");

	for (blockNum = 0; blockNum < SynaConfigBlockCount; blockNum++)
	{
	       uData[0] = blockNum & 0xff;
		uData[1] = (blockNum & 0xff00) >> 8;

		//                                                                                       
		writeRMI(client, SynaF34Reflash_BlockNum, &uData[0], 2);
		writeRMI(client, SynaF34Reflash_BlockData, puData, SynaConfigBlockSize);
		puData += SynaConfigBlockSize;

		//                                              
		uData[0] = 0x06;
		writeRMI(client, SynaF34_FlashControl, &uData[0], 1);
		if(SynaWaitATTN(client) < 0) return -1;
		TPD_LOG(".\n");
	}
	return 0;
}

/*                                                  
 */
int SynaFinalizeReflash(struct i2c_client *client)
{
	int ret;
	unsigned char uData;
	unsigned char uStatus;
	TPD_LOG("%s\n", __FUNCTION__);

	TPD_LOG("Finalizing Reflash...\n");

	//                                                                    
	//                                                                             
	uData = 1;
	writeRMI(client, SynaF01CommandBase, &uData, 1);

	ret = SynaWaitForATTN(100);
	if (ret < 0) {
		TPD_ERR("SynaWaitForATTN 100ms timeout error\n");
		return ret;
	}
	readRMI(client, SynaF01DataBase, &uData, 1);
	
	//                                                       
	do {
	   readRMI(client, SynaF34_FlashControl, &uStatus, 1);
	} while ((uStatus & 0x0f) != 0x00);
	readRMI(client, (SynaF01DataBase + 1), &uStatus, 1);

	SynaSetup(client);
	uData = 0;

	//                                                                   
	//                                                                           
	do {
		readRMI(client, SynaF01DataBase, &uData, 1);
	} while ((uData & 0x40) != 0);

	//                                                   
	SynaSetup(client);

	TPD_LOG("Reflash Completed. Please reboot.\n");

	return 0;
}

/*                                                                               
 */
int SynaFlashFirmwareWrite(struct i2c_client *client)
{
	unsigned char *puFirmwareData = (unsigned char *)&my_image_bin[0x100];
	unsigned char uData[2];
	unsigned short blockNum;
	TPD_LOG("%s\n", __FUNCTION__);

	for (blockNum = 0; blockNum < SynaFirmwareBlockCount; ++blockNum)
	{
		if(blockNum%100 == 0)
			TPD_LOG("blockNum = [%d], (SynaFirmwareBlockCount=%d)\n", blockNum, SynaFirmwareBlockCount);
		//                                                                                       
		uData[0] = blockNum & 0xff;
		uData[1] = (blockNum & 0xff00) >> 8;
		writeRMI(client, SynaF34Reflash_BlockNum, &uData[0], 2);

		writeRMI(client, SynaF34Reflash_BlockData, puFirmwareData, SynaFirmwareBlockSize);
		puFirmwareData += SynaFirmwareBlockSize;

		//                                         
		uData[0] = 2;
		writeRMI(client, SynaF34_FlashControl, &uData[0], 1);
		
	    if(SynaWaitATTN(client) < 0) return -1;
	}
	return 0;

}

/*                                                          
 */
int SynaProgramFirmware(struct i2c_client *client)
{
	int ret;
	unsigned char uData;    
	TPD_LOG("%s\n", __FUNCTION__);

	TPD_LOG("Program Firmware Section...\n");

	SynaWriteBootloadID(client);

	uData = 3; 
	writeRMI(client, SynaF34_FlashControl, &uData, 1);

	msleep(1000);
	
	SynaWaitATTN(client);

	ret = SynaFlashFirmwareWrite(client);
	if(ret < 0){
		TPD_ERR("SynaFlashFirmwareWrite fail\n");
		return ret;
	}

	return 0;
}


/*                                         
*/
int eraseConfigBlock(struct i2c_client *client)
{
	unsigned char uData;    
	TPD_LOG("%s\n", __FUNCTION__);

	//                                                                     
	SynaReadBootloadID(client);
	SynaWriteBootloadID(client);

	//                                
	uData = 7; 
	writeRMI(client, SynaF34_FlashControl, &uData, 1);

	if(SynaWaitATTN(client) < 0) return -1;

	return 0;
}

//                                                                                 
//                                                                                  
//                                    
//                                                                                    
//                                                         
void CRC_Calculate(unsigned short * data, unsigned short len)
{
	short i;
    unsigned long Data_CRC = 0xffffffff;
    unsigned long sum1 = (unsigned long)(Data_CRC & 0xFFFF);
    unsigned long sum2 = (unsigned long)(Data_CRC >> 16);
	TPD_LOG("%s\n", __FUNCTION__);

    for (i = 0; i < len; i++)
    {
        unsigned long temp = data[i];
        sum1 += temp;
        sum2 += sum1;
        sum1 = (unsigned long)((sum1 & 0xffff) + (sum1 >> 16));
        sum2 = (unsigned long)((sum2 & 0xffff) + (sum2 >> 16));
    }

    Data_CRC = (unsigned long)(sum2 << 16 | sum1);
}

int SynaBootloaderLock(struct i2c_client *client)
{
	int ret;
	unsigned short lockBlockCount;
    unsigned char *puFirmwareData = SynalockImgData;
    unsigned char uData[2];
    unsigned short uBlockNum;

	//                                     
	readRMI(client, (SynaF34QueryBase+ 2), &uData[0], 1);
 
	//                  
    if (uData[0] & 0x02)
	{ 
		TPD_LOG("Device unlocked. Lock it first...\n");
		//                                                                             
		//                                                                         
		switch (SynafirmwareImgVersion)
		{
			case 2:
				lockBlockCount = 3;
				break;
			case 3:
			case 4:
				lockBlockCount = 4;
				break;
			case 5:
				lockBlockCount = 5;
				break;
			default:
				lockBlockCount = 0;
				break;
		}

		//                                       
		//                                                                              
		//                                                                                 
		//                                                                                   
		//                                      
		for (uBlockNum = 0; uBlockNum < lockBlockCount; ++uBlockNum)
		{
			uData[0] = uBlockNum & 0xff;
			uData[1] = (uBlockNum & 0xff00) >> 8;

			/*                    */
			readRMI(client, SynaF34Reflash_BlockNum, &uData[0], 2);

			/*                  */
			writeRMI(client, SynaF34Reflash_BlockData, puFirmwareData, SynaFirmwareBlockSize);

			/*                         */
			puFirmwareData += SynaFirmwareBlockSize;

			/*                                    */
			uData[0] = 4;
			writeRMI(client, SynaF34_FlashControl, &uData[0], 1);

			/*                                                                             */
			if(SynaWaitATTN(client) < 0) return -1;
		}
		TPD_LOG("Device locking done.\n");
		
		//                                                     
		//                                                                             
		//                                                                                  
		ret = SynaEnableFlashing(client);
		if(ret < 0) {
			TPD_ERR("SynaEnableFlashing fail\n");
			return ret;
		}
	}
	else TPD_LOG("Device already locked.\n");

	return 0;
}

/*                                                   
*/
int ConfigBlockReflash(struct i2c_client *client)
{
	int ret;
	unsigned char uData[2];
	TPD_LOG("%s\n", __FUNCTION__);

    SynaInitialize(client);
	
	SynaReadConfigInfo(client);
	
	SynaReadFirmwareInfo(client);

	SynaF34_FlashControl = SynaF34DataBase + SynaFirmwareBlockSize + 2;

	ret = SynaEnableFlashing(client);
	if(ret < 0) {
		TPD_ERR("SynaEnableFlashing fail\n");
		return ret;
	}
	
	//                                     
	readRMI(client, (SynaF34QueryBase + 2), &uData[0], 1);
 
	//                  
	if (uData[0] & 0x02)
	{ 
	   ret = SynaFinalizeReflash(client);
	   if(ret < 0) {
			TPD_ERR("SynaFinalizeReflash fail\n");
			return ret;
	   }
	   return 0;
	   //                                           
	}

	ret = eraseConfigBlock(client);
	if(ret < 0) {
		TPD_ERR("eraseConfigBlock fail\n");
		return ret;
	}

	ret = SynaProgramConfiguration(client);
	if(ret < 0) {
		TPD_ERR("SynaProgramConfiguration fail\n");
		return ret;
	}

	ret = SynaFinalizeReflash(client);
	if(ret < 0) {
		TPD_ERR("SynaFinalizeReflash fail\n");
		return ret;
	}

	return 0;
}

/*                                                                                                
*/
int CompleteReflash(struct i2c_client *client)
{   
	int ret;
	TPD_LOG("%s\n", __FUNCTION__);
	SynaInitialize(client);
	
	SynaReadConfigInfo(client);
	
	SynaReadFirmwareInfo(client);
	
	SynaF34_FlashControl = SynaF34DataBase + SynaFirmwareBlockSize + 2;
    
	ret = SynaEnableFlashing(client);
	if(ret < 0) {
		TPD_ERR("SynaEnableFlashing fail\n");
		return ret;
	}

	ret = SynaProgramFirmware(client);
	if(ret < 0) {
		TPD_ERR("SynaProgramFirmware fail\n");
		return ret;
	}

	ret = SynaProgramConfiguration(client);
	if(ret < 0) {
		TPD_ERR("SynaProgramConfiguration fail\n");
		return ret;
	}

	ret = SynaFinalizeReflash(client);
	if(ret < 0) {
		TPD_ERR("SynaFinalizeReflash fail\n");
		return ret;
	}

	return 0;
}

int CompleteReflash_Lockdown(struct i2c_client *client)
{   
	int ret;
	TPD_LOG("%s\n", __FUNCTION__);
	
	SynaInitialize(client);
	
	SynaReadConfigInfo(client);
	
	SynaReadFirmwareInfo(client);

	SynaF34_FlashControl = SynaF34DataBase + SynaFirmwareBlockSize + 2;

	ret = SynaEnableFlashing(client);
	if(ret < 0) {
		TPD_ERR("SynaEnableFlashing fail\n");
		return ret;
	}

	ret = SynaBootloaderLock(client);
	if(ret < 0) {
		TPD_ERR("SynaBootloaderLock fail\n");
		return ret;
	}

	ret = SynaProgramFirmware(client);
	if(ret < 0) {
		TPD_ERR("SynaProgramFirmware fail\n");
		return ret;
	}

	ret = SynaProgramConfiguration(client);
	if(ret < 0) {
		TPD_ERR("SynaProgramConfiguration fail\n");
		return ret;
	}

	ret = SynaFinalizeReflash(client);
	if(ret < 0) {
		TPD_ERR("SynaFinalizeReflash fail\n");
		return ret;
	}

	return 0;
}

