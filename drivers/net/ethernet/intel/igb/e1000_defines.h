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

#ifndef _E1000_DEFINES_H_
#define _E1000_DEFINES_H_

/*                                                                    */
#define REQ_TX_DESCRIPTOR_MULTIPLE  8
#define REQ_RX_DESCRIPTOR_MULTIPLE  8

/*                                                       */
/*                 */
#define E1000_WUC_PME_EN     0x00000002 /*            */

/*                        */
#define E1000_WUFC_LNKC 0x00000001 /*                                  */
#define E1000_WUFC_MAG  0x00000002 /*                            */
#define E1000_WUFC_EX   0x00000004 /*                              */
#define E1000_WUFC_MC   0x00000008 /*                                  */
#define E1000_WUFC_BC   0x00000010 /*                         */

/*                         */
#define E1000_CTRL_EXT_SDP3_DATA 0x00000080 /*                              */
/*                                     */
#define E1000_CTRL_EXT_PFRSTD    0x00004000
#define E1000_CTRL_EXT_LINK_MODE_MASK 0x00C00000
#define E1000_CTRL_EXT_LINK_MODE_PCIE_SERDES  0x00C00000
#define E1000_CTRL_EXT_LINK_MODE_1000BASE_KX  0x00400000
#define E1000_CTRL_EXT_LINK_MODE_SGMII   0x00800000
#define E1000_CTRL_EXT_LINK_MODE_GMII   0x00000000
#define E1000_CTRL_EXT_EIAME          0x01000000
#define E1000_CTRL_EXT_IRCA           0x00000001
/*                              */
/*                          */
#define E1000_CTRL_EXT_DRV_LOAD       0x10000000
/*                                 */
/*                                        */
/*                                              */
/*                                               */
#define E1000_CTRL_EXT_PBA_CLR        0x80000000 /*           */
#define E1000_I2CCMD_REG_ADDR_SHIFT   16
#define E1000_I2CCMD_PHY_ADDR_SHIFT   24
#define E1000_I2CCMD_OPCODE_READ      0x08000000
#define E1000_I2CCMD_OPCODE_WRITE     0x00000000
#define E1000_I2CCMD_READY            0x20000000
#define E1000_I2CCMD_ERROR            0x80000000
#define E1000_MAX_SGMII_PHY_REG_ADDR  255
#define E1000_I2CCMD_PHY_TIMEOUT      200
#define E1000_IVAR_VALID              0x80
#define E1000_GPIE_NSICR              0x00000001
#define E1000_GPIE_MSIX_MODE          0x00000010
#define E1000_GPIE_EIAME              0x40000000
#define E1000_GPIE_PBA                0x80000000

/*                                    */
#define E1000_RXD_STAT_DD       0x01    /*                 */
#define E1000_RXD_STAT_EOP      0x02    /*               */
#define E1000_RXD_STAT_IXSM     0x04    /*                 */
#define E1000_RXD_STAT_VP       0x08    /*                  */
#define E1000_RXD_STAT_UDPCS    0x10    /*                     */
#define E1000_RXD_STAT_TCPCS    0x20    /*                     */
#define E1000_RXD_STAT_TS       0x10000 /*                      */

#define E1000_RXDEXT_STATERR_LB    0x00040000
#define E1000_RXDEXT_STATERR_CE    0x01000000
#define E1000_RXDEXT_STATERR_SE    0x02000000
#define E1000_RXDEXT_STATERR_SEQ   0x04000000
#define E1000_RXDEXT_STATERR_CXE   0x10000000
#define E1000_RXDEXT_STATERR_TCPE  0x20000000
#define E1000_RXDEXT_STATERR_IPE   0x40000000
#define E1000_RXDEXT_STATERR_RXE   0x80000000

/*                                                          */
#define E1000_RXDEXT_ERR_FRAME_ERR_MASK ( \
    E1000_RXDEXT_STATERR_CE  |            \
    E1000_RXDEXT_STATERR_SE  |            \
    E1000_RXDEXT_STATERR_SEQ |            \
    E1000_RXDEXT_STATERR_CXE |            \
    E1000_RXDEXT_STATERR_RXE)

#define E1000_MRQC_RSS_FIELD_IPV4_TCP          0x00010000
#define E1000_MRQC_RSS_FIELD_IPV4              0x00020000
#define E1000_MRQC_RSS_FIELD_IPV6_TCP_EX       0x00040000
#define E1000_MRQC_RSS_FIELD_IPV6              0x00100000
#define E1000_MRQC_RSS_FIELD_IPV6_TCP          0x00200000


/*                    */
#define E1000_MANC_SMBUS_EN      0x00000001 /*                    */
#define E1000_MANC_ASF_EN        0x00000002 /*                  */
#define E1000_MANC_EN_BMC2OS     0x10000000 /*                         */
/*                                     */
#define E1000_MANC_RCV_TCO_EN    0x00020000 /*                             */
#define E1000_MANC_BLK_PHY_RST_ON_IDE   0x00040000 /*                  */
/*                              */
#define E1000_MANC_EN_MAC_ADDR_FILTER   0x00100000

