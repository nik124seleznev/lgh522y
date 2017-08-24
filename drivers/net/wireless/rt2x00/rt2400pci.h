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

#ifndef RT2400PCI_H
#define RT2400PCI_H

/*
                   
 */
#define RF2420				0x0000
#define RF2421				0x0001

/*
                      
                                                          
 */
#define DEFAULT_RSSI_OFFSET		100

/*
                               
 */
#define CSR_REG_BASE			0x0000
#define CSR_REG_SIZE			0x014c
#define EEPROM_BASE			0x0000
#define EEPROM_SIZE			0x0100
#define BBP_BASE			0x0000
#define BBP_SIZE			0x0020
#define RF_BASE				0x0004
#define RF_SIZE				0x000c

/*
                       
 */
#define NUM_TX_QUEUES			2

/*
                                 
                                                      
 */

/*
                              
 */
#define CSR0				0x0000
#define CSR0_REVISION			FIELD32(0x0000ffff)

/*
                                 
                                                   
                                                   
                                               
 */
#define CSR1				0x0004
#define CSR1_SOFT_RESET			FIELD32(0x00000001)
#define CSR1_BBP_RESET			FIELD32(0x00000002)
#define CSR1_HOST_READY			FIELD32(0x00000004)

/*
                                                
 */
#define CSR2				0x0008

/*
                                    
 */
#define CSR3				0x000c
#define CSR3_BYTE0			FIELD32(0x000000ff)
#define CSR3_BYTE1			FIELD32(0x0000ff00)
#define CSR3_BYTE2			FIELD32(0x00ff0000)
#define CSR3_BYTE3			FIELD32(0xff000000)

/*
                                    
 */
#define CSR4				0x0010
#define CSR4_BYTE4			FIELD32(0x000000ff)
#define CSR4_BYTE5			FIELD32(0x0000ff00)

/*
                          
 */
#define CSR5				0x0014
#define CSR5_BYTE0			FIELD32(0x000000ff)
#define CSR5_BYTE1			FIELD32(0x0000ff00)
#define CSR5_BYTE2			FIELD32(0x00ff0000)
#define CSR5_BYTE3			FIELD32(0xff000000)

/*
                          
 */
#define CSR6				0x0018
#define CSR6_BYTE4			FIELD32(0x000000ff)
#define CSR6_BYTE5			FIELD32(0x0000ff00)

/*
                                   
                              
                                               
                                                
                                                         
                                                  
                                                      
                                                          
                                  
 */
#define CSR7				0x001c
#define CSR7_TBCN_EXPIRE		FIELD32(0x00000001)
#define CSR7_TWAKE_EXPIRE		FIELD32(0x00000002)
#define CSR7_TATIMW_EXPIRE		FIELD32(0x00000004)
#define CSR7_TXDONE_TXRING		FIELD32(0x00000008)
#define CSR7_TXDONE_ATIMRING		FIELD32(0x00000010)
#define CSR7_TXDONE_PRIORING		FIELD32(0x00000020)
#define CSR7_RXDONE			FIELD32(0x00000040)

/*
                                 
                             
                                               
                                                
                                                         
                                                  
                                                      
                                                          
                                  
 */
#define CSR8				0x0020
#define CSR8_TBCN_EXPIRE		FIELD32(0x00000001)
#define CSR8_TWAKE_EXPIRE		FIELD32(0x00000002)
#define CSR8_TATIMW_EXPIRE		FIELD32(0x00000004)
#define CSR8_TXDONE_TXRING		FIELD32(0x00000008)
#define CSR8_TXDONE_ATIMRING		FIELD32(0x00000010)
#define CSR8_TXDONE_PRIORING		FIELD32(0x00000020)
#define CSR8_RXDONE			FIELD32(0x00000040)

/*
                                       
                                                                  
 */
#define CSR9				0x0024
#define CSR9_MAX_FRAME_UNIT		FIELD32(0x00000f80)

/*
                                    
                                               
                                                  
                                                     
                                
                                  
 */
