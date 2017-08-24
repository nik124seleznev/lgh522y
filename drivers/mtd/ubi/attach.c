/*
 * Copyright (c) International Business Machines Corp., 2006
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

/*
                            
  
                                                                       
                                   
  
                                                                         
                                                                              
                                                                            
                                           
  
                                                                             
                                                                             
                                                                               
                                                                         
                   
  
                                                                          
                                                                          
                                     
  
                    
                    
  
                                                                          
                                                                            
                                                                               
                                                                  
  
                                                           
  
                                                                              
                                                                          
                                                                          
                                                                               
                                                                        
                                                                       
  
                                                                              
                                                                           
                                                           
  
                                                                       
                                                                         
                                                                              
                                                                              
                                                         
  
                                                                          
                                                                        
                                                                              
                                                                           
                                                                             
                                                                           
                  
                                                                              
                                                          
                                                                             
                                                                        
                                                                             
                            
                                           
 */

#include <linux/err.h>
#include <linux/slab.h>
#include <linux/crc32.h>
#include <linux/math64.h>
#include <linux/random.h>
#include "ubi.h"

static int self_check_ai(struct ubi_device *ubi, struct ubi_attach_info *ai);

/*                                          */
static struct ubi_ec_hdr *ech;
static struct ubi_vid_hdr *vidh;

/* 
                                                   
                             
                                           
                                               
                                              
                                                
                                                     
                            
  
                                                                      
                                                                         
                                                               
                                                                         
                                                                            
                                                                           
                                                                           
                                                                          
                                                                       
           
 */
static int add_to_list(struct ubi_attach_info *ai, int pnum, int vol_id,
		       int lnum, int ec, int to_head, struct list_head *list)
{
	struct ubi_ainf_peb *aeb;

	if (list == &ai->free) {
		dbg_bld("add to free: PEB %d, EC %d", pnum, ec);
	} else if (list == &ai->erase) {
		dbg_bld("add to erase: PEB %d, EC %d", pnum, ec);
	} else if (list == &ai->alien) {
		dbg_bld("add to alien: PEB %d, EC %d", pnum, ec);
		ai->alien_peb_count += 1;
	} else
		BUG();

	aeb = kmem_cache_alloc(ai->aeb_slab_cache, GFP_KERNEL);
	if (!aeb)
		return -ENOMEM;

	aeb->pnum = pnum;
	aeb->vol_id = vol_id;
	aeb->lnum = lnum;
	aeb->ec = ec;
	if (to_head)
		list_add(&aeb->u.list, list);
	else
		list_add_tail(&aeb->u.list, list);
	return 0;
}

/* 
                                                       
                             
                                           
                                                
  
                                                                         
                                                                            
                                                                            
                                                
 */
static int add_corrupted(struct ubi_attach_info *ai, int pnum, int ec)
{
	struct ubi_ainf_peb *aeb;

	dbg_bld("add to corrupted: PEB %d, EC %d", pnum, ec);

	aeb = kmem_cache_alloc(ai->aeb_slab_cache, GFP_KERNEL);
	if (!aeb)
		return -ENOMEM;

	ai->corr_peb_count += 1;
	aeb->pnum = pnum;
	aeb->ec = ec;
	list_add(&aeb->u.list, &ai->corr);
	return 0;
}

/* 
                                                     
                                                  
                                                                       
                                                             
  
                                                                           
                                                          
  
                                                                           
                                                                            
                                                                              
                              
 */
static int validate_vid_hdr(const struct ubi_vid_hdr *vid_hdr,
			    const struct ubi_ainf_volume *av, int pnum)
{
	int vol_type = vid_hdr->vol_type;
	int vol_id = be32_to_cpu(vid_hdr->vol_id);
	int used_ebs = be32_to_cpu(vid_hdr->used_ebs);
	int data_pad = be32_to_cpu(vid_hdr->data_pad);

	if (av->leb_count != 0) {
		int av_vol_type;

		/*
                                                               
                                                                 
                                                        
   */

		if (vol_id != av->vol_id) {
			ubi_err("inconsistent vol_id");
			goto bad;
		}

		if (av->vol_type == UBI_STATIC_VOLUME)
			av_vol_type = UBI_VID_STATIC;
		else
			av_vol_type = UBI_VID_DYNAMIC;

		if (vol_type != av_vol_type) {
			ubi_err("inconsistent vol_type");
			goto bad;
		}

		if (used_ebs != av->used_ebs) {
			ubi_err("inconsistent used_ebs");
			goto bad;
		}

		if (data_pad != av->data_pad) {
			ubi_err("inconsistent data_pad");
			goto bad;
		}
	}

	return 0;

bad:
	ubi_err("inconsistent VID header at PEB %d", pnum);
	ubi_dump_vid_hdr(vid_hdr);
	ubi_dump_av(av);
	return -EINVAL;
}

/* 
                                                        
                             
                                   
                                    
                                     
  
                                                                            
                                                                              
                                                                               
                                                                               
                   
 */
