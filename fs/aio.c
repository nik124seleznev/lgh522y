/*
 *	An async IO implementation for Linux
 *	Written by Benjamin LaHaise <bcrl@kvack.org>
 *
 *	Implements an efficient asynchronous io interface.
 *
 *	Copyright 2000, 2001, 2002 Red Hat, Inc.  All Rights Reserved.
 *
 *	See ../COPYING for licensing terms.
 */
#define pr_fmt(fmt) "%s: " fmt, __func__

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/time.h>
#include <linux/aio_abi.h>
#include <linux/export.h>
#include <linux/syscalls.h>
#include <linux/backing-dev.h>
#include <linux/uio.h>

#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/mm.h>
#include <linux/mman.h>
#include <linux/mmu_context.h>
#include <linux/slab.h>
#include <linux/timer.h>
#include <linux/aio.h>
#include <linux/highmem.h>
#include <linux/workqueue.h>
#include <linux/security.h>
#include <linux/eventfd.h>
#include <linux/blkdev.h>
#include <linux/compat.h>

#include <asm/kmap_types.h>
#include <asm/uaccess.h>

#define AIO_RING_MAGIC			0xa10a10a1
#define AIO_RING_COMPAT_FEATURES	1
#define AIO_RING_INCOMPAT_FEATURES	0
struct aio_ring {
	unsigned	id;	/*                              */
	unsigned	nr;	/*                     */
	unsigned	head;
	unsigned	tail;

	unsigned	magic;
	unsigned	compat_features;
	unsigned	incompat_features;
	unsigned	header_length;	/*                  */


	struct io_event		io_events[0];
}; /*                       */

#define AIO_RING_PAGES	8

struct kioctx {
	atomic_t		users;
	atomic_t		dead;

	/*                      */
	unsigned long		user_id;
	struct hlist_node	list;

	/*
                                                                  
                                                            
   
                                                              
                     
  */
	unsigned		max_reqs;

	/*                                                 */
	unsigned		nr_events;

	unsigned long		mmap_base;
	unsigned long		mmap_size;

	struct page		**ring_pages;
	long			nr_pages;

	struct rcu_head		rcu_head;
	struct work_struct	rcu_work;

	struct {
		atomic_t	reqs_active;
	} ____cacheline_aligned_in_smp;

	struct {
		spinlock_t	ctx_lock;
		struct list_head active_reqs;	/*                       */
	} ____cacheline_aligned_in_smp;

	struct {
		struct mutex	ring_lock;
		wait_queue_head_t wait;
	} ____cacheline_aligned_in_smp;

	struct {
		unsigned	tail;
		spinlock_t	completion_lock;
	} ____cacheline_aligned_in_smp;

	struct page		*internal_pages[AIO_RING_PAGES];
};

/*                           */
static DEFINE_SPINLOCK(aio_nr_lock);
unsigned long aio_nr;		/*                                            */
unsigned long aio_max_nr = 0x10000; /*                                            */
/*                           */

static struct kmem_cache	*kiocb_cachep;
static struct kmem_cache	*kioctx_cachep;

/*          
                                                             
                                                          
 */
static int __init aio_setup(void)
{
	kiocb_cachep = KMEM_CACHE(kiocb, SLAB_HWCACHE_ALIGN|SLAB_PANIC);
	kioctx_cachep = KMEM_CACHE(kioctx,SLAB_HWCACHE_ALIGN|SLAB_PANIC);

	pr_debug("sizeof(struct page) = %zu\n", sizeof(struct page));

	return 0;
}
__initcall(aio_setup);

static void aio_free_ring(struct kioctx *ctx)
{
	long i;

	for (i = 0; i < ctx->nr_pages; i++)
		put_page(ctx->ring_pages[i]);

	if (ctx->ring_pages && ctx->ring_pages != ctx->internal_pages)
		kfree(ctx->ring_pages);
}

