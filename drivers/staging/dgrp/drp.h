/*
 *
 * Copyright 1999 Digi International (www.digi.com)
 *     Gene Olson  <gene at digi dot com>
 *     James Puzzo <jamesp at digi dot com>
 *     Scott Kilau <scottk at digi dot com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY, EXPRESS OR IMPLIED; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 */

/*                                                                       
                                                 
                                                                        */

#ifndef __DRP_H
#define __DRP_H

#include <linux/types.h>
#include <linux/wait.h>
#include <linux/semaphore.h>
#include <linux/tty.h>


#include "digirp.h"

/*                                                                       
                     
                                                                        */

#define CHAN_MAX	64		/*                        */

#define SEQ_MAX		128		/*                                */
#define SEQ_MASK	(SEQ_MAX-1)	/*                              */

#define TBUF_MAX	4096		/*                               */
#define RBUF_MAX	4096		/*                              */

#define TBUF_MASK	(TBUF_MAX-1)	/*                              */
#define RBUF_MASK	(RBUF_MAX-1)	/*                             */

#define TBUF_LOW	1000		/*                         */

#define UIO_BASE	1000		/*                           */
#define UIO_MIN		2000		/*                                 */
#define UIO_MAX		8100		/*                      */

#define MON_MAX		65536		/*                           */
#define MON_MASK	(MON_MAX-1)	/*                   */

#define DPA_MAX		65536		/*                       */
#define DPA_MASK	(DPA_MAX-1)	/*               */
#define DPA_HIGH_WATER	58000		/*                          
                        
      */

#define IDLE_MAX	(20 * HZ)	/*                        */

#define MAX_DESC_LEN	100		/*                            
                   
      */

#define WRITEBUFLEN	((4096) + 4)    /*                                  */

#define VPDSIZE		512

/*                                                                       
                                     
                                                                         
  
                                                                   
                                                                   
                                                                  
                                                           
  
 */

/*
                                    
  
                                                        
                                                       
                                                     
  
                                            
  
                                                              
                                                 
                       
  
                                                                  
  
                                                               
                                            
  
                                                                   
                                  
  
                                                            
  
 */

#define PORT_NUM(dev)			((dev) & 0x3f)

#define OPEN_CATEGORY(dev)		((((dev) & 0x80) & 0x40))
#define IS_PRINT(dev)			(((dev) & 0xff) >= 0x80)

#define OPEN_WAIT_AVAIL(cat)		(((cat) & 0x40) == 0x000)
#define OPEN_WAIT_CARRIER(cat)		(((cat) & 0x40) == 0x000)
#define OPEN_FORCES_CARRIER(cat)	(((cat) & 0x40) != 0x000)


/*                                                                       
                                                       
                                         
                                                                        */

/*                                                                  */

#define DM_DTR		0x01
#define DM_RTS		0x02
#define DM_RTS_TOGGLE	0x04

#define DM_OUT1		0x04
#define DM_OUT2		0x08

#define DM_CTS		0x10
#define DM_DSR		0x20
#define DM_RI		0x40
#define DM_CD		0x80		/*                      */


/*                                                                       
                        
                                                                        */

#define EV_OPU		0x0001		/*                        */
#define EV_OPS		0x0002		/*                       */
#define EV_OPX		0x0004		/*                        */
#define EV_OPH		0x0008		/*                        */
#define EV_IPU		0x0010		/*                        */
#define EV_IPS		0x0020		/*                              */
#define EV_TXB		0x0040		/*                        */
#define EV_TXI		0x0080		/*                            */
#define EV_TXF		0x0100		/*                               */
#define EV_RXB		0x0200		/*                */


/*                                                                       
                   
                                                                        */

#define CF_CS5		0x0000		/*                  */
#define CF_CS6		0x0010		/*                  */
#define CF_CS7		0x0020		/*                  */
#define CF_CS8		0x0030		/*                  */
#define CF_CSIZE	0x0030		/*                */
#define CF_CSTOPB	0x0040		/*               */
#define CF_CREAD	0x0080		/*                 */
#define CF_PARENB	0x0100		/*               */
#define CF_PARODD	0x0200		/*            */
#define CF_HUPCL	0x0400		/*                   */


/*                                                                       
                   
                                                                        */

#define XF_XPAR		0x0001		/*                          */
#define XF_XMODEM	0x0002		/*                                 */
#define XF_XCASE	0x0004		/*                            */
#define XF_XEDATA	0x0008		/*                      */
#define XF_XTOSS	0x0010		/*                       */
#define XF_XIXON	0x0020		/*                   */


