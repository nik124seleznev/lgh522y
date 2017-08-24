/*
 * fs/f2fs/f2fs.h
 *
 * Copyright (c) 2012 Samsung Electronics Co., Ltd.
 *             http://www.samsung.com/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef _LINUX_F2FS_H
#define _LINUX_F2FS_H

#include <linux/types.h>
#include <linux/page-flags.h>
#include <linux/buffer_head.h>
#include <linux/slab.h>
#include <linux/crc32.h>
#include <linux/magic.h>

/*
                    
 */
#define F2FS_MOUNT_BG_GC		0x00000001
#define F2FS_MOUNT_DISABLE_ROLL_FORWARD	0x00000002
#define F2FS_MOUNT_DISCARD		0x00000004
#define F2FS_MOUNT_NOHEAP		0x00000008
#define F2FS_MOUNT_XATTR_USER		0x00000010
#define F2FS_MOUNT_POSIX_ACL		0x00000020
#define F2FS_MOUNT_DISABLE_EXT_IDENTIFY	0x00000040

#define clear_opt(sbi, option)	(sbi->mount_opt.opt &= ~F2FS_MOUNT_##option)
#define set_opt(sbi, option)	(sbi->mount_opt.opt |= F2FS_MOUNT_##option)
#define test_opt(sbi, option)	(sbi->mount_opt.opt & F2FS_MOUNT_##option)

#define ver_after(a, b)	(typecheck(unsigned long long, a) &&		\
		typecheck(unsigned long long, b) &&			\
		((long long)((a) - (b)) > 0))

typedef u64 block_t;
typedef u32 nid_t;

struct f2fs_mount_info {
	unsigned int	opt;
};

static inline __u32 f2fs_crc32(void *buff, size_t len)
{
	return crc32_le(F2FS_SUPER_MAGIC, buff, len);
}

static inline bool f2fs_crc_valid(__u32 blk_crc, void *buff, size_t buff_size)
{
	return f2fs_crc32(buff, buff_size) == blk_crc;
}

/*
                         
 */
enum {
	NAT_BITMAP,
	SIT_BITMAP
};

/*                               */
struct orphan_inode_entry {
	struct list_head list;	/*           */
	nid_t ino;		/*              */
};

/*                                  */
struct dir_inode_entry {
	struct list_head list;	/*           */
	struct inode *inode;	/*                   */
};

/*                                                         */
struct fsync_inode_entry {
	struct list_head list;	/*           */
	struct inode *inode;	/*                   */
	block_t blkaddr;	/*                                       */
};

#define nats_in_cursum(sum)		(le16_to_cpu(sum->n_nats))
#define sits_in_cursum(sum)		(le16_to_cpu(sum->n_sits))

#define nat_in_journal(sum, i)		(sum->nat_j.entries[i].ne)
#define nid_in_journal(sum, i)		(sum->nat_j.entries[i].nid)
#define sit_in_journal(sum, i)		(sum->sit_j.entries[i].se)
#define segno_in_journal(sum, i)	(sum->sit_j.entries[i].segno)

static inline int update_nats_in_cursum(struct f2fs_summary_block *rs, int i)
{
	int before = nats_in_cursum(rs);
	rs->n_nats = cpu_to_le16(before + i);
	return before;
}

static inline int update_sits_in_cursum(struct f2fs_summary_block *rs, int i)
{
	int before = sits_in_cursum(rs);
	rs->n_sits = cpu_to_le16(before + i);
	return before;
}

/*
                 
 */
#define F2FS_IOC_GETFLAGS               FS_IOC_GETFLAGS
#define F2FS_IOC_SETFLAGS               FS_IOC_SETFLAGS

#if defined(__KERNEL__) && defined(CONFIG_COMPAT)
/*
                                     
 */
#define F2FS_IOC32_GETFLAGS             FS_IOC32_GETFLAGS
#define F2FS_IOC32_SETFLAGS             FS_IOC32_SETFLAGS
#endif

/*
                             
 */
#define XATTR_NODE_OFFSET	(-1)	/*
                                          
                                             
                                           
      */
enum {
	ALLOC_NODE,			/*                                    */
	LOOKUP_NODE,			/*                                  */
	LOOKUP_NODE_RA,			/*
                                            
                            
      */
};

#define F2FS_LINK_MAX		32000	/*                             */

/*                                  */
struct extent_info {
	rwlock_t ext_lock;	/*                        */
	unsigned int fofs;	/*                        */
	u32 blk_addr;		/*                                   */
	unsigned int len;	/*                      */
};

/*
                                                                    
 */
#define FADVISE_COLD_BIT	0x01
#define FADVISE_CP_BIT		0x02

struct f2fs_inode_info {
	struct inode vfs_inode;		/*                   */
	unsigned long i_flags;		/*                               */
	unsigned char i_advise;		/*                                  */
	unsigned int i_current_depth;	/*                                 */
	unsigned int i_pino;		/*                     */
	umode_t i_acl_mode;		/*                                */

