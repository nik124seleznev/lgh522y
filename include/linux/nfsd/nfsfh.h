/*
 * include/linux/nfsd/nfsfh.h
 *
 * This file describes the layout of the file handles as passed
 * over the wire.
 *
 * Earlier versions of knfsd used to sign file handles using keyed MD5
 * or SHA. I've removed this code, because it doesn't give you more
 * security than blocking external access to port 2049 on your firewall.
 *
 * Copyright (C) 1995, 1996, 1997 Olaf Kirch <okir@monad.swb.de>
 */
#ifndef _LINUX_NFSD_FH_H
#define _LINUX_NFSD_FH_H

# include <linux/sunrpc/svc.h>
#include <uapi/linux/nfsd/nfsfh.h>

static inline __u32 ino_t_to_u32(ino_t ino)
{
	return (__u32) ino;
}

static inline ino_t u32_to_ino_t(__u32 uino)
{
	return (ino_t) uino;
}

/*
                                                                      
                                                                      
 */
typedef struct svc_fh {
	struct knfsd_fh		fh_handle;	/*         */
	struct dentry *		fh_dentry;	/*                  */
	struct svc_export *	fh_export;	/*                */
	int			fh_maxsize;	/*                        */

	unsigned char		fh_locked;	/*                    */
	unsigned char		fh_want_write;	/*                          */

#ifdef CONFIG_NFSD_V3
	unsigned char		fh_post_saved;	/*                     */
	unsigned char		fh_pre_saved;	/*                    */

	/*                                        */
	__u64			fh_pre_size;	/*                       */
	struct timespec		fh_pre_mtime;	/*                   */
	struct timespec		fh_pre_ctime;	/*                   */
	/*
                                                                 
                                
  */
	u64			fh_pre_change;

	/*                                       */
	struct kstat		fh_post_attr;	/*                            */
	u64			fh_post_change; /*                         */
#endif /*                */

} svc_fh;

#endif /*                  */
