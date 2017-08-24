/*
 * Copyright (C) 2012 Texas Instruments Inc
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 * Contributors:
 *      Manjunath Hadli <manjunath.hadli@ti.com>
 *      Prabhakar Lad <prabhakar.lad@ti.com>
 */

#ifndef _DAVINCI_VPFE_USER_H
#define _DAVINCI_VPFE_USER_H

#include <linux/types.h>
#include <linux/videodev2.h>

/*
                
  
                                                        
                                                          
                                                           
                                                           
                                                             
                                                             
 */

#define VIDIOC_VPFE_ISIF_S_RAW_PARAMS \
	_IOW('V', BASE_VIDIOC_PRIVATE + 1,  struct vpfe_isif_raw_config)
#define VIDIOC_VPFE_ISIF_G_RAW_PARAMS \
	_IOR('V', BASE_VIDIOC_PRIVATE + 2, struct vpfe_isif_raw_config)
#define VIDIOC_VPFE_IPIPE_S_CONFIG \
	_IOWR('P', BASE_VIDIOC_PRIVATE + 3, struct vpfe_ipipe_config)
#define VIDIOC_VPFE_IPIPE_G_CONFIG \
	_IOWR('P', BASE_VIDIOC_PRIVATE + 4, struct vpfe_ipipe_config)
#define VIDIOC_VPFE_RSZ_S_CONFIG \
	_IOWR('R', BASE_VIDIOC_PRIVATE + 5, struct vpfe_rsz_config)
#define VIDIOC_VPFE_RSZ_G_CONFIG \
	_IOWR('R', BASE_VIDIOC_PRIVATE + 6, struct vpfe_rsz_config)

/*
                             
 */
#define VPFE_ISIF_CID_CRGAIN		(V4L2_CID_USER_BASE | 0xa001)
#define VPFE_ISIF_CID_CGRGAIN		(V4L2_CID_USER_BASE | 0xa002)
#define VPFE_ISIF_CID_CGBGAIN		(V4L2_CID_USER_BASE | 0xa003)
#define VPFE_ISIF_CID_CBGAIN		(V4L2_CID_USER_BASE | 0xa004)
#define VPFE_ISIF_CID_GAIN_OFFSET	(V4L2_CID_USER_BASE | 0xa005)

/*
                                         
 */
#define VPFE_CID_DPCM_PREDICTOR		(V4L2_CID_USER_BASE | 0xa006)

/*                                                                       
                                          
                                                                       */

/* 
                                     
 */
enum vpfe_isif_vdfc_corr_mode {
	/*                                                          */
	VPFE_ISIF_VDFC_NORMAL,
	/* 
                                                                      
                      
  */
	VPFE_ISIF_VDFC_HORZ_INTERPOL_IF_SAT,
	/*                                            */
	VPFE_ISIF_VDFC_HORZ_INTERPOL
};

/* 
                                                   
 */
#define VPFE_ISIF_VDFC_TABLE_SIZE	8

/* 
                                                    
 */
enum vpfe_isif_vdfc_shift {
	/*          */
	VPFE_ISIF_VDFC_NO_SHIFT,
	/*                */
	VPFE_ISIF_VDFC_SHIFT_1,
	/*                */
	VPFE_ISIF_VDFC_SHIFT_2,
	/*                */
	VPFE_ISIF_VDFC_SHIFT_3,
	/*                */
	VPFE_ISIF_VDFC_SHIFT_4
};

/* 
                                      
 */
struct vpfe_isif_vdfc_entry {
	/*                             */
	unsigned short pos_vert;
	/*                               */
	unsigned short pos_horz;
	/* 
                                                                     
                                        
  */
	unsigned char level_at_pos;
	/* 
                                                                   
                                    
  */
	unsigned char level_up_pixels;
	/* 
                                                                   
                                    
  */
	unsigned char level_low_pixels;
};

/* 
                                                  
 */
struct vpfe_isif_dfc {
	/*                                   */
	unsigned char en;
	/*                    */
	enum vpfe_isif_vdfc_corr_mode corr_mode;
	/* 
                                     
                                
  */
	unsigned char corr_whole_line;
	/* 
                                                            
                                                       
  */
	enum vpfe_isif_vdfc_shift def_level_shift;
	/*                         */
	unsigned short def_sat_level;
	/*                                                              */
	short num_vdefects;
	/*                */
	struct vpfe_isif_vdfc_entry table[VPFE_ISIF_VDFC_TABLE_SIZE];
};

/*                                                                       
                                                 
                                                                       */
/* 
                               
 */
