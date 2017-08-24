/**************************************************************************
 *
 * Copyright © 2008-2012 VMware, Inc., Palo Alto, CA., USA
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * THE COPYRIGHT HOLDERS, AUTHORS AND/OR ITS SUPPLIERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 **************************************************************************/

#ifdef __KERNEL__

#include <drm/vmwgfx_drm.h>
#define surf_size_struct struct drm_vmw_size

#else /*            */

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(_A) (sizeof(_A) / sizeof((_A)[0]))
#endif /*            */

#define DIV_ROUND_UP(x, y)  (((x) + (y) - 1) / (y))
#define max_t(type, x, y)  ((x) > (y) ? (x) : (y))
#define surf_size_struct SVGA3dSize
#define u32 uint32

#endif /*            */

#include "svga3d_reg.h"

/*
                                                                        
  
                                                        
                                                                          
                                                                    
                                                         
                                                           
  
                                                                  
                                                                           
                                                                        
                                                                          
                                    
                                                                           
  
                                                                      
 */

enum svga3d_block_desc {
	SVGA3DBLOCKDESC_NONE        = 0,         /*                        */
	SVGA3DBLOCKDESC_BLUE        = 1 << 0,    /*                       
               */
	SVGA3DBLOCKDESC_U           = 1 << 0,    /*                          
               */
	SVGA3DBLOCKDESC_UV_VIDEO    = 1 << 7,    /*                             
                  */
	SVGA3DBLOCKDESC_GREEN       = 1 << 1,    /*                         
               */
	SVGA3DBLOCKDESC_V           = 1 << 1,    /*                          
               */
	SVGA3DBLOCKDESC_STENCIL     = 1 << 1,    /*                     
                  */
	SVGA3DBLOCKDESC_RED         = 1 << 2,    /*                        
               */
	SVGA3DBLOCKDESC_W           = 1 << 2,    /*                          
               */
	SVGA3DBLOCKDESC_LUMINANCE   = 1 << 2,    /*                             
               */
	SVGA3DBLOCKDESC_Y           = 1 << 2,    /*                           
               */
	SVGA3DBLOCKDESC_DEPTH       = 1 << 2,    /*                          */
	SVGA3DBLOCKDESC_ALPHA       = 1 << 3,    /*                    
                  */
	SVGA3DBLOCKDESC_Q           = 1 << 3,    /*                          
               */
	SVGA3DBLOCKDESC_BUFFER      = 1 << 4,    /*                       
               */
	SVGA3DBLOCKDESC_COMPRESSED  = 1 << 5,    /*                        
                               
                                  */
	SVGA3DBLOCKDESC_IEEE_FP     = 1 << 6,    /*                             
                        
                           
                       */
	SVGA3DBLOCKDESC_PLANAR_YUV  = 1 << 8,    /*                            
                */
	SVGA3DBLOCKDESC_U_VIDEO     = 1 << 9,    /*                         */
	SVGA3DBLOCKDESC_V_VIDEO     = 1 << 10,   /*                         */
	SVGA3DBLOCKDESC_EXP         = 1 << 11,   /*                 */
	SVGA3DBLOCKDESC_SRGB        = 1 << 12,   /*                        */
	SVGA3DBLOCKDESC_2PLANAR_YUV = 1 << 13,   /*                   
                      */
	SVGA3DBLOCKDESC_3PLANAR_YUV = 1 << 14,   /*                     
                               */

