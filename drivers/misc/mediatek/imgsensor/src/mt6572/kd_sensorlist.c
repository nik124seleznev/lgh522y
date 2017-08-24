#include <linux/videodev2.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <asm/atomic.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>   //             
#include <linux/dma-mapping.h>
#include <linux/xlog.h>

#include "../camera/kd_camera_hw.h"
#include <asm/system.h>


#include "kd_imgsensor.h"
#include "kd_imgsensor_define.h"
#include "kd_camera_feature.h"
#include "kd_imgsensor_errcode.h"

#include "kd_sensorlist.h"

static DEFINE_SPINLOCK(kdsensor_drv_lock);

#define SUPPORT_I2C_BUS_NUM1          0
//                                     


#define CAMERA_HW_DRVNAME1  "kd_camera_hw"
//                                               

static struct i2c_board_info __initdata i2c_devs1={I2C_BOARD_INFO(CAMERA_HW_DRVNAME1, 0xfe>>1)};
//                                                                                                




/*                                                                             
                      
                                                                             */
#define PFX "[kd_sensorlist]"
#define PK_DBG_NONE(fmt, arg...)    do {} while (0)
#define PK_DBG_FUNC(fmt, arg...)    xlog_printk(ANDROID_LOG_DEBUG , PFX, fmt, ##arg)


