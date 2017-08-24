#ifndef _EEPROM_H
#define _EEPROM_H

#include <linux/ioctl.h>


#define EEPROMAGIC 'i'
/*                                    */
/*                             */
/*                               */
/*                        */
/*                                 */
/*                                     */
/*                                     */

/*                                                                              
 
                                                                               */

/*              */
#define EEPROMIOC_S_WRITE            _IOW(EEPROMAGIC, 0, stEEPROM_INFO_STRUCT)
/*             */
#define EEPROMIOC_G_READ            _IOWR(EEPROMAGIC, 5, stPEEPROM_INFO_STRUCT)

#endif				/*           */