	SVGA3DBLOCKDESC_RG         = SVGA3DBLOCKDESC_RED |
	SVGA3DBLOCKDESC_GREEN,
	SVGA3DBLOCKDESC_RGB        = SVGA3DBLOCKDESC_RG |
	SVGA3DBLOCKDESC_BLUE,
	SVGA3DBLOCKDESC_RGB_SRGB   = SVGA3DBLOCKDESC_RGB |
	SVGA3DBLOCKDESC_SRGB,
	SVGA3DBLOCKDESC_RGBA       = SVGA3DBLOCKDESC_RGB |
	SVGA3DBLOCKDESC_ALPHA,
	SVGA3DBLOCKDESC_RGBA_SRGB  = SVGA3DBLOCKDESC_RGBA |
	SVGA3DBLOCKDESC_SRGB,
	SVGA3DBLOCKDESC_UV         = SVGA3DBLOCKDESC_U |
	SVGA3DBLOCKDESC_V,
	SVGA3DBLOCKDESC_UVL        = SVGA3DBLOCKDESC_UV |
	SVGA3DBLOCKDESC_LUMINANCE,
	SVGA3DBLOCKDESC_UVW        = SVGA3DBLOCKDESC_UV |
	SVGA3DBLOCKDESC_W,
	SVGA3DBLOCKDESC_UVWA       = SVGA3DBLOCKDESC_UVW |
	SVGA3DBLOCKDESC_ALPHA,
	SVGA3DBLOCKDESC_UVWQ       = SVGA3DBLOCKDESC_U |
	SVGA3DBLOCKDESC_V |
	SVGA3DBLOCKDESC_W |
	SVGA3DBLOCKDESC_Q,
	SVGA3DBLOCKDESC_LA         = SVGA3DBLOCKDESC_LUMINANCE |
	SVGA3DBLOCKDESC_ALPHA,
	SVGA3DBLOCKDESC_R_FP       = SVGA3DBLOCKDESC_RED |
	SVGA3DBLOCKDESC_IEEE_FP,
	SVGA3DBLOCKDESC_RG_FP      = SVGA3DBLOCKDESC_R_FP |
	SVGA3DBLOCKDESC_GREEN,
	SVGA3DBLOCKDESC_RGB_FP     = SVGA3DBLOCKDESC_RG_FP |
	SVGA3DBLOCKDESC_BLUE,
	SVGA3DBLOCKDESC_RGBA_FP    = SVGA3DBLOCKDESC_RGB_FP |
	SVGA3DBLOCKDESC_ALPHA,
	SVGA3DBLOCKDESC_DS         = SVGA3DBLOCKDESC_DEPTH |
	SVGA3DBLOCKDESC_STENCIL,
	SVGA3DBLOCKDESC_YUV        = SVGA3DBLOCKDESC_UV_VIDEO |
	SVGA3DBLOCKDESC_Y,
	SVGA3DBLOCKDESC_AYUV       = SVGA3DBLOCKDESC_ALPHA |
	SVGA3DBLOCKDESC_Y |
	SVGA3DBLOCKDESC_U_VIDEO |
	SVGA3DBLOCKDESC_V_VIDEO,
	SVGA3DBLOCKDESC_RGBE       = SVGA3DBLOCKDESC_RGB |
	SVGA3DBLOCKDESC_EXP,
	SVGA3DBLOCKDESC_COMPRESSED_SRGB = SVGA3DBLOCKDESC_COMPRESSED |
	SVGA3DBLOCKDESC_SRGB,
	SVGA3DBLOCKDESC_NV12       = SVGA3DBLOCKDESC_PLANAR_YUV |
	SVGA3DBLOCKDESC_2PLANAR_YUV,
	SVGA3DBLOCKDESC_YV12       = SVGA3DBLOCKDESC_PLANAR_YUV |
	SVGA3DBLOCKDESC_3PLANAR_YUV,
};

/*
                                                     
  
                                                     
                           
                                              
                                
                                     
                                                      
 */
#define SVGA3D_CHANNEL_DEF(type)		\
	struct {				\
		union {				\
			type blue;              \
			type u;                 \
			type uv_video;          \
			type u_video;           \
		};				\
		union {				\
			type green;             \
			type v;                 \
			type stencil;           \
			type v_video;           \
		};				\
		union {				\
			type red;               \
			type w;                 \
			type luminance;         \
			type y;                 \
			type depth;             \
			type data;              \
		};				\
		union {				\
			type alpha;             \
			type q;                 \
			type exp;               \
		};				\
	}

struct svga3d_surface_desc {
	enum svga3d_block_desc block_desc;
	surf_size_struct block_size;
	u32 bytes_per_block;
	u32 pitch_bytes_per_block;

	struct {
		u32 total;
		SVGA3D_CHANNEL_DEF(uint8);
	} bit_depth;

	struct {
		SVGA3D_CHANNEL_DEF(uint8);
	} bit_offset;
};

