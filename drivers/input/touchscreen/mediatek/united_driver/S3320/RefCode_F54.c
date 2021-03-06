//                                                              
// Copyright ?2012 Synaptics Incorporated. All rights reserved.
//
//                                                             
//                                                             
//       
//
//                                                                 
//                                                             
//                                                             
// information in this file are not expressly or implicitly licensed
//                                                                
//                             
//                                                              
//                                     
//
#include <linux/delay.h>	//      
#include <linux/file.h>		//               
#include <linux/syscalls.h> //               
#include <linux/uaccess.h>  //               
#include <linux/i2c.h>
#include <linux/slab.h>

#include "RefCode_F54.h"
#include "TestLimits.h"

#include <linux/input/lgtp_common.h>
#include <linux/input/lgtp_common_driver.h>
#include <linux/input/lgtp_platform_api.h>
#include <linux/input/lgtp_model_config.h>
#include <linux/input/lgtp_device_s3320.h>

//                  
#define TRX_mapping_max 54
#define LOWER_ABS_ADC_RANGE_LIMIT 60
#define UPPER_ABS_ADC_RANGE_LIMIT 190
#define LOWER_ABS_RAW_CAP_LIMIT 1000 //  
#define UPPER_ABS_RAW_CAP_LIMIT 14000 //  
#define REPORT_DATA_OFFEST 3
#define VERSION "1.0"

unsigned int count;
const unsigned short DefaultTarget = 0;
const int DefaultTimeout = 10; //          

int pageNum = 0;
int scanMaxPageCount = 5;
//              
int input;

bool bHaveF01 = false;
bool bHaveF11 = false;
bool bHaveF1A = false;
bool bHaveF12 = false;
bool bHaveF34 = false;
bool bHaveF54 = false;
bool bHaveF55 = false;
bool SignalClarityOn = false;
bool bHaveF54Ctrl07 = false;
bool bHaveF54Ctrl41 = false;
bool bHaveF54Ctrl57 = false;
bool bHavePixelTouchThresholdTuning = false;
bool bHaveInterferenceMetric = false;
bool bHaveCtrl11 = false;
bool bHaveRelaxationControl = false;
bool bHaveSensorAssignment = false;
bool bHaveSenseFrequencyControl = false;
bool bHaveFirmwareNoiseMitigation = false;
bool bHaveIIRFilter = false;
bool bHaveCmnRemoval = false;
bool bHaveCmnMaximum = false;
bool bHaveTouchHysteresis = false;
bool bHaveEdgeCompensation = false;
bool bHavePerFrequencyNoiseControl = false;
bool bHaveSignalClarity = false;
bool bHaveMultiMetricStateMachine = false;
bool bHaveVarianceMetric = false;
bool bHave0DRelaxationControl = false;
bool bHave0DAcquisitionControl = false;
bool bHaveSlewMetric = false;
bool bHaveHBlank = false;
bool bHaveVBlank = false;
bool bHaveLongHBlank = false;
bool bHaveNoiseMitigation2 = false;
bool bHaveSlewOption = false;
bool bHaveEnhancedStretch = false;
bool bHaveStartupFastRelaxation = false;
bool bHaveESDControl = false;
bool bHaveEnergyRatioRelaxation = false;
bool bHaveCtrl86 = false;
bool bHaveCtrl87 = false;
bool bHaveCtrl88 = false;
bool bHaveCtrl89 = false;
bool bHaveCtrl90 = false;
bool bHaveCtrl91 = false;
bool bHaveCtrl92 = false;
bool bHaveCtrl93 = false;
bool bHaveCtrl94 = false;
bool bHaveCtrl95 = false;
bool bHaveCtrl96 = false;
bool bHaveCtrl97 = false;
bool bHaveCtrl98 = false;
bool bHaveCtrl99 = false;
bool bHaveCtrl100 = false;
bool bHaveCtrl101 = false;
bool bHaveCtrl102 = false;
bool bHaveF54Query13 = false;
bool bHaveF54Query15 = false;
bool bHaveF54Query16 = false;
bool bHaveF54Query17 = false;
bool bHaveF54Query18 = false;
bool bHaveF54Query19 = false;
bool bHaveF54Query22 = false;
bool bHaveF54Query23 = false;
bool bHaveF54Query25 = false;
bool ButtonShared = false;

unsigned char F54DataBase;
unsigned char F54QueryBase;
unsigned char F54CommandBase;
unsigned char F54ControlBase;
unsigned char F55QueryBase;
unsigned char F55ControlBase;
unsigned char F01ControlBase;
unsigned char F01CommandBase;
unsigned char RxChannelCount;
unsigned char TxChannelCount;
unsigned char TouchControllerFamily;
unsigned char CurveCompensationMode;
unsigned char NumberOfSensingFrequencies;
unsigned char F54Ctrl07Offset;
unsigned char F54Ctrl41Offset;
unsigned char F54Ctrl57Offset;
unsigned char F54Ctrl88Offset;
unsigned char F54Ctrl89Offset;
unsigned char F54Ctrl98Offset;
unsigned char F1AControlBase;
unsigned char F12ControlBase;
unsigned char F12QueryBase;
unsigned char F12_2DTxCount;
unsigned char F12_2DRxCount;
unsigned char ButtonTx[8];
unsigned char ButtonRx[8];
unsigned char ButtonCount;
unsigned char F12Support;
unsigned char F12ControlRegisterPresence;
unsigned char mask;

//                                                            
short Image1[TRX_max][TRX_max];
int ImagepF[TRX_max][TRX_max];
int AbsSigned32Data[TRX_mapping_max];
unsigned char AbsADCRangeData[TRX_mapping_max];
unsigned char Data[TRX_max * TRX_max *4];
unsigned char TRxPhysical[TRX_mapping_max] ;

int MaxArrayLength;

unsigned char TREX_mapped[7] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3f};
unsigned char TRX_Open[7] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00} ;
unsigned char TRX_Gnd[7] = {0xff,0xff,0xff,0xff,0x3,0xff,0xfc} ;
unsigned char TRX_Short[7] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00} ;
int HighResistanceLowerLimit[3] = {-1000, -1000, -400};
int HighResistanceUpperLimit[3] = {450, 450, 200};
unsigned int AbsShort[TRX_max*2] = {0};
unsigned int AbsOpen[TRX_max*2] = {0};
int AbsTxShortLimit = 0;
int AbsRxShortLimit = 0;
int AbsTxOpenLimit = 0;
int AbsRxOpenLimit = 1000;
int AbsRawRef[16] = {77, 11919, 14023, 15163, 16192, 18319, 19337, 21491, 22633, 24692, 26853, 27993, 30147, 32253, 34411, 37605};
short NoiseDeltaMin[TRX_max][TRX_max];
short NoiseDeltaMax[TRX_max][TRX_max];
short NoiseLimitLow = -16;
short NoiseLimitHigh = 16;

enum {
	STARTTIME,
	ENDTIME,
	TIME_PROFILE_MAX
};

#define get_time_interval(a,b) a>=b ? a-b : 1000000+a-b
struct timeval t_interval[TIME_PROFILE_MAX];
static int outbuf = 0;
static int out_buf = 0;
char f54_wlog_buf[6000] = {0};
char wlog_buf[6000] = {0};

int UpperImage[32][32];
int LowerImage[32][32];
int SensorSpeedUpperImage[32][32];
int SensorSpeedLowerImage[32][32];
int ADCUpperImage[32][32];
int ADCLowerImage[32][32];

struct i2c_client* ds4_i2c_client;

int Read8BitRegisters(unsigned short regAddr, unsigned char *data, int length)
{
	//         
	int rst = 0;

	rst = touch_i2c_read_byte(ds4_i2c_client, regAddr, length, data);

	return rst;
}

int Write8BitRegisters(unsigned short regAddr, unsigned char *data, int length)
{
	//          
	int rst = 0;

	rst = touch_i2c_write(ds4_i2c_client, regAddr, length, data);

	return rst;
}

void delayMS(int val)
{
	//                
	msleep(val);
}

int write_file(char *filename, char *data)
{
	int fd = 0;

	fd = sys_open(filename, O_WRONLY|O_CREAT|O_APPEND, 0666);
	if (fd < 0) {
		LGTC_DBG("%s :  Open file error [ %d ]\n",__func__, fd);
		return fd;
	} else {
		sys_write(fd, data, strlen(data));
		sys_close(fd);
	}
	return 0;
}

int write_log(char *filename, char *data)
{
	int fd;
	char *fname = "/mnt/sdcard/touch_self_test.txt";
	int cap_file_exist = 0;

	mm_segment_t old_fs = get_fs();
	set_fs(KERNEL_DS);
	if(filename == NULL){
		fd = sys_open(fname, O_WRONLY|O_CREAT|O_APPEND, 0666);
		LGTC_DBG("write log in /mnt/sdcard/touch_self_test.txt\n");
	} else{
		fd = sys_open(filename, O_WRONLY|O_CREAT, 0666);
		LGTC_DBG("write log in /sns/touch/cap_diff_test.txt\n");
	}

	LGTC_DBG("[%s]write file open %s, fd : %d\n", __FUNCTION__, (fd >= 0)? "success": "fail", fd);

	if(fd >= 0) {
		sys_write(fd, data, strlen(data));
		sys_close(fd);

		if(filename != NULL)
			cap_file_exist = 1;
	}
	set_fs(old_fs);

	return cap_file_exist;
}