enum vpfe_isif_horz_bc_mode {
	/* 
                                                  
                       
  */
	VPFE_ISIF_HORZ_BC_DISABLE,
	/* 
                                                       
                                                   
  */
	VPFE_ISIF_HORZ_BC_CLAMP_CALC_ENABLED,
	/* 
                                                         
                                                           
                                                             
                                  
  */
	VPFE_ISIF_HORZ_BC_CLAMP_NOT_UPDATED
};

/* 
                                                                
 */
enum vpfe_isif_horz_bc_base_win_sel {
	/*                                            */
	VPFE_ISIF_SEL_MOST_LEFT_WIN,

	/*                                             */
	VPFE_ISIF_SEL_MOST_RIGHT_WIN,
};

/*                                                          */
enum vpfe_isif_horz_bc_sz_h {
	VPFE_ISIF_HORZ_BC_SZ_H_2PIXELS,
	VPFE_ISIF_HORZ_BC_SZ_H_4PIXELS,
	VPFE_ISIF_HORZ_BC_SZ_H_8PIXELS,
	VPFE_ISIF_HORZ_BC_SZ_H_16PIXELS
};

/*                                                     */
enum vpfe_isif_horz_bc_sz_v {
	VPFE_ISIF_HORZ_BC_SZ_H_32PIXELS,
	VPFE_ISIF_HORZ_BC_SZ_H_64PIXELS,
	VPFE_ISIF_HORZ_BC_SZ_H_128PIXELS,
	VPFE_ISIF_HORZ_BC_SZ_H_256PIXELS
};

/* 
                                                     
 */
struct vpfe_isif_horz_bclamp {
	/*                       */
	enum vpfe_isif_horz_bc_mode mode;
	/* 
                             
                       
                                    
  */
	unsigned char clamp_pix_limit;
	/* 
                                                  
               
  */
	enum vpfe_isif_horz_bc_base_win_sel base_win_sel_calc;
	/*                                                    */
	unsigned char win_count_calc;
	/*                                                              */
	unsigned short win_start_h_calc;
	/*                                                           */
	unsigned short win_start_v_calc;
	/*                                                      */
	enum vpfe_isif_horz_bc_sz_h win_h_sz_calc;
	/*                                                       */
	enum vpfe_isif_horz_bc_sz_v win_v_sz_calc;
};

/* 
                                    
 */
enum vpfe_isif_vert_bc_reset_val_sel {
	/*                                                */
	VPFE_ISIF_VERT_BC_USE_HORZ_CLAMP_VAL,
	/*                                                */
	VPFE_ISIF_VERT_BC_USE_CONFIG_CLAMP_VAL,
	/*                                         */
	VPFE_ISIF_VERT_BC_NO_UPDATE
};

enum vpfe_isif_vert_bc_sz_h {
	VPFE_ISIF_VERT_BC_SZ_H_2PIXELS,
	VPFE_ISIF_VERT_BC_SZ_H_4PIXELS,
	VPFE_ISIF_VERT_BC_SZ_H_8PIXELS,
	VPFE_ISIF_VERT_BC_SZ_H_16PIXELS,
	VPFE_ISIF_VERT_BC_SZ_H_32PIXELS,
	VPFE_ISIF_VERT_BC_SZ_H_64PIXELS
};

/* 
                                                          
 */
struct vpfe_isif_vert_bclamp {
	/*                                                      */
	enum vpfe_isif_vert_bc_reset_val_sel reset_val_sel;
	/*                                                            */
	unsigned short reset_clamp_val;
	/* 
                                                         
               
  */
	unsigned char line_ave_coef;
	/*                                                                   */
	enum vpfe_isif_vert_bc_sz_h ob_h_sz_calc;
	/*                                                    */
	unsigned short ob_v_sz_calc;
	/*                                                            */
	unsigned short ob_start_h;
	/*                                                         */
	unsigned short ob_start_v;
};

/* 
                                                 
 */
struct vpfe_isif_black_clamp {
	/* 
                                                        
                      
  */
	unsigned short dc_offset;
	/* 
                                                     
                       
  */
	unsigned char en;
	/* 
                                                      
                                                 
                                                        
  */
	unsigned char bc_mode_color;
	/*                                            */
	unsigned short vert_start_sub;
	/*                                      */
	struct vpfe_isif_horz_bclamp horz;
	/*                                    */
	struct vpfe_isif_vert_bclamp vert;
};

/*                                                                        
                               
                                                                        */
/* 
                                            
 */
#define VPFE_ISIF_CSC_NUM_COEFF 16

struct float_8_bit {
	/*                    */
	__u8 integer;
	/*                    */
	__u8 decimal;
};

struct float_16_bit {
	/*                     */
	__u16 integer;
	/*                     */
	__u16 decimal;
};

