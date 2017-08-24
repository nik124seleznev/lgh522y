#ifndef _METAG_CACHEFLUSH_H
#define _METAG_CACHEFLUSH_H

#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/io.h>

#include <asm/l2cache.h>
#include <asm/metag_isa.h>
#include <asm/metag_mem.h>

void metag_cache_probe(void);

void metag_data_cache_flush_all(const void *start);
void metag_code_cache_flush_all(const void *start);

/*
                                                                                
                                                                              
                                                                                
                                                                            
                                                     
 */
void metag_data_cache_flush(const void *start, int bytes);
void metag_code_cache_flush(const void *start, int bytes);

#ifdef CONFIG_METAG_META12

/*                                                   */

static inline void __flush_cache_all(void)
{
	metag_code_cache_flush_all((void *) PAGE_OFFSET);
	metag_data_cache_flush_all((void *) PAGE_OFFSET);
}

#define flush_cache_all() __flush_cache_all()

/*                                                                     */
static inline void flush_cache_mm(struct mm_struct *mm)
{
	if (mm == current->mm)
		__flush_cache_all();
}

#define flush_cache_dup_mm(mm) flush_cache_mm(mm)

/*                                         */
static inline void flush_cache_range(struct vm_area_struct *vma,
				     unsigned long start, unsigned long end)
{
	flush_cache_mm(vma->vm_mm);
}

static inline void flush_cache_page(struct vm_area_struct *vma,
				    unsigned long vmaddr, unsigned long pfn)
{
	flush_cache_mm(vma->vm_mm);
}

#define ARCH_IMPLEMENTS_FLUSH_DCACHE_PAGE	1
static inline void flush_dcache_page(struct page *page)
{
	metag_data_cache_flush_all((void *) PAGE_OFFSET);
}

#define flush_dcache_mmap_lock(mapping)		do { } while (0)
#define flush_dcache_mmap_unlock(mapping)	do { } while (0)

static inline void flush_icache_page(struct vm_area_struct *vma,
				     struct page *page)
{
	metag_code_cache_flush(page_to_virt(page), PAGE_SIZE);
}

static inline void flush_cache_vmap(unsigned long start, unsigned long end)
{
	metag_data_cache_flush_all((void *) PAGE_OFFSET);
}

static inline void flush_cache_vunmap(unsigned long start, unsigned long end)
{
	metag_data_cache_flush_all((void *) PAGE_OFFSET);
}

#else

/*                                                    */

#define flush_cache_all()			do { } while (0)
#define flush_cache_mm(mm)			do { } while (0)
#define flush_cache_dup_mm(mm)			do { } while (0)
#define flush_cache_range(vma, start, end)	do { } while (0)
#define flush_cache_page(vma, vmaddr, pfn)	do { } while (0)
#define flush_dcache_mmap_lock(mapping)		do { } while (0)
#define flush_dcache_mmap_unlock(mapping)	do { } while (0)
#define flush_icache_page(vma, pg)		do { } while (0)
#define flush_cache_vmap(start, end)		do { } while (0)
#define flush_cache_vunmap(start, end)		do { } while (0)

#define ARCH_IMPLEMENTS_FLUSH_DCACHE_PAGE	1
static inline void flush_dcache_page(struct page *page)
{
	/*                                                              
                                                                   
                 
  */
#ifdef CONFIG_SMP
	metag_out32(1, SYSC_ICACHE_FLUSH);
#else
	metag_code_cache_flush_all((void *) PAGE_OFFSET);
#endif
}

#endif

/*                                                             */
static inline void flush_icache_range(unsigned long address,
				      unsigned long endaddr)
{
#ifdef CONFIG_SMP
	metag_out32(1, SYSC_ICACHE_FLUSH);
#else
	metag_code_cache_flush((void *) address, endaddr - address);
#endif
}

static inline void flush_cache_sigtramp(unsigned long addr, int size)
{
	/*
                                                           
                                                           
   
                                                             
                                               
  */
#ifdef CONFIG_SMP
	metag_out32(1, SYSC_ICACHE_FLUSH);
#else
	metag_code_cache_flush((void *) addr, size);
#endif
}

#ifdef CONFIG_METAG_L2C

/*
                                                                                
                    
 */
static inline void cachewd_line(void *addr, unsigned int data)
{
	unsigned long masked = (unsigned long)addr & -0x40;
	__builtin_meta2_cachewd((void *)masked, data);
}

/*                                                           */
static inline void cachew_region_op(void *start, unsigned long size,
				    unsigned int op)
{
	unsigned long offset = (unsigned long)start & 0x3f;
	int i;
	if (offset) {
		size += offset;
		start -= offset;
	}
	i = (size - 1) >> 6;
	do {
		__builtin_meta2_cachewd(start, op);
		start += 0x40;
	} while (i--);
}

/*                                                          */
static inline void l2c_fence_flush(void *addr)
{
	/*
                                                
                                                                       
                           
  */
	(void)(volatile u8 *)addr;
	cachewd_line(addr, CACHEW_FLUSH_L1D_L2);
}

/*                                                          */
static inline void l2c_fence(void *addr)
{
	/*
                                                                        
                                                                    
                                                 
  */
	cachewd_line(addr, CACHEW_FLUSH_L1D_L2);
	l2c_fence_flush(addr);
}

/*                                                */
static inline void flush_dcache_region(void *start, unsigned long size)
{
	/*                                                                   */
	if (meta_l2c_is_enabled()) {
		cachew_region_op(start, size, CACHEW_FLUSH_L1D_L2);
		if (meta_l2c_is_writeback())
			l2c_fence_flush(start + size - 1);
	} else {
		metag_data_cache_flush(start, size);
	}
}

/*                                                                         */
static inline void writeback_dcache_region(void *start, unsigned long size)
{
	if (meta_l2c_is_enabled() && meta_l2c_is_writeback()) {
		cachew_region_op(start, size, CACHEW_WRITEBACK_L1D_L2);
		l2c_fence(start + size - 1);
	}
}

/*                                               */
static inline void invalidate_dcache_region(void *start, unsigned long size)
{
	if (meta_l2c_is_enabled())
		cachew_region_op(start, size, CACHEW_INVALIDATE_L1D_L2);
	else
		metag_data_cache_flush(start, size);
}
#else
#define flush_dcache_region(s, l)	metag_data_cache_flush((s), (l))
#define writeback_dcache_region(s, l)	do {} while (0)
#define invalidate_dcache_region(s, l)	flush_dcache_region((s), (l))
#endif

static inline void copy_to_user_page(struct vm_area_struct *vma,
				     struct page *page, unsigned long vaddr,
				     void *dst, const void *src,
				     unsigned long len)
{
	memcpy(dst, src, len);
	flush_icache_range((unsigned long)dst, (unsigned long)dst + len);
}

static inline void copy_from_user_page(struct vm_area_struct *vma,
				       struct page *page, unsigned long vaddr,
				       void *dst, const void *src,
				       unsigned long len)
{
	memcpy(dst, src, len);
}

#endif /*                     */
