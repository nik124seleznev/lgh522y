/*
 * This file is part of UBIFS.
 *
 * Copyright (C) 2006-2008 Nokia Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Author: Adrian Hunter
 */

#include "ubifs.h"

/*
                                                                                
                                                                       
                                                                                
                                                                              
                                                                           
                                                                               
                                                                               
                       
  
                                                                              
                                                                           
                                                                          
                                                                                
                             
  
                                                  
  
                                                              
  
                                                                             
  
                                                                            
                                                                              
                                                                               
                                                                               
                                                                          
                                                                              
                                              
 */

static int dbg_check_orphans(struct ubifs_info *c);

/* 
                                    
                                           
                             
  
                                                                            
        
 */
int ubifs_add_orphan(struct ubifs_info *c, ino_t inum)
{
	struct ubifs_orphan *orphan, *o;
	struct rb_node **p, *parent = NULL;

	orphan = kzalloc(sizeof(struct ubifs_orphan), GFP_NOFS);
	if (!orphan)
		return -ENOMEM;
	orphan->inum = inum;
	orphan->new = 1;

	spin_lock(&c->orphan_lock);
	if (c->tot_orphans >= c->max_orphans) {
		spin_unlock(&c->orphan_lock);
		kfree(orphan);
		return -ENFILE;
	}
	p = &c->orph_tree.rb_node;
	while (*p) {
		parent = *p;
		o = rb_entry(parent, struct ubifs_orphan, rb);
		if (inum < o->inum)
			p = &(*p)->rb_left;
		else if (inum > o->inum)
			p = &(*p)->rb_right;
		else {
			ubifs_err("orphaned twice");
			spin_unlock(&c->orphan_lock);
			kfree(orphan);
			return 0;
		}
	}
	c->tot_orphans += 1;
	c->new_orphans += 1;
	rb_link_node(&orphan->rb, parent, p);
	rb_insert_color(&orphan->rb, &c->orph_tree);
	list_add_tail(&orphan->list, &c->orph_list);
	list_add_tail(&orphan->new_list, &c->orph_new);
	spin_unlock(&c->orphan_lock);
	dbg_gen("ino %lu", (unsigned long)inum);
	return 0;
}

/* 
                                          
                                           
                             
  
                                                                      
 */
void ubifs_delete_orphan(struct ubifs_info *c, ino_t inum)
{
	struct ubifs_orphan *o;
	struct rb_node *p;

	spin_lock(&c->orphan_lock);
	p = c->orph_tree.rb_node;
	while (p) {
		o = rb_entry(p, struct ubifs_orphan, rb);
		if (inum < o->inum)
			p = p->rb_left;
		else if (inum > o->inum)
			p = p->rb_right;
		else {
			if (o->del) {
				spin_unlock(&c->orphan_lock);
				dbg_gen("deleted twice ino %lu",
					(unsigned long)inum);
				return;
			}
			if (o->cmt) {
				o->del = 1;
				o->dnext = c->orph_dnext;
				c->orph_dnext = o;
				spin_unlock(&c->orphan_lock);
				dbg_gen("delete later ino %lu",
					(unsigned long)inum);
				return;
			}
			rb_erase(p, &c->orph_tree);
			list_del(&o->list);
			c->tot_orphans -= 1;
			if (o->new) {
				list_del(&o->new_list);
				c->new_orphans -= 1;
			}
			spin_unlock(&c->orphan_lock);
			kfree(o);
			dbg_gen("inum %lu", (unsigned long)inum);
			return;
		}
	}
	spin_unlock(&c->orphan_lock);
	ubifs_err("missing orphan ino %lu", (unsigned long)inum);
	dump_stack();
}

/* 
                                                       
                                           
  
                           
 */
int ubifs_orphan_start_commit(struct ubifs_info *c)
{
	struct ubifs_orphan *orphan, **last;

	spin_lock(&c->orphan_lock);
	last = &c->orph_cnext;
	list_for_each_entry(orphan, &c->orph_new, new_list) {
		ubifs_assert(orphan->new);
		ubifs_assert(!orphan->cmt);
		orphan->new = 0;
		orphan->cmt = 1;
		*last = orphan;
		last = &orphan->cnext;
	}
	*last = NULL;
	c->cmt_orphans = c->new_orphans;
	c->new_orphans = 0;
	dbg_cmt("%d orphans to commit", c->cmt_orphans);
	INIT_LIST_HEAD(&c->orph_new);
	if (c->tot_orphans == 0)
		c->no_orphs = 1;
	else
		c->no_orphs = 0;
	spin_unlock(&c->orphan_lock);
	return 0;
}

