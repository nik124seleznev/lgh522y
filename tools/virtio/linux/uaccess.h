#ifndef UACCESS_H
#define UACCESS_H
extern void *__user_addr_min, *__user_addr_max;

#define ACCESS_ONCE(x) (*(volatile typeof(x) *)&(x))

static inline void __chk_user_ptr(const volatile void *p, size_t size)
{
	assert(p >= __user_addr_min && p + size <= __user_addr_max);
}

#define put_user(x, ptr)					\
({								\
	typeof(ptr) __pu_ptr = (ptr);				\
	__chk_user_ptr(__pu_ptr, sizeof(*__pu_ptr));		\
	ACCESS_ONCE(*(__pu_ptr)) = x;				\
	0;							\
})

#define get_user(x, ptr)					\
({								\
	typeof(ptr) __pu_ptr = (ptr);				\
	__chk_user_ptr(__pu_ptr, sizeof(*__pu_ptr));		\
	x = ACCESS_ONCE(*(__pu_ptr));				\
	0;							\
})

static void volatile_memcpy(volatile char *to, const volatile char *from, 
			    unsigned long n)
{
	while (n--)
		*(to++) = *(from++);
}

static inline int copy_from_user(void *to, const void __user volatile *from,
				 unsigned long n)
{
	__chk_user_ptr(from, n);
	volatile_memcpy(to, from, n);
	return 0;
}

static inline int copy_to_user(void __user volatile *to, const void *from,
			       unsigned long n)
{
	__chk_user_ptr(to, n);
	volatile_memcpy(to, from, n);
	return 0;
}
#endif /*           */