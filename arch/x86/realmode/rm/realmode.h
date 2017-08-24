#ifndef ARCH_X86_REALMODE_RM_REALMODE_H
#define ARCH_X86_REALMODE_RM_REALMODE_H

#ifdef __ASSEMBLY__

/*
                                    
  
                                                          
                                              
 */
#define LJMPW_RM(to)	.byte 0xea ; .word (to), real_mode_seg

#endif /*              */

/*
                                              
 */
#define REALMODE_END_SIGNATURE	0x65a22c82

#endif /*                                 */
