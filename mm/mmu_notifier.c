/*
 *  linux/mm/mmu_notifier.c
 *
 *  Copyright (C) 2008  Qumranet, Inc.
 *  Copyright (C) 2008  SGI
 *             Christoph Lameter <clameter@sgi.com>
 *
 *  This work is licensed under the terms of the GNU GPL, version 2. See
 *  the COPYING file in the top-level directory.
 */

#include <linux/rculist.h>
#include <linux/mmu_notifier.h>
#include <linux/export.h>
#include <linux/mm.h>
#include <linux/err.h>
#include <linux/srcu.h>
#include <linux/rcupdate.h>
#include <linux/sched.h>
#include <linux/slab.h>

/*                         */
static struct srcu_struct srcu;

/*
                                                                     
                                                                      
                                                                      
                                                                  
                                                                  
                                                                   
                                                                  
                                                                    
                                                                 
          
 */
void __mmu_notifier_release(struct mm_struct *mm)
{
	struct mmu_notifier *mn;
	int id;

	/*
                                                      
                      
  */
	id = srcu_read_lock(&srcu);
	hlist_for_each_entry_rcu(mn, &mm->mmu_notifier_mm->list, hlist)
		/*
                                                                
                                                              
                                                                  
                                                    
   */
		if (mn->ops->release)
			mn->ops->release(mn, mm);
	srcu_read_unlock(&srcu, id);

	spin_lock(&mm->mmu_notifier_mm->lock);
	while (unlikely(!hlist_empty(&mm->mmu_notifier_mm->list))) {
		mn = hlist_entry(mm->mmu_notifier_mm->list.first,
				 struct mmu_notifier,
				 hlist);
		/*
                                                 
                                                               
                                                               
            
   */
		hlist_del_init_rcu(&mn->hlist);
	}
	spin_unlock(&mm->mmu_notifier_mm->lock);

	/*
                                                                         
                                                                    
                                                            
                            
   
                                                                        
                         
  */
	synchronize_srcu(&srcu);
}

/*
                                                                            
                                                                          
                  
 */
int __mmu_notifier_clear_flush_young(struct mm_struct *mm,
					unsigned long address)
{
	struct mmu_notifier *mn;
	int young = 0, id;

	id = srcu_read_lock(&srcu);
	hlist_for_each_entry_rcu(mn, &mm->mmu_notifier_mm->list, hlist) {
		if (mn->ops->clear_flush_young)
			young |= mn->ops->clear_flush_young(mn, mm, address);
	}
	srcu_read_unlock(&srcu, id);

	return young;
}

int __mmu_notifier_test_young(struct mm_struct *mm,
			      unsigned long address)
{
	struct mmu_notifier *mn;
	int young = 0, id;

	id = srcu_read_lock(&srcu);
	hlist_for_each_entry_rcu(mn, &mm->mmu_notifier_mm->list, hlist) {
		if (mn->ops->test_young) {
			young = mn->ops->test_young(mn, mm, address);
			if (young)
				break;
		}
	}
	srcu_read_unlock(&srcu, id);

	return young;
}

void __mmu_notifier_change_pte(struct mm_struct *mm, unsigned long address,
			       pte_t pte)
{
	struct mmu_notifier *mn;
	int id;

	id = srcu_read_lock(&srcu);
	hlist_for_each_entry_rcu(mn, &mm->mmu_notifier_mm->list, hlist) {
		if (mn->ops->change_pte)
			mn->ops->change_pte(mn, mm, address, pte);
	}
	srcu_read_unlock(&srcu, id);
}

void __mmu_notifier_invalidate_page(struct mm_struct *mm,
					  unsigned long address)
{
	struct mmu_notifier *mn;
	int id;

	id = srcu_read_lock(&srcu);
	hlist_for_each_entry_rcu(mn, &mm->mmu_notifier_mm->list, hlist) {
		if (mn->ops->invalidate_page)
			mn->ops->invalidate_page(mn, mm, address);
	}
	srcu_read_unlock(&srcu, id);
}

void __mmu_notifier_invalidate_range_start(struct mm_struct *mm,
				  unsigned long start, unsigned long end)
{
	struct mmu_notifier *mn;
	int id;

	id = srcu_read_lock(&srcu);
	hlist_for_each_entry_rcu(mn, &mm->mmu_notifier_mm->list, hlist) {
		if (mn->ops->invalidate_range_start)
			mn->ops->invalidate_range_start(mn, mm, start, end);
	}
	srcu_read_unlock(&srcu, id);
}
EXPORT_SYMBOL_GPL(__mmu_notifier_invalidate_range_start);

