/* audit.c -- Auditing support
 * Gateway between the kernel (e.g., selinux) and the user-space audit daemon.
 * System-call specific features have moved to auditsc.c
 *
 * Copyright 2003-2007 Red Hat Inc., Durham, North Carolina.
 * All Rights Reserved.
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
 *
 * Written by Rickard E. (Rik) Faith <faith@redhat.com>
 *
 * Goals: 1) Integrate fully with Security Modules.
 *	  2) Minimal run-time overhead:
 *	     a) Minimal when syscall auditing is disabled (audit_enable=0).
 *	     b) Small when syscall auditing is enabled and no audit record
 *		is generated (defer as much work as possible to record
 *		generation time):
 *		i) context is allocated,
 *		ii) names from getname are stored without a copy, and
 *		iii) inode information stored from path_lookup.
 *	  3) Ability to disable syscall auditing at boot time (audit=0).
 *	  4) Usable by other parts of the kernel (if audit_log* is called,
 *	     then a syscall record will be generated automatically for the
 *	     current syscall).
 *	  5) Netlink interface to user-space.
 *	  6) Support low-overhead kernel-based filtering to minimize the
 *	     information that must be passed to user-space.
 *
 * Example user-space utilities: http://people.redhat.com/sgrubb/audit/
 */

#include <linux/init.h>
#include <asm/types.h>
#include <linux/atomic.h>
#include <linux/mm.h>
#include <linux/export.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/kthread.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>

#include <linux/audit.h>

#include <net/sock.h>
#include <net/netlink.h>
#include <linux/skbuff.h>
#ifdef CONFIG_SECURITY
#include <linux/security.h>
#endif
#include <net/netlink.h>
#include <linux/freezer.h>
#include <linux/tty.h>
#include <linux/pid_namespace.h>

#include "audit.h"

/*                                                                          
                                                      */
#define AUDIT_DISABLED		-1
#define AUDIT_UNINITIALIZED	0
#define AUDIT_INITIALIZED	1
static int	audit_initialized;

#define AUDIT_OFF	0
#define AUDIT_ON	1
#define AUDIT_LOCKED	2
int		audit_enabled;
int		audit_ever_enabled;

EXPORT_SYMBOL_GPL(audit_enabled);

/*                                                         */
static int	audit_default;

/*                                                                 */
static int	audit_failure = AUDIT_FAIL_PRINTK;

/*
                                                                      
                                                                       
                                                              
 */
int		audit_pid;
static int	audit_nlk_portid;

/*                                                                         
                                                                        
                                */
static int	audit_rate_limit;

/*                                              */
static int	audit_backlog_limit = 64;
#define AUDIT_BACKLOG_WAIT_TIME (60 * HZ)
static int	audit_backlog_wait_time = AUDIT_BACKLOG_WAIT_TIME;
static int	audit_backlog_wait_overflow = 0;

/*                                                          */
kuid_t		audit_sig_uid = INVALID_UID;
pid_t		audit_sig_pid = -1;
u32		audit_sig_sid = 0;

/*                                     
                                 
                                                                       
                                                 
                                        
                                           
*/
static atomic_t    audit_lost = ATOMIC_INIT(0);

/*                     */
static struct sock *audit_sock;

/*                            */
struct list_head audit_inode_hash[AUDIT_INODE_BUCKETS];

/*                                                                     
                                                                      
                                  */
static DEFINE_SPINLOCK(audit_freelist_lock);
static int	   audit_freelist_count;
static LIST_HEAD(audit_freelist);

static struct sk_buff_head audit_skb_queue;
/*                                                       */
static struct sk_buff_head audit_skb_hold_queue;
static struct task_struct *kauditd_task;
static DECLARE_WAIT_QUEUE_HEAD(kauditd_wait);
static DECLARE_WAIT_QUEUE_HEAD(audit_backlog_wait);

/*                                    */
DEFINE_MUTEX(audit_cmd_mutex);

/*                                                                     
                                                                    
                                  */
#define AUDIT_BUFSIZ 1024

/*                                                                  
                                                                  */
#define AUDIT_MAXFREE  (2*NR_CPUS)

/*                                                                      
                                                                      
                                                                       
                                                                     
                       */
struct audit_buffer {
	struct list_head     list;
	struct sk_buff       *skb;	/*                             */
	struct audit_context *ctx;	/*                            */
	gfp_t		     gfp_mask;
};

struct audit_reply {
	int pid;
	struct sk_buff *skb;
};

static void audit_set_pid(struct audit_buffer *ab, pid_t pid)
{
	if (ab) {
		struct nlmsghdr *nlh = nlmsg_hdr(ab->skb);
		nlh->nlmsg_pid = pid;
	}
}

void audit_panic(const char *message)
{
	switch (audit_failure)
	{
	case AUDIT_FAIL_SILENT:
		break;
	case AUDIT_FAIL_PRINTK:
		if (printk_ratelimit())
			printk(KERN_ERR "audit: %s\n", message);
		break;
	case AUDIT_FAIL_PANIC:
		/*                                                          */
		if (audit_pid)
			panic("audit: %s\n", message);
		break;
	}
}

static inline int audit_rate_check(void)
{
	static unsigned long	last_check = 0;
	static int		messages   = 0;
	static DEFINE_SPINLOCK(lock);
	unsigned long		flags;
	unsigned long		now;
	unsigned long		elapsed;
	int			retval	   = 0;

	if (!audit_rate_limit) return 1;

	spin_lock_irqsave(&lock, flags);
	if (++messages < audit_rate_limit) {
		retval = 1;
	} else {
		now     = jiffies;
		elapsed = now - last_check;
		if (elapsed > HZ) {
			last_check = now;
			messages   = 0;
			retval     = 1;
		}
	}
	spin_unlock_irqrestore(&lock, flags);

	return retval;
}

