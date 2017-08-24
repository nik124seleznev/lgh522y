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
//             
//             
#include <linux/videodev2.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/xlog.h>
#include <asm/atomic.h>
#include <asm/io.h>
#include <asm/system.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/dma-mapping.h>

#include "kd_camera_hw.h"
#include "kd_imgsensor.h"
#include "kd_imgsensor_define.h"
#include "kd_imgsensor_errcode.h"
#include "kd_camera_feature.h"


#include "s5k8aayxmipi_Sensor.h"
#include "s5k8aayxmipi_Camera_Sensor_para.h"
#include "s5k8aayxmipi_CameraCustomized.h"

#define S5K8AAYX_MIPI_DEBUG
#ifdef S5K8AAYX_MIPI_DEBUG
#define SENSORDB(fmt, arg...) xlog_printk(ANDROID_LOG_DEBUG, "[S5K8AAYXMIPI]", fmt, ##arg)

#else
#define SENSORDB(x,...)
#endif

#define S5K8AAYX_TEST_PATTERN_CHECKSUM (0x7b7a669f)

typedef struct
{
  UINT16  iSensorVersion;
  UINT16  iNightMode;
  UINT16  iWB;
  UINT16  iEffect;
  UINT16  iEV;
  UINT16  iBanding;
  UINT16  iMirror;
  UINT16  iFrameRate;
} S5K8AAYX_MIPIStatus;
S5K8AAYX_MIPIStatus S5K8AAYX_MIPICurrentStatus;

static DEFINE_SPINLOCK(s5k8aayxmipi_drv_lock);

extern int iReadRegI2C(u8 *a_pSendData , u16 a_sizeSendData, u8 * a_pRecvData, u16 a_sizeRecvData, u16 i2cId);
extern int iWriteRegI2C(u8 *a_pSendData , u16 a_sizeSendData, u16 i2cId);
extern int iMultiWriteReg(u8 *pData, u16 lens, u16 i2cId);
#define S5K8AAYX_MIPI_multi_write_cmos_sensor(pData, lens) iMultiWriteReg((u8*) pData, (u16) lens, S5K8AAYX_MIPI_WRITE_ID)


//                              
static kal_uint32 zoom_factor = 0;


kal_uint16 S5K8AAYX_MIPI_read_cmos_sensor(kal_uint32 addr)
{
	kal_uint16 get_byte=0;
	char puSendCmd[2] = {(char)(addr >> 8) , (char)(addr & 0xFF) };
	iReadRegI2C(puSendCmd , 2, (u8*)&get_byte,2,S5K8AAYX_MIPI_WRITE_ID);
	return (kal_uint16)(((get_byte<<8)&0xff00)|((get_byte>>8)&0x00ff));

}

inline void S5K8AAYX_MIPI_write_cmos_sensor(u16 addr, u32 para)
{
   char puSendCmd[4] = {(char)(addr >> 8) , (char)(addr & 0xFF) ,(char)(para >> 8),(char)(para & 0xFF)};

   iWriteRegI2C(puSendCmd , 4,S5K8AAYX_MIPI_WRITE_ID);
}


void S5K8AAYX_MIPI_write_init_setting(kal_uint8* DataArray,int ArrayCnt)
{

	int totalCnt = 0, len = 0;
	int transfer_len, transac_len=4;
	kal_uint8* pBuf=NULL;
	dma_addr_t dmaHandle;

	pBuf = (kal_uint8*)kmalloc(1024, GFP_KERNEL);

	//                                 
	totalCnt = ArrayCnt;

	transfer_len = totalCnt / transac_len;
	len = (transfer_len<<8)|transac_len;
	//                                                         

	memcpy(pBuf, DataArray, totalCnt );
	dmaHandle = dma_map_single(NULL, pBuf, 1024, DMA_TO_DEVICE);

	S5K8AAYX_MIPI_multi_write_cmos_sensor(dmaHandle, len);

	dma_unmap_single(NULL, dmaHandle, 1024, DMA_TO_DEVICE);
}



/*                                                                              
                              
                                                                               */
#define WINMO_USE 0

#define Sleep(ms) mdelay(ms)
#define RETAILMSG(x,...)
#define TEXT


/*                                                                              
                                  
                                                                               */
/*                 */
kal_bool S5K8AAYX_MIPI_MPEG4_encode_mode = KAL_FALSE, S5K8AAYX_MIPI_MJPEG_encode_mode = KAL_FALSE;
static kal_bool S5K8AAYX_MIPI_VEDIO_encode_mode = KAL_FALSE;
static kal_bool S5K8AAYX_MIPI_sensor_cap_state = KAL_FALSE;
kal_uint32 S5K8AAYX_MIPI_PV_dummy_pixels=0,S5K8AAYX_MIPI_PV_dummy_lines=0,S5K8AAYX_MIPI_isp_master_clock=0;
static kal_uint32  S5K8AAYX_MIPI_sensor_pclk=920;
static kal_bool S5K8AAYX_MIPI_AE_ENABLE = KAL_TRUE;

MSDK_SENSOR_CONFIG_STRUCT S5K8AAYX_MIPISensorConfigData;


static kal_uint8 s5k8aayx_init_1[] = {
	0x00,0x2A,0x12,0x78,
	0x0F,0x12,0xAA,0xF0,
	0x00,0x28,0x70,0x00,
	0x00,0x2A,0x24,0x70,
	0x0F,0x12,0xB5,0x10,
	0x0F,0x12,0x49,0x0D,
	0x0F,0x12,0x48,0x0D,
	0x0F,0x12,0xF0,0x00,
	0x0F,0x12,0xF9,0x6D,
	0x0F,0x12,0x49,0x0D,
	0x0F,0x12,0x48,0x0D,
	0x0F,0x12,0xF0,0x00,
	0x0F,0x12,0xF9,0x69,
	0x0F,0x12,0x49,0x0D,
	0x0F,0x12,0x48,0x0D,
	0x0F,0x12,0x63,0x41,
	0x0F,0x12,0x49,0x0D,
	0x0F,0x12,0x48,0x0E,
	0x0F,0x12,0xF0,0x00,
	0x0F,0x12,0xF9,0x62,
	0x0F,0x12,0x49,0x0D,
	0x0F,0x12,0x48,0x0E,
	0x0F,0x12,0xF0,0x00,
	0x0F,0x12,0xF9,0x5E,
	0x0F,0x12,0x49,0x0D,
	0x0F,0x12,0x48,0x0E,
	0x0F,0x12,0xF0,0x00,
	0x0F,0x12,0xF9,0x5A,
	0x0F,0x12,0xBC,0x10,
	0x0F,0x12,0xBC,0x08,
	0x0F,0x12,0x47,0x18,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x26,0xD4,
	0x0F,0x12,0x70,0x00,
	0x0F,0x12,0x8E,0xDD,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x26,0x4C,
	0x0F,0x12,0x70,0x00,
	0x0F,0x12,0x87,0x25,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x25,0xEC,
	0x0F,0x12,0x70,0x00,
	0x0F,0x12,0x00,0x80,
	0x0F,0x12,0x70,0x00,
	0x0F,0x12,0x25,0x40,
	0x0F,0x12,0x70,0x00,
	0x0F,0x12,0xA6,0xEF,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x25,0x0C,
	0x0F,0x12,0x70,0x00,
	0x0F,0x12,0xA0,0xF1,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x24,0xD8,
	0x0F,0x12,0x70,0x00,
	0x0F,0x12,0x05,0x8F,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x40,0x10,
	0x0F,0x12,0xE9,0x2D,
	0x0F,0x12,0x00,0xA0,
	0x0F,0x12,0xEB,0x00,
	0x0F,0x12,0x02,0x3C,
	0x0F,0x12,0xE5,0x9F,
	0x0F,0x12,0x00,0xB2,
	0x0F,0x12,0xE1,0xD0,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0xE3,0x50,
	0x0F,0x12,0x00,0x04,
	0x0F,0x12,0x0A,0x00,
	0x0F,0x12,0x00,0x80,
	0x0F,0x12,0xE3,0x10,
	0x0F,0x12,0x00,0x02,
	0x0F,0x12,0x1A,0x00,
	0x0F,0x12,0x12,0x28,
	0x0F,0x12,0xE5,0x9F,
	0x0F,0x12,0x00,0x01,
	0x0F,0x12,0xE3,0xA0,
	0x0F,0x12,0x0D,0xB2,
	0x0F,0x12,0xE1,0xC1,
	0x0F,0x12,0x40,0x10,
	0x0F,0x12,0xE8,0xBD,
	0x0F,0x12,0xFF,0x1E,
	0x0F,0x12,0xE1,0x2F,
	0x0F,0x12,0x40,0x10,
	0x0F,0x12,0xE9,0x2D,
	0x0F,0x12,0x40,0x00,
	0x0F,0x12,0xE5,0x90,
	0x0F,0x12,0x00,0x04,
	0x0F,0x12,0xE1,0xA0,
	0x0F,0x12,0x00,0x94,
	0x0F,0x12,0xEB,0x00,
	0x0F,0x12,0x02,0x08,
	0x0F,0x12,0xE5,0x9F,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0xE5,0xD0,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0xE3,0x50,
	0x0F,0x12,0x00,0x02,
	0x0F,0x12,0x0A,0x00,
	0x0F,0x12,0x00,0x04,
	0x0F,0x12,0xE5,0x94,
	0x0F,0x12,0x00,0xA0,
	0x0F,0x12,0xE1,0xA0,
	0x0F,0x12,0x00,0x04,
	0x0F,0x12,0xE5,0x84,
	0x0F,0x12,0x40,0x10,
	0x0F,0x12,0xE8,0xBD,
	0x0F,0x12,0xFF,0x1E,
	0x0F,0x12,0xE1,0x2F,
	0x0F,0x12,0x40,0x70,
	0x0F,0x12,0xE9,0x2D,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0xE5,0x90,
	0x0F,0x12,0x08,0x00,
	0x0F,0x12,0xE1,0xA0,
	0x0F,0x12,0x08,0x20,
	0x0F,0x12,0xE1,0xA0,
	0x0F,0x12,0x40,0x41,
	0x0F,0x12,0xE2,0x80,
	0x0F,0x12,0x01,0xD4,
	0x0F,0x12,0xE5,0x9F,
	0x0F,0x12,0x11,0xB8,
	0x0F,0x12,0xE1,0xD0,
	0x0F,0x12,0x51,0xB6,
	0x0F,0x12,0xE1,0xD0,
	0x0F,0x12,0x00,0x05,
	0x0F,0x12,0xE0,0x41,
	0x0F,0x12,0x00,0x94,
	0x0F,0x12,0xE0,0x00,
	0x0F,0x12,0x1D,0x11,
	0x0F,0x12,0xE3,0xA0,
	0x0F,0x12,0x00,0x82,
	0x0F,0x12,0xEB,0x00,
	0x0F,0x12,0x11,0xB4,
	0x0F,0x12,0xE5,0x9F,
	0x0F,0x12,0x10,0x00,
	0x0F,0x12,0xE5,0xD1,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0xE3,0x51,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x0A,0x00,
	0x0F,0x12,0x00,0xA0,
	0x0F,0x12,0xE1,0xA0,
	0x0F,0x12,0x21,0x9C,
	0x0F,0x12,0xE5,0x9F,
	0x0F,0x12,0x3F,0xB0,
	0x0F,0x12,0xE1,0xD2,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0xE3,0x53,
	0x0F,0x12,0x00,0x03,
	0x0F,0x12,0x0A,0x00,
	0x0F,0x12,0x31,0x98,
	0x0F,0x12,0xE5,0x9F,
	0x0F,0x12,0x5B,0xB2,
	0x0F,0x12,0xE1,0xC3,
	0x0F,0x12,0xC0,0x00,
	0x0F,0x12,0xE0,0x85,
	0x0F,0x12,0xCB,0xB4,
	0x0F,0x12,0xE1,0xC3,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0xE3,0x51,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x0A,0x00,
	0x0F,0x12,0x00,0x80,
	0x0F,0x12,0xE1,0xA0,
	0x0F,0x12,0x1D,0xBC,
	0x0F,0x12,0xE1,0xD2,
	0x0F,0x12,0x3E,0xB4,
	0x0F,0x12,0xE1,0xD2,
	0x0F,0x12,0x2E,0xB2,
	0x0F,0x12,0xE1,0xD2,
	0x0F,0x12,0x01,0x93,
	0x0F,0x12,0xE0,0x01,
	0x0F,0x12,0x00,0x92,
	0x0F,0x12,0xE0,0x00,
	0x0F,0x12,0x28,0x11,
	0x0F,0x12,0xE3,0xA0,
	0x0F,0x12,0x01,0x94,
	0x0F,0x12,0xE0,0x01,
	0x0F,0x12,0x00,0x92,
	0x0F,0x12,0xE0,0x00,
	0x0F,0x12,0x11,0xA1,
	0x0F,0x12,0xE1,0xA0,
	0x0F,0x12,0x01,0xA0,
	0x0F,0x12,0xE1,0xA0,
	0x0F,0x12,0x00,0x67,
	0x0F,0x12,0xEB,0x00,
	0x0F,0x12,0x11,0x54,
	0x0F,0x12,0xE5,0x9F,
	0x0F,0x12,0x02,0xB4,
	0x0F,0x12,0xE1,0xC1,
	0x0F,0x12,0x40,0x70,
	0x0F,0x12,0xE8,0xBD,
	0x0F,0x12,0xFF,0x1E,
	0x0F,0x12,0xE1,0x2F,
	0x0F,0x12,0x40,0x10,
	0x0F,0x12,0xE9,0x2D,
	0x0F,0x12,0x00,0x63,
	0x0F,0x12,0xEB,0x00,
	0x0F,0x12,0x21,0x3C};


static kal_uint8 s5k8aayx_init_2[] = {
	0x0F,0x12,0xE5,0x9F,
	0x0F,0x12,0x14,0xB0,
	0x0F,0x12,0xE1,0xD2,
	0x0F,0x12,0x00,0x80,
	0x0F,0x12,0xE3,0x11,
	0x0F,0x12,0x00,0x05,
	0x0F,0x12,0x0A,0x00,
	0x0F,0x12,0x01,0x30,
	0x0F,0x12,0xE5,0x9F,
	0x0F,0x12,0x00,0xB0,
	0x0F,0x12,0xE1,0xD0,
	0x0F,0x12,0x00,0x01,
	0x0F,0x12,0xE3,0x50,
	0x0F,0x12,0x00,0x01,
	0x0F,0x12,0x9A,0x00,
	0x0F,0x12,0x00,0x01,
	0x0F,0x12,0xE3,0xA0,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0xEA,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0xE3,0xA0,
	0x0F,0x12,0x31,0x04,
	0x0F,0x12,0xE5,0x9F,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0xE5,0xC3,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0xE5,0xD3,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0xE3,0x50,
	0x0F,0x12,0x00,0x03,
	0x0F,0x12,0x0A,0x00,
	0x0F,0x12,0x00,0x80,
	0x0F,0x12,0xE3,0xC1,
	0x0F,0x12,0x11,0x00,
	0x0F,0x12,0xE5,0x9F,
	0x0F,0x12,0x04,0xB0,
	0x0F,0x12,0xE1,0xC2,
	0x0F,0x12,0x00,0xB2,
	0x0F,0x12,0xE1,0xC1,
	0x0F,0x12,0x40,0x10,
	0x0F,0x12,0xE8,0xBD,
	0x0F,0x12,0xFF,0x1E,
	0x0F,0x12,0xE1,0x2F,
	0x0F,0x12,0x41,0xF0,
	0x0F,0x12,0xE9,0x2D,
	0x0F,0x12,0x10,0x00,
	0x0F,0x12,0xE5,0x90,
	0x0F,0x12,0xC8,0x01,
	0x0F,0x12,0xE1,0xA0,
	0x0F,0x12,0xC8,0x2C,
	0x0F,0x12,0xE1,0xA0,
	0x0F,0x12,0x10,0x04,
	0x0F,0x12,0xE5,0x90,
	0x0F,0x12,0x18,0x01,
	0x0F,0x12,0xE1,0xA0,
	0x0F,0x12,0x18,0x21,
	0x0F,0x12,0xE1,0xA0,
	0x0F,0x12,0x40,0x08,
	0x0F,0x12,0xE5,0x90,
	0x0F,0x12,0x50,0x0C,
	0x0F,0x12,0xE5,0x90,
	0x0F,0x12,0x20,0x04,
	0x0F,0x12,0xE1,0xA0,
	0x0F,0x12,0x30,0x05,
	0x0F,0x12,0xE1,0xA0,
	0x0F,0x12,0x00,0x0C,
	0x0F,0x12,0xE1,0xA0,
	0x0F,0x12,0x00,0x43,
	0x0F,0x12,0xEB,0x00,
	0x0F,0x12,0x60,0xBC,
	0x0F,0x12,0xE5,0x9F,
	0x0F,0x12,0x00,0xB2,
	0x0F,0x12,0xE1,0xD6,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0xE3,0x50,
	0x0F,0x12,0x00,0x0E,
	0x0F,0x12,0x0A,0x00,
	0x0F,0x12,0x00,0xB0,
	0x0F,0x12,0xE5,0x9F,
	0x0F,0x12,0x05,0xB4,
	0x0F,0x12,0xE1,0xD0,
	0x0F,0x12,0x00,0x02,
	0x0F,0x12,0xE3,0x50,
	0x0F,0x12,0x00,0x0A,
	0x0F,0x12,0x1A,0x00,
	0x0F,0x12,0x70,0xA4,
	0x0F,0x12,0xE5,0x9F,
	0x0F,0x12,0x10,0xF4,
	0x0F,0x12,0xE1,0xD6,
	0x0F,0x12,0x26,0xB0,
	0x0F,0x12,0xE1,0xD7,
	0x0F,0x12,0x00,0xF0,
	0x0F,0x12,0xE1,0xD4,
	0x0F,0x12,0x00,0x39,
	0x0F,0x12,0xEB,0x00,
	0x0F,0x12,0x00,0xB0,
	0x0F,0x12,0xE1,0xC4,
	0x0F,0x12,0x26,0xB0,
	0x0F,0x12,0xE1,0xD7,
	0x0F,0x12,0x10,0xF6,
	0x0F,0x12,0xE1,0xD6,
	0x0F,0x12,0x00,0xF0,
	0x0F,0x12,0xE1,0xD5,
	0x0F,0x12,0x00,0x34,
	0x0F,0x12,0xEB,0x00,
	0x0F,0x12,0x00,0xB0,
	0x0F,0x12,0xE1,0xC5,
	0x0F,0x12,0x41,0xF0,
	0x0F,0x12,0xE8,0xBD,
	0x0F,0x12,0xFF,0x1E,
	0x0F,0x12,0xE1,0x2F,
	0x0F,0x12,0x40,0x10,
	0x0F,0x12,0xE9,0x2D,
	0x0F,0x12,0x40,0x00,
	0x0F,0x12,0xE1,0xA0,
	0x0F,0x12,0x10,0x04,
	0x0F,0x12,0xE5,0x94,
	0x0F,0x12,0x00,0x5C,
	0x0F,0x12,0xE5,0x9F,
	0x0F,0x12,0x00,0xB0,
	0x0F,0x12,0xE1,0xD0,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0xE3,0x50,
	0x0F,0x12,0x00,0x08,
	0x0F,0x12,0x0A,0x00,
	0x0F,0x12,0x00,0x54,
	0x0F,0x12,0xE5,0x9F,
	0x0F,0x12,0x30,0x01,
	0x0F,0x12,0xE1,0xA0,
	0x0F,0x12,0x20,0x68,
	0x0F,0x12,0xE5,0x90,
	0x0F,0x12,0x00,0x4C,
	0x0F,0x12,0xE5,0x9F,
	0x0F,0x12,0x10,0x05,
	0x0F,0x12,0xE3,0xA0,
	0x0F,0x12,0x00,0x27,
	0x0F,0x12,0xEB,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0xE5,0x84,
	0x0F,0x12,0x40,0x10,
	0x0F,0x12,0xE8,0xBD,
	0x0F,0x12,0xFF,0x1E,
	0x0F,0x12,0xE1,0x2F,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0xE5,0x94,
	0x0F,0x12,0x00,0x25,
	0x0F,0x12,0xEB,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0xE5,0x84,
	0x0F,0x12,0xFF,0xF9,
	0x0F,0x12,0xEA,0xFF,
	0x0F,0x12,0x17,0x28,
	0x0F,0x12,0x70,0x00,
	0x0F,0x12,0x11,0x2C,
	0x0F,0x12,0x70,0x00,
	0x0F,0x12,0x27,0xC4,
	0x0F,0x12,0x70,0x00,
	0x0F,0x12,0x12,0x2C,
	0x0F,0x12,0x70,0x00,
	0x0F,0x12,0xF2,0x00,
	0x0F,0x12,0xD0,0x00,
	0x0F,0x12,0x23,0x40,
	0x0F,0x12,0x70,0x00,
	0x0F,0x12,0x0E,0x2C,
	0x0F,0x12,0x70,0x00,
	0x0F,0x12,0xF4,0x00,
	0x0F,0x12,0xD0,0x00,
	0x0F,0x12,0x33,0x70,
	0x0F,0x12,0x70,0x00,
	0x0F,0x12,0x0C,0xDC,
	0x0F,0x12,0x70,0x00,
	0x0F,0x12,0x20,0xD4,
	0x0F,0x12,0x70,0x00,
	0x0F,0x12,0x06,0xD4,
	0x0F,0x12,0x70,0x00,
	0x0F,0x12,0x47,0x78,
	0x0F,0x12,0x46,0xC0,
	0x0F,0x12,0xC0,0x00,
	0x0F,0x12,0xE5,0x9F,
	0x0F,0x12,0xFF,0x1C,
	0x0F,0x12,0xE1,0x2F,
	0x0F,0x12,0xC0,0x91,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0xC0,0x00,
	0x0F,0x12,0xE5,0x9F,
	0x0F,0x12,0xFF,0x1C,
	0x0F,0x12,0xE1,0x2F,
	0x0F,0x12,0x05,0x8F,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0xC0,0x00,
	0x0F,0x12,0xE5,0x9F,
	0x0F,0x12,0xFF,0x1C,
	0x0F,0x12,0xE1,0x2F,
	0x0F,0x12,0xA0,0xF1,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0xF0,0x04,
	0x0F,0x12,0xE5,0x1F,
	0x0F,0x12,0xD1,0x4C,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0xC0,0x00};

	static kal_uint8 s5k8aayx_init_3[] = {
	0x0F,0x12,0xE5,0x9F,
	0x0F,0x12,0xFF,0x1C,
	0x0F,0x12,0xE1,0x2F,
	0x0F,0x12,0x2B,0x43,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0xC0,0x00,
	0x0F,0x12,0xE5,0x9F,
	0x0F,0x12,0xFF,0x1C,
	0x0F,0x12,0xE1,0x2F,
	0x0F,0x12,0x87,0x25,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0xC0,0x00,
	0x0F,0x12,0xE5,0x9F,
	0x0F,0x12,0xFF,0x1C,
	0x0F,0x12,0xE1,0x2F,
	0x0F,0x12,0x67,0x77,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0xC0,0x00,
	0x0F,0x12,0xE5,0x9F,
	0x0F,0x12,0xFF,0x1C,
	0x0F,0x12,0xE1,0x2F,
	0x0F,0x12,0x8E,0x49,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0xC0,0x00,
	0x0F,0x12,0xE5,0x9F,
	0x0F,0x12,0xFF,0x1C,
	0x0F,0x12,0xE1,0x2F,
	0x0F,0x12,0x8E,0xDD,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x90,0xC8,
	0x0F,0x12,0x00,0x00,
	0x00,0x2A,0x13,0x26,
	0x0F,0x12,0x00,0x00,
	0x00,0x2A,0x06,0x3A,
	0x0F,0x12,0x01,0x00,
	0x0F,0x12,0x01,0x00,
	0x0F,0x12,0x01,0x00,
	0x0F,0x12,0x01,0x00,
	0x0F,0x12,0x01,0x00,
	0x0F,0x12,0x01,0x00,
	0x0F,0x12,0x01,0x00,
	0x0F,0x12,0x01,0x00,
	0x0F,0x12,0x01,0x00,
	0x0F,0x12,0x01,0x00,
	0x0F,0x12,0x01,0x00,
	0x0F,0x12,0x01,0x00,
	0x0F,0x12,0x00,0xB8,
	0x0F,0x12,0x01,0x00,
	0x0F,0x12,0x01,0x00,
	0x0F,0x12,0x01,0x00,
	0x0F,0x12,0x00,0x98,
	0x0F,0x12,0x00,0xF8,
	0x0F,0x12,0x00,0xF8,
	0x0F,0x12,0x01,0x00,
	0x0F,0x12,0x00,0xA0,
	0x0F,0x12,0x01,0x00,
	0x0F,0x12,0x01,0x00,
	0x0F,0x12,0x01,0x00,
	0x0F,0x12,0x00,0xA0,
	0x0F,0x12,0x01,0x00,
	0x0F,0x12,0x01,0x00,
	0x0F,0x12,0x01,0x00,
	0x00,0x2A,0x06,0x7A,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x00,0x2A,0x06,0xBA,
	0x0F,0x12,0x00,0x01,
	0x00,0x2A,0x06,0x32,
	0x0F,0x12,0x01,0x00,
	0x0F,0x12,0x01,0x00,
	0x0F,0x12,0x01,0x00,
	0x0F,0x12,0x01,0x00,
	0x00,0x2A,0x06,0x72,
	0x0F,0x12,0x01,0x00,
	0x0F,0x12,0x01,0x00,
	0x0F,0x12,0x01,0x00,
	0x0F,0x12,0x01,0x00,
	0x00,0x2A,0x06,0xB2,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x00,0x2A,0x06,0xD0,
	0x0F,0x12,0x00,0x0D,
	0x0F,0x12,0x00,0x0F,
	0x00,0x2A,0x06,0xCC,
	0x0F,0x12,0x02,0x80,
	0x0F,0x12,0x01,0xE0,
	0x00,0x2A,0x06,0xC6,
	0x0F,0x12,0x00,0x01,
	0x00,0x2A,0x06,0x24,
	0x0F,0x12,0x00,0x9D,
	0x0F,0x12,0x00,0xD5,
	0x0F,0x12,0x01,0x03,
	0x0F,0x12,0x01,0x28,
	0x0F,0x12,0x01,0x66,
	0x0F,0x12,0x01,0x93,
	0x0F,0x12,0x01,0xA0,
	0x00,0x2A,0x34,0x7C,
	0x0F,0x12,0x01,0x3B,
	0x0F,0x12,0x01,0x16,
	0x0F,0x12,0x00,0xD9,
	0x0F,0x12,0x00,0xA6,
	0x0F,0x12,0x00,0x82,
	0x0F,0x12,0x00,0x6C,
	0x0F,0x12,0x00,0x65,
	0x0F,0x12,0x00,0x6C,
	0x0F,0x12,0x00,0x80,
	0x0F,0x12,0x00,0xA3,
	0x0F,0x12,0x00,0xD4,
	0x0F,0x12,0x01,0x0D,
	0x0F,0x12,0x01,0x2E,
	0x0F,0x12,0x01,0x38,
	0x0F,0x12,0x01,0x04,
	0x0F,0x12,0x00,0xBE,
	0x0F,0x12,0x00,0x88,
	0x0F,0x12,0x00,0x62,
	0x0F,0x12,0x00,0x4D,
	0x0F,0x12,0x00,0x46,
	0x0F,0x12,0x00,0x4C,
	0x0F,0x12,0x00,0x60,
	0x0F,0x12,0x00,0x84,
	0x0F,0x12,0x00,0xB8,
	0x0F,0x12,0x00,0xF9,
	0x0F,0x12,0x01,0x2C,
	0x0F,0x12,0x01,0x1A,
	0x0F,0x12,0x00,0xDB,
	0x0F,0x12,0x00,0x93,
	0x0F,0x12,0x00,0x5F,
	0x0F,0x12,0x00,0x3C,
	0x0F,0x12,0x00,0x27,
	0x0F,0x12,0x00,0x20,
	0x0F,0x12,0x00,0x26,
	0x0F,0x12,0x00,0x3A,
	0x0F,0x12,0x00,0x5C,
	0x0F,0x12,0x00,0x8E,
	0x0F,0x12,0x00,0xD2,
	0x0F,0x12,0x01,0x0E,
	0x0F,0x12,0x01,0x01,
	0x0F,0x12,0x00,0xBF,
	0x0F,0x12,0x00,0x77,
	0x0F,0x12,0x00,0x44,
	0x0F,0x12,0x00,0x23,
	0x0F,0x12,0x00,0x11,
	0x0F,0x12,0x00,0x0C,
	0x0F,0x12,0x00,0x10,
	0x0F,0x12,0x00,0x22,
	0x0F,0x12,0x00,0x43,
	0x0F,0x12,0x00,0x74,
	0x0F,0x12,0x00,0xB7,
	0x0F,0x12,0x00,0xF7,
	0x0F,0x12,0x00,0xFC,
	0x0F,0x12,0x00,0xB7,
	0x0F,0x12,0x00,0x6F,
	0x0F,0x12,0x00,0x3C,
	0x0F,0x12,0x00,0x1C,
	0x0F,0x12,0x00,0x0A,
	0x0F,0x12,0x00,0x04,
	0x0F,0x12,0x00,0x0A,
	0x0F,0x12,0x00,0x1B,
	0x0F,0x12,0x00,0x3B,
	0x0F,0x12,0x00,0x6C,
	0x0F,0x12,0x00,0xB0,
	0x0F,0x12,0x00,0xF2,
	0x0F,0x12,0x00,0xEF,
	0x0F,0x12,0x00,0xAB,
	0x0F,0x12,0x00,0x65,
	0x0F,0x12,0x00,0x34,
	0x0F,0x12,0x00,0x15,
	0x0F,0x12,0x00,0x04,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x04,
	0x0F,0x12,0x00,0x13,
	0x0F,0x12,0x00,0x33};