	/*                             */
	unsigned long flags;		/*                            */
	atomic_t dirty_dents;		/*                         */
	f2fs_hash_t chash;		/*                               */
	unsigned int clevel;		/*                                  */
	nid_t i_xattr_nid;		/*                              */
	struct extent_info ext;		/*                              */
};

static inline void get_extent_info(struct extent_info *ext,
					struct f2fs_extent i_ext)
{
	write_lock(&ext->ext_lock);
	ext->fofs = le32_to_cpu(i_ext.fofs);
	ext->blk_addr = le32_to_cpu(i_ext.blk_addr);
	ext->len = le32_to_cpu(i_ext.len);
	write_unlock(&ext->ext_lock);
}

static inline void set_raw_extent(struct extent_info *ext,
					struct f2fs_extent *i_ext)
{
	read_lock(&ext->ext_lock);
	i_ext->fofs = cpu_to_le32(ext->fofs);
	i_ext->blk_addr = cpu_to_le32(ext->blk_addr);
	i_ext->len = cpu_to_le32(ext->len);
	read_unlock(&ext->ext_lock);
}

struct f2fs_nm_info {
	block_t nat_blkaddr;		/*                          */
	nid_t max_nid;			/*                           */
	nid_t next_scan_nid;		/*                            */

	/*                      */
	struct radix_tree_root nat_root;/*                             */
	rwlock_t nat_tree_lock;		/*                       */
	unsigned int nat_cnt;		/*                             */
	struct list_head nat_entries;	/*                               */
	struct list_head dirty_nat_entries; /*                               */

	/*                          */
	struct list_head free_nid_list;	/*                      */
	spinlock_t free_nid_list_lock;	/*                       */
	unsigned int fcnt;		/*                            */
	struct mutex build_lock;	/*                          */

	/*                */
	char *nat_bitmap;		/*                    */
	int bitmap_size;		/*             */
};

/*
                                                        
                                                                            
                                
 */
struct dnode_of_data {
	struct inode *inode;		/*                   */
	struct page *inode_page;	/*                                  */
	struct page *node_page;		/*                         */
	nid_t nid;			/*                                  */
	unsigned int ofs_in_node;	/*                              */
	bool inode_page_locked;		/*                             */
	block_t	data_blkaddr;		/*                                 */
};

static inline void set_new_dnode(struct dnode_of_data *dn, struct inode *inode,
		struct page *ipage, struct page *npage, nid_t nid)
{
	memset(dn, 0, sizeof(*dn));
	dn->inode = inode;
	dn->inode_page = ipage;
	dn->node_page = npage;
	dn->nid = nid;
}

/*
                  
  
                                                                       
                                                                             
                                                                             
                
                                                                          
                                                                            
                                                                      
                                                                            
                            
 */
#define	NR_CURSEG_DATA_TYPE	(3)
#define NR_CURSEG_NODE_TYPE	(3)
#define NR_CURSEG_TYPE	(NR_CURSEG_DATA_TYPE + NR_CURSEG_NODE_TYPE)

enum {
	CURSEG_HOT_DATA	= 0,	/*                        */
	CURSEG_WARM_DATA,	/*             */
	CURSEG_COLD_DATA,	/*                                */
	CURSEG_HOT_NODE,	/*                                       */
	CURSEG_WARM_NODE,	/*                                    */
	CURSEG_COLD_NODE,	/*                      */
	NO_CHECK_TYPE
};

struct f2fs_sm_info {
	struct sit_info *sit_info;		/*                           */
	struct free_segmap_info *free_info;	/*                          */
	struct dirty_seglist_info *dirty_info;	/*                           */
	struct curseg_info *curseg_array;	/*                            */

	struct list_head wblist_head;	/*                               */
	spinlock_t wblist_lock;		/*                     */

	block_t seg0_blkaddr;		/*                               */
	block_t main_blkaddr;		/*                                  */
	block_t ssa_blkaddr;		/*                                 */

	unsigned int segment_count;	/*                     */
	unsigned int main_segments;	/*                            */
	unsigned int reserved_segments;	/*                        */
	unsigned int ovp_segments;	/*                             */
};

/*
                          
 */
#define	NODE_DIR1_BLOCK		(ADDRS_PER_INODE + 1)
#define	NODE_DIR2_BLOCK		(ADDRS_PER_INODE + 2)
#define	NODE_IND1_BLOCK		(ADDRS_PER_INODE + 3)
#define	NODE_IND2_BLOCK		(ADDRS_PER_INODE + 4)
#define	NODE_DIND_BLOCK		(ADDRS_PER_INODE + 5)

/*
                 
 */
/*
                            
  
                                                                        
                                                                 
 */
enum count_type {
	F2FS_WRITEBACK,
	F2FS_DIRTY_DENTS,
	F2FS_DIRTY_NODES,
	F2FS_DIRTY_META,
	NR_COUNT_TYPE,
};

