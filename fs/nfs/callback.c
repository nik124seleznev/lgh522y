/*
 * linux/fs/nfs/callback.c
 *
 * Copyright (C) 2004 Trond Myklebust
 *
 * NFSv4 callback handling
 */

#include <linux/completion.h>
#include <linux/ip.h>
#include <linux/module.h>
#include <linux/sunrpc/svc.h>
#include <linux/sunrpc/svcsock.h>
#include <linux/nfs_fs.h>
#include <linux/errno.h>
#include <linux/mutex.h>
#include <linux/freezer.h>
#include <linux/kthread.h>
#include <linux/sunrpc/svcauth_gss.h>
#include <linux/sunrpc/bc_xprt.h>

#include <net/inet_sock.h>

#include "nfs4_fs.h"
#include "callback.h"
#include "internal.h"
#include "netns.h"

#define NFSDBG_FACILITY NFSDBG_CALLBACK

struct nfs_callback_data {
	unsigned int users;
	struct svc_serv *serv;
	struct svc_rqst *rqst;
	struct task_struct *task;
};

static struct nfs_callback_data nfs_callback_info[NFS4_MAX_MINOR_VERSION + 1];
static DEFINE_MUTEX(nfs_callback_mutex);
static struct svc_program nfs4_callback_program;

static int nfs4_callback_up_net(struct svc_serv *serv, struct net *net)
{
	int ret;
	struct nfs_net *nn = net_generic(net, nfs_net_id);

	ret = svc_create_xprt(serv, "tcp", net, PF_INET,
				nfs_callback_set_tcpport, SVC_SOCK_ANONYMOUS);
	if (ret <= 0)
		goto out_err;
	nn->nfs_callback_tcpport = ret;
	dprintk("NFS: Callback listener port = %u (af %u, net %p)\n",
			nn->nfs_callback_tcpport, PF_INET, net);

	ret = svc_create_xprt(serv, "tcp", net, PF_INET6,
				nfs_callback_set_tcpport, SVC_SOCK_ANONYMOUS);
	if (ret > 0) {
		nn->nfs_callback_tcpport6 = ret;
		dprintk("NFS: Callback listener port = %u (af %u, net %p)\n",
				nn->nfs_callback_tcpport6, PF_INET6, net);
	} else if (ret != -EAFNOSUPPORT)
		goto out_err;
	return 0;

out_err:
	return (ret) ? ret : -ENOMEM;
}

/*
                                            
 */
static int
nfs4_callback_svc(void *vrqstp)
{
	int err;
	struct svc_rqst *rqstp = vrqstp;

	set_freezable();

	while (!kthread_should_stop()) {
		/*
                                       
   */
		err = svc_recv(rqstp, MAX_SCHEDULE_TIMEOUT);
		if (err == -EAGAIN || err == -EINTR)
			continue;
		svc_process(rqstp);
	}
	return 0;
}

/*
                                                 
 */
static struct svc_rqst *
nfs4_callback_up(struct svc_serv *serv)
{
	return svc_prepare_thread(serv, &serv->sv_pools[0], NUMA_NO_NODE);
}

#if defined(CONFIG_NFS_V4_1)
static int nfs41_callback_up_net(struct svc_serv *serv, struct net *net)
{
	/*
                                                                   
                            
                                                                       
  */
	return svc_create_xprt(serv, "tcp-bc", net, PF_INET, 0,
			      SVC_SOCK_ANONYMOUS);
}

/*
                                             
 */
static int
nfs41_callback_svc(void *vrqstp)
{
	struct svc_rqst *rqstp = vrqstp;
	struct svc_serv *serv = rqstp->rq_server;
	struct rpc_rqst *req;
	int error;
	DEFINE_WAIT(wq);

	set_freezable();

	while (!kthread_should_stop()) {
		if (try_to_freeze())
			continue;

		prepare_to_wait(&serv->sv_cb_waitq, &wq, TASK_INTERRUPTIBLE);
		spin_lock_bh(&serv->sv_cb_lock);
		if (!list_empty(&serv->sv_cb_list)) {
			req = list_first_entry(&serv->sv_cb_list,
					struct rpc_rqst, rq_bc_list);
			list_del(&req->rq_bc_list);
			spin_unlock_bh(&serv->sv_cb_lock);
			dprintk("Invoking bc_svc_process()\n");
			error = bc_svc_process(serv, req, rqstp);
			dprintk("bc_svc_process() returned w/ error code= %d\n",
				error);
		} else {
			spin_unlock_bh(&serv->sv_cb_lock);
			schedule();
		}
		finish_wait(&serv->sv_cb_waitq, &wq);
	}
	return 0;
}

/*
                                        
 */
