/*                                  */


#ifndef _CH703x_LIB_H_
#define _CH703x_LIB_H_

/*                                                            */
typedef struct {
	uint32 ht;		/*                         */
	uint32 ha;		/*                          */
	uint32 ho;		/*                                */
	uint32 hw;		/*                             */
	uint32 vt;		/*                      */
	uint32 va;		/*                       */
	uint32 vo;		/*                              */
	uint32 vw;		/*                           */
} CH703x_TIMING, *PCH703x_TIMING;

/*                                                           */
/*                        */
#define PIXEL_FMT_RGB888	0	/*                         */
#define PIXEL_FMT_RGB666	1	/*                         */
#define PIXEL_FMT_RGB565	2	/*                         */
#define PIXEL_FMT_RGB555	3	/*                         */
#define PIXEL_FMT_RGBDVO	4	/*         */
#define PIXEL_FMT_YUV422_8	5	/*                   */
#define PIXEL_FMT_YUV422_10	6	/*                    */
#define PIXEL_FMT_YUV444	7	/*                   */
#define PIXEL_FMT_RGB666_C	9	/*                    */
#define PIXEL_FMT_RGB565_C	10	/*                    */
#define PIXEL_FMT_RGB555_C	11	/*                    */

/*                     */
#define POL_HIGH			1
#define POL_LOW				0
#define POL_INVERT			1
#define POL_NO_INV			0

/*                  */
#define AUDIO_I2S			1
#define AUDIO_SPDIF			0

#define SPDIF_MOD_DIFF		0
#define SPDIF_MOD_CMOS		1

/*                  */
#define I2S_FMT_STD			0
#define I2S_FMT_RTJ			1	/*                 */
#define I2S_FMT_LTJ			2	/*                */

#define I2S_LEN_16B			0	/*          */
#define I2S_LEN_20B			1	/*          */
#define I2S_LEN_24B			2	/*          */

typedef enum {
	IF_PRO = 0,		/*                             */
	IF_INT,			/*                           */
	IF_CPU,			/*                                         */
} IF_TYPE;

/*                  */
#define M2X_SEQ_NO_INV		0
#define M2X_SEQ_INVERT		1
#define M2X_ALIGH_LBIT		0
#define M2X_ALIGH_HBIT		1
typedef struct {
	/*               */
	uint8 halfword_swap;	/*                                                 */
	uint8 halfword_align;	/*                                                      */
	uint8 yc_swap;		/*                                              */
	uint8 reserved;
} M2X_FMT, *PM2X_FMT;

/*                  */
#define M3X_ALIGH_LBIT		0
#define M3X_ALIGH_MBIT		1
#define M3X_ALIGH_HBIT		2
typedef struct {
	/*               */
	uint8 byte_align;	/*                                           */
	uint8 reserved[3];
} M3X_FMT, *PM3X_FMT;

#define MULTI_1X		0
#define MULTI_2X		1
#define MULTI_3X		2

/*                                         */
#define BYTE_SWAP_RGB	0
#define BYTE_SWAP_RBG	1
#define BYTE_SWAP_GRB	2
#define BYTE_SWAP_GBR	3
#define BYTE_SWAP_BRG	4
#define BYTE_SWAP_BGR	5

typedef struct {
	uint8 format;		/*                                              */
	uint8 bit_swap;		/*                                                              */
	uint8 byte_swap;	/*                                                   */
	uint8 byte_align;	/*                                                    */
	uint8 multiplexed;	/*                                                              */
	uint8 embedded_sync;	/*                                   */
	uint8 reserved[2];
	M2X_FMT m2x_fmt;	/*                                */
	M3X_FMT m3x_fmt;	/*                                */
} PIXEL_FMT, *PPIXEL_FMT;

typedef struct {
	IF_TYPE if_type;	/*                */
	CH703x_TIMING timing;
	uint32 pclk_khz;	/*                      */
	PIXEL_FMT pixel_fmt;
	uint8 hs_pol;		/*                          */
	uint8 vs_pol;		/*                        */
	uint8 de_pol;		/*                       */
	/*                */
	uint8 audio_type;	/*                   */
	uint8 spdif_mode;	/*                                     */
	uint8 i2s_pol;
	uint8 i2s_len;
	uint8 i2s_fmt;
	/*                   */
	uint8 crystal_used;	/*                                  */
	uint8 reserved[3];
	uint32 crystal_khz;
} CH703x_INPUT_INFO, *PCH703x_INPUT_INFO;

/*                                                            */
/*                     */
#define LVDS_DATA0_SEL			0
#define LVDS_DATA1_SEL			1
#define LVDS_DATA2_SEL			2
#define LVDS_DATA3_SEL			3
#define LVDS_CLOCK_SEL			4

