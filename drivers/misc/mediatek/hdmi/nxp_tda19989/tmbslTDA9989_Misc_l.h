/**
 * Copyright (C) 2009 Koninklijke Philips Electronics N.V., All Rights Reserved.
 * This source code and any compilation or derivative thereof is the proprietary
 * information of Koninklijke Philips Electronics N.V. and is confidential in
 * nature. Under no circumstances is this software to be  exposed to or placed
 * under an Open Source License of any type without the expressed written
 * permission of Koninklijke Philips Electronics N.V.
 *
 * \file          tmbslTDA9989_Misc_l.h
 *
 * \version       $Revision: 2 $
 *
*/

#ifndef TMBSLTDA9989_MISC_L_H
#define TMBSLTDA9989_MISC_L_H

/*                                                                            */
/*                                                                            */
/*                                                                            */

#ifdef __cplusplus
extern "C" {
#endif

/*                                                                            */
/*                                                                            */
/*                                                                            */


/*                                                                            */
/*                                                                            */
/*                                                                            */

/*                                                                            */
/*                                                                            */
/*                                                                            */

#ifdef TMFL_TDA9989_PIXEL_CLOCK_ON_DDC
	extern CONST_DAT UInt8 kndiv_im[];
	extern CONST_DAT UInt8 kclk_div[];
#endif				/*                                 */


/*                                                                            */
/*                                                                            */
/*                                                                            */

	tmErrorCode_t hotPlugRestore(tmUnitSelect_t txUnit);

#ifdef TMFL_TDA9989_PIXEL_CLOCK_ON_DDC
	tmErrorCode_t hotPlugRestore(tmUnitSelect_t txUnit);
#endif				/*                                 */

#ifdef __cplusplus
}
#endif
#endif				/*                       */
/*                                                                            *//*                                                                            *//*                                                                            */
