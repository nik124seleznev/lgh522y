/*
 *  drivers/media/radio/si470x/radio-si470x-common.c
 *
 *  Driver for radios with Silicon Labs Si470x FM Radio Receivers
 *
 *  Copyright (c) 2009 Tobias Lorenz <tobias.lorenz@gmx.net>
 *  Copyright (c) 2012 Hans de Goede <hdegoede@redhat.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */


/*
           
                                                   
                 
                           
                                                   
                 
                                                               
                                                  
                                                            
                                                         
                                                   
                                            
                                        
                                                   
                  
                                    
                                                       
                                         
                                                                   
                                                                 
                                    
                                                      
                                                       
                                                   
                 
                                                  
                                                     
                           
                                                 
                                                                   
                                                                 
                                                   
                 
                                                                 
                                                
                                              
                                                                   
                                    
                                                   
                 
                                                    
                                                                  
                                                
                            
                                                   
                                     
                 
                                                                
                                                        
                                                               
                              
                                          
                                           
                              
                                    
                                              
                              
                                                   
                                     
                 
                             
                      
                                                   
                 
                                     
                    
                                                          
                                                      
                                                    
                                                
                                                           
                                         
                                           
                                        
                                            
                                                     
                                         
                                         
                           
                                       
                                                        
                  
                                                      
                                     
                                                      
 */


/*                 */
#include "radio-si470x.h"



/*                                                                         
                    
                                                                          */

/*               */
/*                             */
/*                            */
/*            */
static unsigned short space = 2;
module_param(space, ushort, 0444);
MODULE_PARM_DESC(space, "Spacing: 0=200kHz 1=100kHz *2=50kHz*");

/*             */
/*                */
/*                                     */
static unsigned short de = 1;
module_param(de, ushort, 0444);
MODULE_PARM_DESC(de, "De-emphasis: 0=75us *1=50us*");

/*              */
static unsigned int tune_timeout = 3000;
module_param(tune_timeout, uint, 0644);
MODULE_PARM_DESC(tune_timeout, "Tune timeout: *3000*");

/*              */
static unsigned int seek_timeout = 5000;
module_param(seek_timeout, uint, 0644);
MODULE_PARM_DESC(seek_timeout, "Seek timeout: *5000*");

static const struct v4l2_frequency_band bands[] = {
	{
		.type = V4L2_TUNER_RADIO,
		.index = 0,
		.capability = V4L2_TUNER_CAP_LOW | V4L2_TUNER_CAP_STEREO |
			    V4L2_TUNER_CAP_RDS | V4L2_TUNER_CAP_RDS_BLOCK_IO |
			    V4L2_TUNER_CAP_FREQ_BANDS |
			    V4L2_TUNER_CAP_HWSEEK_BOUNDED |
			    V4L2_TUNER_CAP_HWSEEK_WRAP,
		.rangelow   =  87500 * 16,
		.rangehigh  = 108000 * 16,
		.modulation = V4L2_BAND_MODULATION_FM,
	},
	{
		.type = V4L2_TUNER_RADIO,
		.index = 1,
		.capability = V4L2_TUNER_CAP_LOW | V4L2_TUNER_CAP_STEREO |
			    V4L2_TUNER_CAP_RDS | V4L2_TUNER_CAP_RDS_BLOCK_IO |
			    V4L2_TUNER_CAP_FREQ_BANDS |
			    V4L2_TUNER_CAP_HWSEEK_BOUNDED |
			    V4L2_TUNER_CAP_HWSEEK_WRAP,
		.rangelow   =  76000 * 16,
		.rangehigh  = 108000 * 16,
		.modulation = V4L2_BAND_MODULATION_FM,
	},
	{
		.type = V4L2_TUNER_RADIO,
		.index = 2,
		.capability = V4L2_TUNER_CAP_LOW | V4L2_TUNER_CAP_STEREO |
			    V4L2_TUNER_CAP_RDS | V4L2_TUNER_CAP_RDS_BLOCK_IO |
			    V4L2_TUNER_CAP_FREQ_BANDS |
			    V4L2_TUNER_CAP_HWSEEK_BOUNDED |
			    V4L2_TUNER_CAP_HWSEEK_WRAP,
		.rangelow   =  76000 * 16,
		.rangehigh  =  90000 * 16,
		.modulation = V4L2_BAND_MODULATION_FM,
	},
};

