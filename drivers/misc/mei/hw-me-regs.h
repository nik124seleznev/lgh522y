/******************************************************************************
 * Intel Management Engine Interface (Intel MEI) Linux driver
 * Intel MEI Interface Header
 *
 * This file is provided under a dual BSD/GPLv2 license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * GPL LICENSE SUMMARY
 *
 * Copyright(c) 2003 - 2012 Intel Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110,
 * USA
 *
 * The full GNU General Public License is included in this distribution
 * in the file called LICENSE.GPL.
 *
 * Contact Information:
 *	Intel Corporation.
 *	linux-mei@linux.intel.com
 *	http://www.intel.com
 *
 * BSD LICENSE
 *
 * Copyright(c) 2003 - 2012 Intel Corporation. All rights reserved.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name Intel Corporation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *****************************************************************************/
#ifndef _MEI_HW_MEI_REGS_H_
#define _MEI_HW_MEI_REGS_H_

/*
                 
 */
#define MEI_DEV_ID_82946GZ    0x2974  /*            */
#define MEI_DEV_ID_82G35      0x2984  /*               */
#define MEI_DEV_ID_82Q965     0x2994  /*             */
#define MEI_DEV_ID_82G965     0x29A4  /*             */

#define MEI_DEV_ID_82GM965    0x2A04  /*                    */
#define MEI_DEV_ID_82GME965   0x2A14  /*                      */

#define MEI_DEV_ID_ICH9_82Q35 0x29B4  /*               */
#define MEI_DEV_ID_ICH9_82G33 0x29C4  /*                           */
#define MEI_DEV_ID_ICH9_82Q33 0x29D4  /*               */
#define MEI_DEV_ID_ICH9_82X38 0x29E4  /*                   */
#define MEI_DEV_ID_ICH9_3200  0x29F4  /*                  */

#define MEI_DEV_ID_ICH9_6     0x28B4  /*          */
#define MEI_DEV_ID_ICH9_7     0x28C4  /*          */
#define MEI_DEV_ID_ICH9_8     0x28D4  /*          */
#define MEI_DEV_ID_ICH9_9     0x28E4  /*          */
#define MEI_DEV_ID_ICH9_10    0x28F4  /*          */

#define MEI_DEV_ID_ICH9M_1    0x2A44  /*         */
#define MEI_DEV_ID_ICH9M_2    0x2A54  /*         */
#define MEI_DEV_ID_ICH9M_3    0x2A64  /*         */
#define MEI_DEV_ID_ICH9M_4    0x2A74  /*         */

#define MEI_DEV_ID_ICH10_1    0x2E04  /*           */
#define MEI_DEV_ID_ICH10_2    0x2E14  /*           */
#define MEI_DEV_ID_ICH10_3    0x2E24  /*           */
#define MEI_DEV_ID_ICH10_4    0x2E34  /*           */

#define MEI_DEV_ID_IBXPK_1    0x3B64  /*          */
#define MEI_DEV_ID_IBXPK_2    0x3B65  /*          */

#define MEI_DEV_ID_CPT_1      0x1C3A  /*              */
#define MEI_DEV_ID_PBG_1      0x1D3A  /*                   */

#define MEI_DEV_ID_PPT_1      0x1E3A  /*               */
#define MEI_DEV_ID_PPT_2      0x1CBA  /*               */
#define MEI_DEV_ID_PPT_3      0x1DBA  /*               */

#define MEI_DEV_ID_LPT_H      0x8C3A  /*              */
#define MEI_DEV_ID_LPT_W      0x8D3A  /*                        */
#define MEI_DEV_ID_LPT_LP     0x9C3A  /*               */
#define MEI_DEV_ID_LPT_HR     0x8CBA  /*                      */

#define MEI_DEV_ID_WPT_LP     0x9CBA  /*                  */

/*                                                    */
#define PCI_CFG_HFS_1         0x40
#define PCI_CFG_HFS_2         0x48

/*
                 
 */

/*               */
/*                                                           */
#define H_CB_WW    0
/*                                      */
#define H_CSR      4
/*                                                                */
#define ME_CB_RW   8
/*                                                                */
#define ME_CSR_HA  0xC


/*                                                       */
/*                                                                     */
#define H_CBD             0xFF000000
/*                                    */
#define H_CBWP            0x00FF0000
/*                                   */
#define H_CBRP            0x0000FF00
/*            */
#define H_RST             0x00000010
/*            */
#define H_RDY             0x00000008
/*                         */
#define H_IG              0x00000004
/*                       */
#define H_IS              0x00000002
/*                       */
#define H_IE              0x00000001


/*                                                                     */
/*                                                                      
                 */
#define ME_CBD_HRA        0xFF000000
/*                                                            */
#define ME_CBWP_HRA       0x00FF0000
/*                                                           */
#define ME_CBRP_HRA       0x0000FF00
/*                                                */
#define ME_RST_HRA        0x00000010
/*                                                */
#define ME_RDY_HRA        0x00000008
/*                                                            */
#define ME_IG_HRA         0x00000004
/*                                                          */
#define ME_IS_HRA         0x00000002
/*                                                          */
#define ME_IE_HRA         0x00000001

#endif /*                     */