static kal_uint8 s5k8aayx_init_4[] = {
	0x0F,0x12,0x00,0x63,
	0x0F,0x12,0x00,0xA5,
	0x0F,0x12,0x00,0xE5,
	0x0F,0x12,0x00,0xF7,
	0x0F,0x12,0x00,0xB4,
	0x0F,0x12,0x00,0x6D,
	0x0F,0x12,0x00,0x3C,
	0x0F,0x12,0x00,0x1C,
	0x0F,0x12,0x00,0x0B,
	0x0F,0x12,0x00,0x05,
	0x0F,0x12,0x00,0x0A,
	0x0F,0x12,0x00,0x1B,
	0x0F,0x12,0x00,0x3B,
	0x0F,0x12,0x00,0x6B,
	0x0F,0x12,0x00,0xAD,
	0x0F,0x12,0x00,0xED,
	0x0F,0x12,0x01,0x0B,
	0x0F,0x12,0x00,0xCB,
	0x0F,0x12,0x00,0x85,
	0x0F,0x12,0x00,0x51,
	0x0F,0x12,0x00,0x2F,
	0x0F,0x12,0x00,0x1C,
	0x0F,0x12,0x00,0x16,
	0x0F,0x12,0x00,0x1C,
	0x0F,0x12,0x00,0x2E,
	0x0F,0x12,0x00,0x4F,
	0x0F,0x12,0x00,0x81,
	0x0F,0x12,0x00,0xC4,
	0x0F,0x12,0x01,0x02,
	0x0F,0x12,0x01,0x19,
	0x0F,0x12,0x00,0xDF,
	0x0F,0x12,0x00,0x9B,
	0x0F,0x12,0x00,0x67,
	0x0F,0x12,0x00,0x45,
	0x0F,0x12,0x00,0x30,
	0x0F,0x12,0x00,0x29,
	0x0F,0x12,0x00,0x2F,
	0x0F,0x12,0x00,0x43,
	0x0F,0x12,0x00,0x66,
	0x0F,0x12,0x00,0x98,
	0x0F,0x12,0x00,0xD9,
	0x0F,0x12,0x01,0x0F,
	0x0F,0x12,0x01,0x38,
	0x0F,0x12,0x01,0x0C,
	0x0F,0x12,0x00,0xCB,
	0x0F,0x12,0x00,0x97,
	0x0F,0x12,0x00,0x73,
	0x0F,0x12,0x00,0x5C,
	0x0F,0x12,0x00,0x54,
	0x0F,0x12,0x00,0x5B,
	0x0F,0x12,0x00,0x70,
	0x0F,0x12,0x00,0x96,
	0x0F,0x12,0x00,0xC9,
	0x0F,0x12,0x01,0x06,
	0x0F,0x12,0x01,0x2D,
	0x0F,0x12,0x01,0x47,
	0x0F,0x12,0x01,0x2F,
	0x0F,0x12,0x00,0xF8,
	0x0F,0x12,0x00,0xC5,
	0x0F,0x12,0x00,0xA1,
	0x0F,0x12,0x00,0x8B,
	0x0F,0x12,0x00,0x83,
	0x0F,0x12,0x00,0x8B,
	0x0F,0x12,0x00,0xA0,
	0x0F,0x12,0x00,0xC2,
	0x0F,0x12,0x00,0xF3,
	0x0F,0x12,0x01,0x24,
	0x0F,0x12,0x01,0x39,
	0x0F,0x12,0x00,0x93,
	0x0F,0x12,0x00,0x7E,
	0x0F,0x12,0x00,0x62,
	0x0F,0x12,0x00,0x4D,
	0x0F,0x12,0x00,0x3E,
	0x0F,0x12,0x00,0x34,
	0x0F,0x12,0x00,0x30,
	0x0F,0x12,0x00,0x32,
	0x0F,0x12,0x00,0x3B,
	0x0F,0x12,0x00,0x49,
	0x0F,0x12,0x00,0x5C,
	0x0F,0x12,0x00,0x77,
	0x0F,0x12,0x00,0x8A,
	0x0F,0x12,0x00,0x93,
	0x0F,0x12,0x00,0x77,
	0x0F,0x12,0x00,0x59,
	0x0F,0x12,0x00,0x42,
	0x0F,0x12,0x00,0x32,
	0x0F,0x12,0x00,0x27,
	0x0F,0x12,0x00,0x24,
	0x0F,0x12,0x00,0x26,
	0x0F,0x12,0x00,0x2F,
	0x0F,0x12,0x00,0x3D,
	0x0F,0x12,0x00,0x52,
	0x0F,0x12,0x00,0x6E,
	0x0F,0x12,0x00,0x8B,
	0x0F,0x12,0x00,0x83,
	0x0F,0x12,0x00,0x64,
	0x0F,0x12,0x00,0x46,
	0x0F,0x12,0x00,0x30,
	0x0F,0x12,0x00,0x20,
	0x0F,0x12,0x00,0x16,
	0x0F,0x12,0x00,0x11,
	0x0F,0x12,0x00,0x14,
	0x0F,0x12,0x00,0x1E,
	0x0F,0x12,0x00,0x2D,
	0x0F,0x12,0x00,0x41,
	0x0F,0x12,0x00,0x5D,
	0x0F,0x12,0x00,0x7C,
	0x0F,0x12,0x00,0x77,
	0x0F,0x12,0x00,0x57,
	0x0F,0x12,0x00,0x39,
	0x0F,0x12,0x00,0x24,
	0x0F,0x12,0x00,0x14,
	0x0F,0x12,0x00,0x0A,
	0x0F,0x12,0x00,0x07,
	0x0F,0x12,0x00,0x09,
	0x0F,0x12,0x00,0x12,
	0x0F,0x12,0x00,0x21,
	0x0F,0x12,0x00,0x36,
	0x0F,0x12,0x00,0x51,
	0x0F,0x12,0x00,0x70,
	0x0F,0x12,0x00,0x77,
	0x0F,0x12,0x00,0x56,
	0x0F,0x12,0x00,0x38,
	0x0F,0x12,0x00,0x22,
	0x0F,0x12,0x00,0x13,
	0x0F,0x12,0x00,0x09,
	0x0F,0x12,0x00,0x05,
	0x0F,0x12,0x00,0x08,
	0x0F,0x12,0x00,0x11,
	0x0F,0x12,0x00,0x20,
	0x0F,0x12,0x00,0x35,
	0x0F,0x12,0x00,0x51,
	0x0F,0x12,0x00,0x71,
	0x0F,0x12,0x00,0x6E,
	0x0F,0x12,0x00,0x4E,
	0x0F,0x12,0x00,0x32,
	0x0F,0x12,0x00,0x1C,
	0x0F,0x12,0x00,0x0D,
	0x0F,0x12,0x00,0x04,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x03,
	0x0F,0x12,0x00,0x0B,
	0x0F,0x12,0x00,0x1A,
	0x0F,0x12,0x00,0x2F,
	0x0F,0x12,0x00,0x49,
	0x0F,0x12,0x00,0x68,
	0x0F,0x12,0x00,0x72,
	0x0F,0x12,0x00,0x53,
	0x0F,0x12,0x00,0x37,
	0x0F,0x12,0x00,0x21,
	0x0F,0x12,0x00,0x12,
	0x0F,0x12,0x00,0x09,
	0x0F,0x12,0x00,0x05,
	0x0F,0x12,0x00,0x08,
	0x0F,0x12,0x00,0x10,
	0x0F,0x12,0x00,0x1F,
	0x0F,0x12,0x00,0x34,
	0x0F,0x12,0x00,0x4E,
	0x0F,0x12,0x00,0x6C,
	0x0F,0x12,0x00,0x7F,
	0x0F,0x12,0x00,0x60,
	0x0F,0x12,0x00,0x43,
	0x0F,0x12,0x00,0x2D,
	0x0F,0x12,0x00,0x1D,
	0x0F,0x12,0x00,0x13,
	0x0F,0x12,0x00,0x10,
	0x0F,0x12,0x00,0x13,
	0x0F,0x12,0x00,0x1C,
	0x0F,0x12,0x00,0x2B,
	0x0F,0x12,0x00,0x40,
	0x0F,0x12,0x00,0x5A,
	0x0F,0x12,0x00,0x79,
	0x0F,0x12,0x00,0x82,
	0x0F,0x12,0x00,0x66,
	0x0F,0x12,0x00,0x49,
	0x0F,0x12,0x00,0x35,
	0x0F,0x12,0x00,0x25,
	0x0F,0x12,0x00,0x1B,
	0x0F,0x12,0x00,0x17,
	0x0F,0x12,0x00,0x19,
	0x0F,0x12,0x00,0x23,
	0x0F,0x12,0x00,0x33,
	0x0F,0x12,0x00,0x46,
	0x0F,0x12,0x00,0x60,
	0x0F,0x12,0x00,0x7B,
	0x0F,0x12,0x00,0x92,
	0x0F,0x12,0x00,0x7C,
	0x0F,0x12,0x00,0x60,
	0x0F,0x12,0x00,0x4B,
	0x0F,0x12,0x00,0x3C,
	0x0F,0x12,0x00,0x32,
	0x0F,0x12,0x00,0x2D,
	0x0F,0x12,0x00,0x30,
	0x0F,0x12,0x00,0x39,
	0x0F,0x12,0x00,0x49,
	0x0F,0x12,0x00,0x5D,
	0x0F,0x12,0x00,0x76,
	0x0F,0x12,0x00,0x8C,
	0x0F,0x12,0x00,0x9F,
	0x0F,0x12,0x00,0x8F};


static kal_uint8 s5k8aayx_init_5[] = {
	0x0F,0x12,0x00,0x77,
	0x0F,0x12,0x00,0x61,
	0x0F,0x12,0x00,0x52,
	0x0F,0x12,0x00,0x48,
	0x0F,0x12,0x00,0x43,
	0x0F,0x12,0x00,0x47,
	0x0F,0x12,0x00,0x50,
	0x0F,0x12,0x00,0x5E,
	0x0F,0x12,0x00,0x71,
	0x0F,0x12,0x00,0x86,
	0x0F,0x12,0x00,0x97,
	0x0F,0x12,0x00,0x93,
	0x0F,0x12,0x00,0x7C,
	0x0F,0x12,0x00,0x5F,
	0x0F,0x12,0x00,0x49,
	0x0F,0x12,0x00,0x3A,
	0x0F,0x12,0x00,0x30,
	0x0F,0x12,0x00,0x2C,
	0x0F,0x12,0x00,0x2F,
	0x0F,0x12,0x00,0x37,
	0x0F,0x12,0x00,0x45,
	0x0F,0x12,0x00,0x5A,
	0x0F,0x12,0x00,0x75,
	0x0F,0x12,0x00,0x8A,
	0x0F,0x12,0x00,0x94,
	0x0F,0x12,0x00,0x77,
	0x0F,0x12,0x00,0x57,
	0x0F,0x12,0x00,0x40,
	0x0F,0x12,0x00,0x2F,
	0x0F,0x12,0x00,0x24,
	0x0F,0x12,0x00,0x20,
	0x0F,0x12,0x00,0x23,
	0x0F,0x12,0x00,0x2D,
	0x0F,0x12,0x00,0x3B,
	0x0F,0x12,0x00,0x51,
	0x0F,0x12,0x00,0x6E,
	0x0F,0x12,0x00,0x8C,
	0x0F,0x12,0x00,0x85,
	0x0F,0x12,0x00,0x66,
	0x0F,0x12,0x00,0x46,
	0x0F,0x12,0x00,0x2F,
	0x0F,0x12,0x00,0x1F,
	0x0F,0x12,0x00,0x14,
	0x0F,0x12,0x00,0x0F,
	0x0F,0x12,0x00,0x12,
	0x0F,0x12,0x00,0x1C,
	0x0F,0x12,0x00,0x2B,
	0x0F,0x12,0x00,0x40,
	0x0F,0x12,0x00,0x5C,
	0x0F,0x12,0x00,0x7D,
	0x0F,0x12,0x00,0x7A,
	0x0F,0x12,0x00,0x5A,
	0x0F,0x12,0x00,0x3A,
	0x0F,0x12,0x00,0x24,
	0x0F,0x12,0x00,0x14,
	0x0F,0x12,0x00,0x09,
	0x0F,0x12,0x00,0x06,
	0x0F,0x12,0x00,0x08,
	0x0F,0x12,0x00,0x11,
	0x0F,0x12,0x00,0x20,
	0x0F,0x12,0x00,0x36,
	0x0F,0x12,0x00,0x51,
	0x0F,0x12,0x00,0x72,
	0x0F,0x12,0x00,0x7B,
	0x0F,0x12,0x00,0x59,
	0x0F,0x12,0x00,0x3A,
	0x0F,0x12,0x00,0x23,
	0x0F,0x12,0x00,0x12,
	0x0F,0x12,0x00,0x08,
	0x0F,0x12,0x00,0x04,
	0x0F,0x12,0x00,0x07,
	0x0F,0x12,0x00,0x0F,
	0x0F,0x12,0x00,0x1F,
	0x0F,0x12,0x00,0x35,
	0x0F,0x12,0x00,0x51,
	0x0F,0x12,0x00,0x72,
	0x0F,0x12,0x00,0x73,
	0x0F,0x12,0x00,0x53,
	0x0F,0x12,0x00,0x34,
	0x0F,0x12,0x00,0x1D,
	0x0F,0x12,0x00,0x0E,
	0x0F,0x12,0x00,0x04,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x02,
	0x0F,0x12,0x00,0x0A,
	0x0F,0x12,0x00,0x1A,
	0x0F,0x12,0x00,0x2F,
	0x0F,0x12,0x00,0x4A,
	0x0F,0x12,0x00,0x6A,
	0x0F,0x12,0x00,0x77,
	0x0F,0x12,0x00,0x58,
	0x0F,0x12,0x00,0x39,
	0x0F,0x12,0x00,0x22,
	0x0F,0x12,0x00,0x12,
	0x0F,0x12,0x00,0x08,
	0x0F,0x12,0x00,0x04,
	0x0F,0x12,0x00,0x07,
	0x0F,0x12,0x00,0x0F,
	0x0F,0x12,0x00,0x1E,
	0x0F,0x12,0x00,0x34,
	0x0F,0x12,0x00,0x4F,
	0x0F,0x12,0x00,0x6F,
	0x0F,0x12,0x00,0x83,
	0x0F,0x12,0x00,0x64,
	0x0F,0x12,0x00,0x45,
	0x0F,0x12,0x00,0x2E,
	0x0F,0x12,0x00,0x1D,
	0x0F,0x12,0x00,0x12,
	0x0F,0x12,0x00,0x0F,
	0x0F,0x12,0x00,0x11,
	0x0F,0x12,0x00,0x1A,
	0x0F,0x12,0x00,0x2A,
	0x0F,0x12,0x00,0x3F,
	0x0F,0x12,0x00,0x5B,
	0x0F,0x12,0x00,0x7B,
	0x0F,0x12,0x00,0x87,
	0x0F,0x12,0x00,0x6A,
	0x0F,0x12,0x00,0x4B,
	0x0F,0x12,0x00,0x36,
	0x0F,0x12,0x00,0x25,
	0x0F,0x12,0x00,0x19,
	0x0F,0x12,0x00,0x15,
	0x0F,0x12,0x00,0x17,
	0x0F,0x12,0x00,0x22,
	0x0F,0x12,0x00,0x31,
	0x0F,0x12,0x00,0x45,
	0x0F,0x12,0x00,0x60,
	0x0F,0x12,0x00,0x7D,
	0x0F,0x12,0x00,0x96,
	0x0F,0x12,0x00,0x7F,
	0x0F,0x12,0x00,0x61,
	0x0F,0x12,0x00,0x4B,
	0x0F,0x12,0x00,0x3B,
	0x0F,0x12,0x00,0x2F,
	0x0F,0x12,0x00,0x2A,
	0x0F,0x12,0x00,0x2D,
	0x0F,0x12,0x00,0x36,
	0x0F,0x12,0x00,0x46,
	0x0F,0x12,0x00,0x5B,
	0x0F,0x12,0x00,0x75,
	0x0F,0x12,0x00,0x8D,
	0x0F,0x12,0x00,0xA1,
	0x0F,0x12,0x00,0x91,
	0x0F,0x12,0x00,0x77,
	0x0F,0x12,0x00,0x60,
	0x0F,0x12,0x00,0x50,
	0x0F,0x12,0x00,0x44,
	0x0F,0x12,0x00,0x40,
	0x0F,0x12,0x00,0x43,
	0x0F,0x12,0x00,0x4C,
	0x0F,0x12,0x00,0x5A,
	0x0F,0x12,0x00,0x6D,
	0x0F,0x12,0x00,0x84,
	0x0F,0x12,0x00,0x94,
	0x0F,0x12,0x00,0x72,
	0x0F,0x12,0x00,0x63,
	0x0F,0x12,0x00,0x4C,
	0x0F,0x12,0x00,0x3A,
	0x0F,0x12,0x00,0x2D,
	0x0F,0x12,0x00,0x25,
	0x0F,0x12,0x00,0x23,
	0x0F,0x12,0x00,0x25,
	0x0F,0x12,0x00,0x2C,
	0x0F,0x12,0x00,0x38,
	0x0F,0x12,0x00,0x4A,
	0x0F,0x12,0x00,0x5F,
	0x0F,0x12,0x00,0x6B,
	0x0F,0x12,0x00,0x79,
	0x0F,0x12,0x00,0x65,
	0x0F,0x12,0x00,0x4A,
	0x0F,0x12,0x00,0x37,
	0x0F,0x12,0x00,0x29,
	0x0F,0x12,0x00,0x21,
	0x0F,0x12,0x00,0x1D,
	0x0F,0x12,0x00,0x1F,
	0x0F,0x12,0x00,0x27,
	0x0F,0x12,0x00,0x33,
	0x0F,0x12,0x00,0x44,
	0x0F,0x12,0x00,0x5E,
	0x0F,0x12,0x00,0x6E,
	0x0F,0x12,0x00,0x6A,
	0x0F,0x12,0x00,0x55,
	0x0F,0x12,0x00,0x3A,
	0x0F,0x12,0x00,0x28,
	0x0F,0x12,0x00,0x1A,
	0x0F,0x12,0x00,0x11,
	0x0F,0x12,0x00,0x0D,
	0x0F,0x12,0x00,0x0F,
	0x0F,0x12,0x00,0x17,
	0x0F,0x12,0x00,0x24,
	0x0F,0x12,0x00,0x35,
	0x0F,0x12,0x00,0x4E,
	0x0F,0x12,0x00,0x61,
	0x0F,0x12,0x00,0x61,
	0x0F,0x12,0x00,0x4A,
	0x0F,0x12,0x00,0x31,
	0x0F,0x12,0x00,0x1E,
	0x0F,0x12,0x00,0x11,
	0x0F,0x12,0x00,0x08,
	0x0F,0x12,0x00,0x05};


