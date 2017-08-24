/*
                                                 
 */
/*
                        
 */
#define OST_OSMR0	(PKUNITY_OST_BASE + 0x0000)
/*
                        
 */
#define OST_OSMR1	(PKUNITY_OST_BASE + 0x0004)
/*
                        
 */
#define OST_OSMR2	(PKUNITY_OST_BASE + 0x0008)
/*
                        
 */
#define OST_OSMR3	(PKUNITY_OST_BASE + 0x000C)
/*
                       
 */
#define OST_OSCR	(PKUNITY_OST_BASE + 0x0010)
/*
                      
 */
#define OST_OSSR	(PKUNITY_OST_BASE + 0x0014)
/*
                               
 */
#define OST_OWER	(PKUNITY_OST_BASE + 0x0018)
/*
                                
 */
#define OST_OIER	(PKUNITY_OST_BASE + 0x001C)

/*
                                                          
                                     
                                    
                               
 */
#define OST_PWM_PWCR	(0x00)
#define OST_PWM_DCCR	(0x04)
#define OST_PWM_PCR 	(0x08)

/*
                               
 */
#define OST_OSSR_M0		FIELD(1, 1, 0)
/*
                               
 */
#define OST_OSSR_M1		FIELD(1, 1, 1)
/*
                               
 */
#define OST_OSSR_M2		FIELD(1, 1, 2)
/*
                               
 */
#define OST_OSSR_M3		FIELD(1, 1, 3)

/*
                                 
 */
#define OST_OIER_E0		FIELD(1, 1, 0)
/*
                                 
 */
#define OST_OIER_E1		FIELD(1, 1, 1)
/*
                                 
 */
#define OST_OIER_E2		FIELD(1, 1, 2)
/*
                                 
 */
#define OST_OIER_E3		FIELD(1, 1, 3)

/*
                                     
 */
#define OST_OWER_WME		FIELD(1, 1, 0)

/*
                                          
 */
#define OST_PWMDCCR_FDCYCLE	FIELD(1, 1, 10)

