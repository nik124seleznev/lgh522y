/*
 * Generic RPC credential
 *
 * Copyright (C) 2008, Trond Myklebust <Trond.Myklebust@netapp.com>
 */

#include <linux/err.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/sunrpc/auth.h>
#include <linux/sunrpc/clnt.h>
#include <linux/sunrpc/debug.h>
#include <linux/sunrpc/sched.h>

#ifdef RPC_DEBUG
# define RPCDBG_FACILITY	RPCDBG_AUTH
#endif

#define RPC_MACHINE_CRED_USERID		GLOBAL_ROOT_UID
#define RPC_MACHINE_CRED_GROUPID	GLOBAL_ROOT_GID

struct generic_cred {
	struct rpc_cred gc_base;
	struct auth_cred acred;
};

static struct rpc_auth generic_auth;
static const struct rpc_credops generic_credops;

/*
                        
 */
struct rpc_cred *rpc_lookup_cred(void)
{
	return rpcauth_lookupcred(&generic_auth, 0);
}
EXPORT_SYMBOL_GPL(rpc_lookup_cred);

/*
                                                      
 */
struct rpc_cred *rpc_lookup_machine_cred(const char *service_name)
{
	struct auth_cred acred = {
		.uid = RPC_MACHINE_CRED_USERID,
		.gid = RPC_MACHINE_CRED_GROUPID,
		.principal = service_name,
		.machine_cred = 1,
	};

	dprintk("RPC:       looking up machine cred for service %s\n",
			service_name);
	return generic_auth.au_ops->lookup_cred(&generic_auth, &acred, 0);
}
EXPORT_SYMBOL_GPL(rpc_lookup_machine_cred);

static struct rpc_cred *generic_bind_cred(struct rpc_task *task,
		struct rpc_cred *cred, int lookupflags)
{
	struct rpc_auth *auth = task->tk_client->cl_auth;
	struct auth_cred *acred = &container_of(cred, struct generic_cred, gc_base)->acred;

	return auth->au_ops->lookup_cred(auth, acred, lookupflags);
}

/*
                                           
 */
static struct rpc_cred *
generic_lookup_cred(struct rpc_auth *auth, struct auth_cred *acred, int flags)
{
	return rpcauth_lookup_credcache(&generic_auth, acred, flags);
}

static struct rpc_cred *
generic_create_cred(struct rpc_auth *auth, struct auth_cred *acred, int flags)
{
	struct generic_cred *gcred;

	gcred = kmalloc(sizeof(*gcred), GFP_KERNEL);
	if (gcred == NULL)
		return ERR_PTR(-ENOMEM);

	rpcauth_init_cred(&gcred->gc_base, acred, &generic_auth, &generic_credops);
	gcred->gc_base.cr_flags = 1UL << RPCAUTH_CRED_UPTODATE;

	gcred->acred.uid = acred->uid;
	gcred->acred.gid = acred->gid;
	gcred->acred.group_info = acred->group_info;
	if (gcred->acred.group_info != NULL)
		get_group_info(gcred->acred.group_info);
	gcred->acred.machine_cred = acred->machine_cred;
	gcred->acred.principal = acred->principal;

	dprintk("RPC:       allocated %s cred %p for uid %d gid %d\n",
			gcred->acred.machine_cred ? "machine" : "generic",
			gcred,
			from_kuid(&init_user_ns, acred->uid),
			from_kgid(&init_user_ns, acred->gid));
	return &gcred->gc_base;
}

static void
generic_free_cred(struct rpc_cred *cred)
{
	struct generic_cred *gcred = container_of(cred, struct generic_cred, gc_base);

	dprintk("RPC:       generic_free_cred %p\n", gcred);
	if (gcred->acred.group_info != NULL)
		put_group_info(gcred->acred.group_info);
	kfree(gcred);
}

static void
generic_free_cred_callback(struct rcu_head *head)
{
	struct rpc_cred *cred = container_of(head, struct rpc_cred, cr_rcu);
	generic_free_cred(cred);
}

static void
generic_destroy_cred(struct rpc_cred *cred)
{
	call_rcu(&cred->cr_rcu, generic_free_cred_callback);
}

static int
machine_cred_match(struct auth_cred *acred, struct generic_cred *gcred, int flags)
{
	if (!gcred->acred.machine_cred ||
	    gcred->acred.principal != acred->principal ||
	    !uid_eq(gcred->acred.uid, acred->uid) ||
	    !gid_eq(gcred->acred.gid, acred->gid))
		return 0;
	return 1;
}

/*
                                                   
 */
static int
generic_match(struct auth_cred *acred, struct rpc_cred *cred, int flags)
{
	struct generic_cred *gcred = container_of(cred, struct generic_cred, gc_base);
	int i;

	if (acred->machine_cred)
		return machine_cred_match(acred, gcred, flags);

	if (!uid_eq(gcred->acred.uid, acred->uid) ||
	    !gid_eq(gcred->acred.gid, acred->gid) ||
	    gcred->acred.machine_cred != 0)
		goto out_nomatch;

	/*                                                          */
	if (gcred->acred.group_info == acred->group_info)
		goto out_match;

	/*              */
	if (gcred->acred.group_info->ngroups != acred->group_info->ngroups)
		goto out_nomatch;
	for (i = 0; i < gcred->acred.group_info->ngroups; i++) {
		if (!gid_eq(GROUP_AT(gcred->acred.group_info, i),
				GROUP_AT(acred->group_info, i)))
			goto out_nomatch;
	}
out_match:
	return 1;
out_nomatch:
	return 0;
}

int __init rpc_init_generic_auth(void)
{
	return rpcauth_init_credcache(&generic_auth);
}

void rpc_destroy_generic_auth(void)
{
	rpcauth_destroy_credcache(&generic_auth);
}

static const struct rpc_authops generic_auth_ops = {
	.owner = THIS_MODULE,
	.au_name = "Generic",
	.lookup_cred = generic_lookup_cred,
	.crcreate = generic_create_cred,
};

static struct rpc_auth generic_auth = {
	.au_ops = &generic_auth_ops,
	.au_count = ATOMIC_INIT(0),
};

static const struct rpc_credops generic_credops = {
	.cr_name = "Generic cred",
	.crdestroy = generic_destroy_cred,
	.crbind = generic_bind_cred,
	.crmatch = generic_match,
};