/*                                                                       
                   
                                                                        */

#define IF_IGNBRK	0x0001		/*                    */
#define IF_BRKINT	0x0002		/*                 */
#define IF_IGNPAR	0x0004		/*                         */
#define IF_PARMRK	0x0008		/*                              */
#define IF_INPCK	0x0010		/*                               */
#define IF_ISTRIP	0x0020		/*                              */
#define IF_IXON		0x0400		/*                              */
#define IF_IXANY	0x0800		/*                            */
#define	IF_IXOFF	0x1000		/*                             */
#define IF_DOSMODE	0x8000		/*                         */


/*                                                                       
                   
                                                                        */

#define OF_OLCUC	0x0002		/*                         */
#define OF_ONLCR	0x0004		/*                 */
#define OF_OCRNL	0x0008		/*              */
#define OF_ONOCR	0x0010		/*                          */
#define OF_ONLRET	0x0020		/*                      */
#define OF_TAB3		0x1800		/*                         */
#define OF_TABDLY	0x1800		/*           */

/*                                                                       
                                     
                                                                        */

/*                               */
#define UN_EXCL		0x00010000	/*                */
#define UN_STICKY	0x00020000	/*                             */
#define UN_BUSY		0x00040000	/*                        */
#define UN_PWAIT	0x00080000	/*                              */
#define UN_TIME		0x00100000	/*                 */
#define UN_EMPTY	0x00200000	/*                            */
#define UN_LOW		0x00400000	/*                          */
#define UN_DIGI_MASK	0x00FF0000	/*                          */

/*
                                                               
  
                                                 
  
 */
#define UN_HUP_NOTIFY	0x0001 /*                            
                                
    */
#define UN_FOURPORT	0x0002	/*                                         */
#define UN_SAK		0x0004	/*                                    */
#define UN_SPLIT_TERMIOS 0x0008 /*                                     */

#define UN_SPD_MASK	0x0030
#define UN_SPD_HI	0x0010	/*                                */
#define UN_SPD_VHI	0x0020	/*                                 */
#define UN_SPD_CUST	0x0030	/*                            */

#define UN_SKIP_TEST	0x0040 /*                                         */
#define UN_AUTO_IRQ	0x0080 /*                                           */

#define UN_SESSION_LOCKOUT 0x0100 /*                                     */
#define UN_PGRP_LOCKOUT	   0x0200 /*                                  */
#define UN_CALLOUT_NOHUP   0x0400 /*                                 */

#define UN_FLAGS	0x0FFF	/*                            */
#define UN_USR_MASK	0x0430	/*                                
                             
     */

#define UN_INITIALIZED		0x80000000 /*                             */
#define UN_CALLOUT_ACTIVE	0x40000000 /*                           */
#define UN_NORMAL_ACTIVE	0x20000000 /*                         */
#define UN_BOOT_AUTOCONF	0x10000000 /*                              */
#define UN_CLOSING		0x08000000 /*                        */
#define UN_CTS_FLOW		0x04000000 /*                     */
#define UN_CHECK_CD		0x02000000 /*              */
#define UN_SHARE_IRQ		0x01000000 /*                         */


/*                                                                       
                                                            
  
                                                                  
                          
                                                                        */

struct un_struct {
	struct tty_struct *un_tty;		/*                   */
	struct ch_struct *un_ch;		/*                    */

	ushort     un_open_count;		/*                       */
	int		un_flag;		/*            */
	ushort     un_tbusy;		/*                     */

	wait_queue_head_t  un_open_wait;
	wait_queue_head_t  un_close_wait;
	ushort	un_type;
	struct device *un_sysfs;
};


/*                                                                       
                                      
                                                                        */

/*
                             
  
                                                                       
  
                                                                    
  
                                                               
 */

enum dgrp_ch_state_t {
	CS_IDLE = 0,	    /*                 */
	CS_WAIT_OPEN = 1,   /*                                 */
	CS_WAIT_CANCEL = 2, /*                                   */
	CS_WAIT_FAIL = 3,   /*                                */
	CS_SEND_QUERY = 4,  /*                          */
	CS_WAIT_QUERY = 5,  /*                                 */
	CS_READY = 6,	    /*                                   */
	CS_SEND_CLOSE =	7,  /*                             */
	CS_WAIT_CLOSE =	8   /*                            */
};

/*                                                                       
                                       
                                                                        */

