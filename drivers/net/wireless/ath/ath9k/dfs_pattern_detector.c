/*
 * Copyright (c) 2012 Neratec Solutions AG
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

#include <linux/slab.h>
#include <linux/export.h>

#include "dfs_pattern_detector.h"
#include "dfs_pri_detector.h"
#include "ath9k.h"

/*
                                                                 
                                           
 */
#define PRI_TOLERANCE	16

/* 
                                                                             
                                 
                                                    
                                  
 */
struct radar_types {
	enum nl80211_dfs_regions region;
	u32 num_radar_types;
	const struct radar_detector_specs *radar_types;
};

/*                                                  */
#define MIN_PPB_THRESH	50
#define PPB_THRESH(PPB) ((PPB * MIN_PPB_THRESH + 50) / 100)
#define PRF2PRI(PRF) ((1000000 + PRF / 2) / PRF)
/*                                     */
#define WIDTH_TOLERANCE 5
#define WIDTH_LOWER(X) ((X*(100-WIDTH_TOLERANCE)+50)/100)
#define WIDTH_UPPER(X) ((X*(100+WIDTH_TOLERANCE)+50)/100)

#define ETSI_PATTERN(ID, WMIN, WMAX, PMIN, PMAX, PRF, PPB)	\
{								\
	ID, WIDTH_LOWER(WMIN), WIDTH_UPPER(WMAX),		\
	(PRF2PRI(PMAX) - PRI_TOLERANCE),			\
	(PRF2PRI(PMIN) * PRF + PRI_TOLERANCE), PRF, PPB * PRF,	\
	PPB_THRESH(PPB), PRI_TOLERANCE,				\
}

/*                                                  */
static const struct radar_detector_specs etsi_radar_ref_types_v15[] = {
	ETSI_PATTERN(0,  0,  1,  700,  700, 1, 18),
	ETSI_PATTERN(1,  0,  5,  200, 1000, 1, 10),
	ETSI_PATTERN(2,  0, 15,  200, 1600, 1, 15),
	ETSI_PATTERN(3,  0, 15, 2300, 4000, 1, 25),
	ETSI_PATTERN(4, 20, 30, 2000, 4000, 1, 20),
	ETSI_PATTERN(5,  0,  2,  300,  400, 3, 10),
	ETSI_PATTERN(6,  0,  2,  400, 1200, 3, 15),
};

static const struct radar_types etsi_radar_types_v15 = {
	.region			= NL80211_DFS_ETSI,
	.num_radar_types	= ARRAY_SIZE(etsi_radar_ref_types_v15),
	.radar_types		= etsi_radar_ref_types_v15,
};

/*                                                           */
static const struct radar_types *dfs_domains[] = {
	&etsi_radar_types_v15,
};

/* 
                                                                        
                           
                                                                          
 */
static const struct radar_types *
get_dfs_domain_radar_types(enum nl80211_dfs_regions region)
{
	u32 i;
	for (i = 0; i < ARRAY_SIZE(dfs_domains); i++) {
		if (dfs_domains[i]->region == region)
			return dfs_domains[i];
	}
	return NULL;
}

/* 
                                                                
                   
                                                    
                                                                           
  
                                                                              
                                                                             
                                                   
 */
struct channel_detector {
	struct list_head head;
	u16 freq;
	struct pri_detector **detectors;
};

/*                                                                     */
static void channel_detector_reset(struct dfs_pattern_detector *dpd,
				   struct channel_detector *cd)
{
	u32 i;
	if (cd == NULL)
		return;
	for (i = 0; i < dpd->num_radar_types; i++)
		cd->detectors[i]->reset(cd->detectors[i], dpd->last_pulse_ts);
}

/*                                      */
static void channel_detector_exit(struct dfs_pattern_detector *dpd,
				  struct channel_detector *cd)
{
	u32 i;
	if (cd == NULL)
		return;
	list_del(&cd->head);
	for (i = 0; i < dpd->num_radar_types; i++) {
		struct pri_detector *de = cd->detectors[i];
		if (de != NULL)
			de->exit(de);
	}
	kfree(cd->detectors);
	kfree(cd);
}

