/*
 * Copyright (C) Sistina Software, Inc.  1997-2003 All rights reserved.
 * Copyright (C) 2004-2006 Red Hat, Inc.  All rights reserved.
 *
 * This copyrighted material is made available to anyone wishing to use,
 * modify, copy, or redistribute it subject to the terms and conditions
 * of the GNU General Public License version 2.
 */

#include <linux/spinlock.h>
#include <linux/completion.h>
#include <linux/buffer_head.h>
#include <linux/blkdev.h>
#include <linux/gfs2_ondisk.h>
#include <linux/crc32.h>

#include "gfs2.h"
#include "incore.h"
#include "bmap.h"
#include "glock.h"
#include "inode.h"
#include "meta_io.h"
#include "quota.h"
#include "rgrp.h"
#include "log.h"
#include "super.h"
#include "trans.h"
#include "dir.h"
#include "util.h"
#include "trace_gfs2.h"

/*                                                                  
                                                                   
                 
 */
struct metapath {
	struct buffer_head *mp_bh[GFS2_MAX_META_HEIGHT];
	__u16 mp_list[GFS2_MAX_META_HEIGHT];
};

struct strip_mine {
	int sm_first;
	unsigned int sm_height;
};

/* 
                                                                              
                 
                           
                                              
                                                                 
  
                 
 */

static int gfs2_unstuffer_page(struct gfs2_inode *ip, struct buffer_head *dibh,
			       u64 block, struct page *page)
{
	struct inode *inode = &ip->i_inode;
	struct buffer_head *bh;
	int release = 0;

	if (!page || page->index) {
		page = find_or_create_page(inode->i_mapping, 0, GFP_NOFS);
		if (!page)
			return -ENOMEM;
		release = 1;
	}

	if (!PageUptodate(page)) {
		void *kaddr = kmap(page);
		u64 dsize = i_size_read(inode);
 
		if (dsize > (dibh->b_size - sizeof(struct gfs2_dinode)))
			dsize = dibh->b_size - sizeof(struct gfs2_dinode);

		memcpy(kaddr, dibh->b_data + sizeof(struct gfs2_dinode), dsize);
		memset(kaddr + dsize, 0, PAGE_CACHE_SIZE - dsize);
		kunmap(page);

		SetPageUptodate(page);
	}

	if (!page_has_buffers(page))
		create_empty_buffers(page, 1 << inode->i_blkbits,
				     (1 << BH_Uptodate));

	bh = page_buffers(page);

	if (!buffer_mapped(bh))
		map_bh(bh, inode->i_sb, block);

	set_buffer_uptodate(bh);
	if (!gfs2_is_jdata(ip))
		mark_buffer_dirty(bh);
	if (!gfs2_is_writeback(ip))
		gfs2_trans_add_data(ip->i_gl, bh);

	if (release) {
		unlock_page(page);
		page_cache_release(page);
	}

	return 0;
}

/* 
                                                                         
                                 
                                                                     
  
                                                                         
                                                       
  
                 
 */

int gfs2_unstuff_dinode(struct gfs2_inode *ip, struct page *page)
{
	struct buffer_head *bh, *dibh;
	struct gfs2_dinode *di;
	u64 block = 0;
	int isdir = gfs2_is_dir(ip);
	int error;

	down_write(&ip->i_rw_mutex);

	error = gfs2_meta_inode_buffer(ip, &dibh);
	if (error)
		goto out;

	if (i_size_read(&ip->i_inode)) {
		/*                                                 
                              */

		unsigned int n = 1;
		error = gfs2_alloc_blocks(ip, &block, &n, 0, NULL);
		if (error)
			goto out_brelse;
		if (isdir) {
			gfs2_trans_add_unrevoke(GFS2_SB(&ip->i_inode), block, 1);
			error = gfs2_dir_get_new_buffer(ip, block, &bh);
			if (error)
				goto out_brelse;
			gfs2_buffer_copy_tail(bh, sizeof(struct gfs2_meta_header),
					      dibh, sizeof(struct gfs2_dinode));
			brelse(bh);
		} else {
			error = gfs2_unstuffer_page(ip, dibh, block, page);
			if (error)
				goto out_brelse;
		}
	}

	/*                                       */

	gfs2_trans_add_meta(ip->i_gl, dibh);
	di = (struct gfs2_dinode *)dibh->b_data;
	gfs2_buffer_clear_tail(dibh, sizeof(struct gfs2_dinode));

	if (i_size_read(&ip->i_inode)) {
		*(__be64 *)(di + 1) = cpu_to_be64(block);
		gfs2_add_inode_blocks(&ip->i_inode, 1);
		di->di_blocks = cpu_to_be64(gfs2_get_inode_blocks(&ip->i_inode));
	}

	ip->i_height = 1;
	di->di_height = cpu_to_be16(1);

out_brelse:
	brelse(dibh);
out:
	up_write(&ip->i_rw_mutex);
	return error;
}


