#ifndef __FM_MAIN_H__
#define __FM_MAIN_H__
#include "fm_typedef.h"
#include "fm_dbg.h"
#include "fm_err.h"
#include "fm_rds.h"
#include "fm_eint.h"
#include "fm_link.h"
#include "fm_interface.h"
#include "fm_stdlib.h"
#include "fm_private.h"

#define FM_NAME             "fm"
#define FM_DEVICE_NAME      "/dev/fm"

#define FM_VOL_MAX           0x2B	/*                 */
#define FM_TIMER_TIMEOUT_DEFAULT 1000
#define FM_TIMER_TIMEOUT_MIN 1000
#define FM_TIMER_TIMEOUT_MAX 1000000
/*       */
#define FM_TX_PWR_LEVEL_MAX  120	/*                                                             */

#define FM_TX_PWR_CTRL_INVAL_DEFAULT 10
#define FM_TX_PWR_CTRL_INVAL_MIN 5
#define FM_TX_PWR_CTRL_INVAL_MAX 10000

#define FM_TX_VCO_OFF_DEFAULT 5
#define FM_TX_VCO_OFF_MIN 1
#define FM_TX_VCO_OFF_MAX 10000

#define FM_TX_VCO_ON_DEFAULT 100
#define FM_TX_VCO_ON_MIN 10
#define FM_TX_VCO_ON_MAX 10000

#define FM_GPS_RTC_AGE_TH       2
#define FM_GPS_RTC_DRIFT_TH     0
#define FM_GPS_RTC_TIME_DIFF_TH 10
#define FM_GPS_RTC_RETRY_CNT    1
#define FM_GPS_RTC_DRIFT_MAX 5000
enum {
	FM_GPS_RTC_INFO_OLD = 0,
	FM_GPS_RTC_INFO_NEW = 1,
	FM_GPS_RTC_INFO_MAX
};

typedef enum {
	FM_OVER_BT_DISABLE = 0,
	FM_OVER_BT_ENABLE
} fm_over_bt_enable_state;

#define FM_RDS_ENABLE		0x01	/*                              */
#define FM_RDS_DATA_READY   (1 << 0)

/*       */
#define FM_SUCCESS      0
#define FM_FAILED       1
#define FM_EPARM        2
#define FM_BADSTATUS    3
#define FM_TUNE_FAILED  4
#define FM_SEEK_FAILED  5
#define FM_BUSY         6
#define FM_SCAN_FAILED  7

struct fm_tune_parm {
	fm_u8 err;
	fm_u8 band;
	fm_u8 space;
	fm_u8 hilo;
	fm_u16 freq;		/*                  */
};

struct fm_seek_parm {
	fm_u8 err;
	fm_u8 band;
	fm_u8 space;
	fm_u8 hilo;
	fm_u8 seekdir;
	fm_u8 seekth;
	fm_u16 freq;		/*                  */
};

#ifdef CONFIG_MTK_FM_50KHZ_SUPPORT
struct fm_scan_parm {
	fm_u8 err;
	fm_u8 band;
	fm_u8 space;
	fm_u8 hilo;
	fm_u16 freq;		/*               */
	fm_u16 ScanTBL[26];	/*                            */
	fm_u16 ScanTBLSize;	/*                  */
};
#else
struct fm_scan_parm {
	fm_u8 err;
	fm_u8 band;
	fm_u8 space;
	fm_u8 hilo;
	fm_u16 freq;		/*               */
	fm_u16 ScanTBL[16];	/*                            */
	fm_u16 ScanTBLSize;	/*                  */
};
#endif

struct fm_cqi {
	fm_s32 ch;
	fm_s32 rssi;
	fm_s32 reserve;
};

struct fm_cqi_req {
	fm_u16 ch_num;
	fm_s32 buf_size;
	fm_s8 *cqi_buf;
};

struct fm_ch_rssi {
	fm_u16 freq;
	fm_s32 rssi;
};

enum fm_scan_cmd_t {
	FM_SCAN_CMD_INIT = 0,
	FM_SCAN_CMD_START,
	FM_SCAN_CMD_GET_NUM,
	FM_SCAN_CMD_GET_CH,
	FM_SCAN_CMD_GET_RSSI,
	FM_SCAN_CMD_GET_CH_RSSI,
	FM_SCAN_CMD_MAX
};

