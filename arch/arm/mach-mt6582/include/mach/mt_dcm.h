#ifndef _MT_DCM_H
#define _MT_DCM_H

#include "mach/mt_reg_base.h"


#if 1
//                                               
//                                                
//                                                
//                                                

//                                                
//                                                                        

//               
//                                                                 
//                                                                 
//                                                                 

//                                                                            

//                                                                     
//                                                                          
//                                                                        
//                                                                           
//                                                                                 

//                                                                              
//                                                                              
//                                                                              
//                                                                      
#define CAM_BASE                	0xF5004000//          
//                                                                  

//                                                                       
//                                                                 
//                                                                       

#endif

#if 1


//                
#define	PERI_USB0_DCM		    	(USB_BASE+0x700)//        

//                
#define MSDC0_IP_DCM				(MSDC_0_BASE + 0x00B4)//        

//                
#define MSDC1_IP_DCM				(MSDC_1_BASE + 0x00B4)//        

//                
#define MSDC2_IP_DCM				(MSDC_2_BASE + 0x00B4)//        

//                     
#define PMIC_WRAP_DCM_EN			(PWRAP_BASE+0x13C)//        


//               
#define I2C0_I2CREG_HW_CG_EN		((I2C0_BASE+0x054))//        

//               
#define I2C1_I2CREG_HW_CG_EN		((I2C1_BASE+0x054))//        

//               
#define I2C2_I2CREG_HW_CG_EN		((I2C2_BASE+0x054))//        


//          
#define CA7_MISC_CONFIG			(MCUSYS_CFGREG_BASE + 0x005C) //         
#define MCU_BIU_CON				(MCUSYS_CFGREG_BASE + 0x8000)


//            
//            
#define DCM_CFG                 (INFRA_BASE + 0x0004)//                 
#define CLK_SCP_CFG_0			(INFRA_BASE + 0x0200)//                   
#define CLK_SCP_CFG_1			(INFRA_BASE + 0x0204)//                   

//       
#define TOP_CKDIV1				(INFRACFG_AO_BASE + 0x008) //                         
#define TOP_DCMCTL              (INFRACFG_AO_BASE + 0x0010) //                               
#define TOP_DCMDBC              (INFRACFG_AO_BASE + 0x0014) //                               

//         
//                                                                                 

//         
#define INFRA_DCMCTL            (INFRACFG_AO_BASE + 0x0050) //                                
#define INFRA_DCMDBC            (INFRACFG_AO_BASE + 0x0054) //                                
#define INFRA_DCMFSEL           (INFRACFG_AO_BASE + 0x0058) //                                 


//        
#define PERI_GLOBALCON_DCMCTL        (PERICFG_BASE + 0x0050) //                               
#define PERI_GLOBALCON_DCMDBC        (PERICFG_BASE + 0x0054) //                               
#define PERI_GLOBALCON_DCMFSEL       (PERICFG_BASE + 0x0058) //                                


#define DRAMC_PD_CTRL           (DRAMC0_BASE + 0x01DC) //                               

//       
#define MMU_DCM					(SMI_MMU_TOP_BASE+0x5f0)//                  

//              
//                                                          


//              
#define SMI_DCM_CONTROL				0xF4011300//                              


//               
//              
#define SMI_COMMON_AO_SMI_CON		(SMI1_BASE+0x010)//                 
#define SMI_COMMON_AO_SMI_CON_SET	(SMI1_BASE+0x014)//                     
#define SMI_COMMON_AO_SMI_CON_CLR	(SMI1_BASE+0x018)//                     



//                    
#define SMILARB0_DCM_STA        (SMI_LARB0_BASE + 0x00)//                        
#define SMILARB0_DCM_CON        (SMI_LARB0_BASE + 0x10)//                       
#define SMILARB0_DCM_SET        (SMI_LARB0_BASE + 0x14)//                           
#define SMILARB0_DCM_CLR        (SMI_LARB0_BASE + 0x18)//                           

#define SMILARB1_DCM_STA        (SMI_LARB1_BASE + 0x00)//                        
#define SMILARB1_DCM_CON        (SMI_LARB1_BASE + 0x10)//                       
#define SMILARB1_DCM_SET        (SMI_LARB1_BASE + 0x14)//                           
#define SMILARB1_DCM_CLR        (SMI_LARB1_BASE + 0x18)//                           

#define SMILARB2_DCM_STA        (SMI_LARB3_BASE + 0x00)//                        
#define SMILARB2_DCM_CON        (SMI_LARB3_BASE + 0x10)//                       
#define SMILARB2_DCM_SET        (SMI_LARB3_BASE + 0x14)//                           
#define SMILARB2_DCM_CLR        (SMI_LARB3_BASE + 0x18)//                           


