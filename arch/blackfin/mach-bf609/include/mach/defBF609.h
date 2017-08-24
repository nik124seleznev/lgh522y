/*
 * Copyright 2011 Analog Devices Inc.
 *
 * Licensed under the Clear BSD license or the GPL-2 (or later)
 */

#ifndef _DEF_BF609_H
#define _DEF_BF609_H

/*                                                                                              */
#include "defBF60x_base.h"

/*                                                                                       */
/*                          
               
                             */

/*                          
      
                             */
#define PIXC0_CTL                   0xFFC19000         /*                        */
#define PIXC0_PPL                   0xFFC19004         /*                                */
#define PIXC0_LPF                   0xFFC19008         /*                               */
#define PIXC0_HSTART_A              0xFFC1900C         /*                                           */
#define PIXC0_HEND_A                0xFFC19010         /*                                         */
#define PIXC0_VSTART_A              0xFFC19014         /*                                         */
#define PIXC0_VEND_A                0xFFC19018         /*                                       */
#define PIXC0_TRANSP_A              0xFFC1901C         /*                                             */
#define PIXC0_HSTART_B              0xFFC19020         /*                                           */
#define PIXC0_HEND_B                0xFFC19024         /*                                         */
#define PIXC0_VSTART_B              0xFFC19028         /*                                         */
#define PIXC0_VEND_B                0xFFC1902C         /*                                       */
#define PIXC0_TRANSP_B              0xFFC19030         /*                                             */
#define PIXC0_IRQSTAT               0xFFC1903C         /*                                 */
#define PIXC0_CONRY                 0xFFC19040         /*                                        */
#define PIXC0_CONGU                 0xFFC19044         /*                                        */
#define PIXC0_CONBV                 0xFFC19048         /*                                        */
#define PIXC0_CCBIAS                0xFFC1904C         /*                                */
#define PIXC0_TC                    0xFFC19050         /*                             */
#define PIXC0_REVID                 0xFFC19054         /*                        */

/*                          
              
                             */

/*                          
     
                             */
