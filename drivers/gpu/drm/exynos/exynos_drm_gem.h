/* exynos_drm_gem.h
 *
 * Copyright (c) 2011 Samsung Electronics Co., Ltd.
 * Authoer: Inki Dae <inki.dae@samsung.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#ifndef _EXYNOS_DRM_GEM_H_
#define _EXYNOS_DRM_GEM_H_

#define to_exynos_gem_obj(x)	container_of(x,\
			struct exynos_drm_gem_obj, base)

#define IS_NONCONTIG_BUFFER(f)		(f & EXYNOS_BO_NONCONTIG)

/*
                                   
  
                                                              
                                
                                                                      
                                                             
                             
                                                          
                                  
                                        
                                          
                                                                      
                    
 */
struct exynos_drm_gem_buf {
	void __iomem		*kvaddr;
	unsigned long		userptr;
	dma_addr_t		dma_addr;
	struct dma_attrs	dma_attrs;
	unsigned int		write;
	struct page		**pages;
	struct sg_table		*sgt;
	unsigned long		size;
	bool			pfnmap;
};

/*
                               
  
                       
                                                     
                              
                                                      
                                                       
                                              
                                                     
                              
                                                                     
                
                              
                                                                         
  
                                                                   
                                                    
 */
struct exynos_drm_gem_obj {
	struct drm_gem_object		base;
	struct exynos_drm_gem_buf	*buffer;
	unsigned long			size;
	struct vm_area_struct		*vma;
	unsigned int			flags;
};

struct page **exynos_gem_get_pages(struct drm_gem_object *obj, gfp_t gfpmask);

/*                                  */
void exynos_drm_gem_destroy(struct exynos_drm_gem_obj *exynos_gem_obj);

/*                                                */
struct exynos_drm_gem_obj *exynos_drm_gem_init(struct drm_device *dev,
						      unsigned long size);

/*                                     */
struct exynos_drm_gem_obj *exynos_drm_gem_create(struct drm_device *dev,
						unsigned int flags,
						unsigned long size);

/*
                                                                
                                                                    
                  
 */
int exynos_drm_gem_create_ioctl(struct drm_device *dev, void *data,
				struct drm_file *file_priv);

/*
                                                                      
                                                    
                                                                          
 */
dma_addr_t *exynos_drm_gem_get_dma_addr(struct drm_device *dev,
					unsigned int gem_handle,
					struct drm_file *filp);

/*
                                                                      
                                                    
                                                                          
 */
void exynos_drm_gem_put_dma_addr(struct drm_device *dev,
					unsigned int gem_handle,
					struct drm_file *filp);

/*                                         */
int exynos_drm_gem_map_offset_ioctl(struct drm_device *dev, void *data,
				    struct drm_file *file_priv);

/*
                                                                   
                 
 */
int exynos_drm_gem_mmap_ioctl(struct drm_device *dev, void *data,
			      struct drm_file *file_priv);

/*                                              */
int exynos_drm_gem_userptr_ioctl(struct drm_device *dev, void *data,
				      struct drm_file *file_priv);

/*                                                           */
int exynos_drm_gem_get_ioctl(struct drm_device *dev, void *data,
				      struct drm_file *file_priv);

/*                                */
unsigned long exynos_drm_gem_get_size(struct drm_device *dev,
						unsigned int gem_handle,
						struct drm_file *file_priv);

/*                        */
int exynos_drm_gem_init_object(struct drm_gem_object *obj);

/*                  */
void exynos_drm_gem_free_object(struct drm_gem_object *gem_obj);

/*                                           */
int exynos_drm_gem_dumb_create(struct drm_file *file_priv,
			       struct drm_device *dev,
			       struct drm_mode_create_dumb *args);

/*                                                      */
int exynos_drm_gem_dumb_map_offset(struct drm_file *file_priv,
				   struct drm_device *dev, uint32_t handle,
				   uint64_t *offset);

/*
                                   
                                                                    
                                                
 */
int exynos_drm_gem_dumb_destroy(struct drm_file *file_priv,
				struct drm_device *dev,
				unsigned int handle);

/*                                                                        */
int exynos_drm_gem_fault(struct vm_area_struct *vma, struct vm_fault *vmf);

/*                                                                       */
int exynos_drm_gem_mmap(struct file *filp, struct vm_area_struct *vma);

static inline int vma_is_io(struct vm_area_struct *vma)
{
	return !!(vma->vm_flags & (VM_IO | VM_PFNMAP));
}

/*                                        */
struct vm_area_struct *exynos_gem_get_vma(struct vm_area_struct *vma);

/*                                          */
void exynos_gem_put_vma(struct vm_area_struct *vma);

/*                            */
int exynos_gem_get_pages_from_userptr(unsigned long start,
						unsigned int npages,
						struct page **pages,
						struct vm_area_struct *vma);

/*                              */
void exynos_gem_put_pages_to_userptr(struct page **pages,
					unsigned int npages,
					struct vm_area_struct *vma);

/*                          */
int exynos_gem_map_sgt_with_dma(struct drm_device *drm_dev,
				struct sg_table *sgt,
				enum dma_data_direction dir);

/*                            */
void exynos_gem_unmap_sgt_from_dma(struct drm_device *drm_dev,
				struct sg_table *sgt,
				enum dma_data_direction dir);

#endif
