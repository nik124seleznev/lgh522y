
#ifndef __EMMC_INIT__
#define __EMMC_INIT__

#include <linux/mmc/card.h>
#include <mach/mt_typedefs.h>
#define MTK_MMC_DUMP_DBG	(0)
#define MAX_POLLING_STATUS (50000)

struct simp_mmc_card;
struct simp_msdc_host;
#define MSDC_PS_DAT0            (0x1  << 16)    /*    */
struct simp_mmc_host {
	int			index;
	unsigned int		f_min;
	unsigned int		f_max;
	unsigned int		f_init;
	u32			ocr_avail;
	u32			ocr_avail_sdio;	/*                   */
	u32			ocr_avail_sd;	/*                 */
	u32			ocr_avail_mmc;	/*                  */

	unsigned long		caps;		/*                   */
	unsigned int		caps2;		/*                        */

	/*                          */
	unsigned int		max_seg_size;	/*                                */
	unsigned short		max_segs;	/*                            */
	unsigned short		unused;
	unsigned int		max_req_size;	/*                                    */
	unsigned int		max_blk_size;	/*                               */
	unsigned int		max_blk_count;	/*                                     */
	unsigned int		max_discard_to;	/*                            */
	
	u32			ocr;		/*                         */

	struct simp_mmc_card		*card;		/*                              */

	unsigned int		actual_clock;	/*                      */

    /*                 */
    struct simp_msdc_host* mtk_host;
};

struct simp_mmc_card {
	struct simp_mmc_host		*host;		/*                                 */
	unsigned int		rca;		/*                                 */
	unsigned int		type;		/*           */
	unsigned int		state;		/*                  */
	unsigned int		quirks; 	/*             */

	unsigned int		erase_size;	/*                       */
 	unsigned int		erase_shift;	/*                          */
 	unsigned int		pref_erase;	/*            */
 	u8			erased_byte;	/*                       */

	u32			raw_cid[4];	/*              */
	u32			raw_csd[4];	/*              */
	u32			raw_scr[2];	/*              */
	struct mmc_cid		cid;		/*                     */
	struct mmc_csd		csd;		/*               */
	struct mmc_ext_csd	ext_csd;	/*                               */
	struct sd_scr		scr;		/*                      */
	struct sd_ssr		ssr;		/*                         */
	struct sd_switch_caps	sw_caps;	/*                    */

	unsigned int		sd_bus_speed;	/*                                 */
};

struct simp_msdc_card {
    unsigned int            rca;           /*                                 */
    unsigned int            type;          /*           */
    unsigned short          state;         /*                  */
    unsigned short          file_system;   /*             */ 
    unsigned short          card_cap;      /*                      */          
};

struct simp_msdc_host
{
    struct simp_msdc_card *card;
    void __iomem      *base;         /*                   */
    unsigned char     id;           /*                */
    unsigned int      clk;          /*                  */     /*                               */
    unsigned int      sclk;         /*                   */    /*               */
    unsigned char     clksrc;       /*              */
    void *priv;                     /*              */
};

typedef enum {
    MSDC_CLKSRC_200M = 0
} CLK_SOURCE_T;

enum {
    FAT16 = 0,
    FAT32 = 1,
    exFAT = 2,
    _RAW_ = 3,
};

enum {
    standard_capacity = 0,
    high_capacity = 1,
    extended_capacity = 2,
};

/*                */
#define CMD0     (0)    /*                           */
#define CMD1     (1)    /*                           */
#define CMD2     (2)    /*                           */
#define CMD3     (3)    /*                           */
#define CMD7     (7)    /*                           */
#define CMD8     (8)    /*                           */
#define CMD9     (9)    /*                           */
#define CMD10    (10)   /*                           */
#define CMD55    (55)   /*                           */
#define ACMD41   (41)   /*                           */
#define CMD13    (13)   /*                           */
#define ACMD42   (42)   /*                           */
#define ACMD6    (6)    /*                           */

//                                                                                            
#define CMD17    (17)
#define CMD18    (18)
#define CMD24    (24)
#define CMD25    (25)
#define CMD12    (12)  

/*                  */
#define CMD0_ARG             (0)

#define CMD8_ARG_VOL_27_36   (0x100)
#define CMD8_ARG_PATTERN     (0x5a)   /*         */
#define CMD8_ARG             (CMD8_ARG_VOL_27_36 | CMD8_ARG_PATTERN)

#define CMD55_ARG            (phost->card->rca) 

#define ACMD41_ARG_HCS       (1 << 30)
#define ACMD41_ARG_VOL_27_36 (0xff8000)
#define ACMD41_ARG_20        (ACMD41_ARG_VOL_27_36 | ACMD41_ARG_HCS) 
#define ACMD41_ARG_10        (ACMD41_ARG_VOL_27_36) 

#define CMD2_ARG             (0)
#define CMD3_ARG             (0)
#define CMD9_ARG             (phost->card->rca) 
#define CMD10_ARG            (phost->card->rca) 
#define CMD13_ARG            (phost->card->rca) 
#define CMD7_ARG             (phost->card->rca) 

