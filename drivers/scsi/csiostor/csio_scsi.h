/*
 * This file is part of the Chelsio FCoE driver for Linux.
 *
 * Copyright (c) 2008-2012 Chelsio Communications, Inc. All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * OpenIB.org BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
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

#ifndef __CSIO_SCSI_H__
#define __CSIO_SCSI_H__

#include <linux/spinlock_types.h>
#include <linux/completion.h>
#include <scsi/scsi.h>
#include <scsi/scsi_cmnd.h>
#include <scsi/scsi_device.h>
#include <scsi/scsi_host.h>
#include <scsi/scsi_eh.h>
#include <scsi/scsi_tcq.h>
#include <scsi/fc/fc_fcp.h>

#include "csio_defs.h"
#include "csio_wr.h"

extern struct scsi_host_template csio_fcoe_shost_template;
extern struct scsi_host_template csio_fcoe_shost_vport_template;

extern int csio_scsi_eqsize;
extern int csio_scsi_iqlen;
extern int csio_scsi_ioreqs;
extern uint32_t csio_max_scan_tmo;
extern uint32_t csio_delta_scan_tmo;
extern int csio_lun_qdepth;

/*
                                                                  
                                                            
                                 
                                                          
                                                          
                                                           
            
          
            
                                                               
                                                
                          
                                
                                                                      
                                                            
                                                       
 */
#define CSIO_SCSI_MAX_SGE		35
#define CSIO_SCSI_ABRT_TMO_MS		60000
#define CSIO_SCSI_LUNRST_TMO_MS		60000
#define CSIO_SCSI_TM_POLL_MS		2000	/*                    
                         
       */
#define CSIO_SCSI_IQ_WRSZ		128
#define CSIO_SCSI_IQSIZE		(csio_scsi_iqlen * CSIO_SCSI_IQ_WRSZ)

#define	CSIO_MAX_SNS_LEN		128
#define	CSIO_SCSI_RSP_LEN	(FCP_RESP_WITH_EXT + 4 + CSIO_MAX_SNS_LEN)

/*                        */
#define csio_scsi_cmnd(req)		((req)->scratch1)

struct csio_scsi_stats {
	uint64_t		n_tot_success;	/*                           */
	uint32_t		n_rn_nr_error;	/*                        
                     
       */
	uint32_t		n_hw_nr_error;	/*                      
                     
       */
	uint32_t		n_dmamap_error;	/*                      */
	uint32_t		n_unsupp_sge_error; /*                     
                    
           */
	uint32_t		n_no_req_error;	/*                            */
	uint32_t		n_busy_error;	/*                      */
	uint32_t		n_hosterror;	/*                         */
	uint32_t		n_rsperror;	/*                        */
	uint32_t		n_autosense;	/*                           */
	uint32_t		n_ovflerror;	/*                        */
	uint32_t		n_unflerror;	/*                         */
	uint32_t		n_rdev_nr_error;/*                
                     
       */
	uint32_t		n_rdev_lost_error;/*                         */
	uint32_t		n_rdev_logo_error;/*                         */
	uint32_t		n_link_down_error;/*                         */
	uint32_t		n_no_xchg_error; /*                       */
	uint32_t		n_unknown_error;/*                         */
	uint32_t		n_aborted;	/*                     */
	uint32_t		n_abrt_timedout; /*                        */
	uint32_t		n_abrt_fail;	/*                       */
	uint32_t		n_abrt_dups;	/*                         */
	uint32_t		n_abrt_race_comp; /*                         
                            
         */
	uint32_t		n_abrt_busy_error;/*                      
                         
         */
	uint32_t		n_closed;	/*                    */
	uint32_t		n_cls_busy_error; /*                      
                         
         */
	uint32_t		n_active;	/*                        */
	uint32_t		n_tm_active;	/*                        */
	uint32_t		n_wcbfn;	/*                      
               
       */
	uint32_t		n_free_ioreq;	/*                         */
	uint32_t		n_free_ddp;	/*                     */
	uint32_t		n_unaligned;	/*                       */
	uint32_t		n_inval_cplop;	/*                            */
	uint32_t		n_inval_scsiop;	/*                            */
};

struct csio_scsim {
	struct csio_hw		*hw;		/*                      */
	uint8_t			max_sge;	/*         */
	uint8_t			proto_cmd_len;	/*                    
                   
       */
	uint16_t		proto_rsp_len;	/*                    
                        
       */
	spinlock_t		freelist_lock;	/*                         */
	struct list_head	active_q;	/*                       */
	struct list_head	ioreq_freelist;	/*                      */
	struct list_head	ddp_freelist;	/*                         */
	struct csio_scsi_stats	stats;		/*                          */
};

/*                       */
enum csio_scsi_ev {
	CSIO_SCSIE_START_IO = 1,		/*                         */
	CSIO_SCSIE_START_TM,			/*               */
	CSIO_SCSIE_COMPLETED,			/*              */
	CSIO_SCSIE_ABORT,			/*          */
	CSIO_SCSIE_ABORTED,			/*            */
	CSIO_SCSIE_CLOSE,			/*                */
	CSIO_SCSIE_CLOSED,			/*                 */
	CSIO_SCSIE_DRVCLEANUP,			/*                         
                          
       */
};

