/*
 *  Copyright (C) 1991, 1992  Linus Torvalds
 *
 *  Added support for a Unix98-style ptmx device.
 *    -- C. Scott Ananian <cananian@alumni.princeton.edu>, 14-Jan-1998
 *
 */

#include <linux/module.h>

#include <linux/errno.h>
#include <linux/interrupt.h>
#include <linux/tty.h>
#include <linux/tty_flip.h>
#include <linux/fcntl.h>
#include <linux/sched.h>
#include <linux/string.h>
#include <linux/major.h>
#include <linux/mm.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/bitops.h>
#include <linux/devpts_fs.h>
#include <linux/slab.h>
#include <linux/mutex.h>

#ifdef CONFIG_UNIX98_PTYS
static struct tty_driver *ptm_driver;
static struct tty_driver *pts_driver;
static DEFINE_MUTEX(devpts_mutex);
#endif

static void pty_close(struct tty_struct *tty, struct file *filp)
{
	BUG_ON(!tty);
	if (tty->driver->subtype == PTY_TYPE_MASTER)
		WARN_ON(tty->count > 1);
	else {
		if (test_bit(TTY_IO_ERROR, &tty->flags))
			return;
		if (tty->count > 2)
			return;
	}
	set_bit(TTY_IO_ERROR, &tty->flags);
	wake_up_interruptible(&tty->read_wait);
	wake_up_interruptible(&tty->write_wait);
	tty->packet = 0;
	/*                               */
	if (!tty->link)
		return;
	set_bit(TTY_OTHER_CLOSED, &tty->link->flags);
	wake_up_interruptible(&tty->link->read_wait);
	wake_up_interruptible(&tty->link->write_wait);
	if (tty->driver->subtype == PTY_TYPE_MASTER) {
		set_bit(TTY_OTHER_CLOSED, &tty->flags);
#ifdef CONFIG_UNIX98_PTYS
		if (tty->driver == ptm_driver) {
			mutex_lock(&devpts_mutex);
			if (tty->link->driver_data)
				devpts_pty_kill(tty->link->driver_data);
			mutex_unlock(&devpts_mutex);
		}
#endif
		tty_unlock(tty);
		tty_vhangup(tty->link);
		tty_lock(tty);
	}
}

/*
                                                                    
                                                                     
                                                                      
                                                                        
                                                                   
                                                                   
                                                                          
                                    
 */
static void pty_unthrottle(struct tty_struct *tty)
{
	tty_wakeup(tty->link);
	set_bit(TTY_THROTTLED, &tty->flags);
}

/* 
                                            
                               
  
                                                                    
                                                                  
                      
 */

static int pty_space(struct tty_struct *to)
{
	int n = 8192 - to->port->buf.memory_used;
	if (n < 0)
		return 0;
	return n;
}

/* 
                              
                              
                              
                         
  
                                                                   
                                                                    
                                                                   
                                      
 */

static int pty_write(struct tty_struct *tty, const unsigned char *buf, int c)
{
	struct tty_struct *to = tty->link;

	if (tty->stopped)
		return 0;

	if (c > 0) {
		/*                                                      */
		c = tty_insert_flip_string(to->port, buf, c);
		/*            */
		if (c) {
			tty_flip_buffer_push(to->port);
			tty_wakeup(tty);
		}
	}
	return c;
}

/* 
                               
                                
  
                                                              
                    
 */

static int pty_write_room(struct tty_struct *tty)
{
	if (tty->stopped)
		return 0;
	return pty_space(tty->link);
}

/* 
                                                             
                
  
                                                                  
                                                        
 */

static int pty_chars_in_buffer(struct tty_struct *tty)
{
	return 0;
}

/*                            */
static int pty_set_lock(struct tty_struct *tty, int __user *arg)
{
	int val;
	if (get_user(val, arg))
		return -EFAULT;
	if (val)
		set_bit(TTY_PTY_LOCK, &tty->flags);
	else
		clear_bit(TTY_PTY_LOCK, &tty->flags);
	return 0;
}

static int pty_get_lock(struct tty_struct *tty, int __user *arg)
{
	int locked = test_bit(TTY_PTY_LOCK, &tty->flags);
	return put_user(locked, arg);
}

