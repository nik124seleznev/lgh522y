
//                                 
//                
#define LVDS_OUTPUT_CTRL   0x0818
  #define RG_LVDSRX_FIFO_EN  0x80000000  //                       
  #define RG_DPMODE          0x00000008  //                            
  #define RG_SYNC_TRIG_MODE  0x00000004  //                                     
  #define RG_OUT_FIFO_EN     0x00000002  //                           
  #define RG_LVDS_E          0x00000001  //                              
      
#define LVDS_CLK_CTRL      0x0820
  #define RG_TEST_CK_SEL2    0x00000400  //                                 
  #define RG_TEST_CK_SEL1    0x00000200  //                                 
  #define RG_TEST_CK_SEL0    0x00000100  //                                  
  #define RG_TEST_CK_EN      0x00000004  //                                    
  #define RG_RX_CK_EN        0x00000002  //                       
  #define RG_TX_CK_EN        0x00000001  //                       

#define LVDS_CLK_RESET     0x082c
  #define RG_CTSCLK_RESET_B  0x00000002  //                           
  #define RG_PCLK_RESET_B 	 0x00000001  //                        

typedef enum
{
	  LCD_DATA_FORMAT_VESA8BIT = 0,
	  LCD_DATA_FORMAT_VESA6BIT = 1,
	  LCD_DATA_FORMAT_DISM8BIT = 2
} LCD_DATA_FMT;


//                                  
#define LVDS_FMT_CTRL      0x0800
  #define RG_8BIT_FORMAT     0x00000000  //                                                   
  #define RG_6BIT_FORMAT     0x00000010  //                                                   
  #define RG_DE_INV          0x00000004  //                   
  #define RG_VSYNC_INV       0x00000002  //                      
  #define RG_HSYNC_INV       0x00000001  //                      

//                                                      
#define LVDS_R_SEL         0x080c
  #define RG_R_SEL_VESA      0x00000000  //             
  #define RG_R_SEL_DISM      0x00492492  //             

#define LVDS_G_SEL         0x0810
  #define RG_G_SEL_VESA      0x00000000  //             
  #define RG_G_SEL_DISM      0x00492492  //             

#define LVDS_B_SEL         0x0814
  #define RG_B_SEL_VESA      0x00000000  //             
  #define RG_B_SEL_DISM      0x00492492  //             

//                          
#define LVDS_RG_HV_TOTAL   0x0908
  #define LVDS_PTGEN_V_TOTAL   0x027b0000  //                                            
  #define LVDS_PTGEN_H_TOTAL   0x00000540  //                                           
    
#define LVDS_RG_HV_WIDTH   0x090c
  #define LVDS_PTGEN_V_WIDTH   0x00050000  //                                            
  #define LVDS_PTGEN_H_WIDTH   0x00000080  //                                           

#define LVDS_RG_HV_START   0x0910
  #define LVDS_PTGEN_V_START   0x00150000  //                                            
  #define LVDS_PTGEN_H_START   0x00000118  //                                           
  
#define LVDS_RG_HV_ACTIVE  0x0914
  #define LVDS_PTGEN_V_ACTIVE  0x02580000  //                                             
  #define LVDS_PTGEN_H_ACTIVE  0x00000400  //                                            

#define LVDS_RG_PTGEN_CTRL 0x0918
  #define LVDS_COLOR_BAR_TH    0x04000000  //                                               
  #define LVDS_PTGEN_TYPE      0x00000200  //                                               
  #define LVDS_TST_PAT_EN      0x00000001  //                      

#define LVDS_RG_PTGEN_BD   0x091c
  #define LVDS_PTGEN_BD_B      0x00200000  //                                                 
  #define LVDS_PTGEN_BD_G      0x00002000  //                                                
  #define LVDS_PTGEN_BD_R      0x000000ff  //                                               

#define LVDS_RG_PTGEN_DATA 0x0920
  #define LVDS_PTGEN_B         0x00ff0000  //                                      
  #define LVDS_PTGEN_G         0x00002000  //                                     
  #define LVDS_PTGEN_R         0x00000020  //                                    

//                                 
#define LVDS_CRC_CTRL      0x0904
  #define RG_CRC_CLR           0x00000002  //                                      
  #define RG_CRC_START         0x00000001  //                                      

#define LVDS_TX_CRC_STATUS 0x0934

//                      
#define LVDS_RG_TST_CH     0x0830
  #define RG_LVDS_CH2        0x00a00000  //                                            
  #define RG_LVDS_CH1        0x00002800  //                                            
  #define RG_LVDS_CH0        0x0000000a  //                                          
  
