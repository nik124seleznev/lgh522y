#ifndef S390_DEVICE_H
#define S390_DEVICE_H

#include <asm/ccwdev.h>
#include <linux/atomic.h>
#include <linux/wait.h>
#include <linux/notifier.h>
#include <linux/kernel_stat.h>
#include "io_sch.h"

/*
                                    
 */
enum dev_state {
	DEV_STATE_NOT_OPER,
	DEV_STATE_SENSE_PGID,
	DEV_STATE_SENSE_ID,
	DEV_STATE_OFFLINE,
	DEV_STATE_VERIFY,
	DEV_STATE_ONLINE,
	DEV_STATE_W4SENSE,
	DEV_STATE_DISBAND_PGID,
	DEV_STATE_BOXED,
	/*                                                          */
	DEV_STATE_TIMEOUT_KILL,
	DEV_STATE_QUIESCE,
	/*                                                 */
	DEV_STATE_DISCONNECTED,
	DEV_STATE_DISCONNECTED_SENSE_ID,
	DEV_STATE_CMFCHANGE,
	DEV_STATE_CMFUPDATE,
	DEV_STATE_STEAL_LOCK,
	/*               */
	NR_DEV_STATES
};

/*
                                                 
 */
enum dev_event {
	DEV_EVENT_NOTOPER,
	DEV_EVENT_INTERRUPT,
	DEV_EVENT_TIMEOUT,
	DEV_EVENT_VERIFY,
	/*               */
	NR_DEV_EVENTS
};

struct ccw_device;

/*
                                  
 */
typedef void (fsm_func_t)(struct ccw_device *, enum dev_event);
extern fsm_func_t *dev_jumptable[NR_DEV_STATES][NR_DEV_EVENTS];

static inline void
dev_fsm_event(struct ccw_device *cdev, enum dev_event dev_event)
{
	int state = cdev->private->state;

	if (dev_event == DEV_EVENT_INTERRUPT) {
		if (state == DEV_STATE_ONLINE)
			inc_irq_stat(cdev->private->int_class);
		else if (state != DEV_STATE_CMFCHANGE &&
			 state != DEV_STATE_CMFUPDATE)
			inc_irq_stat(IRQIO_CIO);
	}
	dev_jumptable[state][dev_event](cdev, dev_event);
}

/*
                                           
 */
static inline int
dev_fsm_final_state(struct ccw_device *cdev)
{
	return (cdev->private->state == DEV_STATE_NOT_OPER ||
		cdev->private->state == DEV_STATE_OFFLINE ||
		cdev->private->state == DEV_STATE_ONLINE ||
		cdev->private->state == DEV_STATE_BOXED);
}

int __init io_subchannel_init(void);

void io_subchannel_recog_done(struct ccw_device *cdev);
void io_subchannel_init_config(struct subchannel *sch);

int ccw_device_cancel_halt_clear(struct ccw_device *);

int ccw_device_is_orphan(struct ccw_device *);

void ccw_device_recognition(struct ccw_device *);
int ccw_device_online(struct ccw_device *);
int ccw_device_offline(struct ccw_device *);
void ccw_device_update_sense_data(struct ccw_device *);
int ccw_device_test_sense_data(struct ccw_device *);
void ccw_device_schedule_sch_unregister(struct ccw_device *);
int ccw_purge_blacklisted(void);
void ccw_device_sched_todo(struct ccw_device *cdev, enum cdev_todo todo);
struct ccw_device *get_ccwdev_by_dev_id(struct ccw_dev_id *dev_id);

/*                                                              */
void ccw_device_accumulate_irb(struct ccw_device *, struct irb *);
void ccw_device_accumulate_basic_sense(struct ccw_device *, struct irb *);
int ccw_device_accumulate_and_sense(struct ccw_device *, struct irb *);
int ccw_device_do_sense(struct ccw_device *, struct irb *);

/*                                                   */
int lpm_adjust(int lpm, int mask);
void ccw_request_start(struct ccw_device *);
int ccw_request_cancel(struct ccw_device *cdev);
void ccw_request_handler(struct ccw_device *cdev);
void ccw_request_timeout(struct ccw_device *cdev);
void ccw_request_notoper(struct ccw_device *cdev);

/*                                         */
void ccw_device_sense_id_start(struct ccw_device *);
void ccw_device_sense_id_done(struct ccw_device *, int);

/*                                              */
void ccw_device_verify_start(struct ccw_device *);
void ccw_device_verify_done(struct ccw_device *, int);

void ccw_device_disband_start(struct ccw_device *);
void ccw_device_disband_done(struct ccw_device *, int);

void ccw_device_stlck_start(struct ccw_device *, void *, void *, void *);
void ccw_device_stlck_done(struct ccw_device *, void *, int);

int ccw_device_call_handler(struct ccw_device *);

int ccw_device_stlck(struct ccw_device *);

/*                                             */
void ccw_device_trigger_reprobe(struct ccw_device *);
void ccw_device_kill_io(struct ccw_device *);
int ccw_device_notify(struct ccw_device *, int);
void ccw_device_set_disconnected(struct ccw_device *cdev);
void ccw_device_set_notoper(struct ccw_device *cdev);

void ccw_device_set_timeout(struct ccw_device *, int);

/*                                      */
void retry_set_schib(struct ccw_device *cdev);
void cmf_retry_copy_block(struct ccw_device *);
int cmf_reenable(struct ccw_device *);
int ccw_set_cmf(struct ccw_device *cdev, int enable);
extern struct device_attribute dev_attr_cmb_enable;
#endif