#ifndef _ETB_REGISTER_H_
#define _ETB_REGISTER_H_

#define ETBID		0x0	/*                           */
#define ETBRDP		0x4	/*                      */
#define TMCRSZ      0x4 /*                     */ 
#define ETBRWT		0x8	/*                      */
#define ETBSTS		0xc	/*                   */
#define ETBRRD		0x10	/*                          */
#define ETBRRP		0x14	/*                             */
#define ETBRWP		0x18	/*                              */
#define ETBTRG		0x1c	/*                            */
#define ETBCTL		0x20	/*                    */
#define ETBRWD		0x24	/*                           */
#define TMCDBALR    0x118   /*                                    */     
#define ETBFFSR		0x300	/*                                       */
#define ETBFFCR		0x304	/*                                        */
#define ETBITMISCOP0	0xee0	/*                                 */
/*                                                                */
#define ETBITTRFLINACK	0xee4
#define ETBITTRFLIN	0xee8	/*                                            */
#define ETBITATBDATA0	0xeec	/*                                        */
#define ETBITATBCTR2	0xef0	/*                                           */
#define ETBITATBCTR1	0xef4	/*                                           */
#define ETBITATBCTR0	0xef8	/*                                           */
#define ETBIMC		0xf00	/*                                     */
#define ETBCTS		0xfa0	/*                          */
#define ETBCTC		0xfa4	/*                            */
#define ETBLA		0xfb0	/*                        */
#define ETBLS		0xfb4	/*                        */
#define ETBAS		0xfb8	/*                                  */
#define ETBDID		0xfc8	/*                      */
#define ETBDTI		0xfcc	/*                                   */
#define ETBPID4		0xfd0	/*                           */
#define ETBPID5		0xfd4	/*                           */
#define ETBPID6		0xfd8	/*                           */
#define ETBPID7		0xfdc	/*                           */
#define ETBPID0		0xfe0	/*                           */
#define ETBPID1		0xfe4	/*                           */
#define ETBPID2		0xfe8	/*                           */
#define ETBPID3		0xfec	/*                           */
#define ETBCID0		0xff0	/*                          */
#define ETBCID1		0xff4	/*                          */
#define ETBCID2		0xff8	/*                          */
#define ETBCID3		0xffc	/*                          */

/*                                                         */
#define ETBLA_UNLOCK_MAGIC	0xc5acce55

#define ETBFFCR_ENABLE_FORMAT		(1<<0) /*                     */
/*                               */
#define ETBFFCR_ENABLE_FORMAT_CONT	(1<<1)
/*                                          */
#define ETBFFCR_MANNUL_FLUSH		(1<<6)

#endif

