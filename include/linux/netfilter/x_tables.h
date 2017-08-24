#ifndef _X_TABLES_H
#define _X_TABLES_H


#include <linux/netdevice.h>
#include <uapi/linux/netfilter/x_tables.h>

/* 
                                                          
  
                              
                                
                             
                               
                        
                         
                                                          
                                                             
                                            
                                                                  
                                                   
  
                                   
  
                                                      
                                                     
 */
struct xt_action_param {
	union {
		const struct xt_match *match;
		const struct xt_target *target;
	};
	union {
		const void *matchinfo, *targinfo;
	};
	const struct net_device *in, *out;
	int fragoff;
	unsigned int thoff;
	unsigned int hooknum;
	u_int8_t family;
	bool hotdrop;
};

/* 
                                                           
                       
  
                                                              
                                                      
                                            
                                                           
                                                                  
                             
                                                        
                         
 */
struct xt_mtchk_param {
	struct net *net;
	const char *table;
	const void *entryinfo;
	const struct xt_match *match;
	void *matchinfo;
	unsigned int hook_mask;
	u_int8_t family;
};

/* 
                                                      
                   
 */
struct xt_mtdtor_param {
	struct net *net;
	const struct xt_match *match;
	void *matchinfo;
	u_int8_t family;
};

/* 
                                                            
                       
  
                                            
                                                          
  
                          
 */
struct xt_tgchk_param {
	struct net *net;
	const char *table;
	const void *entryinfo;
	const struct xt_target *target;
	void *targinfo;
	unsigned int hook_mask;
	u_int8_t family;
};

/*                              */
struct xt_tgdtor_param {
	struct net *net;
	const struct xt_target *target;
	void *targinfo;
	u_int8_t family;
};

struct xt_match {
	struct list_head list;

	const char name[XT_EXTENSION_MAXNAMELEN];
	u_int8_t revision;

	/*                                                           
                                        */
	/*                                                       
                                                
                          */
	bool (*match)(const struct sk_buff *skb,
		      struct xt_action_param *);

	/*                                                         */
	int (*checkentry)(const struct xt_mtchk_param *);

	/*                                         */
	void (*destroy)(const struct xt_mtdtor_param *);
#ifdef CONFIG_COMPAT
	/*                                                           */
	void (*compat_from_user)(void *dst, const void *src);
	int (*compat_to_user)(void __user *dst, const void *src);
#endif
	/*                                                             */
	struct module *me;

	const char *table;
	unsigned int matchsize;
#ifdef CONFIG_COMPAT
	unsigned int compatsize;
#endif
	unsigned int hooks;
	unsigned short proto;

	unsigned short family;
};

/*                                 */
struct xt_target {
	struct list_head list;

	const char name[XT_EXTENSION_MAXNAMELEN];
	u_int8_t revision;

	/*                                                             
                                                            
                          */
	unsigned int (*target)(struct sk_buff *skb,
			       const struct xt_action_param *);

	/*                                                        
                                                               
                   */
	/*                                                                 */
	int (*checkentry)(const struct xt_tgchk_param *);

	/*                                         */
	void (*destroy)(const struct xt_tgdtor_param *);
#ifdef CONFIG_COMPAT
	/*                                                           */
	void (*compat_from_user)(void *dst, const void *src);
	int (*compat_to_user)(void __user *dst, const void *src);
#endif
	/*                                                             */
	struct module *me;

	const char *table;
	unsigned int targetsize;
#ifdef CONFIG_COMPAT
	unsigned int compatsize;
#endif
	unsigned int hooks;
	unsigned short proto;

	unsigned short family;
};

/*                       */
struct xt_table {
	struct list_head list;

	/*                              */
	unsigned int valid_hooks;

	/*                           */
	struct xt_table_info *private;

	/*                                                             */
	struct module *me;

	u_int8_t af;		/*                         */
	int priority;		/*            */

	/*                  */
	const char name[XT_TABLE_MAXNAMELEN];
};

#include <linux/netfilter_ipv4.h>

/*                  */
struct xt_table_info {
	/*                */
	unsigned int size;
	/*                                */
	unsigned int number;
	/*                                                          */
	unsigned int initial_entries;

	/*                             */
	unsigned int hook_entry[NF_INET_NUMHOOKS];
	unsigned int underflow[NF_INET_NUMHOOKS];

	/*
                                                                  
                                                                  
  */
	unsigned int stacksize;
	unsigned int __percpu *stackptr;
	void ***jumpstack;
	/*                               */
	/*                                                              */
	void *entries[1];
};

#define XT_TABLE_INFO_SZ (offsetof(struct xt_table_info, entries) \
			  + nr_cpu_ids * sizeof(char *))
extern int xt_register_target(struct xt_target *target);
extern void xt_unregister_target(struct xt_target *target);
extern int xt_register_targets(struct xt_target *target, unsigned int n);
extern void xt_unregister_targets(struct xt_target *target, unsigned int n);

extern int xt_register_match(struct xt_match *target);
extern void xt_unregister_match(struct xt_match *target);
extern int xt_register_matches(struct xt_match *match, unsigned int n);
extern void xt_unregister_matches(struct xt_match *match, unsigned int n);

extern int xt_check_match(struct xt_mtchk_param *,
			  unsigned int size, u_int8_t proto, bool inv_proto);
extern int xt_check_target(struct xt_tgchk_param *,
			   unsigned int size, u_int8_t proto, bool inv_proto);

extern struct xt_table *xt_register_table(struct net *net,
					  const struct xt_table *table,
					  struct xt_table_info *bootstrap,
					  struct xt_table_info *newinfo);
