#include <cust_leds.h>
#include <cust_leds_def.h>
#include <mach/mt_pwm.h>

#include <linux/kernel.h>
#include <mach/upmu_common_sw.h>
#include <mach/upmu_hw.h>

//                                                         
//                                            
extern int disp_bls_set_backlight(unsigned int level);

//                                                                               
#define BACKLIGHT_LEVEL_PWM_64_FIFO_MODE_SUPPORT 64
//                                                                           
#define BACKLIGHT_LEVEL_PWM_256_SUPPORT 256

//                                                                                                              
#define BACKLIGHT_LEVEL_PWM_MODE_CONFIG BACKLIGHT_LEVEL_PWM_256_SUPPORT

unsigned int Cust_GetBacklightLevelSupport_byPWM(void)
{
	return BACKLIGHT_LEVEL_PWM_MODE_CONFIG;
}

unsigned int brightness_mapping(unsigned int level)
{
    unsigned int mapped_level;
	if (level > 255)
	{
    	mapped_level = (level - 65536)/4;
	}
	else
	{
    mapped_level = level;
	}
	return mapped_level;
}

extern int chargepump_set_backlight_level(unsigned int level);
unsigned int Cust_SetBacklight(int level, int div)
{
    //                             
    //                                                     
    //                                                  
#if 1
    chargepump_set_backlight_level(brightness_mapping(level));
#endif
    return 0;
}

/*

                                                                        
                                
                                           
           
                          
                         
                          
                          
                              
                                 
  
                                                
             
                                 
                                 
                                 
                                 
                                
  
                                                                                    
                     
                     
            
                     
                      
                                                                                             
    
                                                                                             
                                  
                     
                                                    
                                                                                                           
                                                                   
                                                                    
  
               
                                                                        
                                            
  
                                                                                 
                                                                                                  
                                                                                                       
               
                                                 
                                 
          
  
                                                  
                                                                                      
                                                                                                   
                                                                                             
                                   
                     
                                                        
                                                       
                         
                          
                     
  
                                               
               
             
              
                
              
             
  
                                                                
                                                                                             
 */
static struct cust_mt65xx_led cust_led_list[MT65XX_LED_TYPE_TOTAL] = {
/*                                                 */
/*                                                                                    */
    {"red",               MT65XX_LED_MODE_PMIC, MT65XX_LED_PMIC_NLED_ISINK0 ,{0}},
    {"green",             MT65XX_LED_MODE_NONE, -1,{0}},
    {"blue",              MT65XX_LED_MODE_NONE, -1,{0}},
/*                                                 */
    {"jogball-backlight", MT65XX_LED_MODE_NONE, -1,{0}},
	{"keyboard-backlight",MT65XX_LED_MODE_NONE, -1,{0}},
	{"button-backlight",  MT65XX_LED_MODE_NONE, -1,{0}},
	{"lcd-backlight",     MT65XX_LED_MODE_CUST_LCM, (long)Cust_SetBacklight, {0}},
};

struct cust_mt65xx_led *get_cust_led_list(void)
{
	return cust_led_list;
}

