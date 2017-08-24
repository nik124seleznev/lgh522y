/* bnx2x_vfpf.h: Broadcom Everest network driver.
 *
 * Copyright (c) 2011-2013 Broadcom Corporation
 *
 * Unless you and Broadcom execute a separate written software license
 * agreement governing use of this software, this software is licensed to you
 * under the terms of the GNU General Public License version 2, available
 * at http://www.gnu.org/licenses/old-licenses/gpl-2.0.html (the "GPL").
 *
 * Notwithstanding the above, under no circumstances may you combine this
 * software in any way with any other Broadcom software provided under a
 * license other than the GPL, without Broadcom's express prior written
 * consent.
 *
 * Maintained by: Eilon Greenstein <eilong@broadcom.com>
 * Written by: Ariel Elior <ariele@broadcom.com>
 */
#ifndef VF_PF_IF_H
#define VF_PF_IF_H

#ifdef CONFIG_BNX2X_SRIOV

/*                                */
struct vf_pf_resc_request {
	u8  num_rxqs;
	u8  num_txqs;
	u8  num_sbs;
	u8  num_mac_filters;
	u8  num_vlan_filters;
	u8  num_mc_filters; /*                          */
};

struct hw_sb_info {
	u8 hw_sb_id;	/*                                         */
	u8 sb_qid;	/*                           */
};

/*                             
                      
 */
#define TLV_BUFFER_SIZE			1024
#define PF_VF_BULLETIN_SIZE		512

#define VFPF_QUEUE_FLG_TPA		0x0001
#define VFPF_QUEUE_FLG_TPA_IPV6		0x0002
#define VFPF_QUEUE_FLG_TPA_GRO		0x0004
#define VFPF_QUEUE_FLG_CACHE_ALIGN	0x0008
#define VFPF_QUEUE_FLG_STATS		0x0010
#define VFPF_QUEUE_FLG_OV		0x0020
#define VFPF_QUEUE_FLG_VLAN		0x0040
#define VFPF_QUEUE_FLG_COS		0x0080
#define VFPF_QUEUE_FLG_HC		0x0100
#define VFPF_QUEUE_FLG_DHC		0x0200

#define VFPF_QUEUE_DROP_IP_CS_ERR	(1 << 0)
#define VFPF_QUEUE_DROP_TCP_CS_ERR	(1 << 1)
#define VFPF_QUEUE_DROP_TTL0		(1 << 2)
#define VFPF_QUEUE_DROP_UDP_CS_ERR	(1 << 3)

#define VFPF_RX_MASK_ACCEPT_NONE		0x00000000
#define VFPF_RX_MASK_ACCEPT_MATCHED_UNICAST	0x00000001
#define VFPF_RX_MASK_ACCEPT_MATCHED_MULTICAST	0x00000002
#define VFPF_RX_MASK_ACCEPT_ALL_UNICAST		0x00000004
#define VFPF_RX_MASK_ACCEPT_ALL_MULTICAST	0x00000008
#define VFPF_RX_MASK_ACCEPT_BROADCAST		0x00000010
#define BULLETIN_CONTENT_SIZE		(sizeof(struct pf_vf_bulletin_content))
#define BULLETIN_ATTEMPTS	5 /*                                    */
#define BULLETIN_CRC_SEED	0

enum {
	PFVF_STATUS_WAITING = 0,
	PFVF_STATUS_SUCCESS,
	PFVF_STATUS_FAILURE,
	PFVF_STATUS_NOT_SUPPORTED,
	PFVF_STATUS_NO_RESOURCE
};

/*                    */
/*                                                                       */
struct channel_tlv {
	u16 type;
	u16 length;
};

/*                                                                         
                 
 */
struct vfpf_first_tlv {
	struct channel_tlv tl;
	u32 resp_msg_offset;
};

/*                                                                   */
struct pfvf_tlv {
	struct channel_tlv tl;
	u8 status;
	u8 padding[3];
};

/*                                 */
struct pfvf_general_resp_tlv {
	struct pfvf_tlv hdr;
};

