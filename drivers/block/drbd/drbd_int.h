/*
  drbd_int.h

  This file is part of DRBD by Philipp Reisner and Lars Ellenberg.

  Copyright (C) 2001-2008, LINBIT Information Technologies GmbH.
  Copyright (C) 1999-2008, Philipp Reisner <philipp.reisner@linbit.com>.
  Copyright (C) 2002-2008, Lars Ellenberg <lars.ellenberg@linbit.com>.

  drbd is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2, or (at your option)
  any later version.

  drbd is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with drbd; see the file COPYING.  If not, write to
  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#ifndef _DRBD_INT_H
#define _DRBD_INT_H

#include <linux/compiler.h>
#include <linux/types.h>
#include <linux/list.h>
#include <linux/sched.h>
#include <linux/bitops.h>
#include <linux/slab.h>
#include <linux/crypto.h>
#include <linux/ratelimit.h>
#include <linux/tcp.h>
#include <linux/mutex.h>
#include <linux/major.h>
#include <linux/blkdev.h>
#include <linux/genhd.h>
#include <linux/idr.h>
#include <net/tcp.h>
#include <linux/lru_cache.h>
#include <linux/prefetch.h>
#include <linux/drbd_genl_api.h>
#include <linux/drbd.h>
#include "drbd_state.h"

#ifdef __CHECKER__
# define __protected_by(x)       __attribute__((require_context(x,1,999,"rdwr")))
# define __protected_read_by(x)  __attribute__((require_context(x,1,999,"read")))
# define __protected_write_by(x) __attribute__((require_context(x,1,999,"write")))
# define __must_hold(x)       __attribute__((context(x,1,1), require_context(x,1,999,"call")))
#else
# define __protected_by(x)
# define __protected_read_by(x)
# define __protected_write_by(x)
# define __must_hold(x)
#endif

#define __no_warn(lock, stmt) do { __acquire(lock); stmt; __release(lock); } while (0)

/*                                          */
extern unsigned int minor_count;
extern bool disable_sendpage;
extern bool allow_oos;

#ifdef CONFIG_DRBD_FAULT_INJECTION
extern int enable_faults;
extern int fault_rate;
extern int fault_devs;
#endif

extern char usermode_helper[];


/*                              
                                    
                                            
                 
 */
#define DRBD_SIG SIGXCPU

/*                                          
                                              
                                           
                           
 */
#define DRBD_SIGKILL SIGHUP

#define ID_IN_SYNC      (4711ULL)
#define ID_OUT_OF_SYNC  (4712ULL)
#define ID_SYNCER (-1ULL)

#define UUID_NEW_BM_OFFSET ((u64)0x0001000000000000ULL)

struct drbd_conf;
struct drbd_tconn;


/*                                                           */
#define DEV (disk_to_dev(mdev->vdisk))

