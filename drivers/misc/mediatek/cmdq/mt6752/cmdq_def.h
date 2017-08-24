#ifndef __CMDQ_DEF_H__
#define __CMDQ_DEF_H__

#ifdef CONFIG_OF
#define CMDQ_OF_SUPPORT	/*                            */
#else
#undef  CMDQ_OF_SUPPORT
#endif

#if defined(CONFIG_TRUSTONIC_TEE_SUPPORT) && defined(CONFIG_MTK_SEC_VIDEO_PATH_SUPPORT)
#define CMDQ_SECURE_PATH_SUPPORT
#endif

/*                 */
#define CMDQ_GPR_SUPPORT

/*                                     */
#ifdef CMDQ_PROFILE_MARKER_SUPPORT
	#define CMDQ_MAX_PROFILE_MARKER_IN_TASK (5)
#endif

#define CMDQ_DUMP_GIC (0)	/*                                   */
#define CMDQ_PROFILE_MMP (0)


#include <linux/kernel.h>

#define CMDQ_INIT_FREE_TASK_COUNT       (8)
#define CMDQ_MAX_THREAD_COUNT           (16)
#define CMDQ_MAX_HIGH_PRIORITY_THREAD_COUNT (6)	/*                                                 */
#define CMDQ_MIN_SECURE_THREAD_ID 		(12)
#define CMDQ_MAX_SECURE_THREAD_COUNT 	(3)
#define CMDQ_MAX_RECORD_COUNT           (1024)
#define CMDQ_MAX_ERROR_COUNT            (2)
#define CMDQ_MAX_RETRY_COUNT            (1)
#define CMDQ_MAX_TASK_IN_THREAD         (16)
#define CMDQ_MAX_READ_SLOT_COUNT        (4)

#define CMDQ_MAX_PREFETCH_INSTUCTION    (240)	/*                                                */
#define CMDQ_INITIAL_CMD_BLOCK_SIZE     (PAGE_SIZE)
#define CMDQ_EMERGENCY_BLOCK_SIZE       (256 * 1024)	/*                       */
#define CMDQ_EMERGENCY_BLOCK_COUNT      (4)
#define CMDQ_INST_SIZE                  (2 * sizeof(uint32_t))	/*                       */

#define CMDQ_MAX_LOOP_COUNT             (1000000)
#define CMDQ_MAX_INST_CYCLE             (27)
#define CMDQ_MIN_AGE_VALUE              (5)
#define CMDQ_MAX_ERROR_SIZE             (8 * 1024)

#define CMDQ_MAX_COOKIE_VALUE           (0xFFFFFFFF)	/*                                                          */
#define CMDQ_ARG_A_SUBSYS_MASK          (0x001F0000)

#ifdef CONFIG_MTK_FPGA
#define CMDQ_DEFAULT_TIMEOUT_MS         (10000)
#else
#define CMDQ_DEFAULT_TIMEOUT_MS         (1000)
#endif

#define CMDQ_ACQUIRE_THREAD_TIMEOUT_MS  (2000)
#define CMDQ_PREDUMP_TIMEOUT_MS         (200)
#define CMDQ_PREDUMP_RETRY_COUNT        (5)

#define CMDQ_INVALID_THREAD             (-1)

#define CMDQ_DRIVER_DEVICE_NAME         "mtk_cmdq"

#ifndef CONFIG_MTK_FPGA
#define CMDQ_PWR_AWARE		/*                           */
#else
#undef CMDQ_PWR_AWARE
#endif

typedef enum CMDQ_SCENARIO_ENUM {
	CMDQ_SCENARIO_JPEG_DEC = 0,
	CMDQ_SCENARIO_PRIMARY_DISP = 1,
	CMDQ_SCENARIO_PRIMARY_MEMOUT = 2,
	CMDQ_SCENARIO_PRIMARY_ALL = 3,
	CMDQ_SCENARIO_SUB_DISP = 4,
	CMDQ_SCENARIO_SUB_MEMOUT = 5,
	CMDQ_SCENARIO_SUB_ALL = 6,
	CMDQ_SCENARIO_MHL_DISP = 7,
	CMDQ_SCENARIO_RDMA0_DISP = 8,
	CMDQ_SCENARIO_RDMA0_COLOR0_DISP = 9,
	CMDQ_SCENARIO_RDMA1_DISP = 10,

	CMDQ_SCENARIO_TRIGGER_LOOP = 11,	/*                                           */

	CMDQ_SCENARIO_USER_MDP = 12,	/*                                                             */

	CMDQ_SCENARIO_DEBUG = 13,
	CMDQ_SCENARIO_DEBUG_PREFETCH = 14,

	CMDQ_SCENARIO_DISP_ESD_CHECK = 15,	/*           */
	CMDQ_SCENARIO_DISP_SCREEN_CAPTURE = 16,	/*                                                                         */

	CMDQ_SCENARIO_SECURE_NOTIFY_LOOP = 17, /*                                                      */

	CMDQ_SCENARIO_DISP_PRIMARY_DISABLE_SECURE_PATH = 18,
	CMDQ_SCENARIO_DISP_SUB_DISABLE_SECURE_PATH = 19,

	CMDQ_SCENARIO_DISP_COLOR = 20,	/*                                */
	CMDQ_SCENARIO_USER_DISP_COLOR = 21,	/*                                    */

	CMDQ_SCENARIO_USER_SPACE = 22,	/*                                                                         */

	CMDQ_SCENARIO_DISP_MIRROR_MODE = 23,

	CMDQ_MAX_SCENARIO_COUNT	/*                        */
} CMDQ_SCENARIO_ENUM;

