/* audit -- definition of audit_context structure and supporting types 
 *
 * Copyright 2003-2004 Red Hat, Inc.
 * Copyright 2005 Hewlett-Packard Development Company, L.P.
 * Copyright 2005 IBM Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <linux/fs.h>
#include <linux/audit.h>
#include <linux/skbuff.h>
#include <uapi/linux/mqueue.h>

/*                
                         
                                 
*/
#define AUDIT_DEBUG 0

/*                                                                   
                                                                          
                                                                             */
#define AUDIT_NAMES	5

/*                                                                      
                                                                        
                       */
enum audit_state {
	AUDIT_DISABLED,		/*                                      
                                            
                     */
	AUDIT_BUILD_CONTEXT,	/*                                   
                                
                                     
                                       
                                          
                           */
	AUDIT_RECORD_CONTEXT	/*                                   
                                         
                                           
                                     */
};

/*            */
struct audit_watch;
struct audit_tree;
struct audit_chunk;

struct audit_entry {
	struct list_head	list;
	struct rcu_head		rcu;
	struct audit_krule	rule;
};

struct audit_cap_data {
	kernel_cap_t		permitted;
	kernel_cap_t		inheritable;
	union {
		unsigned int	fE;		/*                           */
		kernel_cap_t	effective;	/*                          */
	};
};

/*                                                                      
                                                                   
                                  
  
                                                                      
 */
struct audit_names {
	struct list_head	list;		/*                           */

	struct filename		*name;
	int			name_len;	/*                        */
	bool			hidden;		/*                       */
	bool			name_put;	/*                   */

	unsigned long		ino;
	dev_t			dev;
	umode_t			mode;
	kuid_t			uid;
	kgid_t			gid;
	dev_t			rdev;
	u32			osid;
	struct audit_cap_data	fcap;
	unsigned int		fcap_ver;
	unsigned char		type;		/*             */
	/*
                                                               
                                                              
                                    
  */
	bool			should_free;
};

/*                             */
struct audit_context {
	int		    dummy;	/*                           */
	int		    in_syscall;	/*                           */
	enum audit_state    state, current_state;
	unsigned int	    serial;     /*                          */
	int		    major;      /*                */
	struct timespec	    ctime;      /*                       */
	unsigned long	    argv[4];    /*                   */
	long		    return_code;/*                     */
	u64		    prio;
	int		    return_valid; /*                      */
	/*
                                                                       
                                                                  
                                                                 
                                                                      
                                                                      
                              
  */
	struct audit_names  preallocated_names[AUDIT_NAMES];
	int		    name_count; /*                             */
	struct list_head    names_list;	/*                                 */
	char		    *filterkey;	/*                                    */
	struct path	    pwd;
	struct audit_aux_data *aux;
	struct audit_aux_data *aux_pids;
	struct sockaddr_storage *sockaddr;
	size_t sockaddr_len;
				/*                                        */
	pid_t		    pid, ppid;
	kuid_t		    uid, euid, suid, fsuid;
	kgid_t		    gid, egid, sgid, fsgid;
	unsigned long	    personality;
	int		    arch;

	pid_t		    target_pid;
	kuid_t		    target_auid;
	kuid_t		    target_uid;
	unsigned int	    target_sessionid;
	u32		    target_sid;
	char		    target_comm[TASK_COMM_LEN];

	struct audit_tree_refs *trees, *first_trees;
	struct list_head killed_trees;
	int tree_count;

	int type;
	union {
		struct {
			int nargs;
			long args[6];
		} socketcall;
		struct {
			kuid_t			uid;
			kgid_t			gid;
			umode_t			mode;
			u32			osid;
			int			has_perm;
			uid_t			perm_uid;
			gid_t			perm_gid;
			umode_t			perm_mode;
			unsigned long		qbytes;
		} ipc;
		struct {
			mqd_t			mqdes;
			struct mq_attr		mqstat;
		} mq_getsetattr;
		struct {
			mqd_t			mqdes;
			int			sigev_signo;
		} mq_notify;
		struct {
			mqd_t			mqdes;
			size_t			msg_len;
			unsigned int		msg_prio;
			struct timespec		abs_timeout;
		} mq_sendrecv;
		struct {
			int			oflag;
			umode_t			mode;
			struct mq_attr		attr;
		} mq_open;
		struct {
			pid_t			pid;
			struct audit_cap_data	cap;
		} capset;
		struct {
			int			fd;
			int			flags;
		} mmap;
	};
	int fds[2];

#if AUDIT_DEBUG
	int		    put_count;
	int		    ino_count;
#endif
};

extern int audit_ever_enabled;

extern void audit_copy_inode(struct audit_names *name,
			     const struct dentry *dentry,
			     const struct inode *inode);
