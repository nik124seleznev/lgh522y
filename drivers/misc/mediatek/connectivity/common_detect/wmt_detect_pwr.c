/*
* Copyright (C) 2011-2014 MediaTek Inc.
* 
* This program is free software: you can redistribute it and/or modify it under the terms of the 
* GNU General Public License version 2 as published by the Free Software Foundation.
* 
* This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
* without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
* See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with this program.
* If not, see <http://www.gnu.org/licenses/>.
*/

#include <mach/mtk_rtc.h>
#include <mach/mt_gpio.h>
#include <cust_gpio_usage.h>

#ifdef DFT_TAG
#undef DFT_TAG
#endif
#define DFT_TAG         "[WMT-DETECT]"

#include "wmt_detect.h"



#define INVALID_PIN_ID (0xFFFFFFFF)

#ifdef GPIO_COMBO_6620_LDO_EN_PIN
	#define COMBO_LDO_PIN GPIO_COMBO_6620_LDO_EN_PIN
#else
	#define COMBO_LDO_PIN INVALID_PIN_ID
#endif

#ifdef GPIO_COMBO_PMU_EN_PIN
	#define COMBO_PMU_PIN GPIO_COMBO_PMU_EN_PIN
#else
	#define COMBO_PMU_PIN INVALID_PIN_ID
#endif

#ifdef GPIO_COMBO_PMUV28_EN_PIN
	#define COMBO_PMUV28_PIN GPIO_COMBO_PMUV28_EN_PIN
#else
	#define COMBO_PMUV28_PIN INVALID_PIN_ID
#endif

#ifdef GPIO_COMBO_RST_PIN
	#define COMBO_RST_PIN GPIO_COMBO_RST_PIN
#else
	#define COMBO_RST_PIN INVALID_PIN_ID
#endif

#ifdef GPIO_WIFI_EINT_PIN
	#define COMBO_WIFI_EINT_PIN GPIO_WIFI_EINT_PIN
#else
	#define COMBO_WIFI_EINT_PIN INVALID_PIN_ID
#endif




/*                      */
static int wmt_detect_dump_pin_conf (void)
{
    WMT_DETECT_INFO_FUNC( "[WMT-DETECT]=>dump wmt pin configuration start<=\n");

    #ifdef GPIO_COMBO_6620_LDO_EN_PIN
        WMT_DETECT_INFO_FUNC( "LDO(GPIO%d)\n", GPIO_COMBO_6620_LDO_EN_PIN);
    #else
        WMT_DETECT_INFO_FUNC( "LDO(not defined)\n");
    #endif

    #ifdef GPIO_COMBO_PMU_EN_PIN
        WMT_DETECT_INFO_FUNC( "PMU(GPIO%d)\n", GPIO_COMBO_PMU_EN_PIN);
    #else
        WMT_DETECT_INFO_FUNC( "PMU(not defined)\n");
    #endif

    #ifdef GPIO_COMBO_PMUV28_EN_PIN
        WMT_DETECT_INFO_FUNC( "PMUV28(GPIO%d)\n", GPIO_COMBO_PMUV28_EN_PIN);
    #else
        WMT_DETECT_INFO_FUNC( "PMUV28(not defined)\n");
    #endif

    #ifdef GPIO_COMBO_RST_PIN
        WMT_DETECT_INFO_FUNC( "RST(GPIO%d)\n", GPIO_COMBO_RST_PIN);
    #else
        WMT_DETECT_INFO_FUNC( "RST(not defined)\n");
    #endif

    #ifdef GPIO_COMBO_BGF_EINT_PIN
        WMT_DETECT_INFO_FUNC( "BGF_EINT(GPIO%d)\n", GPIO_COMBO_BGF_EINT_PIN);
    #else
        WMT_DETECT_INFO_FUNC( "BGF_EINT(not defined)\n");
    #endif

    #ifdef CUST_EINT_COMBO_BGF_NUM
        WMT_DETECT_INFO_FUNC( "BGF_EINT_NUM(%d)\n", CUST_EINT_COMBO_BGF_NUM);
    #else
        WMT_DETECT_INFO_FUNC( "BGF_EINT_NUM(not defined)\n");
    #endif

    #ifdef GPIO_WIFI_EINT_PIN
        WMT_DETECT_INFO_FUNC( "WIFI_EINT(GPIO%d)\n", GPIO_WIFI_EINT_PIN);
    #else
        WMT_DETECT_INFO_FUNC( "WIFI_EINT(not defined)\n");
    #endif

    #ifdef CUST_EINT_WIFI_NUM
        WMT_DETECT_INFO_FUNC( "WIFI_EINT_NUM(%d)\n", CUST_EINT_WIFI_NUM);
    #else
        WMT_DETECT_INFO_FUNC( "WIFI_EINT_NUM(not defined)\n");
    #endif

    WMT_DETECT_INFO_FUNC( "[WMT-PLAT]=>dump wmt pin configuration emds<=\n");
    return 0;
}

int _wmt_detect_set_output_mode (unsigned int id)
{
	mt_set_gpio_pull_enable(id, GPIO_PULL_DISABLE);
	mt_set_gpio_dir(id, GPIO_DIR_OUT);
	mt_set_gpio_mode(id, GPIO_MODE_GPIO);
	WMT_DETECT_DBG_FUNC("WMT-DETECT: set GPIO%d to output mode \n", id);
	return 0;
}

int _wmt_detect_set_input_mode (unsigned int id)
{
	mt_set_gpio_pull_enable(id, GPIO_PULL_DISABLE);
	mt_set_gpio_dir(id, GPIO_DIR_IN);
	mt_set_gpio_mode(id, GPIO_MODE_GPIO);
	WMT_DETECT_DBG_FUNC("WMT-DETECT: set GPIO%d to input mode \n", id);
	return 0;
}

