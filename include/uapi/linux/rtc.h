/*
 * Generic RTC interface.
 * This version contains the part of the user interface to the Real Time Clock
 * service. It is used with both the legacy mc146818 and also  EFI
 * Struct rtc_time and first 12 ioctl by Paul Gortmaker, 1996 - separated out
 * from <linux/mc146818rtc.h> to this file for 2.4 kernels.
 *
 * Copyright (C) 1999 Hewlett-Packard Co.
 * Copyright (C) 1999 Stephane Eranian <eranian@hpl.hp.com>
 */
#ifndef _UAPI_LINUX_RTC_H_
#define _UAPI_LINUX_RTC_H_

/*
                                                                       
                                                                    
                                                               
 */

struct rtc_time {
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
	int tm_isdst;
};

/*
                                                            
             
 */
struct rtc_wkalrm {
	unsigned char enabled;	/*                                       */
	unsigned char pending;  /*                                          */
	struct rtc_time time;	/*                          */
};

/*
                                                                   
                                                               
                                                           
                                                              
                                                    
  
                                               
                                    
                                               
                                    
 */

struct rtc_pll_info {
	int pll_ctrl;       /*                                 */
	int pll_value;      /*                          */
	int pll_max;        /*                                   */
	int pll_min;        /*                                   */
	int pll_posmult;    /*                           */
	int pll_negmult;    /*                           */
	long pll_clock;     /*                    */
};

/*
                                                               
                                      
 */

#define RTC_AIE_ON	_IO('p', 0x01)	/*                       */
#define RTC_AIE_OFF	_IO('p', 0x02)	/*           */
#define RTC_UIE_ON	_IO('p', 0x03)	/*                       */
#define RTC_UIE_OFF	_IO('p', 0x04)	/*           */
#define RTC_PIE_ON	_IO('p', 0x05)	/*                         */
#define RTC_PIE_OFF	_IO('p', 0x06)	/*           */
#define RTC_WIE_ON	_IO('p', 0x0f)  /*                         */
#define RTC_WIE_OFF	_IO('p', 0x10)  /*           */
#define RTC_AUTOBOOT_ON		_IO('p', 0x20)  /*                               */
#define RTC_AUTOBOOT_OFF	_IO('p', 0x21)  /*            */

#define RTC_ALM_SET	_IOW('p', 0x07, struct rtc_time) /*                 */
#define RTC_ALM_READ	_IOR('p', 0x08, struct rtc_time) /*                 */
#define RTC_RD_TIME	_IOR('p', 0x09, struct rtc_time) /*                 */
#define RTC_SET_TIME	_IOW('p', 0x0a, struct rtc_time) /*                 */
#define RTC_IRQP_READ	_IOR('p', 0x0b, unsigned long)	 /*                 */
#define RTC_IRQP_SET	_IOW('p', 0x0c, unsigned long)	 /*                 */
#define RTC_EPOCH_READ	_IOR('p', 0x0d, unsigned long)	 /*                 */
#define RTC_EPOCH_SET	_IOW('p', 0x0e, unsigned long)	 /*                 */

#define RTC_WKALM_SET	_IOW('p', 0x0f, struct rtc_wkalrm)/*                 */
#define RTC_WKALM_RD	_IOR('p', 0x10, struct rtc_wkalrm)/*                 */

#define RTC_PLL_GET	_IOR('p', 0x11, struct rtc_pll_info)  /*                    */
#define RTC_PLL_SET	_IOW('p', 0x12, struct rtc_pll_info)  /*                    */

#define RTC_VL_READ	_IOR('p', 0x13, int)	/*                      */
#define RTC_VL_CLR	_IO('p', 0x14)		/*                               */

/*                 */
#define RTC_IRQF 0x80	/*                                */
#define RTC_PF 0x40	/*                    */
#define RTC_AF 0x20	/*                 */
#define RTC_UF 0x10	/*                              */


#define RTC_MAX_FREQ	8192


#endif /*                    */
