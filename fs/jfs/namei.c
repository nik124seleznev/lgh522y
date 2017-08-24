/*
 *   Copyright (C) International Business Machines Corp., 2000-2004
 *   Portions Copyright (C) Christoph Hellwig, 2001-2002
 *
 *   This program is free software;  you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY;  without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 *   the GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program;  if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <linux/fs.h>
#include <linux/namei.h>
#include <linux/ctype.h>
#include <linux/quotaops.h>
#include <linux/exportfs.h>
#include "jfs_incore.h"
#include "jfs_superblock.h"
#include "jfs_inode.h"
#include "jfs_dinode.h"
#include "jfs_dmap.h"
#include "jfs_unicode.h"
#include "jfs_metapage.h"
#include "jfs_xattr.h"
#include "jfs_acl.h"
#include "jfs_debug.h"

/*
                     
 */
const struct dentry_operations jfs_ci_dentry_operations;

static s64 commitZeroLink(tid_t, struct inode *);

/*
                            
  
                                                                  
  
 */
static inline void free_ea_wmap(struct inode *inode)
{
	dxd_t *ea = &JFS_IP(inode)->ea;

	if (ea->flag & DXD_EXTENT) {
		/*                          */
		invalidate_dxd_metapages(inode, *ea);
		dbFree(inode, addressDXD(ea), lengthDXD(ea));
	}
	ea->flag = 0;
}

/*
                                      
  
                                                                
                                               
  
                                          
                               
                                   
                 
  
                                  
  
 */
static int jfs_create(struct inode *dip, struct dentry *dentry, umode_t mode,
		bool excl)
{
	int rc = 0;
	tid_t tid;		/*                */
	struct inode *ip = NULL;	/*                       */
	ino_t ino;
	struct component_name dname;	/*                      */
	struct btstack btstack;
	struct inode *iplist[2];
	struct tblock *tblk;

	jfs_info("jfs_create: dip:0x%p name:%s", dip, dentry->d_name.name);

	dquot_initialize(dip);

	/*
                                               
                                                     
  */
	if ((rc = get_UCSname(&dname, dentry)))
		goto out1;

	/*
                                                                     
                                                                    
                                                         
  */
	ip = ialloc(dip, mode);
	if (IS_ERR(ip)) {
		rc = PTR_ERR(ip);
		goto out2;
	}

	tid = txBegin(dip->i_sb, 0);

	mutex_lock_nested(&JFS_IP(dip)->commit_mutex, COMMIT_MUTEX_PARENT);
	mutex_lock_nested(&JFS_IP(ip)->commit_mutex, COMMIT_MUTEX_CHILD);

	rc = jfs_init_acl(tid, ip, dip);
	if (rc)
		goto out3;

	rc = jfs_init_security(tid, ip, dip, &dentry->d_name);
	if (rc) {
		txAbort(tid, 0);
		goto out3;
	}

	if ((rc = dtSearch(dip, &dname, &ino, &btstack, JFS_CREATE))) {
		jfs_err("jfs_create: dtSearch returned %d", rc);
		txAbort(tid, 0);
		goto out3;
	}

	tblk = tid_to_tblock(tid);
	tblk->xflag |= COMMIT_CREATE;
	tblk->ino = ip->i_ino;
	tblk->u.ixpxd = JFS_IP(ip)->ixpxd;

	iplist[0] = dip;
	iplist[1] = ip;

	/*
                                                       
  */
	xtInitRoot(tid, ip);

	/*
                                                        
                                               
  */
	ino = ip->i_ino;
	if ((rc = dtInsert(tid, dip, &dname, &ino, &btstack))) {
		if (rc == -EIO) {
			jfs_err("jfs_create: dtInsert returned -EIO");
			txAbort(tid, 1);	/*                        */
		} else
			txAbort(tid, 0);	/*                 */
		goto out3;
	}

	ip->i_op = &jfs_file_inode_operations;
	ip->i_fop = &jfs_file_operations;
	ip->i_mapping->a_ops = &jfs_aops;

	mark_inode_dirty(ip);

	dip->i_ctime = dip->i_mtime = CURRENT_TIME;

	mark_inode_dirty(dip);

	rc = txCommit(tid, 2, &iplist[0], 0);

      out3:
	txEnd(tid);
	mutex_unlock(&JFS_IP(ip)->commit_mutex);
	mutex_unlock(&JFS_IP(dip)->commit_mutex);
	if (rc) {
		free_ea_wmap(ip);
		clear_nlink(ip);
		unlock_new_inode(ip);
		iput(ip);
	} else {
		unlock_new_inode(ip);
		d_instantiate(dentry, ip);
	}

      out2:
	free_UCSname(&dname);

      out1:

	jfs_info("jfs_create: rc:%d", rc);
	return rc;
}


/*
                                     
  
                                                                   
                                               
  
                                          
                                      
                                   
  
                                  
  
        
                                                                      
 */
