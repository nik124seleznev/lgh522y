/*
 *   ALSA driver for ICEnsemble VT1724 (Envy24HT)
 *
 *   Lowlevel functions for Philips PSC724 Ultimate Edge
 *
 *	Copyright (c) 2012 Ondrej Zary <linux@rainbow-software.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 */

#include <linux/delay.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <sound/core.h>

#include "ice1712.h"
#include "envy24ht.h"
#include "psc724.h"
#include "wm8766.h"
#include "wm8776.h"

struct psc724_spec {
	struct snd_wm8766 wm8766;
	struct snd_wm8776 wm8776;
	bool mute_all, jack_detect;
	struct snd_ice1712 *ice;
	struct delayed_work hp_work;
	bool hp_connected;
};

/*                                                                          */
/*                                                                          */
/*                                                                          */
/*
                                                                       
  
                                              
                    
               
                                        
                                                 
  
                                              
                       
  
                                            
                                                         
                                                       
                      
  
                                            
                                          
                                     
                     
                                     
  
  
                        
  
          
                                                                              
                                                                                
                                                    
                               
                                                        
                                               
                                               
  
          
                                                                               
                                                 
                                                   
                                                   
                                                   
                                                   
                                                    
  
                                                                        
                                                               
  
                                     
  
                                                               
  
                                                          
                                                            
                                                            
 */

/*                                  */

#define GPIO_HP_JACK	(1 << 14)
#define GPIO_MUTE_SUR	(1 << 20)
#define GPIO_MUTE_ALL	(1 << 22)

#define JACK_INTERVAL	1000

#define PSC724_SPI_DELAY 1

#define PSC724_SPI_DATA	(1 << 16)
#define PSC724_SPI_CLK	(1 << 17)
#define PSC724_SPI_LOAD	(1 << 18)
#define PSC724_SPI_MASK	(PSC724_SPI_DATA | PSC724_SPI_CLK | PSC724_SPI_LOAD)

static void psc724_wm8766_write(struct snd_wm8766 *wm, u16 addr, u16 data)
{
	struct psc724_spec *spec = container_of(wm, struct psc724_spec, wm8766);
	struct snd_ice1712 *ice = spec->ice;
	u32 st, bits;
	int i;

	snd_ice1712_save_gpio_status(ice);

	st = ((addr & 0x7f) << 9) | (data & 0x1ff);
	snd_ice1712_gpio_set_dir(ice, ice->gpio.direction | PSC724_SPI_MASK);
	snd_ice1712_gpio_set_mask(ice, ice->gpio.write_mask & ~PSC724_SPI_MASK);
	bits = snd_ice1712_gpio_read(ice) & ~PSC724_SPI_MASK;
	snd_ice1712_gpio_write(ice, bits);

	for (i = 0; i < 16; i++) {
		udelay(PSC724_SPI_DELAY);
		bits &= ~PSC724_SPI_CLK;
		/*           */
		st <<= 1;
		if (st & 0x10000)
			bits |= PSC724_SPI_DATA;
		else
			bits &= ~PSC724_SPI_DATA;
		snd_ice1712_gpio_write(ice, bits);
		/*            */
		udelay(PSC724_SPI_DELAY);
		bits |= PSC724_SPI_CLK;
		snd_ice1712_gpio_write(ice, bits);
	}
	/*           */
	udelay(PSC724_SPI_DELAY);
	bits |= PSC724_SPI_LOAD;
	snd_ice1712_gpio_write(ice, bits);
	/*                               */
	udelay(PSC724_SPI_DELAY);
	bits |= (PSC724_SPI_DATA | PSC724_SPI_CLK);
	snd_ice1712_gpio_write(ice, bits);

	snd_ice1712_restore_gpio_status(ice);
}

static void psc724_wm8776_write(struct snd_wm8776 *wm, u8 addr, u8 data)
{
	struct psc724_spec *spec = container_of(wm, struct psc724_spec, wm8776);

	snd_vt1724_write_i2c(spec->ice, 0x34, addr, data);
}

/*          */

static void psc724_set_master_switch(struct snd_ice1712 *ice, bool on)
{
	unsigned int bits = snd_ice1712_gpio_read(ice);
	struct psc724_spec *spec = ice->spec;

	spec->mute_all = !on;
	if (on)
		bits &= ~(GPIO_MUTE_ALL | GPIO_MUTE_SUR);
	else
		bits |= GPIO_MUTE_ALL | GPIO_MUTE_SUR;
	snd_ice1712_gpio_write(ice, bits);
}

