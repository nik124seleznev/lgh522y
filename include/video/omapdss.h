/*
 * Copyright (C) 2008 Nokia Corporation
 * Author: Tomi Valkeinen <tomi.valkeinen@nokia.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __OMAP_OMAPDSS_H
#define __OMAP_OMAPDSS_H

#include <linux/list.h>
#include <linux/kobject.h>
#include <linux/device.h>
#include <linux/interrupt.h>

#define DISPC_IRQ_FRAMEDONE		(1 << 0)
#define DISPC_IRQ_VSYNC			(1 << 1)
#define DISPC_IRQ_EVSYNC_EVEN		(1 << 2)
#define DISPC_IRQ_EVSYNC_ODD		(1 << 3)
#define DISPC_IRQ_ACBIAS_COUNT_STAT	(1 << 4)
#define DISPC_IRQ_PROG_LINE_NUM		(1 << 5)
#define DISPC_IRQ_GFX_FIFO_UNDERFLOW	(1 << 6)
#define DISPC_IRQ_GFX_END_WIN		(1 << 7)
#define DISPC_IRQ_PAL_GAMMA_MASK	(1 << 8)
#define DISPC_IRQ_OCP_ERR		(1 << 9)
#define DISPC_IRQ_VID1_FIFO_UNDERFLOW	(1 << 10)
#define DISPC_IRQ_VID1_END_WIN		(1 << 11)
#define DISPC_IRQ_VID2_FIFO_UNDERFLOW	(1 << 12)
#define DISPC_IRQ_VID2_END_WIN		(1 << 13)
#define DISPC_IRQ_SYNC_LOST		(1 << 14)
#define DISPC_IRQ_SYNC_LOST_DIGIT	(1 << 15)
#define DISPC_IRQ_WAKEUP		(1 << 16)
#define DISPC_IRQ_SYNC_LOST2		(1 << 17)
#define DISPC_IRQ_VSYNC2		(1 << 18)
#define DISPC_IRQ_VID3_END_WIN		(1 << 19)
#define DISPC_IRQ_VID3_FIFO_UNDERFLOW	(1 << 20)
#define DISPC_IRQ_ACBIAS_COUNT_STAT2	(1 << 21)
#define DISPC_IRQ_FRAMEDONE2		(1 << 22)
#define DISPC_IRQ_FRAMEDONEWB		(1 << 23)
#define DISPC_IRQ_FRAMEDONETV		(1 << 24)
#define DISPC_IRQ_WBBUFFEROVERFLOW	(1 << 25)
#define DISPC_IRQ_SYNC_LOST3		(1 << 27)
#define DISPC_IRQ_VSYNC3		(1 << 28)
#define DISPC_IRQ_ACBIAS_COUNT_STAT3	(1 << 29)
#define DISPC_IRQ_FRAMEDONE3		(1 << 30)

struct omap_dss_device;
struct omap_overlay_manager;
struct dss_lcd_mgr_config;
struct snd_aes_iec958;
struct snd_cea_861_aud_if;

enum omap_display_type {
	OMAP_DISPLAY_TYPE_NONE		= 0,
	OMAP_DISPLAY_TYPE_DPI		= 1 << 0,
	OMAP_DISPLAY_TYPE_DBI		= 1 << 1,
	OMAP_DISPLAY_TYPE_SDI		= 1 << 2,
	OMAP_DISPLAY_TYPE_DSI		= 1 << 3,
	OMAP_DISPLAY_TYPE_VENC		= 1 << 4,
	OMAP_DISPLAY_TYPE_HDMI		= 1 << 5,
};

enum omap_plane {
	OMAP_DSS_GFX	= 0,
	OMAP_DSS_VIDEO1	= 1,
	OMAP_DSS_VIDEO2	= 2,
	OMAP_DSS_VIDEO3	= 3,
	OMAP_DSS_WB	= 4,
};

enum omap_channel {
	OMAP_DSS_CHANNEL_LCD	= 0,
	OMAP_DSS_CHANNEL_DIGIT	= 1,
	OMAP_DSS_CHANNEL_LCD2	= 2,
	OMAP_DSS_CHANNEL_LCD3	= 3,
};

enum omap_color_mode {
	OMAP_DSS_COLOR_CLUT1	= 1 << 0,  /*          */
	OMAP_DSS_COLOR_CLUT2	= 1 << 1,  /*          */
	OMAP_DSS_COLOR_CLUT4	= 1 << 2,  /*          */
	OMAP_DSS_COLOR_CLUT8	= 1 << 3,  /*          */
	OMAP_DSS_COLOR_RGB12U	= 1 << 4,  /*                         */
	OMAP_DSS_COLOR_ARGB16	= 1 << 5,  /*        */
	OMAP_DSS_COLOR_RGB16	= 1 << 6,  /*       */
	OMAP_DSS_COLOR_RGB24U	= 1 << 7,  /*                         */
	OMAP_DSS_COLOR_RGB24P	= 1 << 8,  /*                         */
	OMAP_DSS_COLOR_YUV2	= 1 << 9,  /*                     */
	OMAP_DSS_COLOR_UYVY	= 1 << 10, /*                     */
	OMAP_DSS_COLOR_ARGB32	= 1 << 11, /*        */
	OMAP_DSS_COLOR_RGBA32	= 1 << 12, /*        */
	OMAP_DSS_COLOR_RGBX32	= 1 << 13, /*        */
	OMAP_DSS_COLOR_NV12		= 1 << 14, /*                        */
	OMAP_DSS_COLOR_RGBA16		= 1 << 15, /*               */
	OMAP_DSS_COLOR_RGBX16		= 1 << 16, /*               */
	OMAP_DSS_COLOR_ARGB16_1555	= 1 << 17, /*               */
	OMAP_DSS_COLOR_XRGB16_1555	= 1 << 18, /*               */
};

