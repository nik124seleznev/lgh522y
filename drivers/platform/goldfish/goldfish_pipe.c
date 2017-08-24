/*
 * Copyright (C) 2011 Google, Inc.
 * Copyright (C) 2012 Intel, Inc.
 * Copyright (C) 2013 Intel, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

/*                                                                        
                                                                          
                                      
  
                                                                            
  
                                              
                                              
  
                                                                            
                                                    
  
                                                                     
                          
                                      
                                              
                                                    
                   
       
  
                                                                      
                                                                           
  
                                                                    
                                                                     
                                       
  
                                                                       
                                                 
 */

#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/spinlock.h>
#include <linux/miscdevice.h>
#include <linux/platform_device.h>
#include <linux/poll.h>
#include <linux/sched.h>
#include <linux/bitops.h>
#include <linux/slab.h>
#include <linux/io.h>

/*
                                                                          
                                                                  
 */

/*                       */
#define PIPE_REG_COMMAND		0x00  /*                        */
#define PIPE_REG_STATUS			0x04  /*      */
#define PIPE_REG_CHANNEL		0x08  /*                        */
#define PIPE_REG_SIZE			0x0c  /*                         */
#define PIPE_REG_ADDRESS		0x10  /*                         */
#define PIPE_REG_WAKES			0x14  /*                  */
#define PIPE_REG_PARAMS_ADDR_LOW	0x18  /*                                */
#define PIPE_REG_PARAMS_ADDR_HIGH	0x1c  /*                                */
#define PIPE_REG_ACCESS_PARAMS		0x20  /*                     */

/*                                       */
#define CMD_OPEN			1  /*                  */
#define CMD_CLOSE			2  /*                            */
#define CMD_POLL			3  /*                        */

/*                                                         */
#define PIPE_POLL_IN			(1 << 0)
#define PIPE_POLL_OUT			(1 << 1)
#define PIPE_POLL_HUP			(1 << 2)

/*                                                        */
#define CMD_WRITE_BUFFER	4  /*                                    */
#define CMD_WAKE_ON_WRITE	5  /*                                          
                     */

/*                                                                    
                                                                       
                                                                    
                                       
 */
#define CMD_READ_BUFFER        6  /*                                         */
#define CMD_WAKE_ON_READ       7  /*                                          
                     */

/*                                                                                */
#define PIPE_ERROR_INVAL       -1
#define PIPE_ERROR_AGAIN       -2
#define PIPE_ERROR_NOMEM       -3
#define PIPE_ERROR_IO          -4

/*                                                   */
#define PIPE_WAKE_CLOSED       (1 << 0)  /*                      */
#define PIPE_WAKE_READ         (1 << 1)  /*                           */
#define PIPE_WAKE_WRITE        (1 << 2)  /*                            */

struct access_params {
	u32 channel;
	u32 size;
	u32 address;
	u32 cmd;
	u32 result;
	/*                               */
	u32 flags;
};

/*                                                                  
                                                                    
                        
 */
struct goldfish_pipe_dev {
	spinlock_t lock;
	unsigned char __iomem *base;
	struct access_params *aps;
	int irq;
};

static struct goldfish_pipe_dev   pipe_dev[1];

/*                                             */
struct goldfish_pipe {
	struct goldfish_pipe_dev *dev;
	struct mutex lock;
	unsigned long flags;
	wait_queue_head_t wake_queue;
};


/*                                 */
enum {
	BIT_CLOSED_ON_HOST = 0,  /*                     */
	BIT_WAKE_ON_WRITE  = 1,  /*                            */
	BIT_WAKE_ON_READ   = 2,  /*                           */
};


static u32 goldfish_cmd_status(struct goldfish_pipe *pipe, u32 cmd)
{ 
	unsigned long flags;
	u32 status;
	struct goldfish_pipe_dev *dev = pipe->dev;

	spin_lock_irqsave(&dev->lock, flags);
	writel((u32)pipe, dev->base + PIPE_REG_CHANNEL);
	writel(cmd, dev->base + PIPE_REG_COMMAND);
	status = readl(dev->base + PIPE_REG_STATUS);
	spin_unlock_irqrestore(&dev->lock, flags);
	return status;
}

