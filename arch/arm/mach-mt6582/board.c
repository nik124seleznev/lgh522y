#include <linux/init.h>
#include <linux/delay.h>
#include "mach/devs.h"
#include "mach/pm_common.h"



/*
                                                    
                   
 */
static __init int board_init(void)
{
    /*
                                                           
                                                                              
      
                                                                    
                                                                          
                                                             
                                                                 
                                                     
      
                                                                  
                                                                          
                                                     
                                                          
                                                           
                                                               
                                                                     
     */
    //                                

    mt_board_init();

    /*                                                 */
    //                              

    /*                                                                  */
# if 0
    custom_board_init();
#endif

    return 0;
}

late_initcall(board_init);
