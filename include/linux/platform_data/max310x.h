/*
 *  Maxim (Dallas) MAX3107/8 serial driver
 *
 *  Copyright (C) 2012 Alexander Shiyan <shc_work@mail.ru>
 *
 *  Based on max3100.c, by Christian Pellegrin <chripell@evolware.org>
 *  Based on max3110.c, by Feng Tang <feng.tang@intel.com>
 *  Based on max3107.c, by Aavamobile
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 */

#ifndef _MAX310X_H_
#define _MAX310X_H_

/*
                                     
  
                                                
                                   
                                                                 
                        
                   
     
  
                                                        
    
                          
                      
                 
                     
                                    
     
     
 */

#define MAX310X_MAX_UARTS	1

/*                                 */
struct max310x_pdata {
	/*                        */
	const u8		driver_flags:2;
#define MAX310X_EXT_CLK		(0x00000001)	/*                       */
#define MAX310X_AUTOSLEEP	(0x00000002)	/*                       */
	/*                           */
	const u8		uart_flags[MAX310X_MAX_UARTS];
#define MAX310X_LOOPBACK	(0x00000001)	/*                      */
#define MAX310X_ECHO_SUPRESS	(0x00000002)	/*                     */
#define MAX310X_AUTO_DIR_CTRL	(0x00000004)	/*                      
                         
       */
	/*                                       */
	const int		frequency;
	/*                                    */
	const int		gpio_base;
	/*                       */
	void (*init)(void);
	/*                      */
	void (*exit)(void);
	/*                  */
	void (*suspend)(int do_suspend);
};

#endif
