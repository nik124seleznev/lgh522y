/*
 * INET		An implementation of the TCP/IP protocol suite for the LINUX
 *		operating system.  INET is implemented using the  BSD Socket
 *		interface as the means of communication with the user level.
 *
 *		Definitions for the UDP protocol.
 *
 * Version:	@(#)udp.h	1.0.2	04/28/93
 *
 * Author:	Fred N. van Kempen, <waltje@uWalt.NL.Mugnet.ORG>
 *
 *		This program is free software; you can redistribute it and/or
 *		modify it under the terms of the GNU General Public License
 *		as published by the Free Software Foundation; either version
 *		2 of the License, or (at your option) any later version.
 */
#ifndef _UAPI_LINUX_UDP_H
#define _UAPI_LINUX_UDP_H

#include <linux/types.h>

struct udphdr {
	__be16	source;
	__be16	dest;
	__be16	len;
	__sum16	check;
};

/*                    */
#define UDP_CORK	1	/*                                        */
#define UDP_ENCAP	100	/*                                               */

/*                         */
#define UDP_ENCAP_ESPINUDP_NON_IKE	1 /*                                  */
#define UDP_ENCAP_ESPINUDP	2 /*                                */
#define UDP_ENCAP_L2TPINUDP	3 /*         */


#endif /*                   */
