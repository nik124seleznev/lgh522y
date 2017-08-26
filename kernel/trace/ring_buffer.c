/*
 * Generic ring buffer
 *
 * Copyright (C) 2008 Steven Rostedt <srostedt@redhat.com>
 */
#include <linux/ftrace_event.h>
#include <linux/ring_buffer.h>
#include <linux/trace_clock.h>
#include <linux/trace_seq.h>
#include <linux/spinlock.h>
#include <linux/irq_work.h>
#include <linux/debugfs.h>
#include <linux/uaccess.h>
#include <linux/hardirq.h>
#include <linux/kthread.h>	/*               */
#include <linux/kmemcheck.h>
#include <linux/module.h>
#include <linux/percpu.h>
#include <linux/mutex.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/hash.h>
#include <linux/list.h>
#include <linux/cpu.h>
#include <linux/fs.h>

#include <asm/local.h>

#ifdef CONFIG_MTK_EXTMEM
extern void* extmem_malloc_page_align(size_t bytes);
extern void extmem_free(void* mem);
#endif

static void update_pages_handler(struct work_struct *work);

/*
                                                                  
 */
int ring_buffer_print_entry_header(struct trace_seq *s)
{
	int ret;

	ret = trace_seq_printf(s, "# compressed entry header\n");
	ret = trace_seq_printf(s, "\ttype_len    :    5 bits\n");
	ret = trace_seq_printf(s, "\ttime_delta  :   27 bits\n");
	ret = trace_seq_printf(s, "\tarray       :   32 bits\n");
	ret = trace_seq_printf(s, "\n");
	ret = trace_seq_printf(s, "\tpadding     : type == %d\n",
			       RINGBUF_TYPE_PADDING);
	ret = trace_seq_printf(s, "\ttime_extend : type == %d\n",
			       RINGBUF_TYPE_TIME_EXTEND);
	ret = trace_seq_printf(s, "\tdata max type_len  == %d\n",
			       RINGBUF_TYPE_DATA_TYPE_LEN_MAX);

	return ret;
}

/*
                                                                             
                                                                      
                                                                           
                           
  
                                                                         
                                                                          
                                                        
  
                                                                            
                                                                         
                                                   
  
                               
  
             
                                  
             
                                          
                                          
                                          
                                        
                                        
                                        
  
  
             
                                  
                                
                                          
                                          
                                          
                                        
                                        
                                        
  
  
             
                                  
                                
                                          
                                          
                                          
                                        
                                        
                                        
  
  
             
                                  
                                
                                          
                                          
                                          
                                        
                                        
                                        
  
  
                                                                           
                                                                           
                                      
  
                                                           
  
 */

/*
                                                         
                                                               
                                                    
                                                      
                                                     
  
                                                         
                      
  
                                   
                                                    
                                                       
  
                                                             
                                           
 */

/*
                                                       
                                   
  
                 
                  
                                           
                                          
                                                            
 */

enum {
	RB_BUFFERS_ON_BIT	= 0,
	RB_BUFFERS_DISABLED_BIT	= 1,
};

enum {
	RB_BUFFERS_ON		= 1 << RB_BUFFERS_ON_BIT,
	RB_BUFFERS_DISABLED	= 1 << RB_BUFFERS_DISABLED_BIT,
};

static unsigned long ring_buffer_flags __read_mostly = RB_BUFFERS_ON;

/*                                                 */
#define RB_BUFFER_OFF		(1 << 20)

#define BUF_PAGE_HDR_SIZE offsetof(struct buffer_data_page, data)

/* 
                                                           
  
                                                            
               
 */
void tracing_off_permanent(void)
{
	set_bit(RB_BUFFERS_DISABLED_BIT, &ring_buffer_flags);
}

#define RB_EVNT_HDR_SIZE (offsetof(struct ring_buffer_event, array))
#define RB_ALIGNMENT		4U
#define RB_MAX_SMALL_DATA	(RB_ALIGNMENT * RINGBUF_TYPE_DATA_TYPE_LEN_MAX)
#define RB_EVNT_MIN_SIZE	8U	/*                 */

#ifndef CONFIG_HAVE_64BIT_ALIGNED_ACCESS
# define RB_FORCE_8BYTE_ALIGNMENT	0
# define RB_ARCH_ALIGNMENT		RB_ALIGNMENT
#else
# define RB_FORCE_8BYTE_ALIGNMENT	1
# define RB_ARCH_ALIGNMENT		8U
#endif

#define RB_ALIGN_DATA		__aligned(RB_ARCH_ALIGNMENT)

/*                                                        */
#define RINGBUF_TYPE_DATA 0 ... RINGBUF_TYPE_DATA_TYPE_LEN_MAX

enum {
	RB_LEN_TIME_EXTEND = 8,
	RB_LEN_TIME_STAMP = 16,
};

#define skip_time_extend(event) \
	((struct ring_buffer_event *)((char *)event + RB_LEN_TIME_EXTEND))

static inline int rb_null_event(struct ring_buffer_event *event)
{
	return event->type_len == RINGBUF_TYPE_PADDING && !event->time_delta;
}

static void rb_event_set_padding(struct ring_buffer_event *event)
{
	/*                               */
	event->type_len = RINGBUF_TYPE_PADDING;
	event->time_delta = 0;
}

static unsigned
rb_event_data_length(struct ring_buffer_event *event)
{
	unsigned length;

	if (event->type_len)
		length = event->type_len * RB_ALIGNMENT;
	else
		length = event->array[0];
	return length + RB_EVNT_HDR_SIZE;
}

/*
                                                    
                                                  
               
 */
static inline unsigned
rb_event_length(struct ring_buffer_event *event)
{
	switch (event->type_len) {
	case RINGBUF_TYPE_PADDING:
		if (rb_null_event(event))
			/*           */
			return -1;
		return  event->array[0] + RB_EVNT_HDR_SIZE;

	case RINGBUF_TYPE_TIME_EXTEND:
		return RB_LEN_TIME_EXTEND;

	case RINGBUF_TYPE_TIME_STAMP:
		return RB_LEN_TIME_STAMP;

	case RINGBUF_TYPE_DATA:
		return rb_event_data_length(event);
	default:
		BUG();
	}
	/*         */
	return 0;
}

/*
                                               
                                                   
 */
static inline unsigned
rb_event_ts_length(struct ring_buffer_event *event)
{
	unsigned len = 0;

	if (event->type_len == RINGBUF_TYPE_TIME_EXTEND) {
		/*                                              */
		len = RB_LEN_TIME_EXTEND;
		event = skip_time_extend(event);
	}
	return len + rb_event_length(event);
}

/* 
                                                            
                                         
  
                                                     
                                                        
                                                           
                                                           
                                        
 */
unsigned ring_buffer_event_length(struct ring_buffer_event *event)
{
	unsigned length;

	if (event->type_len == RINGBUF_TYPE_TIME_EXTEND)
		event = skip_time_extend(event);

	length = rb_event_length(event);
	if (event->type_len > RINGBUF_TYPE_DATA_TYPE_LEN_MAX)
		return length;
	length -= RB_EVNT_HDR_SIZE;
	if (length > RB_MAX_SMALL_DATA + sizeof(event->array[0]))
                length -= sizeof(event->array[0]);
	return length;
}
EXPORT_SYMBOL_GPL(ring_buffer_event_length);

/*                                   */
static void *
rb_event_data(struct ring_buffer_event *event)
{
	if (event->type_len == RINGBUF_TYPE_TIME_EXTEND)
		event = skip_time_extend(event);
	BUG_ON(event->type_len > RINGBUF_TYPE_DATA_TYPE_LEN_MAX);
	/*                                                       */
	if (event->type_len)
		return (void *)&event->array[0];
	/*                                                           */
	return (void *)&event->array[1];
}

/* 
                                                        
                                         
 */
void *ring_buffer_event_data(struct ring_buffer_event *event)
{
	return rb_event_data(event);
}
EXPORT_SYMBOL_GPL(ring_buffer_event_data);

#define for_each_buffer_cpu(buffer, cpu)		\
	for_each_cpu(cpu, buffer->cpumask)

#define TS_SHIFT	27
#define TS_MASK		((1ULL << TS_SHIFT) - 1)
#define TS_DELTA_TEST	(~TS_MASK)

/*                                   */
#define RB_MISSED_EVENTS	(1 << 31)
/*                            */
#define RB_MISSED_STORED	(1 << 30)

struct buffer_data_page {
	u64		 time_stamp;	/*                 */
	local_t		 commit;	/*                       */
	unsigned char	 data[] RB_ALIGN_DATA;	/*                     */
};

/*
                                                             
                                                             
                                                          
                                                          
                                                                 
            
 */
struct buffer_page {
	struct list_head list;		/*                      */
	local_t		 write;		/*                      */
	unsigned	 read;		/*                     */
	local_t		 entries;	/*                      */
	unsigned long	 real_end;	/*                  */
	struct buffer_data_page *page;	/*                  */
};

/*
                                                             
                                                                
                                                            
                                                                  
  
                                                               
                                                                
                                                      
  
                                                    
 */
#define RB_WRITE_MASK		0xfffff
#define RB_WRITE_INTCNT		(1 << 20)

static void rb_init_page(struct buffer_data_page *bpage)
{
	local_set(&bpage->commit, 0);
}

/* 
                                                       
                          
  
                                                                        
 */
size_t ring_buffer_page_len(void *page)
{
	return local_read(&((struct buffer_data_page *)page)->commit)
		+ BUF_PAGE_HDR_SIZE;
}

/*
                                                                       
                  
 */
static void free_buffer_page(struct buffer_page *bpage)
{
#ifdef CONFIG_MTK_EXTMEM
	extmem_free((void*) bpage->page);	 
#else
	free_page((unsigned long)bpage->page);
#endif
	kfree(bpage);
}

/*
                                                    
 */
static inline int test_time_stamp(u64 delta)
{
	if (delta & TS_DELTA_TEST)
		return 1;
	return 0;
}

#define BUF_PAGE_SIZE (PAGE_SIZE - BUF_PAGE_HDR_SIZE)

/*                                                */
#define BUF_MAX_DATA_SIZE (BUF_PAGE_SIZE - (sizeof(u32) * 2))

int ring_buffer_print_page_header(struct trace_seq *s)
{
	struct buffer_data_page field;
	int ret;

	ret = trace_seq_printf(s, "\tfield: u64 timestamp;\t"
			       "offset:0;\tsize:%u;\tsigned:%u;\n",
			       (unsigned int)sizeof(field.time_stamp),
			       (unsigned int)is_signed_type(u64));

	ret = trace_seq_printf(s, "\tfield: local_t commit;\t"
			       "offset:%u;\tsize:%u;\tsigned:%u;\n",
			       (unsigned int)offsetof(typeof(field), commit),
			       (unsigned int)sizeof(field.commit),
			       (unsigned int)is_signed_type(long));

	ret = trace_seq_printf(s, "\tfield: int overwrite;\t"
			       "offset:%u;\tsize:%u;\tsigned:%u;\n",
			       (unsigned int)offsetof(typeof(field), commit),
			       1,
			       (unsigned int)is_signed_type(long));

	ret = trace_seq_printf(s, "\tfield: char data;\t"
			       "offset:%u;\tsize:%u;\tsigned:%u;\n",
			       (unsigned int)offsetof(typeof(field), data),
			       (unsigned int)BUF_PAGE_SIZE,
			       (unsigned int)is_signed_type(char));

	return ret;
}

struct rb_irq_work {
	struct irq_work			work;
	wait_queue_head_t		waiters;
	bool				waiters_pending;
};

/*
                                                               
 */
struct ring_buffer_per_cpu {
	int				cpu;
	atomic_t			record_disabled;
	struct ring_buffer		*buffer;
	raw_spinlock_t			reader_lock;	/*                   */
	arch_spinlock_t			lock;
	struct lock_class_key		lock_key;
	unsigned int			nr_pages;
	struct list_head		*pages;
	struct buffer_page		*head_page;	/*                */
	struct buffer_page		*tail_page;	/*               */
	struct buffer_page		*commit_page;	/*                 */
	struct buffer_page		*reader_page;
	unsigned long			lost_events;
	unsigned long			last_overrun;
	local_t				entries_bytes;
	local_t				entries;
	local_t				overrun;
	local_t				commit_overrun;
	local_t				dropped_events;
	local_t				committing;
	local_t				commits;
	unsigned long			read;
	unsigned long			read_bytes;
	u64				write_stamp;
	u64				read_stamp;
	/*                                                        */
	int				nr_pages_to_update;
	struct list_head		new_pages; /*                  */
	struct work_struct		update_pages_work;
	struct completion		update_done;

	struct rb_irq_work		irq_work;
};

struct ring_buffer {
	unsigned			flags;
	int				cpus;
	atomic_t			record_disabled;
	atomic_t			resize_disabled;
	cpumask_var_t			cpumask;

	struct lock_class_key		*reader_lock_key;

	struct mutex			mutex;

	struct ring_buffer_per_cpu	**buffers;

#ifdef CONFIG_HOTPLUG_CPU
	struct notifier_block		cpu_notify;
#endif
	u64				(*clock)(void);

	struct rb_irq_work		irq_work;
};

struct ring_buffer_iter {
	struct ring_buffer_per_cpu	*cpu_buffer;
	unsigned long			head;
	struct buffer_page		*head_page;
	struct buffer_page		*cache_reader_page;
	unsigned long			cache_read;
	u64				read_stamp;
};

/*
                                                                   
  
                                                                      
                             
 */
static void rb_wake_up_waiters(struct irq_work *work)
{
	struct rb_irq_work *rbwork = container_of(work, struct rb_irq_work, work);

	wake_up_all(&rbwork->waiters);
}

/* 
                                                       
                             
                                  
  
                                                                     
                                                                  
                                                              
 */
int ring_buffer_wait(struct ring_buffer *buffer, int cpu)
{
	struct ring_buffer_per_cpu *cpu_buffer;
	DEFINE_WAIT(wait);
	struct rb_irq_work *work;

	/*
                                                           
                                                         
                                         
  */
	if (cpu == RING_BUFFER_ALL_CPUS)
		work = &buffer->irq_work;
	else {
		if (!cpumask_test_cpu(cpu, buffer->cpumask))
			return -ENODEV;
		cpu_buffer = buffer->buffers[cpu];
		work = &cpu_buffer->irq_work;
	}


	prepare_to_wait(&work->waiters, &wait, TASK_INTERRUPTIBLE);

	/*
                                                    
                                              
                                                      
                                                     
                   
   
                                                     
                                                     
                                                    
                                                          
                                                     
                   
   
                                                     
                                                           
                                                    
                                                       
                                                          
  */
	work->waiters_pending = true;

	if ((cpu == RING_BUFFER_ALL_CPUS && ring_buffer_empty(buffer)) ||
	    (cpu != RING_BUFFER_ALL_CPUS && ring_buffer_empty_cpu(buffer, cpu)))
		schedule();

	finish_wait(&work->waiters, &wait);
	return 0;
}

/* 
                                               
                             
                                  
                             
                                   
  
                                                                     
                                                                  
                                                              
  
                                                             
                  
 */
int ring_buffer_poll_wait(struct ring_buffer *buffer, int cpu,
			  struct file *filp, poll_table *poll_table)
{
	struct ring_buffer_per_cpu *cpu_buffer;
	struct rb_irq_work *work;

