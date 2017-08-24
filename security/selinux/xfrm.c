/*
 *  NSA Security-Enhanced Linux (SELinux) security module
 *
 *  This file contains the SELinux XFRM hook function implementations.
 *
 *  Authors:  Serge Hallyn <sergeh@us.ibm.com>
 *	      Trent Jaeger <jaegert@us.ibm.com>
 *
 *  Updated: Venkat Yekkirala <vyekkirala@TrustedCS.com>
 *
 *           Granular IPSec Associations for use in MLS environments.
 *
 *  Copyright (C) 2005 International Business Machines Corporation
 *  Copyright (C) 2006 Trusted Computer Solutions, Inc.
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License version 2,
 *	as published by the Free Software Foundation.
 */

/*
         
         
                                                                        
                    
                            
                                 
                              
          
                                                                   
                                                         
                                                                            
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/security.h>
#include <linux/types.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netfilter_ipv6.h>
#include <linux/slab.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/skbuff.h>
#include <linux/xfrm.h>
#include <net/xfrm.h>
#include <net/checksum.h>
#include <net/udp.h>
#include <linux/atomic.h>

#include "avc.h"
#include "objsec.h"
#include "xfrm.h"

/*                               */
atomic_t selinux_xfrm_refcount = ATOMIC_INIT(0);

/*
                                         
 */
static inline int selinux_authorizable_ctx(struct xfrm_sec_ctx *ctx)
{
	return (ctx &&
		(ctx->ctx_doi == XFRM_SC_DOI_LSM) &&
		(ctx->ctx_alg == XFRM_SC_ALG_SELINUX));
}

/*
                                                                
 */
static inline int selinux_authorizable_xfrm(struct xfrm_state *x)
{
	return selinux_authorizable_ctx(x->security);
}

/*
                                                              
                      
 */
int selinux_xfrm_policy_lookup(struct xfrm_sec_ctx *ctx, u32 fl_secid, u8 dir)
{
	int rc;
	u32 sel_sid;

	/*                                                 */
	if (ctx) {
		if (!selinux_authorizable_ctx(ctx))
			return -EINVAL;

		sel_sid = ctx->ctx_sid;
	} else
		/*
                                                   
                                                    
                                       
   */
		return 0;

	rc = avc_has_perm(fl_secid, sel_sid, SECCLASS_ASSOCIATION,
			  ASSOCIATION__POLMATCH,
			  NULL);

	if (rc == -EACCES)
		return -ESRCH;

	return rc;
}

/*
                                                               
                                
 */

int selinux_xfrm_state_pol_flow_match(struct xfrm_state *x, struct xfrm_policy *xp,
			const struct flowi *fl)
{
	u32 state_sid;
	int rc;

	if (!xp->security)
		if (x->security)
			/*                                             */
			return 0;
		else
			/*                                                   */
			return 1;
	else
		if (!x->security)
			/*                                             */
			return 0;
		else
			if (!selinux_authorizable_xfrm(x))
				/*                          */
				return 0;

	state_sid = x->security->ctx_sid;

	if (fl->flowi_secid != state_sid)
		return 0;

	rc = avc_has_perm(fl->flowi_secid, state_sid, SECCLASS_ASSOCIATION,
			  ASSOCIATION__SENDTO,
			  NULL)? 0:1;

	/*
                                                         
                                                         
                                                         
                                          
  */

	return rc;
}

static int selinux_xfrm_skb_sid_ingress(struct sk_buff *skb,
					u32 *sid, int ckall)
{
	struct sec_path *sp = skb->sp;

	*sid = SECSID_NULL;

	if (sp) {
		int i, sid_set = 0;

		for (i = sp->len-1; i >= 0; i--) {
			struct xfrm_state *x = sp->xvec[i];
			if (selinux_authorizable_xfrm(x)) {
				struct xfrm_sec_ctx *ctx = x->security;

				if (!sid_set) {
					*sid = ctx->ctx_sid;
					sid_set = 1;

					if (!ckall)
						break;
				} else if (*sid != ctx->ctx_sid)
					return -EINVAL;
			}
		}
	}

	return 0;
}

