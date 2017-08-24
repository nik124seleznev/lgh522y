/*
 * This file is part of wl12xx
 *
 * Copyright (C) 1998-2009 Texas Instruments. All rights reserved.
 * Copyright (C) 2009 Nokia Corporation
 *
 * Contact: Luciano Coelho <luciano.coelho@nokia.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#ifndef __REG_H__
#define __REG_H__

#include <linux/bitops.h>

#define REGISTERS_BASE 0x00300000
#define DRPW_BASE      0x00310000

#define REGISTERS_DOWN_SIZE 0x00008800
#define REGISTERS_WORK_SIZE 0x0000b000

#define FW_STATUS_ADDR                      (0x14FC0 + 0xA000)

/*                                               
                                 
                                           
                   
                                                     
                                                     
                                                      
                                                  
                                                     
                                              
                                                    
                                          
                                   
                                               */
#define WL12XX_SLV_SOFT_RESET		(REGISTERS_BASE + 0x0000)

#define WL1271_SLV_REG_DATA            (REGISTERS_BASE + 0x0008)
#define WL1271_SLV_REG_ADATA           (REGISTERS_BASE + 0x000c)
#define WL1271_SLV_MEM_DATA            (REGISTERS_BASE + 0x0018)

#define WL12XX_REG_INTERRUPT_TRIG         (REGISTERS_BASE + 0x0474)
#define WL12XX_REG_INTERRUPT_TRIG_H       (REGISTERS_BASE + 0x0478)

/*                                             
                                           
                                            
                                          
                                      
                                                  
                              
                                    
                                                   
                                    
                                       
                                       
                                      
                                     
                                     
                                         
     
     
     
                                                
                                                   
                                                    
         
         
         
        
                
                                               */
#define WL12XX_REG_INTERRUPT_MASK         (REGISTERS_BASE + 0x04DC)

/*                                             
                                             
                                            
                                    
                                                
                           
                                     
                                              */
#define ACX_REG_HINT_MASK_SET          (REGISTERS_BASE + 0x04E0)

/*                                             
                                              
                                            
                                      
                                                
                           
                                     
                                             */
#define ACX_REG_HINT_MASK_CLR          (REGISTERS_BASE + 0x04E4)

/*                                             
                                           
                   
                                            
                                             
                             
                              
                    
                                             */
#define WL12XX_REG_INTERRUPT_NO_CLEAR     (REGISTERS_BASE + 0x04E8)

/*                                             
                                               
                   
                                            
                                             
                             
                                 
                                     
                                              */
#define ACX_REG_INTERRUPT_CLEAR        (REGISTERS_BASE + 0x04F8)

/*                                             
                                     
                    
                                            
                                         
                                             
                                              
                                       
                                             
                                              */
#define WL12XX_REG_INTERRUPT_ACK          (REGISTERS_BASE + 0x04F0)

#define WL12XX_REG_RX_DRIVER_COUNTER	(REGISTERS_BASE + 0x0538)

/*                               */
#define SOR_CFG                        (REGISTERS_BASE + 0x0800)

/*                          */

/*                                               
                       
                                           
                                                
                                                      
                                        
                       
                                         
                                 
                                                             
                       
                                                                  
                                                      
                     
                     
            
              
                                                */
#define WL12XX_REG_ECPU_CONTROL           (REGISTERS_BASE + 0x0804)

#define WL12XX_HI_CFG			(REGISTERS_BASE + 0x0808)

/*                                               
                                    
                                           
                
                                             
                                          
                     
                                                              
                                           
                                    
                                              
                                                         

                    
                                                 */
#define ACX_REG_EE_START               (REGISTERS_BASE + 0x080C)

#define WL12XX_OCP_POR_CTR		(REGISTERS_BASE + 0x09B4)
#define WL12XX_OCP_DATA_WRITE		(REGISTERS_BASE + 0x09B8)
#define WL12XX_OCP_DATA_READ		(REGISTERS_BASE + 0x09BC)
#define WL12XX_OCP_CMD			(REGISTERS_BASE + 0x09C0)

#define WL12XX_HOST_WR_ACCESS		(REGISTERS_BASE + 0x09F8)

#define WL12XX_CHIP_ID_B		(REGISTERS_BASE + 0x5674)

#define WL12XX_ENABLE			(REGISTERS_BASE + 0x5450)

