/*****************************************************************************

            (c) Cambridge Silicon Radio Limited 2011
            All rights reserved and confidential information of CSR

            Refer to LICENSE.txt included with this source for details
            on the license terms.

*****************************************************************************/

#include "csr_macro.h"
#include "csr_wifi_hip_chiphelper_private.h"

#ifndef nelem
#define nelem(a) (sizeof(a) / sizeof(a[0]))
#endif

#define counted(foo) { nelem(foo), foo }
#define null_counted()  { 0, NULL }

/*                                                          
                                                               
                                                              
                                                            
                                                              
                       */
static const struct chip_helper_init_values init_vals_v1[] = {
    { 0xFDBB, 0xFFFF },
    { 0xFDB6, 0x03FF },
    { 0xFDB1, 0x01E3 },
    { 0xFDB3, 0x0FFF },
    { 0xFEE3, 0x08F0 },
    { 0xFEE7, 0x3C3F },
    { 0xFEE6, 0x0050 },
    { 0xFDBA, 0x0000 }
};

static const struct chip_helper_init_values init_vals_v2[] = {
    { 0xFDB6, 0x0FFF },
    { 0xF023, 0x3F3F },
    { 0xFDB1, 0x01E3 },
    { 0xFDB3, 0x0FFF },
    { 0xF003, 0x08F0 },
    { 0xF007, 0x3C3F },
    { 0xF006, 0x0050 }
};


static const struct chip_helper_init_values init_vals_v22_v23[] = {
    { 0xF81C, 0x00FF },
    /*                    */
    { 0xF80C, 0x1FFF },
    { 0xFA25, 0x001F },
    { 0xF804, 0x00FF },
    { 0xF802, 0x0FFF },
    /*                   
                         
                        */
};

static const u16 reset_program_a_v1_or_v2[] = {
    0x0000
};
static const u16 reset_program_b_v1_or_v2[] = {
    0x0010, 0xFE00, 0xA021, 0xFF00, 0x8111, 0x0009, 0x0CA4, 0x0114,
    0x0280, 0x04F8, 0xFE00, 0x6F25, 0x06E0, 0x0010, 0xFC00, 0x0121,
    0xFC00, 0x0225, 0xFE00, 0x7125, 0xFE00, 0x6D11, 0x03F0, 0xFE00,
    0x6E25, 0x0008, 0x00E0
};

static const struct chip_helper_reset_values reset_program_v1_or_v2[] =
{
    {
        MAKE_GP(REGISTERS, 0x000C),
        nelem(reset_program_a_v1_or_v2),
        reset_program_a_v1_or_v2
    },
    {
        MAKE_GP(MAC_PMEM, 0x000000),
        nelem(reset_program_b_v1_or_v2),
        reset_program_b_v1_or_v2
    }
};

static const struct chip_map_address_t unifi_map_address_v1_v2[] =
{
    { 0xFE9F, 0xFE7B },     /*                 */
    { 0xFE9E, 0xFE78 },     /*                 */
    { 0xFE9D, 0xFE7E },     /*                  */
    { 0xFE91, 0xFE90 },     /*             */
    { 0xFE8D, 0xFE8C },     /*             */
};

static const struct chip_map_address_t unifi_map_address_v22_v23[] =
{
    { 0xF8F9, 0xF8AC },     /*            */
    { 0xF8FA, 0xF8AD },     /*            */
    { 0xF8FB, 0xF8AE },     /*            */
    { 0xF830, 0xF81E },     /*             */
    { 0xF831, 0xF81F },     /*             */
    { 0xF8FC, 0xF8AF },     /*                */
};

