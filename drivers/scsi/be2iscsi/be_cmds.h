/**
 * Copyright (C) 2005 - 2013 Emulex
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.  The full GNU General
 * Public License is included in this distribution in the file called COPYING.
 *
 * Contact Information:
 * linux-drivers@emulex.com
 *
 * Emulex
 * 3333 Susan Street
 * Costa Mesa, CA 92626
 */

#ifndef BEISCSI_CMDS_H
#define BEISCSI_CMDS_H

/* 
                                                                         
                                                                       
                                                                            
                        
                                                                          
 */
struct be_sge {
	u32 pa_lo;
	u32 pa_hi;
	u32 len;
};

#define MCC_WRB_SGE_CNT_SHIFT 3	/*                       */
#define MCC_WRB_SGE_CNT_MASK 0x1F	/*                       */
struct be_mcc_wrb {
	u32 embedded;		/*         */
	u32 payload_length;	/*         */
	u32 tag0;		/*         */
	u32 tag1;		/*         */
	u32 rsvd;		/*         */
	union {
		u8 embedded_payload[236];	/*                       */
		struct be_sge sgl[19];	/*                           */
	} payload;
};

#define CQE_FLAGS_VALID_MASK (1 << 31)
#define CQE_FLAGS_ASYNC_MASK (1 << 30)
#define CQE_FLAGS_COMPLETED_MASK	(1 << 28)
#define CQE_FLAGS_CONSUMED_MASK		(1 << 27)

/*                   */
#define MCC_STATUS_SUCCESS 0x0
#define MCC_STATUS_FAILED 0x1
#define MCC_STATUS_ILLEGAL_REQUEST 0x2
#define MCC_STATUS_ILLEGAL_FIELD 0x3
#define MCC_STATUS_INSUFFICIENT_BUFFER 0x4

#define CQE_STATUS_COMPL_MASK 0xFFFF
#define CQE_STATUS_COMPL_SHIFT 0	/*             */
#define CQE_STATUS_EXTD_MASK 0xFFFF
#define CQE_STATUS_EXTD_SHIFT 16		/*             */
#define CQE_STATUS_ADDL_MASK	0xFF00
#define CQE_STATUS_MASK	0xFF
#define CQE_STATUS_ADDL_SHIFT	0x08
#define CQE_STATUS_WRB_MASK	0xFF0000
#define CQE_STATUS_WRB_SHIFT	16
#define BEISCSI_HOST_MBX_TIMEOUT (110 * 1000)
#define BEISCSI_FW_MBX_TIMEOUT	100

/*                  */
#define MBX_CMD_VER2	0x02

struct be_mcc_compl {
	u32 status;		/*         */
	u32 tag0;		/*         */
	u32 tag1;		/*         */
	u32 flags;		/*         */
};

/*                                       */
/* 
                                             
                                                                          
                                                                              
                                                                         
                                                                              
                                                                       
                                                                         
               
 */
#define MPU_MAILBOX_DB_OFFSET	0x160
#define MPU_MAILBOX_DB_RDY_MASK	0x1	/*       */
#define MPU_MAILBOX_DB_HI_MASK	0x2	/*       */

/*                                        */
#define MPU_EP_SEMAPHORE_OFFSET 0xac
#define EP_SEMAPHORE_POST_STAGE_MASK 0x0000FFFF
#define EP_SEMAPHORE_POST_ERR_MASK 0x1
#define EP_SEMAPHORE_POST_ERR_SHIFT 31

/*                                   */
#define DB_MCCQ_OFFSET 0x140
#define DB_MCCQ_RING_ID_MASK 0x7FF		/*             */
/*                          */
#define DB_MCCQ_NUM_POSTED_SHIFT 16		/*              */

/*                                */
#define POST_STAGE_ARMFW_RDY		0xc000	/*                      */

/* 
                                                              
                                       
 */
#define ASYNC_TRAILER_EVENT_CODE_SHIFT	8	/*             */
#define ASYNC_TRAILER_EVENT_CODE_MASK	0xFF
#define ASYNC_EVENT_CODE_LINK_STATE	0x1
struct be_async_event_trailer {
	u32 code;
};

enum {
	ASYNC_EVENT_LINK_DOWN = 0x0,
	ASYNC_EVENT_LINK_UP = 0x1,
	ASYNC_EVENT_LOGICAL = 0x2
};

/* 
                                                                       
                                 
 */
struct be_async_event_link_state {
	u8 physical_port;
	u8 port_link_status;
	u8 port_duplex;
	u8 port_speed;
#define BEISCSI_PHY_LINK_FAULT_NONE	0x00
#define BEISCSI_PHY_LINK_FAULT_LOCAL	0x01
#define BEISCSI_PHY_LINK_FAULT_REMOTE	0x02
	u8 port_fault;
	u8 rsvd0[7];
	struct be_async_event_trailer trailer;
} __packed;

struct be_mcc_mailbox {
	struct be_mcc_wrb wrb;
	struct be_mcc_compl compl;
};

/*                                    */
#define CMD_SUBSYSTEM_COMMON    0x1
#define CMD_SUBSYSTEM_ISCSI     0x2
#define CMD_SUBSYSTEM_ETH       0x3
#define CMD_SUBSYSTEM_ISCSI_INI 0x6
#define CMD_COMMON_TCP_UPLOAD   0x1

