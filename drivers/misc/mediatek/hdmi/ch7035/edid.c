/*                */

/*               */
/*                    */
/*                     */
/*                     */
#include <linux/fcntl.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/kernel.h>
/*             */

#include "ch703xtype.h"
#include "ch703xerr.h"
#include "ch703xdriver.h"

#include "edid.h"

/*                                                         */
/*                                                         */
/*                                                         */

/*                                                         */
#define ASPECT_16_10	0
#define ASPECT_4_3	1
#define ASPECT_5_4	2
#define ASPECT_16_9	3
#define N_ASPECTS	4

/*                                                            */

#define	MAX_USER_MODES		32	/*                             */
#define MAX_TOTAL_MODES		(N_KNOWN_MODES + MAX_USER_MODES)
#define DEF_OUTPUT_MODE		HDMI_16_1920x1080P_60

static int gKnownModesNum = N_KNOWN_MODES;
static EDID_TIMING gKnownModes[MAX_TOTAL_MODES + 1] = {
	{			/*                   */
	 154000, MK_PIXEL_HDMI(1, 0, AS_RATIO_NA), POL_HIGH, POL_LOW, POL_HIGH,
	 60, 2080, 1920, 48, 32, 1235, 1200, 3, 6,
	 },
	{			/*                   */
	 162000, MK_PIXEL_HDMI(1, 0, AS_RATIO_4_3), POL_HIGH, POL_HIGH, POL_HIGH,
	 60, 2160, 1600, 64, 192, 1250, 1200, 1, 3,
	 },
	{			/*                       */
	 148500, MK_PIXEL_HDMI(0, 16, AS_RATIO_16_9), POL_HIGH, POL_HIGH, POL_HIGH,
	 60, 2200, 1920, 88, 44, 1125, 1080, 4, 5,
	 },
	{			/*                       */
	 148500, MK_PIXEL_HDMI(0, 31, AS_RATIO_16_9), POL_HIGH, POL_HIGH, POL_HIGH,
	 50, 2640, 1920, 528, 44, 1125, 1080, 4, 5,
	 },
	{			/*                       */
	 74250, MK_PIXEL_HDMI_I(0, 5, AS_RATIO_16_9), POL_HIGH, POL_HIGH, POL_HIGH,
	 60, 2200, 1920, 88, 44, 1125, 1080, 4, 5,
	 },
	{			/*                       */
	 74250, MK_PIXEL_HDMI_I(0, 20, AS_RATIO_16_9), POL_HIGH, POL_HIGH, POL_HIGH,
	 50, 2640, 1920, 528, 44, 1125, 1080, 4, 5,
	 },
	{			/*                                      */
	 119000, MK_PIXEL_HDMI_R(1, 0, AS_RATIO_16_10), POL_HIGH, POL_LOW, POL_HIGH,
	 60, 1840, 1680, 48, 32, 1080, 1050, 3, 6,
	 },
	{			/*                   */
	 156000, MK_PIXEL_HDMI(1, 0, AS_RATIO_16_10), POL_LOW, POL_HIGH, POL_HIGH,
	 75, 1896, 1400, 104, 144, 1099, 1050, 3, 4,
	 },
	{			/*                                      */
	 101000, MK_PIXEL_HDMI_R(1, 0, AS_RATIO_16_10), POL_HIGH, POL_LOW, POL_HIGH,
	 60, 1560, 1400, 48, 32, 1080, 1050, 3, 4,
	 },
	{			/*                   */
	 157500, MK_PIXEL_HDMI(1, 0, AS_RATIO_5_4), POL_HIGH, POL_HIGH, POL_HIGH,
	 85, 1728, 1280, 64, 160, 1072, 1024, 1, 3,
	 },
	{			/*                   */
	 135000, MK_PIXEL_HDMI(1, 0, AS_RATIO_5_4), POL_HIGH, POL_HIGH, POL_HIGH,
	 75, 1688, 1280, 16, 144, 1066, 1024, 1, 3,
	 },
	{			/*                   */
	 108000, MK_PIXEL_HDMI(1, 0, AS_RATIO_5_4), POL_HIGH, POL_HIGH, POL_HIGH,
	 60, 1688, 1280, 48, 112, 1066, 1024, 1, 3,
	 },
	{			/*                        */
	 75428, MK_PIXEL_HDMI(1, 0, AS_RATIO_5_4), POL_HIGH, POL_HIGH, POL_HIGH,
	 50, 1440, 1280, 32, 48, 1049, 1024, 3, 7,
	 },
	{			/*                       */
	 108000, MK_PIXEL_HDMI(1, 0, AS_RATIO_4_3), POL_HIGH, POL_HIGH, POL_HIGH,
	 60, 1800, 1280, 96, 112, 1000, 960, 1, 3,
	 },
	{			/*                  */
	 136750, MK_PIXEL_HDMI(1, 0, AS_RATIO_16_10), POL_LOW, POL_HIGH, POL_HIGH,
	 75, 1936, 1440, 96, 152, 942, 900, 3, 6,
	 },
	{			/*                                     */
	 88750, MK_PIXEL_HDMI_R(1, 0, AS_RATIO_16_10), POL_HIGH, POL_LOW, POL_HIGH,
	 60, 1600, 1440, 48, 32, 934, 900, 3, 6,
	 },
	{			/*                  */
	 106500, MK_PIXEL_HDMI(1, 0, AS_RATIO_16_10), POL_LOW, POL_HIGH, POL_HIGH,
	 60, 1904, 1440, 80, 152, 926, 900, 3, 6,
	 },
	{			/*                  */
	 106500, MK_PIXEL_HDMI(1, 0, AS_RATIO_NA), POL_LOW, POL_HIGH, POL_HIGH,
	 75, 1696, 1280, 80, 128, 838, 800, 3, 6,
	 },
	{			/*                                     */
	 71000, MK_PIXEL_HDMI_R(1, 0, AS_RATIO_NA), POL_HIGH, POL_LOW, POL_HIGH,
	 60, 1440, 1280, 48, 32, 823, 800, 3, 6,
	 },
	{			/*                  */
	 76000, MK_PIXEL_HDMI(1, 0, AS_RATIO_16_9), POL_LOW, POL_LOW, POL_LOW,
	 60, 1560, 1366, 32, 64, 806, 768, 6, 12,
	 },
	{			/*                                     */
	 72350, MK_PIXEL_HDMI_R(1, 0, AS_RATIO_16_9), POL_HIGH, POL_HIGH, POL_HIGH,
	 60, 1526, 1366, 48, 32, 790, 768, 3, 5,
	 },
	{			/*                  */
	 85500, MK_PIXEL_HDMI(1, 0, AS_RATIO_NA), POL_HIGH, POL_HIGH, POL_HIGH,
	 60, 1792, 1360, 64, 112, 795, 768, 3, 6,
	 },
	{			/*                       */
	 102250, MK_PIXEL_HDMI(1, 0, AS_RATIO_NA), POL_LOW, POL_HIGH, POL_HIGH,
	 75, 1696, 1280, 80, 128, 805, 768, 3, 7,
	 },
	{			/*                                      */
	 68250, MK_PIXEL_HDMI_R(1, 0, AS_RATIO_NA), POL_HIGH, POL_LOW, POL_HIGH,
	 60, 1440, 1280, 48, 32, 790, 768, 3, 7,
	 },
	{			/*                                      */
	 79500, MK_PIXEL_HDMI(1, 0, AS_RATIO_NA), POL_LOW, POL_HIGH, POL_HIGH,
	 60, 1664, 1280, 64, 128, 798, 768, 3, 7,
	 },
	{			/*                                           */
	 80120, MK_PIXEL_HDMI(1, 0, AS_RATIO_NA), POL_HIGH, POL_HIGH, POL_HIGH,
	 60, 1680, 1280, 64, 134, 795, 768, 1, 3,
	 },
	{			/*                  */
	 94500, MK_PIXEL_HDMI(1, 0, AS_RATIO_4_3), POL_HIGH, POL_HIGH, POL_HIGH,
	 85, 1376, 1024, 48, 96, 808, 768, 1, 3,
	 },
	{			/*                       */
	 78750, MK_PIXEL_HDMI(1, 0, AS_RATIO_4_3), POL_HIGH, POL_HIGH, POL_HIGH,
	 75, 1312, 1024, 16, 96, 800, 768, 1, 3,
	 },
	{			/*                  */
	 75000, MK_PIXEL_HDMI(1, 0, AS_RATIO_4_3), POL_LOW, POL_LOW, POL_LOW,
	 70, 1328, 1024, 24, 136, 806, 768, 3, 6,
	 },
	{			/*                  */
	 65000, MK_PIXEL_HDMI(1, 0, AS_RATIO_4_3), POL_LOW, POL_LOW, POL_LOW,
	 60, 1344, 1024, 24, 136, 806, 768, 3, 6,
	 },
	{			/*                      */
	 74250, MK_PIXEL_HDMI(0, 4, AS_RATIO_16_9), POL_HIGH, POL_HIGH, POL_HIGH,
	 60, 1650, 1280, 110, 40, 750, 720, 5, 5,
	 },
	{			/*                      */
	 74250, MK_PIXEL_HDMI(0, 19, AS_RATIO_16_9), POL_HIGH, POL_HIGH, POL_HIGH,
	 50, 1980, 1280, 440, 40, 750, 720, 5, 5,
	 },
	{			/*                  */
	 47360, MK_PIXEL_HDMI(1, 0, AS_RATIO_NA), POL_HIGH, POL_HIGH, POL_HIGH,
	 60, 1240, 1024, 40, 128, 638, 600, 10, 3,
	 },
	{			/*                   */
	 45000, MK_PIXEL_HDMI(1, 0, AS_RATIO_NA), POL_LOW, POL_LOW, POL_LOW,
	 60, 1200, 1024, 40, 118, 625, 600, 1, 4,
	 },
	{			/*                 */
	 56250, MK_PIXEL_HDMI(1, 0, AS_RATIO_4_3), POL_HIGH, POL_HIGH, POL_HIGH,
	 85, 1048, 800, 32, 64, 631, 600, 1, 3,
	 },
	{			/*                 */
	 49500, MK_PIXEL_HDMI(1, 0, AS_RATIO_4_3), POL_HIGH, POL_HIGH, POL_HIGH,
	 75, 1056, 800, 16, 80, 625, 600, 1, 3,
	 },
	{			/*                 */
	 40000, MK_PIXEL_HDMI(1, 0, AS_RATIO_4_3), POL_HIGH, POL_HIGH, POL_HIGH,
	 60, 1056, 800, 40, 128, 628, 600, 1, 4,
	 },
	{			/*                     */
	 27000, MK_PIXEL_HDMI(0, 17, AS_RATIO_4_3), POL_HIGH, POL_HIGH, POL_HIGH,
	 50, 864, 720, 12, 64, 625, 576, 5, 5,
	 },
	{			/*                     */
	 27000, MK_PIXEL_HDMI(0, 2, AS_RATIO_16_9), POL_HIGH, POL_HIGH, POL_HIGH,
	 60, 858, 720, 16, 62, 525, 480, 9, 6,
	 },
	{			/*                      */
	 36000, MK_PIXEL_HDMI(1, 0, AS_RATIO_4_3), POL_LOW, POL_LOW, POL_LOW,
	 85, 832, 640, 56, 56, 509, 480, 1, 3,
	 },
	{			/*                      */
	 31500, MK_PIXEL_HDMI(1, 0, AS_RATIO_4_3), POL_LOW, POL_LOW, POL_LOW,
	 75, 840, 640, 16, 64, 500, 480, 1, 3,
	 },
	{			/*                                          */
	 31500, MK_PIXEL_HDMI(1, 0, AS_RATIO_4_3), POL_LOW, POL_LOW, POL_LOW,
	 72, 832, 640, 24, 40, 520, 480, 9, 3,
	 },
	{			/*                     */
	 25200, MK_PIXEL_HDMI(0, 1, AS_RATIO_4_3), POL_LOW, POL_LOW, POL_HIGH,
	 60, 800, 640, 16, 96, 525, 480, 10, 2,
	 },

	{			/*                 */
	 35500, MK_PIXEL_HDMI(1, 0, AS_RATIO_NA), POL_LOW, POL_HIGH, POL_HIGH,
	 85, 936, 720, 36, 72, 446, 400, 1, 3,
	 },
	{			/*                 */
	 31500, MK_PIXEL_HDMI(1, 0, AS_RATIO_NA), POL_LOW, POL_HIGH, POL_HIGH,
	 85, 832, 640, 32, 64, 445, 400, 1, 3,
	 },

	/*                                          */
};

