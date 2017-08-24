/*
 * linux/fs/nfs/nfs4_fs.h
 *
 * Copyright (C) 2005 Trond Myklebust
 *
 * NFSv4-specific filesystem definitions and declarations
 */

#ifndef __LINUX_FS_NFS_NFS4_FS_H
#define __LINUX_FS_NFS_NFS4_FS_H

#if IS_ENABLED(CONFIG_NFS_V4)

#define NFS4_MAX_LOOP_ON_RECOVER (10)

#include <linux/seqlock.h>

struct idmap;

enum nfs4_client_state {
	NFS4CLNT_MANAGER_RUNNING  = 0,
	NFS4CLNT_CHECK_LEASE,
	NFS4CLNT_LEASE_EXPIRED,
	NFS4CLNT_RECLAIM_REBOOT,
	NFS4CLNT_RECLAIM_NOGRACE,
	NFS4CLNT_DELEGRETURN,
	NFS4CLNT_SESSION_RESET,
	NFS4CLNT_LEASE_CONFIRM,
	NFS4CLNT_SERVER_SCOPE_MISMATCH,
	NFS4CLNT_PURGE_STATE,
	NFS4CLNT_BIND_CONN_TO_SESSION,
};

#define NFS4_RENEW_TIMEOUT		0x01
#define NFS4_RENEW_DELEGATION_CB	0x02

struct nfs4_minor_version_ops {
	u32	minor_version;
	unsigned init_caps;

	int	(*call_sync)(struct rpc_clnt *clnt,
			struct nfs_server *server,
			struct rpc_message *msg,
			struct nfs4_sequence_args *args,
			struct nfs4_sequence_res *res);
	bool	(*match_stateid)(const nfs4_stateid *,
			const nfs4_stateid *);
	int	(*find_root_sec)(struct nfs_server *, struct nfs_fh *,
			struct nfs_fsinfo *);
	int	(*free_lock_state)(struct nfs_server *,
			struct nfs4_lock_state *);
	const struct nfs4_state_recovery_ops *reboot_recovery_ops;
	const struct nfs4_state_recovery_ops *nograce_recovery_ops;
	const struct nfs4_state_maintenance_ops *state_renewal_ops;
};

#define NFS_SEQID_CONFIRMED 1
struct nfs_seqid_counter {
	ktime_t create_time;
	int owner_id;
	int flags;
	u32 counter;
	spinlock_t lock;		/*                   */
	struct list_head list;		/*                               */
	struct rpc_wait_queue	wait;	/*                      */
};

struct nfs_seqid {
	struct nfs_seqid_counter *sequence;
	struct list_head list;
	struct rpc_task *task;
};

static inline void nfs_confirm_seqid(struct nfs_seqid_counter *seqid, int status)
{
	if (seqid_mutating_err(-status))
		seqid->flags |= NFS_SEQID_CONFIRMED;
}

/*
                                                                  
                                                                     
                                                                  
 */
struct nfs4_state_owner {
	struct nfs_server    *so_server;
	struct list_head     so_lru;
	unsigned long        so_expires;
	struct rb_node	     so_server_node;

	struct rpc_cred	     *so_cred;	 /*                 */

	spinlock_t	     so_lock;
	atomic_t	     so_count;
	unsigned long	     so_flags;
	struct list_head     so_states;
	struct nfs_seqid_counter so_seqid;
	seqcount_t	     so_reclaim_seqcount;
	struct mutex	     so_delegreturn_mutex;
};

enum {
	NFS_OWNER_RECLAIM_REBOOT,
	NFS_OWNER_RECLAIM_NOGRACE
};

#define NFS_LOCK_NEW		0
#define NFS_LOCK_RECLAIM	1
#define NFS_LOCK_EXPIRED	2

/*
                                                                
                                                          
  
        
                                                                            
                                                                      
                                                    
  
                                                                        
 */

struct nfs4_lock_owner {
	unsigned int lo_type;
#define NFS4_ANY_LOCK_TYPE	(0U)
#define NFS4_FLOCK_LOCK_TYPE	(1U << 0)
#define NFS4_POSIX_LOCK_TYPE	(1U << 1)
	union {
		fl_owner_t posix_owner;
		pid_t flock_owner;
	} lo_u;
};