static int jfs_mkdir(struct inode *dip, struct dentry *dentry, umode_t mode)
{
	int rc = 0;
	tid_t tid;		/*                */
	struct inode *ip = NULL;	/*                       */
	ino_t ino;
	struct component_name dname;	/*                      */
	struct btstack btstack;
	struct inode *iplist[2];
	struct tblock *tblk;

	jfs_info("jfs_mkdir: dip:0x%p name:%s", dip, dentry->d_name.name);

	dquot_initialize(dip);

	/*
                                               
                                                     
  */
	if ((rc = get_UCSname(&dname, dentry)))
		goto out1;

	/*
                                                                     
                                                                    
                                                         
  */
	ip = ialloc(dip, S_IFDIR | mode);
	if (IS_ERR(ip)) {
		rc = PTR_ERR(ip);
		goto out2;
	}

	tid = txBegin(dip->i_sb, 0);

	mutex_lock_nested(&JFS_IP(dip)->commit_mutex, COMMIT_MUTEX_PARENT);
	mutex_lock_nested(&JFS_IP(ip)->commit_mutex, COMMIT_MUTEX_CHILD);

	rc = jfs_init_acl(tid, ip, dip);
	if (rc)
		goto out3;

	rc = jfs_init_security(tid, ip, dip, &dentry->d_name);
	if (rc) {
		txAbort(tid, 0);
		goto out3;
	}

	if ((rc = dtSearch(dip, &dname, &ino, &btstack, JFS_CREATE))) {
		jfs_err("jfs_mkdir: dtSearch returned %d", rc);
		txAbort(tid, 0);
		goto out3;
	}

	tblk = tid_to_tblock(tid);
	tblk->xflag |= COMMIT_CREATE;
	tblk->ino = ip->i_ino;
	tblk->u.ixpxd = JFS_IP(ip)->ixpxd;

	iplist[0] = dip;
	iplist[1] = ip;

	/*
                                                   
  */
	dtInitRoot(tid, ip, dip->i_ino);

	/*
                                                        
                                               
  */
	ino = ip->i_ino;
	if ((rc = dtInsert(tid, dip, &dname, &ino, &btstack))) {
		if (rc == -EIO) {
			jfs_err("jfs_mkdir: dtInsert returned -EIO");
			txAbort(tid, 1);	/*                        */
		} else
			txAbort(tid, 0);	/*                 */
		goto out3;
	}

	set_nlink(ip, 2);	/*         */
	ip->i_op = &jfs_dir_inode_operations;
	ip->i_fop = &jfs_dir_operations;

	mark_inode_dirty(ip);

	/*                               */
	inc_nlink(dip);		/*                               */
	dip->i_ctime = dip->i_mtime = CURRENT_TIME;
	mark_inode_dirty(dip);

	rc = txCommit(tid, 2, &iplist[0], 0);

      out3:
	txEnd(tid);
	mutex_unlock(&JFS_IP(ip)->commit_mutex);
	mutex_unlock(&JFS_IP(dip)->commit_mutex);
	if (rc) {
		free_ea_wmap(ip);
		clear_nlink(ip);
		unlock_new_inode(ip);
		iput(ip);
	} else {
		unlock_new_inode(ip);
		d_instantiate(dentry, ip);
	}

      out2:
	free_UCSname(&dname);


      out1:

	jfs_info("jfs_mkdir: rc:%d", rc);
	return rc;
}

/*
                               
  
                                             
  
                                
                                   
  
                                       
                                               
                           
  
        
                                                              
                                                                       
                                                                      
                                                               
                                                           
 */
static int jfs_rmdir(struct inode *dip, struct dentry *dentry)
{
	int rc;
	tid_t tid;		/*                */
	struct inode *ip = dentry->d_inode;
	ino_t ino;
	struct component_name dname;
	struct inode *iplist[2];
	struct tblock *tblk;

	jfs_info("jfs_rmdir: dip:0x%p name:%s", dip, dentry->d_name.name);

	/*                                  */
	dquot_initialize(dip);
	dquot_initialize(ip);

	/*                                       */
	if (!dtEmpty(ip)) {
		rc = -ENOTEMPTY;
		goto out;
	}

	if ((rc = get_UCSname(&dname, dentry))) {
		goto out;
	}

	tid = txBegin(dip->i_sb, 0);

	mutex_lock_nested(&JFS_IP(dip)->commit_mutex, COMMIT_MUTEX_PARENT);
	mutex_lock_nested(&JFS_IP(ip)->commit_mutex, COMMIT_MUTEX_CHILD);

	iplist[0] = dip;
	iplist[1] = ip;

	tblk = tid_to_tblock(tid);
	tblk->xflag |= COMMIT_DELETE;
	tblk->u.ip = ip;

	/*
                                                              
  */
	ino = ip->i_ino;
	if ((rc = dtDelete(tid, dip, &dname, &ino, JFS_REMOVE))) {
		jfs_err("jfs_rmdir: dtDelete returned %d", rc);
		if (rc == -EIO)
			txAbort(tid, 1);
		txEnd(tid);
		mutex_unlock(&JFS_IP(ip)->commit_mutex);
		mutex_unlock(&JFS_IP(dip)->commit_mutex);

		goto out2;
	}

	/*                                                   
                                                 
  */
	dip->i_ctime = dip->i_mtime = CURRENT_TIME;
	inode_dec_link_count(dip);

	/*
                                         
  */
	/*                                              */
	if (JFS_IP(ip)->ea.flag & DXD_EXTENT) {
		/*               */
		txEA(tid, ip, &JFS_IP(ip)->ea, NULL);
	}
	JFS_IP(ip)->ea.flag = 0;

	/*                                               */
	if (JFS_IP(ip)->acl.flag & DXD_EXTENT) {
		/*                */
		txEA(tid, ip, &JFS_IP(ip)->acl, NULL);
	}
	JFS_IP(ip)->acl.flag = 0;

	/*                                      */
	clear_nlink(ip);
	mark_inode_dirty(ip);

	rc = txCommit(tid, 2, &iplist[0], 0);

	txEnd(tid);

	mutex_unlock(&JFS_IP(ip)->commit_mutex);
	mutex_unlock(&JFS_IP(dip)->commit_mutex);

	/*
                                                               
                                   
  */
	if (test_cflag(COMMIT_Stale, dip)) {
		if (dip->i_size > 1)
			jfs_truncate_nolock(dip, 0);

		clear_cflag(COMMIT_Stale, dip);
	}

      out2:
	free_UCSname(&dname);

      out:
	jfs_info("jfs_rmdir: rc:%d", rc);
	return rc;
}