typedef struct _ESTABLISHTIMING {
	int mode;		/*                      */
	uint8 idx;		/*              */
	uint8 mask;		/*                   */
} ESTABLISHTIMING;

static ESTABLISHTIMING edid_estab[] = {
/*                                                                                   */
	{DVI_720x400P_85, 35, 0x40},	/*            */
	{HDMI_01_640x480P_60, 35, 0x20},	/*            */
/*                                                                           */
	{VGA_640x480P_72, 35, 0x08},	/*            */
	{VGA_640x480P_75, 35, 0x04},	/*            */
/*                                                                                   */
	{VGA_800x600P_60, 35, 0x01},	/*            */
/*                                                                                   */
	{VGA_800x600P_75, 36, 0x40},	/*            */
/*                                                                           */
/*                                                                                     */
	{VGA_1024x768P_60, 36, 0x08},	/*             */
	{VGA_1024x768P_70, 36, 0x04},	/*             */
	{VGA_1024x768P_75, 36, 0x02},	/*             */
	{DVI_1280x1024P_75, 36, 0x01},	/*              */
/*                                                                            */
};

/*                                            */
/*                                                          */
#define	MAX_MODES_IN_EDID		MAX_TOTAL_MODES
static unsigned char gEDIDModes[MAX_MODES_IN_EDID];
static int gEDIDModesNum = 0, gNativeModeIndex = 0, gFHDIndex;