#define CSR11				0x002c
#define CSR11_CWMIN			FIELD32(0x0000000f)
#define CSR11_CWMAX			FIELD32(0x000000f0)
#define CSR11_SLOT_TIME			FIELD32(0x00001f00)
#define CSR11_LONG_RETRY		FIELD32(0x00ff0000)
#define CSR11_SHORT_RETRY		FIELD32(0xff000000)

/*
                                                   
                        
                                                       
                                                            
 */
#define CSR12				0x0030
#define CSR12_BEACON_INTERVAL		FIELD32(0x0000ffff)
#define CSR12_CFP_MAX_DURATION		FIELD32(0xffff0000)

/*
                                                   
                        
                                        
                                           
 */
#define CSR13				0x0034
#define CSR13_ATIMW_DURATION		FIELD32(0x0000ffff)
#define CSR13_CFP_PERIOD		FIELD32(0x00ff0000)

/*
                                           
                                       
                                                                   
                                       
                                          
                                                 
                                       
                                              
                                                     
 */
#define CSR14				0x0038
#define CSR14_TSF_COUNT			FIELD32(0x00000001)
#define CSR14_TSF_SYNC			FIELD32(0x00000006)
#define CSR14_TBCN			FIELD32(0x00000008)
#define CSR14_TCFP			FIELD32(0x00000010)
#define CSR14_TATIMW			FIELD32(0x00000020)
#define CSR14_BEACON_GEN		FIELD32(0x00000040)
#define CSR14_CFP_COUNT_PRELOAD		FIELD32(0x0000ff00)
#define CSR14_TBCM_PRELOAD		FIELD32(0xffff0000)

/*
                                          
                                          
                                 
                               
 */
#define CSR15				0x003c
#define CSR15_CFP			FIELD32(0x00000001)
#define CSR15_ATIMW			FIELD32(0x00000002)
#define CSR15_BEACON_SENT		FIELD32(0x00000004)

/*
                               
 */
#define CSR16				0x0040
#define CSR16_LOW_TSFTIMER		FIELD32(0xffffffff)

/*
                               
 */
#define CSR17				0x0044
#define CSR17_HIGH_TSFTIMER		FIELD32(0xffffffff)

/*
                               
                                
                                
 */
#define CSR18				0x0048
#define CSR18_SIFS			FIELD32(0x0000ffff)
#define CSR18_PIFS			FIELD32(0xffff0000)

/*
                               
                                
                                 
 */
#define CSR19				0x004c
#define CSR19_DIFS			FIELD32(0x0000ffff)
#define CSR19_EIFS			FIELD32(0xffff0000)

/*
                                
                                                                  
                                                      
                                                  
 */
#define CSR20				0x0050
#define CSR20_DELAY_AFTER_TBCN		FIELD32(0x0000ffff)
#define CSR20_TBCN_BEFORE_WAKEUP	FIELD32(0x00ff0000)
#define CSR20_AUTOWAKE			FIELD32(0x01000000)

/*
                                  
                                            
                                 
 */
#define CSR21				0x0054
#define CSR21_RELOAD			FIELD32(0x00000001)
#define CSR21_EEPROM_DATA_CLOCK		FIELD32(0x00000002)
#define CSR21_EEPROM_CHIP_SELECT	FIELD32(0x00000004)
#define CSR21_EEPROM_DATA_IN		FIELD32(0x00000008)
#define CSR21_EEPROM_DATA_OUT		FIELD32(0x00000010)
#define CSR21_TYPE_93C46		FIELD32(0x00000020)

/*
                               
                                                            
                                                              
 */
#define CSR22				0x0058
#define CSR22_CFP_DURATION_REMAIN	FIELD32(0x0000ffff)
#define CSR22_RELOAD_CFP_DURATION	FIELD32(0x00010000)

/*
                         
                                                      
 */

/*
                               
                         
                             
                                 
                                                    
 */
#define TXCSR0				0x0060
#define TXCSR0_KICK_TX			FIELD32(0x00000001)
#define TXCSR0_KICK_ATIM		FIELD32(0x00000002)
#define TXCSR0_KICK_PRIO		FIELD32(0x00000004)
#define TXCSR0_ABORT			FIELD32(0x00000008)

