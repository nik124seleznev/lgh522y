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
 * Authors: Artem Bityutskiy (Битюцкий Артём)
 *          Thomas Gleixner
 *          Frank Haverkamp
 *          Oliver Lohmann
 *          Andreas Arnez
 */

/*
                                                                             
                   
 */

#ifndef __UBI_MEDIA_H__
#define __UBI_MEDIA_H__

#include <asm/byteorder.h>

/*                                                            */
#define UBI_VERSION 1

/*                                                                  */
#define UBI_MAX_ERASECOUNTER 0x7FFFFFFF

/*                                                             */
#define UBI_CRC32_INIT 0xFFFFFFFFU

/*                                                  */
#define UBI_EC_HDR_MAGIC  0x55424923
/*                                                      */
#define UBI_VID_HDR_MAGIC 0x55424921

/*
                                                              
  
                                   
                                 
 */
enum {
	UBI_VID_DYNAMIC = 1,
	UBI_VID_STATIC  = 2
};

/*
                                                
  
                                                    
  
                                                                             
                                                                             
                                                                          
                                                                             
                                                                            
                                                                            
                                                                              
                                                                         
                                                                            
                                                                              
                                         
  
                                                                        
                                                                               
                                                                            
                                                                              
                                                                               
                                                                              
                                                                               
                                                                              
                                                                          
                                                                            
                                                        
  
                                                                       
                                                                          
                                                                               
 */
enum {
	UBI_VTBL_AUTORESIZE_FLG = 0x01,
};

/*
                                                    
  
                                                                             
                                   
                                                       
                                                                         
                                                                            
                                                
                                            
 */
enum {
	UBI_COMPAT_DELETE   = 1,
	UBI_COMPAT_RO       = 2,
	UBI_COMPAT_PRESERVE = 4,
	UBI_COMPAT_REJECT   = 5
};

/*                      */
#define UBI_EC_HDR_SIZE  sizeof(struct ubi_ec_hdr)
#define UBI_VID_HDR_SIZE sizeof(struct ubi_vid_hdr)

/*                                             */
#define UBI_EC_HDR_SIZE_CRC  (UBI_EC_HDR_SIZE  - sizeof(__be32))
#define UBI_VID_HDR_SIZE_CRC (UBI_VID_HDR_SIZE - sizeof(__be32))

/* 
                                                
                                                                
                                                                           
                      
                                         
                         
                                               
                                          
                                    
                                         
                                              
  
                                                                               
                                                                            
                                                                             
                                                                        
                                                                            
                                                                          
          
  
                                                                            
                                                                           
                                                                         
               
  
                                                                              
                                                                              
                                                                              
                                                                              
                                                                               
                                                                             
                                                                
 */
struct ubi_ec_hdr {
	__be32  magic;
	__u8    version;
	__u8    padding1[3];
	__be64  ec; /*                                              */
	__be32  vid_hdr_offset;
	__be32  data_offset;
	__be32  image_seq;
	__u8    padding2[32];
	__be32  hdr_crc;
} __packed;

/* 
                                                              
                                                                     
                                                                            
                                 
                                                               
                                                                          
                                                     
                                                                 
                                                                            
                             
                                   
                                         
                                                                      
                                                                     
                                                                           
                  
                                                                        
                                         
                          
                                         
                                                  
  
                                                                               
                                                                               
                                                                        
                                                                             
                                                                       
                                                                               
                       
  
                                                                             
                                                                              
                                                                              
                                                              
  
                                                                              
                                                                           
                                                                               
                                                                            
                                                                             
                                                                              
         
  
                                                                       
                                                                             
                                                                            
                                                                          
                                                                             
                                                                             
                                                                              
                                                                            
                                                                           
                                                                              
                                                                            
                                                                              
                                                                          
                           
  
                                                                              
                                                                             
                                                                               
                                                                       
                                                                            
                                                                              
                                                                            
                                 
  
                                                                          
                                                                               
                                                                               
                                                                            
                                                                               
                                                
  
                                                                               
                                                                          
                                                                              
                                                                           
                                                                         
                                                                            
                                                                             
                                                                              
                                        
  
                                                                               
                                                                             
                                                                         
                                                                             
                                                                             
                                                                       
  
                                                                               
                                                                             
                                                                               
                                                                   
                                                                          
                                                                            
                                       
  
                                                                             
                                                                              
                                     
  
                                                                           
                                                                              
                                                                              
                                                   
 */
struct ubi_vid_hdr {
	__be32  magic;
	__u8    version;
	__u8    vol_type;
	__u8    copy_flag;
	__u8    compat;
	__be32  vol_id;
	__be32  lnum;
	__u8    padding1[4];
	__be32  data_size;
	__be32  used_ebs;
	__be32  data_pad;
	__be32  data_crc;
	__u8    padding2[4];
	__be64  sqnum;
	__u8    padding3[12];
	__be32  hdr_crc;
} __packed;

/*                            */
#define UBI_INT_VOL_COUNT 1