int edid_valid(uint8 *edid)
{
	return ((edid[0] == 0x00) && (edid[1] == 0xff) &&
		(edid[2] == 0xff) && (edid[3] == 0xff) &&
		(edid[4] == 0xff) && (edid[5] == 0xff) && (edid[6] == 0xff) && (edid[7] == 0x00));
}


/*                                                                                 */
/*   */
/*                       */
/*                                         */
/*                                     */
/*                                                  */
/*                                                          */
/*                                                              */
/*                                                                 */
/*   */


/*                                                             */
/*                                                                        */
/*                                                                */
/*                                        */
/*                                                      */
/*                                                                             */
/*                                                     */
/*                                          */
/*                                          */
/*  */
/*                                                                            */
/*          */
#define		MATCHMODE_REFRESH_RATE			0x0001
#define		MATCHMODE_TOTAL_PIXELS			0x0002
#define		MATCHMODE_PIXEL_CLOCK			0x0004
#define		MATCHMODE_INTERLACED			0x0008

static int match_timing_mode(EDID_TIMING modes[], int num_modes,
			     EDID_TIMING *timing, int match_mode)
{
	int i;

	for (i = 0; i < num_modes; i++) {
		/*                             */
		if (modes[i].ha != timing->ha || modes[i].va != timing->va)
			continue;

		/*                      */
		if ((match_mode & MATCHMODE_REFRESH_RATE) && (modes[i].fps != timing->fps))
			continue;

		/*                      */
		if ((match_mode & MATCHMODE_TOTAL_PIXELS) &&
		    (modes[i].ht != timing->ht || modes[i].vt != timing->vt))
			continue;

		/*                     */
		if ((match_mode & MATCHMODE_PIXEL_CLOCK) && (modes[i].clk_khz != timing->clk_khz))
			continue;

		/*                                   */
		if ((match_mode & MATCHMODE_INTERLACED) &&
		    ((modes[i].pixel_fmt ^ timing->pixel_fmt) & PIXEL_HDMI_ENCODE_INTERLACE))
			continue;

/*                                                          */
		return i;
	}

/*                                               */
	return -1;
}

