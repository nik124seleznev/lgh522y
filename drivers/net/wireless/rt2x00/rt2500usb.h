/*
	Copyright (C) 2004 - 2009 Ivo van Doorn <IvDoorn@gmail.com>
	<http://rt2x00.serialmonkey.com>

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the
	Free Software Foundation, Inc.,
	59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*
                  
                                                                  
                            
 */

#ifndef RT2500USB_H
#define RT2500USB_H

/*
                   
 */
#define RF2522				0x0000
#define RF2523				0x0001
#define RF2524				0x0002
#define RF2525				0x0003
#define RF2525E				0x0005
#define RF5222				0x0010

/*
                 
 */
#define RT2570_VERSION_B		2
#define RT2570_VERSION_C		3
#define RT2570_VERSION_D		4

/*
                      
                                                          
 */
#define DEFAULT_RSSI_OFFSET		120

/*
                               
 */
#define CSR_REG_BASE			0x0400
#define CSR_REG_SIZE			0x0100
#define EEPROM_BASE			0x0000
#define EEPROM_SIZE			0x006a
#define BBP_BASE			0x0000
#define BBP_SIZE			0x0060
#define RF_BASE				0x0004
#define RF_SIZE				0x0010

/*
                       
 */
#define NUM_TX_QUEUES			2

/*
                                 
                                                      
 */

/*
                                  
 */
#define MAC_CSR0			0x0400

/*
                            
                                                   
                                                   
                                               
 */
#define MAC_CSR1			0x0402
#define MAC_CSR1_SOFT_RESET		FIELD16(0x00000001)
#define MAC_CSR1_BBP_RESET		FIELD16(0x00000002)
#define MAC_CSR1_HOST_READY		FIELD16(0x00000004)

/*
                                
 */
#define MAC_CSR2			0x0404
#define MAC_CSR2_BYTE0			FIELD16(0x00ff)
#define MAC_CSR2_BYTE1			FIELD16(0xff00)

/*
                                
 */
#define MAC_CSR3			0x0406
#define MAC_CSR3_BYTE2			FIELD16(0x00ff)
#define MAC_CSR3_BYTE3			FIELD16(0xff00)

/*
                                
 */
#define MAC_CSR4			0X0408
#define MAC_CSR4_BYTE4			FIELD16(0x00ff)
#define MAC_CSR4_BYTE5			FIELD16(0xff00)

/*
                              
 */
#define MAC_CSR5			0x040a
#define MAC_CSR5_BYTE0			FIELD16(0x00ff)
#define MAC_CSR5_BYTE1			FIELD16(0xff00)

/*
                              
 */
#define MAC_CSR6			0x040c
#define MAC_CSR6_BYTE2			FIELD16(0x00ff)
#define MAC_CSR6_BYTE3			FIELD16(0xff00)

/*
                              
 */
#define MAC_CSR7			0x040e
#define MAC_CSR7_BYTE4			FIELD16(0x00ff)
#define MAC_CSR7_BYTE5			FIELD16(0xff00)

/*
                              
 */
#define MAC_CSR8			0x0410
#define MAC_CSR8_MAX_FRAME_UNIT		FIELD16(0x0fff)

/*
                          
                           
                        
                   
                   
                          
                          
                                      
                                      
 */
#define MAC_CSR9			0x0412
#define MAC_CSR10			0x0414
#define MAC_CSR11			0x0416
#define MAC_CSR12			0x0418
#define MAC_CSR13			0x041a
#define MAC_CSR14			0x041c
#define MAC_CSR15			0x041e
#define MAC_CSR16			0x0420

/*
                                                     
                                                               
                                                          
                                       
                                     
                                        
                                      
                                                                
 */
#define MAC_CSR17			0x0422
#define MAC_CSR17_SET_STATE		FIELD16(0x0001)
#define MAC_CSR17_BBP_DESIRE_STATE	FIELD16(0x0006)
#define MAC_CSR17_RF_DESIRE_STATE	FIELD16(0x0018)
#define MAC_CSR17_BBP_CURR_STATE	FIELD16(0x0060)
#define MAC_CSR17_RF_CURR_STATE		FIELD16(0x0180)
#define MAC_CSR17_PUT_TO_SLEEP		FIELD16(0x0200)

