/*
    error.h - Part of libsensors, a Linux library for reading sensor data.
    Copyright (c) 1998, 1999  Frodo Looijaard <frodol@dds.nl>
    Copyright (C) 2007-2009   Jean Delvare <khali@linux-fr.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
    MA 02110-1301 USA.
*/

#ifndef LIB_SENSORS_ERROR_H
#define LIB_SENSORS_ERROR_H

#define SENSORS_ERR_WILDCARDS	1 /*                             */
#define SENSORS_ERR_NO_ENTRY	2 /*                          */
#define SENSORS_ERR_ACCESS_R	3 /*            */
#define SENSORS_ERR_KERNEL	4 /*                        */
#define SENSORS_ERR_DIV_ZERO	5 /*                */
#define SENSORS_ERR_CHIP_NAME	6 /*                       */
#define SENSORS_ERR_BUS_NAME	7 /*                      */
#define SENSORS_ERR_PARSE	8 /*                     */
#define SENSORS_ERR_ACCESS_W	9 /*             */
#define SENSORS_ERR_IO		10 /*           */
#define SENSORS_ERR_RECURSION	11 /*                              */

#ifdef __cplusplus
extern "C" {
#endif /*             */


/*                                                                       
                                                                         
                                  */
const char *sensors_strerror(int errnum);

/*                                                                         
                                                                          
                                                                        
                                                                         
                                                                
                                 
                                                                    
                                                                        
                                                                     
                                                                          
                      
                                                                      
                                                                    */
extern void (*sensors_parse_error) (const char *err, int lineno);
extern void (*sensors_parse_error_wfn) (const char *err,
					const char *filename, int lineno);

/*                                                                 
                                                                       
                                                                    
                                      */
extern void (*sensors_fatal_error) (const char *proc, const char *err);

#ifdef __cplusplus
}
#endif /*             */

#endif /*                         */
