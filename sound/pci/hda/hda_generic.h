/*
 * Generic BIOS auto-parser helper functions for HD-audio
 *
 * Copyright (c) 2012 Takashi Iwai <tiwai@suse.de>
 *
 * This driver is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef __SOUND_HDA_GENERIC_H
#define __SOUND_HDA_GENERIC_H

/*                  */
enum {
	HDA_GEN_HP_EVENT = 1, HDA_GEN_FRONT_EVENT, HDA_GEN_MIC_EVENT,
	HDA_GEN_LAST_EVENT = HDA_GEN_MIC_EVENT
};

/*                                */
struct hda_multi_io {
	hda_nid_t pin;		/*                         */
	hda_nid_t dac;		/*                     */
	unsigned int ctl_in;	/*                                */
};

/*                       
  
                                                        
                                
  
                                                                              
                                                                           
                                                                         
                                               
                                                                 
 */

#define MAX_NID_PATH_DEPTH	10

enum {
	NID_PATH_VOL_CTL,
	NID_PATH_MUTE_CTL,
	NID_PATH_BOOST_CTL,
	NID_PATH_NUM_CTLS
};

struct nid_path {
	int depth;
	hda_nid_t path[MAX_NID_PATH_DEPTH];
	unsigned char idx[MAX_NID_PATH_DEPTH];
	unsigned char multi[MAX_NID_PATH_DEPTH];
	unsigned int ctls[NID_PATH_NUM_CTLS]; /*                  */
	bool active;
};

/*                                  */

#define MAX_AUTO_MIC_PINS	3

struct automic_entry {
	hda_nid_t pin;		/*     */
	int idx;		/*                          */
	unsigned int attr;	/*                                  */
};

/*                  */
enum { STREAM_MULTI_OUT, STREAM_INDEP_HP };

/*                 */
enum {
	HDA_GEN_PCM_ACT_OPEN,
	HDA_GEN_PCM_ACT_PREPARE,
	HDA_GEN_PCM_ACT_CLEANUP,
	HDA_GEN_PCM_ACT_CLOSE,
};

/*                              */
struct badness_table {
	int no_primary_dac;	/*                */
	int no_dac;		/*                   */
	int shared_primary;	/*                                        */
	int shared_surr;	/*                                           */
	int shared_clfe;	/*                                       */
	int shared_surr_main;	/*                                     */
};

extern const struct badness_table hda_main_out_badness;
extern const struct badness_table hda_extra_out_badness;

struct hda_gen_spec {
	char stream_name_analog[32];	/*                   */
	const struct hda_pcm_stream *stream_analog_playback;
	const struct hda_pcm_stream *stream_analog_capture;

	char stream_name_alt_analog[32]; /*                               */
	const struct hda_pcm_stream *stream_analog_alt_playback;
	const struct hda_pcm_stream *stream_analog_alt_capture;

	char stream_name_digital[32];	/*                    */
	const struct hda_pcm_stream *stream_digital_playback;
	const struct hda_pcm_stream *stream_digital_capture;

	/*     */
	unsigned int active_streams;
	struct mutex pcm_mutex;

	/*          */
	struct hda_multi_out multiout;	/*                
                                      
                                           
      */
	hda_nid_t alt_dac_nid;
	hda_nid_t slave_dig_outs[3];	/*                             */
	int dig_out_type;

	/*         */
	unsigned int num_adc_nids;
	hda_nid_t adc_nids[AUTO_CFG_MAX_INS];
	hda_nid_t dig_in_nid;		/*                          */
	hda_nid_t mixer_nid;		/*                  */
	hda_nid_t mixer_merge_nid;	/*                                  */
	const char *input_labels[HDA_MAX_NUM_INPUTS];
	int input_label_idxs[HDA_MAX_NUM_INPUTS];

	/*                                           */
	hda_nid_t cur_adc;
	unsigned int cur_adc_stream_tag;
	unsigned int cur_adc_format;

