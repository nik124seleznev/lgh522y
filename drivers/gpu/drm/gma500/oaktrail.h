/**************************************************************************
 * Copyright (c) 2007-2011, Intel Corporation.
 * All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 *
 **************************************************************************/

/*                                 */

struct oaktrail_timing_info {
	u16 pixel_clock;
	u8 hactive_lo;
	u8 hblank_lo;
	u8 hblank_hi:4;
	u8 hactive_hi:4;
	u8 vactive_lo;
	u8 vblank_lo;
	u8 vblank_hi:4;
	u8 vactive_hi:4;
	u8 hsync_offset_lo;
	u8 hsync_pulse_width_lo;
	u8 vsync_pulse_width_lo:4;
	u8 vsync_offset_lo:4;
	u8 vsync_pulse_width_hi:2;
	u8 vsync_offset_hi:2;
	u8 hsync_pulse_width_hi:2;
	u8 hsync_offset_hi:2;
	u8 width_mm_lo;
	u8 height_mm_lo;
	u8 height_mm_hi:4;
	u8 width_mm_hi:4;
	u8 hborder;
	u8 vborder;
	u8 unknown0:1;
	u8 hsync_positive:1;
	u8 vsync_positive:1;
	u8 separate_sync:2;
	u8 stereo:1;
	u8 unknown6:1;
	u8 interlaced:1;
} __packed;

struct gct_r10_timing_info {
	u16 pixel_clock;
	u32 hactive_lo:8;
	u32 hactive_hi:4;
	u32 hblank_lo:8;
	u32 hblank_hi:4;
	u32 hsync_offset_lo:8;
	u16 hsync_offset_hi:2;
	u16 hsync_pulse_width_lo:8;
	u16 hsync_pulse_width_hi:2;
	u16 hsync_positive:1;
	u16 rsvd_1:3;
	u8  vactive_lo:8;
	u16 vactive_hi:4;
	u16 vblank_lo:8;
	u16 vblank_hi:4;
	u16 vsync_offset_lo:4;
	u16 vsync_offset_hi:2;
	u16 vsync_pulse_width_lo:4;
	u16 vsync_pulse_width_hi:2;
	u16 vsync_positive:1;
	u16 rsvd_2:3;
} __packed;

struct oaktrail_panel_descriptor_v1 {
	u32 Panel_Port_Control; /*                                   */
				/*                 */
	u32 Panel_Power_On_Sequencing;/*                         */
	u32 Panel_Power_Off_Sequencing;/*                         */
	u32 Panel_Power_Cycle_Delay_and_Reference_Divisor;/*         */
						/*                  */
	struct oaktrail_timing_info DTD;/*                              */
	u16 Panel_Backlight_Inverter_Descriptor;/*                     */
				/*                                       */
			/*                                                 */
	u16 Panel_MIPI_Display_Descriptor;
			/*                             */
			/*                         */
			/*                      */
			/*            */
			/*            */
			/*            */
			/*           */
			/*                             */
			/*                                     */
			/*                             */
			/*                     */
			/*             */
			/*                              */
			/*                                                       */
			/*                               */
} __packed;

struct oaktrail_panel_descriptor_v2 {
	u32 Panel_Port_Control; /*                                   */
				/*                 */
	u32 Panel_Power_On_Sequencing;/*                         */
	u32 Panel_Power_Off_Sequencing;/*                         */
	u8 Panel_Power_Cycle_Delay_and_Reference_Divisor;/*        */
						/*                  */
	struct oaktrail_timing_info DTD;/*                              */
	u16 Panel_Backlight_Inverter_Descriptor;/*                   */
				/*                                      */
	u8 Panel_Initial_Brightness;/*                */
			/*                                             */
	u16 Panel_MIPI_Display_Descriptor;
			/*                             */
			/*                         */
			/*                      */
			/*            */
			/*            */
			/*            */
			/*           */
			/*                             */
			/*                                     */
			/*                             */
			/*                     */
			/*             */
			/*                              */
			/*                                                       */
			/*                               */
} __packed;

