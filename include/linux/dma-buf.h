/*
 * Header file for dma buffer sharing framework.
 *
 * Copyright(C) 2011 Linaro Limited. All rights reserved.
 * Author: Sumit Semwal <sumit.semwal@ti.com>
 *
 * Many thanks to linaro-mm-sig list, and specially
 * Arnd Bergmann <arnd@arndb.de>, Rob Clark <rob@ti.com> and
 * Daniel Vetter <daniel@ffwll.ch> for their support in creation and
 * refining of this idea.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef __DMA_BUF_H__
#define __DMA_BUF_H__

#include <linux/file.h>
#include <linux/err.h>
#include <linux/scatterlist.h>
#include <linux/list.h>
#include <linux/dma-mapping.h>
#include <linux/fs.h>

struct device;
struct dma_buf;
struct dma_buf_attachment;

/* 
                                                             
                                                                             
                                                                          
                                                                  
                            
                                                              
                                                                            
                                                            
                                                           
                                                                 
                                                                    
                                                                         
             
                                                                          
                                                                           
                                                                
                                                  
                                                                       
                                                                
                                                                 
                                   
                                                                              
                                     
                                                                
                                                     
                                                                        
                                                                     
                                                                        
                                                  
                                                                         
                                                                    
                                                    
 */
struct dma_buf_ops {
	int (*attach)(struct dma_buf *, struct device *,
			struct dma_buf_attachment *);

	void (*detach)(struct dma_buf *, struct dma_buf_attachment *);

	/*                                                              
                                                                 
                        
  */
	struct sg_table * (*map_dma_buf)(struct dma_buf_attachment *,
						enum dma_data_direction);
	void (*unmap_dma_buf)(struct dma_buf_attachment *,
						struct sg_table *,
						enum dma_data_direction);
	/*                                                               
                            
  */

	/*                           */
	void (*release)(struct dma_buf *);

	int (*begin_cpu_access)(struct dma_buf *, size_t, size_t,
				enum dma_data_direction);
	void (*end_cpu_access)(struct dma_buf *, size_t, size_t,
			       enum dma_data_direction);
	void *(*kmap_atomic)(struct dma_buf *, unsigned long);
	void (*kunmap_atomic)(struct dma_buf *, unsigned long, void *);
	void *(*kmap)(struct dma_buf *, unsigned long);
	void (*kunmap)(struct dma_buf *, unsigned long, void *);

	int (*mmap)(struct dma_buf *, struct vm_area_struct *vma);

	void *(*vmap)(struct dma_buf *);
	void (*vunmap)(struct dma_buf *, void *vaddr);
};

/* 
                                        
                            
                                                                            
                                                                              
                                                        
                                                         
                                                         
                                                                
 */
struct dma_buf {
	size_t size;
	struct file *file;
	struct list_head attachments;
	const struct dma_buf_ops *ops;
	/*                                                                    */
	struct mutex lock;
	unsigned vmapping_counter;
	void *vmap_ptr;
	const char *exp_name;
	struct list_head list_node;
	void *priv;
};

/* 
                                                                  
                                       
                                       
                                     
                                            
  
                                                                             
                                                                             
                          
 */
struct dma_buf_attachment {
	struct dma_buf *dmabuf;
	struct device *dev;
	struct list_head node;
	void *priv;
};

/* 
                                                  
                                   
  
                                                                           
                                                                        
                                                                         
                                                        
 */
static inline void get_dma_buf(struct dma_buf *dmabuf)
{
	get_file(dmabuf->file);
}

struct dma_buf_attachment *dma_buf_attach(struct dma_buf *dmabuf,
							struct device *dev);
void dma_buf_detach(struct dma_buf *dmabuf,
				struct dma_buf_attachment *dmabuf_attach);

struct dma_buf *dma_buf_export_named(void *priv, const struct dma_buf_ops *ops,
			       size_t size, int flags, const char *);

#define dma_buf_export(priv, ops, size, flags)	\
	dma_buf_export_named(priv, ops, size, flags, __FILE__)

int dma_buf_fd(struct dma_buf *dmabuf, int flags);
struct dma_buf *dma_buf_get(int fd);
void dma_buf_put(struct dma_buf *dmabuf);

struct sg_table *dma_buf_map_attachment(struct dma_buf_attachment *,
					enum dma_data_direction);
void dma_buf_unmap_attachment(struct dma_buf_attachment *, struct sg_table *,
				enum dma_data_direction);
int dma_buf_begin_cpu_access(struct dma_buf *dma_buf, size_t start, size_t len,
			     enum dma_data_direction dir);
void dma_buf_end_cpu_access(struct dma_buf *dma_buf, size_t start, size_t len,
			    enum dma_data_direction dir);
void *dma_buf_kmap_atomic(struct dma_buf *, unsigned long);
void dma_buf_kunmap_atomic(struct dma_buf *, unsigned long, void *);
void *dma_buf_kmap(struct dma_buf *, unsigned long);
void dma_buf_kunmap(struct dma_buf *, unsigned long, void *);

int dma_buf_mmap(struct dma_buf *, struct vm_area_struct *,
		 unsigned long);
void *dma_buf_vmap(struct dma_buf *);
void dma_buf_vunmap(struct dma_buf *, void *vaddr);
int dma_buf_debugfs_create_file(const char *name,
				int (*write)(struct seq_file *));
#endif /*               */
