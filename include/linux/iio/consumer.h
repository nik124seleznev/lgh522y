/*
 * Industrial I/O in kernel consumer interface
 *
 * Copyright (c) 2011 Jonathan Cameron
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 */
#ifndef _IIO_INKERN_CONSUMER_H_
#define _IIO_INKERN_CONSUMER_H_

#include <linux/types.h>
#include <linux/iio/types.h>

struct iio_dev;
struct iio_chan_spec;
struct device;

/* 
                                                                         
                                                   
                                              
                                                   
 */
struct iio_channel {
	struct iio_dev *indio_dev;
	const struct iio_chan_spec *channel;
	void *data;
};

/* 
                                                                               
                                                            
                                                      
                                                        
                    
                                                                         
                                                           
                                         
 */
struct iio_channel *iio_channel_get(struct device *dev,
				    const char *consumer_channel);

/* 
                                                                        
                                      
 */
void iio_channel_release(struct iio_channel *chan);

/* 
                                                                    
                                     
  
                                                                      
                        
                                                                   
                                               
 */
struct iio_channel *iio_channel_get_all(struct device *dev);

/* 
                                                          
                                            
 */
void iio_channel_release_all(struct iio_channel *chan);

struct iio_cb_buffer;
/* 
                                                                     
                                   
                            
                                              
  
                                                                     
                                                                     
        
 */
struct iio_cb_buffer *iio_channel_get_all_cb(struct device *dev,
					     int (*cb)(u8 *data,
						       void *private),
					     void *private);
/* 
                                                                      
                                                       
 */
void iio_channel_release_all_cb(struct iio_cb_buffer *cb_buffer);

/* 
                                                                        
                                                  
 */
int iio_channel_start_all_cb(struct iio_cb_buffer *cb_buff);

/* 
                                                                          
                                                  
 */
void iio_channel_stop_all_cb(struct iio_cb_buffer *cb_buff);

/* 
                                                                         
                                                               
                 
  
                                                                     
                                                                        
                                                                        
                                      
 */
struct iio_channel
*iio_channel_cb_get_channels(const struct iio_cb_buffer *cb_buffer);

/* 
                                                     
                                     
                          
  
                                                               
                                                            
 */
int iio_read_channel_raw(struct iio_channel *chan,
			 int *val);

/* 
                                                                           
                                     
                          
  
                              
  
                                                                              
                                                                                
                                                                             
                                                                              
                                     
 */
int iio_read_channel_processed(struct iio_channel *chan, int *val);

/* 
                                                     
                                        
                                   
  
                                                
 */
int iio_get_channel_type(struct iio_channel *channel,
			 enum iio_chan_type *type);

/* 
                                                                
                                     
                                        
                                          
  
                                                              
                                                              
             
 */
int iio_read_channel_scale(struct iio_channel *chan, int *val,
			   int *val2);

/* 
                                                                             
                                    
                                
                                            
                                                       
  
                              
  
                                                                                
                                                                              
                                                                        
                                                                           
                                        
  
                                                                                
                                                                            
                                                                              
                                                   
 */
int iio_convert_raw_to_processed(struct iio_channel *chan, int raw,
	int *processed, unsigned int scale);

#endif