/*
                                    
                                                                    
                                                         
                                                   
 */
#define MAC_CSR18			0x0424
#define MAC_CSR18_DELAY_AFTER_BEACON	FIELD16(0x00ff)
#define MAC_CSR18_BEACONS_BEFORE_WAKEUP	FIELD16(0x7f00)
#define MAC_CSR18_AUTO_WAKE		FIELD16(0x8000)

/*
                                    
                             
                                                        
 */
#define MAC_CSR19			0x0426
#define MAC_CSR19_VAL0			FIELD16(0x0001)
#define MAC_CSR19_VAL1			FIELD16(0x0002)
#define MAC_CSR19_VAL2			FIELD16(0x0004)
#define MAC_CSR19_VAL3			FIELD16(0x0008)
#define MAC_CSR19_VAL4			FIELD16(0x0010)
#define MAC_CSR19_VAL5			FIELD16(0x0020)
#define MAC_CSR19_VAL6			FIELD16(0x0040)
#define MAC_CSR19_VAL7			FIELD16(0x0080)
#define MAC_CSR19_DIR0			FIELD16(0x0100)
#define MAC_CSR19_DIR1			FIELD16(0x0200)
#define MAC_CSR19_DIR2			FIELD16(0x0400)
#define MAC_CSR19_DIR3			FIELD16(0x0800)
#define MAC_CSR19_DIR4			FIELD16(0x1000)
#define MAC_CSR19_DIR5			FIELD16(0x2000)
#define MAC_CSR19_DIR6			FIELD16(0x4000)
#define MAC_CSR19_DIR7			FIELD16(0x8000)

/*
                                   
                                
                               
                                                    
 */
#define MAC_CSR20			0x0428
#define MAC_CSR20_ACTIVITY		FIELD16(0x0001)
#define MAC_CSR20_LINK			FIELD16(0x0002)
#define MAC_CSR20_ACTIVITY_POLARITY	FIELD16(0x0004)

/*
                                   
                                      
                                        
 */
#define MAC_CSR21			0x042a
#define MAC_CSR21_ON_PERIOD		FIELD16(0x00ff)
#define MAC_CSR21_OFF_PERIOD		FIELD16(0xff00)

/*
                                                
 */
#define MAC_CSR22			0x042c

/*
                         
                                                      
 */

/*
                                        
 */
#define TXRX_CSR0			0x0440
#define TXRX_CSR0_ALGORITHM		FIELD16(0x0007)
#define TXRX_CSR0_IV_OFFSET		FIELD16(0x01f8)
#define TXRX_CSR0_KEY_ID		FIELD16(0x1e00)

/*
                               
                                            
                                        
                                                         
 */
#define TXRX_CSR1			0x0442
#define TXRX_CSR1_ACK_TIMEOUT		FIELD16(0x00ff)
#define TXRX_CSR1_TSF_OFFSET		FIELD16(0x7f00)
#define TXRX_CSR1_AUTO_SEQUENCE		FIELD16(0x8000)

/*
                         
                                 
                            
                                      
                                    
                                                
                                          
                                                
                                     
                                     
 */
#define TXRX_CSR2			0x0444
#define	TXRX_CSR2_DISABLE_RX		FIELD16(0x0001)
#define TXRX_CSR2_DROP_CRC		FIELD16(0x0002)
#define TXRX_CSR2_DROP_PHYSICAL		FIELD16(0x0004)
#define TXRX_CSR2_DROP_CONTROL		FIELD16(0x0008)
#define TXRX_CSR2_DROP_NOT_TO_ME	FIELD16(0x0010)
#define TXRX_CSR2_DROP_TODS		FIELD16(0x0020)
#define TXRX_CSR2_DROP_VERSION_ERROR	FIELD16(0x0040)
#define TXRX_CSR2_DROP_MULTICAST	FIELD16(0x0200)
#define TXRX_CSR2_DROP_BROADCAST	FIELD16(0x0400)

/*
                      
                            
                             
 */
#define TXRX_CSR3			0x0446
#define TXRX_CSR4			0x0448

/*
                             
 */