typedef enum CMDQ_HW_THREAD_PRIORITY_ENUM {
	CMDQ_THR_PRIO_NORMAL = 0,	/*                          */
	CMDQ_THR_PRIO_DISPLAY_TRIGGER = 1,	/*                                      */

	CMDQ_THR_PRIO_DISPLAY_ESD = 3,	/*                                  */
	CMDQ_THR_PRIO_DISPLAY_CONFIG = 3,	/*                              */

	CMDQ_THR_PRIO_MAX = 7,	/*                           */
} CMDQ_HW_THREAD_PRIORITY_ENUM;

typedef enum CMDQ_DATA_REGISTER_ENUM {
	/*                          */
	/*                            */
	/*                                                       */
	/*                        */

	CMDQ_DATA_REG_JPEG = 0x00,	/*    */
	CMDQ_DATA_REG_JPEG_DST = 0x11,	/*    */

	CMDQ_DATA_REG_PQ_COLOR = 0x04,	/*    */
	CMDQ_DATA_REG_PQ_COLOR_DST = 0x13,	/*    */

	CMDQ_DATA_REG_2D_SHARPNESS_0 = 0x05,	/*    */
	CMDQ_DATA_REG_2D_SHARPNESS_0_DST = 0x14,	/*    */

	CMDQ_DATA_REG_2D_SHARPNESS_1 = 0x0a,	/*     */
	CMDQ_DATA_REG_2D_SHARPNESS_1_DST = 0x16,	/*    */

	CMDQ_DATA_REG_DEBUG = 0x0b,	/*     */
	CMDQ_DATA_REG_DEBUG_DST = 0x17,	/*    */

	/*                                        */
	CMDQ_DATA_REG_INVALID = -1,
} CMDQ_DATA_REGISTER_ENUM;

/*             */
#undef DECLARE_CMDQ_EVENT
#define DECLARE_CMDQ_EVENT(name, val) name = val,
typedef enum CMDQ_EVENT_ENUM {
#include "cmdq_event.h"
} CMDQ_EVENT_ENUM;
#undef DECLARE_CMDQ_EVENT

typedef enum CMDQ_ENG_ENUM {
	/*     */
	CMDQ_ENG_ISP_IMGI = 0,
	CMDQ_ENG_ISP_IMGO,	/*   */
	CMDQ_ENG_ISP_IMG2O,	/*   */

	/*     */
	CMDQ_ENG_MDP_CAMIN,	/*   */
	CMDQ_ENG_MDP_RDMA0,	/*   */
	CMDQ_ENG_MDP_RSZ0,	/*   */
	CMDQ_ENG_MDP_RSZ1,	/*   */
	CMDQ_ENG_MDP_TDSHP0,	/*   */
	CMDQ_ENG_MDP_WROT0,	/*   */
	CMDQ_ENG_MDP_WDMA,	/*   */

	/*             */
	CMDQ_ENG_JPEG_ENC,	/*    */
	CMDQ_ENG_VIDEO_ENC,	/*    */
	CMDQ_ENG_JPEG_DEC,	/*    */
	CMDQ_ENG_JPEG_REMDC,	/*    */

	/*      */
	CMDQ_ENG_DISP_UFOE,	/*    */
	CMDQ_ENG_DISP_AAL,	/*    */
	CMDQ_ENG_DISP_COLOR0,	/*    */
	CMDQ_ENG_DISP_RDMA0,	/*    */
	CMDQ_ENG_DISP_RDMA1,	/*    */
	CMDQ_ENG_DISP_WDMA0,	/*    */
	CMDQ_ENG_DISP_WDMA1,	/*    */
	CMDQ_ENG_DISP_OVL0,	/*    */
	CMDQ_ENG_DISP_OVL1,	/*    */
	CMDQ_ENG_DISP_GAMMA,	/*    */
	CMDQ_ENG_DISP_DSI0_VDO,	/*    */
	CMDQ_ENG_DISP_DSI0_CMD,	/*    */
	CMDQ_ENG_DISP_DSI0,	/*    */
	CMDQ_ENG_DISP_DPI,	/*    */

	/*                           */
	CMDQ_ENG_CMDQ,
	CMDQ_ENG_DISP_MUTEX,
	CMDQ_ENG_MMSYS_CONFIG,

	CMDQ_MAX_ENGINE_COUNT	/*                        */
} CMDQ_ENG_ENUM;

