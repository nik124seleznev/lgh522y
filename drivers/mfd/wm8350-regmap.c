/*
 * wm8350-regmap.c  --  Wolfson Microelectronics WM8350 register map
 *
 * This file splits out the tables describing the defaults and access
 * status of the WM8350 registers since they are rather large.
 *
 * Copyright 2007, 2008 Wolfson Microelectronics PLC.
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 */

#include <linux/mfd/wm8350/core.h>

/*
                
 */

static const struct wm8350_reg_access {
	u16 readable;		/*                       */
	u16 writable;		/*                       */
	u16 vol;		/*                       */
} wm8350_reg_io_map[] = {
	/*                         */
	{ 0xFFFF, 0xFFFF, 0x0000 }, /*                 */
	{ 0x7CFF, 0x0C00, 0x0000 }, /*           */
	{ 0x007F, 0x0000, 0x0000 }, /*                    */
	{ 0xBE3B, 0xBE3B, 0x8000 }, /*                         */
	{ 0xFEF7, 0xFEF7, 0xF800 }, /*                         */
	{ 0x80FF, 0x80FF, 0x8000 }, /*                         */
	{ 0xFB0E, 0xFB0E, 0x0000 }, /*                          */
	{ 0x0000, 0x0000, 0x0000 }, /*    */
	{ 0xE537, 0xE537, 0xFFFF }, /*                       */
	{ 0x0FF3, 0x0FF3, 0xFFFF }, /*                       */
	{ 0x008F, 0x008F, 0xFFFF }, /*                       */
	{ 0x6D3C, 0x6D3C, 0xFFFF }, /*                       */
	{ 0x1F8F, 0x1F8F, 0xFFFF }, /*                       */
	{ 0x8F3F, 0x8F3F, 0xFFFF }, /*                       */
	{ 0x0003, 0x0003, 0xFFFF }, /*                       */
	{ 0x0000, 0x0000, 0x0000 }, /*     */
	{ 0x7F7F, 0x7F7F, 0xFFFF }, /*                            */
	{ 0x073F, 0x073F, 0xFFFF }, /*                      */
	{ 0x1F3F, 0x1F3F, 0xFFFF }, /*                       */
	{ 0x3FFF, 0x00FF, 0xFFFF }, /*                 */
	{ 0x7F7F, 0x7F7F, 0x0000 }, /*                              */
	{ 0x0F3F, 0x0F3F, 0x0000 }, /*                        */
	{ 0x1F3F, 0x1F3F, 0x0000 }, /*                         */
	{ 0xEF7F, 0xEA7F, 0xFFFF }, /*                         */
	{ 0x3BFF, 0x0000, 0xFFFF }, /*                          */
	{ 0xFEE7, 0x0000, 0xFFFF }, /*                           */
	{ 0x35FF, 0x0000, 0xFFFF }, /*                           */
	{ 0x0F3F, 0x0000, 0xFFFF }, /*                                  */
	{ 0x0F3F, 0x0000, 0xFFFF }, /*                                       */
	{ 0x8000, 0x0000, 0xFFFF }, /*                                      */
	{ 0x1FFF, 0x0000, 0xFFFF }, /*                              */
	{ 0xEF7F, 0x0000, 0xFFFF }, /*                                    */
	{ 0x3FFF, 0x3FFF, 0x0000 }, /*                               */
	{ 0xFEE7, 0xFEE7, 0x0000 }, /*                                */
	{ 0xF5FF, 0xF5FF, 0x0000 }, /*                                */
	{ 0x0F3F, 0x0F3F, 0x0000 }, /*                                       */
	{ 0x0F3F, 0x0F3F, 0x0000 }, /*                                      */
	{ 0x8000, 0x8000, 0x0000 }, /*                                     */
	{ 0x1FFF, 0x1FFF, 0x0000 }, /*                                   */
	{ 0xEF7F, 0xEF7F, 0x0000 }, /*                                  */
	{ 0xC9F7, 0xC9F7, 0xFFFF }, /*                        */
	{ 0x8001, 0x8001, 0x0000 }, /*                        */
	{ 0xFFF7, 0xFFF7, 0xFFFF }, /*                      */
	{ 0xFBFF, 0xFBFF, 0x0000 }, /*                      */
	{ 0xFFFF, 0xFFFF, 0x0000 }, /*                      */
	{ 0x0033, 0x0033, 0x0000 }, /*                      */
	{ 0x0000, 0x0000, 0x0000 }, /*     */
	{ 0x0000, 0x0000, 0x0000 }, /*     */
	{ 0x3033, 0x3033, 0x0000 }, /*                    */
	{ 0x0000, 0x0000, 0x0000 }, /*     */
	{ 0x81FF, 0x81FF, 0xFFFF }, /*                             */
	{ 0x81FF, 0x81FF, 0xFFFF }, /*                             */
	{ 0x0000, 0x0000, 0x0000 }, /*     */
	{ 0x0FFF, 0x0FFF, 0xFFFF }, /*                    */
	{ 0x0017, 0x0017, 0x0000 }, /*                          */
	{ 0x0000, 0x0000, 0x0000 }, /*     */
	{ 0x0000, 0x0000, 0x0000 }, /*     */
	{ 0x0000, 0x0000, 0x0000 }, /*     */
	{ 0x4000, 0x4000, 0x0000 }, /*                 */
	{ 0x7000, 0x7000, 0x0000 }, /*                        */
	{ 0x3C00, 0x3C00, 0x0000 }, /*                 */
	{ 0x0000, 0x0000, 0x0000 }, /*     */
	{ 0x0000, 0x0000, 0x0000 }, /*     */
	{ 0x0000, 0x0000, 0x0000 }, /*     */
	{ 0x8303, 0x8303, 0xFFFF }, /*                    */
	{ 0x0000, 0x0000, 0x0000 }, /*     */
	{ 0x81FF, 0x81FF, 0xFFFF }, /*                             */
	{ 0x81FF, 0x81FF, 0xFFFF }, /*                             */
	{ 0x0FFF, 0x0FFF, 0x0000 }, /*                    */
	{ 0x0000, 0x0000, 0x0000 }, /*     */
	{ 0x0FFF, 0x0FFF, 0xFFFF }, /*                    */
	{ 0x0000, 0x0000, 0x0000 }, /*     */
	{ 0x0707, 0x0707, 0xFFFF }, /*                      */
	{ 0xC0C0, 0xC0C0, 0xFFFF }, /*                          */
	{ 0xC09F, 0xC09F, 0xFFFF }, /*                         */
	{ 0x0000, 0x0000, 0x0000 }, /*     */
	{ 0x0F15, 0x0F15, 0xFFFF }, /*                       */
	{ 0xC000, 0xC000, 0xFFFF }, /*                    */
	{ 0x03FF, 0x03FF, 0x0000 }, /*                         */
	{ 0x0000, 0x0000, 0x0000 }, /*     */
	{ 0xE1FC, 0xE1FC, 0x8000 }, /*                          */
	{ 0xE1FC, 0xE1FC, 0x8000 }, /*                           */
	{ 0x0000, 0x0000, 0x0000 }, /*     */
	{ 0x0000, 0x0000, 0x0000 }, /*     */
	{ 0x0000, 0x0000, 0x0000 }, /*     */
	{ 0x0000, 0x0000, 0x0000 }, /*     */
	{ 0x0000, 0x0000, 0x0000 }, /*     */
	{ 0x0000, 0x0000, 0x0000 }, /*     */
	{ 0x9807, 0x9807, 0xFFFF }, /*                           */
	{ 0x980B, 0x980B, 0xFFFF }, /*                            */
	{ 0x0000, 0x0000, 0x0000 }, /*     */
	{ 0x0000, 0x0000, 0x0000 }, /*     */
	{ 0x8909, 0x8909, 0xFFFF }, /*                           */
	{ 0x9E07, 0x9E07, 0xFFFF }, /*                           */
	{ 0x0000, 0x0000, 0x0000 }, /*     */
	{ 0x0000, 0x0000, 0x0000 }, /*     */
	{ 0x0EEE, 0x0EEE, 0x0000 }, /*                                 */
	{ 0xE0EE, 0xE0EE, 0x0000 }, /*                                  */
	{ 0x0E0F, 0x0E0F, 0x0000 }, /*                             */
	{ 0xE0E1, 0xE0E1, 0x0000 }, /*                             */
	{ 0x800E, 0x800E, 0x0000 }, /*                           */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0xE1FC, 0xE1FC, 0xFFFF }, /*                     */
	{ 0xE1FC, 0xE1FC, 0xFFFF }, /*                     */
	{ 0xE1FC, 0xE1FC, 0xFFFF }, /*                     */
	{ 0xE7FC, 0xE7FC, 0xFFFF }, /*                     */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0x80E0, 0x80E0, 0xFFFF }, /*                    */
	{ 0xBF00, 0xBF00, 0x0000 }, /*                     */
	{ 0x00F1, 0x00F1, 0x0000 }, /*                     */
	{ 0x00F8, 0x00F8, 0x0000 }, /*                       */
	{ 0x40FB, 0x40FB, 0x0000 }, /*                       */
	{ 0x7C30, 0x7C30, 0x0000 }, /*                 */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0x1FFF, 0x1FFF, 0x0000 }, /*                      */
	{ 0x1FFF, 0x1FFF, 0x0000 }, /*                                 */
	{ 0x1FFF, 0x1FFF, 0x0000 }, /*                               */
	{ 0x1FFF, 0x1FFF, 0x0000 }, /*                            */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0x00C0, 0x00C0, 0x0000 }, /*                     */
	{ 0x1FFF, 0x1FFF, 0x0000 }, /*                                 */
	{ 0x1FFF, 0x1FFF, 0x0000 }, /*                                 */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0xFFFF, 0xFFFF, 0x0000 }, /*                               */
	{ 0xFFFF, 0xFFFF, 0x0000 }, /*                               */
	{ 0xFFFF, 0xFFFF, 0x0000 }, /*                               */
	{ 0x000F, 0x000F, 0x0000 }, /*                               */
	{ 0xF0FF, 0xF0FF, 0xA000 }, /*                              */
	{ 0x3707, 0x3707, 0x0000 }, /*                              */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0x7FFF, 0x7000, 0xFFFF }, /*                      */
	{ 0x7FFF, 0x7000, 0xFFFF }, /*                      */
	{ 0x7FFF, 0x7000, 0xFFFF }, /*                      */
	{ 0x7FFF, 0x7000, 0xFFFF }, /*                      */
	{ 0x0FFF, 0x0000, 0xFFFF }, /*                             */
	{ 0x0FFF, 0x0000, 0xFFFF }, /*                              */
	{ 0x0FFF, 0x0000, 0xFFFF }, /*                              */
	{ 0x0FFF, 0x0000, 0xFFFF }, /*                           */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0x000F, 0x000F, 0x0000 }, /*                                   */
	{ 0xFFFF, 0xFFFF, 0x0000 }, /*                             */
	{ 0xFFFF, 0xFFFF, 0x0000 }, /*                             */
	{ 0xFFFF, 0xFFFF, 0x0000 }, /*                             */
	{ 0xFFFF, 0xFFFF, 0x0000 }, /*                             */
	{ 0xBFFF, 0xBFFF, 0x8000 }, /*                                  */
	{ 0xFFFF, 0x4FFF, 0xB000 }, /*                                  */
	{ 0x007F, 0x007F, 0x0000 }, /*                                  */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0x903F, 0x903F, 0xFFFF }, /*                              */
	{ 0xE333, 0xE333, 0xFFFF }, /*                          */
	{ 0x903F, 0x903F, 0xFFFF }, /*                              */
	{ 0xE333, 0xE333, 0xFFFF }, /*                          */
	{ 0x8F3F, 0x8F3F, 0xFFFF }, /*                           */
	{ 0x332D, 0x332D, 0x0000 }, /*                            */
	{ 0x002D, 0x002D, 0x0000 }, /*                           */
	{ 0x5177, 0x5177, 0x8000 }, /*                               */
	{ 0x047F, 0x047F, 0x0000 }, /*                      */
	{ 0xFFC0, 0xFFC0, 0x0000 }, /*                       */
	{ 0x737F, 0x737F, 0x0000 }, /*                        */
	{ 0x535B, 0x535B, 0x0000 }, /*                      */
	{ 0xFFC0, 0xFFC0, 0x0000 }, /*                       */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0x047F, 0x047F, 0x0000 }, /*                      */
	{ 0xFFC0, 0xFFC0, 0x0000 }, /*                       */
	{ 0x737F, 0x737F, 0x0000 }, /*                        */
	{ 0x047F, 0x047F, 0x0000 }, /*                      */
	{ 0xFFC0, 0xFFC0, 0x0000 }, /*                       */
	{ 0x737F, 0x737F, 0x0000 }, /*                        */
	{ 0x535B, 0x535B, 0x0000 }, /*                      */
	{ 0xFFC0, 0xFFC0, 0x0000 }, /*                       */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0x047F, 0x047F, 0x0000 }, /*                      */
	{ 0xFFC0, 0xFFC0, 0x0000 }, /*                       */
	{ 0x737F, 0x737F, 0x0000 }, /*                        */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0xFFD3, 0xFFD3, 0x0000 }, /*                             */
	{ 0x441F, 0x441F, 0x0000 }, /*                     */
	{ 0xFFC0, 0xFFC0, 0x0000 }, /*                      */
	{ 0x331F, 0x331F, 0x0000 }, /*                       */
	{ 0x441F, 0x441F, 0x0000 }, /*                     */
	{ 0xFFC0, 0xFFC0, 0x0000 }, /*                      */
	{ 0x331F, 0x331F, 0x0000 }, /*                       */
	{ 0x441F, 0x441F, 0x0000 }, /*                     */
	{ 0xFFC0, 0xFFC0, 0x0000 }, /*                      */
	{ 0x331F, 0x331F, 0x0000 }, /*                       */
	{ 0x441F, 0x441F, 0x0000 }, /*                     */
	{ 0xFFC0, 0xFFC0, 0x0000 }, /*                      */
	{ 0x331F, 0x331F, 0x0000 }, /*                       */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0x8F3F, 0x8F3F, 0x0000 }, /*                        */
	{ 0xFF3F, 0xE03F, 0x0000 }, /*                             */
	{ 0xEF2F, 0xE02F, 0x0000 }, /*                    */
	{ 0xF3FF, 0xB3FF, 0xc000 }, /*                         */
	{ 0xFFFF, 0xFFFF, 0x0000 }, /*                 */
	{ 0x09FF, 0x01FF, 0x0000 }, /*                   */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0xFFFF, 0xFFFF, 0xFFFF }, /*      */
	{ 0xFFFF, 0xFFFF, 0xFFFF }, /*      */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0x8F3F, 0x0000, 0xFFFF }, /*                        */
	{ 0x0000, 0x0000, 0xFFFF }, /*                       */
	{ 0x34FE, 0x0000, 0xFFFF }, /*      */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0xFFFF, 0x1FFF, 0xFFFF }, /*                        */
	{ 0xFFFF, 0x1FFF, 0xFFFF }, /*      */
	{ 0xFFFF, 0x1FFF, 0xFFFF }, /*      */
	{ 0xFFFF, 0x1FFF, 0xFFFF }, /*      */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0xFFFF, 0x0010, 0xFFFF }, /*      */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0xFFFF, 0x0010, 0xFFFF }, /*      */
	{ 0xFFFF, 0x0010, 0xFFFF }, /*      */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0xFFFF, 0x0010, 0xFFFF }, /*      */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
	{ 0x0000, 0x0000, 0x0000 }, /*      */
};

