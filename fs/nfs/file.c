/*
 *  linux/fs/nfs/file.c
 *
 *  Copyright (C) 1992  Rick Sladkey
 *
 *  Changes Copyright (C) 1994 by Florian La Roche
 *   - Do not copy data too often around in the kernel.
 *   - In nfs_file_read the return value of kmalloc wasn't checked.
 *   - Put in a better version of read look-ahead buffering. Original idea
 *     and implementation by Wai S Kok elekokws@ee.nus.sg.
 *
 *  Expire cache on write to a file by Wai S Kok (Oct 1994).
 *
 *  Total rewrite of read side for new NFS buffer cache.. Linus.
 *
 *  nfs regular file handling functions
 */

#include <linux/module.h>
#include <linux/time.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/fcntl.h>
#include <linux/stat.h>
#include <linux/nfs_fs.h>
#include <linux/nfs_mount.h>
#include <linux/mm.h>
#include <linux/pagemap.h>
#include <linux/aio.h>
#include <linux/gfp.h>
#include <linux/swap.h>

#include <asm/uaccess.h>

#include "delegation.h"
#include "internal.h"
#include "iostat.h"
#include "fscache.h"

#define NFSDBG_FACILITY		NFSDBG_FILE

static const struct vm_operations_struct nfs_file_vm_ops;

/*                                  */
#ifndef IS_SWAPFILE
# define IS_SWAPFILE(inode)	(0)
#endif

int nfs_check_flags(int flags)
{
	if ((flags & (O_APPEND | O_DIRECT)) == (O_APPEND | O_DIRECT))
		return -EINVAL;

	return 0;
}
EXPORT_SYMBOL_GPL(nfs_check_flags);

/*
            
 */
static int
nfs_file_open(struct inode *inode, struct file *filp)
{
	int res;

	dprintk("NFS: open file(%s/%s)\n",
			filp->f_path.dentry->d_parent->d_name.name,
			filp->f_path.dentry->d_name.name);

	nfs_inc_stats(inode, NFSIOS_VFSOPEN);
	res = nfs_check_flags(filp->f_flags);
	if (res)
		return res;

	res = nfs_open(inode, filp);
	return res;
}

int
nfs_file_release(struct inode *inode, struct file *filp)
{
	dprintk("NFS: release(%s/%s)\n",
			filp->f_path.dentry->d_parent->d_name.name,
			filp->f_path.dentry->d_name.name);

	nfs_inc_stats(inode, NFSIOS_VFSRELEASE);
	return nfs_release(inode, filp);
}
EXPORT_SYMBOL_GPL(nfs_file_release);

/* 
                                                 
                                   
                                 
  
                                                                  
                                                                      
                                                                     
                                                                  
                              
 */
static int nfs_revalidate_file_size(struct inode *inode, struct file *filp)
{
	struct nfs_server *server = NFS_SERVER(inode);
	struct nfs_inode *nfsi = NFS_I(inode);

	if (nfs_have_delegated_attributes(inode))
		goto out_noreval;

	if (filp->f_flags & O_DIRECT)
		goto force_reval;
	if (nfsi->cache_validity & NFS_INO_REVAL_PAGECACHE)
		goto force_reval;
	if (nfs_attribute_timeout(inode))
		goto force_reval;
out_noreval:
	return 0;
force_reval:
	return __nfs_revalidate_inode(server, inode);
}

loff_t nfs_file_llseek(struct file *filp, loff_t offset, int whence)
{
	dprintk("NFS: llseek file(%s/%s, %lld, %d)\n",
			filp->f_path.dentry->d_parent->d_name.name,
			filp->f_path.dentry->d_name.name,
			offset, whence);

	/*
                                                                      
                          
  */
	if (whence != SEEK_SET && whence != SEEK_CUR) {
		struct inode *inode = filp->f_mapping->host;

		int retval = nfs_revalidate_file_size(inode, filp);
		if (retval < 0)
			return (loff_t)retval;
	}

	return generic_file_llseek(filp, offset, whence);
}
EXPORT_SYMBOL_GPL(nfs_file_llseek);

