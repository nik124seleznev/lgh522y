#ifndef _VGATYPES_
#define _VGATYPES_

#include <linux/fb.h>	/*                                        */
#include "../../video/sis/vgatypes.h"
#include "../../video/sis/sis.h"		/*              */

#ifndef XGI_VB_CHIP_TYPE
enum XGI_VB_CHIP_TYPE {
	VB_CHIP_Legacy = 0,
	VB_CHIP_301,
	VB_CHIP_301B,
	VB_CHIP_301LV,
	VB_CHIP_302,
	VB_CHIP_302B,
	VB_CHIP_302LV,
	VB_CHIP_301C,
	VB_CHIP_302ELV,
	VB_CHIP_UNKNOWN, /*                                       */
	MAX_VB_CHIP
};
#endif

struct xgi_hw_device_info {
	unsigned long ulExternalChip; /*                            */
				      /*                                  */

	void __iomem *pjVideoMemoryAddress;/*                             */
					    /*                      */

	unsigned long ulVideoMemorySize; /*                       
                             */

	unsigned char jChipType; /*                                */
				 /*                                     */
				 /*                 */

	unsigned char jChipRevision; /*                          
                   */

	unsigned char ujVBChipID; /*                        */
				  /*                                    */
				  /*                    */

	unsigned long ulCRT2LCDType; /*                                    */
};

/*                                                             */
/*                                                            */
#endif