/* 
                                           
                                           
  
                                                                         
                   
 */
static int avail_orphs(struct ubifs_info *c)
{
	int avail_lebs, avail, gap;

	avail_lebs = c->orph_lebs - (c->ohead_lnum - c->orph_first) - 1;
	avail = avail_lebs *
	       ((c->leb_size - UBIFS_ORPH_NODE_SZ) / sizeof(__le64));
	gap = c->leb_size - c->ohead_offs;
	if (gap >= UBIFS_ORPH_NODE_SZ + sizeof(__le64))
		avail += (gap - UBIFS_ORPH_NODE_SZ) / sizeof(__le64);
	return avail;
}

/* 
                                           
                                           
  
                                                                          
                                                                      
 */
static int tot_avail_orphs(struct ubifs_info *c)
{
	int avail_lebs, avail;

	avail_lebs = c->orph_lebs;
	avail = avail_lebs *
	       ((c->leb_size - UBIFS_ORPH_NODE_SZ) / sizeof(__le64));
	return avail / 2;
}

/* 
                                                        
                                           
                       
                            
  
                                                                            
                                                                            
                                                         
 */
static int do_write_orph_node(struct ubifs_info *c, int len, int atomic)
{
	int err = 0;

	if (atomic) {
		ubifs_assert(c->ohead_offs == 0);
		ubifs_prepare_node(c, c->orph_buf, len, 1);
		len = ALIGN(len, c->min_io_size);
		err = ubifs_leb_change(c, c->ohead_lnum, c->orph_buf, len);
	} else {
		if (c->ohead_offs == 0) {
			/*                              */
			err = ubifs_leb_unmap(c, c->ohead_lnum);
			if (err)
				return err;
		}
		err = ubifs_write_node(c, c->orph_buf, len, c->ohead_lnum,
				       c->ohead_offs);
	}
	return err;
}

/* 
                                          
                                           
                            
  
                                                                               
                                                                           
               
 */
static int write_orph_node(struct ubifs_info *c, int atomic)
{
	struct ubifs_orphan *orphan, *cnext;
	struct ubifs_orph_node *orph;
	int gap, err, len, cnt, i;

	ubifs_assert(c->cmt_orphans > 0);
	gap = c->leb_size - c->ohead_offs;
	if (gap < UBIFS_ORPH_NODE_SZ + sizeof(__le64)) {
		c->ohead_lnum += 1;
		c->ohead_offs = 0;
		gap = c->leb_size;
		if (c->ohead_lnum > c->orph_last) {
			/*
                                                        
                   
    */
			ubifs_err("out of space in orphan area");
			return -EINVAL;
		}
	}
	cnt = (gap - UBIFS_ORPH_NODE_SZ) / sizeof(__le64);
	if (cnt > c->cmt_orphans)
		cnt = c->cmt_orphans;
	len = UBIFS_ORPH_NODE_SZ + cnt * sizeof(__le64);
	ubifs_assert(c->orph_buf);
	orph = c->orph_buf;
	orph->ch.node_type = UBIFS_ORPH_NODE;
	spin_lock(&c->orphan_lock);
	cnext = c->orph_cnext;
	for (i = 0; i < cnt; i++) {
		orphan = cnext;
		ubifs_assert(orphan->cmt);
		orph->inos[i] = cpu_to_le64(orphan->inum);
		orphan->cmt = 0;
		cnext = orphan->cnext;
		orphan->cnext = NULL;
	}
	c->orph_cnext = cnext;
	c->cmt_orphans -= cnt;
	spin_unlock(&c->orphan_lock);
	if (c->cmt_orphans)
		orph->cmt_no = cpu_to_le64(c->cmt_no);
	else
		/*                                  */
		orph->cmt_no = cpu_to_le64((c->cmt_no) | (1ULL << 63));
	ubifs_assert(c->ohead_offs + len <= c->leb_size);
	ubifs_assert(c->ohead_lnum >= c->orph_first);
	ubifs_assert(c->ohead_lnum <= c->orph_last);
	err = do_write_orph_node(c, len, atomic);
	c->ohead_offs += ALIGN(len, c->min_io_size);
	c->ohead_offs = ALIGN(c->ohead_offs, 8);
	return err;
}