#define DEBUG_CAMERA_HW_K
#ifdef DEBUG_CAMERA_HW_K
#define PK_DBG PK_DBG_FUNC
#define PK_ERR(fmt, arg...)         xlog_printk(ANDROID_LOG_ERROR , PFX , fmt, ##arg)
#define PK_XLOG_INFO(fmt, args...) \
                do {    \
                    xlog_printk(ANDROID_LOG_DEBUG, PFX, fmt, ##args); \
                } while(0)
#else
#define PK_DBG(a,...)
#define PK_ERR(a,...)
#define PK_XLOG_INFO(a...)

#endif

/*                                                                              
           
                                                                               */
#define PROFILE 1
#if PROFILE
static struct timeval tv1, tv2;
/*                                                                              
 
                                                                               */
inline void KD_IMGSENSOR_PROFILE_INIT(void)
{
    do_gettimeofday(&tv1);
}

/*                                                                              
 
                                                                               */
inline void KD_IMGSENSOR_PROFILE(char *tag)
{
    unsigned long TimeIntervalUS;

	spin_lock(&kdsensor_drv_lock);

    do_gettimeofday(&tv2);
    TimeIntervalUS = (tv2.tv_sec - tv1.tv_sec) * 1000000 + (tv2.tv_usec - tv1.tv_usec);
    tv1 = tv2;

	spin_unlock(&kdsensor_drv_lock);
    PK_DBG("[%s]Profile = %lu\n",tag, TimeIntervalUS);
}
#else
inline static void KD_IMGSENSOR_PROFILE_INIT() {}
inline static void KD_IMGSENSOR_PROFILE(char *tag) {}
#endif

/*                                                                              
 
                                                                               */
extern int kdCISModulePowerOn(CAMERA_DUAL_CAMERA_SENSOR_ENUM SensorIdx, char *currSensorName,BOOL On, char* mode_name);
//                                                                           

/*                                                                              
 
                                                                               */

static struct i2c_client * g_pstI2Cclient = NULL;
//                                                 

//                         
static dev_t g_CAMERA_HWdevno = MKDEV(250,0);
//                               
static struct cdev * g_pCAMERA_HW_CharDrv = NULL;
//                                                  
static struct class *sensor_class = NULL;
//                                          

static atomic_t g_CamHWOpend;
//                              
static atomic_t g_CamHWOpening;
static atomic_t g_CamDrvOpenCnt;
//                                 

//                                     
//                                           

//                                                                      
//                                                                         

static DEFINE_MUTEX(kdCam_Mutex);
static BOOL bSesnorVsyncFlag = FALSE;
static ACDK_KD_SENSOR_SYNC_STRUCT g_NewSensorExpGain = {128, 128, 128, 128, 1000, 640, 0xFF, 0xFF, 0xFF, 0};


extern MULTI_SENSOR_FUNCTION_STRUCT kd_MultiSensorFunc;
static MULTI_SENSOR_FUNCTION_STRUCT *g_pSensorFunc = &kd_MultiSensorFunc;;
static SENSOR_FUNCTION_STRUCT *g_pInvokeSensorFunc[KDIMGSENSOR_MAX_INVOKE_DRIVERS] = {NULL,NULL};
static BOOL g_bEnableDriver[KDIMGSENSOR_MAX_INVOKE_DRIVERS] = {FALSE,FALSE};
static CAMERA_DUAL_CAMERA_SENSOR_ENUM g_invokeSocketIdx[KDIMGSENSOR_MAX_INVOKE_DRIVERS] = {DUAL_CAMERA_NONE_SENSOR,DUAL_CAMERA_NONE_SENSOR};
static char g_invokeSensorNameStr[KDIMGSENSOR_MAX_INVOKE_DRIVERS][32] = {KDIMGSENSOR_NOSENSOR,KDIMGSENSOR_NOSENSOR};

static CAMERA_DUAL_CAMERA_SENSOR_ENUM g_CurrentInvokeCam = DUAL_CAMERA_NONE_SENSOR;

/*                                                                             

                                                                             */
/*                                                                              
                    
                                                                    
                                                                               */
static const struct i2c_device_id CAMERA_HW_i2c_id[] = {{CAMERA_HW_DRVNAME1,0},{}};
//                                                                                    

/*                                                                              
                                           
                                                                              */
UINT32 kdGetSensorInitFuncList(ACDK_KD_SENSOR_INIT_FUNCTION_STRUCT **ppSensorList)
{
	if (NULL == ppSensorList)
	{
		PK_DBG("[kdGetSensorInitFuncList]ERROR: NULL ppSensorList\n");
		return 1;
	}
	*ppSensorList = &kdSensorList[0];
	return 0;
} //                          


/*                                                                              
 
                                                                               */


/*                                                                              
          
                                                                               */
int iReadReg(u16 a_u2Addr , u8 * a_puBuff , u16 i2cId)
{
    int  i4RetValue = 0;
    char puReadCmd[2] = {(char)(a_u2Addr >> 8) , (char)(a_u2Addr & 0xFF)};

    //                                       
    {
		spin_lock(&kdsensor_drv_lock);

	    g_pstI2Cclient->addr = (i2cId >> 1);
	    g_pstI2Cclient->ext_flag = (g_pstI2Cclient->ext_flag)&(~I2C_DMA_FLAG);

		spin_unlock(&kdsensor_drv_lock);

	    //
	    i4RetValue = i2c_master_send(g_pstI2Cclient, puReadCmd, 2);
	    if (i4RetValue != 2) {
	        PK_DBG("[CAMERA SENSOR] I2C send failed, addr = 0x%x, data = 0x%x !! \n", a_u2Addr,  *a_puBuff );
	        return -1;
	    }
	    //
	    i4RetValue = i2c_master_recv(g_pstI2Cclient, (char *)a_puBuff, 1);
	    if (i4RetValue != 1) {
	        PK_DBG("[CAMERA SENSOR] I2C read failed!! \n");
	        return -1;
	    }
    }
    /*      
                                
                                             
                                                                                
                                  
          
                                                                    
                              
                                                                                                             
                      
         
          
                                                                           
                              
                                                           
                      
         
     */
    return 0;
}

/*                                                                              
             
                                                                               */
int iReadRegI2C(u8 *a_pSendData , u16 a_sizeSendData, u8 * a_pRecvData, u16 a_sizeRecvData, u16 i2cId)
{
    int  i4RetValue = 0;
    //                                       
    {
		spin_lock(&kdsensor_drv_lock);
	    g_pstI2Cclient->addr = (i2cId >> 1);
	    g_pstI2Cclient->ext_flag = (g_pstI2Cclient->ext_flag)&(~I2C_DMA_FLAG);
		spin_unlock(&kdsensor_drv_lock);
	    //
	    i4RetValue = i2c_master_send(g_pstI2Cclient, a_pSendData, a_sizeSendData);
	    if (i4RetValue != a_sizeSendData) {
	        PK_DBG("[CAMERA SENSOR] I2C send failed!!, Addr = 0x%x\n", a_pSendData[0]);
	        return -1;
	    }

	    i4RetValue = i2c_master_recv(g_pstI2Cclient, (char *)a_pRecvData, a_sizeRecvData);
	    if (i4RetValue != a_sizeRecvData) {
	        PK_DBG("[CAMERA SENSOR] I2C read failed!! \n");
	        return -1;
	    }
    }
    /*     
                                   
                                             
                                                                                
                                     
                                                                                   
                                           
                                                                                       
                      
         

                                                                                           
                                           
                                                           
                      
         
     */
    return 0;
}

/*                                                                              
                  
                                                                               */
int iBurstReadRegI2C(u8 *a_pSendData , u16 a_sizeSendData, u8 * a_pRecvData, u16 a_sizeRecvData, u16 i2cId)
{
    int  i4RetValue = 0;
    u32 phyAddr = 0;
    u8 *buf = NULL;

		spin_lock(&kdsensor_drv_lock);
	    g_pstI2Cclient->addr = (i2cId >> 1);
	    g_pstI2Cclient->ext_flag = (g_pstI2Cclient->ext_flag)&(~I2C_DMA_FLAG);
		spin_unlock(&kdsensor_drv_lock);
	    //
	    i4RetValue = i2c_master_send(g_pstI2Cclient, a_pSendData, a_sizeSendData);
	    if (i4RetValue != a_sizeSendData) {
	        PK_DBG("[CAMERA SENSOR] I2C send failed!!, Addr = 0x%x\n", a_pSendData[0]);
	        return -1;
	    }

	    spin_lock(&kdsensor_drv_lock);
	    g_pstI2Cclient->ext_flag = (g_pstI2Cclient->ext_flag) | (I2C_DMA_FLAG);
		  spin_unlock(&kdsensor_drv_lock);

		  if (a_sizeRecvData > 255) {
	        PK_DBG("[iBurstWriteReg] exceed the max write length \n");
	        return 1;
	    }
	    phyAddr = 0;

	    buf = dma_alloc_coherent(0, a_sizeRecvData, &phyAddr, GFP_KERNEL);

	    if (NULL == buf) {
	        PK_DBG("[iBurstWriteReg] Not enough memory \n");
	        return -1;
	    }

		  PK_DBG("[iBurstReadRegI2C] g_pstI2Cclient->ext_flag=%x %x %x\n",g_pstI2Cclient->ext_flag,phyAddr,buf);

	    //                                                                                  
	    i4RetValue = i2c_master_recv(g_pstI2Cclient, (char *)phyAddr, a_sizeRecvData);
	    if (i4RetValue != a_sizeRecvData) {
	        PK_DBG("[CAMERA SENSOR] I2C read failed!! \n");
	        return -1;
	    }

	    memcpy(a_pRecvData, buf, a_sizeRecvData);

	    dma_free_coherent(0, a_sizeRecvData, buf, phyAddr);

    return 0;
}


/*                                                                              
           
                                                                               */
int iWriteReg(u16 a_u2Addr , u32 a_u4Data , u32 a_u4Bytes , u16 i2cId)
{
    int  i4RetValue = 0;
    int u4Index = 0;
    u8 * puDataInBytes = (u8 *)&a_u4Data;
    int retry = 3;

    char puSendCmd[6] = {(char)(a_u2Addr >> 8) , (char)(a_u2Addr & 0xFF) ,
        0 , 0 , 0 , 0};

//                                                          

    //                            
    spin_lock(&kdsensor_drv_lock);

    //                                        
    g_pstI2Cclient->addr = (i2cId >> 1);
    // 
    //      
    //                                         
    // 
    spin_unlock(&kdsensor_drv_lock);


    if(a_u4Bytes > 2)
    {
        PK_DBG("[CAMERA SENSOR] exceed 2 bytes \n");
        return -1;
    }

    if(a_u4Data >> (a_u4Bytes << 3))
    {
        PK_DBG("[CAMERA SENSOR] warning!! some data is not sent!! \n");
    }

    for(u4Index = 0 ; u4Index < a_u4Bytes ; u4Index += 1 )
    {
        puSendCmd[(u4Index + 2)] = puDataInBytes[(a_u4Bytes - u4Index-1)];
    }
    //
    do {
        //                                        
        i4RetValue = i2c_master_send(g_pstI2Cclient, puSendCmd, (a_u4Bytes + 2));
        // 
        //      
        //                                                                               
        // 
        if (i4RetValue != (a_u4Bytes + 2)) {
        PK_DBG("[CAMERA SENSOR] I2C send failed addr = 0x%x, data = 0x%x !! \n", a_u2Addr, a_u4Data);
        }
        else {
            break;
        }
        uDELAY(50);
    } while ((retry --) > 0);
    //                                  
    return 0;
}

#if 0
int kdSetI2CBusNum(u32 i2cBusNum){

    if((i2cBusNum != SUPPORT_I2C_BUS_NUM2) && (i2cBusNum != SUPPORT_I2C_BUS_NUM1)){
        PK_DBG("[kdSetI2CBusNum] i2c bus number is not correct(%d) \n",i2cBusNum);
        return -1;
    }
	spin_lock(&kdsensor_drv_lock);
    gI2CBusNum = i2cBusNum;
	spin_unlock(&kdsensor_drv_lock);

    return 0;
}
/*                                                                              
                         
                                                                               */
int kdReleaseI2CTriggerLock(void)
{
    int ret=0;

    //                              

    //               
    //                                     
    // 

    return ret;
}
#endif

/*                                                                              
                
                                                                               */
#define MAX_CMD_LEN          255
int iBurstWriteReg(u8 *pData, u32 bytes, u16 i2cId)
{

    u32 phyAddr = 0;
    u8 *buf = NULL;
    u32 old_addr = 0;
    int ret = 0;
    int retry = 0;

    //                                       
    {
	    if (bytes > MAX_CMD_LEN) {
	        PK_DBG("[iBurstWriteReg] exceed the max write length \n");
	        return 1;
	    }
	    phyAddr = 0;

	    buf = dma_alloc_coherent(0, bytes, &phyAddr, GFP_KERNEL);

	    if (NULL == buf) {
	        PK_DBG("[iBurstWriteReg] Not enough memory \n");
	        return -1;
	    }

	    memcpy(buf, pData, bytes);
	    //                                                                           

	    old_addr = g_pstI2Cclient->addr;
            spin_lock(&kdsensor_drv_lock);
	    g_pstI2Cclient->addr = ( ((i2cId >> 1) &  I2C_MASK_FLAG) | I2C_DMA_FLAG );
	    //                                                                                         
            spin_unlock(&kdsensor_drv_lock);

	    ret = 0;
	    retry = 3;
	    do {
	        ret = i2c_master_send(g_pstI2Cclient, (u8*)phyAddr, bytes);
	        retry --;
	        if (ret != bytes) {
	            PK_DBG("Error sent I2C ret = %d\n", ret);
	        }
	    }while ((ret != bytes) && (retry > 0));

	    dma_free_coherent(0, bytes, buf, phyAddr);

		spin_lock(&kdsensor_drv_lock);
	    g_pstI2Cclient->addr = old_addr;
		spin_unlock(&kdsensor_drv_lock);
    }
   /*     
                                  
                                                                      
                     
         
                    

                                                                 

                          
                                                            
                      
         

                                  
                                                                                     

                                         
                                      
                                                                                                   
                                        


                
                  
            
                                                                        
                     
                               
                                                         
             
                                               

                                                  
                                      
                                         
                                        

     */
    return 0;
}


/*                                                                              
                
                                                                               */

int iMultiWriteReg(u8 *pData, u16 lens)
{
	int ret = 0;

	ret = i2c_master_send(g_pstI2Cclient, pData, lens);

	if(ret != lens) {
		PK_DBG("Error sent I2C ret = %d\n", ret);
	}
	return 0;
}


/*                                                                              
              
                                                                               */
int iWriteRegI2C(u8 *a_pSendData , u16 a_sizeSendData, u16 i2cId)
{
    int  i4RetValue = 0;
    int retry = 3;

//                                                          

    //                            
    spin_lock(&kdsensor_drv_lock);
    //                                        
    g_pstI2Cclient->addr = (i2cId >> 1);
    // 
    //      
    //                                         
    // 
	spin_unlock(&kdsensor_drv_lock);
    //

    do {
	//                                        
	i4RetValue = i2c_master_send(g_pstI2Cclient, a_pSendData, a_sizeSendData);
	// 
	//      
	//                                                                              
	// 
        if (i4RetValue != a_sizeSendData) {
            PK_DBG("[CAMERA SENSOR] I2C send failed!!, Addr = 0x%x, Data = 0x%x \n", a_pSendData[0], a_pSendData[1] );
        }
        else {
            break;
    	}
        uDELAY(50);
    } while ((retry--) > 0);
    //                                     
    return 0;
}

/*                                                                              
                         
                                                                               */
#define KD_MULTI_FUNCTION_ENTRY()   //                                      
#define KD_MULTI_FUNCTION_EXIT()    //                                      
//
MUINT32
kdSetI2CSlaveID(MINT32 i, MUINT32 socketIdx,MUINT32 firstSet) {
MUINT32 u4FeaturePara[4];
MUINT32 FeatureParaLen = 0;
    u4FeaturePara[0] = socketIdx;
    u4FeaturePara[1] = firstSet;
    FeatureParaLen = sizeof(MUINT32)*2;
    return g_pInvokeSensorFunc[i]->SensorFeatureControl(SENSOR_FEATURE_SET_SLAVE_I2C_ID,(MUINT8*)u4FeaturePara,(MUINT32*)&FeatureParaLen);
}

//
MUINT32
kd_MultiSensorOpen ( void )
{
MUINT32 ret = ERROR_NONE;
MINT32 i = 0;
    KD_MULTI_FUNCTION_ENTRY();
    //                 
    //                                                                                    
    //                  
    for ( i = (KDIMGSENSOR_MAX_INVOKE_DRIVERS-1) ; i >= KDIMGSENSOR_INVOKE_DRIVER_0 ; i-- ) {
        if ( g_bEnableDriver[i] && g_pInvokeSensorFunc[i] ) {
            //              
            ret = kdCISModulePowerOn((CAMERA_DUAL_CAMERA_SENSOR_ENUM)g_invokeSocketIdx[i],(char*)g_invokeSensorNameStr[i],true,CAMERA_HW_DRVNAME1);
            if ( ERROR_NONE != ret ) {
                PK_ERR("[%s]",__FUNCTION__);
                return ret;
            }
            //                     
            mDELAY(10);
            KD_IMGSENSOR_PROFILE("kdModulePowerOn");

	    /*                                                         
                                
                                    
                                  
                                                        
      
           
                                
                                    
                                  
                                                        
     */
            //
            //                
            //                                                                       
            //
            ret = g_pInvokeSensorFunc[i]->SensorOpen();
            if ( ERROR_NONE != ret ) {
				kdCISModulePowerOn((CAMERA_DUAL_CAMERA_SENSOR_ENUM)g_invokeSocketIdx[i],(char*)g_invokeSensorNameStr[i],false,CAMERA_HW_DRVNAME1);
                PK_ERR("SensorOpen");
                return ret;
            }
            //                
            //                                    
            //                                                                       
        }
    }
    KD_MULTI_FUNCTION_EXIT();
    return ERROR_NONE;
}
//

MUINT32
kd_MultiSensorGetInfo (
MUINT32 *pScenarioId[2],
MSDK_SENSOR_INFO_STRUCT *pSensorInfo[2],
MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData[2])
{
    MUINT32 ret = ERROR_NONE;
    u32 i = 0;
	MSDK_SENSOR_INFO_STRUCT SensorInfo[2];
	MSDK_SENSOR_CONFIG_STRUCT SensorConfigData[2];
	memset(&SensorInfo[0],0,2*sizeof(MSDK_SENSOR_INFO_STRUCT));
	memset(&SensorConfigData[0],0,2*sizeof(MSDK_SENSOR_CONFIG_STRUCT));


    KD_MULTI_FUNCTION_ENTRY();
    for ( i = KDIMGSENSOR_INVOKE_DRIVER_0 ; i < KDIMGSENSOR_MAX_INVOKE_DRIVERS ; i++ ) {
        if ( g_bEnableDriver[i] && g_pInvokeSensorFunc[i] ) {
	    if(DUAL_CAMERA_MAIN_SENSOR==g_invokeSocketIdx[i] ){
	       ret = g_pInvokeSensorFunc[i]->SensorGetInfo((MSDK_SCENARIO_ID_ENUM)(*pScenarioId[0]),&SensorInfo[0],&SensorConfigData[0]);
	    }
	    //                                                                                                             
            else if(DUAL_CAMERA_SUB_SENSOR==g_invokeSocketIdx[i]) {
	       ret = g_pInvokeSensorFunc[i]->SensorGetInfo((MSDK_SCENARIO_ID_ENUM)(*pScenarioId[1]),&SensorInfo[1],&SensorConfigData[1]);
	    }

            if ( ERROR_NONE != ret ) {
                PK_ERR("[%s]\n",__FUNCTION__);
                return ret;
            }

        }
    }
	memcpy(pSensorInfo[0],&SensorInfo[0],sizeof(MSDK_SENSOR_INFO_STRUCT));
	memcpy(pSensorInfo[1],&SensorInfo[1],sizeof(MSDK_SENSOR_INFO_STRUCT));
	memcpy(pSensorConfigData[0],&SensorConfigData[0],sizeof(MSDK_SENSOR_CONFIG_STRUCT));
	memcpy(pSensorConfigData[1],&SensorConfigData[1],sizeof(MSDK_SENSOR_CONFIG_STRUCT));



    KD_MULTI_FUNCTION_EXIT();
    return ERROR_NONE;
}

//

MUINT32
kd_MultiSensorGetResolution (
MSDK_SENSOR_RESOLUTION_INFO_STRUCT *pSensorResolution[2])
{
    MUINT32 ret = ERROR_NONE;
    u32 i = 0;
    KD_MULTI_FUNCTION_ENTRY();
    for ( i = KDIMGSENSOR_INVOKE_DRIVER_0 ; i < KDIMGSENSOR_MAX_INVOKE_DRIVERS ; i++ ) {
        if ( g_bEnableDriver[i] && g_pInvokeSensorFunc[i] ) {
	   if(DUAL_CAMERA_MAIN_SENSOR==g_invokeSocketIdx[i]){
               ret = g_pInvokeSensorFunc[i]->SensorGetResolution(pSensorResolution[0]);
	    }
            //                                                                                                              
            else if(DUAL_CAMERA_SUB_SENSOR==g_invokeSocketIdx[i]) {
	       ret = g_pInvokeSensorFunc[i]->SensorGetResolution(pSensorResolution[1]);
	    }

            if ( ERROR_NONE != ret ) {
                PK_ERR("[%s]\n",__FUNCTION__);
                return ret;
            }
        }
    }

    KD_MULTI_FUNCTION_EXIT();
    return ERROR_NONE;
}


//
MUINT32
kd_MultiSensorFeatureControl (
CAMERA_DUAL_CAMERA_SENSOR_ENUM InvokeCamera,
MSDK_SENSOR_FEATURE_ENUM FeatureId,
MUINT8 *pFeaturePara,
MUINT32 *pFeatureParaLen)
{
    MUINT32 ret = ERROR_NONE;
    u32 i = 0;
    KD_MULTI_FUNCTION_ENTRY();
    for ( i = KDIMGSENSOR_INVOKE_DRIVER_0 ; i < KDIMGSENSOR_MAX_INVOKE_DRIVERS ; i++ ) {
        if ( g_bEnableDriver[i] && g_pInvokeSensorFunc[i] ) {

                   if(InvokeCamera==g_invokeSocketIdx[i]){
		      /*                                                         
                                 
                                     
                                   
                                                                    
         
              
                                 
                                            
                                   
                                                                   
         */

	            //
	            //                
	            //                                                                       
	            //
	            ret = g_pInvokeSensorFunc[i]->SensorFeatureControl(FeatureId,pFeaturePara,pFeatureParaLen);
	            if ( ERROR_NONE != ret ) {
	                PK_ERR("[%s]\n",__FUNCTION__);
	                return ret;
	            }
			}
        }
    }
    KD_MULTI_FUNCTION_EXIT();
    return ERROR_NONE;
}

//
MUINT32
kd_MultiSensorControl (
MSDK_SCENARIO_ID_ENUM ScenarioId,
MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *pImageWindow,
MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData)
{
    MUINT32 ret = ERROR_NONE;
    u32 i = 0;
    KD_MULTI_FUNCTION_ENTRY();
    for ( i = KDIMGSENSOR_INVOKE_DRIVER_0 ; i < KDIMGSENSOR_MAX_INVOKE_DRIVERS ; i++ ) {
        if ( g_bEnableDriver[i] && g_pInvokeSensorFunc[i] ) {
		/*                                                         
                                 
                                     
                                   
                                                             
   
        
                                 
                                     
                                   
                                                             
   */

            //
            //                
            //                                                                       
            //
            ret = g_pInvokeSensorFunc[i]->SensorControl(ScenarioId,pImageWindow,pSensorConfigData);
            if ( ERROR_NONE != ret ) {
                PK_ERR("ERR:SensorControl(), i =%d\n",i);
                return ret;
            }
        }
    }
    KD_MULTI_FUNCTION_EXIT();


    //            
    //                       
        //                        
    // 
    //      
        if ( g_bEnableDriver[1] ) { //                                     
        MUINT8 frameSync = 0;
        MUINT32 frameSyncSize = 0;
            kd_MultiSensorFeatureControl (g_invokeSocketIdx[1],SENSOR_FEATURE_SUSPEND,&frameSync,&frameSyncSize);
            mDELAY(10);
            kd_MultiSensorFeatureControl (g_invokeSocketIdx[1],SENSOR_FEATURE_RESUME,&frameSync,&frameSyncSize);
        }
    // 


    return ERROR_NONE;
}
//
MUINT32
kd_MultiSensorClose(void)
{
    MUINT32 ret = ERROR_NONE;
    u32 i = 0;
    KD_MULTI_FUNCTION_ENTRY();
    for ( i = KDIMGSENSOR_INVOKE_DRIVER_0 ; i < KDIMGSENSOR_MAX_INVOKE_DRIVERS ; i++ ) {
        if ( g_bEnableDriver[i] && g_pInvokeSensorFunc[i] ) {
		/*                                                         
                                 
                                     
                                   
                                                           
   
        
                                 
                                     
                                   
                                                           
   */

            ret = g_pInvokeSensorFunc[i]->SensorClose();
            if ( ERROR_NONE != ret ) {
                PK_ERR("[%s]\n",__FUNCTION__);
                return ret;
            }
        }
    }
    KD_MULTI_FUNCTION_EXIT();
    return ERROR_NONE;
}
//
MULTI_SENSOR_FUNCTION_STRUCT  kd_MultiSensorFunc =
{
    kd_MultiSensorOpen,
    kd_MultiSensorGetInfo,
    kd_MultiSensorGetResolution,
    kd_MultiSensorFeatureControl,
    kd_MultiSensorControl,
    kd_MultiSensorClose
};


/*                                                                              
                 
                                                                               */
int
kdModulePowerOn(
CAMERA_DUAL_CAMERA_SENSOR_ENUM socketIdx[KDIMGSENSOR_MAX_INVOKE_DRIVERS],
char sensorNameStr[KDIMGSENSOR_MAX_INVOKE_DRIVERS][32],
BOOL On,
char* mode_name )
{
MINT32 ret = ERROR_NONE;
u32 i = 0;

    for ( i = KDIMGSENSOR_INVOKE_DRIVER_0 ; i < KDIMGSENSOR_MAX_INVOKE_DRIVERS ; i++ ) {
        if ( g_bEnableDriver[i] ) {
            //                                                                                                                 
            ret = kdCISModulePowerOn(socketIdx[i],sensorNameStr[i],On,mode_name);
            if ( ERROR_NONE != ret ) {
                PK_ERR("[%s]",__FUNCTION__);
                return ret;
            }
        }
    }
    return ERROR_NONE;
}

/*                                                                              
             
                                                                               */
int kdSetDriver(unsigned int* pDrvIndex)
{
    ACDK_KD_SENSOR_INIT_FUNCTION_STRUCT *pSensorList = NULL;
    u32 drvIdx[KDIMGSENSOR_MAX_INVOKE_DRIVERS] = {0,0};
    u32 i;

    PK_XLOG_INFO("pDrvIndex:0x%08x/0x%08x \n",pDrvIndex[KDIMGSENSOR_INVOKE_DRIVER_0],pDrvIndex[KDIMGSENSOR_INVOKE_DRIVER_1]);
    //                                 

    if (0 != kdGetSensorInitFuncList(&pSensorList))
    {
        PK_ERR("ERROR:kdGetSensorInitFuncList()\n");
        return -EIO;
    }

    for ( i = KDIMGSENSOR_INVOKE_DRIVER_0; i < KDIMGSENSOR_MAX_INVOKE_DRIVERS ; i++ ) {
        //
        spin_lock(&kdsensor_drv_lock);
        g_bEnableDriver[i] = FALSE;
        g_invokeSocketIdx[i] = (CAMERA_DUAL_CAMERA_SENSOR_ENUM)((pDrvIndex[i] & KDIMGSENSOR_DUAL_MASK_MSB)>>KDIMGSENSOR_DUAL_SHIFT);
		spin_unlock(&kdsensor_drv_lock);
        drvIdx[i] = (pDrvIndex[i] & KDIMGSENSOR_DUAL_MASK_LSB);
        //
        if ( DUAL_CAMERA_NONE_SENSOR == g_invokeSocketIdx[i] ) { continue; }

		/*                                                         
                                 
                                     
                                   
                                                   
   
        
                                 
                                     
                                   
                                                      
   */

		//                              
		PK_XLOG_INFO("[kdSetDriver] i,g_invokeSocketIdx[%d] = %d :\n",i,i,drvIdx[i]);
		PK_XLOG_INFO("[kdSetDriver] i,drvIdx[%d] = %d :\n",i,i,drvIdx[i]);
        //
        if ( MAX_NUM_OF_SUPPORT_SENSOR > drvIdx[i] ) {
            if (NULL == pSensorList[drvIdx[i]].SensorInit) {
	            PK_ERR("ERROR:kdSetDriver()\n");
	            return -EIO;
    	    }

            pSensorList[drvIdx[i]].SensorInit(&g_pInvokeSensorFunc[i]);
            if (NULL == g_pInvokeSensorFunc[i]) {
                PK_ERR("ERROR:NULL g_pSensorFunc[%d]\n",i);
            return -EIO;
	        }
            //
            spin_lock(&kdsensor_drv_lock);
            g_bEnableDriver[i] = TRUE;
            g_CurrentInvokeCam = g_invokeSocketIdx[i];			
            spin_unlock(&kdsensor_drv_lock);
    	    //               
            memcpy((char*)g_invokeSensorNameStr[i],(char*)pSensorList[drvIdx[i]].drvname,sizeof(pSensorList[drvIdx[i]].drvname));
            //                
            //                                                          
            PK_XLOG_INFO("[kdSetDriver] :[%d][%d][%d][%s][%d]\n",i,g_bEnableDriver[i],g_invokeSocketIdx[i],g_invokeSensorNameStr[i],sizeof(pSensorList[drvIdx[i]].drvname));
        }
    }
    return 0;
}


/*                                                                              
                    
                                                                               */
int
kdGetSocketPostion(unsigned int* pSocketPos) {
    PK_XLOG_INFO("[%s][%d] \r\n",__FUNCTION__,*pSocketPos);
    switch (*pSocketPos) {
        case DUAL_CAMERA_MAIN_SENSOR:
            //                               
            //    
            *pSocketPos = IMGSENSOR_SOCKET_POS_RIGHT;
            break;
        //                               
        //                                            
        //          
        default:
        case DUAL_CAMERA_SUB_SENSOR:
            *pSocketPos = IMGSENSOR_SOCKET_POS_NONE;
            break;
    }
	return 0;
}
/*                                                                              
                     
                                                                               */
int kdSetSensorSyncFlag(BOOL bSensorSync)
{
	spin_lock(&kdsensor_drv_lock);

    bSesnorVsyncFlag = bSensorSync;
	spin_unlock(&kdsensor_drv_lock);
//                                                                  

    //                                                         

    return 0;
}

/*                                                                              
                      
                                                                               */
int kdCheckSensorPowerOn(void)
{
    if (atomic_read(&g_CamHWOpening)==0){//                
        return 0;
    }
    else {//               
        return 1;
    }
}

/*                                                                              
                         
                                                                               */
//                                                  
int kdSensorSyncFunctionPtr(void)
{
    unsigned int FeatureParaLen = 0;
    //                                                                                                                                                                                 
    mutex_lock(&kdCam_Mutex);
    if (NULL == g_pSensorFunc) {
        PK_ERR("ERROR:NULL g_pSensorFunc\n");
        mutex_unlock(&kdCam_Mutex);
        return -EIO;
    }
    //                                                                                                                            
    //              
    if (g_NewSensorExpGain.uSensorExpDelayFrame == 0) {
        FeatureParaLen = 2;
        g_pSensorFunc->SensorFeatureControl(DUAL_CAMERA_MAIN_SENSOR, SENSOR_FEATURE_SET_ESHUTTER, (unsigned char*)&g_NewSensorExpGain.u2SensorNewExpTime, (unsigned int*) &FeatureParaLen);
        g_NewSensorExpGain.uSensorExpDelayFrame = 0xFF; //        
    }
    else if(g_NewSensorExpGain.uSensorExpDelayFrame != 0xFF) {
        g_NewSensorExpGain.uSensorExpDelayFrame --;
    }

    //              
    if (g_NewSensorExpGain.uSensorGainDelayFrame == 0) {
        FeatureParaLen = 2;
        g_pSensorFunc->SensorFeatureControl(DUAL_CAMERA_MAIN_SENSOR, SENSOR_FEATURE_SET_GAIN, (unsigned char*)&g_NewSensorExpGain.u2SensorNewGain, (unsigned int*) &FeatureParaLen);
        g_NewSensorExpGain.uSensorGainDelayFrame = 0xFF; //        
     }
    else if(g_NewSensorExpGain.uSensorGainDelayFrame != 0xFF) {
        g_NewSensorExpGain.uSensorGainDelayFrame --;
     }

    //                                               
    if((g_NewSensorExpGain.uISPGainDelayFrame != 0xFF) && (g_NewSensorExpGain.uISPGainDelayFrame != 0)){
        g_NewSensorExpGain.uISPGainDelayFrame --;
    }
    mutex_unlock(&kdCam_Mutex);
    return 0;
}

/*                                                                              
                  
                                                                               */
int kdGetRawGainInfoPtr(UINT16 *pRAWGain)
{
    *pRAWGain = 0x00;
    *(pRAWGain+1) = 0x00;
    *(pRAWGain+2) = 0x00;
    *(pRAWGain+3) = 0x00;

    if(g_NewSensorExpGain.uISPGainDelayFrame == 0)    {  //                         
        *pRAWGain = g_NewSensorExpGain.u2ISPNewRGain;
      	 *(pRAWGain+1) = g_NewSensorExpGain.u2ISPNewGrGain;
	 *(pRAWGain+2) = g_NewSensorExpGain.u2ISPNewGbGain;
        *(pRAWGain+3) = g_NewSensorExpGain.u2ISPNewBGain;
//                                                                                                             
//                                                                        
		spin_lock(&kdsensor_drv_lock);
        g_NewSensorExpGain.uISPGainDelayFrame = 0xFF; //        
        spin_unlock(&kdsensor_drv_lock);
    }

    return 0;
}


/*                                                                              
                      
                                                                               */
inline static int adopt_CAMERA_HW_Open(void)
{
    UINT32 err = 0;

        KD_IMGSENSOR_PROFILE_INIT();
        //               
    //                                                                               
    if (atomic_read(&g_CamHWOpend) == 0  ) {
        //                                      
        //              
        //                                                                                                                    
        //                     
        //           
        //                                        
        //
        if (g_pSensorFunc) {
            err = g_pSensorFunc->SensorOpen();
            if(ERROR_NONE != err) {
                PK_DBG(" ERROR:SensorOpen(), turn off power \n");
                kdModulePowerOn((CAMERA_DUAL_CAMERA_SENSOR_ENUM*) g_invokeSocketIdx, g_invokeSensorNameStr, false, CAMERA_HW_DRVNAME1);
            }
        }
        else {
            PK_DBG(" ERROR:NULL g_pSensorFunc\n");
        }
        KD_IMGSENSOR_PROFILE("SensorOpen");
    }
    else {
        //                                                                                                                                     
	PK_ERR("adopt_CAMERA_HW_Open Fail, g_CamHWOpend = %d\n ",atomic_read(&g_CamHWOpend) );
    }

    if (err == 0 ) {
        atomic_set(&g_CamHWOpend, 1);
        //                              
    }

    return err?-EIO:err;
}	/*                        */

/*                                                                              
                              
                                                                               */
inline static int adopt_CAMERA_HW_CheckIsAlive(void)
{
    UINT32 err = 0;
    UINT32 err1 = 0;
	UINT32 i = 0;
    MUINT32 sensorID = 0;
    MUINT32 retLen = 0;


    KD_IMGSENSOR_PROFILE_INIT();
    //               
    kdModulePowerOn((CAMERA_DUAL_CAMERA_SENSOR_ENUM*)g_invokeSocketIdx, g_invokeSensorNameStr,true, CAMERA_HW_DRVNAME1);
    //                     
    mDELAY(10);
    KD_IMGSENSOR_PROFILE("kdModulePowerOn");

    if (g_pSensorFunc) {
		for ( i = KDIMGSENSOR_INVOKE_DRIVER_0 ; i < KDIMGSENSOR_MAX_INVOKE_DRIVERS ; i++ ) {
			if(DUAL_CAMERA_NONE_SENSOR != g_invokeSocketIdx[i]){
		        err = g_pSensorFunc->SensorFeatureControl(g_invokeSocketIdx[i], SENSOR_FEATURE_CHECK_SENSOR_ID, (MUINT8*)&sensorID, &retLen);
		        if (sensorID == 0) {    //                             
		            PK_DBG(" Not implement!!, use old open function to check\n");
		            err = ERROR_SENSOR_CONNECT_FAIL;
		        }
		        else if (sensorID == 0xFFFFFFFF) {    //                       
		            PK_DBG(" No Sensor Found");
		            err = ERROR_SENSOR_CONNECT_FAIL;
		        }
		        else {

		            PK_DBG(" Sensor found ID = 0x%x\n", sensorID);
		            err = ERROR_NONE;
		        }
		        if(ERROR_NONE != err)
		        {
		            PK_DBG("ERROR:adopt_CAMERA_HW_CheckIsAlive(), No imgsensor alive \n");
		        }
			}
		}
    }
    else {
        PK_DBG("ERROR:NULL g_pSensorFunc\n");
    }
    //
    //                                  
    err1 = g_pSensorFunc->SensorClose();
    if(ERROR_NONE != err1) {
        PK_DBG("SensorClose \n");
    }
    //
    kdModulePowerOn((CAMERA_DUAL_CAMERA_SENSOR_ENUM*)g_invokeSocketIdx, g_invokeSensorNameStr, false, CAMERA_HW_DRVNAME1);    
    //
    KD_IMGSENSOR_PROFILE("CheckIsAlive");

    return err?-EIO:err;
}	/*                        */


/*                                                                              
                               
                                                                               */
inline static int adopt_CAMERA_HW_GetResolution(void *pBuf)
{
	//                  
	PK_XLOG_INFO("[CAMERA_HW] adopt_CAMERA_HW_GetResolution\n");

    if (g_pSensorFunc) {
        g_pSensorFunc->SensorGetResolution(pBuf);
    }
    else {
        PK_DBG("[CAMERA_HW]ERROR:NULL g_pSensorFunc\n");
    }

	return 0;
}	/*                                 */


/*                                                                              
                         
                                                                               */
inline static int adopt_CAMERA_HW_GetInfo(void *pBuf)
{
    ACDK_SENSOR_GETINFO_STRUCT *pSensorGetInfo = (ACDK_SENSOR_GETINFO_STRUCT*)pBuf;
    MSDK_SENSOR_INFO_STRUCT info[2],*pInfo[2];
    MSDK_SENSOR_CONFIG_STRUCT config[2],*pConfig[2];
	MUINT32 *pScenarioId[2];
	u32 i=0;

	for(i=0; i<2; i++) {
	    pInfo[i]=&info[i];
		pConfig[i]=&config[i];
		pScenarioId[i]=&(pSensorGetInfo->ScenarioId[i]);
	}


    if (NULL == pSensorGetInfo) {
        PK_DBG("[CAMERA_HW] NULL arg.\n");
        return -EFAULT;
    }

    if ((NULL == pSensorGetInfo->pInfo[0]) || (NULL == pSensorGetInfo->pInfo[1]) ||
		(NULL == pSensorGetInfo->pConfig[0]) || (NULL == pSensorGetInfo->pConfig[1]))  {
        PK_DBG("[CAMERA_HW] NULL arg.\n");
        return -EFAULT;
    }

    if (g_pSensorFunc) {
        g_pSensorFunc->SensorGetInfo(pScenarioId,pInfo,pConfig);
    }
    else {
        PK_DBG("[CAMERA_HW]ERROR:NULL g_pSensorFunc\n");
    }



	for(i=0; i<2; i++) {
	    //          
	    if(copy_to_user((void __user *)(pSensorGetInfo->pInfo[i]), (void*)pInfo[i] , sizeof(MSDK_SENSOR_INFO_STRUCT))) {
	        PK_DBG("[CAMERA_HW][info] ioctl copy to user failed\n");
	        return -EFAULT;
	    }

	    //             
	    if(copy_to_user((void __user *) (pSensorGetInfo->pConfig[i]) , (void*)pConfig[i] , sizeof(MSDK_SENSOR_CONFIG_STRUCT))) {
	        PK_DBG("[CAMERA_HW][config] ioctl copy to user failed\n");
	        return -EFAULT;
	    }
	}
    return 0;
}	/*                           */


/*                                                                              
                         
                                                                               */
inline static int adopt_CAMERA_HW_Control(void *pBuf)
{
    int ret = 0;
    ACDK_SENSOR_CONTROL_STRUCT *pSensorCtrl = (ACDK_SENSOR_CONTROL_STRUCT*)pBuf;
    MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT imageWindow;
    MSDK_SENSOR_CONFIG_STRUCT sensorConfigData;
    memset(&imageWindow, 0, sizeof(ACDK_SENSOR_EXPOSURE_WINDOW_STRUCT));
    memset(&sensorConfigData, 0, sizeof(ACDK_SENSOR_CONFIG_STRUCT));

    if (NULL == pSensorCtrl ) {
        PK_DBG("[CAMERA_HW] NULL arg.\n");
        return -EFAULT;
    }

    if (NULL == pSensorCtrl->pImageWindow || NULL == pSensorCtrl->pSensorConfigData) {
        PK_DBG("[CAMERA_HW] NULL arg.\n");
        return -EFAULT;
    }

    if(copy_from_user((void*)&imageWindow , (void *) pSensorCtrl->pImageWindow, sizeof(ACDK_SENSOR_EXPOSURE_WINDOW_STRUCT))) {
        PK_DBG("[CAMERA_HW][pFeatureData32] ioctl copy from user failed\n");
        return -EFAULT;
    }

    if(copy_from_user((void*)&sensorConfigData , (void *) pSensorCtrl->pSensorConfigData, sizeof(ACDK_SENSOR_CONFIG_STRUCT))) {
        PK_DBG("[CAMERA_HW][pFeatureData32] ioctl copy from user failed\n");
        return -EFAULT;
    }

    //
    if (g_pSensorFunc) {
        ret = g_pSensorFunc->SensorControl(pSensorCtrl->ScenarioId,&imageWindow,&sensorConfigData);
    }
    else {
        PK_DBG("[CAMERA_HW]ERROR:NULL g_pSensorFunc\n");
    }

    //
    if(copy_to_user((void __user *) pSensorCtrl->pImageWindow, (void*)&imageWindow , sizeof(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT))) {
        PK_DBG("[CAMERA_HW][imageWindow] ioctl copy to user failed\n");
        return -EFAULT;
    }

    //
    if(copy_to_user((void __user *) pSensorCtrl->pSensorConfigData, (void*)&sensorConfigData , sizeof(MSDK_SENSOR_CONFIG_STRUCT))) {
        PK_DBG("[CAMERA_HW][imageWindow] ioctl copy to user failed\n");
        return -EFAULT;
    }
    return ret;
}//                       

/*                                                                              
                                
                                                                               */
inline static int  adopt_CAMERA_HW_FeatureControl(void *pBuf)
{
    ACDK_SENSOR_FEATURECONTROL_STRUCT *pFeatureCtrl = (ACDK_SENSOR_FEATURECONTROL_STRUCT*)pBuf;
    unsigned int FeatureParaLen = 0;
    void *pFeaturePara = NULL;

    ACDK_SENSOR_GROUP_INFO_STRUCT *pSensorGroupInfo = NULL;
    ACDK_KD_SENSOR_SYNC_STRUCT *pSensorSyncInfo = NULL;
    char kernelGroupNamePtr[128];
    unsigned char *pUserGroupNamePtr = NULL;
    signed int ret = 0;



    if (NULL == pFeatureCtrl ) {
        PK_ERR(" NULL arg.\n");
        return -EFAULT;
    }

    if(SENSOR_FEATURE_SINGLE_FOCUS_MODE == pFeatureCtrl->FeatureId || SENSOR_FEATURE_CANCEL_AF == pFeatureCtrl->FeatureId
		|| SENSOR_FEATURE_CONSTANT_AF == pFeatureCtrl->FeatureId) {//                                                       
    }
    else
    {
        if (NULL == pFeatureCtrl->pFeaturePara || NULL == pFeatureCtrl->pFeatureParaLen) {
            PK_ERR(" NULL arg.\n");
            return -EFAULT;
        }
    }

    if(copy_from_user((void*)&FeatureParaLen , (void *) pFeatureCtrl->pFeatureParaLen, sizeof(unsigned int))) {
        PK_ERR(" ioctl copy from user failed\n");
        return -EFAULT;
    }

    pFeaturePara = kmalloc(FeatureParaLen,GFP_KERNEL);
    if(NULL == pFeaturePara) {
        PK_ERR(" ioctl allocate mem failed\n");
        return -ENOMEM;
    }
	memset(pFeaturePara,0x0,FeatureParaLen);

    //              
	switch (pFeatureCtrl->FeatureId)
	{
        case SENSOR_FEATURE_SET_ESHUTTER:
        case SENSOR_FEATURE_SET_GAIN:
            //                           
            spin_lock(&kdsensor_drv_lock);
            g_NewSensorExpGain.uSensorExpDelayFrame = 0xFF;
            g_NewSensorExpGain.uSensorGainDelayFrame = 0xFF;
            g_NewSensorExpGain.uISPGainDelayFrame = 0xFF;
			spin_unlock(&kdsensor_drv_lock);
        case SENSOR_FEATURE_SET_ISP_MASTER_CLOCK_FREQ:
        case SENSOR_FEATURE_SET_REGISTER:
        case SENSOR_FEATURE_GET_REGISTER:
        case SENSOR_FEATURE_SET_CCT_REGISTER:
        case SENSOR_FEATURE_SET_ENG_REGISTER:
        case SENSOR_FEATURE_SET_ITEM_INFO:
        case SENSOR_FEATURE_GET_ITEM_INFO:
        case SENSOR_FEATURE_GET_ENG_INFO:
        case SENSOR_FEATURE_SET_VIDEO_MODE:
        case SENSOR_FEATURE_SET_YUV_CMD:
        case SENSOR_FEATURE_MOVE_FOCUS_LENS:
        case SENSOR_FEATURE_SET_AF_WINDOW:
        case SENSOR_FEATURE_SET_CALIBRATION_DATA:
        case SENSOR_FEATURE_SET_AUTO_FLICKER_MODE:
        case SENSOR_FEATURE_GET_EV_AWB_REF:
        case SENSOR_FEATURE_GET_SHUTTER_GAIN_AWB_GAIN:
        case SENSOR_FEATURE_SET_AE_WINDOW:
        case SENSOR_FEATURE_GET_EXIF_INFO:
	case SENSOR_FEATURE_GET_DELAY_INFO:
        case SENSOR_FEATURE_GET_AE_AWB_LOCK_INFO:
	case SENSOR_FEATURE_SET_MAX_FRAME_RATE_BY_SCENARIO:
	case SENSOR_FEATURE_GET_DEFAULT_FRAME_RATE_BY_SCENARIO:
 	case SENSOR_FEATURE_SET_TEST_PATTERN:
	case SENSOR_FEATURE_GET_TEST_PATTERN_CHECKSUM_VALUE:	
	case SENSOR_FEATURE_GET_SENSOR_CURRENT_TEMPERATURE:
        case SENSOR_FEATURE_AUTOTEST_CMD:	
        case SENSOR_FEATURE_GET_AE_FLASHLIGHT_INFO:
        case SENSOR_FEATURE_GET_TRIGGER_FLASHLIGHT_INFO: //                           
        case SENSOR_FEATURE_SET_YUV_3A_CMD: //                              
        case SENSOR_FEATURE_SET_YUV_JPEG_PARA:
        case SENSOR_FEATURE_GET_YUV_CAPTURE_OUTPUT_JPEG:
        case SENSOR_FEATURE_GET_YUV_JPEG_INFO:
            //
            if(copy_from_user((void*)pFeaturePara , (void *) pFeatureCtrl->pFeaturePara, FeatureParaLen)) {
                kfree(pFeaturePara);
                PK_DBG("[CAMERA_HW][pFeaturePara] ioctl copy from user failed\n");
                return -EFAULT;
            }
            break;
	 case SENSOR_FEATURE_SET_SENSOR_SYNC:    //                                                 
            if(copy_from_user((void*)pFeaturePara , (void *) pFeatureCtrl->pFeaturePara, FeatureParaLen)) {
	         PK_DBG("[CAMERA_HW][pFeaturePara] ioctl copy from user failed\n");
	         return -EFAULT;
	    }
	    //                                               
            pSensorSyncInfo = (ACDK_KD_SENSOR_SYNC_STRUCT*)pFeaturePara;
		spin_lock(&kdsensor_drv_lock);
	    g_NewSensorExpGain.u2SensorNewExpTime = pSensorSyncInfo->u2SensorNewExpTime;
            g_NewSensorExpGain.u2SensorNewGain = pSensorSyncInfo->u2SensorNewGain;
	    g_NewSensorExpGain.u2ISPNewRGain = pSensorSyncInfo->u2ISPNewRGain;
	    g_NewSensorExpGain.u2ISPNewGrGain = pSensorSyncInfo->u2ISPNewGrGain;
	    g_NewSensorExpGain.u2ISPNewGbGain = pSensorSyncInfo->u2ISPNewGbGain;
            g_NewSensorExpGain.u2ISPNewBGain = pSensorSyncInfo->u2ISPNewBGain;
            g_NewSensorExpGain.uSensorExpDelayFrame = pSensorSyncInfo->uSensorExpDelayFrame;
	    g_NewSensorExpGain.uSensorGainDelayFrame = pSensorSyncInfo->uSensorGainDelayFrame;
	    g_NewSensorExpGain.uISPGainDelayFrame = pSensorSyncInfo->uISPGainDelayFrame;
		//                                        
		if((0 == g_NewSensorExpGain.u2SensorNewExpTime) || (0xFFFF == g_NewSensorExpGain.u2SensorNewExpTime)) {
			g_NewSensorExpGain.uSensorExpDelayFrame = 0xFF;
		}

		if (g_NewSensorExpGain.uSensorExpDelayFrame == 0) {
			FeatureParaLen = 2;
			g_pSensorFunc->SensorFeatureControl(pFeatureCtrl->InvokeCamera,SENSOR_FEATURE_SET_ESHUTTER, (unsigned char*)&g_NewSensorExpGain.u2SensorNewExpTime, (unsigned int*) &FeatureParaLen);
			g_NewSensorExpGain.uSensorExpDelayFrame = 0xFF; //        
		}
		else if(g_NewSensorExpGain.uSensorExpDelayFrame != 0xFF) {
			g_NewSensorExpGain.uSensorExpDelayFrame --;
		}
		//              
		if (g_NewSensorExpGain.uSensorGainDelayFrame == 0) {
			FeatureParaLen = 2;
			g_pSensorFunc->SensorFeatureControl(pFeatureCtrl->InvokeCamera,SENSOR_FEATURE_SET_GAIN, (unsigned char*)&g_NewSensorExpGain.u2SensorNewGain, (unsigned int*) &FeatureParaLen);
			g_NewSensorExpGain.uSensorGainDelayFrame = 0xFF; //        
		}
		else if(g_NewSensorExpGain.uSensorGainDelayFrame != 0xFF) {
			g_NewSensorExpGain.uSensorGainDelayFrame --;
		}
		//                                               
		if((g_NewSensorExpGain.uISPGainDelayFrame != 0xFF) && (g_NewSensorExpGain.uISPGainDelayFrame != 0)){
			g_NewSensorExpGain.uISPGainDelayFrame --;
		}



	     break;
        case SENSOR_FEATURE_GET_GROUP_INFO:
            if(copy_from_user((void*)pFeaturePara , (void *) pFeatureCtrl->pFeaturePara, FeatureParaLen)) {
                kfree(pFeaturePara);
                PK_DBG("[CAMERA_HW][pFeaturePara] ioctl copy from user failed\n");
                return -EFAULT;
            }
            pSensorGroupInfo = (ACDK_SENSOR_GROUP_INFO_STRUCT*)pFeaturePara;
            pUserGroupNamePtr = pSensorGroupInfo->GroupNamePtr;
            //
            if (NULL == pUserGroupNamePtr) {
                kfree(pFeaturePara);
                PK_DBG("[CAMERA_HW] NULL arg.\n");
                return -EFAULT;
            }
            pSensorGroupInfo->GroupNamePtr = kernelGroupNamePtr;
            break;
        //            
        case SENSOR_FEATURE_GET_RESOLUTION:
        case SENSOR_FEATURE_GET_PERIOD:
        case SENSOR_FEATURE_GET_PIXEL_CLOCK_FREQ:
        case SENSOR_FEATURE_GET_REGISTER_DEFAULT:
        case SENSOR_FEATURE_GET_CONFIG_PARA:
        case SENSOR_FEATURE_GET_GROUP_COUNT:
        case SENSOR_FEATURE_GET_LENS_DRIVER_ID:
        //          
        case SENSOR_FEATURE_CAMERA_PARA_TO_SENSOR:
        case SENSOR_FEATURE_SENSOR_TO_CAMERA_PARA:
        case SENSOR_FEATURE_SINGLE_FOCUS_MODE:
        case SENSOR_FEATURE_CANCEL_AF:
        case SENSOR_FEATURE_CONSTANT_AF:
		default:
			break;
	}

    //
    if (g_pSensorFunc) {
            ret = g_pSensorFunc->SensorFeatureControl(pFeatureCtrl->InvokeCamera, pFeatureCtrl->FeatureId,(unsigned char*)pFeaturePara,(unsigned int*) &FeatureParaLen);
    }
    else {
        PK_DBG("[CAMERA_HW]ERROR:NULL g_pSensorFunc\n");
    }

    //            
	switch (pFeatureCtrl->FeatureId)
	{
        case SENSOR_FEATURE_SET_ESHUTTER:
        case SENSOR_FEATURE_SET_GAIN:
        case SENSOR_FEATURE_SET_ISP_MASTER_CLOCK_FREQ:
        case SENSOR_FEATURE_SET_REGISTER:
        case SENSOR_FEATURE_SET_CCT_REGISTER:
        case SENSOR_FEATURE_SET_ENG_REGISTER:
        case SENSOR_FEATURE_SET_ITEM_INFO:
        //          
        case SENSOR_FEATURE_CAMERA_PARA_TO_SENSOR:
        case SENSOR_FEATURE_SENSOR_TO_CAMERA_PARA:
            break;
        //            
        case SENSOR_FEATURE_GET_EV_AWB_REF:
        case SENSOR_FEATURE_GET_SHUTTER_GAIN_AWB_GAIN:
        case SENSOR_FEATURE_GET_EXIF_INFO:
		case SENSOR_FEATURE_GET_DELAY_INFO:
	case SENSOR_FEATURE_GET_AE_AWB_LOCK_INFO:
        case SENSOR_FEATURE_GET_RESOLUTION:
        case SENSOR_FEATURE_GET_PERIOD:
        case SENSOR_FEATURE_GET_PIXEL_CLOCK_FREQ:
        case SENSOR_FEATURE_GET_REGISTER:
        case SENSOR_FEATURE_GET_REGISTER_DEFAULT:
        case SENSOR_FEATURE_GET_CONFIG_PARA:
        case SENSOR_FEATURE_GET_GROUP_COUNT:
        case SENSOR_FEATURE_GET_LENS_DRIVER_ID:
        case SENSOR_FEATURE_GET_ITEM_INFO:
        case SENSOR_FEATURE_GET_ENG_INFO:
        case SENSOR_FEATURE_GET_AF_STATUS:
        case SENSOR_FEATURE_GET_AF_INF:
        case SENSOR_FEATURE_GET_AF_MACRO:
        case SENSOR_FEATURE_GET_AF_MAX_NUM_FOCUS_AREAS:
        case SENSOR_FEATURE_GET_TRIGGER_FLASHLIGHT_INFO: //                           
        case SENSOR_FEATURE_SET_YUV_3A_CMD: //                              
        case SENSOR_FEATURE_GET_AE_MAX_NUM_METERING_AREAS:
        case SENSOR_FEATURE_CHECK_SENSOR_ID:
		case SENSOR_FEATURE_GET_DEFAULT_FRAME_RATE_BY_SCENARIO:	
		case SENSOR_FEATURE_SET_TEST_PATTERN:
		case SENSOR_FEATURE_GET_TEST_PATTERN_CHECKSUM_VALUE:	
		case SENSOR_FEATURE_GET_SENSOR_CURRENT_TEMPERATURE:
        case SENSOR_FEATURE_GET_AE_FLASHLIGHT_INFO:
            //
            if(copy_to_user((void __user *) pFeatureCtrl->pFeaturePara, (void*)pFeaturePara , FeatureParaLen)) {
                kfree(pFeaturePara);
                PK_DBG("[CAMERA_HW][pSensorRegData] ioctl copy to user failed\n");
                return -EFAULT;
            }
            break;
        //                     
        case SENSOR_FEATURE_GET_GROUP_INFO:
            //             
            if(copy_to_user((void __user *) pUserGroupNamePtr, (void*)kernelGroupNamePtr , sizeof(char)*32)) {
                kfree(pFeaturePara);
                PK_DBG("[CAMERA_HW][pFeatureReturnPara32] ioctl copy to user failed\n");
                return -EFAULT;
            }
            pSensorGroupInfo->GroupNamePtr = pUserGroupNamePtr;
            if(copy_to_user((void __user *) pFeatureCtrl->pFeaturePara, (void*)pFeaturePara , FeatureParaLen)){
                kfree(pFeaturePara);
                PK_DBG("[CAMERA_HW][pFeatureReturnPara32] ioctl copy to user failed\n");
                return -EFAULT;
            }
            break;
	default:
			break;
	}

    kfree(pFeaturePara);
    if(copy_to_user((void __user *) pFeatureCtrl->pFeatureParaLen, (void*)&FeatureParaLen , sizeof(unsigned int))) {
        PK_DBG("[CAMERA_HW][pFeatureParaLen] ioctl copy to user failed\n");
        return -EFAULT;
    }
    return ret;
}	/*                                  */


/*                                                                              
                       
                                                                               */
inline static int adopt_CAMERA_HW_Close(void)
{
    //                                                                            
    if (atomic_read(&g_CamHWOpend) == 0){
        return 0; 
    }
    //                                                                                        
    else if(atomic_read(&g_CamHWOpend) == 1) {
        if (g_pSensorFunc) {
            g_pSensorFunc->SensorClose();
        }
        else {
            PK_DBG("[CAMERA_HW]ERROR:NULL g_pSensorFunc\n");
        }
        //                
        kdModulePowerOn((CAMERA_DUAL_CAMERA_SENSOR_ENUM*)g_invokeSocketIdx, g_invokeSensorNameStr, false, CAMERA_HW_DRVNAME1);
    }
    atomic_set(&g_CamHWOpend, 0);
    //                              
    atomic_set(&g_CamHWOpening, 0);

    //                           
    spin_lock(&kdsensor_drv_lock);
    g_NewSensorExpGain.uSensorExpDelayFrame = 0xFF;
    g_NewSensorExpGain.uSensorGainDelayFrame = 0xFF;
    g_NewSensorExpGain.uISPGainDelayFrame = 0xFF;
    g_CurrentInvokeCam = DUAL_CAMERA_NONE_SENSOR;
    spin_unlock(&kdsensor_drv_lock);

    return 0;
}	/*                         */


/*                                                                              
                 
                                                                               */

static long CAMERA_HW_Ioctl(
    struct file * a_pstFile,
    unsigned int a_u4Command,
    unsigned long a_u4Param
)
{
    int i4RetValue = 0;
    void * pBuff = NULL;
    u32 *pIdx = NULL;

    mutex_lock(&kdCam_Mutex);

    if(_IOC_NONE == _IOC_DIR(a_u4Command)) {
    }
    else {
        pBuff = kmalloc(_IOC_SIZE(a_u4Command),GFP_KERNEL);

        if(NULL == pBuff) {
            PK_DBG("[CAMERA SENSOR] ioctl allocate mem failed\n");
            i4RetValue = -ENOMEM;
            goto CAMERA_HW_Ioctl_EXIT;
        }

        if(_IOC_WRITE & _IOC_DIR(a_u4Command)){
            if(copy_from_user(pBuff , (void *) a_u4Param, _IOC_SIZE(a_u4Command))) {
                kfree(pBuff);
                PK_DBG("[CAMERA SENSOR] ioctl copy from user failed\n");
                i4RetValue =  -EFAULT;
                goto CAMERA_HW_Ioctl_EXIT;
            }
        }
    }

    pIdx = (u32*)pBuff;
    switch(a_u4Command) {
#if 0
        case KDIMGSENSORIOC_X_POWER_ON:
            i4RetValue = kdModulePowerOn((CAMERA_DUAL_CAMERA_SENSOR_ENUM) *pIdx, true, CAMERA_HW_DRVNAME);
            break;
        case KDIMGSENSORIOC_X_POWER_OFF:
            i4RetValue = kdModulePowerOn((CAMERA_DUAL_CAMERA_SENSOR_ENUM) *pIdx, false, CAMERA_HW_DRVNAME);
            break;
#endif
        case KDIMGSENSORIOC_X_SET_DRIVER:
            i4RetValue = kdSetDriver((unsigned int*)pBuff);
            break;
        case KDIMGSENSORIOC_T_OPEN:
            i4RetValue = adopt_CAMERA_HW_Open();
            break;
        case KDIMGSENSORIOC_X_GETINFO:
            i4RetValue = adopt_CAMERA_HW_GetInfo(pBuff);
            break;
        case KDIMGSENSORIOC_X_GETRESOLUTION:
            i4RetValue = adopt_CAMERA_HW_GetResolution(pBuff);
            break;
        case KDIMGSENSORIOC_X_FEATURECONCTROL:
            i4RetValue = adopt_CAMERA_HW_FeatureControl(pBuff);
            break;
        case KDIMGSENSORIOC_X_CONTROL:
            i4RetValue = adopt_CAMERA_HW_Control(pBuff);
            break;
        case KDIMGSENSORIOC_T_CLOSE:
            i4RetValue = adopt_CAMERA_HW_Close();
            break;
        case KDIMGSENSORIOC_T_CHECK_IS_ALIVE:
            i4RetValue = adopt_CAMERA_HW_CheckIsAlive();
            break;
        case KDIMGSENSORIOC_X_GET_SOCKET_POS:
            i4RetValue = kdGetSocketPostion((unsigned int*)pBuff);
            break;
        case KDIMGSENSORIOC_X_SET_I2CBUS:
            //                                   
            break;
        case KDIMGSENSORIOC_X_RELEASE_I2C_TRIGGER_LOCK:
            //                                       
            break;
    	default :
    		PK_DBG("No such command \n");
    		i4RetValue = -EPERM;
    		break;

    }

    if(_IOC_READ & _IOC_DIR(a_u4Command)) {
        if(copy_to_user((void __user *) a_u4Param , pBuff , _IOC_SIZE(a_u4Command))) {
            kfree(pBuff);
            PK_DBG("[CAMERA SENSOR] ioctl copy to user failed\n");
            i4RetValue =  -EFAULT;
            goto CAMERA_HW_Ioctl_EXIT;
        }
    }

    kfree(pBuff);
CAMERA_HW_Ioctl_EXIT:
    mutex_unlock(&kdCam_Mutex);
    return i4RetValue;
}

/*                                                                              
 
                                                                               */
//
//                                     
//                            
//

/*                                                                              
                           
          
             
                                                         
                                                              
                         
                                           
            
                                                                               */
static int CAMERA_HW_Open(struct inode * a_pstInode, struct file * a_pstFile)
{
    //                        
    if ( atomic_read(&g_CamDrvOpenCnt) == 0) {
	    //                 
	    //         
	    //                                                                       
	    //                                                                      
	    //                                                                         
		
	    //                                                                        
	    //                                                                       
	    //                                                                          
    }

    //
    atomic_inc(&g_CamDrvOpenCnt);
    return 0;
}

/*                                                                              
                            
              
                                                                
                                         
                                    
                                    
                                                                               */
static int CAMERA_HW_Release(struct inode * a_pstInode, struct file * a_pstFile)
{
    atomic_dec(&g_CamDrvOpenCnt);

    return 0;
}

static const struct file_operations g_stCAMERA_HW_fops =
{
    .owner = THIS_MODULE,
    .open = CAMERA_HW_Open,
    .release = CAMERA_HW_Release,
    .unlocked_ioctl = CAMERA_HW_Ioctl

};

#define CAMERA_HW_DYNAMIC_ALLOCATE_DEVNO 1
/*                                                                              
                          
                                                                               */
inline static int RegisterCAMERA_HWCharDrv(void)
{
    struct device* sensor_device = NULL;

#if CAMERA_HW_DYNAMIC_ALLOCATE_DEVNO
    if( alloc_chrdev_region(&g_CAMERA_HWdevno, 0, 1,CAMERA_HW_DRVNAME1) )
    {
        PK_DBG("[CAMERA SENSOR] Allocate device no failed\n");

        return -EAGAIN;
    }
#else
    if( register_chrdev_region(  g_CAMERA_HWdevno , 1 , CAMERA_HW_DRVNAME1) )
    {
        PK_DBG("[CAMERA SENSOR] Register device no failed\n");

        return -EAGAIN;
    }
#endif

    //               
    g_pCAMERA_HW_CharDrv = cdev_alloc();

    if(NULL == g_pCAMERA_HW_CharDrv)
    {
        unregister_chrdev_region(g_CAMERA_HWdevno, 1);

        PK_DBG("[CAMERA SENSOR] Allocate mem for kobject failed\n");

        return -ENOMEM;
    }

    //                       
    cdev_init(g_pCAMERA_HW_CharDrv, &g_stCAMERA_HW_fops);

    g_pCAMERA_HW_CharDrv->owner = THIS_MODULE;

    //             
    if(cdev_add(g_pCAMERA_HW_CharDrv, g_CAMERA_HWdevno, 1))
    {
        PK_DBG("[mt6516_IDP] Attatch file operation failed\n");

        unregister_chrdev_region(g_CAMERA_HWdevno, 1);

        return -EAGAIN;
    }

    sensor_class = class_create(THIS_MODULE, "sensordrv");
    if (IS_ERR(sensor_class)) {
        int ret = PTR_ERR(sensor_class);
        PK_DBG("Unable to create class, err = %d\n", ret);
        return ret;
    }
    sensor_device = device_create(sensor_class, NULL, g_CAMERA_HWdevno, NULL, CAMERA_HW_DRVNAME1);

    return 0;
}

/*                                                                              
                            
                                                                               */
inline static void UnregisterCAMERA_HWCharDrv(void)
{
    //                   
    cdev_del(g_pCAMERA_HW_CharDrv);

    unregister_chrdev_region(g_CAMERA_HWdevno, 1);

    device_destroy(sensor_class, g_CAMERA_HWdevno);
    class_destroy(sensor_class);
}
/*                                                                              
                     
                                                                               */
/*                                                                              
                     
                                                                               */
static int CAMERA_HW_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    int i4RetValue = 0;
    PK_DBG("[CAMERA_HW] Attach I2C \n");

    //                     
    spin_lock(&kdsensor_drv_lock);
    g_pstI2Cclient = client;
    //                  
    g_pstI2Cclient->timing = 300;//    
    spin_unlock(&kdsensor_drv_lock);

    //                    
    i4RetValue = RegisterCAMERA_HWCharDrv();

    if(i4RetValue){
        PK_ERR("[CAMERA_HW] register char device failed!\n");
        return i4RetValue;
    }

    //                             

    PK_DBG("[CAMERA_HW] Attached!! \n");
    return 0;
}