/*                              */
static int pty_set_pktmode(struct tty_struct *tty, int __user *arg)
{
	unsigned long flags;
	int pktmode;

	if (get_user(pktmode, arg))
		return -EFAULT;

	spin_lock_irqsave(&tty->ctrl_lock, flags);
	if (pktmode) {
		if (!tty->packet) {
			tty->packet = 1;
			tty->link->ctrl_status = 0;
		}
	} else
		tty->packet = 0;
	spin_unlock_irqrestore(&tty->ctrl_lock, flags);

	return 0;
}

/*                              */
static int pty_get_pktmode(struct tty_struct *tty, int __user *arg)
{
	int pktmode = tty->packet;
	return put_user(pktmode, arg);
}

/*                            */
static int pty_signal(struct tty_struct *tty, int sig)
{
	unsigned long flags;
	struct pid *pgrp;

	if (tty->link) {
		spin_lock_irqsave(&tty->link->ctrl_lock, flags);
		pgrp = get_pid(tty->link->pgrp);
		spin_unlock_irqrestore(&tty->link->ctrl_lock, flags);

		kill_pgrp(pgrp, sig, 1);
		put_pid(pgrp);
	}
	return 0;
}

static void pty_flush_buffer(struct tty_struct *tty)
{
	struct tty_struct *to = tty->link;
	unsigned long flags;

	if (!to)
		return;
	/*                             */
	if (to->packet) {
		spin_lock_irqsave(&tty->ctrl_lock, flags);
		tty->ctrl_status |= TIOCPKT_FLUSHWRITE;
		wake_up_interruptible(&to->read_wait);
		spin_unlock_irqrestore(&tty->ctrl_lock, flags);
	}
}

static int pty_open(struct tty_struct *tty, struct file *filp)
{
	if (!tty || !tty->link)
		return -ENODEV;

	if (test_bit(TTY_OTHER_CLOSED, &tty->flags))
		goto out;
	if (test_bit(TTY_PTY_LOCK, &tty->link->flags))
		goto out;
	if (tty->driver->subtype == PTY_TYPE_SLAVE && tty->link->count != 1)
		goto out;

	clear_bit(TTY_IO_ERROR, &tty->flags);
	clear_bit(TTY_OTHER_CLOSED, &tty->link->flags);
	set_bit(TTY_THROTTLED, &tty->flags);
	return 0;

out:
	set_bit(TTY_IO_ERROR, &tty->flags);
	return -EIO;
}

static void pty_set_termios(struct tty_struct *tty,
					struct ktermios *old_termios)
{
	tty->termios.c_cflag &= ~(CSIZE | PARENB);
	tty->termios.c_cflag |= (CS8 | CREAD);
}

/* 
                                
                          
                              
  
                                                                 
                                     
 */

static int pty_resize(struct tty_struct *tty,  struct winsize *ws)
{
	struct pid *pgrp, *rpgrp;
	unsigned long flags;
	struct tty_struct *pty = tty->link;

	/*                                        */
	mutex_lock(&tty->termios_mutex);
	if (!memcmp(ws, &tty->winsize, sizeof(*ws)))
		goto done;

	/*                                                
                                                          
                                                */

	spin_lock_irqsave(&tty->ctrl_lock, flags);
	pgrp = get_pid(tty->pgrp);
	spin_unlock_irqrestore(&tty->ctrl_lock, flags);

	spin_lock_irqsave(&pty->ctrl_lock, flags);
	rpgrp = get_pid(pty->pgrp);
	spin_unlock_irqrestore(&pty->ctrl_lock, flags);

	if (pgrp)
		kill_pgrp(pgrp, SIGWINCH, 1);
	if (rpgrp != pgrp && rpgrp)
		kill_pgrp(rpgrp, SIGWINCH, 1);

	put_pid(pgrp);
	put_pid(rpgrp);

	tty->winsize = *ws;
	pty->winsize = *ws;	/*                                 */
done:
	mutex_unlock(&tty->termios_mutex);
	return 0;
}

