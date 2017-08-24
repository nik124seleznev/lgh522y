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
#ifndef _GXIO_USB_H_
#define _GXIO_USB_H_

#include <gxio/common.h>

#include <hv/drv_usb_host_intf.h>
#include <hv/iorpc.h>

/*
  
                                                    
 */

/*
  
                                                                     
               
 */

/*                                                         */
typedef struct {

	/*                                                   */
	int fd;

	/*                                                */
	char *mmio_base;
} gxio_usb_host_context_t;

/*                          
  
                                                                           
                                       
  
                                                                     
                                                
                                                 
                                                                         
               
                                                                   
                            
 */
extern int gxio_usb_host_init(gxio_usb_host_context_t * context, int usb_index,
			      int is_ehci);

/*                       
  
                                                                             
                                                                            
                                                                           
                                                                       
                                            
  
                                                       
                                                                 
                            
 */
extern int gxio_usb_host_destroy(gxio_usb_host_context_t * context);

/*                                                   
  
                                                                            
                                                    
 */
extern void *gxio_usb_host_get_reg_start(gxio_usb_host_context_t * context);

/*                                                  
  
                                                                            
                                                   
 */
extern size_t gxio_usb_host_get_reg_len(gxio_usb_host_context_t * context);

#endif /*              */
