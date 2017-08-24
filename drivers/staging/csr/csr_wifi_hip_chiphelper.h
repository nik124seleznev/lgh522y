/*****************************************************************************

            (c) Cambridge Silicon Radio Limited 2011
            All rights reserved and confidential information of CSR

            Refer to LICENSE.txt included with this source for details
            on the license terms.

*****************************************************************************/

#ifndef CSR_WIFI_HIP_CHIPHELPER_H__
#define CSR_WIFI_HIP_CHIPHELPER_H__


#include <linux/types.h>

/*                                                               
                                                                         */
enum chip_helper_bluecore_age
{
    chip_helper_bluecore_pre_bc7,
    chip_helper_bluecore_bc7_or_later
};

/*                                                       
                                                              */
enum chip_helper_window_index
{
    CHIP_HELPER_WINDOW_1        = 0,
    CHIP_HELPER_WINDOW_2        = 1,
    CHIP_HELPER_WINDOW_3        = 2,
    CHIP_HELPER_WINDOW_COUNT    = 3
};

/*                                                          
                                                              */
enum chip_helper_window_type
{
    CHIP_HELPER_WT_CODE_RAM = 0,
    CHIP_HELPER_WT_FLASH    = 1,
    CHIP_HELPER_WT_EXT_SRAM = 2,
    CHIP_HELPER_WT_ROM      = 3,
    CHIP_HELPER_WT_SHARED   = 4,
    CHIP_HELPER_WT_COUNT    = 5
};

/*                                    */
enum chip_helper_dbg_emu_cmd_enum
{
    CHIP_HELPER_DBG_EMU_CMD_XAP_STEP_MASK   = 0x0001,
    CHIP_HELPER_DBG_EMU_CMD_XAP_RUN_B_MASK  = 0x0002,
    CHIP_HELPER_DBG_EMU_CMD_XAP_BRK_MASK    = 0x0004,
    CHIP_HELPER_DBG_EMU_CMD_XAP_WAKEUP_MASK = 0x0008
};

/*                                                                                */
enum chip_helper_dbg_stop_status_enum
{
    CHIP_HELPER_DBG_STOP_STATUS_NONE_MASK               = 0x0000,
    CHIP_HELPER_DBG_STOP_STATUS_P0_MASK                 = 0x0001,
    CHIP_HELPER_DBG_STOP_STATUS_P1_MASK                 = 0x0002,
    CHIP_HELPER_DBG_STOP_STATUS_P2_MASK                 = 0x0004,
    CHIP_HELPER_DBG_STOP_STATUS_SLEEP_STATUS_P0_MASK    = 0x0008,
    CHIP_HELPER_DBG_STOP_STATUS_SLEEP_STATUS_P1_MASK    = 0x0010,
    CHIP_HELPER_DBG_STOP_STATUS_SLEEP_STATUS_P2_MASK    = 0x0020,
    /*                    */
    CHIP_HELPER_DBG_STOP_STATUS_MAC_MASK                = 0x0001,
    CHIP_HELPER_DBG_STOP_STATUS_PHY_MASK                = 0x0002,
    CHIP_HELPER_DBG_STOP_STATUS_BT_MASK                 = 0x0004,
    CHIP_HELPER_DBG_STOP_STATUS_SLEEP_STATUS_MAC_MASK   = 0x0008,
    CHIP_HELPER_DBG_STOP_STATUS_SLEEP_STATUS_PHY_MASK   = 0x0010,
    CHIP_HELPER_DBG_STOP_STATUS_SLEEP_STATUS_BT_MASK    = 0x0020
};

/*                               */
enum chip_helper_watchdog_disable_enum
{
    CHIP_HELPER_WATCHDOG_DISABLE_CODE1 = 0x6734,
    CHIP_HELPER_WATCHDOG_DISABLE_CODE2 = 0xD6BF,
    CHIP_HELPER_WATCHDOG_DISABLE_CODE3 = 0xC31E
};

