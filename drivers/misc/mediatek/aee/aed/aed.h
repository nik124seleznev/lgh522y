#ifndef __aed_h
#define __aed_h

#include <generated/autoconf.h>
#include <linux/types.h>
#include <linux/bug.h>
#include <linux/aee.h>
#include <linux/kallsyms.h>
#include <linux/ptrace.h>

#define LOGD(fmt, msg...)	pr_notice(fmt, ##msg)
#define LOGV(fmt, msg...)
#define LOGI	LOGD
#define LOGE(fmt, msg...)	pr_err(fmt, ##msg)
#define LOGW	LOGE

#define IPANIC_MODULE_TAG "KERNEL-PANIC"

#define AE_INVALID              0xAEEFF000
#define AE_NOT_AVAILABLE        0xAEE00000
#define AE_DEFAULT              0xAEE00001

typedef enum {
	AEE_MODE_MTK_ENG = 1,
	AEE_MODE_MTK_USER,
	AEE_MODE_CUSTOMER_ENG,
	AEE_MODE_CUSTOMER_USER
} AEE_MODE;

typedef enum {
	AEE_FORCE_DISABLE_RED_SCREEN = 0,
	AEE_FORCE_RED_SCREEN,
	AEE_FORCE_NOT_SET
} AEE_FORCE_RED_SCREEN_VALUE;

typedef enum {
	AE_SUCC,
	AE_FAIL
} AE_ERR;

typedef enum {
	AE_PASS_BY_MEM,
	AE_PASS_BY_FILE,
	AE_PASS_METHOD_END
} AE_PASS_METHOD;

typedef enum { AE_REQ, AE_RSP, AE_IND, AE_CMD_TYPE_END } AE_CMD_TYPE;

typedef enum {
	AE_REQ_IDX,

	AE_REQ_CLASS,
	AE_REQ_TYPE,
	AE_REQ_PROCESS,
	AE_REQ_MODULE,
	AE_REQ_BACKTRACE,
	AE_REQ_DETAIL,		/*                                  
                                                            
     */

	AE_REQ_ROOT_LOG_DIR,
	AE_REQ_CURR_LOG_DIR,
	AE_REQ_DFLT_LOG_DIR,
	AE_REQ_MAIN_LOG_FILE_PATH,

	AE_IND_FATAL_RAISED,	/*                                                  */
	AE_IND_EXP_RAISED,	/*                                                      */
	AE_IND_WRN_RAISED,	/*                                                    */
	AE_IND_REM_RAISED,	/*                                                      */

	AE_IND_LOG_STATUS,	/*              */
	AE_IND_LOG_CLOSE,	/*              */

	AE_REQ_SWITCH_DAL_BEEP,	/*                                   */
	AE_REQ_DB_COUNT,	/*               */
	AE_REQ_DB_FORCE_PATH,	/*                           */
	AE_REQ_SWITCH_EXP_LEVEL,
	AE_REQ_IS_AED_READY,	/*                                   */
	AE_REQ_COREDUMP,	/*                     */
	AE_REQ_SET_READFLAG,	/*                   */
	AE_REQ_E2S_INIT,	/*                                                                   */
	AE_CMD_ID_END
} AE_CMD_ID;

typedef struct {
	AE_CMD_TYPE cmdType;	/*              */
	AE_CMD_ID cmdId;	/*            */
	union {
		unsigned int seq;	/*                                    */
		int pid;	/*            */
	};
	union {
		unsigned int arg;	/*                 */
		AE_EXP_CLASS cls;	/*                              */
	};
	union {
		unsigned int len;	/*                         */
		int id;		/*            */
	};
	unsigned int dbOption;	/*                */
} AE_Msg;

/*                        */
struct aee_dal_show {
	char msg[1024];
};

struct aee_dal_setcolor {
	unsigned int foreground;
	unsigned int background;
	unsigned int screencolor;
};

#define MAX_AEE_KERNEL_BT 16	/*                                                 */
#define AEE_NR_FRAME 32

struct aee_ioctl {
	__u32 pid;
	__u32 detail;
	__u32 size;
	__u32 pad;
	__u64 in;
	__u64 out;
};

struct aee_thread_reg {
	pid_t tid;
	struct pt_regs regs;
};

#define AEEIOCTL_DAL_SHOW       _IOW('p', 0x01, struct aee_dal_show)	/*                           */
#define AEEIOCTL_DAL_CLEAN      _IO('p', 0x02)	/*                 */
#define AEEIOCTL_SETCOLOR       _IOW('p', 0x03, struct aee_dal_setcolor)	/*                      */
									    /*                                                                       *//*                     */
#define AEEIOCTL_GET_PROCESS_BT _IOW('p', 0x04, struct aee_ioctl)
#define AEEIOCTL_GET_SMP_INFO   _IOR('p', 0x05, int)
#define AEEIOCTL_SET_AEE_MODE   _IOR('p', 0x06, int)
#define AEEIOCTL_GET_THREAD_REG _IOW('p', 0x07, struct aee_thread_reg)
#define AEEIOCTL_CHECK_SUID_DUMPABLE _IOR('p', 0x08, int)
/*                   */
#define AEEIOCTL_WDT_KICK_POWERKEY _IOR('p', 0x09, int)

#define AEEIOCTL_RT_MON_Kick _IOR('p', 0x0A, int)
#define AEEIOCTL_SET_FORECE_RED_SCREEN _IOR('p', 0x0B, int)

#define AED_FILE_OPS(entry) \
  static const struct file_operations proc_##entry##_fops = { \
    .read = proc_##entry##_read, \
    .write = proc_##entry##_write, \
  }

#define AED_FILE_OPS_RO(entry) \
  static const struct file_operations proc_##entry##_fops = { \
    .read = proc_##entry##_read, \
  }

#define  AED_PROC_ENTRY(name, entry, mode)\
  if (!proc_create(#name, S_IFREG | mode, aed_proc_dir, &proc_##entry##_fops)) \
	  LOGE("proc_create %s failed\n", #name)


struct proc_dir_entry;

int aed_proc_debug_init(struct proc_dir_entry *aed_proc_dir);
int aed_proc_debug_done(struct proc_dir_entry *aed_proc_dir);

int aed_get_process_bt(struct aee_process_bt *bt);

void aee_rr_proc_init(struct proc_dir_entry *aed_proc_dir);
void aee_rr_proc_done(struct proc_dir_entry *aed_proc_dir);

void dram_console_init(struct proc_dir_entry *aed_proc_dir);
void dram_console_done(struct proc_dir_entry *aed_proc_dir);

struct aee_oops *ipanic_oops_copy(void);
void ipanic_oops_free(struct aee_oops *oops, int erase);
#endif