/* 
                                                              
                                                              
  
                                                                  
              
                                              
*/
void audit_log_lost(const char *message)
{
	static unsigned long	last_msg = 0;
	static DEFINE_SPINLOCK(lock);
	unsigned long		flags;
	unsigned long		now;
	int			print;

	atomic_inc(&audit_lost);

	print = (audit_failure == AUDIT_FAIL_PANIC || !audit_rate_limit);

	if (!print) {
		spin_lock_irqsave(&lock, flags);
		now = jiffies;
		if (now - last_msg > HZ) {
			print = 1;
			last_msg = now;
		}
		spin_unlock_irqrestore(&lock, flags);
	}

	if (print) {
		if (printk_ratelimit())
			printk(KERN_WARNING
				"audit: audit_lost=%d audit_rate_limit=%d "
				"audit_backlog_limit=%d\n",
				atomic_read(&audit_lost),
				audit_rate_limit,
				audit_backlog_limit);
		audit_panic(message);
	}
}

static int audit_log_config_change(char *function_name, int new, int old,
				   int allow_changes)
{
	struct audit_buffer *ab;
	int rc = 0;

	ab = audit_log_start(NULL, GFP_KERNEL, AUDIT_CONFIG_CHANGE);
	if (unlikely(!ab))
		return rc;
	audit_log_format(ab, "%s=%d old=%d", function_name, new, old);
	audit_log_session_info(ab);
	rc = audit_log_task_context(ab);
	if (rc)
		allow_changes = 0; /*                               */
	audit_log_format(ab, " res=%d", allow_changes);
	audit_log_end(ab);
	return rc;
}

static int audit_do_config_change(char *function_name, int *to_change, int new)
{
	int allow_changes, rc = 0, old = *to_change;

	/*                        */
	if (audit_enabled == AUDIT_LOCKED)
		allow_changes = 0;
	else
		allow_changes = 1;

	if (audit_enabled != AUDIT_OFF) {
		rc = audit_log_config_change(function_name, new, old, allow_changes);
		if (rc)
			allow_changes = 0;
	}

	/*                                    */
	if (allow_changes == 1)
		*to_change = new;
	/*                            */
	else if (rc == 0)
		rc = -EPERM;
	return rc;
}

static int audit_set_rate_limit(int limit)
{
	return audit_do_config_change("audit_rate_limit", &audit_rate_limit, limit);
}

static int audit_set_backlog_limit(int limit)
{
	return audit_do_config_change("audit_backlog_limit", &audit_backlog_limit, limit);
}

static int audit_set_enabled(int state)
{
	int rc;
	if (state < AUDIT_OFF || state > AUDIT_LOCKED)
		return -EINVAL;

	rc =  audit_do_config_change("audit_enabled", &audit_enabled, state);
	if (!rc)
		audit_ever_enabled |= !!state;

	return rc;
}

static int audit_set_failure(int state)
{
	if (state != AUDIT_FAIL_SILENT
	    && state != AUDIT_FAIL_PRINTK
	    && state != AUDIT_FAIL_PANIC)
		return -EINVAL;

	return audit_do_config_change("audit_failure", &audit_failure, state);
}

/*
                                                                            
                                                                               
                                                                        
                                                                          
                                                                               
                                                                             
                                    
 */
static void audit_hold_skb(struct sk_buff *skb)
{
	if (audit_default &&
	    skb_queue_len(&audit_skb_hold_queue) < audit_backlog_limit)
		skb_queue_tail(&audit_skb_hold_queue, skb);
	else
		kfree_skb(skb);
}

/*
                                                                              
                                        
 */
extern void mtk_audit_hook(char *data);
static void audit_printk_skb(struct sk_buff *skb)
{
	struct nlmsghdr *nlh = nlmsg_hdr(skb);
	char *data = nlmsg_data(nlh);

	if (nlh->nlmsg_type != AUDIT_EOE) {
		if (printk_ratelimit()){
			#ifdef CONFIG_MTK_AEE_FEATURE
            		if(nlh->nlmsg_type==1400){
                   		 mtk_audit_hook(data);
	       		}
	       		 #endif
			printk(KERN_NOTICE "type=%d %s\n", nlh->nlmsg_type, data);
		}		
		else
			audit_log_lost("printk limit exceeded\n");
	}

	audit_hold_skb(skb);
}

static void kauditd_send_skb(struct sk_buff *skb)
{
	int err;
        #ifdef CONFIG_MTK_AEE_FEATURE
	struct nlmsghdr *nlh = nlmsg_hdr(skb);
	char *data = NLMSG_DATA(nlh);
	if (nlh->nlmsg_type != AUDIT_EOE) {
		if(nlh->nlmsg_type==1400){
	   		 mtk_audit_hook(data);
		}
		
	}
	#endif
	/*                                                                  */
	skb_get(skb);
	err = netlink_unicast(audit_sock, skb, audit_nlk_portid, 0);
	if (err < 0) {
		BUG_ON(err != -ECONNREFUSED); /*                  */
		printk(KERN_ERR "audit: *NO* daemon at audit_pid=%d\n", audit_pid);
		audit_log_lost("auditd disappeared\n");
		audit_pid = 0;
		/*                                                    */
		audit_hold_skb(skb);
	} else
		/*                                     */
		consume_skb(skb);
}

/*
                                                            
  
                                                              
                                                                
                                                                
                                                                
                  
  
                                                                
                                                             
                                                             
                                                               
                                                               
 */
static void flush_hold_queue(void)
{
	struct sk_buff *skb;

	if (!audit_default || !audit_pid)
		return;

	skb = skb_dequeue(&audit_skb_hold_queue);
	if (likely(!skb))
		return;

	while (skb && audit_pid) {
		kauditd_send_skb(skb);
		skb = skb_dequeue(&audit_skb_hold_queue);
	}

	/*
                                     
                                       
  */
	if (skb)
		consume_skb(skb);
}

static int kauditd_thread(void *dummy)
{
	set_freezable();
	while (!kthread_should_stop()) {
		struct sk_buff *skb;
		DECLARE_WAITQUEUE(wait, current);

		flush_hold_queue();

		skb = skb_dequeue(&audit_skb_queue);
		wake_up(&audit_backlog_wait);
		if (skb) {
			if (audit_pid)
				kauditd_send_skb(skb);
			else
				audit_printk_skb(skb);
			continue;
		}
		set_current_state(TASK_INTERRUPTIBLE);
		add_wait_queue(&kauditd_wait, &wait);

		if (!skb_queue_len(&audit_skb_queue)) {
			try_to_freeze();
			schedule();
		}

		__set_current_state(TASK_RUNNING);
		remove_wait_queue(&kauditd_wait, &wait);
	}
	return 0;
}

