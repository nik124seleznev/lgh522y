/*
 * Copyright (c) International Business Machines Corp., 2006
 * Copyright (c) Nokia Corporation, 2006, 2007
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * Author: Artem Bityutskiy (Битюцкий Артём)
 */

#ifndef __UBI_UBI_H__
#define __UBI_UBI_H__

#include <linux/init.h>
#include <linux/types.h>
#include <linux/list.h>
#include <linux/rbtree.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/mutex.h>
#include <linux/rwsem.h>
#include <linux/spinlock.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <linux/notifier.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/ubi.h>
#include <asm/pgtable.h>

#include "ubi-media.h"

/*                                         */
#define UBI_MAX_DEVICES 32

/*                                                 */
#define UBI_NAME_STR "ubi"

/*                     */
#define ubi_msg(fmt, ...) pr_notice("UBI: " fmt "\n", ##__VA_ARGS__)
/*                      */
#define ubi_warn(fmt, ...) pr_warn("UBI warning: %s: " fmt "\n",  \
				   __func__, ##__VA_ARGS__)
/*                    */
#define ubi_err(fmt, ...) pr_err("UBI error: %s: " fmt "\n",      \
				 __func__, ##__VA_ARGS__)

/*                                */
#define UBI_BGT_NAME_PATTERN "ubi_bgt%dd"

/*
                                                                
                                                   
 */
#define UBI_LEB_UNMAPPED -1

/*
                                                                            
                                                                           
 */
#define UBI_IO_RETRIES 3

/*
                                                                              
                                                                            
          
 */
#define UBI_PROT_QUEUE_LEN 10

/*                                                   */
#define UBI_UNKNOWN -1

/*
                                                                              
                                                        
 */
#define UBI_DFS_DIR_NAME "ubi%d"
#define UBI_DFS_DIR_LEN  (3 + 2 + 1)

/*
                                              
  
                                                          
                                                                             
                                                                 
                                                                
                                                                       
                                                                            
                                                                          
                                                                    
                                                         
  
                                                                              
                                                                             
                                                                  
 */
enum {
	UBI_IO_FF = 1,
	UBI_IO_FF_BITFLIPS,
	UBI_IO_BAD_HDR,
	UBI_IO_BAD_HDR_EBADMSG,
	UBI_IO_BITFLIPS,
};

/*
                                                     
  
                                                                             
                                                                             
                                                                              
                          
                                                                              
                          
                                                                             
                          
                                                                        
                                   
                                      
 */
enum {
	MOVE_CANCEL_RACE = 1,
	MOVE_SOURCE_RD_ERR,
	MOVE_TARGET_RD_ERR,
	MOVE_TARGET_WR_ERR,
	MOVE_TARGET_BITFLIPS,
	MOVE_RETRY,
};

/*
                                         
  
                                                   
                                                         
 */
enum {
	UBI_NO_FASTMAP = 1,
	UBI_BAD_FASTMAP,
};

/* 
                                             
                                                       
                                        
                     
                                    
  
                                                                             
                                                                             
                                           
 */
struct ubi_wl_entry {
	union {
		struct rb_node rb;
		struct list_head list;
	} u;
	int ec;
	int pnum;
};

/* 
                                                      
                           
                                                      
                                          
                                                                          
                                                                           
                                                  
  
                                                                         
                                                                     
                                                                             
                                  
 */
struct ubi_ltree_entry {
	struct rb_node rb;
	int vol_id;
	int lnum;
	int users;
	struct rw_semaphore mutex;
};

/* 
                                                                       
                                        
                             
                                                                    
                                  
                                           
  
                                                                               
                                                                        
                                                                              
           
 */
struct ubi_rename_entry {
	int new_name_len;
	char new_name[UBI_VOL_NAME_MAX + 1];
	int remove;
	struct ubi_volume_desc *desc;
	struct list_head list;
};

struct ubi_volume_desc;

/* 
                                                                
                                       
                                                 
                                    
                                                
                                                                        
 */
