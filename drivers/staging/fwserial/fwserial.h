#ifndef _FIREWIRE_FWSERIAL_H
#define _FIREWIRE_FWSERIAL_H

#include <linux/kernel.h>
#include <linux/tty.h>
#include <linux/tty_driver.h>
#include <linux/tty_flip.h>
#include <linux/list.h>
#include <linux/firewire.h>
#include <linux/firewire-constants.h>
#include <linux/spinlock.h>
#include <linux/rcupdate.h>
#include <linux/mutex.h>
#include <linux/serial.h>
#include <linux/serial_reg.h>
#include <linux/module.h>
#include <linux/seq_file.h>
#include <linux/debugfs.h>

#include "dma_fifo.h"

#ifdef FWTTY_PROFILING
#define DISTRIBUTION_MAX_SIZE     8192
#define DISTRIBUTION_MAX_INDEX    (ilog2(DISTRIBUTION_MAX_SIZE) + 1)
static inline void profile_size_distrib(unsigned stat[], unsigned val)
{
	int n = (val) ? min(ilog2(val) + 1, DISTRIBUTION_MAX_INDEX) : 0;
	++stat[n];
}
#else
#define DISTRIBUTION_MAX_INDEX    0
#define profile_size_distrib(st, n)
#endif

/*                                                                      */
struct virt_plug_params {
	__be32  status_hi;
	__be32  status_lo;
	__be32	fifo_hi;
	__be32	fifo_lo;
	__be32	fifo_len;
};

struct peer_work_params {
	union {
		struct virt_plug_params plug_req;
	};
};

/* 
                                                                 
                                             
                                                          
                                                 
                                          
                                
                                                                 
                                                       
                                                          
                                                        
                                                       
                                                       
                                       
                                                               
                                                          
                                                                  
                                                                      
                                                   
                                                                     
                        
                                          
                                                       
                                                                
 */
struct fwtty_peer {
	struct fw_unit		*unit;
	struct fw_serial	*serial;
	u64			guid;
	int			generation;
	int			node_id;
	unsigned		speed;
	int			max_payload;
	u64			mgmt_addr;

	/*                                                 */
	u64			status_addr;
	u64			fifo_addr;
	int			fifo_len;

	struct list_head	list;
	struct rcu_head		rcu;

	spinlock_t		lock;
	struct work_struct	work;
	struct peer_work_params work_params;
	struct timer_list	timer;
	int			state;
	struct delayed_work	connect;
	int			connect_retries;

	struct fwtty_port	*port;
};

#define to_peer(ptr, field)	(container_of(ptr, struct fwtty_peer, field))

/*                                         */
enum fwtty_peer_state {
	FWPS_GONE,
	FWPS_NOT_ATTACHED,
	FWPS_ATTACHED,
	FWPS_PLUG_PENDING,
	FWPS_PLUG_RESPONDING,
	FWPS_UNPLUG_PENDING,
	FWPS_UNPLUG_RESPONDING,

	FWPS_NO_MGMT_ADDR = -1,
};

#define CONNECT_RETRY_DELAY	HZ
#define MAX_CONNECT_RETRIES	10

/*                                                    */
static inline void peer_set_state(struct fwtty_peer *peer, int new)
{
	peer->state = new;
}

static inline struct fwtty_port *peer_revert_state(struct fwtty_peer *peer)
{
	struct fwtty_port *port = peer->port;

	peer->port = NULL;
	peer_set_state(peer, FWPS_NOT_ATTACHED);
	return port;
}

struct fwserial_mgmt_pkt {
	struct {
		__be16		len;
		__be16		code;
	} hdr;
	union {
		struct virt_plug_params plug_req;
		struct virt_plug_params plug_rsp;
	};
} __packed;

/*                            */
#define FWSC_RSP_OK			0x0000
#define FWSC_RSP_NACK			0x8000
#define FWSC_CODE_MASK			0x0fff

#define FWSC_VIRT_CABLE_PLUG		1
#define FWSC_VIRT_CABLE_UNPLUG		2
#define FWSC_VIRT_CABLE_PLUG_RSP	3
#define FWSC_VIRT_CABLE_UNPLUG_RSP	4

/*                                                            */
#define VIRT_CABLE_PLUG_TIMEOUT		(60 * HZ)

struct stats {
	unsigned	xchars;
	unsigned	dropped;
	unsigned	tx_stall;
	unsigned	fifo_errs;
	unsigned	sent;
	unsigned	lost;
	unsigned	throttled;
	unsigned	watermark;
	unsigned	reads[DISTRIBUTION_MAX_INDEX + 1];
	unsigned	writes[DISTRIBUTION_MAX_INDEX + 1];
	unsigned	txns[DISTRIBUTION_MAX_INDEX + 1];
	unsigned	unthrottle[DISTRIBUTION_MAX_INDEX + 1];
};

struct fwconsole_ops {
	void (*notify)(int code, void *data);
	void (*stats)(struct stats *stats, void *data);
	void (*proc_show)(struct seq_file *m, void *data);
};

