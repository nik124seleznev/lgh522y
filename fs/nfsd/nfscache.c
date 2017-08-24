/*
 * Request reply cache. This is currently a global cache, but this may
 * change in the future and be a per-client cache.
 *
 * This code is heavily inspired by the 44BSD implementation, although
 * it does things a bit differently.
 *
 * Copyright (C) 1995, 1996 Olaf Kirch <okir@monad.swb.de>
 */

#include <linux/slab.h>
#include <linux/sunrpc/addr.h>
#include <linux/highmem.h>
#include <linux/log2.h>
#include <linux/hash.h>
#include <net/checksum.h>

#include "nfsd.h"
#include "cache.h"

#define NFSDDBG_FACILITY	NFSDDBG_REPCACHE

/*
                                                                         
                                                                          
                                                                            
 */
#define TARGET_BUCKET_SIZE	64

static struct hlist_head *	cache_hash;
static struct list_head 	lru_head;
static struct kmem_cache	*drc_slab;

/*                                            */
static unsigned int		max_drc_entries;

/*                                              */
static unsigned int		maskbits;

/*
                                                                             
                                                               
 */

/*                         */
static unsigned int		num_drc_entries;

/*                                                       */
static unsigned int		payload_misses;

/*                                                           */
static unsigned int		drc_mem_usage;

/*                         */
static unsigned int		longest_chain;

/*                                                  */
static unsigned int		longest_chain_cachesize;

static int	nfsd_cache_append(struct svc_rqst *rqstp, struct kvec *vec);
static void	cache_cleaner_func(struct work_struct *unused);
static int 	nfsd_reply_cache_shrink(struct shrinker *shrink,
					struct shrink_control *sc);

static struct shrinker nfsd_reply_cache_shrinker = {
	.shrink	= nfsd_reply_cache_shrink,
	.seeks	= 1,
};

/*
                               
                                                        
                                                                      
 */
static DEFINE_SPINLOCK(cache_lock);
static DECLARE_DELAYED_WORK(cache_cleaner, cache_cleaner_func);

/*
                                                                    
                             
  
                 
                 
                 
                 
                 
                 
                 
                 
                 
  
                                                                            
                                                                            
             
 */
static unsigned int
nfsd_cache_size_limit(void)
{
	unsigned int limit;
	unsigned long low_pages = totalram_pages - totalhigh_pages;

	limit = (16 * int_sqrt(low_pages)) << (PAGE_SHIFT-10);
	return min_t(unsigned int, limit, 256*1024);
}

/*
                                                                          
                                                                   
 */
static unsigned int
nfsd_hashsize(unsigned int limit)
{
	return roundup_pow_of_two(limit / TARGET_BUCKET_SIZE);
}

static struct svc_cacherep *
nfsd_reply_cache_alloc(void)
{
	struct svc_cacherep	*rp;

	rp = kmem_cache_alloc(drc_slab, GFP_KERNEL);
	if (rp) {
		rp->c_state = RC_UNUSED;
		rp->c_type = RC_NOCACHE;
		INIT_LIST_HEAD(&rp->c_lru);
		INIT_HLIST_NODE(&rp->c_hash);
	}
	return rp;
}

static void
nfsd_reply_cache_free_locked(struct svc_cacherep *rp)
{
	if (rp->c_type == RC_REPLBUFF && rp->c_replvec.iov_base) {
		drc_mem_usage -= rp->c_replvec.iov_len;
		kfree(rp->c_replvec.iov_base);
	}
	if (!hlist_unhashed(&rp->c_hash))
		hlist_del(&rp->c_hash);
	list_del(&rp->c_lru);
	--num_drc_entries;
	drc_mem_usage -= sizeof(*rp);
	kmem_cache_free(drc_slab, rp);
}

static void
nfsd_reply_cache_free(struct svc_cacherep *rp)
{
	spin_lock(&cache_lock);
	nfsd_reply_cache_free_locked(rp);
	spin_unlock(&cache_lock);
}