	if (cpu == RING_BUFFER_ALL_CPUS)
		work = &buffer->irq_work;
	else {
		if (!cpumask_test_cpu(cpu, buffer->cpumask))
			return -EINVAL;

		cpu_buffer = buffer->buffers[cpu];
		work = &cpu_buffer->irq_work;
	}

	poll_wait(filp, &work->waiters, poll_table);
	work->waiters_pending = true;
	/*
	 * There's a tight race between setting the waiters_pending and
	 * checking if the ring buffer is empty.  Once the waiters_pending bit
	 * is set, the next event will wake the task up, but we can get stuck
	 * if there's only a single event in.
	 *
	 * FIXME: Ideally, we need a memory barrier on the writer side as well,
	 * but adding a memory barrier to all events will cause too much of a
	 * performance hit in the fast path.  We only need a memory barrier when
	 * the buffer goes from empty to having content.  But as this race is
	 * extremely small, and it's not a problem if another event comes in, we
	 * will fix it later.
	 */
	smp_mb();

	if ((cpu == RING_BUFFER_ALL_CPUS && !ring_buffer_empty(buffer)) ||
	    (cpu != RING_BUFFER_ALL_CPUS && !ring_buffer_empty_cpu(buffer, cpu)))
		return POLLIN | POLLRDNORM;
	return 0;
}

/*                                                         */
#define RB_WARN_ON(b, cond)						\
	({								\
		int _____ret = unlikely(cond);				\
		if (_____ret) {						\
			if (__same_type(*(b), struct ring_buffer_per_cpu)) { \
				struct ring_buffer_per_cpu *__b =	\
					(void *)b;			\
				atomic_inc(&__b->buffer->record_disabled); \
			} else						\
				atomic_inc(&b->record_disabled);	\
			WARN_ON(1);					\
		}							\
		_____ret;						\
	})

/*                                                                */
#define DEBUG_SHIFT 0

static inline u64 rb_time_stamp(struct ring_buffer *buffer)
{
	/*                                                    */
	return buffer->clock() << DEBUG_SHIFT;
}

u64 ring_buffer_time_stamp(struct ring_buffer *buffer, int cpu)
{
	u64 time;

	preempt_disable_notrace();
	time = rb_time_stamp(buffer);
	preempt_enable_no_resched_notrace();

	return time;
}
EXPORT_SYMBOL_GPL(ring_buffer_time_stamp);

void ring_buffer_normalize_time_stamp(struct ring_buffer *buffer,
				      int cpu, u64 *ts)
{
	/*                                                       */
	*ts >>= DEBUG_SHIFT;
}
EXPORT_SYMBOL_GPL(ring_buffer_normalize_time_stamp);

/*
                                                       
                                                           
                                                          
                     
  
                                                              
                                                              
                                                           
                                                                    
                                                      
  
                         
  
                                                                
                                                              
                                                               
                                                                
                                                             
                                                                
  
                                                                 
                                                          
  
                                                                
                                                                
                                            
  
                                                                
                                                              
                  
  
                                                     
                                                      
                                                   
                                                   
                                                   
  
                                                                    
  
                                      
                                      
                                      
                                      
                                    
                                    
                                    
                                    
                       
  
                                          
                           
                             
                           
  
                                                      
  
                                                            
                                                            
                                                              
                                                               
                                                                 
          
  
                                                             
                                                                
                  
 */

#define RB_PAGE_NORMAL		0UL
#define RB_PAGE_HEAD		1UL
#define RB_PAGE_UPDATE		2UL


#define RB_FLAG_MASK		3UL

/*                                    */
#define RB_PAGE_MOVED		4UL

/*
                                
 */
static struct list_head *rb_list_head(struct list_head *list)
{
	unsigned long val = (unsigned long)list;

	return (struct list_head *)(val & ~RB_FLAG_MASK);
}

/*
                                                            
  
                                                            
                                                         
                                                           
                              
 */
static inline int
rb_is_head_page(struct ring_buffer_per_cpu *cpu_buffer,
		struct buffer_page *page, struct list_head *list)
{
	unsigned long val;

	val = (unsigned long)list->next;

	if ((val & ~RB_FLAG_MASK) != (unsigned long)&page->list)
		return RB_PAGE_MOVED;

	return val & RB_FLAG_MASK;
}

/*
                    
  
                                                          
                                                          
                           
 */
static int rb_is_reader_page(struct buffer_page *page)
{
	struct list_head *list = page->list.prev;

	return rb_list_head(list->next) != &page->list;
}

/*
                                                                
 */
static void rb_set_list_to_head(struct ring_buffer_per_cpu *cpu_buffer,
				struct list_head *list)
{
	unsigned long *ptr;

	ptr = (unsigned long *)&list->next;
	*ptr |= RB_PAGE_HEAD;
	*ptr &= ~RB_PAGE_UPDATE;
}

/*
                                            
 */
static void rb_head_page_activate(struct ring_buffer_per_cpu *cpu_buffer)
{
	struct buffer_page *head;

	head = cpu_buffer->head_page;
	if (!head)
		return;

	/*
                                                        
  */
	rb_set_list_to_head(cpu_buffer, head->list.prev);
}

static void rb_list_head_clear(struct list_head *list)
{
	unsigned long *ptr = (unsigned long *)&list->next;

	*ptr &= ~RB_FLAG_MASK;
}

/*
                                                                
 */
static void
rb_head_page_deactivate(struct ring_buffer_per_cpu *cpu_buffer)
{
	struct list_head *hd;

	/*                                                         */
	rb_list_head_clear(cpu_buffer->pages);

	list_for_each(hd, cpu_buffer->pages)
		rb_list_head_clear(hd);
}

static int rb_head_page_set(struct ring_buffer_per_cpu *cpu_buffer,
			    struct buffer_page *head,
			    struct buffer_page *prev,
			    int old_flag, int new_flag)
{
	struct list_head *list;
	unsigned long val = (unsigned long)&head->list;
	unsigned long ret;

	list = &prev->list;

	val &= ~RB_FLAG_MASK;

	ret = cmpxchg((unsigned long *)&list->next,
		      val | old_flag, val | new_flag);

	/*                                   */
	if ((ret & ~RB_FLAG_MASK) != val)
		return RB_PAGE_MOVED;

	return ret & RB_FLAG_MASK;
}

static int rb_head_page_set_update(struct ring_buffer_per_cpu *cpu_buffer,
				   struct buffer_page *head,
				   struct buffer_page *prev,
				   int old_flag)
{
	return rb_head_page_set(cpu_buffer, head, prev,
				old_flag, RB_PAGE_UPDATE);
}

static int rb_head_page_set_head(struct ring_buffer_per_cpu *cpu_buffer,
				 struct buffer_page *head,
				 struct buffer_page *prev,
				 int old_flag)
{
	return rb_head_page_set(cpu_buffer, head, prev,
				old_flag, RB_PAGE_HEAD);
}

static int rb_head_page_set_normal(struct ring_buffer_per_cpu *cpu_buffer,
				   struct buffer_page *head,
				   struct buffer_page *prev,
				   int old_flag)
{
	return rb_head_page_set(cpu_buffer, head, prev,
				old_flag, RB_PAGE_NORMAL);
}

static inline void rb_inc_page(struct ring_buffer_per_cpu *cpu_buffer,
			       struct buffer_page **bpage)
{
	struct list_head *p = rb_list_head((*bpage)->list.next);

	*bpage = list_entry(p, struct buffer_page, list);
}

static struct buffer_page *
rb_set_head_page(struct ring_buffer_per_cpu *cpu_buffer)
{
	struct buffer_page *head;
	struct buffer_page *page;
	struct list_head *list;
	int i;

	if (RB_WARN_ON(cpu_buffer, !cpu_buffer->head_page))
		return NULL;

	/*              */
	list = cpu_buffer->pages;
	if (RB_WARN_ON(cpu_buffer, rb_list_head(list->prev->next) != list))
		return NULL;

	page = head = cpu_buffer->head_page;
	/*
                                                          
                                              
                                                      
                                          
  */
	for (i = 0; i < 3; i++) {
		do {
			if (rb_is_head_page(cpu_buffer, page, page->list.prev)) {
				cpu_buffer->head_page = page;
				return page;
			}
			rb_inc_page(cpu_buffer, &page);
		} while (page != head);
	}

	RB_WARN_ON(cpu_buffer, 1);

	return NULL;
}

static int rb_head_page_replace(struct buffer_page *old,
				struct buffer_page *new)
{
	unsigned long *ptr = (unsigned long *)&old->list.prev->next;
	unsigned long val;
	unsigned long ret;

	val = *ptr & ~RB_FLAG_MASK;
	val |= RB_PAGE_HEAD;

	ret = cmpxchg(ptr, val, (unsigned long)&new->list);

	return ret == val;
}

/*
                                                   
  
                                                       
 */
static int rb_tail_page_update(struct ring_buffer_per_cpu *cpu_buffer,
			       struct buffer_page *tail_page,
			       struct buffer_page *next_page)
{
	struct buffer_page *old_tail;
	unsigned long old_entries;
	unsigned long old_write;
	int ret = 0;

	/*
                                                
   
                                                       
                                                          
                                                          
   
                                                       
  */
	old_write = local_add_return(RB_WRITE_INTCNT, &next_page->write);
	old_entries = local_add_return(RB_WRITE_INTCNT, &next_page->entries);

	/*
                                                             
                                     
  */
	barrier();

	/*
                                                       
                                                 
            
  */
	if (tail_page == cpu_buffer->tail_page) {
		/*                        */
		unsigned long val = old_write & ~RB_WRITE_MASK;
		unsigned long eval = old_entries & ~RB_WRITE_MASK;

		/*
                                               
                                                 
                              
    
                                                               
                                                          
                                                           
                                                       
   */
		(void)local_cmpxchg(&next_page->write, old_write, val);
		(void)local_cmpxchg(&next_page->entries, old_entries, eval);

		/*
                                                               
                                                              
                                                  
   */
		local_set(&next_page->page->commit, 0);

		old_tail = cmpxchg(&cpu_buffer->tail_page,
				   tail_page, next_page);

		if (old_tail == tail_page)
			ret = 1;
	}

	return ret;
}

static int rb_check_bpage(struct ring_buffer_per_cpu *cpu_buffer,
			  struct buffer_page *bpage)
{
	unsigned long val = (unsigned long)bpage;

	if (RB_WARN_ON(cpu_buffer, val & RB_FLAG_MASK))
		return 1;

	return 0;
}

/* 
                                                                       
 */
static int rb_check_list(struct ring_buffer_per_cpu *cpu_buffer,
			 struct list_head *list)
{
	if (RB_WARN_ON(cpu_buffer, rb_list_head(list->prev) != list->prev))
		return 1;
	if (RB_WARN_ON(cpu_buffer, rb_list_head(list->next) != list->next))
		return 1;
	return 0;
}

/* 
                                                
                                             
  
                                                                    
                  
 */
static int rb_check_pages(struct ring_buffer_per_cpu *cpu_buffer)
{
	struct list_head *head = cpu_buffer->pages;
	struct buffer_page *bpage, *tmp;

	/*                                  */
	if (cpu_buffer->head_page)
		rb_set_head_page(cpu_buffer);

	rb_head_page_deactivate(cpu_buffer);

	if (RB_WARN_ON(cpu_buffer, head->next->prev != head))
		return -1;
	if (RB_WARN_ON(cpu_buffer, head->prev->next != head))
		return -1;

	if (rb_check_list(cpu_buffer, head))
		return -1;

	list_for_each_entry_safe(bpage, tmp, head, list) {
		if (RB_WARN_ON(cpu_buffer,
			       bpage->list.next->prev != &bpage->list))
			return -1;
		if (RB_WARN_ON(cpu_buffer,
			       bpage->list.prev->next != &bpage->list))
			return -1;
		if (rb_check_list(cpu_buffer, &bpage->list))
			return -1;
	}

	rb_head_page_activate(cpu_buffer);

	return 0;
}

static int __rb_allocate_pages(int nr_pages, struct list_head *pages, int cpu)
{
	int i;
	struct buffer_page *bpage, *tmp;

	for (i = 0; i < nr_pages; i++) {
#if !defined (CONFIG_MTK_EXTMEM)
		struct page *page;
#endif
		/*
                                                            
                                                             
                      
   */
		bpage = kzalloc_node(ALIGN(sizeof(*bpage), cache_line_size()),
				    GFP_KERNEL | __GFP_NORETRY,
				    cpu_to_node(cpu));
		if (!bpage)
			goto free_pages;

		list_add(&bpage->list, pages);

#ifdef CONFIG_MTK_EXTMEM
		bpage->page = extmem_malloc_page_align(PAGE_SIZE);
		if(bpage->page == NULL) {
			pr_err("%s[%s] ext memory alloc failed!!!\n", __FILE__, __FUNCTION__);
    		goto free_pages;
    	}
#else
		page = alloc_pages_node(cpu_to_node(cpu),
					GFP_KERNEL | __GFP_NORETRY, 0);
		if (!page)
			goto free_pages;
		bpage->page = page_address(page);
#endif
		rb_init_page(bpage->page);
	}

	return 0;

free_pages:
	list_for_each_entry_safe(bpage, tmp, pages, list) {
		list_del_init(&bpage->list);
		free_buffer_page(bpage);
	}

	return -ENOMEM;
}

static int rb_allocate_pages(struct ring_buffer_per_cpu *cpu_buffer,
			     unsigned nr_pages)
{
	LIST_HEAD(pages);

	WARN_ON(!nr_pages);

	if (__rb_allocate_pages(nr_pages, &pages, cpu_buffer->cpu))
		return -ENOMEM;

	/*
                                                              
                                                                
                
  */
	cpu_buffer->pages = pages.next;
	list_del(&pages);

	cpu_buffer->nr_pages = nr_pages;

	rb_check_pages(cpu_buffer);

	return 0;
}

static struct ring_buffer_per_cpu *
rb_allocate_cpu_buffer(struct ring_buffer *buffer, int nr_pages, int cpu)
{
	struct ring_buffer_per_cpu *cpu_buffer;
	struct buffer_page *bpage;
#if !defined (CONFIG_MTK_EXTMEM)
	struct page *page;
#endif
	int ret;

	cpu_buffer = kzalloc_node(ALIGN(sizeof(*cpu_buffer), cache_line_size()),
				  GFP_KERNEL, cpu_to_node(cpu));
	if (!cpu_buffer)
		return NULL;

	cpu_buffer->cpu = cpu;
	cpu_buffer->buffer = buffer;
	raw_spin_lock_init(&cpu_buffer->reader_lock);
	lockdep_set_class(&cpu_buffer->reader_lock, buffer->reader_lock_key);
	cpu_buffer->lock = (arch_spinlock_t)__ARCH_SPIN_LOCK_UNLOCKED;
	INIT_WORK(&cpu_buffer->update_pages_work, update_pages_handler);
	init_completion(&cpu_buffer->update_done);
	init_irq_work(&cpu_buffer->irq_work.work, rb_wake_up_waiters);
	init_waitqueue_head(&cpu_buffer->irq_work.waiters);

	bpage = kzalloc_node(ALIGN(sizeof(*bpage), cache_line_size()),
			    GFP_KERNEL, cpu_to_node(cpu));
	if (!bpage)
		goto fail_free_buffer;

	rb_check_bpage(cpu_buffer, bpage);

	cpu_buffer->reader_page = bpage;

#ifdef CONFIG_MTK_EXTMEM
	bpage->page = extmem_malloc_page_align(PAGE_SIZE);
	if(bpage->page == NULL)
	    goto fail_free_reader;
#else
	page = alloc_pages_node(cpu_to_node(cpu), GFP_KERNEL, 0);
	if (!page)
		goto fail_free_reader;
	bpage->page = page_address(page);
#endif	
	rb_init_page(bpage->page);

	INIT_LIST_HEAD(&cpu_buffer->reader_page->list);
	INIT_LIST_HEAD(&cpu_buffer->new_pages);

	ret = rb_allocate_pages(cpu_buffer, nr_pages);
	if (ret < 0)
		goto fail_free_reader;

	cpu_buffer->head_page
		= list_entry(cpu_buffer->pages, struct buffer_page, list);
	cpu_buffer->tail_page = cpu_buffer->commit_page = cpu_buffer->head_page;

	rb_head_page_activate(cpu_buffer);

	return cpu_buffer;

 fail_free_reader:
	free_buffer_page(cpu_buffer->reader_page);

 fail_free_buffer:
	kfree(cpu_buffer);
	return NULL;
}

