/*
 * PMC-Sierra SPCv/ve 8088/8089 SAS/SATA based host adapters driver
 *
 * Copyright (c) 2008-2009 USI Co., Ltd.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *	notice, this list of conditions, and the following disclaimer,
 *	without modification.
 * 2. Redistributions in binary form must reproduce at minimum a disclaimer
 *	substantially similar to the "NO WARRANTY" disclaimer below
 *	("Disclaimer") and any redistribution must be conditioned upon
 *	including a substantially similar Disclaimer requirement for further
 *	binary redistribution.
 * 3. Neither the names of the above-listed copyright holders nor the names
 *	of any contributors may be used to endorse or promote products derived
 *	from this software without specific prior written permission.
 *
 * Alternatively, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") version 2 as published by the Free
 * Software Foundation.
 *
 * NO WARRANTY
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDERS OR CONTRIBUTORS BE LIABLE FOR SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES.
 *
 */

#ifndef _PMC8001_REG_H_
#define _PMC8001_REG_H_

#include <linux/types.h>
#include <scsi/libsas.h>

/*                            */
#define OPC_INB_ECHO				1	/*       */
#define OPC_INB_PHYSTART			4	/*       */
#define OPC_INB_PHYSTOP				5	/*       */
#define OPC_INB_SSPINIIOSTART			6	/*       */
#define OPC_INB_SSPINITMSTART			7	/*       */
/*                  */
#define OPC_INB_RSVD				8	/*       */
#define OPC_INB_DEV_HANDLE_ACCEPT		9	/*       */
#define OPC_INB_SSPTGTIOSTART			10	/*       */
#define OPC_INB_SSPTGTRSPSTART			11	/*       */
/*                               */
#define OPC_INB_SSP_ABORT			15	/*       */
#define OPC_INB_DEREG_DEV_HANDLE		16	/*       */
#define OPC_INB_GET_DEV_HANDLE			17	/*       */
#define OPC_INB_SMP_REQUEST			18	/*       */
/*                                      */
#define OPC_INB_SMP_ABORT			20	/*       */
/*                   */
#define OPC_INB_RSVD1				22	/*       */
#define OPC_INB_SATA_HOST_OPSTART		23	/*       */
#define OPC_INB_SATA_ABORT			24	/*       */
#define OPC_INB_LOCAL_PHY_CONTROL		25	/*       */
/*                   */
#define OPC_INB_RSVD2				26	/*       */
#define OPC_INB_FW_FLASH_UPDATE			32	/*       */
#define OPC_INB_GPIO				34	/*       */
#define OPC_INB_SAS_DIAG_MODE_START_END		35	/*       */
#define OPC_INB_SAS_DIAG_EXECUTE		36	/*       */
/*                   */
#define OPC_INB_RSVD3				37	/*       */
#define OPC_INB_GET_TIME_STAMP			38	/*       */
#define OPC_INB_PORT_CONTROL			39	/*       */
#define OPC_INB_GET_NVMD_DATA			40	/*       */
#define OPC_INB_SET_NVMD_DATA			41	/*       */
#define OPC_INB_SET_DEVICE_STATE		42	/*       */
#define OPC_INB_GET_DEVICE_STATE		43	/*       */
#define OPC_INB_SET_DEV_INFO			44	/*       */
/*                   */
#define OPC_INB_RSVD4				45	/*       */
#define OPC_INB_SGPIO_REGISTER			46	/*       */
#define OPC_INB_PCIE_DIAG_EXEC			47	/*       */
#define OPC_INB_SET_CONTROLLER_CONFIG		48	/*       */
#define OPC_INB_GET_CONTROLLER_CONFIG		49	/*       */
#define OPC_INB_REG_DEV				50	/*       */
#define OPC_INB_SAS_HW_EVENT_ACK		51	/*       */
#define OPC_INB_GET_DEVICE_INFO			52	/*       */
#define OPC_INB_GET_PHY_PROFILE			53	/*       */
#define OPC_INB_FLASH_OP_EXT			54	/*       */
#define OPC_INB_SET_PHY_PROFILE			55	/*       */
#define OPC_INB_KEK_MANAGEMENT			256	/*       */
#define OPC_INB_DEK_MANAGEMENT			257	/*       */
#define OPC_INB_SSP_INI_DIF_ENC_IO		258	/*       */
#define OPC_INB_SATA_DIF_ENC_IO			259	/*       */

/*                             */
#define OPC_OUB_ECHO					1	/*       */
#define OPC_OUB_RSVD					4	/*       */
#define OPC_OUB_SSP_COMP				5	/*       */
#define OPC_OUB_SMP_COMP				6	/*       */
#define OPC_OUB_LOCAL_PHY_CNTRL				7	/*       */
#define OPC_OUB_RSVD1					10	/*       */
#define OPC_OUB_DEREG_DEV				11	/*       */
#define OPC_OUB_GET_DEV_HANDLE				12	/*       */
#define OPC_OUB_SATA_COMP				13	/*       */
#define OPC_OUB_SATA_EVENT				14	/*       */
#define OPC_OUB_SSP_EVENT				15	/*       */
#define OPC_OUB_RSVD2					16	/*       */
/*                                                 */
#define OPC_OUB_SSP_RECV_EVENT				18	/*       */
#define OPC_OUB_RSVD3					19	/*       */
#define OPC_OUB_FW_FLASH_UPDATE				20	/*       */
#define OPC_OUB_GPIO_RESPONSE				22	/*       */
#define OPC_OUB_GPIO_EVENT				23	/*       */
#define OPC_OUB_GENERAL_EVENT				24	/*       */
#define OPC_OUB_SSP_ABORT_RSP				26	/*       */
#define OPC_OUB_SATA_ABORT_RSP				27	/*       */
#define OPC_OUB_SAS_DIAG_MODE_START_END			28	/*       */
#define OPC_OUB_SAS_DIAG_EXECUTE			29	/*       */
#define OPC_OUB_GET_TIME_STAMP				30	/*       */
#define OPC_OUB_RSVD4					31	/*       */
#define OPC_OUB_PORT_CONTROL				32	/*       */
#define OPC_OUB_SKIP_ENTRY				33	/*       */
#define OPC_OUB_SMP_ABORT_RSP				34	/*       */
#define OPC_OUB_GET_NVMD_DATA				35	/*       */
#define OPC_OUB_SET_NVMD_DATA				36	/*       */
#define OPC_OUB_DEVICE_HANDLE_REMOVAL			37	/*       */
#define OPC_OUB_SET_DEVICE_STATE			38	/*       */
#define OPC_OUB_GET_DEVICE_STATE			39	/*       */
#define OPC_OUB_SET_DEV_INFO				40	/*       */
#define OPC_OUB_RSVD5					41	/*       */
#define OPC_OUB_HW_EVENT				1792	/*       */
#define OPC_OUB_DEV_HANDLE_ARRIV			1824	/*       */
#define OPC_OUB_THERM_HW_EVENT				1840	/*       */
#define OPC_OUB_SGPIO_RESP				2094	/*       */
#define OPC_OUB_PCIE_DIAG_EXECUTE			2095	/*       */
#define OPC_OUB_DEV_REGIST				2098	/*       */
#define OPC_OUB_SAS_HW_EVENT_ACK			2099	/*       */
#define OPC_OUB_GET_DEVICE_INFO				2100	/*       */
/*                        */
#define OPC_OUB_PHY_START_RESP				2052	/*       */
#define OPC_OUB_PHY_STOP_RESP				2053	/*       */
#define OPC_OUB_SET_CONTROLLER_CONFIG			2096	/*       */
#define OPC_OUB_GET_CONTROLLER_CONFIG			2097	/*       */
#define OPC_OUB_GET_PHY_PROFILE				2101	/*       */
#define OPC_OUB_FLASH_OP_EXT				2102	/*       */
#define OPC_OUB_SET_PHY_PROFILE				2103	/*       */
#define OPC_OUB_KEK_MANAGEMENT_RESP			2304	/*       */
#define OPC_OUB_DEK_MANAGEMENT_RESP			2305	/*       */
#define OPC_OUB_SSP_COALESCED_COMP_RESP			2306	/*       */

