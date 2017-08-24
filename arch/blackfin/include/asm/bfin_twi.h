/*
 * bfin_twi.h - interface to Blackfin TWIs
 *
 * Copyright 2005-2010 Analog Devices Inc.
 *
 * Licensed under the GPL-2 or later.
 */

#ifndef __ASM_BFIN_TWI_H__
#define __ASM_BFIN_TWI_H__

#include <linux/types.h>
#include <linux/i2c.h>

/*
                                                                     
                                                                    
 */
#define __BFP(m) u16 m; u16 __pad_##m

/*
                            
 */
struct bfin_twi_regs {
	__BFP(clkdiv);
	__BFP(control);
	__BFP(slave_ctl);
	__BFP(slave_stat);
	__BFP(slave_addr);
	__BFP(master_ctl);
	__BFP(master_stat);
	__BFP(master_addr);
	__BFP(int_stat);
	__BFP(int_mask);
	__BFP(fifo_ctl);
	__BFP(fifo_stat);
	u32 __pad[20];
	__BFP(xmt_data8);
	__BFP(xmt_data16);
	__BFP(rcv_data8);
	__BFP(rcv_data16);
};

#undef __BFP

struct bfin_twi_iface {
	int			irq;
	spinlock_t		lock;
	char			read_write;
	u8			command;
	u8			*transPtr;
	int			readNum;
	int			writeNum;
	int			cur_mode;
	int			manual_stop;
	int			result;
	struct i2c_adapter	adap;
	struct completion	complete;
	struct i2c_msg		*pmsg;
	int			msg_num;
	int			cur_msg;
	u16			saved_clkdiv;
	u16			saved_control;
	struct bfin_twi_regs __iomem *regs_base;
};

#define DEFINE_TWI_REG(reg_name, reg) \
static inline u16 read_##reg_name(struct bfin_twi_iface *iface) \
	{ return bfin_read16(&iface->regs_base->reg); } \
static inline void write_##reg_name(struct bfin_twi_iface *iface, u16 v) \
	{ bfin_write16(&iface->regs_base->reg, v); }

DEFINE_TWI_REG(CLKDIV, clkdiv)
DEFINE_TWI_REG(CONTROL, control)
DEFINE_TWI_REG(SLAVE_CTL, slave_ctl)
DEFINE_TWI_REG(SLAVE_STAT, slave_stat)
DEFINE_TWI_REG(SLAVE_ADDR, slave_addr)
DEFINE_TWI_REG(MASTER_CTL, master_ctl)
DEFINE_TWI_REG(MASTER_STAT, master_stat)
DEFINE_TWI_REG(MASTER_ADDR, master_addr)
DEFINE_TWI_REG(INT_STAT, int_stat)
DEFINE_TWI_REG(INT_MASK, int_mask)
DEFINE_TWI_REG(FIFO_CTL, fifo_ctl)
DEFINE_TWI_REG(FIFO_STAT, fifo_stat)
DEFINE_TWI_REG(XMT_DATA8, xmt_data8)
DEFINE_TWI_REG(XMT_DATA16, xmt_data16)
#if !ANOMALY_16000030
DEFINE_TWI_REG(RCV_DATA8, rcv_data8)
DEFINE_TWI_REG(RCV_DATA16, rcv_data16)
#else
static inline u16 read_RCV_DATA8(struct bfin_twi_iface *iface)
{
	u16 ret;
	unsigned long flags;

	flags = hard_local_irq_save();
	ret = bfin_read16(&iface->regs_base->rcv_data8);
	hard_local_irq_restore(flags);

	return ret;
}

static inline u16 read_RCV_DATA16(struct bfin_twi_iface *iface)
{
	u16 ret;
	unsigned long flags;

	flags = hard_local_irq_save();
	ret = bfin_read16(&iface->regs_base->rcv_data16);
	hard_local_irq_restore(flags);

	return ret;
}
#endif


/*                                                                              */
/*                                                                  */
#define	CLKLOW(x)	((x) & 0xFF)	/*                                              */
#define CLKHI(y)	(((y)&0xFF)<<0x8)	/*                                              */

/*                                  */
#define	PRESCALE	0x007F	/*                                              */
#define	TWI_ENA		0x0080	/*                                                                              */
#define	SCCB		0x0200	/*                                                              */

/*                                   */
#define	SEN			0x0001	/*                                                                              */
#define	SADD_LEN	0x0002	/*                                                                      */
#define	STDVAL		0x0004	/*                                                              */
#define	NAK			0x0008	/*                                              */
#define	GEN			0x0010	/*                                                      */

/*                                    */
#define	SDIR		0x0001	/*                                              */
#define GCALL		0x0002	/*                                                                      */

/*                                  */
#define	MEN			0x0001	/*                                                              */
#define	MADD_LEN	0x0002	/*                                                              */
#define	MDIR		0x0004	/*                                              */
#define	FAST		0x0008	/*                                                      */
#define	STOP		0x0010	/*                                                              */
#define	RSTART		0x0020	/*                                              */
#define	DCNT		0x3FC0	/*                                                              */
#define	SDAOVR		0x4000	/*                                                              */
#define	SCLOVR		0x8000	/*                                                              */

/*                                    */
#define	MPROG		0x0001	/*                                                              */
#define	LOSTARB		0x0002	/*                                              */
#define	ANAK		0x0004	/*                                                                      */
#define	DNAK		0x0008	/*                                                                      */
#define	BUFRDERR	0x0010	/*                                                                      */
#define	BUFWRERR	0x0020	/*                                                                      */
#define	SDASEN		0x0040	/*                                                                      */
#define	SCLSEN		0x0080	/*                                                                      */
#define	BUSBUSY		0x0100	/*                                                                      */

/*                                           */
#define	SINIT		0x0001	/*                              */
#define	SCOMP		0x0002	/*                              */
#define	SERR		0x0004	/*                              */
#define	SOVF		0x0008	/*                                      */
#define	MCOMP		0x0010	/*                              */
#define	MERR		0x0020	/*                              */
#define	XMTSERV		0x0040	/*                              */
#define	RCVSERV		0x0080	/*                              */

/*                                */
#define	XMTFLUSH	0x0001	/*                                              */
#define	RCVFLUSH	0x0002	/*                                              */
#define	XMTINTLEN	0x0004	/*                                      */
#define	RCVINTLEN	0x0008	/*                                      */

/*                                   */
#define	XMTSTAT		0x0003	/*                                                                      */
#define	XMT_EMPTY	0x0000	/*                                                                              */
#define	XMT_HALF	0x0001	/*                                                              */
#define	XMT_FULL	0x0003	/*                                                      */

#define	RCVSTAT		0x000C	/*                                                                      */
#define	RCV_EMPTY	0x0000	/*                                                                              */
#define	RCV_HALF	0x0004	/*                                                              */
#define	RCV_FULL	0x000C	/*                                                              */

#endif
