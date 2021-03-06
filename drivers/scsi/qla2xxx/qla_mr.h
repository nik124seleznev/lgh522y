/*
 * QLogic Fibre Channel HBA Driver
 * Copyright (c)  2003-2013 QLogic Corporation
 *
 * See LICENSE.qla2xxx for copyright and licensing details.
 */
#ifndef __QLA_MR_H
#define __QLA_MR_H

/*
                                               
 */
#define PCI_DEVICE_ID_QLOGIC_ISPF001		0xF001

/*                           */

#define FX00_COMMAND_TYPE_7	0x07	/*                               */
struct cmd_type_7_fx00 {
	uint8_t entry_type;		/*             */
	uint8_t entry_count;		/*              */
	uint8_t sys_define;		/*                 */
	uint8_t entry_status;		/*               */

	uint32_t handle;		/*                */
	uint32_t handle_hi;

	uint16_t tgt_idx;		/*             */
	uint16_t timeout;		/*                  */

	uint16_t dseg_count;		/*                     */
	uint16_t scsi_rsp_dsd_len;

	struct scsi_lun lun;		/*           */

	uint8_t cntrl_flags;

	uint8_t task_mgmt_flags;	/*                        */

	uint8_t task;

	uint8_t crn;

	uint8_t fcp_cdb[MAX_CMDSZ];	/*                     */
	uint32_t byte_count;		/*                   */

	uint32_t dseg_0_address[2];	/*                         */
	uint32_t dseg_0_len;		/*                        */
};

/*
                                                 
 */
struct mrk_entry_fx00 {
	uint8_t entry_type;		/*             */
	uint8_t entry_count;		/*              */
	uint8_t handle_count;		/*               */
	uint8_t entry_status;		/*               */

	uint32_t handle;		/*                */
	uint32_t handle_hi;		/*                */

	uint16_t tgt_id;		/*            */

	uint8_t modifier;		/*                 */
	uint8_t reserved_1;

	uint8_t reserved_2[5];

	uint8_t lun[8];			/*               */
	uint8_t reserved_3[36];
};


#define	STATUS_TYPE_FX00	0x01		/*               */
struct sts_entry_fx00 {
	uint8_t entry_type;		/*             */
	uint8_t entry_count;		/*              */
	uint8_t sys_define;		/*                 */
	uint8_t entry_status;		/*               */

	uint32_t handle;		/*                */
	uint32_t handle_hi;		/*                */

	uint16_t comp_status;		/*                    */
	uint16_t reserved_0;		/*                             */

	uint32_t residual_len;		/*                                   */

	uint16_t reserved_1;
	uint16_t state_flags;		/*              */

	uint16_t reserved_2;
	uint16_t scsi_status;		/*              */

	uint32_t sense_len;		/*                   */
	uint8_t data[32];		/*                                 */
};


#define MAX_HANDLE_COUNT	15
#define MULTI_STATUS_TYPE_FX00	0x0D

struct multi_sts_entry_fx00 {
	uint8_t entry_type;		/*             */
	uint8_t sys_define;		/*                 */
	uint8_t handle_count;
	uint8_t entry_status;

	uint32_t handles[MAX_HANDLE_COUNT];
};

#define TSK_MGMT_IOCB_TYPE_FX00		0x05
struct tsk_mgmt_entry_fx00 {
	uint8_t entry_type;		/*             */
	uint8_t entry_count;		/*              */
	uint8_t sys_define;
	uint8_t entry_status;		/*               */

	uint32_t handle;		/*                */

	uint32_t handle_hi;		/*                */

	uint16_t tgt_id;		/*             */

	uint16_t reserved_1;

	uint16_t delay;			/*                            */

	uint16_t timeout;		/*                  */

	struct scsi_lun lun;		/*           */

	uint32_t control_flags;		/*                */

	uint8_t reserved_2[32];
};


#define	ABORT_IOCB_TYPE_FX00	0x08		/*                    */
struct abort_iocb_entry_fx00 {
	uint8_t entry_type;		/*             */
	uint8_t entry_count;		/*              */
	uint8_t sys_define;		/*                 */
	uint8_t entry_status;		/*               */

	uint32_t handle;		/*                */
	uint32_t handle_hi;		/*                */

	uint16_t tgt_id_sts;		/*                    */
	uint16_t options;

	uint32_t abort_handle;		/*                */
	uint32_t abort_handle_hi;	/*                */

	uint16_t req_que_no;
	uint8_t reserved_1[38];
};

