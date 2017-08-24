/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 1997, 1999, 2000, 2001 Ralf Baechle
 * Copyright (C) 2000, 2001 Silicon Graphics, Inc.
 */
#ifndef _UAPI_ASM_SOCKET_H
#define _UAPI_ASM_SOCKET_H

#include <asm/sockios.h>

/*
                    
  
                                                                     
 */
#define SOL_SOCKET	0xffff

#define SO_DEBUG	0x0001	/*                                */
#define SO_REUSEADDR	0x0004	/*                                  */
#define SO_KEEPALIVE	0x0008	/*                                
                               */
#define SO_DONTROUTE	0x0010	/*                          */
#define SO_BROADCAST	0x0020	/*                      
                           */
#define SO_LINGER	0x0080	/*                             
                                         */
#define SO_OOBINLINE 0x0100	/*                                    */
#define SO_REUSEPORT 0x0200	/*                                      */

#define SO_TYPE		0x1008	/*                                */
#define SO_STYLE	SO_TYPE /*         */
#define SO_ERROR	0x1007	/*                            */
#define SO_SNDBUF	0x1001	/*                   */
#define SO_RCVBUF	0x1002	/*                 */
#define SO_SNDLOWAT	0x1003	/*                     */
#define SO_RCVLOWAT	0x1004	/*                        */
#define SO_SNDTIMEO	0x1005	/*              */
#define SO_RCVTIMEO	0x1006	/*                 */
#define SO_ACCEPTCONN	0x1009
#define SO_PROTOCOL	0x1028	/*               */
#define SO_DOMAIN	0x1029	/*                      */

/*                                                      */
#define SO_NO_CHECK	11
#define SO_PRIORITY	12
#define SO_BSDCOMPAT	14

#define SO_PASSCRED	17
#define SO_PEERCRED	18

/*                                                                */
#define SO_SECURITY_AUTHENTICATION		22
#define SO_SECURITY_ENCRYPTION_TRANSPORT	23
#define SO_SECURITY_ENCRYPTION_NETWORK		24

#define SO_BINDTODEVICE		25

/*                  */
#define SO_ATTACH_FILTER	26
#define SO_DETACH_FILTER	27
#define SO_GET_FILTER		SO_ATTACH_FILTER

#define SO_PEERNAME		28
#define SO_TIMESTAMP		29
#define SCM_TIMESTAMP		SO_TIMESTAMP

#define SO_PEERSEC		30
#define SO_SNDBUFFORCE		31
#define SO_RCVBUFFORCE		33
#define SO_PASSSEC		34
#define SO_TIMESTAMPNS		35
#define SCM_TIMESTAMPNS		SO_TIMESTAMPNS

#define SO_MARK			36

#define SO_TIMESTAMPING		37
#define SCM_TIMESTAMPING	SO_TIMESTAMPING

#define SO_RXQ_OVFL		40

#define SO_WIFI_STATUS		41
#define SCM_WIFI_STATUS		SO_WIFI_STATUS
#define SO_PEEK_OFF		42

/*                                                              */
#define SO_NOFCS		43

#define SO_LOCK_FILTER		44

#define SO_SELECT_ERR_QUEUE	45

#endif /*                    */