/*                 */
#define E1000_RCTL_EN             0x00000002    /*        */
#define E1000_RCTL_SBP            0x00000004    /*                  */
#define E1000_RCTL_UPE            0x00000008    /*                            */
#define E1000_RCTL_MPE            0x00000010    /*                            */
#define E1000_RCTL_LPE            0x00000020    /*                    */
#define E1000_RCTL_LBM_MAC        0x00000040    /*                   */
#define E1000_RCTL_LBM_TCVR       0x000000C0    /*                    */
#define E1000_RCTL_RDMTS_HALF     0x00000000    /*                            */
#define E1000_RCTL_MO_SHIFT       12            /*                        */
#define E1000_RCTL_BAM            0x00008000    /*                  */
#define E1000_RCTL_SZ_512         0x00020000    /*                    */
#define E1000_RCTL_SZ_256         0x00030000    /*                    */
#define E1000_RCTL_VFE            0x00040000    /*                    */
#define E1000_RCTL_CFIEN          0x00080000    /*                       */
#define E1000_RCTL_DPF            0x00400000    /*                      */
#define E1000_RCTL_PMCF           0x00800000    /*                         */
#define E1000_RCTL_SECRC          0x04000000    /*                    */

/*                                                   
         
                                                                        
                                               
                                                                         
                                               
                                                                         
                                               
                                                                          
                                              
                                            
                                             
                                             
                                          
 */

#define E1000_PSRCTL_BSIZE0_MASK   0x0000007F
#define E1000_PSRCTL_BSIZE1_MASK   0x00003F00
#define E1000_PSRCTL_BSIZE2_MASK   0x003F0000
#define E1000_PSRCTL_BSIZE3_MASK   0x3F000000

#define E1000_PSRCTL_BSIZE0_SHIFT  7            /*                 */
#define E1000_PSRCTL_BSIZE1_SHIFT  2            /*                 */
#define E1000_PSRCTL_BSIZE2_SHIFT  6            /*                */
#define E1000_PSRCTL_BSIZE3_SHIFT 14            /*                 */

/*                       */
#define E1000_SWFW_EEP_SM   0x1
#define E1000_SWFW_PHY0_SM  0x2
#define E1000_SWFW_PHY1_SM  0x4
#define E1000_SWFW_PHY2_SM  0x20
#define E1000_SWFW_PHY3_SM  0x40

/*                    */
/*                */
#define E1000_CTRL_FD       0x00000001  /*                            */
#define E1000_CTRL_GIO_MASTER_DISABLE 0x00000004 /*                           */
#define E1000_CTRL_LRST     0x00000008  /*                              */
#define E1000_CTRL_ASDE     0x00000020  /*                          */
#define E1000_CTRL_SLU      0x00000040  /*                          */
#define E1000_CTRL_ILOS     0x00000080  /*                       */
#define E1000_CTRL_SPD_SEL  0x00000300  /*                   */
#define E1000_CTRL_SPD_100  0x00000100  /*             */
#define E1000_CTRL_SPD_1000 0x00000200  /*           */
#define E1000_CTRL_FRCSPD   0x00000800  /*             */
#define E1000_CTRL_FRCDPX   0x00001000  /*              */
/*                                                      */
/*                                            */
/*                                                         */
#define E1000_CTRL_SWDPIN0  0x00040000  /*                */
#define E1000_CTRL_SWDPIN1  0x00080000  /*                */
#define E1000_CTRL_SWDPIO0  0x00400000  /*                          */
#define E1000_CTRL_RST      0x04000000  /*              */
#define E1000_CTRL_RFCE     0x08000000  /*                             */
#define E1000_CTRL_TFCE     0x10000000  /*                              */
#define E1000_CTRL_VME      0x40000000  /*                       */
#define E1000_CTRL_PHY_RST  0x80000000  /*           */
/*                                               */
#define E1000_CTRL_I2C_ENA  0x02000000  /*            */

/*                                                                      
                                                   
 */

#define E1000_CONNSW_ENRGSRC             0x4
#define E1000_PCS_CFG_PCS_EN             8
#define E1000_PCS_LCTL_FLV_LINK_UP       1
#define E1000_PCS_LCTL_FSV_100           2
#define E1000_PCS_LCTL_FSV_1000          4
#define E1000_PCS_LCTL_FDV_FULL          8
#define E1000_PCS_LCTL_FSD               0x10
#define E1000_PCS_LCTL_FORCE_LINK        0x20
#define E1000_PCS_LCTL_FORCE_FCTRL       0x80
#define E1000_PCS_LCTL_AN_ENABLE         0x10000
#define E1000_PCS_LCTL_AN_RESTART        0x20000
#define E1000_PCS_LCTL_AN_TIMEOUT        0x40000
#define E1000_ENABLE_SERDES_LOOPBACK     0x0410

#define E1000_PCS_LSTS_LINK_OK           1
#define E1000_PCS_LSTS_SPEED_100         2
#define E1000_PCS_LSTS_SPEED_1000        4
#define E1000_PCS_LSTS_DUPLEX_FULL       8
#define E1000_PCS_LSTS_SYNK_OK           0x10

/*               */
#define E1000_STATUS_FD         0x00000001      /*                           */
#define E1000_STATUS_LU         0x00000002      /*                     */
#define E1000_STATUS_FUNC_MASK  0x0000000C      /*                   */
#define E1000_STATUS_FUNC_SHIFT 2
#define E1000_STATUS_FUNC_1     0x00000004      /*            */
#define E1000_STATUS_TXOFF      0x00000010      /*                     */
#define E1000_STATUS_SPEED_100  0x00000040      /*               */
#define E1000_STATUS_SPEED_1000 0x00000080      /*                */
/*                                                  */
/*                            */
#define E1000_STATUS_GIO_MASTER_ENABLE 0x00080000
/*                                      */

#define E1000_STATUS_2P5_SKU		0x00001000 /*                         */
#define E1000_STATUS_2P5_SKU_OVER	0x00002000 /*                        */
/*                                                         */

#define SPEED_10    10
#define SPEED_100   100
#define SPEED_1000  1000
#define SPEED_2500  2500
#define HALF_DUPLEX 1
#define FULL_DUPLEX 2