//                                          
bool switchPage(int page) {
	unsigned char values[1] = {0};
	unsigned char data = 0;

	pageNum = values[0] = page;

	count = 0;
	do {
		Write8BitRegisters(0xFF, values, 1);
		msleep(1);
		Read8BitRegisters(0xFF, &data, 1);
		count++;
	} while ((int)data != page && (count < DefaultTimeout));
	if(count >= DefaultTimeout){
		LGTC_DBG("Timeout -- Page switch fail ! \n");
		return false;
	}
	return true;
}

void Reset(void)
{
	unsigned char data;

	switchPage(0x00);

	data = 0x01;
	Write8BitRegisters(F01CommandBase, &data, 1);

	msleep(10);
}

//                                                 
int CompareImageReport(void)
{
	bool result = true;
	int i = 0;
	int j = 0;

	//               
	for (j = 0; j < (int)F12_2DRxCount; j++){
		for (i = 0; i < (int)F12_2DTxCount; i++) {
			if ((ImagepF[i][j] < LowerImageLimit[i][j]) || (ImagepF[i][j] > UpperImageLimit[i][j])) {
				outbuf += sprintf(f54_wlog_buf+outbuf, "FAIL, %d,%d,%d\n", LowerImageLimit[i][j], UpperImageLimit[i][j], ImagepF[i][j]);
				result = false;
				break;
			}
		}
	}

	if (result)
		outbuf += sprintf(f54_wlog_buf+outbuf, "\nFull Raw Capacitance Image Test passed.\n\n");
	else
		outbuf += sprintf(f54_wlog_buf+outbuf, "\nFull Raw Capacitance Image Test failed.\n\n");

	return (result)?1:0;
}

//                                                
int CompareHighResistance(int maxRxpF, int maxTxpF, int minpF )
{
	bool result = true;

	if ( maxRxpF > HighResistanceUpperLimit[0] || maxRxpF < HighResistanceLowerLimit[0])
		result = false;
	if ( maxTxpF > HighResistanceUpperLimit[1] || maxTxpF < HighResistanceLowerLimit[1])
		result = false;
	if ( minpF > HighResistanceUpperLimit[2] || minpF < HighResistanceLowerLimit[2])
		result = false;

	if (result == false) {
		LGTC_DBG ("HighResistance Test failed.\n");
		outbuf += sprintf(f54_wlog_buf+outbuf, "HighResistance Test failed.\n\n");
	} else {
		LGTC_DBG("HighResistance Test passed.\n");
		outbuf += sprintf(f54_wlog_buf+outbuf, "HighResistance Test passed.\n\n");
	}
	return (result)?1:0;
}


//                                                 
int CompareSensorSpeedReport(void)
{
	bool result = true;
	int i,j = 0;

	for (i = 0; i < (int)F12_2DTxCount; i++){
		for (j = 0; j < (int)F12_2DRxCount; j++){
			//                                                                                                                                                               
			if ((ImagepF[i][j] < SensorSpeedLowerImageLimit[i][j]) || (ImagepF[i][j] > SensorSpeedUpperImageLimit[i][j])){
				result = false;
				LGTC_DBG("Failed : Tx[%d] Rx[%d] -> LOWER : %d Upper : %d  IMAGE DATA : %d\n", i, j, SensorSpeedLowerImageLimit[i][j], SensorSpeedUpperImageLimit[i][j], ImagepF[i][j]);
				out_buf += sprintf(wlog_buf+out_buf, "Failed : Tx[%2d] Rx[%2d] = %3d\n", i, j, ImagepF[i][j]);
				break;
			}
		}
	}

	if (result) {
		LGTC_DBG("Sensor Speed Test passed.\n");
		out_buf += sprintf(wlog_buf+out_buf, "\nSensor Speed Test passed.\n\n");
	} else {
		LGTC_DBG("Sensor Speed Test failed.\n");
		out_buf += sprintf(wlog_buf+out_buf, "\nSensor Speed Test failed.\n\n");
	}

	return (result)?1:0;
}

//                                                 
int CompareADCReport(void)
{
	bool result = true;
	int i,j = 0;

	for (i = 0; i < (int)F12_2DTxCount; i++){
		for (j = 0; j < (int)F12_2DRxCount; j++){
			//                                                                                                                                              
			if ((Image1[i][j] < ADCLowerImageLimit[i][j]) || (Image1[i][j] > ADCUpperImageLimit[i][j])){
				printk("[Touch] Failed : Tx[%d] Rx[%d] -> LOWER : %d Upper : %d IMAGE DATA : %u\n",i ,j, ADCLowerImageLimit[i][j], ADCUpperImageLimit[i][j], Image1[i][j]);
				out_buf += sprintf(wlog_buf+out_buf, "Failed : Tx[%2d] Rx[%2d] = %3u\n", i, j, Image1[i][j]);
				result = false;
				break;
			}
		}
	}

	if (result) {
		LGTC_DBG("ADC Range Test passed.\n");
		out_buf += sprintf(wlog_buf+out_buf, "\nADC Range Test passed.\n\n");
	} else {
		LGTC_DBG("ADC Range Test failed.\n");
		out_buf += sprintf(wlog_buf+out_buf, "\nADC Range Test failed.\n\n");
	}

	return (result)?1:0;
}

void CompareAbsADCRangeReport(void)
{
	bool result = true;
	int i = 0;

	for (i = 0; i < RxChannelCount + F12_2DTxCount; i++) {
		if (i == F12_2DRxCount ){
			i = RxChannelCount;
		}
		if ((AbsADCRangeData[i] < LOWER_ABS_ADC_RANGE_LIMIT) || (AbsADCRangeData[i] > UPPER_ABS_ADC_RANGE_LIMIT)) {
			result = false;
			break;
		}
	}

	if (result) {
		LGTC_DBG("\nAbs Sensing ADC Range Test Passed.\n");
	} else {
		LGTC_DBG("\nAbs Sensing ADC Range Test Failed.\n");
	}
}

void CompareAbsRawReport(void)
{
	bool result = true;
	int i = 0;

	for (i = 0; i < RxChannelCount + F12_2DTxCount; i++) {
		if (i == F12_2DRxCount ){
			i = RxChannelCount;
		}
		if ((AbsSigned32Data[i] < LOWER_ABS_RAW_CAP_LIMIT) || (AbsSigned32Data[i] > UPPER_ABS_RAW_CAP_LIMIT)) {
			result = false;
			break;
		}
	}

	if (result) {
		LGTC_DBG("\nAbs Sensing Raw Capacitance Test Passed.\n");
	} else {
		LGTC_DBG("\nAbs Sensing Raw Capacitance Test Failed.\n");
	}
}

int CompareAbsOpen(void)
{
	bool result = true;
	int i = 0;

	for (i = 0 ;i < ((int)F12_2DRxCount + (int)F12_2DTxCount); i++) {
		if (i < (int)F12_2DRxCount) {
			if (AbsOpen[i] <= AbsRxOpenLimit) {
				result = false;
				LGTC_DBG("RX[%d] failed value:  %d\n",i ,AbsOpen[i]);
			}

		} else {
			if (AbsOpen[i] <= AbsTxOpenLimit) {
				result = false;
				LGTC_DBG("TX[%d] failed value:  %d\n",i - (int)F12_2DRxCount ,AbsOpen[i]);
			}
		}

	}

	LGTC_DBG("AbsRxOpenLimit:  %d  AbsTxOpenLimit : %d\n",AbsRxOpenLimit , AbsTxOpenLimit);

	if (result) {
		LGTC_DBG ("Abs Sensing Open Test Passed.\n");
		outbuf += sprintf(f54_wlog_buf+outbuf, "Abs Sensing Open Test passed.\n\n");
	} else {
		LGTC_DBG("Abs Sensing Open Test Failed.\n");
		outbuf += sprintf(f54_wlog_buf+outbuf, "Abs Sensing Open Test failed.\n\n");
	}

	return (result)?1:0;
}

int CompareAbsShort(void)
{
	bool result = true;
	int i = 0;

	for (i = 0 ;i < ((int)F12_2DRxCount + (int)F12_2DTxCount); i++) {
		if (i < (int)F12_2DRxCount) {
			if (AbsShort[i] >= AbsRxShortLimit) {
				result = false;
				LGTC_DBG("RX[%d] failed value:  %d\n",i ,AbsShort[i]);
			}

		} else {
			if (AbsShort[i] >= AbsTxShortLimit) {
				result = false;
				LGTC_DBG("TX[%d] failed value:  %d\n",i - (int)F12_2DRxCount ,AbsShort[i]);
			}

		}

	}

	LGTC_DBG("AbsRxShortLimit:  %d  AbsTxShortLimit : %d\n",AbsRxShortLimit , AbsTxShortLimit);

	if (result) {
		LGTC_DBG ("Abs Sensing Short Test Passed.\n");
		outbuf += sprintf(f54_wlog_buf+outbuf, "Abs Sensing Short Test passed.\n\n");
	} else {
		LGTC_DBG("Abs Sensing Short Test Failed.\n");
		outbuf += sprintf(f54_wlog_buf+outbuf, "Abs Sensing Short Test failed.\n\n");
	}

	return (result)?1:0;

}

