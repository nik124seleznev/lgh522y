/*
 * wm8962.h  --  WM8962 ASoC driver
 *
 * Copyright 2010 Wolfson Microelectronics, plc
 *
 * Author: Mark Brown <broonie@opensource.wolfsonmicro.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef _WM8962_H
#define _WM8962_H

#include <asm/types.h>
#include <sound/soc.h>

#define WM8962_SYSCLK_MCLK 1
#define WM8962_SYSCLK_FLL  2
#define WM8962_SYSCLK_PLL3 3

#define WM8962_FLL  1

#define WM8962_FLL_MCLK 1
#define WM8962_FLL_BCLK 2
#define WM8962_FLL_OSC  3
#define WM8962_FLL_INT  4

/*
                   
 */
#define WM8962_LEFT_INPUT_VOLUME                0x00
#define WM8962_RIGHT_INPUT_VOLUME               0x01
#define WM8962_HPOUTL_VOLUME                    0x02
#define WM8962_HPOUTR_VOLUME                    0x03
#define WM8962_CLOCKING1                        0x04
#define WM8962_ADC_DAC_CONTROL_1                0x05
#define WM8962_ADC_DAC_CONTROL_2                0x06
#define WM8962_AUDIO_INTERFACE_0                0x07
#define WM8962_CLOCKING2                        0x08
#define WM8962_AUDIO_INTERFACE_1                0x09
#define WM8962_LEFT_DAC_VOLUME                  0x0A
#define WM8962_RIGHT_DAC_VOLUME                 0x0B
#define WM8962_AUDIO_INTERFACE_2                0x0E
#define WM8962_SOFTWARE_RESET                   0x0F
#define WM8962_ALC1                             0x11
#define WM8962_ALC2                             0x12
#define WM8962_ALC3                             0x13
#define WM8962_NOISE_GATE                       0x14
#define WM8962_LEFT_ADC_VOLUME                  0x15
#define WM8962_RIGHT_ADC_VOLUME                 0x16
#define WM8962_ADDITIONAL_CONTROL_1             0x17
#define WM8962_ADDITIONAL_CONTROL_2             0x18
#define WM8962_PWR_MGMT_1                       0x19
#define WM8962_PWR_MGMT_2                       0x1A
#define WM8962_ADDITIONAL_CONTROL_3             0x1B
#define WM8962_ANTI_POP                         0x1C
#define WM8962_CLOCKING_3                       0x1E
#define WM8962_INPUT_MIXER_CONTROL_1            0x1F
#define WM8962_LEFT_INPUT_MIXER_VOLUME          0x20
#define WM8962_RIGHT_INPUT_MIXER_VOLUME         0x21
#define WM8962_INPUT_MIXER_CONTROL_2            0x22
#define WM8962_INPUT_BIAS_CONTROL               0x23
#define WM8962_LEFT_INPUT_PGA_CONTROL           0x25
#define WM8962_RIGHT_INPUT_PGA_CONTROL          0x26
#define WM8962_SPKOUTL_VOLUME                   0x28
#define WM8962_SPKOUTR_VOLUME                   0x29
#define WM8962_THERMAL_SHUTDOWN_STATUS          0x2F
#define WM8962_ADDITIONAL_CONTROL_4             0x30
#define WM8962_CLASS_D_CONTROL_1                0x31
#define WM8962_CLASS_D_CONTROL_2                0x33
#define WM8962_CLOCKING_4                       0x38
#define WM8962_DAC_DSP_MIXING_1                 0x39
#define WM8962_DAC_DSP_MIXING_2                 0x3A
#define WM8962_DC_SERVO_0                       0x3C
#define WM8962_DC_SERVO_1                       0x3D
#define WM8962_DC_SERVO_4                       0x40
#define WM8962_DC_SERVO_6                       0x42
#define WM8962_ANALOGUE_PGA_BIAS                0x44
#define WM8962_ANALOGUE_HP_0                    0x45
#define WM8962_ANALOGUE_HP_2                    0x47
#define WM8962_CHARGE_PUMP_1                    0x48
#define WM8962_CHARGE_PUMP_B                    0x52
#define WM8962_WRITE_SEQUENCER_CONTROL_1        0x57
#define WM8962_WRITE_SEQUENCER_CONTROL_2        0x5A
#define WM8962_WRITE_SEQUENCER_CONTROL_3        0x5D
#define WM8962_CONTROL_INTERFACE                0x5E
#define WM8962_MIXER_ENABLES                    0x63
#define WM8962_HEADPHONE_MIXER_1                0x64
#define WM8962_HEADPHONE_MIXER_2                0x65
#define WM8962_HEADPHONE_MIXER_3                0x66
#define WM8962_HEADPHONE_MIXER_4                0x67
#define WM8962_SPEAKER_MIXER_1                  0x69
#define WM8962_SPEAKER_MIXER_2                  0x6A
#define WM8962_SPEAKER_MIXER_3                  0x6B
#define WM8962_SPEAKER_MIXER_4                  0x6C
#define WM8962_SPEAKER_MIXER_5                  0x6D
#define WM8962_BEEP_GENERATOR_1                 0x6E
#define WM8962_OSCILLATOR_TRIM_3                0x73
#define WM8962_OSCILLATOR_TRIM_4                0x74
#define WM8962_OSCILLATOR_TRIM_7                0x77
#define WM8962_ANALOGUE_CLOCKING1               0x7C
#define WM8962_ANALOGUE_CLOCKING2               0x7D
#define WM8962_ANALOGUE_CLOCKING3               0x7E
#define WM8962_PLL_SOFTWARE_RESET               0x7F
#define WM8962_PLL2                             0x81
#define WM8962_PLL_4                            0x83
#define WM8962_PLL_9                            0x88
#define WM8962_PLL_10                           0x89
#define WM8962_PLL_11                           0x8A
#define WM8962_PLL_12                           0x8B
#define WM8962_PLL_13                           0x8C
#define WM8962_PLL_14                           0x8D
#define WM8962_PLL_15                           0x8E
#define WM8962_PLL_16                           0x8F
#define WM8962_FLL_CONTROL_1                    0x9B
#define WM8962_FLL_CONTROL_2                    0x9C
#define WM8962_FLL_CONTROL_3                    0x9D
#define WM8962_FLL_CONTROL_5                    0x9F
#define WM8962_FLL_CONTROL_6                    0xA0
#define WM8962_FLL_CONTROL_7                    0xA1
#define WM8962_FLL_CONTROL_8                    0xA2
#define WM8962_GENERAL_TEST_1                   0xFC
#define WM8962_DF1                              0x100
#define WM8962_DF2                              0x101
#define WM8962_DF3                              0x102
#define WM8962_DF4                              0x103
#define WM8962_DF5                              0x104
#define WM8962_DF6                              0x105
#define WM8962_DF7                              0x106
#define WM8962_LHPF1                            0x108
#define WM8962_LHPF2                            0x109
#define WM8962_THREED1                          0x10C
#define WM8962_THREED2                          0x10D
#define WM8962_THREED3                          0x10E
#define WM8962_THREED4                          0x10F
#define WM8962_DRC_1                            0x114
#define WM8962_DRC_2                            0x115
#define WM8962_DRC_3                            0x116
#define WM8962_DRC_4                            0x117
#define WM8962_DRC_5                            0x118
#define WM8962_TLOOPBACK                        0x11D
#define WM8962_EQ1                              0x14F
#define WM8962_EQ2                              0x150
#define WM8962_EQ3                              0x151
#define WM8962_EQ4                              0x152
#define WM8962_EQ5                              0x153
#define WM8962_EQ6                              0x154
#define WM8962_EQ7                              0x155
#define WM8962_EQ8                              0x156
#define WM8962_EQ9                              0x157
#define WM8962_EQ10                             0x158
#define WM8962_EQ11                             0x159
#define WM8962_EQ12                             0x15A
#define WM8962_EQ13                             0x15B
#define WM8962_EQ14                             0x15C
#define WM8962_EQ15                             0x15D
#define WM8962_EQ16                             0x15E
#define WM8962_EQ17                             0x15F
#define WM8962_EQ18                             0x160
#define WM8962_EQ19                             0x161
#define WM8962_EQ20                             0x162
#define WM8962_EQ21                             0x163
#define WM8962_EQ22                             0x164
#define WM8962_EQ23                             0x165
#define WM8962_EQ24                             0x166
#define WM8962_EQ25                             0x167
#define WM8962_EQ26                             0x168
#define WM8962_EQ27                             0x169
#define WM8962_EQ28                             0x16A
#define WM8962_EQ29                             0x16B
#define WM8962_EQ30                             0x16C
#define WM8962_EQ31                             0x16D
#define WM8962_EQ32                             0x16E
#define WM8962_EQ33                             0x16F
#define WM8962_EQ34                             0x170
#define WM8962_EQ35                             0x171
#define WM8962_EQ36                             0x172
#define WM8962_EQ37                             0x173
#define WM8962_EQ38                             0x174
#define WM8962_EQ39                             0x175
#define WM8962_EQ40                             0x176
#define WM8962_EQ41                             0x177
#define WM8962_GPIO_BASE			0x200
#define WM8962_GPIO_2                           0x201
#define WM8962_GPIO_3                           0x202
#define WM8962_GPIO_5                           0x204
#define WM8962_GPIO_6                           0x205
#define WM8962_INTERRUPT_STATUS_1               0x230
#define WM8962_INTERRUPT_STATUS_2               0x231
#define WM8962_INTERRUPT_STATUS_1_MASK          0x238
#define WM8962_INTERRUPT_STATUS_2_MASK          0x239
#define WM8962_INTERRUPT_CONTROL                0x240
#define WM8962_IRQ_DEBOUNCE                     0x248
#define WM8962_MICINT_SOURCE_POL                0x24A
#define WM8962_DSP2_POWER_MANAGEMENT            0x300
#define WM8962_DSP2_EXECCONTROL                 0x40D
#define WM8962_WRITE_SEQUENCER_0                0x1000
#define WM8962_WRITE_SEQUENCER_1                0x1001
#define WM8962_WRITE_SEQUENCER_2                0x1002
#define WM8962_WRITE_SEQUENCER_3                0x1003
#define WM8962_WRITE_SEQUENCER_4                0x1004
#define WM8962_WRITE_SEQUENCER_5                0x1005
#define WM8962_WRITE_SEQUENCER_6                0x1006
#define WM8962_WRITE_SEQUENCER_7                0x1007
#define WM8962_WRITE_SEQUENCER_8                0x1008
#define WM8962_WRITE_SEQUENCER_9                0x1009
#define WM8962_WRITE_SEQUENCER_10               0x100A
#define WM8962_WRITE_SEQUENCER_11               0x100B
#define WM8962_WRITE_SEQUENCER_12               0x100C
#define WM8962_WRITE_SEQUENCER_13               0x100D
#define WM8962_WRITE_SEQUENCER_14               0x100E
#define WM8962_WRITE_SEQUENCER_15               0x100F
#define WM8962_WRITE_SEQUENCER_16               0x1010
#define WM8962_WRITE_SEQUENCER_17               0x1011
#define WM8962_WRITE_SEQUENCER_18               0x1012
#define WM8962_WRITE_SEQUENCER_19               0x1013
#define WM8962_WRITE_SEQUENCER_20               0x1014
#define WM8962_WRITE_SEQUENCER_21               0x1015
#define WM8962_WRITE_SEQUENCER_22               0x1016
#define WM8962_WRITE_SEQUENCER_23               0x1017
#define WM8962_WRITE_SEQUENCER_24               0x1018
#define WM8962_WRITE_SEQUENCER_25               0x1019
#define WM8962_WRITE_SEQUENCER_26               0x101A
#define WM8962_WRITE_SEQUENCER_27               0x101B
#define WM8962_WRITE_SEQUENCER_28               0x101C
#define WM8962_WRITE_SEQUENCER_29               0x101D
#define WM8962_WRITE_SEQUENCER_30               0x101E
#define WM8962_WRITE_SEQUENCER_31               0x101F
#define WM8962_WRITE_SEQUENCER_32               0x1020
#define WM8962_WRITE_SEQUENCER_33               0x1021
#define WM8962_WRITE_SEQUENCER_34               0x1022
#define WM8962_WRITE_SEQUENCER_35               0x1023
#define WM8962_WRITE_SEQUENCER_36               0x1024
#define WM8962_WRITE_SEQUENCER_37               0x1025
#define WM8962_WRITE_SEQUENCER_38               0x1026
#define WM8962_WRITE_SEQUENCER_39               0x1027
#define WM8962_WRITE_SEQUENCER_40               0x1028
#define WM8962_WRITE_SEQUENCER_41               0x1029
#define WM8962_WRITE_SEQUENCER_42               0x102A
#define WM8962_WRITE_SEQUENCER_43               0x102B
#define WM8962_WRITE_SEQUENCER_44               0x102C
#define WM8962_WRITE_SEQUENCER_45               0x102D
#define WM8962_WRITE_SEQUENCER_46               0x102E
#define WM8962_WRITE_SEQUENCER_47               0x102F
#define WM8962_WRITE_SEQUENCER_48               0x1030
#define WM8962_WRITE_SEQUENCER_49               0x1031
#define WM8962_WRITE_SEQUENCER_50               0x1032
#define WM8962_WRITE_SEQUENCER_51               0x1033
#define WM8962_WRITE_SEQUENCER_52               0x1034
#define WM8962_WRITE_SEQUENCER_53               0x1035
#define WM8962_WRITE_SEQUENCER_54               0x1036
#define WM8962_WRITE_SEQUENCER_55               0x1037
#define WM8962_WRITE_SEQUENCER_56               0x1038
#define WM8962_WRITE_SEQUENCER_57               0x1039
#define WM8962_WRITE_SEQUENCER_58               0x103A
#define WM8962_WRITE_SEQUENCER_59               0x103B
#define WM8962_WRITE_SEQUENCER_60               0x103C
#define WM8962_WRITE_SEQUENCER_61               0x103D
#define WM8962_WRITE_SEQUENCER_62               0x103E
#define WM8962_WRITE_SEQUENCER_63               0x103F
#define WM8962_WRITE_SEQUENCER_64               0x1040
#define WM8962_WRITE_SEQUENCER_65               0x1041
#define WM8962_WRITE_SEQUENCER_66               0x1042
#define WM8962_WRITE_SEQUENCER_67               0x1043
#define WM8962_WRITE_SEQUENCER_68               0x1044
#define WM8962_WRITE_SEQUENCER_69               0x1045
#define WM8962_WRITE_SEQUENCER_70               0x1046
#define WM8962_WRITE_SEQUENCER_71               0x1047
#define WM8962_WRITE_SEQUENCER_72               0x1048
#define WM8962_WRITE_SEQUENCER_73               0x1049
#define WM8962_WRITE_SEQUENCER_74               0x104A
#define WM8962_WRITE_SEQUENCER_75               0x104B
#define WM8962_WRITE_SEQUENCER_76               0x104C
#define WM8962_WRITE_SEQUENCER_77               0x104D
#define WM8962_WRITE_SEQUENCER_78               0x104E
#define WM8962_WRITE_SEQUENCER_79               0x104F
#define WM8962_WRITE_SEQUENCER_80               0x1050
#define WM8962_WRITE_SEQUENCER_81               0x1051
#define WM8962_WRITE_SEQUENCER_82               0x1052
#define WM8962_WRITE_SEQUENCER_83               0x1053
#define WM8962_WRITE_SEQUENCER_84               0x1054
#define WM8962_WRITE_SEQUENCER_85               0x1055
#define WM8962_WRITE_SEQUENCER_86               0x1056
#define WM8962_WRITE_SEQUENCER_87               0x1057
#define WM8962_WRITE_SEQUENCER_88               0x1058
#define WM8962_WRITE_SEQUENCER_89               0x1059
#define WM8962_WRITE_SEQUENCER_90               0x105A
#define WM8962_WRITE_SEQUENCER_91               0x105B
#define WM8962_WRITE_SEQUENCER_92               0x105C
#define WM8962_WRITE_SEQUENCER_93               0x105D
#define WM8962_WRITE_SEQUENCER_94               0x105E
#define WM8962_WRITE_SEQUENCER_95               0x105F
#define WM8962_WRITE_SEQUENCER_96               0x1060
#define WM8962_WRITE_SEQUENCER_97               0x1061
#define WM8962_WRITE_SEQUENCER_98               0x1062
#define WM8962_WRITE_SEQUENCER_99               0x1063
#define WM8962_WRITE_SEQUENCER_100              0x1064
#define WM8962_WRITE_SEQUENCER_101              0x1065
#define WM8962_WRITE_SEQUENCER_102              0x1066
#define WM8962_WRITE_SEQUENCER_103              0x1067
#define WM8962_WRITE_SEQUENCER_104              0x1068
#define WM8962_WRITE_SEQUENCER_105              0x1069
#define WM8962_WRITE_SEQUENCER_106              0x106A
#define WM8962_WRITE_SEQUENCER_107              0x106B
#define WM8962_WRITE_SEQUENCER_108              0x106C
#define WM8962_WRITE_SEQUENCER_109              0x106D
#define WM8962_WRITE_SEQUENCER_110              0x106E
#define WM8962_WRITE_SEQUENCER_111              0x106F
#define WM8962_WRITE_SEQUENCER_112              0x1070
#define WM8962_WRITE_SEQUENCER_113              0x1071
#define WM8962_WRITE_SEQUENCER_114              0x1072
#define WM8962_WRITE_SEQUENCER_115              0x1073
#define WM8962_WRITE_SEQUENCER_116              0x1074
#define WM8962_WRITE_SEQUENCER_117              0x1075
#define WM8962_WRITE_SEQUENCER_118              0x1076
#define WM8962_WRITE_SEQUENCER_119              0x1077
#define WM8962_WRITE_SEQUENCER_120              0x1078
#define WM8962_WRITE_SEQUENCER_121              0x1079
#define WM8962_WRITE_SEQUENCER_122              0x107A
#define WM8962_WRITE_SEQUENCER_123              0x107B
#define WM8962_WRITE_SEQUENCER_124              0x107C
#define WM8962_WRITE_SEQUENCER_125              0x107D
#define WM8962_WRITE_SEQUENCER_126              0x107E
#define WM8962_WRITE_SEQUENCER_127              0x107F
#define WM8962_WRITE_SEQUENCER_128              0x1080
#define WM8962_WRITE_SEQUENCER_129              0x1081
#define WM8962_WRITE_SEQUENCER_130              0x1082
#define WM8962_WRITE_SEQUENCER_131              0x1083
#define WM8962_WRITE_SEQUENCER_132              0x1084
#define WM8962_WRITE_SEQUENCER_133              0x1085
#define WM8962_WRITE_SEQUENCER_134              0x1086
#define WM8962_WRITE_SEQUENCER_135              0x1087
#define WM8962_WRITE_SEQUENCER_136              0x1088
#define WM8962_WRITE_SEQUENCER_137              0x1089
#define WM8962_WRITE_SEQUENCER_138              0x108A
#define WM8962_WRITE_SEQUENCER_139              0x108B
#define WM8962_WRITE_SEQUENCER_140              0x108C
#define WM8962_WRITE_SEQUENCER_141              0x108D
#define WM8962_WRITE_SEQUENCER_142              0x108E
#define WM8962_WRITE_SEQUENCER_143              0x108F
#define WM8962_WRITE_SEQUENCER_144              0x1090
#define WM8962_WRITE_SEQUENCER_145              0x1091
#define WM8962_WRITE_SEQUENCER_146              0x1092
#define WM8962_WRITE_SEQUENCER_147              0x1093
#define WM8962_WRITE_SEQUENCER_148              0x1094
#define WM8962_WRITE_SEQUENCER_149              0x1095
#define WM8962_WRITE_SEQUENCER_150              0x1096
#define WM8962_WRITE_SEQUENCER_151              0x1097
#define WM8962_WRITE_SEQUENCER_152              0x1098
#define WM8962_WRITE_SEQUENCER_153              0x1099
#define WM8962_WRITE_SEQUENCER_154              0x109A
#define WM8962_WRITE_SEQUENCER_155              0x109B
#define WM8962_WRITE_SEQUENCER_156              0x109C
#define WM8962_WRITE_SEQUENCER_157              0x109D
#define WM8962_WRITE_SEQUENCER_158              0x109E
#define WM8962_WRITE_SEQUENCER_159              0x109F
#define WM8962_WRITE_SEQUENCER_160              0x10A0
#define WM8962_WRITE_SEQUENCER_161              0x10A1
#define WM8962_WRITE_SEQUENCER_162              0x10A2
#define WM8962_WRITE_SEQUENCER_163              0x10A3
#define WM8962_WRITE_SEQUENCER_164              0x10A4
#define WM8962_WRITE_SEQUENCER_165              0x10A5
#define WM8962_WRITE_SEQUENCER_166              0x10A6
#define WM8962_WRITE_SEQUENCER_167              0x10A7
#define WM8962_WRITE_SEQUENCER_168              0x10A8
#define WM8962_WRITE_SEQUENCER_169              0x10A9
#define WM8962_WRITE_SEQUENCER_170              0x10AA
#define WM8962_WRITE_SEQUENCER_171              0x10AB
#define WM8962_WRITE_SEQUENCER_172              0x10AC
#define WM8962_WRITE_SEQUENCER_173              0x10AD
#define WM8962_WRITE_SEQUENCER_174              0x10AE
#define WM8962_WRITE_SEQUENCER_175              0x10AF
#define WM8962_WRITE_SEQUENCER_176              0x10B0
#define WM8962_WRITE_SEQUENCER_177              0x10B1
#define WM8962_WRITE_SEQUENCER_178              0x10B2
#define WM8962_WRITE_SEQUENCER_179              0x10B3
#define WM8962_WRITE_SEQUENCER_180              0x10B4
#define WM8962_WRITE_SEQUENCER_181              0x10B5
#define WM8962_WRITE_SEQUENCER_182              0x10B6
#define WM8962_WRITE_SEQUENCER_183              0x10B7
#define WM8962_WRITE_SEQUENCER_184              0x10B8
#define WM8962_WRITE_SEQUENCER_185              0x10B9
#define WM8962_WRITE_SEQUENCER_186              0x10BA
#define WM8962_WRITE_SEQUENCER_187              0x10BB
#define WM8962_WRITE_SEQUENCER_188              0x10BC
#define WM8962_WRITE_SEQUENCER_189              0x10BD
#define WM8962_WRITE_SEQUENCER_190              0x10BE
#define WM8962_WRITE_SEQUENCER_191              0x10BF
#define WM8962_WRITE_SEQUENCER_192              0x10C0
#define WM8962_WRITE_SEQUENCER_193              0x10C1
#define WM8962_WRITE_SEQUENCER_194              0x10C2
#define WM8962_WRITE_SEQUENCER_195              0x10C3
#define WM8962_WRITE_SEQUENCER_196              0x10C4
#define WM8962_WRITE_SEQUENCER_197              0x10C5
#define WM8962_WRITE_SEQUENCER_198              0x10C6
#define WM8962_WRITE_SEQUENCER_199              0x10C7
#define WM8962_WRITE_SEQUENCER_200              0x10C8
#define WM8962_WRITE_SEQUENCER_201              0x10C9
#define WM8962_WRITE_SEQUENCER_202              0x10CA
#define WM8962_WRITE_SEQUENCER_203              0x10CB
#define WM8962_WRITE_SEQUENCER_204              0x10CC
#define WM8962_WRITE_SEQUENCER_205              0x10CD
#define WM8962_WRITE_SEQUENCER_206              0x10CE
#define WM8962_WRITE_SEQUENCER_207              0x10CF
#define WM8962_WRITE_SEQUENCER_208              0x10D0
#define WM8962_WRITE_SEQUENCER_209              0x10D1
#define WM8962_WRITE_SEQUENCER_210              0x10D2
#define WM8962_WRITE_SEQUENCER_211              0x10D3
#define WM8962_WRITE_SEQUENCER_212              0x10D4
#define WM8962_WRITE_SEQUENCER_213              0x10D5
#define WM8962_WRITE_SEQUENCER_214              0x10D6
#define WM8962_WRITE_SEQUENCER_215              0x10D7
#define WM8962_WRITE_SEQUENCER_216              0x10D8
#define WM8962_WRITE_SEQUENCER_217              0x10D9
#define WM8962_WRITE_SEQUENCER_218              0x10DA
#define WM8962_WRITE_SEQUENCER_219              0x10DB
#define WM8962_WRITE_SEQUENCER_220              0x10DC
#define WM8962_WRITE_SEQUENCER_221              0x10DD
#define WM8962_WRITE_SEQUENCER_222              0x10DE
#define WM8962_WRITE_SEQUENCER_223              0x10DF
#define WM8962_WRITE_SEQUENCER_224              0x10E0
#define WM8962_WRITE_SEQUENCER_225              0x10E1
#define WM8962_WRITE_SEQUENCER_226              0x10E2
#define WM8962_WRITE_SEQUENCER_227              0x10E3
#define WM8962_WRITE_SEQUENCER_228              0x10E4
#define WM8962_WRITE_SEQUENCER_229              0x10E5
#define WM8962_WRITE_SEQUENCER_230              0x10E6
#define WM8962_WRITE_SEQUENCER_231              0x10E7
#define WM8962_WRITE_SEQUENCER_232              0x10E8
#define WM8962_WRITE_SEQUENCER_233              0x10E9
#define WM8962_WRITE_SEQUENCER_234              0x10EA
#define WM8962_WRITE_SEQUENCER_235              0x10EB
#define WM8962_WRITE_SEQUENCER_236              0x10EC
#define WM8962_WRITE_SEQUENCER_237              0x10ED
#define WM8962_WRITE_SEQUENCER_238              0x10EE
#define WM8962_WRITE_SEQUENCER_239              0x10EF
#define WM8962_WRITE_SEQUENCER_240              0x10F0
#define WM8962_WRITE_SEQUENCER_241              0x10F1
#define WM8962_WRITE_SEQUENCER_242              0x10F2
#define WM8962_WRITE_SEQUENCER_243              0x10F3
#define WM8962_WRITE_SEQUENCER_244              0x10F4
#define WM8962_WRITE_SEQUENCER_245              0x10F5
#define WM8962_WRITE_SEQUENCER_246              0x10F6
#define WM8962_WRITE_SEQUENCER_247              0x10F7
#define WM8962_WRITE_SEQUENCER_248              0x10F8
#define WM8962_WRITE_SEQUENCER_249              0x10F9
#define WM8962_WRITE_SEQUENCER_250              0x10FA
#define WM8962_WRITE_SEQUENCER_251              0x10FB
#define WM8962_WRITE_SEQUENCER_252              0x10FC
#define WM8962_WRITE_SEQUENCER_253              0x10FD
#define WM8962_WRITE_SEQUENCER_254              0x10FE
#define WM8962_WRITE_SEQUENCER_255              0x10FF
#define WM8962_WRITE_SEQUENCER_256              0x1100
#define WM8962_WRITE_SEQUENCER_257              0x1101
#define WM8962_WRITE_SEQUENCER_258              0x1102
#define WM8962_WRITE_SEQUENCER_259              0x1103
#define WM8962_WRITE_SEQUENCER_260              0x1104
#define WM8962_WRITE_SEQUENCER_261              0x1105
#define WM8962_WRITE_SEQUENCER_262              0x1106
#define WM8962_WRITE_SEQUENCER_263              0x1107
#define WM8962_WRITE_SEQUENCER_264              0x1108
#define WM8962_WRITE_SEQUENCER_265              0x1109
#define WM8962_WRITE_SEQUENCER_266              0x110A
#define WM8962_WRITE_SEQUENCER_267              0x110B
#define WM8962_WRITE_SEQUENCER_268              0x110C
#define WM8962_WRITE_SEQUENCER_269              0x110D
#define WM8962_WRITE_SEQUENCER_270              0x110E
#define WM8962_WRITE_SEQUENCER_271              0x110F
#define WM8962_WRITE_SEQUENCER_272              0x1110
#define WM8962_WRITE_SEQUENCER_273              0x1111
#define WM8962_WRITE_SEQUENCER_274              0x1112
#define WM8962_WRITE_SEQUENCER_275              0x1113
#define WM8962_WRITE_SEQUENCER_276              0x1114
#define WM8962_WRITE_SEQUENCER_277              0x1115
#define WM8962_WRITE_SEQUENCER_278              0x1116
#define WM8962_WRITE_SEQUENCER_279              0x1117
#define WM8962_WRITE_SEQUENCER_280              0x1118
#define WM8962_WRITE_SEQUENCER_281              0x1119
#define WM8962_WRITE_SEQUENCER_282              0x111A
#define WM8962_WRITE_SEQUENCER_283              0x111B
#define WM8962_WRITE_SEQUENCER_284              0x111C
#define WM8962_WRITE_SEQUENCER_285              0x111D
#define WM8962_WRITE_SEQUENCER_286              0x111E
#define WM8962_WRITE_SEQUENCER_287              0x111F
#define WM8962_WRITE_SEQUENCER_288              0x1120
#define WM8962_WRITE_SEQUENCER_289              0x1121
#define WM8962_WRITE_SEQUENCER_290              0x1122
#define WM8962_WRITE_SEQUENCER_291              0x1123
#define WM8962_WRITE_SEQUENCER_292              0x1124
#define WM8962_WRITE_SEQUENCER_293              0x1125
#define WM8962_WRITE_SEQUENCER_294              0x1126
#define WM8962_WRITE_SEQUENCER_295              0x1127
#define WM8962_WRITE_SEQUENCER_296              0x1128
#define WM8962_WRITE_SEQUENCER_297              0x1129
#define WM8962_WRITE_SEQUENCER_298              0x112A
#define WM8962_WRITE_SEQUENCER_299              0x112B
#define WM8962_WRITE_SEQUENCER_300              0x112C
#define WM8962_WRITE_SEQUENCER_301              0x112D
#define WM8962_WRITE_SEQUENCER_302              0x112E
#define WM8962_WRITE_SEQUENCER_303              0x112F
#define WM8962_WRITE_SEQUENCER_304              0x1130
#define WM8962_WRITE_SEQUENCER_305              0x1131
#define WM8962_WRITE_SEQUENCER_306              0x1132
#define WM8962_WRITE_SEQUENCER_307              0x1133
#define WM8962_WRITE_SEQUENCER_308              0x1134
#define WM8962_WRITE_SEQUENCER_309              0x1135
#define WM8962_WRITE_SEQUENCER_310              0x1136
#define WM8962_WRITE_SEQUENCER_311              0x1137
#define WM8962_WRITE_SEQUENCER_312              0x1138
#define WM8962_WRITE_SEQUENCER_313              0x1139
#define WM8962_WRITE_SEQUENCER_314              0x113A
#define WM8962_WRITE_SEQUENCER_315              0x113B
#define WM8962_WRITE_SEQUENCER_316              0x113C
#define WM8962_WRITE_SEQUENCER_317              0x113D
#define WM8962_WRITE_SEQUENCER_318              0x113E
#define WM8962_WRITE_SEQUENCER_319              0x113F
#define WM8962_WRITE_SEQUENCER_320              0x1140
#define WM8962_WRITE_SEQUENCER_321              0x1141
#define WM8962_WRITE_SEQUENCER_322              0x1142
#define WM8962_WRITE_SEQUENCER_323              0x1143
#define WM8962_WRITE_SEQUENCER_324              0x1144
#define WM8962_WRITE_SEQUENCER_325              0x1145
#define WM8962_WRITE_SEQUENCER_326              0x1146
#define WM8962_WRITE_SEQUENCER_327              0x1147
#define WM8962_WRITE_SEQUENCER_328              0x1148
#define WM8962_WRITE_SEQUENCER_329              0x1149
#define WM8962_WRITE_SEQUENCER_330              0x114A
#define WM8962_WRITE_SEQUENCER_331              0x114B
#define WM8962_WRITE_SEQUENCER_332              0x114C
#define WM8962_WRITE_SEQUENCER_333              0x114D
#define WM8962_WRITE_SEQUENCER_334              0x114E
#define WM8962_WRITE_SEQUENCER_335              0x114F
#define WM8962_WRITE_SEQUENCER_336              0x1150
#define WM8962_WRITE_SEQUENCER_337              0x1151
#define WM8962_WRITE_SEQUENCER_338              0x1152
#define WM8962_WRITE_SEQUENCER_339              0x1153
#define WM8962_WRITE_SEQUENCER_340              0x1154
#define WM8962_WRITE_SEQUENCER_341              0x1155
#define WM8962_WRITE_SEQUENCER_342              0x1156
#define WM8962_WRITE_SEQUENCER_343              0x1157
#define WM8962_WRITE_SEQUENCER_344              0x1158
#define WM8962_WRITE_SEQUENCER_345              0x1159
#define WM8962_WRITE_SEQUENCER_346              0x115A
#define WM8962_WRITE_SEQUENCER_347              0x115B
#define WM8962_WRITE_SEQUENCER_348              0x115C
#define WM8962_WRITE_SEQUENCER_349              0x115D
#define WM8962_WRITE_SEQUENCER_350              0x115E
#define WM8962_WRITE_SEQUENCER_351              0x115F
#define WM8962_WRITE_SEQUENCER_352              0x1160
#define WM8962_WRITE_SEQUENCER_353              0x1161
#define WM8962_WRITE_SEQUENCER_354              0x1162
#define WM8962_WRITE_SEQUENCER_355              0x1163
#define WM8962_WRITE_SEQUENCER_356              0x1164
#define WM8962_WRITE_SEQUENCER_357              0x1165
#define WM8962_WRITE_SEQUENCER_358              0x1166
#define WM8962_WRITE_SEQUENCER_359              0x1167
#define WM8962_WRITE_SEQUENCER_360              0x1168
#define WM8962_WRITE_SEQUENCER_361              0x1169
#define WM8962_WRITE_SEQUENCER_362              0x116A
#define WM8962_WRITE_SEQUENCER_363              0x116B
#define WM8962_WRITE_SEQUENCER_364              0x116C
#define WM8962_WRITE_SEQUENCER_365              0x116D
#define WM8962_WRITE_SEQUENCER_366              0x116E
#define WM8962_WRITE_SEQUENCER_367              0x116F
#define WM8962_WRITE_SEQUENCER_368              0x1170
#define WM8962_WRITE_SEQUENCER_369              0x1171
#define WM8962_WRITE_SEQUENCER_370              0x1172
#define WM8962_WRITE_SEQUENCER_371              0x1173
#define WM8962_WRITE_SEQUENCER_372              0x1174
#define WM8962_WRITE_SEQUENCER_373              0x1175
#define WM8962_WRITE_SEQUENCER_374              0x1176
#define WM8962_WRITE_SEQUENCER_375              0x1177
#define WM8962_WRITE_SEQUENCER_376              0x1178
#define WM8962_WRITE_SEQUENCER_377              0x1179
#define WM8962_WRITE_SEQUENCER_378              0x117A
#define WM8962_WRITE_SEQUENCER_379              0x117B
#define WM8962_WRITE_SEQUENCER_380              0x117C
#define WM8962_WRITE_SEQUENCER_381              0x117D
#define WM8962_WRITE_SEQUENCER_382              0x117E
#define WM8962_WRITE_SEQUENCER_383              0x117F
#define WM8962_WRITE_SEQUENCER_384              0x1180
#define WM8962_WRITE_SEQUENCER_385              0x1181
#define WM8962_WRITE_SEQUENCER_386              0x1182
#define WM8962_WRITE_SEQUENCER_387              0x1183
#define WM8962_WRITE_SEQUENCER_388              0x1184
#define WM8962_WRITE_SEQUENCER_389              0x1185
#define WM8962_WRITE_SEQUENCER_390              0x1186
#define WM8962_WRITE_SEQUENCER_391              0x1187
#define WM8962_WRITE_SEQUENCER_392              0x1188
#define WM8962_WRITE_SEQUENCER_393              0x1189
#define WM8962_WRITE_SEQUENCER_394              0x118A
#define WM8962_WRITE_SEQUENCER_395              0x118B
#define WM8962_WRITE_SEQUENCER_396              0x118C
#define WM8962_WRITE_SEQUENCER_397              0x118D
#define WM8962_WRITE_SEQUENCER_398              0x118E
#define WM8962_WRITE_SEQUENCER_399              0x118F
#define WM8962_WRITE_SEQUENCER_400              0x1190
#define WM8962_WRITE_SEQUENCER_401              0x1191
#define WM8962_WRITE_SEQUENCER_402              0x1192
#define WM8962_WRITE_SEQUENCER_403              0x1193
#define WM8962_WRITE_SEQUENCER_404              0x1194
#define WM8962_WRITE_SEQUENCER_405              0x1195
#define WM8962_WRITE_SEQUENCER_406              0x1196
#define WM8962_WRITE_SEQUENCER_407              0x1197
#define WM8962_WRITE_SEQUENCER_408              0x1198
#define WM8962_WRITE_SEQUENCER_409              0x1199
#define WM8962_WRITE_SEQUENCER_410              0x119A
#define WM8962_WRITE_SEQUENCER_411              0x119B
#define WM8962_WRITE_SEQUENCER_412              0x119C
#define WM8962_WRITE_SEQUENCER_413              0x119D
#define WM8962_WRITE_SEQUENCER_414              0x119E
#define WM8962_WRITE_SEQUENCER_415              0x119F
#define WM8962_WRITE_SEQUENCER_416              0x11A0
#define WM8962_WRITE_SEQUENCER_417              0x11A1
#define WM8962_WRITE_SEQUENCER_418              0x11A2
#define WM8962_WRITE_SEQUENCER_419              0x11A3
#define WM8962_WRITE_SEQUENCER_420              0x11A4
#define WM8962_WRITE_SEQUENCER_421              0x11A5
#define WM8962_WRITE_SEQUENCER_422              0x11A6
#define WM8962_WRITE_SEQUENCER_423              0x11A7
#define WM8962_WRITE_SEQUENCER_424              0x11A8
#define WM8962_WRITE_SEQUENCER_425              0x11A9
#define WM8962_WRITE_SEQUENCER_426              0x11AA
#define WM8962_WRITE_SEQUENCER_427              0x11AB
#define WM8962_WRITE_SEQUENCER_428              0x11AC
#define WM8962_WRITE_SEQUENCER_429              0x11AD
#define WM8962_WRITE_SEQUENCER_430              0x11AE
#define WM8962_WRITE_SEQUENCER_431              0x11AF
#define WM8962_WRITE_SEQUENCER_432              0x11B0
#define WM8962_WRITE_SEQUENCER_433              0x11B1
#define WM8962_WRITE_SEQUENCER_434              0x11B2
#define WM8962_WRITE_SEQUENCER_435              0x11B3
#define WM8962_WRITE_SEQUENCER_436              0x11B4
#define WM8962_WRITE_SEQUENCER_437              0x11B5
#define WM8962_WRITE_SEQUENCER_438              0x11B6
#define WM8962_WRITE_SEQUENCER_439              0x11B7
#define WM8962_WRITE_SEQUENCER_440              0x11B8
#define WM8962_WRITE_SEQUENCER_441              0x11B9
#define WM8962_WRITE_SEQUENCER_442              0x11BA
#define WM8962_WRITE_SEQUENCER_443              0x11BB
#define WM8962_WRITE_SEQUENCER_444              0x11BC
#define WM8962_WRITE_SEQUENCER_445              0x11BD
#define WM8962_WRITE_SEQUENCER_446              0x11BE
#define WM8962_WRITE_SEQUENCER_447              0x11BF
#define WM8962_WRITE_SEQUENCER_448              0x11C0
#define WM8962_WRITE_SEQUENCER_449              0x11C1
#define WM8962_WRITE_SEQUENCER_450              0x11C2
#define WM8962_WRITE_SEQUENCER_451              0x11C3
#define WM8962_WRITE_SEQUENCER_452              0x11C4
#define WM8962_WRITE_SEQUENCER_453              0x11C5
#define WM8962_WRITE_SEQUENCER_454              0x11C6
#define WM8962_WRITE_SEQUENCER_455              0x11C7
#define WM8962_WRITE_SEQUENCER_456              0x11C8
#define WM8962_WRITE_SEQUENCER_457              0x11C9
#define WM8962_WRITE_SEQUENCER_458              0x11CA
#define WM8962_WRITE_SEQUENCER_459              0x11CB
#define WM8962_WRITE_SEQUENCER_460              0x11CC
#define WM8962_WRITE_SEQUENCER_461              0x11CD
#define WM8962_WRITE_SEQUENCER_462              0x11CE
#define WM8962_WRITE_SEQUENCER_463              0x11CF
#define WM8962_WRITE_SEQUENCER_464              0x11D0
#define WM8962_WRITE_SEQUENCER_465              0x11D1
#define WM8962_WRITE_SEQUENCER_466              0x11D2
#define WM8962_WRITE_SEQUENCER_467              0x11D3
#define WM8962_WRITE_SEQUENCER_468              0x11D4
#define WM8962_WRITE_SEQUENCER_469              0x11D5
#define WM8962_WRITE_SEQUENCER_470              0x11D6
#define WM8962_WRITE_SEQUENCER_471              0x11D7
#define WM8962_WRITE_SEQUENCER_472              0x11D8
#define WM8962_WRITE_SEQUENCER_473              0x11D9
#define WM8962_WRITE_SEQUENCER_474              0x11DA
#define WM8962_WRITE_SEQUENCER_475              0x11DB
#define WM8962_WRITE_SEQUENCER_476              0x11DC
#define WM8962_WRITE_SEQUENCER_477              0x11DD
#define WM8962_WRITE_SEQUENCER_478              0x11DE
#define WM8962_WRITE_SEQUENCER_479              0x11DF
#define WM8962_WRITE_SEQUENCER_480              0x11E0
#define WM8962_WRITE_SEQUENCER_481              0x11E1
#define WM8962_WRITE_SEQUENCER_482              0x11E2
#define WM8962_WRITE_SEQUENCER_483              0x11E3
#define WM8962_WRITE_SEQUENCER_484              0x11E4
#define WM8962_WRITE_SEQUENCER_485              0x11E5
#define WM8962_WRITE_SEQUENCER_486              0x11E6
#define WM8962_WRITE_SEQUENCER_487              0x11E7
#define WM8962_WRITE_SEQUENCER_488              0x11E8
#define WM8962_WRITE_SEQUENCER_489              0x11E9
#define WM8962_WRITE_SEQUENCER_490              0x11EA
#define WM8962_WRITE_SEQUENCER_491              0x11EB
#define WM8962_WRITE_SEQUENCER_492              0x11EC
#define WM8962_WRITE_SEQUENCER_493              0x11ED
#define WM8962_WRITE_SEQUENCER_494              0x11EE
#define WM8962_WRITE_SEQUENCER_495              0x11EF
#define WM8962_WRITE_SEQUENCER_496              0x11F0
#define WM8962_WRITE_SEQUENCER_497              0x11F1
#define WM8962_WRITE_SEQUENCER_498              0x11F2
#define WM8962_WRITE_SEQUENCER_499              0x11F3
#define WM8962_WRITE_SEQUENCER_500              0x11F4
#define WM8962_WRITE_SEQUENCER_501              0x11F5
#define WM8962_WRITE_SEQUENCER_502              0x11F6
#define WM8962_WRITE_SEQUENCER_503              0x11F7
#define WM8962_WRITE_SEQUENCER_504              0x11F8
#define WM8962_WRITE_SEQUENCER_505              0x11F9
#define WM8962_WRITE_SEQUENCER_506              0x11FA
#define WM8962_WRITE_SEQUENCER_507              0x11FB
#define WM8962_WRITE_SEQUENCER_508              0x11FC
#define WM8962_WRITE_SEQUENCER_509              0x11FD
#define WM8962_WRITE_SEQUENCER_510              0x11FE
#define WM8962_WRITE_SEQUENCER_511              0x11FF
#define WM8962_DSP2_INSTRUCTION_RAM_0           0x2000
#define WM8962_DSP2_ADDRESS_RAM_2               0x2400
#define WM8962_DSP2_ADDRESS_RAM_1               0x2401
#define WM8962_DSP2_ADDRESS_RAM_0               0x2402
#define WM8962_DSP2_DATA1_RAM_1                 0x3000
#define WM8962_DSP2_DATA1_RAM_0                 0x3001
#define WM8962_DSP2_DATA2_RAM_1                 0x3400
#define WM8962_DSP2_DATA2_RAM_0                 0x3401
#define WM8962_DSP2_DATA3_RAM_1                 0x3800
#define WM8962_DSP2_DATA3_RAM_0                 0x3801
#define WM8962_DSP2_COEFF_RAM_0                 0x3C00
#define WM8962_RETUNEADC_SHARED_COEFF_1         0x4000
#define WM8962_RETUNEADC_SHARED_COEFF_0         0x4001
#define WM8962_RETUNEDAC_SHARED_COEFF_1         0x4002
#define WM8962_RETUNEDAC_SHARED_COEFF_0         0x4003
#define WM8962_SOUNDSTAGE_ENABLES_1             0x4004
#define WM8962_SOUNDSTAGE_ENABLES_0             0x4005
#define WM8962_HDBASS_AI_1                      0x4200
#define WM8962_HDBASS_AI_0                      0x4201
#define WM8962_HDBASS_AR_1                      0x4202
#define WM8962_HDBASS_AR_0                      0x4203
#define WM8962_HDBASS_B_1                       0x4204
#define WM8962_HDBASS_B_0                       0x4205
#define WM8962_HDBASS_K_1                       0x4206
#define WM8962_HDBASS_K_0                       0x4207
#define WM8962_HDBASS_N1_1                      0x4208
#define WM8962_HDBASS_N1_0                      0x4209
#define WM8962_HDBASS_N2_1                      0x420A
#define WM8962_HDBASS_N2_0                      0x420B
#define WM8962_HDBASS_N3_1                      0x420C
#define WM8962_HDBASS_N3_0                      0x420D
#define WM8962_HDBASS_N4_1                      0x420E
#define WM8962_HDBASS_N4_0                      0x420F
#define WM8962_HDBASS_N5_1                      0x4210
#define WM8962_HDBASS_N5_0                      0x4211
#define WM8962_HDBASS_X1_1                      0x4212
#define WM8962_HDBASS_X1_0                      0x4213
#define WM8962_HDBASS_X2_1                      0x4214
#define WM8962_HDBASS_X2_0                      0x4215
#define WM8962_HDBASS_X3_1                      0x4216
#define WM8962_HDBASS_X3_0                      0x4217
#define WM8962_HDBASS_ATK_1                     0x4218
#define WM8962_HDBASS_ATK_0                     0x4219
#define WM8962_HDBASS_DCY_1                     0x421A
#define WM8962_HDBASS_DCY_0                     0x421B
#define WM8962_HDBASS_PG_1                      0x421C
#define WM8962_HDBASS_PG_0                      0x421D
#define WM8962_HPF_C_1                          0x4400
#define WM8962_HPF_C_0                          0x4401
#define WM8962_ADCL_RETUNE_C1_1                 0x4600
#define WM8962_ADCL_RETUNE_C1_0                 0x4601
#define WM8962_ADCL_RETUNE_C2_1                 0x4602
#define WM8962_ADCL_RETUNE_C2_0                 0x4603
#define WM8962_ADCL_RETUNE_C3_1                 0x4604
#define WM8962_ADCL_RETUNE_C3_0                 0x4605
#define WM8962_ADCL_RETUNE_C4_1                 0x4606
#define WM8962_ADCL_RETUNE_C4_0                 0x4607
#define WM8962_ADCL_RETUNE_C5_1                 0x4608
#define WM8962_ADCL_RETUNE_C5_0                 0x4609
#define WM8962_ADCL_RETUNE_C6_1                 0x460A
#define WM8962_ADCL_RETUNE_C6_0                 0x460B
#define WM8962_ADCL_RETUNE_C7_1                 0x460C
#define WM8962_ADCL_RETUNE_C7_0                 0x460D
#define WM8962_ADCL_RETUNE_C8_1                 0x460E
#define WM8962_ADCL_RETUNE_C8_0                 0x460F
#define WM8962_ADCL_RETUNE_C9_1                 0x4610
#define WM8962_ADCL_RETUNE_C9_0                 0x4611
#define WM8962_ADCL_RETUNE_C10_1                0x4612
#define WM8962_ADCL_RETUNE_C10_0                0x4613
#define WM8962_ADCL_RETUNE_C11_1                0x4614
#define WM8962_ADCL_RETUNE_C11_0                0x4615
#define WM8962_ADCL_RETUNE_C12_1                0x4616
#define WM8962_ADCL_RETUNE_C12_0                0x4617
#define WM8962_ADCL_RETUNE_C13_1                0x4618
#define WM8962_ADCL_RETUNE_C13_0                0x4619
#define WM8962_ADCL_RETUNE_C14_1                0x461A
#define WM8962_ADCL_RETUNE_C14_0                0x461B
#define WM8962_ADCL_RETUNE_C15_1                0x461C
#define WM8962_ADCL_RETUNE_C15_0                0x461D
#define WM8962_ADCL_RETUNE_C16_1                0x461E
#define WM8962_ADCL_RETUNE_C16_0                0x461F
#define WM8962_ADCL_RETUNE_C17_1                0x4620
#define WM8962_ADCL_RETUNE_C17_0                0x4621
#define WM8962_ADCL_RETUNE_C18_1                0x4622
#define WM8962_ADCL_RETUNE_C18_0                0x4623
#define WM8962_ADCL_RETUNE_C19_1                0x4624
#define WM8962_ADCL_RETUNE_C19_0                0x4625
#define WM8962_ADCL_RETUNE_C20_1                0x4626
#define WM8962_ADCL_RETUNE_C20_0                0x4627
#define WM8962_ADCL_RETUNE_C21_1                0x4628
#define WM8962_ADCL_RETUNE_C21_0                0x4629
#define WM8962_ADCL_RETUNE_C22_1                0x462A
#define WM8962_ADCL_RETUNE_C22_0                0x462B
#define WM8962_ADCL_RETUNE_C23_1                0x462C
#define WM8962_ADCL_RETUNE_C23_0                0x462D
#define WM8962_ADCL_RETUNE_C24_1                0x462E
#define WM8962_ADCL_RETUNE_C24_0                0x462F
#define WM8962_ADCL_RETUNE_C25_1                0x4630
#define WM8962_ADCL_RETUNE_C25_0                0x4631
#define WM8962_ADCL_RETUNE_C26_1                0x4632
#define WM8962_ADCL_RETUNE_C26_0                0x4633
#define WM8962_ADCL_RETUNE_C27_1                0x4634
#define WM8962_ADCL_RETUNE_C27_0                0x4635
#define WM8962_ADCL_RETUNE_C28_1                0x4636
#define WM8962_ADCL_RETUNE_C28_0                0x4637
#define WM8962_ADCL_RETUNE_C29_1                0x4638
#define WM8962_ADCL_RETUNE_C29_0                0x4639
#define WM8962_ADCL_RETUNE_C30_1                0x463A
#define WM8962_ADCL_RETUNE_C30_0                0x463B
#define WM8962_ADCL_RETUNE_C31_1                0x463C
#define WM8962_ADCL_RETUNE_C31_0                0x463D
#define WM8962_ADCL_RETUNE_C32_1                0x463E
#define WM8962_ADCL_RETUNE_C32_0                0x463F
#define WM8962_RETUNEADC_PG2_1                  0x4800
#define WM8962_RETUNEADC_PG2_0                  0x4801
#define WM8962_RETUNEADC_PG_1                   0x4802
#define WM8962_RETUNEADC_PG_0                   0x4803
#define WM8962_ADCR_RETUNE_C1_1                 0x4A00
#define WM8962_ADCR_RETUNE_C1_0                 0x4A01
#define WM8962_ADCR_RETUNE_C2_1                 0x4A02
#define WM8962_ADCR_RETUNE_C2_0                 0x4A03
#define WM8962_ADCR_RETUNE_C3_1                 0x4A04
#define WM8962_ADCR_RETUNE_C3_0                 0x4A05
#define WM8962_ADCR_RETUNE_C4_1                 0x4A06
#define WM8962_ADCR_RETUNE_C4_0                 0x4A07
#define WM8962_ADCR_RETUNE_C5_1                 0x4A08
#define WM8962_ADCR_RETUNE_C5_0                 0x4A09
#define WM8962_ADCR_RETUNE_C6_1                 0x4A0A
#define WM8962_ADCR_RETUNE_C6_0                 0x4A0B
#define WM8962_ADCR_RETUNE_C7_1                 0x4A0C
#define WM8962_ADCR_RETUNE_C7_0                 0x4A0D
#define WM8962_ADCR_RETUNE_C8_1                 0x4A0E
#define WM8962_ADCR_RETUNE_C8_0                 0x4A0F
#define WM8962_ADCR_RETUNE_C9_1                 0x4A10
#define WM8962_ADCR_RETUNE_C9_0                 0x4A11
#define WM8962_ADCR_RETUNE_C10_1                0x4A12
#define WM8962_ADCR_RETUNE_C10_0                0x4A13
#define WM8962_ADCR_RETUNE_C11_1                0x4A14
#define WM8962_ADCR_RETUNE_C11_0                0x4A15
#define WM8962_ADCR_RETUNE_C12_1                0x4A16
#define WM8962_ADCR_RETUNE_C12_0                0x4A17
#define WM8962_ADCR_RETUNE_C13_1                0x4A18
#define WM8962_ADCR_RETUNE_C13_0                0x4A19
#define WM8962_ADCR_RETUNE_C14_1                0x4A1A
#define WM8962_ADCR_RETUNE_C14_0                0x4A1B
#define WM8962_ADCR_RETUNE_C15_1                0x4A1C
#define WM8962_ADCR_RETUNE_C15_0                0x4A1D
#define WM8962_ADCR_RETUNE_C16_1                0x4A1E
#define WM8962_ADCR_RETUNE_C16_0                0x4A1F
#define WM8962_ADCR_RETUNE_C17_1                0x4A20
#define WM8962_ADCR_RETUNE_C17_0                0x4A21
#define WM8962_ADCR_RETUNE_C18_1                0x4A22
#define WM8962_ADCR_RETUNE_C18_0                0x4A23
#define WM8962_ADCR_RETUNE_C19_1                0x4A24
#define WM8962_ADCR_RETUNE_C19_0                0x4A25
#define WM8962_ADCR_RETUNE_C20_1                0x4A26
#define WM8962_ADCR_RETUNE_C20_0                0x4A27
#define WM8962_ADCR_RETUNE_C21_1                0x4A28
#define WM8962_ADCR_RETUNE_C21_0                0x4A29
#define WM8962_ADCR_RETUNE_C22_1                0x4A2A
#define WM8962_ADCR_RETUNE_C22_0                0x4A2B
#define WM8962_ADCR_RETUNE_C23_1                0x4A2C
#define WM8962_ADCR_RETUNE_C23_0                0x4A2D
#define WM8962_ADCR_RETUNE_C24_1                0x4A2E
#define WM8962_ADCR_RETUNE_C24_0                0x4A2F
#define WM8962_ADCR_RETUNE_C25_1                0x4A30
#define WM8962_ADCR_RETUNE_C25_0                0x4A31
#define WM8962_ADCR_RETUNE_C26_1                0x4A32
#define WM8962_ADCR_RETUNE_C26_0                0x4A33
#define WM8962_ADCR_RETUNE_C27_1                0x4A34
#define WM8962_ADCR_RETUNE_C27_0                0x4A35
#define WM8962_ADCR_RETUNE_C28_1                0x4A36
#define WM8962_ADCR_RETUNE_C28_0                0x4A37
#define WM8962_ADCR_RETUNE_C29_1                0x4A38
#define WM8962_ADCR_RETUNE_C29_0                0x4A39
#define WM8962_ADCR_RETUNE_C30_1                0x4A3A
#define WM8962_ADCR_RETUNE_C30_0                0x4A3B
#define WM8962_ADCR_RETUNE_C31_1                0x4A3C
#define WM8962_ADCR_RETUNE_C31_0                0x4A3D
#define WM8962_ADCR_RETUNE_C32_1                0x4A3E
#define WM8962_ADCR_RETUNE_C32_0                0x4A3F
#define WM8962_DACL_RETUNE_C1_1                 0x4C00
#define WM8962_DACL_RETUNE_C1_0                 0x4C01
#define WM8962_DACL_RETUNE_C2_1                 0x4C02
#define WM8962_DACL_RETUNE_C2_0                 0x4C03
#define WM8962_DACL_RETUNE_C3_1                 0x4C04
#define WM8962_DACL_RETUNE_C3_0                 0x4C05
#define WM8962_DACL_RETUNE_C4_1                 0x4C06
#define WM8962_DACL_RETUNE_C4_0                 0x4C07
#define WM8962_DACL_RETUNE_C5_1                 0x4C08
#define WM8962_DACL_RETUNE_C5_0                 0x4C09
#define WM8962_DACL_RETUNE_C6_1                 0x4C0A
#define WM8962_DACL_RETUNE_C6_0                 0x4C0B
#define WM8962_DACL_RETUNE_C7_1                 0x4C0C
#define WM8962_DACL_RETUNE_C7_0                 0x4C0D
#define WM8962_DACL_RETUNE_C8_1                 0x4C0E
#define WM8962_DACL_RETUNE_C8_0                 0x4C0F
#define WM8962_DACL_RETUNE_C9_1                 0x4C10
#define WM8962_DACL_RETUNE_C9_0                 0x4C11
#define WM8962_DACL_RETUNE_C10_1                0x4C12
#define WM8962_DACL_RETUNE_C10_0                0x4C13
#define WM8962_DACL_RETUNE_C11_1                0x4C14
#define WM8962_DACL_RETUNE_C11_0                0x4C15
#define WM8962_DACL_RETUNE_C12_1                0x4C16
#define WM8962_DACL_RETUNE_C12_0                0x4C17
#define WM8962_DACL_RETUNE_C13_1                0x4C18
#define WM8962_DACL_RETUNE_C13_0                0x4C19
#define WM8962_DACL_RETUNE_C14_1                0x4C1A
#define WM8962_DACL_RETUNE_C14_0                0x4C1B
#define WM8962_DACL_RETUNE_C15_1                0x4C1C
#define WM8962_DACL_RETUNE_C15_0                0x4C1D
#define WM8962_DACL_RETUNE_C16_1                0x4C1E
#define WM8962_DACL_RETUNE_C16_0                0x4C1F
#define WM8962_DACL_RETUNE_C17_1                0x4C20
#define WM8962_DACL_RETUNE_C17_0                0x4C21
#define WM8962_DACL_RETUNE_C18_1                0x4C22
#define WM8962_DACL_RETUNE_C18_0                0x4C23
#define WM8962_DACL_RETUNE_C19_1                0x4C24
#define WM8962_DACL_RETUNE_C19_0                0x4C25
#define WM8962_DACL_RETUNE_C20_1                0x4C26
#define WM8962_DACL_RETUNE_C20_0                0x4C27
#define WM8962_DACL_RETUNE_C21_1                0x4C28
#define WM8962_DACL_RETUNE_C21_0                0x4C29
#define WM8962_DACL_RETUNE_C22_1                0x4C2A
#define WM8962_DACL_RETUNE_C22_0                0x4C2B
#define WM8962_DACL_RETUNE_C23_1                0x4C2C
#define WM8962_DACL_RETUNE_C23_0                0x4C2D
#define WM8962_DACL_RETUNE_C24_1                0x4C2E
#define WM8962_DACL_RETUNE_C24_0                0x4C2F
#define WM8962_DACL_RETUNE_C25_1                0x4C30
#define WM8962_DACL_RETUNE_C25_0                0x4C31
#define WM8962_DACL_RETUNE_C26_1                0x4C32
#define WM8962_DACL_RETUNE_C26_0                0x4C33
#define WM8962_DACL_RETUNE_C27_1                0x4C34
#define WM8962_DACL_RETUNE_C27_0                0x4C35
#define WM8962_DACL_RETUNE_C28_1                0x4C36
#define WM8962_DACL_RETUNE_C28_0                0x4C37
#define WM8962_DACL_RETUNE_C29_1                0x4C38
#define WM8962_DACL_RETUNE_C29_0                0x4C39
#define WM8962_DACL_RETUNE_C30_1                0x4C3A
#define WM8962_DACL_RETUNE_C30_0                0x4C3B
#define WM8962_DACL_RETUNE_C31_1                0x4C3C
#define WM8962_DACL_RETUNE_C31_0                0x4C3D
#define WM8962_DACL_RETUNE_C32_1                0x4C3E
#define WM8962_DACL_RETUNE_C32_0                0x4C3F
#define WM8962_RETUNEDAC_PG2_1                  0x4E00
#define WM8962_RETUNEDAC_PG2_0                  0x4E01
#define WM8962_RETUNEDAC_PG_1                   0x4E02
#define WM8962_RETUNEDAC_PG_0                   0x4E03
#define WM8962_DACR_RETUNE_C1_1                 0x5000
#define WM8962_DACR_RETUNE_C1_0                 0x5001
#define WM8962_DACR_RETUNE_C2_1                 0x5002
#define WM8962_DACR_RETUNE_C2_0                 0x5003
#define WM8962_DACR_RETUNE_C3_1                 0x5004
#define WM8962_DACR_RETUNE_C3_0                 0x5005
#define WM8962_DACR_RETUNE_C4_1                 0x5006
#define WM8962_DACR_RETUNE_C4_0                 0x5007
#define WM8962_DACR_RETUNE_C5_1                 0x5008
#define WM8962_DACR_RETUNE_C5_0                 0x5009
#define WM8962_DACR_RETUNE_C6_1                 0x500A
#define WM8962_DACR_RETUNE_C6_0                 0x500B
#define WM8962_DACR_RETUNE_C7_1                 0x500C
#define WM8962_DACR_RETUNE_C7_0                 0x500D
#define WM8962_DACR_RETUNE_C8_1                 0x500E
#define WM8962_DACR_RETUNE_C8_0                 0x500F
#define WM8962_DACR_RETUNE_C9_1                 0x5010
#define WM8962_DACR_RETUNE_C9_0                 0x5011
#define WM8962_DACR_RETUNE_C10_1                0x5012
#define WM8962_DACR_RETUNE_C10_0                0x5013
#define WM8962_DACR_RETUNE_C11_1                0x5014
#define WM8962_DACR_RETUNE_C11_0                0x5015
#define WM8962_DACR_RETUNE_C12_1                0x5016
#define WM8962_DACR_RETUNE_C12_0                0x5017
#define WM8962_DACR_RETUNE_C13_1                0x5018
#define WM8962_DACR_RETUNE_C13_0                0x5019
#define WM8962_DACR_RETUNE_C14_1                0x501A
#define WM8962_DACR_RETUNE_C14_0                0x501B
#define WM8962_DACR_RETUNE_C15_1                0x501C
#define WM8962_DACR_RETUNE_C15_0                0x501D
#define WM8962_DACR_RETUNE_C16_1                0x501E
#define WM8962_DACR_RETUNE_C16_0                0x501F
#define WM8962_DACR_RETUNE_C17_1                0x5020
#define WM8962_DACR_RETUNE_C17_0                0x5021
#define WM8962_DACR_RETUNE_C18_1                0x5022
#define WM8962_DACR_RETUNE_C18_0                0x5023
#define WM8962_DACR_RETUNE_C19_1                0x5024
#define WM8962_DACR_RETUNE_C19_0                0x5025
#define WM8962_DACR_RETUNE_C20_1                0x5026
#define WM8962_DACR_RETUNE_C20_0                0x5027
#define WM8962_DACR_RETUNE_C21_1                0x5028
#define WM8962_DACR_RETUNE_C21_0                0x5029
#define WM8962_DACR_RETUNE_C22_1                0x502A
#define WM8962_DACR_RETUNE_C22_0                0x502B
#define WM8962_DACR_RETUNE_C23_1                0x502C
#define WM8962_DACR_RETUNE_C23_0                0x502D
#define WM8962_DACR_RETUNE_C24_1                0x502E
#define WM8962_DACR_RETUNE_C24_0                0x502F
#define WM8962_DACR_RETUNE_C25_1                0x5030
#define WM8962_DACR_RETUNE_C25_0                0x5031
#define WM8962_DACR_RETUNE_C26_1                0x5032
#define WM8962_DACR_RETUNE_C26_0                0x5033
#define WM8962_DACR_RETUNE_C27_1                0x5034
#define WM8962_DACR_RETUNE_C27_0                0x5035
#define WM8962_DACR_RETUNE_C28_1                0x5036
#define WM8962_DACR_RETUNE_C28_0                0x5037
#define WM8962_DACR_RETUNE_C29_1                0x5038
#define WM8962_DACR_RETUNE_C29_0                0x5039
#define WM8962_DACR_RETUNE_C30_1                0x503A
#define WM8962_DACR_RETUNE_C30_0                0x503B
#define WM8962_DACR_RETUNE_C31_1                0x503C
#define WM8962_DACR_RETUNE_C31_0                0x503D
#define WM8962_DACR_RETUNE_C32_1                0x503E
#define WM8962_DACR_RETUNE_C32_0                0x503F
#define WM8962_VSS_XHD2_1                       0x5200
#define WM8962_VSS_XHD2_0                       0x5201
#define WM8962_VSS_XHD3_1                       0x5202
#define WM8962_VSS_XHD3_0                       0x5203
#define WM8962_VSS_XHN1_1                       0x5204
#define WM8962_VSS_XHN1_0                       0x5205
#define WM8962_VSS_XHN2_1                       0x5206
#define WM8962_VSS_XHN2_0                       0x5207
#define WM8962_VSS_XHN3_1                       0x5208
#define WM8962_VSS_XHN3_0                       0x5209
#define WM8962_VSS_XLA_1                        0x520A
#define WM8962_VSS_XLA_0                        0x520B
#define WM8962_VSS_XLB_1                        0x520C
#define WM8962_VSS_XLB_0                        0x520D
#define WM8962_VSS_XLG_1                        0x520E
#define WM8962_VSS_XLG_0                        0x520F
#define WM8962_VSS_PG2_1                        0x5210
#define WM8962_VSS_PG2_0                        0x5211
#define WM8962_VSS_PG_1                         0x5212
#define WM8962_VSS_PG_0                         0x5213
#define WM8962_VSS_XTD1_1                       0x5214
#define WM8962_VSS_XTD1_0                       0x5215
#define WM8962_VSS_XTD2_1                       0x5216
#define WM8962_VSS_XTD2_0                       0x5217
#define WM8962_VSS_XTD3_1                       0x5218
#define WM8962_VSS_XTD3_0                       0x5219
#define WM8962_VSS_XTD4_1                       0x521A
#define WM8962_VSS_XTD4_0                       0x521B
#define WM8962_VSS_XTD5_1                       0x521C
#define WM8962_VSS_XTD5_0                       0x521D
#define WM8962_VSS_XTD6_1                       0x521E
#define WM8962_VSS_XTD6_0                       0x521F
#define WM8962_VSS_XTD7_1                       0x5220
#define WM8962_VSS_XTD7_0                       0x5221
#define WM8962_VSS_XTD8_1                       0x5222
#define WM8962_VSS_XTD8_0                       0x5223
#define WM8962_VSS_XTD9_1                       0x5224
#define WM8962_VSS_XTD9_0                       0x5225
#define WM8962_VSS_XTD10_1                      0x5226
#define WM8962_VSS_XTD10_0                      0x5227
#define WM8962_VSS_XTD11_1                      0x5228
#define WM8962_VSS_XTD11_0                      0x5229
#define WM8962_VSS_XTD12_1                      0x522A
#define WM8962_VSS_XTD12_0                      0x522B
#define WM8962_VSS_XTD13_1                      0x522C
#define WM8962_VSS_XTD13_0                      0x522D
#define WM8962_VSS_XTD14_1                      0x522E
#define WM8962_VSS_XTD14_0                      0x522F
#define WM8962_VSS_XTD15_1                      0x5230
#define WM8962_VSS_XTD15_0                      0x5231
#define WM8962_VSS_XTD16_1                      0x5232
#define WM8962_VSS_XTD16_0                      0x5233
#define WM8962_VSS_XTD17_1                      0x5234
#define WM8962_VSS_XTD17_0                      0x5235
#define WM8962_VSS_XTD18_1                      0x5236
#define WM8962_VSS_XTD18_0                      0x5237
#define WM8962_VSS_XTD19_1                      0x5238
#define WM8962_VSS_XTD19_0                      0x5239
#define WM8962_VSS_XTD20_1                      0x523A
#define WM8962_VSS_XTD20_0                      0x523B
#define WM8962_VSS_XTD21_1                      0x523C
#define WM8962_VSS_XTD21_0                      0x523D
#define WM8962_VSS_XTD22_1                      0x523E
#define WM8962_VSS_XTD22_0                      0x523F
#define WM8962_VSS_XTD23_1                      0x5240
#define WM8962_VSS_XTD23_0                      0x5241
#define WM8962_VSS_XTD24_1                      0x5242
#define WM8962_VSS_XTD24_0                      0x5243
#define WM8962_VSS_XTD25_1                      0x5244
#define WM8962_VSS_XTD25_0                      0x5245
#define WM8962_VSS_XTD26_1                      0x5246
#define WM8962_VSS_XTD26_0                      0x5247
#define WM8962_VSS_XTD27_1                      0x5248
#define WM8962_VSS_XTD27_0                      0x5249
#define WM8962_VSS_XTD28_1                      0x524A
#define WM8962_VSS_XTD28_0                      0x524B
#define WM8962_VSS_XTD29_1                      0x524C
#define WM8962_VSS_XTD29_0                      0x524D
#define WM8962_VSS_XTD30_1                      0x524E
#define WM8962_VSS_XTD30_0                      0x524F
#define WM8962_VSS_XTD31_1                      0x5250
#define WM8962_VSS_XTD31_0                      0x5251
#define WM8962_VSS_XTD32_1                      0x5252
#define WM8962_VSS_XTD32_0                      0x5253
#define WM8962_VSS_XTS1_1                       0x5254
#define WM8962_VSS_XTS1_0                       0x5255
#define WM8962_VSS_XTS2_1                       0x5256
#define WM8962_VSS_XTS2_0                       0x5257
#define WM8962_VSS_XTS3_1                       0x5258
#define WM8962_VSS_XTS3_0                       0x5259
#define WM8962_VSS_XTS4_1                       0x525A
#define WM8962_VSS_XTS4_0                       0x525B
#define WM8962_VSS_XTS5_1                       0x525C
#define WM8962_VSS_XTS5_0                       0x525D
#define WM8962_VSS_XTS6_1                       0x525E
#define WM8962_VSS_XTS6_0                       0x525F
#define WM8962_VSS_XTS7_1                       0x5260
#define WM8962_VSS_XTS7_0                       0x5261
#define WM8962_VSS_XTS8_1                       0x5262
#define WM8962_VSS_XTS8_0                       0x5263
#define WM8962_VSS_XTS9_1                       0x5264
#define WM8962_VSS_XTS9_0                       0x5265
#define WM8962_VSS_XTS10_1                      0x5266
#define WM8962_VSS_XTS10_0                      0x5267
#define WM8962_VSS_XTS11_1                      0x5268
#define WM8962_VSS_XTS11_0                      0x5269
#define WM8962_VSS_XTS12_1                      0x526A
#define WM8962_VSS_XTS12_0                      0x526B
#define WM8962_VSS_XTS13_1                      0x526C
#define WM8962_VSS_XTS13_0                      0x526D
#define WM8962_VSS_XTS14_1                      0x526E
#define WM8962_VSS_XTS14_0                      0x526F
#define WM8962_VSS_XTS15_1                      0x5270
#define WM8962_VSS_XTS15_0                      0x5271
#define WM8962_VSS_XTS16_1                      0x5272
#define WM8962_VSS_XTS16_0                      0x5273
#define WM8962_VSS_XTS17_1                      0x5274
#define WM8962_VSS_XTS17_0                      0x5275
#define WM8962_VSS_XTS18_1                      0x5276
#define WM8962_VSS_XTS18_0                      0x5277
#define WM8962_VSS_XTS19_1                      0x5278
#define WM8962_VSS_XTS19_0                      0x5279
#define WM8962_VSS_XTS20_1                      0x527A
#define WM8962_VSS_XTS20_0                      0x527B
#define WM8962_VSS_XTS21_1                      0x527C
#define WM8962_VSS_XTS21_0                      0x527D
#define WM8962_VSS_XTS22_1                      0x527E
#define WM8962_VSS_XTS22_0                      0x527F
#define WM8962_VSS_XTS23_1                      0x5280
#define WM8962_VSS_XTS23_0                      0x5281
#define WM8962_VSS_XTS24_1                      0x5282
#define WM8962_VSS_XTS24_0                      0x5283
#define WM8962_VSS_XTS25_1                      0x5284
#define WM8962_VSS_XTS25_0                      0x5285
#define WM8962_VSS_XTS26_1                      0x5286
#define WM8962_VSS_XTS26_0                      0x5287
#define WM8962_VSS_XTS27_1                      0x5288
#define WM8962_VSS_XTS27_0                      0x5289
#define WM8962_VSS_XTS28_1                      0x528A
#define WM8962_VSS_XTS28_0                      0x528B
#define WM8962_VSS_XTS29_1                      0x528C
#define WM8962_VSS_XTS29_0                      0x528D
#define WM8962_VSS_XTS30_1                      0x528E
#define WM8962_VSS_XTS30_0                      0x528F
#define WM8962_VSS_XTS31_1                      0x5290
#define WM8962_VSS_XTS31_0                      0x5291
#define WM8962_VSS_XTS32_1                      0x5292
#define WM8962_VSS_XTS32_0                      0x5293