/*
                                
  
                                                         
                               
  
                                             
                                           
  
                                  
  
        
                                                              
                                                                 
                                                                 
                                                          
  
                                                
  
 */
static int jfs_unlink(struct inode *dip, struct dentry *dentry)
{
	int rc;
	tid_t tid;		/*                */
	struct inode *ip = dentry->d_inode;
	ino_t ino;
	struct component_name dname;	/*             */
	struct inode *iplist[2];
	struct tblock *tblk;
	s64 new_size = 0;
	int commit_flag;

	jfs_info("jfs_unlink: dip:0x%p name:%s", dip, dentry->d_name.name);

	/*                                  */
	dquot_initialize(dip);
	dquot_initialize(ip);

	if ((rc = get_UCSname(&dname, dentry)))
		goto out;

	IWRITE_LOCK(ip, RDWRLOCK_NORMAL);

	tid = txBegin(dip->i_sb, 0);

	mutex_lock_nested(&JFS_IP(dip)->commit_mutex, COMMIT_MUTEX_PARENT);
	mutex_lock_nested(&JFS_IP(ip)->commit_mutex, COMMIT_MUTEX_CHILD);

	iplist[0] = dip;
	iplist[1] = ip;

	/*
                                                         
  */
	ino = ip->i_ino;
	if ((rc = dtDelete(tid, dip, &dname, &ino, JFS_REMOVE))) {
		jfs_err("jfs_unlink: dtDelete returned %d", rc);
		if (rc == -EIO)
			txAbort(tid, 1);	/*                */
		txEnd(tid);
		mutex_unlock(&JFS_IP(ip)->commit_mutex);
		mutex_unlock(&JFS_IP(dip)->commit_mutex);
		IWRITE_UNLOCK(ip);
		goto out1;
	}

	ASSERT(ip->i_nlink);

	ip->i_ctime = dip->i_ctime = dip->i_mtime = CURRENT_TIME;
	mark_inode_dirty(dip);

	/*                       */
	inode_dec_link_count(ip);

	/*
                                 
  */
	if (ip->i_nlink == 0) {
		assert(!test_cflag(COMMIT_Nolink, ip));
		/*                      */
		if ((new_size = commitZeroLink(tid, ip)) < 0) {
			txAbort(tid, 1);	/*                */
			txEnd(tid);
			mutex_unlock(&JFS_IP(ip)->commit_mutex);
			mutex_unlock(&JFS_IP(dip)->commit_mutex);
			IWRITE_UNLOCK(ip);
			rc = new_size;
			goto out1;
		}
		tblk = tid_to_tblock(tid);
		tblk->xflag |= COMMIT_DELETE;
		tblk->u.ip = ip;
	}

	/*
                                        
                                                                
                
  */
	if (new_size)
		commit_flag = COMMIT_SYNC;
	else
		commit_flag = 0;

	/*
                                                               
                        
  */
	rc = txCommit(tid, 2, &iplist[0], commit_flag);

	txEnd(tid);

	mutex_unlock(&JFS_IP(ip)->commit_mutex);
	mutex_unlock(&JFS_IP(dip)->commit_mutex);

	while (new_size && (rc == 0)) {
		tid = txBegin(dip->i_sb, 0);
		mutex_lock(&JFS_IP(ip)->commit_mutex);
		new_size = xtTruncate_pmap(tid, ip, new_size);
		if (new_size < 0) {
			txAbort(tid, 1);	/*                */
			rc = new_size;
		} else
			rc = txCommit(tid, 2, &iplist[0], COMMIT_SYNC);
		txEnd(tid);
		mutex_unlock(&JFS_IP(ip)->commit_mutex);
	}

	if (ip->i_nlink == 0)
		set_cflag(COMMIT_Nolink, ip);

	IWRITE_UNLOCK(ip);

	/*
                                                               
                                   
  */
	if (test_cflag(COMMIT_Stale, dip)) {
		if (dip->i_size > 1)
			jfs_truncate_nolock(dip, 0);

		clear_cflag(COMMIT_Stale, dip);
	}

      out1:
	free_UCSname(&dname);
      out:
	jfs_info("jfs_unlink: rc:%d", rc);
	return rc;
}

/*
                         
  
                                                       
                                                  
                                                
                 
  
                                                         
                                                          
                                                          
                                                        
                   
                                                         
                                         
                                                        
                       
                             
  
                                                     
                                            
  
                                  
 */