/* 
                                                      
                       
                                            
                                    
                                                          
  
                                                                         
                                                                
  
             
                                                                          
                                         
  
                                                                     
                                                                           
                          
  
                                                                        
                                                                             
                                                                          
                                                                          
                                                      
  
                                                       
                                                       
                                                       
                                                       
                                                       
                                                       
                          
                          
                          
                                                       
                                                       
                                                       
                                                       
                                                       
                                                       
                                   
                                   
                                   
                                                       
                                                       
                                                       
                                                       
                                                       
                                                       
                                              
                                              
                                              
                                                       
                                                       
                                                       
                                                       
                                                       
                                                       
  
 */

static void find_metapath(const struct gfs2_sbd *sdp, u64 block,
			  struct metapath *mp, unsigned int height)
{
	unsigned int i;

	for (i = height; i--;)
		mp->mp_list[i] = do_div(block, sdp->sd_inptrs);

}

static inline unsigned int metapath_branch_start(const struct metapath *mp)
{
	if (mp->mp_list[0] == 0)
		return 2;
	return 1;
}

/* 
                                                                
                                            
                    
  
                                                                          
                                                                          
                 
 */

static inline __be64 *metapointer(unsigned int height, const struct metapath *mp)
{
	struct buffer_head *bh = mp->mp_bh[height];
	unsigned int head_size = (height > 0) ?
		sizeof(struct gfs2_meta_header) : sizeof(struct gfs2_dinode);
	return ((__be64 *)(bh->b_data + head_size)) + mp->mp_list[height];
}

static void gfs2_metapath_ra(struct gfs2_glock *gl,
			     const struct buffer_head *bh, const __be64 *pos)
{
	struct buffer_head *rabh;
	const __be64 *endp = (const __be64 *)(bh->b_data + bh->b_size);
	const __be64 *t;

	for (t = pos; t < endp; t++) {
		if (!*t)
			continue;

		rabh = gfs2_getbuf(gl, be64_to_cpu(*t), CREATE);
		if (trylock_buffer(rabh)) {
			if (!buffer_uptodate(rabh)) {
				rabh->b_end_io = end_buffer_read_sync;
				submit_bh(READA | REQ_META, rabh);
				continue;
			}
			unlock_buffer(rabh);
		}
		brelse(rabh);
	}
}

/* 
                                                               
                 
                    
  
                                                                 
                                                                      
                      
  
                                                                  
                                                                    
                                                                      
                                 
  
                                            
 */

static int lookup_metapath(struct gfs2_inode *ip, struct metapath *mp)
{
	unsigned int end_of_metadata = ip->i_height - 1;
	unsigned int x;
	__be64 *ptr;
	u64 dblock;
	int ret;

	for (x = 0; x < end_of_metadata; x++) {
		ptr = metapointer(x, mp);
		dblock = be64_to_cpu(*ptr);
		if (!dblock)
			return x + 1;

		ret = gfs2_meta_indirect_buffer(ip, x+1, dblock, &mp->mp_bh[x+1]);
		if (ret)
			return ret;
	}

	return ip->i_height;
}

static inline void release_metapath(struct metapath *mp)
{
	int i;

	for (i = 0; i < GFS2_MAX_META_HEIGHT; i++) {
		if (mp->mp_bh[i] == NULL)
			break;
		brelse(mp->mp_bh[i]);
	}
}

/* 
                                                             
                              
                                      
                                       
                                                      
                                          
  
                                                                        
                                                                        
                                      
  
                                                           
 */

static inline unsigned int gfs2_extent_length(void *start, unsigned int len, __be64 *ptr, unsigned limit, int *eob)
{
	const __be64 *end = (start + len);
	const __be64 *first = ptr;
	u64 d = be64_to_cpu(*ptr);

	*eob = 0;
	do {
		ptr++;
		if (ptr >= end)
			break;
		if (limit && --limit == 0)
			break;
		if (d)
			d++;
	} while(be64_to_cpu(*ptr) == d);
	if (ptr >= end)
		*eob = 1;
	return (ptr - first);
}

static inline void bmap_lock(struct gfs2_inode *ip, int create)
{
	if (create)
		down_write(&ip->i_rw_mutex);
	else
		down_read(&ip->i_rw_mutex);
}

static inline void bmap_unlock(struct gfs2_inode *ip, int create)
{
	if (create)
		up_write(&ip->i_rw_mutex);
	else
		up_read(&ip->i_rw_mutex);
}

