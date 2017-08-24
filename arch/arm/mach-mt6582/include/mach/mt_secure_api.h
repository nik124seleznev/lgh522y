/*
 */

#ifndef _MT_SECURE_API_H_
#define _MT_SECURE_API_H_

/*                                                                       */
#if defined(__GNUC__) && \
	defined(__GNUC_MINOR__) && \
	defined(__GNUC_PATCHLEVEL__) && \
	((__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)) \
	>= 40502
#define MC_ARCH_EXTENSION_SEC
#endif


/*
                            
 */
#define MC_FC_RET_OK					0
#define MC_FC_RET_ERR_INVALID			1



/*                  */
/* 
                                                                    
              
                                         
                                       
                      
          
                                        
 */
#define MC_FC_SLEEP                     -3
/* 
                                                 
              
                                        
                         
                      
          
                                        
 */
#define MC_FC_SET_RESET_VECTOR		-301
/* 
                                 
              
                      
                      
                      
          
                                        
 */
#define MC_FC_TURN_OFF_BOOTROM		-302
/* 
                                                              
              
                      
                      
                      
          
                                        
 */
#define MC_FC_SLEEP_CANCELLED		-303


#define MC_FC_MTK_AEEDUMP		-306



/*
                 
              
                                           
                                                                    
          
                                                                   
 */
static inline int mt_secure_call(uint32_t cmd, uint32_t param0, uint32_t param1, uint32_t param2)
{
	/*                            */
	register u32 reg0 __asm__("r0") = cmd;
	register u32 reg1 __asm__("r1") = param0;
	register u32 reg2 __asm__("r2") = param1;
	register u32 reg3 __asm__("r3") = param2;
	int ret = 0;

	__asm__ volatile (
#ifdef MC_ARCH_EXTENSION_SEC
		/*                                              
                      */
		".arch_extension sec\n"
#endif
		"smc 0\n"
		: "+r"(reg0), "+r"(reg1), "+r"(reg2), "+r"(reg3)
	);

	/*              */
	ret = reg0;
	return ret;
}


#endif /*                   */