static s64 commitZeroLink(tid_t tid, struct inode *ip)
{
	int filetype;
	struct tblock *tblk;

	jfs_info("commitZeroLink: tid = %d, ip = 0x%p", tid, ip);

	filetype = ip->i_mode & S_IFMT;
	switch (filetype) {
	case S_IFREG:
		break;
	case S_IFLNK:
		/*                    */
		if (ip->i_size < IDATASIZE) {
			ip->i_size = 0;
			return 0;
		}
		break;
	default:
		assert(filetype != S_IFDIR);
		return 0;
	}

	set_cflag(COMMIT_Freewmap, ip);

	/*                                           */
	tblk = tid_to_tblock(tid);
	tblk->xflag |= COMMIT_PMAP;

	/*
           
  */
	if (JFS_IP(ip)->ea.flag & DXD_EXTENT)
		/*                                                  */
		txEA(tid, ip, &JFS_IP(ip)->ea, NULL);

	/*
            
  */
	if (JFS_IP(ip)->acl.flag & DXD_EXTENT)
		/*                                                  */
		txEA(tid, ip, &JFS_IP(ip)->acl, NULL);

	/*
                                              
                                                     
                                                         
                                                                  
  */
	if (ip->i_size)
		return xtTruncate_pmap(tid, ip, 0);

	return 0;
}


/*
                             
  
                                                   
                                                       
                                                        
                                         
  
                                            
 */
void jfs_free_zero_link(struct inode *ip)
{
	int type;

	jfs_info("jfs_free_zero_link: ip = 0x%p", ip);

	/*                                                 
               
  */
	type = ip->i_mode & S_IFMT;

	switch (type) {
	case S_IFREG:
		break;
	case S_IFLNK:
		/*                                         */
		if (ip->i_size < IDATASIZE)
			return;
		break;
	default:
		return;
	}

	/*
           
  */
	if (JFS_IP(ip)->ea.flag & DXD_EXTENT) {
		s64 xaddr = addressDXD(&JFS_IP(ip)->ea);
		int xlen = lengthDXD(&JFS_IP(ip)->ea);
		struct maplock maplock;	/*                         */
		struct pxd_lock *pxdlock;	/*                         */

		/*                          */
		invalidate_dxd_metapages(ip, JFS_IP(ip)->ea);

		/*                                       */
		maplock.index = 1;
		pxdlock = (struct pxd_lock *) & maplock;
		pxdlock->flag = mlckFREEPXD;
		PXDaddress(&pxdlock->pxd, xaddr);
		PXDlength(&pxdlock->pxd, xlen);
		txFreeMap(ip, pxdlock, NULL, COMMIT_WMAP);
	}

	/*
            
  */
	if (JFS_IP(ip)->acl.flag & DXD_EXTENT) {
		s64 xaddr = addressDXD(&JFS_IP(ip)->acl);
		int xlen = lengthDXD(&JFS_IP(ip)->acl);
		struct maplock maplock;	/*                         */
		struct pxd_lock *pxdlock;	/*                         */

		invalidate_dxd_metapages(ip, JFS_IP(ip)->acl);

		/*                                        */
		maplock.index = 1;
		pxdlock = (struct pxd_lock *) & maplock;
		pxdlock->flag = mlckFREEPXD;
		PXDaddress(&pxdlock->pxd, xaddr);
		PXDlength(&pxdlock->pxd, xlen);
		txFreeMap(ip, pxdlock, NULL, COMMIT_WMAP);
	}

	/*
                                              
                                         
                                                  
  */
	if (ip->i_size)
		xtTruncate(0, ip, 0, COMMIT_WMAP);
}

/*
                                     
  
                                                       
                                 
  
                                
                                      
                                            
                    
  
                                  
  
        
                                                                  
                                    
                                                                 
                                                                       
                                        
  
                                                   
                                                                      
                                                                       
 */
static int jfs_link(struct dentry *old_dentry,
	     struct inode *dir, struct dentry *dentry)
{
	int rc;
	tid_t tid;
	struct inode *ip = old_dentry->d_inode;
	ino_t ino;
	struct component_name dname;
	struct btstack btstack;
	struct inode *iplist[2];

	jfs_info("jfs_link: %s %s", old_dentry->d_name.name,
		 dentry->d_name.name);

	dquot_initialize(dir);

	tid = txBegin(ip->i_sb, 0);

	mutex_lock_nested(&JFS_IP(dir)->commit_mutex, COMMIT_MUTEX_PARENT);
	mutex_lock_nested(&JFS_IP(ip)->commit_mutex, COMMIT_MUTEX_CHILD);

	/*
                                             
  */
	if ((rc = get_UCSname(&dname, dentry)))
		goto out;

	if ((rc = dtSearch(dir, &dname, &ino, &btstack, JFS_CREATE)))
		goto free_dname;

	/*
                                                 
  */
	ino = ip->i_ino;
	if ((rc = dtInsert(tid, dir, &dname, &ino, &btstack)))
		goto free_dname;

	/*                     */
	inc_nlink(ip);		/*              */
	ip->i_ctime = CURRENT_TIME;
	dir->i_ctime = dir->i_mtime = CURRENT_TIME;
	mark_inode_dirty(dir);
	ihold(ip);

	iplist[0] = ip;
	iplist[1] = dir;
	rc = txCommit(tid, 2, &iplist[0], 0);

	if (rc) {
		drop_nlink(ip); /*                    */
		iput(ip);
	} else
		d_instantiate(dentry, ip);

      free_dname:
	free_UCSname(&dname);

      out:
	txEnd(tid);

	mutex_unlock(&JFS_IP(ip)->commit_mutex);
	mutex_unlock(&JFS_IP(dir)->commit_mutex);

	jfs_info("jfs_link: rc:%d", rc);
	return rc;
}

