/******************************************************************************
 *
 * Copyright(c) 2003 - 2013 Intel Corporation. All rights reserved.
 *
 * Portions of this file are derived from the ipw3945 project, as well
 * as portions of the ieee80211 subsystem header files.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 * The full GNU General Public License is included in this distribution in the
 * file called LICENSE.
 *
 * Contact Information:
 *  Intel Linux Wireless <ilw@linux.intel.com>
 * Intel Corporation, 5200 N.E. Elam Young Parkway, Hillsboro, OR 97124-6497
 *
 *****************************************************************************/
#ifndef __iwl_trans_int_pcie_h__
#define __iwl_trans_int_pcie_h__

#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/skbuff.h>
#include <linux/wait.h>
#include <linux/pci.h>
#include <linux/timer.h>

#include "iwl-fh.h"
#include "iwl-csr.h"
#include "iwl-trans.h"
#include "iwl-debug.h"
#include "iwl-io.h"
#include "iwl-op-mode.h"

struct iwl_host_cmd;

/*                                                           
                    */

struct iwl_rx_mem_buffer {
	dma_addr_t page_dma;
	struct page *page;
	struct list_head list;
};

/* 
                                               
  
 */
struct isr_statistics {
	u32 hw;
	u32 sw;
	u32 err_code;
	u32 sch;
	u32 alive;
	u32 rfkill;
	u32 ctkill;
	u32 wakeup;
	u32 rx;
	u32 tx;
	u32 unhandled;
};

/* 
                            
                                                                      
                                                                     
         
          
                                                    
                                                   
                                                          
                 
                                      
                                           
                                                                    
                                                      
                                                     
         
  
                                                                       
 */
struct iwl_rxq {
	__le32 *bd;
	dma_addr_t bd_dma;
	struct iwl_rx_mem_buffer pool[RX_QUEUE_SIZE + RX_FREE_BUFFERS];
	struct iwl_rx_mem_buffer *queue[RX_QUEUE_SIZE];
	u32 read;
	u32 write;
	u32 free_count;
	u32 write_actual;
	struct list_head rx_free;
	struct list_head rx_used;
	int need_update;
	struct iwl_rb_status *rb_stts;
	dma_addr_t rb_stts_dma;
	spinlock_t lock;
};

struct iwl_dma_ptr {
	dma_addr_t dma;
	void *addr;
	size_t size;
};

/* 
                                                                     
                          
                                                                 
 */
static inline int iwl_queue_inc_wrap(int index, int n_bd)
{
	return ++index & (n_bd - 1);
}

/* 
                                                               
                          
                                                                 
 */
static inline int iwl_queue_dec_wrap(int index, int n_bd)
{
	return --index & (n_bd - 1);
}

struct iwl_cmd_meta {
	/*                                                     */
	struct iwl_host_cmd *source;
	u32 flags;
};

/*
                          
  
                                             
  
                                                              
                                                                
                                                              
                                                                 
                                                                
                                                                
                                                                  
                                                               
                                                                
                             
                                                
                                                                    
                                                        
                                                              
                                                
 */
struct iwl_queue {
	int n_bd;              /*                             */
	int write_ptr;       /*                                */
	int read_ptr;         /*                               */
	/*                                                   */
	dma_addr_t dma_addr;   /*                        */
	int n_window;	       /*                   */
	u32 id;
	int low_mark;	       /*                                    
                           */
	int high_mark;         /*                                   
                           */
};

#define TFD_TX_CMD_SLOTS 256
#define TFD_CMD_SLOTS 32

/*
                                                          
                                                          
                                                         
                                                        
                                                          
                                                          
                                         
 */
#define IWL_HCMD_SCRATCHBUF_SIZE	16

struct iwl_pcie_txq_entry {
	struct iwl_device_cmd *cmd;
	struct sk_buff *skb;
	/*                                        */
	const void *free_buf;
	struct iwl_cmd_meta meta;
};

struct iwl_pcie_txq_scratch_buf {
	struct iwl_cmd_header hdr;
	u8 buf[8];
	__le32 scratch;
};

/* 
                                    
                                     
                                                 
                                                                         
                                                                      
                                
                                                          
                                            
                    
                                                     
                                                     
                                                          
                                     
  
                                                                             
                                                
 */
struct iwl_txq {
	struct iwl_queue q;
	struct iwl_tfd *tfds;
	struct iwl_pcie_txq_scratch_buf *scratchbufs;
	dma_addr_t scratchbufs_dma;
	struct iwl_pcie_txq_entry *entries;
	spinlock_t lock;
	struct timer_list stuck_timer;
	struct iwl_trans_pcie *trans_pcie;
	u8 need_update;
	u8 active;
};

