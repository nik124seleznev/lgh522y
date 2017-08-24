/*
 * Copyright 2011 Analog Devices Inc.
 *
 * Licensed under the Clear BSD license or the GPL-2 (or later)
 */

#ifndef _DEF_BF60X_H
#define _DEF_BF60X_H


/*                                                                */
/*                                                                */
/*                                                                */


/*                          
                     
                             */

/*                          
            
                             */
#define CNT_CONFIG                 0xFFC00400         /*                             */
#define CNT_IMASK                  0xFFC00404         /*                              */
#define CNT_STATUS                 0xFFC00408         /*                      */
#define CNT_COMMAND                0xFFC0040C         /*                       */
#define CNT_DEBOUNCE               0xFFC00410         /*                        */
#define CNT_COUNTER                0xFFC00414         /*                       */
#define CNT_MAX                    0xFFC00418         /*                             */
#define CNT_MIN                    0xFFC0041C         /*                             */


/*                          
                     
                             */

#define RSI_CLK_CONTROL            0xFFC00604         /*                             */
#define RSI_ARGUMENT               0xFFC00608         /*                        */
#define RSI_COMMAND                0xFFC0060C         /*                       */
#define RSI_RESP_CMD               0xFFC00610         /*                                */
#define RSI_RESPONSE0              0xFFC00614         /*                          */
#define RSI_RESPONSE1              0xFFC00618         /*                          */
#define RSI_RESPONSE2              0xFFC0061C         /*                          */
#define RSI_RESPONSE3              0xFFC00620         /*                          */
#define RSI_DATA_TIMER             0xFFC00624         /*                          */
#define RSI_DATA_LGTH              0xFFC00628         /*                           */
#define RSI_DATA_CONTROL           0xFFC0062C         /*                            */
#define RSI_DATA_CNT               0xFFC00630         /*                          */
#define RSI_STATUS                 0xFFC00634         /*                      */
#define RSI_STATUSCL               0xFFC00638         /*                            */
#define RSI_MASK0                  0xFFC0063C         /*                                */
#define RSI_MASK1                  0xFFC00640         /*                                */
#define RSI_FIFO_CNT               0xFFC00648         /*                            */
#define RSI_CEATA_CONTROL          0xFFC0064C         /*                                                */
#define RSI_BOOT_TCNTR             0xFFC00650         /*                                   */
#define RSI_BACK_TOUT              0xFFC00654         /*                                        */
#define RSI_SLP_WKUP_TOUT          0xFFC00658         /*                                    */
#define RSI_BLKSZ                  0xFFC0065C         /*                          */
#define RSI_FIFO                   0xFFC00680         /*                         */
#define RSI_ESTAT                  0xFFC006C0         /*                                */
#define RSI_EMASK                  0xFFC006C4         /*                              */
#define RSI_CONFIG                 0xFFC006C8         /*                             */
#define RSI_RD_WAIT_EN             0xFFC006CC         /*                                */
#define RSI_PID0                   0xFFC006D0         /*                                         */
#define RSI_PID1                   0xFFC006D4         /*                                         */
#define RSI_PID2                   0xFFC006D8         /*                                         */
#define RSI_PID3                   0xFFC006DC         /*                                         */

/*                          
                     
                             */

/*                          
            
                             */
#define CAN0_MC1                    0xFFC00A00         /*                                       */
#define CAN0_MD1                    0xFFC00A04         /*                                   */
#define CAN0_TRS1                   0xFFC00A08         /*                                          */
#define CAN0_TRR1                   0xFFC00A0C         /*                                            */
#define CAN0_TA1                    0xFFC00A10         /*                                          */
#define CAN0_AA1                    0xFFC00A14         /*                                   */
#define CAN0_RMP1                   0xFFC00A18         /*                                         */
#define CAN0_RML1                   0xFFC00A1C         /*                                      */
#define CAN0_MBTIF1                 0xFFC00A20         /*                                                 */
#define CAN0_MBRIF1                 0xFFC00A24         /*                                                */
#define CAN0_MBIM1                  0xFFC00A28         /*                                        */
#define CAN0_RFH1                   0xFFC00A2C         /*                                       */
#define CAN0_OPSS1                  0xFFC00A30         /*                                                               */
#define CAN0_MC2                    0xFFC00A40         /*                                       */
#define CAN0_MD2                    0xFFC00A44         /*                                   */
#define CAN0_TRS2                   0xFFC00A48         /*                                          */
#define CAN0_TRR2                   0xFFC00A4C         /*                                            */
#define CAN0_TA2                    0xFFC00A50         /*                                          */
#define CAN0_AA2                    0xFFC00A54         /*                                   */
#define CAN0_RMP2                   0xFFC00A58         /*                                         */
#define CAN0_RML2                   0xFFC00A5C         /*                                      */
#define CAN0_MBTIF2                 0xFFC00A60         /*                                                 */
#define CAN0_MBRIF2                 0xFFC00A64         /*                                                */
#define CAN0_MBIM2                  0xFFC00A68         /*                                        */
#define CAN0_RFH2                   0xFFC00A6C         /*                                       */
#define CAN0_OPSS2                  0xFFC00A70         /*                                                               */
#define CAN0_CLOCK                    0xFFC00A80         /*                     */
#define CAN0_TIMING                 0xFFC00A84         /*                      */
#define CAN0_DEBUG                    0xFFC00A88         /*                     */
#define CAN0_STATUS                   0xFFC00A8C         /*                      */
#define CAN0_CEC                    0xFFC00A90         /*                             */
#define CAN0_GIS                    0xFFC00A94         /*                                  */
#define CAN0_GIM                    0xFFC00A98         /*                                */
#define CAN0_GIF                    0xFFC00A9C         /*                                */
#define CAN0_CONTROL                    0xFFC00AA0         /*                                  */
#define CAN0_INTR                    0xFFC00AA4         /*                                 */
#define CAN0_MBTD                   0xFFC00AAC         /*                                         */
#define CAN0_EWR                    0xFFC00AB0         /*                                           */
#define CAN0_ESR                    0xFFC00AB4         /*                            */
#define CAN0_UCCNT                  0xFFC00AC4         /*                                 */
#define CAN0_UCRC                   0xFFC00AC8         /*                                                */
#define CAN0_UCCNF                  0xFFC00ACC         /*                                                    */
#define CAN0_AM00L                  0xFFC00B00         /*                                   */
#define CAN0_AM01L                  0xFFC00B08         /*                                   */
#define CAN0_AM02L                  0xFFC00B10         /*                                   */
#define CAN0_AM03L                  0xFFC00B18         /*                                   */
#define CAN0_AM04L                  0xFFC00B20         /*                                   */
#define CAN0_AM05L                  0xFFC00B28         /*                                   */
#define CAN0_AM06L                  0xFFC00B30         /*                                   */
#define CAN0_AM07L                  0xFFC00B38         /*                                   */
#define CAN0_AM08L                  0xFFC00B40         /*                                   */
#define CAN0_AM09L                  0xFFC00B48         /*                                   */
#define CAN0_AM10L                  0xFFC00B50         /*                                   */
#define CAN0_AM11L                  0xFFC00B58         /*                                   */
#define CAN0_AM12L                  0xFFC00B60         /*                                   */
#define CAN0_AM13L                  0xFFC00B68         /*                                   */
#define CAN0_AM14L                  0xFFC00B70         /*                                   */
#define CAN0_AM15L                  0xFFC00B78         /*                                   */
#define CAN0_AM16L                  0xFFC00B80         /*                                   */
#define CAN0_AM17L                  0xFFC00B88         /*                                   */
#define CAN0_AM18L                  0xFFC00B90         /*                                   */
#define CAN0_AM19L                  0xFFC00B98         /*                                   */
#define CAN0_AM20L                  0xFFC00BA0         /*                                   */
#define CAN0_AM21L                  0xFFC00BA8         /*                                   */
#define CAN0_AM22L                  0xFFC00BB0         /*                                   */
#define CAN0_AM23L                  0xFFC00BB8         /*                                   */
#define CAN0_AM24L                  0xFFC00BC0         /*                                   */
#define CAN0_AM25L                  0xFFC00BC8         /*                                   */
#define CAN0_AM26L                  0xFFC00BD0         /*                                   */
#define CAN0_AM27L                  0xFFC00BD8         /*                                   */
#define CAN0_AM28L                  0xFFC00BE0         /*                                   */
#define CAN0_AM29L                  0xFFC00BE8         /*                                   */
#define CAN0_AM30L                  0xFFC00BF0         /*                                   */
#define CAN0_AM31L                  0xFFC00BF8         /*                                   */
#define CAN0_AM00H                  0xFFC00B04         /*                                   */
#define CAN0_AM01H                  0xFFC00B0C         /*                                   */
#define CAN0_AM02H                  0xFFC00B14         /*                                   */
#define CAN0_AM03H                  0xFFC00B1C         /*                                   */
#define CAN0_AM04H                  0xFFC00B24         /*                                   */
#define CAN0_AM05H                  0xFFC00B2C         /*                                   */
#define CAN0_AM06H                  0xFFC00B34         /*                                   */
#define CAN0_AM07H                  0xFFC00B3C         /*                                   */
#define CAN0_AM08H                  0xFFC00B44         /*                                   */
#define CAN0_AM09H                  0xFFC00B4C         /*                                   */
#define CAN0_AM10H                  0xFFC00B54         /*                                   */
#define CAN0_AM11H                  0xFFC00B5C         /*                                   */
#define CAN0_AM12H                  0xFFC00B64         /*                                   */
#define CAN0_AM13H                  0xFFC00B6C         /*                                   */
#define CAN0_AM14H                  0xFFC00B74         /*                                   */
#define CAN0_AM15H                  0xFFC00B7C         /*                                   */
#define CAN0_AM16H                  0xFFC00B84         /*                                   */
#define CAN0_AM17H                  0xFFC00B8C         /*                                   */
#define CAN0_AM18H                  0xFFC00B94         /*                                   */
#define CAN0_AM19H                  0xFFC00B9C         /*                                   */
#define CAN0_AM20H                  0xFFC00BA4         /*                                   */
#define CAN0_AM21H                  0xFFC00BAC         /*                                   */
#define CAN0_AM22H                  0xFFC00BB4         /*                                   */
#define CAN0_AM23H                  0xFFC00BBC         /*                                   */
#define CAN0_AM24H                  0xFFC00BC4         /*                                   */
#define CAN0_AM25H                  0xFFC00BCC         /*                                   */
#define CAN0_AM26H                  0xFFC00BD4         /*                                   */
#define CAN0_AM27H                  0xFFC00BDC         /*                                   */
#define CAN0_AM28H                  0xFFC00BE4         /*                                   */
#define CAN0_AM29H                  0xFFC00BEC         /*                                   */
#define CAN0_AM30H                  0xFFC00BF4         /*                                   */
#define CAN0_AM31H                  0xFFC00BFC         /*                                   */
#define CAN0_MB00_DATA0             0xFFC00C00         /*                              */
#define CAN0_MB01_DATA0             0xFFC00C20         /*                              */
#define CAN0_MB02_DATA0             0xFFC00C40         /*                              */
#define CAN0_MB03_DATA0             0xFFC00C60         /*                              */
#define CAN0_MB04_DATA0             0xFFC00C80         /*                              */
#define CAN0_MB05_DATA0             0xFFC00CA0         /*                              */
#define CAN0_MB06_DATA0             0xFFC00CC0         /*                              */
#define CAN0_MB07_DATA0             0xFFC00CE0         /*                              */
#define CAN0_MB08_DATA0             0xFFC00D00         /*                              */
#define CAN0_MB09_DATA0             0xFFC00D20         /*                              */
#define CAN0_MB10_DATA0             0xFFC00D40         /*                              */
#define CAN0_MB11_DATA0             0xFFC00D60         /*                              */
#define CAN0_MB12_DATA0             0xFFC00D80         /*                              */
#define CAN0_MB13_DATA0             0xFFC00DA0         /*                              */
#define CAN0_MB14_DATA0             0xFFC00DC0         /*                              */
#define CAN0_MB15_DATA0             0xFFC00DE0         /*                              */
#define CAN0_MB16_DATA0             0xFFC00E00         /*                              */
#define CAN0_MB17_DATA0             0xFFC00E20         /*                              */
#define CAN0_MB18_DATA0             0xFFC00E40         /*                              */
#define CAN0_MB19_DATA0             0xFFC00E60         /*                              */
#define CAN0_MB20_DATA0             0xFFC00E80         /*                              */
#define CAN0_MB21_DATA0             0xFFC00EA0         /*                              */
#define CAN0_MB22_DATA0             0xFFC00EC0         /*                              */
#define CAN0_MB23_DATA0             0xFFC00EE0         /*                              */
#define CAN0_MB24_DATA0             0xFFC00F00         /*                              */
#define CAN0_MB25_DATA0             0xFFC00F20         /*                              */
#define CAN0_MB26_DATA0             0xFFC00F40         /*                              */
#define CAN0_MB27_DATA0             0xFFC00F60         /*                              */
#define CAN0_MB28_DATA0             0xFFC00F80         /*                              */
#define CAN0_MB29_DATA0             0xFFC00FA0         /*                              */
#define CAN0_MB30_DATA0             0xFFC00FC0         /*                              */
#define CAN0_MB31_DATA0             0xFFC00FE0         /*                              */
#define CAN0_MB00_DATA1             0xFFC00C04         /*                              */
#define CAN0_MB01_DATA1             0xFFC00C24         /*                              */
#define CAN0_MB02_DATA1             0xFFC00C44         /*                              */
#define CAN0_MB03_DATA1             0xFFC00C64         /*                              */
#define CAN0_MB04_DATA1             0xFFC00C84         /*                              */
#define CAN0_MB05_DATA1             0xFFC00CA4         /*                              */
#define CAN0_MB06_DATA1             0xFFC00CC4         /*                              */
#define CAN0_MB07_DATA1             0xFFC00CE4         /*                              */
#define CAN0_MB08_DATA1             0xFFC00D04         /*                              */
#define CAN0_MB09_DATA1             0xFFC00D24         /*                              */
#define CAN0_MB10_DATA1             0xFFC00D44         /*                              */
#define CAN0_MB11_DATA1             0xFFC00D64         /*                              */
#define CAN0_MB12_DATA1             0xFFC00D84         /*                              */
#define CAN0_MB13_DATA1             0xFFC00DA4         /*                              */
#define CAN0_MB14_DATA1             0xFFC00DC4         /*                              */
#define CAN0_MB15_DATA1             0xFFC00DE4         /*                              */
#define CAN0_MB16_DATA1             0xFFC00E04         /*                              */
#define CAN0_MB17_DATA1             0xFFC00E24         /*                              */
#define CAN0_MB18_DATA1             0xFFC00E44         /*                              */
#define CAN0_MB19_DATA1             0xFFC00E64         /*                              */
#define CAN0_MB20_DATA1             0xFFC00E84         /*                              */
#define CAN0_MB21_DATA1             0xFFC00EA4         /*                              */
#define CAN0_MB22_DATA1             0xFFC00EC4         /*                              */
#define CAN0_MB23_DATA1             0xFFC00EE4         /*                              */
#define CAN0_MB24_DATA1             0xFFC00F04         /*                              */
#define CAN0_MB25_DATA1             0xFFC00F24         /*                              */
#define CAN0_MB26_DATA1             0xFFC00F44         /*                              */
#define CAN0_MB27_DATA1             0xFFC00F64         /*                              */
#define CAN0_MB28_DATA1             0xFFC00F84         /*                              */
#define CAN0_MB29_DATA1             0xFFC00FA4         /*                              */
#define CAN0_MB30_DATA1             0xFFC00FC4         /*                              */
#define CAN0_MB31_DATA1             0xFFC00FE4         /*                              */
#define CAN0_MB00_DATA2             0xFFC00C08         /*                              */
#define CAN0_MB01_DATA2             0xFFC00C28         /*                              */
#define CAN0_MB02_DATA2             0xFFC00C48         /*                              */
#define CAN0_MB03_DATA2             0xFFC00C68         /*                              */
#define CAN0_MB04_DATA2             0xFFC00C88         /*                              */
#define CAN0_MB05_DATA2             0xFFC00CA8         /*                              */
#define CAN0_MB06_DATA2             0xFFC00CC8         /*                              */
#define CAN0_MB07_DATA2             0xFFC00CE8         /*                              */
#define CAN0_MB08_DATA2             0xFFC00D08         /*                              */
#define CAN0_MB09_DATA2             0xFFC00D28         /*                              */
#define CAN0_MB10_DATA2             0xFFC00D48         /*                              */
#define CAN0_MB11_DATA2             0xFFC00D68         /*                              */
#define CAN0_MB12_DATA2             0xFFC00D88         /*                              */
#define CAN0_MB13_DATA2             0xFFC00DA8         /*                              */
#define CAN0_MB14_DATA2             0xFFC00DC8         /*                              */
#define CAN0_MB15_DATA2             0xFFC00DE8         /*                              */
#define CAN0_MB16_DATA2             0xFFC00E08         /*                              */
#define CAN0_MB17_DATA2             0xFFC00E28         /*                              */
#define CAN0_MB18_DATA2             0xFFC00E48         /*                              */
#define CAN0_MB19_DATA2             0xFFC00E68         /*                              */
#define CAN0_MB20_DATA2             0xFFC00E88         /*                              */
#define CAN0_MB21_DATA2             0xFFC00EA8         /*                              */
#define CAN0_MB22_DATA2             0xFFC00EC8         /*                              */
#define CAN0_MB23_DATA2             0xFFC00EE8         /*                              */
#define CAN0_MB24_DATA2             0xFFC00F08         /*                              */
#define CAN0_MB25_DATA2             0xFFC00F28         /*                              */
#define CAN0_MB26_DATA2             0xFFC00F48         /*                              */
#define CAN0_MB27_DATA2             0xFFC00F68         /*                              */
#define CAN0_MB28_DATA2             0xFFC00F88         /*                              */
#define CAN0_MB29_DATA2             0xFFC00FA8         /*                              */
#define CAN0_MB30_DATA2             0xFFC00FC8         /*                              */
#define CAN0_MB31_DATA2             0xFFC00FE8         /*                              */
#define CAN0_MB00_DATA3             0xFFC00C0C         /*                              */
#define CAN0_MB01_DATA3             0xFFC00C2C         /*                              */
#define CAN0_MB02_DATA3             0xFFC00C4C         /*                              */
#define CAN0_MB03_DATA3             0xFFC00C6C         /*                              */
#define CAN0_MB04_DATA3             0xFFC00C8C         /*                              */
#define CAN0_MB05_DATA3             0xFFC00CAC         /*                              */
#define CAN0_MB06_DATA3             0xFFC00CCC         /*                              */
#define CAN0_MB07_DATA3             0xFFC00CEC         /*                              */
#define CAN0_MB08_DATA3             0xFFC00D0C         /*                              */
#define CAN0_MB09_DATA3             0xFFC00D2C         /*                              */
#define CAN0_MB10_DATA3             0xFFC00D4C         /*                              */
#define CAN0_MB11_DATA3             0xFFC00D6C         /*                              */
#define CAN0_MB12_DATA3             0xFFC00D8C         /*                              */
#define CAN0_MB13_DATA3             0xFFC00DAC         /*                              */
#define CAN0_MB14_DATA3             0xFFC00DCC         /*                              */
#define CAN0_MB15_DATA3             0xFFC00DEC         /*                              */
#define CAN0_MB16_DATA3             0xFFC00E0C         /*                              */
#define CAN0_MB17_DATA3             0xFFC00E2C         /*                              */
#define CAN0_MB18_DATA3             0xFFC00E4C         /*                              */
#define CAN0_MB19_DATA3             0xFFC00E6C         /*                              */
#define CAN0_MB20_DATA3             0xFFC00E8C         /*                              */
#define CAN0_MB21_DATA3             0xFFC00EAC         /*                              */
#define CAN0_MB22_DATA3             0xFFC00ECC         /*                              */
#define CAN0_MB23_DATA3             0xFFC00EEC         /*                              */
#define CAN0_MB24_DATA3             0xFFC00F0C         /*                              */
#define CAN0_MB25_DATA3             0xFFC00F2C         /*                              */
#define CAN0_MB26_DATA3             0xFFC00F4C         /*                              */
#define CAN0_MB27_DATA3             0xFFC00F6C         /*                              */
#define CAN0_MB28_DATA3             0xFFC00F8C         /*                              */
#define CAN0_MB29_DATA3             0xFFC00FAC         /*                              */
#define CAN0_MB30_DATA3             0xFFC00FCC         /*                              */
#define CAN0_MB31_DATA3             0xFFC00FEC         /*                              */
#define CAN0_MB00_LENGTH            0xFFC00C10         /*                              */
#define CAN0_MB01_LENGTH            0xFFC00C30         /*                              */
#define CAN0_MB02_LENGTH            0xFFC00C50         /*                              */
#define CAN0_MB03_LENGTH            0xFFC00C70         /*                              */
#define CAN0_MB04_LENGTH            0xFFC00C90         /*                              */
#define CAN0_MB05_LENGTH            0xFFC00CB0         /*                              */
#define CAN0_MB06_LENGTH            0xFFC00CD0         /*                              */
#define CAN0_MB07_LENGTH            0xFFC00CF0         /*                              */
#define CAN0_MB08_LENGTH            0xFFC00D10         /*                              */
#define CAN0_MB09_LENGTH            0xFFC00D30         /*                              */
#define CAN0_MB10_LENGTH            0xFFC00D50         /*                              */
#define CAN0_MB11_LENGTH            0xFFC00D70         /*                              */
#define CAN0_MB12_LENGTH            0xFFC00D90         /*                              */
#define CAN0_MB13_LENGTH            0xFFC00DB0         /*                              */
#define CAN0_MB14_LENGTH            0xFFC00DD0         /*                              */
#define CAN0_MB15_LENGTH            0xFFC00DF0         /*                              */
#define CAN0_MB16_LENGTH            0xFFC00E10         /*                              */
#define CAN0_MB17_LENGTH            0xFFC00E30         /*                              */
#define CAN0_MB18_LENGTH            0xFFC00E50         /*                              */
#define CAN0_MB19_LENGTH            0xFFC00E70         /*                              */
#define CAN0_MB20_LENGTH            0xFFC00E90         /*                              */
#define CAN0_MB21_LENGTH            0xFFC00EB0         /*                              */
#define CAN0_MB22_LENGTH            0xFFC00ED0         /*                              */
#define CAN0_MB23_LENGTH            0xFFC00EF0         /*                              */
#define CAN0_MB24_LENGTH            0xFFC00F10         /*                              */
#define CAN0_MB25_LENGTH            0xFFC00F30         /*                              */
#define CAN0_MB26_LENGTH            0xFFC00F50         /*                              */
#define CAN0_MB27_LENGTH            0xFFC00F70         /*                              */
#define CAN0_MB28_LENGTH            0xFFC00F90         /*                              */
#define CAN0_MB29_LENGTH            0xFFC00FB0         /*                              */
#define CAN0_MB30_LENGTH            0xFFC00FD0         /*                              */
#define CAN0_MB31_LENGTH            0xFFC00FF0         /*                              */
#define CAN0_MB00_TIMESTAMP         0xFFC00C14         /*                              */
#define CAN0_MB01_TIMESTAMP         0xFFC00C34         /*                              */
#define CAN0_MB02_TIMESTAMP         0xFFC00C54         /*                              */
#define CAN0_MB03_TIMESTAMP         0xFFC00C74         /*                              */
#define CAN0_MB04_TIMESTAMP         0xFFC00C94         /*                              */
#define CAN0_MB05_TIMESTAMP         0xFFC00CB4         /*                              */
#define CAN0_MB06_TIMESTAMP         0xFFC00CD4         /*                              */
#define CAN0_MB07_TIMESTAMP         0xFFC00CF4         /*                              */
#define CAN0_MB08_TIMESTAMP         0xFFC00D14         /*                              */
#define CAN0_MB09_TIMESTAMP         0xFFC00D34         /*                              */
#define CAN0_MB10_TIMESTAMP         0xFFC00D54         /*                              */
#define CAN0_MB11_TIMESTAMP         0xFFC00D74         /*                              */
#define CAN0_MB12_TIMESTAMP         0xFFC00D94         /*                              */
#define CAN0_MB13_TIMESTAMP         0xFFC00DB4         /*                              */
#define CAN0_MB14_TIMESTAMP         0xFFC00DD4         /*                              */
#define CAN0_MB15_TIMESTAMP         0xFFC00DF4         /*                              */
#define CAN0_MB16_TIMESTAMP         0xFFC00E14         /*                              */
#define CAN0_MB17_TIMESTAMP         0xFFC00E34         /*                              */
#define CAN0_MB18_TIMESTAMP         0xFFC00E54         /*                              */
#define CAN0_MB19_TIMESTAMP         0xFFC00E74         /*                              */
#define CAN0_MB20_TIMESTAMP         0xFFC00E94         /*                              */
#define CAN0_MB21_TIMESTAMP         0xFFC00EB4         /*                              */
#define CAN0_MB22_TIMESTAMP         0xFFC00ED4         /*                              */
#define CAN0_MB23_TIMESTAMP         0xFFC00EF4         /*                              */
#define CAN0_MB24_TIMESTAMP         0xFFC00F14         /*                              */
#define CAN0_MB25_TIMESTAMP         0xFFC00F34         /*                              */
#define CAN0_MB26_TIMESTAMP         0xFFC00F54         /*                              */
#define CAN0_MB27_TIMESTAMP         0xFFC00F74         /*                              */
#define CAN0_MB28_TIMESTAMP         0xFFC00F94         /*                              */
#define CAN0_MB29_TIMESTAMP         0xFFC00FB4         /*                              */
#define CAN0_MB30_TIMESTAMP         0xFFC00FD4         /*                              */
#define CAN0_MB31_TIMESTAMP         0xFFC00FF4         /*                              */
#define CAN0_MB00_ID0               0xFFC00C18         /*                              */
#define CAN0_MB01_ID0               0xFFC00C38         /*                              */
#define CAN0_MB02_ID0               0xFFC00C58         /*                              */
#define CAN0_MB03_ID0               0xFFC00C78         /*                              */
#define CAN0_MB04_ID0               0xFFC00C98         /*                              */
#define CAN0_MB05_ID0               0xFFC00CB8         /*                              */
#define CAN0_MB06_ID0               0xFFC00CD8         /*                              */
#define CAN0_MB07_ID0               0xFFC00CF8         /*                              */
#define CAN0_MB08_ID0               0xFFC00D18         /*                              */
#define CAN0_MB09_ID0               0xFFC00D38         /*                              */
#define CAN0_MB10_ID0               0xFFC00D58         /*                              */
#define CAN0_MB11_ID0               0xFFC00D78         /*                              */
#define CAN0_MB12_ID0               0xFFC00D98         /*                              */
#define CAN0_MB13_ID0               0xFFC00DB8         /*                              */
#define CAN0_MB14_ID0               0xFFC00DD8         /*                              */
#define CAN0_MB15_ID0               0xFFC00DF8         /*                              */
#define CAN0_MB16_ID0               0xFFC00E18         /*                              */
#define CAN0_MB17_ID0               0xFFC00E38         /*                              */
#define CAN0_MB18_ID0               0xFFC00E58         /*                              */
#define CAN0_MB19_ID0               0xFFC00E78         /*                              */
#define CAN0_MB20_ID0               0xFFC00E98         /*                              */
#define CAN0_MB21_ID0               0xFFC00EB8         /*                              */
#define CAN0_MB22_ID0               0xFFC00ED8         /*                              */
#define CAN0_MB23_ID0               0xFFC00EF8         /*                              */
#define CAN0_MB24_ID0               0xFFC00F18         /*                              */
#define CAN0_MB25_ID0               0xFFC00F38         /*                              */
#define CAN0_MB26_ID0               0xFFC00F58         /*                              */
#define CAN0_MB27_ID0               0xFFC00F78         /*                              */
#define CAN0_MB28_ID0               0xFFC00F98         /*                              */
#define CAN0_MB29_ID0               0xFFC00FB8         /*                              */
#define CAN0_MB30_ID0               0xFFC00FD8         /*                              */
#define CAN0_MB31_ID0               0xFFC00FF8         /*                              */
#define CAN0_MB00_ID1               0xFFC00C1C         /*                              */
#define CAN0_MB01_ID1               0xFFC00C3C         /*                              */
#define CAN0_MB02_ID1               0xFFC00C5C         /*                              */
#define CAN0_MB03_ID1               0xFFC00C7C         /*                              */
#define CAN0_MB04_ID1               0xFFC00C9C         /*                              */
#define CAN0_MB05_ID1               0xFFC00CBC         /*                              */
#define CAN0_MB06_ID1               0xFFC00CDC         /*                              */
#define CAN0_MB07_ID1               0xFFC00CFC         /*                              */
#define CAN0_MB08_ID1               0xFFC00D1C         /*                              */
#define CAN0_MB09_ID1               0xFFC00D3C         /*                              */
#define CAN0_MB10_ID1               0xFFC00D5C         /*                              */
#define CAN0_MB11_ID1               0xFFC00D7C         /*                              */
#define CAN0_MB12_ID1               0xFFC00D9C         /*                              */
#define CAN0_MB13_ID1               0xFFC00DBC         /*                              */
#define CAN0_MB14_ID1               0xFFC00DDC         /*                              */
#define CAN0_MB15_ID1               0xFFC00DFC         /*                              */
#define CAN0_MB16_ID1               0xFFC00E1C         /*                              */
#define CAN0_MB17_ID1               0xFFC00E3C         /*                              */
#define CAN0_MB18_ID1               0xFFC00E5C         /*                              */
#define CAN0_MB19_ID1               0xFFC00E7C         /*                              */
#define CAN0_MB20_ID1               0xFFC00E9C         /*                              */
#define CAN0_MB21_ID1               0xFFC00EBC         /*                              */
#define CAN0_MB22_ID1               0xFFC00EDC         /*                              */
#define CAN0_MB23_ID1               0xFFC00EFC         /*                              */
#define CAN0_MB24_ID1               0xFFC00F1C         /*                              */
#define CAN0_MB25_ID1               0xFFC00F3C         /*                              */
#define CAN0_MB26_ID1               0xFFC00F5C         /*                              */
#define CAN0_MB27_ID1               0xFFC00F7C         /*                              */
#define CAN0_MB28_ID1               0xFFC00F9C         /*                              */
#define CAN0_MB29_ID1               0xFFC00FBC         /*                              */
#define CAN0_MB30_ID1               0xFFC00FDC         /*                              */
#define CAN0_MB31_ID1               0xFFC00FFC         /*                              */

