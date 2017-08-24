/*
 * kvm eventfd support - use eventfd objects to signal various KVM events
 *
 * Copyright 2009 Novell.  All Rights Reserved.
 * Copyright 2010 Red Hat, Inc. and/or its affiliates.
 *
 * Author:
 *	Gregory Haskins <ghaskins@novell.com>
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <linux/kvm_host.h>
#include <linux/kvm.h>
#include <linux/workqueue.h>
#include <linux/syscalls.h>
#include <linux/wait.h>
#include <linux/poll.h>
#include <linux/file.h>
#include <linux/list.h>
#include <linux/eventfd.h>
#include <linux/kernel.h>
#include <linux/slab.h>

#include "iodev.h"

#ifdef CONFIG_HAVE_KVM_IRQ_ROUTING
/*
                                                                       
                                                                     
  
                                                   
                                                                       
 */

/*
                                                                    
                                                                    
                                                                 
                                                                 
                                                              
                                                                   
                                                             
 */
struct _irqfd_resampler {
	struct kvm *kvm;
	/*
                                                              
                                                      
  */
	struct list_head list;
	struct kvm_irq_ack_notifier notifier;
	/*
                                                                
                                            
                                                          
  */
	struct list_head link;
};

struct _irqfd {
	/*                        */
	struct kvm *kvm;
	wait_queue_t wait;
	/*                                         */
	struct kvm_kernel_irq_routing_entry __rcu *irq_entry;
	/*                              */
	int gsi;
	struct work_struct inject;
	/*                                                   */
	struct _irqfd_resampler *resampler;
	/*                                               */
	struct eventfd_ctx *resamplefd;
	/*                                                          */
	struct list_head resampler_link;
	/*                         */
	struct eventfd_ctx *eventfd;
	struct list_head list;
	poll_table pt;
	struct work_struct shutdown;
};

static struct workqueue_struct *irqfd_cleanup_wq;

static void
irqfd_inject(struct work_struct *work)
{
	struct _irqfd *irqfd = container_of(work, struct _irqfd, inject);
	struct kvm *kvm = irqfd->kvm;

	if (!irqfd->resampler) {
		kvm_set_irq(kvm, KVM_USERSPACE_IRQ_SOURCE_ID, irqfd->gsi, 1,
				false);
		kvm_set_irq(kvm, KVM_USERSPACE_IRQ_SOURCE_ID, irqfd->gsi, 0,
				false);
	} else
		kvm_set_irq(kvm, KVM_IRQFD_RESAMPLE_IRQ_SOURCE_ID,
			    irqfd->gsi, 1, false);
}

/*
                                                                   
                                                                    
                                                                     
 */
static void
irqfd_resampler_ack(struct kvm_irq_ack_notifier *kian)
{
	struct _irqfd_resampler *resampler;
	struct _irqfd *irqfd;

	resampler = container_of(kian, struct _irqfd_resampler, notifier);

	kvm_set_irq(resampler->kvm, KVM_IRQFD_RESAMPLE_IRQ_SOURCE_ID,
		    resampler->notifier.gsi, 0, false);

	rcu_read_lock();

	list_for_each_entry_rcu(irqfd, &resampler->list, resampler_link)
		eventfd_signal(irqfd->resamplefd, 1);

	rcu_read_unlock();
}

static void
irqfd_resampler_shutdown(struct _irqfd *irqfd)
{
	struct _irqfd_resampler *resampler = irqfd->resampler;
	struct kvm *kvm = resampler->kvm;

	mutex_lock(&kvm->irqfds.resampler_lock);

	list_del_rcu(&irqfd->resampler_link);
	synchronize_rcu();

	if (list_empty(&resampler->list)) {
		list_del(&resampler->link);
		kvm_unregister_irq_ack_notifier(kvm, &resampler->notifier);
		kvm_set_irq(kvm, KVM_IRQFD_RESAMPLE_IRQ_SOURCE_ID,
			    resampler->notifier.gsi, 0, false);
		kfree(resampler);
	}

	mutex_unlock(&kvm->irqfds.resampler_lock);
}

/*
                                                  
 */
