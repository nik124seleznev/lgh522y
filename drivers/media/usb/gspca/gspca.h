#ifndef GSPCAV2_H
#define GSPCAV2_H

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>
#include <linux/videodev2.h>
#include <media/v4l2-common.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-device.h>
#include <linux/mutex.h>



/*                   */

#define D_PROBE  1
#define D_CONF   2
#define D_STREAM 3
#define D_FRAM   4
#define D_PACK   5
#define D_USBI   6
#define D_USBO   7

extern int gspca_debug;


#define PDEBUG(level, fmt, ...) \
	v4l2_dbg(level, gspca_debug, &gspca_dev->v4l2_dev, fmt, ##__VA_ARGS__)

#define PERR(fmt, ...) \
	v4l2_err(&gspca_dev->v4l2_dev, fmt, ##__VA_ARGS__)

#define GSPCA_MAX_FRAMES 16	/*                                       */
/*                 */
#define MAX_NURBS 4		/*                    */


/*                                                                 */
struct framerates {
	const u8 *rates;
	int nrates;
};

/*                                        */
struct cam {
	const struct v4l2_pix_format *cam_mode;	/*             */
	const struct framerates *mode_framerates; /*                       
                              */
	u32 bulk_size;		/*                                         */
	u32 input_flags;	/*                                   */
	u8 nmodes;		/*                  */
	u8 no_urb_create;	/*                            */
	u8 bulk_nurbs;		/*                            
                              
                                        
                                            */
	u8 bulk;		/*                                   */
	u8 npkt;		/*                                     
                                          */
	u8 needs_full_bandwidth;/*                                            
                                                   
                                             */
};

struct gspca_dev;
struct gspca_frame;

/*                      */
typedef int (*cam_op) (struct gspca_dev *);
typedef void (*cam_v_op) (struct gspca_dev *);
typedef int (*cam_cf_op) (struct gspca_dev *, const struct usb_device_id *);
typedef int (*cam_get_jpg_op) (struct gspca_dev *,
				struct v4l2_jpegcompression *);
typedef int (*cam_set_jpg_op) (struct gspca_dev *,
				const struct v4l2_jpegcompression *);
typedef int (*cam_get_reg_op) (struct gspca_dev *,
				struct v4l2_dbg_register *);
typedef int (*cam_set_reg_op) (struct gspca_dev *,
				const struct v4l2_dbg_register *);
typedef int (*cam_ident_op) (struct gspca_dev *,
				struct v4l2_dbg_chip_ident *);
typedef void (*cam_streamparm_op) (struct gspca_dev *,
				  struct v4l2_streamparm *);
typedef void (*cam_pkt_op) (struct gspca_dev *gspca_dev,
				u8 *data,
				int len);
typedef int (*cam_int_pkt_op) (struct gspca_dev *gspca_dev,
				u8 *data,
				int len);

/*                       */
struct sd_desc {
/*             */
	const char *name;	/*                 */
/*                      */
	cam_cf_op config;	/*                 */
	cam_op init;		/*                            */
	cam_op init_controls;	/*                 */
	cam_op start;		/*                                         */
	cam_pkt_op pkt_scan;
/*                     */
	cam_op isoc_init;	/*                                           */
	cam_op isoc_nego;	/*                                          */
	cam_v_op stopN;		/*                                 */
	cam_v_op stop0;		/*                                           */
	cam_v_op dq_callback;	/*                                       */
	cam_get_jpg_op get_jcomp;
	cam_set_jpg_op set_jcomp;
	cam_streamparm_op get_streamparm;
	cam_streamparm_op set_streamparm;
#ifdef CONFIG_VIDEO_ADV_DEBUG
	cam_set_reg_op set_register;
	cam_get_reg_op get_register;
#endif
	cam_ident_op get_chip_ident;
#if IS_ENABLED(CONFIG_INPUT)
	cam_int_pkt_op int_pkt_scan;
	/*                                                                   
                                                                     */
	u8 other_input;
#endif
};

/*                                                    */
enum gspca_packet_type {
	DISCARD_PACKET,
	FIRST_PACKET,
	INTER_PACKET,
	LAST_PACKET
};

struct gspca_frame {
	__u8 *data;			/*              */
	int vma_use_count;
	struct v4l2_buffer v4l2_buf;
};

struct gspca_dev {
	struct video_device vdev;	/*                           */
	struct module *module;		/*                               */
	struct v4l2_device v4l2_dev;
	struct usb_device *dev;
	struct file *capt_file;		/*                          */
					/*                         */
#if IS_ENABLED(CONFIG_INPUT)
	struct input_dev *input_dev;
	char phys[64];			/*                      */
#endif

	struct cam cam;				/*                    */
	const struct sd_desc *sd_desc;		/*                       */
	struct v4l2_ctrl_handler ctrl_handler;

	/*                                                                   
                                                                     */
	struct {
		struct v4l2_ctrl *autogain;
		struct v4l2_ctrl *exposure;
		struct v4l2_ctrl *gain;
		int exp_too_low_cnt, exp_too_high_cnt;
	};

#define USB_BUF_SZ 64
	__u8 *usb_buf;				/*                          */
	struct urb *urb[MAX_NURBS];
#if IS_ENABLED(CONFIG_INPUT)
	struct urb *int_urb;
#endif

	__u8 *frbuf;				/*                    */
	struct gspca_frame frame[GSPCA_MAX_FRAMES];
	u8 *image;				/*                     */
	__u32 frsz;				/*            */
	u32 image_len;				/*                         */
	atomic_t fr_q;				/*                     */
	atomic_t fr_i;				/*                    */
	signed char fr_queue[GSPCA_MAX_FRAMES];	/*             */
	char nframes;				/*                  */
	u8 fr_o;				/*                       */
	__u8 last_packet_type;
	__s8 empty_packet;		/*                                   */
	__u8 streaming;			/*                               */

	__u8 curr_mode;			/*                     */
	__u32 pixfmt;			/*                         */
	__u16 width;
	__u16 height;
	__u32 sequence;			/*                       */

	wait_queue_head_t wq;		/*            */
	struct mutex usb_lock;		/*                         */
	struct mutex queue_lock;	/*                       */
	int usb_err;			/*                                   */
	u16 pkt_size;			/*                  */
#ifdef CONFIG_PM
	char frozen;			/*                  */
#endif
	char present;			/*                  */
	char nbufread;			/*                              */
	char memory;			/*                               */
	__u8 iface;			/*                      */
	__u8 alt;			/*                       */
	u8 audio;			/*                          */

	/*                                                                  
                                                                     
                                                             */
};

int gspca_dev_probe(struct usb_interface *intf,
		const struct usb_device_id *id,
		const struct sd_desc *sd_desc,
		int dev_size,
		struct module *module);
int gspca_dev_probe2(struct usb_interface *intf,
		const struct usb_device_id *id,
		const struct sd_desc *sd_desc,
		int dev_size,
		struct module *module);
void gspca_disconnect(struct usb_interface *intf);
void gspca_frame_add(struct gspca_dev *gspca_dev,
			enum gspca_packet_type packet_type,
			const u8 *data,
			int len);
#ifdef CONFIG_PM
int gspca_suspend(struct usb_interface *intf, pm_message_t message);
int gspca_resume(struct usb_interface *intf);
#endif
int gspca_expo_autogain(struct gspca_dev *gspca_dev, int avg_lum,
	int desired_avg_lum, int deadzone, int gain_knee, int exposure_knee);
int gspca_coarse_grained_expo_autogain(struct gspca_dev *gspca_dev,
        int avg_lum, int desired_avg_lum, int deadzone);

#endif /*           */
