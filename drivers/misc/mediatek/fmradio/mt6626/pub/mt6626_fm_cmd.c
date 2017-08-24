#include <linux/kernel.h>

#include "fm_typedef.h"
#include "fm_dbg.h"
#include "fm_err.h"
#include "fm_rds.h"
#include "fm_cust_cfg.h"

#include "mt6626_fm_reg.h"
#include "mt6626_fm_link.h"
#include "mt6626_fm.h"
#include "mt6626_fm_cmd.h"

static fm_s32 fm_bop_write(fm_u8 addr, fm_u16 value, fm_u8 *buf, fm_s32 size)
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

#if 0
static fm_s32 fm_bop_msleep(fm_u32 value, fm_u8 *buf, fm_s32 size)
{
	if (size < (FM_MSLEEP_BASIC_OP_SIZE + 2)) {
		return (-1);
	}

	if (buf == NULL) {
		return (-2);
	}

	buf[0] = FM_MSLEEP_BASIC_OP;
	buf[1] = FM_MSLEEP_BASIC_OP_SIZE;
	buf[2] = (fm_u8) ((value) & 0x000000FF);
	buf[3] = (fm_u8) ((value >> 8) & 0x000000FF);
	buf[4] = (fm_u8) ((value >> 16) & 0x000000FF);
	buf[5] = (fm_u8) ((value >> 24) & 0x000000FF);

	WCN_DBG(FM_DBG | CHIP, "%02x %02x %02x %02x %02x %02x\n", buf[0], buf[1], buf[2], buf[3],
		buf[4], buf[5]);

	return (FM_MSLEEP_BASIC_OP_SIZE + 2);
}
#endif

/*
                                                                                 
                    
                          
                      
 */
