/*                                                                              
  
            
            
                          
  
           
           
                    
  
               
               
                           
  
          
          
                 
  
  
                                                                                
             
             
         
    
  
                                                                               */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/vmalloc.h>
#include <linux/wait.h>
#include <linux/spinlock.h>
#include <linux/semaphore.h>
#include <mach/mt_typedefs.h>

#ifndef _EXTERNAL_CODEC_DRIVER_H_
#define _EXTERNAL_CODEC_DRIVER_H_

/*                            */
#define CS4398_CHIPID   0x01    /*         */
#define CS4398_MODE 0x02    /*              */
#define CS4398_MIXING   0x03
#define CS4398_MUTE 0x04    /*              */
#define CS4398_VOLA 0x05    /*                              */
#define CS4398_VOLB 0x06    /*                              */
#define CS4398_RAMP 0x07
#define CS4398_MISC1    0x08
#define CS4398_MISC2    0x09

#define CS4398_FIRSTREG 0x01
#define CS4398_LASTREG  0x09
#define CS4398_NUMREGS  (CS4398_LASTREG - CS4398_FIRSTREG + 1)

typedef enum
{
    DIF_LEFT_JUSTIFIED,
    DIF_I2S,
    DIF_RIGHT_JUSTIFIED_16BIT,
    DIF_RIGHT_JUSTIFIED_24BIT,
    DIF_RIGHT_JUSTIFIED_20BIT,
    DIF_RIGHT_JUSTIFIED_18BIT,
    NUM_OF_DIF
} DIGITAL_INTERFACE_FORMAT;


enum ECODEC_CONTROL_SUBCOMMAND
{
    ECODEC_GETREGISTER_VALUE,
    ECODEC_SETREGISTER_VALUE,
};

enum AUDIO_ECODEC_CONTROL_COMMAND
{
    NUM_AUD_ECODEC_COMMAND,
};

typedef struct
{
    unsigned long int   command;
    unsigned long int   param1;
    unsigned long int   param2;
} ECODEC_Control;

void ExtCodec_Init(void);
void ExtCodec_PowerOn(void);
void ExtCodec_PowerOff(void);
bool ExtCodec_Register(void);
void ExtCodec_Mute(void);
void ExtCodec_SetGain(u8 leftright, u8 gain);
u8 ExtCodec_ReadReg(u8 addr);
void ExtCodec_DumpReg(void);
void ExtCodecDevice_Suspend(void);
void ExtCodecDevice_Resume(void);
void cust_extcodec_gpio_on(void);
void cust_extcodec_gpio_off(void);
void cust_extHPAmp_gpio_on(void);
void cust_extHPAmp_gpio_off(void);
void cust_extPLL_gpio_config(void);
#endif