#define WM8962_REGISTER_COUNT                   1138
#define WM8962_MAX_REGISTER                     0x5293

/*
                     
 */

/*
                                
 */
#define WM8962_IN_VU                            0x0100  /*       */
#define WM8962_IN_VU_MASK                       0x0100  /*       */
#define WM8962_IN_VU_SHIFT                           8  /*       */
#define WM8962_IN_VU_WIDTH                           1  /*       */
#define WM8962_INPGAL_MUTE                      0x0080  /*             */
#define WM8962_INPGAL_MUTE_MASK                 0x0080  /*             */
#define WM8962_INPGAL_MUTE_SHIFT                     7  /*             */
#define WM8962_INPGAL_MUTE_WIDTH                     1  /*             */
#define WM8962_INL_ZC                           0x0040  /*        */
#define WM8962_INL_ZC_MASK                      0x0040  /*        */
#define WM8962_INL_ZC_SHIFT                          6  /*        */
#define WM8962_INL_ZC_WIDTH                          1  /*        */
#define WM8962_INL_VOL_MASK                     0x003F  /*                 */
#define WM8962_INL_VOL_SHIFT                         0  /*                 */
#define WM8962_INL_VOL_WIDTH                         6  /*                 */

/*
                                 
 */
#define WM8962_CUST_ID_MASK                     0xF000  /*                   */
#define WM8962_CUST_ID_SHIFT                        12  /*                   */
#define WM8962_CUST_ID_WIDTH                         4  /*                   */
#define WM8962_CHIP_REV_MASK                    0x0E00  /*                   */
#define WM8962_CHIP_REV_SHIFT                        9  /*                   */
#define WM8962_CHIP_REV_WIDTH                        3  /*                   */
#define WM8962_IN_VU                            0x0100  /*       */
#define WM8962_IN_VU_MASK                       0x0100  /*       */
#define WM8962_IN_VU_SHIFT                           8  /*       */
#define WM8962_IN_VU_WIDTH                           1  /*       */
#define WM8962_INPGAR_MUTE                      0x0080  /*             */
#define WM8962_INPGAR_MUTE_MASK                 0x0080  /*             */
#define WM8962_INPGAR_MUTE_SHIFT                     7  /*             */
#define WM8962_INPGAR_MUTE_WIDTH                     1  /*             */
#define WM8962_INR_ZC                           0x0040  /*        */
#define WM8962_INR_ZC_MASK                      0x0040  /*        */
#define WM8962_INR_ZC_SHIFT                          6  /*        */
#define WM8962_INR_ZC_WIDTH                          1  /*        */
#define WM8962_INR_VOL_MASK                     0x003F  /*                 */
#define WM8962_INR_VOL_SHIFT                         0  /*                 */
#define WM8962_INR_VOL_WIDTH                         6  /*                 */

