/*
 *
 * fci_ringbuffer.h: ring buffer implementation for the fci driver
 *
 * Copyright (C) 2003 Oliver Endriss
 * Copyright (C) 2004 Andrew de Quincey
 *
 * based on code originally found in av7110.c & fci_ci.c:
 * Copyright (C) 1999-2003 Ralph Metzler & Marcus Metzler
 *                         for convergence integrated media GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef _FCI_RINGBUFFER_H_
#define _FCI_RINGBUFFER_H_

#include <linux/spinlock.h>
#include <linux/wait.h>

struct fci_ringbuffer {
    u8               *data;
    ssize_t           size;
    ssize_t           pread;
    ssize_t           pwrite;
    int               error;

    wait_queue_head_t queue;
    spinlock_t        lock;
};

#define FCI_RINGBUFFER_PKTHDRSIZE 3


/*
         
         
                                                                               
                                                                              
                                        
  
                                   
                                        
                          
                                                              
           
              
  
                                                   
                                          
                         
                                                                          
           
              
  
                                                                      
                                        
                                                             
                                                      
                                                                 
                                                             
*/

/*                                        */
extern void fci_ringbuffer_init(struct fci_ringbuffer *rbuf, void *data, size_t len);

/*                              */
extern int fci_ringbuffer_empty(struct fci_ringbuffer *rbuf);

/*                                               */
extern ssize_t fci_ringbuffer_free(struct fci_ringbuffer *rbuf);

/*                                                  */
extern ssize_t fci_ringbuffer_avail(struct fci_ringbuffer *rbuf);


/*
                                                                 
                                            
*/
extern void fci_ringbuffer_reset(struct fci_ringbuffer *rbuf);


/*                        */
/*                        */
/*              */
extern void fci_ringbuffer_flush(struct fci_ringbuffer *rbuf);

/*                                                                */
extern void fci_ringbuffer_flush_spinlock_wakeup(struct fci_ringbuffer *rbuf);

/*                                   */
#define FCI_RINGBUFFER_PEEK(rbuf,offs)    \
            (rbuf)->data[((rbuf)->pread+(offs))%(rbuf)->size]

/*                                 */
#define FCI_RINGBUFFER_SKIP(rbuf,num)    \
            (rbuf)->pread=((rbuf)->pread+(num))%(rbuf)->size

/*
                                               
                                                          
                                                 
*/
extern ssize_t fci_ringbuffer_read_user(struct fci_ringbuffer *rbuf,
                   u8 __user *buf, size_t len);
extern void fci_ringbuffer_read(struct fci_ringbuffer *rbuf,
                   u8 *buf, size_t len);


/*                         */
/*                         */
/*                                  */
#define FCI_RINGBUFFER_WRITE_BYTE(rbuf,byte)    \
            { (rbuf)->data[(rbuf)->pwrite]=(byte); \
            (rbuf)->pwrite=((rbuf)->pwrite+1)%(rbuf)->size; }
/*
                                   
                                                          
                                                 
*/
extern ssize_t fci_ringbuffer_write(struct fci_ringbuffer *rbuf, const u8 *buf,
                    size_t len);


/* 
                                      
  
                                 
                         
                                                                 
                                                                 
 */
extern ssize_t fci_ringbuffer_pkt_write(struct fci_ringbuffer *rbuf, u8* buf,
                    size_t len);

/* 
                                                                                 
                                                                   
                                                                       
  
                               
                                                               
                                            
                                     
                                    
                                               
                                            
 */
extern ssize_t fci_ringbuffer_pkt_read_user(struct fci_ringbuffer *rbuf, size_t idx,
                       int offset, u8 __user *buf, size_t len);
extern ssize_t fci_ringbuffer_pkt_read(struct fci_ringbuffer *rbuf, size_t idx,
                       int offset, u8 *buf, size_t len);

/* 
                                          
  
                                
                                                               
 */
extern void fci_ringbuffer_pkt_dispose(struct fci_ringbuffer *rbuf, size_t idx);

/* 
                                                    
  
                               
                                                                       
                                                                                     
                                                                
 */
extern ssize_t fci_ringbuffer_pkt_next(struct fci_ringbuffer *rbuf, size_t idx, size_t* pktlen);


#endif /*                    */
