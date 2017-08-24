#include <cust_leds.h>
#include <cust_leds_def.h>
#include <mach/mt_pwm.h>

#include <linux/kernel.h>
#include <mach/pmic_mt6329_hw_bank1.h> 
#include <mach/pmic_mt6329_sw_bank1.h> 
#include <mach/pmic_mt6329_hw.h>
#include <mach/pmic_mt6329_sw.h>
#include <mach/upmu_common_sw.h>
#include <mach/upmu_hw.h>

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
    
    mapped_level = level;
       
	return mapped_level;
}
/*
                                                  
 
                  
                                   
                                                           


                                                                        
                                
                                           
           
                           
                          
                           
                           
                               
                                 
  
                                                
             
                                  
                                 
                                 
                                 
                                
   
                                                                                    
                       
                     
             
                     
                      
                                                                                             
    
                                                                                             
                                  
                     
                                                    
                                                                                                           
                                                                   
                                                                    
  
               
                                                                        
                                              
  
                                                                                 
                                                                                                  
                                                                                                       
                
                                                 
                                 
          
  
                                                   
                                                                                      
                                                                                                   
                                                                                             
                                   
                     
                                                        
                                                       
                         
                          
                     
  
                                               
                
             
              
                
              
             
  
                                                                 
                                                                                             
 */

extern int chargepump_set_backlight_level(unsigned int level);
unsigned int Cust_SetBacklight(int level, int div)
{
#ifdef CONFIG_BACKLIGHT_LM3639	
    chargepump_set_backlight_level(level);
#endif
    return 0;
}
/*
                        
                       
                   
*/
static struct cust_leds_param cust_leds_hw = {
	.name = "charge-pump",
	.args = (void*)1, /*            */
};

static struct cust_mt65xx_led cust_led_list[MT65XX_LED_TYPE_TOTAL] = {
#if 1 //                                                                     
	{"red",               MT65XX_LED_MODE_PMIC, MT65XX_LED_PMIC_NLED_ISINK0,{0}},
	{"green",             MT65XX_LED_MODE_NONE, -1,{0}},
#else
	{"red",               MT65XX_LED_MODE_PMIC, MT65XX_LED_PMIC_NLED_ISINK2,{0}},
	{"green",             MT65XX_LED_MODE_PMIC, MT65XX_LED_PMIC_NLED_ISINK0,{0}},
#endif
	{"blue",              MT65XX_LED_MODE_NONE, -1,{0}},
	{"jogball-backlight", MT65XX_LED_MODE_NONE, -1,{0}},
	{"keyboard-backlight",MT65XX_LED_MODE_NONE, -1,{0}},
	{"button-backlight",  MT65XX_LED_MODE_NONE, -1,{0}},
#if defined (CONFIG_BACKLIGHT_LM3639)
	{"lcd-backlight",     MT65XX_LED_MODE_CUST_LCM, Cust_SetBacklight, {0}},
#else	/*                   */
	{"lcd-backlight",     MT65XX_LED_MODE_CUST_BLS_PWM, (int)disp_bls_set_backlight,{0}},
#endif
};

struct cust_mt65xx_led *get_cust_led_list(void)
{
	return cust_led_list;
}

struct cust_leds_param* get_cust_leds_param(void)
{
    return &cust_leds_hw;
}