/*
                                                     
 */
int
nfs_file_flush(struct file *file, fl_owner_t id)
{
	struct dentry	*dentry = file->f_path.dentry;
	struct inode	*inode = dentry->d_inode;

	dprintk("NFS: flush(%s/%s)\n",
			dentry->d_parent->d_name.name,
			dentry->d_name.name);

	nfs_inc_stats(inode, NFSIOS_VFSFLUSH);
	if ((file->f_mode & FMODE_WRITE) == 0)
		return 0;

	/*
                                                                
                                                     
  */
	if (NFS_PROTO(inode)->have_delegation(inode, FMODE_WRITE))
		return filemap_fdatawrite(file->f_mapping);

	/*                                                  */
	return vfs_fsync(file, 0);
}
EXPORT_SYMBOL_GPL(nfs_file_flush);

ssize_t
nfs_file_read(struct kiocb *iocb, const struct iovec *iov,
		unsigned long nr_segs, loff_t pos)
{
	struct dentry * dentry = iocb->ki_filp->f_path.dentry;
	struct inode * inode = dentry->d_inode;
	ssize_t result;

	if (iocb->ki_filp->f_flags & O_DIRECT)
		return nfs_file_direct_read(iocb, iov, nr_segs, pos, true);

	dprintk("NFS: read(%s/%s, %lu@%lu)\n",
		dentry->d_parent->d_name.name, dentry->d_name.name,
		(unsigned long) iov_length(iov, nr_segs), (unsigned long) pos);

	result = nfs_revalidate_mapping(inode, iocb->ki_filp->f_mapping);
	if (!result) {
		result = generic_file_aio_read(iocb, iov, nr_segs, pos);
		if (result > 0)
			nfs_add_stats(inode, NFSIOS_NORMALREADBYTES, result);
	}
	return result;
}
EXPORT_SYMBOL_GPL(nfs_file_read);

ssize_t
nfs_file_splice_read(struct file *filp, loff_t *ppos,
		     struct pipe_inode_info *pipe, size_t count,
		     unsigned int flags)
{
	struct dentry *dentry = filp->f_path.dentry;
	struct inode *inode = dentry->d_inode;
	ssize_t res;

	dprintk("NFS: splice_read(%s/%s, %lu@%Lu)\n",
		dentry->d_parent->d_name.name, dentry->d_name.name,
		(unsigned long) count, (unsigned long long) *ppos);

	res = nfs_revalidate_mapping(inode, filp->f_mapping);
	if (!res) {
		res = generic_file_splice_read(filp, ppos, pipe, count, flags);
		if (res > 0)
			nfs_add_stats(inode, NFSIOS_NORMALREADBYTES, res);
	}
	return res;
}
EXPORT_SYMBOL_GPL(nfs_file_splice_read);

int
nfs_file_mmap(struct file * file, struct vm_area_struct * vma)
{
	struct dentry *dentry = file->f_path.dentry;
	struct inode *inode = dentry->d_inode;
	int	status;

	dprintk("NFS: mmap(%s/%s)\n",
		dentry->d_parent->d_name.name, dentry->d_name.name);

	/*                                                          
                                                         
  */
	status = generic_file_mmap(file, vma);
	if (!status) {
		vma->vm_ops = &nfs_file_vm_ops;
		status = nfs_revalidate_mapping(inode, file->f_mapping);
	}
	return status;
}
EXPORT_SYMBOL_GPL(nfs_file_mmap);

/*
                                                                      
                                                                     
                                                      
  
                                                                       
                                                                       
                                                                       
                                                                  
                                                                      
                                          
 */