static struct svc_rqst *
nfs41_callback_up(struct svc_serv *serv)
{
	struct svc_rqst *rqstp;

	INIT_LIST_HEAD(&serv->sv_cb_list);
	spin_lock_init(&serv->sv_cb_lock);
	init_waitqueue_head(&serv->sv_cb_waitq);
	rqstp = svc_prepare_thread(serv, &serv->sv_pools[0], NUMA_NO_NODE);
	if (IS_ERR(rqstp)) {
		svc_xprt_put(serv->sv_bc_xprt);
		serv->sv_bc_xprt = NULL;
	}
	dprintk("--> %s return %ld\n", __func__,
		IS_ERR(rqstp) ? PTR_ERR(rqstp) : 0);
	return rqstp;
}

static void nfs_minorversion_callback_svc_setup(struct svc_serv *serv,
		struct svc_rqst **rqstpp, int (**callback_svc)(void *vrqstp))
{
	*rqstpp = nfs41_callback_up(serv);
	*callback_svc = nfs41_callback_svc;
}

static inline void nfs_callback_bc_serv(u32 minorversion, struct rpc_xprt *xprt,
		struct svc_serv *serv)
{
	if (minorversion)
		/*
                                                      
                                                              
   */
		xprt->bc_serv = serv;
}
#else
static int nfs41_callback_up_net(struct svc_serv *serv, struct net *net)
{
	return 0;
}

static void nfs_minorversion_callback_svc_setup(struct svc_serv *serv,
		struct svc_rqst **rqstpp, int (**callback_svc)(void *vrqstp))
{
	*rqstpp = ERR_PTR(-ENOTSUPP);
	*callback_svc = ERR_PTR(-ENOTSUPP);
}

static inline void nfs_callback_bc_serv(u32 minorversion, struct rpc_xprt *xprt,
		struct svc_serv *serv)
{
}
#endif /*                 */

static int nfs_callback_start_svc(int minorversion, struct rpc_xprt *xprt,
				  struct svc_serv *serv)
{
	struct svc_rqst *rqstp;
	int (*callback_svc)(void *vrqstp);
	struct nfs_callback_data *cb_info = &nfs_callback_info[minorversion];
	char svc_name[12];
	int ret;

	nfs_callback_bc_serv(minorversion, xprt, serv);

	if (cb_info->task)
		return 0;

	switch (minorversion) {
	case 0:
		/*                     */
		rqstp = nfs4_callback_up(serv);
		callback_svc = nfs4_callback_svc;
		break;
	default:
		nfs_minorversion_callback_svc_setup(serv,
				&rqstp, &callback_svc);
	}

	if (IS_ERR(rqstp))
		return PTR_ERR(rqstp);

	svc_sock_update_bufs(serv);

	sprintf(svc_name, "nfsv4.%u-svc", minorversion);
	cb_info->serv = serv;
	cb_info->rqst = rqstp;
	cb_info->task = kthread_run(callback_svc, cb_info->rqst, svc_name);
	if (IS_ERR(cb_info->task)) {
		ret = PTR_ERR(cb_info->task);
		svc_exit_thread(cb_info->rqst);
		cb_info->rqst = NULL;
		cb_info->task = NULL;
		return ret;
	}
	dprintk("nfs_callback_up: service started\n");
	return 0;
}

static void nfs_callback_down_net(u32 minorversion, struct svc_serv *serv, struct net *net)
{
	struct nfs_net *nn = net_generic(net, nfs_net_id);

	if (--nn->cb_users[minorversion])
		return;

	dprintk("NFS: destroy per-net callback data; net=%p\n", net);
	svc_shutdown_net(serv, net);
}

static int nfs_callback_up_net(int minorversion, struct svc_serv *serv, struct net *net)
{
	struct nfs_net *nn = net_generic(net, nfs_net_id);
	int ret;

	if (nn->cb_users[minorversion]++)
		return 0;

	dprintk("NFS: create per-net callback data; net=%p\n", net);

	ret = svc_bind(serv, net);
	if (ret < 0) {
		printk(KERN_WARNING "NFS: bind callback service failed\n");
		goto err_bind;
	}

	switch (minorversion) {
		case 0:
			ret = nfs4_callback_up_net(serv, net);
			break;
		case 1:
			ret = nfs41_callback_up_net(serv, net);
			break;
		default:
			printk(KERN_ERR "NFS: unknown callback version: %d\n",
					minorversion);
			ret = -EINVAL;
			break;
	}

	if (ret < 0) {
		printk(KERN_ERR "NFS: callback service start failed\n");
		goto err_socks;
	}
	return 0;

err_socks:
	svc_rpcb_cleanup(serv, net);
err_bind:
	dprintk("NFS: Couldn't create callback socket: err = %d; "
			"net = %p\n", ret, net);
	return ret;
}

