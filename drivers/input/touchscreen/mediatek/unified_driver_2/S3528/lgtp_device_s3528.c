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
 *    File  	: lgtp_device_s3528.c
 *    Author(s)   : D3 BSP Touch Team < d3-bsp-touch@lge.com >
 *    Description :
 *
 ***************************************************************************/
#define LGTP_MODULE "[S3528]"

/*                                                                           
               
                                                                           */
#include <linux/input/unified_driver_2/lgtp_common.h>

#include <linux/input/unified_driver_2/lgtp_common_driver.h>
#include <linux/input/unified_driver_2/lgtp_platform_api.h>
#include <linux/input/unified_driver_2/lgtp_device_s3528.h>


/*                                                                           
                              
                                                                           */
/*                                   
                               
                              
                                        
                            
                            
                  
                  
                                
                                         
                       
                   
                                    
                           
                            
 */

#define RMI_DEVICE_CONTROL			0x01
#define TOUCHPAD_SENSORS			0x12
#define FLASH_MEMORY_MANAGEMENT		0x34
#define LPWG_CONTROL				0x51
#define ANALOG_CONTROL				0x54
#define SENSOR_CONTROL				0x55

/*                                 
                                                                     
 */
#define DEVICE_CONTROL_REG 			(ts->common_fc.dsc.control_base)
#define MANUFACTURER_ID_REG			(ts->common_fc.dsc.query_base)
#define CUSTOMER_FAMILY_REG			(ts->common_fc.dsc.query_base+2)
#define FW_REVISION_REG				(ts->common_fc.dsc.query_base+3)
#define PRODUCT_ID_REG				(ts->common_fc.dsc.query_base+11)
#define DEVICE_RESET_REG 			(ts->common_fc.dsc.command_base)

#define COMMON_PAGE					(ts->common_fc.function_page)
#define LPWG_PAGE					(ts->lpwg_fc.function_page)
#define FINGER_PAGE					(ts->finger_fc.function_page)
#define ANALOG_PAGE					(ts->analog_fc.function_page)
#define FLASH_PAGE					(ts->flash_fc.function_page)
#define SENSOR_PAGE					(ts->sensor_fc.function_page)

#define DEVICE_STATUS_REG			(ts->common_fc.dsc.data_base)
#define INTERRUPT_STATUS_REG		(ts->common_fc.dsc.data_base+1)
#define INTERRUPT_ENABLE_REG		(ts->common_fc.dsc.control_base+1)

/*                  */
#define FINGER_DATA_REG_START		(ts->finger_fc.dsc.data_base)
#define OBJECT_ATTENTION_REG		(ts->finger_fc.dsc.data_base+3)
#define FINGER_REPORT_REG			(ts->finger_fc.dsc.control_base+7)
#define OBJECT_REPORT_ENABLE_REG	(ts->finger_fc.dsc.control_base+8)
#define WAKEUP_GESTURE_ENABLE_REG	(ts->finger_fc.dsc.control_base+12)

#define DYNAMIC_SENSING_CONTROL_REG	(ts->analog_fc.dsc.control_base+40)

#define FLASH_CONFIG_ID_REG			(ts->flash_fc.dsc.control_base)
#define FLASH_STATUS_REG			(ts->flash_fc.dsc.data_base+3)

#define LPWG_STATUS_REG				(ts->lpwg_fc.dsc.data_base)
#define LPWG_DATA_REG				(ts->lpwg_fc.dsc.data_base+1)
#define LPWG_OVER_TAPCOUNT			(ts->lpwg_fc.dsc.data_base+73)
#define LPWG_TAPCOUNT_REG			(ts->lpwg_fc.dsc.control_base)
#define LPWG_MIN_INTERTAP_REG		(ts->lpwg_fc.dsc.control_base+1)
#define LPWG_MAX_INTERTAP_REG		(ts->lpwg_fc.dsc.control_base+2)
#define LPWG_TOUCH_SLOP_REG			(ts->lpwg_fc.dsc.control_base+3)
#define LPWG_TAP_DISTANCE_REG		(ts->lpwg_fc.dsc.control_base+4)
#define LPWG_INTERRUPT_DELAY_REG	(ts->lpwg_fc.dsc.control_base+6)
#define LPWG_TAPCOUNT_REG2			(ts->lpwg_fc.dsc.control_base+7)
#define LPWG_MIN_INTERTAP_REG2		(ts->lpwg_fc.dsc.control_base+8)
#define LPWG_MAX_INTERTAP_REG2		(ts->lpwg_fc.dsc.control_base+9)
#define LPWG_TOUCH_SLOP_REG2		(ts->lpwg_fc.dsc.control_base+10)
#define LPWG_TAP_DISTANCE_REG2		(ts->lpwg_fc.dsc.control_base+11)
#define LPWG_INTERRUPT_DELAY_REG2	(ts->lpwg_fc.dsc.control_base+13)
#define LPWG_CONTROL_REG		0x1B; /*                           */

#define MAX_PRESSURE			255
#define LPWG_BLOCK_SIZE			7
#define KNOCKON_DELAY			68	/*       */
#define KNOCKCODE_DELAY			25  //     

#define QCOVERX1	124
#define QCOVERX2	956
#define QCOVERY1	30
#define QCOVERY2	862

/*                                                                           
         
                                                                            */
/*                                    
 */
#define GET_X_POSITION(_msb_reg, _lsb_reg) \
	(((u16)((_msb_reg << 8)  & 0xFF00)  | (u16)((_lsb_reg) & 0xFF)))
#define GET_Y_POSITION(_msb_reg, _lsb_reg) \
	(((u16)((_msb_reg << 8)  & 0xFF00)  | (u16)((_lsb_reg) & 0xFF)))
#define GET_WIDTH_MAJOR(_width_x, _width_y) \
	((_width_x - _width_y) > 0) ? _width_x : _width_y
#define GET_WIDTH_MINOR(_width_x, _width_y) \
	((_width_x - _width_y) > 0) ? _width_y : _width_x

#define GET_ORIENTATION(_width_y, _width_x) \
	((_width_y - _width_x) > 0) ? 0 : 1
#define GET_PRESSURE(_pressure) \
		_pressure


/*                                                                           
                  
                                                                           */

/*                                                                           
           
                                                                           */
#if defined ( TOUCH_MODEL_C70 )
static const char defaultFirmware[] = "synaptics/c70/PLG455-V1.02-PR1741017_DS5.2.12.0.1013_40047182.img";
#elif defined ( TOUCH_MODEL_Y90 )
static const char defaultFirmware[] = "synaptics/y90/PLG465-V0.01-PR1741017-DS5.2.12.0.1013-40050181.img";
#elif defined ( TOUCH_MODEL_Y70 )
static const char defaultFirmware[] = "synaptics/y70/PLG456-V1.01-PR1741017_DS5.2.12.0.1013_40047181.img";
#elif defined ( TOUCH_MODEL_C90 )
static const char defaultFirmware[] = "synaptics/c90/PLG431-V1.26_PR1741017_DS5.2.12.1013_4005019A-C90-DDIC-2CUT.img";
#elif defined ( TOUCH_MODEL_B2L )
static const char defaultFirmware[] = "synaptics/b2l/PLG387_T021_PR1705825-DS5.5.1.0.1066-10055115.img";
#else
#error "Model should be defined"
#endif

struct synaptics_ts_data *ts =NULL;
static struct synaptics_ts_exp_fhandler rmidev_fhandler;
int enable_rmi_dev = 0;

int incoming_call = 0;

unsigned int qcover = 0;

/*                                                                           
                            
                                                                           */
//                                                                 
extern int F54TestHandle(struct synaptics_ts_data *ts, int input, int mode, char *buf);

extern struct mutex* pMutexTouch;
/*                                                                           
                           
                                                                           */

/*                                                                           
                                     
                                                                           */