/*                          
                    
                             */
#define LP0_CTL                     0xFFC01000         /*                      */
#define LP0_STAT                    0xFFC01004         /*                     */
#define LP0_DIV                     0xFFC01008         /*                         */
#define LP0_CNT                     0xFFC0100C         /*                                          */
#define LP0_TX                      0xFFC01010         /*                     */
#define LP0_RX                      0xFFC01014         /*                    */
#define LP0_TXIN_SHDW               0xFFC01018         /*                                  */
#define LP0_TXOUT_SHDW              0xFFC0101C         /*                                   */
#define LP1_CTL                     0xFFC01100         /*                      */
#define LP1_STAT                    0xFFC01104         /*                     */
#define LP1_DIV                     0xFFC01108         /*                         */
#define LP1_CNT                     0xFFC0110C         /*                                          */
#define LP1_TX                      0xFFC01110         /*                     */
#define LP1_RX                      0xFFC01114         /*                    */
#define LP1_TXIN_SHDW               0xFFC01118         /*                                  */
#define LP1_TXOUT_SHDW              0xFFC0111C         /*                                   */
#define LP2_CTL                     0xFFC01200         /*                      */
#define LP2_STAT                    0xFFC01204         /*                     */
#define LP2_DIV                     0xFFC01208         /*                         */
#define LP2_CNT                     0xFFC0120C         /*                                          */
#define LP2_TX                      0xFFC01210         /*                     */
#define LP2_RX                      0xFFC01214         /*                    */
#define LP2_TXIN_SHDW               0xFFC01218         /*                                  */
#define LP2_TXOUT_SHDW              0xFFC0121C         /*                                   */
#define LP3_CTL                     0xFFC01300         /*                      */
#define LP3_STAT                    0xFFC01304         /*                     */
#define LP3_DIV                     0xFFC01308         /*                         */
#define LP3_CNT                     0xFFC0130C         /*                                          */
#define LP3_TX                      0xFFC01310         /*                     */
#define LP3_RX                      0xFFC01314         /*                    */
#define LP3_TXIN_SHDW               0xFFC01318         /*                                  */
#define LP3_TXOUT_SHDW              0xFFC0131C         /*                                   */

/*                          
                       
                             */
#define TIMER_REVID                0xFFC01400         /*                             */
#define TIMER_RUN                  0xFFC01404         /*                            */
#define TIMER_RUN_SET              0xFFC01408         /*                                   */
#define TIMER_RUN_CLR              0xFFC0140C         /*                                     */
#define TIMER_STOP_CFG             0xFFC01410         /*                              */
#define TIMER_STOP_CFG_SET         0xFFC01414         /*                                  */
#define TIMER_STOP_CFG_CLR         0xFFC01418         /*                                    */
#define TIMER_DATA_IMSK            0xFFC0141C         /*                                      */
#define TIMER_STAT_IMSK            0xFFC01420         /*                                        */
#define TIMER_TRG_MSK              0xFFC01424         /*                                      */
#define TIMER_TRG_IE               0xFFC01428         /*                                       */
#define TIMER_DATA_ILAT            0xFFC0142C         /*                                 */
#define TIMER_STAT_ILAT            0xFFC01430         /*                                           */
#define TIMER_ERR_TYPE             0xFFC01434         /*                                           */
#define TIMER_BCAST_PER            0xFFC01438         /*                          */
#define TIMER_BCAST_WID            0xFFC0143C         /*                         */
#define TIMER_BCAST_DLY            0xFFC01440         /*                         */

/*                          
         
                             */
#define TIMER0_CONFIG             0xFFC01460         /*                                  */
#define TIMER0_COUNTER            0xFFC01464         /*                                   */
#define TIMER0_PERIOD             0xFFC01468         /*                                  */
#define TIMER0_WIDTH              0xFFC0146C         /*                                 */
#define TIMER0_DELAY              0xFFC01470         /*                                 */

#define TIMER1_CONFIG             0xFFC01480         /*                                  */
#define TIMER1_COUNTER            0xFFC01484         /*                                   */
#define TIMER1_PERIOD             0xFFC01488         /*                                  */
#define TIMER1_WIDTH              0xFFC0148C         /*                                 */
#define TIMER1_DELAY              0xFFC01490         /*                                 */

#define TIMER2_CONFIG             0xFFC014A0         /*                                  */
#define TIMER2_COUNTER            0xFFC014A4         /*                                   */
#define TIMER2_PERIOD             0xFFC014A8         /*                                  */
#define TIMER2_WIDTH              0xFFC014AC         /*                                 */
#define TIMER2_DELAY              0xFFC014B0         /*                                 */

#define TIMER3_CONFIG             0xFFC014C0         /*                                  */
#define TIMER3_COUNTER            0xFFC014C4         /*                                   */
#define TIMER3_PERIOD             0xFFC014C8         /*                                  */
#define TIMER3_WIDTH              0xFFC014CC         /*                                 */
#define TIMER3_DELAY              0xFFC014D0         /*                                 */

#define TIMER4_CONFIG             0xFFC014E0         /*                                  */
#define TIMER4_COUNTER            0xFFC014E4         /*                                   */
#define TIMER4_PERIOD             0xFFC014E8         /*                                  */
#define TIMER4_WIDTH              0xFFC014EC         /*                                 */
#define TIMER4_DELAY              0xFFC014F0         /*                                 */

#define TIMER5_CONFIG             0xFFC01500         /*                                  */
#define TIMER5_COUNTER            0xFFC01504         /*                                   */
#define TIMER5_PERIOD             0xFFC01508         /*                                  */
#define TIMER5_WIDTH              0xFFC0150C         /*                                 */
#define TIMER5_DELAY              0xFFC01510         /*                                 */

#define TIMER6_CONFIG             0xFFC01520         /*                                  */
#define TIMER6_COUNTER            0xFFC01524         /*                                   */
#define TIMER6_PERIOD             0xFFC01528         /*                                  */
#define TIMER6_WIDTH              0xFFC0152C         /*                                 */
#define TIMER6_DELAY              0xFFC01530         /*                                 */

#define TIMER7_CONFIG             0xFFC01540         /*                                  */
#define TIMER7_COUNTER            0xFFC01544         /*                                   */
#define TIMER7_PERIOD             0xFFC01548         /*                                  */
#define TIMER7_WIDTH              0xFFC0154C         /*                                 */
#define TIMER7_DELAY              0xFFC01550         /*                                 */

/*                          
              
                             */

/*                          
     
                             */
#define REG_CRC0_CTL                    0xFFC01C00         /*                       */
#define REG_CRC0_DCNT                   0xFFC01C04         /*                               */
#define REG_CRC0_DCNTRLD                0xFFC01C08         /*                                      */
#define REG_CRC0_COMP                   0xFFC01C14         /*                            */
#define REG_CRC0_FILLVAL                0xFFC01C18         /*                          */
#define REG_CRC0_DFIFO                  0xFFC01C1C         /*                         */
#define REG_CRC0_INEN                   0xFFC01C20         /*                                */
#define REG_CRC0_INEN_SET               0xFFC01C24         /*                                    */
#define REG_CRC0_INEN_CLR               0xFFC01C28         /*                                      */
#define REG_CRC0_POLY                   0xFFC01C2C         /*                          */
#define REG_CRC0_STAT                   0xFFC01C40         /*                      */
#define REG_CRC0_DCNTCAP                0xFFC01C44         /*                                  */
#define REG_CRC0_RESULT_FIN             0xFFC01C4C         /*                                */
#define REG_CRC0_RESULT_CUR             0xFFC01C50         /*                                  */
#define REG_CRC0_REVID                  0xFFC01C60         /*                           */

/*                          
     
                             */
#define REG_CRC1_CTL                    0xFFC01D00         /*                       */
#define REG_CRC1_DCNT                   0xFFC01D04         /*                               */
#define REG_CRC1_DCNTRLD                0xFFC01D08         /*                                      */
#define REG_CRC1_COMP                   0xFFC01D14         /*                            */
#define REG_CRC1_FILLVAL                0xFFC01D18         /*                          */
#define REG_CRC1_DFIFO                  0xFFC01D1C         /*                         */
#define REG_CRC1_INEN                   0xFFC01D20         /*                                */
#define REG_CRC1_INEN_SET               0xFFC01D24         /*                                    */
#define REG_CRC1_INEN_CLR               0xFFC01D28         /*                                      */
#define REG_CRC1_POLY                   0xFFC01D2C         /*                          */
#define REG_CRC1_STAT                   0xFFC01D40         /*                      */
#define REG_CRC1_DCNTCAP                0xFFC01D44         /*                                  */
#define REG_CRC1_RESULT_FIN             0xFFC01D4C         /*                                */
#define REG_CRC1_RESULT_CUR             0xFFC01D50         /*                                  */
#define REG_CRC1_REVID                  0xFFC01D60         /*                           */

/*                          
                     
                             */

/*                          
            
                             */
#define TWI0_CLKDIV                    0xFFC01E00         /*                        */
#define TWI0_CONTROL                   0xFFC01E04         /*                       */
#define TWI0_SLAVE_CTL                 0xFFC01E08         /*                                  */
#define TWI0_SLAVE_STAT                0xFFC01E0C         /*                                 */
#define TWI0_SLAVE_ADDR                0xFFC01E10         /*                                  */
#define TWI0_MASTER_CTL                0xFFC01E14         /*                                    */
#define TWI0_MASTER_STAT               0xFFC01E18         /*                                  */
#define TWI0_MASTER_ADDR               0xFFC01E1C         /*                                   */
#define TWI0_INT_STAT                  0xFFC01E20         /*                                */
#define TWI0_INT_MASK                  0xFFC01E24         /*                              */
#define TWI0_FIFO_CTL                  0xFFC01E28         /*                            */
#define TWI0_FIFO_STAT                 0xFFC01E2C         /*                           */
#define TWI0_XMT_DATA8                 0xFFC01E80         /*                                              */
#define TWI0_XMT_DATA16                0xFFC01E84         /*                                              */
#define TWI0_RCV_DATA8                 0xFFC01E88         /*                                              */
#define TWI0_RCV_DATA16                0xFFC01E8C         /*                                              */

/*                          
            
                             */
#define TWI1_CLKDIV                 0xFFC01F00         /*                        */
#define TWI1_CONTROL                    0xFFC01F04         /*                       */
#define TWI1_SLAVE_CTL                 0xFFC01F08         /*                                  */
#define TWI1_SLAVE_STAT                0xFFC01F0C         /*                                 */
#define TWI1_SLAVE_ADDR                0xFFC01F10         /*                                  */
#define TWI1_MASTER_CTL                0xFFC01F14         /*                                    */
#define TWI1_MASTER_STAT               0xFFC01F18         /*                                  */
#define TWI1_MASTER_ADDR               0xFFC01F1C         /*                                   */
#define TWI1_INT_STAT                  0xFFC01F20         /*                                */
#define TWI1_INT_MASK                   0xFFC01F24         /*                              */
#define TWI1_FIFO_CTL                0xFFC01F28         /*                            */
#define TWI1_FIFO_STAT               0xFFC01F2C         /*                           */
#define TWI1_XMT_DATA8                0xFFC01F80         /*                                              */
#define TWI1_XMT_DATA16               0xFFC01F84         /*                                              */
#define TWI1_RCV_DATA8                0xFFC01F88         /*                                              */
#define TWI1_RCV_DATA16               0xFFC01F8C         /*                                              */


/*                          
                      
                             */

/*                          
             
                             */
#define UART0_REVID                 0xFFC02000         /*                            */
#define UART0_CTL                   0xFFC02004         /*                        */
#define UART0_STAT                  0xFFC02008         /*                       */
#define UART0_SCR                   0xFFC0200C         /*                        */
#define UART0_CLK                   0xFFC02010         /*                           */
#define UART0_IER                   0xFFC02014         /*                               */
#define UART0_IER_SET               0xFFC02018         /*                                   */
#define UART0_IER_CLR               0xFFC0201C         /*                                     */
#define UART0_RBR                   0xFFC02020         /*                               */
#define UART0_THR                   0xFFC02024         /*                              */
#define UART0_TAIP                  0xFFC02028         /*                                              */
#define UART0_TSR                   0xFFC0202C         /*                               */
#define UART0_RSR                   0xFFC02030         /*                              */
#define UART0_TXDIV                 0xFFC02034         /*                                       */
#define UART0_RXDIV                 0xFFC02038         /*                                      */

/*                          
             
                             */
#define UART1_REVID                 0xFFC02400         /*                            */
#define UART1_CTL                   0xFFC02404         /*                        */
#define UART1_STAT                  0xFFC02408         /*                       */
#define UART1_SCR                   0xFFC0240C         /*                        */
#define UART1_CLK                   0xFFC02410         /*                           */
#define UART1_IER                   0xFFC02414         /*                               */
#define UART1_IER_SET               0xFFC02418         /*                                   */
#define UART1_IER_CLR               0xFFC0241C         /*                                     */
#define UART1_RBR                   0xFFC02420         /*                               */
#define UART1_THR                   0xFFC02424         /*                              */
#define UART1_TAIP                  0xFFC02428         /*                                              */
#define UART1_TSR                   0xFFC0242C         /*                               */
#define UART1_RSR                   0xFFC02430         /*                              */
#define UART1_TXDIV                 0xFFC02434         /*                                       */
#define UART1_RXDIV                 0xFFC02438         /*                                      */


/*                          
                      
                             */

/*                          
             
                             */
#define PORTA_FER                   0xFFC03000         /*                                       */
#define PORTA_FER_SET               0xFFC03004         /*                                           */
#define PORTA_FER_CLEAR               0xFFC03008         /*                                             */
#define PORTA_DATA                  0xFFC0300C         /*                                 */
#define PORTA_DATA_SET              0xFFC03010         /*                                     */
#define PORTA_DATA_CLEAR              0xFFC03014         /*                                       */
#define PORTA_DIR                   0xFFC03018         /*                                      */
#define PORTA_DIR_SET               0xFFC0301C         /*                                          */
#define PORTA_DIR_CLEAR               0xFFC03020         /*                                            */
#define PORTA_INEN                  0xFFC03024         /*                                         */
#define PORTA_INEN_SET              0xFFC03028         /*                                             */
#define PORTA_INEN_CLEAR              0xFFC0302C         /*                                               */
#define PORTA_MUX                   0xFFC03030         /*                                           */
#define PORTA_DATA_TGL              0xFFC03034         /*                                                */
#define PORTA_POL                   0xFFC03038         /*                                                  */
#define PORTA_POL_SET               0xFFC0303C         /*                                                      */
#define PORTA_POL_CLEAR               0xFFC03040         /*                                                        */
#define PORTA_LOCK                  0xFFC03044         /*                                 */
#define PORTA_REVID                 0xFFC0307C         /*                               */

/*                          
             
                             */
#define PORTB_FER                   0xFFC03080         /*                                       */
#define PORTB_FER_SET               0xFFC03084         /*                                           */
#define PORTB_FER_CLEAR               0xFFC03088         /*                                             */
#define PORTB_DATA                  0xFFC0308C         /*                                 */
#define PORTB_DATA_SET              0xFFC03090         /*                                     */
#define PORTB_DATA_CLEAR              0xFFC03094         /*                                       */
#define PORTB_DIR                   0xFFC03098         /*                                      */
#define PORTB_DIR_SET               0xFFC0309C         /*                                          */
#define PORTB_DIR_CLEAR               0xFFC030A0         /*                                            */
#define PORTB_INEN                  0xFFC030A4         /*                                         */
#define PORTB_INEN_SET              0xFFC030A8         /*                                             */
#define PORTB_INEN_CLEAR              0xFFC030AC         /*                                               */
#define PORTB_MUX                   0xFFC030B0         /*                                           */
#define PORTB_DATA_TGL              0xFFC030B4         /*                                                */
#define PORTB_POL                   0xFFC030B8         /*                                                  */
#define PORTB_POL_SET               0xFFC030BC         /*                                                      */
#define PORTB_POL_CLEAR               0xFFC030C0         /*                                                        */
#define PORTB_LOCK                  0xFFC030C4         /*                                 */
#define PORTB_REVID                 0xFFC030FC         /*                               */

/*                          
             
                             */
#define PORTC_FER                   0xFFC03100         /*                                       */
#define PORTC_FER_SET               0xFFC03104         /*                                           */
#define PORTC_FER_CLEAR               0xFFC03108         /*                                             */
#define PORTC_DATA                  0xFFC0310C         /*                                 */
#define PORTC_DATA_SET              0xFFC03110         /*                                     */
#define PORTC_DATA_CLEAR              0xFFC03114         /*                                       */
#define PORTC_DIR                   0xFFC03118         /*                                      */
#define PORTC_DIR_SET               0xFFC0311C         /*                                          */
#define PORTC_DIR_CLEAR               0xFFC03120         /*                                            */
#define PORTC_INEN                  0xFFC03124         /*                                         */
#define PORTC_INEN_SET              0xFFC03128         /*                                             */
#define PORTC_INEN_CLEAR              0xFFC0312C         /*                                               */
#define PORTC_MUX                   0xFFC03130         /*                                           */
#define PORTC_DATA_TGL              0xFFC03134         /*                                                */
#define PORTC_POL                   0xFFC03138         /*                                                  */
#define PORTC_POL_SET               0xFFC0313C         /*                                                      */
#define PORTC_POL_CLEAR               0xFFC03140         /*                                                        */
#define PORTC_LOCK                  0xFFC03144         /*                                 */
#define PORTC_REVID                 0xFFC0317C         /*                               */

/*                          
             
                             */
#define PORTD_FER                   0xFFC03180         /*                                       */
#define PORTD_FER_SET               0xFFC03184         /*                                           */
#define PORTD_FER_CLEAR               0xFFC03188         /*                                             */
#define PORTD_DATA                  0xFFC0318C         /*                                 */
#define PORTD_DATA_SET              0xFFC03190         /*                                     */
#define PORTD_DATA_CLEAR              0xFFC03194         /*                                       */
#define PORTD_DIR                   0xFFC03198         /*                                      */
#define PORTD_DIR_SET               0xFFC0319C         /*                                          */
#define PORTD_DIR_CLEAR               0xFFC031A0         /*                                            */
#define PORTD_INEN                  0xFFC031A4         /*                                         */
#define PORTD_INEN_SET              0xFFC031A8         /*                                             */
#define PORTD_INEN_CLEAR              0xFFC031AC         /*                                               */
#define PORTD_MUX                   0xFFC031B0         /*                                           */
#define PORTD_DATA_TGL              0xFFC031B4         /*                                                */
#define PORTD_POL                   0xFFC031B8         /*                                                  */
#define PORTD_POL_SET               0xFFC031BC         /*                                                      */
#define PORTD_POL_CLEAR               0xFFC031C0         /*                                                        */
#define PORTD_LOCK                  0xFFC031C4         /*                                 */
#define PORTD_REVID                 0xFFC031FC         /*                               */

/*                          
             
                             */
#define PORTE_FER                   0xFFC03200         /*                                       */
#define PORTE_FER_SET               0xFFC03204         /*                                           */
#define PORTE_FER_CLEAR               0xFFC03208         /*                                             */
#define PORTE_DATA                  0xFFC0320C         /*                                 */
#define PORTE_DATA_SET              0xFFC03210         /*                                     */
#define PORTE_DATA_CLEAR              0xFFC03214         /*                                       */
#define PORTE_DIR                   0xFFC03218         /*                                      */
#define PORTE_DIR_SET               0xFFC0321C         /*                                          */
#define PORTE_DIR_CLEAR               0xFFC03220         /*                                            */
#define PORTE_INEN                  0xFFC03224         /*                                         */
#define PORTE_INEN_SET              0xFFC03228         /*                                             */
#define PORTE_INEN_CLEAR              0xFFC0322C         /*                                               */
#define PORTE_MUX                   0xFFC03230         /*                                           */
#define PORTE_DATA_TGL              0xFFC03234         /*                                                */
#define PORTE_POL                   0xFFC03238         /*                                                  */
#define PORTE_POL_SET               0xFFC0323C         /*                                                      */
#define PORTE_POL_CLEAR               0xFFC03240         /*                                                        */
#define PORTE_LOCK                  0xFFC03244         /*                                 */
#define PORTE_REVID                 0xFFC0327C         /*                               */

/*                          
             
                             */
#define PORTF_FER                   0xFFC03280         /*                                       */
#define PORTF_FER_SET               0xFFC03284         /*                                           */
#define PORTF_FER_CLEAR               0xFFC03288         /*                                             */
#define PORTF_DATA                  0xFFC0328C         /*                                 */
#define PORTF_DATA_SET              0xFFC03290         /*                                     */
#define PORTF_DATA_CLEAR              0xFFC03294         /*                                       */
#define PORTF_DIR                   0xFFC03298         /*                                      */
#define PORTF_DIR_SET               0xFFC0329C         /*                                          */
#define PORTF_DIR_CLEAR               0xFFC032A0         /*                                            */
#define PORTF_INEN                  0xFFC032A4         /*                                         */
#define PORTF_INEN_SET              0xFFC032A8         /*                                             */
#define PORTF_INEN_CLEAR              0xFFC032AC         /*                                               */
#define PORTF_MUX                   0xFFC032B0         /*                                           */
#define PORTF_DATA_TGL              0xFFC032B4         /*                                                */
#define PORTF_POL                   0xFFC032B8         /*                                                  */
#define PORTF_POL_SET               0xFFC032BC         /*                                                      */
#define PORTF_POL_CLEAR               0xFFC032C0         /*                                                        */
#define PORTF_LOCK                  0xFFC032C4         /*                                 */
#define PORTF_REVID                 0xFFC032FC         /*                               */

/*                          
             
                             */
#define PORTG_FER                   0xFFC03300         /*                                       */
#define PORTG_FER_SET               0xFFC03304         /*                                           */
#define PORTG_FER_CLEAR               0xFFC03308         /*                                             */
#define PORTG_DATA                  0xFFC0330C         /*                                 */
#define PORTG_DATA_SET              0xFFC03310         /*                                     */
#define PORTG_DATA_CLEAR              0xFFC03314         /*                                       */
#define PORTG_DIR                   0xFFC03318         /*                                      */
#define PORTG_DIR_SET               0xFFC0331C         /*                                          */
#define PORTG_DIR_CLEAR               0xFFC03320         /*                                            */
#define PORTG_INEN                  0xFFC03324         /*                                         */
#define PORTG_INEN_SET              0xFFC03328         /*                                             */
#define PORTG_INEN_CLEAR              0xFFC0332C         /*                                               */
#define PORTG_MUX                   0xFFC03330         /*                                           */
#define PORTG_DATA_TGL              0xFFC03334         /*                                                */
#define PORTG_POL                   0xFFC03338         /*                                                  */
#define PORTG_POL_SET               0xFFC0333C         /*                                                      */
#define PORTG_POL_CLEAR               0xFFC03340         /*                                                        */
#define PORTG_LOCK                  0xFFC03344         /*                                 */
#define PORTG_REVID                 0xFFC0337C         /*                               */


/*                          
                      
                             */

/*                          
             
                             */
#define PINT0_MASK_SET              0xFFC04000         /*                              */
#define PINT0_MASK_CLEAR            0xFFC04004         /*                                */
#define PINT0_REQUEST               0xFFC04008         /*                             */
#define PINT0_ASSIGN                0xFFC0400C         /*                            */
#define PINT0_EDGE_SET              0xFFC04010         /*                              */
#define PINT0_EDGE_CLEAR            0xFFC04014         /*                                */
#define PINT0_INVERT_SET            0xFFC04018         /*                                */
#define PINT0_INVERT_CLEAR          0xFFC0401C         /*                                  */
#define PINT0_PINSTATE              0xFFC04020         /*                              */
#define PINT0_LATCH                 0xFFC04024         /*                           */

/*                          
             
                             */
#define PINT1_MASK_SET              0xFFC04100         /*                              */
#define PINT1_MASK_CLEAR            0xFFC04104         /*                                */
#define PINT1_REQUEST               0xFFC04108         /*                             */
#define PINT1_ASSIGN                0xFFC0410C         /*                            */
#define PINT1_EDGE_SET              0xFFC04110         /*                              */
#define PINT1_EDGE_CLEAR            0xFFC04114         /*                                */
#define PINT1_INVERT_SET            0xFFC04118         /*                                */
#define PINT1_INVERT_CLEAR          0xFFC0411C         /*                                  */
#define PINT1_PINSTATE              0xFFC04120         /*                              */
#define PINT1_LATCH                 0xFFC04124         /*                           */

/*                          
             
                             */
#define PINT2_MASK_SET              0xFFC04200         /*                              */
#define PINT2_MASK_CLEAR            0xFFC04204         /*                                */
#define PINT2_REQUEST               0xFFC04208         /*                             */
#define PINT2_ASSIGN                0xFFC0420C         /*                            */
#define PINT2_EDGE_SET              0xFFC04210         /*                              */
#define PINT2_EDGE_CLEAR            0xFFC04214         /*                                */
#define PINT2_INVERT_SET            0xFFC04218         /*                                */
#define PINT2_INVERT_CLEAR          0xFFC0421C         /*                                  */
#define PINT2_PINSTATE              0xFFC04220         /*                              */
#define PINT2_LATCH                 0xFFC04224         /*                           */

/*                          
             
                             */
#define PINT3_MASK_SET              0xFFC04300         /*                              */
#define PINT3_MASK_CLEAR            0xFFC04304         /*                                */
#define PINT3_REQUEST               0xFFC04308         /*                             */
#define PINT3_ASSIGN                0xFFC0430C         /*                            */
#define PINT3_EDGE_SET              0xFFC04310         /*                              */
#define PINT3_EDGE_CLEAR            0xFFC04314         /*                                */
#define PINT3_INVERT_SET            0xFFC04318         /*                                */
#define PINT3_INVERT_CLEAR          0xFFC0431C         /*                                  */
#define PINT3_PINSTATE              0xFFC04320         /*                              */
#define PINT3_LATCH                 0xFFC04324         /*                           */

/*                          
             
                             */
#define PINT4_MASK_SET              0xFFC04400         /*                              */
#define PINT4_MASK_CLEAR            0xFFC04404         /*                                */
#define PINT4_REQUEST               0xFFC04408         /*                             */
#define PINT4_ASSIGN                0xFFC0440C         /*                            */
#define PINT4_EDGE_SET              0xFFC04410         /*                              */
#define PINT4_EDGE_CLEAR            0xFFC04414         /*                                */
#define PINT4_INVERT_SET            0xFFC04418         /*                                */
#define PINT4_INVERT_CLEAR          0xFFC0441C         /*                                  */
#define PINT4_PINSTATE              0xFFC04420         /*                              */
#define PINT4_LATCH                 0xFFC04424         /*                           */

/*                          
             
                             */
#define PINT5_MASK_SET              0xFFC04500         /*                              */
#define PINT5_MASK_CLEAR            0xFFC04504         /*                                */
#define PINT5_REQUEST               0xFFC04508         /*                             */
#define PINT5_ASSIGN                0xFFC0450C         /*                            */
#define PINT5_EDGE_SET              0xFFC04510         /*                              */
#define PINT5_EDGE_CLEAR            0xFFC04514         /*                                */
#define PINT5_INVERT_SET            0xFFC04518         /*                                */
#define PINT5_INVERT_CLEAR          0xFFC0451C         /*                                  */
#define PINT5_PINSTATE              0xFFC04520         /*                              */
#define PINT5_LATCH                 0xFFC04524         /*                           */


/*                          
                     
                             */

/*                          
            
                             */
#define SMC_GCTL                   0xFFC16004         /*                           */
#define SMC_GSTAT                  0xFFC16008         /*                          */
#define SMC_B0CTL                  0xFFC1600C         /*                                 */
#define SMC_B0TIM                  0xFFC16010         /*                                */
#define SMC_B0ETIM                 0xFFC16014         /*                                         */
#define SMC_B1CTL                  0xFFC1601C         /*                                 */
#define SMC_B1TIM                  0xFFC16020         /*                                */
#define SMC_B1ETIM                 0xFFC16024         /*                                         */
#define SMC_B2CTL                  0xFFC1602C         /*                                 */
#define SMC_B2TIM                  0xFFC16030         /*                                */
#define SMC_B2ETIM                 0xFFC16034         /*                                         */
#define SMC_B3CTL                  0xFFC1603C         /*                                 */
#define SMC_B3TIM                  0xFFC16040         /*                                */
#define SMC_B3ETIM                 0xFFC16044         /*                                         */


