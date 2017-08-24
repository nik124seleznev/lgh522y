/*
 *
 * (C) COPYRIGHT ARM Limited. All rights reserved.
 *
 * This program is free software and is provided to you under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation, and any use by you of this program is subject to the terms
 * of such GNU licence.
 *
 * A copy of the licence is included with the program, and can also be obtained
 * from Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 */




#ifndef _LINUX_DMA_BUF_TEST_EXPORTER_H_
#define _LINUX_DMA_BUF_TEST_EXPORTER_H_

#include <linux/types.h>
#include <asm/ioctl.h>

#define DMA_BUF_TE_VER_MAJOR 1
#define DMA_BUF_TE_VER_MINOR 0
#define DMA_BUF_TE_ENQ 0x642d7465
#define DMA_BUF_TE_ACK 0x68692100

struct dma_buf_te_ioctl_version
{
	int op;    /*                                                                                 */
	int major; /*                 */
	int minor; /*                 */
};

struct dma_buf_te_ioctl_alloc
{
	__u64 size; /*                                      */
};

struct dma_buf_te_ioctl_status
{
	/*    */
	int fd; /*                                                                                 */
	/*     */
	int attached_devices; /*                                                       */
	int device_mappings; /*                                                              */
	int cpu_mappings;    /*                                         */
};

struct dma_buf_te_ioctl_set_failing
{
	/*    */
	int fd; /*                                                                                                */

	/*                                                     */
	int fail_attach;
	int fail_map;
	int fail_mmap;
};

struct dma_buf_te_ioctl_fill
{
	int fd;
	unsigned int value;
};

#define DMA_BUF_TE_IOCTL_BASE 'E'
/*                                                                                                          */
#define DMA_BUF_TE_VERSION         _IOR(DMA_BUF_TE_IOCTL_BASE, 0x00, struct dma_buf_te_ioctl_version)
#define DMA_BUF_TE_ALLOC           _IOR(DMA_BUF_TE_IOCTL_BASE, 0x01, struct dma_buf_te_ioctl_alloc)
#define DMA_BUF_TE_QUERY           _IOR(DMA_BUF_TE_IOCTL_BASE, 0x02, struct dma_buf_te_ioctl_status)
#define DMA_BUF_TE_SET_FAILING     _IOW(DMA_BUF_TE_IOCTL_BASE, 0x03, struct dma_buf_te_ioctl_set_failing)
#define DMA_BUF_TE_ALLOC_CONT      _IOR(DMA_BUF_TE_IOCTL_BASE, 0x04, struct dma_buf_te_ioctl_alloc)
#define DMA_BUF_TE_FILL            _IOR(DMA_BUF_TE_IOCTL_BASE, 0x05, struct dma_buf_te_ioctl_fill)

#endif /*                                 */
