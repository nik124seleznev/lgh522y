/*
 * Copyright (c) 1996, 2003 VIA Networking Technologies, Inc.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *
 * File: wpa2.c
 *
 * Purpose: Handles the Basic Service Set & Node Database functions
 *
 * Functions:
 *
 * Revision History:
 *
 * Author: Yiching Chen
 *
 * Date: Oct. 4, 2004
 *
 */

#include "device.h"
#include "wpa2.h"

static int          msglevel                =MSG_LEVEL_INFO;
//                                                             

const u8 abyOUIGK[4]      = { 0x00, 0x0F, 0xAC, 0x00 };
const u8 abyOUIWEP40[4]   = { 0x00, 0x0F, 0xAC, 0x01 };
const u8 abyOUIWEP104[4]  = { 0x00, 0x0F, 0xAC, 0x05 };
const u8 abyOUITKIP[4]    = { 0x00, 0x0F, 0xAC, 0x02 };
const u8 abyOUICCMP[4]    = { 0x00, 0x0F, 0xAC, 0x04 };

const u8 abyOUI8021X[4]   = { 0x00, 0x0F, 0xAC, 0x01 };
const u8 abyOUIPSK[4]     = { 0x00, 0x0F, 0xAC, 0x02 };

/* 
  
               
                                       
  
              
       
                            
        
            
  
                      
  
 */
void
WPA2_ClearRSN (
     PKnownBSS        pBSSNode
    )
{
    int ii;

    pBSSNode->bWPA2Valid = false;

    pBSSNode->byCSSGK = WLAN_11i_CSS_CCMP;
    for (ii=0; ii < 4; ii ++)
        pBSSNode->abyCSSPK[ii] = WLAN_11i_CSS_CCMP;
    pBSSNode->wCSSPKCount = 1;
    for (ii=0; ii < 4; ii ++)
        pBSSNode->abyAKMSSAuthType[ii] = WLAN_11i_AKMSS_802_1X;
    pBSSNode->wAKMSSAuthCount = 1;
    pBSSNode->sRSNCapObj.bRSNCapExist = false;
    pBSSNode->sRSNCapObj.wRSNCap = 0;
}

/* 
  
               
                   
  
              
       
                            
                                     
        
            
  
                      
  
 */