#define LVDS_RG_TST_CLK    0x0834
  #define RG_LVDS_PAT_EN     0x80000000  //                                                
  #define RG_LVDS_CLK        0x00002800  //                                                
  #define RG_LVDS_CH3        0x0000000a  //                                          

//                            
#define LVDS_RG_SRC        0x0804
  #define RG_B               0x00000000  //                               
  #define RG_G               0x00000000  //                             
  #define RG_R               0x00000000  //                            
  #define RG_B_SEL           0x00000000  //                                     
  #define RG_G_SEL           0x00000000  //                                    
  #define RG_R_SEL           0x00000000  //                                   

#define LVDS_RG_CTRL       0x0808
  #define RG_DE              0x00000000  //                          
  #define RG_VSYNC           0x00000000  //                             
  #define RG_HSYNC           0x00000000  //                             
  #define RG_DE_SEL          0x00000000  //                                             
  #define RG_VSYNC_SEL       0x00000000  //                                                 
  #define RG_HSYNC_SEL       0x00000000  //                                                

//                              
#define LVDS_CH_SWAP       0x081c
  #define RG_SWAP_SEL        0x80000000  //                             
  #define RG_TOP_PN          0x00000000  //                        
  #define RG_ML_SWAP         0x00000000  //                            
  #define RG_CLK_SEL         0x00000000  //                                   
  #define RG_CH3_SEL         0x00000000  //                              
  #define RG_CH2_SEL         0x00000000  //                             
  #define RG_CH1_SEL         0x00000000  //                             
  #define RG_CH0_SEL         0x00000000  //                             


//                                
//                
#define DGI0_DEC_CTRL      0x0400
  #define FIFO_WRITE_EN      0x00008000  //                      
  #define RESET_COUNTER      0x08000000  //                                          
  #define CLEAR_COUNTER      0x00000000
	
#define DGI0_FIFO_CTRL     0x0404
  #define SW_RST             0x00080000  //               
  #define FIFO_RESET_ON      0x00020000  //                      
  #define RD_START           0x00000040  //     
  
#define DGI0_DATA_OUT_CTRL 0x0408
  #define DATA_OUT_SWAP      0x04000000  //                                         
  #define TTL_TIM_SWAP	     0x00003000  //                                      
  #define TTL_TIM_SWAP2	     0x00005000  //                                      

#define DGI0_DITHER_CTRL0  0x0410
  #define FRC_EN             0x10000000  //                      
  #define FCNT_DIF_EN        0x00000800  //                        
  #define SYNC_SEL           0x00000200  //                                                          
  #define OUT_FMT            0x00000010  //                                             
  
#define DGI0_DITHER_CTRL1  0x0414  
#define DGI0_DITHER_CTRL2  0x0418

#define DGI0_TG_CTRL00     0x041c
  #define PRGS_OUT           0x00104000  //                    
  
#define DGI0_TG_CTRL01     0x0420
  #define RG_VSYNC_FORWARD   0x80000000  //                                  
  #define RG_VSYNC_DELAY     0x00020000  //                      
  #define RG_HSYNC_DELAY     0x000003a8  //                       
  
#define DGI0_TG_CTRL02     0x0424
  #define VSYNC_TOTAL        0x020d0000  //                      
  #define HSYNC_TOTAL        0x000003aa  //                       
                      
#define DGI0_TG_CTRL03     0x0428
  #define VSYNC_WIDTH        0x00060000  //                   
  #define HSYNC_WIDTH        0x0000003e  //                  

#define DGI0_TG_CTRL04     0x042c
  #define H_ACT2_EN          0x00000400  //                               
  #define V_ACT2_EN          0x00000200  //                            
  #define HD_ON              0x00000100  //         
  #define VSYNC_POL          0x00000040  //                  
  #define HSYNC_POL          0x00000020  //                  
  #define DE_POL             0x00000010  //               
  
#define DGI0_TG_CTRL05     0x0430
  #define X_ACTIVE_START     0x007a0000  //                              
  #define X_ACTIVE_END       0x00000399  //                           
  
#define DGI0_TG_CTRL06     0x0434
  #define Y_ACTIVE_OSTART    0x00240000  //                                
  #define Y_ACTIVE_OEND      0x00000203  //                             
  
#define DGI0_TG_CTRL07     0x0438
  #define Y_ACTIVE_ESTART    0x00240000  //                                 
  #define Y_ACTIVE_EEND      0x00000203  //                              

#define DGI0_TG_CTRL08     0x043c
  #define X_ACTIVE_START_1   0x007a0000  //                                
  #define X_ACTIVE_END_1     0x00000399  //                             
  
#define DGI0_TG_CTRL09     0x0440
  #define Y_ACTIVE_OSTART_1  0x00240000  //                                  
  #define Y_ACTIVE_OEND_1    0x00000203  //                               
  