static void
irqfd_shutdown(struct work_struct *work)
{
	struct _irqfd *irqfd = container_of(work, struct _irqfd, shutdown);
	u64 cnt;

	/*
                                                                   
                   
  */
	eventfd_ctx_remove_wait_queue(irqfd->eventfd, &irqfd->wait, &cnt);

	/*
                                                                   
                                                          
  */
	flush_work(&irqfd->inject);

	if (irqfd->resampler) {
		irqfd_resampler_shutdown(irqfd);
		eventfd_ctx_put(irqfd->resamplefd);
	}

	/*
                                                    
  */
	eventfd_ctx_put(irqfd->eventfd);
	kfree(irqfd);
}


/*                                  */
static bool
irqfd_is_active(struct _irqfd *irqfd)
{
	return list_empty(&irqfd->list) ? false : true;
}

/*
                                                         
  
                                   
 */
static void
irqfd_deactivate(struct _irqfd *irqfd)
{
	BUG_ON(!irqfd_is_active(irqfd));

	list_del_init(&irqfd->list);

	queue_work(irqfd_cleanup_wq, &irqfd->shutdown);
}

/*
                                                     
 */
static int
irqfd_wakeup(wait_queue_t *wait, unsigned mode, int sync, void *key)
{
	struct _irqfd *irqfd = container_of(wait, struct _irqfd, wait);
	unsigned long flags = (unsigned long)key;
	struct kvm_kernel_irq_routing_entry *irq;
	struct kvm *kvm = irqfd->kvm;

	if (flags & POLLIN) {
		rcu_read_lock();
		irq = rcu_dereference(irqfd->irq_entry);
		/*                                                 */
		if (irq)
			kvm_set_msi(irq, kvm, KVM_USERSPACE_IRQ_SOURCE_ID, 1,
					false);
		else
			schedule_work(&irqfd->inject);
		rcu_read_unlock();
	}

	if (flags & POLLHUP) {
		/*                                         */
		unsigned long flags;

		spin_lock_irqsave(&kvm->irqfds.lock, flags);

		/*
                                                          
                                                       
                                                             
                                                          
                                                              
                                                          
                                                               
   */
		if (irqfd_is_active(irqfd))
			irqfd_deactivate(irqfd);

		spin_unlock_irqrestore(&kvm->irqfds.lock, flags);
	}

	return 0;
}

static void
irqfd_ptable_queue_proc(struct file *file, wait_queue_head_t *wqh,
			poll_table *pt)
{
	struct _irqfd *irqfd = container_of(pt, struct _irqfd, pt);
	add_wait_queue(wqh, &irqfd->wait);
}

/*                                  */
static void irqfd_update(struct kvm *kvm, struct _irqfd *irqfd,
			 struct kvm_irq_routing_table *irq_rt)
{
	struct kvm_kernel_irq_routing_entry *e;

	if (irqfd->gsi >= irq_rt->nr_rt_entries) {
		rcu_assign_pointer(irqfd->irq_entry, NULL);
		return;
	}

	hlist_for_each_entry(e, &irq_rt->map[irqfd->gsi], link) {
		/*                     */
		if (e->type == KVM_IRQ_ROUTING_MSI)
			rcu_assign_pointer(irqfd->irq_entry, e);
		else
			rcu_assign_pointer(irqfd->irq_entry, NULL);
	}
}

