/*                                        */

#define U3D_TS_CONFIG                             (SSUSB_USB3_MAC_CSR_BASE+0x0000)
#define U3D_PIPE                                  (SSUSB_USB3_MAC_CSR_BASE+0x0004)
#define U3D_LTSSM_PARAMETER                       (SSUSB_USB3_MAC_CSR_BASE+0x000C)
#define U3D_LTSSM_CTRL                            (SSUSB_USB3_MAC_CSR_BASE+0x0010)
#define U3D_LTSSM_INFO                            (SSUSB_USB3_MAC_CSR_BASE+0x0014)
#define U3D_USB3_CONFIG                           (SSUSB_USB3_MAC_CSR_BASE+0x001C)
#define U3D_USB3_U1_STATE_INFO                    (SSUSB_USB3_MAC_CSR_BASE+0x0050)
#define U3D_USB3_U2_STATE_INFO                    (SSUSB_USB3_MAC_CSR_BASE+0x0054)
#define U3D_UX_LFPS_TIMING_PARAMETER              (SSUSB_USB3_MAC_CSR_BASE+0x007C)
#define U3D_U1_LFPS_TIMING_PARAMETER_2            (SSUSB_USB3_MAC_CSR_BASE+0x0080)
#define U3D_U2_LB_LFPS_TIMING_PARAMETER_1         (SSUSB_USB3_MAC_CSR_BASE+0x0084)
#define U3D_U2_LB_LFPS_TIMING_PARAMETER_2         (SSUSB_USB3_MAC_CSR_BASE+0x0088)
#define U3D_U3_LFPS_TIMING_PARAMETER_1            (SSUSB_USB3_MAC_CSR_BASE+0x008C)
#define U3D_U3_LFPS_TIMING_PARAMETER_2            (SSUSB_USB3_MAC_CSR_BASE+0x0090)
#define U3D_PING_LFPS_TIMING_PARAMETER            (SSUSB_USB3_MAC_CSR_BASE+0x0094)
#define U3D_POLLING_LFPS_TIMING_PARAMETER_1       (SSUSB_USB3_MAC_CSR_BASE+0x0098)
#define U3D_POLLING_LFPS_TIMING_PARAMETER_2       (SSUSB_USB3_MAC_CSR_BASE+0x009C)
#define U3D_UX_EXIT_LFPS_TIMING_PARAMETER         (SSUSB_USB3_MAC_CSR_BASE+0x00A0)
#define U3D_P3_TIMING_PARAMETER                   (SSUSB_USB3_MAC_CSR_BASE+0x00A4)
#define U3D_WARM_RESET_TIMING_PARAMETER           (SSUSB_USB3_MAC_CSR_BASE+0x00A8)
#define U3D_UX_EXIT_TIMING_PARAMETER              (SSUSB_USB3_MAC_CSR_BASE+0x00AC)
#define U3D_REF_CK_PARAMETER                      (SSUSB_USB3_MAC_CSR_BASE+0x00B0)
#define U3D_LTSSM_TIMING_PARAMETER_1              (SSUSB_USB3_MAC_CSR_BASE+0x010C)
#define U3D_LTSSM_TIMING_PARAMETER_2              (SSUSB_USB3_MAC_CSR_BASE+0x0110)
#define U3D_LTSSM_TIMING_PARAMETER_3              (SSUSB_USB3_MAC_CSR_BASE+0x0114)
#define U3D_LTSSM_TIMING_PARAMETER_4              (SSUSB_USB3_MAC_CSR_BASE+0x0118)
#define U3D_LTSSM_TIMING_PARAMETER_5              (SSUSB_USB3_MAC_CSR_BASE+0x011C)
#define U3D_LTSSM_RXDETECT_CTRL                   (SSUSB_USB3_MAC_CSR_BASE+0x0120)
#define U3D_PIPE_RXDATA_ERR_INTR_ENABLE           (SSUSB_USB3_MAC_CSR_BASE+0x0128)
#define U3D_PIPE_RXDATA_ERR_INTR                  (SSUSB_USB3_MAC_CSR_BASE+0x012C)
#define U3D_PIPE_LATCH_SELECT                     (SSUSB_USB3_MAC_CSR_BASE+0x0130)
#define U3D_LINK_STATE_MACHINE                    (SSUSB_USB3_MAC_CSR_BASE+0x0134)
#define U3D_MAC_FAST_SIMULATION                   (SSUSB_USB3_MAC_CSR_BASE+0x0138)
#define U3D_LTSSM_INTR_ENABLE                     (SSUSB_USB3_MAC_CSR_BASE+0x013C)
#define U3D_LTSSM_INTR                            (SSUSB_USB3_MAC_CSR_BASE+0x0140)
#define U3D_SKP_CNT                               (SSUSB_USB3_MAC_CSR_BASE+0x0148)