/* 
                                                                           
                                           
                            
  
                                                                               
                                                               
 */
static int write_orph_nodes(struct ubifs_info *c, int atomic)
{
	int err;

	while (c->cmt_orphans > 0) {
		err = write_orph_node(c, atomic);
		if (err)
			return err;
	}
	if (atomic) {
		int lnum;

		/*                                           */
		lnum = c->ohead_lnum + 1;
		for (lnum = c->ohead_lnum + 1; lnum <= c->orph_last; lnum++) {
			err = ubifs_leb_unmap(c, lnum);
			if (err)
				return err;
		}
	}
	return 0;
}

/* 
                                             
                                           
  
                                                                               
                                                                           
                                                                             
                      
  
                                                                            
 */
static int consolidate(struct ubifs_info *c)
{
	int tot_avail = tot_avail_orphs(c), err = 0;

	spin_lock(&c->orphan_lock);
	dbg_cmt("there is space for %d orphans and there are %d",
		tot_avail, c->tot_orphans);
	if (c->tot_orphans - c->new_orphans <= tot_avail) {
		struct ubifs_orphan *orphan, **last;
		int cnt = 0;

		/*                                                      */
		last = &c->orph_cnext;
		list_for_each_entry(orphan, &c->orph_list, list) {
			if (orphan->new)
				continue;
			orphan->cmt = 1;
			*last = orphan;
			last = &orphan->cnext;
			cnt += 1;
		}
		*last = NULL;
		ubifs_assert(cnt == c->tot_orphans - c->new_orphans);
		c->cmt_orphans = cnt;
		c->ohead_lnum = c->orph_first;
		c->ohead_offs = 0;
	} else {
		/*
                                                       
                  
   */
		ubifs_err("out of space in orphan area");
		err = -EINVAL;
	}
	spin_unlock(&c->orphan_lock);
	return err;
}

/* 
                                   
                                           
  
                                                                      
                                               
 */
static int commit_orphans(struct ubifs_info *c)
{
	int avail, atomic = 0, err;

	ubifs_assert(c->cmt_orphans > 0);
	avail = avail_orphs(c);
	if (avail < c->cmt_orphans) {
		/*                                                       */
		err = consolidate(c);
		if (err)
			return err;
		atomic = 1;
	}
	err = write_orph_nodes(c, atomic);
	return err;
}

/* 
                                                         
                                           
  
                                                                            
                                                                       
                                                                           
            
 */
static void erase_deleted(struct ubifs_info *c)
{
	struct ubifs_orphan *orphan, *dnext;

	spin_lock(&c->orphan_lock);
	dnext = c->orph_dnext;
	while (dnext) {
		orphan = dnext;
		dnext = orphan->dnext;
		ubifs_assert(!orphan->new);
		ubifs_assert(orphan->del);
		rb_erase(&orphan->rb, &c->orph_tree);
		list_del(&orphan->list);
		c->tot_orphans -= 1;
		dbg_gen("deleting orphan ino %lu", (unsigned long)orphan->inum);
		kfree(orphan);
	}
	c->orph_dnext = NULL;
	spin_unlock(&c->orphan_lock);
}

/* 
                                                   
                                           
  
                         
 */
int ubifs_orphan_end_commit(struct ubifs_info *c)
{
	int err;

	if (c->cmt_orphans != 0) {
		err = commit_orphans(c);
		if (err)
			return err;
	}
	erase_deleted(c);
	err = dbg_check_orphans(c);
	return err;
}

/* 
                                                         
                                           
  
                                                                          
                                                                
                             
 */
int ubifs_clear_orphans(struct ubifs_info *c)
{
	int lnum, err;

	for (lnum = c->orph_first; lnum <= c->orph_last; lnum++) {
		err = ubifs_leb_unmap(c, lnum);
		if (err)
			return err;
	}
	c->ohead_lnum = c->orph_first;
	c->ohead_offs = 0;
	return 0;
}