static bool psc724_get_master_switch(struct snd_ice1712 *ice)
{
	struct psc724_spec *spec = ice->spec;

	return !spec->mute_all;
}

/*                */

static void psc724_set_jack_state(struct snd_ice1712 *ice, bool hp_connected)
{
	struct psc724_spec *spec = ice->spec;
	struct snd_ctl_elem_id elem_id;
	struct snd_kcontrol *kctl;
	u16 power = spec->wm8776.regs[WM8776_REG_PWRDOWN] & ~WM8776_PWR_HPPD;

	psc724_set_master_switch(ice, !hp_connected);
	if (!hp_connected)
		power |= WM8776_PWR_HPPD;
	snd_wm8776_set_power(&spec->wm8776, power);
	spec->hp_connected = hp_connected;
	/*                                         */
	memset(&elem_id, 0, sizeof(elem_id));
	elem_id.iface = SNDRV_CTL_ELEM_IFACE_MIXER;
	strncpy(elem_id.name, "Master Speakers Playback Switch",
						sizeof(elem_id.name));
	kctl = snd_ctl_find_id(ice->card, &elem_id);
	snd_ctl_notify(ice->card, SNDRV_CTL_EVENT_MASK_VALUE, &kctl->id);
	/*                           */
	strncpy(elem_id.name, spec->wm8776.ctl[WM8776_CTL_HP_SW].name,
						sizeof(elem_id.name));
	kctl = snd_ctl_find_id(ice->card, &elem_id);
	snd_ctl_notify(ice->card, SNDRV_CTL_EVENT_MASK_VALUE, &kctl->id);
}

static void psc724_update_hp_jack_state(struct work_struct *work)
{
	struct psc724_spec *spec = container_of(work, struct psc724_spec,
						hp_work.work);
	struct snd_ice1712 *ice = spec->ice;
	bool hp_connected = snd_ice1712_gpio_read(ice) & GPIO_HP_JACK;

	schedule_delayed_work(&spec->hp_work, msecs_to_jiffies(JACK_INTERVAL));
	if (hp_connected == spec->hp_connected)
		return;
	psc724_set_jack_state(ice, hp_connected);
}

static void psc724_set_jack_detection(struct snd_ice1712 *ice, bool on)
{
	struct psc724_spec *spec = ice->spec;

	if (spec->jack_detect == on)
		return;

	spec->jack_detect = on;
	if (on) {
		bool hp_connected = snd_ice1712_gpio_read(ice) & GPIO_HP_JACK;
		psc724_set_jack_state(ice, hp_connected);
		schedule_delayed_work(&spec->hp_work,
					msecs_to_jiffies(JACK_INTERVAL));
	} else
		cancel_delayed_work_sync(&spec->hp_work);
}

static bool psc724_get_jack_detection(struct snd_ice1712 *ice)
{
	struct psc724_spec *spec = ice->spec;

	return spec->jack_detect;
}

/*                */

struct psc724_control {
	const char *name;
	void (*set)(struct snd_ice1712 *ice, bool on);
	bool (*get)(struct snd_ice1712 *ice);
};

static const struct psc724_control psc724_cont[] = {
	{
		.name = "Master Speakers Playback Switch",
		.set = psc724_set_master_switch,
		.get = psc724_get_master_switch,
	},
	{
		.name = "Headphone Jack Detection Playback Switch",
		.set = psc724_set_jack_detection,
		.get = psc724_get_jack_detection,
	},
};

static int psc724_ctl_get(struct snd_kcontrol *kcontrol,
				  struct snd_ctl_elem_value *ucontrol)
{
	struct snd_ice1712 *ice = snd_kcontrol_chip(kcontrol);
	int n = kcontrol->private_value;

	ucontrol->value.integer.value[0] = psc724_cont[n].get(ice);

	return 0;
}

static int psc724_ctl_put(struct snd_kcontrol *kcontrol,
				  struct snd_ctl_elem_value *ucontrol)
{
	struct snd_ice1712 *ice = snd_kcontrol_chip(kcontrol);
	int n = kcontrol->private_value;

