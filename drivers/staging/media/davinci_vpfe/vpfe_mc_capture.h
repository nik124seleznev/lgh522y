/*
 * Copyright (C) 2012 Texas Instruments Inc
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 * Contributors:
 *      Manjunath Hadli <manjunath.hadli@ti.com>
 *      Prabhakar Lad <prabhakar.lad@ti.com>
 */

#ifndef _DAVINCI_VPFE_MC_CAPTURE_H
#define _DAVINCI_VPFE_MC_CAPTURE_H

#include "dm365_ipipe.h"
#include "dm365_ipipeif.h"
#include "dm365_isif.h"
#include "dm365_resizer.h"
#include "vpfe_video.h"

#define VPFE_MAJOR_RELEASE		0
#define VPFE_MINOR_RELEASE		0
#define VPFE_BUILD			1
#define VPFE_CAPTURE_VERSION_CODE       ((VPFE_MAJOR_RELEASE << 16) | \
					(VPFE_MINOR_RELEASE << 8)  | \
					VPFE_BUILD)

/*                       */
#define IPIPE_MAX_OUTPUT_WIDTH_A	2176
#define IPIPE_MAX_OUTPUT_WIDTH_B	640

/*                                         */
#define IPIPE_MAX_OUTPUT_HEIGHT_A	1536
/*                                        */
#define IPIPE_MAX_OUTPUT_HEIGHT_B	480

#define to_vpfe_device(ptr_module)				\
	container_of(ptr_module, struct vpfe_device, vpfe_##ptr_module)
#define to_device(ptr_module)						\
	(to_vpfe_device(ptr_module)->dev)

struct vpfe_device {
	/*                                 */
	struct v4l2_subdev		**sd;
	/*                                       */
	unsigned int			num_ext_subdevs;
	/*          */
	struct vpfe_config		*cfg;
	/*                             */
	struct clk			**clks;
	/*             */
	struct v4l2_device		v4l2_dev;
	/*               */
	struct device			*pdev;
	/*                                                               */
	unsigned int			imp_dma_irq;
	/*                                               */
	unsigned int			ccdc_irq0;
	unsigned int			ccdc_irq1;
	/*                                       */
	unsigned int			video_limit;
	/*              */
	struct media_device		media_dev;
	/*                */
	struct vpfe_isif_device		vpfe_isif;
	/*                   */
	struct vpfe_ipipeif_device	vpfe_ipipeif;
	/*                 */
	struct vpfe_ipipe_device	vpfe_ipipe;
	/*                   */
	struct vpfe_resizer_device	vpfe_resizer;
};

/*                       */
struct vpfe_fh {
	struct v4l2_fh vfh;
	struct vpfe_video_device *video;
	/*                                                */
	u8 io_allowed;
	/*                                              */
	enum v4l2_priority prio;
};

void mbus_to_pix(const struct v4l2_mbus_framefmt *mbus,
			   struct v4l2_pix_format *pix);

#endif		/*                            */
