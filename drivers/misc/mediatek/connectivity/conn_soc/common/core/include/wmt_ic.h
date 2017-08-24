/*
* Copyright (C) 2011-2014 MediaTek Inc.
* 
* This program is free software: you can redistribute it and/or modify it under the terms of the 
* GNU General Public License version 2 as published by the Free Software Foundation.
* 
* This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
* without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
* See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with this program.
* If not, see <http://www.gnu.org/licenses/>.
*/

/*       
                                            

                                                                                           
*/



#ifndef _WMT_IC_H_
#define _WMT_IC_H_

/*                                                                              
                                                     
                                                                                
*/

/*                                                                              
                                                          
                                                                                
*/

#include "wmt_core.h"
#include "wmt_exp.h"

/*                                                                              
                                             
                                                                                
*/

/*                                                                              
                                                
                                                                                
*/

#define WMT_IC_NAME_MT6620 "MT6620"
#define WMT_IC_NAME_MT6628 "MT6628"
#define WMT_IC_NAME_DEFAULT "SOC_CONSYS"

#define WMT_IC_VER_E1 "E1"
#define WMT_IC_VER_E2 "E2"
#define WMT_IC_VER_E3 "E3"
#define WMT_IC_VER_E4 "E4"
#define WMT_IC_VER_E5 "E5"
#define WMT_IC_VER_E6 "E6"

#define WMT_IC_PATCH_DUMMY_EXT "_ex"
#define WMT_IC_PATCH_NO_EXT ""
#define WMT_IC_PATCH_E1_EXT "_e1"
#define WMT_IC_PATCH_E2_EXT "_e2"
#define WMT_IC_PATCH_E3_EXT "_e3"
#define WMT_IC_PATCH_E4_EXT "_e4"
#define WMT_IC_PATCH_E5_EXT "_e5"
#define WMT_IC_PATCH_E6_EXT "_e6"

#define WMT_IC_PATCH_TAIL    "_hdr.bin"

#define WMT_IC_INVALID_CHIP_ID 0xFFFF

#define MAJORNUM(x) (x & 0x00F0)
#define MINORNUM(x) (x & 0x000F)

/*                                                                              
                                                     
                                                                                
*/
/*                                               */
/*                                   */
#define GEN_CONFG_BASE (0x80000000UL)
#define GEN_HVR (GEN_CONFG_BASE + 0x0UL) /*        */
#define GEN_FVR (GEN_CONFG_BASE + 0x4UL) /*        */
#define GEN_VER_MASK (0x0000FFFFUL) /*                                   */
#define GEN_HCR (GEN_CONFG_BASE + 0x8UL) /*                  */
#define GEN_HCR_MASK (0x0000FFFFUL) /*                         */

/*                                                                              
                                                 
                                                                                
*/

typedef struct _WMT_IC_INFO_S
{
    UINT32 u4HwVer; /*        */
    PUINT8 cChipName;
    PUINT8 cChipVersion;
    PUINT8 cPatchNameExt;
    MTK_WCN_BOOL bPsmSupport;
    MTK_WCN_BOOL bWorkWithoutPatch;
    ENUM_WMTHWVER_TYPE_T eWmtHwVer;
} WMT_IC_INFO_S, *P_WMT_IC_INFO_S;

/*                                                                              
                                                  
                                                                                
*/

/*                                                                              
                                                   
                                                                                
*/

/*                                                                              
                                                            
                                                                                
*/

/*                                                                              
                                                
                                                                                
*/

#endif /*            */

