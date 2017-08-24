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

#include "s3320_driver.h"

/*                                 */
unsigned short SynaF34DataBase;
unsigned short SynaF34QueryBase;
unsigned short SynaF01DataBase;
unsigned short SynaF01CommandBase;
unsigned short SynaF01QueryBase;

unsigned short SynaF34Reflash_BlockNum;
unsigned short SynaF34Reflash_BlockData;
unsigned short SynaF34ReflashQuery_BootID;
unsigned short SynaF34ReflashQuery_FlashPropertyQuery;
unsigned short SynaF34ReflashQuery_BlockSize;
unsigned short SynaF34ReflashQuery_FirmwareBlockCount;
unsigned short SynaF34ReflashQuery_ConfigBlockCount;

unsigned char SynaF01Query43Length;

unsigned short SynaFirmwareBlockSize;
unsigned short SynaFirmwareBlockCount;
unsigned long SynaImageSize;

unsigned short SynaConfigBlockSize;
unsigned short SynaConfigBlockCount;
unsigned long SynaConfigImageSize;

unsigned short SynaBootloadID;

unsigned short SynaF34_FlashControl;
unsigned short SynaF34_FlashStatus;

unsigned char *SynafirmwareImgData;
unsigned char *SynaconfigImgData;
unsigned char *SynalockImgData;
unsigned int SynafirmwareImgVersion;

unsigned char * ConfigBlock;

enum FlashCommand
{
  m_uF34ReflashCmd_FirmwareCrc        = 0x01,   //                        
  m_uF34ReflashCmd_FirmwareWrite      = 0x02,
  m_uF34ReflashCmd_EraseAll           = 0x03,
  m_uF34ReflashCmd_LockDown           = 0x04,   //                         
  m_uF34ReflashCmd_ConfigRead         = 0x05,
  m_uF34ReflashCmd_ConfigWrite        = 0x06,
  m_uF34ReflashCmd_EraseUIConfig      = 0x07,
  m_uF34ReflashCmd_Enable             = 0x0F,
  m_uF34ReflashCmd_QuerySensorID      = 0x08,
  m_uF34ReflashCmd_EraseBLConfig      = 0x09,
  m_uF34ReflashCmd_EraseDisplayConfig = 0x0A,
};

char SynaFlashCommandStr[0x0C][0x20] = 
{ 
  "",
  "FirmwareCrc",
  "FirmwareWrite",
  "EraseAll",
  "LockDown",
  "ConfigRead",
  "ConfigWrite",
  "EraseUIConfig",
  "Enable",
  "QuerySensorID",
  "EraseBLConfig",
  "EraseDisplayConfig",
};
/*                                 */







unsigned char *my_image_bin;
unsigned long my_image_size;


int CompleteReflash(struct i2c_client *client);
//                                                  
//                                                        
void SynaInitialize(struct i2c_client *client);
//                                                   
void SynaReadFirmwareInfo(struct i2c_client *client);
void SynaEnableFlashing(struct i2c_client *client);
void SynaProgramFirmware(struct i2c_client *client);
//                                                        
void SynaFinalizeReflash(struct i2c_client *client);
//                              
unsigned int SynaWaitForATTN(int timeout, struct i2c_client *client);
void eraseAllBlock(struct i2c_client *client);
void EraseConfigBlock(struct i2c_client *client);
bool CheckTouchControllerType(struct i2c_client *client);

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

#if 1//        
	ret = CompleteReflash(client);
	if(ret < 0) {
		TPD_ERR("CompleteReflash fail\n");
	}
#else
	ret = CompleteReflash_Lockdown(client);
	if(ret < 0) {
		TPD_ERR("CompleteReflash_Lockdown fail\n");
	}
#endif

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

bool CheckFlashStatus(enum FlashCommand command, struct i2c_client *client)
{
  unsigned char uData = 0;
	//                                                                                     
	//            
  readRMI(client, SynaF34_FlashStatus, &uData, 1);

  //                        
    //                                                                                                  

  return !(uData & 0x3F);
}