int nfsd_reply_cache_init(void)
{
	unsigned int hashsize;

	INIT_LIST_HEAD(&lru_head);
	max_drc_entries = nfsd_cache_size_limit();
	num_drc_entries = 0;
	hashsize = nfsd_hashsize(max_drc_entries);
	maskbits = ilog2(hashsize);

	register_shrinker(&nfsd_reply_cache_shrinker);
	drc_slab = kmem_cache_create("nfsd_drc", sizeof(struct svc_cacherep),
					0, 0, NULL);
	if (!drc_slab)
		goto out_nomem;

	cache_hash = kcalloc(hashsize, sizeof(struct hlist_head), GFP_KERNEL);
	if (!cache_hash)
		goto out_nomem;

	return 0;
out_nomem:
	printk(KERN_ERR "nfsd: failed to allocate reply cache\n");
	nfsd_reply_cache_shutdown();
	return -ENOMEM;
}

void nfsd_reply_cache_shutdown(void)
{
	struct svc_cacherep	*rp;

	unregister_shrinker(&nfsd_reply_cache_shrinker);
	cancel_delayed_work_sync(&cache_cleaner);

	while (!list_empty(&lru_head)) {
		rp = list_entry(lru_head.next, struct svc_cacherep, c_lru);
		nfsd_reply_cache_free_locked(rp);
	}

	kfree (cache_hash);
	cache_hash = NULL;

	if (drc_slab) {
		kmem_cache_destroy(drc_slab);
		drc_slab = NULL;
	}
}

/*
                                                                            
                         
 */
static void
lru_put_end(struct svc_cacherep *rp)
{
	rp->c_timestamp = jiffies;
	list_move_tail(&rp->c_lru, &lru_head);
	schedule_delayed_work(&cache_cleaner, RC_EXPIRE);
}

/*
                                                   
 */
static void
hash_refile(struct svc_cacherep *rp)
{
	hlist_del_init(&rp->c_hash);
	hlist_add_head(&rp->c_hash, cache_hash + hash_32(rp->c_xid, maskbits));
}

/*
                                                                         
                                                                               
 */
static void
prune_cache_entries(void)
{
	struct svc_cacherep *rp, *tmp;

	list_for_each_entry_safe(rp, tmp, &lru_head, c_lru) {
		/*
                                                        
                                                
   */
		if (rp->c_state == RC_INPROG)
			continue;
		if (num_drc_entries <= max_drc_entries &&
		    time_before(jiffies, rp->c_timestamp + RC_EXPIRE))
			break;
		nfsd_reply_cache_free_locked(rp);
	}

	/*
                                                                 
                                                                 
                                                                    
                                           
  */
	if (list_empty(&lru_head))
		cancel_delayed_work(&cache_cleaner);
	else
		mod_delayed_work(system_wq, &cache_cleaner, RC_EXPIRE);
}

static void
cache_cleaner_func(struct work_struct *unused)
{
	spin_lock(&cache_lock);
	prune_cache_entries();
	spin_unlock(&cache_lock);
}

static int
nfsd_reply_cache_shrink(struct shrinker *shrink, struct shrink_control *sc)
{
	unsigned int num;

	spin_lock(&cache_lock);
	if (sc->nr_to_scan)
		prune_cache_entries();
	num = num_drc_entries;
	spin_unlock(&cache_lock);

	return num;
}

/*
                                                                       
 */
static __wsum
nfsd_cache_csum(struct svc_rqst *rqstp)
{
	int idx;
	unsigned int base;
	__wsum csum;
	struct xdr_buf *buf = &rqstp->rq_arg;
	const unsigned char *p = buf->head[0].iov_base;
	size_t csum_len = min_t(size_t, buf->head[0].iov_len + buf->page_len,
				RC_CSUMLEN);
	size_t len = min(buf->head[0].iov_len, csum_len);

	/*                   */
	csum = csum_partial(p, len, 0);
	csum_len -= len;

	/*                          */
	idx = buf->page_base / PAGE_SIZE;
	base = buf->page_base & ~PAGE_MASK;
	while (csum_len) {
		p = page_address(buf->pages[idx]) + base;
		len = min_t(size_t, PAGE_SIZE - base, csum_len);
		csum = csum_partial(p, len, csum);
		csum_len -= len;
		base = 0;
		++idx;
	}
	return csum;
}

