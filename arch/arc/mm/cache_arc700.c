/*
 * ARC700 VIPT Cache Management
 *
 * Copyright (C) 2004, 2007-2010, 2011-2012 Synopsys, Inc. (www.synopsys.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *  vineetg: May 2011: for Non-aliasing VIPT D-cache following can be NOPs
 *   -flush_cache_dup_mm (fork)
 *   -likewise for flush_cache_mm (exit/execve)
 *   -likewise for flush_cache_range,flush_cache_page (munmap, exit, COW-break)
 *
 * vineetg: Apr 2011
 *  -Now that MMU can support larger pg sz (16K), the determiniation of
 *   aliasing shd not be based on assumption of 8k pg
 *
 * vineetg: Mar 2011
 *  -optimised version of flush_icache_range( ) for making I/D coherent
 *   when vaddr is available (agnostic of num of aliases)
 *
 * vineetg: Mar 2011
 *  -Added documentation about I-cache aliasing on ARC700 and the way it
 *   was handled up until MMU V2.
 *  -Spotted a three year old bug when killing the 4 aliases, which needs
 *   bottom 2 bits, so we need to do paddr | {0x00, 0x01, 0x02, 0x03}
 *                        instead of paddr | {0x00, 0x01, 0x10, 0x11}
 *   (Rajesh you owe me one now)
 *
 * vineetg: Dec 2010
 *  -Off-by-one error when computing num_of_lines to flush
 *   This broke signal handling with bionic which uses synthetic sigret stub
 *
 * vineetg: Mar 2010
 *  -GCC can't generate ZOL for core cache flush loops.
 *   Conv them into iterations based as opposed to while (start < end) types
 *
 * Vineetg: July 2009
 *  -In I-cache flush routine we used to chk for aliasing for every line INV.
 *   Instead now we setup routines per cache geometry and invoke them
 *   via function pointers.
 *
 * Vineetg: Jan 2009
 *  -Cache Line flush routines used to flush an extra line beyond end addr
 *   because check was while (end >= start) instead of (end > start)
 *     =Some call sites had to work around by doing -1, -4 etc to end param
 *     =Some callers didnt care. This was spec bad in case of INV routines
 *      which would discard valid data (cause of the horrible ext2 bug
 *      in ARC IDE driver)
 *
 * vineetg: June 11th 2008: Fixed flush_icache_range( )
 *  -Since ARC700 caches are not coherent (I$ doesnt snoop D$) both need
 *   to be flushed, which it was not doing.
 *  -load_module( ) passes vmalloc addr (Kernel Virtual Addr) to the API,
 *   however ARC cache maintenance OPs require PHY addr. Thus need to do
 *   vmalloc_to_phy.
 *  -Also added optimisation there, that for range > PAGE SIZE we flush the
 *   entire cache in one shot rather than line by line. For e.g. a module
 *   with Code sz 600k, old code flushed 600k worth of cache (line-by-line),
 *   while cache is only 16 or 32k.
 */

#include <linux/module.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/cache.h>
#include <linux/mmu_context.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/pagemap.h>
#include <asm/cacheflush.h>
#include <asm/cachectl.h>
#include <asm/setup.h>

char *arc_cache_mumbojumbo(int cpu_id, char *buf, int len)
{
	int n = 0;
	unsigned int c = smp_processor_id();

#define PR_CACHE(p, enb, str)						\
{									\
	if (!(p)->ver)							\
		n += scnprintf(buf + n, len - n, str"\t\t: N/A\n");	\
	else								\
		n += scnprintf(buf + n, len - n,			\
			str"\t\t: (%uK) VIPT, %dway set-asc, %ub Line %s\n", \
			TO_KB((p)->sz), (p)->assoc, (p)->line_len,	\
			enb ?  "" : "DISABLED (kernel-build)");		\
}

	PR_CACHE(&cpuinfo_arc700[c].icache, __CONFIG_ARC_HAS_ICACHE, "I-Cache");
	PR_CACHE(&cpuinfo_arc700[c].dcache, __CONFIG_ARC_HAS_DCACHE, "D-Cache");

	return buf;
}

/*
                                                                        
                                       
                                                        
 */
