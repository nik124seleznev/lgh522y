#ifndef _LINUX_VIRTIO_RING_H
#define _LINUX_VIRTIO_RING_H

#include <linux/irqreturn.h>
#include <uapi/linux/virtio_ring.h>

/*
                                                                     
                                                                 
                                                                    
               
  
                                                                     
                                                                     
              
  
                                                                    
                                                                     
                                                                 
                        
 */

#ifdef CONFIG_SMP
static inline void virtio_mb(bool weak_barriers)
{
	if (weak_barriers)
		smp_mb();
	else
		mb();
}

static inline void virtio_rmb(bool weak_barriers)
{
	if (weak_barriers)
		smp_rmb();
	else
		rmb();
}

static inline void virtio_wmb(bool weak_barriers)
{
	if (weak_barriers)
		smp_wmb();
	else
		wmb();
}
#else
static inline void virtio_mb(bool weak_barriers)
{
	mb();
}

static inline void virtio_rmb(bool weak_barriers)
{
	rmb();
}

static inline void virtio_wmb(bool weak_barriers)
{
	wmb();
}
#endif

struct virtio_device;
struct virtqueue;

struct virtqueue *vring_new_virtqueue(unsigned int index,
				      unsigned int num,
				      unsigned int vring_align,
				      struct virtio_device *vdev,
				      bool weak_barriers,
				      void *pages,
				      void (*notify)(struct virtqueue *vq),
				      void (*callback)(struct virtqueue *vq),
				      const char *name);
void vring_del_virtqueue(struct virtqueue *vq);
/*                                             */
void vring_transport_features(struct virtio_device *vdev);

irqreturn_t vring_interrupt(int irq, void *_vq);
#endif /*                      */