static int aio_setup_ring(struct kioctx *ctx)
{
	struct aio_ring *ring;
	unsigned nr_events = ctx->max_reqs;
	struct mm_struct *mm = current->mm;
	unsigned long size, populate;
	int nr_pages;

	/*                                                          */
	nr_events += 2;	/*                                */

	size = sizeof(struct aio_ring);
	size += sizeof(struct io_event) * nr_events;
	nr_pages = (size + PAGE_SIZE-1) >> PAGE_SHIFT;

	if (nr_pages < 0)
		return -EINVAL;

	nr_events = (PAGE_SIZE * nr_pages - sizeof(struct aio_ring)) / sizeof(struct io_event);

	ctx->nr_events = 0;
	ctx->ring_pages = ctx->internal_pages;
	if (nr_pages > AIO_RING_PAGES) {
		ctx->ring_pages = kcalloc(nr_pages, sizeof(struct page *),
					  GFP_KERNEL);
		if (!ctx->ring_pages)
			return -ENOMEM;
	}

	ctx->mmap_size = nr_pages * PAGE_SIZE;
	pr_debug("attempting mmap of %lu bytes\n", ctx->mmap_size);
	down_write(&mm->mmap_sem);
	ctx->mmap_base = do_mmap_pgoff(NULL, 0, ctx->mmap_size,
				       PROT_READ|PROT_WRITE,
				       MAP_ANONYMOUS|MAP_PRIVATE, 0, &populate);
	if (IS_ERR((void *)ctx->mmap_base)) {
		up_write(&mm->mmap_sem);
		ctx->mmap_size = 0;
		aio_free_ring(ctx);
		return -EAGAIN;
	}

	pr_debug("mmap address: 0x%08lx\n", ctx->mmap_base);
	ctx->nr_pages = get_user_pages(current, mm, ctx->mmap_base, nr_pages,
				       1, 0, ctx->ring_pages, NULL);
	up_write(&mm->mmap_sem);

	if (unlikely(ctx->nr_pages != nr_pages)) {
		aio_free_ring(ctx);
		return -EAGAIN;
	}
	if (populate)
		mm_populate(ctx->mmap_base, populate);

	ctx->user_id = ctx->mmap_base;
	ctx->nr_events = nr_events; /*              */

	ring = kmap_atomic(ctx->ring_pages[0]);
	ring->nr = nr_events;	/*           */
	ring->id = ctx->user_id;
	ring->head = ring->tail = 0;
	ring->magic = AIO_RING_MAGIC;
	ring->compat_features = AIO_RING_COMPAT_FEATURES;
	ring->incompat_features = AIO_RING_INCOMPAT_FEATURES;
	ring->header_length = sizeof(struct aio_ring);
	kunmap_atomic(ring);
	flush_dcache_page(ctx->ring_pages[0]);

	return 0;
}

#define AIO_EVENTS_PER_PAGE	(PAGE_SIZE / sizeof(struct io_event))
#define AIO_EVENTS_FIRST_PAGE	((PAGE_SIZE - sizeof(struct aio_ring)) / sizeof(struct io_event))
#define AIO_EVENTS_OFFSET	(AIO_EVENTS_PER_PAGE - AIO_EVENTS_FIRST_PAGE)

void kiocb_set_cancel_fn(struct kiocb *req, kiocb_cancel_fn *cancel)
{
	struct kioctx *ctx = req->ki_ctx;
	unsigned long flags;

	spin_lock_irqsave(&ctx->ctx_lock, flags);

	if (!req->ki_list.next)
		list_add(&req->ki_list, &ctx->active_reqs);

	req->ki_cancel = cancel;

	spin_unlock_irqrestore(&ctx->ctx_lock, flags);
}
EXPORT_SYMBOL(kiocb_set_cancel_fn);

static int kiocb_cancel(struct kioctx *ctx, struct kiocb *kiocb,
			struct io_event *res)
{
	kiocb_cancel_fn *old, *cancel;
	int ret = -EINVAL;

	/*
                                                                  
                                                       
  */

	cancel = ACCESS_ONCE(kiocb->ki_cancel);
	do {
		if (!cancel || cancel == KIOCB_CANCELLED)
			return ret;

		old = cancel;
		cancel = cmpxchg(&kiocb->ki_cancel, old, KIOCB_CANCELLED);
	} while (cancel != old);

	atomic_inc(&kiocb->ki_users);
	spin_unlock_irq(&ctx->ctx_lock);

	memset(res, 0, sizeof(*res));
	res->obj = (u64)(unsigned long)kiocb->ki_obj.user;
	res->data = kiocb->ki_user_data;
	ret = cancel(kiocb, res);

	spin_lock_irq(&ctx->ctx_lock);

	return ret;
}

static void free_ioctx_rcu(struct rcu_head *head)
{
	struct kioctx *ctx = container_of(head, struct kioctx, rcu_head);
	kmem_cache_free(kioctx_cachep, ctx);
}

/*
                                                                             
                                                                                
                                               
 */
