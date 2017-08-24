/**
 * Copyright (C) ST-Ericsson SA 2010
 * Author: Shujuan Chen <shujuan.chen@stericsson.com> for ST-Ericsson.
 * Author: Jonas Linde <jonas.linde@stericsson.com> for ST-Ericsson.
 * Author: Joakim Bech <joakim.xx.bech@stericsson.com> for ST-Ericsson.
 * Author: Berne Hebark <berne.herbark@stericsson.com> for ST-Ericsson.
 * Author: Niklas Hernaeus <niklas.hernaeus@stericsson.com> for ST-Ericsson.
 * License terms: GNU General Public License (GPL) version 2
 */

#ifndef _CRYP_H_
#define _CRYP_H_

#include <linux/completion.h>
#include <linux/dmaengine.h>
#include <linux/klist.h>
#include <linux/mutex.h>

#define DEV_DBG_NAME "crypX crypX:"

/*                     */
enum cryp_crypen {
	CRYP_CRYPEN_DISABLE = 0,
	CRYP_CRYPEN_ENABLE = 1
};

/*                                       */
enum cryp_start {
	CRYP_START_DISABLE = 0,
	CRYP_START_ENABLE = 1
};

/*                                 */
enum cryp_init {
	CRYP_INIT_DISABLE = 0,
	CRYP_INIT_ENABLE = 1
};

/*                           */
enum cryp_state {
	CRYP_STATE_DISABLE = 0,
	CRYP_STATE_ENABLE = 1
};

/*                            */
enum cryp_key_prep {
	KSE_DISABLED = 0,
	KSE_ENABLED = 1
};

/*                  */
#define	CRYP_KEY_SIZE_128 (0)
#define	CRYP_KEY_SIZE_192 (1)
#define	CRYP_KEY_SIZE_256 (2)

/*           */
enum cryp_algo_mode {
	CRYP_ALGO_TDES_ECB,
	CRYP_ALGO_TDES_CBC,
	CRYP_ALGO_DES_ECB,
	CRYP_ALGO_DES_CBC,
	CRYP_ALGO_AES_ECB,
	CRYP_ALGO_AES_CBC,
	CRYP_ALGO_AES_CTR,
	CRYP_ALGO_AES_XTS
};

/*                               */
enum cryp_algorithm_dir {
	CRYP_ALGORITHM_ENCRYPT,
	CRYP_ALGORITHM_DECRYPT
};

/*                        */
enum cryp_mode {
	CRYP_MODE_POLLING,
	CRYP_MODE_INTERRUPT,
	CRYP_MODE_DMA
};

/* 
                       
                             
                       
                                          
  
                                                                 
 */
struct cryp_config {
	int keysize;
	enum cryp_algo_mode algomode;
	enum cryp_algorithm_dir algodir;
};

/* 
                                  
                                                          
                                                   
  
                                                                  
 */
struct cryp_protection_config {
	enum cryp_state privilege_access;
	enum cryp_state secure_access;
};

/*             */
enum cryp_status_id {
	CRYP_STATUS_BUSY = 0x10,
	CRYP_STATUS_OUTPUT_FIFO_FULL = 0x08,
	CRYP_STATUS_OUTPUT_FIFO_NOT_EMPTY = 0x04,
	CRYP_STATUS_INPUT_FIFO_NOT_FULL = 0x02,
	CRYP_STATUS_INPUT_FIFO_EMPTY = 0x01
};

/*                    */
enum cryp_dma_req_type {
	CRYP_DMA_DISABLE_BOTH,
	CRYP_DMA_ENABLE_IN_DATA,
	CRYP_DMA_ENABLE_OUT_DATA,
	CRYP_DMA_ENABLE_BOTH_DIRECTIONS
};

enum cryp_dma_channel {
	CRYP_DMA_RX = 0,
	CRYP_DMA_TX
};

/*               */
enum cryp_key_reg_index {
	CRYP_KEY_REG_1,
	CRYP_KEY_REG_2,
	CRYP_KEY_REG_3,
	CRYP_KEY_REG_4
};

