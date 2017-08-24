/*****************************************************************************

            (c) Cambridge Silicon Radio Limited 2011
            All rights reserved and confidential information of CSR

            Refer to LICENSE.txt included with this source for details
            on the license terms.

*****************************************************************************/

/*
                                                                              
  
                               
  
                                                                     
  
                                                                              
 */
#ifndef __UNIFIHW_H__
#define __UNIFIHW_H__ 1

/*                                                     */
#define SLUT_FINGERPRINT        0xD397


/*                           */
#define UNIFI_LOADER_IDLE       0x00
#define UNIFI_LOADER_COPY       0x01
#define UNIFI_LOADER_ERROR_MASK 0xF0

/*                               */
#define UNIFI_BOOT_LOADER_IDLE       0x00
#define UNIFI_BOOT_LOADER_RESTART    0x01
#define UNIFI_BOOT_LOADER_PATCH      0x02
#define UNIFI_BOOT_LOADER_LOAD_STA   0x10
#define UNIFI_BOOT_LOADER_LOAD_PTEST 0x11


/*                                                           */
#define UNIFI_SH_DMEM   0x01    /*                    */
#define UNIFI_EXT_FLASH 0x02    /*                */
#define UNIFI_EXT_SRAM  0x03    /*               */
#define UNIFI_REGISTERS 0x04    /*           */
#define UNIFI_PHY_DMEM  0x10    /*                 */
#define UNIFI_PHY_PMEM  0x11    /*                    */
#define UNIFI_PHY_ROM   0x12    /*         */
#define UNIFI_MAC_DMEM  0x20    /*                 */
#define UNIFI_MAC_PMEM  0x21    /*                    */
#define UNIFI_MAC_ROM   0x22    /*         */
#define UNIFI_BT_DMEM   0x30    /*                */
#define UNIFI_BT_PMEM   0x31    /*                   */
#define UNIFI_BT_ROM    0x32    /*        */

#define UNIFI_MAKE_GP(R, O)  (((UNIFI_ ## R) << 24) | (O))
#define UNIFI_GP_OFFSET(GP)  ((GP) & 0xFFFFFF)
#define UNIFI_GP_SPACE(GP)   (((GP) >> 24) & 0xFF)

#endif /*               */