static void free_ioctx(struct kioctx *ctx)
{
	struct aio_ring *ring;
	struct io_event res;
	struct kiocb *req;
	unsigned head, avail;

	spin_lock_irq(&ctx->ctx_lock);

	while (!list_empty(&ctx->active_reqs)) {
		req = list_first_entry(&ctx->active_reqs,
				       struct kiocb, ki_list);

		list_del_init(&req->ki_list);
		kiocb_cancel(ctx, req, &res);
	}

	spin_unlock_irq(&ctx->ctx_lock);

	ring = kmap_atomic(ctx->ring_pages[0]);
	head = ring->head;
	kunmap_atomic(ring);

	while (atomic_read(&ctx->reqs_active) > 0) {
		wait_event(ctx->wait,
				head != ctx->tail ||
				atomic_read(&ctx->reqs_active) <= 0);

		avail = (head <= ctx->tail ? ctx->tail : ctx->nr_events) - head;

		head += avail;
		head %= ctx->nr_events;
	}

	WARN_ON(atomic_read(&ctx->reqs_active) < 0);

	aio_free_ring(ctx);

	pr_debug("freeing %p\n", ctx);

	/*
                                                                      
                                 
   
                                                                        
                                          
  */
	call_rcu(&ctx->rcu_head, free_ioctx_rcu);
}

static void put_ioctx(struct kioctx *ctx)
{
	if (unlikely(atomic_dec_and_test(&ctx->users)))
		free_ioctx(ctx);
}

/*            
                                                                        
 */
static struct kioctx *ioctx_alloc(unsigned nr_events)
{
	struct mm_struct *mm = current->mm;
	struct kioctx *ctx;
	int err = -ENOMEM;

	/*                   */
	if ((nr_events > (0x10000000U / sizeof(struct io_event))) ||
	    (nr_events > (0x10000000U / sizeof(struct kiocb)))) {
		pr_debug("ENOMEM: nr_events too high\n");
		return ERR_PTR(-EINVAL);
	}

	if (!nr_events || (unsigned long)nr_events > aio_max_nr)
		return ERR_PTR(-EAGAIN);

	ctx = kmem_cache_zalloc(kioctx_cachep, GFP_KERNEL);
	if (!ctx)
		return ERR_PTR(-ENOMEM);

	ctx->max_reqs = nr_events;

	atomic_set(&ctx->users, 2);
	atomic_set(&ctx->dead, 0);
	spin_lock_init(&ctx->ctx_lock);
	spin_lock_init(&ctx->completion_lock);
	mutex_init(&ctx->ring_lock);
	init_waitqueue_head(&ctx->wait);

	INIT_LIST_HEAD(&ctx->active_reqs);

	if (aio_setup_ring(ctx) < 0)
		goto out_freectx;

	/*                                      */
	spin_lock(&aio_nr_lock);
	if (aio_nr + nr_events > aio_max_nr ||
	    aio_nr + nr_events < aio_nr) {
		spin_unlock(&aio_nr_lock);
		goto out_cleanup;
	}
	aio_nr += ctx->max_reqs;
	spin_unlock(&aio_nr_lock);

	/*                            */
	spin_lock(&mm->ioctx_lock);
	hlist_add_head_rcu(&ctx->list, &mm->ioctx_list);
	spin_unlock(&mm->ioctx_lock);

	pr_debug("allocated ioctx %p[%ld]: mm=%p mask=0x%x\n",
		 ctx, ctx->user_id, mm, ctx->nr_events);
	return ctx;

out_cleanup:
	err = -EAGAIN;
	aio_free_ring(ctx);
out_freectx:
	kmem_cache_free(kioctx_cachep, ctx);
	pr_debug("error allocating ioctx %d\n", err);
	return ERR_PTR(err);
}

static void kill_ioctx_work(struct work_struct *work)
{
	struct kioctx *ctx = container_of(work, struct kioctx, rcu_work);

	wake_up_all(&ctx->wait);
	put_ioctx(ctx);
}

static void kill_ioctx_rcu(struct rcu_head *head)
{
	struct kioctx *ctx = container_of(head, struct kioctx, rcu_head);

	INIT_WORK(&ctx->rcu_work, kill_ioctx_work);
	schedule_work(&ctx->rcu_work);
}

/*           
                                                                
                                                                   
                                       
 */
static void kill_ioctx(struct mm_struct *mm, struct kioctx *ctx)
{
	if (!atomic_xchg(&ctx->dead, 1)) {
		spin_lock(&mm->ioctx_lock);
		hlist_del_rcu(&ctx->list);
		spin_unlock(&mm->ioctx_lock);

		/*
                                                               
                                                                  
                                                                 
                                                                
                  
   */
		spin_lock(&aio_nr_lock);
		BUG_ON(aio_nr - ctx->max_reqs > aio_nr);
		aio_nr -= ctx->max_reqs;
		spin_unlock(&aio_nr_lock);

		if (ctx->mmap_size)
			vm_munmap(ctx->mmap_base, ctx->mmap_size);

		/*                                                      */
		call_rcu(&ctx->rcu_head, kill_ioctx_rcu);
	}
}

/*                    
                                             
 */