int
nfs_file_fsync_commit(struct file *file, loff_t start, loff_t end, int datasync)
{
	struct dentry *dentry = file->f_path.dentry;
	struct nfs_open_context *ctx = nfs_file_open_context(file);
	struct inode *inode = dentry->d_inode;
	int have_error, do_resend, status;
	int ret = 0;

	dprintk("NFS: fsync file(%s/%s) datasync %d\n",
			dentry->d_parent->d_name.name, dentry->d_name.name,
			datasync);

	nfs_inc_stats(inode, NFSIOS_VFSFSYNC);
	do_resend = test_and_clear_bit(NFS_CONTEXT_RESEND_WRITES, &ctx->flags);
	have_error = test_and_clear_bit(NFS_CONTEXT_ERROR_WRITE, &ctx->flags);
	status = nfs_commit_inode(inode, FLUSH_SYNC);
	have_error |= test_bit(NFS_CONTEXT_ERROR_WRITE, &ctx->flags);
	if (have_error) {
		ret = xchg(&ctx->error, 0);
		if (ret)
			goto out;
	}
	if (status < 0) {
		ret = status;
		goto out;
	}
	do_resend |= test_bit(NFS_CONTEXT_RESEND_WRITES, &ctx->flags);
	if (do_resend)
		ret = -EAGAIN;
out:
	return ret;
}
EXPORT_SYMBOL_GPL(nfs_file_fsync_commit);

static int
nfs_file_fsync(struct file *file, loff_t start, loff_t end, int datasync)
{
	int ret;
	struct inode *inode = file_inode(file);

	do {
		ret = filemap_write_and_wait_range(inode->i_mapping, start, end);
		if (ret != 0)
			break;
		mutex_lock(&inode->i_mutex);
		ret = nfs_file_fsync_commit(file, start, end, datasync);
		mutex_unlock(&inode->i_mutex);
		/*
                                                            
                                                           
                                       
   */
		start = 0;
		end = LLONG_MAX;
	} while (ret == -EAGAIN);

	return ret;
}

/*
                                                                 
                                                               
              
  
                                                                 
                                                                 
                                                                  
                                                                    
                                                                 
        
  
                                                                 
                                                                  
                                                           
                                                                 
                                                                 
                                                                 
                          
 */
static int nfs_want_read_modify_write(struct file *file, struct page *page,
			loff_t pos, unsigned len)
{
	unsigned int pglen = nfs_page_length(page);
	unsigned int offset = pos & (PAGE_CACHE_SIZE - 1);
	unsigned int end = offset + len;

	if ((file->f_mode & FMODE_READ) &&	/*                */
	    !PageUptodate(page) &&		/*           */
	    !PagePrivate(page) &&		/*                      */
	    pglen &&				/*                      */
	    (end < pglen || offset))		/*                          */
		return 1;
	return 0;
}

/*
                                                                        
                                                                     
                        
  
                                                                
                                                                
 */
static int nfs_write_begin(struct file *file, struct address_space *mapping,
			loff_t pos, unsigned len, unsigned flags,
			struct page **pagep, void **fsdata)
{
	int ret;
	pgoff_t index = pos >> PAGE_CACHE_SHIFT;
	struct page *page;
	int once_thru = 0;

	dfprintk(PAGECACHE, "NFS: write_begin(%s/%s(%ld), %u@%lld)\n",
		file->f_path.dentry->d_parent->d_name.name,
		file->f_path.dentry->d_name.name,
		mapping->host->i_ino, len, (long long) pos);

start:
	/*
                                                               
                
  */
	ret = wait_on_bit(&NFS_I(mapping->host)->flags, NFS_INO_FLUSHING,
			nfs_wait_bit_killable, TASK_KILLABLE);
	if (ret)
		return ret;

	page = grab_cache_page_write_begin(mapping, index, flags);
	if (!page)
		return -ENOMEM;
	*pagep = page;

	ret = nfs_flush_incompatible(file, page);
	if (ret) {
		unlock_page(page);
		page_cache_release(page);
	} else if (!once_thru &&
		   nfs_want_read_modify_write(file, page, pos, len)) {
		once_thru = 1;
		ret = nfs_readpage(file, page);
		page_cache_release(page);
		if (!ret)
			goto start;
	}
	return ret;
}