void SynaImageParser(struct i2c_client *client)
{
TPD_LOG("%s\n", __FUNCTION__);
  //                 
  SynaImageSize = ( (unsigned int)my_image_bin[0x08] | 
                    (unsigned int)my_image_bin[0x09] <<8 | 
                    (unsigned int)my_image_bin[0x0A] <<16|
                    (unsigned int)my_image_bin[0x0B] <<24);
	SynafirmwareImgData = (unsigned char *)((&my_image_bin[0]) + 0x100);
	SynaconfigImgData   = (unsigned char *)(SynafirmwareImgData + SynaImageSize);
	SynafirmwareImgVersion = (unsigned int)(my_image_bin[7]);

	switch (SynafirmwareImgVersion)
	{
	   case 2: 
		   SynalockImgData = (unsigned char *)((&my_image_bin[0]) + 0xD0);
		   break;
	   case 3:
	   case 4:
		   SynalockImgData = (unsigned char *)((&my_image_bin[0])+ 0xC0);
		   break;
	   case 5:
	   case 6:
		   SynalockImgData = (unsigned char *)((&my_image_bin[0]) + 0xB0);
	   default: break;
	}
}

void SynaBootloaderLock(struct i2c_client *client)
{
	unsigned short lockBlockCount = 0;
  unsigned char uData[2] = {0,};
  unsigned short uBlockNum = 0;
  enum FlashCommand cmd;

TPD_LOG("%s\n", __FUNCTION__);

  if (my_image_bin[0x1E] == 0)
  {
    TPD_LOG("Skip lockdown process with this .img\n");
    return;
  }
	//                                     
	readRMI(client, (SynaF34QueryBase+ 1), &uData[0], 1);
 
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
      case 6:
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
			writeRMI(client, SynaF34Reflash_BlockNum, &uData[0], 2);

			/*                  */
			writeRMI(client, SynaF34Reflash_BlockData, SynalockImgData, SynaFirmwareBlockSize);

			/*                         */
			SynalockImgData += SynaFirmwareBlockSize;

			/*                                    */
			cmd = m_uF34ReflashCmd_LockDown;
			writeRMI(client, SynaF34_FlashControl, (unsigned char*)&cmd, 1);

			/*                                                                             */
			SynaWaitForATTN(1000, client);
      CheckFlashStatus(cmd, client);
		}

		//                                                     
		//                                                                             
		//                                                                                  
		SynaEnableFlashing(client);
	}
	else TPD_LOG("Device already locked.\n");
}


/*                                                                                       
                                                                                          
                                                           
 */
void SynaScanPDT(struct i2c_client *client)
{
	unsigned char address=0;
	unsigned char uData[2]={0,};
	unsigned char buffer[6]={0,};
	TPD_LOG("%s\n", __FUNCTION__);

	for (address = 0xe9; address > 0xc0; address = address - 6)
	{
		readRMI(client, address, buffer, 6);

		switch (buffer[5])
		{
			case 0x34:
				SynaF34DataBase = buffer[3];
				SynaF34QueryBase = buffer[0];
				break;
			case 0x01:
				SynaF01DataBase = buffer[3];
				SynaF01CommandBase = buffer[1];
				SynaF01QueryBase = buffer[0];
				break;
		}
	}

	SynaF34Reflash_BlockNum = SynaF34DataBase;
	SynaF34Reflash_BlockData = SynaF34DataBase + 1;
	SynaF34ReflashQuery_BootID = SynaF34QueryBase;
	SynaF34ReflashQuery_FlashPropertyQuery = SynaF34QueryBase + 1;
	SynaF34ReflashQuery_BlockSize = SynaF34QueryBase + 2;
  SynaF34ReflashQuery_FirmwareBlockCount = SynaF34QueryBase + 3;
  SynaF34_FlashControl = SynaF34DataBase + 2;
  SynaF34_FlashStatus = SynaF34DataBase + 3;

	readRMI(client, SynaF34ReflashQuery_FirmwareBlockCount, buffer, 4);
	SynaFirmwareBlockCount  = buffer[0] | (buffer[1] << 8);
  SynaConfigBlockCount    = buffer[2] | (buffer[3] << 8);

  readRMI(client, SynaF34ReflashQuery_BlockSize, &uData[0], 2);
	SynaConfigBlockSize = SynaFirmwareBlockSize = uData[0] | (uData[1] << 8);

  //          
  readRMI(client, (SynaF01DataBase + 1), buffer, 1);
}
/*                                                                                     
                                                                                          
                                                           
 */








