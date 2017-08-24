/*
 *   fs/cifs/dir.c
 *
 *   vfs operations that deal with dentries
 *
 *   Copyright (C) International Business Machines  Corp., 2002,2009
 *   Author(s): Steve French (sfrench@us.ibm.com)
 *
 *   This library is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published
 *   by the Free Software Foundation; either version 2.1 of the License, or
 *   (at your option) any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 *   the GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with this library; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */
#include <linux/fs.h>
#include <linux/stat.h>
#include <linux/slab.h>
#include <linux/namei.h>
#include <linux/mount.h>
#include <linux/file.h>
#include "cifsfs.h"
#include "cifspdu.h"
#include "cifsglob.h"
#include "cifsproto.h"
#include "cifs_debug.h"
#include "cifs_fs_sb.h"

static void
renew_parental_timestamps(struct dentry *direntry)
{
	/*                                                                 
                     */
	do {
		direntry->d_time = jiffies;
		direntry = direntry->d_parent;
	} while (!IS_ROOT(direntry));
}

char *
cifs_build_path_to_root(struct smb_vol *vol, struct cifs_sb_info *cifs_sb,
			struct cifs_tcon *tcon)
{
	int pplen = vol->prepath ? strlen(vol->prepath) + 1 : 0;
	int dfsplen;
	char *full_path = NULL;

	/*                                                               */
	if (pplen == 0) {
		full_path = kzalloc(1, GFP_KERNEL);
		return full_path;
	}

	if (tcon->Flags & SMB_SHARE_IS_IN_DFS)
		dfsplen = strnlen(tcon->treeName, MAX_TREE_SIZE + 1);
	else
		dfsplen = 0;

	full_path = kmalloc(dfsplen + pplen + 1, GFP_KERNEL);
	if (full_path == NULL)
		return full_path;

	if (dfsplen)
		strncpy(full_path, tcon->treeName, dfsplen);
	full_path[dfsplen] = CIFS_DIR_SEP(cifs_sb);
	strncpy(full_path + dfsplen + 1, vol->prepath, pplen);
	convert_delimiter(full_path, CIFS_DIR_SEP(cifs_sb));
	full_path[dfsplen + pplen] = 0; /*                   */
	return full_path;
}

/*                                      */
char *
build_path_from_dentry(struct dentry *direntry)
{
	struct dentry *temp;
	int namelen;
	int dfsplen;
	char *full_path;
	char dirsep;
	struct cifs_sb_info *cifs_sb = CIFS_SB(direntry->d_sb);
	struct cifs_tcon *tcon = cifs_sb_master_tcon(cifs_sb);
	unsigned seq;

	dirsep = CIFS_DIR_SEP(cifs_sb);
	if (tcon->Flags & SMB_SHARE_IS_IN_DFS)
		dfsplen = strnlen(tcon->treeName, MAX_TREE_SIZE + 1);
	else
		dfsplen = 0;
cifs_bp_rename_retry:
	namelen = dfsplen;
	seq = read_seqbegin(&rename_lock);
	rcu_read_lock();
	for (temp = direntry; !IS_ROOT(temp);) {
		namelen += (1 + temp->d_name.len);
		temp = temp->d_parent;
		if (temp == NULL) {
			cifs_dbg(VFS, "corrupt dentry\n");
			rcu_read_unlock();
			return NULL;
		}
	}
	rcu_read_unlock();

	full_path = kmalloc(namelen+1, GFP_KERNEL);
	if (full_path == NULL)
		return full_path;
	full_path[namelen] = 0;	/*               */
	rcu_read_lock();
	for (temp = direntry; !IS_ROOT(temp);) {
		spin_lock(&temp->d_lock);
		namelen -= 1 + temp->d_name.len;
		if (namelen < 0) {
			spin_unlock(&temp->d_lock);
			break;
		} else {
			full_path[namelen] = dirsep;
			strncpy(full_path + namelen + 1, temp->d_name.name,
				temp->d_name.len);
			cifs_dbg(FYI, "name: %s\n", full_path + namelen);
		}
		spin_unlock(&temp->d_lock);
		temp = temp->d_parent;
		if (temp == NULL) {
			cifs_dbg(VFS, "corrupt dentry\n");
			rcu_read_unlock();
			kfree(full_path);
			return NULL;
		}
	}
	rcu_read_unlock();
	if (namelen != dfsplen || read_seqretry(&rename_lock, seq)) {
		cifs_dbg(FYI, "did not end path lookup where expected. namelen=%ddfsplen=%d\n",
			 namelen, dfsplen);
		/*                                                         
                                                                 
                                                             */
		kfree(full_path);
		goto cifs_bp_rename_retry;
	}
	/*                                                   
                                                      
                                                                  
                                                                 
                                                                      */
	/*                                                           */

	if (dfsplen) {
		strncpy(full_path, tcon->treeName, dfsplen);
		if (cifs_sb->mnt_cifs_flags & CIFS_MOUNT_POSIX_PATHS) {
			int i;
			for (i = 0; i < dfsplen; i++) {
				if (full_path[i] == '\\')
					full_path[i] = '/';
			}
		}
	}
	return full_path;
}

