#ifndef __CCCI_PLATFORM_H__
#define __CCCI_PLATFORM_H__
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/version.h>
#include <linux/fs.h>
#include <mach/irqs.h>
#include <mach/mt_irq.h>
#include <mach/mt_reg_base.h>
#include <mach/mt_typedefs.h>
#include <mach/mt_boot.h>
#include <mach/sync_write.h>
#include <asm/memory.h>
#include <asm/string.h>
#include <ccci_platform_cfg.h>



//                                                                 
#define CCCI_DEV_MAJOR 184


#define MD_IMG_RESRVED_SIZE        (0x700000)    //   
#define MD_RW_MEM_RESERVED_SIZE    (0xF00000)  //    

#if defined(CCCI_STATIC_SHARED_MEM)
#define CCCI_SHARED_MEM_SIZE UL(0x200000)  //                                                               
#endif

/*                     */
#define MD_INDEX 0
#define DSP_INDEX 1
#define IMG_CNT 1

//        
#define MOEDM_IMAGE_NAME "modem.img"
#define DSP_IMAGE_NAME "DSP_ROM"

#define MOEDM_IMAGE_E1_NAME "modem_E1.img"
#define DSP_IMAGE_E1_NAME "DSP_ROM_E1"

#define MOEDM_IMAGE_E2_NAME "modem_E2.img"
#define DSP_IMAGE_E2_NAME "DSP_ROM_E2"

//        
#define MOEDM_SYS2_IMAGE_NAME          "modem_sys2.img"

#define MOEDM_SYS2_IMAGE_E1_NAME     "modem_sys2_E1.img"

#define MOEDM_SYS2_IMAGE_E2_NAME     "modem_sys2_E2.img"



#ifndef CONFIG_MODEM_FIRMWARE_PATH
#define CONFIG_MODEM_FIRMWARE_PATH "/etc/firmware/"

#define CONFIG_MODEM_FIRMWARE_CIP_PATH    "/custom/etc/firmware/"

#define MOEDM_IMAGE_PATH "/etc/firmware/modem.img"
#define DSP_IMAGE_PATH "/etc/firmware/DSP_ROM"

#define MOEDM_IMAGE_E1_PATH "/etc/firmware/modem_E1.img"
#define DSP_IMAGE_E1_PATH "/etc/firmware/DSP_ROM_E1"

#define MOEDM_IMAGE_E2_PATH "/etc/firmware/modem_E2.img"
#define DSP_IMAGE_E2_PATH "/etc/firmware/DSP_ROM_E2"

#endif


/*                                   */
#define DSP_VER_3G  0x0
#define DSP_VER_2G  0x1
#define DSP_VER_INVALID  0x2

#define VER_3G_STR  "3G"
#define VER_2G_STR  "2G"
#define VER_WG_STR   "WG"
#define VER_TG_STR   "TG"
#define VER_INVALID_STR  "INVALID"


#define DEBUG_STR   "Debug"
#define RELEASE_STR  "Release"
#define INVALID_STR  "INVALID"

#define DSP_ROM_TYPE 0x0104
#define DSP_BL_TYPE  0x0003

#define DSP_ROM_STR  "DSP_ROM"
#define DSP_BL_STR   "DSP_BL"

#define MD_HEADER_MAGIC_NO "CHECK_HEADER"
//                                     

#define GFH_HEADER_MAGIC_NO 0x4D4D4D
//                             
#define GFH_FILE_INFO_TYPE 0x0
#define GFH_CHECK_HEADER_TYPE 0x104

#define DSP_2G_BIT 16
#define DSP_DEBUG_BIT 17



//                                                                   
/*                                                           */
/*                                    */
/*           */
#define WDT_MD_MODE_DEFAULT        (0x3)
#define WDT_MD_MODE_KEY            (0x22<<8)
#define WDT_MD_LENGTH_DEFAULT    (0x7FF<<5)
#define WDT_MD_LENGTH_KEY        (0x8)
#define WDT_MD_RESTART_KEY        (0x1971)

/*         */
#define WDT_DSP_MODE_DEFAULT    (0x3)
#define WDT_DSP_MODE_KEY        (0x22<<8)
#define WDT_DSP_LENGTH_DEFAULT    (0x7FF<<5)
#define WDT_DSP_LENGTH_KEY        (0x8)
#define WDT_DSP_RESTART_KEY        (0x1971)

/*           */
#define WDT_MD_MODE(base)        ((base) + 0x00)
#define WDT_MD_LENGTH(base)        ((base) + 0x04)
#define WDT_MD_RESTART(base)    ((base) + 0x08)
#define WDT_MD_STA(base)        ((base) + 0x0C)
#define WDT_MD_SWRST(base)        ((base) + 0x1C)


