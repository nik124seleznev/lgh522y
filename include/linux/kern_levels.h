#ifndef __KERN_LEVELS_H__
#define __KERN_LEVELS_H__

#define KERN_SOH	"\001"		/*                       */
#define KERN_SOH_ASCII	'\001'

#define KERN_EMERG	KERN_SOH "0"	/*                    */
#define KERN_ALERT	KERN_SOH "1"	/*                                  */
#define KERN_CRIT	KERN_SOH "2"	/*                     */
#define KERN_ERR	KERN_SOH "3"	/*                  */
#define KERN_WARNING	KERN_SOH "4"	/*                    */
#define KERN_NOTICE	KERN_SOH "5"	/*                                  */
#define KERN_INFO	KERN_SOH "6"	/*               */
#define KERN_DEBUG	KERN_SOH "7"	/*                      */

#define KERN_DEFAULT	KERN_SOH "d"	/*                             */

/*
                                                                       
                                                                    
                                                                    
 */
#define KERN_CONT	""

#endif