ssize_t wait_on_sync_kiocb(struct kiocb *iocb)
{
	while (atomic_read(&iocb->ki_users)) {
		set_current_state(TASK_UNINTERRUPTIBLE);
		if (!atomic_read(&iocb->ki_users))
			break;
		io_schedule();
	}
	__set_current_state(TASK_RUNNING);
	return iocb->ki_user_data;
}
EXPORT_SYMBOL(wait_on_sync_kiocb);

/*
                                                                                
                                                                               
                         
  
                                                                            
        
 */
void exit_aio(struct mm_struct *mm)
{
	struct kioctx *ctx;
	struct hlist_node *n;

	hlist_for_each_entry_safe(ctx, n, &mm->ioctx_list, list) {
		if (1 != atomic_read(&ctx->users))
			printk(KERN_DEBUG
				"exit_aio:ioctx still alive: %d %d %d\n",
				atomic_read(&ctx->users),
				atomic_read(&ctx->dead),
				atomic_read(&ctx->reqs_active));
		/*
                                                 
                                                        
                                                    
                                                  
                                                  
                                               
   */
		ctx->mmap_size = 0;

		kill_ioctx(mm, ctx);
	}
}

/*            
                                                                     
                                                                       
                                                   
  
                                                                        
                                               
                                                                
                                                                
 */
static inline struct kiocb *aio_get_req(struct kioctx *ctx)
{
	struct kiocb *req;

	if (atomic_read(&ctx->reqs_active) >= ctx->nr_events)
		return NULL;

	if (atomic_inc_return(&ctx->reqs_active) > ctx->nr_events - 1)
		goto out_put;

	req = kmem_cache_alloc(kiocb_cachep, GFP_KERNEL|__GFP_ZERO);
	if (unlikely(!req))
		goto out_put;

	atomic_set(&req->ki_users, 2);
	req->ki_ctx = ctx;

	return req;
out_put:
	atomic_dec(&ctx->reqs_active);
	return NULL;
}

static void kiocb_free(struct kiocb *req)
{
	if (req->ki_filp)
		fput(req->ki_filp);
	if (req->ki_eventfd != NULL)
		eventfd_ctx_put(req->ki_eventfd);
	if (req->ki_dtor)
		req->ki_dtor(req);
	if (req->ki_iovec != &req->ki_inline_vec)
		kfree(req->ki_iovec);
	kmem_cache_free(kiocb_cachep, req);
}

void aio_put_req(struct kiocb *req)
{
	if (atomic_dec_and_test(&req->ki_users))
		kiocb_free(req);
}
EXPORT_SYMBOL(aio_put_req);

static struct kioctx *lookup_ioctx(unsigned long ctx_id)
{
	struct mm_struct *mm = current->mm;
	struct kioctx *ctx, *ret = NULL;

	rcu_read_lock();

	hlist_for_each_entry_rcu(ctx, &mm->ioctx_list, list) {
		if (ctx->user_id == ctx_id) {
			atomic_inc(&ctx->users);
			ret = ctx;
			break;
		}
	}

	rcu_read_unlock();
	return ret;
}

/*             
                                                            
 */
void aio_complete(struct kiocb *iocb, long res, long res2)
{
	struct kioctx	*ctx = iocb->ki_ctx;
	struct aio_ring	*ring;
	struct io_event	*ev_page, *event;
	unsigned long	flags;
	unsigned tail, pos;

	/*
                                         
                                                         
                                                                     
                                                        
                                                                     
  */
	if (is_sync_kiocb(iocb)) {
		BUG_ON(atomic_read(&iocb->ki_users) != 1);
		iocb->ki_user_data = res;
		atomic_set(&iocb->ki_users, 0);
		wake_up_process(iocb->ki_obj.tsk);
		return;
	}

	/*
                                                                     
                                                          
  */
	rcu_read_lock();

	if (iocb->ki_list.next) {
		unsigned long flags;

		spin_lock_irqsave(&ctx->ctx_lock, flags);
		list_del(&iocb->ki_list);
		spin_unlock_irqrestore(&ctx->ctx_lock, flags);
	}

	/*
                                                               
                                 
  */
	if (unlikely(xchg(&iocb->ki_cancel,
			  KIOCB_CANCELLED) == KIOCB_CANCELLED)) {
		atomic_dec(&ctx->reqs_active);
		/*                                                      */
		goto put_rq;
	}

	/*
                                                                   
                                                                  
                                                      
  */
	spin_lock_irqsave(&ctx->completion_lock, flags);

	tail = ctx->tail;
	pos = tail + AIO_EVENTS_OFFSET;

	if (++tail >= ctx->nr_events)
		tail = 0;

	ev_page = kmap_atomic(ctx->ring_pages[pos / AIO_EVENTS_PER_PAGE]);
	event = ev_page + pos % AIO_EVENTS_PER_PAGE;

	event->obj = (u64)(unsigned long)iocb->ki_obj.user;
	event->data = iocb->ki_user_data;
	event->res = res;
	event->res2 = res2;

	kunmap_atomic(ev_page);
	flush_dcache_page(ctx->ring_pages[pos / AIO_EVENTS_PER_PAGE]);

	pr_debug("%p[%u]: %p: %p %Lx %lx %lx\n",
		 ctx, tail, iocb, iocb->ki_obj.user, iocb->ki_user_data,
		 res, res2);

	/*                                       
                                    
  */
	smp_wmb();	/*                                         */

	ctx->tail = tail;

	ring = kmap_atomic(ctx->ring_pages[0]);
	ring->tail = tail;
	kunmap_atomic(ring);
	flush_dcache_page(ctx->ring_pages[0]);

	spin_unlock_irqrestore(&ctx->completion_lock, flags);

	pr_debug("added to ring %p at [%u]\n", iocb, tail);

	/*
                                                               
                                                               
                     
  */
	if (iocb->ki_eventfd != NULL)
		eventfd_signal(iocb->ki_eventfd, 1);

put_rq:
	/*                                                   */
	aio_put_req(iocb);
	atomic_dec(&ctx->reqs_active);

	/*
                                                            
                                                          
                                                        
                                   
  */
	smp_mb();

	if (waitqueue_active(&ctx->wait))
		wake_up(&ctx->wait);

	rcu_read_unlock();
}
EXPORT_SYMBOL(aio_complete);