/*
                            
 */
#define WM8962_HPOUT_VU                         0x0100  /*          */
#define WM8962_HPOUT_VU_MASK                    0x0100  /*          */
#define WM8962_HPOUT_VU_SHIFT                        8  /*          */
#define WM8962_HPOUT_VU_WIDTH                        1  /*          */
#define WM8962_HPOUTL_ZC                        0x0080  /*           */
#define WM8962_HPOUTL_ZC_MASK                   0x0080  /*           */
#define WM8962_HPOUTL_ZC_SHIFT                       7  /*           */
#define WM8962_HPOUTL_ZC_WIDTH                       1  /*           */
#define WM8962_HPOUTL_VOL_MASK                  0x007F  /*                    */
#define WM8962_HPOUTL_VOL_SHIFT                      0  /*                    */
#define WM8962_HPOUTL_VOL_WIDTH                      7  /*                    */

/*
                            
 */
#define WM8962_HPOUT_VU                         0x0100  /*          */
#define WM8962_HPOUT_VU_MASK                    0x0100  /*          */
#define WM8962_HPOUT_VU_SHIFT                        8  /*          */
#define WM8962_HPOUT_VU_WIDTH                        1  /*          */
#define WM8962_HPOUTR_ZC                        0x0080  /*           */
#define WM8962_HPOUTR_ZC_MASK                   0x0080  /*           */
#define WM8962_HPOUTR_ZC_SHIFT                       7  /*           */
#define WM8962_HPOUTR_ZC_WIDTH                       1  /*           */
#define WM8962_HPOUTR_VOL_MASK                  0x007F  /*                    */
#define WM8962_HPOUTR_VOL_SHIFT                      0  /*                    */
#define WM8962_HPOUTR_VOL_WIDTH                      7  /*                    */

/*
                        
 */
#define WM8962_DSPCLK_DIV_MASK                  0x0600  /*                     */
#define WM8962_DSPCLK_DIV_SHIFT                      9  /*                     */
#define WM8962_DSPCLK_DIV_WIDTH                      2  /*                     */
#define WM8962_ADCSYS_CLK_DIV_MASK              0x01C0  /*                        */
#define WM8962_ADCSYS_CLK_DIV_SHIFT                  6  /*                        */
#define WM8962_ADCSYS_CLK_DIV_WIDTH                  3  /*                        */
#define WM8962_DACSYS_CLK_DIV_MASK              0x0038  /*                        */
#define WM8962_DACSYS_CLK_DIV_SHIFT                  3  /*                        */
#define WM8962_DACSYS_CLK_DIV_WIDTH                  3  /*                        */
#define WM8962_MCLKDIV_MASK                     0x0006  /*                 */
#define WM8962_MCLKDIV_SHIFT                         1  /*                 */
#define WM8962_MCLKDIV_WIDTH                         2  /*                 */

/*
                                  
 */
#define WM8962_ADCR_DAT_INV                     0x0040  /*              */
#define WM8962_ADCR_DAT_INV_MASK                0x0040  /*              */
#define WM8962_ADCR_DAT_INV_SHIFT                    6  /*              */
#define WM8962_ADCR_DAT_INV_WIDTH                    1  /*              */
#define WM8962_ADCL_DAT_INV                     0x0020  /*              */
#define WM8962_ADCL_DAT_INV_MASK                0x0020  /*              */
#define WM8962_ADCL_DAT_INV_SHIFT                    5  /*              */
#define WM8962_ADCL_DAT_INV_WIDTH                    1  /*              */
#define WM8962_DAC_MUTE_RAMP                    0x0010  /*               */
#define WM8962_DAC_MUTE_RAMP_MASK               0x0010  /*               */
#define WM8962_DAC_MUTE_RAMP_SHIFT                   4  /*               */
#define WM8962_DAC_MUTE_RAMP_WIDTH                   1  /*               */
#define WM8962_DAC_MUTE                         0x0008  /*          */
#define WM8962_DAC_MUTE_MASK                    0x0008  /*          */
#define WM8962_DAC_MUTE_SHIFT                        3  /*          */
#define WM8962_DAC_MUTE_WIDTH                        1  /*          */
#define WM8962_DAC_DEEMP_MASK                   0x0006  /*                   */
#define WM8962_DAC_DEEMP_SHIFT                       1  /*                   */
#define WM8962_DAC_DEEMP_WIDTH                       2  /*                   */
#define WM8962_ADC_HPF_DIS                      0x0001  /*             */
#define WM8962_ADC_HPF_DIS_MASK                 0x0001  /*             */
#define WM8962_ADC_HPF_DIS_SHIFT                     0  /*             */
#define WM8962_ADC_HPF_DIS_WIDTH                     1  /*             */

/*
                                  
 */
#define WM8962_ADC_HPF_SR_MASK                  0x3000  /*                      */
#define WM8962_ADC_HPF_SR_SHIFT                     12  /*                      */
#define WM8962_ADC_HPF_SR_WIDTH                      2  /*                      */
#define WM8962_ADC_HPF_MODE                     0x0400  /*              */
#define WM8962_ADC_HPF_MODE_MASK                0x0400  /*              */
#define WM8962_ADC_HPF_MODE_SHIFT                   10  /*              */
#define WM8962_ADC_HPF_MODE_WIDTH                    1  /*              */
#define WM8962_ADC_HPF_CUT_MASK                 0x0380  /*                     */
#define WM8962_ADC_HPF_CUT_SHIFT                     7  /*                     */
#define WM8962_ADC_HPF_CUT_WIDTH                     3  /*                     */
#define WM8962_DACR_DAT_INV                     0x0040  /*              */
#define WM8962_DACR_DAT_INV_MASK                0x0040  /*              */
#define WM8962_DACR_DAT_INV_SHIFT                    6  /*              */
#define WM8962_DACR_DAT_INV_WIDTH                    1  /*              */
#define WM8962_DACL_DAT_INV                     0x0020  /*              */
#define WM8962_DACL_DAT_INV_MASK                0x0020  /*              */
#define WM8962_DACL_DAT_INV_SHIFT                    5  /*              */
#define WM8962_DACL_DAT_INV_WIDTH                    1  /*              */
#define WM8962_DAC_UNMUTE_RAMP                  0x0008  /*                 */
#define WM8962_DAC_UNMUTE_RAMP_MASK             0x0008  /*                 */
#define WM8962_DAC_UNMUTE_RAMP_SHIFT                 3  /*                 */
#define WM8962_DAC_UNMUTE_RAMP_WIDTH                 1  /*                 */
#define WM8962_DAC_MUTERATE                     0x0004  /*              */
#define WM8962_DAC_MUTERATE_MASK                0x0004  /*              */
#define WM8962_DAC_MUTERATE_SHIFT                    2  /*              */
#define WM8962_DAC_MUTERATE_WIDTH                    1  /*              */
#define WM8962_DAC_HP                           0x0001  /*        */
#define WM8962_DAC_HP_MASK                      0x0001  /*        */
#define WM8962_DAC_HP_SHIFT                          0  /*        */
#define WM8962_DAC_HP_WIDTH                          1  /*        */

/*
                                
 */
#define WM8962_AIFDAC_TDM_MODE                  0x1000  /*                 */
#define WM8962_AIFDAC_TDM_MODE_MASK             0x1000  /*                 */
#define WM8962_AIFDAC_TDM_MODE_SHIFT                12  /*                 */
#define WM8962_AIFDAC_TDM_MODE_WIDTH                 1  /*                 */
#define WM8962_AIFDAC_TDM_SLOT                  0x0800  /*                 */
#define WM8962_AIFDAC_TDM_SLOT_MASK             0x0800  /*                 */
#define WM8962_AIFDAC_TDM_SLOT_SHIFT                11  /*                 */
#define WM8962_AIFDAC_TDM_SLOT_WIDTH                 1  /*                 */
#define WM8962_AIFADC_TDM_MODE                  0x0400  /*                 */
#define WM8962_AIFADC_TDM_MODE_MASK             0x0400  /*                 */
#define WM8962_AIFADC_TDM_MODE_SHIFT                10  /*                 */
#define WM8962_AIFADC_TDM_MODE_WIDTH                 1  /*                 */
#define WM8962_AIFADC_TDM_SLOT                  0x0200  /*                 */
#define WM8962_AIFADC_TDM_SLOT_MASK             0x0200  /*                 */
#define WM8962_AIFADC_TDM_SLOT_SHIFT                 9  /*                 */
#define WM8962_AIFADC_TDM_SLOT_WIDTH                 1  /*                 */
#define WM8962_ADC_LRSWAP                       0x0100  /*            */
#define WM8962_ADC_LRSWAP_MASK                  0x0100  /*            */
#define WM8962_ADC_LRSWAP_SHIFT                      8  /*            */
#define WM8962_ADC_LRSWAP_WIDTH                      1  /*            */
#define WM8962_BCLK_INV                         0x0080  /*          */
#define WM8962_BCLK_INV_MASK                    0x0080  /*          */
#define WM8962_BCLK_INV_SHIFT                        7  /*          */
#define WM8962_BCLK_INV_WIDTH                        1  /*          */
#define WM8962_MSTR                             0x0040  /*      */
#define WM8962_MSTR_MASK                        0x0040  /*      */
#define WM8962_MSTR_SHIFT                            6  /*      */
#define WM8962_MSTR_WIDTH                            1  /*      */
#define WM8962_DAC_LRSWAP                       0x0020  /*            */
#define WM8962_DAC_LRSWAP_MASK                  0x0020  /*            */
#define WM8962_DAC_LRSWAP_SHIFT                      5  /*            */
#define WM8962_DAC_LRSWAP_WIDTH                      1  /*            */
#define WM8962_LRCLK_INV                        0x0010  /*           */
#define WM8962_LRCLK_INV_MASK                   0x0010  /*           */
#define WM8962_LRCLK_INV_SHIFT                       4  /*           */
#define WM8962_LRCLK_INV_WIDTH                       1  /*           */
#define WM8962_WL_MASK                          0x000C  /*            */
#define WM8962_WL_SHIFT                              2  /*            */
#define WM8962_WL_WIDTH                              2  /*            */
#define WM8962_FMT_MASK                         0x0003  /*             */
#define WM8962_FMT_SHIFT                             0  /*             */
#define WM8962_FMT_WIDTH                             2  /*             */