static void rb_free_cpu_buffer(struct ring_buffer_per_cpu *cpu_buffer)
{
	struct list_head *head = cpu_buffer->pages;
	struct buffer_page *bpage, *tmp;

	free_buffer_page(cpu_buffer->reader_page);

	rb_head_page_deactivate(cpu_buffer);

	if (head) {
		list_for_each_entry_safe(bpage, tmp, head, list) {
			list_del_init(&bpage->list);
			free_buffer_page(bpage);
		}
		bpage = list_entry(head, struct buffer_page, list);
		free_buffer_page(bpage);
	}

	kfree(cpu_buffer);
}

#ifdef CONFIG_HOTPLUG_CPU
static int rb_cpu_notify(struct notifier_block *self,
			 unsigned long action, void *hcpu);
#endif

/* 
                                                 
                                                   
                                                 
  
                                                                   
                                                                
                                                                  
                                         
 */
struct ring_buffer *__ring_buffer_alloc(unsigned long size, unsigned flags,
					struct lock_class_key *key)
{
	struct ring_buffer *buffer;
	int bsize;
	int cpu, nr_pages;

	/*                               */
	buffer = kzalloc(ALIGN(sizeof(*buffer), cache_line_size()),
			 GFP_KERNEL);
	if (!buffer)
		return NULL;

	if (!alloc_cpumask_var(&buffer->cpumask, GFP_KERNEL))
		goto fail_free_buffer;

	nr_pages = DIV_ROUND_UP(size, BUF_PAGE_SIZE);
	buffer->flags = flags;
	buffer->clock = trace_clock_local;
	buffer->reader_lock_key = key;

	init_irq_work(&buffer->irq_work.work, rb_wake_up_waiters);
	init_waitqueue_head(&buffer->irq_work.waiters);

	/*                         */
	if (nr_pages < 2)
		nr_pages = 2;

	/*
                                                               
                                                                 
                                                                
  */
#ifdef CONFIG_HOTPLUG_CPU
	get_online_cpus();
	cpumask_copy(buffer->cpumask, cpu_online_mask);
#else
	cpumask_copy(buffer->cpumask, cpu_possible_mask);
#endif
	buffer->cpus = nr_cpu_ids;

	bsize = sizeof(void *) * nr_cpu_ids;
	buffer->buffers = kzalloc(ALIGN(bsize, cache_line_size()),
				  GFP_KERNEL);
	if (!buffer->buffers)
		goto fail_free_cpumask;

	for_each_buffer_cpu(buffer, cpu) {
		buffer->buffers[cpu] =
			rb_allocate_cpu_buffer(buffer, nr_pages, cpu);
		if (!buffer->buffers[cpu])
			goto fail_free_buffers;
	}

#ifdef CONFIG_HOTPLUG_CPU
	buffer->cpu_notify.notifier_call = rb_cpu_notify;
	buffer->cpu_notify.priority = 0;
	register_cpu_notifier(&buffer->cpu_notify);
#endif

	put_online_cpus();
	mutex_init(&buffer->mutex);

	return buffer;

 fail_free_buffers:
	for_each_buffer_cpu(buffer, cpu) {
		if (buffer->buffers[cpu])
			rb_free_cpu_buffer(buffer->buffers[cpu]);
	}
	kfree(buffer->buffers);

 fail_free_cpumask:
	free_cpumask_var(buffer->cpumask);
	put_online_cpus();

 fail_free_buffer:
	kfree(buffer);
	return NULL;
}
EXPORT_SYMBOL_GPL(__ring_buffer_alloc);

/* 
                                         
                               
 */
void
ring_buffer_free(struct ring_buffer *buffer)
{
	int cpu;

	get_online_cpus();

#ifdef CONFIG_HOTPLUG_CPU
	unregister_cpu_notifier(&buffer->cpu_notify);
#endif

	for_each_buffer_cpu(buffer, cpu)
		rb_free_cpu_buffer(buffer->buffers[cpu]);

	put_online_cpus();

	kfree(buffer->buffers);
	free_cpumask_var(buffer->cpumask);

	kfree(buffer);
}
EXPORT_SYMBOL_GPL(ring_buffer_free);

void ring_buffer_set_clock(struct ring_buffer *buffer,
			   u64 (*clock)(void))
{
	buffer->clock = clock;
}

static void rb_reset_cpu(struct ring_buffer_per_cpu *cpu_buffer);

static inline unsigned long rb_page_entries(struct buffer_page *bpage)
{
	return local_read(&bpage->entries) & RB_WRITE_MASK;
}

static inline unsigned long rb_page_write(struct buffer_page *bpage)
{
	return local_read(&bpage->write) & RB_WRITE_MASK;
}

static int
rb_remove_pages(struct ring_buffer_per_cpu *cpu_buffer, unsigned int nr_pages)
{
	struct list_head *tail_page, *to_remove, *next_page;
	struct buffer_page *to_remove_page, *tmp_iter_page;
	struct buffer_page *last_page, *first_page;
	unsigned int nr_removed;
	unsigned long head_bit;
	int page_entries;

	head_bit = 0;

	raw_spin_lock_irq(&cpu_buffer->reader_lock);
	atomic_inc(&cpu_buffer->record_disabled);
	/*
                                                                    
                                                                    
                                                                      
                                                                       
                                                                    
                                    
                                                                      
  */
	tail_page = &cpu_buffer->tail_page->list;

	/*
                                                              
                        
  */
	if (cpu_buffer->tail_page == cpu_buffer->reader_page)
		tail_page = rb_list_head(tail_page->next);
	to_remove = tail_page;

	/*                          */
	first_page = list_entry(rb_list_head(to_remove->next),
				struct buffer_page, list);

	for (nr_removed = 0; nr_removed < nr_pages; nr_removed++) {
		to_remove = rb_list_head(to_remove)->next;
		head_bit |= (unsigned long)to_remove & RB_PAGE_HEAD;
	}

	next_page = rb_list_head(to_remove)->next;

	/*
                                                            
                                                           
             
  */
	tail_page->next = (struct list_head *)((unsigned long)next_page |
						head_bit);
	next_page = rb_list_head(next_page);
	next_page->prev = tail_page;

	/*                                                           */
	cpu_buffer->pages = next_page;

	/*                  */
	if (head_bit)
		cpu_buffer->head_page = list_entry(next_page,
						struct buffer_page, list);

	/*
                                                             
              
  */
	cpu_buffer->read = 0;

	/*                                                           */
	atomic_dec(&cpu_buffer->record_disabled);
	raw_spin_unlock_irq(&cpu_buffer->reader_lock);

	RB_WARN_ON(cpu_buffer, list_empty(cpu_buffer->pages));

	/*                            */
	last_page = list_entry(rb_list_head(to_remove), struct buffer_page,
				list);
	tmp_iter_page = first_page;

	do {
		to_remove_page = tmp_iter_page;
		rb_inc_page(cpu_buffer, &tmp_iter_page);

		/*                     */
		page_entries = rb_page_entries(to_remove_page);
		if (page_entries) {
			/*
                                                      
                                                     
                                              
                                                       
    */
			local_add(page_entries, &cpu_buffer->overrun);
			local_sub(BUF_PAGE_SIZE, &cpu_buffer->entries_bytes);
		}

		/*
                                                               
                                         
   */
		free_buffer_page(to_remove_page);
		nr_removed--;

	} while (to_remove_page != last_page);

	RB_WARN_ON(cpu_buffer, nr_removed);

	return nr_removed == 0;
}

static int
rb_insert_pages(struct ring_buffer_per_cpu *cpu_buffer)
{
	struct list_head *pages = &cpu_buffer->new_pages;
	int retries, success;

	raw_spin_lock_irq(&cpu_buffer->reader_lock);
	/*
                                                                       
                                                                   
                                     
                                                               
                                                                     
                                           
                                                                    
                               
                                                                
   
                                                                      
             
  */
	retries = 10;
	success = 0;
	while (retries--) {
		struct list_head *head_page, *prev_page, *r;
		struct list_head *last_page, *first_page;
		struct list_head *head_page_with_bit;

		head_page = &rb_set_head_page(cpu_buffer)->list;
		if (!head_page)
			break;
		prev_page = head_page->prev;

		first_page = pages->next;
		last_page  = pages->prev;

		head_page_with_bit = (struct list_head *)
				     ((unsigned long)head_page | RB_PAGE_HEAD);

		last_page->next = head_page_with_bit;
		first_page->prev = prev_page;

		r = cmpxchg(&prev_page->next, head_page_with_bit, first_page);

		if (r == head_page_with_bit) {
			/*
                                                        
                                                     
                                     
    */
			head_page->prev = last_page;
			success = 1;
			break;
		}
	}

	if (success)
		INIT_LIST_HEAD(pages);
	/*
                                                                  
           
  */
	RB_WARN_ON(cpu_buffer, !success);
	raw_spin_unlock_irq(&cpu_buffer->reader_lock);

	/*                                     */
	if (!success) {
		struct buffer_page *bpage, *tmp;
		list_for_each_entry_safe(bpage, tmp, &cpu_buffer->new_pages,
					 list) {
			list_del_init(&bpage->list);
			free_buffer_page(bpage);
		}
	}
	return success;
}

static void rb_update_pages(struct ring_buffer_per_cpu *cpu_buffer)
{
	int success;

	if (cpu_buffer->nr_pages_to_update > 0)
		success = rb_insert_pages(cpu_buffer);
	else
		success = rb_remove_pages(cpu_buffer,
					-cpu_buffer->nr_pages_to_update);

	if (success)
		cpu_buffer->nr_pages += cpu_buffer->nr_pages_to_update;
}

static void update_pages_handler(struct work_struct *work)
{
	struct ring_buffer_per_cpu *cpu_buffer = container_of(work,
			struct ring_buffer_per_cpu, update_pages_work);
	rb_update_pages(cpu_buffer);
	complete(&cpu_buffer->update_done);
}

/* 
                                              
                                 
                       
  
                                     
  
                                           
 */
int ring_buffer_resize(struct ring_buffer *buffer, unsigned long size,
			int cpu_id)
{
	struct ring_buffer_per_cpu *cpu_buffer;
	unsigned nr_pages;
	int cpu, err = 0;

	/*
                                                     
  */
	if (!buffer)
		return size;

	/*                                       */
	if (cpu_id != RING_BUFFER_ALL_CPUS &&
	    !cpumask_test_cpu(cpu_id, buffer->cpumask))
		return size;

	size = DIV_ROUND_UP(size, BUF_PAGE_SIZE);
	size *= BUF_PAGE_SIZE;

	/*                                */
	if (size < BUF_PAGE_SIZE * 2)
		size = BUF_PAGE_SIZE * 2;

	nr_pages = DIV_ROUND_UP(size, BUF_PAGE_SIZE);

	/*
                                                               
                                                                    
                 
  */
	if (atomic_read(&buffer->resize_disabled))
		return -EBUSY;

	/*                                                   */
	mutex_lock(&buffer->mutex);

	if (cpu_id == RING_BUFFER_ALL_CPUS) {
		/*                               */
		for_each_buffer_cpu(buffer, cpu) {
			cpu_buffer = buffer->buffers[cpu];

			cpu_buffer->nr_pages_to_update = nr_pages -
							cpu_buffer->nr_pages;
			/*
                                                        
    */
			if (cpu_buffer->nr_pages_to_update <= 0)
				continue;
			/*
                                                  
                                        
    */
			INIT_LIST_HEAD(&cpu_buffer->new_pages);
			if (__rb_allocate_pages(cpu_buffer->nr_pages_to_update,
						&cpu_buffer->new_pages, cpu)) {
				/*                                 */
				err = -ENOMEM;
				goto out_err;
			}
		}

		get_online_cpus();
		/*
                                            
                                                              
                                                             
   */
		for_each_buffer_cpu(buffer, cpu) {
			cpu_buffer = buffer->buffers[cpu];
			if (!cpu_buffer->nr_pages_to_update)
				continue;

			/*                                                       */
			preempt_disable();
			if (cpu == smp_processor_id() || !cpu_online(cpu)) {
				rb_update_pages(cpu_buffer);
				cpu_buffer->nr_pages_to_update = 0;
			} else {
				/*
                                                        
                     
     */
				preempt_enable();
				schedule_work_on(cpu,
						&cpu_buffer->update_pages_work);
				preempt_disable();
			}
			preempt_enable();
		}

		/*                                      */
		for_each_buffer_cpu(buffer, cpu) {
			cpu_buffer = buffer->buffers[cpu];
			if (!cpu_buffer->nr_pages_to_update)
				continue;

			if (cpu_online(cpu))
				wait_for_completion(&cpu_buffer->update_done);
			cpu_buffer->nr_pages_to_update = 0;
		}

		put_online_cpus();
	} else {
		/*                                          */
		if (!cpumask_test_cpu(cpu_id, buffer->cpumask))
			goto out;

		cpu_buffer = buffer->buffers[cpu_id];

		if (nr_pages == cpu_buffer->nr_pages)
			goto out;

		cpu_buffer->nr_pages_to_update = nr_pages -
						cpu_buffer->nr_pages;

		INIT_LIST_HEAD(&cpu_buffer->new_pages);
		if (cpu_buffer->nr_pages_to_update > 0 &&
			__rb_allocate_pages(cpu_buffer->nr_pages_to_update,
					    &cpu_buffer->new_pages, cpu_id)) {
			err = -ENOMEM;
			goto out_err;
		}

		get_online_cpus();

		preempt_disable();
		/*                                                       */
		if (cpu_id == smp_processor_id() || !cpu_online(cpu_id))
			rb_update_pages(cpu_buffer);
		else {
			/*
                                                       
                    
    */
			preempt_enable();
			schedule_work_on(cpu_id,
					 &cpu_buffer->update_pages_work);
			wait_for_completion(&cpu_buffer->update_done);
			preempt_disable();
		}
		preempt_enable();

		cpu_buffer->nr_pages_to_update = 0;
		put_online_cpus();
	}

 out:
	/*
                                                          
                                                              
                                                              
                                                           
                                   
  */
	if (atomic_read(&buffer->record_disabled)) {
		atomic_inc(&buffer->record_disabled);
		/*
                                                           
                                                             
                                                  
                                        
   */
		synchronize_sched();
		for_each_buffer_cpu(buffer, cpu) {
			cpu_buffer = buffer->buffers[cpu];
			rb_check_pages(cpu_buffer);
		}
		atomic_dec(&buffer->record_disabled);
	}

	mutex_unlock(&buffer->mutex);
	return size;

 out_err:
	for_each_buffer_cpu(buffer, cpu) {
		struct buffer_page *bpage, *tmp;

		cpu_buffer = buffer->buffers[cpu];
		cpu_buffer->nr_pages_to_update = 0;

		if (list_empty(&cpu_buffer->new_pages))
			continue;

		list_for_each_entry_safe(bpage, tmp, &cpu_buffer->new_pages,
					list) {
			list_del_init(&bpage->list);
			free_buffer_page(bpage);
		}
	}
	mutex_unlock(&buffer->mutex);
	return err;
}
EXPORT_SYMBOL_GPL(ring_buffer_resize);