static bool
nfsd_cache_match(struct svc_rqst *rqstp, __wsum csum, struct svc_cacherep *rp)
{
	/*                             */
	if (rqstp->rq_xid != rp->c_xid || rqstp->rq_proc != rp->c_proc ||
	    rqstp->rq_prot != rp->c_prot || rqstp->rq_vers != rp->c_vers ||
	    rqstp->rq_arg.len != rp->c_len ||
	    !rpc_cmp_addr(svc_addr(rqstp), (struct sockaddr *)&rp->c_addr) ||
	    rpc_get_port(svc_addr(rqstp)) != rpc_get_port((struct sockaddr *)&rp->c_addr))
		return false;

	/*                              */
	if (csum != rp->c_csum) {
		++payload_misses;
		return false;
	}

	return true;
}

/*
                                                                     
                                                                  
                   
 */
static struct svc_cacherep *
nfsd_cache_search(struct svc_rqst *rqstp, __wsum csum)
{
	struct svc_cacherep	*rp, *ret = NULL;
	struct hlist_head 	*rh;
	unsigned int		entries = 0;

	rh = &cache_hash[hash_32(rqstp->rq_xid, maskbits)];
	hlist_for_each_entry(rp, rh, c_hash) {
		++entries;
		if (nfsd_cache_match(rqstp, csum, rp)) {
			ret = rp;
			break;
		}
	}

	/*                               */
	if (entries > longest_chain) {
		longest_chain = entries;
		longest_chain_cachesize = num_drc_entries;
	} else if (entries == longest_chain) {
		/*                                                     */
		longest_chain_cachesize = min(longest_chain_cachesize,
						num_drc_entries);
	}

	return ret;
}

/*
                                                                         
                                                                         
                                                                      
                                                                        
                        
 */
int
nfsd_cache_lookup(struct svc_rqst *rqstp)
{
	struct svc_cacherep	*rp, *found;
	__be32			xid = rqstp->rq_xid;
	u32			proto =  rqstp->rq_prot,
				vers = rqstp->rq_vers,
				proc = rqstp->rq_proc;
	__wsum			csum;
	unsigned long		age;
	int type = rqstp->rq_cachetype;
	int rtn = RC_DOIT;

	rqstp->rq_cacherep = NULL;
	if (type == RC_NOCACHE) {
		nfsdstats.rcnocache++;
		return rtn;
	}

	csum = nfsd_cache_csum(rqstp);

	/*
                                                                
                         
  */
	rp = nfsd_reply_cache_alloc();
	spin_lock(&cache_lock);
	if (likely(rp)) {
		++num_drc_entries;
		drc_mem_usage += sizeof(*rp);
	}

	/*                              */
	prune_cache_entries();

	found = nfsd_cache_search(rqstp, csum);
	if (found) {
		if (likely(rp))
			nfsd_reply_cache_free_locked(rp);
		rp = found;
		goto found_entry;
	}

	if (!rp) {
		dprintk("nfsd: unable to allocate DRC entry!\n");
		goto out;
	}

	nfsdstats.rcmisses++;
	rqstp->rq_cacherep = rp;
	rp->c_state = RC_INPROG;
	rp->c_xid = xid;
	rp->c_proc = proc;
	rpc_copy_addr((struct sockaddr *)&rp->c_addr, svc_addr(rqstp));
	rpc_set_port((struct sockaddr *)&rp->c_addr, rpc_get_port(svc_addr(rqstp)));
	rp->c_prot = proto;
	rp->c_vers = vers;
	rp->c_len = rqstp->rq_arg.len;
	rp->c_csum = csum;

	hash_refile(rp);
	lru_put_end(rp);

	/*                    */
	if (rp->c_type == RC_REPLBUFF) {
		drc_mem_usage -= rp->c_replvec.iov_len;
		kfree(rp->c_replvec.iov_base);
		rp->c_replvec.iov_base = NULL;
	}
	rp->c_type = RC_NOCACHE;
 out:
	spin_unlock(&cache_lock);
	return rtn;

found_entry:
	nfsdstats.rchits++;
	/*                                                                */
	age = jiffies - rp->c_timestamp;
	lru_put_end(rp);

	rtn = RC_DROPIT;
	/*                                              */
	if (rp->c_state == RC_INPROG || age < RC_DELAY)
		goto out;

	/*                                              
                                                    */
	rtn = RC_DOIT;
	if (!rqstp->rq_secure && rp->c_secure)
		goto out;

	/*                          */
	switch (rp->c_type) {
	case RC_NOCACHE:
		break;
	case RC_REPLSTAT:
		svc_putu32(&rqstp->rq_res.head[0], rp->c_replstat);
		rtn = RC_REPLY;
		break;
	case RC_REPLBUFF:
		if (!nfsd_cache_append(rqstp, &rp->c_replvec))
			goto out;	/*                   */
		rtn = RC_REPLY;
		break;
	default:
		printk(KERN_WARNING "nfsd: bad repcache type %d\n", rp->c_type);
		nfsd_reply_cache_free_locked(rp);
	}

	goto out;
}

