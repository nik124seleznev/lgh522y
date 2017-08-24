#include "tpd.h"

int tpd_calibrate_en = 0;
module_param(tpd_calibrate_en, int, 00664);

int tpd_show_version = 0;
module_param(tpd_show_version, int, 00664);

/*                                                                      */
void tpd_switch_single_mode(void)
{
#ifdef HAVE_SINGLE_MULTIPLE_SCAN_MODE
	_tpd_switch_single_mode();
#endif
}
EXPORT_SYMBOL(tpd_switch_single_mode);

/*                                                                   */
void tpd_switch_multiple_mode(void)
{
#ifdef HAVE_SINGLE_MULTIPLE_SCAN_MODE
	_tpd_switch_multiple_mode();
#endif
}
EXPORT_SYMBOL(tpd_switch_multiple_mode);

/*                                         */
void tpd_switch_sleep_mode(void)
{
#ifdef HAVE_SLEEP_NORMAL_MODE
	_tpd_switch_sleep_mode();
#endif
}
EXPORT_SYMBOL(tpd_switch_sleep_mode);

/*                                        */
void tpd_switch_normal_mode(void)
{
#ifdef HAVE_SLEEP_NORMAL_MODE
	_tpd_switch_normal_mode();
#endif
}
EXPORT_SYMBOL(tpd_switch_normal_mode);