void ring_buffer_change_overwrite(struct ring_buffer *buffer, int val)
{
	mutex_lock(&buffer->mutex);
	if (val)
		buffer->flags |= RB_FL_OVERWRITE;
	else
		buffer->flags &= ~RB_FL_OVERWRITE;
	mutex_unlock(&buffer->mutex);
}
EXPORT_SYMBOL_GPL(ring_buffer_change_overwrite);

static inline void *
__rb_data_page_index(struct buffer_data_page *bpage, unsigned index)
{
	return bpage->data + index;
}

static inline void *__rb_page_index(struct buffer_page *bpage, unsigned index)
{
	return bpage->page->data + index;
}

static inline struct ring_buffer_event *
rb_reader_event(struct ring_buffer_per_cpu *cpu_buffer)
{
	return __rb_page_index(cpu_buffer->reader_page,
			       cpu_buffer->reader_page->read);
}

static inline struct ring_buffer_event *
rb_iter_head_event(struct ring_buffer_iter *iter)
{
	return __rb_page_index(iter->head_page, iter->head);
}

static inline unsigned rb_page_commit(struct buffer_page *bpage)
{
	return local_read(&bpage->page->commit);
}

/*                                               */
static inline unsigned rb_page_size(struct buffer_page *bpage)
{
	return rb_page_commit(bpage);
}

static inline unsigned
rb_commit_index(struct ring_buffer_per_cpu *cpu_buffer)
{
	return rb_page_commit(cpu_buffer->commit_page);
}

static inline unsigned
rb_event_index(struct ring_buffer_event *event)
{
	unsigned long addr = (unsigned long)event;

	return (addr & ~PAGE_MASK) - BUF_PAGE_HDR_SIZE;
}

static inline int
rb_event_is_commit(struct ring_buffer_per_cpu *cpu_buffer,
		   struct ring_buffer_event *event)
{
	unsigned long addr = (unsigned long)event;
	unsigned long index;

	index = rb_event_index(event);
	addr &= PAGE_MASK;

	return cpu_buffer->commit_page->page == (void *)addr &&
		rb_commit_index(cpu_buffer) == index;
}

static void
rb_set_commit_to_write(struct ring_buffer_per_cpu *cpu_buffer)
{
	unsigned long max_count;

	/*
                                                      
                                                  
                                                           
                                                     
                                                      
                                  
  */
 again:
	max_count = cpu_buffer->nr_pages * 100;

	while (cpu_buffer->commit_page != cpu_buffer->tail_page) {
		if (RB_WARN_ON(cpu_buffer, !(--max_count)))
			return;
		if (RB_WARN_ON(cpu_buffer,
			       rb_is_reader_page(cpu_buffer->tail_page)))
			return;
		local_set(&cpu_buffer->commit_page->page->commit,
			  rb_page_write(cpu_buffer->commit_page));
		rb_inc_page(cpu_buffer, &cpu_buffer->commit_page);
		cpu_buffer->write_stamp =
			cpu_buffer->commit_page->page->time_stamp;
		/*                                                  */
		barrier();
	}
	while (rb_commit_index(cpu_buffer) !=
	       rb_page_write(cpu_buffer->commit_page)) {

		local_set(&cpu_buffer->commit_page->page->commit,
			  rb_page_write(cpu_buffer->commit_page));
		RB_WARN_ON(cpu_buffer,
			   local_read(&cpu_buffer->commit_page->page->commit) &
			   ~RB_WRITE_MASK);
		barrier();
	}

	/*                                 */
	barrier();

	/*
                                                           
                                                          
                                                 
  */
	if (unlikely(cpu_buffer->commit_page != cpu_buffer->tail_page))
		goto again;
}

static void rb_reset_reader_page(struct ring_buffer_per_cpu *cpu_buffer)
{
	cpu_buffer->read_stamp = cpu_buffer->reader_page->page->time_stamp;
	cpu_buffer->reader_page->read = 0;
}

static void rb_inc_iter(struct ring_buffer_iter *iter)
{
	struct ring_buffer_per_cpu *cpu_buffer = iter->cpu_buffer;

	/*
                                                               
                                                       
                                                      
                                     
  */
	if (iter->head_page == cpu_buffer->reader_page)
		iter->head_page = rb_set_head_page(cpu_buffer);
	else
		rb_inc_page(cpu_buffer, &iter->head_page);

	iter->read_stamp = iter->head_page->page->time_stamp;
	iter->head = 0;
}

/*                          */
static noinline struct ring_buffer_event *
rb_add_time_stamp(struct ring_buffer_event *event, u64 delta)
{
	event->type_len = RINGBUF_TYPE_TIME_EXTEND;

	/*                                  */
	if (rb_event_index(event)) {
		event->time_delta = delta & TS_MASK;
		event->array[0] = delta >> TS_SHIFT;
	} else {
		/*                    */
		event->time_delta = 0;
		event->array[0] = 0;
	}

	return skip_time_extend(event);
}

/* 
                                               
                             
                           
                                                          
  
                                                           
                                                         
                                                         
              
 */
static void
rb_update_event(struct ring_buffer_per_cpu *cpu_buffer,
		struct ring_buffer_event *event, unsigned length,
		int add_timestamp, u64 delta)
{
	/*                                     */
	if (unlikely(!rb_event_is_commit(cpu_buffer, event)))
		delta = 0;

	/*
                                          
                                               
  */
	if (unlikely(add_timestamp)) {
		event = rb_add_time_stamp(event, delta);
		length -= RB_LEN_TIME_EXTEND;
		delta = 0;
	}

	event->time_delta = delta;
	length -= RB_EVNT_HDR_SIZE;
	if (length > RB_MAX_SMALL_DATA || RB_FORCE_8BYTE_ALIGNMENT) {
		event->type_len = 0;
		event->array[0] = length;
	} else
		event->type_len = DIV_ROUND_UP(length, RB_ALIGNMENT);
}

/*
                                                 
  
                            
                          
                       
 */
static int
rb_handle_head_page(struct ring_buffer_per_cpu *cpu_buffer,
		    struct buffer_page *tail_page,
		    struct buffer_page *next_page)
{
	struct buffer_page *new_head;
	int entries;
	int type;
	int ret;

	entries = rb_page_entries(next_page);

	/*
                                                   
                                                
                                                    
  */
	type = rb_head_page_set_update(cpu_buffer, next_page, tail_page,
				       RB_PAGE_HEAD);

	/*
                            
                                                  
                                           
                                               
                             
                                                    
                                                 
                            
  */

	switch (type) {
	case RB_PAGE_HEAD:
		/*
                                        
                                       
                  
   */
		local_add(entries, &cpu_buffer->overrun);
		local_sub(BUF_PAGE_SIZE, &cpu_buffer->entries_bytes);

		/*
                                                    
               
   */

		/*                  */
		break;

	case RB_PAGE_UPDATE:
		/*
                                            
                                       
   */
		break;
	case RB_PAGE_NORMAL:
		/*
                                           
                               
                        
   */
		return 1;
	case RB_PAGE_MOVED:
		/*
                                              
                               
               
   */
		return 1;
	default:
		RB_WARN_ON(cpu_buffer, 1); /*        */
		return -1;
	}

	/*
                                                 
                                                 
                                                
                                              
                    
   
                                              
                                               
                                               
                                              
                                           
                                           
  */
	new_head = next_page;
	rb_inc_page(cpu_buffer, &new_head);

	ret = rb_head_page_set_head(cpu_buffer, new_head, next_page,
				    RB_PAGE_NORMAL);

	/*
                      
                                                      
                                
                                   
                                                         
                                         
  */
	switch (ret) {
	case RB_PAGE_HEAD:
	case RB_PAGE_NORMAL:
		/*    */
		break;
	default:
		RB_WARN_ON(cpu_buffer, 1);
		return -1;
	}

	/*
                                             
                                                 
                                              
                                                 
                             
   
                                                  
                                   
  */
	if (ret == RB_PAGE_NORMAL) {
		/*
                                                    
                          
   */
		if (cpu_buffer->tail_page != tail_page &&
		    cpu_buffer->tail_page != next_page)
			rb_head_page_set_normal(cpu_buffer, new_head,
						next_page,
						RB_PAGE_HEAD);
	}

	/*
                                                   
                                                      
                                              
  */
	if (type == RB_PAGE_HEAD) {
		ret = rb_head_page_set_normal(cpu_buffer, next_page,
					      tail_page,
					      RB_PAGE_UPDATE);
		if (RB_WARN_ON(cpu_buffer,
			       ret != RB_PAGE_UPDATE))
			return -1;
	}

	return 0;
}

static unsigned rb_calculate_event_length(unsigned length)
{
	struct ring_buffer_event event; /*                            */

	/*                                  */
	if (!length)
		length = 1;

	if (length > RB_MAX_SMALL_DATA || RB_FORCE_8BYTE_ALIGNMENT)
		length += sizeof(event.array[0]);

	length += RB_EVNT_HDR_SIZE;
	length = ALIGN(length, RB_ARCH_ALIGNMENT);

	return length;
}

static inline void
rb_reset_tail(struct ring_buffer_per_cpu *cpu_buffer,
	      struct buffer_page *tail_page,
	      unsigned long tail, unsigned long length)
{
	struct ring_buffer_event *event;

	/*
                                                 
                                             
  */
	if (tail >= BUF_PAGE_SIZE) {
		/*
                                               
                                             
                                   
   */
		if (tail == BUF_PAGE_SIZE)
			tail_page->real_end = 0;

		local_sub(length, &tail_page->write);
		return;
	}

	event = __rb_page_index(tail_page, tail);
	kmemcheck_annotate_bitfield(event, bitfield);

	/*                           */
	local_add(BUF_PAGE_SIZE - tail, &cpu_buffer->entries_bytes);

	/*
                                              
                                                     
            
  */
	tail_page->real_end = tail;

	/*
                                                       
                                                    
                                                        
                    
                                                      
                                      
   
                                                          
                   
  */
	if (tail > (BUF_PAGE_SIZE - RB_EVNT_MIN_SIZE)) {
		/*                        */

		/*                                        */
		rb_event_set_padding(event);

		/*                                            */
		local_sub(length, &tail_page->write);
		return;
	}

	/*                          */
	event->array[0] = (BUF_PAGE_SIZE - tail) - RB_EVNT_HDR_SIZE;
	event->type_len = RINGBUF_TYPE_PADDING;
	/*                             */
	event->time_delta = 1;

	/*                            */
	length = (tail + length) - BUF_PAGE_SIZE;
	local_sub(length, &tail_page->write);
}

/*
                                                     
 */
static noinline struct ring_buffer_event *
rb_move_tail(struct ring_buffer_per_cpu *cpu_buffer,
	     unsigned long length, unsigned long tail,
	     struct buffer_page *tail_page, u64 ts)
{
	struct buffer_page *commit_page = cpu_buffer->commit_page;
	struct ring_buffer *buffer = cpu_buffer->buffer;
	struct buffer_page *next_page;
	int ret;

	next_page = tail_page;

	rb_inc_page(cpu_buffer, &next_page);

	/*
                                                           
                                                    
             
  */
	if (unlikely(next_page == commit_page)) {
		local_inc(&cpu_buffer->commit_overrun);
		goto out_reset;
	}

	/*
                                 
   
                                                       
                                                     
                              
   
                                                         
                                          
   
                                                         
                                                      
                
  */
	if (rb_is_head_page(cpu_buffer, next_page, &tail_page->list)) {

		/*
                                                  
                          
   */
		if (!rb_is_reader_page(cpu_buffer->commit_page)) {
			/*
                                      
                                   
    */
			if (!(buffer->flags & RB_FL_OVERWRITE)) {
				local_inc(&cpu_buffer->dropped_events);
				goto out_reset;
			}

			ret = rb_handle_head_page(cpu_buffer,
						  tail_page,
						  next_page);
			if (ret < 0)
				goto out_reset;
			if (ret)
				goto out_again;
		} else {
			/*
                                         
                                              
                                             
                                           
                                            
     
                                               
                                      
    */
			if (unlikely((cpu_buffer->commit_page !=
				      cpu_buffer->tail_page) &&
				     (cpu_buffer->commit_page ==
				      cpu_buffer->reader_page))) {
				local_inc(&cpu_buffer->commit_overrun);
				goto out_reset;
			}
		}
	}

	ret = rb_tail_page_update(cpu_buffer, tail_page, next_page);
	if (ret) {
		/*
                                               
                               
   */
		ts = rb_time_stamp(buffer);
		next_page->page->time_stamp = ts;
	}

 out_again:

	rb_reset_tail(cpu_buffer, tail_page, tail, length);

	/*                                   */
	return ERR_PTR(-EAGAIN);

 out_reset:
	/*             */
	rb_reset_tail(cpu_buffer, tail_page, tail, length);

	return NULL;
}

static struct ring_buffer_event *
__rb_reserve_next(struct ring_buffer_per_cpu *cpu_buffer,
		  unsigned long length, u64 ts,
		  u64 delta, int add_timestamp)
{
	struct buffer_page *tail_page;
	struct ring_buffer_event *event;
	unsigned long tail, write;

	/*
                                                        
                                                         
                                              
  */
	if (unlikely(add_timestamp))
		length += RB_LEN_TIME_EXTEND;

	tail_page = cpu_buffer->tail_page;
	write = local_add_return(length, &tail_page->write);

	/*                                          */
	write &= RB_WRITE_MASK;
	tail = write - length;

	/*
                                                                 
                                 
  */
	if (!tail)
		delta = 0;

	/*                                                 */
	if (unlikely(write > BUF_PAGE_SIZE))
		return rb_move_tail(cpu_buffer, length, tail,
				    tail_page, ts);

	/*                                     */

	event = __rb_page_index(tail_page, tail);
	kmemcheck_annotate_bitfield(event, bitfield);
	rb_update_event(cpu_buffer, event, length, add_timestamp, delta);

	local_inc(&tail_page->entries);

	/*
                                                        
                  
  */
	if (!tail)
		tail_page->page->time_stamp = ts;

	/*                               */
	local_add(length, &cpu_buffer->entries_bytes);

	return event;
}

static inline int
rb_try_to_discard(struct ring_buffer_per_cpu *cpu_buffer,
		  struct ring_buffer_event *event)
{
	unsigned long new_index, old_index;
	struct buffer_page *bpage;
	unsigned long index;
	unsigned long addr;

	new_index = rb_event_index(event);
	old_index = new_index + rb_event_ts_length(event);
	addr = (unsigned long)event;
	addr &= PAGE_MASK;

	bpage = cpu_buffer->tail_page;

	if (bpage->page == (void *)addr && rb_page_write(bpage) == old_index) {
		unsigned long write_mask =
			local_read(&bpage->write) & ~RB_WRITE_MASK;
		unsigned long event_length = rb_event_length(event);
		/*
                                                  
                                                 
                                             
                                                  
   */
		old_index += write_mask;
		new_index += write_mask;
		index = local_cmpxchg(&bpage->write, old_index, new_index);
		if (index == old_index) {
			/*                 */
			local_sub(event_length, &cpu_buffer->entries_bytes);
			return 1;
		}
	}