static int find_hdmi_mode(uint8 fmtID)
{
	int i;

	for (i = 0; i < N_KNOWN_MODES; i++) {
		if (!(gKnownModes[i].pixel_fmt & PIXEL_HDMI_ENCODE_DVI) &&
		    ((uint8) PIXEL_HDMI_FOTMAT(gKnownModes[i].pixel_fmt) == fmtID))
			return i;
	}

	return -1;
}

int get_DTD_timing(uint8 *dtd, EDID_TIMING *timing)
{
	uint32 tmp32;
	int idx;
	uint8 mdflg;
/*                                     */

	tmp32 = dtd[0] + (dtd[1] << 8);
	if (tmp32 == 0)
		return ERR_INVALID_DTD;	/*           */

	timing->clk_khz = tmp32 * 10;
	timing->ha = dtd[2] + ((dtd[4] & 0xf0) << 4);	/*      */
	timing->ht = timing->ha + (dtd[3] + ((dtd[4] & 0x0f) << 8));	/*     */
	timing->va = dtd[5] + ((dtd[7] & 0xf0) << 4);	/*      */
	timing->vt = timing->va + (dtd[6] + ((dtd[7] & 0x0f) << 8));	/*     */
	timing->ho = dtd[8] + ((dtd[11] & 0xc0) << 2);	/*     */
	timing->hw = dtd[9] + ((dtd[11] & 0x30) << 4);	/*     */
	timing->vo = (dtd[10] >> 4) + ((dtd[11] & 0x0c) << 2);	/*      */
	timing->vw = (dtd[10] & 0xf) + ((dtd[11] & 0x03) << 4);	/*     */
/*                                         */
/*                                         */
/*                 */
/*                 */

	mdflg = dtd[17];
	timing->de_pol =	/*                              */
	    timing->hs_pol = (mdflg & 2) ? POL_HIGH : POL_LOW;
	timing->vs_pol = (mdflg & 4) ? POL_HIGH : POL_LOW;

	/*                                                                     */
	/*                               */
	/*                                                                     */
	/*                         */
	/*                                                    */
	if (mdflg & 0x80) {
		/*                                              */
		timing->va <<= 1;
		timing->vt = (timing->vt << 1) + 1;
		timing->vo <<= 1;
		/*                            */

		/*                                                */
		timing->pixel_fmt = MK_PIXEL_HDMI_I(1, 0, AS_RATIO_NA);
		tmp32 = (timing->ht * timing->vt) >> 1;
	} else {
		/*                                                */
		timing->pixel_fmt = MK_PIXEL_HDMI(1, 0, AS_RATIO_NA);
		tmp32 = timing->ht * timing->vt;
	}
	if (tmp32 > 0) {
		timing->fps = (uint8) ((timing->clk_khz * 1000 + (tmp32 >> 1)) / tmp32);
	} else {
		timing->fps = 60;
	}
	/*                                            */
	idx = match_timing_mode(gKnownModes, gKnownModesNum, timing,
				MATCHMODE_PIXEL_CLOCK | MATCHMODE_TOTAL_PIXELS);
	if (idx >= 0)
		timing->pixel_fmt = gKnownModes[idx].pixel_fmt;

	return idx;
}

