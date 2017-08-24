/*
 * Copyright (c) 2010 Broadcom Corporation
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _BRCM_PUB_H_
#define _BRCM_PUB_H_

#include <linux/bcma/bcma.h>
#include <brcmu_wifi.h>
#include "types.h"
#include "defs.h"

#define	BRCMS_NUMRATES	16	/*                             */

/*           */
#define	PHY_TYPE_A	0	/*            */
#define	PHY_TYPE_G	2	/*            */
#define	PHY_TYPE_N	4	/*            */
#define	PHY_TYPE_LP	5	/*                          */
#define	PHY_TYPE_SSN	6	/*                          */
#define	PHY_TYPE_LCN	8	/*                          */
#define	PHY_TYPE_LCNXN	9	/*                     */
#define	PHY_TYPE_HT	7	/*                     */

/*    */
#define BRCMS_10_MHZ	10	/*                              */
#define BRCMS_20_MHZ	20	/*                              */
#define BRCMS_40_MHZ	40	/*                              */

#define	BRCMS_RSSI_MINVAL	-200	/*                                  */
#define	BRCMS_RSSI_NO_SIGNAL	-91	/*                                */
#define	BRCMS_RSSI_VERY_LOW	-80	/*                          */
#define	BRCMS_RSSI_LOW		-70	/*                     */
#define	BRCMS_RSSI_GOOD		-68	/*                      */
#define	BRCMS_RSSI_VERY_GOOD	-58	/*                           */
#define	BRCMS_RSSI_EXCELLENT	-57	/*                           */

/*                                                                       
                                                     
 */
#define BRCMS_TXPWR_MAX		(127)	/*                    */

/*                          */
#define	BRCMS_RATE_FLAG	0x80	/*                                     */
#define	BRCMS_RATE_MASK	0x7f	/*                                     */

/*                                            */
#define	ANT_RX_DIV_FORCE_0	0	/*               */
#define	ANT_RX_DIV_FORCE_1	1	/*               */
#define	ANT_RX_DIV_START_1	2	/*                        */
#define	ANT_RX_DIV_START_0	3	/*                        */
#define	ANT_RX_DIV_ENABLE	3	/*                                   */
/*                        */
#define ANT_RX_DIV_DEF		ANT_RX_DIV_START_0

/*                                            */
/*                                     */
#define ANT_TX_FORCE_0		0
/*                                    */
#define ANT_TX_FORCE_1		1
/*                                  */
#define ANT_TX_LAST_RX		3
/*                                     */
#define ANT_TX_DEF		3

/*                 */
/*                       */
#define TXCHAIN_DEF		0x1
/*                                      */
#define TXCHAIN_DEF_NPHY	0x3
/*                                      */
#define TXCHAIN_DEF_HTPHY	0x7
/*                       */
#define RXCHAIN_DEF		0x1
/*                                      */
#define RXCHAIN_DEF_NPHY	0x3
/*                                      */
#define RXCHAIN_DEF_HTPHY	0x7
/*                   */
#define ANTSWITCH_NONE		0
/*                                 */
#define ANTSWITCH_TYPE_1	1
/*                                  */
#define ANTSWITCH_TYPE_2	2
/*                              */
#define ANTSWITCH_TYPE_3	3

#define RXBUFSZ		PKTBUFSZ

#define MAX_STREAMS_SUPPORTED	4	/*                                 */

struct brcm_rateset {
	/*                     */
	u32 count;
	/*                                              */
	u8 rates[WL_NUMRATES];
};

struct brcms_c_rateset {
	uint count;		/*                            */
	 /*                                              */
	u8 rates[BRCMS_NUMRATES];
	u8 htphy_membership;	/*                   */
	u8 mcs[MCSSET_LEN];	/*                             */
};

/*                                                                      
                                      
 */
#define AMPDU_DEF_MPDU_DENSITY	6	/*                                    */

/*                       */
struct brcms_bss_info {
	u8 BSSID[ETH_ALEN];	/*               */
	u16 flags;		/*                               */
	u8 SSID_len;		/*                    */
	u8 SSID[32];		/*             */
	s16 RSSI;		/*                                  */
	s16 SNR;		/*                          */
	u16 beacon_period;	/*                 */
	u16 chanspec;	/*                                   */
	struct brcms_c_rateset rateset;	/*                 */
};

#define MAC80211_PROMISC_BCNS	(1 << 0)
#define MAC80211_SCAN		(1 << 1)

/*
                                                   
                                 
 */