static int nfs_write_end(struct file *file, struct address_space *mapping,
			loff_t pos, unsigned len, unsigned copied,
			struct page *page, void *fsdata)
{
	unsigned offset = pos & (PAGE_CACHE_SIZE - 1);
	int status;

	dfprintk(PAGECACHE, "NFS: write_end(%s/%s(%ld), %u@%lld)\n",
		file->f_path.dentry->d_parent->d_name.name,
		file->f_path.dentry->d_name.name,
		mapping->host->i_ino, len, (long long) pos);

	/*
                                                                    
                                                                
  */
	if (!PageUptodate(page)) {
		unsigned pglen = nfs_page_length(page);
		unsigned end = offset + len;

		if (pglen == 0) {
			zero_user_segments(page, 0, offset,
					end, PAGE_CACHE_SIZE);
			SetPageUptodate(page);
		} else if (end >= pglen) {
			zero_user_segment(page, end, PAGE_CACHE_SIZE);
			if (offset == 0)
				SetPageUptodate(page);
		} else
			zero_user_segment(page, pglen, PAGE_CACHE_SIZE);
	}

	status = nfs_updatepage(file, page, offset, copied);

	unlock_page(page);
	page_cache_release(page);

	if (status < 0)
		return status;
	NFS_I(mapping->host)->write_io += copied;
	return copied;
}

/*
                                        
                                                                            
                      
                                                                 
                           
 */
static void nfs_invalidate_page(struct page *page, unsigned long offset)
{
	dfprintk(PAGECACHE, "NFS: invalidate_page(%p, %lu)\n", page, offset);

	if (offset != 0)
		return;
	/*                                          */
	nfs_wb_page_cancel(page_file_mapping(page)->host, page);

	nfs_fscache_invalidate_page(page, page->mapping->host);
}

/*
                                                              
                                                                 
                           
                                                      
 */
static int nfs_release_page(struct page *page, gfp_t gfp)
{
	struct address_space *mapping = page->mapping;

	dfprintk(PAGECACHE, "NFS: release_page(%p)\n", page);

	/*                                                              
                                                          
  */
	if (mapping && (gfp & GFP_KERNEL) == GFP_KERNEL &&
	    !(current->flags & PF_FSTRANS)) {
		int how = FLUSH_SYNC;

		/*                                                     */
		if (current_is_kswapd())
			how = 0;
		nfs_commit_inode(mapping->host, how);
	}
	/*                                                        */
	if (PagePrivate(page))
		return 0;
	return nfs_fscache_release_page(page, gfp);
}

/*
                                                                          
                                                                             
            
                                                              
                           
                                             
 */
static int nfs_launder_page(struct page *page)
{
	struct inode *inode = page_file_mapping(page)->host;
	struct nfs_inode *nfsi = NFS_I(inode);

	dfprintk(PAGECACHE, "NFS: launder_page(%ld, %llu)\n",
		inode->i_ino, (long long)page_offset(page));

	nfs_fscache_wait_on_page_write(nfsi, page);
	return nfs_wb_page(inode, page);
}

#ifdef CONFIG_NFS_SWAP
static int nfs_swap_activate(struct swap_info_struct *sis, struct file *file,
						sector_t *span)
{
	*span = sis->pages;
	return xs_swapper(NFS_CLIENT(file->f_mapping->host)->cl_xprt, 1);
}

static void nfs_swap_deactivate(struct file *file)
{
	xs_swapper(NFS_CLIENT(file->f_mapping->host)->cl_xprt, 0);
}
#endif

const struct address_space_operations nfs_file_aops = {
	.readpage = nfs_readpage,
	.readpages = nfs_readpages,
	.set_page_dirty = __set_page_dirty_nobuffers,
	.writepage = nfs_writepage,
	.writepages = nfs_writepages,
	.write_begin = nfs_write_begin,
	.write_end = nfs_write_end,
	.invalidatepage = nfs_invalidate_page,
	.releasepage = nfs_release_page,
	.direct_IO = nfs_direct_IO,
	.migratepage = nfs_migrate_page,
	.launder_page = nfs_launder_page,
	.error_remove_page = generic_error_remove_page,
#ifdef CONFIG_NFS_SWAP
	.swap_activate = nfs_swap_activate,
	.swap_deactivate = nfs_swap_deactivate,
#endif
};