/* 
                                                         
                                                            
 */
#define OPCODE_COMMON_CQ_CREATE				12
#define OPCODE_COMMON_EQ_CREATE				13
#define OPCODE_COMMON_MCC_CREATE			21
#define OPCODE_COMMON_GET_CNTL_ATTRIBUTES		32
#define OPCODE_COMMON_GET_FW_VERSION			35
#define OPCODE_COMMON_MODIFY_EQ_DELAY			41
#define OPCODE_COMMON_FIRMWARE_CONFIG			42
#define OPCODE_COMMON_MCC_DESTROY			53
#define OPCODE_COMMON_CQ_DESTROY			54
#define OPCODE_COMMON_EQ_DESTROY			55
#define OPCODE_COMMON_QUERY_FIRMWARE_CONFIG		58
#define OPCODE_COMMON_FUNCTION_RESET			61

/* 
                                                               
                              
                                                            
 */
#define OPCODE_COMMON_ISCSI_CFG_POST_SGL_PAGES		2
#define OPCODE_COMMON_ISCSI_CFG_REMOVE_SGL_PAGES        3
#define OPCODE_COMMON_ISCSI_NTWK_GET_NIC_CONFIG		7
#define OPCODE_COMMON_ISCSI_NTWK_SET_VLAN		14
#define OPCODE_COMMON_ISCSI_NTWK_CONFIG_STATELESS_IP_ADDR	17
#define OPCODE_COMMON_ISCSI_NTWK_REL_STATELESS_IP_ADDR	18
#define OPCODE_COMMON_ISCSI_NTWK_MODIFY_IP_ADDR		21
#define OPCODE_COMMON_ISCSI_NTWK_GET_DEFAULT_GATEWAY	22
#define OPCODE_COMMON_ISCSI_NTWK_MODIFY_DEFAULT_GATEWAY 23
#define OPCODE_COMMON_ISCSI_NTWK_GET_ALL_IF_ID		24
#define OPCODE_COMMON_ISCSI_NTWK_GET_IF_INFO		25
#define OPCODE_COMMON_ISCSI_SET_FRAGNUM_BITS_FOR_SGL_CRA 61
#define OPCODE_COMMON_ISCSI_DEFQ_CREATE                 64
#define OPCODE_COMMON_ISCSI_DEFQ_DESTROY		65
#define OPCODE_COMMON_ISCSI_WRBQ_CREATE			66
#define OPCODE_COMMON_ISCSI_WRBQ_DESTROY		67

struct be_cmd_req_hdr {
	u8 opcode;		/*         */
	u8 subsystem;		/*         */
	u8 port_number;		/*         */
	u8 domain;		/*         */
	u32 timeout;		/*         */
	u32 request_length;	/*         */
	u8 version;		/*         */
	u8 rsvd0[3];		/*         */
};

struct be_cmd_resp_hdr {
	u32 info;		/*         */
	u32 status;		/*         */
	u32 response_length;	/*         */
	u32 actual_resp_len;	/*         */
};

struct phys_addr {
	u32 lo;
	u32 hi;
};

/*                         
                           
                          */

/* 
                                                                              
                                                                
 */
struct amap_eq_context {
	u8 cidx[13];		/*         */
	u8 rsvd0[3];		/*         */
	u8 epidx[13];		/*         */
	u8 valid;		/*         */
	u8 rsvd1;		/*         */
	u8 size;		/*         */
	u8 pidx[13];		/*         */
	u8 rsvd2[3];		/*         */
	u8 pd[10];		/*         */
	u8 count[3];		/*         */
	u8 solevent;		/*         */
	u8 stalled;		/*         */
	u8 armed;		/*         */
	u8 rsvd3[4];		/*         */
	u8 func[8];		/*         */
	u8 rsvd4;		/*         */
	u8 delaymult[10];	/*         */
	u8 rsvd5[2];		/*         */
	u8 phase[2];		/*         */
	u8 nodelay;		/*         */
	u8 rsvd6[4];		/*         */
	u8 rsvd7[32];		/*         */
} __packed;

struct be_cmd_req_eq_create {
	struct be_cmd_req_hdr hdr;	/*       */
	u16 num_pages;		/*       */
	u16 rsvd0;		/*       */
	u8 context[sizeof(struct amap_eq_context) / 8];	/*       */
	struct phys_addr pages[8];
} __packed;

struct be_cmd_resp_eq_create {
	struct be_cmd_resp_hdr resp_hdr;
	u16 eq_id;		/*       */
	u16 rsvd0;		/*       */
} __packed;

struct mgmt_chap_format {
	u32 flags;
	u8  intr_chap_name[256];
	u8  intr_secret[16];
	u8  target_chap_name[256];
	u8  target_secret[16];
	u16 intr_chap_name_length;
	u16 intr_secret_length;
	u16 target_chap_name_length;
	u16 target_secret_length;
} __packed;

struct mgmt_auth_method_format {
	u8	auth_method_type;
	u8	padding[3];
	struct	mgmt_chap_format chap;
} __packed;