enum omap_dss_load_mode {
	OMAP_DSS_LOAD_CLUT_AND_FRAME	= 0,
	OMAP_DSS_LOAD_CLUT_ONLY		= 1,
	OMAP_DSS_LOAD_FRAME_ONLY	= 2,
	OMAP_DSS_LOAD_CLUT_ONCE_FRAME	= 3,
};

enum omap_dss_trans_key_type {
	OMAP_DSS_COLOR_KEY_GFX_DST = 0,
	OMAP_DSS_COLOR_KEY_VID_SRC = 1,
};

enum omap_rfbi_te_mode {
	OMAP_DSS_RFBI_TE_MODE_1 = 1,
	OMAP_DSS_RFBI_TE_MODE_2 = 2,
};

enum omap_dss_signal_level {
	OMAPDSS_SIG_ACTIVE_HIGH	= 0,
	OMAPDSS_SIG_ACTIVE_LOW	= 1,
};

enum omap_dss_signal_edge {
	OMAPDSS_DRIVE_SIG_OPPOSITE_EDGES,
	OMAPDSS_DRIVE_SIG_RISING_EDGE,
	OMAPDSS_DRIVE_SIG_FALLING_EDGE,
};

enum omap_dss_venc_type {
	OMAP_DSS_VENC_TYPE_COMPOSITE,
	OMAP_DSS_VENC_TYPE_SVIDEO,
};

enum omap_dss_dsi_pixel_format {
	OMAP_DSS_DSI_FMT_RGB888,
	OMAP_DSS_DSI_FMT_RGB666,
	OMAP_DSS_DSI_FMT_RGB666_PACKED,
	OMAP_DSS_DSI_FMT_RGB565,
};

enum omap_dss_dsi_mode {
	OMAP_DSS_DSI_CMD_MODE = 0,
	OMAP_DSS_DSI_VIDEO_MODE,
};

enum omap_display_caps {
	OMAP_DSS_DISPLAY_CAP_MANUAL_UPDATE	= 1 << 0,
	OMAP_DSS_DISPLAY_CAP_TEAR_ELIM		= 1 << 1,
};

enum omap_dss_display_state {
	OMAP_DSS_DISPLAY_DISABLED = 0,
	OMAP_DSS_DISPLAY_ACTIVE,
};

enum omap_dss_audio_state {
	OMAP_DSS_AUDIO_DISABLED = 0,
	OMAP_DSS_AUDIO_ENABLED,
	OMAP_DSS_AUDIO_CONFIGURED,
	OMAP_DSS_AUDIO_PLAYING,
};

enum omap_dss_rotation_type {
	OMAP_DSS_ROT_DMA	= 1 << 0,
	OMAP_DSS_ROT_VRFB	= 1 << 1,
	OMAP_DSS_ROT_TILER	= 1 << 2,
};

