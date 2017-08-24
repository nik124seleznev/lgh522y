#ifndef __TRUSTZONE_TZ_CMDQ__
#define __TRUSTZONE_TZ_CMDQ__

#include "trustzone.h"

#ifdef __KERNEL__
#else
#include "types.h" 
#endif

#define TZ_TA_CMDQ_NAME "CMDQ_TA"
#define TZ_TA_CMDQ_UUID "tz_ta_cmdq_uuid_mt8127_svp"

/*                            */
/*                                                           
 */

#define CMDQ_MAX_BLOCK_SIZE     (32 * 1024)

#define CMDQ_IWC_MAX_CMD_LENGTH (32 * 1024 / 4)

#define CMDQ_IWC_MAX_ADDR_LIST_LENGTH (12)
#define CMDQ_IWC_MAX_PORT_LIST_LENGTH (5) 


typedef struct 
{
    uint32_t instrIndex; //                               
    uint32_t baseHandle; //                  
    uint32_t offset;     //                                    

    //           
    bool     isMVA; 
    uint32_t size; 
    uint32_t port; 
} iwcCmdqAddrMetadata_t;


typedef struct 
{
    uint32_t port;
    bool     useMVA; 
} iwcCmdqPortMetadata_t;


typedef struct {
    uint32_t logLevel;
} iwcCmdqDebugConfig_t; 


typedef struct {
    uint32_t addrListLength; 
    uint32_t portListLength; 
    iwcCmdqAddrMetadata_t addrList[CMDQ_IWC_MAX_ADDR_LIST_LENGTH]; 
    iwcCmdqPortMetadata_t portList[CMDQ_IWC_MAX_PORT_LIST_LENGTH];
    iwcCmdqDebugConfig_t debug;
}iwcCmdqMetadata_t; 

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
typedef struct {
    union
    {
        uint32_t cmd;   //                
        int32_t rsp;    //                                   
    };
    
    uint32_t thread; 
    uint32_t scenario; 
    uint32_t priority; 
    uint32_t engineFlag;
    uint32_t pVABase[CMDQ_IWC_MAX_CMD_LENGTH]; 
    uint32_t blockSize; 

    iwcCmdqMetadata_t metadata;
} iwcCmdqMessage_t, *iwcCmdqMessage_ptr;


typedef struct {
    uint32_t    commandId;
    uint32_t    phy_addr;
    uint32_t    size;
    uint32_t    alignment;
    uint32_t    refcount;
    uint32_t    handle;
} tlApimem_t, *tlApimem_ptr;

#define QUERY_MAX_LEN 8



//                    
//                                                 
typedef enum CMDQ_ENG_SEC_ENUM
{
    //    
    CMDQ_ENG_SEC_ISP_IMGI   = 0,
    CMDQ_ENG_SEC_ISP_IMGO   = 1,
    CMDQ_ENG_SEC_ISP_IMG2O  = 2,

    //    
    CMDQ_ENG_SEC_MDP_RDMA0  = 3,
    CMDQ_ENG_SEC_MDP_CAMIN  = 4,
    CMDQ_ENG_SEC_MDP_SCL0   = 5,
    CMDQ_ENG_SEC_MDP_SCL1   = 6,
    CMDQ_ENG_SEC_MDP_TDSHP  = 7,
    CMDQ_ENG_SEC_MDP_WROT   = 8,
    CMDQ_ENG_SEC_MDP_WDMA1  = 9,

    CMDQ_ENG_SEC_LSCI   = 10,
    CMDQ_ENG_SEC_CMDQ   = 11,

    CMDQ_ENG_SEC_ISP_Total  = 12,
}CMDQ_ENG_SEC_ENUM;


// 
//                          
//                                                             
//
#define	CMDQ_ERR_NOMEM		(12)	//               
#define	CMDQ_ERR_FAULT		(14)	//            

#define CMDQ_ERR_ADDR_CONVERT_HANDLE_2_PA (1000)
#define CMDQ_ERR_ADDR_CONVER_PA           (1200)
#define CMDQ_ERR_ADDR_CONVERT_ALLOC_MVA   (1100)
#define CMDQ_ERR_ADDR_CONVERT_FREE_MVA    (1200)
#define CMDQ_ERR_PORT_CONFIG              (1300)

#define CMDQ_TZ_ERR_UNKNOWN_IWC_CMD       (5000)

#define CMDQ_ERR_TZ_IPC_EXECUTE_SESSION   (5001)
#define CMDQ_ERR_TZ_IPC_CLOSE_SESSION     (5002)
#define CMDQ_ERR_TZ_EXEC_FAILED           (5003)



/*                     */
#define TZCMD_CMDQ_SUBMIT_TASK          (1)  
#define TZCMD_CMDQ_RES_RELEASE          (2)  

#define TZCMD_CMDQ_TEST_HELLO           (4000) 
#define TZCMD_CMDQ_TEST_DUMMY           (4001)
#define TZCMD_CMDQ_TEST_SMI_DUMP        (4002)
#define TZCMD_CMDQ_DEBUG_SW_COPY        (4003)
#define TZCMD_CMDQ_TRAP_DR_INFINITELY   (4004)
#define TZCMD_CMDQ_DUMP                 (4005)
#define TZCMD_CMDQ_TEST_TASK_IWC        (4006)


/* 
                    
 */
#define EXIT_ERROR                  ((uint32_t)(-1))


#endif /*                       */