/*
                        
 */
#define WM8962_CLKREG_OVD                       0x0800  /*            */
#define WM8962_CLKREG_OVD_MASK                  0x0800  /*            */
#define WM8962_CLKREG_OVD_SHIFT                     11  /*            */
#define WM8962_CLKREG_OVD_WIDTH                      1  /*            */
#define WM8962_SYSCLK_SRC_MASK                  0x0600  /*                     */
#define WM8962_SYSCLK_SRC_SHIFT                      9  /*                     */
#define WM8962_SYSCLK_SRC_WIDTH                      2  /*                     */
#define WM8962_CLASSD_CLK_DIV_MASK              0x01C0  /*                        */
#define WM8962_CLASSD_CLK_DIV_SHIFT                  6  /*                        */
#define WM8962_CLASSD_CLK_DIV_WIDTH                  3  /*                        */
#define WM8962_SYSCLK_ENA                       0x0020  /*            */
#define WM8962_SYSCLK_ENA_MASK                  0x0020  /*            */
#define WM8962_SYSCLK_ENA_SHIFT                      5  /*            */
#define WM8962_SYSCLK_ENA_WIDTH                      1  /*            */
#define WM8962_BCLK_DIV_MASK                    0x000F  /*                  */
#define WM8962_BCLK_DIV_SHIFT                        0  /*                  */
#define WM8962_BCLK_DIV_WIDTH                        4  /*                  */

/*
                                
 */
#define WM8962_AUTOMUTE_STS                     0x0800  /*              */
#define WM8962_AUTOMUTE_STS_MASK                0x0800  /*              */
#define WM8962_AUTOMUTE_STS_SHIFT                   11  /*              */
#define WM8962_AUTOMUTE_STS_WIDTH                    1  /*              */
#define WM8962_DAC_AUTOMUTE_SAMPLES_MASK        0x0300  /*                              */
#define WM8962_DAC_AUTOMUTE_SAMPLES_SHIFT            8  /*                              */
#define WM8962_DAC_AUTOMUTE_SAMPLES_WIDTH            2  /*                              */
#define WM8962_DAC_AUTOMUTE                     0x0080  /*              */
#define WM8962_DAC_AUTOMUTE_MASK                0x0080  /*              */
#define WM8962_DAC_AUTOMUTE_SHIFT                    7  /*              */
#define WM8962_DAC_AUTOMUTE_WIDTH                    1  /*              */
#define WM8962_DAC_COMP                         0x0010  /*          */
#define WM8962_DAC_COMP_MASK                    0x0010  /*          */
#define WM8962_DAC_COMP_SHIFT                        4  /*          */
#define WM8962_DAC_COMP_WIDTH                        1  /*          */
#define WM8962_DAC_COMPMODE                     0x0008  /*              */
#define WM8962_DAC_COMPMODE_MASK                0x0008  /*              */
#define WM8962_DAC_COMPMODE_SHIFT                    3  /*              */
#define WM8962_DAC_COMPMODE_WIDTH                    1  /*              */
#define WM8962_ADC_COMP                         0x0004  /*          */
#define WM8962_ADC_COMP_MASK                    0x0004  /*          */
#define WM8962_ADC_COMP_SHIFT                        2  /*          */
#define WM8962_ADC_COMP_WIDTH                        1  /*          */
#define WM8962_ADC_COMPMODE                     0x0002  /*              */
#define WM8962_ADC_COMPMODE_MASK                0x0002  /*              */
#define WM8962_ADC_COMPMODE_SHIFT                    1  /*              */
#define WM8962_ADC_COMPMODE_WIDTH                    1  /*              */
#define WM8962_LOOPBACK                         0x0001  /*          */
#define WM8962_LOOPBACK_MASK                    0x0001  /*          */
#define WM8962_LOOPBACK_SHIFT                        0  /*          */
#define WM8962_LOOPBACK_WIDTH                        1  /*          */

/*
                               
 */
#define WM8962_DAC_VU                           0x0100  /*        */
#define WM8962_DAC_VU_MASK                      0x0100  /*        */
#define WM8962_DAC_VU_SHIFT                          8  /*        */
#define WM8962_DAC_VU_WIDTH                          1  /*        */
#define WM8962_DACL_VOL_MASK                    0x00FF  /*                  */
#define WM8962_DACL_VOL_SHIFT                        0  /*                  */
#define WM8962_DACL_VOL_WIDTH                        8  /*                  */

/*
                                
 */
#define WM8962_DAC_VU                           0x0100  /*        */
#define WM8962_DAC_VU_MASK                      0x0100  /*        */
#define WM8962_DAC_VU_SHIFT                          8  /*        */
#define WM8962_DAC_VU_WIDTH                          1  /*        */
#define WM8962_DACR_VOL_MASK                    0x00FF  /*                  */
#define WM8962_DACR_VOL_SHIFT                        0  /*                  */
#define WM8962_DACR_VOL_WIDTH                        8  /*                  */

/*
                                 
 */
#define WM8962_AIF_RATE_MASK                    0x07FF  /*                   */
#define WM8962_AIF_RATE_SHIFT                        0  /*                   */
#define WM8962_AIF_RATE_WIDTH                       11  /*                   */

/*
                              
 */
#define WM8962_SW_RESET_MASK                    0xFFFF  /*                   */
#define WM8962_SW_RESET_SHIFT                        0  /*                   */
#define WM8962_SW_RESET_WIDTH                       16  /*                   */

/*
                    
 */
#define WM8962_ALC_INACTIVE_ENA                 0x0400  /*                  */
#define WM8962_ALC_INACTIVE_ENA_MASK            0x0400  /*                  */
#define WM8962_ALC_INACTIVE_ENA_SHIFT               10  /*                  */
#define WM8962_ALC_INACTIVE_ENA_WIDTH                1  /*                  */
#define WM8962_ALC_LVL_MODE                     0x0200  /*              */
#define WM8962_ALC_LVL_MODE_MASK                0x0200  /*              */
#define WM8962_ALC_LVL_MODE_SHIFT                    9  /*              */
#define WM8962_ALC_LVL_MODE_WIDTH                    1  /*              */
#define WM8962_ALCL_ENA                         0x0100  /*          */
#define WM8962_ALCL_ENA_MASK                    0x0100  /*          */
#define WM8962_ALCL_ENA_SHIFT                        8  /*          */
#define WM8962_ALCL_ENA_WIDTH                        1  /*          */
#define WM8962_ALCR_ENA                         0x0080  /*          */
#define WM8962_ALCR_ENA_MASK                    0x0080  /*          */
#define WM8962_ALCR_ENA_SHIFT                        7  /*          */
#define WM8962_ALCR_ENA_WIDTH                        1  /*          */
#define WM8962_ALC_MAXGAIN_MASK                 0x0070  /*                     */
#define WM8962_ALC_MAXGAIN_SHIFT                     4  /*                     */
#define WM8962_ALC_MAXGAIN_WIDTH                     3  /*                     */
#define WM8962_ALC_LVL_MASK                     0x000F  /*                 */
#define WM8962_ALC_LVL_SHIFT                         0  /*                 */
#define WM8962_ALC_LVL_WIDTH                         4  /*                 */

/*
                    
 */
#define WM8962_ALC_LOCK_STS                     0x8000  /*              */
#define WM8962_ALC_LOCK_STS_MASK                0x8000  /*              */
#define WM8962_ALC_LOCK_STS_SHIFT                   15  /*              */
#define WM8962_ALC_LOCK_STS_WIDTH                    1  /*              */
#define WM8962_ALC_THRESH_STS                   0x4000  /*                */
#define WM8962_ALC_THRESH_STS_MASK              0x4000  /*                */
#define WM8962_ALC_THRESH_STS_SHIFT                 14  /*                */
#define WM8962_ALC_THRESH_STS_WIDTH                  1  /*                */
#define WM8962_ALC_SAT_STS                      0x2000  /*             */
#define WM8962_ALC_SAT_STS_MASK                 0x2000  /*             */
#define WM8962_ALC_SAT_STS_SHIFT                    13  /*             */
#define WM8962_ALC_SAT_STS_WIDTH                     1  /*             */
#define WM8962_ALC_PKOVR_STS                    0x1000  /*               */
#define WM8962_ALC_PKOVR_STS_MASK               0x1000  /*               */
#define WM8962_ALC_PKOVR_STS_SHIFT                  12  /*               */
#define WM8962_ALC_PKOVR_STS_WIDTH                   1  /*               */
#define WM8962_ALC_NGATE_STS                    0x0800  /*               */
#define WM8962_ALC_NGATE_STS_MASK               0x0800  /*               */
#define WM8962_ALC_NGATE_STS_SHIFT                  11  /*               */
#define WM8962_ALC_NGATE_STS_WIDTH                   1  /*               */
#define WM8962_ALC_ZC                           0x0080  /*        */
#define WM8962_ALC_ZC_MASK                      0x0080  /*        */
#define WM8962_ALC_ZC_SHIFT                          7  /*        */
#define WM8962_ALC_ZC_WIDTH                          1  /*        */
#define WM8962_ALC_MINGAIN_MASK                 0x0070  /*                     */
#define WM8962_ALC_MINGAIN_SHIFT                     4  /*                     */
#define WM8962_ALC_MINGAIN_WIDTH                     3  /*                     */
#define WM8962_ALC_HLD_MASK                     0x000F  /*                 */
#define WM8962_ALC_HLD_SHIFT                         0  /*                 */
#define WM8962_ALC_HLD_WIDTH                         4  /*                 */

/*
                    
 */
#define WM8962_ALC_NGATE_GAIN_MASK              0x1C00  /*                          */
#define WM8962_ALC_NGATE_GAIN_SHIFT                 10  /*                          */
#define WM8962_ALC_NGATE_GAIN_WIDTH                  3  /*                          */
#define WM8962_ALC_MODE                         0x0100  /*          */
#define WM8962_ALC_MODE_MASK                    0x0100  /*          */
#define WM8962_ALC_MODE_SHIFT                        8  /*          */
#define WM8962_ALC_MODE_WIDTH                        1  /*          */
#define WM8962_ALC_DCY_MASK                     0x00F0  /*                 */
#define WM8962_ALC_DCY_SHIFT                         4  /*                 */
#define WM8962_ALC_DCY_WIDTH                         4  /*                 */
#define WM8962_ALC_ATK_MASK                     0x000F  /*                 */
#define WM8962_ALC_ATK_SHIFT                         0  /*                 */
#define WM8962_ALC_ATK_WIDTH                         4  /*                 */

/*
                          
 */
#define WM8962_ALC_NGATE_DCY_MASK               0xF000  /*                         */
#define WM8962_ALC_NGATE_DCY_SHIFT                  12  /*                         */
#define WM8962_ALC_NGATE_DCY_WIDTH                   4  /*                         */
#define WM8962_ALC_NGATE_ATK_MASK               0x0F00  /*                        */
#define WM8962_ALC_NGATE_ATK_SHIFT                   8  /*                        */
#define WM8962_ALC_NGATE_ATK_WIDTH                   4  /*                        */
#define WM8962_ALC_NGATE_THR_MASK               0x00F8  /*                       */
#define WM8962_ALC_NGATE_THR_SHIFT                   3  /*                       */
#define WM8962_ALC_NGATE_THR_WIDTH                   5  /*                       */
#define WM8962_ALC_NGATE_MODE_MASK              0x0006  /*                        */
#define WM8962_ALC_NGATE_MODE_SHIFT                  1  /*                        */
#define WM8962_ALC_NGATE_MODE_WIDTH                  2  /*                        */
#define WM8962_ALC_NGATE_ENA                    0x0001  /*               */
#define WM8962_ALC_NGATE_ENA_MASK               0x0001  /*               */
#define WM8962_ALC_NGATE_ENA_SHIFT                   0  /*               */
#define WM8962_ALC_NGATE_ENA_WIDTH                   1  /*               */

/*
                               
 */
#define WM8962_ADC_VU                           0x0100  /*        */
#define WM8962_ADC_VU_MASK                      0x0100  /*        */
#define WM8962_ADC_VU_SHIFT                          8  /*        */
#define WM8962_ADC_VU_WIDTH                          1  /*        */
#define WM8962_ADCL_VOL_MASK                    0x00FF  /*                  */
#define WM8962_ADCL_VOL_SHIFT                        0  /*                  */
#define WM8962_ADCL_VOL_WIDTH                        8  /*                  */

/*
                                
 */
#define WM8962_ADC_VU                           0x0100  /*        */
#define WM8962_ADC_VU_MASK                      0x0100  /*        */
#define WM8962_ADC_VU_SHIFT                          8  /*        */
#define WM8962_ADC_VU_WIDTH                          1  /*        */
#define WM8962_ADCR_VOL_MASK                    0x00FF  /*                  */
#define WM8962_ADCR_VOL_SHIFT                        0  /*                  */
#define WM8962_ADCR_VOL_WIDTH                        8  /*                  */

/*
                                     
 */
#define WM8962_THERR_ACT                        0x0100  /*           */
#define WM8962_THERR_ACT_MASK                   0x0100  /*           */
#define WM8962_THERR_ACT_SHIFT                       8  /*           */
#define WM8962_THERR_ACT_WIDTH                       1  /*           */
#define WM8962_ADC_BIAS                         0x0040  /*          */
#define WM8962_ADC_BIAS_MASK                    0x0040  /*          */
#define WM8962_ADC_BIAS_SHIFT                        6  /*          */
#define WM8962_ADC_BIAS_WIDTH                        1  /*          */
#define WM8962_ADC_HP                           0x0020  /*        */
#define WM8962_ADC_HP_MASK                      0x0020  /*        */
#define WM8962_ADC_HP_SHIFT                          5  /*        */
#define WM8962_ADC_HP_WIDTH                          1  /*        */
#define WM8962_TOCLK_ENA                        0x0001  /*           */
#define WM8962_TOCLK_ENA_MASK                   0x0001  /*           */
#define WM8962_TOCLK_ENA_SHIFT                       0  /*           */
#define WM8962_TOCLK_ENA_WIDTH                       1  /*           */

/*
                                     
 */
#define WM8962_AIF_TRI                          0x0008  /*         */
#define WM8962_AIF_TRI_MASK                     0x0008  /*         */
#define WM8962_AIF_TRI_SHIFT                         3  /*         */
#define WM8962_AIF_TRI_WIDTH                         1  /*         */

/*
                            
 */
#define WM8962_DMIC_ENA                         0x0400  /*          */
#define WM8962_DMIC_ENA_MASK                    0x0400  /*          */
#define WM8962_DMIC_ENA_SHIFT                       10  /*          */
#define WM8962_DMIC_ENA_WIDTH                        1  /*          */
#define WM8962_OPCLK_ENA                        0x0200  /*           */
#define WM8962_OPCLK_ENA_MASK                   0x0200  /*           */
#define WM8962_OPCLK_ENA_SHIFT                       9  /*           */
#define WM8962_OPCLK_ENA_WIDTH                       1  /*           */
#define WM8962_VMID_SEL_MASK                    0x0180  /*                  */
#define WM8962_VMID_SEL_SHIFT                        7  /*                  */
#define WM8962_VMID_SEL_WIDTH                        2  /*                  */
#define WM8962_BIAS_ENA                         0x0040  /*          */
#define WM8962_BIAS_ENA_MASK                    0x0040  /*          */
#define WM8962_BIAS_ENA_SHIFT                        6  /*          */
#define WM8962_BIAS_ENA_WIDTH                        1  /*          */
#define WM8962_INL_ENA                          0x0020  /*         */
#define WM8962_INL_ENA_MASK                     0x0020  /*         */
#define WM8962_INL_ENA_SHIFT                         5  /*         */
#define WM8962_INL_ENA_WIDTH                         1  /*         */
#define WM8962_INR_ENA                          0x0010  /*         */
#define WM8962_INR_ENA_MASK                     0x0010  /*         */
#define WM8962_INR_ENA_SHIFT                         4  /*         */
#define WM8962_INR_ENA_WIDTH                         1  /*         */
#define WM8962_ADCL_ENA                         0x0008  /*          */
#define WM8962_ADCL_ENA_MASK                    0x0008  /*          */
#define WM8962_ADCL_ENA_SHIFT                        3  /*          */
#define WM8962_ADCL_ENA_WIDTH                        1  /*          */
#define WM8962_ADCR_ENA                         0x0004  /*          */
#define WM8962_ADCR_ENA_MASK                    0x0004  /*          */
#define WM8962_ADCR_ENA_SHIFT                        2  /*          */
#define WM8962_ADCR_ENA_WIDTH                        1  /*          */
#define WM8962_MICBIAS_ENA                      0x0002  /*             */
#define WM8962_MICBIAS_ENA_MASK                 0x0002  /*             */
#define WM8962_MICBIAS_ENA_SHIFT                     1  /*             */
#define WM8962_MICBIAS_ENA_WIDTH                     1  /*             */

/*
                            
 */
#define WM8962_DACL_ENA                         0x0100  /*          */
#define WM8962_DACL_ENA_MASK                    0x0100  /*          */
#define WM8962_DACL_ENA_SHIFT                        8  /*          */
#define WM8962_DACL_ENA_WIDTH                        1  /*          */
#define WM8962_DACR_ENA                         0x0080  /*          */
#define WM8962_DACR_ENA_MASK                    0x0080  /*          */
#define WM8962_DACR_ENA_SHIFT                        7  /*          */
#define WM8962_DACR_ENA_WIDTH                        1  /*          */
#define WM8962_HPOUTL_PGA_ENA                   0x0040  /*                */
#define WM8962_HPOUTL_PGA_ENA_MASK              0x0040  /*                */
#define WM8962_HPOUTL_PGA_ENA_SHIFT                  6  /*                */
#define WM8962_HPOUTL_PGA_ENA_WIDTH                  1  /*                */
#define WM8962_HPOUTR_PGA_ENA                   0x0020  /*                */
#define WM8962_HPOUTR_PGA_ENA_MASK              0x0020  /*                */
#define WM8962_HPOUTR_PGA_ENA_SHIFT                  5  /*                */
#define WM8962_HPOUTR_PGA_ENA_WIDTH                  1  /*                */
#define WM8962_SPKOUTL_PGA_ENA                  0x0010  /*                 */
#define WM8962_SPKOUTL_PGA_ENA_MASK             0x0010  /*                 */
#define WM8962_SPKOUTL_PGA_ENA_SHIFT                 4  /*                 */
#define WM8962_SPKOUTL_PGA_ENA_WIDTH                 1  /*                 */
#define WM8962_SPKOUTR_PGA_ENA                  0x0008  /*                 */
#define WM8962_SPKOUTR_PGA_ENA_MASK             0x0008  /*                 */
#define WM8962_SPKOUTR_PGA_ENA_SHIFT                 3  /*                 */
#define WM8962_SPKOUTR_PGA_ENA_WIDTH                 1  /*                 */
#define WM8962_HPOUTL_PGA_MUTE                  0x0002  /*                 */
#define WM8962_HPOUTL_PGA_MUTE_MASK             0x0002  /*                 */
#define WM8962_HPOUTL_PGA_MUTE_SHIFT                 1  /*                 */
#define WM8962_HPOUTL_PGA_MUTE_WIDTH                 1  /*                 */
#define WM8962_HPOUTR_PGA_MUTE                  0x0001  /*                 */
#define WM8962_HPOUTR_PGA_MUTE_MASK             0x0001  /*                 */
#define WM8962_HPOUTR_PGA_MUTE_SHIFT                 0  /*                 */
#define WM8962_HPOUTR_PGA_MUTE_WIDTH                 1  /*                 */

/*
                                      
 */
#define WM8962_SAMPLE_RATE_INT_MODE             0x0010  /*                      */
#define WM8962_SAMPLE_RATE_INT_MODE_MASK        0x0010  /*                      */
#define WM8962_SAMPLE_RATE_INT_MODE_SHIFT            4  /*                      */
#define WM8962_SAMPLE_RATE_INT_MODE_WIDTH            1  /*                      */
#define WM8962_SAMPLE_RATE_MASK                 0x0007  /*                     */
#define WM8962_SAMPLE_RATE_SHIFT                     0  /*                     */
#define WM8962_SAMPLE_RATE_WIDTH                     3  /*                     */

/*
                        
 */
#define WM8962_STARTUP_BIAS_ENA                 0x0010  /*                  */
#define WM8962_STARTUP_BIAS_ENA_MASK            0x0010  /*                  */
#define WM8962_STARTUP_BIAS_ENA_SHIFT                4  /*                  */
#define WM8962_STARTUP_BIAS_ENA_WIDTH                1  /*                  */
#define WM8962_VMID_BUF_ENA                     0x0008  /*              */
#define WM8962_VMID_BUF_ENA_MASK                0x0008  /*              */
#define WM8962_VMID_BUF_ENA_SHIFT                    3  /*              */
#define WM8962_VMID_BUF_ENA_WIDTH                    1  /*              */
#define WM8962_VMID_RAMP                        0x0004  /*           */
#define WM8962_VMID_RAMP_MASK                   0x0004  /*           */
#define WM8962_VMID_RAMP_SHIFT                       2  /*           */
#define WM8962_VMID_RAMP_WIDTH                       1  /*           */

/*
                          
 */
#define WM8962_DBCLK_DIV_MASK                   0xE000  /*                     */
#define WM8962_DBCLK_DIV_SHIFT                      13  /*                     */
#define WM8962_DBCLK_DIV_WIDTH                       3  /*                     */
#define WM8962_OPCLK_DIV_MASK                   0x1C00  /*                     */
#define WM8962_OPCLK_DIV_SHIFT                      10  /*                     */
#define WM8962_OPCLK_DIV_WIDTH                       3  /*                     */
#define WM8962_TOCLK_DIV_MASK                   0x0380  /*                   */
#define WM8962_TOCLK_DIV_SHIFT                       7  /*                   */
#define WM8962_TOCLK_DIV_WIDTH                       3  /*                   */
#define WM8962_F256KCLK_DIV_MASK                0x007E  /*                      */
#define WM8962_F256KCLK_DIV_SHIFT                    1  /*                      */
#define WM8962_F256KCLK_DIV_WIDTH                    6  /*                      */

/*
                                       
 */
#define WM8962_MIXINL_MUTE                      0x0008  /*             */
#define WM8962_MIXINL_MUTE_MASK                 0x0008  /*             */
#define WM8962_MIXINL_MUTE_SHIFT                     3  /*             */
#define WM8962_MIXINL_MUTE_WIDTH                     1  /*             */
#define WM8962_MIXINR_MUTE                      0x0004  /*             */
#define WM8962_MIXINR_MUTE_MASK                 0x0004  /*             */
#define WM8962_MIXINR_MUTE_SHIFT                     2  /*             */
#define WM8962_MIXINR_MUTE_WIDTH                     1  /*             */
#define WM8962_MIXINL_ENA                       0x0002  /*            */
#define WM8962_MIXINL_ENA_MASK                  0x0002  /*            */
#define WM8962_MIXINL_ENA_SHIFT                      1  /*            */
#define WM8962_MIXINL_ENA_WIDTH                      1  /*            */
#define WM8962_MIXINR_ENA                       0x0001  /*            */
#define WM8962_MIXINR_ENA_MASK                  0x0001  /*            */
#define WM8962_MIXINR_ENA_SHIFT                      0  /*            */
#define WM8962_MIXINR_ENA_WIDTH                      1  /*            */

/*
                                       
 */
#define WM8962_IN2L_MIXINL_VOL_MASK             0x01C0  /*                         */
#define WM8962_IN2L_MIXINL_VOL_SHIFT                 6  /*                         */
#define WM8962_IN2L_MIXINL_VOL_WIDTH                 3  /*                         */
#define WM8962_INPGAL_MIXINL_VOL_MASK           0x0038  /*                           */
#define WM8962_INPGAL_MIXINL_VOL_SHIFT               3  /*                           */
#define WM8962_INPGAL_MIXINL_VOL_WIDTH               3  /*                           */
#define WM8962_IN3L_MIXINL_VOL_MASK             0x0007  /*                         */
#define WM8962_IN3L_MIXINL_VOL_SHIFT                 0  /*                         */
#define WM8962_IN3L_MIXINL_VOL_WIDTH                 3  /*                         */

/*
                                        
 */
#define WM8962_IN2R_MIXINR_VOL_MASK             0x01C0  /*                         */
#define WM8962_IN2R_MIXINR_VOL_SHIFT                 6  /*                         */
#define WM8962_IN2R_MIXINR_VOL_WIDTH                 3  /*                         */
#define WM8962_INPGAR_MIXINR_VOL_MASK           0x0038  /*                           */
#define WM8962_INPGAR_MIXINR_VOL_SHIFT               3  /*                           */
#define WM8962_INPGAR_MIXINR_VOL_WIDTH               3  /*                           */
#define WM8962_IN3R_MIXINR_VOL_MASK             0x0007  /*                         */
#define WM8962_IN3R_MIXINR_VOL_SHIFT                 0  /*                         */
#define WM8962_IN3R_MIXINR_VOL_WIDTH                 3  /*                         */

/*
                                       
 */
#define WM8962_IN2L_TO_MIXINL                   0x0020  /*                */
#define WM8962_IN2L_TO_MIXINL_MASK              0x0020  /*                */
#define WM8962_IN2L_TO_MIXINL_SHIFT                  5  /*                */
#define WM8962_IN2L_TO_MIXINL_WIDTH                  1  /*                */
#define WM8962_IN3L_TO_MIXINL                   0x0010  /*                */
#define WM8962_IN3L_TO_MIXINL_MASK              0x0010  /*                */
#define WM8962_IN3L_TO_MIXINL_SHIFT                  4  /*                */
#define WM8962_IN3L_TO_MIXINL_WIDTH                  1  /*                */
#define WM8962_INPGAL_TO_MIXINL                 0x0008  /*                  */
#define WM8962_INPGAL_TO_MIXINL_MASK            0x0008  /*                  */
#define WM8962_INPGAL_TO_MIXINL_SHIFT                3  /*                  */
#define WM8962_INPGAL_TO_MIXINL_WIDTH                1  /*                  */
#define WM8962_IN2R_TO_MIXINR                   0x0004  /*                */
#define WM8962_IN2R_TO_MIXINR_MASK              0x0004  /*                */
#define WM8962_IN2R_TO_MIXINR_SHIFT                  2  /*                */
#define WM8962_IN2R_TO_MIXINR_WIDTH                  1  /*                */
#define WM8962_IN3R_TO_MIXINR                   0x0002  /*                */
#define WM8962_IN3R_TO_MIXINR_MASK              0x0002  /*                */
#define WM8962_IN3R_TO_MIXINR_SHIFT                  1  /*                */
#define WM8962_IN3R_TO_MIXINR_WIDTH                  1  /*                */
#define WM8962_INPGAR_TO_MIXINR                 0x0001  /*                  */
#define WM8962_INPGAR_TO_MIXINR_MASK            0x0001  /*                  */
#define WM8962_INPGAR_TO_MIXINR_SHIFT                0  /*                  */
#define WM8962_INPGAR_TO_MIXINR_WIDTH                1  /*                  */

/*
                                  
 */
#define WM8962_MIXIN_BIAS_MASK                  0x0038  /*                    */
#define WM8962_MIXIN_BIAS_SHIFT                      3  /*                    */
#define WM8962_MIXIN_BIAS_WIDTH                      3  /*                    */
#define WM8962_INPGA_BIAS_MASK                  0x0007  /*                    */
#define WM8962_INPGA_BIAS_SHIFT                      0  /*                    */
#define WM8962_INPGA_BIAS_WIDTH                      3  /*                    */

/*
                                      
 */
