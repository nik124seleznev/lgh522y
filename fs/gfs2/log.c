/*
 * Copyright (C) Sistina Software, Inc.  1997-2003 All rights reserved.
 * Copyright (C) 2004-2007 Red Hat, Inc.  All rights reserved.
 *
 * This copyrighted material is made available to anyone wishing to use,
 * modify, copy, or redistribute it subject to the terms and conditions
 * of the GNU General Public License version 2.
 */

#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/completion.h>
#include <linux/buffer_head.h>
#include <linux/gfs2_ondisk.h>
#include <linux/crc32.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/freezer.h>
#include <linux/bio.h>
#include <linux/writeback.h>
#include <linux/list_sort.h>

#include "gfs2.h"
#include "incore.h"
#include "bmap.h"
#include "glock.h"
#include "log.h"
#include "lops.h"
#include "meta_io.h"
#include "util.h"
#include "dir.h"
#include "trace_gfs2.h"

/* 
                                  
                       
                                     
                                     
  
                                                                              
                                   
  
                                                             
 */

unsigned int gfs2_struct2blk(struct gfs2_sbd *sdp, unsigned int nstruct,
			     unsigned int ssize)
{
	unsigned int blks;
	unsigned int first, second;

	blks = 1;
	first = (sdp->sd_sb.sb_bsize - sizeof(struct gfs2_log_descriptor)) / ssize;

	if (nstruct > first) {
		second = (sdp->sd_sb.sb_bsize -
			  sizeof(struct gfs2_meta_header)) / ssize;
		blks += DIV_ROUND_UP(nstruct - first, second);
	}

	return blks;
}

/* 
                                                                               
                                                
                                  
  
                                                         
  
 */

void gfs2_remove_from_ail(struct gfs2_bufdata *bd)
{
	bd->bd_tr = NULL;
	list_del_init(&bd->bd_ail_st_list);
	list_del_init(&bd->bd_ail_gl_list);
	atomic_dec(&bd->bd_gl->gl_ail_count);
	brelse(bd->bd_bh);
}

/* 
                                                       
                       
                                        
                         
  
 */

static int gfs2_ail1_start_one(struct gfs2_sbd *sdp,
			       struct writeback_control *wbc,
			       struct gfs2_trans *tr)
__releases(&sdp->sd_ail_lock)
__acquires(&sdp->sd_ail_lock)
{
	struct gfs2_glock *gl = NULL;
	struct address_space *mapping;
	struct gfs2_bufdata *bd, *s;
	struct buffer_head *bh;

	list_for_each_entry_safe_reverse(bd, s, &tr->tr_ail1_list, bd_ail_st_list) {
		bh = bd->bd_bh;

		gfs2_assert(sdp, bd->bd_tr == tr);

		if (!buffer_busy(bh)) {
			if (!buffer_uptodate(bh))
				gfs2_io_error_bh(sdp, bh);
			list_move(&bd->bd_ail_st_list, &tr->tr_ail2_list);
			continue;
		}

		if (!buffer_dirty(bh))
			continue;
		if (gl == bd->bd_gl)
			continue;
		gl = bd->bd_gl;
		list_move(&bd->bd_ail_st_list, &tr->tr_ail1_list);
		mapping = bh->b_page->mapping;
		if (!mapping)
			continue;
		spin_unlock(&sdp->sd_ail_lock);
		generic_writepages(mapping, wbc);
		spin_lock(&sdp->sd_ail_lock);
		if (wbc->nr_to_write <= 0)
			break;
		return 1;
	}

	return 0;
}


/* 
                                                          
                        
                                        
  
                                                                
                              
 */