#define TXRX_CSR5			0x044a
#define TXRX_CSR5_BBP_ID0		FIELD16(0x007f)
#define TXRX_CSR5_BBP_ID0_VALID		FIELD16(0x0080)
#define TXRX_CSR5_BBP_ID1		FIELD16(0x7f00)
#define TXRX_CSR5_BBP_ID1_VALID		FIELD16(0x8000)

/*
                             
 */
#define TXRX_CSR6			0x044c
#define TXRX_CSR6_BBP_ID0		FIELD16(0x007f)
#define TXRX_CSR6_BBP_ID0_VALID		FIELD16(0x0080)
#define TXRX_CSR6_BBP_ID1		FIELD16(0x7f00)
#define TXRX_CSR6_BBP_ID1_VALID		FIELD16(0x8000)

/*
                              
 */
#define TXRX_CSR7			0x044e
#define TXRX_CSR7_BBP_ID0		FIELD16(0x007f)
#define TXRX_CSR7_BBP_ID0_VALID		FIELD16(0x0080)
#define TXRX_CSR7_BBP_ID1		FIELD16(0x7f00)
#define TXRX_CSR7_BBP_ID1_VALID		FIELD16(0x8000)

/*
                              
 */
#define TXRX_CSR8			0x0450
#define TXRX_CSR8_BBP_ID0		FIELD16(0x007f)
#define TXRX_CSR8_BBP_ID0_VALID		FIELD16(0x0080)
#define TXRX_CSR8_BBP_ID1		FIELD16(0x7f00)
#define TXRX_CSR8_BBP_ID1_VALID		FIELD16(0x8000)

/*
                              
 */
#define TXRX_CSR9			0x0452

/*
                                      
 */
#define TXRX_CSR10			0x0454
#define TXRX_CSR10_AUTORESPOND_PREAMBLE FIELD16(0x0004)

/*
                                         
 */
#define TXRX_CSR11			0x0456

/*
                          
 */
#define TXRX_CSR12			0x0458
#define TXRX_CSR13			0x045a
#define TXRX_CSR14			0x045c
#define TXRX_CSR15			0x045e
#define TXRX_CSR16			0x0460
#define TXRX_CSR17			0x0462

/*
                                                
 */
#define TXRX_CSR18			0x0464
#define TXRX_CSR18_OFFSET		FIELD16(0x000f)
#define TXRX_CSR18_INTERVAL		FIELD16(0xfff0)

/*
                                                
                                       
                                                                   
                                       
                                       
 */
#define TXRX_CSR19			0x0466
#define TXRX_CSR19_TSF_COUNT		FIELD16(0x0001)
#define TXRX_CSR19_TSF_SYNC		FIELD16(0x0006)
#define TXRX_CSR19_TBCN			FIELD16(0x0008)
#define TXRX_CSR19_BEACON_GEN		FIELD16(0x0010)

/*
                                                      
                            
                                      
 */
#define TXRX_CSR20			0x0468
#define TXRX_CSR20_OFFSET		FIELD16(0x1fff)
#define TXRX_CSR20_BCN_EXPECT_WINDOW	FIELD16(0xe000)

/*
             
 */
#define TXRX_CSR21			0x046a

/*
                           
  
 */

/*
                                 
                                 
                                 
                                 
                                 
                                 
                                 
                                 
 */
#define SEC_CSR0			0x0480
#define SEC_CSR1			0x0482
#define SEC_CSR2			0x0484
#define SEC_CSR3			0x0486
#define SEC_CSR4			0x0488
#define SEC_CSR5			0x048a
#define SEC_CSR6			0x048c
#define SEC_CSR7			0x048e

/*
                                 
                                 
                                  
                                  
                                  
                                  
                                  
                                  
 */
#define SEC_CSR8			0x0490
#define SEC_CSR9			0x0492
#define SEC_CSR10			0x0494
#define SEC_CSR11			0x0496
#define SEC_CSR12			0x0498
#define SEC_CSR13			0x049a
#define SEC_CSR14			0x049c
#define SEC_CSR15			0x049e

/*
                                  
                                  
                                  
                                  
                                  
                                  
                                  
                                  
 */
