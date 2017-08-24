/*
 *  ALSA interface to ivtv PCM capture streams
 *
 *  Copyright (C) 2009,2012  Andy Walls <awalls@md.metrocast.net>
 *  Copyright (C) 2009  Devin Heitmueller <dheitmueller@kernellabs.com>
 *
 *  Portions of this work were sponsored by ONELAN Limited for the cx18 driver
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA
 */

#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/spinlock.h>

#include <media/v4l2-device.h>

#include <sound/core.h>
#include <sound/initval.h>

#include "ivtv-driver.h"
#include "ivtv-version.h"
#include "ivtv-alsa.h"
#include "ivtv-alsa-mixer.h"
#include "ivtv-alsa-pcm.h"

int ivtv_alsa_debug;

#define IVTV_DEBUG_ALSA_INFO(fmt, arg...) \
	do { \
		if (ivtv_alsa_debug & 2) \
			pr_info("%s: " fmt, "ivtv-alsa", ## arg); \
	} while (0)

module_param_named(debug, ivtv_alsa_debug, int, 0644);
MODULE_PARM_DESC(debug,
		 "Debug level (bitmask). Default: 0\n"
		 "\t\t\t  1/0x0001: warning\n"
		 "\t\t\t  2/0x0002: info\n");

MODULE_AUTHOR("Andy Walls");
MODULE_DESCRIPTION("CX23415/CX23416 ALSA Interface");
MODULE_SUPPORTED_DEVICE("CX23415/CX23416 MPEG2 encoder");
MODULE_LICENSE("GPL");

MODULE_VERSION(IVTV_VERSION);

static inline
struct snd_ivtv_card *to_snd_ivtv_card(struct v4l2_device *v4l2_dev)
{
	return to_ivtv(v4l2_dev)->alsa;
}

static inline
struct snd_ivtv_card *p_to_snd_ivtv_card(struct v4l2_device **v4l2_dev)
{
	return container_of(v4l2_dev, struct snd_ivtv_card, v4l2_dev);
}

static void snd_ivtv_card_free(struct snd_ivtv_card *itvsc)
{
	if (itvsc == NULL)
		return;

	if (itvsc->v4l2_dev != NULL)
		to_ivtv(itvsc->v4l2_dev)->alsa = NULL;

	/*                                                */

	kfree(itvsc);
}

static void snd_ivtv_card_private_free(struct snd_card *sc)
{
	if (sc == NULL)
		return;
	snd_ivtv_card_free(sc->private_data);
	sc->private_data = NULL;
	sc->private_free = NULL;
}

static int snd_ivtv_card_create(struct v4l2_device *v4l2_dev,
				       struct snd_card *sc,
				       struct snd_ivtv_card **itvsc)
{
	*itvsc = kzalloc(sizeof(struct snd_ivtv_card), GFP_KERNEL);
	if (*itvsc == NULL)
		return -ENOMEM;

	(*itvsc)->v4l2_dev = v4l2_dev;
	(*itvsc)->sc = sc;

	sc->private_data = *itvsc;
	sc->private_free = snd_ivtv_card_private_free;

	return 0;
}

static int snd_ivtv_card_set_names(struct snd_ivtv_card *itvsc)
{
	struct ivtv *itv = to_ivtv(itvsc->v4l2_dev);
	struct snd_card *sc = itvsc->sc;

	/*                                                               */
	strlcpy(sc->driver, "CX2341[56]", sizeof(sc->driver));

	/*                                                             */
	snprintf(sc->shortname,  sizeof(sc->shortname), "IVTV-%d",
		 itv->instance);

	/*                                              */
	snprintf(sc->longname, sizeof(sc->longname),
		 "CX2341[56] #%d %s TV/FM Radio/Line-In Capture",
		 itv->instance, itv->card_name);

	return 0;
}

static int snd_ivtv_init(struct v4l2_device *v4l2_dev)
{
	struct ivtv *itv = to_ivtv(v4l2_dev);
	struct snd_card *sc = NULL;
	struct snd_ivtv_card *itvsc;
	int ret;

	/*                                                            */

	/*                                          */
	/*                                                      */

	/*                            */
	ret = snd_card_create(SNDRV_DEFAULT_IDX1, /*                        */
			      SNDRV_DEFAULT_STR1, /*                          */
			      THIS_MODULE, 0, &sc);
	if (ret) {
		IVTV_ALSA_ERR("%s: snd_card_create() failed with err %d\n",
			      __func__, ret);
		goto err_exit;
	}

	/*                             */
	ret = snd_ivtv_card_create(v4l2_dev, sc, &itvsc);
	if (ret) {
		IVTV_ALSA_ERR("%s: snd_ivtv_card_create() failed with err %d\n",
			      __func__, ret);
		goto err_exit_free;
	}

	/*                                        */
	snd_ivtv_card_set_names(itvsc);

	/*                                                       */
#if 0
	ret = snd_ivtv_mixer_create(itvsc);
	if (ret) {
		IVTV_ALSA_WARN("%s: snd_ivtv_mixer_create() failed with err %d:"
			       " proceeding anyway\n", __func__, ret);
	}
#endif

	ret = snd_ivtv_pcm_create(itvsc);
	if (ret) {
		IVTV_ALSA_ERR("%s: snd_ivtv_pcm_create() failed with err %d\n",
			      __func__, ret);
		goto err_exit_free;
	}
	/*                    */

	/*                                      */
	/*                                                               */
	itv->alsa = itvsc;

	/*                                */
	ret = snd_card_register(sc);
	if (ret) {
		itv->alsa = NULL;
		IVTV_ALSA_ERR("%s: snd_card_register() failed with err %d\n",
			      __func__, ret);
		goto err_exit_free;
	}

	return 0;

err_exit_free:
	if (sc != NULL)
		snd_card_free(sc);
	kfree(itvsc);
err_exit:
	return ret;
}

static int ivtv_alsa_load(struct ivtv *itv)
{
	struct v4l2_device *v4l2_dev = &itv->v4l2_dev;
	struct ivtv_stream *s;

	if (v4l2_dev == NULL) {
		pr_err("ivtv-alsa: %s: struct v4l2_device * is NULL\n",
		       __func__);
		return 0;
	}

	itv = to_ivtv(v4l2_dev);
	if (itv == NULL) {
		pr_err("ivtv-alsa itv is NULL\n");
		return 0;
	}

	s = &itv->streams[IVTV_ENC_STREAM_TYPE_PCM];
	if (s->vdev == NULL) {
		IVTV_DEBUG_ALSA_INFO("%s: PCM stream for card is disabled - "
				     "skipping\n", __func__);
		return 0;
	}

	if (itv->alsa != NULL) {
		IVTV_ALSA_ERR("%s: struct snd_ivtv_card * already exists\n",
			      __func__);
		return 0;
	}

	if (snd_ivtv_init(v4l2_dev)) {
		IVTV_ALSA_ERR("%s: failed to create struct snd_ivtv_card\n",
			      __func__);
	} else {
		IVTV_DEBUG_ALSA_INFO("%s: created ivtv ALSA interface instance "
				     "\n", __func__);
	}
	return 0;
}

static int __init ivtv_alsa_init(void)
{
	pr_info("ivtv-alsa: module loading...\n");
	ivtv_ext_init = &ivtv_alsa_load;
	return 0;
}

static void __exit snd_ivtv_exit(struct snd_ivtv_card *itvsc)
{
	struct ivtv *itv = to_ivtv(itvsc->v4l2_dev);

	/*                                         */

	snd_card_free(itvsc->sc);
	itv->alsa = NULL;
}

static int __exit ivtv_alsa_exit_callback(struct device *dev, void *data)
{
	struct v4l2_device *v4l2_dev = dev_get_drvdata(dev);
	struct snd_ivtv_card *itvsc;

	if (v4l2_dev == NULL) {
		pr_err("ivtv-alsa: %s: struct v4l2_device * is NULL\n",
		       __func__);
		return 0;
	}

	itvsc = to_snd_ivtv_card(v4l2_dev);
	if (itvsc == NULL) {
		IVTV_ALSA_WARN("%s: struct snd_ivtv_card * is NULL\n",
			       __func__);
		return 0;
	}

	snd_ivtv_exit(itvsc);
	return 0;
}

static void __exit ivtv_alsa_exit(void)
{
	struct device_driver *drv;
	int ret;

	pr_info("ivtv-alsa: module unloading...\n");

	drv = driver_find("ivtv", &pci_bus_type);
	ret = driver_for_each_device(drv, NULL, NULL, ivtv_alsa_exit_callback);
	(void)ret;	/*                           */

	ivtv_ext_init = NULL;
	pr_info("ivtv-alsa: module unload complete\n");
}

module_init(ivtv_alsa_init);
module_exit(ivtv_alsa_exit);