static int
kvm_irqfd_assign(struct kvm *kvm, struct kvm_irqfd *args)
{
	struct kvm_irq_routing_table *irq_rt;
	struct _irqfd *irqfd, *tmp;
	struct file *file = NULL;
	struct eventfd_ctx *eventfd = NULL, *resamplefd = NULL;
	int ret;
	unsigned int events;

	irqfd = kzalloc(sizeof(*irqfd), GFP_KERNEL);
	if (!irqfd)
		return -ENOMEM;

	irqfd->kvm = kvm;
	irqfd->gsi = args->gsi;
	INIT_LIST_HEAD(&irqfd->list);
	INIT_WORK(&irqfd->inject, irqfd_inject);
	INIT_WORK(&irqfd->shutdown, irqfd_shutdown);

	file = eventfd_fget(args->fd);
	if (IS_ERR(file)) {
		ret = PTR_ERR(file);
		goto fail;
	}

	eventfd = eventfd_ctx_fileget(file);
	if (IS_ERR(eventfd)) {
		ret = PTR_ERR(eventfd);
		goto fail;
	}

	irqfd->eventfd = eventfd;

	if (args->flags & KVM_IRQFD_FLAG_RESAMPLE) {
		struct _irqfd_resampler *resampler;

		resamplefd = eventfd_ctx_fdget(args->resamplefd);
		if (IS_ERR(resamplefd)) {
			ret = PTR_ERR(resamplefd);
			goto fail;
		}

		irqfd->resamplefd = resamplefd;
		INIT_LIST_HEAD(&irqfd->resampler_link);

		mutex_lock(&kvm->irqfds.resampler_lock);

		list_for_each_entry(resampler,
				    &kvm->irqfds.resampler_list, link) {
			if (resampler->notifier.gsi == irqfd->gsi) {
				irqfd->resampler = resampler;
				break;
			}
		}

		if (!irqfd->resampler) {
			resampler = kzalloc(sizeof(*resampler), GFP_KERNEL);
			if (!resampler) {
				ret = -ENOMEM;
				mutex_unlock(&kvm->irqfds.resampler_lock);
				goto fail;
			}

			resampler->kvm = kvm;
			INIT_LIST_HEAD(&resampler->list);
			resampler->notifier.gsi = irqfd->gsi;
			resampler->notifier.irq_acked = irqfd_resampler_ack;
			INIT_LIST_HEAD(&resampler->link);

			list_add(&resampler->link, &kvm->irqfds.resampler_list);
			kvm_register_irq_ack_notifier(kvm,
						      &resampler->notifier);
			irqfd->resampler = resampler;
		}

		list_add_rcu(&irqfd->resampler_link, &irqfd->resampler->list);
		synchronize_rcu();

		mutex_unlock(&kvm->irqfds.resampler_lock);
	}

	/*
                                                                  
                                                              
  */
	init_waitqueue_func_entry(&irqfd->wait, irqfd_wakeup);
	init_poll_funcptr(&irqfd->pt, irqfd_ptable_queue_proc);

	spin_lock_irq(&kvm->irqfds.lock);

	ret = 0;
	list_for_each_entry(tmp, &kvm->irqfds.items, list) {
		if (irqfd->eventfd != tmp->eventfd)
			continue;
		/*                                          */
		ret = -EBUSY;
		spin_unlock_irq(&kvm->irqfds.lock);
		goto fail;
	}

	irq_rt = rcu_dereference_protected(kvm->irq_routing,
					   lockdep_is_held(&kvm->irqfds.lock));
	irqfd_update(kvm, irqfd, irq_rt);

	events = file->f_op->poll(file, &irqfd->pt);

	list_add_tail(&irqfd->list, &kvm->irqfds.items);

	/*
                                                              
                                                                 
  */
	if (events & POLLIN)
		schedule_work(&irqfd->inject);

	spin_unlock_irq(&kvm->irqfds.lock);

	/*
                                                                        
                                     
  */
	fput(file);

	return 0;

fail:
	if (irqfd->resampler)
		irqfd_resampler_shutdown(irqfd);

	if (resamplefd && !IS_ERR(resamplefd))
		eventfd_ctx_put(resamplefd);

	if (eventfd && !IS_ERR(eventfd))
		eventfd_ctx_put(eventfd);

	if (!IS_ERR(file))
		fput(file);

	kfree(irqfd);
	return ret;
}
#endif

void
kvm_eventfd_init(struct kvm *kvm)
{
#ifdef CONFIG_HAVE_KVM_IRQ_ROUTING
	spin_lock_init(&kvm->irqfds.lock);
	INIT_LIST_HEAD(&kvm->irqfds.items);
	INIT_LIST_HEAD(&kvm->irqfds.resampler_list);
	mutex_init(&kvm->irqfds.resampler_lock);
#endif
	INIT_LIST_HEAD(&kvm->ioeventfds);
}

#ifdef CONFIG_HAVE_KVM_IRQ_ROUTING
/*
                                         
 */
static int
kvm_irqfd_deassign(struct kvm *kvm, struct kvm_irqfd *args)
{
	struct _irqfd *irqfd, *tmp;
	struct eventfd_ctx *eventfd;

	eventfd = eventfd_ctx_fdget(args->fd);
	if (IS_ERR(eventfd))
		return PTR_ERR(eventfd);

	spin_lock_irq(&kvm->irqfds.lock);

	list_for_each_entry_safe(irqfd, tmp, &kvm->irqfds.items, list) {
		if (irqfd->eventfd == eventfd && irqfd->gsi == args->gsi) {
			/*
                                                
                                                        
                                                   
                                                
                                                      
                       
    */
			rcu_assign_pointer(irqfd->irq_entry, NULL);
			irqfd_deactivate(irqfd);
		}
	}

	spin_unlock_irq(&kvm->irqfds.lock);
	eventfd_ctx_put(eventfd);

	/*
                                                                    
                                                                      
                                            
  */
	flush_workqueue(irqfd_cleanup_wq);

	return 0;
}