/*                                                                        
                                     
                                                                        */
/* 
                                       
 */
struct vpfe_isif_color_space_conv {
	/*                               */
	unsigned char en;
	/* 
                                                                    
            
  */
	struct float_8_bit coeff[VPFE_ISIF_CSC_NUM_COEFF];
};

enum vpfe_isif_datasft {
	/*          */
	VPFE_ISIF_NO_SHIFT,
	/*             */
	VPFE_ISIF_1BIT_SHIFT,
	/*             */
	VPFE_ISIF_2BIT_SHIFT,
	/*             */
	VPFE_ISIF_3BIT_SHIFT,
	/*             */
	VPFE_ISIF_4BIT_SHIFT,
	/*             */
	VPFE_ISIF_5BIT_SHIFT,
	/*             */
	VPFE_ISIF_6BIT_SHIFT
};

#define VPFE_ISIF_LINEAR_TAB_SIZE		192
/*                                                                        
                            
                                                                        */
/* 
                                          
 */
struct vpfe_isif_linearize {
	/*                                         */
	unsigned char en;
	/*                     */
	enum vpfe_isif_datasft corr_shft;
	/*                             */
	struct float_16_bit scale_fact;
	/*                          */
	unsigned short table[VPFE_ISIF_LINEAR_TAB_SIZE];
};

/*                                                                        
                                     
                                                                        */
enum vpfe_isif_fmt_mode {
	VPFE_ISIF_SPLIT,
	VPFE_ISIF_COMBINE
};

enum vpfe_isif_lnum {
	VPFE_ISIF_1LINE,
	VPFE_ISIF_2LINES,
	VPFE_ISIF_3LINES,
	VPFE_ISIF_4LINES
};

enum vpfe_isif_line {
	VPFE_ISIF_1STLINE,
	VPFE_ISIF_2NDLINE,
	VPFE_ISIF_3RDLINE,
	VPFE_ISIF_4THLINE
};

struct vpfe_isif_fmtplen {
	/* 
                                                    
                                                     
                
  */
	unsigned short plen0;
	/* 
                                                    
                                                     
                
  */
	unsigned short plen1;
	/* 
                                                    
                                                     
                
  */
	unsigned short plen2;
	/* 
                                                    
                                                     
                
  */
	unsigned short plen3;
};

struct vpfe_isif_fmt_cfg {
	/*                                    */
	enum vpfe_isif_fmt_mode fmtmode;
	/*                                         */
	unsigned char ln_alter_en;
	/*                           */
	enum vpfe_isif_lnum lnum;
	/*                                */
	unsigned int addrinc;
};

struct vpfe_isif_fmt_addr_ptr {
	/*                 */
	unsigned int init_addr;
	/*                    */
	enum vpfe_isif_line out_line;
};

struct vpfe_isif_fmtpgm_ap {
	/*                         */
	unsigned char pgm_aptr;
	/*                                        */
	unsigned char pgmupdt;
};

struct vpfe_isif_data_formatter {
	/*                               */
	unsigned char en;
	/*                              */
	struct vpfe_isif_fmt_cfg cfg;
	/*                                  */
	struct vpfe_isif_fmtplen plen;
	/*                                        */
	unsigned short fmtrlen;
	/*                                                         */
	unsigned short fmthcnt;
	/*                            */
	struct vpfe_isif_fmt_addr_ptr fmtaddr_ptr[16];
	/*                        */
	unsigned char pgm_en[32];
	/*                          */
	struct vpfe_isif_fmtpgm_ap fmtpgm_ap[32];
};

struct vpfe_isif_df_csc {
	/*                                                       */
	unsigned int df_or_csc;
	/*                                           */
	struct vpfe_isif_color_space_conv csc;
	/*                                                      */
	struct vpfe_isif_data_formatter df;
	/*                                    */
	unsigned int start_pix;
	/*                                */
	unsigned int num_pixels;
	/*                         */
	unsigned int start_line;
	/*                              */
	unsigned int num_lines;
};

struct vpfe_isif_gain_offsets_adj {
	/*                                                  */
	unsigned char gain_sdram_en;
	/*                                                  */
	unsigned char gain_ipipe_en;
	/*                                                */
	unsigned char gain_h3a_en;
	/*                                                  */
	unsigned char offset_sdram_en;
	/*                                                  */
	unsigned char offset_ipipe_en;
	/*                                                */
	unsigned char offset_h3a_en;
};

struct vpfe_isif_cul {
	/*                                       */
	unsigned char hcpat_odd;
	/*                                        */
	unsigned char hcpat_even;
	/*                       */
	unsigned char vcpat;
	/*                                                   */
	unsigned char en_lpf;
};

