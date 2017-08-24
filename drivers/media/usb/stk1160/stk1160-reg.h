/*
 * STK1160 driver
 *
 * Copyright (C) 2012 Ezequiel Garcia
 * <elezegarcia--a.t--gmail.com>
 *
 * Based on Easycap driver by R.M. Thomas
 *	Copyright (C) 2010 R.M. Thomas
 *	<rmthomas--a.t--sciolus.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

/*              */
#define STK1160_GCTRL			0x000

/*                      */
#define STK1160_RMCTL			0x00c

/*
                            
                                        
                                          
 */
#define STK1160_DCTRL			0x100

/*                              */
#define STK116_CFSPO			0x110
#define STK116_CFSPO_STX_L		0x110
#define STK116_CFSPO_STX_H		0x111
#define STK116_CFSPO_STY_L		0x112
#define STK116_CFSPO_STY_H		0x113

/*                            */
#define STK116_CFEPO			0x114
#define STK116_CFEPO_ENX_L		0x114
#define STK116_CFEPO_ENX_H		0x115
#define STK116_CFEPO_ENY_L		0x116
#define STK116_CFEPO_ENY_H		0x117

/*                           */
#define STK1160_SICTL			0x200
#define STK1160_SICTL_CD		0x202
#define STK1160_SICTL_SDA		0x203

/*                  */
#define STK1160_SBUSW			0x204
#define STK1160_SBUSW_WA		0x204
#define STK1160_SBUSW_WD		0x205

/*                 */
#define STK1160_SBUSR			0x208
#define STK1160_SBUSR_RA		0x208
#define STK1160_SBUSR_RD		0x209

/*                                   */
#define STK1160_ASIC			0x2fc

/*                    */
#define STK1160_PLLSO			0x018

/*                       */
#define STK1160_PLLFD			0x01c

/*                  */
#define STK1160_TIGEN			0x300

/*                          */
#define STK1160_TICTL			0x350

/*                    */
#define STK1160_AC97CTL_0		0x500
#define STK1160_AC97CTL_1		0x504

/*                                                               */
#define STK1160_AC97_ADDR		0x504
/*                                                              */
#define STK1160_AC97_CMD		0x502

/*                     */
#define STK1160_I2SCTL			0x50c

/*                  */
#define STK1160_EEPROM_SZ		0x5f0