/*                                      */
struct channel_list_end_tlv {
	struct channel_tlv tl;
	u8 padding[4];
};

/*         */
struct vfpf_acquire_tlv {
	struct vfpf_first_tlv first_tlv;

	struct vf_pf_vfdev_info {
		/*                                             */
		u8  vf_id;		/*                   */
		u8  vf_os;		/*                  */
		u8 padding[2];
	} vfdev_info;

	struct vf_pf_resc_request resc_request;

	aligned_u64 bulletin_addr;
};

/*                                   */
struct vfpf_q_op_tlv {
	struct vfpf_first_tlv	first_tlv;
	u8 vf_qid;
	u8 padding[3];
};

/*                                                        */
struct pfvf_acquire_resp_tlv {
	struct pfvf_tlv hdr;
	struct pf_vf_pfdev_info {
		u32 chip_num;
		u32 pf_cap;
#define PFVF_CAP_RSS		0x00000001
#define PFVF_CAP_DHC		0x00000002
#define PFVF_CAP_TPA		0x00000004
		char fw_ver[32];
		u16 db_size;
		u8  indices_per_sb;
		u8  padding;
	} pfdev_info;
	struct pf_vf_resc {
		/*                                                           
                                                            
                    
   */
#define PFVF_MAX_QUEUES_PER_VF         16
#define PFVF_MAX_SBS_PER_VF            16
		struct hw_sb_info hw_sbs[PFVF_MAX_SBS_PER_VF];
		u8	hw_qid[PFVF_MAX_QUEUES_PER_VF];
		u8	num_rxqs;
		u8	num_txqs;
		u8	num_sbs;
		u8	num_mac_filters;
		u8	num_vlan_filters;
		u8	num_mc_filters;
		u8	permanent_mac_addr[ETH_ALEN];
		u8	current_mac_addr[ETH_ALEN];
		u8	padding[2];
	} resc;
};

/*         */
struct vfpf_init_tlv {
	struct vfpf_first_tlv first_tlv;
	aligned_u64 sb_addr[PFVF_MAX_SBS_PER_VF]; /*             */
	aligned_u64 spq_addr;
	aligned_u64 stats_addr;
};

/*             */
struct vfpf_setup_q_tlv {
	struct vfpf_first_tlv first_tlv;

	struct vf_pf_rxq_params {
		/*                    */
		aligned_u64 rcq_addr;
		aligned_u64 rcq_np_addr;
		aligned_u64 rxq_addr;
		aligned_u64 sge_addr;

		/*              */
		u8  vf_sb;		/*                   */
		u8  sb_index;		/*                 */
		u16 hc_rate;		/*                             */
					/*                             */
		/*                */
		u16 mtu;
		u16 buf_sz;
		u16 flags;		/*                        */
		u16 stat_id;		/*                                */

		/*                              */
		u16 sge_buf_sz;
		u16 tpa_agg_sz;
		u8 max_sge_pkt;

		u8 drop_flags;		/*                                 
                                    
      */

		u8 cache_line_log;	/*                            */
		u8 padding;
	} rxq;

	struct vf_pf_txq_params {
		/*                    */
		aligned_u64 txq_addr;

		/*              */
		u8  vf_sb;		/*                   */
		u8  sb_index;		/*                 */
		u16 hc_rate;		/*                             */
					/*                             */
		u32 flags;		/*                        */
		u16 stat_id;		/*                                */
		u8  traffic_type;	/*                        */
		u8  padding;
	} txq;

	u8 vf_qid;			/*                   */
	u8 param_valid;
#define VFPF_RXQ_VALID		0x01
#define VFPF_TXQ_VALID		0x02
	u8 padding[2];
};

/*                   */
struct vfpf_q_mac_vlan_filter {
	u32 flags;
#define VFPF_Q_FILTER_DEST_MAC_VALID	0x01
#define VFPF_Q_FILTER_VLAN_TAG_VALID	0x02
#define VFPF_Q_FILTER_SET_MAC		0x100	/*           */
	u8  mac[ETH_ALEN];
	u16 vlan_tag;
};

