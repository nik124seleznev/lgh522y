#ifndef _LINUX_VIRTIO_BLK_H
#define _LINUX_VIRTIO_BLK_H
/* This header is BSD licensed so anyone can use the definitions to implement
 * compatible drivers/servers.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of IBM nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL IBM OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE. */
#include <linux/types.h>
#include <linux/virtio_ids.h>
#include <linux/virtio_config.h>

/*              */
#define VIRTIO_BLK_F_BARRIER	0	/*                             */
#define VIRTIO_BLK_F_SIZE_MAX	1	/*                                */
#define VIRTIO_BLK_F_SEG_MAX	2	/*                                 */
#define VIRTIO_BLK_F_GEOMETRY	4	/*                            */
#define VIRTIO_BLK_F_RO		5	/*                   */
#define VIRTIO_BLK_F_BLK_SIZE	6	/*                                */
#define VIRTIO_BLK_F_SCSI	7	/*                                */
#define VIRTIO_BLK_F_WCE	9	/*                                    */
#define VIRTIO_BLK_F_TOPOLOGY	10	/*                                   */
#define VIRTIO_BLK_F_CONFIG_WCE	11	/*                                    */

#ifndef __KERNEL__
/*                                             */
#define VIRTIO_BLK_F_FLUSH VIRTIO_BLK_F_WCE
#endif

#define VIRTIO_BLK_ID_BYTES	20	/*                  */

struct virtio_blk_config {
	/*                                     */
	__u64 capacity;
	/*                                                     */
	__u32 size_max;
	/*                                                          */
	__u32 seg_max;
	/*                                                */
	struct virtio_blk_geometry {
		__u16 cylinders;
		__u8 heads;
		__u8 sectors;
	} geometry;

	/*                                                 */
	__u32 blk_size;

	/*                                                          */
	/*                                                */
	__u8 physical_block_exp;
	/*                                     */
	__u8 alignment_offset;
	/*                                                                 */
	__u16 min_io_size;
	/*                                               */
	__u32 opt_io_size;

	/*                                             */
	__u8 wce;
} __attribute__((packed));

/*
                
  
                                                                         
  
         
                                                                   
                                                                      
                                                         
 */

/*                             */
#define VIRTIO_BLK_T_IN		0
#define VIRTIO_BLK_T_OUT	1

/*                                                                 */
#define VIRTIO_BLK_T_SCSI_CMD	2

/*                     */
#define VIRTIO_BLK_T_FLUSH	4

/*                       */
#define VIRTIO_BLK_T_GET_ID    8

/*                         */
#define VIRTIO_BLK_T_BARRIER	0x80000000

/*                                                            */
struct virtio_blk_outhdr {
	/*               */
	__u32 type;
	/*              */
	__u32 ioprio;
	/*                              */
	__u64 sector;
};

struct virtio_scsi_inhdr {
	__u32 errors;
	__u32 data_len;
	__u32 sense_len;
	__u32 residual;
};

/*                                                              */
#define VIRTIO_BLK_S_OK		0
#define VIRTIO_BLK_S_IOERR	1
#define VIRTIO_BLK_S_UNSUPP	2
#endif /*                     */