struct mgmt_conn_login_options {
	u8 flags;
	u8 header_digest;
	u8 data_digest;
	u8 rsvd0;
	u32 max_recv_datasegment_len_ini;
	u32 max_recv_datasegment_len_tgt;
	u32 tcp_mss;
	u32 tcp_window_size;
	struct	mgmt_auth_method_format auth_data;
} __packed;

struct ip_addr_format {
	u16 size_of_structure;
	u8 reserved;
	u8 ip_type;
	u8 addr[16];
	u32 rsvd0;
} __packed;

struct mgmt_conn_info {
	u32	connection_handle;
	u32	connection_status;
	u16	src_port;
	u16	dest_port;
	u16	dest_port_redirected;
	u16	cid;
	u32	estimated_throughput;
	struct	ip_addr_format	src_ipaddr;
	struct	ip_addr_format	dest_ipaddr;
	struct	ip_addr_format	dest_ipaddr_redirected;
	struct	mgmt_conn_login_options	negotiated_login_options;
} __packed;

struct mgmt_session_login_options {
	u8	flags;
	u8	error_recovery_level;
	u16	rsvd0;
	u32	first_burst_length;
	u32	max_burst_length;
	u16	max_connections;
	u16	max_outstanding_r2t;
	u16	default_time2wait;
	u16	default_time2retain;
} __packed;

struct mgmt_session_info {
	u32	session_handle;
	u32	status;
	u8	isid[6];
	u16	tsih;
	u32	session_flags;
	u16	conn_count;
	u16	pad;
	u8	target_name[224];
	u8	initiator_iscsiname[224];
	struct	mgmt_session_login_options negotiated_login_options;
	struct	mgmt_conn_info	conn_list[1];
} __packed;

struct be_cmd_get_session_req {
	struct be_cmd_req_hdr hdr;
	u32 session_handle;
} __packed;

struct be_cmd_get_session_resp {
	struct be_cmd_resp_hdr hdr;
	struct mgmt_session_info session_info;
} __packed;

struct mac_addr {
	u16 size_of_structure;
	u8 addr[ETH_ALEN];
} __packed;

struct be_cmd_get_boot_target_req {
	struct be_cmd_req_hdr hdr;
} __packed;

struct be_cmd_get_boot_target_resp {
	struct be_cmd_resp_hdr hdr;
	u32  boot_session_count;
	int  boot_session_handle;
};

struct be_cmd_reopen_session_req {
	struct be_cmd_req_hdr hdr;
#define BE_REOPEN_ALL_SESSIONS  0x00
#define BE_REOPEN_BOOT_SESSIONS 0x01
#define BE_REOPEN_A_SESSION     0x02
	u16 reopen_type;
	u16 rsvd;
	u32 session_handle;
} __packed;

struct be_cmd_reopen_session_resp {
	struct be_cmd_resp_hdr hdr;
	u32 rsvd;
	u32 session_handle;
} __packed;


struct be_cmd_mac_query_req {
	struct be_cmd_req_hdr hdr;
	u8 type;
	u8 permanent;
	u16 if_id;
} __packed;

struct be_cmd_get_mac_resp {
	struct be_cmd_resp_hdr hdr;
	struct mac_addr mac;
};

struct be_ip_addr_subnet_format {
	u16 size_of_structure;
	u8 ip_type;
	u8 ipv6_prefix_length;
	u8 addr[16];
	u8 subnet_mask[16];
	u32 rsvd0;
} __packed;

struct be_cmd_get_if_info_req {
	struct be_cmd_req_hdr hdr;
	u32 interface_hndl;
	u32 ip_type;
} __packed;

struct be_cmd_get_if_info_resp {
	struct be_cmd_req_hdr hdr;
	u32 interface_hndl;
	u32 vlan_priority;
	u32 ip_addr_count;
	u32 dhcp_state;
	struct be_ip_addr_subnet_format ip_addr;
} __packed;

struct be_ip_addr_record {
	u32 action;
	u32 interface_hndl;
	struct be_ip_addr_subnet_format ip_addr;
	u32 status;
} __packed;

struct be_ip_addr_record_params {
	u32 record_entry_count;
	struct be_ip_addr_record ip_record;
} __packed;

struct be_cmd_set_ip_addr_req {
	struct be_cmd_req_hdr hdr;
	struct be_ip_addr_record_params ip_params;
} __packed;


struct be_cmd_set_dhcp_req {
	struct be_cmd_req_hdr hdr;
	u32 interface_hndl;
	u32 ip_type;
	u32 flags;
	u32 retry_count;
} __packed;

struct be_cmd_rel_dhcp_req {
	struct be_cmd_req_hdr hdr;
	u32 interface_hndl;
	u32 ip_type;
} __packed;

struct be_cmd_set_def_gateway_req {
	struct be_cmd_req_hdr hdr;
	u32 action;
	struct ip_addr_format ip_addr;
} __packed;

struct be_cmd_get_def_gateway_req {
	struct be_cmd_req_hdr hdr;
	u32 ip_type;
} __packed;

struct be_cmd_get_def_gateway_resp {
	struct be_cmd_req_hdr hdr;
	struct ip_addr_format ip_addr;
} __packed;