void gfs2_ail1_flush(struct gfs2_sbd *sdp, struct writeback_control *wbc)
{
	struct list_head *head = &sdp->sd_ail1_list;
	struct gfs2_trans *tr;

	trace_gfs2_ail_flush(sdp, wbc, 1);
	spin_lock(&sdp->sd_ail_lock);
restart:
	list_for_each_entry_reverse(tr, head, tr_list) {
		if (wbc->nr_to_write <= 0)
			break;
		if (gfs2_ail1_start_one(sdp, wbc, tr))
			goto restart;
	}
	spin_unlock(&sdp->sd_ail_lock);
	trace_gfs2_ail_flush(sdp, wbc, 0);
}

/* 
                                                        
                       
 */

static void gfs2_ail1_start(struct gfs2_sbd *sdp)
{
	struct writeback_control wbc = {
		.sync_mode = WB_SYNC_NONE,
		.nr_to_write = LONG_MAX,
		.range_start = 0,
		.range_end = LLONG_MAX,
	};

	return gfs2_ail1_flush(sdp, &wbc);
}

/* 
                                                                                
                       
                     
  
 */

static void gfs2_ail1_empty_one(struct gfs2_sbd *sdp, struct gfs2_trans *tr)
{
	struct gfs2_bufdata *bd, *s;
	struct buffer_head *bh;

	list_for_each_entry_safe_reverse(bd, s, &tr->tr_ail1_list,
					 bd_ail_st_list) {
		bh = bd->bd_bh;
		gfs2_assert(sdp, bd->bd_tr == tr);
		if (buffer_busy(bh))
			continue;
		if (!buffer_uptodate(bh))
			gfs2_io_error_bh(sdp, bh);
		list_move(&bd->bd_ail_st_list, &tr->tr_ail2_list);
	}

}

/* 
                                                
                       
  
                                                                
 */

static int gfs2_ail1_empty(struct gfs2_sbd *sdp)
{
	struct gfs2_trans *tr, *s;
	int ret;

	spin_lock(&sdp->sd_ail_lock);
	list_for_each_entry_safe_reverse(tr, s, &sdp->sd_ail1_list, tr_list) {
		gfs2_ail1_empty_one(sdp, tr);
		if (list_empty(&tr->tr_ail1_list))
			list_move(&tr->tr_list, &sdp->sd_ail2_list);
		else
			break;
	}
	ret = list_empty(&sdp->sd_ail1_list);
	spin_unlock(&sdp->sd_ail_lock);

	return ret;
}

static void gfs2_ail1_wait(struct gfs2_sbd *sdp)
{
	struct gfs2_trans *tr;
	struct gfs2_bufdata *bd;
	struct buffer_head *bh;

	spin_lock(&sdp->sd_ail_lock);
	list_for_each_entry_reverse(tr, &sdp->sd_ail1_list, tr_list) {
		list_for_each_entry(bd, &tr->tr_ail1_list, bd_ail_st_list) {
			bh = bd->bd_bh;
			if (!buffer_locked(bh))
				continue;
			get_bh(bh);
			spin_unlock(&sdp->sd_ail_lock);
			wait_on_buffer(bh);
			brelse(bh);
			return;
		}
	}
	spin_unlock(&sdp->sd_ail_lock);
}

/* 
                                                                                
                       
                     
  
 */

static void gfs2_ail2_empty_one(struct gfs2_sbd *sdp, struct gfs2_trans *tr)
{
	struct list_head *head = &tr->tr_ail2_list;
	struct gfs2_bufdata *bd;

	while (!list_empty(head)) {
		bd = list_entry(head->prev, struct gfs2_bufdata,
				bd_ail_st_list);
		gfs2_assert(sdp, bd->bd_tr == tr);
		gfs2_remove_from_ail(bd);
	}
}