/*                                                                         
                    
                                                                          */

/*
                                 
 */
static int si470x_set_band(struct si470x_device *radio, int band)
{
	if (radio->band == band)
		return 0;

	radio->band = band;
	radio->registers[SYSCONFIG2] &= ~SYSCONFIG2_BAND;
	radio->registers[SYSCONFIG2] |= radio->band << 6;
	return si470x_set_register(radio, SYSCONFIG2);
}

/*
                                    
 */
static int si470x_set_chan(struct si470x_device *radio, unsigned short chan)
{
	int retval;
	bool timed_out = 0;

	/*              */
	radio->registers[CHANNEL] &= ~CHANNEL_CHAN;
	radio->registers[CHANNEL] |= CHANNEL_TUNE | chan;
	retval = si470x_set_register(radio, CHANNEL);
	if (retval < 0)
		goto done;

	/*                                        */
	INIT_COMPLETION(radio->completion);
	retval = wait_for_completion_timeout(&radio->completion,
			msecs_to_jiffies(tune_timeout));
	if (!retval)
		timed_out = true;

	if ((radio->registers[STATUSRSSI] & STATUSRSSI_STC) == 0)
		dev_warn(&radio->videodev.dev, "tune does not complete\n");
	if (timed_out)
		dev_warn(&radio->videodev.dev,
			"tune timed out after %u ms\n", tune_timeout);

	/*             */
	radio->registers[CHANNEL] &= ~CHANNEL_TUNE;
	retval = si470x_set_register(radio, CHANNEL);

done:
	return retval;
}

/*
                                        
 */
static unsigned int si470x_get_step(struct si470x_device *radio)
{
	/*               */
	switch ((radio->registers[SYSCONFIG2] & SYSCONFIG2_SPACE) >> 4) {
	/*                             */
	case 0:
		return 200 * 16;
	/*                            */
	case 1:
		return 100 * 16;
	/*            */
	default:
		return 50 * 16;
	};
}


/*
                                      
 */
static int si470x_get_freq(struct si470x_device *radio, unsigned int *freq)
{
	int chan, retval;

	/*              */
	retval = si470x_get_register(radio, READCHAN);
	chan = radio->registers[READCHAN] & READCHAN_READCHAN;

	/*                                                                  */
	*freq = chan * si470x_get_step(radio) + bands[radio->band].rangelow;

	return retval;
}


/*
                                      
 */
int si470x_set_freq(struct si470x_device *radio, unsigned int freq)
{
	unsigned short chan;

	freq = clamp(freq, bands[radio->band].rangelow,
			   bands[radio->band].rangehigh);
	/*                                                              */
	chan = (freq - bands[radio->band].rangelow) / si470x_get_step(radio);

	return si470x_set_chan(radio, chan);
}


/*
                             
 */
static int si470x_set_seek(struct si470x_device *radio,
			   const struct v4l2_hw_freq_seek *seek)
{
	int band, retval;
	unsigned int freq;
	bool timed_out = 0;

	/*          */
	if (seek->rangelow || seek->rangehigh) {
		for (band = 0; band < ARRAY_SIZE(bands); band++) {
			if (bands[band].rangelow  == seek->rangelow &&
			    bands[band].rangehigh == seek->rangehigh)
				break;
		}
		if (band == ARRAY_SIZE(bands))
			return -EINVAL; /*                        */
	} else
		band = 1; /*                                           */

	if (radio->band != band) {
		retval = si470x_get_freq(radio, &freq);
		if (retval)
			return retval;
		retval = si470x_set_band(radio, band);
		if (retval)
			return retval;
		retval = si470x_set_freq(radio, freq);
		if (retval)
			return retval;
	}

	/*               */
	radio->registers[POWERCFG] |= POWERCFG_SEEK;
	if (seek->wrap_around)
		radio->registers[POWERCFG] &= ~POWERCFG_SKMODE;
	else
		radio->registers[POWERCFG] |= POWERCFG_SKMODE;
	if (seek->seek_upward)
		radio->registers[POWERCFG] |= POWERCFG_SEEKUP;
	else
		radio->registers[POWERCFG] &= ~POWERCFG_SEEKUP;
	retval = si470x_set_register(radio, POWERCFG);
	if (retval < 0)
		return retval;

	/*                                        */
	INIT_COMPLETION(radio->completion);
	retval = wait_for_completion_timeout(&radio->completion,
			msecs_to_jiffies(seek_timeout));
	if (!retval)
		timed_out = true;

	if ((radio->registers[STATUSRSSI] & STATUSRSSI_STC) == 0)
		dev_warn(&radio->videodev.dev, "seek does not complete\n");
	if (radio->registers[STATUSRSSI] & STATUSRSSI_SF)
		dev_warn(&radio->videodev.dev,
			"seek failed / band limit reached\n");

	/*              */
	radio->registers[POWERCFG] &= ~POWERCFG_SEEK;
	retval = si470x_set_register(radio, POWERCFG);

	/*                         */
	if (retval == 0 && timed_out)
		return -ENODATA;
	return retval;
}