struct fm_scan_t {
	enum fm_scan_cmd_t cmd;
	fm_s32 ret;		/*                             */
	fm_u16 lower;		/*                                 */
	fm_u16 upper;		/*                                   */
	fm_s32 space;		/*                                  */
	fm_s32 num;		/*                      */
	void *priv;
	fm_s32 sr_size;		/*                                  */
	union {
		fm_u16 *ch_buf;	/*                */
		fm_s32 *rssi_buf;	/*             */
		struct fm_ch_rssi *ch_rssi_buf;	/*                         */
	} sr;
};

struct fm_seek_t {
	fm_s32 ret;		/*                             */
	fm_u16 freq;
	fm_u16 lower;		/*                                 */
	fm_u16 upper;		/*                                   */
	fm_s32 space;		/*                                  */
	fm_s32 dir;		/*                */
	fm_s32 th;		/*                                   */
	void *priv;
};

struct fm_tune_t {
	fm_s32 ret;		/*                             */
	fm_u16 freq;
	fm_u16 lower;		/*                                 */
	fm_u16 upper;		/*                                   */
	fm_s32 space;		/*                                  */
	void *priv;
};


#ifdef CONFIG_MTK_FM_50KHZ_SUPPORT
struct fm_rssi_req {
	fm_u16 num;
	fm_u16 read_cnt;
	struct fm_ch_rssi cr[26 * 16];
};
#else
struct fm_rssi_req {
	fm_u16 num;
	fm_u16 read_cnt;
	struct fm_ch_rssi cr[16 * 16];
};
#endif

struct fm_rds_tx_parm {
	fm_u8 err;
	fm_u16 pi;
	fm_u16 ps[12];		/*      */
	fm_u16 other_rds[87];	/*                   */
	fm_u8 other_rds_cnt;	/*                  */
};

typedef struct fm_rds_tx_req {
	unsigned char pty;	/*              */
	unsigned char rds_rbds;	/*               */
	unsigned char dyn_pty;	/*                     */
	unsigned short pi_code;	/*            */
	unsigned char ps_buf[8];	/*               */
	unsigned char ps_len;	/*                              */
	unsigned char af;	/*                                           */
	unsigned char ah;	/*                              */
	unsigned char stereo;	/*                  */
	unsigned char compress;	/*                             */
	unsigned char tp;	/*                              */
	unsigned char ta;	/*                                   */
	unsigned char speech;	/*                   */
} fm_rds_tx_req;

#define TX_SCAN_MAX 10
#define TX_SCAN_MIN 1

struct fm_tx_scan_parm {
	fm_u8 err;
	fm_u8 band;		/*             */
	fm_u8 space;
	fm_u8 hilo;
	fm_u16 freq;		/*                                                                     */
	fm_u8 scandir;
	fm_u16 ScanTBL[TX_SCAN_MAX];	/*                            */
	fm_u16 ScanTBLSize;	/*                                         */
};

struct fm_gps_rtc_info {
	fm_s32 err;		/*                                           */
	fm_s32 retryCnt;	/*                                */
	fm_s32 ageThd;		/*                                */
	fm_s32 driftThd;	/*                         */
	struct timeval tvThd;	/*                           */
	fm_s32 age;		/*                      */
	fm_s32 drift;		/*               */
	union {
		unsigned long stamp;	/*                       */
		struct timeval tv;	/*                         */
	};
	fm_s32 flag;		/*         */
};

typedef struct {
	fm_s32 freq;
	fm_s32 rssi;
} fm_desense_check_t;

typedef struct {
	uint16_t lower;		/*                                 */
	uint16_t upper;		/*                                   */
	int space;		/*                                      */
	int cycle;		/*              */
} fm_full_cqi_log_t;

typedef enum {
	FM_RX = 0,
	FM_TX = 1
} FM_PWR_T;