static int S3528_Initialize(struct i2c_client *client);
static void S3528_Reset(struct i2c_client *client);
static int S3528_Connect(void);
static int S3528_InitRegister(struct i2c_client *client);
static void S3528_ClearInterrupt(struct i2c_client *client);
static int S3528_InterruptHandler(struct i2c_client *client,TouchReadData *pData);
static int S3528_ReadIcFirmwareInfo(struct i2c_client *client, TouchFirmwareInfo *pFwInfo);
static int S3528_GetBinFirmwareInfo(struct i2c_client *client, char *pFilename, TouchFirmwareInfo *pFwInfo);
static int S3528_UpdateFirmware(struct i2c_client *client, char *pFilename);
static int S3528_SetLpwgMode(struct i2c_client *client, TouchState newState, LpwgSetting  *pLpwgSetting);
static int S3528_DoSelfDiagnosis(struct i2c_client *client, int* pRawStatus, int* pChannelStatus, char* pBuf, int bufSize, int* pDataLen);
static int S3528_AccessRegister(struct i2c_client *client, int cmd, int reg, int *pValue);


/*                                                                           
                 
                                                                           */
static int synaptics_ts_page_data_read(struct i2c_client *client,
	 u8 page, u8 reg, int size, u8 *data)
{
	int ret = 0;

	ret = Touch_I2C_Write_Byte(client, PAGE_SELECT_REG, page);
	if( ret == TOUCH_FAIL ) {
		return TOUCH_FAIL;
	}
	
	ret = Touch_I2C_Read(client, reg, data, size);
	if( ret == TOUCH_FAIL ) {
		return TOUCH_FAIL;
	}

	ret = Touch_I2C_Write_Byte(client, PAGE_SELECT_REG, DEFAULT_PAGE);
	if( ret == TOUCH_FAIL ) {
		return TOUCH_FAIL;
	}

	return TOUCH_SUCCESS;
	
}
 
static int synaptics_ts_page_data_write(struct i2c_client *client,
	 u8 page, u8 reg, int size, u8 *data)
{
	int ret = 0;

	ret = Touch_I2C_Write_Byte(client, PAGE_SELECT_REG, page);
	if( ret == TOUCH_FAIL ) {
		return TOUCH_FAIL;
	}
	
	ret = Touch_I2C_Write(client, reg, data, size);
	if( ret == TOUCH_FAIL ) {
		return TOUCH_FAIL;
	}

	ret = Touch_I2C_Write_Byte(client, PAGE_SELECT_REG, DEFAULT_PAGE);
	if( ret == TOUCH_FAIL ) {
		return TOUCH_FAIL;
	}

	return TOUCH_SUCCESS;

}
 
static int synaptics_ts_page_data_write_byte(struct i2c_client *client,
	 u8 page, u8 reg, u8 data)
{
	int ret = 0;

	ret = Touch_I2C_Write_Byte(client, PAGE_SELECT_REG, page);
	if( ret == TOUCH_FAIL ) {
		return TOUCH_FAIL;
	}
	
	ret = Touch_I2C_Write_Byte(client, reg, data);
	if( ret == TOUCH_FAIL ) {
		return TOUCH_FAIL;
	}

	ret = Touch_I2C_Write_Byte(client, PAGE_SELECT_REG, DEFAULT_PAGE);
	if( ret == TOUCH_FAIL ) {
		return TOUCH_FAIL;
	}

	return TOUCH_SUCCESS;

}

static int read_page_description_table(struct i2c_client *client)
{
	int ret = 0;
	struct function_descriptor buffer;

	unsigned short address = 0;
	unsigned short page_num = 0;

	TOUCH_FUNC();

	memset(&buffer, 0x0, sizeof(struct function_descriptor));
	memset(&ts->common_fc, 0x0, sizeof(struct ts_ic_function));
	memset(&ts->finger_fc, 0x0, sizeof(struct ts_ic_function));
	memset(&ts->lpwg_fc, 0x0, sizeof(struct ts_ic_function));
	memset(&ts->sensor_fc, 0x0, sizeof(struct ts_ic_function));
	memset(&ts->analog_fc, 0x0, sizeof(struct ts_ic_function));
	memset(&ts->flash_fc, 0x0, sizeof(struct ts_ic_function));

	for (page_num = 0; page_num < PAGE_MAX_NUM; page_num++)
	{
		ret = Touch_I2C_Write_Byte(client, PAGE_SELECT_REG, page_num);
		if( ret == TOUCH_FAIL ) {
			return TOUCH_FAIL;
		}

		for (address = DESCRIPTION_TABLE_START; address > 10;
				address -= sizeof(struct function_descriptor)) {
			ret = Touch_I2C_Read(client, address, (unsigned char *)&buffer, sizeof(buffer));
			if( ret == TOUCH_FAIL ) {
				return TOUCH_FAIL;
			}

			if (buffer.id == 0)
				break;

			switch (buffer.id) {
			case RMI_DEVICE_CONTROL:
				ts->common_fc.dsc = buffer;
				ts->common_fc.function_page = page_num;
				break;
			case TOUCHPAD_SENSORS:
				ts->finger_fc.dsc = buffer;
				ts->finger_fc.function_page = page_num;
				break;
			case LPWG_CONTROL:
				ts->lpwg_fc.dsc = buffer;
				ts->lpwg_fc.function_page = page_num;
				break;
			case SENSOR_CONTROL:
				ts->sensor_fc.dsc = buffer;
				ts->sensor_fc.function_page = page_num;
				break;
			case ANALOG_CONTROL:
				ts->analog_fc.dsc = buffer;
				ts->analog_fc.function_page = page_num;
				break;
			case FLASH_MEMORY_MANAGEMENT:
				ts->flash_fc.dsc = buffer;
				ts->flash_fc.function_page = page_num;
			default:
				break;
			}
		}
	}

	ret = Touch_I2C_Write_Byte(client, PAGE_SELECT_REG, 0x00);
	if( ret == TOUCH_FAIL ) {
		return TOUCH_FAIL;
	}

	if( ts->common_fc.dsc.id == 0 || ts->finger_fc.dsc.id == 0 || ts->analog_fc.dsc.id == 0 || ts->flash_fc.dsc.id == 0 ) {
		TOUCH_ERR("failed to read page description\n");
		return TOUCH_FAIL;
	}

	TOUCH_DBG("common[%dP:0x%02x] finger[%dP:0x%02x] lpwg[%dP:0x%02x] sensor[%dP:0x%02x]"
		"analog[%dP:0x%02x] flash[%dP:0x%02x]\n",
		ts->common_fc.function_page, ts->common_fc.dsc.id,
		ts->finger_fc.function_page, ts->finger_fc.dsc.id,
		ts->lpwg_fc.function_page, ts->lpwg_fc.dsc.id,
		ts->sensor_fc.function_page, ts->sensor_fc.dsc.id,
		ts->analog_fc.function_page, ts->analog_fc.dsc.id,
		ts->flash_fc.function_page, ts->flash_fc.dsc.id);

	return TOUCH_SUCCESS;

}

static int check_firmware_status(struct i2c_client *client)
{
	u8 device_status = 0;
	u8 flash_status = 0;

	Touch_I2C_Read_Byte(client, FLASH_STATUS_REG, &flash_status);
	Touch_I2C_Read_Byte(client, DEVICE_STATUS_REG, &device_status);

	if ((device_status & (DEVICE_STATUS_FLASH_PROG|DEVICE_CRC_ERROR_MASK)) || (flash_status & 0xFF)) {
		TOUCH_ERR("FLASH_STATUS[0x%x] DEVICE_STATUS[0x%x]\n", (u32)flash_status, (u32)device_status);
		return TOUCH_FAIL;
	}

	return TOUCH_SUCCESS;
}