struct ubi_fastmap_layout {
	struct ubi_wl_entry *e[UBI_FM_MAX_BLOCKS];
	int to_be_tortured[UBI_FM_MAX_BLOCKS];
	int used_blocks;
	int max_pool_size;
	int max_wl_pool_size;
};

/* 
                                              
                           
                             
                                           
                                      
  
                                          
                                                                     
                                                           
  
 */
struct ubi_fm_pool {
	int pebs[UBI_FM_MAX_POOL_SIZE];
	int used;
	int size;
	int max_size;
};

/* 
                                                             
                                                                
                                                            
                                                       
                     
                                     
                                                                  
                                                                   
                                                                   
  
                                                                             
                                                                     
                                                            
                                                                      
                                                                           
                                                           
                               
                                                                               
                                             
                                
                     
  
                                                            
                                                                        
                      
                                                                              
                                
                                                                           
                                   
                                                                           
                              
  
                                                        
                                                 
                                                                  
                                                              
                                               
                                                                          
                                                                  
  
                                                                          
                                                                        
                                                                     
             
  
                                                                             
                                                         
 */
struct ubi_volume {
	struct device dev;
	struct cdev cdev;
	struct ubi_device *ubi;
	int vol_id;
	int ref_count;
	int readers;
	int writers;
	int exclusive;

	int reserved_pebs;
	int vol_type;
	int usable_leb_size;
	int used_ebs;
	int last_eb_bytes;
	long long used_bytes;
	int alignment;
	int data_pad;
	int name_len;
	char name[UBI_VOL_NAME_MAX + 1];

	int upd_ebs;
	int ch_lnum;
	long long upd_bytes;
	long long upd_received;
	void *upd_buf;

	int *eba_tbl;
	unsigned int checked:1;
	unsigned int corrupted:1;
	unsigned int upd_marker:1;
	unsigned int updating:1;
	unsigned int changing_leb:1;
	unsigned int direct_writes:1;
};

/* 
                                                                             
                                                                 
                                                                      
 */
struct ubi_volume_desc {
	struct ubi_volume *vol;
	int mode;
};

struct ubi_wl_entry;

/* 
                                                                   
  
                                                    
                                               
                                                                 
                                                            
                                                                          
                                                                               
                                                                
                                                                
                                                           
                                                                
                                                           
                                                                         
 */
struct ubi_debug_info {
	unsigned int chk_gen:1;
	unsigned int chk_io:1;
	unsigned int disable_bgt:1;
	unsigned int emulate_bitflips:1;
	unsigned int emulate_io_failures:1;
	char dfs_dir_name[UBI_DFS_DIR_LEN + 1];
	struct dentry *dfs_dir;
	struct dentry *dfs_chk_gen;
	struct dentry *dfs_chk_io;
	struct dentry *dfs_disable_bgt;
	struct dentry *dfs_emulate_bitflips;
	struct dentry *dfs_emulate_io_failures;
};

/* 
                                                       
                                                            
                                                            
                              
                             
                                                   
                                       
                                                                            
                                                                               
                                                                
                                                                   
                                                    
                                                           
  
                                                     
                                                       
                                                                         
                           
                                                                      
  
                                                                               
                                            
                                                                
                                                
                                  
                                                                      
                                                                      
                          
  
                                               
                                             
  
                                        
                                                        
                        
                                                        
  
                                                          
                                                              
                                                         
                                                                           
              
                                                                  
                                                          
                                  
                                                                  
                               
  
                                              
                                                             
                                              
                                                        
                                                               
                                                                            
                                                
                                  
                                                                              
                                                                     
                                                   
                                           
                                                       
                                                             
                                                                            
                                  
                                                                     
                                                                 
                                        
                                
                                       
                                                    
                                                       
                                    
  
                                                     
                                                              
                                      
                                                                 
                                                    
                                                      
                                                                              
                                
                                                                              
                                                                           
                                                                            
                                                                       
                                                   
                                     
                                                                     
                          
                                                                     
                                                                       
                                                                             
                              
                                                                  
                    
                                                               
                                                                             
                    
                                                      
                                                                               
                                                
                              
  
                                                             
                                
                                                               
  
                                                  
 */
