#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/mm_types.h>
#include <linux/module.h>
#include <generated/autoconf.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/delay.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/spinlock.h>
#include <linux/param.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/slab.h>

#include <linux/xlog.h>
#include <asm/io.h>

#include <mach/irqs.h>
#include <mach/mt_reg_base.h>
#include <mach/mt_irq.h>
#include <mach/irqs.h>
#include <mach/mt_clkmgr.h> //     
#include <mach/mt_irq.h>
#include <mach/sync_write.h>
#include <mach/m4u.h>

#include "ddp_hal.h"
#include "ddp_drv.h"
#include "ddp_reg.h"
#include "ddp_path.h"
#include "ddp_debug.h"
#include "ddp_bls.h"
#include "ddp_rdma.h"
#include "ddp_wdma.h"
#include "ddp_ovl.h"

#define DISP_INDEX_OFFSET 0x0  //                                   

unsigned int gMutexID = 0;
unsigned int gMemOutMutexID = 1;
extern unsigned int decouple_addr;
extern BOOL DISP_IsDecoupleMode(void);
extern UINT32 DISP_GetScreenWidth(void);
extern UINT32 DISP_GetScreenHeight(void);

UINT32 fb_width = 0;
UINT32 fb_height = 0;
static DEFINE_MUTEX(DpEngineMutexLock);

DECLARE_WAIT_QUEUE_HEAD(mem_out_wq);
unsigned int mem_out_done = 0;

static void disp_reg_backup_module(DISP_MODULE_ENUM module);
static void disp_reg_restore_module(DISP_MODULE_ENUM module);
extern void DpEngine_COLORonConfig(unsigned int srcWidth,unsigned int srcHeight);
extern    void DpEngine_COLORonInit(void);
extern BOOL DISP_IsVideoMode(void);


#ifdef DDP_USE_CLOCK_API
static int disp_reg_backup(void);
static int disp_reg_restore(void);
#endif

extern unsigned int disp_ms2jiffies(unsigned long ms);

extern unsigned char pq_debug_flag;
static DECLARE_WAIT_QUEUE_HEAD(g_disp_mutex_wq);
static unsigned int g_disp_mutex_reg_update = 0;

#if 0
static void dispsys_bypass_color(unsigned int width, unsigned int height)
{
    printk("dispsys_bypass_color, width=%d, height=%d \n", width, height);

    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_COLOR + 0xf50) = width;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_COLOR + 0xf54) = height;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_COLOR + 0x400) = 0x2000323c;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_COLOR + 0xf00) = 0x00000001;

    printk("dispsys_bypass_color, 0x%x, 0x%x, 0x%x, 0x%x \n",
        *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_COLOR + 0x400),
        *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_COLOR + 0xf00),
        *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_COLOR + 0xf50),
        *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_COLOR + 0xf54));
}

static void dispsys_bypass_bls(unsigned int width, unsigned int height)
{
    printk("dispsys_bypass_bls, width=%d, height=%d, reg=0x%x \n", 
        width, height, ((height<<16) + width));
    
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x000b0) = 0x00000001;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x000a0) = 0x000003ff;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x000b0) = 0x00000000;                      

    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00010) = 0x00010100;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00014) = 0x00000000;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00018) = ((height<<16) + width);
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00020) = 0x00010001;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00030) = 0x00000001;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00034) = 0x3fffffff;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00060) = 0x00002328;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00064) = 0x00006f54;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00068) = 0x00fa00b0;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x0006c) = 0x00800020;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00070) = 0x00000010;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00074) = 0x00f00060;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00078) = 0x00000000;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00080) = 0x00001770;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00084) = 0x00000008;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00088) = 0x00020001;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x0008c) = 0x00040001;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00090) = 0x01a201a2;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00094) = 0x00003cbc;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00098) = 0x01500150;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x0009c) = 0x00003d60;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x000a0) = 0x000003ff;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x000a4) = 0x00080001;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x000a8) = 0x00000001;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00300) = 0x00000000;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00304) = 0x00000004;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00308) = 0x00000010;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x0030C) = 0x00000024;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00310) = 0x00000040;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00314) = 0x00000064;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00318) = 0x00000090;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x0031C) = 0x000000C4;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00320) = 0x00000100;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00324) = 0x00000144;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00328) = 0x00000190;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x0032C) = 0x000001E4;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00330) = 0x00000240;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00334) = 0x000002A4;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00338) = 0x00000310;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x0033C) = 0x00000384;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00340) = 0x00000400;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00344) = 0x00000484;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00348) = 0x00000510;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x0034C) = 0x000005A4;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00350) = 0x00000640;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00354) = 0x000006E4;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00358) = 0x00000790;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x0035C) = 0x00000843;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00360) = 0x000008FF;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00364) = 0x000009C3;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00368) = 0x00000A8F;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x0036C) = 0x00000B63;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00370) = 0x00000C3F;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00374) = 0x00000D23;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00378) = 0x00000E0F;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x0037C) = 0x00000F03;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00380) = 0x00000FFF;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00384) = 0x00000FDF;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00e18) = 0x00003001;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00e2c) = 0x00002101;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00e30) = 0x00000001;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00e34) = 0x00000000;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00e3c) = 0x20200001;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00e40) = 0x20202020;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00e00) = 0x00000000; 
    
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00004) = 0x00000001;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00004) = 0x00000000;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x000a0) = 0x800003ff;  
    
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00008) = 0x0000001f;
    *(volatile kal_uint32 *)(DDP_REG_BASE_DISP_BLS + 0x00000) = 0x00010001;

    //                 
    //                                                                        

}
#endif


#if 0
int disp_path_get_mutex_()
{
    unsigned int cnt=0;
    
    DISP_REG_SET(DISP_REG_CONFIG_MUTEX(gMutexID), 1);
    DISP_REG_SET_FIELD(REG_FLD(1, gMutexID), DISP_REG_CONFIG_MUTEX_INTSTA, 0);

    while(((DISP_REG_GET(DISP_REG_CONFIG_MUTEX(gMutexID))& DISP_INT_MUTEX_BIT_MASK) != DISP_INT_MUTEX_BIT_MASK))
    {
        cnt++;
        if(cnt>10000)
        {
            printk("[DDP] disp_path_get_mutex() timeout! \n");
            disp_dump_reg(DISP_MODULE_CONFIG);
            break;
        }
    }

    return 0;
}

int disp_path_release_mutex_()
{
    DISP_REG_SET(DISP_REG_CONFIG_MUTEX(gMutexID), 0);
}
#endif

//                                   
#define DDP_MUTEX_INTR_BIT 0x0100
static void _disp_path_mutex_reg_update_cb(unsigned int param)
{
    if (param & (1 << gMutexID))
    {
        g_disp_mutex_reg_update = 1;
        wake_up_interruptible(&g_disp_mutex_wq);
    }
    if (param & (DDP_MUTEX_INTR_BIT << gMutexID)) {
    	DISP_ERR("mutex%d register update timeout! commit=0x%x, mod=0x%x \n",
    			gMutexID, DISP_REG_GET(DISP_REG_CONFIG_REG_COMMIT),
    			DISP_REG_GET(DISP_REG_CONFIG_MUTEX_MOD(gMutexID)));
    	//                  
    	DISP_REG_SET(DISP_REG_CONFIG_MUTEX_INTSTA, 0);
    	//              
    	disp_dump_reg(DISP_MODULE_CONFIG);
    	disp_dump_reg(DISP_MODULE_MUTEX);
    	disp_dump_reg(DISP_MODULE_OVL);
    	disp_dump_reg(DISP_MODULE_RDMA);
    	disp_dump_reg(DISP_MODULE_COLOR);
    	disp_dump_reg(DISP_MODULE_BLS);
    }
}

#if 1
#include "dsi_drv.h"
void disp_path_reset(void) {
	//              
 	OVLReset();

	//            
	RDMAReset(0);

	//            
	DpEngine_COLORonInit();
	DpEngine_COLORonConfig(fb_width, fb_height);

	//          
	DISP_REG_SET(DISP_REG_BLS_RST, 0x1);
	DISP_REG_SET(DISP_REG_BLS_RST, 0x0);
	disp_bls_init(fb_width, fb_height);

    //          
    DSI_Reset();
    msleep(1);
    DSI_Start();
}
#endif

int disp_path_ovl_reset(void)
{
    static unsigned int ovl_hw_reset = 0;
    int delay_cnt = 100;

    DISP_REG_SET(DISP_REG_OVL_RST, 0x1);              //           
    DISP_REG_SET(DISP_REG_OVL_RST, 0x0);
    while (delay_cnt && ((DISP_REG_GET(DISP_REG_OVL_FLOW_CTRL_DBG)&0x3ff) != 0x1) &&
    ((DISP_REG_GET(DISP_REG_OVL_FLOW_CTRL_DBG)&0x3ff) != 0x2)) {
        delay_cnt--;
        udelay(100);
    }
    if(delay_cnt ==0 )
    {
        DISP_MSG("sw reset timeout, flow_ctrl=0x%x \n", DISP_REG_GET(DISP_REG_OVL_FLOW_CTRL_DBG));
    }
    DISP_MSG("after sw reset in intr, flow_ctrl=0x%x \n", DISP_REG_GET(DISP_REG_OVL_FLOW_CTRL_DBG));
    if(((DISP_REG_GET(DISP_REG_OVL_FLOW_CTRL_DBG)&0x3ff) != 0x1) &&
    		((DISP_REG_GET(DISP_REG_OVL_FLOW_CTRL_DBG)&0x3ff) != 0x2))
    {
         delay_cnt = 100;
         while(delay_cnt && (((DISP_REG_GET(DDP_REG_BASE_SMI_LARB0)&0x1) == 0x1) ||
             (((DISP_REG_GET(DDP_REG_BASE_SMI_COMMON+0x404)>>12)&0x1) != 0x1)))
        {
            delay_cnt--;
            udelay(100);
        }
        if(delay_cnt ==0 )
        {
            DISP_MSG("wait smi idle timeout, smi_larb 0x%x, smi_comm 0x%x \n", 
                 DISP_REG_GET(DDP_REG_BASE_SMI_LARB0),DISP_REG_GET(DDP_REG_BASE_SMI_COMMON+0x404));
        }
        else
        {
             //                                                                                   
            disp_reg_backup_module(DISP_MODULE_OVL);         
            DISP_REG_SET(DISP_REG_CONFIG_MMSYS_SW_RST_B, ~(1<<8));
            DISP_REG_SET(DISP_REG_CONFIG_MMSYS_SW_RST_B, ~0);
            
            disp_reg_restore_module(DISP_MODULE_OVL);
            DISP_MSG("after hw reset, flow_ctrl=0x%x \n", DISP_REG_GET(DISP_REG_OVL_FLOW_CTRL_DBG));
            
            //                                                            
            if((ovl_hw_reset > 0) && (DISP_IsDecoupleMode() == 0))
            {
                 DISP_MSG("disp_path_reset called! \n");
                 disp_path_reset();
            }
            ovl_hw_reset++;      
        }
    }
    return 0;
}

unsigned int gNeedToRecover = 0; //                  
#ifdef DDP_USE_CLOCK_API
extern unsigned int* pRegBackup;
int disp_intr_restore(void);
static void disp_path_recovery(unsigned int mutexID)
{
	if(gNeedToRecover || (DISP_REG_GET(DISP_REG_CONFIG_MUTEX_MOD(mutexID))==0))
	{
		DISP_ERR("mutex%d_mod invalid, try to recover!\n", mutexID);
		gNeedToRecover = 0;
		if (*pRegBackup != DDP_UNBACKED_REG_MEM)
		{
			disp_reg_restore();
			disp_intr_restore();
		}
		else
		{
			DISP_ERR("recover failed!\n");
		}
	}
}
#endif

unsigned int disp_mutex_lock_cnt[MUTEX_RESOURCE_NUM] = {0};
unsigned int disp_mutex_unlock_cnt[MUTEX_RESOURCE_NUM] = {0};
int disp_path_get_mutex(void)
{
    if(pq_debug_flag == 3)
    {
        return 0;
    }
    else
    {
        disp_register_irq(DISP_MODULE_MUTEX, _disp_path_mutex_reg_update_cb);
        return disp_path_get_mutex_(gMutexID);
    }
}

int disp_path_get_mutex_(int mutexID)
{
    unsigned int cnt=0;
#ifdef DDP_USE_CLOCK_API
    //                                                 
    disp_path_recovery(mutexID);
#endif
    //                                                                                  
    //                               
    MMProfileLog(DDP_MMP_Events.Mutex[mutexID], MMProfileFlagStart);
    DISP_REG_SET(DISP_REG_CONFIG_MUTEX_EN(mutexID), 1);
    
    DISP_REG_SET(DISP_REG_CONFIG_MUTEX(mutexID), 1);
    DISP_REG_SET_FIELD(REG_FLD(1, mutexID), DISP_REG_CONFIG_MUTEX_INTSTA, 0);

    while(((DISP_REG_GET(DISP_REG_CONFIG_MUTEX(mutexID))& DISP_INT_MUTEX_BIT_MASK) != DISP_INT_MUTEX_BIT_MASK))
    {
        cnt++;
		udelay(1);
        if(cnt>10000)
        {
            DISP_ERR("disp_path_get_mutex() %d timeout! \n", mutexID);
			//           

            disp_clock_check();
            ddp_dump_info(DISP_MODULE_CONFIG);
            ddp_dump_info(DISP_MODULE_MUTEX);
            ddp_dump_info(DISP_MODULE_OVL);
            ddp_dump_info(DISP_MODULE_RDMA);
            ddp_dump_info(DISP_MODULE_COLOR);
            ddp_dump_info(DISP_MODULE_BLS);

            disp_dump_reg(DISP_MODULE_MUTEX);
            disp_dump_reg(DISP_MODULE_OVL);
            
            MMProfileLogEx(DDP_MMP_Events.Mutex[mutexID], MMProfileFlagPulse, 0, 0);
            break;
        }
    }

    return 0;
}
int disp_path_release_mutex(void)
{
    if(pq_debug_flag == 3)
    {
        return 0;
    }
    else
    {
        g_disp_mutex_reg_update = 0;
        return disp_path_release_mutex_(gMutexID);
    }
}