/*
                                     
                                                                           
                                                                        
                                 
                                                                
 */
#define TXCSR1				0x0064
#define TXCSR1_ACK_TIMEOUT		FIELD32(0x000001ff)
#define TXCSR1_ACK_CONSUME_TIME		FIELD32(0x0003fe00)
#define TXCSR1_TSF_OFFSET		FIELD32(0x00fc0000)
#define TXCSR1_AUTORESPONDER		FIELD32(0x01000000)

/*
                                                
                                               
                                         
                                            
                                                
 */
#define TXCSR2				0x0068
#define TXCSR2_TXD_SIZE			FIELD32(0x000000ff)
#define TXCSR2_NUM_TXD			FIELD32(0x0000ff00)
#define TXCSR2_NUM_ATIM			FIELD32(0x00ff0000)
#define TXCSR2_NUM_PRIO			FIELD32(0xff000000)

/*
                                         
 */
#define TXCSR3				0x006c
#define TXCSR3_TX_RING_REGISTER		FIELD32(0xffffffff)

/*
                                              
 */
#define TXCSR4				0x0070
#define TXCSR4_ATIM_RING_REGISTER	FIELD32(0xffffffff)

/*
                                              
 */
#define TXCSR5				0x0074
#define TXCSR5_PRIO_RING_REGISTER	FIELD32(0xffffffff)

/*
                                        
 */
#define TXCSR6				0x0078
#define TXCSR6_BEACON_RING_REGISTER	FIELD32(0xffffffff)

/*
                                           
                                                           
 */
#define TXCSR7				0x007c
#define TXCSR7_AR_POWERMANAGEMENT	FIELD32(0x00000001)

/*
                        
                                                      
 */

/*
                               
                                 
                            
                                      
                                    
                                                
                                          
                                                
                                                
 */
#define RXCSR0				0x0080
#define RXCSR0_DISABLE_RX		FIELD32(0x00000001)
#define RXCSR0_DROP_CRC			FIELD32(0x00000002)
#define RXCSR0_DROP_PHYSICAL		FIELD32(0x00000004)
#define RXCSR0_DROP_CONTROL		FIELD32(0x00000008)
#define RXCSR0_DROP_NOT_TO_ME		FIELD32(0x00000010)
#define RXCSR0_DROP_TODS		FIELD32(0x00000020)
#define RXCSR0_DROP_VERSION_ERROR	FIELD32(0x00000040)
#define RXCSR0_PASS_CRC			FIELD32(0x00000080)

/*
                                                
                                                
                                         
 */
#define RXCSR1				0x0084
#define RXCSR1_RXD_SIZE			FIELD32(0x000000ff)
#define RXCSR1_NUM_RXD			FIELD32(0x0000ff00)

/*
                                         
 */
#define RXCSR2				0x0088
#define RXCSR2_RX_RING_REGISTER		FIELD32(0xffffffff)

/*
                                            
                              
                                                    
 */
#define RXCSR3				0x0090
#define RXCSR3_BBP_ID0			FIELD32(0x0000007f)
#define RXCSR3_BBP_ID0_VALID		FIELD32(0x00000080)
#define RXCSR3_BBP_ID1			FIELD32(0x00007f00)
#define RXCSR3_BBP_ID1_VALID		FIELD32(0x00008000)
#define RXCSR3_BBP_ID2			FIELD32(0x007f0000)
#define RXCSR3_BBP_ID2_VALID		FIELD32(0x00800000)
#define RXCSR3_BBP_ID3			FIELD32(0x7f000000)
#define RXCSR3_BBP_ID3_VALID		FIELD32(0x80000000)

/*
                                            
                              
                                                    
 */
#define RXCSR4				0x0094
#define RXCSR4_BBP_ID4			FIELD32(0x0000007f)
#define RXCSR4_BBP_ID4_VALID		FIELD32(0x00000080)
#define RXCSR4_BBP_ID5			FIELD32(0x00007f00)
#define RXCSR4_BBP_ID5_VALID		FIELD32(0x00008000)

