/*
 * Copyright (c) 2000-2001,2005 Silicon Graphics, Inc.
 * All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it would be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write the Free Software Foundation,
 * Inc.,  51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef	__XFS_BUF_ITEM_H__
#define	__XFS_BUF_ITEM_H__

extern kmem_zone_t	*xfs_buf_item_zone;

/*
                                                              
                                          
 */
#define	XFS_BLF_INODE_BUF	(1<<0)
/*
                                                             
                                                      
 */
#define	XFS_BLF_CANCEL		(1<<1)

/*
                                                       
                                                                  
 */
#define	XFS_BLF_UDQUOT_BUF	(1<<2)
#define XFS_BLF_PDQUOT_BUF	(1<<3)
#define	XFS_BLF_GDQUOT_BUF	(1<<4)

#define	XFS_BLF_CHUNK		128
#define	XFS_BLF_SHIFT		7
#define	BIT_TO_WORD_SHIFT	5
#define	NBWORD			(NBBY * sizeof(unsigned int))

/*
                                                              
                                                                   
                    
 */
#define XFS_BLF_DATAMAP_SIZE	((XFS_MAX_BLOCKSIZE / XFS_BLF_CHUNK) / NBWORD)

typedef struct xfs_buf_log_format {
	unsigned short	blf_type;	/*                             */
	unsigned short	blf_size;	/*                   */
	ushort		blf_flags;	/*            */
	ushort		blf_len;	/*                              */
	__int64_t	blf_blkno;	/*                            */
	unsigned int	blf_map_size;	/*                                   */
	unsigned int	blf_data_map[XFS_BLF_DATAMAP_SIZE]; /*              */
} xfs_buf_log_format_t;

/*
                                                               
                                                                          
                                                       
  
                                                                              
                                                                           
 */
#define XFS_BLFT_BITS	5
#define XFS_BLFT_SHIFT	11
#define XFS_BLFT_MASK	(((1 << XFS_BLFT_BITS) - 1) << XFS_BLFT_SHIFT)

enum xfs_blft {
	XFS_BLFT_UNKNOWN_BUF = 0,
	XFS_BLFT_UDQUOT_BUF,
	XFS_BLFT_PDQUOT_BUF,
	XFS_BLFT_GDQUOT_BUF,
	XFS_BLFT_BTREE_BUF,
	XFS_BLFT_AGF_BUF,
	XFS_BLFT_AGFL_BUF,
	XFS_BLFT_AGI_BUF,
	XFS_BLFT_DINO_BUF,
	XFS_BLFT_SYMLINK_BUF,
	XFS_BLFT_DIR_BLOCK_BUF,
	XFS_BLFT_DIR_DATA_BUF,
	XFS_BLFT_DIR_FREE_BUF,
	XFS_BLFT_DIR_LEAF1_BUF,
	XFS_BLFT_DIR_LEAFN_BUF,
	XFS_BLFT_DA_NODE_BUF,
	XFS_BLFT_ATTR_LEAF_BUF,
	XFS_BLFT_ATTR_RMT_BUF,
	XFS_BLFT_SB_BUF,
	XFS_BLFT_MAX_BUF = (1 << XFS_BLFT_BITS),
};

static inline void
xfs_blft_to_flags(struct xfs_buf_log_format *blf, enum xfs_blft type)
{
	ASSERT(type > XFS_BLFT_UNKNOWN_BUF && type < XFS_BLFT_MAX_BUF);
	blf->blf_flags &= ~XFS_BLFT_MASK;
	blf->blf_flags |= ((type << XFS_BLFT_SHIFT) & XFS_BLFT_MASK);
}

static inline __uint16_t
xfs_blft_from_flags(struct xfs_buf_log_format *blf)
{
	return (blf->blf_flags & XFS_BLFT_MASK) >> XFS_BLFT_SHIFT;
}

/*
                     
 */
#define	XFS_BLI_HOLD		0x01
#define	XFS_BLI_DIRTY		0x02
#define	XFS_BLI_STALE		0x04
#define	XFS_BLI_LOGGED		0x08
#define	XFS_BLI_INODE_ALLOC_BUF	0x10
#define XFS_BLI_STALE_INODE	0x20
#define	XFS_BLI_INODE_BUF	0x40

#define XFS_BLI_FLAGS \
	{ XFS_BLI_HOLD,		"HOLD" }, \
	{ XFS_BLI_DIRTY,	"DIRTY" }, \
	{ XFS_BLI_STALE,	"STALE" }, \
	{ XFS_BLI_LOGGED,	"LOGGED" }, \
	{ XFS_BLI_INODE_ALLOC_BUF, "INODE_ALLOC" }, \
	{ XFS_BLI_STALE_INODE,	"STALE_INODE" }, \
	{ XFS_BLI_INODE_BUF,	"INODE_BUF" }


#ifdef __KERNEL__

struct xfs_buf;
struct xfs_mount;
struct xfs_buf_log_item;

/*
                                                                   
                                                                     
                                                             
 */
typedef struct xfs_buf_log_item {
	xfs_log_item_t		bli_item;	/*                       */
	struct xfs_buf		*bli_buf;	/*                     */
	unsigned int		bli_flags;	/*            */
	unsigned int		bli_recur;	/*                      */
	atomic_t		bli_refcount;	/*                */
	int			bli_format_count;	/*                  */
	struct xfs_buf_log_format *bli_formats;	/*                             */
	struct xfs_buf_log_format __bli_format;	/*                        */
} xfs_buf_log_item_t;

void	xfs_buf_item_init(struct xfs_buf *, struct xfs_mount *);
void	xfs_buf_item_relse(struct xfs_buf *);
void	xfs_buf_item_log(xfs_buf_log_item_t *, uint, uint);
uint	xfs_buf_item_dirty(xfs_buf_log_item_t *);
void	xfs_buf_attach_iodone(struct xfs_buf *,
			      void(*)(struct xfs_buf *, xfs_log_item_t *),
			      xfs_log_item_t *);
void	xfs_buf_iodone_callbacks(struct xfs_buf *);
void	xfs_buf_iodone(struct xfs_buf *, struct xfs_log_item *);

void	xfs_trans_buf_set_type(struct xfs_trans *, struct xfs_buf *,
			       enum xfs_blft);
void	xfs_trans_buf_copy_type(struct xfs_buf *dst_bp, struct xfs_buf *src_bp);

#endif	/*            */

#endif	/*                    */
