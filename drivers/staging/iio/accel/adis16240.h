#ifndef SPI_ADIS16240_H_
#define SPI_ADIS16240_H_

#define ADIS16240_STARTUP_DELAY	220 /*    */

/*                          */
#define ADIS16240_FLASH_CNT      0x00
/*                      */
#define ADIS16240_SUPPLY_OUT     0x02
/*                              */
#define ADIS16240_XACCL_OUT      0x04
/*                              */
#define ADIS16240_YACCL_OUT      0x06
/*                              */
#define ADIS16240_ZACCL_OUT      0x08
/*                             */
#define ADIS16240_AUX_ADC        0x0A
/*                     */
#define ADIS16240_TEMP_OUT       0x0C
/*                                  */
#define ADIS16240_XPEAK_OUT      0x0E
/*                                  */
#define ADIS16240_YPEAK_OUT      0x10
/*                                  */
#define ADIS16240_ZPEAK_OUT      0x12
/*                                          */
#define ADIS16240_XYZPEAK_OUT    0x14
/*                                                */
#define ADIS16240_CAPT_BUF1      0x16
/*                                          */
#define ADIS16240_CAPT_BUF2      0x18
/*                         */
#define ADIS16240_DIAG_STAT      0x1A
/*                           */
#define ADIS16240_EVNT_CNTR      0x1C
/*                                                */
#define ADIS16240_CHK_SUM        0x1E
/*                                                    */
#define ADIS16240_XACCL_OFF      0x20
/*                                                    */
#define ADIS16240_YACCL_OFF      0x22
/*                                                    */
#define ADIS16240_ZACCL_OFF      0x24
/*                        */
#define ADIS16240_CLK_TIME       0x2E
/*                      */
#define ADIS16240_CLK_DATE       0x30
/*             */
#define ADIS16240_CLK_YEAR       0x32
/*                                  */
#define ADIS16240_WAKE_TIME      0x34
/*                                */
#define ADIS16240_WAKE_DATE      0x36
/*                             */
#define ADIS16240_ALM_MAG1       0x38
/*                             */
#define ADIS16240_ALM_MAG2       0x3A
/*               */
#define ADIS16240_ALM_CTRL       0x3C
/*                                   */
#define ADIS16240_XTRIG_CTRL     0x3E
/*                          */
#define ADIS16240_CAPT_PNTR      0x40
/*                                    */
#define ADIS16240_CAPT_CTRL      0x42
/*                                              */
#define ADIS16240_GPIO_CTRL      0x44
/*                       */
#define ADIS16240_MSC_CTRL       0x46
/*                                       */
#define ADIS16240_SMPL_PRD       0x48
/*                */
#define ADIS16240_GLOB_CMD       0x4A

/*          */
/*                                             */
#define ADIS16240_MSC_CTRL_XYZPEAK_OUT_EN	(1 << 15)
/*                                                                    */
#define ADIS16240_MSC_CTRL_X_Y_ZPEAK_OUT_EN	(1 << 14)
/*                                                               */
#define ADIS16240_MSC_CTRL_SELF_TEST_EN	        (1 << 8)
/*                                              */
#define ADIS16240_MSC_CTRL_DATA_RDY_EN	        (1 << 2)
/*                                                      */
#define ADIS16240_MSC_CTRL_ACTIVE_HIGH	        (1 << 1)
/*                                               */
#define ADIS16240_MSC_CTRL_DATA_RDY_DIO2	(1 << 0)

/*           */
/*                                                      */
#define ADIS16240_DIAG_STAT_ALARM2      (1<<9)
/*                                                      */
#define ADIS16240_DIAG_STAT_ALARM1      (1<<8)
/*                                                 */
#define ADIS16240_DIAG_STAT_CPT_BUF_FUL (1<<7)
/*                                                    */
#define ADIS16240_DIAG_STAT_CHKSUM      (1<<6)
/*                                                 */
#define ADIS16240_DIAG_STAT_PWRON_FAIL_BIT  5
/*                                                   */
#define ADIS16240_DIAG_STAT_PWRON_BUSY  (1<<4)
/*                            */
#define ADIS16240_DIAG_STAT_SPI_FAIL_BIT	3
/*                      */
#define ADIS16240_DIAG_STAT_FLASH_UPT_BIT	2
/*                            */
#define ADIS16240_DIAG_STAT_POWER_HIGH_BIT	1
 /*                           */
#define ADIS16240_DIAG_STAT_POWER_LOW_BIT	0

/*          */
#define ADIS16240_GLOB_CMD_RESUME	(1<<8)
#define ADIS16240_GLOB_CMD_SW_RESET	(1<<7)
#define ADIS16240_GLOB_CMD_STANDBY	(1<<2)

#define ADIS16240_ERROR_ACTIVE          (1<<14)

/*                                                     
                            
 */

#define ADIS16240_SCAN_ACC_X	0
#define ADIS16240_SCAN_ACC_Y	1
#define ADIS16240_SCAN_ACC_Z	2
#define ADIS16240_SCAN_SUPPLY	3
#define ADIS16240_SCAN_AUX_ADC	4
#define ADIS16240_SCAN_TEMP	5

#endif /*                  */