/*                          */
enum omap_dss_rotation_angle {
	OMAP_DSS_ROT_0   = 0,
	OMAP_DSS_ROT_90  = 1,
	OMAP_DSS_ROT_180 = 2,
	OMAP_DSS_ROT_270 = 3,
};

enum omap_overlay_caps {
	OMAP_DSS_OVL_CAP_SCALE = 1 << 0,
	OMAP_DSS_OVL_CAP_GLOBAL_ALPHA = 1 << 1,
	OMAP_DSS_OVL_CAP_PRE_MULT_ALPHA = 1 << 2,
	OMAP_DSS_OVL_CAP_ZORDER = 1 << 3,
	OMAP_DSS_OVL_CAP_POS = 1 << 4,
	OMAP_DSS_OVL_CAP_REPLICATION = 1 << 5,
};

enum omap_overlay_manager_caps {
	OMAP_DSS_DUMMY_VALUE, /*                                             */
};

enum omap_dss_clk_source {
	OMAP_DSS_CLK_SRC_FCK = 0,		/*                         
                         */
	OMAP_DSS_CLK_SRC_DSI_PLL_HSDIV_DISPC,	/*                     
                          */
	OMAP_DSS_CLK_SRC_DSI_PLL_HSDIV_DSI,	/*                     
                          */
	OMAP_DSS_CLK_SRC_DSI2_PLL_HSDIV_DISPC,	/*                  */
	OMAP_DSS_CLK_SRC_DSI2_PLL_HSDIV_DSI,	/*                  */
};

enum omap_hdmi_flags {
	OMAP_HDMI_SDA_SCL_EXTERNAL_PULLUP = 1 << 0,
};

enum omap_dss_output_id {
	OMAP_DSS_OUTPUT_DPI	= 1 << 0,
	OMAP_DSS_OUTPUT_DBI	= 1 << 1,
	OMAP_DSS_OUTPUT_SDI	= 1 << 2,
	OMAP_DSS_OUTPUT_DSI1	= 1 << 3,
	OMAP_DSS_OUTPUT_DSI2	= 1 << 4,
	OMAP_DSS_OUTPUT_VENC	= 1 << 5,
	OMAP_DSS_OUTPUT_HDMI	= 1 << 6,
};

/*      */

struct rfbi_timings {
	int cs_on_time;
	int cs_off_time;
	int we_on_time;
	int we_off_time;
	int re_on_time;
	int re_off_time;
	int we_cycle_time;
	int re_cycle_time;
	int cs_pulse_width;
	int access_time;

	int clk_div;

	u32 tim[5];             /*                               */

	int converted;
};

void omap_rfbi_write_command(const void *buf, u32 len);
void omap_rfbi_read_data(void *buf, u32 len);
void omap_rfbi_write_data(const void *buf, u32 len);
void omap_rfbi_write_pixels(const void __iomem *buf, int scr_width,
		u16 x, u16 y,
		u16 w, u16 h);
int omap_rfbi_enable_te(bool enable, unsigned line);
int omap_rfbi_setup_te(enum omap_rfbi_te_mode mode,
			     unsigned hs_pulse_time, unsigned vs_pulse_time,
			     int hs_pol_inv, int vs_pol_inv, int extif_div);
void rfbi_bus_lock(void);
void rfbi_bus_unlock(void);

/*     */

enum omap_dss_dsi_trans_mode {
	/*                                                   */
	OMAP_DSS_DSI_PULSE_MODE,
	/*                                           */
	OMAP_DSS_DSI_EVENT_MODE,
	/*                                                                 */
	OMAP_DSS_DSI_BURST_MODE,
};

struct omap_dss_dsi_videomode_timings {
	unsigned long hsclk;

	unsigned ndl;
	unsigned bitspp;

	/*        */
	u16 hact;
	/*       */
	u16 vact;

	/*                              */
	/*                         */
	u16 hss;
	u16 hsa;
	u16 hse;
	u16 hfp;
	u16 hbp;
	/*                   */
	u16 vsa;
	u16 vfp;
	u16 vbp;

	/*                    */
	int blanking_mode;
	int hsa_blanking_mode;
	int hbp_blanking_mode;
	int hfp_blanking_mode;

	enum omap_dss_dsi_trans_mode trans_mode;

	bool ddr_clk_always_on;
	int window_sync;
};

struct omap_dss_dsi_config {
	enum omap_dss_dsi_mode mode;
	enum omap_dss_dsi_pixel_format pixel_format;
	const struct omap_video_timings *timings;