static void goldfish_cmd(struct goldfish_pipe *pipe, u32 cmd)
{ 
	unsigned long flags;
	struct goldfish_pipe_dev *dev = pipe->dev;

	spin_lock_irqsave(&dev->lock, flags);
	writel((u32)pipe, dev->base + PIPE_REG_CHANNEL);
	writel(cmd, dev->base + PIPE_REG_COMMAND);
	spin_unlock_irqrestore(&dev->lock, flags);
}

/*                                                                      
                                                                      
 */
static int goldfish_pipe_error_convert(int status)
{
	switch (status) {
	case PIPE_ERROR_AGAIN:
		return -EAGAIN;
	case PIPE_ERROR_NOMEM:
		return -ENOMEM;
	case PIPE_ERROR_IO:
		return -EIO;
	default:
		return -EINVAL;
	}
}

/*
                                                                      
                                  
 */
static int valid_batchbuffer_addr(struct goldfish_pipe_dev *dev,
				  struct access_params *aps)
{
	u32 aph, apl;
	u64 paddr;
	aph = readl(dev->base + PIPE_REG_PARAMS_ADDR_HIGH);
	apl = readl(dev->base + PIPE_REG_PARAMS_ADDR_LOW);

	paddr = ((u64)aph << 32) | apl;
	if (paddr != (__pa(aps)))
		return 0;
	return 1;
}

/*              */
static int setup_access_params_addr(struct platform_device *pdev,
					struct goldfish_pipe_dev *dev)
{
	u64 paddr;
	struct access_params *aps;

	aps = devm_kzalloc(&pdev->dev, sizeof(struct access_params), GFP_KERNEL);
	if (!aps)
		return -1;

	/*       */
	paddr = __pa(aps);
	writel((u32)(paddr >> 32), dev->base + PIPE_REG_PARAMS_ADDR_HIGH);
	writel((u32)paddr, dev->base + PIPE_REG_PARAMS_ADDR_LOW);

	if (valid_batchbuffer_addr(dev, aps)) {
		dev->aps = aps;
		return 0;
	} else
		return -1;
}

/*                                               */
#define INITIAL_BATCH_RESULT (0xdeadbeaf)
static int access_with_param(struct goldfish_pipe_dev *dev, const int cmd,
				unsigned long address, unsigned long avail,
				struct goldfish_pipe *pipe, int *status)
{
	struct access_params *aps = dev->aps;

	if (aps == NULL)
		return -1;

	aps->result = INITIAL_BATCH_RESULT;
	aps->channel = (unsigned long)pipe;
	aps->size = avail;
	aps->address = address;
	aps->cmd = cmd;
	writel(cmd, dev->base + PIPE_REG_ACCESS_PARAMS);
	/*
                                                  
                                 
  */
	if (aps->result == INITIAL_BATCH_RESULT)
		return -1;
	*status = aps->result;
	return 0;
}

/*                                                                   
        
 */