/*                                                           */
struct vpfe_isif_raw_config {
	/*                                                      */
	struct vpfe_isif_linearize linearize;
	/*                       */
	struct vpfe_isif_df_csc df_csc;
	/*                                            */
	struct vpfe_isif_dfc dfc;
	/*                                   */
	struct vpfe_isif_black_clamp bclamp;
	/*                          */
	struct vpfe_isif_gain_offsets_adj gain_offset;
	/*         */
	struct vpfe_isif_cul culling;
	/*                                    */
	unsigned short horz_offset;
	/*                                  */
	unsigned short vert_offset;
};

/*                                                                     
                          
                                                                     */

/*                             */

/*                           */
#define VPFE_IPIPE_INPUT_CONFIG		(1 << 0)
/*                                   */
#define VPFE_IPIPE_LUTDPC		(1 << 1)
/*                                          */
#define VPFE_IPIPE_OTFDPC		(1 << 2)
/*                  */
#define VPFE_IPIPE_NF1			(1 << 3)
/*                  */
#define VPFE_IPIPE_NF2			(1 << 4)
/*                                   */
#define VPFE_IPIPE_WB			(1 << 5)
/*                             */
#define VPFE_IPIPE_RGB2RGB_1		(1 << 6)
/*                             */
#define VPFE_IPIPE_RGB2RGB_2		(1 << 7)
/*                  */
#define VPFE_IPIPE_GAMMA		(1 << 8)
/*                         */
#define VPFE_IPIPE_3D_LUT		(1 << 9)
/*                     */
#define VPFE_IPIPE_RGB2YUV		(1 << 10)
/*                           */
#define VPFE_IPIPE_YUV422_CONV		(1 << 11)
/*                  */
#define VPFE_IPIPE_YEE			(1 << 12)
/*                            */
#define VPFE_IPIPE_GIC			(1 << 13)
/*                   */
#define VPFE_IPIPE_CFA			(1 << 14)
/*                           */
#define VPFE_IPIPE_CAR			(1 << 15)
/*                         */
#define VPFE_IPIPE_CGS			(1 << 16)
/*                                        */
#define VPFE_IPIPE_GBCE			(1 << 17)

#define VPFE_IPIPE_MAX_MODULES		18

struct ipipe_float_u16 {
	unsigned short integer;
	unsigned short decimal;
};

struct ipipe_float_s16 {
	short integer;
	unsigned short decimal;
};

struct ipipe_float_u8 {
	unsigned char integer;
	unsigned char decimal;
};

/*                                              
                                                               
 */
enum vpfe_ipipe_dpc_corr_meth {
	/*                                                       */
	VPFE_IPIPE_DPC_REPL_BY_DOT = 0,
	/*                */
	VPFE_IPIPE_DPC_CL = 1,
	/*                 */
	VPFE_IPIPE_DPC_CR = 2,
	/*                          */
	VPFE_IPIPE_DPC_H_INTP = 3,
	/*                        */
	VPFE_IPIPE_DPC_V_INTP = 4,
	/*                */
	VPFE_IPIPE_DPC_CT = 5,
	/*                  */
	VPFE_IPIPE_DPC_CB = 6,
	/*                  */
	VPFE_IPIPE_DPC_2D_INTP = 7,
};

struct vpfe_ipipe_lutdpc_entry {
	/*                     */
	unsigned short horz_pos;
	/*                   */
	unsigned short vert_pos;
	enum vpfe_ipipe_dpc_corr_meth method;
};

#define VPFE_IPIPE_MAX_SIZE_DPC 256

/*                                      */
struct vpfe_ipipe_lutdpc {
	/*                         */
	unsigned char en;
	/*                                                          
                                       
  */
	unsigned char repl_white;
	/*                                                          
                                                             
  */
	unsigned short dpc_size;
	struct vpfe_ipipe_lutdpc_entry table[VPFE_IPIPE_MAX_SIZE_DPC];
};

enum vpfe_ipipe_otfdpc_det_meth {
	VPFE_IPIPE_DPC_OTF_MIN_MAX,
	VPFE_IPIPE_DPC_OTF_MIN_MAX2
};

struct vpfe_ipipe_otfdpc_thr {
	unsigned short r;
	unsigned short gr;
	unsigned short gb;
	unsigned short b;
};

enum vpfe_ipipe_otfdpc_alg {
	VPFE_IPIPE_OTFDPC_2_0,
	VPFE_IPIPE_OTFDPC_3_0
};

struct vpfe_ipipe_otfdpc_2_0_cfg {
	/*                                                               */
	struct vpfe_ipipe_otfdpc_thr det_thr;
	/*                                                                 
                                    
  */
	struct vpfe_ipipe_otfdpc_thr corr_thr;
};

