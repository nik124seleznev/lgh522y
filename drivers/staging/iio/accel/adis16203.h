#ifndef SPI_ADIS16203_H_
#define SPI_ADIS16203_H_

#define ADIS16203_STARTUP_DELAY	220 /*    */

#define ADIS16203_FLASH_CNT      0x00 /*                          */
#define ADIS16203_SUPPLY_OUT     0x02 /*                      */
#define ADIS16203_AUX_ADC        0x08 /*                             */
#define ADIS16203_TEMP_OUT       0x0A /*                     */
#define ADIS16203_XINCL_OUT      0x0C /*                            */
#define ADIS16203_YINCL_OUT      0x0E /*                            */
#define ADIS16203_INCL_NULL      0x18 /*                          */
#define ADIS16203_ALM_MAG1       0x20 /*                             */
#define ADIS16203_ALM_MAG2       0x22 /*                             */
#define ADIS16203_ALM_SMPL1      0x24 /*                        */
#define ADIS16203_ALM_SMPL2      0x26 /*                        */
#define ADIS16203_ALM_CTRL       0x28 /*               */
#define ADIS16203_AUX_DAC        0x30 /*                    */
#define ADIS16203_GPIO_CTRL      0x32 /*                                              */
#define ADIS16203_MSC_CTRL       0x34 /*                       */
#define ADIS16203_SMPL_PRD       0x36 /*                                       */
#define ADIS16203_AVG_CNT        0x38 /*                                 */
#define ADIS16203_SLP_CNT        0x3A /*                               */
#define ADIS16203_DIAG_STAT      0x3C /*                                     */
#define ADIS16203_GLOB_CMD       0x3E /*                                    */

/*          */
#define ADIS16203_MSC_CTRL_PWRUP_SELF_TEST	(1 << 10) /*                                                  */
#define ADIS16203_MSC_CTRL_REVERSE_ROT_EN	(1 << 9)  /*                                               */
#define ADIS16203_MSC_CTRL_SELF_TEST_EN	        (1 << 8)  /*                  */
#define ADIS16203_MSC_CTRL_DATA_RDY_EN	        (1 << 2)  /*                                              */
#define ADIS16203_MSC_CTRL_ACTIVE_HIGH	        (1 << 1)  /*                                                      */
#define ADIS16203_MSC_CTRL_DATA_RDY_DIO1	(1 << 0)  /*                                               */

/*           */
#define ADIS16203_DIAG_STAT_ALARM2        (1<<9) /*                                                      */
#define ADIS16203_DIAG_STAT_ALARM1        (1<<8) /*                                                      */
#define ADIS16203_DIAG_STAT_SELFTEST_FAIL_BIT 5 /*                                 */
#define ADIS16203_DIAG_STAT_SPI_FAIL_BIT      3 /*                            */
#define ADIS16203_DIAG_STAT_FLASH_UPT_BIT     2 /*                      */
#define ADIS16203_DIAG_STAT_POWER_HIGH_BIT    1 /*                            */
#define ADIS16203_DIAG_STAT_POWER_LOW_BIT     0 /*                           */

/*          */
#define ADIS16203_GLOB_CMD_SW_RESET	(1<<7)
#define ADIS16203_GLOB_CMD_CLEAR_STAT	(1<<4)
#define ADIS16203_GLOB_CMD_FACTORY_CAL	(1<<1)

#define ADIS16203_ERROR_ACTIVE          (1<<14)

enum adis16203_scan {
	ADIS16203_SCAN_INCLI_X,
	ADIS16203_SCAN_INCLI_Y,
	ADIS16203_SCAN_SUPPLY,
	ADIS16203_SCAN_AUX_ADC,
	ADIS16203_SCAN_TEMP,
};

#endif /*                  */