//                                                 
void CompareTRexOpenTestReport(int i)
{
	int index;
	for(index = 0; index < 7; index++){
		if(Data[index] != TRX_Open[index]){
			LGTC_DBG("\nTRex Open Test failed.\n");
			return;
		}
	}

	LGTC_DBG("\nTRex Open Test passed.\n");
}

//                                                 
int CompareTRexGroundTestReport(int i)
{
	int index;//               

	for(index = 0; index < 7; index++){
		if(Data[index] !=  TRX_Gnd[index]){
			outbuf += sprintf(f54_wlog_buf+outbuf, "\nTRex Ground Test failed.\n\n");
		}
	}

	outbuf += sprintf(f54_wlog_buf+outbuf, "\nTRex Ground Test passed.\n\n");
	return 1;
}

//                                                 
int CompareTRexShortTestReport(int i)
{
	int index;//         
	for(index = 0; index < 7; index++){
		if(Data[index] != TRX_Short[index]){
			outbuf += sprintf(f54_wlog_buf+outbuf, "\nTRex-TRex Short Test failed.\n\n");
			return 0;
			//                                         
			//       
		}
	}

	outbuf += sprintf(f54_wlog_buf+outbuf, "\nTRex-TRex Short Test passed.\n\n");

	return 1;
}

//                                                
int CompareNoiseReport(void)
{
	bool result = true;
	int i,j = 0;

	LGTC_DBG("\n\nNoise Test Data :\n");
	out_buf += sprintf(wlog_buf+out_buf, "\nNoise Test Data : \n");
	out_buf += sprintf(wlog_buf+out_buf, "==========================================================================================================\n         :");

	for (i = 0; i < (int)RxChannelCount; i++)
		out_buf += sprintf(wlog_buf+out_buf, "%5d ", i);

	out_buf += sprintf(wlog_buf+out_buf, "\n----------------------------------------------------------------------------------------------------------\n");
	for (i = 0; i < TxChannelCount; i++){
		printk("[Touch] Tx[%2d]: ", i);
		out_buf += sprintf(wlog_buf+out_buf, "   %5d : ", i);
		for (j = 0; j < RxChannelCount; j++){
			ImagepF[i][j] = NoiseDeltaMax[i][j] - NoiseDeltaMin[i][j];
			printk("%3d,", ImagepF[i][j]);
			out_buf += sprintf(wlog_buf+out_buf, "%5d ", ImagepF[i][j]);
			//                           
		}
		printk("\n");
		out_buf += sprintf(wlog_buf+out_buf, "\n");
	}
	out_buf += sprintf(wlog_buf+out_buf, "------------------------------------------------------------------------------------------------------------\n");

	//               
	/*                                                
                                                                                                                                               
                                                                                                       
                 
   
   
  */
	//               
	for (i = 0; i < F12_2DTxCount; i++){
		for (j = 0; j < F12_2DRxCount; j++){
			if ((ImagepF[i][j] < NoiseLimitLow) || (ImagepF[i][j] > NoiseLimitHigh)) {
				LGTC_DBG("\tFailed: 2D area: Tx [%d] Rx [%d]\n",i, j);
				out_buf += sprintf(wlog_buf+out_buf, "Failed Tx [%2d] Rx [%2d] = %3d\n", i, j, ImagepF[i][j]);
				result = false;
			}
		}
	}

	if (result == false) {
		LGTC_DBG ("Noise Test failed.\n");
		out_buf += sprintf(wlog_buf+out_buf, "\nNoise Test failed.\n\n");
	} else {
		LGTC_DBG("Noise Test passed.\n");
		out_buf += sprintf(wlog_buf+out_buf, "\nNoise Test passed.\n\n");
	}

	return (result)?1:0;
}


//                                         
int ReadImageReport(void)
{
	int ret = 0;
	int i,j,k = 0;

	Read8BitRegisters((F54DataBase+REPORT_DATA_OFFEST), &Data[0], MaxArrayLength);

	outbuf += sprintf(f54_wlog_buf+outbuf, "\nInfo: Tx = %d Rx = %d \n", (int)TxChannelCount, (int)RxChannelCount);
	outbuf += sprintf(f54_wlog_buf+outbuf, "Image Data : \n");
	outbuf += sprintf(f54_wlog_buf+outbuf, "==========================================================================================================\n         :");

	for (i = 0; i < (int)RxChannelCount; i++)
		outbuf += sprintf(f54_wlog_buf+outbuf, "%5d ", i);

	outbuf += sprintf(f54_wlog_buf+outbuf, "\n----------------------------------------------------------------------------------------------------------\n");

	for (i = 0; i < (int)TxChannelCount; i++){
		outbuf += sprintf(f54_wlog_buf+outbuf, "   %5d : ", i);
		for (j = 0; j < (int)RxChannelCount; j++){
			Image1[i][j] = ((short)Data[k] | (short)Data[k+1] << 8);
			ImagepF[i][j] = Image1[i][j];
			outbuf += sprintf(f54_wlog_buf+outbuf, "%5d ", ImagepF[i][j]);
			k = k + 2;
		}
		outbuf += sprintf(f54_wlog_buf+outbuf, "\n");
	}
	outbuf += sprintf(f54_wlog_buf+outbuf, "------------------------------------------------------------------------------------------------------------\n");

	ret = CompareImageReport();
	write_log(NULL, f54_wlog_buf);
	msleep(30);

	return ret;
}

//                                         
int GetImageReport(char *buf)
{
	int ret = 0;
	int i,j,k = 0;

	Read8BitRegisters((F54DataBase+REPORT_DATA_OFFEST), &Data[0], MaxArrayLength);

	*buf = 0;
	ret += sprintf(buf+ret, "\n\nInfo: Tx = %d Rx = %d \n\n", (int)TxChannelCount, (int)RxChannelCount);
	ret += sprintf(buf+ret, "==========================================================================================================\n         :");

	for (i = 0; i < (int)RxChannelCount; i++)
		ret += sprintf(buf+ret, "%5d ", i);

	ret += sprintf(buf+ret, "\n----------------------------------------------------------------------------------------------------------\n");

	for (i = 0; i < (int)TxChannelCount; i++) {
		ret += sprintf(buf+ret, "   %5d : ", i);
		for (j = 0; j < (int)RxChannelCount; j++) {
			Image1[i][j] = ((short)Data[k] | (short)Data[k+1] << 8);
			ImagepF[i][j] = Image1[i][j];
			ret += sprintf(buf+ret, "%5d ", ImagepF[i][j]);
			k = k + 2;
		}
		ret += sprintf(buf+ret, "\n");
	}
	ret += sprintf(buf+ret, "------------------------------------------------------------------------------------------------------------\n");

	return ret;
}

//                                        
int ReadNoiseReport(void)
{
	int ret = 0;
	int i,j,k = 0;

	//              
	unsigned char fifoIndex[2] = {0, 0};
	Write8BitRegisters(F54DataBase + 1, fifoIndex, sizeof(fifoIndex));

	Read8BitRegisters((F54DataBase+REPORT_DATA_OFFEST), &Data[0], MaxArrayLength);

	for (i = 0; i < (int)TxChannelCount; i++) {
		for (j = 0; j < (int)RxChannelCount; j++) {
			Image1[i][j] = (short)Data[k] | ((short)Data[k + 1] << 8);
			ImagepF[i][j] = Image1[i][j];

			if (ImagepF[i][j] < NoiseDeltaMin[i][j])
			{
				NoiseDeltaMin[i][j] = ImagepF[i][j];
			}
			if (ImagepF[i][j] > NoiseDeltaMax[i][j])
			{
				NoiseDeltaMax[i][j] = ImagepF[i][j];
			}

			k = k + 2;
		}
	}
	ret = CompareNoiseReport();
	write_log(NULL, wlog_buf);
	msleep(30);

	return ret ;
}

//                                        
int ReadHighResistanceReport(void)
{
	short maxRx, maxTx, min;
	int maxRxpF, maxTxpF, minpF;
	int ret = 0;
	int i = 0;

	memset(Data, 0x00, sizeof(Data));
	Read8BitRegisters((F54DataBase+REPORT_DATA_OFFEST), Data, 6);

	maxRx = ((short)Data[0] | (short)Data[1] << 8);
	maxTx = ((short)Data[2] | (short)Data[3] << 8);
	min = ((short)Data[4] | (short)Data[5] << 8);

	maxRxpF = maxRx;
	maxTxpF = maxTx;
	minpF = min;

	outbuf += sprintf(f54_wlog_buf+outbuf, "Max Rx Offset(pF) = %d\n", maxRxpF);
	outbuf += sprintf(f54_wlog_buf+outbuf, "Max Tx Offset(pF) = %d\n", maxTxpF);
	outbuf += sprintf(f54_wlog_buf+outbuf, "Min(pF) = %d\n", minpF);

	outbuf += sprintf(f54_wlog_buf+outbuf, "\n=====================================================\n");
	outbuf += sprintf(f54_wlog_buf+outbuf, "\tHigh Resistance Test\n");
	outbuf += sprintf(f54_wlog_buf+outbuf, "=====================================================\n");
	outbuf += sprintf(f54_wlog_buf+outbuf, " Parameters: ");
	outbuf += sprintf(f54_wlog_buf+outbuf, "%5d %5d %5d ", maxRxpF, maxTxpF, minpF);
	outbuf += sprintf(f54_wlog_buf+outbuf, "\n\n Limits(+) : ");
	for(i = 0; i < 3; i++)
		outbuf += sprintf(f54_wlog_buf+outbuf, "%5d ", HighResistanceUpperLimit[i]);

	outbuf += sprintf(f54_wlog_buf+outbuf, "\n Limits(-) : ");
	for(i = 0; i < 3; i++)
		outbuf += sprintf(f54_wlog_buf+outbuf, "%5d ", HighResistanceLowerLimit[i]);

	outbuf += sprintf(f54_wlog_buf+outbuf, "\n-----------------------------------------------------\n");

	ret = CompareHighResistance(maxRxpF, maxTxpF, minpF);
	write_log(NULL, f54_wlog_buf);
	msleep(30);

	return ret;
}