#define ADVERTISE_10_HALF                 0x0001
#define ADVERTISE_10_FULL                 0x0002
#define ADVERTISE_100_HALF                0x0004
#define ADVERTISE_100_FULL                0x0008
#define ADVERTISE_1000_HALF               0x0010 /*                    */
#define ADVERTISE_1000_FULL               0x0020

/*                                              */
#define E1000_ALL_SPEED_DUPLEX (ADVERTISE_10_HALF  |  ADVERTISE_10_FULL | \
				ADVERTISE_100_HALF |  ADVERTISE_100_FULL | \
						      ADVERTISE_1000_FULL)
#define E1000_ALL_NOT_GIG      (ADVERTISE_10_HALF  |  ADVERTISE_10_FULL | \
				ADVERTISE_100_HALF |  ADVERTISE_100_FULL)
#define E1000_ALL_100_SPEED    (ADVERTISE_100_HALF |  ADVERTISE_100_FULL)
#define E1000_ALL_10_SPEED     (ADVERTISE_10_HALF  |  ADVERTISE_10_FULL)
#define E1000_ALL_FULL_DUPLEX  (ADVERTISE_10_FULL  |  ADVERTISE_100_FULL | \
						      ADVERTISE_1000_FULL)
#define E1000_ALL_HALF_DUPLEX  (ADVERTISE_10_HALF  |  ADVERTISE_100_HALF)

#define AUTONEG_ADVERTISE_SPEED_DEFAULT   E1000_ALL_SPEED_DUPLEX

/*             */
#define E1000_LEDCTL_LED0_MODE_SHIFT      0
#define E1000_LEDCTL_LED0_BLINK           0x00000080

#define E1000_LEDCTL_MODE_LED_ON        0xE
#define E1000_LEDCTL_MODE_LED_OFF       0xF

/*                                     */
#define E1000_TXD_POPTS_IXSM 0x01       /*                    */
#define E1000_TXD_POPTS_TXSM 0x02       /*                         */
#define E1000_TXD_CMD_EOP    0x01000000 /*               */
#define E1000_TXD_CMD_IFCS   0x02000000 /*                           */
#define E1000_TXD_CMD_RS     0x08000000 /*               */
#define E1000_TXD_CMD_DEXT   0x20000000 /*                                   */
#define E1000_TXD_STAT_DD    0x00000001 /*                 */
/*                                             */

/*                  */
#define E1000_TCTL_EN     0x00000002    /*           */
#define E1000_TCTL_PSP    0x00000008    /*                   */
#define E1000_TCTL_CT     0x00000ff0    /*                     */
#define E1000_TCTL_COLD   0x003ff000    /*                    */
#define E1000_TCTL_RTLC   0x01000000    /*                               */

/*                                */
#define E1000_DMACR_DMACWT_MASK         0x00003FFF /*               
                        */
#define E1000_DMACR_DMACTHR_MASK        0x00FF0000 /*                       
                   */
#define E1000_DMACR_DMACTHR_SHIFT       16
#define E1000_DMACR_DMAC_LX_MASK        0x30000000 /*                
                      */
#define E1000_DMACR_DMAC_LX_SHIFT       28
#define E1000_DMACR_DMAC_EN             0x80000000 /*                       */
/*                                          */
#define E1000_DMACR_DC_BMC2OSW_EN	0x00008000

#define E1000_DMCTXTH_DMCTTHR_MASK      0x00000FFF /*                        
                   */

#define E1000_DMCTLX_TTLX_MASK          0x00000FFF /*                    */

#define E1000_DMCRTRH_UTRESH_MASK       0x0007FFFF /*                     
                   */
#define E1000_DMCRTRH_LRPRCW            0x80000000 /*                   
                        */

#define E1000_DMCCNT_CCOUNT_MASK        0x01FFFFFF /*                     
                     */

#define E1000_FCRTC_RTH_COAL_MASK       0x0003FFF0 /*                        
                  */
#define E1000_FCRTC_RTH_COAL_SHIFT      4
#define E1000_PCIEMISC_LX_DECISION      0x00000080 /*                   */

/*                        */
#define E1000_RXPBS_CFG_TS_EN           0x80000000

/*                */
#define E1000_SCTL_DISABLE_SERDES_LOOPBACK 0x0400

/*                          */
#define E1000_RXCSUM_IPOFL     0x00000100   /*                       */
#define E1000_RXCSUM_TUOFL     0x00000200   /*                            */
#define E1000_RXCSUM_CRCOFL    0x00000800   /*                      */
#define E1000_RXCSUM_PCSD      0x00002000   /*                          */

/*                      */
#define E1000_RFCTL_LEF        0x00040000

/*                                            */
#define E1000_COLLISION_THRESHOLD       15
#define E1000_CT_SHIFT                  4
#define E1000_COLLISION_DISTANCE        63
#define E1000_COLD_SHIFT                12

/*                        */
#define ETHERNET_IEEE_VLAN_TYPE 0x8100  /*                */

#define MAX_JUMBO_FRAME_SIZE    0x3F00

/*               */
#define E1000_PBA_34K 0x0022
#define E1000_PBA_64K 0x0040    /*      */

/*                       */
#define E1000_SWSM_SMBI         0x00000001 /*                      */
#define E1000_SWSM_SWESMBI      0x00000002 /*                  */

/*                      */
#define E1000_ICR_TXDW          0x00000001 /*                            */
#define E1000_ICR_LSC           0x00000004 /*                    */
#define E1000_ICR_RXSEQ         0x00000008 /*                   */
#define E1000_ICR_RXDMT0        0x00000010 /*                            */
#define E1000_ICR_RXT0          0x00000080 /*                        */
#define E1000_ICR_VMMB          0x00000100 /*             */
#define E1000_ICR_TS            0x00080000 /*                     */
#define E1000_ICR_DRSTA         0x40000000 /*                       */
/*                                                             */
#define E1000_ICR_INT_ASSERTED  0x80000000
/*                                             */
#define E1000_ICR_DOUTSYNC      0x10000000 /*                     */

