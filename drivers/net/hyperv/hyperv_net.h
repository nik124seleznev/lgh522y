/*
 *
 * Copyright (c) 2011, Microsoft Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place - Suite 330, Boston, MA 02111-1307 USA.
 *
 * Authors:
 *   Haiyang Zhang <haiyangz@microsoft.com>
 *   Hank Janssen  <hjanssen@microsoft.com>
 *   K. Y. Srinivasan <kys@microsoft.com>
 *
 */

#ifndef _HYPERV_NET_H
#define _HYPERV_NET_H

#include <linux/list.h>
#include <linux/hyperv.h>
#include <linux/rndis.h>

/*                 */
struct hv_netvsc_packet;

/*                                                                       */
struct xferpage_packet {
	struct list_head list_ent;
	u32 status;

	/*                                               */
	u32 count;
};

/*
                                                                      
                   
 */
struct hv_netvsc_packet {
	/*                   */
	struct list_head list_ent;
	u32 status;

	struct hv_device *device;
	bool is_data_pkt;
	u16 vlan_tci;

	/*
                                                            
                                
  */
	struct xferpage_packet *xfer_page_pkt;

	union {
		struct {
			u64 recv_completion_tid;
			void *recv_completion_ctx;
			void (*recv_completion)(void *context);
		} recv;
		struct {
			u64 send_completion_tid;
			void *send_completion_ctx;
			void (*send_completion)(void *context);
		} send;
	} completion;

	/*                                          */
	void *extension;

	u32 total_data_buflen;
	/*                                                               */
	void *data;
	u32 page_buf_cnt;
	struct hv_page_buffer page_buf[0];
};

struct netvsc_device_info {
	unsigned char mac_adr[ETH_ALEN];
	bool link_state;	/*                            */
	int  ring_size;
};

enum rndis_device_state {
	RNDIS_DEV_UNINITIALIZED = 0,
	RNDIS_DEV_INITIALIZING,
	RNDIS_DEV_INITIALIZED,
	RNDIS_DEV_DATAINITIALIZED,
};

struct rndis_device {
	struct netvsc_device *net_dev;

	enum rndis_device_state state;
	bool link_state;
	atomic_t new_req_id;

	spinlock_t request_lock;
	struct list_head req_list;

	unsigned char hw_mac_adr[ETH_ALEN];
};


/*           */
int netvsc_device_add(struct hv_device *device, void *additional_info);
int netvsc_device_remove(struct hv_device *device);
int netvsc_send(struct hv_device *device,
		struct hv_netvsc_packet *packet);
void netvsc_linkstatus_callback(struct hv_device *device_obj,
				unsigned int status);
int netvsc_recv_callback(struct hv_device *device_obj,
			struct hv_netvsc_packet *packet);
int rndis_filter_open(struct hv_device *dev);
int rndis_filter_close(struct hv_device *dev);
int rndis_filter_device_add(struct hv_device *dev,
			void *additional_info);
void rndis_filter_device_remove(struct hv_device *dev);
int rndis_filter_receive(struct hv_device *dev,
			struct hv_netvsc_packet *pkt);



int rndis_filter_send(struct hv_device *dev,
			struct hv_netvsc_packet *pkt);

int rndis_filter_set_packet_filter(struct rndis_device *dev, u32 new_filter);
int rndis_filter_set_device_mac(struct hv_device *hdev, char *mac);


#define NVSP_INVALID_PROTOCOL_VERSION	((u32)0xFFFFFFFF)

#define NVSP_PROTOCOL_VERSION_1		2
#define NVSP_PROTOCOL_VERSION_2		0x30002

enum {
	NVSP_MSG_TYPE_NONE = 0,

	/*               */
	NVSP_MSG_TYPE_INIT			= 1,
	NVSP_MSG_TYPE_INIT_COMPLETE		= 2,

	NVSP_VERSION_MSG_START			= 100,

	/*                    */
	NVSP_MSG1_TYPE_SEND_NDIS_VER		= NVSP_VERSION_MSG_START,

