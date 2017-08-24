#ifndef __CCCI_BM_H__
#define __CCCI_BM_H__

#include "ccci_core.h"

/*
                                                                                               
                                                            
                                                                                                   
                                                                                             
                                                                    
   
                                               
 */
#define SKB_4K CCCI_MTU+128 //                                                               
#define SKB_1_5K CCMNI_MTU+16 //                                   
#define SKB_16 16 //                       

#ifdef NET_SKBUFF_DATA_USES_OFFSET
#define skb_size(x) ((x)->end)
#define skb_data_size(x) ((x)->head + (x)->end - (x)->data)
#else
#define skb_size(x) ((x)->end - (x)->head)
#define skb_data_size(x) ((x)->end - (x)->data)
#endif

struct ccci_skb_queue {
	struct sk_buff_head skb_list;
	unsigned int max_len;
	struct work_struct reload_work;
};

struct sk_buff *ccci_alloc_skb(int size, char blocking);
void ccci_free_skb(struct sk_buff *skb, DATA_POLICY policy);
	
struct ccci_request *ccci_alloc_req(DIRECTION dir, int size, char blk1, char blk2);
void ccci_free_req(struct ccci_request *req);
void ccci_dump_req(struct ccci_request *req);
void ccci_mem_dump(int md_id, void *start_addr, int len);
void ccci_cmpt_mem_dump(int md_id, void *start_addr, int len);

#endif //             