#define SEC_CSR16			0x04a0
#define SEC_CSR17			0x04a2
#define SEC_CSR18			0X04A4
#define SEC_CSR19			0x04a6
#define SEC_CSR20			0x04a8
#define SEC_CSR21			0x04aa
#define SEC_CSR22			0x04ac
#define SEC_CSR23			0x04ae

/*
                                  
                                  
                                  
                                  
                                  
                                  
                                  
                                  
 */
#define SEC_CSR24			0x04b0
#define SEC_CSR25			0x04b2
#define SEC_CSR26			0x04b4
#define SEC_CSR27			0x04b6
#define SEC_CSR28			0x04b8
#define SEC_CSR29			0x04ba
#define SEC_CSR30			0x04bc
#define SEC_CSR31			0x04be

#define KEY_ENTRY(__idx) \
	( SEC_CSR0 + ((__idx) * 16) )

/*
                         
 */

/*
                                         
 */
#define PHY_CSR0			0x04c0

/*
                                 
 */
#define PHY_CSR1			0x04c2

/*
                               
 */

/*
                                  
                                                 
                                                  
                                                  
 */
#define PHY_CSR2			0x04c4
#define PHY_CSR2_LNA			FIELD16(0x0002)
#define PHY_CSR2_LNA_MODE		FIELD16(0x3000)

/*
                                  
 */
#define PHY_CSR3			0x04c6

/*
                                     
 */
#define PHY_CSR4			0x04c8
#define PHY_CSR4_LOW_RF_LE		FIELD16(0x0001)

/*
                        
                            
 */
#define PHY_CSR5			0x04ca
#define PHY_CSR5_CCK			FIELD16(0x0003)
#define PHY_CSR5_CCK_FLIP		FIELD16(0x0004)

/*
                        
                             
 */
#define PHY_CSR6			0x04cc
#define PHY_CSR6_OFDM			FIELD16(0x0003)
#define PHY_CSR6_OFDM_FLIP		FIELD16(0x0004)

/*
                                   
                      
                               
                                       
 */
#define PHY_CSR7			0x04ce
#define PHY_CSR7_DATA			FIELD16(0x00ff)
#define PHY_CSR7_REG_ID			FIELD16(0x7f00)
#define PHY_CSR7_READ_CONTROL		FIELD16(0x8000)

/*
                                   
                                                  
 */
#define PHY_CSR8			0x04d0
#define PHY_CSR8_BUSY			FIELD16(0x0001)

/*
                                
                                                       
 */
#define PHY_CSR9			0x04d2
#define PHY_CSR9_RF_VALUE		FIELD16(0xffff)

/*
                                 
                                                       
                                                                   
                                               
                               
                                                   
 */
#define PHY_CSR10			0x04d4
#define PHY_CSR10_RF_VALUE		FIELD16(0x00ff)
#define PHY_CSR10_RF_NUMBER_OF_BITS	FIELD16(0x1f00)
#define PHY_CSR10_RF_IF_SELECT		FIELD16(0x2000)
#define PHY_CSR10_RF_PLL_LD		FIELD16(0x4000)
#define PHY_CSR10_RF_BUSY		FIELD16(0x8000)

/*
                             
                                                 
 */
#define STA_CSR0			0x04e0
#define STA_CSR0_FCS_ERROR		FIELD16(0xffff)

/*
                              
 */
#define STA_CSR1			0x04e2

/*
                              
 */
#define STA_CSR2			0x04e4

/*
                             
                                                             
 */
#define STA_CSR3			0x04e6
#define STA_CSR3_FALSE_CCA_ERROR	FIELD16(0xffff)

/*
                              
 */
#define STA_CSR4			0x04e8

/*
                                 
 */
#define STA_CSR5			0x04ea

/*
                        
 */
#define STA_CSR6			0x04ec
#define STA_CSR7			0x04ee
#define STA_CSR8			0x04f0
#define STA_CSR9			0x04f2
#define STA_CSR10			0x04f4

/*
                 
                                     
 */

/*
                         
 */
#define BBP_R2_TX_ANTENNA		FIELD8(0x03)
#define BBP_R2_TX_IQ_FLIP		FIELD8(0x04)

/*
                          
 */