static void find_native_mode(void)
{
	int i, n;
	EDID_TIMING *t, *cur;

	n = 0;			/*                                   */
	cur = &gKnownModes[gEDIDModes[0]];
	for (i = 1; i < gEDIDModesNum; i++) {
		t = &gKnownModes[gEDIDModes[i]];

		/*                                                                                       */
		/*                        */
		if (!(t->pixel_fmt & PIXEL_HDMI_ENCODE_NATIVE))
			continue;

		/*                        */
		if (t->pixel_fmt & PIXEL_HDMI_ENCODE_INTERLACE)
			continue;

		/*                                               */
		if (cur->pixel_fmt & PIXEL_HDMI_ENCODE_NATIVE) {
			if (t->va < cur->va)
				continue;	/*                    */

			if (t->va == cur->va) {
				if (t->ha < cur->ha)
					continue;	/*                    */

				if (t->ha == cur->ha) {
					if (t->clk_khz < cur->clk_khz)
						continue;	/*                    */
				}
			}
		}
/*                                                    */
		n = i;
		cur = t;
	}

	gNativeModeIndex = n;
}

static void find_fhd(void)
{
	int i, n;
	EDID_TIMING *t, *cur;

	n = 0;			/*                         */
	cur = &gKnownModes[gEDIDModes[0]];
	for (i = 1; i < gEDIDModesNum; i++) {
		t = &gKnownModes[gEDIDModes[i]];

		/*                        */
		if (t->pixel_fmt & PIXEL_HDMI_ENCODE_INTERLACE)
			continue;

		if (t->va < cur->va)
			continue;	/*                    */

		if (t->va == cur->va) {
			if (t->ha < cur->ha)
				continue;	/*                    */

			if (t->ha == cur->ha) {
				if (t->clk_khz < cur->clk_khz)
					continue;	/*                    */
				if (t->clk_khz == cur->clk_khz) {
					if (t->fps < cur->fps)
						continue;	/*                  */
				}
			}
		}

		n = i;
		cur = t;
	}

	gFHDIndex = n;
}