/*                
                                                                      
                 
 */
static long aio_read_events_ring(struct kioctx *ctx,
				 struct io_event __user *event, long nr)
{
	struct aio_ring *ring;
	unsigned head, pos;
	long ret = 0;
	int copy_ret;

	mutex_lock(&ctx->ring_lock);

	ring = kmap_atomic(ctx->ring_pages[0]);
	head = ring->head;
	kunmap_atomic(ring);

	pr_debug("h%u t%u m%u\n", head, ctx->tail, ctx->nr_events);

	if (head == ctx->tail)
		goto out;

	head %= ctx->nr_events;

	while (ret < nr) {
		long avail;
		struct io_event *ev;
		struct page *page;

		avail = (head <= ctx->tail ? ctx->tail : ctx->nr_events) - head;
		if (head == ctx->tail)
			break;

		avail = min(avail, nr - ret);
		avail = min_t(long, avail, AIO_EVENTS_PER_PAGE -
			    ((head + AIO_EVENTS_OFFSET) % AIO_EVENTS_PER_PAGE));

		pos = head + AIO_EVENTS_OFFSET;
		page = ctx->ring_pages[pos / AIO_EVENTS_PER_PAGE];
		pos %= AIO_EVENTS_PER_PAGE;

		ev = kmap(page);
		copy_ret = copy_to_user(event + ret, ev + pos,
					sizeof(*ev) * avail);
		kunmap(page);

		if (unlikely(copy_ret)) {
			ret = -EFAULT;
			goto out;
		}

		ret += avail;
		head += avail;
		head %= ctx->nr_events;
	}

	ring = kmap_atomic(ctx->ring_pages[0]);
	ring->head = head;
	kunmap_atomic(ring);
	flush_dcache_page(ctx->ring_pages[0]);

	pr_debug("%li  h%u t%u\n", ret, head, ctx->tail);
out:
	mutex_unlock(&ctx->ring_lock);

	return ret;
}

static bool aio_read_events(struct kioctx *ctx, long min_nr, long nr,
			    struct io_event __user *event, long *i)
{
	long ret = aio_read_events_ring(ctx, event + *i, nr - *i);

	if (ret > 0)
		*i += ret;

	if (unlikely(atomic_read(&ctx->dead)))
		ret = -EINVAL;

	if (!*i)
		*i = ret;

	return ret < 0 || *i >= min_nr;
}

static long read_events(struct kioctx *ctx, long min_nr, long nr,
			struct io_event __user *event,
			struct timespec __user *timeout)
{
	ktime_t until = { .tv64 = KTIME_MAX };
	long ret = 0;

	if (timeout) {
		struct timespec	ts;

		if (unlikely(copy_from_user(&ts, timeout, sizeof(ts))))
			return -EFAULT;

		until = timespec_to_ktime(ts);
	}

	/*
                                                                         
                                                                  
                       
   
                                                                        
                                        
   
                                                                 
                                                                     
                                                                      
                                                                  
                                                     
  */
	wait_event_interruptible_hrtimeout(ctx->wait,
			aio_read_events(ctx, min_nr, nr, event, &ret), until);

	if (!ret && signal_pending(current))
		ret = -EINTR;

