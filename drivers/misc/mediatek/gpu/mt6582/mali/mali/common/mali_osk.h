/*
 * Copyright (C) 2010-2013 ARM Limited. All rights reserved.
 * 
 * This program is free software and is provided to you under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation, and any use by you of this program is subject to the terms of such GNU licence.
 * 
 * A copy of the licence is included with the program, and can also be obtained from Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

/* 
                   
                                                                      
 */

#ifndef __MALI_OSK_H__
#define __MALI_OSK_H__

#include "mali_osk_types.h"
#include "mali_osk_specific.h"           /*                              */
#include "mali_osk_locks.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 
                                                      
  
     
 */

/* 
                                                                   
  
     
 */

/*                                                       
      */

#ifdef DEBUG
/*                                                                       
 */
#define MALI_DEBUG_ASSERT_LOCK_HELD(l) MALI_DEBUG_ASSERT(_mali_osk_lock_get_owner((_mali_osk_lock_debug_t *)l) == _mali_osk_get_tid());

/*                                                                   
 */
#else
#define MALI_DEBUG_ASSERT_LOCK_HELD(l) do {} while(0)
#endif

/*     */ /*                          */

/*                                     
      */

/*                                                           
  
                                                                       
                                    
  
                                                                
  
                              
  
                                                                               
  
                                                                             
                                                                         
                  
  
                                                                           
            
                                                                 
                                                                            
                                                                             
                
 */
#define _MALI_OSK_CONTAINER_OF(ptr, type, member) \
             ((type *)( ((char *)ptr) - offsetof(type,member) ))

/*                          
      */

/*                                                   
  
                                                          
 */
_mali_osk_errcode_t _mali_osk_wq_init(void);

/*                                                  
 */
void _mali_osk_wq_term(void);

/*                                      
  
                                                                       
                                                                     
  
                                                                     
                             
  
                                                                          
                         
 */
_mali_osk_wq_work_t *_mali_osk_wq_create_work( _mali_osk_wq_work_handler_t handler, void *data );

/*                                                                 
  
                                                                                
  
                                                                                                   
                                                                                                    
                                                                                                    
                  
  
                                                                                                    
                                                                                                   
                                                                                               
                                                                          
  
                                                                              
 */
_mali_osk_wq_work_t *_mali_osk_wq_create_work_high_pri( _mali_osk_wq_work_handler_t handler, void *data );

/*                             
  
                                                                          
                            
 */
void _mali_osk_wq_delete_work( _mali_osk_wq_work_t *work );

/*                             
  
                                                                                                   
                                
 */
void _mali_osk_wq_delete_work_nonflush( _mali_osk_wq_work_t *work );

/*                                                          
  
                                                                               
                                                                            
                                                                     
  
                                                                                
                                                                              
                                                                             
                                                                             
                                                                      
               
  
                                                                      
                                                                            
                                                                               
                                                                              
                                         
  
                                                                            
                                                                             
                                                                              
                                                   
  
                                                              
                                       
                                        
  
                                                                            
                                                                            
        
                                                                          
          
                                                                           
                       
  
                                                          
                                                                       
                                                                              
                     
  
                                                                              
                                                                           
                                                                              
                                                                             
                                   
  
                                                                               
                            
 */
void _mali_osk_wq_schedule_work( _mali_osk_wq_work_t *work );

/*                                                                        
  
                                                                        
                                                                        
  
                                                                            
  
                                                                             
                          
*/
void _mali_osk_wq_schedule_work_high_pri( _mali_osk_wq_work_t *work );

/*                             
  
                                                                         
                              
  
                                                                     
                                                                            
                                                              
  
 */
void _mali_osk_wq_flush(void);

/*                                              
  
                                                                       
                                                                          
                                         
  
                                                                             
                             
  
                                                                                           
                         
 */
_mali_osk_wq_delayed_work_t *_mali_osk_wq_delayed_create_work(_mali_osk_wq_work_handler_t handler, void *data);

/*                             
  
                                                                                                   
                                
 */
void _mali_osk_wq_delayed_delete_work_nonflush(_mali_osk_wq_delayed_work_t *work);

/*                                                               
  
                                                                              
                                            
  
                                               
 */