/*                                     */

//             
#define TS_CONFIG_DISABLE_SCRAMBLING              (0x1<<0) //   

//        
#define CP_TXDEEMPH_WITHOUT                       (0x3<<6) //   
#define CP_TXDEEMPH_WITH                          (0x3<<4) //   
#define CP_TXDEEMPH                               (0x3<<2) //   
#define PIPE_TXDEEMPH                             (0x3<<0) //   

//                   
#define DISABLE_NUM                               (0xf<<24) //     
#define RXDETECT_NUM                              (0x1f<<16) //     
#define TX_TSEQ_NUM                               (0xffff<<0) //    

//              
#define FORCE_POLLING_FAIL                        (0x1<<4) //   
#define FORCE_RXDETECT_FAIL                       (0x1<<3) //   
#define SOFT_U3_EXIT_EN                           (0x1<<2) //   
#define COMPLIANCE_EN                             (0x1<<1) //   
#define U1_GO_U2_EN                               (0x1<<0) //   

//              
#define CLR_PWR_CHG_TMOUT_FLAG                    (0x1<<26) //     
#define CLR_DISABLE_CNT                           (0x1<<25) //     
#define CLR_RXDETECT_CNT                          (0x1<<24) //     
#define PWR_CHG_TMOUT_FLAG                        (0x1<<16) //     
#define DISABLE_CNT                               (0xf<<8) //    
#define RXDETECT_CNT                              (0x1f<<0) //   

//               
#define USB3_EN                                   (0x1<<0) //   

//                      
#define CLR_USB3_U1_CNT                           (0x1<<16) //     
#define USB3_U1_CNT                               (0xffff<<0) //    

//                      
#define CLR_USB3_U2_CNT                           (0x1<<16) //     
#define USB3_U2_CNT                               (0xffff<<0) //    

//                            
#define UX_EXIT_T12_T11_MINUS_300NS               (0xff<<0) //   

//                              
#define U1_EXIT_NO_LFPS_TMOUT                     (0xfff<<16) //     
#define U1_EXIT_T13_T11                           (0xff<<8) //    
#define U1_EXIT_T12_T10                           (0xff<<0) //   

//                                 
#define U2_LB_EXIT_T13_T11                        (0xfff<<16) //     
#define U2_LB_EXIT_T12_T10                        (0xfff<<0) //    

//                                 
#define U2_LB_EXIT_NO_LFPS_TMOUT                  (0xfff<<0) //    

//                              
#define U3_EXIT_T13_T11                           (0x3fff<<16) //     
#define U3_EXIT_T12_T10                           (0x3fff<<0) //    

//                              
#define SEND_U3_EXIT_LFPS_WAIT_CYCLE              (0xff<<16) //     
#define U3_EXIT_NO_LFPS_TMOUT                     (0x3fff<<0) //    

//                              
#define TX_PING_LFPS_TBURST                       (0x3f<<16) //     
#define RX_PING_LFPS_TBURST_MAX                   (0x3f<<8) //    
#define RX_PING_LFPS_TBURST_MIN                   (0xf<<0) //   

//                                   
#define RX_POLLING_LFPS_TBURST_MAX                (0xff<<8) //    
#define RX_POLLING_LFPS_TBURST_MIN                (0x7f<<0) //   

//                                   
#define TX_POLLING_LFPS_NUM                       (0x1f<<16) //     
#define TX_POLLING_LFPS_TREPEAT                   (0xf<<8) //    
#define TX_POLLING_LFPS_TBURST                    (0xff<<0) //   

//                                 
#define RX_UX_EXIT_LFPS_REF                       (0xff<<8) //    
#define RX_UX_EXIT_LFPS_PIPE                      (0xff<<0) //   

//                       
#define P3_ENTER_CYCLE                            (0xf<<20) //     
#define P3_EXIT_CYCLE                             (0xf<<16) //     

