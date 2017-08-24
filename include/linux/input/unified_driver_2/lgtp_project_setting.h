/***************************************************************************
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 *    File  	: lgtp_project_setting.h
 *    Author(s)   : D3 BSP Touch Team < d3-bsp-touch@lge.com >
 *    Description :
 *
 ***************************************************************************/

#if !defined ( _LGTP_PROJECT_SETTING_H_ )
#define _LGTP_PROJECT_SETTING_H_


/*                                                                           
                           
                                                                           */
#if defined ( CONFIG_TOUCHSCREEN_LU201X )
#define TOUCH_MODEL_Y30
#endif

#if defined ( CONFIG_TOUCHSCREEN_LGE_SYNAPTICS )
#define TOUCH_MODEL_C70
#endif

#if defined ( TARGET_MT6582_Y90 )
#define TOUCH_MODEL_Y90
#endif

#if defined ( TARGET_MT6582_Y70 )
#define TOUCH_MODEL_Y70
#endif

#if defined ( TARGET_MT6732_C90 )
#define TOUCH_MODEL_C90
#endif

#if defined ( TARGET_MT6582_P1S3G )
#define TOUCH_MODEL_P1S3G
#endif

#if defined ( CONFIG_TOUCHSCREEN_LGE_MELFAS )
#define TOUCH_MODEL_C50
#endif

#if defined ( TARGET_MT6582_B2L )
#define TOUCH_MODEL_B2L
#endif

#if defined ( TARGET_MT6582_L80 )
#define TOUCH_MODEL_L80
#endif

/*                                                                           
                                                             
                                                                           */
#if defined ( TOUCH_MODEL_Y30 )

/*             */
#define TOUCH_PLATFORM_QCT

/*            */
#define TOUCH_PLATFORM_MSM8210

/*              */
#define TOUCH_DEVICE_LU201X
#define TOUCH_DEVICE_LU202X
#define TOUCH_DEVICE_FT6X36
#define TOUCH_DEVICE_DUMMY

/*                */
#define ENABLE_HOVER_DETECTION

#elif defined ( TOUCH_MODEL_C70 )

/*             */
#define TOUCH_PLATFORM_QCT

/*            */
#define TOUCH_PLATFORM_MSM8916

/*              */
#define TOUCH_DEVICE_S3320

/*                */
#define ENABLE_TOUCH_AT_OFF_CHARGING

#elif defined ( TOUCH_MODEL_Y90 )

/*             */
#define TOUCH_PLATFORM_MTK

/*            */
#define TOUCH_PLATFORM_MT6582

/*              */
#define TOUCH_DEVICE_S3320

/*            */
#define ENABLE_SWIPE_MODE

#elif defined ( TOUCH_MODEL_Y70 )

/*             */
#define TOUCH_PLATFORM_MTK

/*            */
#define TOUCH_PLATFORM_MT6582

/*              */
#define TOUCH_DEVICE_S3320

/*            */
#define ENABLE_SWIPE_MODE

#elif defined ( TOUCH_MODEL_C90 )

/*             */
#define TOUCH_PLATFORM_MTK

/*            */
#define TOUCH_PLATFORM_MT6732

/*              */
#define TOUCH_DEVICE_S3320

/*            */
#define ENABLE_SWIPE_MODE

#elif defined ( TOUCH_MODEL_C50 )

/*             */
#define TOUCH_PLATFORM_QCT

/*            */
#define TOUCH_PLATFORM_MSM8916

/*              */
#define TOUCH_DEVICE_MIT200

#elif defined ( TOUCH_MODEL_B2L )

/*             */
#define TOUCH_PLATFORM_MTK

/*            */
#define TOUCH_PLATFORM_MT6582

/*              */
#define TOUCH_DEVICE_S3528

#elif defined ( TOUCH_MODEL_P1S3G )

/*             */
#define TOUCH_PLATFORM_MTK

/*            */
#define TOUCH_PLATFORM_MT6582

/*              */
#define TOUCH_DEVICE_TD4191
#elif defined ( TOUCH_MODEL_L80 )

/*             */
#define TOUCH_PLATFORM_MTK

/*            */
#define TOUCH_PLATFORM_MT6582

/*              */
#define TOUCH_DEVICE_S7020
#else
#error "Model should be defined"
#endif

#endif /*                          */

/*             */