void _mali_osk_wq_delayed_cancel_work_async(_mali_osk_wq_delayed_work_t *work);

/*                                                        
  
                                                                                       
  
                                               
 */
void _mali_osk_wq_delayed_cancel_work_sync(_mali_osk_wq_delayed_work_t *work);

/*                                                         
  
                                                                      
             
  
                                                                                      
  
                             
                                                                      
 */
void _mali_osk_wq_delayed_schedule_work(_mali_osk_wq_delayed_work_t *work, u32 delay);

/*     */ /*                        */


/*                           
      */

/*                                               
  
                                                                                 
                                                                              
  
                                                                             
                                                                          
                                     
  
                                                                           
                                                                             
                                              
                                                                            
               
                                                                           
                                                                             
                                                     
                                                                             
                                                     
                                                                    
                                         
                                                              
                                                                              
                                                      
 */
_mali_osk_irq_t *_mali_osk_irq_init( u32 irqnum, _mali_osk_irq_uhandler_t uhandler, void *int_data, _mali_osk_irq_trigger_t trigger_func, _mali_osk_irq_ack_t ack_func, void *probe_data, const char *description );

/*                                              
  
                                                                          
                                                
  
                                                                  
                                                                         
                                                                                      
  
                                                                          
                                                   
 */
void _mali_osk_irq_term( _mali_osk_irq_t *irq );

/*     */ /*                         */


/*                              
      */

/*                                    
  
                                                                
  
                                            */
void _mali_osk_atomic_dec( _mali_osk_atomic_t *atom );

/*                                                      
  
                                           
                                          */
u32 _mali_osk_atomic_dec_return( _mali_osk_atomic_t *atom );

/*                                    
  
                                                                 
  
                                            */
void _mali_osk_atomic_inc( _mali_osk_atomic_t *atom );

/*                                                      
  
                                            */
u32 _mali_osk_atomic_inc_return( _mali_osk_atomic_t *atom );

/*                                     
  
                                                                          
               
  
                                           
                                                         
                                                             
                                  
 */
_mali_osk_errcode_t _mali_osk_atomic_init( _mali_osk_atomic_t *atom, u32 val );

/*                                            
  
                                                                             
                                                                           
                                
  
                                           
 */
u32 _mali_osk_atomic_read( _mali_osk_atomic_t *atom );

/*                                    
  
                                           
 */
void _mali_osk_atomic_term( _mali_osk_atomic_t *atom );

/*                                                                              
  
                                           
                                                        
                                              
 */
u32 _mali_osk_atomic_xchg( _mali_osk_atomic_t *atom, u32 val );
/*     */  /*                            */


/*                                                  
      */

/*                                          
  
                                                                           
                                                 
  
                                                                         
                                                                      
                                                   
  
                                                                            
                                                                              
                                                                              
                                               
  
                                                                               
                                             
  
                                                                         
                                     
  
                                          
                                   
                                                                             
 */
void *_mali_osk_calloc( u32 n, u32 size );

/*                         
  
                                                                     
                                        
  
                                                                         
                                                                      
                                                   
  
                                                                            
                                                                              
                                                                              
                                               
  
                                                                               
                                             
  
                                                                         
                                     
  
                                                  
                                          
                                                             
 */
void *_mali_osk_malloc( u32 size );

/*                     
  
                                                                         
                                                                     
                                                         
                            
  
                                                                              
                  
  
                                        
  
                                       
 */
void _mali_osk_free( void *ptr );

/*                         
  
                                                                     
                                        
  
                                                                                      
                               
  
                                                                            
                                                                              
                                                                              
                                               
  
                                                                               
                                             
  
                                                                         
                                     
  
                                                  
                                          
                                                             
 */
void *_mali_osk_valloc( u32 size );

/*                     
  
                                                                         
                                                                       
                                                         
  
                                                                              
                  
  
                                        
  
                                       
 */
void _mali_osk_vfree( void *ptr );

/*                       
  
                                                                          
                                            
  
                                                                        
  
                                                                         
          
                                                         
                                      
                                                      
 */
void *_mali_osk_memcpy( void *dst, const void *src, u32 len );