/*              */
#define SSC_DISABLE_15			(0x01 << 16)
#define SSC_DISABLE_30			(0x02 << 16)
#define SSC_DISABLE_60			(0x04 << 16)
#define SAS_ASE				(0x01 << 15)
#define SPINHOLD_DISABLE		(0x00 << 14)
#define SPINHOLD_ENABLE			(0x01 << 14)
#define LINKMODE_SAS			(0x01 << 12)
#define LINKMODE_DSATA			(0x02 << 12)
#define LINKMODE_AUTO			(0x03 << 12)
#define LINKRATE_15			(0x01 << 8)
#define LINKRATE_30			(0x02 << 8)
#define LINKRATE_60			(0x06 << 8)

/*                 */
#define	THERMAL_ENABLE			0x1
#define	THERMAL_LOG_ENABLE		0x1
#define THERMAL_OP_CODE			0x6
#define LTEMPHIL			 70
#define RTEMPHIL			100

/*                 */
#define SCRATCH_PAD3_ENC_DISABLED	0x00000000
#define SCRATCH_PAD3_ENC_DIS_ERR	0x00000001
#define SCRATCH_PAD3_ENC_ENA_ERR	0x00000002
#define SCRATCH_PAD3_ENC_READY		0x00000003
#define SCRATCH_PAD3_ENC_MASK		SCRATCH_PAD3_ENC_READY

#define SCRATCH_PAD3_XTS_ENABLED		(1 << 14)
#define SCRATCH_PAD3_SMA_ENABLED		(1 << 4)
#define SCRATCH_PAD3_SMB_ENABLED		(1 << 5)
#define SCRATCH_PAD3_SMF_ENABLED		0
#define SCRATCH_PAD3_SM_MASK			0x000000F0
#define SCRATCH_PAD3_ERR_CODE			0x00FF0000

#define SEC_MODE_SMF				0x0
#define SEC_MODE_SMA				0x100
#define SEC_MODE_SMB				0x200
#define CIPHER_MODE_ECB				0x00000001
#define CIPHER_MODE_XTS				0x00000002
#define KEK_MGMT_SUBOP_KEYCARDUPDATE		0x4

/*                                       */
#define SAS_PROTOCOL_TIMER_CONFIG_PAGE  0x04
#define STP_MCT_TMO                     32
#define SSP_MCT_TMO                     32
#define SAS_MAX_OPEN_TIME				5
#define SMP_MAX_CONN_TIMER              0xFF
#define STP_FRM_TIMER                   0
#define STP_IDLE_TIME                   5 /*                          */
#define SAS_MFD                         0
#define SAS_OPNRJT_RTRY_INTVL           2
#define SAS_DOPNRJT_RTRY_TMO            128
#define SAS_COPNRJT_RTRY_TMO            128

/*
                                                                     
                                                                   
                     
*/
#define SAS_DOPNRJT_RTRY_THR            23438
#define SAS_COPNRJT_RTRY_THR            23438
#define SAS_MAX_AIP                     0x200000
#define IT_NEXUS_TIMEOUT       0x7D0
#define PORT_RECOVERY_TIMEOUT  ((IT_NEXUS_TIMEOUT/100) + 30)

struct mpi_msg_hdr {
	__le32	header;	/*                                      */
	/*                                 */
	/*                                        
                              */
	/*                         */
	/*                                           
                                           */
	/*                         */
	/*                               */
} __attribute__((packed, aligned(4)));

/*
                                                
                                             
 */
struct phy_start_req {
	__le32	tag;
	__le32	ase_sh_lm_slr_phyid;
	struct sas_identify_frame sas_identify; /*          */
	__le32 spasti;
	u32	reserved[21];
} __attribute__((packed, aligned(4)));

/*
                                                
                                     
 */
struct phy_stop_req {
	__le32	tag;
	__le32	phy_id;
	u32	reserved[29];
} __attribute__((packed, aligned(4)));

/*                                      */
struct set_dev_bits_fis {
	u8	fis_type;	/*     */
	u8	n_i_pmport;
	/*                                                              */
	/*                           */
	/*                  */
	/*                */
	u8	status;
	u8	error;
	u32	_r_a;
} __attribute__ ((packed));
/*                                */
struct pio_setup_fis {
	u8	fis_type;	/*      */
	u8	i_d_pmPort;
	/*               */
	/*                           */
	/*                                                                 
      */
	/*               */
	/*                */
	u8	status;
	u8	error;
	u8	lbal;
	u8	lbam;
	u8	lbah;
	u8	device;
	u8	lbal_exp;
	u8	lbam_exp;
	u8	lbah_exp;
	u8	_r_a;
	u8	sector_count;
	u8	sector_count_exp;
	u8	_r_b;
	u8	e_status;
	u8	_r_c[2];
	u8	transfer_count;
} __attribute__ ((packed));

/*
                                                       
                                                    
 */
struct sata_completion_resp {
	__le32	tag;
	__le32	status;
	__le32	param;
	u32	sata_resp[12];
} __attribute__((packed, aligned(4)));

/*
                                                        
                                                           
 */
/*                                  */

struct hw_event_resp {
	__le32	lr_status_evt_portid;
	__le32	evt_param;
	__le32	phyid_npip_portstate;
	struct sas_identify_frame	sas_identify;
	struct dev_to_host_fis	sata_fis;
} __attribute__((packed, aligned(4)));

/*
                                                          
 */

struct thermal_hw_event {
	__le32	thermal_event;
	__le32	rht_lht;
} __attribute__((packed, aligned(4)));

/*
                                                      
                                                         
 */

struct reg_dev_req {
	__le32	tag;
	__le32	phyid_portid;
	__le32	dtype_dlr_mcn_ir_retry;
	__le32	firstburstsize_ITNexustimeout;
	u8	sas_addr[SAS_ADDR_SIZE];
	__le32	upper_device_id;
	u32	reserved[24];
} __attribute__((packed, aligned(4)));

/*
                                                        
                                                                 
                                
 */

struct dereg_dev_req {
	__le32	tag;
	__le32	device_id;
	u32	reserved[29];
} __attribute__((packed, aligned(4)));

/*
                                                           
                                                                     
 */
struct dev_reg_resp {
	__le32	tag;
	__le32	status;
	__le32	device_id;
	u32	reserved[12];
} __attribute__((packed, aligned(4)));

/*
                                                        
                                                   
 */
struct local_phy_ctl_req {
	__le32	tag;
	__le32	phyop_phyid;
	u32	reserved1[29];
} __attribute__((packed, aligned(4)));

/* 
                                                         
                                                            
 */
 struct local_phy_ctl_resp {
	__le32	tag;
	__le32	phyop_phyid;
	__le32	status;
	u32	reserved[12];
} __attribute__((packed, aligned(4)));

