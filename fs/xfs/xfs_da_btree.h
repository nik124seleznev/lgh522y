/*
 * Copyright (c) 2000,2002,2005 Silicon Graphics, Inc.
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
#ifndef __XFS_DA_BTREE_H__
#define	__XFS_DA_BTREE_H__

struct xfs_bmap_free;
struct xfs_inode;
struct xfs_trans;
struct zone;

/*                                                                        
                                                              
                                                                          */

/*
                                                                               
  
                                                                      
                                                                   
 */
#define XFS_DA_NODE_MAGIC	0xfebe	/*                               */
#define XFS_ATTR_LEAF_MAGIC	0xfbee	/*                                   */
#define	XFS_DIR2_LEAF1_MAGIC	0xd2f1	/*                                    */
#define	XFS_DIR2_LEAFN_MAGIC	0xd2ff	/*                                   */

typedef struct xfs_da_blkinfo {
	__be32		forw;			/*                        */
	__be32		back;			/*                         */
	__be16		magic;			/*                         */
	__be16		pad;			/*        */
} xfs_da_blkinfo_t;

/*
                                        
  
                                                                           
                                                                            
                                                                             
                                                               
 */
#define XFS_DA3_NODE_MAGIC	0x3ebe	/*                               */
#define XFS_ATTR3_LEAF_MAGIC	0x3bee	/*                                   */
#define	XFS_DIR3_LEAF1_MAGIC	0x3df1	/*                                    */
#define	XFS_DIR3_LEAFN_MAGIC	0x3dff	/*                                   */

struct xfs_da3_blkinfo {
	/*
                                                                     
                                                                    
                                                         
  */
	struct xfs_da_blkinfo	hdr;
	__be32			crc;	/*              */
	__be64			blkno;	/*                           */
	__be64			lsn;	/*                               */
	uuid_t			uuid;	/*                         */
	__be64			owner;	/*                           */
};

/*
                                                                         
                                    
  
                          
  
                                                                      
                                                          
 */
#define	XFS_DA_NODE_MAXDEPTH	5	/*                    */

typedef struct xfs_da_node_hdr {
	struct xfs_da_blkinfo	info;	/*                         */
	__be16			__count; /*                         */
	__be16			__level; /*                                */
} xfs_da_node_hdr_t;

struct xfs_da3_node_hdr {
	struct xfs_da3_blkinfo	info;	/*                         */
	__be16			__count; /*                         */
	__be16			__level; /*                                */
	__be32			__pad32;
};

#define XFS_DA3_NODE_CRC_OFF	(offsetof(struct xfs_da3_node_hdr, info.crc))

typedef struct xfs_da_node_entry {
	__be32	hashval;	/*                                */
	__be32	before;		/*                             */
} xfs_da_node_entry_t;

typedef struct xfs_da_intnode {
	struct xfs_da_node_hdr	hdr;
	struct xfs_da_node_entry __btree[];
} xfs_da_intnode_t;

struct xfs_da3_intnode {
	struct xfs_da3_node_hdr	hdr;
	struct xfs_da_node_entry __btree[];
};

/*
                                                                               
                                                                                
               
 */
struct xfs_da3_icnode_hdr {
	__uint32_t	forw;
	__uint32_t	back;
	__uint16_t	magic;
	__uint16_t	count;
	__uint16_t	level;
};

extern void xfs_da3_node_hdr_from_disk(struct xfs_da3_icnode_hdr *to,
				       struct xfs_da_intnode *from);
extern void xfs_da3_node_hdr_to_disk(struct xfs_da_intnode *to,
				     struct xfs_da3_icnode_hdr *from);

static inline int
xfs_da3_node_hdr_size(struct xfs_da_intnode *dap)
{
	if (dap->hdr.info.magic == cpu_to_be16(XFS_DA3_NODE_MAGIC))
		return sizeof(struct xfs_da3_node_hdr);
	return sizeof(struct xfs_da_node_hdr);
}

