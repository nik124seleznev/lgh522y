#ifndef __IIO_BFIN_TIMER_TRIGGER_H__
#define __IIO_BFIN_TIMER_TRIGGER_H__

/* 
                                                                    
                                                            
                                                        
                                                        
  
                                                                               
                                                                            
                                                                           
                                                                                
                                                                                
                                                                             
                           
  */
struct iio_bfin_timer_trigger_pdata {
	bool output_enable;
	bool active_low;
	unsigned int duty_ns;
};

#endif