/*                               */
#define E1000_EICR_RX_QUEUE0    0x00000001 /*                      */
#define E1000_EICR_RX_QUEUE1    0x00000002 /*                      */
#define E1000_EICR_RX_QUEUE2    0x00000004 /*                      */
#define E1000_EICR_RX_QUEUE3    0x00000008 /*                      */
#define E1000_EICR_TX_QUEUE0    0x00000100 /*                      */
#define E1000_EICR_TX_QUEUE1    0x00000200 /*                      */
#define E1000_EICR_TX_QUEUE2    0x00000400 /*                      */
#define E1000_EICR_TX_QUEUE3    0x00000800 /*                      */
#define E1000_EICR_OTHER        0x80000000 /*                        */
/*           */

/*                                                         
                                                    
                                                 
                                                
                                                                 
                                      
                                  
 */
#define IMS_ENABLE_MASK ( \
    E1000_IMS_RXT0   |    \
    E1000_IMS_TXDW   |    \
    E1000_IMS_RXDMT0 |    \
    E1000_IMS_RXSEQ  |    \
    E1000_IMS_LSC    |    \
    E1000_IMS_DOUTSYNC)

/*                    */
#define E1000_IMS_TXDW      E1000_ICR_TXDW      /*                            */
#define E1000_IMS_LSC       E1000_ICR_LSC       /*                    */
#define E1000_IMS_VMMB      E1000_ICR_VMMB      /*                   */
#define E1000_IMS_TS        E1000_ICR_TS        /*                     */
#define E1000_IMS_RXSEQ     E1000_ICR_RXSEQ     /*                   */
#define E1000_IMS_RXDMT0    E1000_ICR_RXDMT0    /*                        */
#define E1000_IMS_RXT0      E1000_ICR_RXT0      /*               */
#define E1000_IMS_DRSTA     E1000_ICR_DRSTA     /*                       */
#define E1000_IMS_DOUTSYNC  E1000_ICR_DOUTSYNC /*                     */

/*                             */
#define E1000_EIMS_OTHER        E1000_EICR_OTHER   /*                        */

/*                     */
#define E1000_ICS_LSC       E1000_ICR_LSC       /*                    */
#define E1000_ICS_RXDMT0    E1000_ICR_RXDMT0    /*                        */
#define E1000_ICS_DRSTA     E1000_ICR_DRSTA     /*                      */

/*                              */
/*                                                 */
#define E1000_EITR_CNT_IGNR     0x80000000 /*                               */


/*                             */
/*                                                           */

/*                        */
#define FLOW_CONTROL_ADDRESS_LOW  0x00C28001
#define FLOW_CONTROL_ADDRESS_HIGH 0x00000100
#define FLOW_CONTROL_TYPE         0x8808

/*                      */
#define E1000_TXCW_ASM_DIR	0x00000100 /*                           */
#define E1000_TXCW_PAUSE	0x00000080 /*                        */

/*                         */
#define VLAN_TAG_SIZE              4    /*                         */
#define E1000_VLAN_FILTER_TBL_SIZE 128  /*                               */

/*                 */
/*                                                                       
                                                                         
                                                                         
                                                              
                                                                      
 */
#define E1000_RAH_AV  0x80000000        /*                          */
#define E1000_RAL_MAC_ADDR_LEN 4
#define E1000_RAH_MAC_ADDR_LEN 2
#define E1000_RAH_POOL_MASK 0x03FC0000
#define E1000_RAH_POOL_1 0x00040000

/*             */
#define E1000_SUCCESS      0
#define E1000_ERR_NVM      1
#define E1000_ERR_PHY      2
#define E1000_ERR_CONFIG   3
#define E1000_ERR_PARAM    4
#define E1000_ERR_MAC_INIT 5
#define E1000_ERR_RESET   9
#define E1000_ERR_MASTER_REQUESTS_PENDING 10
#define E1000_BLK_PHY_RESET   12
#define E1000_ERR_SWFW_SYNC 13
#define E1000_NOT_IMPLEMENTED 14
#define E1000_ERR_MBX      15
#define E1000_ERR_INVALID_ARGUMENT  16
#define E1000_ERR_NO_SPACE          17
#define E1000_ERR_NVM_PBA_SECTION   18
#define E1000_ERR_INVM_VALUE_NOT_FOUND	19
#define E1000_ERR_I2C               20

/*                                                                 */
#define COPPER_LINK_UP_LIMIT              10
#define PHY_AUTO_NEG_LIMIT                45
#define PHY_FORCE_LIMIT                   20
/*                                                                   */
#define MASTER_DISABLE_TIMEOUT      800
/*                                                                           */
#define PHY_CFG_TIMEOUT             100
/*                                                                */
/*                                                                */
#define AUTO_READ_DONE_TIMEOUT      10

/*              */
#define E1000_FCRTL_XONE 0x80000000     /*                               */

#define E1000_TSYNCTXCTL_VALID    0x00000001 /*                    */
#define E1000_TSYNCTXCTL_ENABLED  0x00000010 /*                         */

