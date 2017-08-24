/*
 * Copyright (c) 2006 Damien Bergamini <damien.bergamini@free.fr>
 * Copyright (c) 2006 Sam Leffler, Errno Consulting
 * Copyright (c) 2007 Christoph Hellwig <hch@lst.de>
 * Copyright (c) 2008-2009 Weongyo Jeong <weongyo@freebsd.org>
 * Copyright (c) 2012 Pontus Fuchs <pontus.fuchs@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*                           */
struct ar5523_fwblock {
	__be32		flags;
#define AR5523_WRITE_BLOCK	(1 << 4)

	__be32	len;
#define AR5523_MAX_FWBLOCK_SIZE	2048

	__be32		total;
	__be32		remain;
	__be32		rxtotal;
	__be32		pad[123];
} __packed;

#define AR5523_MAX_RXCMDSZ	1024
#define AR5523_MAX_TXCMDSZ	1024

struct ar5523_cmd_hdr {
	__be32		len;
	__be32		code;
/*                                                                  */
/*                              */
#define	WDCMSG_HOST_AVAILABLE		0x01
#define WDCMSG_BIND			0x02
#define WDCMSG_TARGET_RESET		0x03
#define WDCMSG_TARGET_GET_CAPABILITY	0x04
#define WDCMSG_TARGET_SET_CONFIG	0x05
#define WDCMSG_TARGET_GET_STATUS	0x06
#define WDCMSG_TARGET_GET_STATS		0x07
#define WDCMSG_TARGET_START		0x08
#define WDCMSG_TARGET_STOP		0x09
#define WDCMSG_TARGET_ENABLE		0x0a
#define WDCMSG_TARGET_DISABLE		0x0b
#define	WDCMSG_CREATE_CONNECTION	0x0c
#define WDCMSG_UPDATE_CONNECT_ATTR	0x0d
#define	WDCMSG_DELETE_CONNECT		0x0e
#define	WDCMSG_SEND			0x0f
#define WDCMSG_FLUSH			0x10
/*                              */
#define	WDCMSG_STATS_UPDATE		0x11
#define	WDCMSG_BMISS			0x12
#define	WDCMSG_DEVICE_AVAIL		0x13
#define	WDCMSG_SEND_COMPLETE		0x14
#define	WDCMSG_DATA_AVAIL		0x15
#define	WDCMSG_SET_PWR_MODE		0x16
#define	WDCMSG_BMISS_ACK		0x17
#define	WDCMSG_SET_LED_STEADY		0x18
#define	WDCMSG_SET_LED_BLINK		0x19
/*               */
#define	WDCMSG_SETUP_BEACON_DESC	0x1a
#define	WDCMSG_BEACON_INIT		0x1b
#define	WDCMSG_RESET_KEY_CACHE		0x1c
#define	WDCMSG_RESET_KEY_CACHE_ENTRY	0x1d
#define	WDCMSG_SET_KEY_CACHE_ENTRY	0x1e
#define	WDCMSG_SET_DECOMP_MASK		0x1f
#define	WDCMSG_SET_REGULATORY_DOMAIN	0x20
#define	WDCMSG_SET_LED_STATE		0x21
#define	WDCMSG_WRITE_ASSOCID		0x22
#define	WDCMSG_SET_STA_BEACON_TIMERS	0x23
#define	WDCMSG_GET_TSF			0x24
#define	WDCMSG_RESET_TSF		0x25
#define	WDCMSG_SET_ADHOC_MODE		0x26
#define	WDCMSG_SET_BASIC_RATE		0x27
#define	WDCMSG_MIB_CONTROL		0x28
#define	WDCMSG_GET_CHANNEL_DATA		0x29
#define	WDCMSG_GET_CUR_RSSI		0x2a
#define	WDCMSG_SET_ANTENNA_SWITCH	0x2b
#define	WDCMSG_USE_SHORT_SLOT_TIME	0x2f
#define	WDCMSG_SET_POWER_MODE		0x30
#define	WDCMSG_SETUP_PSPOLL_DESC	0x31
#define	WDCMSG_SET_RX_MULTICAST_FILTER	0x32
#define	WDCMSG_RX_FILTER		0x33
#define	WDCMSG_PER_CALIBRATION		0x34
#define	WDCMSG_RESET			0x35
#define	WDCMSG_DISABLE			0x36
#define	WDCMSG_PHY_DISABLE		0x37
#define	WDCMSG_SET_TX_POWER_LIMIT	0x38
#define	WDCMSG_SET_TX_QUEUE_PARAMS	0x39
#define	WDCMSG_SETUP_TX_QUEUE		0x3a
#define	WDCMSG_RELEASE_TX_QUEUE		0x3b
#define	WDCMSG_SET_DEFAULT_KEY		0x43