	unsigned long hs_clk_min, hs_clk_max;
	unsigned long lp_clk_min, lp_clk_max;

	bool ddr_clk_always_on;
	enum omap_dss_dsi_trans_mode trans_mode;
};

void dsi_bus_lock(struct omap_dss_device *dssdev);
void dsi_bus_unlock(struct omap_dss_device *dssdev);
int dsi_vc_dcs_write(struct omap_dss_device *dssdev, int channel, u8 *data,
		int len);
int dsi_vc_generic_write(struct omap_dss_device *dssdev, int channel, u8 *data,
		int len);
int dsi_vc_dcs_write_0(struct omap_dss_device *dssdev, int channel, u8 dcs_cmd);
int dsi_vc_generic_write_0(struct omap_dss_device *dssdev, int channel);
int dsi_vc_dcs_write_1(struct omap_dss_device *dssdev, int channel, u8 dcs_cmd,
		u8 param);
int dsi_vc_generic_write_1(struct omap_dss_device *dssdev, int channel,
		u8 param);
int dsi_vc_generic_write_2(struct omap_dss_device *dssdev, int channel,
		u8 param1, u8 param2);
int dsi_vc_dcs_write_nosync(struct omap_dss_device *dssdev, int channel,
		u8 *data, int len);
int dsi_vc_generic_write_nosync(struct omap_dss_device *dssdev, int channel,
		u8 *data, int len);
int dsi_vc_dcs_read(struct omap_dss_device *dssdev, int channel, u8 dcs_cmd,
		u8 *buf, int buflen);
int dsi_vc_generic_read_0(struct omap_dss_device *dssdev, int channel, u8 *buf,
		int buflen);
int dsi_vc_generic_read_1(struct omap_dss_device *dssdev, int channel, u8 param,
		u8 *buf, int buflen);
int dsi_vc_generic_read_2(struct omap_dss_device *dssdev, int channel,
		u8 param1, u8 param2, u8 *buf, int buflen);
int dsi_vc_set_max_rx_packet_size(struct omap_dss_device *dssdev, int channel,
		u16 len);
int dsi_vc_send_null(struct omap_dss_device *dssdev, int channel);
int dsi_vc_send_bta_sync(struct omap_dss_device *dssdev, int channel);
int dsi_enable_video_output(struct omap_dss_device *dssdev, int channel);
void dsi_disable_video_output(struct omap_dss_device *dssdev, int channel);

enum omapdss_version {
	OMAPDSS_VER_UNKNOWN = 0,
	OMAPDSS_VER_OMAP24xx,
	OMAPDSS_VER_OMAP34xx_ES1,	/*                     */
	OMAPDSS_VER_OMAP34xx_ES3,	/*                 */
	OMAPDSS_VER_OMAP3630,
	OMAPDSS_VER_AM35xx,
	OMAPDSS_VER_OMAP4430_ES1,	/*                */
	OMAPDSS_VER_OMAP4430_ES2,	/*                          */
	OMAPDSS_VER_OMAP4,		/*                  */
	OMAPDSS_VER_OMAP5,
};

/*                     */
struct omap_dss_board_info {
	int (*get_context_loss_count)(struct device *dev);
	int num_devices;
	struct omap_dss_device **devices;
	struct omap_dss_device *default_device;
	int (*dsi_enable_pads)(int dsi_id, unsigned lane_mask);
	void (*dsi_disable_pads)(int dsi_id, unsigned lane_mask);
	int (*set_min_bus_tput)(struct device *dev, unsigned long r);
	enum omapdss_version version;
};

/*                          */
extern int omap_display_init(struct omap_dss_board_info *board_data);
/*              */
extern int omap_hdmi_init(enum omap_hdmi_flags flags);

struct omap_video_timings {
	/*              */
	u16 x_res;
	/*              */
	u16 y_res;
	/*           */
	u32 pixel_clock;
	/*                    */
	u16 hsw;	/*                                        */
	/*                    */
	u16 hfp;	/*                        */
	/*                    */
	u16 hbp;	/*                       */
	/*                   */
	u16 vsw;	/*                                      */
	/*                   */
	u16 vfp;	/*                      */
	/*                   */
	u16 vbp;	/*                     */

	/*                   */
	enum omap_dss_signal_level vsync_level;
	/*                   */
	enum omap_dss_signal_level hsync_level;
	/*                                   */
	bool interlace;
	/*                                    */
	enum omap_dss_signal_edge data_pclk_edge;
	/*                         */
	enum omap_dss_signal_level de_level;
	/*                                                    */
	enum omap_dss_signal_edge sync_pclk_edge;
};

