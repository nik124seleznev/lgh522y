/*
 * Copyright (c) 2000,2002-2003,2005 Silicon Graphics, Inc.
 * Copyright (c) 2013 Red Hat, Inc.
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
#ifndef __XFS_ATTR_LEAF_H__
#define	__XFS_ATTR_LEAF_H__

/*
                                                                    
  
                                                                  
                                                                           
                                                                        
                                                                           
                                                                       
 */

struct attrlist;
struct attrlist_cursor_kern;
struct xfs_attr_list_context;
struct xfs_da_args;
struct xfs_da_state;
struct xfs_da_state_blk;
struct xfs_inode;
struct xfs_trans;

/*                                                                        
                                                          
                                                                          */

/*
                                                        
  
                                                                          
                                                                              
                                                                          
                                                                        
                                                                          
                                                                       
                                                                             
  
                                                                        
                                                                        
                                                                       
                                                                        
                                                                   
  
                                                                         
                                                                            
                                                                          
  
                                                                         
                                                                         
                                                                          
                                                                          
                                                                             
                                                                           
 */
#define XFS_ATTR_LEAF_MAPSIZE	3	/*                          */

typedef struct xfs_attr_leaf_map {	/*                       */
	__be16	base;			  /*                     */
	__be16	size;			  /*                       */
} xfs_attr_leaf_map_t;

typedef struct xfs_attr_leaf_hdr {	/*                                 */
	xfs_da_blkinfo_t info;		/*                         */
	__be16	count;			/*                              */
	__be16	usedbytes;		/*                                  */
	__be16	firstused;		/*                              */
	__u8	holes;			/*                              */
	__u8	pad1;
	xfs_attr_leaf_map_t freemap[XFS_ATTR_LEAF_MAPSIZE];
					/*                        */
} xfs_attr_leaf_hdr_t;

typedef struct xfs_attr_leaf_entry {	/*                         */
	__be32	hashval;		/*                    */
	__be16	nameidx;		/*                                 */
	__u8	flags;			/*                                   */
	__u8	pad2;			/*                 */
} xfs_attr_leaf_entry_t;

typedef struct xfs_attr_leaf_name_local {
	__be16	valuelen;		/*                          */
	__u8	namelen;		/*                      */
	__u8	nameval[1];		/*                  */
} xfs_attr_leaf_name_local_t;

typedef struct xfs_attr_leaf_name_remote {
	__be32	valueblk;		/*                             */
	__be32	valuelen;		/*                          */
	__u8	namelen;		/*                      */
	__u8	name[1];		/*            */
} xfs_attr_leaf_name_remote_t;

typedef struct xfs_attr_leafblock {
	xfs_attr_leaf_hdr_t	hdr;	/*                                 */
	xfs_attr_leaf_entry_t	entries[1];	/*                         */
	xfs_attr_leaf_name_local_t namelist;	/*                          */
	xfs_attr_leaf_name_remote_t valuelist;	/*                          */
} xfs_attr_leafblock_t;

/*
                                                                          
                                                                             
                                                                            
 */
struct xfs_attr3_leaf_hdr {
	struct xfs_da3_blkinfo	info;
	__be16			count;
	__be16			usedbytes;
	__be16			firstused;
	__u8			holes;
	__u8			pad1;
	struct xfs_attr_leaf_map freemap[XFS_ATTR_LEAF_MAPSIZE];
	__be32			pad2;		/*                  */
};

#define XFS_ATTR3_LEAF_CRC_OFF	(offsetof(struct xfs_attr3_leaf_hdr, info.crc))

struct xfs_attr3_leafblock {
	struct xfs_attr3_leaf_hdr	hdr;
	struct xfs_attr_leaf_entry	entries[1];

	/*
                                                                     
                                                                     
                                                                    
   
                                   
                                    
  */
};

/*
                                                       
 */
struct xfs_attr3_icleaf_hdr {
	__uint32_t	forw;
	__uint32_t	back;
	__uint16_t	magic;
	__uint16_t	count;
	__uint16_t	usedbytes;
	__uint16_t	firstused;
	__u8		holes;
	struct {
		__uint16_t	base;
		__uint16_t	size;
	} freemap[XFS_ATTR_LEAF_MAPSIZE];
};

