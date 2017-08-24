/*                                                                                                
           
                                                         
                                          
          
                                                         
     
                                                                
                                                                     
                               
   
                                   
                                                     
                                                                                          
                                                                                                 */
#ifndef _A5142_OTP_H_
#define _A5142_OTP_H_
#define USHORT             unsigned short

/*                                                                                                
                               
                                                                  
                                                                  
                                             
                                             
                                                                                                 */
bool start_read_otp(USHORT zone);


/*                                                                                                
                             
                                    
                                                                  
                                              
                                                                                     
                                                                                                 */
/*
                                   
*/


/*                                                                                                
                             
                                       
                                                                  
                                                                                                
                                                                                                 */
USHORT get_otp_flag(USHORT zone);


/*                                                                                                
                                  
                                   
                                                                  
                                            
                                                                              
                                                                                                 */
USHORT get_otp_module_id(USHORT zone);


/*                                                                                                
                                
                                       
                                                                  
                                            
                                                        
                                                                                                 */
USHORT get_otp_lens_id(USHORT zone);


/*                                                                                                
                               
                                      
                                                                  
                                            
                                                       
                                                                                                 */
USHORT get_otp_vcm_id(USHORT zone);


/*                                                                                                
                                  
                                         
                                                                  
                                            
                                                          
                                                                                                 */
USHORT get_otp_driver_id(USHORT zone);

/*                                                                                                
                             
                                                                                
                                               
                                              
                                                                  
                                                                                                 */
/*
                                  
*/

/*                                                                                                
                                
                                        
                                                                  
                                          
                                                               
                                                                                                 */
bool otp_lenc_update(USHORT zone);

/*                                                                                                
                             
                                                        
                                               
                                                
                                                         
                                                                                                 */
int wb_multi_cal(USHORT value);

/*                                                                                                
                            
                                                        
                                                  
                                                             
                                                                                                 */
USHORT wb_gain_cal(int multiple);

/*                                                                                                
                            
                                                        
                                                                              
                                                                             
                                        
                                                      
                                                                                                 */
bool wb_gain_set(int r_ratio, int b_ratio);

/*                                                                                                
                              
                                      
                                                                  
                                          
                                                             
                                                                                                 */
bool otp_wb_update(USHORT zone);

/*                                                                                                
                           
                              
                                                                  
                                          
                                                             
                                                                                                 */
bool get_otp_wb(USHORT zone);

#endif