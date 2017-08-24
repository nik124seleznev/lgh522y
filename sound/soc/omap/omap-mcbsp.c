/*
 * omap-mcbsp.c  --  OMAP ALSA SoC DAI driver using McBSP port
 *
 * Copyright (C) 2008 Nokia Corporation
 *
 * Contact: Jarkko Nikula <jarkko.nikula@bitmer.com>
 *          Peter Ujfalusi <peter.ujfalusi@ti.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/pm_runtime.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/initval.h>
#include <sound/soc.h>
#include <sound/dmaengine_pcm.h>

#include <linux/platform_data/asoc-ti-mcbsp.h>
#include "mcbsp.h"
#include "omap-mcbsp.h"

#define OMAP_MCBSP_RATES	(SNDRV_PCM_RATE_8000_96000)

#define OMAP_MCBSP_SOC_SINGLE_S16_EXT(xname, xmin, xmax, \
	xhandler_get, xhandler_put) \
{	.iface = SNDRV_CTL_ELEM_IFACE_MIXER, .name = xname, \
	.info = omap_mcbsp_st_info_volsw, \
	.get = xhandler_get, .put = xhandler_put, \
	.private_value = (unsigned long) &(struct soc_mixer_control) \
	{.min = xmin, .max = xmax} }

enum {
	OMAP_MCBSP_WORD_8 = 0,
	OMAP_MCBSP_WORD_12,
	OMAP_MCBSP_WORD_16,
	OMAP_MCBSP_WORD_20,
	OMAP_MCBSP_WORD_24,
	OMAP_MCBSP_WORD_32,
};

/*
                                                                           
                                                         
 */
static void omap_mcbsp_set_threshold(struct snd_pcm_substream *substream,
		unsigned int packet_size)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *cpu_dai = rtd->cpu_dai;
	struct omap_mcbsp *mcbsp = snd_soc_dai_get_drvdata(cpu_dai);
	int words;

	/*
                                              
                                                                 
                                                                    
                     
  */
	if (packet_size)
		words = packet_size;
	else
		words = 1;

	/*                                       */
	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
		omap_mcbsp_set_tx_threshold(mcbsp, words);
	else
		omap_mcbsp_set_rx_threshold(mcbsp, words);
}

static int omap_mcbsp_hwrule_min_buffersize(struct snd_pcm_hw_params *params,
				    struct snd_pcm_hw_rule *rule)
{
	struct snd_interval *buffer_size = hw_param_interval(params,
					SNDRV_PCM_HW_PARAM_BUFFER_SIZE);
	struct snd_interval *channels = hw_param_interval(params,
					SNDRV_PCM_HW_PARAM_CHANNELS);
	struct omap_mcbsp *mcbsp = rule->private;
	struct snd_interval frames;
	int size;

	snd_interval_any(&frames);
	size = mcbsp->pdata->buffer_size;

	frames.min = size / channels->min;
	frames.integer = 1;
	return snd_interval_refine(buffer_size, &frames);
}

static int omap_mcbsp_dai_startup(struct snd_pcm_substream *substream,
				  struct snd_soc_dai *cpu_dai)
{
	struct omap_mcbsp *mcbsp = snd_soc_dai_get_drvdata(cpu_dai);
	int err = 0;

	if (!cpu_dai->active)
		err = omap_mcbsp_request(mcbsp);

	/*
                                        
                                                  
                                         
                                                                      
                          
                                              
                                              
                                                                    
             
                                                   
                                                    
                                                                   
                                                          
  */
	if (mcbsp->pdata->buffer_size) {
		/*
                                                 
                                                         
                                              
  */
		if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
			snd_pcm_hw_rule_add(substream->runtime, 0,
					    SNDRV_PCM_HW_PARAM_BUFFER_SIZE,
					    omap_mcbsp_hwrule_min_buffersize,
					    mcbsp,
					    SNDRV_PCM_HW_PARAM_CHANNELS, -1);

		/*                                                */
		snd_pcm_hw_constraint_step(substream->runtime, 0,
					   SNDRV_PCM_HW_PARAM_PERIOD_SIZE, 2);
	}

