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

#ifndef __MALI_OSK_TYPES_H__
#define __MALI_OSK_TYPES_H__

#ifdef __cplusplus
extern "C" {
#endif

/* 
                                                      
  
     
 */

/* 
                                                                   
  
     
 */

/*                                                                                    
      */

/*                                                                                                                        */
#ifndef __KERNEL__
typedef unsigned char      u8;
typedef signed char        s8;
typedef unsigned short     u16;
typedef signed short       s16;
typedef unsigned int       u32;
typedef signed int         s32;
typedef unsigned long long u64;
#define BITS_PER_LONG (sizeof(long)*8)
#else
/*                                          */
#include <linux/types.h>
#endif

/*                                                              
  */
typedef unsigned long mali_bool;

#ifndef MALI_TRUE
#define MALI_TRUE ((mali_bool)1)
#endif

#ifndef MALI_FALSE
#define MALI_FALSE ((mali_bool)0)
#endif

#define MALI_HW_CORE_NO_COUNTER     ((u32)-1)

/* 
                         
  
                                                                       
                                                                            
                                                                          
                                                     
  
                                                                         
                                                                       
                                                                          
                                                      
 */
typedef enum {
	_MALI_OSK_ERR_OK = 0, /*            */
	_MALI_OSK_ERR_FAULT = -1, /*                       */
	_MALI_OSK_ERR_INVALID_FUNC = -2, /*                                                                                    */
	_MALI_OSK_ERR_INVALID_ARGS = -3, /*                                                          */
	_MALI_OSK_ERR_NOMEM = -4, /*                       */
	_MALI_OSK_ERR_TIMEOUT = -5, /*                    */
	_MALI_OSK_ERR_RESTARTSYSCALL = -6, /*                                                                                                      */
	_MALI_OSK_ERR_ITEM_NOT_FOUND = -7, /*                       */
	_MALI_OSK_ERR_BUSY = -8, /*                                             */
	_MALI_OSK_ERR_UNSUPPORTED = -9, /*                                                           */
} _mali_osk_errcode_t;

/*     */ /*                                   */

/*                                        
      */

/*                                       */
typedef struct _mali_osk_wq_work_s _mali_osk_wq_work_t;
typedef struct _mali_osk_wq_delayed_work_s _mali_osk_wq_delayed_work_t;

/*                                    
  
                                                                             
                                      
  
                                                                         
                                
  
                                    
 */
typedef void (*_mali_osk_wq_work_handler_t)( void * arg );

/*    */ /*                        */

/*                                          
      */

/*                                               */
typedef struct _mali_osk_irq_t_struct _mali_osk_irq_t;

/*                                                            
  
                                                                                         
                                     */
typedef void  (*_mali_osk_irq_trigger_t)( void * arg );

/*                                                                
  
                                                                                         
                                    
                                                                                                     */
typedef _mali_osk_errcode_t (*_mali_osk_irq_ack_t)( void * arg );

/*                                           
  
                                                                                   
                                                                              
                                               
  
                                                                             
                                     
  
                                                                       
                                                                             
          
  
                                                                         
                                                              
                                                                             
                    
  
                                    
                                                                           
                                 
 */
typedef _mali_osk_errcode_t  (*_mali_osk_irq_uhandler_t)( void * arg );


/*     */ /*                         */


/*                                                
      */

/*                                       
  
                                                                                                          
                                                                   
  
                                                                                    
                                                                               
                                         
 */
typedef struct {
	union {
		u32 val;
		void *obj;
	} u;
} _mali_osk_atomic_t;
/*     */ /*                            */


/*                                                     
      */


/*                                               
  
                                                                           
                                             
  
                                                                         
             
                                                                              
                    
  
 */
typedef enum {
	/*                     */
	/*                     */
	/*                     */
	/*                     */
	/*                     */

	_MALI_OSK_LOCK_ORDER_FIRST = 0,

	_MALI_OSK_LOCK_ORDER_SESSIONS,
	_MALI_OSK_LOCK_ORDER_MEM_SESSION,
	_MALI_OSK_LOCK_ORDER_MEM_INFO,
	_MALI_OSK_LOCK_ORDER_MEM_PT_CACHE,
	_MALI_OSK_LOCK_ORDER_DESCRIPTOR_MAP,
	_MALI_OSK_LOCK_ORDER_GROUP_VIRTUAL,
	_MALI_OSK_LOCK_ORDER_GROUP,
	_MALI_OSK_LOCK_ORDER_TIMELINE_SYSTEM,
	_MALI_OSK_LOCK_ORDER_SCHEDULER,
	_MALI_OSK_LOCK_ORDER_SCHEDULER_DEFERRED,
	_MALI_OSK_LOCK_ORDER_PM_CORE_STATE,
	_MALI_OSK_LOCK_ORDER_L2_COMMAND,
	_MALI_OSK_LOCK_ORDER_DMA_COMMAND,
	_MALI_OSK_LOCK_ORDER_PROFILING,
	_MALI_OSK_LOCK_ORDER_L2_COUNTER,
	_MALI_OSK_LOCK_ORDER_UTILIZATION,
	_MALI_OSK_LOCK_ORDER_PM_EXECUTE,
	_MALI_OSK_LOCK_ORDER_SESSION_PENDING_JOBS,
	_MALI_OSK_LOCK_ORDER_PM_DOMAIN,
	_MALI_OSK_LOCK_ORDER_PMU,

	_MALI_OSK_LOCK_ORDER_LAST,
} _mali_osk_lock_order_t;


/*                                             
  
                                          
 */
typedef enum {
	_MALI_OSK_LOCKFLAG_UNORDERED        = 0x1, /*                                                              */
	_MALI_OSK_LOCKFLAG_ORDERED          = 0x2,
	/*                              
   
                                                              */

} _mali_osk_lock_flags_t;

/*                                                     
  
                                                                                  
                                                                                     
                                                                                        
                                                                                  
                                                                            
  
                                                                              
                                                               
  
                                                                             
                                                   
  
 */
typedef enum {
	_MALI_OSK_LOCKMODE_UNDEF = -1,  /*                                              */
	_MALI_OSK_LOCKMODE_RW    = 0x0, /*                                                                            */
	_MALI_OSK_LOCKMODE_RO,          /*                                                                                                            */
	/*                             
   
                                                     */
} _mali_osk_lock_mode_t;

/*                                                         */
typedef struct _mali_osk_lock_debug_s _mali_osk_lock_debug_t;
typedef struct _mali_osk_spinlock_s _mali_osk_spinlock_t;
typedef struct _mali_osk_spinlock_irq_s _mali_osk_spinlock_irq_t;
typedef struct _mali_osk_mutex_s _mali_osk_mutex_t;
typedef struct _mali_osk_mutex_rw_s _mali_osk_mutex_rw_t;

/*     */ /*                          */

/*                                                                      
      */

/* 
                                                                   
  
                                                                          
           
                                                                
                                                                                
  
                                                                             
                                       
 */
typedef struct _mali_io_address * mali_io_address;

/*                                                             
  
                                             
                                                                              
                                               
  
                                                                             
                                                                
  
                                                                                
                                                                              
                             
  
                                                                               
                      
  
     
 */

/*                                                                                  */
#define _MALI_OSK_CPU_PAGE_ORDER ((u32)12)
/*                                         */
#define _MALI_OSK_CPU_PAGE_SIZE (((u32)1) << (_MALI_OSK_CPU_PAGE_ORDER))
/*                                                          */
#define _MALI_OSK_CPU_PAGE_MASK (~((((u32)1) << (_MALI_OSK_CPU_PAGE_ORDER)) - ((u32)1)))
/*     */ /*                                 */

/*                                                              
  
                                                                             
                                                                              
                                               
  
                                                                             
                                                                
  
                                                                                
                                                                              
                             
  
                                                                               
                      
  
     
 */

/*                                                                                    */
#define _MALI_OSK_MALI_PAGE_ORDER ((u32)12)
/*                                          */
#define _MALI_OSK_MALI_PAGE_SIZE (((u32)1) << (_MALI_OSK_MALI_PAGE_ORDER))
/*                                                           */
#define _MALI_OSK_MALI_PAGE_MASK (~((((u32)1) << (_MALI_OSK_MALI_PAGE_ORDER)) - ((u32)1)))
/*     */ /*                                 */

/*                                                         
  
                                                                              
                                                                             
                                                                            
                                  
  
                                                                             
                         
 */
typedef enum {
	_MALI_OSK_MEM_MAPREGION_FLAG_OS_ALLOCATED_PHYSADDR = 0x1, /*                                    */
} _mali_osk_mem_mapregion_flags_t;
/*     */ /*                                      */

/*                                                          
      */

/*                                                     */
typedef struct _mali_osk_notification_queue_t_struct _mali_osk_notification_queue_t;

/*                                              */
typedef struct _mali_osk_notification_t_struct {
	u32 notification_type;   /*                         */
	u32 result_buffer_size; /*                                                   */
	void * result_buffer;   /*                                            */
} _mali_osk_notification_t;

/*     */ /*                                  */


/*                                               
      */

/*                                              
  
                                                                            
                                                                            
                                                           
  
                                                                          
                                                                             
                                                                             
                                   
  
                                                                              
                                                                             
                                                                            
  
                                     */
typedef void (*_mali_osk_timer_callback_t)(void * arg);

/*                                                 */
typedef struct _mali_osk_timer_t_struct _mali_osk_timer_t;
/*     */ /*                           */


/*                                                             
      */

/*                             
  
                                                                              
                                                                   
                                                                   
                         
  
                                                                         
                                                                              
                                                                         
                        
 */
typedef struct _mali_osk_list_s {
	struct _mali_osk_list_s *next;
	struct _mali_osk_list_s *prev;
} _mali_osk_list_t;
/*     */ /*                          */

/*                                     
      */

/*                                    
  
                                                            
 */
typedef struct _mali_osk_resource {
	const char * description;       /*                                     */
	u32 base;                       /*                                                                     */
	u32 irq;                        /*                                                                                           */
} _mali_osk_resource_t;
/*     */ /*                                   */

/*                                                             
      */
/*                                             */
typedef struct _mali_osk_wait_queue_t_struct _mali_osk_wait_queue_t;
/*     */ /*                                */

/*     */ /*                  */

/*     */ /*                  */

#ifdef __cplusplus
}
#endif

#endif /*                      */
