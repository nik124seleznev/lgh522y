/*
                        
  
                                        
  
                                                       
 */

/*          

                
                                                                            
                                                       

                                                                        
                                                                  

                                                                         
                                                                              
                                          */

#if !defined(__LITTLE_ENDIAN) && !defined(__BIG_ENDIAN)
#error unknown endian
#endif

/*          */

typedef u32 secno;			/*                                   */

typedef secno dnode_secno;		/*                          */
typedef secno fnode_secno;		/*                           */
typedef secno anode_secno;		/*                           */

typedef u32 time32_t;		/*                    */

/*          */

/*                                                              
                                                                   */

#define BB_MAGIC 0xaa55

struct hpfs_boot_block
{
  u8 jmp[3];
  u8 oem_id[8];
  u8 bytes_per_sector[2];	/*     */
  u8 sectors_per_cluster;
  u8 n_reserved_sectors[2];
  u8 n_fats;
  u8 n_rootdir_entries[2];
  u8 n_sectors_s[2];
  u8 media_byte;
  __le16 sectors_per_fat;
  __le16 sectors_per_track;
  __le16 heads_per_cyl;
  __le32 n_hidden_sectors;
  __le32 n_sectors_l;		/*                   */
  u8 drive_number;
  u8 mbz;
  u8 sig_28h;			/*     */
  u8 vol_serno[4];
  u8 vol_label[11];
  u8 sig_hpfs[8];		/*            */
  u8 pad[448];
  __le16 magic;			/*      */
};


/*           */

/*                                                        */

#define SB_MAGIC 0xf995e849

struct hpfs_super_block
{
  __le32 magic;				/*           */
  __le32 magic1;			/*                        */
  u8 version;				/*                                    */
  u8 funcversion;			/*                                    
                                           
                  */
  __le16 zero;				/*   */
  __le32 root;				/*                         */
  __le32 n_sectors;			/*                    */
  __le32 n_badblocks;			/*                      */
  __le32 bitmaps;			/*                                 */
  __le32 zero1;				/*   */
  __le32 badblocks;			/*                */
  __le32 zero3;				/*   */
  __le32 last_chkdsk;			/*                               */
  __le32 last_optimize;			/*                                 */
  __le32 n_dir_band;			/*                               */
  __le32 dir_band_start;			/*                          */
  __le32 dir_band_end;			/*                         */
  __le32 dir_band_bitmap;		/*                                 */
  u8 volume_name[32];			/*          */
  __le32 user_id_table;			/*                                  */
  u32 zero6[103];			/*   */
};


/*           */

/*                                                 */

#define SP_MAGIC 0xf9911849

struct hpfs_spare_block
{
  __le32 magic;				/*           */
  __le32 magic1;				/*                        */

#ifdef __LITTLE_ENDIAN
  u8 dirty: 1;				/*                                 */
  u8 sparedir_used: 1;			/*                    */
  u8 hotfixes_used: 1;			/*               */
  u8 bad_sector: 1;			/*                                  */
  u8 bad_bitmap: 1;			/*            */
  u8 fast: 1;				/*                              */
  u8 old_wrote: 1;			/*                              */
  u8 old_wrote_1: 1;			/*                                  */
#else
  u8 old_wrote_1: 1;			/*                                  */
  u8 old_wrote: 1;			/*                              */
  u8 fast: 1;				/*                              */
  u8 bad_bitmap: 1;			/*            */
  u8 bad_sector: 1;			/*                                  */
  u8 hotfixes_used: 1;			/*               */
  u8 sparedir_used: 1;			/*                    */
  u8 dirty: 1;				/*                                 */
#endif

#ifdef __LITTLE_ENDIAN
  u8 install_dasd_limits: 1;		/*               */
  u8 resynch_dasd_limits: 1;
  u8 dasd_limits_operational: 1;
  u8 multimedia_active: 1;
  u8 dce_acls_active: 1;
  u8 dasd_limits_dirty: 1;
  u8 flag67: 2;
#else
  u8 flag67: 2;
  u8 dasd_limits_dirty: 1;
  u8 dce_acls_active: 1;
  u8 multimedia_active: 1;
  u8 dasd_limits_operational: 1;
  u8 resynch_dasd_limits: 1;
  u8 install_dasd_limits: 1;		/*               */
#endif

  u8 mm_contlgulty;
  u8 unused;

