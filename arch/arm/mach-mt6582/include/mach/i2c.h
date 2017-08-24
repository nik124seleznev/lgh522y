/*
* Copyright (C) 2011-2014 MediaTek Inc.
* 
* This program is free software: you can redistribute it and/or modify it under the terms of the 
* GNU General Public License version 2 as published by the Free Software Foundation.
* 
* This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
* without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
* See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with this program.
* If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __ARCH_I2C_H_
#define __ARCH_I2C_H_

//                             

/*                                                                  */
#define I2C_CLK_WRAPPER_RATE	24000

#if (defined(CONFIG_MT8135_FPGA))
#define	CONFIG_MT_I2C_FPGA_ENABLE
#endif

#if (defined(CONFIG_MT_I2C_FPGA_ENABLE))
#define I2C_CLK_RATE	12000	/*                                 */
#else
#define I2C_CLK_RATE	I2C_CLK_WRAPPER_RATE
#endif
/*
      
                                    
  
*/
struct mt_i2c_data {
	u16 pdn;	/*              */
	//                               
	u16 speed;	/*                  */
	//                                        
	u32 flags;
	//                                              
	//                                                  
	//                                          
	//               
};

struct mt_i2c_msg {
	u16 addr;		/*                                      */
	u16 flags;
#define I2C_M_TEN		0x0010	/*                                */
#define I2C_M_RD		0x0001	/*                                 */
#define I2C_M_STOP		0x8000	/*                               */
#define I2C_M_NOSTART		0x4000	/*                     */
#define I2C_M_REV_DIR_ADDR	0x2000	/*                               */
#define I2C_M_IGNORE_NAK	0x1000	/*                               */
#define I2C_M_NO_RD_ACK		0x0800	/*                               */
#define I2C_M_RECV_LEN		0x0400	/*                                    */
	u16 len;		/*                                      */
	u8 *buf;		/*                                      */
	u32 timing;
	u32 ext_flag;
};

#define I2C_A_FILTER_MSG	0x8000	/*                              */
#define I2C_A_CHANGE_TIMING	0x4000	/*                              */
#define I2C_MASK_FLAG	(0x00ff)
#define I2C_DMA_FLAG	(0x2000)
#define I2C_WR_FLAG		(0x1000)
#define I2C_RS_FLAG		(0x0800)
#define I2C_HS_FLAG   (0x0400)
#define I2C_ENEXT_FLAG (0x0200)
#define I2C_DISEXT_FLAG (0x0000)
#define I2C_POLL_FLAG (0x4000)
#define I2C_CH2_FLAG	(0x8000)

#define I2C_PUSHPULL_FLAG (0x00000002)
#define I2C_3DCAMERA_FLAG (0x00000004)
#define I2C_DIRECTION_FLAG (0x00000008)

int mtk_i2c_master_send(const struct i2c_client *client,
	const char *buf, int count, u32 ext_flag,u32 timing);

int mtk_i2c_master_recv(
	const struct i2c_client *client, char *buf, int count, u32 ext_flag,u32 timing);
int mtk_i2c_transfer(
	struct i2c_adapter *adap, struct mt_i2c_msg *msgs, int num);


#endif /*               */