/*                                                                              
                      
                                                                               */
static int CAMERA_HW_i2c_remove(struct i2c_client *client)
{
    return 0;
}

/*                                                                              
                      
                                                                               */
struct i2c_driver CAMERA_HW_i2c_driver = {
    .probe = CAMERA_HW_i2c_probe,
    .remove = CAMERA_HW_i2c_remove,
    .driver.name = CAMERA_HW_DRVNAME1,
    .id_table = CAMERA_HW_i2c_id,
};


/*                                                                              
                  
                                                                            */


#if 0
/*                                                                              
                           
          
             
                                                         
                                                              
                         
                                           
            
                                                                               */
static int CAMERA_HW_Open2(struct inode * a_pstInode, struct file * a_pstFile)
{
    //
     if ( atomic_read(&g_CamDrvOpenCnt2) == 0) {
    	 //                                                                         

    	//                                                                          
    }
    atomic_inc(&g_CamDrvOpenCnt2);
    return 0;
}

/*                                                                              
                            
              
                                                                
                                         
                                    
                                    
                                                                               */
static int CAMERA_HW_Release2(struct inode * a_pstInode, struct file * a_pstFile)
{
    atomic_dec(&g_CamDrvOpenCnt2);

    return 0;
}


static const struct file_operations g_stCAMERA_HW_fops0 =
{
    .owner = THIS_MODULE,
    .open = CAMERA_HW_Open2,
    .release = CAMERA_HW_Release2,
    .unlocked_ioctl = CAMERA_HW_Ioctl

};