/*
                                       
  
                                                                
                       
  
                                          
                                    
                                               
                                          
  
                                  
  
        
                                                                
                                                                
*/

static int jfs_symlink(struct inode *dip, struct dentry *dentry,
		const char *name)
{
	int rc;
	tid_t tid;
	ino_t ino = 0;
	struct component_name dname;
	int ssize;		/*                      */
	struct btstack btstack;
	struct inode *ip = dentry->d_inode;
	unchar *i_fastsymlink;
	s64 xlen = 0;
	int bmask = 0, xsize;
	s64 xaddr;
	struct metapage *mp;
	struct super_block *sb;
	struct tblock *tblk;

	struct inode *iplist[2];

	jfs_info("jfs_symlink: dip:0x%p name:%s", dip, name);

	dquot_initialize(dip);

	ssize = strlen(name) + 1;

	/*
                                               
                                                     
  */

	if ((rc = get_UCSname(&dname, dentry)))
		goto out1;

	/*
                                                       
                                        
  */
	ip = ialloc(dip, S_IFLNK | 0777);
	if (IS_ERR(ip)) {
		rc = PTR_ERR(ip);
		goto out2;
	}

	tid = txBegin(dip->i_sb, 0);

	mutex_lock_nested(&JFS_IP(dip)->commit_mutex, COMMIT_MUTEX_PARENT);
	mutex_lock_nested(&JFS_IP(ip)->commit_mutex, COMMIT_MUTEX_CHILD);

	rc = jfs_init_security(tid, ip, dip, &dentry->d_name);
	if (rc)
		goto out3;

	tblk = tid_to_tblock(tid);
	tblk->xflag |= COMMIT_CREATE;
	tblk->ino = ip->i_ino;
	tblk->u.ixpxd = JFS_IP(ip)->ixpxd;

	/*                              
                                        
                                              
  */
	ip->i_mode |= 0777;

	/*
                                        
  */
	xtInitRoot(tid, ip);

	/*
                                                                       
  */

	if (ssize <= IDATASIZE) {
		ip->i_op = &jfs_fast_symlink_inode_operations;

		i_fastsymlink = JFS_IP(ip)->i_inline;
		memcpy(i_fastsymlink, name, ssize);
		ip->i_size = ssize - 1;

		/*
                                                          
                                     
   */
		if (ssize > sizeof (JFS_IP(ip)->i_inline))
			JFS_IP(ip)->mode2 &= ~INLINEEA;

		jfs_info("jfs_symlink: fast symlink added  ssize:%d name:%s ",
			 ssize, name);
	}
	/*
                                             
  */
	else {
		jfs_info("jfs_symlink: allocate extent ip:0x%p", ip);

		ip->i_op = &jfs_symlink_inode_operations;
		ip->i_mapping->a_ops = &jfs_aops;

		/*
                                                   
                                                      
                                                          
   */
		sb = ip->i_sb;
		bmask = JFS_SBI(sb)->bsize - 1;
		xsize = (ssize + bmask) & ~bmask;
		xaddr = 0;
		xlen = xsize >> JFS_SBI(sb)->l2bsize;
		if ((rc = xtInsert(tid, ip, 0, 0, xlen, &xaddr, 0))) {
			txAbort(tid, 0);
			goto out3;
		}
		ip->i_size = ssize - 1;
		while (ssize) {
			/*                                            */
			int copy_size = min(ssize, PSIZE);

			mp = get_metapage(ip, xaddr, PSIZE, 1);

			if (mp == NULL) {
				xtTruncate(tid, ip, 0, COMMIT_PWMAP);
				rc = -EIO;
				txAbort(tid, 0);
				goto out3;
			}
			memcpy(mp->data, name, copy_size);
			flush_metapage(mp);
			ssize -= copy_size;
			name += copy_size;
			xaddr += JFS_SBI(sb)->nbperpage;
		}
	}

	/*
                                                      
  */
	rc = dtSearch(dip, &dname, &ino, &btstack, JFS_CREATE);
	if (rc == 0) {
		ino = ip->i_ino;
		rc = dtInsert(tid, dip, &dname, &ino, &btstack);
	}
	if (rc) {
		if (xlen)
			xtTruncate(tid, ip, 0, COMMIT_PWMAP);
		txAbort(tid, 0);
		/*                   */
		goto out3;
	}

	mark_inode_dirty(ip);

	dip->i_ctime = dip->i_mtime = CURRENT_TIME;
	mark_inode_dirty(dip);
	/*
                                                     
  */

	iplist[0] = dip;
	iplist[1] = ip;
	rc = txCommit(tid, 2, &iplist[0], 0);

      out3:
	txEnd(tid);
	mutex_unlock(&JFS_IP(ip)->commit_mutex);
	mutex_unlock(&JFS_IP(dip)->commit_mutex);
	if (rc) {
		free_ea_wmap(ip);
		clear_nlink(ip);
		unlock_new_inode(ip);
		iput(ip);
	} else {
		unlock_new_inode(ip);
		d_instantiate(dentry, ip);
	}

      out2:
	free_UCSname(&dname);

      out1:
	jfs_info("jfs_symlink: rc:%d", rc);
	return rc;
}


/*
                   
  
                                       
 */
static int jfs_rename(struct inode *old_dir, struct dentry *old_dentry,
	       struct inode *new_dir, struct dentry *new_dentry)
{
	struct btstack btstack;
	ino_t ino;
	struct component_name new_dname;
	struct inode *new_ip;
	struct component_name old_dname;
	struct inode *old_ip;
	int rc;
	tid_t tid;
	struct tlock *tlck;
	struct dt_lock *dtlck;
	struct lv *lv;
	int ipcount;
	struct inode *iplist[4];
	struct tblock *tblk;
	s64 new_size = 0;
	int commit_flag;