static void ail2_empty(struct gfs2_sbd *sdp, unsigned int new_tail)
{
	struct gfs2_trans *tr, *safe;
	unsigned int old_tail = sdp->sd_log_tail;
	int wrap = (new_tail < old_tail);
	int a, b, rm;

	spin_lock(&sdp->sd_ail_lock);

	list_for_each_entry_safe(tr, safe, &sdp->sd_ail2_list, tr_list) {
		a = (old_tail <= tr->tr_first);
		b = (tr->tr_first < new_tail);
		rm = (wrap) ? (a || b) : (a && b);
		if (!rm)
			continue;

		gfs2_ail2_empty_one(sdp, tr);
		list_del(&tr->tr_list);
		gfs2_assert_warn(sdp, list_empty(&tr->tr_ail1_list));
		gfs2_assert_warn(sdp, list_empty(&tr->tr_ail2_list));
		kfree(tr);
	}

	spin_unlock(&sdp->sd_ail_lock);
}

/* 
                                            
                            
                                         
  
                                                                        
                                                                
                                                                        
                                                                       
                                                 
  
                                                                      
                                                                         
                                                                         
                                                                         
                                    
  
                 
 */

int gfs2_log_reserve(struct gfs2_sbd *sdp, unsigned int blks)
{
	unsigned reserved_blks = 6 * (4096 / sdp->sd_vfs->s_blocksize);
	unsigned wanted = blks + reserved_blks;
	DEFINE_WAIT(wait);
	int did_wait = 0;
	unsigned int free_blocks;

	if (gfs2_assert_warn(sdp, blks) ||
	    gfs2_assert_warn(sdp, blks <= sdp->sd_jdesc->jd_blocks))
		return -EINVAL;
retry:
	free_blocks = atomic_read(&sdp->sd_log_blks_free);
	if (unlikely(free_blocks <= wanted)) {
		do {
			prepare_to_wait_exclusive(&sdp->sd_log_waitq, &wait,
					TASK_UNINTERRUPTIBLE);
			wake_up(&sdp->sd_logd_waitq);
			did_wait = 1;
			if (atomic_read(&sdp->sd_log_blks_free) <= wanted)
				io_schedule();
			free_blocks = atomic_read(&sdp->sd_log_blks_free);
		} while(free_blocks <= wanted);
		finish_wait(&sdp->sd_log_waitq, &wait);
	}
	if (atomic_cmpxchg(&sdp->sd_log_blks_free, free_blocks,
				free_blocks - blks) != free_blocks)
		goto retry;
	trace_gfs2_log_blocks(sdp, -blks);

	/*
                                                                   
                         
  */
	if (unlikely(did_wait))
		wake_up(&sdp->sd_log_waitq);

	down_read(&sdp->sd_log_flush_lock);

	return 0;
}

/* 
                                                             
                            
                                                    
                                              
  
                                                                
                          
  
                                  
 */

static inline unsigned int log_distance(struct gfs2_sbd *sdp, unsigned int newer,
					unsigned int older)
{
	int dist;

	dist = newer - older;
	if (dist < 0)
		dist += sdp->sd_jdesc->jd_blocks;

	return dist;
}

/* 
                                                                 
                                                            
                            
  
                                                                       
                                                                          
                                                                         
                                                               
                                                                       
                                                                             
  
                                                                         
                                                                      
                                                                         
                                                                       
                                                                               
                                                                         
                                                       
  
                                                                            
                              
  
                                         
 */
static unsigned int calc_reserved(struct gfs2_sbd *sdp)
{
	unsigned int reserved = 0;
	unsigned int mbuf_limit, metabufhdrs_needed;
	unsigned int dbuf_limit, databufhdrs_needed;
	unsigned int revokes = 0;

	mbuf_limit = buf_limit(sdp);
	metabufhdrs_needed = (sdp->sd_log_commited_buf +
			      (mbuf_limit - 1)) / mbuf_limit;
	dbuf_limit = databuf_limit(sdp);
	databufhdrs_needed = (sdp->sd_log_commited_databuf +
			      (dbuf_limit - 1)) / dbuf_limit;

	if (sdp->sd_log_commited_revoke > 0)
		revokes = gfs2_struct2blk(sdp, sdp->sd_log_commited_revoke,
					  sizeof(u64));

	reserved = sdp->sd_log_commited_buf + metabufhdrs_needed +
		sdp->sd_log_commited_databuf + databufhdrs_needed +
		revokes;
	/*                            */
	if (reserved)
		reserved++;
	return reserved;
}