/*                                                                              
                          
                                                                               */
inline static int RegisterCAMERA_HWCharDrv2(void)
{
    struct device* sensor_device = NULL;
	UINT32 major;

#if CAMERA_HW_DYNAMIC_ALLOCATE_DEVNO
    if( alloc_chrdev_region(&g_CAMERA_HWdevno2, 0, 1, CAMERA_HW_DRVNAME2) )
    {
        PK_DBG("[CAMERA SENSOR] Allocate device no failed\n");

        return -EAGAIN;
    }
#else
    if( register_chrdev_region(  g_CAMERA_HWdevno2 , 1 , CAMERA_HW_DRVNAME2) )
    {
        PK_DBG("[CAMERA SENSOR] Register device no failed\n");

        return -EAGAIN;
    }
#endif

	major = MAJOR(g_CAMERA_HWdevno2);
    g_CAMERA_HWdevno2 = MKDEV(major,0);

    //               
    g_pCAMERA_HW_CharDrv2 = cdev_alloc();

    if(NULL == g_pCAMERA_HW_CharDrv2)
    {
        unregister_chrdev_region(g_CAMERA_HWdevno2, 1);

        PK_DBG("[CAMERA SENSOR] Allocate mem for kobject failed\n");

        return -ENOMEM;
    }

    //                       
    cdev_init(g_pCAMERA_HW_CharDrv2, &g_stCAMERA_HW_fops0);

    g_pCAMERA_HW_CharDrv2->owner = THIS_MODULE;

    //             
    if(cdev_add(g_pCAMERA_HW_CharDrv2, g_CAMERA_HWdevno2, 1))
    {
        PK_DBG("[mt6516_IDP] Attatch file operation failed\n");

        unregister_chrdev_region(g_CAMERA_HWdevno2, 1);

        return -EAGAIN;
    }

    sensor2_class = class_create(THIS_MODULE, "sensordrv2");
    if (IS_ERR(sensor2_class)) {
        int ret = PTR_ERR(sensor2_class);
        PK_DBG("Unable to create class, err = %d\n", ret);
        return ret;
    }
    sensor_device = device_create(sensor2_class, NULL, g_CAMERA_HWdevno2, NULL, CAMERA_HW_DRVNAME2);

    return 0;
}