void
WPA2vParseRSN (
     PKnownBSS        pBSSNode,
     PWLAN_IE_RSN     pRSN
    )
{
    int                 i, j;
    u16                m = 0, n = 0;
    u8 *               pbyOUI;
    bool                bUseGK = false;

    DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"WPA2_ParseRSN: [%d]\n", pRSN->len);

    WPA2_ClearRSN(pBSSNode);

    if (pRSN->len == 2) { //       
        if ((pRSN->byElementID == WLAN_EID_RSN) && (pRSN->wVersion == 1)) {
            pBSSNode->bWPA2Valid = true;
        }
        return;
    }

    if (pRSN->len < 6) { //               
        //                                 
        return;
    }

    //                                       
    if ((pRSN->byElementID == WLAN_EID_RSN) &&
        (pRSN->wVersion == 1)) {

        DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"Legal 802.11i RSN\n");

        pbyOUI = &(pRSN->abyRSN[0]);
        if ( !memcmp(pbyOUI, abyOUIWEP40, 4))
            pBSSNode->byCSSGK = WLAN_11i_CSS_WEP40;
        else if ( !memcmp(pbyOUI, abyOUITKIP, 4))
            pBSSNode->byCSSGK = WLAN_11i_CSS_TKIP;
        else if ( !memcmp(pbyOUI, abyOUICCMP, 4))
            pBSSNode->byCSSGK = WLAN_11i_CSS_CCMP;
        else if ( !memcmp(pbyOUI, abyOUIWEP104, 4))
            pBSSNode->byCSSGK = WLAN_11i_CSS_WEP104;
        else if ( !memcmp(pbyOUI, abyOUIGK, 4)) {
            //                                 
            return;
        } else
            //                       
            pBSSNode->byCSSGK = WLAN_11i_CSS_UNKNOWN;

        DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"802.11i CSS: %X\n", pBSSNode->byCSSGK);

        if (pRSN->len == 6) {
            pBSSNode->bWPA2Valid = true;
            return;
        }

        if (pRSN->len >= 8) { //                             
            pBSSNode->wCSSPKCount = *((u16 *) &(pRSN->abyRSN[4]));
            j = 0;
            pbyOUI = &(pRSN->abyRSN[6]);

            for (i = 0; (i < pBSSNode->wCSSPKCount) && (j < sizeof(pBSSNode->abyCSSPK)/sizeof(u8)); i++) {

                if (pRSN->len >= 8+i*4+4) { //                               
                    if ( !memcmp(pbyOUI, abyOUIGK, 4)) {
                        pBSSNode->abyCSSPK[j++] = WLAN_11i_CSS_USE_GROUP;
                        bUseGK = true;
                    } else if ( !memcmp(pbyOUI, abyOUIWEP40, 4)) {
                        //                              
                    } else if ( !memcmp(pbyOUI, abyOUITKIP, 4)) {
                        if (pBSSNode->byCSSGK != WLAN_11i_CSS_CCMP)
                            pBSSNode->abyCSSPK[j++] = WLAN_11i_CSS_TKIP;
                        else
                            ; //                              
                    } else if ( !memcmp(pbyOUI, abyOUICCMP, 4)) {
                        pBSSNode->abyCSSPK[j++] = WLAN_11i_CSS_CCMP;
                    } else if ( !memcmp(pbyOUI, abyOUIWEP104, 4)) {
                        //                              
                    } else {
                        //                       
                        pBSSNode->abyCSSPK[j++] = WLAN_11i_CSS_UNKNOWN;
                    }
                    pbyOUI += 4;
                    DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"abyCSSPK[%d]: %X\n", j-1, pBSSNode->abyCSSPK[j-1]);
                } else
                    break;
            } //   

            if (bUseGK == true) {
                if (j != 1) {
                    //                                         
                    return;
                }
                if (pBSSNode->byCSSGK == WLAN_11i_CSS_CCMP) {
                    //                                                    
                    return;
                }
            }
            if ((pBSSNode->wCSSPKCount != 0) && (j == 0)) {
                //                          
                return;
            }
            pBSSNode->wCSSPKCount = (u16)j;
            DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"wCSSPKCount: %d\n", pBSSNode->wCSSPKCount);
        }

        m = *((u16 *) &(pRSN->abyRSN[4]));

        if (pRSN->len >= 10+m*4) { //                                                         
            pBSSNode->wAKMSSAuthCount = *((u16 *) &(pRSN->abyRSN[6+4*m]));
            j = 0;
            pbyOUI = &(pRSN->abyRSN[8+4*m]);
            for (i = 0; (i < pBSSNode->wAKMSSAuthCount) && (j < sizeof(pBSSNode->abyAKMSSAuthType)/sizeof(u8)); i++) {
                if (pRSN->len >= 10+(m+i)*4+4) { //                                                 
                    if ( !memcmp(pbyOUI, abyOUI8021X, 4))
                        pBSSNode->abyAKMSSAuthType[j++] = WLAN_11i_AKMSS_802_1X;
                    else if ( !memcmp(pbyOUI, abyOUIPSK, 4))
                        pBSSNode->abyAKMSSAuthType[j++] = WLAN_11i_AKMSS_PSK;
                    else
                        //                       
                        pBSSNode->abyAKMSSAuthType[j++] = WLAN_11i_AKMSS_UNKNOWN;
                    DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"abyAKMSSAuthType[%d]: %X\n", j-1, pBSSNode->abyAKMSSAuthType[j-1]);
                } else
                    break;
            }
            pBSSNode->wAKMSSAuthCount = (u16)j;
            DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"wAKMSSAuthCount: %d\n", pBSSNode->wAKMSSAuthCount);

            n = *((u16 *) &(pRSN->abyRSN[6+4*m]));
            if (pRSN->len >= 12+4*m+4*n) { //                                                             
                pBSSNode->sRSNCapObj.bRSNCapExist = true;
                pBSSNode->sRSNCapObj.wRSNCap = *((u16 *) &(pRSN->abyRSN[8+4*m+4*n]));
            }
        }
        //                                                           
        pBSSNode->bWPA2Valid = true;
    }
}