/*
                                         
                                                                       
                                                                    
                                                    
 */
#define NR_GLOBAL_LOCKS	8

/*
                                                             
                           
                                                     
                                                
                                                 
                                          
                                                       
                                      
                                    
 */
enum page_type {
	DATA,
	NODE,
	META,
	NR_PAGE_TYPE,
	META_FLUSH,
};

struct f2fs_sb_info {
	struct super_block *sb;			/*                            */
	struct buffer_head *raw_super_buf;	/*                       */
	struct f2fs_super_block *raw_super;	/*                         */
	int s_dirty;				/*                           */

	/*                             */
	struct f2fs_nm_info *nm_info;		/*              */
	struct inode *node_inode;		/*                   */

	/*                                */
	struct f2fs_sm_info *sm_info;		/*                 */
	struct bio *bio[NR_PAGE_TYPE];		/*               */
	sector_t last_block_in_bio[NR_PAGE_TYPE];	/*                   */
	struct rw_semaphore bio_sem;		/*              */

	/*                */
	struct f2fs_checkpoint *ckpt;		/*                        */
	struct inode *meta_inode;		/*                   */
	struct mutex cp_mutex;			/*                           */
	struct mutex fs_lock[NR_GLOBAL_LOCKS];	/*                        */
	struct mutex node_write;		/*                     */
	struct mutex writepages;		/*                        */
	unsigned char next_lock_num;		/*                          */
	int por_doing;				/*                          */
	int on_build_free_nids;			/*                          */

	/*                             */
	struct list_head orphan_inode_list;	/*                   */
	struct mutex orphan_inode_mutex;	/*                       */
	unsigned int n_orphans;			/*                    */

	/*                                */
	struct list_head dir_inode_list;	/*                */
	spinlock_t dir_inode_lock;		/*                         */
	unsigned int n_dirty_dirs;		/*                 */

	/*                         */
	unsigned int log_sectors_per_block;	/*                        */
	unsigned int log_blocksize;		/*                 */
	unsigned int blocksize;			/*            */
	unsigned int root_ino_num;		/*                  */
	unsigned int node_ino_num;		/*                  */
	unsigned int meta_ino_num;		/*                  */
	unsigned int log_blocks_per_seg;	/*                         */
	unsigned int blocks_per_seg;		/*                    */
	unsigned int segs_per_sec;		/*                      */
	unsigned int secs_per_zone;		/*                   */
	unsigned int total_sections;		/*                     */
	unsigned int total_node_count;		/*                        */
	unsigned int total_valid_node_count;	/*                        */
	unsigned int total_valid_inode_count;	/*                   */
	int active_logs;			/*                  */

	block_t user_block_count;		/*                  */
	block_t total_valid_block_count;	/*                   */
	block_t alloc_valid_block_count;	/*                       */
	block_t last_valid_block_count;		/*              */
	u32 s_next_generation;			/*                 */
	atomic_t nr_pages[NR_COUNT_TYPE];	/*                            */

	struct f2fs_mount_info mount_opt;	/*               */

	/*                         */
	struct mutex gc_mutex;			/*              */
	struct f2fs_gc_kthread	*gc_thread;	/*           */
	unsigned int cur_victim_sec;		/*                            */

	/*
                         
                                                               
  */
	struct f2fs_stat_info *stat_info;	/*                       */
	unsigned int segment_count[2];		/*                         */
	unsigned int block_count[2];		/*                       */
	unsigned int last_victim[2];		/*                       */
	int total_hit_ext, read_hit_ext;	/*                        */
	int bg_gc;				/*                     */
	spinlock_t stat_lock;			/*                          */
};

/*
                   
 */
static inline struct f2fs_inode_info *F2FS_I(struct inode *inode)
{
	return container_of(inode, struct f2fs_inode_info, vfs_inode);
}

static inline struct f2fs_sb_info *F2FS_SB(struct super_block *sb)
{
	return sb->s_fs_info;
}

static inline struct f2fs_super_block *F2FS_RAW_SUPER(struct f2fs_sb_info *sbi)
{
	return (struct f2fs_super_block *)(sbi->raw_super);
}

static inline struct f2fs_checkpoint *F2FS_CKPT(struct f2fs_sb_info *sbi)
{
	return (struct f2fs_checkpoint *)(sbi->ckpt);
}

static inline struct f2fs_nm_info *NM_I(struct f2fs_sb_info *sbi)
{
	return (struct f2fs_nm_info *)(sbi->nm_info);
}

static inline struct f2fs_sm_info *SM_I(struct f2fs_sb_info *sbi)
{
	return (struct f2fs_sm_info *)(sbi->sm_info);
}

static inline struct sit_info *SIT_I(struct f2fs_sb_info *sbi)
{
	return (struct sit_info *)(SM_I(sbi)->sit_info);
}

static inline struct free_segmap_info *FREE_I(struct f2fs_sb_info *sbi)
{
	return (struct free_segmap_info *)(SM_I(sbi)->free_info);
}

