#ifndef CSR_LOG_H__
#define CSR_LOG_H__
/*****************************************************************************

	(c) Cambridge Silicon Radio Limited 2010
	All rights reserved and confidential information of CSR

	Refer to LICENSE.txt included with this source for details
	on the license terms.

*****************************************************************************/

#include "csr_sched.h"
#include "csr_prim_defs.h"
#include "csr_msgconv.h"

/*
                
 */

/*                                                    */
/*                                                    */
/*                                                    */
typedef u32 CsrLogLevelEnvironment;
#define CSR_LOG_LEVEL_ENVIRONMENT_OFF          ((CsrLogLevelEnvironment) 0x00000000) /*                                       */
#define CSR_LOG_LEVEL_ENVIRONMENT_BCI_ACL      ((CsrLogLevelEnvironment) 0x00000001) /*                                                    */
#define CSR_LOG_LEVEL_ENVIRONMENT_BCI_HCI      ((CsrLogLevelEnvironment) 0x00000002) /*                                                        */
#define CSR_LOG_LEVEL_ENVIRONMENT_BCI_SCO      ((CsrLogLevelEnvironment) 0x00000004) /*                                                    */
#define CSR_LOG_LEVEL_ENVIRONMENT_BCI_VENDOR   ((CsrLogLevelEnvironment) 0x00000008) /*                                                                                                  */
#define CSR_LOG_LEVEL_ENVIRONMENT_TRANSPORTS   ((CsrLogLevelEnvironment) 0x00000010) /*                                                                                          */
#define CSR_LOG_LEVEL_ENVIRONMENT_BGINT_REG    ((CsrLogLevelEnvironment) 0x00000020) /*                                                     */
#define CSR_LOG_LEVEL_ENVIRONMENT_BGINT_UNREG  ((CsrLogLevelEnvironment) 0x00000040) /*                                                       */
#define CSR_LOG_LEVEL_ENVIRONMENT_BGINT_SET    ((CsrLogLevelEnvironment) 0x00000080) /*                                            */
#define CSR_LOG_LEVEL_ENVIRONMENT_BGINT_START  ((CsrLogLevelEnvironment) 0x00000100) /*                                              */
#define CSR_LOG_LEVEL_ENVIRONMENT_BGINT_DONE   ((CsrLogLevelEnvironment) 0x00000200) /*                                             */
#define CSR_LOG_LEVEL_ENVIRONMENT_PROTO        ((CsrLogLevelEnvironment) 0x00000400) /*                                      */
#define CSR_LOG_LEVEL_ENVIRONMENT_PROTO_LOC    ((CsrLogLevelEnvironment) 0x00000800) /*                                                                                                                                                              */
/*                                                          */
#define CSR_LOG_LEVEL_ENVIRONMENT_ALL          ((CsrLogLevelEnvironment) 0xFFFFFFFF) /*                                                                                                                                                                */