extern int disp_path_release_mutex1_(int mutexID);

int disp_path_release_mutex1(void)
{
    if(pq_debug_flag == 3)
    {
        return 0;
    }
    else
    {
        g_disp_mutex_reg_update = 0;
        return disp_path_release_mutex1_(gMutexID);
    }
}

//                                                            
//                                                   
#define DDP_SMI_COMMON_POWER_BIT     (1<<0)
#define DDP_SMI_LARB0_POWER_BIT      (1<<1)
#define DDP_OVL_POWER_BIT     (1<<8)
#define DDP_RDMA_POWER_BIT   (1<<7)
#define DDP_WDMA0_POWER_BIT   (1<<6)
#define DDP_BLS_POWER_BIT     (1<<5)
#define DDP_COLOR_POWER_BIT   (1<<4)

#define DDP_OVL_UPDATE_BIT     (1<<3)
#define DDP_RDMA_UPDATE_BIT   (1<<10)
#define DDP_COLOR_UPDATE_BIT   (1<<7)
#define DDP_BLS_UPDATE_BIT     (1<<9)
#define DDP_WDMA0_UPDATE_BIT   (1<<6)

//                                                
#define DDP_MUTEX_INTEN_BIT	0x0101
//          
#define DDP_OVL_IDLE_BIT       (1<<0)
#define DDP_OVL_WAIT_BIT       (1<<1)

int disp_check_engine_status(int mutexID)
{
    int result = 0;
    unsigned int engine = DISP_REG_GET(DISP_REG_CONFIG_MUTEX_MOD(mutexID)); 
    unsigned int mutex_inten = DISP_REG_GET(DISP_REG_CONFIG_MUTEX_INTEN);

    if((DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON0)&DDP_SMI_LARB0_POWER_BIT) != 0)
    {
        result = -1;
        DISP_ERR("smi clk abnormal, clk=0x%x \n", DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON0));
    }
    if( (mutex_inten & DDP_MUTEX_INTEN_BIT) != DDP_MUTEX_INTEN_BIT )
    {
    	DISP_ERR("mutex0 inten abnormal, inten=0x%x \n", mutex_inten);
    	DISP_REG_SET(DISP_REG_CONFIG_MUTEX_INTEN, mutex_inten | DDP_MUTEX_INTEN_BIT);
    }

    if(engine&DDP_OVL_UPDATE_BIT) //   
    {
    	//                     
        if(DISP_REG_GET(DISP_REG_OVL_EN)==0 || 
           (DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON0)&DDP_OVL_POWER_BIT) != 0)
        {
            result = -1;
            DISP_ERR("ovl abnormal, en=%d, clk=0x%x \n", 
                DISP_REG_GET(DISP_REG_OVL_EN), 
                DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON0));
        }
        //              
        if( ((DISP_REG_GET(DISP_REG_OVL_ROI_SIZE) & 0x0fff) < 2) ||
        		(((DISP_REG_GET(DISP_REG_OVL_ROI_SIZE) >> 16) & 0x0fff) < 2) )
        {
        	result = -1;
        	DISP_ERR("ovl abnormal, roiW=%d, roiH=%d \n",
        			DISP_REG_GET(DISP_REG_OVL_ROI_SIZE)&0x0fff,
        			(DISP_REG_GET(DISP_REG_OVL_ROI_SIZE) >> 16)&0x0fff);
        }
        //                                                                 
        if((DISP_IsVideoMode()==0 || DISP_IsDecoupleMode()) &&
           ((DISP_REG_GET(DISP_REG_OVL_FLOW_CTRL_DBG)&0x3ff) != 0x1) &&
           ((DISP_REG_GET(DISP_REG_OVL_FLOW_CTRL_DBG)&0x3ff) != 0x2))
        {
        	//                                                                    
        	//                    
        	result = -1;
        	DISP_ERR("ovl abnormal, flow_ctrl=0x%x, add_con=0x%x \n", 
        	    DISP_REG_GET(DISP_REG_OVL_FLOW_CTRL_DBG),
        	    DISP_REG_GET(DISP_REG_OVL_ADDCON_DBG));
        	disp_clock_check();
        	ddp_dump_info(DISP_MODULE_CONFIG);
            ddp_dump_info(DISP_MODULE_MUTEX);
            ddp_dump_info(DISP_MODULE_OVL);
        	disp_path_ovl_reset();
        }
    }
    if(engine&DDP_RDMA_UPDATE_BIT) //     
    {
        if((DISP_REG_GET(DISP_REG_RDMA_GLOBAL_CON)&0x1) ==0 || 
           (DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON0)&DDP_RDMA_POWER_BIT) != 0)
        {
            result = -1;
            DISP_ERR("rdma abnormal, en=%d, clk=0x%x \n",
                DISP_REG_GET(DISP_REG_RDMA_GLOBAL_CON), 
                DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON0));
        }
    }    
    if(engine&DDP_COLOR_UPDATE_BIT) //     
    {

    }
    if(engine&DDP_WDMA0_UPDATE_BIT) //     
    {
        if(DISP_REG_GET(DISP_REG_WDMA_EN)==0 || 
           (DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON0)&DDP_WDMA0_POWER_BIT) != 0)
        {
            result = -1;
            DISP_ERR("wdma0 abnormal, en=%d, clk=0x%x \n", 
                DISP_REG_GET(DISP_REG_WDMA_EN), 
                DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON0));
        }
    }
    if(engine&DDP_BLS_UPDATE_BIT) //   
    {

    }

    if(result!=0)
    {
        DISP_ERR("engine status error before release mutex, engine=0x%x, mutexID=%d \n", engine, mutexID);
        ddp_dump_info(DISP_MODULE_CONFIG);
        ddp_dump_info(DISP_MODULE_MUTEX);
        ddp_dump_info(DISP_MODULE_OVL);
        ddp_dump_info(DISP_MODULE_RDMA);
        ddp_dump_info(DISP_MODULE_DSI_CMD);
        disp_dump_reg(DISP_MODULE_OVL);
    }

    return result;

}


int disp_path_release_mutex_(int mutexID)
{
//                       
//                             
    disp_check_engine_status(mutexID);
    
    DISP_REG_SET(DISP_REG_CONFIG_MUTEX(mutexID), 0);
    
   
#if 0
    while(((DISP_REG_GET(DISP_REG_CONFIG_MUTEX_INTSTA) & (1<<mutexID)) != (1<<mutexID)))
    {
        if((DISP_REG_GET(DISP_REG_CONFIG_MUTEX_INTSTA) & (1<<(mutexID+6))) == (1<<(mutexID+6)))
        {
            DISP_ERR("disp_path_release_mutex() timeout! \n");
            disp_dump_reg(DISP_MODULE_CONFIG);
            //                  
            reg = DISP_REG_GET(DISP_REG_CONFIG_REG_COMMIT);
            if(reg!=0)
            {
                  if(reg&(1<<3))  { DISP_MSG(" OVL update reg timeout! \n"); disp_dump_reg(DISP_MODULE_OVL); }
                  if(reg&(1<<10))  { DISP_MSG(" RDMA1 update reg timeout! \n"); disp_dump_reg(DISP_MODULE_RDMA); }
                  if(reg&(1<<7))  { DISP_MSG(" COLOR update reg timeout! \n"); disp_dump_reg(DISP_MODULE_COLOR); }
                  if(reg&(1<<9))  { DISP_MSG(" BLS update reg timeout! \n"); disp_dump_reg(DISP_MODULE_BLS); }
                  if(reg&(1<<6))  { DISP_MSG(" WDMA0 update reg timeout! \n"); disp_dump_reg(DISP_MODULE_WDMA0); }
            }  
                     
            //           
            DISP_REG_SET(DISP_REG_CONFIG_MUTEX_RST(mutexID), 1);
            DISP_REG_SET(DISP_REG_CONFIG_MUTEX_RST(mutexID), 0);
            DISP_MSG("mutex reset done! \n");
            MMProfileLogEx(DDP_MMP_Events.Mutex0, MMProfileFlagPulse, mutexID, 1);
            break;
        }

        cnt++;
        if(cnt>1000)
        {
            DISP_ERR("disp_path_release_mutex() timeout! \n");
            MMProfileLogEx(DDP_MMP_Events.Mutex0, MMProfileFlagPulse, mutexID, 2);
            break;
        }
    }

    //             
    reg = DISP_REG_GET(DISP_REG_CONFIG_MUTEX_INTSTA);
    reg &= ~(1<<mutexID);
    reg &= ~(1<<(mutexID+6));
    DISP_REG_SET(DISP_REG_CONFIG_MUTEX_INTSTA, reg);
#endif
    MMProfileLog(DDP_MMP_Events.Mutex[mutexID], MMProfileFlagEnd);
    //                                 
    //                                                                            

    return 0;
}
int disp_path_release_mutex1_(int mutexID)
{

    disp_check_engine_status(mutexID);

    DISP_REG_SET(DISP_REG_CONFIG_MUTEX(mutexID), 0);


    MMProfileLog(DDP_MMP_Events.Mutex[mutexID], MMProfileFlagEnd);

    return 0;
}

int disp_path_release_soft_mutex(void)
{

    //                                 

    return 0;
}

int disp_path_wait_reg_update(void)
{
    int ret = 0;
    //                                            
    ret = wait_event_interruptible_timeout(
                    g_disp_mutex_wq,
                    g_disp_mutex_reg_update,
                    disp_ms2jiffies(1000) );
                    
    /*                  */
    if(ret==0) //        
    {
        DISP_ERR("disp_path_wait_reg_update timeout \n");
        disp_dump_reg(DISP_MODULE_CONFIG); 
        disp_dump_reg(DISP_MODULE_MUTEX);
        disp_dump_reg(DISP_MODULE_OVL);
        disp_dump_reg(DISP_MODULE_RDMA);
        disp_dump_reg(DISP_MODULE_COLOR);
        disp_dump_reg(DISP_MODULE_BLS);
    }
    else if(ret<0) //                 
    {
        DISP_ERR("disp_path_wait_reg_update  intr by a signal ret=%d \n", ret);
    }
    
    return 0;
}
int disp_path_change_tdshp_status(unsigned int layer, unsigned int enable)
{
    return 0;
}

//                                                                             
//                
//                         
void _disp_path_wdma_callback(unsigned int param);

int disp_path_get_mem_read_mutex (void) {
	disp_path_get_mutex_(gMutexID);
	return 0;
}

int disp_path_release_mem_read_mutex (void) {
	disp_path_release_mutex_(gMutexID);
	return 0;
}

int disp_path_get_mem_write_mutex (void) {
	disp_path_get_mutex_(gMemOutMutexID);
	return 0;
}

int disp_path_release_mem_write_mutex (void) {
	disp_path_release_mutex_(gMemOutMutexID);
	return 0;
}
/*           
                                                                 
                                   
                  
                      
                          
        
                       
                         
        
                        
                         
        
                        
                         
        
         
        
  

                   
 */

static int disp_path_init_m4u_port (DISP_MODULE_ENUM module) {
	int m4u_module = M4U_PORT_UNKNOWN;
	M4U_PORT_STRUCT portStruct;
	
	switch (module) {
	case DISP_MODULE_OVL:
		m4u_module = DISP_OVL_0;
		break;
	case DISP_MODULE_RDMA:
		m4u_module = DISP_RDMA;
		break;
	case DISP_MODULE_RDMA1:
		m4u_module = DISP_RDMA;
		break;
	case DISP_MODULE_WDMA0:
		m4u_module = DISP_WDMA;
		break;
	default:
		break;
	}
    
    portStruct.ePortID = m4u_module;		   //                                             
    portStruct.Virtuality = 1;
    portStruct.Security = 0;
    portStruct.domain = 0;            //                
    portStruct.Distance = 1;
    portStruct.Direction = 0;
    m4u_config_port(&portStruct);
    return 0;
}
/*            
                                                                
                                   
                            
 
                  
                      
                          
        
                       
                         
        
                        
                         
        
                        
                         
        
         
        
  
    
                                                                                        
                              
                            
                                                        
                            
                             
                                 
             
 */

/* 
                                                                             
                           
 */
int disp_path_config_rdma (RDMA_CONFIG_STRUCT* pRdmaConfig) {
	DISP_DBG("[DDP] config_rdma(), idx=%d, mode=%d, in_fmt=%d, addr=0x%x, out_fmt=%d, pitch=%d, x=%d, y=%d, byteSwap=%d, rgbSwap=%d \n ",
			pRdmaConfig->idx,
			pRdmaConfig->mode,       	 //                 
			pRdmaConfig->inputFormat,    //            
			pRdmaConfig->address,        //        
			pRdmaConfig->outputFormat,   //              
			pRdmaConfig->pitch,       	 //      
			pRdmaConfig->width,       	 //      
			pRdmaConfig->height,      	 //       
			pRdmaConfig->isByteSwap,  	 //          
			pRdmaConfig->isRGBSwap);
	//                                                                                     
	//                                                       
	RDMAConfig(pRdmaConfig->idx, pRdmaConfig->mode, pRdmaConfig->inputFormat, pRdmaConfig->address,
			pRdmaConfig->outputFormat, pRdmaConfig->pitch, pRdmaConfig->width,
			pRdmaConfig->height, pRdmaConfig->isByteSwap, pRdmaConfig->isRGBSwap);
	return 0;
}

/* 
                                                                          
                                   
 */
int disp_path_config_wdma (struct disp_path_config_mem_out_struct* pConfig) {
	DISP_DBG("[DDP] config_wdma(), idx=%d, dstAddr=%x, out_fmt=%d\n",
			0,
			pConfig->dstAddr,
			pConfig->outFormat);
	//                                                                                     
	WDMAConfigAddress(0, pConfig->dstAddr);

	return 0;
}

/* 
                                                                          
                                   
                                 
 */
