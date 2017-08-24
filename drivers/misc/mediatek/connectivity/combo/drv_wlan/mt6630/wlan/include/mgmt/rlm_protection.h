/*
                                                                                               
*/

/*                            
          
*/



/*
                           
  
                      
                                                             
                                                  
                                                                      
  
                      
       
                                   
  
                   
  
                                                                               
  
                      
                                                            
                                
  
                      
                                                               
                                                    
  
                      
                                                               
                          
  
                      
                                                               
                                           
  
                      
                                                               
                                          
*/

#ifndef _RLM_PROTECTION_H
#define _RLM_PROTECTION_H

/*                                                                              
                                                     
                                                                                
*/

/*                                                                              
                                                          
                                                                                
*/

/*                                                                              
                                                
                                                                                
*/

/*                                                                              
                                                 
                                                                                
*/
typedef enum _ENUM_SYS_PROTECT_MODE_T {
	SYS_PROTECT_MODE_NONE = 0,	/*        */
	SYS_PROTECT_MODE_ERP,	/*        */
	SYS_PROTECT_MODE_NON_HT,	/*        */
	SYS_PROTECT_MODE_20M,	/*        */

	SYS_PROTECT_MODE_NUM
} ENUM_SYS_PROTECT_MODE_T, *P_ENUM_SYS_PROTECT_MODE_T;

/*                                                                */
typedef enum _ENUM_HT_PROTECT_MODE_T {
	HT_PROTECT_MODE_NONE = 0,
	HT_PROTECT_MODE_NON_MEMBER,
	HT_PROTECT_MODE_20M,
	HT_PROTECT_MODE_NON_HT,

	HT_PROTECT_MODE_NUM
} ENUM_HT_PROTECT_MODE_T, *P_ENUM_HT_PROTECT_MODE_T;


typedef enum _ENUM_GF_MODE_T {
	GF_MODE_NORMAL = 0,
	GF_MODE_PROTECT,
	GF_MODE_DISALLOWED,

	GF_MODE_NUM
} ENUM_GF_MODE_T, *P_ENUM_GF_MODE_T;

typedef enum _ENUM_RIFS_MODE_T {
	RIFS_MODE_NORMAL = 0,
	RIFS_MODE_DISALLOWED,

	RIFS_MODE_NUM
} ENUM_RIFS_MODE_T, *P_ENUM_RIFS_MODE_T;

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

#endif				/*                   */