/* 
                                         
                                           
                             
  
                                                                            
                                                                            
                 
 */
static int insert_dead_orphan(struct ubifs_info *c, ino_t inum)
{
	struct ubifs_orphan *orphan, *o;
	struct rb_node **p, *parent = NULL;

	orphan = kzalloc(sizeof(struct ubifs_orphan), GFP_KERNEL);
	if (!orphan)
		return -ENOMEM;
	orphan->inum = inum;

	p = &c->orph_tree.rb_node;
	while (*p) {
		parent = *p;
		o = rb_entry(parent, struct ubifs_orphan, rb);
		if (inum < o->inum)
			p = &(*p)->rb_left;
		else if (inum > o->inum)
			p = &(*p)->rb_right;
		else {
			/*                            */
			kfree(orphan);
			return 0;
		}
	}
	c->tot_orphans += 1;
	rb_link_node(&orphan->rb, parent, p);
	rb_insert_color(&orphan->rb, &c->orph_tree);
	list_add_tail(&orphan->list, &c->orph_list);
	orphan->del = 1;
	orphan->dnext = c->orph_dnext;
	c->orph_dnext = orphan;
	dbg_mnt("ino %lu, new %d, tot %d", (unsigned long)inum,
		c->new_orphans, c->tot_orphans);
	return 0;
}

/* 
                                                         
                                           
                     
                                                                            
                                                              
                                                            
  
                                                                              
                                                                          
                                        
 */
static int do_kill_orphans(struct ubifs_info *c, struct ubifs_scan_leb *sleb,
			   unsigned long long *last_cmt_no, int *outofdate,
			   int *last_flagged)
{
	struct ubifs_scan_node *snod;
	struct ubifs_orph_node *orph;
	unsigned long long cmt_no;
	ino_t inum;
	int i, n, err, first = 1;

	list_for_each_entry(snod, &sleb->nodes, list) {
		if (snod->type != UBIFS_ORPH_NODE) {
			ubifs_err("invalid node type %d in orphan area at %d:%d",
				  snod->type, sleb->lnum, snod->offs);
			ubifs_dump_node(c, snod->node);
			return -EINVAL;
		}

		orph = snod->node;

		/*                     */
		cmt_no = le64_to_cpu(orph->cmt_no) & LLONG_MAX;
		/*
                                                              
                                                                 
                                                                 
                                                                  
                                                               
                                                     
   */
		if (cmt_no > c->cmt_no)
			c->cmt_no = cmt_no;
		if (cmt_no < *last_cmt_no && *last_flagged) {
			/*
                                                         
                                                     
                                                       
    */
			if (!first) {
				ubifs_err("out of order commit number %llu in orphan node at %d:%d",
					  cmt_no, sleb->lnum, snod->offs);
				ubifs_dump_node(c, snod->node);
				return -EINVAL;
			}
			dbg_rcvry("out of date LEB %d", sleb->lnum);
			*outofdate = 1;
			return 0;
		}

		if (first)
			first = 0;

		n = (le32_to_cpu(orph->ch.len) - UBIFS_ORPH_NODE_SZ) >> 3;
		for (i = 0; i < n; i++) {
			inum = le64_to_cpu(orph->inos[i]);
			dbg_rcvry("deleting orphaned inode %lu",
				  (unsigned long)inum);
			err = ubifs_tnc_remove_ino(c, inum);
			if (err)
				return err;
			err = insert_dead_orphan(c, inum);
			if (err)
				return err;
		}

		*last_cmt_no = cmt_no;
		if (le64_to_cpu(orph->cmt_no) & (1ULL << 63)) {
			dbg_rcvry("last orph node for commit %llu at %d:%d",
				  cmt_no, sleb->lnum, snod->offs);
			*last_flagged = 1;
		} else
			*last_flagged = 0;
	}

	return 0;
}

/* 
                                                          
                                           
  
                                                                           
                                                                                
                                                                             
                                                                             
                                
 */
