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

#define BP_CAP                   0x01	/*       */
#define BP_STATUS_CAP            0x02
#define BP_STATUS_CHANGE_CAP     0x04
#define SW_CTL_CAP               0x08
#define BP_DIS_CAP               0x10
#define BP_DIS_STATUS_CAP        0x20
#define STD_NIC_CAP              0x40
#define BP_PWOFF_ON_CAP          0x80
#define BP_PWOFF_OFF_CAP         0x0100
#define BP_PWOFF_CTL_CAP         0x0200
#define BP_PWUP_ON_CAP           0x0400
#define BP_PWUP_OFF_CAP          0x0800
#define BP_PWUP_CTL_CAP          0x1000
#define WD_CTL_CAP               0x2000
#define WD_STATUS_CAP            0x4000
#define WD_TIMEOUT_CAP           0x8000
#define TX_CTL_CAP               0x10000
#define TX_STATUS_CAP            0x20000
#define TAP_CAP                  0x40000
#define TAP_STATUS_CAP           0x80000
#define TAP_STATUS_CHANGE_CAP    0x100000
#define TAP_DIS_CAP              0x200000
#define TAP_DIS_STATUS_CAP       0x400000
#define TAP_PWUP_ON_CAP          0x800000
#define TAP_PWUP_OFF_CAP         0x1000000
#define TAP_PWUP_CTL_CAP         0x2000000
#define NIC_CAP_NEG              0x4000000	/*        */

#define WD_MIN_TIME_GET(desc)   (desc & 0xf)
#define WDT_STEP_TIME           0x10

struct bp_info {
	char prod_name[14];
	unsigned char fw_ver;
};

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
int set_tpl_sd(int if_index, int tpl_state);

/* 
                                             
                                                            
  
          
                                            
                                                           
  */
int get_tpl_sd(int if_index);

int get_bp_hw_reset_sd(int if_index);

int set_bp_hw_reset_sd(int if_index, int status);

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

int get_bypass_info_sd(int if_index, struct bp_info *bp_info);
int bp_if_scan_sd(void);
/*                         */