/**
 *	pty_common_install		-	set up the pty pair
 *	@driver: the pty driver
 *	@tty: the tty being instantiated
 *	@bool: legacy, true if this is BSD style
 *
 *	Perform the initial set up for the tty/pty pair. Called from the
 *	tty layer when the port is first opened.
 *
 *	Locking: the caller must hold the tty_mutex
 */
static int pty_common_install(struct tty_driver *driver, struct tty_struct *tty,
		bool legacy)
{
	struct tty_struct *o_tty;
	struct tty_port *ports[2];
	int idx = tty->index;
	int retval = -ENOMEM;

	o_tty = alloc_tty_struct();
	if (!o_tty)
		goto err;
	ports[0] = kmalloc(sizeof **ports, GFP_KERNEL);
	ports[1] = kmalloc(sizeof **ports, GFP_KERNEL);
	if (!ports[0] || !ports[1])
		goto err_free_tty;
	if (!try_module_get(driver->other->owner)) {
		/*                                      */
		goto err_free_tty;
	}
	initialize_tty_struct(o_tty, driver->other, idx);

	if (legacy) {
		/*                                                     
                     */
		retval = tty_init_termios(tty);
		if (retval)
			goto err_deinit_tty;

		retval = tty_init_termios(o_tty);
		if (retval)
			goto err_free_termios;

		driver->other->ttys[idx] = o_tty;
		driver->ttys[idx] = tty;
	} else {
		memset(&tty->termios_locked, 0, sizeof(tty->termios_locked));
		tty->termios = driver->init_termios;
		memset(&o_tty->termios_locked, 0, sizeof(tty->termios_locked));
		o_tty->termios = driver->other->init_termios;
	}

	/*
                                                        
  */
	tty_driver_kref_get(driver->other);
	if (driver->subtype == PTY_TYPE_MASTER)
		o_tty->count++;
	/*                                        */
	tty->link   = o_tty;
	o_tty->link = tty;
	tty_port_init(ports[0]);
	tty_port_init(ports[1]);
	o_tty->port = ports[0];
	tty->port = ports[1];
	o_tty->port->itty = o_tty;

	tty_driver_kref_get(driver);
	tty->count++;
	return 0;
err_free_termios:
	if (legacy)
		tty_free_termios(tty);
err_deinit_tty:
	deinitialize_tty_struct(o_tty);
	module_put(o_tty->driver->owner);
err_free_tty:
	kfree(ports[0]);
	kfree(ports[1]);
	free_tty_struct(o_tty);
err:
	return retval;
}

static void pty_cleanup(struct tty_struct *tty)
{
	tty_port_put(tty->port);
}

/* Traditional BSD devices */
#ifdef CONFIG_LEGACY_PTYS

static int pty_install(struct tty_driver *driver, struct tty_struct *tty)
{
	return pty_common_install(driver, tty, true);
}

static void pty_remove(struct tty_driver *driver, struct tty_struct *tty)
{
	struct tty_struct *pair = tty->link;
	driver->ttys[tty->index] = NULL;
	if (pair)
		pair->driver->ttys[pair->index] = NULL;
}

static int pty_bsd_ioctl(struct tty_struct *tty,
			 unsigned int cmd, unsigned long arg)
{
	switch (cmd) {
	case TIOCSPTLCK: /*                                   */
		return pty_set_lock(tty, (int __user *) arg);
	case TIOCGPTLCK: /*                    */
		return pty_get_lock(tty, (int __user *)arg);
	case TIOCPKT: /*                    */
		return pty_set_pktmode(tty, (int __user *)arg);
	case TIOCGPKT: /*                    */
		return pty_get_pktmode(tty, (int __user *)arg);
	case TIOCSIG:    /*                                  */
		return pty_signal(tty, (int) arg);
	case TIOCGPTN: /*                                                   */
		return -EINVAL;
	}
	return -ENOIOCTLCMD;
}

static int legacy_count = CONFIG_LEGACY_PTY_COUNT;
module_param(legacy_count, int, 0);

/*
 * The master side of a pty can do TIOCSPTLCK and thus
 * has pty_bsd_ioctl.
 */
