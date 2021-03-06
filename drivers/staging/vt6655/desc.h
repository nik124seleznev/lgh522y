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
 * File: desc.h
 *
 * Purpose:The header file of descriptor
 *
 * Revision History:
 *
 * Author: Tevin Chen
 *
 * Date: May 21, 1996
 *
 */

#ifndef __DESC_H__
#define __DESC_H__

#include <linux/types.h>
#include <linux/mm.h>
#include "ttype.h"
#include "tether.h"

/*                                                                   */

#define B_OWNED_BY_CHIP     1           //
#define B_OWNED_BY_HOST     0           //

//
//                         
//
#define RSR_ADDRBROAD       0x80        //          
#define RSR_ADDRMULTI       0x40        //          
#define RSR_ADDRUNI         0x00        //          
#define RSR_IVLDTYP         0x20        //                                
#define RSR_IVLDLEN         0x10        //                                      
#define RSR_BSSIDOK         0x08        //          
#define RSR_CRCOK           0x04        //          
#define RSR_BCNSSIDOK       0x02        //          
#define RSR_ADDROK          0x01        //          

//
//                             
//
#define NEWRSR_DECRYPTOK    0x10        //          
#define NEWRSR_CFPIND       0x08        //          
#define NEWRSR_HWUTSF       0x04        //          
#define NEWRSR_BCNHITAID    0x02        //          
#define NEWRSR_BCNHITAID0   0x01        //          

//
//                          
//
#define TSR0_PWRSTS1_2      0xC0        //          
#define TSR0_PWRSTS7        0x20        //          
#define TSR0_NCR            0x1F        //          

//
//                          
//
#define TSR1_TERR           0x80        //          
#define TSR1_PWRSTS4_6      0x70        //          
#define TSR1_RETRYTMO       0x08        //          
#define TSR1_TMO            0x04        //          
#define TSR1_PWRSTS3        0x02        //          
#define ACK_DATA            0x01        //          

//
//                         
//
#define EDMSDU              0x04        //                     
#define TCR_EDP             0x02        //                        
#define TCR_STP             0x01        //                          

//                                    
#define CB_MAX_BUF_SIZE     2900U       //                
					//                            
#define CB_MAX_TX_BUF_SIZE          CB_MAX_BUF_SIZE //                   
#define CB_MAX_RX_BUF_SIZE_NORMAL   CB_MAX_BUF_SIZE //                                         

#define CB_BEACON_BUF_SIZE  512U        //                           

#define CB_MAX_RX_DESC      128         //                    
#define CB_MIN_RX_DESC      16          //                       
#define CB_MAX_TX_DESC      64          //                    
#define CB_MIN_TX_DESC      16          //                       

#define CB_MAX_RECEIVED_PACKETS     16  //                                      
					//                                        
					//                                   
					//                                         
					//                                      

#define CB_EXTRA_RD_NUM     32          //                      
#define CB_RD_NUM           32          //                
#define CB_TD_NUM           32          //                

//                                
//                                                          
//                                                     
#define CB_MAX_SEGMENT      4

#define CB_MIN_MAP_REG_NUM  4
#define CB_MAX_MAP_REG_NUM  CB_MAX_TX_DESC

#define CB_PROTOCOL_RESERVED_SECTION    16

//                                               
//                                   
//                           
#define CB_MAX_TX_ABORT_RETRY   3

#ifdef __BIG_ENDIAN

//                             
#define FIFOCTL_AUTO_FB_1   0x0010 //                    
#define FIFOCTL_AUTO_FB_0   0x0008 //                    
#define FIFOCTL_GRPACK      0x0004 //                    
#define FIFOCTL_11GA        0x0003 //                    
#define FIFOCTL_11GB        0x0002 //                    
#define FIFOCTL_11B         0x0001 //                    
#define FIFOCTL_11A         0x0000 //                    
#define FIFOCTL_RTS         0x8000 //                    
#define FIFOCTL_ISDMA0      0x4000 //                    
#define FIFOCTL_GENINT      0x2000 //                    
#define FIFOCTL_TMOEN       0x1000 //                    
#define FIFOCTL_LRETRY      0x0800 //                    
#define FIFOCTL_CRCDIS      0x0400 //                    
#define FIFOCTL_NEEDACK     0x0200 //                    
#define FIFOCTL_LHEAD       0x0100 //                    