static const struct svga3d_surface_desc svga3d_surface_descs[] = {
	{SVGA3DBLOCKDESC_NONE,
	 {1, 1, 1},  0, 0, {0, {{0}, {0}, {0}, {0} } },
	 {{{0}, {0}, {0}, {0} } } },     /*                       */

	{SVGA3DBLOCKDESC_RGB,
	 {1, 1, 1},  4, 4, {24, {{8}, {8}, {8}, {0} } },
	 {{{0}, {8}, {16}, {24} } } },   /*                 */

	{SVGA3DBLOCKDESC_RGBA,
	 {1, 1, 1},  4, 4, {32, {{8}, {8}, {8}, {8} } },
	 {{{0}, {8}, {16}, {24} } } },   /*                 */

	{SVGA3DBLOCKDESC_RGB,
	 {1, 1, 1},  2, 2, {16, {{5}, {6}, {5}, {0} } },
	 {{{0}, {5}, {11}, {0} } } },    /*               */

	{SVGA3DBLOCKDESC_RGB,
	 {1, 1, 1},  2, 2, {15, {{5}, {5}, {5}, {0} } },
	 {{{0}, {5}, {10}, {0} } } },    /*                 */

	{SVGA3DBLOCKDESC_RGBA,
	 {1, 1, 1},  2, 2, {16, {{5}, {5}, {5}, {1} } },
	 {{{0}, {5}, {10}, {15} } } },   /*                 */

	{SVGA3DBLOCKDESC_RGBA,
	 {1, 1, 1},  2, 2, {16, {{4}, {4}, {4}, {4} } },
	 {{{0}, {4}, {8}, {12} } } },    /*                 */

	{SVGA3DBLOCKDESC_DEPTH,
	 {1, 1, 1},  4, 4, {32, {{0}, {0}, {32}, {0} } },
	 {{{0}, {0}, {0}, {0} } } },     /*              */

	{SVGA3DBLOCKDESC_DEPTH,
	 {1, 1, 1},  2, 2, {16, {{0}, {0}, {16}, {0} } },
	 {{{0}, {0}, {0}, {0} } } },     /*              */

	{SVGA3DBLOCKDESC_DS,
	 {1, 1, 1},  4, 4, {32, {{0}, {8}, {24}, {0} } },
	 {{{0}, {24}, {0}, {0} } } },    /*                */

	{SVGA3DBLOCKDESC_DS,
	 {1, 1, 1},  2, 2, {16, {{0}, {1}, {15}, {0} } },
	 {{{0}, {15}, {0}, {0} } } },    /*                */

	{SVGA3DBLOCKDESC_LUMINANCE,
	 {1, 1, 1},  1, 1, {8, {{0}, {0}, {8}, {0} } },
	 {{{0}, {0}, {0}, {0} } } },     /*                   */

	{SVGA3DBLOCKDESC_LA,
	 {1, 1, 1},  1, 1, {8, {{0}, {0}, {4}, {4} } },
	 {{{0}, {0}, {0}, {4} } } },     /*                          */

	{SVGA3DBLOCKDESC_LUMINANCE,
	 {1, 1, 1},  2, 2, {16, {{0}, {0}, {16}, {0} } },
	 {{{0}, {0}, {0}, {0} } } },     /*                    */

	{SVGA3DBLOCKDESC_LA,
	 {1, 1, 1},  2, 2, {16, {{0}, {0}, {8}, {8} } },
	 {{{0}, {0}, {0}, {8} } } },     /*                          */

	{SVGA3DBLOCKDESC_COMPRESSED,
	 {4, 4, 1},  8, 8, {64, {{0}, {0}, {64}, {0} } },
	 {{{0}, {0}, {0}, {0} } } },     /*             */

	{SVGA3DBLOCKDESC_COMPRESSED,
	 {4, 4, 1},  16, 16, {128, {{0}, {0}, {128}, {0} } },
	 {{{0}, {0}, {0}, {0} } } },     /*             */

	{SVGA3DBLOCKDESC_COMPRESSED,
	 {4, 4, 1},  16, 16, {128, {{0}, {0}, {128}, {0} } },
	 {{{0}, {0}, {0}, {0} } } },     /*             */

	{SVGA3DBLOCKDESC_COMPRESSED,
	 {4, 4, 1},  16, 16, {128, {{0}, {0}, {128}, {0} } },
	 {{{0}, {0}, {0}, {0} } } },     /*             */

	{SVGA3DBLOCKDESC_COMPRESSED,
	 {4, 4, 1},  16, 16, {128, {{0}, {0}, {128}, {0} } },
	 {{{0}, {0}, {0}, {0} } } },     /*             */

	{SVGA3DBLOCKDESC_UV,
	 {1, 1, 1},  2, 2, {16, {{0}, {0}, {8}, {8} } },
	 {{{0}, {0}, {0}, {8} } } },     /*                 */

	{SVGA3DBLOCKDESC_UVL,
	 {1, 1, 1},  2, 2, {16, {{5}, {5}, {6}, {0} } },
	 {{{11}, {6}, {0}, {0} } } },    /*                   */

	{SVGA3DBLOCKDESC_UVL,
	 {1, 1, 1},  4, 4, {32, {{8}, {8}, {8}, {0} } },
	 {{{16}, {8}, {0}, {0} } } },    /*                     */

	{SVGA3DBLOCKDESC_UVL,
	 {1, 1, 1},  3, 3, {24, {{8}, {8}, {8}, {0} } },
	 {{{16}, {8}, {0}, {0} } } },    /*                   */

	{SVGA3DBLOCKDESC_RGBA_FP,
	 {1, 1, 1},  8, 8, {64, {{16}, {16}, {16}, {16} } },
	 {{{32}, {16}, {0}, {48} } } },  /*                   */

	{SVGA3DBLOCKDESC_RGBA_FP,
	 {1, 1, 1},  16, 16, {128, {{32}, {32}, {32}, {32} } },
	 {{{64}, {32}, {0}, {96} } } },  /*                   */

	{SVGA3DBLOCKDESC_RGBA,
	 {1, 1, 1},  4, 4, {32, {{10}, {10}, {10}, {2} } },
	 {{{0}, {10}, {20}, {30} } } },  /*                    */

	{SVGA3DBLOCKDESC_UV,
	 {1, 1, 1},  2, 2, {16, {{8}, {8}, {0}, {0} } },
	 {{{8}, {0}, {0}, {0} } } },     /*             */

	{SVGA3DBLOCKDESC_UVWQ,
	 {1, 1, 1},  4, 4, {32, {{8}, {8}, {8}, {8} } },
	 {{{24}, {16}, {8}, {0} } } },   /*                 */

	{SVGA3DBLOCKDESC_UV,
	 {1, 1, 1},  2, 2, {16, {{8}, {8}, {0}, {0} } },
	 {{{8}, {0}, {0}, {0} } } },     /*               */

	{SVGA3DBLOCKDESC_UVL,
	 {1, 1, 1},  4, 4, {24, {{8}, {8}, {8}, {0} } },
	 {{{16}, {8}, {0}, {0} } } },    /*                 */

	{SVGA3DBLOCKDESC_UVWA,
	 {1, 1, 1},  4, 4, {32, {{10}, {10}, {10}, {2} } },
	 {{{0}, {10}, {20}, {30} } } },  /*                    */

	{SVGA3DBLOCKDESC_ALPHA,
	 {1, 1, 1},  1, 1, {8, {{0}, {0}, {0}, {8} } },
	 {{{0}, {0}, {0}, {0} } } },     /*               */

	{SVGA3DBLOCKDESC_R_FP,
	 {1, 1, 1},  2, 2, {16, {{0}, {0}, {16}, {0} } },
	 {{{0}, {0}, {0}, {0} } } },     /*                */

	{SVGA3DBLOCKDESC_R_FP,
	 {1, 1, 1},  4, 4, {32, {{0}, {0}, {32}, {0} } },
	 {{{0}, {0}, {0}, {0} } } },     /*                */

	{SVGA3DBLOCKDESC_RG_FP,
	 {1, 1, 1},  4, 4, {32, {{0}, {16}, {16}, {0} } },
	 {{{0}, {16}, {0}, {0} } } },    /*                 */

	{SVGA3DBLOCKDESC_RG_FP,
	 {1, 1, 1},  8, 8, {64, {{0}, {32}, {32}, {0} } },
	 {{{0}, {32}, {0}, {0} } } },    /*                 */

	{SVGA3DBLOCKDESC_BUFFER,
	 {1, 1, 1},  1, 1, {8, {{0}, {0}, {8}, {0} } },
	 {{{0}, {0}, {0}, {0} } } },     /*               */

	{SVGA3DBLOCKDESC_DEPTH,
	 {1, 1, 1},  4, 4, {32, {{0}, {0}, {24}, {0} } },
	 {{{0}, {24}, {0}, {0} } } },    /*                */

	{SVGA3DBLOCKDESC_UV,
	 {1, 1, 1},  4, 4, {32, {{16}, {16}, {0}, {0} } },
	 {{{16}, {0}, {0}, {0} } } },    /*               */

	{SVGA3DBLOCKDESC_RG,
	 {1, 1, 1},  4, 4, {32, {{0}, {16}, {16}, {0} } },
	 {{{0}, {0}, {16}, {0} } } },    /*               */

	{SVGA3DBLOCKDESC_RGBA,
	 {1, 1, 1},  8, 8, {64, {{16}, {16}, {16}, {16} } },
	 {{{32}, {16}, {0}, {48} } } },  /*                     */

	{SVGA3DBLOCKDESC_YUV,
	 {1, 1, 1},  2, 2, {16, {{8}, {0}, {8}, {0} } },
	 {{{0}, {0}, {8}, {0} } } },     /*             */

	{SVGA3DBLOCKDESC_YUV,
	 {1, 1, 1},  2, 2, {16, {{8}, {0}, {8}, {0} } },
	 {{{8}, {0}, {0}, {0} } } },     /*             */

	{SVGA3DBLOCKDESC_NV12,
	 {2, 2, 1},  6, 2, {48, {{0}, {0}, {48}, {0} } },
	 {{{0}, {0}, {0}, {0} } } },     /*             */

	{SVGA3DBLOCKDESC_AYUV,
	 {1, 1, 1},  4, 4, {32, {{8}, {8}, {8}, {8} } },
	 {{{0}, {8}, {16}, {24} } } },   /*             */

	{SVGA3DBLOCKDESC_RGBA,
	 {1, 1, 1},  16, 16, {128, {{32}, {32}, {32}, {32} } },
	 {{{64}, {32}, {0}, {96} } } },  /*                              */

	{SVGA3DBLOCKDESC_RGBA,
	 {1, 1, 1},  16, 16, {128, {{32}, {32}, {32}, {32} } },
	 {{{64}, {32}, {0}, {96} } } },  /*                          */

	{SVGA3DBLOCKDESC_UVWQ,
	 {1, 1, 1},  16, 16, {128, {{32}, {32}, {32}, {32} } },
	 {{{64}, {32}, {0}, {96} } } },  /*                          */

	{SVGA3DBLOCKDESC_RGB,
	 {1, 1, 1},  12, 12, {96, {{32}, {32}, {32}, {0} } },
	 {{{64}, {32}, {0}, {0} } } },   /*                           */

	{SVGA3DBLOCKDESC_RGB_FP,
	 {1, 1, 1},  12, 12, {96, {{32}, {32}, {32}, {0} } },
	 {{{64}, {32}, {0}, {0} } } },   /*                        */

	{SVGA3DBLOCKDESC_RGB,
	 {1, 1, 1},  12, 12, {96, {{32}, {32}, {32}, {0} } },
	 {{{64}, {32}, {0}, {0} } } },   /*                       */

	{SVGA3DBLOCKDESC_UVW,
	 {1, 1, 1},  12, 12, {96, {{32}, {32}, {32}, {0} } },
	 {{{64}, {32}, {0}, {0} } } },   /*                       */

	{SVGA3DBLOCKDESC_RGBA,
	 {1, 1, 1},  8, 8, {64, {{16}, {16}, {16}, {16} } },
	 {{{32}, {16}, {0}, {48} } } },  /*                              */

	{SVGA3DBLOCKDESC_RGBA,
	 {1, 1, 1},  8, 8, {64, {{16}, {16}, {16}, {16} } },
	 {{{32}, {16}, {0}, {48} } } },  /*                          */

	{SVGA3DBLOCKDESC_UVWQ,
	 {1, 1, 1},  8, 8, {64, {{16}, {16}, {16}, {16} } },
	 {{{32}, {16}, {0}, {48} } } },  /*                           */

	{SVGA3DBLOCKDESC_UVWQ,
	 {1, 1, 1},  8, 8, {64, {{16}, {16}, {16}, {16} } },
	 {{{32}, {16}, {0}, {48} } } },  /*                          */

	{SVGA3DBLOCKDESC_RG,
	 {1, 1, 1},  8, 8, {64, {{0}, {32}, {32}, {0} } },
	 {{{0}, {32}, {0}, {0} } } },    /*                        */

	{SVGA3DBLOCKDESC_RG,
	 {1, 1, 1},  8, 8, {64, {{0}, {32}, {32}, {0} } },
	 {{{0}, {32}, {0}, {0} } } },    /*                    */

	{SVGA3DBLOCKDESC_UV,
	 {1, 1, 1},  8, 8, {64, {{0}, {32}, {32}, {0} } },
	 {{{0}, {32}, {0}, {0} } } },    /*                    */

	{SVGA3DBLOCKDESC_RG,
	 {1, 1, 1},  8, 8, {64, {{0}, {8}, {32}, {0} } },
	 {{{0}, {32}, {0}, {0} } } },    /*                          */

	{SVGA3DBLOCKDESC_DS,
	 {1, 1, 1},  8, 8, {64, {{0}, {8}, {32}, {0} } },
	 {{{0}, {32}, {0}, {0} } } },    /*                             */

	{SVGA3DBLOCKDESC_R_FP,
	 {1, 1, 1},  8, 8, {64, {{0}, {0}, {32}, {0} } },
	 {{{0}, {0}, {0}, {0} } } },    /*                                  */

	{SVGA3DBLOCKDESC_GREEN,
	 {1, 1, 1},  8, 8, {64, {{0}, {8}, {0}, {0} } },
	 {{{0}, {32}, {0}, {0} } } },    /*                                */

	{SVGA3DBLOCKDESC_RGBA,
	 {1, 1, 1},  4, 4, {32, {{10}, {10}, {10}, {2} } },
	 {{{0}, {10}, {20}, {30} } } },  /*                             */

	{SVGA3DBLOCKDESC_RGBA,
	 {1, 1, 1},  4, 4, {32, {{10}, {10}, {10}, {2} } },
	 {{{0}, {10}, {20}, {30} } } },  /*                         */

	{SVGA3DBLOCKDESC_RGB_FP,
	 {1, 1, 1},  4, 4, {32, {{10}, {11}, {11}, {0} } },
	 {{{0}, {10}, {21}, {0} } } },  /*                        */

	{SVGA3DBLOCKDESC_RGBA,
	 {1, 1, 1},  4, 4, {32, {{8}, {8}, {8}, {8} } },
	 {{{16}, {8}, {0}, {24} } } },   /*                          */

	{SVGA3DBLOCKDESC_RGBA,
	 {1, 1, 1},  4, 4, {32, {{8}, {8}, {8}, {8} } },
	 {{{16}, {8}, {0}, {24} } } },   /*                       */

	{SVGA3DBLOCKDESC_RGBA_SRGB,
	 {1, 1, 1},  4, 4, {32, {{8}, {8}, {8}, {8} } },
	 {{{16}, {8}, {0}, {24} } } },   /*                            */

	{SVGA3DBLOCKDESC_RGBA,
	 {1, 1, 1},  4, 4, {32, {{8}, {8}, {8}, {8} } },
	 {{{16}, {8}, {0}, {24} } } },   /*                      */

	{SVGA3DBLOCKDESC_RGBA,
	 {1, 1, 1},  4, 4, {32, {{8}, {8}, {8}, {8} } },
	 {{{16}, {8}, {0}, {24} } } },   /*                      */

	{SVGA3DBLOCKDESC_RG,
	 {1, 1, 1},  4, 4, {32, {{0}, {16}, {16}, {0} } },
	 {{{0}, {16}, {0}, {0} } } },    /*                        */

	{SVGA3DBLOCKDESC_RG_FP,
	 {1, 1, 1},  4, 4, {32, {{0}, {16}, {16}, {0} } },
	 {{{0}, {16}, {0}, {0} } } },    /*                    */

	{SVGA3DBLOCKDESC_UV,
	 {1, 1, 1},  4, 4, {32, {{0}, {16}, {16}, {0} } },
	 {{{0}, {16}, {0}, {0} } } },    /*                    */

	{SVGA3DBLOCKDESC_RED,
	 {1, 1, 1},  4, 4, {32, {{0}, {0}, {32}, {0} } },
	 {{{0}, {0}, {0}, {0} } } },     /*                     */

	{SVGA3DBLOCKDESC_DEPTH,
	 {1, 1, 1},  4, 4, {32, {{0}, {0}, {32}, {0} } },
	 {{{0}, {0}, {0}, {0} } } },     /*                  */

	{SVGA3DBLOCKDESC_RED,
	 {1, 1, 1},  4, 4, {32, {{0}, {0}, {32}, {0} } },
	 {{{0}, {0}, {0}, {0} } } },     /*                 */

	{SVGA3DBLOCKDESC_RED,
	 {1, 1, 1},  4, 4, {32, {{0}, {0}, {32}, {0} } },
	 {{{0}, {0}, {0}, {0} } } },     /*                 */

	{SVGA3DBLOCKDESC_RG,
	 {1, 1, 1},  4, 4, {32, {{0}, {8}, {24}, {0} } },
	 {{{0}, {24}, {0}, {0} } } },    /*                       */

	{SVGA3DBLOCKDESC_DS,
	 {1, 1, 1},  4, 4, {32, {{0}, {8}, {24}, {0} } },
	 {{{0}, {24}, {0}, {0} } } },    /*                          */

	{SVGA3DBLOCKDESC_RED,
	 {1, 1, 1},  4, 4, {32, {{0}, {0}, {24}, {0} } },
	 {{{0}, {0}, {0}, {0} } } },     /*                              */

	{SVGA3DBLOCKDESC_GREEN,
	 {1, 1, 1},  4, 4, {32, {{0}, {8}, {0}, {0} } },
	 {{{0}, {24}, {0}, {0} } } },    /*                             */

	{SVGA3DBLOCKDESC_RG,
	 {1, 1, 1},  2, 2, {16, {{0}, {8}, {8}, {0} } },
	 {{{0}, {8}, {0}, {0} } } },     /*                      */

	{SVGA3DBLOCKDESC_RG,
	 {1, 1, 1},  2, 2, {16, {{0}, {8}, {8}, {0} } },
	 {{{0}, {8}, {0}, {0} } } },     /*                   */

	{SVGA3DBLOCKDESC_RG,
	 {1, 1, 1},  2, 2, {16, {{0}, {8}, {8}, {0} } },
	 {{{0}, {8}, {0}, {0} } } },     /*                  */

	{SVGA3DBLOCKDESC_UV,
	 {1, 1, 1},  2, 2, {16, {{0}, {8}, {8}, {0} } },
	 {{{0}, {8}, {0}, {0} } } },     /*                  */

	{SVGA3DBLOCKDESC_RED,
	 {1, 1, 1},  2, 2, {16, {{0}, {0}, {16}, {0} } },
	 {{{0}, {0}, {0}, {0} } } },     /*                     */

	{SVGA3DBLOCKDESC_RED,
	 {1, 1, 1},  2, 2, {16, {{0}, {0}, {16}, {0} } },
	 {{{0}, {0}, {0}, {0} } } },     /*                  */

	{SVGA3DBLOCKDESC_RED,
	 {1, 1, 1},  2, 2, {16, {{0}, {0}, {16}, {0} } },
	 {{{0}, {0}, {0}, {0} } } },     /*                 */

	{SVGA3DBLOCKDESC_U,
	 {1, 1, 1},  2, 2, {16, {{0}, {0}, {16}, {0} } },
	 {{{0}, {0}, {0}, {0} } } },     /*                  */

	{SVGA3DBLOCKDESC_U,
	 {1, 1, 1},  2, 2, {16, {{0}, {0}, {16}, {0} } },
	 {{{0}, {0}, {0}, {0} } } },     /*                 */

	{SVGA3DBLOCKDESC_RED,
	 {1, 1, 1},  1, 1, {8, {{0}, {0}, {8}, {0} } },
	 {{{0}, {0}, {0}, {0} } } },     /*                    */

	{SVGA3DBLOCKDESC_RED,
	 {1, 1, 1},  1, 1, {8, {{0}, {0}, {8}, {0} } },
	 {{{0}, {0}, {0}, {0} } } },     /*                 */

	{SVGA3DBLOCKDESC_RED,
	 {1, 1, 1},  1, 1, {8, {{0}, {0}, {8}, {0} } },
	 {{{0}, {0}, {0}, {0} } } },     /*                */

	{SVGA3DBLOCKDESC_U,
	 {1, 1, 1},  1, 1, {8, {{0}, {0}, {8}, {0} } },
	 {{{0}, {0}, {0}, {0} } } },     /*                 */

	{SVGA3DBLOCKDESC_U,
	 {1, 1, 1},  1, 1, {8, {{0}, {0}, {8}, {0} } },
	 {{{0}, {0}, {0}, {0} } } },     /*                */

	{SVGA3DBLOCKDESC_RED,
	 {8, 1, 1},  1, 1, {8, {{0}, {0}, {8}, {0} } },
	 {{{0}, {0}, {0}, {0} } } },     /*                 */

	{SVGA3DBLOCKDESC_RGBE,
	 {1, 1, 1},  4, 4, {32, {{9}, {9}, {9}, {5} } },
	 {{{18}, {9}, {0}, {27} } } },   /*                           */

	{SVGA3DBLOCKDESC_RG,
	 {1, 1, 1},  2, 2, {16, {{0}, {8}, {8}, {0} } },
	 {{{0}, {8}, {0}, {0} } } },     /*                        */

	{SVGA3DBLOCKDESC_RG,
	 {1, 1, 1},  2, 2, {16, {{0}, {8}, {8}, {0} } },
	 {{{0}, {8}, {0}, {0} } } },     /*                        */

	{SVGA3DBLOCKDESC_COMPRESSED,
	 {4, 4, 1},  8, 8, {64, {{0}, {0}, {64}, {0} } },
	 {{{0}, {0}, {0}, {0} } } },     /*                     */

	{SVGA3DBLOCKDESC_COMPRESSED_SRGB,
	 {4, 4, 1},  8, 8, {64, {{0}, {0}, {64}, {0} } },
	 {{{0}, {0}, {0}, {0} } } },     /*                       */

	{SVGA3DBLOCKDESC_COMPRESSED,
	 {4, 4, 1},  16, 16, {128, {{0}, {0}, {128}, {0} } },
	 {{{0}, {0}, {0}, {0} } } },     /*                     */

	{SVGA3DBLOCKDESC_COMPRESSED_SRGB,
	 {4, 4, 1},  16, 16, {128, {{0}, {0}, {128}, {0} } },
	 {{{0}, {0}, {0}, {0} } } },     /*                       */

	{SVGA3DBLOCKDESC_COMPRESSED,
	 {4, 4, 1},  16, 16, {128, {{0}, {0}, {128}, {0} } },
	 {{{0}, {0}, {0}, {0} } } },     /*                     */

	{SVGA3DBLOCKDESC_COMPRESSED_SRGB,
	 {4, 4, 1},  16, 16, {128, {{0}, {0}, {128}, {0} } },
	 {{{0}, {0}, {0}, {0} } } },     /*                       */

	{SVGA3DBLOCKDESC_COMPRESSED,
	 {4, 4, 1},  8, 8, {64, {{0}, {0}, {64}, {0} } },
	 {{{0}, {0}, {0}, {0} } } },     /*                     */

	{SVGA3DBLOCKDESC_COMPRESSED,
	 {4, 4, 1},  8, 8, {64, {{0}, {0}, {64}, {0} } },
	 {{{0}, {0}, {0}, {0} } } },     /*                  */

	{SVGA3DBLOCKDESC_COMPRESSED,
	 {4, 4, 1},  8, 8, {64, {{0}, {0}, {64}, {0} } },
	 {{{0}, {0}, {0}, {0} } } },     /*                  */

	{SVGA3DBLOCKDESC_COMPRESSED,
	 {4, 4, 1},  16, 16, {128, {{0}, {0}, {128}, {0} } },
	 {{{0}, {0}, {0}, {0} } } },     /*                     */

	{SVGA3DBLOCKDESC_COMPRESSED,
	 {4, 4, 1},  16, 16, {128, {{0}, {0}, {128}, {0} } },
	 {{{0}, {0}, {0}, {0} } } },     /*                  */

	{SVGA3DBLOCKDESC_COMPRESSED,
	 {4, 4, 1},  16, 16, {128, {{0}, {0}, {128}, {0} } },
	 {{{0}, {0}, {0}, {0} } } },     /*                  */

	{SVGA3DBLOCKDESC_RGBA,
	 {1, 1, 1},  4, 4, {32, {{10}, {10}, {10}, {2} } },
	 {{{0}, {10}, {20}, {30} } } },  /*                                   */

	{SVGA3DBLOCKDESC_RGBA,
	 {1, 1, 1},  4, 4, {32, {{8}, {8}, {8}, {8} } },
	 {{{0}, {8}, {16}, {24} } } },   /*                          */

	{SVGA3DBLOCKDESC_RGBA_SRGB,
	 {1, 1, 1},  4, 4, {32, {{8}, {8}, {8}, {8} } },
	 {{{0}, {8}, {16}, {24} } } },   /*                            */

	{SVGA3DBLOCKDESC_RGB,
	 {1, 1, 1},  4, 4, {24, {{8}, {8}, {8}, {0} } },
	 {{{0}, {8}, {16}, {24} } } },   /*                          */

	{SVGA3DBLOCKDESC_RGB_SRGB,
	 {1, 1, 1},  4, 4, {24, {{8}, {8}, {8}, {0} } },
	 {{{0}, {8}, {16}, {24} } } },   /*                            */

	{SVGA3DBLOCKDESC_DEPTH,
	 {1, 1, 1},  2, 2, {16, {{0}, {0}, {16}, {0} } },
	 {{{0}, {0}, {0}, {0} } } },     /*               */

	{SVGA3DBLOCKDESC_DS,
	 {1, 1, 1},  4, 4, {32, {{0}, {8}, {24}, {0} } },
	 {{{0}, {24}, {0}, {0} } } },    /*               */

	{SVGA3DBLOCKDESC_DS,
	 {1, 1, 1},  4, 4, {32, {{0}, {8}, {24}, {0} } },
	 {{{0}, {24}, {0}, {0} } } },    /*                    */
};