struct ubi_device {
	struct cdev cdev;
	struct device dev;
	int ubi_num;
	char ubi_name[sizeof(UBI_NAME_STR)+5];
	int vol_count;
	struct ubi_volume *volumes[UBI_MAX_VOLUMES+UBI_INT_VOL_COUNT];
	spinlock_t volumes_lock;
	int ref_count;
	int image_seq;

	int rsvd_pebs;
	int avail_pebs;
	int beb_rsvd_pebs;
	int beb_rsvd_level;
	int bad_peb_limit;

	int autoresize_vol_id;
	int vtbl_slots;
	int vtbl_size;
	struct ubi_vtbl_record *vtbl;
	struct mutex device_mutex;

	int max_ec;
	/*                                                         */
	int mean_ec;

	/*                        */
	unsigned long long global_sqnum;
	spinlock_t ltree_lock;
	struct rb_root ltree;
	struct mutex alc_mutex;

	/*               */
	int fm_disabled;
	struct ubi_fastmap_layout *fm;
	struct ubi_fm_pool fm_pool;
	struct ubi_fm_pool fm_wl_pool;
	struct rw_semaphore fm_sem;
	struct mutex fm_mutex;
	void *fm_buf;
	size_t fm_size;
	struct work_struct fm_work;

	/*                                  */
	struct rb_root used;
	struct rb_root erroneous;
	struct rb_root free;
	int free_count;
	struct rb_root scrub;
	struct list_head pq[UBI_PROT_QUEUE_LEN];
	int pq_head;
	spinlock_t wl_lock;
	struct mutex move_mutex;
	struct rw_semaphore work_sem;
	int wl_scheduled;
	struct ubi_wl_entry **lookuptbl;
	struct ubi_wl_entry *move_from;
	struct ubi_wl_entry *move_to;
	int move_to_put;
	struct list_head works;
	int works_count;
	struct task_struct *bgt_thread;
	int thread_enabled;
	char bgt_name[sizeof(UBI_BGT_NAME_PATTERN)+2];

	/*                        */
	long long flash_size;
	int peb_count;
	int peb_size;
	int bad_peb_count;
	int good_peb_count;
	int corr_peb_count;
	int erroneous_peb_count;
	int max_erroneous;
	int min_io_size;
	int hdrs_min_io_size;
	int ro_mode;
	int leb_size;
	int leb_start;
	int ec_hdr_alsize;
	int vid_hdr_alsize;
	int vid_hdr_offset;
	int vid_hdr_aloffset;
	int vid_hdr_shift;
	unsigned int bad_allowed:1;
	unsigned int nor_flash:1;
	int max_write_size;
	struct mtd_info *mtd;

	void *peb_buf;
	struct mutex buf_mutex;
	struct mutex ckvol_mutex;

	struct ubi_debug_info dbg;
};

/* 
                                                                        
                                                     
                                    
                                                
                                   
                                                      
                                                                               
                          
                                    
                                                                        
                                               
  
                                                                         
                                                                          
                                                                        
 */
struct ubi_ainf_peb {
	int ec;
	int pnum;
	int vol_id;
	int lnum;
	unsigned int scrub:1;
	unsigned int copy_flag:1;
	unsigned long long sqnum;
	union {
		struct rb_node rb;
		struct list_head list;
	} u;
};

/* 
                                                                 
                     
                                                                  
                                                           
                         
                                                                         
                             
                                                                         
                                                                              
                                                    
                                                                             
                                                    
                                              
                                  
                                                                             
                                               
  
                                                                             
          
 */
struct ubi_ainf_volume {
	int vol_id;
	int highest_lnum;
	int leb_count;
	int vol_type;
	int used_ebs;
	int last_data_size;
	int data_pad;
	int compat;
	struct rb_node rb;
	struct rb_root root;
};