static struct ubi_ainf_volume *add_volume(struct ubi_attach_info *ai,
					  int vol_id, int pnum,
					  const struct ubi_vid_hdr *vid_hdr)
{
	struct ubi_ainf_volume *av;
	struct rb_node **p = &ai->volumes.rb_node, *parent = NULL;

	ubi_assert(vol_id == be32_to_cpu(vid_hdr->vol_id));

	/*                                                                   */
	while (*p) {
		parent = *p;
		av = rb_entry(parent, struct ubi_ainf_volume, rb);

		if (vol_id == av->vol_id)
			return av;

		if (vol_id > av->vol_id)
			p = &(*p)->rb_left;
		else
			p = &(*p)->rb_right;
	}

	/*                               */
	av = kmalloc(sizeof(struct ubi_ainf_volume), GFP_KERNEL);
	if (!av)
		return ERR_PTR(-ENOMEM);

	av->highest_lnum = av->leb_count = 0;
	av->vol_id = vol_id;
	av->root = RB_ROOT;
	av->used_ebs = be32_to_cpu(vid_hdr->used_ebs);
	av->data_pad = be32_to_cpu(vid_hdr->data_pad);
	av->compat = vid_hdr->compat;
	av->vol_type = vid_hdr->vol_type == UBI_VID_DYNAMIC ? UBI_DYNAMIC_VOLUME
							    : UBI_STATIC_VOLUME;
	if (vol_id > ai->highest_vol_id)
		ai->highest_vol_id = vol_id;

	rb_link_node(&av->rb, parent, p);
	rb_insert_color(&av->rb, &ai->volumes);
	ai->vols_found += 1;
	dbg_bld("added volume %d", vol_id);
	return av;
}

/* 
                                                                 
                                      
                                            
                                                                        
          
                                                                      
  
                                                                              
                                                                                
                                                                              
        
                                                                              
                                                      
                                               
                                                                       
                                                                
                                                          
                                                  
 */
int ubi_compare_lebs(struct ubi_device *ubi, const struct ubi_ainf_peb *aeb,
			int pnum, const struct ubi_vid_hdr *vid_hdr)
{
	int len, err, second_is_newer, bitflips = 0, corrupted = 0;
	uint32_t data_crc, crc;
	struct ubi_vid_hdr *vh = NULL;
	unsigned long long sqnum2 = be64_to_cpu(vid_hdr->sqnum);

	if (sqnum2 == aeb->sqnum) {
		/*
                                                           
                                                               
                                                             
                                                                  
                                                                 
                                             
   */
		ubi_err("unsupported on-flash UBI format");
		return -EINVAL;
	}

	/*                                                        */
	second_is_newer = (sqnum2 > aeb->sqnum);

	/*
                                                                     
                                                                       
                                                                      
                                                            
   
                                                               
  */

	if (second_is_newer) {
		if (!vid_hdr->copy_flag) {
			/*                                  */
			dbg_bld("second PEB %d is newer, copy_flag is unset",
				pnum);
			return 1;
		}
	} else {
		if (!aeb->copy_flag) {
			/*                                  */
			dbg_bld("first PEB %d is newer, copy_flag is unset",
				pnum);
			return bitflips << 1;
		}

		vh = ubi_zalloc_vid_hdr(ubi, GFP_KERNEL);
		if (!vh)
			return -ENOMEM;

		pnum = aeb->pnum;
		err = ubi_io_read_vid_hdr(ubi, pnum, vh, 0);
		if (err) {
			if (err == UBI_IO_BITFLIPS)
				bitflips = 1;
			else {
				ubi_err("VID of PEB %d header is bad, but it was OK earlier, err %d",
					pnum, err);
				if (err > 0)
					err = -EIO;

				goto out_free_vidh;
			}
		}

		vid_hdr = vh;
	}

	/*                                             */

	len = be32_to_cpu(vid_hdr->data_size);

	mutex_lock(&ubi->buf_mutex);
	err = ubi_io_read_data(ubi, ubi->peb_buf, pnum, 0, len);
	if (err && err != UBI_IO_BITFLIPS && !mtd_is_eccerr(err))
		goto out_unlock;

	data_crc = be32_to_cpu(vid_hdr->data_crc);
	crc = crc32(UBI_CRC32_INIT, ubi->peb_buf, len);
	if (crc != data_crc) {
		dbg_bld("PEB %d CRC error: calculated %#08x, must be %#08x",
			pnum, crc, data_crc);
		corrupted = 1;
		bitflips = 0;
		second_is_newer = !second_is_newer;
	} else {
		dbg_bld("PEB %d CRC is OK", pnum);
		bitflips = !!err;
	}
	mutex_unlock(&ubi->buf_mutex);

	ubi_free_vid_hdr(ubi, vh);

	if (second_is_newer)
		dbg_bld("second PEB %d is newer, copy_flag is set", pnum);
	else
		dbg_bld("first PEB %d is newer, copy_flag is set", pnum);

	return second_is_newer | (bitflips << 1) | (corrupted << 2);

out_unlock:
	mutex_unlock(&ubi->buf_mutex);
out_free_vidh:
	ubi_free_vid_hdr(ubi, vh);
	return err;
}

/* 
                                                                             
                                      
                             
                                        
                     
                                         
                                                                               
  
                                                                         
                                                                          
                                                                     
                                                                             
                                                                             
                                                                        
 */