	/*                   */
	return 0;
}

static void rb_start_commit(struct ring_buffer_per_cpu *cpu_buffer)
{
	local_inc(&cpu_buffer->committing);
	local_inc(&cpu_buffer->commits);
}

static inline void rb_end_commit(struct ring_buffer_per_cpu *cpu_buffer)
{
	unsigned long commits;

	if (RB_WARN_ON(cpu_buffer,
		       !local_read(&cpu_buffer->committing)))
		return;

 again:
	commits = local_read(&cpu_buffer->commits);
	/*                             */
	barrier();
	if (local_read(&cpu_buffer->committing) == 1)
		rb_set_commit_to_write(cpu_buffer);

	local_dec(&cpu_buffer->committing);

	/*                             */
	barrier();

	/*
                                                        
                                                       
                       
  */
	if (unlikely(local_read(&cpu_buffer->commits) != commits) &&
	    !local_read(&cpu_buffer->committing)) {
		local_inc(&cpu_buffer->committing);
		goto again;
	}
}

static struct ring_buffer_event *
rb_reserve_next_event(struct ring_buffer *buffer,
		      struct ring_buffer_per_cpu *cpu_buffer,
		      unsigned long length)
{
	struct ring_buffer_event *event;
	u64 ts, delta;
	int nr_loops = 0;
	int add_timestamp;
	u64 diff;

	rb_start_commit(cpu_buffer);

#ifdef CONFIG_RING_BUFFER_ALLOW_SWAP
	/*
                                                         
                                                      
                                                       
                                              
  */
	barrier();
	if (unlikely(ACCESS_ONCE(cpu_buffer->buffer) != buffer)) {
		local_dec(&cpu_buffer->committing);
		local_dec(&cpu_buffer->commits);
		return NULL;
	}
#endif

	length = rb_calculate_event_length(length);
 again:
	add_timestamp = 0;
	delta = 0;

	/*
                                                           
                                                         
                                                         
                                                            
                                                          
                                     
         
  */
	if (RB_WARN_ON(cpu_buffer, ++nr_loops > 1000))
		goto out_fail;

	ts = rb_time_stamp(cpu_buffer->buffer);
	diff = ts - cpu_buffer->write_stamp;

	/*                                        */
	barrier();

	/*                                          */
	if (likely(ts >= cpu_buffer->write_stamp)) {
		delta = diff;
		if (unlikely(test_time_stamp(delta))) {
			int local_clock_stable = 1;
#ifdef CONFIG_HAVE_UNSTABLE_SCHED_CLOCK
			local_clock_stable = sched_clock_stable;
#endif
			WARN_ONCE(delta > (1ULL << 59),
				  KERN_WARNING "Delta way too big! %llu ts=%llu write stamp = %llu\n%s",
				  (unsigned long long)delta,
				  (unsigned long long)ts,
				  (unsigned long long)cpu_buffer->write_stamp,
				  local_clock_stable ? "" :
				  "If you just came from a suspend/resume,\n"
				  "please switch to the trace global clock:\n"
				  "  echo global > /sys/kernel/debug/tracing/trace_clock\n");
			add_timestamp = 1;
		}
	}

	event = __rb_reserve_next(cpu_buffer, length, ts,
				  delta, add_timestamp);
	if (unlikely(PTR_ERR(event) == -EAGAIN))
		goto again;

	if (!event)
		goto out_fail;

	return event;

 out_fail:
	rb_end_commit(cpu_buffer);
	return NULL;
}

#ifdef CONFIG_TRACING

/*
                                                                 
                                                            
                                                          
                                                            
                                                            
                                                             
                                                                  
                          
  
                        
                        
                            
                            
  
                                                            
                                                          
           
  
                                                           
                                                                   
             
  
                                                                
                                                   
  
           
                 
                                       
  
                   
                                       
  
                                                            
                                                           
                       
 */
static DEFINE_PER_CPU(unsigned int, current_context);

static __always_inline int trace_recursive_lock(void)
{
	unsigned int val = this_cpu_read(current_context);
	int bit;

	if (in_interrupt()) {
		if (in_nmi())
			bit = 0;
		else if (in_irq())
			bit = 1;
		else
			bit = 2;
	} else
		bit = 3;

	if (unlikely(val & (1 << bit)))
		return 1;

	val |= (1 << bit);
	this_cpu_write(current_context, val);

	return 0;
}

static __always_inline void trace_recursive_unlock(void)
{
	unsigned int val = this_cpu_read(current_context);

	val--;
	val &= this_cpu_read(current_context);
	this_cpu_write(current_context, val);
}

#else

#define trace_recursive_lock()		(0)
#define trace_recursive_unlock()	do { } while (0)

#endif

/* 
                                                          
                                           
                                                                      
  
                                                                   
                                                                     
                                                      
  
                                                                    
                                        
  
                                                                          
                                                                  
 */
struct ring_buffer_event *
ring_buffer_lock_reserve(struct ring_buffer *buffer, unsigned long length)
{
	struct ring_buffer_per_cpu *cpu_buffer;
	struct ring_buffer_event *event;
	int cpu;

	if (ring_buffer_flags != RB_BUFFERS_ON)
		return NULL;

	/*                                                      */
	preempt_disable_notrace();

	if (atomic_read(&buffer->record_disabled))
		goto out_nocheck;

	if (trace_recursive_lock())
		goto out_nocheck;

	cpu = raw_smp_processor_id();

	if (!cpumask_test_cpu(cpu, buffer->cpumask))
		goto out;

	cpu_buffer = buffer->buffers[cpu];

	if (atomic_read(&cpu_buffer->record_disabled))
		goto out;

	if (length > BUF_MAX_DATA_SIZE)
		goto out;

	event = rb_reserve_next_event(buffer, cpu_buffer, length);
	if (!event)
		goto out;

	return event;

 out:
	trace_recursive_unlock();

 out_nocheck:
	preempt_enable_notrace();
	return NULL;
}
EXPORT_SYMBOL_GPL(ring_buffer_lock_reserve);

static void
rb_update_write_stamp(struct ring_buffer_per_cpu *cpu_buffer,
		      struct ring_buffer_event *event)
{
	u64 delta;

	/*
                                                   
               
  */
	if (rb_event_is_commit(cpu_buffer, event)) {
		/*
                                           
                                                    
   */
		if (!rb_event_index(event))
			cpu_buffer->write_stamp =
				cpu_buffer->commit_page->page->time_stamp;
		else if (event->type_len == RINGBUF_TYPE_TIME_EXTEND) {
			delta = event->array[0];
			delta <<= TS_SHIFT;
			delta += event->time_delta;
			cpu_buffer->write_stamp += delta;
		} else
			cpu_buffer->write_stamp += event->time_delta;
	}
}

static void rb_commit(struct ring_buffer_per_cpu *cpu_buffer,
		      struct ring_buffer_event *event)
{
	local_inc(&cpu_buffer->entries);
	rb_update_write_stamp(cpu_buffer, event);
	rb_end_commit(cpu_buffer);
}

static __always_inline void
rb_wakeups(struct ring_buffer *buffer, struct ring_buffer_per_cpu *cpu_buffer)
{
	if (buffer->irq_work.waiters_pending) {
		buffer->irq_work.waiters_pending = false;
		/*                                                    */
		irq_work_queue(&buffer->irq_work.work);
	}

	if (cpu_buffer->irq_work.waiters_pending) {
		cpu_buffer->irq_work.waiters_pending = false;
		/*                                                    */
		irq_work_queue(&cpu_buffer->irq_work.work);
	}
}

/* 
                                                
                                   
                                       
  
                                                                         
  
                                                
 */
int ring_buffer_unlock_commit(struct ring_buffer *buffer,
			      struct ring_buffer_event *event)
{
	struct ring_buffer_per_cpu *cpu_buffer;
	int cpu = raw_smp_processor_id();

	cpu_buffer = buffer->buffers[cpu];

	rb_commit(cpu_buffer, event);

	rb_wakeups(buffer, cpu_buffer);

	trace_recursive_unlock();

	preempt_enable_notrace();

	return 0;
}
EXPORT_SYMBOL_GPL(ring_buffer_unlock_commit);

static inline void rb_event_discard(struct ring_buffer_event *event)
{
	if (event->type_len == RINGBUF_TYPE_TIME_EXTEND)
		event = skip_time_extend(event);

	/*                                                          */
	event->array[0] = rb_event_data_length(event) - RB_EVNT_HDR_SIZE;
	event->type_len = RINGBUF_TYPE_PADDING;
	/*                             */
	if (!event->time_delta)
		event->time_delta = 1;
}

/*
                                                         
                                                          
                                                                  
               
 */
static inline void
rb_decrement_entry(struct ring_buffer_per_cpu *cpu_buffer,
		   struct ring_buffer_event *event)
{
	unsigned long addr = (unsigned long)event;
	struct buffer_page *bpage = cpu_buffer->commit_page;
	struct buffer_page *start;

	addr &= PAGE_MASK;

	/*                          */
	if (likely(bpage->page == (void *)addr)) {
		local_dec(&bpage->entries);
		return;
	}

	/*
                                                        
                                                          
  */
	rb_inc_page(cpu_buffer, &bpage);
	start = bpage;
	do {
		if (bpage->page == (void *)addr) {
			local_dec(&bpage->entries);
			return;
		}
		rb_inc_page(cpu_buffer, &bpage);
	} while (bpage != start);

	/*                                  */
	RB_WARN_ON(cpu_buffer, 1);
}

/* 
                                                                            
                           
                                         
  
                                                                     
                                                                  
                                              
  
                                                                        
                                                                
                                                 
  
                                                                   
                                           
  
                                                                       
             
 */
void ring_buffer_discard_commit(struct ring_buffer *buffer,
				struct ring_buffer_event *event)
{
	struct ring_buffer_per_cpu *cpu_buffer;
	int cpu;

	/*                                   */
	rb_event_discard(event);

	cpu = smp_processor_id();
	cpu_buffer = buffer->buffers[cpu];

	/*
                                                      
                                                     
                      
  */
	RB_WARN_ON(buffer, !local_read(&cpu_buffer->committing));

	rb_decrement_entry(cpu_buffer, event);
	if (rb_try_to_discard(cpu_buffer, event))
		goto out;

	/*
                                                    
                                    
  */
	rb_update_write_stamp(cpu_buffer, event);
 out:
	rb_end_commit(cpu_buffer);

	trace_recursive_unlock();

	preempt_enable_notrace();

}
EXPORT_SYMBOL_GPL(ring_buffer_discard_commit);

/* 
                                                                 
                                        
                                                                             
                                          
  
                                                                         
                                                                        
                                              
  
                                                                            
                                                               
 */
int ring_buffer_write(struct ring_buffer *buffer,
		      unsigned long length,
		      void *data)
{
	struct ring_buffer_per_cpu *cpu_buffer;
	struct ring_buffer_event *event;
	void *body;
	int ret = -EBUSY;
	int cpu;

	if (ring_buffer_flags != RB_BUFFERS_ON)
		return -EBUSY;

	preempt_disable_notrace();

	if (atomic_read(&buffer->record_disabled))
		goto out;

	cpu = raw_smp_processor_id();

	if (!cpumask_test_cpu(cpu, buffer->cpumask))
		goto out;

	cpu_buffer = buffer->buffers[cpu];

	if (atomic_read(&cpu_buffer->record_disabled))
		goto out;

	if (length > BUF_MAX_DATA_SIZE)
		goto out;

	event = rb_reserve_next_event(buffer, cpu_buffer, length);
	if (!event)
		goto out;

	body = rb_event_data(event);

	memcpy(body, data, length);

	rb_commit(cpu_buffer, event);

	rb_wakeups(buffer, cpu_buffer);

	ret = 0;
 out:
	preempt_enable_notrace();

	return ret;
}
EXPORT_SYMBOL_GPL(ring_buffer_write);

static int rb_per_cpu_empty(struct ring_buffer_per_cpu *cpu_buffer)
{
	struct buffer_page *reader = cpu_buffer->reader_page;
	struct buffer_page *head = rb_set_head_page(cpu_buffer);
	struct buffer_page *commit = cpu_buffer->commit_page;

	/*                                     */
	if (unlikely(!head))
		return 1;

	return reader->read == rb_page_commit(reader) &&
		(commit == reader ||
		 (commit == head &&
		  head->read == rb_page_commit(commit)));
}

/* 
                                                               
                                              
  
                                                               
                                                      
  
                                                         
 */
void ring_buffer_record_disable(struct ring_buffer *buffer)
{
	atomic_inc(&buffer->record_disabled);
}
EXPORT_SYMBOL_GPL(ring_buffer_record_disable);

/* 
                                                          
                                            
  
                                                               
                                                           
 */
void ring_buffer_record_enable(struct ring_buffer *buffer)
{
	atomic_dec(&buffer->record_disabled);
}
EXPORT_SYMBOL_GPL(ring_buffer_record_enable);

/* 
                                                           
                                              
  
                                                               
                                                      
  
                                                         
                                                                 
                                  
 */
void ring_buffer_record_off(struct ring_buffer *buffer)
{
	unsigned int rd;
	unsigned int new_rd;

	do {
		rd = atomic_read(&buffer->record_disabled);
		new_rd = rd | RB_BUFFER_OFF;
	} while (atomic_cmpxchg(&buffer->record_disabled, rd, new_rd) != rd);
}
EXPORT_SYMBOL_GPL(ring_buffer_record_off);

/* 
                                                         
                                               
  
                                                             
                            
  
                                                        
                                                                
                                   
 */
void ring_buffer_record_on(struct ring_buffer *buffer)
{
	unsigned int rd;
	unsigned int new_rd;

	do {
		rd = atomic_read(&buffer->record_disabled);
		new_rd = rd & ~RB_BUFFER_OFF;
	} while (atomic_cmpxchg(&buffer->record_disabled, rd, new_rd) != rd);
}
EXPORT_SYMBOL_GPL(ring_buffer_record_on);

/* 
                                                                      
                                                      
  
                                                                        
 */
int ring_buffer_record_is_on(struct ring_buffer *buffer)
{
	return !atomic_read(&buffer->record_disabled);
}

/* 
                                                                       
                                              
                               
  
                                                               
                                                      
  
                                                         
 */
void ring_buffer_record_disable_cpu(struct ring_buffer *buffer, int cpu)
{
	struct ring_buffer_per_cpu *cpu_buffer;

	if (!cpumask_test_cpu(cpu, buffer->cpumask))
		return;

	cpu_buffer = buffer->buffers[cpu];
	atomic_inc(&cpu_buffer->record_disabled);
}
EXPORT_SYMBOL_GPL(ring_buffer_record_disable_cpu);

/* 
                                                              
                                            
                           
  
                                                               
                                                           
 */
void ring_buffer_record_enable_cpu(struct ring_buffer *buffer, int cpu)
{
	struct ring_buffer_per_cpu *cpu_buffer;

	if (!cpumask_test_cpu(cpu, buffer->cpumask))
		return;

	cpu_buffer = buffer->buffers[cpu];
	atomic_dec(&cpu_buffer->record_disabled);
}
EXPORT_SYMBOL_GPL(ring_buffer_record_enable_cpu);

/*
                                                              
                                                            
                                                          
                                 
 */