static inline dma_addr_t
iwl_pcie_get_scratchbuf_dma(struct iwl_txq *txq, int idx)
{
	return txq->scratchbufs_dma +
	       sizeof(struct iwl_pcie_txq_scratch_buf) * idx;
}

/* 
                                                       
                              
                                                                            
                            
                                                
                                                      
                                                                 
                         
                                           
                                         
                                                                   
                                                
                                            
                                    
                                       
                                                                            
                                                     
                                                
                                        
 */
struct iwl_trans_pcie {
	struct iwl_rxq rxq;
	struct work_struct rx_replenish;
	struct iwl_trans *trans;
	struct iwl_drv *drv;

	/*               */
	__le32 *ict_tbl;
	dma_addr_t ict_tbl_dma;
	int ict_index;
	u32 inta;
	bool use_ict;
	struct isr_statistics isr_stats;

	spinlock_t irq_lock;
	u32 inta_mask;
	u32 scd_base_addr;
	struct iwl_dma_ptr scd_bc_tbls;
	struct iwl_dma_ptr kw;

	struct iwl_txq *txq;
	unsigned long queue_used[BITS_TO_LONGS(IWL_MAX_HW_QUEUES)];
	unsigned long queue_stopped[BITS_TO_LONGS(IWL_MAX_HW_QUEUES)];

	/*                      */
	struct pci_dev *pci_dev;
	void __iomem *hw_base;

	bool ucode_write_complete;
	wait_queue_head_t ucode_write_waitq;
	wait_queue_head_t wait_command_queue;

	unsigned long status;
	u8 cmd_queue;
	u8 cmd_fifo;
	u8 n_no_reclaim_cmds;
	u8 no_reclaim_cmds[MAX_NO_RECLAIM_CMDS];

	bool rx_buf_size_8k;
	bool bc_table_dword;
	u32 rx_page_order;

	const char **command_names;

	/*                */
	unsigned long wd_timeout;

	/*                    */
	spinlock_t reg_lock;
};

/* 
                                                     
                                                         
                                         
                                                                      
                                              
                                                           
                                             
 */
enum iwl_pcie_status {
	STATUS_HCMD_ACTIVE,
	STATUS_DEVICE_ENABLED,
	STATUS_TPOWER_PMI,
	STATUS_INT_ENABLED,
	STATUS_RFKILL,
	STATUS_FW_ERROR,
};

#define IWL_TRANS_GET_PCIE_TRANS(_iwl_trans) \
	((struct iwl_trans_pcie *) ((_iwl_trans)->trans_specific))

static inline struct iwl_trans *
iwl_trans_pcie_get_trans(struct iwl_trans_pcie *trans_pcie)
{
	return container_of((void *)trans_pcie, struct iwl_trans,
			    trans_specific);
}

/*
                                                      
                                
 */
struct iwl_trans *iwl_trans_pcie_alloc(struct pci_dev *pdev,
				       const struct pci_device_id *ent,
				       const struct iwl_cfg *cfg);
void iwl_trans_pcie_free(struct iwl_trans *trans);

/*                                                    
    
                                                     */
int iwl_pcie_rx_init(struct iwl_trans *trans);
irqreturn_t iwl_pcie_irq_handler(int irq, void *dev_id);
int iwl_pcie_rx_stop(struct iwl_trans *trans);
void iwl_pcie_rx_free(struct iwl_trans *trans);

/*                                                    
                          
                                                     */
irqreturn_t iwl_pcie_isr_ict(int irq, void *data);
int iwl_pcie_alloc_ict(struct iwl_trans *trans);
void iwl_pcie_free_ict(struct iwl_trans *trans);
void iwl_pcie_reset_ict(struct iwl_trans *trans);
void iwl_pcie_disable_ict(struct iwl_trans *trans);

/*                                                    
           
                                                     */
int iwl_pcie_tx_init(struct iwl_trans *trans);
void iwl_pcie_tx_start(struct iwl_trans *trans, u32 scd_base_addr);
int iwl_pcie_tx_stop(struct iwl_trans *trans);
void iwl_pcie_tx_free(struct iwl_trans *trans);
void iwl_trans_pcie_txq_enable(struct iwl_trans *trans, int txq_id, int fifo,
			       int sta_id, int tid, int frame_limit, u16 ssn);
