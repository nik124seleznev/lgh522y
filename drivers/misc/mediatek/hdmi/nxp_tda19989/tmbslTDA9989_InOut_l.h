/**
 * Copyright (C) 2009 Koninklijke Philips Electronics N.V., All Rights Reserved.
 * This source code and any compilation or derivative thereof is the proprietary
 * information of Koninklijke Philips Electronics N.V. and is confidential in
 * nature. Under no circumstances is this software to be  exposed to or placed
 * under an Open Source License of any type without the expressed written
 * permission of Koninklijke Philips Electronics N.V.
 *
 * \file          tmbslTDA9989_InOut_l.h
 *
 * \version       $Revision: 2 $
 *
 *
*/

#ifndef TMBSLTDA9989_INOUT_L_H
#define TMBSLTDA9989_INOUT_L_H

/*                                                                            */
/*                                                                            */
/*                                                                            */

#ifdef __cplusplus
extern "C" {
#endif

/*                                                                            */
/*                                                                            */
/*                                                                            */

#define IS_TV(fmt) (fmt >= HDMITX_VFMT_TV_MIN && fmt <= HDMITX_VFMT_TV_MAX)
#define IS_VALID_FMT(fmt) IS_TV(fmt)
#ifdef FORMAT_PC
#define IS_PC(fmt) (fmt >= HDMITX_VFMT_PC_MIN && fmt <= HDMITX_VFMT_PC_MAX)
#define IS_VALID_FMT(fmt) (IS_TV(fmt) || IS_PC(fmt))
#endif
#define VIC2REG_LOOP(array, idx) do {                           \
   hash = (struct vic2reg *)(array);                         \
   for (i = 0; i < (sizeof(array)/sizeof(struct vic2reg)); i++) { \
      if (hash[i].vic == fmt) {                              \
         (*idx) = hash[i].reg;                               \
	 break;                                            \
      }                                                    \
   }                                                       \
} while (0);

/*                                                                            */
/*                                                                            */
/*                                                                            */

	typedef struct {
		UInt16 Register;
		UInt8 MaskSwap;
		UInt8 MaskMirror;
	} tmbslTDA9989RegVip;

/*                                                                            */
/*                                                                            */
/*                                                                            */

	extern tmHdmiTxRegMaskVal_t kCommonPllCfg[];

/* 
                                                                        
 */
	extern tmHdmiTxRegMaskVal_t kVfmt480i576iPllCfg[];

/* 
                                                                           
                           
 */
	extern tmHdmiTxRegMaskVal_t kSinglePrateVfmt480i576iPllCfg[];

/* 
                                                                                    
                           
 */
	extern tmHdmiTxRegMaskVal_t kSrepeatedPrateVfmt480i576iPllCfg[];

/* 
                                                                                   
 */
	extern tmHdmiTxRegMaskVal_t kVfmtOtherPllCfg[];

/* 
                                                                       
                                 
 */
	extern tmHdmiTxRegMaskVal_t kSinglePrateVfmtOtherPllCfg[];

/* 
                                                                       
                                 
 */
	extern tmHdmiTxRegMaskVal_t kDoublePrateVfmtOtherPllCfg[];


/*                                                                            */
/*                                                                            */
/*                                                                            */
	extern tmbslHdmiTxVidFmt_t calculateVidFmtIndex(tmbslHdmiTxVidFmt_t vidFmt);


#ifdef __cplusplus
}
#endif
#endif				/*                        */
/*                                                                            *//*                                                                            *//*                                                                            */