static const struct chip_device_regs_t unifi_device_regs_null =
{
    0xFE81,                     /*                  */
    0x0000,                     /*                  */
    0x0000,                     /*             */
    {
        0x0000,                 /*                      */
        0x0000,                 /*                      */
        0x0000,                 /*                   */
        0x0000,                 /*                   */
        0x0000,                 /*                   */
        0x0000                  /*                  */
    },
    {
        0x0000,                 /*                     */
        0x0000,                 /*                     */
        0x0000,                 /*                  */
        0x0000,                 /*                  */
        0x0000,                 /*                  */
        0x0000                  /*                 */
    },
    0x0000,                     /*           */
    0x0000,                     /*                   */
    0x0000,                     /*                */
    0x0000,                     /*                      */
    0x0000,                     /*                  */
    0xFFE9,                     /*         */
    0xFFEA,                     /*         */
    0x0000,                     /*               */
    0x0000,                     /*                  */
    0x0000,                     /*          */
    0x0000,                     /*          */
    0x0000,                     /*          */
    0x0000,                     /*          */
    0x0000,                     /*               */
    0x0000,                     /*                     */
    0x0000,                     /*                    */
    0x0000                      /*             */
};

/*        */
static const struct chip_device_regs_t unifi_device_regs_v1 =
{
    0xFE81,                     /*                  */
    0xFE87,                     /*                  */
    0xFE9C,                     /*             */
    {
        0xFE90,                 /*                      */
        0xFE8C,                 /*                      */
        0xFE7B,                 /*                   */
        0xFE78,                 /*                   */
        0xFE7E,                 /*                   */
        0x0000                  /*                  */
    },
    {
        0xFE91,                 /*                     */
        0xFE8D,                 /*                     */
        0xFE9F,                 /*                  */
        0xFE9E,                 /*                  */
        0xFE9D,                 /*                  */
        0x0000                  /*                 */
    },
    0xFE92,                     /*           */
    0x0001,                     /*                   */
    0xFDA0,                     /*                              */
    0x0000,                     /*                      */
    0xFE92,                     /*                  */
    0xFFE9,                     /*         */
    0xFFEA,                     /*         */
    0x0051,                     /*               */
    0xFE70,                     /*                  */
    0xFE6B,                     /*          */
    0xFE6A,                     /*          */
    0xFE69,                     /*          */
    0xFE68,                     /*          */
    0xFE67,                     /*               */
    0xFE65,                     /*                     */
    0xFDE9,                     /*                    */
    0x0000                      /*             */
};

/*        */
static const struct chip_device_regs_t unifi_device_regs_v2 =
{
    0xFE81,                     /*                  */
    0xFE87,                     /*                  */
    0xFE9C,                     /*             */
    {
        0xFE90,                 /*                      */
        0xFE8C,                 /*                      */
        0xFE7B,                 /*                   */
        0xFE78,                 /*                   */
        0xFE7E,                 /*                   */
        0x0000                  /*                  */
    },
    {
        0xFE91,                 /*                     */
        0xFE8D,                 /*                     */
        0xFE9F,                 /*                  */
        0xFE9E,                 /*                  */
        0xFE9D,                 /*                  */
        0x0000                  /*                 */
    },
    0xFE92,                     /*           */
    0x0000,                     /*                   */
    0xFDE9,                     /*                                                     */
    0xFFFF,                     /*                      */
    0xFDE9,                     /*                                    */
    0xFFE9,                     /*         */
    0xFFEA,                     /*         */
    0x0051,                     /*               */
    0xFE70,                     /*                  */
    0xFE6B,                     /*          */
    0xFE6A,                     /*          */
    0xFE69,                     /*          */
    0xFE68,                     /*          */
    0xFE67,                     /*               */
    0xFE65,                     /*                     */
    0xFE69,                     /*                    */
    0x0000                      /*             */
};