struct fm_ctl_parm {
	fm_u8 err;
	fm_u8 addr;
	fm_u16 val;
	fm_u16 rw_flag;		/*                 */
};
struct fm_em_parm {
	fm_u16 group_idx;
	fm_u16 item_idx;
	fm_u32 item_value;
};
struct fm_top_rw_parm {
	fm_u8 err;
	fm_u8 rw_flag;		/*                 */
	fm_u16 addr;
	fm_u32 val;
};
struct fm_host_rw_parm {
	fm_u8 err;
	fm_u8 rw_flag;		/*                 */
	fm_u32 addr;
	fm_u32 val;
};

enum {
	FM_SUBSYS_RST_OFF,
	FM_SUBSYS_RST_START,
	FM_SUBSYS_RST_END,
	FM_SUBSYS_RST_MAX
};
enum {
	FM_TX_PWR_CTRL_DISABLE,
	FM_TX_PWR_CTRL_ENABLE,
	FM_TX_PWR_CTRL_MAX
};

enum {
	FM_TX_RTC_CTRL_DISABLE,
	FM_TX_RTC_CTRL_ENABLE,
	FM_TX_RTC_CTRL_MAX
};

enum {
	FM_TX_DESENSE_DISABLE,
	FM_TX_DESENSE_ENABLE,
	FM_TX_DESENSE_MAX
};

struct fm_softmute_tune_t {
	fm_s32 rssi;		/*                         */
	fm_u16 freq;		/*                   */
	fm_bool valid;		/*                                              */
};
struct fm_search_threshold_t {
	fm_s32 th_type;		/*                                 */
	fm_s32 th_val;		/*                 */
	fm_s32 reserve;
};

typedef struct
{
	int which;
	bool stat;
}fm_status_t;

/*                      */
extern fm_s32 fm_env_setup(void);
extern fm_s32 fm_env_destroy(void);
extern struct fm *fm_dev_init(fm_u32 arg);
extern fm_s32 fm_dev_destroy(struct fm *fm);


/*                    */
extern enum fm_pwr_state fm_pwr_state_get(struct fm *fmp);
extern enum fm_pwr_state fm_pwr_state_set(struct fm *fmp, enum fm_pwr_state sta);
extern fm_s32 fm_open(struct fm *fmp);
extern fm_s32 fm_close(struct fm *fmp);
extern fm_s32 fm_rds_read(struct fm *fmp, fm_s8 *dst, fm_s32 len);
extern fm_s32 fm_powerup(struct fm *fm, struct fm_tune_parm *parm);
extern fm_s32 fm_powerdown(struct fm *fm, int type);
extern fm_s32 fm_seek(struct fm *fm, struct fm_seek_parm *parm);
extern fm_s32 fm_scan(struct fm *fm, struct fm_scan_parm *parm);
extern fm_s32 fm_cqi_get(struct fm *fm, fm_s32 ch_num, fm_s8 *buf, fm_s32 buf_size);
extern fm_s32 fm_get_hw_info(struct fm *pfm, struct fm_hw_info *req);
extern fm_s32 fm_hwscan_stop(struct fm *fm);
extern fm_s32 fm_ana_switch(struct fm *fm, fm_s32 antenna);
extern fm_s32 fm_setvol(struct fm *fm, fm_u32 vol);
extern fm_s32 fm_getvol(struct fm *fm, fm_u32 *vol);
extern fm_s32 fm_mute(struct fm *fm, fm_u32 bmute);
extern fm_s32 fm_getrssi(struct fm *fm, fm_s32 *rssi);
extern fm_s32 fm_reg_read(struct fm *fm, fm_u8 addr, fm_u16 *val);
extern fm_s32 fm_reg_write(struct fm *fm, fm_u8 addr, fm_u16 val);
extern fm_s32 fm_top_read(struct fm *fm, fm_u16 addr, fm_u32 *val);
extern fm_s32 fm_top_write(struct fm *fm, fm_u16 addr, fm_u32 val);
extern fm_s32 fm_host_read(struct fm *fm, fm_u32 addr, fm_u32 *val);
extern fm_s32 fm_host_write(struct fm *fm, fm_u32 addr, fm_u32 val);
extern fm_s32 fm_chipid_get(struct fm *fm, fm_u16 *chipid);
extern fm_s32 fm_monostereo_get(struct fm *fm, fm_u16 *ms);
extern fm_s32 fm_monostereo_set(struct fm *fm, fm_s32 ms);
extern fm_s32 fm_pamd_get(struct fm *fm, fm_u16 *pamd);
extern fm_s32 fm_caparray_get(struct fm *fm, fm_s32 *ca);
extern fm_s32 fm_em_test(struct fm *fm, fm_u16 group, fm_u16 item, fm_u32 val);
extern fm_s32 fm_rds_onoff(struct fm *fm, fm_u16 rdson_off);
extern fm_s32 fm_rds_good_bc_get(struct fm *fm, fm_u16 *gbc);
extern fm_s32 fm_rds_bad_bc_get(struct fm *fm, fm_u16 *bbc);
extern fm_s32 fm_rds_bler_ratio_get(struct fm *fm, fm_u16 *bbr);
extern fm_s32 fm_rds_group_cnt_get(struct fm *fm, struct rds_group_cnt_t *dst);
extern fm_s32 fm_rds_group_cnt_reset(struct fm *fm);
extern fm_s32 fm_rds_log_get(struct fm *fm, struct rds_rx_t *dst, fm_s32 *dst_len);
extern fm_s32 fm_rds_block_cnt_reset(struct fm *fm);
extern fm_s32 fm_i2s_set(struct fm *fm, fm_s32 onoff, fm_s32 mode, fm_s32 sample);
extern fm_s32 fm_get_i2s_info(struct fm *pfm, struct fm_i2s_info *req);
extern fm_s32 fm_tune(struct fm *fm, struct fm_tune_parm *parm);
extern fm_s32 fm_is_dese_chan(struct fm *pfm, fm_u16 freq);
extern fm_s32 fm_desense_check(struct fm *pfm, fm_u16 freq, fm_s32 rssi);
extern fm_s32 fm_sys_state_get(struct fm *fmp);
extern fm_s32 fm_sys_state_set(struct fm *fmp, fm_s32 sta);
extern fm_s32 fm_set_stat(struct fm *fmp, int which, bool stat);
extern fm_s32 fm_get_stat(struct fm *fmp, int which, bool *stat);
extern fm_s32 fm_subsys_reset(struct fm *fm);