//                                         
void ReadMaxMinReport(void)
{
	short max, min;
	int maxpF, minpF;

	Read8BitRegisters((F54DataBase+REPORT_DATA_OFFEST), Data, 4);

	max = ((short)Data[0] | (short)Data[1] << 8);
	min = ((short)Data[2] | (short)Data[3] << 8);
	maxpF = max;
	minpF = min;

	LGTC_DBG("\nRaw Capacitance Maximum and Minimum Test:\n");
	/*                              
                                 */
	LGTC_DBG("Max(pF) = %d\n", maxpF);
	LGTC_DBG("Min(pF) = %d\n", minpF);

}

//                                         
int ReadADCRangeReport(void)
{
	int temp = TxChannelCount;
	int ret = 0;
	int i, j, k = 0;

	if (SignalClarityOn){
		if ((TxChannelCount / 4) != 0)	{
			temp = (4 - (TxChannelCount % 4)) +  TxChannelCount;
			Read8BitRegisters((F54DataBase+REPORT_DATA_OFFEST), Data, (temp * RxChannelCount * 2));
		}
	} else {
		Read8BitRegisters((F54DataBase+REPORT_DATA_OFFEST), &Data[0], MaxArrayLength);
	}

	k = 0;

	LGTC_DBG("ADC Range Data: \n");
	//                                                                   
	for (i = 0; i < (int)TxChannelCount; i++){
		//                               
		//                                                       
		for (j = 0; j < (int)RxChannelCount; j++){
			//                                                                           
			Image1[i][j] = ((unsigned short)Data[k]);
			//                                    
			//                             
			//                                                             
			k = k + 2;
		}
		//             
		//                                             
	}

	ret = CompareADCReport();
	write_log(NULL, wlog_buf);
	msleep(20);

	return ret;
}

void ReadAbsADCRangeReport(void)
{
	int i, k = 0;

	Read8BitRegisters((F54DataBase + REPORT_DATA_OFFEST), &Data[0], 2 * (RxChannelCount + TxChannelCount));

	LGTC_DBG("Abs Sensing ADC Range Data:\n");
	LGTC_DBG("Rx: ");
	for (i = 0; i < (int)RxChannelCount; i++) {
		AbsADCRangeData[k / 2] = (unsigned char)Data[k];
		LGTC_DBG("%d ", AbsADCRangeData[k / 2]);
		k = k + 2;
	}
	LGTC_DBG("\n");
	LGTC_DBG("Tx: ");
	for (i = 0; i < (int)TxChannelCount; i++) {
		AbsADCRangeData[k / 2] = (unsigned char)Data[k];
		LGTC_DBG("%d ", AbsADCRangeData[k / 2]);
		k = k + 2;
	}
	LGTC_DBG("\n");

	CompareAbsADCRangeReport();
}

void ReadAbsDeltaReport(void)
{
	int i, k = 0;
	int *p32data;

	Read8BitRegisters((F54DataBase + REPORT_DATA_OFFEST), &Data[0], 4 * (RxChannelCount + TxChannelCount));

	p32data = (int *)&Data[0];

	outbuf += sprintf(f54_wlog_buf+outbuf, "Abs Sensing Delta Capacitance Data:\n");
	outbuf += sprintf(f54_wlog_buf+outbuf, "Rx: ");
	for (i = 0; i < (int)RxChannelCount; i++) {
		AbsSigned32Data[k] = (int)*p32data;
		outbuf += sprintf(f54_wlog_buf+outbuf, "%d ", AbsSigned32Data[k]);
		k++;
		p32data++;
	}

	outbuf += sprintf(f54_wlog_buf+outbuf, "\nTx: ");
	for (i = 0; i < (int)TxChannelCount; i++) {
		AbsSigned32Data[k] = (int)*p32data;
		outbuf += sprintf(f54_wlog_buf+outbuf, "%d ", AbsSigned32Data[k]);
		k++;
		p32data++;
	}

	outbuf += sprintf(f54_wlog_buf+outbuf, "\n");
}

void ReadAbsRawReport(void)
{
	int i, k = 0;
	int *p32data;

	Read8BitRegisters((F54DataBase + REPORT_DATA_OFFEST), &Data[0], 4 * (RxChannelCount + TxChannelCount));

	p32data = (int *)&Data[0];

	LGTC_DBG("Abs Sensing Raw Capacitance Data:\n");
	outbuf += sprintf(f54_wlog_buf+outbuf, "Abs Sensing Raw Capacitance Data:\n");
	LGTC_DBG("Rx: ");
	outbuf += sprintf(f54_wlog_buf+outbuf, "Rx: ");
	for (i = 0; i < (int)RxChannelCount; i++) {
		AbsSigned32Data[k] = (int)*p32data;
		LGTC_DBG("%d ", AbsSigned32Data[k]);
		outbuf += sprintf(f54_wlog_buf+outbuf, "%d ", AbsSigned32Data[k]);
		k++;
		p32data++;
	}
	LGTC_DBG("\n");
	LGTC_DBG("Tx: ");
	outbuf += sprintf(f54_wlog_buf+outbuf, "\nTx: ");
	for (i = 0; i < (int)TxChannelCount; i++) {
		AbsSigned32Data[k] = (int)*p32data;
		LGTC_DBG("%d ", AbsSigned32Data[k]);
		outbuf += sprintf(f54_wlog_buf+outbuf, "%d ", AbsSigned32Data[k]);
		k++;
		p32data++;
	}
	LGTC_DBG("\n");
	outbuf += sprintf(f54_wlog_buf+outbuf, "\n");

	CompareAbsRawReport();
}

//                                         
int ReadAbsRawOpen(void)
{
	int i = 0;
	int ret = 0;
	unsigned char k = 0;

	Read8BitRegisters((F54DataBase + REPORT_DATA_OFFEST), &Data[0], (F12_2DRxCount + F12_2DTxCount) * 4);

	LGTC_DBG("Abs Sensing Open Test Data:\n");
	outbuf += sprintf(f54_wlog_buf+outbuf, "Abs Sensing Open Test Data:\n");

	for(i = 0; i < ((int)F12_2DRxCount + (int)F12_2DTxCount); i++)
	{

		AbsOpen[i] = (unsigned int)Data[k] | ((unsigned int)Data[k + 1] << 8) |
			((unsigned int)Data[k + 2] << 16) | ((unsigned int)Data[k + 3] << 24);

		k += 4;

		if (i < (int)F12_2DRxCount) {
			LGTC_DBG("RX[%d]: %d, ", i, AbsOpen[i]);
			outbuf += sprintf(f54_wlog_buf+outbuf, "%5d ", AbsOpen[i]);
		}
		else {
			LGTC_DBG("TX[%d]: %d, ", i - (int)F12_2DRxCount, AbsOpen[i]);
			outbuf += sprintf(f54_wlog_buf+outbuf, "%5d ", AbsOpen[i]);
		}

		if(i == ((int)F12_2DRxCount - 1)) {
			LGTC_DBG("\n");
			outbuf += sprintf(f54_wlog_buf+outbuf, "\n");
		}
	}
	LGTC_DBG("\n");

	ret = CompareAbsOpen();

	return ret;
}

//                                         
int ReadAbsRawShort(void)
{
	int i = 0;
	int ret = 0;
	unsigned char k = 0;

	Read8BitRegisters((F54DataBase + REPORT_DATA_OFFEST), &Data[0], (F12_2DRxCount + F12_2DTxCount) * 4);

	LGTC_DBG("Abs Sensing Short Test Data:\n");
	outbuf += sprintf(f54_wlog_buf+outbuf, "Abs Sensing Short Test Data:\n");

	for(i = 0; i < ((int)F12_2DRxCount + (int)F12_2DTxCount); i++ )
	{

		AbsShort[i] = (unsigned int)Data[k] | ((unsigned int)Data[k + 1] << 8) |
			((unsigned int)Data[k + 2] << 16) | ((unsigned int)Data[k + 3] << 24);

		k += 4;

		if (i < (int)F12_2DRxCount) {
			LGTC_DBG("RX[%d]: %d, ", i, AbsShort[i]);
			outbuf += sprintf(f54_wlog_buf+outbuf, "%5d ", AbsShort[i]);
		}
		else {
			LGTC_DBG("TX[%d]: %d, ", i - (int)F12_2DRxCount, AbsShort[i]);
			outbuf += sprintf(f54_wlog_buf+outbuf, "%5d ", AbsShort[i]);
		}

		if(i == ((int)F12_2DRxCount - 1)) {
			LGTC_DBG("\n");
			outbuf += sprintf(f54_wlog_buf+outbuf, "\n");
		}
	}
	LGTC_DBG("\n");

	ret = CompareAbsShort();

	return ret;
}