#ifdef CONFIG_OMAP2_DSS_VENC
/*                                                        
                                                           
                                                        
                                  */
extern const struct omap_video_timings omap_dss_pal_timings;
extern const struct omap_video_timings omap_dss_ntsc_timings;
#endif

struct omap_dss_cpr_coefs {
	s16 rr, rg, rb;
	s16 gr, gg, gb;
	s16 br, bg, bb;
};

struct omap_overlay_info {
	u32 paddr;
	u32 p_uv_addr;  /*                 */
	u16 screen_width;
	u16 width;
	u16 height;
	enum omap_color_mode color_mode;
	u8 rotation;
	enum omap_dss_rotation_type rotation_type;
	bool mirror;

	u16 pos_x;
	u16 pos_y;
	u16 out_width;	/*                          */
	u16 out_height;	/*                            */
	u8 global_alpha;
	u8 pre_mult_alpha;
	u8 zorder;
};

struct omap_overlay {
	struct kobject kobj;
	struct list_head list;

	/*               */
	const char *name;
	enum omap_plane id;
	enum omap_color_mode supported_modes;
	enum omap_overlay_caps caps;

	/*                */
	struct omap_overlay_manager *manager;

	/*
                                         
   
              
                    
                    
   
                                                                 
                     
  */

	int (*enable)(struct omap_overlay *ovl);
	int (*disable)(struct omap_overlay *ovl);
	bool (*is_enabled)(struct omap_overlay *ovl);

	int (*set_manager)(struct omap_overlay *ovl,
		struct omap_overlay_manager *mgr);
	int (*unset_manager)(struct omap_overlay *ovl);

	int (*set_overlay_info)(struct omap_overlay *ovl,
			struct omap_overlay_info *info);
	void (*get_overlay_info)(struct omap_overlay *ovl,
			struct omap_overlay_info *info);

	int (*wait_for_go)(struct omap_overlay *ovl);

	struct omap_dss_device *(*get_device)(struct omap_overlay *ovl);
};

struct omap_overlay_manager_info {
	u32 default_color;

	enum omap_dss_trans_key_type trans_key_type;
	u32 trans_key;
	bool trans_enabled;

	bool partial_alpha_enabled;

	bool cpr_enable;
	struct omap_dss_cpr_coefs cpr_coefs;
};

struct omap_overlay_manager {
	struct kobject kobj;

	/*               */
	const char *name;
	enum omap_channel id;
	enum omap_overlay_manager_caps caps;
	struct list_head overlays;
	enum omap_display_type supported_displays;
	enum omap_dss_output_id supported_outputs;

	/*                */
	struct omap_dss_output *output;

	/*
                                         
   
                    
                    
         
   
                                                                 
                     
  */

	int (*set_output)(struct omap_overlay_manager *mgr,
		struct omap_dss_output *output);
	int (*unset_output)(struct omap_overlay_manager *mgr);

	int (*set_manager_info)(struct omap_overlay_manager *mgr,
			struct omap_overlay_manager_info *info);
	void (*get_manager_info)(struct omap_overlay_manager *mgr,
			struct omap_overlay_manager_info *info);

	int (*apply)(struct omap_overlay_manager *mgr);
	int (*wait_for_go)(struct omap_overlay_manager *mgr);
	int (*wait_for_vsync)(struct omap_overlay_manager *mgr);

	struct omap_dss_device *(*get_device)(struct omap_overlay_manager *mgr);
};

/*                                            */
#define OMAP_DSS_MAX_DSI_PINS 22

struct omap_dsi_pin_config {
	int num_pins;
	/*
                                       
              
                  
                  
       
  */
	int pins[OMAP_DSS_MAX_DSI_PINS];
};

struct omap_dss_writeback_info {
	u32 paddr;
	u32 p_uv_addr;
	u16 buf_width;
	u16 width;
	u16 height;
	enum omap_color_mode color_mode;
	u8 rotation;
	enum omap_dss_rotation_type rotation_type;
	bool mirror;
	u8 pre_mult_alpha;
};

struct omap_dss_output {
	struct list_head list;

	const char *name;

	/*                                      */
	enum omap_display_type type;

	/*                               */
	enum omap_channel dispc_channel;