/* 
                                                             
                                       
                                                
                                           
                                                               
                                                                              
                                                                     
                                                   
                                                                           
                                
                                                     
                                                    
                                                                               
                                                            
                                       
                                     
                                                                    
                                      
                                       
                                            
                                     
                                                               
                                                                 
                                                               
  
                                                                             
                                                                               
                            
 */
struct ubi_attach_info {
	struct rb_root volumes;
	struct list_head corr;
	struct list_head free;
	struct list_head erase;
	struct list_head alien;
	int corr_peb_count;
	int empty_peb_count;
	int alien_peb_count;
	int bad_peb_count;
	int maybe_bad_peb_count;
	int vols_found;
	int highest_vol_id;
	int is_empty;
	int min_ec;
	int max_ec;
	unsigned long long max_sqnum;
	int mean_ec;
	uint64_t ec_sum;
	int ec_count;
	struct kmem_cache *aeb_slab_cache;
};

/* 
                                                         
                                             
                         
                                   
                                                                  
                                       
                                                          
                                                      
  
                                                                              
                                                                           
                                                                            
                   
 */
struct ubi_work {
	struct list_head list;
	int (*func)(struct ubi_device *ubi, struct ubi_work *wrk, int cancel);
	/*                                                     */
	struct ubi_wl_entry *e;
	int vol_id;
	int lnum;
	int torture;
	int anchor;
};

#include "debug.h"

extern struct kmem_cache *ubi_wl_entry_slab;
extern const struct file_operations ubi_ctrl_cdev_operations;
extern const struct file_operations ubi_cdev_operations;
extern const struct file_operations ubi_vol_cdev_operations;
extern struct class *ubi_class;
extern struct mutex ubi_devices_mutex;
extern struct blocking_notifier_head ubi_notifiers;

/*          */
int ubi_add_to_av(struct ubi_device *ubi, struct ubi_attach_info *ai, int pnum,
		  int ec, const struct ubi_vid_hdr *vid_hdr, int bitflips);
struct ubi_ainf_volume *ubi_find_av(const struct ubi_attach_info *ai,
				    int vol_id);
void ubi_remove_av(struct ubi_attach_info *ai, struct ubi_ainf_volume *av);
struct ubi_ainf_peb *ubi_early_get_peb(struct ubi_device *ubi,
				       struct ubi_attach_info *ai);
int ubi_attach(struct ubi_device *ubi, int force_scan);
void ubi_destroy_ai(struct ubi_attach_info *ai);

/*        */
int ubi_change_vtbl_record(struct ubi_device *ubi, int idx,
			   struct ubi_vtbl_record *vtbl_rec);
int ubi_vtbl_rename_volumes(struct ubi_device *ubi,
			    struct list_head *rename_list);
int ubi_read_volume_table(struct ubi_device *ubi, struct ubi_attach_info *ai);

/*       */
int ubi_create_volume(struct ubi_device *ubi, struct ubi_mkvol_req *req);
int ubi_remove_volume(struct ubi_volume_desc *desc, int no_vtbl);
int ubi_resize_volume(struct ubi_volume_desc *desc, int reserved_pebs);
int ubi_rename_volumes(struct ubi_device *ubi, struct list_head *rename_list);
int ubi_add_volume(struct ubi_device *ubi, struct ubi_volume *vol);
void ubi_free_volume(struct ubi_device *ubi, struct ubi_volume *vol);

/*       */
int ubi_start_update(struct ubi_device *ubi, struct ubi_volume *vol,
		     long long bytes);
int ubi_more_update_data(struct ubi_device *ubi, struct ubi_volume *vol,
			 const void __user *buf, int count);
int ubi_start_leb_change(struct ubi_device *ubi, struct ubi_volume *vol,
			 const struct ubi_leb_change_req *req);
int ubi_more_leb_change_data(struct ubi_device *ubi, struct ubi_volume *vol,
			     const void __user *buf, int count);

/*        */
int ubi_calc_data_len(const struct ubi_device *ubi, const void *buf,
		      int length);
int ubi_check_volume(struct ubi_device *ubi, int vol_id);
void ubi_update_reserved(struct ubi_device *ubi);
void ubi_calculate_reserved(struct ubi_device *ubi);
int ubi_check_pattern(const void *buf, uint8_t patt, int size);