/*
                                 
 */
int si470x_start(struct si470x_device *radio)
{
	int retval;

	/*          */
	radio->registers[POWERCFG] =
		POWERCFG_DMUTE | POWERCFG_ENABLE | POWERCFG_RDSM;
	retval = si470x_set_register(radio, POWERCFG);
	if (retval < 0)
		goto done;

	/*             */
	radio->registers[SYSCONFIG1] =
		(de << 11) & SYSCONFIG1_DE;		/*   */
	retval = si470x_set_register(radio, SYSCONFIG1);
	if (retval < 0)
		goto done;

	/*             */
	radio->registers[SYSCONFIG2] =
		(0x1f  << 8) |				/*        */
		((radio->band << 6) & SYSCONFIG2_BAND) |/*      */
		((space << 4) & SYSCONFIG2_SPACE) |	/*       */
		15;					/*              */
	retval = si470x_set_register(radio, SYSCONFIG2);
	if (retval < 0)
		goto done;

	/*                    */
	retval = si470x_set_chan(radio,
		radio->registers[CHANNEL] & CHANNEL_CHAN);

done:
	return retval;
}


/*
                                 
 */
int si470x_stop(struct si470x_device *radio)
{
	int retval;

	/*             */
	radio->registers[SYSCONFIG1] &= ~SYSCONFIG1_RDS;
	retval = si470x_set_register(radio, SYSCONFIG1);
	if (retval < 0)
		goto done;

	/*          */
	radio->registers[POWERCFG] &= ~POWERCFG_DMUTE;
	/*                                             */
	radio->registers[POWERCFG] |= POWERCFG_ENABLE |	POWERCFG_DISABLE;
	retval = si470x_set_register(radio, POWERCFG);

done:
	return retval;
}


/*
                                          
 */
static int si470x_rds_on(struct si470x_device *radio)
{
	int retval;

	/*             */
	radio->registers[SYSCONFIG1] |= SYSCONFIG1_RDS;
	retval = si470x_set_register(radio, SYSCONFIG1);
	if (retval < 0)
		radio->registers[SYSCONFIG1] &= ~SYSCONFIG1_RDS;

	return retval;
}



/*                                                                         
                            
                                                                          */

/*
                                   
 */
static ssize_t si470x_fops_read(struct file *file, char __user *buf,
		size_t count, loff_t *ppos)
{
	struct si470x_device *radio = video_drvdata(file);
	int retval = 0;
	unsigned int block_count = 0;

	/*                         */
	if ((radio->registers[SYSCONFIG1] & SYSCONFIG1_RDS) == 0)
		si470x_rds_on(radio);

	/*                                */
	while (radio->wr_index == radio->rd_index) {
		if (file->f_flags & O_NONBLOCK) {
			retval = -EWOULDBLOCK;
			goto done;
		}
		if (wait_event_interruptible(radio->read_queue,
			radio->wr_index != radio->rd_index) < 0) {
			retval = -EINTR;
			goto done;
		}
	}

	/*                                       */
	count /= 3;

	/*                                                          */
	while (block_count < count) {
		if (radio->rd_index == radio->wr_index)
			break;

		/*                                     */
		if (copy_to_user(buf, &radio->buffer[radio->rd_index], 3))
			/*                   */
			break;

		/*                                 */
		radio->rd_index += 3;
		if (radio->rd_index >= radio->buf_size)
			radio->rd_index = 0;

		/*                    */
		block_count++;
		buf += 3;
		retval += 3;
	}

done:
	return retval;
}