	NVSP_MSG1_TYPE_SEND_RECV_BUF,
	NVSP_MSG1_TYPE_SEND_RECV_BUF_COMPLETE,
	NVSP_MSG1_TYPE_REVOKE_RECV_BUF,

	NVSP_MSG1_TYPE_SEND_SEND_BUF,
	NVSP_MSG1_TYPE_SEND_SEND_BUF_COMPLETE,
	NVSP_MSG1_TYPE_REVOKE_SEND_BUF,

	NVSP_MSG1_TYPE_SEND_RNDIS_PKT,
	NVSP_MSG1_TYPE_SEND_RNDIS_PKT_COMPLETE,

	/*                    */
	NVSP_MSG2_TYPE_SEND_CHIMNEY_DELEGATED_BUF,
	NVSP_MSG2_TYPE_SEND_CHIMNEY_DELEGATED_BUF_COMP,
	NVSP_MSG2_TYPE_REVOKE_CHIMNEY_DELEGATED_BUF,

	NVSP_MSG2_TYPE_RESUME_CHIMNEY_RX_INDICATION,

	NVSP_MSG2_TYPE_TERMINATE_CHIMNEY,
	NVSP_MSG2_TYPE_TERMINATE_CHIMNEY_COMP,

	NVSP_MSG2_TYPE_INDICATE_CHIMNEY_EVENT,

	NVSP_MSG2_TYPE_SEND_CHIMNEY_PKT,
	NVSP_MSG2_TYPE_SEND_CHIMNEY_PKT_COMP,

	NVSP_MSG2_TYPE_POST_CHIMNEY_RECV_REQ,
	NVSP_MSG2_TYPE_POST_CHIMNEY_RECV_REQ_COMP,

	NVSP_MSG2_TYPE_ALLOC_RXBUF,
	NVSP_MSG2_TYPE_ALLOC_RXBUF_COMP,

	NVSP_MSG2_TYPE_FREE_RXBUF,

	NVSP_MSG2_TYPE_SEND_VMQ_RNDIS_PKT,
	NVSP_MSG2_TYPE_SEND_VMQ_RNDIS_PKT_COMP,

	NVSP_MSG2_TYPE_SEND_NDIS_CONFIG,

	NVSP_MSG2_TYPE_ALLOC_CHIMNEY_HANDLE,
	NVSP_MSG2_TYPE_ALLOC_CHIMNEY_HANDLE_COMP,
};

enum {
	NVSP_STAT_NONE = 0,
	NVSP_STAT_SUCCESS,
	NVSP_STAT_FAIL,
	NVSP_STAT_PROTOCOL_TOO_NEW,
	NVSP_STAT_PROTOCOL_TOO_OLD,
	NVSP_STAT_INVALID_RNDIS_PKT,
	NVSP_STAT_BUSY,
	NVSP_STAT_PROTOCOL_UNSUPPORTED,
	NVSP_STAT_MAX,
};

struct nvsp_message_header {
	u32 msg_type;
};

/*               */

/*
                                                                               
                                                                         
                                                            
 */
struct nvsp_message_init {
	u32 min_protocol_ver;
	u32 max_protocol_ver;
} __packed;

/*
                                                                        
                                                                            
                                                 
 */
struct nvsp_message_init_complete {
	u32 negotiated_protocol_ver;
	u32 max_mdl_chain_len;
	u32 status;
} __packed;

union nvsp_message_init_uber {
	struct nvsp_message_init init;
	struct nvsp_message_init_complete init_complete;
} __packed;

/*                    */

/*
                                                                               
                                                               
 */
struct nvsp_1_message_send_ndis_version {
	u32 ndis_major_ver;
	u32 ndis_minor_ver;
} __packed;

/*
                                                                               
                                                           
 */
struct nvsp_1_message_send_receive_buffer {
	u32 gpadl_handle;
	u16 id;
} __packed;

struct nvsp_1_receive_buffer_section {
	u32 offset;
	u32 sub_alloc_size;
	u32 num_sub_allocs;
	u32 end_offset;
} __packed;

