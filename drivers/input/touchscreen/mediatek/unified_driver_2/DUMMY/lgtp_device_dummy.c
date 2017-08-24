/***************************************************************************
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 *    File  	: lgtp_device_dummy.c
 *    Author(s)   : D3 BSP Touch Team < d3-bsp-touch@lge.com >
 *    Description :
 *
 ***************************************************************************/
#define LGTP_MODULE "[DUMMY]"

/*                                                                           
               
                                                                           */
#include <linux/input/unified_driver_2/lgtp_common.h>

#include <linux/input/unified_driver_2/lgtp_common_driver.h>
#include <linux/input/unified_driver_2/lgtp_platform_api.h>


/*                                                                           
                              
                                                                           */


/*                                                                           
         
                                                                            */


/*                                                                           
                  
                                                                           */


/*                                                                           
           
                                                                           */
static const char defaultFirmware[] = "leadingUI/dummy_firmware.img";

//                                                                    
//                                              
//                                                                 
//                                         
//                                                    
//                                                                       
//                                        
//                                                                    
static TouchState dummyDeviceState = STATE_UNKNOWN;

/*                                                                           
                            
                                                                           */


/*                                                                           
                           
                                                                           */


/*                                                                           
                 
                                                                           */


/*                                                                           
                           
                                                                           */
static ssize_t show_device_name(struct i2c_client *client, char *buf)
{
	int ret = 0;

	TOUCH_FUNC();

	ret += sprintf(buf+ret, "%s\n", "Dummy Device");
	
	return ret;
}

static LGE_TOUCH_ATTR(device_name, S_IRUGO | S_IWUSR, show_device_name, NULL);

static struct attribute *Dummy_attribute_list[] = {
	&lge_touch_attr_device_name.attr,
	NULL,
};



static int Dummy_Initialize(struct i2c_client *client)
{
	TOUCH_FUNC();

	return TOUCH_SUCCESS;
}

static void Dummy_Reset(struct i2c_client *client)
{
	TOUCH_FUNC();

	TouchResetCtrl(0);
	msleep(10);
	TouchResetCtrl(1);
	msleep(200);

	dummyDeviceState = STATE_NORMAL;
}


//                                                                    
//                                       
//            
//                                    
//                                                                              
//                                                                    
//                                                                                                                  
//                                                                    
static int Dummy_Connect(void)
{
	TOUCH_FUNC();

	return TOUCH_SUCCESS;
}

//                                                                    
//                              
//            
//                                 
//                                                   
//                                                                    
static int Dummy_InitRegister(struct i2c_client *client)
{
	TOUCH_FUNC();

	return TOUCH_SUCCESS;
}

//                                                                    
//                                
//            
//                    
//                                                                                                      
//                                                                    
static void Dummy_ClearInterrupt(struct i2c_client *client)
{
	TOUCH_FUNC();

	return;
}

//                                                                    
//                                  
//            
//                      
//                                               
//                                                                    
static int Dummy_InterruptHandler(struct i2c_client *client,TouchReadData *pData)
{
	return TOUCH_SUCCESS;
}

//                                                                    
//                                    
//            
//                                            
//                                                              
//                                                                    
static int Dummy_ReadIcFirmwareInfo(struct i2c_client *client, TouchFirmwareInfo *pFwInfo)
{
	TOUCH_FUNC();

	/*                                                                      */
	pFwInfo->moduleMakerID = 2;
	pFwInfo->moduleVersion = 0;
	pFwInfo->modelID = 3;
	pFwInfo->isOfficial = 1;
	pFwInfo->version = 4;

	return TOUCH_SUCCESS;	
}

//                                                                    
//                                    
//            
//                                                              
//                                                                      
//                                          
//                                                                    
static int Dummy_GetBinFirmwareInfo(struct i2c_client *client, char *pFilename, TouchFirmwareInfo *pFwInfo)
{
	int ret = 0;
	const struct firmware *fw = NULL;
	u8 *pBin = NULL;
	char *pFwFilename = NULL;

	TOUCH_FUNC();
	
	if( pFilename == NULL ) {
		pFwFilename = (char *)defaultFirmware;
	} else {
		pFwFilename = pFilename;
	}

	TOUCH_LOG("Firmware filename = %s\n", pFwFilename);
	
	/*                                             */
	ret = request_firmware(&fw, pFwFilename, &client->dev);
	if( ret ) {
		TOUCH_ERR("failed at request_firmware() ( error = %d )\n", ret);
		return TOUCH_FAIL;
	}

	pBin = (u8 *)(fw->data);

	/*                                             */
	pFwInfo->moduleMakerID = ( *(pBin) >> 4 ) & 0xF;
	pFwInfo->moduleVersion = *(pBin) & 0xF;
	pFwInfo->modelID = *(pBin+1);
	pFwInfo->isOfficial = ( *(pBin+3) >> 7 ) & 0x1;
	pFwInfo->version = *(pBin+3) & 0x7F;

	/*                            */
	release_firmware(fw);

	return TOUCH_SUCCESS;
		
}