  __le32 hotfix_map;			/*                                 */
  __le32 n_spares_used;			/*                    */
  __le32 n_spares;			/*                                */
  __le32 n_dnode_spares_free;		/*                     */
  __le32 n_dnode_spares;		/*                               
                             */
  __le32 code_page_dir;			/*                           */
  __le32 n_code_pages;			/*                      */
  __le32 super_crc;			/*                                  
                                           
                    */
  __le32 spare_crc;			/*                                   */
  __le32 zero1[15];			/*        */
  __le32 spare_dnodes[100];		/*                           */
  __le32 zero2[1];			/*                */
};

/*                                                                    
                                                          
                                  */

#define BAD_MAGIC 0
       
/*                                                 

                            
                          

                                                                       
                                                                      
                                                                      
                                                      */


/*                                               
                                                                      */


/*                                                                      
                                                                     
                                                                   
                                           */

/*                                               */

#define CP_DIR_MAGIC 0x494521f7

struct code_page_directory
{
  __le32 magic;				/*           */
  __le32 n_code_pages;			/*                              */
  __le32 zero1[2];
  struct {
    __le16 ix;				/*       */
    __le16 code_page_number;		/*                  */
    __le32 bounds;			/*                           
                      */
    __le32 code_page_data;		/*                                  
                              */
    __le16 index;			/*                                   */
    __le16 unknown;			/*                               
                                  */
  } array[31];				/*                */
};

/*                                          */

#define CP_DATA_MAGIC 0x894521f7

struct code_page_data
{
  __le32 magic;				/*           */
  __le32 n_used;			/*                               */
  __le32 bounds[3];			/*                 
                                  
                      */
  __le16 offs[3];			/*                             
                                 */
  struct {
    __le16 ix;				/*       */
    __le16 code_page_number;		/*                  */
    __le16 unknown;			/*                             */
    u8 map[128];			/*                               */
    __le16 zero2;
  } code_page[3];
  u8 incognita[78];
};


/*                                                            
                                                                     
                                                                        

                                                                            
                                

                                                         
                                                
                                                
                                                                              
                                                                           

                                                                           
                                                                         
                               

                                                                      
                                                                       
                                                                       
                                                                        
                                                                      
                                                                    
                           */


/*                                   */

/*                                                            
                                                                  
                                                                     
                                    */

#define DNODE_MAGIC   0x77e40aae

struct dnode {
  __le32 magic;				/*           */
  __le32 first_free;			/*                              
                             */
#ifdef __LITTLE_ENDIAN
  u8 root_dnode: 1;			/*                   */
  u8 increment_me: 7;			/*                                */
					/*                                  
                                */
#else
  u8 increment_me: 7;			/*                                */
					/*                                  
                                */
  u8 root_dnode: 1;			/*                   */
#endif
  u8 increment_me2[3];
  __le32 up;				/*                               
                               */
  __le32 self;			/*                       */
  u8 dirent[2028];			/*                     */
};

struct hpfs_dirent {
  __le16 length;			/*                       */

#ifdef __LITTLE_ENDIAN
  u8 first: 1;				/*                               */
  u8 has_acl: 1;
  u8 down: 1;				/*                                   */
  u8 last: 1;				/*                         */
  u8 has_ea: 1;				/*              */
  u8 has_xtd_perm: 1;			/*                              */
  u8 has_explicit_acl: 1;
  u8 has_needea: 1;			/*                          
                                  
                                   */
#else
  u8 has_needea: 1;			/*                          
                                  
                                   */
  u8 has_explicit_acl: 1;
  u8 has_xtd_perm: 1;			/*                              */
  u8 has_ea: 1;				/*              */
  u8 last: 1;				/*                         */
  u8 down: 1;				/*                                   */
  u8 has_acl: 1;
  u8 first: 1;				/*                               */
#endif

#ifdef __LITTLE_ENDIAN
  u8 read_only: 1;			/*            */
  u8 hidden: 1;				/*            */
  u8 system: 1;				/*            */
  u8 flag11: 1;				/*                                  */
  u8 directory: 1;			/*            */
  u8 archive: 1;			/*            */
  u8 not_8x3: 1;			/*                 */
  u8 flag15: 1;
#else
  u8 flag15: 1;
  u8 not_8x3: 1;			/*                 */
  u8 archive: 1;			/*            */
  u8 directory: 1;			/*            */
  u8 flag11: 1;				/*                                  */
  u8 system: 1;				/*            */
  u8 hidden: 1;				/*            */
  u8 read_only: 1;			/*            */
#endif