int ubi_add_to_av(struct ubi_device *ubi, struct ubi_attach_info *ai, int pnum,
		  int ec, const struct ubi_vid_hdr *vid_hdr, int bitflips)
{
	int err, vol_id, lnum;
	unsigned long long sqnum;
	struct ubi_ainf_volume *av;
	struct ubi_ainf_peb *aeb;
	struct rb_node **p, *parent = NULL;

	vol_id = be32_to_cpu(vid_hdr->vol_id);
	lnum = be32_to_cpu(vid_hdr->lnum);
	sqnum = be64_to_cpu(vid_hdr->sqnum);

	dbg_bld("PEB %d, LEB %d:%d, EC %d, sqnum %llu, bitflips %d",
		pnum, vol_id, lnum, ec, sqnum, bitflips);

	av = add_volume(ai, vol_id, pnum, vid_hdr);
	if (IS_ERR(av))
		return PTR_ERR(av);

	if (ai->max_sqnum < sqnum)
		ai->max_sqnum = sqnum;

	/*
                                                                     
                                                                    
  */
	p = &av->root.rb_node;
	while (*p) {
		int cmp_res;

		parent = *p;
		aeb = rb_entry(parent, struct ubi_ainf_peb, u.rb);
		if (lnum != aeb->lnum) {
			if (lnum < aeb->lnum)
				p = &(*p)->rb_left;
			else
				p = &(*p)->rb_right;
			continue;
		}

		/*
                                                               
                                
   */

		dbg_bld("this LEB already exists: PEB %d, sqnum %llu, EC %d",
			aeb->pnum, aeb->sqnum, aeb->ec);

		/*
                                                          
                                                  
    
                                                               
                                                               
                                                               
                                                       
                                                         
                                                              
                                                            
                                                             
   */
		if (aeb->sqnum == sqnum && sqnum != 0) {
			ubi_err("two LEBs with same sequence number %llu",
				sqnum);
			ubi_dump_aeb(aeb, 0);
			ubi_dump_vid_hdr(vid_hdr);
			return -EINVAL;
		}

		/*
                                                             
         
   */
		cmp_res = ubi_compare_lebs(ubi, aeb, pnum, vid_hdr);
		if (cmp_res < 0)
			return cmp_res;

		if (cmp_res & 1) {
			/*
                                                   
                    
    */
			err = validate_vid_hdr(vid_hdr, av, pnum);
			if (err)
				return err;

			err = add_to_list(ai, aeb->pnum, aeb->vol_id,
					  aeb->lnum, aeb->ec, cmp_res & 4,
					  &ai->erase);
			if (err)
				return err;

			aeb->ec = ec;
			aeb->pnum = pnum;
			aeb->vol_id = vol_id;
			aeb->lnum = lnum;
			aeb->scrub = ((cmp_res & 2) || bitflips);
			aeb->copy_flag = vid_hdr->copy_flag;
			aeb->sqnum = sqnum;

			if (av->highest_lnum == lnum)
				av->last_data_size =
					be32_to_cpu(vid_hdr->data_size);

			return 0;
		} else {
			/*
                                                         
                 
    */
			return add_to_list(ai, pnum, vol_id, lnum, ec,
					   cmp_res & 4, &ai->erase);
		}
	}

	/*
                                                                       
                          
  */

	err = validate_vid_hdr(vid_hdr, av, pnum);
	if (err)
		return err;

	aeb = kmem_cache_alloc(ai->aeb_slab_cache, GFP_KERNEL);
	if (!aeb)
		return -ENOMEM;

	aeb->ec = ec;
	aeb->pnum = pnum;
	aeb->vol_id = vol_id;
	aeb->lnum = lnum;
	aeb->scrub = bitflips;
	aeb->copy_flag = vid_hdr->copy_flag;
	aeb->sqnum = sqnum;

	if (av->highest_lnum <= lnum) {
		av->highest_lnum = lnum;
		av->last_data_size = be32_to_cpu(vid_hdr->data_size);
	}

	av->leb_count += 1;
	rb_link_node(&aeb->u.rb, parent, p);
	rb_insert_color(&aeb->u.rb, &av->root);
	return 0;
}

/* 
                                                          
                             
                                   
  
                                                                              
                                                              
 */
struct ubi_ainf_volume *ubi_find_av(const struct ubi_attach_info *ai,
				    int vol_id)
{
	struct ubi_ainf_volume *av;
	struct rb_node *p = ai->volumes.rb_node;

	while (p) {
		av = rb_entry(p, struct ubi_ainf_volume, rb);

		if (vol_id == av->vol_id)
			return av;

		if (vol_id > av->vol_id)
			p = p->rb_left;
		else
			p = p->rb_right;
	}

	return NULL;
}

/* 
                                                               
                             
                                                  
 */
void ubi_remove_av(struct ubi_attach_info *ai, struct ubi_ainf_volume *av)
{
	struct rb_node *rb;
	struct ubi_ainf_peb *aeb;

	dbg_bld("remove attaching information about volume %d", av->vol_id);

	while ((rb = rb_first(&av->root))) {
		aeb = rb_entry(rb, struct ubi_ainf_peb, u.rb);
		rb_erase(&aeb->u.rb, &av->root);
		list_add_tail(&aeb->u.list, &ai->erase);
	}

	rb_erase(&av->rb, &ai->volumes);
	kfree(av);
	ai->vols_found -= 1;
}

/* 
                                                 
                                      
                             
                                              
                                                                    
  
                                                                        
                                                                        
                                                                               
                                                                             
                   
 */
static int early_erase_peb(struct ubi_device *ubi,
			   const struct ubi_attach_info *ai, int pnum, int ec)
{
	int err;
	struct ubi_ec_hdr *ec_hdr;

	if ((long long)ec >= UBI_MAX_ERASECOUNTER) {
		/*
                                                       
                               
   */
		ubi_err("erase counter overflow at PEB %d, EC %d", pnum, ec);
		return -EINVAL;
	}

	ec_hdr = kzalloc(ubi->ec_hdr_alsize, GFP_KERNEL);
	if (!ec_hdr)
		return -ENOMEM;

	ec_hdr->ec = cpu_to_be64(ec);

	err = ubi_io_sync_erase(ubi, pnum, 0);
	if (err < 0)
		goto out_free;

	err = ubi_io_write_ec_hdr(ubi, pnum, ec_hdr);

out_free:
	kfree(ec_hdr);
	return err;
}

/* 
                                                      
                                      
                             
  
                                                                         
                                                                               
                                                                              
                                                                           
        
  
                                                                             
                                                   
 */
struct ubi_ainf_peb *ubi_early_get_peb(struct ubi_device *ubi,
				       struct ubi_attach_info *ai)
{
	int err = 0;
	struct ubi_ainf_peb *aeb, *tmp_aeb;

	if (!list_empty(&ai->free)) {
		aeb = list_entry(ai->free.next, struct ubi_ainf_peb, u.list);
		list_del(&aeb->u.list);
		dbg_bld("return free PEB %d, EC %d", aeb->pnum, aeb->ec);
		return aeb;
	}