//                            
#define FRAGCTL_AES         0x0003 //                    
#define FRAGCTL_TKIP        0x0002 //                    
#define FRAGCTL_LEGACY      0x0001 //                    
#define FRAGCTL_NONENCRYPT  0x0000 //                    
#define FRAGCTL_ENDFRAG     0x0300 //                    
#define FRAGCTL_MIDFRAG     0x0200 //                    
#define FRAGCTL_STAFRAG     0x0100 //                    
#define FRAGCTL_NONFRAG     0x0000 //                    

#else

#define FIFOCTL_AUTO_FB_1   0x1000 //                    
#define FIFOCTL_AUTO_FB_0   0x0800 //                    
#define FIFOCTL_GRPACK      0x0400 //                    
#define FIFOCTL_11GA        0x0300 //                    
#define FIFOCTL_11GB        0x0200 //                    
#define FIFOCTL_11B         0x0100 //                    
#define FIFOCTL_11A         0x0000 //                    
#define FIFOCTL_RTS         0x0080 //                    
#define FIFOCTL_ISDMA0      0x0040 //                    
#define FIFOCTL_GENINT      0x0020 //                    
#define FIFOCTL_TMOEN       0x0010 //                    
#define FIFOCTL_LRETRY      0x0008 //                    
#define FIFOCTL_CRCDIS      0x0004 //                    
#define FIFOCTL_NEEDACK     0x0002 //                    
#define FIFOCTL_LHEAD       0x0001 //                    

//                            
#define FRAGCTL_AES         0x0300 //                    
#define FRAGCTL_TKIP        0x0200 //                    
#define FRAGCTL_LEGACY      0x0100 //                    
#define FRAGCTL_NONENCRYPT  0x0000 //                    
#define FRAGCTL_ENDFRAG     0x0003 //                    
#define FRAGCTL_MIDFRAG     0x0002 //                    
#define FRAGCTL_STAFRAG     0x0001 //                    
#define FRAGCTL_NONFRAG     0x0000 //                    

#endif //                    

#define TYPE_TXDMA0     0
#define TYPE_AC0DMA     1
#define TYPE_ATIMDMA    2
#define TYPE_SYNCDMA    3
#define TYPE_MAXTD      2

#define TYPE_BEACONDMA  4

#define TYPE_RXDMA0     0
#define TYPE_RXDMA1     1
#define TYPE_MAXRD      2

//                          
#define TD_FLAGS_NETIF_SKB               0x01       //                          
#define TD_FLAGS_PRIV_SKB                0x02       //                                         
#define TD_FLAGS_PS_RETRY                0x04       //                                  

/*                                                                   */

//                                                                                                 
//                                                                                          
//                                        

typedef struct tagDEVICE_RD_INFO {
	struct sk_buff *skb;
	dma_addr_t  skb_dma;
	dma_addr_t  curr_desc;
} DEVICE_RD_INFO,   *PDEVICE_RD_INFO;

/*
                                                     
                       
                                                    
                  
              
        
                                       
             
   
   
*/

/*
                           
                           
                          
                                      
                                 
                              
       
*/

#ifdef __BIG_ENDIAN

typedef struct tagRDES0 {
	volatile unsigned short wResCount;
	union {
		volatile u16    f15Reserved;
		struct {
			volatile u8 f8Reserved1;
			volatile u8 f1Owner:1;
			volatile u8 f7Reserved:7;
		} __attribute__ ((__packed__));
	} __attribute__ ((__packed__));
} __attribute__ ((__packed__))
SRDES0, *PSRDES0;

#else

typedef struct tagRDES0 {
	unsigned short wResCount;
	unsigned short f15Reserved:15;
	unsigned short f1Owner:1;
} __attribute__ ((__packed__))
SRDES0;

#endif

typedef struct tagRDES1 {
	unsigned short wReqCount;
	unsigned short wReserved;
} __attribute__ ((__packed__))
SRDES1;

