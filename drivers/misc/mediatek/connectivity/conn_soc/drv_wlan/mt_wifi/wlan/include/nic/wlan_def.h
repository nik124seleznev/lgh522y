/*
                                                                                        
*/

/*                      
                                                           

*/



/*
                     
  
                   
                                           
  
                   
                                                                              
                              
  
                   
                                                                             
                                                   
  
                   
                                                                                                                                       
                                                                     
                               
  
                        
                                                       
                
  
                        
       
                                                                      
  
                        
                                                                                              
                                                                                                                       
  
                        
                                                                                              
                              
  
                      
                                                                                                        
                                              
                                          
  
                         
                                                                                              
                                                           
  
                   
                                                                                                                                 
                                         
  
                         
       
                                            
  
                         
       
                                                                     
  
                         
       
                                                 
  
                        
       
                                                                                                                        
  
                         
       
                       
  
                         
       
                                                                   
  
                        
       
                                                      
  
                        
       
                                      
  
                        
       
                                             
  
                        
  
                                        
  
                   
  
                                                               
  
                        
  
                                            
  
                   
  
                        
  
                   
  
                                                                                                     
                                                                            
                                                                                                                                   
  
                   
  
                                                                               
  
                      
                                                            
                                
  
                   
                                                    
                                               
                                                            
  
                        
                                                    
                                                            
  
                   
                                                
                                                         
  
                   
                                                    
                      
  
                   
                                                    
                      
                                                                      
                                                           
                                         
                                          
  
                   
                                                    
                                     
  
                                                     
  
                   
                                                    
                                        
  
                   
                                                    
                                                                           
  
                   
                                                    
                    
  
                   
                                                    
                                  
  
                   
                                                    
                                                            
  
                         
                                                
                                   
                                                                           
                    
  
*/

#ifndef _WLAN_DEF_H
#define _WLAN_DEF_H

/*                                                                              
                                                     
                                                                                
*/

/*                                                                              
                                                          
                                                                                
*/

/*                                                                              
                                                
                                                                                
*/
/*                   */
#define DISCONNECT_REASON_CODE_RESERVED         0
#define DISCONNECT_REASON_CODE_RADIO_LOST       1
#define DISCONNECT_REASON_CODE_DEAUTHENTICATED  2
#define DISCONNECT_REASON_CODE_DISASSOCIATED    3
#define DISCONNECT_REASON_CODE_NEW_CONNECTION   4
#define DISCONNECT_REASON_CODE_REASSOCIATION    5

/*                      */
#define TX_MODE_CCK             0x00
#define TX_MODE_OFDM            0x40
#define TX_MODE_HT_MM           0x80
#define TX_MODE_HT_GF           0xC0

#define RATE_CCK_SHORT_PREAMBLE 0x10
#define RATE_OFDM               0x20

#define PHY_RATE_1M             0x0
#define PHY_RATE_2M             0x1
#define PHY_RATE_5_5M           0x2
#define PHY_RATE_11M            0x3
#define PHY_RATE_6M             0xB
#define PHY_RATE_9M             0xF
#define PHY_RATE_12M            0xA
#define PHY_RATE_18M            0xE
#define PHY_RATE_24M            0x9
#define PHY_RATE_36M            0xD
#define PHY_RATE_48M            0x8
#define PHY_RATE_54M            0xC
#define PHY_RATE_MCS0           0x0
#define PHY_RATE_MCS1           0x1
#define PHY_RATE_MCS2           0x2
#define PHY_RATE_MCS3           0x3
#define PHY_RATE_MCS4           0x4
#define PHY_RATE_MCS5           0x5
#define PHY_RATE_MCS6           0x6
#define PHY_RATE_MCS7           0x7
#define PHY_RATE_MCS32          0x20

#define RATE_CCK_1M_LONG        (TX_MODE_CCK | PHY_RATE_1M)
#define RATE_CCK_2M_LONG        (TX_MODE_CCK | PHY_RATE_2M)
#define RATE_CCK_5_5M_LONG      (TX_MODE_CCK | PHY_RATE_5_5M)
#define RATE_CCK_11M_LONG       (TX_MODE_CCK | PHY_RATE_11M)
#define RATE_CCK_2M_SHORT       (TX_MODE_CCK | PHY_RATE_2M | RATE_CCK_SHORT_PREAMBLE)
#define RATE_CCK_5_5M_SHORT     (TX_MODE_CCK | PHY_RATE_5_5M | RATE_CCK_SHORT_PREAMBLE)
#define RATE_CCK_11M_SHORT      (TX_MODE_CCK | PHY_RATE_11M | RATE_CCK_SHORT_PREAMBLE)
#define RATE_OFDM_6M            (TX_MODE_OFDM | RATE_OFDM | PHY_RATE_6M)
#define RATE_OFDM_9M            (TX_MODE_OFDM | RATE_OFDM | PHY_RATE_9M)
#define RATE_OFDM_12M           (TX_MODE_OFDM | RATE_OFDM | PHY_RATE_12M)
#define RATE_OFDM_18M           (TX_MODE_OFDM | RATE_OFDM | PHY_RATE_18M)
#define RATE_OFDM_24M           (TX_MODE_OFDM | RATE_OFDM | PHY_RATE_24M)
#define RATE_OFDM_36M           (TX_MODE_OFDM | RATE_OFDM | PHY_RATE_36M)
#define RATE_OFDM_48M           (TX_MODE_OFDM | RATE_OFDM | PHY_RATE_48M)
#define RATE_OFDM_54M           (TX_MODE_OFDM | RATE_OFDM | PHY_RATE_54M)