/*
                                                               
                                                               
                 
  
                                                                      
                                                                       
                                                                         
  
                                                                      
                                                                   
  
                                                                      
                                                                       
                                                                
 */
void
nfsd_cache_update(struct svc_rqst *rqstp, int cachetype, __be32 *statp)
{
	struct svc_cacherep *rp = rqstp->rq_cacherep;
	struct kvec	*resv = &rqstp->rq_res.head[0], *cachv;
	int		len;
	size_t		bufsize = 0;

	if (!rp)
		return;

	len = resv->iov_len - ((char*)statp - (char*)resv->iov_base);
	len >>= 2;

	/*                                                        */
	if (!statp || len > (256 >> 2)) {
		nfsd_reply_cache_free(rp);
		return;
	}

	switch (cachetype) {
	case RC_REPLSTAT:
		if (len != 1)
			printk("nfsd: RC_REPLSTAT/reply len %d!\n",len);
		rp->c_replstat = *statp;
		break;
	case RC_REPLBUFF:
		cachv = &rp->c_replvec;
		bufsize = len << 2;
		cachv->iov_base = kmalloc(bufsize, GFP_KERNEL);
		if (!cachv->iov_base) {
			nfsd_reply_cache_free(rp);
			return;
		}
		cachv->iov_len = bufsize;
		memcpy(cachv->iov_base, statp, bufsize);
		break;
	case RC_NOCACHE:
		nfsd_reply_cache_free(rp);
		return;
	}
	spin_lock(&cache_lock);
	drc_mem_usage += bufsize;
	lru_put_end(rp);
	rp->c_secure = rqstp->rq_secure;
	rp->c_type = cachetype;
	rp->c_state = RC_DONE;
	spin_unlock(&cache_lock);
	return;
}

/*
                                                                
                                                                   
                      
 */
static int
nfsd_cache_append(struct svc_rqst *rqstp, struct kvec *data)
{
	struct kvec	*vec = &rqstp->rq_res.head[0];

	if (vec->iov_len + data->iov_len > PAGE_SIZE) {
		printk(KERN_WARNING "nfsd: cached reply too large (%Zd).\n",
				data->iov_len);
		return 0;
	}
	memcpy((char*)vec->iov_base + vec->iov_len, data->iov_base, data->iov_len);
	vec->iov_len += data->iov_len;
	return 1;
}

/*
                                                                              
                                                                       
                             
 */
static int nfsd_reply_cache_stats_show(struct seq_file *m, void *v)
{
	spin_lock(&cache_lock);
	seq_printf(m, "max entries:           %u\n", max_drc_entries);
	seq_printf(m, "num entries:           %u\n", num_drc_entries);
	seq_printf(m, "hash buckets:          %u\n", 1 << maskbits);
	seq_printf(m, "mem usage:             %u\n", drc_mem_usage);
	seq_printf(m, "cache hits:            %u\n", nfsdstats.rchits);
	seq_printf(m, "cache misses:          %u\n", nfsdstats.rcmisses);
	seq_printf(m, "not cached:            %u\n", nfsdstats.rcnocache);
	seq_printf(m, "payload misses:        %u\n", payload_misses);
	seq_printf(m, "longest chain len:     %u\n", longest_chain);
	seq_printf(m, "cachesize at longest:  %u\n", longest_chain_cachesize);
	spin_unlock(&cache_lock);
	return 0;
}

int nfsd_reply_cache_stats_open(struct inode *inode, struct file *file)
{
	return single_open(file, nfsd_reply_cache_stats_show, NULL);
}