	/*                 */
	enum omap_dss_output_id id;

	/*                                  */
	struct platform_device *pdev;

	/*                */
	struct omap_overlay_manager *manager;

	struct omap_dss_device *device;
};

struct omap_dss_device {
	struct device dev;

	enum omap_display_type type;

	/*                         */
	enum omap_channel channel;

	union {
		struct {
			u8 data_lines;
		} dpi;

		struct {
			u8 channel;
			u8 data_lines;
		} rfbi;

		struct {
			u8 datapairs;
		} sdi;

		struct {
			int module;

			bool ext_te;
			u8 ext_te_gpio;
		} dsi;

		struct {
			enum omap_dss_venc_type type;
			bool invert_polarity;
		} venc;
	} phy;

	struct {
		struct omap_video_timings timings;

		enum omap_dss_dsi_pixel_format dsi_pix_fmt;
		enum omap_dss_dsi_mode dsi_mode;
	} panel;

	struct {
		u8 pixel_size;
		struct rfbi_timings rfbi_timings;
	} ctrl;

	int reset_gpio;

	int max_backlight_level;

	const char *name;

	/*                                */
	const char *driver_name;

	void *data;

	struct omap_dss_driver *driver;

	/*                                           */
	bool activate_after_resume;

	enum omap_display_caps caps;

	struct omap_dss_output *output;

	enum omap_dss_display_state state;

	enum omap_dss_audio_state audio_state;

	/*                    */
	int (*platform_enable)(struct omap_dss_device *dssdev);
	void (*platform_disable)(struct omap_dss_device *dssdev);
	int (*set_backlight)(struct omap_dss_device *dssdev, int level);
	int (*get_backlight)(struct omap_dss_device *dssdev);
};

struct omap_dss_hdmi_data
{
	int ct_cp_hpd_gpio;
	int ls_oe_gpio;
	int hpd_gpio;
};

struct omap_dss_audio {
	struct snd_aes_iec958 *iec;
	struct snd_cea_861_aud_if *cea;
};

struct omap_dss_driver {
	struct device_driver driver;

	int (*probe)(struct omap_dss_device *);
	void (*remove)(struct omap_dss_device *);

	int (*enable)(struct omap_dss_device *display);
	void (*disable)(struct omap_dss_device *display);
	int (*run_test)(struct omap_dss_device *display, int test);

	int (*update)(struct omap_dss_device *dssdev,
			       u16 x, u16 y, u16 w, u16 h);
	int (*sync)(struct omap_dss_device *dssdev);

	int (*enable_te)(struct omap_dss_device *dssdev, bool enable);
	int (*get_te)(struct omap_dss_device *dssdev);

	u8 (*get_rotate)(struct omap_dss_device *dssdev);
	int (*set_rotate)(struct omap_dss_device *dssdev, u8 rotate);

	bool (*get_mirror)(struct omap_dss_device *dssdev);
	int (*set_mirror)(struct omap_dss_device *dssdev, bool enable);

	int (*memory_read)(struct omap_dss_device *dssdev,
			void *buf, size_t size,
			u16 x, u16 y, u16 w, u16 h);

	void (*get_resolution)(struct omap_dss_device *dssdev,
			u16 *xres, u16 *yres);
	void (*get_dimensions)(struct omap_dss_device *dssdev,
			u32 *width, u32 *height);
	int (*get_recommended_bpp)(struct omap_dss_device *dssdev);

	int (*check_timings)(struct omap_dss_device *dssdev,
			struct omap_video_timings *timings);
	void (*set_timings)(struct omap_dss_device *dssdev,
			struct omap_video_timings *timings);
	void (*get_timings)(struct omap_dss_device *dssdev,
			struct omap_video_timings *timings);

	int (*set_wss)(struct omap_dss_device *dssdev, u32 wss);
	u32 (*get_wss)(struct omap_dss_device *dssdev);

	int (*read_edid)(struct omap_dss_device *dssdev, u8 *buf, int len);
	bool (*detect)(struct omap_dss_device *dssdev);

	/*
                                                            
                                       
  */
	/*
                                                        
                                
  */
	int (*audio_enable)(struct omap_dss_device *dssdev);
	void (*audio_disable)(struct omap_dss_device *dssdev);
	bool (*audio_supported)(struct omap_dss_device *dssdev);
	int (*audio_config)(struct omap_dss_device *dssdev,
		struct omap_dss_audio *audio);
	/*                                     */
	int (*audio_start)(struct omap_dss_device *dssdev);
	void (*audio_stop)(struct omap_dss_device *dssdev);

};

