/*
 *    tape device driver for 3480/3490E/3590 tapes.
 *
 *  S390 and zSeries version
 *    Copyright IBM Corp. 2001, 2009
 *    Author(s): Carsten Otte <cotte@de.ibm.com>
 *		 Tuan Ngo-Anh <ngoanh@de.ibm.com>
 *		 Martin Schwidefsky <schwidefsky@de.ibm.com>
 *		 Stefan Bader <shbader@de.ibm.com>
 */

#ifndef _TAPE_H
#define _TAPE_H

#include <asm/ccwdev.h>
#include <asm/debug.h>
#include <asm/idals.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mtio.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>

struct gendisk;

/*
                                                                  
 */
#define DBF_LIKE_HELL
#ifdef  DBF_LIKE_HELL
#define DBF_LH(level, str, ...) \
do { \
	debug_sprintf_event(TAPE_DBF_AREA, level, str, ## __VA_ARGS__); \
} while (0)
#else
#define DBF_LH(level, str, ...) do {} while(0)
#endif

/*
                                  
 */
#define DBF_EVENT(d_level, d_str...) \
do { \
	debug_sprintf_event(TAPE_DBF_AREA, d_level, d_str); \
} while (0)

#define DBF_EXCEPTION(d_level, d_str...) \
do { \
	debug_sprintf_exception(TAPE_DBF_AREA, d_level, d_str); \
} while (0)

#define TAPE_VERSION_MAJOR 2
#define TAPE_VERSION_MINOR 0
#define TAPE_MAGIC "tape"

#define TAPE_MINORS_PER_DEV 2	    /*                       */
#define TAPEBLOCK_HSEC_SIZE	2048
#define TAPEBLOCK_HSEC_S2B	2
#define TAPEBLOCK_RETRIES	5

enum tape_medium_state {
	MS_UNKNOWN,
	MS_LOADED,
	MS_UNLOADED,
	MS_SIZE
};

enum tape_state {
	TS_UNUSED=0,
	TS_IN_USE,
	TS_BLKUSE,
	TS_INIT,
	TS_NOT_OPER,
	TS_SIZE
};

enum tape_op {
	TO_BLOCK,	/*            */
	TO_BSB,		/*                      */
	TO_BSF,		/*                         */
	TO_DSE,		/*                     */
	TO_FSB,		/*                     */
	TO_FSF,		/*                        */
	TO_LBL,		/*                    */
	TO_NOP,		/*              */
	TO_RBA,		/*               */
	TO_RBI,		/*                        */
	TO_RFO,		/*              */
	TO_REW,		/*             */
	TO_RUN,		/*                        */
	TO_WRI,		/*             */
	TO_WTM,		/*                 */
	TO_MSEN,	/*              */
	TO_LOAD,	/*           */
	TO_READ_CONFIG, /*                         */
	TO_READ_ATTMSG, /*                        */
	TO_DIS,		/*              */
	TO_ASSIGN,	/*                             */
	TO_UNASSIGN,	/*                                 */
	TO_CRYPT_ON,	/*                   */
	TO_CRYPT_OFF,	/*                    */
	TO_KEKL_SET,	/*               */
	TO_KEKL_QUERY,	/*                 */
	TO_RDC,		/*                             */
	TO_SIZE,	/*                       */
};

/*                     */
struct tape_device;

/*                              */
enum tape_request_status {
	TAPE_REQUEST_INIT,	/*                                  */
	TAPE_REQUEST_QUEUED,	/*                                   */
	TAPE_REQUEST_IN_IO,	/*                            */
	TAPE_REQUEST_DONE,	/*                       */
	TAPE_REQUEST_CANCEL,	/*                             */
	TAPE_REQUEST_LONG_BUSY, /*                                             */
};

/*                  */
struct tape_request {
	struct list_head list;		/*                                 */
	struct tape_device *device;	/*                             */
	struct ccw1 *cpaddr;		/*                                 */
	void *cpdata;			/*                      */
	enum tape_request_status status;/*                        */
	int options;			/*                        */
	int retries;			/*                                   */
	int rescnt;			/*                              */

	/*                                       */
	void (*callback)(struct tape_request *, void *);
	void *callback_data;

	enum tape_op op;
	int rc;
};

/*                                          */
typedef int (*tape_mtop_fn)(struct tape_device *, int);

/*                                                         */
#define TAPE_NR_MTOPS (MTMKPART+1)

/*                 */
struct tape_discipline {
	struct module *owner;
	int  (*setup_device)(struct tape_device *);
	void (*cleanup_device)(struct tape_device *);
	int (*irq)(struct tape_device *, struct tape_request *, struct irb *);
	struct tape_request *(*read_block)(struct tape_device *, size_t);
	struct tape_request *(*write_block)(struct tape_device *, size_t);
	void (*process_eov)(struct tape_device*);
	/*                                       */
	int (*ioctl_fn)(struct tape_device *, unsigned int, unsigned long);
	/*                                                   */
	tape_mtop_fn *mtop_array;
};

/*
                                                                      
                                                                           
 */
#define TAPE_IO_SUCCESS		0	/*                    */
#define TAPE_IO_PENDING		1	/*                       */
#define TAPE_IO_RETRY		2	/*                          */
#define TAPE_IO_STOP		3	/*                          */
#define TAPE_IO_LONG_BUSY	4	/*                           */

/*                    */
struct tape_char_data {
	struct idal_buffer *idal_buf;	/*                                */
	int block_size;			/*                       */
};

/*           */
struct tape_device {
	/*                           */
	struct list_head		node;

	int				cdev_id;
	struct ccw_device *		cdev;
	struct tape_class_device *	nt;
	struct tape_class_device *	rt;

	/*                                          */
	struct mutex			mutex;

	/*                                */
	struct tape_discipline *	discipline;
	void *				discdata;

	/*                      */
	long				tape_generic_status;

	/*                           */
	wait_queue_head_t		state_change_wq;
	enum tape_state			tape_state;
	enum tape_medium_state		medium_state;
	unsigned char *			modeset_byte;

	/*                  */
	atomic_t			ref_count;

	/*                */
	struct list_head		req_queue;

	/*                     */
	wait_queue_head_t		wait_queue;

	/*                                                     */
	int				first_minor;

	/*                                                       */
	int				required_tapemarks;

	/*                     */
	unsigned int			bof;

	/*                                */
	struct tape_char_data		char_data;

	/*                                                   */
	struct delayed_work		tape_dnr;

	/*                     */
	struct timer_list		lb_timeout;

};

/*                            */
extern struct tape_request *tape_alloc_request(int cplength, int datasize);
extern void tape_free_request(struct tape_request *);
extern int tape_do_io(struct tape_device *, struct tape_request *);
extern int tape_do_io_async(struct tape_device *, struct tape_request *);
extern int tape_do_io_interruptible(struct tape_device *, struct tape_request *);
extern int tape_cancel_io(struct tape_device *, struct tape_request *);
void tape_hotplug_event(struct tape_device *, int major, int action);

static inline int
tape_do_io_free(struct tape_device *device, struct tape_request *request)
{
	int rc;

	rc = tape_do_io(device, request);
	tape_free_request(request);
	return rc;
}

static inline void
tape_do_io_async_free(struct tape_device *device, struct tape_request *request)
{
	request->callback = (void *) tape_free_request;
	request->callback_data = NULL;
	tape_do_io_async(device, request);
}

extern int tape_oper_handler(int irq, int status);
extern void tape_noper_handler(int irq, int status);
extern int tape_open(struct tape_device *);
extern int tape_release(struct tape_device *);
extern int tape_mtop(struct tape_device *, int, int);
extern void tape_state_set(struct tape_device *, enum tape_state);

extern int tape_generic_online(struct tape_device *, struct tape_discipline *);
extern int tape_generic_offline(struct ccw_device *);
extern int tape_generic_pm_suspend(struct ccw_device *);

/*                              */
extern int tape_generic_probe(struct ccw_device *);
extern void tape_generic_remove(struct ccw_device *);

extern struct tape_device *tape_find_device(int devindex);
extern struct tape_device *tape_get_device(struct tape_device *);
extern void tape_put_device(struct tape_device *);

/*                            */
extern int tapechar_init(void);
extern void tapechar_exit(void);
extern int  tapechar_setup_device(struct tape_device *);
extern void tapechar_cleanup_device(struct tape_device *);

/*                               */
#ifdef CONFIG_PROC_FS
extern void tape_proc_init (void);
extern void tape_proc_cleanup (void);
#else
static inline void tape_proc_init (void) {;}
static inline void tape_proc_cleanup (void) {;}
#endif

/*                                          */
extern void tape_dump_sense_dbf(struct tape_device *, struct tape_request *,
				struct irb *);

/*                                               */
extern void tape_med_state_set(struct tape_device *, enum tape_medium_state);

/*                */
extern debug_info_t *TAPE_DBF_AREA;

/*                             */
static inline struct ccw1 *
tape_ccw_cc(struct ccw1 *ccw, __u8 cmd_code, __u16 memsize, void *cda)
{
	ccw->cmd_code = cmd_code;
	ccw->flags = CCW_FLAG_CC;
	ccw->count = memsize;
	ccw->cda = (__u32)(addr_t) cda;
	return ccw + 1;
}

static inline struct ccw1 *
tape_ccw_end(struct ccw1 *ccw, __u8 cmd_code, __u16 memsize, void *cda)
{
	ccw->cmd_code = cmd_code;
	ccw->flags = 0;
	ccw->count = memsize;
	ccw->cda = (__u32)(addr_t) cda;
	return ccw + 1;
}

static inline struct ccw1 *
tape_ccw_cmd(struct ccw1 *ccw, __u8 cmd_code)
{
	ccw->cmd_code = cmd_code;
	ccw->flags = 0;
	ccw->count = 0;
	ccw->cda = (__u32)(addr_t) &ccw->cmd_code;
	return ccw + 1;
}

static inline struct ccw1 *
tape_ccw_repeat(struct ccw1 *ccw, __u8 cmd_code, int count)
{
	while (count-- > 0) {
		ccw->cmd_code = cmd_code;
		ccw->flags = CCW_FLAG_CC;
		ccw->count = 0;
		ccw->cda = (__u32)(addr_t) &ccw->cmd_code;
		ccw++;
	}
	return ccw;
}

static inline struct ccw1 *
tape_ccw_cc_idal(struct ccw1 *ccw, __u8 cmd_code, struct idal_buffer *idal)
{
	ccw->cmd_code = cmd_code;
	ccw->flags    = CCW_FLAG_CC;
	idal_buffer_set_cda(idal, ccw);
	return ccw++;
}

static inline struct ccw1 *
tape_ccw_end_idal(struct ccw1 *ccw, __u8 cmd_code, struct idal_buffer *idal)
{
	ccw->cmd_code = cmd_code;
	ccw->flags    = 0;
	idal_buffer_set_cda(idal, ccw);
	return ccw++;
}

/*             */
extern const char *tape_state_verbose[];
extern const char *tape_op_verbose[];

#endif /*                  */