/*                          
                      
                             */

/*                          
             
                             */
#define WDOG0_CTL                   0xFFC17000         /*                        */
#define WDOG0_CNT                   0xFFC17004         /*                      */
#define WDOG0_STAT                  0xFFC17008         /*                                      */
#define WDOG_CTL		WDOG0_CTL
#define WDOG_CNT		WDOG0_CNT
#define WDOG_STAT		WDOG0_STAT

/*                          
             
                             */
#define WDOG1_CTL                   0xFFC17800         /*                        */
#define WDOG1_CNT                   0xFFC17804         /*                      */
#define WDOG1_STAT                  0xFFC17808         /*                                      */


/*                          
                     
                             */

/*                          
            
                             */
#define SDU0_IDCODE                 0xFFC1F020         /*                       */
#define SDU0_CTL                    0xFFC1F050         /*                       */
#define SDU0_STAT                   0xFFC1F054         /*                      */
#define SDU0_MACCTL                 0xFFC1F058         /*                                     */
#define SDU0_MACADDR                0xFFC1F05C         /*                                     */
#define SDU0_MACDATA                0xFFC1F060         /*                                  */
#define SDU0_DMARD                  0xFFC1F064         /*                             */
#define SDU0_DMAWD                  0xFFC1F068         /*                              */
#define SDU0_MSG                    0xFFC1F080         /*                       */
#define SDU0_MSG_SET                0xFFC1F084         /*                           */
#define SDU0_MSG_CLR                0xFFC1F088         /*                             */
#define SDU0_GHLT                   0xFFC1F08C         /*                          */


/*                          
                      
                             */
/*                          
             
                             */
#define EMAC0_MACCFG                0xFFC20000         /*                                  */
#define EMAC0_MACFRMFILT            0xFFC20004         /*                                                     */
#define EMAC0_HASHTBL_HI            0xFFC20008         /*                                                    */
#define EMAC0_HASHTBL_LO            0xFFC2000C         /*                                                    */
#define EMAC0_GMII_ADDR             0xFFC20010         /*                                   */
#define EMAC0_GMII_DATA             0xFFC20014         /*                                */
#define EMAC0_FLOWCTL               0xFFC20018         /*                                 */
#define EMAC0_VLANTAG               0xFFC2001C         /*                         */
#define EMAC0_VER                   0xFFC20020         /*                             */
#define EMAC0_DBG                   0xFFC20024         /*                           */
#define EMAC0_RMTWKUP               0xFFC20028         /*                                     */
#define EMAC0_PMT_CTLSTAT           0xFFC2002C         /*                                       */
#define EMAC0_ISTAT                 0xFFC20038         /*                                      */
#define EMAC0_IMSK                  0xFFC2003C         /*                                    */
#define EMAC0_ADDR0_HI              0xFFC20040         /*                                   */
#define EMAC0_ADDR0_LO              0xFFC20044         /*                                  */
#define EMAC0_MMC_CTL               0xFFC20100         /*                            */
#define EMAC0_MMC_RXINT             0xFFC20104         /*                                 */
#define EMAC0_MMC_TXINT             0xFFC20108         /*                                 */
#define EMAC0_MMC_RXIMSK            0xFFC2010C         /*                                      */
#define EMAC0_MMC_TXIMSK            0xFFC20110         /*                                      */
#define EMAC0_TXOCTCNT_GB           0xFFC20114         /*                                                   */
#define EMAC0_TXFRMCNT_GB           0xFFC20118         /*                                                   */
#define EMAC0_TXBCASTFRM_G          0xFFC2011C         /*                                                    */
#define EMAC0_TXMCASTFRM_G          0xFFC20120         /*                                                    */
#define EMAC0_TX64_GB               0xFFC20124         /*                                       */
#define EMAC0_TX65TO127_GB          0xFFC20128         /*                                                               */
#define EMAC0_TX128TO255_GB         0xFFC2012C         /*                                                                */
#define EMAC0_TX256TO511_GB         0xFFC20130         /*                                                                */
#define EMAC0_TX512TO1023_GB        0xFFC20134         /*                                                                 */
#define EMAC0_TX1024TOMAX_GB        0xFFC20138         /*                                                                 */
#define EMAC0_TXUCASTFRM_GB         0xFFC2013C         /*                                                         */
#define EMAC0_TXMCASTFRM_GB         0xFFC20140         /*                                                           */
#define EMAC0_TXBCASTFRM_GB         0xFFC20144         /*                                                           */
#define EMAC0_TXUNDR_ERR            0xFFC20148         /*                                                             */
#define EMAC0_TXSNGCOL_G            0xFFC2014C         /*                                                           */
#define EMAC0_TXMULTCOL_G           0xFFC20150         /*                                                                 */
#define EMAC0_TXDEFERRED            0xFFC20154         /*                                                   */
#define EMAC0_TXLATECOL             0xFFC20158         /*                                                            */
#define EMAC0_TXEXCESSCOL           0xFFC2015C         /*                                                            */
#define EMAC0_TXCARR_ERR            0xFFC20160         /*                                                           */
#define EMAC0_TXOCTCNT_G            0xFFC20164         /*                                                       */
#define EMAC0_TXFRMCNT_G            0xFFC20168         /*                                          */
#define EMAC0_TXEXCESSDEF           0xFFC2016C         /*                                                          */
#define EMAC0_TXPAUSEFRM            0xFFC20170         /*                                                */
#define EMAC0_TXVLANFRM_G           0xFFC20174         /*                                         */
#define EMAC0_RXFRMCNT_GB           0xFFC20180         /*                                               */
#define EMAC0_RXOCTCNT_GB           0xFFC20184         /*                                                       */
#define EMAC0_RXOCTCNT_G            0xFFC20188         /*                                                    */
#define EMAC0_RXBCASTFRM_G          0xFFC2018C         /*                                                 */
#define EMAC0_RXMCASTFRM_G          0xFFC20190         /*                                                */
#define EMAC0_RXCRC_ERR             0xFFC20194         /*                                                */
#define EMAC0_RXALIGN_ERR           0xFFC20198         /*                                             */
#define EMAC0_RXRUNT_ERR            0xFFC2019C         /*                                                  */
#define EMAC0_RXJAB_ERR             0xFFC201A0         /*                                                               */
#define EMAC0_RXUSIZE_G             0xFFC201A4         /*                                                */
#define EMAC0_RXOSIZE_G             0xFFC201A8         /*                                                                 */
#define EMAC0_RX64_GB               0xFFC201AC         /*                                                       */
#define EMAC0_RX65TO127_GB          0xFFC201B0         /*                                                              */
#define EMAC0_RX128TO255_GB         0xFFC201B4         /*                                                                                                                        */
#define EMAC0_RX256TO511_GB         0xFFC201B8         /*                                                                */
#define EMAC0_RX512TO1023_GB        0xFFC201BC         /*                                                               */
#define EMAC0_RX1024TOMAX_GB        0xFFC201C0         /*                                                          */
#define EMAC0_RXUCASTFRM_G          0xFFC201C4         /*                                               */
#define EMAC0_RXLEN_ERR             0xFFC201C8         /*                                                   */
#define EMAC0_RXOORTYPE             0xFFC201CC         /*                                                                  */
#define EMAC0_RXPAUSEFRM            0xFFC201D0         /*                                                       */
#define EMAC0_RXFIFO_OVF            0xFFC201D4         /*                                                                                                                          */
#define EMAC0_RXVLANFRM_GB          0xFFC201D8         /*                                                    */
#define EMAC0_RXWDOG_ERR            0xFFC201DC         /*                                                          */
#define EMAC0_IPC_RXIMSK            0xFFC20200         /*                                          */
#define EMAC0_IPC_RXINT             0xFFC20208         /*                                     */
#define EMAC0_RXIPV4_GD_FRM         0xFFC20210         /*                                     */
#define EMAC0_RXIPV4_HDR_ERR_FRM    0xFFC20214         /*                                                   */
#define EMAC0_RXIPV4_NOPAY_FRM      0xFFC20218         /*                                                 */
#define EMAC0_RXIPV4_FRAG_FRM       0xFFC2021C         /*                                                        */
#define EMAC0_RXIPV4_UDSBL_FRM      0xFFC20220         /*                                                           */
#define EMAC0_RXIPV6_GD_FRM         0xFFC20224         /*                                                           */
#define EMAC0_RXIPV6_HDR_ERR_FRM    0xFFC20228         /*                                                   */
#define EMAC0_RXIPV6_NOPAY_FRM      0xFFC2022C         /*                                                             */
#define EMAC0_RXUDP_GD_FRM          0xFFC20230         /*                                                          */
#define EMAC0_RXUDP_ERR_FRM         0xFFC20234         /*                                                            */
#define EMAC0_RXTCP_GD_FRM          0xFFC20238         /*                                                           */
#define EMAC0_RXTCP_ERR_FRM         0xFFC2023C         /*                                                            */
#define EMAC0_RXICMP_GD_FRM         0xFFC20240         /*                                                            */
#define EMAC0_RXICMP_ERR_FRM        0xFFC20244         /*                                                             */
#define EMAC0_RXIPV4_GD_OCT         0xFFC20250         /*                                                                  */
#define EMAC0_RXIPV4_HDR_ERR_OCT    0xFFC20254         /*                                                           */
#define EMAC0_RXIPV4_NOPAY_OCT      0xFFC20258         /*                                                                  */
#define EMAC0_RXIPV4_FRAG_OCT       0xFFC2025C         /*                                                   */
#define EMAC0_RXIPV4_UDSBL_OCT      0xFFC20260         /*                                                            */
#define EMAC0_RXIPV6_GD_OCT         0xFFC20264         /*                                                                   */
#define EMAC0_RXIPV6_HDR_ERR_OCT    0xFFC20268         /*                                                           */
#define EMAC0_RXIPV6_NOPAY_OCT      0xFFC2026C         /*                                                           */
#define EMAC0_RXUDP_GD_OCT          0xFFC20270         /*                                                     */
#define EMAC0_RXUDP_ERR_OCT         0xFFC20274         /*                                                                 */
#define EMAC0_RXTCP_GD_OCT          0xFFC20278         /*                                                      */
#define EMAC0_RXTCP_ERR_OCT         0xFFC2027C         /*                                                                 */
#define EMAC0_RXICMP_GD_OCT         0xFFC20280         /*                                                       */
#define EMAC0_RXICMP_ERR_OCT        0xFFC20284         /*                                                              */
#define EMAC0_TM_CTL                0xFFC20700         /*                                        */
#define EMAC0_TM_SUBSEC             0xFFC20704         /*                                            */
#define EMAC0_TM_SEC                0xFFC20708         /*                                       */
#define EMAC0_TM_NSEC               0xFFC2070C         /*                                            */
#define EMAC0_TM_SECUPDT            0xFFC20710         /*                                      */
#define EMAC0_TM_NSECUPDT           0xFFC20714         /*                                           */
#define EMAC0_TM_ADDEND             0xFFC20718         /*                                       */
#define EMAC0_TM_TGTM               0xFFC2071C         /*                                        */
#define EMAC0_TM_NTGTM              0xFFC20720         /*                                            */
#define EMAC0_TM_HISEC              0xFFC20724         /*                                            */
#define EMAC0_TM_STMPSTAT           0xFFC20728         /*                                       */
#define EMAC0_TM_PPSCTL             0xFFC2072C         /*                                 */
#define EMAC0_TM_AUXSTMP_NSEC       0xFFC20730         /*                                               */
#define EMAC0_TM_AUXSTMP_SEC        0xFFC20734         /*                                              */
#define EMAC0_DMA_BUSMODE           0xFFC21000         /*                                        */
#define EMAC0_DMA_TXPOLL            0xFFC21004         /*                                   */
#define EMAC0_DMA_RXPOLL            0xFFC21008         /*                                   */
#define EMAC0_DMA_RXDSC_ADDR        0xFFC2100C         /*                                  */
#define EMAC0_DMA_TXDSC_ADDR        0xFFC21010         /*                                  */
#define EMAC0_DMA_STAT              0xFFC21014         /*                           */
#define EMAC0_DMA_OPMODE            0xFFC21018         /*                                   */
#define EMAC0_DMA_IEN               0xFFC2101C         /*                                     */
#define EMAC0_DMA_MISS_FRM          0xFFC21020         /*                                                    */
#define EMAC0_DMA_RXIWDOG           0xFFC21024         /*                                        */
#define EMAC0_DMA_BMMODE            0xFFC21028         /*                             */
#define EMAC0_DMA_BMSTAT            0xFFC2102C         /*                           */
#define EMAC0_DMA_TXDSC_CUR         0xFFC21048         /*                                      */
#define EMAC0_DMA_RXDSC_CUR         0xFFC2104C         /*                                      */
#define EMAC0_DMA_TXBUF_CUR         0xFFC21050         /*                                          */
#define EMAC0_DMA_RXBUF_CUR         0xFFC21054         /*                                          */
#define EMAC0_HWFEAT                0xFFC21058         /*                                 */

/*                          
             
                             */
#define EMAC1_MACCFG                0xFFC22000         /*                                  */
#define EMAC1_MACFRMFILT            0xFFC22004         /*                                                     */
#define EMAC1_HASHTBL_HI            0xFFC22008         /*                                                    */
#define EMAC1_HASHTBL_LO            0xFFC2200C         /*                                                    */
#define EMAC1_GMII_ADDR             0xFFC22010         /*                                   */
#define EMAC1_GMII_DATA             0xFFC22014         /*                                */
#define EMAC1_FLOWCTL               0xFFC22018         /*                                 */
#define EMAC1_VLANTAG               0xFFC2201C         /*                         */
#define EMAC1_VER                   0xFFC22020         /*                             */
#define EMAC1_DBG                   0xFFC22024         /*                           */
#define EMAC1_RMTWKUP               0xFFC22028         /*                                     */
#define EMAC1_PMT_CTLSTAT           0xFFC2202C         /*                                       */
#define EMAC1_ISTAT                 0xFFC22038         /*                                      */
#define EMAC1_IMSK                  0xFFC2203C         /*                                    */
#define EMAC1_ADDR0_HI              0xFFC22040         /*                                   */
#define EMAC1_ADDR0_LO              0xFFC22044         /*                                  */
#define EMAC1_MMC_CTL               0xFFC22100         /*                            */
#define EMAC1_MMC_RXINT             0xFFC22104         /*                                 */
#define EMAC1_MMC_TXINT             0xFFC22108         /*                                 */
#define EMAC1_MMC_RXIMSK            0xFFC2210C         /*                                      */
#define EMAC1_MMC_TXIMSK            0xFFC22110         /*                                      */
#define EMAC1_TXOCTCNT_GB           0xFFC22114         /*                                                   */
#define EMAC1_TXFRMCNT_GB           0xFFC22118         /*                                                   */
#define EMAC1_TXBCASTFRM_G          0xFFC2211C         /*                                                    */
#define EMAC1_TXMCASTFRM_G          0xFFC22120         /*                                                    */
#define EMAC1_TX64_GB               0xFFC22124         /*                                       */
#define EMAC1_TX65TO127_GB          0xFFC22128         /*                                                               */
#define EMAC1_TX128TO255_GB         0xFFC2212C         /*                                                                */
#define EMAC1_TX256TO511_GB         0xFFC22130         /*                                                                */
#define EMAC1_TX512TO1023_GB        0xFFC22134         /*                                                                 */
#define EMAC1_TX1024TOMAX_GB        0xFFC22138         /*                                                                 */
#define EMAC1_TXUCASTFRM_GB         0xFFC2213C         /*                                                         */
#define EMAC1_TXMCASTFRM_GB         0xFFC22140         /*                                                           */
#define EMAC1_TXBCASTFRM_GB         0xFFC22144         /*                                                           */
#define EMAC1_TXUNDR_ERR            0xFFC22148         /*                                                             */
#define EMAC1_TXSNGCOL_G            0xFFC2214C         /*                                                           */
#define EMAC1_TXMULTCOL_G           0xFFC22150         /*                                                                 */
#define EMAC1_TXDEFERRED            0xFFC22154         /*                                                   */
#define EMAC1_TXLATECOL             0xFFC22158         /*                                                            */
#define EMAC1_TXEXCESSCOL           0xFFC2215C         /*                                                            */
#define EMAC1_TXCARR_ERR            0xFFC22160         /*                                                           */
#define EMAC1_TXOCTCNT_G            0xFFC22164         /*                                                       */
#define EMAC1_TXFRMCNT_G            0xFFC22168         /*                                          */
#define EMAC1_TXEXCESSDEF           0xFFC2216C         /*                                                          */
#define EMAC1_TXPAUSEFRM            0xFFC22170         /*                                                */
#define EMAC1_TXVLANFRM_G           0xFFC22174         /*                                         */
#define EMAC1_RXFRMCNT_GB           0xFFC22180         /*                                               */
#define EMAC1_RXOCTCNT_GB           0xFFC22184         /*                                                       */
#define EMAC1_RXOCTCNT_G            0xFFC22188         /*                                                    */
#define EMAC1_RXBCASTFRM_G          0xFFC2218C         /*                                                 */
#define EMAC1_RXMCASTFRM_G          0xFFC22190         /*                                                */
#define EMAC1_RXCRC_ERR             0xFFC22194         /*                                                */
#define EMAC1_RXALIGN_ERR           0xFFC22198         /*                                             */
#define EMAC1_RXRUNT_ERR            0xFFC2219C         /*                                                  */
#define EMAC1_RXJAB_ERR             0xFFC221A0         /*                                                               */
#define EMAC1_RXUSIZE_G             0xFFC221A4         /*                                                */
#define EMAC1_RXOSIZE_G             0xFFC221A8         /*                                                                 */
#define EMAC1_RX64_GB               0xFFC221AC         /*                                                       */
#define EMAC1_RX65TO127_GB          0xFFC221B0         /*                                                              */
#define EMAC1_RX128TO255_GB         0xFFC221B4         /*                                                                                                                        */
#define EMAC1_RX256TO511_GB         0xFFC221B8         /*                                                                */
#define EMAC1_RX512TO1023_GB        0xFFC221BC         /*                                                               */
#define EMAC1_RX1024TOMAX_GB        0xFFC221C0         /*                                                          */
#define EMAC1_RXUCASTFRM_G          0xFFC221C4         /*                                               */
#define EMAC1_RXLEN_ERR             0xFFC221C8         /*                                                   */
#define EMAC1_RXOORTYPE             0xFFC221CC         /*                                                                  */
#define EMAC1_RXPAUSEFRM            0xFFC221D0         /*                                                       */
#define EMAC1_RXFIFO_OVF            0xFFC221D4         /*                                                                                                                          */
#define EMAC1_RXVLANFRM_GB          0xFFC221D8         /*                                                    */
#define EMAC1_RXWDOG_ERR            0xFFC221DC         /*                                                          */
#define EMAC1_IPC_RXIMSK            0xFFC22200         /*                                          */
#define EMAC1_IPC_RXINT             0xFFC22208         /*                                     */
#define EMAC1_RXIPV4_GD_FRM         0xFFC22210         /*                                     */
#define EMAC1_RXIPV4_HDR_ERR_FRM    0xFFC22214         /*                                                   */
#define EMAC1_RXIPV4_NOPAY_FRM      0xFFC22218         /*                                                 */
#define EMAC1_RXIPV4_FRAG_FRM       0xFFC2221C         /*                                                        */
#define EMAC1_RXIPV4_UDSBL_FRM      0xFFC22220         /*                                                           */
#define EMAC1_RXIPV6_GD_FRM         0xFFC22224         /*                                                           */
#define EMAC1_RXIPV6_HDR_ERR_FRM    0xFFC22228         /*                                                   */
#define EMAC1_RXIPV6_NOPAY_FRM      0xFFC2222C         /*                                                             */
#define EMAC1_RXUDP_GD_FRM          0xFFC22230         /*                                                          */
#define EMAC1_RXUDP_ERR_FRM         0xFFC22234         /*                                                            */
#define EMAC1_RXTCP_GD_FRM          0xFFC22238         /*                                                           */
#define EMAC1_RXTCP_ERR_FRM         0xFFC2223C         /*                                                            */
#define EMAC1_RXICMP_GD_FRM         0xFFC22240         /*                                                            */
#define EMAC1_RXICMP_ERR_FRM        0xFFC22244         /*                                                             */
#define EMAC1_RXIPV4_GD_OCT         0xFFC22250         /*                                                                  */
#define EMAC1_RXIPV4_HDR_ERR_OCT    0xFFC22254         /*                                                           */
#define EMAC1_RXIPV4_NOPAY_OCT      0xFFC22258         /*                                                                  */
#define EMAC1_RXIPV4_FRAG_OCT       0xFFC2225C         /*                                                   */
#define EMAC1_RXIPV4_UDSBL_OCT      0xFFC22260         /*                                                            */
#define EMAC1_RXIPV6_GD_OCT         0xFFC22264         /*                                                                   */
#define EMAC1_RXIPV6_HDR_ERR_OCT    0xFFC22268         /*                                                           */
#define EMAC1_RXIPV6_NOPAY_OCT      0xFFC2226C         /*                                                           */
#define EMAC1_RXUDP_GD_OCT          0xFFC22270         /*                                                     */
#define EMAC1_RXUDP_ERR_OCT         0xFFC22274         /*                                                                 */
#define EMAC1_RXTCP_GD_OCT          0xFFC22278         /*                                                      */
#define EMAC1_RXTCP_ERR_OCT         0xFFC2227C         /*                                                                 */
#define EMAC1_RXICMP_GD_OCT         0xFFC22280         /*                                                       */
#define EMAC1_RXICMP_ERR_OCT        0xFFC22284         /*                                                              */
#define EMAC1_TM_CTL                0xFFC22700         /*                                        */
#define EMAC1_TM_SUBSEC             0xFFC22704         /*                                            */
#define EMAC1_TM_SEC                0xFFC22708         /*                                       */
#define EMAC1_TM_NSEC               0xFFC2270C         /*                                            */
#define EMAC1_TM_SECUPDT            0xFFC22710         /*                                      */
#define EMAC1_TM_NSECUPDT           0xFFC22714         /*                                           */
#define EMAC1_TM_ADDEND             0xFFC22718         /*                                       */
#define EMAC1_TM_TGTM               0xFFC2271C         /*                                        */
#define EMAC1_TM_NTGTM              0xFFC22720         /*                                            */
#define EMAC1_TM_HISEC              0xFFC22724         /*                                            */
#define EMAC1_TM_STMPSTAT           0xFFC22728         /*                                       */
#define EMAC1_TM_PPSCTL             0xFFC2272C         /*                                 */
#define EMAC1_TM_AUXSTMP_NSEC       0xFFC22730         /*                                               */
#define EMAC1_TM_AUXSTMP_SEC        0xFFC22734         /*                                              */
#define EMAC1_DMA_BUSMODE           0xFFC23000         /*                                        */
#define EMAC1_DMA_TXPOLL            0xFFC23004         /*                                   */
#define EMAC1_DMA_RXPOLL            0xFFC23008         /*                                   */
#define EMAC1_DMA_RXDSC_ADDR        0xFFC2300C         /*                                  */
#define EMAC1_DMA_TXDSC_ADDR        0xFFC23010         /*                                  */
#define EMAC1_DMA_STAT              0xFFC23014         /*                           */
#define EMAC1_DMA_OPMODE            0xFFC23018         /*                                   */
#define EMAC1_DMA_IEN               0xFFC2301C         /*                                     */
#define EMAC1_DMA_MISS_FRM          0xFFC23020         /*                                                    */
#define EMAC1_DMA_RXIWDOG           0xFFC23024         /*                                        */
#define EMAC1_DMA_BMMODE            0xFFC23028         /*                             */
#define EMAC1_DMA_BMSTAT            0xFFC2302C         /*                           */
#define EMAC1_DMA_TXDSC_CUR         0xFFC23048         /*                                      */
#define EMAC1_DMA_RXDSC_CUR         0xFFC2304C         /*                                      */
#define EMAC1_DMA_TXBUF_CUR         0xFFC23050         /*                                          */
#define EMAC1_DMA_RXBUF_CUR         0xFFC23054         /*                                          */
#define EMAC1_HWFEAT                0xFFC23058         /*                                 */


/*                          
                     
                             */

/*                          
            
                             */
#define SPI0_REGBASE                0xFFC40400
#define SPI0_CTL                    0xFFC40404         /*                       */
#define SPI0_RXCTL                  0xFFC40408         /*                          */
#define SPI0_TXCTL                  0xFFC4040C         /*                          */
#define SPI0_CLK                    0xFFC40410         /*                          */
#define SPI0_DLY                    0xFFC40414         /*                     */
#define SPI0_SLVSEL                 0xFFC40418         /*                            */
#define SPI0_RWC                    0xFFC4041C         /*                                   */
#define SPI0_RWCR                   0xFFC40420         /*                                          */
#define SPI0_TWC                    0xFFC40424         /*                                      */
#define SPI0_TWCR                   0xFFC40428         /*                                             */
#define SPI0_IMSK                   0xFFC40430         /*                              */
#define SPI0_IMSK_CLR               0xFFC40434         /*                                    */
#define SPI0_IMSK_SET               0xFFC40438         /*                                  */
#define SPI0_STAT                   0xFFC40440         /*                      */
#define SPI0_ILAT                   0xFFC40444         /*                                          */
#define SPI0_ILAT_CLR               0xFFC40448         /*                                      */
#define SPI0_RFIFO                  0xFFC40450         /*                                 */
#define SPI0_TFIFO                  0xFFC40458         /*                                  */

/*                          
            
                             */
#define SPI1_REGBASE                0xFFC40500
#define SPI1_CTL                    0xFFC40504         /*                       */
#define SPI1_RXCTL                  0xFFC40508         /*                          */
#define SPI1_TXCTL                  0xFFC4050C         /*                          */
#define SPI1_CLK                    0xFFC40510         /*                          */
#define SPI1_DLY                    0xFFC40514         /*                     */
#define SPI1_SLVSEL                 0xFFC40518         /*                            */
#define SPI1_RWC                    0xFFC4051C         /*                                   */
#define SPI1_RWCR                   0xFFC40520         /*                                          */
#define SPI1_TWC                    0xFFC40524         /*                                      */
#define SPI1_TWCR                   0xFFC40528         /*                                             */
#define SPI1_IMSK                   0xFFC40530         /*                              */
#define SPI1_IMSK_CLR               0xFFC40534         /*                                    */
#define SPI1_IMSK_SET               0xFFC40538         /*                                  */
#define SPI1_STAT                   0xFFC40540         /*                      */
#define SPI1_ILAT                   0xFFC40544         /*                                          */
#define SPI1_ILAT_CLR               0xFFC40548         /*                                      */
#define SPI1_RFIFO                  0xFFC40550         /*                                 */
#define SPI1_TFIFO                  0xFFC40558         /*                                  */

/*                          
                
                             */

/*                          
       
                             */
#define SPORT0_CTL_A                0xFFC40000         /*                             */
#define SPORT0_DIV_A                0xFFC40004         /*                                         */
#define SPORT0_MCTL_A               0xFFC40008         /*                                          */
#define SPORT0_CS0_A                0xFFC4000C         /*                                                         */
#define SPORT0_CS1_A                0xFFC40010         /*                                                          */
#define SPORT0_CS2_A                0xFFC40014         /*                                                          */
#define SPORT0_CS3_A                0xFFC40018         /*                                                           */
#define SPORT0_CNT_A                0xFFC4001C         /*                                                       */
#define SPORT0_ERR_A                0xFFC40020         /*                           */
#define SPORT0_MSTAT_A              0xFFC40024         /*                                              */
#define SPORT0_CTL2_A               0xFFC40028         /*                               */
#define SPORT0_TXPRI_A              0xFFC40040         /*                                                     */
#define SPORT0_RXPRI_A              0xFFC40044         /*                                                    */
#define SPORT0_TXSEC_A              0xFFC40048         /*                                                       */
#define SPORT0_RXSEC_A              0xFFC4004C         /*                                                      */
#define SPORT0_CTL_B                0xFFC40080         /*                             */
#define SPORT0_DIV_B                0xFFC40084         /*                                         */
#define SPORT0_MCTL_B               0xFFC40088         /*                                          */
#define SPORT0_CS0_B                0xFFC4008C         /*                                                         */
#define SPORT0_CS1_B                0xFFC40090         /*                                                          */
#define SPORT0_CS2_B                0xFFC40094         /*                                                          */
#define SPORT0_CS3_B                0xFFC40098         /*                                                           */
#define SPORT0_CNT_B                0xFFC4009C         /*                                                       */
#define SPORT0_ERR_B                0xFFC400A0         /*                           */
#define SPORT0_MSTAT_B              0xFFC400A4         /*                                              */
#define SPORT0_CTL2_B               0xFFC400A8         /*                               */
#define SPORT0_TXPRI_B              0xFFC400C0         /*                                                     */
#define SPORT0_RXPRI_B              0xFFC400C4         /*                                                    */
#define SPORT0_TXSEC_B              0xFFC400C8         /*                                                       */
#define SPORT0_RXSEC_B              0xFFC400CC         /*                                                      */

