#ifndef __MT6575_PANICLOG_H__
#define __MT6575_PANICLOG_H__

#define PANICLOG_BUF_LEN 16384
#define PANICLOG_HEADER_SIZE 512
#define PANICLOG_SECTION_SIZE (PANICLOG_BUF_LEN) + (PANICLOG_HEADER_SIZE)

#ifndef __ASSEMBLY__

#define PANICLOG_VALID_PATTERN 0x90EFABCD
#define PANICLOG_BACKTRACE_NUM 4
#define PANICLOG_PROCESS_NAME_LENGTH 256

struct paniclog {
	int valid;		/*                                                 */
	int crc;

	struct {
		unsigned long bt_where[PANICLOG_BACKTRACE_NUM];
		unsigned long bt_from[PANICLOG_BACKTRACE_NUM];
		char process_path[PANICLOG_PROCESS_NAME_LENGTH];

		char buf[PANICLOG_BUF_LEN];
		int buf_len;
	} c;
};

extern struct paniclog *paniclog;

#ifdef CONFIG_PANICLOG

/*                             */
void paniclog_start(void);

void paniclog_end(void);

/*                                                */
void paniclog_ptree_store(struct task_struct *tsk);

/*                                               */
void paniclog_stack_store(unsigned long where, unsigned long from);

/*                              */
int paniclog_is_available(void);

/*                                            */
void paniclog_dump(void);

/*                                                                  */
void paniclog_copy_and_clear(struct paniclog *log);

#else

#define paniclog_start(a)

#define  paniclog_end()

#define paniclog_ptree_store(tsk)

#define paniclog_stack_store(where, from)

#define paniclog_is_available() 0

#define paniclog_copy_and_clear(log)

#endif /*                    */ 

#endif /*            */

#endif  /*                        */