void iwl_trans_pcie_txq_disable(struct iwl_trans *trans, int queue);
int iwl_trans_pcie_tx(struct iwl_trans *trans, struct sk_buff *skb,
		      struct iwl_device_cmd *dev_cmd, int txq_id);
void iwl_pcie_txq_inc_wr_ptr(struct iwl_trans *trans, struct iwl_txq *txq);
int iwl_trans_pcie_send_hcmd(struct iwl_trans *trans, struct iwl_host_cmd *cmd);
void iwl_pcie_hcmd_complete(struct iwl_trans *trans,
			    struct iwl_rx_cmd_buffer *rxb, int handler_status);
void iwl_trans_pcie_reclaim(struct iwl_trans *trans, int txq_id, int ssn,
			    struct sk_buff_head *skbs);
void iwl_trans_pcie_tx_reset(struct iwl_trans *trans);

/*                                                    
                
                                                     */
int iwl_pcie_dump_fh(struct iwl_trans *trans, char **buf);
void iwl_pcie_dump_csr(struct iwl_trans *trans);

/*                                                    
         
                                                     */
static inline void iwl_disable_interrupts(struct iwl_trans *trans)
{
	struct iwl_trans_pcie *trans_pcie = IWL_TRANS_GET_PCIE_TRANS(trans);
	clear_bit(STATUS_INT_ENABLED, &trans_pcie->status);

	/*                                           */
	iwl_write32(trans, CSR_INT_MASK, 0x00000000);

	/*                                                     
                                           */
	iwl_write32(trans, CSR_INT, 0xffffffff);
	iwl_write32(trans, CSR_FH_INT_STATUS, 0xffffffff);
	IWL_DEBUG_ISR(trans, "Disabled interrupts\n");
}

static inline void iwl_enable_interrupts(struct iwl_trans *trans)
{
	struct iwl_trans_pcie *trans_pcie = IWL_TRANS_GET_PCIE_TRANS(trans);

	IWL_DEBUG_ISR(trans, "Enabling interrupts\n");
	set_bit(STATUS_INT_ENABLED, &trans_pcie->status);
	iwl_write32(trans, CSR_INT_MASK, trans_pcie->inta_mask);
}

static inline void iwl_enable_rfkill_int(struct iwl_trans *trans)
{
	IWL_DEBUG_ISR(trans, "Enabling rfkill interrupt\n");
	iwl_write32(trans, CSR_INT_MASK, CSR_INT_BIT_RF_KILL);
}

static inline void iwl_wake_queue(struct iwl_trans *trans,
				  struct iwl_txq *txq)
{
	struct iwl_trans_pcie *trans_pcie = IWL_TRANS_GET_PCIE_TRANS(trans);

	if (test_and_clear_bit(txq->q.id, trans_pcie->queue_stopped)) {
		IWL_DEBUG_TX_QUEUES(trans, "Wake hwq %d\n", txq->q.id);
		iwl_op_mode_queue_not_full(trans->op_mode, txq->q.id);
	}
}

static inline void iwl_stop_queue(struct iwl_trans *trans,
				  struct iwl_txq *txq)
{
	struct iwl_trans_pcie *trans_pcie = IWL_TRANS_GET_PCIE_TRANS(trans);

	if (!test_and_set_bit(txq->q.id, trans_pcie->queue_stopped)) {
		iwl_op_mode_queue_full(trans->op_mode, txq->q.id);
		IWL_DEBUG_TX_QUEUES(trans, "Stop hwq %d\n", txq->q.id);
	} else
		IWL_DEBUG_TX_QUEUES(trans, "hwq %d already stopped\n",
				    txq->q.id);
}

static inline bool iwl_queue_used(const struct iwl_queue *q, int i)
{
	return q->write_ptr >= q->read_ptr ?
		(i >= q->read_ptr && i < q->write_ptr) :
		!(i < q->read_ptr && i >= q->write_ptr);
}

static inline u8 get_cmd_index(struct iwl_queue *q, u32 index)
{
	return index & (q->n_window - 1);
}

static inline const char *get_cmd_string(struct iwl_trans_pcie *trans_pcie,
					 u8 cmd)
{
	if (!trans_pcie->command_names || !trans_pcie->command_names[cmd])
		return "UNKNOWN";
	return trans_pcie->command_names[cmd];
}

static inline bool iwl_is_rfkill_set(struct iwl_trans *trans)
{
	return !(iwl_read32(trans, CSR_GP_CNTRL) &
		CSR_GP_CNTRL_REG_FLAG_HW_RF_KILL_SW);
}

#endif /*                          */