#define LVDS_CHANNEL_SWAP_DEF	0
#define LVDS_CHANNEL_SWAP_OP1	1

#define LVDS_SPWG_T1_MIN		1
#define LVDS_SPWG_T1_MAX		50
#define LVDS_SPWG_T2_MIN		200
#define LVDS_SPWG_T2_MAX		1023
#define LVDS_SPWG_T3_MIN		200
#define LVDS_SPWG_T3_MAX		1023
#define LVDS_SPWG_T4_MIN		1
#define LVDS_SPWG_T4_MAX		50
#define LVDS_SPWG_T5_MIN		200
#define LVDS_SPWG_T5_MAX		1023

/*                       */
#define IDX_PWM_FREQ_100Hz		100
#define IDX_PWM_FREQ_200Hz		200
#define IDX_PWM_FREQ_2KHz		2000
#define IDX_PWM_FREQ_4KHz		4000
#define IDX_PWM_FREQ_16KHz		16000
#define IDX_PWM_FREQ_32KHz		32000
#define IDX_PWM_FREQ_64KHz		64000
#define IDX_PWM_FREQ_128KHz		128000

typedef struct {
	uint8 bypass;		/*                                     */
	uint8 channel_swap;	/*                          */
	uint8 channel_pol;	/*                  */
	uint8 hs_pol;		/*                */
	uint8 vs_pol;		/*                */
	uint8 de_pol;		/*             */
	/*                        */
	uint8 pwm_duty;		/*                            */
	uint8 pwm_inv;		/*                                        */
	uint32 pwm_freq;	/*                                                                                      */
	uint32 spwg_t1;		/*                      */
	uint32 spwg_t2;		/*                      */
	uint32 spwg_t3;		/*                      */
	uint32 spwg_t4;		/*                      */
	uint32 spwg_t5;		/*                      */
} LVDS_FMT, *PLVDS_FMT;

/*                                                    */
#define AS_RATIO_NA			0	/*         */
#define AS_RATIO_4_3		1	/*         */
#define AS_RATIO_16_9		2	/*         */
#define AS_RATIO_16_10		3	/*             */
#define AS_RATIO_5_4		4	/*             */

/*                                                                            */
/*                                                                             */
#define HDMI_CHANNEL_SWAP_DEF	0

typedef struct {
	uint8 bypass;		/*                                 */
	uint8 is_dvi_mode;	/*                               */
	uint8 format_index;	/*           */
	uint8 aspect_ratio;	/*                  */
	uint8 channel_swap;	/*                                    */
	uint8 data_pol_invert;	/*                                                                             */
	uint8 hs_pol;		/*                                      */
	uint8 vs_pol;		/*                                    */
} HDMI_FMT, *PHDMI_FMT;

/*                                                                       */
/*                                                                             */
#define VGA_CHANNEL_SWAP_DEF	0

/*                                                                 */
/*                                                                             */
#define VGA_SYNC_SWAP_DEF		0

#define VGA_CSYNC_XOR			0
#define VGA_CSYNC_OR			1
#define VGA_CSYNC_AND			2

typedef struct {
	uint8 bypass;		/*                                     */
	uint8 channel_swap;	/*                                                   */
	uint8 sync_swap;	/*                                                  */
	uint8 csync_type;	/*            */
	uint8 hs_pol;		/*                          */
	uint8 vs_pol;		/*                      */
	uint8 de_pol;		/*                             */
	uint8 reserved;
} VGA_FMT, *PVGA_FMT;

/*                                                                         */
/*                                                                             */
#define HDTV_CHANNEL_SWAP_DEF	0

typedef struct {
	uint8 bypass;		/*                                     */
	uint8 format_index;	/*                    */
	uint8 channel_swap;	/*                                                     */
	uint8 reserved;
} HDTV_FMT, *PHDTV_FMT;

/*                              */
#define CHANNEL_LVDS		0x01
#define CHANNEL_HDMI		0x02
#define CHANNEL_VGA			0x04
#define CHANNEL_HDTV		0x08
#define CHANNEL_VGA1		0x10
/*                             */
#define CHANNEL_INPUTLOST	0x20

#define ROTATE_NO			0
#define ROTATE_90			1
#define ROTATE_180			2
#define ROTATE_270			3

#define DOWN_SCALING_MAX	80	/*                                                                       */