/*                                                    */
/*                                                    */
/*                                                    */
typedef u32 CsrLogLevelTask;
#define CSR_LOG_LEVEL_TASK_OFF                 ((CsrLogLevelTask) 0x00000000) /*                                    */
#define CSR_LOG_LEVEL_TASK_TEXT                ((CsrLogLevelTask) 0x00000001) /*                                                                                                                                               */
#define CSR_LOG_LEVEL_TASK_TEXT_LOC            ((CsrLogLevelTask) 0x00000002) /*                                                                                                                                                */
#define CSR_LOG_LEVEL_TASK_STATE               ((CsrLogLevelTask) 0x00000004) /*                                            */
#define CSR_LOG_LEVEL_TASK_STATE_NAME          ((CsrLogLevelTask) 0x00000008) /*                                                                                                                                                */
#define CSR_LOG_LEVEL_TASK_STATE_LOC           ((CsrLogLevelTask) 0x00000010) /*                                                                                                                                                    */
#define CSR_LOG_LEVEL_TASK_TASK_SWITCH         ((CsrLogLevelTask) 0x00000020) /*                                                 */
#define CSR_LOG_LEVEL_TASK_MESSAGE_PUT         ((CsrLogLevelTask) 0x00000080) /*                                   */
#define CSR_LOG_LEVEL_TASK_MESSAGE_PUT_LOC     ((CsrLogLevelTask) 0x00000100) /*                                                                                                                                           */
#define CSR_LOG_LEVEL_TASK_MESSAGE_GET         ((CsrLogLevelTask) 0x00000200) /*                                   */
#define CSR_LOG_LEVEL_TASK_MESSAGE_QUEUE_PUSH  ((CsrLogLevelTask) 0x00000400) /*                                    */
#define CSR_LOG_LEVEL_TASK_MESSAGE_QUEUE_POP   ((CsrLogLevelTask) 0x00000800) /*                                   */
#define CSR_LOG_LEVEL_TASK_PRIM_ONLY_TYPE      ((CsrLogLevelTask) 0x00001000) /*                                                                                                              */
#define CSR_LOG_LEVEL_TASK_PRIM_APPLY_LIMIT    ((CsrLogLevelTask) 0x00002000) /*                                                                                                                                                                                                               */
#define CSR_LOG_LEVEL_TASK_TIMER_IN            ((CsrLogLevelTask) 0x00004000) /*                                */
#define CSR_LOG_LEVEL_TASK_TIMER_IN_LOC        ((CsrLogLevelTask) 0x00008000) /*                                                                                                                                         */
#define CSR_LOG_LEVEL_TASK_TIMER_CANCEL        ((CsrLogLevelTask) 0x00010000) /*                                    */
#define CSR_LOG_LEVEL_TASK_TIMER_CANCEL_LOC    ((CsrLogLevelTask) 0x00020000) /*                                                                                                                                               */
#define CSR_LOG_LEVEL_TASK_TIMER_FIRE          ((CsrLogLevelTask) 0x00040000) /*                                  */
#define CSR_LOG_LEVEL_TASK_TIMER_DONE          ((CsrLogLevelTask) 0x00080000) /*                                  */
/*                                                          */
#define CSR_LOG_LEVEL_TASK_ALL                 ((CsrLogLevelTask) 0xFFFFFFFF & ~(CSR_LOG_LEVEL_TASK_PRIM_ONLY_TYPE | CSR_LOG_LEVEL_TASK_PRIM_APPLY_LIMIT)) /*                                                                                                                                                         */

u8 CsrLogEnvironmentIsFiltered(CsrLogLevelEnvironment level);
CsrLogLevelTask CsrLogTaskFilterGet(CsrSchedQid taskId);
u8 CsrLogTaskIsFiltered(CsrSchedQid taskId, CsrLogLevelTask level);

/*
                
 */
#define CSR_LOG_STRINGIFY_REAL(a) (#a)
#define CSR_LOG_STRINGIFY(a) CSR_LOG_STRINGIFY_REAL(a)

typedef struct {
	u16            primitiveType;
	const char *primitiveName;
	CsrMsgConvMsgEntry  *messageConv; /*                      */
} CsrLogPrimitiveInformation;

typedef struct {
	const char        *techVer;
	u32                   primitiveInfoCount;
	CsrLogPrimitiveInformation *primitiveInfo;
} CsrLogTechInformation;

/*                                 */
/*               */
/*                                 */
typedef u8 bitmask8_t;
typedef u16 bitmask16_t;
typedef u32 bitmask32_t;

#ifdef CSR_LOG_ENABLE
#ifdef CSR_LOG_INCLUDE_FILE_NAME_AND_LINE_NUMBER
/*                                         */
#define CSR_LOG_TEXT(text) \
	do { \
		if (!CsrLogTaskIsFiltered(CsrSchedTaskQueueGet(), CSR_LOG_LEVEL_TASK_TEXT)) { \
			CsrLogTaskText(text, __LINE__, __FILE__); \
		} \
	} while (0)
