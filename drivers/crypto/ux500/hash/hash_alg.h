/*
 * Copyright (C) ST-Ericsson SA 2010
 * Author: Shujuan Chen (shujuan.chen@stericsson.com)
 * Author: Joakim Bech (joakim.xx.bech@stericsson.com)
 * Author: Berne Hebark (berne.hebark@stericsson.com))
 * License terms: GNU General Public License (GPL) version 2
 */
#ifndef _HASH_ALG_H
#define _HASH_ALG_H

#include <linux/bitops.h>

#define HASH_BLOCK_SIZE			64
#define HASH_DMA_ALIGN_SIZE		4
#define HASH_DMA_PERFORMANCE_MIN_SIZE	1024
#define HASH_BYTES_PER_WORD		4

/*                                         */
#define HASH_HIGH_WORD_MAX_VAL		0xFFFFFFFFUL

/*                                      */
#define HASH_RESET_CR_VALUE		0x0
#define HASH_RESET_STR_VALUE		0x0

/*                                  */
#define HASH_CSR_COUNT			52

#define HASH_RESET_CSRX_REG_VALUE	0x0
#define HASH_RESET_CSFULL_REG_VALUE	0x0
#define HASH_RESET_CSDATAIN_REG_VALUE	0x0

#define HASH_RESET_INDEX_VAL		0x0
#define HASH_RESET_BIT_INDEX_VAL	0x0
#define HASH_RESET_BUFFER_VAL		0x0
#define HASH_RESET_LEN_HIGH_VAL		0x0
#define HASH_RESET_LEN_LOW_VAL		0x0

/*                            */
#define HASH_CR_RESUME_MASK	0x11FCF

#define HASH_CR_SWITCHON_POS	31
#define HASH_CR_SWITCHON_MASK	BIT(31)

#define HASH_CR_EMPTYMSG_POS	20
#define HASH_CR_EMPTYMSG_MASK	BIT(20)

#define HASH_CR_DINF_POS	12
#define HASH_CR_DINF_MASK	BIT(12)

#define HASH_CR_NBW_POS		8
#define HASH_CR_NBW_MASK	0x00000F00UL

#define HASH_CR_LKEY_POS	16
#define HASH_CR_LKEY_MASK	BIT(16)

#define HASH_CR_ALGO_POS	7
#define HASH_CR_ALGO_MASK	BIT(7)

#define HASH_CR_MODE_POS	6
#define HASH_CR_MODE_MASK	BIT(6)

#define HASH_CR_DATAFORM_POS	4
#define HASH_CR_DATAFORM_MASK	(BIT(4) | BIT(5))

#define HASH_CR_DMAE_POS	3
#define HASH_CR_DMAE_MASK	BIT(3)

#define HASH_CR_INIT_POS	2
#define HASH_CR_INIT_MASK	BIT(2)

#define HASH_CR_PRIVN_POS	1
#define HASH_CR_PRIVN_MASK	BIT(1)

#define HASH_CR_SECN_POS	0
#define HASH_CR_SECN_MASK	BIT(0)

/*                          */
#define HASH_STR_DCAL_POS	8
#define HASH_STR_DCAL_MASK	BIT(8)
#define HASH_STR_DEFAULT	0x0

#define HASH_STR_NBLW_POS	0
#define HASH_STR_NBLW_MASK	0x0000001FUL

#define HASH_NBLW_MAX_VAL	0x1F

/*               */
#define HASH_P_ID0		0xE0
#define HASH_P_ID1		0x05
#define HASH_P_ID2		0x38
#define HASH_P_ID3		0x00
#define HASH_CELL_ID0		0x0D
#define HASH_CELL_ID1		0xF0
#define HASH_CELL_ID2		0x05
#define HASH_CELL_ID3		0xB1

#define HASH_SET_BITS(reg_name, mask)	\
	writel_relaxed((readl_relaxed(reg_name) | mask), reg_name)

#define HASH_CLEAR_BITS(reg_name, mask)	\
	writel_relaxed((readl_relaxed(reg_name) & ~mask), reg_name)

#define HASH_PUT_BITS(reg, val, shift, mask)	\
	writel_relaxed(((readl(reg) & ~(mask)) |	\
		(((u32)val << shift) & (mask))), reg)

#define HASH_SET_DIN(val, len)	writesl(&device_data->base->din, (val), (len))

#define HASH_INITIALIZE			\
	HASH_PUT_BITS(			\
		&device_data->base->cr,	\
		0x01, HASH_CR_INIT_POS,	\
		HASH_CR_INIT_MASK)

#define HASH_SET_DATA_FORMAT(data_format)				\
		HASH_PUT_BITS(						\
			&device_data->base->cr,				\
			(u32) (data_format), HASH_CR_DATAFORM_POS,	\
			HASH_CR_DATAFORM_MASK)
#define HASH_SET_NBLW(val)					\
		HASH_PUT_BITS(					\
			&device_data->base->str,		\
			(u32) (val), HASH_STR_NBLW_POS,		\
			HASH_STR_NBLW_MASK)
