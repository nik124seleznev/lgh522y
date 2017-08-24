/*                                                                              
  
            
            
                         
  
           
           
                                
  
               
               
                          
  
          
          
  
  
  
                                                                                
             
             
         
  
    
  
  
  
                                                                               */
#ifndef _AUDDRV_BTCVSD_IOCTL_MSG_H
#define _AUDDRV_BTCVSD_IOCTL_MSG_H


/*                                                                            
                                                 
                                                                              
*/


/*                                                                            
                                            
                                                                              
*/

//                        
#define AUD_DRV_BTCVSD_IOC_MAGIC 'C'

#define ALLOCATE_FREE_BTCVSD_BUF _IOWR(AUD_DRV_BTCVSD_IOC_MAGIC, 0xE0, unsigned int)
#define SET_BTCVSD_STATE         _IOWR(AUD_DRV_BTCVSD_IOC_MAGIC, 0xE1, unsigned int)
#define GET_BTCVSD_STATE         _IOWR(AUD_DRV_BTCVSD_IOC_MAGIC, 0xE2, unsigned int)



/*                                                                            
                                             
                                                                              
*/

/*                                                                            
                                                              
                                                                              
*/


#endif

