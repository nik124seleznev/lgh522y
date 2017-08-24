/*
	Copyright (C) 2004 - 2009 Ivo van Doorn <IvDoorn@gmail.com>
	<http://rt2x00.serialmonkey.com>

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the
	Free Software Foundation, Inc.,
	59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*
                   
                                                     
 */

#ifndef RT2X00MMIO_H
#define RT2X00MMIO_H

#include <linux/io.h>

/*
                   
 */
static inline void rt2x00mmio_register_read(struct rt2x00_dev *rt2x00dev,
					    const unsigned int offset,
					    u32 *value)
{
	*value = readl(rt2x00dev->csr.base + offset);
}

static inline void rt2x00mmio_register_multiread(struct rt2x00_dev *rt2x00dev,
						 const unsigned int offset,
						 void *value, const u32 length)
{
	memcpy_fromio(value, rt2x00dev->csr.base + offset, length);
}

static inline void rt2x00mmio_register_write(struct rt2x00_dev *rt2x00dev,
					     const unsigned int offset,
					     u32 value)
{
	writel(value, rt2x00dev->csr.base + offset);
}

static inline void rt2x00mmio_register_multiwrite(struct rt2x00_dev *rt2x00dev,
						  const unsigned int offset,
						  const void *value,
						  const u32 length)
{
	__iowrite32_copy(rt2x00dev->csr.base + offset, value, length >> 2);
}

/* 
                                                               
                                                      
                           
                                             
                                                            
  
                                                                
                                                            
                                                                  
                                                                 
         
 */
int rt2x00mmio_regbusy_read(struct rt2x00_dev *rt2x00dev,
			    const unsigned int offset,
			    const struct rt2x00_field32 field,
			    u32 *reg);

/* 
                                                                   
  
                                      
                                   
                                           
                                   
 */
struct queue_entry_priv_mmio {
	__le32 *desc;
	dma_addr_t desc_dma;
};

/* 
                                            
                                                      
  
                                                                     
                                    
 */
bool rt2x00mmio_rxdone(struct rt2x00_dev *rt2x00dev);

/* 
                                            
                             
                                          
  
                                                                
                   
 */
void rt2x00mmio_flush_queue(struct data_queue *queue, bool drop);

/*
                                  
 */
int rt2x00mmio_initialize(struct rt2x00_dev *rt2x00dev);
void rt2x00mmio_uninitialize(struct rt2x00_dev *rt2x00dev);

#endif /*              */