#define BEISCSI_VLAN_DISABLE	0xFFFF
struct be_cmd_set_vlan_req {
	struct be_cmd_req_hdr hdr;
	u32 interface_hndl;
	u32 vlan_priority;
} __packed;
/*                                                        */
/* 
                                                                              
                                                                
 */
struct amap_cq_context {
	u8 cidx[11];		/*         */
	u8 rsvd0;		/*         */
	u8 coalescwm[2];	/*         */
	u8 nodelay;		/*         */
	u8 epidx[11];		/*         */
	u8 rsvd1;		/*         */
	u8 count[2];		/*         */
	u8 valid;		/*         */
	u8 solevent;		/*         */
	u8 eventable;		/*         */
	u8 pidx[11];		/*         */
	u8 rsvd2;		/*         */
	u8 pd[10];		/*         */
	u8 eqid[8];		/*         */
	u8 stalled;		/*         */
	u8 armed;		/*         */
	u8 rsvd3[4];		/*         */
	u8 func[8];		/*         */
	u8 rsvd4[20];		/*         */
	u8 rsvd5[32];		/*         */
} __packed;

struct amap_cq_context_v2 {
	u8 rsvd0[12];   /*         */
	u8 coalescwm[2];    /*         */
	u8 nodelay;     /*         */
	u8 rsvd1[12];   /*         */
	u8 count[2];    /*         */
	u8 valid;       /*         */
	u8 rsvd2;       /*         */
	u8 eventable;   /*         */
	u8 eqid[16];    /*         */
	u8 rsvd3[15];   /*         */
	u8 armed;       /*         */
	u8 cqecount[16];/*         */
	u8 rsvd4[16];   /*         */
	u8 rsvd5[32];   /*         */
};

struct be_cmd_req_cq_create {
	struct be_cmd_req_hdr hdr;
	u16 num_pages;
	u8 page_size;
	u8 rsvd0;
	u8 context[sizeof(struct amap_cq_context) / 8];
	struct phys_addr pages[4];
} __packed;

struct be_cmd_resp_cq_create {
	struct be_cmd_resp_hdr hdr;
	u16 cq_id;
	u16 rsvd0;
} __packed;

/*                                                          */
/* 
                                                                              
                                                                
 */
struct amap_mcc_context {
	u8 con_index[14];
	u8 rsvd0[2];
	u8 ring_size[4];
	u8 fetch_wrb;
	u8 fetch_r2t;
	u8 cq_id[10];
	u8 prod_index[14];
	u8 fid[8];
	u8 pdid[9];
	u8 valid;
	u8 rsvd1[32];
	u8 rsvd2[32];
} __packed;

struct be_cmd_req_mcc_create {
	struct be_cmd_req_hdr hdr;
	u16 num_pages;
	u16 rsvd0;
	u8 context[sizeof(struct amap_mcc_context) / 8];
	struct phys_addr pages[8];
} __packed;

struct be_cmd_resp_mcc_create {
	struct be_cmd_resp_hdr hdr;
	u16 id;
	u16 rsvd0;
} __packed;

/*                                                         */
/*                               */
enum {
	QTYPE_EQ = 1,
	QTYPE_CQ,
	QTYPE_MCCQ,
	QTYPE_WRBQ,
	QTYPE_DPDUQ,
	QTYPE_SGL
};

struct be_cmd_req_q_destroy {
	struct be_cmd_req_hdr hdr;
	u16 id;
	u16 bypass_flush;	/*                             */
} __packed;

struct macaddr {
	u8 byte[ETH_ALEN];
};

struct be_cmd_req_mcast_mac_config {
	struct be_cmd_req_hdr hdr;
	u16 num_mac;
	u8 promiscuous;
	u8 interface_id;
	struct macaddr mac[32];
} __packed;

static inline void *embedded_payload(struct be_mcc_wrb *wrb)
{
	return wrb->payload.embedded_payload;
}

static inline struct be_sge *nonembedded_sgl(struct be_mcc_wrb *wrb)
{
	return &wrb->payload.sgl[0];
}

/*                                                      */
struct be_cmd_req_modify_eq_delay {
	struct be_cmd_req_hdr hdr;
	u32 num_eq;
	struct {
		u32 eq_id;
		u32 phase;
		u32 delay_multiplier;
	} delay[8];
} __packed;

/*                                                   */

#define ETH_ALEN	6

struct be_cmd_get_nic_conf_req {
	struct be_cmd_req_hdr hdr;
	u32 nic_port_count;
	u32 speed;
	u32 max_speed;
	u32 link_state;
	u32 max_frame_size;
	u16 size_of_structure;
	u8 mac_address[ETH_ALEN];
	u32 rsvd[23];
};

struct be_cmd_get_nic_conf_resp {
	struct be_cmd_resp_hdr hdr;
	u32 nic_port_count;
	u32 speed;
	u32 max_speed;
	u32 link_state;
	u32 max_frame_size;
	u16 size_of_structure;
	u8 mac_address[6];
	u32 rsvd[23];
};

#define BEISCSI_ALIAS_LEN 32

struct be_cmd_hba_name {
	struct be_cmd_req_hdr hdr;
	u16 flags;
	u16 rsvd0;
	u8 initiator_name[ISCSI_NAME_LEN];
	u8 initiator_alias[BEISCSI_ALIAS_LEN];
} __packed;

