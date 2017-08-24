/*
 *  linux/include/asm/setup.h
 *
 *  Copyright (C) 1997-1999 Russell King
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *  Structure passed to kernel to tell it about the
 *  hardware it's running on.  See Documentation/arm/Setup
 *  for more info.
 */
#ifndef _UAPI__ASMARM_SETUP_H
#define _UAPI__ASMARM_SETUP_H

#include <linux/types.h>
#include <mach/dfo_boot.h>
#include <mach/mt_devinfo.h>

#define COMMAND_LINE_SIZE 1024

/*                                       */
#define ATAG_NONE	0x00000000

struct tag_header {
	__u32 size;
	__u32 tag;
};

/*                                            */
#define ATAG_CORE	0x54410001

struct tag_core {
	__u32 flags;		/*                   */
	__u32 pagesize;
	__u32 rootdev;
};

/*                                               */
#define ATAG_MEM	0x54410002

struct tag_mem32 {
	__u32	size;
	__u32	start;	/*                        */
};

/*                                               */
#define ATAG_MEM64	0x54420002

struct tag_mem64 {
	__u64	size;
	__u64	start;	/*                        */
};

/*                        */
#define ATAG_VIDEOTEXT	0x54410003

struct tag_videotext {
	__u8		x;
	__u8		y;
	__u16		video_page;
	__u8		video_mode;
	__u8		video_cols;
	__u16		video_ega_bx;
	__u8		video_lines;
	__u8		video_isvga;
	__u16		video_points;
};

/*                                                  */
#define ATAG_RAMDISK	0x54410004

struct tag_ramdisk {
	__u32 flags;	/*                              */
	__u32 size;	/*                                           */
	__u32 start;	/*                                               */
};

/*                                                                      */
/*
                                                          
                   
 */
#define ATAG_INITRD	0x54410005

/*                                                                       */
#define ATAG_INITRD2	0x54420005

struct tag_initrd {
	__u32 start;	/*                        */
	__u32 size;	/*                                           */
};

/*                                                               */
#define ATAG_SERIAL	0x54410006

struct tag_serialnr {
	__u32 low;
	__u32 high;
};

/*                */
#define ATAG_REVISION	0x54410007

struct tag_revision {
	__u32 rev;
};

/*                                                                    
                         
 */
#define ATAG_VIDEOLFB	0x54410008

struct tag_videolfb {
	__u16		lfb_width;
	__u16		lfb_height;
	__u16		lfb_depth;
	__u16		lfb_linelength;
	__u32		lfb_base;
	__u32		lfb_size;
	__u8		red_size;
	__u8		red_pos;
	__u8		green_size;
	__u8		green_pos;
	__u8		blue_size;
	__u8		blue_pos;
	__u8		rsvd_size;
	__u8		rsvd_pos;
};

/*                                    */
#define ATAG_CMDLINE	0x54410009

struct tag_cmdline {
	char	cmdline[1];	/*                          */
};

/*                                   */
#define ATAG_ACORN	0x41000101

struct tag_acorn {
	__u32 memc_control_reg;
	__u32 vram_pages;
	__u8 sounddefault;
	__u8 adfsdrives;
};

/*                                                              */
#define ATAG_MEMCLK	0x41000402

struct tag_memclk {
	__u32 fmemclk;
};

/*                  */
#define ATAG_BOOT       0x41000802

struct tag_boot {
	u32 bootmode;
};

/*                         */
#define ATAG_META_COM 0x41000803

struct tag_meta_com {
	u32 meta_com_type; /*                               */
	u32 meta_com_id;  /*                                        */
};


/*        */
#define ATAG_MDINFO_DATA 0x41000806
struct tag_mdinfo_data{
	u8 md_type[4];
};

#define ATAG_TEE_DATA 0x41000808

/*                           */
typedef struct {
    u64 start;
    u64 size;
} mem_desc_t;

/*                       */
typedef struct {
	u64 start;
	u64 size;
	u32 rank;	/*                            */
} mblock_t;

typedef struct {
	u32 mblock_num;
	mblock_t mblock[4];
} mblock_info_t;

typedef struct {
	u32 rank_num;
	mem_desc_t rank_info[4];
} dram_info_t;

struct tag {
	struct tag_header hdr;
	union {
		struct tag_core		core;
		struct tag_mem32	mem;
		struct tag_mem64	mem64;
		struct tag_videotext	videotext;
		struct tag_ramdisk	ramdisk;
		struct tag_initrd	initrd;
		struct tag_serialnr	serialnr;
		struct tag_revision	revision;
		struct tag_videolfb	videolfb;
		struct tag_cmdline	cmdline;

		/*
                   
   */
		struct tag_acorn	acorn;

		/*
                     
   */
		struct tag_memclk	memclk;
		struct tag_boot		boot;
		struct tag_meta_com	meta_com;
		struct tag_devinfo_data	devinfo_data;
                tag_dfo_boot     dfo_data;
                struct tag_mdinfo_data mdinfo_data;
		mem_desc_t tee_reserved_mem;
#ifdef PT_ABTC_ATAG
		struct tag_pt_info tag_pt_info;
#endif
#ifdef NAND_ABTC_ATAG
		struct tag_nand_number tag_nand_number;
		flashdev_info_t gen_FlashTable_p;
#endif
	} u;
};

struct tagtable {
	__u32 tag;
	int (*parse)(const struct tag *);
};

#define tag_member_present(tag,member)				\
	((unsigned long)(&((struct tag *)0L)->member + 1)	\
		<= (tag)->hdr.size * 4)

#define tag_next(t)	((struct tag *)((__u32 *)(t) + (t)->hdr.size))
#define tag_size(type)	((sizeof(struct tag_header) + sizeof(struct type)) >> 2)

#define for_each_tag(t,base)		\
	for (t = base; t->hdr.size; t = tag_next(t))


#endif /*                       */
