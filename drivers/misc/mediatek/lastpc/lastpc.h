#ifndef __LASTPC_H__
#define __LASTPC_H__

#include <linux/platform_device.h>
#include <linux/pm.h>
#include <linux/compiler.h>

struct lastpc_plt;

struct lastpc_plt_operations {
	/*                                           */
	int (*start)(struct lastpc_plt *plt);
	/*                      */
	int (*dump)(struct lastpc_plt *plt, char *buf, int len);
	/*                                                                 */
	int (*reboot_test)(struct lastpc_plt *plt);
	/*                                                              */
	int (*probe)(struct lastpc_plt *plt, struct platform_device *pdev);
	/*                                                               */
	int (*remove)(struct lastpc_plt *plt, struct platform_device *pdev);
	/*                                                                */
	int (*suspend)(struct lastpc_plt *plt, struct platform_device *pdev, pm_message_t state);
	/*                                                               */
	int (*resume)(struct lastpc_plt *plt, struct platform_device *pdev);
};

struct lastpc_plt {
	unsigned int chip_code;
	unsigned int min_buf_len;
	struct lastpc_plt_operations *ops;
	struct lastpc *common;
};

struct lastpc {
	struct platform_driver plt_drv;
	void __iomem *base;
	struct lastpc_plt *cur_plt;
};

/*                                                      
                                       
*/
int lastpc_register(struct lastpc_plt *plt);

#endif //                   
