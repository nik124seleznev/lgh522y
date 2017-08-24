/*                                                                            */
/*                                                                            */
/* bypass library, Copyright (c) 2004 Silicom, Ltd                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/* it under the terms of the GNU General Public License as published by       */
/* the Free Software Foundation, located in the file LICENSE.                 */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */

/* 
                                                             
                                  
  
          
                                                 
                                         
                                  
  */
int is_bypass_sd(int if_index);

/* 
                                                                    
                                  
  
          
                                            
                                                                      
  */
int get_bypass_slave_sd(int if_index);

/* 
                                                                   
                                  
  
          
                                                                                         
                                  
                                                                             
                                                                                         
                                          
   
                                                                      
   
                                                                              
   
                                                                                   
                                                                  
   
                                                                                                     
   
                                                                                                
                                                                                                     
                                                                                                  
                                                                       
   
                                                                             
   
                                                                                                           
                                                                                                       
   
                                                                                   
   
                                                                                                  
                                                                           
   
                                                                                                  
                                                                          
   
                                                                                           
                                                                                           
   
                                                                                                   
                                                                                           
   
                                                                                                      
   
                                                                                                          
                                                           
   
                                                                                            
   
                                                                                                         
   
                 
  
    */
int get_bypass_caps_sd(int if_index);

/* 
                                                               
                                  
  
          
   
                                                                          
                                    
                                                                     
   
                                  
   
                                                                           
   
                                               
                                                              
                                                                                    
   
                                                                  
                                                                
   
   
   
  */
int get_wd_set_caps_sd(int if_index);

/* 
                                
                                                            
                                            
          
                  
                                                                      
  */
int set_bypass_sd(int if_index, int bypass_mode);

/* 
                                     
                                                            
          
                             
                                                                      
  */
int get_bypass_sd(int if_index);

/* 
                                                                             
                                                            
          
                             
                                                                      
  */
int get_bypass_change_sd(int if_index);

/* 
                                           
                                                            
                                           
          
                  
                                                                                       
                                            
  */
int set_dis_bypass_sd(int if_index, int dis_bypass);

/* 
                                                 
                                                            
          
                                                         
                                                                                       
                                            
  */
int get_dis_bypass_sd(int if_index);

/* 
                                                        
                                                            
                                                                           
          
                   
                                                                                       
                                            
  */
int set_bypass_pwoff_sd(int if_index, int bypass_mode);

/* 
                                                              
                                                            
          
                                                                             
                                                                                       
                                            
  */
int get_bypass_pwoff_sd(int if_index);

/* 
                                                      
                                                            
                                                                          
          
                   
                                                                                       
                                            
  */
int set_bypass_pwup_sd(int if_index, int bypass_mode);

/* 
                                                            
                                                            
          
                                                                            
                                                                                       
                                            
  */
int get_bypass_pwup_sd(int if_index);

/* 
                                     
                                                            
                                                                                   
                                                             
                                                                                      
          
                   
                                                                                       
                                            
  */
int set_bypass_wd_sd(int if_index, int ms_timeout, int *ms_timeout_set);

/* 
                                     
                                                            
                                                    
                                                  
                                              
          
                  
                                                                                       
                                            
  */
int get_bypass_wd_sd(int if_index, int *ms_timeout_set);

/* 
                                           
                                                            
                                                                
                                              
                                              
          
                  
                                                                                       
                                                                  
  */
int get_wd_expire_time_sd(int if_index, int *ms_time_left);

/* 
                                               
                                                            
   
          
                  
                                 
                                                                                       
                                                                  
  */
int reset_bypass_wd_timer_sd(int if_index);

/* 
                                               
                                                            
                                                           
   
          
                  
                                                                                       
                                            
  */
int set_std_nic_sd(int if_index, int nic_mode);

/* 
                                              
                                                            
   
          
                                                           
                                                                                       
                                            
  */
int get_std_nic_sd(int if_index);

/* 
                                           
                                                            
                                                      
   
          
                  
                                                             
  */
int set_tx_sd(int if_index, int tx_state);

/* 
                                                         
                                                            
   
          
                                                     
                                                                                        
  */
int get_tx_sd(int if_index);

/* 
                          
                                                            
                                             
          
                  
                                                                   
  */
int set_tap_sd(int if_index, int tap_mode);

/* 
                               
                                                            
          
                             
                                                                   
  */
int get_tap_sd(int if_index);

/* 
                                                                       
                                                            
          
                             
                                                                   
  */
int get_tap_change_sd(int if_index);

/* 
                                     
                                                            
                                     
          
                  
                                                                                    
                                            
  */
int set_dis_tap_sd(int if_index, int dis_tap);

/* 
                                           
                                                            
          
                                                   
                                                                                    
                                            
  */
int get_dis_tap_sd(int if_index);

/* 
                                                
                                                            
                                                                         
          
                   
                                                                                    
                                            
  */
int set_tap_pwup_sd(int if_index, int tap_mode);

/* 
                                                      
                                                            
          
                                                                      
                                                                                    
                                            
  */
int get_tap_pwup_sd(int if_index);

/* 
                                     
                                                            
                                            
          
                  
                                                                          
  */
int set_bp_disc_sd(int if_index, int disc_mode);

/* 
                                          
                                                            
          
                             
                                                                          
  */
int get_bp_disc_sd(int if_index);

/* 
                                                                                  
                                                            
          
                             
                                                                          
  */
int get_bp_disc_change_sd(int if_index);

/* 
                                                
                                                            
                                     
          
                  
                                                                                          
                                            
  */
int set_bp_dis_disc_sd(int if_index, int dis_disc);

/* 
                                                  
                                                            
          
                                                                 
                                                                                           
                                            
  */
int get_bp_dis_disc_sd(int if_index);

/* 
                                                           
                                                            
                                                                         
          
                   
                                                                                           
                                            
  */
int set_bp_disc_pwup_sd(int if_index, int disc_mode);

/* 
                                                                 
                                                            
          
                                                                                    
                                                                                    
                                            
  */
int get_bp_disc_pwup_sd(int if_index);

/* 
                                                        
                                                            
                                                           
          
                  
                                                                      
  */
int set_wd_exp_mode_sd(int if_index, int bypass_mode);

/* 
                                                        
                                                            
          
                                 
                                                                      
  */
int get_wd_exp_mode_sd(int if_index);

/* 
                                             
                                                            
                                                           
          
                  
                                                                                       
                                                                  
  */
int set_wd_autoreset_sd(int if_index, int time);

/* 
                                             
                                                            
                                                           
          
                  
                                                                                       
                                                                  
  */
int get_wd_autoreset_sd(int if_index);

/* 
                          
                                                            
                                             
          
                  
                                            
  */
int set_tpl_sd(int if_index, int tpl_mode);

/* 
                               
                                                            
          
                             
                                                                   
  */
int get_tpl_sd(int if_index);

int get_bypass_info_sd(int if_index, struct bp_info *bp_info);
int bp_if_scan_sd(void);
/*                         */
