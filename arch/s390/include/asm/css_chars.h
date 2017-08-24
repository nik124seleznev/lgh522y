#ifndef _ASM_CSS_CHARS_H
#define _ASM_CSS_CHARS_H

#include <linux/types.h>

struct css_general_char {
	u64 : 12;
	u32 dynio : 1;	 /*        */
	u32 : 4;
	u32 eadm : 1;	 /*        */
	u32 : 23;
	u32 aif : 1;	 /*        */
	u32 : 3;
	u32 mcss : 1;	 /*        */
	u32 fcs : 1;	 /*        */
	u32 : 1;
	u32 ext_mb : 1;  /*        */
	u32 : 7;
	u32 aif_tdd : 1; /*        */
	u32 : 1;
	u32 qebsm : 1;	 /*        */
	u32 : 8;
	u32 aif_osa : 1; /*        */
	u32 : 12;
	u32 eadm_rf : 1; /*        */
	u32 : 1;
	u32 cib : 1;	 /*        */
	u32 : 5;
	u32 fcx : 1;	 /*        */
	u32 : 19;
	u32 alt_ssi : 1; /*         */
} __packed;

extern struct css_general_char css_general_characteristics;

#endif
