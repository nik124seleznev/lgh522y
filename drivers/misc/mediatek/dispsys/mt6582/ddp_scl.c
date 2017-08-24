#include <linux/kernel.h>
#include <mach/mt_typedefs.h>

#include "ddp_reg.h"
#include "ddp_scl.h"

#define ENABLE_DDP_SCL_LOG

#ifdef ENABLE_DDP_SCL_LOG
#define DDP_SCL_LOG(fmt, arg...) {printk("[SCL]"); printk(fmt, ##arg);}
#else
#define DDP_SCL_LOG()
#endif

#define YUV422_LINE_BUFFER_DEPTH 1280
#define YUV444_LINE_BUFFER_DEPTH 853

BOOL SCL_EN;
BOOL RELAY_MODE;
BOOL HRZ_EN;
BOOL VRZ_EN;
BOOL FIFO_EN;
BOOL HRZ_POSIT;
BOOL VRZ_POSIT;

unsigned int YUV422;
unsigned int C422_SEL;
unsigned int C444_SEL;

unsigned int HRZ_MODE;
unsigned int HRZ_METHOD;
unsigned int HRZ_TAPNUM_SEL;
unsigned int HRZ_COEF_TYPE;
unsigned int HRZ_COEF_SEL;
unsigned int HRZ_DERING_EN;
unsigned int HRZ_SRCSZ;
unsigned int HRZ_TARSZ;
unsigned int HRZ_FACTOR;
unsigned int HRZ_OFFSET;

unsigned int VRZ_MODE;
unsigned int VRZ_METHOD;
unsigned int VRZ_TAPNUM_SEL;
unsigned int VRZ_COEF_TYPE;
unsigned int VRZ_COEF_SEL;
unsigned int VRZ_DERING_EN;
unsigned int VRZ_SRCSZ;
unsigned int VRZ_TARSZ;
unsigned int VRZ_OFFSET;
unsigned int VRZ_FACTOR;

unsigned int HPK_EN;
unsigned int VPK_EN;
unsigned int HPK_BP_GAIN;
unsigned int HPK_HP_GAIN;
unsigned int VPK_HP_GAIN;

//             
int SCLStart(void)
{
    DISP_REG_SET_FIELD(DSCL_INTEN_FLD_OF_END_INT_EN, DISP_REG_SCL_INTEN, 0);
    DISP_REG_SET_FIELD(DSCL_INTEN_FLD_IF_END_INT_EN, DISP_REG_SCL_INTEN, 0);
    DISP_REG_SET_FIELD(DSCL_CTRL_FLD_SCL_EN, DISP_REG_SCL_CTRL, 1);
    return 0;
}

//            
int SCLStop(void)
{
    DISP_REG_SET_FIELD(DSCL_CTRL_FLD_SCL_EN, DISP_REG_SCL_CTRL, 0);
    DISP_REG_SET(DISP_REG_SCL_INTEN, 0);
    DISP_REG_SET(DISP_REG_SCL_INTSTA, 0);
    return 0;
}

//             
int SCLReset(void)
{
    DISP_REG_SET_FIELD(DSCL_CTRL_FLD_SCL_RESET, DISP_REG_SCL_CTRL, 1);
    DISP_REG_SET_FIELD(DSCL_CTRL_FLD_SCL_RESET, DISP_REG_SCL_CTRL, 0);
    return 0;
}