static inline struct dirty_seglist_info *DIRTY_I(struct f2fs_sb_info *sbi)
{
	return (struct dirty_seglist_info *)(SM_I(sbi)->dirty_info);
}

static inline void F2FS_SET_SB_DIRT(struct f2fs_sb_info *sbi)
{
	sbi->s_dirty = 1;
}

static inline void F2FS_RESET_SB_DIRT(struct f2fs_sb_info *sbi)
{
	sbi->s_dirty = 0;
}

static inline bool is_set_ckpt_flags(struct f2fs_checkpoint *cp, unsigned int f)
{
	unsigned int ckpt_flags = le32_to_cpu(cp->ckpt_flags);
	return ckpt_flags & f;
}

static inline void set_ckpt_flags(struct f2fs_checkpoint *cp, unsigned int f)
{
	unsigned int ckpt_flags = le32_to_cpu(cp->ckpt_flags);
	ckpt_flags |= f;
	cp->ckpt_flags = cpu_to_le32(ckpt_flags);
}

static inline void clear_ckpt_flags(struct f2fs_checkpoint *cp, unsigned int f)
{
	unsigned int ckpt_flags = le32_to_cpu(cp->ckpt_flags);
	ckpt_flags &= (~f);
	cp->ckpt_flags = cpu_to_le32(ckpt_flags);
}

static inline void mutex_lock_all(struct f2fs_sb_info *sbi)
{
	int i = 0;
	for (; i < NR_GLOBAL_LOCKS; i++)
		mutex_lock(&sbi->fs_lock[i]);
}

static inline void mutex_unlock_all(struct f2fs_sb_info *sbi)
{
	int i = 0;
	for (; i < NR_GLOBAL_LOCKS; i++)
		mutex_unlock(&sbi->fs_lock[i]);
}

static inline int mutex_lock_op(struct f2fs_sb_info *sbi)
{
	unsigned char next_lock = sbi->next_lock_num % NR_GLOBAL_LOCKS;
	int i = 0;

	for (; i < NR_GLOBAL_LOCKS; i++)
		if (mutex_trylock(&sbi->fs_lock[i]))
			return i;

	mutex_lock(&sbi->fs_lock[next_lock]);
	sbi->next_lock_num++;
	return next_lock;
}

static inline void mutex_unlock_op(struct f2fs_sb_info *sbi, int ilock)
{
	if (ilock < 0)
		return;
	BUG_ON(ilock >= NR_GLOBAL_LOCKS);
	mutex_unlock(&sbi->fs_lock[ilock]);
}

/*
                                                       
 */
static inline int check_nid_range(struct f2fs_sb_info *sbi, nid_t nid)
{
	WARN_ON((nid >= NM_I(sbi)->max_nid));
	if (nid >= NM_I(sbi)->max_nid)
		return -EINVAL;
	return 0;
}

#define F2FS_DEFAULT_ALLOCATED_BLOCKS	1

/*
                                            
 */
static inline int F2FS_HAS_BLOCKS(struct inode *inode)
{
	if (F2FS_I(inode)->i_xattr_nid)
		return (inode->i_blocks > F2FS_DEFAULT_ALLOCATED_BLOCKS + 1);
	else
		return (inode->i_blocks > F2FS_DEFAULT_ALLOCATED_BLOCKS);
}

static inline bool inc_valid_block_count(struct f2fs_sb_info *sbi,
				 struct inode *inode, blkcnt_t count)
{
	block_t	valid_block_count;

	spin_lock(&sbi->stat_lock);
	valid_block_count =
		sbi->total_valid_block_count + (block_t)count;
	if (valid_block_count > sbi->user_block_count) {
		spin_unlock(&sbi->stat_lock);
		return false;
	}
	inode->i_blocks += count;
	sbi->total_valid_block_count = valid_block_count;
	sbi->alloc_valid_block_count += (block_t)count;
	spin_unlock(&sbi->stat_lock);
	return true;
}

static inline int dec_valid_block_count(struct f2fs_sb_info *sbi,
						struct inode *inode,
						blkcnt_t count)
{
	spin_lock(&sbi->stat_lock);
	BUG_ON(sbi->total_valid_block_count < (block_t) count);
	BUG_ON(inode->i_blocks < count);
	inode->i_blocks -= count;
	sbi->total_valid_block_count -= (block_t)count;
	spin_unlock(&sbi->stat_lock);
	return 0;
}

static inline void inc_page_count(struct f2fs_sb_info *sbi, int count_type)
{
	atomic_inc(&sbi->nr_pages[count_type]);
	F2FS_SET_SB_DIRT(sbi);
}

static inline void inode_inc_dirty_dents(struct inode *inode)
{
	atomic_inc(&F2FS_I(inode)->dirty_dents);
}

static inline void dec_page_count(struct f2fs_sb_info *sbi, int count_type)
{
	atomic_dec(&sbi->nr_pages[count_type]);
}

