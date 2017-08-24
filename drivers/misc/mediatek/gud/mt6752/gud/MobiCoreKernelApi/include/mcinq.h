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
#ifndef _MCINQ_H_
#define _MCINQ_H_

/*                                                                 */
#define MIN_NQ_ELEM	1	/*                                      */
#define MAX_NQ_ELEM	64	/*                                      */

/*                                         */
#define MIN_NQ_LEN	(MIN_NQ_ELEM * sizeof(notification))

/*                                         */
#define MAX_NQ_LEN	(MAX_NQ_ELEM * sizeof(notification))

/*
                                                                       
                                                 
 */
#define SID_MCP		0
/*                                                     */
#define SID_INVALID	0xffffffff

/*                              */
struct notification {
	uint32_t	session_id;	/*             */
	int32_t		payload;	/*                              */
};

/*
                              
                                           
                                                          
                                                          
                                            
 */
enum notification_payload {
	/*                                           */
	ERR_INVALID_EXIT_CODE	= -1,
	/*                                                            */
	ERR_SESSION_CLOSE	= -2,
	/*                                          */
	ERR_INVALID_OPERATION	= -3,
	/*                       */
	ERR_INVALID_SID		= -4,
	/*                        */
	ERR_SID_NOT_ACTIVE	= -5
};

/*
                                                
                                                           
 */
struct notification_queue_header {
	uint32_t	write_cnt;	/*                */
	uint32_t	read_cnt;	/*               */
	uint32_t	queue_size;	/*             */
};

/*
                                             
                                                               
                                                                  
                                                       
 */
struct notification_queue {
	/*               */
	struct notification_queue_header hdr;
	/*                        */
	struct notification notification[MIN_NQ_ELEM];
};

#endif /*           */