int audit_send_list(void *_dest)
{
	struct audit_netlink_list *dest = _dest;
	int pid = dest->pid;
	struct sk_buff *skb;

	/*                                           */
	mutex_lock(&audit_cmd_mutex);
	mutex_unlock(&audit_cmd_mutex);

	while ((skb = __skb_dequeue(&dest->q)) != NULL)
		netlink_unicast(audit_sock, skb, pid, 0);

	kfree(dest);

	return 0;
}

struct sk_buff *audit_make_reply(int pid, int seq, int type, int done,
				 int multi, const void *payload, int size)
{
	struct sk_buff	*skb;
	struct nlmsghdr	*nlh;
	void		*data;
	int		flags = multi ? NLM_F_MULTI : 0;
	int		t     = done  ? NLMSG_DONE  : type;

	skb = nlmsg_new(size, GFP_KERNEL);
	if (!skb)
		return NULL;

	nlh	= nlmsg_put(skb, pid, seq, t, size, flags);
	if (!nlh)
		goto out_kfree_skb;
	data = nlmsg_data(nlh);
	memcpy(data, payload, size);
	return skb;

out_kfree_skb:
	kfree_skb(skb);
	return NULL;
}

static int audit_send_reply_thread(void *arg)
{
	struct audit_reply *reply = (struct audit_reply *)arg;

	mutex_lock(&audit_cmd_mutex);
	mutex_unlock(&audit_cmd_mutex);

	/*                                                           
                                            */
	netlink_unicast(audit_sock, reply->skb, reply->pid, 0);
	kfree(reply);
	return 0;
}
/* 
                                                             
                                    
                        
                            
                          
                                  
                         
                      
  
                                                                         
                            
 */
static void audit_send_reply(int pid, int seq, int type, int done, int multi,
			     const void *payload, int size)
{
	struct sk_buff *skb;
	struct task_struct *tsk;
	struct audit_reply *reply = kmalloc(sizeof(struct audit_reply),
					    GFP_KERNEL);

	if (!reply)
		return;

	skb = audit_make_reply(pid, seq, type, done, multi, payload, size);
	if (!skb)
		goto out;

	reply->pid = pid;
	reply->skb = skb;

	tsk = kthread_run(audit_send_reply_thread, reply, "audit_send_reply");
	if (!IS_ERR(tsk))
		return;
	kfree_skb(skb);
out:
	kfree(reply);
}

/*
                                                                  
                    
 */
static int audit_netlink_ok(struct sk_buff *skb, u16 msg_type)
{
	int err = 0;

	/*                                              */
	if ((current_user_ns() != &init_user_ns) ||
	    (task_active_pid_ns(current) != &init_pid_ns))
		return -EPERM;

	switch (msg_type) {
	case AUDIT_LIST:
	case AUDIT_ADD:
	case AUDIT_DEL:
		return -EOPNOTSUPP;
	case AUDIT_GET:
	case AUDIT_SET:
	case AUDIT_LIST_RULES:
	case AUDIT_ADD_RULE:
	case AUDIT_DEL_RULE:
	case AUDIT_SIGNAL_INFO:
	case AUDIT_TTY_GET:
	case AUDIT_TTY_SET:
	case AUDIT_TRIM:
	case AUDIT_MAKE_EQUIV:
		if (!netlink_capable(skb, CAP_AUDIT_CONTROL))
			err = -EPERM;
		break;
	case AUDIT_USER:
	case AUDIT_FIRST_USER_MSG ... AUDIT_LAST_USER_MSG:
	case AUDIT_FIRST_USER_MSG2 ... AUDIT_LAST_USER_MSG2:
		if (!netlink_capable(skb, CAP_AUDIT_WRITE))
			err = -EPERM;
		break;
	default:  /*         */
		err = -EINVAL;
	}

	return err;
}

static int audit_log_common_recv_msg(struct audit_buffer **ab, u16 msg_type)
{
	int rc = 0;
	uid_t uid = from_kuid(&init_user_ns, current_uid());

	if (!audit_enabled && msg_type != AUDIT_USER_AVC) {
		*ab = NULL;
		return rc;
	}

	*ab = audit_log_start(NULL, GFP_KERNEL, msg_type);
	if (unlikely(!*ab))
		return rc;
	audit_log_format(*ab, "pid=%d uid=%u", task_tgid_vnr(current), uid);
	audit_log_session_info(*ab);
	audit_log_task_context(*ab);

	return rc;
}

