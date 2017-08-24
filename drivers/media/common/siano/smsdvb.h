/***********************************************************************
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

struct smsdvb_debugfs;
struct smsdvb_client_t;

typedef void (*sms_prt_dvb_stats_t)(struct smsdvb_debugfs *debug_data,
				    struct sms_stats *p);

typedef void (*sms_prt_isdb_stats_t)(struct smsdvb_debugfs *debug_data,
				     struct sms_isdbt_stats *p);

typedef void (*sms_prt_isdb_stats_ex_t)
			(struct smsdvb_debugfs *debug_data,
			 struct sms_isdbt_stats_ex *p);


struct smsdvb_client_t {
	struct list_head entry;

	struct smscore_device_t *coredev;
	struct smscore_client_t *smsclient;

	struct dvb_adapter      adapter;
	struct dvb_demux        demux;
	struct dmxdev           dmxdev;
	struct dvb_frontend     frontend;

	fe_status_t             fe_status;

	struct completion       tune_done;
	struct completion       stats_done;

	int last_per;

	int legacy_ber, legacy_per;

	int event_fe_state;
	int event_unc_state;

	unsigned long		get_stats_jiffies;

	int			feed_users;
	bool			has_tuned;

	/*                    */
	struct dentry		*debugfs;

	struct smsdvb_debugfs	*debug_data;

	sms_prt_dvb_stats_t	prt_dvb_stats;
	sms_prt_isdb_stats_t	prt_isdb_stats;
	sms_prt_isdb_stats_ex_t	prt_isdb_stats_ex;
};

/*
                                                     
                                 
                                                                          
 */
struct RECEPTION_STATISTICS_PER_SLICES_S {
	u32 result;
	u32 snr;
	s32 in_band_power;
	u32 ts_packets;
	u32 ets_packets;
	u32 constellation;
	u32 hp_code;
	u32 tps_srv_ind_lp;
	u32 tps_srv_ind_hp;
	u32 cell_id;
	u32 reason;
	u32 request_id;
	u32 modem_state;		/*                                    */

	u32 ber;		/*                         */
	s32 RSSI;		/*     */
	s32 carrier_offset;	/*                            */

	u32 is_rf_locked;		/*                            */
	u32 is_demod_locked;	/*                            */

	u32 ber_bit_count;	/*                            */
	u32 ber_error_count;	/*                               */

	s32 MRC_SNR;		/*    */
	s32 mrc_in_band_pwr;	/*                      */
	s32 MRC_RSSI;		/*     */
};

/*                       */
#ifdef CONFIG_SMS_SIANO_DEBUGFS

int smsdvb_debugfs_create(struct smsdvb_client_t *client);
void smsdvb_debugfs_release(struct smsdvb_client_t *client);
int smsdvb_debugfs_register(void);
void smsdvb_debugfs_unregister(void);

#else

static inline int smsdvb_debugfs_create(struct smsdvb_client_t *client)
{
	return 0;
}

static inline void smsdvb_debugfs_release(struct smsdvb_client_t *client) {}

static inline int smsdvb_debugfs_register(void)
{
	return 0;
};

static inline void smsdvb_debugfs_unregister(void) {};

#endif