/*                            */
#define WL12XX_ELP_CFG_MODE		(REGISTERS_BASE + 0x5804)
#define WL12XX_ELP_CMD			(REGISTERS_BASE + 0x5808)
#define WL12XX_PLL_CAL_TIME		(REGISTERS_BASE + 0x5810)
#define WL12XX_CLK_REQ_TIME		(REGISTERS_BASE + 0x5814)
#define WL12XX_CLK_BUF_TIME		(REGISTERS_BASE + 0x5818)

#define WL12XX_CFG_PLL_SYNC_CNT		(REGISTERS_BASE + 0x5820)

/*                      */
#define WL12XX_SCR_PAD0			(REGISTERS_BASE + 0x5608)
#define WL12XX_SCR_PAD1			(REGISTERS_BASE + 0x560C)
#define WL12XX_SCR_PAD2			(REGISTERS_BASE + 0x5610)
#define WL12XX_SCR_PAD3			(REGISTERS_BASE + 0x5614)
#define WL12XX_SCR_PAD4			(REGISTERS_BASE + 0x5618)
#define WL12XX_SCR_PAD4_SET		(REGISTERS_BASE + 0x561C)
#define WL12XX_SCR_PAD4_CLR		(REGISTERS_BASE + 0x5620)
#define WL12XX_SCR_PAD5			(REGISTERS_BASE + 0x5624)
#define WL12XX_SCR_PAD5_SET		(REGISTERS_BASE + 0x5628)
#define WL12XX_SCR_PAD5_CLR		(REGISTERS_BASE + 0x562C)
#define WL12XX_SCR_PAD6			(REGISTERS_BASE + 0x5630)
#define WL12XX_SCR_PAD7			(REGISTERS_BASE + 0x5634)
#define WL12XX_SCR_PAD8			(REGISTERS_BASE + 0x5638)
#define WL12XX_SCR_PAD9			(REGISTERS_BASE + 0x563C)

/*                */
#define WL12XX_SPARE_A1			(REGISTERS_BASE + 0x0994)
#define WL12XX_SPARE_A2			(REGISTERS_BASE + 0x0998)
#define WL12XX_SPARE_A3			(REGISTERS_BASE + 0x099C)
#define WL12XX_SPARE_A4			(REGISTERS_BASE + 0x09A0)
#define WL12XX_SPARE_A5			(REGISTERS_BASE + 0x09A4)
#define WL12XX_SPARE_A6			(REGISTERS_BASE + 0x09A8)
#define WL12XX_SPARE_A7			(REGISTERS_BASE + 0x09AC)
#define WL12XX_SPARE_A8			(REGISTERS_BASE + 0x09B0)
#define WL12XX_SPARE_B1			(REGISTERS_BASE + 0x5420)
#define WL12XX_SPARE_B2			(REGISTERS_BASE + 0x5424)
#define WL12XX_SPARE_B3			(REGISTERS_BASE + 0x5428)
#define WL12XX_SPARE_B4			(REGISTERS_BASE + 0x542C)
#define WL12XX_SPARE_B5			(REGISTERS_BASE + 0x5430)
#define WL12XX_SPARE_B6			(REGISTERS_BASE + 0x5434)
#define WL12XX_SPARE_B7			(REGISTERS_BASE + 0x5438)
#define WL12XX_SPARE_B8			(REGISTERS_BASE + 0x543C)

#define WL12XX_PLL_PARAMETERS		(REGISTERS_BASE + 0x6040)
#define WL12XX_WU_COUNTER_PAUSE		(REGISTERS_BASE + 0x6008)
#define WL12XX_WELP_ARM_COMMAND		(REGISTERS_BASE + 0x6100)
#define WL12XX_DRPW_SCRATCH_START	(DRPW_BASE + 0x002C)

#define WL12XX_CMD_MBOX_ADDRESS		0x407B4

#define ACX_REG_EEPROM_START_BIT BIT(1)

/*                                      */

/*                                               
                                    
                                           
                                         
                                                         
                                                 
                                          
                                                  
                                                         
                                                     
                                                   
                   
                                                */
#define WL12XX_REG_COMMAND_MAILBOX_PTR		(WL12XX_SCR_PAD0)

/*                                               
                                        
                                           
                                         
                                                             
                                                      
                                         
                                                              
                                                     
                                             
                                                               
                   
                                                */
#define WL12XX_REG_EVENT_MAILBOX_PTR		(WL12XX_SCR_PAD1)

/*                                               
                                   
                                           
                                                     
                                             
                                               
                      
                                                          
                                           

                                                       
                                                                
                                                   
                                                          
                                                */
#define ACX_EE_CTL_REG                      EE_CTL
#define EE_WRITE                            0x00000001ul
#define EE_READ                             0x00000002ul