/*
                                   
 */
static unsigned int si470x_fops_poll(struct file *file,
		struct poll_table_struct *pts)
{
	struct si470x_device *radio = video_drvdata(file);
	unsigned long req_events = poll_requested_events(pts);
	int retval = v4l2_ctrl_poll(file, pts);

	if (req_events & (POLLIN | POLLRDNORM)) {
		/*                         */
		if ((radio->registers[SYSCONFIG1] & SYSCONFIG1_RDS) == 0)
			si470x_rds_on(radio);

		poll_wait(file, &radio->read_queue, pts);

		if (radio->rd_index != radio->wr_index)
			retval |= POLLIN | POLLRDNORM;
	}

	return retval;
}


/*
                                          
 */
static const struct v4l2_file_operations si470x_fops = {
	.owner			= THIS_MODULE,
	.read			= si470x_fops_read,
	.poll			= si470x_fops_poll,
	.unlocked_ioctl		= video_ioctl2,
	.open			= si470x_fops_open,
	.release		= si470x_fops_release,
};



/*                                                                         
                        
                                                                          */


static int si470x_s_ctrl(struct v4l2_ctrl *ctrl)
{
	struct si470x_device *radio =
		container_of(ctrl->handler, struct si470x_device, hdl);

	switch (ctrl->id) {
	case V4L2_CID_AUDIO_VOLUME:
		radio->registers[SYSCONFIG2] &= ~SYSCONFIG2_VOLUME;
		radio->registers[SYSCONFIG2] |= ctrl->val;
		return si470x_set_register(radio, SYSCONFIG2);
	case V4L2_CID_AUDIO_MUTE:
		if (ctrl->val)
			radio->registers[POWERCFG] &= ~POWERCFG_DMUTE;
		else
			radio->registers[POWERCFG] |= POWERCFG_DMUTE;
		return si470x_set_register(radio, POWERCFG);
	default:
		return -EINVAL;
	}
}


/*
                                               
 */
static int si470x_vidioc_g_tuner(struct file *file, void *priv,
		struct v4l2_tuner *tuner)
{
	struct si470x_device *radio = video_drvdata(file);
	int retval = 0;

	if (tuner->index != 0)
		return -EINVAL;

	if (!radio->status_rssi_auto_update) {
		retval = si470x_get_register(radio, STATUSRSSI);
		if (retval < 0)
			return retval;
	}

	/*                  */
	strcpy(tuner->name, "FM");
	tuner->type = V4L2_TUNER_RADIO;
	tuner->capability = V4L2_TUNER_CAP_LOW | V4L2_TUNER_CAP_STEREO |
			    V4L2_TUNER_CAP_RDS | V4L2_TUNER_CAP_RDS_BLOCK_IO |
			    V4L2_TUNER_CAP_HWSEEK_BOUNDED |
			    V4L2_TUNER_CAP_HWSEEK_WRAP;
	tuner->rangelow  =  76 * FREQ_MUL;
	tuner->rangehigh = 108 * FREQ_MUL;

	/*                                              */
	if ((radio->registers[STATUSRSSI] & STATUSRSSI_ST) == 0)
		tuner->rxsubchans = V4L2_TUNER_SUB_MONO;
	else
		tuner->rxsubchans = V4L2_TUNER_SUB_STEREO;
	/*                                                           
                                                            
                    */
	tuner->rxsubchans |= V4L2_TUNER_SUB_RDS;

	/*                      */
	if ((radio->registers[POWERCFG] & POWERCFG_MONO) == 0)
		tuner->audmode = V4L2_TUNER_MODE_STEREO;
	else
		tuner->audmode = V4L2_TUNER_MODE_MONO;