	return ret;
}

/*              
                                                                 
                                                                 
                                                             
                                                                      
                                                              
                                                                 
                                                                    
                                                                     
                                                                
                                                             
               
 */
SYSCALL_DEFINE2(io_setup, unsigned, nr_events, aio_context_t __user *, ctxp)
{
	struct kioctx *ioctx = NULL;
	unsigned long ctx;
	long ret;

	ret = get_user(ctx, ctxp);
	if (unlikely(ret))
		goto out;

	ret = -EINVAL;
	if (unlikely(ctx || nr_events == 0)) {
		pr_debug("EINVAL: io_setup: ctx %lu nr_events %u\n",
		         ctx, nr_events);
		goto out;
	}

	ioctx = ioctx_alloc(nr_events);
	ret = PTR_ERR(ioctx);
	if (!IS_ERR(ioctx)) {
		ret = put_user(ioctx->user_id, ctxp);
		if (ret)
			kill_ioctx(current->mm, ioctx);
		put_ioctx(ioctx);
	}

out:
	return ret;
}

/*                
                                                                  
                                                               
                                                                
              
 */
SYSCALL_DEFINE1(io_destroy, aio_context_t, ctx)
{
	struct kioctx *ioctx = lookup_ioctx(ctx);
	if (likely(NULL != ioctx)) {
		kill_ioctx(current->mm, ioctx);
		put_ioctx(ioctx);
		return 0;
	}
	pr_debug("EINVAL: io_destroy: invalid context id\n");
	return -EINVAL;
}

static void aio_advance_iovec(struct kiocb *iocb, ssize_t ret)
{
	struct iovec *iov = &iocb->ki_iovec[iocb->ki_cur_seg];

	BUG_ON(ret <= 0);

	while (iocb->ki_cur_seg < iocb->ki_nr_segs && ret > 0) {
		ssize_t this = min((ssize_t)iov->iov_len, ret);
		iov->iov_base += this;
		iov->iov_len -= this;
		iocb->ki_left -= this;
		ret -= this;
		if (iov->iov_len == 0) {
			iocb->ki_cur_seg++;
			iov++;
		}
	}

	/*                                                         
                         */
	BUG_ON(ret > 0 && iocb->ki_left == 0);
}

typedef ssize_t (aio_rw_op)(struct kiocb *, const struct iovec *,
			    unsigned long, loff_t);

static ssize_t aio_rw_vect_retry(struct kiocb *iocb, int rw, aio_rw_op *rw_op)
{
	struct file *file = iocb->ki_filp;
	struct address_space *mapping = file->f_mapping;
	struct inode *inode = mapping->host;
	ssize_t ret = 0;

	/*                                         */
	if (iocb->ki_pos < 0)
		return -EINVAL;

	if (rw == WRITE)
		file_start_write(file);
	do {
		ret = rw_op(iocb, &iocb->ki_iovec[iocb->ki_cur_seg],
			    iocb->ki_nr_segs - iocb->ki_cur_seg,
			    iocb->ki_pos);
		if (ret > 0)
			aio_advance_iovec(iocb, ret);

	/*                                                                
                  */
	} while (ret > 0 && iocb->ki_left > 0 &&
		 (rw == WRITE ||
		  (!S_ISFIFO(inode->i_mode) && !S_ISSOCK(inode->i_mode))));
	if (rw == WRITE)
		file_end_write(file);

	/*                                                       */
	/*                          */
	if ((ret == 0) || (iocb->ki_left == 0))
		ret = iocb->ki_nbytes - iocb->ki_left;

	/*                                                            
                        */
	if (rw == WRITE
	    && ret < 0 && ret != -EIOCBQUEUED
	    && iocb->ki_nbytes - iocb->ki_left)
		ret = iocb->ki_nbytes - iocb->ki_left;

	return ret;
}

static ssize_t aio_setup_vectored_rw(int rw, struct kiocb *kiocb, bool compat)
{
	ssize_t ret;

	kiocb->ki_nr_segs = kiocb->ki_nbytes;

#ifdef CONFIG_COMPAT
	if (compat)
		ret = compat_rw_copy_check_uvector(rw,
				(struct compat_iovec __user *)kiocb->ki_buf,
				kiocb->ki_nr_segs, 1, &kiocb->ki_inline_vec,
				&kiocb->ki_iovec);
	else
#endif
		ret = rw_copy_check_uvector(rw,
				(struct iovec __user *)kiocb->ki_buf,
				kiocb->ki_nr_segs, 1, &kiocb->ki_inline_vec,
				&kiocb->ki_iovec);
	if (ret < 0)
		return ret;

	/*                                             */
	kiocb->ki_nbytes = ret;
	return 0;
}