//               
int SCLConfig(DISP_INTERLACE_FORMAT interlace,
              int rotateDegree,
              int srcWidth, int srcHeight,
              int dstWidth, int dstHeight,
              int flip)
{
    unsigned char rotate = 0;
    unsigned char isInterlace = (interlace == DISP_INTERLACE_FORMAT_NONE) ? 0 : 1;
    unsigned char isBottomField = (interlace == DISP_INTERLACE_FORMAT_BOTTOM_FIELD) ? 1 : 0;

    switch (rotateDegree)
    {
        case 0:
            rotate = 0;
            break;

        case 90:
            rotate = 1;
            break;

        case 180:
            rotate = 2;
            break;

        case 270:
            rotate = 3;
            break;

        default:
            DDP_SCL_LOG("unsupport rotate degree %d", rotateDegree);
            break;

    }
    DDP_SCL_LOG("DpEngine_SCL config...\n");
    DDP_SCL_LOG(" input (w, h) : %d %d\n", srcWidth, srcHeight);
    DDP_SCL_LOG(" output (w, h) : %d %d\n", dstWidth, dstHeight);
    DDP_SCL_LOG(" rotate : %d, flip=%d\n", rotate, flip);

    disp_scl_set_scale_param (srcWidth,
                              srcHeight,
                              dstWidth,
                              dstHeight,
                              isInterlace,  //                                          
                              isBottomField,     //                                                
                              rotate,         //                                                     
                              flip);       //                                                

    DISP_REG_SET_FIELD(DSCL_INP_CHKSUM_FLD_INP_CHKSUM_EN, DISP_REG_SCL_INP_CHKSUM, 1);
    DISP_REG_SET_FIELD(DSCL_OUTP_CHKSUM_FLD_OUTP_CHKSUM_EN, DISP_REG_SCL_OUTP_CHKSUM, 1);

    return 0;
}

/*
                
*/
void disp_scl_reg_enable(BOOL enable)
{
    BOOL bEnable = enable;

    if (bEnable)
    {
        DISP_REG_SET_FIELD(DSCL_INTEN_FLD_OF_END_INT_EN, DISP_REG_SCL_INTEN, 1);
        DISP_REG_SET_FIELD(DSCL_INTEN_FLD_IF_END_INT_EN, DISP_REG_SCL_INTEN, 1);
    }

    DISP_REG_SET_FIELD(DSCL_CTRL_FLD_SCL_EN, DISP_REG_SCL_CTRL, bEnable);
}

/*
                           
*/
void disp_scl_enable_relay_mode(BOOL enable, unsigned src_width, unsigned src_height)
{
    DISP_REG_SET_FIELD(DSCL_CFG_FLD_RELAY_MODE, DISP_REG_SCL_CFG, enable);

    DISP_REG_SET_FIELD(DSCL_HRZ_SIZE_FLD_HRZ_TARSZ, DISP_REG_SCL_HRZ_SIZE, src_width);
    DISP_REG_SET_FIELD(DSCL_HRZ_SIZE_FLD_HRZ_SRCSZ, DISP_REG_SCL_HRZ_SIZE, src_width);
    DISP_REG_SET_FIELD(DSCL_VRZ_SIZE_FLD_VRZ_TARSZ, DISP_REG_SCL_VRZ_SIZE, src_height);
    DISP_REG_SET_FIELD(DSCL_VRZ_SIZE_FLD_VRZ_SRCSZ, DISP_REG_SCL_VRZ_SIZE, src_height);
}