static inline void inode_dec_dirty_dents(struct inode *inode)
{
	atomic_dec(&F2FS_I(inode)->dirty_dents);
}

static inline int get_pages(struct f2fs_sb_info *sbi, int count_type)
{
	return atomic_read(&sbi->nr_pages[count_type]);
}

static inline int get_blocktype_secs(struct f2fs_sb_info *sbi, int block_type)
{
	unsigned int pages_per_sec = sbi->segs_per_sec *
					(1 << sbi->log_blocks_per_seg);
	return ((get_pages(sbi, block_type) + pages_per_sec - 1)
			>> sbi->log_blocks_per_seg) / sbi->segs_per_sec;
}

static inline block_t valid_user_blocks(struct f2fs_sb_info *sbi)
{
	block_t ret;
	spin_lock(&sbi->stat_lock);
	ret = sbi->total_valid_block_count;
	spin_unlock(&sbi->stat_lock);
	return ret;
}

static inline unsigned long __bitmap_size(struct f2fs_sb_info *sbi, int flag)
{
	struct f2fs_checkpoint *ckpt = F2FS_CKPT(sbi);

	/*                          */
	if (flag == NAT_BITMAP)
		return le32_to_cpu(ckpt->nat_ver_bitmap_bytesize);
	else if (flag == SIT_BITMAP)
		return le32_to_cpu(ckpt->sit_ver_bitmap_bytesize);

	return 0;
}

static inline void *__bitmap_ptr(struct f2fs_sb_info *sbi, int flag)
{
	struct f2fs_checkpoint *ckpt = F2FS_CKPT(sbi);
	int offset = (flag == NAT_BITMAP) ?
			le32_to_cpu(ckpt->sit_ver_bitmap_bytesize) : 0;
	return &ckpt->sit_nat_version_bitmap + offset;
}

static inline block_t __start_cp_addr(struct f2fs_sb_info *sbi)
{
	block_t start_addr;
	struct f2fs_checkpoint *ckpt = F2FS_CKPT(sbi);
	unsigned long long ckpt_version = le64_to_cpu(ckpt->checkpoint_ver);

	start_addr = le32_to_cpu(F2FS_RAW_SUPER(sbi)->cp_blkaddr);

	/*
                                                  
                                           
  */
	if (!(ckpt_version & 1))
		start_addr += sbi->blocks_per_seg;

	return start_addr;
}

static inline block_t __start_sum_addr(struct f2fs_sb_info *sbi)
{
	return le32_to_cpu(F2FS_CKPT(sbi)->cp_pack_start_sum);
}

static inline bool inc_valid_node_count(struct f2fs_sb_info *sbi,
						struct inode *inode,
						unsigned int count)
{
	block_t	valid_block_count;
	unsigned int valid_node_count;

	spin_lock(&sbi->stat_lock);

	valid_block_count = sbi->total_valid_block_count + (block_t)count;
	sbi->alloc_valid_block_count += (block_t)count;
	valid_node_count = sbi->total_valid_node_count + count;

	if (valid_block_count > sbi->user_block_count) {
		spin_unlock(&sbi->stat_lock);
		return false;
	}

	if (valid_node_count > sbi->total_node_count) {
		spin_unlock(&sbi->stat_lock);
		return false;
	}

	if (inode)
		inode->i_blocks += count;
	sbi->total_valid_node_count = valid_node_count;
	sbi->total_valid_block_count = valid_block_count;
	spin_unlock(&sbi->stat_lock);

	return true;
}

static inline void dec_valid_node_count(struct f2fs_sb_info *sbi,
						struct inode *inode,
						unsigned int count)
{
	spin_lock(&sbi->stat_lock);

	BUG_ON(sbi->total_valid_block_count < count);
	BUG_ON(sbi->total_valid_node_count < count);
	BUG_ON(inode->i_blocks < count);

	inode->i_blocks -= count;
	sbi->total_valid_node_count -= count;
	sbi->total_valid_block_count -= (block_t)count;

	spin_unlock(&sbi->stat_lock);
}

static inline unsigned int valid_node_count(struct f2fs_sb_info *sbi)
{
	unsigned int ret;
	spin_lock(&sbi->stat_lock);
	ret = sbi->total_valid_node_count;
	spin_unlock(&sbi->stat_lock);
	return ret;
}

static inline void inc_valid_inode_count(struct f2fs_sb_info *sbi)
{
	spin_lock(&sbi->stat_lock);
	BUG_ON(sbi->total_valid_inode_count == sbi->total_node_count);
	sbi->total_valid_inode_count++;
	spin_unlock(&sbi->stat_lock);
}

static inline int dec_valid_inode_count(struct f2fs_sb_info *sbi)
{
	spin_lock(&sbi->stat_lock);
	BUG_ON(!sbi->total_valid_inode_count);
	sbi->total_valid_inode_count--;
	spin_unlock(&sbi->stat_lock);
	return 0;
}