#define DGI0_TG_CTRL10     0x0444
  #define Y_ACTIVE_ESTART_1  0x00240000  //                                   
  #define Y_ACTIVE_EEND_1    0x00000203  //                                
  
#define DGI0_ANAIF_CTRL2   0x0448
  #define HDMIPLL_REG_CLK_SEL 0x00000000  //                                                                          
  #define DGI1_DEL_D2_D4_SEL 0x00000000  //                                                                                
  #define DGI1_DEL_D1_SEL    0x00000000  //                                                                                  
  #define DGI0_DEL_D2_D4_SEL 0x00000000  //                                                                                
  #define DGI0_DEL_D1_SEL    0x00000000  //                                                                                
  
#define DGI0_ANAIF_CTRL0   0x044c
  #define DGI0_CLK_DELAY_SEL1  0x0000000  //                                       
  #define DGI0_CLK_DELAY_SEL0  0x0000000  //                                       
  #define DGI0_CK_INV_PRE_CTRL 0x0000000  //                         
  #define DGI0_PAD_CLK_ENABLE  0x0000001  //                         
  #define DGI0_PAD_CLK_DISABLE 0x0000000  //                          
  
#define DGI0_ANAIF_CTRL1   0x0450
  #define DGI1_PAD_CLK_INV_EN  0x0000000  //                 
  #define DGI1_CLK_DELAY_SEL1  0x0000000  //                                       
  #define DGI1_CLK_DELAY_SEL0  0x0000000  //                                       
  #define DATA_IN_TV_MODE      0x0000000  //                   
  #define DATA_IN_BIT_INV      0x0000000  //                   
  #define ANAIF_DGI1_CLK_SEL   0x0000000  //                                                         
  #define CLK_SEL_TV_MODE      0x0000000  //                    
  #define CLK_MODE_SEL         0x0000000  //                     
  #define NWEB_CLK_EN          0x0000000  //                     
  #define DGI1_PAD_CLK_EN      0x0000000  //                       
  #define TV_MODE_CLK_EN       0x0000000  //                        
  
  
#define DGI0_TTL_ANAIF_CTRL 0x0454
  #define TTL_CLK_TEST_MODE  0x00000000  //                                                                    
  #define TTL_CLK_DELAY_SEL1 0x00000000  //                                      
  #define TTL_CLK_DELAY_SEL0 0x00000000  //                                     
  #define TTL_CLK_INV_ENABLE 0x00000001  //                           
  
#define DGI0_TTL_ANAIF_CTRL1 0x0458
  #define PAD_TTL_EN_PP        0x00000002  //                                     
  #define PAD_TTL_EN_FUN_SEL   0x00000001  //                                            
       
#define DGI0_CLK_RST_CTRL  0x045c
  #define DGI0_TEST_MODE         0x80000000  //                           
  #define CLK_OUT_TO_IN_INV      0x00000000  //                                       
  #define CLK_OUT_TO_IN          0x00000000  //                                   
  #define CLK_PAT_GEN_EN         0x00000008  //                        
  #define DGI0_CLK_OUT_ENABLE    0x00000004  //                       
  #define DGI0_CLK_IN_INV_ENABLE 0x00000002  //                          
  #define DGI0_CLK_IN_ENABLE     0x00000001  //                      
  #define DGI0_CLK_OUT_DISABLE   0x00000000  //                        

#define DGI0_PAT_GEN_CTRL0 0x0500
  #define RG_PTGEN_V_TOTAL   0x027b0000  //                                                
  #define RG_PTGEN_H_TOTAL   0x00000540  //                                            
  
#define DGI0_PAT_GEN_CTRL1 0x0504
  #define RG_PTGEN_V_WIDTH   0x00050000  //                                                
  #define RG_PTGEN_H_WIDTH   0x00000080  //                                           
  
#define DGI0_PAT_GEN_CTRL2 0x0508
  #define RG_PTGEN_V_START   0x00150000  //                                                
  #define RG_PTGEN_H_START   0x00000118  //                                           
  
#define DGI0_PAT_GEN_CTRL3 0x050c
  #define RG_PTGEN_V_ACTIVE  0x02580000  //                                                 
  #define RG_PTGEN_H_ACTIVE  0x00000400  //                                            
        
#define DGI0_PAT_GEN_CTRL4 0x0510
  #define RG_COLOR_BAR_TH	 0x04000000  //                                               
  #define RG_PTGEN_TYPE      0x00000200  //                                               
  #define PAT_GEN_RST        0x00000004  //                 
  #define PAT_IN             0x00000002  //                                
  #define RG_TST_PAT_EN      0x00000001  //                      
  
