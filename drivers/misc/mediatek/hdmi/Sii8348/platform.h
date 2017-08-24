/*

SiI8348 Linux Driver

Copyright (C) 2013 Silicon Image, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation version 2.
This program is distributed AS-IS WITHOUT ANY WARRANTY of any
kind, whether express or implied; INCLUDING without the implied warranty
of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE or NON-INFRINGEMENT.  See 
the GNU General Public License for more details at http://www.gnu.org/licenses/gpl-2.0.html.             

*/

#if !defined(PLATFORM_H)
#define PLATFORM_H
 
#define DEVICE_ID_8348				0x8348
#define DEVICE_ID_8346				0x8346

#define DEBUG_I2C_WRITE				1
#define DEBUG_I2C_READ				0
#define MAX_DEBUG_TRANSFER_SIZE			16

#define LCD_TEXT_LENGTH_MAX	16

enum dbg_msg_level {
	DBG_MSG_LEVEL_ERR = 0
	,DBG_MSG_LEVEL_WARN
	,DBG_MSG_LEVEL_INFO
	,DBG_MSG_LEVEL_GPIO
	,DBG_MSG_LEVEL_EDID_READ
	,DBG_MSG_LEVEL_EDID_INFO
};


typedef enum
{
    HAL_RET_SUCCESS,			
    HAL_RET_FAILURE,			
    HAL_RET_PARAMETER_ERROR,	
    HAL_RET_NO_DEVICE,			
    HAL_RET_DEVICE_NOT_OPEN,	
    HAL_RET_NOT_INITIALIZED,	
    HAL_RET_OUT_OF_RESOURCES,	
    HAL_RET_TIMEOUT,			
    HAL_RET_ALREADY_INITIALIZED 
} halReturn_t;


#define DEBUG
#if defined(DEBUG)

void print_formatted_debug_msg(int level,
		char *file_spec, const char *func_name,
		int line_num, 
		char *fmt, ...);

void dump_i2c_transfer(void *context, u8 page, u8 offset,
		u16 count, u8 *values, bool write);
/*
                                                         
                                                   
                                 
                
*/
#define MHL_TX_EDID_READ(context,fmt,arg...) 					\
	print_formatted_debug_msg(DBG_MSG_LEVEL_EDID_READ,			\
			NULL, __func__, __LINE__,				\
			fmt, ## arg);

#define MHL_TX_EDID_INFO(context,fmt,arg...) 					\
	print_formatted_debug_msg(DBG_MSG_LEVEL_EDID_INFO,			\
			NULL, __func__, __LINE__,				\
			fmt, ## arg);

#define MHL_TX_DBG_INFO(driver_context, fmt, arg...)				\
	print_formatted_debug_msg(DBG_MSG_LEVEL_GPIO,				\
			NULL, __func__, __LINE__,				\
			fmt, ## arg);

#define MHL_TX_DBG_WARN(driver_context, fmt, arg...) 				\
	print_formatted_debug_msg(DBG_MSG_LEVEL_WARN,				\
			NULL, __func__, __LINE__,				\
			fmt, ## arg);

#define MHL_TX_DBG_ERR(driver_context, fmt, arg...)				\
	print_formatted_debug_msg(DBG_MSG_LEVEL_ERR,				\
			NULL, __func__, __LINE__,				\
			fmt, ## arg);

#define DUMP_I2C_TRANSFER(context, page, offset, count, values, write_flag)	\
	//                                                                    

#define DEBUG_PRINT_WRAPPER(...)						\
	print_formatted_debug_msg(DBG_MSG_LEVEL_ERR,				\
			__FILE__, __func__,					\
			__LINE__, __VA_ARGS__);

#define APP_DEBUG_PRINT(x) 			DEBUG_PRINT_WRAPPER x
#define PP_DEBUG_PRINT(x)			DEBUG_PRINT_WRAPPER x
#define PIXCLK_DEBUG_PRINT(x)			DEBUG_PRINT_WRAPPER x
#define ERROR_DEBUG_PRINT(x)			DEBUG_PRINT_WRAPPER x
#define TX_EDID_PRINT(x)			DEBUG_PRINT_WRAPPER x
#define TX_DEBUG_PRINT(x)			DEBUG_PRINT_WRAPPER x
#define TX_PRUNE_PRINT(x)			DEBUG_PRINT_WRAPPER x
#define EDID_DEBUG_PRINT(x)			DEBUG_PRINT_WRAPPER x
#define TX_EDID_PRINT_SIMPLE(x)			DEBUG_PRINT_WRAPPER x

#else

#define MHL_TX_DBG_INFO(driver_context, fmt, ...)
#define MHL_TX_DBG_WARN(driver_context, fmt, ...)
#define MHL_TX_DBG_ERR(driver_context, fmt, ...)

#define DUMP_I2C_TRANSFER(context, page, offset, count, values, write_flag)

#define APP_DEBUG_PRINT(x)
#define PP_DEBUG_PRINT(x)
#define PIXCLK_DEBUG_PRINT(x)
#define ERROR_DEBUG_PRINT(x)
#define TX_EDID_PRINT(x)
#define TX_DEBUG_PRINT(x)
#define TX_PRUNE_PRINT(x)
#define EDID_DEBUG_PRINT(x)
#define TX_EDID_PRINT_SIMPLE(x)

#endif

enum vbus_power_state {
	VBUS_OFF,
	VBUS_ON
};

void mhl_tx_vbus_control(enum vbus_power_state power_state);
/*
                         
                    
                
  
                              
             
                      
                                           
                            
 
              
  
*/
//                                                            

//                               
bool is_reset_on_exit_requested(void);

int mhl_tx_write_reg(void *drv_context, u8 page, u8 offset, u8 value);
int mhl_tx_read_reg(void *drv_context, u8 page, u8 offset);
int mhl_tx_write_reg_block(void *drv_context, u8 page, u8 offset, u16 count, u8 *values);
int mhl_tx_read_reg_block(void *drv_context, u8 page, u8 offset, u8 count, u8 *values);
int mhl_tx_modify_reg(void *drv_context, u8 page, u8 offset, u8 mask, u8 value);

//                                
//                                
//                                                   

int is_interrupt_asserted(void);
//                                                  
//                     
//                      
//                                                                                                    
//                                                                                                          
/*
              

             
         
                 
                  
        
             
             
        

            
        
           
           
           
            
            
               

                 
            
            
            
            
            
               
               

              
              
          
      
         
         
              
              

                     
                 
           
              
              
        
         
           

          
              

            
              
*/
#endif /*                         */
