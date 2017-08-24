#ifndef _ASM_S390_CLP_H
#define _ASM_S390_CLP_H

/*                                          */
#define CLP_BLK_SIZE			PAGE_SIZE

struct clp_req_hdr {
	u16 len;
	u16 cmd;
} __packed;

struct clp_rsp_hdr {
	u16 len;
	u16 rsp;
} __packed;

/*                    */
#define CLP_RC_OK			0x0010	/*                              */
#define CLP_RC_CMD			0x0020	/*                             */
#define CLP_RC_PERM			0x0030	/*                        */
#define CLP_RC_FMT			0x0040	/*                                */
#define CLP_RC_LEN			0x0050	/*                                */
#define CLP_RC_8K			0x0060	/*                          */
#define CLP_RC_RESNOT0			0x0070	/*                         */
#define CLP_RC_NODATA			0x0080	/*                   */
#define CLP_RC_FC_UNKNOWN		0x0100	/*                              */

#endif