static int tci_control(struct i2c_client *client, int type, u8 value)
{
	int ret = 0;
	u8 buffer[3] = {0};

	switch (type)
	{
		case REPORT_MODE_CTRL:
			ret = Touch_I2C_Read(client, INTERRUPT_ENABLE_REG, buffer, 1);
			if( ret < 0 ) {
				return TOUCH_FAIL;
			}
			ret = Touch_I2C_Write_Byte(client, INTERRUPT_ENABLE_REG,
					value ? buffer[0] & ~INTERRUPT_MASK_ABS0 : buffer[0] | INTERRUPT_MASK_ABS0);
			if( ret < 0 ) {
				return TOUCH_FAIL;
			}
			ret = Touch_I2C_Read(client, FINGER_REPORT_REG, buffer, 3);
			if( ret < 0 ) {
				return TOUCH_FAIL;
			}
			buffer[2] = (buffer[2] & 0xfc) | (value ? 0x2 : 0x0);
			ret = Touch_I2C_Write(client, FINGER_REPORT_REG, buffer, 3);
			if( ret < 0 ) {
				return TOUCH_FAIL;
			}
			TOUCH_DBG("report mode: %d\n", value);
			break;
		case TCI_ENABLE_CTRL:
			ret = synaptics_ts_page_data_read(client, LPWG_PAGE, LPWG_TAPCOUNT_REG, 1, buffer);
			if( ret < 0 ) {
				return TOUCH_FAIL;
			}
			buffer[0] = (buffer[0] & 0xfe) | (value & 0x1);
			ret = synaptics_ts_page_data_write(client, LPWG_PAGE, LPWG_TAPCOUNT_REG, 1, buffer);
			if( ret < 0 ) {
				return TOUCH_FAIL;
			}
			break;
		case TCI_ENABLE_CTRL2:
			ret = synaptics_ts_page_data_read(client, LPWG_PAGE, LPWG_TAPCOUNT_REG2, 1, buffer);
			if( ret < 0 ) {
				return TOUCH_FAIL;
			}
			buffer[0] = (buffer[0] & 0xfe) | (value & 0x1);
			ret = synaptics_ts_page_data_write(client, LPWG_PAGE, LPWG_TAPCOUNT_REG2, 1, buffer);
			if( ret < 0 ) {
				return TOUCH_FAIL;
			}
			break;
		case TAP_COUNT_CTRL:
			ret = synaptics_ts_page_data_read(client, LPWG_PAGE, LPWG_TAPCOUNT_REG, 1, buffer);
			if( ret < 0 ) {
				return TOUCH_FAIL;
			}
			buffer[0] = ((value << 3) & 0xf8) | (buffer[0] & 0x7);
			ret = synaptics_ts_page_data_write(client, LPWG_PAGE, LPWG_TAPCOUNT_REG, 1, buffer);
			if( ret < 0 ) {
				return TOUCH_FAIL;
			}
			break;
		case TAP_COUNT_CTRL2:
			ret = synaptics_ts_page_data_read(client, LPWG_PAGE, LPWG_TAPCOUNT_REG2, 1, buffer);
			if( ret < 0 ) {
				return TOUCH_FAIL;
			}
			buffer[0] = ((value << 3) & 0xf8) | (buffer[0] & 0x7);
			ret = synaptics_ts_page_data_write(client, LPWG_PAGE, LPWG_TAPCOUNT_REG2, 1, buffer);
			if( ret < 0 ) {
				return TOUCH_FAIL;
			}
			break;
		case MIN_INTERTAP_CTRL:
			ret = synaptics_ts_page_data_write_byte(client, LPWG_PAGE, LPWG_MIN_INTERTAP_REG, value);
			if( ret < 0 ) {
				return TOUCH_FAIL;
			}
			break;
		case MIN_INTERTAP_CTRL2:
			ret = synaptics_ts_page_data_write_byte(client, LPWG_PAGE, LPWG_MIN_INTERTAP_REG2, value);
			if( ret < 0 ) {
				return TOUCH_FAIL;
			}
			break;
		case MAX_INTERTAP_CTRL:
			ret = synaptics_ts_page_data_write_byte(client, LPWG_PAGE, LPWG_MAX_INTERTAP_REG, value);
			if( ret < 0 ) {
				return TOUCH_FAIL;
			}
			break;
		case MAX_INTERTAP_CTRL2:
			ret = synaptics_ts_page_data_write_byte(client, LPWG_PAGE, LPWG_MAX_INTERTAP_REG2, value);
			if( ret < 0 ) {
				return TOUCH_FAIL;
			}
			break;
		case TOUCH_SLOP_CTRL:
			ret = synaptics_ts_page_data_write_byte(client, LPWG_PAGE, LPWG_TOUCH_SLOP_REG, value);
			if( ret < 0 ) {
				return TOUCH_FAIL;
			}
			break;
		case TOUCH_SLOP_CTRL2:
			ret = synaptics_ts_page_data_write_byte(client, LPWG_PAGE, LPWG_TOUCH_SLOP_REG2, value);
			if( ret < 0 ) {
				return TOUCH_FAIL;
			}
			break;
		case TAP_DISTANCE_CTRL:
			ret = synaptics_ts_page_data_write_byte(client, LPWG_PAGE, LPWG_TAP_DISTANCE_REG, value);
			if( ret < 0 ) {
				return TOUCH_FAIL;
			}
			break;
		case TAP_DISTANCE_CTRL2:
			ret = synaptics_ts_page_data_write_byte(client, LPWG_PAGE, LPWG_TAP_DISTANCE_REG2, value);
			if( ret < 0 ) {
				return TOUCH_FAIL;
			}
			break;
		case INTERRUPT_DELAY_CTRL:
			ret = synaptics_ts_page_data_write_byte(client, LPWG_PAGE, LPWG_INTERRUPT_DELAY_REG,
					value ? (buffer[0] = (KNOCKON_DELAY << 1) | 0x1) : (buffer[0] = 0));
			if( ret < 0 ) {
				return TOUCH_FAIL;
			}
			break;
		case INTERRUPT_DELAY_CTRL2:
			ret = synaptics_ts_page_data_write_byte(client, LPWG_PAGE, LPWG_INTERRUPT_DELAY_REG2,
					value ? (buffer[0] = (KNOCKON_DELAY << 1) | 0x1) : (buffer[0] = 0));
			if( ret < 0 ) {
				return TOUCH_FAIL;
			}
			break;

		default:
			break;
	}

	return TOUCH_SUCCESS;
	
}

static int sleep_control(struct i2c_client *client, int mode, int recal)
{
	int ret = 0;
	u8 curr = 0;
	u8 next = 0;

	ret = Touch_I2C_Read(client, DEVICE_CONTROL_REG, &curr, 1);
	if( ret < 0 ) {
		return TOUCH_FAIL;
	}

	next = (curr & 0xFC);

	if ( mode == 1 ) {
		next = next | DEVICE_CONTROL_NORMAL_OP;
	} else {
		next = next | DEVICE_CONTROL_SLEEP;
	}

	ret = Touch_I2C_Write_Byte(client, DEVICE_CONTROL_REG, next);
	if( ret < 0 ) {
		return TOUCH_FAIL;
	}

	TOUCH_DBG("%s : curr = [%x] next[%x]\n", __func__, curr, next);

	return TOUCH_SUCCESS;
	
}