#define RATE_MM_MCS_0           (TX_MODE_HT_MM | PHY_RATE_MCS0)
#define RATE_MM_MCS_1           (TX_MODE_HT_MM | PHY_RATE_MCS1)
#define RATE_MM_MCS_2           (TX_MODE_HT_MM | PHY_RATE_MCS2)
#define RATE_MM_MCS_3           (TX_MODE_HT_MM | PHY_RATE_MCS3)
#define RATE_MM_MCS_4           (TX_MODE_HT_MM | PHY_RATE_MCS4)
#define RATE_MM_MCS_5           (TX_MODE_HT_MM | PHY_RATE_MCS5)
#define RATE_MM_MCS_6           (TX_MODE_HT_MM | PHY_RATE_MCS6)
#define RATE_MM_MCS_7           (TX_MODE_HT_MM | PHY_RATE_MCS7)
#define RATE_MM_MCS_32          (TX_MODE_HT_MM | PHY_RATE_MCS32)

#define RATE_GF_MCS_0           (TX_MODE_HT_GF | PHY_RATE_MCS0)
#define RATE_GF_MCS_1           (TX_MODE_HT_GF | PHY_RATE_MCS1)
#define RATE_GF_MCS_2           (TX_MODE_HT_GF | PHY_RATE_MCS2)
#define RATE_GF_MCS_3           (TX_MODE_HT_GF | PHY_RATE_MCS3)
#define RATE_GF_MCS_4           (TX_MODE_HT_GF | PHY_RATE_MCS4)
#define RATE_GF_MCS_5           (TX_MODE_HT_GF | PHY_RATE_MCS5)
#define RATE_GF_MCS_6           (TX_MODE_HT_GF | PHY_RATE_MCS6)
#define RATE_GF_MCS_7           (TX_MODE_HT_GF | PHY_RATE_MCS7)
#define RATE_GF_MCS_32          (TX_MODE_HT_GF | PHY_RATE_MCS32)

#define RATE_TX_MODE_MASK                   BITS(6,7)
#define RATE_TX_MODE_OFFSET                 6
#define RATE_CODE_GET_TX_MODE(_ucRateCode)  ((_ucRateCode & RATE_TX_MODE_MASK) >> RATE_TX_MODE_OFFSET)
#define RATE_PHY_RATE_MASK                  BITS(0,5)
#define RATE_PHY_RATE_OFFSET                0
#define RATE_CODE_GET_PHY_RATE(_ucRateCode) ((_ucRateCode & RATE_PHY_RATE_MASK) >> RATE_PHY_RATE_OFFSET)
#define RATE_PHY_RATE_SHORT_PREAMBLE        BIT(4)
#define RATE_CODE_IS_SHORT_PREAMBLE(_ucRateCode)  ((_ucRateCode & RATE_PHY_RATE_SHORT_PREAMBLE)?TRUE:FALSE)


#define CHNL_LIST_SZ_2G         14
#define CHNL_LIST_SZ_5G         14

/*                                       */
#define CFG_STA_REC_NUM         20

/*                          */
#define PHY_TYPE_BIT_HR_DSSS    BIT(PHY_TYPE_HR_DSSS_INDEX) /*                         */
#define PHY_TYPE_BIT_ERP        BIT(PHY_TYPE_ERP_INDEX)     /*                     */
#define PHY_TYPE_BIT_OFDM       BIT(PHY_TYPE_OFDM_INDEX)    /*                            */
#define PHY_TYPE_BIT_HT         BIT(PHY_TYPE_HT_INDEX)      /*                    */


/*                          */
#define PHY_TYPE_SET_802_11ABGN (PHY_TYPE_BIT_OFDM | \
                                 PHY_TYPE_BIT_HR_DSSS | \
                                 PHY_TYPE_BIT_ERP | \
                                 PHY_TYPE_BIT_HT)

#define PHY_TYPE_SET_802_11BGN  (PHY_TYPE_BIT_HR_DSSS | \
                                 PHY_TYPE_BIT_ERP | \
                                 PHY_TYPE_BIT_HT)

#define PHY_TYPE_SET_802_11GN   (PHY_TYPE_BIT_ERP | \
                                 PHY_TYPE_BIT_HT)

#define PHY_TYPE_SET_802_11AN   (PHY_TYPE_BIT_OFDM | \
                                 PHY_TYPE_BIT_HT)

#define PHY_TYPE_SET_802_11ABG  (PHY_TYPE_BIT_OFDM | \
                                 PHY_TYPE_BIT_HR_DSSS | \
                                 PHY_TYPE_BIT_ERP)

#define PHY_TYPE_SET_802_11BG   (PHY_TYPE_BIT_HR_DSSS | \
                                 PHY_TYPE_BIT_ERP)

#define PHY_TYPE_SET_802_11A    (PHY_TYPE_BIT_OFDM)

#define PHY_TYPE_SET_802_11G    (PHY_TYPE_BIT_ERP)

#define PHY_TYPE_SET_802_11B    (PHY_TYPE_BIT_HR_DSSS)

#define PHY_TYPE_SET_802_11N    (PHY_TYPE_BIT_HT)