static inline __be64 *gfs2_indirect_init(struct metapath *mp,
					 struct gfs2_glock *gl, unsigned int i,
					 unsigned offset, u64 bn)
{
	__be64 *ptr = (__be64 *)(mp->mp_bh[i - 1]->b_data +
		       ((i > 1) ? sizeof(struct gfs2_meta_header) :
				 sizeof(struct gfs2_dinode)));
	BUG_ON(i < 1);
	BUG_ON(mp->mp_bh[i] != NULL);
	mp->mp_bh[i] = gfs2_meta_new(gl, bn);
	gfs2_trans_add_meta(gl, mp->mp_bh[i]);
	gfs2_metatype_set(mp->mp_bh[i], GFS2_METATYPE_IN, GFS2_FORMAT_IN);
	gfs2_buffer_clear_tail(mp->mp_bh[i], sizeof(struct gfs2_meta_header));
	ptr += offset;
	*ptr = cpu_to_be64(bn);
	return ptr;
}

enum alloc_state {
	ALLOC_DATA = 0,
	ALLOC_GROW_DEPTH = 1,
	ALLOC_GROW_HEIGHT = 2,
	/*                                                 */
};

/* 
                                                                  
                         
                                                    
                                                      
                    
                                                            
                                  
                                                  
  
                                        
                                                        
                                                                  
                   
  
                                                                   
                                                                  
                                                                       
                                                                      
                                                                
  
                          
 */

static int gfs2_bmap_alloc(struct inode *inode, const sector_t lblock,
			   struct buffer_head *bh_map, struct metapath *mp,
			   const unsigned int sheight,
			   const unsigned int height,
			   const unsigned int maxlen)
{
	struct gfs2_inode *ip = GFS2_I(inode);
	struct gfs2_sbd *sdp = GFS2_SB(inode);
	struct super_block *sb = sdp->sd_vfs;
	struct buffer_head *dibh = mp->mp_bh[0];
	u64 bn, dblock = 0;
	unsigned n, i, blks, alloced = 0, iblks = 0, branch_start = 0;
	unsigned dblks = 0;
	unsigned ptrs_per_blk;
	const unsigned end_of_metadata = height - 1;
	int ret;
	int eob = 0;
	enum alloc_state state;
	__be64 *ptr;
	__be64 zero_bn = 0;

	BUG_ON(sheight < 1);
	BUG_ON(dibh == NULL);

	gfs2_trans_add_meta(ip->i_gl, dibh);

	if (height == sheight) {
		struct buffer_head *bh;
		/*                                                          */
		ptr = metapointer(end_of_metadata, mp);
		bh = mp->mp_bh[end_of_metadata];
		dblks = gfs2_extent_length(bh->b_data, bh->b_size, ptr, maxlen,
					   &eob);
		BUG_ON(dblks < 1);
		state = ALLOC_DATA;
	} else {
		/*                                  */
		ptrs_per_blk = height > 1 ? sdp->sd_inptrs : sdp->sd_diptrs;
		dblks = min(maxlen, ptrs_per_blk - mp->mp_list[end_of_metadata]);
		if (height == ip->i_height) {
			/*                                              */
			iblks = height - sheight;
			state = ALLOC_GROW_DEPTH;
		} else {
			/*                         */
			state = ALLOC_GROW_HEIGHT;
			iblks = height - ip->i_height;
			branch_start = metapath_branch_start(mp);
			iblks += (height - branch_start);
		}
	}

	/*                                                          */

	blks = dblks + iblks;
	i = sheight;
	do {
		int error;
		n = blks - alloced;
		error = gfs2_alloc_blocks(ip, &bn, &n, 0, NULL);
		if (error)
			return error;
		alloced += n;
		if (state != ALLOC_DATA || gfs2_is_jdata(ip))
			gfs2_trans_add_unrevoke(sdp, bn, n);
		switch (state) {
		/*                        */
		case ALLOC_GROW_HEIGHT:
			if (i == 1) {
				ptr = (__be64 *)(dibh->b_data +
						 sizeof(struct gfs2_dinode));
				zero_bn = *ptr;
			}
			for (; i - 1 < height - ip->i_height && n > 0; i++, n--)
				gfs2_indirect_init(mp, ip->i_gl, i, 0, bn++);
			if (i - 1 == height - ip->i_height) {
				i--;
				gfs2_buffer_copy_tail(mp->mp_bh[i],
						sizeof(struct gfs2_meta_header),
						dibh, sizeof(struct gfs2_dinode));
				gfs2_buffer_clear_tail(dibh,
						sizeof(struct gfs2_dinode) +
						sizeof(__be64));
				ptr = (__be64 *)(mp->mp_bh[i]->b_data +
					sizeof(struct gfs2_meta_header));
				*ptr = zero_bn;
				state = ALLOC_GROW_DEPTH;
				for(i = branch_start; i < height; i++) {
					if (mp->mp_bh[i] == NULL)
						break;
					brelse(mp->mp_bh[i]);
					mp->mp_bh[i] = NULL;
				}
				i = branch_start;
			}
			if (n == 0)
				break;
		/*                              */
		case ALLOC_GROW_DEPTH:
			if (i > 1 && i < height)
				gfs2_trans_add_meta(ip->i_gl, mp->mp_bh[i-1]);
			for (; i < height && n > 0; i++, n--)
				gfs2_indirect_init(mp, ip->i_gl, i,
						   mp->mp_list[i-1], bn++);
			if (i == height)
				state = ALLOC_DATA;
			if (n == 0)
				break;
		/*                                   */
		case ALLOC_DATA:
			BUG_ON(n > dblks);
			BUG_ON(mp->mp_bh[end_of_metadata] == NULL);
			gfs2_trans_add_meta(ip->i_gl, mp->mp_bh[end_of_metadata]);
			dblks = n;
			ptr = metapointer(end_of_metadata, mp);
			dblock = bn;
			while (n-- > 0)
				*ptr++ = cpu_to_be64(bn++);
			if (buffer_zeronew(bh_map)) {
				ret = sb_issue_zeroout(sb, dblock, dblks,
						       GFP_NOFS);
				if (ret) {
					fs_err(sdp,
					       "Failed to zero data buffers\n");
					clear_buffer_zeronew(bh_map);
				}
			}
			break;
		}
	} while ((state != ALLOC_DATA) || !dblock);

	ip->i_height = height;
	gfs2_add_inode_blocks(&ip->i_inode, alloced);
	gfs2_dinode_out(ip, mp->mp_bh[0]->b_data);
	map_bh(bh_map, inode->i_sb, dblock);
	bh_map->b_size = dblks << inode->i_blkbits;
	set_buffer_new(bh_map);
	return 0;
}