enum csio_scsi_lev {
	CSIO_LEV_ALL = 1,
	CSIO_LEV_LNODE,
	CSIO_LEV_RNODE,
	CSIO_LEV_LUN,
};

struct csio_scsi_level_data {
	enum csio_scsi_lev	level;
	struct csio_rnode	*rnode;
	struct csio_lnode	*lnode;
	uint64_t		oslun;
};

static inline struct csio_ioreq *
csio_get_scsi_ioreq(struct csio_scsim *scm)
{
	struct csio_sm *req;

	if (likely(!list_empty(&scm->ioreq_freelist))) {
		req = list_first_entry(&scm->ioreq_freelist,
				       struct csio_sm, sm_list);
		list_del_init(&req->sm_list);
		CSIO_DEC_STATS(scm, n_free_ioreq);
		return (struct csio_ioreq *)req;
	} else
		return NULL;
}

static inline void
csio_put_scsi_ioreq(struct csio_scsim *scm, struct csio_ioreq *ioreq)
{
	list_add_tail(&ioreq->sm.sm_list, &scm->ioreq_freelist);
	CSIO_INC_STATS(scm, n_free_ioreq);
}

static inline void
csio_put_scsi_ioreq_list(struct csio_scsim *scm, struct list_head *reqlist,
			 int n)
{
	list_splice_init(reqlist, &scm->ioreq_freelist);
	scm->stats.n_free_ioreq += n;
}

static inline struct csio_dma_buf *
csio_get_scsi_ddp(struct csio_scsim *scm)
{
	struct csio_dma_buf *ddp;

	if (likely(!list_empty(&scm->ddp_freelist))) {
		ddp = list_first_entry(&scm->ddp_freelist,
				       struct csio_dma_buf, list);
		list_del_init(&ddp->list);
		CSIO_DEC_STATS(scm, n_free_ddp);
		return ddp;
	} else
		return NULL;
}

static inline void
csio_put_scsi_ddp(struct csio_scsim *scm, struct csio_dma_buf *ddp)
{
	list_add_tail(&ddp->list, &scm->ddp_freelist);
	CSIO_INC_STATS(scm, n_free_ddp);
}

static inline void
csio_put_scsi_ddp_list(struct csio_scsim *scm, struct list_head *reqlist,
			 int n)
{
	list_splice_tail_init(reqlist, &scm->ddp_freelist);
	scm->stats.n_free_ddp += n;
}

static inline void
csio_scsi_completed(struct csio_ioreq *ioreq, struct list_head *cbfn_q)
{
	csio_post_event(&ioreq->sm, CSIO_SCSIE_COMPLETED);
	if (csio_list_deleted(&ioreq->sm.sm_list))
		list_add_tail(&ioreq->sm.sm_list, cbfn_q);
}

static inline void
csio_scsi_aborted(struct csio_ioreq *ioreq, struct list_head *cbfn_q)
{
	csio_post_event(&ioreq->sm, CSIO_SCSIE_ABORTED);
	list_add_tail(&ioreq->sm.sm_list, cbfn_q);
}

static inline void
csio_scsi_closed(struct csio_ioreq *ioreq, struct list_head *cbfn_q)
{
	csio_post_event(&ioreq->sm, CSIO_SCSIE_CLOSED);
	list_add_tail(&ioreq->sm.sm_list, cbfn_q);
}

static inline void
csio_scsi_drvcleanup(struct csio_ioreq *ioreq)
{
	csio_post_event(&ioreq->sm, CSIO_SCSIE_DRVCLEANUP);
}

/*
                                              
                       
  
                                     
 */
static inline int
csio_scsi_start_io(struct csio_ioreq *ioreq)
{
	csio_post_event(&ioreq->sm, CSIO_SCSIE_START_IO);
	return ioreq->drv_status;
}

/*
                                                            
                       
  
                                     
 */
static inline int
csio_scsi_start_tm(struct csio_ioreq *ioreq)
{
	csio_post_event(&ioreq->sm, CSIO_SCSIE_START_TM);
	return ioreq->drv_status;
}

/*
                                        
                       
  
                                     
 */
static inline int
csio_scsi_abort(struct csio_ioreq *ioreq)
{
	csio_post_event(&ioreq->sm, CSIO_SCSIE_ABORT);
	return ioreq->drv_status;
}

/*
                                        
                       
  
                                     
 */
static inline int
csio_scsi_close(struct csio_ioreq *ioreq)
{
	csio_post_event(&ioreq->sm, CSIO_SCSIE_CLOSE);
	return ioreq->drv_status;
}

void csio_scsi_cleanup_io_q(struct csio_scsim *, struct list_head *);
int csio_scsim_cleanup_io(struct csio_scsim *, bool abort);
int csio_scsim_cleanup_io_lnode(struct csio_scsim *,
					  struct csio_lnode *);
struct csio_ioreq *csio_scsi_cmpl_handler(struct csio_hw *, void *, uint32_t,
					  struct csio_fl_dma_buf *,
					  void *, uint8_t **);
int csio_scsi_qconfig(struct csio_hw *);
int csio_scsim_init(struct csio_scsim *, struct csio_hw *);
void csio_scsim_exit(struct csio_scsim *);

#endif /*                 */