struct be_cmd_ntwk_link_status_req {
	struct be_cmd_req_hdr hdr;
	u32 rsvd0;
} __packed;

/*                       */
#define BE2ISCSI_LINK_SPEED_ZERO	0x00
#define BE2ISCSI_LINK_SPEED_10MBPS	0x01
#define BE2ISCSI_LINK_SPEED_100MBPS	0x02
#define BE2ISCSI_LINK_SPEED_1GBPS	0x03
#define BE2ISCSI_LINK_SPEED_10GBPS	0x04
struct be_cmd_ntwk_link_status_resp {
	struct be_cmd_resp_hdr hdr;
	u8 phys_port;
	u8 mac_duplex;
	u8 mac_speed;
	u8 mac_fault;
	u8 mgmt_mac_duplex;
	u8 mgmt_mac_speed;
	u16 qos_link_speed;
	u32 logical_link_speed;
} __packed;

int beiscsi_cmd_eq_create(struct be_ctrl_info *ctrl,
			  struct be_queue_info *eq, int eq_delay);

int beiscsi_cmd_cq_create(struct be_ctrl_info *ctrl,
			  struct be_queue_info *cq, struct be_queue_info *eq,
			  bool sol_evts, bool no_delay,
			  int num_cqe_dma_coalesce);

int beiscsi_cmd_q_destroy(struct be_ctrl_info *ctrl, struct be_queue_info *q,
			  int type);
int beiscsi_cmd_mccq_create(struct beiscsi_hba *phba,
			struct be_queue_info *mccq,
			struct be_queue_info *cq);

int be_poll_mcc(struct be_ctrl_info *ctrl);
int mgmt_check_supported_fw(struct be_ctrl_info *ctrl,
				      struct beiscsi_hba *phba);
unsigned int be_cmd_get_initname(struct beiscsi_hba *phba);
unsigned int be_cmd_get_port_speed(struct beiscsi_hba *phba);

void free_mcc_tag(struct be_ctrl_info *ctrl, unsigned int tag);

int beiscsi_mccq_compl(struct beiscsi_hba *phba,
			uint32_t tag, struct be_mcc_wrb **wrb, void *cmd_va);
/*                 */
int be_cmd_fw_initialize(struct be_ctrl_info *ctrl);
int be_cmd_fw_uninit(struct be_ctrl_info *ctrl);

struct be_mcc_wrb *wrb_from_mbox(struct be_dma_mem *mbox_mem);
struct be_mcc_wrb *wrb_from_mccq(struct beiscsi_hba *phba);
int be_mcc_notify_wait(struct beiscsi_hba *phba);
void be_mcc_notify(struct beiscsi_hba *phba);
unsigned int alloc_mcc_tag(struct beiscsi_hba *phba);
void beiscsi_async_link_state_process(struct beiscsi_hba *phba,
		struct be_async_event_link_state *evt);
int be_mcc_compl_process_isr(struct be_ctrl_info *ctrl,
				    struct be_mcc_compl *compl);

int be_mbox_notify(struct be_ctrl_info *ctrl);

int be_cmd_create_default_pdu_queue(struct be_ctrl_info *ctrl,
				    struct be_queue_info *cq,
				    struct be_queue_info *dq, int length,
				    int entry_size);

int be_cmd_iscsi_post_sgl_pages(struct be_ctrl_info *ctrl,
				struct be_dma_mem *q_mem, u32 page_offset,
				u32 num_pages);

int beiscsi_cmd_reset_function(struct beiscsi_hba *phba);

int be_cmd_wrbq_create(struct be_ctrl_info *ctrl, struct be_dma_mem *q_mem,
		       struct be_queue_info *wrbq);

bool is_link_state_evt(u32 trailer);

/*                         */
int be_cmd_set_vlan(struct beiscsi_hba *phba, uint16_t vlan_tag);

struct be_default_pdu_context {
	u32 dw[4];
} __packed;

struct amap_be_default_pdu_context {
	u8 dbuf_cindex[13];	/*         */
	u8 rsvd0[3];		/*         */
	u8 ring_size[4];	/*         */
	u8 ring_state[4];	/*         */
	u8 rsvd1[8];		/*         */
	u8 dbuf_pindex[13];	/*         */
	u8 rsvd2;		/*         */
	u8 pci_func_id[8];	/*         */
	u8 rx_pdid[9];		/*         */
	u8 rx_pdid_valid;	/*         */
	u8 default_buffer_size[16];	/*         */
	u8 cq_id_recv[10];	/*         */
	u8 rx_pdid_not_valid;	/*         */
	u8 rsvd3[5];		/*         */
	u8 rsvd4[32];		/*         */
} __packed;

struct amap_default_pdu_context_ext {
	u8 rsvd0[16];   /*         */
	u8 ring_size[4];    /*         */
	u8 rsvd1[12];   /*         */
	u8 rsvd2[22];   /*         */
	u8 rx_pdid[9];  /*         */
	u8 rx_pdid_valid;   /*         */
	u8 default_buffer_size[16]; /*         */
	u8 cq_id_recv[16];  /*         */
	u8 rsvd3[32];   /*         */
} __packed;