//                                         
int ReadSensorSpeedReport(void)
{
	int i,j,k = 0;
	int ret = 0;

	Read8BitRegisters((F54DataBase+REPORT_DATA_OFFEST), &Data[0], MaxArrayLength);

	LGTC_DBG("Sensor speed Test Data : \n");
	//                                                                    

	for (i = 0; i < (int)TxChannelCount; i++)
	{
		//                               
		//                                                      
		for (j = 0; j < (int)RxChannelCount; j++)
		{
			Image1[i][j] = ((short)Data[k] | (short)Data[k+1] << 8);
			ImagepF[i][j] = Image1[i][j];
			//                              
			//                                                              
			k = k + 2;
		}
		//             
		//                                             
	}

	ret = CompareSensorSpeedReport();
	write_log(NULL, wlog_buf);
	msleep(20);

	return ret;
}

int pow_func(int x, int y)
{
	int result = 1;
	int i = 0;
	for(i = 0; i < y; i++)
		result *= x;
	return result;
}

//                                         
void ReadTRexOpenReport(void)
{
	int i, j = 0;
	//                                      
	int k = 7, mask = 0x01, value;//                                                     

	Read8BitRegisters((F54DataBase+REPORT_DATA_OFFEST), Data, k);

	for (i = 0; i < k ; i++){
		value = Data[i];
		Data[i] = 0;
		for (j = 0; j < 8; j++){
			if((value & mask) == 1) {
				Data[i] = Data[i] + (unsigned char)pow_func(2, (7 - j));
			}
			value >>= 1;
		}
		LGTC_DBG("TRex-Open Test Data = %#x,", Data[i]);
	}
	LGTC_DBG("\n");

	CompareTRexOpenTestReport(k * 8);
}

//                                         
int ReadTRexGroundReport(void)
{
	int ret = 0;
	int i,j = 0;
	//                                      
	int k = 7, mask = 0x01, value;//                                                     
	Read8BitRegisters((F54DataBase+REPORT_DATA_OFFEST), Data, k);

	for (i = 0; i < k; i++){
		value = Data[i];
		Data[i] = 0;
		for (j = 0; j < 8; j++){
			if((value & mask) == 1) {
				Data[i] = Data[i] + (unsigned char)pow_func(2, (7 - j));
			}
			value >>= 1;
		}

		outbuf += sprintf(f54_wlog_buf+outbuf, "TRex-Ground Test Data = %#x\n", Data[i]);
	}
	outbuf += sprintf(f54_wlog_buf+outbuf, "\n");

	ret = CompareTRexGroundTestReport(k * 8);
	write_log(NULL, f54_wlog_buf);
	msleep(30);

	return ret;
}

//                                         
int ReadTRexShortReport(void)
{
	int ret = 0;
	int i, j = 0;
	//                                      
	int k = 7, mask = 0x01, value;//                                                     
	char buf[40] = {0};

	memset(Data, 0x00, sizeof(Data));

	Read8BitRegisters((F54DataBase+REPORT_DATA_OFFEST), Data, k);

	for (i = 0; i < k; i++)	{
		value = Data[i];
		Data[i] = 0;
		ret = 0;
		for (j = 0; j < 8; j++){
			if((value & mask) == 1) {
				Data[i] = Data[i] + (unsigned char)pow_func(2, (7 - j));
				ret += sprintf(buf+ret, "%d ", (i * 8 + (7 - j)));
			}
			value >>= 1;
		}
		outbuf += sprintf(f54_wlog_buf+outbuf, "TRex-TRex Short Test Data = %#x", Data[i]);

		if (ret) {
			outbuf += sprintf(f54_wlog_buf+outbuf, " (Short TRx Number: ");
			outbuf += sprintf(f54_wlog_buf+outbuf, buf);
			outbuf += sprintf(f54_wlog_buf+outbuf, ")");
		}
		outbuf += sprintf(f54_wlog_buf+outbuf, "\n");
	}

	outbuf += sprintf(f54_wlog_buf+outbuf, "\n");

	ret = CompareTRexShortTestReport(k * 8);
	write_log(NULL, f54_wlog_buf);
	msleep(30);

	return ret;
}

//                                                    
int ReadReport(unsigned char input, char *buf)
{
	int ret = 0;
	unsigned char data;

	//                                         
	data = 0x01;
	Write8BitRegisters(F54CommandBase, &data, 1);

	count = 0;
	do {
		Read8BitRegisters(F54CommandBase, &data, 1);
		msleep(1);
		count++;
	} while (data != 0x00 && (count < DefaultTimeout));
	if(count >= DefaultTimeout) {
		outbuf += sprintf(f54_wlog_buf+outbuf, "Timeout -- Not supported Report Type in FW\n");
		Reset();
		return ret;
	}

	do_gettimeofday(&t_interval[ENDTIME]);

	LGTC_DBG("Takes %lu ticks\n", get_time_interval(t_interval[ENDTIME].tv_sec,t_interval[STARTTIME].tv_sec));

	switch (input){
		case 'a':
			ret = ReadImageReport();
			break;
		case 'b':
			ret = ReadADCRangeReport();
			break;
		case 'c':
			ret = ReadSensorSpeedReport();
			break;
		case 'd':
			//                                                                                    
			//            
			ReadTRexOpenReport();
			break;
		case 'e':
			ret = ReadTRexGroundReport();
			break;
		case 'f':
			ret = ReadTRexShortReport();
			break;
		case 'g':
			ret = ReadHighResistanceReport();
			break;
		case 'h':
			ReadMaxMinReport();
			break;
		case 'i':
			ReadAbsADCRangeReport();
			break;
		case 'j':
			ReadAbsDeltaReport();
			break;
		case 'k':
			ReadAbsRawReport();
			break;
		case 'l':
			ret = GetImageReport(buf);
			break;
		case 'm':
			ret = ReadNoiseReport();
			break;
		case 'n':
			ret = ReadAbsRawShort();
			break;
		case 'o':
			ret = ReadAbsRawOpen();
			break;
		default:
			break;
	}

	return ret;
}