typedef struct {
	CH703x_TIMING timing;	/*        */
	uint32 uclk_khz;	/*                        */
	uint8 channel;		/*                       */
	uint8 reserved1[3];
	LVDS_FMT lvds_fmt;	/*                                       */
	HDMI_FMT hdmi_fmt;	/*                                       */
	VGA_FMT vga_fmt;	/*                                      */
	HDTV_FMT hdtv_fmt;	/*                                        */

	/*          */
	uint8 ds_percent_h;	/*                                               */
	uint8 ds_percent_v;	/*                                             */
	uint8 rotate;		/*                  */
	uint8 h_flip;		/*                         */
	uint8 v_flip;		/*                       */
	uint8 reserved2[3];
} CH703x_OUTPUT_INFO, *PCH703x_OUTPUT_INFO;

/*                                                                              */
#define  GET_VGA_EDID		0x80
#define  GET_HDMI_EDID		0x00
#define  GET_EDID_MASK		0x80

/*                                                            */
typedef enum {
	DEV_CH7033E = 0,
	DEV_CH7034E,
	DEV_CH7035E,

	DEV_CH7033F,
	DEV_CH7034F,
	DEV_CH7035F,

	DEV_UNKNOW,		/*                    */
} CH703x_DEV_TYPE;

typedef struct _FW703X_CFG {
	unsigned char size;	/*                                          */
	unsigned char ver_major;	/*                      */
	unsigned char ver_minor;
	unsigned char did;	/*           */
	unsigned char rid;	/*             */
	unsigned char capbility;	/*                      */
	unsigned char reserved;
	unsigned char reserve2;
} FW703X_CFG;

/*                                                               */

#ifdef __cplusplus
extern "C" {
#endif

/*                                                               */

/*        */
/*                                                                    */
/*                                           */
/*         */
/*                                            */
/*                                         */
	CH703x_DEV_TYPE ch_check_chip(CH703x_DEV_TYPE chip);

/*                                                   */
	int32 ch_start_firmware(CH703x_DEV_TYPE chip);

/*                     */
	int32 ch_getMcuVersion(CH703x_DEV_TYPE chip, FW703X_CFG * cfg);

/*                             */
	int32 ch_load_firmware(CH703x_DEV_TYPE chip, uint8 *fw, uint16 size);

/*                         */
/*                                                                        */
	int32 ch_set_mode(CH703x_DEV_TYPE chip, CH703x_INPUT_INFO * in, CH703x_OUTPUT_INFO * out);

/*                                                                             */
/*                                                                           */
/*            */
	int32 ch_set_test_mode(CH703x_DEV_TYPE chip);

/*                                         */
/*        */
/*                                                                  */
/*                     */
/*                                          */
	int32 ch_get_edid(CH703x_DEV_TYPE chip, uint8 *buf, uint16 len, uint8 whichEDID);

#define VGA_HOTPLUG_DELAY		50	/*         */
/*                                                                           */
/*                                            */
/*                                                                    */
/*                                                                            */
/*                                                                            */
/*                                                   */
/*         */
/*                                                          */
/*                     */
	uint8 ch_check_connection(CH703x_DEV_TYPE chip, int delay);
	int32 ch_set_plug_out_power(CH703x_DEV_TYPE chip, uint8 enable);
/*       */
/*                                            */
/*                                  */
/*                                  */
	int32 ch_set_channel_power(CH703x_DEV_TYPE chip, uint8 channel_bits);

/*        */
/*                                            */
/*                                                         */
/*                                                                */
/*                                                    */
	int32 ch_set_chip_power(CH703x_DEV_TYPE chip, uint8 channel_bits);

/*                                 */
	int32 ch_set_dither(CH703x_DEV_TYPE chip, uint8 dither);

/*        */
/*                                                     */
/*                      */
	uint8 ch_set_lvds_brightness(CH703x_DEV_TYPE chip, uint8 pwm_duty);

#define MCUAUTO_SETMODE		0x40
/*                                                                           */
/*                                                                        */
/*                                                                        */
/*                                                              */
/*        */
/*                                                            */
/*                          */
	uint8 ch_enable_MCU_AutoFunc(CH703x_DEV_TYPE chip, uint8 mcuAutoBits);

/*                                                         */

/*        */
/*                                             */
/*                                */
/*                                                          */
/*                                                 */
/*       */
/*                                                   */
/*                                                */
/*                      */
/*                                                                 */

/*        */
/*                         */
/*                      */
	uint8 ch_set_lvds_common_voltage(CH703x_DEV_TYPE chip, uint8 bOn);

/*         */
	void ch_dump_reg(void);
	void ch_read_reg(unsigned char u8Reg);
	void ch_write_reg(unsigned char u8Reg, unsigned char u8Data);
	void hw_set_HPD_MAX(uint8 addr);
	void hw_set_inv_clk(uint8 inv);
	void hw_set_suspend(void);
	void hw_set_resume(void);

#ifdef __cplusplus
}
#endif
#endif