static unsigned int current_tail(struct gfs2_sbd *sdp)
{
	struct gfs2_trans *tr;
	unsigned int tail;

	spin_lock(&sdp->sd_ail_lock);

	if (list_empty(&sdp->sd_ail1_list)) {
		tail = sdp->sd_log_head;
	} else {
		tr = list_entry(sdp->sd_ail1_list.prev, struct gfs2_trans,
				tr_list);
		tail = tr->tr_first;
	}

	spin_unlock(&sdp->sd_ail_lock);

	return tail;
}

static void log_pull_tail(struct gfs2_sbd *sdp, unsigned int new_tail)
{
	unsigned int dist = log_distance(sdp, new_tail, sdp->sd_log_tail);

	ail2_empty(sdp, new_tail);

	atomic_add(dist, &sdp->sd_log_blks_free);
	trace_gfs2_log_blocks(sdp, dist);
	gfs2_assert_withdraw(sdp, atomic_read(&sdp->sd_log_blks_free) <=
			     sdp->sd_jdesc->jd_blocks);

	sdp->sd_log_tail = new_tail;
}


static void log_flush_wait(struct gfs2_sbd *sdp)
{
	DEFINE_WAIT(wait);

	if (atomic_read(&sdp->sd_log_in_flight)) {
		do {
			prepare_to_wait(&sdp->sd_log_flush_wait, &wait,
					TASK_UNINTERRUPTIBLE);
			if (atomic_read(&sdp->sd_log_in_flight))
				io_schedule();
		} while(atomic_read(&sdp->sd_log_in_flight));
		finish_wait(&sdp->sd_log_flush_wait, &wait);
	}
}

static int ip_cmp(void *priv, struct list_head *a, struct list_head *b)
{
	struct gfs2_inode *ipa, *ipb;

	ipa = list_entry(a, struct gfs2_inode, i_ordered);
	ipb = list_entry(b, struct gfs2_inode, i_ordered);

	if (ipa->i_no_addr < ipb->i_no_addr)
		return -1;
	if (ipa->i_no_addr > ipb->i_no_addr)
		return 1;
	return 0;
}

static void gfs2_ordered_write(struct gfs2_sbd *sdp)
{
	struct gfs2_inode *ip;
	LIST_HEAD(written);

	spin_lock(&sdp->sd_ordered_lock);
	list_sort(NULL, &sdp->sd_log_le_ordered, &ip_cmp);
	while (!list_empty(&sdp->sd_log_le_ordered)) {
		ip = list_entry(sdp->sd_log_le_ordered.next, struct gfs2_inode, i_ordered);
		list_move(&ip->i_ordered, &written);
		if (ip->i_inode.i_mapping->nrpages == 0)
			continue;
		spin_unlock(&sdp->sd_ordered_lock);
		filemap_fdatawrite(ip->i_inode.i_mapping);
		spin_lock(&sdp->sd_ordered_lock);
	}
	list_splice(&written, &sdp->sd_log_le_ordered);
	spin_unlock(&sdp->sd_ordered_lock);
}

static void gfs2_ordered_wait(struct gfs2_sbd *sdp)
{
	struct gfs2_inode *ip;

	spin_lock(&sdp->sd_ordered_lock);
	while (!list_empty(&sdp->sd_log_le_ordered)) {
		ip = list_entry(sdp->sd_log_le_ordered.next, struct gfs2_inode, i_ordered);
		list_del(&ip->i_ordered);
		WARN_ON(!test_and_clear_bit(GIF_ORDERED, &ip->i_flags));
		if (ip->i_inode.i_mapping->nrpages == 0)
			continue;
		spin_unlock(&sdp->sd_ordered_lock);
		filemap_fdatawait(ip->i_inode.i_mapping);
		spin_lock(&sdp->sd_ordered_lock);
	}
	spin_unlock(&sdp->sd_ordered_lock);
}