/*                          */
#define RATE_SET_BIT_1M         BIT(RATE_1M_INDEX)      /*           */
#define RATE_SET_BIT_2M         BIT(RATE_2M_INDEX)      /*           */
#define RATE_SET_BIT_5_5M       BIT(RATE_5_5M_INDEX)    /*             */
#define RATE_SET_BIT_11M        BIT(RATE_11M_INDEX)     /*            */
#define RATE_SET_BIT_22M        BIT(RATE_22M_INDEX)     /*            */
#define RATE_SET_BIT_33M        BIT(RATE_33M_INDEX)     /*            */
#define RATE_SET_BIT_6M         BIT(RATE_6M_INDEX)      /*           */
#define RATE_SET_BIT_9M         BIT(RATE_9M_INDEX)      /*           */
#define RATE_SET_BIT_12M        BIT(RATE_12M_INDEX)     /*            */
#define RATE_SET_BIT_18M        BIT(RATE_18M_INDEX)     /*            */
#define RATE_SET_BIT_24M        BIT(RATE_24M_INDEX)     /*             */
#define RATE_SET_BIT_36M        BIT(RATE_36M_INDEX)     /*             */
#define RATE_SET_BIT_48M        BIT(RATE_48M_INDEX)     /*             */
#define RATE_SET_BIT_54M        BIT(RATE_54M_INDEX)     /*             */
#define RATE_SET_BIT_HT_PHY     BIT(RATE_HT_PHY_INDEX)  /*                      */


/*                      */
#define RATE_SET_HR_DSSS            (RATE_SET_BIT_1M | \
                                     RATE_SET_BIT_2M | \
                                     RATE_SET_BIT_5_5M | \
                                     RATE_SET_BIT_11M)

#define RATE_SET_ERP                (RATE_SET_BIT_1M | \
                                     RATE_SET_BIT_2M | \
                                     RATE_SET_BIT_5_5M | \
                                     RATE_SET_BIT_11M | \
                                     RATE_SET_BIT_6M | \
                                     RATE_SET_BIT_9M | \
                                     RATE_SET_BIT_12M | \
                                     RATE_SET_BIT_18M | \
                                     RATE_SET_BIT_24M | \
                                     RATE_SET_BIT_36M | \
                                     RATE_SET_BIT_48M | \
                                     RATE_SET_BIT_54M)

#define RATE_SET_ERP_P2P            (RATE_SET_BIT_6M | \
                                     RATE_SET_BIT_9M | \
                                     RATE_SET_BIT_12M | \
                                     RATE_SET_BIT_18M | \
                                     RATE_SET_BIT_24M | \
                                     RATE_SET_BIT_36M | \
                                     RATE_SET_BIT_48M | \
                                     RATE_SET_BIT_54M)

#define RATE_SET_OFDM               (RATE_SET_BIT_6M | \
                                     RATE_SET_BIT_9M | \
                                     RATE_SET_BIT_12M | \
                                     RATE_SET_BIT_18M | \
                                     RATE_SET_BIT_24M | \
                                     RATE_SET_BIT_36M | \
                                     RATE_SET_BIT_48M | \
                                     RATE_SET_BIT_54M)

#define RATE_SET_HT                 (RATE_SET_ERP)
//                                                                                               


#define RATE_SET_ALL_ABG             RATE_SET_ERP

#define BASIC_RATE_SET_HR_DSSS      (RATE_SET_BIT_1M | \
                                     RATE_SET_BIT_2M)

#define BASIC_RATE_SET_HR_DSSS_ERP  (RATE_SET_BIT_1M | \
                                     RATE_SET_BIT_2M | \
                                     RATE_SET_BIT_5_5M | \
                                     RATE_SET_BIT_11M)

#define BASIC_RATE_SET_ERP          (RATE_SET_BIT_1M | \
                                     RATE_SET_BIT_2M | \
                                     RATE_SET_BIT_5_5M | \
                                     RATE_SET_BIT_11M | \
                                     RATE_SET_BIT_6M | \
                                     RATE_SET_BIT_12M | \
                                     RATE_SET_BIT_24M)

#define BASIC_RATE_SET_OFDM         (RATE_SET_BIT_6M | \
                                     RATE_SET_BIT_12M | \
                                     RATE_SET_BIT_24M)

#define BASIC_RATE_SET_ERP_P2P      (RATE_SET_BIT_6M | \
                                     RATE_SET_BIT_12M | \
                                     RATE_SET_BIT_24M)

#define INITIAL_RATE_SET_RCPI_100    RATE_SET_ALL_ABG

#define INITIAL_RATE_SET_RCPI_80    (RATE_SET_BIT_1M | \
                                     RATE_SET_BIT_2M | \
                                     RATE_SET_BIT_5_5M | \
                                     RATE_SET_BIT_11M | \
                                     RATE_SET_BIT_6M | \
                                     RATE_SET_BIT_9M | \
                                     RATE_SET_BIT_12M | \
                                     RATE_SET_BIT_24M)

#define INITIAL_RATE_SET_RCPI_60    (RATE_SET_BIT_1M | \
                                     RATE_SET_BIT_2M | \
                                     RATE_SET_BIT_5_5M | \
                                     RATE_SET_BIT_11M | \
                                     RATE_SET_BIT_6M)

#define INITIAL_RATE_SET(_rcpi)     (INITIAL_RATE_SET_ ## _rcpi)

#define RCPI_100                    100 /*         */
#define RCPI_80                     80  /*         */
#define RCPI_60                     60  /*         */


/*                                                                  */
#define MAX_NUM_RCPI_RECORDS        10

/*                                                                  */
#define NO_RCPI_RECORDS             -128
#define MAX_RCPI_DBM                0
#define MIN_RCPI_DBM                -100


#define MAC_TX_RESERVED_FIELD       0 /*                                     */

#define MAX_ASSOC_ID                (CFG_STA_REC_NUM)   /*                                    */


#define MAX_DEAUTH_INFO_COUNT       4       /*                             */
#define MIN_DEAUTH_INTERVAL_MSEC    500     /*                                                        */