/*                          
       
                             */
#define SPORT1_CTL_A                0xFFC40100         /*                             */
#define SPORT1_DIV_A                0xFFC40104         /*                                         */
#define SPORT1_MCTL_A               0xFFC40108         /*                                          */
#define SPORT1_CS0_A                0xFFC4010C         /*                                                         */
#define SPORT1_CS1_A                0xFFC40110         /*                                                          */
#define SPORT1_CS2_A                0xFFC40114         /*                                                          */
#define SPORT1_CS3_A                0xFFC40118         /*                                                           */
#define SPORT1_CNT_A                0xFFC4011C         /*                                                       */
#define SPORT1_ERR_A                0xFFC40120         /*                           */
#define SPORT1_MSTAT_A              0xFFC40124         /*                                              */
#define SPORT1_CTL2_A               0xFFC40128         /*                               */
#define SPORT1_TXPRI_A              0xFFC40140         /*                                                     */
#define SPORT1_RXPRI_A              0xFFC40144         /*                                                    */
#define SPORT1_TXSEC_A              0xFFC40148         /*                                                       */
#define SPORT1_RXSEC_A              0xFFC4014C         /*                                                      */
#define SPORT1_CTL_B                0xFFC40180         /*                             */
#define SPORT1_DIV_B                0xFFC40184         /*                                         */
#define SPORT1_MCTL_B               0xFFC40188         /*                                          */
#define SPORT1_CS0_B                0xFFC4018C         /*                                                         */
#define SPORT1_CS1_B                0xFFC40190         /*                                                          */
#define SPORT1_CS2_B                0xFFC40194         /*                                                          */
#define SPORT1_CS3_B                0xFFC40198         /*                                                           */
#define SPORT1_CNT_B                0xFFC4019C         /*                                                       */
#define SPORT1_ERR_B                0xFFC401A0         /*                           */
#define SPORT1_MSTAT_B              0xFFC401A4         /*                                              */
#define SPORT1_CTL2_B               0xFFC401A8         /*                               */
#define SPORT1_TXPRI_B              0xFFC401C0         /*                                                     */
#define SPORT1_RXPRI_B              0xFFC401C4         /*                                                    */
#define SPORT1_TXSEC_B              0xFFC401C8         /*                                                       */
#define SPORT1_RXSEC_B              0xFFC401CC         /*                                                      */

/*                          
       
                             */
#define SPORT2_CTL_A                0xFFC40200         /*                             */
#define SPORT2_DIV_A                0xFFC40204         /*                                         */
#define SPORT2_MCTL_A               0xFFC40208         /*                                          */
#define SPORT2_CS0_A                0xFFC4020C         /*                                                         */
#define SPORT2_CS1_A                0xFFC40210         /*                                                          */
#define SPORT2_CS2_A                0xFFC40214         /*                                                          */
#define SPORT2_CS3_A                0xFFC40218         /*                                                           */
#define SPORT2_CNT_A                0xFFC4021C         /*                                                       */
#define SPORT2_ERR_A                0xFFC40220         /*                           */
#define SPORT2_MSTAT_A              0xFFC40224         /*                                              */
#define SPORT2_CTL2_A               0xFFC40228         /*                               */
#define SPORT2_TXPRI_A              0xFFC40240         /*                                                     */
#define SPORT2_RXPRI_A              0xFFC40244         /*                                                    */
#define SPORT2_TXSEC_A              0xFFC40248         /*                                                       */
#define SPORT2_RXSEC_A              0xFFC4024C         /*                                                      */
#define SPORT2_CTL_B                0xFFC40280         /*                             */
#define SPORT2_DIV_B                0xFFC40284         /*                                         */
#define SPORT2_MCTL_B               0xFFC40288         /*                                          */
#define SPORT2_CS0_B                0xFFC4028C         /*                                                         */
#define SPORT2_CS1_B                0xFFC40290         /*                                                          */
#define SPORT2_CS2_B                0xFFC40294         /*                                                          */
#define SPORT2_CS3_B                0xFFC40298         /*                                                           */
#define SPORT2_CNT_B                0xFFC4029C         /*                                                       */
#define SPORT2_ERR_B                0xFFC402A0         /*                           */
#define SPORT2_MSTAT_B              0xFFC402A4         /*                                              */
#define SPORT2_CTL2_B               0xFFC402A8         /*                               */
#define SPORT2_TXPRI_B              0xFFC402C0         /*                                                     */
#define SPORT2_RXPRI_B              0xFFC402C4         /*                                                    */
#define SPORT2_TXSEC_B              0xFFC402C8         /*                                                       */
#define SPORT2_RXSEC_B              0xFFC402CC         /*                                                      */

/*                          
               
                             */

/*                          
      
                             */
#define EPPI0_STAT                  0xFFC18000         /*                       */
#define EPPI0_HCNT                  0xFFC18004         /*                                          */
#define EPPI0_HDLY                  0xFFC18008         /*                                       */
#define EPPI0_VCNT                  0xFFC1800C         /*                                        */
#define EPPI0_VDLY                  0xFFC18010         /*                                     */
#define EPPI0_FRAME                 0xFFC18014         /*                                */
#define EPPI0_LINE                  0xFFC18018         /*                                 */
#define EPPI0_CLKDIV                0xFFC1801C         /*                             */
#define EPPI0_CTL                   0xFFC18020         /*                        */
#define EPPI0_FS1_WLHB              0xFFC18024         /*                                                                               */
#define EPPI0_FS1_PASPL             0xFFC18028         /*                                                                   */
#define EPPI0_FS2_WLVB              0xFFC1802C         /*                                                                     */
#define EPPI0_FS2_PALPF             0xFFC18030         /*                                                                  */
#define EPPI0_IMSK                  0xFFC18034         /*                               */
#define EPPI0_ODDCLIP               0xFFC1803C         /*                                               */
#define EPPI0_EVENCLIP              0xFFC18040         /*                                              */
#define EPPI0_FS1_DLY               0xFFC18044         /*                                */
#define EPPI0_FS2_DLY               0xFFC18048         /*                                */
#define EPPI0_CTL2                  0xFFC1804C         /*                          */

/*                          
      
                             */
#define EPPI1_STAT                  0xFFC18400         /*                       */
#define EPPI1_HCNT                  0xFFC18404         /*                                          */
#define EPPI1_HDLY                  0xFFC18408         /*                                       */
#define EPPI1_VCNT                  0xFFC1840C         /*                                        */
#define EPPI1_VDLY                  0xFFC18410         /*                                     */
#define EPPI1_FRAME                 0xFFC18414         /*                                */
#define EPPI1_LINE                  0xFFC18418         /*                                 */
#define EPPI1_CLKDIV                0xFFC1841C         /*                             */
#define EPPI1_CTL                   0xFFC18420         /*                        */
#define EPPI1_FS1_WLHB              0xFFC18424         /*                                                                               */
#define EPPI1_FS1_PASPL             0xFFC18428         /*                                                                   */
#define EPPI1_FS2_WLVB              0xFFC1842C         /*                                                                     */
#define EPPI1_FS2_PALPF             0xFFC18430         /*                                                                  */
#define EPPI1_IMSK                  0xFFC18434         /*                               */
#define EPPI1_ODDCLIP               0xFFC1843C         /*                                               */
#define EPPI1_EVENCLIP              0xFFC18440         /*                                              */
#define EPPI1_FS1_DLY               0xFFC18444         /*                                */
#define EPPI1_FS2_DLY               0xFFC18448         /*                                */
#define EPPI1_CTL2                  0xFFC1844C         /*                          */

/*                          
      
                             */
#define EPPI2_STAT                  0xFFC18800         /*                       */
#define EPPI2_HCNT                  0xFFC18804         /*                                          */
#define EPPI2_HDLY                  0xFFC18808         /*                                       */
#define EPPI2_VCNT                  0xFFC1880C         /*                                        */
#define EPPI2_VDLY                  0xFFC18810         /*                                     */
#define EPPI2_FRAME                 0xFFC18814         /*                                */
#define EPPI2_LINE                  0xFFC18818         /*                                 */
#define EPPI2_CLKDIV                0xFFC1881C         /*                             */
#define EPPI2_CTL                   0xFFC18820         /*                        */
#define EPPI2_FS1_WLHB              0xFFC18824         /*                                                                               */
#define EPPI2_FS1_PASPL             0xFFC18828         /*                                                                   */
#define EPPI2_FS2_WLVB              0xFFC1882C         /*                                                                     */
#define EPPI2_FS2_PALPF             0xFFC18830         /*                                                                  */
#define EPPI2_IMSK                  0xFFC18834         /*                               */
#define EPPI2_ODDCLIP               0xFFC1883C         /*                                               */
#define EPPI2_EVENCLIP              0xFFC18840         /*                                              */
#define EPPI2_FS1_DLY               0xFFC18844         /*                                */
#define EPPI2_FS2_DLY               0xFFC18848         /*                                */
#define EPPI2_CTL2                  0xFFC1884C         /*                          */



/*                          
                     
                             */

/*                          
            
                             */
#define DMA0_NEXT_DESC_PTR          0xFFC41000         /*                                         */
#define DMA0_START_ADDR             0xFFC41004         /*                                      */
#define DMA0_CONFIG                 0xFFC41008         /*                             */
#define DMA0_X_COUNT                0xFFC4100C         /*                                   */
#define DMA0_X_MODIFY               0xFFC41010         /*                                   */
#define DMA0_Y_COUNT                0xFFC41014         /*                                             */
#define DMA0_Y_MODIFY               0xFFC41018         /*                                             */
#define DMA0_CURR_DESC_PTR          0xFFC41024         /*                                 */
#define DMA0_PREV_DESC_PTR          0xFFC41028         /*                                          */
#define DMA0_CURR_ADDR              0xFFC4102C         /*                      */
#define DMA0_IRQ_STATUS             0xFFC41030         /*                      */
#define DMA0_CURR_X_COUNT           0xFFC41034         /*                                               */
#define DMA0_CURR_Y_COUNT           0xFFC41038         /*                                  */
#define DMA0_BWL_COUNT              0xFFC41040         /*                            */
#define DMA0_CURR_BWL_COUNT         0xFFC41044         /*                                    */
#define DMA0_BWM_COUNT              0xFFC41048         /*                              */
#define DMA0_CURR_BWM_COUNT         0xFFC4104C         /*                                      */

/*                          
            
                             */
#define DMA1_NEXT_DESC_PTR             0xFFC41080         /*                                         */
#define DMA1_START_ADDR              0xFFC41084         /*                                      */
#define DMA1_CONFIG                    0xFFC41088         /*                             */
#define DMA1_X_COUNT                   0xFFC4108C         /*                                   */
#define DMA1_X_MODIFY                   0xFFC41090         /*                                   */
#define DMA1_Y_COUNT                   0xFFC41094         /*                                             */
#define DMA1_Y_MODIFY                   0xFFC41098         /*                                             */
#define DMA1_CURR_DESC_PTR             0xFFC410A4         /*                                 */
#define DMA1_PREV_DESC_PTR             0xFFC410A8         /*                                          */
#define DMA1_CURR_ADDR               0xFFC410AC         /*                      */
#define DMA1_IRQ_STATUS                   0xFFC410B0         /*                      */
#define DMA1_CURR_X_COUNT               0xFFC410B4         /*                                               */
#define DMA1_CURR_Y_COUNT               0xFFC410B8         /*                                  */
#define DMA1_BWL_COUNT                 0xFFC410C0         /*                            */
#define DMA1_CURR_BWL_COUNT             0xFFC410C4         /*                                    */
#define DMA1_BWM_COUNT                 0xFFC410C8         /*                              */
#define DMA1_CURR_BWM_COUNT             0xFFC410CC         /*                                      */

/*                          
            
                             */
#define DMA2_NEXT_DESC_PTR             0xFFC41100         /*                                         */
#define DMA2_START_ADDR              0xFFC41104         /*                                      */
#define DMA2_CONFIG                    0xFFC41108         /*                             */
#define DMA2_X_COUNT                   0xFFC4110C         /*                                   */
#define DMA2_X_MODIFY                   0xFFC41110         /*                                   */
#define DMA2_Y_COUNT                   0xFFC41114         /*                                             */
#define DMA2_Y_MODIFY                   0xFFC41118         /*                                             */
#define DMA2_CURR_DESC_PTR             0xFFC41124         /*                                 */
#define DMA2_PREV_DESC_PTR             0xFFC41128         /*                                          */
#define DMA2_CURR_ADDR               0xFFC4112C         /*                      */
#define DMA2_IRQ_STATUS                   0xFFC41130         /*                      */
#define DMA2_CURR_X_COUNT               0xFFC41134         /*                                               */
#define DMA2_CURR_Y_COUNT               0xFFC41138         /*                                  */
#define DMA2_BWL_COUNT                 0xFFC41140         /*                            */
#define DMA2_CURR_BWL_COUNT             0xFFC41144         /*                                    */
#define DMA2_BWM_COUNT                 0xFFC41148         /*                              */
#define DMA2_CURR_BWM_COUNT             0xFFC4114C         /*                                      */

/*                          
            
                             */
#define DMA3_NEXT_DESC_PTR             0xFFC41180         /*                                         */
#define DMA3_START_ADDR              0xFFC41184         /*                                      */
#define DMA3_CONFIG                    0xFFC41188         /*                             */
#define DMA3_X_COUNT                   0xFFC4118C         /*                                   */
#define DMA3_X_MODIFY                   0xFFC41190         /*                                   */
#define DMA3_Y_COUNT                   0xFFC41194         /*                                             */
#define DMA3_Y_MODIFY                   0xFFC41198         /*                                             */
#define DMA3_CURR_DESC_PTR             0xFFC411A4         /*                                 */
#define DMA3_PREV_DESC_PTR             0xFFC411A8         /*                                          */
#define DMA3_CURR_ADDR               0xFFC411AC         /*                      */
#define DMA3_IRQ_STATUS                   0xFFC411B0         /*                      */
#define DMA3_CURR_X_COUNT               0xFFC411B4         /*                                               */
#define DMA3_CURR_Y_COUNT               0xFFC411B8         /*                                  */
#define DMA3_BWL_COUNT                 0xFFC411C0         /*                            */
#define DMA3_CURR_BWL_COUNT             0xFFC411C4         /*                                    */
#define DMA3_BWM_COUNT                 0xFFC411C8         /*                              */
#define DMA3_CURR_BWM_COUNT             0xFFC411CC         /*                                      */

/*                          
            
                             */
#define DMA4_NEXT_DESC_PTR             0xFFC41200         /*                                         */
#define DMA4_START_ADDR              0xFFC41204         /*                                      */
#define DMA4_CONFIG                    0xFFC41208         /*                             */
#define DMA4_X_COUNT                   0xFFC4120C         /*                                   */
#define DMA4_X_MODIFY                   0xFFC41210         /*                                   */
#define DMA4_Y_COUNT                   0xFFC41214         /*                                             */
#define DMA4_Y_MODIFY                   0xFFC41218         /*                                             */
#define DMA4_CURR_DESC_PTR             0xFFC41224         /*                                 */
#define DMA4_PREV_DESC_PTR             0xFFC41228         /*                                          */
#define DMA4_CURR_ADDR               0xFFC4122C         /*                      */
#define DMA4_IRQ_STATUS                   0xFFC41230         /*                      */
#define DMA4_CURR_X_COUNT               0xFFC41234         /*                                               */
#define DMA4_CURR_Y_COUNT               0xFFC41238         /*                                  */
#define DMA4_BWL_COUNT                 0xFFC41240         /*                            */
#define DMA4_CURR_BWL_COUNT             0xFFC41244         /*                                    */
#define DMA4_BWM_COUNT                 0xFFC41248         /*                              */
#define DMA4_CURR_BWM_COUNT             0xFFC4124C         /*                                      */

/*                          
            
                             */
#define DMA5_NEXT_DESC_PTR             0xFFC41280         /*                                         */
#define DMA5_START_ADDR              0xFFC41284         /*                                      */
#define DMA5_CONFIG                    0xFFC41288         /*                             */
#define DMA5_X_COUNT                   0xFFC4128C         /*                                   */
#define DMA5_X_MODIFY                   0xFFC41290         /*                                   */
#define DMA5_Y_COUNT                   0xFFC41294         /*                                             */
#define DMA5_Y_MODIFY                   0xFFC41298         /*                                             */
#define DMA5_CURR_DESC_PTR             0xFFC412A4         /*                                 */
#define DMA5_PREV_DESC_PTR             0xFFC412A8         /*                                          */
#define DMA5_CURR_ADDR               0xFFC412AC         /*                      */
#define DMA5_IRQ_STATUS                   0xFFC412B0         /*                      */
#define DMA5_CURR_X_COUNT               0xFFC412B4         /*                                               */
#define DMA5_CURR_Y_COUNT               0xFFC412B8         /*                                  */
#define DMA5_BWL_COUNT                 0xFFC412C0         /*                            */
#define DMA5_CURR_BWL_COUNT             0xFFC412C4         /*                                    */
#define DMA5_BWM_COUNT                 0xFFC412C8         /*                              */
#define DMA5_CURR_BWM_COUNT             0xFFC412CC         /*                                      */

/*                          
            
                             */
#define DMA6_NEXT_DESC_PTR             0xFFC41300         /*                                         */
#define DMA6_START_ADDR              0xFFC41304         /*                                      */
#define DMA6_CONFIG                    0xFFC41308         /*                             */
#define DMA6_X_COUNT                   0xFFC4130C         /*                                   */
#define DMA6_X_MODIFY                   0xFFC41310         /*                                   */
#define DMA6_Y_COUNT                   0xFFC41314         /*                                             */
#define DMA6_Y_MODIFY                   0xFFC41318         /*                                             */
#define DMA6_CURR_DESC_PTR             0xFFC41324         /*                                 */
#define DMA6_PREV_DESC_PTR             0xFFC41328         /*                                          */
#define DMA6_CURR_ADDR               0xFFC4132C         /*                      */
#define DMA6_IRQ_STATUS                   0xFFC41330         /*                      */
#define DMA6_CURR_X_COUNT               0xFFC41334         /*                                               */
#define DMA6_CURR_Y_COUNT               0xFFC41338         /*                                  */
#define DMA6_BWL_COUNT                 0xFFC41340         /*                            */
#define DMA6_CURR_BWL_COUNT             0xFFC41344         /*                                    */
#define DMA6_BWM_COUNT                 0xFFC41348         /*                              */
#define DMA6_CURR_BWM_COUNT             0xFFC4134C         /*                                      */

/*                          
            
                             */
#define DMA7_NEXT_DESC_PTR             0xFFC41380         /*                                         */
#define DMA7_START_ADDR              0xFFC41384         /*                                      */
#define DMA7_CONFIG                    0xFFC41388         /*                             */
#define DMA7_X_COUNT                   0xFFC4138C         /*                                   */
#define DMA7_X_MODIFY                   0xFFC41390         /*                                   */
#define DMA7_Y_COUNT                   0xFFC41394         /*                                             */
#define DMA7_Y_MODIFY                   0xFFC41398         /*                                             */
#define DMA7_CURR_DESC_PTR             0xFFC413A4         /*                                 */
#define DMA7_PREV_DESC_PTR             0xFFC413A8         /*                                          */
#define DMA7_CURR_ADDR               0xFFC413AC         /*                      */
#define DMA7_IRQ_STATUS                   0xFFC413B0         /*                      */
#define DMA7_CURR_X_COUNT               0xFFC413B4         /*                                               */
#define DMA7_CURR_Y_COUNT               0xFFC413B8         /*                                  */
#define DMA7_BWL_COUNT                 0xFFC413C0         /*                            */
#define DMA7_CURR_BWL_COUNT             0xFFC413C4         /*                                    */
#define DMA7_BWM_COUNT                 0xFFC413C8         /*                              */
#define DMA7_CURR_BWM_COUNT             0xFFC413CC         /*                                      */

/*                          
            
                             */
#define DMA8_NEXT_DESC_PTR             0xFFC41400         /*                                         */
#define DMA8_START_ADDR              0xFFC41404         /*                                      */
#define DMA8_CONFIG                    0xFFC41408         /*                             */
#define DMA8_X_COUNT                   0xFFC4140C         /*                                   */
#define DMA8_X_MODIFY                   0xFFC41410         /*                                   */
#define DMA8_Y_COUNT                   0xFFC41414         /*                                             */
#define DMA8_Y_MODIFY                   0xFFC41418         /*                                             */
#define DMA8_CURR_DESC_PTR             0xFFC41424         /*                                 */
#define DMA8_PREV_DESC_PTR             0xFFC41428         /*                                          */
#define DMA8_CURR_ADDR               0xFFC4142C         /*                      */
#define DMA8_IRQ_STATUS                   0xFFC41430         /*                      */
#define DMA8_CURR_X_COUNT               0xFFC41434         /*                                               */
#define DMA8_CURR_Y_COUNT               0xFFC41438         /*                                  */
#define DMA8_BWL_COUNT                 0xFFC41440         /*                            */
#define DMA8_CURR_BWL_COUNT             0xFFC41444         /*                                    */
#define DMA8_BWM_COUNT                 0xFFC41448         /*                              */
#define DMA8_CURR_BWM_COUNT             0xFFC4144C         /*                                      */

/*                          
            
                             */
#define DMA9_NEXT_DESC_PTR             0xFFC41480         /*                                         */
#define DMA9_START_ADDR              0xFFC41484         /*                                      */
#define DMA9_CONFIG                    0xFFC41488         /*                             */
#define DMA9_X_COUNT                   0xFFC4148C         /*                                   */
#define DMA9_X_MODIFY                   0xFFC41490         /*                                   */
#define DMA9_Y_COUNT                   0xFFC41494         /*                                             */
#define DMA9_Y_MODIFY                   0xFFC41498         /*                                             */
#define DMA9_CURR_DESC_PTR             0xFFC414A4         /*                                 */
#define DMA9_PREV_DESC_PTR             0xFFC414A8         /*                                          */
#define DMA9_CURR_ADDR               0xFFC414AC         /*                      */
#define DMA9_IRQ_STATUS                   0xFFC414B0         /*                      */
#define DMA9_CURR_X_COUNT               0xFFC414B4         /*                                               */
#define DMA9_CURR_Y_COUNT               0xFFC414B8         /*                                  */
#define DMA9_BWL_COUNT                 0xFFC414C0         /*                            */
#define DMA9_CURR_BWL_COUNT             0xFFC414C4         /*                                    */
#define DMA9_BWM_COUNT                 0xFFC414C8         /*                              */
#define DMA9_CURR_BWM_COUNT             0xFFC414CC         /*                                      */

/*                          
             
                             */
#define DMA10_NEXT_DESC_PTR            0xFFC05000         /*                                          */
#define DMA10_START_ADDR             0xFFC05004         /*                                       */
#define DMA10_CONFIG                   0xFFC05008         /*                              */
#define DMA10_X_COUNT                  0xFFC0500C         /*                                    */
#define DMA10_X_MODIFY                  0xFFC05010         /*                                    */
#define DMA10_Y_COUNT                  0xFFC05014         /*                                              */
#define DMA10_Y_MODIFY                  0xFFC05018         /*                                              */
#define DMA10_CURR_DESC_PTR            0xFFC05024         /*                                  */
#define DMA10_PREV_DESC_PTR            0xFFC05028         /*                                           */
#define DMA10_CURR_ADDR              0xFFC0502C         /*                       */
#define DMA10_IRQ_STATUS                  0xFFC05030         /*                       */
#define DMA10_CURR_X_COUNT              0xFFC05034         /*                                                */
#define DMA10_CURR_Y_COUNT              0xFFC05038         /*                                   */
#define DMA10_BWL_COUNT                0xFFC05040         /*                             */
#define DMA10_CURR_BWL_COUNT            0xFFC05044         /*                                     */
#define DMA10_BWM_COUNT                0xFFC05048         /*                               */
#define DMA10_CURR_BWM_COUNT            0xFFC0504C         /*                                       */

/*                          
             
                             */
#define DMA11_NEXT_DESC_PTR            0xFFC05080         /*                                          */
#define DMA11_START_ADDR             0xFFC05084         /*                                       */
#define DMA11_CONFIG                   0xFFC05088         /*                              */
#define DMA11_X_COUNT                  0xFFC0508C         /*                                    */
#define DMA11_X_MODIFY                  0xFFC05090         /*                                    */
#define DMA11_Y_COUNT                  0xFFC05094         /*                                              */
#define DMA11_Y_MODIFY                  0xFFC05098         /*                                              */
#define DMA11_CURR_DESC_PTR            0xFFC050A4         /*                                  */
#define DMA11_PREV_DESC_PTR            0xFFC050A8         /*                                           */
#define DMA11_CURR_ADDR              0xFFC050AC         /*                       */
#define DMA11_IRQ_STATUS                  0xFFC050B0         /*                       */
#define DMA11_CURR_X_COUNT              0xFFC050B4         /*                                                */
#define DMA11_CURR_Y_COUNT              0xFFC050B8         /*                                   */
#define DMA11_BWL_COUNT                0xFFC050C0         /*                             */
#define DMA11_CURR_BWL_COUNT            0xFFC050C4         /*                                     */
#define DMA11_BWM_COUNT                0xFFC050C8         /*                               */
#define DMA11_CURR_BWM_COUNT            0xFFC050CC         /*                                       */

/*                          
             
                             */
#define DMA12_NEXT_DESC_PTR            0xFFC05100         /*                                          */
#define DMA12_START_ADDR             0xFFC05104         /*                                       */
#define DMA12_CONFIG                   0xFFC05108         /*                              */
#define DMA12_X_COUNT                  0xFFC0510C         /*                                    */
#define DMA12_X_MODIFY                  0xFFC05110         /*                                    */
#define DMA12_Y_COUNT                  0xFFC05114         /*                                              */
#define DMA12_Y_MODIFY                  0xFFC05118         /*                                              */
#define DMA12_CURR_DESC_PTR            0xFFC05124         /*                                  */
#define DMA12_PREV_DESC_PTR            0xFFC05128         /*                                           */
#define DMA12_CURR_ADDR              0xFFC0512C         /*                       */
#define DMA12_IRQ_STATUS                  0xFFC05130         /*                       */
#define DMA12_CURR_X_COUNT              0xFFC05134         /*                                                */
#define DMA12_CURR_Y_COUNT              0xFFC05138         /*                                   */
#define DMA12_BWL_COUNT                0xFFC05140         /*                             */
#define DMA12_CURR_BWL_COUNT            0xFFC05144         /*                                     */
#define DMA12_BWM_COUNT                0xFFC05148         /*                               */
#define DMA12_CURR_BWM_COUNT            0xFFC0514C         /*                                       */

/*                          
             
                             */
#define DMA13_NEXT_DESC_PTR            0xFFC07000         /*                                          */
#define DMA13_START_ADDR             0xFFC07004         /*                                       */
#define DMA13_CONFIG                   0xFFC07008         /*                              */
#define DMA13_X_COUNT                  0xFFC0700C         /*                                    */
#define DMA13_X_MODIFY                  0xFFC07010         /*                                    */
#define DMA13_Y_COUNT                  0xFFC07014         /*                                              */
#define DMA13_Y_MODIFY                  0xFFC07018         /*                                              */
#define DMA13_CURR_DESC_PTR            0xFFC07024         /*                                  */
#define DMA13_PREV_DESC_PTR            0xFFC07028         /*                                           */
#define DMA13_CURR_ADDR              0xFFC0702C         /*                       */
#define DMA13_IRQ_STATUS                  0xFFC07030         /*                       */
#define DMA13_CURR_X_COUNT              0xFFC07034         /*                                                */
#define DMA13_CURR_Y_COUNT              0xFFC07038         /*                                   */
#define DMA13_BWL_COUNT                0xFFC07040         /*                             */
#define DMA13_CURR_BWL_COUNT            0xFFC07044         /*                                     */
#define DMA13_BWM_COUNT                0xFFC07048         /*                               */
#define DMA13_CURR_BWM_COUNT            0xFFC0704C         /*                                       */

/*                          
             
                             */
#define DMA14_NEXT_DESC_PTR            0xFFC07080         /*                                          */
#define DMA14_START_ADDR             0xFFC07084         /*                                       */
#define DMA14_CONFIG                   0xFFC07088         /*                              */
#define DMA14_X_COUNT                  0xFFC0708C         /*                                    */
#define DMA14_X_MODIFY                  0xFFC07090         /*                                    */
#define DMA14_Y_COUNT                  0xFFC07094         /*                                              */
#define DMA14_Y_MODIFY                  0xFFC07098         /*                                              */
#define DMA14_CURR_DESC_PTR            0xFFC070A4         /*                                  */
#define DMA14_PREV_DESC_PTR            0xFFC070A8         /*                                           */
#define DMA14_CURR_ADDR              0xFFC070AC         /*                       */
#define DMA14_IRQ_STATUS                  0xFFC070B0         /*                       */
#define DMA14_CURR_X_COUNT              0xFFC070B4         /*                                                */
#define DMA14_CURR_Y_COUNT              0xFFC070B8         /*                                   */
#define DMA14_BWL_COUNT                0xFFC070C0         /*                             */
#define DMA14_CURR_BWL_COUNT            0xFFC070C4         /*                                     */
#define DMA14_BWM_COUNT                0xFFC070C8         /*                               */
#define DMA14_CURR_BWM_COUNT            0xFFC070CC         /*                                       */

/*                          
             
                             */