/*
                                                                      
                                                                        
                          
 */
static int nfs_vm_page_mkwrite(struct vm_area_struct *vma, struct vm_fault *vmf)
{
	struct page *page = vmf->page;
	struct file *filp = vma->vm_file;
	struct dentry *dentry = filp->f_path.dentry;
	unsigned pagelen;
	int ret = VM_FAULT_NOPAGE;
	struct address_space *mapping;

	dfprintk(PAGECACHE, "NFS: vm_page_mkwrite(%s/%s(%ld), offset %lld)\n",
		dentry->d_parent->d_name.name, dentry->d_name.name,
		filp->f_mapping->host->i_ino,
		(long long)page_offset(page));

	/*                                                   */
	nfs_fscache_wait_on_page_write(NFS_I(dentry->d_inode), page);

	lock_page(page);
	mapping = page_file_mapping(page);
	if (mapping != dentry->d_inode->i_mapping)
		goto out_unlock;

	wait_on_page_writeback(page);

	pagelen = nfs_page_length(page);
	if (pagelen == 0)
		goto out_unlock;

	ret = VM_FAULT_LOCKED;
	if (nfs_flush_incompatible(filp, page) == 0 &&
	    nfs_updatepage(filp, page, 0, pagelen) == 0)
		goto out;

	ret = VM_FAULT_SIGBUS;
out_unlock:
	unlock_page(page);
out:
	return ret;
}

static const struct vm_operations_struct nfs_file_vm_ops = {
	.fault = filemap_fault,
	.page_mkwrite = nfs_vm_page_mkwrite,
	.remap_pages = generic_file_remap_pages,
};

static int nfs_need_sync_write(struct file *filp, struct inode *inode)
{
	struct nfs_open_context *ctx;

	if (IS_SYNC(inode) || (filp->f_flags & O_DSYNC))
		return 1;
	ctx = nfs_file_open_context(filp);
	if (test_bit(NFS_CONTEXT_ERROR_WRITE, &ctx->flags))
		return 1;
	return 0;
}

ssize_t nfs_file_write(struct kiocb *iocb, const struct iovec *iov,
		       unsigned long nr_segs, loff_t pos)
{
	struct dentry * dentry = iocb->ki_filp->f_path.dentry;
	struct inode * inode = dentry->d_inode;
	unsigned long written = 0;
	ssize_t result;
	size_t count = iov_length(iov, nr_segs);

	if (iocb->ki_filp->f_flags & O_DIRECT)
		return nfs_file_direct_write(iocb, iov, nr_segs, pos, true);

	dprintk("NFS: write(%s/%s, %lu@%Ld)\n",
		dentry->d_parent->d_name.name, dentry->d_name.name,
		(unsigned long) count, (long long) pos);

	result = -EBUSY;
	if (IS_SWAPFILE(inode))
		goto out_swapfile;
	/*
                                                             
  */
	if (iocb->ki_filp->f_flags & O_APPEND) {
		result = nfs_revalidate_file_size(inode, iocb->ki_filp);
		if (result)
			goto out;
	}

	result = count;
	if (!count)
		goto out;

	result = generic_file_aio_write(iocb, iov, nr_segs, pos);
	if (result > 0)
		written = result;

	/*                                               */
	if (result >= 0 && nfs_need_sync_write(iocb->ki_filp, inode)) {
		int err = vfs_fsync(iocb->ki_filp, 0);
		if (err < 0)
			result = err;
	}
	if (result > 0)
		nfs_add_stats(inode, NFSIOS_NORMALWRITTENBYTES, written);
out:
	return result;

out_swapfile:
	printk(KERN_INFO "NFS: attempt to write to active swap file!\n");
	goto out;
}
EXPORT_SYMBOL_GPL(nfs_file_write);

