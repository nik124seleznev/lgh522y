/*
 * edac_module.c
 *
 * (C) 2007 www.softwarebitmaker.com
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 *
 * Author: Doug Thompson <dougthompson@xmission.com>
 *
 */
#include <linux/edac.h>

#include "edac_core.h"
#include "edac_module.h"

#define EDAC_VERSION "Ver: 3.0.0"

#ifdef CONFIG_EDAC_DEBUG

static int edac_set_debug_level(const char *buf, struct kernel_param *kp)
{
	unsigned long val;
	int ret;

	ret = kstrtoul(buf, 0, &val);
	if (ret)
		return ret;

	if (val < 0 || val > 4)
		return -EINVAL;

	return param_set_int(buf, kp);
}

/*                                       */
int edac_debug_level = 2;
EXPORT_SYMBOL_GPL(edac_debug_level);

module_param_call(edac_debug_level, edac_set_debug_level, param_get_int,
		  &edac_debug_level, 0644);
MODULE_PARM_DESC(edac_debug_level, "EDAC debug level: [0-4], default: 2");
#endif

/*                               */
struct workqueue_struct *edac_workqueue;

/*
                            
 */
char *edac_op_state_to_string(int opstate)
{
	if (opstate == OP_RUNNING_POLL)
		return "POLLED";
	else if (opstate == OP_RUNNING_INTERRUPT)
		return "INTERRUPT";
	else if (opstate == OP_RUNNING_POLL_INTR)
		return "POLL-INTR";
	else if (opstate == OP_ALLOC)
		return "ALLOC";
	else if (opstate == OP_OFFLINE)
		return "OFFLINE";

	return "UNKNOWN";
}

/*
                       
                                                        
 */
static int edac_workqueue_setup(void)
{
	edac_workqueue = create_singlethread_workqueue("edac-poller");
	if (edac_workqueue == NULL)
		return -ENODEV;
	else
		return 0;
}

/*
                          
                              
 */
static void edac_workqueue_teardown(void)
{
	if (edac_workqueue) {
		flush_workqueue(edac_workqueue);
		destroy_workqueue(edac_workqueue);
		edac_workqueue = NULL;
	}
}

/*
            
                                         
 */
static int __init edac_init(void)
{
	int err = 0;

	edac_printk(KERN_INFO, EDAC_MC, EDAC_VERSION "\n");

	/*
                                                               
   
                                                                       
                                                                   
                                         
  */
	edac_pci_clear_parity_errors();

	err = edac_mc_sysfs_init();
	if (err)
		goto error;

	edac_debugfs_init();

	/*                                          */
	err = edac_workqueue_setup();
	if (err) {
		edac_printk(KERN_ERR, EDAC_MC, "init WorkQueue failure\n");
		goto error;
	}

	return 0;

error:
	return err;
}

/*
              
                                        
 */
static void __exit edac_exit(void)
{
	edac_dbg(0, "\n");

	/*                                  */
	edac_workqueue_teardown();
	edac_mc_sysfs_exit();
	edac_debugfs_exit();
}

/*
                                                 
 */
subsys_initcall(edac_init);
module_exit(edac_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Doug Thompson www.softwarebitmaker.com, et al");
MODULE_DESCRIPTION("Core library routines for EDAC reporting");
