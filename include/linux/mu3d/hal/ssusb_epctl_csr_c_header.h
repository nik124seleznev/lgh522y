/*                                     */

#define U3D_DEVICE_CONF                           (SSUSB_EPCTL_CSR_BASE+0x0000)
#define U3D_EP_RST                                (SSUSB_EPCTL_CSR_BASE+0x0004)
#define U3D_USB3_ERDY_TIMING_PARAMETER            (SSUSB_EPCTL_CSR_BASE+0x0008)
#define U3D_USB3_EPCTRL_CAP                       (SSUSB_EPCTL_CSR_BASE+0x000C)
#define U3D_USB2_ISOINEP_INCOMP_INTR              (SSUSB_EPCTL_CSR_BASE+0x0010)
#define U3D_USB2_ISOOUTEP_INCOMP_ERR              (SSUSB_EPCTL_CSR_BASE+0x0014)
#define U3D_ISO_UNDERRUN_INTR                     (SSUSB_EPCTL_CSR_BASE+0x0018)
#define U3D_ISO_OVERRUN_INTR                      (SSUSB_EPCTL_CSR_BASE+0x001C)
#define U3D_USB2_RX_EP_DATAERR_INTR               (SSUSB_EPCTL_CSR_BASE+0x0020)
#define U3D_USB2_EPCTRL_CAP                       (SSUSB_EPCTL_CSR_BASE+0x0024)
#define U3D_USB2_EPCTL_LPM                        (SSUSB_EPCTL_CSR_BASE+0x0028)
#define U3D_USB3_SW_ERDY                          (SSUSB_EPCTL_CSR_BASE+0x0030)
#define U3D_EP_FLOW_CTRL                          (SSUSB_EPCTL_CSR_BASE+0x0040)
#define U3D_USB3_EP_ACT                           (SSUSB_EPCTL_CSR_BASE+0x0044)
#define U3D_USB3_EP_PACKET_PENDING                (SSUSB_EPCTL_CSR_BASE+0x0048)
#define U3D_DEV_LINK_INTR_ENABLE                  (SSUSB_EPCTL_CSR_BASE+0x0050)
#define U3D_DEV_LINK_INTR                         (SSUSB_EPCTL_CSR_BASE+0x0054)
#define U3D_USB2_EPCTL_LPM_FC_CHK                 (SSUSB_EPCTL_CSR_BASE+0x0060)
#define U3D_DEVICE_MONITOR                        (SSUSB_EPCTL_CSR_BASE+0x0064)

/*                                  */

//               
#define DEV_ADDR                                  (0x7f<<24) //     
#define HW_USB2_3_SEL                             (0x1<<18) //     
#define SW_USB2_3_SEL_EN                          (0x1<<17) //     
#define SW_USB2_3_SEL                             (0x1<<16) //     
#define SSUSB_DEV_SPEED                           (0x7<<0) //   

//          
#define EP_IN_RST                                 (0x7fff<<17) //     
#define EP_OUT_RST                                (0x7fff<<1) //    
#define EP0_RST                                   (0x1<<0) //   

//                              
#define ERDY_TIMEOUT_VALUE                        (0x3ff<<0) //   

//                   
#define TX_NUMP_0_EN                              (0x1<<4) //   
#define SEND_STALL_CLR_PP_EN                      (0x1<<3) //   
#define USB3_ISO_CRC_CHK_DIS                      (0x1<<2) //   
#define SET_EOB_EN                                (0x1<<1) //   
#define TX_BURST_EN                               (0x1<<0) //   

//                            
#define USB2_ISOINEP_INCOMP_INTR_EN               (0x7fff<<17) //     
#define USB2_ISOINEP_INCOMP_INTR                  (0x7fff<<1) //    

//                            
#define USB2_ISOOUTEP_INCOMP_INTR_EN              (0x7fff<<17) //     
#define USB2_ISOOUTEP_INCOMP_INTR                 (0x7fff<<1) //    

//                     
#define ISOIN_UNDERRUN_INTR_EN                    (0x7fff<<17) //     
#define ISOIN_UNDERRUN_INTR                       (0x7fff<<1) //    

//                    
#define ISOOUT_OVERRUN_INTR_EN                    (0x7fff<<17) //     
#define ISOOUT_OVERRUN_INTR                       (0x7fff<<1) //    

//                           
#define USB2_RX_EP_DATAERR_INTR_EN                (0xffff<<16) //     
#define USB2_RX_EP_DATAERR_INTR                   (0xffff<<0) //    