/*                              */
#define FWCON_NOTIFY_ATTACH		1
#define FWCON_NOTIFY_DETACH		2

struct buffered_rx {
	struct list_head	list;
	size_t			n;
	unsigned char		data[0];
};

/* 
                                                                    
                             
                      
                                                         
                                                                      
                                                    
                                                                          
                                                              
                                                   
                                                   
                                         
                                                                   
                                                                        
                                            
                                                                  
                                                          
                                                                    
                                            
                                                 
                                         
                                      
                          
                                                    
                                                                      
                                      
                                                 
                                      
                               
                                      
                                                                    
                                                                      
                                                                 
                                                            
                                                                
                                                                  
                                
                                                         
                                       
                                                                   
                                                                              
                                                                        
                                                                               
                                                        
                                
                                                           
                                              
                           
                                                                              
                                    
                                                          
                                                            
                                  
                                                                   
                                                                             
 */
struct fwtty_port {
	struct tty_port		   port;
	struct device		   *device;
	unsigned		   index;
	struct fw_serial	   *serial;
	struct fw_address_handler  rx_handler;

	struct fwconsole_ops	   *fwcon_ops;
	void			   *con_data;

	wait_queue_head_t	   wait_tx;
	struct delayed_work	   emit_breaks;
	unsigned		   cps;
	unsigned long		   break_last;

	struct work_struct	   hangup;

	unsigned		   mstatus;

	spinlock_t		   lock;
	unsigned		   mctrl;
	struct delayed_work	   drain;
	struct work_struct	   push;
	struct list_head	   buf_list;
	int			   buffered;
	struct dma_fifo		   tx_fifo;
	int			   max_payload;
	unsigned		   status_mask;
	unsigned		   ignore_mask;
	unsigned		   break_ctl:1,
				   write_only:1,
				   overrun:1,
				   loopback:1;
	unsigned long		   flags;

	struct fwtty_peer __rcu	   *peer;

	struct async_icount	   icount;
	struct stats		   stats;
};

#define to_port(ptr, field)	(container_of(ptr, struct fwtty_port, field))

/*                        */
#define IN_TX                      0
#define STOP_TX                    1
#define BUFFERING_RX		   2

/*                                         */
#define OOB_RX_THROTTLE   0x00010000
#define MCTRL_RSRVD       0x000e0000
#define OOB_TX_THROTTLE   0x00100000
#define MSTATUS_RSRVD     0x00e00000

#define MCTRL_MASK        (TIOCM_DTR | TIOCM_RTS | TIOCM_OUT1 | TIOCM_OUT2 | \
			   TIOCM_LOOP | OOB_RX_THROTTLE | MCTRL_RSRVD)

/*                                                           */
/*                                    */
#define FREQ_BREAKS        (HZ / 50)

/*                                                             */
#define MAX_CARD_PORTS           32	/*                         */
#define MAX_TOTAL_PORTS          64	/*                         */

/*                   */
#define FWTTY_PORT_TXFIFO_LEN	4096
#define FWTTY_PORT_MAX_PEND_DMA    8    /*                             */
#define DRAIN_THRESHOLD         1024
#define MAX_ASYNC_PAYLOAD       4096    /*                             */
#define WRITER_MINIMUM           128
/*                                                               */
#define HIGH_WATERMARK         32768	/*                          */

/*
                                                                   
                                                     
 */
#define FWTTY_PORT_RXFIFO_LEN	MAX_ASYNC_PAYLOAD

/* 
                                                                         
                                                                         
                                                                        
                                                                        
                         
                                               
                                                                        
                                                                  
 */
struct fw_serial {
	struct fw_card	  *card;
	struct kref	  kref;

	struct dentry	  *debugfs;
	struct fwtty_peer *self;

	struct list_head  list;
	struct list_head  peer_list;

	struct fwtty_port *ports[MAX_CARD_PORTS];
};

#define to_serial(ptr, field)	(container_of(ptr, struct fw_serial, field))

#define TTY_DEV_NAME		    "fwtty"	/*                           */
static const char tty_dev_name[] =  TTY_DEV_NAME;
static const char loop_dev_name[] = "fwloop";

extern struct tty_driver *fwtty_driver;

#define driver_err(s, v...)	pr_err(KBUILD_MODNAME ": " s, ##v)

struct fwtty_port *fwtty_port_get(unsigned index);
void fwtty_port_put(struct fwtty_port *port);

static inline void fwtty_bind_console(struct fwtty_port *port,
				      struct fwconsole_ops *fwcon_ops,
				      void *data)
{
	port->con_data = data;
	port->fwcon_ops = fwcon_ops;
}

/*
                                                                            
                                                                              
                                              
                                                                           
                                      
                                            
 */
static inline int link_speed_to_max_payload(unsigned speed)
{
	/*                                                                  */
	return min(512 << speed, 4096);
}

#endif /*                      */