static int audit_receive_msg(struct sk_buff *skb, struct nlmsghdr *nlh)
{
	u32			seq;
	void			*data;
	struct audit_status	*status_get, status_set;
	int			err;
	struct audit_buffer	*ab;
	u16			msg_type = nlh->nlmsg_type;
	struct audit_sig_info   *sig_data;
	char			*ctx = NULL;
	u32			len;

	err = audit_netlink_ok(skb, msg_type);
	if (err)
		return err;

	/*                                                 
                                */
	if (!kauditd_task) {
		kauditd_task = kthread_run(kauditd_thread, NULL, "kauditd");
		if (IS_ERR(kauditd_task)) {
			err = PTR_ERR(kauditd_task);
			kauditd_task = NULL;
			return err;
		}
	}
	seq  = nlh->nlmsg_seq;
	data = nlmsg_data(nlh);

	switch (msg_type) {
	case AUDIT_GET:
		status_set.mask		 = 0;
		status_set.enabled	 = audit_enabled;
		status_set.failure	 = audit_failure;
		status_set.pid		 = audit_pid;
		status_set.rate_limit	 = audit_rate_limit;
		status_set.backlog_limit = audit_backlog_limit;
		status_set.lost		 = atomic_read(&audit_lost);
		status_set.backlog	 = skb_queue_len(&audit_skb_queue);
		audit_send_reply(NETLINK_CB(skb).portid, seq, AUDIT_GET, 0, 0,
				 &status_set, sizeof(status_set));
		break;
	case AUDIT_SET:
		if (nlmsg_len(nlh) < sizeof(struct audit_status))
			return -EINVAL;
		status_get   = (struct audit_status *)data;
		if (status_get->mask & AUDIT_STATUS_ENABLED) {
			err = audit_set_enabled(status_get->enabled);
			if (err < 0)
				return err;
		}
		if (status_get->mask & AUDIT_STATUS_FAILURE) {
			err = audit_set_failure(status_get->failure);
			if (err < 0)
				return err;
		}
		if (status_get->mask & AUDIT_STATUS_PID) {
			int new_pid = status_get->pid;

			if (audit_enabled != AUDIT_OFF)
				audit_log_config_change("audit_pid", new_pid, audit_pid, 1);
			audit_pid = new_pid;
			audit_nlk_portid = NETLINK_CB(skb).portid;
		}
		if (status_get->mask & AUDIT_STATUS_RATE_LIMIT) {
			err = audit_set_rate_limit(status_get->rate_limit);
			if (err < 0)
				return err;
		}
		if (status_get->mask & AUDIT_STATUS_BACKLOG_LIMIT)
			err = audit_set_backlog_limit(status_get->backlog_limit);
		break;
	case AUDIT_USER:
	case AUDIT_FIRST_USER_MSG ... AUDIT_LAST_USER_MSG:
	case AUDIT_FIRST_USER_MSG2 ... AUDIT_LAST_USER_MSG2:
		if (!audit_enabled && msg_type != AUDIT_USER_AVC)
			return 0;

		err = audit_filter_user(msg_type);
		if (err == 1) {
			err = 0;
			if (msg_type == AUDIT_USER_TTY) {
				err = tty_audit_push_current();
				if (err)
					break;
			}
			audit_log_common_recv_msg(&ab, msg_type);
			if (msg_type != AUDIT_USER_TTY)
				audit_log_format(ab, " msg='%.1024s'",
						 (char *)data);
			else {
				int size;

				audit_log_format(ab, " data=");
				size = nlmsg_len(nlh);
				if (size > 0 &&
				    ((unsigned char *)data)[size - 1] == '\0')
					size--;
				audit_log_n_untrustedstring(ab, data, size);
			}
			audit_set_pid(ab, NETLINK_CB(skb).portid);
			audit_log_end(ab);
		}
		break;
	case AUDIT_ADD_RULE:
	case AUDIT_DEL_RULE:
		if (nlmsg_len(nlh) < sizeof(struct audit_rule_data))
			return -EINVAL;
		if (audit_enabled == AUDIT_LOCKED) {
			audit_log_common_recv_msg(&ab, AUDIT_CONFIG_CHANGE);
			audit_log_format(ab, " audit_enabled=%d res=0", audit_enabled);
			audit_log_end(ab);
			return -EPERM;
		}
		/*             */
	case AUDIT_LIST_RULES:
		err = audit_receive_filter(msg_type, NETLINK_CB(skb).portid,
					   seq, data, nlmsg_len(nlh));
		break;
	case AUDIT_TRIM:
		audit_trim_trees();
		audit_log_common_recv_msg(&ab, AUDIT_CONFIG_CHANGE);
		audit_log_format(ab, " op=trim res=1");
		audit_log_end(ab);
		break;
	case AUDIT_MAKE_EQUIV: {
		void *bufp = data;
		u32 sizes[2];
		size_t msglen = nlmsg_len(nlh);
		char *old, *new;

		err = -EINVAL;
		if (msglen < 2 * sizeof(u32))
			break;
		memcpy(sizes, bufp, 2 * sizeof(u32));
		bufp += 2 * sizeof(u32);
		msglen -= 2 * sizeof(u32);
		old = audit_unpack_string(&bufp, &msglen, sizes[0]);
		if (IS_ERR(old)) {
			err = PTR_ERR(old);
			break;
		}
		new = audit_unpack_string(&bufp, &msglen, sizes[1]);
		if (IS_ERR(new)) {
			err = PTR_ERR(new);
			kfree(old);
			break;
		}
		/*                   */
		err = audit_tag_tree(old, new);

		audit_log_common_recv_msg(&ab, AUDIT_CONFIG_CHANGE);

		audit_log_format(ab, " op=make_equiv old=");
		audit_log_untrustedstring(ab, old);
		audit_log_format(ab, " new=");
		audit_log_untrustedstring(ab, new);
		audit_log_format(ab, " res=%d", !err);
		audit_log_end(ab);
		kfree(old);
		kfree(new);
		break;
	}
	case AUDIT_SIGNAL_INFO:
		len = 0;
		if (audit_sig_sid) {
			err = security_secid_to_secctx(audit_sig_sid, &ctx, &len);
			if (err)
				return err;
		}
		sig_data = kmalloc(sizeof(*sig_data) + len, GFP_KERNEL);
		if (!sig_data) {
			if (audit_sig_sid)
				security_release_secctx(ctx, len);
			return -ENOMEM;
		}
		sig_data->uid = from_kuid(&init_user_ns, audit_sig_uid);
		sig_data->pid = audit_sig_pid;
		if (audit_sig_sid) {
			memcpy(sig_data->ctx, ctx, len);
			security_release_secctx(ctx, len);
		}
		audit_send_reply(NETLINK_CB(skb).portid, seq, AUDIT_SIGNAL_INFO,
				0, 0, sig_data, sizeof(*sig_data) + len);
		kfree(sig_data);
		break;
	case AUDIT_TTY_GET: {
		struct audit_tty_status s;
		struct task_struct *tsk = current;

		spin_lock(&tsk->sighand->siglock);
		s.enabled = tsk->signal->audit_tty != 0;
		s.log_passwd = tsk->signal->audit_tty_log_passwd;
		spin_unlock(&tsk->sighand->siglock);

		audit_send_reply(NETLINK_CB(skb).portid, seq,
				 AUDIT_TTY_GET, 0, 0, &s, sizeof(s));
		break;
	}
	case AUDIT_TTY_SET: {
		struct audit_tty_status s;
		struct task_struct *tsk = current;

		memset(&s, 0, sizeof(s));
		/*                                           */
		memcpy(&s, data, min_t(size_t, sizeof(s), nlmsg_len(nlh)));
		if ((s.enabled != 0 && s.enabled != 1) ||
		    (s.log_passwd != 0 && s.log_passwd != 1))
			return -EINVAL;

		spin_lock(&tsk->sighand->siglock);
		tsk->signal->audit_tty = s.enabled;
		tsk->signal->audit_tty_log_passwd = s.log_passwd;
		spin_unlock(&tsk->sighand->siglock);
		break;
	}
	default:
		err = -EINVAL;
		break;
	}

	return err < 0 ? err : 0;
}

