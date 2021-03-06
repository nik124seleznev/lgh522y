/*******************************************************************************

  Intel(R) Gigabit Ethernet Linux driver
  Copyright(c) 2007-2013 Intel Corporation.

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
  e1000-devel Mailing List <e1000-devel@lists.sourceforge.net>
  Intel Corporation, 5200 N.E. Elam Young Parkway, Hillsboro, OR 97124-6497

*******************************************************************************/

#ifndef _E1000_REGS_H_
#define _E1000_REGS_H_

#define E1000_CTRL     0x00000  /*                     */
#define E1000_STATUS   0x00008  /*                    */
#define E1000_EECD     0x00010  /*                           */
#define E1000_EERD     0x00014  /*                  */
#define E1000_CTRL_EXT 0x00018  /*                              */
#define E1000_MDIC     0x00020  /*                  */
#define E1000_MDICNFG  0x00E04  /*                 */
#define E1000_SCTL     0x00024  /*                     */
#define E1000_FCAL     0x00028  /*                               */
#define E1000_FCAH     0x0002C  /*                               */
#define E1000_FCT      0x00030  /*                        */
#define E1000_CONNSW   0x00034  /*                                  */
#define E1000_VET      0x00038  /*                      */
#define E1000_ICR      0x000C0  /*                              */
#define E1000_ITR      0x000C4  /*                                */
#define E1000_ICS      0x000C8  /*                          */
#define E1000_IMS      0x000D0  /*                         */
#define E1000_IMC      0x000D8  /*                           */
#define E1000_IAM      0x000E0  /*                                 */
#define E1000_RCTL     0x00100  /*                 */
#define E1000_FCTTV    0x00170  /*                                        */
#define E1000_TXCW     0x00178  /*                            */
#define E1000_EICR     0x01580  /*                                   */
#define E1000_EITR(_n) (0x01680 + (0x4 * (_n)))
#define E1000_EICS     0x01520  /*                               */
#define E1000_EIMS     0x01524  /*                                   */
#define E1000_EIMC     0x01528  /*                                */
#define E1000_EIAC     0x0152C  /*                                */
#define E1000_EIAM     0x01530  /*                                         */
#define E1000_GPIE     0x01514  /*                                       */
#define E1000_IVAR0    0x01700  /*                                          */
#define E1000_IVAR_MISC 0x01740 /*                              */
#define E1000_TCTL     0x00400  /*                 */
#define E1000_TCTL_EXT 0x00404  /*                          */
#define E1000_TIPG     0x00410  /*                         */
#define E1000_AIT      0x00458  /*                                           */
#define E1000_LEDCTL   0x00E00  /*                  */
#define E1000_LEDMUX   0x08130  /*                 */
#define E1000_PBA      0x01000  /*                               */
#define E1000_PBS      0x01008  /*                    */
#define E1000_EEMNGCTL 0x01010  /*                    */
#define E1000_EEWR     0x0102C  /*                            */
#define E1000_I2CCMD   0x01028  /*                              */
#define E1000_FRTIMER  0x01048  /*                         */
#define E1000_TCPTIMER 0x0104C  /*                */
#define E1000_FCRTL    0x02160  /*                                         */
#define E1000_FCRTH    0x02168  /*                                          */
#define E1000_FCRTV    0x02460  /*                                       */
#define E1000_I2CPARAMS        0x0102C /*                                 */
#define E1000_I2CBB_EN      0x00000100  /*                       */
#define E1000_I2C_CLK_OUT   0x00000200  /*            */
#define E1000_I2C_DATA_OUT  0x00000400  /*               */
#define E1000_I2C_DATA_OE_N 0x00000800  /*                         */
#define E1000_I2C_DATA_IN   0x00001000  /*              */
#define E1000_I2C_CLK_OE_N  0x00002000  /*                          */
#define E1000_I2C_CLK_IN    0x00004000  /*               */
#define E1000_MPHY_ADDR_CTRL	0x0024 /*                          */
#define E1000_MPHY_DATA		0x0E10 /*               */
#define E1000_MPHY_STAT		0x0E0C /*                     */