void gfs2_ordered_del_inode(struct gfs2_inode *ip)
{
	struct gfs2_sbd *sdp = GFS2_SB(&ip->i_inode);

	spin_lock(&sdp->sd_ordered_lock);
	if (test_and_clear_bit(GIF_ORDERED, &ip->i_flags))
		list_del(&ip->i_ordered);
	spin_unlock(&sdp->sd_ordered_lock);
}

/* 
                                                                
                            
  
                                                 
 */

static void log_write_header(struct gfs2_sbd *sdp, u32 flags)
{
	struct gfs2_log_header *lh;
	unsigned int tail;
	u32 hash;
	int rw = WRITE_FLUSH_FUA | REQ_META;
	struct page *page = mempool_alloc(gfs2_page_pool, GFP_NOIO);
	lh = page_address(page);
	clear_page(lh);

	gfs2_ail1_empty(sdp);
	tail = current_tail(sdp);

	lh->lh_header.mh_magic = cpu_to_be32(GFS2_MAGIC);
	lh->lh_header.mh_type = cpu_to_be32(GFS2_METATYPE_LH);
	lh->lh_header.__pad0 = cpu_to_be64(0);
	lh->lh_header.mh_format = cpu_to_be32(GFS2_FORMAT_LH);
	lh->lh_header.mh_jid = cpu_to_be32(sdp->sd_jdesc->jd_jid);
	lh->lh_sequence = cpu_to_be64(sdp->sd_log_sequence++);
	lh->lh_flags = cpu_to_be32(flags);
	lh->lh_tail = cpu_to_be32(tail);
	lh->lh_blkno = cpu_to_be32(sdp->sd_log_flush_head);
	hash = gfs2_disk_hash(page_address(page), sizeof(struct gfs2_log_header));
	lh->lh_hash = cpu_to_be32(hash);

	if (test_bit(SDF_NOBARRIERS, &sdp->sd_flags)) {
		gfs2_ordered_wait(sdp);
		log_flush_wait(sdp);
		rw = WRITE_SYNC | REQ_META | REQ_PRIO;
	}

	sdp->sd_log_idle = (tail == sdp->sd_log_flush_head);
	gfs2_log_write_page(sdp, page);
	gfs2_log_flush_bio(sdp, rw);
	log_flush_wait(sdp);

	if (sdp->sd_log_tail != tail)
		log_pull_tail(sdp, tail);
}

/* 
                                               
                       
                                                                          
  
 */