static inline unsigned int valid_inode_count(struct f2fs_sb_info *sbi)
{
	unsigned int ret;
	spin_lock(&sbi->stat_lock);
	ret = sbi->total_valid_inode_count;
	spin_unlock(&sbi->stat_lock);
	return ret;
}

static inline void f2fs_put_page(struct page *page, int unlock)
{
	if (!page || IS_ERR(page))
		return;

	if (unlock) {
		BUG_ON(!PageLocked(page));
		unlock_page(page);
	}
	page_cache_release(page);
}

static inline void f2fs_put_dnode(struct dnode_of_data *dn)
{
	if (dn->node_page)
		f2fs_put_page(dn->node_page, 1);
	if (dn->inode_page && dn->node_page != dn->inode_page)
		f2fs_put_page(dn->inode_page, 0);
	dn->node_page = NULL;
	dn->inode_page = NULL;
}

static inline struct kmem_cache *f2fs_kmem_cache_create(const char *name,
					size_t size, void (*ctor)(void *))
{
	return kmem_cache_create(name, size, 0, SLAB_RECLAIM_ACCOUNT, ctor);
}

#define RAW_IS_INODE(p)	((p)->footer.nid == (p)->footer.ino)

static inline bool IS_INODE(struct page *page)
{
	struct f2fs_node *p = (struct f2fs_node *)page_address(page);
	return RAW_IS_INODE(p);
}

static inline __le32 *blkaddr_in_node(struct f2fs_node *node)
{
	return RAW_IS_INODE(node) ? node->i.i_addr : node->dn.addr;
}

static inline block_t datablock_addr(struct page *node_page,
		unsigned int offset)
{
	struct f2fs_node *raw_node;
	__le32 *addr_array;
	raw_node = (struct f2fs_node *)page_address(node_page);
	addr_array = blkaddr_in_node(raw_node);
	return le32_to_cpu(addr_array[offset]);
}

static inline int f2fs_test_bit(unsigned int nr, char *addr)
{
	int mask;

	addr += (nr >> 3);
	mask = 1 << (7 - (nr & 0x07));
	return mask & *addr;
}

static inline int f2fs_set_bit(unsigned int nr, char *addr)
{
	int mask;
	int ret;

	addr += (nr >> 3);
	mask = 1 << (7 - (nr & 0x07));
	ret = mask & *addr;
	*addr |= mask;
	return ret;
}

static inline int f2fs_clear_bit(unsigned int nr, char *addr)
{
	int mask;
	int ret;

	addr += (nr >> 3);
	mask = 1 << (7 - (nr & 0x07));
	ret = mask & *addr;
	*addr &= ~mask;
	return ret;
}

/*                                 */
enum {
	FI_NEW_INODE,		/*                                */
	FI_INC_LINK,		/*                           */
	FI_ACL_MODE,		/*                   */
	FI_NO_ALLOC,		/*                                */
};

static inline void set_inode_flag(struct f2fs_inode_info *fi, int flag)
{
	set_bit(flag, &fi->flags);
}

static inline int is_inode_flag_set(struct f2fs_inode_info *fi, int flag)
{
	return test_bit(flag, &fi->flags);
}

static inline void clear_inode_flag(struct f2fs_inode_info *fi, int flag)
{
	clear_bit(flag, &fi->flags);
}

static inline void set_acl_inode(struct f2fs_inode_info *fi, umode_t mode)
{
	fi->i_acl_mode = mode;
	set_inode_flag(fi, FI_ACL_MODE);
}

static inline int cond_clear_inode_flag(struct f2fs_inode_info *fi, int flag)
{
	if (is_inode_flag_set(fi, FI_ACL_MODE)) {
		clear_inode_flag(fi, FI_ACL_MODE);
		return 1;
	}
	return 0;
}

/*
         
 */
int f2fs_sync_file(struct file *, loff_t, loff_t, int);
void truncate_data_blocks(struct dnode_of_data *);
void f2fs_truncate(struct inode *);
int f2fs_setattr(struct dentry *, struct iattr *);
int truncate_hole(struct inode *, pgoff_t, pgoff_t);
long f2fs_ioctl(struct file *, unsigned int, unsigned long);
long f2fs_compat_ioctl(struct file *, unsigned int, unsigned long);

/*
          
 */
void f2fs_set_inode_flags(struct inode *);
struct inode *f2fs_iget(struct super_block *, unsigned long);
void update_inode(struct inode *, struct page *);
int update_inode_page(struct inode *);
int f2fs_write_inode(struct inode *, struct writeback_control *);
void f2fs_evict_inode(struct inode *);

/*
          
 */
struct dentry *f2fs_get_parent(struct dentry *child);

/*
        
 */
struct f2fs_dir_entry *f2fs_find_entry(struct inode *, struct qstr *,
							struct page **);
