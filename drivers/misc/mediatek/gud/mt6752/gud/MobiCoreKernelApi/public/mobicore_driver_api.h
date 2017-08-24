/*
 * Copyright (c) 2013 TRUSTONIC LIMITED
 * All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */
/*
                       
  
                                                                         
                                                   
 */
#ifndef _MOBICORE_DRIVER_API_H_
#define _MOBICORE_DRIVER_API_H_

#define __MC_CLIENT_LIB_API

#include "mcuuid.h"

/*
                                              
 */
enum mc_result {
	/*                          */
	MC_DRV_OK			= 0,
	/*                            */
	MC_DRV_NO_NOTIFICATION		= 1,
	/*                                                   */
	MC_DRV_ERR_NOTIFICATION		= 2,
	/*                           */
	MC_DRV_ERR_NOT_IMPLEMENTED	= 3,
	/*                              */
	MC_DRV_ERR_OUT_OF_RESOURCES	= 4,
	/*                               */
	MC_DRV_ERR_INIT			= 5,
	/*                */
	MC_DRV_ERR_UNKNOWN		= 6,
	/*                                  */
	MC_DRV_ERR_UNKNOWN_DEVICE	= 7,
	/*                                  */
	MC_DRV_ERR_UNKNOWN_SESSION	= 8,
	/*                                         */
	MC_DRV_ERR_INVALID_OPERATION	= 9,
	/*                                             */
	MC_DRV_ERR_INVALID_RESPONSE	= 10,
	/*                          */
	MC_DRV_ERR_TIMEOUT		= 11,
	/*                                     */
	MC_DRV_ERR_NO_FREE_MEMORY	= 12,
	/*                     */
	MC_DRV_ERR_FREE_MEMORY_FAILED	= 13,
	/*                                   */
	MC_DRV_ERR_SESSION_PENDING	= 14,
	/*                         */
	MC_DRV_ERR_DAEMON_UNREACHABLE	= 15,
	/*                                                           */
	MC_DRV_ERR_INVALID_DEVICE_FILE	= 16,
	/*                    */
	MC_DRV_ERR_INVALID_PARAMETER	= 17,
	/*                                     */
	MC_DRV_ERR_KERNEL_MODULE	= 18,
	/*                                                            */
	MC_DRV_ERR_BULK_MAPPING		= 19,
	/*                                                              */
	MC_DRV_ERR_BULK_UNMAPPING	= 20,
	/*                                             */
	MC_DRV_INFO_NOTIFICATION	= 21,
	/*                                  */
	MC_DRV_ERR_NQ_FAILED		= 22
};

/*
                          
 */
enum mc_driver_ctrl {
	/*                           */
	MC_CTRL_GET_VERSION		= 1
};

/*
                                                                             
                      
                                                                            
                                                                            
                        
 */
struct mc_session_handle {
	uint32_t session_id;		/*                     */
	uint32_t device_id;		/*                                  */
};

/*
                                                                        
                                                                     
                                                             
                                                                             
                                                                      
 */
struct mc_bulk_map {
	/*                                                                   
                                                         */
	uint32_t secure_virt_addr;
	uint32_t secure_virt_len;	/*                                  */
};

/*                       */
#define MC_DEVICE_ID_DEFAULT	0
/*                                         */
#define MC_INFINITE_TIMEOUT	((int32_t)(-1))
/*                                       */
#define MC_NO_TIMEOUT		0
/*                              */
#define MC_MAX_TCI_LEN		0x100000

/* 
                                                                 
                                                              
                                                       
  
                                                                           
                                                                          
                                                                            
  
                
                                                
                                                      
                                                      
                                               
                                                                    
                       
 */
__MC_CLIENT_LIB_API enum mc_result mc_open_device(uint32_t device_id);

/* 
                                                                 
                                                   
  
                                                                       
                                                         
                                                           
  
                                                     
  
                
                                                
                                                  
                                                      
                                                            
 */
__MC_CLIENT_LIB_API enum mc_result mc_close_device(uint32_t device_id);

/* 
                                                        
                                                           
                                            
                                                        
                                                             
                      
  
                                                                                
  
                                                                              
                    
  
                                                                               
           
  
                                                                           
  
                
                                                
                                                         
                                                  
                                                                   
                                                 
 */
__MC_CLIENT_LIB_API enum mc_result mc_open_session(
	struct mc_session_handle *session, const struct mc_uuid_t *uuid,
	uint8_t *tci, uint32_t tci_len);

/* 
                                                 
                                   
  
                                                                       
                           
  
                                                
  
                
                                                
                                                         
                                                    
                                                             
                                                            
                                                                   
 */
__MC_CLIENT_LIB_API enum mc_result mc_close_session(
	struct mc_session_handle *session);

/* 
                                  
                                         
  
                                                               
                                                                   
                                                                       
  
                                         
  
                
                                                
                                                         
                                                    
                                                             
 */
__MC_CLIENT_LIB_API enum mc_result mc_notify(struct mc_session_handle *session);

/* 
                                                    
                                                                
                                          
                                                        
                                            
  
                                                                         
                                                                                
                      
  
                                                                            
                                                                               
                             
  
                                          
  
                                                                           
         
  
                
                                                
                                                       
                                                           
                                         
                                  
                                                              
                                                   
                                                    
                                                             
 */
__MC_CLIENT_LIB_API enum mc_result mc_wait_notification(
	struct mc_session_handle *session, int32_t timeout);

/* 
                                                                   
                                                                    
                                                               
                               
                                       
                                                           
                                                                
                  
  
                                                                            
       
                                                                        
                       
  
                                                          
  
                                            
  
                
                                                
                                                   
                                                  
                                                          
                                         
              
 */
__MC_CLIENT_LIB_API enum mc_result mc_malloc_wsm(
	uint32_t  device_id,
	uint32_t  align,
	uint32_t  len,
	uint8_t   **wsm,
	uint32_t  wsm_flags
);

/* 
                                                             
                                                         
                                          
  
                                                                          
                                                                            
                                                    
  
                
                                                
                                                   
                                                       
                                            
 */
__MC_CLIENT_LIB_API enum mc_result mc_free_wsm(uint32_t device_id,
					       uint8_t *wsm);

/* 
                                                                          
                                       
                                                                  
                                                      
                                           
                                                               
                                                       
                     
                                          
                                                                 
                                            
  
                                                                        
                                                                         
                                                                              
                            
  
                                                                 
                                           
  
                
                                                
                                                   
                                                    
                                                             
                                                            
                                                                 
                                         
 */
__MC_CLIENT_LIB_API enum mc_result mc_map(
	struct mc_session_handle *session, void	*buf, uint32_t len,
	struct mc_bulk_map *map_info);

/* 
                                                                               
                                             
                                                                  
                                                          
                                            
                                                               
                                                            
                                                                 
                                           
  
                                                                         
  
                                                                        
                                                  
  
                                                             
  
                
                                                
                                                   
                                                    
                                                             
                                                            
                                                            
                                   
 */
__MC_CLIENT_LIB_API enum mc_result mc_unmap(
	struct mc_session_handle *session, void *buf,
	struct mc_bulk_map *map_info);

/* 
                                                                             
                                        
                                                                  
                   
                                                                   
                                                       
                                                   
                
  
                                                           
  
                
                                                
                                                   
                                                    
                                                             
 */
__MC_CLIENT_LIB_API enum mc_result mc_get_session_error_code(
	struct mc_session_handle *session, int32_t *last_error);

#endif /*                         */