static inline struct xfs_da_node_entry *
xfs_da3_node_tree_p(struct xfs_da_intnode *dap)
{
	if (dap->hdr.info.magic == cpu_to_be16(XFS_DA3_NODE_MAGIC)) {
		struct xfs_da3_intnode *dap3 = (struct xfs_da3_intnode *)dap;
		return dap3->__btree;
	}
	return dap->__btree;
}

extern void xfs_da3_intnode_from_disk(struct xfs_da3_icnode_hdr *to,
				      struct xfs_da_intnode *from);
extern void xfs_da3_intnode_to_disk(struct xfs_da_intnode *to,
				    struct xfs_da3_icnode_hdr *from);

#define	XFS_LBSIZE(mp)	(mp)->m_sb.sb_blocksize

/*                                                                        
                                                          
                                                                          */

/*
                            
 */
enum xfs_dacmp {
	XFS_CMP_DIFFERENT,	/*                                */
	XFS_CMP_EXACT,		/*                            */
	XFS_CMP_CASE		/*                                   */
};

/*
                                                    
 */
typedef struct xfs_da_args {
	const __uint8_t	*name;		/*                                    */
	int		namelen;	/*                                  */
	__uint8_t	*value;		/*                                    */
	int		valuelen;	/*                 */
	int		flags;		/*                                    */
	xfs_dahash_t	hashval;	/*                    */
	xfs_ino_t	inumber;	/*                           */
	struct xfs_inode *dp;		/*                               */
	xfs_fsblock_t	*firstblock;	/*                                  */
	struct xfs_bmap_free *flist;	/*                                 */
	struct xfs_trans *trans;	/*                                   */
	xfs_extlen_t	total;		/*                                   */
	int		whichfork;	/*                        */
	xfs_dablk_t	blkno;		/*                                */
	int		index;		/*                                  */
	xfs_dablk_t	rmtblkno;	/*                                  */
	int		rmtblkcnt;	/*                               */
	xfs_dablk_t	blkno2;		/*                                    */
	int		index2;		/*                          */
	xfs_dablk_t	rmtblkno2;	/*                                  */
	int		rmtblkcnt2;	/*                               */
	int		op_flags;	/*                 */
	enum xfs_dacmp	cmpresult;	/*                                 */
} xfs_da_args_t;

/*
                   
 */
#define XFS_DA_OP_JUSTCHECK	0x0001	/*                            */
#define XFS_DA_OP_RENAME	0x0002	/*                             */
#define XFS_DA_OP_ADDNAME	0x0004	/*                          */
#define XFS_DA_OP_OKNOENT	0x0008	/*                                    */
#define XFS_DA_OP_CILOOKUP	0x0010	/*                                   */

#define XFS_DA_OP_FLAGS \
	{ XFS_DA_OP_JUSTCHECK,	"JUSTCHECK" }, \
	{ XFS_DA_OP_RENAME,	"RENAME" }, \
	{ XFS_DA_OP_ADDNAME,	"ADDNAME" }, \
	{ XFS_DA_OP_OKNOENT,	"OKNOENT" }, \
	{ XFS_DA_OP_CILOOKUP,	"CILOOKUP" }

/*
                                                                      
  
                                                                      
                                                                         
                                      
 */
typedef struct xfs_da_state_blk {
	struct xfs_buf	*bp;		/*                         */
	xfs_dablk_t	blkno;		/*                            */
	xfs_daddr_t	disk_blkno;	/*                                  */
	int		index;		/*                           */
	xfs_dahash_t	hashval;	/*                          */
	int		magic;		/*                                  */
} xfs_da_state_blk_t;

typedef struct xfs_da_state_path {
	int			active;		/*                         */
	xfs_da_state_blk_t	blk[XFS_DA_NODE_MAXDEPTH];
} xfs_da_state_path_t;