struct vpfe_ipipe_otfdpc_3_0_cfg {
	/*                                                            
  */
	unsigned char act_adj_shf;
	/*                                 */
	unsigned short det_thr;
	/*                                       */
	unsigned short det_slp;
	/*                                     */
	unsigned short det_thr_min;
	/*                                     */
	unsigned short det_thr_max;
	/*                                  */
	unsigned short corr_thr;
	/*                                        */
	unsigned short corr_slp;
	/*                                      */
	unsigned short corr_thr_min;
	/*                                      */
	unsigned short corr_thr_max;
};

struct vpfe_ipipe_otfdpc {
	/*                         */
	unsigned char en;
	/*                         */
	enum vpfe_ipipe_otfdpc_det_meth det_method;
	/*                                                               
        
  */
	enum vpfe_ipipe_otfdpc_alg alg;
	union {
		/*                            */
		struct vpfe_ipipe_otfdpc_2_0_cfg dpc_2_0;
		/*                            */
		struct vpfe_ipipe_otfdpc_3_0_cfg dpc_3_0;
	} alg_cfg;
};

/*                             */
#define VPFE_IPIPE_NF_THR_TABLE_SIZE		8
/*                             */
#define VPFE_IPIPE_NF_STR_TABLE_SIZE		8

/*                                      */
enum vpfe_ipipe_nf_sampl_meth {
	/*                */
	VPFE_IPIPE_NF_BOX,
	/*              */
	VPFE_IPIPE_NF_DIAMOND
};

/*                                     */
struct vpfe_ipipe_nf {
	/*                         */
	unsigned char en;
	/*                                  */
	enum vpfe_ipipe_nf_sampl_meth gr_sample_meth;
	/*                                          
  */
	unsigned char shft_val;
	/*                             
  */
	unsigned char spread_val;
	/*                                                 
                          
  */
	unsigned char apply_lsc_gain;
	/*                        */
	unsigned short thr[VPFE_IPIPE_NF_THR_TABLE_SIZE];
	/*                        */
	unsigned char str[VPFE_IPIPE_NF_STR_TABLE_SIZE];
	/*                                  */
	unsigned short edge_det_min_thr;
	/*                                  */
	unsigned short edge_det_max_thr;
};

enum vpfe_ipipe_gic_alg {
	VPFE_IPIPE_GIC_ALG_CONST_GAIN,
	VPFE_IPIPE_GIC_ALG_ADAPT_GAIN
};

enum vpfe_ipipe_gic_thr_sel {
	VPFE_IPIPE_GIC_THR_REG,
	VPFE_IPIPE_GIC_THR_NF
};

enum vpfe_ipipe_gic_wt_fn_type {
	/*                         */
	VPFE_IPIPE_GIC_WT_FN_TYP_DIF,
	/*                              */
	VPFE_IPIPE_GIC_WT_FN_TYP_HP_VAL
};

/*                                          */
struct vpfe_ipipe_gic {
	/*                         */
	unsigned char en;
	/*                                                 */
	enum vpfe_ipipe_gic_alg gic_alg;
	/*                                                                   
  */
	unsigned short gain;
	/*                                                           */
	enum vpfe_ipipe_gic_thr_sel thr_sel;
	/*                                               */
	unsigned short thr;
	/*                                               
                                                    
          
  */
	unsigned short slope;
	/*                                                 
                                                         
  */
	unsigned char apply_lsc_gain;
	/*                                                           
                       
  */
	struct ipipe_float_u8 nf2_thr_gain;
	/*                                                             
                                    
  */
	enum vpfe_ipipe_gic_wt_fn_type wt_fn_type;
};

/*                                     */
struct vpfe_ipipe_wb {
	/*                    */
	short ofst_r;
	/*                     */
	short ofst_gr;
	/*                     */
	short ofst_gb;
	/*                    */
	short ofst_b;
	/*                      */
	struct ipipe_float_u16 gain_r;
	/*                     */
	struct ipipe_float_u16 gain_gr;
	/*                     */
	struct ipipe_float_u16 gain_gb;
	/*                       */
	struct ipipe_float_u16 gain_b;
};

enum vpfe_ipipe_cfa_alg {
	/*                     */
	VPFE_IPIPE_CFA_ALG_2DIRAC,
	/*                                                  */
	VPFE_IPIPE_CFA_ALG_2DIRAC_DAA,
	/*                  */
	VPFE_IPIPE_CFA_ALG_DAA
};