/*                                          */
enum chip_helper_gbl_misc_enum
{
    CHIP_HELPER_GBL_MISC_SPI_STOP_OUT_EN_MASK  = 0x0001,
    CHIP_HELPER_GBL_MISC_MMU_INIT_DONE_MASK    = 0x0004
};

/*                                                                         
                                                                  
                                    */
enum chip_helper_coex_status_mask_enum
{
    CHIP_HELPER_COEX_STATUS_RST_PULLS_LSB_MASK   = 0x0001,
    CHIP_HELPER_COEX_STATUS_RST_PULLS_MSB_MASK   = 0x0008,
    CHIP_HELPER_COEX_STATUS_WL_FEC_PINS_LSB_MASK = 0x0010,
    CHIP_HELPER_COEX_STATUS_WL_FEC_PINS_MSB_MASK = 0x0080,
    CHIP_HELPER_COEX_STATUS_INT_UART_MASK        = 0x0100,
    CHIP_HELPER_COEX_STATUS_INT_BT_LEG_MASK      = 0x0200
};

/*                                  */
enum chip_helper_dbg_proc_sel_enum
{
    CHIP_HELPER_DBG_PROC_SEL_MAC  = 0,
    CHIP_HELPER_DBG_PROC_SEL_PHY  = 1,
    CHIP_HELPER_DBG_PROC_SEL_BT   = 2,
    CHIP_HELPER_DBG_PROC_SEL_NONE = 2,
    CHIP_HELPER_DBG_PROC_SEL_BOTH = 3
};

/*                                                      
                                               */
enum chip_helper_fixed_registers
{
    /*                                                
                         
                                 */
    CHIP_HELPER_UNIFI_GBL_CHIP_VERSION  = 0xFE81,

    CHIP_HELPER_OLD_BLUECORE_GBL_CHIP_VERSION = 0xFF9A

                                                /*                                                   
                                                                                          */
                                                /*                                                  */
};

/*                                                        */
struct chip_helper_init_values
{
    u16 addr;
    u16 value;
};

/*                                                      */
struct chip_helper_reset_values
{
    u32        gp_address;
    u32        len;
    const u16 *data;
};

/*
                     
 */

/*             */
typedef const struct chip_device_desc_t ChipDescript;

/*                          */
ChipDescript* ChipHelper_Null(void);

/*                                                      
                                                     
                                                       
                                                                
                         

                                                          
                                                         
                                                    */
ChipDescript* ChipHelper_GetVersionAny(u16 from_FF9A, u16 from_FE81);

/*                                                  
                                                                 */
ChipDescript* ChipHelper_GetVersionUniFi(u16 version);

/*                                                          
                                                                 
                                              */
ChipDescript* ChipHelper_GetVersionSdio(u8 sdio_version);

/*                                                               
                                                                  
                                                                   
                                                   */
ChipDescript* ChipHelper_GetVersionBlueCore(enum chip_helper_bluecore_age age,
                                            u16                     version);

/*                                                                  
                                                                    
                             

                                                                   
                                                                   
                                                                   
                                            

                                                                   
                                                                
                                                              
                               */