/*                     */
#define AUTH_TYPE_OPEN_SYSTEM                       BIT(AUTH_ALGORITHM_NUM_OPEN_SYSTEM)
#define AUTH_TYPE_SHARED_KEY                        BIT(AUTH_ALGORITHM_NUM_SHARED_KEY)
#define AUTH_TYPE_FAST_BSS_TRANSITION               BIT(AUTH_ALGORITHM_NUM_FAST_BSS_TRANSITION)

/*                            */
#define TX_AUTH_ASSOCI_RETRY_LIMIT                  2// 
#define TX_AUTH_ASSOCI_RETRY_LIMIT_FOR_ROAMING      1

/*                                   */
#define ELEM_MAX_LEN_WMM_INFO       7

/*                                                                              
                                                 
                                                                                
*/
typedef UINT_16                     PHY_TYPE, *P_PHY_TYPE;
typedef UINT_8                      RCPI, *P_RCPI;
typedef UINT_8                      ALC_VAL, *P_ALC_VAL;

typedef enum _ENUM_HW_BSSID_T {
    BSSID_0 = 0,
    BSSID_1,
    BSSID_NUM
} ENUM_HW_BSSID_T;

typedef enum _ENUM_HW_MAC_ADDR_T {
    MAC_ADDR_0 = 0,
    MAC_ADDR_1,
    MAC_ADDR_NUM
} ENUM_HW_MAC_ADDR_T;

typedef enum _ENUM_HW_OP_MODE_T {
    HW_OP_MODE_STA = 0,
    HW_OP_MODE_AP,
    HW_OP_MODE_ADHOC,
    HW_OP_MODE_NUM
} ENUM_HW_OP_MODE_T;

typedef enum _ENUM_TSF_T {
    ENUM_LOCAL_TSF_0,
    ENUM_LOCAL_TSF_1,
    ENUM_LOCAL_TSF_NUM
} ENUM_LOCAL_TSF_T, *P_ENUM_LOCAL_TSF_T;

typedef enum _HAL_TS_HW_UPDATE_MODE {
    HAL_TSF_HW_UPDATE_BY_TICK_AND_RECEIVED_FRAME,
    HAL_TSF_HW_UPDATE_BY_TICK_ONLY,
    HAL_TSF_HW_UPDATE_BY_RECEIVED_FRAME_ONLY,
    HAL_TSF_HW_UPDATE_BY_TICK_AND_RECEIVED_FRAME_AD_HOC
} HAL_TSF_HW_UPDATE_MODE;


typedef enum _ENUM_AC_T {
    AC0 = 0,
    AC1,
    AC2,
    AC3,
    AC_NUM
} ENUM_AC_T, *P_ENUM_AC_T;


/*                                    */
typedef enum _ENUM_NETWORK_TYPE_INDEX_T {
    NETWORK_TYPE_AIS_INDEX = 0,
    NETWORK_TYPE_P2P_INDEX,
    NETWORK_TYPE_BOW_INDEX,
    NETWORK_TYPE_INDEX_NUM
} ENUM_NETWORK_TYPE_INDEX_T;


/*                       */
typedef enum _ENUM_STA_TYPE_INDEX_T {
    STA_TYPE_LEGACY_INDEX = 0,
    STA_TYPE_P2P_INDEX,
    STA_TYPE_BOW_INDEX,
    STA_TYPE_INDEX_NUM
}
ENUM_STA_TYPE_INDEX_T;

#define STA_ROLE_BASE_INDEX     4

typedef enum _ENUM_STA_ROLE_INDEX_T {
    STA_ROLE_ADHOC_INDEX = STA_ROLE_BASE_INDEX, // 
    STA_ROLE_CLIENT_INDEX,
    STA_ROLE_AP_INDEX,
	STA_ROLE_TDLS_INDEX,
	STA_ROLE_DLS_INDEX /*                                                */
} ENUM_STA_ROLE_INDEX_T;

/*                                       */
typedef enum _ENUM_PWR_STATE_T {
    PWR_STATE_IDLE = 0,
    PWR_STATE_ACTIVE,
    PWR_STATE_PS,
    PWR_STATE_NUM
} ENUM_PWR_STATE_T;

typedef enum _ENUM_PHY_TYPE_INDEX_T {
    //                                                                        
    PHY_TYPE_HR_DSSS_INDEX = 0, /*                         */
    PHY_TYPE_ERP_INDEX,         /*                     */
    PHY_TYPE_ERP_P2P_INDEX,     /*                                 */
    PHY_TYPE_OFDM_INDEX,        /*                            */
    PHY_TYPE_HT_INDEX,          /*                    */
    PHY_TYPE_INDEX_NUM //  
} ENUM_PHY_TYPE_INDEX_T, *P_ENUM_PHY_TYPE_INDEX_T;

typedef enum _ENUM_ACPI_STATE_T {
    ACPI_STATE_D0 = 0,
    ACPI_STATE_D1,
    ACPI_STATE_D2,
    ACPI_STATE_D3
} ENUM_ACPI_STATE_T;

/*                                          */
typedef enum _ENUM_OP_MODE_T {
    OP_MODE_INFRASTRUCTURE = 0,         /*                   */
    OP_MODE_IBSS,                       /*       */
    OP_MODE_ACCESS_POINT,               /*        */
    OP_MODE_P2P_DEVICE,                    /*            */
    OP_MODE_BOW,
    OP_MODE_NUM
} ENUM_OP_MODE_T, *P_ENUM_OP_MODE_T;

typedef enum _ENUM_CHNL_EXT_T {
    CHNL_EXT_SCN = 0,
    CHNL_EXT_SCA = 1,
    CHNL_EXT_RES = 2,
    CHNL_EXT_SCB = 3
} ENUM_CHNL_EXT_T, *P_ENUM_CHNL_EXT_T;