/*        */
static const struct chip_device_regs_t unifi_device_regs_v22_v23 =
{
    0xFE81,                     /*                  */
    0xF84F,                     /*                  */
    0xF81D,                     /*             */
    {
        0xF81E,                 /*                      */
        0xF81F,                 /*                      */
        0xF8AC,                 /*                   */
        0xF8AD,                 /*                   */
        0xF8AE,                 /*                   */
        0xF8AF                  /*                  */
    },
    {
        0xF830,                 /*                     */
        0xF831,                 /*                     */
        0xF8F9,                 /*                  */
        0xF8FA,                 /*                  */
        0xF8FB,                 /*                  */
        0xF8FC                  /*                 */
    },
    0xF82F,                     /*           */
    0x0001,                     /*                   */
    0x0000,                     /*                */
    0x0000,                     /*                      */
    0xF82F,                     /*                  */
    0xFFE9,                     /*         */
    0xFFEA,                     /*         */
    0x001B,                     /*               */
    0x0055,                     /*                  */
    0xF84B,                     /*          */
    0xF84C,                     /*          */
    0xF84D,                     /*          */
    0xF84E,                     /*          */
    0xF92F,                     /*               */
    0xF92B,                     /*                                              */
    0xF84D,                     /*                               */
    0xF9FB                      /*             */
};

/*                                  */
static const struct window_shift_info_t prog_window_array_unifi_v1_v2[CHIP_HELPER_WT_COUNT] =
{
    { TRUE, 11, 0x0200 }, /*          */
    { TRUE, 11, 0x0000 }, /*       */
    { TRUE, 11, 0x0400 }, /*               */
    { FALSE, 0, 0 },      /*     */
    { FALSE, 0, 0 }       /*        */
};

/*                                 */
static const struct window_shift_info_t shared_window_array_unifi_v1_v2[CHIP_HELPER_WT_COUNT] =
{
    { FALSE, 0, 0 },      /*          */
    { FALSE, 0, 0 },      /*       */
    { FALSE, 0, 0 },      /*               */
    { FALSE, 0, 0 },      /*     */
    { TRUE, 11, 0x0000 }  /*        */
};

/*                                                 */
static const struct window_shift_info_t generic_window_array_unifi_v22_v23[CHIP_HELPER_WT_COUNT] =
{
    { TRUE, 11, 0x3800 }, /*          */
    { FALSE, 0, 0 },      /*       */
    { FALSE, 0, 0 },      /*               */
    { TRUE, 11, 0x2000 }, /*     */
    { TRUE, 11, 0x0000 }  /*        */
};

/*                              */
static const struct window_info_t prog1_window_unifi_v1_v2  = { 0x0000, 0x2000, 0x0080, prog_window_array_unifi_v1_v2 };
static const struct window_info_t prog2_window_unifi_v1_v2  = { 0x2000, 0x2000, 0x0000, prog_window_array_unifi_v1_v2 };
static const struct window_info_t shared_window_unifi_v1_v2 = { 0x4000, 0x2000, 0x0000, shared_window_array_unifi_v1_v2 };

/*                                        */
static const struct window_info_t generic1_window_unifi_v22_v23 = { 0x0000, 0x2000, 0x0080, generic_window_array_unifi_v22_v23 };
static const struct window_info_t generic2_window_unifi_v22_v23 = { 0x2000, 0x2000, 0x0000, generic_window_array_unifi_v22_v23 };
static const struct window_info_t generic3_window_unifi_v22_v23 = { 0x4000, 0x2000, 0x0000, generic_window_array_unifi_v22_v23 };

static const struct chip_device_desc_t chip_device_desc_null =
{
    { FALSE, 0x0000, 0x0000, 0x00 },
    "",
    "",
    null_counted(),                         /*      */
    null_counted(),                         /*            */
    &unifi_device_regs_null,                /*      */
    {
        FALSE,                              /*           */
        FALSE,                              /*              */
        FALSE,                              /*         */
        FALSE,                              /*        */
        FALSE,                              /*          */
    },
    null_counted(),
    /*             */
    {
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000
    },
    /*             */
    {
        0x0000                              /*     */
    },
    /*         */
    {
        NULL,
        NULL,
        NULL
    }
};

