/******************************************************************************
 *
 * This file is provided under a dual BSD/GPLv2 license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * GPL LICENSE SUMMARY
 *
 * Copyright(c) 2007 - 2013 Intel Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110,
 * USA
 *
 * The full GNU General Public License is included in this distribution
 * in the file called COPYING.
 *
 * Contact Information:
 *  Intel Linux Wireless <ilw@linux.intel.com>
 * Intel Corporation, 5200 N.E. Elam Young Parkway, Hillsboro, OR 97124-6497
 *
 * BSD LICENSE
 *
 * Copyright(c) 2005 - 2013 Intel Corporation. All rights reserved.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name Intel Corporation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *****************************************************************************/
#ifndef __iwl_trans_h__
#define __iwl_trans_h__

#include <linux/ieee80211.h>
#include <linux/mm.h> /*                  */
#include <linux/lockdep.h>

#include "iwl-debug.h"
#include "iwl-config.h"
#include "iwl-fw.h"

/* 
                                      
  
                                                                               
                                                                              
                                                                           
                                                                            
               
                                                                   
 */

/* 
                                         
  
                                                     
  
                                                                      
                                                                         
                                                                      
                                              
                                                                     
                        
  
                                                                       
                             
              
              
  
                                    
                                                 
             
  
                                                   
 */

/* 
                            
  
                                                                              
                                                                        
                                            
                                                                             
 */
#define SEQ_TO_QUEUE(s)	(((s) >> 8) & 0x1f)
#define QUEUE_TO_SEQ(q)	(((q) & 0x1f) << 8)
#define SEQ_TO_INDEX(s)	((s) & 0xff)
#define INDEX_TO_SEQ(i)	((i) & 0xff)
#define SEQ_RX_FRAME	cpu_to_le16(0x8000)

/* 
                        
  
                                                                            
                                                              
 */
struct iwl_cmd_header {
	u8 cmd;		/*                               */
	u8 flags;	/*                                   */
	/*
                                                                     
                                                                   
                                                                   
                                                                       
                                                                       
   
                                                                 
                                                                  
                                                                  
                                                                 
                                                                     
   
                                               
   
                                              
                     
                   
                                                        
  */
	__le16 sequence;
} __packed;

/*                            */
#define IWL_CMD_FAILED_MSK 0x40


#define FH_RSCSR_FRAME_SIZE_MSK		0x00003FFF	/*           */
#define FH_RSCSR_FRAME_INVALID		0x55550000
#define FH_RSCSR_FRAME_ALIGN		0x40

struct iwl_rx_packet {
	/*
                                                                      
                        
               
                                
                                                    
                                
                   
                        
  */
	__le32 len_n_flags;
	struct iwl_cmd_header hdr;
	u8 data[];
} __packed;

/* 
                                                  
  
                                                                             
                                                                
                                                                              
                                                              
                                                              
 */
enum CMD_MODE {
	CMD_SYNC		= 0,
	CMD_ASYNC		= BIT(0),
	CMD_WANT_SKB		= BIT(1),
	CMD_ON_DEMAND		= BIT(2),
};

#define DEF_CMD_PAYLOAD_SIZE 320

/* 
                        
  
                                                                            
                                                                         
                                               
 */
struct iwl_device_cmd {
	struct iwl_cmd_header hdr;	/*           */
	u8 payload[DEF_CMD_PAYLOAD_SIZE];
} __packed;

#define TFD_MAX_PAYLOAD_SIZE (sizeof(struct iwl_device_cmd))

/*
                                                                        
                                                           
 */
#define IWL_MAX_CMD_TBS_PER_TFD	2

/* 
                                                                            
  
                                                                                
                                                                         
                                                                          
                                                                       
                                                                       
                                                                  
                                                                          
                                                                              
                                                                       
                                                 
                                                                       
 */
enum iwl_hcmd_dataflag {
	IWL_HCMD_DFL_NOCOPY	= BIT(0),
	IWL_HCMD_DFL_DUP	= BIT(1),
};

/* 
                                                  
  
                                                                    
                                                       
                                                             
                                                            
                                                              
                                                        
                       
                                                  
                             
                              
 */