struct f2fs_dir_entry *f2fs_parent_dir(struct inode *, struct page **);
ino_t f2fs_inode_by_name(struct inode *, struct qstr *);
void f2fs_set_link(struct inode *, struct f2fs_dir_entry *,
				struct page *, struct inode *);
void init_dent_inode(const struct qstr *, struct page *);
int __f2fs_add_link(struct inode *, const struct qstr *, struct inode *);
void f2fs_delete_entry(struct f2fs_dir_entry *, struct page *, struct inode *);
int f2fs_make_empty(struct inode *, struct inode *);
bool f2fs_empty_dir(struct inode *);

static inline int f2fs_add_link(struct dentry *dentry, struct inode *inode)
{
	return __f2fs_add_link(dentry->d_parent->d_inode, &dentry->d_name,
				inode);
}

/*
          
 */
int f2fs_sync_fs(struct super_block *, int);
extern __printf(3, 4)
void f2fs_msg(struct super_block *, const char *, const char *, ...);

/*
         
 */
f2fs_hash_t f2fs_dentry_hash(const char *, size_t);

/*
         
 */
struct dnode_of_data;
struct node_info;

int is_checkpointed_node(struct f2fs_sb_info *, nid_t);
void get_node_info(struct f2fs_sb_info *, nid_t, struct node_info *);
int get_dnode_of_data(struct dnode_of_data *, pgoff_t, int);
int truncate_inode_blocks(struct inode *, pgoff_t);
int remove_inode_page(struct inode *);
int new_inode_page(struct inode *, const struct qstr *);
struct page *new_node_page(struct dnode_of_data *, unsigned int);
void ra_node_page(struct f2fs_sb_info *, nid_t);
struct page *get_node_page(struct f2fs_sb_info *, pgoff_t);
struct page *get_node_page_ra(struct page *, int);
void sync_inode_page(struct dnode_of_data *);
int sync_node_pages(struct f2fs_sb_info *, nid_t, struct writeback_control *);
bool alloc_nid(struct f2fs_sb_info *, nid_t *);
void alloc_nid_done(struct f2fs_sb_info *, nid_t);
void alloc_nid_failed(struct f2fs_sb_info *, nid_t);
void recover_node_page(struct f2fs_sb_info *, struct page *,
		struct f2fs_summary *, struct node_info *, block_t);
int recover_inode_page(struct f2fs_sb_info *, struct page *);
int restore_node_summary(struct f2fs_sb_info *, unsigned int,
				struct f2fs_summary_block *);
void flush_nat_entries(struct f2fs_sb_info *);
int build_node_manager(struct f2fs_sb_info *);
void destroy_node_manager(struct f2fs_sb_info *);
int __init create_node_manager_caches(void);
void destroy_node_manager_caches(void);

/*
            
 */
void f2fs_balance_fs(struct f2fs_sb_info *);
void invalidate_blocks(struct f2fs_sb_info *, block_t);
void locate_dirty_segment(struct f2fs_sb_info *, unsigned int);
void clear_prefree_segments(struct f2fs_sb_info *);
int npages_for_summary_flush(struct f2fs_sb_info *);
void allocate_new_segments(struct f2fs_sb_info *);
struct page *get_sum_page(struct f2fs_sb_info *, unsigned int);
struct bio *f2fs_bio_alloc(struct block_device *, int);
void f2fs_submit_bio(struct f2fs_sb_info *, enum page_type, bool sync);
void write_meta_page(struct f2fs_sb_info *, struct page *);
void write_node_page(struct f2fs_sb_info *, struct page *, unsigned int,
					block_t, block_t *);
void write_data_page(struct inode *, struct page *, struct dnode_of_data*,
					block_t, block_t *);
void rewrite_data_page(struct f2fs_sb_info *, struct page *, block_t);
void recover_data_page(struct f2fs_sb_info *, struct page *,
				struct f2fs_summary *, block_t, block_t);
void rewrite_node_page(struct f2fs_sb_info *, struct page *,
				struct f2fs_summary *, block_t, block_t);
void write_data_summaries(struct f2fs_sb_info *, block_t);
void write_node_summaries(struct f2fs_sb_info *, block_t);
int lookup_journal_in_cursum(struct f2fs_summary_block *,
					int, unsigned int, int);
void flush_sit_entries(struct f2fs_sb_info *);
int build_segment_manager(struct f2fs_sb_info *);
void destroy_segment_manager(struct f2fs_sb_info *);

/*
               
 */
struct page *grab_meta_page(struct f2fs_sb_info *, pgoff_t);
struct page *get_meta_page(struct f2fs_sb_info *, pgoff_t);
long sync_meta_pages(struct f2fs_sb_info *, enum page_type, long);
int check_orphan_space(struct f2fs_sb_info *);
void add_orphan_inode(struct f2fs_sb_info *, nid_t);
void remove_orphan_inode(struct f2fs_sb_info *, nid_t);
int recover_orphan_inodes(struct f2fs_sb_info *);
int get_valid_checkpoint(struct f2fs_sb_info *);
void set_dirty_dir_page(struct inode *, struct page *);
void remove_dirty_dir_inode(struct inode *);
void sync_dirty_dir_inodes(struct f2fs_sb_info *);
void write_checkpoint(struct f2fs_sb_info *, bool);
void init_orphan_info(struct f2fs_sb_info *);
int __init create_checkpoint_caches(void);
void destroy_checkpoint_caches(void);