static int lpwg_control(struct i2c_client *client, TouchState newState)
{
	int ret = 0;
	u8 readValue = 0;

	TOUCH_FUNC();

	/*                                  */
	ret = Touch_I2C_Read_Byte(client, INTERRUPT_ENABLE_REG, &readValue);
	if( ret < 0 ) {
		return TOUCH_FAIL;
	}

	/*                   */
	ret = Touch_I2C_Write_Byte(client, INTERRUPT_ENABLE_REG, 0x00);
	if( ret < 0 ) {
		return TOUCH_FAIL;
	}
	TOUCH_DBG("%s : newState = [%d] ts->currState[%d]\n", __func__, newState, ts->currState);

	switch(newState)
	{
		case STATE_NORMAL:
			if( ts->currState == STATE_OFF ) {
				sleep_control(client, 1, 0);
				msleep(5);
			}
			tci_control(client, TCI_ENABLE_CTRL, 0);		//              
			tci_control(client, TCI_ENABLE_CTRL2, 0);		//              
			tci_control(client, REPORT_MODE_CTRL, 0);		//       
			msleep(25);
			break;
		case STATE_KNOCK_ON_ONLY:					//           
			if( ts->currState == STATE_NORMAL ) {
				msleep(70);
			} else if ( ts->currState == STATE_OFF ) {
				sleep_control(client, 1, 0);
				msleep(5);
				tci_control(client, REPORT_MODE_CTRL, 0);
			}
			tci_control(client, TAP_COUNT_CTRL, 2); 		//              
			tci_control(client, MIN_INTERTAP_CTRL, 0);		//                    
			tci_control(client, MAX_INTERTAP_CTRL, 70);		//                      
			tci_control(client, TOUCH_SLOP_CTRL, 100);		//                  
			tci_control(client, TAP_DISTANCE_CTRL, 10);		//                    
			tci_control(client, INTERRUPT_DELAY_CTRL, 0);	//                      
			tci_control(client, TCI_ENABLE_CTRL, 1);
			tci_control(client, TCI_ENABLE_CTRL2, 0);		//              
			tci_control(client, REPORT_MODE_CTRL, 1);		//                    
			break;
		case STATE_KNOCK_ON_CODE:					//                
			if( ts->currState == STATE_NORMAL ) {
				msleep(70);
			} else if ( ts->currState == STATE_OFF ) {
				sleep_control(client, 1, 0);
				msleep(5);
				tci_control(client, REPORT_MODE_CTRL, 0);
			}
			tci_control(client, TAP_COUNT_CTRL, 2); 		//              
			tci_control(client, MIN_INTERTAP_CTRL, 0);		//                    
			tci_control(client, MAX_INTERTAP_CTRL, 70);		//                      
			tci_control(client, TOUCH_SLOP_CTRL, 100);		//                  
			tci_control(client, TAP_DISTANCE_CTRL, 7);		//                   
			tci_control(client, INTERRUPT_DELAY_CTRL, (u8)ts->lpwgSetting.isFirstTwoTapSame);	//                      

			tci_control(client, TAP_COUNT_CTRL2, (u8)ts->lpwgSetting.tapCount); //                           
			tci_control(client, MIN_INTERTAP_CTRL2, 0);		//                    
			tci_control(client, MAX_INTERTAP_CTRL2, 70);	//                      
			tci_control(client, TOUCH_SLOP_CTRL2, 100);		//                  
			tci_control(client, TAP_DISTANCE_CTRL2, 255);	//                   
			tci_control(client, INTERRUPT_DELAY_CTRL2, 0);	//                      

			tci_control(client, TCI_ENABLE_CTRL, 1);
			tci_control(client, TCI_ENABLE_CTRL2, 1);
			tci_control(client, REPORT_MODE_CTRL, 1);		//                    
			break;
		case STATE_OFF:
			if( ts->currState == STATE_NORMAL ) {
				msleep(70);
			}
			tci_control(client, TCI_ENABLE_CTRL, 0);		//              
			tci_control(client, TCI_ENABLE_CTRL2, 0);		//              
			tci_control(client, REPORT_MODE_CTRL, 1);
			msleep(5);
			sleep_control(client, 0, 0);
			break;
		default:
			TOUCH_ERR("invalid touch state ( %d )\n", newState);
			break;
	}

	/*                                   */
	ret = Touch_I2C_Write_Byte(client, INTERRUPT_ENABLE_REG, readValue);
	if( ret < 0 ) {
		return TOUCH_FAIL;
	}

	return TOUCH_SUCCESS;	

}
static int get_object_count(struct i2c_client *client)
{
	u8 object_num = 0;
	u8 buf[2] = {0};
	u16 object_attention_data = 0;
	u16 i = 0;
	int ret = 0;

	ret = Touch_I2C_Read(client, OBJECT_ATTENTION_REG, buf, 2);
	if( ret == TOUCH_FAIL ) {
		TOUCH_WARN("failed to read finger number\n");
	}

	object_attention_data = (((u16)((buf[1] << 8)  & 0xFF00)  | (u16)((buf[0]) & 0xFF)));

	for (i = 0; i < MAX_NUM_OF_FINGERS; i++) {
		if (object_attention_data & (0x1 << i)) {
			object_num = i + 1;
		}
	}

	return object_num;
}

/*                                                                           
                           
                                                                           */
int synaptics_ts_rmidev_function(struct synaptics_ts_exp_fn *rmidev_fn, bool insert)
{
	rmidev_fhandler.inserted = insert;

	if (insert)
		rmidev_fhandler.exp_fn = rmidev_fn;
	else
		rmidev_fhandler.exp_fn = NULL;

	return TOUCH_SUCCESS;
}

static ssize_t store_incoming_call(struct i2c_client *client, const char *buf, size_t count)
{
	TouchDriverData *pDriverData = i2c_get_clientdata(client);
	static char incoming_call_str[3][8] = {"IDLE", "RINGING", "OFFHOOK"};
	int reply = 0;
	u8 buffer[2] = {0,};
	sscanf(buf, "%d", &reply);
	incoming_call = reply;

	mutex_lock(pMutexTouch);
	if(
		(( reply == INCOMING_CALL_RINGING ) || ( reply == INCOMING_CALL_OFFHOOK ))
		&& (pDriverData->lpwgSetting.lcdState == 0)
		)//                                                           
	{
		//                                                                 
	}
	else if(reply == INCOMING_CALL_IDLE )//                                                                  
	{
		buffer[0] = 0x19;
		buffer[1] = 0x1e;
		synaptics_ts_page_data_write(client, 0x00, 0x17, 2, buffer);
		buffer[0] = 0x26;
		buffer[1] = 0x14;
		synaptics_ts_page_data_write(client, 0x00, 0x1A, 2, buffer);
	}
	mutex_unlock(pMutexTouch);
	if (reply <= INCOMING_CALL_OFFHOOK)
		TOUCH_LOG("%s : %s(%d) \n", __func__, incoming_call_str[reply], reply);
	else
		TOUCH_LOG("%s : %d \n", __func__, reply);
	return count;
}

static ssize_t store_tci(struct i2c_client *client,
	const char *buf, size_t count)
{
	u32 type = 0, tci_num = 0, value = 0;

	sscanf(buf, "%d %d %d", &type, &tci_num, &value);

	tci_control(client, type, (u8)value);

	return count;
}

static ssize_t show_tci(struct i2c_client *client, char *buf)
{
	int ret = 0;
	int i = 0;
	u8 buffer[7] = {0};

	ret = Touch_I2C_Read(client, FINGER_REPORT_REG, buffer, 3);
	ret += sprintf(buf+ret, "report_mode [%s]\n",
		(buffer[2] & 0x3) == 0x2 ? "WAKEUP_ONLY" : "NORMAL");
	ret = Touch_I2C_Read(client, WAKEUP_GESTURE_ENABLE_REG, buffer, 1);
	ret += sprintf(buf+ret, "wakeup_gesture [%d]\n\n", buffer[0]);

	for (i = 0; i < 2; i++) {
		synaptics_ts_page_data_read(client, LPWG_PAGE,
			LPWG_TAPCOUNT_REG + (i * LPWG_BLOCK_SIZE), 7, buffer);
		ret += sprintf(buf+ret, "TCI - %d\n", i+1);
		ret += sprintf(buf+ret, "TCI [%s]\n",
			(buffer[0] & 0x1) == 1 ? "enabled" : "disabled");
		ret += sprintf(buf+ret, "Tap Count [%d]\n",
			(buffer[0] & 0xf8) >> 3);
		ret += sprintf(buf+ret, "Min InterTap [%d]\n", buffer[1]);
		ret += sprintf(buf+ret, "Max InterTap [%d]\n", buffer[2]);
		ret += sprintf(buf+ret, "Touch Slop [%d]\n", buffer[3]);
		ret += sprintf(buf+ret, "Tap Distance [%d]\n", buffer[4]);
		ret += sprintf(buf+ret, "Interrupt Delay [%d]\n\n", buffer[6]);
	}

	return ret;
	
}