/*       */
int ubi_eba_unmap_leb(struct ubi_device *ubi, struct ubi_volume *vol,
		      int lnum);
int ubi_eba_read_leb(struct ubi_device *ubi, struct ubi_volume *vol, int lnum,
		     void *buf, int offset, int len, int check);
int ubi_eba_write_leb(struct ubi_device *ubi, struct ubi_volume *vol, int lnum,
		      const void *buf, int offset, int len);
int ubi_eba_write_leb_st(struct ubi_device *ubi, struct ubi_volume *vol,
			 int lnum, const void *buf, int len, int used_ebs);
int ubi_eba_atomic_leb_change(struct ubi_device *ubi, struct ubi_volume *vol,
			      int lnum, const void *buf, int len);
int ubi_eba_copy_leb(struct ubi_device *ubi, int from, int to,
		     struct ubi_vid_hdr *vid_hdr);
int ubi_eba_init(struct ubi_device *ubi, struct ubi_attach_info *ai);
unsigned long long ubi_next_sqnum(struct ubi_device *ubi);
int self_check_eba(struct ubi_device *ubi, struct ubi_attach_info *ai_fastmap,
		   struct ubi_attach_info *ai_scan);

/*      */
int ubi_wl_get_peb(struct ubi_device *ubi);
int ubi_wl_put_peb(struct ubi_device *ubi, int vol_id, int lnum,
		   int pnum, int torture);
int ubi_wl_flush(struct ubi_device *ubi, int vol_id, int lnum);
int ubi_wl_scrub_peb(struct ubi_device *ubi, int pnum);
int ubi_wl_init(struct ubi_device *ubi, struct ubi_attach_info *ai);
void ubi_wl_close(struct ubi_device *ubi);
int ubi_thread(void *u);
struct ubi_wl_entry *ubi_wl_get_fm_peb(struct ubi_device *ubi, int anchor);
int ubi_wl_put_fm_peb(struct ubi_device *ubi, struct ubi_wl_entry *used_e,
		      int lnum, int torture);
int ubi_is_erase_work(struct ubi_work *wrk);
void ubi_refill_pools(struct ubi_device *ubi);
int ubi_ensure_anchor_pebs(struct ubi_device *ubi);

/*      */
int ubi_io_read(const struct ubi_device *ubi, void *buf, int pnum, int offset,
		int len);
int ubi_io_write(struct ubi_device *ubi, const void *buf, int pnum, int offset,
		 int len);
int ubi_io_sync_erase(struct ubi_device *ubi, int pnum, int torture);
int ubi_io_is_bad(const struct ubi_device *ubi, int pnum);
int ubi_io_mark_bad(const struct ubi_device *ubi, int pnum);
int ubi_io_read_ec_hdr(struct ubi_device *ubi, int pnum,
		       struct ubi_ec_hdr *ec_hdr, int verbose);
int ubi_io_write_ec_hdr(struct ubi_device *ubi, int pnum,
			struct ubi_ec_hdr *ec_hdr);
int ubi_io_read_vid_hdr(struct ubi_device *ubi, int pnum,
			struct ubi_vid_hdr *vid_hdr, int verbose);
int ubi_io_write_vid_hdr(struct ubi_device *ubi, int pnum,
			 struct ubi_vid_hdr *vid_hdr);

/*         */
int ubi_attach_mtd_dev(struct mtd_info *mtd, int ubi_num,
		       int vid_hdr_offset, int max_beb_per1024);
int ubi_detach_mtd_dev(int ubi_num, int anyway);
struct ubi_device *ubi_get_device(int ubi_num);
void ubi_put_device(struct ubi_device *ubi);
struct ubi_device *ubi_get_by_major(int major);
int ubi_major2num(int major);
int ubi_volume_notify(struct ubi_device *ubi, struct ubi_volume *vol,
		      int ntype);
int ubi_notify_all(struct ubi_device *ubi, int ntype,
		   struct notifier_block *nb);