/*
         
 */
int reserve_new_block(struct dnode_of_data *);
void update_extent_cache(block_t, struct dnode_of_data *);
struct page *find_data_page(struct inode *, pgoff_t, bool);
struct page *get_lock_data_page(struct inode *, pgoff_t);
struct page *get_new_data_page(struct inode *, pgoff_t, bool);
int f2fs_readpage(struct f2fs_sb_info *, struct page *, block_t, int);
int do_write_data_page(struct page *);

/*
       
 */
int start_gc_thread(struct f2fs_sb_info *);
void stop_gc_thread(struct f2fs_sb_info *);
block_t start_bidx_of_node(unsigned int);
int f2fs_gc(struct f2fs_sb_info *);
void build_gc_manager(struct f2fs_sb_info *);
int __init create_gc_caches(void);
void destroy_gc_caches(void);

/*
             
 */
int recover_fsync_data(struct f2fs_sb_info *);
bool space_for_roll_forward(struct f2fs_sb_info *);

/*
          
 */
#ifdef CONFIG_F2FS_STAT_FS
struct f2fs_stat_info {
	struct list_head stat_list;
	struct f2fs_sb_info *sbi;
	struct mutex stat_lock;
	int all_area_segs, sit_area_segs, nat_area_segs, ssa_area_segs;
	int main_area_segs, main_area_sections, main_area_zones;
	int hit_ext, total_ext;
	int ndirty_node, ndirty_dent, ndirty_dirs, ndirty_meta;
	int nats, sits, fnids;
	int total_count, utilization;
	int bg_gc;
	unsigned int valid_count, valid_node_count, valid_inode_count;
	unsigned int bimodal, avg_vblocks;
	int util_free, util_valid, util_invalid;
	int rsvd_segs, overp_segs;
	int dirty_count, node_pages, meta_pages;
	int prefree_count, call_count;
	int tot_segs, node_segs, data_segs, free_segs, free_secs;
	int tot_blks, data_blks, node_blks;
	int curseg[NR_CURSEG_TYPE];
	int cursec[NR_CURSEG_TYPE];
	int curzone[NR_CURSEG_TYPE];

	unsigned int segment_count[2];
	unsigned int block_count[2];
	unsigned base_mem, cache_mem;
};

#define stat_inc_call_count(si)	((si)->call_count++)

#define stat_inc_seg_count(sbi, type)					\
	do {								\
		struct f2fs_stat_info *si = sbi->stat_info;		\
		(si)->tot_segs++;					\
		if (type == SUM_TYPE_DATA)				\
			si->data_segs++;				\
		else							\
			si->node_segs++;				\
	} while (0)

#define stat_inc_tot_blk_count(si, blks)				\
	(si->tot_blks += (blks))

#define stat_inc_data_blk_count(sbi, blks)				\
	do {								\
		struct f2fs_stat_info *si = sbi->stat_info;		\
		stat_inc_tot_blk_count(si, blks);			\
		si->data_blks += (blks);				\
	} while (0)

#define stat_inc_node_blk_count(sbi, blks)				\
	do {								\
		struct f2fs_stat_info *si = sbi->stat_info;		\
		stat_inc_tot_blk_count(si, blks);			\
		si->node_blks += (blks);				\
	} while (0)

int f2fs_build_stats(struct f2fs_sb_info *);
void f2fs_destroy_stats(struct f2fs_sb_info *);
void __init f2fs_create_root_stats(void);
void f2fs_destroy_root_stats(void);
#else
#define stat_inc_call_count(si)
#define stat_inc_seg_count(si, type)
#define stat_inc_tot_blk_count(si, blks)
#define stat_inc_data_blk_count(si, blks)
#define stat_inc_node_blk_count(sbi, blks)

static inline int f2fs_build_stats(struct f2fs_sb_info *sbi) { return 0; }
static inline void f2fs_destroy_stats(struct f2fs_sb_info *sbi) { }
static inline void __init f2fs_create_root_stats(void) { }
static inline void f2fs_destroy_root_stats(void) { }
#endif

extern const struct file_operations f2fs_dir_operations;
extern const struct file_operations f2fs_file_operations;
extern const struct inode_operations f2fs_file_inode_operations;
extern const struct address_space_operations f2fs_dblock_aops;
extern const struct address_space_operations f2fs_node_aops;
extern const struct address_space_operations f2fs_meta_aops;
extern const struct inode_operations f2fs_dir_inode_operations;
extern const struct inode_operations f2fs_symlink_inode_operations;
extern const struct inode_operations f2fs_special_inode_operations;
#endif