#define IOCTL_IOSB_TYPE_FX00	0x0C
struct ioctl_iocb_entry_fx00 {
	uint8_t entry_type;		/*             */
	uint8_t entry_count;		/*              */
	uint8_t sys_define;		/*                 */
	uint8_t entry_status;		/*               */

	uint32_t handle;		/*                */
	uint32_t reserved_0;		/*                */

	uint16_t comp_func_num;
	uint16_t fw_iotcl_flags;

	uint32_t dataword_r;		/*                    */
	uint32_t adapid;		/*            */
	uint32_t adapid_hi;		/*                 */
	uint32_t reserved_1;

	uint32_t seq_no;
	uint8_t reserved_2[20];
	uint32_t residuallen;
	uint32_t status;
};

#define STATUS_CONT_TYPE_FX00 0x04

#define FX00_IOCB_TYPE		0x0B
struct fxdisc_entry_fx00 {
	uint8_t entry_type;		/*             */
	uint8_t entry_count;		/*              */
	uint8_t sys_define;		/*                 */
	uint8_t entry_status;		/*               */

	uint32_t handle;		/*                */
	uint32_t reserved_0;		/*                */

	uint16_t func_num;
	uint16_t req_xfrcnt;
	uint16_t req_dsdcnt;
	uint16_t rsp_xfrcnt;
	uint16_t rsp_dsdcnt;
	uint8_t flags;
	uint8_t reserved_1;

	uint32_t dseg_rq_address[2];	/*                         */
	uint32_t dseg_rq_len;		/*                        */
	uint32_t dseg_rsp_address[2];	/*                         */
	uint32_t dseg_rsp_len;		/*                        */

	uint32_t dataword;
	uint32_t adapid;
	uint32_t adapid_hi;
	uint32_t dataword_extra;
};

struct qlafx00_tgt_node_info {
	uint8_t tgt_node_wwpn[WWN_SIZE];
	uint8_t tgt_node_wwnn[WWN_SIZE];
	uint32_t tgt_node_state;
	uint8_t reserved[128];
	uint32_t reserved_1[8];
	uint64_t reserved_2[4];
} __packed;

#define QLAFX00_TGT_NODE_INFO sizeof(struct qlafx00_tgt_node_info)

#define QLAFX00_LINK_STATUS_DOWN	0x10
#define QLAFX00_LINK_STATUS_UP		0x11

#define QLAFX00_PORT_SPEED_2G	0x2
#define QLAFX00_PORT_SPEED_4G	0x4
#define QLAFX00_PORT_SPEED_8G	0x8
#define QLAFX00_PORT_SPEED_10G	0xa
struct port_info_data {
	uint8_t         port_state;
	uint8_t         port_type;
	uint16_t        port_identifier;
	uint32_t        up_port_state;
	uint8_t         fw_ver_num[32];
	uint8_t         portal_attrib;
	uint16_t        host_option;
	uint8_t         reset_delay;
	uint8_t         pdwn_retry_cnt;
	uint16_t        max_luns2tgt;
	uint8_t         risc_ver;
	uint8_t         pconn_option;
	uint16_t        risc_option;
	uint16_t        max_frame_len;
	uint16_t        max_iocb_alloc;
	uint16_t        exec_throttle;
	uint8_t         retry_cnt;
	uint8_t         retry_delay;
	uint8_t         port_name[8];
	uint8_t         port_id[3];
	uint8_t         link_status;
	uint8_t         plink_rate;
	uint32_t        link_config;
	uint16_t        adap_haddr;
	uint8_t         tgt_disc;
	uint8_t         log_tout;
	uint8_t         node_name[8];
	uint16_t        erisc_opt1;
	uint8_t         resp_acc_tmr;
	uint8_t         intr_del_tmr;
	uint8_t         erisc_opt2;
	uint8_t         alt_port_name[8];
	uint8_t         alt_node_name[8];
	uint8_t         link_down_tout;
	uint8_t         conn_type;
	uint8_t         fc_fw_mode;
	uint32_t        uiReserved[48];
} __packed;

/*                      */
#define OS_TYPE_UNKNOWN             0
#define OS_TYPE_LINUX               2

/*            */
#define SYSNAME_LENGTH              128
#define NODENAME_LENGTH             64
#define RELEASE_LENGTH              64
#define VERSION_LENGTH              64
#define MACHINE_LENGTH              64
#define DOMNAME_LENGTH              64