void __mmu_notifier_invalidate_range_end(struct mm_struct *mm,
				  unsigned long start, unsigned long end)
{
	struct mmu_notifier *mn;
	int id;

	id = srcu_read_lock(&srcu);
	hlist_for_each_entry_rcu(mn, &mm->mmu_notifier_mm->list, hlist) {
		if (mn->ops->invalidate_range_end)
			mn->ops->invalidate_range_end(mn, mm, start, end);
	}
	srcu_read_unlock(&srcu, id);
}
EXPORT_SYMBOL_GPL(__mmu_notifier_invalidate_range_end);

static int do_mmu_notifier_register(struct mmu_notifier *mn,
				    struct mm_struct *mm,
				    int take_mmap_sem)
{
	struct mmu_notifier_mm *mmu_notifier_mm;
	int ret;

	BUG_ON(atomic_read(&mm->mm_users) <= 0);

	/*
                                                                      
                
  */
	BUG_ON(!srcu.per_cpu_ref);

	ret = -ENOMEM;
	mmu_notifier_mm = kmalloc(sizeof(struct mmu_notifier_mm), GFP_KERNEL);
	if (unlikely(!mmu_notifier_mm))
		goto out;

	if (take_mmap_sem)
		down_write(&mm->mmap_sem);
	ret = mm_take_all_locks(mm);
	if (unlikely(ret))
		goto out_clean;

	if (!mm_has_notifiers(mm)) {
		INIT_HLIST_HEAD(&mmu_notifier_mm->list);
		spin_lock_init(&mmu_notifier_mm->lock);

		mm->mmu_notifier_mm = mmu_notifier_mm;
		mmu_notifier_mm = NULL;
	}
	atomic_inc(&mm->mm_count);

	/*
                                                           
                                                               
                                                             
                                                             
                                                              
                                  
  */
	spin_lock(&mm->mmu_notifier_mm->lock);
	hlist_add_head(&mn->hlist, &mm->mmu_notifier_mm->list);
	spin_unlock(&mm->mmu_notifier_mm->lock);

	mm_drop_all_locks(mm);
out_clean:
	if (take_mmap_sem)
		up_write(&mm->mmap_sem);
	kfree(mmu_notifier_mm);
out:
	BUG_ON(atomic_read(&mm->mm_users) <= 0);
	return ret;
}

/*
                                                                    
                                                                      
                                                                    
                                                                     
                                                                    
                                                                      
                                                            
                                                                     
                                                                     
                                                                    
                   
 */
int mmu_notifier_register(struct mmu_notifier *mn, struct mm_struct *mm)
{
	return do_mmu_notifier_register(mn, mm, 1);
}
EXPORT_SYMBOL_GPL(mmu_notifier_register);

/*
                                                                  
                          
 */
int __mmu_notifier_register(struct mmu_notifier *mn, struct mm_struct *mm)
{
	return do_mmu_notifier_register(mn, mm, 0);
}
EXPORT_SYMBOL_GPL(__mmu_notifier_register);

/*                                                                  */
void __mmu_notifier_mm_destroy(struct mm_struct *mm)
{
	BUG_ON(!hlist_empty(&mm->mmu_notifier_mm->list));
	kfree(mm->mmu_notifier_mm);
	mm->mmu_notifier_mm = LIST_POISON1; /*       */
}

/*
                                                                
                                                                 
                                                                      
                                                                    
                                                                   
                                                                   
                                                                   
                                                        
 */
void mmu_notifier_unregister(struct mmu_notifier *mn, struct mm_struct *mm)
{
	BUG_ON(atomic_read(&mm->mm_count) <= 0);

	if (!hlist_unhashed(&mn->hlist)) {
		/*
                                                            
                                     
   */
		int id;

		id = srcu_read_lock(&srcu);
		/*
                                                              
                                                       
   */
		if (mn->ops->release)
			mn->ops->release(mn, mm);
		srcu_read_unlock(&srcu, id);

		spin_lock(&mm->mmu_notifier_mm->lock);
		/*
                                                            
                                           
   */
		hlist_del_init_rcu(&mn->hlist);
		spin_unlock(&mm->mmu_notifier_mm->lock);
	}

	/*
                                                              
                                                                 
  */
	synchronize_srcu(&srcu);

	BUG_ON(atomic_read(&mm->mm_count) <= 0);

	mmdrop(mm);
}
EXPORT_SYMBOL_GPL(mmu_notifier_unregister);

static int __init mmu_notifier_init(void)
{
	return init_srcu_struct(&srcu);
}

module_init(mmu_notifier_init);