void __cpuinit read_decode_cache_bcr(void)
{
	struct bcr_cache ibcr, dbcr;
	struct cpuinfo_arc_cache *p_ic, *p_dc;
	unsigned int cpu = smp_processor_id();

	p_ic = &cpuinfo_arc700[cpu].icache;
	READ_BCR(ARC_REG_IC_BCR, ibcr);

	if (ibcr.config == 0x3)
		p_ic->assoc = 2;
	p_ic->line_len = 8 << ibcr.line_len;
	p_ic->sz = 0x200 << ibcr.sz;
	p_ic->ver = ibcr.ver;

	p_dc = &cpuinfo_arc700[cpu].dcache;
	READ_BCR(ARC_REG_DC_BCR, dbcr);

	if (dbcr.config == 0x2)
		p_dc->assoc = 4;
	p_dc->line_len = 16 << dbcr.line_len;
	p_dc->sz = 0x200 << dbcr.sz;
	p_dc->ver = dbcr.ver;
}

/*
                                                                        
                                                                            
                                                             
                                                             
                                             
 */
void __cpuinit arc_cache_init(void)
{
	unsigned int temp;
	unsigned int cpu = smp_processor_id();
	struct cpuinfo_arc_cache *ic = &cpuinfo_arc700[cpu].icache;
	struct cpuinfo_arc_cache *dc = &cpuinfo_arc700[cpu].dcache;
	int way_pg_ratio = way_pg_ratio;
	int dcache_does_alias;
	char str[256];

	printk(arc_cache_mumbojumbo(0, str, sizeof(str)));

	if (!ic->ver)
		goto chk_dc;

#ifdef CONFIG_ARC_HAS_ICACHE
	/*                                                       */
	if ((ic->assoc != ARC_ICACHE_WAYS) ||
	    (ic->line_len != ARC_ICACHE_LINE_LEN)) {
		panic("Cache H/W doesn't match kernel Config");
	}
#if (CONFIG_ARC_MMU_VER > 2)
	if (ic->ver != 3) {
		if (running_on_hw)
			panic("Cache ver doesn't match MMU ver\n");

		/*                                      */
		pr_err("Use -prop=icache_version=3,-prop=dcache_version=3\n");

	}
#endif
#endif

	/*                        */
	temp = read_aux_reg(ARC_REG_IC_CTRL);

#ifdef CONFIG_ARC_HAS_ICACHE
	temp &= ~IC_CTRL_CACHE_DISABLE;
#else
	temp |= IC_CTRL_CACHE_DISABLE;
#endif

	write_aux_reg(ARC_REG_IC_CTRL, temp);

chk_dc:
	if (!dc->ver)
		return;

#ifdef CONFIG_ARC_HAS_DCACHE
	if ((dc->assoc != ARC_DCACHE_WAYS) ||
	    (dc->line_len != ARC_DCACHE_LINE_LEN)) {
		panic("Cache H/W doesn't match kernel Config");
	}

	dcache_does_alias = (dc->sz / ARC_DCACHE_WAYS) > PAGE_SIZE;

	/*                            */
	if (dcache_does_alias && !cache_is_vipt_aliasing())
		panic("Enable CONFIG_ARC_CACHE_VIPT_ALIASING\n");
	else if (!dcache_does_alias && cache_is_vipt_aliasing())
		panic("Don't need CONFIG_ARC_CACHE_VIPT_ALIASING\n");
#endif

	/*                                                               
                                                          
                                                         
  */
	temp = read_aux_reg(ARC_REG_DC_CTRL);
	temp &= ~DC_CTRL_INV_MODE_FLUSH;

#ifdef CONFIG_ARC_HAS_DCACHE
	/*                             */
	write_aux_reg(ARC_REG_DC_CTRL, temp & ~IC_CTRL_CACHE_DISABLE);
#else
	/*               */
	write_aux_reg(ARC_REG_DC_FLSH, 0x1);
	/*                 */
	write_aux_reg(ARC_REG_DC_CTRL, temp | IC_CTRL_CACHE_DISABLE);
#endif

	return;
}

#define OP_INV		0x1
#define OP_FLUSH	0x2
#define OP_FLUSH_N_INV	0x3

#ifdef CONFIG_ARC_HAS_DCACHE