	/*                                                            */
	/*                                                                  */
	tuner->signal = (radio->registers[STATUSRSSI] & STATUSRSSI_RSSI);
	/*                                       */
	tuner->signal = (tuner->signal * 873) + (8 * tuner->signal / 10);
	if (tuner->signal > 0xffff)
		tuner->signal = 0xffff;

	/*                                                              */
	/*                                                                  */
	tuner->afc = (radio->registers[STATUSRSSI] & STATUSRSSI_AFCRL) ? 1 : 0;

	return retval;
}


/*
                                               
 */
static int si470x_vidioc_s_tuner(struct file *file, void *priv,
		const struct v4l2_tuner *tuner)
{
	struct si470x_device *radio = video_drvdata(file);

	if (tuner->index != 0)
		return -EINVAL;

	/*                      */
	switch (tuner->audmode) {
	case V4L2_TUNER_MODE_MONO:
		radio->registers[POWERCFG] |= POWERCFG_MONO;  /*            */
		break;
	case V4L2_TUNER_MODE_STEREO:
	default:
		radio->registers[POWERCFG] &= ~POWERCFG_MONO; /*            */
		break;
	}

	return si470x_set_register(radio, POWERCFG);
}


/*
                                                                     
 */
static int si470x_vidioc_g_frequency(struct file *file, void *priv,
		struct v4l2_frequency *freq)
{
	struct si470x_device *radio = video_drvdata(file);

	if (freq->tuner != 0)
		return -EINVAL;

	freq->type = V4L2_TUNER_RADIO;
	return si470x_get_freq(radio, &freq->frequency);
}


/*
                                                                     
 */
static int si470x_vidioc_s_frequency(struct file *file, void *priv,
		const struct v4l2_frequency *freq)
{
	struct si470x_device *radio = video_drvdata(file);
	int retval;

	if (freq->tuner != 0)
		return -EINVAL;

	if (freq->frequency < bands[radio->band].rangelow ||
	    freq->frequency > bands[radio->band].rangehigh) {
		/*                                                     */
		retval = si470x_set_band(radio, 1);
		if (retval)
			return retval;
	}
	return si470x_set_freq(radio, freq->frequency);
}


/*
                                                             
 */
static int si470x_vidioc_s_hw_freq_seek(struct file *file, void *priv,
		const struct v4l2_hw_freq_seek *seek)
{
	struct si470x_device *radio = video_drvdata(file);

	if (seek->tuner != 0)
		return -EINVAL;

	if (file->f_flags & O_NONBLOCK)
		return -EWOULDBLOCK;

	return si470x_set_seek(radio, seek);
}

/*
                                                            
 */
static int si470x_vidioc_enum_freq_bands(struct file *file, void *priv,
					 struct v4l2_frequency_band *band)
{
	if (band->tuner != 0)
		return -EINVAL;
	if (band->index >= ARRAY_SIZE(bands))
		return -EINVAL;
	*band = bands[band->index];
	return 0;
}

const struct v4l2_ctrl_ops si470x_ctrl_ops = {
	.s_ctrl = si470x_s_ctrl,
};

/*
                                                   
 */
static const struct v4l2_ioctl_ops si470x_ioctl_ops = {
	.vidioc_querycap	= si470x_vidioc_querycap,
	.vidioc_g_tuner		= si470x_vidioc_g_tuner,
	.vidioc_s_tuner		= si470x_vidioc_s_tuner,
	.vidioc_g_frequency	= si470x_vidioc_g_frequency,
	.vidioc_s_frequency	= si470x_vidioc_s_frequency,
	.vidioc_s_hw_freq_seek	= si470x_vidioc_s_hw_freq_seek,
	.vidioc_enum_freq_bands = si470x_vidioc_enum_freq_bands,
	.vidioc_subscribe_event = v4l2_ctrl_subscribe_event,
	.vidioc_unsubscribe_event = v4l2_event_unsubscribe,
};


/*
                                                  
 */
struct video_device si470x_viddev_template = {
	.fops			= &si470x_fops,
	.name			= DRIVER_NAME,
	.release		= video_device_release_empty,
	.ioctl_ops		= &si470x_ioctl_ops,
};