static int kill_orphans(struct ubifs_info *c)
{
	unsigned long long last_cmt_no = 0;
	int lnum, err = 0, outofdate = 0, last_flagged = 0;

	c->ohead_lnum = c->orph_first;
	c->ohead_offs = 0;
	/*                                                   */
	if (c->no_orphs) {
		dbg_rcvry("no orphans");
		return 0;
	}
	/*
                                                                    
                                                                         
                                                                   
                                                                         
                                                                     
                                                                        
                                                                        
                                                                       
                                                  
  */
	for (lnum = c->orph_first; lnum <= c->orph_last; lnum++) {
		struct ubifs_scan_leb *sleb;

		dbg_rcvry("LEB %d", lnum);
		sleb = ubifs_scan(c, lnum, 0, c->sbuf, 1);
		if (IS_ERR(sleb)) {
			if (PTR_ERR(sleb) == -EUCLEAN)
				sleb = ubifs_recover_leb(c, lnum, 0,
							 c->sbuf, -1);
			if (IS_ERR(sleb)) {
				err = PTR_ERR(sleb);
				break;
			}
		}
		err = do_kill_orphans(c, sleb, &last_cmt_no, &outofdate,
				      &last_flagged);
		if (err || outofdate) {
			ubifs_scan_destroy(sleb);
			break;
		}
		if (sleb->endpt) {
			c->ohead_lnum = lnum;
			c->ohead_offs = sleb->endpt;
		}
		ubifs_scan_destroy(sleb);
	}
	return err;
}

/* 
                                                                                
                                           
                                                    
                                        
  
                                                                           
                                                                           
                       
 */
int ubifs_mount_orphans(struct ubifs_info *c, int unclean, int read_only)
{
	int err = 0;

	c->max_orphans = tot_avail_orphs(c);

	if (!read_only) {
		c->orph_buf = vmalloc(c->leb_size);
		if (!c->orph_buf)
			return -ENOMEM;
	}

	if (unclean)
		err = kill_orphans(c);
	else if (!read_only)
		err = ubifs_clear_orphans(c);

	return err;
}

/*
                                            
 */

struct check_orphan {
	struct rb_node rb;
	ino_t inum;
};

struct check_info {
	unsigned long last_ino;
	unsigned long tot_inos;
	unsigned long missing;
	unsigned long long leaf_cnt;
	struct ubifs_ino_node *node;
	struct rb_root root;
};

static int dbg_find_orphan(struct ubifs_info *c, ino_t inum)
{
	struct ubifs_orphan *o;
	struct rb_node *p;

	spin_lock(&c->orphan_lock);
	p = c->orph_tree.rb_node;
	while (p) {
		o = rb_entry(p, struct ubifs_orphan, rb);
		if (inum < o->inum)
			p = p->rb_left;
		else if (inum > o->inum)
			p = p->rb_right;
		else {
			spin_unlock(&c->orphan_lock);
			return 1;
		}
	}
	spin_unlock(&c->orphan_lock);
	return 0;
}

static int dbg_ins_check_orphan(struct rb_root *root, ino_t inum)
{
	struct check_orphan *orphan, *o;
	struct rb_node **p, *parent = NULL;

	orphan = kzalloc(sizeof(struct check_orphan), GFP_NOFS);
	if (!orphan)
		return -ENOMEM;
	orphan->inum = inum;

	p = &root->rb_node;
	while (*p) {
		parent = *p;
		o = rb_entry(parent, struct check_orphan, rb);
		if (inum < o->inum)
			p = &(*p)->rb_left;
		else if (inum > o->inum)
			p = &(*p)->rb_right;
		else {
			kfree(orphan);
			return 0;
		}
	}
	rb_link_node(&orphan->rb, parent, p);
	rb_insert_color(&orphan->rb, root);
	return 0;
}

static int dbg_find_check_orphan(struct rb_root *root, ino_t inum)
{
	struct check_orphan *o;
	struct rb_node *p;

	p = root->rb_node;
	while (p) {
		o = rb_entry(p, struct check_orphan, rb);
		if (inum < o->inum)
			p = p->rb_left;
		else if (inum > o->inum)
			p = p->rb_right;
		else
			return 1;
	}
	return 0;
}

static void dbg_free_check_tree(struct rb_root *root)
{
	struct rb_node *this = root->rb_node;
	struct check_orphan *o;

	while (this) {
		if (this->rb_left) {
			this = this->rb_left;
			continue;
		} else if (this->rb_right) {
			this = this->rb_right;
			continue;
		}
		o = rb_entry(this, struct check_orphan, rb);
		this = rb_parent(this);
		if (this) {
			if (this->rb_left == &o->rb)
				this->rb_left = NULL;
			else
				this->rb_right = NULL;
		}
		kfree(o);
	}
}