static kal_uint8 s5k8aayx_init_6[] = {
	0x0F,0x12,0x00,0x07,
	0x0F,0x12,0x00,0x0E,
	0x0F,0x12,0x00,0x1B,
	0x0F,0x12,0x00,0x2D,
	0x0F,0x12,0x00,0x45,
	0x0F,0x12,0x00,0x59,
	0x0F,0x12,0x00,0x62,
	0x0F,0x12,0x00,0x4B,
	0x0F,0x12,0x00,0x31,
	0x0F,0x12,0x00,0x1E,
	0x0F,0x12,0x00,0x10,
	0x0F,0x12,0x00,0x08,
	0x0F,0x12,0x00,0x04,
	0x0F,0x12,0x00,0x06,
	0x0F,0x12,0x00,0x0E,
	0x0F,0x12,0x00,0x1B,
	0x0F,0x12,0x00,0x2E,
	0x0F,0x12,0x00,0x46,
	0x0F,0x12,0x00,0x5A,
	0x0F,0x12,0x00,0x5B,
	0x0F,0x12,0x00,0x45,
	0x0F,0x12,0x00,0x2C,
	0x0F,0x12,0x00,0x1A,
	0x0F,0x12,0x00,0x0C,
	0x0F,0x12,0x00,0x03,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x02,
	0x0F,0x12,0x00,0x09,
	0x0F,0x12,0x00,0x16,
	0x0F,0x12,0x00,0x29,
	0x0F,0x12,0x00,0x40,
	0x0F,0x12,0x00,0x54,
	0x0F,0x12,0x00,0x5F,
	0x0F,0x12,0x00,0x4A,
	0x0F,0x12,0x00,0x31,
	0x0F,0x12,0x00,0x1F,
	0x0F,0x12,0x00,0x10,
	0x0F,0x12,0x00,0x08,
	0x0F,0x12,0x00,0x04,
	0x0F,0x12,0x00,0x07,
	0x0F,0x12,0x00,0x0E,
	0x0F,0x12,0x00,0x1B,
	0x0F,0x12,0x00,0x2E,
	0x0F,0x12,0x00,0x45,
	0x0F,0x12,0x00,0x59,
	0x0F,0x12,0x00,0x6C,
	0x0F,0x12,0x00,0x57,
	0x0F,0x12,0x00,0x3E,
	0x0F,0x12,0x00,0x2A,
	0x0F,0x12,0x00,0x1B,
	0x0F,0x12,0x00,0x12,
	0x0F,0x12,0x00,0x0F,
	0x0F,0x12,0x00,0x11,
	0x0F,0x12,0x00,0x19,
	0x0F,0x12,0x00,0x27,
	0x0F,0x12,0x00,0x39,
	0x0F,0x12,0x00,0x50,
	0x0F,0x12,0x00,0x63,
	0x0F,0x12,0x00,0x6F,
	0x0F,0x12,0x00,0x5C,
	0x0F,0x12,0x00,0x44,
	0x0F,0x12,0x00,0x31,
	0x0F,0x12,0x00,0x23,
	0x0F,0x12,0x00,0x19,
	0x0F,0x12,0x00,0x16,
	0x0F,0x12,0x00,0x17,
	0x0F,0x12,0x00,0x20,
	0x0F,0x12,0x00,0x2E,
	0x0F,0x12,0x00,0x40,
	0x0F,0x12,0x00,0x55,
	0x0F,0x12,0x00,0x64,
	0x0F,0x12,0x00,0x7E,
	0x0F,0x12,0x00,0x71,
	0x0F,0x12,0x00,0x59,
	0x0F,0x12,0x00,0x46,
	0x0F,0x12,0x00,0x39,
	0x0F,0x12,0x00,0x2F,
	0x0F,0x12,0x00,0x2A,
	0x0F,0x12,0x00,0x2D,
	0x0F,0x12,0x00,0x35,
	0x0F,0x12,0x00,0x43,
	0x0F,0x12,0x00,0x54,
	0x0F,0x12,0x00,0x69,
	0x0F,0x12,0x00,0x74,
	0x0F,0x12,0x00,0x83,
	0x0F,0x12,0x00,0x7D,
	0x0F,0x12,0x00,0x68,
	0x0F,0x12,0x00,0x55,
	0x0F,0x12,0x00,0x48,
	0x0F,0x12,0x00,0x3E,
	0x0F,0x12,0x00,0x3A,
	0x0F,0x12,0x00,0x3D,
	0x0F,0x12,0x00,0x45,
	0x0F,0x12,0x00,0x51,
	0x0F,0x12,0x00,0x61,
	0x0F,0x12,0x00,0x72,
	0x0F,0x12,0x00,0x77,
	0x00,0x2A,0x13,0x48,
	0x0F,0x12,0x00,0x01,
	0x00,0x2A,0x0E,0xC6,
	0x0F,0x12,0x00,0x0B,
	0x00,0x2A,0x0E,0xCE,
	0x0F,0x12,0x00,0x0B,
	0x00,0x2A,0x0F,0x16,
	0x0F,0x12,0x00,0x2B,
	0x00,0x2A,0x0F,0x1E,
	0x0F,0x12,0x00,0x2B,
	0x00,0x2A,0x0E,0xC8,
	0x0F,0x12,0x00,0x0B,
	0x00,0x2A,0x0E,0xD0,
	0x0F,0x12,0x03,0x3F,
	0x00,0x2A,0x0F,0x18,
	0x0F,0x12,0x03,0x66,
	0x00,0x2A,0x0F,0x20,
	0x0F,0x12,0x00,0x32,
	0x00,0x2A,0x0E,0x38,
	0x0F,0x12,0x04,0x76,
	0x0F,0x12,0x04,0x76,
	0x00,0x2A,0x0A,0xA0,
	0x0F,0x12,0x00,0x01,
	0x00,0x2A,0x0E,0x2C,
	0x0F,0x12,0x00,0x01,
	0x00,0x2A,0x12,0x50,
	0x0F,0x12,0xFF,0xFF,
	0x00,0x2A,0x12,0x02,
	0x0F,0x12,0x00,0x10,
	0x00,0x2A,0x0B,0x36,
	0x0F,0x12,0x00,0x05,
	0x00,0x2A,0x0B,0x3A,
	0x0F,0x12,0x00,0xEC,
	0x0F,0x12,0x02,0xC1,
	0x00,0x2A,0x0B,0x38,
	0x0F,0x12,0x00,0x10,
	0x00,0x2A,0x0A,0xE6,
	0x0F,0x12,0x03,0xE1,
	0x0F,0x12,0x04,0x13,
	0x0F,0x12,0x03,0x9E,
	0x0F,0x12,0x04,0x16,
	0x0F,0x12,0x03,0x67,
	0x0F,0x12,0x03,0xF3,
	0x0F,0x12,0x03,0x2D,
	0x0F,0x12,0x03,0xC5,
	0x0F,0x12,0x02,0xFD,
	0x0F,0x12,0x03,0x8F,
	0x0F,0x12,0x02,0xD3,
	0x0F,0x12,0x03,0x65,
	0x0F,0x12,0x02,0xAA,
	0x0F,0x12,0x03,0x3E,
	0x0F,0x12,0x02,0x8D,
	0x0F,0x12,0x03,0x10,
	0x0F,0x12,0x02,0x71,
	0x0F,0x12,0x02,0xF1,
	0x0F,0x12,0x02,0x5A,
	0x0F,0x12,0x02,0xD2,
	0x0F,0x12,0x02,0x49,
	0x0F,0x12,0x02,0xB9,
	0x0F,0x12,0x02,0x38,
	0x0F,0x12,0x02,0xA2,
	0x0F,0x12,0x02,0x1B,
	0x0F,0x12,0x02,0x89,
	0x0F,0x12,0x02,0x00,
	0x0F,0x12,0x02,0x6C,
	0x0F,0x12,0x01,0xFC,
	0x0F,0x12,0x02,0x4F,
	0x0F,0x12,0x02,0x1E,
	0x0F,0x12,0x02,0x2C,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x00,0x2A,0x0B,0xAA,
	0x0F,0x12,0x00,0x06,
	0x00,0x2A,0x0B,0xAE,
	0x0F,0x12,0x01,0x0E,
	0x0F,0x12,0x02,0xE9,
	0x00,0x2A,0x0B,0xAC,
	0x0F,0x12,0x00,0x09,
	0x00,0x2A,0x0B,0x7A,
	0x0F,0x12,0x03,0x8C,
	0x0F,0x12,0x03,0xDA,
	0x0F,0x12,0x03,0x0E,
	0x0F,0x12,0x03,0xE9,
	0x0F,0x12,0x02,0xA2,
	0x0F,0x12,0x03,0xC2,
	0x0F,0x12,0x02,0x59,
	0x0F,0x12,0x03,0x8A,
	0x0F,0x12,0x02,0x18,
	0x0F,0x12,0x03,0x52,
	0x0F,0x12,0x01,0xF4,
	0x0F,0x12,0x02,0xE1,
	0x0F,0x12,0x01,0xD7,
	0x0F,0x12,0x02,0x8E,
	0x0F,0x12,0x01,0xCB,
	0x0F,0x12,0x02,0x58,
	0x0F,0x12,0x02,0x2B,
	0x0F,0x12,0x01,0xCC};


	static kal_uint8 s5k8aayx_init_7[] = {
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x00,0x2A,0x0B,0x70,
	0x0F,0x12,0x00,0x05,
	0x00,0x2A,0x0B,0x74,
	0x0F,0x12,0x01,0xF8,
	0x0F,0x12,0x02,0xA8,
	0x00,0x2A,0x0B,0x72,
	0x0F,0x12,0x00,0x07,
	0x00,0x2A,0x0B,0x40,
	0x0F,0x12,0x02,0x9E,
	0x0F,0x12,0x02,0xC8,
	0x0F,0x12,0x02,0x81,
	0x0F,0x12,0x02,0xC8,
	0x0F,0x12,0x02,0x66,
	0x0F,0x12,0x02,0xAC,
	0x0F,0x12,0x02,0x51,
	0x0F,0x12,0x02,0x8E,
	0x0F,0x12,0x02,0x3D,
	0x0F,0x12,0x02,0x75,
	0x0F,0x12,0x02,0x28,
	0x0F,0x12,0x02,0x5D,
	0x0F,0x12,0x02,0x28,
	0x0F,0x12,0x02,0x43,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x00,0x2A,0x0B,0xC8,
	0x0F,0x12,0x00,0x05,
	0x00,0x2A,0x0B,0xCC,
	0x0F,0x12,0x01,0x0F,
	0x0F,0x12,0x01,0x8F,
	0x00,0x2A,0x0B,0xCA,
	0x0F,0x12,0x00,0x05,
	0x00,0x2A,0x0B,0xB4,
	0x0F,0x12,0x03,0xE7,
	0x0F,0x12,0x03,0xF8,
	0x0F,0x12,0x03,0xA7,
	0x0F,0x12,0x03,0xFC,
	0x0F,0x12,0x03,0x52,
	0x0F,0x12,0x03,0xD0,
	0x0F,0x12,0x03,0x22,
	0x0F,0x12,0x03,0x9E,
	0x0F,0x12,0x03,0x2B,
	0x0F,0x12,0x03,0x4D,
	0x00,0x2A,0x0B,0xE6,
	0x0F,0x12,0x00,0x06,
	0x00,0x2A,0x0B,0xEA,
	0x0F,0x12,0x01,0x9E,
	0x0F,0x12,0x02,0x57,
	0x00,0x2A,0x0B,0xE8,
	0x0F,0x12,0x00,0x04,
	0x00,0x2A,0x0B,0xD2,
	0x0F,0x12,0x03,0x0B,
	0x0F,0x12,0x03,0x23,
	0x0F,0x12,0x02,0xC3,
	0x0F,0x12,0x03,0x0F,
	0x0F,0x12,0x02,0x88,
	0x0F,0x12,0x02,0xE5,
	0x0F,0x12,0x02,0x6A,
	0x0F,0x12,0x02,0xA2,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x00,0x2A,0x0C,0x2C,
	0x0F,0x12,0x01,0x39,
	0x0F,0x12,0x01,0x22,
	0x00,0x2A,0x0B,0xFC,
	0x0F,0x12,0x03,0xAD,
	0x0F,0x12,0x01,0x3F,
	0x0F,0x12,0x03,0x41,
	0x0F,0x12,0x01,0x7B,
	0x0F,0x12,0x03,0x8D,
	0x0F,0x12,0x01,0x4B,
	0x0F,0x12,0x02,0xC3,
	0x0F,0x12,0x01,0xCC,
	0x0F,0x12,0x02,0x41,
	0x0F,0x12,0x02,0x7F,
	0x0F,0x12,0x02,0x41,
	0x0F,0x12,0x02,0x7F,
	0x0F,0x12,0x02,0x14,
	0x0F,0x12,0x02,0xA8,
	0x0F,0x12,0x02,0x70,
	0x0F,0x12,0x02,0x10,
	0x00,0x2A,0x0C,0x4C,
	0x0F,0x12,0x04,0x52,
	0x00,0x2A,0x0C,0x58,
	0x0F,0x12,0x05,0x9C,
	0x00,0x2A,0x0B,0xF8,
	0x0F,0x12,0x01,0xAE,
	0x00,0x2A,0x0C,0x28,
	0x0F,0x12,0x00,0x00,
	0x00,0x2A,0x0C,0xAC,
	0x0F,0x12,0x00,0x50,
	0x00,0x2A,0x0C,0x28,
	0x0F,0x12,0x00,0x00,
	0x00,0x2A,0x0D,0x0E,
	0x0F,0x12,0x00,0xB8,
	0x0F,0x12,0x00,0xB2,
	0x00,0x2A,0x0C,0xFE,
	0x0F,0x12,0x0F,0xAB,
	0x0F,0x12,0x0F,0xF5,
	0x0F,0x12,0x10,0xBB,
	0x0F,0x12,0x11,0x23,
	0x0F,0x12,0x11,0x65,
	0x0F,0x12,0x12,0x2A,
	0x0F,0x12,0x00,0xA9,
	0x0F,0x12,0x00,0xC0,
	0x00,0x2A,0x0C,0xF8,
	0x0F,0x12,0x03,0x0E,
	0x0F,0x12,0x03,0x4C,
	0x0F,0x12,0x03,0x88,
	0x00,0x2A,0x0C,0xB0,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x64,
	0x0F,0x12,0x00,0x64,
	0x0F,0x12,0x00,0x78,
	0x0F,0x12,0x00,0x78,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x64,
	0x0F,0x12,0x00,0x64,
	0x0F,0x12,0x00,0x78,
	0x0F,0x12,0x00,0x78,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x64,
	0x0F,0x12,0x00,0x64,
	0x0F,0x12,0x00,0x78,
	0x0F,0x12,0x00,0x78,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0xFF,0xC0,
	0x0F,0x12,0xFF,0x60,
	0x0F,0x12,0xFF,0x40,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0xFF,0xC0,
	0x0F,0x12,0xFF,0x60,
	0x0F,0x12,0xFF,0x40,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0xFF,0xC0,
	0x0F,0x12,0xFF,0x60,
	0x0F,0x12,0xFF,0x40,
	0x00,0x2A,0x0D,0x30,
	0x0F,0x12,0x00,0x02,
	0x00,0x2A,0x33,0x72,
	0x0F,0x12,0x00,0x01,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x00,0x2A,0x0C,0x86,
	0x0F,0x12,0x00,0x05,
	0x00,0x2A,0x0C,0x70,
	0x0F,0x12,0xFF,0x7B,
	0x0F,0x12,0x00,0xCE,
	0x0F,0x12,0xFF,0x23,
	0x0F,0x12,0x01,0x0D,
	0x0F,0x12,0xFE,0xF3,
	0x0F,0x12,0x01,0x2C,
	0x0F,0x12,0xFE,0xD7,
	0x0F,0x12,0x01,0x4E,
	0x0F,0x12,0xFE,0xBB,
	0x0F,0x12,0x01,0x62,
	0x0F,0x12,0x13,0x88,
	0x00,0x2A,0x0C,0x8A,
	0x0F,0x12,0x4A,0xCB,
	0x00,0x2A,0x0C,0x88,
	0x0F,0x12,0x0A,0x7C,
	0x00,0x2A,0x01,0x2E,
	0x0F,0x12,0x5D,0xC0,
	0x0F,0x12,0x00,0x00,
	0x00,0x2A,0x01,0x46,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x02,
	0x00,0x2A,0x01,0x4C,
	0x0F,0x12,0x2C,0xEC,
	0x00,0x2A,0x01,0x52,
	0x0F,0x12,0x59,0xD8,
	0x00,0x2A,0x01,0x4E,
	0x0F,0x12,0x59,0xD8,
	0x00,0x2A,0x01,0x54,
	0x0F,0x12,0x29,0x81,
	0x00,0x2A,0x01,0x5A,
	0x0F,0x12,0x52,0x08,
	0x00,0x2A,0x01,0x56};


	static kal_uint8 s5k8aayx_init_8[] = {
	0x0F,0x12,0x53,0xFC,
	0x00,0x2A,0x01,0x64,
	0x0F,0x12,0x00,0x01,
	0x00,0x28,0x70,0x00,
	0x00,0x2A,0x03,0xF4,
	0x0F,0x12,0x00,0x01,
	0x0F,0x12,0x00,0x01,
	0x00,0x2A,0x04,0x08,
	0x0F,0x12,0x06,0x7F,
	0x00,0x2A,0x0D,0x40,
	0x0F,0x12,0x00,0x3E,
	0x00,0x2A,0x0D,0x46,
	0x0F,0x12,0x00,0x0F,
	0x00,0x2A,0x04,0x40,
	0x0F,0x12,0x34,0x10,
	0x00,0x2A,0x04,0x44,
	0x0F,0x12,0x65,0x90,
	0x00,0x2A,0x04,0x48,
	0x0F,0x12,0xBB,0x80,
	0x00,0x2A,0x04,0x4C,
	0x0F,0x12,0x38,0x80,
	0x0F,0x12,0x00,0x01,
	0x00,0x2A,0x04,0x50,
	0x0F,0x12,0x34,0x10,
	0x00,0x2A,0x04,0x54,
	0x0F,0x12,0x65,0x90,
	0x00,0x2A,0x04,0x58,
	0x0F,0x12,0xBB,0x80,
	0x00,0x2A,0x04,0x5C,
	0x0F,0x12,0x38,0x80,
	0x0F,0x12,0x00,0x01,
	0x0F,0x12,0x01,0xB0,
	0x0F,0x12,0x01,0xB0,
	0x0F,0x12,0x02,0x80,
	0x0F,0x12,0x08,0x00,
	0x0F,0x12,0x01,0x00,
	0x0F,0x12,0x30,0x00,
	0x00,0x2A,0x04,0x2E,
	0x0F,0x12,0x01,0x0E,
	0x0F,0x12,0x00,0xF5,
	0x00,0x2A,0x0D,0xE0,
	0x0F,0x12,0x00,0x02,
	0x00,0x2A,0x0D,0x38,
	0x0F,0x12,0x01,0x98,
	0x00,0x2A,0x0D,0x3E,
	0x0F,0x12,0x02,0xA0,
	0x00,0x2A,0x0D,0x4E,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x01,0x01,
	0x0F,0x12,0x01,0x01,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x02,0x01,
	0x0F,0x12,0x02,0x02,
	0x0F,0x12,0x02,0x02,
	0x0F,0x12,0x01,0x02,
	0x0F,0x12,0x02,0x01,
	0x0F,0x12,0x03,0x03,
	0x0F,0x12,0x03,0x03,
	0x0F,0x12,0x01,0x02,
	0x0F,0x12,0x02,0x01,
	0x0F,0x12,0x04,0x03,
	0x0F,0x12,0x03,0x04,
	0x0F,0x12,0x01,0x02,
	0x0F,0x12,0x02,0x01,
	0x0F,0x12,0x04,0x03,
	0x0F,0x12,0x03,0x04,
	0x0F,0x12,0x01,0x02,
	0x0F,0x12,0x02,0x01,
	0x0F,0x12,0x03,0x03,
	0x0F,0x12,0x03,0x03,
	0x0F,0x12,0x01,0x02,
	0x0F,0x12,0x02,0x01,
	0x0F,0x12,0x02,0x02,
	0x0F,0x12,0x02,0x02,
	0x0F,0x12,0x01,0x02,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x01,0x01,
	0x0F,0x12,0x01,0x01,
	0x0F,0x12,0x00,0x00,
	0x00,0x2A,0x33,0xA4,
	0x0F,0x12,0x01,0xD0,
	0x0F,0x12,0xFF,0xA1,
	0x0F,0x12,0xFF,0xFA,
	0x0F,0x12,0xFF,0x6F,
	0x0F,0x12,0x01,0x40,
	0x0F,0x12,0xFF,0x49,
	0x0F,0x12,0xFF,0xC1,
	0x0F,0x12,0x00,0x1F,
	0x0F,0x12,0x01,0xBD,
	0x0F,0x12,0x01,0x3F,
	0x0F,0x12,0x00,0xE1,
	0x0F,0x12,0xFF,0x43,
	0x0F,0x12,0x01,0x91,
	0x0F,0x12,0xFF,0xC0,
	0x0F,0x12,0x01,0xB7,
	0x0F,0x12,0xFF,0x30,
	0x0F,0x12,0x01,0x5F,
	0x0F,0x12,0x01,0x06,
	0x0F,0x12,0x01,0xD0,
	0x0F,0x12,0xFF,0xA1,
	0x0F,0x12,0xFF,0xFA,
	0x0F,0x12,0xFF,0x6F,
	0x0F,0x12,0x01,0x40,
	0x0F,0x12,0xFF,0x49,
	0x0F,0x12,0xFF,0xC1,
	0x0F,0x12,0x00,0x1F,
	0x0F,0x12,0x01,0xBD,
	0x0F,0x12,0x01,0x3F,
	0x0F,0x12,0x00,0xE1,
	0x0F,0x12,0xFF,0x43,
	0x0F,0x12,0x01,0x91,
	0x0F,0x12,0xFF,0xC0,
	0x0F,0x12,0x01,0xB7,
	0x0F,0x12,0xFF,0x30,
	0x0F,0x12,0x01,0x5F,
	0x0F,0x12,0x01,0x06,
	0x0F,0x12,0x01,0xD0,
	0x0F,0x12,0xFF,0xA1,
	0x0F,0x12,0xFF,0xFA,
	0x0F,0x12,0xFF,0x6F,
	0x0F,0x12,0x01,0x40,
	0x0F,0x12,0xFF,0x49,
	0x0F,0x12,0xFF,0xE3,
	0x0F,0x12,0xFF,0xF9,
	0x0F,0x12,0x01,0xC1,
	0x0F,0x12,0x01,0x3F,
	0x0F,0x12,0x00,0xE1,
	0x0F,0x12,0xFF,0x43,
	0x0F,0x12,0x01,0x91,
	0x0F,0x12,0xFF,0xC0,
	0x0F,0x12,0x01,0xB7,
	0x0F,0x12,0xFF,0x30,
	0x0F,0x12,0x01,0x5F,
	0x0F,0x12,0x01,0x06,
	0x0F,0x12,0x01,0xD0,
	0x0F,0x12,0xFF,0xA1,
	0x0F,0x12,0xFF,0xFA,
	0x0F,0x12,0xFF,0x6F,
	0x0F,0x12,0x01,0x40,
	0x0F,0x12,0xFF,0x49,
	0x0F,0x12,0xFF,0xE3,
	0x0F,0x12,0xFF,0xF9,
	0x0F,0x12,0x01,0xC1,
	0x0F,0x12,0x01,0x3F,
	0x0F,0x12,0x00,0xE1,
	0x0F,0x12,0xFF,0x43,
	0x0F,0x12,0x01,0x91,
	0x0F,0x12,0xFF,0xC0,
	0x0F,0x12,0x01,0xB7,
	0x0F,0x12,0xFF,0x30,
	0x0F,0x12,0x01,0x5F,
	0x0F,0x12,0x01,0x06,
	0x0F,0x12,0x01,0xBF,
	0x0F,0x12,0xFF,0xBF,
	0x0F,0x12,0xFF,0xFE,
	0x0F,0x12,0xFF,0x6D,
	0x0F,0x12,0x01,0xB4,
	0x0F,0x12,0xFF,0x66,
	0x0F,0x12,0xFF,0xCA,
	0x0F,0x12,0xFF,0xCE,
	0x0F,0x12,0x01,0x7B,
	0x0F,0x12,0x01,0x36,
	0x0F,0x12,0x01,0x32,
	0x0F,0x12,0xFF,0x85,
	0x0F,0x12,0x01,0x8B,
	0x0F,0x12,0xFF,0x73,
	0x0F,0x12,0x01,0x91,
	0x0F,0x12,0xFF,0x3F,
	0x0F,0x12,0x01,0x5B,
	0x0F,0x12,0x00,0xD0,
	0x0F,0x12,0x01,0xBF,
	0x0F,0x12,0xFF,0xBF,
	0x0F,0x12,0xFF,0xFE,
	0x0F,0x12,0xFF,0x6D,
	0x0F,0x12,0x01,0xB4,
	0x0F,0x12,0xFF,0x66,
	0x0F,0x12,0xFF,0xCA,
	0x0F,0x12,0xFF,0xCE,
	0x0F,0x12,0x01,0x7B,
	0x0F,0x12,0x01,0x36,
	0x0F,0x12,0x01,0x32,
	0x0F,0x12,0xFF,0x85,
	0x0F,0x12,0x01,0x8B,
	0x0F,0x12,0xFF,0x73,
	0x0F,0x12,0x01,0x91,
	0x0F,0x12,0xFF,0x3F,
	0x0F,0x12,0x01,0x5B,
	0x0F,0x12,0x00,0xD0,
	0x00,0x2A,0x33,0x80,
	0x0F,0x12,0x01,0xAC,
	0x0F,0x12,0xFF,0xD7,
	0x0F,0x12,0x00,0x19,
	0x0F,0x12,0xFF,0x49,
	0x0F,0x12,0x01,0xD9,
	0x0F,0x12,0xFF,0x63,
	0x0F,0x12,0xFF,0xCA,
	0x0F,0x12,0xFF,0xCE,
	0x0F,0x12,0x01,0x7B,
	0x0F,0x12,0x01,0x32,
	0x0F,0x12,0x01,0x2E};