/*
                                                                              
                                                                            
          
 */
struct nvsp_1_message_send_receive_buffer_complete {
	u32 status;
	u32 num_sections;

	/*
                                                                     
                                                                      
                                   
  */

	/*
                                                                   
                                                     
  */

	/*
                                                                
                                                                
                                                                
                                            
                                                       
  */

	struct nvsp_1_receive_buffer_section sections[1];
} __packed;

/*
                                                                               
                                                                          
         
 */
struct nvsp_1_message_revoke_receive_buffer {
	u16 id;
};

/*
                                                                            
                                                        
 */
struct nvsp_1_message_send_send_buffer {
	u32 gpadl_handle;
	u16 id;
} __packed;

/*
                                                                           
                                                                         
          
 */
struct nvsp_1_message_send_send_buffer_complete {
	u32 status;

	/*
                                                                       
                                                                       
                                                                
              
  */
	u32 section_size;
} __packed;

/*
                                                                            
                                                                              
 */
struct nvsp_1_message_revoke_send_buffer {
	u16 id;
};

/*
                                                                              
                                 
 */
struct nvsp_1_message_send_rndis_packet {
	/*
                                                                       
                                                                     
                                                         
  */
	u32 channel_type;

	/*
                                                                     
                                                                       
                                                                       
                                                        
  */
	u32 send_buf_section_index;
	u32 send_buf_section_size;
} __packed;

/*
                                                                               
                                                                         
                                                                              
 */
struct nvsp_1_message_send_rndis_packet_complete {
	u32 status;
};

union nvsp_1_message_uber {
	struct nvsp_1_message_send_ndis_version send_ndis_ver;

	struct nvsp_1_message_send_receive_buffer send_recv_buf;
	struct nvsp_1_message_send_receive_buffer_complete
						send_recv_buf_complete;
	struct nvsp_1_message_revoke_receive_buffer revoke_recv_buf;

	struct nvsp_1_message_send_send_buffer send_send_buf;
	struct nvsp_1_message_send_send_buffer_complete send_send_buf_complete;
	struct nvsp_1_message_revoke_send_buffer revoke_send_buf;

	struct nvsp_1_message_send_rndis_packet send_rndis_pkt;
	struct nvsp_1_message_send_rndis_packet_complete
						send_rndis_pkt_complete;
} __packed;


/*
                                           
 */
struct nvsp_2_vsc_capability {
	union {
		u64 data;
		struct {
			u64 vmq:1;
			u64 chimney:1;
			u64 sriov:1;
			u64 ieee8021q:1;
			u64 correlation_id:1;
		};
	};
} __packed;

struct nvsp_2_send_ndis_config {
	u32 mtu;
	u32 reserved;
	struct nvsp_2_vsc_capability capability;
} __packed;

/*                         */
struct nvsp_2_alloc_rxbuf {
	/*                                                            */
	u32 alloc_id;

	/*                                                            
                
  */
	u32 len;
} __packed;

/*                                  */
struct nvsp_2_alloc_rxbuf_comp {
	/*                                            */
	u32 status;

	u32 alloc_id;

	/*                                               */
	u32 gpadl_handle;

	/*                   */
	u64 recv_buf_id;
} __packed;

struct nvsp_2_free_rxbuf {
	u64 recv_buf_id;
} __packed;

union nvsp_2_message_uber {
	struct nvsp_2_send_ndis_config send_ndis_config;
	struct nvsp_2_alloc_rxbuf alloc_rxbuf;
	struct nvsp_2_alloc_rxbuf_comp alloc_rxbuf_comp;
	struct nvsp_2_free_rxbuf free_rxbuf;
} __packed;

union nvsp_all_messages {
	union nvsp_message_init_uber init_msg;
	union nvsp_1_message_uber v1_msg;
	union nvsp_2_message_uber v2_msg;
} __packed;

/*              */
struct nvsp_message {
	struct nvsp_message_header hdr;
	union nvsp_all_messages msg;
} __packed;


