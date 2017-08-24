#ifndef _LINUX_VIRTIO_CONFIG_H
#define _LINUX_VIRTIO_CONFIG_H

#include <linux/err.h>
#include <linux/bug.h>
#include <linux/virtio.h>
#include <uapi/linux/virtio_config.h>

/* 
                                                                 
                                                
                          
                                                
                                                 
                                
                                                 
                          
                                                
                                                
                                
                                    
                          
                          
                                     
                          
                              
                           
                          
                                                                
                                                               
                                     
                                                   
                          
                                         
                                           
                                                    
                                                            
                                                         
                                                 
                                       
                                                 
                                                                
                          
                                                             
                                                                   
                          
                                                                  
                                     
                                                            
                          
                                                                       
                                 
                                                      
 */
typedef void vq_callback_t(struct virtqueue *);
struct virtio_config_ops {
	void (*get)(struct virtio_device *vdev, unsigned offset,
		    void *buf, unsigned len);
	void (*set)(struct virtio_device *vdev, unsigned offset,
		    const void *buf, unsigned len);
	u8 (*get_status)(struct virtio_device *vdev);
	void (*set_status)(struct virtio_device *vdev, u8 status);
	void (*reset)(struct virtio_device *vdev);
	int (*find_vqs)(struct virtio_device *, unsigned nvqs,
			struct virtqueue *vqs[],
			vq_callback_t *callbacks[],
			const char *names[]);
	void (*del_vqs)(struct virtio_device *);
	u32 (*get_features)(struct virtio_device *vdev);
	void (*finalize_features)(struct virtio_device *vdev);
	const char *(*bus_name)(struct virtio_device *vdev);
	int (*set_vq_affinity)(struct virtqueue *vq, int cpu);
};

/*                                                               */
void virtio_check_driver_offered_feature(const struct virtio_device *vdev,
					 unsigned int fbit);

/* 
                                                                            
                    
                         
 */
static inline bool virtio_has_feature(const struct virtio_device *vdev,
				      unsigned int fbit)
{
	/*                                                    */
	if (__builtin_constant_p(fbit))
		BUILD_BUG_ON(fbit >= 32);
	else
		BUG_ON(fbit >= 32);

	if (fbit < VIRTIO_TRANSPORT_F_START)
		virtio_check_driver_offered_feature(vdev, fbit);

	return test_bit(fbit, vdev->features);
}

/* 
                                                                        
                           
                         
                                   
                                         
  
                                                                       
                                                                */
#define virtio_config_val(vdev, fbit, offset, v) \
	virtio_config_buf((vdev), (fbit), (offset), (v), sizeof(*v))

#define virtio_config_val_len(vdev, fbit, offset, v, len) \
	virtio_config_buf((vdev), (fbit), (offset), (v), (len))

static inline int virtio_config_buf(struct virtio_device *vdev,
				    unsigned int fbit,
				    unsigned int offset,
				    void *buf, unsigned len)
{
	if (!virtio_has_feature(vdev, fbit))
		return -ENOENT;

	vdev->config->get(vdev, offset, buf, len);
	return 0;
}

static inline
struct virtqueue *virtio_find_single_vq(struct virtio_device *vdev,
					vq_callback_t *c, const char *n)
{
	vq_callback_t *callbacks[] = { c };
	const char *names[] = { n };
	struct virtqueue *vq;
	int err = vdev->config->find_vqs(vdev, 1, &vq, callbacks, names);
	if (err < 0)
		return ERR_PTR(err);
	return vq;
}

static inline
const char *virtio_bus_name(struct virtio_device *vdev)
{
	if (!vdev->config->bus_name)
		return "virtio";
	return vdev->config->bus_name(vdev);
}

/* 
                                                            
                     
                    
  
                                                                               
                                               
  
 */
static inline
int virtqueue_set_affinity(struct virtqueue *vq, int cpu)
{
	struct virtio_device *vdev = vq->vdev;
	if (vdev->config->set_vq_affinity)
		return vdev->config->set_vq_affinity(vq, cpu);
	return 0;
}


#endif /*                        */
