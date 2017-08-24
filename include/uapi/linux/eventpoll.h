/*
 *  include/linux/eventpoll.h ( Efficient event polling implementation )
 *  Copyright (C) 2001,...,2006	 Davide Libenzi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Davide Libenzi <davidel@xmailserver.org>
 *
 */

#ifndef _UAPI_LINUX_EVENTPOLL_H
#define _UAPI_LINUX_EVENTPOLL_H

/*               */
#include <linux/fcntl.h>
#include <linux/types.h>

/*                           */
#define EPOLL_CLOEXEC O_CLOEXEC

/*                                           */
#define EPOLL_CTL_ADD 1
#define EPOLL_CTL_DEL 2
#define EPOLL_CTL_MOD 3

/*
                                                                                
                                                         
  
                                                                                
                                                                         
            
  
                             
 */
#define EPOLLWAKEUP (1 << 29)

/*                                                           */
#define EPOLLONESHOT (1 << 30)

/*                                                                 */
#define EPOLLET (1 << 31)

/* 
                                                                    
                                                      
  
                                              
 */
#ifdef __x86_64__
#define EPOLL_PACKED __attribute__((packed))
#else
#define EPOLL_PACKED
#endif

struct epoll_event {
	__u32 events;
	__u64 data;
} EPOLL_PACKED;


#endif /*                         */