//
//              
//
typedef struct tagSRxDesc {
	volatile SRDES0 m_rd0RD0;
	volatile SRDES1 m_rd1RD1;
	volatile u32    buff_addr;
	volatile u32    next_desc;
	struct tagSRxDesc   *next;//       
	volatile PDEVICE_RD_INFO    pRDInfo;//       
	volatile u32    Reserved[2];//       
} __attribute__ ((__packed__))
SRxDesc, *PSRxDesc;
typedef const SRxDesc *PCSRxDesc;

#ifdef __BIG_ENDIAN

/*
                           
                                   
                                   
                                           
                                                
                                           
                              
       
*/

typedef struct tagTDES0 {
	volatile    unsigned char byTSR0;
	volatile    unsigned char byTSR1;
	union {
		volatile u16    f15Txtime;
		struct {
			volatile u8 f8Reserved1;
			volatile u8 f1Owner:1;
			volatile u8 f7Reserved:7;
		} __attribute__ ((__packed__));
	} __attribute__ ((__packed__));
} __attribute__ ((__packed__))
STDES0, PSTDES0;

#else

typedef struct tagTDES0 {
	volatile    unsigned char byTSR0;
	volatile    unsigned char byTSR1;
	volatile    unsigned short f15Txtime:15;
	volatile    unsigned short f1Owner:1;
} __attribute__ ((__packed__))
STDES0;

#endif

typedef struct tagTDES1 {
	volatile    unsigned short wReqCount;
	volatile    unsigned char byTCR;
	volatile    unsigned char byReserved;
} __attribute__ ((__packed__))
STDES1;

typedef struct tagDEVICE_TD_INFO {
	struct sk_buff *skb;
	unsigned char *buf;
	dma_addr_t          skb_dma;
	dma_addr_t          buf_dma;
	dma_addr_t          curr_desc;
	unsigned long dwReqCount;
	unsigned long dwHeaderLength;
	unsigned char byFlags;
} DEVICE_TD_INFO,    *PDEVICE_TD_INFO;

/*
                                                     
                       
                                                   
                  
              
        
                                       
             
   
   
*/

//
//                    
//
typedef struct tagSTxDesc {
	volatile    STDES0  m_td0TD0;
	volatile    STDES1  m_td1TD1;
	volatile    u32    buff_addr;
	volatile    u32    next_desc;
	struct tagSTxDesc *next; //       
	volatile    PDEVICE_TD_INFO pTDInfo;//       
	volatile    u32    Reserved[2];//       
} __attribute__ ((__packed__))
STxDesc, *PSTxDesc;
typedef const STxDesc *PCSTxDesc;

typedef struct tagSTxSyncDesc {
	volatile    STDES0  m_td0TD0;
	volatile    STDES1  m_td1TD1;
	volatile    u32 buff_addr; //                          
	volatile    u32 next_desc; //                                   
	volatile    unsigned short m_wFIFOCtl;
	volatile    unsigned short m_wTimeStamp;
	struct tagSTxSyncDesc *next; //       
	volatile    PDEVICE_TD_INFO pTDInfo;//       
	volatile    u32 m_dwReserved2;
} __attribute__ ((__packed__))
STxSyncDesc, *PSTxSyncDesc;
typedef const STxSyncDesc *PCSTxSyncDesc;

//
//                      
//
typedef struct tagSRrvTime_gRTS {
	unsigned short wRTSTxRrvTime_ba;
	unsigned short wRTSTxRrvTime_aa;
	unsigned short wRTSTxRrvTime_bb;
	unsigned short wReserved;
	unsigned short wTxRrvTime_b;
	unsigned short wTxRrvTime_a;
} __attribute__ ((__packed__))
SRrvTime_gRTS, *PSRrvTime_gRTS;
typedef const SRrvTime_gRTS *PCSRrvTime_gRTS;

typedef struct tagSRrvTime_gCTS {
	unsigned short wCTSTxRrvTime_ba;
	unsigned short wReserved;
	unsigned short wTxRrvTime_b;
	unsigned short wTxRrvTime_a;
} __attribute__ ((__packed__))
SRrvTime_gCTS, *PSRrvTime_gCTS;
typedef const SRrvTime_gCTS *PCSRrvTime_gCTS;