#define DMA15_NEXT_DESC_PTR            0xFFC07100         /*                                          */
#define DMA15_START_ADDR             0xFFC07104         /*                                       */
#define DMA15_CONFIG                   0xFFC07108         /*                              */
#define DMA15_X_COUNT                  0xFFC0710C         /*                                    */
#define DMA15_X_MODIFY                  0xFFC07110         /*                                    */
#define DMA15_Y_COUNT                  0xFFC07114         /*                                              */
#define DMA15_Y_MODIFY                  0xFFC07118         /*                                              */
#define DMA15_CURR_DESC_PTR            0xFFC07124         /*                                  */
#define DMA15_PREV_DESC_PTR            0xFFC07128         /*                                           */
#define DMA15_CURR_ADDR              0xFFC0712C         /*                       */
#define DMA15_IRQ_STATUS                  0xFFC07130         /*                       */
#define DMA15_CURR_X_COUNT              0xFFC07134         /*                                                */
#define DMA15_CURR_Y_COUNT              0xFFC07138         /*                                   */
#define DMA15_BWL_COUNT                0xFFC07140         /*                             */
#define DMA15_CURR_BWL_COUNT            0xFFC07144         /*                                     */
#define DMA15_BWM_COUNT                0xFFC07148         /*                               */
#define DMA15_CURR_BWM_COUNT            0xFFC0714C         /*                                       */

/*                          
             
                             */
#define DMA16_NEXT_DESC_PTR            0xFFC07180         /*                                          */
#define DMA16_START_ADDR             0xFFC07184         /*                                       */
#define DMA16_CONFIG                   0xFFC07188         /*                              */
#define DMA16_X_COUNT                  0xFFC0718C         /*                                    */
#define DMA16_X_MODIFY                  0xFFC07190         /*                                    */
#define DMA16_Y_COUNT                  0xFFC07194         /*                                              */
#define DMA16_Y_MODIFY                  0xFFC07198         /*                                              */
#define DMA16_CURR_DESC_PTR            0xFFC071A4         /*                                  */
#define DMA16_PREV_DESC_PTR            0xFFC071A8         /*                                           */
#define DMA16_CURR_ADDR              0xFFC071AC         /*                       */
#define DMA16_IRQ_STATUS                  0xFFC071B0         /*                       */
#define DMA16_CURR_X_COUNT              0xFFC071B4         /*                                                */
#define DMA16_CURR_Y_COUNT              0xFFC071B8         /*                                   */
#define DMA16_BWL_COUNT                0xFFC071C0         /*                             */
#define DMA16_CURR_BWL_COUNT            0xFFC071C4         /*                                     */
#define DMA16_BWM_COUNT                0xFFC071C8         /*                               */
#define DMA16_CURR_BWM_COUNT            0xFFC071CC         /*                                       */

/*                          
             
                             */
#define DMA17_NEXT_DESC_PTR            0xFFC07200         /*                                          */
#define DMA17_START_ADDR             0xFFC07204         /*                                       */
#define DMA17_CONFIG                   0xFFC07208         /*                              */
#define DMA17_X_COUNT                  0xFFC0720C         /*                                    */
#define DMA17_X_MODIFY                  0xFFC07210         /*                                    */
#define DMA17_Y_COUNT                  0xFFC07214         /*                                              */
#define DMA17_Y_MODIFY                  0xFFC07218         /*                                              */
#define DMA17_CURR_DESC_PTR            0xFFC07224         /*                                  */
#define DMA17_PREV_DESC_PTR            0xFFC07228         /*                                           */
#define DMA17_CURR_ADDR              0xFFC0722C         /*                       */
#define DMA17_IRQ_STATUS                  0xFFC07230         /*                       */
#define DMA17_CURR_X_COUNT              0xFFC07234         /*                                                */
#define DMA17_CURR_Y_COUNT              0xFFC07238         /*                                   */
#define DMA17_BWL_COUNT                0xFFC07240         /*                             */
#define DMA17_CURR_BWL_COUNT            0xFFC07244         /*                                     */
#define DMA17_BWM_COUNT                0xFFC07248         /*                               */
#define DMA17_CURR_BWM_COUNT            0xFFC0724C         /*                                       */

/*                          
             
                             */
#define DMA18_NEXT_DESC_PTR            0xFFC07280         /*                                          */
#define DMA18_START_ADDR             0xFFC07284         /*                                       */
#define DMA18_CONFIG                   0xFFC07288         /*                              */
#define DMA18_X_COUNT                  0xFFC0728C         /*                                    */
#define DMA18_X_MODIFY                  0xFFC07290         /*                                    */
#define DMA18_Y_COUNT                  0xFFC07294         /*                                              */
#define DMA18_Y_MODIFY                  0xFFC07298         /*                                              */
#define DMA18_CURR_DESC_PTR            0xFFC072A4         /*                                  */
#define DMA18_PREV_DESC_PTR            0xFFC072A8         /*                                           */
#define DMA18_CURR_ADDR              0xFFC072AC         /*                       */
#define DMA18_IRQ_STATUS                  0xFFC072B0         /*                       */
#define DMA18_CURR_X_COUNT              0xFFC072B4         /*                                                */
#define DMA18_CURR_Y_COUNT              0xFFC072B8         /*                                   */
#define DMA18_BWL_COUNT                0xFFC072C0         /*                             */
#define DMA18_CURR_BWL_COUNT            0xFFC072C4         /*                                     */
#define DMA18_BWM_COUNT                0xFFC072C8         /*                               */
#define DMA18_CURR_BWM_COUNT            0xFFC072CC         /*                                       */

/*                          
             
                             */
#define DMA19_NEXT_DESC_PTR            0xFFC07300         /*                                          */
#define DMA19_START_ADDR             0xFFC07304         /*                                       */
#define DMA19_CONFIG                   0xFFC07308         /*                              */
#define DMA19_X_COUNT                  0xFFC0730C         /*                                    */
#define DMA19_X_MODIFY                  0xFFC07310         /*                                    */
#define DMA19_Y_COUNT                  0xFFC07314         /*                                              */
#define DMA19_Y_MODIFY                  0xFFC07318         /*                                              */
#define DMA19_CURR_DESC_PTR            0xFFC07324         /*                                  */
#define DMA19_PREV_DESC_PTR            0xFFC07328         /*                                           */
#define DMA19_CURR_ADDR              0xFFC0732C         /*                       */
#define DMA19_IRQ_STATUS                  0xFFC07330         /*                       */
#define DMA19_CURR_X_COUNT              0xFFC07334         /*                                                */
#define DMA19_CURR_Y_COUNT              0xFFC07338         /*                                   */
#define DMA19_BWL_COUNT                0xFFC07340         /*                             */
#define DMA19_CURR_BWL_COUNT            0xFFC07344         /*                                     */
#define DMA19_BWM_COUNT                0xFFC07348         /*                               */
#define DMA19_CURR_BWM_COUNT            0xFFC0734C         /*                                       */

/*                          
             
                             */
#define DMA20_NEXT_DESC_PTR            0xFFC07380         /*                                          */
#define DMA20_START_ADDR             0xFFC07384         /*                                       */
#define DMA20_CONFIG                   0xFFC07388         /*                              */
#define DMA20_X_COUNT                  0xFFC0738C         /*                                    */
#define DMA20_X_MODIFY                  0xFFC07390         /*                                    */
#define DMA20_Y_COUNT                  0xFFC07394         /*                                              */
#define DMA20_Y_MODIFY                  0xFFC07398         /*                                              */
#define DMA20_CURR_DESC_PTR            0xFFC073A4         /*                                  */
#define DMA20_PREV_DESC_PTR            0xFFC073A8         /*                                           */
#define DMA20_CURR_ADDR              0xFFC073AC         /*                       */
#define DMA20_IRQ_STATUS                  0xFFC073B0         /*                       */
#define DMA20_CURR_X_COUNT              0xFFC073B4         /*                                                */
#define DMA20_CURR_Y_COUNT              0xFFC073B8         /*                                   */
#define DMA20_BWL_COUNT                0xFFC073C0         /*                             */
#define DMA20_CURR_BWL_COUNT            0xFFC073C4         /*                                     */
#define DMA20_BWM_COUNT                0xFFC073C8         /*                               */
#define DMA20_CURR_BWM_COUNT            0xFFC073CC         /*                                       */

/*                          
             
                             */
#define DMA21_NEXT_DESC_PTR            0xFFC09000         /*                                          */
#define DMA21_START_ADDR             0xFFC09004         /*                                       */
#define DMA21_CONFIG                   0xFFC09008         /*                              */
#define DMA21_X_COUNT                  0xFFC0900C         /*                                    */
#define DMA21_X_MODIFY                  0xFFC09010         /*                                    */
#define DMA21_Y_COUNT                  0xFFC09014         /*                                              */
#define DMA21_Y_MODIFY                  0xFFC09018         /*                                              */
#define DMA21_CURR_DESC_PTR            0xFFC09024         /*                                  */
#define DMA21_PREV_DESC_PTR            0xFFC09028         /*                                           */
#define DMA21_CURR_ADDR              0xFFC0902C         /*                       */
#define DMA21_IRQ_STATUS                  0xFFC09030         /*                       */
#define DMA21_CURR_X_COUNT              0xFFC09034         /*                                                */
#define DMA21_CURR_Y_COUNT              0xFFC09038         /*                                   */
#define DMA21_BWL_COUNT                0xFFC09040         /*                             */
#define DMA21_CURR_BWL_COUNT            0xFFC09044         /*                                     */
#define DMA21_BWM_COUNT                0xFFC09048         /*                               */
#define DMA21_CURR_BWM_COUNT            0xFFC0904C         /*                                       */

/*                          
             
                             */
#define DMA22_NEXT_DESC_PTR            0xFFC09080         /*                                          */
#define DMA22_START_ADDR             0xFFC09084         /*                                       */
#define DMA22_CONFIG                   0xFFC09088         /*                              */
#define DMA22_X_COUNT                  0xFFC0908C         /*                                    */
#define DMA22_X_MODIFY                  0xFFC09090         /*                                    */
#define DMA22_Y_COUNT                  0xFFC09094         /*                                              */
#define DMA22_Y_MODIFY                  0xFFC09098         /*                                              */
#define DMA22_CURR_DESC_PTR            0xFFC090A4         /*                                  */
#define DMA22_PREV_DESC_PTR            0xFFC090A8         /*                                           */
#define DMA22_CURR_ADDR              0xFFC090AC         /*                       */
#define DMA22_IRQ_STATUS                  0xFFC090B0         /*                       */
#define DMA22_CURR_X_COUNT              0xFFC090B4         /*                                                */
#define DMA22_CURR_Y_COUNT              0xFFC090B8         /*                                   */
#define DMA22_BWL_COUNT                0xFFC090C0         /*                             */
#define DMA22_CURR_BWL_COUNT            0xFFC090C4         /*                                     */
#define DMA22_BWM_COUNT                0xFFC090C8         /*                               */
#define DMA22_CURR_BWM_COUNT            0xFFC090CC         /*                                       */

/*                          
             
                             */
#define DMA23_NEXT_DESC_PTR            0xFFC09100         /*                                          */
#define DMA23_START_ADDR             0xFFC09104         /*                                       */
#define DMA23_CONFIG                   0xFFC09108         /*                              */
#define DMA23_X_COUNT                  0xFFC0910C         /*                                    */
#define DMA23_X_MODIFY                  0xFFC09110         /*                                    */
#define DMA23_Y_COUNT                  0xFFC09114         /*                                              */
#define DMA23_Y_MODIFY                  0xFFC09118         /*                                              */
#define DMA23_CURR_DESC_PTR            0xFFC09124         /*                                  */
#define DMA23_PREV_DESC_PTR            0xFFC09128         /*                                           */
#define DMA23_CURR_ADDR              0xFFC0912C         /*                       */
#define DMA23_IRQ_STATUS                  0xFFC09130         /*                       */
#define DMA23_CURR_X_COUNT              0xFFC09134         /*                                                */
#define DMA23_CURR_Y_COUNT              0xFFC09138         /*                                   */
#define DMA23_BWL_COUNT                0xFFC09140         /*                             */
#define DMA23_CURR_BWL_COUNT            0xFFC09144         /*                                     */
#define DMA23_BWM_COUNT                0xFFC09148         /*                               */
#define DMA23_CURR_BWM_COUNT            0xFFC0914C         /*                                       */

/*                          
             
                             */
#define DMA24_NEXT_DESC_PTR            0xFFC09180         /*                                          */
#define DMA24_START_ADDR             0xFFC09184         /*                                       */
#define DMA24_CONFIG                   0xFFC09188         /*                              */
#define DMA24_X_COUNT                  0xFFC0918C         /*                                    */
#define DMA24_X_MODIFY                  0xFFC09190         /*                                    */
#define DMA24_Y_COUNT                  0xFFC09194         /*                                              */
#define DMA24_Y_MODIFY                  0xFFC09198         /*                                              */
#define DMA24_CURR_DESC_PTR            0xFFC091A4         /*                                  */
#define DMA24_PREV_DESC_PTR            0xFFC091A8         /*                                           */
#define DMA24_CURR_ADDR              0xFFC091AC         /*                       */
#define DMA24_IRQ_STATUS                  0xFFC091B0         /*                       */
#define DMA24_CURR_X_COUNT              0xFFC091B4         /*                                                */
#define DMA24_CURR_Y_COUNT              0xFFC091B8         /*                                   */
#define DMA24_BWL_COUNT                0xFFC091C0         /*                             */
#define DMA24_CURR_BWL_COUNT            0xFFC091C4         /*                                     */
#define DMA24_BWM_COUNT                0xFFC091C8         /*                               */
#define DMA24_CURR_BWM_COUNT            0xFFC091CC         /*                                       */

/*                          
             
                             */
#define DMA25_NEXT_DESC_PTR            0xFFC09200         /*                                          */
#define DMA25_START_ADDR             0xFFC09204         /*                                       */
#define DMA25_CONFIG                   0xFFC09208         /*                              */
#define DMA25_X_COUNT                  0xFFC0920C         /*                                    */
#define DMA25_X_MODIFY                  0xFFC09210         /*                                    */
#define DMA25_Y_COUNT                  0xFFC09214         /*                                              */
#define DMA25_Y_MODIFY                  0xFFC09218         /*                                              */
#define DMA25_CURR_DESC_PTR            0xFFC09224         /*                                  */
#define DMA25_PREV_DESC_PTR            0xFFC09228         /*                                           */
#define DMA25_CURR_ADDR              0xFFC0922C         /*                       */
#define DMA25_IRQ_STATUS                  0xFFC09230         /*                       */
#define DMA25_CURR_X_COUNT              0xFFC09234         /*                                                */
#define DMA25_CURR_Y_COUNT              0xFFC09238         /*                                   */
#define DMA25_BWL_COUNT                0xFFC09240         /*                             */
#define DMA25_CURR_BWL_COUNT            0xFFC09244         /*                                     */
#define DMA25_BWM_COUNT                0xFFC09248         /*                               */
#define DMA25_CURR_BWM_COUNT            0xFFC0924C         /*                                       */

/*                          
             
                             */
#define DMA26_NEXT_DESC_PTR            0xFFC09280         /*                                          */
#define DMA26_START_ADDR             0xFFC09284         /*                                       */
#define DMA26_CONFIG                   0xFFC09288         /*                              */
#define DMA26_X_COUNT                  0xFFC0928C         /*                                    */
#define DMA26_X_MODIFY                  0xFFC09290         /*                                    */
#define DMA26_Y_COUNT                  0xFFC09294         /*                                              */
#define DMA26_Y_MODIFY                  0xFFC09298         /*                                              */
#define DMA26_CURR_DESC_PTR            0xFFC092A4         /*                                  */
#define DMA26_PREV_DESC_PTR            0xFFC092A8         /*                                           */
#define DMA26_CURR_ADDR              0xFFC092AC         /*                       */
#define DMA26_IRQ_STATUS                  0xFFC092B0         /*                       */
#define DMA26_CURR_X_COUNT              0xFFC092B4         /*                                                */
#define DMA26_CURR_Y_COUNT              0xFFC092B8         /*                                   */
#define DMA26_BWL_COUNT                0xFFC092C0         /*                             */
#define DMA26_CURR_BWL_COUNT            0xFFC092C4         /*                                     */
#define DMA26_BWM_COUNT                0xFFC092C8         /*                               */
#define DMA26_CURR_BWM_COUNT            0xFFC092CC         /*                                       */

/*                          
             
                             */
#define DMA27_NEXT_DESC_PTR            0xFFC09300         /*                                          */
#define DMA27_START_ADDR             0xFFC09304         /*                                       */
#define DMA27_CONFIG                   0xFFC09308         /*                              */
#define DMA27_X_COUNT                  0xFFC0930C         /*                                    */
#define DMA27_X_MODIFY                  0xFFC09310         /*                                    */
#define DMA27_Y_COUNT                  0xFFC09314         /*                                              */
#define DMA27_Y_MODIFY                  0xFFC09318         /*                                              */
#define DMA27_CURR_DESC_PTR            0xFFC09324         /*                                  */
#define DMA27_PREV_DESC_PTR            0xFFC09328         /*                                           */
#define DMA27_CURR_ADDR              0xFFC0932C         /*                       */
#define DMA27_IRQ_STATUS                  0xFFC09330         /*                       */
#define DMA27_CURR_X_COUNT              0xFFC09334         /*                                                */
#define DMA27_CURR_Y_COUNT              0xFFC09338         /*                                   */
#define DMA27_BWL_COUNT                0xFFC09340         /*                             */
#define DMA27_CURR_BWL_COUNT            0xFFC09344         /*                                     */
#define DMA27_BWM_COUNT                0xFFC09348         /*                               */
#define DMA27_CURR_BWM_COUNT            0xFFC0934C         /*                                       */

/*                          
             
                             */
#define DMA28_NEXT_DESC_PTR            0xFFC09380         /*                                          */
#define DMA28_START_ADDR             0xFFC09384         /*                                       */
#define DMA28_CONFIG                   0xFFC09388         /*                              */
#define DMA28_X_COUNT                  0xFFC0938C         /*                                    */
#define DMA28_X_MODIFY                  0xFFC09390         /*                                    */
#define DMA28_Y_COUNT                  0xFFC09394         /*                                              */
#define DMA28_Y_MODIFY                  0xFFC09398         /*                                              */
#define DMA28_CURR_DESC_PTR            0xFFC093A4         /*                                  */
#define DMA28_PREV_DESC_PTR            0xFFC093A8         /*                                           */
#define DMA28_CURR_ADDR              0xFFC093AC         /*                       */
#define DMA28_IRQ_STATUS                  0xFFC093B0         /*                       */
#define DMA28_CURR_X_COUNT              0xFFC093B4         /*                                                */
#define DMA28_CURR_Y_COUNT              0xFFC093B8         /*                                   */
#define DMA28_BWL_COUNT                0xFFC093C0         /*                             */
#define DMA28_CURR_BWL_COUNT            0xFFC093C4         /*                                     */
#define DMA28_BWM_COUNT                0xFFC093C8         /*                               */
#define DMA28_CURR_BWM_COUNT            0xFFC093CC         /*                                       */

/*                          
             
                             */
#define DMA29_NEXT_DESC_PTR            0xFFC0B000         /*                                          */
#define DMA29_START_ADDR             0xFFC0B004         /*                                       */
#define DMA29_CONFIG                   0xFFC0B008         /*                              */
#define DMA29_X_COUNT                  0xFFC0B00C         /*                                    */
#define DMA29_X_MODIFY                  0xFFC0B010         /*                                    */
#define DMA29_Y_COUNT                  0xFFC0B014         /*                                              */
#define DMA29_Y_MODIFY                  0xFFC0B018         /*                                              */
#define DMA29_CURR_DESC_PTR            0xFFC0B024         /*                                  */
#define DMA29_PREV_DESC_PTR            0xFFC0B028         /*                                           */
#define DMA29_CURR_ADDR              0xFFC0B02C         /*                       */
#define DMA29_IRQ_STATUS                  0xFFC0B030         /*                       */
#define DMA29_CURR_X_COUNT              0xFFC0B034         /*                                                */
#define DMA29_CURR_Y_COUNT              0xFFC0B038         /*                                   */
#define DMA29_BWL_COUNT                0xFFC0B040         /*                             */
#define DMA29_CURR_BWL_COUNT            0xFFC0B044         /*                                     */
#define DMA29_BWM_COUNT                0xFFC0B048         /*                               */
#define DMA29_CURR_BWM_COUNT            0xFFC0B04C         /*                                       */

/*                          
             
                             */
#define DMA30_NEXT_DESC_PTR            0xFFC0B080         /*                                          */
#define DMA30_START_ADDR             0xFFC0B084         /*                                       */
#define DMA30_CONFIG                   0xFFC0B088         /*                              */
#define DMA30_X_COUNT                  0xFFC0B08C         /*                                    */
#define DMA30_X_MODIFY                  0xFFC0B090         /*                                    */
#define DMA30_Y_COUNT                  0xFFC0B094         /*                                              */
#define DMA30_Y_MODIFY                  0xFFC0B098         /*                                              */
#define DMA30_CURR_DESC_PTR            0xFFC0B0A4         /*                                  */
#define DMA30_PREV_DESC_PTR            0xFFC0B0A8         /*                                           */
#define DMA30_CURR_ADDR              0xFFC0B0AC         /*                       */
#define DMA30_IRQ_STATUS                  0xFFC0B0B0         /*                       */
#define DMA30_CURR_X_COUNT              0xFFC0B0B4         /*                                                */
#define DMA30_CURR_Y_COUNT              0xFFC0B0B8         /*                                   */
#define DMA30_BWL_COUNT                0xFFC0B0C0         /*                             */
#define DMA30_CURR_BWL_COUNT            0xFFC0B0C4         /*                                     */
#define DMA30_BWM_COUNT                0xFFC0B0C8         /*                               */
#define DMA30_CURR_BWM_COUNT            0xFFC0B0CC         /*                                       */

/*                          
             
                             */
#define DMA31_NEXT_DESC_PTR            0xFFC0B100         /*                                          */
#define DMA31_START_ADDR             0xFFC0B104         /*                                       */
#define DMA31_CONFIG                   0xFFC0B108         /*                              */
#define DMA31_X_COUNT                  0xFFC0B10C         /*                                    */
#define DMA31_X_MODIFY                  0xFFC0B110         /*                                    */
#define DMA31_Y_COUNT                  0xFFC0B114         /*                                              */
#define DMA31_Y_MODIFY                  0xFFC0B118         /*                                              */
#define DMA31_CURR_DESC_PTR            0xFFC0B124         /*                                  */
#define DMA31_PREV_DESC_PTR            0xFFC0B128         /*                                           */
#define DMA31_CURR_ADDR              0xFFC0B12C         /*                       */
#define DMA31_IRQ_STATUS                  0xFFC0B130         /*                       */
#define DMA31_CURR_X_COUNT              0xFFC0B134         /*                                                */
#define DMA31_CURR_Y_COUNT              0xFFC0B138         /*                                   */
#define DMA31_BWL_COUNT                0xFFC0B140         /*                             */
#define DMA31_CURR_BWL_COUNT            0xFFC0B144         /*                                     */
#define DMA31_BWM_COUNT                0xFFC0B148         /*                               */
#define DMA31_CURR_BWM_COUNT            0xFFC0B14C         /*                                       */

/*                          
             
                             */
#define DMA32_NEXT_DESC_PTR            0xFFC0B180         /*                                          */
#define DMA32_START_ADDR             0xFFC0B184         /*                                       */
#define DMA32_CONFIG                   0xFFC0B188         /*                              */
#define DMA32_X_COUNT                  0xFFC0B18C         /*                                    */
#define DMA32_X_MODIFY                  0xFFC0B190         /*                                    */
#define DMA32_Y_COUNT                  0xFFC0B194         /*                                              */
#define DMA32_Y_MODIFY                  0xFFC0B198         /*                                              */
#define DMA32_CURR_DESC_PTR            0xFFC0B1A4         /*                                  */
#define DMA32_PREV_DESC_PTR            0xFFC0B1A8         /*                                           */
#define DMA32_CURR_ADDR              0xFFC0B1AC         /*                       */
#define DMA32_IRQ_STATUS                  0xFFC0B1B0         /*                       */
#define DMA32_CURR_X_COUNT              0xFFC0B1B4         /*                                                */
#define DMA32_CURR_Y_COUNT              0xFFC0B1B8         /*                                   */
#define DMA32_BWL_COUNT                0xFFC0B1C0         /*                             */
#define DMA32_CURR_BWL_COUNT            0xFFC0B1C4         /*                                     */
#define DMA32_BWM_COUNT                0xFFC0B1C8         /*                               */
#define DMA32_CURR_BWM_COUNT            0xFFC0B1CC         /*                                       */

/*                          
             
                             */
#define DMA33_NEXT_DESC_PTR            0xFFC0D000         /*                                          */
#define DMA33_START_ADDR             0xFFC0D004         /*                                       */
#define DMA33_CONFIG                   0xFFC0D008         /*                              */
#define DMA33_X_COUNT                  0xFFC0D00C         /*                                    */
#define DMA33_X_MODIFY                  0xFFC0D010         /*                                    */
#define DMA33_Y_COUNT                  0xFFC0D014         /*                                              */
#define DMA33_Y_MODIFY                  0xFFC0D018         /*                                              */
#define DMA33_CURR_DESC_PTR            0xFFC0D024         /*                                  */
#define DMA33_PREV_DESC_PTR            0xFFC0D028         /*                                           */
#define DMA33_CURR_ADDR              0xFFC0D02C         /*                       */
#define DMA33_IRQ_STATUS                  0xFFC0D030         /*                       */
#define DMA33_CURR_X_COUNT              0xFFC0D034         /*                                                */
#define DMA33_CURR_Y_COUNT              0xFFC0D038         /*                                   */
#define DMA33_BWL_COUNT                0xFFC0D040         /*                             */
#define DMA33_CURR_BWL_COUNT            0xFFC0D044         /*                                     */
#define DMA33_BWM_COUNT                0xFFC0D048         /*                               */
#define DMA33_CURR_BWM_COUNT            0xFFC0D04C         /*                                       */

/*                          
             
                             */
#define DMA34_NEXT_DESC_PTR            0xFFC0D080         /*                                          */
#define DMA34_START_ADDR             0xFFC0D084         /*                                       */
#define DMA34_CONFIG                   0xFFC0D088         /*                              */
#define DMA34_X_COUNT                  0xFFC0D08C         /*                                    */
#define DMA34_X_MODIFY                  0xFFC0D090         /*                                    */
#define DMA34_Y_COUNT                  0xFFC0D094         /*                                              */
#define DMA34_Y_MODIFY                  0xFFC0D098         /*                                              */
#define DMA34_CURR_DESC_PTR            0xFFC0D0A4         /*                                  */
#define DMA34_PREV_DESC_PTR            0xFFC0D0A8         /*                                           */
#define DMA34_CURR_ADDR              0xFFC0D0AC         /*                       */
#define DMA34_IRQ_STATUS                  0xFFC0D0B0         /*                       */
#define DMA34_CURR_X_COUNT              0xFFC0D0B4         /*                                                */
#define DMA34_CURR_Y_COUNT              0xFFC0D0B8         /*                                   */
#define DMA34_BWL_COUNT                0xFFC0D0C0         /*                             */
#define DMA34_CURR_BWL_COUNT            0xFFC0D0C4         /*                                     */
#define DMA34_BWM_COUNT                0xFFC0D0C8         /*                               */
#define DMA34_CURR_BWM_COUNT            0xFFC0D0CC         /*                                       */

/*                          
             
                             */
#define DMA35_NEXT_DESC_PTR            0xFFC10000         /*                                          */
#define DMA35_START_ADDR             0xFFC10004         /*                                       */
#define DMA35_CONFIG                   0xFFC10008         /*                              */
#define DMA35_X_COUNT                  0xFFC1000C         /*                                    */
#define DMA35_X_MODIFY                  0xFFC10010         /*                                    */
#define DMA35_Y_COUNT                  0xFFC10014         /*                                              */
#define DMA35_Y_MODIFY                  0xFFC10018         /*                                              */
#define DMA35_CURR_DESC_PTR            0xFFC10024         /*                                  */
#define DMA35_PREV_DESC_PTR            0xFFC10028         /*                                           */
#define DMA35_CURR_ADDR              0xFFC1002C         /*                       */
#define DMA35_IRQ_STATUS                  0xFFC10030         /*                       */
#define DMA35_CURR_X_COUNT              0xFFC10034         /*                                                */
#define DMA35_CURR_Y_COUNT              0xFFC10038         /*                                   */
#define DMA35_BWL_COUNT                0xFFC10040         /*                             */
#define DMA35_CURR_BWL_COUNT            0xFFC10044         /*                                     */
#define DMA35_BWM_COUNT                0xFFC10048         /*                               */
#define DMA35_CURR_BWM_COUNT            0xFFC1004C         /*                                       */

/*                          
             
                             */