	snd_soc_dai_set_dma_data(cpu_dai, substream,
				 &mcbsp->dma_data[substream->stream]);

	return err;
}

static void omap_mcbsp_dai_shutdown(struct snd_pcm_substream *substream,
				    struct snd_soc_dai *cpu_dai)
{
	struct omap_mcbsp *mcbsp = snd_soc_dai_get_drvdata(cpu_dai);

	if (!cpu_dai->active) {
		omap_mcbsp_free(mcbsp);
		mcbsp->configured = 0;
	}
}

static int omap_mcbsp_dai_trigger(struct snd_pcm_substream *substream, int cmd,
				  struct snd_soc_dai *cpu_dai)
{
	struct omap_mcbsp *mcbsp = snd_soc_dai_get_drvdata(cpu_dai);
	int err = 0, play = (substream->stream == SNDRV_PCM_STREAM_PLAYBACK);

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
	case SNDRV_PCM_TRIGGER_RESUME:
	case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
		mcbsp->active++;
		omap_mcbsp_start(mcbsp, play, !play);
		break;

	case SNDRV_PCM_TRIGGER_STOP:
	case SNDRV_PCM_TRIGGER_SUSPEND:
	case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
		omap_mcbsp_stop(mcbsp, play, !play);
		mcbsp->active--;
		break;
	default:
		err = -EINVAL;
	}

	return err;
}

static snd_pcm_sframes_t omap_mcbsp_dai_delay(
			struct snd_pcm_substream *substream,
			struct snd_soc_dai *dai)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *cpu_dai = rtd->cpu_dai;
	struct omap_mcbsp *mcbsp = snd_soc_dai_get_drvdata(cpu_dai);
	u16 fifo_use;
	snd_pcm_sframes_t delay;

	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
		fifo_use = omap_mcbsp_get_tx_delay(mcbsp);
	else
		fifo_use = omap_mcbsp_get_rx_delay(mcbsp);

	/*
                                                               
                                                                  
            
  */
	delay = fifo_use / substream->runtime->channels;

	return delay;
}

static int omap_mcbsp_dai_hw_params(struct snd_pcm_substream *substream,
				    struct snd_pcm_hw_params *params,
				    struct snd_soc_dai *cpu_dai)
{
	struct omap_mcbsp *mcbsp = snd_soc_dai_get_drvdata(cpu_dai);
	struct omap_mcbsp_reg_cfg *regs = &mcbsp->cfg_regs;
	struct snd_dmaengine_dai_dma_data *dma_data;
	int wlen, channels, wpf;
	int pkt_size = 0;
	unsigned int format, div, framesize, master;

	dma_data = snd_soc_dai_get_dma_data(cpu_dai, substream);
	channels = params_channels(params);

	switch (params_format(params)) {
	case SNDRV_PCM_FORMAT_S16_LE:
		wlen = 16;
		break;
	case SNDRV_PCM_FORMAT_S32_LE:
		wlen = 32;
		break;
	default:
		return -EINVAL;
	}
	if (mcbsp->pdata->buffer_size) {
		if (mcbsp->dma_op_mode == MCBSP_DMA_MODE_THRESHOLD) {
			int period_words, max_thrsh;
			int divider = 0;

			period_words = params_period_bytes(params) / (wlen / 8);
			if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
				max_thrsh = mcbsp->max_tx_thres;
			else
				max_thrsh = mcbsp->max_rx_thres;
			/*
                                                         
                                                        
                                                          
                                                        
                             
    */
			divider = period_words / max_thrsh;
			if (period_words % max_thrsh)
				divider++;
			while (period_words % divider &&
				divider < period_words)
				divider++;
			if (divider == period_words)
				return -EINVAL;

			pkt_size = period_words / divider;
		} else if (channels > 1) {
			/*                                      */
			pkt_size = channels;
		}
		omap_mcbsp_set_threshold(substream, pkt_size);
	}