static ssize_t store_reg_ctrl(struct i2c_client *client,
	const char *buf, size_t count)
{
	u8 buffer[50] = {0};
	char command[6] = {0};
	int page = 0;
	int reg = 0;
	int offset = 0;
	int value = 0;

	sscanf(buf, "%s %d %d %d %d ", command, &page, &reg, &offset, &value);

	if (!strcmp(command, "write")) {
		synaptics_ts_page_data_read(client, page,
			reg, offset+1, buffer);
		buffer[offset] = (u8)value;
		synaptics_ts_page_data_write(client, page,
			reg, offset+1, buffer);
	} else if (!strcmp(command, "read")) {
		synaptics_ts_page_data_read(client, page,
			reg, offset+1, buffer);
		TOUCH_DBG("page[%d] reg[%d] offset[%d] = 0x%x\n",
			page, reg, offset, buffer[offset]);
	} else {
		TOUCH_DBG("Usage\n");
		TOUCH_DBG("Write page reg offset value\n");
		TOUCH_DBG("Read page reg offset\n");
	}
	return count;
	
}

static ssize_t show_object_report(struct i2c_client *client, char *buf)
{
	u8 object_report_enable_reg = 0;
	u8 temp[8];

	int ret = 0;
	int i;

	ret = Touch_I2C_Read(client, OBJECT_REPORT_ENABLE_REG, &object_report_enable_reg,sizeof(object_report_enable_reg));
	if( ret < 0 ) {
		return ret;
	}

	for (i = 0; i < 8; i++)
		temp[i] = (object_report_enable_reg >> i) & 0x01;

	ret = sprintf(buf,
		"\n======= read object_report_enable register =======\n");
	ret += sprintf(buf+ret,
		" Addr Bit7 Bit6 Bit5 Bit4 Bit3 Bit2 Bit1 Bit0 HEX\n");
	ret += sprintf(buf+ret,
		"--------------------------------------------------\n");
	ret += sprintf(buf+ret,
		" 0x%02X %4d %4d %4d %4d %4d %4d %4d %4d 0x%02X\n",
		OBJECT_REPORT_ENABLE_REG, temp[7], temp[6],
		temp[5], temp[4], temp[3], temp[2], temp[1], temp[0],
		object_report_enable_reg);
	ret += sprintf(buf+ret,
		"--------------------------------------------------\n");
	ret += sprintf(buf+ret,
		" Bit0  : [F]inger -> %7s\n", temp[0] ? "Enable" : "Disable");
	ret += sprintf(buf+ret,
		" Bit1  : [S]tylus -> %7s\n", temp[1] ? "Enable" : "Disable");
	ret += sprintf(buf+ret,
		" Bit2  : [P]alm -> %7s\n", temp[2] ? "Enable" : "Disable");
	ret += sprintf(buf+ret,
		" Bit3  : [U]nclassified Object -> %7s\n",
		temp[3] ? "Enable" : "Disable");
	ret += sprintf(buf+ret,
		" Bit4  : [H]overing Finger -> %7s\n",
		temp[4] ? "Enable" : "Disable");
	ret += sprintf(buf+ret,
		" Bit5  : [G]loved Finger -> %7s\n",
		temp[5] ? "Enable" : "Disable");
	ret += sprintf(buf+ret,
		" Bit6  : [N]arrow Object Swipe -> %7s\n",
		temp[6] ? "Enable" : "Disable");
	ret += sprintf(buf+ret,
		" Bit7  : Hand[E]dge  -> %7s\n",
		temp[7] ? "Enable" : "Disable");
	ret += sprintf(buf+ret,
		"==================================================\n\n");
	
	return ret;
	
}

static ssize_t store_object_report(struct i2c_client *client,
	const char *buf, size_t count)
{
	int ret = 0;
	char select[16];
	u8 value = 2;
	int select_cnt;
	int i;
	u8 bit_select = 0;
	u8 object_report_enable_reg_old = 0;
	u8 object_report_enable_reg_new = 0;

	sscanf(buf, "%s %hhu", select, &value);

	if ((strlen(select) > 8) || (value > 1)) {
		TOUCH_DBG("<writing object_report guide>\n");
		TOUCH_DBG("echo [select] [value] > object_report\n");
		TOUCH_DBG("select: [F]inger, [S]tylus, [P]alm,"
			" [U]nclassified Object, [H]overing Finger,"
			" [G]loved Finger, [N]arrow Object Swipe,"
			" Hand[E]dge\n");
		TOUCH_DBG("select length: 1~8, value: 0~1\n");
		TOUCH_DBG("ex) echo F 1 > object_report        "
			" (enable [F]inger)\n");
		TOUCH_DBG("ex) echo s 1 > object_report        "
			" (enable [S]tylus)\n");
		TOUCH_DBG("ex) echo P 0 > object_report        "
			" (disable [P]alm)\n");
		TOUCH_DBG("ex) echo u 0 > object_report        "
			" (disable [U]nclassified Object)\n");
		TOUCH_DBG("ex) echo HgNe 1 > object_report     "
			" (enable [H]overing Finger, [G]loved Finger,"
			" [N]arrow Object Swipe, Hand[E]dge)\n");
		TOUCH_DBG("ex) echo eNGh 1 > object_report     "
			" (enable Hand[E]dge, [N]arrow Object Swipe,"
			" [G]loved Finger, [H]overing Finger)\n");
		TOUCH_DBG("ex) echo uPsF 0 > object_report     "
			" (disable [U]nclassified Object, [P]alm,"
			" [S]tylus, [F]inger)\n");
		TOUCH_DBG("ex) echo HguP 0 > object_report     "
			" (disable [H]overing Finger, [G]loved Finger,"
			" [U]nclassified Object, [P]alm)\n");
		TOUCH_DBG("ex) echo HFnuPSfe 1 > object_report "
			" (enable all object)\n");
		TOUCH_DBG("ex) echo enghupsf 0 > object_report "
			" (disbale all object)\n");
	} else {
		select_cnt = strlen(select);

		for (i = 0; i < select_cnt; i++) {
			switch ((char)(*(select + i))) {
			case 'F': case 'f': /*          */
				bit_select |= (0x01 << 0);
				break;
			case 'S': case 's': /*          */
				bit_select |= (0x01 << 1);
				break;
			case 'P': case 'p': /*        */
				bit_select |= (0x01 << 2);
				break;
			case 'U': case 'u': /*                       */
				bit_select |= (0x01 << 3);
				break;
			case 'H': case 'h': /*                   */
				bit_select |= (0x01 << 4);
				break;
			case 'G': case 'g': /*                 */
				bit_select |= (0x01 << 5);
				break;
			case 'N': case 'n': /*                       */
				bit_select |= (0x01 << 6);
				break;
			case 'E': case 'e': /*             */
				bit_select |= (0x01 << 7);
				break;
			default:
				break;
			}
		}

		ret = Touch_I2C_Read(client, OBJECT_REPORT_ENABLE_REG, &object_report_enable_reg_old, sizeof(object_report_enable_reg_old));
		if( ret < 0 ) {
			return count;
		}

		object_report_enable_reg_new = object_report_enable_reg_old;

		if (value > 0)
			object_report_enable_reg_new |= bit_select;
		else
			object_report_enable_reg_new &= ~(bit_select);

		ret = Touch_I2C_Write_Byte(client, OBJECT_REPORT_ENABLE_REG, object_report_enable_reg_new);
		if( ret < 0 ) {
			return count;
		}
		
	}

	return count;
	
}