static inline u32 clamped_umul32(u32 a, u32 b)
{
	uint64_t tmp = (uint64_t) a*b;
	return (tmp > (uint64_t) ((u32) -1)) ? (u32) -1 : tmp;
}

static inline const struct svga3d_surface_desc *
svga3dsurface_get_desc(SVGA3dSurfaceFormat format)
{
	if (format < ARRAY_SIZE(svga3d_surface_descs))
		return &svga3d_surface_descs[format];

	return &svga3d_surface_descs[SVGA3D_FORMAT_INVALID];
}

/*
                                                                        
  
                                
  
                                                                      
                      
  
           
                  
  
                
             
  
                                                                        
 */

static inline surf_size_struct
svga3dsurface_get_mip_size(surf_size_struct base_level, u32 mip_level)
{
	surf_size_struct size;

	size.width = max_t(u32, base_level.width >> mip_level, 1);
	size.height = max_t(u32, base_level.height >> mip_level, 1);
	size.depth = max_t(u32, base_level.depth >> mip_level, 1);
	return size;
}

static inline void
svga3dsurface_get_size_in_blocks(const struct svga3d_surface_desc *desc,
				 const surf_size_struct *pixel_size,
				 surf_size_struct *block_size)
{
	block_size->width = DIV_ROUND_UP(pixel_size->width,
					 desc->block_size.width);
	block_size->height = DIV_ROUND_UP(pixel_size->height,
					  desc->block_size.height);
	block_size->depth = DIV_ROUND_UP(pixel_size->depth,
					 desc->block_size.depth);
}

