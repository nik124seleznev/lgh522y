/*****************************************************************************
 *                                                                           *
 * Copyright (c) David L. Mills 1993                                         *
 *                                                                           *
 * Permission to use, copy, modify, and distribute this software and its     *
 * documentation for any purpose and without fee is hereby granted, provided *
 * that the above copyright notice appears in all copies and that both the   *
 * copyright notice and this permission notice appear in supporting          *
 * documentation, and that the name University of Delaware not be used in    *
 * advertising or publicity pertaining to distribution of the software       *
 * without specific, written prior permission.  The University of Delaware   *
 * makes no representations about the suitability this software for any      *
 * purpose.  It is provided "as is" without express or implied warranty.     *
 *                                                                           *
 *****************************************************************************/

/*
                               
  
                         
                                                                       
                   
  
                          
                                                               
  
                           
                                                      
  
                           
                                                                  
                                   
  
                           
                                                                 
                     
  
                           
                                                             
            
  
                              
                                             
                            
                             
                             
                                                         
                             
                                          
                                  
                                         
 */
#ifndef _LINUX_TIMEX_H
#define _LINUX_TIMEX_H

#include <uapi/linux/timex.h>

#define ADJ_ADJTIME		0x8000	/*                                       */
#define ADJ_OFFSET_SINGLESHOT	0x0001	/*                       */
#define ADJ_OFFSET_READONLY	0x2000	/*                   */
#include <linux/compiler.h>
#include <linux/types.h>
#include <linux/param.h>

#include <asm/timex.h>

/*
                                                                
                                                                  
                                                                 
                                                               
                                                               
                                                                  
                                                                    
                                                      
  
                                                                      
                                                                 
  
                                                                   
  
                                                                
                                                               
                                                                  
                                                   
  
                                                                     
                                                                     
                                       
  
  
                                                                
                                                                  
                                                                      
  
                                                          
 */
#define SHIFT_PLL	2	/*                              */
#define SHIFT_FLL	2	/*                              */
#define MAXTC		10	/*                               */

/*
                                                              
                                                                  
                                          
 */
#define SHIFT_USEC 16		/*                                */
#define PPM_SCALE ((s64)NSEC_PER_USEC << (NTP_SCALE_SHIFT - SHIFT_USEC))
#define PPM_SCALE_INV_SHIFT 19
#define PPM_SCALE_INV ((1LL << (PPM_SCALE_INV_SHIFT + NTP_SCALE_SHIFT)) / \
		       PPM_SCALE + 1)

#define MAXPHASE 500000000L	/*                      */
#define MAXFREQ 500000		/*                            */
#define MAXFREQ_SCALED ((s64)MAXFREQ << NTP_SCALE_SHIFT)
#define MINSEC 256		/*                                  */
#define MAXSEC 2048		/*                                  */
#define NTP_PHASE_LIMIT ((MAXPHASE / NSEC_PER_USEC) << 5) /*                        */

/*
                   
                                                                     
                                    
 */
extern unsigned long tick_usec;		/*                       */
extern unsigned long tick_nsec;		/*                          */

/*                                          */
#define shift_right(x, s) ({	\
	__typeof__(x) __x = (x);	\
	__typeof__(s) __s = (s);	\
	__x < 0 ? -(-__x >> __s) : __x >> __s;	\
})

#define NTP_SCALE_SHIFT		32

#define NTP_INTERVAL_FREQ  (HZ)
#define NTP_INTERVAL_LENGTH (NSEC_PER_SEC/NTP_INTERVAL_FREQ)

extern int do_adjtimex(struct timex *);
extern void hardpps(const struct timespec *, const struct timespec *);

int read_current_timer(unsigned long *timer_val);
void ntp_notify_cmos_timer(void);

/*                                            */
#define PIT_TICK_RATE 1193182ul

#endif /*               */
