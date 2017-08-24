#ifndef _DDP_OVL_H_
#define _DDP_OVL_H_
#include "ddp_hal.h"
#include "DpDataType.h"

#define OVL_CASCADE_SUPPORT
#define OVL_MAX_WIDTH  (4095)
#define OVL_MAX_HEIGHT (4095)
#ifdef OVL_CASCADE_SUPPORT
#define OVL_LAYER_NUM  (8)
#else
#define OVL_LAYER_NUM  (4)
#endif

#define OVL_LAYER_NUM_PER_OVL 4

typedef enum 
{
    DDP_OVL1_STATUS_IDLE=0,
    DDP_OVL1_STATUS_PRIMARY=1,  //                                  
    DDP_OVL1_STATUS_SUB=2,      //                             
    DDP_OVL1_STATUS_SUB_REQUESTING=3,  //                        
    DDP_OVL1_STATUS_PRIMARY_RELEASED=4,
    DDP_OVL1_STATUS_PRIMARY_RELEASING=5,
    DDP_OVL1_STATUS_PRIMARY_DISABLE=6
} DISP_OVL1_STATUS;


//                     
int ovl_start(DISP_MODULE_ENUM module,void * handle);

//                    
int ovl_stop(DISP_MODULE_ENUM module,void * handle);

//                     
int ovl_reset(DISP_MODULE_ENUM module,void * handle);

//                       
int ovl_roi( DISP_MODULE_ENUM module,
               unsigned int bgW, 
               unsigned int bgH,    //            
               unsigned int bgColor,//             
               void * handle); 

//                    
int ovl_layer_switch(DISP_MODULE_ENUM module,
                       unsigned layer, 
                       unsigned int en,
                       void * handle);
//                     
void ovl_get_address(DISP_MODULE_ENUM module, unsigned long *add);

//                         
int ovl_layer_config(DISP_MODULE_ENUM module,
                       unsigned layer,
                       unsigned source, 
                       DpColorFormat format, 
                       unsigned long addr, 
                       unsigned int src_x,     //             
                       unsigned int src_y,     //             
                       unsigned int src_pitch,
                       unsigned int dst_x,     //             
                       unsigned int dst_y,     //             
                       unsigned int dst_w,     //          
                       unsigned int dst_h,     //           
                       unsigned int keyEn,
                       unsigned int key, 
                       unsigned int aen, 
                       unsigned char alpha,//             
                       unsigned int sur_aen,
                       unsigned int src_alpha,
                       unsigned int dst_alpha,
                       unsigned int constant_color,
                       unsigned int yuv_range,
                       void * handle);                    

int ovl_3d_config(DISP_MODULE_ENUM module,
                    unsigned int layer_id, 
                    unsigned int en_3d,
                    unsigned int landscape,
                    unsigned int r_first,
                    void * handle);

void ovl_dump_analysis(DISP_MODULE_ENUM module);
void ovl_dump_reg(DISP_MODULE_ENUM module);

void ovl_get_info(int idx, void * data);

DISP_OVL1_STATUS ovl_get_status(void);
void ovl_set_status(DISP_OVL1_STATUS status);

#endif
