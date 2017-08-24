#ifndef __CMDQ_SEC_H__
#define __CMDQ_SEC_H__

#include "cmdq_core.h"

#if defined(CMDQ_SECURE_PATH_SUPPORT)
#include "mobicore_driver_api.h"
#include "cmdq_sec_iwc_common.h"
#endif

/* 
                      
 */

#define CMDQ_ERR_NULL_SEC_CTX_HANDLE (6000)
#define CMDQ_ERR_SEC_CTX_SETUP (6001)
#define CMDQ_ERR_SEC_CTX_TEARDOWN (6002)

/* 
                                  
 */
typedef enum {
	IWC_INIT = 0,
	IWC_MOBICORE_OPENED = 1,
	IWC_WSM_ALLOCATED = 2,
	IWC_SES_OPENED = 3,
	IWC_SES_MSG_PACKAGED = 4,
	IWC_SES_TRANSACTED = 5,
	IWC_SES_ON_TRANSACTED = 6,
	IWC_END_OF_ENUM = 7,
} CMDQ_IWC_STATE_ENUM;


/* 
                             
                                                                        
 */
typedef struct cmdqSecContextStruct {
	struct list_head listEntry;

	/*            */
	uint32_t tgid;		/*                         */
	uint32_t referCount;	/*                                           */

	/*           */
	CMDQ_IWC_STATE_ENUM state;

	/*                 */
	void *iwcMessage;	/*                */
#if defined(CMDQ_SECURE_PATH_SUPPORT)
	struct mc_uuid_t uuid;	/*                                               */
	struct mc_session_handle sessionHandle;	/*                */
#endif
	uint32_t openMobicoreByOther;	/*                                                                     */
} cmdqSecContextStruct, *cmdqSecContextHandle;

/* 
                                                 
                                                         
  
                                                                                        
 */
int32_t cmdq_sec_create_shared_memory(cmdqSecSharedMemoryHandle *pHandle, const uint32_t size);
int32_t cmdq_sec_destroy_shared_memory(cmdqSecSharedMemoryHandle handle);

/* 
                                                  
  
          
                          
                                  
                                 
                                                    
          
                       
 */
typedef int32_t(*CmdqSecFillIwcCB) (int32_t, void *, int32_t, void *);


/* 
                                                 
                
                      
  */

int32_t cmdq_sec_exec_task_async_unlocked(TaskStruct *pTask, int32_t thread);
int32_t cmdq_sec_cancel_error_task_unlocked(TaskStruct *pTask, int32_t thread, cmdqSecCancelTaskResultStruct *pResult);
int32_t cmdq_sec_allocate_path_resource_unlocked(void);


/* 
                       
  */
void cmdq_sec_lock_secure_path(void);
void cmdq_sec_unlock_secure_path(void);

void cmdqSecInitialize(void);
void cmdqSecDeInitialize(void);

void cmdqSecEnableProfile(const bool enable);

#endif				/*                    */