static inline unsigned long
rb_num_of_entries(struct ring_buffer_per_cpu *cpu_buffer)
{
	return local_read(&cpu_buffer->entries) -
		(local_read(&cpu_buffer->overrun) + cpu_buffer->read);
}

/* 
                                                                               
                           
                                         
 */
u64 ring_buffer_oldest_event_ts(struct ring_buffer *buffer, int cpu)
{
	unsigned long flags;
	struct ring_buffer_per_cpu *cpu_buffer;
	struct buffer_page *bpage;
	u64 ret = 0;

	if (!cpumask_test_cpu(cpu, buffer->cpumask))
		return 0;

	cpu_buffer = buffer->buffers[cpu];
	raw_spin_lock_irqsave(&cpu_buffer->reader_lock, flags);
	/*
                                                                     
        
  */
	if (cpu_buffer->tail_page == cpu_buffer->reader_page)
		bpage = cpu_buffer->reader_page;
	else
		bpage = rb_set_head_page(cpu_buffer);
	if (bpage)
		ret = bpage->page->time_stamp;
	raw_spin_unlock_irqrestore(&cpu_buffer->reader_lock, flags);

	return ret;
}
EXPORT_SYMBOL_GPL(ring_buffer_oldest_event_ts);

/* 
                                                                           
                           
                                         
 */
unsigned long ring_buffer_bytes_cpu(struct ring_buffer *buffer, int cpu)
{
	struct ring_buffer_per_cpu *cpu_buffer;
	unsigned long ret;

	if (!cpumask_test_cpu(cpu, buffer->cpumask))
		return 0;

	cpu_buffer = buffer->buffers[cpu];
	ret = local_read(&cpu_buffer->entries_bytes) - cpu_buffer->read_bytes;

	return ret;
}
EXPORT_SYMBOL_GPL(ring_buffer_bytes_cpu);

/* 
                                                                      
                           
                                                    
 */
unsigned long ring_buffer_entries_cpu(struct ring_buffer *buffer, int cpu)
{
	struct ring_buffer_per_cpu *cpu_buffer;

	if (!cpumask_test_cpu(cpu, buffer->cpumask))
		return 0;

	cpu_buffer = buffer->buffers[cpu];

	return rb_num_of_entries(cpu_buffer);
}
EXPORT_SYMBOL_GPL(ring_buffer_entries_cpu);

/* 
                                                                          
                                                          
                           
                                                              
 */
unsigned long ring_buffer_overrun_cpu(struct ring_buffer *buffer, int cpu)
{
	struct ring_buffer_per_cpu *cpu_buffer;
	unsigned long ret;

	if (!cpumask_test_cpu(cpu, buffer->cpumask))
		return 0;

	cpu_buffer = buffer->buffers[cpu];
	ret = local_read(&cpu_buffer->overrun);

	return ret;
}
EXPORT_SYMBOL_GPL(ring_buffer_overrun_cpu);

/* 
                                                                        
                                                                                
                                             
                           
                                                              
 */
unsigned long
ring_buffer_commit_overrun_cpu(struct ring_buffer *buffer, int cpu)
{
	struct ring_buffer_per_cpu *cpu_buffer;
	unsigned long ret;

	if (!cpumask_test_cpu(cpu, buffer->cpumask))
		return 0;

	cpu_buffer = buffer->buffers[cpu];
	ret = local_read(&cpu_buffer->commit_overrun);

	return ret;
}
EXPORT_SYMBOL_GPL(ring_buffer_commit_overrun_cpu);

/* 
                                                                              
                                                               
                           
                                                              
 */
unsigned long
ring_buffer_dropped_events_cpu(struct ring_buffer *buffer, int cpu)
{
	struct ring_buffer_per_cpu *cpu_buffer;
	unsigned long ret;

	if (!cpumask_test_cpu(cpu, buffer->cpumask))
		return 0;

	cpu_buffer = buffer->buffers[cpu];
	ret = local_read(&cpu_buffer->dropped_events);

	return ret;
}
EXPORT_SYMBOL_GPL(ring_buffer_dropped_events_cpu);

/* 
                                                                           
                           
                                                            
 */
unsigned long
ring_buffer_read_events_cpu(struct ring_buffer *buffer, int cpu)
{
	struct ring_buffer_per_cpu *cpu_buffer;

	if (!cpumask_test_cpu(cpu, buffer->cpumask))
		return 0;

	cpu_buffer = buffer->buffers[cpu];
	return cpu_buffer->read;
}
EXPORT_SYMBOL_GPL(ring_buffer_read_events_cpu);

/* 
                                                              
                           
  
                                                         
                    
 */
unsigned long ring_buffer_entries(struct ring_buffer *buffer)
{
	struct ring_buffer_per_cpu *cpu_buffer;
	unsigned long entries = 0;
	int cpu;

	/*                                                       */
	for_each_buffer_cpu(buffer, cpu) {
		cpu_buffer = buffer->buffers[cpu];
		entries += rb_num_of_entries(cpu_buffer);
	}

	return entries;
}
EXPORT_SYMBOL_GPL(ring_buffer_entries);

/* 
                                                              
                           
  
                                                          
                    
 */
unsigned long ring_buffer_overruns(struct ring_buffer *buffer)
{
	struct ring_buffer_per_cpu *cpu_buffer;
	unsigned long overruns = 0;
	int cpu;

	/*                                                       */
	for_each_buffer_cpu(buffer, cpu) {
		cpu_buffer = buffer->buffers[cpu];
		overruns += local_read(&cpu_buffer->overrun);
	}

	return overruns;
}
EXPORT_SYMBOL_GPL(ring_buffer_overruns);

static void rb_iter_reset(struct ring_buffer_iter *iter)
{
	struct ring_buffer_per_cpu *cpu_buffer = iter->cpu_buffer;

 	/* Iterator usage is expected to have record disabled */
	iter->head_page = cpu_buffer->reader_page;
	iter->head = cpu_buffer->reader_page->read;

	iter->cache_reader_page = iter->head_page;
	iter->cache_read = cpu_buffer->read;

	if (iter->head)
		iter->read_stamp = cpu_buffer->read_stamp;
	else
		iter->read_stamp = iter->head_page->page->time_stamp;
}

/* 
                                             
                               
  
                                                                
         
 */
void ring_buffer_iter_reset(struct ring_buffer_iter *iter)
{
	struct ring_buffer_per_cpu *cpu_buffer;
	unsigned long flags;

	if (!iter)
		return;

	cpu_buffer = iter->cpu_buffer;

	raw_spin_lock_irqsave(&cpu_buffer->reader_lock, flags);
	rb_iter_reset(iter);
	raw_spin_unlock_irqrestore(&cpu_buffer->reader_lock, flags);
}
EXPORT_SYMBOL_GPL(ring_buffer_iter_reset);

/* 
                                                                    
                               
 */
int ring_buffer_iter_empty(struct ring_buffer_iter *iter)
{
	struct ring_buffer_per_cpu *cpu_buffer;

	cpu_buffer = iter->cpu_buffer;

	return iter->head_page == cpu_buffer->commit_page &&
		iter->head == rb_commit_index(cpu_buffer);
}
EXPORT_SYMBOL_GPL(ring_buffer_iter_empty);

static void
rb_update_read_stamp(struct ring_buffer_per_cpu *cpu_buffer,
		     struct ring_buffer_event *event)
{
	u64 delta;

	switch (event->type_len) {
	case RINGBUF_TYPE_PADDING:
		return;

	case RINGBUF_TYPE_TIME_EXTEND:
		delta = event->array[0];
		delta <<= TS_SHIFT;
		delta += event->time_delta;
		cpu_buffer->read_stamp += delta;
		return;

	case RINGBUF_TYPE_TIME_STAMP:
		/*                        */
		return;

	case RINGBUF_TYPE_DATA:
		cpu_buffer->read_stamp += event->time_delta;
		return;

	default:
		BUG();
	}
	return;
}

static void
rb_update_iter_read_stamp(struct ring_buffer_iter *iter,
			  struct ring_buffer_event *event)
{
	u64 delta;

	switch (event->type_len) {
	case RINGBUF_TYPE_PADDING:
		return;

	case RINGBUF_TYPE_TIME_EXTEND:
		delta = event->array[0];
		delta <<= TS_SHIFT;
		delta += event->time_delta;
		iter->read_stamp += delta;
		return;

	case RINGBUF_TYPE_TIME_STAMP:
		/*                        */
		return;

	case RINGBUF_TYPE_DATA:
		iter->read_stamp += event->time_delta;
		return;

	default:
		BUG();
	}
	return;
}

static struct buffer_page *
rb_get_reader_page(struct ring_buffer_per_cpu *cpu_buffer)
{
	struct buffer_page *reader = NULL;
	unsigned long overwrite;
	unsigned long flags;
	int nr_loops = 0;
	int ret;

	local_irq_save(flags);
	arch_spin_lock(&cpu_buffer->lock);

 again:
	/*
                                                         
                                                        
                                                             
                                               
  */
	if (RB_WARN_ON(cpu_buffer, ++nr_loops > 3)) {
		reader = NULL;
		goto out;
	}

	reader = cpu_buffer->reader_page;

	/*                                           */
	if (cpu_buffer->reader_page->read < rb_page_size(reader))
		goto out;

	/*                                                     */
	if (RB_WARN_ON(cpu_buffer,
		       cpu_buffer->reader_page->read > rb_page_size(reader)))
		goto out;

	/*                                   */
	reader = NULL;
	if (cpu_buffer->commit_page == cpu_buffer->reader_page)
		goto out;

	/*                                                   */
	if (rb_num_of_entries(cpu_buffer) == 0)
		goto out;

	/*
                                       
  */
	local_set(&cpu_buffer->reader_page->write, 0);
	local_set(&cpu_buffer->reader_page->entries, 0);
	local_set(&cpu_buffer->reader_page->page->commit, 0);
	cpu_buffer->reader_page->real_end = 0;

 spin:
	/*
                                                               
  */
	reader = rb_set_head_page(cpu_buffer);
	if (!reader)
		goto out;
	cpu_buffer->reader_page->list.next = rb_list_head(reader->list.next);
	cpu_buffer->reader_page->list.prev = reader->list.prev;

	/*
                                                           
                                                              
                                                 
  */
	cpu_buffer->pages = reader->list.prev;

	/*                                                  */
	rb_set_list_to_head(cpu_buffer, &cpu_buffer->reader_page->list);

	/*
                                                                 
                                                       
                                                             
                                                        
                                                      
                                                             
                                          
  */
	smp_mb();
	overwrite = local_read(&(cpu_buffer->overrun));

	/*
                           
   
                                                     
                                                        
                                                      
                                                           
                                                    
                                             
  */

	ret = rb_head_page_replace(reader, cpu_buffer->reader_page);

	/*
                                                     
  */
	if (!ret)
		goto spin;

	/*
                                             
   
                                                        
  */
	rb_list_head(reader->list.next)->prev = &cpu_buffer->reader_page->list;
	rb_inc_page(cpu_buffer, &cpu_buffer->head_page);

	/*                                                */
	cpu_buffer->reader_page = reader;
	rb_reset_reader_page(cpu_buffer);

	if (overwrite != cpu_buffer->last_overrun) {
		cpu_buffer->lost_events = overwrite - cpu_buffer->last_overrun;
		cpu_buffer->last_overrun = overwrite;
	}

	goto again;

 out:
	arch_spin_unlock(&cpu_buffer->lock);
	local_irq_restore(flags);

	return reader;
}

static void rb_advance_reader(struct ring_buffer_per_cpu *cpu_buffer)
{
	struct ring_buffer_event *event;
	struct buffer_page *reader;
	unsigned length;

	reader = rb_get_reader_page(cpu_buffer);

	/*                                                         */
	if (RB_WARN_ON(cpu_buffer, !reader))
		return;

	event = rb_reader_event(cpu_buffer);

	if (event->type_len <= RINGBUF_TYPE_DATA_TYPE_LEN_MAX)
		cpu_buffer->read++;

	rb_update_read_stamp(cpu_buffer, event);

	length = rb_event_length(event);
	cpu_buffer->reader_page->read += length;
}

static void rb_advance_iter(struct ring_buffer_iter *iter)
{
	struct ring_buffer_per_cpu *cpu_buffer;
	struct ring_buffer_event *event;
	unsigned length;

	cpu_buffer = iter->cpu_buffer;

	/*
                                             
  */
	if (iter->head >= rb_page_size(iter->head_page)) {
		/*                                           */
		if (iter->head_page == cpu_buffer->commit_page)
			return;
		rb_inc_iter(iter);
		return;
	}

	event = rb_iter_head_event(iter);

	length = rb_event_length(event);

	/*
                                                             
                              
  */
	if (RB_WARN_ON(cpu_buffer,
		       (iter->head_page == cpu_buffer->commit_page) &&
		       (iter->head + length > rb_commit_index(cpu_buffer))))
		return;

	rb_update_iter_read_stamp(iter, event);

	iter->head += length;

	/*                               */
	if ((iter->head >= rb_page_size(iter->head_page)) &&
	    (iter->head_page != cpu_buffer->commit_page))
		rb_inc_iter(iter);
}

static int rb_lost_events(struct ring_buffer_per_cpu *cpu_buffer)
{
	return cpu_buffer->lost_events;
}

static struct ring_buffer_event *
rb_buffer_peek(struct ring_buffer_per_cpu *cpu_buffer, u64 *ts,
	       unsigned long *lost_events)
{
	struct ring_buffer_event *event;
	struct buffer_page *reader;
	int nr_loops = 0;

 again:
	/*
                                                
                                                             
                                        
                                                           
  */
	if (RB_WARN_ON(cpu_buffer, ++nr_loops > 2))
		return NULL;

	reader = rb_get_reader_page(cpu_buffer);
	if (!reader)
		return NULL;

	event = rb_reader_event(cpu_buffer);

	switch (event->type_len) {
	case RINGBUF_TYPE_PADDING:
		if (rb_null_event(event))
			RB_WARN_ON(cpu_buffer, 1);
		/*
                                                 
                                                   
                                                       
                                                    
                                                     
                                      
   */
		return event;

	case RINGBUF_TYPE_TIME_EXTEND:
		/*                              */
		rb_advance_reader(cpu_buffer);
		goto again;

	case RINGBUF_TYPE_TIME_STAMP:
		/*                        */
		rb_advance_reader(cpu_buffer);
		goto again;

	case RINGBUF_TYPE_DATA:
		if (ts) {
			*ts = cpu_buffer->read_stamp + event->time_delta;
			ring_buffer_normalize_time_stamp(cpu_buffer->buffer,
							 cpu_buffer->cpu, ts);
		}
		if (lost_events)
			*lost_events = rb_lost_events(cpu_buffer);
		return event;

	default:
		BUG();
	}

	return NULL;
}
EXPORT_SYMBOL_GPL(ring_buffer_peek);