#define DGI0_PAT_GEN_CTRL5 0x0514
  #define RG_PTGEN_BD_B      0x00ff0000  //                                                 
  #define RG_PTGEN_BD_G      0x00008800  //                                                
  #define RG_PTGEN_BD_R      0x00000044  //                                               
  
#define DGI0_PAT_GEN_CTRL6 0x0518
  #define RG_PTGEN_B         0x00ff0000  //                                      
  #define RG_PTGEN_G         0x00008800  //                                     
  #define RG_PTGEN_R         0x00000044  //                                    
  
#define DGI0_CRC_MON_CT    0x051c
  #define C_CRC_CLR          0x00000002
  #define C_CRC_START        0x00000001
  
#define DGI0_CRC_OUT       0x0520
  #define CRC_RDY            0x10000000
  #define CRC_OUT            0x00ffffff
  
#define DGI0_MON           0x0524

//             
#define REG_LVDS_DISP_CKCFG  0x1014
#define REG_LVDS_CTSCLKCFG   0x1018
#define REG_LVDS_PWR_RST_B   0x1108
#define REG_LVDS_PWR_CTRL    0x110c

#define RG_LVDSWRAP_CTRL1  0x1254
  #define RG_DCXO_POR_MON_EN 0x00000100  //                      
  #define RG_PLL1_DIV        0x00000004  //                 
  #define RG_PLL1_DIV2 	     0x00000002  //                 
  #define RG_PLL1_DIV3	     0x00000006  //                 

#define REG_LVDS_ANACFG0   0x1310
  #define RG_LVDS_APD        0xf8000000  //       
  #define RG_LVDS_BIASA_PD   0x02000000  //    
  #define RG_LVDS_ATERM_EN   0x00800000  //                                                                                                          
  #define RG_LVDS_APSRC      0x00400000  //                                                                     
  #define RG_LVDS_ANSRC      0x00070000  //                                                                     
  #define RG_LVDS_ATVCM      0x0000c000  //                                               
  #define RG_LVDS_ATVO       0x00002000  //                                 

#define REG_LVDS_ANACFG1   0x1314
  #define RG_LVDS_AE4        0x80000000  //                                                    
  #define RG_LVDS_AE8        0x40000000  //                                                    
  #define RG_LVDS_ASR        0x20000000  //                                        
  #define RG_LVDS_ASMTEN     0x10000000  //                                           
  #define RG_LVDS_AMINI_SEL_CK0 0x00000000  //                                       
  
#define REG_LVDS_ANACFG2   0x1318 
  #define RG_VPLL_BC         0x30000000  //                              
  #define RG_VPLL_BIC        0x02000000  //                                        
  #define RG_VPLL_BIR        0x00200000  //                                             
  #define RG_VPLL_BP         0x00010000  //                                
  #define RG_VPLL_BG_PD      0x00008000  //                          
  #define RG_VPLL_BR         0x00005000  //                                  
  #define RG_VPLL_BIAS_PD    0x00000800  //                             

#define REG_LVDS_ANACFG3   0x131c
  #define RG_VPLL_DIV        0x00040000  //                      
  #define RG_VPLL_DPIX_CKSEL 0x00001000  //                              
  #define RG_LVDS_DELAY      0x00000080  //                                             
  #define RG_VPLL_MKVCO      0x00000040  //                                     
  #define RG_VPLL_POSTDIV_EN 0x00000010  //   

#define REG_LVDS_ANACFG4   0x1320
  #define RG_VPLL_RST        0x00800000  //    
  #define RG_T2TTLO_EN       0x00000800  //    
  #define RG_VPLL_PD         0x00000400  //      
  #define RG_BYPASS          0x00000200  //                                        
  #define RG_LVDS_BYPASS     0x00000100  //   
  
#define REG_PLL_GPANACFG0  0x134c
  #define RG_PLL1_EN         0x80000000  //                                        
  #define RG_PLL1_FBDIV      0x4c000000  //                                           
  #define RG_PLL1_FBDIV2     0x2c000000  // 
  #define RG_PLL1_FBDIV3     0x6c000000  //  
  #define RG_PLL1_PREDIV     0x01000000  //                         
  #define RG_PLL1_RST_DLY    0x00300000  //                                                  
  #define RG_PLL1_LF         0x00000800  //                          
  #define RG_PLL1_MONCKEN    0x00000100  //                            
  #define RG_PLL1_VODEN      0x00000080  //                       
  #define RG_NFIPLL_EN       0x00000002  //                                       
  
#define REG_PLL_GPANACFG2              0x1354
#define PLLGP_ANACFG2_PLLGP_BIAS_EN       (1U<<20)
  