static u32 selinux_xfrm_skb_sid_egress(struct sk_buff *skb)
{
	struct dst_entry *dst = skb_dst(skb);
	struct xfrm_state *x;

	if (dst == NULL)
		return SECSID_NULL;
	x = dst->xfrm;
	if (x == NULL || !selinux_authorizable_xfrm(x))
		return SECSID_NULL;

	return x->security->ctx_sid;
}

/*
                                                                          
                   
 */

int selinux_xfrm_decode_session(struct sk_buff *skb, u32 *sid, int ckall)
{
	if (skb == NULL) {
		*sid = SECSID_NULL;
		return 0;
	}
	return selinux_xfrm_skb_sid_ingress(skb, sid, ckall);
}

int selinux_xfrm_skb_sid(struct sk_buff *skb, u32 *sid)
{
	int rc;

	rc = selinux_xfrm_skb_sid_ingress(skb, sid, 0);
	if (rc == 0 && *sid == SECSID_NULL)
		*sid = selinux_xfrm_skb_sid_egress(skb);

	return rc;
}

/*
                                                          
                                                
 */
static int selinux_xfrm_sec_ctx_alloc(struct xfrm_sec_ctx **ctxp,
	struct xfrm_user_sec_ctx *uctx, u32 sid)
{
	int rc = 0;
	const struct task_security_struct *tsec = current_security();
	struct xfrm_sec_ctx *ctx = NULL;
	char *ctx_str = NULL;
	u32 str_len;

	BUG_ON(uctx && sid);

	if (!uctx)
		goto not_from_user;

	if (uctx->ctx_alg != XFRM_SC_ALG_SELINUX)
		return -EINVAL;

	str_len = uctx->ctx_len;
	if (str_len >= PAGE_SIZE)
		return -ENOMEM;

	*ctxp = ctx = kmalloc(sizeof(*ctx) +
			      str_len + 1,
			      GFP_KERNEL);

	if (!ctx)
		return -ENOMEM;

	ctx->ctx_doi = uctx->ctx_doi;
	ctx->ctx_len = str_len;
	ctx->ctx_alg = uctx->ctx_alg;

	memcpy(ctx->ctx_str,
	       uctx+1,
	       str_len);
	ctx->ctx_str[str_len] = 0;
	rc = security_context_to_sid(ctx->ctx_str,
				     str_len,
				     &ctx->ctx_sid);

	if (rc)
		goto out;

	/*
                                                             
  */
	rc = avc_has_perm(tsec->sid, ctx->ctx_sid,
			  SECCLASS_ASSOCIATION,
			  ASSOCIATION__SETCONTEXT, NULL);
	if (rc)
		goto out;

	return rc;

not_from_user:
	rc = security_sid_to_context(sid, &ctx_str, &str_len);
	if (rc)
		goto out;

	*ctxp = ctx = kmalloc(sizeof(*ctx) +
			      str_len,
			      GFP_ATOMIC);

	if (!ctx) {
		rc = -ENOMEM;
		goto out;
	}

	ctx->ctx_doi = XFRM_SC_DOI_LSM;
	ctx->ctx_alg = XFRM_SC_ALG_SELINUX;
	ctx->ctx_sid = sid;
	ctx->ctx_len = str_len;
	memcpy(ctx->ctx_str,
	       ctx_str,
	       str_len);

	goto out2;

out:
	*ctxp = NULL;
	kfree(ctx);
out2:
	kfree(ctx_str);
	return rc;
}

/*
                                                                 
               
 */
int selinux_xfrm_policy_alloc(struct xfrm_sec_ctx **ctxp,
			      struct xfrm_user_sec_ctx *uctx)
{
	int err;

	BUG_ON(!uctx);

	err = selinux_xfrm_sec_ctx_alloc(ctxp, uctx, 0);
	if (err == 0)
		atomic_inc(&selinux_xfrm_refcount);

	return err;
}


/*
                                                                          
                          
 */
int selinux_xfrm_policy_clone(struct xfrm_sec_ctx *old_ctx,
			      struct xfrm_sec_ctx **new_ctxp)
{
	struct xfrm_sec_ctx *new_ctx;