/*                                               
                            
                                            
                                     
                                                     
                                                 */
#define ACX_EE_ADDR_REG                     EE_ADDR

/*                                               
                         
                                            
                                               
                                        
                              
                                                 */
#define ACX_EE_DATA_REG                     EE_DATA

/*                                               
                                 
                                            
                                         
                                            
                                               
                                                        
                                                    
                                                 */
#define ACX_EE_CFG                          EE_CFG

/*                                               
                                
                                            
                   
                                                                      
                           
                                                 */
#define ACX_GPIO_OUT_REG            GPIO_OUT
#define ACX_MAX_GPIO_LINES          15

/*                                               
                               
                                            
                   
                      
                   
                                                            
                                                 */
#define ACX_CONT_WIND_CFG_REG    CONT_WIND_CFG
#define ACX_CONT_WIND_MIN_MASK   0x0000007f
#define ACX_CONT_WIND_MAX        0x03ff0000

#define REF_FREQ_19_2                       0
#define REF_FREQ_26_0                       1
#define REF_FREQ_38_4                       2
#define REF_FREQ_40_0                       3
#define REF_FREQ_33_6                       4
#define REF_FREQ_NUM                        5

#define LUT_PARAM_INTEGER_DIVIDER           0
#define LUT_PARAM_FRACTIONAL_DIVIDER        1
#define LUT_PARAM_ATTN_BB                   2
#define LUT_PARAM_ALPHA_BB                  3
#define LUT_PARAM_STOP_TIME_BB              4
#define LUT_PARAM_BB_PLL_LOOP_FILTER        5
#define LUT_PARAM_NUM                       6

#define WL12XX_EEPROMLESS_IND		(WL12XX_SCR_PAD4)
#define USE_EEPROM                          0
#define NVS_DATA_BUNDARY_ALIGNMENT          4

/*                            */
#define FW_HDR_SIZE 8

/*                                                                             

                                            

                                                                             */

#define SHORT_PREAMBLE_BIT   BIT(0) /*                                     */
#define OFDM_RATE_BIT        BIT(6)
#define PBCC_RATE_BIT        BIT(7)

enum {
	CCK_LONG = 0,
	CCK_SHORT = SHORT_PREAMBLE_BIT,
	PBCC_LONG = PBCC_RATE_BIT,
	PBCC_SHORT = PBCC_RATE_BIT | SHORT_PREAMBLE_BIT,
	OFDM = OFDM_RATE_BIT
};

/*                                                                             

                                                 

                                                         
                                                  

             
                                                  
       
                                 
                                               
                                                
       
                                                  
                                               
                           
                                                        

                                                                             */

#define OCP_CMD_LOOP		32
#define OCP_CMD_WRITE		0x1
#define OCP_CMD_READ		0x2
#define OCP_READY_MASK		BIT(18)
#define OCP_STATUS_MASK		(BIT(16) | BIT(17))
#define OCP_STATUS_NO_RESP	0x00000
#define OCP_STATUS_OK		0x10000
#define OCP_STATUS_REQ_FAILED	0x20000
#define OCP_STATUS_RESP_ERROR	0x30000

#define OCP_REG_POLARITY     0x0064
#define OCP_REG_CLK_TYPE     0x0448
#define OCP_REG_CLK_POLARITY 0x0cb2
#define OCP_REG_CLK_PULL     0x0cb4

#define POLARITY_LOW         BIT(1)
#define NO_PULL              (BIT(14) | BIT(15))

#define FREF_CLK_TYPE_BITS     0xfffffe7f
#define CLK_REQ_PRCM           0x100
#define FREF_CLK_POLARITY_BITS 0xfffff8ff
#define CLK_REQ_OUTN_SEL       0x700

#define WU_COUNTER_PAUSE_VAL 0x3FF

/*                                        */
#define SYS_CLK_CFG_REG              0x2200
/*                             */
#define MCS_PLL_CLK_SEL_FREF         BIT(0)
/*                             */
#define WL_CLK_REQ_TYPE_FREF         BIT(3)
#define WL_CLK_REQ_TYPE_PG2          (BIT(3) | BIT(2))
/*                             */
#define PRCM_CM_EN_MUX_WLAN_FREF     BIT(4)

#define TCXO_ILOAD_INT_REG           0x2264
#define TCXO_CLK_DETECT_REG          0x2266

#define TCXO_DET_FAILED              BIT(4)