/*                                 */
struct vpfe_ipipe_cfa {
	/*                        */
	enum vpfe_ipipe_cfa_alg alg;
	/*                                 */
	unsigned short hpf_thr_2dir;
	/*                         */
	unsigned short hpf_slp_2dir;
	/*                           */
	unsigned short hp_mix_thr_2dir;
	/*                       */
	unsigned short hp_mix_slope_2dir;
	/*                          */
	unsigned short dir_thr_2dir;
	/*                      */
	unsigned short dir_slope_2dir;
	/*                             */
	unsigned short nd_wt_2dir;
	/*                       */
	unsigned short hue_fract_daa;
	/*                         */
	unsigned short edge_thr_daa;
	/*                            */
	unsigned short thr_min_daa;
	/*                          */
	unsigned short thr_slope_daa;
	/*                        */
	unsigned short slope_min_daa;
	/*                      */
	unsigned short slope_slope_daa;
	/*                   */
	unsigned short lp_wt_daa;
};

/*                                                */
struct vpfe_ipipe_rgb2rgb {
	/*                                                                 */
	struct ipipe_float_s16 coef_rr;
	/*                                       */
	struct ipipe_float_s16 coef_gr;
	/*                                       */
	struct ipipe_float_s16 coef_br;
	/*                                       */
	struct ipipe_float_s16 coef_rg;
	/*                                       */
	struct ipipe_float_s16 coef_gg;
	/*                                       */
	struct ipipe_float_s16 coef_bg;
	/*                                       */
	struct ipipe_float_s16 coef_rb;
	/*                                       */
	struct ipipe_float_s16 coef_gb;
	/*                                       */
	struct ipipe_float_s16 coef_bb;
	/*                             */
	int out_ofst_r;
	/*                             */
	int out_ofst_g;
	/*                             */
	int out_ofst_b;
};

#define VPFE_IPIPE_MAX_SIZE_GAMMA		512

enum vpfe_ipipe_gamma_tbl_size {
	VPFE_IPIPE_GAMMA_TBL_SZ_64 = 64,
	VPFE_IPIPE_GAMMA_TBL_SZ_128 = 128,
	VPFE_IPIPE_GAMMA_TBL_SZ_256 = 256,
	VPFE_IPIPE_GAMMA_TBL_SZ_512 = 512,
};

enum vpfe_ipipe_gamma_tbl_sel {
	VPFE_IPIPE_GAMMA_TBL_RAM = 0,
	VPFE_IPIPE_GAMMA_TBL_ROM = 1,
};

struct vpfe_ipipe_gamma_entry {
	/*              */
	short slope;
	/*               */
	unsigned short offset;
};

/*                                                   */
struct vpfe_ipipe_gamma {
	/*                                    
                                                      
  */
	unsigned char bypass_r;
	/*                                     
                                                      
  */
	unsigned char bypass_b;
	/*                                      
                                                      
  */
	unsigned char bypass_g;
	/*                                            */
	enum vpfe_ipipe_gamma_tbl_sel tbl_sel;
	/*                                
  */
	enum vpfe_ipipe_gamma_tbl_size tbl_size;
	/*         */
	struct vpfe_ipipe_gamma_entry table_r[VPFE_IPIPE_MAX_SIZE_GAMMA];
	/*            */
	struct vpfe_ipipe_gamma_entry table_b[VPFE_IPIPE_MAX_SIZE_GAMMA];
	/*             */
	struct vpfe_ipipe_gamma_entry table_g[VPFE_IPIPE_MAX_SIZE_GAMMA];
};

#define VPFE_IPIPE_MAX_SIZE_3D_LUT		729

struct vpfe_ipipe_3d_lut_entry {
	/*                      */
	unsigned short r;
	/*                        */
	unsigned short g;
	/*                       */
	unsigned short b;
};

/*                      */
struct vpfe_ipipe_3d_lut {
	/*                       */
	unsigned char en;
	/*                      */
	struct vpfe_ipipe_3d_lut_entry table[VPFE_IPIPE_MAX_SIZE_3D_LUT];
};

/*                                         */
struct vpfe_ipipe_rgb2yuv {
	/*                                 */
	struct ipipe_float_s16 coef_ry;
	/*                                 */
	struct ipipe_float_s16 coef_gy;
	/*                                 */
	struct ipipe_float_s16 coef_by;
	/*                                  */
	struct ipipe_float_s16 coef_rcb;
	/*                                  */
	struct ipipe_float_s16 coef_gcb;
	/*                                  */
	struct ipipe_float_s16 coef_bcb;
	/*                                  */
	struct ipipe_float_s16 coef_rcr;
	/*                                  */
	struct ipipe_float_s16 coef_gcr;
	/*                                  */
	struct ipipe_float_s16 coef_bcr;
	/*                         */
	int out_ofst_y;
	/*                          */
	int out_ofst_cb;
	/*                          */
	int out_ofst_cr;
};

