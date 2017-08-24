/*
                                                                                      
*/

/*                             
          
 */



#ifndef _P2P_NIC_CMD_EVENT_H
#define _P2P_NIC_CMD_EVENT_H

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

/*                                                                              
                                                             
                                                                                 
 */


typedef struct _EVENT_P2P_DEV_DISCOVER_RESULT_T {
/*                                                                                   */
	UINT_8			aucDeviceAddr[MAC_ADDR_LEN];	/*                 */
	UINT_8			aucInterfaceAddr[MAC_ADDR_LEN];	/*                 */
	UINT_8			ucDeviceCapabilityBitmap;
	UINT_8			ucGroupCapabilityBitmap;
	UINT_16			u2ConfigMethod;	/*                   */
	P2P_DEVICE_TYPE_T	rPriDevType;
	UINT_8			ucSecDevTypeNum;
	P2P_DEVICE_TYPE_T	arSecDevType[2];
	UINT_16			u2NameLength;
	UINT_8			aucName[32];
	PUINT_8			pucIeBuf;
	UINT_16			u2IELength;
	UINT_8			aucBSSID[MAC_ADDR_LEN];
	/*                                                */
} EVENT_P2P_DEV_DISCOVER_RESULT_T, *P_EVENT_P2P_DEV_DISCOVER_RESULT_T;

#endif
