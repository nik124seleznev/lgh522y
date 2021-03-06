#ifndef __LINUX__AIO_H
#define __LINUX__AIO_H

#include <linux/list.h>
#include <linux/workqueue.h>
#include <linux/aio_abi.h>
#include <linux/uio.h>
#include <linux/rcupdate.h>

#include <linux/atomic.h>

struct kioctx;
struct kiocb;

#define KIOCB_KEY		0

/*
                                                                               
                                                                       
                                                                         
              
  
                                                                                
                                                                     
                                                                              
                                                                         
 */
#define KIOCB_CANCELLED		((void *) (~0ULL))

typedef int (kiocb_cancel_fn)(struct kiocb *, struct io_event *);

struct kiocb {
	atomic_t		ki_users;

	struct file		*ki_filp;
	struct kioctx		*ki_ctx;	/*                   */
	kiocb_cancel_fn		*ki_cancel;
	void			(*ki_dtor)(struct kiocb *);

	union {
		void __user		*user;
		struct task_struct	*tsk;
	} ki_obj;

	__u64			ki_user_data;	/*                            */
	loff_t			ki_pos;

	void			*private;
	/*                                                     */
	unsigned short		ki_opcode;
	size_t			ki_nbytes; 	/*                          */
	char 			__user *ki_buf;	/*                         */
	size_t			ki_left; 	/*                 */
	struct iovec		ki_inline_vec;	/*               */
 	struct iovec		*ki_iovec;
 	unsigned long		ki_nr_segs;
 	unsigned long		ki_cur_seg;

	struct list_head	ki_list;	/*                       
                          */

	/*
                                                             
                                                                
  */
	struct eventfd_ctx	*ki_eventfd;
};

static inline bool is_sync_kiocb(struct kiocb *kiocb)
{
	return kiocb->ki_ctx == NULL;
}

static inline void init_sync_kiocb(struct kiocb *kiocb, struct file *filp)
{
	*kiocb = (struct kiocb) {
			.ki_users = ATOMIC_INIT(1),
			.ki_ctx = NULL,
			.ki_filp = filp,
			.ki_obj.tsk = current,
		};
}

/*            */
#ifdef CONFIG_AIO
extern ssize_t wait_on_sync_kiocb(struct kiocb *iocb);
extern void aio_put_req(struct kiocb *iocb);
extern void aio_complete(struct kiocb *iocb, long res, long res2);
struct mm_struct;
extern void exit_aio(struct mm_struct *mm);
extern long do_io_submit(aio_context_t ctx_id, long nr,
			 struct iocb __user *__user *iocbpp, bool compat);
void kiocb_set_cancel_fn(struct kiocb *req, kiocb_cancel_fn *cancel);
#else
static inline ssize_t wait_on_sync_kiocb(struct kiocb *iocb) { return 0; }
static inline void aio_put_req(struct kiocb *iocb) { }
static inline void aio_complete(struct kiocb *iocb, long res, long res2) { }
struct mm_struct;
static inline void exit_aio(struct mm_struct *mm) { }
static inline long do_io_submit(aio_context_t ctx_id, long nr,
				struct iocb __user * __user *iocbpp,
				bool compat) { return 0; }
static inline void kiocb_set_cancel_fn(struct kiocb *req,
				       kiocb_cancel_fn *cancel) { }
#endif /*            */

static inline struct kiocb *list_kiocb(struct list_head *h)
{
	return list_entry(h, struct kiocb, ki_list);
}

/*             */
extern unsigned long aio_nr;
extern unsigned long aio_max_nr;

#endif /*                */