/*
                                                                         
                                                           
 */
static void audit_receive_skb(struct sk_buff *skb)
{
	struct nlmsghdr *nlh;
	/*
                                                                  
                                    
  */
	int len;
	int err;

	nlh = nlmsg_hdr(skb);
	len = skb->len;

	while (nlmsg_ok(nlh, len)) {
		err = audit_receive_msg(skb, nlh);
		/*                                                    */
		if (err || (nlh->nlmsg_flags & NLM_F_ACK))
			netlink_ack(skb, nlh, err);

		nlh = nlmsg_next(nlh, &len);
	}
}

/*                                       */
static void audit_receive(struct sk_buff  *skb)
{
	mutex_lock(&audit_cmd_mutex);
	audit_receive_skb(skb);
	mutex_unlock(&audit_cmd_mutex);
}

/*                                        */
static int __init audit_init(void)
{
	int i;
	struct netlink_kernel_cfg cfg = {
		.input	= audit_receive,
	};

	if (audit_initialized == AUDIT_DISABLED)
		return 0;

	printk(KERN_INFO "audit: initializing netlink socket (%s)\n",
	       audit_default ? "enabled" : "disabled");
	audit_sock = netlink_kernel_create(&init_net, NETLINK_AUDIT, &cfg);
	if (!audit_sock)
		audit_panic("cannot initialize netlink socket");
	else
		audit_sock->sk_sndtimeo = MAX_SCHEDULE_TIMEOUT;

	skb_queue_head_init(&audit_skb_queue);
	skb_queue_head_init(&audit_skb_hold_queue);
	audit_initialized = AUDIT_INITIALIZED;
	audit_enabled = audit_default;
	audit_ever_enabled |= !!audit_default;

	audit_log(NULL, GFP_KERNEL, AUDIT_KERNEL, "initialized");

	for (i = 0; i < AUDIT_INODE_BUCKETS; i++)
		INIT_LIST_HEAD(&audit_inode_hash[i]);

	return 0;
}
__initcall(audit_init);

/*                                                                          */
static int __init audit_enable(char *str)
{
	audit_default = !!simple_strtol(str, NULL, 0);
	if (!audit_default)
		audit_initialized = AUDIT_DISABLED;

	printk(KERN_INFO "audit: %s", audit_default ? "enabled" : "disabled");

	if (audit_initialized == AUDIT_INITIALIZED) {
		audit_enabled = audit_default;
		audit_ever_enabled |= !!audit_default;
	} else if (audit_initialized == AUDIT_UNINITIALIZED) {
		printk(" (after initialization)");
	} else {
		printk(" (until reboot)");
	}
	printk("\n");

	return 1;
}

__setup("audit=", audit_enable);

static void audit_buffer_free(struct audit_buffer *ab)
{
	unsigned long flags;

	if (!ab)
		return;

	if (ab->skb)
		kfree_skb(ab->skb);

	spin_lock_irqsave(&audit_freelist_lock, flags);
	if (audit_freelist_count > AUDIT_MAXFREE)
		kfree(ab);
	else {
		audit_freelist_count++;
		list_add(&ab->list, &audit_freelist);
	}
	spin_unlock_irqrestore(&audit_freelist_lock, flags);
}

static struct audit_buffer * audit_buffer_alloc(struct audit_context *ctx,
						gfp_t gfp_mask, int type)
{
	unsigned long flags;
	struct audit_buffer *ab = NULL;
	struct nlmsghdr *nlh;

	spin_lock_irqsave(&audit_freelist_lock, flags);
	if (!list_empty(&audit_freelist)) {
		ab = list_entry(audit_freelist.next,
				struct audit_buffer, list);
		list_del(&ab->list);
		--audit_freelist_count;
	}
	spin_unlock_irqrestore(&audit_freelist_lock, flags);

	if (!ab) {
		ab = kmalloc(sizeof(*ab), gfp_mask);
		if (!ab)
			goto err;
	}

	ab->ctx = ctx;
	ab->gfp_mask = gfp_mask;

	ab->skb = nlmsg_new(AUDIT_BUFSIZ, gfp_mask);
	if (!ab->skb)
		goto err;

	nlh = nlmsg_put(ab->skb, 0, 0, type, 0, 0);
	if (!nlh)
		goto out_kfree_skb;

	return ab;

out_kfree_skb:
	kfree_skb(ab->skb);
	ab->skb = NULL;
err:
	audit_buffer_free(ab);
	return NULL;
}

/* 
                                                              
  
                                                                   
                                                                     
                                                                       
                                                                    
                                                               
                                                                       
                                 
  
                                                                      
                                                                       
                                                                   
                                                                 
          
 */
unsigned int audit_serial(void)
{
	static DEFINE_SPINLOCK(serial_lock);
	static unsigned int serial = 0;

	unsigned long flags;
	unsigned int ret;

	spin_lock_irqsave(&serial_lock, flags);
	do {
		ret = ++serial;
	} while (unlikely(!ret));
	spin_unlock_irqrestore(&serial_lock, flags);

	return ret;
}

static inline void audit_get_stamp(struct audit_context *ctx,
				   struct timespec *t, unsigned int *serial)
{
	if (!ctx || !auditsc_get_stamp(ctx, t, serial)) {
		*t = CURRENT_TIME;
		*serial = audit_serial();
	}
}

/*
                                              
 */
static void wait_for_auditd(unsigned long sleep_time)
{
	DECLARE_WAITQUEUE(wait, current);
	set_current_state(TASK_UNINTERRUPTIBLE);
	add_wait_queue(&audit_backlog_wait, &wait);

	if (audit_backlog_limit &&
	    skb_queue_len(&audit_skb_queue) > audit_backlog_limit)
		schedule_timeout(sleep_time);

	__set_current_state(TASK_RUNNING);
	remove_wait_queue(&audit_backlog_wait, &wait);
}