/*                     */
#define E1000_TSYNCRXCTL 0x0B620 /*                                    */
#define E1000_TSYNCTXCTL 0x0B614 /*                                    */
#define E1000_TSYNCRXCFG 0x05F50 /*                                 */
#define E1000_RXSTMPL    0x0B624 /*                       */
#define E1000_RXSTMPH    0x0B628 /*                        */
#define E1000_RXSATRL    0x0B62C /*                                 */
#define E1000_RXSATRH    0x0B630 /*                                  */
#define E1000_TXSTMPL    0x0B618 /*                             */
#define E1000_TXSTMPH    0x0B61C /*                              */
#define E1000_SYSTIML    0x0B600 /*                               */
#define E1000_SYSTIMH    0x0B604 /*                                */
#define E1000_TIMINCA    0x0B608 /*                                    */
#define E1000_TSAUXC     0x0B640 /*                                     */
#define E1000_SYSTIMR    0x0B6F8 /*                              */
#define E1000_TSICR      0x0B66C /*                          */
#define E1000_TSIM       0x0B674 /*                         */

/*                     */
#define E1000_SAQF(_n) (0x5980 + 4 * (_n))
#define E1000_DAQF(_n) (0x59A0 + 4 * (_n))
#define E1000_SPQF(_n) (0x59C0 + 4 * (_n))
#define E1000_FTQF(_n) (0x59E0 + 4 * (_n))
#define E1000_SAQF0 E1000_SAQF(0)
#define E1000_DAQF0 E1000_DAQF(0)
#define E1000_SPQF0 E1000_SPQF(0)
#define E1000_FTQF0 E1000_FTQF(0)
#define E1000_SYNQF(_n) (0x055FC + (4 * (_n))) /*                       */
#define E1000_ETQF(_n)  (0x05CB0 + (4 * (_n))) /*                  */

#define E1000_RQDPC(_n) (0x0C030 + ((_n) * 0x40))

/*                          */
#define E1000_DMACR	0x02508 /*                  */
#define E1000_DMCTXTH	0x03550 /*                    */
#define E1000_DMCTLX	0x02514 /*                    */
#define E1000_DMCRTRH	0x05DD0 /*                               */
#define E1000_DMCCNT	0x05DD4 /*                  */
#define E1000_FCRTC	0x02170 /*                                */
#define E1000_PCIEMISC	0x05BB8 /*                           */

/*                         */
#define E1000_RTTDQSEL	0x3604 /*                                 */
#define E1000_RTTBCNRM	0x3690 /*                           */
#define E1000_RTTBCNRC	0x36B0 /*                                   */

/*                                       */
#define E1000_RXPBS	0x02404 /*                            */

/*                                                   */
#define E1000_THMJT	0x08100 /*                      */
#define E1000_THLOWTC	0x08104 /*                       */
#define E1000_THMIDTC	0x08108 /*                       */
#define E1000_THHIGHTC	0x0810C /*                        */
#define E1000_THSTAT	0x08110 /*                       */

/*                   
  
                                                                   
  
                 
                                    
 */
#define E1000_RDBAL(_n)   ((_n) < 4 ? (0x02800 + ((_n) * 0x100)) \
				    : (0x0C000 + ((_n) * 0x40)))
#define E1000_RDBAH(_n)   ((_n) < 4 ? (0x02804 + ((_n) * 0x100)) \
				    : (0x0C004 + ((_n) * 0x40)))
#define E1000_RDLEN(_n)   ((_n) < 4 ? (0x02808 + ((_n) * 0x100)) \
				    : (0x0C008 + ((_n) * 0x40)))
#define E1000_SRRCTL(_n)  ((_n) < 4 ? (0x0280C + ((_n) * 0x100)) \
				    : (0x0C00C + ((_n) * 0x40)))
#define E1000_RDH(_n)     ((_n) < 4 ? (0x02810 + ((_n) * 0x100)) \
				    : (0x0C010 + ((_n) * 0x40)))
#define E1000_RDT(_n)     ((_n) < 4 ? (0x02818 + ((_n) * 0x100)) \
				    : (0x0C018 + ((_n) * 0x40)))
#define E1000_RXDCTL(_n)  ((_n) < 4 ? (0x02828 + ((_n) * 0x100)) \
				    : (0x0C028 + ((_n) * 0x40)))
#define E1000_TDBAL(_n)   ((_n) < 4 ? (0x03800 + ((_n) * 0x100)) \
				    : (0x0E000 + ((_n) * 0x40)))
#define E1000_TDBAH(_n)   ((_n) < 4 ? (0x03804 + ((_n) * 0x100)) \
				    : (0x0E004 + ((_n) * 0x40)))