#define DMA36_NEXT_DESC_PTR            0xFFC10080         /*                                          */
#define DMA36_START_ADDR             0xFFC10084         /*                                       */
#define DMA36_CONFIG                   0xFFC10088         /*                              */
#define DMA36_X_COUNT                  0xFFC1008C         /*                                    */
#define DMA36_X_MODIFY                  0xFFC10090         /*                                    */
#define DMA36_Y_COUNT                  0xFFC10094         /*                                              */
#define DMA36_Y_MODIFY                  0xFFC10098         /*                                              */
#define DMA36_CURR_DESC_PTR            0xFFC100A4         /*                                  */
#define DMA36_PREV_DESC_PTR            0xFFC100A8         /*                                           */
#define DMA36_CURR_ADDR              0xFFC100AC         /*                       */
#define DMA36_IRQ_STATUS                  0xFFC100B0         /*                       */
#define DMA36_CURR_X_COUNT              0xFFC100B4         /*                                                */
#define DMA36_CURR_Y_COUNT              0xFFC100B8         /*                                   */
#define DMA36_BWL_COUNT                0xFFC100C0         /*                             */
#define DMA36_CURR_BWL_COUNT            0xFFC100C4         /*                                     */
#define DMA36_BWM_COUNT                0xFFC100C8         /*                               */
#define DMA36_CURR_BWM_COUNT            0xFFC100CC         /*                                       */

/*                          
             
                             */
#define DMA37_NEXT_DESC_PTR            0xFFC10100         /*                                          */
#define DMA37_START_ADDR             0xFFC10104         /*                                       */
#define DMA37_CONFIG                   0xFFC10108         /*                              */
#define DMA37_X_COUNT                  0xFFC1010C         /*                                    */
#define DMA37_X_MODIFY                  0xFFC10110         /*                                    */
#define DMA37_Y_COUNT                  0xFFC10114         /*                                              */
#define DMA37_Y_MODIFY                  0xFFC10118         /*                                              */
#define DMA37_CURR_DESC_PTR            0xFFC10124         /*                                  */
#define DMA37_PREV_DESC_PTR            0xFFC10128         /*                                           */
#define DMA37_CURR_ADDR              0xFFC1012C         /*                       */
#define DMA37_IRQ_STATUS                  0xFFC10130         /*                       */
#define DMA37_CURR_X_COUNT              0xFFC10134         /*                                                */
#define DMA37_CURR_Y_COUNT              0xFFC10138         /*                                   */
#define DMA37_BWL_COUNT                0xFFC10140         /*                             */
#define DMA37_CURR_BWL_COUNT            0xFFC10144         /*                                     */
#define DMA37_BWM_COUNT                0xFFC10148         /*                               */
#define DMA37_CURR_BWM_COUNT            0xFFC1014C         /*                                       */

/*                          
             
                             */
#define DMA38_NEXT_DESC_PTR            0xFFC12000         /*                                          */
#define DMA38_START_ADDR             0xFFC12004         /*                                       */
#define DMA38_CONFIG                   0xFFC12008         /*                              */
#define DMA38_X_COUNT                  0xFFC1200C         /*                                    */
#define DMA38_X_MODIFY                  0xFFC12010         /*                                    */
#define DMA38_Y_COUNT                  0xFFC12014         /*                                              */
#define DMA38_Y_MODIFY                  0xFFC12018         /*                                              */
#define DMA38_CURR_DESC_PTR            0xFFC12024         /*                                  */
#define DMA38_PREV_DESC_PTR            0xFFC12028         /*                                           */
#define DMA38_CURR_ADDR              0xFFC1202C         /*                       */
#define DMA38_IRQ_STATUS                  0xFFC12030         /*                       */
#define DMA38_CURR_X_COUNT              0xFFC12034         /*                                                */
#define DMA38_CURR_Y_COUNT              0xFFC12038         /*                                   */
#define DMA38_BWL_COUNT                0xFFC12040         /*                             */
#define DMA38_CURR_BWL_COUNT            0xFFC12044         /*                                     */
#define DMA38_BWM_COUNT                0xFFC12048         /*                               */
#define DMA38_CURR_BWM_COUNT            0xFFC1204C         /*                                       */

/*                          
             
                             */
#define DMA39_NEXT_DESC_PTR            0xFFC12080         /*                                          */
#define DMA39_START_ADDR             0xFFC12084         /*                                       */
#define DMA39_CONFIG                   0xFFC12088         /*                              */
#define DMA39_X_COUNT                  0xFFC1208C         /*                                    */
#define DMA39_X_MODIFY                  0xFFC12090         /*                                    */
#define DMA39_Y_COUNT                  0xFFC12094         /*                                              */
#define DMA39_Y_MODIFY                  0xFFC12098         /*                                              */
#define DMA39_CURR_DESC_PTR            0xFFC120A4         /*                                  */
#define DMA39_PREV_DESC_PTR            0xFFC120A8         /*                                           */
#define DMA39_CURR_ADDR              0xFFC120AC         /*                       */
#define DMA39_IRQ_STATUS                  0xFFC120B0         /*                       */
#define DMA39_CURR_X_COUNT              0xFFC120B4         /*                                                */
#define DMA39_CURR_Y_COUNT              0xFFC120B8         /*                                   */
#define DMA39_BWL_COUNT                0xFFC120C0         /*                             */
#define DMA39_CURR_BWL_COUNT            0xFFC120C4         /*                                     */
#define DMA39_BWM_COUNT                0xFFC120C8         /*                               */
#define DMA39_CURR_BWM_COUNT            0xFFC120CC         /*                                       */

/*                          
             
                             */
#define DMA40_NEXT_DESC_PTR            0xFFC12100         /*                                          */
#define DMA40_START_ADDR             0xFFC12104         /*                                       */
#define DMA40_CONFIG                   0xFFC12108         /*                              */
#define DMA40_X_COUNT                  0xFFC1210C         /*                                    */
#define DMA40_X_MODIFY                  0xFFC12110         /*                                    */
#define DMA40_Y_COUNT                  0xFFC12114         /*                                              */
#define DMA40_Y_MODIFY                  0xFFC12118         /*                                              */
#define DMA40_CURR_DESC_PTR            0xFFC12124         /*                                  */
#define DMA40_PREV_DESC_PTR            0xFFC12128         /*                                           */
#define DMA40_CURR_ADDR              0xFFC1212C         /*                       */
#define DMA40_IRQ_STATUS                  0xFFC12130         /*                       */
#define DMA40_CURR_X_COUNT              0xFFC12134         /*                                                */
#define DMA40_CURR_Y_COUNT              0xFFC12138         /*                                   */
#define DMA40_BWL_COUNT                0xFFC12140         /*                             */
#define DMA40_CURR_BWL_COUNT            0xFFC12144         /*                                     */
#define DMA40_BWM_COUNT                0xFFC12148         /*                               */
#define DMA40_CURR_BWM_COUNT            0xFFC1214C         /*                                       */

/*                          
             
                             */
#define DMA41_NEXT_DESC_PTR            0xFFC12180         /*                                          */
#define DMA41_START_ADDR             0xFFC12184         /*                                       */
#define DMA41_CONFIG                   0xFFC12188         /*                              */
#define DMA41_X_COUNT                  0xFFC1218C         /*                                    */
#define DMA41_X_MODIFY                  0xFFC12190         /*                                    */
#define DMA41_Y_COUNT                  0xFFC12194         /*                                              */
#define DMA41_Y_MODIFY                  0xFFC12198         /*                                              */
#define DMA41_CURR_DESC_PTR            0xFFC121A4         /*                                  */
#define DMA41_PREV_DESC_PTR            0xFFC121A8         /*                                           */
#define DMA41_CURR_ADDR              0xFFC121AC         /*                       */
#define DMA41_IRQ_STATUS                  0xFFC121B0         /*                       */
#define DMA41_CURR_X_COUNT              0xFFC121B4         /*                                                */
#define DMA41_CURR_Y_COUNT              0xFFC121B8         /*                                   */
#define DMA41_BWL_COUNT                0xFFC121C0         /*                             */
#define DMA41_CURR_BWL_COUNT            0xFFC121C4         /*                                     */
#define DMA41_BWM_COUNT                0xFFC121C8         /*                               */
#define DMA41_CURR_BWM_COUNT            0xFFC121CC         /*                                       */

/*                          
             
                             */
#define DMA42_NEXT_DESC_PTR            0xFFC14000         /*                                          */
#define DMA42_START_ADDR             0xFFC14004         /*                                       */
#define DMA42_CONFIG                   0xFFC14008         /*                              */
#define DMA42_X_COUNT                  0xFFC1400C         /*                                    */
#define DMA42_X_MODIFY                  0xFFC14010         /*                                    */
#define DMA42_Y_COUNT                  0xFFC14014         /*                                              */
#define DMA42_Y_MODIFY                  0xFFC14018         /*                                              */
#define DMA42_CURR_DESC_PTR            0xFFC14024         /*                                  */
#define DMA42_PREV_DESC_PTR            0xFFC14028         /*                                           */
#define DMA42_CURR_ADDR              0xFFC1402C         /*                       */
#define DMA42_IRQ_STATUS                  0xFFC14030         /*                       */
#define DMA42_CURR_X_COUNT              0xFFC14034         /*                                                */
#define DMA42_CURR_Y_COUNT              0xFFC14038         /*                                   */
#define DMA42_BWL_COUNT                0xFFC14040         /*                             */
#define DMA42_CURR_BWL_COUNT            0xFFC14044         /*                                     */
#define DMA42_BWM_COUNT                0xFFC14048         /*                               */
#define DMA42_CURR_BWM_COUNT            0xFFC1404C         /*                                       */

/*                          
             
                             */
#define DMA43_NEXT_DESC_PTR            0xFFC14080         /*                                          */
#define DMA43_START_ADDR             0xFFC14084         /*                                       */
#define DMA43_CONFIG                   0xFFC14088         /*                              */
#define DMA43_X_COUNT                  0xFFC1408C         /*                                    */
#define DMA43_X_MODIFY                  0xFFC14090         /*                                    */
#define DMA43_Y_COUNT                  0xFFC14094         /*                                              */
#define DMA43_Y_MODIFY                  0xFFC14098         /*                                              */
#define DMA43_CURR_DESC_PTR            0xFFC140A4         /*                                  */
#define DMA43_PREV_DESC_PTR            0xFFC140A8         /*                                           */
#define DMA43_CURR_ADDR              0xFFC140AC         /*                       */
#define DMA43_IRQ_STATUS                  0xFFC140B0         /*                       */
#define DMA43_CURR_X_COUNT              0xFFC140B4         /*                                                */
#define DMA43_CURR_Y_COUNT              0xFFC140B8         /*                                   */
#define DMA43_BWL_COUNT                0xFFC140C0         /*                             */
#define DMA43_CURR_BWL_COUNT            0xFFC140C4         /*                                     */
#define DMA43_BWM_COUNT                0xFFC140C8         /*                               */
#define DMA43_CURR_BWM_COUNT            0xFFC140CC         /*                                       */

/*                          
             
                             */
#define DMA44_NEXT_DESC_PTR            0xFFC14100         /*                                          */
#define DMA44_START_ADDR             0xFFC14104         /*                                       */
#define DMA44_CONFIG                   0xFFC14108         /*                              */
#define DMA44_X_COUNT                  0xFFC1410C         /*                                    */
#define DMA44_X_MODIFY                  0xFFC14110         /*                                    */
#define DMA44_Y_COUNT                  0xFFC14114         /*                                              */
#define DMA44_Y_MODIFY                  0xFFC14118         /*                                              */
#define DMA44_CURR_DESC_PTR            0xFFC14124         /*                                  */
#define DMA44_PREV_DESC_PTR            0xFFC14128         /*                                           */
#define DMA44_CURR_ADDR              0xFFC1412C         /*                       */
#define DMA44_IRQ_STATUS                  0xFFC14130         /*                       */
#define DMA44_CURR_X_COUNT              0xFFC14134         /*                                                */
#define DMA44_CURR_Y_COUNT              0xFFC14138         /*                                   */
#define DMA44_BWL_COUNT                0xFFC14140         /*                             */
#define DMA44_CURR_BWL_COUNT            0xFFC14144         /*                                     */
#define DMA44_BWM_COUNT                0xFFC14148         /*                               */
#define DMA44_CURR_BWM_COUNT            0xFFC1414C         /*                                       */

/*                          
             
                             */
#define DMA45_NEXT_DESC_PTR            0xFFC14180         /*                                          */
#define DMA45_START_ADDR             0xFFC14184         /*                                       */
#define DMA45_CONFIG                   0xFFC14188         /*                              */
#define DMA45_X_COUNT                  0xFFC1418C         /*                                    */
#define DMA45_X_MODIFY                  0xFFC14190         /*                                    */
#define DMA45_Y_COUNT                  0xFFC14194         /*                                              */
#define DMA45_Y_MODIFY                  0xFFC14198         /*                                              */
#define DMA45_CURR_DESC_PTR            0xFFC141A4         /*                                  */
#define DMA45_PREV_DESC_PTR            0xFFC141A8         /*                                           */
#define DMA45_CURR_ADDR              0xFFC141AC         /*                       */
#define DMA45_IRQ_STATUS                  0xFFC141B0         /*                       */
#define DMA45_CURR_X_COUNT              0xFFC141B4         /*                                                */
#define DMA45_CURR_Y_COUNT              0xFFC141B8         /*                                   */
#define DMA45_BWL_COUNT                0xFFC141C0         /*                             */
#define DMA45_CURR_BWL_COUNT            0xFFC141C4         /*                                     */
#define DMA45_BWM_COUNT                0xFFC141C8         /*                               */
#define DMA45_CURR_BWM_COUNT            0xFFC141CC         /*                                       */

/*                          
             
                             */
#define DMA46_NEXT_DESC_PTR            0xFFC14200         /*                                          */
#define DMA46_START_ADDR             0xFFC14204         /*                                       */
#define DMA46_CONFIG                   0xFFC14208         /*                              */
#define DMA46_X_COUNT                  0xFFC1420C         /*                                    */
#define DMA46_X_MODIFY                  0xFFC14210         /*                                    */
#define DMA46_Y_COUNT                  0xFFC14214         /*                                              */
#define DMA46_Y_MODIFY                  0xFFC14218         /*                                              */
#define DMA46_CURR_DESC_PTR            0xFFC14224         /*                                  */
#define DMA46_PREV_DESC_PTR            0xFFC14228         /*                                           */
#define DMA46_CURR_ADDR              0xFFC1422C         /*                       */
#define DMA46_IRQ_STATUS                  0xFFC14230         /*                       */
#define DMA46_CURR_X_COUNT              0xFFC14234         /*                                                */
#define DMA46_CURR_Y_COUNT              0xFFC14238         /*                                   */
#define DMA46_BWL_COUNT                0xFFC14240         /*                             */
#define DMA46_CURR_BWL_COUNT            0xFFC14244         /*                                     */
#define DMA46_BWM_COUNT                0xFFC14248         /*                               */
#define DMA46_CURR_BWM_COUNT            0xFFC1424C         /*                                       */


/*                                                                               
                         
                                                                                */
#define MDMA0_DEST_CRC0_NEXT_DESC_PTR   (DMA22_NEXT_DESC_PTR)
#define MDMA0_DEST_CRC0_START_ADDR    (DMA22_START_ADDR)
#define MDMA0_DEST_CRC0_CONFIG          (DMA22_CONFIG)
#define MDMA0_DEST_CRC0_X_COUNT         (DMA22_X_COUNT)
#define MDMA0_DEST_CRC0_X_MODIFY         (DMA22_X_MODIFY)
#define MDMA0_DEST_CRC0_Y_COUNT         (DMA22_Y_COUNT)
#define MDMA0_DEST_CRC0_Y_MODIFY         (DMA22_Y_MODIFY)
#define MDMA0_DEST_CRC0_CURR_DESC_PTR   (DMA22_CURR_DESC_PTR)
#define MDMA0_DEST_CRC0_PREV_DESC_PTR   (DMA22_PREV_DESC_PTR)
#define MDMA0_DEST_CRC0_CURR_ADDR     (DMA22_CURR_ADDR)
#define MDMA0_DEST_CRC0_IRQ_STATUS         (DMA22_IRQ_STATUS)
#define MDMA0_DEST_CRC0_CURR_X_COUNT     (DMA22_CURR_X_COUNT)
#define MDMA0_DEST_CRC0_CURR_Y_COUNT     (DMA22_CURR_Y_COUNT)
#define MDMA0_DEST_CRC0_BWL_COUNT       (DMA22_BWL_COUNT)
#define MDMA0_DEST_CRC0_CURR_BWL_COUNT   (DMA22_CURR_BWL_COUNT)
#define MDMA0_DEST_CRC0_BWM_COUNT       (DMA22_BWM_COUNT)
#define MDMA0_DEST_CRC0_CURR_BWM_COUNT   (DMA22_CURR_BWM_COUNT)
#define MDMA0_SRC_CRC0_NEXT_DESC_PTR    (DMA21_NEXT_DESC_PTR)
#define MDMA0_SRC_CRC0_START_ADDR     (DMA21_START_ADDR)
#define MDMA0_SRC_CRC0_CONFIG           (DMA21_CONFIG)
#define MDMA0_SRC_CRC0_X_COUNT          (DMA21_X_COUNT)
#define MDMA0_SRC_CRC0_X_MODIFY          (DMA21_X_MODIFY)
#define MDMA0_SRC_CRC0_Y_COUNT          (DMA21_Y_COUNT)
#define MDMA0_SRC_CRC0_Y_MODIFY          (DMA21_Y_MODIFY)
#define MDMA0_SRC_CRC0_CURR_DESC_PTR    (DMA21_CURR_DESC_PTR)
#define MDMA0_SRC_CRC0_PREV_DESC_PTR    (DMA21_PREV_DESC_PTR)
#define MDMA0_SRC_CRC0_CURR_ADDR      (DMA21_CURR_ADDR)
#define MDMA0_SRC_CRC0_IRQ_STATUS          (DMA21_IRQ_STATUS)
#define MDMA0_SRC_CRC0_CURR_X_COUNT      (DMA21_CURR_X_COUNT)
#define MDMA0_SRC_CRC0_CURR_Y_COUNT      (DMA21_CURR_Y_COUNT)
#define MDMA0_SRC_CRC0_BWL_COUNT        (DMA21_BWL_COUNT)
#define MDMA0_SRC_CRC0_CURR_BWL_COUNT    (DMA21_CURR_BWL_COUNT)
#define MDMA0_SRC_CRC0_BWM_COUNT        (DMA21_BWM_COUNT)
#define MDMA0_SRC_CRC0_CURR_BWM_COUNT    (DMA21_CURR_BWM_COUNT)
#define MDMA1_DEST_CRC1_NEXT_DESC_PTR   (DMA24_NEXT_DESC_PTR)
#define MDMA1_DEST_CRC1_START_ADDR    (DMA24_START_ADDR)
#define MDMA1_DEST_CRC1_CONFIG          (DMA24_CONFIG)
#define MDMA1_DEST_CRC1_X_COUNT         (DMA24_X_COUNT)
#define MDMA1_DEST_CRC1_X_MODIFY         (DMA24_X_MODIFY)
#define MDMA1_DEST_CRC1_Y_COUNT         (DMA24_Y_COUNT)
#define MDMA1_DEST_CRC1_Y_MODIFY         (DMA24_Y_MODIFY)
#define MDMA1_DEST_CRC1_CURR_DESC_PTR   (DMA24_CURR_DESC_PTR)
#define MDMA1_DEST_CRC1_PREV_DESC_PTR   (DMA24_PREV_DESC_PTR)
#define MDMA1_DEST_CRC1_CURR_ADDR     (DMA24_CURR_ADDR)
#define MDMA1_DEST_CRC1_IRQ_STATUS         (DMA24_IRQ_STATUS)
#define MDMA1_DEST_CRC1_CURR_X_COUNT     (DMA24_CURR_X_COUNT)
#define MDMA1_DEST_CRC1_CURR_Y_COUNT     (DMA24_CURR_Y_COUNT)
#define MDMA1_DEST_CRC1_BWL_COUNT       (DMA24_BWL_COUNT)
#define MDMA1_DEST_CRC1_CURR_BWL_COUNT   (DMA24_CURR_BWL_COUNT)
#define MDMA1_DEST_CRC1_BWM_COUNT       (DMA24_BWM_COUNT)
#define MDMA1_DEST_CRC1_CURR_BWM_COUNT   (DMA24_CURR_BWM_COUNT)
#define MDMA1_SRC_CRC1_NEXT_DESC_PTR    (DMA23_NEXT_DESC_PTR)
#define MDMA1_SRC_CRC1_START_ADDR     (DMA23_START_ADDR)
#define MDMA1_SRC_CRC1_CONFIG           (DMA23_CONFIG)
#define MDMA1_SRC_CRC1_X_COUNT          (DMA23_X_COUNT)
#define MDMA1_SRC_CRC1_X_MODIFY          (DMA23_X_MODIFY)
#define MDMA1_SRC_CRC1_Y_COUNT          (DMA23_Y_COUNT)
#define MDMA1_SRC_CRC1_Y_MODIFY          (DMA23_Y_MODIFY)
#define MDMA1_SRC_CRC1_CURR_DESC_PTR    (DMA23_CURR_DESC_PTR)
#define MDMA1_SRC_CRC1_PREV_DESC_PTR    (DMA23_PREV_DESC_PTR)
#define MDMA1_SRC_CRC1_CURR_ADDR      (DMA23_CURR_ADDR)
#define MDMA1_SRC_CRC1_IRQ_STATUS          (DMA23_IRQ_STATUS)
#define MDMA1_SRC_CRC1_CURR_X_COUNT      (DMA23_CURR_X_COUNT)
#define MDMA1_SRC_CRC1_CURR_Y_COUNT      (DMA23_CURR_Y_COUNT)
#define MDMA1_SRC_CRC1_BWL_COUNT        (DMA23_BWL_COUNT)
#define MDMA1_SRC_CRC1_CURR_BWL_COUNT    (DMA23_CURR_BWL_COUNT)
#define MDMA1_SRC_CRC1_BWM_COUNT        (DMA23_BWM_COUNT)
#define MDMA1_SRC_CRC1_CURR_BWM_COUNT    (DMA23_CURR_BWM_COUNT)
#define MDMA2_DEST_NEXT_DESC_PTR            (DMA26_NEXT_DESC_PTR)
#define MDMA2_DEST_START_ADDR             (DMA26_START_ADDR)
#define MDMA2_DEST_CONFIG                   (DMA26_CONFIG)
#define MDMA2_DEST_X_COUNT                  (DMA26_X_COUNT)
#define MDMA2_DEST_X_MODIFY                  (DMA26_X_MODIFY)
#define MDMA2_DEST_Y_COUNT                  (DMA26_Y_COUNT)
#define MDMA2_DEST_Y_MODIFY                  (DMA26_Y_MODIFY)
#define MDMA2_DEST_CURR_DESC_PTR            (DMA26_CURR_DESC_PTR)
#define MDMA2_DEST_PREV_DESC_PTR            (DMA26_PREV_DESC_PTR)
#define MDMA2_DEST_CURR_ADDR              (DMA26_CURR_ADDR)
#define MDMA2_DEST_IRQ_STATUS                  (DMA26_IRQ_STATUS)
#define MDMA2_DEST_CURR_X_COUNT              (DMA26_CURR_X_COUNT)
#define MDMA2_DEST_CURR_Y_COUNT              (DMA26_CURR_Y_COUNT)
#define MDMA2_DEST_BWL_COUNT                (DMA26_BWL_COUNT)
#define MDMA2_DEST_CURR_BWL_COUNT            (DMA26_CURR_BWL_COUNT)
#define MDMA2_DEST_BWM_COUNT                (DMA26_BWM_COUNT)
#define MDMA2_DEST_CURR_BWM_COUNT            (DMA26_CURR_BWM_COUNT)
#define MDMA2_SRC_NEXT_DESC_PTR            (DMA25_NEXT_DESC_PTR)
#define MDMA2_SRC_START_ADDR             (DMA25_START_ADDR)
#define MDMA2_SRC_CONFIG                   (DMA25_CONFIG)
#define MDMA2_SRC_X_COUNT                  (DMA25_X_COUNT)
#define MDMA2_SRC_X_MODIFY                  (DMA25_X_MODIFY)
#define MDMA2_SRC_Y_COUNT                  (DMA25_Y_COUNT)
#define MDMA2_SRC_Y_MODIFY                  (DMA25_Y_MODIFY)
#define MDMA2_SRC_CURR_DESC_PTR            (DMA25_CURR_DESC_PTR)
#define MDMA2_SRC_PREV_DESC_PTR            (DMA25_PREV_DESC_PTR)
#define MDMA2_SRC_CURR_ADDR              (DMA25_CURR_ADDR)
#define MDMA2_SRC_IRQ_STATUS                  (DMA25_IRQ_STATUS)
#define MDMA2_SRC_CURR_X_COUNT              (DMA25_CURR_X_COUNT)
#define MDMA2_SRC_CURR_Y_COUNT              (DMA25_CURR_Y_COUNT)
#define MDMA2_SRC_BWL_COUNT                (DMA25_BWL_COUNT)
#define MDMA2_SRC_CURR_BWL_COUNT            (DMA25_CURR_BWL_COUNT)
#define MDMA2_SRC_BWM_COUNT                (DMA25_BWM_COUNT)
#define MDMA2_SRC_CURR_BWM_COUNT            (DMA25_CURR_BWM_COUNT)
#define MDMA3_DEST_NEXT_DESC_PTR            (DMA28_NEXT_DESC_PTR)
#define MDMA3_DEST_START_ADDR             (DMA28_START_ADDR)
#define MDMA3_DEST_CONFIG                   (DMA28_CONFIG)
#define MDMA3_DEST_X_COUNT                  (DMA28_X_COUNT)
#define MDMA3_DEST_X_MODIFY                  (DMA28_X_MODIFY)
#define MDMA3_DEST_Y_COUNT                  (DMA28_Y_COUNT)
#define MDMA3_DEST_Y_MODIFY                  (DMA28_Y_MODIFY)
#define MDMA3_DEST_CURR_DESC_PTR            (DMA28_CURR_DESC_PTR)
#define MDMA3_DEST_PREV_DESC_PTR            (DMA28_PREV_DESC_PTR)
#define MDMA3_DEST_CURR_ADDR              (DMA28_CURR_ADDR)
#define MDMA3_DEST_IRQ_STATUS                  (DMA28_IRQ_STATUS)
#define MDMA3_DEST_CURR_X_COUNT              (DMA28_CURR_X_COUNT)
#define MDMA3_DEST_CURR_Y_COUNT              (DMA28_CURR_Y_COUNT)
#define MDMA3_DEST_BWL_COUNT                (DMA28_BWL_COUNT)
#define MDMA3_DEST_CURR_BWL_COUNT            (DMA28_CURR_BWL_COUNT)
#define MDMA3_DEST_BWM_COUNT                (DMA28_BWM_COUNT)
#define MDMA3_DEST_CURR_BWM_COUNT            (DMA28_CURR_BWM_COUNT)
#define MDMA3_SRC_NEXT_DESC_PTR            (DMA27_NEXT_DESC_PTR)
#define MDMA3_SRC_START_ADDR             (DMA27_START_ADDR)
#define MDMA3_SRC_CONFIG                   (DMA27_CONFIG)
#define MDMA3_SRC_X_COUNT                  (DMA27_X_COUNT)
#define MDMA3_SRC_X_MODIFY                  (DMA27_X_MODIFY)
#define MDMA3_SRC_Y_COUNT                  (DMA27_Y_COUNT)
#define MDMA3_SRC_Y_MODIFY                  (DMA27_Y_MODIFY)
#define MDMA3_SRC_CURR_DESC_PTR            (DMA27_CURR_DESC_PTR)
#define MDMA3_SRC_PREV_DESC_PTR            (DMA27_PREV_DESC_PTR)
#define MDMA3_SRC_CURR_ADDR              (DMA27_CURR_ADDR)
#define MDMA3_SRC_IRQ_STATUS                  (DMA27_IRQ_STATUS)
#define MDMA3_SRC_CURR_X_COUNT              (DMA27_CURR_X_COUNT)
#define MDMA3_SRC_CURR_Y_COUNT              (DMA27_CURR_Y_COUNT)
#define MDMA3_SRC_BWL_COUNT                (DMA27_BWL_COUNT)
#define MDMA3_SRC_CURR_BWL_COUNT            (DMA27_CURR_BWL_COUNT)
#define MDMA3_SRC_BWM_COUNT                (DMA27_BWM_COUNT)
#define MDMA3_SRC_CURR_BWM_COUNT            (DMA27_CURR_BWM_COUNT)


/*                          
                     
                             */

/*                          
            
                             */
#define DMC0_ID                     0xFFC80000         /*                              */
#define DMC0_CTL                    0xFFC80004         /*                       */
#define DMC0_STAT                   0xFFC80008         /*                      */
#define DMC0_EFFCTL                 0xFFC8000C         /*                            */
#define DMC0_PRIO                   0xFFC80010         /*                           */
#define DMC0_PRIOMSK                0xFFC80014         /*                       */
#define DMC0_CFG                    0xFFC80040         /*                          */
#define DMC0_TR0                    0xFFC80044         /*                        */
#define DMC0_TR1                    0xFFC80048         /*                        */
#define DMC0_TR2                    0xFFC8004C         /*                        */
#define DMC0_MSK                    0xFFC8005C         /*                         */
#define DMC0_MR                     0xFFC80060         /*                           */
#define DMC0_EMR1                   0xFFC80064         /*                           */
#define DMC0_EMR2                   0xFFC80068         /*                           */
#define DMC0_EMR3                   0xFFC8006C         /*                           */
#define DMC0_DLLCTL                 0xFFC80080         /*                           */
#define DMC0_PADCTL                 0xFFC800C0         /*                             */