/*                                                                 
                                                             
                                                                  
                                                                       
                                                                        
                   */

/* 
                                           
                                    
                                
                            
  
                                                            
  
                                                                   
                                                             
                                                                         
                                                                       
                                                                         
                                     
 */
struct audit_buffer *audit_log_start(struct audit_context *ctx, gfp_t gfp_mask,
				     int type)
{
	struct audit_buffer	*ab	= NULL;
	struct timespec		t;
	unsigned int		uninitialized_var(serial);
	int reserve;
	unsigned long timeout_start = jiffies;

	if (audit_initialized != AUDIT_INITIALIZED)
		return NULL;

	if (unlikely(audit_filter_type(type)))
		return NULL;

	if (gfp_mask & __GFP_WAIT)
		reserve = 0;
	else
		reserve = 5; /*                                      
                                          */

	while (audit_backlog_limit
	       && skb_queue_len(&audit_skb_queue) > audit_backlog_limit + reserve) {
		if (gfp_mask & __GFP_WAIT && audit_backlog_wait_time) {
			unsigned long sleep_time;

			sleep_time = timeout_start + audit_backlog_wait_time -
					jiffies;
			if ((long)sleep_time > 0) {
				wait_for_auditd(sleep_time);
				continue;
			}
		}
		if (audit_rate_check() && printk_ratelimit())
			printk(KERN_WARNING
			       "audit: audit_backlog=%d > "
			       "audit_backlog_limit=%d\n",
			       skb_queue_len(&audit_skb_queue),
			       audit_backlog_limit);
		audit_log_lost("backlog limit exceeded");
		audit_backlog_wait_time = audit_backlog_wait_overflow;
		wake_up(&audit_backlog_wait);
		return NULL;
	}

	audit_backlog_wait_time = AUDIT_BACKLOG_WAIT_TIME;

	ab = audit_buffer_alloc(ctx, gfp_mask, type);
	if (!ab) {
		audit_log_lost("out of memory in audit_log_start");
		return NULL;
	}

	audit_get_stamp(ab->ctx, &t, &serial);

	audit_log_format(ab, "audit(%lu.%03lu:%u): ",
			 t.tv_sec, t.tv_nsec/1000000, serial);
	return ab;
}

/* 
                                                
                    
                                          
  
                                                                  
              
 */
static inline int audit_expand(struct audit_buffer *ab, int extra)
{
	struct sk_buff *skb = ab->skb;
	int oldtail = skb_tailroom(skb);
	int ret = pskb_expand_head(skb, 0, extra, ab->gfp_mask);
	int newtail = skb_tailroom(skb);

	if (ret < 0) {
		audit_log_lost("out of memory in audit_expand");
		return 0;
	}

	skb->truesize += newtail - oldtail;
	return newtail;
}

/*
                                                                        
                                                                     
                                                                    
                                                                   
 */
static void audit_log_vformat(struct audit_buffer *ab, const char *fmt,
			      va_list args)
{
	int len, avail;
	struct sk_buff *skb;
	va_list args2;

	if (!ab)
		return;

	BUG_ON(!ab->skb);
	skb = ab->skb;
	avail = skb_tailroom(skb);
	if (avail == 0) {
		avail = audit_expand(ab, AUDIT_BUFSIZ);
		if (!avail)
			goto out;
	}
	va_copy(args2, args);
	len = vsnprintf(skb_tail_pointer(skb), avail, fmt, args);
	if (len >= avail) {
		/*                                                   
                                                        
                                                   */
		avail = audit_expand(ab,
			max_t(unsigned, AUDIT_BUFSIZ, 1+len-avail));
		if (!avail)
			goto out_va_end;
		len = vsnprintf(skb_tail_pointer(skb), avail, fmt, args2);
	}
	if (len > 0)
		skb_put(skb, len);
out_va_end:
	va_end(args2);
out:
	return;
}

/* 
                                                             
                    
                      
                                                 
  
                                             
 */
void audit_log_format(struct audit_buffer *ab, const char *fmt, ...)
{
	va_list args;

	if (!ab)
		return;
	va_start(args, fmt);
	audit_log_vformat(ab, fmt, args);
	va_end(args);
}

/* 
                                                                         
                        
                                 
                                       
  
                                                          
  
                                                                         
                                                           
 */
void audit_log_n_hex(struct audit_buffer *ab, const unsigned char *buf,
		size_t len)
{
	int i, avail, new_len;
	unsigned char *ptr;
	struct sk_buff *skb;
	static const unsigned char *hex = "0123456789ABCDEF";

	if (!ab)
		return;

	BUG_ON(!ab->skb);
	skb = ab->skb;
	avail = skb_tailroom(skb);
	new_len = len<<1;
	if (new_len >= avail) {
		/*                                                  */
		new_len = AUDIT_BUFSIZ*(((new_len-avail)/AUDIT_BUFSIZ) + 1);
		avail = audit_expand(ab, new_len);
		if (!avail)
			return;
	}

	ptr = skb_tail_pointer(skb);
	for (i=0; i<len; i++) {
		*ptr++ = hex[(buf[i] & 0xF0)>>4]; /*              */
		*ptr++ = hex[buf[i] & 0x0F];	  /*              */
	}
	*ptr = 0;
	skb_put(skb, len << 1); /*                                    */
}

/*
                                                                         
                           
 */
void audit_log_n_string(struct audit_buffer *ab, const char *string,
			size_t slen)
{
	int avail, new_len;
	unsigned char *ptr;
	struct sk_buff *skb;

	if (!ab)
		return;

	BUG_ON(!ab->skb);
	skb = ab->skb;
	avail = skb_tailroom(skb);
	new_len = slen + 3;	/*                                    */
	if (new_len > avail) {
		avail = audit_expand(ab, new_len);
		if (!avail)
			return;
	}
	ptr = skb_tail_pointer(skb);
	*ptr++ = '"';
	memcpy(ptr, string, slen);
	ptr += slen;
	*ptr++ = '"';
	*ptr = 0;
	skb_put(skb, slen + 2);	/*                               */
}

/* 
                                                                         
                                
                                          
 */
