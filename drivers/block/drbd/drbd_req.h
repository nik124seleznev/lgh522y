/*
   drbd_req.h

   This file is part of DRBD by Philipp Reisner and Lars Ellenberg.

   Copyright (C) 2006-2008, LINBIT Information Technologies GmbH.
   Copyright (C) 2006-2008, Lars Ellenberg <lars.ellenberg@linbit.com>.
   Copyright (C) 2006-2008, Philipp Reisner <philipp.reisner@linbit.com>.

   DRBD is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   DRBD is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with drbd; see the file COPYING.  If not, write to
   the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef _DRBD_REQ_H
#define _DRBD_REQ_H

#include <linux/module.h>

#include <linux/slab.h>
#include <linux/drbd.h>
#include "drbd_int.h"
#include "drbd_wrappers.h"

/*                                                                        
                                                           
                                                           
                                   */

/*
                                                                                 
                                                                         
  
                                                                 
                       
  
                       
                                              
                                    
                           
  
                                                                         
                                         
  
                                                        
                                 
  
                                                      
                                                    
                                           
                                                        
                                 
  
                                 
                                               
                     
                                                                               
                                 
                                                                          
                               
                                                           
                                          
                                                                          
                           
                                                         
                                  
 */

enum drbd_req_event {
	CREATED,
	TO_BE_SENT,
	TO_BE_SUBMITTED,

	/*                                  
                                        
                */
	QUEUE_FOR_NET_WRITE,
	QUEUE_FOR_NET_READ,
	QUEUE_FOR_SEND_OOS,

	/*                                       
                                                   
                                        
   
                                                                        
                                                                     */
	QUEUE_AS_DRBD_BARRIER,

	SEND_CANCELED,
	SEND_FAILED,
	HANDED_OVER_TO_NETWORK,
	OOS_HANDED_TO_NETWORK,
	CONNECTION_LOST_WHILE_PENDING,
	READ_RETRY_REMOTE_CANCELED,
	RECV_ACKED_BY_PEER,
	WRITE_ACKED_BY_PEER,
	WRITE_ACKED_BY_PEER_AND_SIS, /*                 */
	CONFLICT_RESOLVED,
	POSTPONE_WRITE,
	NEG_ACKED,
	BARRIER_ACKED, /*                     */
	DATA_RECEIVED, /*               */

	READ_COMPLETED_WITH_ERROR,
	READ_AHEAD_COMPLETED_WITH_ERROR,
	WRITE_COMPLETED_WITH_ERROR,
	ABORT_DISK_IO,
	COMPLETED_OK,
	RESEND,
	FAIL_FROZEN_DISK_IO,
	RESTART_FROZEN_DISK_IO,
	NOTHING,
};

/*                                                                            
                                                                              
                                                                           
                                                         
 */
enum drbd_req_state_bits {
	/*     
                           
                         
                                                                     
                      
                              
                                     
                                       
  */
	__RQ_LOCAL_PENDING,
	__RQ_LOCAL_COMPLETED,
	__RQ_LOCAL_OK,
	__RQ_LOCAL_ABORTED,

	/*      
                              
                     
                                      
                                                        
                
                                              
                                             
                                                               
                           
                                                        
                                                          
                               
                                        
                                              
                                          
                                               
                               
                                     
                               
  */

	/*                                                               
                                                              
                                                  
                                                                       
                                                 */
	__RQ_NET_PENDING,

	/*                                                                 
                                                                        
                                                                     
                                                                       
                     */
	__RQ_NET_QUEUED,

	/*                                                        
   
                                                                  
                                      
                                                         
                                                               
                                                              */
	__RQ_NET_SENT,

	/*                                                                
                                                                      */
	__RQ_NET_DONE,

	/*                                                           
                                         
  */
	__RQ_NET_OK,

	/*                                               */
	__RQ_NET_SIS,

	/*                                         */
	__RQ_NET_MAX,

	/*                                            */
	__RQ_WRITE,

	/*                                                       */
	__RQ_IN_ACT_LOG,

	/*                               */
	__RQ_POSTPONED,

	/*                           
                                             */
	__RQ_COMPLETION_SUSP,

	/*                                        */
	__RQ_EXP_RECEIVE_ACK,

	/*                                      */
	__RQ_EXP_WRITE_ACK,

	/*                                                  */
	__RQ_EXP_BARR_ACK,
};