//                                                                    
//                                
//            
//                               
//                                                    
//                                                                                                      
//                                                                    
static int Dummy_UpdateFirmware(struct i2c_client *client, char *pFilename)
{
	int ret = 0;
	const struct firmware *fw = NULL;
	u8 *pBin = NULL;
	char *pFwFilename = NULL;

	TOUCH_FUNC();
	
	if( pFilename == NULL ) {
		pFwFilename = (char *)defaultFirmware;
	} else {
		pFwFilename = pFilename;
	}

	TOUCH_LOG("Firmware filename = %s\n", pFwFilename);
	
	/*                                             */
	ret = request_firmware(&fw, pFwFilename, &client->dev);
	if( ret ) {
		TOUCH_ERR("failed at request_firmware() ( error = %d )\n", ret);
		return TOUCH_FAIL;
	}

	pBin = (u8 *)(fw->data);

	/*                                      */

	/*                            */
	release_firmware(fw);

	return TOUCH_SUCCESS;
	
}

//                                                                    
//                             
//            
//                                  
//                                                                    
static int Dummy_SetLpwgMode(struct i2c_client *client, TouchState newState, LpwgSetting  *pLpwgSetting)
{
	int ret = TOUCH_SUCCESS;
	
	TOUCH_FUNC();

	if( dummyDeviceState == newState ) {
		TOUCH_LOG("device state is same as driver requested\n");
		return TOUCH_SUCCESS;
	}

	switch( newState )
	{
		case STATE_NORMAL:
			TOUCH_LOG("device was set to NORMAL\n");
			break;
		case STATE_OFF:
			TOUCH_LOG("device was set to OFF\n");
			break;
		case STATE_KNOCK_ON_ONLY:
			TOUCH_LOG("device was set to KNOCK_ON_ONLY\n");
			break;
		case STATE_KNOCK_ON_CODE:
			TOUCH_LOG("device was set to KNOCK_ON_CODE\n");
			break;
		case STATE_NORMAL_HOVER:
			TOUCH_LOG("device was set to NORMAL_HOVER\n");
			break;
		case STATE_HOVER:
			TOUCH_LOG("device was set to HOVER\n");
			break;
		default:
			TOUCH_LOG("invalid state ( state = %d )\n", newState);
			ret = TOUCH_FAIL;
			break;
			
	}

	if( ret == TOUCH_SUCCESS ) {
		dummyDeviceState = newState;
	}

	return TOUCH_SUCCESS;
	
}

//                                                                    
//                                 
//            
//                                            
//                                                                                   
//                                                                                        
//                                                                    
static int Dummy_DoSelfDiagnosis(struct i2c_client *client, int* pRawStatus, int* pChannelStatus, char* pBuf, int bufSize, int* pDataLen)
{
	int dataLen = 0;

	TOUCH_FUNC();

	*pRawStatus = TOUCH_SUCCESS;
	*pChannelStatus = TOUCH_SUCCESS;

	dataLen += sprintf(pBuf, "%s", "========= Additional Information =========\n");
	dataLen += sprintf(pBuf+dataLen, "%s", "Device Name = Dummy\n");

	*pDataLen = dataLen;

	return TOUCH_SUCCESS;
}

//                                                                    
//                                
//            
//                                   
//                                                                    
static int Dummy_AccessRegister(struct i2c_client *client, int cmd, int reg, int *pValue)
{
	int ret = 0;
	
	TOUCH_FUNC();

	switch( cmd )
	{
		case READ_IC_REG:
			ret = Touch_I2C_Read_Byte(client, (u8)reg, (u8 *)pValue);
			if( ret == TOUCH_FAIL ) {
				return TOUCH_FAIL;
			}
			break;

		case WRITE_IC_REG:
			ret = Touch_I2C_Write_Byte(client, (u8)reg, (u8)*pValue);
			if( ret == TOUCH_FAIL ) {
				return TOUCH_FAIL;
			}
			break;

		default:
			TOUCH_ERR("Invalid access command ( cmd = %d )\n", cmd);
			return TOUCH_FAIL;
			break;
	}

	return TOUCH_SUCCESS;

}

//                                                                    
//                               
//            
//                                              
//                                                                                  
//                                                                                             
//                                                                    
static void Dummy_NotifyHandler(struct i2c_client *client, TouchNotify notify, int data)
{
	switch( notify )
	{
		case NOTIFY_CALL:
			TOUCH_LOG("Call was notified ( data = %d )\n", data);
			break;

		case NOTIFY_Q_COVER:
			TOUCH_LOG("Quick Cover was notified ( data = %d )\n", data);
			break;

		default:
			TOUCH_ERR("Invalid notification ( notify = %d )\n", notify);
			break;
	}

	return;

}

TouchDeviceSpecificFunction Dummy_Func = {

	.Initialize = Dummy_Initialize,
	.Reset = Dummy_Reset,
	.Connect = Dummy_Connect,
	.InitRegister = Dummy_InitRegister,
	.ClearInterrupt = Dummy_ClearInterrupt,
	.InterruptHandler = Dummy_InterruptHandler,
	.ReadIcFirmwareInfo = Dummy_ReadIcFirmwareInfo,
	.GetBinFirmwareInfo = Dummy_GetBinFirmwareInfo,
	.UpdateFirmware = Dummy_UpdateFirmware,
	.SetLpwgMode = Dummy_SetLpwgMode,
	.DoSelfDiagnosis = Dummy_DoSelfDiagnosis,
	.AccessRegister = Dummy_AccessRegister,
	.NotifyHandler = Dummy_NotifyHandler,
	.device_attribute_list = Dummy_attribute_list,
	
};


/*             */