	__u32		priv;	/*                     
                                  */
	__be32		magic;
	__be32		reserved2[4];
};

struct ar5523_cmd_host_available {
	__be32	sw_ver_major;
	__be32	sw_ver_minor;
	__be32	sw_ver_patch;
	__be32	sw_ver_build;
} __packed;

#define	ATH_SW_VER_MAJOR	1
#define	ATH_SW_VER_MINOR	5
#define	ATH_SW_VER_PATCH	0
#define	ATH_SW_VER_BUILD	9999

struct ar5523_chunk {
	u8		seqnum;		/*                              */
	u8		flags;
#define	UATH_CFLAGS_FINAL	0x01	/*                      */
#define	UATH_CFLAGS_RXMSG	0x02	/*                              */
#define	UATH_CFLAGS_DEBUG	0x04	/*               */
	__be16		length;		/*                     */
	/*                    */
} __packed;

/*
                                                                      
 */
struct ar5523_rx_desc {
	__be32	len;		/*                             */
	__be32	code;		/*                   */
	__be32	gennum;		/*                   */
	__be32	status;		/*                       */
#define	UATH_STATUS_OK			0
#define	UATH_STATUS_STOP_IN_PROGRESS	1
#define	UATH_STATUS_CRC_ERR		2
#define	UATH_STATUS_PHY_ERR		3
#define	UATH_STATUS_DECRYPT_CRC_ERR	4
#define	UATH_STATUS_DECRYPT_MIC_ERR	5
#define	UATH_STATUS_DECOMP_ERR		6
#define	UATH_STATUS_KEY_ERR		7
#define	UATH_STATUS_ERR			8
	__be32	tstamp_low;	/*                                   */
	__be32	tstamp_high;	/*                                    */
	__be32	framelen;	/*              */
	__be32	rate;		/*              */
	__be32	antenna;
	__be32	rssi;
	__be32	channel;
	__be32	phyerror;
	__be32	connix;		/*                              */
	__be32	decrypterror;
	__be32	keycachemiss;
	__be32	pad;		/*      */
} __packed;

struct ar5523_tx_desc {
	__be32	msglen;
	u32	msgid;		/*                           */
	__be32	type;		/*                                    */
	__be32	txqid;		/*                       */
#define	UATH_TXQID_MASK		0x0f
#define	UATH_TXQID_MINRATE	0x10	/*                 */
#define	UATH_TXQID_FF		0x20	/*                       */
	__be32	connid;		/*                  */
#define UATH_ID_INVALID	0xffffffff	/*                                 */
	__be32	flags;		/*                              */
#define UATH_TX_NOTIFY	(1 << 24)	/*                               */
	__be32	buflen;		/*                */
} __packed;


#define AR5523_ID_BSS		2
#define AR5523_ID_BROADCAST	0xffffffff

/*                                          */
struct ar5523_write_mac {
	__be32	reg;
	__be32	len;
	u8		data[32];
} __packed;

struct ar5523_cmd_rateset {
	__u8		length;
#define AR5523_MAX_NRATES	32
	__u8		set[AR5523_MAX_NRATES];
};

struct ar5523_cmd_set_associd {		/*                      */
	__be32	defaultrateix;
	__be32	associd;
	__be32	timoffset;
	__be32	turboprime;
	__u8	bssid[6];
} __packed;

/*                                    */
struct ar5523_cmd_reset {
	__be32	flags;		/*               */
#define	UATH_CHAN_TURBO	0x0100
#define	UATH_CHAN_CCK	0x0200
#define	UATH_CHAN_OFDM	0x0400
#define	UATH_CHAN_2GHZ	0x1000
#define	UATH_CHAN_5GHZ	0x2000
	__be32	freq;		/*                   */
	__be32	maxrdpower;
	__be32	cfgctl;
	__be32	twiceantennareduction;
	__be32	channelchange;
	__be32	keeprccontent;
} __packed;

