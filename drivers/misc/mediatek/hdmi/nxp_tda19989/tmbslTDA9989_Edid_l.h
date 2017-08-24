/**
 * Copyright (C) 2009 Koninklijke Philips Electronics N.V., All Rights Reserved.
 * This source code and any compilation or derivative thereof is the proprietary
 * information of Koninklijke Philips Electronics N.V. and is confidential in
 * nature. Under no circumstances is this software to be  exposed to or placed
 * under an Open Source License of any type without the expressed written
 * permission of Koninklijke Philips Electronics N.V.
 *
 * \file          tmbslTDA9989_Edid_l.h
 *
 * \version       $Revision: 2 $
 *
 * \date          $Date: 04/07/07 17:00 $
 *
 *
*/

#ifndef TMBSLTDA9989_EDID_L_H
#define TMBSLTDA9989_EDID_L_H

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

	extern tmErrorCode_t EdidBlockAvailable(tmUnitSelect_t txUnit, Bool * pSendEDIDCallback);

	extern tmErrorCode_t ClearEdidRequest(tmUnitSelect_t txUnit);

/*                                                                            */
/*                                                                            */
/*                                                                            */



#ifdef __cplusplus
}
#endif
#endif				/*                       */
/*                                                                            *//*                                                                            *//*                                                                            */
