/*******************************************************************************

  Intel PRO/1000 Linux driver
  Copyright(c) 1999 - 2013 Intel Corporation.

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

  Contact Information:
  Linux NICS <linux.nics@intel.com>
  e1000-devel Mailing List <e1000-devel@lists.sourceforge.net>
  Intel Corporation, 5200 N.E. Elam Young Parkway, Hillsboro, OR 97124-6497

*******************************************************************************/

#ifndef _E1000E_REGS_H_
#define _E1000E_REGS_H_

#define E1000_CTRL	0x00000	/*                     */
#define E1000_STATUS	0x00008	/*                    */
#define E1000_EECD	0x00010	/*                           */
#define E1000_EERD	0x00014	/*                  */
#define E1000_CTRL_EXT	0x00018	/*                              */
#define E1000_FLA	0x0001C	/*                   */
#define E1000_MDIC	0x00020	/*                  */
#define E1000_SCTL	0x00024	/*                     */
#define E1000_FCAL	0x00028	/*                               */
#define E1000_FCAH	0x0002C	/*                               */
#define E1000_FEXTNVM	0x00028	/*                          */
#define E1000_FEXTNVM3	0x0003C	/*                            */
#define E1000_FEXTNVM4	0x00024	/*                            */
#define E1000_FEXTNVM6	0x00010	/*                            */
#define E1000_FEXTNVM7	0x000E4	/*                            */
#define E1000_FCT	0x00030	/*                        */
#define E1000_VET	0x00038	/*                      */
#define E1000_ICR	0x000C0	/*                              */
#define E1000_ITR	0x000C4	/*                                */
#define E1000_ICS	0x000C8	/*                          */
#define E1000_IMS	0x000D0	/*                         */
#define E1000_IMC	0x000D8	/*                           */
#define E1000_IAM	0x000E0	/*                                 */
#define E1000_IVAR	0x000E4	/*                                           */
#define E1000_SVCR	0x000F0
#define E1000_SVT	0x000F4
#define E1000_LPIC	0x000FC	/*                        */
#define E1000_RCTL	0x00100	/*                 */
#define E1000_FCTTV	0x00170	/*                                        */
#define E1000_TXCW	0x00178	/*                            */
#define E1000_RXCW	0x00180	/*                            */
#define E1000_PBA_ECC	0x01100	/*                  */
#define E1000_TCTL	0x00400	/*                 */
#define E1000_TCTL_EXT	0x00404	/*                          */
#define E1000_TIPG	0x00410	/*                         */
#define E1000_AIT	0x00458	/*                                           */
#define E1000_LEDCTL	0x00E00	/*                  */
#define E1000_EXTCNF_CTRL	0x00F00	/*                                */
#define E1000_EXTCNF_SIZE	0x00F08	/*                             */
#define E1000_PHY_CTRL	0x00F10	/*                             */
#define E1000_POEMB	E1000_PHY_CTRL	/*              */
#define E1000_PBA	0x01000	/*                               */
#define E1000_PBS	0x01008	/*                    */
#define E1000_PBECCSTS	0x0100C	/*                               */
#define E1000_EEMNGCTL	0x01010	/*                    */
#define E1000_EEWR	0x0102C	/*                            */
#define E1000_FLOP	0x0103C	/*                       */
#define E1000_ERT	0x02008	/*                         */
#define E1000_FCRTL	0x02160	/*                                         */
#define E1000_FCRTH	0x02168	/*                                          */
#define E1000_PSRCTL	0x02170	/*                                   */
#define E1000_RDFH	0x02410	/*                        */
#define E1000_RDFT	0x02418	/*                        */
#define E1000_RDFHS	0x02420	/*                              */
#define E1000_RDFTS	0x02428	/*                              */
#define E1000_RDFPC	0x02430	/*                                */
/*                                       */
#define E1000_RDTR	0x02820	/*                     */
#define E1000_RADV	0x0282C	/*                                        */
/*                   
  
                                                                   
  
                 
                                    
 */
#define E1000_RDBAL(_n)	((_n) < 4 ? (0x02800 + ((_n) * 0x100)) : \
			 (0x0C000 + ((_n) * 0x40)))
#define E1000_RDBAH(_n)	((_n) < 4 ? (0x02804 + ((_n) * 0x100)) : \
			 (0x0C004 + ((_n) * 0x40)))
#define E1000_RDLEN(_n)	((_n) < 4 ? (0x02808 + ((_n) * 0x100)) : \
			 (0x0C008 + ((_n) * 0x40)))
#define E1000_RDH(_n)	((_n) < 4 ? (0x02810 + ((_n) * 0x100)) : \
			 (0x0C010 + ((_n) * 0x40)))