/*                                             */
struct ar5523_cmd_rates {
	__be32	connid;
	__be32	keeprccontent;
	__be32	size;
	struct ar5523_cmd_rateset rateset;
} __packed;

enum {
	WLAN_MODE_NONE = 0,
	WLAN_MODE_11b,
	WLAN_MODE_11a,
	WLAN_MODE_11g,
	WLAN_MODE_11a_TURBO,
	WLAN_MODE_11g_TURBO,
	WLAN_MODE_11a_TURBO_PRIME,
	WLAN_MODE_11g_TURBO_PRIME,
	WLAN_MODE_11a_XR,
	WLAN_MODE_11g_XR,
};

struct ar5523_cmd_connection_attr {
	__be32	longpreambleonly;
	struct ar5523_cmd_rateset	rateset;
	__be32	wlanmode;
} __packed;

/*                                                */
struct ar5523_cmd_create_connection {
	__be32	connid;
	__be32	bssid;
	__be32	size;
	struct ar5523_cmd_connection_attr	connattr;
} __packed;

struct ar5523_cmd_ledsteady {		/*                       */
	__be32	lednum;
#define UATH_LED_LINK		0
#define UATH_LED_ACTIVITY	1
	__be32	ledmode;
#define UATH_LED_OFF	0
#define UATH_LED_ON	1
} __packed;

struct ar5523_cmd_ledblink {		/*                      */
	__be32	lednum;
	__be32	ledmode;
	__be32	blinkrate;
	__be32	slowmode;
} __packed;

struct ar5523_cmd_ledstate {		/*                      */
	__be32	connected;
} __packed;

struct ar5523_cmd_txq_attr {
	__be32	priority;
	__be32	aifs;
	__be32	logcwmin;
	__be32	logcwmax;
	__be32	bursttime;
	__be32	mode;
	__be32	qflags;
} __packed;

struct ar5523_cmd_txq_setup {		/*                       */
	__be32	qid;
	__be32	len;
	struct ar5523_cmd_txq_attr attr;
} __packed;

struct ar5523_cmd_rx_filter {		/*                  */
	__be32	bits;
#define UATH_FILTER_RX_UCAST		0x00000001
#define UATH_FILTER_RX_MCAST		0x00000002
#define UATH_FILTER_RX_BCAST		0x00000004
#define UATH_FILTER_RX_CONTROL		0x00000008
#define UATH_FILTER_RX_BEACON		0x00000010	/*               */
#define UATH_FILTER_RX_PROM		0x00000020	/*                  */
#define UATH_FILTER_RX_PHY_ERR		0x00000040	/*            */
#define UATH_FILTER_RX_PHY_RADAR	0x00000080	/*                  */
#define UATH_FILTER_RX_XR_POOL		0x00000400	/*                */
#define UATH_FILTER_RX_PROBE_REQ	0x00000800
	__be32	op;
#define UATH_FILTER_OP_INIT		0x0
#define UATH_FILTER_OP_SET		0x1
#define UATH_FILTER_OP_CLEAR		0x2
#define UATH_FILTER_OP_TEMP		0x3
#define UATH_FILTER_OP_RESTORE		0x4
} __packed;