/*                      
  
                                                                         
                      
                                                   
                                                                                   
            
                                                   
                                                   
 */
void *_mali_osk_memset( void *s, u32 c, u32 n );
/*     */ /*                            */


/*                                              
  
                                                                  
  
                                                                  
                                                            
                                                                
                                                      
                          
  
                                                                               
                                                   
 */
mali_bool _mali_osk_mem_check_allocated( u32 max_allocated );


/*                                        
      */

/*                               
  
                                                                                          
                                       
 */
void _mali_osk_mem_barrier( void );

/*                                     
  
                                                                                     
                              
 */
void _mali_osk_write_mem_barrier( void );

/*                                                             
  
                                                                               
                                                              
  
                                                                              
  
                                                                           
                                                                   
                                                                            
         
                                                                          
                                                                    
                                                     
 */
mali_io_address _mali_osk_mem_mapioregion( u32 phys, u32 size, const char *description );

/*                                                                       
  
                                                               
                             
  
                                                                      
                           
                                                                          
                                                                          
                                                                  
                            
                                                 
  
                                                                          
                                                                              
           
                                                                  
                                                                  
            
 */
void _mali_osk_mem_unmapioregion( u32 phys, u32 size, mali_io_address mapping );

/*                                                                          
  
                                                                              
                                                                       
                             
  
                                                                    
                           
  
                                                                            
  
                                                                       
                                                                            
                                 
  
                                                                           
                                                       
                                      
  
                                                                         
                                                                
  
                                                                    
                                                                                
 */
mali_io_address _mali_osk_mem_allocioregion( u32 *phys, u32 size );

/*                                                                      
  
                                                               
                               
  
                                                                      
                                              
                                                                           
                                                                           
                                                                 
                              
                                                 
  
                                                                          
                                                           
                                                                            
                                         
                                                                  
            
 */
void _mali_osk_mem_freeioregion( u32 phys, u32 size, mali_io_address mapping );

/*                                                         
  
                                                                              
          
  
                                                                            
                                                                               
                                                                            
          
  
                                                                            
                                                                   
                                                                            
           
                                                                          
                                                             
                                  
 */
_mali_osk_errcode_t _mali_osk_mem_reqregion( u32 phys, u32 size, const char *description );

/*                                                            
  
                                                                               
                                                                              
                                                                         
  
                                           
                             
                                                                            
                                                                            
                                                                         
                          
  
                                                                               
                                                                  
                                                                            
              
 */
void _mali_osk_mem_unreqregion( u32 phys, u32 size );

/*                                                         
                            
  
                                                                               
                                                                           
                                                                         
                                 
  
                                              
                                                                                             
                                                       
 */
u32 _mali_osk_mem_ioread32( volatile mali_io_address mapping, u32 offset );

/*                                                        
                                                    
  
                                                                                      
                                                                                              
                                                                         
                                 
  
                                             
                                                                                             
                                       
 */
void _mali_osk_mem_iowrite32_relaxed( volatile mali_io_address addr, u32 offset, u32 val );

/*                                                        
                                                      
  
                                                                             
                                                                          
                                                                         
                                 
  
                                             
                                                                                             
                                       
 */
void _mali_osk_mem_iowrite32( volatile mali_io_address mapping, u32 offset, u32 val );

/*                             
  
                                                                           
                                                    
 */
void _mali_osk_cache_flushall( void );

/*                                                                                                                   
  
                                                                                           
                                           
  
                                                                                                     
                                                                                                       
                                                                    
 */
void _mali_osk_cache_ensure_uncached_range_flushed( void *uncached_mapping, u32 offset, u32 size );

/*     */ /*                                      */


/*                                    
  
                                    
  
                                                                              
                                        
  
                                                                          
                                                                                
                                                                             
                                                                              
                                                                             
  
                                                                             
                                                                          
         
  
                                                                              
                                                                           
                                                                
  
                                                                             
                                      
  
                                                                         
                                                                          
  
                                                   
                                                                             
                                                                 
                                                                               
                                                                            
                             
  
      */

/*                                     
  
                                                                   
                                                 
  
                                                        
                                                           
                                                                              
                                                                             
                                                                       
                                      
  
                                                               
                                                   
  
                                                                                     
                                                                             
                                                                          
                                        
  
                                    
                                                           
                                                                                     
 */