#define OP_BITS 0x0000FF00
#define ID_BITS 0x000000FF

/*
                                                   
                                            
 */

struct port_ctl_req {
	__le32	tag;
	__le32	portop_portid;
	__le32	param0;
	__le32	param1;
	u32	reserved1[27];
} __attribute__((packed, aligned(4)));

/*
                                                   
                                                 
 */
struct hw_event_ack_req {
	__le32	tag;
	__le32	phyid_sea_portid;
	__le32	param0;
	__le32	param1;
	u32	reserved1[27];
} __attribute__((packed, aligned(4)));

/*
                                                         
                                                           
 */
struct phy_start_resp {
	__le32	tag;
	__le32	status;
	__le32	phyid;
	u32	reserved[12];
} __attribute__((packed, aligned(4)));

/*
                                                        
                                                          
 */
struct phy_stop_resp {
	__le32	tag;
	__le32	status;
	__le32	phyid;
	u32	reserved[12];
} __attribute__((packed, aligned(4)));

/*
                                                      
                                             
 */
struct ssp_completion_resp {
	__le32	tag;
	__le32	status;
	__le32	param;
	__le32	ssptag_rescv_rescpad;
	struct ssp_response_iu ssp_resp_iu;
	__le32	residual_count;
} __attribute__((packed, aligned(4)));

#define SSP_RESCV_BIT	0x00010000

/*
                                                  
                                               
 */
struct sata_event_resp {
	__le32 tag;
	__le32 event;
	__le32 port_id;
	__le32 device_id;
	u32 reserved;
	__le32 event_param0;
	__le32 event_param1;
	__le32 sata_addr_h32;
	__le32 sata_addr_l32;
	__le32 e_udt1_udt0_crc;
	__le32 e_udt5_udt4_udt3_udt2;
	__le32 a_udt1_udt0_crc;
	__le32 a_udt5_udt4_udt3_udt2;
	__le32 hwdevid_diferr;
	__le32 err_framelen_byteoffset;
	__le32 err_dataframe;
} __attribute__((packed, aligned(4)));

/*
                                                
                                              
 */
struct ssp_event_resp {
	__le32 tag;
	__le32 event;
	__le32 port_id;
	__le32 device_id;
	__le32 ssp_tag;
	__le32 event_param0;
	__le32 event_param1;
	__le32 sas_addr_h32;
	__le32 sas_addr_l32;
	__le32 e_udt1_udt0_crc;
	__le32 e_udt5_udt4_udt3_udt2;
	__le32 a_udt1_udt0_crc;
	__le32 a_udt5_udt4_udt3_udt2;
	__le32 hwdevid_diferr;
	__le32 err_framelen_byteoffset;
	__le32 err_dataframe;
} __attribute__((packed, aligned(4)));

/* 
                                                                  
                                                                     
 */
struct general_event_resp {
	__le32	status;
	__le32	inb_IOMB_payload[14];
} __attribute__((packed, aligned(4)));

#define GENERAL_EVENT_PAYLOAD	14
#define OPCODE_BITS	0x00000fff

/*
                                                  
                                                     
 */
struct smp_req {
	__le32	tag;
	__le32	device_id;
	__le32	len_ip_ir;
	/*                              */
	/*                             */
	/*                        */
	/*                                   */
	/*                         */
	u8	smp_req16[16];
	union {
		u8	smp_req[32];
		struct {
			__le64 long_req_addr;/*                    */
			__le32 long_req_size;/*    */
			u32	_r_a;
			__le64 long_resp_addr;/*                    */
			__le32 long_resp_size;/*    */
			u32	_r_b;
			} long_smp_req;/*                     */
	};
	__le32	rsvd[16];
} __attribute__((packed, aligned(4)));
/*
                                                      
                                                         
 */
struct smp_completion_resp {
	__le32	tag;
	__le32	status;
	__le32	param;
	u8	_r_a[252];
} __attribute__((packed, aligned(4)));

/*
                                                        
                                                              
 */
struct task_abort_req {
	__le32	tag;
	__le32	device_id;
	__le32	tag_to_abort;
	__le32	abort_all;
	u32	reserved[27];
} __attribute__((packed, aligned(4)));

/*                                           */
#define ABORT_MASK		0x3
#define ABORT_SINGLE		0x0
#define ABORT_ALL		0x1

/* 
                                                          
                                                            
 */
struct task_abort_resp {
	__le32	tag;
	__le32	status;
	__le32	scp;
	u32	reserved[12];
} __attribute__((packed, aligned(4)));

/* 
                                                               
                                                                  
 */
struct sas_diag_start_end_req {
	__le32	tag;
	__le32	operation_phyid;
	u32	reserved[29];
} __attribute__((packed, aligned(4)));

/* 
                                                             
                                                                
 */
struct sas_diag_execute_req {
	__le32	tag;
	__le32	cmdtype_cmddesc_phyid;
	__le32	pat1_pat2;
	__le32	threshold;
	__le32	codepat_errmsk;
	__le32	pmon;
	__le32	pERF1CTL;
	u32	reserved[24];
} __attribute__((packed, aligned(4)));

#define SAS_DIAG_PARAM_BYTES 24

/*
                                                       
                                                          
 */
struct set_dev_state_req {
	__le32	tag;
	__le32	device_id;
	__le32	nds;
	u32	reserved[28];
} __attribute__((packed, aligned(4)));

/*
                                                 
                                                       
                                                             
 */

struct sata_start_req {
	__le32	tag;
	__le32	device_id;
	__le32	data_len;
	__le32	ncqtag_atap_dir_m_dad;
	struct host_to_dev_fis	sata_fis;
	u32	reserved1;
	u32	reserved2;	/*                                */
				/*                        */
	u32	addr_low;	/*                            */
	u32	addr_high;	/*                                */
	__le32	len;		/*                                  */
				/*                        */
	__le32	esgl;		/*                            */
	__le32	atapi_scsi_cdb[4];	/*                               */
	/*                                              */
	__le32	key_index_mode;	/*          */
	__le32	sector_cnt_enss;/*          */
	__le32	keytagl;	/*          */
	__le32	keytagh;	/*          */
	__le32	twk_val0;	/*          */
	__le32	twk_val1;	/*          */
	__le32	twk_val2;	/*          */
	__le32	twk_val3;	/*          */
	__le32	enc_addr_low;	/*                                       */
	__le32	enc_addr_high;	/*                                      */
	__le32	enc_len;	/*                             */
	__le32	enc_esgl;	/*                               */
} __attribute__((packed, aligned(4)));

/* 
                                                       
                                                          
 */
struct ssp_ini_tm_start_req {
	__le32	tag;
	__le32	device_id;
	__le32	relate_tag;
	__le32	tmf;
	u8	lun[8];
	__le32	ds_ads_m;
	u32	reserved[24];
} __attribute__((packed, aligned(4)));

struct ssp_info_unit {
	u8	lun[8];/*                          */
	u8	reserved1;/*          */
	u8	efb_prio_attr;
	/*                        */
	/*                     */
	/*                      */
	u8	reserved2;	/*          */
	u8	additional_cdb_len;
	/*                           */
	/*                 */
	u8	cdb[16];/*                                    */
} __attribute__((packed, aligned(4)));

/* 
                                                       
                                                          
                                                             
 */
