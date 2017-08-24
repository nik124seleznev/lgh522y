/******************************************************************************
 * mt_gpio_ext.c - MTKLinux GPIO Device Driver
 * 
 * Copyright 2008-2009 MediaTek Co.,Ltd.
 * 
 * DESCRIPTION:
 *     This file provid the other drivers GPIO debug functions
 *
 ******************************************************************************/

#include <mach/mt_reg_base.h>
#include <mach/mt_pmic_wrap.h>
#include <mach/mt_gpio.h>
#include <mach/mt_gpio_core.h>
#include <mach/mt_gpio_ext.h>

//                                 
//                                
//                                 
#define GPIOEXT_BASE        (0x0) 			//               

//                                                                 
//               
/*                                                                           */
int mt_set_gpio_dir_ext(unsigned long pin, unsigned long dir)			{    dump_stack();    return ERINVAL;}
/*                                                                           */
int mt_get_gpio_dir_ext(unsigned long pin)								{    dump_stack();    return ERINVAL;}
/*                                                                           */
int mt_set_gpio_pull_enable_ext(unsigned long pin, unsigned long enable){    dump_stack();    return ERINVAL;}
/*                                                                           */
int mt_get_gpio_pull_enable_ext(unsigned long pin)						{    dump_stack();    return ERINVAL;}
/*                                                                           */
int mt_set_gpio_smt_ext(unsigned long pin, unsigned long enable)		{    dump_stack();    return ERINVAL;}
/*                                                                           */
int mt_get_gpio_smt_ext(unsigned long pin)								{    dump_stack();    return ERINVAL;}
/*                                                                           */
int mt_set_gpio_ies_ext(unsigned long pin, unsigned long enable)		{    dump_stack();    return ERINVAL;}
/*                                                                           */
int mt_get_gpio_ies_ext(unsigned long pin)								{    dump_stack();    return ERINVAL;}
/*                                                                           */
int mt_set_gpio_pull_select_ext(unsigned long pin, unsigned long select){    dump_stack();    return ERINVAL;}
/*                                                                           */
int mt_get_gpio_pull_select_ext(unsigned long pin)						{    dump_stack();    return ERINVAL;}
/*                                                                           */
int mt_set_gpio_inversion_ext(unsigned long pin, unsigned long enable)	{    dump_stack();    return ERINVAL;}
/*                                                                           */
int mt_get_gpio_inversion_ext(unsigned long pin)						{    dump_stack();    return ERINVAL;}
/*                                                                           */
int mt_set_gpio_out_ext(unsigned long pin, unsigned long output)		{    dump_stack();    return ERINVAL;}
/*                                                                           */
int mt_get_gpio_out_ext(unsigned long pin)								{    dump_stack();    return ERINVAL;}
/*                                                                           */
int mt_get_gpio_in_ext(unsigned long pin)								{    dump_stack();    return ERINVAL;}
/*                                                                           */
int mt_set_gpio_mode_ext(unsigned long pin, unsigned long mode)			{    dump_stack();    return ERINVAL;}
/*                                                                           */
int mt_get_gpio_mode_ext(unsigned long pin)								{    dump_stack();    return ERINVAL;}