ssize_t nfs_file_splice_write(struct pipe_inode_info *pipe,
			      struct file *filp, loff_t *ppos,
			      size_t count, unsigned int flags)
{
	struct dentry *dentry = filp->f_path.dentry;
	struct inode *inode = dentry->d_inode;
	unsigned long written = 0;
	ssize_t ret;

	dprintk("NFS splice_write(%s/%s, %lu@%llu)\n",
		dentry->d_parent->d_name.name, dentry->d_name.name,
		(unsigned long) count, (unsigned long long) *ppos);

	/*
                                                                        
  */

	ret = generic_file_splice_write(pipe, filp, ppos, count, flags);
	if (ret > 0)
		written = ret;

	if (ret >= 0 && nfs_need_sync_write(filp, inode)) {
		int err = vfs_fsync(filp, 0);
		if (err < 0)
			ret = err;
	}
	if (ret > 0)
		nfs_add_stats(inode, NFSIOS_NORMALWRITTENBYTES, written);
	return ret;
}
EXPORT_SYMBOL_GPL(nfs_file_splice_write);

static int
do_getlk(struct file *filp, int cmd, struct file_lock *fl, int is_local)
{
	struct inode *inode = filp->f_mapping->host;
	int status = 0;
	unsigned int saved_type = fl->fl_type;

	/*                         */
	posix_test_lock(filp, fl);
	if (fl->fl_type != F_UNLCK) {
		/*                  */
		goto out;
	}
	fl->fl_type = saved_type;

	if (NFS_PROTO(inode)->have_delegation(inode, FMODE_READ))
		goto out_noconflict;

	if (is_local)
		goto out_noconflict;

	status = NFS_PROTO(inode)->lock(filp, cmd, fl);
out:
	return status;
out_noconflict:
	fl->fl_type = F_UNLCK;
	goto out;
}

static int do_vfs_lock(struct file *file, struct file_lock *fl)
{
	int res = 0;
	switch (fl->fl_flags & (FL_POSIX|FL_FLOCK)) {
		case FL_POSIX:
			res = posix_lock_file_wait(file, fl);
			break;
		case FL_FLOCK:
			res = flock_lock_file_wait(file, fl);
			break;
		default:
			BUG();
	}
	return res;
}

static int
do_unlk(struct file *filp, int cmd, struct file_lock *fl, int is_local)
{
	struct inode *inode = filp->f_mapping->host;
	struct nfs_lock_context *l_ctx;
	int status;

	/*
                                                  
                
  */
	nfs_sync_mapping(filp->f_mapping);

	l_ctx = nfs_get_lock_context(nfs_file_open_context(filp));
	if (!IS_ERR(l_ctx)) {
		status = nfs_iocounter_wait(&l_ctx->io_count);
		nfs_put_lock_context(l_ctx);
		if (status < 0)
			return status;
	}

	/*                   
                                                                   
                                       
  */
	/*
                                                                    
                   
  */
	if (!is_local)
		status = NFS_PROTO(inode)->lock(filp, cmd, fl);
	else
		status = do_vfs_lock(filp, fl);
	return status;
}

static int
is_time_granular(struct timespec *ts) {
	return ((ts->tv_sec == 0) && (ts->tv_nsec <= 1000));
}

static int
do_setlk(struct file *filp, int cmd, struct file_lock *fl, int is_local)
{
	struct inode *inode = filp->f_mapping->host;
	int status;

	/*
                                                  
                
  */
	status = nfs_sync_mapping(filp->f_mapping);
	if (status != 0)
		goto out;

	/*
                                                                    
                   
  */
	if (!is_local)
		status = NFS_PROTO(inode)->lock(filp, cmd, fl);
	else
		status = do_vfs_lock(filp, fl);
	if (status < 0)
		goto out;

	/*
                                                               
                                                             
                                         
   
                                                      
  */
	nfs_sync_mapping(filp->f_mapping);
	if (!NFS_PROTO(inode)->have_delegation(inode, FMODE_READ)) {
		if (is_time_granular(&NFS_SERVER(inode)->time_delta))
			__nfs_revalidate_inode(NFS_SERVER(inode), inode);
		else
			nfs_zap_caches(inode);
	}
out:
	return status;
}