//   
//       
//                   
#define MFG_DCM_CON_0            (G3D_CONFIG_BASE + 0x10) //                       

//           
#define CAM_CTL_RAW_DCM         (CAM_BASE + 0x190)//                             
#define CAM_CTL_RGB_DCM         (CAM_BASE + 0x194)//                             
#define CAM_CTL_YUV_DCM         (CAM_BASE + 0x198)//                             
#define CAM_CTL_CDP_DCM         (CAM_BASE + 0x19C)//                             
#define CAM_CTL_DMA_DCM			(CAM_BASE + 0x1B0)//                             

#define CAM_CTL_RAW_DCM_STA     (CAM_BASE + 0x1A0)//                                
#define CAM_CTL_RGB_DCM_STA     (CAM_BASE + 0x1A4)//                                
#define CAM_CTL_YUV_DCM_STA     (CAM_BASE + 0x1A8)//                                
#define CAM_CTL_CDP_DCM_STA     (CAM_BASE + 0x1AC)//                                
#define CAM_CTL_DMA_DCM_STA     (CAM_BASE + 0x1B4)//                                


//                                                                  
#define JPGENC_DCM_CTRL         (JPGENC_BASE + 0x300) //                               

//                                                        
//                                                      
//                                                      

//           
//         
//                        
#define DISP_HW_DCM_DIS0        (DISPSYS_BASE + 0x120)//                           
#define DISP_HW_DCM_DIS_SET0    (DISPSYS_BASE + 0x124)//                               
#define DISP_HW_DCM_DIS_CLR0    (DISPSYS_BASE + 0x128)//                               

#define DISP_HW_DCM_DIS1        (DISPSYS_BASE + 0x12C)//                           
#define DISP_HW_DCM_DIS_SET1    (DISPSYS_BASE + 0x130)//                               
#define DISP_HW_DCM_DIS_CLR1    (DISPSYS_BASE + 0x134)//                               

//        
#define VENC_CE                 (VENC_BASE + 0xEC)//                               
#define VENC_CLK_DCM_CTRL       (VENC_BASE + 0xF4)//                               
#define VENC_CLK_CG_CTRL        (VENC_BASE + 0x94)//                               
//                                          

//    
//        
#define VDEC_DCM_CON            (VDEC_GCON_BASE + 0x18)//                                    


#else
//            
#define DCM_CFG                 (TOPRGU_BASE + 0x0104)

//       
#define TOP_DCMCTL              (INFRACFG_BASE + 0x0010)
#define TOP_DCMDBC              (INFRACFG_BASE + 0x0014)
#define TOP_CA7DCMFSEL          (INFRACFG_BASE + 0x0018)

//         
#define INFRA_DCMCTL            (INFRACFG_BASE + 0x0050)
#define INFRA_DCMDBC            (INFRACFG_BASE + 0x0054)
#define INFRA_DCMFSEL           (INFRACFG_BASE + 0x0058)

#define DRAMC_PD_CTRL           (DRAMC0_BASE + 0x01DC)

//        
#define PERI_GCON_DCMCTL        (PERICFG_BASE + 0x0050)
#define PERI_GCON_DCMDBC        (PERICFG_BASE + 0x0054)
#define PERI_GCON_DCMFSEL       (PERICFG_BASE + 0x0058)

//       
#define M4U_DCM                 (SMI_MMU_TOP_BASE + 0x001C)

//              
#define SMI_COMMON_DCM          0xF0202300

//              
#define SMI_SECURE_DCMCON       0xF000E010
#define SMI_SECURE_DCMSET       0xF000E014
#define SMI_SECURE_DCMCLR       0xF000E018

#define SMILARB0_BASE           0xF7001000
#define SMILARB1_BASE           0xF6010000
#define SMILARB3_BASE           0xF5001000
#define SMILARB4_BASE           0xF5002000


#define SMILARB0_DCM_STA        (SMILARB0_BASE + 0x00)
#define SMILARB0_DCM_CON        (SMILARB0_BASE + 0x10)
#define SMILARB0_DCM_SET        (SMILARB0_BASE + 0x14)
#define SMILARB0_DCM_CLR        (SMILARB0_BASE + 0x18)

#define SMILARB1_DCM_STA        (SMILARB1_BASE + 0x00)
#define SMILARB1_DCM_CON        (SMILARB1_BASE + 0x10)
#define SMILARB1_DCM_SET        (SMILARB1_BASE + 0x14)
#define SMILARB1_DCM_CLR        (SMILARB1_BASE + 0x18)