enum omapdss_version omapdss_get_version(void);
bool omapdss_is_initialized(void);

int omap_dss_register_driver(struct omap_dss_driver *);
void omap_dss_unregister_driver(struct omap_dss_driver *);

void omap_dss_get_device(struct omap_dss_device *dssdev);
void omap_dss_put_device(struct omap_dss_device *dssdev);
#define for_each_dss_dev(d) while ((d = omap_dss_get_next_device(d)) != NULL)
struct omap_dss_device *omap_dss_get_next_device(struct omap_dss_device *from);
struct omap_dss_device *omap_dss_find_device(void *data,
		int (*match)(struct omap_dss_device *dssdev, void *data));
const char *omapdss_get_default_display_name(void);

int omap_dss_start_device(struct omap_dss_device *dssdev);
void omap_dss_stop_device(struct omap_dss_device *dssdev);

int dss_feat_get_num_mgrs(void);
int dss_feat_get_num_ovls(void);
enum omap_display_type dss_feat_get_supported_displays(enum omap_channel channel);
enum omap_dss_output_id dss_feat_get_supported_outputs(enum omap_channel channel);
enum omap_color_mode dss_feat_get_supported_color_modes(enum omap_plane plane);



int omap_dss_get_num_overlay_managers(void);
struct omap_overlay_manager *omap_dss_get_overlay_manager(int num);

int omap_dss_get_num_overlays(void);
struct omap_overlay *omap_dss_get_overlay(int num);

struct omap_dss_output *omap_dss_get_output(enum omap_dss_output_id id);
int omapdss_output_set_device(struct omap_dss_output *out,
		struct omap_dss_device *dssdev);
int omapdss_output_unset_device(struct omap_dss_output *out);

void omapdss_default_get_resolution(struct omap_dss_device *dssdev,
		u16 *xres, u16 *yres);
int omapdss_default_get_recommended_bpp(struct omap_dss_device *dssdev);
void omapdss_default_get_timings(struct omap_dss_device *dssdev,
		struct omap_video_timings *timings);

typedef void (*omap_dispc_isr_t) (void *arg, u32 mask);
int omap_dispc_register_isr(omap_dispc_isr_t isr, void *arg, u32 mask);
int omap_dispc_unregister_isr(omap_dispc_isr_t isr, void *arg, u32 mask);

u32 dispc_read_irqstatus(void);
void dispc_clear_irqstatus(u32 mask);
u32 dispc_read_irqenable(void);
void dispc_write_irqenable(u32 mask);

int dispc_request_irq(irq_handler_t handler, void *dev_id);
void dispc_free_irq(void *dev_id);

int dispc_runtime_get(void);
void dispc_runtime_put(void);

void dispc_mgr_enable(enum omap_channel channel, bool enable);
bool dispc_mgr_is_enabled(enum omap_channel channel);
u32 dispc_mgr_get_vsync_irq(enum omap_channel channel);
u32 dispc_mgr_get_framedone_irq(enum omap_channel channel);
u32 dispc_mgr_get_sync_lost_irq(enum omap_channel channel);
bool dispc_mgr_go_busy(enum omap_channel channel);
void dispc_mgr_go(enum omap_channel channel);
void dispc_mgr_set_lcd_config(enum omap_channel channel,
		const struct dss_lcd_mgr_config *config);
void dispc_mgr_set_timings(enum omap_channel channel,
		const struct omap_video_timings *timings);
void dispc_mgr_setup(enum omap_channel channel,
		const struct omap_overlay_manager_info *info);

int dispc_ovl_check(enum omap_plane plane, enum omap_channel channel,
		const struct omap_overlay_info *oi,
		const struct omap_video_timings *timings,
		int *x_predecim, int *y_predecim);

int dispc_ovl_enable(enum omap_plane plane, bool enable);
bool dispc_ovl_enabled(enum omap_plane plane);
void dispc_ovl_set_channel_out(enum omap_plane plane,
		enum omap_channel channel);
int dispc_ovl_setup(enum omap_plane plane, const struct omap_overlay_info *oi,
		bool replication, const struct omap_video_timings *mgr_timings,
		bool mem_to_mem);

#define to_dss_driver(x) container_of((x), struct omap_dss_driver, driver)
#define to_dss_device(x) container_of((x), struct omap_dss_device, dev)

