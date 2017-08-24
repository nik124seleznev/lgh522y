/*
 *
 * (C) COPYRIGHT ARM Limited. All rights reserved.
 *
 * This program is free software and is provided to you under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation, and any use by you of this program is subject to the terms
 * of such GNU licence.
 *
 * A copy of the licence is included with the program, and can also be obtained
 * from Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 */





#ifndef _KDS_H_
#define _KDS_H_

#include <linux/list.h>
#include <linux/workqueue.h>

#define KDS_WAIT_BLOCKING (ULONG_MAX)

struct kds_resource_set;

typedef void (*kds_callback_fn) (void *callback_parameter, void *callback_extra_parameter);

struct kds_callback
{
	kds_callback_fn  user_cb; /*         */
	int direct;               /*                           */
	struct workqueue_struct *wq;
};

struct kds_link
{
	struct kds_resource_set *parent;
	struct list_head         link;
	unsigned long            state;
};

struct kds_resource
{
	struct kds_link waiters;
};

/*              */

/*                              
  
                                                    
  
                                                               
                        
  
                                                                               
                                                                             
                              
  
                                                                        
                                                                     
 */
int kds_callback_init(struct kds_callback *cb, int direct, kds_callback_fn user_cb);

/*                                        
  
                                                  
                                               
                                                         
                                                      
 */
void kds_callback_term(struct kds_callback *cb);


/*                     */

/*                                                    */
void kds_resource_init(struct kds_resource * const resource);

/*
                            
                                                              
                                                                                   
                                                                     
 */
int kds_resource_term(struct kds_resource *resource);

/*                                          
                                                            
                                                                                                   
                                                                                                 
                                                                                  
                                                                                                                   
                                                                                                  
  
                                     
 */
int kds_async_waitall(
		struct kds_resource_set ** const pprset,
		struct kds_callback      *cb,
		void                     *callback_parameter,
		void                     *callback_extra_parameter,
		int                       number_resources,
		unsigned long            *exclusive_access_bitmap,
		struct kds_resource     **resource_list);

/*                                         
                                                        
                                     
                                 
                                        
  
                                                                                        
                                                                                      
                                                                     
  
                                                                        
                                                               
  
                                                                                                         
                                                                                                         
                                                                          
  
                                                        
                  
                                
                                                          
                                                  
                                                                                     
 */
struct kds_resource_set *kds_waitall(
		int                   number_resources,
		unsigned long        *exclusive_access_bitmap,
		struct kds_resource **resource_list,
		unsigned long         jifies_timeout);

/*                             
                                                              
                                                        
  
                                                               
                                          
  
                                                             
                                                            
                                             
 */
void kds_resource_set_release(struct kds_resource_set **pprset);

/*                                                            
                                                              
                                                        
  
                                                               
                                          
  
                                                             
                                                            
                                             
  
                                                                 
            
  
                                                                
                                                 
 */

void kds_resource_set_release_sync(struct kds_resource_set **pprset);
#endif /*         */