int disp_path_config_wdma2 (WDMA_CONFIG_STRUCT* pConfig) {
	DISP_DBG("disp_path_config_wdma2(), idx=%d, dstAddr=0x%08x, fmt=%d, clip(%d,%d %d,%d), dst(%d,%d,%d)\n",
			pConfig->idx, pConfig->dstAddress, DP_COLOR_GET_UNIQUE_ID(pConfig->outputFormat),
			pConfig->clipX, pConfig->clipY, pConfig->clipWidth, pConfig->clipHeight,
			pConfig->dstWidth, pConfig->dstPitch, pConfig->dstPitchUV);
	//                                          
#if defined(CONFIG_TRUSTONIC_TEE_SUPPORT) && defined(CONFIG_MTK_SEC_VIDEO_PATH_SUPPORT)
    if (1 == secure_port_flag()) {
        MMProfileLog(MTKFB_MMP_Events.ConfigWDMASecure, MMProfileFlagStart);
        switch_wdma_apc(1);
        config_wdma_secure((void *)pConfig, sizeof(WDMA_CONFIG_STRUCT));
        switch_wdma_secure_port(1);
        MMProfileLog(MTKFB_MMP_Events.ConfigWDMASecure, MMProfileFlagEnd);
    }
    else
    {
        switch_wdma_secure_port(0);
#endif
	WDMAConfig2(pConfig->idx, pConfig->inputFormat, pConfig->srcWidth, pConfig->srcHeight,
			pConfig->clipX, pConfig->clipY, pConfig->clipWidth, pConfig->clipHeight,
			pConfig->outputFormat, pConfig->dstAddress, pConfig->dstWidth, pConfig->dstPitch, pConfig->dstPitchUV,
			pConfig->useSpecifiedAlpha, pConfig->alpha);
#if defined(CONFIG_TRUSTONIC_TEE_SUPPORT) && defined(CONFIG_MTK_SEC_VIDEO_PATH_SUPPORT)
        //                                              
        notify_decouple_mva(pConfig->dstAddress);
    }
#endif

	return 0;
}

int disp_path_config_ovl_roi (unsigned int w, unsigned int h) {
#if defined(CONFIG_TRUSTONIC_TEE_SUPPORT) && defined(CONFIG_MTK_SEC_VIDEO_PATH_SUPPORT)
    if (secure_port_flag() == 1)
    {
        config_ovl_roi_secure(w, h);
    }
    else
    {
#endif
	OVLROI(w, h, 0);
#if defined(CONFIG_TRUSTONIC_TEE_SUPPORT) && defined(CONFIG_MTK_SEC_VIDEO_PATH_SUPPORT)
	}
#endif
	return 0;
}

/* 
                   
                                    
                                                   
                                                             
                                
                           
 */
int disp_path_switch_ovl_mode (struct disp_path_config_ovl_mode_t *pConfig) {
    int reg_mutex_mod;
    //                  

    if (DISP_IsDecoupleMode()) {
    	//                                                                  
    	reg_mutex_mod = DISP_REG_GET(DISP_REG_CONFIG_MUTEX_MOD(gMutexID));
        //         
        DISP_REG_SET(DISP_REG_CONFIG_DISP_OVL_MOUT_EN, 1<<1);   //                         
    	DISP_REG_SET(DISP_REG_CONFIG_MUTEX_INTEN , 0x0303);
    	DISP_REG_SET(DISP_REG_CONFIG_MUTEX_INTSTA, (1<<gMemOutMutexID)|(1<<gMutexID));
        //       
        DISP_REG_SET(DISP_REG_CONFIG_MUTEX_MOD(gMutexID), reg_mutex_mod&~(1<<3)); 	//          
        //       
        DISP_REG_SET(DISP_REG_CONFIG_MUTEX_EN(gMemOutMutexID), 1); 					//             
        DISP_REG_SET(DISP_REG_CONFIG_MUTEX_MOD(gMemOutMutexID), (1<<3)|(1<<6)); 	//         
        DISP_REG_SET(DISP_REG_CONFIG_MUTEX_SOF(gMemOutMutexID), 0);					//           

        disp_register_irq(DISP_MODULE_WDMA0, _disp_path_wdma_callback);

        //             
        WDMAReset(0);
        WDMAConfig(0,
        		WDMA_INPUT_FORMAT_ARGB,
        		pConfig->roi.width,
        		pConfig->roi.height,
        		pConfig->roi.x,
        		pConfig->roi.y,
        		pConfig->roi.width,
        		pConfig->roi.height,
        		pConfig->format,
        		pConfig->address,
        		pConfig->roi.width,
        		1,
        		0);
        WDMAStart(0);

        //            
        RDMAConfig(0,
        		RDMA_MODE_MEMORY,       		//            
        		pConfig->format,    			//            
        		pConfig->address,       		//                             
        		RDMA_OUTPUT_FORMAT_ARGB,     	//              
        		pConfig->pitch,         		//      
        		pConfig->roi.width,
        		pConfig->roi.height,
        		0,                      		//         
        		0);                     		//            
        RDMAStart(0);
        //                                 
    } else {
    	//                                                                        
    	reg_mutex_mod = DISP_REG_GET(DISP_REG_CONFIG_MUTEX_MOD(gMutexID));
        //         
        DISP_REG_SET(DISP_REG_CONFIG_DISP_OVL_MOUT_EN, 1<<0);   //                        
    	//                                                    
    	DISP_REG_SET(DISP_REG_CONFIG_MUTEX_INTEN , 0x0101);
    	DISP_REG_SET(DISP_REG_CONFIG_MUTEX_INTSTA, 1<<gMutexID);
        //       
        DISP_REG_SET(DISP_REG_CONFIG_MUTEX_MOD(gMutexID), reg_mutex_mod|(1<<3)); //       
        //                                                                 
        //       
        DISP_REG_SET(DISP_REG_CONFIG_MUTEX_MOD(gMemOutMutexID), 0);
        DISP_REG_SET(DISP_REG_CONFIG_MUTEX_EN(gMemOutMutexID), 0); //              

        //            
        RDMAConfig(0,
        		RDMA_MODE_DIRECT_LINK,       //                 
        		pConfig->format,    		 //            
        		pConfig->address,            //        
        		RDMA_OUTPUT_FORMAT_ARGB,     //              
        		pConfig->pitch,         	 //      
        		pConfig->roi.width,
        		pConfig->roi.height,
        		0,                           //          
        		0);                          //            

        disp_unregister_irq(DISP_MODULE_WDMA0, _disp_path_wdma_callback);
        RDMAStart(0);
    }

    return 0;
}

int disp_path_wait_frame_done(void) {
    int ret = 0;
    ret = wait_event_interruptible_timeout(mem_out_wq, mem_out_done, disp_ms2jiffies(100) );
    if(ret==0)  {
        DISP_ERR("disp_path_wait_frame_done timeout \n");
        ddp_dump_info(DISP_MODULE_CONFIG);
        ddp_dump_info(DISP_MODULE_MUTEX);
        ddp_dump_info(DISP_MODULE_OVL);
        ddp_dump_info(DISP_MODULE_WDMA0);
        disp_dump_reg(DISP_MODULE_OVL);
        disp_dump_reg(DISP_MODULE_WDMA0);
        //                       
    } else if(ret<0) {
        DISP_ERR("disp_path_wait_frame_done intr by a signal ret=%d \n", ret);
    }
    mem_out_done = 0;
    return ret;
}
//                
//                         
/* 
                                                                                
                
                                                                               
                
 */
int disp_path_config_layer(OVL_CONFIG_STRUCT* pOvlConfig)
{
//                          

    DISP_DBG("[DDP] config_layer(), layer=%d, en=%d, src=%d, fmt=%d, addr=0x%x, x=%d, y=%d, pitch=%d, dst(%d, %d, %d, %d),keyEn=%d, key=%d, aen=%d, alpha=%d \n ", 
    pOvlConfig->layer,   //      
    pOvlConfig->layer_en,   
    pOvlConfig->source,   //                       
    pOvlConfig->fmt, 
    pOvlConfig->addr, //      
    pOvlConfig->src_x,  //  
    pOvlConfig->src_y,  //  
    pOvlConfig->src_pitch, //                   
    pOvlConfig->dst_x,  //  
    pOvlConfig->dst_y,  //  
    pOvlConfig->dst_w, //      
    pOvlConfig->dst_h, //       
    pOvlConfig->keyEn,  //         
    pOvlConfig->key,  //         
    pOvlConfig->aen, //             
    pOvlConfig->alpha);

    if (pOvlConfig->layer_en != 0 && pOvlConfig->addr == 0)
    {
        DISP_ERR("[DDP] config_layer(), invalid mva(%x) for layer %d while enable it\n ",
            pOvlConfig->addr, pOvlConfig->layer);
        return 0;
    }
    //               
    OVLLayerSwitch(pOvlConfig->layer, pOvlConfig->layer_en);
    if(pOvlConfig->layer_en!=0)
    {
        OVLLayerConfig(pOvlConfig->layer,   //      
                       pOvlConfig->source,   //                       
                       pOvlConfig->fmt, 
                       pOvlConfig->addr, //      
                       pOvlConfig->src_x,  //  
                       pOvlConfig->src_y,  //  
                       pOvlConfig->src_pitch, //                   
                       pOvlConfig->dst_x,  //  
                       pOvlConfig->dst_y,  //  
                       pOvlConfig->dst_w, //      
                       pOvlConfig->dst_h, //       
                       pOvlConfig->keyEn,  //         
                       pOvlConfig->key,  //         
                       pOvlConfig->aen, //             
                       pOvlConfig->alpha); //      
    }
    else //               
    {
        OVLLayerConfig(pOvlConfig->layer,   //      
                       OVL_LAYER_SOURCE_MEM,   //                       
                       eRGB888, //   
                       1, //      
                       0,  //  
                       0,  //  
                       0, //                   
                       0,  //  
                       0,  //  
                       0, //      
                       0, //       
                       0,  //         
                       0,  //         
                       0, //             
                       0); //      

    }

    //                                                                             

    return 0;
}

int disp_path_config_layer_addr(unsigned int layer, unsigned int addr)
{
    unsigned int reg_addr;

    DISP_DBG("[DDP]disp_path_config_layer_addr(), layer=%d, addr=0x%x\n ", layer, addr);

    switch(layer)
    {
        case 0:
            DISP_REG_SET(DISP_REG_OVL_L0_ADDR, addr);
            reg_addr = DISP_REG_OVL_L0_ADDR;
            break;
        case 1:
            DISP_REG_SET(DISP_REG_OVL_L1_ADDR, addr);
            reg_addr = DISP_REG_OVL_L1_ADDR;
            break;
        case 2:
            DISP_REG_SET(DISP_REG_OVL_L2_ADDR, addr);
            reg_addr = DISP_REG_OVL_L2_ADDR;
            break;
        case 3:
            DISP_REG_SET(DISP_REG_OVL_L3_ADDR, addr);
            reg_addr = DISP_REG_OVL_L3_ADDR;
            break;
        default:
            DISP_ERR("unknow layer=%d \n", layer);
    }
   
    return 0;
}

void _disp_path_wdma_callback(unsigned int param)
{
    mem_out_done = 1;
    wake_up_interruptible(&mem_out_wq);
}

void disp_path_wait_mem_out_done(void)
{
    int ret = 0;

    ret = wait_event_interruptible_timeout(
                    mem_out_wq, 
                    mem_out_done, 
                    disp_ms2jiffies(100) );
                    

    if(ret==0) //        
    {
        DISP_ERR("disp_path_wait_mem_out_done timeout \n");
        ddp_dump_info(DISP_MODULE_CONFIG); 
        ddp_dump_info(DISP_MODULE_MUTEX); 
        ddp_dump_info(DISP_MODULE_OVL); 
        ddp_dump_info(DISP_MODULE_WDMA0); 

        disp_dump_reg(DISP_MODULE_OVL);
        disp_dump_reg(DISP_MODULE_WDMA0); 

        //                       
        //              
    }
    else if(ret<0) //                 
    {
        DISP_ERR("disp_path_wait_mem_out_done intr by a signal ret=%d \n", ret);
    }
    
    mem_out_done = 0;    
}

//                                                                                          
//                                                                                                 
//                                                                         
//                                                                                          
void disp_path_clear_mem_out_done_flag(void)
{
    mem_out_done = 0;    
}

int  disp_path_query()
{
    return 0;
}


//                                                               
//                                                                        
//                                                                        
int disp_path_config_mem_out_without_lcd(struct disp_path_config_mem_out_struct* pConfig)
{
    static unsigned int reg_mutex_mod;
    static unsigned int reg_mutex_sof;
    static unsigned int reg_mout;
    
    DISP_DBG(" disp_path_config_mem_out(), enable = %d, outFormat=%d, dstAddr=0x%x, ROI(%d,%d,%d,%d) \n",
            pConfig->enable,
            pConfig->outFormat,            
            pConfig->dstAddr,  
            pConfig->srcROI.x, 
            pConfig->srcROI.y, 
            pConfig->srcROI.width, 
            pConfig->srcROI.height);
            
    if(pConfig->enable==1 && pConfig->dstAddr==0)
    {
          DISP_ERR("pConfig->dstAddr==0! \n");
    }

    if(pConfig->enable==1)
    {
        mem_out_done = 0;
        disp_register_irq(DISP_MODULE_WDMA0, _disp_path_wdma_callback);

        //             
        WDMAReset(0);
        WDMAConfig(0, 
                   WDMA_INPUT_FORMAT_ARGB, 
                   pConfig->srcROI.width,
                   pConfig->srcROI.height,
                   0,
                   0,
                   pConfig->srcROI.width, 
                   pConfig->srcROI.height, 
                   pConfig->outFormat, 
                   pConfig->dstAddr, 
                   pConfig->srcROI.width,
                   1, 
                   0);      
        WDMAStart(0);

        //             
        reg_mutex_mod = DISP_REG_GET(DISP_REG_CONFIG_MUTEX_MOD(gMutexID));
        DISP_REG_SET(DISP_REG_CONFIG_MUTEX_MOD(gMutexID), (1<<3)|(1<<6)); //          

        //          
        reg_mutex_sof = DISP_REG_GET(DISP_REG_CONFIG_MUTEX_SOF(gMutexID));
        DISP_REG_SET(DISP_REG_CONFIG_MUTEX_SOF(gMutexID), 0); //           
                
        //         
        reg_mout = DISP_REG_GET(DISP_REG_CONFIG_DISP_OVL_MOUT_EN);
        DISP_REG_SET(DISP_REG_CONFIG_DISP_OVL_MOUT_EN, 0x2);   //                         
        
        //                                 
    }
    else
    {
        //      
        DISP_REG_SET(DISP_REG_CONFIG_MUTEX_MOD(gMutexID), reg_mutex_mod);
        DISP_REG_SET(DISP_REG_CONFIG_MUTEX_SOF(gMutexID), reg_mutex_sof);         
        //         
        DISP_REG_SET(DISP_REG_CONFIG_DISP_OVL_MOUT_EN, reg_mout);        

        disp_unregister_irq(DISP_MODULE_WDMA0, _disp_path_wdma_callback);
    }

    return 0;
}