	psc724_cont[n].set(ice, ucontrol->value.integer.value[0]);

	return 0;
}

static const char *front_volume	= "Front Playback Volume";
static const char *front_switch	= "Front Playback Switch";
static const char *front_zc	= "Front Zero Cross Detect Playback Switch";
static const char *front_izd	= "Front Infinite Zero Detect Playback Switch";
static const char *front_phase	= "Front Phase Invert Playback Switch";
static const char *front_deemph	= "Front Deemphasis Playback Switch";
static const char *ain1_switch	= "Line Capture Switch";
static const char *ain2_switch	= "CD Capture Switch";
static const char *ain3_switch	= "AUX Capture Switch";
static const char *ain4_switch	= "Front Mic Capture Switch";
static const char *ain5_switch	= "Rear Mic Capture Switch";
static const char *rear_volume	= "Surround Playback Volume";
static const char *clfe_volume	= "CLFE Playback Volume";
static const char *rear_switch	= "Surround Playback Switch";
static const char *clfe_switch	= "CLFE Playback Switch";
static const char *rear_phase	= "Surround Phase Invert Playback Switch";
static const char *clfe_phase	= "CLFE Phase Invert Playback Switch";
static const char *rear_deemph	= "Surround Deemphasis Playback Switch";
static const char *clfe_deemph	= "CLFE Deemphasis Playback Switch";
static const char *rear_clfe_izd = "Rear Infinite Zero Detect Playback Switch";
static const char *rear_clfe_zc	= "Rear Zero Cross Detect Playback Switch";

static int psc724_add_controls(struct snd_ice1712 *ice)
{
	struct snd_kcontrol_new cont;
	struct snd_kcontrol *ctl;
	int err, i;
	struct psc724_spec *spec = ice->spec;

	spec->wm8776.ctl[WM8776_CTL_DAC_VOL].name = front_volume;
	spec->wm8776.ctl[WM8776_CTL_DAC_SW].name = front_switch;
	spec->wm8776.ctl[WM8776_CTL_DAC_ZC_SW].name = front_zc;
	spec->wm8776.ctl[WM8776_CTL_AUX_SW].name = NULL;
	spec->wm8776.ctl[WM8776_CTL_DAC_IZD_SW].name = front_izd;
	spec->wm8776.ctl[WM8776_CTL_PHASE_SW].name = front_phase;
	spec->wm8776.ctl[WM8776_CTL_DEEMPH_SW].name = front_deemph;
	spec->wm8776.ctl[WM8776_CTL_INPUT1_SW].name = ain1_switch;
	spec->wm8776.ctl[WM8776_CTL_INPUT2_SW].name = ain2_switch;
	spec->wm8776.ctl[WM8776_CTL_INPUT3_SW].name = ain3_switch;
	spec->wm8776.ctl[WM8776_CTL_INPUT4_SW].name = ain4_switch;
	spec->wm8776.ctl[WM8776_CTL_INPUT5_SW].name = ain5_switch;
	snd_wm8776_build_controls(&spec->wm8776);
	spec->wm8766.ctl[WM8766_CTL_CH1_VOL].name = rear_volume;
	spec->wm8766.ctl[WM8766_CTL_CH2_VOL].name = clfe_volume;
	spec->wm8766.ctl[WM8766_CTL_CH3_VOL].name = NULL;
	spec->wm8766.ctl[WM8766_CTL_CH1_SW].name = rear_switch;
	spec->wm8766.ctl[WM8766_CTL_CH2_SW].name = clfe_switch;
	spec->wm8766.ctl[WM8766_CTL_CH3_SW].name = NULL;
	spec->wm8766.ctl[WM8766_CTL_PHASE1_SW].name = rear_phase;
	spec->wm8766.ctl[WM8766_CTL_PHASE2_SW].name = clfe_phase;
	spec->wm8766.ctl[WM8766_CTL_PHASE3_SW].name = NULL;
	spec->wm8766.ctl[WM8766_CTL_DEEMPH1_SW].name = rear_deemph;
	spec->wm8766.ctl[WM8766_CTL_DEEMPH2_SW].name = clfe_deemph;
	spec->wm8766.ctl[WM8766_CTL_DEEMPH3_SW].name = NULL;
	spec->wm8766.ctl[WM8766_CTL_IZD_SW].name = rear_clfe_izd;
	spec->wm8766.ctl[WM8766_CTL_ZC_SW].name = rear_clfe_zc;
	snd_wm8766_build_controls(&spec->wm8766);

	memset(&cont, 0, sizeof(cont));
	cont.iface = SNDRV_CTL_ELEM_IFACE_MIXER;
	for (i = 0; i < ARRAY_SIZE(psc724_cont); i++) {
		cont.private_value = i;
		cont.name = psc724_cont[i].name;
		cont.access = SNDRV_CTL_ELEM_ACCESS_READWRITE;
		cont.info = snd_ctl_boolean_mono_info;
		cont.get = psc724_ctl_get;
		cont.put = psc724_ctl_put;
		ctl = snd_ctl_new1(&cont, ice);
		if (!ctl)
			return -ENOMEM;
		err = snd_ctl_add(ice->card, ctl);
		if (err < 0)
			return err;
	}

	return 0;
}