/*                         */
struct vfpf_set_q_filters_tlv {
	struct vfpf_first_tlv first_tlv;

	u32 flags;
#define VFPF_SET_Q_FILTERS_MAC_VLAN_CHANGED	0x01
#define VFPF_SET_Q_FILTERS_MULTICAST_CHANGED	0x02
#define VFPF_SET_Q_FILTERS_RX_MASK_CHANGED	0x04

	u8 vf_qid;			/*                   */
	u8 n_mac_vlan_filters;
	u8 n_multicast;
	u8 padding;

#define PFVF_MAX_MAC_FILTERS                   16
#define PFVF_MAX_VLAN_FILTERS                  16
#define PFVF_MAX_FILTERS               (PFVF_MAX_MAC_FILTERS +\
					 PFVF_MAX_VLAN_FILTERS)
	struct vfpf_q_mac_vlan_filter filters[PFVF_MAX_FILTERS];

#define PFVF_MAX_MULTICAST_PER_VF              32
	u8  multicast[PFVF_MAX_MULTICAST_PER_VF][ETH_ALEN];

	u32 rx_mask;	/*                                           */
};

/*                       */
struct vfpf_close_tlv {
	struct vfpf_first_tlv   first_tlv;
	u16			vf_id;  /*           */
	u8 padding[2];
};

/*                                     */
struct vfpf_release_tlv {
	struct vfpf_first_tlv	first_tlv;
	u16			vf_id;
	u8 padding[2];
};

struct tlv_buffer_size {
	u8 tlv_buffer[TLV_BUFFER_SIZE];
};

union vfpf_tlvs {
	struct vfpf_first_tlv		first_tlv;
	struct vfpf_acquire_tlv		acquire;
	struct vfpf_init_tlv		init;
	struct vfpf_close_tlv		close;
	struct vfpf_q_op_tlv		q_op;
	struct vfpf_setup_q_tlv		setup_q;
	struct vfpf_set_q_filters_tlv	set_q_filters;
	struct vfpf_release_tlv         release;
	struct channel_list_end_tlv     list_end;
	struct tlv_buffer_size		tlv_buf_size;
};

union pfvf_tlvs {
	struct pfvf_general_resp_tlv    general_resp;
	struct pfvf_acquire_resp_tlv	acquire_resp;
	struct channel_list_end_tlv	list_end;
	struct tlv_buffer_size		tlv_buf_size;
};

/*                                                                          
                                                                     
                                                                            
                                                                           
 */
struct pf_vf_bulletin_size {
	u8 size[PF_VF_BULLETIN_SIZE];
};

struct pf_vf_bulletin_content {
	u32 crc;			/*                                     
                  
      */
	u16 version;
	u16 length;

	aligned_u64 valid_bitmap;	/*                               
                         
      */

#define MAC_ADDR_VALID		0	/*                                    
                           
      */
#define VLAN_VALID		1	/*                                   
                        
      */

	u8 mac[ETH_ALEN];
	u8 mac_padding[2];

	u16 vlan;
	u8 vlan_padding[6];
};

union pf_vf_bulletin {
	struct pf_vf_bulletin_content content;
	struct pf_vf_bulletin_size size;
};

#define MAX_TLVS_IN_LIST 50

enum channel_tlvs {
	CHANNEL_TLV_NONE,
	CHANNEL_TLV_ACQUIRE,
	CHANNEL_TLV_INIT,
	CHANNEL_TLV_SETUP_Q,
	CHANNEL_TLV_SET_Q_FILTERS,
	CHANNEL_TLV_TEARDOWN_Q,
	CHANNEL_TLV_CLOSE,
	CHANNEL_TLV_RELEASE,
	CHANNEL_TLV_PF_RELEASE_VF,
	CHANNEL_TLV_LIST_END,
	CHANNEL_TLV_FLR,
	CHANNEL_TLV_PF_SET_MAC,
	CHANNEL_TLV_PF_SET_VLAN,
	CHANNEL_TLV_MAX
};

#endif /*                    */
#endif /*            */