#define RQ_LOCAL_PENDING   (1UL << __RQ_LOCAL_PENDING)
#define RQ_LOCAL_COMPLETED (1UL << __RQ_LOCAL_COMPLETED)
#define RQ_LOCAL_OK        (1UL << __RQ_LOCAL_OK)
#define RQ_LOCAL_ABORTED   (1UL << __RQ_LOCAL_ABORTED)

#define RQ_LOCAL_MASK      ((RQ_LOCAL_ABORTED << 1)-1)

#define RQ_NET_PENDING     (1UL << __RQ_NET_PENDING)
#define RQ_NET_QUEUED      (1UL << __RQ_NET_QUEUED)
#define RQ_NET_SENT        (1UL << __RQ_NET_SENT)
#define RQ_NET_DONE        (1UL << __RQ_NET_DONE)
#define RQ_NET_OK          (1UL << __RQ_NET_OK)
#define RQ_NET_SIS         (1UL << __RQ_NET_SIS)

/*       */
#define RQ_NET_MASK        (((1UL << __RQ_NET_MAX)-1) & ~RQ_LOCAL_MASK)

#define RQ_WRITE           (1UL << __RQ_WRITE)
#define RQ_IN_ACT_LOG      (1UL << __RQ_IN_ACT_LOG)
#define RQ_POSTPONED	   (1UL << __RQ_POSTPONED)
#define RQ_COMPLETION_SUSP (1UL << __RQ_COMPLETION_SUSP)
#define RQ_EXP_RECEIVE_ACK (1UL << __RQ_EXP_RECEIVE_ACK)
#define RQ_EXP_WRITE_ACK   (1UL << __RQ_EXP_WRITE_ACK)
#define RQ_EXP_BARR_ACK    (1UL << __RQ_EXP_BARR_ACK)

/*                                                                             
                                        */
#define MR_WRITE       1
#define MR_READ        2

static inline void drbd_req_make_private_bio(struct drbd_request *req, struct bio *bio_src)
{
	struct bio *bio;
	bio = bio_clone(bio_src, GFP_NOIO); /*                   */

	req->private_bio = bio;

	bio->bi_private  = req;
	bio->bi_end_io   = drbd_request_endio;
	bio->bi_next     = NULL;
}

/*                                           
                                                  
                                                         */
struct bio_and_error {
	struct bio *bio;
	int error;
};

extern void start_new_tl_epoch(struct drbd_tconn *tconn);
extern void drbd_req_destroy(struct kref *kref);
extern void _req_may_be_done(struct drbd_request *req,
		struct bio_and_error *m);
extern int __req_mod(struct drbd_request *req, enum drbd_req_event what,
		struct bio_and_error *m);
extern void complete_master_bio(struct drbd_conf *mdev,
		struct bio_and_error *m);
extern void request_timer_fn(unsigned long data);
extern void tl_restart(struct drbd_tconn *tconn, enum drbd_req_event what);
extern void _tl_restart(struct drbd_tconn *tconn, enum drbd_req_event what);

/*                        */
extern void drbd_restart_request(struct drbd_request *req);

/*                                                                      
                                                                 */
static inline int _req_mod(struct drbd_request *req, enum drbd_req_event what)
{
	struct drbd_conf *mdev = req->w.mdev;
	struct bio_and_error m;
	int rv;

	/*                                                            */
	rv = __req_mod(req, what, &m);
	if (m.bio)
		complete_master_bio(mdev, &m);

	return rv;
}

/*                                                     
                                                               
                                                               
                           */
static inline int req_mod(struct drbd_request *req,
		enum drbd_req_event what)
{
	unsigned long flags;
	struct drbd_conf *mdev = req->w.mdev;
	struct bio_and_error m;
	int rv;

	spin_lock_irqsave(&mdev->tconn->req_lock, flags);
	rv = __req_mod(req, what, &m);
	spin_unlock_irqrestore(&mdev->tconn->req_lock, flags);

	if (m.bio)
		complete_master_bio(mdev, &m);

	return rv;
}

static inline bool drbd_should_do_remote(union drbd_dev_state s)
{
	return s.pdsk == D_UP_TO_DATE ||
		(s.pdsk >= D_INCONSISTENT &&
		 s.conn >= C_WF_BITMAP_T &&
		 s.conn < C_AHEAD);
	/*                                                                   
                                                                        
            */
}
static inline bool drbd_should_send_out_of_sync(union drbd_dev_state s)
{
	return s.conn == C_AHEAD || s.conn == C_WF_BITMAP_S;
	/*                                                                        
                                                     */
}

#endif