//                   
#define USB2_ISO_CRC_CHK_DIS                      (0x1<<0) //   

//                  
#define L1_EXIT_EP_OUT_CHK                        (0x7fff<<17) //     
#define L1_EXIT_EP_IN_CHK                         (0x7fff<<1) //    
#define L1_EXIT_EP0_CHK                           (0x1<<0) //   

//                
#define SW_ERDY_EP_NUM                            (0xf<<2) //   
#define SW_ERDY_EP_DIR                            (0x1<<1) //   
#define SW_SEND_ERDY                              (0x1<<0) //   

//                
#define EP_OUT_FC                                 (0xffff<<16) //     
#define EP_IN_FC                                  (0xffff<<0) //    

//               
#define EP_IN_ACT                                 (0xffff<<0) //    

//                          
#define EP_OUT_PP                                 (0xffff<<16) //     
#define EP_IN_PP                                  (0xffff<<0) //    

//                        
#define SSUSB_DEV_SPEED_CHG_INTR_EN               (0x1<<0) //   

//                 
#define SSUSB_DEV_SPEED_CHG_INTR                  (0x1<<0) //   

//                         
#define L1_EXIT_EP_OUT_FC_CHK                     (0x7fff<<17) //     
#define L1_EXIT_EP_IN_FC_CHK                      (0x7fff<<1) //    
#define L1_EXIT_EP0_FC_CHK                        (0x1<<0) //   

//                  
#define CUR_DEV_ADDR                              (0x7f<<0) //   


/*                                         */

//               
#define DEV_ADDR_OFST                             (24)
#define HW_USB2_3_SEL_OFST                        (18)
#define SW_USB2_3_SEL_EN_OFST                     (17)
#define SW_USB2_3_SEL_OFST                        (16)
#define SSUSB_DEV_SPEED_OFST                      (0)

//          
#define EP_IN_RST_OFST                            (17)
#define EP_OUT_RST_OFST                           (1)
#define EP0_RST_OFST                              (0)

//                              
#define ERDY_TIMEOUT_VALUE_OFST                   (0)

//                   
#define SEND_STALL_CLR_PP_EN_OFST                 (3)
#define USB3_ISO_CRC_CHK_DIS_OFST                 (2)
#define SET_EOB_EN_OFST                           (1)
#define TX_BURST_EN_OFST                          (0)

//                            
#define USB2_ISOINEP_INCOMP_INTR_EN_OFST          (17)
#define USB2_ISOINEP_INCOMP_INTR_OFST             (1)

//                            
#define USB2_ISOOUTEP_INCOMP_INTR_EN_OFST         (17)
#define USB2_ISOOUTEP_INCOMP_INTR_OFST            (1)

//                     
#define ISOIN_UNDERRUN_INTR_EN_OFST               (17)
#define ISOIN_UNDERRUN_INTR_OFST                  (1)

//                    
#define ISOOUT_OVERRUN_INTR_EN_OFST               (17)
#define ISOOUT_OVERRUN_INTR_OFST                  (1)

//                           
#define USB2_RX_EP_DATAERR_INTR_EN_OFST           (16)
#define USB2_RX_EP_DATAERR_INTR_OFST              (0)

//                   
#define USB2_ISO_CRC_CHK_DIS_OFST                 (0)

//                  
#define L1_EXIT_EP_OUT_CHK_OFST                   (17)
#define L1_EXIT_EP_IN_CHK_OFST                    (1)
#define L1_EXIT_EP0_CHK_OFST                      (0)

//                
#define SW_ERDY_EP_NUM_OFST                       (2)
#define SW_ERDY_EP_DIR_OFST                       (1)
#define SW_SEND_ERDY_OFST                         (0)

//                
#define EP_OUT_FC_OFST                            (16)
#define EP_IN_FC_OFST                             (0)

//               
#define EP_IN_ACT_OFST                            (0)

//                          
#define EP_OUT_PP_OFST                            (16)
#define EP_IN_PP_OFST                             (0)

//                        
#define SSUSB_DEV_SPEED_CHG_INTR_EN_OFST          (0)

//                 
#define SSUSB_DEV_SPEED_CHG_INTR_OFST             (0)

//                         
#define L1_EXIT_EP_OUT_FC_CHK_OFST                (17)
#define L1_EXIT_EP_IN_FC_CHK_OFST                 (1)
#define L1_EXIT_EP0_FC_CHK_OFST                   (0)

//                  
#define CUR_DEV_ADDR_OFST                         (0)

//                                                                    
