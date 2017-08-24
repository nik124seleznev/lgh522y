/*
 *   fs/cifs/cifsacl.h
 *
 *   Copyright (c) International Business Machines  Corp., 2007
 *   Author(s): Steve French (sfrench@us.ibm.com)
 *
 *   This library is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published
 *   by the Free Software Foundation; either version 2.1 of the License, or
 *   (at your option) any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 *   the GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with this library; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef _CIFSACL_H
#define _CIFSACL_H


#define NUM_AUTHS (6)	/*                            */
#define SID_MAX_SUB_AUTHORITIES (15) /*                                    */

#define READ_BIT        0x4
#define WRITE_BIT       0x2
#define EXEC_BIT        0x1

#define UBITSHIFT	6
#define GBITSHIFT	3

#define ACCESS_ALLOWED	0
#define ACCESS_DENIED	1

#define SIDOWNER 1
#define SIDGROUP 2

/*
                                                                       
                           
 */
#define DEFAULT_SEC_DESC_LEN (sizeof(struct cifs_ntsd) + \
			      sizeof(struct cifs_acl) + \
			      (sizeof(struct cifs_ace) * 3))

/*
                                                    
  
                                                 
  
                              
                               
  
                                                                              
  
                                                                            
                                                
  
                                                                       
 */
#define SID_STRING_BASE_SIZE (2 + 3 + 15 + 1)
#define SID_STRING_SUBAUTH_SIZE (11) /*                                 */

struct cifs_ntsd {
	__le16 revision; /*                */
	__le16 type;
	__le32 osidoffset;
	__le32 gsidoffset;
	__le32 sacloffset;
	__le32 dacloffset;
} __attribute__((packed));

struct cifs_sid {
	__u8 revision; /*                */
	__u8 num_subauth;
	__u8 authority[NUM_AUTHS];
	__le32 sub_auth[SID_MAX_SUB_AUTHORITIES]; /*                       */
} __attribute__((packed));

/*                                                */
#define CIFS_SID_BASE_SIZE (1 + 1 + NUM_AUTHS)

struct cifs_acl {
	__le16 revision; /*                */
	__le16 size;
	__le32 num_aces;
} __attribute__((packed));

struct cifs_ace {
	__u8 type;
	__u8 flags;
	__le16 size;
	__le32 access_req;
	struct cifs_sid sid; /*                                               */
} __attribute__((packed));

#endif /*            */
