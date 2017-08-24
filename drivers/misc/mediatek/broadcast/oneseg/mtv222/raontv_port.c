/********************************************************************************
* (c) COPYRIGHT 2010 RAONTECH, Inc. ALL RIGHTS RESERVED.
*
* This software is the property of RAONTECH and is furnished under license by RAONTECH.
* This software may be used only in accordance with the terms of said license.
* This copyright noitce may not be remoced, modified or obliterated without the prior
* written permission of RAONTECH, Inc.
*
* This software may not be copied, transmitted, provided to or otherwise made available
* to any other person, company, corporation or other entity except as specified in the
* terms of said license.
*
* No right, title, ownership or other interest in the software is hereby granted or transferred.
*
* The information contained herein is subject to change without notice and should
* not be construed as a commitment by RAONTECH, Inc.
*
* TITLE 	  : RAONTECH TV OEM source file.
*
* FILENAME    : raontv_port.c
*
* DESCRIPTION :
*		User-supplied Routines for RAONTECH TV Services.
*
********************************************************************************/

/*                                                                               
                  
 
                              
                                                                                 
                                       
                                                                               */

#include "raontv.h"
#include "raontv_internal.h"

/*                                                   */
#if defined(RTV_IF_SPI) || defined(RTV_IF_EBI2)
	#if defined(__KERNEL__)
	struct mutex raontv_guard;
	#elif defined(WINCE)
        CRITICAL_SECTION raontv_guard;
	#else
	/*                  */
	#endif
#endif


void rtvOEM_ConfigureInterrupt(void)
{
#if defined(RTV_IF_SPI) || defined(RTV_IF_EBI2)
	RTV_REG_MAP_SEL(SPI_CTRL_PAGE);
	RTV_REG_SET(0x21, SPI_INTR_POL_ACTIVE|0x02);

#ifdef RTV_IF_SPI
	RTV_REG_SET(0x27, 0x01); /*                                */
#endif

	RTV_REG_SET(0x2B, RTV_SPI_INTR_DEACT_PRD_VAL);

	RTV_REG_SET(0x2A, 1); /*           */
	RTV_REG_SET(0x2A, 0);
#endif
}

void rtvOEM_PowerOn(int on)
{
	if (on)	{
		/*                                     */
//                                

		RTV_GUARD_INIT;
	} else {
		/*                                    */
	//                               

		RTV_GUARD_DEINIT;
	}
}