	/*                */
	struct hda_input_mux input_mux;
	unsigned int cur_mux[3];

	/*               */
	/*                                                                 
                                                                   
                                                                      
   
                                                                       
                                         
   
                                                                       
                                                                     
                                                                 
  */
	int min_channel_count;		/*                                    */
	int ext_channel_count;		/*                                   */
	int const_channel_count;	/*                       */

	/*                 */
	struct hda_pcm pcm_rec[3];	/*                      */

	/*                                            */
	struct auto_pin_cfg autocfg;
	struct snd_array kctls;
	hda_nid_t private_dac_nids[AUTO_CFG_MAX_OUTS];
	hda_nid_t imux_pins[HDA_MAX_NUM_INPUTS];
	unsigned int dyn_adc_idx[HDA_MAX_NUM_INPUTS];
	/*               */
	hda_nid_t shared_mic_vref_pin;
	hda_nid_t hp_mic_pin;
	int hp_mic_mux_idx;

	/*               */
	int num_all_dacs;
	hda_nid_t all_dacs[16];
	int num_all_adcs;
	hda_nid_t all_adcs[AUTO_CFG_MAX_INS];

	/*           */
	struct snd_array paths;

	/*              */
	int out_paths[AUTO_CFG_MAX_OUTS];
	int hp_paths[AUTO_CFG_MAX_OUTS];
	int speaker_paths[AUTO_CFG_MAX_OUTS];
	int aamix_out_paths[3];
	int digout_paths[AUTO_CFG_MAX_OUTS];
	int input_paths[HDA_MAX_NUM_INPUTS][AUTO_CFG_MAX_INS];
	int loopback_paths[HDA_MAX_NUM_INPUTS];
	int loopback_merge_path;
	int digin_path;

	/*                */
	int am_num_entries;
	struct automic_entry am_entry[MAX_AUTO_MIC_PINS];

	/*                 */
	/*                                     */
	unsigned int hp_jack_present:1;
	unsigned int line_jack_present:1;
	unsigned int speaker_muted:1; /*                                */
	unsigned int line_out_muted:1; /*                           */

	/*                                                   */
	unsigned int auto_mic:1;
	unsigned int automute_speaker:1; /*                          */
	unsigned int automute_lo:1; /*                     */

	/*                                 */
	unsigned int detect_hp:1;	/*                             */
	unsigned int detect_lo:1;	/*                            */
	unsigned int automute_speaker_possible:1; /*                                        */
	unsigned int automute_lo_possible:1;	  /*                            */

	/*                                            */
	unsigned int master_mute:1;	/*                      */
	unsigned int keep_vref_in_automute:1; /*                              */
	unsigned int line_in_auto_switch:1; /*                           */

	/*                                                                   */
	unsigned int suppress_auto_mute:1; /*                               */
	unsigned int suppress_auto_mic:1; /*                                 */

	/*                            */
	unsigned int need_dac_fix:1; /*                                       */
	unsigned int hp_mic:1; /*                      */
	unsigned int suppress_hp_mic_detect:1; /*                     */
	unsigned int no_primary_hp:1; /*                                      */
	unsigned int multi_cap_vol:1; /*                                    */
	unsigned int inv_dmic_split:1; /*                                */
	unsigned int own_eapd_ctl:1; /*                          */
	unsigned int keep_eapd_on:1; /*                                   */
	unsigned int vmaster_mute_enum:1; /*                            */
	unsigned int indep_hp:1; /*                          */
	unsigned int prefer_hp_amp:1; /*                                  */
	unsigned int add_stereo_mix_input:1; /*                            */
	unsigned int add_jack_modes:1; /*                             */
	unsigned int power_down_unused:1; /*                           */