/*                                                              
                                                              
 */

static inline void wait_for_flush(void)
{
	while (read_aux_reg(ARC_REG_DC_CTRL) & DC_CTRL_FLUSH_STATUS)
		;
}

/*
                              
                                                
                                                                 
                    
 */
static inline void __dc_entire_op(const int cacheop)
{
	unsigned long flags, tmp = tmp;
	int aux;

	local_irq_save(flags);

	if (cacheop == OP_FLUSH_N_INV) {
		/*                                    
                                                      
                                                     
                                                               
   */
		tmp = read_aux_reg(ARC_REG_DC_CTRL);
		write_aux_reg(ARC_REG_DC_CTRL, tmp | DC_CTRL_INV_MODE_FLUSH);
	}

	if (cacheop & OP_INV)	/*                                     */
		aux = ARC_REG_DC_IVDC;
	else
		aux = ARC_REG_DC_FLSH;

	write_aux_reg(aux, 0x1);

	if (cacheop & OP_FLUSH)	/*                               */
		wait_for_flush();

	/*                                              */
	if (cacheop == OP_FLUSH_N_INV)
		write_aux_reg(ARC_REG_DC_CTRL, tmp & ~DC_CTRL_INV_MODE_FLUSH);

	local_irq_restore(flags);
}

/*
                                
                                                                           
                                                
                                                             
 */
static inline void __dc_line_loop(unsigned long paddr, unsigned long vaddr,
				  unsigned long sz, const int aux_reg)
{
	int num_lines;

	/*                                                                  
                                                                    
                                                     
                                                                    
                                                                    
  */
	if (!(__builtin_constant_p(sz) && sz == PAGE_SIZE)) {
		sz += paddr & ~DCACHE_LINE_MASK;
		paddr &= DCACHE_LINE_MASK;
		vaddr &= DCACHE_LINE_MASK;
	}

	num_lines = DIV_ROUND_UP(sz, ARC_DCACHE_LINE_LEN);

#if (CONFIG_ARC_MMU_VER <= 2)
	paddr |= (vaddr >> PAGE_SHIFT) & 0x1F;
#endif

	while (num_lines-- > 0) {
#if (CONFIG_ARC_MMU_VER > 2)
		/*
                                                   
                                                         
   */
		write_aux_reg(ARC_REG_DC_PTAG, paddr);

		write_aux_reg(aux_reg, vaddr);
		vaddr += ARC_DCACHE_LINE_LEN;
#else
		/*                                    */
		write_aux_reg(aux_reg, paddr);
#endif
		paddr += ARC_DCACHE_LINE_LEN;
	}
}

/*                                                             */
#define __dc_line_op_k(p, sz, op)	__dc_line_op(p, p, sz, op)

/*
                                                                     
 */
static inline void __dc_line_op(unsigned long paddr, unsigned long vaddr,
				unsigned long sz, const int cacheop)
{
	unsigned long flags, tmp = tmp;
	int aux;

	local_irq_save(flags);

	if (cacheop == OP_FLUSH_N_INV) {
		/*
                                        
                                                      
                                                     
                                                               
   */
		tmp = read_aux_reg(ARC_REG_DC_CTRL);
		write_aux_reg(ARC_REG_DC_CTRL, tmp | DC_CTRL_INV_MODE_FLUSH);
	}

	if (cacheop & OP_INV)	/*                                    */
		aux = ARC_REG_DC_IVDL;
	else
		aux = ARC_REG_DC_FLDL;

	__dc_line_loop(paddr, vaddr, sz, aux);

	if (cacheop & OP_FLUSH)	/*                               */
		wait_for_flush();

	/*                                              */
	if (cacheop == OP_FLUSH_N_INV)
		write_aux_reg(ARC_REG_DC_CTRL, tmp & ~DC_CTRL_INV_MODE_FLUSH);

	local_irq_restore(flags);
}

#else

#define __dc_entire_op(cacheop)
#define __dc_line_op(paddr, vaddr, sz, cacheop)
#define __dc_line_op_k(paddr, sz, cacheop)

#endif /*                       */


#ifdef CONFIG_ARC_HAS_ICACHE