//                        
//                                                      
int disp_path_config_mem_out(struct disp_path_config_mem_out_struct* pConfig)
{
    unsigned int reg;
#if 1    
    DISP_MSG(" disp_path_config_mem_out(), enable = %d, outFormat=%d, dstAddr=0x%x, ROI(%d,%d,%d,%d) \n",
            pConfig->enable,
            pConfig->outFormat,            
            pConfig->dstAddr,  
            pConfig->srcROI.x, 
            pConfig->srcROI.y, 
            pConfig->srcROI.width, 
            pConfig->srcROI.height);
#endif
    if(pConfig->enable==1 && pConfig->dstAddr==0)
    {
          DISP_ERR("pConfig->dstAddr==0! \n");
    }

    if(pConfig->enable==1)
    {
        mem_out_done = 0;
        disp_register_irq(DISP_MODULE_WDMA0, _disp_path_wdma_callback);

        //             
        WDMAReset(0);
        WDMAConfig(0, 
                   WDMA_INPUT_FORMAT_ARGB, 
                   pConfig->srcROI.width,
                   pConfig->srcROI.height,
                   0,
                   0,
                   pConfig->srcROI.width, 
                   pConfig->srcROI.height, 
                   pConfig->outFormat, 
                   pConfig->dstAddr, 
                   pConfig->srcROI.width,
                   1, 
                   0);      
		if(pConfig->outFormat==eYUV_420_3P)
		{
			WDMAConfigUV(0, 
			pConfig->dstAddr+pConfig->srcROI.width*pConfig->srcROI.height, 
			pConfig->dstAddr+pConfig->srcROI.width*pConfig->srcROI.height*5/4, 
			pConfig->srcROI.width);
		} 
        WDMAStart(0);

        //      
        reg = DISP_REG_GET(DISP_REG_CONFIG_MUTEX_MOD(gMutexID));
        DISP_REG_SET(DISP_REG_CONFIG_MUTEX_MOD(gMutexID), reg|(1<<6)); //       
        
        //         
        reg = DISP_REG_GET(DISP_REG_CONFIG_DISP_OVL_MOUT_EN);
        DISP_REG_SET(DISP_REG_CONFIG_DISP_OVL_MOUT_EN, reg|(0x2));   //                               
        
        //                                 
    }
    else
    {
        //      
        reg = DISP_REG_GET(DISP_REG_CONFIG_MUTEX_MOD(gMutexID));
        DISP_REG_SET(DISP_REG_CONFIG_MUTEX_MOD(gMutexID), reg&(~(1<<6))); //       
        
        //         
        reg = DISP_REG_GET(DISP_REG_CONFIG_DISP_OVL_MOUT_EN);
        DISP_REG_SET(DISP_REG_CONFIG_DISP_OVL_MOUT_EN, reg&(~0x2));   //                       
        
        //             
        //             
        disp_unregister_irq(DISP_MODULE_WDMA0, _disp_path_wdma_callback);
    }

    return 0;
}


int disp_path_config(struct disp_path_config_struct* pConfig)
{
	fb_width = pConfig->srcROI.width;
	fb_height = pConfig->srcROI.height;
    return disp_path_config_(pConfig, gMutexID);
}

DISP_MODULE_ENUM g_dst_module;
#if defined(CONFIG_MTK_HDMI_SUPPORT)
extern bool is_hdmi_active(void);
#endif


int disp_path_config_(struct disp_path_config_struct* pConfig, int mutexId)
{
    unsigned int mutexMode;
    //                        
    
    DISP_DBG("[DDP] disp_path_config(), srcModule=%d, addr=0x%x, inFormat=%d, \n\
            pitch=%d, bgROI(%d,%d,%d,%d), bgColor=%d, outFormat=%d, dstModule=%d, dstAddr=0x%x, dstPitch=%d, mutexId=%d \n",
            pConfig->srcModule,
            pConfig->addr,
            pConfig->inFormat,
            pConfig->pitch,
            pConfig->bgROI.x,
            pConfig->bgROI.y,
            pConfig->bgROI.width,
            pConfig->bgROI.height,
            pConfig->bgColor,
            pConfig->outFormat,
            pConfig->dstModule,
            pConfig->dstAddr,
            pConfig->dstPitch,
            mutexId);
    g_dst_module = pConfig->dstModule;

#ifdef DDP_USE_CLOCK_API
#else
        //                                                       
        DISP_REG_SET(DISP_REG_CONFIG_MMSYS_CG_CLR0 , 0xFFFFFFFF);
        DISP_REG_SET(DISP_REG_CONFIG_MMSYS_CG_CLR1 , 0xFFFFFFFF);
#endif

        switch(pConfig->dstModule)
        {
            case DISP_MODULE_DSI_VDO:
                mutexMode = 1;
            break;

            case DISP_MODULE_DPI0:
                mutexMode = 2;
            break;

            case DISP_MODULE_DBI:
            case DISP_MODULE_DSI_CMD:
            case DISP_MODULE_WDMA0:
                mutexMode = 0;
            break;

            default:
                mutexMode = 0;
               DISP_ERR("unknown dstModule=%d \n", pConfig->dstModule); 
               return -1;
        }

       
        {
	        if(pConfig->dstModule==DISP_MODULE_WDMA0)
	        {
	            DISP_REG_SET(DISP_REG_CONFIG_MUTEX_MOD(mutexId), (1<<3)|(1<<6)); //          
	        }
	        else if(pConfig->srcModule==DISP_MODULE_OVL)
	        {
	        
	            DISP_REG_SET(DISP_REG_CONFIG_MUTEX_MOD(mutexId), (1<<3)|(1<<10)|(1<<7)|(1<<9)); //                      

	        }
	        else if(pConfig->srcModule==DISP_MODULE_RDMA)
	        {

	            DISP_REG_SET(DISP_REG_CONFIG_MUTEX_MOD(mutexId), (1<<10)|(1<<7)|(1<<9)); //                

	        } 
	        else if(pConfig->srcModule==DISP_MODULE_RDMA1)
	        {
	            //                                                            
	            DISP_REG_SET(DISP_REG_CONFIG_MUTEX_MOD(mutexId), (1<<10)|(1<<7)|(1<<9)); //                
	            //                                                                                   
	        }
        }
        DISP_REG_SET(DISP_REG_CONFIG_MUTEX_SOF(mutexId), mutexMode);
        DISP_REG_SET(DISP_REG_CONFIG_MUTEX_INTSTA, (1 << mutexId));

        DISP_REG_SET(DISP_REG_CONFIG_MUTEX_INTEN , 0x0101);
        DISP_REG_SET(DISP_REG_CONFIG_MUTEX_EN(mutexId), 1);
        if (DISP_IsDecoupleMode()) {
        	disp_path_init_m4u_port(DISP_MODULE_RDMA);
        	disp_path_init_m4u_port(DISP_MODULE_WDMA0);
        	DISP_REG_SET(DISP_REG_CONFIG_MUTEX_EN(gMemOutMutexID), 1);
        	DISP_REG_SET(DISP_REG_CONFIG_MUTEX_INTEN , 0x0303);
        	//                                       
        	DISP_REG_SET(DISP_REG_CONFIG_MUTEX_MOD(gMemOutMutexID), (1<<3)|(1<<6)); //           
        	DISP_REG_SET(DISP_REG_CONFIG_MUTEX_SOF(gMemOutMutexID), 0x0);//            
        	DISP_REG_SET(DISP_REG_CONFIG_MUTEX_INTSTA, (1 << gMemOutMutexID)|(1 << mutexId));
        }
        //                    
        switch(pConfig->dstModule)
        {
            case DISP_MODULE_DSI_VDO:
            case DISP_MODULE_DSI_CMD:
            	if (DISP_IsDecoupleMode()) {
            		DISP_REG_SET(DISP_REG_CONFIG_DISP_OVL_MOUT_EN, 1<<1);  //            
            	} else {
            		DISP_REG_SET(DISP_REG_CONFIG_DISP_OVL_MOUT_EN, 1<<0);  //           
            	}
                DISP_REG_SET(DISP_REG_CONFIG_DISP_OUT_SEL, 0);        //              
            break;

            case DISP_MODULE_DPI0:
				printk("DISI_MODULE_DPI0\n");
            	if (DISP_IsDecoupleMode()
#if defined(CONFIG_MTK_HDMI_SUPPORT)
            	    || is_hdmi_active()
#endif
            	) {
            		DISP_REG_SET(DISP_REG_CONFIG_DISP_OVL_MOUT_EN, 1<<1);  //            
            	} else {
            		DISP_REG_SET(DISP_REG_CONFIG_DISP_OVL_MOUT_EN, 1<<0);  //           
            	}
                DISP_REG_SET(DISP_REG_CONFIG_DISP_OUT_SEL, 1);         //               
            break;

            case DISP_MODULE_WDMA0:
                DISP_REG_SET(DISP_REG_CONFIG_DISP_OVL_MOUT_EN, 0x1<<1);   //      
            break;

            default:
               printk("[DDP] error! unknown dstModule=%d \n", pConfig->dstModule); 
        }    
        
        //                 
        if(pConfig->srcModule!=DISP_MODULE_RDMA1)
        {
        	//           
            OVLROI(pConfig->bgROI.width, //      
                   pConfig->bgROI.height, //       
                   pConfig->bgColor);//             

            if(pConfig->dstModule!=DISP_MODULE_DSI_VDO && pConfig->dstModule!=DISP_MODULE_DPI0)
            {
                OVLStop();
                //            
            }
            if(pConfig->ovl_config.layer<4)
            {
                OVLLayerSwitch(pConfig->ovl_config.layer, pConfig->ovl_config.layer_en);
                if(pConfig->ovl_config.layer_en!=0)
                {
                    if(pConfig->ovl_config.addr==0 ||
                       pConfig->ovl_config.dst_w==0    ||
                       pConfig->ovl_config.dst_h==0    )
                    {
                        DISP_ERR("ovl parameter invalidate, addr=0x%x, w=%d, h=%d \n",
                               pConfig->ovl_config.addr, 
                               pConfig->ovl_config.dst_w,
                               pConfig->ovl_config.dst_h);
                        return -1;
                    }
                
                    OVLLayerConfig(pConfig->ovl_config.layer,   //      
                                   pConfig->ovl_config.source,   //                       
                                   pConfig->ovl_config.fmt, 
                                   pConfig->ovl_config.addr, //      
                                   pConfig->ovl_config.src_x,  //  
                                   pConfig->ovl_config.src_y,  //  
                                   pConfig->ovl_config.src_pitch, //                   
                                   pConfig->ovl_config.dst_x,  //  
                                   pConfig->ovl_config.dst_y,  //  
                                   pConfig->ovl_config.dst_w, //      
                                   pConfig->ovl_config.dst_h, //       
                                   pConfig->ovl_config.keyEn,  //         
                                   pConfig->ovl_config.key,  //         
                                   pConfig->ovl_config.aen, //             
                                   pConfig->ovl_config.alpha); //      
                }
            }
            else
            {
                DISP_ERR("layer ID undefined! %d \n", pConfig->ovl_config.layer);
            }
            OVLStart();

            if(pConfig->dstModule==DISP_MODULE_WDMA0)  //                       
            {
                WDMAReset(0);
                if(pConfig->dstAddr==0 ||
                   pConfig->srcROI.width==0    ||
                   pConfig->srcROI.height==0    )
                {
                    DISP_ERR("wdma parameter invalidate, addr=0x%x, w=%d, h=%d \n",
                           pConfig->dstAddr, 
                           pConfig->srcROI.width,
                           pConfig->srcROI.height);
                    return -1;
                }

                WDMAConfig(0, 
                           WDMA_INPUT_FORMAT_ARGB, 
                           pConfig->srcROI.width, 
                           pConfig->srcROI.height, 
                           0, 
                           0, 
                           pConfig->srcROI.width, 
                           pConfig->srcROI.height, 
                           pConfig->outFormat, 
                           pConfig->dstAddr, 
                           pConfig->srcROI.width, 
                           1, 
                           0);      
                WDMAStart(0);
            }
            else    //                       
            {

                disp_bls_init(pConfig->srcROI.width, pConfig->srcROI.height);

               //                                                                                   
                DpEngine_COLORonInit();
                DpEngine_COLORonConfig(pConfig->srcROI.width,  //     
                                                     pConfig->srcROI.height); //      

								
                //                                                                            
                //            
                if(pConfig->dstModule!=DISP_MODULE_DSI_VDO && pConfig->dstModule!=DISP_MODULE_DPI0)
                {
                    RDMAStop(0);
                    RDMAReset(0);
                }
                if(pConfig->srcROI.width==0    ||
                   pConfig->srcROI.height==0    )
                {
                    DISP_ERR("rdma parameter invalidate, w=%d, h=%d \n",
                           pConfig->srcROI.width,
                           pConfig->srcROI.height);
                    return -1;
                }

                if (DISP_IsDecoupleMode()) {
                	printk("from de-couple\n");
                	WDMAReset(0);
                	if(decouple_addr==0 ||
                			pConfig->srcROI.width==0    ||
                			pConfig->srcROI.height==0    )
                	{
                		DISP_ERR("wdma parameter invalidate, addr=0x%x, w=%d, h=%d \n",
                				decouple_addr,
                				pConfig->srcROI.width,
                				pConfig->srcROI.height);
                		return -1;
                	}
                	WDMAConfig(0,
                			WDMA_INPUT_FORMAT_ARGB,
                			pConfig->srcROI.width,
                			pConfig->srcROI.height,
                			0,
                			0,
                			pConfig->srcROI.width,
                			pConfig->srcROI.height,
                			eRGB888,
                			decouple_addr,
                			pConfig->srcROI.width,
                			1,
                			0);
                	WDMAStart(0);
                	//                   
                	mem_out_done = 0;
                	disp_register_irq(DISP_MODULE_WDMA0, _disp_path_wdma_callback);

                	RDMAConfig(0,
                			RDMA_MODE_MEMORY,       	 //         
                			eRGB888,    				 //            
                			decouple_addr,               //                                     
                			RDMA_OUTPUT_FORMAT_ARGB,     //              
                			pConfig->srcROI.width*3,     //               
                			pConfig->srcROI.width,       //      
                			pConfig->srcROI.height,      //       
                			0,                           //          
                			0);
                	RDMAStart(0);
                } else {
                	RDMAConfig(0,
                			RDMA_MODE_DIRECT_LINK,       //                 
                			eRGB888,    //            
                			0,                        //        
                			pConfig->outFormat,          //              
                			pConfig->pitch,              //      
                			pConfig->srcROI.width,       //      
                			pConfig->srcROI.height,      //       
                			0,                           //         
                			0);                          //            

                	RDMAStart(0);
                }
            }
        }
        else  //                   
        {
            printk("from rdma1\n");
#if defined(CONFIG_MTK_HDMI_SUPPORT)
            disp_bls_init(pConfig->srcROI.width, pConfig->srcROI.height);
            //                                                                              
            DpEngine_COLORonInit();
            DpEngine_COLORonConfig(pConfig->srcROI.width,  //     
                           pConfig->srcROI.height); //      
#endif
            //            
            RDMAStop(1);
            RDMAReset(1);

            RDMAConfig(1, 
                           RDMA_MODE_MEMORY,       		//                 
                           eARGB8888,    				//            
                           pConfig->addr,               //        
                           pConfig->outFormat,   		//              
                           pConfig->pitch,              //      
                           pConfig->srcROI.width,       //      
                           pConfig->srcROI.height,      //       
                           0,                           //         
                           1);   

            RDMAStart(1);
    
                    //             
                //                                                                  

                //               
                //                                                                    
        }

#if 0
        disp_dump_reg(DISP_MODULE_OVL);
        disp_dump_reg(DISP_MODULE_WDMA0);
        disp_dump_reg(DISP_MODULE_COLOR);
        disp_dump_reg(DISP_MODULE_BLS);
        disp_dump_reg(DISP_MODULE_DPI0);
        disp_dump_reg(DISP_MODULE_RDMA);
        disp_dump_reg(DISP_MODULE_CONFIG);
        disp_dump_reg(DISP_MODULE_MUTEX);
#endif

/*                                               */
//             
    //                     
    DISP_REG_SET(DISP_REG_OVL_RDMA0_MEM_GMC_SETTING, 0x40402020);
    DISP_REG_SET(DISP_REG_OVL_RDMA1_MEM_GMC_SETTING, 0x40402020);
    DISP_REG_SET(DISP_REG_OVL_RDMA2_MEM_GMC_SETTING, 0x40402020);
    DISP_REG_SET(DISP_REG_OVL_RDMA3_MEM_GMC_SETTING, 0x40402020);
    //                 
    //                                                          
    //                 
    //                                                 
    //                                                 

    //                                                    
    DISP_REG_SET(DISP_REG_OVL_DATAPATH_CON, (1<<29));

    return 0;
}