	jfs_info("jfs_rename: %s %s", old_dentry->d_name.name,
		 new_dentry->d_name.name);

	dquot_initialize(old_dir);
	dquot_initialize(new_dir);

	old_ip = old_dentry->d_inode;
	new_ip = new_dentry->d_inode;

	if ((rc = get_UCSname(&old_dname, old_dentry)))
		goto out1;

	if ((rc = get_UCSname(&new_dname, new_dentry)))
		goto out2;

	/*
                                                        
  */
	rc = dtSearch(old_dir, &old_dname, &ino, &btstack, JFS_LOOKUP);
	if (rc || (ino != old_ip->i_ino)) {
		rc = -ENOENT;
		goto out3;
	}

	/*
                                                               
  */
	rc = dtSearch(new_dir, &new_dname, &ino, &btstack, JFS_LOOKUP);
	if (!rc) {
		if ((!new_ip) || (ino != new_ip->i_ino)) {
			rc = -ESTALE;
			goto out3;
		}
	} else if (rc != -ENOENT)
		goto out3;
	else if (new_ip) {
		/*                                       */
		rc = -ESTALE;
		goto out3;
	}

	if (S_ISDIR(old_ip->i_mode)) {
		if (new_ip) {
			if (!dtEmpty(new_ip)) {
				rc = -ENOTEMPTY;
				goto out3;
			}
		}
	} else if (new_ip) {
		IWRITE_LOCK(new_ip, RDWRLOCK_NORMAL);
		/*                                  */
		dquot_initialize(new_ip);
	}

	/*
                             
  */
	tid = txBegin(new_dir->i_sb, 0);

	/*
                                                      
                                                                     
                                                                   
                                                                        
  */
	mutex_lock_nested(&JFS_IP(new_dir)->commit_mutex, COMMIT_MUTEX_PARENT);
	mutex_lock_nested(&JFS_IP(old_ip)->commit_mutex, COMMIT_MUTEX_CHILD);
	if (old_dir != new_dir)
		mutex_lock_nested(&JFS_IP(old_dir)->commit_mutex,
				  COMMIT_MUTEX_SECOND_PARENT);

	if (new_ip) {
		mutex_lock_nested(&JFS_IP(new_ip)->commit_mutex,
				  COMMIT_MUTEX_VICTIM);
		/*
                                                        
   */
		ino = new_ip->i_ino;
		rc = dtModify(tid, new_dir, &new_dname, &ino,
			      old_ip->i_ino, JFS_RENAME);
		if (rc)
			goto out4;
		drop_nlink(new_ip);
		if (S_ISDIR(new_ip->i_mode)) {
			drop_nlink(new_ip);
			if (new_ip->i_nlink) {
				mutex_unlock(&JFS_IP(new_ip)->commit_mutex);
				if (old_dir != new_dir)
					mutex_unlock(&JFS_IP(old_dir)->commit_mutex);
				mutex_unlock(&JFS_IP(old_ip)->commit_mutex);
				mutex_unlock(&JFS_IP(new_dir)->commit_mutex);
				if (!S_ISDIR(old_ip->i_mode) && new_ip)
					IWRITE_UNLOCK(new_ip);
				jfs_error(new_ip->i_sb,
					  "jfs_rename: new_ip->i_nlink != 0");
				return -EIO;
			}
			tblk = tid_to_tblock(tid);
			tblk->xflag |= COMMIT_DELETE;
			tblk->u.ip = new_ip;
		} else if (new_ip->i_nlink == 0) {
			assert(!test_cflag(COMMIT_Nolink, new_ip));
			/*                      */
			if ((new_size = commitZeroLink(tid, new_ip)) < 0) {
				txAbort(tid, 1);	/*                */
				rc = new_size;
				goto out4;
			}
			tblk = tid_to_tblock(tid);
			tblk->xflag |= COMMIT_DELETE;
			tblk->u.ip = new_ip;
		} else {
			new_ip->i_ctime = CURRENT_TIME;
			mark_inode_dirty(new_ip);
		}
	} else {
		/*
                            
   */
		rc = dtSearch(new_dir, &new_dname, &ino, &btstack,
			      JFS_CREATE);
		if (rc) {
			jfs_err("jfs_rename didn't expect dtSearch to fail "
				"w/rc = %d", rc);
			goto out4;
		}

		ino = old_ip->i_ino;
		rc = dtInsert(tid, new_dir, &new_dname, &ino, &btstack);
		if (rc) {
			if (rc == -EIO)
				jfs_err("jfs_rename: dtInsert returned -EIO");
			goto out4;
		}
		if (S_ISDIR(old_ip->i_mode))
			inc_nlink(new_dir);
	}
	/*
                              
  */

