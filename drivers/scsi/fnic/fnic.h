/*
 * Copyright 2008 Cisco Systems, Inc.  All rights reserved.
 * Copyright 2007 Nuova Systems, Inc.  All rights reserved.
 *
 * This program is free software; you may redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef _FNIC_H_
#define _FNIC_H_

#include <linux/interrupt.h>
#include <linux/netdevice.h>
#include <linux/workqueue.h>
#include <linux/bitops.h>
#include <scsi/libfc.h>
#include <scsi/libfcoe.h>
#include "fnic_io.h"
#include "fnic_res.h"
#include "fnic_trace.h"
#include "vnic_dev.h"
#include "vnic_wq.h"
#include "vnic_rq.h"
#include "vnic_cq.h"
#include "vnic_wq_copy.h"
#include "vnic_intr.h"
#include "vnic_stats.h"
#include "vnic_scsi.h"

#define DRV_NAME		"fnic"
#define DRV_DESCRIPTION		"Cisco FCoE HBA Driver"
#define DRV_VERSION		"1.5.0.22"
#define PFX			DRV_NAME ": "
#define DFX                     DRV_NAME "%d: "

#define DESC_CLEAN_LOW_WATERMARK 8
#define FNIC_MAX_IO_REQ		2048 /*                           */
#define	FNIC_IO_LOCKS		64 /*                      */
#define FNIC_DFLT_QUEUE_DEPTH	32
#define	FNIC_STATS_RATE_LIMIT	4 /*                                         */

/*
                                      
 */
#define FNIC_TAG_ABORT		BIT(30)		/*                          */
#define FNIC_TAG_DEV_RST	BIT(29)		/*                        */
#define FNIC_TAG_MASK		(BIT(24) - 1)	/*                 */
#define FNIC_NO_TAG             -1

/*
                                                                     
       
 */
#define FNIC_NO_FLAGS                   0
#define FNIC_IO_INITIALIZED             BIT(0)
#define FNIC_IO_ISSUED                  BIT(1)
#define FNIC_IO_DONE                    BIT(2)
#define FNIC_IO_REQ_NULL                BIT(3)
#define FNIC_IO_ABTS_PENDING            BIT(4)
#define FNIC_IO_ABORTED                 BIT(5)
#define FNIC_IO_ABTS_ISSUED             BIT(6)
#define FNIC_IO_TERM_ISSUED             BIT(7)
#define FNIC_IO_INTERNAL_TERM_ISSUED    BIT(8)
#define FNIC_IO_ABT_TERM_DONE           BIT(9)
#define FNIC_IO_ABT_TERM_REQ_NULL       BIT(10)
#define FNIC_IO_ABT_TERM_TIMED_OUT      BIT(11)
#define FNIC_DEVICE_RESET               BIT(12)  /*                      */
#define FNIC_DEV_RST_ISSUED             BIT(13)
#define FNIC_DEV_RST_TIMED_OUT          BIT(14)
#define FNIC_DEV_RST_ABTS_ISSUED        BIT(15)
#define FNIC_DEV_RST_TERM_ISSUED        BIT(16)
#define FNIC_DEV_RST_DONE               BIT(17)
#define FNIC_DEV_RST_REQ_NULL           BIT(18)
#define FNIC_DEV_RST_ABTS_DONE          BIT(19)
#define FNIC_DEV_RST_TERM_DONE          BIT(20)
#define FNIC_DEV_RST_ABTS_PENDING       BIT(21)

/*
                                     
                                                     
 */
#define CMD_SP(Cmnd)		((Cmnd)->SCp.ptr)
#define CMD_STATE(Cmnd)		((Cmnd)->SCp.phase)
#define CMD_ABTS_STATUS(Cmnd)	((Cmnd)->SCp.Message)
#define CMD_LR_STATUS(Cmnd)	((Cmnd)->SCp.have_data_in)
#define CMD_TAG(Cmnd)           ((Cmnd)->SCp.sent_command)
#define CMD_FLAGS(Cmnd)         ((Cmnd)->SCp.Status)

#define FCPIO_INVALID_CODE 0x100 /*                                     */

#define FNIC_LUN_RESET_TIMEOUT	     10000	/*      */
#define FNIC_HOST_RESET_TIMEOUT	     10000	/*      */
#define FNIC_RMDEVICE_TIMEOUT        1000       /*      */
#define FNIC_HOST_RESET_SETTLE_TIME  30         /*     */
#define FNIC_ABT_TERM_DELAY_TIMEOUT  500        /*      */

#define FNIC_MAX_FCP_TARGET     256

/* 
                                                                   
  */
#define __FNIC_FLAGS_FWRESET		BIT(0) /*                     */
#define __FNIC_FLAGS_BLOCK_IO		BIT(1) /*                 */

#define FNIC_FLAGS_NONE			(0)
#define FNIC_FLAGS_FWRESET		(__FNIC_FLAGS_FWRESET | \
					__FNIC_FLAGS_BLOCK_IO)

#define FNIC_FLAGS_IO_BLOCKED		(__FNIC_FLAGS_BLOCK_IO)

