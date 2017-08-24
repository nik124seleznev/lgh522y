/*
 * Copyright (c) 2012 Neratec Solutions AG
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef DFS_PRI_DETECTOR_H
#define DFS_PRI_DETECTOR_H

#include <linux/list.h>

/* 
                                                            
                   
                                                 
                                      
                                            
                                                                
                                                
                                              
                                                                             
 */
struct pri_sequence {
	struct list_head head;
	u32 pri;
	u32 dur;
	u32 count;
	u32 count_falses;
	u64 first_ts;
	u64 last_ts;
	u64 deadline_ts;
};

/* 
                                                                        
                      
                                                                              
                                                       
                                                
                                                                       
                                                          
                                              
                                    
                                                    
                                                                       
 */
struct pri_detector {
	void (*exit)     (struct pri_detector *de);
	struct pri_sequence *
	     (*add_pulse)(struct pri_detector *de, struct pulse_event *e);
	void (*reset)    (struct pri_detector *de, u64 ts);

/*                            */
	const struct radar_detector_specs *rs;
	u64 last_ts;
	struct list_head sequences;
	struct list_head pulses;
	u32 count;
	u32 max_count;
	u32 window_size;
};

struct pri_detector *pri_detector_init(const struct radar_detector_specs *rs);

#endif /*                    */