struct brcms_pub {
	struct brcms_c_info *wlc;
	struct ieee80211_hw *ieee_hw;
	struct scb_ampdu *global_ampdu;
	uint mac80211_state;
	uint unit;		/*                        */
	uint corerev;		/*               */
	struct si_pub *sih;	/*                                      */
	bool up;		/*                          */
	bool hw_off;		/*           */
	bool hw_up;		/*                     */
	bool _piomode;		/*                  */
	uint _nbands;		/*                   */
	uint now;		/*                   */

	bool delayed_down;	/*              */
	bool associated;	/*                                 */
	/*                                            */
	bool _ampdu;		/*                      */
	u8 _n_enab;		/*                            */

	u8 cur_etheraddr[ETH_ALEN];	/*                            */

	u32 radio_disabled;	/*                                       */

	u16 boardrev;	/*                               */
	u8 sromrev;		/*                       */
	char srom_ccode[BRCM_CNTRY_BUF_SZ];	/*                      */
	u32 boardflags;	/*                                */
	u32 boardflags2;	/*                                  */
	bool phy_11ncapable;	/*                                  */

	struct wl_cnt *_cnt;	/*                              */
	struct dentry *dbgfs_dir;
};

enum wlc_par_id {
	IOV_MPC = 1,
	IOV_RTSTHRESH,
	IOV_QTXPOWER,
	IOV_BCN_LI_BCN		/*                                        */
};

/*                                              
                                                
                                                
 */

#define ENAB_1x1	0x01
#define ENAB_2x2	0x02
#define ENAB_3x3	0x04
#define ENAB_4x4	0x08
#define SUPPORT_11N	(ENAB_1x1|ENAB_2x2)
#define SUPPORT_HT	(ENAB_1x1|ENAB_2x2|ENAB_3x3)

/*               */
#define AMPDU_AGG_HOST	1

/*                           */
#define	BRCMS_PROT_G_SPEC		1	/*                   */
#define	BRCMS_PROT_G_OVR		2	/*                      */
#define	BRCMS_PROT_G_USER		3	/*                         */
#define	BRCMS_PROT_OVERLAP	4	/*         */
#define	BRCMS_PROT_N_USER		10	/*                         */
#define	BRCMS_PROT_N_CFG		11	/*              */
#define	BRCMS_PROT_N_CFG_OVR	12	/*                       */
#define	BRCMS_PROT_N_NONGF	13	/*                   */
#define	BRCMS_PROT_N_NONGF_OVR	14	/*                            */
#define	BRCMS_PROT_N_PAM_OVR	15	/*                     */
#define	BRCMS_PROT_N_OBSS		16	/*                     */

/*
                                                 
  
                 
                           
                 
                 
             
                                             
                                 
                 
                  
             
                                              
                                   
                           
                  
                   
                                             
                                 
                 
                
                    
                                                              
                           
                             
            
                             
                                                 
                 
                  
 */
#define GMODE_LEGACY_B		0
#define GMODE_AUTO		1
#define GMODE_ONLY		2
#define GMODE_B_DEFERRED	3
#define GMODE_PERFORMANCE	4
#define GMODE_LRS		5
#define GMODE_MAX		6

/*                                                      */
#define HIGHEST_SINGLE_STREAM_MCS	7

#define	MAXBANDS		2	/*                   */

/*                                      */
#define ANT_SELCFG_MAX		4

struct brcms_antselcfg {
	u8 ant_config[ANT_SELCFG_MAX];	/*                       */
	u8 num_antcfg;	/*                                            */
};

/*                                 */
extern struct brcms_c_info *
brcms_c_attach(struct brcms_info *wl, struct bcma_device *core, uint unit,
	       bool piomode, uint *perr);
extern uint brcms_c_detach(struct brcms_c_info *wlc);
extern int brcms_c_up(struct brcms_c_info *wlc);
extern uint brcms_c_down(struct brcms_c_info *wlc);

extern bool brcms_c_chipmatch(struct bcma_device *core);
extern void brcms_c_init(struct brcms_c_info *wlc, bool mute_tx);
extern void brcms_c_reset(struct brcms_c_info *wlc);

