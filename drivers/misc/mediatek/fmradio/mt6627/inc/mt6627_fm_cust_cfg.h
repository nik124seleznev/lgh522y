/*
 *
 * (C) Copyright 20011
 * MediaTek <www.MediaTek.com>
 * Hongcheng Xia<Hongcheng.Xia@MediaTek.com>
 *
 * FM Radio Driver
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __FM_CUST_CFG_H__
#define __FM_CUST_CFG_H__

/*                     */
enum {
	FM_SCAN_SORT_NON = 0,
	FM_SCAN_SORT_UP,
	FM_SCAN_SORT_DOWN,
	FM_SCAN_SORT_MAX
};
/*                 
                             
                            
                          
                   
                 
                   
                  
                    
                  
                      
                    
                       

               
                                 
                    
*/
/*                                                                                           */
/*                                                                                                */
/*                                                                                           */
/*    */
#define FM_RX_RSSI_TH_LONG_MT6627    -296	/*                                              */
#define FM_RX_RSSI_TH_SHORT_MT6627   -296	/*                                               */
#define FM_RX_DESENSE_RSSI_MT6627   -258
#define FM_RX_PAMD_TH_MT6627          -12
#define FM_RX_MR_TH_MT6627           -67
#define FM_RX_ATDC_TH_MT6627           3496
#define FM_RX_PRX_TH_MT6627           64
#define FM_RX_SMG_TH_MT6627          16421	/*                             */
#define FM_RX_DEEMPHASIS_MT6627       0	/*                                             */
#define FM_RX_OSC_FREQ_MT6627         0	/*                                                        */
/*                                                                                           */
/*                                                                                   */
/*                                                                                                                                        */
/*                                                        */
/*                                         */
/*                                          */
/*                                            */
/*                                         */
/*                                         */
/*                                         */
/*                                         */

/*    */
/*                                         */
/*                                                                                           */
/*                                                                                           */


/*                                                                                           */
/*                                                                                               */
/*                                                                                           */

/*                                                      */
/*                  */
/*                                                          */
/*           */
#define FM_AUTO_HILO_OFF_MT6627    0
#define FM_AUTO_HILO_ON_MT6627     1


/*                */
#define FM_SEEKTH_LEVEL_DEFAULT_MT6627 4

#endif				/*                   */