inline static void UnregisterCAMERA_HWCharDrv2(void)
{
    //                   
    cdev_del(g_pCAMERA_HW_CharDrv2);

    unregister_chrdev_region(g_CAMERA_HWdevno2, 1);

    device_destroy(sensor2_class, g_CAMERA_HWdevno2);
    class_destroy(sensor2_class);
}


/*                                                                              
                     
                                                                               */
static int CAMERA_HW_i2c_probe2(struct i2c_client *client, const struct i2c_device_id *id)
{
    int i4RetValue = 0;
    PK_DBG("[CAMERA_HW] Attach I2C0 \n");

	spin_lock(&kdsensor_drv_lock);

    //                     
    g_pstI2Cclient2 = client;

    //                  
    g_pstI2Cclient2->timing = 200;//    
	spin_unlock(&kdsensor_drv_lock);

    //                    
    i4RetValue = RegisterCAMERA_HWCharDrv2();

    if(i4RetValue){
        PK_ERR("[CAMERA_HW] register char device failed!\n");
        return i4RetValue;
    }

    //                             

    PK_DBG("[CAMERA_HW] Attached!! \n");
    return 0;
}


/*                                                                              
                      
                                                                               */
static int CAMERA_HW_i2c_remove2(struct i2c_client *client)
{
    return 0;
}