/*                                               */
typedef enum _ENUM_BAND_T {
    BAND_NULL,
    BAND_2G4,
    BAND_5G,
    BAND_NUM
} ENUM_BAND_T, *P_ENUM_BAND_T;

/*                                                                    */
typedef struct _RF_CHANNEL_INFO_T {
    ENUM_BAND_T     eBand;
    UINT_8          ucChannelNum;
} RF_CHANNEL_INFO_T, *P_RF_CHANNEL_INFO_T;

typedef enum _ENUM_RATE_INDEX_T {
    RATE_1M_INDEX = 0,      /*    */
    RATE_2M_INDEX,          /*    */
    RATE_5_5M_INDEX,        /*      */
    RATE_11M_INDEX,         /*     */
    RATE_22M_INDEX,         /*     */
    RATE_33M_INDEX,         /*     */
    RATE_6M_INDEX,          /*    */
    RATE_9M_INDEX,          /*    */
    RATE_12M_INDEX,         /*     */
    RATE_18M_INDEX,         /*     */
    RATE_24M_INDEX,         /*     */
    RATE_36M_INDEX,         /*     */
    RATE_48M_INDEX,         /*     */
    RATE_54M_INDEX,         /*     */
    RATE_HT_PHY_INDEX,      /*                       */
    RATE_NUM //   
} ENUM_RATE_INDEX_T, *P_ENUM_RATE_INDEX_T;

typedef enum _ENUM_HT_RATE_INDEX_T {
    HT_RATE_MCS0_INDEX = 0,
    HT_RATE_MCS1_INDEX,
    HT_RATE_MCS2_INDEX,
    HT_RATE_MCS3_INDEX,
    HT_RATE_MCS4_INDEX,
    HT_RATE_MCS5_INDEX,
    HT_RATE_MCS6_INDEX,
    HT_RATE_MCS7_INDEX,
    HT_RATE_MCS32_INDEX,
    HT_RATE_NUM //  
} ENUM_HT_RATE_INDEX_T, *P_ENUM_HT_RATE_INDEX_T;

typedef enum _ENUM_PREMABLE_OPTION_T {
    PREAMBLE_DEFAULT_LONG_NONE = 0, /*                                                   */
    PREAMBLE_OPTION_SHORT, /*                                                                     */
    PREAMBLE_HT_MIXED_MODE,
    PREAMBLE_HT_GREEN_FIELD,
    PREAMBLE_OPTION_NUM
} ENUM_PREMABLE_OPTION_T, *P_ENUM_PREMABLE_OPTION_T;

typedef enum _ENUM_MODULATION_SYSTEM_T {
    MODULATION_SYSTEM_CCK = 0,
    MODULATION_SYSTEM_OFDM,
    MODULATION_SYSTEM_HT20,
    MODULATION_SYSTEM_HT40,
    MODULATION_SYSTEM_NUM
} ENUM_MODULATION_SYSTEM_T, *P_ENUM_MODULATION_SYSTEM_T;

typedef enum _ENUM_MODULATION_TYPE_T {
    MODULATION_TYPE_CCK_BPSK = 0,
    MODULATION_TYPE_QPSK,
    MODULATION_TYPE_16QAM,
    MODULATION_TYPE_64QAM,
    MODULATION_TYPE_NUM
} ENUM_MODULATION_TYPE_T, *P_ENUM_MODULATION_TYPE_T;

typedef enum _ENUM_PS_FORWARDING_TYPE_T {
    PS_FORWARDING_TYPE_NON_PS = 0,
    PS_FORWARDING_TYPE_DELIVERY_ENABLED,
    PS_FORWARDING_TYPE_NON_DELIVERY_ENABLED,
    PS_FORWARDING_MORE_DATA_ENABLED,
    PS_FORWARDING_TYPE_NUM
} ENUM_PS_FORWARDING_TYPE_T, *P_ENUM_PS_FORWARDING_TYPE_T;

typedef struct _DEAUTH_INFO_T {
    UINT_8 aucRxAddr[MAC_ADDR_LEN];
    OS_SYSTIME rLastSendTime;
} DEAUTH_INFO_T, *P_DEAUTH_INFO_T;

/*                                                                            */
/*                                                                            */
/*                                                                            */
typedef VOID (*PFN_APPEND_IE_FUNC)(P_ADAPTER_T, P_MSDU_INFO_T);
typedef VOID (*PFN_HANDLE_IE_FUNC)(P_ADAPTER_T, P_SW_RFB_T, P_IE_HDR_T);
typedef VOID (*PFN_VERIFY_IE_FUNC)(P_ADAPTER_T, P_SW_RFB_T, P_IE_HDR_T, PUINT_16);
typedef UINT_32 (*PFN_CALCULATE_VAR_IE_LEN_FUNC)(P_ADAPTER_T, ENUM_NETWORK_TYPE_INDEX_T, P_STA_RECORD_T);

typedef struct _APPEND_IE_ENTRY_T {
    UINT_16             u2EstimatedIELen;
    PFN_APPEND_IE_FUNC  pfnAppendIE;
} APPEND_IE_ENTRY_T, *P_APPEND_IE_ENTRY_T;

typedef struct _APPEND_VAR_IE_ENTRY_T {
    UINT_16                         u2EstimatedFixedIELen; /*                  */
    PFN_CALCULATE_VAR_IE_LEN_FUNC   pfnCalculateVariableIELen;
    PFN_APPEND_IE_FUNC              pfnAppendIE;
} APPEND_VAR_IE_ENTRY_T, *P_APPEND_VAR_IE_ENTRY_T;

typedef struct _HANDLE_IE_ENTRY_T {
    UINT_8              ucElemID;
    PFN_HANDLE_IE_FUNC  pfnHandleIE;
} HANDLE_IE_ENTRY_T, *P_HANDLE_IE_ENTRY_T;