	/*
                                                                     
                                                                       
                                                                     
                             
  */
	list_for_each_entry_safe(aeb, tmp_aeb, &ai->erase, u.list) {
		if (aeb->ec == UBI_UNKNOWN)
			aeb->ec = ai->mean_ec;

		err = early_erase_peb(ubi, ai, aeb->pnum, aeb->ec+1);
		if (err)
			continue;

		aeb->ec += 1;
		list_del(&aeb->u.list);
		dbg_bld("return PEB %d, EC %d", aeb->pnum, aeb->ec);
		return aeb;
	}

	ubi_err("no free eraseblocks");
	return ERR_PTR(-ENOSPC);
}

/* 
                                                 
                                      
                                                   
                                                 
  
                                                                            
                                                                               
                                                                         
                                                                          
                                                                             
                                                            
  
                                                                           
                                                                          
               
 */
static int check_corruption(struct ubi_device *ubi, struct ubi_vid_hdr *vid_hdr,
			    int pnum)
{
	int err;

	mutex_lock(&ubi->buf_mutex);
	memset(ubi->peb_buf, 0x00, ubi->leb_size);

	err = ubi_io_read(ubi, ubi->peb_buf, pnum, ubi->leb_start,
			  ubi->leb_size);
	if (err == UBI_IO_BITFLIPS || mtd_is_eccerr(err)) {
		/*
                                                               
                                                               
                                                                 
                                                               
            
   */
		err = 0;
		goto out_unlock;
	}

	if (err)
		goto out_unlock;

	if (ubi_check_pattern(ubi->peb_buf, 0xFF, ubi->leb_size))
		goto out_unlock;

	ubi_err("PEB %d contains corrupted VID header, and the data does not contain all 0xFF",
		pnum);
	ubi_err("this may be a non-UBI PEB or a severe VID header corruption which requires manual inspection");
	ubi_dump_vid_hdr(vid_hdr);
	pr_err("hexdump of PEB %d offset %d, length %d",
	       pnum, ubi->leb_start, ubi->leb_size);
	ubi_dbg_print_hex_dump(KERN_DEBUG, "", DUMP_PREFIX_OFFSET, 32, 1,
			       ubi->peb_buf, ubi->leb_size, 1);
	err = 1;

out_unlock:
	mutex_unlock(&ubi->buf_mutex);
	return err;
}

/* 
                                                    
                                      
                             
                                        
                                                                         
                                                                       
  
                                                                      
                                                                         
                                                                          
                                                                     
 */