struct ssp_ini_io_start_req {
	__le32	tag;
	__le32	device_id;
	__le32	data_len;
	__le32	dad_dir_m_tlr;
	struct ssp_info_unit	ssp_iu;
	__le32	addr_low;	/*                                   */
				/*                              */
	__le32	addr_high;	/*                                 */
				/*                              */
	__le32	len;		/*                               */
				/*                               */
	__le32	esgl;		/*                                    */
				/*                                   */
	/*                                              */
	u8	udt[12];	/*             */
	__le32	sectcnt_ios;	/*          */
	__le32	key_cmode;	/*          */
	__le32	ks_enss;	/*          */
	__le32	keytagl;	/*          */
	__le32	keytagh;	/*          */
	__le32	twk_val0;	/*          */
	__le32	twk_val1;	/*          */
	__le32	twk_val2;	/*          */
	__le32	twk_val3;	/*          */
	__le32	enc_addr_low;	/*                                   */
	__le32	enc_addr_high;	/*                                  */
	__le32	enc_len;	/*                             */
	__le32	enc_esgl;	/*                                   */
} __attribute__((packed, aligned(4)));

/* 
                                                          
                                                          
 */
struct ssp_dif_enc_io_req {
	__le32	tag;
	__le32	device_id;
	__le32	data_len;
	__le32	dirMTlr;
	__le32	sspiu0;
	__le32	sspiu1;
	__le32	sspiu2;
	__le32	sspiu3;
	__le32	sspiu4;
	__le32	sspiu5;
	__le32	sspiu6;
	__le32	epl_des;
	__le32	dpl_desl_ndplr;
	__le32	dpl_desh;
	__le32	uum_uuv_bss_difbits;
	u8	udt[12];
	__le32	sectcnt_ios;
	__le32	key_cmode;
	__le32	ks_enss;
	__le32	keytagl;
	__le32	keytagh;
	__le32	twk_val0;
	__le32	twk_val1;
	__le32	twk_val2;
	__le32	twk_val3;
	__le32	addr_low;
	__le32	addr_high;
	__le32	len;
	__le32	esgl;
} __attribute__((packed, aligned(4)));

/* 
                                                
                                                     
 */
struct fw_flash_Update_req {
	__le32	tag;
	__le32	cur_image_offset;
	__le32	cur_image_len;
	__le32	total_image_len;
	u32	reserved0[7];
	__le32	sgl_addr_lo;
	__le32	sgl_addr_hi;
	__le32	len;
	__le32	ext_reserved;
	u32	reserved1[16];
} __attribute__((packed, aligned(4)));

#define FWFLASH_IOMB_RESERVED_LEN 0x07
/* 
                                                       
                                                          
  
 */
 struct fw_flash_Update_resp {
	__le32	tag;
	__le32	status;
	u32	reserved[13];
} __attribute__((packed, aligned(4)));

/* 
                                                   
                                            
 */
struct get_nvm_data_req {
	__le32	tag;
	__le32	len_ir_vpdd;
	__le32	vpd_offset;
	u32	reserved[8];
	__le32	resp_addr_lo;
	__le32	resp_addr_hi;
	__le32	resp_len;
	u32	reserved1[17];
} __attribute__((packed, aligned(4)));

struct set_nvm_data_req {
	__le32	tag;
	__le32	len_ir_vpdd;
	__le32	vpd_offset;
	u32	reserved[8];
	__le32	resp_addr_lo;
	__le32	resp_addr_hi;
	__le32	resp_len;
	u32	reserved1[17];
} __attribute__((packed, aligned(4)));

/* 
                                                             
                                         
 */
struct set_ctrl_cfg_req {
	__le32	tag;
	__le32	cfg_pg[14];
	u32	reserved[16];
} __attribute__((packed, aligned(4)));

/* 
                                                             
                                           
 */
struct get_ctrl_cfg_req {
	__le32	tag;
	__le32	pgcd;
	__le32	int_vec;
	u32	reserved[28];
} __attribute__((packed, aligned(4)));

/* 
                                                      
                         
 */
struct kek_mgmt_req {
	__le32	tag;
	__le32	new_curidx_ksop;
	u32	reserved;
	__le32	kblob[12];
	u32	reserved1[16];
} __attribute__((packed, aligned(4)));

/* 
                                                      
                         
 */
struct dek_mgmt_req {
	__le32	tag;
	__le32	kidx_dsop;
	__le32	dekidx;
	__le32	addr_l;
	__le32	addr_h;
	__le32	nent;
	__le32	dbf_tblsize;
	u32	reserved[24];
} __attribute__((packed, aligned(4)));

/* 
                                                       
                                          
 */
struct set_phy_profile_req {
	__le32	tag;
	__le32	ppc_phyid;
	u32	reserved[29];
} __attribute__((packed, aligned(4)));

/* 
                                                       
                                          
 */
struct get_phy_profile_req {
	__le32	tag;
	__le32	ppc_phyid;
	__le32	profile[29];
} __attribute__((packed, aligned(4)));

/* 
                                                   
                                    
 */
struct ext_flash_partition_req {
	__le32	tag;
	__le32	cmd;
	__le32	offset;
	__le32	len;
	u32	reserved[7];
	__le32	addr_low;
	__le32	addr_high;
	__le32	len1;
	__le32	ext;
	u32	reserved1[16];
} __attribute__((packed, aligned(4)));

#define TWI_DEVICE	0x0
#define C_SEEPROM	0x1
#define VPD_FLASH	0x4
#define AAP1_RDUMP	0x5
#define IOP_RDUMP	0x6
#define EXPAN_ROM	0x7

#define IPMode		0x80000000
#define NVMD_TYPE	0x0000000F
#define NVMD_STAT	0x0000FFFF
#define NVMD_LEN	0xFF000000
/* 
                                                     
                                                        
 */
struct get_nvm_data_resp {
	__le32		tag;
	__le32		ir_tda_bn_dps_das_nvm;
	__le32		dlen_status;
	__le32		nvm_data[12];
} __attribute__((packed, aligned(4)));

/* 
                                                                
                                                                   
  
 */
struct sas_diag_start_end_resp {
	__le32		tag;
	__le32		status;
	u32		reserved[13];
} __attribute__((packed, aligned(4)));

/* 
                                                              
                                                                 
  
 */
struct sas_diag_execute_resp {
	__le32		tag;
	__le32		cmdtype_cmddesc_phyid;
	__le32		Status;
	__le32		ReportData;
	u32		reserved[11];
} __attribute__((packed, aligned(4)));

/* 
                                                        
                                                           
  
 */
struct set_dev_state_resp {
	__le32		tag;
	__le32		status;
	__le32		device_id;
	__le32		pds_nds;
	u32		reserved[11];
} __attribute__((packed, aligned(4)));

/*                                          */
/* 
                                                             
                                         
 */
struct set_ctrl_cfg_resp {
	__le32 tag;
	__le32 status;
	__le32 err_qlfr_pgcd;
	u32 reserved[12];
} __attribute__((packed, aligned(4)));

struct get_ctrl_cfg_resp {
	__le32 tag;
	__le32 status;
	__le32 err_qlfr;
	__le32 confg_page[12];
} __attribute__((packed, aligned(4)));

struct kek_mgmt_resp {
	__le32 tag;
	__le32 status;
	__le32 kidx_new_curr_ksop;
	__le32 err_qlfr;
	u32 reserved[11];
} __attribute__((packed, aligned(4)));