typedef struct _VERIFY_IE_ENTRY_T {
    UINT_8              ucElemID;
    PFN_VERIFY_IE_FUNC  pfnVarifyIE;
} VERIFY_IE_ENTRY_T, *P_VERIFY_IE_ENTRY_T;

/*                                                                            */
/*                                                                            */
/*                                                                            */
typedef enum _ENUM_PARAM_CONNECTION_POLICY_T {
    CONNECT_BY_SSID_BEST_RSSI = 0,
    CONNECT_BY_SSID_GOOD_RSSI_MIN_CH_LOAD,
    CONNECT_BY_SSID_ANY, /*                             */
    CONNECT_BY_BSSID,
    CONNECT_BY_CUSTOMIZED_RULE /*                  */
} ENUM_PARAM_CONNECTION_POLICY_T, *P_ENUM_PARAM_CONNECTION_POLICY_T;

typedef enum _ENUM_PARAM_PREAMBLE_TYPE_T {
    PREAMBLE_TYPE_LONG = 0,
    PREAMBLE_TYPE_SHORT,
    PREAMBLE_TYPE_AUTO                  /*                                                         */
} ENUM_PARAM_PREAMBLE_TYPE_T, *P_ENUM_PARAM_PREAMBLE_TYPE_T;

/*                                                                          */
typedef enum _ENUM_PARAM_PHY_CONFIG_T {
    PHY_CONFIG_802_11ABG = 0,           /*                                                                              */
    PHY_CONFIG_802_11BG,                /*                                                                             */
    PHY_CONFIG_802_11G,                 /*                                                                                 */
    PHY_CONFIG_802_11A,                 /*                                                                                   */
    PHY_CONFIG_802_11B,                 /*                                                                                 */
    PHY_CONFIG_802_11ABGN,              /*                                                      */
    PHY_CONFIG_802_11BGN,               /*                                                                              */
    PHY_CONFIG_802_11AN,                /*                                                                               */
    PHY_CONFIG_802_11GN,                /*                                                                             */
    PHY_CONFIG_NUM //  
} ENUM_PARAM_PHY_CONFIG_T, *P_ENUM_PARAM_PHY_CONFIG_T;

/*                                                    */
typedef enum _ENUM_PARAM_AP_MODE_T {
    AP_MODE_11B = 0,                /*                                                    */
    AP_MODE_MIXED_11BG,             /*                                                                   */
    AP_MODE_11G,                    /*                                                                 */
    AP_MODE_11G_P2P,                /*                                                                         */
    AP_MODE_11A,                    /*                                                */
    AP_MODE_NUM //  
} ENUM_PARAM_AP_MODE_T, *P_ENUM_PARAM_AP_MODE_T;


/*                                                                                       */
#define NETWORK_TYPE_AIS_MASK               BIT(NETWORK_TYPE_AIS_INDEX)
#define NETWORK_TYPE_P2P_MASK               BIT(NETWORK_TYPE_P2P_INDEX)
#define NETWORK_TYPE_BOW_MASK               BIT(NETWORK_TYPE_BOW_INDEX)
#define STA_TYPE_LEGACY_MASK                 BIT(STA_TYPE_LEGACY_INDEX)
#define STA_TYPE_P2P_MASK                       BIT(STA_TYPE_P2P_INDEX)
#define STA_TYPE_BOW_MASK                     BIT(STA_TYPE_BOW_INDEX)
#define STA_TYPE_ADHOC_MASK                 BIT(STA_ROLE_ADHOC_INDEX)
#define STA_TYPE_CLIENT_MASK                BIT(STA_ROLE_CLIENT_INDEX)
#define STA_TYPE_AP_MASK                    BIT(STA_ROLE_AP_INDEX)
#define STA_TYPE_DLS_MASK                   BIT(STA_ROLE_DLS_INDEX)
#define STA_TYPE_TDLS_MASK                  BIT(STA_ROLE_TDLS_INDEX)

/*                                                                                      */
#define IS_STA_IN_AIS(_prStaRec)        ((_prStaRec)->ucNetTypeIndex == NETWORK_TYPE_AIS_INDEX)
#define IS_STA_IN_P2P(_prStaRec)        ((_prStaRec)->ucNetTypeIndex ==  NETWORK_TYPE_P2P_INDEX)
#define IS_STA_IN_BOW(_prStaRec)        ((_prStaRec)->ucNetTypeIndex ==  NETWORK_TYPE_BOW_INDEX)
#define IS_STA_LEGACY_TYPE(_prStaRec)        ((_prStaRec->eStaType) & STA_TYPE_LEGACY_MASK)
#define IS_STA_P2P_TYPE(_prStaRec)        ((_prStaRec->eStaType) & STA_TYPE_P2P_MASK)
#define IS_STA_BOW_TYPE(_prStaRec)        ((_prStaRec->eStaType) & STA_TYPE_BOW_MASK)
#define IS_ADHOC_STA(_prStaRec)         ((_prStaRec->eStaType) & STA_TYPE_ADHOC_MASK)
#define IS_CLIENT_STA(_prStaRec)        ((_prStaRec->eStaType) & STA_TYPE_CLIENT_MASK)
#define IS_AP_STA(_prStaRec)            ((_prStaRec->eStaType) & STA_TYPE_AP_MASK)
#define IS_DLS_STA(_prStaRec)           ((_prStaRec->eStaType) & STA_TYPE_DLS_MASK)
#define IS_TDLS_STA(_prStaRec)          ((_prStaRec->eStaType) & STA_TYPE_TDLS_MASK)