#define CHIP_HELPER_LIST(m)                                             \
    CHIP_HELPER_DEF0(m, (const char *, FriendlyName, friendly_name))     \
    CHIP_HELPER_DEF0(m, (const char *, MarketingName, marketing_name))  \
    CHIP_HELPER_DEF0(m, (u16, DBG_EMU_CMD, regs->dbg_emu_cmd))       \
    CHIP_HELPER_DEF0(m, (u16, DBG_HOST_PROC_SELECT, regs->host.dbg_proc_select)) \
    CHIP_HELPER_DEF0(m, (u16, DBG_HOST_STOP_STATUS, regs->host.dbg_stop_status)) \
    CHIP_HELPER_DEF0(m, (u16, HOST_WINDOW1_PAGE, regs->host.window1_page)) \
    CHIP_HELPER_DEF0(m, (u16, HOST_WINDOW2_PAGE, regs->host.window2_page)) \
    CHIP_HELPER_DEF0(m, (u16, HOST_WINDOW3_PAGE, regs->host.window3_page)) \
    CHIP_HELPER_DEF0(m, (u16, HOST_IO_LOG_ADDR, regs->host.io_log_addr)) \
    CHIP_HELPER_DEF0(m, (u16, DBG_SPI_PROC_SELECT, regs->spi.dbg_proc_select)) \
    CHIP_HELPER_DEF0(m, (u16, DBG_SPI_STOP_STATUS, regs->spi.dbg_stop_status)) \
    CHIP_HELPER_DEF0(m, (u16, SPI_WINDOW1_PAGE, regs->spi.window1_page)) \
    CHIP_HELPER_DEF0(m, (u16, SPI_WINDOW2_PAGE, regs->spi.window2_page)) \
    CHIP_HELPER_DEF0(m, (u16, SPI_WINDOW3_PAGE, regs->spi.window3_page)) \
    CHIP_HELPER_DEF0(m, (u16, SPI_IO_LOG_ADDR, regs->spi.io_log_addr)) \
    CHIP_HELPER_DEF0(m, (u16, DBG_RESET, regs->dbg_reset))           \
    CHIP_HELPER_DEF0(m, (u16, DBG_RESET_VALUE, regs->dbg_reset_value)) \
    CHIP_HELPER_DEF0(m, (u16, DBG_RESET_WARN, regs->dbg_reset_warn)) \
    CHIP_HELPER_DEF0(m, (u16, DBG_RESET_WARN_VALUE, regs->dbg_reset_warn_value)) \
    CHIP_HELPER_DEF0(m, (u16, DBG_RESET_RESULT, regs->dbg_reset_result)) \
    CHIP_HELPER_DEF0(m, (u16, WATCHDOG_DISABLE, regs->watchdog_disable)) \
    CHIP_HELPER_DEF0(m, (u16, PROC_PC_SNOOP, regs->proc_pc_snoop))   \
    CHIP_HELPER_DEF0(m, (u16, GBL_CHIP_VERSION, regs->gbl_chip_version)) \
    CHIP_HELPER_DEF0(m, (u16, GBL_MISC_ENABLES, regs->gbl_misc_enables)) \
    CHIP_HELPER_DEF0(m, (u16, XAP_PCH, regs->xap_pch))               \
    CHIP_HELPER_DEF0(m, (u16, XAP_PCL, regs->xap_pcl))               \
    CHIP_HELPER_DEF0(m, (u16, MAILBOX0, regs->mailbox0))             \
    CHIP_HELPER_DEF0(m, (u16, MAILBOX1, regs->mailbox1))             \
    CHIP_HELPER_DEF0(m, (u16, MAILBOX2, regs->mailbox2))             \
    CHIP_HELPER_DEF0(m, (u16, MAILBOX3, regs->mailbox3))             \
    CHIP_HELPER_DEF0(m, (u16, SDIO_HIP_HANDSHAKE, regs->sdio_hip_handshake))   \
    CHIP_HELPER_DEF0(m, (u16, SDIO_HOST_INT, regs->sdio_host_int))   \
    CHIP_HELPER_DEF0(m, (u16, COEX_STATUS, regs->coex_status))       \
    CHIP_HELPER_DEF0(m, (u16, SHARED_IO_INTERRUPT, regs->shared_io_interrupt)) \
    CHIP_HELPER_DEF0(m, (u32, PROGRAM_MEMORY_RAM_OFFSET, prog_offset.ram)) \
    CHIP_HELPER_DEF0(m, (u32, PROGRAM_MEMORY_ROM_OFFSET, prog_offset.rom)) \
    CHIP_HELPER_DEF0(m, (u32, PROGRAM_MEMORY_FLASH_OFFSET, prog_offset.flash)) \
    CHIP_HELPER_DEF0(m, (u32, PROGRAM_MEMORY_EXT_SRAM_OFFSET, prog_offset.ext_sram)) \
    CHIP_HELPER_DEF0(m, (u16, DATA_MEMORY_RAM_OFFSET, data_offset.ram)) \
    CHIP_HELPER_DEF0(m, (s32, HasFlash, bools.has_flash))              \
    CHIP_HELPER_DEF0(m, (s32, HasExtSram, bools.has_ext_sram))         \
    CHIP_HELPER_DEF0(m, (s32, HasRom, bools.has_rom))                  \
    CHIP_HELPER_DEF0(m, (s32, HasBt, bools.has_bt))                    \
    CHIP_HELPER_DEF0(m, (s32, HasWLan, bools.has_wlan))                \
    CHIP_HELPER_DEF1(m, (u16, WINDOW_ADDRESS, enum chip_helper_window_index, window)) \
    CHIP_HELPER_DEF1(m, (u16, WINDOW_SIZE, enum chip_helper_window_index, window)) \
    CHIP_HELPER_DEF1(m, (u16, MapAddress_SPI2HOST, u16, addr))          \
    CHIP_HELPER_DEF1(m, (u16, MapAddress_HOST2SPI, u16, addr))          \
    CHIP_HELPER_DEF1(m, (u32, ClockStartupSequence, const struct chip_helper_init_values **, val)) \
    CHIP_HELPER_DEF1(m, (u32, HostResetSequence, const struct chip_helper_reset_values **, val))