union oaktrail_panel_rx {
	struct {
		u16 NumberOfLanes:2; /*                                */
			/*                                        */
		u16 MaxLaneFreq:3; /*                                  */
		/*                                                      */
		u16 SupportedVideoTransferMode:2; /*                  */
					/*                        */
					/*               */
		u16 HSClkBehavior:1; /*                                */
		u16 DuoDisplaySupport:1; /*                   */
		u16 ECC_ChecksumCapabilities:1;/*                   */
		u16 BidirectionalCommunication:1;/*                    */
		u16 Rsvd:5;/*              */
	} panelrx;
	u16 panel_receiver;
} __packed;

struct gct_r0 {
	union { /*                           */
		struct {
			u8 PanelType:4; /*                            */
					/*                          */
					/*                             */
			u8 BootPanelIndex:2;
					/*                           */
			u8 BootMIPI_DSI_RxIndex:2;/*                  */
					/*                                */
		} PD;
		u8 PanelDescriptor;
	};
	struct oaktrail_panel_descriptor_v1 panel[4];/*                          */
	union oaktrail_panel_rx panelrx[4]; /*                */
} __packed;

struct gct_r1 {
	union { /*                           */
		struct {
			u8 PanelType:4; /*                            */
					/*                          */
					/*                             */
			u8 BootPanelIndex:2;
					/*                           */
			u8 BootMIPI_DSI_RxIndex:2;/*                  */
					/*                                */
		} PD;
		u8 PanelDescriptor;
	};
	struct oaktrail_panel_descriptor_v2 panel[4];/*                          */
	union oaktrail_panel_rx panelrx[4]; /*                */
} __packed;

struct gct_r10 {
	struct gct_r10_timing_info DTD;
	u16 Panel_MIPI_Display_Descriptor;
	u16 Panel_MIPI_Receiver_Descriptor;
	u16 Panel_Backlight_Inverter_Descriptor;
	u8 Panel_Initial_Brightness;
	u32 MIPI_Ctlr_Init_ptr;
	u32 MIPI_Panel_Init_ptr;
} __packed;

struct oaktrail_gct_data {
	u8 bpi; /*                                                    */
	u8 pt; /*                                         */
	struct oaktrail_timing_info DTD; /*                                    */
	u32 Panel_Port_Control;
	u32 PP_On_Sequencing;/*                         */
	u32 PP_Off_Sequencing;/*                         */
	u32 PP_Cycle_Delay;
	u16 Panel_Backlight_Inverter_Descriptor;
	u16 Panel_MIPI_Display_Descriptor;
} __packed;

#define MODE_SETTING_IN_CRTC		0x1
#define MODE_SETTING_IN_ENCODER		0x2
#define MODE_SETTING_ON_GOING		0x3
#define MODE_SETTING_IN_DSR		0x4
#define MODE_SETTING_ENCODER_DONE	0x8

/*
                             
 */

struct oaktrail_hdmi_dev {
	struct pci_dev *dev;
	void __iomem *regs;
	unsigned int mmio, mmio_len;
	int dpms_mode;
	struct hdmi_i2c_dev *i2c_dev;

	/*                */
	u32 saveDPLL_CTRL;
	u32 saveDPLL_DIV_CTRL;
	u32 saveDPLL_ADJUST;
	u32 saveDPLL_UPDATE;
	u32 saveDPLL_CLK_ENABLE;
	u32 savePCH_HTOTAL_B;
	u32 savePCH_HBLANK_B;
	u32 savePCH_HSYNC_B;
	u32 savePCH_VTOTAL_B;
	u32 savePCH_VBLANK_B;
	u32 savePCH_VSYNC_B;
	u32 savePCH_PIPEBCONF;
	u32 savePCH_PIPEBSRC;
};

extern void oaktrail_hdmi_setup(struct drm_device *dev);
extern void oaktrail_hdmi_teardown(struct drm_device *dev);
extern int  oaktrail_hdmi_i2c_init(struct pci_dev *dev);
extern void oaktrail_hdmi_i2c_exit(struct pci_dev *dev);
extern void oaktrail_hdmi_save(struct drm_device *dev);
extern void oaktrail_hdmi_restore(struct drm_device *dev);
extern void oaktrail_hdmi_init(struct drm_device *dev, struct psb_intel_mode_device *mode_dev);
extern int oaktrail_crtc_hdmi_mode_set(struct drm_crtc *crtc, struct drm_display_mode *mode,
						struct drm_display_mode *adjusted_mode, int x, int y,
						struct drm_framebuffer *old_fb);
extern void oaktrail_crtc_hdmi_dpms(struct drm_crtc *crtc, int mode);


