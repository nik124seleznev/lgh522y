/*                                                                             
  
                                                                             
  
                                                                             */

/*
 * Copyright (C) 2000 - 2013, Intel Corp.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions, and the following disclaimer,
 *    without modification.
 * 2. Redistributions in binary form must reproduce at minimum a disclaimer
 *    substantially similar to the "NO WARRANTY" disclaimer below
 *    ("Disclaimer") and any redistribution must be conditioned upon
 *    including a substantially similar Disclaimer requirement for further
 *    binary redistribution.
 * 3. Neither the names of the above-listed copyright holders nor the names
 *    of any contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
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
 */

#ifndef __ACPREDEF_H__
#define __ACPREDEF_H__

/*                                                                             
  
                       
  
                                                  
  
                                                               
                   
             
                   
             
  
                                                                           
                                 
  
                                                                           
                       
  
  
                                                                            
                                                                               
  
                                                                         
                                  
                   
             
                   
             
                                           
  
                                                                   
                                              
                   
                                 
  
                                                                             
                   
             
                   
             
                       
  
                                                                     
                                  
                       
  
                                                                             
                                              
                       
  
                                                                            
                             
  
                                                                              
                                                                               
                   
             
                   
                            
                       
  
                                                                             */

enum acpi_return_package_types {
	ACPI_PTYPE1_FIXED = 1,
	ACPI_PTYPE1_VAR = 2,
	ACPI_PTYPE1_OPTION = 3,
	ACPI_PTYPE2 = 4,
	ACPI_PTYPE2_COUNT = 5,
	ACPI_PTYPE2_PKG_COUNT = 6,
	ACPI_PTYPE2_FIXED = 7,
	ACPI_PTYPE2_MIN = 8,
	ACPI_PTYPE2_REV_FIXED = 9,
	ACPI_PTYPE2_FIX_VAR = 10
};

/*                                                       */

#define METHOD_PREDEF_ARGS_MAX          4
#define METHOD_ARG_BIT_WIDTH            3
#define METHOD_ARG_MASK                 0x0007
#define ARG_COUNT_IS_MINIMUM            0x8000
#define METHOD_MAX_ARG_TYPE             ACPI_TYPE_PACKAGE

#define METHOD_GET_COUNT(arg_list)      (arg_list & METHOD_ARG_MASK)
#define METHOD_GET_NEXT_ARG(arg_list)   (arg_list >> METHOD_ARG_BIT_WIDTH)

/*                                                */

#define METHOD_0ARGS                    0
#define METHOD_1ARGS(a1)                (1 | (a1 << 3))
#define METHOD_2ARGS(a1,a2)             (2 | (a1 << 3) | (a2 << 6))
#define METHOD_3ARGS(a1,a2,a3)          (3 | (a1 << 3) | (a2 << 6) | (a3 << 9))
#define METHOD_4ARGS(a1,a2,a3,a4)       (4 | (a1 << 3) | (a2 << 6) | (a3 << 9) | (a4 << 12))

#define METHOD_RETURNS(type)            (type)
#define METHOD_NO_RETURN_VALUE          0

#define PACKAGE_INFO(a,b,c,d,e,f)       {{{(a),(b),(c),(d)}, ((((u16)(f)) << 8) | (e)), 0}}

/*                                                       */

#define WIDTH_1                         0x0001
#define WIDTH_2                         0x0002
#define WIDTH_3                         0x0004
#define WIDTH_8                         0x0008
#define WIDTH_16                        0x0010
#define WIDTH_32                        0x0020
#define WIDTH_64                        0x0040
#define VARIABLE_DATA                   0x0080
#define NUM_RESOURCE_WIDTHS             8

#define WIDTH_ADDRESS                   WIDTH_16 | WIDTH_32 | WIDTH_64

#ifdef ACPI_CREATE_PREDEFINED_TABLE
/*                                                                             
  
                                              
  
                                                                               
                                               
  
                                                                 
                                
                                         
                               
                                                 
                                
  
                                                                          
                                                          
  
                                                               
                    
  
                                                                  
  
                                                
                                                                       
                                                                            
                                                                          
                                                                               
                                                               
                                                                   
  
                                                                         
                                                                            
                                                                           
                                   
  
                                                                   
  
                                                                            
                             
  
                                                                             
                                                   
  
                                                                             */