#define WM8962_INPGAL_ENA                       0x0010  /*            */
#define WM8962_INPGAL_ENA_MASK                  0x0010  /*            */
#define WM8962_INPGAL_ENA_SHIFT                      4  /*            */
#define WM8962_INPGAL_ENA_WIDTH                      1  /*            */
#define WM8962_IN1L_TO_INPGAL                   0x0008  /*                */
#define WM8962_IN1L_TO_INPGAL_MASK              0x0008  /*                */
#define WM8962_IN1L_TO_INPGAL_SHIFT                  3  /*                */
#define WM8962_IN1L_TO_INPGAL_WIDTH                  1  /*                */
#define WM8962_IN2L_TO_INPGAL                   0x0004  /*                */
#define WM8962_IN2L_TO_INPGAL_MASK              0x0004  /*                */
#define WM8962_IN2L_TO_INPGAL_SHIFT                  2  /*                */
#define WM8962_IN2L_TO_INPGAL_WIDTH                  1  /*                */
#define WM8962_IN3L_TO_INPGAL                   0x0002  /*                */
#define WM8962_IN3L_TO_INPGAL_MASK              0x0002  /*                */
#define WM8962_IN3L_TO_INPGAL_SHIFT                  1  /*                */
#define WM8962_IN3L_TO_INPGAL_WIDTH                  1  /*                */
#define WM8962_IN4L_TO_INPGAL                   0x0001  /*                */
#define WM8962_IN4L_TO_INPGAL_MASK              0x0001  /*                */
#define WM8962_IN4L_TO_INPGAL_SHIFT                  0  /*                */
#define WM8962_IN4L_TO_INPGAL_WIDTH                  1  /*                */

/*
                                       
 */
#define WM8962_INPGAR_ENA                       0x0010  /*            */
#define WM8962_INPGAR_ENA_MASK                  0x0010  /*            */
#define WM8962_INPGAR_ENA_SHIFT                      4  /*            */
#define WM8962_INPGAR_ENA_WIDTH                      1  /*            */
#define WM8962_IN1R_TO_INPGAR                   0x0008  /*                */
#define WM8962_IN1R_TO_INPGAR_MASK              0x0008  /*                */
#define WM8962_IN1R_TO_INPGAR_SHIFT                  3  /*                */
#define WM8962_IN1R_TO_INPGAR_WIDTH                  1  /*                */
#define WM8962_IN2R_TO_INPGAR                   0x0004  /*                */
#define WM8962_IN2R_TO_INPGAR_MASK              0x0004  /*                */
#define WM8962_IN2R_TO_INPGAR_SHIFT                  2  /*                */
#define WM8962_IN2R_TO_INPGAR_WIDTH                  1  /*                */
#define WM8962_IN3R_TO_INPGAR                   0x0002  /*                */
#define WM8962_IN3R_TO_INPGAR_MASK              0x0002  /*                */
#define WM8962_IN3R_TO_INPGAR_SHIFT                  1  /*                */
#define WM8962_IN3R_TO_INPGAR_WIDTH                  1  /*                */
#define WM8962_IN4R_TO_INPGAR                   0x0001  /*                */
#define WM8962_IN4R_TO_INPGAR_MASK              0x0001  /*                */
#define WM8962_IN4R_TO_INPGAR_SHIFT                  0  /*                */
#define WM8962_IN4R_TO_INPGAR_WIDTH                  1  /*                */

/*
                              
 */
#define WM8962_SPKOUT_VU                        0x0100  /*           */
#define WM8962_SPKOUT_VU_MASK                   0x0100  /*           */
#define WM8962_SPKOUT_VU_SHIFT                       8  /*           */
#define WM8962_SPKOUT_VU_WIDTH                       1  /*           */
#define WM8962_SPKOUTL_ZC                       0x0080  /*            */
#define WM8962_SPKOUTL_ZC_MASK                  0x0080  /*            */
#define WM8962_SPKOUTL_ZC_SHIFT                      7  /*            */
#define WM8962_SPKOUTL_ZC_WIDTH                      1  /*            */
#define WM8962_SPKOUTL_VOL_MASK                 0x007F  /*                     */
#define WM8962_SPKOUTL_VOL_SHIFT                     0  /*                     */
#define WM8962_SPKOUTL_VOL_WIDTH                     7  /*                     */

/*
                              
 */
#define WM8962_SPKOUTR_ZC                       0x0080  /*            */
#define WM8962_SPKOUTR_ZC_MASK                  0x0080  /*            */
#define WM8962_SPKOUTR_ZC_SHIFT                      7  /*            */
#define WM8962_SPKOUTR_ZC_WIDTH                      1  /*            */
#define WM8962_SPKOUTR_VOL_MASK                 0x007F  /*                     */
#define WM8962_SPKOUTR_VOL_SHIFT                     0  /*                     */
#define WM8962_SPKOUTR_VOL_WIDTH                     7  /*                     */

/*
                                       
 */
#define WM8962_TEMP_ERR_HP                      0x0008  /*             */
#define WM8962_TEMP_ERR_HP_MASK                 0x0008  /*             */
#define WM8962_TEMP_ERR_HP_SHIFT                     3  /*             */
#define WM8962_TEMP_ERR_HP_WIDTH                     1  /*             */
#define WM8962_TEMP_WARN_HP                     0x0004  /*              */
#define WM8962_TEMP_WARN_HP_MASK                0x0004  /*              */
#define WM8962_TEMP_WARN_HP_SHIFT                    2  /*              */
#define WM8962_TEMP_WARN_HP_WIDTH                    1  /*              */
#define WM8962_TEMP_ERR_SPK                     0x0002  /*              */
#define WM8962_TEMP_ERR_SPK_MASK                0x0002  /*              */
#define WM8962_TEMP_ERR_SPK_SHIFT                    1  /*              */
#define WM8962_TEMP_ERR_SPK_WIDTH                    1  /*              */
#define WM8962_TEMP_WARN_SPK                    0x0001  /*               */
#define WM8962_TEMP_WARN_SPK_MASK               0x0001  /*               */
#define WM8962_TEMP_WARN_SPK_SHIFT                   0  /*               */
#define WM8962_TEMP_WARN_SPK_WIDTH                   1  /*               */

/*
                                      
 */
#define WM8962_MICDET_THR_MASK                  0x7000  /*                      */
#define WM8962_MICDET_THR_SHIFT                     12  /*                      */
#define WM8962_MICDET_THR_WIDTH                      3  /*                      */
#define WM8962_MICSHORT_THR_MASK                0x0C00  /*                        */
#define WM8962_MICSHORT_THR_SHIFT                   10  /*                        */
#define WM8962_MICSHORT_THR_WIDTH                    2  /*                        */
#define WM8962_MICDET_ENA                       0x0200  /*            */
#define WM8962_MICDET_ENA_MASK                  0x0200  /*            */
#define WM8962_MICDET_ENA_SHIFT                      9  /*            */
#define WM8962_MICDET_ENA_WIDTH                      1  /*            */
#define WM8962_MICDET_STS                       0x0080  /*            */
#define WM8962_MICDET_STS_MASK                  0x0080  /*            */
#define WM8962_MICDET_STS_SHIFT                      7  /*            */
#define WM8962_MICDET_STS_WIDTH                      1  /*            */
#define WM8962_MICSHORT_STS                     0x0040  /*              */
#define WM8962_MICSHORT_STS_MASK                0x0040  /*              */
#define WM8962_MICSHORT_STS_SHIFT                    6  /*              */
#define WM8962_MICSHORT_STS_WIDTH                    1  /*              */
#define WM8962_TEMP_ENA_HP                      0x0004  /*             */
#define WM8962_TEMP_ENA_HP_MASK                 0x0004  /*             */
#define WM8962_TEMP_ENA_HP_SHIFT                     2  /*             */
#define WM8962_TEMP_ENA_HP_WIDTH                     1  /*             */
#define WM8962_TEMP_ENA_SPK                     0x0002  /*              */
#define WM8962_TEMP_ENA_SPK_MASK                0x0002  /*              */
#define WM8962_TEMP_ENA_SPK_SHIFT                    1  /*              */
#define WM8962_TEMP_ENA_SPK_WIDTH                    1  /*              */
#define WM8962_MICBIAS_LVL                      0x0001  /*             */
#define WM8962_MICBIAS_LVL_MASK                 0x0001  /*             */
#define WM8962_MICBIAS_LVL_SHIFT                     0  /*             */
#define WM8962_MICBIAS_LVL_WIDTH                     1  /*             */

/*
                                 
 */
#define WM8962_SPKOUTR_ENA                      0x0080  /*             */
#define WM8962_SPKOUTR_ENA_MASK                 0x0080  /*             */
#define WM8962_SPKOUTR_ENA_SHIFT                     7  /*             */
#define WM8962_SPKOUTR_ENA_WIDTH                     1  /*             */
#define WM8962_SPKOUTL_ENA                      0x0040  /*             */
#define WM8962_SPKOUTL_ENA_MASK                 0x0040  /*             */
#define WM8962_SPKOUTL_ENA_SHIFT                     6  /*             */
#define WM8962_SPKOUTL_ENA_WIDTH                     1  /*             */
#define WM8962_DAC_MUTE_ALT                     0x0010  /*          */
#define WM8962_DAC_MUTE_ALT_MASK                0x0010  /*          */
#define WM8962_DAC_MUTE_ALT_SHIFT                    4  /*          */
#define WM8962_DAC_MUTE_ALT_WIDTH                    1  /*          */
#define WM8962_SPKOUTL_PGA_MUTE                 0x0002  /*                  */
#define WM8962_SPKOUTL_PGA_MUTE_MASK            0x0002  /*                  */
#define WM8962_SPKOUTL_PGA_MUTE_SHIFT                1  /*                  */
#define WM8962_SPKOUTL_PGA_MUTE_WIDTH                1  /*                  */
#define WM8962_SPKOUTR_PGA_MUTE                 0x0001  /*                  */
#define WM8962_SPKOUTR_PGA_MUTE_MASK            0x0001  /*                  */
#define WM8962_SPKOUTR_PGA_MUTE_SHIFT                0  /*                  */
#define WM8962_SPKOUTR_PGA_MUTE_WIDTH                1  /*                  */

/*
                                 
 */
#define WM8962_SPK_MONO                         0x0040  /*          */
#define WM8962_SPK_MONO_MASK                    0x0040  /*          */
#define WM8962_SPK_MONO_SHIFT                        6  /*          */
#define WM8962_SPK_MONO_WIDTH                        1  /*          */
#define WM8962_CLASSD_VOL_MASK                  0x0007  /*                    */
#define WM8962_CLASSD_VOL_SHIFT                      0  /*                    */
#define WM8962_CLASSD_VOL_WIDTH                      3  /*                    */

/*
                          
 */
#define WM8962_SYSCLK_RATE_MASK                 0x001E  /*                     */
#define WM8962_SYSCLK_RATE_SHIFT                     1  /*                     */
#define WM8962_SYSCLK_RATE_WIDTH                     4  /*                     */

/*
                                  
 */
#define WM8962_DAC_MONOMIX                      0x0200  /*             */
#define WM8962_DAC_MONOMIX_MASK                 0x0200  /*             */
#define WM8962_DAC_MONOMIX_SHIFT                     9  /*             */
#define WM8962_DAC_MONOMIX_WIDTH                     1  /*             */
#define WM8962_ADCR_DAC_SVOL_MASK               0x00F0  /*                       */
#define WM8962_ADCR_DAC_SVOL_SHIFT                   4  /*                       */
#define WM8962_ADCR_DAC_SVOL_WIDTH                   4  /*                       */
#define WM8962_ADC_TO_DACR_MASK                 0x000C  /*                     */
#define WM8962_ADC_TO_DACR_SHIFT                     2  /*                     */
#define WM8962_ADC_TO_DACR_WIDTH                     2  /*                     */

/*
                                  
 */
#define WM8962_ADCL_DAC_SVOL_MASK               0x00F0  /*                       */
#define WM8962_ADCL_DAC_SVOL_SHIFT                   4  /*                       */
#define WM8962_ADCL_DAC_SVOL_WIDTH                   4  /*                       */
#define WM8962_ADC_TO_DACL_MASK                 0x000C  /*                     */
#define WM8962_ADC_TO_DACL_SHIFT                     2  /*                     */
#define WM8962_ADC_TO_DACL_WIDTH                     2  /*                     */

/*
                          
 */
#define WM8962_INL_DCS_ENA                      0x0080  /*             */
#define WM8962_INL_DCS_ENA_MASK                 0x0080  /*             */
#define WM8962_INL_DCS_ENA_SHIFT                     7  /*             */
#define WM8962_INL_DCS_ENA_WIDTH                     1  /*             */
#define WM8962_INL_DCS_STARTUP                  0x0040  /*                 */
#define WM8962_INL_DCS_STARTUP_MASK             0x0040  /*                 */
#define WM8962_INL_DCS_STARTUP_SHIFT                 6  /*                 */
#define WM8962_INL_DCS_STARTUP_WIDTH                 1  /*                 */
#define WM8962_INR_DCS_ENA                      0x0008  /*             */
#define WM8962_INR_DCS_ENA_MASK                 0x0008  /*             */
#define WM8962_INR_DCS_ENA_SHIFT                     3  /*             */
#define WM8962_INR_DCS_ENA_WIDTH                     1  /*             */
#define WM8962_INR_DCS_STARTUP                  0x0004  /*                 */
#define WM8962_INR_DCS_STARTUP_MASK             0x0004  /*                 */
#define WM8962_INR_DCS_STARTUP_SHIFT                 2  /*                 */
#define WM8962_INR_DCS_STARTUP_WIDTH                 1  /*                 */

/*
                          
 */
#define WM8962_HP1L_DCS_ENA                     0x0080  /*              */
#define WM8962_HP1L_DCS_ENA_MASK                0x0080  /*              */
#define WM8962_HP1L_DCS_ENA_SHIFT                    7  /*              */
#define WM8962_HP1L_DCS_ENA_WIDTH                    1  /*              */
#define WM8962_HP1L_DCS_STARTUP                 0x0040  /*                  */
#define WM8962_HP1L_DCS_STARTUP_MASK            0x0040  /*                  */
#define WM8962_HP1L_DCS_STARTUP_SHIFT                6  /*                  */
#define WM8962_HP1L_DCS_STARTUP_WIDTH                1  /*                  */
#define WM8962_HP1L_DCS_SYNC                    0x0010  /*               */
#define WM8962_HP1L_DCS_SYNC_MASK               0x0010  /*               */
#define WM8962_HP1L_DCS_SYNC_SHIFT                   4  /*               */
#define WM8962_HP1L_DCS_SYNC_WIDTH                   1  /*               */
#define WM8962_HP1R_DCS_ENA                     0x0008  /*              */
#define WM8962_HP1R_DCS_ENA_MASK                0x0008  /*              */
#define WM8962_HP1R_DCS_ENA_SHIFT                    3  /*              */
#define WM8962_HP1R_DCS_ENA_WIDTH                    1  /*              */
#define WM8962_HP1R_DCS_STARTUP                 0x0004  /*                  */
#define WM8962_HP1R_DCS_STARTUP_MASK            0x0004  /*                  */
#define WM8962_HP1R_DCS_STARTUP_SHIFT                2  /*                  */
#define WM8962_HP1R_DCS_STARTUP_WIDTH                1  /*                  */
#define WM8962_HP1R_DCS_SYNC                    0x0001  /*               */
#define WM8962_HP1R_DCS_SYNC_MASK               0x0001  /*               */
#define WM8962_HP1R_DCS_SYNC_SHIFT                   0  /*               */
#define WM8962_HP1R_DCS_SYNC_WIDTH                   1  /*               */

/*
                          
 */
#define WM8962_HP1_DCS_SYNC_STEPS_MASK          0x3F80  /*                             */
#define WM8962_HP1_DCS_SYNC_STEPS_SHIFT              7  /*                             */
#define WM8962_HP1_DCS_SYNC_STEPS_WIDTH              7  /*                             */

/*
                          
 */
#define WM8962_DCS_STARTUP_DONE_INL             0x0400  /*                      */
#define WM8962_DCS_STARTUP_DONE_INL_MASK        0x0400  /*                      */
#define WM8962_DCS_STARTUP_DONE_INL_SHIFT           10  /*                      */
#define WM8962_DCS_STARTUP_DONE_INL_WIDTH            1  /*                      */
#define WM8962_DCS_STARTUP_DONE_INR             0x0200  /*                      */
#define WM8962_DCS_STARTUP_DONE_INR_MASK        0x0200  /*                      */
#define WM8962_DCS_STARTUP_DONE_INR_SHIFT            9  /*                      */
#define WM8962_DCS_STARTUP_DONE_INR_WIDTH            1  /*                      */
#define WM8962_DCS_STARTUP_DONE_HP1L            0x0100  /*                       */
#define WM8962_DCS_STARTUP_DONE_HP1L_MASK       0x0100  /*                       */
#define WM8962_DCS_STARTUP_DONE_HP1L_SHIFT           8  /*                       */
#define WM8962_DCS_STARTUP_DONE_HP1L_WIDTH           1  /*                       */
#define WM8962_DCS_STARTUP_DONE_HP1R            0x0080  /*                       */
#define WM8962_DCS_STARTUP_DONE_HP1R_MASK       0x0080  /*                       */
#define WM8962_DCS_STARTUP_DONE_HP1R_SHIFT           7  /*                       */
#define WM8962_DCS_STARTUP_DONE_HP1R_WIDTH           1  /*                       */

/*
                                 
 */
#define WM8962_HP_PGAS_BIAS_MASK                0x0007  /*                      */
#define WM8962_HP_PGAS_BIAS_SHIFT                    0  /*                      */
#define WM8962_HP_PGAS_BIAS_WIDTH                    3  /*                      */

/*
                             
 */
#define WM8962_HP1L_RMV_SHORT                   0x0080  /*                */
#define WM8962_HP1L_RMV_SHORT_MASK              0x0080  /*                */
#define WM8962_HP1L_RMV_SHORT_SHIFT                  7  /*                */
#define WM8962_HP1L_RMV_SHORT_WIDTH                  1  /*                */
#define WM8962_HP1L_ENA_OUTP                    0x0040  /*               */
#define WM8962_HP1L_ENA_OUTP_MASK               0x0040  /*               */
#define WM8962_HP1L_ENA_OUTP_SHIFT                   6  /*               */
#define WM8962_HP1L_ENA_OUTP_WIDTH                   1  /*               */
#define WM8962_HP1L_ENA_DLY                     0x0020  /*              */
#define WM8962_HP1L_ENA_DLY_MASK                0x0020  /*              */
#define WM8962_HP1L_ENA_DLY_SHIFT                    5  /*              */
#define WM8962_HP1L_ENA_DLY_WIDTH                    1  /*              */
#define WM8962_HP1L_ENA                         0x0010  /*          */
#define WM8962_HP1L_ENA_MASK                    0x0010  /*          */
#define WM8962_HP1L_ENA_SHIFT                        4  /*          */
#define WM8962_HP1L_ENA_WIDTH                        1  /*          */
#define WM8962_HP1R_RMV_SHORT                   0x0008  /*                */
#define WM8962_HP1R_RMV_SHORT_MASK              0x0008  /*                */
#define WM8962_HP1R_RMV_SHORT_SHIFT                  3  /*                */
#define WM8962_HP1R_RMV_SHORT_WIDTH                  1  /*                */
#define WM8962_HP1R_ENA_OUTP                    0x0004  /*               */
#define WM8962_HP1R_ENA_OUTP_MASK               0x0004  /*               */
#define WM8962_HP1R_ENA_OUTP_SHIFT                   2  /*               */
#define WM8962_HP1R_ENA_OUTP_WIDTH                   1  /*               */
#define WM8962_HP1R_ENA_DLY                     0x0002  /*              */
#define WM8962_HP1R_ENA_DLY_MASK                0x0002  /*              */
#define WM8962_HP1R_ENA_DLY_SHIFT                    1  /*              */
#define WM8962_HP1R_ENA_DLY_WIDTH                    1  /*              */
#define WM8962_HP1R_ENA                         0x0001  /*          */
#define WM8962_HP1R_ENA_MASK                    0x0001  /*          */
#define WM8962_HP1R_ENA_SHIFT                        0  /*          */
#define WM8962_HP1R_ENA_WIDTH                        1  /*          */

/*
                             
 */
#define WM8962_HP1L_VOL_MASK                    0x01C0  /*                  */
#define WM8962_HP1L_VOL_SHIFT                        6  /*                  */
#define WM8962_HP1L_VOL_WIDTH                        3  /*                  */
#define WM8962_HP1R_VOL_MASK                    0x0038  /*                  */
#define WM8962_HP1R_VOL_SHIFT                        3  /*                  */
#define WM8962_HP1R_VOL_WIDTH                        3  /*                  */
#define WM8962_HP_BIAS_BOOST_MASK               0x0007  /*                       */
#define WM8962_HP_BIAS_BOOST_SHIFT                   0  /*                       */
#define WM8962_HP_BIAS_BOOST_WIDTH                   3  /*                       */

/*
                             
 */
#define WM8962_CP_ENA                           0x0001  /*        */
#define WM8962_CP_ENA_MASK                      0x0001  /*        */
#define WM8962_CP_ENA_SHIFT                          0  /*        */
#define WM8962_CP_ENA_WIDTH                          1  /*        */

/*
                             
 */
#define WM8962_CP_DYN_PWR                       0x0001  /*            */
#define WM8962_CP_DYN_PWR_MASK                  0x0001  /*            */
#define WM8962_CP_DYN_PWR_SHIFT                      0  /*            */
#define WM8962_CP_DYN_PWR_WIDTH                      1  /*            */

/*
                                         
 */
#define WM8962_WSEQ_AUTOSEQ_ENA                 0x0080  /*                  */
#define WM8962_WSEQ_AUTOSEQ_ENA_MASK            0x0080  /*                  */
#define WM8962_WSEQ_AUTOSEQ_ENA_SHIFT                7  /*                  */
#define WM8962_WSEQ_AUTOSEQ_ENA_WIDTH                1  /*                  */
#define WM8962_WSEQ_ENA                         0x0020  /*          */
#define WM8962_WSEQ_ENA_MASK                    0x0020  /*          */
#define WM8962_WSEQ_ENA_SHIFT                        5  /*          */
#define WM8962_WSEQ_ENA_WIDTH                        1  /*          */

/*
                                         
 */
#define WM8962_WSEQ_ABORT                       0x0100  /*            */
#define WM8962_WSEQ_ABORT_MASK                  0x0100  /*            */
#define WM8962_WSEQ_ABORT_SHIFT                      8  /*            */
#define WM8962_WSEQ_ABORT_WIDTH                      1  /*            */
#define WM8962_WSEQ_START                       0x0080  /*            */
#define WM8962_WSEQ_START_MASK                  0x0080  /*            */
#define WM8962_WSEQ_START_SHIFT                      7  /*            */
#define WM8962_WSEQ_START_WIDTH                      1  /*            */
#define WM8962_WSEQ_START_INDEX_MASK            0x007F  /*                          */
#define WM8962_WSEQ_START_INDEX_SHIFT                0  /*                          */
#define WM8962_WSEQ_START_INDEX_WIDTH                7  /*                          */

/*
                                         
 */
#define WM8962_WSEQ_CURRENT_INDEX_MASK          0x03F8  /*                            */
#define WM8962_WSEQ_CURRENT_INDEX_SHIFT              3  /*                            */
#define WM8962_WSEQ_CURRENT_INDEX_WIDTH              7  /*                            */
#define WM8962_WSEQ_BUSY                        0x0001  /*           */
#define WM8962_WSEQ_BUSY_MASK                   0x0001  /*           */
#define WM8962_WSEQ_BUSY_SHIFT                       0  /*           */
#define WM8962_WSEQ_BUSY_WIDTH                       1  /*           */

/*
                                 
 */
#define WM8962_SPI_CONTRD                       0x0040  /*            */
#define WM8962_SPI_CONTRD_MASK                  0x0040  /*            */
#define WM8962_SPI_CONTRD_SHIFT                      6  /*            */
#define WM8962_SPI_CONTRD_WIDTH                      1  /*            */
#define WM8962_SPI_4WIRE                        0x0020  /*           */
#define WM8962_SPI_4WIRE_MASK                   0x0020  /*           */
#define WM8962_SPI_4WIRE_SHIFT                       5  /*           */
#define WM8962_SPI_4WIRE_WIDTH                       1  /*           */
#define WM8962_SPI_CFG                          0x0010  /*         */
#define WM8962_SPI_CFG_MASK                     0x0010  /*         */
#define WM8962_SPI_CFG_SHIFT                         4  /*         */
#define WM8962_SPI_CFG_WIDTH                         1  /*         */

/*
                             
 */
#define WM8962_HPMIXL_ENA                       0x0008  /*            */
#define WM8962_HPMIXL_ENA_MASK                  0x0008  /*            */
#define WM8962_HPMIXL_ENA_SHIFT                      3  /*            */
#define WM8962_HPMIXL_ENA_WIDTH                      1  /*            */
#define WM8962_HPMIXR_ENA                       0x0004  /*            */
#define WM8962_HPMIXR_ENA_MASK                  0x0004  /*            */
#define WM8962_HPMIXR_ENA_SHIFT                      2  /*            */
#define WM8962_HPMIXR_ENA_WIDTH                      1  /*            */
#define WM8962_SPKMIXL_ENA                      0x0002  /*             */
#define WM8962_SPKMIXL_ENA_MASK                 0x0002  /*             */
#define WM8962_SPKMIXL_ENA_SHIFT                     1  /*             */
#define WM8962_SPKMIXL_ENA_WIDTH                     1  /*             */
#define WM8962_SPKMIXR_ENA                      0x0001  /*             */
#define WM8962_SPKMIXR_ENA_MASK                 0x0001  /*             */
#define WM8962_SPKMIXR_ENA_SHIFT                     0  /*             */
#define WM8962_SPKMIXR_ENA_WIDTH                     1  /*             */

/*
                                    
 */
#define WM8962_HPMIXL_TO_HPOUTL_PGA             0x0080  /*                      */
#define WM8962_HPMIXL_TO_HPOUTL_PGA_MASK        0x0080  /*                      */
#define WM8962_HPMIXL_TO_HPOUTL_PGA_SHIFT            7  /*                      */
#define WM8962_HPMIXL_TO_HPOUTL_PGA_WIDTH            1  /*                      */
#define WM8962_DACL_TO_HPMIXL                   0x0020  /*                */
#define WM8962_DACL_TO_HPMIXL_MASK              0x0020  /*                */
#define WM8962_DACL_TO_HPMIXL_SHIFT                  5  /*                */
#define WM8962_DACL_TO_HPMIXL_WIDTH                  1  /*                */
#define WM8962_DACR_TO_HPMIXL                   0x0010  /*                */
#define WM8962_DACR_TO_HPMIXL_MASK              0x0010  /*                */
#define WM8962_DACR_TO_HPMIXL_SHIFT                  4  /*                */
#define WM8962_DACR_TO_HPMIXL_WIDTH                  1  /*                */
#define WM8962_MIXINL_TO_HPMIXL                 0x0008  /*                  */
#define WM8962_MIXINL_TO_HPMIXL_MASK            0x0008  /*                  */
#define WM8962_MIXINL_TO_HPMIXL_SHIFT                3  /*                  */
#define WM8962_MIXINL_TO_HPMIXL_WIDTH                1  /*                  */
#define WM8962_MIXINR_TO_HPMIXL                 0x0004  /*                  */
#define WM8962_MIXINR_TO_HPMIXL_MASK            0x0004  /*                  */
#define WM8962_MIXINR_TO_HPMIXL_SHIFT                2  /*                  */
#define WM8962_MIXINR_TO_HPMIXL_WIDTH                1  /*                  */
#define WM8962_IN4L_TO_HPMIXL                   0x0002  /*                */
#define WM8962_IN4L_TO_HPMIXL_MASK              0x0002  /*                */
#define WM8962_IN4L_TO_HPMIXL_SHIFT                  1  /*                */
#define WM8962_IN4L_TO_HPMIXL_WIDTH                  1  /*                */
#define WM8962_IN4R_TO_HPMIXL                   0x0001  /*                */
#define WM8962_IN4R_TO_HPMIXL_MASK              0x0001  /*                */
#define WM8962_IN4R_TO_HPMIXL_SHIFT                  0  /*                */
#define WM8962_IN4R_TO_HPMIXL_WIDTH                  1  /*                */

/*
                                    
 */