extern void audit_log_cap(struct audit_buffer *ab, char *prefix,
			  kernel_cap_t *cap);
extern void audit_log_fcaps(struct audit_buffer *ab, struct audit_names *name);
extern void audit_log_name(struct audit_context *context,
			   struct audit_names *n, struct path *path,
			   int record_num, int *call_panic);

extern int audit_pid;

#define AUDIT_INODE_BUCKETS	32
extern struct list_head audit_inode_hash[AUDIT_INODE_BUCKETS];

static inline int audit_hash_ino(u32 ino)
{
	return (ino & (AUDIT_INODE_BUCKETS-1));
}

/*                                                    */
#define AUDIT_NAME_FULL -1

extern int audit_match_class(int class, unsigned syscall);
extern int audit_comparator(const u32 left, const u32 op, const u32 right);
extern int audit_uid_comparator(kuid_t left, u32 op, kuid_t right);
extern int audit_gid_comparator(kgid_t left, u32 op, kgid_t right);
extern int parent_len(const char *path);
extern int audit_compare_dname_path(const char *dname, const char *path, int plen);
extern struct sk_buff *	    audit_make_reply(int pid, int seq, int type,
					     int done, int multi,
					     const void *payload, int size);
extern void		    audit_panic(const char *message);

struct audit_netlink_list {
	int pid;
	struct sk_buff_head q;
};

int audit_send_list(void *);

extern int selinux_audit_rule_update(void);

extern struct mutex audit_filter_mutex;
extern void audit_free_rule_rcu(struct rcu_head *);
extern struct list_head audit_filter_list[];

extern struct audit_entry *audit_dupe_rule(struct audit_krule *old);

/*                       */
#ifdef CONFIG_AUDIT_WATCH
extern void audit_put_watch(struct audit_watch *watch);
extern void audit_get_watch(struct audit_watch *watch);
extern int audit_to_watch(struct audit_krule *krule, char *path, int len, u32 op);
extern int audit_add_watch(struct audit_krule *krule, struct list_head **list);
extern void audit_remove_watch_rule(struct audit_krule *krule);
extern char *audit_watch_path(struct audit_watch *watch);
extern int audit_watch_compare(struct audit_watch *watch, unsigned long ino, dev_t dev);
#else
#define audit_put_watch(w) {}
#define audit_get_watch(w) {}
#define audit_to_watch(k, p, l, o) (-EINVAL)
#define audit_add_watch(k, l) (-EINVAL)
#define audit_remove_watch_rule(k) BUG()
#define audit_watch_path(w) ""
#define audit_watch_compare(w, i, d) 0

#endif /*                    */

#ifdef CONFIG_AUDIT_TREE
extern struct audit_chunk *audit_tree_lookup(const struct inode *);
extern void audit_put_chunk(struct audit_chunk *);
extern int audit_tree_match(struct audit_chunk *, struct audit_tree *);
extern int audit_make_tree(struct audit_krule *, char *, u32);
extern int audit_add_tree_rule(struct audit_krule *);
extern int audit_remove_tree_rule(struct audit_krule *);
extern void audit_trim_trees(void);
extern int audit_tag_tree(char *old, char *new);
extern const char *audit_tree_path(struct audit_tree *);
extern void audit_put_tree(struct audit_tree *);
extern void audit_kill_trees(struct list_head *);
#else
#define audit_remove_tree_rule(rule) BUG()
#define audit_add_tree_rule(rule) -EINVAL
#define audit_make_tree(rule, str, op) -EINVAL
#define audit_trim_trees() (void)0
#define audit_put_tree(tree) (void)0
#define audit_tag_tree(old, new) -EINVAL
#define audit_tree_path(rule) ""	/*              */
#define audit_kill_trees(list) BUG()
#endif

extern char *audit_unpack_string(void **, size_t *, size_t);

extern pid_t audit_sig_pid;
extern kuid_t audit_sig_uid;
extern u32 audit_sig_sid;

#ifdef CONFIG_AUDITSYSCALL
extern int __audit_signal_info(int sig, struct task_struct *t);
static inline int audit_signal_info(int sig, struct task_struct *t)
{
	if (unlikely((audit_pid && t->tgid == audit_pid) ||
		     (audit_signals && !audit_dummy_context())))
		return __audit_signal_info(sig, t);
	return 0;
}
extern void audit_filter_inodes(struct task_struct *, struct audit_context *);
extern struct list_head *audit_killed_trees(void);
#else
#define audit_signal_info(s,t) AUDIT_DISABLED
#define audit_filter_inodes(t,c) AUDIT_DISABLED
#endif

extern struct mutex audit_cmd_mutex;
