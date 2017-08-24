/* Virtio ring implementation.
 *
 *  Copyright 2007 Rusty Russell IBM Corporation
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include <linux/virtio.h>
#include <linux/virtio_ring.h>
#include <linux/virtio_config.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/hrtimer.h>

#ifdef DEBUG
/*                                                                 */
#define BAD_RING(_vq, fmt, args...)				\
	do {							\
		dev_err(&(_vq)->vq.vdev->dev,			\
			"%s:"fmt, (_vq)->vq.name, ##args);	\
		BUG();						\
	} while (0)
/*                                             */
#define START_USE(_vq)						\
	do {							\
		if ((_vq)->in_use)				\
			panic("%s:in_use = %i\n",		\
			      (_vq)->vq.name, (_vq)->in_use);	\
		(_vq)->in_use = __LINE__;			\
	} while (0)
#define END_USE(_vq) \
	do { BUG_ON(!(_vq)->in_use); (_vq)->in_use = 0; } while(0)
#else
#define BAD_RING(_vq, fmt, args...)				\
	do {							\
		dev_err(&_vq->vq.vdev->dev,			\
			"%s:"fmt, (_vq)->vq.name, ##args);	\
		(_vq)->broken = true;				\
	} while (0)
#define START_USE(vq)
#define END_USE(vq)
#endif

struct vring_virtqueue
{
	struct virtqueue vq;

	/*                                     */
	struct vring vring;

	/*                           */
	bool weak_barriers;

	/*                                                 */
	bool broken;

	/*                                */
	bool indirect;

	/*                                */
	bool event;

	/*                           */
	unsigned int free_head;
	/*                                     */
	unsigned int num_added;

	/*                             */
	u16 last_used_idx;

	/*                                                      */
	void (*notify)(struct virtqueue *vq);

#ifdef DEBUG
	/*                                  */
	unsigned int in_use;

	/*                                            */
	bool last_add_time_valid;
	ktime_t last_add_time;
#endif

	/*                       */
	void *data[];
};

#define to_vvq(_vq) container_of(_vq, struct vring_virtqueue, vq)

static inline struct scatterlist *sg_next_chained(struct scatterlist *sg,
						  unsigned int *count)
{
	return sg_next(sg);
}

static inline struct scatterlist *sg_next_arr(struct scatterlist *sg,
					      unsigned int *count)
{
	if (--(*count) == 0)
		return NULL;
	return sg + 1;
}

/*                                                                  */
static inline int vring_add_indirect(struct vring_virtqueue *vq,
				     struct scatterlist *sgs[],
				     struct scatterlist *(*next)
				       (struct scatterlist *, unsigned int *),
				     unsigned int total_sg,
				     unsigned int total_out,
				     unsigned int total_in,
				     unsigned int out_sgs,
				     unsigned int in_sgs,
				     gfp_t gfp)
{
	struct vring_desc *desc;
	unsigned head;
	struct scatterlist *sg;
	int i, n;

	/*
                                                          
                                                              
              
  */
	gfp &= ~(__GFP_HIGHMEM | __GFP_HIGH);

	desc = kmalloc(total_sg * sizeof(struct vring_desc), gfp);
	if (!desc)
		return -ENOMEM;

	/*                                                           */
	i = 0;
	for (n = 0; n < out_sgs; n++) {
		for (sg = sgs[n]; sg; sg = next(sg, &total_out)) {
			desc[i].flags = VRING_DESC_F_NEXT;
			desc[i].addr = sg_phys(sg);
			desc[i].len = sg->length;
			desc[i].next = i+1;
			i++;
		}
	}
	for (; n < (out_sgs + in_sgs); n++) {
		for (sg = sgs[n]; sg; sg = next(sg, &total_in)) {
			desc[i].flags = VRING_DESC_F_NEXT|VRING_DESC_F_WRITE;
			desc[i].addr = sg_phys(sg);
			desc[i].len = sg->length;
			desc[i].next = i+1;
			i++;
		}
	}
	BUG_ON(i != total_sg);

	/*                            */
	desc[i-1].flags &= ~VRING_DESC_F_NEXT;
	desc[i-1].next = 0;

	/*                             */
	vq->vq.num_free--;

	/*                                            */
	head = vq->free_head;
	vq->vring.desc[head].flags = VRING_DESC_F_INDIRECT;
	vq->vring.desc[head].addr = virt_to_phys(desc);
	vq->vring.desc[head].len = i * sizeof(struct vring_desc);

	/*                     */
	vq->free_head = vq->vring.desc[head].next;

	return head;
}

static inline int virtqueue_add(struct virtqueue *_vq,
				struct scatterlist *sgs[],
				struct scatterlist *(*next)
				  (struct scatterlist *, unsigned int *),
				unsigned int total_out,
				unsigned int total_in,
				unsigned int out_sgs,
				unsigned int in_sgs,
				void *data,
				gfp_t gfp)
{
	struct vring_virtqueue *vq = to_vvq(_vq);
	struct scatterlist *sg;
	unsigned int i, n, avail, uninitialized_var(prev), total_sg;
	int head;

	START_USE(vq);

	BUG_ON(data == NULL);

#ifdef DEBUG
	{
		ktime_t now = ktime_get();

		/*                                                */
		if (vq->last_add_time_valid)
			WARN_ON(ktime_to_ms(ktime_sub(now, vq->last_add_time))
					    > 100);
		vq->last_add_time = now;
		vq->last_add_time_valid = true;
	}
#endif

	total_sg = total_in + total_out;

	/*                                                                      
                                                          */
	if (vq->indirect && total_sg > 1 && vq->vq.num_free) {
		head = vring_add_indirect(vq, sgs, next, total_sg, total_out,
					  total_in,
					  out_sgs, in_sgs, gfp);
		if (likely(head >= 0))
			goto add_head;
	}

	BUG_ON(total_sg > vq->vring.num);
	BUG_ON(total_sg == 0);

	if (vq->vq.num_free < total_sg) {
		pr_debug("Can't add buf len %i - avail = %i\n",
			 total_sg, vq->vq.num_free);
		/*                                                         
                                                            
                                        */
		if (out_sgs)
			vq->notify(&vq->vq);
		END_USE(vq);
		return -ENOSPC;
	}

	/*                                                     */
	vq->vq.num_free -= total_sg;

	head = i = vq->free_head;
	for (n = 0; n < out_sgs; n++) {
		for (sg = sgs[n]; sg; sg = next(sg, &total_out)) {
			vq->vring.desc[i].flags = VRING_DESC_F_NEXT;
			vq->vring.desc[i].addr = sg_phys(sg);
			vq->vring.desc[i].len = sg->length;
			prev = i;
			i = vq->vring.desc[i].next;
		}
	}
	for (; n < (out_sgs + in_sgs); n++) {
		for (sg = sgs[n]; sg; sg = next(sg, &total_in)) {
			vq->vring.desc[i].flags = VRING_DESC_F_NEXT|VRING_DESC_F_WRITE;
			vq->vring.desc[i].addr = sg_phys(sg);
			vq->vring.desc[i].len = sg->length;
			prev = i;
			i = vq->vring.desc[i].next;
		}
	}
	/*                            */
	vq->vring.desc[prev].flags &= ~VRING_DESC_F_NEXT;

	/*                     */
	vq->free_head = i;

add_head:
	/*            */
	vq->data[head] = data;

	/*                                                                     
              */
	avail = (vq->vring.avail->idx & (vq->vring.num-1));
	vq->vring.avail->ring[avail] = head;

	/*                                                                    
                                 */
	virtio_wmb(vq->weak_barriers);
	vq->vring.avail->idx++;
	vq->num_added++;

	/*                                                         
                  */
	if (unlikely(vq->num_added == (1 << 16) - 1))
		virtqueue_kick(_vq);

	pr_debug("Added buffer head %i to %p\n", head, vq);
	END_USE(vq);

	return 0;
}

/* 
                                                 
                                                 
                                         
                                                    
                                                                     
                                           
                                                     
  
                                                                        
                                         
  
                                                         
 */
int virtqueue_add_buf(struct virtqueue *_vq,
		      struct scatterlist sg[],
		      unsigned int out,
		      unsigned int in,
		      void *data,
		      gfp_t gfp)
{
	struct scatterlist *sgs[2];

	sgs[0] = sg;
	sgs[1] = sg + out;

	return virtqueue_add(_vq, sgs, sg_next_arr,
			     out, in, out ? 1 : 0, in ? 1 : 0, data, gfp);
}
EXPORT_SYMBOL_GPL(virtqueue_add_buf);

/* 
                                                  
                                                 
                                          
                                                              
                                                                               
                                           
                                                     
  
                                                                        
                                         
  
                                                         
 */
int virtqueue_add_sgs(struct virtqueue *_vq,
		      struct scatterlist *sgs[],
		      unsigned int out_sgs,
		      unsigned int in_sgs,
		      void *data,
		      gfp_t gfp)
{
	unsigned int i, total_out, total_in;

	/*                   */
	for (i = total_out = total_in = 0; i < out_sgs; i++) {
		struct scatterlist *sg;
		for (sg = sgs[i]; sg; sg = sg_next(sg))
			total_out++;
	}
	for (; i < out_sgs + in_sgs; i++) {
		struct scatterlist *sg;
		for (sg = sgs[i]; sg; sg = sg_next(sg))
			total_in++;
	}
	return virtqueue_add(_vq, sgs, sg_next_chained,
			     total_out, total_in, out_sgs, in_sgs, data, gfp);
}
EXPORT_SYMBOL_GPL(virtqueue_add_sgs);

/* 
                                                            
                                                 
                                                        
                                                          
                                           
                                                     
  
                                                                        
                                         
  
                                                         
 */
int virtqueue_add_outbuf(struct virtqueue *vq,
			 struct scatterlist sg[], unsigned int num,
			 void *data,
			 gfp_t gfp)
{
	return virtqueue_add(vq, &sg, sg_next_arr, num, 0, 1, 0, data, gfp);
}
EXPORT_SYMBOL_GPL(virtqueue_add_outbuf);

/* 
                                                          
                                                 
                                                        
                                                          
                                           
                                                     
  
                                                                        
                                         
  
                                                         
 */
int virtqueue_add_inbuf(struct virtqueue *vq,
			struct scatterlist sg[], unsigned int num,
			void *data,
			gfp_t gfp)
{
	return virtqueue_add(vq, &sg, sg_next_arr, 0, num, 0, 1, data, gfp);
}
EXPORT_SYMBOL_GPL(virtqueue_add_inbuf);

/* 
                                                                    
                            
  
                                           
                                  
                         
  
                                                                      
                                                                     
 */
bool virtqueue_kick_prepare(struct virtqueue *_vq)
{
	struct vring_virtqueue *vq = to_vvq(_vq);
	u16 new, old;
	bool needs_kick;

	START_USE(vq);
	/*                                                                
           */
	virtio_mb(vq->weak_barriers);

	old = vq->vring.avail->idx - vq->num_added;
	new = vq->vring.avail->idx;
	vq->num_added = 0;

#ifdef DEBUG
	if (vq->last_add_time_valid) {
		WARN_ON(ktime_to_ms(ktime_sub(ktime_get(),
					      vq->last_add_time)) > 100);
	}
	vq->last_add_time_valid = false;
#endif

	if (vq->event) {
		needs_kick = vring_need_event(vring_avail_event(&vq->vring),
					      new, old);
	} else {
		needs_kick = !(vq->vring.used->flags & VRING_USED_F_NO_NOTIFY);
	}
	END_USE(vq);
	return needs_kick;
}
EXPORT_SYMBOL_GPL(virtqueue_kick_prepare);

/* 
                                                               
                            
  
                                       
 */
void virtqueue_notify(struct virtqueue *_vq)
{
	struct vring_virtqueue *vq = to_vvq(_vq);

	/*                                           */
	vq->notify(_vq);
}
EXPORT_SYMBOL_GPL(virtqueue_notify);

/* 
                                        
                            
  
                                                                 
                  
  
                                                             
                                                    
 */
void virtqueue_kick(struct virtqueue *vq)
{
	if (virtqueue_kick_prepare(vq))
		virtqueue_notify(vq);
}
EXPORT_SYMBOL_GPL(virtqueue_kick);

static void detach_buf(struct vring_virtqueue *vq, unsigned int head)
{
	unsigned int i;

	/*                 */
	vq->data[head] = NULL;

	/*                                 */
	i = head;

	/*                         */
	if (vq->vring.desc[i].flags & VRING_DESC_F_INDIRECT)
		kfree(phys_to_virt(vq->vring.desc[i].addr));

	while (vq->vring.desc[i].flags & VRING_DESC_F_NEXT) {
		i = vq->vring.desc[i].next;
		vq->vq.num_free++;
	}

	vq->vring.desc[i].next = vq->free_head;
	vq->free_head = head;
	/*                       */
	vq->vq.num_free++;
}

static inline bool more_used(const struct vring_virtqueue *vq)
{
	return vq->last_used_idx != vq->vring.used->idx;
}

/* 
                                               
                                                 
                                           
  
                                                                    
                                                                 
                                                                    
          
  
                                                             
                                                    
  
                                                                 
                                 
 */
void *virtqueue_get_buf(struct virtqueue *_vq, unsigned int *len)
{
	struct vring_virtqueue *vq = to_vvq(_vq);
	void *ret;
	unsigned int i;
	u16 last_used;

	START_USE(vq);

	if (unlikely(vq->broken)) {
		END_USE(vq);
		return NULL;
	}

	if (!more_used(vq)) {
		pr_debug("No more buffers in queue\n");
		END_USE(vq);
		return NULL;
	}

	/*                                                                   */
	virtio_rmb(vq->weak_barriers);

	last_used = (vq->last_used_idx & (vq->vring.num - 1));
	i = vq->vring.used->ring[last_used].id;
	*len = vq->vring.used->ring[last_used].len;

	if (unlikely(i >= vq->vring.num)) {
		BAD_RING(vq, "id %u out of range\n", i);
		return NULL;
	}
	if (unlikely(!vq->data[i])) {
		BAD_RING(vq, "id %u is not a head!\n", i);
		return NULL;
	}

	/*                                         */
	ret = vq->data[i];
	detach_buf(vq, i);
	vq->last_used_idx++;
	/*                                                        
                                                         
                                       */
	if (!(vq->vring.avail->flags & VRING_AVAIL_F_NO_INTERRUPT)) {
		vring_used_event(&vq->vring) = vq->last_used_idx;
		virtio_mb(vq->weak_barriers);
	}

#ifdef DEBUG
	vq->last_add_time_valid = false;
#endif

	END_USE(vq);
	return ret;
}
EXPORT_SYMBOL_GPL(virtqueue_get_buf);

/* 
                                           
                                                 
  
                                                                           
                             
  
                                                        
 */
void virtqueue_disable_cb(struct virtqueue *_vq)
{
	struct vring_virtqueue *vq = to_vvq(_vq);

	vq->vring.avail->flags |= VRING_AVAIL_F_NO_INTERRUPT;
}
EXPORT_SYMBOL_GPL(virtqueue_disable_cb);

/* 
                                                                   
                                                 
  
                                                            
                                                                    
                                                                            
                                     
  
                                                             
                                                    
 */
unsigned virtqueue_enable_cb_prepare(struct virtqueue *_vq)
{
	struct vring_virtqueue *vq = to_vvq(_vq);
	u16 last_used_idx;

	START_USE(vq);

	/*                                                                   
                */
	/*                                                             
                                                                   
                                               */
	vq->vring.avail->flags &= ~VRING_AVAIL_F_NO_INTERRUPT;
	vring_used_event(&vq->vring) = last_used_idx = vq->last_used_idx;
	END_USE(vq);
	return last_used_idx;
}
EXPORT_SYMBOL_GPL(virtqueue_enable_cb_prepare);

/* 
                                              
                                                 
                                                                              
  
                                                                 
  
                                       
 */
bool virtqueue_poll(struct virtqueue *_vq, unsigned last_used_idx)
{
	struct vring_virtqueue *vq = to_vvq(_vq);

	virtio_mb(vq->weak_barriers);
	return (u16)last_used_idx != vq->vring.used->idx;
}
EXPORT_SYMBOL_GPL(virtqueue_poll);

/* 
                                                            
                                                 
  
                                                                     
                                                                     
                                                  
  
                                                             
                                                    
 */
bool virtqueue_enable_cb(struct virtqueue *_vq)
{
	unsigned last_used_idx = virtqueue_enable_cb_prepare(_vq);
	return !virtqueue_poll(_vq, last_used_idx);
}
EXPORT_SYMBOL_GPL(virtqueue_enable_cb);

/* 
                                                                    
                                                 
  
                                                                 
                                                                      
                                                                     
                                                                       
                          
  
                                                             
                                                    
 */
bool virtqueue_enable_cb_delayed(struct virtqueue *_vq)
{
	struct vring_virtqueue *vq = to_vvq(_vq);
	u16 bufs;

	START_USE(vq);

	/*                                                                   
                */
	/*                                                                  
                                                                   
                                               */
	vq->vring.avail->flags &= ~VRING_AVAIL_F_NO_INTERRUPT;
	/*                           */
	bufs = (u16)(vq->vring.avail->idx - vq->last_used_idx) * 3 / 4;
	vring_used_event(&vq->vring) = vq->last_used_idx + bufs;
	virtio_mb(vq->weak_barriers);
	if (unlikely((u16)(vq->vring.used->idx - vq->last_used_idx) > bufs)) {
		END_USE(vq);
		return false;
	}

	END_USE(vq);
	return true;
}
EXPORT_SYMBOL_GPL(virtqueue_enable_cb_delayed);

/* 
                                                           
                                                 
  
                                                                  
                                                                     
            
 */
void *virtqueue_detach_unused_buf(struct virtqueue *_vq)
{
	struct vring_virtqueue *vq = to_vvq(_vq);
	unsigned int i;
	void *buf;

	START_USE(vq);

	for (i = 0; i < vq->vring.num; i++) {
		if (!vq->data[i])
			continue;
		/*                                         */
		buf = vq->data[i];
		detach_buf(vq, i);
		vq->vring.avail->idx--;
		END_USE(vq);
		return buf;
	}
	/*                                    */
	BUG_ON(vq->vq.num_free != vq->vring.num);

	END_USE(vq);
	return NULL;
}
EXPORT_SYMBOL_GPL(virtqueue_detach_unused_buf);

irqreturn_t vring_interrupt(int irq, void *_vq)
{
	struct vring_virtqueue *vq = to_vvq(_vq);

	if (!more_used(vq)) {
		pr_debug("virtqueue interrupt with no work for %p\n", vq);
		return IRQ_NONE;
	}

	if (unlikely(vq->broken))
		return IRQ_HANDLED;

	pr_debug("virtqueue callback for %p (%p)\n", vq, vq->vq.callback);
	if (vq->vq.callback)
		vq->vq.callback(&vq->vq);

	return IRQ_HANDLED;
}
EXPORT_SYMBOL_GPL(vring_interrupt);

struct virtqueue *vring_new_virtqueue(unsigned int index,
				      unsigned int num,
				      unsigned int vring_align,
				      struct virtio_device *vdev,
				      bool weak_barriers,
				      void *pages,
				      void (*notify)(struct virtqueue *),
				      void (*callback)(struct virtqueue *),
				      const char *name)
{
	struct vring_virtqueue *vq;
	unsigned int i;

	/*                                */
	if (num & (num - 1)) {
		dev_warn(&vdev->dev, "Bad virtqueue length %u\n", num);
		return NULL;
	}

	vq = kmalloc(sizeof(*vq) + sizeof(void *)*num, GFP_KERNEL);
	if (!vq)
		return NULL;

	vring_init(&vq->vring, num, pages, vring_align);
	vq->vq.callback = callback;
	vq->vq.vdev = vdev;
	vq->vq.name = name;
	vq->vq.num_free = num;
	vq->vq.index = index;
	vq->notify = notify;
	vq->weak_barriers = weak_barriers;
	vq->broken = false;
	vq->last_used_idx = 0;
	vq->num_added = 0;
	list_add_tail(&vq->vq.list, &vdev->vqs);
#ifdef DEBUG
	vq->in_use = false;
	vq->last_add_time_valid = false;
#endif

	vq->indirect = virtio_has_feature(vdev, VIRTIO_RING_F_INDIRECT_DESC);
	vq->event = virtio_has_feature(vdev, VIRTIO_RING_F_EVENT_IDX);

	/*                                                 */
	if (!callback)
		vq->vring.avail->flags |= VRING_AVAIL_F_NO_INTERRUPT;

	/*                               */
	vq->free_head = 0;
	for (i = 0; i < num-1; i++) {
		vq->vring.desc[i].next = i+1;
		vq->data[i] = NULL;
	}
	vq->data[i] = NULL;

	return &vq->vq;
}
EXPORT_SYMBOL_GPL(vring_new_virtqueue);

void vring_del_virtqueue(struct virtqueue *vq)
{
	list_del(&vq->list);
	kfree(to_vvq(vq));
}
EXPORT_SYMBOL_GPL(vring_del_virtqueue);

/*                                              */
void vring_transport_features(struct virtio_device *vdev)
{
	unsigned int i;

	for (i = VIRTIO_TRANSPORT_F_START; i < VIRTIO_TRANSPORT_F_END; i++) {
		switch (i) {
		case VIRTIO_RING_F_INDIRECT_DESC:
			break;
		case VIRTIO_RING_F_EVENT_IDX:
			break;
		default:
			/*                               */
			clear_bit(i, vdev->features);
		}
	}
}
EXPORT_SYMBOL_GPL(vring_transport_features);

/* 
                                                                      
                                                              
  
                                                                      
                                                                    
 */
unsigned int virtqueue_get_vring_size(struct virtqueue *_vq)
{

	struct vring_virtqueue *vq = to_vvq(_vq);

	return vq->vring.num;
}
EXPORT_SYMBOL_GPL(virtqueue_get_vring_size);

MODULE_LICENSE("GPL");
