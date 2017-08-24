/* bnx2x_mfw_req.h: Broadcom Everest network driver.
 *
 * Copyright (c) 2012-2013 Broadcom Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation.
 */

#ifndef BNX2X_MFW_REQ_H
#define BNX2X_MFW_REQ_H

#define PORT_0			0
#define PORT_1			1
#define PORT_MAX		2
#define NVM_PATH_MAX		2

/*                                            */
struct fcoe_capabilities {
	u32 capability1;
	/*                                       */
	#define FCOE_IOS_PER_CONNECTION_MASK    0x0000ffff
	#define FCOE_IOS_PER_CONNECTION_SHIFT   0
	/*                                   */
	#define FCOE_LOGINS_PER_PORT_MASK       0xffff0000
	#define FCOE_LOGINS_PER_PORT_SHIFT   16

	u32 capability2;
	/*                             */
	#define FCOE_NUMBER_OF_EXCHANGES_MASK   0x0000ffff
	#define FCOE_NUMBER_OF_EXCHANGES_SHIFT  0
	/*                           */
	#define FCOE_NPIV_WWN_PER_PORT_MASK     0xffff0000
	#define FCOE_NPIV_WWN_PER_PORT_SHIFT    16

	u32 capability3;
	/*                                     */
	#define FCOE_TARGETS_SUPPORTED_MASK     0x0000ffff
	#define FCOE_TARGETS_SUPPORTED_SHIFT    0
	/*                                                               */
	#define FCOE_OUTSTANDING_COMMANDS_MASK  0xffff0000
	#define FCOE_OUTSTANDING_COMMANDS_SHIFT 16

	u32 capability4;
	#define FCOE_CAPABILITY4_STATEFUL			0x00000001
	#define FCOE_CAPABILITY4_STATELESS			0x00000002
	#define FCOE_CAPABILITY4_CAPABILITIES_REPORTED_VALID	0x00000004
};

struct glob_ncsi_oem_data {
	u32 driver_version;
	u32 unused[3];
	struct fcoe_capabilities fcoe_features[NVM_PATH_MAX][PORT_MAX];
};

/*                          */
#define DRV_INFO_CUR_VER 2

/*                             */
enum drv_info_opcode {
	ETH_STATS_OPCODE,
	FCOE_STATS_OPCODE,
	ISCSI_STATS_OPCODE
};

#define ETH_STAT_INFO_VERSION_LEN	12
/*                                                                */
struct eth_stats_info {
	/*                                         */
	u8 version[ETH_STAT_INFO_VERSION_LEN];
	/*                                                             */
	u8 mac_local[8];
	u8 mac_add1[8];		/*                                   */
	u8 mac_add2[8];		/*                                   */
	u32 mtu_size;		/*                                   */
	u32 feature_flags;	/*                */
#define FEATURE_ETH_CHKSUM_OFFLOAD_MASK		0x01
#define FEATURE_ETH_LSO_MASK			0x02
#define FEATURE_ETH_BOOTMODE_MASK		0x1C
#define FEATURE_ETH_BOOTMODE_SHIFT		2
#define FEATURE_ETH_BOOTMODE_NONE		(0x0 << 2)
#define FEATURE_ETH_BOOTMODE_PXE		(0x1 << 2)
#define FEATURE_ETH_BOOTMODE_ISCSI		(0x2 << 2)
#define FEATURE_ETH_BOOTMODE_FCOE		(0x3 << 2)
#define FEATURE_ETH_TOE_MASK			0x20
	u32 lso_max_size;	/*                     */
	u32 lso_min_seg_cnt;	/*                      */
	/*                                     */
	u32 ipv4_ofld_cnt;
	/*                                     */
	u32 ipv6_ofld_cnt;
	u32 promiscuous_mode;	/*                                 */
	u32 txq_size;		/*                           */
	u32 rxq_size;		/*                           */
	/*                                                                  */
	u32 txq_avg_depth;
	/*                                                                   */
	u32 rxq_avg_depth;
	/*                                                 */
	u32 iov_offload;
	/*                                  */
	u32 netq_cnt;
	u32 vf_cnt;		/*                             */
};

/*                                                            */
struct fcoe_stats_info {
	u8 version[12];		/*                            */
	u8 mac_local[8];	/*                     */
	u8 mac_add1[8];		/*                                   */
	u8 mac_add2[8];		/*                                   */
	/*                                   */
	u32 qos_priority;
	u32 txq_size;		/*                                 */
	u32 rxq_size;		/*                                 */
	/*                                     */
	u32 txq_avg_depth;
	/*                                      */
	u32 rxq_avg_depth;
	u32 rx_frames_lo;	/*                          */
	u32 rx_frames_hi;	/*                          */
	u32 rx_bytes_lo;	/*                         */
	u32 rx_bytes_hi;	/*                         */
	u32 tx_frames_lo;	/*                      */
	u32 tx_frames_hi;	/*                      */
	u32 tx_bytes_lo;	/*                     */
	u32 tx_bytes_hi;	/*                     */
};

/*                                                            */
struct iscsi_stats_info {
	u8 version[12];		/*                            */
	u8 mac_local[8];	/*                               */
	u8 mac_add1[8];		/*                                   */
	/*                                   */
	u32 qos_priority;
	u8 initiator_name[64];	/*                                 */
	u8 ww_port_name[64];	/*                            */
	u8 boot_target_name[64];/*                         */
	u8 boot_target_ip[16];	/*                       */
	u32 boot_target_portal;	/*                           */
	u8 boot_init_ip[16];	/*                                  */
	u32 max_frame_size;	/*                       */
	u32 txq_size;		/*                                */
	u32 rxq_size;		/*                                */
	u32 txq_avg_depth;	/*                                    */
	u32 rxq_avg_depth;	/*                                     */
	u32 rx_pdus_lo;		/*                      */
	u32 rx_pdus_hi;		/*                      */
	u32 rx_bytes_lo;	/*                          */
	u32 rx_bytes_hi;	/*                          */
	u32 tx_pdus_lo;		/*                  */
	u32 tx_pdus_hi;		/*                  */
	u32 tx_bytes_lo;	/*                          */
	u32 tx_bytes_hi;	/*                          */
	u32 pcp_prior_map_tbl;	/*                                  
                                             
                                            
                                   
     */
};

union drv_info_to_mcp {
	struct eth_stats_info	ether_stat;
	struct fcoe_stats_info	fcoe_stat;
	struct iscsi_stats_info	iscsi_stat;
};
#endif /*                 */