static void psc724_set_pro_rate(struct snd_ice1712 *ice, unsigned int rate)
{
	struct psc724_spec *spec = ice->spec;
	/*                                                              */
	snd_wm8776_volume_restore(&spec->wm8776);
	snd_wm8766_volume_restore(&spec->wm8766);
}

/*                  */

#ifdef CONFIG_PM_SLEEP
static int psc724_resume(struct snd_ice1712 *ice)
{
	struct psc724_spec *spec = ice->spec;

	snd_wm8776_resume(&spec->wm8776);
	snd_wm8766_resume(&spec->wm8766);

	return 0;
}
#endif

/*      */

static int psc724_init(struct snd_ice1712 *ice)
{
	struct psc724_spec *spec;

	spec = kzalloc(sizeof(*spec), GFP_KERNEL);
	if (!spec)
		return -ENOMEM;
	ice->spec = spec;
	spec->ice = ice;

	ice->num_total_dacs = 6;
	ice->num_total_adcs = 2;
	spec->wm8776.ops.write = psc724_wm8776_write;
	spec->wm8776.card = ice->card;
	snd_wm8776_init(&spec->wm8776);
	spec->wm8766.ops.write = psc724_wm8766_write;
	spec->wm8766.card = ice->card;
#ifdef CONFIG_PM_SLEEP
	ice->pm_resume = psc724_resume;
	ice->pm_suspend_enabled = 1;
#endif
	snd_wm8766_init(&spec->wm8766);
	snd_wm8766_set_if(&spec->wm8766,
			WM8766_IF_FMT_I2S | WM8766_IF_IWL_24BIT);
	ice->gpio.set_pro_rate = psc724_set_pro_rate;
	INIT_DELAYED_WORK(&spec->hp_work, psc724_update_hp_jack_state);
	psc724_set_jack_detection(ice, true);
	return 0;
}

static void psc724_exit(struct snd_ice1712 *ice)
{
	struct psc724_spec *spec = ice->spec;

	cancel_delayed_work_sync(&spec->hp_work);
}

/*                                                                            */
static unsigned char psc724_eeprom[] = {
	[ICE_EEP2_SYSCONF]	= 0x42,	/*                          */
	[ICE_EEP2_ACLINK]	= 0x80,	/*     */
	[ICE_EEP2_I2S]		= 0xf0,	/*                          */
	[ICE_EEP2_SPDIF]	= 0xc1,	/*                                 */
	/*              */
	[ICE_EEP2_GPIO_DIR2]	= 0x5f, /*                                    */
	/*                   */
	[ICE_EEP2_GPIO_MASK]	= 0xff, /*           */
	[ICE_EEP2_GPIO_MASK1]	= 0xff, /*           */
	[ICE_EEP2_GPIO_MASK2]	= 0xa0, /*                                    */
	/*                    */
	[ICE_EEP2_GPIO_STATE2]	= 0x20,	/*                              */
};

struct snd_ice1712_card_info snd_vt1724_psc724_cards[] = {
	{
		.subvendor = VT1724_SUBDEVICE_PSC724,
		.name = "Philips PSC724 Ultimate Edge",
		.model = "psc724",
		.chip_init = psc724_init,
		.chip_exit = psc724_exit,
		.build_controls = psc724_add_controls,
		.eeprom_size = sizeof(psc724_eeprom),
		.eeprom_data = psc724_eeprom,
	},
	{} /*          */
};