#define NETVSC_MTU 65536

#define NETVSC_RECEIVE_BUFFER_SIZE		(1024*1024*2)	/*     */

#define NETVSC_RECEIVE_BUFFER_ID		0xcafe

/*                              */
#define NETVSC_RECEIVE_PACKETLIST_COUNT		256

#define NETVSC_PACKET_SIZE                      2048

/*                             */
struct netvsc_device {
	struct hv_device *dev;

	u32 nvsp_version;

	atomic_t num_outstanding_sends;
	wait_queue_head_t wait_drain;
	bool start_remove;
	bool destroy;
	/*
                                                                   
          
  */
	struct list_head recv_pkt_list;
	spinlock_t recv_pkt_list_lock;

	/*                                                      */
	void *recv_buf;
	u32 recv_buf_size;
	u32 recv_buf_gpadl_handle;
	u32 recv_section_cnt;
	struct nvsp_1_receive_buffer_section *recv_section;

	/*                                         */
	struct completion channel_init_wait;
	struct nvsp_message channel_init_pkt;

	struct nvsp_message revoke_packet;
	/*                                          */

	struct net_device *ndev;

	/*                         */
	void *extension;
};

/*                        */
struct rndis_initialize_request {
	u32 req_id;
	u32 major_ver;
	u32 minor_ver;
	u32 max_xfer_size;
};

/*                            */
struct rndis_initialize_complete {
	u32 req_id;
	u32 status;
	u32 major_ver;
	u32 minor_ver;
	u32 dev_flags;
	u32 medium;
	u32 max_pkt_per_msg;
	u32 max_xfer_size;
	u32 pkt_alignment_factor;
	u32 af_list_offset;
	u32 af_list_size;
};

/*                                                                */
/*                                                                        */
struct rndis_co_address_family {
	u32 address_family;
	u32 major_ver;
	u32 minor_ver;
};

/*                  */
struct rndis_halt_request {
	u32 req_id;
};

/*                          */
struct rndis_query_request {
	u32 req_id;
	u32 oid;
	u32 info_buflen;
	u32 info_buf_offset;
	u32 dev_vc_handle;
};

/*                              */
struct rndis_query_complete {
	u32 req_id;
	u32 status;
	u32 info_buflen;
	u32 info_buf_offset;
};

/*                        */
struct rndis_set_request {
	u32 req_id;
	u32 oid;
	u32 info_buflen;
	u32 info_buf_offset;
	u32 dev_vc_handle;
};

/*                            */
struct rndis_set_complete {
	u32 req_id;
	u32 status;
};

/*                   */
struct rndis_reset_request {
	u32 reserved;
};

/*                       */
struct rndis_reset_complete {
	u32 status;
	u32 addressing_reset;
};

/*                             */
struct rndis_indicate_status {
	u32 status;
	u32 status_buflen;
	u32 status_buf_offset;
};

/*                                                       */
/*                                                                    */
struct rndis_diagnostic_info {
	u32 diag_status;
	u32 error_offset;
};

/*                       */
struct rndis_keepalive_request {
	u32 req_id;
};

/*                           */
struct rndis_keepalive_complete {
	u32 req_id;
	u32 status;
};

/*
                                                                             
                                                                        
                                                                     
 */
struct rndis_packet {
	u32 data_offset;
	u32 data_len;
	u32 oob_data_offset;
	u32 oob_data_len;
	u32 num_oob_data_elements;
	u32 per_pkt_info_offset;
	u32 per_pkt_info_len;
	u32 vc_handle;
	u32 reserved;
};

/*                                                           */
struct rndis_oobd {
	u32 size;
	u32 type;
	u32 class_info_offset;
};

/*                                                                 */
struct rndis_per_packet_info {
	u32 size;
	u32 type;
	u32 ppi_offset;
};

