/*
 * Copyright 2011 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * VA LINUX SYSTEMS AND/OR ITS SUPPLIERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef DRM_FOURCC_H
#define DRM_FOURCC_H

#include <linux/types.h>

#define fourcc_code(a, b, c, d) ((__u32)(a) | ((__u32)(b) << 8) | \
				 ((__u32)(c) << 16) | ((__u32)(d) << 24))

#define DRM_FORMAT_BIG_ENDIAN (1<<31) /*                                               */

/*             */
#define DRM_FORMAT_C8		fourcc_code('C', '8', ' ', ' ') /*         */

/*           */
#define DRM_FORMAT_RGB332	fourcc_code('R', 'G', 'B', '8') /*                   */
#define DRM_FORMAT_BGR233	fourcc_code('B', 'G', 'R', '8') /*                   */

/*            */
#define DRM_FORMAT_XRGB4444	fourcc_code('X', 'R', '1', '2') /*                                      */
#define DRM_FORMAT_XBGR4444	fourcc_code('X', 'B', '1', '2') /*                                      */
#define DRM_FORMAT_RGBX4444	fourcc_code('R', 'X', '1', '2') /*                                      */
#define DRM_FORMAT_BGRX4444	fourcc_code('B', 'X', '1', '2') /*                                      */

#define DRM_FORMAT_ARGB4444	fourcc_code('A', 'R', '1', '2') /*                                      */
#define DRM_FORMAT_ABGR4444	fourcc_code('A', 'B', '1', '2') /*                                      */
#define DRM_FORMAT_RGBA4444	fourcc_code('R', 'A', '1', '2') /*                                      */
#define DRM_FORMAT_BGRA4444	fourcc_code('B', 'A', '1', '2') /*                                      */

#define DRM_FORMAT_XRGB1555	fourcc_code('X', 'R', '1', '5') /*                                      */
#define DRM_FORMAT_XBGR1555	fourcc_code('X', 'B', '1', '5') /*                                      */
#define DRM_FORMAT_RGBX5551	fourcc_code('R', 'X', '1', '5') /*                                      */
#define DRM_FORMAT_BGRX5551	fourcc_code('B', 'X', '1', '5') /*                                      */

#define DRM_FORMAT_ARGB1555	fourcc_code('A', 'R', '1', '5') /*                                      */
#define DRM_FORMAT_ABGR1555	fourcc_code('A', 'B', '1', '5') /*                                      */
#define DRM_FORMAT_RGBA5551	fourcc_code('R', 'A', '1', '5') /*                                      */
#define DRM_FORMAT_BGRA5551	fourcc_code('B', 'A', '1', '5') /*                                      */

#define DRM_FORMAT_RGB565	fourcc_code('R', 'G', '1', '6') /*                                  */
#define DRM_FORMAT_BGR565	fourcc_code('B', 'G', '1', '6') /*                                  */

/*            */
#define DRM_FORMAT_RGB888	fourcc_code('R', 'G', '2', '4') /*                            */
#define DRM_FORMAT_BGR888	fourcc_code('B', 'G', '2', '4') /*                            */

/*            */
#define DRM_FORMAT_XRGB8888	fourcc_code('X', 'R', '2', '4') /*                                      */
#define DRM_FORMAT_XBGR8888	fourcc_code('X', 'B', '2', '4') /*                                      */
#define DRM_FORMAT_RGBX8888	fourcc_code('R', 'X', '2', '4') /*                                      */
#define DRM_FORMAT_BGRX8888	fourcc_code('B', 'X', '2', '4') /*                                      */

#define DRM_FORMAT_ARGB8888	fourcc_code('A', 'R', '2', '4') /*                                      */
#define DRM_FORMAT_ABGR8888	fourcc_code('A', 'B', '2', '4') /*                                      */
#define DRM_FORMAT_RGBA8888	fourcc_code('R', 'A', '2', '4') /*                                      */
#define DRM_FORMAT_BGRA8888	fourcc_code('B', 'A', '2', '4') /*                                      */

#define DRM_FORMAT_XRGB2101010	fourcc_code('X', 'R', '3', '0') /*                                         */
#define DRM_FORMAT_XBGR2101010	fourcc_code('X', 'B', '3', '0') /*                                         */
#define DRM_FORMAT_RGBX1010102	fourcc_code('R', 'X', '3', '0') /*                                         */
#define DRM_FORMAT_BGRX1010102	fourcc_code('B', 'X', '3', '0') /*                                         */

#define DRM_FORMAT_ARGB2101010	fourcc_code('A', 'R', '3', '0') /*                                         */
#define DRM_FORMAT_ABGR2101010	fourcc_code('A', 'B', '3', '0') /*                                         */
#define DRM_FORMAT_RGBA1010102	fourcc_code('R', 'A', '3', '0') /*                                         */
#define DRM_FORMAT_BGRA1010102	fourcc_code('B', 'A', '3', '0') /*                                         */

/*              */
#define DRM_FORMAT_YUYV		fourcc_code('Y', 'U', 'Y', 'V') /*                                            */
#define DRM_FORMAT_YVYU		fourcc_code('Y', 'V', 'Y', 'U') /*                                            */
#define DRM_FORMAT_UYVY		fourcc_code('U', 'Y', 'V', 'Y') /*                                            */
#define DRM_FORMAT_VYUY		fourcc_code('V', 'Y', 'U', 'Y') /*                                            */

#define DRM_FORMAT_AYUV		fourcc_code('A', 'Y', 'U', 'V') /*                                        */

/*
                
                             
                                                    
     
                                                    
 */
#define DRM_FORMAT_NV12		fourcc_code('N', 'V', '1', '2') /*                            */
#define DRM_FORMAT_NV21		fourcc_code('N', 'V', '2', '1') /*                            */
#define DRM_FORMAT_NV16		fourcc_code('N', 'V', '1', '6') /*                            */
#define DRM_FORMAT_NV61		fourcc_code('N', 'V', '6', '1') /*                            */
#define DRM_FORMAT_NV24		fourcc_code('N', 'V', '2', '4') /*                            */
#define DRM_FORMAT_NV42		fourcc_code('N', 'V', '4', '2') /*                            */

/*                           */
#define DRM_FORMAT_NV12MT	fourcc_code('T', 'M', '1', '2') /*                                              */

/*
                
                            
                              
                              
     
                              
                              
 */
#define DRM_FORMAT_YUV410	fourcc_code('Y', 'U', 'V', '9') /*                                         */
#define DRM_FORMAT_YVU410	fourcc_code('Y', 'V', 'U', '9') /*                                         */
#define DRM_FORMAT_YUV411	fourcc_code('Y', 'U', '1', '1') /*                                         */
#define DRM_FORMAT_YVU411	fourcc_code('Y', 'V', '1', '1') /*                                         */
#define DRM_FORMAT_YUV420	fourcc_code('Y', 'U', '1', '2') /*                                         */
#define DRM_FORMAT_YVU420	fourcc_code('Y', 'V', '1', '2') /*                                         */
#define DRM_FORMAT_YUV422	fourcc_code('Y', 'U', '1', '6') /*                                         */
#define DRM_FORMAT_YVU422	fourcc_code('Y', 'V', '1', '6') /*                                         */
#define DRM_FORMAT_YUV444	fourcc_code('Y', 'U', '2', '4') /*                                         */
#define DRM_FORMAT_YVU444	fourcc_code('Y', 'V', '2', '4') /*                                         */

#endif /*              */