#ifdef DDP_USE_CLOCK_API
unsigned int reg_offset = 0;
//                                                                                              
void reg_backup(unsigned int reg_addr)
{
   *(pRegBackup+reg_offset) = DISP_REG_GET(reg_addr);
#ifdef DDP_RECORD_REG_BACKUP_RESTORE
      printk("0x%08x(0x%08x), ", reg_addr, *(pRegBackup+reg_offset));
      if((reg_offset+1)%8==0)
          printk("\n");
#endif      
      reg_offset++;
      if(reg_offset>=DDP_BACKUP_REG_NUM)
      {
          DISP_ERR("reg_backup fail, reg_offset=%d, regBackupSize=%d \n", reg_offset, DDP_BACKUP_REG_NUM);        
      }
}

void reg_restore(unsigned int reg_addr)
{
      DISP_REG_SET(reg_addr, *(pRegBackup+reg_offset));
#ifdef DDP_RECORD_REG_BACKUP_RESTORE
      printk("0x%08x(0x%08x), ", reg_addr, DISP_REG_GET(reg_addr));
      if((reg_offset+1)%8==0)
          printk("\n");
#endif                
      reg_offset++;
      
      if(reg_offset>=DDP_BACKUP_REG_NUM)
      {
          DISP_ERR("reg_backup fail, reg_offset=%d, regBackupSize=%d \n", reg_offset, DDP_BACKUP_REG_NUM);        
      }
}

unsigned int g_reg_backup[200];
static void disp_reg_backup_module(DISP_MODULE_ENUM module)
{
    unsigned int index = 0;
    if(module==DISP_MODULE_OVL)
    {    
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_STA                     );           
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_INTEN                   ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_INTSTA                  ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_EN                      ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_TRIG                    ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_RST                     ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_ROI_SIZE                ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_DATAPATH_CON            ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_ROI_BGCLR               ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_SRC_CON                 ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L0_CON                  ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L0_SRCKEY               ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L0_SRC_SIZE             ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L0_OFFSET               ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L0_ADDR                 ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L0_PITCH                ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L1_CON                  ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L1_SRCKEY               ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L1_SRC_SIZE             ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L1_OFFSET               ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L1_ADDR                 ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L1_PITCH                ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L2_CON                  ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L2_SRCKEY               ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L2_SRC_SIZE             ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L2_OFFSET               ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L2_ADDR                 ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L2_PITCH                ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L3_CON                  ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L3_SRCKEY               ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L3_SRC_SIZE             ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L3_OFFSET               ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L3_ADDR                 ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L3_PITCH                ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_RDMA0_CTRL              ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_RDMA0_MEM_START_TRIG    ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_RDMA0_MEM_GMC_SETTING   ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_RDMA0_MEM_SLOW_CON      ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_RDMA0_FIFO_CTRL         ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_RDMA1_CTRL              ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_RDMA1_MEM_START_TRIG    ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_RDMA1_MEM_GMC_SETTING   ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_RDMA1_MEM_SLOW_CON      ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_RDMA1_FIFO_CTRL         ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_RDMA2_CTRL              ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_RDMA2_MEM_START_TRIG    ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_RDMA2_MEM_GMC_SETTING   ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_RDMA2_MEM_SLOW_CON      ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_RDMA2_FIFO_CTRL         ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_RDMA3_CTRL              ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_RDMA3_MEM_START_TRIG    ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_RDMA3_MEM_GMC_SETTING   ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_RDMA3_MEM_SLOW_CON      ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_RDMA3_FIFO_CTRL         ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L0_Y2R_PARA_R0          ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L0_Y2R_PARA_R1          ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L0_Y2R_PARA_G0          ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L0_Y2R_PARA_G1          ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L0_Y2R_PARA_B0          ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L0_Y2R_PARA_B1          ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L0_Y2R_PARA_YUV_A_0     ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L0_Y2R_PARA_YUV_A_1     ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L0_Y2R_PARA_RGB_A_0     ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L0_Y2R_PARA_RGB_A_1     ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L1_Y2R_PARA_R0          ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L1_Y2R_PARA_R1          ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L1_Y2R_PARA_G0          ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L1_Y2R_PARA_G1          ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L1_Y2R_PARA_B0          ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L1_Y2R_PARA_B1          ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L1_Y2R_PARA_YUV_A_0     ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L1_Y2R_PARA_YUV_A_1     ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L1_Y2R_PARA_RGB_A_0     ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L1_Y2R_PARA_RGB_A_1     ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L2_Y2R_PARA_R0          ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L2_Y2R_PARA_R1          ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L2_Y2R_PARA_G0          ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L2_Y2R_PARA_G1          ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L2_Y2R_PARA_B0          ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L2_Y2R_PARA_B1          ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L2_Y2R_PARA_YUV_A_0     ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L2_Y2R_PARA_YUV_A_1     ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L2_Y2R_PARA_RGB_A_0     ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L2_Y2R_PARA_RGB_A_1     ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L3_Y2R_PARA_R0          ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L3_Y2R_PARA_R1          ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L3_Y2R_PARA_G0          ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L3_Y2R_PARA_G1          ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L3_Y2R_PARA_B0          ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L3_Y2R_PARA_B1          ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L3_Y2R_PARA_YUV_A_0     ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L3_Y2R_PARA_YUV_A_1     ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L3_Y2R_PARA_RGB_A_0     ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_L3_Y2R_PARA_RGB_A_1     ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_DEBUG_MON_SEL           ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_RDMA0_MEM_GMC_SETTING2  ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_RDMA1_MEM_GMC_SETTING2  ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_RDMA2_MEM_GMC_SETTING2  ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_RDMA3_MEM_GMC_SETTING2  ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_FLOW_CTRL_DBG           ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_ADDCON_DBG              ); 
        g_reg_backup[index++] = DISP_REG_GET(DISP_REG_OVL_OUTMUX_DBG              ); 

    }
}

static void disp_reg_restore_module(DISP_MODULE_ENUM module)
{
    unsigned int index = 0;
    if(module==DISP_MODULE_OVL)
    {   
        DISP_REG_SET(DISP_REG_OVL_STA                     , g_reg_backup[index++]);           
        DISP_REG_SET(DISP_REG_OVL_INTEN                   , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_INTSTA                  , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_EN                      , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_TRIG                    , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_RST                     , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_ROI_SIZE                , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_DATAPATH_CON            , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_ROI_BGCLR               , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_SRC_CON                 , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L0_CON                  , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L0_SRCKEY               , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L0_SRC_SIZE             , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L0_OFFSET               , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L0_ADDR                 , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L0_PITCH                , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L1_CON                  , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L1_SRCKEY               , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L1_SRC_SIZE             , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L1_OFFSET               , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L1_ADDR                 , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L1_PITCH                , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L2_CON                  , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L2_SRCKEY               , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L2_SRC_SIZE             , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L2_OFFSET               , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L2_ADDR                 , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L2_PITCH                , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L3_CON                  , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L3_SRCKEY               , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L3_SRC_SIZE             , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L3_OFFSET               , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L3_ADDR                 , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L3_PITCH                , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_RDMA0_CTRL              , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_RDMA0_MEM_START_TRIG    , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_RDMA0_MEM_GMC_SETTING   , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_RDMA0_MEM_SLOW_CON      , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_RDMA0_FIFO_CTRL         , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_RDMA1_CTRL              , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_RDMA1_MEM_START_TRIG    , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_RDMA1_MEM_GMC_SETTING   , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_RDMA1_MEM_SLOW_CON      , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_RDMA1_FIFO_CTRL         , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_RDMA2_CTRL              , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_RDMA2_MEM_START_TRIG    , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_RDMA2_MEM_GMC_SETTING   , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_RDMA2_MEM_SLOW_CON      , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_RDMA2_FIFO_CTRL         , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_RDMA3_CTRL              , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_RDMA3_MEM_START_TRIG    , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_RDMA3_MEM_GMC_SETTING   , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_RDMA3_MEM_SLOW_CON      , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_RDMA3_FIFO_CTRL         , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L0_Y2R_PARA_R0          , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L0_Y2R_PARA_R1          , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L0_Y2R_PARA_G0          , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L0_Y2R_PARA_G1          , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L0_Y2R_PARA_B0          , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L0_Y2R_PARA_B1          , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L0_Y2R_PARA_YUV_A_0     , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L0_Y2R_PARA_YUV_A_1     , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L0_Y2R_PARA_RGB_A_0     , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L0_Y2R_PARA_RGB_A_1     , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L1_Y2R_PARA_R0          , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L1_Y2R_PARA_R1          , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L1_Y2R_PARA_G0          , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L1_Y2R_PARA_G1          , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L1_Y2R_PARA_B0          , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L1_Y2R_PARA_B1          , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L1_Y2R_PARA_YUV_A_0     , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L1_Y2R_PARA_YUV_A_1     , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L1_Y2R_PARA_RGB_A_0     , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L1_Y2R_PARA_RGB_A_1     , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L2_Y2R_PARA_R0          , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L2_Y2R_PARA_R1          , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L2_Y2R_PARA_G0          , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L2_Y2R_PARA_G1          , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L2_Y2R_PARA_B0          , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L2_Y2R_PARA_B1          , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L2_Y2R_PARA_YUV_A_0     , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L2_Y2R_PARA_YUV_A_1     , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L2_Y2R_PARA_RGB_A_0     , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L2_Y2R_PARA_RGB_A_1     , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L3_Y2R_PARA_R0          , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L3_Y2R_PARA_R1          , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L3_Y2R_PARA_G0          , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L3_Y2R_PARA_G1          , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L3_Y2R_PARA_B0          , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L3_Y2R_PARA_B1          , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L3_Y2R_PARA_YUV_A_0     , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L3_Y2R_PARA_YUV_A_1     , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L3_Y2R_PARA_RGB_A_0     , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_L3_Y2R_PARA_RGB_A_1     , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_DEBUG_MON_SEL           , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_RDMA0_MEM_GMC_SETTING2  , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_RDMA1_MEM_GMC_SETTING2  , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_RDMA2_MEM_GMC_SETTING2  , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_RDMA3_MEM_GMC_SETTING2  , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_FLOW_CTRL_DBG           , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_ADDCON_DBG              , g_reg_backup[index++]); 
        DISP_REG_SET(DISP_REG_OVL_OUTMUX_DBG              , g_reg_backup[index++]); 

    }

}


static int disp_mutex_backup(void)
{
    int i;

    reg_backup(DISP_REG_CONFIG_MUTEX_INTEN);
    reg_backup(DISP_REG_CONFIG_REG_UPD_TIMEOUT);

    for (i=0; i<4; i++)
    {
    	reg_backup(DISP_REG_CONFIG_MUTEX_EN(i));
    	reg_backup(DISP_REG_CONFIG_MUTEX_MOD(i));
    	reg_backup(DISP_REG_CONFIG_MUTEX_SOF(i));
    }

    return 0;
}