//                               
#define TRESET_TBURST                             (0xff<<8) //    
#define TRESETDELAY                               (0x3f<<0) //   

//                            
#define UX_EXIT_TIMER                             (0xfffff<<0) //    

//                    
#define REF_1000NS                                (0xff<<0) //   

//                            
#define MAC_6MS                                   (0xf<<16) //     
#define MAC_2MS                                   (0xff<<0) //   

//                            
#define MAC_100MS                                 (0xff<<16) //     
#define MAC_12MS                                  (0x1f<<0) //   

//                            
#define MAC_360MS                                 (0x3ff<<16) //     
#define MAC_300MS                                 (0x3ff<<0) //   

//                            
#define MAC_200MS                                 (0xff<<0) //   

//                            
#define POWER_CHANGE_TIMEOUT_VALUE                (0x3ff<<16) //     
#define RXDETECT_TIMEOUT_VALUE                    (0x3ff<<0) //   

//                       
#define RXDETECT_WAIT_TIME                        (0xff<<8) //    
#define RXDETECT_WAIT_EN                          (0x1<<0) //   

//                               
#define DEC_8B10B_ERR_INTR_EN                     (0x1<<2) //   
#define DISPARITY_ERR_INTR_EN                     (0x1<<1) //   
#define EBUF_ERR_INTR_EN                          (0x1<<0) //   

//                        
#define DEC_8B10B_ERR_INTR                        (0x1<<2) //   
#define DISPARITY_ERR_INTR                        (0x1<<1) //   
#define EBUF_ERR_INTR                             (0x1<<0) //   

//                     
#define TX_SIGNAL_SEL                             (0x3<<2) //   
#define RX_SIGNAL_SEL                             (0x3<<0) //   

//                      
#define VBUS_DBC_CYCLE                            (0xffff<<16) //     
#define VBUS_VALID                                (0x1<<8) //   
#define LTSSM                                     (0x1f<<0) //   

//                       
#define FORCE_U0_TO_U3                            (0x1<<5) //   
#define FORCE_U0_TO_U2                            (0x1<<4) //   
#define FORCE_U0_TO_U1                            (0x1<<3) //   
#define MAC_SPEED_MS_TO_US                        (0x1<<2) //   
#define BYPASS_WARM_RESET                         (0x1<<1) //   

//                     
#define U3_RESUME_INTR_EN                         (0x1<<18) //     
#define U3_LFPS_TMOUT_INTR_EN                     (0x1<<17) //     
#define VBUS_FALL_INTR_EN                         (0x1<<16) //     
#define VBUS_RISE_INTR_EN                         (0x1<<15) //     
#define RXDET_SUCCESS_INTR_EN                     (0x1<<14) //     
#define EXIT_U3_INTR_EN                           (0x1<<13) //     
#define EXIT_U2_INTR_EN                           (0x1<<12) //     
#define EXIT_U1_INTR_EN                           (0x1<<11) //     
#define ENTER_U3_INTR_EN                          (0x1<<10) //     
#define ENTER_U2_INTR_EN                          (0x1<<9) //   
#define ENTER_U1_INTR_EN                          (0x1<<8) //   
#define ENTER_U0_INTR_EN                          (0x1<<7) //   
#define RECOVERY_INTR_EN                          (0x1<<6) //   
#define WARM_RST_INTR_EN                          (0x1<<5) //   
#define HOT_RST_INTR_EN                           (0x1<<4) //   
#define LOOPBACK_INTR_EN                          (0x1<<3) //   
#define COMPLIANCE_INTR_EN                        (0x1<<2) //   
#define SS_DISABLE_INTR_EN                        (0x1<<1) //   
#define SS_INACTIVE_INTR_EN                       (0x1<<0) //   