typedef struct xfs_da_state {
	xfs_da_args_t		*args;		/*                    */
	struct xfs_mount	*mp;		/*                        */
	unsigned int		blocksize;	/*                    */
	unsigned int		node_ents;	/*                            */
	xfs_da_state_path_t	path;		/*                    */
	xfs_da_state_path_t	altpath;	/*                         */
	unsigned char		inleaf;		/*                            */
	unsigned char		extravalid;	/*                         */
	unsigned char		extraafter;	/*                            */
	xfs_da_state_blk_t	extrablk;	/*                             */
						/*                            */
} xfs_da_state_t;

/*
                                                             
 */
#define XFS_DA_LOGOFF(BASE, ADDR)	((char *)(ADDR) - (char *)(BASE))
#define XFS_DA_LOGRANGE(BASE, ADDR, SIZE)	\
		(uint)(XFS_DA_LOGOFF(BASE, ADDR)), \
		(uint)(XFS_DA_LOGOFF(BASE, ADDR)+(SIZE)-1)

/*
                                                     
 */
struct xfs_nameops {
	xfs_dahash_t	(*hashname)(struct xfs_name *);
	enum xfs_dacmp	(*compname)(struct xfs_da_args *,
					const unsigned char *, int);
};


/*                                                                        
                       
                                                                          */

/*
                                       
 */
int	xfs_da3_node_create(struct xfs_da_args *args, xfs_dablk_t blkno,
			    int level, struct xfs_buf **bpp, int whichfork);
int	xfs_da3_split(xfs_da_state_t *state);

/*
                                         
 */
int	xfs_da3_join(xfs_da_state_t *state);
void	xfs_da3_fixhashpath(struct xfs_da_state *state,
			    struct xfs_da_state_path *path_to_to_fix);

/*
                                                 
 */
int	xfs_da3_node_lookup_int(xfs_da_state_t *state, int *result);
int	xfs_da3_path_shift(xfs_da_state_t *state, xfs_da_state_path_t *path,
					 int forward, int release, int *result);
/*
                    
 */
int	xfs_da3_blk_link(xfs_da_state_t *state, xfs_da_state_blk_t *old_blk,
				       xfs_da_state_blk_t *new_blk);
int	xfs_da3_node_read(struct xfs_trans *tp, struct xfs_inode *dp,
			 xfs_dablk_t bno, xfs_daddr_t mappedbno,
			 struct xfs_buf **bpp, int which_fork);

extern const struct xfs_buf_ops xfs_da3_node_buf_ops;

/*
                    
 */
int	xfs_da_grow_inode(xfs_da_args_t *args, xfs_dablk_t *new_blkno);
int	xfs_da_grow_inode_int(struct xfs_da_args *args, xfs_fileoff_t *bno,
			      int count);
int	xfs_da_get_buf(struct xfs_trans *trans, struct xfs_inode *dp,
			      xfs_dablk_t bno, xfs_daddr_t mappedbno,
			      struct xfs_buf **bp, int whichfork);
int	xfs_da_read_buf(struct xfs_trans *trans, struct xfs_inode *dp,
			       xfs_dablk_t bno, xfs_daddr_t mappedbno,
			       struct xfs_buf **bpp, int whichfork,
			       const struct xfs_buf_ops *ops);
xfs_daddr_t	xfs_da_reada_buf(struct xfs_trans *trans, struct xfs_inode *dp,
				xfs_dablk_t bno, xfs_daddr_t mapped_bno,
				int whichfork, const struct xfs_buf_ops *ops);
int	xfs_da_shrink_inode(xfs_da_args_t *args, xfs_dablk_t dead_blkno,
					  struct xfs_buf *dead_buf);

uint xfs_da_hashname(const __uint8_t *name_string, int name_length);
enum xfs_dacmp xfs_da_compname(struct xfs_da_args *args,
				const unsigned char *name, int len);


xfs_da_state_t *xfs_da_state_alloc(void);
void xfs_da_state_free(xfs_da_state_t *state);

extern struct kmem_zone *xfs_da_state_zone;
extern const struct xfs_nameops xfs_default_nameops;

#endif	/*                    */