struct be_defq_create_req {
	struct be_cmd_req_hdr hdr;
	u16 num_pages;
	u8 ulp_num;
	u8 rsvd0;
	struct be_default_pdu_context context;
	struct phys_addr pages[8];
} __packed;

struct be_defq_create_resp {
	struct be_cmd_req_hdr hdr;
	u16 id;
	u16 rsvd0;
} __packed;

struct be_post_sgl_pages_req {
	struct be_cmd_req_hdr hdr;
	u16 num_pages;
	u16 page_offset;
	u32 rsvd0;
	struct phys_addr pages[26];
	u32 rsvd1;
} __packed;

struct be_wrbq_create_req {
	struct be_cmd_req_hdr hdr;
	u16 num_pages;
	u8 ulp_num;
	u8 rsvd0;
	struct phys_addr pages[8];
} __packed;

struct be_wrbq_create_resp {
	struct be_cmd_resp_hdr resp_hdr;
	u16 cid;
	u16 rsvd0;
} __packed;

#define SOL_CID_MASK		0x0000FFC0
#define SOL_CODE_MASK		0x0000003F
#define SOL_WRB_INDEX_MASK	0x00FF0000
#define SOL_CMD_WND_MASK	0xFF000000
#define SOL_RES_CNT_MASK	0x7FFFFFFF
#define SOL_EXP_CMD_SN_MASK	0xFFFFFFFF
#define SOL_HW_STS_MASK		0x000000FF
#define SOL_STS_MASK		0x0000FF00
#define SOL_RESP_MASK		0x00FF0000
#define SOL_FLAGS_MASK		0x7F000000
#define SOL_S_MASK		0x80000000

struct sol_cqe {
	u32 dw[4];
};

struct amap_sol_cqe {
	u8 hw_sts[8];		/*         */
	u8 i_sts[8];		/*         */
	u8 i_resp[8];		/*         */
	u8 i_flags[7];		/*         */
	u8 s;			/*         */
	u8 i_exp_cmd_sn[32];	/*         */
	u8 code[6];		/*         */
	u8 cid[10];		/*         */
	u8 wrb_index[8];	/*         */
	u8 i_cmd_wnd[8];	/*         */
	u8 i_res_cnt[31];	/*         */
	u8 valid;		/*         */
} __packed;

#define SOL_ICD_INDEX_MASK	0x0003FFC0
struct amap_sol_cqe_ring {
	u8 hw_sts[8];		/*         */
	u8 i_sts[8];		/*         */
	u8 i_resp[8];		/*         */
	u8 i_flags[7];		/*         */
	u8 s;			/*         */
	u8 i_exp_cmd_sn[32];	/*         */
	u8 code[6];		/*         */
	u8 icd_index[12];	/*         */
	u8 rsvd[6];		/*         */
	u8 i_cmd_wnd[8];	/*         */
	u8 i_res_cnt[31];	/*         */
	u8 valid;		/*         */
} __packed;

struct amap_sol_cqe_v2 {
	u8 hw_sts[8];   /*         */
	u8 i_sts[8];    /*         */
	u8 wrb_index[16];   /*         */
	u8 i_exp_cmd_sn[32];    /*         */
	u8 code[6]; /*         */
	u8 cmd_cmpl;    /*         */
	u8 rsvd0;   /*         */
	u8 i_cmd_wnd[8];    /*         */
	u8 cid[13]; /*         */
	u8 u;   /*         */
	u8 o;   /*         */
	u8 s;   /*         */
	u8 i_res_cnt[31];   /*         */
	u8 valid;   /*         */
} __packed;

struct common_sol_cqe {
	u32 exp_cmdsn;
	u32 res_cnt;
	u16 wrb_index;
	u16 cid;
	u8 hw_sts;
	u8 cmd_wnd;
	u8 res_flag; /*                          */
	u8 i_resp; /*                                                       */
	u8 i_flags; /*                               */
	u8 i_sts; /*                                                         */
};

/*                                          */
struct amap_it_dmsg_cqe {
	u8 ack_num[32]; /*         */
	u8 pdu_bytes_rcvd[32];  /*         */
	u8 code[6]; /*         */
	u8 cid[10]; /*         */
	u8 wrb_idx[8];  /*         */
	u8 rsvd0[8];    /*        */
	u8 rsvd1[31];   /*        */
	u8 valid;   /*         */
} __packed;

struct amap_it_dmsg_cqe_v2 {
	u8 ack_num[32]; /*         */
	u8 pdu_bytes_rcvd[32];  /*         */
	u8 code[6]; /*         */
	u8 rsvd0[10];   /*         */
	u8 wrb_idx[16]; /*         */
	u8 rsvd1[16];   /*         */
	u8 cid[13]; /*         */
	u8 rsvd2[2];    /*         */
	u8 valid;   /*         */
} __packed;


/* 
                                                            
                      
                                            
 */
#define DB_WRB_POST_CID_MASK		0xFFFF	/*             */
#define DB_DEF_PDU_WRB_INDEX_MASK	0xFF	/*            */

#define DB_DEF_PDU_WRB_INDEX_SHIFT	16
#define DB_DEF_PDU_NUM_POSTED_SHIFT	24

