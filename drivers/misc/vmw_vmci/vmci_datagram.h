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

#ifndef _VMCI_DATAGRAM_H_
#define _VMCI_DATAGRAM_H_

#include <linux/types.h>
#include <linux/list.h>

#include "vmci_context.h"

#define VMCI_MAX_DELAYED_DG_HOST_QUEUE_SIZE 256

/*
                                                                                
                                                               
                                                                     
                                                                   
                                                                 
                                            
 */
struct vmci_datagram_queue_entry {
	struct list_head list_item;	/*              */
	size_t dg_size;		/*                   */
	struct vmci_datagram *dg;	/*                   */
};

/*                          */
struct vmci_datagram_snd_rcv_info {
	u64 addr;
	u32 len;
	s32 result;
};

/*                                  */
int vmci_datagram_dispatch(u32 context_id, struct vmci_datagram *dg,
			   bool from_guest);
int vmci_datagram_invoke_guest_handler(struct vmci_datagram *dg);

#endif /*                   */