int audit_string_contains_control(const char *string, size_t len)
{
	const unsigned char *p;
	for (p = string; p < (const unsigned char *)string + len; p++) {
		if (*p == '"' || *p < 0x21 || *p > 0x7e)
			return 1;
	}
	return 0;
}

/* 
                                                                                
                    
                                                       
                               
  
                                                                    
                                                                          
                                                                             
                                                                   
  
                                                                                
                                   
 */
void audit_log_n_untrustedstring(struct audit_buffer *ab, const char *string,
				 size_t len)
{
	if (audit_string_contains_control(string, len))
		audit_log_n_hex(ab, string, len);
	else
		audit_log_n_string(ab, string, len);
}

/* 
                                                                              
                    
                               
  
                                                                       
                           
 */
void audit_log_untrustedstring(struct audit_buffer *ab, const char *string)
{
	audit_log_n_untrustedstring(ab, string, strlen(string));
}

/*                                                       */
void audit_log_d_path(struct audit_buffer *ab, const char *prefix,
		      const struct path *path)
{
	char *p, *pathname;

	if (prefix)
		audit_log_format(ab, "%s", prefix);

	/*                                                         */
	pathname = kmalloc(PATH_MAX+11, ab->gfp_mask);
	if (!pathname) {
		audit_log_string(ab, "<no_memory>");
		return;
	}
	p = d_path(path, pathname, PATH_MAX+11);
	if (IS_ERR(p)) { /*                                            */
		/*                                           */
		audit_log_string(ab, "<too_long>");
	} else
		audit_log_untrustedstring(ab, p);
	kfree(pathname);
}

void audit_log_session_info(struct audit_buffer *ab)
{
	u32 sessionid = audit_get_sessionid(current);
	uid_t auid = from_kuid(&init_user_ns, audit_get_loginuid(current));

	audit_log_format(ab, " auid=%u ses=%u\n", auid, sessionid);
}

void audit_log_key(struct audit_buffer *ab, char *key)
{
	audit_log_format(ab, " key=");
	if (key)
		audit_log_untrustedstring(ab, key);
	else
		audit_log_format(ab, "(null)");
}

void audit_log_cap(struct audit_buffer *ab, char *prefix, kernel_cap_t *cap)
{
	int i;

	audit_log_format(ab, " %s=", prefix);
	CAP_FOR_EACH_U32(i) {
		audit_log_format(ab, "%08x",
				 cap->cap[(_KERNEL_CAPABILITY_U32S-1) - i]);
	}
}

void audit_log_fcaps(struct audit_buffer *ab, struct audit_names *name)
{
	kernel_cap_t *perm = &name->fcap.permitted;
	kernel_cap_t *inh = &name->fcap.inheritable;
	int log = 0;

	if (!cap_isclear(*perm)) {
		audit_log_cap(ab, "cap_fp", perm);
		log = 1;
	}
	if (!cap_isclear(*inh)) {
		audit_log_cap(ab, "cap_fi", inh);
		log = 1;
	}

	if (log)
		audit_log_format(ab, " cap_fe=%d cap_fver=%x",
				 name->fcap.fE, name->fcap_ver);
}

static inline int audit_copy_fcaps(struct audit_names *name,
				   const struct dentry *dentry)
{
	struct cpu_vfs_cap_data caps;
	int rc;

	if (!dentry)
		return 0;

	rc = get_vfs_caps_from_disk(dentry, &caps);
	if (rc)
		return rc;

	name->fcap.permitted = caps.permitted;
	name->fcap.inheritable = caps.inheritable;
	name->fcap.fE = !!(caps.magic_etc & VFS_CAP_FLAGS_EFFECTIVE);
	name->fcap_ver = (caps.magic_etc & VFS_CAP_REVISION_MASK) >>
				VFS_CAP_REVISION_SHIFT;

	return 0;
}

/*                                      */
void audit_copy_inode(struct audit_names *name, const struct dentry *dentry,
		      const struct inode *inode)
{
	name->ino   = inode->i_ino;
	name->dev   = inode->i_sb->s_dev;
	name->mode  = inode->i_mode;
	name->uid   = inode->i_uid;
	name->gid   = inode->i_gid;
	name->rdev  = inode->i_rdev;
	security_inode_getsecid(inode, &name->osid);
	audit_copy_fcaps(name, dentry);
}

/* 
                                                                     
                                       
                                                    
                                                              
                                                                     
                                                                           
 */
void audit_log_name(struct audit_context *context, struct audit_names *n,
		    struct path *path, int record_num, int *call_panic)
{
	struct audit_buffer *ab;
	ab = audit_log_start(context, GFP_KERNEL, AUDIT_PATH);
	if (!ab)
		return;

	audit_log_format(ab, "item=%d", record_num);

	if (path)
		audit_log_d_path(ab, " name=", path);
	else if (n->name) {
		switch (n->name_len) {
		case AUDIT_NAME_FULL:
			/*                   */
			audit_log_format(ab, " name=");
			audit_log_untrustedstring(ab, n->name->name);
			break;
		case 0:
			/*                                              
                                     */
			audit_log_d_path(ab, " name=", &context->pwd);
			break;
		default:
			/*                                    */
			audit_log_format(ab, " name=");
			audit_log_n_untrustedstring(ab, n->name->name,
						    n->name_len);
		}
	} else
		audit_log_format(ab, " name=(null)");

	if (n->ino != (unsigned long)-1) {
		audit_log_format(ab, " inode=%lu"
				 " dev=%02x:%02x mode=%#ho"
				 " ouid=%u ogid=%u rdev=%02x:%02x",
				 n->ino,
				 MAJOR(n->dev),
				 MINOR(n->dev),
				 n->mode,
				 from_kuid(&init_user_ns, n->uid),
				 from_kgid(&init_user_ns, n->gid),
				 MAJOR(n->rdev),
				 MINOR(n->rdev));
	}
	if (n->osid != 0) {
		char *ctx = NULL;
		u32 len;
		if (security_secid_to_secctx(
			n->osid, &ctx, &len)) {
			audit_log_format(ab, " osid=%u", n->osid);
			if (call_panic)
				*call_panic = 2;
		} else {
			audit_log_format(ab, " obj=%s", ctx);
			security_release_secctx(ctx, len);
		}
	}

