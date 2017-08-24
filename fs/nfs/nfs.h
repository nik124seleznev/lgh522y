/*
 * Copyright (c) 2012 Netapp, Inc. All rights reserved.
 *
 * Function and structures exported by the NFS module
 * for use by NFS version-specific modules.
 */
#ifndef __LINUX_INTERNAL_NFS_H
#define __LINUX_INTERNAL_NFS_H

#include <linux/fs.h>
#include <linux/sunrpc/sched.h>
#include <linux/nfs_xdr.h>

struct nfs_subversion {
	struct module *owner;	/*                     */
	struct file_system_type *nfs_fs;	/*                     */
	const struct rpc_version *rpc_vers;	/*                         */
	const struct nfs_rpc_ops *rpc_ops;	/*                */
	const struct super_operations *sops;	/*                      */
	const struct xattr_handler **xattr;	/*                    */
	struct list_head list;		/*                      */
};

struct nfs_subversion *get_nfs_version(unsigned int);
void put_nfs_version(struct nfs_subversion *);
void register_nfs_version(struct nfs_subversion *);
void unregister_nfs_version(struct nfs_subversion *);

#endif /*                        */