fm_s32 mt6626_pwrup_clock_on(fm_u8 *buf, fm_s32 buf_size)
{
	fm_s32 pkt_size = 0;

	if (buf_size < TX_BUF_SIZE) {
		return (-1);
	}

	buf[0] = FM_TASK_COMMAND_PKT_TYPE;
	buf[1] = FM_ENABLE_OPCODE;
	pkt_size = 4;

	/*                         */
	pkt_size += fm_bop_write(0x60, 0x3000, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x60, 0x3001, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_udelay(3000, &buf[pkt_size], buf_size - pkt_size);	/*           */
	pkt_size += fm_bop_write(0x60, 0x3003, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x60, 0x3007, &buf[pkt_size], buf_size - pkt_size);	/*            */
	/*                                                 */
	pkt_size += fm_bop_modify(0x61, 0xFF63, 0x0000, &buf[pkt_size], buf_size - pkt_size);

	buf[2] = (fm_u8) ((pkt_size - 4) & 0x00FF);
	buf[3] = (fm_u8) (((pkt_size - 4) >> 8) & 0x00FF);

	return pkt_size;
}

/*
                                                                                
                    
                          
                                                         
                                                           
                             
                                                         
                      
 */
fm_s32 mt6626_patch_download(fm_u8 *buf, fm_s32 buf_size, fm_u8 seg_num, fm_u8 seg_id,
			     const fm_u8 *src, fm_s32 seg_len)
{
	fm_s32 pkt_size = 0;
	fm_u8 *dst = NULL;

	if (buf_size < TX_BUF_SIZE) {
		return (-1);
	}

	buf[0] = FM_TASK_COMMAND_PKT_TYPE;
	buf[1] = FM_PATCH_DOWNLOAD_OPCODE;
	pkt_size = 4;

	buf[pkt_size++] = seg_num;
	buf[pkt_size++] = seg_id;

	if (seg_len > (buf_size - pkt_size)) {
		return -1;
	}

	dst = &buf[pkt_size];
	pkt_size += seg_len;

	/*                         */
	while (seg_len--) {
		*dst = *src;
		/*                          */
		src++;
		dst++;
	}

	buf[2] = (fm_u8) ((pkt_size - 4) & 0x00FF);
	buf[3] = (fm_u8) (((pkt_size - 4) >> 8) & 0x00FF);
	WCN_DBG(FM_DBG | CHIP, "%02x %02x %02x %02x %02x %02x %02x\n", buf[0], buf[1], buf[2],
		buf[3], buf[4], buf[5], buf[6]);

	return pkt_size;
}

/*
                                                                               
                    
                          
                                                         
                                                           
                             
                                                         
                      
 */
fm_s32 mt6626_coeff_download(fm_u8 *buf, fm_s32 buf_size, fm_u8 seg_num, fm_u8 seg_id,
			     const fm_u8 *src, fm_s32 seg_len)
{
	fm_s32 pkt_size = 0;
	fm_u8 *dst = NULL;

	if (buf_size < TX_BUF_SIZE) {
		return (-1);
	}

	buf[0] = FM_TASK_COMMAND_PKT_TYPE;
	buf[1] = FM_COEFF_DOWNLOAD_OPCODE;
	pkt_size = 4;

	buf[pkt_size++] = seg_num;
	buf[pkt_size++] = seg_id;

	if (seg_len > (buf_size - pkt_size)) {
		return -1;
	}

	dst = &buf[pkt_size];
	pkt_size += seg_len;

	/*                         */
	while (seg_len--) {
		*dst = *src;
		/*                          */
		src++;
		dst++;
	}

	buf[2] = (fm_u8) ((pkt_size - 4) & 0x00FF);
	buf[3] = (fm_u8) (((pkt_size - 4) >> 8) & 0x00FF);
	WCN_DBG(FM_DBG | CHIP, "%02x %02x %02x %02x %02x %02x %02x\n", buf[0], buf[1], buf[2],
		buf[3], buf[4], buf[5], buf[6]);

	return pkt_size;
}

/*
                                                                                   
                    
                          
                                                         
                                                           
                             
                                                         
                      
 */
fm_s32 mt6626_hwcoeff_download(fm_u8 *buf, fm_s32 buf_size, fm_u8 seg_num, fm_u8 seg_id,
			       const fm_u8 *src, fm_s32 seg_len)
{
	fm_s32 pkt_size = 0;
	fm_u8 *dst = NULL;

	if (buf_size < TX_BUF_SIZE) {
		return (-1);
	}

	buf[0] = FM_TASK_COMMAND_PKT_TYPE;
	buf[1] = FM_HWCOEFF_DOWNLOAD_OPCODE;
	pkt_size = 4;

	buf[pkt_size++] = seg_num;
	buf[pkt_size++] = seg_id;

	if (seg_len > (buf_size - pkt_size)) {
		return -1;
	}

	dst = &buf[pkt_size];
	pkt_size += seg_len;

	/*                         */
	while (seg_len--) {
		*dst = *src;
		/*                          */
		src++;
		dst++;
	}

	buf[2] = (fm_u8) ((pkt_size - 4) & 0x00FF);
	buf[3] = (fm_u8) (((pkt_size - 4) >> 8) & 0x00FF);
	WCN_DBG(FM_DBG | CHIP, "%02x %02x %02x %02x %02x %02x %02x\n", buf[0], buf[1], buf[2],
		buf[3], buf[4], buf[5], buf[6]);

	return pkt_size;
}

/*
                                                                           
                    
                          
                                                         
                                                           
                             
                                                         
                      
 */
fm_s32 mt6626_rom_download(fm_u8 *buf, fm_s32 buf_size, fm_u8 seg_num, fm_u8 seg_id,
			   const fm_u8 *src, fm_s32 seg_len)
{
	fm_s32 pkt_size = 0;
	fm_u8 *dst = NULL;

	if (buf_size < TX_BUF_SIZE) {
		return (-1);
	}

	buf[0] = FM_TASK_COMMAND_PKT_TYPE;
	buf[1] = FM_ROM_DOWNLOAD_OPCODE;
	pkt_size = 4;

	buf[pkt_size++] = seg_num;
	buf[pkt_size++] = seg_id;

	if (seg_len > (buf_size - pkt_size)) {
		return -1;
	}

	dst = &buf[pkt_size];
	pkt_size += seg_len;

	/*                         */
	while (seg_len--) {
		*dst = *src;
		/*                          */
		src++;
		dst++;
	}

	buf[2] = (fm_u8) ((pkt_size - 4) & 0x00FF);
	buf[3] = (fm_u8) (((pkt_size - 4) >> 8) & 0x00FF);
	WCN_DBG(FM_DBG | CHIP, "%02x %02x %02x %02x %02x %02x %02x\n", buf[0], buf[1], buf[2],
		buf[3], buf[4], buf[5], buf[6]);

	return pkt_size;
}

/*
                                                                                             
                    
                          
                      
 */
fm_s32 mt6626_pwrup_digital_init_1(fm_u8 *buf, fm_s32 buf_size)
{
	fm_s32 pkt_size = 0;

	if (buf_size < TX_BUF_SIZE) {
		return (-1);
	}

	buf[0] = FM_TASK_COMMAND_PKT_TYPE;
	buf[1] = FM_ENABLE_OPCODE;
	pkt_size = 4;

	/*                                                        */
	pkt_size += fm_bop_write(0x6A, 0x0020, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x6B, 0x0020, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0xA0, 0xD0B1, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x60, 0x300F, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x61, 0x0003, &buf[pkt_size], buf_size - pkt_size);	/*         */
	pkt_size += fm_bop_write(0x61, 0x0002, &buf[pkt_size], buf_size - pkt_size);	/*         */
	/*                                                                                       */
	/*                                                                                       */

	buf[2] = (fm_u8) ((pkt_size - 4) & 0x00FF);
	buf[3] = (fm_u8) (((pkt_size - 4) >> 8) & 0x00FF);

	return pkt_size;
}

/*
                                                                                             
                    
                          
                      
 */
fm_s32 mt6626_pwrup_digital_init_2(fm_u8 *buf, fm_s32 buf_size)
{
	fm_s32 pkt_size = 0;

	if (buf_size < TX_BUF_SIZE) {
		return (-1);
	}

	buf[0] = FM_TASK_COMMAND_PKT_TYPE;
	buf[1] = FM_ENABLE_OPCODE;
	pkt_size = 4;

	pkt_size += fm_bop_rd_until(0x64, 0x001F, 0x0002, &buf[pkt_size], buf_size - pkt_size);	/*                  */

	buf[2] = (fm_u8) ((pkt_size - 4) & 0x00FF);
	buf[3] = (fm_u8) (((pkt_size - 4) >> 8) & 0x00FF);

	return pkt_size;
}

/*
                                                                     
                    
                          
                      
 */
fm_s32 mt6626_pwrdown(fm_u8 *buf, fm_s32 buf_size)
{
	fm_s32 pkt_size = 0;

	if (buf_size < TX_BUF_SIZE) {
		return (-1);
	}

	buf[0] = FM_TASK_COMMAND_PKT_TYPE;
	buf[1] = FM_ENABLE_OPCODE;
	pkt_size = 4;

	/*                          */
	pkt_size += fm_bop_write(0x60, 0x330F, &buf[pkt_size], buf_size - pkt_size);	/*            */
	/*            */
	pkt_size += fm_bop_write(0x61, 0x0001, &buf[pkt_size], buf_size - pkt_size);	/*            */
	/*                                  */
	pkt_size += fm_bop_modify(0x6E, 0xFFF8, 0x0000, &buf[pkt_size], buf_size - pkt_size);	/*              */
	pkt_size += fm_bop_modify(0x6E, 0xFFF8, 0x0000, &buf[pkt_size], buf_size - pkt_size);	/*              */
	pkt_size += fm_bop_modify(0x6E, 0xFFF8, 0x0000, &buf[pkt_size], buf_size - pkt_size);	/*              */
	/*                   */
	pkt_size += fm_bop_write(0x60, 0x0000, &buf[pkt_size], buf_size - pkt_size);	/*            */
	/*             */
	pkt_size += fm_bop_write(0x60, 0x4000, &buf[pkt_size], buf_size - pkt_size);	/*            */
	pkt_size += fm_bop_write(0x60, 0x0000, &buf[pkt_size], buf_size - pkt_size);	/*            */

	buf[2] = (fm_u8) ((pkt_size - 4) & 0x00FF);
	buf[3] = (fm_u8) (((pkt_size - 4) >> 8) & 0x00FF);

	return pkt_size;
}

/*
                                                         
                    
                          
                      
 */
fm_s32 mt6626_rampdown(fm_u8 *buf, fm_s32 buf_size)
{
	fm_s32 pkt_size = 0;

	if (buf_size < TX_BUF_SIZE) {
		return (-1);
	}

	buf[0] = FM_TASK_COMMAND_PKT_TYPE;
	buf[1] = FM_RAMPDOWN_OPCODE;
	pkt_size = 4;

	/*                 */
	pkt_size += fm_bop_modify(FM_MAIN_CTRL, 0xFFF0, 0x0000, &buf[pkt_size], buf_size - pkt_size);	/*                */
	/*                         */
	pkt_size += fm_bop_modify(FM_MAIN_CTRL, 0xFFFF, RAMP_DOWN, &buf[pkt_size], buf_size - pkt_size);	/*              */
	/*                               */
	pkt_size += fm_bop_rd_until(FM_MAIN_INTR, FM_INTR_STC_DONE, FM_INTR_STC_DONE, &buf[pkt_size], buf_size - pkt_size);	/*                  */
	/*                           */
	pkt_size += fm_bop_modify(FM_MAIN_CTRL, (~RAMP_DOWN), 0x0000, &buf[pkt_size], buf_size - pkt_size);	/*              */
	/*                                                  */
	pkt_size += fm_bop_modify(FM_MAIN_INTR, 0xFFFF, FM_INTR_STC_DONE, &buf[pkt_size], buf_size - pkt_size);	/*              */

	buf[2] = (fm_u8) ((pkt_size - 4) & 0x00FF);
	buf[3] = (fm_u8) (((pkt_size - 4) >> 8) & 0x00FF);

	return pkt_size;
}

/*
                                     
                    
                          
                                  
                      
 */
fm_s32 mt6626_tune(fm_u8 *buf, fm_s32 buf_size, fm_u16 freq)
{
	fm_s32 pkt_size = 0;

	if (buf_size < TX_BUF_SIZE) {
		return (-1);
	}

	freq = (freq - 640) * 2;

	buf[0] = FM_TASK_COMMAND_PKT_TYPE;
	buf[1] = FM_TUNE_OPCODE;
	pkt_size = 4;

	/*                                         */
	pkt_size += fm_bop_modify(FM_CHANNEL_SET, 0xFC00, freq, &buf[pkt_size], buf_size - pkt_size);	/*                                               */
	/*                                            */
	pkt_size += fm_bop_modify(FM_MAIN_CTRL, 0xFFFE, TUNE, &buf[pkt_size], buf_size - pkt_size);	/*                 */
	/*                             */
	pkt_size += fm_bop_rd_until(FM_MAIN_INTR, FM_INTR_STC_DONE, FM_INTR_STC_DONE, &buf[pkt_size], buf_size - pkt_size);	/*                  */
	/*                                                  */
	pkt_size += fm_bop_modify(FM_MAIN_INTR, 0xFFFF, FM_INTR_STC_DONE, &buf[pkt_size], buf_size - pkt_size);	/*              */

	buf[2] = (fm_u8) ((pkt_size - 4) & 0x00FF);
	buf[3] = (fm_u8) (((pkt_size - 4) >> 8) & 0x00FF);

	return pkt_size;
}

/*
                                     
                    
                          
                                    
                                                   
                          
                          
                      
 */
fm_s32 mt6626_seek(fm_u8 *buf, fm_s32 buf_size, fm_u16 seekdir, fm_u16 space, fm_u16 max_freq,
		   fm_u16 min_freq)
{
	fm_s32 pkt_size = 0;

	if (buf_size < TX_BUF_SIZE) {
		return (-1);
	}

	buf[0] = FM_TASK_COMMAND_PKT_TYPE;
	buf[1] = FM_SEEK_OPCODE;
	pkt_size = 4;

	/*                        */
	if (seekdir == 0) {
		pkt_size += fm_bop_modify(FM_MAIN_CFG1, 0xFBFF, 0x0000, &buf[pkt_size], buf_size - pkt_size);	/*                       */
	} else {
		pkt_size += fm_bop_modify(FM_MAIN_CFG1, 0xFFFF, 0x0400, &buf[pkt_size], buf_size - pkt_size);	/*                         */
	}

	/*                              */
	if (space == 1) {
		pkt_size += fm_bop_modify(FM_MAIN_CFG1, 0x8FFF, 0x1000, &buf[pkt_size], buf_size - pkt_size);	/*                                        */
	} else if (space == 2) {
		pkt_size += fm_bop_modify(FM_MAIN_CFG1, 0x8FFF, 0x2000, &buf[pkt_size], buf_size - pkt_size);	/*                                        */
	} else if (space == 4) {
		pkt_size += fm_bop_modify(FM_MAIN_CFG1, 0x8FFF, 0x4000, &buf[pkt_size], buf_size - pkt_size);	/*                                        */
	}
	/*                                                                            */
	pkt_size += fm_bop_modify(FM_MAIN_CFG1, 0xFFFF, 0x0800, &buf[pkt_size], buf_size - pkt_size);	/*                    */
	/*                            */
	max_freq = (max_freq - 640) * 2;
	pkt_size +=
	    fm_bop_modify(FM_MAIN_CFG1, 0xFC00, max_freq, &buf[pkt_size], buf_size - pkt_size);
	/*                            */
	min_freq = (min_freq - 640) * 2;
	pkt_size +=
	    fm_bop_modify(FM_MAIN_CFG2, 0xFC00, min_freq, &buf[pkt_size], buf_size - pkt_size);
	/*                                             */
	pkt_size += fm_bop_modify(FM_MAIN_CTRL, 0xFFFF, SEEK, &buf[pkt_size], buf_size - pkt_size);	/*             */
	/*                             */
	pkt_size += fm_bop_rd_until(FM_MAIN_INTR, FM_INTR_STC_DONE, FM_INTR_STC_DONE, &buf[pkt_size], buf_size - pkt_size);	/*                  */
	/*                                                  */
	pkt_size += fm_bop_modify(FM_MAIN_INTR, 0xFFFF, FM_INTR_STC_DONE, &buf[pkt_size], buf_size - pkt_size);	/*              */

	buf[2] = (fm_u8) ((pkt_size - 4) & 0x00FF);
	buf[3] = (fm_u8) (((pkt_size - 4) >> 8) & 0x00FF);

	return pkt_size;
}

/*
                                     
                    
                          
                                    
                                                   
                          
                          
                      
 */
fm_s32 mt6626_scan(fm_u8 *buf, fm_s32 buf_size, fm_u16 scandir, fm_u16 space, fm_u16 max_freq,
		   fm_u16 min_freq)
{
	fm_s32 pkt_size = 0;

	if (buf_size < TX_BUF_SIZE) {
		return (-1);
	}

	buf[0] = FM_TASK_COMMAND_PKT_TYPE;
	buf[1] = FM_SCAN_OPCODE;
	pkt_size = 4;

	/*                        */
	if (scandir == 0) {
		pkt_size += fm_bop_modify(FM_MAIN_CFG1, 0xFBFF, 0x0000, &buf[pkt_size], buf_size - pkt_size);	/*                       */
	} else {
		pkt_size += fm_bop_modify(FM_MAIN_CFG1, 0xFFFF, 0x0400, &buf[pkt_size], buf_size - pkt_size);	/*                         */
	}

	/*                              */
	if (space == 1) {
		pkt_size += fm_bop_modify(FM_MAIN_CFG1, 0x8FFF, 0x1000, &buf[pkt_size], buf_size - pkt_size);	/*                                        */
	} else if (space == 2) {
		pkt_size += fm_bop_modify(FM_MAIN_CFG1, 0x8FFF, 0x2000, &buf[pkt_size], buf_size - pkt_size);	/*                                        */
	} else if (space == 4) {
		pkt_size += fm_bop_modify(FM_MAIN_CFG1, 0x8FFF, 0x4000, &buf[pkt_size], buf_size - pkt_size);	/*                                        */
	}
	/*                                                                         */
	pkt_size += fm_bop_modify(FM_MAIN_CFG1, 0xF7FF, 0x0000, &buf[pkt_size], buf_size - pkt_size);	/*                       */
	/*                            */
	max_freq = (max_freq - 640) * 2;
	pkt_size +=
	    fm_bop_modify(FM_MAIN_CFG1, 0xFC00, max_freq, &buf[pkt_size], buf_size - pkt_size);
	/*                            */
	min_freq = (min_freq - 640) * 2;
	pkt_size +=
	    fm_bop_modify(FM_MAIN_CFG2, 0xFC00, min_freq, &buf[pkt_size], buf_size - pkt_size);
	/*                                              */
	pkt_size += fm_bop_modify(FM_MAIN_CTRL, 0xFFFF, SCAN, &buf[pkt_size], buf_size - pkt_size);	/*             */
	/*                             */
	pkt_size += fm_bop_rd_until(FM_MAIN_INTR, FM_INTR_STC_DONE, FM_INTR_STC_DONE, &buf[pkt_size], buf_size - pkt_size);	/*                  */
	/*                                                  */
	pkt_size += fm_bop_modify(FM_MAIN_INTR, 0xFFFF, FM_INTR_STC_DONE, &buf[pkt_size], buf_size - pkt_size);	/*              */

	buf[2] = (fm_u8) ((pkt_size - 4) & 0x00FF);
	buf[3] = (fm_u8) (((pkt_size - 4) >> 8) & 0x00FF);

	return pkt_size;
}

fm_s32 mt6626_get_reg(fm_u8 *buf, fm_s32 buf_size, fm_u8 addr)
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

fm_s32 mt6626_set_reg(fm_u8 *buf, fm_s32 buf_size, fm_u8 addr, fm_u16 value)
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