/* 
                                                             
                    
                                    
                               
                                                                
  
                                                                  
                                                                 
                                                          
  
                 
 */

int gfs2_block_map(struct inode *inode, sector_t lblock,
		   struct buffer_head *bh_map, int create)
{
	struct gfs2_inode *ip = GFS2_I(inode);
	struct gfs2_sbd *sdp = GFS2_SB(inode);
	unsigned int bsize = sdp->sd_sb.sb_bsize;
	const unsigned int maxlen = bh_map->b_size >> inode->i_blkbits;
	const u64 *arr = sdp->sd_heightsize;
	__be64 *ptr;
	u64 size;
	struct metapath mp;
	int ret;
	int eob;
	unsigned int len;
	struct buffer_head *bh;
	u8 height;

	BUG_ON(maxlen == 0);

	memset(mp.mp_bh, 0, sizeof(mp.mp_bh));
	bmap_lock(ip, create);
	clear_buffer_mapped(bh_map);
	clear_buffer_new(bh_map);
	clear_buffer_boundary(bh_map);
	trace_gfs2_bmap(ip, bh_map, lblock, create, 1);
	if (gfs2_is_dir(ip)) {
		bsize = sdp->sd_jbsize;
		arr = sdp->sd_jheightsize;
	}

	ret = gfs2_meta_inode_buffer(ip, &mp.mp_bh[0]);
	if (ret)
		goto out;

	height = ip->i_height;
	size = (lblock + 1) * bsize;
	while (size > arr[height])
		height++;
	find_metapath(sdp, lblock, &mp, height);
	ret = 1;
	if (height > ip->i_height || gfs2_is_stuffed(ip))
		goto do_alloc;
	ret = lookup_metapath(ip, &mp);
	if (ret < 0)
		goto out;
	if (ret != ip->i_height)
		goto do_alloc;
	ptr = metapointer(ip->i_height - 1, &mp);
	if (*ptr == 0)
		goto do_alloc;
	map_bh(bh_map, inode->i_sb, be64_to_cpu(*ptr));
	bh = mp.mp_bh[ip->i_height - 1];
	len = gfs2_extent_length(bh->b_data, bh->b_size, ptr, maxlen, &eob);
	bh_map->b_size = (len << inode->i_blkbits);
	if (eob)
		set_buffer_boundary(bh_map);
	ret = 0;
out:
	release_metapath(&mp);
	trace_gfs2_bmap(ip, bh_map, lblock, create, ret);
	bmap_unlock(ip, create);
	return ret;

do_alloc:
	/*                                                          */
	if (!create) {
		BUG_ON(gfs2_is_stuffed(ip));
		ret = 0;
		goto out;
	}

