#include "dm.h"

/*
                                                                      
                                                   
  
                                                                         
                                                                         
             
  
                                                                         
                                                                       
                                                                        
                                                                    
                                             
  
                                                                    
          
                                                         
                                                             
                                                                          
                                                     
                                            
                                                                    
                                                     
                                          
                                                             
                                                                           
                           
                                     
                                                                         
                                                                 
  
                                                                          
                                                                
                                                                       
                                                           
  
                                                                       
                                                                
 */

void dm_kobject_release(struct kobject *kobj)
{
	complete(dm_get_completion_from_kobject(kobj));
}

EXPORT_SYMBOL(dm_kobject_release);
