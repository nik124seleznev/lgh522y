#ifndef _EEPROM_DATA_H
#define _EEPROM_DATA_H

/*                                         */
/*                            */

typedef struct {
	u32 u4Offset;
	u32 u4Length;
	u8 *pu1Params;
} stEEPROM_INFO_STRUCT, *stPEEPROM_INFO_STRUCT;
#endif				/*                */