	/*                                                                 */
	ret = gfs2_bmap_alloc(inode, lblock, bh_map, &mp, ret, height, maxlen);
	goto out;
}

/*
                                     
 */
int gfs2_extent_map(struct inode *inode, u64 lblock, int *new, u64 *dblock, unsigned *extlen)
{
	struct buffer_head bh = { .b_state = 0, .b_blocknr = 0 };
	int ret;
	int create = *new;

	BUG_ON(!extlen);
	BUG_ON(!dblock);
	BUG_ON(!new);

	bh.b_size = 1 << (inode->i_blkbits + (create ? 0 : 5));
	ret = gfs2_block_map(inode, lblock, &bh, create);
	*extlen = bh.b_size >> inode->i_blkbits;
	*dblock = bh.b_blocknr;
	if (buffer_new(&bh))
		*new = 1;
	else
		*new = 0;
	return ret;
}

/* 
                                                                              
                 
                           
                            
                                        
                                          
                                        
                                          
  
                 
 */

static int do_strip(struct gfs2_inode *ip, struct buffer_head *dibh,
		    struct buffer_head *bh, __be64 *top, __be64 *bottom,
		    unsigned int height, struct strip_mine *sm)
{
	struct gfs2_sbd *sdp = GFS2_SB(&ip->i_inode);
	struct gfs2_rgrp_list rlist;
	u64 bn, bstart;
	u32 blen, btotal;
	__be64 *p;
	unsigned int rg_blocks = 0;
	int metadata;
	unsigned int revokes = 0;
	int x;
	int error;

	error = gfs2_rindex_update(sdp);
	if (error)
		return error;

	if (!*top)
		sm->sm_first = 0;

	if (height != sm->sm_height)
		return 0;

	if (sm->sm_first) {
		top++;
		sm->sm_first = 0;
	}

	metadata = (height != ip->i_height - 1);
	if (metadata)
		revokes = (height) ? sdp->sd_inptrs : sdp->sd_diptrs;
	else if (ip->i_depth)
		revokes = sdp->sd_inptrs;

	memset(&rlist, 0, sizeof(struct gfs2_rgrp_list));
	bstart = 0;
	blen = 0;

	for (p = top; p < bottom; p++) {
		if (!*p)
			continue;

		bn = be64_to_cpu(*p);

		if (bstart + blen == bn)
			blen++;
		else {
			if (bstart)
				gfs2_rlist_add(ip, &rlist, bstart);

			bstart = bn;
			blen = 1;
		}
	}

	if (bstart)
		gfs2_rlist_add(ip, &rlist, bstart);
	else
		goto out; /*               */

	gfs2_rlist_alloc(&rlist, LM_ST_EXCLUSIVE);

	for (x = 0; x < rlist.rl_rgrps; x++) {
		struct gfs2_rgrpd *rgd;
		rgd = rlist.rl_ghs[x].gh_gl->gl_object;
		rg_blocks += rgd->rd_length;
	}

	error = gfs2_glock_nq_m(rlist.rl_rgrps, rlist.rl_ghs);
	if (error)
		goto out_rlist;

	if (gfs2_rs_active(ip->i_res)) /*                                           */
		gfs2_rs_deltree(ip->i_res);

	error = gfs2_trans_begin(sdp, rg_blocks + RES_DINODE +
				 RES_INDIRECT + RES_STATFS + RES_QUOTA,
				 revokes);
	if (error)
		goto out_rg_gunlock;

	down_write(&ip->i_rw_mutex);

	gfs2_trans_add_meta(ip->i_gl, dibh);
	gfs2_trans_add_meta(ip->i_gl, bh);

	bstart = 0;
	blen = 0;
	btotal = 0;

	for (p = top; p < bottom; p++) {
		if (!*p)
			continue;

		bn = be64_to_cpu(*p);

		if (bstart + blen == bn)
			blen++;
		else {
			if (bstart) {
				__gfs2_free_blocks(ip, bstart, blen, metadata);
				btotal += blen;
			}

			bstart = bn;
			blen = 1;
		}

		*p = 0;
		gfs2_add_inode_blocks(&ip->i_inode, -1);
	}
	if (bstart) {
		__gfs2_free_blocks(ip, bstart, blen, metadata);
		btotal += blen;
	}

	gfs2_statfs_change(sdp, 0, +btotal, 0);
	gfs2_quota_change(ip, -(s64)btotal, ip->i_inode.i_uid,
			  ip->i_inode.i_gid);

	ip->i_inode.i_mtime = ip->i_inode.i_ctime = CURRENT_TIME;

	gfs2_dinode_out(ip, dibh->b_data);

	up_write(&ip->i_rw_mutex);

	gfs2_trans_end(sdp);

out_rg_gunlock:
	gfs2_glock_dq_m(rlist.rl_rgrps, rlist.rl_ghs);
out_rlist:
	gfs2_rlist_free(&rlist);
out:
	return error;
}