static struct ring_buffer_event *
rb_iter_peek(struct ring_buffer_iter *iter, u64 *ts)
{
	struct ring_buffer *buffer;
	struct ring_buffer_per_cpu *cpu_buffer;
	struct ring_buffer_event *event;
	int nr_loops = 0;

	cpu_buffer = iter->cpu_buffer;
	buffer = cpu_buffer->buffer;

	/*
                                                  
                                                         
                                                   
  */
	if (unlikely(iter->cache_read != cpu_buffer->read ||
		     iter->cache_reader_page != cpu_buffer->reader_page))
		rb_iter_reset(iter);

 again:
	if (ring_buffer_iter_empty(iter))
		return NULL;

 	/*
	 * We repeat when a time extend is encountered or we hit
	 * the end of the page. Since the time extend is always attached
	 * to a data event, we should never loop more than three times.
	 * Once for going to next page, once on time extend, and
	 * finally once to get the event.
	 * (We never hit the following condition more than thrice).
 	 */
	if (RB_WARN_ON(cpu_buffer, ++nr_loops > 3))
		return NULL;

	if (rb_per_cpu_empty(cpu_buffer))
		return NULL;

	if (iter->head >= local_read(&iter->head_page->page->commit)) {
		rb_inc_iter(iter);
		goto again;
	}

	event = rb_iter_head_event(iter);

	switch (event->type_len) {
	case RINGBUF_TYPE_PADDING:
		if (rb_null_event(event)) {
			rb_inc_iter(iter);
			goto again;
		}
		rb_advance_iter(iter);
		return event;

	case RINGBUF_TYPE_TIME_EXTEND:
		/*                              */
		rb_advance_iter(iter);
		goto again;

	case RINGBUF_TYPE_TIME_STAMP:
		/*                        */
		rb_advance_iter(iter);
		goto again;

	case RINGBUF_TYPE_DATA:
		if (ts) {
			*ts = iter->read_stamp + event->time_delta;
			ring_buffer_normalize_time_stamp(buffer,
							 cpu_buffer->cpu, ts);
		}
		return event;

	default:
		BUG();
	}

	return NULL;
}
EXPORT_SYMBOL_GPL(ring_buffer_iter_peek);

static inline int rb_ok_to_lock(void)
{
	/*
                                                          
                                                           
                                                           
                                
  */
	if (likely(!in_nmi()))
		return 1;

	tracing_off_permanent();
	return 0;
}

/* 
                                                       
                                   
                           
                                            
                                                                      
  
                                                              
                        
 */
struct ring_buffer_event *
ring_buffer_peek(struct ring_buffer *buffer, int cpu, u64 *ts,
		 unsigned long *lost_events)
{
	struct ring_buffer_per_cpu *cpu_buffer = buffer->buffers[cpu];
	struct ring_buffer_event *event;
	unsigned long flags;
	int dolock;

	if (!cpumask_test_cpu(cpu, buffer->cpumask))
		return NULL;

	dolock = rb_ok_to_lock();
 again:
	local_irq_save(flags);
	if (dolock)
		raw_spin_lock(&cpu_buffer->reader_lock);
	event = rb_buffer_peek(cpu_buffer, ts, lost_events);
	if (event && event->type_len == RINGBUF_TYPE_PADDING)
		rb_advance_reader(cpu_buffer);
	if (dolock)
		raw_spin_unlock(&cpu_buffer->reader_lock);
	local_irq_restore(flags);

	if (event && event->type_len == RINGBUF_TYPE_PADDING)
		goto again;

	return event;
}

/* 
                                                            
                                  
                                            
  
                                                              
                              
 */
struct ring_buffer_event *
ring_buffer_iter_peek(struct ring_buffer_iter *iter, u64 *ts)
{
	struct ring_buffer_per_cpu *cpu_buffer = iter->cpu_buffer;
	struct ring_buffer_event *event;
	unsigned long flags;

 again:
	raw_spin_lock_irqsave(&cpu_buffer->reader_lock, flags);
	event = rb_iter_peek(iter, ts);
	raw_spin_unlock_irqrestore(&cpu_buffer->reader_lock, flags);

	if (event && event->type_len == RINGBUF_TYPE_PADDING)
		goto again;

	return event;
}

/* 
                                                       
                                                      
                                        
                                                       
                                                                      
  
                                                                         
                                                                        
                                                                  
 */
struct ring_buffer_event *
ring_buffer_consume(struct ring_buffer *buffer, int cpu, u64 *ts,
		    unsigned long *lost_events)
{
	struct ring_buffer_per_cpu *cpu_buffer;
	struct ring_buffer_event *event = NULL;
	unsigned long flags;
	int dolock;

	dolock = rb_ok_to_lock();

 again:
	/*                           */
	preempt_disable();

	if (!cpumask_test_cpu(cpu, buffer->cpumask))
		goto out;

	cpu_buffer = buffer->buffers[cpu];
	local_irq_save(flags);
	if (dolock)
		raw_spin_lock(&cpu_buffer->reader_lock);

	event = rb_buffer_peek(cpu_buffer, ts, lost_events);
	if (event) {
		cpu_buffer->lost_events = 0;
		rb_advance_reader(cpu_buffer);
	}

	if (dolock)
		raw_spin_unlock(&cpu_buffer->reader_lock);
	local_irq_restore(flags);

 out:
	preempt_enable();

	if (event && event->type_len == RINGBUF_TYPE_PADDING)
		goto again;

	return event;
}
EXPORT_SYMBOL_GPL(ring_buffer_consume);

/* 
                                                                            
                                        
                                       
  
                                                              
                                                             
                                                                   
  
                                                            
                                                                
            
  
                                                                  
                                                                  
                                                                    
            
  
                                                       
 */
struct ring_buffer_iter *
ring_buffer_read_prepare(struct ring_buffer *buffer, int cpu)
{
	struct ring_buffer_per_cpu *cpu_buffer;
	struct ring_buffer_iter *iter;

	if (!cpumask_test_cpu(cpu, buffer->cpumask))
		return NULL;

	iter = kmalloc(sizeof(*iter), GFP_KERNEL);
	if (!iter)
		return NULL;

	cpu_buffer = buffer->buffers[cpu];

	iter->cpu_buffer = cpu_buffer;

	atomic_inc(&buffer->resize_disabled);
	atomic_inc(&cpu_buffer->record_disabled);

	return iter;
}
EXPORT_SYMBOL_GPL(ring_buffer_read_prepare);

/* 
                                                                     
  
                                                                   
                                                                      
                                        
 */
void
ring_buffer_read_prepare_sync(void)
{
	synchronize_sched();
}
EXPORT_SYMBOL_GPL(ring_buffer_read_prepare_sync);

/* 
                                                                    
                                                           
  
                                                                 
                                                                 
                                                              
             
  
                                          
 */
void
ring_buffer_read_start(struct ring_buffer_iter *iter)
{
	struct ring_buffer_per_cpu *cpu_buffer;
	unsigned long flags;

	if (!iter)
		return;

	cpu_buffer = iter->cpu_buffer;

	raw_spin_lock_irqsave(&cpu_buffer->reader_lock, flags);
	arch_spin_lock(&cpu_buffer->lock);
	rb_iter_reset(iter);
	arch_spin_unlock(&cpu_buffer->lock);
	raw_spin_unlock_irqrestore(&cpu_buffer->reader_lock, flags);
}
EXPORT_SYMBOL_GPL(ring_buffer_read_start);

/* 
                                                                 
                                                     
  
                                                             
            
 */
void
ring_buffer_read_finish(struct ring_buffer_iter *iter)
{
	struct ring_buffer_per_cpu *cpu_buffer = iter->cpu_buffer;
	unsigned long flags;

	/*
                                                               
                                              
                                                          
                                                
  */
	raw_spin_lock_irqsave(&cpu_buffer->reader_lock, flags);
	rb_check_pages(cpu_buffer);
	raw_spin_unlock_irqrestore(&cpu_buffer->reader_lock, flags);

	atomic_dec(&cpu_buffer->record_disabled);
	atomic_dec(&cpu_buffer->buffer->resize_disabled);
	kfree(iter);
}
EXPORT_SYMBOL_GPL(ring_buffer_read_finish);

/* 
                                                                           
                                  
                                         
  
                                                                            
 */
struct ring_buffer_event *
ring_buffer_read(struct ring_buffer_iter *iter, u64 *ts)
{
	struct ring_buffer_event *event;
	struct ring_buffer_per_cpu *cpu_buffer = iter->cpu_buffer;
	unsigned long flags;

	raw_spin_lock_irqsave(&cpu_buffer->reader_lock, flags);
 again:
	event = rb_iter_peek(iter, ts);
	if (!event)
		goto out;

	if (event->type_len == RINGBUF_TYPE_PADDING)
		goto again;

	rb_advance_iter(iter);
 out:
	raw_spin_unlock_irqrestore(&cpu_buffer->reader_lock, flags);

	return event;
}
EXPORT_SYMBOL_GPL(ring_buffer_read);

/* 
                                                                   
                            
 */
unsigned long ring_buffer_size(struct ring_buffer *buffer, int cpu)
{
	/*
                                 
                                    
                                                                      
                                    
  */
	if (!cpumask_test_cpu(cpu, buffer->cpumask))
		return 0;

	return BUF_PAGE_SIZE * buffer->buffers[cpu]->nr_pages;
}
EXPORT_SYMBOL_GPL(ring_buffer_size);

static void
rb_reset_cpu(struct ring_buffer_per_cpu *cpu_buffer)
{
	rb_head_page_deactivate(cpu_buffer);

	cpu_buffer->head_page
		= list_entry(cpu_buffer->pages, struct buffer_page, list);
	local_set(&cpu_buffer->head_page->write, 0);
	local_set(&cpu_buffer->head_page->entries, 0);
	local_set(&cpu_buffer->head_page->page->commit, 0);

	cpu_buffer->head_page->read = 0;

	cpu_buffer->tail_page = cpu_buffer->head_page;
	cpu_buffer->commit_page = cpu_buffer->head_page;

	INIT_LIST_HEAD(&cpu_buffer->reader_page->list);
	INIT_LIST_HEAD(&cpu_buffer->new_pages);
	local_set(&cpu_buffer->reader_page->write, 0);
	local_set(&cpu_buffer->reader_page->entries, 0);
	local_set(&cpu_buffer->reader_page->page->commit, 0);
	cpu_buffer->reader_page->read = 0;

	local_set(&cpu_buffer->entries_bytes, 0);
	local_set(&cpu_buffer->overrun, 0);
	local_set(&cpu_buffer->commit_overrun, 0);
	local_set(&cpu_buffer->dropped_events, 0);
	local_set(&cpu_buffer->entries, 0);
	local_set(&cpu_buffer->committing, 0);
	local_set(&cpu_buffer->commits, 0);
	cpu_buffer->read = 0;
	cpu_buffer->read_bytes = 0;

	cpu_buffer->write_stamp = 0;
	cpu_buffer->read_stamp = 0;

	cpu_buffer->lost_events = 0;
	cpu_buffer->last_overrun = 0;

	rb_head_page_activate(cpu_buffer);
}

/* 
                                                             
                                                        
                                   
 */
void ring_buffer_reset_cpu(struct ring_buffer *buffer, int cpu)
{
	struct ring_buffer_per_cpu *cpu_buffer = buffer->buffers[cpu];
	unsigned long flags;

	if (!cpumask_test_cpu(cpu, buffer->cpumask))
		return;

	atomic_inc(&buffer->resize_disabled);
	atomic_inc(&cpu_buffer->record_disabled);

	/*                                     */
	synchronize_sched();

	raw_spin_lock_irqsave(&cpu_buffer->reader_lock, flags);

	if (RB_WARN_ON(cpu_buffer, local_read(&cpu_buffer->committing)))
		goto out;

	arch_spin_lock(&cpu_buffer->lock);

	rb_reset_cpu(cpu_buffer);

	arch_spin_unlock(&cpu_buffer->lock);

 out:
	raw_spin_unlock_irqrestore(&cpu_buffer->reader_lock, flags);

	atomic_dec(&cpu_buffer->record_disabled);
	atomic_dec(&buffer->resize_disabled);
}
EXPORT_SYMBOL_GPL(ring_buffer_reset_cpu);

/* 
                                          
                                                    
 */
void ring_buffer_reset(struct ring_buffer *buffer)
{
	int cpu;

	for_each_buffer_cpu(buffer, cpu)
		ring_buffer_reset_cpu(buffer, cpu);
}
EXPORT_SYMBOL_GPL(ring_buffer_reset);

/* 
                                                
                                   
 */
int ring_buffer_empty(struct ring_buffer *buffer)
{
	struct ring_buffer_per_cpu *cpu_buffer;
	unsigned long flags;
	int dolock;
	int cpu;
	int ret;

	dolock = rb_ok_to_lock();

	/*                                                                   */
	for_each_buffer_cpu(buffer, cpu) {
		cpu_buffer = buffer->buffers[cpu];
		local_irq_save(flags);
		if (dolock)
			raw_spin_lock(&cpu_buffer->reader_lock);
		ret = rb_per_cpu_empty(cpu_buffer);
		if (dolock)
			raw_spin_unlock(&cpu_buffer->reader_lock);
		local_irq_restore(flags);

		if (!ret)
			return 0;
	}

	return 1;
}
EXPORT_SYMBOL_GPL(ring_buffer_empty);

/* 
                                                                  
                           
                               
 */
int ring_buffer_empty_cpu(struct ring_buffer *buffer, int cpu)
{
	struct ring_buffer_per_cpu *cpu_buffer;
	unsigned long flags;
	int dolock;
	int ret;

	if (!cpumask_test_cpu(cpu, buffer->cpumask))
		return 1;

	dolock = rb_ok_to_lock();

	cpu_buffer = buffer->buffers[cpu];
	local_irq_save(flags);
	if (dolock)
		raw_spin_lock(&cpu_buffer->reader_lock);
	ret = rb_per_cpu_empty(cpu_buffer);
	if (dolock)
		raw_spin_unlock(&cpu_buffer->reader_lock);
	local_irq_restore(flags);

	return ret;
}
EXPORT_SYMBOL_GPL(ring_buffer_empty_cpu);

#ifdef CONFIG_RING_BUFFER_ALLOW_SWAP
/* 
                                                                    
                                     
                                           
  
                                                                     
                                                               
                                                                  
                      
 */
int ring_buffer_swap_cpu(struct ring_buffer *buffer_a,
			 struct ring_buffer *buffer_b, int cpu)
{
	struct ring_buffer_per_cpu *cpu_buffer_a;
	struct ring_buffer_per_cpu *cpu_buffer_b;
	int ret = -EINVAL;

	if (!cpumask_test_cpu(cpu, buffer_a->cpumask) ||
	    !cpumask_test_cpu(cpu, buffer_b->cpumask))
		goto out;

	cpu_buffer_a = buffer_a->buffers[cpu];
	cpu_buffer_b = buffer_b->buffers[cpu];

	/*                                                          */
	if (cpu_buffer_a->nr_pages != cpu_buffer_b->nr_pages)
		goto out;

	ret = -EAGAIN;

	if (ring_buffer_flags != RB_BUFFERS_ON)
		goto out;

	if (atomic_read(&buffer_a->record_disabled))
		goto out;

	if (atomic_read(&buffer_b->record_disabled))
		goto out;

	if (atomic_read(&cpu_buffer_a->record_disabled))
		goto out;

	if (atomic_read(&cpu_buffer_b->record_disabled))
		goto out;

	/*
                                                     
                                             
                                                          
                                                 
  */
	atomic_inc(&cpu_buffer_a->record_disabled);
	atomic_inc(&cpu_buffer_b->record_disabled);

	ret = -EBUSY;
	if (local_read(&cpu_buffer_a->committing))
		goto out_dec;
	if (local_read(&cpu_buffer_b->committing))
		goto out_dec;

	buffer_a->buffers[cpu] = cpu_buffer_b;
	buffer_b->buffers[cpu] = cpu_buffer_a;

	cpu_buffer_b->buffer = buffer_a;
	cpu_buffer_a->buffer = buffer_b;