#define DEVSZ_64                0x000         /*                                 */
#define DEVSZ_128               0x100         /*                                  */
#define DEVSZ_256               0x200         /*                                  */
#define DEVSZ_512               0x300         /*                                  */
#define DEVSZ_1G                0x400         /*                                */
#define DEVSZ_2G                0x500         /*                                */

/*                          
                       
                             */

/*                          
              
                             */
#define L2CTL0_CTL                  0xFFCA3000         /*                            */
#define L2CTL0_ACTL_C0              0xFFCA3004         /*                                          */
#define L2CTL0_ACTL_C1              0xFFCA3008         /*                                          */
#define L2CTL0_ACTL_SYS             0xFFCA300C         /*                                          */
#define L2CTL0_STAT                 0xFFCA3010         /*                           */
#define L2CTL0_RPCR                 0xFFCA3014         /*                                        */
#define L2CTL0_WPCR                 0xFFCA3018         /*                                         */
#define L2CTL0_RFA                  0xFFCA3024         /*                                    */
#define L2CTL0_ERRADDR0             0xFFCA3040         /*                                             */
#define L2CTL0_ERRADDR1             0xFFCA3044         /*                                             */
#define L2CTL0_ERRADDR2             0xFFCA3048         /*                                             */
#define L2CTL0_ERRADDR3             0xFFCA304C         /*                                             */
#define L2CTL0_ERRADDR4             0xFFCA3050         /*                                             */
#define L2CTL0_ERRADDR5             0xFFCA3054         /*                                             */
#define L2CTL0_ERRADDR6             0xFFCA3058         /*                                             */
#define L2CTL0_ERRADDR7             0xFFCA305C         /*                                             */
#define L2CTL0_ET0                  0xFFCA3080         /*                                     */
#define L2CTL0_EADDR0               0xFFCA3084         /*                                        */
#define L2CTL0_ET1                  0xFFCA3088         /*                                     */
#define L2CTL0_EADDR1               0xFFCA308C         /*                                        */


/*                          
                     
                             */
/*                                                                                                                         
                                                    
                                                                                                                            */

#define SEC_SCI_BASE 0xFFCA4400
#define SEC_SCI_OFF 0x40
#define SEC_CCTL 0x0         /*                             */
#define SEC_CSTAT 0x4         /*                            */
#define SEC_CPND 0x8         /*                                 */
#define SEC_CACT 0xC         /*                                */
#define SEC_CPMSK 0x10         /*                                       */
#define SEC_CGMSK 0x14         /*                                    */
#define SEC_CPLVL 0x18         /*                                        */
#define SEC_CSID 0x1C         /*                                   */

#define bfin_read_SEC_SCI(n, reg) bfin_read32(SEC_SCI_BASE + (n) * SEC_SCI_OFF + reg)
#define bfin_write_SEC_SCI(n, reg, val) \
	bfin_write32(SEC_SCI_BASE + (n) * SEC_SCI_OFF + reg, val)

/*                                                                                                                         
                                                                
                                                                                                                            */
#define SEC_FCTL                   0xFFCA4010         /*                            */
#define SEC_FSTAT                  0xFFCA4014         /*                           */
#define SEC_FSID                   0xFFCA4018         /*                              */
#define SEC_FEND                   0xFFCA401C         /*                        */
#define SEC_FDLY                   0xFFCA4020         /*                          */
#define SEC_FDLY_CUR               0xFFCA4024         /*                                  */
#define SEC_FSRDLY                 0xFFCA4028         /*                                       */
#define SEC_FSRDLY_CUR             0xFFCA402C         /*                                               */
#define SEC_FCOPP                  0xFFCA4030         /*                               */
#define SEC_FCOPP_CUR              0xFFCA4034         /*                                       */

/*                                                                                                                         
                                      
                                                                                                                            */
#define SEC_GCTL                   0xFFCA4000         /*                             */
#define SEC_GSTAT                  0xFFCA4004         /*                            */
#define SEC_RAISE                  0xFFCA4008         /*                           */
#define SEC_END                    0xFFCA400C         /*                         */

/*                                                                                                                         
                                                      
                                                                                                                            */
#define SEC_SCTL0                  0xFFCA4800         /*                                   */
#define SEC_SCTL1                  0xFFCA4808         /*                                   */
#define SEC_SCTL2                  0xFFCA4810         /*                                   */
#define SEC_SCTL3                  0xFFCA4818         /*                                   */
#define SEC_SCTL4                  0xFFCA4820         /*                                   */
#define SEC_SCTL5                  0xFFCA4828         /*                                   */
#define SEC_SCTL6                  0xFFCA4830         /*                                   */
#define SEC_SCTL7                  0xFFCA4838         /*                                   */
#define SEC_SCTL8                  0xFFCA4840         /*                                   */
#define SEC_SCTL9                  0xFFCA4848         /*                                   */
#define SEC_SCTL10                 0xFFCA4850         /*                                   */
#define SEC_SCTL11                 0xFFCA4858         /*                                   */
#define SEC_SCTL12                 0xFFCA4860         /*                                   */
#define SEC_SCTL13                 0xFFCA4868         /*                                   */
#define SEC_SCTL14                 0xFFCA4870         /*                                   */
#define SEC_SCTL15                 0xFFCA4878         /*                                   */
#define SEC_SCTL16                 0xFFCA4880         /*                                   */
#define SEC_SCTL17                 0xFFCA4888         /*                                   */
#define SEC_SCTL18                 0xFFCA4890         /*                                   */
#define SEC_SCTL19                 0xFFCA4898         /*                                   */
#define SEC_SCTL20                 0xFFCA48A0         /*                                   */
#define SEC_SCTL21                 0xFFCA48A8         /*                                   */
#define SEC_SCTL22                 0xFFCA48B0         /*                                   */
#define SEC_SCTL23                 0xFFCA48B8         /*                                   */
#define SEC_SCTL24                 0xFFCA48C0         /*                                   */
#define SEC_SCTL25                 0xFFCA48C8         /*                                   */
#define SEC_SCTL26                 0xFFCA48D0         /*                                   */
#define SEC_SCTL27                 0xFFCA48D8         /*                                   */
#define SEC_SCTL28                 0xFFCA48E0         /*                                   */
#define SEC_SCTL29                 0xFFCA48E8         /*                                   */
#define SEC_SCTL30                 0xFFCA48F0         /*                                   */
#define SEC_SCTL31                 0xFFCA48F8         /*                                   */
#define SEC_SCTL32                 0xFFCA4900         /*                                   */
#define SEC_SCTL33                 0xFFCA4908         /*                                   */
#define SEC_SCTL34                 0xFFCA4910         /*                                   */
#define SEC_SCTL35                 0xFFCA4918         /*                                   */
#define SEC_SCTL36                 0xFFCA4920         /*                                   */
#define SEC_SCTL37                 0xFFCA4928         /*                                   */
#define SEC_SCTL38                 0xFFCA4930         /*                                   */
#define SEC_SCTL39                 0xFFCA4938         /*                                   */
#define SEC_SCTL40                 0xFFCA4940         /*                                   */
#define SEC_SCTL41                 0xFFCA4948         /*                                   */
#define SEC_SCTL42                 0xFFCA4950         /*                                   */
#define SEC_SCTL43                 0xFFCA4958         /*                                   */
#define SEC_SCTL44                 0xFFCA4960         /*                                   */
#define SEC_SCTL45                 0xFFCA4968         /*                                   */
#define SEC_SCTL46                 0xFFCA4970         /*                                   */
#define SEC_SCTL47                 0xFFCA4978         /*                                   */
#define SEC_SCTL48                 0xFFCA4980         /*                                   */
#define SEC_SCTL49                 0xFFCA4988         /*                                   */
#define SEC_SCTL50                 0xFFCA4990         /*                                   */
#define SEC_SCTL51                 0xFFCA4998         /*                                   */
#define SEC_SCTL52                 0xFFCA49A0         /*                                   */
#define SEC_SCTL53                 0xFFCA49A8         /*                                   */
#define SEC_SCTL54                 0xFFCA49B0         /*                                   */
#define SEC_SCTL55                 0xFFCA49B8         /*                                   */
#define SEC_SCTL56                 0xFFCA49C0         /*                                   */
#define SEC_SCTL57                 0xFFCA49C8         /*                                   */
#define SEC_SCTL58                 0xFFCA49D0         /*                                   */
#define SEC_SCTL59                 0xFFCA49D8         /*                                   */
#define SEC_SCTL60                 0xFFCA49E0         /*                                   */
#define SEC_SCTL61                 0xFFCA49E8         /*                                   */
#define SEC_SCTL62                 0xFFCA49F0         /*                                   */
#define SEC_SCTL63                 0xFFCA49F8         /*                                   */
#define SEC_SCTL64                 0xFFCA4A00         /*                                   */
#define SEC_SCTL65                 0xFFCA4A08         /*                                   */
#define SEC_SCTL66                 0xFFCA4A10         /*                                   */
#define SEC_SCTL67                 0xFFCA4A18         /*                                   */
#define SEC_SCTL68                 0xFFCA4A20         /*                                   */
#define SEC_SCTL69                 0xFFCA4A28         /*                                   */
#define SEC_SCTL70                 0xFFCA4A30         /*                                   */
#define SEC_SCTL71                 0xFFCA4A38         /*                                   */
#define SEC_SCTL72                 0xFFCA4A40         /*                                   */
#define SEC_SCTL73                 0xFFCA4A48         /*                                   */
#define SEC_SCTL74                 0xFFCA4A50         /*                                   */
#define SEC_SCTL75                 0xFFCA4A58         /*                                   */
#define SEC_SCTL76                 0xFFCA4A60         /*                                   */
#define SEC_SCTL77                 0xFFCA4A68         /*                                   */
#define SEC_SCTL78                 0xFFCA4A70         /*                                   */
#define SEC_SCTL79                 0xFFCA4A78         /*                                   */
#define SEC_SCTL80                 0xFFCA4A80         /*                                   */
#define SEC_SCTL81                 0xFFCA4A88         /*                                   */
#define SEC_SCTL82                 0xFFCA4A90         /*                                   */
#define SEC_SCTL83                 0xFFCA4A98         /*                                   */
#define SEC_SCTL84                 0xFFCA4AA0         /*                                   */
#define SEC_SCTL85                 0xFFCA4AA8         /*                                   */
#define SEC_SCTL86                 0xFFCA4AB0         /*                                   */
#define SEC_SCTL87                 0xFFCA4AB8         /*                                   */
#define SEC_SCTL88                 0xFFCA4AC0         /*                                   */
#define SEC_SCTL89                 0xFFCA4AC8         /*                                   */
#define SEC_SCTL90                 0xFFCA4AD0         /*                                   */
#define SEC_SCTL91                 0xFFCA4AD8         /*                                   */
#define SEC_SCTL92                 0xFFCA4AE0         /*                                   */
#define SEC_SCTL93                 0xFFCA4AE8         /*                                   */
#define SEC_SCTL94                 0xFFCA4AF0         /*                                   */
#define SEC_SCTL95                 0xFFCA4AF8         /*                                   */
#define SEC_SCTL96                 0xFFCA4B00         /*                                   */
#define SEC_SCTL97                 0xFFCA4B08         /*                                   */
#define SEC_SCTL98                 0xFFCA4B10         /*                                   */
#define SEC_SCTL99                 0xFFCA4B18         /*                                   */
#define SEC_SCTL100                0xFFCA4B20         /*                                   */
#define SEC_SCTL101                0xFFCA4B28         /*                                   */
#define SEC_SCTL102                0xFFCA4B30         /*                                   */
#define SEC_SCTL103                0xFFCA4B38         /*                                   */
#define SEC_SCTL104                0xFFCA4B40         /*                                   */
#define SEC_SCTL105                0xFFCA4B48         /*                                   */
#define SEC_SCTL106                0xFFCA4B50         /*                                   */
#define SEC_SCTL107                0xFFCA4B58         /*                                   */
#define SEC_SCTL108                0xFFCA4B60         /*                                   */
#define SEC_SCTL109                0xFFCA4B68         /*                                   */
#define SEC_SCTL110                0xFFCA4B70         /*                                   */
#define SEC_SCTL111                0xFFCA4B78         /*                                   */
#define SEC_SCTL112                0xFFCA4B80         /*                                   */
#define SEC_SCTL113                0xFFCA4B88         /*                                   */
#define SEC_SCTL114                0xFFCA4B90         /*                                   */
#define SEC_SCTL115                0xFFCA4B98         /*                                   */
#define SEC_SCTL116                0xFFCA4BA0         /*                                   */
#define SEC_SCTL117                0xFFCA4BA8         /*                                   */
#define SEC_SCTL118                0xFFCA4BB0         /*                                   */
#define SEC_SCTL119                0xFFCA4BB8         /*                                   */
#define SEC_SCTL120                0xFFCA4BC0         /*                                   */
#define SEC_SCTL121                0xFFCA4BC8         /*                                   */
#define SEC_SCTL122                0xFFCA4BD0         /*                                   */
#define SEC_SCTL123                0xFFCA4BD8         /*                                   */
#define SEC_SCTL124                0xFFCA4BE0         /*                                   */
#define SEC_SCTL125                0xFFCA4BE8         /*                                   */
#define SEC_SCTL126                0xFFCA4BF0         /*                                   */
#define SEC_SCTL127                0xFFCA4BF8         /*                                   */
#define SEC_SCTL128                0xFFCA4C00         /*                                   */
#define SEC_SCTL129                0xFFCA4C08         /*                                   */
#define SEC_SCTL130                0xFFCA4C10         /*                                   */
#define SEC_SCTL131                0xFFCA4C18         /*                                   */
#define SEC_SCTL132                0xFFCA4C20         /*                                   */
#define SEC_SCTL133                0xFFCA4C28         /*                                   */
#define SEC_SCTL134                0xFFCA4C30         /*                                   */
#define SEC_SCTL135                0xFFCA4C38         /*                                   */
#define SEC_SCTL136                0xFFCA4C40         /*                                   */
#define SEC_SCTL137                0xFFCA4C48         /*                                   */
#define SEC_SCTL138                0xFFCA4C50         /*                                   */
#define SEC_SCTL139                0xFFCA4C58         /*                                   */
#define SEC_SSTAT0                 0xFFCA4804         /*                                  */
#define SEC_SSTAT1                 0xFFCA480C         /*                                  */
#define SEC_SSTAT2                 0xFFCA4814         /*                                  */
#define SEC_SSTAT3                 0xFFCA481C         /*                                  */
#define SEC_SSTAT4                 0xFFCA4824         /*                                  */
#define SEC_SSTAT5                 0xFFCA482C         /*                                  */
#define SEC_SSTAT6                 0xFFCA4834         /*                                  */
#define SEC_SSTAT7                 0xFFCA483C         /*                                  */
#define SEC_SSTAT8                 0xFFCA4844         /*                                  */
#define SEC_SSTAT9                 0xFFCA484C         /*                                  */
#define SEC_SSTAT10                0xFFCA4854         /*                                  */
#define SEC_SSTAT11                0xFFCA485C         /*                                  */
#define SEC_SSTAT12                0xFFCA4864         /*                                  */
#define SEC_SSTAT13                0xFFCA486C         /*                                  */
#define SEC_SSTAT14                0xFFCA4874         /*                                  */
#define SEC_SSTAT15                0xFFCA487C         /*                                  */
#define SEC_SSTAT16                0xFFCA4884         /*                                  */
#define SEC_SSTAT17                0xFFCA488C         /*                                  */
#define SEC_SSTAT18                0xFFCA4894         /*                                  */
#define SEC_SSTAT19                0xFFCA489C         /*                                  */
#define SEC_SSTAT20                0xFFCA48A4         /*                                  */
#define SEC_SSTAT21                0xFFCA48AC         /*                                  */
#define SEC_SSTAT22                0xFFCA48B4         /*                                  */
#define SEC_SSTAT23                0xFFCA48BC         /*                                  */
#define SEC_SSTAT24                0xFFCA48C4         /*                                  */
#define SEC_SSTAT25                0xFFCA48CC         /*                                  */
#define SEC_SSTAT26                0xFFCA48D4         /*                                  */
#define SEC_SSTAT27                0xFFCA48DC         /*                                  */
#define SEC_SSTAT28                0xFFCA48E4         /*                                  */
#define SEC_SSTAT29                0xFFCA48EC         /*                                  */
#define SEC_SSTAT30                0xFFCA48F4         /*                                  */
#define SEC_SSTAT31                0xFFCA48FC         /*                                  */
#define SEC_SSTAT32                0xFFCA4904         /*                                  */
#define SEC_SSTAT33                0xFFCA490C         /*                                  */
#define SEC_SSTAT34                0xFFCA4914         /*                                  */
#define SEC_SSTAT35                0xFFCA491C         /*                                  */
#define SEC_SSTAT36                0xFFCA4924         /*                                  */
#define SEC_SSTAT37                0xFFCA492C         /*                                  */
#define SEC_SSTAT38                0xFFCA4934         /*                                  */
#define SEC_SSTAT39                0xFFCA493C         /*                                  */
#define SEC_SSTAT40                0xFFCA4944         /*                                  */
#define SEC_SSTAT41                0xFFCA494C         /*                                  */
#define SEC_SSTAT42                0xFFCA4954         /*                                  */
#define SEC_SSTAT43                0xFFCA495C         /*                                  */
#define SEC_SSTAT44                0xFFCA4964         /*                                  */
#define SEC_SSTAT45                0xFFCA496C         /*                                  */
#define SEC_SSTAT46                0xFFCA4974         /*                                  */
#define SEC_SSTAT47                0xFFCA497C         /*                                  */
#define SEC_SSTAT48                0xFFCA4984         /*                                  */
#define SEC_SSTAT49                0xFFCA498C         /*                                  */
#define SEC_SSTAT50                0xFFCA4994         /*                                  */
#define SEC_SSTAT51                0xFFCA499C         /*                                  */
#define SEC_SSTAT52                0xFFCA49A4         /*                                  */
#define SEC_SSTAT53                0xFFCA49AC         /*                                  */
#define SEC_SSTAT54                0xFFCA49B4         /*                                  */
#define SEC_SSTAT55                0xFFCA49BC         /*                                  */
#define SEC_SSTAT56                0xFFCA49C4         /*                                  */
#define SEC_SSTAT57                0xFFCA49CC         /*                                  */
#define SEC_SSTAT58                0xFFCA49D4         /*                                  */
#define SEC_SSTAT59                0xFFCA49DC         /*                                  */
#define SEC_SSTAT60                0xFFCA49E4         /*                                  */
#define SEC_SSTAT61                0xFFCA49EC         /*                                  */
#define SEC_SSTAT62                0xFFCA49F4         /*                                  */
#define SEC_SSTAT63                0xFFCA49FC         /*                                  */
#define SEC_SSTAT64                0xFFCA4A04         /*                                  */
#define SEC_SSTAT65                0xFFCA4A0C         /*                                  */
#define SEC_SSTAT66                0xFFCA4A14         /*                                  */
#define SEC_SSTAT67                0xFFCA4A1C         /*                                  */
#define SEC_SSTAT68                0xFFCA4A24         /*                                  */
#define SEC_SSTAT69                0xFFCA4A2C         /*                                  */
#define SEC_SSTAT70                0xFFCA4A34         /*                                  */
#define SEC_SSTAT71                0xFFCA4A3C         /*                                  */
#define SEC_SSTAT72                0xFFCA4A44         /*                                  */
#define SEC_SSTAT73                0xFFCA4A4C         /*                                  */
#define SEC_SSTAT74                0xFFCA4A54         /*                                  */
#define SEC_SSTAT75                0xFFCA4A5C         /*                                  */
#define SEC_SSTAT76                0xFFCA4A64         /*                                  */
#define SEC_SSTAT77                0xFFCA4A6C         /*                                  */
#define SEC_SSTAT78                0xFFCA4A74         /*                                  */
#define SEC_SSTAT79                0xFFCA4A7C         /*                                  */
#define SEC_SSTAT80                0xFFCA4A84         /*                                  */
#define SEC_SSTAT81                0xFFCA4A8C         /*                                  */
#define SEC_SSTAT82                0xFFCA4A94         /*                                  */
#define SEC_SSTAT83                0xFFCA4A9C         /*                                  */
#define SEC_SSTAT84                0xFFCA4AA4         /*                                  */
#define SEC_SSTAT85                0xFFCA4AAC         /*                                  */
#define SEC_SSTAT86                0xFFCA4AB4         /*                                  */
#define SEC_SSTAT87                0xFFCA4ABC         /*                                  */
#define SEC_SSTAT88                0xFFCA4AC4         /*                                  */
#define SEC_SSTAT89                0xFFCA4ACC         /*                                  */
#define SEC_SSTAT90                0xFFCA4AD4         /*                                  */
#define SEC_SSTAT91                0xFFCA4ADC         /*                                  */
#define SEC_SSTAT92                0xFFCA4AE4         /*                                  */
#define SEC_SSTAT93                0xFFCA4AEC         /*                                  */
#define SEC_SSTAT94                0xFFCA4AF4         /*                                  */
#define SEC_SSTAT95                0xFFCA4AFC         /*                                  */
#define SEC_SSTAT96                0xFFCA4B04         /*                                  */
#define SEC_SSTAT97                0xFFCA4B0C         /*                                  */
#define SEC_SSTAT98                0xFFCA4B14         /*                                  */
#define SEC_SSTAT99                0xFFCA4B1C         /*                                  */
#define SEC_SSTAT100               0xFFCA4B24         /*                                  */
#define SEC_SSTAT101               0xFFCA4B2C         /*                                  */
#define SEC_SSTAT102               0xFFCA4B34         /*                                  */
#define SEC_SSTAT103               0xFFCA4B3C         /*                                  */
#define SEC_SSTAT104               0xFFCA4B44         /*                                  */
#define SEC_SSTAT105               0xFFCA4B4C         /*                                  */
#define SEC_SSTAT106               0xFFCA4B54         /*                                  */
#define SEC_SSTAT107               0xFFCA4B5C         /*                                  */
#define SEC_SSTAT108               0xFFCA4B64         /*                                  */
#define SEC_SSTAT109               0xFFCA4B6C         /*                                  */
#define SEC_SSTAT110               0xFFCA4B74         /*                                  */
#define SEC_SSTAT111               0xFFCA4B7C         /*                                  */
#define SEC_SSTAT112               0xFFCA4B84         /*                                  */
#define SEC_SSTAT113               0xFFCA4B8C         /*                                  */
#define SEC_SSTAT114               0xFFCA4B94         /*                                  */
#define SEC_SSTAT115               0xFFCA4B9C         /*                                  */
#define SEC_SSTAT116               0xFFCA4BA4         /*                                  */
#define SEC_SSTAT117               0xFFCA4BAC         /*                                  */
#define SEC_SSTAT118               0xFFCA4BB4         /*                                  */
#define SEC_SSTAT119               0xFFCA4BBC         /*                                  */
#define SEC_SSTAT120               0xFFCA4BC4         /*                                  */
#define SEC_SSTAT121               0xFFCA4BCC         /*                                  */
#define SEC_SSTAT122               0xFFCA4BD4         /*                                  */
#define SEC_SSTAT123               0xFFCA4BDC         /*                                  */
#define SEC_SSTAT124               0xFFCA4BE4         /*                                  */
#define SEC_SSTAT125               0xFFCA4BEC         /*                                  */
#define SEC_SSTAT126               0xFFCA4BF4         /*                                  */
#define SEC_SSTAT127               0xFFCA4BFC         /*                                  */
#define SEC_SSTAT128               0xFFCA4C04         /*                                  */
#define SEC_SSTAT129               0xFFCA4C0C         /*                                  */
#define SEC_SSTAT130               0xFFCA4C14         /*                                  */
#define SEC_SSTAT131               0xFFCA4C1C         /*                                  */
#define SEC_SSTAT132               0xFFCA4C24         /*                                  */
#define SEC_SSTAT133               0xFFCA4C2C         /*                                  */
#define SEC_SSTAT134               0xFFCA4C34         /*                                  */
#define SEC_SSTAT135               0xFFCA4C3C         /*                                  */
#define SEC_SSTAT136               0xFFCA4C44         /*                                  */
#define SEC_SSTAT137               0xFFCA4C4C         /*                                  */
#define SEC_SSTAT138               0xFFCA4C54         /*                                  */
#define SEC_SSTAT139               0xFFCA4C5C         /*                                  */

/*                                                                                                                         
                                                                      
                                                                                                                            */
#define SEC_CCTL_LOCK                   0x80000000    /*            */
#define SEC_CCTL_NMI_EN                 0x00010000    /*               */
#define SEC_CCTL_WAITIDLE               0x00001000    /*                    */
#define SEC_CCTL_RESET                  0x00000002    /*              */
#define SEC_CCTL_EN                     0x00000001    /*            */

/*                                                                                                                         
                                                                      
                                                                                                                            */
#define SEC_CSTAT_NMI                   0x00010000    /*            */
#define SEC_CSTAT_WAITING               0x00001000    /*              */
#define SEC_CSTAT_VALID_SID             0x00000400    /*             */
#define SEC_CSTAT_VALID_ACT             0x00000200    /*             */
#define SEC_CSTAT_VALID_PND             0x00000100    /*             */
#define SEC_CSTAT_ERRC                  0x00000030    /*             */
#define SEC_CSTAT_ACKERR                0x00000010    /*                         */
#define SEC_CSTAT_ERR                   0x00000002    /*                     */

/*                                                                                                                         
                                                                      
                                                                                                                            */
#define SEC_CPND_PRIO                   0x0000FF00    /*                              */
#define SEC_CPND_SID                    0x000000FF    /*                               */

/*                                                                                                                         
                                                                      
                                                                                                                            */
#define SEC_CACT_PRIO                   0x0000FF00    /*                             */
#define SEC_CACT_SID                    0x000000FF    /*                              */

/*                                                                                                                         
                                                                      
                                                                                                                            */
#define SEC_CPMSK_LOCK                  0x80000000    /*            */
#define SEC_CPMSK_PRIO                  0x000000FF    /*                   */

/*                                                                                                                         
                                                                      
                                                                                                                            */
#define SEC_CGMSK_LOCK                  0x80000000    /*            */
#define SEC_CGMSK_MASK                  0x00000100    /*                              */
#define SEC_CGMSK_GRP                   0x0000000F    /*              */

/*                                                                                                                         
                                                                      
                                                                                                                            */
#define SEC_CPLVL_LOCK                  0x80000000    /*            */
#define SEC_CPLVL_PLVL                  0x00000007    /*                 */

/*                                                                                                                         
                                                                      
                                                                                                                            */
#define SEC_CSID_SID                    0x000000FF    /*           */


/*                                                                                                                         
                                                                      
                                                                                                                            */
#define SEC_FCTL_LOCK                   0x80000000    /*            */
#define SEC_FCTL_FLTPND_MODE            0x00002000    /*                         */
#define SEC_FCTL_COP_MODE               0x00001000    /*               */
#define SEC_FCTL_FLTIN_EN               0x00000080    /*              */
#define SEC_FCTL_SYSRST_EN              0x00000040    /*              */
#define SEC_FCTL_TRGOUT_EN              0x00000020    /*              */
#define SEC_FCTL_FLTOUT_EN              0x00000010    /*              */
#define SEC_FCTL_RESET                  0x00000002    /*              */
#define SEC_FCTL_EN                     0x00000001    /*            */

/*                                                                                                                         
                                                                      
                                                                                                                            */
#define SEC_FSTAT_NXTFLT                0x00000400    /*               */
#define SEC_FSTAT_FLTACT                0x00000200    /*                   */
#define SEC_FSTAT_FLTPND                0x00000100    /*              */
#define SEC_FSTAT_ERRC                  0x00000030    /*             */
#define SEC_FSTAT_ENDERR                0x00000020    /*                 */
#define SEC_FSTAT_ERR                   0x00000002    /*                     */

/*                                                                                                                         
                                                                      
                                                                                                                            */
#define SEC_FSID_SRC_EXTFLT             0x00010000    /*                      */
#define SEC_FSID_SID                    0x000000FF    /*           */

/*                                                                                                                         
                                                                      
                                                                                                                            */
#define SEC_FEND_END_EXTFLT             0x00010000    /*                      */
#define SEC_FEND_SID                    0x000000FF    /*           */


/*                                                                                                                         
                                                                      
                                                                                                                            */
#define SEC_GCTL_LOCK                   0x80000000    /*      */
#define SEC_GCTL_RESET                  0x00000002    /*       */
#define SEC_GCTL_EN                     0x00000001    /*        */

/*                                                                                                                         
                                                                      
                                                                                                                            */
#define SEC_GSTAT_LWERR                 0x80000000    /*                       */
#define SEC_GSTAT_ADRERR                0x40000000    /*                        */
#define SEC_GSTAT_SID                   0x00FF0000    /*                         */
#define SEC_GSTAT_SCI                   0x00000F00    /*                      */
#define SEC_GSTAT_ERRC                  0x00000030    /*             */
#define SEC_GSTAT_SCIERR                0x00000010    /*                 */
#define SEC_GSTAT_SSIERR                0x00000020    /*                 */
#define SEC_GSTAT_ERR                   0x00000002    /*                     */

/*                                                                                                                         
                                                                      
                                                                                                                            */