static const struct chip_device_desc_t unifi_device_desc_v1 =
{
    { FALSE, 0xf0ff, 0x1001, 0x01 },        /*            */
    "UF105x",
    "UniFi-1",
    counted(init_vals_v1),                  /*      */
    counted(reset_program_v1_or_v2),        /*            */
    &unifi_device_regs_v1,                  /*      */
    {
        TRUE,                               /*              */
        TRUE,                               /*              */
        FALSE,                              /*              */
        FALSE,                              /*              */
        TRUE,                               /*          */
    },
    counted(unifi_map_address_v1_v2),       /*     */
    /*             */
    {
        0x00100000,                         /*     */
        0x00000000,                         /*               */
        0x00000000,                         /*       */
        0x00200000,                         /*         */
    },
    /*             */
    {
        0x8000                              /*     */
    },
    /*         */
    {
        &prog1_window_unifi_v1_v2,
        &prog2_window_unifi_v1_v2,
        &shared_window_unifi_v1_v2
    }
};

static const struct chip_device_desc_t unifi_device_desc_v2 =
{
    { FALSE, 0xf0ff, 0x2001, 0x02 },        /*            */
    "UF2...",
    "UniFi-2",
    counted(init_vals_v2),                  /*      */
    counted(reset_program_v1_or_v2),        /*            */
    &unifi_device_regs_v2,                  /*      */
    {
        TRUE,                               /*              */
        TRUE,                               /*              */
        FALSE,                              /*              */
        FALSE,                              /*             */
        TRUE,                               /*          */
    },
    counted(unifi_map_address_v1_v2),       /*     */
    /*             */
    {
        0x00100000,                         /*     */
        0x00000000,                         /*               */
        0x00000000,                         /*       */
        0x00200000,                         /*         */
    },
    /*             */
    {
        0x8000                              /*     */
    },
    /*         */
    {
        &prog1_window_unifi_v1_v2,
        &prog2_window_unifi_v1_v2,
        &shared_window_unifi_v1_v2
    }
};

static const struct chip_device_desc_t unifi_device_desc_v3 =
{
    { FALSE, 0xf0ff, 0x3001, 0x02 },        /*            */
    "UF2...",
    "UniFi-3",
    counted(init_vals_v2),                  /*      */
    counted(reset_program_v1_or_v2),        /*            */
    &unifi_device_regs_v2,                  /*      */
    {
        TRUE,                               /*              */
        TRUE,                               /*              */
        FALSE,                              /*              */
        FALSE,                              /*             */
        TRUE,                               /*          */
    },
    counted(unifi_map_address_v1_v2),       /*     */
    /*             */
    {
        0x00100000,                         /*     */
        0x00000000,                         /*               */
        0x00000000,                         /*       */
        0x00200000,                         /*         */
    },
    /*             */
    {
        0x8000                              /*     */
    },
    /*         */
    {
        &prog1_window_unifi_v1_v2,
        &prog2_window_unifi_v1_v2,
        &shared_window_unifi_v1_v2
    }
};

static const struct chip_device_desc_t unifi_device_desc_v22 =
{
    { FALSE, 0x00ff, 0x0022, 0x07 },        /*        */
    "UF60xx",
    "UniFi-4",
    counted(init_vals_v22_v23),             /*      */
    null_counted(),                         /*            */
    &unifi_device_regs_v22_v23,             /*      */
    {
        FALSE,                              /*              */
        FALSE,                              /*              */
        TRUE,                               /*              */
        FALSE,                              /*              */
        TRUE,                               /*          */
    },
    counted(unifi_map_address_v22_v23),     /*     */
    /*             */
    {
        0x00C00000,                         /*     */
        0x00000000,                         /*     */
        0x00000000,                         /*                 */
        0x00000000,                         /*                   */
    },
    /*             */
    {
        0x8000                              /*     */
    },
    /*         */
    {
        &generic1_window_unifi_v22_v23,
        &generic2_window_unifi_v22_v23,
        &generic3_window_unifi_v22_v23
    }
};