static ssize_t show_use_rmi_dev(struct i2c_client *client, char *buf)
{
	int ret = 0;

	ret = sprintf(buf, "%u\n", enable_rmi_dev);

	return ret;
}

static ssize_t store_use_rmi_dev(struct i2c_client *client, const char *buf, size_t count)
{
	int ret = 0;
	int value = 0;

	sscanf(buf, "%d", &value);

	if (value < 0 || value > 1) {
		TOUCH_DBG("Invalid enable_rmi_dev value:%d\n", value);
		return count;
	}

	enable_rmi_dev = value;
	TOUCH_DBG("enable_rmi_dev:%u\n", enable_rmi_dev);

	if (enable_rmi_dev && rmidev_fhandler.inserted) {
		if (!rmidev_fhandler.initialized) {
			ret = rmidev_fhandler.exp_fn->init(ts);
			if( ret < 0 ) {
				TOUCH_ERR("fail to enable_rmi_dev\n");
				return count;
			}
			
			rmidev_fhandler.initialized = true;
		}
	}
	else {
		rmidev_fhandler.exp_fn->remove(ts);
		rmidev_fhandler.initialized = false;
	}

	return count;

}

static LGE_TOUCH_ATTR(tci, S_IRUGO | S_IWUSR, show_tci, store_tci);
static LGE_TOUCH_ATTR(reg_ctrl, S_IRUGO | S_IWUSR, NULL, store_reg_ctrl);
static LGE_TOUCH_ATTR(object_report, S_IRUGO | S_IWUSR, show_object_report, store_object_report);
static LGE_TOUCH_ATTR(enable_rmi_dev, S_IRUGO | S_IWUSR, show_use_rmi_dev, store_use_rmi_dev);
static LGE_TOUCH_ATTR(incoming_call, S_IRUGO | S_IWUSR, NULL, store_incoming_call);

static struct attribute *S3528_attribute_list[] = {
	&lge_touch_attr_tci.attr,
	&lge_touch_attr_reg_ctrl.attr,
	&lge_touch_attr_object_report.attr,
	&lge_touch_attr_enable_rmi_dev.attr,
	&lge_touch_attr_incoming_call.attr,
	NULL,
};
static int synaptics_set_qcover_area(struct i2c_client *client,u32 X1, u32 X2, u32 Y1, u32 Y2)
{
	TOUCH_FUNC ();
	int ret = 0;
	u8 buffer[8] = {0};
	int i=0;
	u8 doubleTap_area_reg_addr = LPWG_CONTROL_REG;
	u32 value[4] = {X1,X2,Y1,Y2};
	buffer[0] = value[0];
	buffer[1] = value[0]>>8;
	buffer[2] = value[2];
	buffer[3] = value[2]>>8;
	buffer[4] = value[1];
	buffer[5] = value[1]>>8;
	buffer[6] = value[3];
	buffer[7] = value[3]>>8;
	TOUCH_LOG("X1 =%d X2 = %d Y1 = %d Y2 = %d ts->common_fc.function_page =%x\n",X1,X2,Y1,Y2,ts->common_fc.function_page);
	ret = synaptics_ts_page_data_write(client, ts->common_fc.function_page, doubleTap_area_reg_addr, 8, buffer);
	TOUCH_LOG("ret = %d\n",ret);
	return ret;
}

static int S3528_Initialize(struct i2c_client *client)
{
	int ret = 0;
	
	TOUCH_FUNC();

	/*                                              */
	ts = devm_kzalloc(&client->dev, sizeof(struct synaptics_ts_data), GFP_KERNEL);
	if( ts == NULL ) {
		TOUCH_ERR("failed to allocate memory for device driver data\n");
		return TOUCH_FAIL;
	}

	ts->client = client;

	/*                               */
	ret = read_page_description_table(client);
	if( ret == TOUCH_FAIL ) {
		if( check_firmware_status(client) == TOUCH_FAIL ) {
			int cnt = 0;

			do {
				ret = S3528_UpdateFirmware(client, NULL);
				cnt++;
			} while(ret == TOUCH_FAIL && cnt < 3);

			if( ret == TOUCH_FAIL ) {
				devm_kfree(&client->dev, ts);
				return TOUCH_FAIL;
			}
		}
	}

	return TOUCH_SUCCESS;
	
}

static void S3528_Reset(struct i2c_client *client)
{
	/*                                   */
	if( client != NULL ) {
		int ret = 0;
		synaptics_ts_page_data_write_byte(client, ANALOG_PAGE, DYNAMIC_SENSING_CONTROL_REG, SENSING_CONTROL_120HZ);
		if( ret == TOUCH_FAIL ) {
			TOUCH_WARN("failed to change sensing control to 120Hz\n");
		}
	}
	
	TouchResetCtrl(0);
	msleep(10);
	TouchResetCtrl(1);
	msleep(150);
	
	TOUCH_LOG("Device was reset\n");
}


static int S3528_Connect(void)
{
	TOUCH_FUNC();

	/*                                       */

	return TOUCH_SUCCESS;
}

static int S3528_InitRegister(struct i2c_client *client)
{
	int ret = 0;
	u8 reg = 0;
	u8 data[2] = {0};

	reg = DEVICE_CONTROL_REG;
	data[0] = DEVICE_CONTROL_NORMAL_OP | DEVICE_CONTROL_CONFIGURED;
	ret = Touch_I2C_Write(client, reg, data, 1);
	if( ret == TOUCH_FAIL ) {
		return TOUCH_FAIL;
	}
	
	reg = INTERRUPT_ENABLE_REG;
	data[0] = 0;
	ret = Touch_I2C_Read(client, reg, data, 1);
	if( ret == TOUCH_FAIL ) {
		return TOUCH_FAIL;
	}

	reg = INTERRUPT_ENABLE_REG;
	data[0] |= INTERRUPT_MASK_ABS0;
	ret = Touch_I2C_Write(client, reg, data, 1);
	if( ret == TOUCH_FAIL ) {
		return TOUCH_FAIL;
	}

	/*                        
             
                                             
                          
                    
  */
	
	reg = INTERRUPT_STATUS_REG;
	data[0] = 0;
	ret = Touch_I2C_Read(client, reg, data, 1);
	if( ret == TOUCH_FAIL ) {
		return TOUCH_FAIL;
	}

	ts->currState = STATE_NORMAL;
	//                                                                          
	if ( incoming_call == INCOMING_CALL_RINGING )
	{
		u8 buffer[2] = {0,};
		buffer[0] = 0x50;
		buffer[1] = 0x64;
		synaptics_ts_page_data_write(client, 0x00, 0x17, 2, buffer);
		buffer[0] = 0x66;
		buffer[1] = 0x66;
		synaptics_ts_page_data_write(client, 0x00, 0x1A, 2, buffer);
	}

	return TOUCH_SUCCESS;
	
}

static void S3528_ClearInterrupt(struct i2c_client *client)
{
	int ret = 0;

	u8 regDevStatus = 0;
	u8 regIntStatus = 0;
	TOUCH_FUNC();

	ret = Touch_I2C_Read_Byte(client, DEVICE_STATUS_REG, &regDevStatus);
	if( ret == TOUCH_FAIL ) {
		TOUCH_ERR("failed to read device status reg\n");
	}

	ret = Touch_I2C_Read_Byte(client, INTERRUPT_STATUS_REG, &regIntStatus);
	if( ret == TOUCH_FAIL ) {
		TOUCH_ERR("failed to read interrupt status reg\n");
	}

	if( ret == TOUCH_FAIL ) {
		TOUCH_ERR("failed to clear interrupt\n");
	}

	return;

}