static int scan_peb(struct ubi_device *ubi, struct ubi_attach_info *ai,
		    int pnum, int *vid, unsigned long long *sqnum)
{
	long long uninitialized_var(ec);
	int err, bitflips = 0, vol_id = -1, ec_err = 0;

	dbg_bld("scan PEB %d", pnum);

	/*                               */
	err = ubi_io_is_bad(ubi, pnum);
	if (err < 0)
		return err;
	else if (err) {
		ai->bad_peb_count += 1;
		return 0;
	}

	err = ubi_io_read_ec_hdr(ubi, pnum, ech, 0);
	if (err < 0)
		return err;
	switch (err) {
	case 0:
		break;
	case UBI_IO_BITFLIPS:
		bitflips = 1;
		break;
	case UBI_IO_FF:
		ai->empty_peb_count += 1;
		return add_to_list(ai, pnum, UBI_UNKNOWN, UBI_UNKNOWN,
				   UBI_UNKNOWN, 0, &ai->erase);
	case UBI_IO_FF_BITFLIPS:
		ai->empty_peb_count += 1;
		return add_to_list(ai, pnum, UBI_UNKNOWN, UBI_UNKNOWN,
				   UBI_UNKNOWN, 1, &ai->erase);
	case UBI_IO_BAD_HDR_EBADMSG:
	case UBI_IO_BAD_HDR:
		/*
                                                               
                                                               
                                
   */
		ec_err = err;
		ec = UBI_UNKNOWN;
		bitflips = 1;
		break;
	default:
		ubi_err("'ubi_io_read_ec_hdr()' returned unknown code %d", err);
		return -EINVAL;
	}

	if (!ec_err) {
		int image_seq;

		/*                             */
		if (ech->version != UBI_VERSION) {
			ubi_err("this UBI version is %d, image version is %d",
				UBI_VERSION, (int)ech->version);
			return -EINVAL;
		}

		ec = be64_to_cpu(ech->ec);
		if (ec > UBI_MAX_ERASECOUNTER) {
			/*
                                                         
                                                     
                                                         
                                                      
                 
    */
			ubi_err("erase counter overflow, max is %d",
				UBI_MAX_ERASECOUNTER);
			ubi_dump_ec_hdr(ech);
			return -EINVAL;
		}

		/*
                                                                 
                                                             
                                                                
                                                           
                                                            
                                                                 
                                                             
                                                             
            
   */
		image_seq = be32_to_cpu(ech->image_seq);
		if (!ubi->image_seq && image_seq)
			ubi->image_seq = image_seq;
		if (ubi->image_seq && image_seq &&
		    ubi->image_seq != image_seq) {
			ubi_err("bad image sequence number %d in PEB %d, expected %d",
				image_seq, pnum, ubi->image_seq);
			ubi_dump_ec_hdr(ech);
			return -EINVAL;
		}
	}

	/*                                                                 */

	err = ubi_io_read_vid_hdr(ubi, pnum, vidh, 0);
	if (err < 0)
		return err;
	switch (err) {
	case 0:
		break;
	case UBI_IO_BITFLIPS:
		bitflips = 1;
		break;
	case UBI_IO_BAD_HDR_EBADMSG:
		if (ec_err == UBI_IO_BAD_HDR_EBADMSG)
			/*
                                                         
                                                       
                                                         
                                              
    */
			ai->maybe_bad_peb_count += 1;
	case UBI_IO_BAD_HDR:
		if (ec_err)
			/*
                                                        
                                                       
                                                        
                                                        
                                                          
                                                          
     
                                                    
                                                       
                                      
    */
			err = 0;
		else
			/*
                                                        
                                             
    */
			err = check_corruption(ubi, vidh, pnum);

		if (err < 0)
			return err;
		else if (!err)
			/*                                          */
			err = add_to_list(ai, pnum, UBI_UNKNOWN,
					  UBI_UNKNOWN, ec, 1, &ai->erase);
		else
			/*                                  */
			err = add_corrupted(ai, pnum, ec);
		if (err)
			return err;
		goto adjust_mean_ec;
	case UBI_IO_FF_BITFLIPS:
		err = add_to_list(ai, pnum, UBI_UNKNOWN, UBI_UNKNOWN,
				  ec, 1, &ai->erase);
		if (err)
			return err;
		goto adjust_mean_ec;
	case UBI_IO_FF:
		if (ec_err || bitflips)
			err = add_to_list(ai, pnum, UBI_UNKNOWN,
					  UBI_UNKNOWN, ec, 1, &ai->erase);
		else
			err = add_to_list(ai, pnum, UBI_UNKNOWN,
					  UBI_UNKNOWN, ec, 0, &ai->free);
		if (err)
			return err;
		goto adjust_mean_ec;
	default:
		ubi_err("'ubi_io_read_vid_hdr()' returned unknown code %d",
			err);
		return -EINVAL;
	}

	vol_id = be32_to_cpu(vidh->vol_id);
	if (vid)
		*vid = vol_id;
	if (sqnum)
		*sqnum = be64_to_cpu(vidh->sqnum);
	if (vol_id > UBI_MAX_VOLUMES && vol_id != UBI_LAYOUT_VOLUME_ID) {
		int lnum = be32_to_cpu(vidh->lnum);

		/*                             */
		switch (vidh->compat) {
		case UBI_COMPAT_DELETE:
			if (vol_id != UBI_FM_SB_VOLUME_ID
			    && vol_id != UBI_FM_DATA_VOLUME_ID) {
				ubi_msg("\"delete\" compatible internal volume %d:%d found, will remove it",
					vol_id, lnum);
			}
			err = add_to_list(ai, pnum, vol_id, lnum,
					  ec, 1, &ai->erase);
			if (err)
				return err;
			return 0;

		case UBI_COMPAT_RO:
			ubi_msg("read-only compatible internal volume %d:%d found, switch to read-only mode",
				vol_id, lnum);
			ubi->ro_mode = 1;
			break;

		case UBI_COMPAT_PRESERVE:
			ubi_msg("\"preserve\" compatible internal volume %d:%d found",
				vol_id, lnum);
			err = add_to_list(ai, pnum, vol_id, lnum,
					  ec, 0, &ai->alien);
			if (err)
				return err;
			return 0;

		case UBI_COMPAT_REJECT:
			ubi_err("incompatible internal volume %d:%d found",
				vol_id, lnum);
			return -EINVAL;
		}
	}

	if (ec_err)
		ubi_warn("valid VID header but corrupted EC header at PEB %d",
			 pnum);
	err = ubi_add_to_av(ubi, ai, pnum, ec, vidh, bitflips);
	if (err)
		return err;

adjust_mean_ec:
	if (!ec_err) {
		ai->ec_sum += ec;
		ai->ec_count += 1;
		if (ec > ai->max_ec)
			ai->max_ec = ec;
		if (ec < ai->min_ec)
			ai->min_ec = ec;
	}

	return 0;
}

/* 
                                                          
                                      
                             
  
                                                                          
                                                                             
                                                                           
                                                                              
                                                                               
 */
static int late_analysis(struct ubi_device *ubi, struct ubi_attach_info *ai)
{
	struct ubi_ainf_peb *aeb;
	int max_corr, peb_count;

	peb_count = ubi->peb_count - ai->bad_peb_count - ai->alien_peb_count;
	max_corr = peb_count / 20 ?: 8;

	/*
                                                                   
                                                                     
                                
  */
	if (ai->corr_peb_count) {
		ubi_err("%d PEBs are corrupted and preserved",
			ai->corr_peb_count);
		pr_err("Corrupted PEBs are:");
		list_for_each_entry(aeb, &ai->corr, u.list)
			pr_cont(" %d", aeb->pnum);
		pr_cont("\n");

		/*
                                                         
                                     
   */
		if (ai->corr_peb_count >= max_corr) {
			ubi_err("too many corrupted PEBs, refusing");
			return -EINVAL;
		}
	}

	if (ai->empty_peb_count + ai->maybe_bad_peb_count == peb_count) {
		/*
                                                                
                                                           
    
                                                              
                              
    
                                                                 
                                                                
                                                                 
                                                       
    
                                                                
                                                      
   */
		if (ai->maybe_bad_peb_count <= 2) {
			ai->is_empty = 1;
			ubi_msg("empty MTD device detected");
			get_random_bytes(&ubi->image_seq,
					 sizeof(ubi->image_seq));
		} else {
			ubi_err("MTD device is not UBI-formatted and possibly contains non-UBI data - refusing it");
			return -EINVAL;
		}

	}

	return 0;
}

/* 
                                                  
                                    
                             
  
                                                           
 */