//              
#define U3_RESUME_INTR                            (0x1<<18) //     
#define U3_LFPS_TMOUT_INTR                        (0x1<<17) //     
#define VBUS_FALL_INTR                            (0x1<<16) //     
#define VBUS_RISE_INTR                            (0x1<<15) //     
#define RXDET_SUCCESS_INTR                        (0x1<<14) //     
#define EXIT_U3_INTR                              (0x1<<13) //     
#define EXIT_U2_INTR                              (0x1<<12) //     
#define EXIT_U1_INTR                              (0x1<<11) //     
#define ENTER_U3_INTR                             (0x1<<10) //     
#define ENTER_U2_INTR                             (0x1<<9) //   
#define ENTER_U1_INTR                             (0x1<<8) //   
#define ENTER_U0_INTR                             (0x1<<7) //   
#define RECOVERY_INTR                             (0x1<<6) //   
#define WARM_RST_INTR                             (0x1<<5) //   
#define HOT_RST_INTR                              (0x1<<4) //   
#define LOOPBACK_INTR                             (0x1<<3) //   
#define COMPLIANCE_INTR                           (0x1<<2) //   
#define SS_DISABLE_INTR                           (0x1<<1) //   
#define SS_INACTIVE_INTR                          (0x1<<0) //   

//           
#define SKP_SYMBOL_NUM                            (0x7f<<0) //   


/*                                            */

//             
#define TS_CONFIG_DISABLE_SCRAMBLING_OFST         (0)

//        
#define CP_TXDEEMPH_WITHOUT_OFST                  (6)
#define CP_TXDEEMPH_WITH_OFST                     (4)
#define CP_TXDEEMPH_OFST                          (2)
#define PIPE_TXDEEMPH_OFST                        (0)

//                   
#define DISABLE_NUM_OFST                          (24)
#define RXDETECT_NUM_OFST                         (16)
#define TX_TSEQ_NUM_OFST                          (0)

//              
#define FORCE_POLLING_FAIL_OFST                   (4)
#define FORCE_RXDETECT_FAIL_OFST                  (3)
#define SOFT_U3_EXIT_EN_OFST                      (2)
#define COMPLIANCE_EN_OFST                        (1)
#define U1_GO_U2_EN_OFST                          (0)

//              
#define CLR_PWR_CHG_TMOUT_FLAG_OFST               (26)
#define CLR_DISABLE_CNT_OFST                      (25)
#define CLR_RXDETECT_CNT_OFST                     (24)
#define PWR_CHG_TMOUT_FLAG_OFST                   (16)
#define DISABLE_CNT_OFST                          (8)
#define RXDETECT_CNT_OFST                         (0)

//               
#define USB3_EN_OFST                              (0)

//                      
#define CLR_USB3_U1_CNT_OFST                      (16)
#define USB3_U1_CNT_OFST                          (0)

//                      
#define CLR_USB3_U2_CNT_OFST                      (16)
#define USB3_U2_CNT_OFST                          (0)

//                            
#define UX_EXIT_T12_T11_MINUS_300NS_OFST          (0)

//                              
#define U1_EXIT_NO_LFPS_TMOUT_OFST                (16)
#define U1_EXIT_T13_T11_OFST                      (8)
#define U1_EXIT_T12_T10_OFST                      (0)

//                                 
#define U2_LB_EXIT_T13_T11_OFST                   (16)
#define U2_LB_EXIT_T12_T10_OFST                   (0)

//                                 
#define U2_LB_EXIT_NO_LFPS_TMOUT_OFST             (0)

//                              
#define U3_EXIT_T13_T11_OFST                      (16)
#define U3_EXIT_T12_T10_OFST                      (0)

//                              
#define SEND_U3_EXIT_LFPS_WAIT_CYCLE_OFST         (16)
#define U3_EXIT_NO_LFPS_TMOUT_OFST                (0)

//                              
#define TX_PING_LFPS_TBURST_OFST                  (16)
#define RX_PING_LFPS_TBURST_MAX_OFST              (8)
#define RX_PING_LFPS_TBURST_MIN_OFST              (0)

//                                   
#define RX_POLLING_LFPS_TBURST_MAX_OFST           (8)
#define RX_POLLING_LFPS_TBURST_MIN_OFST           (0)

//                                   
#define TX_POLLING_LFPS_NUM_OFST                  (16)
#define TX_POLLING_LFPS_TREPEAT_OFST              (8)
#define TX_POLLING_LFPS_TBURST_OFST               (0)

//                                 
#define RX_UX_EXIT_LFPS_REF_OFST                  (8)
#define RX_UX_EXIT_LFPS_PIPE_OFST                 (0)

//                       
#define P3_ENTER_CYCLE_OFST                       (20)
#define P3_EXIT_CYCLE_OFST                        (16)

//                               
#define TRESET_TBURST_OFST                        (8)
#define TRESETDELAY_OFST                          (0)

//                            
#define UX_EXIT_TIMER_OFST                        (0)