//                                                                                                         
void RunQueries(void)
{
	unsigned short cAddr = 0xEE;
	unsigned char cFunc = 0;
	int rxCount = 0;
	int txCount = 0;
	int offset = 0;
	int query_offset = 0;
	int i,j = 0;
#if defined(CONFIG_MACH_MSM8974_VU3_KR)
	int k = 0;
	int cnt = 0;
#endif

	//                                                                                      
	//                                                                                             
	//                                                               
	//                                                                                        
	do{
		Read8BitRegisters(cAddr, &cFunc, 1);
		if (cFunc == 0){
			break;
		}

		switch (cFunc){
			case 0x01:
				if (!bHaveF01){
					Read8BitRegisters((cAddr-3), &F01ControlBase, 1);
					Read8BitRegisters((cAddr-4), &F01CommandBase, 1);
					break;
				}
#if defined(CONFIG_MACH_MSM8974_VU3_KR)
			case 0x1a:
				if(!bHaveF1A){
					k =0;
					Read8BitRegisters((cAddr-3), &F1AControlBase, 1);
					Read8BitRegisters(F1AControlBase+1, &ButtonCount, 1);
					//                                                    
					while(ButtonCount){
						cnt++;
						ButtonCount=(ButtonCount>>1);
					}
					ButtonCount=cnt;
					for (i = 0; i < ButtonCount; i++){
						Read8BitRegisters(F1AControlBase + 3 + k, &ButtonTx[i], 1);
						Read8BitRegisters(F1AControlBase + 3 + k + 1 , &ButtonRx[i], 1);
						k= k +2;
					}
					//                                                     
					bHaveF1A = true;
				}
				break;
#endif
			case 0x12:
				if(!bHaveF12){
					Read8BitRegisters((cAddr-3), &F12ControlBase, 1);
					Read8BitRegisters((cAddr-5), &F12QueryBase, 1);
					Read8BitRegisters((F12QueryBase), &F12Support, 1);

					if( (F12Support | 0x00) == 0  ){
						LGTC_DBG("Device not support F12.\n");
						break;
					}
					Read8BitRegisters((F12QueryBase+5), Data, 2);
					mask = 0x01;
					for (j = 0; j < 8; j++){
						if((Data[1] & mask) == 1) offset++;
						Data[1] >>= 1;
					}
					Read8BitRegisters((F12ControlBase + offset), Data, 14);
					F12_2DRxCount = Data[12];
					F12_2DTxCount = Data[13];

					if(TRX_max<=F12_2DRxCount)
						F12_2DRxCount = TRX_max;
					if(TRX_max<=F12_2DTxCount)
						F12_2DTxCount = 16;

					offset = 0;
					break;
				}
			case 0x54:
				if (!bHaveF54){
					Read8BitRegisters((cAddr-2), &F54DataBase, 1);
					Read8BitRegisters((cAddr-3), &F54ControlBase, 1);
					Read8BitRegisters((cAddr-4), &F54CommandBase, 1);
					Read8BitRegisters((cAddr-5), &F54QueryBase, 1);

					Read8BitRegisters(F54QueryBase, &RxChannelCount, 1);
					Read8BitRegisters((F54QueryBase+1), &TxChannelCount, 1);

					if(TRX_max<=RxChannelCount)
						RxChannelCount = TRX_max;
					if(TRX_max<=TxChannelCount)
						TxChannelCount = 16;

					MaxArrayLength = (int)RxChannelCount * (int)TxChannelCount * 2;
					Read8BitRegisters(F54QueryBase, Data, 24);

					TouchControllerFamily = Data[5];
					offset++; //       
					if (TouchControllerFamily == 0x0 || TouchControllerFamily == 0x01)
						offset++; //       
					offset+=2; //       
					bHavePixelTouchThresholdTuning = ((Data[6] & 0x01) == 0x01);
					if (bHavePixelTouchThresholdTuning) offset++; //        
					if (TouchControllerFamily == 0x0 || TouchControllerFamily == 0x01)
						offset+=3; //             
					if (TouchControllerFamily == 0x01){
						F54Ctrl07Offset = offset;
						offset++; //        
						bHaveF54Ctrl07 = true;
					}
					if (TouchControllerFamily == 0x0 || TouchControllerFamily == 0x01) offset+=2; //       
					if (TouchControllerFamily == 0x0 || TouchControllerFamily == 0x01) offset++; //       
					bHaveInterferenceMetric = ((Data[7] & 0x02) == 0x02);
					if (bHaveInterferenceMetric) offset++; //        
					bHaveCtrl11 = ((Data[7] & 0x10) == 0x10);
					if (bHaveCtrl11) offset+=2; //       
					bHaveRelaxationControl = ((Data[7] & 0x80) == 0x80);
					if (bHaveRelaxationControl) offset+=2; //          
					bHaveSensorAssignment = ((Data[7] & 0x01) == 0x01);
					if (bHaveSensorAssignment) offset++; //       
					if (bHaveSensorAssignment) offset+=RxChannelCount; //       
					if (bHaveSensorAssignment) offset+=TxChannelCount; //       
					bHaveSenseFrequencyControl = ((Data[7] & 0x04) == 0x04);
					NumberOfSensingFrequencies = (Data[13] & 0x0F);
					if (bHaveSenseFrequencyControl) offset+=(3*(int)NumberOfSensingFrequencies); //             
					offset++; //       
					if (bHaveSenseFrequencyControl) offset+=2; //       
					bHaveFirmwareNoiseMitigation = ((Data[7] & 0x08) == 0x08);
					if (bHaveFirmwareNoiseMitigation) offset++; //       
					if (bHaveFirmwareNoiseMitigation) offset+=2; //       
					if (bHaveFirmwareNoiseMitigation) offset+=2; //       
					if (bHaveFirmwareNoiseMitigation) offset++; //       
					if (bHaveFirmwareNoiseMitigation) offset++; //       
					bHaveIIRFilter = ((Data[9] & 0x02) == 0x02);
					if (bHaveIIRFilter) offset++; //       
					if (bHaveFirmwareNoiseMitigation) offset+=2; //       
					bHaveCmnRemoval = ((Data[9] & 0x04) == 0x04);
					bHaveCmnMaximum = ((Data[9] & 0x08) == 0x08);
					if (bHaveCmnRemoval) offset++; //       
					if (bHaveCmnMaximum) offset++; //       
					bHaveTouchHysteresis = ((Data[9] & 0x10) == 0x10);
					if (bHaveTouchHysteresis) offset++; //       
					bHaveEdgeCompensation = ((Data[9] & 0x20) == 0x20);
					if (bHaveEdgeCompensation) offset+=2; //       
					if (bHaveEdgeCompensation) offset+=2; //       
					if (bHaveEdgeCompensation) offset+=2; //       
					if (bHaveEdgeCompensation) offset+=2; //       
					CurveCompensationMode = (Data[8] & 0x03);
					if (CurveCompensationMode == 0x02) {
						offset += (int)RxChannelCount;
					} else if (CurveCompensationMode == 0x01) {
						offset += ((int)RxChannelCount > (int)TxChannelCount) ? (int)RxChannelCount: (int)TxChannelCount;
					} //       
					if (CurveCompensationMode == 0x02) offset += (int)TxChannelCount; //       
					bHavePerFrequencyNoiseControl = ((Data[9] & 0x40) == 0x40);
					if (bHavePerFrequencyNoiseControl) offset+=(3*(int)NumberOfSensingFrequencies); //             
					bHaveSignalClarity = ((Data[10] & 0x04) == 0x04);
					if (bHaveSignalClarity){
						F54Ctrl41Offset = offset;
						offset++; //       
						bHaveF54Ctrl41 = true;
					}
					bHaveMultiMetricStateMachine = ((Data[10] & 0x02) == 0x02);
					bHaveVarianceMetric = ((Data[10] & 0x08) == 0x08);
					if (bHaveVarianceMetric) offset+=2; //      
					if (bHaveMultiMetricStateMachine) offset+=2; //       
					if (bHaveMultiMetricStateMachine) offset+=11 ; //                                     
					bHave0DRelaxationControl = ((Data[10] & 0x10) == 0x10);
					bHave0DAcquisitionControl = ((Data[10] & 0x20) == 0x20);
					if (bHave0DRelaxationControl) offset+=2; //          
					if (bHave0DAcquisitionControl){
						F54Ctrl57Offset = offset;
						offset++; //        
						bHaveF54Ctrl57 = true;
					}
					if (bHave0DAcquisitionControl) offset += 1; //       
					bHaveSlewMetric = ((Data[10] & 0x80) == 0x80);
					bHaveHBlank = ((Data[11] & 0x01) == 0x01);
					bHaveVBlank = ((Data[11] & 0x02) == 0x02);
					bHaveLongHBlank = ((Data[11] & 0x04) == 0x04);
					bHaveNoiseMitigation2 = ((Data[11] & 0x20) == 0x20);
					bHaveSlewOption = ((Data[12] & 0x02) == 0x02);
					if (bHaveHBlank) offset += 1; //       
					if (bHaveHBlank || bHaveVBlank || bHaveLongHBlank) offset +=3; //             
					if (bHaveSlewMetric || bHaveHBlank || bHaveVBlank || bHaveLongHBlank || bHaveNoiseMitigation2 || bHaveSlewOption) offset += 1; //       
					if (bHaveHBlank) offset += 28; //                
					else if (bHaveVBlank || bHaveLongHBlank) offset += 4; //                
					if (bHaveHBlank || bHaveVBlank || bHaveLongHBlank) offset += 8; //                      
					if (bHaveSlewMetric) offset += 2; //       
					bHaveEnhancedStretch = ((Data[9] & 0x80) == 0x80);
					if (bHaveEnhancedStretch) offset += (int)NumberOfSensingFrequencies; //       
					bHaveStartupFastRelaxation = ((Data[11] & 0x08) == 0x08);
					if (bHaveStartupFastRelaxation) offset += 1; //       
					bHaveESDControl = ((Data[11] & 0x10) == 0x10);
					if (bHaveESDControl) offset += 2; //          
					if (bHaveNoiseMitigation2) offset += 5; //                   
					bHaveEnergyRatioRelaxation = ((Data[11] & 0x80) == 0x80);
					if (bHaveEnergyRatioRelaxation) offset += 2; //          
					bHaveF54Query13 = ((Data[12] & 0x08) == 0x08);
					if (bHaveSenseFrequencyControl){
						query_offset = 13;
						NumberOfSensingFrequencies = (Data[13] & 0x0F);
					}
					else
						query_offset = 12;
					if (bHaveF54Query13) query_offset++;
					bHaveCtrl86 = (bHaveF54Query13 && ((Data[13] & 0x01) == 0x01));
					bHaveCtrl87 = (bHaveF54Query13 && ((Data[13] & 0x02) == 0x02));
					bHaveCtrl88 = ((Data[12] & 0x40) == 0x40);
					if (bHaveCtrl86) offset += 1; //       
					if (bHaveCtrl87) offset += 1; //       
					if (bHaveCtrl88){
						F54Ctrl88Offset = offset;
						offset++; //        
					}
					bHaveCtrl89 = ((Data[query_offset] & 0x20) == 0x20);
					if (bHaveCtrl89) offset++;
					bHaveF54Query15 = ((Data[12] & 0x80) == 0x80);
					if (bHaveF54Query15) query_offset++;  //                 
					bHaveCtrl90 = (bHaveF54Query15 && ((Data[query_offset] & 0x01) == 0x01));
					if (bHaveCtrl90) offset++;
					bHaveF54Query16 = ((Data[query_offset] & 0x8) == 0x8);
					bHaveF54Query22 = ((Data[query_offset] & 0x40) == 0x40);
					bHaveF54Query25 = ((Data[query_offset] & 0x80) == 0x80);
					if (bHaveF54Query16) query_offset++; //                 
					bHaveF54Query17 = ((Data[query_offset] & 0x1) == 0x1);
					bHaveCtrl92 = ((Data[query_offset] & 0x4) == 0x4);
					bHaveCtrl93 = ((Data[query_offset] & 0x8) == 0x8);
					bHaveCtrl94 = ((Data[query_offset] & 0x10) == 0x10);
					bHaveF54Query18 = bHaveCtrl94;
					bHaveCtrl95 = ((Data[query_offset] & 0x20) == 0x20);
					bHaveF54Query19 = bHaveCtrl95;
					bHaveCtrl99 = ((Data[query_offset] & 0x40) == 0x40);
					bHaveCtrl100 = ((Data[query_offset] & 0x80) == 0x80);
					if (bHaveF54Query17) query_offset++; //                 
					if (bHaveF54Query18) query_offset++; //                 
					if (bHaveF54Query19) query_offset++; //                 
					query_offset = query_offset + 2 ; //                                
					bHaveCtrl91 = ((Data[query_offset] & 0x4) == 0x4);
					bHaveCtrl96  = ((Data[query_offset] & 0x8) == 0x8);
					bHaveCtrl97  = ((Data[query_offset] & 0x10) == 0x10);
					bHaveCtrl98  = ((Data[query_offset] & 0x20) == 0x20);
					if (bHaveF54Query22) query_offset++; //                 
					bHaveCtrl101 = ((Data[query_offset] & 0x2) == 0x2);
					bHaveF54Query23 = ((Data[query_offset] & 0x8) == 0x8);
					if (bHaveF54Query23) {
						query_offset++; //                 
						bHaveCtrl102 = ((Data[query_offset] & 0x01) == 0x01);
					}
					else
						bHaveCtrl102 = false;
					if (bHaveCtrl91) offset++;
					if (bHaveCtrl92) offset++;
					if (bHaveCtrl93) offset++;
					if (bHaveCtrl94) offset++;
					if (bHaveCtrl95) offset++;
					if (bHaveCtrl96) offset++;
					if (bHaveCtrl97) offset++;
					if (bHaveCtrl98)
					{
						F54Ctrl98Offset = offset;
						offset++;
					}
					if (bHaveCtrl99) offset++;
					if (bHaveCtrl100) offset++;
					if (bHaveCtrl101) offset++;
					break;
				}
			case 0x55:
				if (!bHaveF55){
					Read8BitRegisters((cAddr-3), &F55ControlBase, 1);
					Read8BitRegisters((cAddr-5), &F55QueryBase, 1);

					Read8BitRegisters(F55QueryBase, &RxChannelCount, 1);
					Read8BitRegisters((F55QueryBase+1), &TxChannelCount, 1);

					rxCount = 0;
					txCount = 0;
					//                   
					Read8BitRegisters((F55ControlBase+1), Data, (int)RxChannelCount);
					for (i = 0; i < (int)RxChannelCount; i++){
						if (Data[i] != 0xFF){
							rxCount++;
							TRxPhysical[i] = Data[i];
						}
						else
							break;
					}
					Read8BitRegisters((F55ControlBase+2), Data, (int)TxChannelCount);
					for (i = 0; i < (int)TxChannelCount; i++){
						if (Data[i] != 0xFF ){
							TRxPhysical[rxCount + i] = Data[i];
							txCount++;
						}
						else
							break;
					}
					for (i = (rxCount + txCount); i < (TRX_mapping_max); i++){
						TRxPhysical[i] = 0xFF;
					}

					RxChannelCount = rxCount;
					TxChannelCount = txCount;
					if(TRX_max<=RxChannelCount)
						RxChannelCount = TRX_max;
					if(TRX_max<=TxChannelCount)
						TxChannelCount = 16;

					MaxArrayLength = (int)RxChannelCount * (int)TxChannelCount * 2;
					if(((int)TxChannelCount - F12_2DTxCount == 0) && ButtonCount > 0){
						ButtonShared = true;
					}

					break;
				}
			default: //                   
				break;
		}
		cAddr -= 6;
	} while (true);
}

