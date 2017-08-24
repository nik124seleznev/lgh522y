#ifndef SEC_ROMINFO_H
#define SEC_ROMINFO_H

#include "sec_boot.h"
#include "sec_key.h"
#include "sec_ctrl.h"


/*                                                                         
                  
                                                                          */ 
#define RI_NAME                             "AND_ROMINFO_v"
#define RI_NAME_LEN                         13
/*                                           */
/*                                        */
#define ROM_INFO_VER                        0x2
#define ROM_INFO_SEC_RO_EXIST               0x1
#define ROM_INFO_ANTI_CLONE_OFFSET          0x54
#define ROM_INFO_ANTI_CLONE_LENGTH          0xE0
#define ROM_INFO_DEFAULT_SEC_CFG_OFFSET     0x360000
#define ROM_INFO_SEC_CFG_LENGTH             0x20000

/*                                                                         
                           
                                                                          */ 
/*                                                 
                                                         */
#define AND_ROM_INFO_SIZE                  (960)   
typedef struct {

    unsigned char                   m_id[16];           /*     */
    unsigned int                    m_rom_info_ver;     /*     */
    unsigned char                   m_platform_id[16];  /*          */
    unsigned char                   m_project_id[16];
    
    unsigned int                    m_sec_ro_exist;     /*     */
    unsigned int                    m_sec_ro_offset;    /*     */
    unsigned int                    m_sec_ro_length;    /*     */
    
    unsigned int                    m_ac_offset;        /*       
                                                                   */

    unsigned int                    m_ac_length;        /*       
                                                                   */
                                                            
    unsigned int                    m_sec_cfg_offset;   /*      
                                                                            
                                                                            

                                                                              
                                                                            
                                                                         */

    unsigned int                    m_sec_cfg_length;   /*      
                                                                            
                                                                            
                                                            
                                                                              
                                                                            
                                                                         */ 

    unsigned char                   m_reserve1[128];    
    
    AND_SECCTRL_T                   m_SEC_CTRL;         /*           
                                                                           
                                                                    */

    unsigned char                   m_reserve2[18];

                                                        /*           
                                                                              
                                                                      */
    AND_SECBOOT_CHECK_PART_T        m_SEC_BOOT_CHECK_PART; 

    AND_SECKEY_T                    m_SEC_KEY;          /*           
                                                                */

} AND_ROMINFO_T;

#endif /*               */

