#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>
#include <linux/errno.h>

#include <asm/byteorder.h>
#include <asm/word-at-a-time.h>

#ifdef CONFIG_HAVE_EFFICIENT_UNALIGNED_ACCESS
#define IS_UNALIGNED(src, dst)	0
#else
#define IS_UNALIGNED(src, dst)	\
	(((long) dst | (long) src) & (sizeof(long) - 1))
#endif

/*
                                                            
                                                           
                                                             
                         
 */
static inline long do_strncpy_from_user(char *dst, const char __user *src, long count, unsigned long max)
{
	const struct word_at_a_time constants = WORD_AT_A_TIME_CONSTANTS;
	long res = 0;

	/*
                                                       
                                                       
  */
	if (max > count)
		max = count;

	if (IS_UNALIGNED(src, dst))
		goto byte_at_a_time;

	while (max >= sizeof(unsigned long)) {
		unsigned long c, data;

		/*                                                    */
		if (unlikely(__get_user(c,(unsigned long __user *)(src+res))))
			break;
		*(unsigned long *)(dst+res) = c;
		if (has_zero(c, &data, &constants)) {
			data = prep_zero_mask(c, data, &constants);
			data = create_zero_mask(data);
			return res + find_zero(data);
		}
		res += sizeof(unsigned long);
		max -= sizeof(unsigned long);
	}

byte_at_a_time:
	while (max) {
		char c;

		if (unlikely(__get_user(c,src+res)))
			return -EFAULT;
		dst[res] = c;
		if (!c)
			return res;
		res++;
		max--;
	}

	/*
                                                                
                                                                 
  */
	if (res >= count)
		return res;

	/*
                                                               
                                                              
  */
	return -EFAULT;
}

/* 
                                                                    
                                                                        
                                   
                                         
                                                                       
  
                                                                 
  
                                                                           
        
  
                                                                         
           
  
                                                                          
                      
 */
long strncpy_from_user(char *dst, const char __user *src, long count)
{
	unsigned long max_addr, src_addr;

	if (unlikely(count <= 0))
		return 0;

	max_addr = user_addr_max();
	src_addr = (unsigned long)src;
	if (likely(src_addr < max_addr)) {
		unsigned long max = max_addr - src_addr;
		return do_strncpy_from_user(dst, src, count, max);
	}
	return -EFAULT;
}
EXPORT_SYMBOL(strncpy_from_user);