struct dek_mgmt_resp {
	__le32 tag;
	__le32 status;
	__le32 kekidx_tbls_dsop;
	__le32 dekidx;
	__le32 err_qlfr;
	u32 reserved[10];
} __attribute__((packed, aligned(4)));

struct get_phy_profile_resp {
	__le32 tag;
	__le32 status;
	__le32 ppc_phyid;
	__le32 ppc_specific_rsp[12];
} __attribute__((packed, aligned(4)));

struct flash_op_ext_resp {
	__le32 tag;
	__le32 cmd;
	__le32 status;
	__le32 epart_size;
	__le32 epart_sect_size;
	u32 reserved[10];
} __attribute__((packed, aligned(4)));

struct set_phy_profile_resp {
	__le32 tag;
	__le32 status;
	__le32 ppc_phyid;
	__le32 ppc_specific_rsp[12];
} __attribute__((packed, aligned(4)));

struct ssp_coalesced_comp_resp {
	__le32 coal_cnt;
	__le32 tag0;
	__le32 ssp_tag0;
	__le32 tag1;
	__le32 ssp_tag1;
	__le32 add_tag_ssp_tag[10];
} __attribute__((packed, aligned(4)));

/*                                        */

/*                                                                
  
 */
struct SASProtocolTimerConfig {
	__le32 pageCode;			/*   */
	__le32 MST_MSI;				/*   */
	__le32 STP_SSP_MCT_TMO;			/*   */
	__le32 STP_FRM_TMO;			/*   */
	__le32 STP_IDLE_TMO;			/*   */
	__le32 OPNRJT_RTRY_INTVL;		/*   */
	__le32 Data_Cmd_OPNRJT_RTRY_TMO;	/*   */
	__le32 Data_Cmd_OPNRJT_RTRY_THR;	/*   */
	__le32 MAX_AIP;				/*   */
} __attribute__((packed, aligned(4)));

typedef struct SASProtocolTimerConfig SASProtocolTimerConfig_t;

#define NDS_BITS 0x0F
#define PDS_BITS 0xF0

/*
                 
 */

#define HW_EVENT_RESET_START			0x01
#define HW_EVENT_CHIP_RESET_COMPLETE		0x02
#define HW_EVENT_PHY_STOP_STATUS		0x03
#define HW_EVENT_SAS_PHY_UP			0x04
#define HW_EVENT_SATA_PHY_UP			0x05
#define HW_EVENT_SATA_SPINUP_HOLD		0x06
#define HW_EVENT_PHY_DOWN			0x07
#define HW_EVENT_PORT_INVALID			0x08
#define HW_EVENT_BROADCAST_CHANGE		0x09
#define HW_EVENT_PHY_ERROR			0x0A
#define HW_EVENT_BROADCAST_SES			0x0B
#define HW_EVENT_INBOUND_CRC_ERROR		0x0C
#define HW_EVENT_HARD_RESET_RECEIVED		0x0D
#define HW_EVENT_MALFUNCTION			0x0E
#define HW_EVENT_ID_FRAME_TIMEOUT		0x0F
#define HW_EVENT_BROADCAST_EXP			0x10
#define HW_EVENT_PHY_START_STATUS		0x11
#define HW_EVENT_LINK_ERR_INVALID_DWORD		0x12
#define HW_EVENT_LINK_ERR_DISPARITY_ERROR	0x13
#define HW_EVENT_LINK_ERR_CODE_VIOLATION	0x14
#define HW_EVENT_LINK_ERR_LOSS_OF_DWORD_SYNCH	0x15
#define HW_EVENT_LINK_ERR_PHY_RESET_FAILED	0x16
#define HW_EVENT_PORT_RECOVERY_TIMER_TMO	0x17
#define HW_EVENT_PORT_RECOVER			0x18
#define HW_EVENT_PORT_RESET_TIMER_TMO		0x19
#define HW_EVENT_PORT_RESET_COMPLETE		0x20
#define EVENT_BROADCAST_ASYNCH_EVENT		0x21

/*            */
#define PORT_NOT_ESTABLISHED			0x00
#define PORT_VALID				0x01
#define PORT_LOSTCOMM				0x02
#define PORT_IN_RESET				0x04
#define PORT_3RD_PARTY_RESET			0x07
#define PORT_INVALID				0x08

/*
                                           
 */

#define IO_SUCCESS				0x00
#define IO_ABORTED				0x01
#define IO_OVERFLOW				0x02
#define IO_UNDERFLOW				0x03
#define IO_FAILED				0x04
#define IO_ABORT_RESET				0x05
#define IO_NOT_VALID				0x06
#define IO_NO_DEVICE				0x07
#define IO_ILLEGAL_PARAMETER			0x08
#define IO_LINK_FAILURE				0x09
#define IO_PROG_ERROR				0x0A

#define IO_EDC_IN_ERROR				0x0B
#define IO_EDC_OUT_ERROR			0x0C
#define IO_ERROR_HW_TIMEOUT			0x0D
#define IO_XFER_ERROR_BREAK			0x0E
#define IO_XFER_ERROR_PHY_NOT_READY		0x0F
#define IO_OPEN_CNX_ERROR_PROTOCOL_NOT_SUPPORTED	0x10
#define IO_OPEN_CNX_ERROR_ZONE_VIOLATION		0x11
#define IO_OPEN_CNX_ERROR_BREAK				0x12
#define IO_OPEN_CNX_ERROR_IT_NEXUS_LOSS			0x13
#define IO_OPEN_CNX_ERROR_BAD_DESTINATION		0x14
#define IO_OPEN_CNX_ERROR_CONNECTION_RATE_NOT_SUPPORTED	0x15
#define IO_OPEN_CNX_ERROR_STP_RESOURCES_BUSY		0x16
#define IO_OPEN_CNX_ERROR_WRONG_DESTINATION		0x17
/*                                          */
#define IO_OPEN_CNX_ERROR_UNKNOWN_ERROR			0x18
#define IO_XFER_ERROR_NAK_RECEIVED			0x19
#define IO_XFER_ERROR_ACK_NAK_TIMEOUT			0x1A
#define IO_XFER_ERROR_PEER_ABORTED			0x1B
#define IO_XFER_ERROR_RX_FRAME				0x1C
#define IO_XFER_ERROR_DMA				0x1D
#define IO_XFER_ERROR_CREDIT_TIMEOUT			0x1E
#define IO_XFER_ERROR_SATA_LINK_TIMEOUT			0x1F
#define IO_XFER_ERROR_SATA				0x20

/*                                          */
#define IO_XFER_ERROR_ABORTED_DUE_TO_SRST		0x22
#define IO_XFER_ERROR_REJECTED_NCQ_MODE			0x21
#define IO_XFER_ERROR_ABORTED_NCQ_MODE			0x23
#define IO_XFER_OPEN_RETRY_TIMEOUT			0x24
/*                                          */
#define IO_XFER_SMP_RESP_CONNECTION_ERROR		0x25
#define IO_XFER_ERROR_UNEXPECTED_PHASE			0x26
#define IO_XFER_ERROR_XFER_RDY_OVERRUN			0x27
#define IO_XFER_ERROR_XFER_RDY_NOT_EXPECTED		0x28
#define IO_XFER_ERROR_CMD_ISSUE_ACK_NAK_TIMEOUT		0x30

/*                                                                 */
#define IO_XFER_ERROR_CMD_ISSUE_BREAK_BEFORE_ACK_NAK	0x31
#define IO_XFER_ERROR_CMD_ISSUE_PHY_DOWN_BEFORE_ACK_NAK	0x32

