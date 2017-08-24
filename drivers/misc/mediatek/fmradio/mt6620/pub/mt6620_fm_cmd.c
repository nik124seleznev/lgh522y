#include <linux/kernel.h>
#include <linux/types.h>

#include "fm_typedef.h"
#include "fm_dbg.h"
#include "fm_err.h"
#include "fm_rds.h"
#include "fm_link.h"

#include "mt6620_fm_reg.h"
/*                             */
#include "mt6620_fm.h"
#include "mt6620_fm_cmd.h"
#include "mt6620_fm_cust_cfg.h"

static fm_s32 fm_bop_write(fm_u8 addr, fm_u16 value, fm_u8 * buf, fm_s32 size)
{
	if (size < (FM_WRITE_BASIC_OP_SIZE + 2)) {
		return (-1);
	}

	if (buf == NULL) {
		return (-2);
	}

	buf[0] = FM_WRITE_BASIC_OP;
	buf[1] = FM_WRITE_BASIC_OP_SIZE;
	buf[2] = addr;
	buf[3] = (fm_u8) ((value) & 0x00FF);
	buf[4] = (fm_u8) ((value >> 8) & 0x00FF);

	WCN_DBG(FM_DBG | CHIP, "%02x %02x %02x %02x %02x\n", buf[0], buf[1], buf[2], buf[3],
		buf[4]);

	return (FM_WRITE_BASIC_OP_SIZE + 2);
}

static fm_s32 fm_bop_udelay(fm_u32 value, fm_u8 *buf, fm_s32 size)
{
	if (size < (FM_UDELAY_BASIC_OP_SIZE + 2)) {
		return (-1);
	}

	if (buf == NULL) {
		return (-2);
	}

	buf[0] = FM_UDELAY_BASIC_OP;
	buf[1] = FM_UDELAY_BASIC_OP_SIZE;
	buf[2] = (fm_u8) ((value) & 0x000000FF);
	buf[3] = (fm_u8) ((value >> 8) & 0x000000FF);
	buf[4] = (fm_u8) ((value >> 16) & 0x000000FF);
	buf[5] = (fm_u8) ((value >> 24) & 0x000000FF);

	WCN_DBG(FM_DBG | CHIP, "%02x %02x %02x %02x %02x %02x\n", buf[0], buf[1], buf[2], buf[3],
		buf[4], buf[5]);

	return (FM_UDELAY_BASIC_OP_SIZE + 2);
}

static fm_s32 fm_bop_rd_until(fm_u8 addr, fm_u16 mask, fm_u16 value, fm_u8 *buf, fm_s32 size)
{
	if (size < (FM_RD_UNTIL_BASIC_OP_SIZE + 2)) {
		return (-1);
	}

	if (buf == NULL) {
		return (-2);
	}

	buf[0] = FM_RD_UNTIL_BASIC_OP;
	buf[1] = FM_RD_UNTIL_BASIC_OP_SIZE;
	buf[2] = addr;
	buf[3] = (fm_u8) ((mask) & 0x00FF);
	buf[4] = (fm_u8) ((mask >> 8) & 0x00FF);
	buf[5] = (fm_u8) ((value) & 0x00FF);
	buf[6] = (fm_u8) ((value >> 8) & 0x00FF);

	WCN_DBG(FM_DBG | CHIP, "%02x %02x %02x %02x %02x %02x %02x\n", buf[0], buf[1], buf[2],
		buf[3], buf[4], buf[5], buf[6]);

	return (FM_RD_UNTIL_BASIC_OP_SIZE + 2);
}

static fm_s32 fm_bop_modify(fm_u8 addr, fm_u16 mask_and, fm_u16 mask_or, fm_u8 *buf, fm_s32 size)
{
	if (size < (FM_MODIFY_BASIC_OP_SIZE + 2)) {
		return (-1);
	}

	if (buf == NULL) {
		return (-2);
	}

	buf[0] = FM_MODIFY_BASIC_OP;
	buf[1] = FM_MODIFY_BASIC_OP_SIZE;
	buf[2] = addr;
	buf[3] = (fm_u8) ((mask_and) & 0x00FF);
	buf[4] = (fm_u8) ((mask_and >> 8) & 0x00FF);
	buf[5] = (fm_u8) ((mask_or) & 0x00FF);
	buf[6] = (fm_u8) ((mask_or >> 8) & 0x00FF);

	WCN_DBG(FM_DBG | CHIP, "%02x %02x %02x %02x %02x %02x %02x\n", buf[0], buf[1], buf[2],
		buf[3], buf[4], buf[5], buf[6]);

	return (FM_MODIFY_BASIC_OP_SIZE + 2);
}


/*
                                                                                 
                    
                          
                      
 */