struct nfs4_lock_state {
	struct list_head	ls_locks;	/*                     */
	struct nfs4_state *	ls_state;	/*                       */
#define NFS_LOCK_INITIALIZED 0
	unsigned long		ls_flags;
	struct nfs_seqid_counter	ls_seqid;
	nfs4_stateid		ls_stateid;
	atomic_t		ls_count;
	struct nfs4_lock_owner	ls_owner;
};

/*                            */
enum {
	LK_STATE_IN_USE,
	NFS_DELEGATED_STATE,		/*                               */
	NFS_OPEN_STATE,			/*                     */
	NFS_O_RDONLY_STATE,		/*                                  */
	NFS_O_WRONLY_STATE,		/*                                   */
	NFS_O_RDWR_STATE,		/*                                   */
	NFS_STATE_RECLAIM_REBOOT,	/*                              */
	NFS_STATE_RECLAIM_NOGRACE,	/*                                     */
	NFS_STATE_POSIX_LOCKS,		/*                           */
	NFS_STATE_RECOVERY_FAILED,	/*                                    */
};

struct nfs4_state {
	struct list_head open_states;	/*                                         */
	struct list_head inode_states;	/*                                   */
	struct list_head lock_states;	/*                                   */

	struct nfs4_state_owner *owner;	/*                           */
	struct inode *inode;		/*                      */

	unsigned long flags;		/*                       */
	spinlock_t state_lock;		/*                               */

	seqlock_t seqlock;		/*                                   */
	nfs4_stateid stateid;		/*                                    */
	nfs4_stateid open_stateid;	/*              */

	/*                                                        */
	unsigned int n_rdonly;		/*                                */
	unsigned int n_wronly;		/*                                 */
	unsigned int n_rdwr;		/*                                 */
	fmode_t state;			/*                                  */
	atomic_t count;
};


struct nfs4_exception {
	long timeout;
	int retry;
	struct nfs4_state *state;
	struct inode *inode;
};

struct nfs4_state_recovery_ops {
	int owner_flag_bit;
	int state_flag_bit;
	int (*recover_open)(struct nfs4_state_owner *, struct nfs4_state *);
	int (*recover_lock)(struct nfs4_state *, struct file_lock *);
	int (*establish_clid)(struct nfs_client *, struct rpc_cred *);
	struct rpc_cred * (*get_clid_cred)(struct nfs_client *);
	int (*reclaim_complete)(struct nfs_client *);
	int (*detect_trunking)(struct nfs_client *, struct nfs_client **,
		struct rpc_cred *);
};

struct nfs4_state_maintenance_ops {
	int (*sched_state_renewal)(struct nfs_client *, struct rpc_cred *, unsigned);
	struct rpc_cred * (*get_state_renewal_cred_locked)(struct nfs_client *);
	int (*renew_lease)(struct nfs_client *, struct rpc_cred *);
};

extern const struct dentry_operations nfs4_dentry_operations;

/*       */
int nfs_atomic_open(struct inode *, struct dentry *, struct file *,
		    unsigned, umode_t, int *);

/*         */
extern struct file_system_type nfs4_fs_type;

/*                 */
rpc_authflavor_t nfs_find_best_sec(struct nfs4_secinfo_flavors *);
struct rpc_clnt *nfs4_create_sec_client(struct rpc_clnt *, struct inode *, struct qstr *);
struct vfsmount *nfs4_submount(struct nfs_server *, struct dentry *,
			       struct nfs_fh *, struct nfs_fattr *);

/*            */
extern int nfs4_proc_setclientid(struct nfs_client *, u32, unsigned short, struct rpc_cred *, struct nfs4_setclientid_res *);
extern int nfs4_proc_setclientid_confirm(struct nfs_client *, struct nfs4_setclientid_res *arg, struct rpc_cred *);
extern int nfs4_proc_get_rootfh(struct nfs_server *, struct nfs_fh *, struct nfs_fsinfo *);
extern int nfs4_proc_bind_conn_to_session(struct nfs_client *, struct rpc_cred *cred);
extern int nfs4_proc_exchange_id(struct nfs_client *clp, struct rpc_cred *cred);
extern int nfs4_destroy_clientid(struct nfs_client *clp);
extern int nfs4_init_clientid(struct nfs_client *, struct rpc_cred *);
extern int nfs41_init_clientid(struct nfs_client *, struct rpc_cred *);
extern int nfs4_do_close(struct nfs4_state *state, gfp_t gfp_mask, int wait);
extern int nfs4_server_capabilities(struct nfs_server *server, struct nfs_fh *fhandle);
extern int nfs4_proc_fs_locations(struct rpc_clnt *, struct inode *, const struct qstr *,
				  struct nfs4_fs_locations *, struct page *);