int
kvm_irqfd(struct kvm *kvm, struct kvm_irqfd *args)
{
	if (args->flags & ~(KVM_IRQFD_FLAG_DEASSIGN | KVM_IRQFD_FLAG_RESAMPLE))
		return -EINVAL;

	if (args->flags & KVM_IRQFD_FLAG_DEASSIGN)
		return kvm_irqfd_deassign(kvm, args);

	return kvm_irqfd_assign(kvm, args);
}

/*
                                                                           
                                
 */
void
kvm_irqfd_release(struct kvm *kvm)
{
	struct _irqfd *irqfd, *tmp;

	spin_lock_irq(&kvm->irqfds.lock);

	list_for_each_entry_safe(irqfd, tmp, &kvm->irqfds.items, list)
		irqfd_deactivate(irqfd);

	spin_unlock_irq(&kvm->irqfds.lock);

	/*
                                                                    
                                          
  */
	flush_workqueue(irqfd_cleanup_wq);

}

/*
                                
                                                 
 */
void kvm_irq_routing_update(struct kvm *kvm,
			    struct kvm_irq_routing_table *irq_rt)
{
	struct _irqfd *irqfd;

	spin_lock_irq(&kvm->irqfds.lock);

	rcu_assign_pointer(kvm->irq_routing, irq_rt);

	list_for_each_entry(irqfd, &kvm->irqfds.items, list)
		irqfd_update(kvm, irqfd, irq_rt);

	spin_unlock_irq(&kvm->irqfds.lock);
}

/*
                                                                      
                                                                            
                                                                    
 */
int kvm_irqfd_init(void)
{
	irqfd_cleanup_wq = create_singlethread_workqueue("kvm-irqfd-cleanup");
	if (!irqfd_cleanup_wq)
		return -ENOMEM;

	return 0;
}

void kvm_irqfd_exit(void)
{
	destroy_workqueue(irqfd_cleanup_wq);
}
#endif

/*
                                                                       
                                                                     
  
                                                                          
                                                 
                                                                       
 */

struct _ioeventfd {
	struct list_head     list;
	u64                  addr;
	int                  length;
	struct eventfd_ctx  *eventfd;
	u64                  datamatch;
	struct kvm_io_device dev;
	u8                   bus_idx;
	bool                 wildcard;
};

static inline struct _ioeventfd *
to_ioeventfd(struct kvm_io_device *dev)
{
	return container_of(dev, struct _ioeventfd, dev);
}

static void
ioeventfd_release(struct _ioeventfd *p)
{
	eventfd_ctx_put(p->eventfd);
	list_del(&p->list);
	kfree(p);
}

static bool
ioeventfd_in_range(struct _ioeventfd *p, gpa_t addr, int len, const void *val)
{
	u64 _val;

	if (!(addr == p->addr && len == p->length))
		/*                                         */
		return false;

	if (p->wildcard)
		/*                                          */
		return true;

	/*                                                 */

	BUG_ON(!IS_ALIGNED((unsigned long)val, len));

	switch (len) {
	case 1:
		_val = *(u8 *)val;
		break;
	case 2:
		_val = *(u16 *)val;
		break;
	case 4:
		_val = *(u32 *)val;
		break;
	case 8:
		_val = *(u64 *)val;
		break;
	default:
		return false;
	}

	return _val == p->datamatch ? true : false;
}

/*                                                        */
static int
ioeventfd_write(struct kvm_io_device *this, gpa_t addr, int len,
		const void *val)
{
	struct _ioeventfd *p = to_ioeventfd(this);

	if (!ioeventfd_in_range(p, addr, len, val))
		return -EOPNOTSUPP;

	eventfd_signal(p->eventfd, 1);
	return 0;
}

/*
                                                                         
                                                                                
 */
static void
ioeventfd_destructor(struct kvm_io_device *this)
{
	struct _ioeventfd *p = to_ioeventfd(this);

	ioeventfd_release(p);
}

static const struct kvm_io_device_ops ioeventfd_ops = {
	.write      = ioeventfd_write,
	.destructor = ioeventfd_destructor,
};

