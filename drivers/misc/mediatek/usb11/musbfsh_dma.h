/*
 * MUSB OTG driver DMA controller abstraction
 *
 * Copyright 2005 Mentor Graphics Corporation
 * Copyright (C) 2005-2006 by Texas Instruments
 * Copyright (C) 2006-2007 Nokia Corporation
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
 * NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef __MUSBFSH_DMA_H__
#define __MUSBFSH_DMA_H__

struct musbfsh_hw_ep;

/*
                             
  
                                                                        
                                                                        
                                                                         
                                                                        
                                      
  
                                                         
                                                                     
                                                                    
                                                 
  
                                                          
                                                                  
  
                                                         
  
                                            
                                                        
 */

#define	DMA_ADDR_INVALID	(~(dma_addr_t)0)

#ifndef CONFIG_MUSBFSH_PIO_ONLY
#define	is_dma_capable()	(1)
#else
#define	is_dma_capable()	(0)
#endif

/*
                                                                            
                                                                    
 */
enum dma_channel_status {
	/*             */
	MUSBFSH_DMA_STATUS_UNKNOWN,
	/*                                       */
	MUSBFSH_DMA_STATUS_FREE,
	/*                                  */
	MUSBFSH_DMA_STATUS_BUSY,
	/*                                                           */
	MUSBFSH_DMA_STATUS_BUS_ABORT,
	/*                                                           */
	MUSBFSH_DMA_STATUS_CORE_ABORT
};

struct dma_controller;

/* 
                                      
                                      
                                                                    
                                                                      
                                                    
                                                              
                                                                       
  
                                                                        
                    
 */
struct dma_channel {
	void			*private_data;
	/*                                                      */
	size_t			max_len;
	size_t			actual_len;
	enum dma_channel_status	status;
	bool			desired_mode;
};

/*
                                                    
                  
  
                                                                             
                                                                              
                                             
 */
static inline enum dma_channel_status
dma_channel_status(struct dma_channel *c)
{
	return (is_dma_capable() && c) ? c->status : MUSBFSH_DMA_STATUS_UNKNOWN;
}

/* 
                                            
                                               
                                           
                                            
                                           
                                                      
                                                       
                                                                
                                             
  
                                   
 */
struct dma_controller {
	int			(*start)(struct dma_controller *);
	int			(*stop)(struct dma_controller *);
	struct dma_channel	*(*channel_alloc)(struct dma_controller *,
					struct musbfsh_hw_ep *, u8 is_tx);
	void			(*channel_release)(struct dma_channel *);
	int			(*channel_program)(struct dma_channel *channel,
							u16 maxpacket, u8 mode,
							dma_addr_t dma_addr,
							u32 length);
	int			(*channel_abort)(struct dma_channel *);
};

/*                                                      */
extern void musbfsh_dma_completion(struct musbfsh *musb, u8 epnum, u8 transmit);


extern struct dma_controller *__init
musbfsh_dma_controller_create(struct musbfsh *, void __iomem *);

extern void musbfsh_dma_controller_destroy(struct dma_controller *);

#endif	/*                   */