/*                                                                                
                                                           
   */
typedef enum _ENUM_STA_TYPE_T {
    STA_TYPE_LEGACY_AP       = (STA_TYPE_LEGACY_MASK | STA_TYPE_AP_MASK),
    STA_TYPE_LEGACY_CLIENT = (STA_TYPE_LEGACY_MASK | STA_TYPE_CLIENT_MASK),
    STA_TYPE_ADHOC_PEER     = (STA_TYPE_LEGACY_MASK | STA_TYPE_ADHOC_MASK),
#if CFG_ENABLE_WIFI_DIRECT
    STA_TYPE_P2P_GO         = (STA_TYPE_P2P_MASK | STA_TYPE_AP_MASK),
    STA_TYPE_P2P_GC         = (STA_TYPE_P2P_MASK | STA_TYPE_CLIENT_MASK),
#endif
#if CFG_ENABLE_BT_OVER_WIFI
    STA_TYPE_BOW_AP         = (STA_TYPE_BOW_MASK | STA_TYPE_AP_MASK),
    STA_TYPE_BOW_CLIENT     = (STA_TYPE_BOW_MASK | STA_TYPE_CLIENT_MASK),
#endif
    STA_TYPE_DLS_PEER       = (STA_TYPE_LEGACY_MASK | STA_TYPE_DLS_MASK),
    STA_TYPE_TDLS_PEER		= (STA_TYPE_LEGACY_MASK | STA_TYPE_TDLS_MASK)
} ENUM_STA_TYPE_T, *P_ENUM_STA_TYPE_T;

/*                               */
typedef enum _ENUM_BSS_TYPE_T {
    BSS_TYPE_INFRASTRUCTURE = 1,
    BSS_TYPE_IBSS,
    BSS_TYPE_P2P_DEVICE,
    BSS_TYPE_BOW_DEVICE,
    BSS_TYPE_NUM
} ENUM_BSS_TYPE_T, *P_ENUM_BSS_TYPE_T;

/*                                                                            */
/*                                                                            */
/*                                                                            */
//                                               
//               
//      

#define MAX_NUM_SUPPORTED_CIPHER_SUITES 8     /*                                       */
#if CFG_SUPPORT_802_11W
#define MAX_NUM_SUPPORTED_AKM_SUITES    8     /*                                    */
#else
#define MAX_NUM_SUPPORTED_AKM_SUITES    6     /*                                    */
#endif

/*                              */
typedef struct _RSN_INFO_T {
    UINT_8          ucElemId;
    UINT_16         u2Version;
    UINT_32         u4GroupKeyCipherSuite;
    UINT_32         u4PairwiseKeyCipherSuiteCount;
    UINT_32         au4PairwiseKeyCipherSuite[MAX_NUM_SUPPORTED_CIPHER_SUITES];
    UINT_32         u4AuthKeyMgtSuiteCount;
    UINT_32         au4AuthKeyMgtSuite[MAX_NUM_SUPPORTED_AKM_SUITES];
    UINT_16         u2RsnCap;
    BOOLEAN         fgRsnCapPresent;
} /*                     */ RSN_INFO_T, *P_RSN_INFO_T;

#define MAX_NUM_SUPPORTED_WAPI_AKM_SUITES    1 /*                                    */
#define MAX_NUM_SUPPORTED_WAPI_CIPHER_SUITES 1 /*                                       */

/*                               */
typedef struct _WAPI_INFO_T {
    UINT_8          ucElemId;
    UCHAR           ucLength;
    UINT_16         u2Version;
    UINT_32         u4AuthKeyMgtSuiteCount;
    UINT_32         au4AuthKeyMgtSuite[MAX_NUM_SUPPORTED_WAPI_AKM_SUITES];
    UINT_32         u4PairwiseKeyCipherSuiteCount;
    UINT_32         au4PairwiseKeyCipherSuite[MAX_NUM_SUPPORTED_WAPI_CIPHER_SUITES];
    UINT_32         u4GroupKeyCipherSuite;
    UINT_16         u2WapiCap;
    UINT_16         u2Bkid;
    UINT_8          aucBkid[1][16];
} /*                      */ WAPI_INFO_T, *P_WAPI_INFO_T;

//                                               
//              
//      


#if CFG_ENABLE_WIFI_DIRECT

typedef struct _P2P_DEVICE_TYPE_T {
    UINT_16 u2CategoryID;
    UINT_16 u2SubCategoryID;
} P2P_DEVICE_TYPE_T, *P_P2P_DEVICE_TYPE_T;

typedef struct _P2P_DEVICE_DESC_T {
    LINK_ENTRY_T        rLinkEntry;
    BOOLEAN             fgDevInfoValid;
    UINT_8              aucDeviceAddr[MAC_ADDR_LEN]; /*                 */
    UINT_8              aucInterfaceAddr[MAC_ADDR_LEN]; /*                    */
    UINT_8              ucDeviceCapabilityBitmap;
    UINT_8              ucGroupCapabilityBitmap;
    UINT_16             u2ConfigMethod; /*                           */
    P2P_DEVICE_TYPE_T   rPriDevType;
    UINT_8              ucSecDevTypeNum;
    P2P_DEVICE_TYPE_T   arSecDevType[8];   //                                                  
    UINT_16             u2NameLength;
    UINT_8              aucName[32];                //                                           
    //                                               
} P2P_DEVICE_DESC_T, *P_P2P_DEVICE_DESC_T;

#endif


/*                                                                              
                                                  
                                                                                
*/