static const struct tty_operations master_pty_ops_bsd = {
	.install = pty_install,
	.open = pty_open,
	.close = pty_close,
	.write = pty_write,
	.write_room = pty_write_room,
	.flush_buffer = pty_flush_buffer,
	.chars_in_buffer = pty_chars_in_buffer,
	.unthrottle = pty_unthrottle,
	.set_termios = pty_set_termios,
	.ioctl = pty_bsd_ioctl,
	.cleanup = pty_cleanup,
	.resize = pty_resize,
	.remove = pty_remove
};

static const struct tty_operations slave_pty_ops_bsd = {
	.install = pty_install,
	.open = pty_open,
	.close = pty_close,
	.write = pty_write,
	.write_room = pty_write_room,
	.flush_buffer = pty_flush_buffer,
	.chars_in_buffer = pty_chars_in_buffer,
	.unthrottle = pty_unthrottle,
	.set_termios = pty_set_termios,
	.cleanup = pty_cleanup,
	.resize = pty_resize,
	.remove = pty_remove
};

static void __init legacy_pty_init(void)
{
	struct tty_driver *pty_driver, *pty_slave_driver;

	if (legacy_count <= 0)
		return;

	pty_driver = tty_alloc_driver(legacy_count,
			TTY_DRIVER_RESET_TERMIOS |
			TTY_DRIVER_REAL_RAW |
			TTY_DRIVER_DYNAMIC_ALLOC);
	if (IS_ERR(pty_driver))
		panic("Couldn't allocate pty driver");

	pty_slave_driver = tty_alloc_driver(legacy_count,
			TTY_DRIVER_RESET_TERMIOS |
			TTY_DRIVER_REAL_RAW |
			TTY_DRIVER_DYNAMIC_ALLOC);
	if (IS_ERR(pty_slave_driver))
		panic("Couldn't allocate pty slave driver");

	pty_driver->driver_name = "pty_master";
	pty_driver->name = "pty";
	pty_driver->major = PTY_MASTER_MAJOR;
	pty_driver->minor_start = 0;
	pty_driver->type = TTY_DRIVER_TYPE_PTY;
	pty_driver->subtype = PTY_TYPE_MASTER;
	pty_driver->init_termios = tty_std_termios;
	pty_driver->init_termios.c_iflag = 0;
	pty_driver->init_termios.c_oflag = 0;
	pty_driver->init_termios.c_cflag = B38400 | CS8 | CREAD;
	pty_driver->init_termios.c_lflag = 0;
	pty_driver->init_termios.c_ispeed = 38400;
	pty_driver->init_termios.c_ospeed = 38400;
	pty_driver->other = pty_slave_driver;
	tty_set_operations(pty_driver, &master_pty_ops_bsd);

	pty_slave_driver->driver_name = "pty_slave";
	pty_slave_driver->name = "ttyp";
	pty_slave_driver->major = PTY_SLAVE_MAJOR;
	pty_slave_driver->minor_start = 0;
	pty_slave_driver->type = TTY_DRIVER_TYPE_PTY;
	pty_slave_driver->subtype = PTY_TYPE_SLAVE;
	pty_slave_driver->init_termios = tty_std_termios;
	pty_slave_driver->init_termios.c_cflag = B38400 | CS8 | CREAD;
	pty_slave_driver->init_termios.c_ispeed = 38400;
	pty_slave_driver->init_termios.c_ospeed = 38400;
	pty_slave_driver->other = pty_driver;
	tty_set_operations(pty_slave_driver, &slave_pty_ops_bsd);

	if (tty_register_driver(pty_driver))
		panic("Couldn't register pty driver");
	if (tty_register_driver(pty_slave_driver))
		panic("Couldn't register pty slave driver");
}
#else
static inline void legacy_pty_init(void) { }
#endif

/*                */
#ifdef CONFIG_UNIX98_PTYS

static struct cdev ptmx_cdev;

static int pty_unix98_ioctl(struct tty_struct *tty,
			    unsigned int cmd, unsigned long arg)
{
	switch (cmd) {
	case TIOCSPTLCK: /*                                   */
		return pty_set_lock(tty, (int __user *)arg);
	case TIOCGPTLCK: /*                    */
		return pty_get_lock(tty, (int __user *)arg);
	case TIOCPKT: /*                    */
		return pty_set_pktmode(tty, (int __user *)arg);
	case TIOCGPKT: /*                    */
		return pty_get_pktmode(tty, (int __user *)arg);
	case TIOCGPTN: /*               */
		return put_user(tty->index, (unsigned int __user *)arg);
	case TIOCSIG:    /*                                  */
		return pty_signal(tty, (int) arg);
	}

	return -ENOIOCTLCMD;
}

