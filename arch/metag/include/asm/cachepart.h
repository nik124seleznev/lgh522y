/*
 * Meta cache partition manipulation.
 *
 * Copyright 2010 Imagination Technologies Ltd.
 */

#ifndef _METAG_CACHEPART_H_
#define _METAG_CACHEPART_H_

/* 
                                              
 */
unsigned int get_dcache_size(void);

/* 
                                              
 */
unsigned int get_icache_size(void);

/* 
                                                             
  
                                                                    
 */
unsigned int get_global_dcache_size(void);

/* 
                                                             
  
                                                                    
 */
unsigned int get_global_icache_size(void);

/* 
                                                                             
                                              
                                 
  
 */
void check_for_cache_aliasing(int thread_id);

#endif