/*
                                                 
                                                           
                                                       
 */
#define ARCSR0				0x0098
#define ARCSR0_AR_BBP_DATA0		FIELD32(0x000000ff)
#define ARCSR0_AR_BBP_ID0		FIELD32(0x0000ff00)
#define ARCSR0_AR_BBP_DATA1		FIELD32(0x00ff0000)
#define ARCSR0_AR_BBP_ID1		FIELD32(0xff000000)

/*
                                                 
                                                           
                                                       
 */
#define ARCSR1				0x009c
#define ARCSR1_AR_BBP_DATA2		FIELD32(0x000000ff)
#define ARCSR1_AR_BBP_ID2		FIELD32(0x0000ff00)
#define ARCSR1_AR_BBP_DATA3		FIELD32(0x00ff0000)
#define ARCSR1_AR_BBP_ID3		FIELD32(0xff000000)

/*
                           
                                                      
 */

/*
                                
                                               
                                                      
                                              
                                                      
                                                
                                                                          
                                                                             
 */
#define PCICSR				0x008c
#define PCICSR_BIG_ENDIAN		FIELD32(0x00000001)
#define PCICSR_RX_TRESHOLD		FIELD32(0x00000006)
#define PCICSR_TX_TRESHOLD		FIELD32(0x00000018)
#define PCICSR_BURST_LENTH		FIELD32(0x00000060)
#define PCICSR_ENABLE_CLK		FIELD32(0x00000080)

/*
                         
                                                 
 */
#define CNT0				0x00a0
#define CNT0_FCS_ERROR			FIELD32(0x0000ffff)

/*
                      
                          
                          
                               
                                
                                
 */
#define TIMECSR2			0x00a8
#define CNT1				0x00ac
#define CNT2				0x00b0
#define TIMECSR3			0x00b4
#define CNT3				0x00b8
#define CNT4				0x00bc
#define CNT5				0x00c0

/*
                             
 */

/*
                                              
 */
#define PWRCSR0				0x00c4

/*
                                         
 */
#define PSCSR0				0x00c8
#define PSCSR1				0x00cc
#define PSCSR2				0x00d0
#define PSCSR3				0x00d4

/*
                                                   
                                                               
                                                          
                                       
                                     
                                     
                                   
                                                                
 */
#define PWRCSR1				0x00d8
#define PWRCSR1_SET_STATE		FIELD32(0x00000001)
#define PWRCSR1_BBP_DESIRE_STATE	FIELD32(0x00000006)
#define PWRCSR1_RF_DESIRE_STATE		FIELD32(0x00000018)
#define PWRCSR1_BBP_CURR_STATE		FIELD32(0x00000060)
#define PWRCSR1_RF_CURR_STATE		FIELD32(0x00000180)
#define PWRCSR1_PUT_TO_SLEEP		FIELD32(0x00000200)

/*
                                   
                                                       
                                                         
                                       
 */
#define TIMECSR				0x00dc
#define TIMECSR_US_COUNT		FIELD32(0x000000ff)
#define TIMECSR_US_64_COUNT		FIELD32(0x0000ff00)
#define TIMECSR_BEACON_EXPECT		FIELD32(0x00070000)

/*
                                         
 */
#define MACCSR0				0x00e0

/*
                                         
                                                 
                                                         
                                              
                                                         
                                                         
                                                                        
                                            
 */
#define MACCSR1				0x00e4
#define MACCSR1_KICK_RX			FIELD32(0x00000001)
#define MACCSR1_ONESHOT_RXMODE		FIELD32(0x00000002)
#define MACCSR1_BBPRX_RESET_MODE	FIELD32(0x00000004)
#define MACCSR1_AUTO_TXBBP		FIELD32(0x00000008)
#define MACCSR1_AUTO_RXBBP		FIELD32(0x00000010)
#define MACCSR1_LOOPBACK		FIELD32(0x00000060)
#define MACCSR1_INTERSIL_IF		FIELD32(0x00000080)

/*
                                        
                                                
                                            
 */
