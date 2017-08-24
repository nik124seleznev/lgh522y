#ifndef __USBAUDIO_CARD_H
#define __USBAUDIO_CARD_H

#define MAX_NR_RATES	1024
#define MAX_PACKS	20
#define MAX_PACKS_HS	(MAX_PACKS * 8)	/*                    */
#define MAX_URBS	8
#define SYNC_URBS	4	/*                           */
#define MAX_QUEUE	24	/*                                            */

struct audioformat {
	struct list_head list;
	u64 formats;			/*                  */
	unsigned int channels;		/*            */
	unsigned int fmt_type;		/*                             */
	unsigned int frame_size;	/*                                 */
	int iface;			/*                  */
	unsigned char altsetting;	/*                                 */
	unsigned char altset_idx;	/*                                 */
	unsigned char attributes;	/*                                         */
	unsigned char endpoint;		/*          */
	unsigned char ep_attr;		/*                     */
	unsigned char datainterval;	/*                               */
	unsigned int maxpacksize;	/*                  */
	unsigned int rates;		/*               */
	unsigned int rate_min, rate_max;	/*               */
	unsigned int nr_rates;		/*                              */
	unsigned int *rate_table;	/*            */
	unsigned char clock;		/*                  */
	struct snd_pcm_chmap_elem *chmap; /*                        */
	bool dsd_dop;			/*                                        */
	bool dsd_bitrev;		/*                                     */
};

struct snd_usb_substream;
struct snd_usb_endpoint;

struct snd_urb_ctx {
	struct urb *urb;
	unsigned int buffer_size;	/*                                  */
	struct snd_usb_substream *subs;
	struct snd_usb_endpoint *ep;
	int index;	/*                     */
	int packets;	/*                           */
	int packet_size[MAX_PACKS_HS]; /*                                     */
	struct list_head ready_list;
};

struct snd_usb_endpoint {
	struct snd_usb_audio *chip;

	int use_count;
	int ep_num;		/*                                */
	int type;		/*                         */
	unsigned long flags;

	void (*prepare_data_urb) (struct snd_usb_substream *subs,
				  struct urb *urb);
	void (*retire_data_urb) (struct snd_usb_substream *subs,
				 struct urb *urb);

	struct snd_usb_substream *data_subs;
	struct snd_usb_endpoint *sync_master;
	struct snd_usb_endpoint *sync_slave;

	struct snd_urb_ctx urb[MAX_URBS];

	struct snd_usb_packet_info {
		uint32_t packet_size[MAX_PACKS_HS];
		int packets;
	} next_packet[MAX_URBS];
	int next_packet_read_pos, next_packet_write_pos;
	struct list_head ready_playback_urbs;

	unsigned int nurbs;		/*        */
	unsigned long active_mask;	/*                        */
	unsigned long unlink_mask;	/*                          */
	char *syncbuf;			/*                               */
	dma_addr_t sync_dma;		/*                        */

	unsigned int pipe;		/*                   */
	unsigned int freqn;		/*                                                  */
	unsigned int freqm;		/*                                                    */
	int	   freqshift;		/*                                                    */
	unsigned int freqmax;		/*                                                   */
	unsigned int phase;		/*                   */
	unsigned int maxpacksize;	/*                          */
	unsigned int maxframesize;      /*                           */
	unsigned int curpacksize;	/*                                            */
	unsigned int curframesize;      /*                                             */
	unsigned int syncmaxsize;	/*                           */
	unsigned int fill_max:1;	/*                             */
	unsigned int udh01_fb_quirk:1;	/*                         */
	unsigned int datainterval;      /*                               */
	unsigned int syncinterval;	/*                                  */
	unsigned char silence_value;
	unsigned int stride;
	int iface, alt_idx;
	int skip_packets;		/*                                                 
                    */

	spinlock_t lock;
	struct list_head list;
};

struct snd_usb_substream {
	struct snd_usb_stream *stream;
	struct usb_device *dev;
	struct snd_pcm_substream *pcm_substream;
	int direction;	/*                     */
	int interface;	/*                   */
	int endpoint;	/*                   */
	struct audioformat *cur_audiofmt;	/*                                                      */
	snd_pcm_format_t pcm_format;	/*                                               */
	unsigned int channels;		/*                                                     */
	unsigned int channels_max;	/*                                   */
	unsigned int cur_rate;		/*                                       */
	unsigned int period_bytes;	/*                                               */
	unsigned int altset_idx;     /*                                             */
	unsigned int txfr_quirk:1;	/*                           */
	unsigned int fmt_type;		/*                             */
	unsigned int pkt_offset_adj;	/*                                                                     */

	unsigned int running: 1;	/*                */

	unsigned int hwptr_done;	/*                                       */
	unsigned int transfer_done;		/*                                           */

	/*                                         */
	unsigned int ep_num;		/*                     */
	struct snd_usb_endpoint *data_endpoint;
	struct snd_usb_endpoint *sync_endpoint;
	unsigned long flags;
	bool need_setup_ep;		/*                              */
	unsigned int speed;		/*               */

	u64 formats;			/*                             */
	unsigned int num_formats;		/*                                          */
	struct list_head fmt_list;	/*             */
	struct snd_pcm_hw_constraint_list rate_list;	/*               */
	spinlock_t lock;

	int last_frame_number;          /*                     */
	int last_delay;                 /*              */

	struct {
		int marker;
		int channel;
		int byte_idx;
	} dsd_dop;
};

struct snd_usb_stream {
	struct snd_usb_audio *chip;
	struct snd_pcm *pcm;
	int pcm_index;
	unsigned int fmt_type;		/*                             */
	struct snd_usb_substream substream[2];
	struct list_head list;
};

#endif /*                   */