static ssize_t aio_setup_single_vector(int rw, struct kiocb *kiocb)
{
	if (unlikely(!access_ok(!rw, kiocb->ki_buf, kiocb->ki_nbytes)))
		return -EFAULT;

	kiocb->ki_iovec = &kiocb->ki_inline_vec;
	kiocb->ki_iovec->iov_base = kiocb->ki_buf;
	kiocb->ki_iovec->iov_len = kiocb->ki_nbytes;
	kiocb->ki_nr_segs = 1;
	return 0;
}

/*
                  
                                                   
                                                    
 */
static ssize_t aio_run_iocb(struct kiocb *req, bool compat)
{
	struct file *file = req->ki_filp;
	ssize_t ret;
	int rw;
	fmode_t mode;
	aio_rw_op *rw_op;

	switch (req->ki_opcode) {
	case IOCB_CMD_PREAD:
	case IOCB_CMD_PREADV:
		mode	= FMODE_READ;
		rw	= READ;
		rw_op	= file->f_op->aio_read;
		goto rw_common;

	case IOCB_CMD_PWRITE:
	case IOCB_CMD_PWRITEV:
		mode	= FMODE_WRITE;
		rw	= WRITE;
		rw_op	= file->f_op->aio_write;
		goto rw_common;
rw_common:
		if (unlikely(!(file->f_mode & mode)))
			return -EBADF;

		if (!rw_op)
			return -EINVAL;

		ret = (req->ki_opcode == IOCB_CMD_PREADV ||
		       req->ki_opcode == IOCB_CMD_PWRITEV)
			? aio_setup_vectored_rw(rw, req, compat)
			: aio_setup_single_vector(rw, req);
		if (ret)
			return ret;

		ret = rw_verify_area(rw, file, &req->ki_pos, req->ki_nbytes);
		if (ret < 0)
			return ret;

		req->ki_nbytes = ret;
		req->ki_left = ret;

		ret = aio_rw_vect_retry(req, rw, rw_op);
		break;

	case IOCB_CMD_FDSYNC:
		if (!file->f_op->aio_fsync)
			return -EINVAL;

		ret = file->f_op->aio_fsync(req, 1);
		break;

	case IOCB_CMD_FSYNC:
		if (!file->f_op->aio_fsync)
			return -EINVAL;

		ret = file->f_op->aio_fsync(req, 0);
		break;

	default:
		pr_debug("EINVAL: no operation provided\n");
		return -EINVAL;
	}

	if (ret != -EIOCBQUEUED) {
		/*
                                                                 
                                                          
   */
		if (unlikely(ret == -ERESTARTSYS || ret == -ERESTARTNOINTR ||
			     ret == -ERESTARTNOHAND ||
			     ret == -ERESTART_RESTARTBLOCK))
			ret = -EINTR;
		aio_complete(req, ret, 0);
	}

	return 0;
}

static int io_submit_one(struct kioctx *ctx, struct iocb __user *user_iocb,
			 struct iocb *iocb, bool compat)
{
	struct kiocb *req;
	ssize_t ret;

	/*                                         */
	if (unlikely(iocb->aio_reserved1 || iocb->aio_reserved2)) {
		pr_debug("EINVAL: reserve field set\n");
		return -EINVAL;
	}

	/*                   */
	if (unlikely(
	    (iocb->aio_buf != (unsigned long)iocb->aio_buf) ||
	    (iocb->aio_nbytes != (size_t)iocb->aio_nbytes) ||
	    ((ssize_t)iocb->aio_nbytes < 0)
	   )) {
		pr_debug("EINVAL: io_submit: overflow check\n");
		return -EINVAL;
	}

	req = aio_get_req(ctx);
	if (unlikely(!req))
		return -EAGAIN;

	req->ki_filp = fget(iocb->aio_fildes);
	if (unlikely(!req->ki_filp)) {
		ret = -EBADF;
		goto out_put_req;
	}

	if (iocb->aio_flags & IOCB_FLAG_RESFD) {
		/*
                                                            
                                                           
                                                             
                                               
   */
		req->ki_eventfd = eventfd_ctx_fdget((int) iocb->aio_resfd);
		if (IS_ERR(req->ki_eventfd)) {
			ret = PTR_ERR(req->ki_eventfd);
			req->ki_eventfd = NULL;
			goto out_put_req;
		}
	}

	ret = put_user(KIOCB_KEY, &user_iocb->aio_key);
	if (unlikely(ret)) {
		pr_debug("EFAULT: aio_key\n");
		goto out_put_req;
	}

	req->ki_obj.user = user_iocb;
	req->ki_user_data = iocb->aio_data;
	req->ki_pos = iocb->aio_offset;