#define RALINKCSR			0x00e8
#define RALINKCSR_AR_BBP_DATA0		FIELD32(0x000000ff)
#define RALINKCSR_AR_BBP_ID0		FIELD32(0x0000ff00)
#define RALINKCSR_AR_BBP_DATA1		FIELD32(0x00ff0000)
#define RALINKCSR_AR_BBP_ID1		FIELD32(0xff000000)

/*
                                            
                                               
                                   
                                                  
                                    
                                        
 */
#define BCNCSR				0x00ec
#define BCNCSR_CHANGE			FIELD32(0x00000001)
#define BCNCSR_DELTATIME		FIELD32(0x0000001e)
#define BCNCSR_NUM_BEACON		FIELD32(0x00001fe0)
#define BCNCSR_MODE			FIELD32(0x00006000)
#define BCNCSR_PLUS			FIELD32(0x00008000)

/*
                                  
 */

/*
                                       
                                             
                                 
                                                 
                                            
 */
#define BBPCSR				0x00f0
#define BBPCSR_VALUE			FIELD32(0x000000ff)
#define BBPCSR_REGNUM			FIELD32(0x00007f00)
#define BBPCSR_BUSY			FIELD32(0x00008000)
#define BBPCSR_WRITE_CONTROL		FIELD32(0x00010000)

/*
                                     
                                                    
                                                                
                                            
                            
                                                
 */
#define RFCSR				0x00f4
#define RFCSR_VALUE			FIELD32(0x00ffffff)
#define RFCSR_NUMBER_OF_BITS		FIELD32(0x1f000000)
#define RFCSR_IF_SELECT			FIELD32(0x20000000)
#define RFCSR_PLL_LD			FIELD32(0x40000000)
#define RFCSR_BUSY			FIELD32(0x80000000)

/*
                                
                                      
                                        
                               
                                
 */
#define LEDCSR				0x00f8
#define LEDCSR_ON_PERIOD		FIELD32(0x000000ff)
#define LEDCSR_OFF_PERIOD		FIELD32(0x0000ff00)
#define LEDCSR_LINK			FIELD32(0x00010000)
#define LEDCSR_ACTIVITY			FIELD32(0x00020000)

/*
                            
                                  
                                  
                                         
                                      
 */
#define RXPTR				0x0100
#define TXPTR				0x0104
#define PRIPTR				0x0108
#define ATIMPTR				0x010c

/*
                   
 */

/*
                                  
                                        
                                                      
 */
#define GPIOCSR				0x0120
#define GPIOCSR_VAL0			FIELD32(0x00000001)
#define GPIOCSR_VAL1			FIELD32(0x00000002)
#define GPIOCSR_VAL2			FIELD32(0x00000004)
#define GPIOCSR_VAL3			FIELD32(0x00000008)
#define GPIOCSR_VAL4			FIELD32(0x00000010)
#define GPIOCSR_VAL5			FIELD32(0x00000020)
#define GPIOCSR_VAL6			FIELD32(0x00000040)
#define GPIOCSR_VAL7			FIELD32(0x00000080)
#define GPIOCSR_DIR0			FIELD32(0x00000100)
#define GPIOCSR_DIR1			FIELD32(0x00000200)
#define GPIOCSR_DIR2			FIELD32(0x00000400)
#define GPIOCSR_DIR3			FIELD32(0x00000800)
#define GPIOCSR_DIR4			FIELD32(0x00001000)
#define GPIOCSR_DIR5			FIELD32(0x00002000)
#define GPIOCSR_DIR6			FIELD32(0x00004000)
#define GPIOCSR_DIR7			FIELD32(0x00008000)

/*
                                     
 */
#define BBPPCSR				0x0124

/*
                                                   
                                                 
 */
#define BCNCSR1				0x0130
#define BCNCSR1_PRELOAD			FIELD32(0x0000ffff)

/*
                                                            
                                                       
 */
#define MACCSR2				0x0134
#define MACCSR2_DELAY			FIELD32(0x000000ff)

/*
                               
 */