static kal_uint8 s5k8aayx_init_9[] = {
	0x0F,0x12,0xFF,0x8D,
	0x0F,0x12,0x01,0x8B,
	0x0F,0x12,0xFF,0x73,
	0x0F,0x12,0x01,0x91,
	0x0F,0x12,0xFF,0x3F,
	0x0F,0x12,0x01,0x5B,
	0x0F,0x12,0x00,0xD0,
	0x00,0x2A,0x06,0x12,
	0x0F,0x12,0x00,0x9D,
	0x0F,0x12,0x00,0xD5,
	0x0F,0x12,0x01,0x03,
	0x0F,0x12,0x01,0x28,
	0x0F,0x12,0x01,0x66,
	0x0F,0x12,0x01,0x93,
	0x00,0x2A,0x05,0x38,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x1F,
	0x0F,0x12,0x00,0x35,
	0x0F,0x12,0x00,0x5A,
	0x0F,0x12,0x00,0x95,
	0x0F,0x12,0x00,0xE6,
	0x0F,0x12,0x01,0x21,
	0x0F,0x12,0x01,0x39,
	0x0F,0x12,0x01,0x50,
	0x0F,0x12,0x01,0x77,
	0x0F,0x12,0x01,0x9A,
	0x0F,0x12,0x01,0xBB,
	0x0F,0x12,0x01,0xDC,
	0x0F,0x12,0x02,0x19,
	0x0F,0x12,0x02,0x51,
	0x0F,0x12,0x02,0xB3,
	0x0F,0x12,0x03,0x0A,
	0x0F,0x12,0x03,0x5F,
	0x0F,0x12,0x03,0xB1,
	0x0F,0x12,0x03,0xFF,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x01,
	0x0F,0x12,0x00,0x01,
	0x0F,0x12,0x00,0x02,
	0x0F,0x12,0x00,0x04,
	0x0F,0x12,0x00,0x0A,
	0x0F,0x12,0x00,0x12,
	0x0F,0x12,0x00,0x16,
	0x0F,0x12,0x00,0x1A,
	0x0F,0x12,0x00,0x24,
	0x0F,0x12,0x00,0x31,
	0x0F,0x12,0x00,0x3E,
	0x0F,0x12,0x00,0x4E,
	0x0F,0x12,0x00,0x75,
	0x0F,0x12,0x00,0xA8,
	0x0F,0x12,0x01,0x26,
	0x0F,0x12,0x01,0xBE,
	0x0F,0x12,0x02,0x72,
	0x0F,0x12,0x03,0x34,
	0x0F,0x12,0x03,0xFF,
	0x00,0x2A,0x04,0x98,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x02,
	0x0F,0x12,0x00,0x07,
	0x0F,0x12,0x00,0x1D,
	0x0F,0x12,0x00,0x6E,
	0x0F,0x12,0x00,0xD3,
	0x0F,0x12,0x01,0x27,
	0x0F,0x12,0x01,0x4C,
	0x0F,0x12,0x01,0x6E,
	0x0F,0x12,0x01,0xA5,
	0x0F,0x12,0x01,0xD3,
	0x0F,0x12,0x01,0xFB,
	0x0F,0x12,0x02,0x1F,
	0x0F,0x12,0x02,0x60,
	0x0F,0x12,0x02,0x9A,
	0x0F,0x12,0x02,0xF7,
	0x0F,0x12,0x03,0x4D,
	0x0F,0x12,0x03,0x95,
	0x0F,0x12,0x03,0xCE,
	0x0F,0x12,0x03,0xFF,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x04,
	0x0F,0x12,0x00,0x0C,
	0x0F,0x12,0x00,0x24,
	0x0F,0x12,0x00,0x6E,
	0x0F,0x12,0x00,0xD1,
	0x0F,0x12,0x01,0x19,
	0x0F,0x12,0x01,0x39,
	0x0F,0x12,0x01,0x57,
	0x0F,0x12,0x01,0x8E,
	0x0F,0x12,0x01,0xC3,
	0x0F,0x12,0x01,0xF3,
	0x0F,0x12,0x02,0x1F,
	0x0F,0x12,0x02,0x69,
	0x0F,0x12,0x02,0xA6,
	0x0F,0x12,0x02,0xFF,
	0x0F,0x12,0x03,0x51,
	0x0F,0x12,0x03,0x95,
	0x0F,0x12,0x03,0xCE,
	0x0F,0x12,0x03,0xFF,
	0x00,0x2A,0x33,0x70,
	0x0F,0x12,0x00,0x00,
	0x00,0x2A,0x06,0xD4,
	0x0F,0x12,0x00,0x32,
	0x0F,0x12,0x00,0x78,
	0x0F,0x12,0x00,0xC8,
	0x0F,0x12,0x01,0x90,
	0x0F,0x12,0x02,0x8C,
	0x00,0x2A,0x07,0x34,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x78,
	0x0F,0x12,0x01,0x2C,
	0x0F,0x12,0x03,0xFF,
	0x0F,0x12,0x00,0x14,
	0x0F,0x12,0x00,0x64,
	0x0F,0x12,0x00,0x0C,
	0x0F,0x12,0x00,0x10,
	0x0F,0x12,0x01,0xE6,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x70,
	0x0F,0x12,0x01,0xFF,
	0x0F,0x12,0x01,0x44,
	0x0F,0x12,0x00,0x0F,
	0x0F,0x12,0x00,0x0A,
	0x0F,0x12,0x00,0x73,
	0x0F,0x12,0x00,0x87,
	0x0F,0x12,0x00,0x14,
	0x0F,0x12,0x00,0x0A,
	0x0F,0x12,0x00,0x23,
	0x0F,0x12,0x00,0x1E,
	0x0F,0x12,0x00,0x14,
	0x0F,0x12,0x00,0x0A,
	0x0F,0x12,0x00,0x23,
	0x0F,0x12,0x00,0x46,
	0x0F,0x12,0x2B,0x32,
	0x0F,0x12,0x06,0x01,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0xFF,
	0x0F,0x12,0x07,0xFF,
	0x0F,0x12,0xFF,0xFF,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x05,0x0D,
	0x0F,0x12,0x1E,0x80,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x14,0x08,
	0x0F,0x12,0x02,0x14,
	0x0F,0x12,0xFF,0x01,
	0x0F,0x12,0x18,0x0F,
	0x0F,0x12,0x00,0x01,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x80,0x03,
	0x0F,0x12,0x00,0x94,
	0x0F,0x12,0x05,0x80,
	0x0F,0x12,0x02,0x80,
	0x0F,0x12,0x03,0x08,
	0x0F,0x12,0x31,0x86,
	0x0F,0x12,0x52,0x60,
	0x0F,0x12,0x0A,0x02,
	0x0F,0x12,0x08,0x0A,
	0x0F,0x12,0x05,0x00,
	0x0F,0x12,0x03,0x2D,
	0x0F,0x12,0x32,0x4E,
	0x0F,0x12,0x00,0x1E,
	0x0F,0x12,0x02,0x00,
	0x0F,0x12,0x01,0x03,
	0x0F,0x12,0x01,0x0C,
	0x0F,0x12,0x96,0x96,
	0x0F,0x12,0x46,0x46,
	0x0F,0x12,0x08,0x02,
	0x0F,0x12,0x08,0x02,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x03,0x0F,
	0x0F,0x12,0x32,0x02,
	0x0F,0x12,0x0F,0x1E,
	0x0F,0x12,0x02,0x0F,
	0x0F,0x12,0x01,0x03,
	0x0F,0x12,0x01,0x0C,
	0x0F,0x12,0x96,0x96,
	0x0F,0x12,0x46,0x46,
	0x0F,0x12,0x08,0x02,
	0x0F,0x12,0x08,0x02,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x03,0x0F,
	0x0F,0x12,0x32,0x02,
	0x0F,0x12,0x0F,0x1E,
	0x0F,0x12,0x02,0x0F,
	0x0F,0x12,0x00,0x03,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x6A,
	0x0F,0x12,0x01,0x2C,
	0x0F,0x12,0x03,0xFF,
	0x0F,0x12,0x00,0x14,
	0x0F,0x12,0x00,0x64,
	0x0F,0x12,0x00,0x0C};


	static kal_uint8 s5k8aayx_init_10[] = {
	0x0F,0x12,0x00,0x10,
	0x0F,0x12,0x01,0xE6,
	0x0F,0x12,0x03,0xFF,
	0x0F,0x12,0x00,0x70,
	0x0F,0x12,0x00,0x7D,
	0x0F,0x12,0x00,0x64,
	0x0F,0x12,0x00,0x14,
	0x0F,0x12,0x00,0x0A,
	0x0F,0x12,0x00,0x73,
	0x0F,0x12,0x00,0x87,
	0x0F,0x12,0x00,0x14,
	0x0F,0x12,0x00,0x0A,
	0x0F,0x12,0x00,0x23,
	0x0F,0x12,0x00,0x1E,
	0x0F,0x12,0x00,0x14,
	0x0F,0x12,0x00,0x0A,
	0x0F,0x12,0x00,0x23,
	0x0F,0x12,0x00,0x1E,
	0x0F,0x12,0x2B,0x32,
	0x0F,0x12,0x06,0x01,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0xFF,
	0x0F,0x12,0x07,0xFF,
	0x0F,0x12,0xFF,0xFF,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x05,0x0D,
	0x0F,0x12,0x1E,0x80,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x14,0x08,
	0x0F,0x12,0x02,0x14,
	0x0F,0x12,0xFF,0x01,
	0x0F,0x12,0x18,0x0F,
	0x0F,0x12,0x00,0x02,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x80,0x03,
	0x0F,0x12,0x00,0x80,
	0x0F,0x12,0x00,0x80,
	0x0F,0x12,0x02,0x80,
	0x0F,0x12,0x03,0x08,
	0x0F,0x12,0x1E,0x65,
	0x0F,0x12,0x1A,0x24,
	0x0F,0x12,0x0A,0x03,
	0x0F,0x12,0x08,0x0A,
	0x0F,0x12,0x05,0x00,
	0x0F,0x12,0x03,0x2D,
	0x0F,0x12,0x32,0x4D,
	0x0F,0x12,0x00,0x1E,
	0x0F,0x12,0x02,0x00,
	0x0F,0x12,0x01,0x03,
	0x0F,0x12,0x01,0x0C,
	0x0F,0x12,0x96,0x96,
	0x0F,0x12,0x2F,0x34,
	0x0F,0x12,0x05,0x04,
	0x0F,0x12,0x08,0x0F,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x03,0x0F,
	0x0F,0x12,0x32,0x08,
	0x0F,0x12,0x0F,0x1E,
	0x0F,0x12,0x02,0x0F,
	0x0F,0x12,0x01,0x03,
	0x0F,0x12,0x01,0x0C,
	0x0F,0x12,0x96,0x96,
	0x0F,0x12,0x14,0x14,
	0x0F,0x12,0x05,0x04,
	0x0F,0x12,0x08,0x0F,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x03,0x0F,
	0x0F,0x12,0x32,0x08,
	0x0F,0x12,0x0F,0x1E,
	0x0F,0x12,0x02,0x0F,
	0x0F,0x12,0x00,0x03,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x64,
	0x0F,0x12,0x01,0x2C,
	0x0F,0x12,0x03,0xFF,
	0x0F,0x12,0x00,0x14,
	0x0F,0x12,0x00,0x64,
	0x0F,0x12,0x00,0x0C,
	0x0F,0x12,0x00,0x10,
	0x0F,0x12,0x01,0xE6,
	0x0F,0x12,0x03,0xFF,
	0x0F,0x12,0x00,0x70,
	0x0F,0x12,0x00,0x7D,
	0x0F,0x12,0x00,0x64,
	0x0F,0x12,0x00,0x96,
	0x0F,0x12,0x00,0x96,
	0x0F,0x12,0x00,0x73,
	0x0F,0x12,0x00,0x87,
	0x0F,0x12,0x00,0x14,
	0x0F,0x12,0x00,0x19,
	0x0F,0x12,0x00,0x23,
	0x0F,0x12,0x00,0x1E,
	0x0F,0x12,0x00,0x14,
	0x0F,0x12,0x00,0x19,
	0x0F,0x12,0x00,0x23,
	0x0F,0x12,0x00,0x1E,
	0x0F,0x12,0x2B,0x32,
	0x0F,0x12,0x06,0x01,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0xFF,
	0x0F,0x12,0x07,0xFF,
	0x0F,0x12,0xFF,0xFF,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x05,0x0D,
	0x0F,0x12,0x1E,0x80,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x0A,0x08,
	0x0F,0x12,0x02,0x00,
	0x0F,0x12,0xFF,0x01,
	0x0F,0x12,0x18,0x0F,
	0x0F,0x12,0x00,0x02,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x80,0x03,
	0x0F,0x12,0x00,0x80,
	0x0F,0x12,0x00,0x80,
	0x0F,0x12,0x02,0x80,
	0x0F,0x12,0x02,0x08,
	0x0F,0x12,0x1E,0x4B,
	0x0F,0x12,0x1A,0x24,
	0x0F,0x12,0x0A,0x05,
	0x0F,0x12,0x08,0x0A,
	0x0F,0x12,0x05,0x00,
	0x0F,0x12,0x03,0x2D,
	0x0F,0x12,0x32,0x4D,
	0x0F,0x12,0x00,0x1E,
	0x0F,0x12,0x02,0x00,
	0x0F,0x12,0x01,0x03,
	0x0F,0x12,0x01,0x0C,
	0x0F,0x12,0x96,0x96,
	0x0F,0x12,0x1E,0x23,
	0x0F,0x12,0x05,0x05,
	0x0F,0x12,0x08,0x0F,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x03,0x0F,
	0x0F,0x12,0x32,0x08,
	0x0F,0x12,0x0F,0x1E,
	0x0F,0x12,0x02,0x0F,
	0x0F,0x12,0x01,0x03,
	0x0F,0x12,0x01,0x0C,
	0x0F,0x12,0x96,0x96,
	0x0F,0x12,0x1E,0x23,
	0x0F,0x12,0x05,0x05,
	0x0F,0x12,0x08,0x0F,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x03,0x0F,
	0x0F,0x12,0x32,0x08,
	0x0F,0x12,0x0F,0x1E,
	0x0F,0x12,0x02,0x0F,
	0x0F,0x12,0x00,0x03,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x18,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x64,
	0x0F,0x12,0x01,0x2C,
	0x0F,0x12,0x03,0xFF,
	0x0F,0x12,0x00,0x14,
	0x0F,0x12,0x00,0x64,
	0x0F,0x12,0x00,0x0C,
	0x0F,0x12,0x00,0x10,
	0x0F,0x12,0x01,0xE6,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x70,
	0x0F,0x12,0x00,0x7D,
	0x0F,0x12,0x00,0x64,
	0x0F,0x12,0x00,0x96,
	0x0F,0x12,0x00,0x96,
	0x0F,0x12,0x00,0x73,
	0x0F,0x12,0x00,0x9F,
	0x0F,0x12,0x00,0x28,
	0x0F,0x12,0x00,0x28,
	0x0F,0x12,0x00,0x23,
	0x0F,0x12,0x00,0x37,
	0x0F,0x12,0x00,0x28,
	0x0F,0x12,0x00,0x28,
	0x0F,0x12,0x00,0x23,
	0x0F,0x12,0x00,0x37,
	0x0F,0x12,0x2B,0x32,
	0x0F,0x12,0x06,0x01,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0xFF,
	0x0F,0x12,0x07,0xA0,
	0x0F,0x12,0xFF,0xFF,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x05,0x0D,
	0x0F,0x12,0x1E,0x80,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x0A,0x08,
	0x0F,0x12,0x02,0x00};



	static kal_uint8 s5k8aayx_init_11[] = {
	0x0F,0x12,0xFF,0x01,
	0x0F,0x12,0x18,0x0F,
	0x0F,0x12,0x00,0x01,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x80,0x03,
	0x0F,0x12,0x00,0x80,
	0x0F,0x12,0x00,0x80,
	0x0F,0x12,0x02,0x80,
	0x0F,0x12,0x01,0x08,
	0x0F,0x12,0x1E,0x32,
	0x0F,0x12,0x1A,0x24,
	0x0F,0x12,0x0A,0x05,
	0x0F,0x12,0x08,0x0A,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x03,0x28,
	0x0F,0x12,0x32,0x4C,
	0x0F,0x12,0x00,0x1E,
	0x0F,0x12,0x02,0x00,
	0x0F,0x12,0x01,0x03,
	0x0F,0x12,0x01,0x0C,
	0x0F,0x12,0x96,0x96,
	0x0F,0x12,0x0F,0x0F,
	0x0F,0x12,0x03,0x07,
	0x0F,0x12,0x08,0x0F,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x03,0x0F,
	0x0F,0x12,0x32,0x08,
	0x0F,0x12,0x0F,0x1E,
	0x0F,0x12,0x02,0x0F,
	0x0F,0x12,0x01,0x03,
	0x0F,0x12,0x01,0x0C,
	0x0F,0x12,0x96,0x96,
	0x0F,0x12,0x0F,0x0F,
	0x0F,0x12,0x03,0x07,
	0x0F,0x12,0x08,0x0F,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x03,0x0F,
	0x0F,0x12,0x32,0x08,
	0x0F,0x12,0x0F,0x1E,
	0x0F,0x12,0x02,0x0F,
	0x0F,0x12,0x00,0x03,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x14,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x28,
	0x0F,0x12,0x01,0x2C,
	0x0F,0x12,0x03,0xFF,
	0x0F,0x12,0x00,0x14,
	0x0F,0x12,0x00,0x64,
	0x0F,0x12,0x00,0x0C,
	0x0F,0x12,0x00,0x10,
	0x0F,0x12,0x01,0xE6,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x70,
	0x0F,0x12,0x00,0x87,
	0x0F,0x12,0x00,0x73,
	0x0F,0x12,0x00,0x96,
	0x0F,0x12,0x00,0x96,
	0x0F,0x12,0x00,0x73,
	0x0F,0x12,0x00,0xB4,
	0x0F,0x12,0x00,0x28,
	0x0F,0x12,0x00,0x28,
	0x0F,0x12,0x00,0x23,
	0x0F,0x12,0x00,0x46,
	0x0F,0x12,0x00,0x28,
	0x0F,0x12,0x00,0x28,
	0x0F,0x12,0x00,0x23,
	0x0F,0x12,0x00,0x46,
	0x0F,0x12,0x2B,0x23,
	0x0F,0x12,0x06,0x01,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0xFF,
	0x0F,0x12,0x0B,0x84,
	0x0F,0x12,0xFF,0xFF,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x05,0x0D,
	0x0F,0x12,0x1E,0x80,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x0A,0x08,
	0x0F,0x12,0x02,0x00,
	0x0F,0x12,0xFF,0x01,
	0x0F,0x12,0x18,0x0F,
	0x0F,0x12,0x00,0x01,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x80,0x03,
	0x0F,0x12,0x00,0x80,
	0x0F,0x12,0x00,0x80,
	0x0F,0x12,0x02,0x80,
	0x0F,0x12,0x01,0x08,
	0x0F,0x12,0x1E,0x1E,
	0x0F,0x12,0x14,0x19,
	0x0F,0x12,0x0A,0x0A,
	0x0F,0x12,0x08,0x00,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x03,0x28,
	0x0F,0x12,0x32,0x4C,
	0x0F,0x12,0x00,0x1E,
	0x0F,0x12,0x02,0x00,
	0x0F,0x12,0x01,0x03,
	0x0F,0x12,0x01,0x0C,
	0x0F,0x12,0x64,0x64,
	0x0F,0x12,0x0F,0x0F,
	0x0F,0x12,0x03,0x07,
	0x0F,0x12,0x08,0x0F,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x03,0x0F,
	0x0F,0x12,0x32,0x08,
	0x0F,0x12,0x0F,0x1E,
	0x0F,0x12,0x02,0x0F,
	0x0F,0x12,0x01,0x03,
	0x0F,0x12,0x01,0x0C,
	0x0F,0x12,0x64,0x64,
	0x0F,0x12,0x0F,0x0F,
	0x0F,0x12,0x03,0x07,
	0x0F,0x12,0x08,0x0F,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x03,0x0F,
	0x0F,0x12,0x32,0x08,
	0x0F,0x12,0x0F,0x1E,
	0x0F,0x12,0x02,0x0F,
	0x0F,0x12,0x00,0x03,
	0x0F,0x12,0x7F,0x5E,
	0x0F,0x12,0xFE,0xEE,
	0x0F,0x12,0xD9,0xB7,
	0x0F,0x12,0x04,0x72,
	0x0F,0x12,0x00,0x01,
	0x00,0x2A,0x01,0xBE,
	0x0F,0x12,0x05,0x00,
	0x0F,0x12,0x03,0xC0,
	0x0F,0x12,0x00,0x05,
	0x0F,0x12,0x00,0x62,
	0x0F,0x12,0x01,0x00,
	0x0F,0x12,0x00,0x00,
	0x00,0x2A,0x01,0xD2,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x02,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x02,0x9A,
	0x00,0x2A,0x01,0xE8,
	0x0F,0x12,0x00,0x00,
	0x0F,0x12,0x00,0x00,
	0x00,0x2A,0x01,0xA8,
	0x0F,0x12,0x00,0x00,
	0x00,0x2A,0x01,0xAA,
	0x0F,0x12,0x00,0x01,
	0x00,0x2A,0x01,0x9E,
	0x0F,0x12,0x00,0x01,
	0x0F,0x12,0x00,0x01,
	0x00,0x28,0xD0,0x00,
	0x00,0x2A,0x10,0x00,
	0x0F,0x12,0x00,0x01,
	0x00,0x28,0xD0,0x00,
	0x10,0x00,0x00,0x01};


/*                                                                        
          
                          
 
             
                                                        
 
            
      
 
         
      
 
                  
 
                                                                        */
static void S5K8AAYX_MIPIInitialPara(void)
{
  spin_lock(&s5k8aayxmipi_drv_lock);
  S5K8AAYX_MIPICurrentStatus.iNightMode = 0;
  S5K8AAYX_MIPICurrentStatus.iWB = AWB_MODE_AUTO;
  S5K8AAYX_MIPICurrentStatus.iEffect = MEFFECT_OFF;
  S5K8AAYX_MIPICurrentStatus.iBanding = AE_FLICKER_MODE_50HZ;
  S5K8AAYX_MIPICurrentStatus.iEV = AE_EV_COMP_00;
  S5K8AAYX_MIPICurrentStatus.iMirror = IMAGE_NORMAL;
  S5K8AAYX_MIPICurrentStatus.iFrameRate = 0;
  spin_unlock(&s5k8aayxmipi_drv_lock);
}


