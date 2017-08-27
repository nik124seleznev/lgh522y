/*
 * For the STS-Thompson TDA7432 audio processor chip
 *
 * Handles audio functions: volume, balance, tone, loudness
 * This driver will not complain if used with any
 * other i2c device with the same address.
 *
 * Muting and tone control by Jonathan Isom <jisom@ematic.com>
 *
 * Copyright (c) 2000 Eric Sandeen <eric_sandeen@bigfoot.com>
 * Copyright (c) 2006 Mauro Carvalho Chehab <mchehab@infradead.org>
 * This code is placed under the terms of the GNU General Public License
 * Based on tda9855.c by Steve VanDeBogart (vandebo@uclink.berkeley.edu)
 * Which was based on tda8425.c by Greg Alexander (c) 1998
 *
 * OPTIONS:
 * debug    - set to 1 if you'd like to see debug messages
 *            set to 2 if you'd like to be inundated with debug messages
 *
 * loudness - set between 0 and 15 for varying degrees of loudness effect
 *
 * maxvol   - set maximium volume to +20db (1), default is 0db(0)
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/videodev2.h>
#include <linux/i2c.h>

#include <media/v4l2-device.h>
#include <media/v4l2-ioctl.h>
#include <media/v4l2-ctrls.h>
#include <media/i2c-addr.h>

#ifndef VIDEO_AUDIO_BALANCE
# define VIDEO_AUDIO_BALANCE 32
#endif

MODULE_AUTHOR("Eric Sandeen <eric_sandeen@bigfoot.com>");
MODULE_DESCRIPTION("bttv driver for the tda7432 audio processor chip");
MODULE_LICENSE("GPL");

static int maxvol;
static int loudness; /*                             */
static int debug;	 /*                  */
module_param(debug, int, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(debug, "Set debugging level from 0 to 3. Default is off(0).");
module_param(loudness, int, S_IRUGO);
MODULE_PARM_DESC(loudness, "Turn loudness on(1) else off(0). Default is off(0).");
module_param(maxvol, int, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(maxvol, "Set maximium volume to +20dB(0) else +0dB(1). Default is +20dB(0).");


/*                                                       */

struct tda7432 {
	struct v4l2_subdev sd;
	struct v4l2_ctrl_handler hdl;
	struct {
		/*                     */
		struct v4l2_ctrl *bass;
		struct v4l2_ctrl *treble;
	};
	struct {
		/*                      */
		struct v4l2_ctrl *mute;
		struct v4l2_ctrl *balance;
	};
};

static inline struct tda7432 *to_state(struct v4l2_subdev *sd)
{
	return container_of(sd, struct tda7432, sd);
}

static inline struct v4l2_subdev *to_sd(struct v4l2_ctrl *ctrl)
{
	return &container_of(ctrl->handler, struct tda7432, hdl)->sd;
}

/*                                    
                    
                                                    
  
                                                    
  
                                                                   
                                                                   
                                                                 
                                                                 
                                     
 */

		/*                          */

#define TDA7432_IN	0x00 /*                              */
#define TDA7432_VL	0x01 /*                              */
#define TDA7432_TN	0x02 /*                              */
#define TDA7432_LF	0x03 /*                              */
#define TDA7432_LR	0x04 /*                              */
#define TDA7432_RF	0x05 /*                              */
#define TDA7432_RR	0x06 /*                              */
#define TDA7432_LD	0x07 /*                              */


		/*                                        */

/*                                                 */

/*                                                    */

/*                          
                      
                    
                                                             
                                                          
                                                               
  
                                                                   
                                                                 
  
                          
 */

#define TDA7432_STEREO_IN	0
#define TDA7432_MONO_IN		2	/*                        */
#define TDA7432_BASS_SYM	1 << 3
#define TDA7432_BASS_NORM	1 << 4

/*                          */

/*                                                             
                                
  
              
              
              
              
  
                                                        
 */

#define	TDA7432_VOL_0DB		0x20
#define TDA7432_LD_ON		1 << 7


/*                                */

/*                                                         
                          
  
                                                
               
                      
  
                                                         
                                                         
                               
  
                                              
                    
                           
  
           
                                            
 */

#define TDA7432_TREBLE_0DB		0xf
#define TDA7432_TREBLE			7
#define TDA7432_TREBLE_GAIN		1 << 3
#define TDA7432_BASS_0DB		0xf
#define TDA7432_BASS			7 << 4
#define TDA7432_BASS_GAIN		1 << 7


/*                                           */
/*                                           */
/*                                           */
/*                                           */

/*                                                       
                  
  
                  
                  
  
                                                           
                                                  
                  
 */

#define TDA7432_ATTEN_0DB	0x00
#define TDA7432_MUTE        0x1 << 5


/*                                    */

/*                                                             
                        
  
               
               
  
                                                                 
                              
  
                    
 */



/*            */

static int tda7432_write(struct v4l2_subdev *sd, int subaddr, int val)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	unsigned char buffer[2];

	v4l2_dbg(2, debug, sd, "In tda7432_write\n");
	v4l2_dbg(1, debug, sd, "Writing %d 0x%x\n", subaddr, val);
	buffer[0] = subaddr;
	buffer[1] = val;
	if (2 != i2c_master_send(client, buffer, 2)) {
		v4l2_err(sd, "I/O error, trying (write %d 0x%x)\n",
		       subaddr, val);
		return -1;
	}
	return 0;
}

static int tda7432_set(struct v4l2_subdev *sd)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	unsigned char buf[16];

	buf[0]  = TDA7432_IN;
	buf[1]  = TDA7432_STEREO_IN |  /*                       */
		  TDA7432_BASS_SYM  |  /*                       */
		  TDA7432_BASS_NORM;   /*                       */
	buf[2]  = 0x3b;
	if (loudness)			 /*                       */
		buf[2] |= TDA7432_LD_ON;
	buf[3]  = TDA7432_TREBLE_0DB | (TDA7432_BASS_0DB << 4);
	buf[4]  = TDA7432_ATTEN_0DB;
	buf[5]  = TDA7432_ATTEN_0DB;
	buf[6]  = TDA7432_ATTEN_0DB;
	buf[7]  = TDA7432_ATTEN_0DB;
	buf[8]  = loudness;
	if (9 != i2c_master_send(client, buf, 9)) {
		v4l2_err(sd, "I/O error, trying tda7432_set\n");
		return -1;
	}

	return 0;
}