struct host_system_info {
	uint32_t os_type;
	char    sysname[SYSNAME_LENGTH];
	char    nodename[NODENAME_LENGTH];
	char    release[RELEASE_LENGTH];
	char    version[VERSION_LENGTH];
	char    machine[MACHINE_LENGTH];
	char    domainname[DOMNAME_LENGTH];
	char    hostdriver[VERSION_LENGTH];
	uint32_t reserved[64];
} __packed;

struct register_host_info {
	struct host_system_info     hsi;	/*                  */
	uint64_t        utc;			/*                   */
	uint32_t        reserved[64];		/*                  */
} __packed;


#define QLAFX00_PORT_DATA_INFO (sizeof(struct port_info_data))
#define QLAFX00_TGT_NODE_LIST_SIZE (sizeof(uint32_t) * 32)

struct config_info_data {
	uint8_t		product_name[256];
	uint8_t		symbolic_name[64];
	uint8_t		serial_num[32];
	uint8_t		hw_version[16];
	uint8_t		fw_version[16];
	uint8_t		uboot_version[16];
	uint8_t		fru_serial_num[32];

	uint8_t		fc_port_count;
	uint8_t		iscsi_port_count;
	uint8_t		reserved1[2];

	uint8_t		mode;
	uint8_t		log_level;
	uint8_t		reserved2[2];

	uint32_t	log_size;

	uint8_t		tgt_pres_mode;
	uint8_t		iqn_flags;
	uint8_t		lun_mapping;

	uint64_t	adapter_id;

	uint32_t	cluster_key_len;
	uint8_t		cluster_key[10];

	uint64_t	cluster_master_id;
	uint64_t	cluster_slave_id;
	uint8_t		cluster_flags;
} __packed;

#define FXDISC_GET_CONFIG_INFO		0x01
#define FXDISC_GET_PORT_INFO		0x02
#define FXDISC_GET_TGT_NODE_INFO	0x80
#define FXDISC_GET_TGT_NODE_LIST	0x81
#define FXDISC_REG_HOST_INFO		0x99

#define QLAFX00_HBA_ICNTRL_REG		0x21B08
#define QLAFX00_ICR_ENB_MASK            0x80000000
#define QLAFX00_ICR_DIS_MASK            0x7fffffff
#define QLAFX00_HST_RST_REG		0x18264
#define QLAFX00_HST_TO_HBA_REG		0x20A04
#define QLAFX00_HBA_TO_HOST_REG		0x21B70
#define QLAFX00_HST_INT_STS_BITS	0x7
#define QLAFX00_BAR1_BASE_ADDR_REG	0x40018
#define QLAFX00_PEX0_WIN0_BASE_ADDR_REG	0x41824

#define QLAFX00_INTR_MB_CMPLT		0x1
#define QLAFX00_INTR_RSP_CMPLT		0x2
#define QLAFX00_INTR_MB_RSP_CMPLT	0x3
#define QLAFX00_INTR_ASYNC_CMPLT	0x4
#define QLAFX00_INTR_MB_ASYNC_CMPLT	0x5
#define QLAFX00_INTR_RSP_ASYNC_CMPLT	0x6
#define QLAFX00_INTR_ALL_CMPLT		0x7

#define QLAFX00_MBA_SYSTEM_ERR		0x8002
#define QLAFX00_MBA_LINK_UP		0x8011
#define QLAFX00_MBA_LINK_DOWN		0x8012
#define QLAFX00_MBA_PORT_UPDATE		0x8014
#define QLAFX00_MBA_SHUTDOWN_RQSTD	0x8062

#define SOC_SW_RST_CONTROL_REG_CORE0     0x0020800
#define SOC_FABRIC_RST_CONTROL_REG       0x0020840
#define SOC_FABRIC_CONTROL_REG           0x0020200
#define SOC_FABRIC_CONFIG_REG            0x0020204

#define SOC_INTERRUPT_SOURCE_I_CONTROL_REG     0x0020B00
#define SOC_CORE_TIMER_REG                     0x0021850
#define SOC_IRQ_ACK_REG                        0x00218b4

#define CONTINUE_A64_TYPE_FX00	0x03	/*                     */

#define QLAFX00_SET_HST_INTR(ha, value) \
	WRT_REG_DWORD((ha)->cregbase + QLAFX00_HST_TO_HBA_REG, \
	value)

#define QLAFX00_CLR_HST_INTR(ha, value) \
	WRT_REG_DWORD((ha)->cregbase + QLAFX00_HBA_TO_HOST_REG, \
	~value)

#define QLAFX00_RD_INTR_REG(ha) \
	RD_REG_DWORD((ha)->cregbase + QLAFX00_HBA_TO_HOST_REG)