/*
                                                                   
                                                                  
                                                                     
                                                                  
                                                                   
                                              
  
                                                                        
          
  
                                                                         
                                                                      
 */

#define CH_HANGUP	0x00002		/*                            */

#define CH_VIRT_CD	0x00004		/*                               */
#define CH_PHYS_CD	0x00008		/*                                */

#define CH_CLOCAL	0x00010		/*                      */
#define CH_BAUD0	0x00020		/*                       */

#define CH_FAST_READ	0x00040		/*                        */
#define CH_FAST_WRITE	0x00080		/*                         */

#define CH_PRON		0x00100		/*                          */
#define CH_RX_FLUSH	0x00200		/*                       */
#define CH_LOW		0x00400		/*                              */
#define CH_EMPTY	0x00800		/*                          */
#define CH_DRAIN	0x01000		/*                           */
#define CH_INPUT	0x02000		/*                          */
#define CH_RXSTOP	0x04000		/*                      */
#define CH_PARAM	0x08000		/*                         */
#define CH_WAITING_SYNC 0x10000		/*                            
                     
      */
#define CH_PORT_GONE	0x20000		/*                      */
#define CH_TX_BREAK	0x40000		/*                     
                        
      */

/*                                                                       
                                       
                                                                        */

#define OTYPE_IMMEDIATE	  0		/*                */
#define OTYPE_PERSISTENT  1		/*                 */
#define OTYPE_INCOMING	  2		/*               */


/*                                                                       
                          
                                                                        */

#define RR_SEQUENCE	0x0001		/*                       */
#define RR_STATUS	0x0002		/*                       */
#define RR_BUFFER	0x0004		/*                         */
#define RR_CAPABILITY	0x0008		/*                              */

#define RR_TX_FLUSH	0x0040		/*                      */
#define RR_RX_FLUSH	0x0080		/*                     */

#define RR_TX_STOP	0x0100		/*              */
#define RR_RX_STOP	0x0200		/*             */
#define RR_TX_START	0x0400		/*              */
#define RR_RX_START	0x0800		/*             */

#define RR_TX_BREAK	0x1000		/*            */
#define RR_TX_ICHAR	0x2000		/*                          */


/*                                                                       
                                                    
                                                                        */

struct ch_struct {
	struct digi_struct ch_digi;		/*                */
	int	ch_edelay;		/*             */

	struct tty_port port;
	struct un_struct ch_tun;	/*                    */
	struct un_struct ch_pun;	/*                   */

	struct nd_struct *ch_nd;	/*              */
	u8  *ch_tbuf;		/*                       */
	u8  *ch_rbuf;		/*                      */
	ulong	ch_cpstime;		/*                  */
	ulong	ch_waketime;		/*                   */

	ulong	ch_flag;		/*            */

	enum dgrp_ch_state_t ch_state;		/*                     */
	ushort	ch_send;		/*                             */
	ushort	ch_expect;		/*                              */
	ushort	ch_wait_carrier;	/*                                  */
	ushort	ch_wait_count[3];	/*                               */

	ushort	ch_portnum;		/*             */
	ushort	ch_open_count;		/*                       */
	ushort	ch_category;		/*                 */
	ushort	ch_open_error;		/*                        */
	ushort	ch_break_time;		/*                            */
	ushort	ch_cpsrem;		/*                       */
	ushort	ch_ocook;		/*                          */
	ushort	ch_inwait;		/*                             */

	ushort	ch_tin;			/*                              */
	ushort	ch_tout;		/*                               */
	ushort	ch_s_tin;		/*              */
	ushort	ch_s_tpos;		/*               */
	ushort	ch_s_tsize;		/*                */
	ushort	ch_s_treq;		/*               */
	ushort	ch_s_elast;		/*                */

	ushort	ch_rin;			/*                             */
	ushort	ch_rout;		/*                              */
	ushort	ch_s_rin;		/*              */
	/*                                                                    
                                                                        
                                                                      
                                                                    
                                                                     
                                                                        
                                                                
                                                                      
                                                                   
                                 
  */
	ushort	ch_s_rwin;		/*               */
	ushort	ch_s_rsize;		/*                */

	ushort	ch_tmax;		/*            */
	ushort	ch_ttime;		/*             */
	ushort	ch_rmax;		/*            */
	ushort	ch_rtime;		/*             */
	ushort	ch_rlow;		/*            */
	ushort	ch_rhigh;		/*             */

