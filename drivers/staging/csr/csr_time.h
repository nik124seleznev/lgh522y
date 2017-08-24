#ifndef CSR_TIME_H__
#define CSR_TIME_H__
/*****************************************************************************

(c) Cambridge Silicon Radio Limited 2010
All rights reserved and confidential information of CSR

Refer to LICENSE.txt included with this source for details
on the license terms.

*****************************************************************************/

#include <linux/types.h>

/*                                                                              

    
           

           
                                                                       
                                                                        
                                         

                                                             
                                                                    
                

                                                                        
                                                               
                                        

          
                                                                  
                                                   

       
                                                 

                                                                              */
u32 CsrTimeGet(u32 *high);


/*                                                                  */
/*                */
/*                                                                  */

/*                                                                             
        
                  
  
               
                                                                           
                                              
  
           
                                           
  
                                                                              */
#define CsrTimeAdd(t1, t2) ((t1) + (t2))

/*                                                                             
        
                  
  
               
                                                                        
                                                
  
           
                              
  
                                                                              */
#define CsrTimeSub(t1, t2)    ((s32) (t1) - (s32) (t2))

#endif
