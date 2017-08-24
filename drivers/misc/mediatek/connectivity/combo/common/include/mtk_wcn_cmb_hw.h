/*       
                                            

                                                                                           
*/



#ifndef _MTK_WCN_CMB_HW_H_
#define _MTK_WCN_CMB_HW_H_


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

typedef struct _PWR_SEQ_TIME_ {
	unsigned int rtcStableTime;
	unsigned int ldoStableTime;
	unsigned int rstStableTime;
	unsigned int offStableTime;
	unsigned int onStableTime;
} PWR_SEQ_TIME, *P_PWR_SEQ_TIME;


/*                                                                              
                                                  
                                                                                
*/



/*                                                                              
                                                   
                                                                                
*/





/*                                                                              
                                                            
                                                                                
*/



/*                                                                              
                                                
                                                                                
*/

extern int mtk_wcn_cmb_hw_pwr_off(void);
extern int mtk_wcn_cmb_hw_pwr_on(void);
extern int mtk_wcn_cmb_hw_rst(void);
extern int mtk_wcn_cmb_hw_init(P_PWR_SEQ_TIME pPwrSeqTime);
extern int mtk_wcn_cmb_hw_deinit(void);
extern int mtk_wcn_cmb_hw_state_show(void);


#endif				/*                    */