//                    
#define REF_1000NS_OFST                           (0)

//                            
#define MAC_6MS_OFST                              (16)
#define MAC_2MS_OFST                              (0)

//                            
#define MAC_100MS_OFST                            (16)
#define MAC_12MS_OFST                             (0)

//                            
#define MAC_360MS_OFST                            (16)
#define MAC_300MS_OFST                            (0)

//                            
#define MAC_200MS_OFST                            (0)

//                            
#define POWER_CHANGE_TIMEOUT_VALUE_OFST           (16)
#define RXDETECT_TIMEOUT_VALUE_OFST               (0)

//                       
#define RXDETECT_WAIT_TIME_OFST                   (8)
#define RXDETECT_WAIT_EN_OFST                     (0)

//                               
#define DEC_8B10B_ERR_INTR_EN_OFST                (2)
#define DISPARITY_ERR_INTR_EN_OFST                (1)
#define EBUF_ERR_INTR_EN_OFST                     (0)

//                        
#define DEC_8B10B_ERR_INTR_OFST                   (2)
#define DISPARITY_ERR_INTR_OFST                   (1)
#define EBUF_ERR_INTR_OFST                        (0)

//                     
#define TX_SIGNAL_SEL_OFST                        (2)
#define RX_SIGNAL_SEL_OFST                        (0)

//                      
#define VBUS_DBC_CYCLE_OFST                       (16)
#define VBUS_VALID_OFST                           (8)
#define LTSSM_OFST                                (0)

//                       
#define FORCE_U0_TO_U3_OFST                       (5)
#define FORCE_U0_TO_U2_OFST                       (4)
#define FORCE_U0_TO_U1_OFST                       (3)
#define MAC_SPEED_MS_TO_US_OFST                   (2)
#define BYPASS_WARM_RESET_OFST                    (1)

//                     
#define U3_RESUME_INTR_EN_OFST                    (18)
#define U3_LFPS_TMOUT_INTR_EN_OFST                (17)
#define VBUS_FALL_INTR_EN_OFST                    (16)
#define VBUS_RISE_INTR_EN_OFST                    (15)
#define RXDET_SUCCESS_INTR_EN_OFST                (14)
#define EXIT_U3_INTR_EN_OFST                      (13)
#define EXIT_U2_INTR_EN_OFST                      (12)
#define EXIT_U1_INTR_EN_OFST                      (11)
#define ENTER_U3_INTR_EN_OFST                     (10)
#define ENTER_U2_INTR_EN_OFST                     (9)
#define ENTER_U1_INTR_EN_OFST                     (8)
#define ENTER_U0_INTR_EN_OFST                     (7)
#define RECOVERY_INTR_EN_OFST                     (6)
#define WARM_RST_INTR_EN_OFST                     (5)
#define HOT_RST_INTR_EN_OFST                      (4)
#define LOOPBACK_INTR_EN_OFST                     (3)
#define COMPLIANCE_INTR_EN_OFST                   (2)
#define SS_DISABLE_INTR_EN_OFST                   (1)
#define SS_INACTIVE_INTR_EN_OFST                  (0)

//              
#define U3_RESUME_INTR_OFST                       (18)
#define U3_LFPS_TMOUT_INTR_OFST                   (17)
#define VBUS_FALL_INTR_OFST                       (16)
#define VBUS_RISE_INTR_OFST                       (15)
#define RXDET_SUCCESS_INTR_OFST                   (14)
#define EXIT_U3_INTR_OFST                         (13)
#define EXIT_U2_INTR_OFST                         (12)
#define EXIT_U1_INTR_OFST                         (11)
#define ENTER_U3_INTR_OFST                        (10)
#define ENTER_U2_INTR_OFST                        (9)
#define ENTER_U1_INTR_OFST                        (8)
#define ENTER_U0_INTR_OFST                        (7)
#define RECOVERY_INTR_OFST                        (6)
#define WARM_RST_INTR_OFST                        (5)
#define HOT_RST_INTR_OFST                         (4)
#define LOOPBACK_INTR_OFST                        (3)
#define COMPLIANCE_INTR_OFST                      (2)
#define SS_DISABLE_INTR_OFST                      (1)
#define SS_INACTIVE_INTR_OFST                     (0)

//           
#define SKP_SYMBOL_NUM_OFST                       (0)

//                                                                    
