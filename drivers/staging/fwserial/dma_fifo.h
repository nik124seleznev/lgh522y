/*
 * DMA-able FIFO interface
 *
 * Copyright (C) 2012 Peter Hurley <peter@hurleysoftware.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef _DMA_FIFO_H_
#define _DMA_FIFO_H_

/* 
                                                                      
                                                                             
                                                                            
                                                                             
                                 
  
                                                                        
                                                                          
                                                                        
                                                                               
  
                                                                          
                                                                             
                                                                     
                                
                                                                      
                                                                           
                                                                             
  
                                                                   
                                                                                
                                                                              
 */

#define DMA_FIFO_GUARD 3   /*                                                */

struct dma_fifo {
	unsigned	 in;
	unsigned	 out;		/*                                    */
	unsigned	 done;		/*                                    */
	struct {
		unsigned corrupt:1;
	};
	int		 size;		/*                               */
	int		 guard;		/*                          */
	int		 capacity;	/*                                    */
	int		 avail;		/*                                    */
	unsigned	 align;		/*                                    */
	int		 tx_limit;	/*                                    */
	int		 open_limit;	/*                                    */
	int		 open;		/*                                    */
	struct list_head pending;	/*                                    */
	void		 *data;
};

struct dma_pending {
	struct list_head link;
	void		 *data;
	unsigned	 len;
	unsigned         next;
	unsigned         out;
};

static inline void dp_mark_completed(struct dma_pending *dp)
{
	dp->data += 1;
}

static inline bool dp_is_completed(struct dma_pending *dp)
{
	return (unsigned long)dp->data & 1UL;
}

extern void dma_fifo_init(struct dma_fifo *fifo);
extern int dma_fifo_alloc(struct dma_fifo *fifo, int size, unsigned align,
			  int tx_limit, int open_limit, gfp_t gfp_mask);
extern void dma_fifo_free(struct dma_fifo *fifo);
extern void dma_fifo_reset(struct dma_fifo *fifo);
extern int dma_fifo_in(struct dma_fifo *fifo, const void *src, int n);
extern int dma_fifo_out_pend(struct dma_fifo *fifo, struct dma_pending *pended);
extern int dma_fifo_out_complete(struct dma_fifo *fifo,
				 struct dma_pending *complete);

/*                                         */
static inline int dma_fifo_level(struct dma_fifo *fifo)
{
	return fifo->size - fifo->avail;
}

/*                                                     */
static inline int dma_fifo_out_level(struct dma_fifo *fifo)
{
	return fifo->in - fifo->out;
}

/*                                           */
static inline int dma_fifo_avail(struct dma_fifo *fifo)
{
	return fifo->avail;
}

/*                                                    */
static inline bool dma_fifo_busy(struct dma_fifo *fifo)
{
	return fifo->open == fifo->open_limit;
}

/*                                                               */
static inline int dma_fifo_change_tx_limit(struct dma_fifo *fifo, int tx_limit)
{
	tx_limit = round_down(tx_limit, fifo->align);
	fifo->tx_limit = max_t(int, tx_limit, fifo->align);
	return 0;
}

#endif /*              */
