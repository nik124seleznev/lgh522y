#ifndef _ASM_S390_PCI_CLP_H
#define _ASM_S390_PCI_CLP_H

#include <asm/clp.h>

/*
                                         
 */
#define CLP_LIST_PCI		0x0002
#define CLP_QUERY_PCI_FN	0x0003
#define CLP_QUERY_PCI_FNGRP	0x0004
#define CLP_SET_PCI_FN		0x0005

/*                                */
struct clp_fh_list_entry {
	u16 device_id;
	u16 vendor_id;
	u32 config_state :  1;
	u32		 : 31;
	u32 fid;		/*                 */
	u32 fh;			/*                     */
} __packed;

#define CLP_RC_SETPCIFN_FH	0x0101	/*                       */
#define CLP_RC_SETPCIFN_FHOP	0x0102	/*                            */
#define CLP_RC_SETPCIFN_DMAAS	0x0103	/*                        */
#define CLP_RC_SETPCIFN_RES	0x0104	/*                        */
#define CLP_RC_SETPCIFN_ALRDY	0x0105	/*                               */
#define CLP_RC_SETPCIFN_ERR	0x0106	/*                             */
#define CLP_RC_SETPCIFN_RECPND	0x0107	/*                        */
#define CLP_RC_SETPCIFN_BUSY	0x0108	/*         */
#define CLP_RC_LISTPCI_BADRT	0x010a	/*                             */
#define CLP_RC_QUERYPCIFG_PFGID	0x010b	/*                    */

/*                                         */
#define LIST_PCI_HDR_LEN	32

/*                                                      */
#define CLP_FH_LIST_NR_ENTRIES				\
	((CLP_BLK_SIZE - 2 * LIST_PCI_HDR_LEN)		\
		/ sizeof(struct clp_fh_list_entry))

#define CLP_SET_ENABLE_PCI_FN	0	/*                   */
#define CLP_SET_DISABLE_PCI_FN	1	/*                    */

#define CLP_UTIL_STR_LEN	64

/*                            */
struct clp_req_list_pci {
	struct clp_req_hdr hdr;
	u32 fmt			:  4;	/*                          */
	u32			: 28;
	u64 reserved1;
	u64 resume_token;
	u64 reserved2;
} __packed;

/*                             */
struct clp_rsp_list_pci {
	struct clp_rsp_hdr hdr;
	u32 fmt			:  4;	/*                          */
	u32			: 28;
	u64 reserved1;
	u64 resume_token;
	u32 reserved2;
	u16 max_fn;
	u8 reserved3;
	u8 entry_size;
	struct clp_fh_list_entry fh_list[CLP_FH_LIST_NR_ENTRIES];
} __packed;

/*                            */
struct clp_req_query_pci {
	struct clp_req_hdr hdr;
	u32 fmt			:  4;	/*                          */
	u32			: 28;
	u64 reserved1;
	u32 fh;				/*                 */
	u32 reserved2;
	u64 reserved3;
} __packed;

/*                             */
struct clp_rsp_query_pci {
	struct clp_rsp_hdr hdr;
	u32 fmt			:  4;	/*                          */
	u32			: 28;
	u64 reserved1;
	u16 vfn;			/*                   */
	u16			:  7;
	u16 util_str_avail	:  1;	/*                           */
	u16 pfgid		:  8;	/*                       */
	u32 fid;			/*                 */
	u8 bar_size[PCI_BAR_COUNT];
	u16 pchid;
	u32 bar[PCI_BAR_COUNT];
	u64 reserved2;
	u64 sdma;			/*              */
	u64 edma;			/*            */
	u64 reserved3[6];
	u8 util_str[CLP_UTIL_STR_LEN];	/*                */
} __packed;

/*                                  */
struct clp_req_query_pci_grp {
	struct clp_req_hdr hdr;
	u32 fmt			:  4;	/*                          */
	u32			: 28;
	u64 reserved1;
	u32			: 24;
	u32 pfgid		:  8;	/*                   */
	u32 reserved2;
	u64 reserved3;
} __packed;

/*                                   */
struct clp_rsp_query_pci_grp {
	struct clp_rsp_hdr hdr;
	u32 fmt			:  4;	/*                          */
	u32			: 28;
	u64 reserved1;
	u16			:  4;
	u16 noi			: 12;	/*                      */
	u8 version;
	u8			:  6;
	u8 frame		:  1;
	u8 refresh		:  1;	/*                  */
	u16 reserved2;
	u16 mui;
	u64 reserved3;
	u64 dasm;			/*                        */
	u64 msia;			/*             */
	u64 reserved4;
	u64 reserved5;
} __packed;

/*                          */
struct clp_req_set_pci {
	struct clp_req_hdr hdr;
	u32 fmt			:  4;	/*                          */
	u32			: 28;
	u64 reserved1;
	u32 fh;				/*                 */
	u16 reserved2;
	u8 oc;				/*                    */
	u8 ndas;			/*                      */
	u64 reserved3;
} __packed;

/*                           */
struct clp_rsp_set_pci {
	struct clp_rsp_hdr hdr;
	u32 fmt			:  4;	/*                          */
	u32			: 28;
	u64 reserved1;
	u32 fh;				/*                 */
	u32 reserved3;
	u64 reserved4;
} __packed;

/*                                                             */
struct clp_req_rsp_list_pci {
	struct clp_req_list_pci request;
	struct clp_rsp_list_pci response;
} __packed;

struct clp_req_rsp_set_pci {
	struct clp_req_set_pci request;
	struct clp_rsp_set_pci response;
} __packed;

struct clp_req_rsp_query_pci {
	struct clp_req_query_pci request;
	struct clp_rsp_query_pci response;
} __packed;

struct clp_req_rsp_query_pci_grp {
	struct clp_req_query_pci_grp request;
	struct clp_rsp_query_pci_grp response;
} __packed;

#endif