	/*                                 */
	audit_log_format(ab, " nametype=");
	switch(n->type) {
	case AUDIT_TYPE_NORMAL:
		audit_log_format(ab, "NORMAL");
		break;
	case AUDIT_TYPE_PARENT:
		audit_log_format(ab, "PARENT");
		break;
	case AUDIT_TYPE_CHILD_DELETE:
		audit_log_format(ab, "DELETE");
		break;
	case AUDIT_TYPE_CHILD_CREATE:
		audit_log_format(ab, "CREATE");
		break;
	default:
		audit_log_format(ab, "UNKNOWN");
		break;
	}

	audit_log_fcaps(ab, n);
	audit_log_end(ab);
}

int audit_log_task_context(struct audit_buffer *ab)
{
	char *ctx = NULL;
	unsigned len;
	int error;
	u32 sid;

	security_task_getsecid(current, &sid);
	if (!sid)
		return 0;

	error = security_secid_to_secctx(sid, &ctx, &len);
	if (error) {
		if (error != -EINVAL)
			goto error_path;
		return 0;
	}

	audit_log_format(ab, " subj=%s", ctx);
	security_release_secctx(ctx, len);
	return 0;

error_path:
	audit_panic("error in audit_log_task_context");
	return error;
}
EXPORT_SYMBOL(audit_log_task_context);

void audit_log_task_info(struct audit_buffer *ab, struct task_struct *tsk)
{
	const struct cred *cred;
	char name[sizeof(tsk->comm)];
	struct mm_struct *mm = tsk->mm;
	char *tty;

	if (!ab)
		return;

	/*                */
	cred = current_cred();

	spin_lock_irq(&tsk->sighand->siglock);
	if (tsk->signal && tsk->signal->tty && tsk->signal->tty->name)
		tty = tsk->signal->tty->name;
	else
		tty = "(none)";
	spin_unlock_irq(&tsk->sighand->siglock);

	audit_log_format(ab,
			 " ppid=%ld pid=%d auid=%u uid=%u gid=%u"
			 " euid=%u suid=%u fsuid=%u"
			 " egid=%u sgid=%u fsgid=%u ses=%u tty=%s",
			 sys_getppid(),
			 tsk->pid,
			 from_kuid(&init_user_ns, audit_get_loginuid(tsk)),
			 from_kuid(&init_user_ns, cred->uid),
			 from_kgid(&init_user_ns, cred->gid),
			 from_kuid(&init_user_ns, cred->euid),
			 from_kuid(&init_user_ns, cred->suid),
			 from_kuid(&init_user_ns, cred->fsuid),
			 from_kgid(&init_user_ns, cred->egid),
			 from_kgid(&init_user_ns, cred->sgid),
			 from_kgid(&init_user_ns, cred->fsgid),
			 audit_get_sessionid(tsk), tty);

	get_task_comm(name, tsk);
	audit_log_format(ab, " comm=");
	audit_log_untrustedstring(ab, name);

	if (mm) {
		down_read(&mm->mmap_sem);
		if (mm->exe_file)
			audit_log_d_path(ab, " exe=", &mm->exe_file->f_path);
		up_read(&mm->mmap_sem);
	}
	audit_log_task_context(ab);
}
EXPORT_SYMBOL(audit_log_task_info);

/* 
                                                           
                                      
                                                 
 */
void audit_log_link_denied(const char *operation, struct path *link)
{
	struct audit_buffer *ab;
	struct audit_names *name;

	name = kzalloc(sizeof(*name), GFP_NOFS);
	if (!name)
		return;

	/*                                                            */
	ab = audit_log_start(current->audit_context, GFP_KERNEL,
			     AUDIT_ANOM_LINK);
	if (!ab)
		goto out;
	audit_log_format(ab, "op=%s", operation);
	audit_log_task_info(ab, current);
	audit_log_format(ab, " res=0");
	audit_log_end(ab);

	/*                                         */
	name->type = AUDIT_TYPE_NORMAL;
	audit_copy_inode(name, link->dentry, link->dentry->d_inode);
	audit_log_name(current->audit_context, name, link, 0, NULL);
out:
	kfree(name);
}

/* 
                                       
                        
  
                                                                     
                                                                      
                                                                        
               
 */
void audit_log_end(struct audit_buffer *ab)
{
	if (!ab)
		return;
	if (!audit_rate_check()) {
		audit_log_lost("rate limit exceeded");
	} else {
		struct nlmsghdr *nlh = nlmsg_hdr(ab->skb);
		nlh->nlmsg_len = ab->skb->len - NLMSG_HDRLEN;

		if (audit_pid) {
			skb_queue_tail(&audit_skb_queue, ab->skb);
			wake_up_interruptible(&kauditd_wait);
		} else {
			audit_printk_skb(ab->skb);
		}
		ab->skb = NULL;
	}
	audit_buffer_free(ab);
}

/* 
                                  
                      
                                
                            
                             
                                                       
  
                                                             
                                                          
                  
 */
void audit_log(struct audit_context *ctx, gfp_t gfp_mask, int type,
	       const char *fmt, ...)
{
	struct audit_buffer *ab;
	va_list args;

	ab = audit_log_start(ctx, gfp_mask, type);
	if (ab) {
		va_start(args, fmt);
		audit_log_vformat(ab, fmt, args);
		va_end(args);
		audit_log_end(ab);
	}
}

#ifdef CONFIG_SECURITY
/* 
                                                       
                    
                          
  
                                                                           
                                                                             
                                                                               
                                                                    
 */
void audit_log_secctx(struct audit_buffer *ab, u32 secid)
{
	u32 len;
	char *secctx;

	if (security_secid_to_secctx(secid, &secctx, &len)) {
		audit_panic("Cannot convert secid to context");
	} else {
		audit_log_format(ab, " obj=%s", secctx);
		security_release_secctx(secctx, len);
	}
}
EXPORT_SYMBOL(audit_log_secctx);
#endif

EXPORT_SYMBOL(audit_log_start);
EXPORT_SYMBOL(audit_log_end);
EXPORT_SYMBOL(audit_log_format);
EXPORT_SYMBOL(audit_log);