	ino = old_ip->i_ino;
	rc = dtDelete(tid, old_dir, &old_dname, &ino, JFS_REMOVE);
	if (rc) {
		jfs_err("jfs_rename did not expect dtDelete to return rc = %d",
			rc);
		txAbort(tid, 1);	/*                        */
		goto out4;
	}
	if (S_ISDIR(old_ip->i_mode)) {
		drop_nlink(old_dir);
		if (old_dir != new_dir) {
			/*
                                                       
    */

			JFS_IP(old_ip)->i_dtroot.header.idotdot =
				cpu_to_le32(new_dir->i_ino);

			/*                          */
			tlck = txLock(tid, old_ip,
				    (struct metapage *) &JFS_IP(old_ip)->bxflag,
				      tlckDTREE | tlckBTROOT | tlckRELINK);
			dtlck = (struct dt_lock *) & tlck->lock;
			ASSERT(dtlck->index == 0);
			lv = & dtlck->lv[0];
			lv->offset = 0;
			lv->length = 1;
			dtlck->index++;
		}
	}

	/*
                                                     
  */
	old_ip->i_ctime = CURRENT_TIME;
	mark_inode_dirty(old_ip);

	new_dir->i_ctime = new_dir->i_mtime = current_fs_time(new_dir->i_sb);
	mark_inode_dirty(new_dir);

	/*                                                   */
	ipcount = 0;
	iplist[ipcount++] = old_ip;
	if (new_ip)
		iplist[ipcount++] = new_ip;
	iplist[ipcount++] = old_dir;

	if (old_dir != new_dir) {
		iplist[ipcount++] = new_dir;
		old_dir->i_ctime = old_dir->i_mtime = CURRENT_TIME;
		mark_inode_dirty(old_dir);
	}

	/*
                                        
                                                                
                
  */
	if (new_size)
		commit_flag = COMMIT_SYNC;
	else
		commit_flag = 0;

	rc = txCommit(tid, ipcount, iplist, commit_flag);

      out4:
	txEnd(tid);
	if (new_ip)
		mutex_unlock(&JFS_IP(new_ip)->commit_mutex);
	if (old_dir != new_dir)
		mutex_unlock(&JFS_IP(old_dir)->commit_mutex);
	mutex_unlock(&JFS_IP(old_ip)->commit_mutex);
	mutex_unlock(&JFS_IP(new_dir)->commit_mutex);

	while (new_size && (rc == 0)) {
		tid = txBegin(new_ip->i_sb, 0);
		mutex_lock(&JFS_IP(new_ip)->commit_mutex);
		new_size = xtTruncate_pmap(tid, new_ip, new_size);
		if (new_size < 0) {
			txAbort(tid, 1);
			rc = new_size;
		} else
			rc = txCommit(tid, 1, &new_ip, COMMIT_SYNC);
		txEnd(tid);
		mutex_unlock(&JFS_IP(new_ip)->commit_mutex);
	}
	if (new_ip && (new_ip->i_nlink == 0))
		set_cflag(COMMIT_Nolink, new_ip);
      out3:
	free_UCSname(&new_dname);
      out2:
	free_UCSname(&old_dname);
      out1:
	if (new_ip && !S_ISDIR(new_ip->i_mode))
		IWRITE_UNLOCK(new_ip);
	/*
                                                               
                                   
  */
	if (test_cflag(COMMIT_Stale, old_dir)) {
		if (old_dir->i_size > 1)
			jfs_truncate_nolock(old_dir, 0);

		clear_cflag(COMMIT_Stale, old_dir);
	}

	jfs_info("jfs_rename: returning %d", rc);
	return rc;
}


/*
                  
  
                                           
 */
static int jfs_mknod(struct inode *dir, struct dentry *dentry,
		umode_t mode, dev_t rdev)
{
	struct jfs_inode_info *jfs_ip;
	struct btstack btstack;
	struct component_name dname;
	ino_t ino;
	struct inode *ip;
	struct inode *iplist[2];
	int rc;
	tid_t tid;
	struct tblock *tblk;

	if (!new_valid_dev(rdev))
		return -EINVAL;

	jfs_info("jfs_mknod: %s", dentry->d_name.name);

	dquot_initialize(dir);

	if ((rc = get_UCSname(&dname, dentry)))
		goto out;

	ip = ialloc(dir, mode);
	if (IS_ERR(ip)) {
		rc = PTR_ERR(ip);
		goto out1;
	}
	jfs_ip = JFS_IP(ip);

	tid = txBegin(dir->i_sb, 0);

	mutex_lock_nested(&JFS_IP(dir)->commit_mutex, COMMIT_MUTEX_PARENT);
	mutex_lock_nested(&JFS_IP(ip)->commit_mutex, COMMIT_MUTEX_CHILD);

	rc = jfs_init_acl(tid, ip, dir);
	if (rc)
		goto out3;

	rc = jfs_init_security(tid, ip, dir, &dentry->d_name);
	if (rc) {
		txAbort(tid, 0);
		goto out3;
	}

	if ((rc = dtSearch(dir, &dname, &ino, &btstack, JFS_CREATE))) {
		txAbort(tid, 0);
		goto out3;
	}

	tblk = tid_to_tblock(tid);
	tblk->xflag |= COMMIT_CREATE;
	tblk->ino = ip->i_ino;
	tblk->u.ixpxd = JFS_IP(ip)->ixpxd;

	ino = ip->i_ino;
	if ((rc = dtInsert(tid, dir, &dname, &ino, &btstack))) {
		txAbort(tid, 0);
		goto out3;
	}

	ip->i_op = &jfs_file_inode_operations;
	jfs_ip->dev = new_encode_dev(rdev);
	init_special_inode(ip, ip->i_mode, rdev);

	mark_inode_dirty(ip);

	dir->i_ctime = dir->i_mtime = CURRENT_TIME;

	mark_inode_dirty(dir);

	iplist[0] = dir;
	iplist[1] = ip;
	rc = txCommit(tid, 2, iplist, 0);

      out3:
	txEnd(tid);
	mutex_unlock(&JFS_IP(ip)->commit_mutex);
	mutex_unlock(&JFS_IP(dir)->commit_mutex);
	if (rc) {
		free_ea_wmap(ip);
		clear_nlink(ip);
		unlock_new_inode(ip);
		iput(ip);
	} else {
		unlock_new_inode(ip);
		d_instantiate(dentry, ip);
	}

      out1:
	free_UCSname(&dname);

      out:
	jfs_info("jfs_mknod: returning %d", rc);
	return rc;
}

