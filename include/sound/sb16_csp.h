/*
 *  Copyright (c) 1999 by Uros Bizjak <uros@kss-loka.si>
 *                        Takashi Iwai <tiwai@suse.de>
 *
 *  SB16ASP/AWE32 CSP control
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
#ifndef __SOUND_SB16_CSP_H
#define __SOUND_SB16_CSP_H

#include <sound/sb.h>
#include <sound/hwdep.h>
#include <linux/firmware.h>
#include <uapi/sound/sb16_csp.h>

struct snd_sb_csp;

/*                                    */
enum {
	CSP_PROGRAM_MULAW,
	CSP_PROGRAM_ALAW,
	CSP_PROGRAM_ADPCM_INIT,
	CSP_PROGRAM_ADPCM_PLAYBACK,
	CSP_PROGRAM_ADPCM_CAPTURE,

	CSP_PROGRAM_COUNT
};

/*
                
 */
struct snd_sb_csp_ops {
	int (*csp_use) (struct snd_sb_csp * p);
	int (*csp_unuse) (struct snd_sb_csp * p);
	int (*csp_autoload) (struct snd_sb_csp * p, int pcm_sfmt, int play_rec_mode);
	int (*csp_start) (struct snd_sb_csp * p, int sample_width, int channels);
	int (*csp_stop) (struct snd_sb_csp * p);
	int (*csp_qsound_transfer) (struct snd_sb_csp * p);
};

/*
                   
 */
struct snd_sb_csp {
	struct snd_sb *chip;		/*          */
	int used;		/*                        */
	char codec_name[16];	/*               */
	unsigned short func_nr;	/*                 */
	unsigned int acc_format;	/*                      */
	int acc_channels;	/*                   */
	int acc_width;		/*                       */
	int acc_rates;		/*                       */
	int mode;		/*      */
	int run_channels;	/*                      */
	int run_width;		/*                      */
	int version;		/*                           */
	int running;		/*               */

	struct snd_sb_csp_ops ops;	/*           */

	spinlock_t q_lock;	/*         */
	int q_enabled;		/*              */
	int qpos_left;		/*               */
	int qpos_right;		/*                */
	int qpos_changed;	/*                       */

	struct snd_kcontrol *qsound_switch;
	struct snd_kcontrol *qsound_space;

	struct mutex access_mutex;	/*         */

	const struct firmware *csp_programs[CSP_PROGRAM_COUNT];
};

int snd_sb_csp_new(struct snd_sb *chip, int device, struct snd_hwdep ** rhwdep);
#endif /*                  */