#define WM8962_HPMIXR_TO_HPOUTR_PGA             0x0080  /*                      */
#define WM8962_HPMIXR_TO_HPOUTR_PGA_MASK        0x0080  /*                      */
#define WM8962_HPMIXR_TO_HPOUTR_PGA_SHIFT            7  /*                      */
#define WM8962_HPMIXR_TO_HPOUTR_PGA_WIDTH            1  /*                      */
#define WM8962_DACL_TO_HPMIXR                   0x0020  /*                */
#define WM8962_DACL_TO_HPMIXR_MASK              0x0020  /*                */
#define WM8962_DACL_TO_HPMIXR_SHIFT                  5  /*                */
#define WM8962_DACL_TO_HPMIXR_WIDTH                  1  /*                */
#define WM8962_DACR_TO_HPMIXR                   0x0010  /*                */
#define WM8962_DACR_TO_HPMIXR_MASK              0x0010  /*                */
#define WM8962_DACR_TO_HPMIXR_SHIFT                  4  /*                */
#define WM8962_DACR_TO_HPMIXR_WIDTH                  1  /*                */
#define WM8962_MIXINL_TO_HPMIXR                 0x0008  /*                  */
#define WM8962_MIXINL_TO_HPMIXR_MASK            0x0008  /*                  */
#define WM8962_MIXINL_TO_HPMIXR_SHIFT                3  /*                  */
#define WM8962_MIXINL_TO_HPMIXR_WIDTH                1  /*                  */
#define WM8962_MIXINR_TO_HPMIXR                 0x0004  /*                  */
#define WM8962_MIXINR_TO_HPMIXR_MASK            0x0004  /*                  */
#define WM8962_MIXINR_TO_HPMIXR_SHIFT                2  /*                  */
#define WM8962_MIXINR_TO_HPMIXR_WIDTH                1  /*                  */
#define WM8962_IN4L_TO_HPMIXR                   0x0002  /*                */
#define WM8962_IN4L_TO_HPMIXR_MASK              0x0002  /*                */
#define WM8962_IN4L_TO_HPMIXR_SHIFT                  1  /*                */
#define WM8962_IN4L_TO_HPMIXR_WIDTH                  1  /*                */
#define WM8962_IN4R_TO_HPMIXR                   0x0001  /*                */
#define WM8962_IN4R_TO_HPMIXR_MASK              0x0001  /*                */
#define WM8962_IN4R_TO_HPMIXR_SHIFT                  0  /*                */
#define WM8962_IN4R_TO_HPMIXR_WIDTH                  1  /*                */

/*
                                    
 */
#define WM8962_HPMIXL_MUTE                      0x0100  /*             */
#define WM8962_HPMIXL_MUTE_MASK                 0x0100  /*             */
#define WM8962_HPMIXL_MUTE_SHIFT                     8  /*             */
#define WM8962_HPMIXL_MUTE_WIDTH                     1  /*             */
#define WM8962_MIXINL_HPMIXL_VOL                0x0080  /*                   */
#define WM8962_MIXINL_HPMIXL_VOL_MASK           0x0080  /*                   */
#define WM8962_MIXINL_HPMIXL_VOL_SHIFT               7  /*                   */
#define WM8962_MIXINL_HPMIXL_VOL_WIDTH               1  /*                   */
#define WM8962_MIXINR_HPMIXL_VOL                0x0040  /*                   */
#define WM8962_MIXINR_HPMIXL_VOL_MASK           0x0040  /*                   */
#define WM8962_MIXINR_HPMIXL_VOL_SHIFT               6  /*                   */
#define WM8962_MIXINR_HPMIXL_VOL_WIDTH               1  /*                   */
#define WM8962_IN4L_HPMIXL_VOL_MASK             0x0038  /*                         */
#define WM8962_IN4L_HPMIXL_VOL_SHIFT                 3  /*                         */
#define WM8962_IN4L_HPMIXL_VOL_WIDTH                 3  /*                         */
#define WM8962_IN4R_HPMIXL_VOL_MASK             0x0007  /*                         */
#define WM8962_IN4R_HPMIXL_VOL_SHIFT                 0  /*                         */
#define WM8962_IN4R_HPMIXL_VOL_WIDTH                 3  /*                         */

/*
                                    
 */
#define WM8962_HPMIXR_MUTE                      0x0100  /*             */
#define WM8962_HPMIXR_MUTE_MASK                 0x0100  /*             */
#define WM8962_HPMIXR_MUTE_SHIFT                     8  /*             */
#define WM8962_HPMIXR_MUTE_WIDTH                     1  /*             */
#define WM8962_MIXINL_HPMIXR_VOL                0x0080  /*                   */
#define WM8962_MIXINL_HPMIXR_VOL_MASK           0x0080  /*                   */
#define WM8962_MIXINL_HPMIXR_VOL_SHIFT               7  /*                   */
#define WM8962_MIXINL_HPMIXR_VOL_WIDTH               1  /*                   */
#define WM8962_MIXINR_HPMIXR_VOL                0x0040  /*                   */
#define WM8962_MIXINR_HPMIXR_VOL_MASK           0x0040  /*                   */
#define WM8962_MIXINR_HPMIXR_VOL_SHIFT               6  /*                   */
#define WM8962_MIXINR_HPMIXR_VOL_WIDTH               1  /*                   */
#define WM8962_IN4L_HPMIXR_VOL_MASK             0x0038  /*                         */
#define WM8962_IN4L_HPMIXR_VOL_SHIFT                 3  /*                         */
#define WM8962_IN4L_HPMIXR_VOL_WIDTH                 3  /*                         */
#define WM8962_IN4R_HPMIXR_VOL_MASK             0x0007  /*                         */
#define WM8962_IN4R_HPMIXR_VOL_SHIFT                 0  /*                         */
#define WM8962_IN4R_HPMIXR_VOL_WIDTH                 3  /*                         */

/*
                                  
 */
#define WM8962_SPKMIXL_TO_SPKOUTL_PGA           0x0080  /*                        */
#define WM8962_SPKMIXL_TO_SPKOUTL_PGA_MASK      0x0080  /*                        */
#define WM8962_SPKMIXL_TO_SPKOUTL_PGA_SHIFT          7  /*                        */
#define WM8962_SPKMIXL_TO_SPKOUTL_PGA_WIDTH          1  /*                        */
#define WM8962_DACL_TO_SPKMIXL                  0x0020  /*                 */
#define WM8962_DACL_TO_SPKMIXL_MASK             0x0020  /*                 */
#define WM8962_DACL_TO_SPKMIXL_SHIFT                 5  /*                 */
#define WM8962_DACL_TO_SPKMIXL_WIDTH                 1  /*                 */
#define WM8962_DACR_TO_SPKMIXL                  0x0010  /*                 */
#define WM8962_DACR_TO_SPKMIXL_MASK             0x0010  /*                 */
#define WM8962_DACR_TO_SPKMIXL_SHIFT                 4  /*                 */
#define WM8962_DACR_TO_SPKMIXL_WIDTH                 1  /*                 */
#define WM8962_MIXINL_TO_SPKMIXL                0x0008  /*                   */
#define WM8962_MIXINL_TO_SPKMIXL_MASK           0x0008  /*                   */
#define WM8962_MIXINL_TO_SPKMIXL_SHIFT               3  /*                   */
#define WM8962_MIXINL_TO_SPKMIXL_WIDTH               1  /*                   */
#define WM8962_MIXINR_TO_SPKMIXL                0x0004  /*                   */
#define WM8962_MIXINR_TO_SPKMIXL_MASK           0x0004  /*                   */
#define WM8962_MIXINR_TO_SPKMIXL_SHIFT               2  /*                   */
#define WM8962_MIXINR_TO_SPKMIXL_WIDTH               1  /*                   */
#define WM8962_IN4L_TO_SPKMIXL                  0x0002  /*                 */
#define WM8962_IN4L_TO_SPKMIXL_MASK             0x0002  /*                 */
#define WM8962_IN4L_TO_SPKMIXL_SHIFT                 1  /*                 */
#define WM8962_IN4L_TO_SPKMIXL_WIDTH                 1  /*                 */
#define WM8962_IN4R_TO_SPKMIXL                  0x0001  /*                 */
#define WM8962_IN4R_TO_SPKMIXL_MASK             0x0001  /*                 */
#define WM8962_IN4R_TO_SPKMIXL_SHIFT                 0  /*                 */
#define WM8962_IN4R_TO_SPKMIXL_WIDTH                 1  /*                 */

/*
                                  
 */
#define WM8962_SPKMIXR_TO_SPKOUTR_PGA           0x0080  /*                        */
#define WM8962_SPKMIXR_TO_SPKOUTR_PGA_MASK      0x0080  /*                        */
#define WM8962_SPKMIXR_TO_SPKOUTR_PGA_SHIFT          7  /*                        */
#define WM8962_SPKMIXR_TO_SPKOUTR_PGA_WIDTH          1  /*                        */
#define WM8962_DACL_TO_SPKMIXR                  0x0020  /*                 */
#define WM8962_DACL_TO_SPKMIXR_MASK             0x0020  /*                 */
#define WM8962_DACL_TO_SPKMIXR_SHIFT                 5  /*                 */
#define WM8962_DACL_TO_SPKMIXR_WIDTH                 1  /*                 */
#define WM8962_DACR_TO_SPKMIXR                  0x0010  /*                 */
#define WM8962_DACR_TO_SPKMIXR_MASK             0x0010  /*                 */
#define WM8962_DACR_TO_SPKMIXR_SHIFT                 4  /*                 */
#define WM8962_DACR_TO_SPKMIXR_WIDTH                 1  /*                 */
#define WM8962_MIXINL_TO_SPKMIXR                0x0008  /*                   */
#define WM8962_MIXINL_TO_SPKMIXR_MASK           0x0008  /*                   */
#define WM8962_MIXINL_TO_SPKMIXR_SHIFT               3  /*                   */
#define WM8962_MIXINL_TO_SPKMIXR_WIDTH               1  /*                   */
#define WM8962_MIXINR_TO_SPKMIXR                0x0004  /*                   */
#define WM8962_MIXINR_TO_SPKMIXR_MASK           0x0004  /*                   */
#define WM8962_MIXINR_TO_SPKMIXR_SHIFT               2  /*                   */
#define WM8962_MIXINR_TO_SPKMIXR_WIDTH               1  /*                   */
#define WM8962_IN4L_TO_SPKMIXR                  0x0002  /*                 */
#define WM8962_IN4L_TO_SPKMIXR_MASK             0x0002  /*                 */
#define WM8962_IN4L_TO_SPKMIXR_SHIFT                 1  /*                 */
#define WM8962_IN4L_TO_SPKMIXR_WIDTH                 1  /*                 */
#define WM8962_IN4R_TO_SPKMIXR                  0x0001  /*                 */
#define WM8962_IN4R_TO_SPKMIXR_MASK             0x0001  /*                 */
#define WM8962_IN4R_TO_SPKMIXR_SHIFT                 0  /*                 */
#define WM8962_IN4R_TO_SPKMIXR_WIDTH                 1  /*                 */

/*
                                  
 */
#define WM8962_SPKMIXL_MUTE                     0x0100  /*              */
#define WM8962_SPKMIXL_MUTE_MASK                0x0100  /*              */
#define WM8962_SPKMIXL_MUTE_SHIFT                    8  /*              */
#define WM8962_SPKMIXL_MUTE_WIDTH                    1  /*              */
#define WM8962_MIXINL_SPKMIXL_VOL               0x0080  /*                    */
#define WM8962_MIXINL_SPKMIXL_VOL_MASK          0x0080  /*                    */
#define WM8962_MIXINL_SPKMIXL_VOL_SHIFT              7  /*                    */
#define WM8962_MIXINL_SPKMIXL_VOL_WIDTH              1  /*                    */
#define WM8962_MIXINR_SPKMIXL_VOL               0x0040  /*                    */
#define WM8962_MIXINR_SPKMIXL_VOL_MASK          0x0040  /*                    */
#define WM8962_MIXINR_SPKMIXL_VOL_SHIFT              6  /*                    */
#define WM8962_MIXINR_SPKMIXL_VOL_WIDTH              1  /*                    */
#define WM8962_IN4L_SPKMIXL_VOL_MASK            0x0038  /*                          */
#define WM8962_IN4L_SPKMIXL_VOL_SHIFT                3  /*                          */
#define WM8962_IN4L_SPKMIXL_VOL_WIDTH                3  /*                          */
#define WM8962_IN4R_SPKMIXL_VOL_MASK            0x0007  /*                          */
#define WM8962_IN4R_SPKMIXL_VOL_SHIFT                0  /*                          */
#define WM8962_IN4R_SPKMIXL_VOL_WIDTH                3  /*                          */

/*
                                  
 */
#define WM8962_SPKMIXR_MUTE                     0x0100  /*              */
#define WM8962_SPKMIXR_MUTE_MASK                0x0100  /*              */
#define WM8962_SPKMIXR_MUTE_SHIFT                    8  /*              */
#define WM8962_SPKMIXR_MUTE_WIDTH                    1  /*              */
#define WM8962_MIXINL_SPKMIXR_VOL               0x0080  /*                    */
#define WM8962_MIXINL_SPKMIXR_VOL_MASK          0x0080  /*                    */
#define WM8962_MIXINL_SPKMIXR_VOL_SHIFT              7  /*                    */
#define WM8962_MIXINL_SPKMIXR_VOL_WIDTH              1  /*                    */
#define WM8962_MIXINR_SPKMIXR_VOL               0x0040  /*                    */
#define WM8962_MIXINR_SPKMIXR_VOL_MASK          0x0040  /*                    */
#define WM8962_MIXINR_SPKMIXR_VOL_SHIFT              6  /*                    */
#define WM8962_MIXINR_SPKMIXR_VOL_WIDTH              1  /*                    */
#define WM8962_IN4L_SPKMIXR_VOL_MASK            0x0038  /*                          */
#define WM8962_IN4L_SPKMIXR_VOL_SHIFT                3  /*                          */
#define WM8962_IN4L_SPKMIXR_VOL_WIDTH                3  /*                          */
#define WM8962_IN4R_SPKMIXR_VOL_MASK            0x0007  /*                          */
#define WM8962_IN4R_SPKMIXR_VOL_SHIFT                0  /*                          */
#define WM8962_IN4R_SPKMIXR_VOL_WIDTH                3  /*                          */

/*
                                  
 */
#define WM8962_DACL_SPKMIXL_VOL                 0x0080  /*                  */
#define WM8962_DACL_SPKMIXL_VOL_MASK            0x0080  /*                  */
#define WM8962_DACL_SPKMIXL_VOL_SHIFT                7  /*                  */
#define WM8962_DACL_SPKMIXL_VOL_WIDTH                1  /*                  */
#define WM8962_DACR_SPKMIXL_VOL                 0x0040  /*                  */
#define WM8962_DACR_SPKMIXL_VOL_MASK            0x0040  /*                  */
#define WM8962_DACR_SPKMIXL_VOL_SHIFT                6  /*                  */
#define WM8962_DACR_SPKMIXL_VOL_WIDTH                1  /*                  */
#define WM8962_DACL_SPKMIXR_VOL                 0x0020  /*                  */
#define WM8962_DACL_SPKMIXR_VOL_MASK            0x0020  /*                  */
#define WM8962_DACL_SPKMIXR_VOL_SHIFT                5  /*                  */
#define WM8962_DACL_SPKMIXR_VOL_WIDTH                1  /*                  */
#define WM8962_DACR_SPKMIXR_VOL                 0x0010  /*                  */
#define WM8962_DACR_SPKMIXR_VOL_MASK            0x0010  /*                  */
#define WM8962_DACR_SPKMIXR_VOL_SHIFT                4  /*                  */
#define WM8962_DACR_SPKMIXR_VOL_WIDTH                1  /*                  */

/*
                                   
 */
#define WM8962_BEEP_GAIN_MASK                   0x00F0  /*                   */
#define WM8962_BEEP_GAIN_SHIFT                       4  /*                   */
#define WM8962_BEEP_GAIN_WIDTH                       4  /*                   */
#define WM8962_BEEP_RATE_MASK                   0x0006  /*                   */
#define WM8962_BEEP_RATE_SHIFT                       1  /*                   */
#define WM8962_BEEP_RATE_WIDTH                       2  /*                   */
#define WM8962_BEEP_ENA                         0x0001  /*          */
#define WM8962_BEEP_ENA_MASK                    0x0001  /*          */
#define WM8962_BEEP_ENA_SHIFT                        0  /*          */
#define WM8962_BEEP_ENA_WIDTH                        1  /*          */

/*
                                    
 */
#define WM8962_OSC_TRIM_XTI_MASK                0x001F  /*                      */
#define WM8962_OSC_TRIM_XTI_SHIFT                    0  /*                      */
#define WM8962_OSC_TRIM_XTI_WIDTH                    5  /*                      */

/*
                                    
 */
#define WM8962_OSC_TRIM_XTO_MASK                0x001F  /*                      */
#define WM8962_OSC_TRIM_XTO_SHIFT                    0  /*                      */
#define WM8962_OSC_TRIM_XTO_WIDTH                    5  /*                      */

/*
                                    
 */
#define WM8962_XTO_CAP_SEL_MASK                 0x00F0  /*                     */
#define WM8962_XTO_CAP_SEL_SHIFT                     4  /*                     */
#define WM8962_XTO_CAP_SEL_WIDTH                     4  /*                     */
#define WM8962_XTI_CAP_SEL_MASK                 0x000F  /*                     */
#define WM8962_XTI_CAP_SEL_SHIFT                     0  /*                     */
#define WM8962_XTI_CAP_SEL_WIDTH                     4  /*                     */

/*
                                   
 */
#define WM8962_CLKOUT2_SEL_MASK                 0x0060  /*                     */
#define WM8962_CLKOUT2_SEL_SHIFT                     5  /*                     */
#define WM8962_CLKOUT2_SEL_WIDTH                     2  /*                     */
#define WM8962_CLKOUT3_SEL_MASK                 0x0018  /*                     */
#define WM8962_CLKOUT3_SEL_SHIFT                     3  /*                     */
#define WM8962_CLKOUT3_SEL_WIDTH                     2  /*                     */
#define WM8962_CLKOUT5_SEL                      0x0001  /*             */
#define WM8962_CLKOUT5_SEL_MASK                 0x0001  /*             */
#define WM8962_CLKOUT5_SEL_SHIFT                     0  /*             */
#define WM8962_CLKOUT5_SEL_WIDTH                     1  /*             */

/*
                                   
 */
#define WM8962_PLL2_OUTDIV                      0x0080  /*             */
#define WM8962_PLL2_OUTDIV_MASK                 0x0080  /*             */
#define WM8962_PLL2_OUTDIV_SHIFT                     7  /*             */
#define WM8962_PLL2_OUTDIV_WIDTH                     1  /*             */
#define WM8962_PLL3_OUTDIV                      0x0040  /*             */
#define WM8962_PLL3_OUTDIV_MASK                 0x0040  /*             */
#define WM8962_PLL3_OUTDIV_SHIFT                     6  /*             */
#define WM8962_PLL3_OUTDIV_WIDTH                     1  /*             */
#define WM8962_PLL_SYSCLK_DIV_MASK              0x0018  /*                        */
#define WM8962_PLL_SYSCLK_DIV_SHIFT                  3  /*                        */
#define WM8962_PLL_SYSCLK_DIV_WIDTH                  2  /*                        */
#define WM8962_CLKOUT3_DIV                      0x0004  /*             */
#define WM8962_CLKOUT3_DIV_MASK                 0x0004  /*             */
#define WM8962_CLKOUT3_DIV_SHIFT                     2  /*             */
#define WM8962_CLKOUT3_DIV_WIDTH                     1  /*             */
#define WM8962_CLKOUT2_DIV                      0x0002  /*             */
#define WM8962_CLKOUT2_DIV_MASK                 0x0002  /*             */
#define WM8962_CLKOUT2_DIV_SHIFT                     1  /*             */
#define WM8962_CLKOUT2_DIV_WIDTH                     1  /*             */
#define WM8962_CLKOUT5_DIV                      0x0001  /*             */
#define WM8962_CLKOUT5_DIV_MASK                 0x0001  /*             */
#define WM8962_CLKOUT5_DIV_SHIFT                     0  /*             */
#define WM8962_CLKOUT5_DIV_WIDTH                     1  /*             */

/*
                                   
 */
#define WM8962_CLKOUT2_OE                       0x0008  /*            */
#define WM8962_CLKOUT2_OE_MASK                  0x0008  /*            */
#define WM8962_CLKOUT2_OE_SHIFT                      3  /*            */
#define WM8962_CLKOUT2_OE_WIDTH                      1  /*            */
#define WM8962_CLKOUT3_OE                       0x0004  /*            */
#define WM8962_CLKOUT3_OE_MASK                  0x0004  /*            */
#define WM8962_CLKOUT3_OE_SHIFT                      2  /*            */
#define WM8962_CLKOUT3_OE_WIDTH                      1  /*            */
#define WM8962_CLKOUT5_OE                       0x0001  /*            */
#define WM8962_CLKOUT5_OE_MASK                  0x0001  /*            */
#define WM8962_CLKOUT5_OE_SHIFT                      0  /*            */
#define WM8962_CLKOUT5_OE_WIDTH                      1  /*            */

/*
                                   
 */
#define WM8962_SW_RESET_PLL_MASK                0xFFFF  /*                       */
#define WM8962_SW_RESET_PLL_SHIFT                    0  /*                       */
#define WM8962_SW_RESET_PLL_WIDTH                   16  /*                       */

/*
                     
 */
#define WM8962_OSC_ENA                          0x0080  /*         */
#define WM8962_OSC_ENA_MASK                     0x0080  /*         */
#define WM8962_OSC_ENA_SHIFT                         7  /*         */
#define WM8962_OSC_ENA_WIDTH                         1  /*         */
#define WM8962_PLL2_ENA                         0x0020  /*          */
#define WM8962_PLL2_ENA_MASK                    0x0020  /*          */
#define WM8962_PLL2_ENA_SHIFT                        5  /*          */
#define WM8962_PLL2_ENA_WIDTH                        1  /*          */
#define WM8962_PLL3_ENA                         0x0010  /*          */
#define WM8962_PLL3_ENA_MASK                    0x0010  /*          */
#define WM8962_PLL3_ENA_SHIFT                        4  /*          */
#define WM8962_PLL3_ENA_WIDTH                        1  /*          */

/*
                      
 */
#define WM8962_PLL_CLK_SRC                      0x0002  /*             */
#define WM8962_PLL_CLK_SRC_MASK                 0x0002  /*             */
#define WM8962_PLL_CLK_SRC_SHIFT                     1  /*             */
#define WM8962_PLL_CLK_SRC_WIDTH                     1  /*             */
#define WM8962_FLL_TO_PLL3                      0x0001  /*             */
#define WM8962_FLL_TO_PLL3_MASK                 0x0001  /*             */
#define WM8962_FLL_TO_PLL3_SHIFT                     0  /*             */
#define WM8962_FLL_TO_PLL3_WIDTH                     1  /*             */

/*
                      
 */
#define WM8962_PLL2_FRAC                        0x0040  /*           */
#define WM8962_PLL2_FRAC_MASK                   0x0040  /*           */
#define WM8962_PLL2_FRAC_SHIFT                       6  /*           */
#define WM8962_PLL2_FRAC_WIDTH                       1  /*           */
#define WM8962_PLL2_N_MASK                      0x001F  /*                */
#define WM8962_PLL2_N_SHIFT                          0  /*                */
#define WM8962_PLL2_N_WIDTH                          5  /*                */

/*
                       
 */
#define WM8962_PLL2_K_MASK                      0x00FF  /*                */
#define WM8962_PLL2_K_SHIFT                          0  /*                */
#define WM8962_PLL2_K_WIDTH                          8  /*                */

/*
                       
 */
#define WM8962_PLL2_K_MASK                      0x00FF  /*                */
#define WM8962_PLL2_K_SHIFT                          0  /*                */
#define WM8962_PLL2_K_WIDTH                          8  /*                */

/*
                       
 */
#define WM8962_PLL2_K_MASK                      0x00FF  /*                */
#define WM8962_PLL2_K_SHIFT                          0  /*                */
#define WM8962_PLL2_K_WIDTH                          8  /*                */

/*
                       
 */
#define WM8962_PLL3_FRAC                        0x0040  /*           */
#define WM8962_PLL3_FRAC_MASK                   0x0040  /*           */
#define WM8962_PLL3_FRAC_SHIFT                       6  /*           */
#define WM8962_PLL3_FRAC_WIDTH                       1  /*           */
#define WM8962_PLL3_N_MASK                      0x001F  /*                */
#define WM8962_PLL3_N_SHIFT                          0  /*                */
#define WM8962_PLL3_N_WIDTH                          5  /*                */

/*
                       
 */
#define WM8962_PLL3_K_MASK                      0x00FF  /*                */
#define WM8962_PLL3_K_SHIFT                          0  /*                */
#define WM8962_PLL3_K_WIDTH                          8  /*                */

/*
                       
 */
#define WM8962_PLL3_K_MASK                      0x00FF  /*                */
#define WM8962_PLL3_K_SHIFT                          0  /*                */
#define WM8962_PLL3_K_WIDTH                          8  /*                */

/*
                       
 */
#define WM8962_PLL3_K_MASK                      0x00FF  /*                */
#define WM8962_PLL3_K_SHIFT                          0  /*                */
#define WM8962_PLL3_K_WIDTH                          8  /*                */

/*
                                
 */
#define WM8962_FLL_REFCLK_SRC_MASK              0x0060  /*                        */
#define WM8962_FLL_REFCLK_SRC_SHIFT                  5  /*                        */
#define WM8962_FLL_REFCLK_SRC_WIDTH                  2  /*                        */
#define WM8962_FLL_FRAC                         0x0004  /*          */
#define WM8962_FLL_FRAC_MASK                    0x0004  /*          */
#define WM8962_FLL_FRAC_SHIFT                        2  /*          */
#define WM8962_FLL_FRAC_WIDTH                        1  /*          */
#define WM8962_FLL_OSC_ENA                      0x0002  /*             */
#define WM8962_FLL_OSC_ENA_MASK                 0x0002  /*             */
#define WM8962_FLL_OSC_ENA_SHIFT                     1  /*             */
#define WM8962_FLL_OSC_ENA_WIDTH                     1  /*             */
#define WM8962_FLL_ENA                          0x0001  /*         */
#define WM8962_FLL_ENA_MASK                     0x0001  /*         */
#define WM8962_FLL_ENA_SHIFT                         0  /*         */
#define WM8962_FLL_ENA_WIDTH                         1  /*         */

/*
                                
 */
#define WM8962_FLL_OUTDIV_MASK                  0x01F8  /*                    */
#define WM8962_FLL_OUTDIV_SHIFT                      3  /*                    */
#define WM8962_FLL_OUTDIV_WIDTH                      6  /*                    */
#define WM8962_FLL_REFCLK_DIV_MASK              0x0003  /*                        */
#define WM8962_FLL_REFCLK_DIV_SHIFT                  0  /*                        */
#define WM8962_FLL_REFCLK_DIV_WIDTH                  2  /*                        */

/*
                                
 */
#define WM8962_FLL_FRATIO_MASK                  0x0007  /*                    */
#define WM8962_FLL_FRATIO_SHIFT                      0  /*                    */
#define WM8962_FLL_FRATIO_WIDTH                      3  /*                    */

/*
                                
 */
#define WM8962_FLL_FRC_NCO_VAL_MASK             0x007E  /*                         */
#define WM8962_FLL_FRC_NCO_VAL_SHIFT                 1  /*                         */
#define WM8962_FLL_FRC_NCO_VAL_WIDTH                 6  /*                         */
#define WM8962_FLL_FRC_NCO                      0x0001  /*             */
#define WM8962_FLL_FRC_NCO_MASK                 0x0001  /*             */
#define WM8962_FLL_FRC_NCO_SHIFT                     0  /*             */
#define WM8962_FLL_FRC_NCO_WIDTH                     1  /*             */

/*
                                
 */
#define WM8962_FLL_THETA_MASK                   0xFFFF  /*                    */
#define WM8962_FLL_THETA_SHIFT                       0  /*                    */
#define WM8962_FLL_THETA_WIDTH                      16  /*                    */

/*
                                
 */
#define WM8962_FLL_LAMBDA_MASK                  0xFFFF  /*                     */
#define WM8962_FLL_LAMBDA_SHIFT                      0  /*                     */
#define WM8962_FLL_LAMBDA_WIDTH                     16  /*                     */

/*
                                
 */
#define WM8962_FLL_N_MASK                       0x03FF  /*               */
#define WM8962_FLL_N_SHIFT                           0  /*               */
#define WM8962_FLL_N_WIDTH                          10  /*               */

/*
                               
 */
#define WM8962_REG_SYNC                         0x0004  /*          */
#define WM8962_REG_SYNC_MASK                    0x0004  /*          */
#define WM8962_REG_SYNC_SHIFT                        2  /*          */
#define WM8962_REG_SYNC_WIDTH                        1  /*          */
#define WM8962_AUTO_INC                         0x0001  /*          */
#define WM8962_AUTO_INC_MASK                    0x0001  /*          */
#define WM8962_AUTO_INC_SHIFT                        0  /*          */
#define WM8962_AUTO_INC_WIDTH                        1  /*          */