static struct dentry *jfs_lookup(struct inode *dip, struct dentry *dentry, unsigned int flags)
{
	struct btstack btstack;
	ino_t inum;
	struct inode *ip;
	struct component_name key;
	int rc;

	jfs_info("jfs_lookup: name = %s", dentry->d_name.name);

	if ((rc = get_UCSname(&key, dentry)))
		return ERR_PTR(rc);
	rc = dtSearch(dip, &key, &inum, &btstack, JFS_LOOKUP);
	free_UCSname(&key);
	if (rc == -ENOENT) {
		ip = NULL;
	} else if (rc) {
		jfs_err("jfs_lookup: dtSearch returned %d", rc);
		ip = ERR_PTR(rc);
	} else {
		ip = jfs_iget(dip->i_sb, inum);
		if (IS_ERR(ip))
			jfs_err("jfs_lookup: iget failed on inum %d", (uint)inum);
	}

	return d_splice_alias(ip, dentry);
}

static struct inode *jfs_nfs_get_inode(struct super_block *sb,
		u64 ino, u32 generation)
{
	struct inode *inode;

	if (ino == 0)
		return ERR_PTR(-ESTALE);
	inode = jfs_iget(sb, ino);
	if (IS_ERR(inode))
		return ERR_CAST(inode);

	if (generation && inode->i_generation != generation) {
		iput(inode);
		return ERR_PTR(-ESTALE);
	}

	return inode;
}

struct dentry *jfs_fh_to_dentry(struct super_block *sb, struct fid *fid,
		int fh_len, int fh_type)
{
	return generic_fh_to_dentry(sb, fid, fh_len, fh_type,
				    jfs_nfs_get_inode);
}

struct dentry *jfs_fh_to_parent(struct super_block *sb, struct fid *fid,
		int fh_len, int fh_type)
{
	return generic_fh_to_parent(sb, fid, fh_len, fh_type,
				    jfs_nfs_get_inode);
}

struct dentry *jfs_get_parent(struct dentry *dentry)
{
	unsigned long parent_ino;

	parent_ino =
		le32_to_cpu(JFS_IP(dentry->d_inode)->i_dtroot.header.idotdot);

	return d_obtain_alias(jfs_iget(dentry->d_inode->i_sb, parent_ino));
}

const struct inode_operations jfs_dir_inode_operations = {
	.create		= jfs_create,
	.lookup		= jfs_lookup,
	.link		= jfs_link,
	.unlink		= jfs_unlink,
	.symlink	= jfs_symlink,
	.mkdir		= jfs_mkdir,
	.rmdir		= jfs_rmdir,
	.mknod		= jfs_mknod,
	.rename		= jfs_rename,
	.setxattr	= jfs_setxattr,
	.getxattr	= jfs_getxattr,
	.listxattr	= jfs_listxattr,
	.removexattr	= jfs_removexattr,
	.setattr	= jfs_setattr,
#ifdef CONFIG_JFS_POSIX_ACL
	.get_acl	= jfs_get_acl,
#endif
};

const struct file_operations jfs_dir_operations = {
	.read		= generic_read_dir,
	.readdir	= jfs_readdir,
	.fsync		= jfs_fsync,
	.unlocked_ioctl = jfs_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl	= jfs_compat_ioctl,
#endif
	.llseek		= generic_file_llseek,
};

static int jfs_ci_hash(const struct dentry *dir, const struct inode *inode,
		struct qstr *this)
{
	unsigned long hash;
	int i;

	hash = init_name_hash();
	for (i=0; i < this->len; i++)
		hash = partial_name_hash(tolower(this->name[i]), hash);
	this->hash = end_name_hash(hash);

	return 0;
}

static int jfs_ci_compare(const struct dentry *parent,
		const struct inode *pinode,
		const struct dentry *dentry, const struct inode *inode,
		unsigned int len, const char *str, const struct qstr *name)
{
	int i, result = 1;

	if (len != name->len)
		goto out;
	for (i=0; i < len; i++) {
		if (tolower(str[i]) != tolower(name->name[i]))
			goto out;
	}
	result = 0;
out:
	return result;
}

static int jfs_ci_revalidate(struct dentry *dentry, unsigned int flags)
{
	/*
                                              
   
                                                                   
                                                                  
   
                                                                  
                                                             
                                           
  */
	if (dentry->d_inode)
		return 1;

	/*
                                                             
                                                                
  */
	if (!flags)
		return 0;

	/*
                                                              
                                                             
                 
  */
	if (flags & (LOOKUP_CREATE | LOOKUP_RENAME_TARGET))
		return 0;
	return 1;
}

const struct dentry_operations jfs_ci_dentry_operations =
{
	.d_hash = jfs_ci_hash,
	.d_compare = jfs_ci_compare,
	.d_revalidate = jfs_ci_revalidate,
};