/*                                                                              
                                                   
                                                                                
*/
static const UINT_8 aucRateIndex2RateCode[PREAMBLE_OPTION_NUM][RATE_NUM] = {
    { /*               */
        RATE_CCK_1M_LONG,       /*                   */
        RATE_CCK_2M_LONG,       /*               */
        RATE_CCK_5_5M_LONG,     /*                 */
        RATE_CCK_11M_LONG,      /*                */
        RATE_CCK_1M_LONG,       /*                                */
        RATE_CCK_1M_LONG,       /*                                */
        RATE_OFDM_6M,           /*               */
        RATE_OFDM_9M,           /*               */
        RATE_OFDM_12M,          /*                */
        RATE_OFDM_18M,          /*                */
        RATE_OFDM_24M,          /*                */
        RATE_OFDM_36M,          /*                */
        RATE_OFDM_48M,          /*                */
        RATE_OFDM_54M,           /*                */
    },
    { /*                */
        RATE_CCK_1M_LONG,       /*                   */
        RATE_CCK_2M_SHORT,      /*               */
        RATE_CCK_5_5M_SHORT,    /*                 */
        RATE_CCK_11M_SHORT,     /*                */
        RATE_CCK_1M_LONG,       /*                                */
        RATE_CCK_1M_LONG,       /*                                */
        RATE_OFDM_6M,           /*               */
        RATE_OFDM_9M,           /*               */
        RATE_OFDM_12M,          /*                */
        RATE_OFDM_18M,          /*                */
        RATE_OFDM_24M,          /*                */
        RATE_OFDM_36M,          /*                */
        RATE_OFDM_48M,          /*                */
        RATE_OFDM_54M,           /*                */
    },
    { /*                    */
        RATE_MM_MCS_0,               /*                  */
        RATE_MM_MCS_1,               /*                  */
        RATE_MM_MCS_2,               /*                  */
        RATE_MM_MCS_3,               /*                  */
        RATE_MM_MCS_4,               /*                  */
        RATE_MM_MCS_5,               /*                  */
        RATE_MM_MCS_6,               /*                  */
        RATE_MM_MCS_7,               /*                  */
        RATE_MM_MCS_32               /*                   */
    },
    { /*                     */
        RATE_GF_MCS_0,               /*                  */
        RATE_GF_MCS_1,               /*                  */
        RATE_GF_MCS_2,               /*                  */
        RATE_GF_MCS_3,               /*                  */
        RATE_GF_MCS_4,               /*                  */
        RATE_GF_MCS_5,               /*                  */
        RATE_GF_MCS_6,               /*                  */
        RATE_GF_MCS_7,               /*                  */
        RATE_GF_MCS_32               /*                   */
    }
};

static const UINT_8 aucRateTableSize[PREAMBLE_OPTION_NUM] = {
    RATE_HT_PHY_INDEX,
    RATE_HT_PHY_INDEX,
    HT_RATE_NUM,
    HT_RATE_NUM
};


/*                                                                              
                                             
                                                                                
*/
/*                                                                            
           */
#define WLAN_GET_FIELD_16(_memAddr_p, _value_p) \
        { \
            PUINT_8 __cp = (PUINT_8) (_memAddr_p); \
            *(PUINT_16)(_value_p) = ((UINT_16) __cp[0]) | ((UINT_16) __cp[1] << 8); \
        }

#define WLAN_GET_FIELD_BE16(_memAddr_p, _value_p) \
        { \
            PUINT_8 __cp = (PUINT_8) (_memAddr_p); \
            *(PUINT_16)(_value_p) = ((UINT_16) __cp[0] << 8) | ((UINT_16) __cp[1]); \
        }

#define WLAN_GET_FIELD_32(_memAddr_p, _value_p) \
        { \
            PUINT_8 __cp = (PUINT_8) (_memAddr_p); \
            *(PUINT_32)(_value_p) = ((UINT_32) __cp[0])       | ((UINT_32) __cp[1] << 8) | \
                                    ((UINT_32) __cp[2] << 16) | ((UINT_32) __cp[3] << 24); \
        }

#define WLAN_GET_FIELD_64(_memAddr_p, _value_p) \
        { \
            PUINT_8 __cp = (PUINT_8) (_memAddr_p); \
            *(PUINT_64)(_value_p) = \
                ((UINT_64) __cp[0])       | ((UINT_64) __cp[1] << 8)  | \
                ((UINT_64) __cp[2] << 16) | ((UINT_64) __cp[3] << 24) | \
                ((UINT_64) __cp[4] << 32) | ((UINT_64) __cp[5] << 40) | \
                ((UINT_64) __cp[6] << 48) | ((UINT_64) __cp[7] << 56); \
        }

#define WLAN_SET_FIELD_16(_memAddr_p, _value) \
        { \
            PUINT_8 __cp = (PUINT_8) (_memAddr_p); \
            __cp[0] = (UINT_8) (_value); \
            __cp[1] = (UINT_8) ((_value) >> 8); \
        }

#define WLAN_SET_FIELD_BE16(_memAddr_p, _value) \
       { \
            PUINT_8 __cp = (PUINT_8) (_memAddr_p); \
            __cp[0] = (UINT_8) ((_value) >> 8); \
            __cp[1] = (UINT_8) (_value); \
       }

#define WLAN_SET_FIELD_32(_memAddr_p, _value) \
        { \
            PUINT_8 __cp = (PUINT_8) (_memAddr_p); \
            __cp[0] = (UINT_8) (_value); \
            __cp[1] = (UINT_8) ((_value) >> 8); \
            __cp[2] = (UINT_8) ((_value) >> 16); \
            __cp[3] = (UINT_8) ((_value) >> 24); \
        }



/*                                                                              
                                                             
                                                                                
*/


/*                                                                              
                                                
                                                                                
*/

#endif /*             */