void omapdss_dsi_vc_enable_hs(struct omap_dss_device *dssdev, int channel,
		bool enable);
int omapdss_dsi_enable_te(struct omap_dss_device *dssdev, bool enable);
int omapdss_dsi_set_config(struct omap_dss_device *dssdev,
		const struct omap_dss_dsi_config *config);

int omap_dsi_update(struct omap_dss_device *dssdev, int channel,
		void (*callback)(int, void *), void *data);
int omap_dsi_request_vc(struct omap_dss_device *dssdev, int *channel);
int omap_dsi_set_vc_id(struct omap_dss_device *dssdev, int channel, int vc_id);
void omap_dsi_release_vc(struct omap_dss_device *dssdev, int channel);
int omapdss_dsi_configure_pins(struct omap_dss_device *dssdev,
		const struct omap_dsi_pin_config *pin_cfg);

int omapdss_dsi_display_enable(struct omap_dss_device *dssdev);
void omapdss_dsi_display_disable(struct omap_dss_device *dssdev,
		bool disconnect_lanes, bool enter_ulps);

int omapdss_dpi_display_enable(struct omap_dss_device *dssdev);
void omapdss_dpi_display_disable(struct omap_dss_device *dssdev);
void omapdss_dpi_set_timings(struct omap_dss_device *dssdev,
		struct omap_video_timings *timings);
int dpi_check_timings(struct omap_dss_device *dssdev,
			struct omap_video_timings *timings);
void omapdss_dpi_set_data_lines(struct omap_dss_device *dssdev, int data_lines);

int omapdss_sdi_display_enable(struct omap_dss_device *dssdev);
void omapdss_sdi_display_disable(struct omap_dss_device *dssdev);
void omapdss_sdi_set_timings(struct omap_dss_device *dssdev,
		struct omap_video_timings *timings);
void omapdss_sdi_set_datapairs(struct omap_dss_device *dssdev, int datapairs);

int omapdss_rfbi_display_enable(struct omap_dss_device *dssdev);
void omapdss_rfbi_display_disable(struct omap_dss_device *dssdev);
int omap_rfbi_update(struct omap_dss_device *dssdev, void (*callback)(void *),
		void *data);
int omap_rfbi_configure(struct omap_dss_device *dssdev);
void omapdss_rfbi_set_size(struct omap_dss_device *dssdev, u16 w, u16 h);
void omapdss_rfbi_set_pixel_size(struct omap_dss_device *dssdev,
		int pixel_size);
void omapdss_rfbi_set_data_lines(struct omap_dss_device *dssdev,
		int data_lines);
void omapdss_rfbi_set_interface_timings(struct omap_dss_device *dssdev,
		struct rfbi_timings *timings);

int omapdss_compat_init(void);
void omapdss_compat_uninit(void);

struct dss_mgr_ops {
	void (*start_update)(struct omap_overlay_manager *mgr);
	int (*enable)(struct omap_overlay_manager *mgr);
	void (*disable)(struct omap_overlay_manager *mgr);
	void (*set_timings)(struct omap_overlay_manager *mgr,
			const struct omap_video_timings *timings);
	void (*set_lcd_config)(struct omap_overlay_manager *mgr,
			const struct dss_lcd_mgr_config *config);
	int (*register_framedone_handler)(struct omap_overlay_manager *mgr,
			void (*handler)(void *), void *data);
	void (*unregister_framedone_handler)(struct omap_overlay_manager *mgr,
			void (*handler)(void *), void *data);
};

int dss_install_mgr_ops(const struct dss_mgr_ops *mgr_ops);
void dss_uninstall_mgr_ops(void);

void dss_mgr_set_timings(struct omap_overlay_manager *mgr,
		const struct omap_video_timings *timings);
void dss_mgr_set_lcd_config(struct omap_overlay_manager *mgr,
		const struct dss_lcd_mgr_config *config);
int dss_mgr_enable(struct omap_overlay_manager *mgr);
void dss_mgr_disable(struct omap_overlay_manager *mgr);
void dss_mgr_start_update(struct omap_overlay_manager *mgr);
int dss_mgr_register_framedone_handler(struct omap_overlay_manager *mgr,
		void (*handler)(void *), void *data);
void dss_mgr_unregister_framedone_handler(struct omap_overlay_manager *mgr,
		void (*handler)(void *), void *data);
#endif
