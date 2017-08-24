/*
 * bq2415x charger driver
 *
 * Copyright (C) 2011-2012  Pali Rohár <pali.rohar@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef BQ2415X_CHARGER_H
#define BQ2415X_CHARGER_H

/*
                                                                    
                                                                         
                                                                       
        
  
                                                                
                                                                          
                                                
  
                                                                         
                                                                       
                                                                         
                       
  
                           
                                                         
                   
  
                                                                          
       
  
                                  
  
                                                                          
  
                                                                        
                                                                          
                                                                       
                                                                      
                                                                        
              
  
                                    
  
                                                    
  
                                          
                                                         
                         
  
                                                      
  
                                               
  
                                                   
  
                                                                        
                                                                          
                               
 */

/*                                            */
enum bq2415x_mode {
	BQ2415X_MODE_OFF,		/*                                 */
	BQ2415X_MODE_NONE,		/*                         */
	BQ2415X_MODE_HOST_CHARGER,	/*                              */
	BQ2415X_MODE_DEDICATED_CHARGER, /*                               */
	BQ2415X_MODE_BOOST,		/*                                */
};

struct bq2415x_platform_data {
	int current_limit;		/*    */
	int weak_battery_voltage;	/*    */
	int battery_regulation_voltage;	/*    */
	int charge_current;		/*    */
	int termination_current;	/*    */
	int resistor_sense;		/*       */
	int (*set_mode_hook)(void (*hook)(enum bq2415x_mode mode, void *data),
			     void *data);
};

#endif
