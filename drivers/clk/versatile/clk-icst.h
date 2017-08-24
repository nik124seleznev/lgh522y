#include <asm/hardware/icst.h>

/* 
                                                     
                           
                                                                    
                                                                          
              
 */
struct clk_icst_desc {
	const struct icst_params *params;
	u32 vco_offset;
	u32 lock_offset;
};

struct clk *icst_clk_register(struct device *dev,
			      const struct clk_icst_desc *desc,
			      void __iomem *base);