#define E1000_TSYNCRXCTL_VALID      0x00000001 /*                    */
#define E1000_TSYNCRXCTL_TYPE_MASK  0x0000000E /*              */
#define E1000_TSYNCRXCTL_TYPE_L2_V2       0x00
#define E1000_TSYNCRXCTL_TYPE_L4_V1       0x02
#define E1000_TSYNCRXCTL_TYPE_L2_L4_V2    0x04
#define E1000_TSYNCRXCTL_TYPE_ALL         0x08
#define E1000_TSYNCRXCTL_TYPE_EVENT_V2    0x0A
#define E1000_TSYNCRXCTL_ENABLED    0x00000010 /*                         */

#define E1000_TSYNCRXCFG_PTP_V1_CTRLT_MASK   0x000000FF
#define E1000_TSYNCRXCFG_PTP_V1_SYNC_MESSAGE       0x00
#define E1000_TSYNCRXCFG_PTP_V1_DELAY_REQ_MESSAGE  0x01
#define E1000_TSYNCRXCFG_PTP_V1_FOLLOWUP_MESSAGE   0x02
#define E1000_TSYNCRXCFG_PTP_V1_DELAY_RESP_MESSAGE 0x03
#define E1000_TSYNCRXCFG_PTP_V1_MANAGEMENT_MESSAGE 0x04

#define E1000_TSYNCRXCFG_PTP_V2_MSGID_MASK               0x00000F00
#define E1000_TSYNCRXCFG_PTP_V2_SYNC_MESSAGE                 0x0000
#define E1000_TSYNCRXCFG_PTP_V2_DELAY_REQ_MESSAGE            0x0100
#define E1000_TSYNCRXCFG_PTP_V2_PATH_DELAY_REQ_MESSAGE       0x0200
#define E1000_TSYNCRXCFG_PTP_V2_PATH_DELAY_RESP_MESSAGE      0x0300
#define E1000_TSYNCRXCFG_PTP_V2_FOLLOWUP_MESSAGE             0x0800
#define E1000_TSYNCRXCFG_PTP_V2_DELAY_RESP_MESSAGE           0x0900
#define E1000_TSYNCRXCFG_PTP_V2_PATH_DELAY_FOLLOWUP_MESSAGE  0x0A00
#define E1000_TSYNCRXCFG_PTP_V2_ANNOUNCE_MESSAGE             0x0B00
#define E1000_TSYNCRXCFG_PTP_V2_SIGNALLING_MESSAGE           0x0C00
#define E1000_TSYNCRXCFG_PTP_V2_MANAGEMENT_MESSAGE           0x0D00

#define E1000_TIMINCA_16NS_SHIFT 24

#define E1000_TSICR_TXTS 0x00000002
#define E1000_TSIM_TXTS 0x00000002

#define E1000_MDICNFG_EXT_MDIO    0x80000000      /*                         */
#define E1000_MDICNFG_COM_MDIO    0x40000000      /*                     */
#define E1000_MDICNFG_PHY_MASK    0x03E00000
#define E1000_MDICNFG_PHY_SHIFT   21

/*                     */
#define E1000_GCR_CMPL_TMOUT_MASK       0x0000F000
#define E1000_GCR_CMPL_TMOUT_10ms       0x00001000
#define E1000_GCR_CMPL_TMOUT_RESEND     0x00010000
#define E1000_GCR_CAP_VER2              0x00040000

/*                                         */
#define E1000_MPHY_ADDR_CTL          0x0024 /*                               */
#define E1000_MPHY_ADDR_CTL_OFFSET_MASK 0xFFFF0000
#define E1000_MPHY_DATA                 0x0E10 /*                    */

/*                       */
#define E1000_MPHY_PCS_CLK_REG_OFFSET  0x0004 /*                             */
/*                                             */
#define E1000_MPHY_PCS_CLK_REG_DIGINELBEN 0x10

#define E1000_PCS_LCTL_FORCE_FCTRL	0x80
#define E1000_PCS_LSTS_AN_COMPLETE	0x10000

/*                      */
#define MII_CR_FULL_DUPLEX      0x0100  /*                        */
#define MII_CR_RESTART_AUTO_NEG 0x0200  /*                          */
#define MII_CR_POWER_DOWN       0x0800  /*            */
#define MII_CR_AUTO_NEG_EN      0x1000  /*                 */
#define MII_CR_LOOPBACK         0x4000  /*                          */
#define MII_CR_RESET            0x8000  /*                           */
#define MII_CR_SPEED_1000       0x0040
#define MII_CR_SPEED_100        0x2000
#define MII_CR_SPEED_10         0x0000

/*                     */
#define MII_SR_LINK_STATUS       0x0004 /*                      */
#define MII_SR_AUTONEG_COMPLETE  0x0020 /*                   */

/*                                */
#define NWAY_AR_10T_HD_CAPS      0x0020   /*                           */
#define NWAY_AR_10T_FD_CAPS      0x0040   /*                           */
#define NWAY_AR_100TX_HD_CAPS    0x0080   /*                           */
#define NWAY_AR_100TX_FD_CAPS    0x0100   /*                           */
#define NWAY_AR_PAUSE            0x0400   /*                         */
#define NWAY_AR_ASM_DIR          0x0800   /*                                */

/*                                           */
#define NWAY_LPAR_PAUSE          0x0400 /*                            */
#define NWAY_LPAR_ASM_DIR        0x0800 /*                                   */

/*                            */

/*                             */
#define CR_1000T_HD_CAPS         0x0100 /*                               */
#define CR_1000T_FD_CAPS         0x0200 /*                                */
#define CR_1000T_MS_VALUE        0x0800 /*                           */
					/*                          */
#define CR_1000T_MS_ENABLE       0x1000 /*                                    */
					/*                                 */

/*                            */
#define SR_1000T_REMOTE_RX_STATUS 0x1000 /*                    */
#define SR_1000T_LOCAL_RX_STATUS  0x2000 /*                   */