/*
                                                           
                                                           
 */
static int
check_name(struct dentry *direntry)
{
	struct cifs_sb_info *cifs_sb = CIFS_SB(direntry->d_sb);
	int i;

	if (!(cifs_sb->mnt_cifs_flags & CIFS_MOUNT_POSIX_PATHS)) {
		for (i = 0; i < direntry->d_name.len; i++) {
			if (direntry->d_name.name[i] == '\\') {
				cifs_dbg(FYI, "Invalid file name\n");
				return -EINVAL;
			}
		}
	}
	return 0;
}


/*                                                                         */

static int
cifs_do_create(struct inode *inode, struct dentry *direntry, unsigned int xid,
	       struct tcon_link *tlink, unsigned oflags, umode_t mode,
	       __u32 *oplock, struct cifs_fid *fid, int *created)
{
	int rc = -ENOENT;
	int create_options = CREATE_NOT_DIR;
	int desired_access;
	struct cifs_sb_info *cifs_sb = CIFS_SB(inode->i_sb);
	struct cifs_tcon *tcon = tlink_tcon(tlink);
	char *full_path = NULL;
	FILE_ALL_INFO *buf = NULL;
	struct inode *newinode = NULL;
	int disposition;
	struct TCP_Server_Info *server = tcon->ses->server;

	*oplock = 0;
	if (tcon->ses->server->oplocks)
		*oplock = REQ_OPLOCK;

	full_path = build_path_from_dentry(direntry);
	if (full_path == NULL) {
		rc = -ENOMEM;
		goto out;
	}

	if (tcon->unix_ext && cap_unix(tcon->ses) && !tcon->broken_posix_open &&
	    (CIFS_UNIX_POSIX_PATH_OPS_CAP &
			le64_to_cpu(tcon->fsUnixInfo.Capability))) {
		rc = cifs_posix_open(full_path, &newinode, inode->i_sb, mode,
				     oflags, oplock, &fid->netfid, xid);
		switch (rc) {
		case 0:
			if (newinode == NULL) {
				/*                  */
				goto cifs_create_get_file_info;
			}

			if (!S_ISREG(newinode->i_mode)) {
				/*
                                                  
                                                 
                                                  
                                                  
              
     */
				CIFSSMBClose(xid, tcon, fid->netfid);
				goto cifs_create_get_file_info;
			}
			/*                           */
			goto cifs_create_set_dentry;

		case -ENOENT:
			goto cifs_create_get_file_info;

		case -EIO:
		case -EINVAL:
			/*
                                                           
                                                          
                  
     
                                                          
                                              
    */
			tcon->broken_posix_open = true;
			break;

		case -EREMOTE:
		case -EOPNOTSUPP:
			/*
                                                          
                                                        
                                         
    */
			break;

		default:
			goto out;
		}
		/*
                                                              
                                                              
                                                                  
                                        
   */
	}

	desired_access = 0;
	if (OPEN_FMODE(oflags) & FMODE_READ)
		desired_access |= GENERIC_READ; /*                     */
	if (OPEN_FMODE(oflags) & FMODE_WRITE)
		desired_access |= GENERIC_WRITE;

	disposition = FILE_OVERWRITE_IF;
	if ((oflags & (O_CREAT | O_EXCL)) == (O_CREAT | O_EXCL))
		disposition = FILE_CREATE;
	else if ((oflags & (O_CREAT | O_TRUNC)) == (O_CREAT | O_TRUNC))
		disposition = FILE_OVERWRITE_IF;
	else if ((oflags & O_CREAT) == O_CREAT)
		disposition = FILE_OPEN_IF;
	else
		cifs_dbg(FYI, "Create flag not set in create function\n");

	/*
                                                                       
        
  */

	if (!server->ops->open) {
		rc = -ENOSYS;
		goto out;
	}

	buf = kmalloc(sizeof(FILE_ALL_INFO), GFP_KERNEL);
	if (buf == NULL) {
		rc = -ENOMEM;
		goto out;
	}

	/*
                                                             
                                    
  */
	if (!tcon->unix_ext && (mode & S_IWUGO) == 0)
		create_options |= CREATE_OPTION_READONLY;

	if (backup_cred(cifs_sb))
		create_options |= CREATE_OPEN_BACKUP_INTENT;

	rc = server->ops->open(xid, tcon, full_path, disposition,
			       desired_access, create_options, fid, oplock,
			       buf, cifs_sb);
	if (rc) {
		cifs_dbg(FYI, "cifs_create returned 0x%x\n", rc);
		goto out;
	}

	/*
                                                                        
                             
  */
	if ((tcon->unix_ext) && (*oplock & CIFS_CREATE_ACTION)) {
		struct cifs_unix_set_info_args args = {
				.mode	= mode,
				.ctime	= NO_CHANGE_64,
				.atime	= NO_CHANGE_64,
				.mtime	= NO_CHANGE_64,
				.device	= 0,
		};

		*created |= FILE_CREATED;
		if (cifs_sb->mnt_cifs_flags & CIFS_MOUNT_SET_UID) {
			args.uid = current_fsuid();
			if (inode->i_mode & S_ISGID)
				args.gid = inode->i_gid;
			else
				args.gid = current_fsgid();
		} else {
			args.uid = INVALID_UID; /*           */
			args.gid = INVALID_GID; /*           */
		}
		CIFSSMBUnixSetFileInfo(xid, tcon, &args, fid->netfid,
				       current->tgid);
	} else {
		/*
                                                   
                     
   */
		/*                                                  */

		/*                                                 */
	}

cifs_create_get_file_info:
	/*                                                   */
	if (tcon->unix_ext)
		rc = cifs_get_inode_info_unix(&newinode, full_path, inode->i_sb,
					      xid);
	else {
		rc = cifs_get_inode_info(&newinode, full_path, buf, inode->i_sb,
					 xid, &fid->netfid);
		if (newinode) {
			if (server->ops->set_lease_key)
				server->ops->set_lease_key(newinode, fid);
			if (cifs_sb->mnt_cifs_flags & CIFS_MOUNT_DYNPERM)
				newinode->i_mode = mode;
			if ((*oplock & CIFS_CREATE_ACTION) &&
			    (cifs_sb->mnt_cifs_flags & CIFS_MOUNT_SET_UID)) {
				newinode->i_uid = current_fsuid();
				if (inode->i_mode & S_ISGID)
					newinode->i_gid = inode->i_gid;
				else
					newinode->i_gid = current_fsgid();
			}
		}
	}

cifs_create_set_dentry:
	if (rc != 0) {
		cifs_dbg(FYI, "Create worked, get_inode_info failed rc = %d\n",
			 rc);
		if (server->ops->close)
			server->ops->close(xid, tcon, fid);
		goto out;
	}
	d_drop(direntry);
	d_add(direntry, newinode);

out:
	kfree(buf);
	kfree(full_path);
	return rc;
}

