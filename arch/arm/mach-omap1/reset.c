/*
                      
 */
#include <linux/kernel.h>
#include <linux/io.h>

#include <mach/hardware.h>

#include "iomap.h"
#include "common.h"

/*                                                   */
#define ARM_SYSST_POR_SHIFT				5
#define ARM_SYSST_EXT_RST_SHIFT				4
#define ARM_SYSST_ARM_WDRST_SHIFT			2
#define ARM_SYSST_GLOB_SWRST_SHIFT			1

/*                                                       */
#define OMAP_GLOBAL_COLD_RST_SRC_ID_SHIFT		0
#define OMAP_GLOBAL_WARM_RST_SRC_ID_SHIFT		1
#define OMAP_MPU_WD_RST_SRC_ID_SHIFT			3
#define OMAP_EXTWARM_RST_SRC_ID_SHIFT			5


void omap1_restart(char mode, const char *cmd)
{
	/*
                                                                 
                                                                 
  */
	if (cpu_is_omap5912()) {
		omap_writew(omap_readw(DPLL_CTL) & ~(1 << 4), DPLL_CTL);
		omap_writew(0x8, ARM_RSTCT1);
	}

	omap_writew(1, ARM_RSTCT1);
}

/* 
                                                                      
  
                                                                      
                                                                    
 */
u32 omap1_get_reset_sources(void)
{
	u32 ret = 0;
	u16 rs;

	rs = __raw_readw(OMAP1_IO_ADDRESS(ARM_SYSST));

	if (rs & (1 << ARM_SYSST_POR_SHIFT))
		ret |= 1 << OMAP_GLOBAL_COLD_RST_SRC_ID_SHIFT;
	if (rs & (1 << ARM_SYSST_EXT_RST_SHIFT))
		ret |= 1 << OMAP_EXTWARM_RST_SRC_ID_SHIFT;
	if (rs & (1 << ARM_SYSST_ARM_WDRST_SHIFT))
		ret |= 1 << OMAP_MPU_WD_RST_SRC_ID_SHIFT;
	if (rs & (1 << ARM_SYSST_GLOB_SWRST_SHIFT))
		ret |= 1 << OMAP_GLOBAL_WARM_RST_SRC_ID_SHIFT;

	return ret;
}