int _wmt_detect_output_low (unsigned int id)
{
	/*                                */
	mt_set_gpio_out(id, GPIO_OUT_ZERO);
	WMT_DETECT_DBG_FUNC("WMT-DETECT: set GPIO%d to output 0 \n", id);
	return 0;
}

int _wmt_detect_output_high (unsigned int id)
{
	/*                                */
	mt_set_gpio_out(id, GPIO_OUT_ONE);
	WMT_DETECT_DBG_FUNC("WMT-DETECT: set GPIO%d to output 0 \n", id);
	return 0;
}

int _wmt_detect_read_gpio_input (unsigned int id)
{
	int retval = 0;
	_wmt_detect_set_input_mode(id);
	retval = mt_get_gpio_in(id);
	WMT_DETECT_DBG_FUNC("WMT-DETECT: set GPIO%d to output 0 \n", id);
	return retval;
}


/*                                                                             
                                                
                          
                         
                                                                               
                                  
*/
static int wmt_detect_chip_pwr_on (void)
{
	int retval = -1;
	
	/*                         */
	if ((COMBO_PMU_PIN == INVALID_PIN_ID) || \
		(COMBO_RST_PIN == INVALID_PIN_ID) || \
		(COMBO_WIFI_EINT_PIN == INVALID_PIN_ID) )
	{
		WMT_DETECT_ERR_FUNC("WMT-DETECT: either PMU(%d) or RST(%d) or WIFI_EINT(%d) is not set\n",\
			COMBO_PMU_PIN, \
			COMBO_RST_PIN, \
			COMBO_WIFI_EINT_PIN);
		return retval;
	}
	
	/*                                    */
	if (COMBO_LDO_PIN != INVALID_PIN_ID)
	{
		_wmt_detect_set_output_mode(COMBO_LDO_PIN);
		_wmt_detect_output_low(COMBO_LDO_PIN);
	}

	
	_wmt_detect_set_output_mode(COMBO_PMU_PIN);
	_wmt_detect_output_low(COMBO_PMU_PIN);
	

	_wmt_detect_set_output_mode(COMBO_RST_PIN);
	_wmt_detect_output_low(COMBO_RST_PIN);
	
#if 0
	_wmt_detect_set_output_mode(COMBO_WIFI_EINT_PIN);
	
	_wmt_detect_output_low(COMBO_WIFI_EINT_PIN);
#endif

	/*             */
	_wmt_detect_output_high(COMBO_LDO_PIN);
	/*                         */
	msleep(MAX_LDO_STABLE_TIME);
	
	/*                                           */
	rtc_gpio_enable_32k(RTC_GPIO_USER_GPS);
	msleep(MAX_RTC_STABLE_TIME);
	
	/*                                                                 */
	/*           */
	
	/*                               */
	msleep(MAX_OFF_STABLE_TIME);
	/*                                                */
	_wmt_detect_output_high(COMBO_PMU_PIN);
	msleep(MAX_RST_STABLE_TIME);
	/*                                                   */
	_wmt_detect_output_high(COMBO_RST_PIN);
	msleep(MAX_ON_STABLE_TIME);
	
	retval = 0;
	return retval;
}

static int wmt_detect_chip_pwr_off (void)
{
	/*                               */
	if (COMBO_LDO_PIN != INVALID_PIN_ID)
	{
		_wmt_detect_set_output_mode(COMBO_RST_PIN);
		_wmt_detect_output_low(COMBO_RST_PIN);
	}
	
	/*                               */
	if (COMBO_RST_PIN != INVALID_PIN_ID)
	{
		_wmt_detect_set_output_mode(COMBO_RST_PIN);
		_wmt_detect_output_low(COMBO_RST_PIN);
	}		
	/*                               */
	if (COMBO_PMU_PIN != INVALID_PIN_ID)
	{
		_wmt_detect_set_output_mode(COMBO_PMU_PIN);
		_wmt_detect_output_low(COMBO_PMU_PIN);
	}
	return 0;	
}

int wmt_detect_read_ext_cmb_status (void)
{
	int retval = 0;
	/*                         */
	if (COMBO_WIFI_EINT_PIN == INVALID_PIN_ID)
	{
		retval = 0;
		WMT_DETECT_ERR_FUNC("WMT-DETECT: no WIFI_EINT pin set\n");
	}
	else
	{
		retval = _wmt_detect_read_gpio_input(COMBO_WIFI_EINT_PIN);
		WMT_DETECT_ERR_FUNC("WMT-DETECT: WIFI_EINT input status:%d\n", retval);
		
	}
	return retval;
}



int wmt_detect_chip_pwr_ctrl (int on)
{
	int retval = -1;
	if (0 == on)
	{
		/*                    */
		retval = wmt_detect_chip_pwr_off();
	}
	else
	{
		wmt_detect_dump_pin_conf();
		/*                   */
		retval = wmt_detect_chip_pwr_on();
	}
	return retval;
}


int wmt_detect_sdio_pwr_ctrl (int on)
{
	int retval = -1;
#ifdef MTK_WCN_COMBO_CHIP_SUPPORT	
	if (0 == on)
	{
		/*                   */
		retval = board_sdio_ctrl(1, 0);
	}
	else
	{
		/*                  */
		retval = board_sdio_ctrl(1, 1);
	}
#else
	WMT_DETECT_WARN_FUNC("WMT-DETECT: MTK_WCN_COMBO_CHIP_SUPPORT is not set\n");
#endif
	return retval;
}