/*                             */
struct cryp_key_value {
	u32 key_value_left;
	u32 key_value_right;
};

/*                               */
enum cryp_init_vector_index {
	CRYP_INIT_VECTOR_INDEX_0,
	CRYP_INIT_VECTOR_INDEX_1
};

/*                                
                   
                    
   */
struct cryp_init_vector_value {
	u32 init_value_left;
	u32 init_value_right;
};

/* 
                                                             
                        
                               
                                           
                            
                            
                            
                            
                            
                            
                            
                            
                                                    
                                                    
                                                    
                                                    
                         
                           
  
                                           
 */
struct cryp_device_context {
	u32 cr;
	u32 dmacr;
	u32 imsc;

	u32 key_1_l;
	u32 key_1_r;
	u32 key_2_l;
	u32 key_2_r;
	u32 key_3_l;
	u32 key_3_r;
	u32 key_4_l;
	u32 key_4_r;

	u32 init_vect_0_l;
	u32 init_vect_0_r;
	u32 init_vect_1_l;
	u32 init_vect_1_r;

	u32 din;
	u32 dout;
};

struct cryp_dma {
	dma_cap_mask_t mask;
	struct completion cryp_dma_complete;
	struct dma_chan *chan_cryp2mem;
	struct dma_chan *chan_mem2cryp;
	struct stedma40_chan_cfg *cfg_cryp2mem;
	struct stedma40_chan_cfg *cfg_mem2cryp;
	int sg_src_len;
	int sg_dst_len;
	struct scatterlist *sg_src;
	struct scatterlist *sg_dst;
	int nents_src;
	int nents_dst;
};

/* 
                                                         
                                               
                                              
                                               
                                                         
                                              
                                   
                                                            
                                          
                                                         
                                                                
                                                   
                                                            
 */
struct cryp_device_data {
	struct cryp_register __iomem *base;
	struct device *dev;
	struct clk *clk;
	struct regulator *pwr_regulator;
	int power_status;
	struct spinlock ctx_lock;
	struct cryp_ctx *current_ctx;
	struct klist_node list_node;
	struct cryp_dma dma;
	bool power_state;
	struct spinlock power_state_spinlock;
	bool restore_dev_ctx;
};

void cryp_wait_until_done(struct cryp_device_data *device_data);

/*                          */

int cryp_check(struct cryp_device_data *device_data);

void cryp_activity(struct cryp_device_data *device_data,
		   enum cryp_crypen cryp_crypen);

void cryp_flush_inoutfifo(struct cryp_device_data *device_data);

int cryp_set_configuration(struct cryp_device_data *device_data,
			   struct cryp_config *cryp_config,
			   u32 *control_register);

void cryp_configure_for_dma(struct cryp_device_data *device_data,
			    enum cryp_dma_req_type dma_req);

int cryp_configure_key_values(struct cryp_device_data *device_data,
			      enum cryp_key_reg_index key_reg_index,
			      struct cryp_key_value key_value);

int cryp_configure_init_vector(struct cryp_device_data *device_data,
			       enum cryp_init_vector_index
			       init_vector_index,
			       struct cryp_init_vector_value
			       init_vector_value);

int cryp_configure_protection(struct cryp_device_data *device_data,
			      struct cryp_protection_config *p_protect_config);

/*                           */
void cryp_save_device_context(struct cryp_device_data *device_data,
			      struct cryp_device_context *ctx,
			      int cryp_mode);

void cryp_restore_device_context(struct cryp_device_data *device_data,
				 struct cryp_device_context *ctx);

/*                                */
int cryp_is_logic_busy(struct cryp_device_data *device_data);

int cryp_get_status(struct cryp_device_data *device_data);

/* 
                                                                          
                                           
                                                                    
                              
 */
int cryp_write_indata(struct cryp_device_data *device_data, u32 write_data);

/* 
                                                                       
                                     
                                                                    
                                                  
 */
int cryp_read_outdata(struct cryp_device_data *device_data, u32 *read_data);

#endif /*          */