/*                                                                      */
/*                                                             */
static void insert_edid_mode(int newIdx)
{
	int i, j;
	EDID_TIMING *t, *cur;

	cur = &gKnownModes[newIdx];
	for (i = 0; i < gEDIDModesNum; i++) {
		t = &gKnownModes[gEDIDModes[i]];

		if (t->va < cur->va)
			break;

		if (t->va == cur->va) {
			if (t->ha < cur->ha)
				break;

			if (t->ha == cur->ha) {
				if (t->clk_khz < cur->clk_khz)
					break;
			}
		}
	}

	for (j = gEDIDModesNum; j > i; j--)
		gEDIDModes[j] = gEDIDModes[j - 1];

	gEDIDModes[i] = newIdx;
	gEDIDModesNum++;
}

static void add_edid_mode(int mode, int bNative)
{
	int i;

	/*                                                              */
	/*                                      */
	for (i = 0; i < gEDIDModesNum; i++) {
		if (mode == gEDIDModes[i]) {
/*                                                                  */
			return;
		}
	}

/*                                                           */
	if (gEDIDModesNum >= MAX_MODES_IN_EDID) {
		pr_debug("too many modes %d in EDID\n", gEDIDModesNum);	/*            */
		return;
	}
	insert_edid_mode(mode);

	if (bNative)
		gKnownModes[mode].pixel_fmt |= PIXEL_HDMI_ENCODE_NATIVE;
	else
		gKnownModes[mode].pixel_fmt &= ~PIXEL_HDMI_ENCODE_NATIVE;
}

static void add_edid_dtd(uint8 *dtd, int bNative)
{
	EDID_TIMING *timing;
	int idx;

	timing = &gKnownModes[gKnownModesNum];
	idx = get_DTD_timing(dtd, timing);
	if (idx < -1)		/*                  */
		return;

	if (idx == -1) {	/*                       */
/*                                                              */
		if (gKnownModesNum >= MAX_TOTAL_MODES) {	/*                           */
			pr_debug("too many user timing modes %d\n", gKnownModesNum);	/*            */
			return;
		}
		idx = gKnownModesNum++;
	}
	/*                                            */
	add_edid_mode(idx, bNative);
}