int
cifs_atomic_open(struct inode *inode, struct dentry *direntry,
		 struct file *file, unsigned oflags, umode_t mode,
		 int *opened)
{
	int rc;
	unsigned int xid;
	struct tcon_link *tlink;
	struct cifs_tcon *tcon;
	struct TCP_Server_Info *server;
	struct cifs_fid fid;
	struct cifs_pending_open open;
	__u32 oplock;
	struct cifsFileInfo *file_info;

	/*
                                                                       
                                                                       
                                                                       
                                                                        
                                                                
                                                                      
                                                                    
                                                                         
                                          
  */
	if (!(oflags & O_CREAT)) {
		struct dentry *res;

		/*
                                                                  
                                                                  
   */
		if (!d_unhashed(direntry))
			return -ENOENT;

		res = cifs_lookup(inode, direntry, 0);
		if (IS_ERR(res))
			return PTR_ERR(res);

		return finish_no_open(file, res);
	}

	rc = check_name(direntry);
	if (rc)
		return rc;

	xid = get_xid();

	cifs_dbg(FYI, "parent inode = 0x%p name is: %s and dentry = 0x%p\n",
		 inode, direntry->d_name.name, direntry);

	tlink = cifs_sb_tlink(CIFS_SB(inode->i_sb));
	if (IS_ERR(tlink)) {
		rc = PTR_ERR(tlink);
		goto out_free_xid;
	}

	tcon = tlink_tcon(tlink);
	server = tcon->ses->server;

	if (server->ops->new_lease_key)
		server->ops->new_lease_key(&fid);

	cifs_add_pending_open(&fid, tlink, &open);

	rc = cifs_do_create(inode, direntry, xid, tlink, oflags, mode,
			    &oplock, &fid, opened);

	if (rc) {
		cifs_del_pending_open(&open);
		goto out;
	}

	rc = finish_open(file, direntry, generic_file_open, opened);
	if (rc) {
		if (server->ops->close)
			server->ops->close(xid, tcon, &fid);
		cifs_del_pending_open(&open);
		goto out;
	}

	file_info = cifs_new_fileinfo(&fid, file, tlink, oplock);
	if (file_info == NULL) {
		if (server->ops->close)
			server->ops->close(xid, tcon, &fid);
		cifs_del_pending_open(&open);
		fput(file);
		rc = -ENOMEM;
	}

out:
	cifs_put_tlink(tlink);
out_free_xid:
	free_xid(xid);
	return rc;
}

