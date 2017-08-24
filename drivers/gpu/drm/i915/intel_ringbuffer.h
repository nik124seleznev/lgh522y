#ifndef _INTEL_RINGBUFFER_H_
#define _INTEL_RINGBUFFER_H_

/*
                                                                      
                                                                            
                                                                                      
  
                                                                        
                                                                
            
 */
#define I915_RING_FREE_SPACE 64

struct  intel_hw_status_page {
	u32		*page_addr;
	unsigned int	gfx_addr;
	struct		drm_i915_gem_object *obj;
};

#define I915_READ_TAIL(ring) I915_READ(RING_TAIL((ring)->mmio_base))
#define I915_WRITE_TAIL(ring, val) I915_WRITE(RING_TAIL((ring)->mmio_base), val)

#define I915_READ_START(ring) I915_READ(RING_START((ring)->mmio_base))
#define I915_WRITE_START(ring, val) I915_WRITE(RING_START((ring)->mmio_base), val)

#define I915_READ_HEAD(ring)  I915_READ(RING_HEAD((ring)->mmio_base))
#define I915_WRITE_HEAD(ring, val) I915_WRITE(RING_HEAD((ring)->mmio_base), val)

#define I915_READ_CTL(ring) I915_READ(RING_CTL((ring)->mmio_base))
#define I915_WRITE_CTL(ring, val) I915_WRITE(RING_CTL((ring)->mmio_base), val)

#define I915_READ_IMR(ring) I915_READ(RING_IMR((ring)->mmio_base))
#define I915_WRITE_IMR(ring, val) I915_WRITE(RING_IMR((ring)->mmio_base), val)

#define I915_READ_NOPID(ring) I915_READ(RING_NOPID((ring)->mmio_base))
#define I915_READ_SYNC_0(ring) I915_READ(RING_SYNC_0((ring)->mmio_base))
#define I915_READ_SYNC_1(ring) I915_READ(RING_SYNC_1((ring)->mmio_base))

struct  intel_ring_buffer {
	const char	*name;
	enum intel_ring_id {
		RCS = 0x0,
		VCS,
		BCS,
	} id;
#define I915_NUM_RINGS 3
	u32		mmio_base;
	void		__iomem *virtual_start;
	struct		drm_device *dev;
	struct		drm_i915_gem_object *obj;

	u32		head;
	u32		tail;
	int		space;
	int		size;
	int		effective_size;
	struct intel_hw_status_page status_page;

	/*                                                               
                                                                  
                                                               
                                                   
   
                                                                 
                                  
  */
	u32		last_retired_head;

	u32		irq_refcount;		/*                                 */
	u32		irq_enable_mask;	/*                                  */
	u32		trace_irq_seqno;
	u32		sync_seqno[I915_NUM_RINGS-1];
	bool __must_check (*irq_get)(struct intel_ring_buffer *ring);
	void		(*irq_put)(struct intel_ring_buffer *ring);

	int		(*init)(struct intel_ring_buffer *ring);

	void		(*write_tail)(struct intel_ring_buffer *ring,
				      u32 value);
	int __must_check (*flush)(struct intel_ring_buffer *ring,
				  u32	invalidate_domains,
				  u32	flush_domains);
	int		(*add_request)(struct intel_ring_buffer *ring);
	/*                                                               
                                                                   
                                                                     
                                                                 
                                    
  */
	u32		(*get_seqno)(struct intel_ring_buffer *ring,
				     bool lazy_coherency);
	void		(*set_seqno)(struct intel_ring_buffer *ring,
				     u32 seqno);
	int		(*dispatch_execbuffer)(struct intel_ring_buffer *ring,
					       u32 offset, u32 length,
					       unsigned flags);
#define I915_DISPATCH_SECURE 0x1
#define I915_DISPATCH_PINNED 0x2
	void		(*cleanup)(struct intel_ring_buffer *ring);
	int		(*sync_to)(struct intel_ring_buffer *ring,
				   struct intel_ring_buffer *to,
				   u32 seqno);