static void destroy_av(struct ubi_attach_info *ai, struct ubi_ainf_volume *av)
{
	struct ubi_ainf_peb *aeb;
	struct rb_node *this = av->root.rb_node;

	while (this) {
		if (this->rb_left)
			this = this->rb_left;
		else if (this->rb_right)
			this = this->rb_right;
		else {
			aeb = rb_entry(this, struct ubi_ainf_peb, u.rb);
			this = rb_parent(this);
			if (this) {
				if (this->rb_left == &aeb->u.rb)
					this->rb_left = NULL;
				else
					this->rb_right = NULL;
			}

			kmem_cache_free(ai->aeb_slab_cache, aeb);
		}
	}
	kfree(av);
}

/* 
                                              
                             
 */
static void destroy_ai(struct ubi_attach_info *ai)
{
	struct ubi_ainf_peb *aeb, *aeb_tmp;
	struct ubi_ainf_volume *av;
	struct rb_node *rb;

	list_for_each_entry_safe(aeb, aeb_tmp, &ai->alien, u.list) {
		list_del(&aeb->u.list);
		kmem_cache_free(ai->aeb_slab_cache, aeb);
	}
	list_for_each_entry_safe(aeb, aeb_tmp, &ai->erase, u.list) {
		list_del(&aeb->u.list);
		kmem_cache_free(ai->aeb_slab_cache, aeb);
	}
	list_for_each_entry_safe(aeb, aeb_tmp, &ai->corr, u.list) {
		list_del(&aeb->u.list);
		kmem_cache_free(ai->aeb_slab_cache, aeb);
	}
	list_for_each_entry_safe(aeb, aeb_tmp, &ai->free, u.list) {
		list_del(&aeb->u.list);
		kmem_cache_free(ai->aeb_slab_cache, aeb);
	}

	/*                            */
	rb = ai->volumes.rb_node;
	while (rb) {
		if (rb->rb_left)
			rb = rb->rb_left;
		else if (rb->rb_right)
			rb = rb->rb_right;
		else {
			av = rb_entry(rb, struct ubi_ainf_volume, rb);

			rb = rb_parent(rb);
			if (rb) {
				if (rb->rb_left == &av->rb)
					rb->rb_left = NULL;
				else
					rb->rb_right = NULL;
			}

			destroy_av(ai, av);
		}
	}

	if (ai->aeb_slab_cache)
		kmem_cache_destroy(ai->aeb_slab_cache);

	kfree(ai);
}

/* 
                                     
                                      
                          
                                     
  
                                                                         
                                                                             
                                         
 */
static int scan_all(struct ubi_device *ubi, struct ubi_attach_info *ai,
		    int start)
{
	int err, pnum;
	struct rb_node *rb1, *rb2;
	struct ubi_ainf_volume *av;
	struct ubi_ainf_peb *aeb;

	err = -ENOMEM;

	ech = kzalloc(ubi->ec_hdr_alsize, GFP_KERNEL);
	if (!ech)
		return err;

	vidh = ubi_zalloc_vid_hdr(ubi, GFP_KERNEL);
	if (!vidh)
		goto out_ech;

	for (pnum = start; pnum < ubi->peb_count; pnum++) {
		cond_resched();

		dbg_gen("process PEB %d", pnum);
		err = scan_peb(ubi, ai, pnum, NULL, NULL);
		if (err < 0)
			goto out_vidh;
	}

	ubi_msg("scanning is finished");

	/*                              */
	if (ai->ec_count)
		ai->mean_ec = div_u64(ai->ec_sum, ai->ec_count);

	err = late_analysis(ubi, ai);
	if (err)
		goto out_vidh;

	/*
                                                                  
          
  */
	ubi_rb_for_each_entry(rb1, av, &ai->volumes, rb) {
		ubi_rb_for_each_entry(rb2, aeb, &av->root, u.rb)
			if (aeb->ec == UBI_UNKNOWN)
				aeb->ec = ai->mean_ec;
	}

	list_for_each_entry(aeb, &ai->free, u.list) {
		if (aeb->ec == UBI_UNKNOWN)
			aeb->ec = ai->mean_ec;
	}

	list_for_each_entry(aeb, &ai->corr, u.list)
		if (aeb->ec == UBI_UNKNOWN)
			aeb->ec = ai->mean_ec;

	list_for_each_entry(aeb, &ai->erase, u.list)
		if (aeb->ec == UBI_UNKNOWN)
			aeb->ec = ai->mean_ec;

	err = self_check_ai(ubi, ai);
	if (err)
		goto out_vidh;

	ubi_free_vid_hdr(ubi, vidh);
	kfree(ech);

	return 0;

out_vidh:
	ubi_free_vid_hdr(ubi, vidh);
out_ech:
	kfree(ech);
	return err;
}

#ifdef CONFIG_MTD_UBI_FASTMAP

/* 
                                                           
                                      
                          
  
                                                                    
         
                                                    
                                                              
 */
static int scan_fast(struct ubi_device *ubi, struct ubi_attach_info *ai)
{
	int err, pnum, fm_anchor = -1;
	unsigned long long max_sqnum = 0;

	err = -ENOMEM;

	ech = kzalloc(ubi->ec_hdr_alsize, GFP_KERNEL);
	if (!ech)
		goto out;

	vidh = ubi_zalloc_vid_hdr(ubi, GFP_KERNEL);
	if (!vidh)
		goto out_ech;

	for (pnum = 0; pnum < UBI_FM_MAX_START; pnum++) {
		int vol_id = -1;
		unsigned long long sqnum = -1;
		cond_resched();

		dbg_gen("process PEB %d", pnum);
		err = scan_peb(ubi, ai, pnum, &vol_id, &sqnum);
		if (err < 0)
			goto out_vidh;

		if (vol_id == UBI_FM_SB_VOLUME_ID && sqnum > max_sqnum) {
			max_sqnum = sqnum;
			fm_anchor = pnum;
		}
	}

	ubi_free_vid_hdr(ubi, vidh);
	kfree(ech);

	if (fm_anchor < 0)
		return UBI_NO_FASTMAP;

	return ubi_scan_fastmap(ubi, ai, fm_anchor);

out_vidh:
	ubi_free_vid_hdr(ubi, vidh);
out_ech:
	kfree(ech);
out:
	return err;
}

