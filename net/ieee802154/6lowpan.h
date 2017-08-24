/*
 * Copyright 2011, Siemens AG
 * written by Alexander Smirnov <alex.bluesman.smirnov@gmail.com>
 */

/*
 * Based on patches from Jon Smirl <jonsmirl@gmail.com>
 * Copyright (c) 2011 Jon Smirl <jonsmirl@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

/* Jon's code is based on 6lowpan implementation for Contiki which is:
 * Copyright (c) 2008, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef __6LOWPAN_H__
#define __6LOWPAN_H__

#define UIP_802154_SHORTADDR_LEN	2  /*                                */
#define UIP_IPH_LEN			40 /*                        */
#define UIP_PROTO_UDP			17 /*                                */
#define UIP_FRAGH_LEN			8  /*                           */

/*
                            
                                                             
 */
#define is_addr_mac_addr_based(a, m) \
	((((a)->s6_addr[8])  == (((m)[0]) ^ 0x02)) &&	\
	 (((a)->s6_addr[9])  == (m)[1]) &&		\
	 (((a)->s6_addr[10]) == (m)[2]) &&		\
	 (((a)->s6_addr[11]) == (m)[3]) &&		\
	 (((a)->s6_addr[12]) == (m)[4]) &&		\
	 (((a)->s6_addr[13]) == (m)[5]) &&		\
	 (((a)->s6_addr[14]) == (m)[6]) &&		\
	 (((a)->s6_addr[15]) == (m)[7]))

/*                             */
#define is_addr_unspecified(a)		\
	((((a)->s6_addr32[0]) == 0) &&	\
	 (((a)->s6_addr32[1]) == 0) &&	\
	 (((a)->s6_addr32[2]) == 0) &&	\
	 (((a)->s6_addr32[3]) == 0))

/*                                 */
#define ipaddr_prefixcmp(addr1, addr2, length) \
	(memcmp(addr1, addr2, length >> 3) == 0)

/*                            */
#define is_addr_link_local(a) (((a)->s6_addr16[0]) == htons(0xFE80))

/*
                                                    
                                                                       
 */
#define lowpan_is_iid_16_bit_compressable(a)	\
	((((a)->s6_addr16[4]) == 0) &&		\
	 (((a)->s6_addr[10]) == 0) &&		\
	 (((a)->s6_addr[11]) == 0xff) &&	\
	 (((a)->s6_addr[12]) == 0xfe) &&	\
	 (((a)->s6_addr[13]) == 0))

/*                   */
#define is_addr_mcast(a) (((a)->s6_addr[0]) == 0xFF)

/*                                                                        */

/*                                                                           */
#define lowpan_is_mcast_addr_compressable(a)	\
	((((a)->s6_addr16[1]) == 0) &&		\
	 (((a)->s6_addr16[2]) == 0) &&		\
	 (((a)->s6_addr16[3]) == 0) &&		\
	 (((a)->s6_addr16[4]) == 0) &&		\
	 (((a)->s6_addr16[5]) == 0) &&		\
	 (((a)->s6_addr16[6]) == 0) &&		\
	 (((a)->s6_addr[14])  == 0) &&		\
	 ((((a)->s6_addr[15]) == 1) || (((a)->s6_addr[15]) == 2)))

/*                               */
#define lowpan_is_mcast_addr_compressable48(a)	\
	((((a)->s6_addr16[1]) == 0) &&		\
	 (((a)->s6_addr16[2]) == 0) &&		\
	 (((a)->s6_addr16[3]) == 0) &&		\
	 (((a)->s6_addr16[4]) == 0) &&		\
	 (((a)->s6_addr[10]) == 0))

/*                          */
#define lowpan_is_mcast_addr_compressable32(a)	\
	((((a)->s6_addr16[1]) == 0) &&		\
	 (((a)->s6_addr16[2]) == 0) &&		\
	 (((a)->s6_addr16[3]) == 0) &&		\
	 (((a)->s6_addr16[4]) == 0) &&		\
	 (((a)->s6_addr16[5]) == 0) &&		\
	 (((a)->s6_addr[12]) == 0))