	dma_data->maxburst = pkt_size;

	if (mcbsp->configured) {
		/*                                            */
		return 0;
	}

	regs->rcr2	&= ~(RPHASE | RFRLEN2(0x7f) | RWDLEN2(7));
	regs->xcr2	&= ~(RPHASE | XFRLEN2(0x7f) | XWDLEN2(7));
	regs->rcr1	&= ~(RFRLEN1(0x7f) | RWDLEN1(7));
	regs->xcr1	&= ~(XFRLEN1(0x7f) | XWDLEN1(7));
	format = mcbsp->fmt & SND_SOC_DAIFMT_FORMAT_MASK;
	wpf = channels;
	if (channels == 2 && (format == SND_SOC_DAIFMT_I2S ||
			      format == SND_SOC_DAIFMT_LEFT_J)) {
		/*                       */
		regs->rcr2	|= RPHASE;
		regs->xcr2	|= XPHASE;
		/*                                                    */
		wpf--;
		regs->rcr2	|= RFRLEN2(wpf - 1);
		regs->xcr2	|= XFRLEN2(wpf - 1);
	}

	regs->rcr1	|= RFRLEN1(wpf - 1);
	regs->xcr1	|= XFRLEN1(wpf - 1);

	switch (params_format(params)) {
	case SNDRV_PCM_FORMAT_S16_LE:
		/*                  */
		regs->rcr2	|= RWDLEN2(OMAP_MCBSP_WORD_16);
		regs->rcr1	|= RWDLEN1(OMAP_MCBSP_WORD_16);
		regs->xcr2	|= XWDLEN2(OMAP_MCBSP_WORD_16);
		regs->xcr1	|= XWDLEN1(OMAP_MCBSP_WORD_16);
		break;
	case SNDRV_PCM_FORMAT_S32_LE:
		/*                  */
		regs->rcr2	|= RWDLEN2(OMAP_MCBSP_WORD_32);
		regs->rcr1	|= RWDLEN1(OMAP_MCBSP_WORD_32);
		regs->xcr2	|= XWDLEN2(OMAP_MCBSP_WORD_32);
		regs->xcr1	|= XWDLEN1(OMAP_MCBSP_WORD_32);
		break;
	default:
		/*                        */
		return -EINVAL;
	}

	/*                                                             
                                                       */
	master = mcbsp->fmt & SND_SOC_DAIFMT_MASTER_MASK;
	if (master ==	SND_SOC_DAIFMT_CBS_CFS) {
		div = mcbsp->clk_div ? mcbsp->clk_div : 1;
		framesize = (mcbsp->in_freq / div) / params_rate(params);

		if (framesize < wlen * channels) {
			printk(KERN_ERR "%s: not enough bandwidth for desired rate and "
					"channels\n", __func__);
			return -EINVAL;
		}
	} else
		framesize = wlen * channels;

	/*                                                        */
	regs->srgr2	&= ~FPER(0xfff);
	regs->srgr1	&= ~FWID(0xff);
	switch (format) {
	case SND_SOC_DAIFMT_I2S:
	case SND_SOC_DAIFMT_LEFT_J:
		regs->srgr2	|= FPER(framesize - 1);
		regs->srgr1	|= FWID((framesize >> 1) - 1);
		break;
	case SND_SOC_DAIFMT_DSP_A:
	case SND_SOC_DAIFMT_DSP_B:
		regs->srgr2	|= FPER(framesize - 1);
		regs->srgr1	|= FWID(0);
		break;
	}

	omap_mcbsp_config(mcbsp, &mcbsp->cfg_regs);
	mcbsp->wlen = wlen;
	mcbsp->configured = 1;

	return 0;
}

/*
                                                                              
                            
 */