	if (old_ctx) {
		new_ctx = kmalloc(sizeof(*old_ctx) + old_ctx->ctx_len,
				  GFP_ATOMIC);
		if (!new_ctx)
			return -ENOMEM;

		memcpy(new_ctx, old_ctx, sizeof(*new_ctx));
		memcpy(new_ctx->ctx_str, old_ctx->ctx_str, new_ctx->ctx_len);
		atomic_inc(&selinux_xfrm_refcount);
		*new_ctxp = new_ctx;
	}
	return 0;
}

/*
                                                                        
 */
void selinux_xfrm_policy_free(struct xfrm_sec_ctx *ctx)
{
	atomic_dec(&selinux_xfrm_refcount);
	kfree(ctx);
}

/*
                                                                        
 */
int selinux_xfrm_policy_delete(struct xfrm_sec_ctx *ctx)
{
	const struct task_security_struct *tsec = current_security();

	if (!ctx)
		return 0;

	return avc_has_perm(tsec->sid, ctx->ctx_sid,
			    SECCLASS_ASSOCIATION, ASSOCIATION__SETCONTEXT,
			    NULL);
}

/*
                                                                    
              
 */
int selinux_xfrm_state_alloc(struct xfrm_state *x, struct xfrm_user_sec_ctx *uctx,
		u32 secid)
{
	int err;

	BUG_ON(!x);

	err = selinux_xfrm_sec_ctx_alloc(&x->security, uctx, secid);
	if (err == 0)
		atomic_inc(&selinux_xfrm_refcount);
	return err;
}

/*
                                                                      
 */
void selinux_xfrm_state_free(struct xfrm_state *x)
{
	atomic_dec(&selinux_xfrm_refcount);
	kfree(x->security);
}

 /*
                                                                    
  */
int selinux_xfrm_state_delete(struct xfrm_state *x)
{
	const struct task_security_struct *tsec = current_security();
	struct xfrm_sec_ctx *ctx = x->security;

	if (!ctx)
		return 0;

	return avc_has_perm(tsec->sid, ctx->ctx_sid,
			    SECCLASS_ASSOCIATION, ASSOCIATION__SETCONTEXT,
			    NULL);
}

/*
                                                           
                                                              
                                                         
                                                                 
                               
 */
int selinux_xfrm_sock_rcv_skb(u32 isec_sid, struct sk_buff *skb,
				struct common_audit_data *ad)
{
	int i, rc = 0;
	struct sec_path *sp;
	u32 sel_sid = SECINITSID_UNLABELED;

	sp = skb->sp;

	if (sp) {
		for (i = 0; i < sp->len; i++) {
			struct xfrm_state *x = sp->xvec[i];

			if (x && selinux_authorizable_xfrm(x)) {
				struct xfrm_sec_ctx *ctx = x->security;
				sel_sid = ctx->ctx_sid;
				break;
			}
		}
	}

	/*
                                                           
                                                       
                                                          
                                 
  */

	rc = avc_has_perm(isec_sid, sel_sid, SECCLASS_ASSOCIATION,
			  ASSOCIATION__RECVFROM, ad);

	return rc;
}

/*
                                         
                                                                
                                                                      
                                                                              
                                                               
 */
int selinux_xfrm_postroute_last(u32 isec_sid, struct sk_buff *skb,
					struct common_audit_data *ad, u8 proto)
{
	struct dst_entry *dst;
	int rc = 0;

	dst = skb_dst(skb);

	if (dst) {
		struct dst_entry *dst_test;

		for (dst_test = dst; dst_test != NULL;
		     dst_test = dst_test->child) {
			struct xfrm_state *x = dst_test->xfrm;

			if (x && selinux_authorizable_xfrm(x))
				goto out;
		}
	}

	switch (proto) {
	case IPPROTO_AH:
	case IPPROTO_ESP:
	case IPPROTO_COMP:
		/*
                                                        
                                                       
                     
   */
		goto out;
	default:
		break;
	}

	/*
                                                           
                                                       
                                                          
                                 
  */

	rc = avc_has_perm(isec_sid, SECINITSID_UNLABELED, SECCLASS_ASSOCIATION,
			  ASSOCIATION__SENDTO, ad);
out:
	return rc;
}