void disp_scl_set_scale_param (unsigned src_width,
                               unsigned src_height,
                               unsigned dst_width,
                               unsigned dst_height,
                               unsigned char interlaced, //                                         
                               unsigned char fieldID,   //                                               
                               unsigned char rotation,   //                                                    
                               unsigned char flipping)
{
    unsigned cutoff_freqz; //                       

    /*                          */
    /*                          */
    /*                          */
    unsigned char interlace_dir = 0;    //                                                                  
    unsigned char interlace_offset = 0; //                                    
    /*                                
                                   
                               
                               
                            
                               
                            
                            
                               
                            
    */
    if (interlaced)
    {
        if ((rotation == 0) || (rotation == 2))
        {
            interlace_dir = 0;
            if (((rotation == 0) && (fieldID != 0)) || ((rotation == 2) && (fieldID == 0)))
                interlace_offset = 1;
            else
                interlace_offset = 0;
        }
        else //                                       
        {
            interlace_dir = 1;
            if (((rotation == 1) && (flipping == fieldID)) || ((rotation == 3) && (flipping != fieldID)))
                interlace_offset = 1;
            else
                interlace_offset = 0;
        }
    }

    /*                                 */
    /*                                 */
    /*                                 */
    HRZ_SRCSZ = src_width;
    HRZ_TARSZ = dst_width;

    VRZ_SRCSZ = src_height;
    VRZ_TARSZ = dst_height;

    /*                     */
    /*                     */
    /*                     */
    //          
    if ((HRZ_SRCSZ != HRZ_TARSZ) || ((interlaced != 0) && (interlace_dir != 0)))
        HRZ_EN = 1;
    else
        HRZ_EN = 0;

    //          
    if ((VRZ_SRCSZ != VRZ_TARSZ) || ((interlaced != 0) && (interlace_dir == 0)))
        VRZ_EN = 1;
    else
        VRZ_EN = 0;

    //          
    if (HRZ_EN == 0 && VRZ_EN == 0) RELAY_MODE = 1;
    else                            RELAY_MODE = 0;

    //          
    if (HRZ_EN == 0 && VRZ_EN == 0) SCL_EN = 0;
    else                            SCL_EN = 1;

    /*                  */
    /*                  */
    /*                  */
    cutoff_freqz = (HRZ_TARSZ << 16) / HRZ_SRCSZ;

    if (HRZ_SRCSZ < HRZ_TARSZ) //            
    {
        if ((interlaced != 0) && (interlace_dir != 0)) //                                               
        {
            if (interlace_offset != 0)
                HRZ_OFFSET = -1 << 19;
            else
                HRZ_OFFSET = 0;
            HRZ_FACTOR = (((HRZ_SRCSZ << 1) - 1) << 19) / (HRZ_TARSZ - 1);
        }
        else //                        
        {
            HRZ_OFFSET = 0;
            HRZ_FACTOR = ((HRZ_SRCSZ - 1) << 20) / (HRZ_TARSZ - 1);
        }

        HRZ_POSIT      = 1; //                                                    
        HRZ_MODE       = 1; //                          
        HRZ_METHOD     = 0; //             
        HRZ_TAPNUM_SEL = 0; //                                      
        HRZ_COEF_TYPE  = 0; //                        
        HRZ_COEF_SEL   = 5; //                
        HRZ_DERING_EN  = 1; //                                   
    }
    else if (cutoff_freqz >= 13000) //                                                
    {
        if ((interlaced != 0) && (interlace_dir != 0)) //                                                 
        {
            if (interlace_offset != 0)
                HRZ_OFFSET = -1 << 19;
            else
                HRZ_OFFSET = 0;
            HRZ_FACTOR = (((HRZ_SRCSZ << 1) - 1) << 19) / (HRZ_TARSZ - 1);
        }
        else //                          
        {
            //               
            //                                                       
            HRZ_OFFSET = ((HRZ_SRCSZ - HRZ_TARSZ) << 19) / HRZ_TARSZ;
            HRZ_FACTOR = (HRZ_SRCSZ << 20) / HRZ_TARSZ;
        }

        HRZ_POSIT      = 0; //                                                    
        HRZ_MODE       = 0; //                          
        HRZ_METHOD     = 0; //             
        HRZ_TAPNUM_SEL = 0; //                                      
        HRZ_COEF_TYPE  = 0; //                        
        HRZ_DERING_EN  = 0; //                                   

        if      (cutoff_freqz >= 55888) HRZ_COEF_SEL =  6; //             
        else if (cutoff_freqz >= 51337) HRZ_COEF_SEL =  7; //             
        else if (cutoff_freqz >= 47878) HRZ_COEF_SEL =  8; //             
        else if (cutoff_freqz >= 45147) HRZ_COEF_SEL =  9; //             
        else if (cutoff_freqz >= 42325) HRZ_COEF_SEL = 10; //             
        else if (cutoff_freqz >= 38912) HRZ_COEF_SEL = 11; //             
        else if (cutoff_freqz >= 34816) HRZ_COEF_SEL = 12; //             
        else if (cutoff_freqz >= 30948) HRZ_COEF_SEL = 13; //             
        else if (cutoff_freqz >= 28217) HRZ_COEF_SEL = 14; //             
        else if (cutoff_freqz >= 25941) HRZ_COEF_SEL = 15; //             
        else if (cutoff_freqz >= 23211) HRZ_COEF_SEL = 16; //             
        else if (cutoff_freqz >= 20633) HRZ_COEF_SEL = 17; //             
        else if (cutoff_freqz >= 17902) HRZ_COEF_SEL = 18; //             
        else if (cutoff_freqz >= 14746) HRZ_COEF_SEL = 19; //             
        else if (cutoff_freqz >= 12015) HRZ_COEF_SEL = 20; //             
        else                            HRZ_COEF_SEL = 21; //             
    }
    else //               
    {
        HRZ_POSIT      = 0; //                                                    
        HRZ_MODE       = 0; //                          
        HRZ_METHOD     = 1; //             
        HRZ_TAPNUM_SEL = 2; //                                      
        HRZ_COEF_TYPE  = 0; //                        
        HRZ_COEF_SEL   = 2; //                                    
        HRZ_DERING_EN  = 0; //                                   
        HRZ_FACTOR     = ((HRZ_TARSZ - 1) << 20) / (HRZ_SRCSZ - 1);
        HRZ_OFFSET     = 0;
    }

    /*                  */
    /*                  */
    /*                  */
    cutoff_freqz = (VRZ_TARSZ << 16) / VRZ_SRCSZ;

    if (VRZ_SRCSZ < VRZ_TARSZ) //            
    {
        if ((interlaced != 0) && (interlace_dir == 0)) //                                               
        {
            if (interlace_offset != 0)
                VRZ_OFFSET = -1 << 19;
            else
                VRZ_OFFSET = 0;
            VRZ_FACTOR = (((VRZ_SRCSZ << 1) - 1) << 19) / (VRZ_TARSZ - 1);
        }
        else //                        
        {
            VRZ_OFFSET = 0;
            VRZ_FACTOR = ((VRZ_SRCSZ - 1) << 20) / (VRZ_TARSZ - 1);
        }

        VRZ_POSIT      = 1; //                                                      
        VRZ_MODE       = 1; //                          
        VRZ_METHOD     = 0; //             
        VRZ_COEF_TYPE  = 0; //                        
        VRZ_DERING_EN  = 1; //                                   

        if (HRZ_SRCSZ > HRZ_TARSZ) //                                            
        {
            VRZ_TAPNUM_SEL = 1; //                            
            VRZ_COEF_SEL   = 1; //              
        }
        else
        {
            VRZ_TAPNUM_SEL = 0; //                            
            VRZ_COEF_SEL   = 4; //                
        }
    }
    else if (cutoff_freqz >= 13000) //                            
    {
        if ((interlaced != 0) && (interlace_dir == 0)) //                                                 
        {
            if (interlace_offset != 0)
                VRZ_OFFSET = -1 << 19;
            else
                VRZ_OFFSET = 0;
            VRZ_FACTOR = (((VRZ_SRCSZ << 1) - 1) << 19) / (VRZ_TARSZ - 1);
        }
        else //                          
        {
            //               
            //                                                       
            VRZ_OFFSET = ((VRZ_SRCSZ - VRZ_TARSZ) << 19) / VRZ_TARSZ;
            VRZ_FACTOR = (VRZ_SRCSZ << 20) / VRZ_TARSZ;
        }

        VRZ_POSIT      = 0; //                                                      
        VRZ_MODE       = 0; //                          
        VRZ_METHOD     = 0; //             
        VRZ_TAPNUM_SEL = 0; //                            
        VRZ_COEF_TYPE  = 0; //                        
        VRZ_DERING_EN  = 0; //                                   

        if      (cutoff_freqz >= 55888) VRZ_COEF_SEL =  6; //             
        else if (cutoff_freqz >= 51337) VRZ_COEF_SEL =  7; //             
        else if (cutoff_freqz >= 47878) VRZ_COEF_SEL =  8; //             
        else if (cutoff_freqz >= 45147) VRZ_COEF_SEL =  9; //             
        else if (cutoff_freqz >= 42325) VRZ_COEF_SEL = 10; //             
        else if (cutoff_freqz >= 38912) VRZ_COEF_SEL = 11; //             
        else if (cutoff_freqz >= 34816) VRZ_COEF_SEL = 12; //             
        else if (cutoff_freqz >= 30948) VRZ_COEF_SEL = 13; //             
        else if (cutoff_freqz >= 28217) VRZ_COEF_SEL = 14; //             
        else if (cutoff_freqz >= 25941) VRZ_COEF_SEL = 15; //             
        else if (cutoff_freqz >= 23211) VRZ_COEF_SEL = 16; //             
        else if (cutoff_freqz >= 20633) VRZ_COEF_SEL = 17; //             
        else if (cutoff_freqz >= 17902) VRZ_COEF_SEL = 18; //             
        else if (cutoff_freqz >= 14746) VRZ_COEF_SEL = 19; //             
        else if (cutoff_freqz >= 12015) VRZ_COEF_SEL = 20; //             
        else                            VRZ_COEF_SEL = 21; //             
    }
    else //               
    {
        VRZ_POSIT      = 0; //                                                      
        VRZ_MODE       = 0; //                          
        VRZ_METHOD     = 1; //             
        VRZ_TAPNUM_SEL = 1; //                            
        VRZ_COEF_TYPE  = 0; //                        
        VRZ_COEF_SEL   = 2; //                                    
        VRZ_DERING_EN  = 0; //                                   
        VRZ_FACTOR     = ((VRZ_TARSZ - 1) << 20) / (VRZ_SRCSZ - 1);
        VRZ_OFFSET     = 0;
    }

    /*                      */
    /*                      */
    /*                      */
    HPK_HP_GAIN = 2;
    HPK_BP_GAIN = 6;
    VPK_HP_GAIN = 4;

    if (HRZ_SRCSZ > HRZ_TARSZ) HPK_EN = 1;
    else                       HPK_EN = 0;

    //      
    VPK_EN = 0;

    /*                        */
    /*                        */
    /*                        */
    //                                
    //                                                                 
    //                                                                       
    if (VRZ_EN & ((VRZ_METHOD == 1) ||
        ((HRZ_SRCSZ > YUV444_LINE_BUFFER_DEPTH) &&
        (HRZ_TARSZ > YUV444_LINE_BUFFER_DEPTH))))
    {
        YUV422   = 1;
    }
    else
    {
        YUV422   = 0;
    }

    C422_SEL = 0; //                                   
    C444_SEL = 0; //                                

    /*                   */
    /*                   */
    /*                   */
    if ((HRZ_SRCSZ > HRZ_TARSZ) || (VRZ_SRCSZ > VRZ_TARSZ)) //                     
        FIFO_EN = 1;
    else
        FIFO_EN = 0;

    /*                       */
    /*                       */
    /*                       */
    //        

    disp_scl_reg_enable(SCL_EN);
    disp_scl_enable_relay_mode(RELAY_MODE, HRZ_SRCSZ, VRZ_SRCSZ);

    //                          
    DISP_REG_SET_FIELD(DSCL_CFG_FLD_C444_SEL, DISP_REG_SCL_CFG, C444_SEL);
    DISP_REG_SET_FIELD(DSCL_CFG_FLD_C422_SEL, DISP_REG_SCL_CFG, C422_SEL);
    DISP_REG_SET_FIELD(DSCL_CFG_FLD_VRZ_POSIT, DISP_REG_SCL_CFG, VRZ_POSIT);
    DISP_REG_SET_FIELD(DSCL_CFG_FLD_HRZ_POSIT, DISP_REG_SCL_CFG, HRZ_POSIT);
    DISP_REG_SET_FIELD(DSCL_CFG_FLD_VPK_EN, DISP_REG_SCL_CFG, VPK_EN);
    DISP_REG_SET_FIELD(DSCL_CFG_FLD_HPK_EN, DISP_REG_SCL_CFG, HPK_EN);
    DISP_REG_SET_FIELD(DSCL_CFG_FLD_YUV422, DISP_REG_SCL_CFG, YUV422);
    DISP_REG_SET_FIELD(DSCL_CFG_FLD_FIFO_EN, DISP_REG_SCL_CFG, FIFO_EN);
    DISP_REG_SET_FIELD(DSCL_CFG_FLD_VRZ_EN, DISP_REG_SCL_CFG, VRZ_EN);
    DISP_REG_SET_FIELD(DSCL_CFG_FLD_HRZ_EN, DISP_REG_SCL_CFG, HRZ_EN);

    //                              
    DISP_REG_SET_FIELD(DSCL_HRZ_CFG_FLD_HRZ_COEF_SEL, DISP_REG_SCL_HRZ_CFG, HRZ_COEF_SEL);
    DISP_REG_SET_FIELD(DSCL_HRZ_CFG_FLD_HRZ_DERING_EN, DISP_REG_SCL_HRZ_CFG, HRZ_DERING_EN);
    DISP_REG_SET_FIELD(DSCL_HRZ_CFG_FLD_HRZ_COEF_TYPE, DISP_REG_SCL_HRZ_CFG, HRZ_COEF_TYPE);
    DISP_REG_SET_FIELD(DSCL_HRZ_CFG_FLD_HRZ_TAPNUM_SEL, DISP_REG_SCL_HRZ_CFG, HRZ_TAPNUM_SEL);
    DISP_REG_SET_FIELD(DSCL_HRZ_CFG_FLD_HRZ_METHOD, DISP_REG_SCL_HRZ_CFG, HRZ_METHOD);
    DISP_REG_SET_FIELD(DSCL_HRZ_CFG_FLD_HRZ_MODE, DISP_REG_SCL_HRZ_CFG, HRZ_MODE);

    //                               
    DISP_REG_SET(DISP_REG_SCL_HRZ_SIZE, HRZ_TARSZ<<16 | HRZ_SRCSZ);

    //                                 
    DISP_REG_SET_FIELD(DSCL_HRZ_FACTOR_FLD_HRZ_FACTOR, DISP_REG_SCL_HRZ_FACTOR, HRZ_FACTOR);
    //                      
    DISP_REG_SET_FIELD(DSCL_HRZ_OFFSET_FLD_HRZ_OFFSET, DISP_REG_SCL_HRZ_OFFSET, HRZ_OFFSET);

    //                              
    DISP_REG_SET_FIELD(DSCL_VRZ_CFG_FLD_VRZ_COEF_SEL, DISP_REG_SCL_VRZ_CFG, VRZ_COEF_SEL);
    //                                
    DISP_REG_SET_FIELD(DSCL_VRZ_CFG_FLD_VRZ_DERING_EN, DISP_REG_SCL_VRZ_CFG, VRZ_DERING_EN);
    DISP_REG_SET_FIELD(DSCL_VRZ_CFG_FLD_VRZ_COEF_TYPE, DISP_REG_SCL_VRZ_CFG, VRZ_COEF_TYPE);
    DISP_REG_SET_FIELD(DSCL_VRZ_CFG_FLD_VRZ_TAPNUM_SEL, DISP_REG_SCL_VRZ_CFG, VRZ_TAPNUM_SEL);
    DISP_REG_SET_FIELD(DSCL_VRZ_CFG_FLD_VRZ_METHOD, DISP_REG_SCL_VRZ_CFG, VRZ_METHOD);
    DISP_REG_SET_FIELD(DSCL_VRZ_CFG_FLD_VRZ_MODE, DISP_REG_SCL_VRZ_CFG, VRZ_MODE);

    //                               
    DISP_REG_SET(DISP_REG_SCL_VRZ_SIZE, VRZ_TARSZ<<16 | VRZ_SRCSZ);

    //                                 
    DISP_REG_SET_FIELD(DSCL_VRZ_FACTOR_FLD_VRZ_FACTOR, DISP_REG_SCL_VRZ_FACTOR, VRZ_FACTOR);
    //                      
    DISP_REG_SET_FIELD(DSCL_VRZ_OFFSET_FLD_VRZ_OFFSET, DISP_REG_SCL_VRZ_OFFSET, VRZ_OFFSET);

    //                      
    DISP_REG_SET(DISP_REG_SCL_PEAK_CFG, HPK_BP_GAIN<<8 | HPK_HP_GAIN<<4 | VPK_HP_GAIN);

}
