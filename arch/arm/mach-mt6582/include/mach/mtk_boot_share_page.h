#ifndef __MTK_BOOT_SHARE_PAGE_H__
#define __MTK_BOOT_SHARE_PAGE_H__

#define BOOT_SHARE_BASE  (0xC0002000)      /*                         */
#define BOOT_SHARE_SIZE  (0x1000)          /*                    */

#define BOOT_SHARE_MAGIC (0xBEEFFEEB)      /*      */

/*                                          */
/* 
        
                                                                            
*/
#define BOOT_SHARE_MAGIC1_OFST   (0)
#define BOOT_SHARE_MAGIC1_SIZE   (4)

#define BOOT_SHARE_DEV_INFO_OFST (BOOT_SHARE_MAGIC1_OFST+BOOT_SHARE_MAGIC1_SIZE)
#define BOOT_SHARE_DEV_INFO_SIZE (16)

#define BOOT_SHARE_HOTPLUG_OFST  (1008)    /*                               */
#define BOOT_SHARE_HOTPLUG_SIZE  (32)

#define BOOT_SHARE_MAGIC2_OFST   (4092)
#define BOOT_SHARE_MAGIC2_SIZE   (4)

#endif /*                           */