#define E1000_TDLEN(_n)   ((_n) < 4 ? (0x03808 + ((_n) * 0x100)) \
				    : (0x0E008 + ((_n) * 0x40)))
#define E1000_TDH(_n)     ((_n) < 4 ? (0x03810 + ((_n) * 0x100)) \
				    : (0x0E010 + ((_n) * 0x40)))
#define E1000_TDT(_n)     ((_n) < 4 ? (0x03818 + ((_n) * 0x100)) \
				    : (0x0E018 + ((_n) * 0x40)))
#define E1000_TXDCTL(_n)  ((_n) < 4 ? (0x03828 + ((_n) * 0x100)) \
				    : (0x0E028 + ((_n) * 0x40)))
#define E1000_RXCTL(_n)	  ((_n) < 4 ? (0x02814 + ((_n) * 0x100)) : \
				      (0x0C014 + ((_n) * 0x40)))
#define E1000_DCA_RXCTRL(_n)	E1000_RXCTL(_n)
#define E1000_TXCTL(_n)   ((_n) < 4 ? (0x03814 + ((_n) * 0x100)) : \
				      (0x0E014 + ((_n) * 0x40)))
#define E1000_DCA_TXCTRL(_n) E1000_TXCTL(_n)
#define E1000_TDWBAL(_n)  ((_n) < 4 ? (0x03838 + ((_n) * 0x100)) \
				    : (0x0E038 + ((_n) * 0x40)))
#define E1000_TDWBAH(_n)  ((_n) < 4 ? (0x0383C + ((_n) * 0x100)) \
				    : (0x0E03C + ((_n) * 0x40)))
