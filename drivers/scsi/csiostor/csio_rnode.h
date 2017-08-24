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

#ifndef __CSIO_RNODE_H__
#define __CSIO_RNODE_H__

#include "csio_defs.h"

/*                     */
enum csio_rn_ev {
	CSIO_RNFE_NONE = (uint32_t)0,			/*      */
	CSIO_RNFE_LOGGED_IN,				/*                
                   
        */
	CSIO_RNFE_PRLI_DONE,				/*                */
	CSIO_RNFE_PLOGI_RECV,				/*                */
	CSIO_RNFE_PRLI_RECV,				/*                */
	CSIO_RNFE_LOGO_RECV,				/*               */
	CSIO_RNFE_PRLO_RECV,				/*               */
	CSIO_RNFE_DOWN,					/*               */
	CSIO_RNFE_CLOSE,				/*             */
	CSIO_RNFE_NAME_MISSING,				/*                   
                         
        */
	CSIO_RNFE_MAX_EVENT,
};

/*             */
struct csio_rnode_stats {
	uint32_t	n_err;		/*       */
	uint32_t	n_err_inval;	/*                   */
	uint32_t	n_err_nomem;	/*             */
	uint32_t	n_evt_unexp;	/*                  */
	uint32_t	n_evt_drop;	/*                  */
	uint32_t	n_evt_fw[PROTO_ERR_IMPL_LOGO + 1];	/*           */
	enum csio_rn_ev	n_evt_sm[CSIO_RNFE_MAX_EVENT];	/*                  */
	uint32_t	n_lun_rst;	/*                    
                          
              
      */
	uint32_t	n_lun_rst_fail;	/*                    
                 
      */
	uint32_t	n_tgt_rst;	/*                         */
	uint32_t	n_tgt_rst_fail;	/*                       
                 
      */
};

/*                        */
#define	CSIO_RNFR_INITIATOR	0x1
#define	CSIO_RNFR_TARGET	0x2
#define CSIO_RNFR_FABRIC	0x4
#define	CSIO_RNFR_NS		0x8
#define CSIO_RNFR_NPORT		0x10

struct csio_rnode {
	struct csio_sm		sm;			/*                
                       
                    
        */
	struct csio_lnode	*lnp;			/*                  
                */
	uint32_t		flowid;			/*             */
	struct list_head	host_cmpl_q;		/*         
                              
                       
        */
	/*                                */
	uint32_t		nport_id;
	uint16_t		fcp_flags;		/*           */
	uint8_t			cur_evt;		/*               */
	uint8_t			prev_evt;		/*                */
	uint32_t		role;			/*               
                      
        */
	struct fcoe_rdev_entry		*rdev_entry;	/*            */
	struct csio_service_parms	rn_sparm;

	/*                         */
	struct fc_rport		*rport;		/*                    */
	uint32_t		supp_classes;	/*                      */
	uint32_t		maxframe_size;	/*                */
	uint32_t		scsi_id;	/*                         */

	struct csio_rnode_stats	stats;		/*                    */
};

#define csio_rn_flowid(rn)			((rn)->flowid)
#define csio_rn_wwpn(rn)			((rn)->rn_sparm.wwpn)
#define csio_rn_wwnn(rn)			((rn)->rn_sparm.wwnn)
#define csio_rnode_to_lnode(rn)			((rn)->lnp)

int csio_is_rnode_ready(struct csio_rnode *rn);
void csio_rnode_state_to_str(struct csio_rnode *rn, int8_t *str);

struct csio_rnode *csio_rnode_lookup_portid(struct csio_lnode *, uint32_t);
struct csio_rnode *csio_confirm_rnode(struct csio_lnode *,
					  uint32_t, struct fcoe_rdev_entry *);

void csio_rnode_fwevt_handler(struct csio_rnode *rn, uint8_t fwevt);

void csio_put_rnode(struct csio_lnode *ln, struct csio_rnode *rn);

void csio_reg_rnode(struct csio_rnode *);
void csio_unreg_rnode(struct csio_rnode *);

void csio_rnode_devloss_handler(struct csio_rnode *);

#endif /*                         */