#define SEC_RAISE_SID                   0x000000FF    /*                              */

/*                                                                                                                         
                                                                      
                                                                                                                            */
#define SEC_END_SID                     0x000000FF    /*                      */


/*                                                                                                                         
                                                                      
                                                                                                                            */
#define SEC_SCTL_LOCK                   0x80000000    /*      */
#define SEC_SCTL_CTG                    0x0F000000    /*                    */
#define SEC_SCTL_GRP                    0x000F0000    /*              */
#define SEC_SCTL_PRIO                   0x0000FF00    /*                       */
#define SEC_SCTL_ERR_EN                 0x00000010    /*               */
#define SEC_SCTL_EDGE                   0x00000008    /*                    */
#define SEC_SCTL_SRC_EN                 0x00000004    /*             */
#define SEC_SCTL_FAULT_EN               0x00000002    /*             */
#define SEC_SCTL_INT_EN                 0x00000001    /*             */

/*                                                                                                                         
                                                                      
                                                                                                                            */
#define SEC_SSTAT_CHID                  0x00FF0000    /*            */
#define SEC_SSTAT_ACTIVE_SRC            0x00000200    /*                    */
#define SEC_SSTAT_PENDING               0x00000100    /*              */
#define SEC_SSTAT_ERRC                  0x00000030    /*             */
#define SEC_SSTAT_ENDERR                0x00000020    /*                 */
#define SEC_SSTAT_ERR                   0x00000002    /*       */


/*                          
                     
                             */

/*                          
            
                             */
#define RCU0_CTL                    0xFFCA6000         /*                       */
#define RCU0_STAT                   0xFFCA6004         /*                      */
#define RCU0_CRCTL                  0xFFCA6008         /*                                  */
#define RCU0_CRSTAT                 0xFFCA600C         /*                                 */
#define RCU0_SIDIS                  0xFFCA6010         /*                                        */
#define RCU0_SISTAT                 0xFFCA6014         /*                                       */
#define RCU0_SVECT_LCK              0xFFCA6018         /*                          */
#define RCU0_BCODE                  0xFFCA601C         /*                         */
#define RCU0_SVECT0                 0xFFCA6020         /*                                 */
#define RCU0_SVECT1                 0xFFCA6024         /*                                 */


/*                          
            
                             */
#define CGU0_CTL                    0xFFCA8000         /*                       */
#define CGU0_STAT                   0xFFCA8004         /*                      */
#define CGU0_DIV                    0xFFCA8008         /*                       */
#define CGU0_CLKOUTSEL              0xFFCA800C         /*                             */


/*                          
                     
                             */

/*                          
            
                             */
#define DPM0_CTL                    0xFFCA9000         /*                       */
#define DPM0_STAT                   0xFFCA9004         /*                      */
#define DPM0_CCBF_DIS               0xFFCA9008         /*                                         */
#define DPM0_CCBF_EN                0xFFCA900C         /*                                        */
#define DPM0_CCBF_STAT              0xFFCA9010         /*                                        */
#define DPM0_CCBF_STAT_STKY         0xFFCA9014         /*                                               */
#define DPM0_SCBF_DIS               0xFFCA9018         /*                                           */
#define DPM0_WAKE_EN                0xFFCA901C         /*                             */
#define DPM0_WAKE_POL               0xFFCA9020         /*                               */
#define DPM0_WAKE_STAT              0xFFCA9024         /*                             */
#define DPM0_HIB_DIS                0xFFCA9028         /*                                 */
#define DPM0_PGCNTR                 0xFFCA902C         /*                                  */
#define DPM0_RESTORE0               0xFFCA9030         /*                       */
#define DPM0_RESTORE1               0xFFCA9034         /*                       */
#define DPM0_RESTORE2               0xFFCA9038         /*                       */
#define DPM0_RESTORE3               0xFFCA903C         /*                       */
#define DPM0_RESTORE4               0xFFCA9040         /*                       */
#define DPM0_RESTORE5               0xFFCA9044         /*                       */
#define DPM0_RESTORE6               0xFFCA9048         /*                       */
#define DPM0_RESTORE7               0xFFCA904C         /*                       */
#define DPM0_RESTORE8               0xFFCA9050         /*                       */
#define DPM0_RESTORE9               0xFFCA9054         /*                       */
#define DPM0_RESTORE10              0xFFCA9058         /*                       */
#define DPM0_RESTORE11              0xFFCA905C         /*                       */
#define DPM0_RESTORE12              0xFFCA9060         /*                       */
#define DPM0_RESTORE13              0xFFCA9064         /*                       */
#define DPM0_RESTORE14              0xFFCA9068         /*                       */
#define DPM0_RESTORE15              0xFFCA906C         /*                       */


/*                          
                     
                             */

/*              */
#define USB_FADDR                  0xFFCC1000         /*                                       */
#define USB_POWER                  0xFFCC1001         /*                              */
#define USB_INTRTX                 0xFFCC1002         /*                        */
#define USB_INTRRX                 0xFFCC1004         /*                        */
#define USB_INTRTXE                0xFFCC1006         /*                               */
#define USB_INTRRXE                0xFFCC1008         /*                              */
#define USB_INTRUSB                    0xFFCC100A         /*                    */
#define USB_INTRUSBE                    0xFFCC100B         /*                          */
#define USB_FRAME                  0xFFCC100C         /*                  */
#define USB_INDEX                  0xFFCC100E         /*           */
#define USB_TESTMODE               0xFFCC100F         /*               */
#define USB_EPI_TXMAXP0            0xFFCC1010         /*                                    */
#define USB_EP_NI0_TXMAXP          0xFFCC1010
#define USB_EP0I_CSR0_H            0xFFCC1012         /*                           */
#define USB_EPI_TXCSR0_H           0xFFCC1012         /*                                       */
#define USB_EP0I_CSR0_P            0xFFCC1012         /*                           */
#define USB_EPI_TXCSR0_P           0xFFCC1012         /*                                       */
#define USB_EPI_RXMAXP0            0xFFCC1014         /*                                   */
#define USB_EPI_RXCSR0_H           0xFFCC1016         /*                                               */
#define USB_EPI_RXCSR0_P           0xFFCC1016         /*                                               */
#define USB_EP0I_CNT0              0xFFCC1018         /*                                             */
#define USB_EPI_RXCNT0             0xFFCC1018         /*                             */
#define USB_EP0I_TYPE0             0xFFCC101A         /*                          */
#define USB_EPI_TXTYPE0            0xFFCC101A         /*                   */
#define USB_EP0I_NAKLIMIT0         0xFFCC101B         /*                                         */
#define USB_EPI_TXINTERVAL0        0xFFCC101B         /*                               */
#define USB_EPI_RXTYPE0            0xFFCC101C         /*                  */
#define USB_EPI_RXINTERVAL0        0xFFCC101D         /*                              */
#define USB_EP0I_CFGDATA0          0xFFCC101F         /*                               */
#define USB_FIFOB0                 0xFFCC1020         /*               */
#define USB_FIFOB1                 0xFFCC1024         /*               */
#define USB_FIFOB2                 0xFFCC1028         /*               */
#define USB_FIFOB3                 0xFFCC102C         /*               */
#define USB_FIFOB4                 0xFFCC1030         /*               */
#define USB_FIFOB5                 0xFFCC1034         /*               */
#define USB_FIFOB6                 0xFFCC1038         /*               */
#define USB_FIFOB7                 0xFFCC103C         /*               */
#define USB_FIFOB8                 0xFFCC1040         /*               */
#define USB_FIFOB9                 0xFFCC1044         /*               */
#define USB_FIFOB10                0xFFCC1048         /*               */
#define USB_FIFOB11                0xFFCC104C         /*               */
#define USB_FIFOH0                 0xFFCC1020         /*               */
#define USB_FIFOH1                 0xFFCC1024         /*               */
#define USB_FIFOH2                 0xFFCC1028         /*               */
#define USB_FIFOH3                 0xFFCC102C         /*               */
#define USB_FIFOH4                 0xFFCC1030         /*               */
#define USB_FIFOH5                 0xFFCC1034         /*               */
#define USB_FIFOH6                 0xFFCC1038         /*               */
#define USB_FIFOH7                 0xFFCC103C         /*               */
#define USB_FIFOH8                 0xFFCC1040         /*               */
#define USB_FIFOH9                 0xFFCC1044         /*               */
#define USB_FIFOH10                0xFFCC1048         /*               */
#define USB_FIFOH11                0xFFCC104C         /*               */
#define USB_FIFO0                  0xFFCC1020         /*               */
#define USB_EP0_FIFO               0xFFCC1020
#define USB_FIFO1                  0xFFCC1024         /*               */
#define USB_FIFO2                  0xFFCC1028         /*               */
#define USB_FIFO3                  0xFFCC102C         /*               */
#define USB_FIFO4                  0xFFCC1030         /*               */
#define USB_FIFO5                  0xFFCC1034         /*               */
#define USB_FIFO6                  0xFFCC1038         /*               */
#define USB_FIFO7                  0xFFCC103C         /*               */
#define USB_FIFO8                  0xFFCC1040         /*               */
#define USB_FIFO9                  0xFFCC1044         /*               */
#define USB_FIFO10                 0xFFCC1048         /*               */
#define USB_FIFO11                 0xFFCC104C         /*               */
#define USB_OTG_DEV_CTL                0xFFCC1060         /*                    */
#define USB_TXFIFOSZ               0xFFCC1062         /*                        */
#define USB_RXFIFOSZ               0xFFCC1063         /*                       */
#define USB_TXFIFOADDR             0xFFCC1064         /*                           */
#define USB_RXFIFOADDR             0xFFCC1066         /*                          */
#define USB_VENDSTAT               0xFFCC1068         /*                   */
#define USB_HWVERS                 0xFFCC106C         /*                      */
#define USB_EPINFO                 0xFFCC1078         /*                   */
#define USB_RAMINFO                0xFFCC1079         /*                     */
#define USB_LINKINFO               0xFFCC107A         /*                               */
#define USB_VPLEN                  0xFFCC107B         /*                         */
#define USB_HS_EOF1                0xFFCC107C         /*                                       */
#define USB_FS_EOF1                0xFFCC107D         /*                                       */
#define USB_LS_EOF1                0xFFCC107E         /*                                      */
#define USB_SOFT_RST               0xFFCC107F         /*                    */
#define USB_TXFUNCADDR0            0xFFCC1080         /*                               */
#define USB_TXFUNCADDR1            0xFFCC1088         /*                               */
#define USB_TXFUNCADDR2            0xFFCC1090         /*                               */
#define USB_TXFUNCADDR3            0xFFCC1098         /*                               */
#define USB_TXFUNCADDR4            0xFFCC10A0         /*                               */
#define USB_TXFUNCADDR5            0xFFCC10A8         /*                               */
#define USB_TXFUNCADDR6            0xFFCC10B0         /*                               */
#define USB_TXFUNCADDR7            0xFFCC10B8         /*                               */
#define USB_TXFUNCADDR8            0xFFCC10C0         /*                               */
#define USB_TXFUNCADDR9            0xFFCC10C8         /*                               */
#define USB_TXFUNCADDR10           0xFFCC10D0         /*                               */
#define USB_TXFUNCADDR11           0xFFCC10D8         /*                               */
#define USB_TXHUBADDR0             0xFFCC1082         /*                          */
#define USB_TXHUBADDR1             0xFFCC108A         /*                          */
#define USB_TXHUBADDR2             0xFFCC1092         /*                          */
#define USB_TXHUBADDR3             0xFFCC109A         /*                          */
#define USB_TXHUBADDR4             0xFFCC10A2         /*                          */
#define USB_TXHUBADDR5             0xFFCC10AA         /*                          */
#define USB_TXHUBADDR6             0xFFCC10B2         /*                          */
#define USB_TXHUBADDR7             0xFFCC10BA         /*                          */
#define USB_TXHUBADDR8             0xFFCC10C2         /*                          */
#define USB_TXHUBADDR9             0xFFCC10CA         /*                          */
#define USB_TXHUBADDR10            0xFFCC10D2         /*                          */
#define USB_TXHUBADDR11            0xFFCC10DA         /*                          */
#define USB_TXHUBPORT0             0xFFCC1083         /*                       */
#define USB_TXHUBPORT1             0xFFCC108B         /*                       */
#define USB_TXHUBPORT2             0xFFCC1093         /*                       */
#define USB_TXHUBPORT3             0xFFCC109B         /*                       */
#define USB_TXHUBPORT4             0xFFCC10A3         /*                       */
#define USB_TXHUBPORT5             0xFFCC10AB         /*                       */
#define USB_TXHUBPORT6             0xFFCC10B3         /*                       */
#define USB_TXHUBPORT7             0xFFCC10BB         /*                       */
#define USB_TXHUBPORT8             0xFFCC10C3         /*                       */
#define USB_TXHUBPORT9             0xFFCC10CB         /*                       */
#define USB_TXHUBPORT10            0xFFCC10D3         /*                       */
#define USB_TXHUBPORT11            0xFFCC10DB         /*                       */
#define USB_RXFUNCADDR0            0xFFCC1084         /*                              */
#define USB_RXFUNCADDR1            0xFFCC108C         /*                              */
#define USB_RXFUNCADDR2            0xFFCC1094         /*                              */
#define USB_RXFUNCADDR3            0xFFCC109C         /*                              */
#define USB_RXFUNCADDR4            0xFFCC10A4         /*                              */
#define USB_RXFUNCADDR5            0xFFCC10AC         /*                              */
#define USB_RXFUNCADDR6            0xFFCC10B4         /*                              */
#define USB_RXFUNCADDR7            0xFFCC10BC         /*                              */
#define USB_RXFUNCADDR8            0xFFCC10C4         /*                              */
#define USB_RXFUNCADDR9            0xFFCC10CC         /*                              */
#define USB_RXFUNCADDR10           0xFFCC10D4         /*                              */
#define USB_RXFUNCADDR11           0xFFCC10DC         /*                              */
#define USB_RXHUBADDR0             0xFFCC1086         /*                         */
#define USB_RXHUBADDR1             0xFFCC108E         /*                         */
#define USB_RXHUBADDR2             0xFFCC1096         /*                         */
#define USB_RXHUBADDR3             0xFFCC109E         /*                         */
#define USB_RXHUBADDR4             0xFFCC10A6         /*                         */
#define USB_RXHUBADDR5             0xFFCC10AE         /*                         */
#define USB_RXHUBADDR6             0xFFCC10B6         /*                         */
#define USB_RXHUBADDR7             0xFFCC10BE         /*                         */
#define USB_RXHUBADDR8             0xFFCC10C6         /*                         */
#define USB_RXHUBADDR9             0xFFCC10CE         /*                         */
#define USB_RXHUBADDR10            0xFFCC10D6         /*                         */
#define USB_RXHUBADDR11            0xFFCC10DE         /*                         */
#define USB_RXHUBPORT0             0xFFCC1087         /*                      */
#define USB_RXHUBPORT1             0xFFCC108F         /*                      */
#define USB_RXHUBPORT2             0xFFCC1097         /*                      */
#define USB_RXHUBPORT3             0xFFCC109F         /*                      */
#define USB_RXHUBPORT4             0xFFCC10A7         /*                      */
#define USB_RXHUBPORT5             0xFFCC10AF         /*                      */
#define USB_RXHUBPORT6             0xFFCC10B7         /*                      */
#define USB_RXHUBPORT7             0xFFCC10BF         /*                      */
#define USB_RXHUBPORT8             0xFFCC10C7         /*                      */
#define USB_RXHUBPORT9             0xFFCC10CF         /*                      */
#define USB_RXHUBPORT10            0xFFCC10D7         /*                      */
#define USB_RXHUBPORT11            0xFFCC10DF         /*                      */
#define USB_EP0_CSR0_H             0xFFCC1102         /*                           */
#define USB_EP0_CSR0_P             0xFFCC1102         /*                           */
#define USB_EP0_CNT0               0xFFCC1108         /*                                             */
#define USB_EP0_TYPE0              0xFFCC110A         /*                          */
#define USB_EP0_NAKLIMIT0          0xFFCC110B         /*                                         */
#define USB_EP0_CFGDATA0           0xFFCC110F         /*                               */
#define USB_EP_TXMAXP0             0xFFCC1110         /*                                    */
#define USB_EP_TXMAXP1             0xFFCC1120         /*                                    */
#define USB_EP_TXMAXP2             0xFFCC1130         /*                                    */
#define USB_EP_TXMAXP3             0xFFCC1140         /*                                    */
#define USB_EP_TXMAXP4             0xFFCC1150         /*                                    */
#define USB_EP_TXMAXP5             0xFFCC1160         /*                                    */
#define USB_EP_TXMAXP6             0xFFCC1170         /*                                    */
#define USB_EP_TXMAXP7             0xFFCC1180         /*                                    */
#define USB_EP_TXMAXP8             0xFFCC1190         /*                                    */
#define USB_EP_TXMAXP9             0xFFCC11A0         /*                                    */
#define USB_EP_TXMAXP10            0xFFCC11B0         /*                                    */
#define USB_EP_TXCSR0_H            0xFFCC1112         /*                                       */
#define USB_EP_TXCSR1_H            0xFFCC1122         /*                                       */
#define USB_EP_TXCSR2_H            0xFFCC1132         /*                                       */
#define USB_EP_TXCSR3_H            0xFFCC1142         /*                                       */
#define USB_EP_TXCSR4_H            0xFFCC1152         /*                                       */
#define USB_EP_TXCSR5_H            0xFFCC1162         /*                                       */
#define USB_EP_TXCSR6_H            0xFFCC1172         /*                                       */
#define USB_EP_TXCSR7_H            0xFFCC1182         /*                                       */
#define USB_EP_TXCSR8_H            0xFFCC1192         /*                                       */
#define USB_EP_TXCSR9_H            0xFFCC11A2         /*                                       */
#define USB_EP_TXCSR10_H           0xFFCC11B2         /*                                       */
#define USB_EP_TXCSR0_P            0xFFCC1112         /*                                       */
#define USB_EP_TXCSR1_P            0xFFCC1122         /*                                       */
#define USB_EP_TXCSR2_P            0xFFCC1132         /*                                       */
#define USB_EP_TXCSR3_P            0xFFCC1142         /*                                       */
#define USB_EP_TXCSR4_P            0xFFCC1152         /*                                       */
#define USB_EP_TXCSR5_P            0xFFCC1162         /*                                       */
#define USB_EP_TXCSR6_P            0xFFCC1172         /*                                       */
#define USB_EP_TXCSR7_P            0xFFCC1182         /*                                       */
#define USB_EP_TXCSR8_P            0xFFCC1192         /*                                       */
#define USB_EP_TXCSR9_P            0xFFCC11A2         /*                                       */
#define USB_EP_TXCSR10_P           0xFFCC11B2         /*                                       */
#define USB_EP_RXMAXP0             0xFFCC1114         /*                                   */
#define USB_EP_RXMAXP1             0xFFCC1124         /*                                   */
#define USB_EP_RXMAXP2             0xFFCC1134         /*                                   */
#define USB_EP_RXMAXP3             0xFFCC1144         /*                                   */
#define USB_EP_RXMAXP4             0xFFCC1154         /*                                   */
#define USB_EP_RXMAXP5             0xFFCC1164         /*                                   */
#define USB_EP_RXMAXP6             0xFFCC1174         /*                                   */
#define USB_EP_RXMAXP7             0xFFCC1184         /*                                   */
#define USB_EP_RXMAXP8             0xFFCC1194         /*                                   */
#define USB_EP_RXMAXP9             0xFFCC11A4         /*                                   */
#define USB_EP_RXMAXP10            0xFFCC11B4         /*                                   */
#define USB_EP_RXCSR0_H            0xFFCC1116         /*                                               */
#define USB_EP_RXCSR1_H            0xFFCC1126         /*                                               */
#define USB_EP_RXCSR2_H            0xFFCC1136         /*                                               */
#define USB_EP_RXCSR3_H            0xFFCC1146         /*                                               */
#define USB_EP_RXCSR4_H            0xFFCC1156         /*                                               */
#define USB_EP_RXCSR5_H            0xFFCC1166         /*                                               */
#define USB_EP_RXCSR6_H            0xFFCC1176         /*                                               */
#define USB_EP_RXCSR7_H            0xFFCC1186         /*                                               */
#define USB_EP_RXCSR8_H            0xFFCC1196         /*                                               */
#define USB_EP_RXCSR9_H            0xFFCC11A6         /*                                               */
#define USB_EP_RXCSR10_H           0xFFCC11B6         /*                                               */
#define USB_EP_RXCSR0_P            0xFFCC1116         /*                                               */
#define USB_EP_RXCSR1_P            0xFFCC1126         /*                                               */
#define USB_EP_RXCSR2_P            0xFFCC1136         /*                                               */
#define USB_EP_RXCSR3_P            0xFFCC1146         /*                                               */
#define USB_EP_RXCSR4_P            0xFFCC1156         /*                                               */
#define USB_EP_RXCSR5_P            0xFFCC1166         /*                                               */
#define USB_EP_RXCSR6_P            0xFFCC1176         /*                                               */
#define USB_EP_RXCSR7_P            0xFFCC1186         /*                                               */
#define USB_EP_RXCSR8_P            0xFFCC1196         /*                                               */
#define USB_EP_RXCSR9_P            0xFFCC11A6         /*                                               */
#define USB_EP_RXCSR10_P           0xFFCC11B6         /*                                               */
#define USB_EP_RXCNT0              0xFFCC1118         /*                             */
#define USB_EP_RXCNT1              0xFFCC1128         /*                             */
#define USB_EP_RXCNT2              0xFFCC1138         /*                             */
#define USB_EP_RXCNT3              0xFFCC1148         /*                             */
#define USB_EP_RXCNT4              0xFFCC1158         /*                             */
#define USB_EP_RXCNT5              0xFFCC1168         /*                             */
#define USB_EP_RXCNT6              0xFFCC1178         /*                             */
#define USB_EP_RXCNT7              0xFFCC1188         /*                             */
#define USB_EP_RXCNT8              0xFFCC1198         /*                             */
#define USB_EP_RXCNT9              0xFFCC11A8         /*                             */
#define USB_EP_RXCNT10             0xFFCC11B8         /*                             */
#define USB_EP_TXTYPE0             0xFFCC111A         /*                   */
#define USB_EP_TXTYPE1             0xFFCC112A         /*                   */
#define USB_EP_TXTYPE2             0xFFCC113A         /*                   */
#define USB_EP_TXTYPE3             0xFFCC114A         /*                   */
#define USB_EP_TXTYPE4             0xFFCC115A         /*                   */
#define USB_EP_TXTYPE5             0xFFCC116A         /*                   */
#define USB_EP_TXTYPE6             0xFFCC117A         /*                   */
#define USB_EP_TXTYPE7             0xFFCC118A         /*                   */
#define USB_EP_TXTYPE8             0xFFCC119A         /*                   */
#define USB_EP_TXTYPE9             0xFFCC11AA         /*                   */
#define USB_EP_TXTYPE10            0xFFCC11BA         /*                   */
#define USB_EP_TXINTERVAL0         0xFFCC111B         /*                               */
#define USB_EP_TXINTERVAL1         0xFFCC112B         /*                               */
#define USB_EP_TXINTERVAL2         0xFFCC113B         /*                               */
#define USB_EP_TXINTERVAL3         0xFFCC114B         /*                               */
#define USB_EP_TXINTERVAL4         0xFFCC115B         /*                               */
#define USB_EP_TXINTERVAL5         0xFFCC116B         /*                               */
#define USB_EP_TXINTERVAL6         0xFFCC117B         /*                               */
#define USB_EP_TXINTERVAL7         0xFFCC118B         /*                               */
#define USB_EP_TXINTERVAL8         0xFFCC119B         /*                               */
#define USB_EP_TXINTERVAL9         0xFFCC11AB         /*                               */
#define USB_EP_TXINTERVAL10        0xFFCC11BB         /*                               */
#define USB_EP_RXTYPE0             0xFFCC111C         /*                  */
#define USB_EP_RXTYPE1             0xFFCC112C         /*                  */
#define USB_EP_RXTYPE2             0xFFCC113C         /*                  */
#define USB_EP_RXTYPE3             0xFFCC114C         /*                  */
#define USB_EP_RXTYPE4             0xFFCC115C         /*                  */
#define USB_EP_RXTYPE5             0xFFCC116C         /*                  */
#define USB_EP_RXTYPE6             0xFFCC117C         /*                  */
#define USB_EP_RXTYPE7             0xFFCC118C         /*                  */
#define USB_EP_RXTYPE8             0xFFCC119C         /*                  */
#define USB_EP_RXTYPE9             0xFFCC11AC         /*                  */
#define USB_EP_RXTYPE10            0xFFCC11BC         /*                  */
#define USB_EP_RXINTERVAL0         0xFFCC111D         /*                              */
#define USB_EP_RXINTERVAL1         0xFFCC112D         /*                              */
#define USB_EP_RXINTERVAL2         0xFFCC113D         /*                              */
#define USB_EP_RXINTERVAL3         0xFFCC114D         /*                              */
#define USB_EP_RXINTERVAL4         0xFFCC115D         /*                              */
#define USB_EP_RXINTERVAL5         0xFFCC116D         /*                              */
#define USB_EP_RXINTERVAL6         0xFFCC117D         /*                              */
#define USB_EP_RXINTERVAL7         0xFFCC118D         /*                              */
#define USB_EP_RXINTERVAL8         0xFFCC119D         /*                              */
#define USB_EP_RXINTERVAL9         0xFFCC11AD         /*                              */
#define USB_EP_RXINTERVAL10        0xFFCC11BD         /*                              */
#define USB_DMA_IRQ                0xFFCC1200         /*                        */
#define USB_DMA_CTL0               0xFFCC1204         /*                 */
#define USB_DMA_CTL1               0xFFCC1214         /*                 */
#define USB_DMA_CTL2               0xFFCC1224         /*                 */
#define USB_DMA_CTL3               0xFFCC1234         /*                 */
#define USB_DMA_CTL4               0xFFCC1244         /*                 */
#define USB_DMA_CTL5               0xFFCC1254         /*                 */
#define USB_DMA_CTL6               0xFFCC1264         /*                 */
#define USB_DMA_CTL7               0xFFCC1274         /*                 */
#define USB_DMA_ADDR0              0xFFCC1208         /*                 */
#define USB_DMA_ADDR1              0xFFCC1218         /*                 */
#define USB_DMA_ADDR2              0xFFCC1228         /*                 */
#define USB_DMA_ADDR3              0xFFCC1238         /*                 */
#define USB_DMA_ADDR4              0xFFCC1248         /*                 */
#define USB_DMA_ADDR5              0xFFCC1258         /*                 */
#define USB_DMA_ADDR6              0xFFCC1268         /*                 */
#define USB_DMA_ADDR7              0xFFCC1278         /*                 */
#define USB_DMA_CNT0               0xFFCC120C         /*               */
#define USB_DMA_CNT1               0xFFCC121C         /*               */
#define USB_DMA_CNT2               0xFFCC122C         /*               */
#define USB_DMA_CNT3               0xFFCC123C         /*               */
#define USB_DMA_CNT4               0xFFCC124C         /*               */
#define USB_DMA_CNT5               0xFFCC125C         /*               */
#define USB_DMA_CNT6               0xFFCC126C         /*               */
#define USB_DMA_CNT7               0xFFCC127C         /*               */
#define USB_RQPKTCNT0              0xFFCC1300         /*                          */
#define USB_RQPKTCNT1              0xFFCC1304         /*                          */
#define USB_RQPKTCNT2              0xFFCC1308         /*                          */
#define USB_RQPKTCNT3              0xFFCC130C         /*                          */
#define USB_RQPKTCNT4              0xFFCC1310         /*                          */
#define USB_RQPKTCNT5              0xFFCC1314         /*                          */
#define USB_RQPKTCNT6              0xFFCC1318         /*                          */
#define USB_RQPKTCNT7              0xFFCC131C         /*                          */
#define USB_RQPKTCNT8              0xFFCC1320         /*                          */
#define USB_RQPKTCNT9              0xFFCC1324         /*                          */
#define USB_RQPKTCNT10             0xFFCC1328         /*                          */
#define USB_CT_UCH                 0xFFCC1344         /*                   */
#define USB_CT_HHSRTN              0xFFCC1346         /*                                        */
#define USB_CT_HSBT                0xFFCC1348         /*                        */
#define USB_LPM_ATTR               0xFFCC1360         /*                   */
#define USB_LPM_CTL                0xFFCC1362         /*                 */
#define USB_LPM_IEN                0xFFCC1363         /*                          */
#define USB_LPM_IRQ                0xFFCC1364         /*                   */
#define USB_LPM_FADDR              0xFFCC1365         /*                          */
#define USB_VBUS_CTL               0xFFCC1380         /*                  */
#define USB_BAT_CHG                0xFFCC1381         /*                      */
#define USB_PHY_CTL                0xFFCC1394         /*                 */
#define USB_TESTCTL                0xFFCC1397         /*                  */
#define USB_PLL_OSC                0xFFCC1398         /*                                */



/*                          
              
                             */

#define                           CHIPID  0xffc00014
/*              */
#define                   CHIPID_VERSION  0xF0000000
#define                    CHIPID_FAMILY  0x0FFFF000
#define               CHIPID_MANUFACTURE  0x00000FFE


#endif /*              */
