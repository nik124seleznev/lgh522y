#ifndef __CMDQ_DRIVER_H__
#define __CMDQ_DRIVER_H__

#include <linux/kernel.h>
#include "cmdq_def.h"

typedef struct cmdqUsageInfoStruct {
	uint32_t count[CMDQ_MAX_ENGINE_COUNT];	/*                                */
} cmdqUsageInfoStruct;

typedef struct cmdqJobStruct {
	struct cmdqCommandStruct command;	/*                         */
	cmdqJobHandle_t hJob;	/*                               */
} cmdqJobStruct;

typedef struct cmdqJobResultStruct {
	cmdqJobHandle_t hJob;	/*                                                 */
	uint64_t engineFlag;	/*                                          */

	/*                                        */
	/*                                            */
	/*                                      */
	/*                                             */
	/*                                  */
	/*                                             */
	/*                                            */
	/*                                */
	cmdqRegValueStruct regValue;

	cmdqReadAddressStruct readAddress;	/*                                   */
} cmdqJobResultStruct;

typedef struct cmdqWriteAddressStruct {
    /*                                                                        */
	uint32_t count;

	/*                                                                                */
	/*                                                              */
	/*  */
	/*                                                                     */
	/*                                                                              */
	/*                                                            */
	uint32_t startPA;
} cmdqWriteAddressStruct;

#define CMDQ_IOCTL_MAGIC_NUMBER 'x'

#define CMDQ_IOCTL_LOCK_MUTEX   _IOW(CMDQ_IOCTL_MAGIC_NUMBER, 1, int)
#define CMDQ_IOCTL_UNLOCK_MUTEX _IOR(CMDQ_IOCTL_MAGIC_NUMBER, 2, int)
#define CMDQ_IOCTL_EXEC_COMMAND _IOW(CMDQ_IOCTL_MAGIC_NUMBER, 3, cmdqCommandStruct)
#define CMDQ_IOCTL_QUERY_USAGE  _IOW(CMDQ_IOCTL_MAGIC_NUMBER, 4, cmdqUsageInfoStruct)

/*  */
/*                  */
/*  */
#define CMDQ_IOCTL_ASYNC_JOB_EXEC _IOW(CMDQ_IOCTL_MAGIC_NUMBER, 5, cmdqJobStruct)
#define CMDQ_IOCTL_ASYNC_JOB_WAIT_AND_CLOSE _IOR(CMDQ_IOCTL_MAGIC_NUMBER, 6, cmdqJobResultStruct)

#define CMDQ_IOCTL_ALLOC_WRITE_ADDRESS _IOW(CMDQ_IOCTL_MAGIC_NUMBER, 7, cmdqWriteAddressStruct)
#define CMDQ_IOCTL_FREE_WRITE_ADDRESS _IOW(CMDQ_IOCTL_MAGIC_NUMBER, 8, cmdqWriteAddressStruct)
#define CMDQ_IOCTL_READ_ADDRESS_VALUE _IOW(CMDQ_IOCTL_MAGIC_NUMBER, 9, cmdqReadAddressStruct)

/*  */
/*                                                         */
/*                                  */
/*  */
#define CMDQ_IOCTL_QUERY_CAP_BITS _IOW(CMDQ_IOCTL_MAGIC_NUMBER, 10, int)


#endif				/*                   */