/*                                           
 */
void SynaInitialize(struct i2c_client *client)
{	
	u8 data;

	TPD_LOG("%s\n", __FUNCTION__);
	TPD_LOG("\nInitializing Reflash Process...\n");

	data = 0x00;
	writeRMI(client, 0xff, &data, 1);

  SynaImageParser(client);

	SynaScanPDT(client);
}
/*                                           
 */

	



	//                        



/*                                                                                          
                                                        
 */
void SynaReadFirmwareInfo(struct i2c_client *client)
{
	unsigned char uData[3] = {0,};
  unsigned char product_id[11] = {0,};
  int firmware_version = 0;
	TPD_LOG("%s\n", __FUNCTION__);
  
  readRMI(client, SynaF01QueryBase + 11, product_id, 10);
  product_id[10] = '\0';
  TPD_LOG("Read Product ID %s\n", product_id);

  readRMI(client, SynaF01QueryBase + 18, uData, 3);
  firmware_version = uData[2] << 16 | uData[1] << 8 | uData[0];
  TPD_LOG("Read Firmware Info %d\n", firmware_version);

  CheckTouchControllerType(client);
  //                                  
}

/*                                                                                                 
 */
void SynaReadBootloadID(struct i2c_client *client)
{
	unsigned char uData[2] = {0,};
	TPD_LOG("%s\n", __FUNCTION__);

	readRMI(client, SynaF34ReflashQuery_BootID, &uData[0], 2);
	SynaBootloadID = uData[0] | (uData[1] << 8);
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





/*                                                                                        
                                                             
 */




/*                                                                                                 
 */


/*                                                                                                    
 */



/*                                                 
 */
void SynaEnableFlashing(struct i2c_client *client)
{
	unsigned char uStatus = 0;
  enum FlashCommand cmd;
	TPD_LOG("%s\n", __FUNCTION__);
	TPD_LOG("Enable Reflash...\n");

	readRMI (client, SynaF01DataBase, &uStatus, 1);

//        
TPD_LOG("APK_TEST uStatus= 0x%02x\n", uStatus);

	if ((uStatus &0x40) == 0 /*        */)
	{
    //                                                                                          
    SynaReadBootloadID(client);
    SynaWriteBootloadID(client);

		//                                                                    
		//                                       
    cmd = m_uF34ReflashCmd_Enable;
		writeRMI(client, SynaF34_FlashControl, (unsigned char*)&cmd, 1);
		SynaWaitForATTN(1000,client);

    //                     
    //                                

		//                                                              
		SynaScanPDT(client);
		
		//                                                                                     
		//            
    CheckFlashStatus(cmd,client);
	}
}

/*                                                                               
 */
unsigned int SynaWaitForATTN(int timeout, struct i2c_client *client)
{
  unsigned char uStatus;
  //                  
  //                             
  //              
#ifdef POLLING
  do {
    uStatus = 0x00;
    readRMI((SynaF01DataBase + 1), &uStatus, 1);
    if (uStatus != 0)
      break;
    Sleep(duration);
    times++;
  } while (times < retry);
  
  if (times == retry)
    return -1;
#else
  //                                                    
  // 
    //          
  // 
	int trial_us=0;

	while((mt_get_gpio_in(GPIO_CTP_EINT_PIN) != 0) && (trial_us < (timeout * 1000))) {
		udelay(1);
		trial_us++;
	}

	if (mt_get_gpio_in(GPIO_CTP_EINT_PIN) != 0){
		TPD_LOG("interrupt pin is busy...");
		return -EBUSY;
	}

	readRMI(client, (SynaF01DataBase + 1), &uStatus, 1);
#endif
  return 0;
}
/*                                                                               
 */




/*                                                                            
                                                                                         
 */




/*                                                 
 */

    
	//                                                                                          

	//                                         

	//           

//        

		//                                                                    
		//                                       
		//                                                              
		//                                                                                     
		//            
			//                                                                               
			//                                                 
			//        
			//                                           


/*                                                  
 */
void SynaFinalizeReflash(struct i2c_client *client)
{
	unsigned char uData;

  char deviceStatusStr[7][20] = {
    "0x00",
    "0x01",
    "0x02",
    "0x03",
    "config CRC failed",
    "firmware CRC failed",
    "CRC in progress\n"
    };

	TPD_LOG("%s\n", __FUNCTION__);
	TPD_LOG("\nFinalizing Reflash...\n");

	//                                                                    
	//                                                                             
	uData = 1;
	writeRMI(client, SynaF01CommandBase, &uData, 1);

  //                                                              
  //                                            
  msleep(150);
	SynaWaitForATTN(1000, client);

	SynaScanPDT(client);

  readRMI(client, SynaF01DataBase, &uData, 1);

  if ((uData & 0x40) != 0)
  {
    TPD_LOG("\nDevice is in bootloader mode (status: %s).\n", deviceStatusStr[uData & 0xF]);
  }
  else
  {
    TPD_LOG("\nReflash Completed and Succeed.\n");
  }
}

/*                                                                               
 */
void SynaFlashFirmwareWrite(struct i2c_client *client)
{
	unsigned char *puFirmwareData = SynafirmwareImgData;
	unsigned char uData[2];
	unsigned short blockNum;
  enum FlashCommand cmd;

	TPD_LOG("%s\n", __FUNCTION__);
	for (blockNum = 0; blockNum < SynaFirmwareBlockCount; ++blockNum)
	{
    if (blockNum == 0)
    {
		  //                                                                                       
	    uData[0] = blockNum & 0xff;
		  uData[1] = (blockNum & 0xff00) >> 8;
		  writeRMI(client, SynaF34Reflash_BlockNum, &uData[0], 2);
    }

		writeRMI(client, SynaF34Reflash_BlockData, puFirmwareData, SynaFirmwareBlockSize);
		puFirmwareData += SynaFirmwareBlockSize;

		//                                         
    cmd = m_uF34ReflashCmd_FirmwareWrite;
		writeRMI(client, SynaF34_FlashControl, (unsigned char*)&cmd, 1);
		
	  SynaWaitForATTN(1000, client);
    CheckFlashStatus(cmd, client);
//                    
#if 1//        
    if (blockNum % 100 == 0)
       TPD_LOG("blk %d / %d\n", blockNum, SynaFirmwareBlockCount);
#endif
	}
//                    
#if 1//        
  TPD_LOG("blk %d / %d\n", SynaFirmwareBlockCount, SynaFirmwareBlockCount);
#endif
}

/*                                                                               
 */
void SynaFlashConfigWrite(struct i2c_client *client)
{
	unsigned char *puConfigData = SynaconfigImgData;
	unsigned char uData[2];
	unsigned short blockNum;
  enum FlashCommand cmd;

	TPD_LOG("%s\n", __FUNCTION__);
	for (blockNum = 0; blockNum < SynaConfigBlockCount; ++blockNum)
	{
		//                                                                                       
	  uData[0] = blockNum & 0xff;
		uData[1] = (blockNum & 0xff00) >> 8;
		writeRMI(client, SynaF34Reflash_BlockNum, &uData[0], 2);

		writeRMI(client, SynaF34Reflash_BlockData, puConfigData, SynaConfigBlockSize);
		puConfigData += SynaConfigBlockSize;

		//                                       
    cmd = m_uF34ReflashCmd_ConfigWrite;
		writeRMI(client, SynaF34_FlashControl, (unsigned char*)&cmd, 1);
		
	  SynaWaitForATTN(1000, client);
    CheckFlashStatus(cmd, client);
//                    
#if 1
    if (blockNum % 100 == 0)
       TPD_LOG("blk %d / %d\n", blockNum, SynaConfigBlockCount);
#endif
	}
//                    
#if 1
  TPD_LOG("blk %d / %d\n", SynaConfigBlockCount, SynaConfigBlockCount);
#endif
}

/*                                                          
 */
void SynaProgramFirmware(struct i2c_client *client)
{
	TPD_LOG("%s\n", __FUNCTION__);
	TPD_LOG("\nProgram Firmware Section...\n");

  eraseAllBlock(client);

	SynaFlashFirmwareWrite(client);

  SynaFlashConfigWrite(client);
}

/*                                                          
 */
void SynaUpdateConfig(struct i2c_client *client)
{
	TPD_LOG("%s\n", __FUNCTION__);
	TPD_LOG("\nUpdate Config Section...\n");

  EraseConfigBlock(client);

  SynaFlashConfigWrite(client);
}

/*                                         
*/
void eraseAllBlock(struct i2c_client *client)
{ 
  enum FlashCommand cmd;

	TPD_LOG("%s\n", __FUNCTION__);
	//                                                                     
	SynaReadBootloadID(client);
	SynaWriteBootloadID(client);

	//                                
	cmd = m_uF34ReflashCmd_EraseAll; 
	writeRMI(client, SynaF34_FlashControl, (unsigned char*)&cmd, 1);

	SynaWaitForATTN(6000, client);
  CheckFlashStatus(cmd, client);
}

/*                                         
*/
void EraseConfigBlock(struct i2c_client *client)
{  
  enum FlashCommand cmd;

	TPD_LOG("%s\n", __FUNCTION__);
	//                                                                     
	SynaReadBootloadID(client);
	SynaWriteBootloadID(client);

	//                                
	cmd = m_uF34ReflashCmd_EraseUIConfig; 
	writeRMI(client, SynaF34_FlashControl, (unsigned char*)&cmd, 1);

	SynaWaitForATTN(2000, client);
  CheckFlashStatus(cmd, client);
}

//                                                                                                            
bool CheckTouchControllerType(struct i2c_client *client)
{
  int ID=0;
  char buffer[5]={0,};
	char controllerType[20]={0,};
	unsigned char uData[4]={0,};

	TPD_LOG("%s\n", __FUNCTION__);
	readRMI(client, (SynaF01QueryBase + 22), &SynaF01Query43Length, 1); //  

	if ((SynaF01Query43Length & 0x0f) > 0)
	{
		readRMI(client, (SynaF01QueryBase + 23), &uData[0], 1);
		if (uData[0] & 0x01)
		{
			readRMI(client, (SynaF01QueryBase + 17), &uData[0], 2);

			ID = ((int)uData[0] | ((int)uData[1] << 8));
			//                                        

			if (strstr(controllerType, buffer) != 0) return true;
				return false;
		}
		else return false;
	}
	else return false;
}

#if 0//        
bool CheckFimrwareRevision(struct i2c_client *client)
{
	unsigned char uData[16];
	char revision[106];

	TPD_LOG("%s\n", __FUNCTION__);
	readRMI((SynaF01QueryBase + 28 + SynaF01Query43Length), &uData[0], 16);

	for (int i = 0; i < 0; i++)
	{
		while (uData[i] != NULL)
		{
			revision[i] = char(uData[0]);
		}
	}

	if (strcmp(revision, FW_REVISION) == 0) return true;
		return false;
}
#endif

/*                                                                                      
 */
	



		//                                                                                       

		//                                              

/*                                                  
 */


	//                                                                    
	//                                                                             

	
	//                                                       


	//                                                                   
	//                                                                           

	//                                                   



/*                                                                               
 */

		//                                                                                       


		//                                         
		


/*                                                          
 */




	




/*                                         
*/

	//                                                                     

	//                                



//                                                                                 
//                                                                                  
//                                    
//                                                                                    
//                                                         





	//                                     
 
	//                  
		//                                                                             
		//                                                                         

		//                                       
		//                                                                              
		//                                                                                 
		//                                                                                   
		//                                      

			/*                    */

			/*                  */

			/*                         */

			/*                                    */

			/*                                                                             */
		
		//                                                     
		//                                                                             
		//                                                                                  


/*                                                   
*/
	

	
	


	
	//                                     
 
	//                  
	   //                                           





/*                                                                                                
*/
int CompleteReflash(struct i2c_client *client)
{
  bool bFlashAll = true;

	SynaInitialize(client);

	SynaReadFirmwareInfo(client);

	SynaEnableFlashing(client);

  SynaBootloaderLock(client);

  if (bFlashAll)
  	SynaProgramFirmware(client);
  else
    SynaUpdateConfig(client);

	SynaFinalizeReflash(client);

	return 0;
}

/*                                                                                                
*/
	
	
	
    





	
	
	