#define ACMD42_ARG_CLR_CD    (0)
#define ACMD42_ARG_SET_CD    (1)
#define ACMD42_ARG           (ACMD42_ARG_CLR_CD) 
 
#define ACMD6_ARG_BUS_WIDTH_4 (0x2)
#define ACMD6_ARG             (ACMD6_ARG_BUS_WIDTH_4)
#define EXT_CSD_CMD_SET_NORMAL	(1<<0)
//                                         
//                                
//                                
#define ACMD6_ARG_EMMC		  ((MMC_SWITCH_MODE_WRITE_BYTE << 24) | (EXT_CSD_BUS_WIDTH << 16) | (EXT_CSD_BUS_WIDTH_4 << 8) | EXT_CSD_CMD_SET_NORMAL)
#ifdef MTK_MSDC_USE_CACHE
#define ACMD6_ARG_DISABLE_CACHE		  ((MMC_SWITCH_MODE_WRITE_BYTE << 24) | (EXT_CSD_CACHE_CTRL << 16) | (0 << 8) | EXT_CSD_CMD_SET_NORMAL)
#endif

#define CMD17_ARG    (data_address)  /*                          */
#define CMD18_ARG    (data_address)  /*                                     */
#define CMD24_ARG    (data_address)   
#define CMD25_ARG    (data_address)   
#define CMD12_ARG    (0) 
#define CMD8_RAW_EMMC     CMD_RAW(8 , msdc_rsp[RESP_R1]  , 1, 0,   512, 0)  /*            */

#define CMD_RAW(cmd, rspt, dtyp, rw, len, stop) \
          (cmd) | (rspt << 7) | \
          (dtyp << 11) | (rw << 13) | (len << 16) | \
          (stop << 14)  
       
/*                                       */          
#define CMD0_RAW     CMD_RAW(0 , msdc_rsp[RESP_NONE], 0, 0,   0, 0)
#define CMD1_RAW     CMD_RAW(1 , msdc_rsp[RESP_R3], 0, 0,   0, 0)
#define CMD2_RAW     CMD_RAW(2 , msdc_rsp[RESP_R2], 0, 0,   0, 0)
#define CMD3_RAW     CMD_RAW(3 , msdc_rsp[RESP_R1], 0, 0,   0, 0)
#define CMD7_RAW     CMD_RAW(7 , msdc_rsp[RESP_R1], 0, 0,   0, 0)
#define CMD8_RAW     CMD_RAW(8 , msdc_rsp[RESP_R7]  , 0, 0,   0, 0)  /*            */
#define CMD9_RAW     CMD_RAW(9 , msdc_rsp[RESP_R2]  , 0, 0,   0, 0)
#define CMD10_RAW    CMD_RAW(10, msdc_rsp[RESP_R2]  , 0, 0,   0, 0)
#define CMD55_RAW    CMD_RAW(55, msdc_rsp[RESP_R1]  , 0, 0,   0, 0)  /*            */
#define ACMD41_RAW   CMD_RAW(41, msdc_rsp[RESP_R3]  , 0, 0,   0, 0)
#define CMD13_RAW    CMD_RAW(13, msdc_rsp[RESP_R1]  , 0, 0,   0, 0)
#define ACMD42_RAW   CMD_RAW(42, msdc_rsp[RESP_R1]  , 0, 0,   0, 0)
#define ACMD6_RAW    CMD_RAW(6 , msdc_rsp[RESP_R1]  , 0, 0,   0, 0)
#define ACMD6_RAW_EMMC    CMD_RAW(6 , msdc_rsp[RESP_R1B]  , 0, 0,   0, 0)

/*                                */
#define CMD17_RAW    CMD_RAW(17, msdc_rsp[RESP_R1]  , 1, 0, 512, 0)  /*                     */    
#define CMD18_RAW    CMD_RAW(18, msdc_rsp[RESP_R1]  , 2, 0, 512, 0)  /*                     */ 
#define CMD24_RAW    CMD_RAW(24, msdc_rsp[RESP_R1]  , 1, 1, 512, 0)  /*                     */ 
#define CMD25_RAW    CMD_RAW(25, msdc_rsp[RESP_R1]  , 2, 1, 512, 0)  /*                     */  
#define CMD12_RAW    CMD_RAW(12, msdc_rsp[RESP_R1B] , 0, 0,   0, 1)  

/*                  */
#define R3_OCR_POWER_UP_BIT        (1 << 31)
#define R3_OCR_CARD_CAPACITY_BIT   (1 << 30)

#define REG_VEMC33_VOLSEL (0x0A4A)
#define REG_VEMC33_EN     (0x0A18)
#define REG_VMC_VOLSEL    (0x0A48)
#define REG_VMC_EN        (0x0A16)
#define REG_VMCH_VOLSEL   (0x0A48)
#define REG_VMCH_EN       (0x0A14)


#define MASK_VEMC33_VOLSEL (0x1 << 9)
#define MASK_VEMC33_EN     (0x1 << 1)
#define MASK_VMC_VOLSEL    (0x1 << 1)
#define MASK_VMC_EN        (0x1 << 1)
#define MASK_VMCH_VOLSEL   (0x1 << 9)
#define MASK_VMCH_EN       (0x1 << 1)


#endif /*                      */