/*                                  */
#define CHIP_HELPER_DEF0(a, b) \
    CHIP_HELPER_DEF0_ ## a b
#define CHIP_HELPER_DEF1(a, b) \
    CHIP_HELPER_DEF1_ ## a b

/*                                                                        */
#define CHIP_HELPER_DEF0_C_DEC(ret_type, name, info)    \
    ret_type ChipHelper_ ## name(ChipDescript * chip_help);
#define CHIP_HELPER_DEF1_C_DEC(ret_type, name, type1, name1)   \
    ret_type ChipHelper_ ## name(ChipDescript * chip_help, type1 name1);

CHIP_HELPER_LIST(C_DEC)

/*             
                

                                                                  
                                                                     
                                                                      
                                   
*/
/*            
                   
                
                   
          
          

                                                
*/
/*                                           
                                                        
                   
                                           
                                                     
                  

                                                             
                                                             
                                                             
                                                 
*/
/*           
                   
                  
                        
                    

                                                           
                                                           
                                                              
                                                                   
                                                                 
                                                               
                   
*/
/*                  

                                                          
                                                              
                  
*/
/*          
            
            
            

                                                                
                                                                 
                                   
*/
/*                    

                                                                  
                                                               
                                       
*/
/*               
                       

                                                                   
                                                                   
                                                           
*/
/*             

                                                             
                                                      
*/
/*                           
                             
                               
                                  
                          

                                                                 
                                                     
*/
/*                            
                 

                                                 
*/
/*                            

                                                                   
                                                                 
                                                                
                  

                                                            
                                                              
                                                                
                         
*/
/*                     
                       

                                                                  
                                                                    
                                                               
                                           
*/
/*                      

                                                               
                                                                  
                                                                  
                                                                  
                                                                  
                                                                     
                                                    
*/
/*                   

                                                                 
                                                                  
                                                                 
                                                                  
                                                                
                                                                
                                                                
                                                                 
*/

/*                                                            
                                                            
                                                             

                                                           
                                                         
                                                          
                                                          
                       */
s32 ChipHelper_DecodeWindow(ChipDescript *chip_help,
                                 enum chip_helper_window_index window,
                                 enum chip_helper_window_type type,
                                 u32 offset,
                                 u16 *page, u16 *addr, u32 *len);

#endif
