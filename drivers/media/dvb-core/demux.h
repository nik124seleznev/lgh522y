/*
 * demux.h
 *
 * Copyright (c) 2002 Convergence GmbH
 *
 * based on code:
 * Copyright (c) 2000 Nokia Research Center
 *                    Tampere, FINLAND
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#ifndef __DEMUX_H
#define __DEMUX_H

#include <linux/types.h>
#include <linux/errno.h>
#include <linux/list.h>
#include <linux/time.h>
#include <linux/dvb/dmx.h>

/*                                                                          */
/*                    */
/*                                                                          */

/*
                                                                          
 */

#ifndef DMX_MAX_FILTER_SIZE
#define DMX_MAX_FILTER_SIZE 18
#endif

/*
                                                                                    
 */

#ifndef DMX_MAX_SECTION_SIZE
#define DMX_MAX_SECTION_SIZE 4096
#endif
#ifndef DMX_MAX_SECFEED_SIZE
#define DMX_MAX_SECFEED_SIZE (DMX_MAX_SECTION_SIZE + 188)
#endif


/*
                                                              
 */

enum dmx_success {
  DMX_OK = 0, /*             */
  DMX_LENGTH_ERROR, /*                  */
  DMX_OVERRUN_ERROR, /*                              */
  DMX_CRC_ERROR, /*               */
  DMX_FRAME_ERROR, /*                       */
  DMX_FIFO_ERROR, /*                       */
  DMX_MISSED_ERROR /*                        */
} ;

/*                                                                          */
/*                     */
/*                                                                          */

/*                          */

#define TS_PACKET       1   /*                                                   */
#define	TS_PAYLOAD_ONLY 2   /*                                           
                                                       */
#define TS_DECODER      4   /*                                              */
#define TS_DEMUX        8   /*                                         
                                                  */

struct dmx_ts_feed {
	int is_filtering; /*                                            */
	struct dmx_demux *parent; /*              */
	void *priv; /*                                           */
	int (*set) (struct dmx_ts_feed *feed,
		    u16 pid,
		    int type,
		    enum dmx_ts_pes pes_type,
		    size_t circular_buffer_size,
		    struct timespec timeout);
	int (*start_filtering) (struct dmx_ts_feed* feed);
	int (*stop_filtering) (struct dmx_ts_feed* feed);
};

/*                                                                          */
/*                   */
/*                                                                          */

struct dmx_section_filter {
	u8 filter_value [DMX_MAX_FILTER_SIZE];
	u8 filter_mask [DMX_MAX_FILTER_SIZE];
	u8 filter_mode [DMX_MAX_FILTER_SIZE];
	struct dmx_section_feed* parent; /*              */
	void* priv; /*                                           */
};

struct dmx_section_feed {
	int is_filtering; /*                                            */
	struct dmx_demux* parent; /*              */
	void* priv; /*                                           */

	int check_crc;
	u32 crc_val;

	u8 *secbuf;
	u8 secbuf_base[DMX_MAX_SECFEED_SIZE];
	u16 secbufp, seclen, tsfeedp;

	int (*set) (struct dmx_section_feed* feed,
		    u16 pid,
		    size_t circular_buffer_size,
		    int check_crc);
	int (*allocate_filter) (struct dmx_section_feed* feed,
				struct dmx_section_filter** filter);
	int (*release_filter) (struct dmx_section_feed* feed,
			       struct dmx_section_filter* filter);
	int (*start_filtering) (struct dmx_section_feed* feed);
	int (*stop_filtering) (struct dmx_section_feed* feed);
};

/*                                                                          */
/*                    */
/*                                                                          */

typedef int (*dmx_ts_cb) ( const u8 * buffer1,
			   size_t buffer1_length,
			   const u8 * buffer2,
			   size_t buffer2_length,
			   struct dmx_ts_feed* source,
			   enum dmx_success success);

typedef int (*dmx_section_cb) (	const u8 * buffer1,
				size_t buffer1_len,
				const u8 * buffer2,
				size_t buffer2_len,
				struct dmx_section_filter * source,
				enum dmx_success success);

/*                                                                          */
/*               */
/*                                                                          */

enum dmx_frontend_source {
	DMX_MEMORY_FE,
	DMX_FRONTEND_0,
	DMX_FRONTEND_1,
	DMX_FRONTEND_2,
	DMX_FRONTEND_3,
	DMX_STREAM_0,    /*                                  */
	DMX_STREAM_1,
	DMX_STREAM_2,
	DMX_STREAM_3
};

struct dmx_frontend {
	struct list_head connectivity_list; /*                            
                                        
                  */
	enum dmx_frontend_source source;
};

/*                                                                          */
/*                 */
/*                                                                          */

/*
                                                             
 */

#define DMX_TS_FILTERING                        1
#define DMX_PES_FILTERING                       2
#define DMX_SECTION_FILTERING                   4
#define DMX_MEMORY_BASED_FILTERING              8    /*                   */
#define DMX_CRC_CHECKING                        16
#define DMX_TS_DESCRAMBLING                     32

/*
                                  
*/

/*
                                                           
                                                    
                                    
   
*/

#define DMX_FE_ENTRY(list) list_entry(list, struct dmx_frontend, connectivity_list)

struct dmx_demux {
	u32 capabilities;            /*                              */
	struct dmx_frontend* frontend;    /*                                  */
	void* priv;                  /*                                           */
	int (*open) (struct dmx_demux* demux);
	int (*close) (struct dmx_demux* demux);
	int (*write) (struct dmx_demux* demux, const char __user *buf, size_t count);
	int (*allocate_ts_feed) (struct dmx_demux* demux,
				 struct dmx_ts_feed** feed,
				 dmx_ts_cb callback);
	int (*release_ts_feed) (struct dmx_demux* demux,
				struct dmx_ts_feed* feed);
	int (*allocate_section_feed) (struct dmx_demux* demux,
				      struct dmx_section_feed** feed,
				      dmx_section_cb callback);
	int (*release_section_feed) (struct dmx_demux* demux,
				     struct dmx_section_feed* feed);
	int (*add_frontend) (struct dmx_demux* demux,
			     struct dmx_frontend* frontend);
	int (*remove_frontend) (struct dmx_demux* demux,
				struct dmx_frontend* frontend);
	struct list_head* (*get_frontends) (struct dmx_demux* demux);
	int (*connect_frontend) (struct dmx_demux* demux,
				 struct dmx_frontend* frontend);
	int (*disconnect_frontend) (struct dmx_demux* demux);

	int (*get_pes_pids) (struct dmx_demux* demux, u16 *pids);

	int (*get_caps) (struct dmx_demux* demux, struct dmx_caps *caps);

	int (*set_source) (struct dmx_demux* demux, const dmx_source_t *src);

	int (*get_stc) (struct dmx_demux* demux, unsigned int num,
			u64 *stc, unsigned int *base);
};

#endif /*                   */