enum {
	CFG_NONE,			/*                                  */
	CFG_REG_DOMAIN,			/*                   */
	CFG_RATE_CONTROL_ENABLE,
	CFG_DEF_XMIT_DATA_RATE,		/*                                    */
	CFG_HW_TX_RETRIES,
	CFG_SW_TX_RETRIES,
	CFG_SLOW_CLOCK_ENABLE,
	CFG_COMP_PROC,
	CFG_USER_RTS_THRESHOLD,
	CFG_XR2NORM_RATE_THRESHOLD,
	CFG_XRMODE_SWITCH_COUNT,
	CFG_PROTECTION_TYPE,
	CFG_BURST_SEQ_THRESHOLD,
	CFG_ABOLT,
	CFG_IQ_LOG_COUNT_MAX,
	CFG_MODE_CTS,
	CFG_WME_ENABLED,
	CFG_GPRS_CBR_PERIOD,
	CFG_SERVICE_TYPE,
	/*                                       */
	CFG_MAC_ADDR,
	CFG_DEBUG_EAR,
	CFG_INIT_REGS,
	/*                                         */
	CFG_DEBUG_ID,
	CFG_COMP_WIN_SZ,
	CFG_DIVERSITY_CTL,
	CFG_TP_SCALE,
	CFG_TPC_HALF_DBM5,
	CFG_TPC_HALF_DBM2,
	CFG_OVERRD_TX_POWER,
	CFG_USE_32KHZ_CLOCK,
	CFG_GMODE_PROTECTION,
	CFG_GMODE_PROTECT_RATE_INDEX,
	CFG_GMODE_NON_ERP_PREAMBLE,
	CFG_WDC_TRANSPORT_CHUNK_SIZE,
};

enum {
	/*                                      */
	CAP_NONE,
	CAP_ALL,			/*                  */
	CAP_TARGET_VERSION,
	CAP_TARGET_REVISION,
	CAP_MAC_VERSION,
	CAP_MAC_REVISION,
	CAP_PHY_REVISION,
	CAP_ANALOG_5GHz_REVISION,
	CAP_ANALOG_2GHz_REVISION,
	/*                                            */
	CAP_DEBUG_WDCMSG_SUPPORT,

	CAP_REG_DOMAIN,
	CAP_COUNTRY_CODE,
	CAP_REG_CAP_BITS,

	CAP_WIRELESS_MODES,
	CAP_CHAN_SPREAD_SUPPORT,
	CAP_SLEEP_AFTER_BEACON_BROKEN,
	CAP_COMPRESS_SUPPORT,
	CAP_BURST_SUPPORT,
	CAP_FAST_FRAMES_SUPPORT,
	CAP_CHAP_TUNING_SUPPORT,
	CAP_TURBOG_SUPPORT,
	CAP_TURBO_PRIME_SUPPORT,
	CAP_DEVICE_TYPE,
	CAP_XR_SUPPORT,
	CAP_WME_SUPPORT,
	CAP_TOTAL_QUEUES,
	CAP_CONNECTION_ID_MAX,		/*                                  */

	CAP_LOW_5GHZ_CHAN,
	CAP_HIGH_5GHZ_CHAN,
	CAP_LOW_2GHZ_CHAN,
	CAP_HIGH_2GHZ_CHAN,

	CAP_MIC_AES_CCM,
	CAP_MIC_CKIP,
	CAP_MIC_TKIP,
	CAP_MIC_TKIP_WME,
	CAP_CIPHER_AES_CCM,
	CAP_CIPHER_CKIP,
	CAP_CIPHER_TKIP,

	CAP_TWICE_ANTENNAGAIN_5G,
	CAP_TWICE_ANTENNAGAIN_2G,
};

enum {
	ST_NONE,                    /*                                  */
	ST_ALL,
	ST_SERVICE_TYPE,
	ST_WLAN_MODE,
	ST_FREQ,
	ST_BAND,
	ST_LAST_RSSI,
	ST_PS_FRAMES_DROPPED,
	ST_CACHED_DEF_ANT,
	ST_COUNT_OTHER_RX_ANT,
	ST_USE_FAST_DIVERSITY,
	ST_MAC_ADDR,
	ST_RX_GENERATION_NUM,
	ST_TX_QUEUE_DEPTH,
	ST_SERIAL_NUMBER,
	ST_WDC_TRANSPORT_CHUNK_SIZE,
};

enum {
	TARGET_DEVICE_AWAKE,
	TARGET_DEVICE_SLEEP,
	TARGET_DEVICE_PWRDN,
	TARGET_DEVICE_PWRSAVE,
	TARGET_DEVICE_SUSPEND,
	TARGET_DEVICE_RESUME,
};

/*                                                                          */
#define IEEE80211_2ADDR_LEN	16

#define AR5523_MIN_RXBUFSZ				\
	(((sizeof(__be32) + IEEE80211_2ADDR_LEN +	\
	   sizeof(struct ar5523_rx_desc)) + 3) & ~3)