#define E1000_RDT(_n)	((_n) < 4 ? (0x02818 + ((_n) * 0x100)) : \
			 (0x0C018 + ((_n) * 0x40)))
#define E1000_RXDCTL(_n)	((_n) < 4 ? (0x02828 + ((_n) * 0x100)) : \
				 (0x0C028 + ((_n) * 0x40)))
#define E1000_TDBAL(_n)	((_n) < 4 ? (0x03800 + ((_n) * 0x100)) : \
			 (0x0E000 + ((_n) * 0x40)))
#define E1000_TDBAH(_n)	((_n) < 4 ? (0x03804 + ((_n) * 0x100)) : \
			 (0x0E004 + ((_n) * 0x40)))
#define E1000_TDLEN(_n)	((_n) < 4 ? (0x03808 + ((_n) * 0x100)) : \
			 (0x0E008 + ((_n) * 0x40)))
#define E1000_TDH(_n)	((_n) < 4 ? (0x03810 + ((_n) * 0x100)) : \
			 (0x0E010 + ((_n) * 0x40)))
#define E1000_TDT(_n)	((_n) < 4 ? (0x03818 + ((_n) * 0x100)) : \
			 (0x0E018 + ((_n) * 0x40)))
#define E1000_TXDCTL(_n)	((_n) < 4 ? (0x03828 + ((_n) * 0x100)) : \
				 (0x0E028 + ((_n) * 0x40)))
#define E1000_TARC(_n)		(0x03840 + ((_n) * 0x100))
#define E1000_KABGTXD		0x03004	/*                                */
#define E1000_RAL(_i)		(((_i) <= 15) ? (0x05400 + ((_i) * 8)) : \
				 (0x054E0 + ((_i - 16) * 8)))
#define E1000_RAH(_i)		(((_i) <= 15) ? (0x05404 + ((_i) * 8)) : \
				 (0x054E4 + ((_i - 16) * 8)))
#define E1000_SHRAL(_i)		(0x05438 + ((_i) * 8))
#define E1000_SHRAH(_i)		(0x0543C + ((_i) * 8))
#define E1000_TDFH		0x03410	/*                        */
#define E1000_TDFT		0x03418	/*                        */
#define E1000_TDFHS		0x03420	/*                              */
#define E1000_TDFTS		0x03428	/*                              */
#define E1000_TDFPC		0x03430	/*                                */
#define E1000_TIDV	0x03820	/*                               */
#define E1000_TADV	0x0382C	/*                                      */
#define E1000_CRCERRS	0x04000	/*                         */
#define E1000_ALGNERRC	0x04004	/*                               */
#define E1000_SYMERRS	0x04008	/*                            */
#define E1000_RXERRC	0x0400C	/*                             */
#define E1000_MPC	0x04010	/*                             */
#define E1000_SCC	0x04014	/*                                */
#define E1000_ECOL	0x04018	/*                                   */
#define E1000_MCC	0x0401C	/*                                  */
#define E1000_LATECOL	0x04020	/*                              */
#define E1000_COLC	0x04028	/*                         */
#define E1000_DC	0x04030	/*                     */
#define E1000_TNCRS	0x04034	/*                   */
#define E1000_SEC	0x04038	/*                              */
#define E1000_CEXTERR	0x0403C	/*                                       */
#define E1000_RLEC	0x04040	/*                                    */
#define E1000_XONRXC	0x04048	/*                      */
#define E1000_XONTXC	0x0404C	/*                      */
#define E1000_XOFFRXC	0x04050	/*                       */
#define E1000_XOFFTXC	0x04054	/*                       */
#define E1000_FCRUC	0x04058	/*                                          */
#define E1000_PRC64	0x0405C	/*                               */
#define E1000_PRC127	0x04060	/*                                   */
#define E1000_PRC255	0x04064	/*                                    */
#define E1000_PRC511	0x04068	/*                                    */
#define E1000_PRC1023	0x0406C	/*                                     */
#define E1000_PRC1522	0x04070	/*                                      */
#define E1000_GPRC	0x04074	/*                               */
#define E1000_BPRC	0x04078	/*                                    */
#define E1000_MPRC	0x0407C	/*                                    */
#define E1000_GPTC	0x04080	/*                               */
#define E1000_GORCL	0x04088	/*                                  */
#define E1000_GORCH	0x0408C	/*                                   */
#define E1000_GOTCL	0x04090	/*                                  */
#define E1000_GOTCH	0x04094	/*                                   */
#define E1000_RNBC	0x040A0	/*                             */
#define E1000_RUC	0x040A4	/*                            */
#define E1000_RFC	0x040A8	/*                           */
#define E1000_ROC	0x040AC	/*                           */
#define E1000_RJC	0x040B0	/*                         */
#define E1000_MGTPRC	0x040B4	/*                                     */
#define E1000_MGTPDC	0x040B8	/*                                          */
#define E1000_MGTPTC	0x040BC	/*                                     */
#define E1000_TORL	0x040C0	/*                             */
#define E1000_TORH	0x040C4	/*                              */
#define E1000_TOTL	0x040C8	/*                             */
#define E1000_TOTH	0x040CC	/*                              */
#define E1000_TPR	0x040D0	/*                          */
#define E1000_TPT	0x040D4	/*                          */
#define E1000_PTC64	0x040D8	/*                               */
#define E1000_PTC127	0x040DC	/*                                   */
#define E1000_PTC255	0x040E0	/*                                    */
#define E1000_PTC511	0x040E4	/*                                    */
#define E1000_PTC1023	0x040E8	/*                                     */
#define E1000_PTC1522	0x040EC	/*                                      */
#define E1000_MPTC	0x040F0	/*                                    */
#define E1000_BPTC	0x040F4	/*                                    */
#define E1000_TSCTC	0x040F8	/*                                     */
#define E1000_TSCTFC	0x040FC	/*                                          */
#define E1000_IAC	0x04100	/*                           */
#define E1000_ICRXPTC	0x04104	/*                                           */
#define E1000_ICRXATC	0x04108	/*                                           */
#define E1000_ICTXPTC	0x0410C	/*                                           */
#define E1000_ICTXATC	0x04110	/*                                           */
#define E1000_ICTXQEC	0x04118	/*                                      */
#define E1000_ICTXQMTC	0x0411C	/*                                           */
#define E1000_ICRXDMTC	0x04120	/*                                          */
#define E1000_ICRXOC	0x04124	/*                                        */
#define E1000_CRC_OFFSET	0x05F50	/*                     */