#define IO_XFER_ERROR_OFFSET_MISMATCH			0x34
#define IO_XFER_ERROR_XFER_ZERO_DATA_LEN		0x35
#define IO_XFER_CMD_FRAME_ISSUED			0x36
#define IO_ERROR_INTERNAL_SMP_RESOURCE			0x37
#define IO_PORT_IN_RESET				0x38
#define IO_DS_NON_OPERATIONAL				0x39
#define IO_DS_IN_RECOVERY				0x3A
#define IO_TM_TAG_NOT_FOUND				0x3B
#define IO_XFER_PIO_SETUP_ERROR				0x3C
#define IO_SSP_EXT_IU_ZERO_LEN_ERROR			0x3D
#define IO_DS_IN_ERROR					0x3E
#define IO_OPEN_CNX_ERROR_HW_RESOURCE_BUSY		0x3F
#define IO_ABORT_IN_PROGRESS				0x40
#define IO_ABORT_DELAYED				0x41
#define IO_INVALID_LENGTH				0x42

/*                                                           */

#define IO_OPEN_CNX_ERROR_HW_RESOURCE_BUSY_ALT		0x43
#define IO_XFER_OPEN_RETRY_BACKOFF_THRESHOLD_REACHED	0x44
#define IO_OPEN_CNX_ERROR_IT_NEXUS_LOSS_OPEN_TMO	0x45
#define IO_OPEN_CNX_ERROR_IT_NEXUS_LOSS_NO_DEST		0x46
#define IO_OPEN_CNX_ERROR_IT_NEXUS_LOSS_OPEN_COLLIDE	0x47
#define IO_OPEN_CNX_ERROR_IT_NEXUS_LOSS_PATHWAY_BLOCKED	0x48
#define IO_DS_INVALID					0x49
/*                                                */
#define IO_XFER_ERR_LAST_PIO_DATAIN_CRC_ERR	0x52
#define IO_XFER_DMA_ACTIVATE_TIMEOUT		0x53
#define IO_XFER_ERROR_INTERNAL_CRC_ERROR	0x54
#define MPI_IO_RQE_BUSY_FULL			0x55
#define IO_XFER_ERR_EOB_DATA_OVERRUN		0x56
#define IO_XFR_ERROR_INVALID_SSP_RSP_FRAME	0x57
#define IO_OPEN_CNX_ERROR_OPEN_PREEMPTED	0x58

#define MPI_ERR_IO_RESOURCE_UNAVAILABLE		0x1004
#define MPI_ERR_ATAPI_DEVICE_BUSY		0x1024

#define IO_XFR_ERROR_DEK_KEY_CACHE_MISS		0x2040
/*
                                                               
                                                                  
                                           
 */
#define IO_XFR_ERROR_DEK_KEY_TAG_MISMATCH	0x2041
#define IO_XFR_ERROR_CIPHER_MODE_INVALID	0x2042
/*
                                                                  
                                                                   
 */
#define IO_XFR_ERROR_DEK_IV_MISMATCH		0x2043
/*                                                               
                                             */
#define IO_XFR_ERROR_DEK_RAM_INTERFACE_ERROR	0x2044
/*                                                               
                                             */
#define IO_XFR_ERROR_INTERNAL_RAM		0x2045
/*
                                   
                                                                       
                                                     
 */
#define IO_XFR_ERROR_DEK_INDEX_OUT_OF_BOUNDS0x2046

/*                                          */
#define IO_XFR_ERROR_DIF_MISMATCH			0x3000
#define IO_XFR_ERROR_DIF_APPLICATION_TAG_MISMATCH	0x3001
#define IO_XFR_ERROR_DIF_REFERENCE_TAG_MISMATCH		0x3002
#define IO_XFR_ERROR_DIF_CRC_MISMATCH			0x3003

/*                                                                     */
#define OPR_MGMT_OP_NOT_SUPPORTED			0x2060
#define OPR_MGMT_MPI_ENC_ERR_OPR_PARAM_ILLEGAL		0x2061
#define OPR_MGMT_MPI_ENC_ERR_OPR_ID_NOT_FOUND		0x2062
#define OPR_MGMT_MPI_ENC_ERR_OPR_ROLE_NOT_MATCH		0x2063
#define OPR_MGMT_MPI_ENC_ERR_OPR_MAX_NUM_EXCEEDED	0x2064
#define OPR_MGMT_MPI_ENC_ERR_KEK_UNWRAP_FAIL		0x2022
#define OPR_MGMT_MPI_ENC_ERR_NVRAM_OPERATION_FAILURE	0x2023
/*                                                                */

/*                                                         
                                               
                         
 */
#define IO_ERROR_UNKNOWN_GENERIC			0x2023

/*                         */

#define SPCv_MSGU_CFG_TABLE_UPDATE		0x01
#define SPCv_MSGU_CFG_TABLE_RESET		0x02
#define SPCv_MSGU_CFG_TABLE_FREEZE		0x04
#define SPCv_MSGU_CFG_TABLE_UNFREEZE		0x08
#define MSGU_IBDB_SET				0x00
#define MSGU_HOST_INT_STATUS			0x08
#define MSGU_HOST_INT_MASK			0x0C
#define MSGU_IOPIB_INT_STATUS			0x18
#define MSGU_IOPIB_INT_MASK			0x1C
#define MSGU_IBDB_CLEAR				0x20

#define MSGU_MSGU_CONTROL			0x24
#define MSGU_ODR				0x20
#define MSGU_ODCR				0x28

#define MSGU_ODMR				0x30
#define MSGU_ODMR_U				0x34
#define MSGU_ODMR_CLR				0x38
#define MSGU_ODMR_CLR_U				0x3C
#define MSGU_OD_RSVD				0x40

#define MSGU_SCRATCH_PAD_0			0x44
#define MSGU_SCRATCH_PAD_1			0x48
#define MSGU_SCRATCH_PAD_2			0x4C
#define MSGU_SCRATCH_PAD_3			0x50
#define MSGU_HOST_SCRATCH_PAD_0			0x54
#define MSGU_HOST_SCRATCH_PAD_1			0x58
#define MSGU_HOST_SCRATCH_PAD_2			0x5C
#define MSGU_HOST_SCRATCH_PAD_3			0x60
#define MSGU_HOST_SCRATCH_PAD_4			0x64
#define MSGU_HOST_SCRATCH_PAD_5			0x68
#define MSGU_HOST_SCRATCH_PAD_6			0x6C
#define MSGU_HOST_SCRATCH_PAD_7			0x70

/*                                  */
#define ODMR_MASK_ALL			0xFFFFFFFF/*         
                      */
#define ODMR_CLEAR_ALL			0	/*          
                      */
/*                                  */
#define ODCR_CLEAR_ALL			0xFFFFFFFF /*         
                     */
/*                */
#define MSIX_TABLE_OFFSET		0x2000
#define MSIX_TABLE_ELEMENT_SIZE		0x10
#define MSIX_INTERRUPT_CONTROL_OFFSET	0xC
#define MSIX_TABLE_BASE			(MSIX_TABLE_OFFSET + \
					MSIX_INTERRUPT_CONTROL_OFFSET)
#define MSIX_INTERRUPT_DISABLE		0x1
#define MSIX_INTERRUPT_ENABLE		0x0

