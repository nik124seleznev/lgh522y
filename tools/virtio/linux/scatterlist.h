#ifndef SCATTERLIST_H
#define SCATTERLIST_H
#include <linux/kernel.h>

struct scatterlist {
	unsigned long	page_link;
	unsigned int	offset;
	unsigned int	length;
	dma_addr_t	dma_address;
};

/*                                                      */
#define sg_is_chain(sg)		((sg)->page_link & 0x01)
#define sg_is_last(sg)		((sg)->page_link & 0x02)
#define sg_chain_ptr(sg)	\
	((struct scatterlist *) ((sg)->page_link & ~0x03))

/* 
                                                      
                     
                      
  
               
                                                                            
             
  
  */
static inline void sg_assign_page(struct scatterlist *sg, struct page *page)
{
	unsigned long page_link = sg->page_link & 0x3;

	/*
                                                             
                                                      
  */
	BUG_ON((unsigned long) page & 0x03);
#ifdef CONFIG_DEBUG_SG
	BUG_ON(sg->sg_magic != SG_MAGIC);
	BUG_ON(sg_is_chain(sg));
#endif
	sg->page_link = page_link | (unsigned long) page;
}

/* 
                                                    
                  
                   
                        
                             
  
               
                                                                          
                                                                        
                                                                         
                    
  
  */
static inline void sg_set_page(struct scatterlist *sg, struct page *page,
			       unsigned int len, unsigned int offset)
{
	sg_assign_page(sg, page);
	sg->offset = offset;
	sg->length = len;
}

static inline struct page *sg_page(struct scatterlist *sg)
{
#ifdef CONFIG_DEBUG_SG
	BUG_ON(sg->sg_magic != SG_MAGIC);
	BUG_ON(sg_is_chain(sg));
#endif
	return (struct page *)((sg)->page_link & ~0x3);
}

/*
                                                                              
 */
#define for_each_sg(sglist, sg, nr, __i)	\
	for (__i = 0, sg = (sglist); __i < (nr); __i++, sg = sg_next(sg))

/* 
                                        
                          
                                       
                           
  
               
                                                                  
  
  */
static inline void sg_chain(struct scatterlist *prv, unsigned int prv_nents,
			    struct scatterlist *sgl)
{
	/*
                                                              
  */
	prv[prv_nents - 1].offset = 0;
	prv[prv_nents - 1].length = 0;

	/*
                                                                     
                                                
  */
	prv[prv_nents - 1].page_link = ((unsigned long) sgl | 0x01) & ~0x02;
}

/* 
                                                
                             
  
               
                                                                     
                                                               
  
  */
static inline void sg_mark_end(struct scatterlist *sg)
{
#ifdef CONFIG_DEBUG_SG
	BUG_ON(sg->sg_magic != SG_MAGIC);
#endif
	/*
                                                  
  */
	sg->page_link |= 0x02;
	sg->page_link &= ~0x01;
}

/* 
                                                          
                             
  
               
                                                                            
  
  */
static inline void sg_unmark_end(struct scatterlist *sg)
{
#ifdef CONFIG_DEBUG_SG
	BUG_ON(sg->sg_magic != SG_MAGIC);
#endif
	sg->page_link &= ~0x02;
}

static inline struct scatterlist *sg_next(struct scatterlist *sg)
{
#ifdef CONFIG_DEBUG_SG
	BUG_ON(sg->sg_magic != SG_MAGIC);
#endif
	if (sg_is_last(sg))
		return NULL;

	sg++;
	if (unlikely(sg_is_chain(sg)))
		sg = sg_chain_ptr(sg);

	return sg;
}

static inline void sg_init_table(struct scatterlist *sgl, unsigned int nents)
{
	memset(sgl, 0, sizeof(*sgl) * nents);
#ifdef CONFIG_DEBUG_SG
	{
		unsigned int i;
		for (i = 0; i < nents; i++)
			sgl[i].sg_magic = SG_MAGIC;
	}
#endif
	sg_mark_end(&sgl[nents - 1]);
}

static inline dma_addr_t sg_phys(struct scatterlist *sg)
{
	return page_to_phys(sg_page(sg)) + sg->offset;
}

static inline void sg_set_buf(struct scatterlist *sg, const void *buf,
			      unsigned int buflen)
{
	sg_set_page(sg, virt_to_page(buf), buflen, offset_in_page(buf));
}

static inline void sg_init_one(struct scatterlist *sg,
			       const void *buf, unsigned int buflen)
{
	sg_init_table(sg, 1);
	sg_set_buf(sg, buf, buflen);
}
#endif /*               */