void gfs2_log_flush(struct gfs2_sbd *sdp, struct gfs2_glock *gl)
{
	struct gfs2_trans *tr;

	down_write(&sdp->sd_log_flush_lock);

	/*                                                                */
	if (gl && !test_bit(GLF_LFLUSH, &gl->gl_flags)) {
		up_write(&sdp->sd_log_flush_lock);
		return;
	}
	trace_gfs2_log_flush(sdp, 1);

	tr = sdp->sd_log_tr;
	if (tr) {
		sdp->sd_log_tr = NULL;
		INIT_LIST_HEAD(&tr->tr_ail1_list);
		INIT_LIST_HEAD(&tr->tr_ail2_list);
	}

	if (sdp->sd_log_num_buf != sdp->sd_log_commited_buf) {
		printk(KERN_INFO "GFS2: log buf %u %u\n", sdp->sd_log_num_buf,
		       sdp->sd_log_commited_buf);
		gfs2_assert_withdraw(sdp, 0);
	}
	if (sdp->sd_log_num_databuf != sdp->sd_log_commited_databuf) {
		printk(KERN_INFO "GFS2: log databuf %u %u\n",
		       sdp->sd_log_num_databuf, sdp->sd_log_commited_databuf);
		gfs2_assert_withdraw(sdp, 0);
	}
	gfs2_assert_withdraw(sdp,
			sdp->sd_log_num_revoke == sdp->sd_log_commited_revoke);

	sdp->sd_log_flush_head = sdp->sd_log_head;
	sdp->sd_log_flush_wrapped = 0;
	if (tr)
		tr->tr_first = sdp->sd_log_flush_head;

	gfs2_ordered_write(sdp);
	lops_before_commit(sdp);
	gfs2_log_flush_bio(sdp, WRITE);

	if (sdp->sd_log_head != sdp->sd_log_flush_head) {
		log_write_header(sdp, 0);
	} else if (sdp->sd_log_tail != current_tail(sdp) && !sdp->sd_log_idle){
		atomic_dec(&sdp->sd_log_blks_free); /*                              */
		trace_gfs2_log_blocks(sdp, -1);
		log_write_header(sdp, 0);
	}
	lops_after_commit(sdp, tr);

	gfs2_log_lock(sdp);
	sdp->sd_log_head = sdp->sd_log_flush_head;
	sdp->sd_log_blks_reserved = 0;
	sdp->sd_log_commited_buf = 0;
	sdp->sd_log_commited_databuf = 0;
	sdp->sd_log_commited_revoke = 0;

	spin_lock(&sdp->sd_ail_lock);
	if (tr && !list_empty(&tr->tr_ail1_list)) {
		list_add(&tr->tr_list, &sdp->sd_ail1_list);
		tr = NULL;
	}
	spin_unlock(&sdp->sd_ail_lock);
	gfs2_log_unlock(sdp);
	trace_gfs2_log_flush(sdp, 0);
	up_write(&sdp->sd_log_flush_lock);

	kfree(tr);
}

static void log_refund(struct gfs2_sbd *sdp, struct gfs2_trans *tr)
{
	unsigned int reserved;
	unsigned int unused;

	gfs2_log_lock(sdp);

	sdp->sd_log_commited_buf += tr->tr_num_buf_new - tr->tr_num_buf_rm;
	sdp->sd_log_commited_databuf += tr->tr_num_databuf_new -
		tr->tr_num_databuf_rm;
	gfs2_assert_withdraw(sdp, (((int)sdp->sd_log_commited_buf) >= 0) ||
			     (((int)sdp->sd_log_commited_databuf) >= 0));
	sdp->sd_log_commited_revoke += tr->tr_num_revoke - tr->tr_num_revoke_rm;
	reserved = calc_reserved(sdp);
	gfs2_assert_withdraw(sdp, sdp->sd_log_blks_reserved + tr->tr_reserved >= reserved);
	unused = sdp->sd_log_blks_reserved - reserved + tr->tr_reserved;
	atomic_add(unused, &sdp->sd_log_blks_free);
	trace_gfs2_log_blocks(sdp, unused);
	gfs2_assert_withdraw(sdp, atomic_read(&sdp->sd_log_blks_free) <=
			     sdp->sd_jdesc->jd_blocks);
	sdp->sd_log_blks_reserved = reserved;

	if (sdp->sd_log_tr == NULL &&
	    (tr->tr_num_buf_new || tr->tr_num_databuf_new)) {
		gfs2_assert_withdraw(sdp, tr->tr_t_gh.gh_gl);
		sdp->sd_log_tr = tr;
		tr->tr_attached = 1;
	}
	gfs2_log_unlock(sdp);
}

/* 
                                                    
                       
                       
  
                                                                     
                                                                     
                           
  
                                                                      
                
  
                 
 */

void gfs2_log_commit(struct gfs2_sbd *sdp, struct gfs2_trans *tr)
{
	log_refund(sdp, tr);

	if (atomic_read(&sdp->sd_log_pinned) > atomic_read(&sdp->sd_log_thresh1) ||
	    ((sdp->sd_jdesc->jd_blocks - atomic_read(&sdp->sd_log_blks_free)) >
	    atomic_read(&sdp->sd_log_thresh2)))
		wake_up(&sdp->sd_logd_waitq);
}

/* 
                                                             
                       
  
 */