/* 
                                                              
                 
                           
                                                           
                                          
                                        
                                       
                                                   
  
                                                                  
                      
  
                 
 */

static int recursive_scan(struct gfs2_inode *ip, struct buffer_head *dibh,
			  struct metapath *mp, unsigned int height,
			  u64 block, int first, struct strip_mine *sm)
{
	struct gfs2_sbd *sdp = GFS2_SB(&ip->i_inode);
	struct buffer_head *bh = NULL;
	__be64 *top, *bottom;
	u64 bn;
	int error;
	int mh_size = sizeof(struct gfs2_meta_header);

	if (!height) {
		error = gfs2_meta_inode_buffer(ip, &bh);
		if (error)
			return error;
		dibh = bh;

		top = (__be64 *)(bh->b_data + sizeof(struct gfs2_dinode)) + mp->mp_list[0];
		bottom = (__be64 *)(bh->b_data + sizeof(struct gfs2_dinode)) + sdp->sd_diptrs;
	} else {
		error = gfs2_meta_indirect_buffer(ip, height, block, &bh);
		if (error)
			return error;

		top = (__be64 *)(bh->b_data + mh_size) +
				  (first ? mp->mp_list[height] : 0);

		bottom = (__be64 *)(bh->b_data + mh_size) + sdp->sd_inptrs;
	}

	error = do_strip(ip, dibh, bh, top, bottom, height, sm);
	if (error)
		goto out;

	if (height < ip->i_height - 1) {

		gfs2_metapath_ra(ip->i_gl, bh, top);

		for (; top < bottom; top++, first = 0) {
			if (!*top)
				continue;

			bn = be64_to_cpu(*top);

			error = recursive_scan(ip, dibh, mp, height + 1, bn,
					       first, sm);
			if (error)
				break;
		}
	}
out:
	brelse(bh);
	return error;
}


/* 
                                                                     
  
                                     
 */
static int gfs2_block_truncate_page(struct address_space *mapping, loff_t from)
{
	struct inode *inode = mapping->host;
	struct gfs2_inode *ip = GFS2_I(inode);
	unsigned long index = from >> PAGE_CACHE_SHIFT;
	unsigned offset = from & (PAGE_CACHE_SIZE-1);
	unsigned blocksize, iblock, length, pos;
	struct buffer_head *bh;
	struct page *page;
	int err;

	page = find_or_create_page(mapping, index, GFP_NOFS);
	if (!page)
		return 0;

	blocksize = inode->i_sb->s_blocksize;
	length = blocksize - (offset & (blocksize - 1));
	iblock = index << (PAGE_CACHE_SHIFT - inode->i_sb->s_blocksize_bits);

	if (!page_has_buffers(page))
		create_empty_buffers(page, blocksize, 0);

	/*                                        */
	bh = page_buffers(page);
	pos = blocksize;
	while (offset >= pos) {
		bh = bh->b_this_page;
		iblock++;
		pos += blocksize;
	}

	err = 0;

	if (!buffer_mapped(bh)) {
		gfs2_block_map(inode, iblock, bh, 0);
		/*                                       */
		if (!buffer_mapped(bh))
			goto unlock;
	}

	/*                                            */
	if (PageUptodate(page))
		set_buffer_uptodate(bh);

	if (!buffer_uptodate(bh)) {
		err = -EIO;
		ll_rw_block(READ, 1, &bh);
		wait_on_buffer(bh);
		/*                                       */
		if (!buffer_uptodate(bh))
			goto unlock;
		err = 0;
	}

	if (!gfs2_is_writeback(ip))
		gfs2_trans_add_data(ip->i_gl, bh);

	zero_user(page, offset, length);
	mark_buffer_dirty(bh);
unlock:
	unlock_page(page);
	page_cache_release(page);
	return err;
}

/* 
                                                                           
                                    
                                       
                                 
  
                                                                        
                                                                           
                                                         
 */

#define GFS2_JTRUNC_REVOKES 8192

static int gfs2_journaled_truncate(struct inode *inode, u64 oldsize, u64 newsize)
{
	struct gfs2_sbd *sdp = GFS2_SB(inode);
	u64 max_chunk = GFS2_JTRUNC_REVOKES * sdp->sd_vfs->s_blocksize;
	u64 chunk;
	int error;

	while (oldsize != newsize) {
		chunk = oldsize - newsize;
		if (chunk > max_chunk)
			chunk = max_chunk;
		truncate_pagecache(inode, oldsize, oldsize - chunk);
		oldsize -= chunk;
		gfs2_trans_end(sdp);
		error = gfs2_trans_begin(sdp, RES_DINODE, GFS2_JTRUNC_REVOKES);
		if (error)
			return error;
	}

	return 0;
}