#define HASH_SET_DCAL					\
		HASH_PUT_BITS(				\
			&device_data->base->str,	\
			0x01, HASH_STR_DCAL_POS,	\
			HASH_STR_DCAL_MASK)

/*                        */
enum hash_mode {
	HASH_MODE_CPU,
	HASH_MODE_DMA
};

/* 
                                                        
                                     
                                     
  
                                   
 */
struct uint64 {
	u32 high_word;
	u32 low_word;
};

/* 
                                                                        
                                       
                                          
                                     
                                                 
                               
                                                    
                                                  
                                                   
                               
                                               
                                                            
                                                         
                               
                                                                 
                                                                 
                                                                 
                                                                 
                                                          
                                                          
                                                          
                                                          
  
                                                                        
                                                                              
                           
 */
struct hash_register {
	u32 cr;
	u32 din;
	u32 str;
	u32 hx[8];

	u32 padding0[(0x080 - 0x02C) / sizeof(u32)];

	u32 itcr;
	u32 itip;
	u32 itop;

	u32 padding1[(0x0F8 - 0x08C) / sizeof(u32)];

	u32 csfull;
	u32 csdatain;
	u32 csrx[HASH_CSR_COUNT];

	u32 padding2[(0xFE0 - 0x1D0) / sizeof(u32)];

	u32 periphid0;
	u32 periphid1;
	u32 periphid2;
	u32 periphid3;

	u32 cellid0;
	u32 cellid1;
	u32 cellid2;
	u32 cellid3;
};

/* 
                                          
                                             
                                 
                                      
                                              
                                                           
                                                           
                                                              
                                                                           
                                                    
                                                      
  
                                                                              
                                                                            
                       
  
                                                                              
                                                                            
                                                                     
 */
struct hash_state {
	u32		temp_cr;
	u32		str_reg;
	u32		din_reg;
	u32		csr[52];
	u32		csfull;
	u32		csdatain;
	u32		buffer[HASH_BLOCK_SIZE / sizeof(u32)];
	struct uint64	length;
	u8		index;
	u8		bit_index;
};

/* 
                                        
                                             
                                             
 */
enum hash_device_id {
	HASH_DEVICE_ID_0 = 0,
	HASH_DEVICE_ID_1 = 1
};

/* 
                                            
                                          
                                          
                                         
                                        
 */
enum hash_data_format {
	HASH_DATA_32_BITS	= 0x0,
	HASH_DATA_16_BITS	= 0x1,
	HASH_DATA_8_BITS	= 0x2,
	HASH_DATA_1_BIT		= 0x3
};

/* 
                                                                             
                                                
                                                         
 */
enum hash_algo {
	HASH_ALGO_SHA1		= 0x0,
	HASH_ALGO_SHA256	= 0x1
};

/* 
                                                                      
                                                             
                                                 
 */
enum hash_op {
	HASH_OPER_MODE_HASH = 0x0,
	HASH_OPER_MODE_HMAC = 0x1
};

/* 
                                                            
                                                                       
                                        
                                             
 */
struct hash_config {
	int data_format;
	int algorithm;
	int oper_mode;
};

/* 
                                            
                                        
                                                         
                               
                                            
                                
                      
                                 
 */
struct hash_dma {
	dma_cap_mask_t		mask;
	struct completion	complete;
	struct dma_chan		*chan_mem2hash;
	void			*cfg_mem2hash;
	int			sg_len;
	struct scatterlist	*sg;
	int			nents;
};

/* 
                                                            
                                       
                                  
                                                 
                                      
                                           
                                            
 */
struct hash_ctx {
	u8			*key;
	u32			keylen;
	struct hash_config	config;
	int			digestsize;
	struct hash_device_data	*device;
};

/* 
                                                                    
                                                 
                                                                        
                                                   
                                                                     
 */
struct hash_req_ctx {
	struct hash_state	state;
	bool			dma_mode;
	u8			updated;
};

/* 
                                                         
                                                
                                       
                                              
                                        
                                                            
                                                                
                                               
                                                      
                                                
                                                                  
                                 
 */
struct hash_device_data {
	struct hash_register __iomem	*base;
	struct klist_node	list_node;
	struct device		*dev;
	struct spinlock		ctx_lock;
	struct hash_ctx		*current_ctx;
	bool			power_state;
	struct spinlock		power_state_lock;
	struct regulator	*regulator;
	struct clk		*clk;
	bool			restore_dev_state;
	struct hash_state	state; /*                                    */
	struct hash_dma		dma;
};

int hash_check_hw(struct hash_device_data *device_data);

int hash_setconfiguration(struct hash_device_data *device_data,
		struct hash_config *config);

void hash_begin(struct hash_device_data *device_data, struct hash_ctx *ctx);

void hash_get_digest(struct hash_device_data *device_data,
		u8 *digest, int algorithm);

int hash_hw_update(struct ahash_request *req);

int hash_save_state(struct hash_device_data *device_data,
		struct hash_state *state);

int hash_resume_state(struct hash_device_data *device_data,
		const struct hash_state *state);

#endif