typedef struct tagSRrvTime_ab {
	unsigned short wRTSTxRrvTime;
	unsigned short wTxRrvTime;
} __attribute__ ((__packed__))
SRrvTime_ab, *PSRrvTime_ab;
typedef const SRrvTime_ab *PCSRrvTime_ab;

typedef struct tagSRrvTime_atim {
	unsigned short wCTSTxRrvTime_ba;
	unsigned short wTxRrvTime_a;
} __attribute__ ((__packed__))
SRrvTime_atim, *PSRrvTime_atim;
typedef const SRrvTime_atim *PCSRrvTime_atim;

//
//                  
//
typedef struct tagSRTSData {
	unsigned short wFrameControl;
	unsigned short wDurationID;
	unsigned char abyRA[ETH_ALEN];
	unsigned char abyTA[ETH_ALEN];
} __attribute__ ((__packed__))
SRTSData, *PSRTSData;
typedef const SRTSData *PCSRTSData;

typedef struct tagSRTS_g {
	unsigned char bySignalField_b;
	unsigned char byServiceField_b;
	unsigned short wTransmitLength_b;
	unsigned char bySignalField_a;
	unsigned char byServiceField_a;
	unsigned short wTransmitLength_a;
	unsigned short wDuration_ba;
	unsigned short wDuration_aa;
	unsigned short wDuration_bb;
	unsigned short wReserved;
	SRTSData    Data;
} __attribute__ ((__packed__))
SRTS_g, *PSRTS_g;
typedef const SRTS_g *PCSRTS_g;

typedef struct tagSRTS_g_FB {
	unsigned char bySignalField_b;
	unsigned char byServiceField_b;
	unsigned short wTransmitLength_b;
	unsigned char bySignalField_a;
	unsigned char byServiceField_a;
	unsigned short wTransmitLength_a;
	unsigned short wDuration_ba;
	unsigned short wDuration_aa;
	unsigned short wDuration_bb;
	unsigned short wReserved;
	unsigned short wRTSDuration_ba_f0;
	unsigned short wRTSDuration_aa_f0;
	unsigned short wRTSDuration_ba_f1;
	unsigned short wRTSDuration_aa_f1;
	SRTSData    Data;
} __attribute__ ((__packed__))
SRTS_g_FB, *PSRTS_g_FB;
typedef const SRTS_g_FB *PCSRTS_g_FB;

typedef struct tagSRTS_ab {
	unsigned char bySignalField;
	unsigned char byServiceField;
	unsigned short wTransmitLength;
	unsigned short wDuration;
	unsigned short wReserved;
	SRTSData    Data;
} __attribute__ ((__packed__))
SRTS_ab, *PSRTS_ab;
typedef const SRTS_ab *PCSRTS_ab;

typedef struct tagSRTS_a_FB {
	unsigned char bySignalField;
	unsigned char byServiceField;
	unsigned short wTransmitLength;
	unsigned short wDuration;
	unsigned short wReserved;
	unsigned short wRTSDuration_f0;
	unsigned short wRTSDuration_f1;
	SRTSData    Data;
} __attribute__ ((__packed__))
SRTS_a_FB, *PSRTS_a_FB;
typedef const SRTS_a_FB *PCSRTS_a_FB;

//
//                  
//
typedef struct tagSCTSData {
	unsigned short wFrameControl;
	unsigned short wDurationID;
	unsigned char abyRA[ETH_ALEN];
	unsigned short wReserved;
} __attribute__ ((__packed__))
SCTSData, *PSCTSData;

typedef struct tagSCTS {
	unsigned char bySignalField_b;
	unsigned char byServiceField_b;
	unsigned short wTransmitLength_b;
	unsigned short wDuration_ba;
	unsigned short wReserved;
	SCTSData    Data;
} __attribute__ ((__packed__))
SCTS, *PSCTS;
typedef const SCTS *PCSCTS;

typedef struct tagSCTS_FB {
	unsigned char bySignalField_b;
	unsigned char byServiceField_b;
	unsigned short wTransmitLength_b;
	unsigned short wDuration_ba;
	unsigned short wReserved;
	unsigned short wCTSDuration_ba_f0;
	unsigned short wCTSDuration_ba_f1;
	SCTSData    Data;
} __attribute__ ((__packed__))
SCTS_FB, *PSCTS_FB;
typedef const SCTS_FB *PCSCTS_FB;