void S5K8AAYX_MIPI_set_mirror(kal_uint8 image_mirror)
{

		if(S5K8AAYX_MIPICurrentStatus.iMirror == image_mirror)
		  return;

		S5K8AAYX_MIPI_write_cmos_sensor(0x0028, 0x7000);
		S5K8AAYX_MIPI_write_cmos_sensor(0x002a, 0x01E8);

		switch (image_mirror)
		{
			case IMAGE_NORMAL:
				S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, 0x0000); 	//                         
				S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, 0x0000); 	//                            
				break;
			case IMAGE_H_MIRROR:
				S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, 0x0001); 	//                         
				S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, 0x0001); 	//                            
				break;
			case IMAGE_V_MIRROR:
				S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, 0x0002); 	//                         
				S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, 0x0002); 	//                            
				break;
			case IMAGE_HV_MIRROR:
				S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, 0x0003); 	//                         
				S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, 0x0003); 	//                            
				break;

			default:
				ASSERT(0);
				break;
		}

		S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x01A8);
		S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                                                              
		S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x01AC);
		S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0001); //                               
		S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x01A6);
		S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0001);  //                                                         
		S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x01AA);
		S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0001);  //                             
		S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x019E);
		S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0001);  //                                          
		S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0001);  //                                

		spin_lock(&s5k8aayxmipi_drv_lock);
		S5K8AAYX_MIPICurrentStatus.iMirror = image_mirror;
		spin_unlock(&s5k8aayxmipi_drv_lock);

}





/*                                                                            
           
                           
              
  
             
                    
                    
          
        
                                                                             */
void S5K8AAYX_MIPI_set_dummy(kal_uint16 dummy_pixels, kal_uint16 dummy_lines)
{
		/*                                                   
                                               
                                                        */
		S5K8AAYX_MIPI_write_cmos_sensor(0x0028, 0x7000);
		S5K8AAYX_MIPI_write_cmos_sensor(0x002A, 0x044C);

		S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, dummy_pixels);
		//                                                                           
		S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, dummy_lines); 	//                 
}   /*                         */




void S5K8AAYX_MIPI_Initialize_Setting(void)
{

	SENSORDB("S5K8AAYX_MIPI_Initialize_Setting get in\n");
	S5K8AAYX_MIPI_write_cmos_sensor(0xFCFC,0xD000);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0010,0x0001);	//          
	S5K8AAYX_MIPI_write_cmos_sensor(0xFCFC,0x0000);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0000,0x0000);	//                       

	S5K8AAYX_MIPI_write_cmos_sensor(0xFCFC,0xD000);
	S5K8AAYX_MIPI_write_cmos_sensor(0x1030,0x0000);	//                 
	S5K8AAYX_MIPI_write_cmos_sensor(0x0014,0x0001);	//                                                       
	mdelay(10);		//               

#if 0
	//                                                                                 
	//              
	//                                                                                 
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x1278);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xAAF0);	//                                     

	//                                                                                 
	//                                 
	//                                                                                 

	//                                                                                 
	//                  
	//                                                                                 
	//                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0028,0x7000);
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x2470);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xB510);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x490D);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x480D);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xF000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xF96D);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x490D);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x480D);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xF000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xF969);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x490D);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x480D);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x6341);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x490D);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x480E);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xF000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xF962);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x490D);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x480E);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xF000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xF95E);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x490D);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x480E);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xF000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xF95A);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xBC10);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xBC08);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x4718);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x26D4);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x7000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x8EDD);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x264C);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x7000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x8725);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x25EC);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x7000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0080);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x7000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x2540);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x7000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xA6EF);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x250C);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x7000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xA0F1);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x24D8);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x7000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x058F);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x4010);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE92D);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00A0);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xEB00);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x023C);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE59F);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00B2);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE1D0);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE350);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0004);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0A00);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0080);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE310);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0002);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x1A00);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x1228);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE59F);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0001);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE3A0);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0DB2);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE1C1);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x4010);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE8BD);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF1E);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE12F);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x4010);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE92D);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x4000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE590);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0004);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE1A0);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0094);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xEB00);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0208);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE59F);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE5D0);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE350);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0002);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0A00);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0004);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE594);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00A0);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE1A0);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0004);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE584);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x4010);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE8BD);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF1E);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE12F);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x4070);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE92D);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE590);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0800);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE1A0);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0820);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE1A0);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x4041);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE280);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x01D4);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE59F);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x11B8);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE1D0);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x51B6);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE1D0);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0005);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE041);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0094);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x1D11);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE3A0);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0082);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xEB00);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x11B4);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE59F);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x1000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE5D1);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE351);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0A00);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00A0);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE1A0);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x219C);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE59F);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x3FB0);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE1D2);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE353);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0003);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0A00);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x3198);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE59F);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x5BB2);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE1C3);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xC000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE085);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xCBB4);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE1C3);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE351);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0A00);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0080);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE1A0);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x1DBC);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE1D2);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x3EB4);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE1D2);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x2EB2);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE1D2);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0193);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE001);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0092);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x2811);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE3A0);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0194);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE001);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0092);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x11A1);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE1A0);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x01A0);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE1A0);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0067);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xEB00);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x1154);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE59F);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x02B4);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE1C1);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x4070);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE8BD);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF1E);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE12F);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x4010);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE92D);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0063);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xEB00);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x213C);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE59F);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x14B0);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE1D2);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0080);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE311);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0005);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0A00);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0130);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE59F);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00B0);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE1D0);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0001);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE350);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0001);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x9A00);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0001);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE3A0);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xEA00);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE3A0);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x3104);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE59F);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE5C3);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE5D3);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE350);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0003);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0A00);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0080);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE3C1);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x1100);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE59F);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x04B0);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE1C2);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00B2);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE1C1);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x4010);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE8BD);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF1E);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE12F);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x41F0);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE92D);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x1000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE590);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xC801);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE1A0);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xC82C);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE1A0);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x1004);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE590);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x1801);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE1A0);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x1821);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE1A0);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x4008);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE590);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x500C);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE590);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x2004);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE1A0);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x3005);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE1A0);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x000C);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE1A0);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0043);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xEB00);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x60BC);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE59F);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00B2);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE1D6);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE350);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x000E);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0A00);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00B0);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE59F);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x05B4);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE1D0);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0002);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE350);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x000A);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x1A00);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x70A4);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE59F);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x10F4);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE1D6);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x26B0);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE1D7);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00F0);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE1D4);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0039);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xEB00);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00B0);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE1C4);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x26B0);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE1D7);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x10F6);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE1D6);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00F0);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE1D5);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0034);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xEB00);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00B0);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE1C5);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x41F0);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE8BD);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF1E);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE12F);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x4010);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE92D);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x4000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE1A0);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x1004);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE594);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x005C);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE59F);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00B0);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE1D0);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE350);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0008);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0A00);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0054);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE59F);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x3001);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE1A0);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x2068);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE590);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x004C);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE59F);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x1005);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE3A0);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0027);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xEB00);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE584);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x4010);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE8BD);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF1E);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE12F);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE594);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0025);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xEB00);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE584);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFFF9);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xEAFF);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x1728);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x7000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x112C);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x7000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x27C4);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x7000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x122C);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x7000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xF200);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xD000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x2340);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x7000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0E2C);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x7000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xF400);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xD000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x3370);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x7000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0CDC);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x7000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x20D4);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x7000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x06D4);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x7000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x4778);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x46C0);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xC000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE59F);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF1C);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE12F);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xC091);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xC000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE59F);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF1C);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE12F);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x058F);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xC000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE59F);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF1C);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE12F);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xA0F1);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xF004);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE51F);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xD14C);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xC000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE59F);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF1C);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE12F);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x2B43);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xC000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE59F);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF1C);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE12F);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x8725);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xC000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE59F);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF1C);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE12F);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x6777);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xC000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE59F);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF1C);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE12F);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x8E49);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xC000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE59F);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF1C);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xE12F);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x8EDD);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x90C8);	//         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//         
	//                                    
	//                        
	//                               


	//                                                                                 
	//               
	//                                                                                 

	//                                                                                 
	//                           
	//                                                                                 
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x1326);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x063A);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0100);	//                                
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0100);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0100);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0100);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0100);	//                                
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0100);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0100);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0100);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0100);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0100);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0100);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0100);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00B8);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0100);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0100);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0100);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0098);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00F8);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00F8);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0100);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00A0);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0100);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0100);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0100);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00A0);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0100);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0100);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0100);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x067A);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                          
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                  
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                  
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                  
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                          
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                  
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                  
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                  
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                  
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                  
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                  
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                  
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                  
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                  
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                      
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                  
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                  
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                  
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                      
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                  
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                  
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                  
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                      
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                  
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                  
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                  
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x06BA);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0001);	//             
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0632);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0100);	//                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0100);	//                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0100);	//                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0100);	//                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0672);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0100);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0100);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0100);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0100);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x06B2);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                      
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                      
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                      
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                      
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x06D0);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x000D);	//                       
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x000F);	//                       
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x06CC);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0280);	//                      
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x01E0);	//                      
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x06C6);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0001);	//                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0624);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x009D);	//                               
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00D5);	//                               
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0103);	//                          
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0128);	//                          
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0166);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0193);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x01A0);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x347C);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x013B);	//                          
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0116);	//                          
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00D9);	//                          
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00A6);	//                          
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0082);	//                          
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x006C);	//                          
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0065);	//                          
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x006C);	//                          
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0080);	//                          
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00A3);	//                          
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00D4);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x010D);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x012E);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0138);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0104);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00BE);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0088);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0062);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x004D);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0046);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x004C);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0060);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0084);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00B8);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00F9);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x012C);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x011A);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00DB);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0093);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x005F);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x003C);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0027);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0020);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0026);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x003A);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x005C);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x008E);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00D2);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x010E);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0101);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00BF);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0077);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0044);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0023);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0011);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x000C);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0010);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0022);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0043);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0074);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00B7);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00F7);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00FC);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00B7);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x006F);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x003C);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x001C);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x000A);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0004);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x000A);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x001B);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x003B);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x006C);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00B0);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00F2);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00EF);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00AB);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0065);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0034);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0015);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0004);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0004);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0013);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0033);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0063);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00A5);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00E5);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00F7);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00B4);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x006D);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x003C);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x001C);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x000B);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0005);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x000A);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x001B);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x003B);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x006B);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00AD);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00ED);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x010B);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00CB);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0085);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0051);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x002F);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x001C);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0016);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x001C);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x002E);	//                           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x004F);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0081);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00C4);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0102);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0119);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00DF);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x009B);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0067);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0045);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0030);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0029);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x002F);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0043);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0066);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0098);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00D9);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x010F);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0138);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x010C);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00CB);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0097);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0073);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x005C);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0054);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x005B);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0070);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0096);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00C9);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0106);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x012D);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0147);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x012F);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00F8);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00C5);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00A1);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x008B);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0083);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x008B);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00A0);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00C2);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00F3);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0124);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0139);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0093);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x007E);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0062);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x004D);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x003E);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0034);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0030);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0032);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x003B);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0049);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x005C);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0077);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x008A);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0093);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0077);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0059);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0042);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0032);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0027);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0024);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0026);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x002F);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x003D);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0052);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x006E);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x008B);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0083);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0064);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0046);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0030);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0020);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0016);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0011);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0014);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x001E);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x002D);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0041);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x005D);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x007C);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0077);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0057);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0039);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0024);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0014);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x000A);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0007);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0009);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0012);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0021);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0036);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0051);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0070);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0077);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0056);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0038);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0022);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0013);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0009);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0005);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0008);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0011);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0020);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0035);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0051);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0071);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x006E);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x004E);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0032);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x001C);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x000D);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0004);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0003);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x000B);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x001A);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x002F);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0049);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0068);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0072);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0053);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0037);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0021);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0012);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0009);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0005);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0008);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0010);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x001F);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0034);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x004E);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x006C);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x007F);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0060);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0043);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x002D);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x001D);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0013);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0010);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0013);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x001C);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x002B);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0040);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x005A);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0079);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0082);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0066);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0049);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0035);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0025);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x001B);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0017);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0019);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0023);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0033);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0046);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0060);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x007B);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0092);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x007C);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0060);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x004B);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x003C);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0032);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x002D);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0030);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0039);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0049);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x005D);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0076);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x008C);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x009F);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x008F);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0077);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0061);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0052);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0048);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0043);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0047);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0050);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x005E);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0071);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0086);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0097);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0093);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x007C);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x005F);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0049);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x003A);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0030);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x002C);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x002F);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0037);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0045);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x005A);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0075);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x008A);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0094);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0077);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0057);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0040);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x002F);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0024);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0020);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0023);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x002D);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x003B);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0051);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x006E);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x008C);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0085);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0066);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0046);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x002F);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x001F);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0014);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x000F);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0012);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x001C);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x002B);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0040);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x005C);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x007D);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x007A);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x005A);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x003A);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0024);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0014);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0009);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0006);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0008);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0011);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0020);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0036);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0051);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0072);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x007B);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0059);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x003A);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0023);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0012);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0008);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0004);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0007);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x000F);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x001F);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0035);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0051);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0072);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0073);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0053);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0034);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x001D);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x000E);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0004);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0002);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x000A);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x001A);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x002F);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x004A);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x006A);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0077);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0058);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0039);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0022);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0012);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0008);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0004);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0007);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x000F);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x001E);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0034);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x004F);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x006F);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0083);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0064);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0045);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x002E);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x001D);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0012);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x000F);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0011);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x001A);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x002A);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x003F);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x005B);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x007B);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0087);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x006A);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x004B);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0036);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0025);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0019);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0015);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0017);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0022);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0031);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0045);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0060);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x007D);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0096);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x007F);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0061);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x004B);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x003B);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x002F);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x002A);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x002D);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0036);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0046);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x005B);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0075);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x008D);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00A1);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0091);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0077);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0060);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0050);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0044);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0040);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0043);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x004C);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x005A);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x006D);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0084);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0094);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0072);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0063);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x004C);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x003A);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x002D);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0025);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0023);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0025);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x002C);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0038);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x004A);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x005F);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x006B);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0079);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0065);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x004A);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0037);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0029);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0021);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x001D);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x001F);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0027);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0033);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0044);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x005E);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x006E);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x006A);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0055);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x003A);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0028);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x001A);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0011);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x000D);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x000F);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0017);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0024);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0035);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x004E);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0061);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0061);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x004A);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0031);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x001E);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0011);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0008);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0005);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0007);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x000E);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x001B);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x002D);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0045);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0059);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0062);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x004B);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0031);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x001E);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0010);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0008);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0004);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0006);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x000E);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x001B);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x002E);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0046);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x005A);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x005B);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0045);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x002C);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x001A);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x000C);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0003);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0002);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0009);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0016);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0029);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0040);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0054);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x005F);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x004A);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0031);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x001F);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0010);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0008);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0004);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0007);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x000E);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x001B);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x002E);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0045);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0059);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x006C);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0057);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x003E);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x002A);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x001B);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0012);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x000F);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0011);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0019);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0027);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0039);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0050);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0063);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x006F);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x005C);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0044);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0031);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0023);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0019);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0016);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0017);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0020);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x002E);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0040);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0055);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0064);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x007E);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0071);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0059);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0046);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0039);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x002F);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x002A);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x002D);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0035);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0043);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0054);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0069);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0074);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0083);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x007D);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0068);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0055);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0048);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x003E);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x003A);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x003D);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0045);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0051);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0061);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0072);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0077);	//                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x1348);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0001);	//                 

	//                                                                                 
	//                     
	//                                                                                 
	//                                   
	//                                            
	//                                                                   
	//               
	//                                                                                 
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0EC6);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x000B);	//                  
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0ECE);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x000B);	//                  
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0F16);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x002B);	//                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0F1E);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x002B);	//                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0EC8);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x000B);	//                  
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0ED0);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x033F);	//                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0F18);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0366);	//                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0F20);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0032);	//                   

	//                          
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0E38);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0476);	//                                  
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0476);	//                               
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0AA0);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0001);	//                                          
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0E2C);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0001);	//                                                          
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x1250);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFFFF); 	//                       
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x1202);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0010); 	//                       
	//                                                                                 
	//              
	//                                                                                 

	//                                                                                 
	//                     
	//                                                                                 
	//                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0B36);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0005);	//                                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0B3A);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00EC);	//                                
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x02C1);	//                                
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0B38);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0010);	//                                  
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0AE6);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x03E1);	//                                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0413);	//                                      
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x039E);	//                                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0416);	//                                      
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0367);	//                                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x03F3);	//                                      
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x032D);	//                                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x03C5);	//                                      
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x02FD);	//                                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x038F);	//                                      
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x02D3);	//                                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0365);	//                                      
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x02AA);	//                                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x033E);	//                                      
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x028D);	//                                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0310);	//                                      
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0271);	//                                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x02F1);	//                                      
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x025A);	//                                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x02D2);	//                                      
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0249);	//                                      
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x02B9);	//                                       
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0238);	//                                      
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x02A2);	//                                       
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x021B);	//                                      
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0289);	//                                       
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0200);	//                                      
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x026C);	//                                       
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x01FC);	//                                      
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x024F);	//                                       
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x021E);	//                                      
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x022C);	//                                       
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                                      
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                                       
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                                      
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                                       
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                                      
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                                       
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                                      
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                                       
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0BAA);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0006);	//                                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0BAE);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x010E);	//                               
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x02E9);	//                               
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0BAC);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0009);	//                                 
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0B7A);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x038C);	//                                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x03DA);	//                                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x030E);	//                                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x03E9);	//                                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x02A2);	//                                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x03C2);	//                                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0259);	//                                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x038A);	//                                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0218);	//                                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0352);	//                                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x01F4);	//                                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x02E1);	//                                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x01D7);	//                                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x028E);	//                                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x01CB);	//                                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0258);	//                                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x022B);	//                                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x01CC);	//                                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                                      
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                                      
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0B70);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0005);	//                                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0B74);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x01F8);	//                                 
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x02A8);	//                                 
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0B72);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0007);	//                                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0B40);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x029E);	//                                      
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x02C8);	//                                       
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0281);	//                                      
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x02C8);	//                                       
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0266);	//                                      
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x02AC);	//                                       
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0251);	//                                      
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x028E);	//                                       
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x023D);	//                                      
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0275);	//                                       
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0228);	//                                      
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x025D);	//                                       
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0228);	//                                      
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0243);	//                                       
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                                      
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                                       
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                                      
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                                       
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                                      
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                                       
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                                       
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                                       
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0BC8);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0005);	//                                 
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0BCC);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x010F);	//                             
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x018F);	//                             
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0BCA);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0005);	//                               
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0BB4);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x03E7);	//                                  
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x03F8);	//                                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x03A7);	//                                  
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x03FC);	//                                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0352);	//                                  
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x03D0);	//                                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0322);	//                                  
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x039E);	//                                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x032B);	//                                  
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x034D);	//                                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0BE6);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0006);	//                                 
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0BEA);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x019E);	//                             
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0257);	//                             
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0BE8);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0004);	//                               
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0BD2);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x030B);	//                                  
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0323);	//                                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x02C3);	//                                  
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x030F);	//                                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0288);	//                                  
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x02E5);	//                                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x026A);	//                                  
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x02A2);	//                                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                                  
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0C2C);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0139);	//           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0122);	//           
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0BFC);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x03AD);	//                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x013F);	//                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0341);	//                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x017B);	//                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x038D);	//                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x014B);	//                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x02C3);	//                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x01CC);	//                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0241);	//                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x027F);	//                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0241);	//                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x027F);	//                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0214);	//                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x02A8);	//                   

	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0270);	//                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0210);	//                     

	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0C4C);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0452);	//                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0C58);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x059C);	//               
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0BF8);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x01AE);	//                       
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0C28);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0CAC);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0050);	//                 
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0C28);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0D0E);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00B8);	//                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00B2);	//                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0CFE);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0FAB);	//                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0FF5);	//                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x10BB);	//                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x1123);	//                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x1165);	//                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x122A);	//                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00A9);	//                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00C0);	//                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0CF8);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x030E);	//                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x034C);	//                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0388);	//                    

	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0CB0);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0064);	//                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0064);	//                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0078);	//                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0078);	//                     

	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0064);	//                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0064);	//                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0078);	//                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0078);	//                     

	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0064);	//                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0064);	//                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0078);	//                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0078);	//                     

	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFFC0);	//                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF60);	//                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF40);	//                     

	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFFC0);	//                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF60);	//                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF40);	//                     

	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFFC0);	//                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF60);	//                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF40);	//                     

	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0D30);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0002);	//                

	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x3372);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0001);	//                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                       
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                       


	//                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0C86);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0005);	//                                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0C70);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF7B);	//                                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00CE);	//                                             
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF23);	//                                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x010D);	//                                             
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFEF3);	//                                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x012C);	//                                             
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFED7);	//                                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x014E);	//                                             
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFEBB);	//                                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0162);	//                                             
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x1388);	//                                              
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0C8A);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x4ACB);	//                                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0C88);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0A7C);	//                                         

	//                                                                                  
	//                 
	//                                                                                  
	//                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x012E);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x5DC0);	//           
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0146);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0002); //                          

	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x014C);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x2CEC);//                              
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0152);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x59D8);//                                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x014E);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x59D8);//                                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0154);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x2981); //                              
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x015A);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x5208); //                                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0156);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x53FC); //                                   

	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0164); //          
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0001);
	//                                                                                 
	//                          
	//                                                                                 
	S5K8AAYX_MIPI_write_cmos_sensor(0x0028,0x7000);
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x03F4);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0001); //                               
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0001);	//                               
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0408);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x067F);	//                                      

	//                                                                                 
	//              
	//                                                                                 

	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0D40);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x003E); //                 
	//                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0D46);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x000F);	//            

	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0440);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x3410);	//              
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0444);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x6590);	//              
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0448);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xBB80);	//              
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x044C);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x3880);	//              
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0001);
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0450);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x3410);	//                 
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0454);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x6590);	//                 
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0458);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xBB80);	//                 
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x045C);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x3880);	//                 
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0001);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x01B0);	//                 
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x01B0);	//                 
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0280);	//                 
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0800);	//                 
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0100);	//               
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x3000);	//               
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x042E);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x010E);	//               
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00F5);	//             
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0DE0);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0002);	//                                    

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

	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0D38); //                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0198);
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0D3E); //                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x02A0);

	//                                                                                 
	//                      
	//                                                                                 
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0D4E);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                  
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0101); //                  
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0101); //                  
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                  
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0201); //                  
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0202); //                  
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0202); //                  
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0102); //                  
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0201); //                  
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0303); //                  
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0303); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0102); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0201); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0403); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0304); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0102); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0201); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0403); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0304); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0102); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0201); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0303); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0303); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0102); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0201); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0202); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0202); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0102); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0101); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0101); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                   

	//                                                                                 
	//                 
	//                                                                                 

	//                                                                                 
	//               
	//                                                                                 

	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x33A4);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x01D0);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFFA1);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFFFA);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF6F);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0140);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF49);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFFC1);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x001F);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x01BD);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x013F);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00E1);	//                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF43);	//                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0191);	//                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFFC0);	//                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x01B7);	//                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF30);	//                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x015F);	//                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0106);	//                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x01D0);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFFA1);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFFFA);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF6F);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0140);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF49);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFFC1);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x001F);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x01BD);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x013F);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00E1);	//                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF43);	//                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0191);	//                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFFC0);	//                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x01B7);	//                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF30);	//                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x015F);	//                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0106);	//                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x01D0);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFFA1);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFFFA);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF6F);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0140);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF49);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFFE3);	//                             
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFFF9);	//                             
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x01C1);	//                             
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x013F);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00E1);	//                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF43);	//                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0191);	//                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFFC0);	//                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x01B7);	//                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF30);	//                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x015F);	//                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0106);	//                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x01D0);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFFA1);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFFFA);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF6F);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0140);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF49);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFFE3);	//                             
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFFF9);	//                             
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x01C1);	//                             
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x013F);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00E1);	//                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF43);	//                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0191);	//                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFFC0);	//                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x01B7);	//                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF30);	//                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x015F);	//                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0106);	//                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x01BF);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFFBF);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFFFE);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF6D);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x01B4);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF66);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFFCA);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFFCE);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x017B);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0136);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0132);	//                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF85);	//                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x018B);	//                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF73);	//                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0191);	//                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF3F);	//                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x015B);	//                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00D0);	//                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x01BF);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFFBF);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFFFE);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF6D);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x01B4);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF66);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFFCA);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFFCE);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x017B);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0136);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0132);	//                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF85);	//                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x018B);	//                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF73);	//                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0191);	//                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF3F);	//                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x015B);	//                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00D0);	//                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x3380);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x01AC);	//                       
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFFD7);	//                       
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0019);	//                       
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF49);	//                       
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x01D9);	//                       
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF63);	//                       
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFFCA);	//                       
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFFCE);	//                       
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x017B);	//                       
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0132);	//                       
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x012E);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF8D);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x018B);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF73);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0191);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF3F);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x015B);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00D0);	//                        
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0612);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x009D);	//                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00D5);	//                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0103);	//                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0128);	//                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0166);	//                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0193);	//                   

	//                                                                                 
	//         
	//                                                                                 
	//                               
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0538);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                             
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x001F);	//                             
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0035);	//                             
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x005A);	//                             
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0095);	//                             
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00E6);	//                             
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0121);	//                             
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0139);	//                             
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0150);	//                             
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0177);	//                             
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x019A);	//                              
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x01BB);	//                              
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x01DC);	//                              
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0219);	//                              
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0251);	//                              
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x02B3);	//                              
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x030A);	//                              
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x035F);	//                              
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x03B1);	//                              
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x03FF);	//                              
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                              
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0001);	//                              
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0001);	//                              
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0002);	//                              
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0004);	//                              
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x000A);	//                              
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0012);	//                              
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0016);	//                              
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x001A);	//                              
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0024);	//                              
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0031);	//                               
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x003E);	//                               
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x004E);	//                               
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0075);	//                               
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00A8);	//                               
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0126);	//                               
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x01BE);	//                               
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0272);	//                               
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0334);	//                               
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x03FF);	//                               

	//                      

	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0498);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0002);	//                                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0007);	//                                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x001D);	//                                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x006E);	//                                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00D3);	//                                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0127);	//                                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x014C);	//                                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x016E);	//                                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x01A5);	//                                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x01D3);	//                                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x01FB);	//                                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x021F);	//                                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0260);	//                                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x029A);	//                                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x02F7);	//                                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x034D);	//                                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0395);	//                                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x03CE);	//                                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x03FF);	//                                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0004);	//                                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x000C);	//                                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0024);	//                                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x006E);	//                                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00D1);	//                                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0119);	//                                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0139);	//                                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0157);	//                                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x018E);	//                                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x01C3);	//                                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x01F3);	//                                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x021F);	//                                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0269);	//                                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x02A6);	//                                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x02FF);	//                                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0351);	//                                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0395);	//                                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x03CE);	//                                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x03FF);	//                                     

	//                                                                                 
	//        
	//                                                                                 
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x3370);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                       

	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x06D4);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0032);	//                       
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0078);	//                       
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00C8);	//                       
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0190);	//                       
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x028C);	//                       

	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0734);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                                  
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                                
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0078); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x012C); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x03FF); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0014); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0064); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x000C); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0010); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x01E6); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0070); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x01FF); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0144); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x000F); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x000A); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0073); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0087); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0014); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x000A); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0023); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x001E); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0014); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x000A); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0023); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0046); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x2B32); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0601); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00FF); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x07FF); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFFFF); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x050D); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x1E80); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x1408); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0214); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF01); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x180F); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0001); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x8003); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0094); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0580); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0280); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0308); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x3186); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x5260); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0A02); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x080A); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0500); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x032D); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x324E); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x001E); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0200); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0103); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x010C); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x9696); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x4646); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0802); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0802); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x030F); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x3202); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0F1E); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x020F); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0103); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x010C); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x9696); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x4646); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0802); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0802); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x030F); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x3202); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0F1E); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x020F); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0003); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                                  
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                                
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x006A); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x012C); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x03FF); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0014); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0064); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x000C); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0010); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x01E6); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x03FF); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0070); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x007D); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0064); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0014); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x000A); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0073); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0087); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0014); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x000A); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0023); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x001E); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0014); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x000A); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0023); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x001E); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x2B32); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0601); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00FF); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x07FF); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFFFF); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x050D); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x1E80); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x1408); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0214); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF01); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x180F); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0002); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x8003); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0080); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0080); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0280); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0308); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x1E65); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x1A24); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0A03); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x080A); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0500); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x032D); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x324D); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x001E); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0200); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0103); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x010C); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x9696); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x2F34); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0504); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x080F); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x030F); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x3208); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0F1E); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x020F); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0103); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x010C); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x9696); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x1414); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0504); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x080F); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x030F); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x3208); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0F1E); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x020F); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0003); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                                  
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                                
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0064); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x012C); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x03FF); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0014); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0064); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x000C); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0010); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x01E6); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x03FF); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0070); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x007D); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0064); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0096); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0096); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0073); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0087); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0014); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0019); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0023); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x001E); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0014); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0019); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0023); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x001E); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x2B32); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0601); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00FF); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x07FF); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFFFF); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x050D); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x1E80); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0A08); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0200); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF01); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x180F); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0002); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x8003); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0080); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0080); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0280); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0208); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x1E4B); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x1A24); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0A05); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x080A); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0500); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x032D); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x324D); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x001E); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0200); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0103); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x010C); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x9696); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x1E23); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0505); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x080F); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x030F); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x3208); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0F1E); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x020F); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0103); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x010C); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x9696); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x1E23); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0505); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x080F); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x030F); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x3208); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0F1E); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x020F); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0003); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                                  
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0018); //                                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0064); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x012C); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x03FF); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0014); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0064); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x000C); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0010); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x01E6); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0070); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x007D); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0064); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0096); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0096); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0073); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x009F); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0028); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0028); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0023); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0037); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0028); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0028); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0023); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0037); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x2B32); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0601); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00FF); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x07A0); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFFFF); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x050D); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x1E80); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0A08); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0200); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF01); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x180F); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0001); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x8003); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0080); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0080); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0280); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0108); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x1E32); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x1A24); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0A05); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x080A); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0328); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x324C); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x001E); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0200); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0103); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x010C); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x9696); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0F0F); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0307); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x080F); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x030F); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x3208); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0F1E); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x020F); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0103); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x010C); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x9696); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0F0F); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0307); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x080F); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x030F); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x3208); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0F1E); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x020F); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0003); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                                  
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0014); //                                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0028); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x012C); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x03FF); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0014); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0064); //                   
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x000C); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0010); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x01E6); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0070); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0087); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0073); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0096); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0096); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0073); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00B4); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0028); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0028); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0023); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0046); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0028); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0028); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0023); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0046); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x2B23); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0601); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x00FF); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0B84); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFFFF); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x050D); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x1E80); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0A08); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0200); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFF01); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x180F); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0001); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x8003); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0080); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0080); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0280); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0108); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x1E1E); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x1419); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0A0A); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0800); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0328); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x324C); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x001E); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0200); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0103); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x010C); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x6464); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0F0F); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0307); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x080F); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x030F); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x3208); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0F1E); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x020F); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0103); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x010C); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x6464); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0F0F); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0307); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x080F); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x030F); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x3208); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0F1E); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x020F); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0003); //                    
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x7F5E);	//                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xFEEE);	//                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0xD9B7);	//                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0472);	//                     
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0001);	//                     

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
		S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x01BE);
		S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0500); //                    
		S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x03C0); //                     
		S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0005); //                   
		S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0062); //                    
		S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0100); //                    
		S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                      
		S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x01D2);
		S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                         
		S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0002); //                              
		S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);//                                               
		S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x029A);//                                                            
		S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x01E8);
		S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                        
		S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //                            

		//                                                                                  
		//                              
		//                                                                                  
		S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x01A8);
		S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);//                                 
		S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x01AA);
		S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0001);//                                  
		S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x019E);
		S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0001);//                           
		S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0001);//                                  
		S5K8AAYX_MIPI_write_cmos_sensor(0x0028,0xD000);
		S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x1000);
		S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0001); //                   

		S5K8AAYX_MIPI_write_cmos_sensor(0x0028,0xD000);
		S5K8AAYX_MIPI_write_cmos_sensor(0x1000,0x0001);