/*
                             
 */
int nfs_lock(struct file *filp, int cmd, struct file_lock *fl)
{
	struct inode *inode = filp->f_mapping->host;
	int ret = -ENOLCK;
	int is_local = 0;

	dprintk("NFS: lock(%s/%s, t=%x, fl=%x, r=%lld:%lld)\n",
			filp->f_path.dentry->d_parent->d_name.name,
			filp->f_path.dentry->d_name.name,
			fl->fl_type, fl->fl_flags,
			(long long)fl->fl_start, (long long)fl->fl_end);

	nfs_inc_stats(inode, NFSIOS_VFSLOCK);

	/*                             */
	if (__mandatory_lock(inode) && fl->fl_type != F_UNLCK)
		goto out_err;

	if (NFS_SERVER(inode)->flags & NFS_MOUNT_LOCAL_FCNTL)
		is_local = 1;

	if (NFS_PROTO(inode)->lock_check_bounds != NULL) {
		ret = NFS_PROTO(inode)->lock_check_bounds(fl);
		if (ret < 0)
			goto out_err;
	}

	if (IS_GETLK(cmd))
		ret = do_getlk(filp, cmd, fl, is_local);
	else if (fl->fl_type == F_UNLCK)
		ret = do_unlk(filp, cmd, fl, is_local);
	else
		ret = do_setlk(filp, cmd, fl, is_local);
out_err:
	return ret;
}
EXPORT_SYMBOL_GPL(nfs_lock);

/*
                             
 */
int nfs_flock(struct file *filp, int cmd, struct file_lock *fl)
{
	struct inode *inode = filp->f_mapping->host;
	int is_local = 0;

	dprintk("NFS: flock(%s/%s, t=%x, fl=%x)\n",
			filp->f_path.dentry->d_parent->d_name.name,
			filp->f_path.dentry->d_name.name,
			fl->fl_type, fl->fl_flags);

	if (!(fl->fl_flags & FL_FLOCK))
		return -ENOLCK;

	/*
                                                                      
                                                                    
                                                                     
                                  
  */
	if (fl->fl_type & LOCK_MAND)
		return -EINVAL;

	if (NFS_SERVER(inode)->flags & NFS_MOUNT_LOCAL_FLOCK)
		is_local = 1;

	/*                                                                */
	fl->fl_owner = (fl_owner_t)filp;
	fl->fl_start = 0;
	fl->fl_end = OFFSET_MAX;

	if (fl->fl_type == F_UNLCK)
		return do_unlk(filp, cmd, fl, is_local);
	return do_setlk(filp, cmd, fl, is_local);
}
EXPORT_SYMBOL_GPL(nfs_flock);

/*
                                                                          
                                                        
 */
int nfs_setlease(struct file *file, long arg, struct file_lock **fl)
{
	dprintk("NFS: setlease(%s/%s, arg=%ld)\n",
			file->f_path.dentry->d_parent->d_name.name,
			file->f_path.dentry->d_name.name, arg);
	return -EINVAL;
}
EXPORT_SYMBOL_GPL(nfs_setlease);

const struct file_operations nfs_file_operations = {
	.llseek		= nfs_file_llseek,
	.read		= do_sync_read,
	.write		= do_sync_write,
	.aio_read	= nfs_file_read,
	.aio_write	= nfs_file_write,
	.mmap		= nfs_file_mmap,
	.open		= nfs_file_open,
	.flush		= nfs_file_flush,
	.release	= nfs_file_release,
	.fsync		= nfs_file_fsync,
	.lock		= nfs_lock,
	.flock		= nfs_flock,
	.splice_read	= nfs_file_splice_read,
	.splice_write	= nfs_file_splice_write,
	.check_flags	= nfs_check_flags,
	.setlease	= nfs_setlease,
};
EXPORT_SYMBOL_GPL(nfs_file_operations);