enum vpfe_ipipe_gbce_type {
	VPFE_IPIPE_GBCE_Y_VAL_TBL = 0,
	VPFE_IPIPE_GBCE_GAIN_TBL = 1,
};

#define VPFE_IPIPE_MAX_SIZE_GBCE_LUT		1024

/*                                              */
struct vpfe_ipipe_gbce {
	/*                     */
	unsigned char en;
	/*                               */
	enum vpfe_ipipe_gbce_type type;
	/*                                       */
	unsigned short table[VPFE_IPIPE_MAX_SIZE_GBCE_LUT];
};

/*                                                      
                                 
 */
enum vpfe_chr_pos {
	/*                                         */
	VPFE_IPIPE_YUV422_CHR_POS_COSITE = 0,
	/*                                       */
	VPFE_IPIPE_YUV422_CHR_POS_CENTRE = 1,
};

/*                                                    */
struct vpfe_ipipe_yuv422_conv {
	/*                       */
	unsigned char en_chrom_lpf;
	/*                                            */
	enum vpfe_chr_pos chrom_pos;
};

#define VPFE_IPIPE_MAX_SIZE_YEE_LUT		1024

enum vpfe_ipipe_yee_merge_meth {
	VPFE_IPIPE_YEE_ABS_MAX = 0,
	VPFE_IPIPE_YEE_EE_ES = 1,
};

/*                                                       */
struct vpfe_ipipe_yee {
	/*                                     */
	unsigned char en;
	/*                                                */
	unsigned char en_halo_red;
	/*                                                      */
	enum vpfe_ipipe_yee_merge_meth merge_meth;
	/*                  */
	unsigned char hpf_shft;
	/*                         */
	short hpf_coef_00;
	/*                         */
	short hpf_coef_01;
	/*                         */
	short hpf_coef_02;
	/*                         */
	short hpf_coef_10;
	/*                         */
	short hpf_coef_11;
	/*                         */
	short hpf_coef_12;
	/*                         */
	short hpf_coef_20;
	/*                         */
	short hpf_coef_21;
	/*                         */
	short hpf_coef_22;
	/*                                         */
	unsigned short yee_thr;
	/*                     */
	unsigned short es_gain;
	/*                                */
	unsigned short es_thr1;
	/*                                */
	unsigned short es_thr2;
	/*                                 */
	unsigned short es_gain_grad;
	/*                                   */
	unsigned short es_ofst_grad;
	/*                                         */
	short table[VPFE_IPIPE_MAX_SIZE_YEE_LUT];
};

enum vpfe_ipipe_car_meth {
	/*                        */
	VPFE_IPIPE_CAR_CHR_GAIN_CTRL = 0,
	/*                                        
                
  */
	VPFE_IPIPE_CAR_DYN_SWITCH = 1,
	/*               */
	VPFE_IPIPE_CAR_MED_FLTR = 2,
};

enum vpfe_ipipe_car_hpf_type {
	VPFE_IPIPE_CAR_HPF_Y = 0,
	VPFE_IPIPE_CAR_HPF_H = 1,
	VPFE_IPIPE_CAR_HPF_V = 2,
	VPFE_IPIPE_CAR_HPF_2D = 3,
	/*                                  */
	VPFE_IPIPE_CAR_HPF_2D_YEE = 4,
};

struct vpfe_ipipe_car_gain {
	/*           */
	unsigned char gain;
	/*           */
	unsigned char shft;
	/*              */
	unsigned short gain_min;
};

/*                                            */
struct vpfe_ipipe_car {
	/*                */
	unsigned char en;
	/*                                   */
	enum vpfe_ipipe_car_meth meth;
	/*                                               */
	struct vpfe_ipipe_car_gain gain1;
	/*                                               */
	struct vpfe_ipipe_car_gain gain2;
	/*                       */
	enum vpfe_ipipe_car_hpf_type hpf;
	/*                                          */
	unsigned char hpf_thr;
	/*                                  */
	unsigned char hpf_shft;
	/*                                */
	unsigned char sw0;
	/*                                     */
	unsigned char sw1;
};

/*                                          */
struct vpfe_ipipe_cgs {
	/*                */
	unsigned char en;
	/*                             */
	unsigned char h_thr;
	/*                         */
	unsigned char h_slope;
	/*                                        */
	unsigned char h_shft;
	/*                                */
	unsigned char h_min;
};

/*                                                                 
                                       
 */
#define VPFE_IPIPE_MAX_INPUT_WIDTH	2600

struct vpfe_ipipe_input_config {
	unsigned int vst;
	unsigned int hst;
};