extern struct rpc_clnt *nfs4_proc_lookup_mountpoint(struct inode *, struct qstr *,
			    struct nfs_fh *, struct nfs_fattr *);
extern int nfs4_proc_secinfo(struct inode *, const struct qstr *, struct nfs4_secinfo_flavors *);
extern const struct xattr_handler *nfs4_xattr_handlers[];
extern int nfs4_set_rw_stateid(nfs4_stateid *stateid,
		const struct nfs_open_context *ctx,
		const struct nfs_lock_context *l_ctx,
		fmode_t fmode);

#if defined(CONFIG_NFS_V4_1)
static inline struct nfs4_session *nfs4_get_session(const struct nfs_server *server)
{
	return server->nfs_client->cl_session;
}

extern int nfs4_setup_sequence(const struct nfs_server *server,
		struct nfs4_sequence_args *args, struct nfs4_sequence_res *res,
		struct rpc_task *task);
extern int nfs41_setup_sequence(struct nfs4_session *session,
		struct nfs4_sequence_args *args, struct nfs4_sequence_res *res,
		struct rpc_task *task);
extern int nfs4_proc_create_session(struct nfs_client *, struct rpc_cred *);
extern int nfs4_proc_destroy_session(struct nfs4_session *, struct rpc_cred *);
extern int nfs4_proc_get_lease_time(struct nfs_client *clp,
		struct nfs_fsinfo *fsinfo);
extern int nfs4_proc_layoutcommit(struct nfs4_layoutcommit_data *data,
				  bool sync);

static inline bool
is_ds_only_client(struct nfs_client *clp)
{
	return (clp->cl_exchange_flags & EXCHGID4_FLAG_MASK_PNFS) ==
		EXCHGID4_FLAG_USE_PNFS_DS;
}

static inline bool
is_ds_client(struct nfs_client *clp)
{
	return clp->cl_exchange_flags & EXCHGID4_FLAG_USE_PNFS_DS;
}
#else /*                 */
static inline struct nfs4_session *nfs4_get_session(const struct nfs_server *server)
{
	return NULL;
}

static inline int nfs4_setup_sequence(const struct nfs_server *server,
		struct nfs4_sequence_args *args, struct nfs4_sequence_res *res,
		struct rpc_task *task)
{
	rpc_call_start(task);
	return 0;
}

static inline bool
is_ds_only_client(struct nfs_client *clp)
{
	return false;
}

static inline bool
is_ds_client(struct nfs_client *clp)
{
	return false;
}
#endif /*                 */

extern const struct nfs4_minor_version_ops *nfs_v4_minor_ops[];

extern const u32 nfs4_fattr_bitmap[3];
extern const u32 nfs4_statfs_bitmap[2];
extern const u32 nfs4_pathconf_bitmap[2];
extern const u32 nfs4_fsinfo_bitmap[3];
extern const u32 nfs4_fs_locations_bitmap[2];

void nfs4_free_client(struct nfs_client *);

struct nfs_client *nfs4_alloc_client(const struct nfs_client_initdata *);

/*              */
extern void nfs4_schedule_state_renewal(struct nfs_client *);
extern void nfs4_renewd_prepare_shutdown(struct nfs_server *);
extern void nfs4_kill_renewd(struct nfs_client *);
extern void nfs4_renew_state(struct work_struct *);

/*             */
struct rpc_cred *nfs4_get_setclientid_cred(struct nfs_client *clp);
struct rpc_cred *nfs4_get_machine_cred_locked(struct nfs_client *clp);
struct rpc_cred *nfs4_get_renew_cred_locked(struct nfs_client *clp);
int nfs4_discover_server_trunking(struct nfs_client *clp,
			struct nfs_client **);
int nfs40_discover_server_trunking(struct nfs_client *clp,
			struct nfs_client **, struct rpc_cred *);
#if defined(CONFIG_NFS_V4_1)
struct rpc_cred *nfs4_get_exchange_id_cred(struct nfs_client *clp);
int nfs41_discover_server_trunking(struct nfs_client *clp,
			struct nfs_client **, struct rpc_cred *);
extern void nfs4_schedule_session_recovery(struct nfs4_session *, int);
extern void nfs41_server_notify_target_slotid_update(struct nfs_client *clp);
extern void nfs41_server_notify_highest_slotid_update(struct nfs_client *clp);