#define E1000_TDFH     0x03410  /*                        */
#define E1000_TDFT     0x03418  /*                        */
#define E1000_TDFHS    0x03420  /*                              */
#define E1000_TDFPC    0x03430  /*                                */
#define E1000_DTXCTL   0x03590  /*                     */
#define E1000_CRCERRS  0x04000  /*                         */
#define E1000_ALGNERRC 0x04004  /*                               */
#define E1000_SYMERRS  0x04008  /*                            */
#define E1000_RXERRC   0x0400C  /*                             */
#define E1000_MPC      0x04010  /*                             */
#define E1000_SCC      0x04014  /*                                */
#define E1000_ECOL     0x04018  /*                                   */
#define E1000_MCC      0x0401C  /*                                  */
#define E1000_LATECOL  0x04020  /*                              */
#define E1000_COLC     0x04028  /*                         */
#define E1000_DC       0x04030  /*                     */
#define E1000_TNCRS    0x04034  /*                   */
#define E1000_SEC      0x04038  /*                              */
#define E1000_CEXTERR  0x0403C  /*                                       */
#define E1000_RLEC     0x04040  /*                                    */
#define E1000_XONRXC   0x04048  /*                      */
#define E1000_XONTXC   0x0404C  /*                      */
#define E1000_XOFFRXC  0x04050  /*                       */
#define E1000_XOFFTXC  0x04054  /*                       */
#define E1000_FCRUC    0x04058  /*                                          */
#define E1000_PRC64    0x0405C  /*                               */
#define E1000_PRC127   0x04060  /*                                   */
#define E1000_PRC255   0x04064  /*                                    */
#define E1000_PRC511   0x04068  /*                                    */
#define E1000_PRC1023  0x0406C  /*                                     */
#define E1000_PRC1522  0x04070  /*                                      */
#define E1000_GPRC     0x04074  /*                               */
#define E1000_BPRC     0x04078  /*                                    */
#define E1000_MPRC     0x0407C  /*                                    */
#define E1000_GPTC     0x04080  /*                               */
#define E1000_GORCL    0x04088  /*                                  */
#define E1000_GORCH    0x0408C  /*                                   */
#define E1000_GOTCL    0x04090  /*                                  */
#define E1000_GOTCH    0x04094  /*                                   */
#define E1000_RNBC     0x040A0  /*                             */
#define E1000_RUC      0x040A4  /*                            */
#define E1000_RFC      0x040A8  /*                           */
#define E1000_ROC      0x040AC  /*                           */
#define E1000_RJC      0x040B0  /*                         */
#define E1000_MGTPRC   0x040B4  /*                                     */
#define E1000_MGTPDC   0x040B8  /*                                          */
#define E1000_MGTPTC   0x040BC  /*                                     */
#define E1000_TORL     0x040C0  /*                             */
#define E1000_TORH     0x040C4  /*                              */
#define E1000_TOTL     0x040C8  /*                             */
#define E1000_TOTH     0x040CC  /*                              */
#define E1000_TPR      0x040D0  /*                          */
#define E1000_TPT      0x040D4  /*                          */
#define E1000_PTC64    0x040D8  /*                               */
#define E1000_PTC127   0x040DC  /*                                   */
#define E1000_PTC255   0x040E0  /*                                    */
#define E1000_PTC511   0x040E4  /*                                    */
#define E1000_PTC1023  0x040E8  /*                                     */
#define E1000_PTC1522  0x040EC  /*                                      */
#define E1000_MPTC     0x040F0  /*                                    */
#define E1000_BPTC     0x040F4  /*                                    */
#define E1000_TSCTC    0x040F8  /*                                     */
#define E1000_TSCTFC   0x040FC  /*                                          */
#define E1000_IAC      0x04100  /*                           */
/*                                              */
#define E1000_ICRXPTC  0x04104
/*                                                */
#define E1000_ICRXATC  0x04108
/*                                              */
#define E1000_ICTXPTC  0x0410C
/*                                                */
#define E1000_ICTXATC  0x04110
/*                                      */
#define E1000_ICTXQEC  0x04118
/*                                                  */
#define E1000_ICTXQMTC 0x0411C
/*                                                       */
#define E1000_ICRXDMTC 0x04120
#define E1000_ICRXOC   0x04124  /*                                        */
#define E1000_PCS_CFG0    0x04200  /*                          */
#define E1000_PCS_LCTL    0x04208  /*                       */
#define E1000_PCS_LSTAT   0x0420C  /*                      */
#define E1000_CBTMPC      0x0402C  /*                                 */
#define E1000_HTDPMC      0x0403C  /*                                 */
#define E1000_CBRMPC      0x040FC  /*                                 */
#define E1000_RPTHC       0x04104  /*                    */
#define E1000_HGPTC       0x04118  /*                            */
#define E1000_HTCBDPC     0x04124  /*                                       */
#define E1000_HGORCL      0x04128  /*                                     */
#define E1000_HGORCH      0x0412C  /*                                      */
#define E1000_HGOTCL      0x04130  /*                                     */
#define E1000_HGOTCH      0x04134  /*                                      */
#define E1000_LENERRS     0x04138  /*                     */
#define E1000_SCVPC       0x04228  /*                                       */
#define E1000_PCS_ANADV   0x04218  /*                       */
#define E1000_PCS_LPAB    0x0421C  /*                           */
#define E1000_PCS_NPTX    0x04220  /*                            */
#define E1000_PCS_LPABNP  0x04224  /*                                     */
#define E1000_RXCSUM   0x05000  /*                          */
#define E1000_RLPML    0x05004  /*                           */
#define E1000_RFCTL    0x05008  /*                       */
#define E1000_MTA      0x05200  /*                                  */
#define E1000_RA       0x05400  /*                            */
#define E1000_RA2      0x054E0  /*                                         */
#define E1000_PSRTYPE(_i)       (0x05480 + ((_i) * 4))
#define E1000_RAL(_i)  (((_i) <= 15) ? (0x05400 + ((_i) * 8)) : \
                                       (0x054E0 + ((_i - 16) * 8)))
#define E1000_RAH(_i)  (((_i) <= 15) ? (0x05404 + ((_i) * 8)) : \
                                       (0x054E4 + ((_i - 16) * 8)))
#define E1000_IP4AT_REG(_i)     (0x05840 + ((_i) * 8))
#define E1000_IP6AT_REG(_i)     (0x05880 + ((_i) * 4))
#define E1000_WUPM_REG(_i)      (0x05A00 + ((_i) * 4))
#define E1000_FFMT_REG(_i)      (0x09000 + ((_i) * 8))
#define E1000_FFVT_REG(_i)      (0x09800 + ((_i) * 8))
#define E1000_FFLT_REG(_i)      (0x05F00 + ((_i) * 8))
#define E1000_VFTA     0x05600  /*                                    */
#define E1000_VT_CTL   0x0581C  /*                   */
#define E1000_WUC      0x05800  /*                     */
#define E1000_WUFC     0x05808  /*                            */
#define E1000_WUS      0x05810  /*                    */
#define E1000_MANC     0x05820  /*                         */
#define E1000_IPAV     0x05838  /*                       */
#define E1000_WUPL     0x05900  /*                           */