int cifs_create(struct inode *inode, struct dentry *direntry, umode_t mode,
		bool excl)
{
	int rc;
	unsigned int xid = get_xid();
	/*
                                                             
                                                            
                                                                   
                                                                
                       
  */
	unsigned oflags = O_EXCL | O_CREAT | O_RDWR;
	struct tcon_link *tlink;
	struct cifs_tcon *tcon;
	struct TCP_Server_Info *server;
	struct cifs_fid fid;
	__u32 oplock;
	int created = FILE_CREATED;

	cifs_dbg(FYI, "cifs_create parent inode = 0x%p name is: %s and dentry = 0x%p\n",
		 inode, direntry->d_name.name, direntry);

	tlink = cifs_sb_tlink(CIFS_SB(inode->i_sb));
	rc = PTR_ERR(tlink);
	if (IS_ERR(tlink))
		goto out_free_xid;

	tcon = tlink_tcon(tlink);
	server = tcon->ses->server;

	if (server->ops->new_lease_key)
		server->ops->new_lease_key(&fid);

	rc = cifs_do_create(inode, direntry, xid, tlink, oflags, mode,
			    &oplock, &fid, &created);
	if (!rc && server->ops->close)
		server->ops->close(xid, tcon, &fid);

	cifs_put_tlink(tlink);
out_free_xid:
	free_xid(xid);
	return rc;
}