#else
static inline void nfs4_schedule_session_recovery(struct nfs4_session *session, int err)
{
}
#endif /*                 */

extern struct nfs4_state_owner *nfs4_get_state_owner(struct nfs_server *, struct rpc_cred *, gfp_t);
extern void nfs4_put_state_owner(struct nfs4_state_owner *);
extern void nfs4_purge_state_owners(struct nfs_server *);
extern struct nfs4_state * nfs4_get_open_state(struct inode *, struct nfs4_state_owner *);
extern void nfs4_put_open_state(struct nfs4_state *);
extern void nfs4_close_state(struct nfs4_state *, fmode_t);
extern void nfs4_close_sync(struct nfs4_state *, fmode_t);
extern void nfs4_state_set_mode_locked(struct nfs4_state *, fmode_t);
extern void nfs_inode_find_state_and_recover(struct inode *inode,
		const nfs4_stateid *stateid);
extern void nfs4_schedule_lease_recovery(struct nfs_client *);
extern int nfs4_wait_clnt_recover(struct nfs_client *clp);
extern int nfs4_client_recover_expired_lease(struct nfs_client *clp);
extern void nfs4_schedule_state_manager(struct nfs_client *);
extern void nfs4_schedule_path_down_recovery(struct nfs_client *clp);
extern int nfs4_schedule_stateid_recovery(const struct nfs_server *, struct nfs4_state *);
extern void nfs41_handle_sequence_flag_errors(struct nfs_client *clp, u32 flags);
extern void nfs41_handle_server_scope(struct nfs_client *,
				      struct nfs41_server_scope **);
extern void nfs4_put_lock_state(struct nfs4_lock_state *lsp);
extern int nfs4_set_lock_state(struct nfs4_state *state, struct file_lock *fl);
extern int nfs4_select_rw_stateid(nfs4_stateid *, struct nfs4_state *,
		fmode_t, const struct nfs_lockowner *);

extern struct nfs_seqid *nfs_alloc_seqid(struct nfs_seqid_counter *counter, gfp_t gfp_mask);
extern int nfs_wait_on_sequence(struct nfs_seqid *seqid, struct rpc_task *task);
extern void nfs_increment_open_seqid(int status, struct nfs_seqid *seqid);
extern void nfs_increment_lock_seqid(int status, struct nfs_seqid *seqid);
extern void nfs_release_seqid(struct nfs_seqid *seqid);
extern void nfs_free_seqid(struct nfs_seqid *seqid);

extern void nfs4_free_lock_state(struct nfs_server *server, struct nfs4_lock_state *lsp);

extern const nfs4_stateid zero_stateid;

/*             */
struct nfs_mount_info;
extern struct nfs_subversion nfs_v4;
struct dentry *nfs4_try_mount(int, const char *, struct nfs_mount_info *, struct nfs_subversion *);
extern bool nfs4_disable_idmapping;
extern unsigned short max_session_slots;
extern unsigned short send_implementation_id;

#define NFS4_CLIENT_ID_UNIQ_LEN		(64)
extern char nfs4_client_id_uniquifier[NFS4_CLIENT_ID_UNIQ_LEN];

/*              */
#ifdef CONFIG_SYSCTL
int nfs4_register_sysctl(void);
void nfs4_unregister_sysctl(void);
#else
static inline int nfs4_register_sysctl(void)
{
	return 0;
}

static inline void nfs4_unregister_sysctl(void)
{
}
#endif

/*           */
extern struct rpc_procinfo nfs4_procedures[];

struct nfs4_mount_data;

/*                */
extern struct svc_version nfs4_callback_version1;
extern struct svc_version nfs4_callback_version4;

static inline void nfs4_stateid_copy(nfs4_stateid *dst, const nfs4_stateid *src)
{
	memcpy(dst, src, sizeof(*dst));
}

static inline bool nfs4_stateid_match(const nfs4_stateid *dst, const nfs4_stateid *src)
{
	return memcmp(dst, src, sizeof(*dst)) == 0;
}

static inline bool nfs4_valid_open_stateid(const struct nfs4_state *state)
{
	return test_bit(NFS_STATE_RECOVERY_FAILED, &state->flags) == 0;
}

#else

#define nfs4_close_state(a, b) do { } while (0)
#define nfs4_close_sync(a, b) do { } while (0)

#endif /*               */
#endif /*                          */