#define E1000_SW_FW_SYNC  0x05B5C /*                                        */
#define E1000_CCMCTL      0x05B48 /*                      */
#define E1000_GIOCTL      0x05B44 /*                             */
#define E1000_SCCTL       0x05B4C /*                                 */
#define E1000_GCR         0x05B00 /*                */
#define E1000_FACTPS    0x05B30 /*                                        */
#define E1000_SWSM      0x05B50 /*              */
#define E1000_FWSM      0x05B54 /*              */
#define E1000_DCA_CTRL  0x05B74 /*                  */

/*               */
#define E1000_MRQC      0x05818 /*                               */
#define E1000_IMIR(_i)      (0x05A80 + ((_i) * 4))  /*                     */
#define E1000_IMIREXT(_i)   (0x05AA0 + ((_i) * 4))  /*                        */
#define E1000_IMIRVP    0x05AC0 /*                                           */
/*                                     */
#define E1000_MSIXBM(_i)    (0x01600 + ((_i) * 4))
/*                              */
#define E1000_RETA(_i)  (0x05C00 + ((_i) * 4))
#define E1000_RSSRK(_i) (0x05C80 + ((_i) * 4)) /*                           */

/*              */
#define E1000_MBVFICR   0x00C80 /*                        */
#define E1000_MBVFIMR   0x00C84 /*                          */
#define E1000_VFLRE     0x00C88 /*                          */
#define E1000_VFRE      0x00C8C /*                    */
#define E1000_VFTE      0x00C90 /*                     */
#define E1000_QDE       0x02408 /*                        */
#define E1000_DTXSWC    0x03500 /*                            */
#define E1000_WVBR      0x03554 /*                         */
#define E1000_RPLOLR    0x05AF0 /*                          */
#define E1000_UTA       0x0A000 /*                          */
#define E1000_IOVTCL    0x05BBC /*                      */
#define E1000_TXSWC     0x05ACC /*                   */
/*                                                     */
#define E1000_P2VMAILBOX(_n)   (0x00C00 + (4 * (_n)))
#define E1000_VMBMEM(_n)       (0x00800 + (64 * (_n)))
#define E1000_VMOLR(_n)        (0x05AD0 + (4 * (_n)))
#define E1000_VLVF(_n)         (0x05D00 + (4 * (_n))) /*                     
                                                                     */
#define E1000_VMVIR(_n)        (0x03700 + (4 * (_n)))

#define wr32(reg, value) (writel(value, hw->hw_addr + reg))
#define rd32(reg) (readl(hw->hw_addr + reg))
#define wrfl() ((void)rd32(E1000_STATUS))

#define array_wr32(reg, offset, value) \
	(writel(value, hw->hw_addr + reg + ((offset) << 2)))
#define array_rd32(reg, offset) \
	(readl(hw->hw_addr + reg + ((offset) << 2)))

/*                          */
#define E1000_PCIEMISC	0x05BB8 /*                           */

/*                                          */
#define E1000_IPCNFG	0x0E38 /*                            */
#define E1000_EEER	0x0E30 /*                           */
#define E1000_EEE_SU	0X0E34 /*           */
#define E1000_EMIADD	0x10   /*                                  */
#define E1000_EMIDATA	0x11   /*                               */
#define E1000_MMDAC	13     /*                    */
#define E1000_MMDAAD	14     /*                         */

/*                         */
#define E1000_THSTAT	0x08110 /*                       */

/*                  */
#define E1000_B2OSPC	0x08FE0 /*                            */
#define E1000_B2OGPRC	0x04158 /*                                 */
#define E1000_O2BGPTC	0x08FE4 /*                                */
#define E1000_O2BSPC	0x0415C /*                                    */

#define E1000_SRWR		0x12018  /*                                */
#define E1000_I210_FLMNGCTL	0x12038
#define E1000_I210_FLMNGDATA	0x1203C
#define E1000_I210_FLMNGCNT	0x12040

#define E1000_I210_FLSWCTL	0x12048
#define E1000_I210_FLSWDATA	0x1204C
#define E1000_I210_FLSWCNT	0x12050

#define E1000_I210_FLA		0x1201C

#define E1000_INVM_DATA_REG(_n)	(0x12120 + 4*(_n))
#define E1000_INVM_SIZE		64 /*                               */

#endif