int ubi_enumerate_volumes(struct notifier_block *nb);
void ubi_free_internal_volumes(struct ubi_device *ubi);

/*        */
void ubi_do_get_device_info(struct ubi_device *ubi, struct ubi_device_info *di);
void ubi_do_get_volume_info(struct ubi_device *ubi, struct ubi_volume *vol,
			    struct ubi_volume_info *vi);
/*        */
int ubi_compare_lebs(struct ubi_device *ubi, const struct ubi_ainf_peb *aeb,
		      int pnum, const struct ubi_vid_hdr *vid_hdr);

/*           */
size_t ubi_calc_fm_size(struct ubi_device *ubi);
int ubi_update_fastmap(struct ubi_device *ubi);
int ubi_scan_fastmap(struct ubi_device *ubi, struct ubi_attach_info *ai,
		     int fm_anchor);

/*
                                           
                                                                   
                                                                 
                        
                                                                     
 */
#define ubi_rb_for_each_entry(rb, pos, root, member)                         \
	for (rb = rb_first(root),                                            \
	     pos = (rb ? container_of(rb, typeof(*pos), member) : NULL);     \
	     rb;                                                             \
	     rb = rb_next(rb),                                               \
	     pos = (rb ? container_of(rb, typeof(*pos), member) : NULL))

/*
                                                                    
  
                                    
                                         
                             
 */
static inline void ubi_move_aeb_to_list(struct ubi_ainf_volume *av,
					 struct ubi_ainf_peb *aeb,
					 struct list_head *list)
{
		rb_erase(&aeb->u.rb, &av->root);
		list_add_tail(&aeb->u.list, list);
}

/* 
                                                                   
                                      
                                         
  
                                                                         
                                                                          
           
 */
static inline struct ubi_vid_hdr *
ubi_zalloc_vid_hdr(const struct ubi_device *ubi, gfp_t gfp_flags)
{
	void *vid_hdr;

	vid_hdr = kzalloc(ubi->vid_hdr_alsize, gfp_flags);
	if (!vid_hdr)
		return NULL;

	/*
                                                                      
                
  */
	return vid_hdr + ubi->vid_hdr_shift;
}

/* 
                                                             
                                      
                               
 */
static inline void ubi_free_vid_hdr(const struct ubi_device *ubi,
				    struct ubi_vid_hdr *vid_hdr)
{
	void *p = vid_hdr;

	if (!p)
		return;

	kfree(p - ubi->vid_hdr_shift);
}

/*
                                                                             
                                                                       
                       
 */
static inline int ubi_io_read_data(const struct ubi_device *ubi, void *buf,
				   int pnum, int offset, int len)
{
	ubi_assert(offset >= 0);
	return ubi_io_read(ubi, buf, pnum, offset + ubi->leb_start, len);
}

/*
                                                                              
                                                                       
                       
 */
static inline int ubi_io_write_data(struct ubi_device *ubi, const void *buf,
				    int pnum, int offset, int len)
{
	ubi_assert(offset >= 0);
	return ubi_io_write(ubi, buf, pnum, offset + ubi->leb_start, len);
}

/* 
                                          
                                      
 */
static inline void ubi_ro_mode(struct ubi_device *ubi)
{
	if (!ubi->ro_mode) {
		ubi->ro_mode = 1;
		ubi_warn("switch to read-only mode");
		dump_stack();
	}
}

/* 
                                             
                                      
                     
 */
static inline int vol_id2idx(const struct ubi_device *ubi, int vol_id)
{
	if (vol_id >= UBI_INTERNAL_VOL_START)
		return vol_id - UBI_INTERNAL_VOL_START + ubi->vtbl_slots;
	else
		return vol_id;
}

/* 
                                             
                                      
                    
 */
static inline int idx2vol_id(const struct ubi_device *ubi, int idx)
{
	if (idx >= ubi->vtbl_slots)
		return idx - ubi->vtbl_slots + UBI_INTERNAL_VOL_START;
	else
		return idx;
}

#endif /*                */
