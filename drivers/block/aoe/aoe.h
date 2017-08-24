/* Copyright (c) 2012 Coraid, Inc.  See COPYING for GPL terms. */
#define VERSION "81"
#define AOE_MAJOR 152
#define DEVICE_NAME "aoe"

/*                                                
                                                            
 */
#ifndef AOE_PARTITIONS
#define AOE_PARTITIONS (16)
#endif

#define WHITESPACE " \t\v\f\n,"

enum {
	AOECMD_ATA,
	AOECMD_CFG,
	AOECMD_VEND_MIN = 0xf0,

	AOEFL_RSP = (1<<3),
	AOEFL_ERR = (1<<2),

	AOEAFL_EXT = (1<<6),
	AOEAFL_DEV = (1<<4),
	AOEAFL_ASYNC = (1<<1),
	AOEAFL_WRITE = (1<<0),

	AOECCMD_READ = 0,
	AOECCMD_TEST,
	AOECCMD_PTEST,
	AOECCMD_SET,
	AOECCMD_FSET,

	AOE_HVER = 0x10,
};

struct aoe_hdr {
	unsigned char dst[6];
	unsigned char src[6];
	__be16 type;
	unsigned char verfl;
	unsigned char err;
	__be16 major;
	unsigned char minor;
	unsigned char cmd;
	__be32 tag;
};

struct aoe_atahdr {
	unsigned char aflags;
	unsigned char errfeat;
	unsigned char scnt;
	unsigned char cmdstat;
	unsigned char lba0;
	unsigned char lba1;
	unsigned char lba2;
	unsigned char lba3;
	unsigned char lba4;
	unsigned char lba5;
	unsigned char res[2];
};

struct aoe_cfghdr {
	__be16 bufcnt;
	__be16 fwver;
	unsigned char scnt;
	unsigned char aoeccmd;
	unsigned char cslen[2];
};

enum {
	DEVFL_UP = 1,	/*                                                               */
	DEVFL_TKILL = (1<<1),	/*                                          */
	DEVFL_EXT = (1<<2),	/*                               */
	DEVFL_GDALLOC = (1<<3),	/*                       */
	DEVFL_GD_NOW = (1<<4),	/*                    */
	DEVFL_KICKME = (1<<5),	/*                                 */
	DEVFL_NEWSIZE = (1<<6),	/*                                        */
	DEVFL_FREEING = (1<<7),	/*                                     */
	DEVFL_FREED = (1<<8),	/*                            */
};

enum {
	DEFAULTBCNT = 2 * 512,	/*           */
	MIN_BUFS = 16,
	NTARGETS = 4,
	NAOEIFS = 8,
	NSKBPOOLMAX = 256,
	NFACTIVE = 61,

	TIMERTICK = HZ / 10,
	RTTSCALE = 8,
	RTTDSCALE = 3,
	RTTAVG_INIT = USEC_PER_SEC / 4 << RTTSCALE,
	RTTDEV_INIT = RTTAVG_INIT / 4,

	HARD_SCORN_SECS = 10,	/*                                    */
	MAX_TAINT = 1000,	/*                     */
};

struct buf {
	ulong nframesout;
	ulong resid;
	ulong bv_resid;
	sector_t sector;
	struct bio *bio;
	struct bio_vec *bv;
	struct request *rq;
};

enum frame_flags {
	FFL_PROBE = 1,
};

struct frame {
	struct list_head head;
	u32 tag;
	struct timeval sent;	/*                               */
	u32 sent_jiffs;		/*                                 */
	ulong waited;
	ulong waited_total;
	struct aoetgt *t;		/*                           */
	sector_t lba;
	struct sk_buff *skb;		/*                                  */
	struct sk_buff *r_skb;		/*                                   */
	struct buf *buf;
	struct bio_vec *bv;
	ulong bcnt;
	ulong bv_off;
	char flags;
};

struct aoeif {
	struct net_device *nd;
	ulong lost;
	int bcnt;
};

struct aoetgt {
	unsigned char addr[6];
	ushort nframes;		/*                      */
	struct aoedev *d;			/*                           */
	struct list_head ffree;			/*                     */
	struct aoeif ifs[NAOEIFS];
	struct aoeif *ifp;	/*                      */
	ushort nout;		/*                                    */
	ushort maxout;		/*                                   */
	ushort next_cwnd;	/*                                        */
	ushort ssthresh;	/*                      */
	ulong falloc;		/*                            */
	int taint;		/*                                       */
	int minbcnt;
	int wpkts, rpkts;
	char nout_probes;
};

struct aoedev {
	struct aoedev *next;
	ulong sysminor;
	ulong aoemajor;
	u32 rttavg;		/*                                    */
	u32 rttdev;		/*                                       */
	u16 aoeminor;
	u16 flags;
	u16 nopen;		/*                                               */
	u16 fw_ver;		/*                             */
	u16 lasttag;		/*               */
	u16 useme;
	ulong ref;
	struct work_struct work;/*                         */
	struct gendisk *gd;
	struct request_queue *blkq;
	struct hd_geometry geo;
	sector_t ssize;
	struct timer_list timer;
	spinlock_t lock;
	struct sk_buff_head skbpool;
	mempool_t *bufpool;	/*                                  */
	struct {		/*                              */
		struct buf *buf;
		struct bio *nxbio;
		struct request *rq;
	} ip;
	ulong maxbcnt;
	struct list_head factive[NFACTIVE];	/*                       */
	struct list_head rexmitq; /*                          */
	struct aoetgt **targets;
	ulong ntargets;		/*                                     */
	struct aoetgt **tgt;	/*                            */
	ulong kicked;
	char ident[512];
};

/*                  */
struct ktstate {
	struct completion rendez;
	struct task_struct *task;
	wait_queue_head_t *waitq;
	int (*fn) (void);
	char *name;
	spinlock_t *lock;
};

int aoeblk_init(void);
void aoeblk_exit(void);
void aoeblk_gdalloc(void *);
void aoedisk_rm_sysfs(struct aoedev *d);

int aoechr_init(void);
void aoechr_exit(void);
void aoechr_error(char *);

void aoecmd_work(struct aoedev *d);
void aoecmd_cfg(ushort aoemajor, unsigned char aoeminor);
struct sk_buff *aoecmd_ata_rsp(struct sk_buff *);
void aoecmd_cfg_rsp(struct sk_buff *);
void aoecmd_sleepwork(struct work_struct *);
void aoecmd_wreset(struct aoetgt *t);
void aoecmd_cleanslate(struct aoedev *);
void aoecmd_exit(void);
int aoecmd_init(void);
struct sk_buff *aoecmd_ata_id(struct aoedev *);
void aoe_freetframe(struct frame *);
void aoe_flush_iocq(void);
void aoe_end_request(struct aoedev *, struct request *, int);
int aoe_ktstart(struct ktstate *k);
void aoe_ktstop(struct ktstate *k);

int aoedev_init(void);
void aoedev_exit(void);
struct aoedev *aoedev_by_aoeaddr(ulong maj, int min, int do_alloc);
void aoedev_downdev(struct aoedev *d);
int aoedev_flush(const char __user *str, size_t size);
void aoe_failbuf(struct aoedev *, struct buf *);
void aoedev_put(struct aoedev *);

int aoenet_init(void);
void aoenet_exit(void);
void aoenet_xmit(struct sk_buff_head *);
int is_aoe_netif(struct net_device *ifp);
int set_aoe_iflist(const char __user *str, size_t size);