/*
                                               
                                                    
 */
#define UBI_INTERNAL_VOL_START (0x7FFFFFFF - 4096)

/*                                             */

#define UBI_LAYOUT_VOLUME_ID     UBI_INTERNAL_VOL_START
#define UBI_LAYOUT_VOLUME_TYPE   UBI_VID_DYNAMIC
#define UBI_LAYOUT_VOLUME_ALIGN  1
#define UBI_LAYOUT_VOLUME_EBS    2
#define UBI_LAYOUT_VOLUME_NAME   "layout volume"
#define UBI_LAYOUT_VOLUME_COMPAT UBI_COMPAT_REJECT

/*                                                  */
#define UBI_MAX_VOLUMES 128

/*                                */
#define UBI_VOL_NAME_MAX 127

/*                                 */
#define UBI_VTBL_RECORD_SIZE sizeof(struct ubi_vtbl_record)

/*                                                        */
#define UBI_VTBL_RECORD_SIZE_CRC (UBI_VTBL_RECORD_SIZE - sizeof(__be32))

/* 
                                                         
                                                                             
                               
                                                                       
                                                
                                                                     
                                                             
                                
                         
                                                  
                             
                                       
  
                                                                              
                                                                              
                                                                          
                                                                       
                                    
  
                                                               
                                                                       
                                                                               
                                                                 
  
                                                                           
                                                                               
                                                       
  
                                                                             
                                                                           
                                                                          
                                                                                
                                                                         
                                    
  
                                                                         
 */
struct ubi_vtbl_record {
	__be32  reserved_pebs;
	__be32  alignment;
	__be32  data_pad;
	__u8    vol_type;
	__u8    upd_marker;
	__be16  name_len;
	__u8    name[UBI_VOL_NAME_MAX+1];
	__u8    flags;
	__u8    padding[23];
	__be32  crc;
} __packed;

/*                                      */

#define UBI_FM_SB_VOLUME_ID	(UBI_LAYOUT_VOLUME_ID + 1)
#define UBI_FM_DATA_VOLUME_ID	(UBI_LAYOUT_VOLUME_ID + 2)

/*                                                */
#define UBI_FM_FMT_VERSION	1

#define UBI_FM_SB_MAGIC		0x7B11D69F
#define UBI_FM_HDR_MAGIC	0xD4B82EF7
#define UBI_FM_VHDR_MAGIC	0xFA370ED1
#define UBI_FM_POOL_MAGIC	0x67AF4D08
#define UBI_FM_EBA_MAGIC	0xf0c040a8

/*                                                        
                    */
#define UBI_FM_MAX_START	64

/*                                                */
#define UBI_FM_MAX_BLOCKS	32

/*                                                                  
                  
                                                                              
                        */
#define UBI_FM_MIN_POOL_SIZE	8
#define UBI_FM_MAX_POOL_SIZE	256

#define UBI_FM_WL_POOL_SIZE	25

/* 
                                             
                                                              
                                           
                                       
                                                    
                                                                          
                                                
                                                                             
  
 */
struct ubi_fm_sb {
	__be32 magic;
	__u8 version;
	__u8 padding1[3];
	__be32 data_crc;
	__be32 used_blocks;
	__be32 block_loc[UBI_FM_MAX_BLOCKS];
	__be32 block_ec[UBI_FM_MAX_BLOCKS];
	__be64 sqnum;
	__u8 padding2[32];
} __packed;

/* 
                                                     
                                                          
                                                             
                                                             
                                                                        
                                                           
                                                               
                                                          
 */
struct ubi_fm_hdr {
	__be32 magic;
	__be32 free_peb_count;
	__be32 used_peb_count;
	__be32 scrub_peb_count;
	__be32 bad_peb_count;
	__be32 erase_peb_count;
	__be32 vol_count;
	__u8 padding[4];
} __packed;

/*                                                              */

/* 
                                                                            
                                                
                           
                               
                                                                   
 */
struct ubi_fm_scan_pool {
	__be32 magic;
	__be16 size;
	__be16 max_size;
	__be32 pebs[UBI_FM_MAX_POOL_SIZE];
	__be32 padding[4];
} __packed;

/*                                                                      */

/* 
                                                       
                    
                      
 */
struct ubi_fm_ec {
	__be32 pnum;
	__be32 ec;
} __packed;

/* 
                                               
                                          
                                                                  
                                              
                                           
                                                     
                                                    
                                                       
 */
struct ubi_fm_volhdr {
	__be32 magic;
	__be32 vol_id;
	__u8 vol_type;
	__u8 padding1[3];
	__be32 data_pad;
	__be32 used_ebs;
	__be32 last_eb_bytes;
	__u8 padding2[8];
} __packed;

/*                                                                   */

/* 
                                                                  
                                 
                                          
                                              
 */
struct ubi_fm_eba {
	__be32 magic;
	__be32 reserved_pebs;
	__be32 pnum[0];
} __packed;
#endif /*                  */