/*                                            */
#define SCRATCH_PAD_RAAE_READY		0x3
#define SCRATCH_PAD_ILA_READY		0xC
#define SCRATCH_PAD_BOOT_LOAD_SUCCESS	0x0
#define SCRATCH_PAD_IOP0_READY		0xC00
#define SCRATCH_PAD_IOP1_READY		0x3000

/*                   */
#define SCRATCH_PAD1_BOOTSTATE_MASK		0x70	/*         */
#define SCRATCH_PAD1_BOOTSTATE_SUCESS		0x0	/*                 */
#define SCRATCH_PAD1_BOOTSTATE_HDA_SEEPROM	0x10	/*             */
#define SCRATCH_PAD1_BOOTSTATE_HDA_BOOTSTRAP	0x20	/*                    */
#define SCRATCH_PAD1_BOOTSTATE_HDA_SOFTRESET	0x30	/*                */
#define SCRATCH_PAD1_BOOTSTATE_CRIT_ERROR	0x40	/*                    */
#define SCRATCH_PAD1_BOOTSTATE_R1		0x50	/*          */
#define SCRATCH_PAD1_BOOTSTATE_R2		0x60	/*          */
#define SCRATCH_PAD1_BOOTSTATE_FATAL		0x70	/*             */

 /*                                            */
#define SCRATCH_PAD2_POR		0x00	/*                */
#define SCRATCH_PAD2_SFR		0x01	/*                  */
#define SCRATCH_PAD2_ERR		0x02	/*             */
#define SCRATCH_PAD2_RDY		0x03	/*             */
#define SCRATCH_PAD2_FWRDY_RST		0x04	/*                            */
#define SCRATCH_PAD2_IOPRDY_RST		0x08	/*                          */
#define SCRATCH_PAD2_STATE_MASK		0xFFFFFFF4 /*             
                    */
#define SCRATCH_PAD2_RESERVED		0x000003FC/*             
                     */

#define SCRATCH_PAD_ERROR_MASK		0xFFFFFC00 /*                 */
#define SCRATCH_PAD_STATE_MASK		0x00000003 /*                 */

/*                                         */
#define MAIN_SIGNATURE_OFFSET		0x00 /*            */
#define MAIN_INTERFACE_REVISION		0x04 /*            */
#define MAIN_FW_REVISION		0x08 /*            */
#define MAIN_MAX_OUTSTANDING_IO_OFFSET	0x0C /*            */
#define MAIN_MAX_SGL_OFFSET		0x10 /*            */
#define MAIN_CNTRL_CAP_OFFSET		0x14 /*            */
#define MAIN_GST_OFFSET			0x18 /*            */
#define MAIN_IBQ_OFFSET			0x1C /*            */
#define MAIN_OBQ_OFFSET			0x20 /*            */
#define MAIN_IQNPPD_HPPD_OFFSET		0x24 /*            */

/*                    */
#define MAIN_EVENT_CRC_CHECK		0x48 /*            */
#define MAIN_EVENT_LOG_ADDR_HI		0x50 /*            */
#define MAIN_EVENT_LOG_ADDR_LO		0x54 /*            */
#define MAIN_EVENT_LOG_BUFF_SIZE	0x58 /*            */
#define MAIN_EVENT_LOG_OPTION		0x5C /*            */
#define MAIN_PCS_EVENT_LOG_ADDR_HI	0x60 /*            */
#define MAIN_PCS_EVENT_LOG_ADDR_LO	0x64 /*            */
#define MAIN_PCS_EVENT_LOG_BUFF_SIZE	0x68 /*            */
#define MAIN_PCS_EVENT_LOG_OPTION	0x6C /*            */
#define MAIN_FATAL_ERROR_INTERRUPT	0x70 /*            */
#define MAIN_FATAL_ERROR_RDUMP0_OFFSET	0x74 /*            */
#define MAIN_FATAL_ERROR_RDUMP0_LENGTH	0x78 /*            */
#define MAIN_FATAL_ERROR_RDUMP1_OFFSET	0x7C /*            */
#define MAIN_FATAL_ERROR_RDUMP1_LENGTH	0x80 /*            */
#define MAIN_GPIO_LED_FLAGS_OFFSET	0x84 /*            */
#define MAIN_ANALOG_SETUP_OFFSET	0x88 /*            */

#define MAIN_INT_VECTOR_TABLE_OFFSET	0x8C /*            */
#define MAIN_SAS_PHY_ATTR_TABLE_OFFSET	0x90 /*            */
#define MAIN_PORT_RECOVERY_TIMER	0x94 /*            */
#define MAIN_INT_REASSERTION_DELAY	0x98 /*            */

/*                                           */
#define GST_GSTLEN_MPIS_OFFSET		0x00
#define GST_IQ_FREEZE_STATE0_OFFSET	0x04
#define GST_IQ_FREEZE_STATE1_OFFSET	0x08
#define GST_MSGUTCNT_OFFSET		0x0C
#define GST_IOPTCNT_OFFSET		0x10
/*                    */
#define GST_GPIO_INPUT_VAL		0x38
/*                    */
#define GST_RERRINFO_OFFSET0		0x44
#define GST_RERRINFO_OFFSET1		0x48
#define GST_RERRINFO_OFFSET2		0x4c
#define GST_RERRINFO_OFFSET3		0x50
#define GST_RERRINFO_OFFSET4		0x54
#define GST_RERRINFO_OFFSET5		0x58
#define GST_RERRINFO_OFFSET6		0x5c
#define GST_RERRINFO_OFFSET7		0x60

/*                                  */
#define GST_MPI_STATE_UNINIT		0x00
#define GST_MPI_STATE_INIT		0x01
#define GST_MPI_STATE_TERMINATION	0x02
#define GST_MPI_STATE_ERROR		0x03
#define GST_MPI_STATE_MASK		0x07

/*                        */

#define PSPA_PHYSTATE0_OFFSET		0x00 /*         */
#define PSPA_OB_HW_EVENT_PID0_OFFSET	0x04 /*           */
#define PSPA_PHYSTATE1_OFFSET		0x08 /*           */
#define PSPA_OB_HW_EVENT_PID1_OFFSET	0x0C /*           */
#define PSPA_PHYSTATE2_OFFSET		0x10 /*           */
#define PSPA_OB_HW_EVENT_PID2_OFFSET	0x14 /*           */
#define PSPA_PHYSTATE3_OFFSET		0x18 /*           */
#define PSPA_OB_HW_EVENT_PID3_OFFSET	0x1C /*           */
#define PSPA_PHYSTATE4_OFFSET		0x20 /*           */
#define PSPA_OB_HW_EVENT_PID4_OFFSET	0x24 /*           */
#define PSPA_PHYSTATE5_OFFSET		0x28 /*            */
#define PSPA_OB_HW_EVENT_PID5_OFFSET	0x2C /*            */
#define PSPA_PHYSTATE6_OFFSET		0x30 /*            */
#define PSPA_OB_HW_EVENT_PID6_OFFSET	0x34 /*            */
#define PSPA_PHYSTATE7_OFFSET		0x38 /*            */
#define PSPA_OB_HW_EVENT_PID7_OFFSET	0x3C /*            */
#define PSPA_PHYSTATE8_OFFSET		0x40 /*            */
#define PSPA_OB_HW_EVENT_PID8_OFFSET	0x44 /*            */
#define PSPA_PHYSTATE9_OFFSET		0x48 /*            */
#define PSPA_OB_HW_EVENT_PID9_OFFSET	0x4C /*            */
#define PSPA_PHYSTATE10_OFFSET		0x50 /*            */
#define PSPA_OB_HW_EVENT_PID10_OFFSET	0x54 /*            */
#define PSPA_PHYSTATE11_OFFSET		0x58 /*            */
#define PSPA_OB_HW_EVENT_PID11_OFFSET	0x5C /*            */
#define PSPA_PHYSTATE12_OFFSET		0x60 /*            */
#define PSPA_OB_HW_EVENT_PID12_OFFSET	0x64 /*            */
#define PSPA_PHYSTATE13_OFFSET		0x68 /*            */
#define PSPA_OB_HW_EVENT_PID13_OFFSET	0x6c /*            */
#define PSPA_PHYSTATE14_OFFSET		0x70 /*            */
#define PSPA_OB_HW_EVENT_PID14_OFFSET	0x74 /*            */
#define PSPA_PHYSTATE15_OFFSET		0x78 /*            */
#define PSPA_OB_HW_EVENT_PID15_OFFSET	0x7c /*            */
/*          */