static inline bool
svga3dsurface_is_planar_surface(const struct svga3d_surface_desc *desc)
{
	return (desc->block_desc & SVGA3DBLOCKDESC_PLANAR_YUV) != 0;
}

static inline u32
svga3dsurface_calculate_pitch(const struct svga3d_surface_desc *desc,
			      const surf_size_struct *size)
{
	u32 pitch;
	surf_size_struct blocks;

	svga3dsurface_get_size_in_blocks(desc, size, &blocks);

	pitch = blocks.width * desc->pitch_bytes_per_block;

	return pitch;
}

/*
                                                                               
  
                                         
  
                                                                    
                                                                     
  
                                                                     
  
                                                                
                                                 
  
           
                   
  
                
             
  
                                                                               
 */

static inline u32
svga3dsurface_get_image_buffer_size(const struct svga3d_surface_desc *desc,
				    const surf_size_struct *size,
				    u32 pitch)
{
	surf_size_struct image_blocks;
	u32 slice_size, total_size;

	svga3dsurface_get_size_in_blocks(desc, size, &image_blocks);

	if (svga3dsurface_is_planar_surface(desc)) {
		total_size = clamped_umul32(image_blocks.width,
					    image_blocks.height);
		total_size = clamped_umul32(total_size, image_blocks.depth);
		total_size = clamped_umul32(total_size, desc->bytes_per_block);
		return total_size;
	}

	if (pitch == 0)
		pitch = svga3dsurface_calculate_pitch(desc, size);

	slice_size = clamped_umul32(image_blocks.height, pitch);
	total_size = clamped_umul32(slice_size, image_blocks.depth);

	return total_size;
}