static int trunc_start(struct inode *inode, u64 oldsize, u64 newsize)
{
	struct gfs2_inode *ip = GFS2_I(inode);
	struct gfs2_sbd *sdp = GFS2_SB(inode);
	struct address_space *mapping = inode->i_mapping;
	struct buffer_head *dibh;
	int journaled = gfs2_is_jdata(ip);
	int error;

	if (journaled)
		error = gfs2_trans_begin(sdp, RES_DINODE + RES_JDATA, GFS2_JTRUNC_REVOKES);
	else
		error = gfs2_trans_begin(sdp, RES_DINODE, 0);
	if (error)
		return error;

	error = gfs2_meta_inode_buffer(ip, &dibh);
	if (error)
		goto out;

	gfs2_trans_add_meta(ip->i_gl, dibh);

	if (gfs2_is_stuffed(ip)) {
		gfs2_buffer_clear_tail(dibh, sizeof(struct gfs2_dinode) + newsize);
	} else {
		if (newsize & (u64)(sdp->sd_sb.sb_bsize - 1)) {
			error = gfs2_block_truncate_page(mapping, newsize);
			if (error)
				goto out_brelse;
		}
		ip->i_diskflags |= GFS2_DIF_TRUNC_IN_PROG;
	}

	i_size_write(inode, newsize);
	ip->i_inode.i_mtime = ip->i_inode.i_ctime = CURRENT_TIME;
	gfs2_dinode_out(ip, dibh->b_data);

	if (journaled)
		error = gfs2_journaled_truncate(inode, oldsize, newsize);
	else
		truncate_pagecache(inode, oldsize, newsize);

	if (error) {
		brelse(dibh);
		return error;
	}

out_brelse:
	brelse(dibh);
out:
	gfs2_trans_end(sdp);
	return error;
}

static int trunc_dealloc(struct gfs2_inode *ip, u64 size)
{
	struct gfs2_sbd *sdp = GFS2_SB(&ip->i_inode);
	unsigned int height = ip->i_height;
	u64 lblock;
	struct metapath mp;
	int error;

	if (!size)
		lblock = 0;
	else
		lblock = (size - 1) >> sdp->sd_sb.sb_bsize_shift;

	find_metapath(sdp, lblock, &mp, ip->i_height);
	error = gfs2_rindex_update(sdp);
	if (error)
		return error;

	error = gfs2_quota_hold(ip, NO_UID_QUOTA_CHANGE, NO_GID_QUOTA_CHANGE);
	if (error)
		return error;

	while (height--) {
		struct strip_mine sm;
		sm.sm_first = !!size;
		sm.sm_height = height;

		error = recursive_scan(ip, NULL, &mp, 0, 0, 1, &sm);
		if (error)
			break;
	}

	gfs2_quota_unhold(ip);

	return error;
}

static int trunc_end(struct gfs2_inode *ip)
{
	struct gfs2_sbd *sdp = GFS2_SB(&ip->i_inode);
	struct buffer_head *dibh;
	int error;

	error = gfs2_trans_begin(sdp, RES_DINODE, 0);
	if (error)
		return error;

	down_write(&ip->i_rw_mutex);

	error = gfs2_meta_inode_buffer(ip, &dibh);
	if (error)
		goto out;

	if (!i_size_read(&ip->i_inode)) {
		ip->i_height = 0;
		ip->i_goal = ip->i_no_addr;
		gfs2_buffer_clear_tail(dibh, sizeof(struct gfs2_dinode));
		gfs2_ordered_del_inode(ip);
	}
	ip->i_inode.i_mtime = ip->i_inode.i_ctime = CURRENT_TIME;
	ip->i_diskflags &= ~GFS2_DIF_TRUNC_IN_PROG;

	gfs2_trans_add_meta(ip->i_gl, dibh);
	gfs2_dinode_out(ip, dibh->b_data);
	brelse(dibh);

out:
	up_write(&ip->i_rw_mutex);
	gfs2_trans_end(sdp);
	return error;
}

/* 
                                  
                    
                                   
                                      
  
                                                          
                                                      
  
                 
 */

static int do_shrink(struct inode *inode, u64 oldsize, u64 newsize)
{
	struct gfs2_inode *ip = GFS2_I(inode);
	int error;

	error = trunc_start(inode, oldsize, newsize);
	if (error < 0)
		return error;
	if (gfs2_is_stuffed(ip))
		return 0;

	error = trunc_dealloc(ip, newsize);
	if (error == 0)
		error = trunc_end(ip);

	return error;
}

