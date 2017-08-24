#ifndef __DDP_AAL_H__
#define __DDP_AAL_H__

#define AAL_HIST_BIN        33	/*         */
#define AAL_DRE_POINT_NUM   29

#define AAL_SERVICE_FORCE_UPDATE 0x1

typedef struct {
	/*     */
	int dre_map_bypass;
	/*      */
	int cabc_gainlmt[33];
} DISP_AAL_INITREG;

typedef struct {
	unsigned int serviceFlags;
	int backlight;
	unsigned int maxHist[AAL_HIST_BIN];
} DISP_AAL_HIST;

typedef struct {
	int DREGainFltStatus[AAL_DRE_POINT_NUM];
	int cabc_fltgain_force;	/*                   */
	int cabc_gainlmt[33];
	int FinalBacklight;	/*                   */
} DISP_AAL_PARAM;


void disp_aal_on_end_of_frame(void);

extern int aal_dbg_en;

#endif
