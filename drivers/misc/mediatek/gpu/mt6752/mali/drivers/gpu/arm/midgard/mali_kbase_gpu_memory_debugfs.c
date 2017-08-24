/*
 *
 * (C) COPYRIGHT ARM Limited. All rights reserved.
 *
 * This program is free software and is provided to you under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation, and any use by you of this program is subject to the terms
 * of such GNU licence.
 *
 * A copy of the licence is included with the program, and can also be obtained
 * from Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 */



#include <mali_kbase_gpu_memory_debugfs.h>

#ifdef CONFIG_DEBUG_FS
/*                                                   
  
                                                                           
                                                      
  
                                 
                                             
  
                                                              
                                        
 */

static int kbasep_gpu_memory_seq_show(struct seq_file *sfile, void *data)
{
	ssize_t ret = 0;
	struct list_head *entry;
	const struct list_head *kbdev_list;
	kbdev_list = kbase_dev_list_get();
	list_for_each(entry, kbdev_list) {
		struct kbase_device *kbdev = NULL;
		struct kbasep_kctx_list_element *element;

		kbdev = list_entry(entry, struct kbase_device, entry);
		/*                                                       */
		ret = seq_printf(sfile, "%-16s  %10u\n", \
				kbdev->devname, \
				atomic_read(&(kbdev->memdev.used_pages)));
		mutex_lock(&kbdev->kctx_list_lock);
		list_for_each_entry(element, &kbdev->kctx_list, link) {
			/*                                              
                           */
			ret = seq_printf(sfile, "  %s-0x%p %10u\n", \
				"kctx",
				element->kctx, \
				atomic_read(&(element->kctx->used_pages)));
		}
		mutex_unlock(&kbdev->kctx_list_lock);
	}
	kbase_dev_list_put(kbdev_list);
	return ret;
}

/*
                                                           
 */
STATIC int kbasep_gpu_memory_debugfs_open(struct inode *in, struct file *file)
{
	return single_open(file, kbasep_gpu_memory_seq_show , NULL);
}

static const struct file_operations kbasep_gpu_memory_debugfs_fops = {
	.open = kbasep_gpu_memory_debugfs_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release_private,
};

/*
                                           
 */
mali_error kbasep_gpu_memory_debugfs_init(struct kbase_device *kbdev)
{
	kbdev->gpu_memory_dentry = debugfs_create_file("gpu_memory", \
					S_IRUGO, \
					kbdev->mali_debugfs_directory, \
					NULL, \
					&kbasep_gpu_memory_debugfs_fops);
	if (IS_ERR(kbdev->gpu_memory_dentry))
		return MALI_ERROR_FUNCTION_FAILED;

	return MALI_ERROR_NONE;
}

/*
                                          
 */
void kbasep_gpu_memory_debugfs_term(struct kbase_device *kbdev)
{
	debugfs_remove(kbdev->gpu_memory_dentry);
}
#else
/*
                                              
 */
mali_error kbasep_gpu_memory_debugfs_init(struct kbase_device *kbdev)
{
	return MALI_ERROR_NONE;
}
void kbasep_gpu_memory_debugfs_term(struct kbase_device *kbdev)
{
}
#endif