#else
	S5K8AAYX_MIPI_write_init_setting(s5k8aayx_init_1,ARRAY_SIZE(s5k8aayx_init_1));
	S5K8AAYX_MIPI_write_init_setting(s5k8aayx_init_2,ARRAY_SIZE(s5k8aayx_init_2));
	S5K8AAYX_MIPI_write_init_setting(s5k8aayx_init_3,ARRAY_SIZE(s5k8aayx_init_3));
	S5K8AAYX_MIPI_write_init_setting(s5k8aayx_init_4,ARRAY_SIZE(s5k8aayx_init_4));
	S5K8AAYX_MIPI_write_init_setting(s5k8aayx_init_5,ARRAY_SIZE(s5k8aayx_init_5));
	S5K8AAYX_MIPI_write_init_setting(s5k8aayx_init_6,ARRAY_SIZE(s5k8aayx_init_6));
	S5K8AAYX_MIPI_write_init_setting(s5k8aayx_init_7,ARRAY_SIZE(s5k8aayx_init_7));
	S5K8AAYX_MIPI_write_init_setting(s5k8aayx_init_8,ARRAY_SIZE(s5k8aayx_init_8));
	S5K8AAYX_MIPI_write_init_setting(s5k8aayx_init_9,ARRAY_SIZE(s5k8aayx_init_9));
	S5K8AAYX_MIPI_write_init_setting(s5k8aayx_init_10,ARRAY_SIZE(s5k8aayx_init_10));
	S5K8AAYX_MIPI_write_init_setting(s5k8aayx_init_11,ARRAY_SIZE(s5k8aayx_init_11));
#endif

	SENSORDB("S5K8AAYX_MIPI_Initialize_Setting end\n");

}

/*                                                                            
           
                         
              
  
             
        
          
        
                                                                             */
void S5K8AAYX_MIPI_PV_Mode(void)
{

}



/*                                                                            
           
                          
              
  
             
        
          
        
                                                                             */

void S5K8AAYX_MIPI_CAP_Mode(void)
{
//
}

static void S5K8AAYX_MIPI_set_AE_mode(kal_bool AE_enable)
{
     if(AE_enable==KAL_TRUE)
     {
               S5K8AAYX_MIPI_write_cmos_sensor(0xFCFC, 0xD000); //         
               S5K8AAYX_MIPI_write_cmos_sensor(0x0028, 0x7000); //            

               S5K8AAYX_MIPI_write_cmos_sensor(0x002A, 0x214A);
               S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, 0x0001);
     }
     else
     {
               S5K8AAYX_MIPI_write_cmos_sensor(0xFCFC, 0xD000); //         
               S5K8AAYX_MIPI_write_cmos_sensor(0x0028, 0x7000); //            

               S5K8AAYX_MIPI_write_cmos_sensor(0x002A, 0x214A);
               S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, 0x0000);
     }

}


/*                                                                        
          
                          
 
             
                                            
 
            
      
 
         
      
 
                  
 
                                                                        */
void S5K8AAYX_MIPI_night_mode(kal_bool enable)
{
    //                                                   
    //           

	if (S5K8AAYX_MIPI_sensor_cap_state == KAL_TRUE)
		return ;

	//                                                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0028, 0x7000); //            
	if (enable)
	{
		if (S5K8AAYX_MIPI_MPEG4_encode_mode == KAL_TRUE)
		{
			S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x01D2);
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0001);	//                         
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0002);	//                              
			S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x01D6);
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,S5K8AAYX_MIPI_VID_NIT_FIX_FR_TIME); //                                  
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,S5K8AAYX_MIPI_VID_NIT_FIX_FR_TIME); //                                      
		}
		else
		{
			S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x01D2);
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                         
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0002);	//                              

			S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x01D6);
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,S5K8AAYX_MIPI_CAM_NIT_MIN_FR_TIME); //                                  
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,S5K8AAYX_MIPI_CAM_NIT_MAX_FR_TIME);	//                                  
		}
		S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0468);
        S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0140);  //              
	}
	else
	{
		if (S5K8AAYX_MIPI_MPEG4_encode_mode == KAL_TRUE)
		{
			S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x01D2);
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0001);	//                         
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0002);	//                              
			S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x01D6);
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, S5K8AAYX_MIPI_VID_NOM_FIX_FR_TIME); //                                   
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, S5K8AAYX_MIPI_VID_NOM_FIX_FR_TIME); //                                   
		}
		else
		{
			S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x01D2);
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                         
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0002);	//                              

			S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x01D6);
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,S5K8AAYX_MIPI_CAM_NOM_MIN_FR_TIME); //                                  
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,S5K8AAYX_MIPI_CAM_NOM_MAX_FR_TIME);	//                                  
		}

		S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x0468);
		S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0100);	//              
	}

	//                         
	//                                                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x01A8);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                                                              
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x01AC);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0001);	//                               
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x01A6);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0001);	//                                                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x01AA);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0001);	//                             
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x019E);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0001);	//                                          
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0001);	//                                

	spin_lock(&s5k8aayxmipi_drv_lock);
    S5K8AAYX_MIPICurrentStatus.iNightMode = enable;
	spin_unlock(&s5k8aayxmipi_drv_lock);
}	/*                          */

/*                                                                        
          
                           
 
             
                                 
 
            
      
 
         
      
 
                  
 
                                                                        */
static kal_uint32 S5K8AAYX_MIPI_GetSensorID(kal_uint32 *sensorID)
{
   //                        
	kal_uint16 sensor_id=0;//               

     //                           
	 S5K8AAYX_MIPI_write_cmos_sensor(0xFCFC, 0x0000);
	 sensor_id=S5K8AAYX_MIPI_read_cmos_sensor(0x0040);

	 SENSORDB("Sensor Read S5K8AAYX_MIPI ID_use_FCFC %x \r\n",sensor_id);

	 *sensorID=sensor_id;
	if (sensor_id != S5K8AAYX_MIPI_SENSOR_ID)
	{
	    *sensorID=0xFFFFFFFF;
	    SENSORDB("Sensor Read ByeBye \r\n");
		return ERROR_SENSOR_CONNECT_FAIL;
	}
    return ERROR_NONE;
}


/*                                                                           */
/*                                 */
/*                                                                           */
/*                                                                        
          
                   
 
             
                                                       
 
            
      
 
         
      
 
                  
 
                                                                        */
UINT32 S5K8AAYX_MIPIOpen(void)
{
	//                       
	kal_uint16 sensor_id=0;

	 S5K8AAYX_MIPI_write_cmos_sensor(0xFCFC, 0x0000);
	 sensor_id=S5K8AAYX_MIPI_read_cmos_sensor(0x0040);
	 SENSORDB("Sensor Read S5K8AAYX_MIPI ID %x \r\n",sensor_id);

	if (sensor_id != S5K8AAYX_MIPI_SENSOR_ID)
	{
	    SENSORDB("Sensor Read ByeBye \r\n");
		return ERROR_SENSOR_CONNECT_FAIL;
	}

    S5K8AAYX_MIPIInitialPara();
	S5K8AAYX_MIPI_Initialize_Setting();
	return ERROR_NONE;
}

/*                                                                        
          
                    
 
             
                                                   
 
            
      
 
         
      
 
                  
 
                                                                        */
UINT32 S5K8AAYX_MIPIClose(void)
{
	return ERROR_NONE;
}	/*                      */

/*                                                                        
          
                      
 
             
                                         
 
            
                                                                         
                                                                               
 
         
      
 
                  
 
                                                                        */