/*
                     
 */
#define WM8962_DRC_DF1_ENA                      0x0008  /*             */
#define WM8962_DRC_DF1_ENA_MASK                 0x0008  /*             */
#define WM8962_DRC_DF1_ENA_SHIFT                     3  /*             */
#define WM8962_DRC_DF1_ENA_WIDTH                     1  /*             */
#define WM8962_DF1_SHARED_COEFF                 0x0004  /*                  */
#define WM8962_DF1_SHARED_COEFF_MASK            0x0004  /*                  */
#define WM8962_DF1_SHARED_COEFF_SHIFT                2  /*                  */
#define WM8962_DF1_SHARED_COEFF_WIDTH                1  /*                  */
#define WM8962_DF1_SHARED_COEFF_SEL             0x0002  /*                      */
#define WM8962_DF1_SHARED_COEFF_SEL_MASK        0x0002  /*                      */
#define WM8962_DF1_SHARED_COEFF_SEL_SHIFT            1  /*                      */
#define WM8962_DF1_SHARED_COEFF_SEL_WIDTH            1  /*                      */
#define WM8962_DF1_ENA                          0x0001  /*         */
#define WM8962_DF1_ENA_MASK                     0x0001  /*         */
#define WM8962_DF1_ENA_SHIFT                         0  /*         */
#define WM8962_DF1_ENA_WIDTH                         1  /*         */

/*
                     
 */
#define WM8962_DF1_COEFF_L0_MASK                0xFFFF  /*                       */
#define WM8962_DF1_COEFF_L0_SHIFT                    0  /*                       */
#define WM8962_DF1_COEFF_L0_WIDTH                   16  /*                       */

/*
                     
 */
#define WM8962_DF1_COEFF_L1_MASK                0xFFFF  /*                       */
#define WM8962_DF1_COEFF_L1_SHIFT                    0  /*                       */
#define WM8962_DF1_COEFF_L1_WIDTH                   16  /*                       */

/*
                     
 */
#define WM8962_DF1_COEFF_L2_MASK                0xFFFF  /*                       */
#define WM8962_DF1_COEFF_L2_SHIFT                    0  /*                       */
#define WM8962_DF1_COEFF_L2_WIDTH                   16  /*                       */

/*
                     
 */
#define WM8962_DF1_COEFF_R0_MASK                0xFFFF  /*                       */
#define WM8962_DF1_COEFF_R0_SHIFT                    0  /*                       */
#define WM8962_DF1_COEFF_R0_WIDTH                   16  /*                       */

/*
                     
 */
#define WM8962_DF1_COEFF_R1_MASK                0xFFFF  /*                       */
#define WM8962_DF1_COEFF_R1_SHIFT                    0  /*                       */
#define WM8962_DF1_COEFF_R1_WIDTH                   16  /*                       */

/*
                     
 */
#define WM8962_DF1_COEFF_R2_MASK                0xFFFF  /*                       */
#define WM8962_DF1_COEFF_R2_SHIFT                    0  /*                       */
#define WM8962_DF1_COEFF_R2_WIDTH                   16  /*                       */

/*
                       
 */
#define WM8962_LHPF_MODE                        0x0002  /*           */
#define WM8962_LHPF_MODE_MASK                   0x0002  /*           */
#define WM8962_LHPF_MODE_SHIFT                       1  /*           */
#define WM8962_LHPF_MODE_WIDTH                       1  /*           */
#define WM8962_LHPF_ENA                         0x0001  /*          */
#define WM8962_LHPF_ENA_MASK                    0x0001  /*          */
#define WM8962_LHPF_ENA_SHIFT                        0  /*          */
#define WM8962_LHPF_ENA_WIDTH                        1  /*          */

/*
                       
 */
#define WM8962_LHPF_COEFF_MASK                  0xFFFF  /*                     */
#define WM8962_LHPF_COEFF_SHIFT                      0  /*                     */
#define WM8962_LHPF_COEFF_WIDTH                     16  /*                     */

/*
                         
 */
#define WM8962_ADC_MONOMIX                      0x0040  /*             */
#define WM8962_ADC_MONOMIX_MASK                 0x0040  /*             */
#define WM8962_ADC_MONOMIX_SHIFT                     6  /*             */
#define WM8962_ADC_MONOMIX_WIDTH                     1  /*             */
#define WM8962_THREED_SIGN_L                    0x0020  /*               */
#define WM8962_THREED_SIGN_L_MASK               0x0020  /*               */
#define WM8962_THREED_SIGN_L_SHIFT                   5  /*               */
#define WM8962_THREED_SIGN_L_WIDTH                   1  /*               */
#define WM8962_THREED_SIGN_R                    0x0010  /*               */
#define WM8962_THREED_SIGN_R_MASK               0x0010  /*               */
#define WM8962_THREED_SIGN_R_SHIFT                   4  /*               */
#define WM8962_THREED_SIGN_R_WIDTH                   1  /*               */
#define WM8962_THREED_LHPF_MODE                 0x0004  /*                  */
#define WM8962_THREED_LHPF_MODE_MASK            0x0004  /*                  */
#define WM8962_THREED_LHPF_MODE_SHIFT                2  /*                  */
#define WM8962_THREED_LHPF_MODE_WIDTH                1  /*                  */
#define WM8962_THREED_LHPF_ENA                  0x0002  /*                 */
#define WM8962_THREED_LHPF_ENA_MASK             0x0002  /*                 */
#define WM8962_THREED_LHPF_ENA_SHIFT                 1  /*                 */
#define WM8962_THREED_LHPF_ENA_WIDTH                 1  /*                 */
#define WM8962_THREED_ENA                       0x0001  /*            */
#define WM8962_THREED_ENA_MASK                  0x0001  /*            */
#define WM8962_THREED_ENA_SHIFT                      0  /*            */
#define WM8962_THREED_ENA_WIDTH                      1  /*            */

/*
                         
 */
#define WM8962_THREED_FGAINL_MASK               0xF800  /*                         */
#define WM8962_THREED_FGAINL_SHIFT                  11  /*                         */
#define WM8962_THREED_FGAINL_WIDTH                   5  /*                         */
#define WM8962_THREED_CGAINL_MASK               0x07C0  /*                        */
#define WM8962_THREED_CGAINL_SHIFT                   6  /*                        */
#define WM8962_THREED_CGAINL_WIDTH                   5  /*                        */
#define WM8962_THREED_DELAYL_MASK               0x003C  /*                       */
#define WM8962_THREED_DELAYL_SHIFT                   2  /*                       */
#define WM8962_THREED_DELAYL_WIDTH                   4  /*                       */

/*
                         
 */
#define WM8962_THREED_LHPF_COEFF_MASK           0xFFFF  /*                            */
#define WM8962_THREED_LHPF_COEFF_SHIFT               0  /*                            */
#define WM8962_THREED_LHPF_COEFF_WIDTH              16  /*                            */

/*
                         
 */
#define WM8962_THREED_FGAINR_MASK               0xF800  /*                         */
#define WM8962_THREED_FGAINR_SHIFT                  11  /*                         */
#define WM8962_THREED_FGAINR_WIDTH                   5  /*                         */
#define WM8962_THREED_CGAINR_MASK               0x07C0  /*                        */
#define WM8962_THREED_CGAINR_SHIFT                   6  /*                        */
#define WM8962_THREED_CGAINR_WIDTH                   5  /*                        */
#define WM8962_THREED_DELAYR_MASK               0x003C  /*                       */
#define WM8962_THREED_DELAYR_SHIFT                   2  /*                       */
#define WM8962_THREED_DELAYR_WIDTH                   4  /*                       */

/*
                       
 */
#define WM8962_DRC_SIG_DET_RMS_MASK             0x7C00  /*                           */
#define WM8962_DRC_SIG_DET_RMS_SHIFT                10  /*                           */
#define WM8962_DRC_SIG_DET_RMS_WIDTH                 5  /*                           */
#define WM8962_DRC_SIG_DET_PK_MASK              0x0300  /*                        */
#define WM8962_DRC_SIG_DET_PK_SHIFT                  8  /*                        */
#define WM8962_DRC_SIG_DET_PK_WIDTH                  2  /*                        */
#define WM8962_DRC_NG_ENA                       0x0080  /*            */
#define WM8962_DRC_NG_ENA_MASK                  0x0080  /*            */
#define WM8962_DRC_NG_ENA_SHIFT                      7  /*            */
#define WM8962_DRC_NG_ENA_WIDTH                      1  /*            */
#define WM8962_DRC_SIG_DET_MODE                 0x0040  /*                  */
#define WM8962_DRC_SIG_DET_MODE_MASK            0x0040  /*                  */
#define WM8962_DRC_SIG_DET_MODE_SHIFT                6  /*                  */
#define WM8962_DRC_SIG_DET_MODE_WIDTH                1  /*                  */
#define WM8962_DRC_SIG_DET                      0x0020  /*             */
#define WM8962_DRC_SIG_DET_MASK                 0x0020  /*             */
#define WM8962_DRC_SIG_DET_SHIFT                     5  /*             */
#define WM8962_DRC_SIG_DET_WIDTH                     1  /*             */
#define WM8962_DRC_KNEE2_OP_ENA                 0x0010  /*                  */
#define WM8962_DRC_KNEE2_OP_ENA_MASK            0x0010  /*                  */
#define WM8962_DRC_KNEE2_OP_ENA_SHIFT                4  /*                  */
#define WM8962_DRC_KNEE2_OP_ENA_WIDTH                1  /*                  */
#define WM8962_DRC_QR                           0x0008  /*        */
#define WM8962_DRC_QR_MASK                      0x0008  /*        */
#define WM8962_DRC_QR_SHIFT                          3  /*        */
#define WM8962_DRC_QR_WIDTH                          1  /*        */
#define WM8962_DRC_ANTICLIP                     0x0004  /*              */
#define WM8962_DRC_ANTICLIP_MASK                0x0004  /*              */
#define WM8962_DRC_ANTICLIP_SHIFT                    2  /*              */
#define WM8962_DRC_ANTICLIP_WIDTH                    1  /*              */
#define WM8962_DRC_MODE                         0x0002  /*          */
#define WM8962_DRC_MODE_MASK                    0x0002  /*          */
#define WM8962_DRC_MODE_SHIFT                        1  /*          */
#define WM8962_DRC_MODE_WIDTH                        1  /*          */
#define WM8962_DRC_ENA                          0x0001  /*         */
#define WM8962_DRC_ENA_MASK                     0x0001  /*         */
#define WM8962_DRC_ENA_SHIFT                         0  /*         */
#define WM8962_DRC_ENA_WIDTH                         1  /*         */

/*
                       
 */
#define WM8962_DRC_ATK_MASK                     0x1E00  /*                  */
#define WM8962_DRC_ATK_SHIFT                         9  /*                  */
#define WM8962_DRC_ATK_WIDTH                         4  /*                  */
#define WM8962_DRC_DCY_MASK                     0x01E0  /*                 */
#define WM8962_DRC_DCY_SHIFT                         5  /*                 */
#define WM8962_DRC_DCY_WIDTH                         4  /*                 */
#define WM8962_DRC_MINGAIN_MASK                 0x001C  /*                     */
#define WM8962_DRC_MINGAIN_SHIFT                     2  /*                     */
#define WM8962_DRC_MINGAIN_WIDTH                     3  /*                     */
#define WM8962_DRC_MAXGAIN_MASK                 0x0003  /*                     */
#define WM8962_DRC_MAXGAIN_SHIFT                     0  /*                     */
#define WM8962_DRC_MAXGAIN_WIDTH                     2  /*                     */

/*
                       
 */
#define WM8962_DRC_NG_MINGAIN_MASK              0xF000  /*                          */
#define WM8962_DRC_NG_MINGAIN_SHIFT                 12  /*                          */
#define WM8962_DRC_NG_MINGAIN_WIDTH                  4  /*                          */
#define WM8962_DRC_QR_THR_MASK                  0x0C00  /*                      */
#define WM8962_DRC_QR_THR_SHIFT                     10  /*                      */
#define WM8962_DRC_QR_THR_WIDTH                      2  /*                      */
#define WM8962_DRC_QR_DCY_MASK                  0x0300  /*                    */
#define WM8962_DRC_QR_DCY_SHIFT                      8  /*                    */
#define WM8962_DRC_QR_DCY_WIDTH                      2  /*                    */
#define WM8962_DRC_NG_EXP_MASK                  0x00C0  /*                    */
#define WM8962_DRC_NG_EXP_SHIFT                      6  /*                    */
#define WM8962_DRC_NG_EXP_WIDTH                      2  /*                    */
#define WM8962_DRC_HI_COMP_MASK                 0x0038  /*                     */
#define WM8962_DRC_HI_COMP_SHIFT                     3  /*                     */
#define WM8962_DRC_HI_COMP_WIDTH                     3  /*                     */
#define WM8962_DRC_LO_COMP_MASK                 0x0007  /*                     */
#define WM8962_DRC_LO_COMP_SHIFT                     0  /*                     */
#define WM8962_DRC_LO_COMP_WIDTH                     3  /*                     */

/*
                       
 */
#define WM8962_DRC_KNEE_IP_MASK                 0x07E0  /*                      */
#define WM8962_DRC_KNEE_IP_SHIFT                     5  /*                      */
#define WM8962_DRC_KNEE_IP_WIDTH                     6  /*                      */
#define WM8962_DRC_KNEE_OP_MASK                 0x001F  /*                     */
#define WM8962_DRC_KNEE_OP_SHIFT                     0  /*                     */
#define WM8962_DRC_KNEE_OP_WIDTH                     5  /*                     */

/*
                       
 */
#define WM8962_DRC_KNEE2_IP_MASK                0x03E0  /*                      */
#define WM8962_DRC_KNEE2_IP_SHIFT                    5  /*                      */
#define WM8962_DRC_KNEE2_IP_WIDTH                    5  /*                      */
#define WM8962_DRC_KNEE2_OP_MASK                0x001F  /*                      */
#define WM8962_DRC_KNEE2_OP_SHIFT                    0  /*                      */
#define WM8962_DRC_KNEE2_OP_WIDTH                    5  /*                      */

/*
                           
 */
#define WM8962_TLB_ENA                          0x0002  /*         */
#define WM8962_TLB_ENA_MASK                     0x0002  /*         */
#define WM8962_TLB_ENA_SHIFT                         1  /*         */
#define WM8962_TLB_ENA_WIDTH                         1  /*         */
#define WM8962_TLB_MODE                         0x0001  /*          */
#define WM8962_TLB_MODE_MASK                    0x0001  /*          */
#define WM8962_TLB_MODE_SHIFT                        0  /*          */
#define WM8962_TLB_MODE_WIDTH                        1  /*          */

/*
                     
 */
#define WM8962_EQ_SHARED_COEFF                  0x0004  /*                 */
#define WM8962_EQ_SHARED_COEFF_MASK             0x0004  /*                 */
#define WM8962_EQ_SHARED_COEFF_SHIFT                 2  /*                 */
#define WM8962_EQ_SHARED_COEFF_WIDTH                 1  /*                 */
#define WM8962_EQ_SHARED_COEFF_SEL              0x0002  /*                     */
#define WM8962_EQ_SHARED_COEFF_SEL_MASK         0x0002  /*                     */
#define WM8962_EQ_SHARED_COEFF_SEL_SHIFT             1  /*                     */
#define WM8962_EQ_SHARED_COEFF_SEL_WIDTH             1  /*                     */
#define WM8962_EQ_ENA                           0x0001  /*        */
#define WM8962_EQ_ENA_MASK                      0x0001  /*        */
#define WM8962_EQ_ENA_SHIFT                          0  /*        */
#define WM8962_EQ_ENA_WIDTH                          1  /*        */

/*
                     
 */
#define WM8962_EQL_B1_GAIN_MASK                 0xF800  /*                       */
#define WM8962_EQL_B1_GAIN_SHIFT                    11  /*                       */
#define WM8962_EQL_B1_GAIN_WIDTH                     5  /*                       */
#define WM8962_EQL_B2_GAIN_MASK                 0x07C0  /*                      */
#define WM8962_EQL_B2_GAIN_SHIFT                     6  /*                      */
#define WM8962_EQL_B2_GAIN_WIDTH                     5  /*                      */
#define WM8962_EQL_B3_GAIN_MASK                 0x003E  /*                     */
#define WM8962_EQL_B3_GAIN_SHIFT                     1  /*                     */
#define WM8962_EQL_B3_GAIN_WIDTH                     5  /*                     */

/*
                     
 */
#define WM8962_EQL_B4_GAIN_MASK                 0xF800  /*                       */
#define WM8962_EQL_B4_GAIN_SHIFT                    11  /*                       */
#define WM8962_EQL_B4_GAIN_WIDTH                     5  /*                       */
#define WM8962_EQL_B5_GAIN_MASK                 0x07C0  /*                      */
#define WM8962_EQL_B5_GAIN_SHIFT                     6  /*                      */
#define WM8962_EQL_B5_GAIN_WIDTH                     5  /*                      */

/*
                     
 */
#define WM8962_EQL_B1_A_MASK                    0xFFFF  /*                   */
#define WM8962_EQL_B1_A_SHIFT                        0  /*                   */
#define WM8962_EQL_B1_A_WIDTH                       16  /*                   */

/*
                     
 */
#define WM8962_EQL_B1_B_MASK                    0xFFFF  /*                   */
#define WM8962_EQL_B1_B_SHIFT                        0  /*                   */
#define WM8962_EQL_B1_B_WIDTH                       16  /*                   */

/*
                     
 */
#define WM8962_EQL_B1_PG_MASK                   0xFFFF  /*                    */
#define WM8962_EQL_B1_PG_SHIFT                       0  /*                    */
#define WM8962_EQL_B1_PG_WIDTH                      16  /*                    */

/*
                     
 */
#define WM8962_EQL_B2_A_MASK                    0xFFFF  /*                   */
#define WM8962_EQL_B2_A_SHIFT                        0  /*                   */
#define WM8962_EQL_B2_A_WIDTH                       16  /*                   */

/*
                     
 */
#define WM8962_EQL_B2_B_MASK                    0xFFFF  /*                   */
#define WM8962_EQL_B2_B_SHIFT                        0  /*                   */
#define WM8962_EQL_B2_B_WIDTH                       16  /*                   */

/*
                     
 */
#define WM8962_EQL_B2_C_MASK                    0xFFFF  /*                   */
#define WM8962_EQL_B2_C_SHIFT                        0  /*                   */
#define WM8962_EQL_B2_C_WIDTH                       16  /*                   */

/*
                      
 */
#define WM8962_EQL_B2_PG_MASK                   0xFFFF  /*                    */
#define WM8962_EQL_B2_PG_SHIFT                       0  /*                    */
#define WM8962_EQL_B2_PG_WIDTH                      16  /*                    */

/*
                      
 */
#define WM8962_EQL_B3_A_MASK                    0xFFFF  /*                   */
#define WM8962_EQL_B3_A_SHIFT                        0  /*                   */
#define WM8962_EQL_B3_A_WIDTH                       16  /*                   */

/*
                      
 */
#define WM8962_EQL_B3_B_MASK                    0xFFFF  /*                   */
#define WM8962_EQL_B3_B_SHIFT                        0  /*                   */
#define WM8962_EQL_B3_B_WIDTH                       16  /*                   */

/*
                      
 */
#define WM8962_EQL_B3_C_MASK                    0xFFFF  /*                   */
#define WM8962_EQL_B3_C_SHIFT                        0  /*                   */
#define WM8962_EQL_B3_C_WIDTH                       16  /*                   */

/*
                      
 */
#define WM8962_EQL_B3_PG_MASK                   0xFFFF  /*                    */
#define WM8962_EQL_B3_PG_SHIFT                       0  /*                    */
#define WM8962_EQL_B3_PG_WIDTH                      16  /*                    */

/*
                      
 */
#define WM8962_EQL_B4_A_MASK                    0xFFFF  /*                   */
#define WM8962_EQL_B4_A_SHIFT                        0  /*                   */
#define WM8962_EQL_B4_A_WIDTH                       16  /*                   */

/*
                      
 */
#define WM8962_EQL_B4_B_MASK                    0xFFFF  /*                   */
#define WM8962_EQL_B4_B_SHIFT                        0  /*                   */
#define WM8962_EQL_B4_B_WIDTH                       16  /*                   */

/*
                      
 */
#define WM8962_EQL_B4_C_MASK                    0xFFFF  /*                   */
#define WM8962_EQL_B4_C_SHIFT                        0  /*                   */
#define WM8962_EQL_B4_C_WIDTH                       16  /*                   */

/*
                      
 */
#define WM8962_EQL_B4_PG_MASK                   0xFFFF  /*                    */
#define WM8962_EQL_B4_PG_SHIFT                       0  /*                    */
#define WM8962_EQL_B4_PG_WIDTH                      16  /*                    */

/*
                      
 */
#define WM8962_EQL_B5_A_MASK                    0xFFFF  /*                   */
#define WM8962_EQL_B5_A_SHIFT                        0  /*                   */
#define WM8962_EQL_B5_A_WIDTH                       16  /*                   */

/*
                      
 */
#define WM8962_EQL_B5_B_MASK                    0xFFFF  /*                   */
#define WM8962_EQL_B5_B_SHIFT                        0  /*                   */
#define WM8962_EQL_B5_B_WIDTH                       16  /*                   */

/*
                      
 */
#define WM8962_EQL_B5_PG_MASK                   0xFFFF  /*                    */
#define WM8962_EQL_B5_PG_SHIFT                       0  /*                    */
#define WM8962_EQL_B5_PG_WIDTH                      16  /*                    */

/*
                      
 */
#define WM8962_EQR_B1_GAIN_MASK                 0xF800  /*                       */
#define WM8962_EQR_B1_GAIN_SHIFT                    11  /*                       */
#define WM8962_EQR_B1_GAIN_WIDTH                     5  /*                       */
#define WM8962_EQR_B2_GAIN_MASK                 0x07C0  /*                      */
#define WM8962_EQR_B2_GAIN_SHIFT                     6  /*                      */
#define WM8962_EQR_B2_GAIN_WIDTH                     5  /*                      */
#define WM8962_EQR_B3_GAIN_MASK                 0x003E  /*                     */
#define WM8962_EQR_B3_GAIN_SHIFT                     1  /*                     */
#define WM8962_EQR_B3_GAIN_WIDTH                     5  /*                     */

/*
                      
 */
#define WM8962_EQR_B4_GAIN_MASK                 0xF800  /*                       */
#define WM8962_EQR_B4_GAIN_SHIFT                    11  /*                       */
#define WM8962_EQR_B4_GAIN_WIDTH                     5  /*                       */
#define WM8962_EQR_B5_GAIN_MASK                 0x07C0  /*                      */
#define WM8962_EQR_B5_GAIN_SHIFT                     6  /*                      */
#define WM8962_EQR_B5_GAIN_WIDTH                     5  /*                      */

/*
                      
 */
#define WM8962_EQR_B1_A_MASK                    0xFFFF  /*                   */
#define WM8962_EQR_B1_A_SHIFT                        0  /*                   */
#define WM8962_EQR_B1_A_WIDTH                       16  /*                   */

/*
                      
 */
#define WM8962_EQR_B1_B_MASK                    0xFFFF  /*                   */
#define WM8962_EQR_B1_B_SHIFT                        0  /*                   */
#define WM8962_EQR_B1_B_WIDTH                       16  /*                   */

/*
                      
 */
#define WM8962_EQR_B1_PG_MASK                   0xFFFF  /*                    */
#define WM8962_EQR_B1_PG_SHIFT                       0  /*                    */
#define WM8962_EQR_B1_PG_WIDTH                      16  /*                    */

/*
                      
 */
#define WM8962_EQR_B2_A_MASK                    0xFFFF  /*                   */
#define WM8962_EQR_B2_A_SHIFT                        0  /*                   */
#define WM8962_EQR_B2_A_WIDTH                       16  /*                   */

/*
                      
 */
#define WM8962_EQR_B2_B_MASK                    0xFFFF  /*                   */
#define WM8962_EQR_B2_B_SHIFT                        0  /*                   */
#define WM8962_EQR_B2_B_WIDTH                       16  /*                   */

/*
                      
 */
#define WM8962_EQR_B2_C_MASK                    0xFFFF  /*                   */
#define WM8962_EQR_B2_C_SHIFT                        0  /*                   */
#define WM8962_EQR_B2_C_WIDTH                       16  /*                   */

/*
                      
 */
#define WM8962_EQR_B2_PG_MASK                   0xFFFF  /*                    */
#define WM8962_EQR_B2_PG_SHIFT                       0  /*                    */
#define WM8962_EQR_B2_PG_WIDTH                      16  /*                    */

/*
                      
 */
#define WM8962_EQR_B3_A_MASK                    0xFFFF  /*                   */
#define WM8962_EQR_B3_A_SHIFT                        0  /*                   */
#define WM8962_EQR_B3_A_WIDTH                       16  /*                   */

/*
                      
 */
#define WM8962_EQR_B3_B_MASK                    0xFFFF  /*                   */
#define WM8962_EQR_B3_B_SHIFT                        0  /*                   */
#define WM8962_EQR_B3_B_WIDTH                       16  /*                   */

/*
                      
 */
#define WM8962_EQR_B3_C_MASK                    0xFFFF  /*                   */
#define WM8962_EQR_B3_C_SHIFT                        0  /*                   */
#define WM8962_EQR_B3_C_WIDTH                       16  /*                   */

/*
                      
 */
#define WM8962_EQR_B3_PG_MASK                   0xFFFF  /*                    */
#define WM8962_EQR_B3_PG_SHIFT                       0  /*                    */
#define WM8962_EQR_B3_PG_WIDTH                      16  /*                    */

/*
                      
 */
#define WM8962_EQR_B4_A_MASK                    0xFFFF  /*                   */
#define WM8962_EQR_B4_A_SHIFT                        0  /*                   */
#define WM8962_EQR_B4_A_WIDTH                       16  /*                   */

/*
                      
 */
#define WM8962_EQR_B4_B_MASK                    0xFFFF  /*                   */
#define WM8962_EQR_B4_B_SHIFT                        0  /*                   */
#define WM8962_EQR_B4_B_WIDTH                       16  /*                   */

/*
                      
 */
#define WM8962_EQR_B4_C_MASK                    0xFFFF  /*                   */
#define WM8962_EQR_B4_C_SHIFT                        0  /*                   */
#define WM8962_EQR_B4_C_WIDTH                       16  /*                   */

/*
                      
 */
#define WM8962_EQR_B4_PG_MASK                   0xFFFF  /*                    */
#define WM8962_EQR_B4_PG_SHIFT                       0  /*                    */
#define WM8962_EQR_B4_PG_WIDTH                      16  /*                    */

/*
                      
 */
#define WM8962_EQR_B5_A_MASK                    0xFFFF  /*                   */
#define WM8962_EQR_B5_A_SHIFT                        0  /*                   */
#define WM8962_EQR_B5_A_WIDTH                       16  /*                   */

/*
                      
 */
#define WM8962_EQR_B5_B_MASK                    0xFFFF  /*                   */
#define WM8962_EQR_B5_B_SHIFT                        0  /*                   */
#define WM8962_EQR_B5_B_WIDTH                       16  /*                   */

/*
                      
 */
#define WM8962_EQR_B5_PG_MASK                   0xFFFF  /*                    */
#define WM8962_EQR_B5_PG_SHIFT                       0  /*                    */
#define WM8962_EQR_B5_PG_WIDTH                      16  /*                    */

/*
                        
 */
#define WM8962_GP2_POL                          0x0400  /*         */
#define WM8962_GP2_POL_MASK                     0x0400  /*         */
#define WM8962_GP2_POL_SHIFT                        10  /*         */
#define WM8962_GP2_POL_WIDTH                         1  /*         */
#define WM8962_GP2_LVL                          0x0040  /*         */
#define WM8962_GP2_LVL_MASK                     0x0040  /*         */
#define WM8962_GP2_LVL_SHIFT                         6  /*         */
#define WM8962_GP2_LVL_WIDTH                         1  /*         */
#define WM8962_GP2_FN_MASK                      0x001F  /*                */
#define WM8962_GP2_FN_SHIFT                          0  /*                */
#define WM8962_GP2_FN_WIDTH                          5  /*                */

/*
                        
 */