#define MD_INFRA_BASE              (0xD10D0000) //                       
#define MD_RGU_BASE              (0xD10C0000) //                       

#define CLK_SW_CON0             (0x00000910)
#define CLK_SW_CON1             (0x00000914)
#define CLK_SW_CON2             (0x00000918)
#define BOOT_JUMP_ADDR             (0x00000980)


/*                                                                           */
//                                
//           
//                                                                              
#define AP_BANK4_MAP0(base)                ((volatile unsigned int*)(base+0x200))
#define AP_BANK4_MAP1(base)                ((volatile unsigned int*)(base+0x204))

//                                  
//             
#define MD1_BANK0_MAP0(base)            ((volatile unsigned int*)(base+0x300))
#define MD1_BANK0_MAP1(base)            ((volatile unsigned int*)(base+0x304))
//             
#define MD1_BANK4_MAP0(base)            ((volatile unsigned int*)(base+0x308))
#define MD1_BANK4_MAP1(base)            ((volatile unsigned int*)(base+0x30C))


//                              
#define TOPAXI_AXI_ASLICE_CRL(base)        ((volatile unsigned int*)(base+0x22C))


//                                                                    
//                       
typedef struct {
    U32 m_magic_ver;          /*                                                      */
    U16 m_size;               /*                          */
    U16 m_type;               /*                                                           */
} GFH_HEADER;

typedef struct {
    GFH_HEADER m_gfh_hdr;     /*         */
    U8 m_identifier[12];      /*             */
    U32 m_file_ver;           /*                                                        */
    U16 m_file_type;          /*                              */
    U8 md_id;                   /*                        */
    U8 dummy2;
    U32 dummy3[7];
} GFH_FILE_INFO_v1;

typedef struct {
    GFH_HEADER m_gfh_hdr;      /*                          */
    U32 m_product_ver;        /*                                                     */
    U32 m_image_type;          /*                                          */
    U8 m_platform_id[16];       /*                             */
    U8 m_project_id[64];       /*                                       */
    U8 m_build_time[64];       /*                                   */
    U8 reserved[64];
}GFH_CHECK_CFG_v1;


typedef enum{
    INVALID_VARSION = 0,
    DEBUG_VERSION,
    RELEASE_VERSION
}PRODUCT_VER_TYPE;


typedef struct{
    U8 check_header[12];        /*                               */
    U32 header_verno;            /*                                 */
    U32 product_ver;            /*                                                     */
    U32 image_type;                /*                                          */
    U8 platform[16];            /*                          */
    U8 build_time[64];            /*                   */
    U8 build_ver[64];            /*                                   */
    U8 bind_sys_id;                /*                                           */
    U8 ext_attr;                /*                        */
    U8 reserved[2];             /*              */
    U32 mem_size;               /*                                       */
    U32 md_img_size;            /*                                 */
    U32 reserved_info;          /*              */
    U32 size;                    /*                            */
}MD_CHECK_HEADER;


typedef struct{
    unsigned long modem_region_base;
    unsigned long modem_region_len;
    unsigned long dsp_region_base;
    unsigned long dsp_region_len;
    unsigned long mdif_region_base;
    unsigned long mdif_region_len;
    unsigned long ccif_region_base;
    unsigned long ccif_region_len;
    unsigned long mdcfg_region_base;
    unsigned long mdcfg_region_len;
}CCCI_REGION_LAYOUT;

struct IMG_CHECK_INFO{
    char *product_ver;    /*                       */
    char *image_type;    /*             */
    char *platform;        /*                                              */
    char *build_time;    /*                   */
    char *build_ver;    /*                                   */
    unsigned int mem_size; /*                   */
    unsigned int md_img_size; /*                                          */
};


#define  NAME_LEN 100
#define  AP_PLATFORM_LEN 16
struct image_info
{
    int                type;            /*                                      */
    char            file_name[NAME_LEN];
    unsigned long    address;
    ssize_t            size;
    loff_t            offset;
    unsigned int    tail_length;
    char            *ap_platform;
    struct IMG_CHECK_INFO img_info;
    struct IMG_CHECK_INFO ap_info;
    int (*load_firmware)(int, struct image_info *info);
    unsigned int    flags;
};

/*                           */
typedef enum{
    LOAD_NONE = 0,
    LOAD_MD_ONLY = (1<<MD_INDEX),
    LOAD_DSP_ONLY = (1<<DSP_INDEX),
    LOAD_MD_DSP = (1<<MD_INDEX | 1<<DSP_INDEX),
}LOAD_FLAG;


//                                                                  
extern void dump_firmware_info(void);

extern unsigned int get_max_DRAM_size (void);
extern unsigned int get_phys_offset (void);
#endif //                   