#else
/*                                         */
#define CSR_LOG_TEXT(text) \
	do { \
		if (!CsrLogTaskIsFiltered(CsrSchedTaskQueueGet(), CSR_LOG_LEVEL_TASK_TEXT)) { \
			CsrLogTaskText(text, 0, NULL); \
		} \
	} while (0)
#endif
#else
#define CSR_LOG_TEXT(text)
#endif

/*                                         */
void CsrLogTaskText(const char *text,
	u32 line,
	const char *file);

#define CSR_LOG_STATE_TRANSITION_MASK_FSM_NAME          (0x001)
#define CSR_LOG_STATE_TRANSITION_MASK_NEXT_STATE        (0x002)
#define CSR_LOG_STATE_TRANSITION_MASK_NEXT_STATE_STR    (0x004)
#define CSR_LOG_STATE_TRANSITION_MASK_PREV_STATE        (0x008)
#define CSR_LOG_STATE_TRANSITION_MASK_PREV_STATE_STR    (0x010)
#define CSR_LOG_STATE_TRANSITION_MASK_EVENT             (0x020)
#define CSR_LOG_STATE_TRANSITION_MASK_EVENT_STR         (0x040)

/*                                         */
void CsrLogStateTransition(bitmask16_t mask,
	u32 identifier,
	const char *fsm_name,
	u32 prev_state,
	const char *prev_state_str,
	u32 in_event,
	const char *in_event_str,
	u32 next_state,
	const char *next_state_str,
	u32 line,
	const char *file);

/*                                 */
/*              */
/*                                 */
void CsrLogSchedInit(u8 thread_id);
void CsrLogSchedDeinit(u8 thread_id);

void CsrLogSchedStart(u8 thread_id);
void CsrLogSchedStop(u8 thread_id);

void CsrLogInitTask(u8 thread_id, CsrSchedQid tskid, const char *tskName);
void CsrLogDeinitTask(u16 task_id);

void CsrLogActivate(CsrSchedQid tskid);
void CsrLogDeactivate(CsrSchedQid tskid);

#define SYNERGY_SERIALIZER_TYPE_DUMP    (0x000)
#define SYNERGY_SERIALIZER_TYPE_SER     (0x001)

void CsrLogMessagePut(u32 line,
	const char *file,
	CsrSchedQid src_task_id,
	CsrSchedQid dst_taskid,
	CsrSchedMsgId msg_id,
	u16 prim_type,
	const void *msg);

void CsrLogMessageGet(CsrSchedQid src_task_id,
	CsrSchedQid dst_taskid,
	u8 get_res,
	CsrSchedMsgId msg_id,
	u16 prim_type,
	const void *msg);

void CsrLogTimedEventIn(u32 line,
	const char *file,
	CsrSchedQid task_id,
	CsrSchedTid tid,
	u32 requested_delay,
	u16 fniarg,
	const void *fnvarg);

void CsrLogTimedEventFire(CsrSchedQid task_id,
	CsrSchedTid tid);

void CsrLogTimedEventDone(CsrSchedQid task_id,
	CsrSchedTid tid);

void CsrLogTimedEventCancel(u32 line,
	const char *file,
	CsrSchedQid task_id,
	CsrSchedTid tid,
	u8 cancel_res);

void CsrLogBgintRegister(u8 thread_id,
	CsrSchedBgint irq,
	const char *callback,
	const void *ptr);
void CsrLogBgintUnregister(CsrSchedBgint irq);
void CsrLogBgintSet(CsrSchedBgint irq);
void CsrLogBgintServiceStart(CsrSchedBgint irq);
void CsrLogBgintServiceDone(CsrSchedBgint irq);

void CsrLogExceptionStateEvent(u16 prim_type,
	CsrPrim msg_type,
	u16 state,
	u32 line,
	const char *file);
void CsrLogExceptionGeneral(u16 prim_type,
	u16 state,
	const char *text,
	u32 line,
	const char *file);
void CsrLogExceptionWarning(u16 prim_type,
	u16 state,
	const char *text,
	u32 line,
	const char *file);

#endif
