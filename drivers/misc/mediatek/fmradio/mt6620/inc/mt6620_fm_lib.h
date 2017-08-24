#ifndef __MT6620_FM_LIB_H__
#define __MT6620_FM_LIB_H__

#include "fm_typedef.h"

#define MT6620_VOL_MAX   0x2B	/*                 */
enum {
	DSPPATCH = 0xFFF9,
	USDELAY = 0xFFFA,
	MSDELAY = 0xFFFB,
	HW_VER = 0xFFFD,
	POLL_N = 0xFFFE,	/*                               */
	POLL_P = 0xFFFF,	/*                                */
};

enum {
	FM_PUS_DSPPATCH = DSPPATCH,
	FM_PUS_USDELAY = USDELAY,
	FM_PUS_MSDELAY = MSDELAY,
	FM_PUS_HW_VER = HW_VER,
	FM_PUS_POLL_N = POLL_N,	/*                               */
	FM_PUS_POLL_P = POLL_P,	/*                                */
	FM_PUS_MAX
};

enum {
	DSP_PATH = 0x02,
	DSP_COEFF = 0x03,
	DSP_HW_COEFF = 0x04
};

enum IMG_TYPE {
	IMG_WRONG = 0,
	IMG_ROM,
	IMG_PATCH,
	IMG_COEFFICIENT,
	IMG_HW_COEFFICIENT
};

enum {
	mt6620_E1 = 0,
	mt6620_E2
};

struct mt6620_fm_softmute_tune_cqi_t {
	fm_u16 ch;		/*                   */
	fm_u16 rssi;		/*                                    */
	fm_u16 pamd;		/*                                    */
	fm_u16 mr;		/*                                  */
	fm_u16 atdc;		/*                                    */
	fm_u16 prx;		/*                                   */
	fm_u16 smg;		/*                                              */
};

#define BITn(n) (uint16_t)(1<<(n))
#define MASK(n) (uint16_t)(~(1<<(n)))
/*                           */
#define FM_TX_PWR_CTRL_FREQ_THR 890
#define FM_TX_PWR_CTRL_TMP_THR_UP 45
#define FM_TX_PWR_CTRL_TMP_THR_DOWN 0

#define FM_TX_TRACKING_TIME_MAX 10000	/*                                     */

/*                     */
/*                                */
/*
                                     
                                                     
                                                     
                                                     
                                                     
                                                     
                                                     
                                                     
                                                     
                                                     
                                                     
                                                     
                                                     
                                                     
                                                     
                                                     
                                                     
*/
enum group_idx {
	mono = 0,
	stereo,
	RSSI_threshold,
	HCC_Enable,
	PAMD_threshold,
	Softmute_Enable,
	De_emphasis,
	HL_Side,
	Demod_BW,
	Dynamic_Limiter,
	Softmute_Rate,
	AFC_Enable,
	Softmute_Level,
	Analog_Volume,
	GROUP_TOTAL_NUMS
};

enum item_idx {
	Sblend_OFF = 0,
	Sblend_ON,
	ITEM_TOTAL_NUMS
};

#endif