	ushort	ch_s_tmax;		/*               */
	ushort	ch_s_ttime;		/*                */
	ushort	ch_s_rmax;		/*               */
	ushort	ch_s_rtime;		/*                */
	ushort	ch_s_rlow;		/*               */
	ushort	ch_s_rhigh;		/*                */

	ushort	ch_brate;		/*                 */
	ushort	ch_cflag;		/*                  */
	ushort	ch_iflag;		/*                  */
	ushort	ch_oflag;		/*                  */
	ushort	ch_xflag;		/*                  */

	ushort	ch_s_brate;		/*                */
	ushort	ch_s_cflag;		/*                */
	ushort	ch_s_iflag;		/*                */
	ushort	ch_s_oflag;		/*                */
	ushort	ch_s_xflag;		/*                */

	u8	ch_otype;		/*                   */
	u8	ch_pscan_savechar;	/*                                    */
	u8	ch_pscan_state;		/*                                   */
	u8	ch_otype_waiting;	/*                                */
	u8	ch_flush_seq;		/*                            */
	u8	ch_s_mlast;		/*                */

	u8	ch_mout;		/*            */
	u8	ch_mflow;		/*             */
	u8	ch_mctrl;		/*             */
	u8	ch_xon;			/*           */
	u8	ch_xoff;		/*            */
	u8	ch_lnext;		/*             */
	u8	ch_xxon;		/*            */
	u8	ch_xxoff;		/*             */

	u8	ch_s_mout;		/*               */
	u8	ch_s_mflow;		/*                */
	u8	ch_s_mctrl;		/*                */
	u8	ch_s_xon;		/*              */
	u8	ch_s_xoff;		/*               */
	u8	ch_s_lnext;		/*                */
	u8	ch_s_xxon;		/*               */
	u8	ch_s_xxoff;		/*                */

	wait_queue_head_t ch_flag_wait;	/*                                */
	wait_queue_head_t ch_sleep;	/*                           */

	int	ch_custom_speed;	/*                       */
	int	ch_txcount;		/*                  */
	int	ch_rxcount;		/*                  */
};


/*                                                                       
                          
                                                                        */

enum dgrp_nd_state_t {
	NS_CLOSED = 0,	   /*                          */
	NS_IDLE = 1,	   /*                             */
	NS_SEND_QUERY =	2, /*                   */
	NS_WAIT_QUERY =	3, /*                         */
	NS_READY = 4,	   /*               */
	NS_SEND_ERROR =	5  /*                        */
};

#define ND_STATE_STR(x) \
	((x) == NS_CLOSED     ? "CLOSED"     : \
	((x) == NS_IDLE	      ? "IDLE"	     : \
	((x) == NS_SEND_QUERY ? "SEND_QUERY" : \
	((x) == NS_WAIT_QUERY ? "WAIT_QUERY" : \
	((x) == NS_READY      ? "READY"	     : \
	((x) == NS_SEND_ERROR ? "SEND_ERROR" : "UNKNOWN"))))))

/*                                                                       
                         
                                                                        */

#define ND_SELECT	0x0001		/*                           */
#define ND_DEB_WAIT	0x0002		/*                      */


/*                                                                       
                               
                                                                        */

#define MON_WAIT_DATA	0x0001		/*                         */
#define MON_WAIT_SPACE	0x0002		/*                          */

/*                                                                       
                        
                                                                        */

#define DPA_WAIT_DATA	0x0001		/*                         */
#define DPA_WAIT_SPACE	0x0002		/*                          */


/*                                                                       
                                        
                                                                        */

#define RPDUMP_MAGIC	"Digi-RealPort-1.0"

#define RPDUMP_MESSAGE	0xE2		/*                     */
#define RPDUMP_RESET	0xE7		/*                  */
#define RPDUMP_CLIENT	0xE8		/*             */
#define RPDUMP_SERVER	0xE9		/*             */


/*                                                                       
                                     
                                                                        */

#define NR_ECHO		0x0001		/*                    */
#define NR_IDENT	0x0002		/*                   */
#define NR_CAPABILITY	0x0004		/*                    */
#define NR_VPD		0x0008		/*                    */
#define NR_PASSWORD	0x0010		/*                 */

/*                                                                       
                                                                        
                                                                        */
#define SERIAL_TTDRV_REG   0x0001     /*                               */
#define CALLOUT_TTDRV_REG  0x0002     /*                                */
#define XPRINT_TTDRV_REG   0x0004     /*                               */


/*                                                                       
                                                                 
                   
                                                                        */