extern void *xt_unregister_table(struct xt_table *table);

extern struct xt_table_info *xt_replace_table(struct xt_table *table,
					      unsigned int num_counters,
					      struct xt_table_info *newinfo,
					      int *error);

extern struct xt_match *xt_find_match(u8 af, const char *name, u8 revision);
extern struct xt_target *xt_find_target(u8 af, const char *name, u8 revision);
extern struct xt_match *xt_request_find_match(u8 af, const char *name,
					      u8 revision);
extern struct xt_target *xt_request_find_target(u8 af, const char *name,
						u8 revision);
extern int xt_find_revision(u8 af, const char *name, u8 revision,
			    int target, int *err);

extern struct xt_table *xt_find_table_lock(struct net *net, u_int8_t af,
					   const char *name);
extern void xt_table_unlock(struct xt_table *t);

extern int xt_proto_init(struct net *net, u_int8_t af);
extern void xt_proto_fini(struct net *net, u_int8_t af);

extern struct xt_table_info *xt_alloc_table_info(unsigned int size);
extern void xt_free_table_info(struct xt_table_info *info);

/* 
                                                   
   
                                                                       
                                                                      
                                                  
                                                
 */
DECLARE_PER_CPU(seqcount_t, xt_recseq);

/* 
                                                   
  
                                                          
                                             
                                                                     
            
                                 
                           
 */
static inline unsigned int xt_write_recseq_begin(void)
{
	unsigned int addend;

	/*
                                                  
                                   
  */
	addend = (__this_cpu_read(xt_recseq.sequence) + 1) & 1;

	/*
                                                                  
                                                                    
                                 
  */
	__this_cpu_add(xt_recseq.sequence, addend);
	smp_wmb();

	return addend;
}

/* 
                                               
                                                              
  
                                                  
                                             
                                                                     
 */
static inline void xt_write_recseq_end(unsigned int addend)
{
	/*                                                              */
	smp_wmb();
	__this_cpu_add(xt_recseq.sequence, addend);
}

/*
                                                          
 */
static inline unsigned long ifname_compare_aligned(const char *_a,
						   const char *_b,
						   const char *_mask)
{
	const unsigned long *a = (const unsigned long *)_a;
	const unsigned long *b = (const unsigned long *)_b;
	const unsigned long *mask = (const unsigned long *)_mask;
	unsigned long ret;

	ret = (a[0] ^ b[0]) & mask[0];
	if (IFNAMSIZ > sizeof(unsigned long))
		ret |= (a[1] ^ b[1]) & mask[1];
	if (IFNAMSIZ > 2 * sizeof(unsigned long))
		ret |= (a[2] ^ b[2]) & mask[2];
	if (IFNAMSIZ > 3 * sizeof(unsigned long))
		ret |= (a[3] ^ b[3]) & mask[3];
	BUILD_BUG_ON(IFNAMSIZ > 4 * sizeof(unsigned long));
	return ret;
}

extern struct nf_hook_ops *xt_hook_link(const struct xt_table *, nf_hookfn *);
extern void xt_hook_unlink(const struct xt_table *, struct nf_hook_ops *);

#ifdef CONFIG_COMPAT
#include <net/compat.h>

struct compat_xt_entry_match {
	union {
		struct {
			u_int16_t match_size;
			char name[XT_FUNCTION_MAXNAMELEN - 1];
			u_int8_t revision;
		} user;
		struct {
			u_int16_t match_size;
			compat_uptr_t match;
		} kernel;
		u_int16_t match_size;
	} u;
	unsigned char data[0];
};

struct compat_xt_entry_target {
	union {
		struct {
			u_int16_t target_size;
			char name[XT_FUNCTION_MAXNAMELEN - 1];
			u_int8_t revision;
		} user;
		struct {
			u_int16_t target_size;
			compat_uptr_t target;
		} kernel;
		u_int16_t target_size;
	} u;
	unsigned char data[0];
};

/*                                       
                                                                           
                          */

struct compat_xt_counters {
	compat_u64 pcnt, bcnt;			/*                          */
};

struct compat_xt_counters_info {
	char name[XT_TABLE_MAXNAMELEN];
	compat_uint_t num_counters;
	struct compat_xt_counters counters[0];
};

struct _compat_xt_align {
	__u8 u8;
	__u16 u16;
	__u32 u32;
	compat_u64 u64;
};

#define COMPAT_XT_ALIGN(s) __ALIGN_KERNEL((s), __alignof__(struct _compat_xt_align))

extern void xt_compat_lock(u_int8_t af);
extern void xt_compat_unlock(u_int8_t af);

extern int xt_compat_add_offset(u_int8_t af, unsigned int offset, int delta);
extern void xt_compat_flush_offsets(u_int8_t af);
extern void xt_compat_init_offsets(u_int8_t af, unsigned int number);
extern int xt_compat_calc_jump(u_int8_t af, unsigned int offset);

extern int xt_compat_match_offset(const struct xt_match *match);
extern int xt_compat_match_from_user(struct xt_entry_match *m,
				     void **dstptr, unsigned int *size);
extern int xt_compat_match_to_user(const struct xt_entry_match *m,
				   void __user **dstptr, unsigned int *size);

extern int xt_compat_target_offset(const struct xt_target *target);
extern void xt_compat_target_from_user(struct xt_entry_target *t,
				       void **dstptr, unsigned int *size);
extern int xt_compat_target_to_user(const struct xt_entry_target *t,
				    void __user **dstptr, unsigned int *size);

#endif /*               */
#endif /*             */