static inline u32
svga3dsurface_get_serialized_size(SVGA3dSurfaceFormat format,
				  surf_size_struct base_level_size,
				  u32 num_mip_levels,
				  bool cubemap)
{
	const struct svga3d_surface_desc *desc = svga3dsurface_get_desc(format);
	u32 total_size = 0;
	u32 mip;

	for (mip = 0; mip < num_mip_levels; mip++) {
		surf_size_struct size =
			svga3dsurface_get_mip_size(base_level_size, mip);
		total_size += svga3dsurface_get_image_buffer_size(desc,
								  &size, 0);
	}

	if (cubemap)
		total_size *= SVGA3D_MAX_SURFACE_FACES;

	return total_size;
}


/* 
                                                                            
                           
  
                                     
                                      
 */
static inline u32
svga3dsurface_get_pixel_offset(SVGA3dSurfaceFormat format,
			       u32 width, u32 height,
			       u32 x, u32 y, u32 z)
{
	const struct svga3d_surface_desc *desc = svga3dsurface_get_desc(format);
	const u32 bw = desc->block_size.width, bh = desc->block_size.height;
	const u32 bd = desc->block_size.depth;
	const u32 rowstride = DIV_ROUND_UP(width, bw) * desc->bytes_per_block;
	const u32 imgstride = DIV_ROUND_UP(height, bh) * rowstride;
	const u32 offset = (z / bd * imgstride +
			    y / bh * rowstride +
			    x / bw * desc->bytes_per_block);
	return offset;
}


static inline u32
svga3dsurface_get_image_offset(SVGA3dSurfaceFormat format,
			       surf_size_struct baseLevelSize,
			       u32 numMipLevels,
			       u32 face,
			       u32 mip)

{
	u32 offset;
	u32 mipChainBytes;
	u32 mipChainBytesToLevel;
	u32 i;
	const struct svga3d_surface_desc *desc;
	surf_size_struct mipSize;
	u32 bytes;

	desc = svga3dsurface_get_desc(format);

	mipChainBytes = 0;
	mipChainBytesToLevel = 0;
	for (i = 0; i < numMipLevels; i++) {
		mipSize = svga3dsurface_get_mip_size(baseLevelSize, i);
		bytes = svga3dsurface_get_image_buffer_size(desc, &mipSize, 0);
		mipChainBytes += bytes;
		if (i < mip)
			mipChainBytesToLevel += bytes;
	}

	offset = mipChainBytes * face + mipChainBytesToLevel;

	return offset;
}