  __le32 fnode;				/*                              */
  __le32 write_date;			/*       */
  __le32 file_size;			/*                    */
  __le32 read_date;			/*       */
  __le32 creation_date;			/*       */
  __le32 ea_size;			/*                        */
  u8 no_of_acls;			/*                              */
  u8 ix;				/*                                   
                              */
  u8 namelen, name[1];			/*           */
  /*                                                    
                                                         
                                                        */
};


/*                                               */

/*                                                                     
                                                                     
                                                                     
                                                           
                                                                   
                                                                          
                                                                    
                                                                      */

struct bplus_leaf_node
{
  __le32 file_secno;			/*                             */
  __le32 length;			/*                 */
  __le32 disk_secno;			/*                                 */
};

struct bplus_internal_node
{
  __le32 file_secno;			/*                              */
  __le32 down;				/*                    */
};

enum {
	BP_hbff = 1,
	BP_fnode_parent = 0x20,
	BP_binary_search = 0x40,
	BP_internal = 0x80
};
struct bplus_header
{
  u8 flags;				/*                                            
                                             
                                        
                                        
                                              
                                              
                                     */
  u8 fill[3];
  u8 n_free_nodes;			/*                               */
  u8 n_used_nodes;			/*                               */
  __le16 first_free;			/*                               
                                 */
  union {
    struct bplus_internal_node internal[0]; /*                                 
                             */
    struct bplus_leaf_node external[0];	    /*                                 
                        */
  } u;
};

static inline bool bp_internal(struct bplus_header *bp)
{
	return bp->flags & BP_internal;
}

static inline bool bp_fnode_parent(struct bplus_header *bp)
{
	return bp->flags & BP_fnode_parent;
}

/*                                             */

/*                                                                          
                                                                            
                                                                            */

#define FNODE_MAGIC 0xf7e40aae

enum {FNODE_anode = cpu_to_le16(2), FNODE_dir = cpu_to_le16(256)};
struct fnode
{
  __le32 magic;				/*           */
  __le32 zero1[2];			/*              */
  u8 len, name[15];			/*                             */
  __le32 up;				/*                                   */
  __le32 acl_size_l;
  __le32 acl_secno;
  __le16 acl_size_s;
  u8 acl_anode;
  u8 zero2;				/*                   */
  __le32 ea_size_l;			/*                              */
  __le32 ea_secno;			/*                                   */
  __le16 ea_size_s;			/*                               */

  __le16 flags;				/*                                   */
					/*                                             
                         */
  struct bplus_header btree;		/*                                   */
  union {
    struct bplus_leaf_node external[8];
    struct bplus_internal_node internal[12];
  } u;

  __le32 file_size;			/*                    */
  __le32 n_needea;			/*                                */
  u8 user_id[16];			/*        */
  __le16 ea_offs;			/*                           
                                   */
  u8 dasd_limit_treshhold;
  u8 dasd_limit_delta;
  __le32 dasd_limit;
  __le32 dasd_usage;
  u8 ea[316];				/*                                   
                                  
                                       
                                       */
};

static inline bool fnode_in_anode(struct fnode *p)
{
	return (p->flags & FNODE_anode) != 0;
}

static inline bool fnode_is_dir(struct fnode *p)
{
	return (p->flags & FNODE_dir) != 0;
}


/*                                    */

#define ANODE_MAGIC 0x37e40aae

struct anode
{
  __le32 magic;				/*           */
  __le32 self;				/*                       */
  __le32 up;				/*                       */

  struct bplus_header btree;		/*                                   */
  union {
    struct bplus_leaf_node external[40];
    struct bplus_internal_node internal[60];
  } u;

  __le32 fill[3];			/*        */
};


/*                     

                                                                     
                                                                    
                                                                       
                       

                                                                        
                                                                        
                                                                      

                                                                          
                                                                         
                                                       */

enum {EA_indirect = 1, EA_anode = 2, EA_needea = 128 };
struct extended_attribute
{
  u8 flags;				/*                                       
                                */
					/*                                
                                        */
					/*                          */
  u8 namelen;				/*                       */
  u8 valuelen_lo;			/*                        */
  u8 valuelen_hi;			/*                        */
  u8 name[];
  /*
                                         
                                            
                                          
                                                       
                                                 
                                                     
                                                                         
                                  
  */
};

static inline bool ea_indirect(struct extended_attribute *ea)
{
	return ea->flags & EA_indirect;
}

static inline bool ea_in_anode(struct extended_attribute *ea)
{
	return ea->flags & EA_anode;
}

/*
                   
                     
       
*/
