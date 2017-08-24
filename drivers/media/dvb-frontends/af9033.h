/*
 * Afatech AF9033 demodulator driver
 *
 * Copyright (C) 2009 Antti Palosaari <crope@iki.fi>
 * Copyright (C) 2012 Antti Palosaari <crope@iki.fi>
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License along
 *    with this program; if not, write to the Free Software Foundation, Inc.,
 *    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef AF9033_H
#define AF9033_H

#include <linux/kconfig.h>

struct af9033_config {
	/*
               
  */
	u8 i2c_addr;

	/*
            
                                                                         
                                          
  */
	u32 clock;

	/*
                  
  */
#define AF9033_ADC_MULTIPLIER_1X   0
#define AF9033_ADC_MULTIPLIER_2X   1
	u8 adc_multiplier;

	/*
         
  */
#define AF9033_TUNER_TUA9001     0x27 /*                   */
#define AF9033_TUNER_FC0011      0x28 /*                  */
#define AF9033_TUNER_FC0012      0x2e /*                  */
#define AF9033_TUNER_MXL5007T    0xa0 /*                    */
#define AF9033_TUNER_TDA18218    0xa1 /*                 */
#define AF9033_TUNER_FC2580      0x32 /*            */
/*             */
#define AF9033_TUNER_IT9135_38   0x38 /*       */
#define AF9033_TUNER_IT9135_51   0x51 /*                    */
#define AF9033_TUNER_IT9135_52   0x52 /*                    */
/*                */
#define AF9033_TUNER_IT9135_60   0x60 /*          */
#define AF9033_TUNER_IT9135_61   0x61 /*                       */
#define AF9033_TUNER_IT9135_62   0x62 /*                       */
	u8 tuner;

	/*
               
  */
#define AF9033_TS_MODE_USB       0
#define AF9033_TS_MODE_PARALLEL  1
#define AF9033_TS_MODE_SERIAL    2
	u8 ts_mode:2;

	/*
                            
  */
	bool spec_inv;
};


#if IS_ENABLED(CONFIG_DVB_AF9033)
extern struct dvb_frontend *af9033_attach(const struct af9033_config *config,
	struct i2c_adapter *i2c);
#else
static inline struct dvb_frontend *af9033_attach(
	const struct af9033_config *config, struct i2c_adapter *i2c)
{
	pr_warn("%s: driver disabled by Kconfig\n", __func__);
	return NULL;
}
#endif

#endif /*          */