const union acpi_predefined_info acpi_gbl_predefined_methods[] = {
	{{"_AC0", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_AC1", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_AC2", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_AC3", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_AC4", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_AC5", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_AC6", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_AC7", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_AC8", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_AC9", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_ADR", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_AEI", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_BUFFER)}},

	{{"_AL0", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                        */
	PACKAGE_INFO(ACPI_PTYPE1_VAR, ACPI_RTYPE_REFERENCE, 0, 0, 0, 0),

	{{"_AL1", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                        */
	PACKAGE_INFO(ACPI_PTYPE1_VAR, ACPI_RTYPE_REFERENCE, 0, 0, 0, 0),

	{{"_AL2", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                        */
	PACKAGE_INFO(ACPI_PTYPE1_VAR, ACPI_RTYPE_REFERENCE, 0, 0, 0, 0),

	{{"_AL3", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                        */
	PACKAGE_INFO(ACPI_PTYPE1_VAR, ACPI_RTYPE_REFERENCE, 0, 0, 0, 0),

	{{"_AL4", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                        */
	PACKAGE_INFO(ACPI_PTYPE1_VAR, ACPI_RTYPE_REFERENCE, 0, 0, 0, 0),

	{{"_AL5", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                        */
	PACKAGE_INFO(ACPI_PTYPE1_VAR, ACPI_RTYPE_REFERENCE, 0, 0, 0, 0),

	{{"_AL6", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                        */
	PACKAGE_INFO(ACPI_PTYPE1_VAR, ACPI_RTYPE_REFERENCE, 0, 0, 0, 0),

	{{"_AL7", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                        */
	PACKAGE_INFO(ACPI_PTYPE1_VAR, ACPI_RTYPE_REFERENCE, 0, 0, 0, 0),

	{{"_AL8", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                        */
	PACKAGE_INFO(ACPI_PTYPE1_VAR, ACPI_RTYPE_REFERENCE, 0, 0, 0, 0),

	{{"_AL9", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                        */
	PACKAGE_INFO(ACPI_PTYPE1_VAR, ACPI_RTYPE_REFERENCE, 0, 0, 0, 0),

	{{"_ALC", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_ALI", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_ALP", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_ALR", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                                      */
	PACKAGE_INFO(ACPI_PTYPE2, ACPI_RTYPE_INTEGER, 2, 0, 0, 0),

	{{"_ALT", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_ART", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                                                   */
	PACKAGE_INFO(ACPI_PTYPE2_REV_FIXED, ACPI_RTYPE_REFERENCE, 2,
		     ACPI_RTYPE_INTEGER, 11, 0),

	{{"_BBN", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_BCL", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                        */
	PACKAGE_INFO(ACPI_PTYPE1_VAR, ACPI_RTYPE_INTEGER, 0, 0, 0, 0),

	{{"_BCM", METHOD_1ARGS(ACPI_TYPE_INTEGER),
	  METHOD_NO_RETURN_VALUE}},

	{{"_BCT", METHOD_1ARGS(ACPI_TYPE_INTEGER),
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_BDN", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_BFS", METHOD_1ARGS(ACPI_TYPE_INTEGER),
	  METHOD_NO_RETURN_VALUE}},

	{{"_BIF", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                              */
	PACKAGE_INFO(ACPI_PTYPE1_FIXED, ACPI_RTYPE_INTEGER, 9,
		     ACPI_RTYPE_STRING, 4, 0),

	{{"_BIX", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                               */
	PACKAGE_INFO(ACPI_PTYPE1_FIXED, ACPI_RTYPE_INTEGER, 16,
		     ACPI_RTYPE_STRING, 4, 0),

	{{"_BLT",
	  METHOD_3ARGS(ACPI_TYPE_INTEGER, ACPI_TYPE_INTEGER, ACPI_TYPE_INTEGER),
	  METHOD_NO_RETURN_VALUE}},

	{{"_BMA", METHOD_1ARGS(ACPI_TYPE_INTEGER),
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_BMC", METHOD_1ARGS(ACPI_TYPE_INTEGER),
	  METHOD_NO_RETURN_VALUE}},

	{{"_BMD", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                      */
	PACKAGE_INFO(ACPI_PTYPE1_FIXED, ACPI_RTYPE_INTEGER, 5, 0, 0, 0),

	{{"_BMS", METHOD_1ARGS(ACPI_TYPE_INTEGER),
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_BQC", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_BST", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                      */
	PACKAGE_INFO(ACPI_PTYPE1_FIXED, ACPI_RTYPE_INTEGER, 4, 0, 0, 0),

	{{"_BTM", METHOD_1ARGS(ACPI_TYPE_INTEGER),
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_BTP", METHOD_1ARGS(ACPI_TYPE_INTEGER),
	  METHOD_NO_RETURN_VALUE}},

	{{"_CBA", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},	/*                           */

	{{"_CDM", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_CID", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER | ACPI_RTYPE_STRING | ACPI_RTYPE_PACKAGE)}},	/*                             */
	PACKAGE_INFO(ACPI_PTYPE1_VAR, ACPI_RTYPE_INTEGER | ACPI_RTYPE_STRING, 0,
		     0, 0, 0),

	{{"_CLS", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                      */
	PACKAGE_INFO(ACPI_PTYPE1_FIXED, ACPI_RTYPE_INTEGER, 3, 0, 0, 0),

	{{"_CPC", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                             */
	PACKAGE_INFO(ACPI_PTYPE1_VAR, ACPI_RTYPE_INTEGER | ACPI_RTYPE_BUFFER, 0,
		     0, 0, 0),

	{{"_CRS", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_BUFFER)}},

	{{"_CRT", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_CSD", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                                     */
	PACKAGE_INFO(ACPI_PTYPE2_COUNT, ACPI_RTYPE_INTEGER, 0, 0, 0, 0),

	{{"_CST", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                                                */
	PACKAGE_INFO(ACPI_PTYPE2_PKG_COUNT, ACPI_RTYPE_BUFFER, 1,
		     ACPI_RTYPE_INTEGER, 3, 0),

	{{"_CWS", METHOD_1ARGS(ACPI_TYPE_INTEGER),
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_DCK", METHOD_1ARGS(ACPI_TYPE_INTEGER),
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_DCS", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_DDC", METHOD_1ARGS(ACPI_TYPE_INTEGER),
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER | ACPI_RTYPE_BUFFER)}},

	{{"_DDN", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_STRING)}},

	{{"_DEP", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                        */
	PACKAGE_INFO(ACPI_PTYPE1_VAR, ACPI_RTYPE_REFERENCE, 0, 0, 0, 0),

	{{"_DGS", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_DIS", METHOD_0ARGS,
	  METHOD_NO_RETURN_VALUE}},

	{{"_DLM", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                                                           */
	PACKAGE_INFO(ACPI_PTYPE2_FIX_VAR, ACPI_RTYPE_REFERENCE, 1,
		     ACPI_RTYPE_REFERENCE | ACPI_RTYPE_BUFFER, 0, 0),

	{{"_DMA", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_BUFFER)}},

	{{"_DOD", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                        */
	PACKAGE_INFO(ACPI_PTYPE1_VAR, ACPI_RTYPE_INTEGER, 0, 0, 0, 0),

	{{"_DOS", METHOD_1ARGS(ACPI_TYPE_INTEGER),
	  METHOD_NO_RETURN_VALUE}},

	{{"_DSM",
	  METHOD_4ARGS(ACPI_TYPE_BUFFER, ACPI_TYPE_INTEGER, ACPI_TYPE_INTEGER,
		       ACPI_TYPE_PACKAGE),
	  METHOD_RETURNS(ACPI_RTYPE_ALL)}},	/*                                                */

	{{"_DSS", METHOD_1ARGS(ACPI_TYPE_INTEGER),
	  METHOD_NO_RETURN_VALUE}},

	{{"_DSW",
	  METHOD_3ARGS(ACPI_TYPE_INTEGER, ACPI_TYPE_INTEGER, ACPI_TYPE_INTEGER),
	  METHOD_NO_RETURN_VALUE}},

	{{"_DTI", METHOD_1ARGS(ACPI_TYPE_INTEGER),
	  METHOD_NO_RETURN_VALUE}},

	{{"_EC_", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_EDL", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                        */
	PACKAGE_INFO(ACPI_PTYPE1_VAR, ACPI_RTYPE_REFERENCE, 0, 0, 0, 0),

	{{"_EJ0", METHOD_1ARGS(ACPI_TYPE_INTEGER),
	  METHOD_NO_RETURN_VALUE}},

	{{"_EJ1", METHOD_1ARGS(ACPI_TYPE_INTEGER),
	  METHOD_NO_RETURN_VALUE}},

	{{"_EJ2", METHOD_1ARGS(ACPI_TYPE_INTEGER),
	  METHOD_NO_RETURN_VALUE}},

	{{"_EJ3", METHOD_1ARGS(ACPI_TYPE_INTEGER),
	  METHOD_NO_RETURN_VALUE}},

	{{"_EJ4", METHOD_1ARGS(ACPI_TYPE_INTEGER),
	  METHOD_NO_RETURN_VALUE}},

	{{"_EJD", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_STRING)}},

	{{"_ERR",
	  METHOD_3ARGS(ACPI_TYPE_INTEGER, ACPI_TYPE_STRING, ACPI_TYPE_INTEGER),
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},	/*                                               */

	{{"_EVT", METHOD_1ARGS(ACPI_TYPE_INTEGER),
	  METHOD_NO_RETURN_VALUE}},

	{{"_FDE", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_BUFFER)}},

	{{"_FDI", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                       */
	PACKAGE_INFO(ACPI_PTYPE1_FIXED, ACPI_RTYPE_INTEGER, 16, 0, 0, 0),

	{{"_FDM", METHOD_1ARGS(ACPI_TYPE_INTEGER),
	  METHOD_NO_RETURN_VALUE}},

	{{"_FIF", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                      */
	PACKAGE_INFO(ACPI_PTYPE1_FIXED, ACPI_RTYPE_INTEGER, 4, 0, 0, 0),

	{{"_FIX", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                        */
	PACKAGE_INFO(ACPI_PTYPE1_VAR, ACPI_RTYPE_INTEGER, 0, 0, 0, 0),

	{{"_FPS", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                                            */
	PACKAGE_INFO(ACPI_PTYPE2_REV_FIXED, ACPI_RTYPE_INTEGER, 5, 0, 0, 0),

	{{"_FSL", METHOD_1ARGS(ACPI_TYPE_INTEGER),
	  METHOD_NO_RETURN_VALUE}},

	{{"_FST", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                      */
	PACKAGE_INFO(ACPI_PTYPE1_FIXED, ACPI_RTYPE_INTEGER, 3, 0, 0, 0),

	{{"_GAI", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_GCP", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_GHL", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_GLK", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_GPD", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_GPE", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},	/*                             */

	{{"_GRT", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_BUFFER)}},

	{{"_GSB", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_GTF", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_BUFFER)}},

	{{"_GTM", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_BUFFER)}},

	{{"_GTS", METHOD_1ARGS(ACPI_TYPE_INTEGER),
	  METHOD_NO_RETURN_VALUE}},

	{{"_GWS", METHOD_1ARGS(ACPI_TYPE_INTEGER),
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_HID", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER | ACPI_RTYPE_STRING)}},

	{{"_HOT", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_HPP", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                      */
	PACKAGE_INFO(ACPI_PTYPE1_FIXED, ACPI_RTYPE_INTEGER, 4, 0, 0, 0),

	/*
                                                                               
                                                                    
                                                                     
                                                           
  */
	{{"_HPX", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                                        */
	PACKAGE_INFO(ACPI_PTYPE2_MIN, ACPI_RTYPE_INTEGER, 5, 0, 0, 0),

	{{"_HRV", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_IFT", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},	/*               */

	{{"_INI", METHOD_0ARGS,
	  METHOD_NO_RETURN_VALUE}},

	{{"_IRC", METHOD_0ARGS,
	  METHOD_NO_RETURN_VALUE}},

	{{"_LCK", METHOD_1ARGS(ACPI_TYPE_INTEGER),
	  METHOD_NO_RETURN_VALUE}},

	{{"_LID", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_MAT", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_BUFFER)}},

	{{"_MBM", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                      */
	PACKAGE_INFO(ACPI_PTYPE1_FIXED, ACPI_RTYPE_INTEGER, 8, 0, 0, 0),

	{{"_MLS", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                                           */
	PACKAGE_INFO(ACPI_PTYPE2, ACPI_RTYPE_STRING, 1, ACPI_RTYPE_BUFFER, 1,
		     0),

	{{"_MSG", METHOD_1ARGS(ACPI_TYPE_INTEGER),
	  METHOD_NO_RETURN_VALUE}},

	{{"_MSM",
	  METHOD_4ARGS(ACPI_TYPE_INTEGER, ACPI_TYPE_INTEGER, ACPI_TYPE_INTEGER,
		       ACPI_TYPE_INTEGER),
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_NTT", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_OFF", METHOD_0ARGS,
	  METHOD_NO_RETURN_VALUE}},

	{{"_ON_", METHOD_0ARGS,
	  METHOD_NO_RETURN_VALUE}},

	{{"_OS_", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_STRING)}},

	{{"_OSC",
	  METHOD_4ARGS(ACPI_TYPE_BUFFER, ACPI_TYPE_INTEGER, ACPI_TYPE_INTEGER,
		       ACPI_TYPE_BUFFER),
	  METHOD_RETURNS(ACPI_RTYPE_BUFFER)}},

	{{"_OST",
	  METHOD_3ARGS(ACPI_TYPE_INTEGER, ACPI_TYPE_INTEGER, ACPI_TYPE_BUFFER),
	  METHOD_NO_RETURN_VALUE}},

	{{"_PAI", METHOD_1ARGS(ACPI_TYPE_INTEGER),
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_PCL", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                        */
	PACKAGE_INFO(ACPI_PTYPE1_VAR, ACPI_RTYPE_REFERENCE, 0, 0, 0, 0),

	{{"_PCT", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                      */
	PACKAGE_INFO(ACPI_PTYPE1_FIXED, ACPI_RTYPE_BUFFER, 2, 0, 0, 0),

	{{"_PDC", METHOD_1ARGS(ACPI_TYPE_BUFFER),
	  METHOD_NO_RETURN_VALUE}},

	{{"_PDL", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_PIC", METHOD_1ARGS(ACPI_TYPE_INTEGER),
	  METHOD_NO_RETURN_VALUE}},

	{{"_PIF", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                              */
	PACKAGE_INFO(ACPI_PTYPE1_FIXED, ACPI_RTYPE_INTEGER, 3,
		     ACPI_RTYPE_STRING, 3, 0),

	{{"_PLD", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                        */
	PACKAGE_INFO(ACPI_PTYPE1_VAR, ACPI_RTYPE_BUFFER, 0, 0, 0, 0),

	{{"_PMC", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                               */
	PACKAGE_INFO(ACPI_PTYPE1_FIXED, ACPI_RTYPE_INTEGER, 11,
		     ACPI_RTYPE_STRING, 3, 0),

	{{"_PMD", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                        */
	PACKAGE_INFO(ACPI_PTYPE1_VAR, ACPI_RTYPE_REFERENCE, 0, 0, 0, 0),

	{{"_PMM", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_PPC", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_PPE", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},	/*                */

	{{"_PR0", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                        */
	PACKAGE_INFO(ACPI_PTYPE1_VAR, ACPI_RTYPE_REFERENCE, 0, 0, 0, 0),

	{{"_PR1", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                        */
	PACKAGE_INFO(ACPI_PTYPE1_VAR, ACPI_RTYPE_REFERENCE, 0, 0, 0, 0),

	{{"_PR2", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                        */
	PACKAGE_INFO(ACPI_PTYPE1_VAR, ACPI_RTYPE_REFERENCE, 0, 0, 0, 0),

	{{"_PR3", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                        */
	PACKAGE_INFO(ACPI_PTYPE1_VAR, ACPI_RTYPE_REFERENCE, 0, 0, 0, 0),

	{{"_PRE", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                        */
	PACKAGE_INFO(ACPI_PTYPE1_VAR, ACPI_RTYPE_REFERENCE, 0, 0, 0, 0),

	{{"_PRL", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                        */
	PACKAGE_INFO(ACPI_PTYPE1_VAR, ACPI_RTYPE_REFERENCE, 0, 0, 0, 0),

	{{"_PRS", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_BUFFER)}},

	/*
                                                                         
                                                                         
                                                                         
                                                                     
                                                                           
                           
  */
	{{"_PRT", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                                                      */
	PACKAGE_INFO(ACPI_PTYPE2_FIXED, 4, ACPI_RTYPE_INTEGER,
		     ACPI_RTYPE_INTEGER,
		     ACPI_RTYPE_INTEGER | ACPI_RTYPE_REFERENCE,
		     ACPI_RTYPE_INTEGER),

	{{"_PRW", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                                                                                  */
	PACKAGE_INFO(ACPI_PTYPE1_OPTION, 2,
		     ACPI_RTYPE_INTEGER | ACPI_RTYPE_PACKAGE,
		     ACPI_RTYPE_INTEGER, ACPI_RTYPE_REFERENCE, 0),

	{{"_PS0", METHOD_0ARGS,
	  METHOD_NO_RETURN_VALUE}},

	{{"_PS1", METHOD_0ARGS,
	  METHOD_NO_RETURN_VALUE}},

	{{"_PS2", METHOD_0ARGS,
	  METHOD_NO_RETURN_VALUE}},

	{{"_PS3", METHOD_0ARGS,
	  METHOD_NO_RETURN_VALUE}},

	{{"_PSC", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_PSD", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                                                */
	PACKAGE_INFO(ACPI_PTYPE2_COUNT, ACPI_RTYPE_INTEGER, 0, 0, 0, 0),

	{{"_PSE", METHOD_1ARGS(ACPI_TYPE_INTEGER),
	  METHOD_NO_RETURN_VALUE}},

	{{"_PSL", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                        */
	PACKAGE_INFO(ACPI_PTYPE1_VAR, ACPI_RTYPE_REFERENCE, 0, 0, 0, 0),

	{{"_PSR", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_PSS", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                                     */
	PACKAGE_INFO(ACPI_PTYPE2, ACPI_RTYPE_INTEGER, 6, 0, 0, 0),

	{{"_PSV", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_PSW", METHOD_1ARGS(ACPI_TYPE_INTEGER),
	  METHOD_NO_RETURN_VALUE}},

	{{"_PTC", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                      */
	PACKAGE_INFO(ACPI_PTYPE1_FIXED, ACPI_RTYPE_BUFFER, 2, 0, 0, 0),

	{{"_PTP", METHOD_2ARGS(ACPI_TYPE_INTEGER, ACPI_TYPE_INTEGER),
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_PTS", METHOD_1ARGS(ACPI_TYPE_INTEGER),
	  METHOD_NO_RETURN_VALUE}},

	{{"_PUR", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                      */
	PACKAGE_INFO(ACPI_PTYPE1_FIXED, ACPI_RTYPE_INTEGER, 2, 0, 0, 0),

	{{"_PXM", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_REG", METHOD_2ARGS(ACPI_TYPE_INTEGER, ACPI_TYPE_INTEGER),
	  METHOD_NO_RETURN_VALUE}},

	{{"_REV", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_RMV", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_ROM", METHOD_2ARGS(ACPI_TYPE_INTEGER, ACPI_TYPE_INTEGER),
	  METHOD_RETURNS(ACPI_RTYPE_BUFFER)}},

	{{"_RTV", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	/*
                                                                 
                                                                            
                                                                               
                     
  */
	{{"_S0_", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                      */
	PACKAGE_INFO(ACPI_PTYPE1_VAR, ACPI_RTYPE_INTEGER, 1, 0, 0, 0),

	{{"_S1_", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                      */
	PACKAGE_INFO(ACPI_PTYPE1_VAR, ACPI_RTYPE_INTEGER, 1, 0, 0, 0),

	{{"_S2_", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                      */
	PACKAGE_INFO(ACPI_PTYPE1_VAR, ACPI_RTYPE_INTEGER, 1, 0, 0, 0),

	{{"_S3_", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                      */
	PACKAGE_INFO(ACPI_PTYPE1_VAR, ACPI_RTYPE_INTEGER, 1, 0, 0, 0),

	{{"_S4_", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                      */
	PACKAGE_INFO(ACPI_PTYPE1_VAR, ACPI_RTYPE_INTEGER, 1, 0, 0, 0),

	{{"_S5_", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                      */
	PACKAGE_INFO(ACPI_PTYPE1_VAR, ACPI_RTYPE_INTEGER, 1, 0, 0, 0),

	{{"_S1D", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_S2D", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_S3D", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_S4D", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_S0W", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_S1W", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_S2W", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_S3W", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_S4W", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_SBS", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_SCP", METHOD_1ARGS(ACPI_TYPE_INTEGER) | ARG_COUNT_IS_MINIMUM,
	  METHOD_NO_RETURN_VALUE}},	/*                                                                          */

	{{"_SDD", METHOD_1ARGS(ACPI_TYPE_BUFFER),
	  METHOD_NO_RETURN_VALUE}},

	{{"_SEG", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_SHL", METHOD_1ARGS(ACPI_TYPE_INTEGER),
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_SLI", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_BUFFER)}},

	{{"_SPD", METHOD_1ARGS(ACPI_TYPE_INTEGER),
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_SRS", METHOD_1ARGS(ACPI_TYPE_BUFFER),
	  METHOD_NO_RETURN_VALUE}},

	{{"_SRT", METHOD_1ARGS(ACPI_TYPE_BUFFER),
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_SRV", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},	/*               */

	{{"_SST", METHOD_1ARGS(ACPI_TYPE_INTEGER),
	  METHOD_NO_RETURN_VALUE}},

	{{"_STA", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_STM",
	  METHOD_3ARGS(ACPI_TYPE_BUFFER, ACPI_TYPE_BUFFER, ACPI_TYPE_BUFFER),
	  METHOD_NO_RETURN_VALUE}},

	{{"_STP", METHOD_2ARGS(ACPI_TYPE_INTEGER, ACPI_TYPE_INTEGER),
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_STR", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_BUFFER)}},

	{{"_STV", METHOD_2ARGS(ACPI_TYPE_INTEGER, ACPI_TYPE_INTEGER),
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_SUB", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_STRING)}},

	{{"_SUN", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_SWS", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_TC1", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_TC2", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_TDL", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_TIP", METHOD_1ARGS(ACPI_TYPE_INTEGER),
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_TIV", METHOD_1ARGS(ACPI_TYPE_INTEGER),
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_TMP", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_TPC", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_TPT", METHOD_1ARGS(ACPI_TYPE_INTEGER),
	  METHOD_NO_RETURN_VALUE}},

	{{"_TRT", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                                         */
	PACKAGE_INFO(ACPI_PTYPE2, ACPI_RTYPE_REFERENCE, 2, ACPI_RTYPE_INTEGER,
		     6, 0),

	{{"_TSD", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                                              */
	PACKAGE_INFO(ACPI_PTYPE2_COUNT, ACPI_RTYPE_INTEGER, 5, 0, 0, 0),

	{{"_TSP", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_TSS", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                                   */
	PACKAGE_INFO(ACPI_PTYPE2, ACPI_RTYPE_INTEGER, 5, 0, 0, 0),

	{{"_TST", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_TTS", METHOD_1ARGS(ACPI_TYPE_INTEGER),
	  METHOD_NO_RETURN_VALUE}},

	{{"_TZD", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                        */
	PACKAGE_INFO(ACPI_PTYPE1_VAR, ACPI_RTYPE_REFERENCE, 0, 0, 0, 0),

	{{"_TZM", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_REFERENCE)}},

	{{"_TZP", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_UID", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER | ACPI_RTYPE_STRING)}},

	{{"_UPC", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_PACKAGE)}},	/*                      */
	PACKAGE_INFO(ACPI_PTYPE1_FIXED, ACPI_RTYPE_INTEGER, 4, 0, 0, 0),

	{{"_UPD", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_UPP", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	{{"_VPO", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER)}},

	/*                                                                                       */

	{{"_WAK", METHOD_1ARGS(ACPI_TYPE_INTEGER),
	  METHOD_RETURNS(ACPI_RTYPE_NONE | ACPI_RTYPE_INTEGER |
			 ACPI_RTYPE_PACKAGE)}},
	PACKAGE_INFO(ACPI_PTYPE1_FIXED, ACPI_RTYPE_INTEGER, 2, 0, 0, 0),	/*                                       */

	/*                                                                  */

	{{"_WDG", METHOD_0ARGS,
	  METHOD_RETURNS(ACPI_RTYPE_BUFFER)}},

	{{"_WED", METHOD_1ARGS(ACPI_TYPE_INTEGER),
	  METHOD_RETURNS(ACPI_RTYPE_INTEGER | ACPI_RTYPE_STRING |
			 ACPI_RTYPE_BUFFER)}},

	PACKAGE_INFO(0, 0, 0, 0, 0, 0)	/*                  */
};
#else
extern const union acpi_predefined_info acpi_gbl_predefined_methods[];
#endif

#if (defined ACPI_CREATE_RESOURCE_TABLE && defined ACPI_APPLICATION)
/*                                                                             
  
                                                                       
                                                                      
                                                            
  
                                                          
  
                                                                             */

const union acpi_predefined_info acpi_gbl_resource_names[] = {
	{{"_ADR", WIDTH_16 | WIDTH_64, 0}},
	{{"_ALN", WIDTH_8 | WIDTH_16 | WIDTH_32, 0}},
	{{"_ASI", WIDTH_8, 0}},
	{{"_ASZ", WIDTH_8, 0}},
	{{"_ATT", WIDTH_64, 0}},
	{{"_BAS", WIDTH_16 | WIDTH_32, 0}},
	{{"_BM_", WIDTH_1, 0}},
	{{"_DBT", WIDTH_16, 0}},	/*          */
	{{"_DEC", WIDTH_1, 0}},
	{{"_DMA", WIDTH_8, 0}},
	{{"_DPL", WIDTH_1, 0}},	/*          */
	{{"_DRS", WIDTH_16, 0}},	/*          */
	{{"_END", WIDTH_1, 0}},	/*          */
	{{"_FLC", WIDTH_2, 0}},	/*          */
	{{"_GRA", WIDTH_ADDRESS, 0}},
	{{"_HE_", WIDTH_1, 0}},
	{{"_INT", WIDTH_16 | WIDTH_32, 0}},
	{{"_IOR", WIDTH_2, 0}},	/*          */
	{{"_LEN", WIDTH_8 | WIDTH_ADDRESS, 0}},
	{{"_LIN", WIDTH_8, 0}},	/*          */
	{{"_LL_", WIDTH_1, 0}},
	{{"_MAF", WIDTH_1, 0}},
	{{"_MAX", WIDTH_ADDRESS, 0}},
	{{"_MEM", WIDTH_2, 0}},
	{{"_MIF", WIDTH_1, 0}},
	{{"_MIN", WIDTH_ADDRESS, 0}},
	{{"_MOD", WIDTH_1, 0}},	/*          */
	{{"_MTP", WIDTH_2, 0}},
	{{"_PAR", WIDTH_8, 0}},	/*          */
	{{"_PHA", WIDTH_1, 0}},	/*          */
	{{"_PIN", WIDTH_16, 0}},	/*          */
	{{"_PPI", WIDTH_8, 0}},	/*          */
	{{"_POL", WIDTH_1 | WIDTH_2, 0}},	/*          */
	{{"_RBO", WIDTH_8, 0}},
	{{"_RBW", WIDTH_8, 0}},
	{{"_RNG", WIDTH_1, 0}},
	{{"_RT_", WIDTH_8, 0}},	/*          */
	{{"_RW_", WIDTH_1, 0}},
	{{"_RXL", WIDTH_16, 0}},	/*          */
	{{"_SHR", WIDTH_2, 0}},
	{{"_SIZ", WIDTH_2, 0}},
	{{"_SLV", WIDTH_1, 0}},	/*          */
	{{"_SPE", WIDTH_32, 0}},	/*          */
	{{"_STB", WIDTH_2, 0}},	/*          */
	{{"_TRA", WIDTH_ADDRESS, 0}},
	{{"_TRS", WIDTH_1, 0}},
	{{"_TSF", WIDTH_8, 0}},	/*          */
	{{"_TTP", WIDTH_1, 0}},
	{{"_TXL", WIDTH_16, 0}},	/*          */
	{{"_TYP", WIDTH_2 | WIDTH_16, 0}},
	{{"_VEN", VARIABLE_DATA, 0}},	/*          */
	PACKAGE_INFO(0, 0, 0, 0, 0, 0)	/*                  */
};

static const union acpi_predefined_info acpi_gbl_scope_names[] = {
	{{"_GPE", 0, 0}},
	{{"_PR_", 0, 0}},
	{{"_SB_", 0, 0}},
	{{"_SI_", 0, 0}},
	{{"_TZ_", 0, 0}},
	PACKAGE_INFO(0, 0, 0, 0, 0, 0)	/*                  */
};
#else
extern const union acpi_predefined_info acpi_gbl_resource_names[];
#endif

#endif