struct iwl_host_cmd {
	const void *data[IWL_MAX_CMD_TBS_PER_TFD];
	struct iwl_rx_packet *resp_pkt;
	unsigned long _rx_page_addr;
	u32 _rx_page_order;
	int handler_status;

	u32 flags;
	u16 len[IWL_MAX_CMD_TBS_PER_TFD];
	u8 dataflags[IWL_MAX_CMD_TBS_PER_TFD];
	u8 id;
};

static inline void iwl_free_resp(struct iwl_host_cmd *cmd)
{
	free_pages(cmd->_rx_page_addr, cmd->_rx_page_order);
}

struct iwl_rx_cmd_buffer {
	struct page *_page;
	int _offset;
	bool _page_stolen;
	u32 _rx_page_order;
	unsigned int truesize;
};

static inline void *rxb_addr(struct iwl_rx_cmd_buffer *r)
{
	return (void *)((unsigned long)page_address(r->_page) + r->_offset);
}

static inline int rxb_offset(struct iwl_rx_cmd_buffer *r)
{
	return r->_offset;
}

static inline struct page *rxb_steal_page(struct iwl_rx_cmd_buffer *r)
{
	r->_page_stolen = true;
	get_page(r->_page);
	return r->_page;
}

static inline void iwl_free_rxb(struct iwl_rx_cmd_buffer *r)
{
	__free_pages(r->_page, r->_rx_page_order);
}

#define MAX_NO_RECLAIM_CMDS	6

#define IWL_MASK(lo, hi) ((1 << (hi)) | ((1 << (hi)) - (1 << (lo))))

/*
                                                  
                     
 */
#define IWL_MAX_HW_QUEUES		32
#define IWL_MAX_TID_COUNT	8
#define IWL_FRAME_LIMIT	64

/* 
                                                      
                                                               
                                                         
 */
enum iwl_d3_status {
	IWL_D3_STATUS_ALIVE,
	IWL_D3_STATUS_RESET,
};

/* 
                                                    
  
                                        
                                              
                               
                                        
                                                           
                                                      
                                                      
                        
                                            
                                                           
                                         
                                                                            
                                 
                                                           
                                                       
                                                              
                                             
 */
struct iwl_trans_config {
	struct iwl_op_mode *op_mode;

	u8 cmd_queue;
	u8 cmd_fifo;
	const u8 *no_reclaim_cmds;
	int n_no_reclaim_cmds;

	bool rx_buf_size_8k;
	bool bc_table_dword;
	unsigned int queue_watchdog_timeout;
	const char **command_names;
};

struct iwl_trans;

/* 
                                                       
  
                                       
  
                                                                           
            
                                                                              
                                                                   
                           
            
                                                                     
                               
            
                                                                             
                                                                      
            
                                                                  
            
                                                                      
                                                                   
                                      
                                                                     
                                                                       
                                                                     
                                                                             
                                                                      
                                 
                                    
                   
                 
                                                                    
                 
                                                            
                                                                         
                                                                      
                                                       
                 
                                                           
            
                                                                           
                         
                                                        
                                         
                                                               
                                                                 
                                                          
                                                     
                                                     
                                         
                                                                              
                  
                                                                        
                                                                     
                     
                                    
                                                                     
                                                      
                      
                                                                      
                                                                    
                                                                  
 */
struct iwl_trans_ops {

	int (*start_hw)(struct iwl_trans *iwl_trans);
	void (*stop_hw)(struct iwl_trans *iwl_trans, bool op_mode_leaving);
	int (*start_fw)(struct iwl_trans *trans, const struct fw_img *fw,
			bool run_in_rfkill);
	void (*fw_alive)(struct iwl_trans *trans, u32 scd_addr);
	void (*stop_device)(struct iwl_trans *trans);

	void (*d3_suspend)(struct iwl_trans *trans);
	int (*d3_resume)(struct iwl_trans *trans, enum iwl_d3_status *status);

	int (*send_cmd)(struct iwl_trans *trans, struct iwl_host_cmd *cmd);