struct fragnum_bits_for_sgl_cra_in {
	struct be_cmd_req_hdr hdr;
	u32 num_bits;
} __packed;

struct iscsi_cleanup_req {
	struct be_cmd_req_hdr hdr;
	u16 chute;
	u8 hdr_ring_id;
	u8 data_ring_id;

} __packed;

struct eq_delay {
	u32 eq_id;
	u32 phase;
	u32 delay_multiplier;
} __packed;

struct be_eq_delay_params_in {
	struct be_cmd_req_hdr hdr;
	u32 num_eq;
	struct eq_delay delay[8];
} __packed;

struct tcp_connect_and_offload_in {
	struct be_cmd_req_hdr hdr;
	struct ip_addr_format ip_address;
	u16 tcp_port;
	u16 cid;
	u16 cq_id;
	u16 defq_id;
	struct phys_addr dataout_template_pa;
	u16 hdr_ring_id;
	u16 data_ring_id;
	u8 do_offload;
	u8 rsvd0[3];
} __packed;

struct tcp_connect_and_offload_out {
	struct be_cmd_resp_hdr hdr;
	u32 connection_handle;
	u16 cid;
	u16 rsvd0;

} __packed;

struct be_mcc_wrb_context {
	struct MCC_WRB *wrb;
	int *users_final_status;
} __packed;

#define DB_DEF_PDU_RING_ID_MASK		0x3FF	/*            */
#define DB_DEF_PDU_CQPROC_MASK		0x3FFF	/*            */
#define DB_DEF_PDU_REARM_SHIFT		14
#define DB_DEF_PDU_EVENT_SHIFT		15
#define DB_DEF_PDU_CQPROC_SHIFT		16

struct dmsg_cqe {
	u32 dw[4];
} __packed;

struct tcp_upload_params_in {
	struct be_cmd_req_hdr hdr;
	u16 id;
	u16 upload_type;
	u32 reset_seq;
} __packed;

struct tcp_upload_params_out {
	u32 dw[32];
} __packed;

union tcp_upload_params {
	struct tcp_upload_params_in request;
	struct tcp_upload_params_out response;
} __packed;

struct be_ulp_fw_cfg {
	u32 ulp_mode;
	u32 etx_base;
	u32 etx_count;
	u32 sq_base;
	u32 sq_count;
	u32 rq_base;
	u32 rq_count;
	u32 dq_base;
	u32 dq_count;
	u32 lro_base;
	u32 lro_count;
	u32 icd_base;
	u32 icd_count;
};

struct be_fw_cfg {
	struct be_cmd_req_hdr hdr;
	u32 be_config_number;
	u32 asic_revision;
	u32 phys_port;
	u32 function_mode;
	struct be_ulp_fw_cfg ulp[2];
	u32 function_caps;
} __packed;

struct be_cmd_get_all_if_id_req {
	struct be_cmd_req_hdr hdr;
	u32 if_count;
	u32 if_hndl_list[1];
} __packed;

#define ISCSI_OPCODE_SCSI_DATA_OUT		5
#define OPCODE_COMMON_NTWK_LINK_STATUS_QUERY 5
#define OPCODE_COMMON_MODIFY_EQ_DELAY		41
#define OPCODE_COMMON_ISCSI_CLEANUP		59
#define	OPCODE_COMMON_TCP_UPLOAD		56
#define OPCODE_COMMON_ISCSI_TCP_CONNECT_AND_OFFLOAD 70
#define OPCODE_COMMON_ISCSI_ERROR_RECOVERY_INVALIDATE_COMMANDS 1
#define OPCODE_ISCSI_INI_CFG_GET_HBA_NAME	6
#define OPCODE_ISCSI_INI_CFG_SET_HBA_NAME	7
#define OPCODE_ISCSI_INI_SESSION_GET_A_SESSION  14
#define OPCODE_ISCSI_INI_DRIVER_REOPEN_ALL_SESSIONS 36
#define OPCODE_ISCSI_INI_DRIVER_OFFLOAD_SESSION 41
#define OPCODE_ISCSI_INI_DRIVER_INVALIDATE_CONNECTION 42
#define OPCODE_ISCSI_INI_BOOT_GET_BOOT_TARGET	52
#define OPCODE_COMMON_WRITE_FLASH		96
#define OPCODE_COMMON_READ_FLASH		97

/*                                              */
#define CMD_ISCSI_COMMAND_INVALIDATE		1
#define CMD_ISCSI_CONNECTION_INVALIDATE		0x8001
#define CMD_ISCSI_CONNECTION_ISSUE_TCP_RST	0x8002

#define INI_WR_CMD			1	/*                         */
#define INI_TMF_CMD			2	/*                       */
#define INI_NOPOUT_CMD			3	/*                           */
#define INI_RD_CMD			5	/*                             
                       
       */
#define TGT_CTX_UPDT_CMD		7	/*                       */
#define TGT_STS_CMD			8	/*                         
                                     
                           
       */
#define TGT_DATAIN_CMD			9	/*                            
                        
       */
#define TGT_SOS_PDU			10	/*                         
                 
       */
#define TGT_DM_CMD			11	/*                       
                    
                                     
       */
