/*
* Copyright (C) 2011-2014 MediaTek Inc.
* 
* This program is free software: you can redistribute it and/or modify it under the terms of the 
* GNU General Public License version 2 as published by the Free Software Foundation.
* 
* This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
* without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
* See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with this program.
* If not, see <http://www.gnu.org/licenses/>.
*/

/*          
                                            

                                                                                           
*/



#ifndef _WMT_CONF_H_
#define _WMT_CONF_H_

/*                                                                              
                                                     
                                                                                
*/

/*                                                                              
                                             
                                                                                
*/
#define CUST_CFG_WMT "WMT_SOC.cfg"
#define CUST_CFG_WMT_PREFIX "/system/etc/firmware/"

/*                                                                              
                                                          
                                                                                
*/



/*                                                                              
                                                
                                                                                
*/



/*                                                                              
                                                 
                                                                                
*/
 



/*                                                                              
                                                  
                                                                                
*/

/*                                                                              
                                                   
                                                                                
*/





/*                                                                              
                                                            
                                                                                
*/

/*                                                                              
                                                
                                                                                
*/
INT32  wmt_conf_read_file(VOID);
P_WMT_GEN_CONF wmt_conf_get_cfg(VOID);
INT32  wmt_conf_set_cfg_file(const char *name);


#endif /*              */