/*                                       */
/*                               */
#define PHY_CONTROL      0x00 /*                  */
#define PHY_STATUS       0x01 /*                 */
#define PHY_ID1          0x02 /*                     */
#define PHY_ID2          0x03 /*                     */
#define PHY_AUTONEG_ADV  0x04 /*                       */
#define PHY_LP_ABILITY   0x05 /*                                  */
#define PHY_1000T_CTRL   0x09 /*                        */
#define PHY_1000T_STATUS 0x0A /*                       */

/*             */
#define E1000_EECD_SK        0x00000001 /*           */
#define E1000_EECD_CS        0x00000002 /*                 */
#define E1000_EECD_DI        0x00000004 /*             */
#define E1000_EECD_DO        0x00000008 /*              */
#define E1000_EECD_REQ       0x00000040 /*                    */
#define E1000_EECD_GNT       0x00000080 /*                  */
#define E1000_EECD_PRES      0x00000100 /*             */
/*                                                    */
#define E1000_EECD_ADDR_BITS 0x00000400
#define E1000_NVM_GRANT_ATTEMPTS   1000 /*                              */
#define E1000_EECD_AUTO_RD          0x00000200  /*                    */
#define E1000_EECD_SIZE_EX_MASK     0x00007800  /*          */
#define E1000_EECD_SIZE_EX_SHIFT     11
#define E1000_EECD_FLUPD_I210		0x00800000 /*              */
#define E1000_EECD_FLUDONE_I210		0x04000000 /*                  */
#define E1000_FLUDONE_ATTEMPTS		20000
#define E1000_EERD_EEWR_MAX_COUNT	512 /*                          */
#define E1000_I210_FIFO_SEL_RX		0x00
#define E1000_I210_FIFO_SEL_TX_QAV(_i)	(0x02 + (_i))
#define E1000_I210_FIFO_SEL_TX_LEGACY	E1000_I210_FIFO_SEL_TX_QAV(0)
#define E1000_I210_FIFO_SEL_BMC2OS_TX	0x06
#define E1000_I210_FIFO_SEL_BMC2OS_RX	0x01
#define E1000_EECD_FLUPD_I210		0x00800000 /*              */
#define E1000_EECD_FLUDONE_I210		0x04000000 /*                  */
#define E1000_FLUDONE_ATTEMPTS		20000
#define E1000_EERD_EEWR_MAX_COUNT	512 /*                          */
#define E1000_I210_FIFO_SEL_RX		0x00
#define E1000_I210_FIFO_SEL_TX_QAV(_i)	(0x02 + (_i))
#define E1000_I210_FIFO_SEL_TX_LEGACY	E1000_I210_FIFO_SEL_TX_QAV(0)
#define E1000_I210_FIFO_SEL_BMC2OS_TX	0x06
#define E1000_I210_FIFO_SEL_BMC2OS_RX	0x01


/*                                            */
#define E1000_NVM_RW_REG_DATA   16
#define E1000_NVM_RW_REG_DONE   2    /*                               */
#define E1000_NVM_RW_REG_START  1    /*                 */
#define E1000_NVM_RW_ADDR_SHIFT 2    /*                           */
#define E1000_NVM_POLL_READ     0    /*                                    */

/*                  */
#define NVM_COMPAT                 0x0003
#define NVM_ID_LED_SETTINGS        0x0004 /*                         */
#define NVM_VERSION                0x0005
#define NVM_INIT_CONTROL2_REG      0x000F
#define NVM_INIT_CONTROL3_PORT_B   0x0014
#define NVM_INIT_CONTROL3_PORT_A   0x0024
#define NVM_ALT_MAC_ADDR_PTR       0x0037
#define NVM_CHECKSUM_REG           0x003F
#define NVM_COMPATIBILITY_REG_3    0x0003
#define NVM_COMPATIBILITY_BIT_MASK 0x8000
#define NVM_MAC_ADDR               0x0000
#define NVM_SUB_DEV_ID             0x000B
#define NVM_SUB_VEN_ID             0x000C
#define NVM_DEV_ID                 0x000D
#define NVM_VEN_ID                 0x000E
#define NVM_INIT_CTRL_2            0x000F
#define NVM_INIT_CTRL_4            0x0013
#define NVM_LED_1_CFG              0x001C
#define NVM_LED_0_2_CFG            0x001F

/*                     */
#define NVM_ETRACK_WORD            0x0042
#define NVM_COMB_VER_OFF           0x0083
#define NVM_COMB_VER_PTR           0x003d
#define NVM_MAJOR_MASK             0xF000
#define NVM_MINOR_MASK             0x0FF0
#define NVM_BUILD_MASK             0x000F
#define NVM_COMB_VER_MASK          0x00FF
#define NVM_MAJOR_SHIFT                12
#define NVM_MINOR_SHIFT                 4
#define NVM_COMB_VER_SHFT               8
#define NVM_VER_INVALID            0xFFFF
#define NVM_ETRACK_SHIFT               16
#define NVM_ETS_CFG			0x003E
#define NVM_ETS_LTHRES_DELTA_MASK	0x07C0
#define NVM_ETS_LTHRES_DELTA_SHIFT	6
#define NVM_ETS_TYPE_MASK		0x0038
#define NVM_ETS_TYPE_SHIFT		3
#define NVM_ETS_TYPE_EMC		0x000
#define NVM_ETS_NUM_SENSORS_MASK	0x0007
#define NVM_ETS_DATA_LOC_MASK		0x3C00
#define NVM_ETS_DATA_LOC_SHIFT		10
#define NVM_ETS_DATA_INDEX_MASK		0x0300
#define NVM_ETS_DATA_INDEX_SHIFT	8
#define NVM_ETS_DATA_HTHRESH_MASK	0x00FF