#define PVP0_REVID                  0xFFC1A000         /*                  */
#define PVP0_CTL                    0xFFC1A004         /*              */
#define PVP0_IMSK0                  0xFFC1A008         /*                                */
#define PVP0_IMSK1                  0xFFC1A00C         /*                                */
#define PVP0_STAT                   0xFFC1A010         /*             */
#define PVP0_ILAT                   0xFFC1A014         /*                     */
#define PVP0_IREQ0                  0xFFC1A018         /*                                 */
#define PVP0_IREQ1                  0xFFC1A01C         /*                                 */
#define PVP0_OPF0_CFG               0xFFC1A020         /*             */
#define PVP0_OPF1_CFG               0xFFC1A040         /*             */
#define PVP0_OPF2_CFG               0xFFC1A060         /*             */
#define PVP0_OPF0_CTL               0xFFC1A024         /*              */
#define PVP0_OPF1_CTL               0xFFC1A044         /*              */
#define PVP0_OPF2_CTL               0xFFC1A064         /*              */
#define PVP0_OPF3_CFG               0xFFC1A080         /*             */
#define PVP0_OPF3_CTL               0xFFC1A084         /*              */
#define PVP0_PEC_CFG                0xFFC1A0A0         /*             */
#define PVP0_PEC_CTL                0xFFC1A0A4         /*              */
#define PVP0_PEC_D1TH0              0xFFC1A0A8         /*                                 */
#define PVP0_PEC_D1TH1              0xFFC1A0AC         /*                                 */
#define PVP0_PEC_D2TH0              0xFFC1A0B0         /*                                   */
#define PVP0_PEC_D2TH1              0xFFC1A0B4         /*                                     */
#define PVP0_IIM0_CFG               0xFFC1A0C0         /*             */
#define PVP0_IIM1_CFG               0xFFC1A0E0         /*             */
#define PVP0_IIM0_CTL               0xFFC1A0C4         /*              */
#define PVP0_IIM1_CTL               0xFFC1A0E4         /*              */
#define PVP0_IIM0_SCALE             0xFFC1A0C8         /*                    */
#define PVP0_IIM1_SCALE             0xFFC1A0E8         /*                    */
#define PVP0_IIM0_SOVF_STAT         0xFFC1A0CC         /*                             */
#define PVP0_IIM1_SOVF_STAT         0xFFC1A0EC         /*                             */
#define PVP0_IIM0_UOVF_STAT         0xFFC1A0D0         /*                               */
#define PVP0_IIM1_UOVF_STAT         0xFFC1A0F0         /*                               */
#define PVP0_ACU_CFG                0xFFC1A100         /*                                 */
#define PVP0_ACU_CTL                0xFFC1A104         /*                           */
#define PVP0_ACU_OFFSET             0xFFC1A108         /*                            */
#define PVP0_ACU_FACTOR             0xFFC1A10C         /*                             */
#define PVP0_ACU_SHIFT              0xFFC1A110         /*                              */
#define PVP0_ACU_MIN                0xFFC1A114         /*                                            */
#define PVP0_ACU_MAX                0xFFC1A118         /*                                            */
#define PVP0_UDS_CFG                0xFFC1A140         /*                                 */
#define PVP0_UDS_CTL                0xFFC1A144         /*                           */
#define PVP0_UDS_OHCNT              0xFFC1A148         /*                             */
#define PVP0_UDS_OVCNT              0xFFC1A14C         /*                             */
#define PVP0_UDS_HAVG               0xFFC1A150         /*                 */
#define PVP0_UDS_VAVG               0xFFC1A154         /*                 */
#define PVP0_IPF0_CFG               0xFFC1A180         /*                    */
#define PVP0_IPF0_PIPECTL           0xFFC1A184         /*                   */
#define PVP0_IPF1_PIPECTL           0xFFC1A1C4         /*                   */
#define PVP0_IPF0_CTL               0xFFC1A188         /*              */
#define PVP0_IPF1_CTL               0xFFC1A1C8         /*              */
#define PVP0_IPF0_TAG               0xFFC1A18C         /*                */
#define PVP0_IPF1_TAG               0xFFC1A1CC         /*                */
#define PVP0_IPF0_FCNT              0xFFC1A190         /*                  */
#define PVP0_IPF1_FCNT              0xFFC1A1D0         /*                  */
#define PVP0_IPF0_HCNT              0xFFC1A194         /*                       */
#define PVP0_IPF1_HCNT              0xFFC1A1D4         /*                       */
#define PVP0_IPF0_VCNT              0xFFC1A198         /*                     */
#define PVP0_IPF1_VCNT              0xFFC1A1D8         /*                     */
#define PVP0_IPF0_HPOS              0xFFC1A19C         /*                          */
#define PVP0_IPF0_VPOS              0xFFC1A1A0         /*                        */
#define PVP0_IPF0_TAG_STAT          0xFFC1A1A4         /*                 */
#define PVP0_IPF1_TAG_STAT          0xFFC1A1E4         /*                 */
#define PVP0_IPF1_CFG               0xFFC1A1C0         /*                    */
#define PVP0_CNV0_CFG               0xFFC1A200         /*                    */
#define PVP0_CNV1_CFG               0xFFC1A280         /*                    */
#define PVP0_CNV2_CFG               0xFFC1A300         /*                    */
#define PVP0_CNV3_CFG               0xFFC1A380         /*                    */
#define PVP0_CNV0_CTL               0xFFC1A204         /*              */
#define PVP0_CNV1_CTL               0xFFC1A284         /*              */
#define PVP0_CNV2_CTL               0xFFC1A304         /*              */
#define PVP0_CNV3_CTL               0xFFC1A384         /*              */
#define PVP0_CNV0_C00C01            0xFFC1A208         /*                                 */
#define PVP0_CNV1_C00C01            0xFFC1A288         /*                                 */
#define PVP0_CNV2_C00C01            0xFFC1A308         /*                                 */
#define PVP0_CNV3_C00C01            0xFFC1A388         /*                                 */
#define PVP0_CNV0_C02C03            0xFFC1A20C         /*                                 */
#define PVP0_CNV1_C02C03            0xFFC1A28C         /*                                 */
#define PVP0_CNV2_C02C03            0xFFC1A30C         /*                                 */
#define PVP0_CNV3_C02C03            0xFFC1A38C         /*                                 */
#define PVP0_CNV0_C04               0xFFC1A210         /*                       */
#define PVP0_CNV1_C04               0xFFC1A290         /*                       */
#define PVP0_CNV2_C04               0xFFC1A310         /*                       */
#define PVP0_CNV3_C04               0xFFC1A390         /*                       */
#define PVP0_CNV0_C10C11            0xFFC1A214         /*                                 */
#define PVP0_CNV1_C10C11            0xFFC1A294         /*                                 */
#define PVP0_CNV2_C10C11            0xFFC1A314         /*                                 */
#define PVP0_CNV3_C10C11            0xFFC1A394         /*                                 */
#define PVP0_CNV0_C12C13            0xFFC1A218         /*                                 */
#define PVP0_CNV1_C12C13            0xFFC1A298         /*                                 */
#define PVP0_CNV2_C12C13            0xFFC1A318         /*                                 */
#define PVP0_CNV3_C12C13            0xFFC1A398         /*                                 */
#define PVP0_CNV0_C14               0xFFC1A21C         /*                       */
#define PVP0_CNV1_C14               0xFFC1A29C         /*                       */
#define PVP0_CNV2_C14               0xFFC1A31C         /*                       */
#define PVP0_CNV3_C14               0xFFC1A39C         /*                       */
#define PVP0_CNV0_C20C21            0xFFC1A220         /*                                 */
#define PVP0_CNV1_C20C21            0xFFC1A2A0         /*                                 */
#define PVP0_CNV2_C20C21            0xFFC1A320         /*                                 */
#define PVP0_CNV3_C20C21            0xFFC1A3A0         /*                                 */
#define PVP0_CNV0_C22C23            0xFFC1A224         /*                                 */
#define PVP0_CNV1_C22C23            0xFFC1A2A4         /*                                 */
#define PVP0_CNV2_C22C23            0xFFC1A324         /*                                 */
#define PVP0_CNV3_C22C23            0xFFC1A3A4         /*                                 */
#define PVP0_CNV0_C24               0xFFC1A228         /*                      */
#define PVP0_CNV1_C24               0xFFC1A2A8         /*                      */
#define PVP0_CNV2_C24               0xFFC1A328         /*                      */
#define PVP0_CNV3_C24               0xFFC1A3A8         /*                      */
#define PVP0_CNV0_C30C31            0xFFC1A22C         /*                                 */
#define PVP0_CNV1_C30C31            0xFFC1A2AC         /*                                 */
#define PVP0_CNV2_C30C31            0xFFC1A32C         /*                                 */
#define PVP0_CNV3_C30C31            0xFFC1A3AC         /*                                 */
#define PVP0_CNV0_C32C33            0xFFC1A230         /*                                 */
#define PVP0_CNV1_C32C33            0xFFC1A2B0         /*                                 */
#define PVP0_CNV2_C32C33            0xFFC1A330         /*                                 */
#define PVP0_CNV3_C32C33            0xFFC1A3B0         /*                                 */
#define PVP0_CNV0_C34               0xFFC1A234         /*                       */
#define PVP0_CNV1_C34               0xFFC1A2B4         /*                       */
#define PVP0_CNV2_C34               0xFFC1A334         /*                       */
#define PVP0_CNV3_C34               0xFFC1A3B4         /*                       */
#define PVP0_CNV0_C40C41            0xFFC1A238         /*                                 */
#define PVP0_CNV1_C40C41            0xFFC1A2B8         /*                                 */
#define PVP0_CNV2_C40C41            0xFFC1A338         /*                                 */
#define PVP0_CNV3_C40C41            0xFFC1A3B8         /*                                 */
#define PVP0_CNV0_C42C43            0xFFC1A23C         /*                                 */
#define PVP0_CNV1_C42C43            0xFFC1A2BC         /*                                 */
#define PVP0_CNV2_C42C43            0xFFC1A33C         /*                                 */
#define PVP0_CNV3_C42C43            0xFFC1A3BC         /*                                 */
#define PVP0_CNV0_C44               0xFFC1A240         /*                       */
#define PVP0_CNV1_C44               0xFFC1A2C0         /*                       */
#define PVP0_CNV2_C44               0xFFC1A340         /*                       */
#define PVP0_CNV3_C44               0xFFC1A3C0         /*                       */
#define PVP0_CNV0_SCALE             0xFFC1A244         /*                     */
#define PVP0_CNV1_SCALE             0xFFC1A2C4         /*                     */
#define PVP0_CNV2_SCALE             0xFFC1A344         /*                     */
#define PVP0_CNV3_SCALE             0xFFC1A3C4         /*                     */
#define PVP0_THC0_CFG               0xFFC1A400         /*                    */
#define PVP0_THC1_CFG               0xFFC1A500         /*                    */
#define PVP0_THC0_CTL               0xFFC1A404         /*              */
#define PVP0_THC1_CTL               0xFFC1A504         /*              */
#define PVP0_THC0_HFCNT             0xFFC1A408         /*                       */
#define PVP0_THC1_HFCNT             0xFFC1A508         /*                       */
#define PVP0_THC0_RMAXREP           0xFFC1A40C         /*                                    */
#define PVP0_THC1_RMAXREP           0xFFC1A50C         /*                                    */
#define PVP0_THC0_CMINVAL           0xFFC1A410         /*                     */
#define PVP0_THC1_CMINVAL           0xFFC1A510         /*                     */
#define PVP0_THC0_CMINTH            0xFFC1A414         /*                         */
#define PVP0_THC1_CMINTH            0xFFC1A514         /*                         */
#define PVP0_THC0_CMAXTH            0xFFC1A418         /*                         */
#define PVP0_THC1_CMAXTH            0xFFC1A518         /*                         */
#define PVP0_THC0_CMAXVAL           0xFFC1A41C         /*                     */
#define PVP0_THC1_CMAXVAL           0xFFC1A51C         /*                     */
#define PVP0_THC0_TH0               0xFFC1A420         /*                      */
#define PVP0_THC1_TH0               0xFFC1A520         /*                      */
#define PVP0_THC0_TH1               0xFFC1A424         /*                      */
#define PVP0_THC1_TH1               0xFFC1A524         /*                      */
#define PVP0_THC0_TH2               0xFFC1A428         /*                      */
#define PVP0_THC1_TH2               0xFFC1A528         /*                      */
#define PVP0_THC0_TH3               0xFFC1A42C         /*                      */
#define PVP0_THC1_TH3               0xFFC1A52C         /*                      */
#define PVP0_THC0_TH4               0xFFC1A430         /*                      */
#define PVP0_THC1_TH4               0xFFC1A530         /*                      */
#define PVP0_THC0_TH5               0xFFC1A434         /*                      */
#define PVP0_THC1_TH5               0xFFC1A534         /*                      */
#define PVP0_THC0_TH6               0xFFC1A438         /*                      */
#define PVP0_THC1_TH6               0xFFC1A538         /*                      */
#define PVP0_THC0_TH7               0xFFC1A43C         /*                      */
#define PVP0_THC1_TH7               0xFFC1A53C         /*                      */
#define PVP0_THC0_TH8               0xFFC1A440         /*                      */
#define PVP0_THC1_TH8               0xFFC1A540         /*                      */
#define PVP0_THC0_TH9               0xFFC1A444         /*                      */
#define PVP0_THC1_TH9               0xFFC1A544         /*                      */
#define PVP0_THC0_TH10              0xFFC1A448         /*                      */
#define PVP0_THC1_TH10              0xFFC1A548         /*                      */
#define PVP0_THC0_TH11              0xFFC1A44C         /*                      */
#define PVP0_THC1_TH11              0xFFC1A54C         /*                      */
#define PVP0_THC0_TH12              0xFFC1A450         /*                      */
#define PVP0_THC1_TH12              0xFFC1A550         /*                      */
#define PVP0_THC0_TH13              0xFFC1A454         /*                      */
#define PVP0_THC1_TH13              0xFFC1A554         /*                      */
#define PVP0_THC0_TH14              0xFFC1A458         /*                      */
#define PVP0_THC1_TH14              0xFFC1A558         /*                      */
#define PVP0_THC0_TH15              0xFFC1A45C         /*                      */
#define PVP0_THC1_TH15              0xFFC1A55C         /*                      */
#define PVP0_THC0_HHPOS             0xFFC1A460         /*                                */
#define PVP0_THC1_HHPOS             0xFFC1A560         /*                                */
#define PVP0_THC0_HVPOS             0xFFC1A464         /*                                */
#define PVP0_THC1_HVPOS             0xFFC1A564         /*                                */
#define PVP0_THC0_HHCNT             0xFFC1A468         /*                                  */
#define PVP0_THC1_HHCNT             0xFFC1A568         /*                                  */
#define PVP0_THC0_HVCNT             0xFFC1A46C         /*                                  */
#define PVP0_THC1_HVCNT             0xFFC1A56C         /*                                  */
#define PVP0_THC0_RHPOS             0xFFC1A470         /*                                */
#define PVP0_THC1_RHPOS             0xFFC1A570         /*                                */
#define PVP0_THC0_RVPOS             0xFFC1A474         /*                                */
#define PVP0_THC1_RVPOS             0xFFC1A574         /*                                */
#define PVP0_THC0_RHCNT             0xFFC1A478         /*                                  */
#define PVP0_THC1_RHCNT             0xFFC1A578         /*                                  */
#define PVP0_THC0_RVCNT             0xFFC1A47C         /*                                  */
#define PVP0_THC1_RVCNT             0xFFC1A57C         /*                                  */
#define PVP0_THC0_HFCNT_STAT        0xFFC1A480         /*                            */
#define PVP0_THC1_HFCNT_STAT        0xFFC1A580         /*                            */
#define PVP0_THC0_HCNT0_STAT        0xFFC1A484         /*                              */
#define PVP0_THC1_HCNT0_STAT        0xFFC1A584         /*                              */
#define PVP0_THC0_HCNT1_STAT        0xFFC1A488         /*                              */
#define PVP0_THC1_HCNT1_STAT        0xFFC1A588         /*                              */
#define PVP0_THC0_HCNT2_STAT        0xFFC1A48C         /*                              */
#define PVP0_THC1_HCNT2_STAT        0xFFC1A58C         /*                              */
#define PVP0_THC0_HCNT3_STAT        0xFFC1A490         /*                              */
#define PVP0_THC1_HCNT3_STAT        0xFFC1A590         /*                              */
#define PVP0_THC0_HCNT4_STAT        0xFFC1A494         /*                              */
#define PVP0_THC1_HCNT4_STAT        0xFFC1A594         /*                              */
#define PVP0_THC0_HCNT5_STAT        0xFFC1A498         /*                              */
#define PVP0_THC1_HCNT5_STAT        0xFFC1A598         /*                              */
#define PVP0_THC0_HCNT6_STAT        0xFFC1A49C         /*                              */
#define PVP0_THC1_HCNT6_STAT        0xFFC1A59C         /*                              */
#define PVP0_THC0_HCNT7_STAT        0xFFC1A4A0         /*                              */
#define PVP0_THC1_HCNT7_STAT        0xFFC1A5A0         /*                              */
#define PVP0_THC0_HCNT8_STAT        0xFFC1A4A4         /*                              */
#define PVP0_THC1_HCNT8_STAT        0xFFC1A5A4         /*                              */
#define PVP0_THC0_HCNT9_STAT        0xFFC1A4A8         /*                              */
#define PVP0_THC1_HCNT9_STAT        0xFFC1A5A8         /*                              */
#define PVP0_THC0_HCNT10_STAT       0xFFC1A4AC         /*                              */
#define PVP0_THC1_HCNT10_STAT       0xFFC1A5AC         /*                              */
#define PVP0_THC0_HCNT11_STAT       0xFFC1A4B0         /*                              */
#define PVP0_THC1_HCNT11_STAT       0xFFC1A5B0         /*                              */
#define PVP0_THC0_HCNT12_STAT       0xFFC1A4B4         /*                              */
#define PVP0_THC1_HCNT12_STAT       0xFFC1A5B4         /*                              */
#define PVP0_THC0_HCNT13_STAT       0xFFC1A4B8         /*                              */
#define PVP0_THC1_HCNT13_STAT       0xFFC1A5B8         /*                              */
#define PVP0_THC0_HCNT14_STAT       0xFFC1A4BC         /*                              */
#define PVP0_THC1_HCNT14_STAT       0xFFC1A5BC         /*                              */
#define PVP0_THC0_HCNT15_STAT       0xFFC1A4C0         /*                              */
#define PVP0_THC1_HCNT15_STAT       0xFFC1A5C0         /*                              */
#define PVP0_THC0_RREP_STAT         0xFFC1A4C4         /*                            */
#define PVP0_THC1_RREP_STAT         0xFFC1A5C4         /*                            */
#define PVP0_PMA_CFG                0xFFC1A600         /*                                 */

#endif /*              */