/*                    */
#define lowpan_is_mcast_addr_compressable8(a)	\
	((((a)->s6_addr[1])  == 2) &&		\
	 (((a)->s6_addr16[1]) == 0) &&		\
	 (((a)->s6_addr16[2]) == 0) &&		\
	 (((a)->s6_addr16[3]) == 0) &&		\
	 (((a)->s6_addr16[4]) == 0) &&		\
	 (((a)->s6_addr16[5]) == 0) &&		\
	 (((a)->s6_addr16[6]) == 0) &&		\
	 (((a)->s6_addr[14]) == 0))

#define lowpan_is_addr_broadcast(a)	\
	((((a)[0]) == 0xFF) &&	\
	 (((a)[1]) == 0xFF) &&	\
	 (((a)[2]) == 0xFF) &&	\
	 (((a)[3]) == 0xFF) &&	\
	 (((a)[4]) == 0xFF) &&	\
	 (((a)[5]) == 0xFF) &&	\
	 (((a)[6]) == 0xFF) &&	\
	 (((a)[7]) == 0xFF))

#define LOWPAN_DISPATCH_IPV6	0x41 /*               */
#define LOWPAN_DISPATCH_HC1	0x42 /*               */
#define LOWPAN_DISPATCH_IPHC	0x60 /*                */
#define LOWPAN_DISPATCH_FRAG1	0xc0 /*          */
#define LOWPAN_DISPATCH_FRAGN	0xe0 /*          */

#define LOWPAN_DISPATCH_MASK	0xf8 /*          */

#define LOWPAN_FRAG_TIMEOUT	(HZ * 60)	/*                 */

#define LOWPAN_FRAG1_HEAD_SIZE	0x4
#define LOWPAN_FRAGN_HEAD_SIZE	0x5

/*
                                   
                        
                                    
                           
  
                                                    
                                
 */
#define LOWPAN_FRAG_SIZE	88

/*
                                                       
                                             
 */
#define LOWPAN_IPHC_TF		0x18

#define LOWPAN_IPHC_FL_C	0x10
#define LOWPAN_IPHC_TC_C	0x08
#define LOWPAN_IPHC_NH_C	0x04
#define LOWPAN_IPHC_TTL_1	0x01
#define LOWPAN_IPHC_TTL_64	0x02
#define LOWPAN_IPHC_TTL_255	0x03
#define LOWPAN_IPHC_TTL_I	0x00


/*                                                       */
#define LOWPAN_IPHC_CID		0x80

#define LOWPAN_IPHC_SAC		0x40
#define LOWPAN_IPHC_SAM_00	0x00
#define LOWPAN_IPHC_SAM_01	0x10
#define LOWPAN_IPHC_SAM_10	0x20
#define LOWPAN_IPHC_SAM		0x30

#define LOWPAN_IPHC_SAM_BIT	4

#define LOWPAN_IPHC_M		0x08
#define LOWPAN_IPHC_DAC		0x04
#define LOWPAN_IPHC_DAM_00	0x00
#define LOWPAN_IPHC_DAM_01	0x01
#define LOWPAN_IPHC_DAM_10	0x02
#define LOWPAN_IPHC_DAM_11	0x03

#define LOWPAN_IPHC_DAM_BIT	0
/*
                                                 
 */
#define LOWPAN_NHC_UDP_MASK		0xF8
#define LOWPAN_NHC_UDP_ID		0xF0
#define LOWPAN_NHC_UDP_CHECKSUMC	0x04
#define LOWPAN_NHC_UDP_CHECKSUMI	0x00

#define LOWPAN_NHC_UDP_4BIT_PORT	0xF0B0
#define LOWPAN_NHC_UDP_4BIT_MASK	0xFFF0
#define LOWPAN_NHC_UDP_8BIT_PORT	0xF000
#define LOWPAN_NHC_UDP_8BIT_MASK	0xFF00

/*                                                                */
#define LOWPAN_NHC_UDP_CS_P_00	0xF0 /*            */
#define LOWPAN_NHC_UDP_CS_P_01	0xF1 /*                     
                                */
#define LOWPAN_NHC_UDP_CS_P_10	0xF2 /*                             
                          */
#define LOWPAN_NHC_UDP_CS_P_11	0xF3 /*                                     */

#endif /*               */