int cifs_mknod(struct inode *inode, struct dentry *direntry, umode_t mode,
		dev_t device_number)
{
	int rc = -EPERM;
	unsigned int xid;
	int create_options = CREATE_NOT_DIR | CREATE_OPTION_SPECIAL;
	struct cifs_sb_info *cifs_sb;
	struct tcon_link *tlink;
	struct cifs_tcon *pTcon;
	struct cifs_io_parms io_parms;
	char *full_path = NULL;
	struct inode *newinode = NULL;
	int oplock = 0;
	u16 fileHandle;
	FILE_ALL_INFO *buf = NULL;
	unsigned int bytes_written;
	struct win_dev *pdev;

	if (!old_valid_dev(device_number))
		return -EINVAL;

	cifs_sb = CIFS_SB(inode->i_sb);
	tlink = cifs_sb_tlink(cifs_sb);
	if (IS_ERR(tlink))
		return PTR_ERR(tlink);

	pTcon = tlink_tcon(tlink);

	xid = get_xid();

	full_path = build_path_from_dentry(direntry);
	if (full_path == NULL) {
		rc = -ENOMEM;
		goto mknod_out;
	}

	if (pTcon->unix_ext) {
		struct cifs_unix_set_info_args args = {
			.mode	= mode & ~current_umask(),
			.ctime	= NO_CHANGE_64,
			.atime	= NO_CHANGE_64,
			.mtime	= NO_CHANGE_64,
			.device	= device_number,
		};
		if (cifs_sb->mnt_cifs_flags & CIFS_MOUNT_SET_UID) {
			args.uid = current_fsuid();
			args.gid = current_fsgid();
		} else {
			args.uid = INVALID_UID; /*           */
			args.gid = INVALID_GID; /*           */
		}
		rc = CIFSSMBUnixSetPathInfo(xid, pTcon, full_path, &args,
					    cifs_sb->local_nls,
					    cifs_sb->mnt_cifs_flags &
						CIFS_MOUNT_MAP_SPECIAL_CHR);
		if (rc)
			goto mknod_out;

		rc = cifs_get_inode_info_unix(&newinode, full_path,
						inode->i_sb, xid);

		if (rc == 0)
			d_instantiate(direntry, newinode);
		goto mknod_out;
	}

	if (!(cifs_sb->mnt_cifs_flags & CIFS_MOUNT_UNX_EMUL))
		goto mknod_out;


	cifs_dbg(FYI, "sfu compat create special file\n");

	buf = kmalloc(sizeof(FILE_ALL_INFO), GFP_KERNEL);
	if (buf == NULL) {
		kfree(full_path);
		rc = -ENOMEM;
		free_xid(xid);
		return rc;
	}

	if (backup_cred(cifs_sb))
		create_options |= CREATE_OPEN_BACKUP_INTENT;

	rc = CIFSSMBOpen(xid, pTcon, full_path, FILE_CREATE,
			 GENERIC_WRITE, create_options,
			 &fileHandle, &oplock, buf, cifs_sb->local_nls,
			 cifs_sb->mnt_cifs_flags & CIFS_MOUNT_MAP_SPECIAL_CHR);
	if (rc)
		goto mknod_out;

	/*                                                               
                                              */

	pdev = (struct win_dev *)buf;
	io_parms.netfid = fileHandle;
	io_parms.pid = current->tgid;
	io_parms.tcon = pTcon;
	io_parms.offset = 0;
	io_parms.length = sizeof(struct win_dev);
	if (S_ISCHR(mode)) {
		memcpy(pdev->type, "IntxCHR", 8);
		pdev->major =
		      cpu_to_le64(MAJOR(device_number));
		pdev->minor =
		      cpu_to_le64(MINOR(device_number));
		rc = CIFSSMBWrite(xid, &io_parms,
			&bytes_written, (char *)pdev,
			NULL, 0);
	} else if (S_ISBLK(mode)) {
		memcpy(pdev->type, "IntxBLK", 8);
		pdev->major =
		      cpu_to_le64(MAJOR(device_number));
		pdev->minor =
		      cpu_to_le64(MINOR(device_number));
		rc = CIFSSMBWrite(xid, &io_parms,
			&bytes_written, (char *)pdev,
			NULL, 0);
	} /*                    */
	CIFSSMBClose(xid, pTcon, fileHandle);
	d_drop(direntry);

	/*                                 */

mknod_out:
	kfree(full_path);
	kfree(buf);
	free_xid(xid);
	cifs_put_tlink(tlink);
	return rc;
}

