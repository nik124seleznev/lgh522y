/******************************************************************************
  PTP Header file

  Copyright (C) 2013  Vayavya Labs Pvt Ltd

  This program is free software; you can redistribute it and/or modify it
  under the terms and conditions of the GNU General Public License,
  version 2, as published by the Free Software Foundation.

  This program is distributed in the hope it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

  The full GNU General Public License is included in this distribution in
  the file called "COPYING".

  Author: Rayagond Kokatanur <rayagond@vayavyalabs.com>
******************************************************************************/

#ifndef __STMMAC_PTP_H__
#define __STMMAC_PTP_H__

#define STMMAC_SYSCLOCK 62500000

/*                                */
#define PTP_TCR		0x0700	/*                       */
#define PTP_SSIR	0x0704	/*                          */
#define PTP_STSR	0x0708	/*                              */
#define PTP_STNSR	0x070C	/*                                 */
#define PTP_STSUR	0x0710	/*                                    */
#define PTP_STNSUR	0x0714	/*                                        */
#define PTP_TAR		0x0718	/*                      */
#define PTP_TTSR	0x071C	/*                         */
#define PTP_TTNSR	0x0720	/*                             */
#define	PTP_STHWSR	0x0724	/*                                       */
#define PTP_TSR		0x0728	/*                  */

#define PTP_STNSUR_ADDSUB_SHIFT 31

/*                 */
#define PTP_TCR_TSENA		0x00000001 /*                  */
#define PTP_TCR_TSCFUPDT	0x00000002 /*                              */
#define PTP_TCR_TSINIT		0x00000004 /*                      */
#define PTP_TCR_TSUPDT		0x00000008 /*                  */
/*                                    */
#define PTP_TCR_TSTRIG		0x00000010
#define PTP_TCR_TSADDREG	0x00000020 /*                   */
#define PTP_TCR_TSENALL		0x00000100 /*                                 */
/*                                              */
#define PTP_TCR_TSCTRLSSR	0x00000200

/*                                                   */
#define PTP_TCR_TSVER2ENA	0x00000400
/*                                               */
#define PTP_TCR_TSIPENA		0x00000800
/*                                                    */
#define PTP_TCR_TSIPV6ENA	0x00001000
/*                                                    */
#define PTP_TCR_TSIPV4ENA	0x00002000
/*                                              */
#define PTP_TCR_TSEVNTENA	0x00004000
/*                                                 */
#define PTP_TCR_TSMSTRENA	0x00008000
/*                                         */
#define PTP_TCR_SNAPTYPSEL_1	0x00010000
/*                                            */
#define PTP_TCR_TSENMACADDR	0x00040000

#endif /*                  */