#endif

static struct ubi_attach_info *alloc_ai(const char *slab_name)
{
	struct ubi_attach_info *ai;

	ai = kzalloc(sizeof(struct ubi_attach_info), GFP_KERNEL);
	if (!ai)
		return ai;

	INIT_LIST_HEAD(&ai->corr);
	INIT_LIST_HEAD(&ai->free);
	INIT_LIST_HEAD(&ai->erase);
	INIT_LIST_HEAD(&ai->alien);
	ai->volumes = RB_ROOT;
	ai->aeb_slab_cache = kmem_cache_create(slab_name,
					       sizeof(struct ubi_ainf_peb),
					       0, 0, NULL);
	if (!ai->aeb_slab_cache) {
		kfree(ai);
		ai = NULL;
	}

	return ai;
}

/* 
                                     
                              
                                                     
  
                                                                             
                   
 */
int ubi_attach(struct ubi_device *ubi, int force_scan)
{
	int err;
	struct ubi_attach_info *ai;

	ai = alloc_ai("ubi_aeb_slab_cache");
	if (!ai)
		return -ENOMEM;

#ifdef CONFIG_MTD_UBI_FASTMAP
	/*                                                        */
	if ((int)mtd_div_by_eb(ubi->mtd->size, ubi->mtd) <= UBI_FM_MAX_START) {
		ubi->fm_disabled = 1;
		force_scan = 1;
	}

	if (force_scan)
		err = scan_all(ubi, ai, 0);
	else {
		err = scan_fast(ubi, ai);
		if (err > 0) {
			if (err != UBI_NO_FASTMAP) {
				destroy_ai(ai);
				ai = alloc_ai("ubi_aeb_slab_cache2");
				if (!ai)
					return -ENOMEM;
			}

			err = scan_all(ubi, ai, UBI_FM_MAX_START);
		}
	}
#else
	err = scan_all(ubi, ai, 0);
#endif
	if (err)
		goto out_ai;

	ubi->bad_peb_count = ai->bad_peb_count;
	ubi->good_peb_count = ubi->peb_count - ubi->bad_peb_count;
	ubi->corr_peb_count = ai->corr_peb_count;
	ubi->max_ec = ai->max_ec;
	ubi->mean_ec = ai->mean_ec;
	dbg_gen("max. sequence number:       %llu", ai->max_sqnum);

	err = ubi_read_volume_table(ubi, ai);
	if (err)
		goto out_ai;

	err = ubi_wl_init(ubi, ai);
	if (err)
		goto out_vtbl;

	err = ubi_eba_init(ubi, ai);
	if (err)
		goto out_wl;

#ifdef CONFIG_MTD_UBI_FASTMAP
	if (ubi->fm && ubi_dbg_chk_gen(ubi)) {
		struct ubi_attach_info *scan_ai;

		scan_ai = alloc_ai("ubi_ckh_aeb_slab_cache");
		if (!scan_ai)
			goto out_wl;

		err = scan_all(ubi, scan_ai, 0);
		if (err) {
			destroy_ai(scan_ai);
			goto out_wl;
		}

		err = self_check_eba(ubi, ai, scan_ai);
		destroy_ai(scan_ai);

		if (err)
			goto out_wl;
	}
#endif

	destroy_ai(ai);
	return 0;

out_wl:
	ubi_wl_close(ubi);
out_vtbl:
	ubi_free_internal_volumes(ubi);
	vfree(ubi->vtbl);
out_ai:
	destroy_ai(ai);
	return err;
}

/* 
                                                   
                                      
                             
  
                                                                              
                                                      
 */