#define conn_printk(LEVEL, TCONN, FMT, ARGS...) \
	printk(LEVEL "d-con %s: " FMT, TCONN->name , ## ARGS)
#define conn_alert(TCONN, FMT, ARGS...)  conn_printk(KERN_ALERT, TCONN, FMT, ## ARGS)
#define conn_crit(TCONN, FMT, ARGS...)   conn_printk(KERN_CRIT, TCONN, FMT, ## ARGS)
#define conn_err(TCONN, FMT, ARGS...)    conn_printk(KERN_ERR, TCONN, FMT, ## ARGS)
#define conn_warn(TCONN, FMT, ARGS...)   conn_printk(KERN_WARNING, TCONN, FMT, ## ARGS)
#define conn_notice(TCONN, FMT, ARGS...) conn_printk(KERN_NOTICE, TCONN, FMT, ## ARGS)
#define conn_info(TCONN, FMT, ARGS...)   conn_printk(KERN_INFO, TCONN, FMT, ## ARGS)
#define conn_dbg(TCONN, FMT, ARGS...)    conn_printk(KERN_DEBUG, TCONN, FMT, ## ARGS)

#define D_ASSERT(exp)	if (!(exp)) \
	 dev_err(DEV, "ASSERT( " #exp " ) in %s:%d\n", __FILE__, __LINE__)

/* 
                               
  
                                                                
 */
#define expect(exp) ({								\
		bool _bool = (exp);						\
		if (!_bool)							\
			dev_err(DEV, "ASSERTION %s FAILED in %s\n",		\
			        #exp, __func__);				\
		_bool;								\
		})

/*                                    */
enum {
	DRBD_FAULT_MD_WR = 0,	/*                 */
	DRBD_FAULT_MD_RD = 1,	/*                 */
	DRBD_FAULT_RS_WR = 2,	/*                 */
	DRBD_FAULT_RS_RD = 3,
	DRBD_FAULT_DT_WR = 4,	/*                 */
	DRBD_FAULT_DT_RD = 5,
	DRBD_FAULT_DT_RA = 6,	/*                 */
	DRBD_FAULT_BM_ALLOC = 7,	/*                   */
	DRBD_FAULT_AL_EE = 8,	/*          */
	DRBD_FAULT_RECEIVE = 9, /*                                                    */

	DRBD_FAULT_MAX,
};

extern unsigned int
_drbd_insert_fault(struct drbd_conf *mdev, unsigned int type);

static inline int
drbd_insert_fault(struct drbd_conf *mdev, unsigned int type) {
#ifdef CONFIG_DRBD_FAULT_INJECTION
	return fault_rate &&
		(enable_faults & (1<<type)) &&
		_drbd_insert_fault(mdev, type);
#else
	return 0;
#endif
}

/*                                                  */
#define div_ceil(A, B) ((A)/(B) + ((A)%(B) ? 1 : 0))
/*                        */
#define div_floor(A, B) ((A)/(B))

extern struct ratelimit_state drbd_ratelimit_state;
extern struct idr minors; /*                           */
extern struct list_head drbd_tconns; /*                           */

/*             */
enum drbd_packet {
	/*                        */
	P_DATA		      = 0x00,
	P_DATA_REPLY	      = 0x01, /*                            */
	P_RS_DATA_REPLY	      = 0x02, /*                               */
	P_BARRIER	      = 0x03,
	P_BITMAP	      = 0x04,
	P_BECOME_SYNC_TARGET  = 0x05,
	P_BECOME_SYNC_SOURCE  = 0x06,
	P_UNPLUG_REMOTE	      = 0x07, /*                                        */
	P_DATA_REQUEST	      = 0x08, /*                              */
	P_RS_DATA_REQUEST     = 0x09, /*                                         */
	P_SYNC_PARAM	      = 0x0a,
	P_PROTOCOL	      = 0x0b,
	P_UUIDS		      = 0x0c,
	P_SIZES		      = 0x0d,
	P_STATE		      = 0x0e,
	P_SYNC_UUID	      = 0x0f,
	P_AUTH_CHALLENGE      = 0x10,
	P_AUTH_RESPONSE	      = 0x11,
	P_STATE_CHG_REQ	      = 0x12,

	/*                      */
	P_PING		      = 0x13,
	P_PING_ACK	      = 0x14,
	P_RECV_ACK	      = 0x15, /*                    */
	P_WRITE_ACK	      = 0x16, /*                    */
	P_RS_WRITE_ACK	      = 0x17, /*                                                    */
	P_SUPERSEDED	      = 0x18, /*                                                   */
	P_NEG_ACK	      = 0x19, /*                                */
	P_NEG_DREPLY	      = 0x1a, /*                         */
	P_NEG_RS_DREPLY	      = 0x1b, /*                         */
	P_BARRIER_ACK	      = 0x1c,
	P_STATE_CHG_REPLY     = 0x1d,

	/*                                                                  */

	P_OV_REQUEST	      = 0x1e, /*             */
	P_OV_REPLY	      = 0x1f,
	P_OV_RESULT	      = 0x20, /*             */
	P_CSUM_RS_REQUEST     = 0x21, /*             */
	P_RS_IS_IN_SYNC	      = 0x22, /*             */
	P_SYNC_PARAM89	      = 0x23, /*                                                               */
	P_COMPRESSED_BITMAP   = 0x24, /*                                                 */
	/*                                                                   */
	/*                                                                   */
	P_DELAY_PROBE         = 0x27, /*                         */
	P_OUT_OF_SYNC         = 0x28, /*                                               */
	P_RS_CANCEL           = 0x29, /*                                                           */
	P_CONN_ST_CHG_REQ     = 0x2a, /*                                          */
	P_CONN_ST_CHG_REPLY   = 0x2b, /*                                            */
	P_RETRY_WRITE	      = 0x2c, /*                                             */
	P_PROTOCOL_UPDATE     = 0x2d, /*                                               */

	P_MAY_IGNORE	      = 0x100, /*                                          */
	P_MAX_OPT_CMD	      = 0x101,

	/*                                   */

	P_INITIAL_META	      = 0xfff1, /*                              */
	P_INITIAL_DATA	      = 0xfff2, /*                            */

	P_CONNECTION_FEATURES = 0xfffe	/*                             */
};

extern const char *cmdname(enum drbd_packet cmd);

/*                                  
                                    */
struct bm_xfer_ctx {
	/*        
                                    
                                      
                                                */
	unsigned long bm_bits;
	unsigned long bm_words;
	/*                                                 */
	unsigned long bit_offset;
	unsigned long word_offset;

	/*                                             */
	unsigned packets[2];
	unsigned bytes[2];
};

extern void INFO_bm_xfer_stats(struct drbd_conf *mdev,
		const char *direction, struct bm_xfer_ctx *c);

static inline void bm_xfer_ctx_bit_to_word_offset(struct bm_xfer_ctx *c)
{
	/*                                                       
                      
                                                      
                                                          
                                                         
                                                               
                                                 */
#if BITS_PER_LONG == 64
	c->word_offset = c->bit_offset >> 6;
#elif BITS_PER_LONG == 32
	c->word_offset = c->bit_offset >> 5;
	c->word_offset &= ~(1UL);
#else
# error "unsupported BITS_PER_LONG"
#endif
}

#ifndef __packed
#define __packed __attribute__((packed))
#endif

/*                                             
                                           
                                           
                                      
                                         
                                       
  
                                                         
                                   
 */
struct p_header80 {
	u32	  magic;
	u16	  command;
	u16	  length;	/*                                 */
} __packed;

/*                                                              */
struct p_header95 {
	u16	  magic;	/*                         */
	u16	  command;
	u32	  length;
} __packed;

struct p_header100 {
	u32	  magic;
	u16	  volume;
	u16	  command;
	u32	  length;
	u32	  pad;
} __packed;

extern unsigned int drbd_header_size(struct drbd_tconn *tconn);

/*                                                                         */
#define DP_HARDBARRIER	      1 /*            */
#define DP_RW_SYNC	      2 /*                    */
#define DP_MAY_SET_IN_SYNC    4
#define DP_UNPLUG             8 /*                    */
#define DP_FUA               16 /*                    */
#define DP_FLUSH             32 /*                    */
#define DP_DISCARD           64 /*                    */
#define DP_SEND_RECEIVE_ACK 128 /*                                 */
#define DP_SEND_WRITE_ACK   256 /*                                 */

struct p_data {
	u64	    sector;    /*                       */
	u64	    block_id;  /*                                         */
	u32	    seq_num;
	u32	    dp_flags;
} __packed;

/*
                                 
                
                                                 
                                                             
                
                                      
 */
struct p_block_ack {
	u64	    sector;
	u64	    block_id;
	u32	    blksize;
	u32	    seq_num;
} __packed;

struct p_block_req {
	u64 sector;
	u64 block_id;
	u32 blksize;
	u32 pad;	/*                       */
} __packed;

/*
                                                        
                          
              
                  
                 
                 
 */

struct p_connection_features {
	u32 protocol_min;
	u32 feature_flags;
	u32 protocol_max;

	/*                                                   
                                                                
  */

	u32 _pad;
	u64 reserved[7];
} __packed;

struct p_barrier {
	u32 barrier;	/*                              */
	u32 pad;	/*                       */
} __packed;

struct p_barrier_ack {
	u32 barrier;
	u32 set_size;
} __packed;

struct p_rs_param {
	u32 resync_rate;

	      /*                                       */
	char verify_alg[0];
} __packed;

struct p_rs_param_89 {
	u32 resync_rate;
        /*                      */
	char verify_alg[SHARED_SECRET_MAX];
	char csums_alg[SHARED_SECRET_MAX];
} __packed;

struct p_rs_param_95 {
	u32 resync_rate;
	char verify_alg[SHARED_SECRET_MAX];
	char csums_alg[SHARED_SECRET_MAX];
	u32 c_plan_ahead;
	u32 c_delay_target;
	u32 c_fill_target;
	u32 c_max_rate;
} __packed;

enum drbd_conn_flags {
	CF_DISCARD_MY_DATA = 1,
	CF_DRY_RUN = 2,
};

struct p_protocol {
	u32 protocol;
	u32 after_sb_0p;
	u32 after_sb_1p;
	u32 after_sb_2p;
	u32 conn_flags;
	u32 two_primaries;

              /*                                       */
	char integrity_alg[0];

} __packed;

struct p_uuids {
	u64 uuid[UI_EXTENDED_SIZE];
} __packed;

struct p_rs_uuid {
	u64	    uuid;
} __packed;

struct p_sizes {
	u64	    d_size;  /*              */
	u64	    u_size;  /*                     */
	u64	    c_size;  /*                       */
	u32	    max_bio_size;  /*                       */
	u16	    queue_order_type;  /*                            */
	u16	    dds_flags; /*                          */
} __packed;

struct p_state {
	u32	    state;
} __packed;

struct p_req_state {
	u32	    mask;
	u32	    val;
} __packed;

struct p_req_state_reply {
	u32	    retcode;
} __packed;

struct p_drbd06_param {
	u64	  size;
	u32	  state;
	u32	  blksize;
	u32	  protocol;
	u32	  version;
	u32	  gen_cnt[5];
	u32	  bit_map_gen[5];
} __packed;

struct p_block_desc {
	u64 sector;
	u32 blksize;
	u32 pad;	/*                       */
} __packed;

/*                                     
                                          */
enum drbd_bitmap_code {
	/*                   
                                                  
                          */
	RLE_VLI_Bits = 2,
};

struct p_compressed_bm {
	/*                                                              
                                                              
                                                                    
                                       
  */
	u8 encoding;

	u8 code[0];
} __packed;

struct p_delay_probe93 {
	u32     seq_num; /*                                                */
	u32     offset;  /*                                                         */
} __packed;

/*
                                                                              
                                                                          
 */
#define DRBD_SOCKET_BUFFER_SIZE 4096

/*                                                                    */
enum drbd_thread_state {
	NONE,
	RUNNING,
	EXITING,
	RESTARTING
};

struct drbd_thread {
	spinlock_t t_lock;
	struct task_struct *task;
	struct completion stop;
	enum drbd_thread_state t_state;
	int (*function) (struct drbd_thread *);
	struct drbd_tconn *tconn;
	int reset_cpu_mask;
	char name[9];
};

static inline enum drbd_thread_state get_t_state(struct drbd_thread *thi)
{
	/*                                                              
                                                                    
          */

	smp_rmb();
	return thi->t_state;
}

struct drbd_work {
	struct list_head list;
	int (*cb)(struct drbd_work *, int cancel);
	union {
		struct drbd_conf *mdev;
		struct drbd_tconn *tconn;
	};
};

#include "drbd_interval.h"

extern int drbd_wait_misc(struct drbd_conf *, struct drbd_interval *);

struct drbd_request {
	struct drbd_work w;

	/*                                          
                                                                    
                                                      
                              */
	struct bio *private_bio;

	struct drbd_interval i;

	/*                                                             
                                                         
                                                                
   
                                                            
                                                         
                                                       
  */
	unsigned int epoch;

	struct list_head tl_requests; /*                               */
	struct bio *master_bio;       /*                    */
	unsigned long start_time;

	/*                                                */
	atomic_t completion_ref;
	/*                                                         */
	struct kref kref;

	unsigned rq_state; /*                               */
};

struct drbd_epoch {
	struct drbd_tconn *tconn;
	struct list_head list;
	unsigned int barrier_nr;
	atomic_t epoch_size; /*                                   */
	atomic_t active;     /*                                                           */
	unsigned long flags;
};

/*                      */
enum {
	DE_HAVE_BARRIER_NUMBER,
};

enum epoch_event {
	EV_PUT,
	EV_GOT_BARRIER_NR,
	EV_BECAME_LAST,
	EV_CLEANUP = 32, /*              */
};

struct drbd_wq_barrier {
	struct drbd_work w;
	struct completion done;
};

struct digest_info {
	int digest_size;
	void *digest;
};

struct drbd_peer_request {
	struct drbd_work w;
	struct drbd_epoch *epoch; /*            */
	struct page *pages;
	atomic_t pending_bios;
	struct drbd_interval i;
	/*                                    */
	unsigned long flags;
	union {
		u64 block_id;
		struct digest_info *digest;
	};
};

/*              
                                                                        
                                             
                                                            
                                              
 */
enum {
	__EE_CALL_AL_COMPLETE_IO,
	__EE_MAY_SET_IN_SYNC,

	/*                          
                                                  */
	__EE_RESUBMITTED,

	/*                                           
                                                     
                            */
	__EE_WAS_ERROR,

	/*                                                         */
	__EE_HAS_DIGEST,

	/*                                                                    */
	__EE_RESTART_REQUESTS,

	/*                                                    */
	__EE_SEND_WRITE_ACK,

	/*                                                                         */
	__EE_IN_INTERVAL_TREE,
};
#define EE_CALL_AL_COMPLETE_IO (1<<__EE_CALL_AL_COMPLETE_IO)
#define EE_MAY_SET_IN_SYNC     (1<<__EE_MAY_SET_IN_SYNC)
#define	EE_RESUBMITTED         (1<<__EE_RESUBMITTED)
#define EE_WAS_ERROR           (1<<__EE_WAS_ERROR)
#define EE_HAS_DIGEST          (1<<__EE_HAS_DIGEST)
#define EE_RESTART_REQUESTS	(1<<__EE_RESTART_REQUESTS)
#define EE_SEND_WRITE_ACK	(1<<__EE_SEND_WRITE_ACK)
#define EE_IN_INTERVAL_TREE	(1<<__EE_IN_INTERVAL_TREE)

/*                    */
enum {
	UNPLUG_REMOTE,		/*                                     */
	MD_DIRTY,		/*                                         */
	USE_DEGR_WFC_T,		/*                                          */
	CL_ST_CHG_SUCCESS,
	CL_ST_CHG_FAIL,
	CRASHED_PRIMARY,	/*                                 
                                       
                                    */
	CONSIDER_RESYNC,

	MD_NO_FUA,		/*                                                      */
	SUSPEND_IO,		/*                        */
	BITMAP_IO,		/*                        
                                                  */
	BITMAP_IO_QUEUED,       /*                   */
	GO_DISKLESS,		/*                                                       */
	WAS_IO_ERROR,		/*                                      */
	WAS_READ_ERROR,		/*                                                        */
	FORCE_DETACH,		/*                                                             */
	RESYNC_AFTER_NEG,       /*                                                               */
	RESIZE_PENDING,		/*                                                            
                                              */
	NEW_CUR_UUID,		/*                                         */
	AL_SUSPENDED,		/*                                          */
	AHEAD_TO_SYNC_SOURCE,   /*                            */
	B_RS_H_DONE,		/*                                               */
	DISCARD_MY_DATA,	/*                                 */
	READ_BALANCE_RR,
};

struct drbd_bitmap; /*                      */

/*                                                          
                                   */
enum bm_flag {
	/*                                         */
	BM_P_VMALLOCED = 0x10000, /*                                       */

	/*                                     */
	BM_LOCKED_MASK = 0xf,

	/*                     */
	BM_DONT_CLEAR = 0x1,
	BM_DONT_SET   = 0x2,
	BM_DONT_TEST  = 0x4,

	/*                                             
                                            */
	BM_IS_LOCKED  = 0x8,

	/*                                             */
	BM_LOCKED_TEST_ALLOWED = BM_DONT_CLEAR | BM_DONT_SET | BM_IS_LOCKED,

	/*                                                                     
                                                                       
                                                           */
	BM_LOCKED_SET_ALLOWED = BM_DONT_CLEAR | BM_IS_LOCKED,

	/*                                                     
                                                    */
	BM_LOCKED_CHANGE_ALLOWED = BM_IS_LOCKED,
};

struct drbd_work_queue {
	struct list_head q;
	spinlock_t q_lock;  /*                      */
	wait_queue_head_t q_wait;
};

struct drbd_socket {
	struct mutex mutex;
	struct socket    *socket;
	/*                    
                                       */
	void *sbuf;
	void *rbuf;
};

struct drbd_md {
	u64 md_offset;		/*                                */

	u64 la_size_sect;	/*                                */
	spinlock_t uuid_lock;
	u64 uuid[UI_SIZE];
	u64 device_uuid;
	u32 flags;
	u32 md_size_sect;

	s32 al_offset;	/*                                               */
	s32 bm_offset;	/*                                         */

	/*                                                           */
	s32 meta_dev_idx;

	/*                                      */
	u32 al_stripes;
	u32 al_stripe_size_4k;
	u32 al_size_4k; /*                             */
};

struct drbd_backing_dev {
	struct block_device *backing_bdev;
	struct block_device *md_bdev;
	struct drbd_md md;
	struct disk_conf *disk_conf; /*                                            */
	sector_t known_size; /*                                        */
};

struct drbd_md_io {
	unsigned int done;
	int error;
};

struct bm_io_work {
	struct drbd_work w;
	char *why;
	enum bm_flag flags;
	int (*io_fn)(struct drbd_conf *mdev);
	void (*done)(struct drbd_conf *mdev, int rv);
};

enum write_ordering_e {
	WO_none,
	WO_drain_io,
	WO_bdev_flush,
};

struct fifo_buffer {
	unsigned int head_index;
	unsigned int size;
	int total; /*                   */
	int values[0];
};
extern struct fifo_buffer *fifo_alloc(int fifo_size);

/*                     */
enum {
	NET_CONGESTED,		/*                              */
	RESOLVE_CONFLICTS,	/*                                       */
	SEND_PING,		/*                                         */
	SIGNAL_ASENDER,		/*                                         */
	GOT_PING_ACK,		/*                                                             */
	CONN_WD_ST_CHG_REQ,	/*                                                         */
	CONN_WD_ST_CHG_OKAY,
	CONN_WD_ST_CHG_FAIL,
	CONN_DRY_RUN,		/*                                           */
	CREATE_BARRIER,		/*                                        */
	STATE_SENT,		/*                                             */
	CALLBACK_PENDING,	/*                                                       
                                         
                                                  
                                                    
                                                     
     */
	DISCONNECT_SENT,
};

struct drbd_tconn {			/*                                    */
	char *name;			/*               */
	struct list_head all_tconn;	/*                              */
	struct kref kref;
	struct idr volumes;		/*                              */
	enum drbd_conns cstate;		/*                                         */
	unsigned susp:1;		/*                      */
	unsigned susp_nod:1;		/*                              */
	unsigned susp_fen:1;		/*                                              */
	struct mutex cstate_mutex;	/*                               */

	unsigned long flags;
	struct net_conf *net_conf;	/*                          */
	struct mutex conf_update;	/*                                                       */
	wait_queue_head_t ping_wait;	/*                                                    */
	struct res_opts res_opts;

	struct sockaddr_storage my_addr;
	int my_addr_len;
	struct sockaddr_storage peer_addr;
	int peer_addr_len;

	struct drbd_socket data;	/*                                       */
	struct drbd_socket meta;	/*                             */
	int agreed_pro_version;		/*                                */
	unsigned long last_received;	/*                           */
	unsigned int ko_count;

	spinlock_t req_lock;

	struct list_head transfer_log;	/*                                      */

	struct crypto_hash *cram_hmac_tfm;
	struct crypto_hash *integrity_tfm;  /*                                                               */
	struct crypto_hash *peer_integrity_tfm;  /*                                                          */
	struct crypto_hash *csums_tfm;
	struct crypto_hash *verify_tfm;
	void *int_dig_in;
	void *int_dig_vv;

	/*               */
	struct drbd_epoch *current_epoch;
	spinlock_t epoch_lock;
	unsigned int epochs;
	enum write_ordering_e write_ordering;
	atomic_t current_tle_nr;	/*                           */
	unsigned current_tle_writes;	/*                                  */

	unsigned long last_reconnect_jif;
	struct drbd_thread receiver;
	struct drbd_thread worker;
	struct drbd_thread asender;
	cpumask_var_t cpu_mask;

	/*             */
	struct drbd_work_queue sender_work;

	struct {
		/*                           
                                       */
		bool seen_any_write_yet;

		/*                                                      */
		int current_epoch_nr;

		/*                                       
                                         
                                         */
		unsigned current_epoch_writes;
	} send;
};

struct submit_worker {
	struct workqueue_struct *wq;
	struct work_struct worker;

	spinlock_t lock;
	struct list_head writes;
};

struct drbd_conf {
	struct drbd_tconn *tconn;
	int vnr;			/*                                     */
	struct kref kref;

	/*                                                         */
	unsigned long flags;

	/*                         */
	struct drbd_backing_dev *ldev __protected_by(local);

	sector_t p_size;     /*                     */
	struct request_queue *rq_queue;
	struct block_device *this_bdev;
	struct gendisk	    *vdisk;

	unsigned long last_reattach_jif;
	struct drbd_work  resync_work,
			  unplug_work,
			  go_diskless,
			  md_sync_work,
			  start_resync_work;
	struct timer_list resync_timer;
	struct timer_list md_sync_timer;
	struct timer_list start_resync_timer;
	struct timer_list request_timer;
#ifdef DRBD_DEBUG_MD_SYNC
	struct {
		unsigned int line;
		const char* func;
	} last_md_mark_dirty;
#endif

	/*                                                     */
	union drbd_state new_state_tmp;

	union drbd_dev_state state;
	wait_queue_head_t misc_wait;
	wait_queue_head_t state_wait;  /*                         */
	unsigned int send_cnt;
	unsigned int recv_cnt;
	unsigned int read_cnt;
	unsigned int writ_cnt;
	unsigned int al_writ_cnt;
	unsigned int bm_writ_cnt;
	atomic_t ap_bio_cnt;	 /*                              */
	atomic_t ap_pending_cnt; /*                                           */
	atomic_t rs_pending_cnt; /*                                     */
	atomic_t unacked_cnt;	 /*                          */
	atomic_t local_cnt;	 /*                              */

	/*                                         */
	struct rb_root read_requests;
	struct rb_root write_requests;

	/*                                                   */
	unsigned long rs_total;
	/*                                                 */
	unsigned long rs_failed;
	/*                                    */
	unsigned long rs_start;
	/*                                                    */
	unsigned long rs_paused;
	/*                                                     */
	unsigned long rs_same_csum;
#define DRBD_SYNC_MARKS 8
#define DRBD_SYNC_MARK_STEP (3*HZ)
	/*                                                   */
	unsigned long rs_mark_left[DRBD_SYNC_MARKS];
	/*                             */
	unsigned long rs_mark_time[DRBD_SYNC_MARKS];
	/*                                        */
	int rs_last_mark;
	unsigned long rs_last_bcast; /*                */

	/*                                                 */
	sector_t ov_start_sector;
	sector_t ov_stop_sector;
	/*                            */
	sector_t ov_position;
	/*                                                                */
	sector_t ov_last_oos_start;
	/*                                       */
	sector_t ov_last_oos_size;
	unsigned long ov_left; /*         */

	struct drbd_bitmap *bitmap;
	unsigned long bm_resync_fo; /*                                  */

	/*                                       */
	struct lru_cache *resync;
	/*                                         */
	unsigned int resync_locked;
	/*                                                       */
	unsigned int resync_wenr;

	int open_cnt;
	u64 *p_uuid;

	struct list_head active_ee; /*                                              */
	struct list_head sync_ee;   /*                                                       */
	struct list_head done_ee;   /*                          */
	struct list_head read_ee;   /*                               */
	struct list_head net_ee;    /*                                    */

	int next_barrier_nr;
	struct list_head resync_reads;
	atomic_t pp_in_use;		/*                          */
	atomic_t pp_in_use_by_net;	/*                                      */
	wait_queue_head_t ee_wait;
	struct page *md_io_page;	/*                           */
	struct drbd_md_io md_io;
	atomic_t md_io_in_use;		/*                                               */
	spinlock_t al_lock;
	wait_queue_head_t al_wait;
	struct lru_cache *act_log;	/*              */
	unsigned int al_tr_number;
	int al_tr_cycle;
	wait_queue_head_t seq_wait;
	atomic_t packet_seq;
	unsigned int peer_seq;
	spinlock_t peer_seq_lock;
	unsigned int minor;
	unsigned long comm_bm_set; /*                                  */
	struct bm_io_work bm_io_work;
	u64 ed_uuid; /*                          */
	struct mutex own_state_mutex;
	struct mutex *state_mutex; /*                                                     */
	char congestion_reason;  /*                           */
	atomic_t rs_sect_in; /*                                           */
	atomic_t rs_sect_ev; /*                                      */
	int rs_last_sect_ev; /*                         */
	int rs_last_events;  /*                                                 
                                                          */
	int c_sync_rate; /*                                                 */
	struct fifo_buffer *rs_plan_s; /*                                                              */
	int rs_in_flight; /*                                                              */
	atomic_t ap_in_flight; /*                                         */
	unsigned int peer_max_bio_size;
	unsigned int local_max_bio_size;

	/*                                                     
                                                    */
	struct submit_worker submit;
};

static inline struct drbd_conf *minor_to_mdev(unsigned int minor)
{
	return (struct drbd_conf *)idr_find(&minors, minor);
}

static inline unsigned int mdev_to_minor(struct drbd_conf *mdev)
{
	return mdev->minor;
}

static inline struct drbd_conf *vnr_to_mdev(struct drbd_tconn *tconn, int vnr)
{
	return (struct drbd_conf *)idr_find(&tconn->volumes, vnr);
}

/*
                        
                         */

/*             */

enum dds_flags {
	DDSF_FORCED    = 1,
	DDSF_NO_RESYNC = 2, /*                                       */
};

extern void drbd_init_set_defaults(struct drbd_conf *mdev);
extern int  drbd_thread_start(struct drbd_thread *thi);
extern void _drbd_thread_stop(struct drbd_thread *thi, int restart, int wait);
extern char *drbd_task_to_thread_name(struct drbd_tconn *tconn, struct task_struct *task);
#ifdef CONFIG_SMP
extern void drbd_thread_current_set_cpu(struct drbd_thread *thi);
extern void drbd_calc_cpu_mask(struct drbd_tconn *tconn);
#else
#define drbd_thread_current_set_cpu(A) ({})
#define drbd_calc_cpu_mask(A) ({})
#endif
extern void tl_release(struct drbd_tconn *, unsigned int barrier_nr,
		       unsigned int set_size);
extern void tl_clear(struct drbd_tconn *);
extern void drbd_free_sock(struct drbd_tconn *tconn);
extern int drbd_send(struct drbd_tconn *tconn, struct socket *sock,
		     void *buf, size_t size, unsigned msg_flags);
extern int drbd_send_all(struct drbd_tconn *, struct socket *, void *, size_t,
			 unsigned);

extern int __drbd_send_protocol(struct drbd_tconn *tconn, enum drbd_packet cmd);
extern int drbd_send_protocol(struct drbd_tconn *tconn);
extern int drbd_send_uuids(struct drbd_conf *mdev);
extern int drbd_send_uuids_skip_initial_sync(struct drbd_conf *mdev);
extern void drbd_gen_and_send_sync_uuid(struct drbd_conf *mdev);
extern int drbd_send_sizes(struct drbd_conf *mdev, int trigger_reply, enum dds_flags flags);
extern int drbd_send_state(struct drbd_conf *mdev, union drbd_state s);
extern int drbd_send_current_state(struct drbd_conf *mdev);
extern int drbd_send_sync_param(struct drbd_conf *mdev);
extern void drbd_send_b_ack(struct drbd_tconn *tconn, u32 barrier_nr,
			    u32 set_size);
extern int drbd_send_ack(struct drbd_conf *, enum drbd_packet,
			 struct drbd_peer_request *);
extern void drbd_send_ack_rp(struct drbd_conf *mdev, enum drbd_packet cmd,
			     struct p_block_req *rp);
extern void drbd_send_ack_dp(struct drbd_conf *mdev, enum drbd_packet cmd,
			     struct p_data *dp, int data_size);
extern int drbd_send_ack_ex(struct drbd_conf *mdev, enum drbd_packet cmd,
			    sector_t sector, int blksize, u64 block_id);
extern int drbd_send_out_of_sync(struct drbd_conf *, struct drbd_request *);
extern int drbd_send_block(struct drbd_conf *, enum drbd_packet,
			   struct drbd_peer_request *);
extern int drbd_send_dblock(struct drbd_conf *mdev, struct drbd_request *req);
extern int drbd_send_drequest(struct drbd_conf *mdev, int cmd,
			      sector_t sector, int size, u64 block_id);
extern int drbd_send_drequest_csum(struct drbd_conf *mdev, sector_t sector,
				   int size, void *digest, int digest_size,
				   enum drbd_packet cmd);
extern int drbd_send_ov_request(struct drbd_conf *mdev,sector_t sector,int size);

extern int drbd_send_bitmap(struct drbd_conf *mdev);
extern void drbd_send_sr_reply(struct drbd_conf *mdev, enum drbd_state_rv retcode);
extern void conn_send_sr_reply(struct drbd_tconn *tconn, enum drbd_state_rv retcode);
extern void drbd_free_bc(struct drbd_backing_dev *ldev);
extern void drbd_mdev_cleanup(struct drbd_conf *mdev);
void drbd_print_uuids(struct drbd_conf *mdev, const char *text);

extern void conn_md_sync(struct drbd_tconn *tconn);
extern void drbd_md_sync(struct drbd_conf *mdev);
extern int  drbd_md_read(struct drbd_conf *mdev, struct drbd_backing_dev *bdev);
extern void drbd_uuid_set(struct drbd_conf *mdev, int idx, u64 val) __must_hold(local);
extern void _drbd_uuid_set(struct drbd_conf *mdev, int idx, u64 val) __must_hold(local);
extern void drbd_uuid_new_current(struct drbd_conf *mdev) __must_hold(local);
extern void drbd_uuid_set_bm(struct drbd_conf *mdev, u64 val) __must_hold(local);
extern void drbd_uuid_move_history(struct drbd_conf *mdev) __must_hold(local);
extern void __drbd_uuid_set(struct drbd_conf *mdev, int idx, u64 val) __must_hold(local);
extern void drbd_md_set_flag(struct drbd_conf *mdev, int flags) __must_hold(local);
extern void drbd_md_clear_flag(struct drbd_conf *mdev, int flags)__must_hold(local);
extern int drbd_md_test_flag(struct drbd_backing_dev *, int);
#ifndef DRBD_DEBUG_MD_SYNC
extern void drbd_md_mark_dirty(struct drbd_conf *mdev);
#else
#define drbd_md_mark_dirty(m)	drbd_md_mark_dirty_(m, __LINE__ , __func__ )
extern void drbd_md_mark_dirty_(struct drbd_conf *mdev,
		unsigned int line, const char *func);
#endif
extern void drbd_queue_bitmap_io(struct drbd_conf *mdev,
				 int (*io_fn)(struct drbd_conf *),
				 void (*done)(struct drbd_conf *, int),
				 char *why, enum bm_flag flags);
extern int drbd_bitmap_io(struct drbd_conf *mdev,
		int (*io_fn)(struct drbd_conf *),
		char *why, enum bm_flag flags);
extern int drbd_bitmap_io_from_worker(struct drbd_conf *mdev,
		int (*io_fn)(struct drbd_conf *),
		char *why, enum bm_flag flags);
extern int drbd_bmio_set_n_write(struct drbd_conf *mdev);
extern int drbd_bmio_clear_n_write(struct drbd_conf *mdev);
extern void drbd_ldev_destroy(struct drbd_conf *mdev);

/*                 
  
                                          
                                 
            
                                                  
                                                  
                       
                                       
                                                 
  
             
                                         
  
            
                                                           
                                                                     
                                           
                                           
                                                  
  
                                                                 
                                                                      
  
                                                       
                                                                            
                                                
                                        
 */

/*                                    
                                                 
                                                    */
#define MD_128MB_SECT (128LLU << 11)  /*                      */
#define MD_4kB_SECT	 8
#define MD_32kB_SECT	64

/*                                                  */
#define AL_EXTENT_SHIFT 22
#define AL_EXTENT_SIZE (1<<AL_EXTENT_SHIFT)

/*                                                               
                                                                     
                                                                 
                   
  
                           
                                                                            
                              
                           
                                                                       
                                                                              
                                                                          
                                                    
   */
#define AL_UPDATES_PER_TRANSACTION	 64	//          
#define AL_CONTEXT_PER_TRANSACTION	919	//                     

#if BITS_PER_LONG == 32
#define LN2_BPL 5
#define cpu_to_lel(A) cpu_to_le32(A)
#define lel_to_cpu(A) le32_to_cpu(A)
#elif BITS_PER_LONG == 64
#define LN2_BPL 6
#define cpu_to_lel(A) cpu_to_le64(A)
#define lel_to_cpu(A) le64_to_cpu(A)
#else
#error "LN2 of BITS_PER_LONG unknown!"
#endif

/*               */
/*                                                  */
struct bm_extent {
	int rs_left; /*                                                  */
	int rs_failed; /*                                                  */
	unsigned long flags;
	struct lc_element lce;
};

#define BME_NO_WRITES  0  /*                                                */
#define BME_LOCKED     1  /*                                             */
#define BME_PRIORITY   2  /*                                                       */

/*               */
/*
                                        
                                                                 
                                                                           
                                                             
 */

#define SLEEP_TIME (HZ/10)

/*                                       
                                                       */
#define BM_BLOCK_SHIFT	12			 /*            */
#define BM_BLOCK_SIZE	 (1<<BM_BLOCK_SHIFT)
/*                                                                  
                                                                       
                             */
#define BM_EXT_SHIFT	 24	/*                          */
#define BM_EXT_SIZE	 (1<<BM_EXT_SHIFT)

#if (BM_EXT_SHIFT != 24) || (BM_BLOCK_SHIFT != 12)
#error "HAVE YOU FIXED drbdmeta AS WELL??"
#endif

/*                                                      */
#define BM_SECT_TO_BIT(x)   ((x)>>(BM_BLOCK_SHIFT-9))
#define BM_BIT_TO_SECT(x)   ((sector_t)(x)<<(BM_BLOCK_SHIFT-9))
#define BM_SECT_PER_BIT     BM_BIT_TO_SECT(1)

/*                                         */
#define Bit2KB(bits) ((bits)<<(BM_BLOCK_SHIFT-10))

/*                                                              
                                  */
#define BM_SECT_TO_EXT(x)   ((x)>>(BM_EXT_SHIFT-9))

/*                                                      */
#define BM_EXT_TO_SECT(x)   ((sector_t)(x) << (BM_EXT_SHIFT-9))
#define BM_SECT_PER_EXT     BM_EXT_TO_SECT(1)

/*                                                                      */
#define AL_EXT_PER_BM_SECT  (1 << (BM_EXT_SHIFT - AL_EXTENT_SHIFT))

#define BM_BLOCKS_PER_BM_EXT_B (BM_EXT_SHIFT - BM_BLOCK_SHIFT)
#define BM_BLOCKS_PER_BM_EXT_MASK  ((1<<BM_BLOCKS_PER_BM_EXT_B) - 1)

/*                                                           
                                                           
                                                
                                                          
                                      
                                              
                                                
                                         
  
                                                                           
                                                                       
                                                                 
 */

#define DRBD_MAX_SECTORS_32 (0xffffffffLU)
/*                                                                         
                                                                             
                                                 
 */

#define DRBD_MAX_SECTORS_FIXED_BM \
	  ((MD_128MB_SECT - MD_32kB_SECT - MD_4kB_SECT) * (1LL<<(BM_EXT_SHIFT-9)))
#if !defined(CONFIG_LBDAF) && BITS_PER_LONG == 32
#define DRBD_MAX_SECTORS      DRBD_MAX_SECTORS_32
#define DRBD_MAX_SECTORS_FLEX DRBD_MAX_SECTORS_32
#else
#define DRBD_MAX_SECTORS      DRBD_MAX_SECTORS_FIXED_BM
/*                           */
#if BITS_PER_LONG == 32
/*                                    
                                                    
                                                           */
#define DRBD_MAX_SECTORS_FLEX BM_BIT_TO_SECT(0xffff7fff)
#else
/*                                                                          */
#define DRBD_MAX_SECTORS_FLEX (1UL << 51)
/*                                            */
#endif
#endif

/*                                       
                                                              
                                                 
                                                            
                                                     
                                 */
#define DRBD_MAX_BIO_SIZE (1U << 20)
#if DRBD_MAX_BIO_SIZE > BIO_MAX_SIZE
#error Architecture not supported: DRBD_MAX_BIO_SIZE > BIO_MAX_SIZE
#endif
#define DRBD_MAX_BIO_SIZE_SAFE (1U << 12)       /*                   */

#define DRBD_MAX_SIZE_H80_PACKET (1U << 15) /*                                                */
#define DRBD_MAX_BIO_SIZE_P95    (1U << 17) /*                                            */

extern int  drbd_bm_init(struct drbd_conf *mdev);
extern int  drbd_bm_resize(struct drbd_conf *mdev, sector_t sectors, int set_new_bits);
extern void drbd_bm_cleanup(struct drbd_conf *mdev);
extern void drbd_bm_set_all(struct drbd_conf *mdev);
extern void drbd_bm_clear_all(struct drbd_conf *mdev);
/*                                          */
extern int  drbd_bm_set_bits(
		struct drbd_conf *mdev, unsigned long s, unsigned long e);
extern int  drbd_bm_clear_bits(
		struct drbd_conf *mdev, unsigned long s, unsigned long e);
extern int drbd_bm_count_bits(
	struct drbd_conf *mdev, const unsigned long s, const unsigned long e);
/*                                                        
                                          */
extern void _drbd_bm_set_bits(struct drbd_conf *mdev,
		const unsigned long s, const unsigned long e);
extern int  drbd_bm_test_bit(struct drbd_conf *mdev, unsigned long bitnr);
extern int  drbd_bm_e_weight(struct drbd_conf *mdev, unsigned long enr);
extern int  drbd_bm_write_page(struct drbd_conf *mdev, unsigned int idx) __must_hold(local);
extern int  drbd_bm_read(struct drbd_conf *mdev) __must_hold(local);
extern void drbd_bm_mark_for_writeout(struct drbd_conf *mdev, int page_nr);
extern int  drbd_bm_write(struct drbd_conf *mdev) __must_hold(local);
extern int  drbd_bm_write_hinted(struct drbd_conf *mdev) __must_hold(local);
extern int drbd_bm_write_all(struct drbd_conf *mdev) __must_hold(local);
extern int  drbd_bm_write_copy_pages(struct drbd_conf *mdev) __must_hold(local);
extern size_t	     drbd_bm_words(struct drbd_conf *mdev);
extern unsigned long drbd_bm_bits(struct drbd_conf *mdev);
extern sector_t      drbd_bm_capacity(struct drbd_conf *mdev);

#define DRBD_END_OF_BITMAP	(~(unsigned long)0)
extern unsigned long drbd_bm_find_next(struct drbd_conf *mdev, unsigned long bm_fo);
/*                                                             */
extern unsigned long _drbd_bm_find_next(struct drbd_conf *mdev, unsigned long bm_fo);
extern unsigned long _drbd_bm_find_next_zero(struct drbd_conf *mdev, unsigned long bm_fo);
extern unsigned long _drbd_bm_total_weight(struct drbd_conf *mdev);
extern unsigned long drbd_bm_total_weight(struct drbd_conf *mdev);
extern int drbd_bm_rs_done(struct drbd_conf *mdev);
/*                    */
extern void drbd_bm_merge_lel(struct drbd_conf *mdev, size_t offset,
		size_t number, unsigned long *buffer);
/*                       */
extern void drbd_bm_get_lel(struct drbd_conf *mdev, size_t offset,
		size_t number, unsigned long *buffer);

extern void drbd_bm_lock(struct drbd_conf *mdev, char *why, enum bm_flag flags);
extern void drbd_bm_unlock(struct drbd_conf *mdev);
/*             */

extern struct kmem_cache *drbd_request_cache;
extern struct kmem_cache *drbd_ee_cache;	/*               */
extern struct kmem_cache *drbd_bm_ext_cache;	/*                */
extern struct kmem_cache *drbd_al_ext_cache;	/*                      */
extern mempool_t *drbd_request_mempool;
extern mempool_t *drbd_ee_mempool;

/*                                                              
                                 
  
                                           
  
                                                                      
                                                                     
  
                                                                       
                                                                       
                                                                               
                         
 */
extern struct page *drbd_pp_pool;
extern spinlock_t   drbd_pp_lock;
extern int	    drbd_pp_vacant;
extern wait_queue_head_t drbd_pp_wait;

/*                                                             
                                           
                                                                       
                                                                             
 */
#define DRBD_MIN_POOL_PAGES	128
extern mempool_t *drbd_md_io_page_pool;

/*                                       
                                             */
extern struct bio_set *drbd_md_io_bio_set;
/*                           */
extern struct bio *bio_alloc_drbd(gfp_t gfp_mask);

extern rwlock_t global_state_lock;

extern int conn_lowest_minor(struct drbd_tconn *tconn);
enum drbd_ret_code conn_new_minor(struct drbd_tconn *tconn, unsigned int minor, int vnr);
extern void drbd_minor_destroy(struct kref *kref);

extern int set_resource_options(struct drbd_tconn *tconn, struct res_opts *res_opts);
extern struct drbd_tconn *conn_create(const char *name, struct res_opts *res_opts);
extern void conn_destroy(struct kref *kref);
struct drbd_tconn *conn_get_by_name(const char *name);
extern struct drbd_tconn *conn_get_by_addrs(void *my_addr, int my_addr_len,
					    void *peer_addr, int peer_addr_len);
extern void conn_free_crypto(struct drbd_tconn *tconn);

extern int proc_details;

/*          */
extern void do_submit(struct work_struct *ws);
extern void __drbd_make_request(struct drbd_conf *, struct bio *, unsigned long);
extern void drbd_make_request(struct request_queue *q, struct bio *bio);
extern int drbd_read_remote(struct drbd_conf *mdev, struct drbd_request *req);
extern int drbd_merge_bvec(struct request_queue *q, struct bvec_merge_data *bvm, struct bio_vec *bvec);
extern int is_valid_ar_handle(struct drbd_request *, sector_t);


/*           */
extern int drbd_msg_put_info(const char *info);
extern void drbd_suspend_io(struct drbd_conf *mdev);
extern void drbd_resume_io(struct drbd_conf *mdev);
extern char *ppsize(char *buf, unsigned long long size);
extern sector_t drbd_new_dev_size(struct drbd_conf *, struct drbd_backing_dev *, sector_t, int);
enum determine_dev_size { dev_size_error = -1, unchanged = 0, shrunk = 1, grew = 2 };
extern enum determine_dev_size drbd_determine_dev_size(struct drbd_conf *, enum dds_flags) __must_hold(local);
extern void resync_after_online_grow(struct drbd_conf *);
extern void drbd_reconsider_max_bio_size(struct drbd_conf *mdev);
extern enum drbd_state_rv drbd_set_role(struct drbd_conf *mdev,
					enum drbd_role new_role,
					int force);
extern bool conn_try_outdate_peer(struct drbd_tconn *tconn);
extern void conn_try_outdate_peer_async(struct drbd_tconn *tconn);
extern int drbd_khelper(struct drbd_conf *mdev, char *cmd);

/*               */
extern int drbd_worker(struct drbd_thread *thi);
enum drbd_ret_code drbd_resync_after_valid(struct drbd_conf *mdev, int o_minor);
void drbd_resync_after_changed(struct drbd_conf *mdev);
extern void drbd_start_resync(struct drbd_conf *mdev, enum drbd_conns side);
extern void resume_next_sg(struct drbd_conf *mdev);
extern void suspend_other_sg(struct drbd_conf *mdev);
extern int drbd_resync_finished(struct drbd_conf *mdev);
/*                            */
extern void *drbd_md_get_buffer(struct drbd_conf *mdev);
extern void drbd_md_put_buffer(struct drbd_conf *mdev);
extern int drbd_md_sync_page_io(struct drbd_conf *mdev,
		struct drbd_backing_dev *bdev, sector_t sector, int rw);
extern void drbd_ov_out_of_sync_found(struct drbd_conf *, sector_t, int);
extern void wait_until_done_or_force_detached(struct drbd_conf *mdev,
		struct drbd_backing_dev *bdev, unsigned int *done);
extern void drbd_rs_controller_reset(struct drbd_conf *mdev);

static inline void ov_out_of_sync_print(struct drbd_conf *mdev)
{
	if (mdev->ov_last_oos_size) {
		dev_err(DEV, "Out of sync: start=%llu, size=%lu (sectors)\n",
		     (unsigned long long)mdev->ov_last_oos_start,
		     (unsigned long)mdev->ov_last_oos_size);
	}
	mdev->ov_last_oos_size=0;
}


extern void drbd_csum_bio(struct drbd_conf *, struct crypto_hash *, struct bio *, void *);
extern void drbd_csum_ee(struct drbd_conf *, struct crypto_hash *,
			 struct drbd_peer_request *, void *);
/*                  */
extern int w_e_end_data_req(struct drbd_work *, int);
extern int w_e_end_rsdata_req(struct drbd_work *, int);
extern int w_e_end_csum_rs_req(struct drbd_work *, int);
extern int w_e_end_ov_reply(struct drbd_work *, int);
extern int w_e_end_ov_req(struct drbd_work *, int);
extern int w_ov_finished(struct drbd_work *, int);
extern int w_resync_timer(struct drbd_work *, int);
extern int w_send_write_hint(struct drbd_work *, int);
extern int w_make_resync_request(struct drbd_work *, int);
extern int w_send_dblock(struct drbd_work *, int);
extern int w_send_read_req(struct drbd_work *, int);
extern int w_prev_work_done(struct drbd_work *, int);
extern int w_e_reissue(struct drbd_work *, int);
extern int w_restart_disk_io(struct drbd_work *, int);
extern int w_send_out_of_sync(struct drbd_work *, int);
extern int w_start_resync(struct drbd_work *, int);

extern void resync_timer_fn(unsigned long data);
extern void start_resync_timer_fn(unsigned long data);

/*                 */
extern int drbd_rs_should_slow_down(struct drbd_conf *mdev, sector_t sector);
extern int drbd_submit_peer_request(struct drbd_conf *,
				    struct drbd_peer_request *, const unsigned,
				    const int);
extern int drbd_free_peer_reqs(struct drbd_conf *, struct list_head *);
extern struct drbd_peer_request *drbd_alloc_peer_req(struct drbd_conf *, u64,
						     sector_t, unsigned int,
						     gfp_t) __must_hold(local);
extern void __drbd_free_peer_req(struct drbd_conf *, struct drbd_peer_request *,
				 int);
#define drbd_free_peer_req(m,e) __drbd_free_peer_req(m, e, 0)
#define drbd_free_net_peer_req(m,e) __drbd_free_peer_req(m, e, 1)
extern struct page *drbd_alloc_pages(struct drbd_conf *, unsigned int, bool);
extern void drbd_set_recv_tcq(struct drbd_conf *mdev, int tcq_enabled);
extern void _drbd_clear_done_ee(struct drbd_conf *mdev, struct list_head *to_be_freed);
extern void conn_flush_workqueue(struct drbd_tconn *tconn);
extern int drbd_connected(struct drbd_conf *mdev);
static inline void drbd_flush_workqueue(struct drbd_conf *mdev)
{
	conn_flush_workqueue(mdev->tconn);
}

/*                                                        
                                       */
static inline int drbd_setsockopt(struct socket *sock, int level, int optname,
				  char *optval, int optlen)
{
	mm_segment_t oldfs = get_fs();
	char __user *uoptval;
	int err;

	uoptval = (char __user __force *)optval;

	set_fs(KERNEL_DS);
	if (level == SOL_SOCKET)
		err = sock_setsockopt(sock, level, optname, uoptval, optlen);
	else
		err = sock->ops->setsockopt(sock, level, optname, uoptval,
					    optlen);
	set_fs(oldfs);
	return err;
}

static inline void drbd_tcp_cork(struct socket *sock)
{
	int val = 1;
	(void) drbd_setsockopt(sock, SOL_TCP, TCP_CORK,
			(char*)&val, sizeof(val));
}

static inline void drbd_tcp_uncork(struct socket *sock)
{
	int val = 0;
	(void) drbd_setsockopt(sock, SOL_TCP, TCP_CORK,
			(char*)&val, sizeof(val));
}

static inline void drbd_tcp_nodelay(struct socket *sock)
{
	int val = 1;
	(void) drbd_setsockopt(sock, SOL_TCP, TCP_NODELAY,
			(char*)&val, sizeof(val));
}

static inline void drbd_tcp_quickack(struct socket *sock)
{
	int val = 2;
	(void) drbd_setsockopt(sock, SOL_TCP, TCP_QUICKACK,
			(char*)&val, sizeof(val));
}

void drbd_bump_write_ordering(struct drbd_tconn *tconn, enum write_ordering_e wo);

/*             */
extern struct proc_dir_entry *drbd_proc;
extern const struct file_operations drbd_proc_fops;
extern const char *drbd_conn_str(enum drbd_conns s);
extern const char *drbd_role_str(enum drbd_role s);

/*               */
extern int drbd_al_begin_io_nonblock(struct drbd_conf *mdev, struct drbd_interval *i);
extern void drbd_al_begin_io_commit(struct drbd_conf *mdev, bool delegate);
extern bool drbd_al_begin_io_fastpath(struct drbd_conf *mdev, struct drbd_interval *i);
extern void drbd_al_begin_io(struct drbd_conf *mdev, struct drbd_interval *i, bool delegate);
extern void drbd_al_complete_io(struct drbd_conf *mdev, struct drbd_interval *i);
extern void drbd_rs_complete_io(struct drbd_conf *mdev, sector_t sector);
extern int drbd_rs_begin_io(struct drbd_conf *mdev, sector_t sector);
extern int drbd_try_rs_begin_io(struct drbd_conf *mdev, sector_t sector);
extern void drbd_rs_cancel_all(struct drbd_conf *mdev);
extern int drbd_rs_del_all(struct drbd_conf *mdev);
extern void drbd_rs_failed_io(struct drbd_conf *mdev,
		sector_t sector, int size);
extern void drbd_advance_rs_marks(struct drbd_conf *mdev, unsigned long still_to_go);
extern void __drbd_set_in_sync(struct drbd_conf *mdev, sector_t sector,
		int size, const char *file, const unsigned int line);
#define drbd_set_in_sync(mdev, sector, size) \
	__drbd_set_in_sync(mdev, sector, size, __FILE__, __LINE__)
extern int __drbd_set_out_of_sync(struct drbd_conf *mdev, sector_t sector,
		int size, const char *file, const unsigned int line);
#define drbd_set_out_of_sync(mdev, sector, size) \
	__drbd_set_out_of_sync(mdev, sector, size, __FILE__, __LINE__)
extern void drbd_al_shrink(struct drbd_conf *mdev);

/*           */
/*                      */
struct sib_info {
	enum drbd_state_info_bcast_reason sib_reason;
	union {
		struct {
			char *helper_name;
			unsigned helper_exit_code;
		};
		struct {
			union drbd_state os;
			union drbd_state ns;
		};
	};
};
void drbd_bcast_event(struct drbd_conf *mdev, const struct sib_info *sib);

/*
                          
                         */

/*                                                        */
static inline struct page *page_chain_next(struct page *page)
{
	return (struct page *)page_private(page);
}
#define page_chain_for_each(page) \
	for (; page && ({ prefetch(page_chain_next(page)); 1; }); \
			page = page_chain_next(page))
#define page_chain_for_each_safe(page, n) \
	for (; page && ({ n = page_chain_next(page); 1; }); page = n)


static inline int drbd_peer_req_has_active_page(struct drbd_peer_request *peer_req)
{
	struct page *page = peer_req->pages;
	page_chain_for_each(page) {
		if (page_count(page) > 1)
			return 1;
	}
	return 0;
}

static inline enum drbd_state_rv
_drbd_set_state(struct drbd_conf *mdev, union drbd_state ns,
		enum chg_state_flags flags, struct completion *done)
{
	enum drbd_state_rv rv;

	read_lock(&global_state_lock);
	rv = __drbd_set_state(mdev, ns, flags, done);
	read_unlock(&global_state_lock);

	return rv;
}

static inline union drbd_state drbd_read_state(struct drbd_conf *mdev)
{
	union drbd_state rv;

	rv.i = mdev->state.i;
	rv.susp = mdev->tconn->susp;
	rv.susp_nod = mdev->tconn->susp_nod;
	rv.susp_fen = mdev->tconn->susp_fen;

	return rv;
}

enum drbd_force_detach_flags {
	DRBD_READ_ERROR,
	DRBD_WRITE_ERROR,
	DRBD_META_IO_ERROR,
	DRBD_FORCE_DETACH,
};

#define __drbd_chk_io_error(m,f) __drbd_chk_io_error_(m,f, __func__)
static inline void __drbd_chk_io_error_(struct drbd_conf *mdev,
		enum drbd_force_detach_flags df,
		const char *where)
{
	enum drbd_io_error_p ep;

	rcu_read_lock();
	ep = rcu_dereference(mdev->ldev->disk_conf)->on_io_error;
	rcu_read_unlock();
	switch (ep) {
	case EP_PASS_ON: /*                                            */
		if (df == DRBD_READ_ERROR || df == DRBD_WRITE_ERROR) {
			if (__ratelimit(&drbd_ratelimit_state))
				dev_err(DEV, "Local IO failed in %s.\n", where);
			if (mdev->state.disk > D_INCONSISTENT)
				_drbd_set_state(_NS(mdev, disk, D_INCONSISTENT), CS_HARD, NULL);
			break;
		}
		/*                                                               */
	case EP_DETACH:
	case EP_CALL_HELPER:
		/*                                               
    
                                                               
                         
                                                                
                                                                
                                                                
    
                                                       
                                                              
    
                                                         
                                                       
                                       
                                                 
    
                                         
                                               
                                       
   */
		set_bit(WAS_IO_ERROR, &mdev->flags);
		if (df == DRBD_READ_ERROR)
			set_bit(WAS_READ_ERROR, &mdev->flags);
		if (df == DRBD_FORCE_DETACH)
			set_bit(FORCE_DETACH, &mdev->flags);
		if (mdev->state.disk > D_FAILED) {
			_drbd_set_state(_NS(mdev, disk, D_FAILED), CS_HARD, NULL);
			dev_err(DEV,
				"Local IO failed in %s. Detaching...\n", where);
		}
		break;
	}
}

/* 
                                                                                                      
                       
                                                           
                                                                                    
  
                                                                                       
 */
#define drbd_chk_io_error(m,e,f) drbd_chk_io_error_(m,e,f, __func__)
static inline void drbd_chk_io_error_(struct drbd_conf *mdev,
	int error, enum drbd_force_detach_flags forcedetach, const char *where)
{
	if (error) {
		unsigned long flags;
		spin_lock_irqsave(&mdev->tconn->req_lock, flags);
		__drbd_chk_io_error_(mdev, forcedetach, where);
		spin_unlock_irqrestore(&mdev->tconn->req_lock, flags);
	}
}


/* 
                                                                                 
                                 
  
                                                                       
                                          
 */
static inline sector_t drbd_md_first_sector(struct drbd_backing_dev *bdev)
{
	switch (bdev->md.meta_dev_idx) {
	case DRBD_MD_INDEX_INTERNAL:
	case DRBD_MD_INDEX_FLEX_INT:
		return bdev->md.md_offset + bdev->md.bm_offset;
	case DRBD_MD_INDEX_FLEX_EXT:
	default:
		return bdev->md.md_offset;
	}
}

/* 
                                                                              
                                 
 */
static inline sector_t drbd_md_last_sector(struct drbd_backing_dev *bdev)
{
	switch (bdev->md.meta_dev_idx) {
	case DRBD_MD_INDEX_INTERNAL:
	case DRBD_MD_INDEX_FLEX_INT:
		return bdev->md.md_offset + MD_4kB_SECT -1;
	case DRBD_MD_INDEX_FLEX_EXT:
	default:
		return bdev->md.md_offset + bdev->md.md_size_sect -1;
	}
}

/*                                                      */
static inline sector_t drbd_get_capacity(struct block_device *bdev)
{
	/*                                                */
	return bdev ? i_size_read(bdev->bd_inode) >> 9 : 0;
}

/* 
                                                                         
                                 
  
                                                                          
                                                                        
                       
 */
static inline sector_t drbd_get_max_capacity(struct drbd_backing_dev *bdev)
{
	sector_t s;

	switch (bdev->md.meta_dev_idx) {
	case DRBD_MD_INDEX_INTERNAL:
	case DRBD_MD_INDEX_FLEX_INT:
		s = drbd_get_capacity(bdev->backing_bdev)
			? min_t(sector_t, DRBD_MAX_SECTORS_FLEX,
				drbd_md_first_sector(bdev))
			: 0;
		break;
	case DRBD_MD_INDEX_FLEX_EXT:
		s = min_t(sector_t, DRBD_MAX_SECTORS_FLEX,
				drbd_get_capacity(bdev->backing_bdev));
		/*                                                  */
		s = min_t(sector_t, s,
			BM_EXT_TO_SECT(bdev->md.md_size_sect
				     - bdev->md.bm_offset));
		break;
	default:
		s = min_t(sector_t, DRBD_MAX_SECTORS,
				drbd_get_capacity(bdev->backing_bdev));
	}
	return s;
}

/* 
                                                                       
                                 
 */
static inline sector_t drbd_md_ss(struct drbd_backing_dev *bdev)
{
	const int meta_dev_idx = bdev->md.meta_dev_idx;

	if (meta_dev_idx == DRBD_MD_INDEX_FLEX_EXT)
		return 0;

	/*                                                       
                                               */
	if (meta_dev_idx == DRBD_MD_INDEX_INTERNAL ||
	    meta_dev_idx == DRBD_MD_INDEX_FLEX_INT)
		return (drbd_get_capacity(bdev->backing_bdev) & ~7ULL) - 8;

	/*                                                         */
	return MD_128MB_SECT * bdev->md.meta_dev_idx;
}

static inline void
drbd_queue_work_front(struct drbd_work_queue *q, struct drbd_work *w)
{
	unsigned long flags;
	spin_lock_irqsave(&q->q_lock, flags);
	list_add(&w->list, &q->q);
	spin_unlock_irqrestore(&q->q_lock, flags);
	wake_up(&q->q_wait);
}

static inline void
drbd_queue_work(struct drbd_work_queue *q, struct drbd_work *w)
{
	unsigned long flags;
	spin_lock_irqsave(&q->q_lock, flags);
	list_add_tail(&w->list, &q->q);
	spin_unlock_irqrestore(&q->q_lock, flags);
	wake_up(&q->q_wait);
}

static inline void wake_asender(struct drbd_tconn *tconn)
{
	if (test_bit(SIGNAL_ASENDER, &tconn->flags))
		force_sig(DRBD_SIG, tconn->asender.task);
}

static inline void request_ping(struct drbd_tconn *tconn)
{
	set_bit(SEND_PING, &tconn->flags);
	wake_asender(tconn);
}

extern void *conn_prepare_command(struct drbd_tconn *, struct drbd_socket *);
extern void *drbd_prepare_command(struct drbd_conf *, struct drbd_socket *);
extern int conn_send_command(struct drbd_tconn *, struct drbd_socket *,
			     enum drbd_packet, unsigned int, void *,
			     unsigned int);
extern int drbd_send_command(struct drbd_conf *, struct drbd_socket *,
			     enum drbd_packet, unsigned int, void *,
			     unsigned int);

extern int drbd_send_ping(struct drbd_tconn *tconn);
extern int drbd_send_ping_ack(struct drbd_tconn *tconn);
extern int drbd_send_state_req(struct drbd_conf *, union drbd_state, union drbd_state);
extern int conn_send_state_req(struct drbd_tconn *, union drbd_state, union drbd_state);

static inline void drbd_thread_stop(struct drbd_thread *thi)
{
	_drbd_thread_stop(thi, false, true);
}

static inline void drbd_thread_stop_nowait(struct drbd_thread *thi)
{
	_drbd_thread_stop(thi, false, false);
}

static inline void drbd_thread_restart_nowait(struct drbd_thread *thi)
{
	_drbd_thread_stop(thi, true, false);
}

/*                                                                
                                            
                                            
             
                  
                                                             
                                                                        
                                                         
                                                                
             
                                                          
                                
                               
                                                                         
                                                    
                                                         
                                                            
                                                       
                                               
                                                
                              
 */
static inline void inc_ap_pending(struct drbd_conf *mdev)
{
	atomic_inc(&mdev->ap_pending_cnt);
}

#define ERR_IF_CNT_IS_NEGATIVE(which, func, line)			\
	if (atomic_read(&mdev->which) < 0)				\
		dev_err(DEV, "in %s:%d: " #which " = %d < 0 !\n",	\
			func, line,					\
			atomic_read(&mdev->which))

#define dec_ap_pending(mdev) _dec_ap_pending(mdev, __FUNCTION__, __LINE__)
static inline void _dec_ap_pending(struct drbd_conf *mdev, const char *func, int line)
{
	if (atomic_dec_and_test(&mdev->ap_pending_cnt))
		wake_up(&mdev->misc_wait);
	ERR_IF_CNT_IS_NEGATIVE(ap_pending_cnt, func, line);
}

/*                                                                     
                            
                                                                      
                                                                                 
                                       
 */
static inline void inc_rs_pending(struct drbd_conf *mdev)
{
	atomic_inc(&mdev->rs_pending_cnt);
}

#define dec_rs_pending(mdev) _dec_rs_pending(mdev, __FUNCTION__, __LINE__)
static inline void _dec_rs_pending(struct drbd_conf *mdev, const char *func, int line)
{
	atomic_dec(&mdev->rs_pending_cnt);
	ERR_IF_CNT_IS_NEGATIVE(rs_pending_cnt, func, line);
}

/*                                                           
               
                                   
                                           
                             
                                                                        
                                                                           
                                                     
 */
static inline void inc_unacked(struct drbd_conf *mdev)
{
	atomic_inc(&mdev->unacked_cnt);
}

#define dec_unacked(mdev) _dec_unacked(mdev, __FUNCTION__, __LINE__)
static inline void _dec_unacked(struct drbd_conf *mdev, const char *func, int line)
{
	atomic_dec(&mdev->unacked_cnt);
	ERR_IF_CNT_IS_NEGATIVE(unacked_cnt, func, line);
}

#define sub_unacked(mdev, n) _sub_unacked(mdev, n, __FUNCTION__, __LINE__)
static inline void _sub_unacked(struct drbd_conf *mdev, int n, const char *func, int line)
{
	atomic_sub(n, &mdev->unacked_cnt);
	ERR_IF_CNT_IS_NEGATIVE(unacked_cnt, func, line);
}

/* 
                                                                                   
                    
  
                                                                     
 */
#define get_ldev(M) __cond_lock(local, _get_ldev_if_state(M,D_INCONSISTENT))
#define get_ldev_if_state(M,MINS) __cond_lock(local, _get_ldev_if_state(M,MINS))

static inline void put_ldev(struct drbd_conf *mdev)
{
	int i = atomic_dec_return(&mdev->local_cnt);

	/*                                            
                               */

	__release(local);
	D_ASSERT(i >= 0);
	if (i == 0) {
		if (mdev->state.disk == D_DISKLESS)
			/*                                                */
			drbd_ldev_destroy(mdev);
		if (mdev->state.disk == D_FAILED) {
			/*                                     */
			if (!test_and_set_bit(GO_DISKLESS, &mdev->flags))
				drbd_queue_work(&mdev->tconn->sender_work, &mdev->go_diskless);
		}
		wake_up(&mdev->misc_wait);
	}
}

#ifndef __CHECKER__
static inline int _get_ldev_if_state(struct drbd_conf *mdev, enum drbd_disk_state mins)
{
	int io_allowed;

	/*                                        */
	if (mdev->state.disk == D_DISKLESS)
		return 0;

	atomic_inc(&mdev->local_cnt);
	io_allowed = (mdev->state.disk >= mins);
	if (!io_allowed)
		put_ldev(mdev);
	return io_allowed;
}
#else
extern int _get_ldev_if_state(struct drbd_conf *mdev, enum drbd_disk_state mins);
#endif

/*                                       */
static inline void drbd_get_syncer_progress(struct drbd_conf *mdev,
		unsigned long *bits_left, unsigned int *per_mil_done)
{
	/*                                                                    
                                                            */
	typecheck(unsigned long, mdev->rs_total);

	/*                                                     
                           
                                       */

	if (mdev->state.conn == C_VERIFY_S || mdev->state.conn == C_VERIFY_T)
		*bits_left = mdev->ov_left;
	else
		*bits_left = drbd_bm_total_weight(mdev) - mdev->rs_failed;
	/*                           
                                   */
	if (*bits_left > mdev->rs_total) {
		/*                                  
                                      
                                               
                                                     
   */
		smp_rmb();
		dev_warn(DEV, "cs:%s rs_left=%lu > rs_total=%lu (rs_failed %lu)\n",
				drbd_conn_str(mdev->state.conn),
				*bits_left, mdev->rs_total, mdev->rs_failed);
		*per_mil_done = 0;
	} else {
		/*                                                
                                                   
                                                       
                                                  
                                                    
                                                     
                                                                 
                                                               
   */
		unsigned int shift = mdev->rs_total > UINT_MAX ? 16 : 10;
		unsigned long left = *bits_left >> shift;
		unsigned long total = 1UL + (mdev->rs_total >> shift);
		unsigned long tmp = 1000UL - left * 1000UL/total;
		*per_mil_done = tmp;
	}
}


/*                                               
                                     
                                        */
static inline int drbd_get_max_buffers(struct drbd_conf *mdev)
{
	struct net_conf *nc;
	int mxb;

	rcu_read_lock();
	nc = rcu_dereference(mdev->tconn->net_conf);
	mxb = nc ? nc->max_buffers : 1000000;  /*                                  */
	rcu_read_unlock();

	return mxb;
}

static inline int drbd_state_is_stable(struct drbd_conf *mdev)
{
	union drbd_dev_state s = mdev->state;

	/*                                                             
                                                                     */

	switch ((enum drbd_conns)s.conn) {
	/*                                                       */
	case C_STANDALONE:
	case C_WF_CONNECTION:
	/*                                                */
	case C_CONNECTED:
	case C_SYNC_SOURCE:
	case C_SYNC_TARGET:
	case C_VERIFY_S:
	case C_VERIFY_T:
	case C_PAUSED_SYNC_S:
	case C_PAUSED_SYNC_T:
	case C_AHEAD:
	case C_BEHIND:
		/*                                 */
	case C_DISCONNECTING:
	case C_UNCONNECTED:
	case C_TIMEOUT:
	case C_BROKEN_PIPE:
	case C_NETWORK_FAILURE:
	case C_PROTOCOL_ERROR:
	case C_TEAR_DOWN:
	case C_WF_REPORT_PARAMS:
	case C_STARTING_SYNC_S:
	case C_STARTING_SYNC_T:
		break;

		/*                                                   */
	case C_WF_BITMAP_S:
		if (mdev->tconn->agreed_pro_version < 96)
			return 0;
		break;

		/*                                    */
	case C_WF_BITMAP_T:
	case C_WF_SYNC_UUID:
	case C_MASK:
		/*              */
		return 0;
	}

	switch ((enum drbd_disk_state)s.disk) {
	case D_DISKLESS:
	case D_INCONSISTENT:
	case D_OUTDATED:
	case D_CONSISTENT:
	case D_UP_TO_DATE:
	case D_FAILED:
		/*                               */
		break;

	/*                                               */
	case D_ATTACHING:
	case D_NEGOTIATING:
	case D_UNKNOWN:
	case D_MASK:
		/*              */
		return 0;
	}

	return 1;
}

static inline int drbd_suspended(struct drbd_conf *mdev)
{
	struct drbd_tconn *tconn = mdev->tconn;

	return tconn->susp || tconn->susp_fen || tconn->susp_nod;
}

static inline bool may_inc_ap_bio(struct drbd_conf *mdev)
{
	int mxb = drbd_get_max_buffers(mdev);

	if (drbd_suspended(mdev))
		return false;
	if (test_bit(SUSPEND_IO, &mdev->flags))
		return false;

	/*                                                  
                                                       
                                     */

	/*                                                         */
	if (!drbd_state_is_stable(mdev))
		return false;

	/*                                                       
                                                                      */
	if (atomic_read(&mdev->ap_bio_cnt) > mxb)
		return false;
	if (test_bit(BITMAP_IO, &mdev->flags))
		return false;
	return true;
}

static inline bool inc_ap_bio_cond(struct drbd_conf *mdev)
{
	bool rv = false;

	spin_lock_irq(&mdev->tconn->req_lock);
	rv = may_inc_ap_bio(mdev);
	if (rv)
		atomic_inc(&mdev->ap_bio_cnt);
	spin_unlock_irq(&mdev->tconn->req_lock);

	return rv;
}

static inline void inc_ap_bio(struct drbd_conf *mdev)
{
	/*             
                                         
                                                                 
                                                                 
   
                                           
                                               */

	wait_event(mdev->misc_wait, inc_ap_bio_cond(mdev));
}

static inline void dec_ap_bio(struct drbd_conf *mdev)
{
	int mxb = drbd_get_max_buffers(mdev);
	int ap_bio = atomic_dec_return(&mdev->ap_bio_cnt);

	D_ASSERT(ap_bio >= 0);

	if (ap_bio == 0 && test_bit(BITMAP_IO, &mdev->flags)) {
		if (!test_and_set_bit(BITMAP_IO_QUEUED, &mdev->flags))
			drbd_queue_work(&mdev->tconn->sender_work, &mdev->bm_io_work.w);
	}

	/*                                                  
                                                   
                                            */
	if (ap_bio < mxb)
		wake_up(&mdev->misc_wait);
}

static inline bool verify_can_do_stop_sector(struct drbd_conf *mdev)
{
	return mdev->tconn->agreed_pro_version >= 97 &&
		mdev->tconn->agreed_pro_version != 100;
}

static inline int drbd_set_ed_uuid(struct drbd_conf *mdev, u64 val)
{
	int changed = mdev->ed_uuid != val;
	mdev->ed_uuid = val;
	return changed;
}

static inline int drbd_queue_order_type(struct drbd_conf *mdev)
{
	/*                                                   
                             */
#ifndef QUEUE_ORDERED_NONE
#define QUEUE_ORDERED_NONE 0
#endif
	return QUEUE_ORDERED_NONE;
}

static inline void drbd_md_flush(struct drbd_conf *mdev)
{
	int r;

	if (mdev->ldev == NULL) {
		dev_warn(DEV, "mdev->ldev == NULL in drbd_md_flush\n");
		return;
	}

	if (test_bit(MD_NO_FUA, &mdev->flags))
		return;

	r = blkdev_issue_flush(mdev->ldev->md_bdev, GFP_NOIO, NULL);
	if (r) {
		set_bit(MD_NO_FUA, &mdev->flags);
		dev_err(DEV, "meta data flush failed with status %d, disabling md-flushes\n", r);
	}
}

#endif