#define BBP_R14_RX_ANTENNA		FIELD8(0x03)
#define BBP_R14_RX_IQ_FLIP		FIELD8(0x04)

/*
                
 */

/*
       
 */
#define RF1_TUNER			FIELD32(0x00020000)

/*
       
 */
#define RF3_TUNER			FIELD32(0x00000100)
#define RF3_TXPOWER			FIELD32(0x00003e00)

/*
                   
 */

/*
                  
 */
#define EEPROM_MAC_ADDR_0		0x0002
#define EEPROM_MAC_ADDR_BYTE0		FIELD16(0x00ff)
#define EEPROM_MAC_ADDR_BYTE1		FIELD16(0xff00)
#define EEPROM_MAC_ADDR1		0x0003
#define EEPROM_MAC_ADDR_BYTE2		FIELD16(0x00ff)
#define EEPROM_MAC_ADDR_BYTE3		FIELD16(0xff00)
#define EEPROM_MAC_ADDR_2		0x0004
#define EEPROM_MAC_ADDR_BYTE4		FIELD16(0x00ff)
#define EEPROM_MAC_ADDR_BYTE5		FIELD16(0xff00)

/*
                  
                                    
                                                        
                                                        
                                                                             
                                     
                                                            
                                    
 */
#define EEPROM_ANTENNA			0x000b
#define EEPROM_ANTENNA_NUM		FIELD16(0x0003)
#define EEPROM_ANTENNA_TX_DEFAULT	FIELD16(0x000c)
#define EEPROM_ANTENNA_RX_DEFAULT	FIELD16(0x0030)
#define EEPROM_ANTENNA_LED_MODE		FIELD16(0x01c0)
#define EEPROM_ANTENNA_DYN_TXAGC	FIELD16(0x0200)
#define EEPROM_ANTENNA_HARDWARE_RADIO	FIELD16(0x0400)
#define EEPROM_ANTENNA_RF_TYPE		FIELD16(0xf800)

/*
                     
                                        
                                       
                                           
 */
#define EEPROM_NIC			0x000c
#define EEPROM_NIC_CARDBUS_ACCEL	FIELD16(0x0001)
#define EEPROM_NIC_DYN_BBP_TUNE		FIELD16(0x0002)
#define EEPROM_NIC_CCK_TX_POWER		FIELD16(0x000c)

/*
                    
                                             
 */
#define EEPROM_GEOGRAPHY		0x000d
#define EEPROM_GEOGRAPHY_GEO		FIELD16(0x0f00)

/*
              
 */
#define EEPROM_BBP_START		0x000e
#define EEPROM_BBP_SIZE			16
#define EEPROM_BBP_VALUE		FIELD16(0x00ff)
#define EEPROM_BBP_REG_ID		FIELD16(0xff00)

/*
                 
 */
#define EEPROM_TXPOWER_START		0x001e
#define EEPROM_TXPOWER_SIZE		7
#define EEPROM_TXPOWER_1		FIELD16(0x00ff)
#define EEPROM_TXPOWER_2		FIELD16(0xff00)

/*
                          
 */
#define EEPROM_BBPTUNE			0x0030
#define EEPROM_BBPTUNE_THRESHOLD	FIELD16(0x00ff)

/*
                         
 */
#define EEPROM_BBPTUNE_R24		0x0031
#define EEPROM_BBPTUNE_R24_LOW		FIELD16(0x00ff)
#define EEPROM_BBPTUNE_R24_HIGH		FIELD16(0xff00)

/*
                         
 */
#define EEPROM_BBPTUNE_R25		0x0032
#define EEPROM_BBPTUNE_R25_LOW		FIELD16(0x00ff)
#define EEPROM_BBPTUNE_R25_HIGH		FIELD16(0xff00)

/*
                         
 */
#define EEPROM_BBPTUNE_R61		0x0033
#define EEPROM_BBPTUNE_R61_LOW		FIELD16(0x00ff)
#define EEPROM_BBPTUNE_R61_HIGH		FIELD16(0xff00)

/*
                         
 */
#define EEPROM_BBPTUNE_VGC		0x0034
#define EEPROM_BBPTUNE_VGCUPPER		FIELD16(0x00ff)
#define EEPROM_BBPTUNE_VGCLOWER		FIELD16(0xff00)