#if 0
static int disp_bls_backup()
{
    int i;

    reg_backup(DISP_REG_BLS_DEBUG);
    reg_backup(DISP_REG_BLS_PWM_DUTY);

    reg_backup(DISP_REG_BLS_BLS_SETTING);
    reg_backup(DISP_REG_BLS_SRC_SIZE);
    reg_backup(DISP_REG_BLS_GAMMA_SETTING);
    reg_backup(DISP_REG_BLS_GAMMA_BOUNDARY);

    /*                   */
    for (i=0; i<=32; i++)
    {
    	reg_backup(DISP_REG_BLS_LUMINANCE(i));
    }

    /*                   */
    reg_backup(DISP_REG_BLS_LUMINANCE_255);

    /*        */
    for (i=0; i<18; i++)
    {
    	reg_backup(DISP_REG_BLS_DITHER(i));
    }

    reg_backup(DISP_REG_BLS_INTEN);
    reg_backup(DISP_REG_BLS_EN);

    return 0;
}
#endif

static int disp_mutex_restore(void)
{
    int i;

    reg_restore(DISP_REG_CONFIG_MUTEX_INTEN);
    reg_restore(DISP_REG_CONFIG_REG_UPD_TIMEOUT);

    for (i=0; i<4; i++)
    {
    	reg_restore(DISP_REG_CONFIG_MUTEX_EN(i));
        reg_restore(DISP_REG_CONFIG_MUTEX_MOD(i));
        reg_restore(DISP_REG_CONFIG_MUTEX_SOF(i));
    }

    return 0;
}

#if 0
static int disp_bls_restore()
{
    int i;

    reg_restore(DISP_REG_BLS_DEBUG);
    reg_restore(DISP_REG_BLS_PWM_DUTY);

    reg_restore(DISP_REG_BLS_BLS_SETTING);
    reg_restore(DISP_REG_BLS_SRC_SIZE);
    reg_restore(DISP_REG_BLS_GAMMA_SETTING);
    reg_restore(DISP_REG_BLS_GAMMA_BOUNDARY);

    /*                   */
    for (i=0; i<=32; i++)
    {
    	reg_restore(DISP_REG_BLS_LUMINANCE(i));
    }

    /*                   */
    reg_restore(DISP_REG_BLS_LUMINANCE_255);

    /*        */
    for (i=0; i<18; i++)
    {
    	reg_restore(DISP_REG_BLS_DITHER(i));
    }

    reg_restore(DISP_REG_BLS_INTEN);
    reg_restore(DISP_REG_BLS_EN);

    return 0;
}
#endif

static int disp_reg_backup(void)
{
	unsigned int index;
	reg_offset = 0;
	DISP_MSG("disp_reg_backup() start, *pRegBackup=0x%x, reg_offset=%d  \n", *pRegBackup, reg_offset);

	//       
	reg_backup(DISP_REG_CONFIG_DISP_OVL_MOUT_EN );
	reg_backup(DISP_REG_CONFIG_DISP_OUT_SEL     );

	//            
	disp_mutex_backup();
	//    
	//                  

	//    
	reg_backup(DISP_REG_OVL_STA                     );
	reg_backup(DISP_REG_OVL_INTEN                   );
	reg_backup(DISP_REG_OVL_INTSTA                  );
	reg_backup(DISP_REG_OVL_EN                      );
	reg_backup(DISP_REG_OVL_TRIG                    );
	reg_backup(DISP_REG_OVL_RST                     );
	reg_backup(DISP_REG_OVL_ROI_SIZE                );
	reg_backup(DISP_REG_OVL_DATAPATH_CON            );
	reg_backup(DISP_REG_OVL_ROI_BGCLR               );
	reg_backup(DISP_REG_OVL_SRC_CON                 );
	reg_backup(DISP_REG_OVL_L0_CON                  );
	reg_backup(DISP_REG_OVL_L0_SRCKEY               );
	reg_backup(DISP_REG_OVL_L0_SRC_SIZE             );
	reg_backup(DISP_REG_OVL_L0_OFFSET               );
	reg_backup(DISP_REG_OVL_L0_ADDR                 );
	reg_backup(DISP_REG_OVL_L0_PITCH                );
	reg_backup(DISP_REG_OVL_L1_CON                  );
	reg_backup(DISP_REG_OVL_L1_SRCKEY               );
	reg_backup(DISP_REG_OVL_L1_SRC_SIZE             );
	reg_backup(DISP_REG_OVL_L1_OFFSET               );
	reg_backup(DISP_REG_OVL_L1_ADDR                 );
	reg_backup(DISP_REG_OVL_L1_PITCH                );
	reg_backup(DISP_REG_OVL_L2_CON                  );
	reg_backup(DISP_REG_OVL_L2_SRCKEY               );
	reg_backup(DISP_REG_OVL_L2_SRC_SIZE             );
	reg_backup(DISP_REG_OVL_L2_OFFSET               );
	reg_backup(DISP_REG_OVL_L2_ADDR                 );
	reg_backup(DISP_REG_OVL_L2_PITCH                );
	reg_backup(DISP_REG_OVL_L3_CON                  );
	reg_backup(DISP_REG_OVL_L3_SRCKEY               );
	reg_backup(DISP_REG_OVL_L3_SRC_SIZE             );
	reg_backup(DISP_REG_OVL_L3_OFFSET               );
	reg_backup(DISP_REG_OVL_L3_ADDR                 );
	reg_backup(DISP_REG_OVL_L3_PITCH                );
	reg_backup(DISP_REG_OVL_RDMA0_CTRL              );
	reg_backup(DISP_REG_OVL_RDMA0_MEM_START_TRIG    );
	reg_backup(DISP_REG_OVL_RDMA0_MEM_GMC_SETTING   );
	reg_backup(DISP_REG_OVL_RDMA0_MEM_SLOW_CON      );
	reg_backup(DISP_REG_OVL_RDMA0_FIFO_CTRL         );
	reg_backup(DISP_REG_OVL_RDMA1_CTRL              );
	reg_backup(DISP_REG_OVL_RDMA1_MEM_START_TRIG    );
	reg_backup(DISP_REG_OVL_RDMA1_MEM_GMC_SETTING   );
	reg_backup(DISP_REG_OVL_RDMA1_MEM_SLOW_CON      );
	reg_backup(DISP_REG_OVL_RDMA1_FIFO_CTRL         );
	reg_backup(DISP_REG_OVL_RDMA2_CTRL              );
	reg_backup(DISP_REG_OVL_RDMA2_MEM_START_TRIG    );
	reg_backup(DISP_REG_OVL_RDMA2_MEM_GMC_SETTING   );
	reg_backup(DISP_REG_OVL_RDMA2_MEM_SLOW_CON      );
	reg_backup(DISP_REG_OVL_RDMA2_FIFO_CTRL         );
	reg_backup(DISP_REG_OVL_RDMA3_CTRL              );
	reg_backup(DISP_REG_OVL_RDMA3_MEM_START_TRIG    );
	reg_backup(DISP_REG_OVL_RDMA3_MEM_GMC_SETTING   );
	reg_backup(DISP_REG_OVL_RDMA3_MEM_SLOW_CON      );
	reg_backup(DISP_REG_OVL_RDMA3_FIFO_CTRL         );
	reg_backup(DISP_REG_OVL_L0_Y2R_PARA_R0          );
	reg_backup(DISP_REG_OVL_L0_Y2R_PARA_R1          );
	reg_backup(DISP_REG_OVL_L0_Y2R_PARA_G0          );
	reg_backup(DISP_REG_OVL_L0_Y2R_PARA_G1          );
	reg_backup(DISP_REG_OVL_L0_Y2R_PARA_B0          );
	reg_backup(DISP_REG_OVL_L0_Y2R_PARA_B1          );
	reg_backup(DISP_REG_OVL_L0_Y2R_PARA_YUV_A_0     );
	reg_backup(DISP_REG_OVL_L0_Y2R_PARA_YUV_A_1     );
	reg_backup(DISP_REG_OVL_L0_Y2R_PARA_RGB_A_0     );
	reg_backup(DISP_REG_OVL_L0_Y2R_PARA_RGB_A_1     );
	reg_backup(DISP_REG_OVL_L1_Y2R_PARA_R0          );
	reg_backup(DISP_REG_OVL_L1_Y2R_PARA_R1          );
	reg_backup(DISP_REG_OVL_L1_Y2R_PARA_G0          );
	reg_backup(DISP_REG_OVL_L1_Y2R_PARA_G1          );
	reg_backup(DISP_REG_OVL_L1_Y2R_PARA_B0          );
	reg_backup(DISP_REG_OVL_L1_Y2R_PARA_B1          );
	reg_backup(DISP_REG_OVL_L1_Y2R_PARA_YUV_A_0     );
	reg_backup(DISP_REG_OVL_L1_Y2R_PARA_YUV_A_1     );
	reg_backup(DISP_REG_OVL_L1_Y2R_PARA_RGB_A_0     );
	reg_backup(DISP_REG_OVL_L1_Y2R_PARA_RGB_A_1     );
	reg_backup(DISP_REG_OVL_L2_Y2R_PARA_R0          );
	reg_backup(DISP_REG_OVL_L2_Y2R_PARA_R1          );
	reg_backup(DISP_REG_OVL_L2_Y2R_PARA_G0          );
	reg_backup(DISP_REG_OVL_L2_Y2R_PARA_G1          );
	reg_backup(DISP_REG_OVL_L2_Y2R_PARA_B0          );
	reg_backup(DISP_REG_OVL_L2_Y2R_PARA_B1          );
	reg_backup(DISP_REG_OVL_L2_Y2R_PARA_YUV_A_0     );
	reg_backup(DISP_REG_OVL_L2_Y2R_PARA_YUV_A_1     );
	reg_backup(DISP_REG_OVL_L2_Y2R_PARA_RGB_A_0     );
	reg_backup(DISP_REG_OVL_L2_Y2R_PARA_RGB_A_1     );
	reg_backup(DISP_REG_OVL_L3_Y2R_PARA_R0          );
	reg_backup(DISP_REG_OVL_L3_Y2R_PARA_R1          );
	reg_backup(DISP_REG_OVL_L3_Y2R_PARA_G0          );
	reg_backup(DISP_REG_OVL_L3_Y2R_PARA_G1          );
	reg_backup(DISP_REG_OVL_L3_Y2R_PARA_B0          );
	reg_backup(DISP_REG_OVL_L3_Y2R_PARA_B1          );
	reg_backup(DISP_REG_OVL_L3_Y2R_PARA_YUV_A_0     );
	reg_backup(DISP_REG_OVL_L3_Y2R_PARA_YUV_A_1     );
	reg_backup(DISP_REG_OVL_L3_Y2R_PARA_RGB_A_0     );
	reg_backup(DISP_REG_OVL_L3_Y2R_PARA_RGB_A_1     );
	reg_backup(DISP_REG_OVL_DEBUG_MON_SEL           );
	reg_backup(DISP_REG_OVL_RDMA0_MEM_GMC_SETTING2  );
	reg_backup(DISP_REG_OVL_RDMA1_MEM_GMC_SETTING2  );
	reg_backup(DISP_REG_OVL_RDMA2_MEM_GMC_SETTING2  );
	reg_backup(DISP_REG_OVL_RDMA3_MEM_GMC_SETTING2  );
	reg_backup(DISP_REG_OVL_FLOW_CTRL_DBG           );
	reg_backup(DISP_REG_OVL_ADDCON_DBG              );
	reg_backup(DISP_REG_OVL_OUTMUX_DBG              );
	//            
	for (index = 0; index < 2; index++) {
		reg_backup(DISP_REG_RDMA_INT_ENABLE        + index*DISP_INDEX_OFFSET);
		reg_backup(DISP_REG_RDMA_INT_STATUS        + index*DISP_INDEX_OFFSET);
		reg_backup(DISP_REG_RDMA_GLOBAL_CON        + index*DISP_INDEX_OFFSET);
		reg_backup(DISP_REG_RDMA_SIZE_CON_0        + index*DISP_INDEX_OFFSET);
		reg_backup(DISP_REG_RDMA_SIZE_CON_1        + index*DISP_INDEX_OFFSET);
		reg_backup(DISP_REG_RDMA_TARGET_LINE       + index*DISP_INDEX_OFFSET);
		reg_backup(DISP_REG_RDMA_MEM_CON           + index*DISP_INDEX_OFFSET);
		reg_backup(DISP_REG_RDMA_MEM_START_ADDR    + index*DISP_INDEX_OFFSET);
		reg_backup(DISP_REG_RDMA_MEM_SRC_PITCH     + index*DISP_INDEX_OFFSET);
		reg_backup(DISP_REG_RDMA_MEM_GMC_SETTING_0 + index*DISP_INDEX_OFFSET);
		reg_backup(DISP_REG_RDMA_MEM_SLOW_CON      + index*DISP_INDEX_OFFSET);
		reg_backup(DISP_REG_RDMA_MEM_GMC_SETTING_1 + index*DISP_INDEX_OFFSET);
		reg_backup(DISP_REG_RDMA_FIFO_CON          + index*DISP_INDEX_OFFSET);
		reg_backup(DISP_REG_RDMA_FIFO_LOG          + index*DISP_INDEX_OFFSET);
		reg_backup(DISP_REG_RDMA_CF_00             + index*DISP_INDEX_OFFSET);
		reg_backup(DISP_REG_RDMA_CF_01             + index*DISP_INDEX_OFFSET);
		reg_backup(DISP_REG_RDMA_CF_02             + index*DISP_INDEX_OFFSET);
		reg_backup(DISP_REG_RDMA_CF_10             + index*DISP_INDEX_OFFSET);
		reg_backup(DISP_REG_RDMA_CF_11             + index*DISP_INDEX_OFFSET);
		reg_backup(DISP_REG_RDMA_CF_12             + index*DISP_INDEX_OFFSET);
		reg_backup(DISP_REG_RDMA_CF_20             + index*DISP_INDEX_OFFSET);
		reg_backup(DISP_REG_RDMA_CF_21             + index*DISP_INDEX_OFFSET);
		reg_backup(DISP_REG_RDMA_CF_22             + index*DISP_INDEX_OFFSET);
		reg_backup(DISP_REG_RDMA_CF_PRE_ADD0       + index*DISP_INDEX_OFFSET);
		reg_backup(DISP_REG_RDMA_CF_PRE_ADD1       + index*DISP_INDEX_OFFSET);
		reg_backup(DISP_REG_RDMA_CF_PRE_ADD2       + index*DISP_INDEX_OFFSET);
		reg_backup(DISP_REG_RDMA_CF_POST_ADD0      + index*DISP_INDEX_OFFSET);
		reg_backup(DISP_REG_RDMA_CF_POST_ADD1      + index*DISP_INDEX_OFFSET);
		reg_backup(DISP_REG_RDMA_CF_POST_ADD2      + index*DISP_INDEX_OFFSET);
		reg_backup(DISP_REG_RDMA_DUMMY             + index*DISP_INDEX_OFFSET);
		reg_backup(DISP_REG_RDMA_DEBUG_OUT_SEL     + index*DISP_INDEX_OFFSET);
	}
	//     
	reg_backup(DISP_REG_WDMA_INTEN          );
	reg_backup(DISP_REG_WDMA_INTSTA         );
	reg_backup(DISP_REG_WDMA_EN             );
	reg_backup(DISP_REG_WDMA_RST            );
	reg_backup(DISP_REG_WDMA_SMI_CON        );
	reg_backup(DISP_REG_WDMA_CFG            );
	reg_backup(DISP_REG_WDMA_SRC_SIZE       );
	reg_backup(DISP_REG_WDMA_CLIP_SIZE      );
	reg_backup(DISP_REG_WDMA_CLIP_COORD     );
	reg_backup(DISP_REG_WDMA_DST_ADDR       );
	reg_backup(DISP_REG_WDMA_DST_W_IN_BYTE  );
	reg_backup(DISP_REG_WDMA_ALPHA          );
	reg_backup(DISP_REG_WDMA_BUF_ADDR       );
	reg_backup(DISP_REG_WDMA_STA            );
	reg_backup(DISP_REG_WDMA_BUF_CON1       );
	reg_backup(DISP_REG_WDMA_BUF_CON2       );
	reg_backup(DISP_REG_WDMA_C00            );
	reg_backup(DISP_REG_WDMA_C02            );
	reg_backup(DISP_REG_WDMA_C10            );
	reg_backup(DISP_REG_WDMA_C12            );
	reg_backup(DISP_REG_WDMA_C20            );
	reg_backup(DISP_REG_WDMA_C22            );
	reg_backup(DISP_REG_WDMA_PRE_ADD0       );
	reg_backup(DISP_REG_WDMA_PRE_ADD2       );
	reg_backup(DISP_REG_WDMA_POST_ADD0      );
	reg_backup(DISP_REG_WDMA_POST_ADD2      );
	reg_backup(DISP_REG_WDMA_DST_U_ADDR     );
	reg_backup(DISP_REG_WDMA_DST_V_ADDR     );
	reg_backup(DISP_REG_WDMA_DST_UV_PITCH   );
	reg_backup(DISP_REG_WDMA_DITHER_CON     );
	reg_backup(DISP_REG_WDMA_FLOW_CTRL_DBG  );
	reg_backup(DISP_REG_WDMA_EXEC_DBG       );
	reg_backup(DISP_REG_WDMA_CLIP_DBG       );


	DISP_MSG("disp_reg_backup() end, *pRegBackup=0x%x, reg_offset=%d \n", *pRegBackup, reg_offset);

	return 0;
}