static struct channel_detector *
channel_detector_create(struct dfs_pattern_detector *dpd, u16 freq)
{
	u32 sz, i;
	struct channel_detector *cd;
	struct ath_common *common = ath9k_hw_common(dpd->ah);

	cd = kmalloc(sizeof(*cd), GFP_ATOMIC);
	if (cd == NULL)
		goto fail;

	INIT_LIST_HEAD(&cd->head);
	cd->freq = freq;
	sz = sizeof(cd->detectors) * dpd->num_radar_types;
	cd->detectors = kzalloc(sz, GFP_ATOMIC);
	if (cd->detectors == NULL)
		goto fail;

	for (i = 0; i < dpd->num_radar_types; i++) {
		const struct radar_detector_specs *rs = &dpd->radar_spec[i];
		struct pri_detector *de = pri_detector_init(rs);
		if (de == NULL)
			goto fail;
		cd->detectors[i] = de;
	}
	list_add(&cd->head, &dpd->channel_detectors);
	return cd;

fail:
	ath_dbg(common, DFS,
		"failed to allocate channel_detector for freq=%d\n", freq);
	channel_detector_exit(dpd, cd);
	return NULL;
}

/* 
                                                                    
                              
                               
                                                                 
  
                                                                       
                    
 */
static struct channel_detector *
channel_detector_get(struct dfs_pattern_detector *dpd, u16 freq)
{
	struct channel_detector *cd;
	list_for_each_entry(cd, &dpd->channel_detectors, head) {
		if (cd->freq == freq)
			return cd;
	}
	return channel_detector_create(dpd, freq);
}

/*
                       
 */

/*                                          */
static void dpd_reset(struct dfs_pattern_detector *dpd)
{
	struct channel_detector *cd;
	if (!list_empty(&dpd->channel_detectors))
		list_for_each_entry(cd, &dpd->channel_detectors, head)
			channel_detector_reset(dpd, cd);

}
static void dpd_exit(struct dfs_pattern_detector *dpd)
{
	struct channel_detector *cd, *cd0;
	if (!list_empty(&dpd->channel_detectors))
		list_for_each_entry_safe(cd, cd0, &dpd->channel_detectors, head)
			channel_detector_exit(dpd, cd);
	kfree(dpd);
}

static bool
dpd_add_pulse(struct dfs_pattern_detector *dpd, struct pulse_event *event)
{
	u32 i;
	struct channel_detector *cd;

	/*
                                                         
                                                         
  */
	if (dpd->region == NL80211_DFS_UNSET)
		return true;

	cd = channel_detector_get(dpd, event->freq);
	if (cd == NULL)
		return false;

	dpd->last_pulse_ts = event->ts;
	/*                                                              */
	if (event->ts < dpd->last_pulse_ts)
		dpd_reset(dpd);

	/*                                     */
	for (i = 0; i < dpd->num_radar_types; i++) {
		struct pri_detector *pd = cd->detectors[i];
		struct pri_sequence *ps = pd->add_pulse(pd, event);
		if (ps != NULL) {
			ath_dbg(ath9k_hw_common(dpd->ah), DFS,
				"DFS: radar found on freq=%d: id=%d, pri=%d, "
				"count=%d, count_false=%d\n",
				event->freq, pd->rs->type_id,
				ps->pri, ps->count, ps->count_falses);
			channel_detector_reset(dpd, cd);
			return true;
		}
	}
	return false;
}

static bool dpd_set_domain(struct dfs_pattern_detector *dpd,
			   enum nl80211_dfs_regions region)
{
	const struct radar_types *rt;
	struct channel_detector *cd, *cd0;

	if (dpd->region == region)
		return true;

	dpd->region = NL80211_DFS_UNSET;

	rt = get_dfs_domain_radar_types(region);
	if (rt == NULL)
		return false;

	/*                                                      */
	if (!list_empty(&dpd->channel_detectors))
		list_for_each_entry_safe(cd, cd0, &dpd->channel_detectors, head)
			channel_detector_exit(dpd, cd);
	dpd->radar_spec = rt->radar_types;
	dpd->num_radar_types = rt->num_radar_types;

	dpd->region = region;
	return true;
}

static struct dfs_pattern_detector default_dpd = {
	.exit		= dpd_exit,
	.set_dfs_domain	= dpd_set_domain,
	.add_pulse	= dpd_add_pulse,
	.region		= NL80211_DFS_UNSET,
};

struct dfs_pattern_detector *
dfs_pattern_detector_init(struct ath_hw *ah, enum nl80211_dfs_regions region)
{
	struct dfs_pattern_detector *dpd;
	struct ath_common *common = ath9k_hw_common(ah);

	dpd = kmalloc(sizeof(*dpd), GFP_KERNEL);
	if (dpd == NULL)
		return NULL;

	*dpd = default_dpd;
	INIT_LIST_HEAD(&dpd->channel_detectors);

	dpd->ah = ah;
	if (dpd->set_dfs_domain(dpd, region))
		return dpd;

	ath_dbg(common, DFS,"Could not set DFS domain to %d", region);
	kfree(dpd);
	return NULL;
}
EXPORT_SYMBOL(dfs_pattern_detector_init);