	u32		semaphore_register[3]; /*                           */
	u32		signal_mbox[2]; /*                             */
	/* 
                                                            
               
   
                                                            
                                                              
                                                             
   
                                                         
  */
	struct list_head active_list;

	/* 
                                                              
                
  */
	struct list_head request_list;

	/* 
                                                         
  */
	u32 outstanding_lazy_request;
	bool gpu_caches_dirty;

	wait_queue_head_t irq_queue;

	/* 
                                                  
  */
	bool itlb_before_ctx_switch;
	struct i915_hw_context *default_context;
	struct drm_i915_gem_object *last_context_obj;

	void *private;
};

static inline bool
intel_ring_initialized(struct intel_ring_buffer *ring)
{
	return ring->obj != NULL;
}

static inline unsigned
intel_ring_flag(struct intel_ring_buffer *ring)
{
	return 1 << ring->id;
}

static inline u32
intel_ring_sync_index(struct intel_ring_buffer *ring,
		      struct intel_ring_buffer *other)
{
	int idx;

	/*
                          
                           
                           
  */

	idx = (other - ring) - 1;
	if (idx < 0)
		idx += I915_NUM_RINGS;

	return idx;
}

static inline u32
intel_read_status_page(struct intel_ring_buffer *ring,
		       int reg)
{
	/*                                                          */
	barrier();
	return ring->status_page.page_addr[reg];
}

static inline void
intel_write_status_page(struct intel_ring_buffer *ring,
			int reg, u32 value)
{
	ring->status_page.page_addr[reg] = value;
}

/* 
                                                                             
                                                                      
                     
  
                                                
                                                                         
                            
                                        
                                        
                                          
                                           
  
                                                                   
 */
#define I915_GEM_HWS_INDEX		0x20
#define I915_GEM_HWS_SCRATCH_INDEX	0x30
#define I915_GEM_HWS_SCRATCH_ADDR (I915_GEM_HWS_SCRATCH_INDEX << MI_STORE_DWORD_INDEX_SHIFT)

void intel_cleanup_ring_buffer(struct intel_ring_buffer *ring);

int __must_check intel_ring_begin(struct intel_ring_buffer *ring, int n);
static inline void intel_ring_emit(struct intel_ring_buffer *ring,
				   u32 data)
{
	iowrite32(data, ring->virtual_start + ring->tail);
	ring->tail += 4;
}
void intel_ring_advance(struct intel_ring_buffer *ring);
int __must_check intel_ring_idle(struct intel_ring_buffer *ring);
void intel_ring_init_seqno(struct intel_ring_buffer *ring, u32 seqno);
int intel_ring_flush_all_caches(struct intel_ring_buffer *ring);
int intel_ring_invalidate_all_caches(struct intel_ring_buffer *ring);

int intel_init_render_ring_buffer(struct drm_device *dev);
int intel_init_bsd_ring_buffer(struct drm_device *dev);
int intel_init_blt_ring_buffer(struct drm_device *dev);

u32 intel_ring_get_active_head(struct intel_ring_buffer *ring);
void intel_ring_setup_status_page(struct intel_ring_buffer *ring);

static inline u32 intel_ring_get_tail(struct intel_ring_buffer *ring)
{
	return ring->tail;
}

static inline u32 intel_ring_get_seqno(struct intel_ring_buffer *ring)
{
	BUG_ON(ring->outstanding_lazy_request == 0);
	return ring->outstanding_lazy_request;
}

static inline void i915_trace_irq_get(struct intel_ring_buffer *ring, u32 seqno)
{
	if (ring->trace_irq_seqno == 0 && ring->irq_get(ring))
		ring->trace_irq_seqno = seqno;
}

/*           */
int intel_render_ring_init_dri(struct drm_device *dev, u64 start, u32 size);

#endif /*                      */
