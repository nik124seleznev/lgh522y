#ifndef __LINUX_SEQLOCK_H
#define __LINUX_SEQLOCK_H
/*
                                                                            
                                                                       
                                                                     
                                                     
                                              
  
                                                                    
                                                            
                                                    
  
                         
        
                                 
       
                                           
  
  
                                                                 
                                                                         
                                
  
                                         
                                      
 */

#include <linux/spinlock.h>
#include <linux/preempt.h>
#include <asm/processor.h>

/*
                                       
                                                              
                                                                
                                  
 */
typedef struct seqcount {
	unsigned sequence;
} seqcount_t;

#define SEQCNT_ZERO { 0 }
#define seqcount_init(x)	do { *(x) = (seqcount_t) SEQCNT_ZERO; } while (0)

/* 
                                                                              
                            
                                                     
  
                                                                          
                                                                          
                                                                       
                                      
  
                                                                       
            
 */
static inline unsigned __read_seqcount_begin(const seqcount_t *s)
{
	unsigned ret;

repeat:
	ret = ACCESS_ONCE(s->sequence);
	if (unlikely(ret & 1)) {
		cpu_relax();
		goto repeat;
	}
	return ret;
}

/* 
                                                          
                            
                                                     
  
                                                                           
                                                                             
            
 */
static inline unsigned read_seqcount_begin(const seqcount_t *s)
{
	unsigned ret = __read_seqcount_begin(s);
	smp_rmb();
	return ret;
}

/* 
                                                         
                            
                                                     
  
                                                                          
                                                                             
            
  
                                                                          
                                                                      
                                                                       
                    
 */
static inline unsigned raw_seqcount_begin(const seqcount_t *s)
{
	unsigned ret = ACCESS_ONCE(s->sequence);
	smp_rmb();
	return ret & ~1;
}

/* 
                                                                            
                            
                                          
                                          
  
                                                                          
                                                                          
                                                                       
                                      
  
                                                                       
            
 */
static inline int __read_seqcount_retry(const seqcount_t *s, unsigned start)
{
	return unlikely(s->sequence != start);
}

/* 
                                                        
                            
                                          
                                          
  
                                                                            
                                                                         
            
 */
static inline int read_seqcount_retry(const seqcount_t *s, unsigned start)
{
	smp_rmb();
	return __read_seqcount_retry(s, start);
}


/*
                                                                     
                
 */
static inline void write_seqcount_begin(seqcount_t *s)
{
	s->sequence++;
	smp_wmb();
}

static inline void write_seqcount_end(seqcount_t *s)
{
	smp_wmb();
	s->sequence++;
}

/* 
                                                                           
                            
  
                                                                          
                                             
 */
static inline void write_seqcount_barrier(seqcount_t *s)
{
	smp_wmb();
	s->sequence+=2;
}

typedef struct {
	struct seqcount seqcount;
	spinlock_t lock;
} seqlock_t;

/*
                                                                            
                        
 */
#define __SEQLOCK_UNLOCKED(lockname)			\
	{						\
		.seqcount = SEQCNT_ZERO,		\
		.lock =	__SPIN_LOCK_UNLOCKED(lockname)	\
	}

#define seqlock_init(x)					\
	do {						\
		seqcount_init(&(x)->seqcount);		\
		spin_lock_init(&(x)->lock);		\
	} while (0)

#define DEFINE_SEQLOCK(x) \
		seqlock_t x = __SEQLOCK_UNLOCKED(x)

/*
                                                                       
 */
static inline unsigned read_seqbegin(const seqlock_t *sl)
{
	return read_seqcount_begin(&sl->seqcount);
}

static inline unsigned read_seqretry(const seqlock_t *sl, unsigned start)
{
	return read_seqcount_retry(&sl->seqcount, start);
}

/*
                                               
                                       
                                                                         
 */
static inline void write_seqlock(seqlock_t *sl)
{
	spin_lock(&sl->lock);
	write_seqcount_begin(&sl->seqcount);
}

static inline void write_sequnlock(seqlock_t *sl)
{
	write_seqcount_end(&sl->seqcount);
	spin_unlock(&sl->lock);
}

static inline void write_seqlock_bh(seqlock_t *sl)
{
	spin_lock_bh(&sl->lock);
	write_seqcount_begin(&sl->seqcount);
}

static inline void write_sequnlock_bh(seqlock_t *sl)
{
	write_seqcount_end(&sl->seqcount);
	spin_unlock_bh(&sl->lock);
}

static inline void write_seqlock_irq(seqlock_t *sl)
{
	spin_lock_irq(&sl->lock);
	write_seqcount_begin(&sl->seqcount);
}

static inline void write_sequnlock_irq(seqlock_t *sl)
{
	write_seqcount_end(&sl->seqcount);
	spin_unlock_irq(&sl->lock);
}

static inline unsigned long __write_seqlock_irqsave(seqlock_t *sl)
{
	unsigned long flags;

	spin_lock_irqsave(&sl->lock, flags);
	write_seqcount_begin(&sl->seqcount);
	return flags;
}

#define write_seqlock_irqsave(lock, flags)				\
	do { flags = __write_seqlock_irqsave(lock); } while (0)

static inline void
write_sequnlock_irqrestore(seqlock_t *sl, unsigned long flags)
{
	write_seqcount_end(&sl->seqcount);
	spin_unlock_irqrestore(&sl->lock, flags);
}

#endif /*                   */