/*
                                          
  
                                                                              
                                                                             
                                                                               
                                                                                
                       
  
                                                                              
                                                              
  
                     
                                 
                     
                                                                         
                                                                        
                            
                                                                             
                                                                          
                                                                     
                                                                         
                                                                             
         
  
                                                                          
                                               
                                                           
                                                          
  
                     
         
                     
                                                                             
                                         
                                                                           
                                                                     
                                                                             
                                                    
                                                                              
 */

/*                                                          
                                                           
 */
static void __ic_line_inv_vaddr(unsigned long paddr, unsigned long vaddr,
				unsigned long sz)
{
	unsigned long flags;
	int num_lines;

	/*
                                                                      
                                                             
                                                                    
                                                                    
  */
	if (!(__builtin_constant_p(sz) && sz == PAGE_SIZE)) {
		sz += paddr & ~ICACHE_LINE_MASK;
		paddr &= ICACHE_LINE_MASK;
		vaddr &= ICACHE_LINE_MASK;
	}

	num_lines = DIV_ROUND_UP(sz, ARC_ICACHE_LINE_LEN);

#if (CONFIG_ARC_MMU_VER <= 2)
	/*                                             */
	paddr |= (vaddr >> PAGE_SHIFT) & 0x1F;
#endif

	local_irq_save(flags);
	while (num_lines-- > 0) {
#if (CONFIG_ARC_MMU_VER > 2)
		/*                         */
		write_aux_reg(ARC_REG_IC_PTAG, paddr);

		/*                            */
		write_aux_reg(ARC_REG_IC_IVIL, vaddr);
		vaddr += ARC_ICACHE_LINE_LEN;
#else
		/*                                    */
		write_aux_reg(ARC_REG_IC_IVIL, paddr);
#endif
		paddr += ARC_ICACHE_LINE_LEN;
	}
	local_irq_restore(flags);
}

#else

#define __ic_line_inv_vaddr(pstart, vstart, sz)

#endif /*                       */


/*                                                          
                
 */

/*
                                                                               
                       
  
                                                                              
                                                                         
                                                                         
                                            
  
                                                                             
                                                                            
 */
void flush_dcache_page(struct page *page)
{
	struct address_space *mapping;

	if (!cache_is_vipt_aliasing()) {
		set_bit(PG_arch_1, &page->flags);
		return;
	}

	/*                              */
	mapping = page_mapping(page);
	if (!mapping)
		return;

	/*
                                                    
                                       
  */
	if (!mapping_mapped(mapping)) {
		set_bit(PG_arch_1, &page->flags);
	} else if (page_mapped(page)) {

		/*                                         */
		void *paddr = page_address(page);
		unsigned long vaddr = page->index << PAGE_CACHE_SHIFT;

		if (addr_not_cache_congruent(paddr, vaddr))
			__flush_dcache_page(paddr, vaddr);
	}
}
EXPORT_SYMBOL(flush_dcache_page);


void dma_cache_wback_inv(unsigned long start, unsigned long sz)
{
	__dc_line_op_k(start, sz, OP_FLUSH_N_INV);
}
EXPORT_SYMBOL(dma_cache_wback_inv);

void dma_cache_inv(unsigned long start, unsigned long sz)
{
	__dc_line_op_k(start, sz, OP_INV);
}
EXPORT_SYMBOL(dma_cache_inv);

void dma_cache_wback(unsigned long start, unsigned long sz)
{
	__dc_line_op_k(start, sz, OP_FLUSH);
}
EXPORT_SYMBOL(dma_cache_wback);

/*
                                                              
                                                   
                                                                    
                                                                    
                                           
 */
void flush_icache_range(unsigned long kstart, unsigned long kend)
{
	unsigned int tot_sz, off, sz;
	unsigned long phy, pfn;

	/*                                                              */

	/*                                                    */
	if (kstart < TASK_SIZE) {
		BUG_ON("Flush icache range for user virtual addr space");
		return;
	}

	/*                                  
                                                             
  */
	tot_sz = kend - kstart;
	if (tot_sz > PAGE_SIZE) {
		flush_cache_all();
		return;
	}

	/*                                             */
	if (likely(kstart > PAGE_OFFSET)) {
		/*
                                                                 
                                                              
                                                             
                      
   */
		__sync_icache_dcache(kstart, kstart, kend - kstart);
		return;
	}

	/*
                                                   
                                                                   
                                 
   
                                                                         
                                                                    
                                                                
  */
	while (tot_sz > 0) {
		off = kstart % PAGE_SIZE;
		pfn = vmalloc_to_pfn((void *)kstart);
		phy = (pfn << PAGE_SHIFT) + off;
		sz = min_t(unsigned int, tot_sz, PAGE_SIZE - off);
		__sync_icache_dcache(phy, kstart, sz);
		kstart += sz;
		tot_sz -= sz;
	}
}