/*
                         
 */
#define EEPROM_BBPTUNE_R17		0x0035
#define EEPROM_BBPTUNE_R17_LOW		FIELD16(0x00ff)
#define EEPROM_BBPTUNE_R17_HIGH		FIELD16(0xff00)

/*
                                  
 */
#define EEPROM_CALIBRATE_OFFSET		0x0036
#define EEPROM_CALIBRATE_OFFSET_RSSI	FIELD16(0x00ff)

/*
                          
 */
#define TXD_DESC_SIZE			( 5 * sizeof(__le32) )
#define RXD_DESC_SIZE			( 4 * sizeof(__le32) )

/*
                                                           
 */

/*
        
 */
#define TXD_W0_PACKET_ID		FIELD32(0x0000000f)
#define TXD_W0_RETRY_LIMIT		FIELD32(0x000000f0)
#define TXD_W0_MORE_FRAG		FIELD32(0x00000100)
#define TXD_W0_ACK			FIELD32(0x00000200)
#define TXD_W0_TIMESTAMP		FIELD32(0x00000400)
#define TXD_W0_OFDM			FIELD32(0x00000800)
#define TXD_W0_NEW_SEQ			FIELD32(0x00001000)
#define TXD_W0_IFS			FIELD32(0x00006000)
#define TXD_W0_DATABYTE_COUNT		FIELD32(0x0fff0000)
#define TXD_W0_CIPHER			FIELD32(0x20000000)
#define TXD_W0_KEY_ID			FIELD32(0xc0000000)

/*
        
 */
#define TXD_W1_IV_OFFSET		FIELD32(0x0000003f)
#define TXD_W1_AIFS			FIELD32(0x000000c0)
#define TXD_W1_CWMIN			FIELD32(0x00000f00)
#define TXD_W1_CWMAX			FIELD32(0x0000f000)

/*
                          
 */
#define TXD_W2_PLCP_SIGNAL		FIELD32(0x000000ff)
#define TXD_W2_PLCP_SERVICE		FIELD32(0x0000ff00)
#define TXD_W2_PLCP_LENGTH_LOW		FIELD32(0x00ff0000)
#define TXD_W2_PLCP_LENGTH_HIGH		FIELD32(0xff000000)

/*
        
 */
#define TXD_W3_IV			FIELD32(0xffffffff)

/*
        
 */
#define TXD_W4_EIV			FIELD32(0xffffffff)

/*
                                    
 */

/*
        
 */
#define RXD_W0_UNICAST_TO_ME		FIELD32(0x00000002)
#define RXD_W0_MULTICAST		FIELD32(0x00000004)
#define RXD_W0_BROADCAST		FIELD32(0x00000008)
#define RXD_W0_MY_BSS			FIELD32(0x00000010)
#define RXD_W0_CRC_ERROR		FIELD32(0x00000020)
#define RXD_W0_OFDM			FIELD32(0x00000040)
#define RXD_W0_PHYSICAL_ERROR		FIELD32(0x00000080)
#define RXD_W0_CIPHER			FIELD32(0x00000100)
#define RXD_W0_CIPHER_ERROR		FIELD32(0x00000200)
#define RXD_W0_DATABYTE_COUNT		FIELD32(0x0fff0000)

/*
        
 */
#define RXD_W1_RSSI			FIELD32(0x000000ff)
#define RXD_W1_SIGNAL			FIELD32(0x0000ff00)

/*
        
 */
#define RXD_W2_IV			FIELD32(0xffffffff)

/*
        
 */
#define RXD_W3_EIV			FIELD32(0xffffffff)

/*
                                                              
                                             
 */
#define MIN_TXPOWER	0
#define MAX_TXPOWER	31
#define DEFAULT_TXPOWER	24

#define TXPOWER_FROM_DEV(__txpower) \
	(((u8)(__txpower)) > MAX_TXPOWER) ? DEFAULT_TXPOWER : (__txpower)

#define TXPOWER_TO_DEV(__txpower) \
	clamp_t(char, __txpower, MIN_TXPOWER, MAX_TXPOWER)

#endif /*             */