fm_s32 mt6620_off_2_longANA_1(fm_u8 *buf, fm_s32 buf_size)
{
	fm_s32 pkt_size = 0;

	if (buf_size < TX_BUF_SIZE) {
		return (-1);
	}

	buf[0] = FM_TASK_COMMAND_PKT_TYPE;
	buf[1] = FM_ENABLE_OPCODE;
	pkt_size = 4;

	/*                                                  */
	pkt_size += fm_bop_write(0x01, 0x4A00, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_udelay(30, &buf[pkt_size], buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0x01, 0x6A00, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_udelay(50, &buf[pkt_size], buf_size - pkt_size);	/*          */
	/*                                 */
	pkt_size += fm_bop_write(0x02, 0x299C, &buf[pkt_size], buf_size - pkt_size);	/*           */
	/*                                  */
	pkt_size += fm_bop_write(0x01, 0x6B82, &buf[pkt_size], buf_size - pkt_size);	/*           */
	/*                                             */
	pkt_size += fm_bop_write(0x04, 0x0142, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0x05, 0x00E7, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0x0A, 0x0060, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0x0C, 0xA88C, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0x0D, 0x0888, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0x10, 0x1E8D, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x27, 0x0005, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x0E, 0x0040, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0x03, 0x50F0, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0x3F, 0xAD06, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x3E, 0x3280, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x06, 0x0124, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0x08, 0x15B8, &buf[pkt_size], buf_size - pkt_size);	/*           */
	/*                              */
	pkt_size += fm_bop_write(0x28, 0x0000, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x00, 0x0166, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0x3A, 0x0004, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x37, 0x2590, &buf[pkt_size], buf_size - pkt_size);	/*            */
	/*                   */
	/*                                       */
	pkt_size += fm_bop_write(0x25, 0x040F, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x20, 0x2720, &buf[pkt_size], buf_size - pkt_size);	/*            */
	/*                                     */
	pkt_size += fm_bop_write(0x22, 0x6680, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x25, 0x080F, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x1E, 0x0863, &buf[pkt_size], buf_size - pkt_size);	/*                  */
	pkt_size += fm_bop_udelay(5000, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x1E, 0x0865, &buf[pkt_size], buf_size - pkt_size);	/*                   */
	pkt_size += fm_bop_udelay(5000, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x1E, 0x0871, &buf[pkt_size], buf_size - pkt_size);	/*                   */

	buf[2] = (fm_u8) ((pkt_size - 4) & 0x00FF);
	buf[3] = (fm_u8) (((pkt_size - 4) >> 8) & 0x00FF);

	return pkt_size;
}

fm_s32 mt6620_off_2_longANA_2(fm_u8 *buf, fm_s32 buf_size)
{
	fm_s32 pkt_size = 0;

	if (buf_size < TX_BUF_SIZE) {
		return (-1);
	}

	buf[0] = FM_TASK_COMMAND_PKT_TYPE;
	buf[1] = FM_ENABLE_OPCODE;
	pkt_size = 4;

	pkt_size += fm_bop_udelay(100000, &buf[pkt_size], buf_size - pkt_size);	/*              */
	pkt_size += fm_bop_write(0x2A, 0x1026, &buf[pkt_size], buf_size - pkt_size);	/*            */
	/*                   */
	pkt_size += fm_bop_write(0x00, 0x01E6, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_udelay(1, &buf[pkt_size], buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0x1B, 0x0094, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x1B, 0x0095, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_udelay(200, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0x1B, 0x0094, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x00, 0x0166, &buf[pkt_size], buf_size - pkt_size);	/*           */
	/*               */
	pkt_size += fm_bop_write(0x01, 0x6B8A, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_udelay(1, &buf[pkt_size], buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0x00, 0xC166, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_udelay(3000, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x00, 0xF166, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0x09, 0x2964, &buf[pkt_size], buf_size - pkt_size);	/*           */
	/*                   */
	pkt_size += fm_bop_write(0x2E, 0x0008, &buf[pkt_size], buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0x2B, 0x0064, &buf[pkt_size], buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0x2C, 0x0032, &buf[pkt_size], buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0x11, 0x17d4, &buf[pkt_size], buf_size - pkt_size);	/*            */
	/*                                                          */
	pkt_size += fm_bop_write(0x13, 0xFA00, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x14, 0x0580, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x15, 0xFA80, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x16, 0x0580, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x33, 0x0008, &buf[pkt_size], buf_size - pkt_size);	/*            */
	/*                     */
	pkt_size += fm_bop_write(0x64, 0x0001, &buf[pkt_size], buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0x63, 0x0020, &buf[pkt_size], buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0x9C, 0x0044, &buf[pkt_size], buf_size - pkt_size);	/*            */
	/*                                                                                                                                                                              */
	pkt_size += fm_bop_write(0x0F, 0x1A08, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0x63, 0x0021, &buf[pkt_size], buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_rd_until(0x69, 0x0001, 0x0001, &buf[pkt_size], buf_size - pkt_size);	/*                                    */
	pkt_size += fm_bop_write(0x69, 0x0001, &buf[pkt_size], buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0x63, 0x0000, &buf[pkt_size], buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_rd_until(0x6F, 0x0001, 0x0000, &buf[pkt_size], buf_size - pkt_size);	/*                           */
	/*        */
	pkt_size += fm_bop_write(0x00, 0xF167, &buf[pkt_size], buf_size - pkt_size);	/*           */

	buf[2] = (fm_u8) ((pkt_size - 4) & 0x00FF);
	buf[3] = (fm_u8) (((pkt_size - 4) >> 8) & 0x00FF);

	return pkt_size;
}

fm_s32 mt6620_rx_digital_init(fm_u8 *tx_buf, fm_s32 tx_buf_size)
{
	fm_s32 pkt_size = 0;

	if (tx_buf_size < TX_BUF_SIZE) {
		return (-1);
	}

	tx_buf[0] = FM_TASK_COMMAND_PKT_TYPE;
	tx_buf[1] = FM_ENABLE_OPCODE;
	pkt_size = 4;

	/*                */
	/*       */
	pkt_size += fm_bop_write(0x65, 0x0000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0x64, 0x0001, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0x63, 0x0480, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0x6D, 0x1AB2, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x6B, 0x2100, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x68, 0xE100, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_udelay(10000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*             */
	pkt_size += fm_bop_write(0x68, 0xE000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	/*            */
	pkt_size += fm_bop_write(0x9C, 0xab48, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x9E, 0x000C, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0x71, 0x807f, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x72, 0x878f, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	/*                                                                            */
	pkt_size += fm_bop_write(0x73, 0x07c1, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	/*              */
	pkt_size += fm_bop_write(0x9F, 0x0000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0xCB, 0x00B0, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*          */
	/*                                */
/*                                                                                                                                           */
/*                                                                                                                                               */
	/*                                   */
	pkt_size += fm_bop_write(0xE0, ((0xA301 & 0xFC00) | (0x0301 & 0x03FF)), &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xE1, ((0x00E9 & 0xFF00) | (0x00E9 & 0x00FF)), &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                      */
	/*                                            */
/*                                                                                                  */
	/*                                   */
	pkt_size += fm_bop_write(0xE3, 0x01BD, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xE4, 0x008F, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0xCC, 0x0488, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xD6, 0x036A, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xD7, 0x836a, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xDD, 0x0080, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0xB0, 0xcd00, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	/*                                                                     */
	/*                                                                          */
	/*                                                                          */
	/*                                                  */
	pkt_size += fm_bop_write(0x96, 0x4000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                */
	pkt_size += fm_bop_write(0x97, 0x021F, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                */
	pkt_size += fm_bop_write(0x98, 0x0D00, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                */
	pkt_size += fm_bop_write(0x99, 0x0E7F, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                */
	pkt_size += fm_bop_write(0xD0, 0x8192, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*               */
	pkt_size += fm_bop_write(0xD1, 0x2086, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*               */
	pkt_size += fm_bop_write(0x90, 0x0192, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                */
	pkt_size += fm_bop_write(0x91, 0x0086, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                */
	pkt_size += fm_bop_write(0x92, 0x0192, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                */
	pkt_size += fm_bop_write(0x93, 0x0086, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                */
	pkt_size += fm_bop_write(0x94, 0x0192, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                */
	pkt_size += fm_bop_write(0x95, 0x0086, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                */
	pkt_size += fm_bop_write(0x52, 0x17F3, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                */
	/*            */
	pkt_size += fm_bop_write(0x9F, 0x0001, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0xDE, 0x3388, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xC2, 0x0180, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xC3, 0x0180, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xDB, 0x0181, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xDC, 0x0184, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xA2, 0x03C0, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xA3, 0x03C0, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xA4, 0x03C0, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xA5, 0x03C0, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xA6, 0x03C0, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xA7, 0x03C0, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xE3, 0x0280, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xE4, 0x0280, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xE5, 0x0280, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xE6, 0x026C, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xE7, 0x026C, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xEA, 0x026C, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_udelay(1000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xB6, 0x03FC, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xB7, 0x02C1, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xA8, 0x0820, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xAC, 0x065E, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xAD, 0x09AD, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xAE, 0x0CF8, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xAF, 0x0302, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xB0, 0x04A6, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xB1, 0x062C, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xEC, 0x014A, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xC8, 0x0232, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xC7, 0x0184, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xD8, 0x00E8, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xFB, 0x051F, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_udelay(1000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	/*                                                     */
	pkt_size += fm_bop_write(0xC9, 0x01F0, &tx_buf[pkt_size], tx_buf_size - pkt_size);
	pkt_size += fm_bop_write(0xCA, 0x0250, &tx_buf[pkt_size], tx_buf_size - pkt_size);
	pkt_size += fm_bop_write(0xD4, 0x2657, &tx_buf[pkt_size], tx_buf_size - pkt_size);
	pkt_size += fm_bop_write(0x9F, 0x0002, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0xA8, 0x0000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0xA8, 0x0000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0xA8, 0x0000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0xA8, 0xFF80, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xA8, 0x0061, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0xA8, 0xFF22, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xA8, 0x0000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0xA8, 0x0100, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xA8, 0x009A, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0xA8, 0xFF80, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xA8, 0x0140, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xA8, 0xFF42, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xA8, 0xFFE0, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xA8, 0x0114, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xA8, 0xFF4E, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xA8, 0x0000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0xA8, 0x0000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0xA8, 0x0000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0xA8, 0x003E, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0xA8, 0xFF6E, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xA8, 0x0087, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0xA8, 0x0000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0xA8, 0xFEDC, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xA8, 0x0015, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0xA8, 0x00CF, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0xA8, 0xFF6B, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xA8, 0x0164, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xA8, 0x002B, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0xA8, 0xFF7E, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xA8, 0x0065, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_udelay(10000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*             */
	pkt_size += fm_bop_write(0x9F, 0x0002, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0xA9, 0x0000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0xA9, 0xFFEE, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xA9, 0xFF81, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	/*                             */
	pkt_size += fm_bop_write(0xA9, 0xFFC3, &tx_buf[pkt_size], tx_buf_size - pkt_size);
	pkt_size += fm_bop_write(0xA9, 0x0022, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0xA9, 0x0000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0xA9, 0xFFCC, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xA9, 0x0023, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0xA9, 0xFFDA, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xA9, 0xFFF7, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_udelay(10, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0x9F, 0x0001, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0xD3, 0x250b, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xBB, 0x2320, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xD0, 0x02f8, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xEC, 0x019a, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xFE, 0x2140, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x9F, 0x0000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*         */
	/*            */
	pkt_size += fm_bop_write(0x9F, 0x0003, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0xBD, 0x37EB, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xBC, 0x0404, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0x9F, 0x0000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*         */

	tx_buf[2] = (fm_u8) ((pkt_size - 4) & 0x00FF);
	tx_buf[3] = (fm_u8) (((pkt_size - 4) >> 8) & 0x00FF);

	return pkt_size;
}

/*
                                                                                             
                    
                          
                      
 */
fm_s32 mt6620_pwrup_digital_init_1(fm_u8 *buf, fm_s32 buf_size)
{
	fm_s32 pkt_size = 0;

	if (buf_size < TX_BUF_SIZE) {
		return (-1);
	}

	buf[0] = FM_TASK_COMMAND_PKT_TYPE;
	buf[1] = FM_ENABLE_OPCODE;
	pkt_size = 4;


	/*                */
	/*       */
	pkt_size += fm_bop_write(0x65, 0x0000, &buf[pkt_size], buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0x64, 0x0001, &buf[pkt_size], buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0x63, 0x0480, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0x6D, 0x1AB2, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x6B, 0x2100, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x68, 0xE100, &buf[pkt_size], buf_size - pkt_size);	/*            */

	buf[2] = (fm_u8) ((pkt_size - 4) & 0x00FF);
	buf[3] = (fm_u8) (((pkt_size - 4) >> 8) & 0x00FF);

	return pkt_size;
}

fm_s32 mt6620_pwrup_digital_init_2(fm_u8 *buf, fm_s32 buf_size)
{
	fm_s32 pkt_size = 0;

	if (buf_size < TX_BUF_SIZE) {
		return (-1);
	}

	buf[0] = FM_TASK_COMMAND_PKT_TYPE;
	buf[1] = FM_ENABLE_OPCODE;
	pkt_size = 4;

	pkt_size += fm_bop_write(0x68, 0xE000, &buf[pkt_size], buf_size - pkt_size);	/*            */
	/*            */
	pkt_size += fm_bop_write(0x9C, 0xab48, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x9E, 0x000C, &buf[pkt_size], buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0x71, 0x807f, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x72, 0x878f, &buf[pkt_size], buf_size - pkt_size);	/*            */
	/*                                                                            */
	pkt_size += fm_bop_write(0x73, 0x07c1, &buf[pkt_size], buf_size - pkt_size);	/*           */
	/*              */
	pkt_size += fm_bop_write(0x9F, 0x0000, &buf[pkt_size], buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0xCB, 0x00B0, &buf[pkt_size], buf_size - pkt_size);	/*          */
	/*                                */
	pkt_size += fm_bop_write(0xE0, ((0xA301 & 0xFC00) | (FMR_RSSI_TH_LONG_MT6620 & 0x03FF)), &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xE1, ((0x00E9 & 0xFF00) | (FMR_CQI_TH_MT6620 & 0x00FF)), &buf[pkt_size], buf_size - pkt_size);	/*                      */
	/*                                            */
	pkt_size += fm_bop_write(0xE3, FMR_MR_TH_MT6620, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xE4, 0x008F, &buf[pkt_size], buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0xCC, 0x0488, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xD6, 0x036A, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xD7, 0x836a, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xDD, 0x0080, &buf[pkt_size], buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0xB0, 0xcd00, &buf[pkt_size], buf_size - pkt_size);	/*            */
	/*                                                                     */
	/*                                                                          */
	/*                                                                          */
	/*                                                  */
	pkt_size += fm_bop_write(0x96, 0x4000, &buf[pkt_size], buf_size - pkt_size);	/*                */
	pkt_size += fm_bop_write(0x97, 0x021F, &buf[pkt_size], buf_size - pkt_size);	/*                */
	pkt_size += fm_bop_write(0x98, 0x0D00, &buf[pkt_size], buf_size - pkt_size);	/*                */
	pkt_size += fm_bop_write(0x99, 0x0E7F, &buf[pkt_size], buf_size - pkt_size);	/*                */
	pkt_size += fm_bop_write(0xD0, 0x8192, &buf[pkt_size], buf_size - pkt_size);	/*               */
	pkt_size += fm_bop_write(0xD1, 0x2086, &buf[pkt_size], buf_size - pkt_size);	/*               */
	pkt_size += fm_bop_write(0x90, 0x0192, &buf[pkt_size], buf_size - pkt_size);	/*                */
	pkt_size += fm_bop_write(0x91, 0x0086, &buf[pkt_size], buf_size - pkt_size);	/*                */
	pkt_size += fm_bop_write(0x92, 0x0192, &buf[pkt_size], buf_size - pkt_size);	/*                */
	pkt_size += fm_bop_write(0x93, 0x0086, &buf[pkt_size], buf_size - pkt_size);	/*                */
	pkt_size += fm_bop_write(0x94, 0x0192, &buf[pkt_size], buf_size - pkt_size);	/*                */
	pkt_size += fm_bop_write(0x95, 0x0086, &buf[pkt_size], buf_size - pkt_size);	/*                */
	pkt_size += fm_bop_write(0x52, 0x17F3, &buf[pkt_size], buf_size - pkt_size);	/*                */
	/*            */
	pkt_size += fm_bop_write(0x9F, 0x0001, &buf[pkt_size], buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0xDE, 0x3388, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xC2, 0x0180, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xC3, 0x0180, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xDB, 0x0181, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xDC, 0x0184, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xA2, 0x03C0, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xA3, 0x03C0, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xA4, 0x03C0, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xA5, 0x03C0, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xA6, 0x03C0, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xA7, 0x03C0, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xE3, 0x0280, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xE4, 0x0280, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xE5, 0x0280, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xE6, 0x026C, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xE7, 0x026C, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xEA, 0x026C, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_udelay(1000, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xB6, 0x03FC, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xB7, 0x02C1, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xA8, 0x0820, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xAC, 0x065E, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xAD, 0x09AD, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xAE, 0x0CF8, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xAF, 0x0302, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xB0, 0x04A6, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xB1, 0x062C, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xEC, 0x014A, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xC8, 0x0232, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xC7, 0x0184, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xD8, 0x00E8, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xFB, 0x051F, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_udelay(1000, &buf[pkt_size], buf_size - pkt_size);	/*            */
	/*                                                     */
	pkt_size += fm_bop_write(0xC9, 0x01F0, &buf[pkt_size], buf_size - pkt_size);
	pkt_size += fm_bop_write(0xCA, 0x0250, &buf[pkt_size], buf_size - pkt_size);
	pkt_size += fm_bop_write(0xD4, 0x2657, &buf[pkt_size], buf_size - pkt_size);
	pkt_size += fm_bop_write(0x9F, 0x0002, &buf[pkt_size], buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0xA8, 0x0000, &buf[pkt_size], buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0xA8, 0x0000, &buf[pkt_size], buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0xA8, 0x0000, &buf[pkt_size], buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0xA8, 0xFF80, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xA8, 0x0061, &buf[pkt_size], buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0xA8, 0xFF22, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xA8, 0x0000, &buf[pkt_size], buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0xA8, 0x0100, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xA8, 0x009A, &buf[pkt_size], buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0xA8, 0xFF80, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xA8, 0x0140, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xA8, 0xFF42, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xA8, 0xFFE0, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xA8, 0x0114, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xA8, 0xFF4E, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xA8, 0x0000, &buf[pkt_size], buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0xA8, 0x0000, &buf[pkt_size], buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0xA8, 0x0000, &buf[pkt_size], buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0xA8, 0x003E, &buf[pkt_size], buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0xA8, 0xFF6E, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xA8, 0x0087, &buf[pkt_size], buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0xA8, 0x0000, &buf[pkt_size], buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0xA8, 0xFEDC, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xA8, 0x0015, &buf[pkt_size], buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0xA8, 0x00CF, &buf[pkt_size], buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0xA8, 0xFF6B, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xA8, 0x0164, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xA8, 0x002B, &buf[pkt_size], buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0xA8, 0xFF7E, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xA8, 0x0065, &buf[pkt_size], buf_size - pkt_size);	/*          */

	buf[2] = (fm_u8) ((pkt_size - 4) & 0x00FF);
	buf[3] = (fm_u8) (((pkt_size - 4) >> 8) & 0x00FF);

	return pkt_size;
}

fm_s32 mt6620_pwrup_digital_init_3(fm_u8 *buf, fm_s32 buf_size)
{
	fm_s32 pkt_size = 0;

	if (buf_size < TX_BUF_SIZE) {
		return (-1);
	}

	buf[0] = FM_TASK_COMMAND_PKT_TYPE;
	buf[1] = FM_ENABLE_OPCODE;
	pkt_size = 4;

	pkt_size += fm_bop_write(0x9F, 0x0002, &buf[pkt_size], buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0xA9, 0x0000, &buf[pkt_size], buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0xA9, 0xFFEE, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xA9, 0xFF81, &buf[pkt_size], buf_size - pkt_size);	/*            */
	/*                             */
	pkt_size += fm_bop_write(0xA9, 0xFFC3, &buf[pkt_size], buf_size - pkt_size);
	pkt_size += fm_bop_write(0xA9, 0x0022, &buf[pkt_size], buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0xA9, 0x0000, &buf[pkt_size], buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0xA9, 0xFFCC, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xA9, 0x0023, &buf[pkt_size], buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0xA9, 0xFFDA, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xA9, 0xFFF7, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_udelay(10, &buf[pkt_size], buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0x9F, 0x0001, &buf[pkt_size], buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0xD3, 0x250b, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xBB, 0x2320, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xD0, 0x02f8, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xEC, 0x019a, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xFE, 0x2140, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x9F, 0x0000, &buf[pkt_size], buf_size - pkt_size);	/*         */
	/*            */
	pkt_size += fm_bop_write(0x9F, 0x0003, &buf[pkt_size], buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0xBD, 0x37EB, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xBC, 0x0404, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0x9F, 0x0000, &buf[pkt_size], buf_size - pkt_size);	/*         */

	buf[2] = (fm_u8) ((pkt_size - 4) & 0x00FF);
	buf[3] = (fm_u8) (((pkt_size - 4) >> 8) & 0x00FF);

	return pkt_size;
}

/*
                                                                     
                    
                          
                      
 */
fm_s32 mt6620_pwrdown(fm_u8 *buf, fm_s32 buf_size)
{
	fm_s32 pkt_size = 0;

	if (buf_size < TX_BUF_SIZE) {
		return (-1);
	}

	buf[0] = FM_TASK_COMMAND_PKT_TYPE;
	buf[1] = FM_ENABLE_OPCODE;
	pkt_size = 4;

	/*                          */
	pkt_size += fm_bop_write(0x63, 0x0000, &buf[pkt_size], buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_modify(0x6E, 0xFFFC, 0x0000, &buf[pkt_size], buf_size - pkt_size);	/*                         */
	pkt_size += fm_bop_modify(0x6E, 0xFFFC, 0x0000, &buf[pkt_size], buf_size - pkt_size);	/*                         */
	pkt_size += fm_bop_modify(0x6E, 0xFFFC, 0x0000, &buf[pkt_size], buf_size - pkt_size);	/*                         */
	pkt_size += fm_bop_modify(0x6E, 0xFFFC, 0x0000, &buf[pkt_size], buf_size - pkt_size);	/*                         */
	/*                          */
	pkt_size += fm_bop_write(0x2A, 0x0022, &buf[pkt_size], buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0x1E, 0x0860, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x20, 0x0720, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x20, 0x2720, &buf[pkt_size], buf_size - pkt_size);	/*            */
	/*                              */
	pkt_size += fm_bop_write(0x00, 0x0000, &buf[pkt_size], buf_size - pkt_size);	/*        */
	pkt_size += fm_bop_write(0x01, 0x0000, &buf[pkt_size], buf_size - pkt_size);	/*        */
	pkt_size += fm_bop_write(0x04, 0x0141, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0x09, 0x0964, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0x0C, 0x288C, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0x26, 0x0004, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x3A, 0x0000, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x3B, 0x00C3, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x3E, 0x3280, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x3F, 0x4E16, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x41, 0x0004, &buf[pkt_size], buf_size - pkt_size);	/*            */
	/*                   */
	pkt_size += fm_bop_write(0x12, 0x0000, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x47, 0x0000, &buf[pkt_size], buf_size - pkt_size);	/*            */

	buf[2] = (fm_u8) ((pkt_size - 4) & 0x00FF);
	buf[3] = (fm_u8) (((pkt_size - 4) >> 8) & 0x00FF);

	return pkt_size;
}

/*
                                                         
                    
                          
                      
 */
fm_s32 mt6620_rampdown(fm_u8 *buf, fm_s32 buf_size)
{
	fm_s32 pkt_size = 0;

	if (buf_size < TX_BUF_SIZE) {
		return (-1);
	}

	buf[0] = FM_TASK_COMMAND_PKT_TYPE;
	buf[1] = FM_RAMPDOWN_OPCODE;
	pkt_size = 4;

	pkt_size += fm_bop_modify(0x9C, 0xFF87, 0x0068, &buf[pkt_size], buf_size - pkt_size);	/*                         */
	pkt_size += fm_bop_write(0x9F, 0x0001, &buf[pkt_size], buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0xC8, 0x0101, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xDD, 0x0000, &buf[pkt_size], buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0xD8, 0x0000, &buf[pkt_size], buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0x9F, 0x0000, &buf[pkt_size], buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_udelay(35000, &buf[pkt_size], buf_size - pkt_size);	/*             */
	/*                                   */
	pkt_size += fm_bop_write(0x6B, 0x0000, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_modify(0x63, 0xFFF0, 0x0000, &buf[pkt_size], buf_size - pkt_size);	/*                           */
	pkt_size += fm_bop_rd_until(0x6f, 0x0001, 0x0000, &buf[pkt_size], buf_size - pkt_size);	/*                  */
	pkt_size += fm_bop_write(0x6B, 0x2100, &buf[pkt_size], buf_size - pkt_size);	/*            */
	/*                                 */

	buf[2] = (fm_u8) ((pkt_size - 4) & 0x00FF);
	buf[3] = (fm_u8) (((pkt_size - 4) >> 8) & 0x00FF);

	return pkt_size;
}

#if 0				/*                                       */
fm_s32 mt6620_rampdown_tx(fm_u8 *tx_buf, fm_s32 tx_buf_size)
{
	fm_s32 pkt_size = 0;

	if (tx_buf_size < TX_BUF_SIZE) {
		return (-1);
	}

	tx_buf[0] = FM_TASK_COMMAND_PKT_TYPE;
	tx_buf[1] = FM_RAMPDOWN_OPCODE;
	pkt_size = 4;

	pkt_size += fm_bop_write(0x3B, 0x0500, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x3A, 0x00FF, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0x65, 0x0001, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0x48, 0x0000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*         */

	tx_buf[2] = (fm_u8) ((pkt_size - 4) & 0x00FF);
	tx_buf[3] = (fm_u8) (((pkt_size - 4) >> 8) & 0x00FF);

	return pkt_size;
}
#endif
/*
                                     
                    
                          
                                  
                      
 */
fm_s32 mt6620_tune_1(fm_u8 *buf, fm_s32 buf_size, fm_u16 freq)
{
	fm_s32 pkt_size = 0;

	if (buf_size < TX_BUF_SIZE) {
		return (-1);
	}
	if (0 == fm_get_channel_space(freq)) {
		freq *= 10;
	}

	freq = (freq - 6400) * 2 / 10;

	buf[0] = FM_TASK_COMMAND_PKT_TYPE;
	buf[1] = FM_TUNE_OPCODE;
	pkt_size = 4;

	pkt_size += fm_bop_modify(0x0F, 0xFC00, freq, &buf[pkt_size], buf_size - pkt_size);	/*                                               */
	pkt_size += fm_bop_modify(0x63, 0xFFFF, 0x0001, &buf[pkt_size], buf_size - pkt_size);	/*                 */

	buf[2] = (fm_u8) ((pkt_size - 4) & 0x00FF);
	buf[3] = (fm_u8) (((pkt_size - 4) >> 8) & 0x00FF);

	return pkt_size;
}

fm_s32 mt6620_tune_2(fm_u8 *buf, fm_s32 buf_size, fm_u16 freq)
{
	fm_s32 pkt_size = 0;

	if (buf_size < TX_BUF_SIZE) {
		return (-1);
	}
/*
                                          
            
     

                                 
*/
	buf[0] = FM_TASK_COMMAND_PKT_TYPE;
	buf[1] = FM_TUNE_OPCODE;
	pkt_size = 4;

	pkt_size += fm_bop_modify(0x9C, 0xFFF7, 0x0000, &buf[pkt_size], buf_size - pkt_size);	/*                 */

	buf[2] = (fm_u8) ((pkt_size - 4) & 0x00FF);
	buf[3] = (fm_u8) (((pkt_size - 4) >> 8) & 0x00FF);

	return pkt_size;
}

fm_s32 mt6620_tune_3(fm_u8 *buf, fm_s32 buf_size, fm_u16 freq)
{
	fm_s32 pkt_size = 0;

	if (buf_size < TX_BUF_SIZE) {
		return (-1);
	}
/*
                                          
            
     

                                 
*/
	buf[0] = FM_TASK_COMMAND_PKT_TYPE;
	buf[1] = FM_TUNE_OPCODE;
	pkt_size = 4;

	pkt_size += fm_bop_write(0x9F, 0x0001, &buf[pkt_size], buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0xC8, 0x0232, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xDD, 0x8833, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xD8, 0x00E8, &buf[pkt_size], buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0x9F, 0x0000, &buf[pkt_size], buf_size - pkt_size);	/*         */

	buf[2] = (fm_u8) ((pkt_size - 4) & 0x00FF);
	buf[3] = (fm_u8) (((pkt_size - 4) >> 8) & 0x00FF);

	return pkt_size;
}

fm_s32 mt6620_fast_tune(fm_u8 *tx_buf, fm_s32 tx_buf_size, fm_u16 freq)
{
	fm_s32 pkt_size = 0;

	if (tx_buf_size < TX_BUF_SIZE) {
		return (-1);
	}

	if (0 == fm_get_channel_space(freq)) {
		freq *= 10;
	}

	freq = (freq - 6400) * 2 / 10;

	tx_buf[0] = FM_TASK_COMMAND_PKT_TYPE;
	tx_buf[1] = FM_TUNE_OPCODE;
	pkt_size = 4;

	pkt_size += fm_bop_modify(0x63, 0xFFF0, 0x0000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                   */
	pkt_size += fm_bop_modify(0x6F, 0xFFFE, 0x0000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                 */
	pkt_size += fm_bop_modify(0x0F, 0xFC00, freq, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                                               */
	/*                                   */
	pkt_size += fm_bop_write(0x6B, 0x0000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_modify(0x63, 0xFFFE, 0x0001, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                 */
	pkt_size += fm_bop_rd_until(0x69, 0x0001, 0x0001, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                  */
	pkt_size += fm_bop_modify(0x69, 0xFFFE, 0x0001, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                 */
	pkt_size += fm_bop_write(0x6B, 0x2100, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	/*                                 */

	tx_buf[2] = (fm_u8) ((pkt_size - 4) & 0x00FF);
	tx_buf[3] = (fm_u8) (((pkt_size - 4) >> 8) & 0x00FF);

	return pkt_size;
}

/*
                                                         
                    
                          
                                   
                       
                                                                                                                                  
  
                      
 */

fm_s32 mt6620_full_cqi_req(fm_u8 *buf, fm_s32 buf_size, fm_u16 freq, fm_s32 cnt, fm_s32 type)
{
	fm_s32 pkt_size = 0;

	if (buf_size < TX_BUF_SIZE) {
		return (-1);
	}
	if (0 == fm_get_channel_space(freq)) {
		freq *= 10;
	}
	buf[0] = FM_TASK_COMMAND_PKT_TYPE;
	buf[1] = FM_SOFT_MUTE_TUNE_OPCODE;
	pkt_size = 4;

	switch (type) {
	case 1:
		buf[pkt_size] = 0x0001;
		pkt_size++;
		buf[pkt_size] = (fm_u8) (freq & 0x00FF);
		pkt_size++;
		buf[pkt_size] = (fm_u8) ((freq >> 8) & 0x00FF);
		pkt_size++;
		break;
	default:
		buf[pkt_size] = (fm_u16) type;
		pkt_size++;
		break;
	}

	buf[2] = (fm_u8) ((pkt_size - 4) & 0x00FF);
	buf[3] = (fm_u8) (((pkt_size - 4) >> 8) & 0x00FF);

	return pkt_size;

}

/*                               */
fm_s32 mt6620_tune_tx(fm_u8 *tx_buf, fm_s32 tx_buf_size, fm_u16 freq)
{
	fm_s32 pkt_size = 0;

	if (tx_buf_size < TX_BUF_SIZE) {
		return (-1);
	}

	if (0 == fm_get_channel_space(freq)) {
		freq *= 10;
	}

	freq = (freq - 6400) * 2 / 10;

	tx_buf[0] = FM_TASK_COMMAND_PKT_TYPE;
	tx_buf[1] = FM_TUNE_OPCODE;
	pkt_size = 4;

	/*                                        */
	pkt_size += fm_bop_write(0x3B, 0x0500, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x3A, 0x00FF, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0x65, 0x0001, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0x48, 0x0000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*         */
	/*                 */
	pkt_size += fm_bop_modify(0x41, 0xFFFE, 0x0001, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                 */
	/*                 */
	pkt_size += fm_bop_modify(0x12, 0x7FFF, 0x0000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                  */
	/*                                              */
	pkt_size += fm_bop_modify(0x47, 0x003F, 0x0000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                    */
	pkt_size += fm_bop_modify(0x0F, 0xFC00, freq, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                      */
	pkt_size += fm_bop_write(0x26, 0x002C, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                                                        */
	pkt_size += fm_bop_udelay(1000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*              */
	pkt_size += fm_bop_write(0x26, 0x003C, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                            */
	pkt_size += fm_bop_udelay(1000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*              */
	pkt_size += fm_bop_write(0x10, 0x1E8D, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_udelay(1000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*              */
	pkt_size += fm_bop_write(0x10, 0x9E8D, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_udelay(10000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*               */
	pkt_size += fm_bop_write(0x26, 0x0024, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                                         */
	pkt_size += fm_bop_write(0x65, 0x400F, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x48, 0x8000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x3B, 0x0420, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x3A, 0x01FF, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	/*                 */
	/*                                                                                             */
	/*                                                                                                               */

	tx_buf[2] = (fm_u8) ((pkt_size - 4) & 0x00FF);
	tx_buf[3] = (fm_u8) (((pkt_size - 4) >> 8) & 0x00FF);

	return pkt_size;
}

/*                               */
fm_s32 mt6620_tune_txscan(fm_u8 *tx_buf, fm_s32 tx_buf_size, fm_u16 freq)
{
	fm_s32 pkt_size = 0;

	if (tx_buf_size < TX_BUF_SIZE) {
		return (-1);
	}

	if (0 == fm_get_channel_space(freq)) {
		freq *= 10;
	}

	freq = (freq - 6400) * 2 / 10;

	tx_buf[0] = FM_TASK_COMMAND_PKT_TYPE;
	tx_buf[1] = FM_TUNE_OPCODE;
	pkt_size = 4;

	/*          */
	/*                                   */
	pkt_size += fm_bop_write(0x6B, 0x0000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_modify(0x63, 0xFFF0, 0x0000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                           */
	pkt_size += fm_bop_rd_until(0x6f, 0x0001, 0x0000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                  */
	pkt_size += fm_bop_modify(0x9C, 0xFFFF, 0x0008, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                         */
	pkt_size += fm_bop_write(0x6B, 0x2100, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	/*                                 */

	/*                     */
	pkt_size += fm_bop_modify(0x0F, 0xFC00, freq, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                                               */

	/*                              */
#ifdef MT6620_FM_USE_SHORT_ANTENNA
	pkt_size += fm_bop_write(0x28, 0x3800, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x03, 0x90F0, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0x2E, 0x0028, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0x2F, 0x0000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0x26, 0x003C, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0x2E, 0x002C, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_udelay(10000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0x26, 0x0024, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0x28, 0x0000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*          */
#endif

	/*             */
#ifdef MT6620E1
	pkt_size += fm_bop_write(0x9F, 0x0000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0xAF, 0x2210, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
#endif

	/*                                   */
	/*                                                                                              */

	/*                                                */
	pkt_size += fm_bop_modify(0x63, 0xFFFF, 0x0001, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                          */

	/*                                      */
	/*                                                                                                                                   */

	/*                                        */
	/*                                                                                                                        */

	/*                                     */
	/*                                                                                              */

	/*             */
#ifdef MT6620E1
	/*                                                                                                */
#endif

	tx_buf[2] = (fm_u8) ((pkt_size - 4) & 0x00FF);
	tx_buf[3] = (fm_u8) (((pkt_size - 4) >> 8) & 0x00FF);

	return pkt_size;
}

/*
                                     
                    
                          
                                    
                                                   
                          
                          
                      
 */
fm_s32 mt6620_seek_1(fm_u8 *buf, fm_s32 buf_size, fm_u16 seekdir, fm_u16 space, fm_u16 max_freq,
		     fm_u16 min_freq)
{
	fm_s32 pkt_size = 0;

	if (buf_size < TX_BUF_SIZE) {
		return (-1);
	}

	buf[0] = FM_TASK_COMMAND_PKT_TYPE;
	buf[1] = FM_SEEK_OPCODE;
	pkt_size = 4;

	/*                                                             */
	if (seekdir == 0) {
		pkt_size += fm_bop_modify(0x66, 0xFBFF, 0x0000, &buf[pkt_size], buf_size - pkt_size);	/*                       */
	} else {
		pkt_size += fm_bop_modify(0x66, 0xFFFF, 0x0400, &buf[pkt_size], buf_size - pkt_size);	/*                         */
	}

	/*                             */
	pkt_size += fm_bop_modify(0x66, 0xFFFF, 0x0800, &buf[pkt_size], buf_size - pkt_size);	/*                    */
	/*                                                                                   */
	if (space == 4) {
		pkt_size += fm_bop_modify(0x66, 0x8FFF, 0x4000, &buf[pkt_size], buf_size - pkt_size);	/*                                        */
	} else {
		pkt_size += fm_bop_modify(0x66, 0x8FFF, 0x2000, &buf[pkt_size], buf_size - pkt_size);	/*                                        */
	}

	/*                            */
	max_freq = (max_freq - 640) * 2;
	pkt_size += fm_bop_modify(0x66, 0xFC00, max_freq, &buf[pkt_size], buf_size - pkt_size);

	/*                            */
	min_freq = (min_freq - 640) * 2;
	pkt_size += fm_bop_modify(0x67, 0xFC00, min_freq, &buf[pkt_size], buf_size - pkt_size);
	/*                                                */
	pkt_size += fm_bop_modify(0x63, 0xFFFF, 0x0002, &buf[pkt_size], buf_size - pkt_size);	/*             */

	buf[2] = (fm_u8) ((pkt_size - 4) & 0x00FF);
	buf[3] = (fm_u8) (((pkt_size - 4) >> 8) & 0x00FF);

	return pkt_size;
}

fm_s32 mt6620_seek_2(fm_u8 *buf, fm_s32 buf_size, fm_u16 seekdir, fm_u16 space, fm_u16 max_freq,
		     fm_u16 min_freq)
{
	fm_s32 pkt_size = 0;

	if (buf_size < TX_BUF_SIZE) {
		return (-1);
	}

	buf[0] = FM_TASK_COMMAND_PKT_TYPE;
	buf[1] = FM_SEEK_OPCODE;
	pkt_size = 4;

	/*                                 */
	pkt_size += fm_bop_write(0x9F, 0x0001, &buf[pkt_size], buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0xC8, 0x0232, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xDD, 0x8833, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xD8, 0x00E8, &buf[pkt_size], buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0x9F, 0x0000, &buf[pkt_size], buf_size - pkt_size);	/*         */

	buf[2] = (fm_u8) ((pkt_size - 4) & 0x00FF);
	buf[3] = (fm_u8) (((pkt_size - 4) >> 8) & 0x00FF);

	return pkt_size;
}

/*
                                     
                    
                          
                                    
                                                   
                          
                          
                      
 */
fm_s32 mt6620_scan_1(fm_u8 *buf, fm_s32 buf_size, fm_u16 scandir, fm_u16 space, fm_u16 max_freq,
		     fm_u16 min_freq)
{
	fm_s32 pkt_size = 0;

	if (buf_size < TX_BUF_SIZE) {
		return (-1);
	}

	buf[0] = FM_TASK_COMMAND_PKT_TYPE;
	buf[1] = FM_SCAN_OPCODE;
	pkt_size = 4;

	/*                                                             */
	if (scandir == 0) {
		pkt_size += fm_bop_modify(0x66, 0xFBFF, 0x0000, &buf[pkt_size], buf_size - pkt_size);	/*                       */
	} else {
		pkt_size += fm_bop_modify(0x66, 0xFFFF, 0x0400, &buf[pkt_size], buf_size - pkt_size);	/*                         */
	}

	/*                             */
	pkt_size += fm_bop_modify(0x66, 0xFFFF, 0x0800, &buf[pkt_size], buf_size - pkt_size);	/*                    */
	/*                                                                                   */
	if (space == 4) {
		pkt_size += fm_bop_modify(0x66, 0x8FFF, 0x4000, &buf[pkt_size], buf_size - pkt_size);	/*                                        */
	} else {
		pkt_size += fm_bop_modify(0x66, 0x8FFF, 0x2000, &buf[pkt_size], buf_size - pkt_size);	/*                                        */
	}

	/*                            */
	max_freq = (max_freq - 640) * 2;
	pkt_size += fm_bop_modify(0x66, 0xFC00, max_freq, &buf[pkt_size], buf_size - pkt_size);

	/*                            */
	min_freq = (min_freq - 640) * 2;
	pkt_size += fm_bop_modify(0x67, 0xFC00, min_freq, &buf[pkt_size], buf_size - pkt_size);
	/*                                                */
	pkt_size += fm_bop_modify(0x63, 0xFFFF, 0x0004, &buf[pkt_size], buf_size - pkt_size);	/*             */

	buf[2] = (fm_u8) ((pkt_size - 4) & 0x00FF);
	buf[3] = (fm_u8) (((pkt_size - 4) >> 8) & 0x00FF);

	return pkt_size;
}

fm_s32 mt6620_scan_2(fm_u8 *buf, fm_s32 buf_size, fm_u16 scandir, fm_u16 space, fm_u16 max_freq,
		     fm_u16 min_freq)
{
	fm_s32 pkt_size = 0;

	if (buf_size < TX_BUF_SIZE) {
		return (-1);
	}

	buf[0] = FM_TASK_COMMAND_PKT_TYPE;
	buf[1] = FM_SCAN_OPCODE;
	pkt_size = 4;

	/*                                 */
	pkt_size += fm_bop_write(0x9F, 0x0001, &buf[pkt_size], buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0xC8, 0x0232, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xDD, 0x8833, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xD8, 0x00E8, &buf[pkt_size], buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0x9F, 0x0000, &buf[pkt_size], buf_size - pkt_size);	/*         */

	buf[2] = (fm_u8) ((pkt_size - 4) & 0x00FF);
	buf[3] = (fm_u8) (((pkt_size - 4) >> 8) & 0x00FF);

	return pkt_size;
}

fm_s32 mt6620_get_reg(fm_u8 *buf, fm_s32 buf_size, fm_u8 addr)
{
	if (buf_size < TX_BUF_SIZE) {
		return (-1);
	}

	buf[0] = FM_TASK_COMMAND_PKT_TYPE;
	buf[1] = FSPI_READ_OPCODE;
	buf[2] = 0x01;
	buf[3] = 0x00;
	buf[4] = addr;

	WCN_DBG(FM_DBG | CHIP, "%02x %02x %02x %02x %02x\n", buf[0], buf[1], buf[2], buf[3],
		buf[4]);
	return 5;
}

fm_s32 mt6620_set_reg(fm_u8 *buf, fm_s32 buf_size, fm_u8 addr, fm_u16 value)
{
	if (buf_size < TX_BUF_SIZE) {
		return (-1);
	}

	buf[0] = FM_TASK_COMMAND_PKT_TYPE;
	buf[1] = FSPI_WRITE_OPCODE;
	buf[2] = 0x03;
	buf[3] = 0x00;
	buf[4] = addr;
	buf[5] = (fm_u8) ((value) & 0x00FF);
	buf[6] = (fm_u8) ((value >> 8) & 0x00FF);

	WCN_DBG(FM_DBG | CHIP, "%02x %02x %02x %02x %02x %02x %02x\n", buf[0], buf[1], buf[2],
		buf[3], buf[4], buf[5], buf[6]);
	return 7;
}

fm_s32 mt6620_rds_rx_enable(fm_u8 *tx_buf, fm_s32 tx_buf_size)	/*            */
{
	fm_s32 pkt_size = 0;

	if (tx_buf_size < TX_BUF_SIZE) {
		return (-1);
	}

	tx_buf[0] = FM_TASK_COMMAND_PKT_TYPE;
	tx_buf[1] = RDS_RX_ENABLE_OPCODE;
	pkt_size = 4;

	pkt_size += fm_bop_write(0x9F, 0x0003, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xCB, 0xE016, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*              */
	pkt_size += fm_bop_write(0x9F, 0x0000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0x63, 0x0491, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*             */
	pkt_size += fm_bop_modify(0x6B, 0xFFFF, 0x2000, &tx_buf[pkt_size], TX_BUF_SIZE - pkt_size);	/*                  */

	tx_buf[2] = (fm_u8) ((pkt_size - 4) & 0x00FF);
	tx_buf[3] = (fm_u8) (((pkt_size - 4) >> 8) & 0x00FF);

	return pkt_size;
}

fm_s32 mt6620_rds_rx_disable(fm_u8 *tx_buf, fm_s32 tx_buf_size)	/*            */
{
	fm_s32 pkt_size = 0;

	if (tx_buf_size < TX_BUF_SIZE) {
		return (-1);
	}

	tx_buf[0] = FM_TASK_COMMAND_PKT_TYPE;
	tx_buf[1] = RDS_RX_ENABLE_OPCODE;
	pkt_size = 4;

	pkt_size += fm_bop_modify(0x6B, 0xDFFF, 0x0000, &tx_buf[pkt_size], TX_BUF_SIZE - pkt_size);	/*                  */
	pkt_size += fm_bop_write(0x63, 0x0481, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*             */


	tx_buf[2] = (fm_u8) ((pkt_size - 4) & 0x00FF);
	tx_buf[3] = (fm_u8) (((pkt_size - 4) >> 8) & 0x00FF);

	return pkt_size;
}

/*            */
fm_s32 mt6620_rds_tx(fm_u8 *tx_buf, fm_s32 tx_buf_size, fm_u16 pi, fm_u16 *ps, fm_u16 *other_rds,
		     fm_u8 other_rds_cnt)
{
	fm_s32 pkt_size = 0;
	fm_s32 i;

	if (tx_buf_size < TX_BUF_SIZE) {
		return (-1);
	}

	if (other_rds_cnt > 29) {
		return (-2);
	}

	tx_buf[0] = FM_TASK_COMMAND_PKT_TYPE;
	tx_buf[1] = RDS_TX_OPCODE;
	pkt_size = 4;

	/*               */
	pkt_size += fm_bop_modify(0x65, 0xFFFF, 0x0010, &tx_buf[pkt_size], TX_BUF_SIZE - pkt_size);	/*                               */
	pkt_size += fm_bop_write(0x9F, 0x0003, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0xA2, 0x0005, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                               */
	pkt_size += fm_bop_write(0xA1, 0x0000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*         */
	/*                                                                                             */
	pkt_size += fm_bop_write(0xA4, pi, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                    */
	/*                */
	pkt_size += fm_bop_rd_until(0xAA, 0x0001, 0x0000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                                         */
	/*                                                                                 */
	for (i = 0; i < 12; i++) {
		pkt_size += fm_bop_write(0xA8, ps[i], &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                  */
	}
	pkt_size += fm_bop_modify(0xA2, 0xFFFF, 0x0002, &tx_buf[pkt_size], TX_BUF_SIZE - pkt_size);	/*                               */
	for (i = 0; i < 12; i++) {
		pkt_size += fm_bop_write(0xA7, (0x0063 + i), &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*               */
		pkt_size += fm_bop_write(0xA8, ps[i], &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                  */
	}
	pkt_size += fm_bop_modify(0xA2, 0xFFFF, 0x0010, &tx_buf[pkt_size], TX_BUF_SIZE - pkt_size);	/*                                  */
	/*                    */
	pkt_size += fm_bop_rd_until(0xAA, 0x0001, 0x0001, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                                     */
	pkt_size += fm_bop_modify(0xA2, 0xFFFD, 0x0000, &tx_buf[pkt_size], TX_BUF_SIZE - pkt_size);	/*                               */
	for (i = 0; i < 12; i++) {
		pkt_size += fm_bop_write(0xA8, ps[i], &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                  */
	}
	for (i = 0; i < (other_rds_cnt * 3); i++) {
		pkt_size += fm_bop_write(0xA8, other_rds[i], &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                  */
	}
	pkt_size += fm_bop_modify(0xA2, 0xFFEF, 0x0000, &tx_buf[pkt_size], TX_BUF_SIZE - pkt_size);	/*                                      */
	pkt_size += fm_bop_write(0x9F, 0x0000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*         */

	tx_buf[2] = (fm_u8) ((pkt_size - 4) & 0x00FF);
	tx_buf[3] = (fm_u8) (((pkt_size - 4) >> 8) & 0x00FF);

	return pkt_size;
}

/*            */
fm_s32 mt6620_off_2_tx_shortANA(fm_u8 *tx_buf, fm_s32 tx_buf_size)
{
	fm_s32 pkt_size = 0;

	if (tx_buf_size < TX_BUF_SIZE) {
		return (-1);
	}

	tx_buf[0] = FM_TASK_COMMAND_PKT_TYPE;
	tx_buf[1] = FM_ENABLE_OPCODE;
	pkt_size = 4;

	pkt_size += fm_bop_write(0x01, 0x4A00, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                                          */
	pkt_size += fm_bop_udelay(30, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0x01, 0x6A00, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                            */
	pkt_size += fm_bop_udelay(50, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0x02, 0x299C, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                                             */
	pkt_size += fm_bop_write(0x01, 0x6B82, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                                             */
	pkt_size += fm_bop_write(0x3B, 0x0500, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                                                                                           */
	pkt_size += fm_bop_write(0x04, 0x0548, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                                                                 */
	pkt_size += fm_bop_write(0x37, 0x2000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                                           */
	pkt_size += fm_bop_write(0x42, 0xC002, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                                           */
	pkt_size += fm_bop_write(0x0A, 0x0060, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                                          */
	pkt_size += fm_bop_write(0x0E, 0x0040, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                                    */
	pkt_size += fm_bop_write(0x0C, 0xA88C, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0x10, 0x1E8D, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x27, 0x0005, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                                               */
	pkt_size += fm_bop_write(0x11, 0x07D8, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                                     */
	pkt_size += fm_bop_write(0x41, 0x0003, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                                                  */
	pkt_size += fm_bop_write(0x08, 0x25B8, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                                       */
	pkt_size += fm_bop_write(0x09, 0x2964, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                           */
	pkt_size += fm_bop_write(0x3F, 0xAD86, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                                           */
	pkt_size += fm_bop_write(0x3A, 0x01EF, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                                                                */
	pkt_size += fm_bop_write(0x3E, 0x3181, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                                            */
	pkt_size += fm_bop_write(0x00, 0x0100, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                              */
	pkt_size += fm_bop_write(0x37, 0x2000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                                                            */
	/*                   */
	pkt_size += fm_bop_write(0x25, 0x040F, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                                   */
	pkt_size += fm_bop_write(0x20, 0x2720, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                                         */
	/*                                     */
	pkt_size += fm_bop_write(0x22, 0x6680, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                                */
	pkt_size += fm_bop_write(0x25, 0x080F, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                                                                        */
	pkt_size += fm_bop_write(0x1E, 0x0863, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                                            */
	pkt_size += fm_bop_udelay(5000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x1E, 0x0865, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                                                  */
	pkt_size += fm_bop_udelay(5000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x1E, 0x0871, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                                                              */
	pkt_size += fm_bop_udelay(100000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*              */
	pkt_size += fm_bop_write(0x2A, 0x1026, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                              */
	/*                   */
	pkt_size += fm_bop_write(0x00, 0x0080, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                     */
	pkt_size += fm_bop_udelay(1, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x1B, 0x0094, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                                          */
	pkt_size += fm_bop_write(0x1B, 0x0095, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                            */
	pkt_size += fm_bop_udelay(200, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x1B, 0x0094, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                          */
	pkt_size += fm_bop_write(0x00, 0x0100, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                                           */
	/*               */
	pkt_size += fm_bop_write(0x01, 0x6B8A, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                               */
	pkt_size += fm_bop_udelay(1, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x00, 0xC100, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                                   */
	pkt_size += fm_bop_write(0x0C, 0xB88C, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                                */
	pkt_size += fm_bop_udelay(3000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x3A, 0x01FF, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                                  */
	/*                       */
	pkt_size += fm_bop_write(0x42, 0xF002, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                                                        */
	pkt_size += fm_bop_write(0x3C, 0xABE9, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                                                                */
	pkt_size += fm_bop_write(0x3D, 0x027E, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                                                         */
	pkt_size += fm_bop_write(0x33, 0x0008, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                                                                         */
	pkt_size += fm_bop_write(0x28, 0xFFFF, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x2E, 0x0020, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                                                                                 */
	pkt_size += fm_bop_write(0x2F, 0x0000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                                                                    */
	pkt_size += fm_bop_write(0x44, 0x006E, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0x46, 0xDC22, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x49, 0x0080, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0x4A, 0x0004, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0x4B, 0x0040, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*          */

	tx_buf[2] = (fm_u8) ((pkt_size - 4) & 0x00FF);
	tx_buf[3] = (fm_u8) (((pkt_size - 4) >> 8) & 0x00FF);

	return pkt_size;
}

/*            */
fm_s32 mt6620_dig_init(fm_u8 *tx_buf, fm_s32 tx_buf_size)
{
	fm_s32 pkt_size = 0;

	if (tx_buf_size < TX_BUF_SIZE) {
		return (-1);
	}

	tx_buf[0] = FM_TASK_COMMAND_PKT_TYPE;
	tx_buf[1] = FM_ENABLE_OPCODE;
	pkt_size = 4;

/*                */
	pkt_size += fm_bop_write(0x64, 0x0000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0x65, 0x0001, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0x68, 0xE100, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_udelay(10000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*             */
	pkt_size += fm_bop_write(0x68, 0xE000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
/*            */
	pkt_size += fm_bop_write(0x9F, 0x0000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0x9E, 0x001C, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0x9C, 0xA540, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
/*              */
	pkt_size += fm_bop_write(0x9F, 0x0000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                             */
	pkt_size += fm_bop_write(0xB8, 0x0001, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0xAB, 0x39B6, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xAC, 0x3C3E, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xAD, 0x0000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0xAE, 0x03C2, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xAF, 0x0001, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0xB1, 0x623D, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xF4, 0x0020, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0xF5, 0xBF16, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xB9, 0x0050, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xBA, 0x00C3, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xBB, 0x0000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0xBC, 0x0000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0xBD, 0x0056, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0xBE, 0x0089, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0xBF, 0x004C, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0xC0, 0x0171, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xC1, 0x002B, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0xC2, 0x001F, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0xC3, 0x0066, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0xC4, 0x00F6, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0xC5, 0x0066, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0xC6, 0x001F, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0xC7, 0x0007, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0xFE, 0x0039, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0xFF, 0x3907, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
/*            */
	pkt_size += fm_bop_write(0x9F, 0x0001, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*                             */
	pkt_size += fm_bop_write(0xC0, 0x076C, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xB7, 0x0004, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0xD8, 0x006A, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0xC8, 0x2828, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xCE, 0x0090, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*          */
	pkt_size += fm_bop_write(0xFE, 0x000F, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0xA2, 0x0100, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xA3, 0x0111, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xA4, 0x0122, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xA5, 0x0135, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xA6, 0x0149, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xA7, 0x015E, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xDB, 0x0174, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xDC, 0x018D, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xC9, 0x01A6, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xCA, 0x01C1, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xCB, 0x01DE, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xCC, 0x01FD, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0xD4, 0x2657, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xA0, 0x85B2, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x9F, 0x0000, &tx_buf[pkt_size], tx_buf_size - pkt_size);	/*         */

	tx_buf[2] = (fm_u8) ((pkt_size - 4) & 0x00FF);
	tx_buf[3] = (fm_u8) (((pkt_size - 4) >> 8) & 0x00FF);

	return pkt_size;
}