//                                                                                                     
//                                                                                                   
//                     
bool TestPreparation(void)
{
	unsigned char data = 0;
	unsigned char addr = 0;

	//                      
	//              

	//              
	if (bHaveF54Ctrl07) {
		addr = F54ControlBase + F54Ctrl07Offset;
		Read8BitRegisters(addr, &data, 1);
		//                    
		data = 0;
		Write8BitRegisters(addr, &data, 1);
	}
	else if (bHaveCtrl88){
		addr = F54ControlBase + F54Ctrl88Offset;
		Read8BitRegisters(addr, &data, 1);
		data = data & 0xDF;
		Write8BitRegisters(addr, &data, 1);
	}

	//                 
	if (bHaveF54Ctrl57){
		addr = F54ControlBase + F54Ctrl57Offset;
		Read8BitRegisters(addr, &data, 1);
		//                  
		data = 0;
		Write8BitRegisters(addr, &data, 1);
	}

	//                                                                               
	if (bHaveF54Ctrl41){
		addr = F54ControlBase + F54Ctrl41Offset;
		Read8BitRegisters(addr, &data, 1);
		data = data | 0x01;
		Write8BitRegisters(addr, &data, 1);
	}

	//                   
	Read8BitRegisters(F54CommandBase, &data, 1);
	data = data | 0x04;
	Write8BitRegisters(F54CommandBase, &data, 1);
	//              
	count = 0;
	do {
		Read8BitRegisters(F54CommandBase, &data, 1);
		msleep(1);
		count++;
	} while (data != 0x00 && (count < DefaultTimeout));

	if(count >= DefaultTimeout) {
		outbuf += sprintf(f54_wlog_buf+outbuf, "Timeout -- ForceUpdate can not complete\n");
		//                                                      
		Reset();
		return false;
	}

	//                
	Read8BitRegisters(F54CommandBase, &data, 1);
	data = data | 0x02;
	Write8BitRegisters(F54CommandBase, &data, 1);

	//              
	count = 0;
	do {
		Read8BitRegisters(F54CommandBase, &data, 1);
		msleep(1);
		count++;
	} while (data != 0x00 && (count < DefaultTimeout));

	if(count >= DefaultTimeout){
		outbuf += sprintf(f54_wlog_buf+outbuf, "Timeout -- ForceCal can not complete\n");
		//                                                   
		Reset();
		return false;
	}

	return true;
}

int diffnode(unsigned short *ImagepTest)
{

	int i = 0;
	int k = 0;
	unsigned char data;

	if (!bHaveF54) {
		LGTC_DBG("not bHaveF54\n");
		return -1;
	}
	if (!switchPage(0x01)) {
		LGTC_DBG("not switchPage(0x01)\n");
		return -1;
	}

	if (TestPreparation()){

		//                                                             
		//                                                             
		data = 20;//            
		Write8BitRegisters(F54DataBase, &data, 1);
		data = 0x01;
		Write8BitRegisters(F54CommandBase, &data, 1);
		count = 0;
		do {
			Read8BitRegisters(F54CommandBase, &data, 1);
			msleep(1);
			count++;
		} while (data != 0x00 && (count < DefaultTimeout));
		if(count >= DefaultTimeout) {
			LGTC_DBG("Timeout -- Not supported Report Type in FW\n");
			Reset();
			return -1;
		}

		Read8BitRegisters((F54DataBase+REPORT_DATA_OFFEST), &Data[0], MaxArrayLength);

		for (i = 0; i < (int)TxChannelCount * (int)RxChannelCount; i++) {
			ImagepTest[i] = ((short)Data[k] | (short)Data[k + 1] << 8);
			k = k + 2;
		}
		//            
		Reset();
		LGTC_DBG("diff_node success\n");
		return 0;

	} else {
		return -1;
	}

}

//                                                                                                             
int ImageTest(int mode, char *buf)
{
	unsigned char data;

	if (TestPreparation()){
//                                                               
//                                                               

		//                                      

		data = 20;//                    
		LGTC_DBG("[Touch][%s] raw capacitance mode!\n", __FUNCTION__);

		Write8BitRegisters(F54DataBase, &data, 1);

		do_gettimeofday(&t_interval[STARTTIME]);

		if(mode == 0)
			data = 'a';//                  
		else
			data = 'l';//                    

		return ReadReport(data, buf);

	} else {
		return -1;
	}
}

int DeltaTest(char *buf)
{
	unsigned char data;

	memcpy(LowerImageLimit, LowerImage, sizeof(LowerImageLimit));
	memcpy(UpperImageLimit, UpperImage, sizeof(UpperImageLimit));

	//                                      
	data = 0x02;//          
	LGTC_DBG("[Touch][%s] delta mode!\n", __FUNCTION__);

	Write8BitRegisters(F54DataBase, &data, 1);

	do_gettimeofday(&t_interval[STARTTIME]);

	data = 'l';//                    

	return ReadReport(data, buf);

}


int NoiseDeltaTest(char *buf)
{
	unsigned char data;

	memset(NoiseDeltaMin, 0, TRX_max * TRX_max * sizeof(short));
	memset(NoiseDeltaMax, 0, TRX_max * TRX_max * sizeof(short));

	LGTC_DBG("[Touch][%s] Noise Delta mode!\n", __FUNCTION__);

	//                                      
	data = 0x02;//          

	Write8BitRegisters(F54DataBase, &data, 1);

	data = 'm';//                    

	 return ReadReport(data, buf);

}

//                                                                                                                
int SensorSpeed(char *buf)
{
	unsigned char data;

	memcpy(SensorSpeedLowerImageLimit, SensorSpeedLowerImage, sizeof(SensorSpeedLowerImageLimit));
	memcpy(SensorSpeedUpperImageLimit, SensorSpeedUpperImage, sizeof(SensorSpeedUpperImageLimit));

	//                                         
	data = 22;
	Write8BitRegisters(F54DataBase, &data, 1);

	do_gettimeofday(&t_interval[STARTTIME]);

	data = 'c';
	return ReadReport(data, buf);

}