	int (*tx)(struct iwl_trans *trans, struct sk_buff *skb,
		  struct iwl_device_cmd *dev_cmd, int queue);
	void (*reclaim)(struct iwl_trans *trans, int queue, int ssn,
			struct sk_buff_head *skbs);

	void (*txq_enable)(struct iwl_trans *trans, int queue, int fifo,
			   int sta_id, int tid, int frame_limit, u16 ssn);
	void (*txq_disable)(struct iwl_trans *trans, int queue);

	int (*dbgfs_register)(struct iwl_trans *trans, struct dentry* dir);
	int (*wait_tx_queue_empty)(struct iwl_trans *trans);
#ifdef CONFIG_PM_SLEEP
	int (*suspend)(struct iwl_trans *trans);
	int (*resume)(struct iwl_trans *trans);
#endif
	void (*write8)(struct iwl_trans *trans, u32 ofs, u8 val);
	void (*write32)(struct iwl_trans *trans, u32 ofs, u32 val);
	u32 (*read32)(struct iwl_trans *trans, u32 ofs);
	u32 (*read_prph)(struct iwl_trans *trans, u32 ofs);
	void (*write_prph)(struct iwl_trans *trans, u32 ofs, u32 val);
	int (*read_mem)(struct iwl_trans *trans, u32 addr,
			void *buf, int dwords);
	int (*write_mem)(struct iwl_trans *trans, u32 addr,
			 void *buf, int dwords);
	void (*configure)(struct iwl_trans *trans,
			  const struct iwl_trans_config *trans_cfg);
	void (*set_pmi)(struct iwl_trans *trans, bool state);
	bool (*grab_nic_access)(struct iwl_trans *trans, bool silent,
				unsigned long *flags);
	void (*release_nic_access)(struct iwl_trans *trans,
				   unsigned long *flags);
	void (*set_bits_mask)(struct iwl_trans *trans, u32 reg, u32 mask,
			      u32 value);
};

/* 
                                                      
  
                                                     
                                                       
 */
enum iwl_trans_state {
	IWL_TRANS_NO_FW = 0,
	IWL_TRANS_FW_ALIVE	= 1,
};

/* 
                                           
  
                                  
                                    
                                      
                                                               
                                                       
                                   
                                                                               
                                                               
                                                                     
                                                     
                                                                            
                                            
                                                     
                                                                      
                                          
                                                                     
                                                 
 */
struct iwl_trans {
	const struct iwl_trans_ops *ops;
	struct iwl_op_mode *op_mode;
	const struct iwl_cfg *cfg;
	enum iwl_trans_state state;

	struct device *dev;
	u32 hw_rev;
	u32 hw_id;
	char hw_id_str[52];

	u8 rx_mpdu_cmd, rx_mpdu_cmd_hdr_size;

	bool pm_support;
	bool ltr_enabled;

	/* The following fields are internal only */
	struct kmem_cache *dev_cmd_pool;
	size_t dev_cmd_headroom;
	char dev_cmd_pool_name[50];

	struct dentry *dbgfs_dir;

#ifdef CONFIG_LOCKDEP
	struct lockdep_map sync_cmd_lockdep_map;
#endif

	/*                                  */
	/*                                                                  */
	char trans_specific[0] __aligned(sizeof(void *));
};

static inline void iwl_trans_configure(struct iwl_trans *trans,
				       const struct iwl_trans_config *trans_cfg)
{
	trans->op_mode = trans_cfg->op_mode;

	trans->ops->configure(trans, trans_cfg);
}

static inline int iwl_trans_start_hw(struct iwl_trans *trans)
{
	might_sleep();

	return trans->ops->start_hw(trans);
}

static inline void iwl_trans_stop_hw(struct iwl_trans *trans,
				     bool op_mode_leaving)
{
	might_sleep();

	trans->ops->stop_hw(trans, op_mode_leaving);

	if (op_mode_leaving)
		trans->op_mode = NULL;

	trans->state = IWL_TRANS_NO_FW;
}

static inline void iwl_trans_fw_alive(struct iwl_trans *trans, u32 scd_addr)
{
	might_sleep();

	trans->state = IWL_TRANS_FW_ALIVE;

	trans->ops->fw_alive(trans, scd_addr);
}