static int tda7432_log_status(struct v4l2_subdev *sd)
{
	struct tda7432 *state = to_state(sd);

	v4l2_ctrl_handler_log_status(&state->hdl, sd->name);
	return 0;
}

static int tda7432_s_ctrl(struct v4l2_ctrl *ctrl)
{
	struct v4l2_subdev *sd = to_sd(ctrl);
	struct tda7432 *t = to_state(sd);
	u8 bass, treble, volume;
	u8 lf, lr, rf, rr;

	switch (ctrl->id) {
	case V4L2_CID_AUDIO_MUTE:
		if (t->balance->val < 0) {
			/*                                  */
			rr = rf = -t->balance->val;
			lr = lf = TDA7432_ATTEN_0DB;
		} else if (t->balance->val > 0) {
			/*                                  */
			rr = rf = TDA7432_ATTEN_0DB;
			lr = lf = t->balance->val;
		} else {
			/*          */
			rr = rf = TDA7432_ATTEN_0DB;
			lr = lf = TDA7432_ATTEN_0DB;
		}
		if (t->mute->val) {
			lf |= TDA7432_MUTE;
			lr |= TDA7432_MUTE;
			rf |= TDA7432_MUTE;
			rr |= TDA7432_MUTE;
		}
		/*                      */
		tda7432_write(sd, TDA7432_LF, lf);
		tda7432_write(sd, TDA7432_LR, lr);
		tda7432_write(sd, TDA7432_RF, rf);
		tda7432_write(sd, TDA7432_RR, rr);
		return 0;
	case V4L2_CID_AUDIO_VOLUME:
		volume = 0x6f - ctrl->val;
		if (loudness)		/*                          */
			volume |= TDA7432_LD_ON;

		tda7432_write(sd, TDA7432_VL, volume);
		return 0;
	case V4L2_CID_AUDIO_BASS:
		bass = t->bass->val;
		treble = t->treble->val;
		if (bass >= 0x8)
			bass = 14 - (bass - 8);
		if (treble >= 0x8)
			treble = 14 - (treble - 8);

		tda7432_write(sd, TDA7432_TN, 0x10 | (bass << 4) | treble);
		return 0;
	}
	return -EINVAL;
}

