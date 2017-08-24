/*                                                                             
 
  
            
            
                    
  
           
           
                                  
  
               
               
                   
  
          
          
                             
  
                                                                             
   
                  
             
         
  
  

                                                                              */

#ifndef AUDIO_DEF_H
#define AUDIO_DEF_H

//                   
#ifndef int8
typedef signed char int8;
#endif

#ifndef uint8
typedef unsigned char uint8;
#endif

#ifndef int16
typedef short int16;
#endif

#ifndef uint16
typedef unsigned short uint16;
#endif

#ifndef int32
typedef int int32;
#endif

#ifndef uint32
typedef unsigned int uint32;
#endif

#ifndef int64
typedef long long int64;
#endif

#ifndef uint64
typedef unsigned long long uint64;
#endif

#define PM_MANAGER_API
#define AUDIO_MEMORY_SRAM
#define AUDIO_MEM_IOREMAP
#define MTK_AUDIO_DYNAMIC_SRAM_SUPPORT


//                          
#define DEBUG_AUDDRV
#define DEBUG_AFE_REG
#define DEBUG_ANA_REG
#define DEBUG_AUD_CLK

/*
                   
                                                                                                
     
                                      
      

                    
                                                                                                 
     
                                       
      

                    
                                                                                                 
     
                                       
      

                    
                                                                                                 
     
                                       
      

                                                                                                   
*/

#ifdef DEBUG_AUDDRV
#define PRINTK_AUDDRV(format, args...) printk(format, ##args )
#else
#define PRINTK_AUDDRV(format, args...)
#endif

#ifdef DEBUG_AFE_REG
#define PRINTK_AFE_REG(format, args...) printk(format, ##args )
#else
#define PRINTK_AFE_REG(format, args...)
#endif

#ifdef DEBUG_ANA_REG
#define PRINTK_ANA_REG(format, args...) printk(format, ##args )
#else
#define PRINTK_ANA_REG(format, args...)
#endif

#ifdef DEBUG_AUD_CLK
#define PRINTK_AUD_CLK(format, args...)  printk(format, ##args )
#else
#define PRINTK_AUD_CLK(format, args...)
#endif

#define PRINTK_AUD_ERROR(format, args...)  printk(format, ##args )

//                                        
#define AUDIO_ASSERT(value) BUG_ON(false)


/*                                 
                                   
                                  */
#define BIT_00  0x00000001        /*                                         */
#define BIT_01  0x00000002        /*                                         */
#define BIT_02  0x00000004        /*                                         */
#define BIT_03  0x00000008        /*                                         */
#define BIT_04  0x00000010        /*                                         */
#define BIT_05  0x00000020        /*                                         */
#define BIT_06  0x00000040        /*                                         */
#define BIT_07  0x00000080        /*                                         */
#define BIT_08  0x00000100        /*                                         */
#define BIT_09  0x00000200        /*                                         */
#define BIT_10  0x00000400        /*                                         */
#define BIT_11  0x00000800        /*                                         */
#define BIT_12  0x00001000        /*                                         */
#define BIT_13  0x00002000        /*                                         */
#define BIT_14  0x00004000        /*                                         */
#define BIT_15  0x00008000        /*                                         */
#define BIT_16  0x00010000        /*                                         */
#define BIT_17  0x00020000        /*                                         */
#define BIT_18  0x00040000        /*                                         */
#define BIT_19  0x00080000        /*                                         */
#define BIT_20  0x00100000        /*                                         */
#define BIT_21  0x00200000        /*                                         */
#define BIT_22  0x00400000        /*                                         */
#define BIT_23  0x00800000        /*                                         */
#define BIT_24  0x01000000        /*                                         */
#define BIT_25  0x02000000        /*                                         */
#define BIT_26  0x04000000        /*                                         */
#define BIT_27  0x08000000        /*                                         */
#define BIT_28  0x10000000        /*                                         */
#define BIT_29  0x20000000        /*                                         */
#define BIT_30  0x40000000        /*                                         */
#define BIT_31  0x80000000        /*                                         */

#endif