#define fnic_set_state_flags(fnicp, st_flags)	\
	__fnic_set_state_flags(fnicp, st_flags, 0)

#define fnic_clear_state_flags(fnicp, st_flags)  \
	__fnic_set_state_flags(fnicp, st_flags, 1)

extern unsigned int fnic_log_level;

#define FNIC_MAIN_LOGGING 0x01
#define FNIC_FCS_LOGGING 0x02
#define FNIC_SCSI_LOGGING 0x04
#define FNIC_ISR_LOGGING 0x08

#define FNIC_CHECK_LOGGING(LEVEL, CMD)				\
do {								\
	if (unlikely(fnic_log_level & LEVEL))			\
		do {						\
			CMD;					\
		} while (0);					\
} while (0)

#define FNIC_MAIN_DBG(kern_level, host, fmt, args...)		\
	FNIC_CHECK_LOGGING(FNIC_MAIN_LOGGING,			\
			 shost_printk(kern_level, host, fmt, ##args);)

#define FNIC_FCS_DBG(kern_level, host, fmt, args...)		\
	FNIC_CHECK_LOGGING(FNIC_FCS_LOGGING,			\
			 shost_printk(kern_level, host, fmt, ##args);)

#define FNIC_SCSI_DBG(kern_level, host, fmt, args...)		\
	FNIC_CHECK_LOGGING(FNIC_SCSI_LOGGING,			\
			 shost_printk(kern_level, host, fmt, ##args);)

#define FNIC_ISR_DBG(kern_level, host, fmt, args...)		\
	FNIC_CHECK_LOGGING(FNIC_ISR_LOGGING,			\
			 shost_printk(kern_level, host, fmt, ##args);)

extern const char *fnic_state_str[];

enum fnic_intx_intr_index {
	FNIC_INTX_WQ_RQ_COPYWQ,
	FNIC_INTX_ERR,
	FNIC_INTX_NOTIFY,
	FNIC_INTX_INTR_MAX,
};

enum fnic_msix_intr_index {
	FNIC_MSIX_RQ,
	FNIC_MSIX_WQ,
	FNIC_MSIX_WQ_COPY,
	FNIC_MSIX_ERR_NOTIFY,
	FNIC_MSIX_INTR_MAX,
};

struct fnic_msix_entry {
	int requested;
	char devname[IFNAMSIZ];
	irqreturn_t (*isr)(int, void *);
	void *devid;
};

enum fnic_state {
	FNIC_IN_FC_MODE = 0,
	FNIC_IN_FC_TRANS_ETH_MODE,
	FNIC_IN_ETH_MODE,
	FNIC_IN_ETH_TRANS_FC_MODE,
};

#define FNIC_WQ_COPY_MAX 1
#define FNIC_WQ_MAX 1
#define FNIC_RQ_MAX 1
#define FNIC_CQ_MAX (FNIC_WQ_COPY_MAX + FNIC_WQ_MAX + FNIC_RQ_MAX)

struct mempool;

enum fnic_evt {
	FNIC_EVT_START_VLAN_DISC = 1,
	FNIC_EVT_START_FCF_DISC = 2,
	FNIC_EVT_MAX,
};

struct fnic_event {
	struct list_head list;
	struct fnic *fnic;
	enum fnic_evt event;
};

/*                                     */
struct fnic {
	struct fc_lport *lport;
	struct fcoe_ctlr ctlr;		/*                               */
	struct vnic_dev_bar bar0;

	struct msix_entry msix_entry[FNIC_MSIX_INTR_MAX];
	struct fnic_msix_entry msix[FNIC_MSIX_INTR_MAX];

	struct vnic_stats *stats;
	unsigned long stats_time;	/*                      */
	struct vnic_nic_cfg *nic_cfg;
	char name[IFNAMSIZ];
	struct timer_list notify_timer; /*                         */

	unsigned int err_intr_offset;
	unsigned int link_intr_offset;

	unsigned int wq_count;
	unsigned int cq_count;

	u32 vlan_hw_insert:1;	        /*                       */
	u32 in_remove:1;                /*                        */
	u32 stop_rx_link_events:1;      /*                                   */

	struct completion *remove_wait; /*                             */

	atomic_t in_flight;		/*            */
	u32 _reserved;			/*           */
	unsigned long state_flags;	/*                        */
	enum fnic_state state;
	spinlock_t fnic_lock;

	u16 vlan_id;	                /*                             */
	u8 data_src_addr[ETH_ALEN];
	u64 fcp_input_bytes;		/*                    */
	u64 fcp_output_bytes;		/*                    */
	u32 link_down_cnt;
	int link_status;

	struct list_head list;
	struct pci_dev *pdev;
	struct vnic_fc_config config;
	struct vnic_dev *vdev;
	unsigned int raw_wq_count;
	unsigned int wq_copy_count;
	unsigned int rq_count;
	int fw_ack_index[FNIC_WQ_COPY_MAX];
	unsigned short fw_ack_recd[FNIC_WQ_COPY_MAX];
	unsigned short wq_copy_desc_low[FNIC_WQ_COPY_MAX];
	unsigned int intr_count;
	u32 __iomem *legacy_pba;
	struct fnic_host_tag *tags;
	mempool_t *io_req_pool;
	mempool_t *io_sgl_pool[FNIC_SGL_NUM_CACHES];
	spinlock_t io_req_lock[FNIC_IO_LOCKS];	/*                      */

	struct work_struct link_work;
	struct work_struct frame_work;
	struct sk_buff_head frame_queue;
	struct sk_buff_head tx_queue;

	/*                                        */
	void (*set_vlan)(struct fnic *, u16 vlan);
	struct work_struct      fip_frame_work;
	struct sk_buff_head     fip_frame_queue;
	struct timer_list       fip_timer;
	struct list_head        vlans;
	spinlock_t              vlans_lock;

	struct work_struct      event_work;
	struct list_head        evlist;
	/*                                      */

	/*                                    */
	____cacheline_aligned struct vnic_wq_copy wq_copy[FNIC_WQ_COPY_MAX];
	/*                                     */
	____cacheline_aligned struct vnic_cq cq[FNIC_CQ_MAX];

	spinlock_t wq_copy_lock[FNIC_WQ_COPY_MAX];

	/*                               */
	____cacheline_aligned struct vnic_wq wq[FNIC_WQ_MAX];
	spinlock_t wq_lock[FNIC_WQ_MAX];

	/*                                  */
	____cacheline_aligned struct vnic_rq rq[FNIC_RQ_MAX];

	/*                                       */
	____cacheline_aligned struct vnic_intr intr[FNIC_MSIX_INTR_MAX];
};

static inline struct fnic *fnic_from_ctlr(struct fcoe_ctlr *fip)
{
	return container_of(fip, struct fnic, ctlr);
}

extern struct workqueue_struct *fnic_event_queue;
extern struct workqueue_struct *fnic_fip_queue;
extern struct device_attribute *fnic_attrs[];

void fnic_clear_intr_mode(struct fnic *fnic);
int fnic_set_intr_mode(struct fnic *fnic);
void fnic_free_intr(struct fnic *fnic);
int fnic_request_intr(struct fnic *fnic);

int fnic_send(struct fc_lport *, struct fc_frame *);
void fnic_free_wq_buf(struct vnic_wq *wq, struct vnic_wq_buf *buf);
void fnic_handle_frame(struct work_struct *work);
void fnic_handle_link(struct work_struct *work);
void fnic_handle_event(struct work_struct *work);
int fnic_rq_cmpl_handler(struct fnic *fnic, int);
int fnic_alloc_rq_frame(struct vnic_rq *rq);
void fnic_free_rq_buf(struct vnic_rq *rq, struct vnic_rq_buf *buf);
void fnic_flush_tx(struct fnic *);
void fnic_eth_send(struct fcoe_ctlr *, struct sk_buff *skb);
void fnic_set_port_id(struct fc_lport *, u32, struct fc_frame *);
void fnic_update_mac(struct fc_lport *, u8 *new);
void fnic_update_mac_locked(struct fnic *, u8 *new);

int fnic_queuecommand(struct Scsi_Host *, struct scsi_cmnd *);
int fnic_abort_cmd(struct scsi_cmnd *);
int fnic_device_reset(struct scsi_cmnd *);
int fnic_host_reset(struct scsi_cmnd *);
int fnic_reset(struct Scsi_Host *);
void fnic_scsi_cleanup(struct fc_lport *);
void fnic_scsi_abort_io(struct fc_lport *);
void fnic_empty_scsi_cleanup(struct fc_lport *);
void fnic_exch_mgr_reset(struct fc_lport *, u32, u32);
int fnic_wq_copy_cmpl_handler(struct fnic *fnic, int);
int fnic_wq_cmpl_handler(struct fnic *fnic, int);
int fnic_flogi_reg_handler(struct fnic *fnic, u32);
void fnic_wq_copy_cleanup_handler(struct vnic_wq_copy *wq,
				  struct fcpio_host_req *desc);
int fnic_fw_reset_handler(struct fnic *fnic);
void fnic_terminate_rport_io(struct fc_rport *);
const char *fnic_state_to_str(unsigned int state);

void fnic_log_q_error(struct fnic *fnic);
void fnic_handle_link_event(struct fnic *fnic);

int fnic_is_abts_pending(struct fnic *, struct scsi_cmnd *);

void fnic_handle_fip_frame(struct work_struct *work);
void fnic_handle_fip_event(struct fnic *fnic);
void fnic_fcoe_reset_vlans(struct fnic *fnic);
void fnic_fcoe_evlist_free(struct fnic *fnic);
extern void fnic_handle_fip_timer(struct fnic *fnic);

static inline int
fnic_chk_state_flags_locked(struct fnic *fnic, unsigned long st_flags)
{
	return ((fnic->state_flags & st_flags) == st_flags);
}
void __fnic_set_state_flags(struct fnic *, unsigned long, unsigned long);
#endif /*          */