/*                              */
static bool
ioeventfd_check_collision(struct kvm *kvm, struct _ioeventfd *p)
{
	struct _ioeventfd *_p;

	list_for_each_entry(_p, &kvm->ioeventfds, list)
		if (_p->bus_idx == p->bus_idx &&
		    _p->addr == p->addr && _p->length == p->length &&
		    (_p->wildcard || p->wildcard ||
		     _p->datamatch == p->datamatch))
			return true;

	return false;
}

static enum kvm_bus ioeventfd_bus_from_flags(__u32 flags)
{
	if (flags & KVM_IOEVENTFD_FLAG_PIO)
		return KVM_PIO_BUS;
	if (flags & KVM_IOEVENTFD_FLAG_VIRTIO_CCW_NOTIFY)
		return KVM_VIRTIO_CCW_NOTIFY_BUS;
	return KVM_MMIO_BUS;
}

static int
kvm_assign_ioeventfd(struct kvm *kvm, struct kvm_ioeventfd *args)
{
	enum kvm_bus              bus_idx;
	struct _ioeventfd        *p;
	struct eventfd_ctx       *eventfd;
	int                       ret;

	bus_idx = ioeventfd_bus_from_flags(args->flags);
	/*                            */
	switch (args->len) {
	case 1:
	case 2:
	case 4:
	case 8:
		break;
	default:
		return -EINVAL;
	}

	/*                          */
	if (args->addr + args->len < args->addr)
		return -EINVAL;

	/*                                                */
	if (args->flags & ~KVM_IOEVENTFD_VALID_FLAG_MASK)
		return -EINVAL;

	eventfd = eventfd_ctx_fdget(args->fd);
	if (IS_ERR(eventfd))
		return PTR_ERR(eventfd);

	p = kzalloc(sizeof(*p), GFP_KERNEL);
	if (!p) {
		ret = -ENOMEM;
		goto fail;
	}

	INIT_LIST_HEAD(&p->list);
	p->addr    = args->addr;
	p->bus_idx = bus_idx;
	p->length  = args->len;
	p->eventfd = eventfd;

	/*                                                                 */
	if (args->flags & KVM_IOEVENTFD_FLAG_DATAMATCH)
		p->datamatch = args->datamatch;
	else
		p->wildcard = true;

	mutex_lock(&kvm->slots_lock);

	/*                                         */
	if (ioeventfd_check_collision(kvm, p)) {
		ret = -EEXIST;
		goto unlock_fail;
	}

	kvm_iodevice_init(&p->dev, &ioeventfd_ops);

	ret = kvm_io_bus_register_dev(kvm, bus_idx, p->addr, p->length,
				      &p->dev);
	if (ret < 0)
		goto unlock_fail;

	list_add_tail(&p->list, &kvm->ioeventfds);

	mutex_unlock(&kvm->slots_lock);

	return 0;

unlock_fail:
	mutex_unlock(&kvm->slots_lock);

fail:
	kfree(p);
	eventfd_ctx_put(eventfd);

	return ret;
}

static int
kvm_deassign_ioeventfd(struct kvm *kvm, struct kvm_ioeventfd *args)
{
	enum kvm_bus              bus_idx;
	struct _ioeventfd        *p, *tmp;
	struct eventfd_ctx       *eventfd;
	int                       ret = -ENOENT;

	bus_idx = ioeventfd_bus_from_flags(args->flags);
	eventfd = eventfd_ctx_fdget(args->fd);
	if (IS_ERR(eventfd))
		return PTR_ERR(eventfd);

	mutex_lock(&kvm->slots_lock);

	list_for_each_entry_safe(p, tmp, &kvm->ioeventfds, list) {
		bool wildcard = !(args->flags & KVM_IOEVENTFD_FLAG_DATAMATCH);

		if (p->bus_idx != bus_idx ||
		    p->eventfd != eventfd  ||
		    p->addr != args->addr  ||
		    p->length != args->len ||
		    p->wildcard != wildcard)
			continue;

		if (!p->wildcard && p->datamatch != args->datamatch)
			continue;

		kvm_io_bus_unregister_dev(kvm, bus_idx, &p->dev);
		ioeventfd_release(p);
		ret = 0;
		break;
	}

	mutex_unlock(&kvm->slots_lock);

	eventfd_ctx_put(eventfd);

	return ret;
}

int
kvm_ioeventfd(struct kvm *kvm, struct kvm_ioeventfd *args)
{
	if (args->flags & KVM_IOEVENTFD_FLAG_DEASSIGN)
		return kvm_deassign_ioeventfd(kvm, args);

	return kvm_assign_ioeventfd(kvm, args);
}
