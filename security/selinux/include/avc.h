/*
                                                     
  
                                                 
 */
#ifndef _SELINUX_AVC_H_
#define _SELINUX_AVC_H_

#include <linux/stddef.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/kdev_t.h>
#include <linux/spinlock.h>
#include <linux/init.h>
#include <linux/audit.h>
#include <linux/lsm_audit.h>
#include <linux/in6.h>
#include "flask.h"
#include "av_permissions.h"
#include "security.h"

#ifdef CONFIG_SECURITY_SELINUX_DEVELOP
extern int selinux_enforcing;
#else
#define selinux_enforcing 1
#endif

/*
                       
 */
struct avc_entry;

struct task_struct;
struct inode;
struct sock;
struct sk_buff;

/*
                 
 */
struct avc_cache_stats {
	unsigned int lookups;
	unsigned int misses;
	unsigned int allocations;
	unsigned int reclaims;
	unsigned int frees;
};

/*
                                                                         
 */
struct selinux_audit_data {
	u32 ssid;
	u32 tsid;
	u16 tclass;
	u32 requested;
	u32 audited;
	u32 denied;
	int result;
};

/*
                 
 */

void __init avc_init(void);

static inline u32 avc_audit_required(u32 requested,
			      struct av_decision *avd,
			      int result,
			      u32 auditdeny,
			      u32 *deniedp)
{
	u32 denied, audited;
	denied = requested & ~avd->allowed;
	if (unlikely(denied)) {
		audited = denied & avd->auditdeny;
		/*
                                           
                                                               
                                                            
                                                           
                                                             
            
    
                   
                                                              
                            
    
                                                        
                                                          
           
   */
		if (auditdeny && !(auditdeny & avd->auditdeny))
			audited = 0;
	} else if (result)
		audited = denied = requested;
	else
		audited = requested & avd->auditallow;
	*deniedp = denied;
	return audited;
}

int slow_avc_audit(u32 ssid, u32 tsid, u16 tclass,
		   u32 requested, u32 audited, u32 denied, int result,
		   struct common_audit_data *a,
		   unsigned flags);

/* 
                                                           
                                    
                                    
                                 
                                    
                                
                                            
                            
                         
  
                                                            
                                                         
                                                           
                                                            
                                                               
                                                                        
                                                              
                                    
 */
static inline int avc_audit(u32 ssid, u32 tsid,
			    u16 tclass, u32 requested,
			    struct av_decision *avd,
			    int result,
			    struct common_audit_data *a, unsigned flags)
{
	u32 audited, denied;
	audited = avc_audit_required(requested, avd, result, 0, &denied);
	if (likely(!audited))
		return 0;
	return slow_avc_audit(ssid, tsid, tclass,
			      requested, audited, denied, result,
			      a, flags);
}

#define AVC_STRICT 1 /*                         */
#define AVC_OPERATION_CMD 2	/* ignore command when updating operations */
int avc_has_perm_noaudit(u32 ssid, u32 tsid,
			 u16 tclass, u32 requested,
			 unsigned flags,
			 struct av_decision *avd);

int avc_has_operation(u32 ssid, u32 tsid, u16 tclass, u32 requested,
		u16 cmd, struct common_audit_data *ad);

int avc_has_perm_flags(u32 ssid, u32 tsid,
		       u16 tclass, u32 requested,
		       struct common_audit_data *auditdata,
		       unsigned);

static inline int avc_has_perm(u32 ssid, u32 tsid,
			       u16 tclass, u32 requested,
			       struct common_audit_data *auditdata)
{
	return avc_has_perm_flags(ssid, tsid, tclass, requested, auditdata, 0);
}

u32 avc_policy_seqno(void);

#define AVC_CALLBACK_GRANT		1
#define AVC_CALLBACK_TRY_REVOKE		2
#define AVC_CALLBACK_REVOKE		4
#define AVC_CALLBACK_RESET		8
#define AVC_CALLBACK_AUDITALLOW_ENABLE	16
#define AVC_CALLBACK_AUDITALLOW_DISABLE	32
#define AVC_CALLBACK_AUDITDENY_ENABLE	64
#define AVC_CALLBACK_AUDITDENY_DISABLE	128
#define AVC_CALLBACK_ADD_OPERATION	256

int avc_add_callback(int (*callback)(u32 event), u32 events);

/*                       */
int avc_get_hash_stats(char *page);
extern unsigned int avc_cache_threshold;

/*                                */
void avc_disable(void);

#ifdef CONFIG_SECURITY_SELINUX_AVC_STATS
DECLARE_PER_CPU(struct avc_cache_stats, avc_cache_stats);
#endif

#ifdef CONFIG_MTK_AEE_FEATURE
extern struct sk_buff *audit_get_skb(struct audit_buffer *ab);
extern void mtk_audit_hook(char *data);
#endif

#endif /*                 */