enum ndis_per_pkt_info_type {
	TCPIP_CHKSUM_PKTINFO,
	IPSEC_PKTINFO,
	TCP_LARGESEND_PKTINFO,
	CLASSIFICATION_HANDLE_PKTINFO,
	NDIS_RESERVED,
	SG_LIST_PKTINFO,
	IEEE_8021Q_INFO,
	ORIGINAL_PKTINFO,
	PACKET_CANCEL_ID,
	ORIGINAL_NET_BUFLIST,
	CACHED_NET_BUFLIST,
	SHORT_PKT_PADINFO,
	MAX_PER_PKT_INFO
};

struct ndis_pkt_8021q_info {
	union {
		struct {
			u32 pri:3; /*               */
			u32 cfi:1; /*                     */
			u32 vlanid:12; /*         */
			u32 reserved:16;
		};
		u32 value;
	};
};

#define NDIS_VLAN_PPI_SIZE (sizeof(struct rndis_per_packet_info) + \
		sizeof(struct ndis_pkt_8021q_info))

/*                                                                 */
/*                                 */
struct rndis_config_parameter_info {
	u32 parameter_name_offset;
	u32 parameter_name_length;
	u32 parameter_type;
	u32 parameter_value_offset;
	u32 parameter_value_length;
};

/*                                                                */
#define RNDIS_CONFIG_PARAM_TYPE_INTEGER     0
#define RNDIS_CONFIG_PARAM_TYPE_STRING      2

/*                                                          */
/*                                       */

/*                                */
struct rcondis_mp_create_vc {
	u32 req_id;
	u32 ndis_vc_handle;
};

/*                                    */
struct rcondis_mp_create_vc_complete {
	u32 req_id;
	u32 dev_vc_handle;
	u32 status;
};

/*                                */
struct rcondis_mp_delete_vc {
	u32 req_id;
	u32 dev_vc_handle;
};

/*                                    */
struct rcondis_mp_delete_vc_complete {
	u32 req_id;
	u32 status;
};

/*                                    */
struct rcondis_mp_query_request {
	u32 req_id;
	u32 request_type;
	u32 oid;
	u32 dev_vc_handle;
	u32 info_buflen;
	u32 info_buf_offset;
};

/*                                  */
struct rcondis_mp_set_request {
	u32 req_id;
	u32 request_type;
	u32 oid;
	u32 dev_vc_handle;
	u32 info_buflen;
	u32 info_buf_offset;
};

/*                              */
struct rcondis_indicate_status {
	u32 ndis_vc_handle;
	u32 status;
	u32 status_buflen;
	u32 status_buf_offset;
};

/*                           */
struct rcondis_specific_parameters {
	u32 parameter_type;
	u32 parameter_length;
	u32 parameter_lffset;
};

struct rcondis_media_parameters {
	u32 flags;
	u32 reserved1;
	u32 reserved2;
	struct rcondis_specific_parameters media_specific;
};

struct rndis_flowspec {
	u32 token_rate;
	u32 token_bucket_size;
	u32 peak_bandwidth;
	u32 latency;
	u32 delay_variation;
	u32 service_type;
	u32 max_sdu_size;
	u32 minimum_policed_size;
};

struct rcondis_call_manager_parameters {
	struct rndis_flowspec transmit;
	struct rndis_flowspec receive;
	struct rcondis_specific_parameters call_mgr_specific;
};

/*                                  */
struct rcondis_mp_activate_vc_request {
	u32 req_id;
	u32 flags;
	u32 dev_vc_handle;
	u32 media_params_offset;
	u32 media_params_length;
	u32 call_mgr_params_offset;
	u32 call_mgr_params_length;
};

/*                                      */
struct rcondis_mp_activate_vc_complete {
	u32 req_id;
	u32 status;
};

/*                                    */
struct rcondis_mp_deactivate_vc_request {
	u32 req_id;
	u32 flags;
	u32 dev_vc_handle;
};

/*                                        */
struct rcondis_mp_deactivate_vc_complete {
	u32 req_id;
	u32 status;
};


