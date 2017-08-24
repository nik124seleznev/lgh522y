/* include/linux/logger.h
 *
 * Copyright (C) 2007-2008 Google, Inc.
 * Author: Robert Love <rlove@android.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef _LINUX_LOGGER_H
#define _LINUX_LOGGER_H

#include <linux/types.h>
#include <linux/ioctl.h>

/* 
                                                                                     
                                  
                                                          
                                           
                                          
                                                                
                                                                
                                         
  
                                                                 
                                                                     
                                     
 */
struct user_logger_entry_compat {
	__u16		len;
	__u16		__pad;
	__s32		pid;
	__s32		tid;
	__s32		sec;
	__s32		nsec;
	char		msg[0];
};

/* 
                                                                         
                                  
                                            
                                           
                                          
                                                                
                                                                
                                 
                                         
  
                                                       
                                                                       
                              
 */
struct logger_entry {
	__u16		len;
	__u16		hdr_size;
	__s32		pid;
	__s32		tid;
	__s32		sec;
	__s32		nsec;
    __s32       tz;         /*         */
	kuid_t		euid;
	char		msg[0];
};

/*
                                            
                                      
*/
#ifdef CONFIG_SMP
/*                                                                     */
#ifndef __MAIN_BUF_SIZE
#define __MAIN_BUF_SIZE 256*1024
#endif

#ifndef __EVENTS_BUF_SIZE
#define __EVENTS_BUF_SIZE 256*1024
#endif

#ifndef __RADIO_BUF_SIZE
#define __RADIO_BUF_SIZE 256*1024
#endif

#ifndef __SYSTEM_BUF_SIZE
#define __SYSTEM_BUF_SIZE 256*1024
#endif
#else
#ifndef __MAIN_BUF_SIZE
#define __MAIN_BUF_SIZE 256*1024
#endif

#ifndef __EVENTS_BUF_SIZE
#define __EVENTS_BUF_SIZE 256*1024 
#endif

#ifndef __RADIO_BUF_SIZE
#define __RADIO_BUF_SIZE 64*1024
#endif

#ifndef __SYSTEM_BUF_SIZE
#define __SYSTEM_BUF_SIZE 64*1024
#endif
#endif

#define LOGGER_LOG_RADIO	"log_radio"	/*                        */
#define LOGGER_LOG_EVENTS	"log_events"	/*                        */
#define LOGGER_LOG_SYSTEM	"log_system"	/*                           */
#define LOGGER_LOG_MAIN		"log_main"	/*                 */

#define LOGGER_ENTRY_MAX_PAYLOAD	4076

#define __LOGGERIO	0xAE

#define LOGGER_GET_LOG_BUF_SIZE		_IO(__LOGGERIO, 1) /*             */
#define LOGGER_GET_LOG_LEN		_IO(__LOGGERIO, 2) /*              */
#define LOGGER_GET_NEXT_ENTRY_LEN	_IO(__LOGGERIO, 3) /*                */
#define LOGGER_FLUSH_LOG		_IO(__LOGGERIO, 4) /*           */
#define LOGGER_GET_VERSION		_IO(__LOGGERIO, 5) /*             */
#define LOGGER_SET_VERSION		_IO(__LOGGERIO, 6) /*             */
#define LOGGER_SET_INTERVAL     _IO(__LOGGERIO, 101)    /*                  */
#define LOGGER_SET_TIMER        _IO(__LOGGERIO, 102)    /*              */
#endif /*                 */