	ret = 0;

out_dec:
	atomic_dec(&cpu_buffer_a->record_disabled);
	atomic_dec(&cpu_buffer_b->record_disabled);
out:
	return ret;
}
EXPORT_SYMBOL_GPL(ring_buffer_swap_cpu);
#endif /*                               */

/* 
                                                                    
                                       
  
                                                                   
                                                                 
                                                                   
                                                              
                                                                
                                                               
                                                                 
  
           
                                         
 */
void *ring_buffer_alloc_read_page(struct ring_buffer *buffer, int cpu)
{
	struct buffer_data_page *bpage;
	struct page *page;

	page = alloc_pages_node(cpu_to_node(cpu),
				GFP_KERNEL | __GFP_NORETRY, 0);
	if (!page)
		return NULL;

	bpage = page_address(page);

	rb_init_page(bpage);

	return bpage;
}
EXPORT_SYMBOL_GPL(ring_buffer_alloc_read_page);

/* 
                                                           
                                                
                          
  
                                                          
 */
void ring_buffer_free_read_page(struct ring_buffer *buffer, void *data)
{
	free_page((unsigned long)data);
}
EXPORT_SYMBOL_GPL(ring_buffer_free_read_page);

/* 
                                                              
                                  
                                                                         
                          
                                         
                                                                  
  
                                                                          
                                                                   
                                                                           
                                          
  
               
                                               
              
                 
                                                            
                
                             
  
                                                              
                                     
  
                                                                        
                                                                  
                                                                     
                         
  
           
                                                                          
                                       
 */
int ring_buffer_read_page(struct ring_buffer *buffer,
			  void **data_page, size_t len, int cpu, int full)
{
	struct ring_buffer_per_cpu *cpu_buffer = buffer->buffers[cpu];
	struct ring_buffer_event *event;
	struct buffer_data_page *bpage;
	struct buffer_page *reader;
	unsigned long missed_events;
	unsigned long flags;
	unsigned int commit;
	unsigned int read;
	u64 save_timestamp;
	int ret = -1;

	if (!cpumask_test_cpu(cpu, buffer->cpumask))
		goto out;

	/*
                                                          
                             
  */
	if (len <= BUF_PAGE_HDR_SIZE)
		goto out;

	len -= BUF_PAGE_HDR_SIZE;

	if (!data_page)
		goto out;

	bpage = *data_page;
	if (!bpage)
		goto out;

	raw_spin_lock_irqsave(&cpu_buffer->reader_lock, flags);

	reader = rb_get_reader_page(cpu_buffer);
	if (!reader)
		goto out_unlock;

	event = rb_reader_event(cpu_buffer);

	read = reader->read;
	commit = rb_page_commit(reader);

	/*                                  */
	missed_events = cpu_buffer->lost_events;

	/*
                                           
                                                            
                                       
                                                      
                                                                  
  */
	if (read || (len < (commit - read)) ||
	    cpu_buffer->reader_page == cpu_buffer->commit_page) {
		struct buffer_data_page *rpage = cpu_buffer->reader_page->page;
		unsigned int rpos = read;
		unsigned int pos = 0;
		unsigned int size;

		if (full)
			goto out_unlock;

		if (len > (commit - read))
			len = (commit - read);

		/*                                               */
		size = rb_event_ts_length(event);

		if (len < size)
			goto out_unlock;

		/*                                                         */
		save_timestamp = cpu_buffer->read_stamp;

		/*                                  */
		do {
			/*                                       
                                                   
                                                        
                        
                                                          
                        */
			size = rb_event_length(event);
			memcpy(bpage->data + pos, rpage->data + rpos, size);

			len -= size;

			rb_advance_reader(cpu_buffer);
			rpos = reader->read;
			pos += size;

			if (rpos >= commit)
				break;

			event = rb_reader_event(cpu_buffer);
			/*                                               */
			size = rb_event_ts_length(event);
		} while (len >= size);

		/*              */
		local_set(&bpage->commit, pos);
		bpage->time_stamp = save_timestamp;

		/*                                       */
		read = 0;
	} else {
		/*                          */
		cpu_buffer->read += rb_page_entries(reader);
		cpu_buffer->read_bytes += BUF_PAGE_SIZE;

		/*                */
		rb_init_page(bpage);
		bpage = reader->page;
		reader->page = *data_page;
		local_set(&reader->write, 0);
		local_set(&reader->entries, 0);
		reader->read = 0;
		*data_page = bpage;

		/*
                                        
                                                    
                 
   */
		if (reader->real_end)
			local_set(&bpage->commit, reader->real_end);
	}
	ret = read;

	cpu_buffer->lost_events = 0;

	commit = local_read(&bpage->commit);
	/*
                                                    
  */
	if (missed_events) {
		/*                                                    
                                         
   */
		if (BUF_PAGE_SIZE - commit >= sizeof(missed_events)) {
			memcpy(&bpage->data[commit], &missed_events,
			       sizeof(missed_events));
			local_add(RB_MISSED_STORED, &bpage->commit);
			commit += sizeof(missed_events);
		}
		local_add(RB_MISSED_EVENTS, &bpage->commit);
	}

	/*
                                                        
  */
	if (commit < BUF_PAGE_SIZE)
		memset(&bpage->data[commit], 0, BUF_PAGE_SIZE - commit);

 out_unlock:
	raw_spin_unlock_irqrestore(&cpu_buffer->reader_lock, flags);

 out:
	return ret;
}
EXPORT_SYMBOL_GPL(ring_buffer_read_page);

#ifdef CONFIG_HOTPLUG_CPU
static int rb_cpu_notify(struct notifier_block *self,
			 unsigned long action, void *hcpu)
{
	struct ring_buffer *buffer =
		container_of(self, struct ring_buffer, cpu_notify);
	long cpu = (long)hcpu;
	int cpu_i, nr_pages_same;
	unsigned int nr_pages;

	switch (action) {
	case CPU_UP_PREPARE:
	case CPU_UP_PREPARE_FROZEN:
		if (cpumask_test_cpu(cpu, buffer->cpumask))
			return NOTIFY_OK;

		nr_pages = 0;
		nr_pages_same = 1;
		/*                                 */
		for_each_buffer_cpu(buffer, cpu_i) {
			/*                                         */
			if (nr_pages == 0)
				nr_pages = buffer->buffers[cpu_i]->nr_pages;
			if (nr_pages != buffer->buffers[cpu_i]->nr_pages) {
				nr_pages_same = 0;
				break;
			}
		}
		/*                                                  */
		if (!nr_pages_same)
			nr_pages = 2;
		buffer->buffers[cpu] =
			rb_allocate_cpu_buffer(buffer, nr_pages, cpu);
		if (!buffer->buffers[cpu]) {
			WARN(1, "failed to allocate ring buffer on CPU %ld\n",
			     cpu);
			return NOTIFY_OK;
		}
		smp_wmb();
		cpumask_set_cpu(cpu, buffer->cpumask);
		break;
	case CPU_DOWN_PREPARE:
	case CPU_DOWN_PREPARE_FROZEN:
		/*
                
                                                        
                                            
   */
		break;
	default:
		break;
	}
	return NOTIFY_OK;
}
#endif

#ifdef CONFIG_RING_BUFFER_STARTUP_TEST
/*
                                                      
                                                                
                                                             
                                                            
                                                    
  
                                                               
                                                             
                                                     
  
                                                             
                                                               
                                                  
 */
static struct task_struct *rb_threads[NR_CPUS] __initdata;

struct rb_test_data {
	struct ring_buffer	*buffer;
	unsigned long		events;
	unsigned long		bytes_written;
	unsigned long		bytes_alloc;
	unsigned long		bytes_dropped;
	unsigned long		events_nested;
	unsigned long		bytes_written_nested;
	unsigned long		bytes_alloc_nested;
	unsigned long		bytes_dropped_nested;
	int			min_size_nested;
	int			max_size_nested;
	int			max_size;
	int			min_size;
	int			cpu;
	int			cnt;
};

static struct rb_test_data rb_data[NR_CPUS] __initdata;

/*               */
#define RB_TEST_BUFFER_SIZE	1048576

static char rb_string[] __initdata =
	"abcdefghijklmnopqrstuvwxyz1234567890!@#$%^&*()?+\\"
	"?+|:';\",.<>/?abcdefghijklmnopqrstuvwxyz1234567890"
	"!@#$%^&*()?+\\?+|:';\",.<>/?abcdefghijklmnopqrstuv";

static bool rb_test_started __initdata;

struct rb_item {
	int size;
	char str[];
};

static __init int rb_write_something(struct rb_test_data *data, bool nested)
{
	struct ring_buffer_event *event;
	struct rb_item *item;
	bool started;
	int event_len;
	int size;
	int len;
	int cnt;

	/*                                                   */
	cnt = data->cnt + (nested ? 27 : 0);

	/*                                                   */
	size = (data->cnt * 68 / 25) % (sizeof(rb_string) - 1);

	len = size + sizeof(struct rb_item);

	started = rb_test_started;
	/*                                                     */
	smp_rmb();

	event = ring_buffer_lock_reserve(data->buffer, len);
	if (!event) {
		/*                                           */
		if (started) {
			if (nested)
				data->bytes_dropped += len;
			else
				data->bytes_dropped_nested += len;
		}
		return len;
	}

	event_len = ring_buffer_event_length(event);

	if (RB_WARN_ON(data->buffer, event_len < len))
		goto out;

	item = ring_buffer_event_data(event);
	item->size = size;
	memcpy(item->str, rb_string, size);

	if (nested) {
		data->bytes_alloc_nested += event_len;
		data->bytes_written_nested += len;
		data->events_nested++;
		if (!data->min_size_nested || len < data->min_size_nested)
			data->min_size_nested = len;
		if (len > data->max_size_nested)
			data->max_size_nested = len;
	} else {
		data->bytes_alloc += event_len;
		data->bytes_written += len;
		data->events++;
		if (!data->min_size || len < data->min_size)
			data->max_size = len;
		if (len > data->max_size)
			data->max_size = len;
	}

 out:
	ring_buffer_unlock_commit(data->buffer, event);

	return 0;
}

static __init int rb_test(void *arg)
{
	struct rb_test_data *data = arg;

	while (!kthread_should_stop()) {
		rb_write_something(data, false);
		data->cnt++;

		set_current_state(TASK_INTERRUPTIBLE);
		/*                                                       */
		usleep_range(((data->cnt % 3) + 1) * 100, 1000);
	}

	return 0;
}

static __init void rb_ipi(void *ignore)
{
	struct rb_test_data *data;
	int cpu = smp_processor_id();

	data = &rb_data[cpu];
	rb_write_something(data, true);
}

static __init int rb_hammer_test(void *arg)
{
	while (!kthread_should_stop()) {

		/*                                        */
		smp_call_function(rb_ipi, NULL, 1);
		/*                                               */
		schedule();
	}

	return 0;
}

static __init int test_ringbuffer(void)
{
	struct task_struct *rb_hammer;
	struct ring_buffer *buffer;
	int cpu;
	int ret = 0;

	pr_info("Running ring buffer tests...\n");

	buffer = ring_buffer_alloc(RB_TEST_BUFFER_SIZE, RB_FL_OVERWRITE);
	if (WARN_ON(!buffer))
		return 0;

	/*                                                      */
	ring_buffer_record_off(buffer);

	for_each_online_cpu(cpu) {
		rb_data[cpu].buffer = buffer;
		rb_data[cpu].cpu = cpu;
		rb_data[cpu].cnt = cpu;
		rb_threads[cpu] = kthread_create(rb_test, &rb_data[cpu],
						 "rbtester/%d", cpu);
		if (WARN_ON(!rb_threads[cpu])) {
			pr_cont("FAILED\n");
			ret = -1;
			goto out_free;
		}

		kthread_bind(rb_threads[cpu], cpu);
 		wake_up_process(rb_threads[cpu]);
	}

	/*                           */
	rb_hammer = kthread_run(rb_hammer_test, NULL, "rbhammer");
	if (WARN_ON(!rb_hammer)) {
		pr_cont("FAILED\n");
		ret = -1;
		goto out_free;
	}

	ring_buffer_record_on(buffer);
	/*
                                                          
                                                         
                                                         
                                                          
                                                             
                                                            
                                              
  */
	smp_wmb();
	rb_test_started = true;

	set_current_state(TASK_INTERRUPTIBLE);
	/*                         */;
	schedule_timeout(10 * HZ);

	kthread_stop(rb_hammer);

 out_free:
	for_each_online_cpu(cpu) {
		if (!rb_threads[cpu])
			break;
		kthread_stop(rb_threads[cpu]);
	}
	if (ret) {
		ring_buffer_free(buffer);
		return ret;
	}

	/*         */
	pr_info("finished\n");
	for_each_online_cpu(cpu) {
		struct ring_buffer_event *event;
		struct rb_test_data *data = &rb_data[cpu];
		struct rb_item *item;
		unsigned long total_events;
		unsigned long total_dropped;
		unsigned long total_written;
		unsigned long total_alloc;
		unsigned long total_read = 0;
		unsigned long total_size = 0;
		unsigned long total_len = 0;
		unsigned long total_lost = 0;
		unsigned long lost;
		int big_event_size;
		int small_event_size;

		ret = -1;

		total_events = data->events + data->events_nested;
		total_written = data->bytes_written + data->bytes_written_nested;
		total_alloc = data->bytes_alloc + data->bytes_alloc_nested;
		total_dropped = data->bytes_dropped + data->bytes_dropped_nested;

		big_event_size = data->max_size + data->max_size_nested;
		small_event_size = data->min_size + data->min_size_nested;

		pr_info("CPU %d:\n", cpu);
		pr_info("              events:    %ld\n", total_events);
		pr_info("       dropped bytes:    %ld\n", total_dropped);
		pr_info("       alloced bytes:    %ld\n", total_alloc);
		pr_info("       written bytes:    %ld\n", total_written);
		pr_info("       biggest event:    %d\n", big_event_size);
		pr_info("      smallest event:    %d\n", small_event_size);

		if (RB_WARN_ON(buffer, total_dropped))
			break;

		ret = 0;

		while ((event = ring_buffer_consume(buffer, cpu, NULL, &lost))) {
			total_lost += lost;
			item = ring_buffer_event_data(event);
			total_len += ring_buffer_event_length(event);
			total_size += item->size + sizeof(struct rb_item);
			if (memcmp(&item->str[0], rb_string, item->size) != 0) {
				pr_info("FAILED!\n");
				pr_info("buffer had: %.*s\n", item->size, item->str);
				pr_info("expected:   %.*s\n", item->size, rb_string);
				RB_WARN_ON(buffer, 1);
				ret = -1;
				break;
			}
			total_read++;
		}
		if (ret)
			break;

		ret = -1;

		pr_info("         read events:   %ld\n", total_read);
		pr_info("         lost events:   %ld\n", total_lost);
		pr_info("        total events:   %ld\n", total_lost + total_read);
		pr_info("  recorded len bytes:   %ld\n", total_len);
		pr_info(" recorded size bytes:   %ld\n", total_size);
		if (total_lost)
			pr_info(" With dropped events, record len and size may not match\n"
				" alloced and written from above\n");
		if (!total_lost) {
			if (RB_WARN_ON(buffer, total_len != total_alloc ||
				       total_size != total_written))
				break;
		}
		if (RB_WARN_ON(buffer, total_lost + total_read != total_events))
			break;

		ret = 0;
	}
	if (!ret)
		pr_info("Ring buffer PASSED!\n");

	ring_buffer_free(buffer);
	return 0;
}

late_initcall(test_ringbuffer);
#endif /*                                 */