/*                                                                              
                      
                                                                               */
struct i2c_driver CAMERA_HW_i2c_driver2 = {
    .probe = CAMERA_HW_i2c_probe2,
    .remove = CAMERA_HW_i2c_remove2,
    .driver.name = CAMERA_HW_DRVNAME2,
    .id_table = CAMERA_HW_i2c_id2,
};
#endif

/*                                                                              
                  
                                                                               */
static int CAMERA_HW_probe(struct platform_device *pdev)
{
    return i2c_add_driver(&CAMERA_HW_i2c_driver);
}

/*                                                                              
                    
                                                                               */
static int CAMERA_HW_remove(struct platform_device *pdev)
{
    i2c_del_driver(&CAMERA_HW_i2c_driver);
    return 0;
}

/*                                                                              
                    
                                                                               */
static int CAMERA_HW_suspend(struct platform_device *pdev, pm_message_t mesg)
{
    return 0;
}

/*                                                                              
                               
                                              
                                                                                 */
static int CAMERA_HW_resume(struct platform_device *pdev)
{
    return 0;
}

#if 0
/*                                                                              
                  
                                                                               */
static int CAMERA_HW_probe2(struct platform_device *pdev)
{
    return i2c_add_driver(&CAMERA_HW_i2c_driver2);
}