_mali_osk_notification_t *_mali_osk_notification_create( u32 type, u32 size );

/*                                     
  
                                                                              
            
                                                
                                              
  
                                                                            
                                   
  
                                                                           
                                                        
                                                                         
                                                                            
                                         
  
                                                   
 */
void _mali_osk_notification_delete( _mali_osk_notification_t *object );

/*                                    
  
                                                                            
                                        
  
                                                                            
           
  
                                                                 
                                                                              
               
  
                                                                
 */
_mali_osk_notification_queue_t *_mali_osk_notification_queue_init( void );

/*                                     
  
                                                                               
  
                                                                           
                                                                        
                                                                            
                                                                            
                                        
                                                                       
                                                                             
                                                                               
                                        
                                                                          
  
                                                                           
                                                                            
                                                                         
                                             
  
                                                                          
                                                                         
         
  
                                                                
  
                                    
 */
void _mali_osk_notification_queue_term( _mali_osk_notification_queue_t *queue );

/*                                           
  
                                                                            
                                                                             
            
  
                                                   
  
                                                                                
                                                                        
                                                                                 
                                                                                   
  
                                                                               
                                                               
  
                                                                  
                                 
 */
void _mali_osk_notification_queue_send( _mali_osk_notification_queue_t *queue, _mali_osk_notification_t *object );

/*                                            
  
                                                       
  
                                                                               
                                                       
                                                                             
  
                                         
                                                        
                                                                             
                                                                
                                                                       
                                                                                                  
 */
_mali_osk_errcode_t _mali_osk_notification_queue_receive( _mali_osk_notification_queue_t *queue, _mali_osk_notification_t **result );

/*                                             
  
                                                       
  
                                                                             
  
                                         
                                                        
                                                                             
                                                                
                                                                       
                                                                                        
 */
_mali_osk_errcode_t _mali_osk_notification_queue_dequeue( _mali_osk_notification_queue_t *queue, _mali_osk_notification_t **result );

/*     */ /*                                  */


/*                             
  
                                                                            
                                                                          
             
  
      */

/*                           
  
                                                                           
                   
  
                                                                       
 */
_mali_osk_timer_t *_mali_osk_timer_init(void);

/*                      
  
                                                                   
                                 
  
                                                                
  
                                                                         
                                                                     
                                                  
  
                                
                                                                          
                     
 */
void _mali_osk_timer_add( _mali_osk_timer_t *tim, u32 ticks_to_expire );

/*                       
  
                                                                            
                                                                
  
                                                                    
                                  
  
                                                                            
                                                              
                                                                   
  
                                                         
                                                                            
                  
  
 */
void _mali_osk_timer_mod( _mali_osk_timer_t *tim, u32 ticks_to_expire);

/*                                                   
  
                                                                               
                                                
  
                                                                              
                                                                               
         
  
                                                                        
                                                
                                                                           
                                            
  
                                                
  
                                
  
 */
void _mali_osk_timer_del( _mali_osk_timer_t *tim );

/*                      
  
                                                                               
                           
  
                                                
  
                                
 */
void _mali_osk_timer_del_async( _mali_osk_timer_t *tim );

/*                                   
  
                            
  
                                
                                                                      
 */
mali_bool _mali_osk_timer_pending( _mali_osk_timer_t *tim);

/*                                           
  
                                                                        
  
                                                                            
                                                                            
                        
  
                                           
                                                      
                                                                          
 */
void _mali_osk_timer_setcallback( _mali_osk_timer_t *tim, _mali_osk_timer_callback_t callback, void *data );

/*                                                     
  
                                                                    
  
                                                                        
                                    
                                                      
  
                                      
 */
void _mali_osk_timer_term( _mali_osk_timer_t *tim );
/*     */ /*                           */


/*                                             
  
                                                                     
                                                                           
                                
  
                                                                            
                                                                              
                                                                           
                                                                             
                                                                             
                  
  
                                                                           
                                             
  
                                                       
                                                
                                           
  
      */

/*                                                
  
                                                                               
                                                                        
                                                                              
  
                     
                     
                                                                       
                  
 */
