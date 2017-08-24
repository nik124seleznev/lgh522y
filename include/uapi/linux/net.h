/*
 * NET		An implementation of the SOCKET network access protocol.
 *		This is the master header file for the Linux NET layer,
 *		or, in plain English: the networking handling part of the
 *		kernel.
 *
 * Version:	@(#)net.h	1.0.3	05/25/93
 *
 * Authors:	Orest Zborowski, <obz@Kodak.COM>
 *		Ross Biro
 *		Fred N. van Kempen, <waltje@uWalt.NL.Mugnet.ORG>
 *
 *		This program is free software; you can redistribute it and/or
 *		modify it under the terms of the GNU General Public License
 *		as published by the Free Software Foundation; either version
 *		2 of the License, or (at your option) any later version.
 */
#ifndef _UAPI_LINUX_NET_H
#define _UAPI_LINUX_NET_H

#include <linux/socket.h>
#include <asm/socket.h>

#define NPROTO		AF_MAX

#define SYS_SOCKET	1		/*                */
#define SYS_BIND	2		/*               */
#define SYS_CONNECT	3		/*                 */
#define SYS_LISTEN	4		/*                */
#define SYS_ACCEPT	5		/*                */
#define SYS_GETSOCKNAME	6		/*                     */
#define SYS_GETPEERNAME	7		/*                     */
#define SYS_SOCKETPAIR	8		/*                    */
#define SYS_SEND	9		/*               */
#define SYS_RECV	10		/*               */
#define SYS_SENDTO	11		/*                */
#define SYS_RECVFROM	12		/*                  */
#define SYS_SHUTDOWN	13		/*                  */
#define SYS_SETSOCKOPT	14		/*                    */
#define SYS_GETSOCKOPT	15		/*                    */
#define SYS_SENDMSG	16		/*                 */
#define SYS_RECVMSG	17		/*                 */
#define SYS_ACCEPT4	18		/*                 */
#define SYS_RECVMMSG	19		/*                  */
#define SYS_SENDMMSG	20		/*                  */

typedef enum {
	SS_FREE = 0,			/*                */
	SS_UNCONNECTED,			/*                           */
	SS_CONNECTING,			/*                          */
	SS_CONNECTED,			/*                      */
	SS_DISCONNECTING		/*                             */
} socket_state;

#define __SO_ACCEPTCON	(1 << 16)	/*                     */

#endif /*                   */
