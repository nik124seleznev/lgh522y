/*
 * safe read and write memory routines callable while atomic
 *
 * Copyright 2012 Imagination Technologies
 */

#include <linux/uaccess.h>
#include <asm/io.h>

/*
                                                                               
                                                                           
                                                                                
                                                     
                                                                          
 */
#ifdef CONFIG_METAG_META21
long probe_kernel_write(void *dst, const void *src, size_t size)
{
	unsigned long ldst = (unsigned long)dst;
	void __iomem *iodst = (void __iomem *)dst;
	unsigned long lsrc = (unsigned long)src;
	const u8 *psrc = (u8 *)src;
	unsigned int pte, i;
	u8 bounce[8] __aligned(8);

	if (!size)
		return 0;

	/*                                                                 */
	pte = __builtin_meta2_cacherd(dst);

	/*                                           */
	if ((pte & (MMCU_ENTRY_WR_BIT | MMCU_ENTRY_VAL_BIT))
	    != (MMCU_ENTRY_WR_BIT | MMCU_ENTRY_VAL_BIT))
		return -EFAULT;

	/*                                                                 */
	if (pte & MMCU_ENTRY_WRC_BIT	|| /*                       */
	    (ldst & (size - 1))		|| /*                       */
	    size > 8			|| /*                          */
	    (size & (size - 1)))	   /*                     */
		return __probe_kernel_write(dst, src, size);

	/*                                                               */
	if (lsrc & (size - 1)) {
		for (i = 0; i < size; ++i)
			bounce[i] = psrc[i];
		psrc = bounce;
	}

	switch (size) {
	case 1:
		writeb(*psrc, iodst);
		break;
	case 2:
		writew(*(const u16 *)psrc, iodst);
		break;
	case 4:
		writel(*(const u32 *)psrc, iodst);
		break;
	case 8:
		writeq(*(const u64 *)psrc, iodst);
		break;
	}
	return 0;
}
#endif