static int omap_mcbsp_dai_set_dai_fmt(struct snd_soc_dai *cpu_dai,
				      unsigned int fmt)
{
	struct omap_mcbsp *mcbsp = snd_soc_dai_get_drvdata(cpu_dai);
	struct omap_mcbsp_reg_cfg *regs = &mcbsp->cfg_regs;
	bool inv_fs = false;

	if (mcbsp->configured)
		return 0;

	mcbsp->fmt = fmt;
	memset(regs, 0, sizeof(*regs));
	/*                                 */
	regs->spcr2	|= XINTM(3) | FREE;
	regs->spcr1	|= RINTM(3);
	/*                                                     */
	if (!mcbsp->pdata->has_ccr) {
		regs->rcr2	|= RFIG;
		regs->xcr2	|= XFIG;
	}

	/*                                                                 */
	if (mcbsp->pdata->has_ccr) {
		regs->xccr = DXENDLY(1) | XDMAEN | XDISABLE;
		regs->rccr = RFULL_CYCLE | RDMAEN | RDISABLE;
	}

	switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
	case SND_SOC_DAIFMT_I2S:
		/*                  */
		regs->rcr2	|= RDATDLY(1);
		regs->xcr2	|= XDATDLY(1);
		break;
	case SND_SOC_DAIFMT_LEFT_J:
		/*                  */
		regs->rcr2	|= RDATDLY(0);
		regs->xcr2	|= XDATDLY(0);
		regs->spcr1	|= RJUST(2);
		/*                                  */
		inv_fs = true;
		break;
	case SND_SOC_DAIFMT_DSP_A:
		/*                  */
		regs->rcr2      |= RDATDLY(1);
		regs->xcr2      |= XDATDLY(1);
		/*                                  */
		inv_fs = true;
		break;
	case SND_SOC_DAIFMT_DSP_B:
		/*                  */
		regs->rcr2      |= RDATDLY(0);
		regs->xcr2      |= XDATDLY(0);
		/*                                  */
		inv_fs = true;
		break;
	default:
		/*                         */
		return -EINVAL;
	}

	switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
	case SND_SOC_DAIFMT_CBS_CFS:
		/*                                                */
		regs->pcr0	|= FSXM | FSRM |
				   CLKXM | CLKRM;
		/*                                     */
		regs->srgr2	|= FSGM;
		break;
	case SND_SOC_DAIFMT_CBM_CFM:
		/*             */
		break;
	default:
		/*                                        */
		return -EINVAL;
	}

	/*                                             */
	switch (fmt & SND_SOC_DAIFMT_INV_MASK) {
	case SND_SOC_DAIFMT_NB_NF:
		/*
                      
                                                               
                                                     
   */
		regs->pcr0	|= FSXP | FSRP |
				   CLKXP | CLKRP;
		break;
	case SND_SOC_DAIFMT_NB_IF:
		regs->pcr0	|= CLKXP | CLKRP;
		break;
	case SND_SOC_DAIFMT_IB_NF:
		regs->pcr0	|= FSXP | FSRP;
		break;
	case SND_SOC_DAIFMT_IB_IF:
		break;
	default:
		return -EINVAL;
	}
	if (inv_fs == true)
		regs->pcr0 ^= FSXP | FSRP;

	return 0;
}

static int omap_mcbsp_dai_set_clkdiv(struct snd_soc_dai *cpu_dai,
				     int div_id, int div)
{
	struct omap_mcbsp *mcbsp = snd_soc_dai_get_drvdata(cpu_dai);
	struct omap_mcbsp_reg_cfg *regs = &mcbsp->cfg_regs;

	if (div_id != OMAP_MCBSP_CLKGDV)
		return -ENODEV;

	mcbsp->clk_div = div;
	regs->srgr1	&= ~CLKGDV(0xff);
	regs->srgr1	|= CLKGDV(div - 1);

	return 0;
}