struct dentry *
cifs_lookup(struct inode *parent_dir_inode, struct dentry *direntry,
	    unsigned int flags)
{
	unsigned int xid;
	int rc = 0; /*                                                      */
	struct cifs_sb_info *cifs_sb;
	struct tcon_link *tlink;
	struct cifs_tcon *pTcon;
	struct inode *newInode = NULL;
	char *full_path = NULL;

	xid = get_xid();

	cifs_dbg(FYI, "parent inode = 0x%p name is: %s and dentry = 0x%p\n",
		 parent_dir_inode, direntry->d_name.name, direntry);

	/*                           */

	cifs_sb = CIFS_SB(parent_dir_inode->i_sb);
	tlink = cifs_sb_tlink(cifs_sb);
	if (IS_ERR(tlink)) {
		free_xid(xid);
		return (struct dentry *)tlink;
	}
	pTcon = tlink_tcon(tlink);

	rc = check_name(direntry);
	if (rc)
		goto lookup_out;

	/*                                                
                                                       
                                            */
	full_path = build_path_from_dentry(direntry);
	if (full_path == NULL) {
		rc = -ENOMEM;
		goto lookup_out;
	}

	if (direntry->d_inode != NULL) {
		cifs_dbg(FYI, "non-NULL inode in lookup\n");
	} else {
		cifs_dbg(FYI, "NULL inode in lookup\n");
	}
	cifs_dbg(FYI, "Full path: %s inode = 0x%p\n",
		 full_path, direntry->d_inode);

	if (pTcon->unix_ext) {
		rc = cifs_get_inode_info_unix(&newInode, full_path,
					      parent_dir_inode->i_sb, xid);
	} else {
		rc = cifs_get_inode_info(&newInode, full_path, NULL,
				parent_dir_inode->i_sb, xid, NULL);
	}

	if ((rc == 0) && (newInode != NULL)) {
		d_add(direntry, newInode);
		/*                                                        
                                              */
		renew_parental_timestamps(direntry);

	} else if (rc == -ENOENT) {
		rc = 0;
		direntry->d_time = jiffies;
		d_add(direntry, NULL);
	/*                                                              
                                  */
	} else if (rc != -EACCES) {
		cifs_dbg(VFS, "Unexpected lookup error %d\n", rc);
		/*                                                     
                          */
	}

lookup_out:
	kfree(full_path);
	cifs_put_tlink(tlink);
	free_xid(xid);
	return ERR_PTR(rc);
}

static int
cifs_d_revalidate(struct dentry *direntry, unsigned int flags)
{
	if (flags & LOOKUP_RCU)
		return -ECHILD;

	if (direntry->d_inode) {
		if (cifs_revalidate_dentry(direntry))
			return 0;
		else {
			/*
                                                      
                                                       
                                                       
                                          
                               
    */
			if (IS_AUTOMOUNT(direntry->d_inode) &&
			   !(direntry->d_flags & DCACHE_NEED_AUTOMOUNT)) {
				spin_lock(&direntry->d_lock);
				direntry->d_flags |= DCACHE_NEED_AUTOMOUNT;
				spin_unlock(&direntry->d_lock);
			}

			return 1;
		}
	}

	/*
                                                             
                                                                
  */
	if (!flags)
		return 0;

	/*
                                                              
                                                             
                 
  */
	if (flags & (LOOKUP_CREATE | LOOKUP_RENAME_TARGET))
		return 0;

	if (time_after(jiffies, direntry->d_time + HZ) || !lookupCacheEnabled)
		return 0;

	return 1;
}

/*                                                  
 
            

                                                                       

           
      */

const struct dentry_operations cifs_dentry_ops = {
	.d_revalidate = cifs_d_revalidate,
	.d_automount = cifs_dfs_d_automount,
/*                                     */ /*                                 */
};

static int cifs_ci_hash(const struct dentry *dentry, const struct inode *inode,
		struct qstr *q)
{
	struct nls_table *codepage = CIFS_SB(dentry->d_sb)->local_nls;
	unsigned long hash;
	int i;

	hash = init_name_hash();
	for (i = 0; i < q->len; i++)
		hash = partial_name_hash(nls_tolower(codepage, q->name[i]),
					 hash);
	q->hash = end_name_hash(hash);

	return 0;
}

static int cifs_ci_compare(const struct dentry *parent,
		const struct inode *pinode,
		const struct dentry *dentry, const struct inode *inode,
		unsigned int len, const char *str, const struct qstr *name)
{
	struct nls_table *codepage = CIFS_SB(pinode->i_sb)->local_nls;

	if ((name->len == len) &&
	    (nls_strnicmp(codepage, name->name, str, len) == 0))
		return 0;
	return 1;
}

const struct dentry_operations cifs_ci_dentry_ops = {
	.d_revalidate = cifs_d_revalidate,
	.d_hash = cifs_ci_hash,
	.d_compare = cifs_ci_compare,
	.d_automount = cifs_dfs_d_automount,
};