#define E1000_NVM_CFG_DONE_PORT_0  0x040000 /*                       */
#define E1000_NVM_CFG_DONE_PORT_1  0x080000 /*                    */
#define E1000_NVM_CFG_DONE_PORT_2  0x100000 /*                   */
#define E1000_NVM_CFG_DONE_PORT_3  0x200000 /*                    */

#define NVM_82580_LAN_FUNC_OFFSET(a) (a ? (0x40 + (0x40 * a)) : 0)

/*                                              */
#define NVM_WORD24_COM_MDIO         0x0008 /*                       */
#define NVM_WORD24_EXT_MDIO         0x0004 /*                               */

/*                                              */
#define NVM_WORD0F_PAUSE_MASK       0x3000
#define NVM_WORD0F_ASM_DIR          0x2000

/*                                              */

/*                                           */
#define E1000_PBANUM_LENGTH         11

/*                                                                        */
#define NVM_SUM                    0xBABA

#define NVM_PBA_OFFSET_0           8
#define NVM_PBA_OFFSET_1           9
#define NVM_RESERVED_WORD		0xFFFF
#define NVM_PBA_PTR_GUARD          0xFAFA
#define NVM_WORD_SIZE_BASE_SHIFT   6

/*                          */

/*                    */
#define NVM_MAX_RETRY_SPI          5000 /*                                 */
#define NVM_WRITE_OPCODE_SPI       0x02 /*                  */
#define NVM_READ_OPCODE_SPI        0x03 /*                 */
#define NVM_A8_OPCODE_SPI          0x08 /*                              */
#define NVM_WREN_OPCODE_SPI        0x06 /*                            */
#define NVM_RDSR_OPCODE_SPI        0x05 /*                          */

/*                         */
#define NVM_STATUS_RDY_SPI         0x01

/*                                      */
#define ID_LED_RESERVED_0000 0x0000
#define ID_LED_RESERVED_FFFF 0xFFFF
#define ID_LED_DEFAULT       ((ID_LED_OFF1_ON2  << 12) | \
			      (ID_LED_OFF1_OFF2 <<  8) | \
			      (ID_LED_DEF1_DEF2 <<  4) | \
			      (ID_LED_DEF1_DEF2))
#define ID_LED_DEF1_DEF2     0x1
#define ID_LED_DEF1_ON2      0x2
#define ID_LED_DEF1_OFF2     0x3
#define ID_LED_ON1_DEF2      0x4
#define ID_LED_ON1_ON2       0x5
#define ID_LED_ON1_OFF2      0x6
#define ID_LED_OFF1_DEF2     0x7
#define ID_LED_OFF1_ON2      0x8
#define ID_LED_OFF1_OFF2     0x9

#define IGP_ACTIVITY_LED_MASK   0xFFFFF0FF
#define IGP_ACTIVITY_LED_ENABLE 0x0300
#define IGP_LED3_MODE           0x07000000

/*                               */
#define PCIE_DEVICE_CONTROL2         0x28
#define PCIE_DEVICE_CONTROL2_16ms    0x0005

#define PHY_REVISION_MASK      0xFFFFFFF0
#define MAX_PHY_REG_ADDRESS    0x1F  /*                            */
#define MAX_PHY_MULTI_PAGE_REG 0xF

/*                                    */
/*               
               
 */
#define M88E1111_I_PHY_ID    0x01410CC0
#define M88E1112_E_PHY_ID    0x01410C90
#define I347AT4_E_PHY_ID     0x01410DC0
#define IGP03E1000_E_PHY_ID  0x02A80390
#define I82580_I_PHY_ID      0x015403A0
#define I350_I_PHY_ID        0x015403B0
#define M88_VENDOR           0x0141
#define I210_I_PHY_ID        0x01410C00
#define M88E1545_E_PHY_ID    0x01410EA0

/*                             */
#define M88E1000_PHY_SPEC_CTRL     0x10  /*                               */
#define M88E1000_PHY_SPEC_STATUS   0x11  /*                              */
#define M88E1000_EXT_PHY_SPEC_CTRL 0x14  /*                               */

#define M88E1000_PHY_PAGE_SELECT   0x1D  /*                                */
#define M88E1000_PHY_GEN_CONTROL   0x1E  /*                               */

/*                                        */
#define M88E1000_PSCR_POLARITY_REVERSAL 0x0002 /*                             */
/*                                 */
#define M88E1000_PSCR_MDI_MANUAL_MODE  0x0000  /*                             */
					       /*                          */
#define M88E1000_PSCR_MDIX_MANUAL_MODE 0x0020  /*                           */
/*                                                           */
#define M88E1000_PSCR_AUTO_X_1000T     0x0040
/*                                   */
#define M88E1000_PSCR_AUTO_X_MODE      0x0060
/*                                                                 
                                 
 */
/*                                                                */
#define M88E1000_PSCR_ASSERT_CRS_ON_TX     0x0800 /*                          */

/*                                       */
#define M88E1000_PSSR_REV_POLARITY       0x0002 /*                     */
#define M88E1000_PSSR_DOWNSHIFT          0x0020 /*               */
#define M88E1000_PSSR_MDIX               0x0040 /*               */
/*         
             
              
               
            
 */
#define M88E1000_PSSR_CABLE_LENGTH       0x0380
#define M88E1000_PSSR_SPEED              0xC000 /*                   */
#define M88E1000_PSSR_1000MBS            0x8000 /*            */

#define M88E1000_PSSR_CABLE_LENGTH_SHIFT 7

/*                                                 */
/*                              
                                  
                             
                           
 */
/*                                                                           
                 
 */
