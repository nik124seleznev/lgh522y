/********************************************************************************
* (c) COPYRIGHT 2010 RAONTECH, Inc. ALL RIGHTS RESERVED.
*
* This software is the property of RAONTECH and is furnished under license by RAONTECH.
* This software may be used only in accordance with the terms of said license.
* This copyright noitce may not be remoced, modified or obliterated without the prior
* written permission of RAONTECH, Inc.
*
* This software may not be copied, transmitted, provided to or otherwise made available
* to any other person, company, corporation or other entity except as specified in the
* terms of said license.
*
* No right, title, ownership or other interest in the software is hereby granted or transferred.
*
* The information contained herein is subject to change without notice and should
* not be construed as a commitment by RAONTECH, Inc.
*
* TITLE 	  : RAONTECH TV configuration header file.
*
* FILENAME    : raontv_port.h
*
* DESCRIPTION :
*		Configuration for RAONTECH TV Services.
*
********************************************************************************/

/*                                                                               
                  
 
                              
                                                                                 
                                       
                                                                               */

#ifndef __RAONTV_PORT_H__
#define __RAONTV_PORT_H__

/*                                                                             
                                               
                                                                              */
#if defined(__KERNEL__) /*              */
    #include <linux/io.h>
    #include <linux/kernel.h>
    #include <linux/delay.h>
    #include <linux/mm.h>
    #include <linux/mutex.h>
    #include <linux/uaccess.h>
    #include <linux/jiffies.h>

#elif defined(WINCE)
    #include <windows.h>
    #include <drvmsg.h>

#else
	#include <stdio.h>
#endif