struct nd_struct {
	struct list_head	list;
	long	      nd_major;		   /*                         */
	long	      nd_ID;		   /*                     */

	char	      nd_serial_name[50];   /*                             */
	char	      nd_callout_name[50];  /*                            */
	char	      nd_xprint_name[50];   /*                            */

	char	     password[16];	  /*                                */
	int	     nd_tty_ref_cnt;	  /*                              */

	struct proc_dir_entry *nd_net_de; /*                               */
	struct proc_dir_entry *nd_mon_de; /*                               */
	struct proc_dir_entry *nd_ports_de; /*                               */
	struct proc_dir_entry *nd_dpa_de; /*                               */

	spinlock_t nd_lock;		  /*                       */

	struct semaphore nd_net_semaphore; /*                         */
	struct semaphore nd_mon_semaphore; /*                         */
	spinlock_t nd_dpa_lock;		/*                      */

	enum dgrp_nd_state_t nd_state;	  /*                    */
	int	      nd_chan_count;	   /*                        */
	int	      nd_flag;		   /*                 */
	int	      nd_send;		   /*                        */
	int	      nd_expect;	   /*                         */

	u8	 *nd_iobuf;	       /*                     */
	wait_queue_head_t nd_tx_waitq;	  /*                              */

	u8	 *nd_inputbuf;	       /*                */
	u8	 *nd_inputflagbuf;     /*                     */

	int	      nd_tx_deposit;	   /*                               */
	int	      nd_tx_charge;	   /*                               */
	int	      nd_tx_credit;	   /*                        */
	int	      nd_tx_ready;	   /*                        */
	int	      nd_tx_work;	   /*                      */
	ulong	     nd_tx_time;	  /*                        */
	ulong	     nd_poll_time;	  /*                             */

	int	      nd_delay;		   /*                       */
	int	      nd_rate;		   /*                      */
	struct link_struct nd_link;		/*                      */

	int	      nd_seq_in;	   /*                    */
	int	      nd_seq_out;	   /*                     */
	int	      nd_unack;		   /*                               */
	int	      nd_remain;	   /*                             */
	int	      nd_tx_module;	   /*                         */
	int	      nd_rx_module;	   /*                         */
	char	     *nd_error;		   /*                            */

	int	      nd_write_count;	   /*                            */
	int	      nd_read_count;	   /*                       */
	int	      nd_send_count;	   /*                       */
	int	      nd_tx_byte;	   /*                         */
	int	      nd_rx_byte;	   /*                        */

	ulong	     nd_mon_lbolt;	 /*                        */
	int	      nd_mon_flag;	  /*                    */
	int	      nd_mon_in;	  /*                         */
	int	      nd_mon_out;	  /*                         */
	wait_queue_head_t nd_mon_wqueue;  /*                                */
	u8	 *nd_mon_buf;	      /*                  */

	ulong	     nd_dpa_lbolt;	/*                      */
	int	     nd_dpa_flag;	/*                  */
	int	     nd_dpa_in;		/*                      */
	int	     nd_dpa_out;	/*                       */
	wait_queue_head_t nd_dpa_wqueue; /*                            */
	u8	  *nd_dpa_buf;	/*                   */

	uint	     nd_dpa_debug;
	uint	     nd_dpa_port;

	wait_queue_head_t nd_seq_wque[SEQ_MAX];	  /*                       */
	u8	  nd_seq_wait[SEQ_MAX];	  /*                            */

	ushort	     nd_seq_size[SEQ_MAX];   /*                            */
	ulong	     nd_seq_time[SEQ_MAX];   /*                            */

	ushort	     nd_hw_ver;		  /*                               */
	ushort	     nd_sw_ver;		  /*                               */
	uint	     nd_hw_id;		  /*                           */
	u8	  nd_ps_desc[MAX_DESC_LEN+1];  /*                     */
	uint	     nd_vpd_len;		/*                 */
	u8	     nd_vpd[VPDSIZE];		/*             */

	ulong	     nd_ttdriver_flags;	  /*                         */
	struct tty_driver *nd_serial_ttdriver;	/*                           */
	struct tty_driver *nd_callout_ttdriver; /*                           */
	struct tty_driver *nd_xprint_ttdriver;	/*                           */

	u8	     *nd_writebuf;		/*                        
                  
       */
	struct ch_struct nd_chan[CHAN_MAX];  /*                    */
	struct device *nd_class_dev;	/*                                  */
};

#endif /*         */