/*                                                                         */

static const struct v4l2_ctrl_ops tda7432_ctrl_ops = {
	.s_ctrl = tda7432_s_ctrl,
};

static const struct v4l2_subdev_core_ops tda7432_core_ops = {
	.log_status = tda7432_log_status,
	.g_ext_ctrls = v4l2_subdev_g_ext_ctrls,
	.try_ext_ctrls = v4l2_subdev_try_ext_ctrls,
	.s_ext_ctrls = v4l2_subdev_s_ext_ctrls,
	.g_ctrl = v4l2_subdev_g_ctrl,
	.s_ctrl = v4l2_subdev_s_ctrl,
	.queryctrl = v4l2_subdev_queryctrl,
	.querymenu = v4l2_subdev_querymenu,
};

static const struct v4l2_subdev_ops tda7432_ops = {
	.core = &tda7432_core_ops,
};

/*                                                                         */

/*                          
                            
                           */

static int tda7432_probe(struct i2c_client *client,
			const struct i2c_device_id *id)
{
	struct tda7432 *t;
	struct v4l2_subdev *sd;

	v4l_info(client, "chip found @ 0x%02x (%s)\n",
			client->addr << 1, client->adapter->name);

	t = kzalloc(sizeof(*t), GFP_KERNEL);
	if (!t)
		return -ENOMEM;
	sd = &t->sd;
	v4l2_i2c_subdev_init(sd, client, &tda7432_ops);
	v4l2_ctrl_handler_init(&t->hdl, 5);
	v4l2_ctrl_new_std(&t->hdl, &tda7432_ctrl_ops,
		V4L2_CID_AUDIO_VOLUME, 0, maxvol ? 0x68 : 0x4f, 1, maxvol ? 0x5d : 0x47);
	t->mute = v4l2_ctrl_new_std(&t->hdl, &tda7432_ctrl_ops,
		V4L2_CID_AUDIO_MUTE, 0, 1, 1, 0);
	t->balance = v4l2_ctrl_new_std(&t->hdl, &tda7432_ctrl_ops,
		V4L2_CID_AUDIO_BALANCE, -31, 31, 1, 0);
	t->bass = v4l2_ctrl_new_std(&t->hdl, &tda7432_ctrl_ops,
		V4L2_CID_AUDIO_BASS, 0, 14, 1, 7);
	t->treble = v4l2_ctrl_new_std(&t->hdl, &tda7432_ctrl_ops,
		V4L2_CID_AUDIO_TREBLE, 0, 14, 1, 7);
	sd->ctrl_handler = &t->hdl;
	if (t->hdl.error) {
		int err = t->hdl.error;

		v4l2_ctrl_handler_free(&t->hdl);
		kfree(t);
		return err;
	}
	v4l2_ctrl_cluster(2, &t->bass);
	v4l2_ctrl_cluster(2, &t->mute);
	v4l2_ctrl_handler_setup(&t->hdl);
	if (loudness < 0 || loudness > 15) {
		v4l2_warn(sd, "loudness parameter must be between 0 and 15\n");
		if (loudness < 0)
			loudness = 0;
		if (loudness > 15)
			loudness = 15;
	}

	tda7432_set(sd);
	return 0;
}

static int tda7432_remove(struct i2c_client *client)
{
	struct v4l2_subdev *sd = i2c_get_clientdata(client);
	struct tda7432 *t = to_state(sd);

	tda7432_set(sd);
	v4l2_device_unregister_subdev(sd);
	v4l2_ctrl_handler_free(&t->hdl);
	kfree(t);
	return 0;
}

static const struct i2c_device_id tda7432_id[] = {
	{ "tda7432", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, tda7432_id);

static struct i2c_driver tda7432_driver = {
	.driver = {
		.owner	= THIS_MODULE,
		.name	= "tda7432",
	},
	.probe		= tda7432_probe,
	.remove		= tda7432_remove,
	.id_table	= tda7432_id,
};

module_i2c_driver(tda7432_driver);