#define SMILARB2_DCM_STA        (SMILARB2_BASE + 0x00)
#define SMILARB2_DCM_CON        (SMILARB2_BASE + 0x10)
#define SMILARB2_DCM_SET        (SMILARB2_BASE + 0x14)
#define SMILARB2_DCM_CLR        (SMILARB2_BASE + 0x18)

#define SMILARB3_DCM_STA        (SMILARB3_BASE + 0x00)
#define SMILARB3_DCM_CON        (SMILARB3_BASE + 0x10)
#define SMILARB3_DCM_SET        (SMILARB3_BASE + 0x14)
#define SMILARB3_DCM_CLR        (SMILARB3_BASE + 0x18)

#define SMILARB4_DCM_STA        (SMILARB4_BASE + 0x00)
#define SMILARB4_DCM_CON        (SMILARB4_BASE + 0x10)
#define SMILARB4_DCM_SET        (SMILARB4_BASE + 0x14)
#define SMILARB4_DCM_CLR        (SMILARB4_BASE + 0x18)

//   
#define MFG_DCM_CON0            0xF0206010
#define MFG_DCM_CON1            0xF0206014

//           
#define CAM_BASE                0xF5004000
#define CAM_CTL_RAW_DCM         (CAM_BASE + 0x190)
#define CAM_CTL_RGB_DCM         (CAM_BASE + 0x194)
#define CAM_CTL_YUV_DCM         (CAM_BASE + 0x198)
#define CAM_CTL_CDP_DCM         (CAM_BASE + 0x19C)

#define CAM_CTL_RAW_DCM_STA     (CAM_BASE + 0x1A0)
#define CAM_CTL_RGB_DCM_STA     (CAM_BASE + 0x1A4)
#define CAM_CTL_YUV_DCM_STA     (CAM_BASE + 0x1A8)
#define CAM_CTL_CDP_DCM_STA     (CAM_BASE + 0x1AC)

#define JPGDEC_DCM_CTRL         0xF5009300
#define JPGENC_DCM_CTRL         0xF500A300

#define SMI_ISP_COMMON_DCMCON   0xF5003010
#define SMI_ISP_COMMON_DCMSET   0xF5003014
#define SMI_ISP_COMMON_DCMCLR   0xF5003018

//           
#define DISP_HW_DCM_DIS0        0xF4000120
#define DISP_HW_DCM_DIS_SET0    0xF4000124
#define DISP_HW_DCM_DIS_CLR0    0xF4000128

#define DISP_HW_DCM_DIS1        0xF4000130
#define DISP_HW_DCM_DIS_SET1    0xF4000134
#define DISP_HW_DCM_DIS_CLR1    0xF4000138

//        
#define VENC_CE                 0xF70020EC
#define VENC_CLK_DCM_CTRL       0xF70020F4
#define VENC_CLK_CG_CTRL        0xF7002094
#define VENC_MP4_DCM_CTRL       0xF70026F0

//    
#define VDEC_DCM_CON            (VDEC_GCON_BASE + 0x0018)
#endif

#if 0
//                                      
//                                      
//                                      
#define IFRA_DCM                (0x04)
#define PERI_DCM                (0x05)
#define SMI_DCM                 (0x06)
#define MFG_DCM                 (0x07)
#define DIS_DCM                 (0x08)
#define JPEG_DCM                (0x09)
#define VDE_DCM                 (0x10)
#define VEN_DCM                 (0x11)
//                                      
#define TOPCKGEN_DCM            (0x13)
#define CPUSYS_DCM				(0x14)
#define MMSYS_DCM				(0x15)
#define CAM_DCM					(0x16)
#define CA7_DCM					(0x17)
//                          
#define SMILARB_DCM				(0x19)
#else
#define CPU_DCM                 (1U << 0)
#define IFR_DCM                 (1U << 1)
#define PER_DCM                 (1U << 2)
#define SMI_DCM                 (1U << 3)
#define MFG_DCM                 (1U << 4)
#define DIS_DCM                 (1U << 5)
#define ISP_DCM                 (1U << 6)
#define VDE_DCM                 (1U << 7)
//                                
#define TOPCKGEN_DCM			(1U << 8)
#define ALL_DCM                 (CPU_DCM|IFR_DCM|PER_DCM|SMI_DCM|MFG_DCM|DIS_DCM|ISP_DCM|VDE_DCM|TOPCKGEN_DCM)
#define NR_DCMS                 (0x9)
#endif

//                                              
extern void dcm_enable(unsigned int type);
extern void dcm_disable(unsigned int type);

extern void bus_dcm_enable(void);
extern void bus_dcm_disable(void);

extern void disable_infra_dcm(void);
extern void restore_infra_dcm(void);

extern void disable_peri_dcm(void);
extern void restore_peri_dcm(void);

extern void mt_dcm_init(void);

#endif
