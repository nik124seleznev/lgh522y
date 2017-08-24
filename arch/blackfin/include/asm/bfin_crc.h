/*
 * bfin_crc.h - interface to Blackfin CRC controllers
 *
 * Copyright 2012 Analog Devices Inc.
 *
 * Licensed under the GPL-2 or later.
 */

#ifndef __BFIN_CRC_H__
#define __BFIN_CRC_H__

/*                                                                      */
struct crc_info {
	/*                    */
	unsigned char *in_addr;
	/*                     */
	unsigned char *out_addr;
	/*                       */
	unsigned long datasize;
	union {
	/*                                          */
	unsigned long crc_compare;
	/*                                  */
	unsigned long val_verify;
	/*               */
	unsigned long val_fill;
	};
	/*                                         */
	unsigned long crc_poly;
	union {
	/*                                    */
	unsigned long crc_result;
	/*                                            */
	unsigned long pos_verify;
	};
	/*                  */
	unsigned int bitmirr:1;
	unsigned int bytmirr:1;
	unsigned int w16swp:1;
	unsigned int fdsel:1;
	unsigned int rsltmirr:1;
	unsigned int polymirr:1;
	unsigned int cmpmirr:1;
};

/*                     */
#define CRC_IOC_MAGIC		'C'
#define CRC_IOC_CALC_CRC	_IOWR('C', 0x01, unsigned int)
#define CRC_IOC_MEMCPY_CRC	_IOWR('C', 0x02, unsigned int)
#define CRC_IOC_VERIFY_VAL	_IOWR('C', 0x03, unsigned int)
#define CRC_IOC_FILL_VAL	_IOWR('C', 0x04, unsigned int)


#ifdef __KERNEL__

#include <linux/types.h>
#include <linux/spinlock.h>
#include <linux/miscdevice.h>

struct crc_register {
	u32 control;
	u32 datacnt;
	u32 datacntrld;
	u32 __pad_1[2];
	u32 compare;
	u32 fillval;
	u32 datafifo;
	u32 intren;
	u32 intrenset;
	u32 intrenclr;
	u32 poly;
	u32 __pad_2[4];
	u32 status;
	u32 datacntcap;
	u32 __pad_3;
	u32 result;
	u32 curresult;
	u32 __pad_4[3];
	u32 revid;
};

/*                  */
#define CMPERR			0x00000002	/*               */
#define DCNTEXP			0x00000010	/*                          */
#define IBR			0x00010000	/*                    */
#define OBR			0x00020000	/*                     */
#define IRR			0x00040000	/*                        */
#define LUTDONE			0x00080000	/*                               */
#define FSTAT			0x00700000	/*             */
#define MAX_FIFO		4		/*               */

/*                   */
#define BLKEN			0x00000001	/*              */
#define OPMODE			0x000000F0	/*                */
#define OPMODE_OFFSET		4		/*                           */
#define MODE_DMACPY_CRC		1		/*                             */
#define MODE_DATA_FILL		2		/*               */
#define MODE_CALC_CRC		3		/*                             */
#define MODE_DATA_VERIFY	4		/*                 */
#define AUTOCLRZ		0x00000100	/*                    */
#define AUTOCLRF		0x00000200	/*                   */
#define OBRSTALL		0x00001000	/*                              */
#define IRRSTALL		0x00002000	/*                                 */
#define BITMIRR			0x00010000	/*                                                   */
#define BITMIRR_OFFSET		16		/*                    */
#define BYTMIRR			0x00020000	/*                                   */
#define BYTMIRR_OFFSET		17		/*                     */
#define W16SWP			0x00040000	/*                                                          */
#define W16SWP_OFFSET		18		/*                           */
#define FDSEL			0x00080000	/*                                              */
#define FDSEL_OFFSET		19		/*                    */
#define RSLTMIRR		0x00100000	/*                                    */
#define RSLTMIRR_OFFSET		20		/*                           */
#define POLYMIRR		0x00200000	/*                                */
#define POLYMIRR_OFFSET		21		/*                         */
#define CMPMIRR			0x00400000	/*                                   */
#define CMPMIRR_OFFSET		22		/*                            */

/*                  */
#define CMPERRI 		0x02		/*                */
#define DCNTEXPI 		0x10		/*                 */

#endif

#endif