#define M88E1000_EPSCR_MASTER_DOWNSHIFT_MASK 0x0C00
#define M88E1000_EPSCR_MASTER_DOWNSHIFT_1X   0x0000
/*                                                                           
                
 */
#define M88E1000_EPSCR_SLAVE_DOWNSHIFT_MASK  0x0300
#define M88E1000_EPSCR_SLAVE_DOWNSHIFT_1X    0x0100
#define M88E1000_EPSCR_TX_CLK_25      0x0070 /*                */

/*                          */

#define I347AT4_PCDL                   0x10 /*                              */
#define I347AT4_PCDC                   0x15 /*                               */
#define I347AT4_PAGE_SELECT            0x16

/*                                                 */

/*                                                                            
                  
 */
#define I347AT4_PSCR_DOWNSHIFT_ENABLE 0x0800
#define I347AT4_PSCR_DOWNSHIFT_MASK   0x7000
#define I347AT4_PSCR_DOWNSHIFT_1X     0x0000
#define I347AT4_PSCR_DOWNSHIFT_2X     0x1000
#define I347AT4_PSCR_DOWNSHIFT_3X     0x2000
#define I347AT4_PSCR_DOWNSHIFT_4X     0x3000
#define I347AT4_PSCR_DOWNSHIFT_5X     0x4000
#define I347AT4_PSCR_DOWNSHIFT_6X     0x5000
#define I347AT4_PSCR_DOWNSHIFT_7X     0x6000
#define I347AT4_PSCR_DOWNSHIFT_8X     0x7000

/*                                        */
#define I347AT4_PCDC_CABLE_LENGTH_UNIT 0x0400 /*               */

/*                             */
#define M88E1112_VCT_DSP_DISTANCE       0x001A

/*                                            */
#define M88EC018_EPSCR_DOWNSHIFT_COUNTER_MASK  0x0E00
#define M88EC018_EPSCR_DOWNSHIFT_COUNTER_5X    0x0800

/*             */
#define E1000_MDIC_DATA_MASK 0x0000FFFF
#define E1000_MDIC_REG_MASK  0x001F0000
#define E1000_MDIC_REG_SHIFT 16
#define E1000_MDIC_PHY_MASK  0x03E00000
#define E1000_MDIC_PHY_SHIFT 21
#define E1000_MDIC_OP_WRITE  0x04000000
#define E1000_MDIC_OP_READ   0x08000000
#define E1000_MDIC_READY     0x10000000
#define E1000_MDIC_INT_EN    0x20000000
#define E1000_MDIC_ERROR     0x40000000
#define E1000_MDIC_DEST      0x80000000

/*                */
#define E1000_THSTAT_PWR_DOWN       0x00000001 /*                  */
#define E1000_THSTAT_LINK_THROTTLE  0x00000002 /*                           */

/*                           */
#define E1000_IPCNFG_EEE_1G_AN       0x00000008  /*                  */
#define E1000_IPCNFG_EEE_100M_AN     0x00000004  /*                    */
#define E1000_EEER_TX_LPI_EN         0x00010000  /*                   */
#define E1000_EEER_RX_LPI_EN         0x00020000  /*                   */
#define E1000_EEER_FRC_AN            0x10000000  /*                        */
#define E1000_EEER_LPI_FC            0x00040000  /*                  */
#define E1000_EEE_SU_LPI_CLK_STP     0X00800000  /*                    */
#define E1000_EEER_EEE_NEG           0x20000000  /*                     */
#define E1000_EEE_LP_ADV_ADDR_I350   0x040F      /*                      */
#define E1000_EEE_LP_ADV_DEV_I210    7           /*                   */
#define E1000_EEE_LP_ADV_ADDR_I210   61          /*                     */
#define E1000_MMDAC_FUNC_DATA        0x4000      /*                         */
#define E1000_M88E1545_PAGE_ADDR	0x16       /*                      */
#define E1000_M88E1545_EEE_CTRL_1	0x0
#define E1000_M88E1545_EEE_CTRL_1_MS	0x0001     /*                  */
#define E1000_EEE_ADV_DEV_I354		7
#define E1000_EEE_ADV_ADDR_I354		60
#define E1000_EEE_ADV_100_SUPPORTED	(1 << 1)   /*                         */
#define E1000_EEE_ADV_1000_SUPPORTED	(1 << 2)   /*                         */
#define E1000_PCS_STATUS_DEV_I354	3
#define E1000_PCS_STATUS_ADDR_I354	1
#define E1000_PCS_STATUS_TX_LPI_IND	0x0200     /*                 */
#define E1000_PCS_STATUS_RX_LPI_RCVD	0x0400
#define E1000_PCS_STATUS_TX_LPI_RCVD	0x0800

/*                */
#define E1000_GEN_CTL_READY             0x80000000
#define E1000_GEN_CTL_ADDRESS_SHIFT     8
#define E1000_GEN_POLL_TIMEOUT          640

#define E1000_VFTA_ENTRY_SHIFT               5
#define E1000_VFTA_ENTRY_MASK                0x7F
#define E1000_VFTA_ENTRY_BIT_SHIFT_MASK      0x1F

/*                                */
#define E1000_PCIEMISC_LX_DECISION      0x00000080 /*                        
                                                                  */

/*                                 */
#define E1000_RTTBCNRC_RS_ENA		0x80000000
#define E1000_RTTBCNRC_RF_DEC_MASK	0x00003FFF
#define E1000_RTTBCNRC_RF_INT_SHIFT	14
#define E1000_RTTBCNRC_RF_INT_MASK	\
	(E1000_RTTBCNRC_RF_DEC_MASK << E1000_RTTBCNRC_RF_INT_SHIFT)

#endif