/*
 * Samsung S5P/Exynos4 SoC series camera interface driver header
 *
 * Copyright (C) 2010 - 2013 Samsung Electronics Co., Ltd.
 * Sylwester Nawrocki <s.nawrocki@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef S5P_FIMC_H_
#define S5P_FIMC_H_

#include <media/media-entity.h>
#include <media/v4l2-mediabus.h>

/*
                                                      
 */
enum fimc_input {
	FIMC_INPUT_PARALLEL_0	= 1,
	FIMC_INPUT_PARALLEL_1,
	FIMC_INPUT_MIPI_CSI2_0	= 3,
	FIMC_INPUT_MIPI_CSI2_1,
	FIMC_INPUT_WRITEBACK_A	= 5,
	FIMC_INPUT_WRITEBACK_B,
	FIMC_INPUT_WRITEBACK_ISP = 5,
};

/*
                                          
 */
enum fimc_bus_type {
	/*                     */
	FIMC_BUS_TYPE_ITU_601 = 1,
	/*                                                   */
	FIMC_BUS_TYPE_ITU_656,
	/*                             */
	FIMC_BUS_TYPE_MIPI_CSI2,
	/*                                             */
	FIMC_BUS_TYPE_LCD_WRITEBACK_A,
	/*                                             */
	FIMC_BUS_TYPE_LCD_WRITEBACK_B,
	/*                        */
	FIMC_BUS_TYPE_ISP_WRITEBACK = FIMC_BUS_TYPE_LCD_WRITEBACK_B,
};

#define fimc_input_is_parallel(x) ((x) == 1 || (x) == 2)
#define fimc_input_is_mipi_csi(x) ((x) == 3 || (x) == 4)

/*
                             
 */
#define GRP_ID_SENSOR		(1 << 8)
#define GRP_ID_FIMC_IS_SENSOR	(1 << 9)
#define GRP_ID_WRITEBACK	(1 << 10)
#define GRP_ID_CSIS		(1 << 11)
#define GRP_ID_FIMC		(1 << 12)
#define GRP_ID_FLITE		(1 << 13)
#define GRP_ID_FIMC_IS		(1 << 14)

struct i2c_board_info;

/* 
                                                                           
                                 
  
                                                     
                                                                               
                                         
                                                                   
                                                                                
                                                             
                                                                               
                                                         
 */
struct fimc_source_info {
	struct i2c_board_info *board_info;
	unsigned long clk_frequency;
	enum fimc_bus_type fimc_bus_type;
	enum fimc_bus_type sensor_bus_type;
	u16 flags;
	u16 i2c_bus_num;
	u16 mux_id;
	u8 clk_id;
};

/* 
                                                                 
  
                                                                           
                                                     
 */
struct s5p_platform_fimc {
	struct fimc_source_info *source_info;
	int num_clients;
};

/*
                                                                            
                                                                            
                                                                             
                                         
 */
#define S5P_FIMC_TX_END_NOTIFY _IO('e', 0)

#define FIMC_MAX_PLANES	3

/* 
                                                
                                                         
                            
                                                            
                                               
                                                              
                                                          
                                                                
                                                                       
                                                                  
 */
struct fimc_fmt {
	enum v4l2_mbus_pixelcode mbus_code;
	char	*name;
	u32	fourcc;
	u32	color;
	u16	memplanes;
	u16	colplanes;
	u8	depth[FIMC_MAX_PLANES];
	u16	mdataplanes;
	u16	flags;
#define FMT_FLAGS_CAM		(1 << 0)
#define FMT_FLAGS_M2M_IN	(1 << 1)
#define FMT_FLAGS_M2M_OUT	(1 << 2)
#define FMT_FLAGS_M2M		(1 << 1 | 1 << 2)
#define FMT_HAS_ALPHA		(1 << 3)
#define FMT_FLAGS_COMPRESSED	(1 << 4)
#define FMT_FLAGS_WRITEBACK	(1 << 5)
#define FMT_FLAGS_RAW_BAYER	(1 << 6)
#define FMT_FLAGS_YUV		(1 << 7)
};

enum fimc_subdev_index {
	IDX_SENSOR,
	IDX_CSIS,
	IDX_FLITE,
	IDX_IS_ISP,
	IDX_FIMC,
	IDX_MAX,
};

struct media_pipeline;
struct v4l2_subdev;

struct fimc_pipeline {
	struct v4l2_subdev *subdevs[IDX_MAX];
	struct media_pipeline *m_pipeline;
};

/*
                                                                     
                                                                     
                                        
 */
struct fimc_pipeline_ops {
	int (*open)(struct fimc_pipeline *p, struct media_entity *me,
			  bool resume);
	int (*close)(struct fimc_pipeline *p);
	int (*set_stream)(struct fimc_pipeline *p, bool state);
};

#define fimc_pipeline_call(f, op, p, args...)				\
	(!(f) ? -ENODEV : (((f)->pipeline_ops && (f)->pipeline_ops->op) ? \
			    (f)->pipeline_ops->op((p), ##args) : -ENOIOCTLCMD))

#endif /*             */
