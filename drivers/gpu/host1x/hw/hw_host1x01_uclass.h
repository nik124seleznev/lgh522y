/*
 * Copyright (c) 2012-2013, NVIDIA Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

 /*
                                            
   
                                                          
   
                                                                            
   
                                                                            
   
                                                                              
                                                                            
                                                                       
                         
   
                                                                             
                                                                             
                         
   
                                                                                
                                                                            
                                                                 
   
                                                                              
                                                                        
                                                                             
                                                                    
   
                                                                            
                                                                         
                                                                             
                            
  */

#ifndef __hw_host1x_uclass_host1x_h__
#define __hw_host1x_uclass_host1x_h__

static inline u32 host1x_uclass_incr_syncpt_r(void)
{
	return 0x0;
}
#define HOST1X_UCLASS_INCR_SYNCPT \
	host1x_uclass_incr_syncpt_r()
static inline u32 host1x_uclass_incr_syncpt_cond_f(u32 v)
{
	return (v & 0xff) << 8;
}
#define HOST1X_UCLASS_INCR_SYNCPT_COND_F(v) \
	host1x_uclass_incr_syncpt_cond_f(v)
static inline u32 host1x_uclass_incr_syncpt_indx_f(u32 v)
{
	return (v & 0xff) << 0;
}
#define HOST1X_UCLASS_INCR_SYNCPT_INDX_F(v) \
	host1x_uclass_incr_syncpt_indx_f(v)
static inline u32 host1x_uclass_wait_syncpt_r(void)
{
	return 0x8;
}
#define HOST1X_UCLASS_WAIT_SYNCPT \
	host1x_uclass_wait_syncpt_r()
static inline u32 host1x_uclass_wait_syncpt_indx_f(u32 v)
{
	return (v & 0xff) << 24;
}
#define HOST1X_UCLASS_WAIT_SYNCPT_INDX_F(v) \
	host1x_uclass_wait_syncpt_indx_f(v)
static inline u32 host1x_uclass_wait_syncpt_thresh_f(u32 v)
{
	return (v & 0xffffff) << 0;
}
#define HOST1X_UCLASS_WAIT_SYNCPT_THRESH_F(v) \
	host1x_uclass_wait_syncpt_thresh_f(v)
static inline u32 host1x_uclass_wait_syncpt_base_r(void)
{
	return 0x9;
}
#define HOST1X_UCLASS_WAIT_SYNCPT_BASE \
	host1x_uclass_wait_syncpt_base_r()
static inline u32 host1x_uclass_wait_syncpt_base_indx_f(u32 v)
{
	return (v & 0xff) << 24;
}
#define HOST1X_UCLASS_WAIT_SYNCPT_BASE_INDX_F(v) \
	host1x_uclass_wait_syncpt_base_indx_f(v)
static inline u32 host1x_uclass_wait_syncpt_base_base_indx_f(u32 v)
{
	return (v & 0xff) << 16;
}
#define HOST1X_UCLASS_WAIT_SYNCPT_BASE_BASE_INDX_F(v) \
	host1x_uclass_wait_syncpt_base_base_indx_f(v)
static inline u32 host1x_uclass_wait_syncpt_base_offset_f(u32 v)
{
	return (v & 0xffff) << 0;
}
#define HOST1X_UCLASS_WAIT_SYNCPT_BASE_OFFSET_F(v) \
	host1x_uclass_wait_syncpt_base_offset_f(v)
static inline u32 host1x_uclass_load_syncpt_base_base_indx_f(u32 v)
{
	return (v & 0xff) << 24;
}
#define HOST1X_UCLASS_LOAD_SYNCPT_BASE_BASE_INDX_F(v) \
	host1x_uclass_load_syncpt_base_base_indx_f(v)
static inline u32 host1x_uclass_load_syncpt_base_value_f(u32 v)
{
	return (v & 0xffffff) << 0;
}
#define HOST1X_UCLASS_LOAD_SYNCPT_BASE_VALUE_F(v) \
	host1x_uclass_load_syncpt_base_value_f(v)
static inline u32 host1x_uclass_incr_syncpt_base_base_indx_f(u32 v)
{
	return (v & 0xff) << 24;
}
#define HOST1X_UCLASS_INCR_SYNCPT_BASE_BASE_INDX_F(v) \
	host1x_uclass_incr_syncpt_base_base_indx_f(v)
static inline u32 host1x_uclass_incr_syncpt_base_offset_f(u32 v)
{
	return (v & 0xffffff) << 0;
}
#define HOST1X_UCLASS_INCR_SYNCPT_BASE_OFFSET_F(v) \
	host1x_uclass_incr_syncpt_base_offset_f(v)
static inline u32 host1x_uclass_indoff_r(void)
{
	return 0x2d;
}
#define HOST1X_UCLASS_INDOFF \
	host1x_uclass_indoff_r()
static inline u32 host1x_uclass_indoff_indbe_f(u32 v)
{
	return (v & 0xf) << 28;
}
#define HOST1X_UCLASS_INDOFF_INDBE_F(v) \
	host1x_uclass_indoff_indbe_f(v)
static inline u32 host1x_uclass_indoff_autoinc_f(u32 v)
{
	return (v & 0x1) << 27;
}
#define HOST1X_UCLASS_INDOFF_AUTOINC_F(v) \
	host1x_uclass_indoff_autoinc_f(v)
static inline u32 host1x_uclass_indoff_indmodid_f(u32 v)
{
	return (v & 0xff) << 18;
}
#define HOST1X_UCLASS_INDOFF_INDMODID_F(v) \
	host1x_uclass_indoff_indmodid_f(v)
static inline u32 host1x_uclass_indoff_indroffset_f(u32 v)
{
	return (v & 0xffff) << 2;
}
#define HOST1X_UCLASS_INDOFF_INDROFFSET_F(v) \
	host1x_uclass_indoff_indroffset_f(v)
static inline u32 host1x_uclass_indoff_rwn_read_v(void)
{
	return 1;
}
#define HOST1X_UCLASS_INDOFF_INDROFFSET_F(v) \
	host1x_uclass_indoff_indroffset_f(v)
#endif