static const struct chip_device_desc_t unifi_device_desc_v23 =
{
    { FALSE, 0x00ff, 0x0023, 0x08 },        /*        */
    "UF....",
    "UF.... (5)",
    counted(init_vals_v22_v23),             /*      */
    null_counted(),                         /*            */
    &unifi_device_regs_v22_v23,             /*      */
    {
        FALSE,                              /*              */
        FALSE,                              /*              */
        TRUE,                               /*              */
        TRUE,                               /*              */
        TRUE,                               /*          */
    },
    counted(unifi_map_address_v22_v23),
    /*             */
    {
        0x00C00000,                         /*     */
        0x00000000,                         /*     */
        0x00000000,                         /*                 */
        0x00000000,                         /*                    */
    },
    /*             */
    {
        0x8000                              /*     */
    },
    /*         */
    {
        &generic1_window_unifi_v22_v23,
        &generic2_window_unifi_v22_v23,
        &generic3_window_unifi_v22_v23
    }
};

static const struct chip_device_desc_t hyd_wlan_subsys_desc_v1 =
{
    { FALSE, 0x00ff, 0x0044, 0x00 },        /*        */
    "HYD...",
    "HYD...    ",
    counted(init_vals_v22_v23),             /*      */
    null_counted(),                         /*            */
    &unifi_device_regs_v22_v23,             /*      */
    {
        FALSE,                              /*              */
        FALSE,                              /*              */
        TRUE,                               /*              */
        FALSE,                              /*              */
        TRUE,                               /*          */
    },
    counted(unifi_map_address_v22_v23),
    /*             */
    {
        0x00C00000,                         /*     */
        0x00000000,                         /*     */
        0x00000000,                         /*                 */
        0x00000000,                         /*                    */
    },
    /*             */
    {
        0x8000                              /*     */
    },
    /*         */
    {
        &generic1_window_unifi_v22_v23,
        &generic2_window_unifi_v22_v23,
        &generic3_window_unifi_v22_v23
    }
};


/*                                                       
                                                      
                                                         
                                                        
                                                        
                                                        
                               

                                                          
               

                                                         
                                                           
                                                          
          */
static const struct chip_device_desc_t *chip_ver_to_desc[] =
{
    &unifi_device_desc_v1,      /*            */
    &unifi_device_desc_v2,      /*            */
    &unifi_device_desc_v3,      /*            */
    &unifi_device_desc_v22,     /*        */
    &unifi_device_desc_v23,     /*        */
    &hyd_wlan_subsys_desc_v1
};

ChipDescript* ChipHelper_GetVersionSdio(u8 sdio_ver)
{
    u32 i;

    for (i = 0; i < nelem(chip_ver_to_desc); i++)
    {
        if (chip_ver_to_desc[i]->chip_version.sdio == sdio_ver)
        {
            return chip_ver_to_desc[i];
        }
    }

    return &chip_device_desc_null;
}


ChipDescript* ChipHelper_GetVersionAny(u16 from_FF9A, u16 from_FE81)
{
    u32 i;

    if ((from_FF9A & 0xFF00) != 0)
    {
        for (i = 0; i < nelem(chip_ver_to_desc); i++)
        {
            if (chip_ver_to_desc[i]->chip_version.pre_bc7 &&
                ((from_FF9A & chip_ver_to_desc[i]->chip_version.mask) ==
                 chip_ver_to_desc[i]->chip_version.result))
            {
                return chip_ver_to_desc[i];
            }
        }
    }
    else
    {
        for (i = 0; i < nelem(chip_ver_to_desc); i++)
        {
            if (!chip_ver_to_desc[i]->chip_version.pre_bc7 &&
                ((from_FE81 & chip_ver_to_desc[i]->chip_version.mask) ==
                 chip_ver_to_desc[i]->chip_version.result))
            {
                return chip_ver_to_desc[i];
            }
        }
    }

    return &chip_device_desc_null;
}


ChipDescript* ChipHelper_GetVersionUniFi(u16 ver)
{
    return ChipHelper_GetVersionAny(0x0000, ver);
}


ChipDescript *ChipHelper_Null(void)
{
    return &chip_device_desc_null;
}