#define QLAFX00_CLR_INTR_REG(ha, value) \
	WRT_REG_DWORD((ha)->cregbase + QLAFX00_HBA_TO_HOST_REG, \
	~value)

#define QLAFX00_SET_HBA_SOC_REG(ha, off, val)\
	WRT_REG_DWORD((ha)->cregbase + off, val)

#define QLAFX00_GET_HBA_SOC_REG(ha, off)\
	RD_REG_DWORD((ha)->cregbase + off)

#define QLAFX00_HBA_RST_REG(ha, val)\
	WRT_REG_DWORD((ha)->cregbase + QLAFX00_HST_RST_REG, val)

#define QLAFX00_RD_ICNTRL_REG(ha) \
	RD_REG_DWORD((ha)->cregbase + QLAFX00_HBA_ICNTRL_REG)

#define QLAFX00_ENABLE_ICNTRL_REG(ha) \
	WRT_REG_DWORD((ha)->cregbase + QLAFX00_HBA_ICNTRL_REG, \
	(QLAFX00_GET_HBA_SOC_REG(ha, QLAFX00_HBA_ICNTRL_REG) | \
	 QLAFX00_ICR_ENB_MASK))

#define QLAFX00_DISABLE_ICNTRL_REG(ha) \
	WRT_REG_DWORD((ha)->cregbase + QLAFX00_HBA_ICNTRL_REG, \
	(QLAFX00_GET_HBA_SOC_REG(ha, QLAFX00_HBA_ICNTRL_REG) & \
	 QLAFX00_ICR_DIS_MASK))

#define QLAFX00_RD_REG(ha, off) \
	RD_REG_DWORD((ha)->cregbase + off)

#define QLAFX00_WR_REG(ha, off, val) \
	WRT_REG_DWORD((ha)->cregbase + off, val)

struct qla_mt_iocb_rqst_fx00 {
	uint32_t reserved_0;

	uint16_t func_type;
	uint8_t flags;
	uint8_t reserved_1;

	uint32_t dataword;

	uint32_t adapid;
	uint32_t adapid_hi;

	uint32_t dataword_extra;

	uint32_t req_len;

	uint32_t rsp_len;
};

struct qla_mt_iocb_rsp_fx00 {
	uint32_t reserved_1;

	uint16_t func_type;
	uint16_t ioctl_flags;

	uint32_t ioctl_data;

	uint32_t adapid;
	uint32_t adapid_hi;

	uint32_t reserved_2;
	uint32_t seq_number;

	uint8_t reserved_3[20];

	int32_t res_count;

	uint32_t status;
};


#define MAILBOX_REGISTER_COUNT_FX00	16
#define AEN_MAILBOX_REGISTER_COUNT_FX00	8
#define MAX_FIBRE_DEVICES_FX00	512
#define MAX_LUNS_FX00		0x1024
#define MAX_TARGETS_FX00	MAX_ISA_DEVICES
#define REQUEST_ENTRY_CNT_FX00		512	/*                            */
#define RESPONSE_ENTRY_CNT_FX00		256	/*                            */

/*
                                            
 */
#define FSTATE_FX00_CONFIG_WAIT     0x0000	/*                            
                               
       */
#define FSTATE_FX00_INITIALIZED     0x1000	/*                           
                   
       */

#define FX00_DEF_RATOV	10

struct mr_data_fx00 {
	uint8_t	product_name[256];
	uint8_t	symbolic_name[64];
	uint8_t	serial_num[32];
	uint8_t	hw_version[16];
	uint8_t	fw_version[16];
	uint8_t	uboot_version[16];
	uint8_t	fru_serial_num[32];
	fc_port_t       fcport;		/*                         
                           
                              
      */
	uint8_t fw_hbt_en;
	uint8_t fw_hbt_cnt;
	uint8_t fw_hbt_miss_cnt;
	uint32_t old_fw_hbt_cnt;
	uint16_t fw_reset_timer_tick;
	uint8_t fw_reset_timer_exp;
	uint32_t old_aenmbx0_state;
};

#define QLAFX00_LOOP_DOWN_TIME		615     /*     */
#define QLAFX00_HEARTBEAT_INTERVAL	6	/*                   */
#define QLAFX00_HEARTBEAT_MISS_CNT	3	/*                */
#define QLAFX00_RESET_INTERVAL		120	/*                   */
#define QLAFX00_MAX_RESET_INTERVAL	600	/*                   */
#endif
