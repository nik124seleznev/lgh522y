/* getroot.c: get the root dentry for an NFS mount
 *
 * Copyright (C) 2006 Red Hat, Inc. All Rights Reserved.
 * Written by David Howells (dhowells@redhat.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <linux/module.h>
#include <linux/init.h>

#include <linux/time.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/string.h>
#include <linux/stat.h>
#include <linux/errno.h>
#include <linux/unistd.h>
#include <linux/sunrpc/clnt.h>
#include <linux/sunrpc/stats.h>
#include <linux/nfs_fs.h>
#include <linux/nfs_mount.h>
#include <linux/lockd/bind.h>
#include <linux/seq_file.h>
#include <linux/mount.h>
#include <linux/vfs.h>
#include <linux/namei.h>
#include <linux/security.h>

#include <asm/uaccess.h>

#include "internal.h"

#define NFSDBG_FACILITY		NFSDBG_CLIENT

/*
                                  
                                                            
 */
static int nfs_superblock_set_dummy_root(struct super_block *sb, struct inode *inode)
{
	/*                                                               */
	if (sb->s_root == NULL) {
		sb->s_root = d_make_root(inode);
		if (sb->s_root == NULL)
			return -ENOMEM;
		ihold(inode);
		/*
                                                            
                                                  
                                                             
                                             
                                                            
                                                  
   */
		spin_lock(&sb->s_root->d_inode->i_lock);
		spin_lock(&sb->s_root->d_lock);
		hlist_del_init(&sb->s_root->d_alias);
		spin_unlock(&sb->s_root->d_lock);
		spin_unlock(&sb->s_root->d_inode->i_lock);
	}
	return 0;
}

/*
                                                        
 */
struct dentry *nfs_get_root(struct super_block *sb, struct nfs_fh *mntfh,
			    const char *devname)
{
	struct nfs_server *server = NFS_SB(sb);
	struct nfs_fsinfo fsinfo;
	struct dentry *ret;
	struct inode *inode;
	void *name = kstrdup(devname, GFP_KERNEL);
	int error;

	if (!name)
		return ERR_PTR(-ENOMEM);

	/*                                    */
	fsinfo.fattr = nfs_alloc_fattr();
	if (fsinfo.fattr == NULL) {
		kfree(name);
		return ERR_PTR(-ENOMEM);
	}

	error = server->nfs_client->rpc_ops->getroot(server, mntfh, &fsinfo);
	if (error < 0) {
		dprintk("nfs_get_root: getattr error = %d\n", -error);
		ret = ERR_PTR(error);
		goto out;
	}

	inode = nfs_fhget(sb, mntfh, fsinfo.fattr);
	if (IS_ERR(inode)) {
		dprintk("nfs_get_root: get root inode failed\n");
		ret = ERR_CAST(inode);
		goto out;
	}

	error = nfs_superblock_set_dummy_root(sb, inode);
	if (error != 0) {
		ret = ERR_PTR(error);
		goto out;
	}

	/*                                                                    
                                                                  
                                                                 
  */
	ret = d_obtain_alias(inode);
	if (IS_ERR(ret)) {
		dprintk("nfs_get_root: get root dentry failed\n");
		goto out;
	}

	security_d_instantiate(ret, inode);
	spin_lock(&ret->d_lock);
	if (IS_ROOT(ret) && !(ret->d_flags & DCACHE_NFSFS_RENAMED)) {
		ret->d_fsdata = name;
		name = NULL;
	}
	spin_unlock(&ret->d_lock);
out:
	kfree(name);
	nfs_free_fattr(fsinfo.fattr);
	return ret;
}