static int disp_reg_restore(void)
{
	unsigned int index;
    reg_offset = 0;
    DISP_MSG("disp_reg_restore(*) start, *pRegBackup=0x%x, reg_offset=%d  \n", *pRegBackup, reg_offset);

    //       
    reg_restore(DISP_REG_CONFIG_DISP_OVL_MOUT_EN );     
    reg_restore(DISP_REG_CONFIG_DISP_OUT_SEL     );     
    
    //      
    disp_mutex_restore();
    //    
    //                   

    //    
    reg_restore(DISP_REG_OVL_STA                     );           
    reg_restore(DISP_REG_OVL_INTEN                   ); 
    reg_restore(DISP_REG_OVL_INTSTA                  ); 
    reg_restore(DISP_REG_OVL_EN                      ); 
    reg_restore(DISP_REG_OVL_TRIG                    ); 
    reg_restore(DISP_REG_OVL_RST                     ); 
    reg_restore(DISP_REG_OVL_ROI_SIZE                ); 
    reg_restore(DISP_REG_OVL_DATAPATH_CON            ); 
    reg_restore(DISP_REG_OVL_ROI_BGCLR               ); 
    reg_restore(DISP_REG_OVL_SRC_CON                 ); 
    reg_restore(DISP_REG_OVL_L0_CON                  ); 
    reg_restore(DISP_REG_OVL_L0_SRCKEY               ); 
    reg_restore(DISP_REG_OVL_L0_SRC_SIZE             ); 
    reg_restore(DISP_REG_OVL_L0_OFFSET               ); 
    reg_restore(DISP_REG_OVL_L0_ADDR                 ); 
    reg_restore(DISP_REG_OVL_L0_PITCH                ); 
    reg_restore(DISP_REG_OVL_L1_CON                  ); 
    reg_restore(DISP_REG_OVL_L1_SRCKEY               ); 
    reg_restore(DISP_REG_OVL_L1_SRC_SIZE             ); 
    reg_restore(DISP_REG_OVL_L1_OFFSET               ); 
    reg_restore(DISP_REG_OVL_L1_ADDR                 ); 
    reg_restore(DISP_REG_OVL_L1_PITCH                ); 
    reg_restore(DISP_REG_OVL_L2_CON                  ); 
    reg_restore(DISP_REG_OVL_L2_SRCKEY               ); 
    reg_restore(DISP_REG_OVL_L2_SRC_SIZE             ); 
    reg_restore(DISP_REG_OVL_L2_OFFSET               ); 
    reg_restore(DISP_REG_OVL_L2_ADDR                 ); 
    reg_restore(DISP_REG_OVL_L2_PITCH                ); 
    reg_restore(DISP_REG_OVL_L3_CON                  ); 
    reg_restore(DISP_REG_OVL_L3_SRCKEY               ); 
    reg_restore(DISP_REG_OVL_L3_SRC_SIZE             ); 
    reg_restore(DISP_REG_OVL_L3_OFFSET               ); 
    reg_restore(DISP_REG_OVL_L3_ADDR                 ); 
    reg_restore(DISP_REG_OVL_L3_PITCH                ); 
    reg_restore(DISP_REG_OVL_RDMA0_CTRL              ); 
    reg_restore(DISP_REG_OVL_RDMA0_MEM_START_TRIG    ); 
    reg_restore(DISP_REG_OVL_RDMA0_MEM_GMC_SETTING   ); 
    reg_restore(DISP_REG_OVL_RDMA0_MEM_SLOW_CON      ); 
    reg_restore(DISP_REG_OVL_RDMA0_FIFO_CTRL         ); 
    reg_restore(DISP_REG_OVL_RDMA1_CTRL              ); 
    reg_restore(DISP_REG_OVL_RDMA1_MEM_START_TRIG    ); 
    reg_restore(DISP_REG_OVL_RDMA1_MEM_GMC_SETTING   ); 
    reg_restore(DISP_REG_OVL_RDMA1_MEM_SLOW_CON      ); 
    reg_restore(DISP_REG_OVL_RDMA1_FIFO_CTRL         ); 
    reg_restore(DISP_REG_OVL_RDMA2_CTRL              ); 
    reg_restore(DISP_REG_OVL_RDMA2_MEM_START_TRIG    ); 
    reg_restore(DISP_REG_OVL_RDMA2_MEM_GMC_SETTING   ); 
    reg_restore(DISP_REG_OVL_RDMA2_MEM_SLOW_CON      ); 
    reg_restore(DISP_REG_OVL_RDMA2_FIFO_CTRL         ); 
    reg_restore(DISP_REG_OVL_RDMA3_CTRL              ); 
    reg_restore(DISP_REG_OVL_RDMA3_MEM_START_TRIG    ); 
    reg_restore(DISP_REG_OVL_RDMA3_MEM_GMC_SETTING   ); 
    reg_restore(DISP_REG_OVL_RDMA3_MEM_SLOW_CON      ); 
    reg_restore(DISP_REG_OVL_RDMA3_FIFO_CTRL         ); 
    reg_restore(DISP_REG_OVL_L0_Y2R_PARA_R0          ); 
    reg_restore(DISP_REG_OVL_L0_Y2R_PARA_R1          ); 
    reg_restore(DISP_REG_OVL_L0_Y2R_PARA_G0          ); 
    reg_restore(DISP_REG_OVL_L0_Y2R_PARA_G1          ); 
    reg_restore(DISP_REG_OVL_L0_Y2R_PARA_B0          ); 
    reg_restore(DISP_REG_OVL_L0_Y2R_PARA_B1          ); 
    reg_restore(DISP_REG_OVL_L0_Y2R_PARA_YUV_A_0     ); 
    reg_restore(DISP_REG_OVL_L0_Y2R_PARA_YUV_A_1     ); 
    reg_restore(DISP_REG_OVL_L0_Y2R_PARA_RGB_A_0     ); 
    reg_restore(DISP_REG_OVL_L0_Y2R_PARA_RGB_A_1     ); 
    reg_restore(DISP_REG_OVL_L1_Y2R_PARA_R0          ); 
    reg_restore(DISP_REG_OVL_L1_Y2R_PARA_R1          ); 
    reg_restore(DISP_REG_OVL_L1_Y2R_PARA_G0          ); 
    reg_restore(DISP_REG_OVL_L1_Y2R_PARA_G1          ); 
    reg_restore(DISP_REG_OVL_L1_Y2R_PARA_B0          ); 
    reg_restore(DISP_REG_OVL_L1_Y2R_PARA_B1          ); 
    reg_restore(DISP_REG_OVL_L1_Y2R_PARA_YUV_A_0     ); 
    reg_restore(DISP_REG_OVL_L1_Y2R_PARA_YUV_A_1     ); 
    reg_restore(DISP_REG_OVL_L1_Y2R_PARA_RGB_A_0     ); 
    reg_restore(DISP_REG_OVL_L1_Y2R_PARA_RGB_A_1     ); 
    reg_restore(DISP_REG_OVL_L2_Y2R_PARA_R0          ); 
    reg_restore(DISP_REG_OVL_L2_Y2R_PARA_R1          ); 
    reg_restore(DISP_REG_OVL_L2_Y2R_PARA_G0          ); 
    reg_restore(DISP_REG_OVL_L2_Y2R_PARA_G1          ); 
    reg_restore(DISP_REG_OVL_L2_Y2R_PARA_B0          ); 
    reg_restore(DISP_REG_OVL_L2_Y2R_PARA_B1          ); 
    reg_restore(DISP_REG_OVL_L2_Y2R_PARA_YUV_A_0     ); 
    reg_restore(DISP_REG_OVL_L2_Y2R_PARA_YUV_A_1     ); 
    reg_restore(DISP_REG_OVL_L2_Y2R_PARA_RGB_A_0     ); 
    reg_restore(DISP_REG_OVL_L2_Y2R_PARA_RGB_A_1     ); 
    reg_restore(DISP_REG_OVL_L3_Y2R_PARA_R0          ); 
    reg_restore(DISP_REG_OVL_L3_Y2R_PARA_R1          ); 
    reg_restore(DISP_REG_OVL_L3_Y2R_PARA_G0          ); 
    reg_restore(DISP_REG_OVL_L3_Y2R_PARA_G1          ); 
    reg_restore(DISP_REG_OVL_L3_Y2R_PARA_B0          ); 
    reg_restore(DISP_REG_OVL_L3_Y2R_PARA_B1          ); 
    reg_restore(DISP_REG_OVL_L3_Y2R_PARA_YUV_A_0     ); 
    reg_restore(DISP_REG_OVL_L3_Y2R_PARA_YUV_A_1     ); 
    reg_restore(DISP_REG_OVL_L3_Y2R_PARA_RGB_A_0     ); 
    reg_restore(DISP_REG_OVL_L3_Y2R_PARA_RGB_A_1     ); 
    reg_restore(DISP_REG_OVL_DEBUG_MON_SEL           ); 
    reg_restore(DISP_REG_OVL_RDMA0_MEM_GMC_SETTING2  ); 
    reg_restore(DISP_REG_OVL_RDMA1_MEM_GMC_SETTING2  ); 
    reg_restore(DISP_REG_OVL_RDMA2_MEM_GMC_SETTING2  ); 
    reg_restore(DISP_REG_OVL_RDMA3_MEM_GMC_SETTING2  ); 
    reg_restore(DISP_REG_OVL_FLOW_CTRL_DBG           ); 
    reg_restore(DISP_REG_OVL_ADDCON_DBG              ); 
    reg_restore(DISP_REG_OVL_OUTMUX_DBG              ); 
    //            
    for (index = 0; index < 2; index++) {
    	reg_restore(DISP_REG_RDMA_INT_ENABLE        + index*DISP_INDEX_OFFSET);
    	reg_restore(DISP_REG_RDMA_INT_STATUS        + index*DISP_INDEX_OFFSET);
    	reg_restore(DISP_REG_RDMA_GLOBAL_CON        + index*DISP_INDEX_OFFSET);
    	reg_restore(DISP_REG_RDMA_SIZE_CON_0        + index*DISP_INDEX_OFFSET);
    	reg_restore(DISP_REG_RDMA_SIZE_CON_1        + index*DISP_INDEX_OFFSET);
    	reg_restore(DISP_REG_RDMA_TARGET_LINE       + index*DISP_INDEX_OFFSET);
    	reg_restore(DISP_REG_RDMA_MEM_CON           + index*DISP_INDEX_OFFSET);
    	reg_restore(DISP_REG_RDMA_MEM_START_ADDR    + index*DISP_INDEX_OFFSET);
    	reg_restore(DISP_REG_RDMA_MEM_SRC_PITCH     + index*DISP_INDEX_OFFSET);
    	reg_restore(DISP_REG_RDMA_MEM_GMC_SETTING_0 + index*DISP_INDEX_OFFSET);
    	reg_restore(DISP_REG_RDMA_MEM_SLOW_CON      + index*DISP_INDEX_OFFSET);
    	reg_restore(DISP_REG_RDMA_MEM_GMC_SETTING_1 + index*DISP_INDEX_OFFSET);
    	reg_restore(DISP_REG_RDMA_FIFO_CON          + index*DISP_INDEX_OFFSET);
    	reg_restore(DISP_REG_RDMA_FIFO_LOG          + index*DISP_INDEX_OFFSET);
    	reg_restore(DISP_REG_RDMA_CF_00             + index*DISP_INDEX_OFFSET);
    	reg_restore(DISP_REG_RDMA_CF_01             + index*DISP_INDEX_OFFSET);
    	reg_restore(DISP_REG_RDMA_CF_02             + index*DISP_INDEX_OFFSET);
    	reg_restore(DISP_REG_RDMA_CF_10             + index*DISP_INDEX_OFFSET);
    	reg_restore(DISP_REG_RDMA_CF_11             + index*DISP_INDEX_OFFSET);
    	reg_restore(DISP_REG_RDMA_CF_12             + index*DISP_INDEX_OFFSET);
    	reg_restore(DISP_REG_RDMA_CF_20             + index*DISP_INDEX_OFFSET);
    	reg_restore(DISP_REG_RDMA_CF_21             + index*DISP_INDEX_OFFSET);
    	reg_restore(DISP_REG_RDMA_CF_22             + index*DISP_INDEX_OFFSET);
    	reg_restore(DISP_REG_RDMA_CF_PRE_ADD0       + index*DISP_INDEX_OFFSET);
    	reg_restore(DISP_REG_RDMA_CF_PRE_ADD1       + index*DISP_INDEX_OFFSET);
    	reg_restore(DISP_REG_RDMA_CF_PRE_ADD2       + index*DISP_INDEX_OFFSET);
    	reg_restore(DISP_REG_RDMA_CF_POST_ADD0      + index*DISP_INDEX_OFFSET);
    	reg_restore(DISP_REG_RDMA_CF_POST_ADD1      + index*DISP_INDEX_OFFSET);
    	reg_restore(DISP_REG_RDMA_CF_POST_ADD2      + index*DISP_INDEX_OFFSET);
    	reg_restore(DISP_REG_RDMA_DUMMY             + index*DISP_INDEX_OFFSET);
    	reg_restore(DISP_REG_RDMA_DEBUG_OUT_SEL     + index*DISP_INDEX_OFFSET);
    }
    //     
    reg_restore(DISP_REG_WDMA_INTEN          );             
    reg_restore(DISP_REG_WDMA_INTSTA         );     
    reg_restore(DISP_REG_WDMA_EN             );     
    reg_restore(DISP_REG_WDMA_RST            );     
    reg_restore(DISP_REG_WDMA_SMI_CON        );     
    reg_restore(DISP_REG_WDMA_CFG            );     
    reg_restore(DISP_REG_WDMA_SRC_SIZE       );     
    reg_restore(DISP_REG_WDMA_CLIP_SIZE      );     
    reg_restore(DISP_REG_WDMA_CLIP_COORD     );     
    reg_restore(DISP_REG_WDMA_DST_ADDR       );     
    reg_restore(DISP_REG_WDMA_DST_W_IN_BYTE  );     
    reg_restore(DISP_REG_WDMA_ALPHA          );     
    reg_restore(DISP_REG_WDMA_BUF_ADDR       );     
    reg_restore(DISP_REG_WDMA_STA            );     
    reg_restore(DISP_REG_WDMA_BUF_CON1       );     
    reg_restore(DISP_REG_WDMA_BUF_CON2       );     
    reg_restore(DISP_REG_WDMA_C00            );     
    reg_restore(DISP_REG_WDMA_C02            );     
    reg_restore(DISP_REG_WDMA_C10            );     
    reg_restore(DISP_REG_WDMA_C12            );     
    reg_restore(DISP_REG_WDMA_C20            );     
    reg_restore(DISP_REG_WDMA_C22            );     
    reg_restore(DISP_REG_WDMA_PRE_ADD0       );     
    reg_restore(DISP_REG_WDMA_PRE_ADD2       );     
    reg_restore(DISP_REG_WDMA_POST_ADD0      );     
    reg_restore(DISP_REG_WDMA_POST_ADD2      );     
    reg_restore(DISP_REG_WDMA_DST_U_ADDR     );     
    reg_restore(DISP_REG_WDMA_DST_V_ADDR     );     
    reg_restore(DISP_REG_WDMA_DST_UV_PITCH   );     
    reg_restore(DISP_REG_WDMA_DITHER_CON     );     
    reg_restore(DISP_REG_WDMA_FLOW_CTRL_DBG  );     
    reg_restore(DISP_REG_WDMA_EXEC_DBG       );     
    reg_restore(DISP_REG_WDMA_CLIP_DBG       );     

    //                                                
    //                          
    DISP_MSG("disp_reg_restore() done \n");
    
    disp_bls_init(fb_width, fb_height);

    DpEngine_COLORonInit();
    DpEngine_COLORonConfig(fb_width,fb_height);

    return 0;        
}