static int S3528_InterruptHandler(struct i2c_client *client,TouchReadData *pData)
{
	TouchFingerData *pFingerData = NULL;
	TouchDriverData *pDriverData = i2c_get_clientdata(client);
	int result = 0;
	u8 i = 0;
	u8 lpwgStatus = 0;
	u8 lpwgTapCount = 0;
	u8 readFingerCnt = 0;
	u8 regDevStatus = 0;
	u8 regIntStatus = 0;
	u8 regFingerData[MAX_NUM_OF_FINGERS][NUM_OF_EACH_FINGER_DATA_REG];
	u8 buffer[12][4] = { {0} };

	pData->type = DATA_UNKNOWN;
	pData->count = 0;

	result = Touch_I2C_Read_Byte(client, DEVICE_STATUS_REG, &regDevStatus);
	if( result == TOUCH_FAIL ) {
		TOUCH_WARN("failed to read device status\n");
	}

	result = Touch_I2C_Read_Byte(client, INTERRUPT_STATUS_REG, &regIntStatus);
	if( result == TOUCH_FAIL ) {
		TOUCH_WARN("failed to read interrupt status\n");
	}

	if (regIntStatus == INTERRUPT_MASK_NONE) {
		return TOUCH_SUCCESS;
	} else if (regIntStatus & INTERRUPT_MASK_ABS0 ) {
		readFingerCnt = get_object_count(client);

		if (readFingerCnt > 0) {
			Touch_I2C_Read(client, FINGER_DATA_REG_START,			
				(u8 *)regFingerData, (NUM_OF_EACH_FINGER_DATA_REG * readFingerCnt));

			for (i = 0; i < readFingerCnt; i++) {
				if (regFingerData[i][0] & 0x03) {
					pFingerData = &pData->fingerData[pData->count];
					pFingerData->id = i;
					pFingerData->type = regFingerData[i][REG_OBJECT];
					pFingerData->x = GET_X_POSITION( regFingerData[i][REG_X_MSB], regFingerData[i][REG_X_LSB] );
					pFingerData->y = GET_Y_POSITION( regFingerData[i][REG_Y_MSB], regFingerData[i][REG_Y_LSB] );
					pFingerData->width_major = GET_WIDTH_MAJOR( regFingerData[i][REG_WX], regFingerData[i][REG_WY] );
					pFingerData->width_minor = GET_WIDTH_MINOR( regFingerData[i][REG_WX], regFingerData[i][REG_WY] );
					pFingerData->orientation = GET_ORIENTATION( regFingerData[i][REG_WY], regFingerData[i][REG_WX] );
					pFingerData->pressure = GET_PRESSURE( regFingerData[i][REG_Z] );
					//                                                             
					if(qcover == 1)
					{	//                                                                                         
						if(pFingerData->x < QCOVERX1|| pFingerData->x > QCOVERX2
						|| pFingerData->y < QCOVERY1 || pFingerData->y > QCOVERY2 )
						{
							continue;
						}
					}

					pData->count++;
				}
			}
		}

		pData->type = DATA_FINGER;
		/*                                                                            */
		if (ts->currState == STATE_KNOCK_ON_CODE)
		{
			TOUCH_LOG("STATE_KNOCK_ON_CODE Check in Event1\n");
			pData->count = 2;
			pData->knockData[0].x = 1;
			pData->knockData[0].y = 1;
			pData->knockData[1].x = -1;
			pData->knockData[1].y = -1;
			pData->type = DATA_KNOCK_CODE;
		}

	} else if (regIntStatus & INTERRUPT_MASK_CUSTOM) {
		synaptics_ts_page_data_read(client, LPWG_PAGE, LPWG_STATUS_REG, 1, &lpwgStatus);
		TOUCH_LOG( "LPWG status: %d\n", lpwgStatus );

		if (lpwgStatus & 0x1)	/*                    */
		{
			TOUCH_LOG( "Knock-on Detected\n" );
			pData->type = DATA_KNOCK_ON;
		}
		else if (lpwgStatus & 0x2)	/*                   */
		{
			TOUCH_LOG( "Knock-code Detected\n" );
			pData->type = DATA_KNOCK_CODE;

			synaptics_ts_page_data_read(client, LPWG_PAGE, LPWG_DATA_REG, 4*ts->lpwgSetting.tapCount, &buffer[0][0]);

			tci_control(client, REPORT_MODE_CTRL, 0);		//                                   
			for (i = 0; i < ts->lpwgSetting.tapCount; i++) {
				pData->knockData[i].x
					= GET_X_POSITION(buffer[i][1], buffer[i][0]);
				pData->knockData[i].y
					= GET_Y_POSITION(buffer[i][3], buffer[i][2]);
				TOUCH_DBG("LPWG data [%d, %d]\n",
					pData->knockData[i].x, pData->knockData[i].y);
			}
			pData->count = ts->lpwgSetting.tapCount;
		}
		else if (lpwgStatus == 0)
		{
			TOUCH_LOG( "OverTap Knock-code Detected\n" );
			pData->type = DATA_KNOCK_CODE;
			synaptics_ts_page_data_read(client, LPWG_PAGE, LPWG_OVER_TAPCOUNT, 1, &lpwgTapCount);
			TOUCH_LOG( "Tap Count: %d\n", lpwgTapCount );

			pData->knockData[0].x = 1;
			pData->knockData[0].y = 1;
			pData->knockData[1].x = -1;
			pData->knockData[1].y = -1;
			pData->count = ts->lpwgSetting.tapCount;
		}
	}
	else
	{
		TOUCH_ERR( "Unexpected Interrupt Status ( 0x%X )\n", regIntStatus );
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
static int S3528_ReadIcFirmwareInfo(struct i2c_client *client, TouchFirmwareInfo *pFwInfo)
{
	int ret = 0;
	u8 readData[FW_VER_INFO_NUM] = {0};
	
	TOUCH_FUNC();

	ret = Touch_I2C_Read(client, FLASH_CONFIG_ID_REG, readData, FW_VER_INFO_NUM);
	if( ret == TOUCH_FAIL ) {
		return TOUCH_FAIL;
	}

	TOUCH_LOG("Maker=%d, Key=%d, Supplier=%d\n", ( readData[0] >> 4 ) & 0xF, readData[0] & 0xF, ( readData[1] >> 4 ) & 0x0F);
	TOUCH_LOG("Panel=%d, Size =%d.%d [Inch]\n", readData[2] & 0xF, readData[1] & 0xF, ( readData[2] >> 4 ) & 0xF);
	TOUCH_LOG("Version=%d ( %s )\n", readData[3] & 0x7F, ( ( readData[3] & 0x80 ) == 0x80 ) ? "Official Release" : "Test Release");
	
	pFwInfo->moduleMakerID = ( readData[0] >> 4 ) & 0x0F;
	pFwInfo->moduleVersion = readData[2] & 0x0F;
	pFwInfo->modelID = 0;
	pFwInfo->isOfficial = ( readData[3] >> 7 ) & 0x01;
	pFwInfo->version = readData[3] & 0x7F;

	return TOUCH_SUCCESS;
	
}

static int S3528_GetBinFirmwareInfo(struct i2c_client *client, char *pFilename, TouchFirmwareInfo *pFwInfo)
{
	int ret = 0;
	const struct firmware *fw = NULL;
	u8 *pBin = NULL;
	char *pFwFilename = NULL;
	u8 *pReadData = NULL;

	TOUCH_FUNC();
	
	if( pFilename == NULL ) {
		pFwFilename = (char *)defaultFirmware;
	} else {
		pFwFilename = pFilename;
	}

	TOUCH_LOG("Firmware filename = %s\n", pFwFilename);
	
	/*                                             */
	ret = request_firmware(&fw, pFwFilename, &client->dev);
	if( ret )
	{
		TOUCH_ERR("failed at request_firmware() ( error = %d )\n", ret);
		return TOUCH_FAIL;
	}

	pBin = (u8 *)(fw->data);
	pReadData = &pBin[0x16d00];

	TOUCH_LOG("Maker=%d, Key=%d, Supplier=%d\n", ( pReadData[0] >> 4 ) & 0xF, pReadData[0] & 0xF, ( pReadData[1] >> 4 ) & 0x0F);
	TOUCH_LOG("Panel=%d, Size =%d.%d [Inch]\n", pReadData[2] & 0xF, pReadData[1] & 0xF, ( pReadData[2] >> 4 ) & 0xF);
	TOUCH_LOG("Version=%d ( %s )\n", pReadData[3] & 0x7F, ( ( pReadData[3] & 0x80 ) == 0x80 ) ? "Official Release" : "Test Release");
	
	pFwInfo->moduleMakerID = ( pReadData[0] >> 4 ) & 0x0F;
	pFwInfo->moduleVersion = pReadData[2] & 0x0F;
	pFwInfo->modelID = 0;
	pFwInfo->isOfficial = ( pReadData[3] >> 7 ) & 0x01;
	pFwInfo->version = pReadData[3] & 0x7F;

	/*                            */
	release_firmware(fw);

	return TOUCH_SUCCESS;
		
}


static int S3528_UpdateFirmware(struct i2c_client *client, char *pFilename)
{
	int ret = 0;
	char *pFwFilename = NULL;
	
	TOUCH_FUNC();
	
	if( pFilename == NULL ) {
		pFwFilename = (char *)defaultFirmware;
	} else {
		pFwFilename = pFilename;
	}

	TOUCH_LOG("Firmware filename = %s\n", pFwFilename);
	
	ret = synaptics_ts_page_data_write_byte(client, ANALOG_PAGE, DYNAMIC_SENSING_CONTROL_REG, SENSING_CONTROL_120HZ);
	if( ret == TOUCH_FAIL ) {
		TOUCH_ERR("failed to change sensing control to 120Hz\n");
		return TOUCH_FAIL;
	}

	FirmwareUpgrade(ts, pFwFilename);

	/*                               */
	ret = read_page_description_table(client);
	if( ret == TOUCH_FAIL ) {
		return TOUCH_FAIL;
	}

	ret = check_firmware_status(client);
	if( ret == TOUCH_FAIL ) {
		return TOUCH_FAIL;
	}

	return TOUCH_SUCCESS;
	
}

static int S3528_SetLpwgMode(struct i2c_client *client, TouchState newState, LpwgSetting  *pLpwgSetting)
{
	int ret = TOUCH_SUCCESS;
	
	TOUCH_FUNC();

	memcpy(&ts->lpwgSetting, pLpwgSetting, sizeof(LpwgSetting));
	//                                    
	if( (ts->currState == newState) && (ts->currState != STATE_KNOCK_ON_CODE) ) {
		TOUCH_LOG("device state is same as driver requested\n");
		return TOUCH_SUCCESS;
	}

	if( ( newState < STATE_NORMAL ) && ( newState > STATE_KNOCK_ON_CODE ) ) {
		TOUCH_LOG("invalid request state ( state = %d )\n", newState);
		return TOUCH_FAIL;
	}

	if((qcover == 1) && (ts->lpwgSetting.lcdState == 0))
	{
		ret = synaptics_set_qcover_area(client,QCOVERX1,
			QCOVERX2,QCOVERY1,
			QCOVERY2);
	}
	ret = lpwg_control(client, newState);
	if( ret == TOUCH_FAIL ) {
		TOUCH_ERR("failed to set lpwg mode in device\n");
		return TOUCH_FAIL;
	}

	if( ret == TOUCH_SUCCESS ) {
		ts->currState = newState;
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
		default:
			TOUCH_LOG("impossilbe state ( state = %d )\n", newState);
			ret = TOUCH_FAIL;
			break;
	}

	return TOUCH_SUCCESS;

}
static int S3528_DoSelfDiagnosis(struct i2c_client *client, int* pRawStatus, int* pChannelStatus, char* pBuf, int bufSize, int* pDataLen)
{
	int ret = 0;
	int dataLen = 0;

	/*                                                */

	int full_raw_cap;
	int trx_to_trx;
	int high_resistance;

	TOUCH_FUNC();

	ret = Touch_I2C_Write_Byte(client, PAGE_SELECT_REG, ANALOG_PAGE);
	if( ret == TOUCH_FAIL ) {
		return TOUCH_FAIL;
	}

	full_raw_cap = F54TestHandle(ts, F54_FULL_RAW_CAP, 0, pBuf);
	high_resistance = F54TestHandle(ts, F54_HIGH_RESISTANCE, 0, pBuf);
	trx_to_trx = F54TestHandle(ts, F54_TRX_TO_TRX, 0, pBuf);

	*pRawStatus = full_raw_cap;

	if (high_resistance == TOUCH_SUCCESS && trx_to_trx == TOUCH_SUCCESS) {
		*pChannelStatus = TOUCH_SUCCESS;
	} else {
		*pChannelStatus = TOUCH_FAIL;
	}

//                                                                                          
//                                                                                                

	*pDataLen = dataLen;

	return TOUCH_SUCCESS;
	
}

static int S3528_AccessRegister(struct i2c_client *client, int cmd, int reg, int *pValue)
{
	int ret = 0;
	
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

static void S3528_NotifyHandler(struct i2c_client *client, TouchNotify notify, int data)
{
	switch( notify )
	{
		case NOTIFY_CALL:
			{
				u8 buffer[2] = {0,};
				if(data == 1 )//                                                           
				{
					buffer[0] = 0x50;
					buffer[1] = 0x64;
					synaptics_ts_page_data_write(client, 0x00, 0x17, 2, buffer);
					buffer[0] = 0x66;
					buffer[1] = 0x66;
					synaptics_ts_page_data_write(client, 0x00, 0x1A, 2, buffer);
				}
				else if(data != 1 )//                                                                  
				{
					buffer[0] = 0x19;
					buffer[1] = 0x1e;
					synaptics_ts_page_data_write(client, 0x00, 0x17, 2, buffer);
					buffer[0] = 0x26;
					buffer[1] = 0x14;
					synaptics_ts_page_data_write(client, 0x00, 0x1A, 2, buffer);
				}
				TOUCH_LOG("Call was notified ( data = %d )\n", data);
			}
			break;

		case NOTIFY_Q_COVER:
			qcover = data;
			TOUCH_LOG("Quick Cover was notified ( data = %d )\n", data);
			break;

		default:
			TOUCH_ERR("Invalid notification ( notify = %d )\n", notify);
			break;
	}

	return;

}
TouchDeviceSpecificFunction S3528_Func = {

	.Initialize = S3528_Initialize,
	.Reset = S3528_Reset,
	.Connect = S3528_Connect,
	.InitRegister = S3528_InitRegister,
	.ClearInterrupt = S3528_ClearInterrupt,
	.InterruptHandler = S3528_InterruptHandler,
	.ReadIcFirmwareInfo = S3528_ReadIcFirmwareInfo,
	.GetBinFirmwareInfo = S3528_GetBinFirmwareInfo,
	.UpdateFirmware = S3528_UpdateFirmware,
	.SetLpwgMode = S3528_SetLpwgMode,
	.DoSelfDiagnosis = S3528_DoSelfDiagnosis,
	.AccessRegister = S3528_AccessRegister,
	.NotifyHandler = S3528_NotifyHandler,
	.device_attribute_list = S3528_attribute_list,
	
};


/*             */


