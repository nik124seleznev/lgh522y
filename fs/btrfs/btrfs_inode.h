/*
 * Copyright (C) 2007 Oracle.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License v2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 021110-1307, USA.
 */

#ifndef __BTRFS_I__
#define __BTRFS_I__

#include "extent_map.h"
#include "extent_io.h"
#include "ordered-data.h"
#include "delayed-inode.h"

/*
                                                              
                                                                
                                                         
                                                                
                                                           
 */
#define BTRFS_INODE_ORDERED_DATA_CLOSE		0
#define BTRFS_INODE_ORPHAN_META_RESERVED	1
#define BTRFS_INODE_DUMMY			2
#define BTRFS_INODE_IN_DEFRAG			3
#define BTRFS_INODE_DELALLOC_META_RESERVED	4
#define BTRFS_INODE_HAS_ORPHAN_ITEM		5
#define BTRFS_INODE_HAS_ASYNC_EXTENT		6
#define BTRFS_INODE_NEEDS_FULL_SYNC		7
#define BTRFS_INODE_COPY_EVERYTHING		8
#define BTRFS_INODE_IN_DELALLOC_LIST		9
#define BTRFS_INODE_READDIO_NEED_LOCK		10

/*                       */
struct btrfs_inode {
	/*                                       */
	struct btrfs_root *root;

	/*                                                               
                                  
  */
	struct btrfs_key location;

	/*                   */
	spinlock_t lock;

	/*                                                               */
	struct extent_map_tree extent_tree;

	/*                                                  */
	struct extent_io_tree io_tree;

	/*                                                                    
                                               
  */
	struct extent_io_tree io_failure_tree;

	/*                                            */
	struct mutex log_mutex;

	/*                                        */
	struct mutex delalloc_mutex;

	/*                                 */
	struct btrfs_ordered_inode_tree ordered_tree;

	/*                                                                    
                                                                  
                     
  */
	struct list_head delalloc_inodes;

	/*
                                                               
                             
  */
	struct list_head ordered_operations;

	/*                                                                 */
	struct rb_node rb_node;

	unsigned long runtime_flags;

	/*                                               */
	atomic_t sync_writers;

	/*                                                                   
                          
  */
	u64 generation;

	/*
                                                             
  */
	u64 last_trans;

	/*
                                                 
  */
	u64 last_sub_trans;

	/*
                                       
  */
	u64 logged_trans;

	/*                                                                 
                                
  */
	u64 delalloc_bytes;

	/*
                                                                      
                                                                    
                                               
  */
	u64 disk_i_size;

	/*
                                                                      
                                         
  */
	u64 index_cnt;

	/*                                                               
                                                           
                                                         
           
  */
	u64 last_unlink_trans;

	/*
                                                                      
                              
  */
	u64 csum_bytes;

	/*                                    */
	u32 flags;

	/*                                        */
	unsigned long last_log_commit;

	/*
                                                                        
                                                                      
                                                                         
                                                
  */
	unsigned outstanding_extents;
	unsigned reserved_extents;

	/*
                                 
  */
	unsigned force_compress;

	struct btrfs_delayed_node *delayed_node;

	struct inode vfs_inode;
};

extern unsigned char btrfs_filetype_table[];

static inline struct btrfs_inode *BTRFS_I(struct inode *inode)
{
	return container_of(inode, struct btrfs_inode, vfs_inode);
}

static inline u64 btrfs_ino(struct inode *inode)
{
	u64 ino = BTRFS_I(inode)->location.objectid;

	/*
                     
                                           
  */
	if (!ino || BTRFS_I(inode)->location.type == BTRFS_ROOT_ITEM_KEY)
		ino = inode->i_ino;
	return ino;
}

static inline void btrfs_i_size_write(struct inode *inode, u64 size)
{
	i_size_write(inode, size);
	BTRFS_I(inode)->disk_i_size = size;
}

static inline bool btrfs_is_free_space_inode(struct inode *inode)
{
	struct btrfs_root *root = BTRFS_I(inode)->root;

	if (root == root->fs_info->tree_root &&
	    btrfs_ino(inode) != BTRFS_BTREE_INODE_OBJECTID)
		return true;
	if (BTRFS_I(inode)->location.objectid == BTRFS_FREE_INO_OBJECTID)
		return true;
	return false;
}

static inline int btrfs_inode_in_log(struct inode *inode, u64 generation)
{
	if (BTRFS_I(inode)->logged_trans == generation &&
	    BTRFS_I(inode)->last_sub_trans <= BTRFS_I(inode)->last_log_commit)
		return 1;
	return 0;
}

/*
                                                                          
                                                                   
                      
 */
static inline void btrfs_inode_block_unlocked_dio(struct inode *inode)
{
	set_bit(BTRFS_INODE_READDIO_NEED_LOCK, &BTRFS_I(inode)->runtime_flags);
	smp_mb();
}

static inline void btrfs_inode_resume_unlocked_dio(struct inode *inode)
{
	smp_mb__before_clear_bit();
	clear_bit(BTRFS_INODE_READDIO_NEED_LOCK,
		  &BTRFS_I(inode)->runtime_flags);
}

#endif