#ifdef __cplusplus
extern "C"{
#endif

/*                                                                            
 
                       
 
                                                                            */
/*                                                                            
                                                               
                                                                            */
#define RAONTV_CHIP_ADDR	0x86

/*                                                                            
                                             
                                                                            */
#define BOOL		int
#define S8		s8
#define U8		u8
#define S16		s16
#define U16		u16
#define S32		s32
#define U32		u32

#define INT		int
#define UINT		unsigned int
#define LONG		long
#define ULONG		unsigned long


typedef volatile U8			VU8;
typedef volatile U16		VU16;
typedef volatile U32		VU32;

#if defined(__GNUC__)
	#define INLINE		inline
#elif defined(_WIN32)
	#define INLINE		__inline
#elif defined(__ARMCC_VERSION)
	#define INLINE		__inline
#else
	/*                  */
	#define INLINE		inline
#endif

/*                                                                            
                                           
                                                                            */
#define RTV_ISDBT_ENABLE

/*                                                                            
                                                     
                                                                            */
#define RAONTV_CHIP_PKG_WLCSP	//       
//                                      

/*                                                                            
                                 
                                                                            */
#define RAONTV_CHIP_INTEGER_MODE_EN

/*                                                                            
                                               
                                                 
                                                                              
                                                    
                                                    
                                                    
                                                                        
                                                                            */
//                                    
//                                    
#define RTV_SRC_CLK_FREQ_KHz			26000
//                                   

/*                                                                            
                        
                                                                             */
//                        
#define RTV_PWR_LDO


/*                                                                            
                             
                                                                            */
//                                                           
//                                                      
#define RTV_IF_SPI //                    
//                                              
//                                                        

/*                                 
                              
                                   */
#if defined(RTV_IF_MPEG2_SERIAL_TSIF) || defined(RTV_IF_QUALCOMM_TSIF)\
|| defined(RTV_IF_MPEG2_PARALLEL_TSIF)
	#define RTV_IF_TSIF /*          */
#endif

#if defined(RTV_IF_MPEG2_SERIAL_TSIF) || defined(RTV_IF_QUALCOMM_TSIF)
	#define RTV_IF_SERIAL_TSIF /*             */
#endif


/*                                                                            
                                          
                                                                            */
#if defined(__KERNEL__) /*              */
	static INLINE void RTV_DELAY_MS(UINT ms)
	{
		unsigned long start_jiffies, end_jiffies;
		UINT diff_time;
		UINT _1ms_cnt = ms;

		start_jiffies = get_jiffies_64();

		do {
			end_jiffies = get_jiffies_64();

			diff_time = jiffies_to_msecs(end_jiffies - start_jiffies);
			if (diff_time >= ms)
				break;

			mdelay(1);
		} while (--_1ms_cnt);
	}

#elif defined(WINCE)
	#define RTV_DELAY_MS(ms)    Sleep(ms)

#else
	extern void mtv_delay_ms(int ms);
	#define RTV_DELAY_MS(ms) 	mtv_delay_ms(ms) //     
#endif

/*                                                                            
                                  
                                                                            */
#if 1
	#define RTV_DBGMSG0(fmt)			printk(fmt)
	#define RTV_DBGMSG1(fmt, arg1)			printk(fmt, arg1)
	#define RTV_DBGMSG2(fmt, arg1, arg2)		printk(fmt, arg1, arg2)
	#define RTV_DBGMSG3(fmt, arg1, arg2, arg3)	printk(fmt, arg1, arg2, arg3)
#else
	/*                                   */
	#define RTV_DBGMSG0(fmt)					((void)0)
	#define RTV_DBGMSG1(fmt, arg1)				((void)0)
	#define RTV_DBGMSG2(fmt, arg1, arg2)		((void)0)
	#define RTV_DBGMSG3(fmt, arg1, arg2, arg3)	((void)0)
#endif
/*                              */


/*                                                                            
 
                                
 
                                                                            */


/*                                                  */
//                                    

#ifndef RTV_ERROR_TSP_OUTPUT_DISABLE
	/*                                                         */
	//                             

#endif /*                              */


/*                                                                            
                                    
                                                              
                                                                            */
#if defined(RTV_ISDBT_ENABLE)
		#define RAONTV_CHIP_PKG_WLCSP_HRM_ON
#endif

/*                                                                            
                                                 
                                                                            */
#define RTV_INTR_POLARITY_LOW_ACTIVE
/*                                       */

/*                                                                            
 
                                        
 
                                                                            */
extern U8 g_bRtvPage;

#if defined(RTV_IF_TSIF) || defined(RTV_IF_SPI_SLAVE)
	/*                                                                 
                                                         
                                                                  */
	//                         
	//                         
	#define RTV_TSIF_FORMAT_3
	//                         
	//                         

	//                                         
	//                                        
	#define RTV_TSIF_CLK_SPEED_DIV_6 //     
	//                                          

	/*                                                                 
                                   
                                                                  */
	unsigned char mtv_i2c_read(U8 reg);
	void mtv_i2c_read_burst(U8 reg, U8 *buf, int size);
	void mtv_i2c_write(U8 reg, U8 val);
	#define	RTV_REG_GET(reg)	mtv_i2c_read((U8)(reg))
	#define	RTV_REG_BURST_GET(reg, buf, size)	mtv_i2c_read_burst((U8)(reg), buf, size)
	#define	RTV_REG_SET(reg, val)	mtv_i2c_write((U8)(reg), (U8)(val))
	#define	RTV_REG_MASK_SET(reg, mask, val)\
		do {					\
			U8 tmp;				\
			tmp = (RTV_REG_GET(reg)|(U8)(mask)) & (U8)((~(mask))|(val));\
			RTV_REG_SET(reg, tmp);		\
		} while(0)

#elif defined(RTV_IF_SPI)
	/*                                                                 
                                   
                                                                  */
	unsigned char mtv222_spi_read(U8 page, U8 reg);
	void mtv222_spi_read_burst(U8 page, U8 reg, U8 *buf, int size);
	void mtv222_spi_write(U8 page, U8 reg, U8 val);

	#define	RTV_REG_GET(reg)			(U8)mtv222_spi_read(g_bRtvPage, (U8)(reg))
	#define	RTV_REG_BURST_GET(reg, buf, size)	mtv222_spi_read_burst(g_bRtvPage, (U8)(reg), buf, (size))
	#define	RTV_REG_SET(reg, val)			mtv222_spi_write(g_bRtvPage, (U8)(reg), (U8)(val))
	#define	RTV_REG_MASK_SET(reg, mask, val)\
		do {					\
			U8 tmp;				\
			tmp = (RTV_REG_GET(reg)|(U8)(mask)) & (U8)((~(mask))|(val));\
			RTV_REG_SET(reg, tmp);		\
		} while(0)

#elif defined(RTV_IF_EBI2)
	unsigned char mtv_ebi2_read(unsigned char reg);
	void mtv_ebi2_read_burst(unsigned char reg, unsigned char *buf, int size);
	void mtv_ebi2_write(unsigned char reg, unsigned char val);

#define	RTV_REG_GET(reg)  (U8)mtv_ebi2_read((U8)(reg))
#define	RTV_REG_BURST_GET(reg, buf, size) mtv_ebi2_read_burst((U8)(reg), buf, size)
#define	RTV_REG_SET(reg, val) mtv_ebi2_write((U8)(reg), (U8)(val))
#define	RTV_REG_MASK_SET(reg, mask, val) \
	do { \
		U8 tmp; \
		tmp = (RTV_REG_GET(reg)|(U8)(mask)) & (U8)((~(mask))|(val)); \
		RTV_REG_SET(reg, tmp); \
	} while (0)

#else
	#error "Must define the interface definition !"
#endif

/*                                                                            
 
                              
                                            
 
                                                                            */

#define RTV_SPI_MSC1_ENABLED /*             */

#if (defined(RTV_TDMB_ENABLE)||defined(RTV_DAB_ENABLE))\
&& !(defined(RTV_ISDBT_ENABLE)||defined(RTV_FM_ENABLE))
	/*                           */
	#define RTV_TDMBorDAB_ONLY_ENABLED

#elif !(defined(RTV_TDMB_ENABLE)||defined(RTV_DAB_ENABLE) || defined(RTV_FM_ENABLE))\
&& defined(RTV_ISDBT_ENABLE)
	/*                    */
	#define RTV_ISDBT_ONLY_ENABLED

#elif (defined(RTV_TDMB_ENABLE)||defined(RTV_DAB_ENABLE)) && defined(RTV_FM_ENABLE)\
&& !defined(RTV_ISDBT_ENABLE)
		/*                                */
	#define RTV_TDMBorDAB_FM_ENABLED
#endif


/*                                                                            
                             
                                                                            */
#if defined(RTV_IF_SPI) || defined(RTV_FIC_I2C_INTR_ENABLED)
	#if defined(__KERNEL__)
		extern struct mutex raontv_guard;
		#define RTV_GUARD_INIT		mutex_init(&raontv_guard)
		#define RTV_GUARD_LOCK		mutex_lock(&raontv_guard)
		#define RTV_GUARD_FREE		mutex_unlock(&raontv_guard)
		#define RTV_GUARD_DEINIT 	((void)0)

    #elif defined(WINCE)
	        extern CRITICAL_SECTION		raontv_guard;
	        #define RTV_GUARD_INIT		InitializeCriticalSection(&raontv_guard)
	        #define RTV_GUARD_LOCK		EnterCriticalSection(&raontv_guard)
	        #define RTV_GUARD_FREE		LeaveCriticalSection(&raontv_guard)
	        #define RTV_GUARD_DEINIT	DeleteCriticalSection(&raontv_guard)
	#else
		//           
		#define RTV_GUARD_INIT		((void)0)
		#define RTV_GUARD_LOCK		((void)0)
		#define RTV_GUARD_FREE 	((void)0)
		#define RTV_GUARD_DEINIT 	((void)0)
	#endif

#else
	#define RTV_GUARD_INIT		((void)0)
	#define RTV_GUARD_LOCK		((void)0)
	#define RTV_GUARD_FREE 	((void)0)
	#define RTV_GUARD_DEINIT 	((void)0)
#endif


/*                                                                            
 
                                     
 
                                                                            */
#if !defined(RAONTV_CHIP_PKG_WLCSP) && !defined(RAONTV_CHIP_PKG_QFN)
	#error "Must define the package type !"
#endif

#if !defined(RTV_PWR_EXTERNAL) && !defined(RTV_PWR_LDO)
	#error "Must define the power type !"
#endif

#if defined(RTV_IF_TSIF) || defined(RTV_IF_SPI_SLAVE)\
|| defined(RTV_IF_SPI)
    #if (RAONTV_CHIP_ADDR >= 0xFF)
	#error "Invalid chip address"
    #endif

#elif defined(RTV_IF_EBI2)

#else
	#error "Must define the interface definition !"
#endif

void rtvOEM_ConfigureInterrupt(void);
void rtvOEM_PowerOn(int on);

#ifdef __cplusplus
}
#endif

#endif /*                   */

