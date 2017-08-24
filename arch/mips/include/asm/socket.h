/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 1997, 1999, 2000, 2001 Ralf Baechle
 * Copyright (C) 2000, 2001 Silicon Graphics, Inc.
 */
#ifndef _ASM_SOCKET_H
#define _ASM_SOCKET_H

#include <uapi/asm/socket.h>


/*                          
  
                                                           
                                                         
                                        
  
                                            
                                            
                         
                                         
                                             
                                                                         
                                                                  
 */
enum sock_type {
	SOCK_DGRAM	= 1,
	SOCK_STREAM	= 2,
	SOCK_RAW	= 3,
	SOCK_RDM	= 4,
	SOCK_SEQPACKET	= 5,
	SOCK_DCCP	= 6,
	SOCK_PACKET	= 10,
};

#define SOCK_MAX (SOCK_PACKET + 1)
/*                                                   
                                        */
#define SOCK_TYPE_MASK 0xf

/*                                       */
#define SOCK_CLOEXEC	O_CLOEXEC
#define SOCK_NONBLOCK	O_NONBLOCK

#define ARCH_HAS_SOCKET_TYPES 1

#endif /*               */