ChipDescript* ChipHelper_GetVersionBlueCore(enum chip_helper_bluecore_age bc_age, u16 version)
{
    if (bc_age == chip_helper_bluecore_pre_bc7)
    {
        return ChipHelper_GetVersionAny(version, 0x0000);
    }
    else
    {
        return ChipHelper_GetVersionAny(0x0000, version);
    }
}


/*                                                         
                                                          
                           */
#define CHIP_HELPER_DEF0_C_DEF(ret_type, name, info)            \
    ret_type ChipHelper_ ## name(ChipDescript * chip_help)           \
    {                                                               \
        return chip_help->info;                                     \
    }
#define CHIP_HELPER_DEF1_C_DEF(ret_type, name, type1, name1)

CHIP_HELPER_LIST(C_DEF)

/*
                                                      
 */
u16 ChipHelper_MapAddress_SPI2HOST(ChipDescript *chip_help, u16 addr)
{
    u32 i;
    for (i = 0; i < chip_help->map.len; i++)
    {
        if (chip_help->map.vals[i].spi == addr)
        {
            return chip_help->map.vals[i].host;
        }
    }
    return addr;
}


u16 ChipHelper_MapAddress_HOST2SPI(ChipDescript *chip_help, u16 addr)
{
    u32 i;
    for (i = 0; i < chip_help->map.len; i++)
    {
        if (chip_help->map.vals[i].host == addr)
        {
            return chip_help->map.vals[i].spi;
        }
    }
    return addr;
}


/*                                                          
                                                          
                                                          
                                                            
                                                           
                                                  */
u16 ChipHelper_WINDOW_ADDRESS(ChipDescript                 *chip_help,
                                    enum chip_helper_window_index window)
{
    if (window < CHIP_HELPER_WINDOW_COUNT &&
        chip_help->windows[window] != NULL)
    {
        return chip_help->windows[window]->address + chip_help->windows[window]->blocked;
    }
    return 0;
}


/*                                                               */
u16 ChipHelper_WINDOW_SIZE(ChipDescript                 *chip_help,
                                 enum chip_helper_window_index window)
{
    if (window < CHIP_HELPER_WINDOW_COUNT &&
        chip_help->windows[window] != NULL)
    {
        return chip_help->windows[window]->size - chip_help->windows[window]->blocked;
    }
    return 0;
}


/*                                                       
                                            */
u32 ChipHelper_ClockStartupSequence(ChipDescript                          *chip_help,
                                          const struct chip_helper_init_values **val)
{
    *val = chip_help->init.vals;
    return chip_help->init.len;
}


/*                                                                           */
u32 ChipHelper_HostResetSequence(ChipDescript                           *chip_help,
                                       const struct chip_helper_reset_values **val)
{
    *val = chip_help->reset_prog.vals;
    return chip_help->reset_prog.len;
}


/*                                       */
s32 ChipHelper_DecodeWindow(ChipDescript *chip_help,
                                 enum chip_helper_window_index window,
                                 enum chip_helper_window_type type,
                                 u32 offset,
                                 u16 *page, u16 *addr, u32 *len)
{
    const struct window_info_t *win;
    const struct window_shift_info_t *mode;
    u16 of, pg;

    if (window >= CHIP_HELPER_WINDOW_COUNT)
    {
        return FALSE;
    }
    if ((win = chip_help->windows[window]) == NULL)
    {
        return FALSE;
    }
    if (type >= CHIP_HELPER_WT_COUNT)
    {
        return FALSE;
    }
    if ((mode = &win->mode[type]) == NULL)
    {
        return FALSE;
    }
    if (!mode->allowed)
    {
        return FALSE;
    }

    pg = (u16)(offset >> mode->page_shift) + mode->page_offset;
    of = (u16)(offset & ((1 << mode->page_shift) - 1));
    /*                                                      
                                                                
                                            */
    while (of < win->blocked)
    {
        of += 1 << mode->page_shift;
        pg--;
    }
    *page = pg;
    *addr = win->address + of;
    *len = win->size - of;
    return TRUE;
}