static int omap_mcbsp_dai_set_dai_sysclk(struct snd_soc_dai *cpu_dai,
					 int clk_id, unsigned int freq,
					 int dir)
{
	struct omap_mcbsp *mcbsp = snd_soc_dai_get_drvdata(cpu_dai);
	struct omap_mcbsp_reg_cfg *regs = &mcbsp->cfg_regs;
	int err = 0;

	if (mcbsp->active) {
		if (freq == mcbsp->in_freq)
			return 0;
		else
			return -EBUSY;
	}

	mcbsp->in_freq = freq;
	regs->srgr2 &= ~CLKSM;
	regs->pcr0 &= ~SCLKME;

	switch (clk_id) {
	case OMAP_MCBSP_SYSCLK_CLK:
		regs->srgr2	|= CLKSM;
		break;
	case OMAP_MCBSP_SYSCLK_CLKS_FCLK:
		if (mcbsp_omap1()) {
			err = -EINVAL;
			break;
		}
		err = omap2_mcbsp_set_clks_src(mcbsp,
					       MCBSP_CLKS_PRCM_SRC);
		break;
	case OMAP_MCBSP_SYSCLK_CLKS_EXT:
		if (mcbsp_omap1()) {
			err = 0;
			break;
		}
		err = omap2_mcbsp_set_clks_src(mcbsp,
					       MCBSP_CLKS_PAD_SRC);
		break;

	case OMAP_MCBSP_SYSCLK_CLKX_EXT:
		regs->srgr2	|= CLKSM;
	case OMAP_MCBSP_SYSCLK_CLKR_EXT:
		regs->pcr0	|= SCLKME;
		break;
	default:
		err = -ENODEV;
	}

	return err;
}

static const struct snd_soc_dai_ops mcbsp_dai_ops = {
	.startup	= omap_mcbsp_dai_startup,
	.shutdown	= omap_mcbsp_dai_shutdown,
	.trigger	= omap_mcbsp_dai_trigger,
	.delay		= omap_mcbsp_dai_delay,
	.hw_params	= omap_mcbsp_dai_hw_params,
	.set_fmt	= omap_mcbsp_dai_set_dai_fmt,
	.set_clkdiv	= omap_mcbsp_dai_set_clkdiv,
	.set_sysclk	= omap_mcbsp_dai_set_dai_sysclk,
};

static int omap_mcbsp_probe(struct snd_soc_dai *dai)
{
	struct omap_mcbsp *mcbsp = snd_soc_dai_get_drvdata(dai);

	pm_runtime_enable(mcbsp->dev);

	return 0;
}

static int omap_mcbsp_remove(struct snd_soc_dai *dai)
{
	struct omap_mcbsp *mcbsp = snd_soc_dai_get_drvdata(dai);

	pm_runtime_disable(mcbsp->dev);

	return 0;
}

static struct snd_soc_dai_driver omap_mcbsp_dai = {
	.probe = omap_mcbsp_probe,
	.remove = omap_mcbsp_remove,
	.playback = {
		.channels_min = 1,
		.channels_max = 16,
		.rates = OMAP_MCBSP_RATES,
		.formats = SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FMTBIT_S32_LE,
	},
	.capture = {
		.channels_min = 1,
		.channels_max = 16,
		.rates = OMAP_MCBSP_RATES,
		.formats = SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FMTBIT_S32_LE,
	},
	.ops = &mcbsp_dai_ops,
};

static const struct snd_soc_component_driver omap_mcbsp_component = {
	.name		= "omap-mcbsp",
};

static int omap_mcbsp_st_info_volsw(struct snd_kcontrol *kcontrol,
			struct snd_ctl_elem_info *uinfo)
{
	struct soc_mixer_control *mc =
		(struct soc_mixer_control *)kcontrol->private_value;
	int max = mc->max;
	int min = mc->min;

	uinfo->type = SNDRV_CTL_ELEM_TYPE_INTEGER;
	uinfo->count = 1;
	uinfo->value.integer.min = min;
	uinfo->value.integer.max = max;
	return 0;
}

