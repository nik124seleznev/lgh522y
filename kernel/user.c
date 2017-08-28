/*
 * The "user cache".
 *
 * (C) Copyright 1991-2000 Linus Torvalds
 *
 * We have a per-user structure to keep track of how many
 * processes, files etc the user has claimed, in order to be
 * able to have per-user limits for system resources. 
 */

#include <linux/init.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/bitops.h>
#include <linux/key.h>
#include <linux/interrupt.h>
#include <linux/export.h>
#include <linux/user_namespace.h>
#include <linux/proc_ns.h>

/*
                                                      
                 
 */
struct user_namespace init_user_ns = {
	.uid_map = {
		.nr_extents = 1,
		.extent[0] = {
			.first = 0,
			.lower_first = 0,
			.count = 4294967295U,
		},
	},
	.gid_map = {
		.nr_extents = 1,
		.extent[0] = {
			.first = 0,
			.lower_first = 0,
			.count = 4294967295U,
		},
	},
	.projid_map = {
		.nr_extents = 1,
		.extent[0] = {
			.first = 0,
			.lower_first = 0,
			.count = 4294967295U,
		},
	},
	.count = ATOMIC_INIT(3),
	.owner = GLOBAL_ROOT_UID,
	.group = GLOBAL_ROOT_GID,
	.proc_inum = PROC_USER_INIT_INO,
	.flags = USERNS_INIT_FLAGS,
	.may_mount_sysfs = true,
	.may_mount_proc = true,
};
EXPORT_SYMBOL_GPL(init_user_ns);

/*
                                                               
                                                     
 */

#define UIDHASH_BITS	(CONFIG_BASE_SMALL ? 3 : 7)
#define UIDHASH_SZ	(1 << UIDHASH_BITS)
#define UIDHASH_MASK		(UIDHASH_SZ - 1)
#define __uidhashfn(uid)	(((uid >> UIDHASH_BITS) + uid) & UIDHASH_MASK)
#define uidhashentry(uid)	(uidhash_table + __uidhashfn((__kuid_val(uid))))

static struct kmem_cache *uid_cachep;
struct hlist_head uidhash_table[UIDHASH_SZ];

/*
                                                                   
                                                             
                                                                      
                                                                            
                                                                           
                                                                         
                                                
 */
static DEFINE_SPINLOCK(uidhash_lock);

/*                                            */
struct user_struct root_user = {
	.__count	= ATOMIC_INIT(1),
	.processes	= ATOMIC_INIT(1),
	.files		= ATOMIC_INIT(0),
	.sigpending	= ATOMIC_INIT(0),
	.locked_shm     = 0,
	.uid		= GLOBAL_ROOT_UID,
};

/*
                                                                
 */
static void uid_hash_insert(struct user_struct *up, struct hlist_head *hashent)
{
	hlist_add_head(&up->uidhash_node, hashent);
}

static void uid_hash_remove(struct user_struct *up)
{
	hlist_del_init(&up->uidhash_node);
}

static struct user_struct *uid_hash_find(kuid_t uid, struct hlist_head *hashent)
{
	struct user_struct *user;

	hlist_for_each_entry(user, hashent, uidhash_node) {
		if (uid_eq(user->uid, uid)) {
			atomic_inc(&user->__count);
			return user;
		}
	}

	return NULL;
}

/*                                                                
                                                                       
                      
 */
static void free_user(struct user_struct *up, unsigned long flags)
	__releases(&uidhash_lock)
{
	uid_hash_remove(up);
	spin_unlock_irqrestore(&uidhash_lock, flags);
	key_put(up->uid_keyring);
	key_put(up->session_keyring);
	kmem_cache_free(uid_cachep, up);
}

/*
                                                                               
                                             
  
                                                      
 */
struct user_struct *find_user(kuid_t uid)
{
	struct user_struct *ret;
	unsigned long flags;

	spin_lock_irqsave(&uidhash_lock, flags);
	ret = uid_hash_find(uid, uidhashentry(uid));
	spin_unlock_irqrestore(&uidhash_lock, flags);
	return ret;
}

void free_uid(struct user_struct *up)
{
	unsigned long flags;

	if (!up)
		return;

	local_irq_save(flags);
	if (atomic_dec_and_lock(&up->__count, &uidhash_lock))
		free_user(up, flags);
	else
		local_irq_restore(flags);
}

struct user_struct *alloc_uid(kuid_t uid)
{
	struct hlist_head *hashent = uidhashentry(uid);
	struct user_struct *up, *new;

	spin_lock_irq(&uidhash_lock);
	up = uid_hash_find(uid, hashent);
	spin_unlock_irq(&uidhash_lock);

	if (!up) {
		new = kmem_cache_zalloc(uid_cachep, GFP_KERNEL);
		if (!new)
			goto out_unlock;

		new->uid = uid;
		atomic_set(&new->__count, 1);

		/*
                                               
                                      
   */
		spin_lock_irq(&uidhash_lock);
		up = uid_hash_find(uid, hashent);
		if (up) {
			key_put(new->uid_keyring);
			key_put(new->session_keyring);
			kmem_cache_free(uid_cachep, new);
		} else {
			uid_hash_insert(new, hashent);
			up = new;
		}
		spin_unlock_irq(&uidhash_lock);
	}

	return up;

out_unlock:
	return NULL;
}

static int __init uid_cache_init(void)
{
	int n;

	uid_cachep = kmem_cache_create("uid_cache", sizeof(struct user_struct),
			0, SLAB_HWCACHE_ALIGN|SLAB_PANIC, NULL);

	for(n = 0; n < UIDHASH_SZ; ++n)
		INIT_HLIST_HEAD(uidhash_table + n);

	/*                                                              */
	spin_lock_irq(&uidhash_lock);
	uid_hash_insert(&root_user, uidhashentry(GLOBAL_ROOT_UID));
	spin_unlock_irq(&uidhash_lock);

	return 0;
}

module_init(uid_cache_init);