extern void brcms_c_intrson(struct brcms_c_info *wlc);
extern u32 brcms_c_intrsoff(struct brcms_c_info *wlc);
extern void brcms_c_intrsrestore(struct brcms_c_info *wlc, u32 macintmask);
extern bool brcms_c_intrsupd(struct brcms_c_info *wlc);
extern bool brcms_c_isr(struct brcms_c_info *wlc);
extern bool brcms_c_dpc(struct brcms_c_info *wlc, bool bounded);
extern bool brcms_c_sendpkt_mac80211(struct brcms_c_info *wlc,
				     struct sk_buff *sdu,
				     struct ieee80211_hw *hw);
extern bool brcms_c_aggregatable(struct brcms_c_info *wlc, u8 tid);
extern void brcms_c_protection_upd(struct brcms_c_info *wlc, uint idx,
				   int val);
extern int brcms_c_get_header_len(void);
extern void brcms_c_set_addrmatch(struct brcms_c_info *wlc,
				  int match_reg_offset,
				  const u8 *addr);
extern void brcms_c_wme_setparams(struct brcms_c_info *wlc, u16 aci,
			      const struct ieee80211_tx_queue_params *arg,
			      bool suspend);
extern struct brcms_pub *brcms_c_pub(struct brcms_c_info *wlc);
extern void brcms_c_ampdu_flush(struct brcms_c_info *wlc,
			    struct ieee80211_sta *sta, u16 tid);
extern void brcms_c_ampdu_tx_operational(struct brcms_c_info *wlc, u8 tid,
					 u8 ba_wsize, uint max_rx_ampdu_bytes);
extern int brcms_c_module_register(struct brcms_pub *pub,
				   const char *name, struct brcms_info *hdl,
				   int (*down_fn)(void *handle));
extern int brcms_c_module_unregister(struct brcms_pub *pub, const char *name,
				     struct brcms_info *hdl);
extern void brcms_c_suspend_mac_and_wait(struct brcms_c_info *wlc);
extern void brcms_c_enable_mac(struct brcms_c_info *wlc);
extern void brcms_c_associate_upd(struct brcms_c_info *wlc, bool state);
extern void brcms_c_scan_start(struct brcms_c_info *wlc);
extern void brcms_c_scan_stop(struct brcms_c_info *wlc);
extern int brcms_c_get_curband(struct brcms_c_info *wlc);
extern int brcms_c_set_channel(struct brcms_c_info *wlc, u16 channel);
extern int brcms_c_set_rate_limit(struct brcms_c_info *wlc, u16 srl, u16 lrl);
extern void brcms_c_get_current_rateset(struct brcms_c_info *wlc,
				 struct brcm_rateset *currs);
extern int brcms_c_set_rateset(struct brcms_c_info *wlc,
					struct brcm_rateset *rs);
extern int brcms_c_set_beacon_period(struct brcms_c_info *wlc, u16 period);
extern u16 brcms_c_get_phy_type(struct brcms_c_info *wlc, int phyidx);
extern void brcms_c_set_shortslot_override(struct brcms_c_info *wlc,
				    s8 sslot_override);
extern void brcms_c_set_beacon_listen_interval(struct brcms_c_info *wlc,
					u8 interval);
extern u64 brcms_c_tsf_get(struct brcms_c_info *wlc);
extern void brcms_c_tsf_set(struct brcms_c_info *wlc, u64 tsf);
extern int brcms_c_set_tx_power(struct brcms_c_info *wlc, int txpwr);
extern int brcms_c_get_tx_power(struct brcms_c_info *wlc);
extern bool brcms_c_check_radio_disabled(struct brcms_c_info *wlc);
extern void brcms_c_mute(struct brcms_c_info *wlc, bool on);
extern bool brcms_c_tx_flush_completed(struct brcms_c_info *wlc);
extern void brcms_c_start_station(struct brcms_c_info *wlc, u8 *addr);
extern void brcms_c_start_ap(struct brcms_c_info *wlc, u8 *addr,
			     const u8 *bssid, u8 *ssid, size_t ssid_len);
extern void brcms_c_start_adhoc(struct brcms_c_info *wlc, u8 *addr);
extern void brcms_c_update_beacon(struct brcms_c_info *wlc);
extern void brcms_c_set_new_beacon(struct brcms_c_info *wlc,
				   struct sk_buff *beacon, u16 tim_offset,
				   u16 dtim_period);
extern void brcms_c_set_new_probe_resp(struct brcms_c_info *wlc,
				       struct sk_buff *probe_resp);
extern void brcms_c_enable_probe_resp(struct brcms_c_info *wlc, bool enable);
extern void brcms_c_set_ssid(struct brcms_c_info *wlc, u8 *ssid,
			     size_t ssid_len);

#endif				/*              */