//                                                                                                        
int ADCRange(char *buf)
{
	unsigned char data = 0;

	memcpy(ADCLowerImageLimit, ADCLowerImage, sizeof(ADCLowerImageLimit));
	memcpy(ADCUpperImageLimit, ADCUpperImage, sizeof(ADCUpperImageLimit));

	Read8BitRegisters((F54ControlBase + F54Ctrl41Offset), &data, 1);
	if (data & 0x01)
		SignalClarityOn = false;
	else SignalClarityOn = true;

	//                                        
	data = 23;
	Write8BitRegisters(F54DataBase, &data, 1);

	do_gettimeofday(&t_interval[STARTTIME]);
	//                           

	data = 'b';
	return  ReadReport(data, buf);
}

void AbsADCRange(char *buf)
{
	unsigned char data;

	if (TestPreparation()){
		//                                                    
		data = 42;
		Write8BitRegisters(F54DataBase, &data, 1);

		do_gettimeofday(&t_interval[STARTTIME]);
		//                           

		data = 'i';
		ReadReport(data, buf);
	}
}
//               
int AbsDelta(char *buf)
{
	unsigned char data;

	//                   

	//                                                            
	data = 40;
	Write8BitRegisters(F54DataBase, &data, 1);

	do_gettimeofday(&t_interval[STARTTIME]);
	//                           

	data = 'j';
	return ReadReport(data, buf);
}
//               
int AbsRaw(int mode, char *buf)
{
	unsigned char data;

	//                 
	if (bHaveCtrl98) {
		Read8BitRegisters((F54ControlBase+F54Ctrl98Offset), &Data[0], 6);

		AbsRxShortLimit = AbsRawRef[Data[0]] * 275/100;//                   
		AbsTxShortLimit = AbsRawRef[Data[5]] * 275/100;//                   
		AbsTxOpenLimit =  AbsRawRef[Data[5]] * 75/100;//                   
	}

	data = 38;
	Write8BitRegisters(F54DataBase, &data, 1);

	do_gettimeofday(&t_interval[STARTTIME]);
	//                           

	if (mode == 1)
		data = 'n';//                           
	else if (mode == 2)
		data = 'o';//                          
	else
		data = 'k';//                         

	return ReadReport(data, buf);
}

//                                                                                                                    
void TRexOpenTest(char *buf)
{
	unsigned char data;

	//                                                                               
	//         

	if (TestPreparation()){
		//                                      
		data = 24;
		Write8BitRegisters(F54DataBase, &data, 1);

		data = 'd';
		ReadReport(data, buf);
	}
}

//                                                                                                                     
int TRexGroundTest(char *buf)
{
	unsigned char data;

	if (TestPreparation()){
		//                                        
		data = 25;
		Write8BitRegisters(F54DataBase, &data, 1);

		data = 'e';
		return ReadReport(data, buf);
	} else {
		return -1;
	}
}

//                                                                                                                         
int TRexShortTest(char *buf)
{
	unsigned char data;

	if (TestPreparation()){
		//                                       
		data = 26;
		Write8BitRegisters(F54DataBase, &data, 1);
		data = 'f';
		return ReadReport(data, buf);
	} else {
		return -1;
	}
}

//                                                                     
int HighResistanceTest(char *buf)
{
	unsigned char data;

	if (TestPreparation()){

		//                                              
		data = 4;
		Write8BitRegisters(F54DataBase, &data, 1);

		data = 'g';
		return ReadReport(data, buf);
	} else {
		return -1;
	}
}

//                                                                                
void MaxMinTest(char *buf)
{
	unsigned char data;

	if (TestPreparation()){
		//                                      
		data = 13;
		Write8BitRegisters(F54DataBase, &data, 1);

		data = 'h';
		ReadReport(data, buf);
	}
}

void CheckCrash(char *rst, int min_caps_value)
{
	int i = 0;
	int j = 0;
	int k = 0;
	int node_crack_count = 0;
	int rx_crack_count = 0;
	int row_crack_count = 0;
	unsigned char cmd;

	if (TestPreparation() == false){
		LGTC_DBG("TestPreparation failed\n");
		goto error;
	}

	cmd = 20;
	DO_SAFE(Write8BitRegisters(F54DataBase, &cmd, 1), error);

	cmd = 0x01;
	DO_SAFE(Write8BitRegisters(F54CommandBase, &cmd, 1), error);

	count = 0;
	do {
		DO_SAFE(Read8BitRegisters(F54CommandBase, &cmd, 1), error);
		msleep(1);
		count++;
	} while (cmd != 0x00 && (count < DefaultTimeout));

	DO_SAFE(Read8BitRegisters((F54DataBase+REPORT_DATA_OFFEST), Data, MaxArrayLength), error);

	for (i = 0; i < (int)TxChannelCount; i++) {
		for (j = 0; j < (int)RxChannelCount; j++) {
			ImagepF[i][j] = ((short)Data[k] | (short)Data[k+1] << 8);

			if ((ImagepF[i][j] < LowerImageLimit[i][j]) || (ImagepF[i][j] > UpperImageLimit[i][j])) {
				if (ImagepF[i][j] < min_caps_value) {
					rx_crack_count++;
					node_crack_count++;
				}
				else {
					row_crack_count = 0;
				}

				if (F12_2DRxCount <= rx_crack_count)
					row_crack_count++;

				if (5 < row_crack_count || (F12_2DTxCount*F12_2DRxCount*40/100) < node_crack_count) {
					sprintf(rst, "%d", 1);
					return;
				}
			}

			k += 2;
		}

		rx_crack_count = 0;
	}

	sprintf(rst, "%d", 0);
	LGTC_DBG("Tx [%d] Rx [%d] node_crack_count %d, row_crack_count %d\n",
						i,
						j,
						node_crack_count,
						row_crack_count);

	return;
error:
	sprintf(rst, "%d", 0);
	LGTC_ERR("winodw crack check fail\n");
}

void SCAN_PDT(struct i2c_client *client)
{
	int i;

	ds4_i2c_client = client;

	for (i = 0; i < scanMaxPageCount ;i++)
	{
		if (switchPage(i))
			RunQueries();
	}
}

//                                     
int F54Test(int input, int mode, char *buf)
{
	int ret = 0;
	unsigned char data;

	if (!switchPage(0x01))
		return false;

	data = 0x00;
	Write8BitRegisters(F54DataBase+1, &data, 1);
	Write8BitRegisters(F54DataBase+2, &data, 1);

	outbuf = 0;
	out_buf = 0;
	memset(f54_wlog_buf, 0, sizeof(f54_wlog_buf));
	memset(wlog_buf, 0, sizeof(wlog_buf));

	switch(input){
		case 'a':
			outbuf = sprintf(f54_wlog_buf, "a - Full Raw Capacitance Test\n");
			ret = ImageTest(mode, buf);
			break;
		case 'b':
			out_buf = sprintf(wlog_buf, "b - ADC Range Test\n");
			ret = ADCRange(buf);
			break;
		case 'c':
			out_buf = sprintf(wlog_buf, "c - Sensor Speed Test\n");
			ret = SensorSpeed(buf);
			break;
		case 'd':
			outbuf = sprintf(f54_wlog_buf, "d - TRex Open Test\n");
			TRexOpenTest(buf);
			break;
		case 'e':
			outbuf = sprintf(f54_wlog_buf, "e - TRex Gnd Test\n");
			ret = TRexGroundTest(buf);
			break;
		case 'f':
			outbuf = sprintf(f54_wlog_buf, "f - TRex Short Test\n");
			ret = TRexShortTest(buf);
			break;
		case 'g':
			outbuf = sprintf(f54_wlog_buf, "g - High Resistance Test\n");
			ret = HighResistanceTest(buf);
			break;
		case 'h':
			outbuf = sprintf(f54_wlog_buf, "h - Full Raw Capacitance Max/Min Test\n");
			MaxMinTest(buf);
			break;
		case 'i':
			outbuf = sprintf(f54_wlog_buf, "i - Abs Sensing ADC Range Test\n");
			AbsADCRange(buf);
			break;
		case 'j':
			outbuf = sprintf(f54_wlog_buf, "j - Abs Sensing Delta Capacitance\n");
			ret = AbsDelta(buf);
			break;
		case 'k':
			outbuf = sprintf(f54_wlog_buf, "k - Abs Sensing Raw Capcitance Test\n");
			AbsRaw(mode, buf);
			break;
		case 'l':
			CheckCrash(buf, mode);
			break;
		case 'm':
			ret = DeltaTest(buf);
			break;
		case 'n':
			outbuf = sprintf(f54_wlog_buf, "n - Abs Sensing Raw Short Capcitance Test\n");
			ret = AbsRaw(mode,buf);
			break;
		case 'o':
			outbuf = sprintf(f54_wlog_buf, "k - Abs Sensing Raw Open Capcitance Test\n");
			ret = AbsRaw(mode,buf);
			break;
		case 'x':
			out_buf = sprintf(wlog_buf, "x - Noise Delta Test\n");
			ret = NoiseDeltaTest(buf);
			break;
		case 'z':
			LGTC_DBG("Version: %s\n", VERSION);
			break;
		default:
			return -1;
			//                 
			//        
	}
	return ret;
}