extern fm_s32 fm_scan_new(struct fm *fm, struct fm_scan_t *parm);
extern fm_s32 fm_seek_new(struct fm *fm, struct fm_seek_t *parm);
extern fm_s32 fm_tune_new(struct fm *fm, struct fm_tune_t *parm);

extern fm_s32 fm_cust_config_setup(fm_s8 *filename);
extern fm_s32 fm_cqi_log(void);
extern fm_s32 fm_soft_mute_tune(struct fm *fm, struct fm_softmute_tune_t *parm);
extern fm_s32 fm_pre_search(struct fm *fm);
extern fm_s32 fm_restore_search(struct fm *fm);

extern fm_s32 fm_dump_reg(void);
extern fm_s32 fm_get_gps_rtc_info(struct fm_gps_rtc_info *src);
extern fm_s32 fm_over_bt(struct fm *fm, fm_s32 flag);
extern fm_s32 fm_set_search_th(struct fm *fm, struct fm_search_threshold_t parm);
extern fm_s32 fm_get_aud_info(fm_audio_info_t *data);
extern fm_s32 fm_read_freq_info(struct fm *fm, fm_u16 *freq);
/*           */
extern fm_s32 fm_tx_support(struct fm *fm, fm_s32 *support);

extern fm_s32 fm_powerup_tx(struct fm *fm, struct fm_tune_parm *parm);
extern fm_s32 fm_tune_tx(struct fm *fm, struct fm_tune_parm *parm);
extern fm_s32 fm_powerdowntx(struct fm *fm);
extern fm_s32 fm_rds_tx(struct fm *fm, struct fm_rds_tx_parm *parm);
extern fm_s32 fm_rdstx_support(struct fm *fm, fm_s32 *support);
extern fm_s32 fm_rdstx_enable(struct fm *fm, fm_s32 enable);
extern fm_s32 fm_tx_scan(struct fm *fm, struct fm_tx_scan_parm *parm);
fm_s32 fm_full_cqi_logger(fm_full_cqi_log_t *setting);

#endif				/*               */