#define OMAP_MCBSP_ST_CHANNEL_VOLUME(channel)				\
static int								\
omap_mcbsp_set_st_ch##channel##_volume(struct snd_kcontrol *kc,		\
					struct snd_ctl_elem_value *uc)	\
{									\
	struct snd_soc_dai *cpu_dai = snd_kcontrol_chip(kc);		\
	struct omap_mcbsp *mcbsp = snd_soc_dai_get_drvdata(cpu_dai);	\
	struct soc_mixer_control *mc =					\
		(struct soc_mixer_control *)kc->private_value;		\
	int max = mc->max;						\
	int min = mc->min;						\
	int val = uc->value.integer.value[0];				\
									\
	if (val < min || val > max)					\
		return -EINVAL;						\
									\
	/*                                                  */		\
	return omap_st_set_chgain(mcbsp, channel, val);			\
}									\
									\
static int								\
omap_mcbsp_get_st_ch##channel##_volume(struct snd_kcontrol *kc,		\
					struct snd_ctl_elem_value *uc)	\
{									\
	struct snd_soc_dai *cpu_dai = snd_kcontrol_chip(kc);		\
	struct omap_mcbsp *mcbsp = snd_soc_dai_get_drvdata(cpu_dai);	\
	s16 chgain;							\
									\
	if (omap_st_get_chgain(mcbsp, channel, &chgain))		\
		return -EAGAIN;						\
									\
	uc->value.integer.value[0] = chgain;				\
	return 0;							\
}

OMAP_MCBSP_ST_CHANNEL_VOLUME(0)
OMAP_MCBSP_ST_CHANNEL_VOLUME(1)

static int omap_mcbsp_st_put_mode(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_dai *cpu_dai = snd_kcontrol_chip(kcontrol);
	struct omap_mcbsp *mcbsp = snd_soc_dai_get_drvdata(cpu_dai);
	u8 value = ucontrol->value.integer.value[0];

	if (value == omap_st_is_enabled(mcbsp))
		return 0;

	if (value)
		omap_st_enable(mcbsp);
	else
		omap_st_disable(mcbsp);

	return 1;
}

static int omap_mcbsp_st_get_mode(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_dai *cpu_dai = snd_kcontrol_chip(kcontrol);
	struct omap_mcbsp *mcbsp = snd_soc_dai_get_drvdata(cpu_dai);

	ucontrol->value.integer.value[0] = omap_st_is_enabled(mcbsp);
	return 0;
}

#define OMAP_MCBSP_ST_CONTROLS(port)					  \
static const struct snd_kcontrol_new omap_mcbsp##port##_st_controls[] = { \
SOC_SINGLE_EXT("McBSP" #port " Sidetone Switch", 1, 0, 1, 0,		  \
	       omap_mcbsp_st_get_mode, omap_mcbsp_st_put_mode),		  \
OMAP_MCBSP_SOC_SINGLE_S16_EXT("McBSP" #port " Sidetone Channel 0 Volume", \
			      -32768, 32767,				  \
			      omap_mcbsp_get_st_ch0_volume,		  \
			      omap_mcbsp_set_st_ch0_volume),		  \
OMAP_MCBSP_SOC_SINGLE_S16_EXT("McBSP" #port " Sidetone Channel 1 Volume", \
			      -32768, 32767,				  \
			      omap_mcbsp_get_st_ch1_volume,		  \
			      omap_mcbsp_set_st_ch1_volume),		  \
}

OMAP_MCBSP_ST_CONTROLS(2);
OMAP_MCBSP_ST_CONTROLS(3);

int omap_mcbsp_st_add_controls(struct snd_soc_pcm_runtime *rtd)
{
	struct snd_soc_dai *cpu_dai = rtd->cpu_dai;
	struct omap_mcbsp *mcbsp = snd_soc_dai_get_drvdata(cpu_dai);

	if (!mcbsp->st_data) {
		dev_warn(mcbsp->dev, "No sidetone data for port\n");
		return 0;
	}

	switch (mcbsp->id) {
	case 2: /*         */
		return snd_soc_add_dai_controls(cpu_dai,
					omap_mcbsp2_st_controls,
					ARRAY_SIZE(omap_mcbsp2_st_controls));
	case 3: /*         */
		return snd_soc_add_dai_controls(cpu_dai,
					omap_mcbsp3_st_controls,
					ARRAY_SIZE(omap_mcbsp3_st_controls));
	default:
		break;
	}

	return -EINVAL;
}
EXPORT_SYMBOL_GPL(omap_mcbsp_st_add_controls);

