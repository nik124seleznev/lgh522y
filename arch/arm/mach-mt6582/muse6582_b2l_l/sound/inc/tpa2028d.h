/* include/sound/tpa2028d.h
 *
 * Copyright (C) 2009 LGE, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

/*
                                                     
                                                                                                    
                                                                        
  
 */

/*                    */
#define IC_CONTROL				0x01
#define AGC_ATTACK_CONTROL		0x02
#define AGC_RELEASE_CONTROL		0x03
#define AGC_HOLD_TIME_CONTROL	0x04
#define AGC_FIXED_GAIN_CONTROL	0x05
#define AGC1_CONTROL			0x06
#define AGC2_CONTROL			0x07

/*              */
#define SPK_ON 1
#define SPK_OFF 0

bool Speaker_Register_tpa2028d(void);
int Audio_eamp_command_tpa2028d(unsigned int type, unsigned long args, unsigned int count);
bool Speaker_Init_tpa2028d(void);