static int dbg_orphan_check(struct ubifs_info *c, struct ubifs_zbranch *zbr,
			    void *priv)
{
	struct check_info *ci = priv;
	ino_t inum;
	int err;

	inum = key_inum(c, &zbr->key);
	if (inum != ci->last_ino) {
		/*                                                       */
		if (key_type(c, &zbr->key) != UBIFS_INO_KEY)
			ubifs_err("found orphan node ino %lu, type %d",
				  (unsigned long)inum, key_type(c, &zbr->key));
		ci->last_ino = inum;
		ci->tot_inos += 1;
		err = ubifs_tnc_read_node(c, zbr, ci->node);
		if (err) {
			ubifs_err("node read failed, error %d", err);
			return err;
		}
		if (ci->node->nlink == 0)
			/*                               */
			if (!dbg_find_check_orphan(&ci->root, inum) &&
			    !dbg_find_orphan(c, inum)) {
				ubifs_err("missing orphan, ino %lu",
					  (unsigned long)inum);
				ci->missing += 1;
			}
	}
	ci->leaf_cnt += 1;
	return 0;
}

static int dbg_read_orphans(struct check_info *ci, struct ubifs_scan_leb *sleb)
{
	struct ubifs_scan_node *snod;
	struct ubifs_orph_node *orph;
	ino_t inum;
	int i, n, err;

	list_for_each_entry(snod, &sleb->nodes, list) {
		cond_resched();
		if (snod->type != UBIFS_ORPH_NODE)
			continue;
		orph = snod->node;
		n = (le32_to_cpu(orph->ch.len) - UBIFS_ORPH_NODE_SZ) >> 3;
		for (i = 0; i < n; i++) {
			inum = le64_to_cpu(orph->inos[i]);
			err = dbg_ins_check_orphan(&ci->root, inum);
			if (err)
				return err;
		}
	}
	return 0;
}

static int dbg_scan_orphans(struct ubifs_info *c, struct check_info *ci)
{
	int lnum, err = 0;
	void *buf;

	/*                                                   */
	if (c->no_orphs)
		return 0;

	buf = __vmalloc(c->leb_size, GFP_NOFS, PAGE_KERNEL);
	if (!buf) {
		ubifs_err("cannot allocate memory to check orphans");
		return 0;
	}

	for (lnum = c->orph_first; lnum <= c->orph_last; lnum++) {
		struct ubifs_scan_leb *sleb;

		sleb = ubifs_scan(c, lnum, 0, buf, 0);
		if (IS_ERR(sleb)) {
			err = PTR_ERR(sleb);
			break;
		}

		err = dbg_read_orphans(ci, sleb);
		ubifs_scan_destroy(sleb);
		if (err)
			break;
	}

	vfree(buf);
	return err;
}

static int dbg_check_orphans(struct ubifs_info *c)
{
	struct check_info ci;
	int err;

	if (!dbg_is_chk_orph(c))
		return 0;

	ci.last_ino = 0;
	ci.tot_inos = 0;
	ci.missing  = 0;
	ci.leaf_cnt = 0;
	ci.root = RB_ROOT;
	ci.node = kmalloc(UBIFS_MAX_INO_NODE_SZ, GFP_NOFS);
	if (!ci.node) {
		ubifs_err("out of memory");
		return -ENOMEM;
	}

	err = dbg_scan_orphans(c, &ci);
	if (err)
		goto out;

	err = dbg_walk_index(c, &dbg_orphan_check, NULL, &ci);
	if (err) {
		ubifs_err("cannot scan TNC, error %d", err);
		goto out;
	}

	if (ci.missing) {
		ubifs_err("%lu missing orphan(s)", ci.missing);
		err = -EINVAL;
		goto out;
	}

	dbg_cmt("last inode number is %lu", ci.last_ino);
	dbg_cmt("total number of inodes is %lu", ci.tot_inos);
	dbg_cmt("total number of leaf nodes is %llu", ci.leaf_cnt);

out:
	dbg_free_check_tree(&ci.root);
	kfree(ci.node);
	return err;
}