static ssize_t goldfish_pipe_read_write(struct file *filp, char __user *buffer,
				    size_t bufflen, int is_write)
{
	unsigned long irq_flags;
	struct goldfish_pipe *pipe = filp->private_data;
	struct goldfish_pipe_dev *dev = pipe->dev;
	const int cmd_offset = is_write ? 0
					: (CMD_READ_BUFFER - CMD_WRITE_BUFFER);
	unsigned long address, address_end;
	int ret = 0;

	/*                                                                */
	if (test_bit(BIT_CLOSED_ON_HOST, &pipe->flags))
		return -EIO;

	/*                               */
	if (unlikely(bufflen) == 0)
		return 0;

	/*                                   */
	if (!access_ok(is_write ? VERIFY_WRITE : VERIFY_READ,
			buffer, bufflen))
		return -EFAULT;

	/*                              */
	if (mutex_lock_interruptible(&pipe->lock))
		return -ERESTARTSYS;

	address = (unsigned long)(void *)buffer;
	address_end = address + bufflen;

	while (address < address_end) {
		unsigned long  page_end = (address & PAGE_MASK) + PAGE_SIZE;
		unsigned long  next     = page_end < address_end ? page_end
								 : address_end;
		unsigned long  avail    = next - address;
		int status, wakeBit;

		/*                                                       */
		/*                                                           */
		if (is_write) {
			char c;
			/*                                             */
			if (__get_user(c, (char __user *)address)) {
				if (!ret)
					ret = -EFAULT;
				break;
			}
		} else {
			/*                                             */
			if (__put_user(0, (char __user *)address)) {
				if (!ret)
					ret = -EFAULT;
				break;
			}
		}

		/*                                                    */
		spin_lock_irqsave(&dev->lock, irq_flags);
		if (access_with_param(dev, CMD_WRITE_BUFFER + cmd_offset,
				address, avail, pipe, &status)) {
			writel((u32)pipe, dev->base + PIPE_REG_CHANNEL);
			writel(avail, dev->base + PIPE_REG_SIZE);
			writel(address, dev->base + PIPE_REG_ADDRESS);
			writel(CMD_WRITE_BUFFER + cmd_offset,
					dev->base + PIPE_REG_COMMAND);
			status = readl(dev->base + PIPE_REG_STATUS);
		}
		spin_unlock_irqrestore(&dev->lock, irq_flags);

		if (status > 0) { /*                  */
			ret += status;
			address += status;
			continue;
		}

		if (status == 0)  /*     */
			break;

		/*                                                       
                                                            
                  */
		if (ret > 0)
			break;

		/*                                                          
                                                  
  */
		if (status != PIPE_ERROR_AGAIN ||
			(filp->f_flags & O_NONBLOCK) != 0) {
			ret = goldfish_pipe_error_convert(status);
			break;
		}

		/*                                                         
                                                               
  */
		wakeBit = is_write ? BIT_WAKE_ON_WRITE : BIT_WAKE_ON_READ;
		set_bit(wakeBit, &pipe->flags);

		/*                                                        */
		goldfish_cmd(pipe, CMD_WAKE_ON_WRITE + cmd_offset);

		/*                                                */
		mutex_unlock(&pipe->lock);

		while (test_bit(wakeBit, &pipe->flags)) {
			if (wait_event_interruptible(
					pipe->wake_queue,
					!test_bit(wakeBit, &pipe->flags)))
				return -ERESTARTSYS;

			if (test_bit(BIT_CLOSED_ON_HOST, &pipe->flags))
				return -EIO;
		}

		/*                            */
		if (mutex_lock_interruptible(&pipe->lock))
			return -ERESTARTSYS;

		/*                        */
		continue;
	}
	mutex_unlock(&pipe->lock);
	return ret;
}

static ssize_t goldfish_pipe_read(struct file *filp, char __user *buffer,
			      size_t bufflen, loff_t *ppos)
{
	return goldfish_pipe_read_write(filp, buffer, bufflen, 0);
}

static ssize_t goldfish_pipe_write(struct file *filp,
				const char __user *buffer, size_t bufflen,
				loff_t *ppos)
{
	return goldfish_pipe_read_write(filp, (char __user *)buffer,
								bufflen, 1);
}


static unsigned int goldfish_pipe_poll(struct file *filp, poll_table *wait)
{
	struct goldfish_pipe *pipe = filp->private_data;
	unsigned int mask = 0;
	int status;

	mutex_lock(&pipe->lock);

	poll_wait(filp, &pipe->wake_queue, wait);

	status = goldfish_cmd_status(pipe, CMD_POLL);

	mutex_unlock(&pipe->lock);

	if (status & PIPE_POLL_IN)
		mask |= POLLIN | POLLRDNORM;

	if (status & PIPE_POLL_OUT)
		mask |= POLLOUT | POLLWRNORM;

	if (status & PIPE_POLL_HUP)
		mask |= POLLHUP;

	if (test_bit(BIT_CLOSED_ON_HOST, &pipe->flags))
		mask |= POLLERR;

	return mask;
}