static struct omap_mcbsp_platform_data omap2420_pdata = {
	.reg_step = 4,
	.reg_size = 2,
};

static struct omap_mcbsp_platform_data omap2430_pdata = {
	.reg_step = 4,
	.reg_size = 4,
	.has_ccr = true,
};

static struct omap_mcbsp_platform_data omap3_pdata = {
	.reg_step = 4,
	.reg_size = 4,
	.has_ccr = true,
	.has_wakeup = true,
};

static struct omap_mcbsp_platform_data omap4_pdata = {
	.reg_step = 4,
	.reg_size = 4,
	.has_ccr = true,
	.has_wakeup = true,
};

static const struct of_device_id omap_mcbsp_of_match[] = {
	{
		.compatible = "ti,omap2420-mcbsp",
		.data = &omap2420_pdata,
	},
	{
		.compatible = "ti,omap2430-mcbsp",
		.data = &omap2430_pdata,
	},
	{
		.compatible = "ti,omap3-mcbsp",
		.data = &omap3_pdata,
	},
	{
		.compatible = "ti,omap4-mcbsp",
		.data = &omap4_pdata,
	},
	{ },
};
MODULE_DEVICE_TABLE(of, omap_mcbsp_of_match);

static int asoc_mcbsp_probe(struct platform_device *pdev)
{
	struct omap_mcbsp_platform_data *pdata = dev_get_platdata(&pdev->dev);
	struct omap_mcbsp *mcbsp;
	const struct of_device_id *match;
	int ret;

	match = of_match_device(omap_mcbsp_of_match, &pdev->dev);
	if (match) {
		struct device_node *node = pdev->dev.of_node;
		int buffer_size;

		pdata = devm_kzalloc(&pdev->dev,
				     sizeof(struct omap_mcbsp_platform_data),
				     GFP_KERNEL);
		if (!pdata)
			return -ENOMEM;

		memcpy(pdata, match->data, sizeof(*pdata));
		if (!of_property_read_u32(node, "ti,buffer-size", &buffer_size))
			pdata->buffer_size = buffer_size;
	} else if (!pdata) {
		dev_err(&pdev->dev, "missing platform data.\n");
		return -EINVAL;
	}
	mcbsp = devm_kzalloc(&pdev->dev, sizeof(struct omap_mcbsp), GFP_KERNEL);
	if (!mcbsp)
		return -ENOMEM;

	mcbsp->id = pdev->id;
	mcbsp->pdata = pdata;
	mcbsp->dev = &pdev->dev;
	platform_set_drvdata(pdev, mcbsp);

	ret = omap_mcbsp_init(pdev);
	if (!ret)
		return snd_soc_register_component(&pdev->dev, &omap_mcbsp_component,
						  &omap_mcbsp_dai, 1);

	return ret;
}

static int asoc_mcbsp_remove(struct platform_device *pdev)
{
	struct omap_mcbsp *mcbsp = platform_get_drvdata(pdev);

	snd_soc_unregister_component(&pdev->dev);

	if (mcbsp->pdata->ops && mcbsp->pdata->ops->free)
		mcbsp->pdata->ops->free(mcbsp->id);

	omap_mcbsp_sysfs_remove(mcbsp);

	clk_put(mcbsp->fclk);

	platform_set_drvdata(pdev, NULL);

	return 0;
}

static struct platform_driver asoc_mcbsp_driver = {
	.driver = {
			.name = "omap-mcbsp",
			.owner = THIS_MODULE,
			.of_match_table = omap_mcbsp_of_match,
	},

	.probe = asoc_mcbsp_probe,
	.remove = asoc_mcbsp_remove,
};

module_platform_driver(asoc_mcbsp_driver);

MODULE_AUTHOR("Jarkko Nikula <jarkko.nikula@bitmer.com>");
MODULE_DESCRIPTION("OMAP I2S SoC Interface");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:omap-mcbsp");