/*                                                                              
                    
                                                                               */
static int CAMERA_HW_remove2(struct platform_device *pdev)
{
    i2c_del_driver(&CAMERA_HW_i2c_driver2);
    return 0;
}

static int CAMERA_HW_suspend2(struct platform_device *pdev, pm_message_t mesg)
{
    return 0;
}


/*                                                                              
                               
                                              
                                                                                 */
static int CAMERA_HW_resume2(struct platform_device *pdev)
{
    return 0;
}

/*                                                                       
                   
                                                                          */
static struct platform_driver g_stCAMERA_HW_Driver2 = {
    .probe		= CAMERA_HW_probe2,
    .remove	    = CAMERA_HW_remove2,
    .suspend	= CAMERA_HW_suspend2,
    .resume	    = CAMERA_HW_resume2,
    .driver		= {
        .name	= "image_sensor_bus2",
        .owner	= THIS_MODULE,
    }
};

/*                                                                              
                  
                                                                               */

int iWriteTriggerReg(u16 a_u2Addr , u32 a_u4Data , u32 a_u4Bytes , u16 i2cId)
{
    int  i4RetValue = 0;
    int u4Index = 0;
    u8 * puDataInBytes = (u8 *)&a_u4Data;
    int retry = 3;
    char puSendCmd[6] = {(char)(a_u2Addr >> 8) , (char)(a_u2Addr & 0xFF) , 0 , 0 , 0 , 0};



    SET_I2CBUS_FLAG(gI2CBusNum);

    if(gI2CBusNum == SUPPORT_I2C_BUS_NUM1) {
		spin_lock(&kdsensor_drv_lock);
        g_pstI2Cclient->addr = (i2cId >> 1);
		spin_unlock(&kdsensor_drv_lock);
    }
    else {
		spin_lock(&kdsensor_drv_lock);
        g_pstI2Cclient2->addr = (i2cId >> 1);
		spin_unlock(&kdsensor_drv_lock);
    }


    if(a_u4Bytes > 2) {
        PK_DBG("[CAMERA SENSOR] exceed 2 bytes \n");
        return -1;
    }

    if(a_u4Data >> (a_u4Bytes << 3)) {
        PK_DBG("[CAMERA SENSOR] warning!! some data is not sent!! \n");
    }

    for(u4Index = 0 ; u4Index < a_u4Bytes ; u4Index += 1 ) {
        puSendCmd[(u4Index + 2)] = puDataInBytes[(a_u4Bytes - u4Index-1)];
    }

    do {
        if(gI2CBusNum == SUPPORT_I2C_BUS_NUM1) {
            i4RetValue = mt_i2c_master_send(g_pstI2Cclient, puSendCmd, (a_u4Bytes + 2), I2C_3DCAMERA_FLAG);
            if (i4RetValue < 0 ) {
                PK_DBG("[CAMERA SENSOR][ERROR]set i2c bus 1 master fail\n");
                CLEAN_I2CBUS_FLAG(gI2CBusNum);
                break;
            }
        }
        else {
            i4RetValue = mt_i2c_master_send(g_pstI2Cclient2, puSendCmd, (a_u4Bytes + 2), I2C_3DCAMERA_FLAG);
            if (i4RetValue < 0 ) {
                PK_DBG("[CAMERA SENSOR][ERROR]set i2c bus 0 master fail\n");
                CLEAN_I2CBUS_FLAG(gI2CBusNum);
                break;
            }
        }

        if (i4RetValue != (a_u4Bytes + 2)) {
            PK_DBG("[CAMERA SENSOR] I2C send failed addr = 0x%x, data = 0x%x !! \n", a_u2Addr, a_u4Data);
        }
        else {
            break;
        }
        uDELAY(50);
    } while ((retry --) > 0);

    return i4RetValue;
}
#endif