#define E1000_PCS_LCTL	0x04208	/*                       */
#define E1000_PCS_LSTAT	0x0420C	/*                      */
#define E1000_PCS_ANADV	0x04218	/*                       */
#define E1000_PCS_LPAB	0x0421C	/*                           */
#define E1000_RXCSUM	0x05000	/*                          */
#define E1000_RFCTL	0x05008	/*                        */
#define E1000_MTA	0x05200	/*                                  */
#define E1000_RA	0x05400	/*                            */
#define E1000_VFTA	0x05600	/*                                    */
#define E1000_WUC	0x05800	/*                     */
#define E1000_WUFC	0x05808	/*                            */
#define E1000_WUS	0x05810	/*                    */
#define E1000_MANC	0x05820	/*                         */
#define E1000_FFLT	0x05F00	/*                                         */
#define E1000_HOST_IF	0x08800	/*                */

#define E1000_KMRNCTRLSTA	0x00034	/*                        */
#define E1000_MANC2H		0x05860	/*                                 */
/*                             */
#define E1000_MDEF(_n)		(0x05890 + (4 * (_n)))
#define E1000_SW_FW_SYNC	0x05B5C	/*                            */
#define E1000_GCR	0x05B00	/*                */
#define E1000_GCR2	0x05B64	/*                   */
#define E1000_FACTPS	0x05B30	/*                                        */
#define E1000_SWSM	0x05B50	/*              */
#define E1000_FWSM	0x05B54	/*              */
/*                                                    */
#define E1000_SWSM2	0x05B58
#define E1000_FFLT_DBG	0x05F04	/*                */
#define E1000_HICR	0x08F00	/*                        */

/*               */
#define E1000_MRQC	0x05818	/*                               */
#define E1000_RETA(_i)	(0x05C00 + ((_i) * 4))	/*                        */
#define E1000_RSSRK(_i)	(0x05C80 + ((_i) * 4))	/*                     */
#define E1000_TSYNCRXCTL	0x0B620	/*                                    */
#define E1000_TSYNCTXCTL	0x0B614	/*                                    */
#define E1000_RXSTMPL	0x0B624	/*                       */
#define E1000_RXSTMPH	0x0B628	/*                        */
#define E1000_TXSTMPL	0x0B618	/*                             */
#define E1000_TXSTMPH	0x0B61C	/*                              */
#define E1000_SYSTIML	0x0B600	/*                               */
#define E1000_SYSTIMH	0x0B604	/*                                */
#define E1000_TIMINCA	0x0B608	/*                                    */
#define E1000_RXMTRL	0x0B634	/*                                          */
#define E1000_RXUDP	0x0B638	/*                            */

#endif
