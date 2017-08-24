/*
 *  linux/drivers/char/ttyprintk.c
 *
 *  Copyright (C) 2010  Samo Pogacnik
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the smems of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 */

/*
                                                                         
                                                                              
                                    
 */

#include <linux/device.h>
#include <linux/serial.h>
#include <linux/tty.h>
#include <linux/export.h>

struct ttyprintk_port {
	struct tty_port port;
	struct mutex port_write_mutex;
};

static struct ttyprintk_port tpk_port;

/*
                                                                         
                                                       
                             
                                                      
                                                                
                                                                     
                                                   
 */
#define TPK_STR_SIZE 508 /*                                                */
#define TPK_MAX_ROOM 4096 /*                                 */
static const char *tpk_tag = "[U] "; /*            */
static int tpk_curr;

static int tpk_printk(const unsigned char *buf, int count)
{
	static char tmp[TPK_STR_SIZE + 4];
	int i = tpk_curr;

	if (buf == NULL) {
		/*             */
		if (tpk_curr > 0) {
			/*                                          */
			tmp[tpk_curr + 0] = '\n';
			tmp[tpk_curr + 1] = '\0';
			printk(KERN_INFO "%s%s", tpk_tag, tmp);
			tpk_curr = 0;
		}
		return i;
	}

	for (i = 0; i < count; i++) {
		tmp[tpk_curr] = buf[i];
		if (tpk_curr < TPK_STR_SIZE) {
			switch (buf[i]) {
			case '\r':
				/*                    */
				tmp[tpk_curr + 0] = '\n';
				tmp[tpk_curr + 1] = '\0';
				printk(KERN_INFO "%s%s", tpk_tag, tmp);
				tpk_curr = 0;
				if ((i + 1) < count && buf[i + 1] == '\n')
					i++;
				break;
			case '\n':
				tmp[tpk_curr + 1] = '\0';
				printk(KERN_INFO "%s%s", tpk_tag, tmp);
				tpk_curr = 0;
				break;
			default:
				tpk_curr++;
			}
		} else {
			/*                                                   */
			tmp[tpk_curr + 1] = '\\';
			tmp[tpk_curr + 2] = '\n';
			tmp[tpk_curr + 3] = '\0';
			printk(KERN_INFO "%s%s", tpk_tag, tmp);
			tpk_curr = 0;
		}
	}

	return count;
}

/*
                                
 */
static int tpk_open(struct tty_struct *tty, struct file *filp)
{
	tty->driver_data = &tpk_port;

	return tty_port_open(&tpk_port.port, tty, filp);
}

/*
                                 
 */
static void tpk_close(struct tty_struct *tty, struct file *filp)
{
	struct ttyprintk_port *tpkp = tty->driver_data;

	mutex_lock(&tpkp->port_write_mutex);
	/*                         */
	tpk_printk(NULL, 0);
	mutex_unlock(&tpkp->port_write_mutex);

	tty_port_close(&tpkp->port, tty, filp);
}

/*
                                 
 */
static int tpk_write(struct tty_struct *tty,
		const unsigned char *buf, int count)
{
	struct ttyprintk_port *tpkp = tty->driver_data;
	int ret;


	/*                                             */
	mutex_lock(&tpkp->port_write_mutex);
	ret = tpk_printk(buf, count);
	mutex_unlock(&tpkp->port_write_mutex);

	return ret;
}

/*
                                      
 */
static int tpk_write_room(struct tty_struct *tty)
{
	return TPK_MAX_ROOM;
}

/*
                                 
 */
static int tpk_ioctl(struct tty_struct *tty,
			unsigned int cmd, unsigned long arg)
{
	struct ttyprintk_port *tpkp = tty->driver_data;

	if (!tpkp)
		return -EINVAL;

	switch (cmd) {
	/*               */
	case TIOCCONS:
		return -EOPNOTSUPP;
	default:
		return -ENOIOCTLCMD;
	}
	return 0;
}

static const struct tty_operations ttyprintk_ops = {
	.open = tpk_open,
	.close = tpk_close,
	.write = tpk_write,
	.write_room = tpk_write_room,
	.ioctl = tpk_ioctl,
};

static struct tty_port_operations null_ops = { };

static struct tty_driver *ttyprintk_driver;

static int __init ttyprintk_init(void)
{
	int ret = -ENOMEM;

	mutex_init(&tpk_port.port_write_mutex);

	ttyprintk_driver = tty_alloc_driver(1,
			TTY_DRIVER_RESET_TERMIOS |
			TTY_DRIVER_REAL_RAW |
			TTY_DRIVER_UNNUMBERED_NODE);
	if (IS_ERR(ttyprintk_driver))
		return PTR_ERR(ttyprintk_driver);

	tty_port_init(&tpk_port.port);
	tpk_port.port.ops = &null_ops;

	ttyprintk_driver->driver_name = "ttyprintk";
	ttyprintk_driver->name = "ttyprintk";
	ttyprintk_driver->major = TTYAUX_MAJOR;
	ttyprintk_driver->minor_start = 3;
	ttyprintk_driver->type = TTY_DRIVER_TYPE_CONSOLE;
	ttyprintk_driver->init_termios = tty_std_termios;
	ttyprintk_driver->init_termios.c_oflag = OPOST | OCRNL | ONOCR | ONLRET;
	tty_set_operations(ttyprintk_driver, &ttyprintk_ops);
	tty_port_link_device(&tpk_port.port, ttyprintk_driver, 0);

	ret = tty_register_driver(ttyprintk_driver);
	if (ret < 0) {
		printk(KERN_ERR "Couldn't register ttyprintk driver\n");
		goto error;
	}

	return 0;

error:
	tty_unregister_driver(ttyprintk_driver);
	put_tty_driver(ttyprintk_driver);
	tty_port_destroy(&tpk_port.port);
	ttyprintk_driver = NULL;
	return ret;
}
module_init(ttyprintk_init);