static bool wm8350_readable(struct device *dev, unsigned int reg)
{
	return wm8350_reg_io_map[reg].readable;
}

static bool wm8350_writeable(struct device *dev, unsigned int reg)
{
	struct wm8350 *wm8350 = dev_get_drvdata(dev);

	if (!wm8350->unlocked) {
		if ((reg >= WM8350_GPIO_FUNCTION_SELECT_1 &&
		     reg <= WM8350_GPIO_FUNCTION_SELECT_4) ||
		    (reg >= WM8350_BATTERY_CHARGER_CONTROL_1 &&
		     reg <= WM8350_BATTERY_CHARGER_CONTROL_3))
			return false;
	}

	return wm8350_reg_io_map[reg].writable;
}

static bool wm8350_volatile(struct device *dev, unsigned int reg)
{
	return wm8350_reg_io_map[reg].vol;
}

static bool wm8350_precious(struct device *dev, unsigned int reg)
{
	switch (reg) {
	case WM8350_SYSTEM_INTERRUPTS:
	case WM8350_INT_STATUS_1:
	case WM8350_INT_STATUS_2:
	case WM8350_POWER_UP_INT_STATUS:
	case WM8350_UNDER_VOLTAGE_INT_STATUS:
	case WM8350_OVER_CURRENT_INT_STATUS:
	case WM8350_GPIO_INT_STATUS:
	case WM8350_COMPARATOR_INT_STATUS:
		return true;

	default:
		return false;
	}
}

const struct regmap_config wm8350_regmap = {
	.reg_bits = 8,
	.val_bits = 16,

	.cache_type = REGCACHE_RBTREE,

	.max_register = WM8350_MAX_REGISTER,
	.readable_reg = wm8350_readable,
	.writeable_reg = wm8350_writeable,
	.volatile_reg = wm8350_volatile,
	.precious_reg = wm8350_precious,
};