int	_mali_osk_time_after( u32 ticka, u32 tickb );

/*                                           
  
                                          
                                                       
 */
u32	_mali_osk_time_mstoticks( u32 ms );

/*                                           
  
                                          
                                                          
 */
u32	_mali_osk_time_tickstoms( u32 ticks );


/*                                            
                                          
 */
u32	_mali_osk_time_tickcount( void );

/*                                  
  
                                                                           
                                                                            
                                                    
  
                                                                              
                                                                               
                                                        
  
                                                       
 */
void _mali_osk_time_ubusydelay( u32 usecs );

/*                                                                
  
                               
 */
u64 _mali_osk_time_get_ns( void );


/*     */ /*                          */

/*                                   
      */

/*                                            
  
                                                                   
                                                                 
                     
  
                                                    
                                       
 */
u32 _mali_osk_clz( u32 val );

/*                                             
  
                                                   
                        
 */
u32 _mali_osk_fls( u32 val );

/*     */ /*                          */

/*                                                               
      */

/*                                        */
_mali_osk_wait_queue_t* _mali_osk_wait_queue_init( void );

/*                                    
  
                                
                                                          
                                                    
  
                                                                             
                                                                         
                                                     
 */
void _mali_osk_wait_queue_wait_event( _mali_osk_wait_queue_t *queue, mali_bool (*condition)(void *), void *data );

/*                                    
  
                                
                                                          
                                                    
                               
  
                                                                             
                                                                         
                                                                          
                   
 */
void _mali_osk_wait_queue_wait_event_timeout( _mali_osk_wait_queue_t *queue, mali_bool (*condition)(void *), void *data, u32 timeout );

/*                                                                             
       
  
                                                          
  
                                                                          
 */
void _mali_osk_wait_queue_wake_up( _mali_osk_wait_queue_t *queue );

/*                               
  
                                       
 */
void _mali_osk_wait_queue_term( _mali_osk_wait_queue_t *queue );
/*     */ /*                                */


/*                                     
      */

/*                                              
  
                                                                         
                         
  
                                                         
                                                                 
 */
void _mali_osk_dbgmsg( const char *fmt, ... );

/*                            
  
                                                                         
                         
  
                                            
                                                                   
                                                         
                                                                 
                                                
 */
u32 _mali_osk_snprintf( char *buf, u32 size, const char *fmt, ... );

/*                                
  
                                                            
  
                                                                                 
  
                                                                            
                                                                               
  
                                                                                 
 */
void _mali_osk_abort(void);

/*                                                           
  
                                                                                 
                                                                                
                                         
  
                                                                                 
 */
void _mali_osk_break(void);

/*                                                     
  
                                             
 */
u32 _mali_osk_get_pid(void);

/*                                                    
  
                                            
 */
u32 _mali_osk_get_tid(void);

/*                                                      
 */
void _mali_osk_pm_dev_enable(void);

/*                                                       
 */
void _mali_osk_pm_dev_disable(void);

_mali_osk_errcode_t _mali_osk_pm_delete_callback_timer(void);

/*                                                                          
  
                                                  
  
                                                                      
 */
_mali_osk_errcode_t _mali_osk_pm_dev_ref_add(void);


/*                                                                              
  
                                                         
  
                                                                                          
 */
void _mali_osk_pm_dev_ref_dec(void);


/*                                                                          
  
                                                                    
  
                                                                      
  
                                                                         
 */
mali_bool _mali_osk_pm_dev_ref_add_no_power_on(void);


/*                                                                                
  
                                                         
  
                                                                                                      
 */
void _mali_osk_pm_dev_ref_dec_no_power_on(void);

/*                                                    
 */
void _mali_osk_pm_dev_barrier(void);

/*     */ /*                                    */

/*     */ /*                  */

/*     */ /*                  */



#ifdef __cplusplus
}
#endif

/*                        */
#ifndef MALI_STATIC_INLINE
#error MALI_STATIC_INLINE not defined on your OS
#endif

#ifndef MALI_NON_STATIC_INLINE
#error MALI_NON_STATIC_INLINE not defined on your OS
#endif

#endif /*                */