#define ARCSR2				0x013c
#define ARCSR2_SIGNAL			FIELD32(0x000000ff)
#define ARCSR2_SERVICE			FIELD32(0x0000ff00)
#define ARCSR2_LENGTH_LOW		FIELD32(0x00ff0000)
#define ARCSR2_LENGTH			FIELD32(0xffff0000)

/*
                               
 */
#define ARCSR3				0x0140
#define ARCSR3_SIGNAL			FIELD32(0x000000ff)
#define ARCSR3_SERVICE			FIELD32(0x0000ff00)
#define ARCSR3_LENGTH			FIELD32(0xffff0000)

/*
                                 
 */
#define ARCSR4				0x0144
#define ARCSR4_SIGNAL			FIELD32(0x000000ff)
#define ARCSR4_SERVICE			FIELD32(0x0000ff00)
#define ARCSR4_LENGTH			FIELD32(0xffff0000)

/*
                                
 */
#define ARCSR5				0x0148
#define ARCSR5_SIGNAL			FIELD32(0x000000ff)
#define ARCSR5_SERVICE			FIELD32(0x0000ff00)
#define ARCSR5_LENGTH			FIELD32(0xffff0000)

/*
                 
                                     
 */

/*
                         
 */
#define BBP_R1_TX_ANTENNA		FIELD8(0x03)

/*
                         
 */
#define BBP_R4_RX_ANTENNA		FIELD8(0x06)

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
#define EEPROM_ANTENNA			0x0b
#define EEPROM_ANTENNA_NUM		FIELD16(0x0003)
#define EEPROM_ANTENNA_TX_DEFAULT	FIELD16(0x000c)
#define EEPROM_ANTENNA_RX_DEFAULT	FIELD16(0x0030)
#define EEPROM_ANTENNA_RF_TYPE		FIELD16(0x0040)
#define EEPROM_ANTENNA_LED_MODE		FIELD16(0x0180)
#define EEPROM_ANTENNA_RX_AGCVGC_TUNING	FIELD16(0x0200)
#define EEPROM_ANTENNA_HARDWARE_RADIO	FIELD16(0x0400)

/*
              
 */
#define EEPROM_BBP_START		0x0c
#define EEPROM_BBP_SIZE			7
#define EEPROM_BBP_VALUE		FIELD16(0x00ff)
#define EEPROM_BBP_REG_ID		FIELD16(0xff00)

/*
                 
 */
#define EEPROM_TXPOWER_START		0x13
#define EEPROM_TXPOWER_SIZE		7
#define EEPROM_TXPOWER_1		FIELD16(0x00ff)
#define EEPROM_TXPOWER_2		FIELD16(0xff00)

/*
                          
 */
#define TXD_DESC_SIZE			(8 * sizeof(__le32))
#define RXD_DESC_SIZE			(8 * sizeof(__le32))

/*
                                                           
 */

/*
        
 */
#define TXD_W0_OWNER_NIC		FIELD32(0x00000001)
#define TXD_W0_VALID			FIELD32(0x00000002)
#define TXD_W0_RESULT			FIELD32(0x0000001c)
#define TXD_W0_RETRY_COUNT		FIELD32(0x000000e0)
#define TXD_W0_MORE_FRAG		FIELD32(0x00000100)
#define TXD_W0_ACK			FIELD32(0x00000200)
#define TXD_W0_TIMESTAMP		FIELD32(0x00000400)
#define TXD_W0_RTS			FIELD32(0x00000800)
#define TXD_W0_IFS			FIELD32(0x00006000)
#define TXD_W0_RETRY_MODE		FIELD32(0x00008000)
#define TXD_W0_AGC			FIELD32(0x00ff0000)
#define TXD_W0_R2			FIELD32(0xff000000)

/*
        
 */
#define TXD_W1_BUFFER_ADDRESS		FIELD32(0xffffffff)

/*
        
 */
#define TXD_W2_BUFFER_LENGTH		FIELD32(0x0000ffff)
#define TXD_W2_DATABYTE_COUNT		FIELD32(0xffff0000)

/*
                              
                                                                
 */
