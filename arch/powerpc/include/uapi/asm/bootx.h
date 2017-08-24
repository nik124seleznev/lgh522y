/*
                                                                      
                                             
  
                                     
 */


#ifndef _UAPI__ASM_BOOTX_H__
#define _UAPI__ASM_BOOTX_H__

#include <linux/types.h>

#ifdef macintosh
#include <Types.h>
#include "linux_type_defs.h"
#endif

#ifdef macintosh
/*                                     */
#pragma options align=power
#endif

/*                 
  
                              
                             
            
  
                                                        
                                                        
                                         
 */

#define BOOT_INFO_VERSION               5
#define BOOT_INFO_COMPATIBLE_VERSION    1

/*                                                         
                                                     
                                                            
                                                
 */
#define BOOT_ARCH_PCI                   0x00000001UL
#define BOOT_ARCH_NUBUS                 0x00000002UL
#define BOOT_ARCH_NUBUS_PDM             0x00000010UL
#define BOOT_ARCH_NUBUS_PERFORMA        0x00000020UL
#define BOOT_ARCH_NUBUS_POWERBOOK       0x00000040UL

/*                                              */
#define MAX_MEM_MAP_SIZE				26

/*                                                                       
                                                                       
            */
typedef struct boot_info_map_entry
{
    __u32       physAddr;                /*                           */
    __u32       size;                    /*               */
} boot_info_map_entry_t;


/*                                                                
                                                                  
                                 */
typedef struct boot_infos
{
    /*                           */
    __u32       version;
    /*                                      */
    __u32       compatible_version;

    /*                                                            
                                                        */
    __u8*       logicalDisplayBase;

    /*                                              */
    __u32       machineID;

    /*                                        */
    __u32       architecture;

    /*                                                                           
                                                                       
                                                                              
                  
     */
    __u32       deviceTreeOffset;        /*                    */
    __u32       deviceTreeSize;          /*                         */

    /*                                            */
    __u32       dispDeviceRect[4];       /*                       */
    __u32       dispDeviceDepth;         /*               */
    __u8*       dispDeviceBase;          /*                         */
    __u32       dispDeviceRowBytes;      /*                     */
    __u32       dispDeviceColorsOffset;  /*                                 */
    /*                                               
                                                   */
     __u32      dispDeviceRegEntryOffset;

    /*                                                               */
    __u32       ramDisk;
    __u32       ramDiskSize;             /*                       */

    /*                                                            */
    __u32       kernelParamsOffset;

    /*                         */

    /*                                                                  
                                                                           
                    */
    boot_info_map_entry_t
    	        physMemoryMap[MAX_MEM_MAP_SIZE]; /*                          */
    __u32       physMemoryMapSize;               /*                         */


    /*                                              */
    __u32       frameBufferSize;         /*                                  */

    /*               */

    /*                                                             */
    __u32       totalParamsSize;

} boot_infos_t;


#ifdef macintosh
#pragma options align=reset
#endif

#endif /*                      */