/*
                                               
                                                                          
                                                                       
 */
#define	XFS_ATTR_LOCAL_BIT	0	/*                        */
#define	XFS_ATTR_ROOT_BIT	1	/*                               */
#define	XFS_ATTR_SECURE_BIT	2	/*                              */
#define	XFS_ATTR_INCOMPLETE_BIT	7	/*                                 */
#define XFS_ATTR_LOCAL		(1 << XFS_ATTR_LOCAL_BIT)
#define XFS_ATTR_ROOT		(1 << XFS_ATTR_ROOT_BIT)
#define XFS_ATTR_SECURE		(1 << XFS_ATTR_SECURE_BIT)
#define XFS_ATTR_INCOMPLETE	(1 << XFS_ATTR_INCOMPLETE_BIT)

/*
                                                                      
                   
 */
#define XFS_ATTR_NSP_ARGS_MASK		(ATTR_ROOT | ATTR_SECURE)
#define XFS_ATTR_NSP_ONDISK_MASK	(XFS_ATTR_ROOT | XFS_ATTR_SECURE)
#define XFS_ATTR_NSP_ONDISK(flags)	((flags) & XFS_ATTR_NSP_ONDISK_MASK)
#define XFS_ATTR_NSP_ARGS(flags)	((flags) & XFS_ATTR_NSP_ARGS_MASK)
#define XFS_ATTR_NSP_ARGS_TO_ONDISK(x)	(((x) & ATTR_ROOT ? XFS_ATTR_ROOT : 0) |\
					 ((x) & ATTR_SECURE ? XFS_ATTR_SECURE : 0))
#define XFS_ATTR_NSP_ONDISK_TO_ARGS(x)	(((x) & XFS_ATTR_ROOT ? ATTR_ROOT : 0) |\
					 ((x) & XFS_ATTR_SECURE ? ATTR_SECURE : 0))

/*
                                                                     
                                         
 */
#define	XFS_ATTR_LEAF_NAME_ALIGN	((uint)sizeof(xfs_dablk_t))

static inline int
xfs_attr3_leaf_hdr_size(struct xfs_attr_leafblock *leafp)
{
	if (leafp->hdr.info.magic == cpu_to_be16(XFS_ATTR3_LEAF_MAGIC))
		return sizeof(struct xfs_attr3_leaf_hdr);
	return sizeof(struct xfs_attr_leaf_hdr);
}

static inline struct xfs_attr_leaf_entry *
xfs_attr3_leaf_entryp(xfs_attr_leafblock_t *leafp)
{
	if (leafp->hdr.info.magic == cpu_to_be16(XFS_ATTR3_LEAF_MAGIC))
		return &((struct xfs_attr3_leafblock *)leafp)->entries[0];
	return &leafp->entries[0];
}

/*
                                                                   
 */
static inline char *
xfs_attr3_leaf_name(xfs_attr_leafblock_t *leafp, int idx)
{
	struct xfs_attr_leaf_entry *entries = xfs_attr3_leaf_entryp(leafp);

	return &((char *)leafp)[be16_to_cpu(entries[idx].nameidx)];
}

static inline xfs_attr_leaf_name_remote_t *
xfs_attr3_leaf_name_remote(xfs_attr_leafblock_t *leafp, int idx)
{
	return (xfs_attr_leaf_name_remote_t *)xfs_attr3_leaf_name(leafp, idx);
}

static inline xfs_attr_leaf_name_local_t *
xfs_attr3_leaf_name_local(xfs_attr_leafblock_t *leafp, int idx)
{
	return (xfs_attr_leaf_name_local_t *)xfs_attr3_leaf_name(leafp, idx);
}

/*
                                                                        
                                                                       
                                   
 */
static inline int xfs_attr_leaf_entsize_remote(int nlen)
{
	return ((uint)sizeof(xfs_attr_leaf_name_remote_t) - 1 + (nlen) + \
		XFS_ATTR_LEAF_NAME_ALIGN - 1) & ~(XFS_ATTR_LEAF_NAME_ALIGN - 1);
}