/*                                                  */
#define IB_PROPERITY_OFFSET		0x00
#define IB_BASE_ADDR_HI_OFFSET		0x04
#define IB_BASE_ADDR_LO_OFFSET		0x08
#define IB_CI_BASE_ADDR_HI_OFFSET	0x0C
#define IB_CI_BASE_ADDR_LO_OFFSET	0x10
#define IB_PIPCI_BAR			0x14
#define IB_PIPCI_BAR_OFFSET		0x18
#define IB_RESERVED_OFFSET		0x1C

/*                                                   */
#define OB_PROPERITY_OFFSET		0x00
#define OB_BASE_ADDR_HI_OFFSET		0x04
#define OB_BASE_ADDR_LO_OFFSET		0x08
#define OB_PI_BASE_ADDR_HI_OFFSET	0x0C
#define OB_PI_BASE_ADDR_LO_OFFSET	0x10
#define OB_CIPCI_BAR			0x14
#define OB_CIPCI_BAR_OFFSET		0x18
#define OB_INTERRUPT_COALES_OFFSET	0x1C
#define OB_DYNAMIC_COALES_OFFSET	0x20
#define OB_PROPERTY_INT_ENABLE		0x40000000

#define MBIC_NMI_ENABLE_VPE0_IOP	0x000418
#define MBIC_NMI_ENABLE_VPE0_AAP1	0x000418
/*                                                 */
#define PCIE_EVENT_INTERRUPT_ENABLE	0x003040
#define PCIE_EVENT_INTERRUPT		0x003044
#define PCIE_ERROR_INTERRUPT_ENABLE	0x003048
#define PCIE_ERROR_INTERRUPT		0x00304C

/*                 */
#define SPC_REG_SOFT_RESET 0x00001000
#define SPCv_NORMAL_RESET_VALUE		0x1

#define SPCv_SOFT_RESET_READ_MASK		0xC0
#define SPCv_SOFT_RESET_NO_RESET		0x0
#define SPCv_SOFT_RESET_NORMAL_RESET_OCCURED	0x40
#define SPCv_SOFT_RESET_HDA_MODE_OCCURED	0x80
#define SPCv_SOFT_RESET_CHIP_RESET_OCCURED	0xC0

/*                                                     */
#define SPC_SOFT_RESET_SIGNATURE	0x252acbcd
/*                          */

/*                                                                   */
#define SPC_REG_RESET			0x000000/*                */

/*                                       */
#define SPC_REG_RESET_OSSP		0x00000001
#define SPC_REG_RESET_RAAE		0x00000002
#define SPC_REG_RESET_PCS_SPBC		0x00000004
#define SPC_REG_RESET_PCS_IOP_SS	0x00000008
#define SPC_REG_RESET_PCS_AAP1_SS	0x00000010
#define SPC_REG_RESET_PCS_AAP2_SS	0x00000020
#define SPC_REG_RESET_PCS_LM		0x00000040
#define SPC_REG_RESET_PCS		0x00000080
#define SPC_REG_RESET_GSM		0x00000100
#define SPC_REG_RESET_DDR2		0x00010000
#define SPC_REG_RESET_BDMA_CORE		0x00020000
#define SPC_REG_RESET_BDMA_SXCBI	0x00040000
#define SPC_REG_RESET_PCIE_AL_SXCBI	0x00080000
#define SPC_REG_RESET_PCIE_PWR		0x00100000
#define SPC_REG_RESET_PCIE_SFT		0x00200000
#define SPC_REG_RESET_PCS_SXCBI		0x00400000
#define SPC_REG_RESET_LMS_SXCBI		0x00800000
#define SPC_REG_RESET_PMIC_SXCBI	0x01000000
#define SPC_REG_RESET_PMIC_CORE		0x02000000
#define SPC_REG_RESET_PCIE_PC_SXCBI	0x04000000
#define SPC_REG_RESET_DEVICE		0x80000000

/*                                                    */
#define SPCV_IBW_AXI_TRANSLATION_LOW	0x001010

#define MBIC_AAP1_ADDR_BASE		0x060000
#define MBIC_IOP_ADDR_BASE		0x070000
#define GSM_ADDR_BASE			0x0700000
/*                                       */
#define GSM_CONFIG_RESET		0x00000000
#define RAM_ECC_DB_ERR			0x00000018
#define GSM_READ_ADDR_PARITY_INDIC	0x00000058
#define GSM_WRITE_ADDR_PARITY_INDIC	0x00000060
#define GSM_WRITE_DATA_PARITY_INDIC	0x00000068
#define GSM_READ_ADDR_PARITY_CHECK	0x00000038
#define GSM_WRITE_ADDR_PARITY_CHECK	0x00000040
#define GSM_WRITE_DATA_PARITY_CHECK	0x00000048

#define RB6_ACCESS_REG			0x6A0000
#define HDAC_EXEC_CMD			0x0002
#define HDA_C_PA			0xcb
#define HDA_SEQ_ID_BITS			0x00ff0000
#define HDA_GSM_OFFSET_BITS		0x00FFFFFF
#define HDA_GSM_CMD_OFFSET_BITS		0x42C0
#define HDA_GSM_RSP_OFFSET_BITS		0x42E0

#define MBIC_AAP1_ADDR_BASE		0x060000
#define MBIC_IOP_ADDR_BASE		0x070000
#define GSM_ADDR_BASE			0x0700000
#define SPC_TOP_LEVEL_ADDR_BASE		0x000000
#define GSM_CONFIG_RESET_VALUE		0x00003b00
#define GPIO_ADDR_BASE			0x00090000
#define GPIO_GPIO_0_0UTPUT_CTL_OFFSET	0x0000010c

/*            */
#define SPC_RB6_OFFSET			0x80C0
/*                                    */
#define RB6_MAGIC_NUMBER_RST		0x1234

/*                        */
#define DEVREG_SUCCESS					0x00
#define DEVREG_FAILURE_OUT_OF_RESOURCE			0x01
#define DEVREG_FAILURE_DEVICE_ALREADY_REGISTERED	0x02
#define DEVREG_FAILURE_INVALID_PHY_ID			0x03
#define DEVREG_FAILURE_PHY_ID_ALREADY_REGISTERED	0x04
#define DEVREG_FAILURE_PORT_ID_OUT_OF_RANGE		0x05
#define DEVREG_FAILURE_PORT_NOT_VALID_STATE		0x06
#define DEVREG_FAILURE_DEVICE_TYPE_NOT_VALID		0x07

#endif