void gfs2_trim_blocks(struct inode *inode)
{
	u64 size = inode->i_size;
	int ret;

	ret = do_shrink(inode, size, size);
	WARN_ON(ret != 0);
}

/* 
                                        
                    
                      
  
                                                        
                                                         
                                                             
              
  
                                                              
                                                                  
                                                                   
                                                                
                                                                 
                                       
  
                                         
 */

static int do_grow(struct inode *inode, u64 size)
{
	struct gfs2_inode *ip = GFS2_I(inode);
	struct gfs2_sbd *sdp = GFS2_SB(inode);
	struct buffer_head *dibh;
	int error;
	int unstuff = 0;

	if (gfs2_is_stuffed(ip) &&
	    (size > (sdp->sd_sb.sb_bsize - sizeof(struct gfs2_dinode)))) {
		error = gfs2_quota_lock_check(ip);
		if (error)
			return error;

		error = gfs2_inplace_reserve(ip, 1, 0);
		if (error)
			goto do_grow_qunlock;
		unstuff = 1;
	}

	error = gfs2_trans_begin(sdp, RES_DINODE + RES_STATFS + RES_RG_BIT, 0);
	if (error)
		goto do_grow_release;

	if (unstuff) {
		error = gfs2_unstuff_dinode(ip, NULL);
		if (error)
			goto do_end_trans;
	}

	error = gfs2_meta_inode_buffer(ip, &dibh);
	if (error)
		goto do_end_trans;

	i_size_write(inode, size);
	ip->i_inode.i_mtime = ip->i_inode.i_ctime = CURRENT_TIME;
	gfs2_trans_add_meta(ip->i_gl, dibh);
	gfs2_dinode_out(ip, dibh->b_data);
	brelse(dibh);

do_end_trans:
	gfs2_trans_end(sdp);
do_grow_release:
	if (unstuff) {
		gfs2_inplace_release(ip);
do_grow_qunlock:
		gfs2_quota_unlock(ip);
	}
	return error;
}

/* 
                                               
                    
                                      
  
                                                              
                                                                
               
  
                 
 */

int gfs2_setattr_size(struct inode *inode, u64 newsize)
{
	int ret;
	u64 oldsize;

	BUG_ON(!S_ISREG(inode->i_mode));

	ret = inode_newsize_ok(inode, newsize);
	if (ret)
		return ret;

	ret = get_write_access(inode);
	if (ret)
		return ret;

	inode_dio_wait(inode);

	ret = gfs2_rs_alloc(GFS2_I(inode));
	if (ret)
		goto out;

	oldsize = inode->i_size;
	if (newsize >= oldsize) {
		ret = do_grow(inode, newsize);
		goto out;
	}

	ret = do_shrink(inode, oldsize, newsize);
out:
	put_write_access(inode);
	return ret;
}

int gfs2_truncatei_resume(struct gfs2_inode *ip)
{
	int error;
	error = trunc_dealloc(ip, i_size_read(&ip->i_inode));
	if (!error)
		error = trunc_end(ip);
	return error;
}

int gfs2_file_dealloc(struct gfs2_inode *ip)
{
	return trunc_dealloc(ip, 0);
}

/* 
                                                                               
                                 
                                  
                                          
  
                                                  
 */

int gfs2_write_alloc_required(struct gfs2_inode *ip, u64 offset,
			      unsigned int len)
{
	struct gfs2_sbd *sdp = GFS2_SB(&ip->i_inode);
	struct buffer_head bh;
	unsigned int shift;
	u64 lblock, lblock_stop, size;
	u64 end_of_file;

	if (!len)
		return 0;

	if (gfs2_is_stuffed(ip)) {
		if (offset + len >
		    sdp->sd_sb.sb_bsize - sizeof(struct gfs2_dinode))
			return 1;
		return 0;
	}

	shift = sdp->sd_sb.sb_bsize_shift;
	BUG_ON(gfs2_is_dir(ip));
	end_of_file = (i_size_read(&ip->i_inode) + sdp->sd_sb.sb_bsize - 1) >> shift;
	lblock = offset >> shift;
	lblock_stop = (offset + len + sdp->sd_sb.sb_bsize - 1) >> shift;
	if (lblock_stop > end_of_file)
		return 1;

	size = (lblock_stop - lblock) << shift;
	do {
		bh.b_state = 0;
		bh.b_size = size;
		gfs2_block_map(&ip->i_inode, lblock, &bh, 0);
		if (!buffer_mapped(&bh))
			return 1;
		size -= bh.b_size;
		lblock += (bh.b_size >> ip->i_inode.i_blkbits);
	} while(size > 0);

	return 0;
}