static inline int iwl_trans_start_fw(struct iwl_trans *trans,
				     const struct fw_img *fw,
				     bool run_in_rfkill)
{
	might_sleep();

	WARN_ON_ONCE(!trans->rx_mpdu_cmd);

	return trans->ops->start_fw(trans, fw, run_in_rfkill);
}

static inline void iwl_trans_stop_device(struct iwl_trans *trans)
{
	might_sleep();

	trans->ops->stop_device(trans);

	trans->state = IWL_TRANS_NO_FW;
}

static inline void iwl_trans_d3_suspend(struct iwl_trans *trans)
{
	might_sleep();
	trans->ops->d3_suspend(trans);
}

static inline int iwl_trans_d3_resume(struct iwl_trans *trans,
				      enum iwl_d3_status *status)
{
	might_sleep();
	return trans->ops->d3_resume(trans, status);
}

static inline int iwl_trans_send_cmd(struct iwl_trans *trans,
				     struct iwl_host_cmd *cmd)
{
	int ret;

	if (trans->state != IWL_TRANS_FW_ALIVE) {
		IWL_ERR(trans, "%s bad state = %d", __func__, trans->state);
		return -EIO;
	}

	if (!(cmd->flags & CMD_ASYNC))
		lock_map_acquire_read(&trans->sync_cmd_lockdep_map);

	ret = trans->ops->send_cmd(trans, cmd);

	if (!(cmd->flags & CMD_ASYNC))
		lock_map_release(&trans->sync_cmd_lockdep_map);

	return ret;
}

static inline struct iwl_device_cmd *
iwl_trans_alloc_tx_cmd(struct iwl_trans *trans)
{
	u8 *dev_cmd_ptr = kmem_cache_alloc(trans->dev_cmd_pool, GFP_ATOMIC);

	if (unlikely(dev_cmd_ptr == NULL))
		return NULL;

	return (struct iwl_device_cmd *)
			(dev_cmd_ptr + trans->dev_cmd_headroom);
}

static inline void iwl_trans_free_tx_cmd(struct iwl_trans *trans,
					 struct iwl_device_cmd *dev_cmd)
{
	u8 *dev_cmd_ptr = (u8 *)dev_cmd - trans->dev_cmd_headroom;

	kmem_cache_free(trans->dev_cmd_pool, dev_cmd_ptr);
}

static inline int iwl_trans_tx(struct iwl_trans *trans, struct sk_buff *skb,
			       struct iwl_device_cmd *dev_cmd, int queue)
{
	WARN_ONCE(trans->state != IWL_TRANS_FW_ALIVE,
		  "%s bad state = %d", __func__, trans->state);

	return trans->ops->tx(trans, skb, dev_cmd, queue);
}

static inline void iwl_trans_reclaim(struct iwl_trans *trans, int queue,
				     int ssn, struct sk_buff_head *skbs)
{
	WARN_ONCE(trans->state != IWL_TRANS_FW_ALIVE,
		  "%s bad state = %d", __func__, trans->state);

	trans->ops->reclaim(trans, queue, ssn, skbs);
}

static inline void iwl_trans_txq_disable(struct iwl_trans *trans, int queue)
{
	WARN_ONCE(trans->state != IWL_TRANS_FW_ALIVE,
		  "%s bad state = %d", __func__, trans->state);

	trans->ops->txq_disable(trans, queue);
}

static inline void iwl_trans_txq_enable(struct iwl_trans *trans, int queue,
					int fifo, int sta_id, int tid,
					int frame_limit, u16 ssn)
{
	might_sleep();

	WARN_ONCE(trans->state != IWL_TRANS_FW_ALIVE,
		  "%s bad state = %d", __func__, trans->state);

	trans->ops->txq_enable(trans, queue, fifo, sta_id, tid,
				 frame_limit, ssn);
}

static inline void iwl_trans_ac_txq_enable(struct iwl_trans *trans, int queue,
					   int fifo)
{
	iwl_trans_txq_enable(trans, queue, fifo, -1,
			     IWL_MAX_TID_COUNT, IWL_FRAME_LIMIT, 0);
}

