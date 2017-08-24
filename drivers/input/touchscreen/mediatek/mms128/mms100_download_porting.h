
//                                                        
//
//
//                                                   
//
//
//                                                        

#ifndef __MELFAS_DOWNLOAD_PORTING_H_INCLUDED__
#define __MELFAS_DOWNLOAD_PORTING_H_INCLUDED__

//                                                            
//
//              
//
//                                                            
/*

                            
                                              

                                                       

                                                                               

                                                                            
                                                                                                    

                                                                                                     
                                                              

                                           

                                    

                           
                           

                                                           

*/

//                                                            
//
//            
//
//                                                            

//                     
//                             
//                               
//                       
//                               
//                                
//                      
//                             
typedef unsigned char		BOOLEAN;


#ifndef TRUE
#define TRUE 				(1==1)
#endif

#ifndef FALSE
#define FALSE 				(1==0)
#endif

#ifndef NULL
#define NULL 				0
#endif


//                                                            
//
//                         
//
//                                                            

//                                                                     
#define MELFAS_ENABLE_DBG_PRINT											1
#define MELFAS_ENABLE_DBG_PROGRESS_PRINT								1

//                                                               
#define MELFAS_ENABLE_DELAY_TEST										0


//                                                            
//
//                   
//
//                             
//                            
//                       
//
//                                                            

//                
//    
//                
#if MCSDL_USE_VDD_CONTROL
#define MCSDL_VDD_SET_HIGH()             			hwPowerOn ( MT6323_POWER_LDO_VGP2, VOL_3000, "TP" );
#define MCSDL_VDD_SET_LOW()              			hwPowerDown ( MT6323_POWER_LDO_VGP2, "TP" );
#else
#define MCSDL_VDD_SET_HIGH()             			//        
#define MCSDL_VDD_SET_LOW()              			//        
#endif

//                
//   
//                
#if MCSDL_USE_CE_CONTROL
#define MCSDL_CE_SET_HIGH()   	          			____HERE!_____
#define MCSDL_CE_SET_LOW()      	        		____HERE!_____
#define MCSDL_CE_SET_OUTPUT()   	        		____HERE!_____
#else
#define MCSDL_CE_SET_HIGH()							//        
#define MCSDL_CE_SET_LOW()							//        
#define MCSDL_CE_SET_OUTPUT()						//        
#endif


//                
//       
//                
#if MCSDL_USE_RESETB_CONTROL
#define MCSDL_RESETB_SET_HIGH()             		mt_set_gpio_out(GPIO_CTP_EINT_PIN, 1);
#define MCSDL_RESETB_SET_LOW()              		mt_set_gpio_out(GPIO_CTP_EINT_PIN, 0);
#define MCSDL_RESETB_SET_OUTPUT()     		        mt_set_gpio_dir(GPIO_CTP_EINT_PIN, GPIO_DIR_OUT);
#define MCSDL_RESETB_SET_INPUT()            		mt_set_gpio_dir(GPIO_CTP_EINT_PIN, GPIO_DIR_IN);
#else
#define MCSDL_RESETB_SET_HIGH()                     //        
#define MCSDL_RESETB_SET_LOW()                      //        
#define MCSDL_RESETB_SET_OUTPUT()                   //        
#define MCSDL_RESETB_SET_INPUT()                    //        
#endif


//                  
//              
//                  

#define MCSDL_GPIO_SCL_SET_HIGH()					mt_set_gpio_out(GPIO_I2C1_SCA_PIN, 1);
#define MCSDL_GPIO_SCL_SET_LOW()					mt_set_gpio_out(GPIO_I2C1_SCA_PIN, 0);

#define MCSDL_GPIO_SDA_SET_HIGH()					mt_set_gpio_out(GPIO_I2C1_SDA_PIN, 1);
#define MCSDL_GPIO_SDA_SET_LOW()					mt_set_gpio_out(GPIO_I2C1_SDA_PIN, 0);

#define MCSDL_GPIO_SCL_SET_OUTPUT()					mt_set_gpio_dir(GPIO_I2C1_SCA_PIN, GPIO_DIR_OUT);
#define MCSDL_GPIO_SCL_SET_INPUT()					mt_set_gpio_dir(GPIO_I2C1_SCA_PIN, GPIO_DIR_IN);
#define MCSDL_GPIO_SCL_IS_HIGH()					((gpio_get_value(GPIO_I2C1_SCA_PIN)>0)?1:0)

#define MCSDL_GPIO_SDA_SET_OUTPUT()					mt_set_gpio_dir(GPIO_I2C1_SDA_PIN, GPIO_DIR_OUT);
#define MCSDL_GPIO_SDA_SET_INPUT()					mt_set_gpio_dir(GPIO_I2C1_SDA_PIN, GPIO_DIR_IN);

#define MCSDL_GPIO_SDA_IS_HIGH()					((mt_get_gpio_in(GPIO_I2C1_SDA_PIN)>0)?1:0)

#define MCSDL_SET_GPIO_I2C()						//        
#define MCSDL_SET_HW_I2C()							//        



//                                                            
//
//                        
//
//                                  
//
//                                                            

#define MCSDL_DELAY_1US			1
#define MCSDL_DELAY_3US			3
#define MCSDL_DELAY_7US			7
#define MCSDL_DELAY_10US		10
#define MCSDL_DELAY_20US		20
#define MCSDL_DELAY_40US		40

#define MCSDL_DELAY_100US		100
#define MCSDL_DELAY_150US		150
#define MCSDL_DELAY_300US		300

#define MCSDL_DELAY_25MS		25000
#define MCSDL_DELAY_30MS		30000
#define MCSDL_DELAY_40MS		40000
#define MCSDL_DELAY_60MS		60000

#define MCSDL_DELAY_100MS		100000
#define MCSDL_DELAY_500MS		500000


//                                                            
//
//                        
//
//                                                            
#if 0

#define MELFAS_DISABLE_BASEBAND_ISR()				____HERE!_____
#define MELFAS_DISABLE_WATCHDOG_TIMER_RESET()		____HERE!_____

#define MELFAS_ROLLBACK_BASEBAND_ISR()				____HERE!_____
#define MELFAS_ROLLBACK_WATCHDOG_TIMER_RESET()		____HERE!_____

#else

#define MELFAS_DISABLE_BASEBAND_ISR()				//        
#define MELFAS_DISABLE_WATCHDOG_TIMER_RESET()		//        

#define MELFAS_ROLLBACK_BASEBAND_ISR()				//        
#define MELFAS_ROLLBACK_WATCHDOG_TIMER_RESET()		//        

#endif



#endif