static struct svc_serv *nfs_callback_create_svc(int minorversion)
{
	struct nfs_callback_data *cb_info = &nfs_callback_info[minorversion];
	struct svc_serv *serv;

	/*
                                               
  */
	if (cb_info->task) {
		/*
                                                                 
                                  
   */
		svc_get(cb_info->serv);
		return cb_info->serv;
	}

	/*
                                     
                                   
  */
	if (cb_info->users)
		printk(KERN_WARNING "nfs_callback_create_svc: no kthread, %d users??\n",
			cb_info->users);

	serv = svc_create(&nfs4_callback_program, NFS4_CALLBACK_BUFSIZE, NULL);
	if (!serv) {
		printk(KERN_ERR "nfs_callback_create_svc: create service failed\n");
		return ERR_PTR(-ENOMEM);
	}
	/*                                                     
                                     
  */
	serv->sv_maxconn = 1024;
	dprintk("nfs_callback_create_svc: service created\n");
	return serv;
}

/*
                                                        
 */
int nfs_callback_up(u32 minorversion, struct rpc_xprt *xprt)
{
	struct svc_serv *serv;
	struct nfs_callback_data *cb_info = &nfs_callback_info[minorversion];
	int ret;
	struct net *net = xprt->xprt_net;

	mutex_lock(&nfs_callback_mutex);

	serv = nfs_callback_create_svc(minorversion);
	if (IS_ERR(serv)) {
		ret = PTR_ERR(serv);
		goto err_create;
	}

	ret = nfs_callback_up_net(minorversion, serv, net);
	if (ret < 0)
		goto err_net;

	ret = nfs_callback_start_svc(minorversion, xprt, serv);
	if (ret < 0)
		goto err_start;

	cb_info->users++;
	/*
                                                                    
                                                                      
                                                                  
                 
  */
err_net:
	svc_destroy(serv);
err_create:
	mutex_unlock(&nfs_callback_mutex);
	return ret;

err_start:
	nfs_callback_down_net(minorversion, serv, net);
	dprintk("NFS: Couldn't create server thread; err = %d\n", ret);
	goto err_net;
}

/*
                                                         
 */
void nfs_callback_down(int minorversion, struct net *net)
{
	struct nfs_callback_data *cb_info = &nfs_callback_info[minorversion];

	mutex_lock(&nfs_callback_mutex);
	nfs_callback_down_net(minorversion, cb_info->serv, net);
	cb_info->users--;
	if (cb_info->users == 0 && cb_info->task != NULL) {
		kthread_stop(cb_info->task);
		dprintk("nfs_callback_down: service stopped\n");
		svc_exit_thread(cb_info->rqst);
		dprintk("nfs_callback_down: service destroyed\n");
		cb_info->serv = NULL;
		cb_info->rqst = NULL;
		cb_info->task = NULL;
	}
	mutex_unlock(&nfs_callback_mutex);
}

/*                                         */
int
check_gss_callback_principal(struct nfs_client *clp, struct svc_rqst *rqstp)
{
	char *p = rqstp->rq_cred.cr_principal;

	if (rqstp->rq_authop->flavour != RPC_AUTH_GSS)
		return 1;

	/*                                             */
	if (clp->cl_minorversion != 0)
		return 0;
	/*
                                                           
                                                      
  */
	if (p == NULL)
		return 0;

	/*                                                            */

	if (memcmp(p, "nfs@", 4) != 0)
		return 0;
	p += 4;
	if (strcmp(p, clp->cl_hostname) != 0)
		return 0;
	return 1;
}

/*
                                                     
  
                                                                         
                                               
  
                                                                         
                                  
 */
static int nfs_callback_authenticate(struct svc_rqst *rqstp)
{
	switch (rqstp->rq_authop->flavour) {
	case RPC_AUTH_NULL:
		if (rqstp->rq_proc != CB_NULL)
			return SVC_DROP;
		break;
	case RPC_AUTH_GSS:
		/*                                        */
		 if (svc_is_backchannel(rqstp))
			return SVC_DROP;
	}
	return SVC_OK;
}

/*
                               
 */
static struct svc_version *nfs4_callback_version[] = {
	[1] = &nfs4_callback_version1,
	[4] = &nfs4_callback_version4,
};

static struct svc_stat nfs4_callback_stats;

static struct svc_program nfs4_callback_program = {
	.pg_prog = NFS4_CALLBACK,			/*                    */
	.pg_nvers = ARRAY_SIZE(nfs4_callback_version),	/*                   */
	.pg_vers = nfs4_callback_version,		/*               */
	.pg_name = "NFSv4 callback",			/*              */
	.pg_class = "nfs",				/*                      */
	.pg_stats = &nfs4_callback_stats,
	.pg_authenticate = nfs_callback_authenticate,
};
