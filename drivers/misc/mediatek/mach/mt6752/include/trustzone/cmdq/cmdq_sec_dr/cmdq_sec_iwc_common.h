#ifndef __CMDQ_SEC_IWC_COMMON_H__
#define __CMDQ_SEC_IWC_COMMON_H__

/*             */
#define CMDQ_SEC_SHARED_IRQ_RAISED_OFFSET    (0x0) /*                                    */
#define CMDQ_SEC_SHARED_THR_CNT_OFFSET (0x100)

/*                            */
#define CMDQ_TZ_CMD_BLOCK_SIZE	 (32 * 1024)

#define CMDQ_IWC_MAX_CMD_LENGTH (32 * 1024 / 4)

#define CMDQ_IWC_MAX_ADDR_LIST_LENGTH (12)

#define CMDQ_IWC_CLIENT_NAME (16)

typedef enum CMDQ_IWC_ADDR_METADATA_TYPE {
	CMDQ_IWC_H_2_PA = 0, /*                      */
	CMDQ_IWC_H_2_MVA = 1, /*                       */
	CMDQ_IWC_NMVA_2_MVA = 2, /*                                */
} CMDQ_IWC_ADDR_METADATA_TYPE;

/*  */
/*             */
/*  */
typedef struct{
	/*                                                                                    */
	uint32_t instrIndex;

	uint32_t type; /*                      */
	uint32_t baseHandle; /*                               */
	uint32_t offset;     /*                                         */
	uint32_t size;       /*             */
	uint32_t port;       /*                              */
} iwcCmdqAddrMetadata_t;

typedef struct {
	int32_t logLevel;
	int32_t enableProfile;
} iwcCmdqDebugConfig_t;

typedef struct {
	uint64_t startTime;	/*                 */
	uint64_t endTime;	/*               */
} iwcCmdqSystraceLog_t;

typedef struct {
	uint32_t addrListLength;
	iwcCmdqAddrMetadata_t addrList[CMDQ_IWC_MAX_ADDR_LIST_LENGTH];

	uint64_t enginesNeedDAPC;
	uint64_t enginesNeedPortSecurity;
} iwcCmdqMetadata_t;

typedef struct {
	uint32_t addr; /*                                                 */
	uint32_t size;
} iwcCmdqSectraceBuffer_t;

typedef struct {
	long long shareMemoyPA; /*                                             */
	uint32_t size;
} iwcCmdqPathResource_t;

typedef struct {
	/*      */
	int32_t thread;
	uint32_t waitCookie;

	/*       */
	bool throwAEE;
	bool hasReset;
	int32_t irqStatus; /*                        */
	int32_t irqFlag; /*                 */
	uint32_t errInstr[2]; /*                                          */
	uint32_t pc;
} iwcCmdqCancelTask_t;

typedef struct {
	/*                      */
	uint32_t thread;
	uint32_t scenario;
	uint32_t priority;
	uint32_t commandSize;
	uint64_t engineFlag;
	uint32_t pVABase[CMDQ_IWC_MAX_CMD_LENGTH];

	/*                 */
	uint32_t waitCookie; /*                                                                 */
	bool resetExecCnt;   /*                      */

	/*             */
	int32_t callerPid;
	char callerName[CMDQ_IWC_CLIENT_NAME];

	/*          */
	iwcCmdqMetadata_t metadata;

	/*       */
	uint64_t hNormalTask; /*                                         */
} iwcCmdqCommand_t;

/*  */
/*                                                     */
/*                                                                      */
/*                                                                */
/*  */
/*                                                                                          */
/*                                                                                               */
/*                                                                        */
/*                                                                             */
/*  */
typedef struct {
	union {
		uint32_t cmd;	/*                 */
		int32_t rsp;	/*                                    */
	};

	union {
		iwcCmdqCommand_t command;
		iwcCmdqCancelTask_t cancelTask;
		iwcCmdqPathResource_t pathResource;
		iwcCmdqSectraceBuffer_t sectracBuffer;
	};

	iwcCmdqDebugConfig_t debug;
} iwcCmdqMessage_t, *iwcCmdqMessage_ptr;

/*  */
/*                           */
/*                                                             */
/*  */
#define	CMDQ_ERR_NOMEM		(12)	/*               */
#define	CMDQ_ERR_FAULT		(14)	/*             */

#define CMDQ_ERR_ADDR_CONVERT_HANDLE_2_PA (1000)
#define CMDQ_ERR_ADDR_CONVERT_ALLOC_MVA   (1100)
#define CMDQ_ERR_ADDR_CONVERT_FREE_MVA	  (1200)
#define CMDQ_ERR_PORT_CONFIG			  (1300)

/*             */
#define CMDQ_ERR_UNKNOWN_ADDR_METADATA_TYPE (1400)
#define CMDQ_ERR_TOO_MANY_SEC_HANDLE (1401)
/*                */
#define CMDQ_ERR_SECURITY_INVALID_INSTR	  (1500)
#define CMDQ_ERR_SECURITY_INVALID_SEC_HANDLE (1501)
#define CMDQ_ERR_SECURITY_INVALID_DAPC_FALG (1502)
#define CMDQ_ERR_INSERT_DAPC_INSTR_FAILED (1503)
#define CMDQ_ERR_INSERT_PORT_SECURITY_INSTR_FAILED (1504)

#define CMDQ_TL_ERR_UNKNOWN_IWC_CMD	   (5000)

#define CMDQ_ERR_DR_IPC_EXECUTE_SESSION   (5001)
#define CMDQ_ERR_DR_IPC_CLOSE_SESSION	 (5002)
#define CMDQ_ERR_DR_EXEC_FAILED		   (5003)

#endif				/*                      */
