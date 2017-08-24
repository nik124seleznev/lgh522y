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



#ifndef _STP_WMT_H
#define _STP_WMT_H

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

typedef enum {
    BTM_RST_OP = 0,
    BTM_DMP_OP = 1,
    BTM_GET_AEE_SUPPORT_FLAG= 2,
    BTM_MAX_OP,
}MTKSTP_BTM_WMT_OP_T;

typedef enum {
    SLEEP = 0,
    HOST_AWAKE,
    WAKEUP,
    EIRQ,
    ROLL_BACK,
    STP_PSM_MAX_ACTION
}MTKSTP_PSM_ACTION_T;

/*                                                                              
                                                  
                                                                                
*/

/*                                                                              
                                                   
                                                                                
*/


/*                                                                              
                                                            
                                                                                
*/
extern MTK_WCN_BOOL wmt_lib_btm_cb (
    MTKSTP_BTM_WMT_OP_T op
    );

extern INT32 wmt_lib_ps_stp_cb (
    MTKSTP_PSM_ACTION_T action
    );
extern MTK_WCN_BOOL wmt_lib_is_quick_ps_support (VOID);

/*                                                                              
                                                
                                                                                
*/

#endif /*            */