static void add_edid_standard(uint8 l, uint8 p, int bNative)
{
	EDID_TIMING *timing;
	int idx;
	uint8 as_ratio;

	/*                                        */
	if (((l == 0x01) && (p == 0x01)) ||
	    ((l == 0x00) && (p == 0x00)) || ((l == 0x20) && (p == 0x20)))
		return;

	timing = &gKnownModes[gKnownModesNum];
	timing->ha = (uint16) (l + 31) << 3;
	switch (p >> 6) {
	case ASPECT_16_10:
		as_ratio = AS_RATIO_16_10;
		timing->va = (timing->ha * 5) >> 3;
		break;
	case ASPECT_4_3:
		as_ratio = AS_RATIO_4_3;
		timing->va = (timing->ha * 3) >> 2;
		break;
	case ASPECT_5_4:
		as_ratio = AS_RATIO_5_4;
		timing->va = (timing->ha * 4) / 5;
		break;
	default:		/*                   */
		as_ratio = AS_RATIO_16_9;
		timing->va = (timing->ha * 9) >> 4;
	}
	timing->fps = 60 + (p & 0x3f);
	timing->pixel_fmt = MK_PIXEL_HDMI(0, 0, as_ratio);	/*                    */

	/*                                                                 */
	/*                                                               */
	idx = match_timing_mode(gKnownModes, N_KNOWN_MODES, timing,
				MATCHMODE_INTERLACED | MATCHMODE_REFRESH_RATE);
	if (idx >= 0)
		add_edid_mode(idx, bNative);
}

/*        */
/*                                             */
/*                                             */
/*                                         */
/*                                            */
int parse_edid_timing(uint8 *edid)
{
	int i, idx;
	uint8 l, p, n;
	uint8 *ext, *dtd;

	/*                         */
	gEDIDModesNum = 0;
	gKnownModesNum = N_KNOWN_MODES;

	if (!edid_valid(edid)) {
		pr_debug("invalid EDID\n");	/*            */
		return ERR_INVALID_EDID;
	}
/*                                                            */
	for (i = 0; i < (int)(sizeof(edid_estab) / sizeof(ESTABLISHTIMING)); i++) {
		if (edid[edid_estab[i].idx] & edid_estab[i].mask) {
			add_edid_mode(edid_estab[i].mode, 0);	/*            */
		}
	}

/*                                                         */
	for (i = 0; i < 8; i++) {
		add_edid_standard(edid[38 + 2 * i], edid[39 + 2 * i], 0);	/*            */
	}

/*                                             */
	for (dtd = edid + 54, i = 0; i < 4; i++, dtd += 18) {
		add_edid_dtd(dtd, 1);	/*        */
	}

	for (ext = edid + 128, n = 1; n <= edid[126]; n++, ext += 128) {
/*                                           */

		/*                                 */
		if ((ext[0] != 0x02) || (ext[1] != 0x03))
			continue;

/*                                                */
		p = 4;
		while (p < ext[2]) {
			l = ext[p] & 0x1f;
			if (0x40 != (ext[p] & 0xE0)) {	/*                             */
				p += l + 1;
				continue;
			}

			for (p++; l > 0; l--, p++) {
				/*                                                   */
				idx = find_hdmi_mode(ext[p] & 0x7F);
				if (idx >= 0)
					add_edid_mode(idx, ext[p] & 0x80);
			}
		}

		/*                          */
		if (ext[2] < 4)
			continue;

/*                                                          */
		for (dtd = ext + ext[2], i = (ext[3] & 0x0F); i > 0; i--, dtd += 18) {
			add_edid_dtd(dtd, 1);	/*        */
		}
	}

	find_native_mode();
	find_fhd();

	pr_debug("Total %d timing modes in EDID, %d is native mode\n", gEDIDModesNum, gNativeModeIndex);	/*            */

	return gEDIDModesNum;
}

/*                                        */
/*                                                              */
/*                   */
int get_known_timing(EDID_TIMING *timing, int index)
{
	/*                                                       */
	if (index < 0)		/*              */
		index = DEF_OUTPUT_MODE;

	if (index >= N_KNOWN_MODES)
		return ERR_UNKOWN_MODE;

	memcpy(timing, &gKnownModes[index], sizeof(EDID_TIMING));

	return index;
}

/*                                  */
/*                                                */
/*                                                                           */
int get_edid_timing(EDID_TIMING *timing, int index)
{
	/*                                                     */
	if (index < 0)		/*             */
		index = gFHDIndex;	/*                   */

	if (index >= gEDIDModesNum)
		return ERR_UNKOWN_EDID_MODE;

	index = gEDIDModes[index];	/*                                         */

	memcpy(timing, &gKnownModes[index], sizeof(EDID_TIMING));

	return index;
}