/*                        */
#define CMD_CONNECTION_CHUTE_0		1
#define CMD_CONNECTION_CHUTE_1		2
#define CMD_CONNECTION_CHUTE_2		3

#define EQ_MAJOR_CODE_COMPLETION	0

#define CMD_ISCSI_SESSION_DEL_CFG_FROM_FLASH 0
#define CMD_ISCSI_SESSION_SAVE_CFG_ON_FLASH 1

/*                                  */
/*                                                                  */
#define CONNECTION_UPLOAD_GRACEFUL      1	/*                  */
#define CONNECTION_UPLOAD_ABORT_RESET   2	/*                     
              
       */
#define CONNECTION_UPLOAD_ABORT		3	/*                        
              
       */
#define CONNECTION_UPLOAD_ABORT_WITH_SEQ 4	/*                            
                                    */

/*                                                 */
#define BE_NUMBER_OF_FIELD(_type_, _field_)	\
	(FIELD_SIZEOF(_type_, _field_)/sizeof((((_type_ *)0)->_field_[0])))\

/* 
                                                                         
                 
                
 */
#define SOL_CMD_COMPLETE		1	/*                            
                 
       */
#define SOL_CMD_KILLED_DATA_DIGEST_ERR  2	/*                      
                                   
                             
       */
#define CXN_KILLED_PDU_SIZE_EXCEEDS_DSL 3	/*                           
                   
                              
                   
       */
#define CXN_KILLED_BURST_LEN_MISMATCH   4	/*                           
                                   
                            
                 
       */
#define CXN_KILLED_AHS_RCVD		5	/*                           
                                     
                         
             */
#define CXN_KILLED_HDR_DIGEST_ERR	6	/*                           
                                     
              
       */
#define CXN_KILLED_UNKNOWN_HDR		7	/*                           
                    
                                   
                
       */
#define CXN_KILLED_STALE_ITT_TTT_RCVD	8	/*                           
                                     
                                     
                           
       */
#define CXN_KILLED_INVALID_ITT_TTT_RCVD 9	/*                           
                                   
                            
                            
       */
#define CXN_KILLED_RST_RCVD		10	/*                           
                             
                         
       */
#define CXN_KILLED_TIMED_OUT		11	/*                           
                                     
                                  
                        
       */
#define CXN_KILLED_RST_SENT		12	/*                           
                                  
                            
       */
#define CXN_KILLED_FIN_RCVD		13	/*                           
                             
                          
       */
#define CXN_KILLED_BAD_UNSOL_PDU_RCVD	14	/*                           
                              
                                    
                           
                       
       */
#define CXN_KILLED_BAD_WRB_INDEX_ERROR	15	/*                           
                                  
               
       */
#define CXN_KILLED_OVER_RUN_RESIDUAL	16	/*                        
                                   
                             
                        
       */
#define CXN_KILLED_UNDER_RUN_RESIDUAL	17	/*                        
                                   
                                   
                   
       */
#define CMD_KILLED_INVALID_STATSN_RCVD	18	/*                        
                                     
                                    
       */
#define CMD_KILLED_INVALID_R2T_RCVD	19	/*                        
                                     
                                 
                     
       */
#define CMD_CXN_KILLED_LUN_INVALID	20	/*                        
                                   
                                
       */
#define CMD_CXN_KILLED_ICD_INVALID	21	/*                        
                              
                                   
                    
       */
#define CMD_CXN_KILLED_ITT_INVALID	22	/*                            
                                
                      
       */
#define CMD_CXN_KILLED_SEQ_OUTOFORDER	23	/*                            
                                    
                                
       */
#define CMD_CXN_KILLED_INVALID_DATASN_RCVD 24	/*                        
                            
                                    
               
       */
#define CXN_INVALIDATE_NOTIFY		25	/*                        
                           
       */
#define CXN_INVALIDATE_INDEX_NOTIFY	26	/*                        
                   
                             
       */
#define CMD_INVALIDATED_NOTIFY		27	/*                     
                            
       */
#define UNSOL_HDR_NOTIFY		28	/*                           */
#define UNSOL_DATA_NOTIFY		29	/*                         */
#define UNSOL_DATA_DIGEST_ERROR_NOTIFY	30	/*                        
                      
       */
#define DRIVERMSG_NOTIFY		31	/*                      
                      
       */
#define CXN_KILLED_CMND_DATA_NOT_ON_SAME_CONN 32 /*                           
                                   
                                  
                     
        */
#define SOL_CMD_KILLED_DIF_ERR		33	/*                      
                                    
                      
       */
#define CXN_KILLED_SYN_RCVD		34	/*                           
                                   
                
       */
#define CXN_KILLED_IMM_DATA_RCVD	35	/*                           
                             
                                 
                                   
                
       */

int beiscsi_pci_soft_reset(struct beiscsi_hba *phba);
int be_chk_reset_complete(struct beiscsi_hba *phba);

void be_wrb_hdr_prepare(struct be_mcc_wrb *wrb, int payload_len,
			bool embedded, u8 sge_cnt);

void be_cmd_hdr_prepare(struct be_cmd_req_hdr *req_hdr,
			u8 subsystem, u8 opcode, int cmd_len);

#endif /*                 */
