/*
                                                                  
                                            
 */
#ifndef CRYPTOCOP_H
#define CRYPTOCOP_H

#include <uapi/arch-v32/arch/cryptocop.h>


/*                                                            */

#include <arch/hwregs/dma.h>

typedef enum {
	cryptocop_alg_csum = 0,
	cryptocop_alg_mem2mem,
	cryptocop_alg_md5,
	cryptocop_alg_sha1,
	cryptocop_alg_des,
	cryptocop_alg_3des,
	cryptocop_alg_aes,
	cryptocop_no_alg,
} cryptocop_algorithm;

typedef u8 cryptocop_tfrm_id;


struct cryptocop_operation;

typedef void (cryptocop_callback)(struct cryptocop_operation*, void*);

struct cryptocop_transform_init {
	cryptocop_algorithm    alg;
	/*                      */
	unsigned char          key[CRYPTOCOP_MAX_KEY_LENGTH];
	unsigned int           keylen;
	cryptocop_cipher_mode  cipher_mode;
	cryptocop_3des_mode    tdes_mode;
	cryptocop_csum_type    csum_mode; /*                                                                 */

	cryptocop_tfrm_id tid; /*                                                                 */
	struct cryptocop_transform_init *next;
};


typedef enum {
	cryptocop_source_dma = 0,
	cryptocop_source_des,
	cryptocop_source_3des,
	cryptocop_source_aes,
	cryptocop_source_md5,
	cryptocop_source_sha1,
	cryptocop_source_csum,
	cryptocop_source_none,
} cryptocop_source;


struct cryptocop_desc_cfg {
	cryptocop_tfrm_id tid;
	cryptocop_source src;
	unsigned int last:1; /*                                                                                   */
	struct cryptocop_desc_cfg *next;
};

struct cryptocop_desc {
	size_t length;
	struct cryptocop_desc_cfg *cfg;
	struct cryptocop_desc *next;
};


/*                              */
#define CRYPTOCOP_NO_FLAG     (0x00)
#define CRYPTOCOP_ENCRYPT     (0x01)
#define CRYPTOCOP_DECRYPT     (0x02)
#define CRYPTOCOP_EXPLICIT_IV (0x04)

struct cryptocop_tfrm_cfg {
	cryptocop_tfrm_id tid;

	unsigned int flags; /*                               */

	/*                                       */
	u8 iv[CRYPTOCOP_MAX_IV_LENGTH];

	/*                                                                       
                                                                          
                                                                      
                 */
	size_t inject_ix;

	struct cryptocop_tfrm_cfg *next;
};



struct cryptocop_dma_list_operation{
	/*                                                                                   
                                                                                
                                                                                       
                       */
	dma_descr_data *outlist; /*                                      */
	char           *out_data_buf;
	dma_descr_data *inlist; /*                                     */
	char           *in_data_buf;

	cryptocop_3des_mode tdes_mode;
	cryptocop_csum_type csum_mode;
};


struct cryptocop_tfrm_operation{
	/*                                                        */
	struct cryptocop_tfrm_cfg *tfrm_cfg;
	struct cryptocop_desc *desc;

	struct iovec *indata;
	size_t incount;
	size_t inlen; /*                 */

	struct iovec *outdata;
	size_t outcount;
	size_t outlen; /*                  */
};


struct cryptocop_operation {
	cryptocop_callback *cb;
	void *cb_data;

	cryptocop_session_id sid;

	/*                                                        */
	int operation_status; /*            */

	/*       */
	unsigned int use_dmalists:1;  /*                                                                    */
	unsigned int in_interrupt:1;  /*                                              */
	unsigned int fast_callback:1; /*                                                           */

	union{
		struct cryptocop_dma_list_operation list_op;
		struct cryptocop_tfrm_operation tfrm_op;
	};
};


int cryptocop_new_session(cryptocop_session_id *sid, struct cryptocop_transform_init *tinit, int alloc_flag);
int cryptocop_free_session(cryptocop_session_id sid);

int cryptocop_job_queue_insert_csum(struct cryptocop_operation *operation);

int cryptocop_job_queue_insert_crypto(struct cryptocop_operation *operation);

int cryptocop_job_queue_insert_user_job(struct cryptocop_operation *operation);

#endif /*             */