typedef unsigned long long cmdqJobHandle_t;   //                                                                 
typedef unsigned long long cmdqU32Ptr_t;      //                                                                                 
#define CMDQ_U32_PTR(x) ((uint32_t*)(unsigned long)x)

typedef struct cmdqReadRegStruct {
	uint32_t count;		/*                                   */
	cmdqU32Ptr_t regAddresses;	/*                                                  */
} cmdqReadRegStruct;

typedef struct cmdqRegValueStruct {
	/*                             */
	uint32_t count;

	/*                                             */
	/*                                        */
	cmdqU32Ptr_t regValues;
} cmdqRegValueStruct;

typedef struct cmdqReadAddressStruct {
	uint32_t count;		/*                                   */

	/*                                           */
	/*                                                                    */
	/*  */
	/*                                                                          */
	/*                                                                              */
	/*                                                            */
	cmdqU32Ptr_t dmaAddresses;

	cmdqU32Ptr_t values;	/*                                                    */
} cmdqReadAddressStruct;

/*
                           
                                                                               
                                                                  
                                                                        
                                                   
                                                   
                                                                                
 */
typedef enum CMDQ_SEC_ADDR_METADATA_TYPE {
	CMDQ_SAM_H_2_PA = 0, /*                      */
	CMDQ_SAM_H_2_MVA = 1, /*                       */
	CMDQ_SAM_NMVA_2_MVA = 2, /*                                */
} CMDQ_SEC_ADDR_METADATA_TYPE;

typedef struct cmdqSecAddrMetadataStruct
{
	/*                                                                                    */
	uint32_t instrIndex;

	CMDQ_SEC_ADDR_METADATA_TYPE type; /*                      */
	uint32_t baseHandle; /*                               */
	uint32_t offset;     /*                                         */
	uint32_t size;       /*             */
	uint32_t port;       /*                              */
} cmdqSecAddrMetadataStruct;

typedef struct cmdqSecDataStruct {
	bool isSecure; /*                             */

	/*                                                                                          */
	uint32_t addrMetadataCount; /*                                   */
	cmdqU32Ptr_t addrMetadatas; /*                                         */
	uint32_t addrMetadataMaxCount; /*          */

	uint64_t enginesNeedDAPC;
	uint64_t enginesNeedPortSecurity;

	/*                                                                  */
	int32_t waitCookie; /*                                                          */
	bool resetExecCnt; /*                        */
} cmdqSecDataStruct;

#ifdef CMDQ_PROFILE_MARKER_SUPPORT
	typedef struct cmdqProfileMarkerStruct {
		uint32_t count;
		long long hSlot; /*                                                                  */
		const char *tag[CMDQ_MAX_PROFILE_MARKER_IN_TASK];
	} cmdqProfileMarkerStruct;
#endif

typedef struct cmdqCommandStruct {
	uint32_t scenario;	/*                                             */
	uint32_t priority;	/*                                                             */
	uint64_t engineFlag;	/*                                */
	cmdqU32Ptr_t pVABase;	/*                                                                                                                      */
	uint32_t blockSize;	/*                                            */
	cmdqReadRegStruct regRequest;	/*                                                            */
	cmdqRegValueStruct regValue;	/*                                     */
	cmdqReadAddressStruct readAddress;	/*                                           */

	cmdqSecDataStruct secData; /*                           */

	uint32_t debugRegDump;	/*                                                     */
	cmdqU32Ptr_t privateData;	/*                                                                                                          */
#ifdef CMDQ_PROFILE_MARKER_SUPPORT
	cmdqProfileMarkerStruct profileMarker;
#endif
} cmdqCommandStruct;

typedef enum CMDQ_CAP_BITS {
	CMDQ_CAP_WFE = 0,	/*                                                                                    */
} CMDQ_CAP_BITS;


/* 
                                             
 */

typedef struct {
	/*       */
	bool throwAEE;
	bool hasReset;
	int32_t irqFlag;
	uint32_t errInstr[2];
	uint32_t pc;
} cmdqSecCancelTaskResultStruct;

#endif				/*                */
