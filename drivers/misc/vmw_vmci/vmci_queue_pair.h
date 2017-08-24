/*
 * VMware VMCI Driver
 *
 * Copyright (C) 2012 VMware, Inc. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation version 2 and no later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 */

#ifndef _VMCI_QUEUE_PAIR_H_
#define _VMCI_QUEUE_PAIR_H_

#include <linux/vmw_vmci_defs.h>
#include <linux/types.h>

#include "vmci_context.h"

/*                                                  */
typedef int (*vmci_event_release_cb) (void *client_data);

/*                        */
struct ppn_set {
	u64 num_produce_pages;
	u64 num_consume_pages;
	u32 *produce_ppns;
	u32 *consume_ppns;
	bool initialized;
};

/*                         */
struct vmci_qp_alloc_info {
	struct vmci_handle handle;
	u32 peer;
	u32 flags;
	u64 produce_size;
	u64 consume_size;
	u64 ppn_va;	/*                              */
	u64 num_ppns;
	s32 result;
	u32 version;
};

/*                         */
struct vmci_qp_set_va_info {
	struct vmci_handle handle;
	u64 va;		/*                              */
	u64 num_ppns;
	u32 version;
	s32 result;
};

/*
                                                        
                                                                       
                                                                  
                                                                      
                                                                     
                                      
                                                                         
                                                                    
                                      
  
                                                                
                                                                     
                                                               
                                                              
                                                   
  
                                                                    
                                                                    
                                                                
  
                                               
                                                                     
                                                                      
                                 
 */

/*                            */
struct vmci_qp_page_file_info {
	struct vmci_handle handle;
	u64 produce_page_file;	  /*          */
	u64 consume_page_file;	  /*          */
	u64 produce_page_file_size;  /*                              */
	u64 consume_page_file_size;  /*                              */
	s32 result;
	u32 version;	/*           */
	u64 produce_va;	/*                             */
	u64 consume_va;	/*                             */
};

/*                            */
struct vmci_qp_dtch_info {
	struct vmci_handle handle;
	s32 result;
	u32 _pad;
};

/*
                                                                           
                                                                      
                                                                     
                                                                   
                                                   
 */
struct vmci_qp_page_store {
	/*                                            */
	u64 pages;
	/*                                                     */
	u32 len;
};

/*
                                                         
                                                                            
                                                                           
                                                                         
                          
  
                                                                              
                                                                            
                                                                        
                                                       
 */
struct vmci_queue {
	struct vmci_queue_header *q_header;
	struct vmci_queue_header *saved_header;
	struct vmci_queue_kern_if *kernel_if;
};

/*
                                                              
                              
          
                                                         
 */
static inline bool
VMCI_QP_PAGESTORE_IS_WELLFORMED(struct vmci_qp_page_store *page_store)
{
	return page_store->len >= 2;
}

/*
                                                    
                                 
 */
static inline bool vmci_can_block(u32 flags)
{
	return !(flags & VMCI_QPFLAG_NONBLOCK);
}

/*
                                                       
               
 */
static inline bool vmci_qp_pinned(u32 flags)
{
	return flags & VMCI_QPFLAG_PINNED;
}

void vmci_qp_broker_exit(void);
int vmci_qp_broker_alloc(struct vmci_handle handle, u32 peer,
			 u32 flags, u32 priv_flags,
			 u64 produce_size, u64 consume_size,
			 struct vmci_qp_page_store *page_store,
			 struct vmci_ctx *context);
int vmci_qp_broker_set_page_store(struct vmci_handle handle,
				  u64 produce_uva, u64 consume_uva,
				  struct vmci_ctx *context);
int vmci_qp_broker_detach(struct vmci_handle handle, struct vmci_ctx *context);

void vmci_qp_guest_endpoints_exit(void);

int vmci_qp_alloc(struct vmci_handle *handle,
		  struct vmci_queue **produce_q, u64 produce_size,
		  struct vmci_queue **consume_q, u64 consume_size,
		  u32 peer, u32 flags, u32 priv_flags,
		  bool guest_endpoint, vmci_event_release_cb wakeup_cb,
		  void *client_data);
int vmci_qp_broker_map(struct vmci_handle handle,
		       struct vmci_ctx *context, u64 guest_mem);
int vmci_qp_broker_unmap(struct vmci_handle handle,
			 struct vmci_ctx *context, u32 gid);

#endif /*                     */