void gfs2_log_shutdown(struct gfs2_sbd *sdp)
{
	down_write(&sdp->sd_log_flush_lock);

	gfs2_assert_withdraw(sdp, !sdp->sd_log_blks_reserved);
	gfs2_assert_withdraw(sdp, !sdp->sd_log_num_buf);
	gfs2_assert_withdraw(sdp, !sdp->sd_log_num_revoke);
	gfs2_assert_withdraw(sdp, !sdp->sd_log_num_rg);
	gfs2_assert_withdraw(sdp, !sdp->sd_log_num_databuf);
	gfs2_assert_withdraw(sdp, list_empty(&sdp->sd_ail1_list));

	sdp->sd_log_flush_head = sdp->sd_log_head;
	sdp->sd_log_flush_wrapped = 0;

	log_write_header(sdp, GFS2_LOG_HEAD_UNMOUNT);

	gfs2_assert_warn(sdp, atomic_read(&sdp->sd_log_blks_free) == sdp->sd_jdesc->jd_blocks);
	gfs2_assert_warn(sdp, sdp->sd_log_head == sdp->sd_log_tail);
	gfs2_assert_warn(sdp, list_empty(&sdp->sd_ail2_list));

	sdp->sd_log_head = sdp->sd_log_flush_head;
	sdp->sd_log_tail = sdp->sd_log_head;

	up_write(&sdp->sd_log_flush_lock);
}


/* 
                                                          
                       
  
 */

void gfs2_meta_syncfs(struct gfs2_sbd *sdp)
{
	gfs2_log_flush(sdp, NULL);
	for (;;) {
		gfs2_ail1_start(sdp);
		gfs2_ail1_wait(sdp);
		if (gfs2_ail1_empty(sdp))
			break;
	}
	gfs2_log_flush(sdp, NULL);
}

static inline int gfs2_jrnl_flush_reqd(struct gfs2_sbd *sdp)
{
	return (atomic_read(&sdp->sd_log_pinned) >= atomic_read(&sdp->sd_log_thresh1));
}

static inline int gfs2_ail_flush_reqd(struct gfs2_sbd *sdp)
{
	unsigned int used_blocks = sdp->sd_jdesc->jd_blocks - atomic_read(&sdp->sd_log_blks_free);
	return used_blocks >= atomic_read(&sdp->sd_log_thresh2);
}

/* 
                                                                             
                                   
  
                                                                         
                 
 */

int gfs2_logd(void *data)
{
	struct gfs2_sbd *sdp = data;
	unsigned long t = 1;
	DEFINE_WAIT(wait);

	set_freezable();

	while (!kthread_freezable_should_stop(NULL)) {

		if (gfs2_jrnl_flush_reqd(sdp) || t == 0) {
			gfs2_ail1_empty(sdp);
			gfs2_log_flush(sdp, NULL);
		}

		if (gfs2_ail_flush_reqd(sdp)) {
			gfs2_ail1_start(sdp);
			gfs2_ail1_wait(sdp);
			gfs2_ail1_empty(sdp);
			gfs2_log_flush(sdp, NULL);
		}

		if (!gfs2_ail_flush_reqd(sdp))
			wake_up(&sdp->sd_log_waitq);

		t = gfs2_tune_get(sdp, gt_logd_secs) * HZ;

		try_to_freeze();

		do {
			prepare_to_wait(&sdp->sd_logd_waitq, &wait,
					TASK_INTERRUPTIBLE);
			if (!gfs2_ail_flush_reqd(sdp) &&
			    !gfs2_jrnl_flush_reqd(sdp) &&
			    !kthread_freezable_should_stop(NULL))
				t = schedule_timeout(t);
		} while(t && !gfs2_ail_flush_reqd(sdp) &&
			!gfs2_jrnl_flush_reqd(sdp) &&
			!kthread_freezable_should_stop(NULL));
		finish_wait(&sdp->sd_logd_waitq, &wait);
	}

	return 0;
}

