#ifndef __EEMCS_NET_H__
#define __EEMCS_NET_H__
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include "eemcs_ccci.h"
#include "eemcs_kal.h"

//                         

#define ECCMNI_MAX_DEV	(END_OF_CCMNI-START_OF_CCMNI)

#define ECCMNI_LB       0xff

#define  IPV4_VERSION 0x40
#define  IPV6_VERSION 0x60

typedef struct 
{
    struct net_device *dev;
    KAL_UINT8         eemcs_port_id;
    ccci_ch_set       ccci_ch;
#if (defined(_ECCMNI_SEQ_SUPPORT_))
    struct{
        KAL_UINT32 UL;
        KAL_UINT32 DL;
    }                 seqno; 
#endif
}eccmni_inst_t;

#if defined(_ECCMNI_LB_UT_)
KAL_UINT32 eccmniut_register_callback(CCCI_CHANNEL_T chn, EEMCS_CCCI_CALLBACK func_ptr , KAL_UINT32 private_data);
KAL_UINT32 eccmniut_unregister_callback(CCCI_CHANNEL_T chn);
inline KAL_UINT32 eccmniut_UL_write_room_alloc(CCCI_CHANNEL_T chn);
inline KAL_INT32 eccmniut_UL_write_skb_to_swq(CCCI_CHANNEL_T chn, struct sk_buff *skb);
inline struct sk_buff* eccmniut_DL_read_skb_from_swq(CCCI_CHANNEL_T chn);

#define ccci_register(ch_num,cb,para)           eccmniut_register_callback(ch_num,cb,para) 
#define ccci_unregister(ch_num)			        eccmniut_unregister_callback(ch_num) 
#define ccci_write_desc_to_q(ch_num,desc_p)     eccmniut_UL_write_skb_to_swq(ch_num, desc_p)
#define ccci_write_space_alloc(ch_num)          eccmniut_UL_write_room_alloc(ch_num)
//                                                                                
#else
#define ccci_register(ch_num,cb,para)           eemcs_ccci_register_callback(ch_num,cb,para) 
#define ccci_unregister(ch_num)			        eemcs_ccci_unregister_callback(ch_num) 
#define ccci_write_desc_to_q(ch_num,desc_p)     eemcs_ccci_UL_write_skb_to_swq(ch_num, desc_p)
#define ccci_write_space_alloc(ch_num)          eemcs_ccci_UL_write_room_alloc(ch_num)
#endif
/*                                                                              
                                    
                                                                               */
KAL_INT32 eccmni_mod_init(void);
void  eccmni_deinit_mod_exit(void);

#if defined(_ECCMNI_LB_UT_) || defined(_EEMCS_CCCI_LB_UT)
KAL_INT32 eccmni_swap(struct sk_buff *skb);
#endif
#endif //               

#if 0
#define ETH_ALEN        6               /*                               */  
#define ETH_HLEN        14              /*                               */  
#define ETH_ZLEN        60              /*                               */  
#define ETH_DATA_LEN    1500            /*                               */  
#define ETH_FRAME_LEN   1514            /*                               */  
#define ETH_FCS_LEN     4               /*                               */  
 
struct ethhdr   
{  
    unsigned char   h_dest[ETH_ALEN];
    unsigned char   h_source[ETH_ALEN];
    __be16          h_proto;
 } __attribute__((packed)); 
 
//       
#define ETH_P_LOOP      0x0060          /*                              */  
#define ETH_P_PUP       0x0200          /*                              */  
#define ETH_P_PUPAT     0x0201          /*                              */  
#define ETH_P_IP        0x0800          /*                              */  
#define ETH_P_X25       0x0805          /*                              */  
#define ETH_P_ARP       0x0806          /*                              */  
#define ETH_P_BPQ       0x08FF           
#define ETH_P_IEEEPUP   0x0a00          /*                            */  
#define ETH_P_IEEEPUPAT 0x0a01            
#define ETH_P_DEC       0x6000          /*                              */  
#define ETH_P_DNA_DL    0x6001          /*                              */  
#define ETH_P_DNA_RC    0x6002          /*                              */  
#define ETH_P_DNA_RT    0x6003          /*                              */  
#define ETH_P_LAT       0x6004          /*                              */  
#define ETH_P_DIAG      0x6005          /*                              */  
#define ETH_P_CUST      0x6006          /*                              */  
#define ETH_P_SCA       0x6007          /*                              */  
#define ETH_P_TEB       0x6558          /*                              */  
#define ETH_P_RARP      0x8035          /*                              */  
#define ETH_P_ATALK     0x809B          /*                              */  
#define ETH_P_AARP      0x80F3          /*                              */  
#define ETH_P_8021Q     0x8100          /*                              */  
#define ETH_P_IPX       0x8137          /*                              */  
#define ETH_P_IPV6      0x86DD          /*                              */  
#define ETH_P_PAUSE     0x8808          /*                                  */  
#define ETH_P_SLOW      0x8809          /*                                */  
#define ETH_P_WCCP      0x883E          /*                                 */  
#define ETH_P_PPP_DISC  0x8863          /*                              */  
#define ETH_P_PPP_SES   0x8864          /*                              */  
#define ETH_P_MPLS_UC   0x8847          /*                              */  
#define ETH_P_MPLS_MC   0x8848          /*                              */  
#define ETH_P_ATMMPOA   0x884c          /*                              */  
#define ETH_P_ATMFATE   0x8884                                  
#define ETH_P_PAE       0x888E          /*                                  */  
#define ETH_P_AOE       0x88A2          /*                              */  
#define ETH_P_TIPC      0x88CA          /*                              */  
#define ETH_P_FCOE      0x8906          /*                              */  
#define ETH_P_EDSA      0xDADA           


typedef  struct  ip_hdr  
{  
unsigned  char  ip_verlen;    
unsigned  char  ip_tos;    
unsigned  short  ip_totallength;    
unsigned  short  ip_id;    
unsigned  short  ip_offset;    
unsigned  char  ip_ttl;    
unsigned  char  ip_protocol;    
unsigned  short  ip_checksum;    
unsigned  int  ip_srcaddr;    
unsigned  int  ip_destaddr;    
}  IP_HDR;  

typedef  struct  udp_hdr  
{  
u_int16_t source; 
u_int16_t dest; 
u_int16_t len; 
u_int16_t check;  
}  UDP_HDR; 


struct iphdr 
{ 
#if __BYTE_ORDER == __LITTLE_ENDIAN 
u_int8_t ihl:4; 
u_int8_t version:4; 
#elif __BYTE_ORDER == __BIG_ENDIAN 
u_int8_t version:4; 
u_int8_t ihl:4; 
#else 
#error "Please fix <bytesex.h>" 
#endif 
u_int8_t tos; 
u_int16_t tot_len; 
u_int16_t id; 
u_int16_t frag_off; 
u_int8_t ttl; 
u_int8_t protocol; 
u_int16_t check; 
u_int32_t saddr; 
u_int32_t daddr; 
}; 

#endif
