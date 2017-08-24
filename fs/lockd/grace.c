/*
                                                            
 */

#include <linux/module.h>
#include <linux/lockd/bind.h>
#include <net/net_namespace.h>

#include "netns.h"

static DEFINE_SPINLOCK(grace_lock);

/* 
                    
                                    
  
                                                                    
                                                                  
                                                                    
                                         
  
                                                   
 */
void locks_start_grace(struct net *net, struct lock_manager *lm)
{
	struct lockd_net *ln = net_generic(net, lockd_net_id);

	spin_lock(&grace_lock);
	list_add(&lm->list, &ln->grace_list);
	spin_unlock(&grace_lock);
}
EXPORT_SYMBOL_GPL(locks_start_grace);

/* 
                  
                                    
  
                                                                      
                                                                        
                                                                        
                                                                        
                                         
 */
void locks_end_grace(struct lock_manager *lm)
{
	spin_lock(&grace_lock);
	list_del_init(&lm->list);
	spin_unlock(&grace_lock);
}
EXPORT_SYMBOL_GPL(locks_end_grace);

/* 
                 
  
                                                                       
                                                                     
                 
 */
int locks_in_grace(struct net *net)
{
	struct lockd_net *ln = net_generic(net, lockd_net_id);

	return !list_empty(&ln->grace_list);
}
EXPORT_SYMBOL_GPL(locks_in_grace);