/* 
                                        
                      
                  
  
                                                                   
                             
 */

static struct tty_struct *ptm_unix98_lookup(struct tty_driver *driver,
		struct inode *ptm_inode, int idx)
{
	/*                                   */
	return ERR_PTR(-EIO);
}

/* 
                                       
                      
                  
  
                                                                   
                                                 
 */

static struct tty_struct *pts_unix98_lookup(struct tty_driver *driver,
		struct inode *pts_inode, int idx)
{
	struct tty_struct *tty;

	mutex_lock(&devpts_mutex);
	tty = devpts_get_priv(pts_inode);
	mutex_unlock(&devpts_mutex);
	/*                                  */
	if (!tty)
		return ERR_PTR(-EIO);
	return tty;
}

/*                                                                         
                   */

static int pty_unix98_install(struct tty_driver *driver, struct tty_struct *tty)
{
	return pty_common_install(driver, tty, false);
}

static void pty_unix98_remove(struct tty_driver *driver, struct tty_struct *tty)
{
}

/*                                                       */
static void pty_unix98_shutdown(struct tty_struct *tty)
{
	devpts_kill_index(tty->driver_data, tty->index);
}

static const struct tty_operations ptm_unix98_ops = {
	.lookup = ptm_unix98_lookup,
	.install = pty_unix98_install,
	.remove = pty_unix98_remove,
	.open = pty_open,
	.close = pty_close,
	.write = pty_write,
	.write_room = pty_write_room,
	.flush_buffer = pty_flush_buffer,
	.chars_in_buffer = pty_chars_in_buffer,
	.unthrottle = pty_unthrottle,
	.set_termios = pty_set_termios,
	.ioctl = pty_unix98_ioctl,
	.resize = pty_resize,
	.shutdown = pty_unix98_shutdown,
	.cleanup = pty_cleanup
};

static const struct tty_operations pty_unix98_ops = {
	.lookup = pts_unix98_lookup,
	.install = pty_unix98_install,
	.remove = pty_unix98_remove,
	.open = pty_open,
	.close = pty_close,
	.write = pty_write,
	.write_room = pty_write_room,
	.flush_buffer = pty_flush_buffer,
	.chars_in_buffer = pty_chars_in_buffer,
	.unthrottle = pty_unthrottle,
	.set_termios = pty_set_termios,
	.shutdown = pty_unix98_shutdown,
	.cleanup = pty_cleanup,
};

/* 
                                         
                               
                             
  
                                                            
  
                                                                   
                     
                                                            
 */

static int ptmx_open(struct inode *inode, struct file *filp)
{
	struct tty_struct *tty;
	struct inode *slave_inode;
	int retval;
	int index;

	nonseekable_open(inode, filp);

	/*                                                                 */
	filp->f_mode |= FMODE_NONOTIFY;

	retval = tty_alloc_file(filp);
	if (retval)
		return retval;

	/*                                   */
	mutex_lock(&devpts_mutex);
	index = devpts_new_index(inode);
	if (index < 0) {
		retval = index;
		mutex_unlock(&devpts_mutex);
		goto err_file;
	}

	mutex_unlock(&devpts_mutex);

	mutex_lock(&tty_mutex);
	tty = tty_init_dev(ptm_driver, index);

	if (IS_ERR(tty)) {
		retval = PTR_ERR(tty);
		goto out;
	}

	/*                                                 
                   */
	mutex_unlock(&tty_mutex);

	set_bit(TTY_PTY_LOCK, &tty->flags); /*                */
	tty->driver_data = inode;

	tty_add_file(tty, filp);

	slave_inode = devpts_pty_new(inode,
			MKDEV(UNIX98_PTY_SLAVE_MAJOR, index), index,
			tty->link);
	if (IS_ERR(slave_inode)) {
		retval = PTR_ERR(slave_inode);
		goto err_release;
	}
	tty->link->driver_data = slave_inode;

	retval = ptm_driver->ops->open(tty, filp);
	if (retval)
		goto err_release;

	tty_unlock(tty);
	return 0;
err_release:
	tty_unlock(tty);
	tty_release(inode, filp);
	return retval;
out:
	mutex_unlock(&tty_mutex);
	devpts_kill_index(inode, index);
err_file:
	tty_free_file(filp);
	return retval;
}

