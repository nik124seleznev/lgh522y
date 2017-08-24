#ifndef SEC_CFG_COMMON_H
#define SEC_CFG_COMMON_H

/*                                                                           */
/*                                                                           */
/*                                                                           */
typedef enum {
    NORMAL_ROM                  = 0x01,
    YAFFS_IMG                   = 0x08,
} ROM_TYPE;

/*                                                                           */
/*                                                                           */
/*                                                                           */
/*                                                    */
#define SEC_IMG_MAGIC_NUM       (0x49494949)    /*      */

typedef enum
{
    ATTR_SEC_IMG_UPDATE         = 0x10,         /*                        */
    ATTR_SEC_IMG_COMPLETE       = 0x43434343,   /*      */ 
    ATTR_SEC_IMG_INCOMPLETE     = 0x49494949,   /*      */
    ATTR_SEC_IMG_FORCE_UPDATE   = 0x46464646    /*      */
    
} SEC_IMG_ATTR;

/*                                                                           */
/*                                                                           */
/*                                                                           */
/*                        */
#define SEC_BUF_LEN             (0x3000)

/*                                                                           */
/*                                                                           */
/*                                                                           */
#define SECCFG_SUPPORT_VERSION  (0x1)

/*                                                                           */
/*                                                                           */
/*                                                                           */
#define SEC_CFG_MAGIC_NUM       (0x4D4D4D4D)    /*      */
#define SEC_CFG_BEGIN           "AND_SECCFG_v"
#define SEC_CFG_BEGIN_LEN       (12)

/*                                              */
/*                                                                    */
typedef enum
{
    SEC_CFG_COMPLETE_NUM        = 0x43434343,   /*      */ 
    SEC_CFG_INCOMPLETE_NUM      = 0x49494949    /*      */
     
} SECCFG_STATUS;

/*                                                              */
typedef enum
{
    ATTR_DEFAULT                = 0x33333333,   /*      */ 
    ATTR_DISABLE_IMG_CHECK      = 0x44444444    /*      */
     
} SECCFG_ATTR;

/*                                                 */
typedef enum
{
    UBOOT_UPDATED_BY_SIU        = 0x0001,
    BOOT_UPDATED_BY_SIU         = 0x0010,    
    RECOVERY_UPDATED_BY_SIU     = 0x0100,        
    SYSTEM_UPDATED_BY_SIU       = 0x1000            
     
} SIU_STATUS;

/*                           */
#define SEC_CFG_END_PATTERN     (0x45454545)    /*      */

#endif //                 