#define FREF_ILOAD_INT_REG           0x2084
#define FREF_CLK_DETECT_REG          0x2086
#define FREF_CLK_DETECT_FAIL         BIT(4)

/*                                               */
#define WL_SPARE_REG                 0x2320
#define WL_SPARE_VAL                 BIT(2)
/*                                                  */
#define WL_SPARE_MASK_8526           (BIT(6) | BIT(5) | BIT(3))

#define PLL_LOCK_COUNTERS_REG        0xD8C
#define PLL_LOCK_COUNTERS_COEX       0x0F
#define PLL_LOCK_COUNTERS_MCS        0xF0
#define MCS_PLL_OVERRIDE_REG         0xD90
#define MCS_PLL_CONFIG_REG           0xD92
#define MCS_SEL_IN_FREQ_MASK         0x0070
#define MCS_SEL_IN_FREQ_SHIFT        4
#define MCS_PLL_CONFIG_REG_VAL       0x73
#define MCS_PLL_ENABLE_HP            (BIT(0) | BIT(1))

#define MCS_PLL_M_REG                0xD94
#define MCS_PLL_N_REG                0xD96
#define MCS_PLL_M_REG_VAL            0xC8
#define MCS_PLL_N_REG_VAL            0x07

#define SDIO_IO_DS                   0xd14

/*                                   */
enum {
	HCI_IO_DS_8MA = 0,
	HCI_IO_DS_4MA = 1, /*         */
	HCI_IO_DS_6MA = 2,
	HCI_IO_DS_2MA = 3,
};

/*                                            */

/*
                                                 
                                               
                                    
                                        
 */
#define WL12XX_INTR_TRIG_CMD		BIT(0)

/*
                                             
                                                
                                             
           
 */
#define WL12XX_INTR_TRIG_EVENT_ACK	BIT(1)

/*                                               
                                                
                                            
                                                 */
#define HI_CFG_UART_ENABLE          0x00000004
#define HI_CFG_RST232_ENABLE        0x00000008
#define HI_CFG_CLOCK_REQ_SELECT     0x00000010
#define HI_CFG_HOST_INT_ENABLE      0x00000020
#define HI_CFG_VLYNQ_OUTPUT_ENABLE  0x00000040
#define HI_CFG_HOST_INT_ACTIVE_LOW  0x00000080
#define HI_CFG_UART_TX_OUT_GPIO_15  0x00000100
#define HI_CFG_UART_TX_OUT_GPIO_14  0x00000200
#define HI_CFG_UART_TX_OUT_GPIO_7   0x00000400

#define HI_CFG_DEF_VAL              \
	(HI_CFG_UART_ENABLE |        \
	HI_CFG_RST232_ENABLE |      \
	HI_CFG_CLOCK_REQ_SELECT |   \
	HI_CFG_HOST_INT_ENABLE)

#define WL127X_REG_FUSE_DATA_2_1	0x050a
#define WL128X_REG_FUSE_DATA_2_1	0x2152
#define PG_VER_MASK			0x3c
#define PG_VER_OFFSET			2

#define WL127X_PG_MAJOR_VER_MASK	0x3
#define WL127X_PG_MAJOR_VER_OFFSET	0x0
#define WL127X_PG_MINOR_VER_MASK	0xc
#define WL127X_PG_MINOR_VER_OFFSET	0x2

#define WL128X_PG_MAJOR_VER_MASK	0xc
#define WL128X_PG_MAJOR_VER_OFFSET	0x2
#define WL128X_PG_MINOR_VER_MASK	0x3
#define WL128X_PG_MINOR_VER_OFFSET	0x0

#define WL127X_PG_GET_MAJOR(pg_ver) ((pg_ver & WL127X_PG_MAJOR_VER_MASK) >> \
				     WL127X_PG_MAJOR_VER_OFFSET)
#define WL127X_PG_GET_MINOR(pg_ver) ((pg_ver & WL127X_PG_MINOR_VER_MASK) >> \
				     WL127X_PG_MINOR_VER_OFFSET)
#define WL128X_PG_GET_MAJOR(pg_ver) ((pg_ver & WL128X_PG_MAJOR_VER_MASK) >> \
				     WL128X_PG_MAJOR_VER_OFFSET)
#define WL128X_PG_GET_MINOR(pg_ver) ((pg_ver & WL128X_PG_MINOR_VER_MASK) >> \
				     WL128X_PG_MINOR_VER_OFFSET)

#define WL12XX_REG_FUSE_BD_ADDR_1	0x00310eb4
#define WL12XX_REG_FUSE_BD_ADDR_2	0x00310eb8

#endif