static int self_check_ai(struct ubi_device *ubi, struct ubi_attach_info *ai)
{
	int pnum, err, vols_found = 0;
	struct rb_node *rb1, *rb2;
	struct ubi_ainf_volume *av;
	struct ubi_ainf_peb *aeb, *last_aeb;
	uint8_t *buf;

	if (!ubi_dbg_chk_gen(ubi))
		return 0;

	/*
                                                     
  */
	ubi_rb_for_each_entry(rb1, av, &ai->volumes, rb) {
		int leb_count = 0;

		cond_resched();

		vols_found += 1;

		if (ai->is_empty) {
			ubi_err("bad is_empty flag");
			goto bad_av;
		}

		if (av->vol_id < 0 || av->highest_lnum < 0 ||
		    av->leb_count < 0 || av->vol_type < 0 || av->used_ebs < 0 ||
		    av->data_pad < 0 || av->last_data_size < 0) {
			ubi_err("negative values");
			goto bad_av;
		}

		if (av->vol_id >= UBI_MAX_VOLUMES &&
		    av->vol_id < UBI_INTERNAL_VOL_START) {
			ubi_err("bad vol_id");
			goto bad_av;
		}

		if (av->vol_id > ai->highest_vol_id) {
			ubi_err("highest_vol_id is %d, but vol_id %d is there",
				ai->highest_vol_id, av->vol_id);
			goto out;
		}

		if (av->vol_type != UBI_DYNAMIC_VOLUME &&
		    av->vol_type != UBI_STATIC_VOLUME) {
			ubi_err("bad vol_type");
			goto bad_av;
		}

		if (av->data_pad > ubi->leb_size / 2) {
			ubi_err("bad data_pad");
			goto bad_av;
		}

		last_aeb = NULL;
		ubi_rb_for_each_entry(rb2, aeb, &av->root, u.rb) {
			cond_resched();

			last_aeb = aeb;
			leb_count += 1;

			if (aeb->pnum < 0 || aeb->ec < 0) {
				ubi_err("negative values");
				goto bad_aeb;
			}

			if (aeb->ec < ai->min_ec) {
				ubi_err("bad ai->min_ec (%d), %d found",
					ai->min_ec, aeb->ec);
				goto bad_aeb;
			}

			if (aeb->ec > ai->max_ec) {
				ubi_err("bad ai->max_ec (%d), %d found",
					ai->max_ec, aeb->ec);
				goto bad_aeb;
			}

			if (aeb->pnum >= ubi->peb_count) {
				ubi_err("too high PEB number %d, total PEBs %d",
					aeb->pnum, ubi->peb_count);
				goto bad_aeb;
			}

			if (av->vol_type == UBI_STATIC_VOLUME) {
				if (aeb->lnum >= av->used_ebs) {
					ubi_err("bad lnum or used_ebs");
					goto bad_aeb;
				}
			} else {
				if (av->used_ebs != 0) {
					ubi_err("non-zero used_ebs");
					goto bad_aeb;
				}
			}

			if (aeb->lnum > av->highest_lnum) {
				ubi_err("incorrect highest_lnum or lnum");
				goto bad_aeb;
			}
		}

		if (av->leb_count != leb_count) {
			ubi_err("bad leb_count, %d objects in the tree",
				leb_count);
			goto bad_av;
		}

		if (!last_aeb)
			continue;

		aeb = last_aeb;

		if (aeb->lnum != av->highest_lnum) {
			ubi_err("bad highest_lnum");
			goto bad_aeb;
		}
	}

	if (vols_found != ai->vols_found) {
		ubi_err("bad ai->vols_found %d, should be %d",
			ai->vols_found, vols_found);
		goto out;
	}

	/*                                             */
	ubi_rb_for_each_entry(rb1, av, &ai->volumes, rb) {
		last_aeb = NULL;
		ubi_rb_for_each_entry(rb2, aeb, &av->root, u.rb) {
			int vol_type;

			cond_resched();

			last_aeb = aeb;

			err = ubi_io_read_vid_hdr(ubi, aeb->pnum, vidh, 1);
			if (err && err != UBI_IO_BITFLIPS) {
				ubi_err("VID header is not OK (%d)", err);
				if (err > 0)
					err = -EIO;
				return err;
			}

			vol_type = vidh->vol_type == UBI_VID_DYNAMIC ?
				   UBI_DYNAMIC_VOLUME : UBI_STATIC_VOLUME;
			if (av->vol_type != vol_type) {
				ubi_err("bad vol_type");
				goto bad_vid_hdr;
			}

			if (aeb->sqnum != be64_to_cpu(vidh->sqnum)) {
				ubi_err("bad sqnum %llu", aeb->sqnum);
				goto bad_vid_hdr;
			}

			if (av->vol_id != be32_to_cpu(vidh->vol_id)) {
				ubi_err("bad vol_id %d", av->vol_id);
				goto bad_vid_hdr;
			}

			if (av->compat != vidh->compat) {
				ubi_err("bad compat %d", vidh->compat);
				goto bad_vid_hdr;
			}

			if (aeb->lnum != be32_to_cpu(vidh->lnum)) {
				ubi_err("bad lnum %d", aeb->lnum);
				goto bad_vid_hdr;
			}

			if (av->used_ebs != be32_to_cpu(vidh->used_ebs)) {
				ubi_err("bad used_ebs %d", av->used_ebs);
				goto bad_vid_hdr;
			}

			if (av->data_pad != be32_to_cpu(vidh->data_pad)) {
				ubi_err("bad data_pad %d", av->data_pad);
				goto bad_vid_hdr;
			}
		}

		if (!last_aeb)
			continue;

		if (av->highest_lnum != be32_to_cpu(vidh->lnum)) {
			ubi_err("bad highest_lnum %d", av->highest_lnum);
			goto bad_vid_hdr;
		}

		if (av->last_data_size != be32_to_cpu(vidh->data_size)) {
			ubi_err("bad last_data_size %d", av->last_data_size);
			goto bad_vid_hdr;
		}
	}

	/*
                                                                       
             
  */
	buf = kzalloc(ubi->peb_count, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	for (pnum = 0; pnum < ubi->peb_count; pnum++) {
		err = ubi_io_is_bad(ubi, pnum);
		if (err < 0) {
			kfree(buf);
			return err;
		} else if (err)
			buf[pnum] = 1;
	}

	ubi_rb_for_each_entry(rb1, av, &ai->volumes, rb)
		ubi_rb_for_each_entry(rb2, aeb, &av->root, u.rb)
			buf[aeb->pnum] = 1;

	list_for_each_entry(aeb, &ai->free, u.list)
		buf[aeb->pnum] = 1;

	list_for_each_entry(aeb, &ai->corr, u.list)
		buf[aeb->pnum] = 1;

	list_for_each_entry(aeb, &ai->erase, u.list)
		buf[aeb->pnum] = 1;

	list_for_each_entry(aeb, &ai->alien, u.list)
		buf[aeb->pnum] = 1;

	err = 0;
	for (pnum = 0; pnum < ubi->peb_count; pnum++)
		if (!buf[pnum]) {
			ubi_err("PEB %d is not referred", pnum);
			err = 1;
		}

	kfree(buf);
	if (err)
		goto out;
	return 0;

bad_aeb:
	ubi_err("bad attaching information about LEB %d", aeb->lnum);
	ubi_dump_aeb(aeb, 0);
	ubi_dump_av(av);
	goto out;

bad_av:
	ubi_err("bad attaching information about volume %d", av->vol_id);
	ubi_dump_av(av);
	goto out;

bad_vid_hdr:
	ubi_err("bad attaching information about volume %d", av->vol_id);
	ubi_dump_av(av);
	ubi_dump_vid_hdr(vidh);

out:
	dump_stack();
	return -EINVAL;
}