/*                                      */
union rndis_message_container {
	struct rndis_packet pkt;
	struct rndis_initialize_request init_req;
	struct rndis_halt_request halt_req;
	struct rndis_query_request query_req;
	struct rndis_set_request set_req;
	struct rndis_reset_request reset_req;
	struct rndis_keepalive_request keep_alive_req;
	struct rndis_indicate_status indicate_status;
	struct rndis_initialize_complete init_complete;
	struct rndis_query_complete query_complete;
	struct rndis_set_complete set_complete;
	struct rndis_reset_complete reset_complete;
	struct rndis_keepalive_complete keep_alive_complete;
	struct rcondis_mp_create_vc co_miniport_create_vc;
	struct rcondis_mp_delete_vc co_miniport_delete_vc;
	struct rcondis_indicate_status co_indicate_status;
	struct rcondis_mp_activate_vc_request co_miniport_activate_vc;
	struct rcondis_mp_deactivate_vc_request co_miniport_deactivate_vc;
	struct rcondis_mp_create_vc_complete co_miniport_create_vc_complete;
	struct rcondis_mp_delete_vc_complete co_miniport_delete_vc_complete;
	struct rcondis_mp_activate_vc_complete co_miniport_activate_vc_complete;
	struct rcondis_mp_deactivate_vc_complete
		co_miniport_deactivate_vc_complete;
};

/*                            */
struct rndis_message {
	u32 ndis_msg_type;

	/*                                                  */
	/*                                       */
	u32 msg_len;

	/*                */
	union rndis_message_container msg;
};


struct rndis_filter_packet {
	void *completion_ctx;
	void (*completion)(void *context);
	struct rndis_message msg;
};

/*              */

/*                                                             */
/*                                                           */
#define RNDIS_MESSAGE_SIZE(msg)				\
	(sizeof(msg) + (sizeof(struct rndis_message) -	\
	 sizeof(union rndis_message_container)))

/*                                                 */
#define MESSAGE_TO_INFO_BUFFER(msg)				\
	(((unsigned char *)(msg)) + msg->info_buf_offset)

/*                                                   */
#define MESSAGE_TO_STATUS_BUFFER(msg)			\
	(((unsigned char *)(msg)) + msg->status_buf_offset)

/*                                                 */
#define MESSAGE_TO_OOBD_BUFFER(msg)				\
	(((unsigned char *)(msg)) + msg->oob_data_offset)

/*                                                 */
#define MESSAGE_TO_DATA_BUFFER(msg)				\
	(((unsigned char *)(msg)) + msg->per_pkt_info_offset)

/*                                                            */
#define RNDIS_MESSAGE_PTR_TO_MESSAGE_PTR(rndis_msg)		\
	((void *) &rndis_msg->msg)

/*                                                            */
#define RNDIS_MESSAGE_RAW_PTR_TO_MESSAGE_PTR(rndis_msg)	\
	((void *) rndis_msg)


#define __struct_bcount(x)



#define RNDIS_HEADER_SIZE	(sizeof(struct rndis_message) - \
				 sizeof(union rndis_message_container))

#define NDIS_PACKET_TYPE_DIRECTED	0x00000001
#define NDIS_PACKET_TYPE_MULTICAST	0x00000002
#define NDIS_PACKET_TYPE_ALL_MULTICAST	0x00000004
#define NDIS_PACKET_TYPE_BROADCAST	0x00000008
#define NDIS_PACKET_TYPE_SOURCE_ROUTING	0x00000010
#define NDIS_PACKET_TYPE_PROMISCUOUS	0x00000020
#define NDIS_PACKET_TYPE_SMT		0x00000040
#define NDIS_PACKET_TYPE_ALL_LOCAL	0x00000080
#define NDIS_PACKET_TYPE_GROUP		0x00000100
#define NDIS_PACKET_TYPE_ALL_FUNCTIONAL	0x00000200
#define NDIS_PACKET_TYPE_FUNCTIONAL	0x00000400
#define NDIS_PACKET_TYPE_MAC_FRAME	0x00000800



#endif /*               */