//
//               
//
typedef struct tagSTxBufHead {
	u32 adwTxKey[4];
	unsigned short wFIFOCtl;
	unsigned short wTimeStamp;
	unsigned short wFragCtl;
	unsigned char byTxPower;
	unsigned char wReserved;
} __attribute__ ((__packed__))
STxBufHead, *PSTxBufHead;
typedef const STxBufHead *PCSTxBufHead;

typedef struct tagSTxShortBufHead {
	unsigned short wFIFOCtl;
	unsigned short wTimeStamp;
} __attribute__ ((__packed__))
STxShortBufHead, *PSTxShortBufHead;
typedef const STxShortBufHead *PCSTxShortBufHead;

//
//               
//
typedef struct tagSTxDataHead_g {
	unsigned char bySignalField_b;
	unsigned char byServiceField_b;
	unsigned short wTransmitLength_b;
	unsigned char bySignalField_a;
	unsigned char byServiceField_a;
	unsigned short wTransmitLength_a;
	unsigned short wDuration_b;
	unsigned short wDuration_a;
	unsigned short wTimeStampOff_b;
	unsigned short wTimeStampOff_a;
} __attribute__ ((__packed__))
STxDataHead_g, *PSTxDataHead_g;
typedef const STxDataHead_g *PCSTxDataHead_g;

typedef struct tagSTxDataHead_g_FB {
	unsigned char bySignalField_b;
	unsigned char byServiceField_b;
	unsigned short wTransmitLength_b;
	unsigned char bySignalField_a;
	unsigned char byServiceField_a;
	unsigned short wTransmitLength_a;
	unsigned short wDuration_b;
	unsigned short wDuration_a;
	unsigned short wDuration_a_f0;
	unsigned short wDuration_a_f1;
	unsigned short wTimeStampOff_b;
	unsigned short wTimeStampOff_a;
} __attribute__ ((__packed__))
STxDataHead_g_FB, *PSTxDataHead_g_FB;
typedef const STxDataHead_g_FB *PCSTxDataHead_g_FB;

typedef struct tagSTxDataHead_ab {
	unsigned char bySignalField;
	unsigned char byServiceField;
	unsigned short wTransmitLength;
	unsigned short wDuration;
	unsigned short wTimeStampOff;
} __attribute__ ((__packed__))
STxDataHead_ab, *PSTxDataHead_ab;
typedef const STxDataHead_ab *PCSTxDataHead_ab;

typedef struct tagSTxDataHead_a_FB {
	unsigned char bySignalField;
	unsigned char byServiceField;
	unsigned short wTransmitLength;
	unsigned short wDuration;
	unsigned short wTimeStampOff;
	unsigned short wDuration_f0;
	unsigned short wDuration_f1;
} __attribute__ ((__packed__))
STxDataHead_a_FB, *PSTxDataHead_a_FB;
typedef const STxDataHead_a_FB *PCSTxDataHead_a_FB;

//
//                   
//
typedef struct tagSMICHDRHead {
	u32 adwHDR0[4];
	u32 adwHDR1[4];
	u32 adwHDR2[4];
} __attribute__ ((__packed__))
SMICHDRHead, *PSMICHDRHead;
typedef const SMICHDRHead *PCSMICHDRHead;

typedef struct tagSBEACONCtl {
	u32 BufReady:1;
	u32 TSF:15;
	u32 BufLen:11;
	u32 Reserved:5;
} __attribute__ ((__packed__))
SBEACONCtl;

typedef struct tagSSecretKey {
	u32 dwLowDword;
	unsigned char byHighByte;
} __attribute__ ((__packed__))
SSecretKey;

typedef struct tagSKeyEntry {
	unsigned char abyAddrHi[2];
	unsigned short wKCTL;
	unsigned char abyAddrLo[4];
	u32 dwKey0[4];
	u32 dwKey1[4];
	u32 dwKey2[4];
	u32 dwKey3[4];
	u32 dwKey4[4];
} __attribute__ ((__packed__))
SKeyEntry;
/*                                                                   */

/*                                                                   */

/*                                                                   */

/*                                                                   */

#endif //           