/*                                                                              
                                
                                               
                                                                                 */
static int  CAMERA_HW_DumpReg_To_Proc(char *page, char **start, off_t off,
                                                                                       int count, int *eof, void *data)
{
    return count;
}

/*                                                                              
                         
                                                    
                                                                                 */
static int  CAMERA_HW_Reg_Debug( struct file *file, const char *buffer, unsigned long count,
                                                                     void *data)
{
    char regBuf[64] = {'\0'};
    u32 u4CopyBufSize = (count < (sizeof(regBuf) - 1)) ? (count) : (sizeof(regBuf) - 1);

    MSDK_SENSOR_REG_INFO_STRUCT sensorReg;
    memset(&sensorReg, 0, sizeof(MSDK_SENSOR_REG_INFO_STRUCT));

    if (copy_from_user(regBuf, buffer, u4CopyBufSize))
        return -EFAULT;

    if (sscanf(regBuf, "%x %x",  &sensorReg.RegAddr, &sensorReg.RegData) == 2) {
        if (g_pSensorFunc != NULL) {
            g_pSensorFunc->SensorFeatureControl(DUAL_CAMERA_MAIN_SENSOR, SENSOR_FEATURE_SET_REGISTER, (MUINT8*)&sensorReg, (MUINT32*)sizeof(MSDK_SENSOR_REG_INFO_STRUCT));
            g_pSensorFunc->SensorFeatureControl(DUAL_CAMERA_MAIN_SENSOR, SENSOR_FEATURE_GET_REGISTER, (MUINT8*)&sensorReg, (MUINT32*)sizeof(MSDK_SENSOR_REG_INFO_STRUCT));
            PK_DBG("write addr = 0x%08x, data = 0x%08x\n", sensorReg.RegAddr, sensorReg.RegData);
        }
    }
    else if (sscanf(regBuf, "%x", &sensorReg.RegAddr) == 1) {
        if (g_pSensorFunc != NULL) {
            g_pSensorFunc->SensorFeatureControl(DUAL_CAMERA_MAIN_SENSOR, SENSOR_FEATURE_GET_REGISTER, (MUINT8*)&sensorReg, (MUINT32*)sizeof(MSDK_SENSOR_REG_INFO_STRUCT));
            PK_DBG("read addr = 0x%08x, data = 0x%08x\n", sensorReg.RegAddr, sensorReg.RegData);
        }
    }

    return count;
}


static int  CAMERA_HW_Reg_Debug2( struct file *file, const char *buffer, unsigned long count,
                                                                     void *data)
{
    char regBuf[64] = {'\0'};
    u32 u4CopyBufSize = (count < (sizeof(regBuf) - 1)) ? (count) : (sizeof(regBuf) - 1);

    MSDK_SENSOR_REG_INFO_STRUCT sensorReg;
    memset(&sensorReg, 0, sizeof(MSDK_SENSOR_REG_INFO_STRUCT));

    if (copy_from_user(regBuf, buffer, u4CopyBufSize))
        return -EFAULT;

    if (sscanf(regBuf, "%x %x",  &sensorReg.RegAddr, &sensorReg.RegData) == 2) {
        if (g_pSensorFunc != NULL) {
            g_pSensorFunc->SensorFeatureControl(DUAL_CAMERA_SUB_SENSOR, SENSOR_FEATURE_SET_REGISTER, (MUINT8*)&sensorReg, (MUINT32*)sizeof(MSDK_SENSOR_REG_INFO_STRUCT));
            g_pSensorFunc->SensorFeatureControl(DUAL_CAMERA_SUB_SENSOR, SENSOR_FEATURE_GET_REGISTER, (MUINT8*)&sensorReg, (MUINT32*)sizeof(MSDK_SENSOR_REG_INFO_STRUCT));
            PK_DBG("write addr = 0x%08x, data = 0x%08x\n", sensorReg.RegAddr, sensorReg.RegData);
        }
    }
    else if (sscanf(regBuf, "%x", &sensorReg.RegAddr) == 1) {
        if (g_pSensorFunc != NULL) {
            g_pSensorFunc->SensorFeatureControl(DUAL_CAMERA_SUB_SENSOR, SENSOR_FEATURE_GET_REGISTER, (MUINT8*)&sensorReg, (MUINT32*)sizeof(MSDK_SENSOR_REG_INFO_STRUCT));
            PK_DBG("read addr = 0x%08x, data = 0x%08x\n", sensorReg.RegAddr, sensorReg.RegData);
        }
    }

    return count;
}


/*                                                                       
                   
                                                                          */
static struct platform_driver g_stCAMERA_HW_Driver = {
    .probe		= CAMERA_HW_probe,
    .remove	    = CAMERA_HW_remove,
    .suspend	= CAMERA_HW_suspend,
    .resume	    = CAMERA_HW_resume,
    .driver		= {
        .name	= "image_sensor",
        .owner	= THIS_MODULE,
    }
};

/*                                                                       
                        
                                                                          */
//                  
#ifndef KERNEL_304
static const struct file_operations camsensor_proc_fops = {
    .owner = THIS_MODULE,
    .read = CAMERA_HW_DumpReg_To_Proc,
    .write = CAMERA_HW_Reg_Debug,
};
static const struct file_operations camsensor2_proc_fops = {
    .owner = THIS_MODULE,
    .read = CAMERA_HW_DumpReg_To_Proc,
    .write = CAMERA_HW_Reg_Debug2,
};
#endif

#if defined (ISX012_MIPI_YUV)||defined (HI707_YUV)
int CAMERA_HW_i2C_init(void)
#else
static int __init CAMERA_HW_i2C_init(void)
#endif
{
    struct proc_dir_entry *prEntry;

	//                                                              
    i2c_register_board_info(SUPPORT_I2C_BUS_NUM1, &i2c_devs1, 1);
    //                                                             


    if(platform_driver_register(&g_stCAMERA_HW_Driver)){
        PK_ERR("failed to register CAMERA_HW driver\n");
        return -ENODEV;
    }
    //                                                     
    //                                                    
    //                   
    // 

    //                                                 
#ifdef KERNEL_304
    prEntry = create_proc_entry("driver/camsensor", 0, NULL);
    if (prEntry) {
        prEntry->read_proc = CAMERA_HW_DumpReg_To_Proc;
        prEntry->write_proc = CAMERA_HW_Reg_Debug;
    }
    else {
        PK_ERR("add /proc/driver/camsensor entry fail \n");
    }

    //                                                
    prEntry = create_proc_entry("driver/camsensor2", 0, NULL);
    if (prEntry) {
        prEntry->read_proc = CAMERA_HW_DumpReg_To_Proc;
        prEntry->write_proc = CAMERA_HW_Reg_Debug2;
    }
    else {
        PK_ERR("add /proc/driver/camsensor2 entry fail \n");
    }
#else

    if ((prEntry = proc_create("driver/camsensor", 0, NULL,&camsensor_proc_fops)) == NULL) {
        PK_ERR("add /proc/driver/camsensor entry fail \n");
    }

    if ((prEntry = proc_create("driver/camsensor2", 0, NULL,&camsensor2_proc_fops)) == NULL) {
        PK_ERR("add /proc/driver/camsensor2 entry fail \n");
    }
#endif
    atomic_set(&g_CamHWOpend, 0); 
    //                              
    atomic_set(&g_CamDrvOpenCnt, 0);
    //                                 
    atomic_set(&g_CamHWOpening, 0);
    return 0;
}



#if defined (ISX012_MIPI_YUV)||defined (HI707_YUV)
EXPORT_SYMBOL(CAMERA_HW_i2C_init);
#endif

/*                                                                       
                        
                                                                          */
static void __exit CAMERA_HW_i2C_exit(void)
{
    platform_driver_unregister(&g_stCAMERA_HW_Driver);
    //                                                   
}


EXPORT_SYMBOL(kdSetSensorSyncFlag);
EXPORT_SYMBOL(kdSensorSyncFunctionPtr);
EXPORT_SYMBOL(kdGetRawGainInfoPtr);

#if defined (ISX012_MIPI_YUV)||defined (HI707_YUV)
#else
module_init(CAMERA_HW_i2C_init);
#endif
module_exit(CAMERA_HW_i2C_exit);

MODULE_DESCRIPTION("CAMERA_HW driver");
MODULE_AUTHOR("Jackie Su <jackie.su@Mediatek.com>");
MODULE_LICENSE("GPL");