static struct file_operations ptmx_fops;

static void __init unix98_pty_init(void)
{
	ptm_driver = tty_alloc_driver(NR_UNIX98_PTY_MAX,
			TTY_DRIVER_RESET_TERMIOS |
			TTY_DRIVER_REAL_RAW |
			TTY_DRIVER_DYNAMIC_DEV |
			TTY_DRIVER_DEVPTS_MEM |
			TTY_DRIVER_DYNAMIC_ALLOC);
	if (IS_ERR(ptm_driver))
		panic("Couldn't allocate Unix98 ptm driver");
	pts_driver = tty_alloc_driver(NR_UNIX98_PTY_MAX,
			TTY_DRIVER_RESET_TERMIOS |
			TTY_DRIVER_REAL_RAW |
			TTY_DRIVER_DYNAMIC_DEV |
			TTY_DRIVER_DEVPTS_MEM |
			TTY_DRIVER_DYNAMIC_ALLOC);
	if (IS_ERR(pts_driver))
		panic("Couldn't allocate Unix98 pts driver");

	ptm_driver->driver_name = "pty_master";
	ptm_driver->name = "ptm";
	ptm_driver->major = UNIX98_PTY_MASTER_MAJOR;
	ptm_driver->minor_start = 0;
	ptm_driver->type = TTY_DRIVER_TYPE_PTY;
	ptm_driver->subtype = PTY_TYPE_MASTER;
	ptm_driver->init_termios = tty_std_termios;
	ptm_driver->init_termios.c_iflag = 0;
	ptm_driver->init_termios.c_oflag = 0;
	ptm_driver->init_termios.c_cflag = B38400 | CS8 | CREAD;
	ptm_driver->init_termios.c_lflag = 0;
	ptm_driver->init_termios.c_ispeed = 38400;
	ptm_driver->init_termios.c_ospeed = 38400;
	ptm_driver->other = pts_driver;
	tty_set_operations(ptm_driver, &ptm_unix98_ops);

	pts_driver->driver_name = "pty_slave";
	pts_driver->name = "pts";
	pts_driver->major = UNIX98_PTY_SLAVE_MAJOR;
	pts_driver->minor_start = 0;
	pts_driver->type = TTY_DRIVER_TYPE_PTY;
	pts_driver->subtype = PTY_TYPE_SLAVE;
	pts_driver->init_termios = tty_std_termios;
	pts_driver->init_termios.c_cflag = B38400 | CS8 | CREAD;
	pts_driver->init_termios.c_ispeed = 38400;
	pts_driver->init_termios.c_ospeed = 38400;
	pts_driver->other = ptm_driver;
	tty_set_operations(pts_driver, &pty_unix98_ops);

	if (tty_register_driver(ptm_driver))
		panic("Couldn't register Unix98 ptm driver");
	if (tty_register_driver(pts_driver))
		panic("Couldn't register Unix98 pts driver");

	/*                                         */
	tty_default_fops(&ptmx_fops);
	ptmx_fops.open = ptmx_open;

	cdev_init(&ptmx_cdev, &ptmx_fops);
	if (cdev_add(&ptmx_cdev, MKDEV(TTYAUX_MAJOR, 2), 1) ||
	    register_chrdev_region(MKDEV(TTYAUX_MAJOR, 2), 1, "/dev/ptmx") < 0)
		panic("Couldn't register /dev/ptmx driver");
	device_create(tty_class, NULL, MKDEV(TTYAUX_MAJOR, 2), NULL, "ptmx");
}

#else
static inline void unix98_pty_init(void) { }
#endif

static int __init pty_init(void)
{
	legacy_pty_init();
	unix98_pty_init();
	return 0;
}
module_init(pty_init);