static inline int xfs_attr_leaf_entsize_local(int nlen, int vlen)
{
	return ((uint)sizeof(xfs_attr_leaf_name_local_t) - 1 + (nlen) + (vlen) +
		XFS_ATTR_LEAF_NAME_ALIGN - 1) & ~(XFS_ATTR_LEAF_NAME_ALIGN - 1);
}

static inline int xfs_attr_leaf_entsize_local_max(int bsize)
{
	return (((bsize) >> 1) + ((bsize) >> 2));
}

/*
                                                                         
 */
typedef struct xfs_attr_inactive_list {
	xfs_dablk_t	valueblk;	/*                             */
	int		valuelen;	/*                          */
} xfs_attr_inactive_list_t;


/*                                                                        
                                      
                                                                          */

/*
                                                               
 */
void	xfs_attr_shortform_create(struct xfs_da_args *args);
void	xfs_attr_shortform_add(struct xfs_da_args *args, int forkoff);
int	xfs_attr_shortform_lookup(struct xfs_da_args *args);
int	xfs_attr_shortform_getvalue(struct xfs_da_args *args);
int	xfs_attr_shortform_to_leaf(struct xfs_da_args *args);
int	xfs_attr_shortform_remove(struct xfs_da_args *args);
int	xfs_attr_shortform_list(struct xfs_attr_list_context *context);
int	xfs_attr_shortform_allfit(struct xfs_buf *bp, struct xfs_inode *dp);
int	xfs_attr_shortform_bytesfit(xfs_inode_t *dp, int bytes);


/*
                                                                
 */
int	xfs_attr3_leaf_to_node(struct xfs_da_args *args);
int	xfs_attr3_leaf_to_shortform(struct xfs_buf *bp,
				   struct xfs_da_args *args, int forkoff);
int	xfs_attr3_leaf_clearflag(struct xfs_da_args *args);
int	xfs_attr3_leaf_setflag(struct xfs_da_args *args);
int	xfs_attr3_leaf_flipflags(struct xfs_da_args *args);

/*
                                       
 */
int	xfs_attr3_leaf_split(struct xfs_da_state *state,
				   struct xfs_da_state_blk *oldblk,
				   struct xfs_da_state_blk *newblk);
int	xfs_attr3_leaf_lookup_int(struct xfs_buf *leaf,
					struct xfs_da_args *args);
int	xfs_attr3_leaf_getvalue(struct xfs_buf *bp, struct xfs_da_args *args);
int	xfs_attr3_leaf_add(struct xfs_buf *leaf_buffer,
				 struct xfs_da_args *args);
int	xfs_attr3_leaf_remove(struct xfs_buf *leaf_buffer,
				    struct xfs_da_args *args);
int	xfs_attr3_leaf_list_int(struct xfs_buf *bp,
				      struct xfs_attr_list_context *context);

/*
                                         
 */
int	xfs_attr3_leaf_toosmall(struct xfs_da_state *state, int *retval);
void	xfs_attr3_leaf_unbalance(struct xfs_da_state *state,
				       struct xfs_da_state_blk *drop_blk,
				       struct xfs_da_state_blk *save_blk);
int	xfs_attr3_root_inactive(struct xfs_trans **trans, struct xfs_inode *dp);

/*
                    
 */
xfs_dahash_t	xfs_attr_leaf_lasthash(struct xfs_buf *bp, int *count);
int	xfs_attr_leaf_order(struct xfs_buf *leaf1_bp,
				   struct xfs_buf *leaf2_bp);
int	xfs_attr_leaf_newentsize(int namelen, int valuelen, int blocksize,
					int *local);
int	xfs_attr3_leaf_read(struct xfs_trans *tp, struct xfs_inode *dp,
			xfs_dablk_t bno, xfs_daddr_t mappedbno,
			struct xfs_buf **bpp);
void	xfs_attr3_leaf_hdr_from_disk(struct xfs_attr3_icleaf_hdr *to,
				     struct xfs_attr_leafblock *from);

extern const struct xfs_buf_ops xfs_attr3_leaf_buf_ops;

#endif	/*                     */