static inline int iwl_trans_wait_tx_queue_empty(struct iwl_trans *trans)
{
	WARN_ONCE(trans->state != IWL_TRANS_FW_ALIVE,
		  "%s bad state = %d", __func__, trans->state);

	return trans->ops->wait_tx_queue_empty(trans);
}

static inline int iwl_trans_dbgfs_register(struct iwl_trans *trans,
					   struct dentry *dir)
{
	return trans->ops->dbgfs_register(trans, dir);
}

#ifdef CONFIG_PM_SLEEP
static inline int iwl_trans_suspend(struct iwl_trans *trans)
{
	return trans->ops->suspend(trans);
}

static inline int iwl_trans_resume(struct iwl_trans *trans)
{
	return trans->ops->resume(trans);
}
#endif

static inline void iwl_trans_write8(struct iwl_trans *trans, u32 ofs, u8 val)
{
	trans->ops->write8(trans, ofs, val);
}

static inline void iwl_trans_write32(struct iwl_trans *trans, u32 ofs, u32 val)
{
	trans->ops->write32(trans, ofs, val);
}

static inline u32 iwl_trans_read32(struct iwl_trans *trans, u32 ofs)
{
	return trans->ops->read32(trans, ofs);
}

static inline u32 iwl_trans_read_prph(struct iwl_trans *trans, u32 ofs)
{
	return trans->ops->read_prph(trans, ofs);
}

static inline void iwl_trans_write_prph(struct iwl_trans *trans, u32 ofs,
					u32 val)
{
	return trans->ops->write_prph(trans, ofs, val);
}

static inline int iwl_trans_read_mem(struct iwl_trans *trans, u32 addr,
				     void *buf, int dwords)
{
	return trans->ops->read_mem(trans, addr, buf, dwords);
}

#define iwl_trans_read_mem_bytes(trans, addr, buf, bufsize)		      \
	do {								      \
		if (__builtin_constant_p(bufsize))			      \
			BUILD_BUG_ON((bufsize) % sizeof(u32));		      \
		iwl_trans_read_mem(trans, addr, buf, (bufsize) / sizeof(u32));\
	} while (0)

static inline u32 iwl_trans_read_mem32(struct iwl_trans *trans, u32 addr)
{
	u32 value;

	if (WARN_ON(iwl_trans_read_mem(trans, addr, &value, 1)))
		return 0xa5a5a5a5;

	return value;
}

static inline int iwl_trans_write_mem(struct iwl_trans *trans, u32 addr,
				      void *buf, int dwords)
{
	return trans->ops->write_mem(trans, addr, buf, dwords);
}

static inline u32 iwl_trans_write_mem32(struct iwl_trans *trans, u32 addr,
					u32 val)
{
	return iwl_trans_write_mem(trans, addr, &val, 1);
}

static inline void iwl_trans_set_pmi(struct iwl_trans *trans, bool state)
{
	trans->ops->set_pmi(trans, state);
}

static inline void
iwl_trans_set_bits_mask(struct iwl_trans *trans, u32 reg, u32 mask, u32 value)
{
	trans->ops->set_bits_mask(trans, reg, mask, value);
}

#define iwl_trans_grab_nic_access(trans, silent, flags)	\
	__cond_lock(nic_access,				\
		    likely((trans)->ops->grab_nic_access(trans, silent, flags)))

static inline void __releases(nic_access)
iwl_trans_release_nic_access(struct iwl_trans *trans, unsigned long *flags)
{
	trans->ops->release_nic_access(trans, flags);
	__release(nic_access);
}

/*                                                    
                                                  
                                                     */
int __must_check iwl_pci_register_driver(void);
void iwl_pci_unregister_driver(void);

static inline void trans_lockdep_init(struct iwl_trans *trans)
{
#ifdef CONFIG_LOCKDEP
	static struct lock_class_key __key;

	lockdep_init_map(&trans->sync_cmd_lockdep_map, "sync_cmd_lockdep_map",
			 &__key, 0);
#endif
}

#endif /*                 */
