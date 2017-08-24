/*
 * Copyright 2012 Tilera Corporation. All Rights Reserved.
 *
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public License
 *   as published by the Free Software Foundation, version 2.
 *
 *   This program is distributed in the hope that it will be useful, but
 *   WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, GOOD TITLE or
 *   NON INFRINGEMENT.  See the GNU General Public License for
 *   more details.
 */

/* 
                                                 
 */

#ifndef _SYS_HV_DRV_USB_HOST_INTF_H
#define _SYS_HV_DRV_USB_HOST_INTF_H

#include <arch/usb_host.h>


/*                                            */
#define HV_USB_HOST_MMIO_OFFSET_EHCI ((uint64_t) USB_HOST_HCCAPBASE_REG)

/*                                            */
#define HV_USB_HOST_MMIO_OFFSET_OHCI ((uint64_t) USB_HOST_OHCD_HC_REVISION_REG)

/*                                                                      
                        */
#define HV_USB_HOST_MMIO_SIZE ((uint64_t) 0x1000)

/*                                                                */
#define HV_USB_HOST_NUM_SVC_DOM 1


#endif /*                             */
