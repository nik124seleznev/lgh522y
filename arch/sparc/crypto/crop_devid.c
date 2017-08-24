#include <linux/module.h>
#include <linux/of_device.h>

/*                                                           
                                                               
                                                            
                                                        
                         
 */
static const struct of_device_id crypto_opcode_match[] = {
	{ .name = "cpu", .compatible = "sun4v", },
	{},
};
MODULE_DEVICE_TABLE(of, crypto_opcode_match);