UINT32 S5K8AAYX_MIPIPreview(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
					  MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{

	spin_lock(&s5k8aayxmipi_drv_lock);
	S5K8AAYX_MIPI_sensor_cap_state = KAL_FALSE;
	S5K8AAYX_MIPI_PV_dummy_lines = 0;

	if(sensor_config_data->SensorOperationMode==MSDK_SENSOR_OPERATION_MODE_VIDEO)		//                  
	{
		S5K8AAYX_MIPI_MPEG4_encode_mode = KAL_TRUE;
		S5K8AAYX_MIPI_MJPEG_encode_mode = KAL_FALSE;
	}
	else
	{
		S5K8AAYX_MIPI_MPEG4_encode_mode = KAL_FALSE;
		S5K8AAYX_MIPI_MJPEG_encode_mode = KAL_FALSE;

	}
	spin_unlock(&s5k8aayxmipi_drv_lock);

	S5K8AAYX_MIPI_PV_Mode();
	S5K8AAYX_MIPI_night_mode(S5K8AAYX_MIPICurrentStatus.iNightMode);
	S5K8AAYX_MIPI_set_mirror(sensor_config_data->SensorImageMirror);

    image_window->ExposureWindowWidth = S5K8AAYX_MIPI_IMAGE_SENSOR_PV_WIDTH;
    image_window->ExposureWindowHeight = S5K8AAYX_MIPI_IMAGE_SENSOR_PV_HEIGHT;

	//                        
	memcpy(&S5K8AAYX_MIPISensorConfigData, sensor_config_data, sizeof(MSDK_SENSOR_CONFIG_STRUCT));
  	return ERROR_NONE;
}	/*                        */

UINT32 S5K8AAYX_MIPICapture(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
                                                 MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
     //                                                       
     //                                    
     //                           
     //                            

	spin_lock(&s5k8aayxmipi_drv_lock);
	S5K8AAYX_MIPI_sensor_cap_state = KAL_TRUE;
	spin_unlock(&s5k8aayxmipi_drv_lock);

    S5K8AAYX_MIPI_CAP_Mode();

    image_window->GrabStartX = S5K8AAYX_MIPI_IMAGE_SENSOR_FULL_INSERTED_PIXELS;
    image_window->GrabStartY = S5K8AAYX_MIPI_IMAGE_SENSOR_FULL_INSERTED_LINES;
    image_window->ExposureWindowWidth= S5K8AAYX_MIPI_IMAGE_SENSOR_FULL_WIDTH;
    image_window->ExposureWindowHeight = S5K8AAYX_MIPI_IMAGE_SENSOR_FULL_HEIGHT;

     //                        
     memcpy(&S5K8AAYX_MIPISensorConfigData, sensor_config_data, sizeof(MSDK_SENSOR_CONFIG_STRUCT));
     return ERROR_NONE;
}        /*                        */

UINT32 S5K8AAYX_MIPIGetResolution(MSDK_SENSOR_RESOLUTION_INFO_STRUCT *pSensorResolution)
{
	pSensorResolution->SensorFullWidth=S5K8AAYX_MIPI_IMAGE_SENSOR_FULL_WIDTH;  //               
	pSensorResolution->SensorFullHeight=S5K8AAYX_MIPI_IMAGE_SENSOR_FULL_HEIGHT;
	pSensorResolution->SensorPreviewWidth=S5K8AAYX_MIPI_IMAGE_SENSOR_PV_WIDTH;
	pSensorResolution->SensorPreviewHeight=S5K8AAYX_MIPI_IMAGE_SENSOR_PV_HEIGHT;

	pSensorResolution->SensorVideoWidth=S5K8AAYX_MIPI_IMAGE_SENSOR_PV_WIDTH;
	pSensorResolution->SensorVideoHeight=S5K8AAYX_MIPI_IMAGE_SENSOR_PV_HEIGHT;

	return ERROR_NONE;
}	/*                              */

UINT32 S5K8AAYX_MIPIGetInfo(MSDK_SCENARIO_ID_ENUM ScenarioId,
					  MSDK_SENSOR_INFO_STRUCT *pSensorInfo,
					  MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData)
{

    switch(ScenarioId)
    {
    	case MSDK_SCENARIO_ID_CAMERA_ZSD:
			 pSensorInfo->SensorPreviewResolutionX=S5K8AAYX_MIPI_IMAGE_SENSOR_FULL_WIDTH;
	         pSensorInfo->SensorPreviewResolutionY=S5K8AAYX_MIPI_IMAGE_SENSOR_FULL_HEIGHT;
			 pSensorInfo->SensorCameraPreviewFrameRate=15;
			 break;

		default:
			 pSensorInfo->SensorPreviewResolutionX=S5K8AAYX_MIPI_IMAGE_SENSOR_PV_WIDTH;
	         pSensorInfo->SensorPreviewResolutionY=S5K8AAYX_MIPI_IMAGE_SENSOR_PV_HEIGHT;
			 pSensorInfo->SensorCameraPreviewFrameRate=30;
    }
	pSensorInfo->SensorFullResolutionX=S5K8AAYX_MIPI_IMAGE_SENSOR_FULL_WIDTH;
	pSensorInfo->SensorFullResolutionY=S5K8AAYX_MIPI_IMAGE_SENSOR_FULL_HEIGHT;
	pSensorInfo->SensorCameraPreviewFrameRate=30;
	pSensorInfo->SensorVideoFrameRate=30;
	pSensorInfo->SensorStillCaptureFrameRate=10;
	pSensorInfo->SensorWebCamCaptureFrameRate=15;
	pSensorInfo->SensorResetActiveHigh=FALSE;
	pSensorInfo->SensorResetDelayCount=1;

	pSensorInfo->SensorOutputDataFormat=SENSOR_OUTPUT_FORMAT_YUYV;

	pSensorInfo->SensorClockPolarity=SENSOR_CLOCK_POLARITY_LOW;
	pSensorInfo->SensorClockFallingPolarity=SENSOR_CLOCK_POLARITY_LOW;

	pSensorInfo->SensorVsyncPolarity = SENSOR_CLOCK_POLARITY_HIGH;
	pSensorInfo->SensorHsyncPolarity = SENSOR_CLOCK_POLARITY_LOW;
	pSensorInfo->SensorInterruptDelayLines = 1;

	#ifdef MIPI_INTERFACE
   		pSensorInfo->SensroInterfaceType        = SENSOR_INTERFACE_TYPE_MIPI;
   	#else
   		pSensorInfo->SensroInterfaceType		= SENSOR_INTERFACE_TYPE_PARALLEL;
   	#endif

	pSensorInfo->SensorMasterClockSwitch = 0;
    pSensorInfo->SensorDrivingCurrent = ISP_DRIVING_8MA;//                
	pSensorInfo->CaptureDelayFrame = 2;
	pSensorInfo->PreviewDelayFrame = 4;
	pSensorInfo->VideoDelayFrame = 4;

	pSensorInfo->YUVAwbDelayFrame = 4;
	pSensorInfo->YUVEffectDelayFrame = 4;

	switch (ScenarioId)
	{
		case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
		case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
			pSensorInfo->SensorClockFreq=24;

			pSensorInfo->SensorClockDividCount=	3;
			pSensorInfo->SensorClockRisingCount= 0;
			pSensorInfo->SensorClockFallingCount= 2;
			pSensorInfo->SensorPixelClockCount= 3;

			pSensorInfo->SensorDataLatchCount= 2;

			pSensorInfo->SensorGrabStartX = 2;
			pSensorInfo->SensorGrabStartY = 2;
#ifdef MIPI_INTERFACE
            pSensorInfo->SensorMIPILaneNumber = SENSOR_MIPI_1_LANE;
            pSensorInfo->MIPIDataLowPwr2HighSpeedTermDelayCount = 0;
	        pSensorInfo->MIPIDataLowPwr2HighSpeedSettleDelayCount = 0x20;
	        pSensorInfo->MIPICLKLowPwr2HighSpeedTermDelayCount = 0;
            pSensorInfo->SensorWidthSampling = 0;  //                
            pSensorInfo->SensorHightSampling = 0;   //                
            pSensorInfo->SensorPacketECCOrder = 1;
#endif
		break;

		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
		case MSDK_SCENARIO_ID_CAMERA_ZSD:
			pSensorInfo->SensorClockFreq=24;
			pSensorInfo->SensorClockDividCount=	3;
			pSensorInfo->SensorClockRisingCount= 0;
			pSensorInfo->SensorClockFallingCount= 2;
			pSensorInfo->SensorPixelClockCount= 3;
			pSensorInfo->SensorDataLatchCount= 2;
			pSensorInfo->SensorGrabStartX = 2;
			pSensorInfo->SensorGrabStartY = 2;
			#ifdef MIPI_INTERFACE
	            pSensorInfo->SensorMIPILaneNumber = SENSOR_MIPI_1_LANE;
	            pSensorInfo->MIPIDataLowPwr2HighSpeedTermDelayCount = 0;
		        pSensorInfo->MIPIDataLowPwr2HighSpeedSettleDelayCount = 0x20;
		        pSensorInfo->MIPICLKLowPwr2HighSpeedTermDelayCount = 0;
	            pSensorInfo->SensorWidthSampling = 0;  //                
	            pSensorInfo->SensorHightSampling = 0;   //                
	            pSensorInfo->SensorPacketECCOrder = 1;
	        	#endif
		break;

		default:
			pSensorInfo->SensorClockFreq=24;
			pSensorInfo->SensorClockDividCount=3;
			pSensorInfo->SensorClockRisingCount=0;
			pSensorInfo->SensorClockFallingCount=2;
			pSensorInfo->SensorPixelClockCount=3;
			pSensorInfo->SensorDataLatchCount=2;
			pSensorInfo->SensorGrabStartX = 2;
			pSensorInfo->SensorGrabStartY = 2;
		break;
	}
	memcpy(pSensorConfigData, &S5K8AAYX_MIPISensorConfigData, sizeof(MSDK_SENSOR_CONFIG_STRUCT));
	return ERROR_NONE;
}	/*                        */


UINT32 S5K8AAYX_MIPIControl(MSDK_SCENARIO_ID_ENUM ScenarioId, MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *pImageWindow,
					  MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData)
{
	switch (ScenarioId)
	{
		case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
		case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
			S5K8AAYX_MIPIPreview(pImageWindow, pSensorConfigData);
		break;

		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
			S5K8AAYX_MIPICapture(pImageWindow, pSensorConfigData);
		break;

		//                   
		case MSDK_SCENARIO_ID_CAMERA_ZSD:
			   S5K8AAYX_MIPICapture(pImageWindow, pSensorConfigData);
			break;
		//      

        default:
            return ERROR_INVALID_SCENARIO_ID;
	}
	return ERROR_NONE;
}	/*                        */

/*                 */
#if WINMO_USE
void S5K8AAYX_MIPIQuery(PMSDK_FEATURE_INFO_STRUCT pSensorFeatureInfo)
{
	MSDK_FEATURE_TYPE_RANGE_STRUCT *pFeatureRange;
	MSDK_FEATURE_TYPE_MULTI_SELECTION_STRUCT *pFeatureMultiSelection;
	switch (pSensorFeatureInfo->FeatureId)
	{
		case ISP_FEATURE_DSC_MODE:
			pSensorFeatureInfo->FeatureType = MSDK_FEATURE_TYPE_MULTI_SELECTION;
			pSensorFeatureInfo->FeatureSupported = (UINT8)(MSDK_SET_GET_FEATURE_SUPPORTED|MSDK_QUERY_CAMERA_SUPPORTED);
			pFeatureMultiSelection = (PMSDK_FEATURE_TYPE_MULTI_SELECTION_STRUCT)(&pSensorFeatureInfo->FeatureInformation.FeatureMultiSelection);
			pFeatureMultiSelection->TotalSelection = CAM_NO_OF_SCENE_MODE_MAX-2;
			pFeatureMultiSelection->DefaultSelection = CAM_AUTO_DSC_MODE;
			pFeatureMultiSelection->SupportedSelection =
				(CAMERA_FEATURE_SUPPORT(CAM_AUTO_DSC_MODE)|
				CAMERA_FEATURE_SUPPORT(CAM_NIGHTSCENE_MODE));
		break;
		case ISP_FEATURE_WHITEBALANCE:
			pSensorFeatureInfo->FeatureType = MSDK_FEATURE_TYPE_MULTI_SELECTION;
			pSensorFeatureInfo->FeatureSupported = (UINT8)(MSDK_SET_GET_FEATURE_SUPPORTED|MSDK_QUERY_CAMERA_VIDEO_SUPPORTED);
			pFeatureMultiSelection = (PMSDK_FEATURE_TYPE_MULTI_SELECTION_STRUCT)(&pSensorFeatureInfo->FeatureInformation.FeatureMultiSelection);
			pFeatureMultiSelection->TotalSelection = CAM_NO_OF_WB;
			pFeatureMultiSelection->DefaultSelection = CAM_WB_AUTO;
			pFeatureMultiSelection->SupportedSelection =
				(CAMERA_FEATURE_SUPPORT(CAM_WB_AUTO)|
				CAMERA_FEATURE_SUPPORT(CAM_WB_CLOUD)|
				CAMERA_FEATURE_SUPPORT(CAM_WB_DAYLIGHT)|
				CAMERA_FEATURE_SUPPORT(CAM_WB_INCANDESCENCE)|
				CAMERA_FEATURE_SUPPORT(CAM_WB_FLUORESCENT));
		break;
		case ISP_FEATURE_IMAGE_EFFECT:
			pSensorFeatureInfo->FeatureType = MSDK_FEATURE_TYPE_MULTI_SELECTION;
			pSensorFeatureInfo->FeatureSupported = (UINT8)(MSDK_SET_GET_FEATURE_SUPPORTED|MSDK_QUERY_CAMERA_VIDEO_SUPPORTED);
			pFeatureMultiSelection = (PMSDK_FEATURE_TYPE_MULTI_SELECTION_STRUCT)(&pSensorFeatureInfo->FeatureInformation.FeatureMultiSelection);
			pFeatureMultiSelection->TotalSelection = CAM_NO_OF_EFFECT_ENC;
			pFeatureMultiSelection->DefaultSelection = CAM_EFFECT_ENC_NORMAL;
			pFeatureMultiSelection->SupportedSelection =
				(CAMERA_FEATURE_SUPPORT(CAM_EFFECT_ENC_NORMAL)|
				CAMERA_FEATURE_SUPPORT(CAM_EFFECT_ENC_GRAYSCALE)|
				CAMERA_FEATURE_SUPPORT(CAM_EFFECT_ENC_COLORINV)|
				CAMERA_FEATURE_SUPPORT(CAM_EFFECT_ENC_GRAYINV)|
				CAMERA_FEATURE_SUPPORT(CAM_EFFECT_ENC_SEPIABLUE)|
				CAMERA_FEATURE_SUPPORT(CAM_EFFECT_ENC_SKETCH)|
				CAMERA_FEATURE_SUPPORT(CAM_EFFECT_ENC_EMBOSSMENT)|
				CAMERA_FEATURE_SUPPORT(CAM_EFFECT_ENC_SEPIA));
		break;
		case ISP_FEATURE_AE_METERING_MODE:
			pSensorFeatureInfo->FeatureSupported = MSDK_FEATURE_NOT_SUPPORTED;
		break;
		case ISP_FEATURE_BRIGHTNESS:
			pSensorFeatureInfo->FeatureType = MSDK_FEATURE_TYPE_RANGE;
			pSensorFeatureInfo->FeatureSupported = (UINT8)(MSDK_SET_GET_FEATURE_SUPPORTED|MSDK_QUERY_CAMERA_VIDEO_SUPPORTED);
			pFeatureRange = (PMSDK_FEATURE_TYPE_RANGE_STRUCT)(&pSensorFeatureInfo->FeatureInformation.FeatureRange);
			pFeatureRange->MinValue = CAM_EV_NEG_4_3;
			pFeatureRange->MaxValue = CAM_EV_POS_4_3;
			pFeatureRange->StepValue = CAMERA_FEATURE_ID_EV_STEP;
			pFeatureRange->DefaultValue = CAM_EV_ZERO;
		break;
		case ISP_FEATURE_BANDING_FREQ:
			pSensorFeatureInfo->FeatureType = MSDK_FEATURE_TYPE_MULTI_SELECTION;
			pSensorFeatureInfo->FeatureSupported = (UINT8)(MSDK_SET_GET_FEATURE_SUPPORTED|MSDK_QUERY_CAMERA_VIDEO_SUPPORTED);
			pFeatureMultiSelection = (PMSDK_FEATURE_TYPE_MULTI_SELECTION_STRUCT)(&pSensorFeatureInfo->FeatureInformation.FeatureMultiSelection);
			pFeatureMultiSelection->TotalSelection = CAM_NO_OF_BANDING;
			pFeatureMultiSelection->DefaultSelection = CAM_BANDING_50HZ;
			pFeatureMultiSelection->SupportedSelection =
				(CAMERA_FEATURE_SUPPORT(CAM_BANDING_50HZ)|
				CAMERA_FEATURE_SUPPORT(CAM_BANDING_60HZ));
		break;
		case ISP_FEATURE_AF_OPERATION:
			pSensorFeatureInfo->FeatureSupported = MSDK_FEATURE_NOT_SUPPORTED;
		break;
		case ISP_FEATURE_AF_RANGE_CONTROL:
			pSensorFeatureInfo->FeatureSupported = MSDK_FEATURE_NOT_SUPPORTED;
		break;
		case ISP_FEATURE_FLASH:
			pSensorFeatureInfo->FeatureSupported = MSDK_FEATURE_NOT_SUPPORTED;
		break;
		case ISP_FEATURE_VIDEO_SCENE_MODE:
			pSensorFeatureInfo->FeatureType = MSDK_FEATURE_TYPE_MULTI_SELECTION;
			pSensorFeatureInfo->FeatureSupported = (UINT8)(MSDK_SET_GET_FEATURE_SUPPORTED|MSDK_QUERY_VIDEO_SUPPORTED);
			pFeatureMultiSelection = (PMSDK_FEATURE_TYPE_MULTI_SELECTION_STRUCT)(&pSensorFeatureInfo->FeatureInformation.FeatureMultiSelection);
			pFeatureMultiSelection->TotalSelection = 2;
			pFeatureMultiSelection->DefaultSelection = CAM_VIDEO_AUTO_MODE;
			pFeatureMultiSelection->SupportedSelection =
				(CAMERA_FEATURE_SUPPORT(CAM_VIDEO_AUTO_MODE)|
				CAMERA_FEATURE_SUPPORT(CAM_VIDEO_NIGHT_MODE));
		break;
		case ISP_FEATURE_ISO:
			pSensorFeatureInfo->FeatureSupported = MSDK_FEATURE_NOT_SUPPORTED;
		break;
		default:
			pSensorFeatureInfo->FeatureSupported = MSDK_FEATURE_NOT_SUPPORTED;
		break;
	}
}
#endif

BOOL S5K8AAYX_MIPI_set_param_wb(UINT16 para)
{

	if(S5K8AAYX_MIPICurrentStatus.iWB == para)
		return TRUE;
	SENSORDB("[Enter]S5K8AAYX_MIPI set_param_wb func:para = %d\n",para);

	S5K8AAYX_MIPI_write_cmos_sensor(0xFCFC, 0xD000); //         
	S5K8AAYX_MIPI_write_cmos_sensor(0x0028, 0x7000); //            

	switch (para)
	{
		case AWB_MODE_AUTO:
			S5K8AAYX_MIPI_write_cmos_sensor(0x002A, 0x0408); //                          
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, 0x067F);
			break;

		case AWB_MODE_CLOUDY_DAYLIGHT:
			//                                                                      
			//                 
			//                                                                      
			S5K8AAYX_MIPI_write_cmos_sensor(0xFCFC, 0xD000);
			S5K8AAYX_MIPI_write_cmos_sensor(0x0028, 0x7000);
			S5K8AAYX_MIPI_write_cmos_sensor(0x002A, 0x0408);
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, 0x0677);
			S5K8AAYX_MIPI_write_cmos_sensor(0x002A, 0x03DA);
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, 0x0740);
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, 0x0001);
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, 0x0400);
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, 0x0001);
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, 0x0460);
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, 0x0001);
			break;

		case AWB_MODE_DAYLIGHT:
			//                                              
			//                       
			//                                              
			S5K8AAYX_MIPI_write_cmos_sensor(0xFCFC, 0xD000);
			S5K8AAYX_MIPI_write_cmos_sensor(0x0028, 0x7000);
			S5K8AAYX_MIPI_write_cmos_sensor(0x002A, 0x0408);
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, 0x0677);
			S5K8AAYX_MIPI_write_cmos_sensor(0x002A, 0x03DA);
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, 0x05E0);
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, 0x0001);
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, 0x0400);
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, 0x0001);
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, 0x0530);
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, 0x0001);
			break;

		case AWB_MODE_INCANDESCENT:
			//                                              
			//                      
			//                                              
			S5K8AAYX_MIPI_write_cmos_sensor(0xFCFC, 0xD000);
			S5K8AAYX_MIPI_write_cmos_sensor(0x0028, 0x7000);
			S5K8AAYX_MIPI_write_cmos_sensor(0x002A, 0x0408);
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, 0x0677);
			S5K8AAYX_MIPI_write_cmos_sensor(0x002A, 0x03DA);
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, 0x05C0); //                 
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, 0x0001); //                               
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, 0x0400); //                 
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, 0x0001); //                        
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, 0x08B0); //                 
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, 0x0001); //                        
			break;

		case AWB_MODE_FLUORESCENT:
			//                                                                  
			//                      
			//                                                                  
			S5K8AAYX_MIPI_write_cmos_sensor(0xFCFC, 0xD000);
			S5K8AAYX_MIPI_write_cmos_sensor(0x0028, 0x7000);
			S5K8AAYX_MIPI_write_cmos_sensor(0x002A, 0x0408);
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, 0x0677);
			S5K8AAYX_MIPI_write_cmos_sensor(0x002A, 0x03DA);
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, 0x0575);
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, 0x0001);
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, 0x0400);
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, 0x0001);
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, 0x0800);
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, 0x0001);
			break;

		case AWB_MODE_TUNGSTEN:
			S5K8AAYX_MIPI_write_cmos_sensor(0xFCFC, 0xD000);
			S5K8AAYX_MIPI_write_cmos_sensor(0x0028, 0x7000);
			S5K8AAYX_MIPI_write_cmos_sensor(0x002A, 0x0408);
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, 0x0677);

			S5K8AAYX_MIPI_write_cmos_sensor(0x002A, 0x03DA);
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, 0x0400);
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, 0x0001);
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, 0x0400);
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, 0x0001);
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, 0x0940);
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, 0x0001);
			break;
		default:
			return KAL_FALSE;

		}
		spin_lock(&s5k8aayxmipi_drv_lock);
	    S5K8AAYX_MIPICurrentStatus.iWB = para;
		spin_unlock(&s5k8aayxmipi_drv_lock);
return TRUE;
}

BOOL S5K8AAYX_MIPI_set_param_effect(UINT16 para)
{
	/*                                                                */
	   /*                             */
	   /*                                                                */
	   kal_uint32 ret = KAL_TRUE;

	   /*                                                                */
	   /*                         */
	   /*                                                                */

	   if(S5K8AAYX_MIPICurrentStatus.iEffect == para)
		  return TRUE;
	   SENSORDB("[Enter]s5k8aayxmipi set_param_effect func:para = %d\n",para);

	   S5K8AAYX_MIPI_write_cmos_sensor(0x0028,0x7000);
	   S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x019C);
	   switch (para)
	   {
		   case MEFFECT_OFF:
			   S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000); //        
			   break;
		   case MEFFECT_MONO:
			   S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0001); //                           
			   break;
		   case MEFFECT_SEPIA:
			   S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0004); //      
			   break;
		   case MEFFECT_SEPIABLUE:
			   S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0005); //            
			   break;
		   case MEFFECT_NEGATIVE:
			   S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0003); //               
		   default:
			   ret = KAL_FALSE;
	   }

	   spin_lock(&s5k8aayxmipi_drv_lock);
	   S5K8AAYX_MIPICurrentStatus.iEffect = para;
	   spin_unlock(&s5k8aayxmipi_drv_lock);
	   return ret;
}

void S5K8AAYX_MIPIGetAEAWBLock(UINT32 *pAElockRet32,UINT32 *pAWBlockRet32)
{
    *pAElockRet32 = 1;
	*pAWBlockRet32 = 1;
    SENSORDB("S5K8AAYX_MIPIGetAEAWBLock,AE=%d ,AWB=%d\n,",*pAElockRet32,*pAWBlockRet32);
}


BOOL S5K8AAYX_MIPI_set_param_banding(UINT16 para)
{
    /*                                                                */
    /*                                                                */
    /*                                                                */
    /*                                                                */
    /*                                                                */
    /*                                                                */

#if (defined(S5K8AAYX_MIPI_MANUAL_ANTI_FLICKER))

	if(S5K8AAYX_MIPICurrentStatus.iBanding == para)
      return TRUE;

    switch (para)
	{
		case AE_FLICKER_MODE_50HZ:
				S5K8AAYX_MIPI_write_cmos_sensor(0x0028,0x7000);
				S5K8AAYX_MIPI_write_cmos_sensor(0x002a,0x0408);
				S5K8AAYX_MIPI_write_cmos_sensor(0x0f12,0x065F);
				S5K8AAYX_MIPI_write_cmos_sensor(0x002a,0x03F4);
				S5K8AAYX_MIPI_write_cmos_sensor(0x0f12,0x0001); //                                       
				S5K8AAYX_MIPI_write_cmos_sensor(0x0f12,0x0001); //                                              
			break;
		case AE_FLICKER_MODE_60HZ:
				S5K8AAYX_MIPI_write_cmos_sensor(0x0028,0x7000);
				S5K8AAYX_MIPI_write_cmos_sensor(0x002a,0x0408);
				S5K8AAYX_MIPI_write_cmos_sensor(0x0f12,0x065F);
				S5K8AAYX_MIPI_write_cmos_sensor(0x002a,0x03F4);
				S5K8AAYX_MIPI_write_cmos_sensor(0x0f12,0x0002); //                                       
				S5K8AAYX_MIPI_write_cmos_sensor(0x0f12,0x0001); //                                              
			break;
		default:
			return KAL_FALSE;
	}
	spin_lock(&s5k8aayxmipi_drv_lock);
    S5K8AAYX_MIPICurrentStatus.iBanding = para;
	spin_unlock(&s5k8aayxmipi_drv_lock);
	return TRUE;

#else
	/*                                                                                 */

#endif	/*                                                  */
	return KAL_TRUE;
} /*                                 */

BOOL S5K8AAYX_MIPI_set_param_exposure(UINT16 para)
{

	//                                          
	//             

	SENSORDB("[Enter]s5k8aayxmipi set_param_exposure func:para = %d\n",para);

	S5K8AAYX_MIPI_write_cmos_sensor(0x0028, 0x7000);
	S5K8AAYX_MIPI_write_cmos_sensor(0x002a, 0x019A);
    switch (para)
	{
		case AE_EV_COMP_n10:
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, 0x0080); //     
			break;
		case AE_EV_COMP_00:
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, 0x0100); //    
			break;
		case AE_EV_COMP_10:
			S5K8AAYX_MIPI_write_cmos_sensor(0x0F12, 0x0200);  //     
			break;

		case AE_EV_COMP_n13:
		case AE_EV_COMP_n07:
		case AE_EV_COMP_n03:
		case AE_EV_COMP_03:
		case AE_EV_COMP_07:
		case AE_EV_COMP_13:
			break;

		default:
			return FALSE;
	}
	spin_lock(&s5k8aayxmipi_drv_lock);
	S5K8AAYX_MIPICurrentStatus.iEV = para;
	spin_unlock(&s5k8aayxmipi_drv_lock);
	return TRUE;

}/*                                  */


BOOL S5K8AAYX_MIPI_set_contrast(UINT16 para)
{
	SENSORDB("[Enter]: S5K8AAYX_MIPI_set_contrast func:para = %d\n",para);

	S5K8AAYX_MIPI_write_cmos_sensor(0x0028, 0x7000);
	S5K8AAYX_MIPI_write_cmos_sensor(0x002a, 0x0190);

	switch (para)
	{
		case ISP_CONTRAST_LOW:
			 S5K8AAYX_MIPI_write_cmos_sensor(0x0f12, 0xFFE0);
			 break;

		case ISP_CONTRAST_HIGH:
			 S5K8AAYX_MIPI_write_cmos_sensor(0x0f12, 0x0020);
			 break;

		case ISP_CONTRAST_MIDDLE:
		default:
			 S5K8AAYX_MIPI_write_cmos_sensor(0x0f12, 0x0000);
			 break;
	}

	return KAL_TRUE;
}


BOOL S5K8AAYX_MIPI_set_brightness(UINT16 para)
{
	SENSORDB(" [Enter]: S5K8AAYX_MIPI_set_brightness func:para = %d\n",para);

	S5K8AAYX_MIPI_write_cmos_sensor(0x0028, 0x7000);
	S5K8AAYX_MIPI_write_cmos_sensor(0x002a, 0x018E);

	switch (para)
	{
		case ISP_BRIGHT_LOW:
			 S5K8AAYX_MIPI_write_cmos_sensor(0x0f12, 0xFFE0);
			 break;

		case ISP_BRIGHT_HIGH:
			 S5K8AAYX_MIPI_write_cmos_sensor(0x0f12, 0x0020);
			 break;

		case ISP_BRIGHT_MIDDLE:
		default:
			 S5K8AAYX_MIPI_write_cmos_sensor(0x0f12, 0x0000);
			 break;
	}

	return KAL_TRUE;
}



UINT32 S5K8AAYX_MIPIYUVSensorSetting(FEATURE_ID iCmd, UINT16 iPara)
{
	switch (iCmd) {
	case FID_SCENE_MODE:
	    if (iPara == SCENE_MODE_OFF)
	    {
	        S5K8AAYX_MIPI_night_mode(0);
	    }

         else if (iPara == SCENE_MODE_NIGHTSCENE)
	    {
            S5K8AAYX_MIPI_night_mode(1);
	    }
	    break;
	case FID_AWB_MODE:
         S5K8AAYX_MIPI_set_param_wb(iPara);
	break;
	case FID_COLOR_EFFECT:

         S5K8AAYX_MIPI_set_param_effect(iPara);
	break;
	case FID_AE_EV:
         S5K8AAYX_MIPI_set_param_exposure(iPara);
	break;
	case FID_AE_FLICKER:

         S5K8AAYX_MIPI_set_param_banding(iPara);
	break;
    case FID_AE_SCENE_MODE:
		spin_lock(&s5k8aayxmipi_drv_lock);
      if (iPara == AE_MODE_OFF) {
          S5K8AAYX_MIPI_AE_ENABLE = KAL_FALSE;
      }
      else {
          S5K8AAYX_MIPI_AE_ENABLE = KAL_TRUE;
      }
	  spin_unlock(&s5k8aayxmipi_drv_lock);
      S5K8AAYX_MIPI_set_AE_mode(S5K8AAYX_MIPI_AE_ENABLE);
    break;
	case FID_ZOOM_FACTOR:
		spin_lock(&s5k8aayxmipi_drv_lock);
	    zoom_factor = iPara;
		spin_unlock(&s5k8aayxmipi_drv_lock);
	break;

	case FID_ISP_CONTRAST:
		S5K8AAYX_MIPI_set_contrast(iPara);
		break;

	case FID_ISP_BRIGHT:
		S5K8AAYX_MIPI_set_brightness(iPara);
		break;

	default:
	break;
	}
	return ERROR_NONE;
}   /*                               */


UINT32 S5K8AAYX_MIPIYUVSetVideoMode(UINT16 u2FrameRate)
{

    if(S5K8AAYX_MIPICurrentStatus.iFrameRate == u2FrameRate)
      return ERROR_NONE;

	spin_lock(&s5k8aayxmipi_drv_lock);
    S5K8AAYX_MIPI_VEDIO_encode_mode = KAL_TRUE;
	S5K8AAYX_MIPI_MPEG4_encode_mode = KAL_TRUE;
	spin_unlock(&s5k8aayxmipi_drv_lock);

	//                                                            
	S5K8AAYX_MIPI_write_cmos_sensor(0x0028, 0x7000); //            

    if(20<=u2FrameRate && u2FrameRate<=30) //         
    {
		S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x01D2);
		S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0001);	//                         
		S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0002);	//                              
		S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x01D6);
		S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,S5K8AAYX_MIPI_VID_NOM_FIX_FR_TIME); //                                  
		S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,S5K8AAYX_MIPI_VID_NOM_FIX_FR_TIME); //                                      
    }
    else if(5<=u2FrameRate && u2FrameRate<20 )//          
    {
		S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x01D2);
		S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0001);	//                         
		S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0002);	//                              
		S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x01D6);
		S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,S5K8AAYX_MIPI_VID_NIT_FIX_FR_TIME); //                                  
		S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,S5K8AAYX_MIPI_VID_NIT_FIX_FR_TIME); //                                      
    }
    else
    {
        printk("Wrong Frame Rate \n");
    }

	//                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x01A8);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0000);	//                                                              
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x01AC);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0001);	//                               
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x01A6);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0001);	//                                                         
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x01AA);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0001);	//                             
	S5K8AAYX_MIPI_write_cmos_sensor(0x002A,0x019E);
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0001);	//                                          
	S5K8AAYX_MIPI_write_cmos_sensor(0x0F12,0x0001);	//                                
    return ERROR_NONE;
}