	req->ki_buf = (char __user *)(unsigned long)iocb->aio_buf;
	req->ki_left = req->ki_nbytes = iocb->aio_nbytes;
	req->ki_opcode = iocb->aio_lio_opcode;

	ret = aio_run_iocb(req, compat);
	if (ret)
		goto out_put_req;

	aio_put_req(req);	/*                       */
	return 0;
out_put_req:
	atomic_dec(&ctx->reqs_active);
	aio_put_req(req);	/*                       */
	aio_put_req(req);	/*                     */
	return ret;
}

long do_io_submit(aio_context_t ctx_id, long nr,
		  struct iocb __user *__user *iocbpp, bool compat)
{
	struct kioctx *ctx;
	long ret = 0;
	int i = 0;
	struct blk_plug plug;

	if (unlikely(nr < 0))
		return -EINVAL;

	if (unlikely(nr > LONG_MAX/sizeof(*iocbpp)))
		nr = LONG_MAX/sizeof(*iocbpp);

	if (unlikely(!access_ok(VERIFY_READ, iocbpp, (nr*sizeof(*iocbpp)))))
		return -EFAULT;

	ctx = lookup_ioctx(ctx_id);
	if (unlikely(!ctx)) {
		pr_debug("EINVAL: invalid context id\n");
		return -EINVAL;
	}

	blk_start_plug(&plug);

	/*
                                                                     
                           
  */
	for (i=0; i<nr; i++) {
		struct iocb __user *user_iocb;
		struct iocb tmp;

		if (unlikely(__get_user(user_iocb, iocbpp + i))) {
			ret = -EFAULT;
			break;
		}

		if (unlikely(copy_from_user(&tmp, user_iocb, sizeof(tmp)))) {
			ret = -EFAULT;
			break;
		}

		ret = io_submit_one(ctx, user_iocb, &tmp, compat);
		if (ret)
			break;
	}
	blk_finish_plug(&plug);

	put_ioctx(ctx);
	return i ? i : ret;
}

/*               
                                                                   
                                                                     
                                                               
                                                                     
                                                                 
                                                                    
                                                                 
                                                                    
                                                                     
                                        
 */
SYSCALL_DEFINE3(io_submit, aio_context_t, ctx_id, long, nr,
		struct iocb __user * __user *, iocbpp)
{
	return do_io_submit(ctx_id, nr, iocbpp, 0);
}

/*             
                                       
 */
static struct kiocb *lookup_kiocb(struct kioctx *ctx, struct iocb __user *iocb,
				  u32 key)
{
	struct list_head *pos;

	assert_spin_locked(&ctx->ctx_lock);

	if (key != KIOCB_KEY)
		return NULL;

	/*                                        */
	list_for_each(pos, &ctx->active_reqs) {
		struct kiocb *kiocb = list_kiocb(pos);
		if (kiocb->ki_obj.user == iocb)
			return kiocb;
	}
	return NULL;
}

/*               
                                                                 
                                                                  
                                                                   
                                                              
                                                                
                                                              
                                                                
                                                         
 */
SYSCALL_DEFINE3(io_cancel, aio_context_t, ctx_id, struct iocb __user *, iocb,
		struct io_event __user *, result)
{
	struct io_event res;
	struct kioctx *ctx;
	struct kiocb *kiocb;
	u32 key;
	int ret;

	ret = get_user(key, &iocb->aio_key);
	if (unlikely(ret))
		return -EFAULT;

	ctx = lookup_ioctx(ctx_id);
	if (unlikely(!ctx))
		return -EINVAL;

	spin_lock_irq(&ctx->ctx_lock);

	kiocb = lookup_kiocb(ctx, iocb, key);
	if (kiocb)
		ret = kiocb_cancel(ctx, kiocb, &res);
	else
		ret = -EINVAL;

	spin_unlock_irq(&ctx->ctx_lock);

	if (!ret) {
		/*                                          
                            
   */
		if (copy_to_user(result, &res, sizeof(res)))
			ret = -EFAULT;
	}

	put_ioctx(ctx);

	return ret;
}

/*              
                                                                   
                                                                   
                                                                    
                                                                    
                                                                   
                                                              
                                                           
                                                                
                                                                     
                                                                   
 */
SYSCALL_DEFINE5(io_getevents, aio_context_t, ctx_id,
		long, min_nr,
		long, nr,
		struct io_event __user *, events,
		struct timespec __user *, timeout)
{
	struct kioctx *ioctx = lookup_ioctx(ctx_id);
	long ret = -EINVAL;

	if (likely(ioctx)) {
		if (likely(min_nr <= nr && min_nr >= 0))
			ret = read_events(ioctx, min_nr, nr, events, timeout);
		put_ioctx(ioctx);
	}
	return ret;
}