/*
                                                                 
                               
                                                     
                                                                              
                                                                                
                                                                               
                                                                               
                                                                    
 */
void __sync_icache_dcache(unsigned long paddr, unsigned long vaddr, int len)
{
	unsigned long flags;

	local_irq_save(flags);
	__ic_line_inv_vaddr(paddr, vaddr, len);
	__dc_line_op(paddr, vaddr, len, OP_FLUSH_N_INV);
	local_irq_restore(flags);
}

/*                                                                  */
void __inv_icache_page(unsigned long paddr, unsigned long vaddr)
{
	__ic_line_inv_vaddr(paddr, vaddr, PAGE_SIZE);
}

/*
                                                             
                                       
 */
void ___flush_dcache_page(unsigned long paddr, unsigned long vaddr)
{
	__dc_line_op(paddr, vaddr & PAGE_MASK, PAGE_SIZE, OP_FLUSH_N_INV);
}

void flush_icache_all(void)
{
	unsigned long flags;

	local_irq_save(flags);

	write_aux_reg(ARC_REG_IC_IVIC, 1);

	/*                                                                */
	read_aux_reg(ARC_REG_IC_CTRL);
	local_irq_restore(flags);
}

noinline void flush_cache_all(void)
{
	unsigned long flags;

	local_irq_save(flags);

	flush_icache_all();
	__dc_entire_op(OP_FLUSH_N_INV);

	local_irq_restore(flags);

}

#ifdef CONFIG_ARC_CACHE_VIPT_ALIASING

void flush_cache_mm(struct mm_struct *mm)
{
	flush_cache_all();
}

void flush_cache_page(struct vm_area_struct *vma, unsigned long u_vaddr,
		      unsigned long pfn)
{
	unsigned int paddr = pfn << PAGE_SHIFT;

	__sync_icache_dcache(paddr, u_vaddr, PAGE_SIZE);
}

void flush_cache_range(struct vm_area_struct *vma, unsigned long start,
		       unsigned long end)
{
	flush_cache_all();
}

void flush_anon_page(struct vm_area_struct *vma, struct page *page,
		     unsigned long u_vaddr)
{
	/*                                                    */
	__flush_dcache_page(page_address(page), u_vaddr);
	__flush_dcache_page(page_address(page), page_address(page));

}

#endif

void copy_user_highpage(struct page *to, struct page *from,
	unsigned long u_vaddr, struct vm_area_struct *vma)
{
	void *kfrom = page_address(from);
	void *kto = page_address(to);
	int clean_src_k_mappings = 0;

	/*
                                                                     
                                                                      
                                                  
   
                                                                        
                                      
  */
	if (page_mapped(from) && addr_not_cache_congruent(kfrom, u_vaddr)) {
		__flush_dcache_page(kfrom, u_vaddr);
		clean_src_k_mappings = 1;
	}

	copy_page(kto, kfrom);

	/*
                                                                
                                                                      
                                                             
                                                                
                                                                        
              
  */
	set_bit(PG_arch_1, &to->flags);

	/*
                                                                     
                                                 
  */
	if (clean_src_k_mappings) {
		__flush_dcache_page(kfrom, kfrom);
	} else {
		set_bit(PG_arch_1, &from->flags);
	}
}

void clear_user_page(void *to, unsigned long u_vaddr, struct page *page)
{
	clear_page(to);
	set_bit(PG_arch_1, &page->flags);
}


/*                                                                     
                                                           
                                                 
 */
SYSCALL_DEFINE3(cacheflush, uint32_t, start, uint32_t, sz, uint32_t, flags)
{
	/*                    */
	flush_cache_all();
	return 0;
}