kal_uint16 S5K8AAYX_MIPIReadShutter_forASD(void)
{
   kal_uint16 temp_msb=0x0000,temp_lsb=0x0000;
   kal_uint32 temp=0x00000000;


   S5K8AAYX_MIPI_write_cmos_sensor(0x002c,0x7000);
   S5K8AAYX_MIPI_write_cmos_sensor(0x002e,0x16E2);//     

   temp_msb=S5K8AAYX_MIPI_read_cmos_sensor(0x0f12);
   temp_msb = (temp_msb<<16)&0xffff0000;

   S5K8AAYX_MIPI_write_cmos_sensor(0x002c,0x7000);
   S5K8AAYX_MIPI_write_cmos_sensor(0x002e,0x16E0);
   temp_lsb=S5K8AAYX_MIPI_read_cmos_sensor(0x0f12);
   temp_lsb = temp_lsb&0x0000ffff;

   temp = (temp_msb|temp_lsb)/400;

   temp = temp*72000/(S5K8AAYX_MIPI_IMAGE_SENSOR_PV_WIDTH*2);
   return temp;

}
kal_uint16 S5K8AAYX_MIPIReadGain_forASD(void)
{
    kal_uint16 temp=0x0000,Base_gain=64;
	S5K8AAYX_MIPI_write_cmos_sensor(0x002c,0x7000);
    S5K8AAYX_MIPI_write_cmos_sensor(0x002e,0x20D0);//     

	temp=S5K8AAYX_MIPI_read_cmos_sensor(0x0f12);
	temp = Base_gain*temp/256;

	return temp;
}
kal_uint16 S5K8AAYX_MIPIReadAwbRGain_forASD(void)
{
    kal_uint16 temp=0x0000,Base_gain=64;

	S5K8AAYX_MIPI_write_cmos_sensor(0x002c,0x7000);
    S5K8AAYX_MIPI_write_cmos_sensor(0x002e,0x20DC);

	temp=S5K8AAYX_MIPI_read_cmos_sensor(0x0f12);
	temp = Base_gain*temp/1024;
	return temp;
}
kal_uint16 S5K8AAYX_MIPIReadAwbBGain_forASD(void)
{
    kal_uint16 temp=0x0000,Base_gain=64;

	S5K8AAYX_MIPI_write_cmos_sensor(0x002c,0x7000);
    S5K8AAYX_MIPI_write_cmos_sensor(0x002e,0x20E0);

	temp=S5K8AAYX_MIPI_read_cmos_sensor(0x0f12);
	temp = Base_gain*temp/1024;

	return temp;
}

//                   

/*                                                                        
          
                             
 
             
         
         
 
                
 
                                                                        */
static void S5K8AAYX_MIPIGetEvAwbRef(UINT32 pSensorAEAWBRefStruct/*                             */)
{
    PSENSOR_AE_AWB_REF_STRUCT Ref = (PSENSOR_AE_AWB_REF_STRUCT)pSensorAEAWBRefStruct;
    SENSORDB("S5K8AAYX_MIPIGetEvAwbRef  \n" );

	Ref->SensorAERef.AeRefLV05Shutter = 5422;
    Ref->SensorAERef.AeRefLV05Gain = 478; /*          */
    Ref->SensorAERef.AeRefLV13Shutter = 80;
    Ref->SensorAERef.AeRefLV13Gain = 128; /*           */
    Ref->SensorAwbGainRef.AwbRefD65Rgain = 186; /*          */
    Ref->SensorAwbGainRef.AwbRefD65Bgain = 158; /*          */
    Ref->SensorAwbGainRef.AwbRefCWFRgain = 196; /*                 */
    Ref->SensorAwbGainRef.AwbRefCWFBgain = 278; /*                    */
}
/*                                                                        
          
                                 
 
             
         
         
 
                
 
                                                                        */
static void S5K8AAYX_MIPIGetCurAeAwbInfo(UINT32 pSensorAEAWBCurStruct/*                              */)
{
    PSENSOR_AE_AWB_CUR_STRUCT Info = (PSENSOR_AE_AWB_CUR_STRUCT)pSensorAEAWBCurStruct;
    SENSORDB("S5K8AAYX_MIPIGetCurAeAwbInfo  \n" );

    Info->SensorAECur.AeCurShutter = S5K8AAYX_MIPIReadShutter_forASD();
    Info->SensorAECur.AeCurGain = S5K8AAYX_MIPIReadGain_forASD() * 2; /*          */

    Info->SensorAwbGainCur.AwbCurRgain = S5K8AAYX_MIPIReadAwbRGain_forASD()<< 1; /*          */

    Info->SensorAwbGainCur.AwbCurBgain = S5K8AAYX_MIPIReadAwbBGain_forASD()<< 1; /*          */
}

//      

void S5K8AAYX_MIPIGetAFMaxNumFocusAreas(UINT32 *pFeatureReturnPara32)
{
    *pFeatureReturnPara32 = 0;
    SENSORDB("S5K8AAYX_MIPIGetAFMaxNumFocusAreas, *pFeatureReturnPara32 = %d\n",*pFeatureReturnPara32);

}

void S5K8AAYX_MIPIGetAFMaxNumMeteringAreas(UINT32 *pFeatureReturnPara32)
{
    *pFeatureReturnPara32 = 0;
    SENSORDB("S5K8AAYX_MIPIGetAFMaxNumMeteringAreas,*pFeatureReturnPara32 = %d\n",*pFeatureReturnPara32);

}

kal_uint16 S5K8AAYX_ReadShutter(void)
{
    kal_uint32 ExposureT = 0;

    S5K8AAYX_MIPI_write_cmos_sensor(0x002c,0x7000);
    S5K8AAYX_MIPI_write_cmos_sensor(0x002e,0x16E2);//     

    ExposureT = S5K8AAYX_MIPI_read_cmos_sensor(0x0f12);
    ExposureT <<= 16;

    S5K8AAYX_MIPI_write_cmos_sensor(0x002c,0x7000);
    S5K8AAYX_MIPI_write_cmos_sensor(0x002e,0x16E0);
    ExposureT |= S5K8AAYX_MIPI_read_cmos_sensor(0x0f12);

    //                                      
    ExposureT = ExposureT*5/2; //   

   //                                                             

   return ExposureT;

}

kal_uint16 S5K8AAYX_ReadISO(void)
{
    kal_uint16 temp=0x0000,AGainBase=256;

	S5K8AAYX_MIPI_write_cmos_sensor(0xFCFC,0xD000);
	S5K8AAYX_MIPI_write_cmos_sensor(0x002c,0x7000);
    S5K8AAYX_MIPI_write_cmos_sensor(0x002e,0x20E0);//     

	temp=S5K8AAYX_MIPI_read_cmos_sensor(0x0f12);
	temp =temp/AGainBase;

	SENSORDB("S5K8AAYX_ReadISO:AGain = %d\n",temp);

	if(temp < 2)
	{
		return AE_ISO_100;
	}
	else if(temp < 4)
	{
		return AE_ISO_200;
	}
    else
    {
		return AE_ISO_400;
	}

}
void S5K8AAYX_MIPIGetExifInfo(UINT32 exifAddr)
{
    SENSOR_EXIF_INFO_STRUCT* pExifInfo = (SENSOR_EXIF_INFO_STRUCT*)exifAddr;
    pExifInfo->FNumber = 28;
    pExifInfo->AEISOSpeed = AE_ISO_100;
    pExifInfo->AWBMode = S5K8AAYX_MIPICurrentStatus.iWB;
    pExifInfo->CapExposureTime = S5K8AAYX_ReadShutter();
    pExifInfo->FlashLightTimeus = 0;
    pExifInfo->RealISOValue = S5K8AAYX_ReadISO();
}

void S5K8AAYX_MIPIGetDelayInfo(UINT32 delayAddr)
{
    SENSOR_DELAY_INFO_STRUCT* pDelayInfo = (SENSOR_DELAY_INFO_STRUCT*)delayAddr;
    pDelayInfo->InitDelay = 4;
    pDelayInfo->EffectDelay = 4;
    pDelayInfo->AwbDelay = 4;
}

void S5K8AAYX_MIPIAutoTestCmd(UINT32 *cmd, UINT32 *para)
{
    switch(*cmd){
        case YUV_AUTOTEST_SET_SHADDING:
            SENSORDB("YUV_AUTOTEST_SET_SHADDING:para = %d\n",*para);
        break;
        case YUV_AUTOTEST_SET_GAMMA:
            SENSORDB("YUV_AUTOTEST_SET_GAMMA:para = %d\n",*para);
        break;
        case YUV_AUTOTEST_SET_AE:
            SENSORDB("YUV_AUTOTEST_SET_AE:para = %d\n",*para);
        break;
        case YUV_AUTOTEST_SET_SHUTTER:
            SENSORDB("YUV_AUTOTEST_SET_SHUTTER:para = %d\n",*para);
        break;
        case YUV_AUTOTEST_SET_GAIN:
            SENSORDB("YUV_AUTOTEST_SET_GAIN:para = %d\n",*para);
        break;
        case YUV_AUTOTEST_GET_SHUTTER_RANGE:
            *para = 8228;
        break;
        default:
            SENSORDB("YUV AUTOTEST NOT SUPPORT CMD:%d\n",*cmd);
        break;
	}
}

UINT32 S5K8AAYXMIPISetMaxFramerateByScenario(MSDK_SCENARIO_ID_ENUM scenarioId, MUINT32 frameRate) {
	kal_uint32 pclk;
	kal_int16 dummyLine;
	kal_uint16 lineLength,frameHeight;

	SENSORDB("S5K8AAYXMIPISetMaxFramerateByScenario: scenarioId = %d, frame rate = %d\n",scenarioId,frameRate);

	switch (scenarioId) {
		case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
			pclk = S5K8AAYX_MIPI_sensor_pclk/10;
			lineLength = S5K8AAYX_MIPI_SXGA_PERIOD_PIXEL_NUMS;
			frameHeight = (10 * pclk)/frameRate/lineLength;
			dummyLine = frameHeight - S5K8AAYX_MIPI_SXGA_PERIOD_LINE_NUMS;
			SENSORDB("S5K8AAYXMIPISetMaxFramerateByScenario MSDK_SCENARIO_ID_CAMERA_PREVIEW: lineLength = %d, dummy=%d\n",lineLength,dummyLine);
			S5K8AAYX_MIPI_set_dummy(0, dummyLine);
			break;
		case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
			pclk = S5K8AAYX_MIPI_sensor_pclk/10;
			lineLength = S5K8AAYX_MIPI_SXGA_PERIOD_PIXEL_NUMS;
			frameHeight = (10 * pclk)/frameRate/lineLength;
			dummyLine = frameHeight - S5K8AAYX_MIPI_SXGA_PERIOD_LINE_NUMS;
			SENSORDB("S5K8AAYXMIPISetMaxFramerateByScenario MSDK_SCENARIO_ID_VIDEO_PREVIEW: lineLength = %d, dummy=%d\n",lineLength,dummyLine);
			S5K8AAYX_MIPI_set_dummy(0, dummyLine);
			break;
		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
		case MSDK_SCENARIO_ID_CAMERA_ZSD:
			pclk = S5K8AAYX_MIPI_sensor_pclk/10;
			lineLength = S5K8AAYX_MIPI_SXGA_PERIOD_PIXEL_NUMS;
			frameHeight = (10 * pclk)/frameRate/lineLength;
			dummyLine = frameHeight - S5K8AAYX_MIPI_SXGA_PERIOD_LINE_NUMS;
			SENSORDB("S5K8AAYXMIPISetMaxFramerateByScenario MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG: lineLength = %d, dummy=%d\n",lineLength,dummyLine);
			S5K8AAYX_MIPI_set_dummy(0, dummyLine);
			break;
        case MSDK_SCENARIO_ID_CAMERA_3D_PREVIEW: //     
            break;
        case MSDK_SCENARIO_ID_CAMERA_3D_VIDEO:
			break;
        case MSDK_SCENARIO_ID_CAMERA_3D_CAPTURE: //     
			break;
		default:
			break;
	}
	return ERROR_NONE;
}


UINT32 S5K8AAYXMIPIGetDefaultFramerateByScenario(MSDK_SCENARIO_ID_ENUM scenarioId, MUINT32 *pframeRate)
{

	switch (scenarioId) {
		case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
		case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
			 *pframeRate = 300;
			 break;
		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
		case MSDK_SCENARIO_ID_CAMERA_ZSD:
			 *pframeRate = 300;
			break;
        case MSDK_SCENARIO_ID_CAMERA_3D_PREVIEW: //     
        case MSDK_SCENARIO_ID_CAMERA_3D_VIDEO:
        case MSDK_SCENARIO_ID_CAMERA_3D_CAPTURE: //     
			 *pframeRate = 300;
			break;
		default:
			break;
	}

	return ERROR_NONE;
}

UINT32 S5K8AAYXSetTestPatternMode(kal_bool bEnable)
{
    SENSORDB("[S5K8AAYXSetTestPatternMode] Test pattern enable:%d\n", bEnable);

	if(bEnable)
	{
        //                         
        S5K8AAYX_MIPI_write_cmos_sensor(0xFCFC,0xD000);
        S5K8AAYX_MIPI_write_cmos_sensor(0x3700,0x0001);
        S5K8AAYX_MIPI_write_cmos_sensor(0x3E00,0x0001);
        S5K8AAYX_MIPI_write_cmos_sensor(0x4300,0x0001);
        S5K8AAYX_MIPI_write_cmos_sensor(0x4400,0x0001);
        S5K8AAYX_MIPI_write_cmos_sensor(0x4500,0x0001);
        S5K8AAYX_MIPI_write_cmos_sensor(0x4600,0x0001);
        S5K8AAYX_MIPI_write_cmos_sensor(0x4700,0x0001);
        S5K8AAYX_MIPI_write_cmos_sensor(0x6000,0x0001);
        S5K8AAYX_MIPI_write_cmos_sensor(0x6100,0x0001);
        S5K8AAYX_MIPI_write_cmos_sensor(0x6330,0x0001);
        S5K8AAYX_MIPI_write_cmos_sensor(0x6400,0x0001);
        S5K8AAYX_MIPI_write_cmos_sensor(0x6500,0x0001);
        S5K8AAYX_MIPI_write_cmos_sensor(0x6700,0x0001);
        S5K8AAYX_MIPI_write_cmos_sensor(0x6800,0x0001);
        S5K8AAYX_MIPI_write_cmos_sensor(0x6A00,0x0001);
        S5K8AAYX_MIPI_write_cmos_sensor(0x6B00,0x0001);
        S5K8AAYX_MIPI_write_cmos_sensor(0x4100,0x0001);

        //                                
        //                    
        S5K8AAYX_MIPI_write_cmos_sensor(0x3602,0x1F40);
        S5K8AAYX_MIPI_write_cmos_sensor(0x3604,0x1A40);
        S5K8AAYX_MIPI_write_cmos_sensor(0x3606,0x1A40);
        S5K8AAYX_MIPI_write_cmos_sensor(0x3608,0x1040);
        //                 
        S5K8AAYX_MIPI_write_cmos_sensor(0x360a,0x0383);
        //                 
        //                
        //                    
        //                 
        //                                
        //               
        //                                           
        //                          
        //                             
        S5K8AAYX_MIPI_write_cmos_sensor(0x3600,0x0004);

        mdelay(100);

	}
	else
	{
		S5K8AAYX_MIPI_write_cmos_sensor(0x3600,0x0000);
	}
    return ERROR_NONE;
}

UINT32 S5K8AAYX_MIPIFeatureControl(MSDK_SENSOR_FEATURE_ENUM FeatureId,
							 UINT8 *pFeaturePara,UINT32 *pFeatureParaLen)
{
	UINT16 *pFeatureReturnPara16=(UINT16 *) pFeaturePara;
	UINT16 *pFeatureData16=(UINT16 *) pFeaturePara;
	UINT32 *pFeatureReturnPara32=(UINT32 *) pFeaturePara;
	UINT32 *pFeatureData32=(UINT32 *) pFeaturePara;
	//                                                                                      
	MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData=(MSDK_SENSOR_CONFIG_STRUCT *) pFeaturePara;
	MSDK_SENSOR_REG_INFO_STRUCT *pSensorRegData=(MSDK_SENSOR_REG_INFO_STRUCT *) pFeaturePara;
	//                                                                                               
	//                                                                                            
	//                                                                                         
#if WINMO_USE
	PMSDK_FEATURE_INFO_STRUCT pSensorFeatureInfo=(PMSDK_FEATURE_INFO_STRUCT) pFeaturePara;
#endif


	switch (FeatureId)
	{
		case SENSOR_FEATURE_GET_RESOLUTION:
			*pFeatureReturnPara16++=S5K8AAYX_MIPI_IMAGE_SENSOR_FULL_WIDTH;
			*pFeatureReturnPara16=S5K8AAYX_MIPI_IMAGE_SENSOR_FULL_HEIGHT;
			*pFeatureParaLen=4;
		break;
		case SENSOR_FEATURE_GET_PERIOD:
			//                                                                                         
			//                                                                                     
			//                   
		break;
		case SENSOR_FEATURE_GET_PIXEL_CLOCK_FREQ:
			*pFeatureReturnPara32 = S5K8AAYX_MIPI_sensor_pclk/10;
			*pFeatureParaLen=4;
		break;
		case SENSOR_FEATURE_SET_ESHUTTER:
		break;
		case SENSOR_FEATURE_SET_NIGHTMODE:
			S5K8AAYX_MIPI_night_mode((BOOL) *pFeatureData16);
		break;
		case SENSOR_FEATURE_SET_GAIN:
		case SENSOR_FEATURE_SET_FLASHLIGHT:
		break;
		case SENSOR_FEATURE_SET_ISP_MASTER_CLOCK_FREQ:
			S5K8AAYX_MIPI_isp_master_clock=*pFeatureData32;
		break;
		case SENSOR_FEATURE_SET_REGISTER:
			S5K8AAYX_MIPI_write_cmos_sensor(pSensorRegData->RegAddr, pSensorRegData->RegData);
		break;
		case SENSOR_FEATURE_GET_REGISTER:
			pSensorRegData->RegData = S5K8AAYX_MIPI_read_cmos_sensor(pSensorRegData->RegAddr);
		break;
		case SENSOR_FEATURE_GET_CONFIG_PARA:
			memcpy(pSensorConfigData, &S5K8AAYX_MIPISensorConfigData, sizeof(MSDK_SENSOR_CONFIG_STRUCT));
			*pFeatureParaLen=sizeof(MSDK_SENSOR_CONFIG_STRUCT);
		break;
		case SENSOR_FEATURE_SET_CCT_REGISTER:
		case SENSOR_FEATURE_GET_CCT_REGISTER:
		case SENSOR_FEATURE_SET_ENG_REGISTER:
		case SENSOR_FEATURE_GET_ENG_REGISTER:
		case SENSOR_FEATURE_GET_REGISTER_DEFAULT:

		case SENSOR_FEATURE_CAMERA_PARA_TO_SENSOR:
		case SENSOR_FEATURE_SENSOR_TO_CAMERA_PARA:

		case SENSOR_FEATURE_GET_GROUP_INFO:
		case SENSOR_FEATURE_GET_ITEM_INFO:
		case SENSOR_FEATURE_SET_ITEM_INFO:
		case SENSOR_FEATURE_GET_ENG_INFO:
					break;
		case SENSOR_FEATURE_GET_GROUP_COUNT:
                        *pFeatureReturnPara32++=0;
                        *pFeatureParaLen=4;

		break;
		case SENSOR_FEATURE_GET_LENS_DRIVER_ID:
			//                                                                             
			//                                           
			*pFeatureReturnPara32=LENS_DRIVER_ID_DO_NOT_CARE;
			*pFeatureParaLen=4;
		break;
		case SENSOR_FEATURE_SET_YUV_CMD:
			//                                                                                           
			S5K8AAYX_MIPIYUVSensorSetting((FEATURE_ID)*pFeatureData32, *(pFeatureData32+1));
		break;

#if WINMO_USE
		case SENSOR_FEATURE_QUERY:
			S5K8AAYX_MIPIQuery(pSensorFeatureInfo);
			*pFeatureParaLen = sizeof(MSDK_FEATURE_INFO_STRUCT);
		break;
		case SENSOR_FEATURE_SET_YUV_CAPTURE_RAW_SUPPORT:
			/*                                                        */
		break;
#endif
		case SENSOR_FEATURE_SET_VIDEO_MODE:
		    S5K8AAYX_MIPIYUVSetVideoMode(*pFeatureData16);
		    break;
		//                   
		case SENSOR_FEATURE_GET_EV_AWB_REF:
			 S5K8AAYX_MIPIGetEvAwbRef(*pFeatureData32);
				break;
  		case SENSOR_FEATURE_GET_SHUTTER_GAIN_AWB_GAIN:
			   S5K8AAYX_MIPIGetCurAeAwbInfo(*pFeatureData32);
			break;
		//      
		case SENSOR_FEATURE_CHECK_SENSOR_ID:
            S5K8AAYX_MIPI_GetSensorID(pFeatureData32);
            break;

		case SENSOR_FEATURE_GET_AF_MAX_NUM_FOCUS_AREAS:
			S5K8AAYX_MIPIGetAFMaxNumFocusAreas(pFeatureReturnPara32);
			*pFeatureParaLen=4;
			break;
		case SENSOR_FEATURE_GET_AE_MAX_NUM_METERING_AREAS:
			S5K8AAYX_MIPIGetAFMaxNumMeteringAreas(pFeatureReturnPara32);
			*pFeatureParaLen=4;
			break;
		case SENSOR_FEATURE_GET_EXIF_INFO:
			SENSORDB("SENSOR_FEATURE_GET_EXIF_INFO\n");
			SENSORDB("EXIF addr = 0x%x\n",*pFeatureData32);
			S5K8AAYX_MIPIGetExifInfo(*pFeatureData32);
			break;
		case SENSOR_FEATURE_GET_DELAY_INFO:
			SENSORDB("SENSOR_FEATURE_GET_DELAY_INFO\n");
		    S5K8AAYX_MIPIGetDelayInfo(*pFeatureData32);
			break;
		case SENSOR_FEATURE_SET_MAX_FRAME_RATE_BY_SCENARIO:
			S5K8AAYXMIPISetMaxFramerateByScenario((MSDK_SCENARIO_ID_ENUM)*pFeatureData32, *(pFeatureData32+1));
			break;
		case SENSOR_FEATURE_GET_DEFAULT_FRAME_RATE_BY_SCENARIO:
			S5K8AAYXMIPIGetDefaultFramerateByScenario((MSDK_SCENARIO_ID_ENUM)*pFeatureData32, (MUINT32 *)(*(pFeatureData32+1)));
			break;
		case SENSOR_FEATURE_GET_AE_AWB_LOCK_INFO:
			S5K8AAYX_MIPIGetAEAWBLock((UINT32*)(*pFeatureData32),(UINT32*)*(pFeatureData32+1));
			break;
        case SENSOR_FEATURE_AUTOTEST_CMD:
			SENSORDB("SENSOR_FEATURE_AUTOTEST_CMD\n");
			S5K8AAYX_MIPIAutoTestCmd((UINT32*)(*pFeatureData32),(UINT32*)*(pFeatureData32+1));
            break;
        case SENSOR_FEATURE_SET_TEST_PATTERN://                             
            S5K8AAYXSetTestPatternMode((BOOL)*pFeatureData16);
            break;
        case SENSOR_FEATURE_GET_TEST_PATTERN_CHECKSUM_VALUE://                             
            *pFeatureReturnPara32= S5K8AAYX_TEST_PATTERN_CHECKSUM;
            *pFeatureParaLen=4;
            break;

		default:
			break;
	}
	return ERROR_NONE;
}

SENSOR_FUNCTION_STRUCT	SensorFuncS5K8AAYX_MIPI=
{
	S5K8AAYX_MIPIOpen,
	S5K8AAYX_MIPIGetInfo,
	S5K8AAYX_MIPIGetResolution,
	S5K8AAYX_MIPIFeatureControl,
	S5K8AAYX_MIPIControl,
	S5K8AAYX_MIPIClose
};

UINT32 S5K8AAYX_MIPI_YUV_SensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc)
{
	/*                                  */
	if (pfFunc!=NULL)
		*pfFunc=&SensorFuncS5K8AAYX_MIPI;
	return ERROR_NONE;
}	/*              */