	/*                      */
	unsigned int no_analog:1; /*                  */
	unsigned int dyn_adc_switch:1; /*                          */
	unsigned int indep_hp_enabled:1; /*                        */
	unsigned int have_aamix_ctl:1;
	unsigned int hp_mic_jack_modes:1;

	/*                                            */
	const struct badness_table *main_out_badness;
	const struct badness_table *extra_out_badness;

	/*                                                  */
	const hda_nid_t *preferred_dacs;

	/*                      */
	bool aamix_mode;

	/*              */
	hda_nid_t beep_nid;

	/*                    */
	hda_nid_t vmaster_nid;
	unsigned int vmaster_tlv[4];
	struct hda_vmaster_mute_hook vmaster_mute;

	struct hda_loopback_check loopback;
	struct snd_array loopback_list;

	/*          */
	int multi_ios;
	struct hda_multi_io multi_io[4];

	/*       */
	void (*init_hook)(struct hda_codec *codec);
	void (*automute_hook)(struct hda_codec *codec);
	void (*cap_sync_hook)(struct hda_codec *codec,
			      struct snd_ctl_elem_value *ucontrol);

	/*           */
	void (*pcm_playback_hook)(struct hda_pcm_stream *hinfo,
				  struct hda_codec *codec,
				  struct snd_pcm_substream *substream,
				  int action);
	void (*pcm_capture_hook)(struct hda_pcm_stream *hinfo,
				 struct hda_codec *codec,
				 struct snd_pcm_substream *substream,
				 int action);

	/*                             */
	void (*hp_automute_hook)(struct hda_codec *codec,
				 struct hda_jack_tbl *tbl);
	void (*line_automute_hook)(struct hda_codec *codec,
				   struct hda_jack_tbl *tbl);
	void (*mic_autoswitch_hook)(struct hda_codec *codec,
				    struct hda_jack_tbl *tbl);
};

int snd_hda_gen_spec_init(struct hda_gen_spec *spec);
void snd_hda_gen_spec_free(struct hda_gen_spec *spec);

int snd_hda_gen_init(struct hda_codec *codec);
void snd_hda_gen_free(struct hda_codec *codec);

struct nid_path *snd_hda_get_nid_path(struct hda_codec *codec,
				      hda_nid_t from_nid, hda_nid_t to_nid);
int snd_hda_get_path_idx(struct hda_codec *codec, struct nid_path *path);
struct nid_path *snd_hda_get_path_from_idx(struct hda_codec *codec, int idx);
bool snd_hda_parse_nid_path(struct hda_codec *codec, hda_nid_t from_nid,
			    hda_nid_t to_nid, int anchor_nid,
			    struct nid_path *path);
struct nid_path *
snd_hda_add_new_path(struct hda_codec *codec, hda_nid_t from_nid,
		     hda_nid_t to_nid, int anchor_nid);
void snd_hda_activate_path(struct hda_codec *codec, struct nid_path *path,
			   bool enable, bool add_aamix);

struct snd_kcontrol_new *
snd_hda_gen_add_kctl(struct hda_gen_spec *spec, const char *name,
		     const struct snd_kcontrol_new *temp);

int snd_hda_gen_parse_auto_config(struct hda_codec *codec,
				  struct auto_pin_cfg *cfg);
int snd_hda_gen_build_controls(struct hda_codec *codec);
int snd_hda_gen_build_pcms(struct hda_codec *codec);

/*                               */
void snd_hda_gen_hp_automute(struct hda_codec *codec,
			     struct hda_jack_tbl *jack);
void snd_hda_gen_line_automute(struct hda_codec *codec,
			       struct hda_jack_tbl *jack);
void snd_hda_gen_mic_autoswitch(struct hda_codec *codec,
				struct hda_jack_tbl *jack);
void snd_hda_gen_update_outputs(struct hda_codec *codec);

#ifdef CONFIG_PM
int snd_hda_gen_check_power_status(struct hda_codec *codec, hda_nid_t nid);
#endif

#endif /*                       */