unsigned int disp_intr_status[DISP_MODULE_MAX] = {0};
int disp_intr_restore(void)
{
    //                         
    //                                                                                     
    //                                                                                    
    DISP_REG_SET(DISP_REG_OVL_INTEN,              disp_intr_status[DISP_MODULE_OVL]  ); 
    //                                                                                    
    DISP_REG_SET(DISP_REG_WDMA_INTEN,      disp_intr_status[DISP_MODULE_WDMA0]); 
    DISP_REG_SET(DISP_REG_RDMA_INT_ENABLE,        disp_intr_status[DISP_MODULE_RDMA]);
    //                                                                                                     
    DISP_REG_SET(DISP_REG_CONFIG_MUTEX_INTEN,     disp_intr_status[DISP_MODULE_MUTEX]); 

    return 0;
}

//                                                                      
int disp_intr_disable_and_clear(void)
{
    //                       
    //                                                                                
    //                                                                     
    disp_intr_status[DISP_MODULE_OVL] = DISP_REG_GET(DISP_REG_OVL_INTEN);
    disp_intr_status[DISP_MODULE_WDMA0] = DISP_REG_GET(DISP_REG_WDMA_INTEN);
    //                                                                               
    //                                                                             
    disp_intr_status[DISP_MODULE_RDMA] = DISP_REG_GET(DISP_REG_RDMA_INT_ENABLE);
    //                                                                                               
    disp_intr_status[DISP_MODULE_MUTEX] = DISP_REG_GET(DISP_REG_CONFIG_MUTEX_INTEN);
    
    //             
    //                                               
    //                                    
    DISP_REG_SET(DISP_REG_OVL_INTEN, 0);
    //                                     
    DISP_REG_SET(DISP_REG_WDMA_INTEN+0x1000, 0);
    DISP_REG_SET(DISP_REG_RDMA_INT_ENABLE, 0);
    DISP_REG_SET(DISP_REG_RDMA_INT_ENABLE+DISP_INDEX_OFFSET, 0);
    DISP_REG_SET(DISP_REG_CONFIG_MUTEX_INTEN, 0);
    
    //                  
    //                                                 
    //                                                    
    DISP_REG_SET(DISP_REG_OVL_INTSTA, 0);    
    //                                          
    DISP_REG_SET(DISP_REG_WDMA_INTSTA+0x1000, 0);    
    DISP_REG_SET(DISP_REG_RDMA_INT_STATUS, 0);    
    DISP_REG_SET(DISP_REG_RDMA_INT_STATUS+DISP_INDEX_OFFSET, 0);
    DISP_REG_SET(DISP_REG_CONFIG_MUTEX_INTSTA, 0);

    return 0;	  
}

int disp_path_clock_on(char* name)
{
    if(name != NULL)
    {
        DISP_MSG("disp_path_power_on, caller:%s \n", name);
    }

    enable_clock(MT_CG_DISP0_SMI_COMMON   , "DDP");
    enable_clock(MT_CG_DISP0_SMI_LARB0   , "DDP");
    //                                          
    enable_clock(MT_CG_DISP0_MUTEX_32K   , "DDP");
    //                                          
//                                                    
    
//                                                      
//                                                  
//                                                  
//                                               
//                                                    

    enable_clock(MT_CG_DISP0_DISP_OVL  , "DDP");
    //                                              
    enable_clock(MT_CG_DISP0_DISP_COLOR       , "DDP");
//                                                  
    enable_clock(MT_CG_DISP0_DISP_BLS         , "DDP");
    enable_clock(MT_CG_DISP0_DISP_WDMA, "DDP");
    //                                              
    //                                              
    //                                              
    //                                              
    
    enable_clock(MT_CG_DISP0_DISP_RDMA, "DDP");
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
    enable_clock(MT_CG_DISP0_MDP_BLS_26M         , "DDP");
//      

    //                              
    if (strncmp(name, "mtkfb", 10) == 0)
    {
    	if(*pRegBackup != DDP_UNBACKED_REG_MEM)
    	{
    		disp_reg_restore();

    		//                              
    		disp_intr_restore();
    	}
    	else
    	{
    		DISP_MSG("disp_path_clock_on(), dose not call disp_reg_restore, cause mem not inited! \n");
    	}
    }
    else
    {
        //                
        disp_aal_reset();
    }
    
    DISP_MSG("DISP CG:%x\n", DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON0));
    return 0;
}

int disp_hdmi_path_clock_on(char* name)
{
    if(name != NULL)
    {
        DISP_MSG("disp_hdmi_path_clock_on, caller:%s \n", name);
    }

    enable_clock(MT_CG_DISP0_DISP_OVL  , "DDP");
    enable_clock(MT_CG_DISP0_DISP_WDMA, "DDP");

    //                              
    if (strncmp(name, "ipoh_mtkfb", 10))
    {
    	if(*pRegBackup != DDP_UNBACKED_REG_MEM)
    	{
    		disp_reg_restore();

    		//                              
    		disp_intr_restore();
    	}
    	else
    	{
    		DISP_MSG("disp_hdmi_path_clock_on(), dose not call disp_reg_restore, cause mem not inited! \n");
    	}
    }
    else
    {
        //                
        disp_aal_reset();
    }
    
    DISP_MSG("DISP CG:%x\n", DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON0));
    return 0;
}

int disp_hdmi_path_clock_off(char* name)
{
    if(name != NULL)
    {
        DISP_MSG("disp_hdmi_path_clock_off, caller:%s \n", name);
    }
    
    //                                   
    //                       
    disp_intr_status[DISP_MODULE_OVL] = DISP_REG_GET(DISP_REG_OVL_INTEN);
    disp_intr_status[DISP_MODULE_WDMA0] = DISP_REG_GET(DISP_REG_WDMA_INTEN);
    disp_intr_status[DISP_MODULE_RDMA] = DISP_REG_GET(DISP_REG_RDMA_INT_ENABLE);
    disp_intr_status[DISP_MODULE_MUTEX] = DISP_REG_GET(DISP_REG_CONFIG_MUTEX_INTEN);
    
    //             
    DISP_REG_SET(DISP_REG_OVL_INTEN, 0);
    DISP_REG_SET(DISP_REG_WDMA_INTEN+0x1000, 0);
    
    //                             
    DISP_REG_SET(DISP_REG_OVL_INTSTA, 0);    
    DISP_REG_SET(DISP_REG_WDMA_INTSTA+0x1000, 0);    
    
    //                             
    disp_reg_backup();

    //                                                      
    RDMAStop(0);
    RDMAReset(0);
	
    WDMAStop(0);
    WDMAReset(0);

    OVLStop();
    OVLReset();

    disable_clock(MT_CG_DISP0_DISP_OVL  , "DDP");
    disable_clock(MT_CG_DISP0_DISP_WDMA, "DDP");
  
    return 0;
}

int disp_path_clock_off(char* name)
{
    if(name != NULL)
    {
        DISP_MSG("disp_path_power_off, caller:%s \n", name);
    }
    if (strncmp(name, "mtkfb", 10) == 0)
    {
    	//                                   
    	disp_intr_disable_and_clear();

    	//                             
    	disp_reg_backup();
    }
//                                                                                                  
//                                                     
//                             
    disable_clock(MT_CG_DISP0_MDP_BLS_26M         , "DDP");
//      

    //                                           
//                                                     
    
//                                                       
//                                                   
//                                                   
//                                                
//                                                     

    //                                                      
    RDMAStop(0);
    RDMAReset(0);

    WDMAStop(0);
    WDMAReset(0);

    OVLStop();
    OVLReset();

    disable_clock(MT_CG_DISP0_DISP_OVL  , "DDP");
    //                                               
    disable_clock(MT_CG_DISP0_DISP_COLOR       , "DDP");
//                                                   
    disable_clock(MT_CG_DISP0_DISP_BLS         , "DDP");
    disable_clock(MT_CG_DISP0_DISP_WDMA, "DDP");
    //                                               
    //                                               
    //                                               
    //                                               
    
    disable_clock(MT_CG_DISP0_DISP_RDMA, "DDP");
    //                                         
    //                                               
    //                                               
    //                                               
    //                                                   

    
    if(0) //                                   
    {
        DISP_MSG("warning: do not power off MT_CG_DISP0_SMI_LARB0 for DPI resume issue\n");
    }
    else
    {
        //                                           
        disable_clock(MT_CG_DISP0_MUTEX_32K   , "DDP");
        disable_clock(MT_CG_DISP0_SMI_LARB0   , "DDP");
        disable_clock(MT_CG_DISP0_SMI_COMMON   , "DDP");
    } 
    //                                               
    //                                               

    //                                                                      
    return 0;
}
#else

static int disp_path_clock_on(char* name)
{
    return 0;
}
static int disp_path_clock_off(char* name)
{
    return 0;
}

#endif

int disp_bls_set_max_backlight(unsigned int level)
{
  return disp_bls_set_max_backlight_(level);
}

int disp_bls_off_on_directly(bool enable)
{
	disp_bls_contrl_directly(enable);
}

int disp_path_clear_config(int mutexId)
{
    DISP_REG_SET(DISP_REG_CONFIG_MUTEX_MOD(mutexId), 0);
    DISP_REG_SET(DISP_REG_CONFIG_MUTEX_SOF(mutexId), 0);
    return 0;
}