#define WM8962_GP3_POL                          0x0400  /*         */
#define WM8962_GP3_POL_MASK                     0x0400  /*         */
#define WM8962_GP3_POL_SHIFT                        10  /*         */
#define WM8962_GP3_POL_WIDTH                         1  /*         */
#define WM8962_GP3_LVL                          0x0040  /*         */
#define WM8962_GP3_LVL_MASK                     0x0040  /*         */
#define WM8962_GP3_LVL_SHIFT                         6  /*         */
#define WM8962_GP3_LVL_WIDTH                         1  /*         */
#define WM8962_GP3_FN_MASK                      0x001F  /*                */
#define WM8962_GP3_FN_SHIFT                          0  /*                */
#define WM8962_GP3_FN_WIDTH                          5  /*                */

/*
                        
 */
#define WM8962_GP5_DIR                          0x8000  /*         */
#define WM8962_GP5_DIR_MASK                     0x8000  /*         */
#define WM8962_GP5_DIR_SHIFT                        15  /*         */
#define WM8962_GP5_DIR_WIDTH                         1  /*         */
#define WM8962_GP5_PU                           0x4000  /*        */
#define WM8962_GP5_PU_MASK                      0x4000  /*        */
#define WM8962_GP5_PU_SHIFT                         14  /*        */
#define WM8962_GP5_PU_WIDTH                          1  /*        */
#define WM8962_GP5_PD                           0x2000  /*        */
#define WM8962_GP5_PD_MASK                      0x2000  /*        */
#define WM8962_GP5_PD_SHIFT                         13  /*        */
#define WM8962_GP5_PD_WIDTH                          1  /*        */
#define WM8962_GP5_POL                          0x0400  /*         */
#define WM8962_GP5_POL_MASK                     0x0400  /*         */
#define WM8962_GP5_POL_SHIFT                        10  /*         */
#define WM8962_GP5_POL_WIDTH                         1  /*         */
#define WM8962_GP5_OP_CFG                       0x0200  /*            */
#define WM8962_GP5_OP_CFG_MASK                  0x0200  /*            */
#define WM8962_GP5_OP_CFG_SHIFT                      9  /*            */
#define WM8962_GP5_OP_CFG_WIDTH                      1  /*            */
#define WM8962_GP5_DB                           0x0100  /*        */
#define WM8962_GP5_DB_MASK                      0x0100  /*        */
#define WM8962_GP5_DB_SHIFT                          8  /*        */
#define WM8962_GP5_DB_WIDTH                          1  /*        */
#define WM8962_GP5_LVL                          0x0040  /*         */
#define WM8962_GP5_LVL_MASK                     0x0040  /*         */
#define WM8962_GP5_LVL_SHIFT                         6  /*         */
#define WM8962_GP5_LVL_WIDTH                         1  /*         */
#define WM8962_GP5_FN_MASK                      0x001F  /*                */
#define WM8962_GP5_FN_SHIFT                          0  /*                */
#define WM8962_GP5_FN_WIDTH                          5  /*                */

/*
                        
 */
#define WM8962_GP6_DIR                          0x8000  /*         */
#define WM8962_GP6_DIR_MASK                     0x8000  /*         */
#define WM8962_GP6_DIR_SHIFT                        15  /*         */
#define WM8962_GP6_DIR_WIDTH                         1  /*         */
#define WM8962_GP6_PU                           0x4000  /*        */
#define WM8962_GP6_PU_MASK                      0x4000  /*        */
#define WM8962_GP6_PU_SHIFT                         14  /*        */
#define WM8962_GP6_PU_WIDTH                          1  /*        */
#define WM8962_GP6_PD                           0x2000  /*        */
#define WM8962_GP6_PD_MASK                      0x2000  /*        */
#define WM8962_GP6_PD_SHIFT                         13  /*        */
#define WM8962_GP6_PD_WIDTH                          1  /*        */
#define WM8962_GP6_POL                          0x0400  /*         */
#define WM8962_GP6_POL_MASK                     0x0400  /*         */
#define WM8962_GP6_POL_SHIFT                        10  /*         */
#define WM8962_GP6_POL_WIDTH                         1  /*         */
#define WM8962_GP6_OP_CFG                       0x0200  /*            */
#define WM8962_GP6_OP_CFG_MASK                  0x0200  /*            */
#define WM8962_GP6_OP_CFG_SHIFT                      9  /*            */
#define WM8962_GP6_OP_CFG_WIDTH                      1  /*            */
#define WM8962_GP6_DB                           0x0100  /*        */
#define WM8962_GP6_DB_MASK                      0x0100  /*        */
#define WM8962_GP6_DB_SHIFT                          8  /*        */
#define WM8962_GP6_DB_WIDTH                          1  /*        */
#define WM8962_GP6_LVL                          0x0040  /*         */
#define WM8962_GP6_LVL_MASK                     0x0040  /*         */
#define WM8962_GP6_LVL_SHIFT                         6  /*         */
#define WM8962_GP6_LVL_WIDTH                         1  /*         */
#define WM8962_GP6_FN_MASK                      0x001F  /*                */
#define WM8962_GP6_FN_SHIFT                          0  /*                */
#define WM8962_GP6_FN_WIDTH                          5  /*                */

/*
                                    
 */
#define WM8962_GP6_EINT                         0x0020  /*          */
#define WM8962_GP6_EINT_MASK                    0x0020  /*          */
#define WM8962_GP6_EINT_SHIFT                        5  /*          */
#define WM8962_GP6_EINT_WIDTH                        1  /*          */
#define WM8962_GP5_EINT                         0x0010  /*          */
#define WM8962_GP5_EINT_MASK                    0x0010  /*          */
#define WM8962_GP5_EINT_SHIFT                        4  /*          */
#define WM8962_GP5_EINT_WIDTH                        1  /*          */

/*
                                    
 */
#define WM8962_MICSCD_EINT                      0x8000  /*             */
#define WM8962_MICSCD_EINT_MASK                 0x8000  /*             */
#define WM8962_MICSCD_EINT_SHIFT                    15  /*             */
#define WM8962_MICSCD_EINT_WIDTH                     1  /*             */
#define WM8962_MICD_EINT                        0x4000  /*           */
#define WM8962_MICD_EINT_MASK                   0x4000  /*           */
#define WM8962_MICD_EINT_SHIFT                      14  /*           */
#define WM8962_MICD_EINT_WIDTH                       1  /*           */
#define WM8962_FIFOS_ERR_EINT                   0x2000  /*                */
#define WM8962_FIFOS_ERR_EINT_MASK              0x2000  /*                */
#define WM8962_FIFOS_ERR_EINT_SHIFT                 13  /*                */
#define WM8962_FIFOS_ERR_EINT_WIDTH                  1  /*                */
#define WM8962_ALC_LOCK_EINT                    0x1000  /*               */
#define WM8962_ALC_LOCK_EINT_MASK               0x1000  /*               */
#define WM8962_ALC_LOCK_EINT_SHIFT                  12  /*               */
#define WM8962_ALC_LOCK_EINT_WIDTH                   1  /*               */
#define WM8962_ALC_THRESH_EINT                  0x0800  /*                 */
#define WM8962_ALC_THRESH_EINT_MASK             0x0800  /*                 */
#define WM8962_ALC_THRESH_EINT_SHIFT                11  /*                 */
#define WM8962_ALC_THRESH_EINT_WIDTH                 1  /*                 */
#define WM8962_ALC_SAT_EINT                     0x0400  /*              */
#define WM8962_ALC_SAT_EINT_MASK                0x0400  /*              */
#define WM8962_ALC_SAT_EINT_SHIFT                   10  /*              */
#define WM8962_ALC_SAT_EINT_WIDTH                    1  /*              */
#define WM8962_ALC_PKOVR_EINT                   0x0200  /*                */
#define WM8962_ALC_PKOVR_EINT_MASK              0x0200  /*                */
#define WM8962_ALC_PKOVR_EINT_SHIFT                  9  /*                */
#define WM8962_ALC_PKOVR_EINT_WIDTH                  1  /*                */
#define WM8962_ALC_NGATE_EINT                   0x0100  /*                */
#define WM8962_ALC_NGATE_EINT_MASK              0x0100  /*                */
#define WM8962_ALC_NGATE_EINT_SHIFT                  8  /*                */
#define WM8962_ALC_NGATE_EINT_WIDTH                  1  /*                */
#define WM8962_WSEQ_DONE_EINT                   0x0080  /*                */
#define WM8962_WSEQ_DONE_EINT_MASK              0x0080  /*                */
#define WM8962_WSEQ_DONE_EINT_SHIFT                  7  /*                */
#define WM8962_WSEQ_DONE_EINT_WIDTH                  1  /*                */
#define WM8962_DRC_ACTDET_EINT                  0x0040  /*                 */
#define WM8962_DRC_ACTDET_EINT_MASK             0x0040  /*                 */
#define WM8962_DRC_ACTDET_EINT_SHIFT                 6  /*                 */
#define WM8962_DRC_ACTDET_EINT_WIDTH                 1  /*                 */
#define WM8962_FLL_LOCK_EINT                    0x0020  /*               */
#define WM8962_FLL_LOCK_EINT_MASK               0x0020  /*               */
#define WM8962_FLL_LOCK_EINT_SHIFT                   5  /*               */
#define WM8962_FLL_LOCK_EINT_WIDTH                   1  /*               */
#define WM8962_PLL3_LOCK_EINT                   0x0008  /*                */
#define WM8962_PLL3_LOCK_EINT_MASK              0x0008  /*                */
#define WM8962_PLL3_LOCK_EINT_SHIFT                  3  /*                */
#define WM8962_PLL3_LOCK_EINT_WIDTH                  1  /*                */
#define WM8962_PLL2_LOCK_EINT                   0x0004  /*                */
#define WM8962_PLL2_LOCK_EINT_MASK              0x0004  /*                */
#define WM8962_PLL2_LOCK_EINT_SHIFT                  2  /*                */
#define WM8962_PLL2_LOCK_EINT_WIDTH                  1  /*                */
#define WM8962_TEMP_SHUT_EINT                   0x0001  /*                */
#define WM8962_TEMP_SHUT_EINT_MASK              0x0001  /*                */
#define WM8962_TEMP_SHUT_EINT_SHIFT                  0  /*                */
#define WM8962_TEMP_SHUT_EINT_WIDTH                  1  /*                */

/*
                                         
 */
#define WM8962_IM_GP6_EINT                      0x0020  /*             */
#define WM8962_IM_GP6_EINT_MASK                 0x0020  /*             */
#define WM8962_IM_GP6_EINT_SHIFT                     5  /*             */
#define WM8962_IM_GP6_EINT_WIDTH                     1  /*             */
#define WM8962_IM_GP5_EINT                      0x0010  /*             */
#define WM8962_IM_GP5_EINT_MASK                 0x0010  /*             */
#define WM8962_IM_GP5_EINT_SHIFT                     4  /*             */
#define WM8962_IM_GP5_EINT_WIDTH                     1  /*             */

/*
                                         
 */
#define WM8962_IM_MICSCD_EINT                   0x8000  /*                */
#define WM8962_IM_MICSCD_EINT_MASK              0x8000  /*                */
#define WM8962_IM_MICSCD_EINT_SHIFT                 15  /*                */
#define WM8962_IM_MICSCD_EINT_WIDTH                  1  /*                */
#define WM8962_IM_MICD_EINT                     0x4000  /*              */
#define WM8962_IM_MICD_EINT_MASK                0x4000  /*              */
#define WM8962_IM_MICD_EINT_SHIFT                   14  /*              */
#define WM8962_IM_MICD_EINT_WIDTH                    1  /*              */
#define WM8962_IM_FIFOS_ERR_EINT                0x2000  /*                   */
#define WM8962_IM_FIFOS_ERR_EINT_MASK           0x2000  /*                   */
#define WM8962_IM_FIFOS_ERR_EINT_SHIFT              13  /*                   */
#define WM8962_IM_FIFOS_ERR_EINT_WIDTH               1  /*                   */
#define WM8962_IM_ALC_LOCK_EINT                 0x1000  /*                  */
#define WM8962_IM_ALC_LOCK_EINT_MASK            0x1000  /*                  */
#define WM8962_IM_ALC_LOCK_EINT_SHIFT               12  /*                  */
#define WM8962_IM_ALC_LOCK_EINT_WIDTH                1  /*                  */
#define WM8962_IM_ALC_THRESH_EINT               0x0800  /*                    */
#define WM8962_IM_ALC_THRESH_EINT_MASK          0x0800  /*                    */
#define WM8962_IM_ALC_THRESH_EINT_SHIFT             11  /*                    */
#define WM8962_IM_ALC_THRESH_EINT_WIDTH              1  /*                    */
#define WM8962_IM_ALC_SAT_EINT                  0x0400  /*                 */
#define WM8962_IM_ALC_SAT_EINT_MASK             0x0400  /*                 */
#define WM8962_IM_ALC_SAT_EINT_SHIFT                10  /*                 */
#define WM8962_IM_ALC_SAT_EINT_WIDTH                 1  /*                 */
#define WM8962_IM_ALC_PKOVR_EINT                0x0200  /*                   */
#define WM8962_IM_ALC_PKOVR_EINT_MASK           0x0200  /*                   */
#define WM8962_IM_ALC_PKOVR_EINT_SHIFT               9  /*                   */
#define WM8962_IM_ALC_PKOVR_EINT_WIDTH               1  /*                   */
#define WM8962_IM_ALC_NGATE_EINT                0x0100  /*                   */
#define WM8962_IM_ALC_NGATE_EINT_MASK           0x0100  /*                   */
#define WM8962_IM_ALC_NGATE_EINT_SHIFT               8  /*                   */
#define WM8962_IM_ALC_NGATE_EINT_WIDTH               1  /*                   */
#define WM8962_IM_WSEQ_DONE_EINT                0x0080  /*                   */
#define WM8962_IM_WSEQ_DONE_EINT_MASK           0x0080  /*                   */
#define WM8962_IM_WSEQ_DONE_EINT_SHIFT               7  /*                   */
#define WM8962_IM_WSEQ_DONE_EINT_WIDTH               1  /*                   */
#define WM8962_IM_DRC_ACTDET_EINT               0x0040  /*                    */
#define WM8962_IM_DRC_ACTDET_EINT_MASK          0x0040  /*                    */
#define WM8962_IM_DRC_ACTDET_EINT_SHIFT              6  /*                    */
#define WM8962_IM_DRC_ACTDET_EINT_WIDTH              1  /*                    */
#define WM8962_IM_FLL_LOCK_EINT                 0x0020  /*                  */
#define WM8962_IM_FLL_LOCK_EINT_MASK            0x0020  /*                  */
#define WM8962_IM_FLL_LOCK_EINT_SHIFT                5  /*                  */
#define WM8962_IM_FLL_LOCK_EINT_WIDTH                1  /*                  */
#define WM8962_IM_PLL3_LOCK_EINT                0x0008  /*                   */
#define WM8962_IM_PLL3_LOCK_EINT_MASK           0x0008  /*                   */
#define WM8962_IM_PLL3_LOCK_EINT_SHIFT               3  /*                   */
#define WM8962_IM_PLL3_LOCK_EINT_WIDTH               1  /*                   */
#define WM8962_IM_PLL2_LOCK_EINT                0x0004  /*                   */
#define WM8962_IM_PLL2_LOCK_EINT_MASK           0x0004  /*                   */
#define WM8962_IM_PLL2_LOCK_EINT_SHIFT               2  /*                   */
#define WM8962_IM_PLL2_LOCK_EINT_WIDTH               1  /*                   */
#define WM8962_IM_TEMP_SHUT_EINT                0x0001  /*                   */
#define WM8962_IM_TEMP_SHUT_EINT_MASK           0x0001  /*                   */
#define WM8962_IM_TEMP_SHUT_EINT_SHIFT               0  /*                   */
#define WM8962_IM_TEMP_SHUT_EINT_WIDTH               1  /*                   */

/*
                                   
 */
#define WM8962_IRQ_POL                          0x0001  /*         */
#define WM8962_IRQ_POL_MASK                     0x0001  /*         */
#define WM8962_IRQ_POL_SHIFT                         0  /*         */
#define WM8962_IRQ_POL_WIDTH                         1  /*         */

/*
                              
 */
#define WM8962_FLL_LOCK_DB                      0x0020  /*             */
#define WM8962_FLL_LOCK_DB_MASK                 0x0020  /*             */
#define WM8962_FLL_LOCK_DB_SHIFT                     5  /*             */
#define WM8962_FLL_LOCK_DB_WIDTH                     1  /*             */
#define WM8962_PLL3_LOCK_DB                     0x0008  /*              */
#define WM8962_PLL3_LOCK_DB_MASK                0x0008  /*              */
#define WM8962_PLL3_LOCK_DB_SHIFT                    3  /*              */
#define WM8962_PLL3_LOCK_DB_WIDTH                    1  /*              */
#define WM8962_PLL2_LOCK_DB                     0x0004  /*              */
#define WM8962_PLL2_LOCK_DB_MASK                0x0004  /*              */
#define WM8962_PLL2_LOCK_DB_SHIFT                    2  /*              */
#define WM8962_PLL2_LOCK_DB_WIDTH                    1  /*              */
#define WM8962_TEMP_SHUT_DB                     0x0001  /*              */
#define WM8962_TEMP_SHUT_DB_MASK                0x0001  /*              */
#define WM8962_TEMP_SHUT_DB_SHIFT                    0  /*              */
#define WM8962_TEMP_SHUT_DB_WIDTH                    1  /*              */

/*
                                    
 */
#define WM8962_MICSCD_IRQ_POL                   0x8000  /*                */
#define WM8962_MICSCD_IRQ_POL_MASK              0x8000  /*                */
#define WM8962_MICSCD_IRQ_POL_SHIFT                 15  /*                */
#define WM8962_MICSCD_IRQ_POL_WIDTH                  1  /*                */
#define WM8962_MICD_IRQ_POL                     0x4000  /*              */
#define WM8962_MICD_IRQ_POL_MASK                0x4000  /*              */
#define WM8962_MICD_IRQ_POL_SHIFT                   14  /*              */
#define WM8962_MICD_IRQ_POL_WIDTH                    1  /*              */

/*
                                       
 */
#define WM8962_DSP2_ENA                         0x0001  /*          */
#define WM8962_DSP2_ENA_MASK                    0x0001  /*          */
#define WM8962_DSP2_ENA_SHIFT                        0  /*          */
#define WM8962_DSP2_ENA_WIDTH                        1  /*          */

/*
                                   
 */
#define WM8962_DSP2_STOPC                       0x0020  /*            */
#define WM8962_DSP2_STOPC_MASK                  0x0020  /*            */
#define WM8962_DSP2_STOPC_SHIFT                      5  /*            */
#define WM8962_DSP2_STOPC_WIDTH                      1  /*            */
#define WM8962_DSP2_STOPS                       0x0010  /*            */
#define WM8962_DSP2_STOPS_MASK                  0x0010  /*            */
#define WM8962_DSP2_STOPS_SHIFT                      4  /*            */
#define WM8962_DSP2_STOPS_WIDTH                      1  /*            */
#define WM8962_DSP2_STOPI                       0x0008  /*            */
#define WM8962_DSP2_STOPI_MASK                  0x0008  /*            */
#define WM8962_DSP2_STOPI_SHIFT                      3  /*            */
#define WM8962_DSP2_STOPI_WIDTH                      1  /*            */
#define WM8962_DSP2_STOP                        0x0004  /*           */
#define WM8962_DSP2_STOP_MASK                   0x0004  /*           */
#define WM8962_DSP2_STOP_SHIFT                       2  /*           */
#define WM8962_DSP2_STOP_WIDTH                       1  /*           */
#define WM8962_DSP2_RUNR                        0x0002  /*           */
#define WM8962_DSP2_RUNR_MASK                   0x0002  /*           */
#define WM8962_DSP2_RUNR_SHIFT                       1  /*           */
#define WM8962_DSP2_RUNR_WIDTH                       1  /*           */
#define WM8962_DSP2_RUN                         0x0001  /*          */
#define WM8962_DSP2_RUN_MASK                    0x0001  /*          */
#define WM8962_DSP2_RUN_SHIFT                        0  /*          */
#define WM8962_DSP2_RUN_WIDTH                        1  /*          */

/*
                                          
 */
#define WM8962_DSP2_INSTR_RAM_1024_10_9_0_MASK  0x03FF  /*                                    */
#define WM8962_DSP2_INSTR_RAM_1024_10_9_0_SHIFT      0  /*                                    */
#define WM8962_DSP2_INSTR_RAM_1024_10_9_0_WIDTH     10  /*                                    */

/*
                                      
 */
#define WM8962_DSP2_ADDR_RAM_1024_38_37_32_MASK 0x003F  /*                                     */
#define WM8962_DSP2_ADDR_RAM_1024_38_37_32_SHIFT      0  /*                                     */
#define WM8962_DSP2_ADDR_RAM_1024_38_37_32_WIDTH      6  /*                                     */

/*
                                      
 */
#define WM8962_DSP2_ADDR_RAM_1024_38_31_16_MASK 0xFFFF  /*                                      */
#define WM8962_DSP2_ADDR_RAM_1024_38_31_16_SHIFT      0  /*                                      */
#define WM8962_DSP2_ADDR_RAM_1024_38_31_16_WIDTH     16  /*                                      */

/*
                                      
 */
#define WM8962_DSP2_ADDR_RAM_1024_38_15_0_MASK  0xFFFF  /*                                     */
#define WM8962_DSP2_ADDR_RAM_1024_38_15_0_SHIFT      0  /*                                     */
#define WM8962_DSP2_ADDR_RAM_1024_38_15_0_WIDTH     16  /*                                     */

/*
                                     
 */
#define WM8962_DSP2_DATA1_RAM_384_24_23_16_MASK 0x00FF  /*                                     */
#define WM8962_DSP2_DATA1_RAM_384_24_23_16_SHIFT      0  /*                                     */
#define WM8962_DSP2_DATA1_RAM_384_24_23_16_WIDTH      8  /*                                     */

/*
                                     
 */
#define WM8962_DSP2_DATA1_RAM_384_24_15_0_MASK  0xFFFF  /*                                     */
#define WM8962_DSP2_DATA1_RAM_384_24_15_0_SHIFT      0  /*                                     */
#define WM8962_DSP2_DATA1_RAM_384_24_15_0_WIDTH     16  /*                                     */

/*
                                     
 */
#define WM8962_DSP2_DATA2_RAM_384_24_23_16_MASK 0x00FF  /*                                     */
#define WM8962_DSP2_DATA2_RAM_384_24_23_16_SHIFT      0  /*                                     */
#define WM8962_DSP2_DATA2_RAM_384_24_23_16_WIDTH      8  /*                                     */

/*
                                     
 */
#define WM8962_DSP2_DATA2_RAM_384_24_15_0_MASK  0xFFFF  /*                                     */
#define WM8962_DSP2_DATA2_RAM_384_24_15_0_SHIFT      0  /*                                     */
#define WM8962_DSP2_DATA2_RAM_384_24_15_0_WIDTH     16  /*                                     */

/*
                                     
 */
#define WM8962_DSP2_DATA3_RAM_384_24_23_16_MASK 0x00FF  /*                                     */
#define WM8962_DSP2_DATA3_RAM_384_24_23_16_SHIFT      0  /*                                     */
#define WM8962_DSP2_DATA3_RAM_384_24_23_16_WIDTH      8  /*                                     */

/*
                                     
 */
#define WM8962_DSP2_DATA3_RAM_384_24_15_0_MASK  0xFFFF  /*                                     */
#define WM8962_DSP2_DATA3_RAM_384_24_15_0_SHIFT      0  /*                                     */
#define WM8962_DSP2_DATA3_RAM_384_24_15_0_WIDTH     16  /*                                     */

/*
                                     
 */
#define WM8962_DSP2_CMAP_RAM_384_11_10_0_MASK   0x07FF  /*                                    */
#define WM8962_DSP2_CMAP_RAM_384_11_10_0_SHIFT       0  /*                                    */
#define WM8962_DSP2_CMAP_RAM_384_11_10_0_WIDTH      11  /*                                    */

/*
                                             
 */
#define WM8962_ADC_RETUNE_SCV                   0x0080  /*                */
#define WM8962_ADC_RETUNE_SCV_MASK              0x0080  /*                */
#define WM8962_ADC_RETUNE_SCV_SHIFT                  7  /*                */
#define WM8962_ADC_RETUNE_SCV_WIDTH                  1  /*                */
#define WM8962_RETUNEADC_SHARED_COEFF_22_16_MASK 0x007F  /*                                      */
#define WM8962_RETUNEADC_SHARED_COEFF_22_16_SHIFT      0  /*                                      */
#define WM8962_RETUNEADC_SHARED_COEFF_22_16_WIDTH      7  /*                                      */

/*
                                             
 */
#define WM8962_RETUNEADC_SHARED_COEFF_15_00_MASK 0xFFFF  /*                                       */
#define WM8962_RETUNEADC_SHARED_COEFF_15_00_SHIFT      0  /*                                       */
#define WM8962_RETUNEADC_SHARED_COEFF_15_00_WIDTH     16  /*                                       */

/*
                                             
 */
#define WM8962_DAC_RETUNE_SCV                   0x0080  /*                */
#define WM8962_DAC_RETUNE_SCV_MASK              0x0080  /*                */
#define WM8962_DAC_RETUNE_SCV_SHIFT                  7  /*                */
#define WM8962_DAC_RETUNE_SCV_WIDTH                  1  /*                */
#define WM8962_RETUNEDAC_SHARED_COEFF_23_16_MASK 0x007F  /*                                      */
#define WM8962_RETUNEDAC_SHARED_COEFF_23_16_SHIFT      0  /*                                      */
#define WM8962_RETUNEDAC_SHARED_COEFF_23_16_WIDTH      7  /*                                      */

/*
                                             
 */
#define WM8962_RETUNEDAC_SHARED_COEFF_15_00_MASK 0xFFFF  /*                                       */
#define WM8962_RETUNEDAC_SHARED_COEFF_15_00_SHIFT      0  /*                                       */
#define WM8962_RETUNEDAC_SHARED_COEFF_15_00_WIDTH     16  /*                                       */

/*
                                         
 */
#define WM8962_SOUNDSTAGE_ENABLES_23_16_MASK    0x00FF  /*                                  */
#define WM8962_SOUNDSTAGE_ENABLES_23_16_SHIFT        0  /*                                  */
#define WM8962_SOUNDSTAGE_ENABLES_23_16_WIDTH        8  /*                                  */

/*
                                         
 */
#define WM8962_SOUNDSTAGE_ENABLES_15_06_MASK    0xFFC0  /*                                   */
#define WM8962_SOUNDSTAGE_ENABLES_15_06_SHIFT        6  /*                                   */
#define WM8962_SOUNDSTAGE_ENABLES_15_06_WIDTH       10  /*                                   */
#define WM8962_RTN_ADC_ENA                      0x0020  /*             */
#define WM8962_RTN_ADC_ENA_MASK                 0x0020  /*             */
#define WM8962_RTN_ADC_ENA_SHIFT                     5  /*             */
#define WM8962_RTN_ADC_ENA_WIDTH                     1  /*             */
#define WM8962_RTN_DAC_ENA                      0x0010  /*             */
#define WM8962_RTN_DAC_ENA_MASK                 0x0010  /*             */
#define WM8962_RTN_DAC_ENA_SHIFT                     4  /*             */
#define WM8962_RTN_DAC_ENA_WIDTH                     1  /*             */
#define WM8962_HDBASS_ENA                       0x0008  /*            */
#define WM8962_HDBASS_ENA_MASK                  0x0008  /*            */
#define WM8962_HDBASS_ENA_SHIFT                      3  /*            */
#define WM8962_HDBASS_ENA_WIDTH                      1  /*            */
#define WM8962_HPF2_ENA                         0x0004  /*          */
#define WM8962_HPF2_ENA_MASK                    0x0004  /*          */
#define WM8962_HPF2_ENA_SHIFT                        2  /*          */
#define WM8962_HPF2_ENA_WIDTH                        1  /*          */
#define WM8962_HPF1_ENA                         0x0002  /*          */
#define WM8962_HPF1_ENA_MASK                    0x0002  /*          */
#define WM8962_HPF1_ENA_SHIFT                        1  /*          */
#define WM8962_HPF1_ENA_WIDTH                        1  /*          */
#define WM8962_VSS_ENA                          0x0001  /*         */
#define WM8962_VSS_ENA_MASK                     0x0001  /*         */
#define WM8962_VSS_ENA_SHIFT                         0  /*         */
#define WM8962_VSS_ENA_WIDTH                         1  /*         */

int wm8962_mic_detect(struct snd_soc_codec *codec, struct snd_soc_jack *jack);

#endif