/* 
                                                               
                                                               
                                                                
                                                  
                                                       
                                               
                                               
                                                  
                                                 
                                                               
                                                           
                                                           
                                      
                                         
                                                           
                                                                      
                                                             
                                                
                                                               
                                                             
 */
struct vpfe_ipipe_config {
	__u32 flag;
	struct vpfe_ipipe_input_config __user *input_config;
	struct vpfe_ipipe_lutdpc __user *lutdpc;
	struct vpfe_ipipe_otfdpc __user *otfdpc;
	struct vpfe_ipipe_nf __user *nf1;
	struct vpfe_ipipe_nf __user *nf2;
	struct vpfe_ipipe_gic __user *gic;
	struct vpfe_ipipe_wb __user *wbal;
	struct vpfe_ipipe_cfa __user *cfa;
	struct vpfe_ipipe_rgb2rgb __user *rgb2rgb1;
	struct vpfe_ipipe_rgb2rgb __user *rgb2rgb2;
	struct vpfe_ipipe_gamma __user *gamma;
	struct vpfe_ipipe_3d_lut __user *lut;
	struct vpfe_ipipe_rgb2yuv __user *rgb2yuv;
	struct vpfe_ipipe_gbce __user *gbce;
	struct vpfe_ipipe_yuv422_conv __user *yuv422_conv;
	struct vpfe_ipipe_yee __user *yee;
	struct vpfe_ipipe_car __user *car;
	struct vpfe_ipipe_cgs __user *cgs;
};

/*                                                                  
                          
                                                                  */
/*                                                 */
enum vpfe_rsz_intp_t {
	VPFE_RSZ_INTP_CUBIC,
	VPFE_RSZ_INTP_LINEAR
};

/*                                    */
enum vpfe_rsz_h_lpf_lse_t {
	VPFE_RSZ_H_LPF_LSE_INTERN,
	VPFE_RSZ_H_LPF_LSE_USER_VAL
};

enum vpfe_rsz_down_scale_ave_sz {
	VPFE_IPIPE_DWN_SCALE_1_OVER_2,
	VPFE_IPIPE_DWN_SCALE_1_OVER_4,
	VPFE_IPIPE_DWN_SCALE_1_OVER_8,
	VPFE_IPIPE_DWN_SCALE_1_OVER_16,
	VPFE_IPIPE_DWN_SCALE_1_OVER_32,
	VPFE_IPIPE_DWN_SCALE_1_OVER_64,
	VPFE_IPIPE_DWN_SCALE_1_OVER_128,
	VPFE_IPIPE_DWN_SCALE_1_OVER_256
};

struct vpfe_rsz_output_spec {
	/*                        */
	unsigned char h_flip;
	/*                      */
	unsigned char v_flip;
	/*                          */
	unsigned int vst_y;
	/*                                       */
	unsigned int vst_c;
	/*                                                         */
	enum vpfe_rsz_intp_t v_typ_y;
	/*                                                     */
	enum vpfe_rsz_intp_t v_typ_c;
	/*                                    */
	unsigned char v_lpf_int_y;
	/*                                      */
	unsigned char v_lpf_int_c;
	/*                                                             */
	enum vpfe_rsz_intp_t h_typ_y;
	/*                                                     */
	enum vpfe_rsz_intp_t h_typ_c;
	/*                                      */
	unsigned char h_lpf_int_y;
	/*                                        */
	unsigned char h_lpf_int_c;
	/*                                    */
	unsigned char en_down_scale;
	/*                                                                 
                                                                
               
  */
	enum vpfe_rsz_down_scale_ave_sz h_dscale_ave_sz;
	/*                                                               
                                                                
               
  */
	enum vpfe_rsz_down_scale_ave_sz v_dscale_ave_sz;
	/*                                                                
  */
	unsigned int user_y_ofst;
	/*                                                                
  */
	unsigned int user_c_ofst;
};

struct vpfe_rsz_config_params {
	unsigned int vst;
	/*                                       
                 
  */
	unsigned int hst;
	/*                                                               
  */
	struct vpfe_rsz_output_spec output1;
	/*                                                               
  */
	struct vpfe_rsz_output_spec output2;
	/*                                                */
	unsigned char chroma_sample_even;
	unsigned char frame_div_mode_en;
	unsigned char yuv_y_min;
	unsigned char yuv_y_max;
	unsigned char yuv_c_min;
	unsigned char yuv_c_max;
	enum vpfe_chr_pos out_chr_pos;
	unsigned char bypass;
};

/*                                                   */
struct vpfe_rsz_config {
	struct vpfe_rsz_config_params *config;
};

#endif		/*                      */
