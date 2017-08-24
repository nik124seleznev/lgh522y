#ifndef SEC_CFG_V3_H
#define SEC_CFG_V3_H

/*                                                                         
                          
                                                                          */
#include "sec_cfg_common.h"

/*                                                                         
                              
                                                                          */
#include "sec_sign_header.h"

/*                                                                         
                           
                                                                          */
#define EXT_REGION_BUF_SIZE     (4096)        /*                                 */
 
typedef struct
{    
    unsigned int                content_len;  /*                        */    
    unsigned char               buf [EXT_REGION_BUF_SIZE];
    
} SECCFG_EXT_REGION;

/*                                                                         
                       
                                                                          */

typedef struct
{   
    unsigned int                magic_number;     
    unsigned char               name [16];    /*                          */    
    unsigned int                real_offset;  /*                                            */        
    ROM_TYPE                    image_type;   /*                             */
    SEC_IMG_ATTR                image_attr;   /*                  */
    SEC_IMG_HEADER_U            header;       /*                   */
    unsigned int                ext_offset;   /*                            */    
    unsigned int                ext_len;      /*                            */        
    
} SECURE_IMG_INFO_V3;

/*                                                                         
                     
                                                                          */
#define SECURE_IMAGE_COUNT_V3   (20)

/*                                   */
/*                                   */
/*                                   */
typedef struct
{   
    unsigned char               id[16];   
    unsigned int                magic_number;
    
    unsigned int                seccfg_ver;     
    unsigned int                seccfg_size; 

    unsigned int                seccfg_enc_offset; 
    unsigned int                seccfg_enc_len; 
    
    unsigned char               sw_sec_lock_try;
    unsigned char               sw_sec_lock_done;
    
    unsigned short              page_size; 
    unsigned int                page_count;

    /*                    */
    /*                    */
    /*                    */    
    SECURE_IMG_INFO_V3          image_info [SECURE_IMAGE_COUNT_V3];
    SIU_STATUS                  siu_status; 
    SECCFG_STATUS               seccfg_status;
    SECCFG_ATTR                 seccfg_attr;     
    SECCFG_EXT_REGION           seccfg_ext;
    /*                    */    
    /*                    */
    /*                    */    
    
    unsigned int                end_pattern;

} SECURE_CFG_V3;

#endif //             
