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

#ifndef __MALI_OSK_LIST_H__
#define __MALI_OSK_LIST_H__

#include "mali_osk.h"
#include "mali_kernel_common.h"

#ifdef __cplusplus
extern "C" {
#endif

MALI_STATIC_INLINE void __mali_osk_list_add(_mali_osk_list_t *new_entry, _mali_osk_list_t *prev, _mali_osk_list_t *next)
{
	next->prev = new_entry;
	new_entry->next = next;
	new_entry->prev = prev;
	prev->next = new_entry;
}

MALI_STATIC_INLINE void __mali_osk_list_del(_mali_osk_list_t *prev, _mali_osk_list_t *next)
{
	next->prev = prev;
	prev->next = next;
}

/*                                                             
      */

/*                                                                         
                                                
  
                                                                             
                                                                          
                                      */

/*                                                        
                                      */
#define _MALI_OSK_INIT_LIST_HEAD(exp) _mali_osk_list_init(exp)

/*                                                        
                                                                         */
#define _MALI_OSK_LIST_HEAD(exp) _mali_osk_list_t exp

/*                                                      
                                                                         */
#define _MALI_OSK_LIST_HEAD_STATIC_INIT(exp) _mali_osk_list_t exp = { &exp, &exp }

/*                                   
  
                                                    
  
                                                                         
                                                                
  
                                             
 */
MALI_STATIC_INLINE void _mali_osk_list_init( _mali_osk_list_t *list )
{
	list->next = list;
	list->prev = list;
}

/*                                                              
  
                                                                              
                                 
  
                                                                              
                        
  
  
                                            
                                                                            
                     
 */
MALI_STATIC_INLINE void _mali_osk_list_add( _mali_osk_list_t *new_entry, _mali_osk_list_t *list )
{
	__mali_osk_list_add(new_entry, list, list->next);
}

/*                                                               
  
                                                                              
                                
  
                                                                              
                        
  
                                            
                                                                                
                     
 */
MALI_STATIC_INLINE void _mali_osk_list_addtail( _mali_osk_list_t *new_entry, _mali_osk_list_t *list )
{
	__mali_osk_list_add(new_entry, list->prev, list);
}

/*                                            
  
                                                                              
                                                                    
                                  
  
                                          
 */
MALI_STATIC_INLINE void _mali_osk_list_del( _mali_osk_list_t *list )
{
	__mali_osk_list_del(list->prev, list->next);
}

/*                                                                  
  
                                                                              
                                                  
  
                                                         
 */
MALI_STATIC_INLINE void _mali_osk_list_delinit( _mali_osk_list_t *list )
{
	__mali_osk_list_del(list->prev, list->next);
	_mali_osk_list_init(list);
}

/*                                           
  
                                                                             
  
                                 
                                                             
 */
MALI_STATIC_INLINE mali_bool _mali_osk_list_empty( _mali_osk_list_t *list )
{
	return list->next == list;
}

/*                                                      
  
                                        
  
                                                                               
                                           
  
                                       
                                                                                
                       
 */
MALI_STATIC_INLINE void _mali_osk_list_move( _mali_osk_list_t *move_entry, _mali_osk_list_t *list )
{
	__mali_osk_list_del(move_entry->prev, move_entry->next);
	_mali_osk_list_add(move_entry, list);
}

/*                            
  
                                        
  
                                                                    
  
                                         
                                                            
 */
MALI_STATIC_INLINE void _mali_osk_list_move_list( _mali_osk_list_t *old_list, _mali_osk_list_t *new_list )
{
	MALI_DEBUG_ASSERT(_mali_osk_list_empty(new_list));
	if (!_mali_osk_list_empty(old_list)) {
		new_list->next = old_list->next;
		new_list->prev = old_list->prev;
		new_list->next->prev = new_list;
		new_list->prev->next = new_list;
		old_list->next = old_list;
		old_list->prev = old_list;
	}
}

/*                                                
  
                                                                            
                                                    
  
                                                                              
                                                                       
                                  
  
                                                                             
                                                                             
                              
  
                                                                          
                                                                 
                                                                
                                                                            
                                                            
 */
#define _MALI_OSK_LIST_ENTRY(ptr, type, member) \
	_MALI_OSK_CONTAINER_OF(ptr, type, member)

/*                                
  
                                                                        
                                                                       
                                                                              
                            
  
                                                                          
                                                                           
                   
  
                                                                         
                                                                              
                       
  
                                                                        
                                                               
                                                                           
                                       
                                                                           
        
                                                                           
                                                    
                                                                             
                             
 */
#define _MALI_OSK_LIST_FOREACHENTRY(ptr, tmp, list, type, member)         \
	for (ptr = _MALI_OSK_LIST_ENTRY((list)->next, type, member),      \
	     tmp = _MALI_OSK_LIST_ENTRY(ptr->member.next, type, member);  \
	     &ptr->member != (list);                                      \
	     ptr = tmp,                                                   \
	     tmp = _MALI_OSK_LIST_ENTRY(tmp->member.next, type, member))

/*                                                 
  
                                                                           
                                           
  
                                                                        
                                                               
                                                                           
                                       
                                                                           
        
                                                                           
                                                    
                                                                             
                             
 */
#define _MALI_OSK_LIST_FOREACHENTRY_REVERSE(ptr, tmp, list, type, member) \
	for (ptr = _MALI_OSK_LIST_ENTRY((list)->prev, type, member),      \
	     tmp = _MALI_OSK_LIST_ENTRY(ptr->member.prev, type, member);  \
	     &ptr->member != (list);                                      \
	     ptr = tmp,                                                   \
	     tmp = _MALI_OSK_LIST_ENTRY(tmp->member.prev, type, member))

/*     */ /*                          */

#ifdef __cplusplus
}
#endif

#endif /*                     */
