/*                                                                           */
/*              */
/*                                                      */
/*              */
/*                                                                           */
/*   Copyright (C) 1995-2000 Simon G. Vogl

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
    MA 02110-1301 USA.							     */
/*                                                                           */

/*                                                               
                                   */

#ifndef _UAPI_LINUX_I2C_H
#define _UAPI_LINUX_I2C_H

#include <linux/types.h>

/* 
                                                                   
                                                                      
                                                               
                                    
                                                                      
                                                               
                                           
                                                                       
                                                                 
                                                                
                                                              
                                                               
                                                          
                                                                        
  
                                                                      
                                                                           
                                                                    
                                      
  
                                                                          
                                                                           
                                                                         
                                                                           
                                                                         
                                                                           
                                                                         
                                                                   
  
                                                                           
                                                                             
                                                                          
                                                                         
                                                                          
 */
struct i2c_msg {
	__u16 addr;	/*                 */
	__u16 flags;
#define I2C_M_TEN		0x0010	/*                                */
#define I2C_M_RD		0x0001	/*                                 */
#define I2C_M_STOP		0x8000	/*                               */
#define I2C_M_NOSTART		0x4000	/*                     */
#define I2C_M_REV_DIR_ADDR	0x2000	/*                               */
#define I2C_M_IGNORE_NAK	0x1000	/*                               */
#define I2C_M_NO_RD_ACK		0x0800	/*                               */
#define I2C_M_RECV_LEN		0x0400	/*                                    */
	__u16 len;		/*               */
	__u8 *buf;		/*                       */
	__u32 timing;			/*                       */
	__u32 ext_flag;
};

/*                                            */

#define I2C_FUNC_I2C			0x00000001
#define I2C_FUNC_10BIT_ADDR		0x00000002
#define I2C_FUNC_PROTOCOL_MANGLING	0x00000004 /*                       */
#define I2C_FUNC_SMBUS_PEC		0x00000008
#define I2C_FUNC_NOSTART		0x00000010 /*               */
#define I2C_FUNC_SMBUS_BLOCK_PROC_CALL	0x00008000 /*           */
#define I2C_FUNC_SMBUS_QUICK		0x00010000
#define I2C_FUNC_SMBUS_READ_BYTE	0x00020000
#define I2C_FUNC_SMBUS_WRITE_BYTE	0x00040000
#define I2C_FUNC_SMBUS_READ_BYTE_DATA	0x00080000
#define I2C_FUNC_SMBUS_WRITE_BYTE_DATA	0x00100000
#define I2C_FUNC_SMBUS_READ_WORD_DATA	0x00200000
#define I2C_FUNC_SMBUS_WRITE_WORD_DATA	0x00400000
#define I2C_FUNC_SMBUS_PROC_CALL	0x00800000
#define I2C_FUNC_SMBUS_READ_BLOCK_DATA	0x01000000
#define I2C_FUNC_SMBUS_WRITE_BLOCK_DATA 0x02000000
#define I2C_FUNC_SMBUS_READ_I2C_BLOCK	0x04000000 /*                      */
#define I2C_FUNC_SMBUS_WRITE_I2C_BLOCK	0x08000000 /*                      */

#define I2C_FUNC_SMBUS_BYTE		(I2C_FUNC_SMBUS_READ_BYTE | \
					 I2C_FUNC_SMBUS_WRITE_BYTE)
#define I2C_FUNC_SMBUS_BYTE_DATA	(I2C_FUNC_SMBUS_READ_BYTE_DATA | \
					 I2C_FUNC_SMBUS_WRITE_BYTE_DATA)
#define I2C_FUNC_SMBUS_WORD_DATA	(I2C_FUNC_SMBUS_READ_WORD_DATA | \
					 I2C_FUNC_SMBUS_WRITE_WORD_DATA)
#define I2C_FUNC_SMBUS_BLOCK_DATA	(I2C_FUNC_SMBUS_READ_BLOCK_DATA | \
					 I2C_FUNC_SMBUS_WRITE_BLOCK_DATA)
#define I2C_FUNC_SMBUS_I2C_BLOCK	(I2C_FUNC_SMBUS_READ_I2C_BLOCK | \
					 I2C_FUNC_SMBUS_WRITE_I2C_BLOCK)

#define I2C_FUNC_SMBUS_EMUL		(I2C_FUNC_SMBUS_QUICK | \
					 I2C_FUNC_SMBUS_BYTE | \
					 I2C_FUNC_SMBUS_BYTE_DATA | \
					 I2C_FUNC_SMBUS_WORD_DATA | \
					 I2C_FUNC_SMBUS_PROC_CALL | \
					 I2C_FUNC_SMBUS_WRITE_BLOCK_DATA | \
					 I2C_FUNC_SMBUS_I2C_BLOCK | \
					 I2C_FUNC_SMBUS_PEC)

/*
                          
 */
#define I2C_SMBUS_BLOCK_MAX	32	/*                                */
union i2c_smbus_data {
	__u8 byte;
	__u16 word;
	__u8 block[I2C_SMBUS_BLOCK_MAX + 2]; /*                             */
			       /*                                           */
};

/*                                      */
#define I2C_SMBUS_READ	1
#define I2C_SMBUS_WRITE	0

/*                                                                
                                                                             */
#define I2C_SMBUS_QUICK		    0
#define I2C_SMBUS_BYTE		    1
#define I2C_SMBUS_BYTE_DATA	    2
#define I2C_SMBUS_WORD_DATA	    3
#define I2C_SMBUS_PROC_CALL	    4
#define I2C_SMBUS_BLOCK_DATA	    5
#define I2C_SMBUS_I2C_BLOCK_BROKEN  6
#define I2C_SMBUS_BLOCK_PROC_CALL   7		/*           */
#define I2C_SMBUS_I2C_BLOCK_DATA    8

#endif /*                   */