#define TXD_W3_PLCP_SIGNAL		FIELD32(0x000000ff)
#define TXD_W3_PLCP_SIGNAL_REGNUM	FIELD32(0x00007f00)
#define TXD_W3_PLCP_SIGNAL_BUSY		FIELD32(0x00008000)
#define TXD_W3_PLCP_SERVICE		FIELD32(0x00ff0000)
#define TXD_W3_PLCP_SERVICE_REGNUM	FIELD32(0x7f000000)
#define TXD_W3_PLCP_SERVICE_BUSY	FIELD32(0x80000000)

#define TXD_W4_PLCP_LENGTH_LOW		FIELD32(0x000000ff)
#define TXD_W3_PLCP_LENGTH_LOW_REGNUM	FIELD32(0x00007f00)
#define TXD_W3_PLCP_LENGTH_LOW_BUSY	FIELD32(0x00008000)
#define TXD_W4_PLCP_LENGTH_HIGH		FIELD32(0x00ff0000)
#define TXD_W3_PLCP_LENGTH_HIGH_REGNUM	FIELD32(0x7f000000)
#define TXD_W3_PLCP_LENGTH_HIGH_BUSY	FIELD32(0x80000000)

/*
        
 */
#define TXD_W5_BBCR4			FIELD32(0x0000ffff)
#define TXD_W5_AGC_REG			FIELD32(0x007f0000)
#define TXD_W5_AGC_REG_VALID		FIELD32(0x00800000)
#define TXD_W5_XXX_REG			FIELD32(0x7f000000)
#define TXD_W5_XXX_REG_VALID		FIELD32(0x80000000)

/*
        
 */
#define TXD_W6_SK_BUFF			FIELD32(0xffffffff)

/*
        
 */
#define TXD_W7_RESERVED			FIELD32(0xffffffff)

/*
                                    
 */

/*
        
 */
#define RXD_W0_OWNER_NIC		FIELD32(0x00000001)
#define RXD_W0_UNICAST_TO_ME		FIELD32(0x00000002)
#define RXD_W0_MULTICAST		FIELD32(0x00000004)
#define RXD_W0_BROADCAST		FIELD32(0x00000008)
#define RXD_W0_MY_BSS			FIELD32(0x00000010)
#define RXD_W0_CRC_ERROR		FIELD32(0x00000020)
#define RXD_W0_PHYSICAL_ERROR		FIELD32(0x00000080)
#define RXD_W0_DATABYTE_COUNT		FIELD32(0xffff0000)

/*
        
 */
#define RXD_W1_BUFFER_ADDRESS		FIELD32(0xffffffff)

/*
        
 */
#define RXD_W2_BUFFER_LENGTH		FIELD32(0x0000ffff)
#define RXD_W2_BBR0			FIELD32(0x00ff0000)
#define RXD_W2_SIGNAL			FIELD32(0xff000000)

/*
        
 */
#define RXD_W3_RSSI			FIELD32(0x000000ff)
#define RXD_W3_BBR3			FIELD32(0x0000ff00)
#define RXD_W3_BBR4			FIELD32(0x00ff0000)
#define RXD_W3_BBR5			FIELD32(0xff000000)

/*
        
 */
#define RXD_W4_RX_END_TIME		FIELD32(0xffffffff)

/*
                          
 */
#define RXD_W5_RESERVED			FIELD32(0xffffffff)
#define RXD_W6_RESERVED			FIELD32(0xffffffff)
#define RXD_W7_RESERVED			FIELD32(0xffffffff)

/*
                                                              
                                             
                                                     
                                                         
                                                        
                                                      
                                                 
 */
#define MIN_TXPOWER	31
#define MAX_TXPOWER	62
#define DEFAULT_TXPOWER	39

#define __CLAMP_TX(__txpower) \
	clamp_t(char, (__txpower), MIN_TXPOWER, MAX_TXPOWER)

#define TXPOWER_FROM_DEV(__txpower) \
	((__CLAMP_TX(__txpower) - MAX_TXPOWER) + MIN_TXPOWER)

#define TXPOWER_TO_DEV(__txpower) \
	(MAX_TXPOWER - (__CLAMP_TX(__txpower) - MIN_TXPOWER))

#endif /*             */