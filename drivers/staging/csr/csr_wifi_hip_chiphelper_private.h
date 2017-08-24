/*****************************************************************************

            (c) Cambridge Silicon Radio Limited 2011
            All rights reserved and confidential information of CSR

            Refer to LICENSE.txt included with this source for details
            on the license terms.

*****************************************************************************/

#ifndef CSR_WIFI_HIP_CHIPHELPER_PRIVATE_H__
#define CSR_WIFI_HIP_CHIPHELPER_PRIVATE_H__


#include "csr_wifi_hip_chiphelper.h"

/*                                         */

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

#define MAKE_GP(R, O)  (((UNIFI_ ## R) << 24) | (O))
#define GP_OFFSET(GP)  ((GP) & 0xFFFFFF)
#define GP_SPACE(GP)   (((GP) >> 24) & 0xFF)


/*                     */
struct val_array_t
{
    u32                             len;
    const struct chip_helper_init_values *vals;
};

/*                            */
struct data_array_t
{
    u32        len;
    const u16 *vals;
};

struct reset_prog_t
{
    u32                              len;
    const struct chip_helper_reset_values *vals;
};

/*                                                      
                              */
struct chip_map_address_t
{
    u16 spi, host;
};

struct map_array_t
{
    u32                        len;
    const struct chip_map_address_t *vals;
};

struct chip_device_regs_per_transport_t
{
    u16 dbg_proc_select;
    u16 dbg_stop_status;
    u16 window1_page;    /*                   */
    u16 window2_page;    /*                   */
    u16 window3_page;    /*               */
    u16 io_log_addr;
};

struct chip_device_regs_t
{
    u16                               gbl_chip_version;
    u16                               gbl_misc_enables;
    u16                               dbg_emu_cmd;
    struct chip_device_regs_per_transport_t host;
    struct chip_device_regs_per_transport_t spi;
    u16                               dbg_reset;
    u16                               dbg_reset_value;
    u16                               dbg_reset_warn;
    u16                               dbg_reset_warn_value;
    u16                               dbg_reset_result;
    u16                               xap_pch;
    u16                               xap_pcl;
    u16                               proc_pc_snoop;
    u16                               watchdog_disable;
    u16                               mailbox0;
    u16                               mailbox1;
    u16                               mailbox2;
    u16                               mailbox3;
    u16                               sdio_host_int;
    u16                               shared_io_interrupt;
    u16                               sdio_hip_handshake;
    u16                               coex_status; /*                       */
};

/*                                                           
                  
                                                             
                                                               
                                                               
                                 */
struct window_shift_info_t
{
    s32  allowed;
    u32 page_shift;
    u16 page_offset;
};

/*                                                                  
                                                                   
                                                                        
                                                                      
                 */
struct window_info_t
{
    u16                         address;
    u16                         size;
    u16                         blocked;
    const struct window_shift_info_t *mode;
};

/*                                                                
                                                                     
                                                       */
struct chip_version_t
{
    s32  pre_bc7;
    u16 mask;
    u16 result;
    u8  sdio;
};

struct chip_device_desc_t
{
    struct chip_version_t chip_version;

    /*                                                                     */
    const char *friendly_name;
    /*                                   */
    const char *marketing_name;

    /*                                                  */
    struct val_array_t init;

    /*                                */
    struct reset_prog_t reset_prog;

    /*                  */
    const struct chip_device_regs_t *regs;

    /*                             */
    struct
    {
        u32 has_flash     : 1;
        u32 has_ext_sram  : 1;
        u32 has_rom       : 1;
        u32 has_bt        : 1;
        u32 has_wlan      : 1;
    } bools;

    /*                                                                 
                                                                    
                                                                 
                                                           */
    struct map_array_t map;

    /*                                                                             
                                                   */
    struct
    {
        u32 ram;
        u32 rom;
        u32 flash;
        u32 ext_sram;
    } prog_offset;

    /*                                                                */
    struct
    {
        u16 ram;
        /*                                 */
    } data_offset;

    /*                                      */
    const struct window_info_t *windows[CHIP_HELPER_WINDOW_COUNT];
};

#endif /*                                     */
