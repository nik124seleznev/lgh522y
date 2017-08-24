/*
                                                                                         
*/

/*                         
                                                               

                                                                                
                                                            
*/



/*
                          
  
                      
                                                                                            
                                                                  
  
                      
                                                                                            
    
  
                   
                                                                                             
                                   
  
                   
                                                                                                                                                                          
                                                                              
  
                   
                                                                                       
                                          
  
                   
                                                                                                            
                                            
  
                   
                                                                                                                                                         
                                               
                                                                   
                                                                                                       
                                                                           
                                            
                                                      
  
                   
                                                                                     
                                                      
  
                   
                                                                                 
                          
                                                                                                     
                                                         
  
                   
                                                                                 
                                     
  
*/

#ifndef _CFG_WIFI_FILE_H
#define _CFG_WIFI_FILE_H

/*                                                                              
                                                     
                                                                                
*/

/*                                                                              
                                                          
                                                                                
*/
#include "gl_typedef.h"

/*                                                                              
                                                
                                                                                
*/

/*                                                                              
                                                 
                                                                                
*/

/*                                                                              
                                                  
                                                                                
*/
/*                                                            */
typedef struct _TX_PWR_PARAM_T {
	INT_8 cTxPwr2G4Cck;	/*                           */
#if defined(MT6620)
	INT_8 acReserved[3];
#elif defined(MT5931) || defined(MT6628)
	INT_8 cTxPwr2G4Dsss;	/*                           */
	INT_8 acReserved[2];
#else
#error "No valid definition!"
#endif

	INT_8 cTxPwr2G4OFDM_BPSK;
	INT_8 cTxPwr2G4OFDM_QPSK;
	INT_8 cTxPwr2G4OFDM_16QAM;
	INT_8 cTxPwr2G4OFDM_Reserved;
	INT_8 cTxPwr2G4OFDM_48Mbps;
	INT_8 cTxPwr2G4OFDM_54Mbps;

	INT_8 cTxPwr2G4HT20_BPSK;
	INT_8 cTxPwr2G4HT20_QPSK;
	INT_8 cTxPwr2G4HT20_16QAM;
	INT_8 cTxPwr2G4HT20_MCS5;
	INT_8 cTxPwr2G4HT20_MCS6;
	INT_8 cTxPwr2G4HT20_MCS7;

	INT_8 cTxPwr2G4HT40_BPSK;
	INT_8 cTxPwr2G4HT40_QPSK;
	INT_8 cTxPwr2G4HT40_16QAM;
	INT_8 cTxPwr2G4HT40_MCS5;
	INT_8 cTxPwr2G4HT40_MCS6;
	INT_8 cTxPwr2G4HT40_MCS7;

	INT_8 cTxPwr5GOFDM_BPSK;
	INT_8 cTxPwr5GOFDM_QPSK;
	INT_8 cTxPwr5GOFDM_16QAM;
	INT_8 cTxPwr5GOFDM_Reserved;
	INT_8 cTxPwr5GOFDM_48Mbps;
	INT_8 cTxPwr5GOFDM_54Mbps;

	INT_8 cTxPwr5GHT20_BPSK;
	INT_8 cTxPwr5GHT20_QPSK;
	INT_8 cTxPwr5GHT20_16QAM;
	INT_8 cTxPwr5GHT20_MCS5;
	INT_8 cTxPwr5GHT20_MCS6;
	INT_8 cTxPwr5GHT20_MCS7;

	INT_8 cTxPwr5GHT40_BPSK;
	INT_8 cTxPwr5GHT40_QPSK;
	INT_8 cTxPwr5GHT40_16QAM;
	INT_8 cTxPwr5GHT40_MCS5;
	INT_8 cTxPwr5GHT40_MCS6;
	INT_8 cTxPwr5GHT40_MCS7;
} TX_PWR_PARAM_T, *P_TX_PWR_PARAM_T;

typedef struct _PWR_5G_OFFSET_T {
	INT_8 cOffsetBand0;	/*              */
	INT_8 cOffsetBand1;	/*              */
	INT_8 cOffsetBand2;	/*              */
	INT_8 cOffsetBand3;	/*              */
	INT_8 cOffsetBand4;	/*              */
	INT_8 cOffsetBand5;	/*              */
	INT_8 cOffsetBand6;	/*              */
	INT_8 cOffsetBand7;	/*              */
} PWR_5G_OFFSET_T, *P_PWR_5G_OFFSET_T;

typedef struct _PWR_PARAM_T {
	UINT_32 au4Data[28];
	UINT_32 u4RefValue1;
	UINT_32 u4RefValue2;
} PWR_PARAM_T, *P_PWR_PARAM_T;

typedef struct _MT6620_CFG_PARAM_STRUCT {
	/*                      */
	UINT_16 u2Part1OwnVersion;
	UINT_16 u2Part1PeerVersion;
	UINT_8 aucMacAddress[6];
	UINT_8 aucCountryCode[2];
	TX_PWR_PARAM_T rTxPwr;
	UINT_8 aucEFUSE[144];
	UINT_8 ucTxPwrValid;
	UINT_8 ucSupport5GBand;
	UINT_8 fg2G4BandEdgePwrUsed;
	INT_8 cBandEdgeMaxPwrCCK;
	INT_8 cBandEdgeMaxPwrOFDM20;
	INT_8 cBandEdgeMaxPwrOFDM40;

	UINT_8 ucRegChannelListMap;
	UINT_8 ucRegChannelListIndex;
	UINT_8 aucRegSubbandInfo[36];

	UINT_8 aucReserved2[256 - 240];

	/*                            */
	UINT_16 u2Part2OwnVersion;
	UINT_16 u2Part2PeerVersion;
	UINT_8 uc2G4BwFixed20M;
	UINT_8 uc5GBwFixed20M;
	UINT_8 ucEnable5GBand;
	UINT_8 aucPreTailReserved;
	UINT_8 aucTailReserved[256 - 8];
} MT6620_CFG_PARAM_STRUCT, *P_MT6620_CFG_PARAM_STRUCT,
    WIFI_CFG_PARAM_STRUCT, *P_WIFI_CFG_PARAM_STRUCT;

/*                                                                              
                                                   
                                                                                
*/

/*                                                                              
                                             
                                                                                
*/
#ifndef DATA_STRUC_INSPECTING_ASSERT
#define DATA_STRUC_INSPECTING_ASSERT(expr) \
        switch (0) {case 0: case (expr): default:; }
#endif

#define CFG_FILE_WIFI_REC_SIZE    sizeof(WIFI_CFG_PARAM_STRUCT)


/*                                                                              
                                                            
                                                                                
*/

/*                                                                              
                                                
                                                                                
*/
#ifndef _lint
/*                                                                        
                                                     
                                                                             
                                               
 */
__KAL_INLINE__ VOID nvramOffsetCheck(VOID)
{
	DATA_STRUC_INSPECTING_ASSERT(OFFSET_OF(WIFI_CFG_PARAM_STRUCT, u2Part2OwnVersion) == 256);

	DATA_STRUC_INSPECTING_ASSERT(sizeof(WIFI_CFG_PARAM_STRUCT) == 512);

	DATA_STRUC_INSPECTING_ASSERT((OFFSET_OF(WIFI_CFG_PARAM_STRUCT, aucEFUSE) & 0x0001) == 0);

	DATA_STRUC_INSPECTING_ASSERT((OFFSET_OF(WIFI_CFG_PARAM_STRUCT, aucRegSubbandInfo) & 0x0001)
				     == 0);
}
#endif

#endif				/*                  */