static irqreturn_t goldfish_pipe_interrupt(int irq, void *dev_id)
{
	struct goldfish_pipe_dev *dev = dev_id;
	unsigned long irq_flags;
	int count = 0;

	/*                                                                
                                                              
                               
 */
	spin_lock_irqsave(&dev->lock, irq_flags);
	for (;;) {
		/*                                                     */
		struct goldfish_pipe *pipe;
		unsigned long wakes;
		unsigned long channel = readl(dev->base + PIPE_REG_CHANNEL);

		if (channel == 0)
			break;

		/*                                                          */
		wakes = readl(dev->base + PIPE_REG_WAKES);
		pipe  = (struct goldfish_pipe *)(ptrdiff_t)channel;

		/*                                      */
		if (wakes & PIPE_WAKE_CLOSED) {
			set_bit(BIT_CLOSED_ON_HOST, &pipe->flags);
			wakes |= PIPE_WAKE_READ | PIPE_WAKE_WRITE;
		}
		if (wakes & PIPE_WAKE_READ)
			clear_bit(BIT_WAKE_ON_READ, &pipe->flags);
		if (wakes & PIPE_WAKE_WRITE)
			clear_bit(BIT_WAKE_ON_WRITE, &pipe->flags);

		wake_up_interruptible(&pipe->wake_queue);
		count++;
	}
	spin_unlock_irqrestore(&dev->lock, irq_flags);

	return (count == 0) ? IRQ_NONE : IRQ_HANDLED;
}

/* 
                                                 
                          
                               
  
                                                                       
                                        
  
                                                                       
                                                                       
 */
static int goldfish_pipe_open(struct inode *inode, struct file *file)
{
	struct goldfish_pipe *pipe;
	struct goldfish_pipe_dev *dev = pipe_dev;
	int32_t status;

	/*                                 */
	pipe = kzalloc(sizeof(*pipe), GFP_KERNEL);
	if (pipe == NULL)
		return -ENOMEM;

	pipe->dev = dev;
	mutex_init(&pipe->lock);
	init_waitqueue_head(&pipe->wake_queue);

	/*
                                                               
                                                                   
  */

	status = goldfish_cmd_status(pipe, CMD_OPEN);
	if (status < 0) {
		kfree(pipe);
		return status;
	}

	/*                                                               */
	file->private_data = pipe;
	return 0;
}

static int goldfish_pipe_release(struct inode *inode, struct file *filp)
{
	struct goldfish_pipe *pipe = filp->private_data;

	/*                                                                  */
	goldfish_cmd(pipe, CMD_CLOSE);
	kfree(pipe);
	filp->private_data = NULL;
	return 0;
}

static const struct file_operations goldfish_pipe_fops = {
	.owner = THIS_MODULE,
	.read = goldfish_pipe_read,
	.write = goldfish_pipe_write,
	.poll = goldfish_pipe_poll,
	.open = goldfish_pipe_open,
	.release = goldfish_pipe_release,
};

static struct miscdevice goldfish_pipe_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "goldfish_pipe",
	.fops = &goldfish_pipe_fops,
};

static int goldfish_pipe_probe(struct platform_device *pdev)
{
	int err;
	struct resource *r;
	struct goldfish_pipe_dev *dev = pipe_dev;

	/*                                             */
	WARN_ON(dev->base != NULL);

	spin_lock_init(&dev->lock);

	r = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (r == NULL || resource_size(r) < PAGE_SIZE) {
		dev_err(&pdev->dev, "can't allocate i/o page\n");
		return -EINVAL;
	}
	dev->base = devm_ioremap(&pdev->dev, r->start, PAGE_SIZE);
	if (dev->base == NULL) {
		dev_err(&pdev->dev, "ioremap failed\n");
		return -EINVAL;
	}

	r = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (r == NULL) {
		err = -EINVAL;
		goto error;
	}
	dev->irq = r->start;

	err = devm_request_irq(&pdev->dev, dev->irq, goldfish_pipe_interrupt,
				IRQF_SHARED, "goldfish_pipe", dev);
	if (err) {
		dev_err(&pdev->dev, "unable to allocate IRQ\n");
		goto error;
	}

	err = misc_register(&goldfish_pipe_device);
	if (err) {
		dev_err(&pdev->dev, "unable to register device\n");
		goto error;
	}
	setup_access_params_addr(pdev, dev);
	return 0;

error:
	dev->base = NULL;
	return err;
}

static int goldfish_pipe_remove(struct platform_device *pdev)
{
	struct goldfish_pipe_dev *dev = pipe_dev;
	misc_deregister(&goldfish_pipe_device);
	dev->base = NULL;
	return 0;
}

static struct platform_driver goldfish_pipe = {
	.probe = goldfish_pipe_probe,
	.remove = goldfish_pipe_remove,
	.driver = {
		.name = "goldfish_pipe"
	}
};

module_platform_driver(goldfish_pipe);
MODULE_AUTHOR("David Turner <digit@google.com>");
MODULE_LICENSE("GPL");
